#include "default.h"
#include "fbdev.h"
#include "ofbdev.h"

INT InitializationRoutine(_In_ PLX_INSTANCE Instance)
{
    DbgPrintEx(77, 0, "lxssex: InitializationRoutine\n");
    return FbInitialize(Instance);
}

NTSTATUS DriverCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    return OfbCreateClose(DeviceObject, Irp);
}

NTSTATUS DriverClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    return OfbCreateClose(DeviceObject, Irp);
}

NTSTATUS DriverCleanup(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    return OfbCleanup(DeviceObject, Irp);
}

NTSTATUS DriverDispatchIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    return OfbDispatchIoControl(DeviceObject, Irp);
}

EXTERN_C VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    OfbUnload(DriverObject);
    return;
}

EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    DbgPrintEx(77, 0, "Loading lxssex\n");


    NTSTATUS status;
    LXP_SUBSYSTEM_INFORMATION subsystemInformation;
    UNREFERENCED_PARAMETER(RegistryPath);

    DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP] = DriverCleanup;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDispatchIoControl;
    DriverObject->DriverUnload = DriverUnload;

    status = OfbEntry(DriverObject, RegistryPath);
    DbgPrintEx(77, 0, "lxssex: OfbEntry exited with status: %x\n", status);

    //
    // Register our own namespace initialization routine
    //
    subsystemInformation.MountNamespaceInitialize = InitializationRoutine;
    status = LxInitialize(DriverObject, &subsystemInformation);
    DbgPrintEx(77, 0, "lxssex: g_LxInitialize exited with status: %x\n", status);
    if (status != STATUS_TOO_LATE)
    {
        return status;
    }

    //
    // We are ready for instance creation
    //
    return STATUS_SUCCESS;
}