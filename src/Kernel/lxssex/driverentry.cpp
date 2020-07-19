#include "vfsplugin.hpp"
#include "service/lxssex.Service.Linux.hpp"
#include "service/lxssex.Service.Windows.hpp"

NTSTATUS InitializationRoutine(_In_ PLX_INSTANCE Instance)
{
    LinuxServiceInitialize(Instance);
    return LoadPlugins(Instance);
}

VOID DriverUnload(_In_ PDRIVER_OBJECT DeviceObject)
{
    WindowsServiceUninitializate(DeviceObject);
}

EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    NTSTATUS status;

    InitializationRoutine();

    LXP_SUBSYSTEM_INFORMATION subsystemInformation
    {
        .MountNamespaceInitialize = InitializationRoutine
    };
    status = LxInitialize(DriverObject, &subsystemInformation);
    if (status == STATUS_TOO_LATE) status = STATUS_SUCCESS;

    InitializeVfsPluginSystem();

    status = WindowsServiceInitializate(DriverObject, RegistryPath);
    DriverObject->DriverUnload = DriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP] = WindowsServiceCleanup;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = WindowsServiceClose;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = WindowsServiceCreate;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = WindowsServiceIOControl;


    return status;
}