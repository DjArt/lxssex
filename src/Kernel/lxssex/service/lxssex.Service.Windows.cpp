#include "lxssex.Service.Windows.hpp"

NTSTATUS WindowsServiceInitializate(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING registryPath)
{
    UNREFERENCED_PARAMETER(registryPath);

    UNICODE_STRING ntDeviceServiceName = RTL_CONSTANT_STRING(NTDEVICE_SERVICE_NAME);
    UNICODE_STRING symbolicServiceName = RTL_CONSTANT_STRING(SYMBOLIC_SERVICE_NAME);
    PDEVICE_OBJECT deviceObject;
    NTSTATUS status = IoCreateDevice(DriverObject, sizeof(WINDOWS_SERVICE_DEVICE), &ntDeviceServiceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &deviceObject);
    if (!NT_SUCCESS(status)) goto Fail;
    status = IoCreateSymbolicLink(&symbolicServiceName, &ntDeviceServiceName);
    if (status == STATUS_OBJECT_NAME_COLLISION) status = STATUS_SUCCESS;
    if (!NT_SUCCESS(status)) goto Fail;
    
    deviceObject->Flags |= DO_BUFFERED_IO;
    DriverObject->DriverExtension->AddDevice = WindowsServiceAddDevice;

    return status;

Fail:
    IoDeleteSymbolicLink(&symbolicServiceName);
    IoDeleteDevice(deviceObject);
    return status;
}

NTSTATUS WindowsServiceAddDevice(_In_ PDRIVER_OBJECT DriverObject, _In_ PDEVICE_OBJECT PhysicalDeviceObject)
{
    PWINDOWS_SERVICE_DEVICE extensionObject = (PWINDOWS_SERVICE_DEVICE)DriverObject->DeviceObject->DeviceExtension;
    NTSTATUS status = IoRegisterDeviceInterface(PhysicalDeviceObject, &LXSSEX_INTERFACE_GUID, NULL, &extensionObject->InterfaceSymbolicLinkName);
    if (!NT_SUCCESS(status)) goto FAIL;
    status = IoSetDeviceInterfaceState(&extensionObject->InterfaceSymbolicLinkName, TRUE);
    if (!NT_SUCCESS(status)) goto FAIL;

FAIL:
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

VOID WindowsServiceUninitializate(_In_ PDRIVER_OBJECT DeviceObject)
{
    UNICODE_STRING symbolicServiceName = RTL_CONSTANT_STRING(SYMBOLIC_SERVICE_NAME);
    PWINDOWS_SERVICE_DEVICE extensionObject = (PWINDOWS_SERVICE_DEVICE)DeviceObject->DeviceObject->DeviceExtension;
    NTSTATUS status = IoSetDeviceInterfaceState(&extensionObject->InterfaceSymbolicLinkName, TRUE);
    status = IoDeleteSymbolicLink(&symbolicServiceName);
    IoDeleteDevice(DeviceObject->DeviceObject);
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

    PCHANNEL_CONTEXT context = FindChannelContext(file->Channel);
    if (context) SetChannelEvent(context, WindowsSide | ChannelClosed);
    file->Channel = NULL;

    status = STATUS_SUCCESS;
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = NULL;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS WindowsServiceIOControl(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(deviceObject);
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
                ProbeForRead(Irp->UserBuffer, sizeof(CHANNEL_REGISTRATION_WINDOWS), sizeof(PVOID));
                ProbeForWrite(Irp->UserBuffer, sizeof(CHANNEL_REGISTRATION_WINDOWS), sizeof(PVOID));
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
                            SetChannelEvent(context, WindowsSide | ChannelOpened);
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
            status = IoctlSetEvent(file->Channel, Irp->UserBuffer, WindowsSide);
        }
        else
        {
            status = STATUS_DEVICE_NOT_READY;
        }
        break;

    case IOCTL_GET_EVENT:
        if (file->Channel)
        {
            status = IoctlGetEvent(file->Channel, Irp->UserBuffer);
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