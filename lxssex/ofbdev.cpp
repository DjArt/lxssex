#include "default.h"
#include "ofbdev.h"

NTSTATUS OfbEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    PDEVICE_OBJECT      deviceObject;
    PDEVICE_EXTENSION   deviceExtension;
    UNICODE_STRING      ntDeviceName;
    UNICODE_STRING      symbolicLinkName;
    NTSTATUS            status;

    //
    // Create the device object
    //
    RtlInitUnicodeString(&ntDeviceName, NTDEVICE_NAME_STRING);

    status = IoCreateDevice(DriverObject,               // DriverObject
        sizeof(DEVICE_EXTENSION), // DeviceExtensionSize
        &ntDeviceName,              // DeviceName
        FILE_DEVICE_UNKNOWN,        // DeviceType
        FILE_DEVICE_SECURE_OPEN,    // DeviceCharacteristics
        FALSE,                      // Not Exclusive
        &deviceObject               // DeviceObject
    );

    if (!NT_SUCCESS(status)) {
        DebugPrint(("\tIoCreateDevice returned 0x%x\n", status));
        return(status);
    }

    //
    // Create a symbolic link for userapp to interact with the driver.
    //
    RtlInitUnicodeString(&symbolicLinkName, SYMBOLIC_NAME_STRING);
    status = IoCreateSymbolicLink(&symbolicLinkName, &ntDeviceName);

    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        DebugPrint(("\tIoCreateSymbolicLink returned 0x%x\n", status));
        return(status);
    }

    //
    // Initialize the device extension.
    //
    deviceExtension = (PDEVICE_EXTENSION)deviceObject->DeviceExtension;
    InitializeListHead(&deviceExtension->EventQueueHead);
    KeInitializeSpinLock(&deviceExtension->QueueLock);
    deviceExtension->Self = deviceObject;

    //
    // Establish user-buffer access method.
    //
    deviceObject->Flags |= DO_BUFFERED_IO;

    ASSERT(NT_SUCCESS(status));

    return status;
}

_Use_decl_annotations_
VOID OfbUnload(PDRIVER_OBJECT DriverObject)
{

    PDEVICE_OBJECT       deviceObject = DriverObject->DeviceObject;
    PDEVICE_EXTENSION    deviceExtension = (PDEVICE_EXTENSION)deviceObject->DeviceExtension;
    UNICODE_STRING      symbolicLinkName;

    DebugPrint(("==>Unload\n"));

    PAGED_CODE();

    if (!IsListEmpty(&deviceExtension->EventQueueHead)) {
        ASSERTMSG("Event Queue is not empty\n", FALSE);
    }

    //
    // Delete the user-mode symbolic link and deviceobjct.
    //
    RtlInitUnicodeString(&symbolicLinkName, SYMBOLIC_NAME_STRING);
    IoDeleteSymbolicLink(&symbolicLinkName);
    IoDeleteDevice(deviceObject);

    return;
}

_Use_decl_annotations_
NTSTATUS OfbCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS            status;
    PFILE_CONTEXT       fileContext;

    UNREFERENCED_PARAMETER(DeviceObject);

    PAGED_CODE();

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    ASSERT(irpStack->FileObject != NULL);

    switch (irpStack->MajorFunction)
    {
    case IRP_MJ_CREATE:
        DebugPrint(("IRP_MJ_CREATE\n"));

        fileContext = (PFILE_CONTEXT)ExAllocatePoolWithQuotaTag(NonPagedPool, sizeof(FILE_CONTEXT), TAG);

        if (NULL == fileContext) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        IoInitializeRemoveLock(&fileContext->FileRundownLock, TAG, 0, 0);

        //
        // Make sure nobody is using the FsContext scratch area.
        //
        ASSERT(irpStack->FileObject->FsContext == NULL);

        //
        // Store the context in the FileObject's scratch area.
        //
        irpStack->FileObject->FsContext = (PVOID)fileContext;

        status = STATUS_SUCCESS;
        break;

    case IRP_MJ_CLOSE:
        DebugPrint(("IRP_MJ_CLOSE\n"));

        fileContext = (PFILE_CONTEXT)irpStack->FileObject->FsContext;

        ExFreePoolWithTag(fileContext, TAG);

        status = STATUS_SUCCESS;
        break;

    default:
        ASSERT(FALSE);  // should never hit this
        status = STATUS_NOT_IMPLEMENTED;
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;

}

_Use_decl_annotations_
NTSTATUS OfbCleanup(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    PIO_STACK_LOCATION  irpStack;
    NTSTATUS            status;
    KIRQL               oldIrql;
    PLIST_ENTRY         thisEntry, nextEntry, listHead;
    PNOTIFY_RECORD      notifyRecord;
    PDEVICE_EXTENSION   deviceExtension;
    LIST_ENTRY          cleanupList;
    PFILE_CONTEXT       fileContext;

    DebugPrint(("==>EventCleanup\n"));

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    ASSERT(irpStack->FileObject != NULL);

    fileContext = (PFILE_CONTEXT)irpStack->FileObject->FsContext;

    //
    // This acquire cannot fail because you cannot get more than one
    // cleanup for the same handle.
    //
    status = IoAcquireRemoveLock(&fileContext->FileRundownLock, Irp);
    ASSERT(NT_SUCCESS(status));

    //
    // Wait for all the threads that are currently dispatching to exit and 
    // prevent any threads dispatching I/O on the same handle beyond this point.
    //
    IoReleaseRemoveLockAndWait(&fileContext->FileRundownLock, Irp);

    InitializeListHead(&cleanupList);

    //
    // Walk the list and remove all the pending notification records
    // that belong to this filehandle.
    //

    KeAcquireSpinLock(&deviceExtension->QueueLock, &oldIrql);

    listHead = &deviceExtension->EventQueueHead;

    for (thisEntry = listHead->Flink;
        thisEntry != listHead;
        thisEntry = nextEntry)
    {
        nextEntry = thisEntry->Flink;

        notifyRecord = CONTAINING_RECORD(thisEntry, NOTIFY_RECORD, ListEntry);

        if (irpStack->FileObject == notifyRecord->FileObject) {

            //
            // KeCancelTimer returns if the timer is successfully cancelled.
            // If it returns FALSE, there are two possibilities. Either the
            // TimerDpc has just run and waiting to acquire the lock or it
            // has run to completion. We wouldn't be here if it had run to
            // completion because we wouldn't found the record in the list.
            // So the only possibility is that it's waiting to acquire the lock.
            // In that case, we will just let the DPC to complete the request
            // and free the record.
            //
            if (KeCancelTimer(&notifyRecord->Timer)) {

                DebugPrint(("\tCanceled timer\n"));
                RemoveEntryList(thisEntry);

                    //
                    // Clear the cancel-routine and check the return value to
                    // see whether it was cleared by us or by the I/O manager.
                    //
                    if (IoSetCancelRoutine(notifyRecord->PendingIrp, NULL) != NULL) {

                        //
                        // We cleared it and as a result we own the IRP and
                        // nobody can cancel it anymore. We will queue the IRP
                        // in the local cleanup list so that we can complete
                        // all the IRPs outside the lock to avoid deadlocks in
                        // the completion routine of the driver above us re-enters
                        // our driver.
                        //
                        InsertTailList(&cleanupList,
                            &notifyRecord->PendingIrp->Tail.Overlay.ListEntry);
                        ExFreePoolWithTag(notifyRecord, TAG);

                    }
                    else {
                        //
                        // The I/O manager cleared it and called the cancel-routine.
                        // Cancel routine is probably waiting to acquire the lock.
                        // So reinitialze the ListEntry so that it doesn't crash
                        // when it tries to remove the entry from the list and
                        // set the CancelRoutineFreeMemory to indicate that it should
                        // free the notification record.
                        //
                        InitializeListHead(&notifyRecord->ListEntry);
                        notifyRecord->CancelRoutineFreeMemory = TRUE;
                    }
            }
        }
    }

    KeReleaseSpinLock(&deviceExtension->QueueLock, oldIrql);

    //
    // Walk through the cleanup list and cancel all
    // the IRPs.
    //
    while (!IsListEmpty(&cleanupList))
    {
        PIRP                pendingIrp;
        //
        // Complete the IRP
        //
        thisEntry = RemoveHeadList(&cleanupList);
        pendingIrp = CONTAINING_RECORD(thisEntry, IRP, Tail.Overlay.ListEntry);

        DebugPrint(("\t canceled IRP %p\n", pendingIrp));

        pendingIrp->Tail.Overlay.DriverContext[3] = NULL;
        pendingIrp->IoStatus.Information = 0;
        pendingIrp->IoStatus.Status = STATUS_CANCELLED;

        IoCompleteRequest(pendingIrp, IO_NO_INCREMENT);
    }

    //
    // Finally complete the cleanup Irp
    //
    Irp->IoStatus.Status = status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    DebugPrint(("<== EventCleanup\n"));
    return status;

}

_Use_decl_annotations_
NTSTATUS OfbDispatchIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{

    PIO_STACK_LOCATION  irpStack;
    PREGISTER_EVENT registerEvent;
    NTSTATUS    status;
    PFILE_CONTEXT fileContext;

    DebugPrint(("==> EventDispatchIoControl\n"));

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    ASSERT(irpStack->FileObject != NULL);

    fileContext = (PFILE_CONTEXT)irpStack->FileObject->FsContext;

    status = IoAcquireRemoveLock(&fileContext->FileRundownLock, Irp);
    if (!NT_SUCCESS(status)) {
        //
        // Lock is in a removed state. That means we have already received 
        // cleaned up request for this handle. 
        //
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode)
    {
    case IOCTL_REGISTER_EVENT:

        DebugPrint(("\tIOCTL_REGISTER_EVENT\n"));

        //
        // First validate the parameters.
        //
        if (irpStack->Parameters.DeviceIoControl.InputBufferLength < SIZEOF_REGISTER_EVENT) {
            status = STATUS_INVALID_PARAMETER;
            break;
        }

        registerEvent = (PREGISTER_EVENT)Irp->AssociatedIrp.SystemBuffer;
        status = RegisterIrpBasedNotification(DeviceObject, Irp);

        break;

    default:
        ASSERT(FALSE);  // should never hit this
        status = STATUS_NOT_IMPLEMENTED;
        break;

    } // switch IoControlCode

    if (status != STATUS_PENDING) {
        //
        // complete the Irp
        //
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }

    //
    // We don't hold the lock for IRP that's pending in the list because this
    // lock is meant to rundown currently dispatching threads when the cleanup
    // is handled.
    //
    IoReleaseRemoveLock(&fileContext->FileRundownLock, Irp);

    DebugPrint(("<== EventDispatchIoControl\n"));
    return status;
}

_Use_decl_annotations_
VOID OfbCancelRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    PDEVICE_EXTENSION   deviceExtension;
    KIRQL               oldIrql;
    PNOTIFY_RECORD      notifyRecord;

    DebugPrint(("==>EventCancelRoutine irp %p\n", Irp));

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Release the cancel spinlock
    //
    IoReleaseCancelSpinLock(Irp->CancelIrql);

    //
    // Acquire the queue spinlock
    //
    KeAcquireSpinLock(&deviceExtension->QueueLock, &oldIrql);

    notifyRecord = (PNOTIFY_RECORD)Irp->Tail.Overlay.DriverContext[3];
    ASSERT(NULL != notifyRecord);

    RemoveEntryList(&notifyRecord->ListEntry);

    //
    // Clear the pending Irp field because we complete the IRP no matter whether
    // we succeed or fail to cancel the timer. TimerDpc will check this field
    // before dereferencing the IRP.
    //
    notifyRecord->PendingIrp = NULL;

    if (KeCancelTimer(&notifyRecord->Timer)) {
        DebugPrint(("\t canceled timer\n"));
        ExFreePoolWithTag(notifyRecord, TAG);
        notifyRecord = NULL;
    }
    else {
        //
        // Here the possibilities are:
        // 1) DPC is fired and waiting to acquire the lock.
        // 2) DPC has run to completion.
        // 3) DPC has been cancelled by the cleanup routine.
        // By checking the CancelRoutineFreeMemory, we can figure out whether
        // dpc is waiting to acquire the lock and access the notifyRecord memory.
        //
        if (notifyRecord->CancelRoutineFreeMemory == FALSE) {
            //
            // This is case 1 where the DPC is waiting to run.
            //
            InitializeListHead(&notifyRecord->ListEntry);
        }
        else {
            //
            // This is either 2 or 3.
            //
            ExFreePoolWithTag(notifyRecord, TAG);
            notifyRecord = NULL;
        }

    }

    KeReleaseSpinLock(&deviceExtension->QueueLock, oldIrql);

    DebugPrint(("\t canceled IRP %p\n", Irp));
    Irp->Tail.Overlay.DriverContext[3] = NULL;
    Irp->IoStatus.Status = STATUS_CANCELLED;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    DebugPrint(("<==EventCancelRoutine irp %p\n", Irp));
    return;

}

_Use_decl_annotations_
VOID CustomTimerDPC(PKDPC Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
{
    PNOTIFY_RECORD      notifyRecord = (PNOTIFY_RECORD)DeferredContext;
    PDEVICE_EXTENSION deviceExtension;
    PIRP irp;

    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);

    DebugPrint(("==> CustomTimerDPC \n"));

    ASSERT(notifyRecord != NULL); // can't be NULL
    _Analysis_assume_(notifyRecord != NULL);

    deviceExtension = notifyRecord->DeviceExtension;

    KeAcquireSpinLockAtDpcLevel(&deviceExtension->QueueLock);

    RemoveEntryList(&notifyRecord->ListEntry);

        irp = notifyRecord->PendingIrp;
        if (irp != NULL) {
            if (IoSetCancelRoutine(irp, NULL) != NULL) {

                irp->Tail.Overlay.DriverContext[3] = NULL;

                //
                // Drop the lock before completing the request.
                //
                KeReleaseSpinLockFromDpcLevel(&deviceExtension->QueueLock);

                irp->IoStatus.Status = STATUS_SUCCESS;
                irp->IoStatus.Information = 0;
                IoCompleteRequest(irp, IO_NO_INCREMENT);

                KeAcquireSpinLockAtDpcLevel(&deviceExtension->QueueLock);

            }
            else {
                //
                // Cancel routine will run as soon as we release the lock.
                // So let it complete the request and free the record.
                //
                InitializeListHead(&notifyRecord->ListEntry);
                notifyRecord->CancelRoutineFreeMemory = TRUE;
                notifyRecord = NULL;
            }
        }
        else {
            //
            // Cancel routine has run and completed the IRP. So just free
            // the record.
            //
            ASSERT(notifyRecord->CancelRoutineFreeMemory == FALSE);
        }

    KeReleaseSpinLockFromDpcLevel(&deviceExtension->QueueLock);

    //
    // Free the memory outside the lock for better performance.
    //
    if (notifyRecord != NULL) {
        ExFreePoolWithTag(notifyRecord, TAG);
        notifyRecord = NULL;
    }

    DebugPrint(("<== CustomTimerDPC\n"));

    return;
}

_Use_decl_annotations_
NTSTATUS RegisterIrpBasedNotification(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    PDEVICE_EXTENSION   deviceExtension;
    PNOTIFY_RECORD notifyRecord;
    PIO_STACK_LOCATION irpStack;
    KIRQL   oldIrql;
    PREGISTER_EVENT registerEvent;

    DebugPrint(("\tRegisterIrpBasedNotification\n"));

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    registerEvent = (PREGISTER_EVENT)Irp->AssociatedIrp.SystemBuffer;

    //
    // Allocate a record and save all the event context.
    //

    notifyRecord = (PNOTIFY_RECORD)ExAllocatePoolWithQuotaTag(NonPagedPool, sizeof(NOTIFY_RECORD), TAG);

    if (NULL == notifyRecord) {
        return  STATUS_INSUFFICIENT_RESOURCES;
    }

    InitializeListHead(&notifyRecord->ListEntry);

    notifyRecord->FileObject = irpStack->FileObject;
    notifyRecord->DeviceExtension = deviceExtension;
    notifyRecord->PendingIrp = Irp;

    //
    // Start the timer to run the CustomTimerDPC in DueTime seconds to
    // simulate an interrupt (which would queue a DPC).
    // The user's event object is signaled or the IRP is completed in the DPC to
    // notify the hardware event.
    //

    // ensure relative time for this sample

    if (registerEvent->DueTime.QuadPart > 0) {
        registerEvent->DueTime.QuadPart = -(registerEvent->DueTime.QuadPart);
    }

    KeInitializeDpc(&notifyRecord->Dpc, CustomTimerDPC, notifyRecord);
    KeInitializeTimer(&notifyRecord->Timer);

    //
    // We will set the cancel routine and TimerDpc within the
    // lock so that they don't modify the list before we are
    // completely done.
    //
    KeAcquireSpinLock(&deviceExtension->QueueLock, &oldIrql);

    //
    // Set the cancel routine. This is required if the app decides to
    // exit or cancel the event prematurely.
    //
    IoSetCancelRoutine(Irp, OfbCancelRoutine);

    //
    // Before we queue the IRP, we must check to see if it's cancelled.
    //
    if (Irp->Cancel) {
        // Clear the cancel-routine automically and check the return value.
        // We will complete the IRP here if we succeed in clearing it. If
        // we fail then we will let the cancel-routine complete it.
        //
        if (IoSetCancelRoutine(Irp, NULL) != NULL) {
            // We are able to successfully clear the routine. Either the
            // the IRP is cancelled before we set the cancel-routine or
            // we won the race with I/O manager in clearing the routine.
            // Return STATUS_CANCELLED so that the caller can complete
            // the request.
            KeReleaseSpinLock(&deviceExtension->QueueLock, oldIrql);
            ExFreePoolWithTag(notifyRecord, TAG);
            return STATUS_CANCELLED;
        }
        else {
            //
            // The IRP got cancelled after we set the cancel-routine and the
            // I/O manager won the race in clearing it and called the cancel
            // routine. So queue the request so that cancel-routine can dequeue
            // and complete it. Note the cancel-routine cannot run until we
            // drop the queue lock.
            //
        }
    }

    IoMarkIrpPending(Irp);
    InsertTailList(&deviceExtension->EventQueueHead, &notifyRecord->ListEntry);
    notifyRecord->CancelRoutineFreeMemory = FALSE;

    //
    // We will save the record pointer in the IRP so that we can get to
    // it directly in the CancelRoutine.
    //
    Irp->Tail.Overlay.DriverContext[3] = notifyRecord;

    KeSetTimer(&notifyRecord->Timer, registerEvent->DueTime, &notifyRecord->Dpc);
    KeReleaseSpinLock(&deviceExtension->QueueLock, oldIrql);

    //
    // We will return pending as we have marked the IRP pending.
    //
    return STATUS_PENDING;
}