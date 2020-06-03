#include "vfsplugin.hpp"
#include "service/lxssex.Service.Linux.hpp"
#include "service/lxssex.Service.Shared.hpp"
#include "service/lxssex.Service.Windows.hpp"

INT InitializationRoutine(_In_ PLX_INSTANCE Instance)
{
    LinuxServiceInitialize(Instance);
    return LoadPlugins(Instance);
}

EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    NTSTATUS status;

    LXP_SUBSYSTEM_INFORMATION subsystemInformation
    {
        .MountNamespaceInitialize = InitializationRoutine
    };
    status = LxInitialize(DriverObject, &subsystemInformation);
    if (status == STATUS_TOO_LATE) status = STATUS_SUCCESS;

    status = WindowsServiceInitializate(DriverObject, RegistryPath);
    DriverObject->DriverUnload = (PDRIVER_UNLOAD)&WindowsServiceInitializate;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP] = (PDRIVER_DISPATCH)&WindowsServiceCleanup;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = (PDRIVER_DISPATCH)&WindowsServiceClose;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = (PDRIVER_DISPATCH)&WindowsServiceCreate;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)&WindowsServiceIOControl;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)&WindowsServiceIOControl;

    return STATUS_SUCCESS;
}