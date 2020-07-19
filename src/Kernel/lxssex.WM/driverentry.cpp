#include "wm/lxssex.WindowManager.Linux.hpp"
#include "wm/lxssex.WindowManager.Windows.hpp"

extern VFS_PLUGIN WindowManager =
{
    .PluginName = RTL_CONSTANT_STRING(L"lxssex.WindowManager"),
};

VOID DriverUnload(_In_ PDRIVER_OBJECT DeviceObject)
{
    WindowsWindowManagerUninitializate(DeviceObject);
}

EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    NTSTATUS status;

    InitializationRoutine();

    VFS_PLUGIN_CALLBACKS callbacks =
    {
        .InitializationCallback = LinuxWindowManagerInitialize,
    };
    status = VfsRegisterPlugin(&WindowManager, &callbacks);

    status = WindowsWindowManagerInitializate(DriverObject, RegistryPath);
    DriverObject->DriverUnload = DriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP] = WindowsWindowManagerCleanup;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = WindowsWindowManagerClose;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = WindowsWindowManagerCreate;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = WindowsWindowManagerIOControl;

    return status;
}