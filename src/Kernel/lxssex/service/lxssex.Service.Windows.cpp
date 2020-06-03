#include "lxssex.Service.Windows.hpp"

NTSTATUS WindowsServiceInitializate(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING registryPath)
{
    UNREFERENCED_PARAMETER(registryPath);

    PDEVICE_OBJECT deviceObject;
    UNICODE_STRING symbolicLinkName;
    UNICODE_STRING ntDeviceServiceName = RTL_CONSTANT_STRING(NTDEVICE_SERVICE_NAME);
    UNICODE_STRING symbolicServiceName = RTL_CONSTANT_STRING(SYMBOLIC_SERVICE_NAME);
    NTSTATUS status = IoCreateDevice(DriverObject, sizeof(WINDOWS_SERVICE_DEVICE), &ntDeviceServiceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &deviceObject);

    if (!NT_SUCCESS(status)) return status;

    status = IoCreateSymbolicLink(&symbolicServiceName, &ntDeviceServiceName);
    if (!NT_SUCCESS(status)) goto Fail;
    status = IoRegisterDeviceInterface(deviceObject, &LXSSEX_INTERFACE_GUID, NULL, &symbolicLinkName);
    if (!NT_SUCCESS(status)) goto Fail;

    deviceObject->Flags |= DO_BUFFERED_IO;

    return status;

Fail:
    IoDeleteDevice(deviceObject);
    return status;
}

NTSTATUS WindowsServiceCreate(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    UNREFERENCED_PARAMETER(deviceObject);

    NTSTATUS status;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    PWINDOWS_SERVICE_FILE file = (PWINDOWS_SERVICE_FILE)ExAllocatePoolWithQuotaTag(NonPagedPool, sizeof(WINDOWS_SERVICE_FILE), TAG);

    if (file == NULL)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    irpStack->FileObject->FsContext = file;

    status = STATUS_SUCCESS;
Exit:
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = NULL;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return status;
}

VOID WindowsServiceUninitializate(_In_ PDEVICE_OBJECT DriverObject)
{

}

NTSTATUS WindowsServiceClose(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    UNREFERENCED_PARAMETER(deviceObject);

    NTSTATUS status;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);

    ExFreePoolWithTag(irpStack->FileObject->FsContext, TAG);

    status = STATUS_SUCCESS;
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = NULL;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS WindowsServiceCleanup(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    NTSTATUS status;
    PWINDOWS_SERVICE_DEVICE extension = (PWINDOWS_SERVICE_DEVICE)deviceObject->DeviceExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    PWINDOWS_SERVICE_FILE file = (PWINDOWS_SERVICE_FILE)irpStack->FileObject->FsContext;

    SetChannelEvent(file->Channel, WindowsSide | ChannelClosed);
    file->Channel = NULL;

    status = STATUS_SUCCESS;
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = NULL;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS WindowsServiceIOControl(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP Irp)
{
    NTSTATUS status;

    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PWINDOWS_SERVICE_FILE file = (PWINDOWS_SERVICE_FILE)irpStack->FileObject->FsContext;

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode)
    {
    case IOCTL_OPEN_CHANNEL:
        if (file->Channel == NULL)
        {
            __try
            {
                ProbeForRead(Irp->UserBuffer, sizeof(CHANNEL_REGISTRATION_WINDOWS), sizeof(ULONG));
                ProbeForWrite(Irp->UserBuffer, sizeof(CHANNEL_REGISTRATION_WINDOWS), sizeof(ULONG));
                PCHANNEL_REGISTRATION_WINDOWS registration = (PCHANNEL_REGISTRATION_WINDOWS)Irp->UserBuffer;
                PCHANNEL_CONTEXT context = FindChannelContext((PCHANNEL)registration->ChannelHandle);
                if (context)
                {
                    irpStack->FileObject->FsContext = context->Channel;
                    registration->Channel = *context->Channel;
                    status = ObReferenceObjectByHandle(registration->ChannelEvent, SYNCHRONIZE | EVENT_MODIFY_STATE, *ExEventObjectType, Irp->RequestorMode, (PVOID*)&context->WindowsChannelEvent, NULL);
                    if (NT_SUCCESS(status))
                    {
                        status = ZwMapViewOfSection(context->SharedSection, ZwCurrentProcess(), &registration->SharedSection, 0, PAGE_SIZE, NULL, &context->Channel->SectionSize, ViewUnmap, 0, PAGE_READWRITE);
                        if (NT_SUCCESS(status))
                        {
                            SetChannelEvent(context->Channel, WindowsSide | ChannelOpened);
                        }
                        {
                            ObDereferenceObject(context->WindowsChannelEvent);
                        }
                    }
                }
                else
                {
                    status = STATUS_INVALID_PARAMETER;
                }
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();
            }
        }
        else
        {
            status = STATUS_DEVICE_ALREADY_ATTACHED;
        }
        break;

    case IOCTL_SET_EVENT:
        if (file->Channel)
        {
            __try
            {
                ProbeForRead(Irp->UserBuffer, sizeof(CHANNEL_EVENT_TYPE), sizeof(ULONG));
                PCHANNEL_EVENT_TYPE eventType = (PCHANNEL_EVENT_TYPE)Irp->UserBuffer;
                switch (*eventType)
                {
                case ChannelSync:
                    SetChannelEvent(file->Channel, WindowsSide | ChannelSync);
                    status = STATUS_SUCCESS;
                    break;
                default:
                    status = STATUS_INVALID_PARAMETER;
                    break;
                }
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();
            }
        }
        else
        {
            status = STATUS_DEVICE_NOT_READY;
        }
        break;

    case IOCTL_GET_EVENT:
        if (file->Channel)
        {
            __try
            {
                ProbeForWrite(Irp->UserBuffer, sizeof(CHANNEL_EVENT_TYPE), sizeof(ULONG));
                PCHANNEL_EVENT_TYPE eventType = (PCHANNEL_EVENT_TYPE)Irp->UserBuffer;
                PCHANNEL_CONTEXT context = FindChannelContext(file->Channel);
                *eventType = context != NULL ? context->LastEventType : ChannelClosed;
                status = STATUS_SUCCESS;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();
            }
        }
        else
        {
            status = STATUS_DEVICE_NOT_READY;
        }
        break;

    default:
        status = STATUS_INVALID_PARAMETER;
        break;
    }

    if (status != STATUS_PENDING)
    {
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }
    return status;
}