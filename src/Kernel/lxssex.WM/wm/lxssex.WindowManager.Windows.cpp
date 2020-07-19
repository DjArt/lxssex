#include "lxssex.WindowManager.Windows.hpp"

NTSTATUS WindowsWindowManagerInitializate(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING registryPath)
{
    UNREFERENCED_PARAMETER(registryPath);

    UNICODE_STRING ntDeviceServiceName = RTL_CONSTANT_STRING(NTDEVICE_SERVICE_NAME);
    UNICODE_STRING symbolicServiceName = RTL_CONSTANT_STRING(SYMBOLIC_SERVICE_NAME);
    PDEVICE_OBJECT deviceObject;
    NTSTATUS status = IoCreateDevice(DriverObject, sizeof(WINDOWS_WM_DEVICE), &ntDeviceServiceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &deviceObject);
    if (!NT_SUCCESS(status)) goto Fail;
    status = IoCreateSymbolicLink(&symbolicServiceName, &ntDeviceServiceName);
    if (status == STATUS_OBJECT_NAME_COLLISION) status = STATUS_SUCCESS;
    if (!NT_SUCCESS(status)) goto Fail;

    deviceObject->Flags |= DO_BUFFERED_IO;
    DriverObject->DriverExtension->AddDevice = WindowsWindowManagerAddDevice;

    return status;

Fail:
    IoDeleteSymbolicLink(&symbolicServiceName);
    IoDeleteDevice(deviceObject);
    return status;
}

NTSTATUS WindowsWindowManagerAddDevice(_In_ PDRIVER_OBJECT DriverObject, _In_ PDEVICE_OBJECT PhysicalDeviceObject)
{
    PWINDOWS_WM_DEVICE extensionObject = (PWINDOWS_WM_DEVICE)DriverObject->DeviceObject->DeviceExtension;
    NTSTATUS status = IoRegisterDeviceInterface(PhysicalDeviceObject, &LXSSEX_WM_INTERFACE_GUID, NULL, &extensionObject->InterfaceSymbolicLinkName);
    if (!NT_SUCCESS(status)) goto FAIL;
    status = IoSetDeviceInterfaceState(&extensionObject->InterfaceSymbolicLinkName, TRUE);
    if (!NT_SUCCESS(status)) goto FAIL;

FAIL:
    return status;
}

NTSTATUS WindowsWindowManagerCreate(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    UNREFERENCED_PARAMETER(deviceObject);

    NTSTATUS status;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    PWINDOWS_WM_FILE file = (PWINDOWS_WM_FILE)ExAllocatePoolWithQuotaTag(NonPagedPool, sizeof(WINDOWS_WM_FILE), TAG);

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

VOID WindowsWindowManagerUninitializate(_In_ PDRIVER_OBJECT DeviceObject)
{
    UNICODE_STRING symbolicServiceName = RTL_CONSTANT_STRING(NTDEVICE_SERVICE_NAME);
    PWINDOWS_WM_DEVICE extensionObject = (PWINDOWS_WM_DEVICE)DeviceObject->DeviceObject->DeviceExtension;
    NTSTATUS status = IoSetDeviceInterfaceState(&extensionObject->InterfaceSymbolicLinkName, TRUE);
    status = IoDeleteSymbolicLink(&symbolicServiceName);
    IoDeleteDevice(DeviceObject->DeviceObject);
}

NTSTATUS WindowsWindowManagerClose(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
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

NTSTATUS WindowsWindowManagerCleanup(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    NTSTATUS status;
    //PWINDOWS_WM_DEVICE extension = (PWINDOWS_WM_DEVICE)deviceObject->DeviceExtension;
    //PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    //PWINDOWS_WM_FILE file = (PWINDOWS_WM_FILE)irpStack->FileObject->FsContext;

    status = STATUS_SUCCESS;
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = NULL;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS WindowsWindowManagerIOControl(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    //PWINDOWS_WM_FILE file = (PWINDOWS_WM_FILE)irpStack->FileObject->FsContext;

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode)
    {
    case IOCTL_OPEN_CHANNEL_PAIR:
        status = IoctlOpenChannelPair(Irp->UserBuffer);
        break;

    case IOCTL_QUERY_UNHANDLED_PAIRS:
        status = IoctlQueryUnhandledPairs(Irp->UserBuffer);
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