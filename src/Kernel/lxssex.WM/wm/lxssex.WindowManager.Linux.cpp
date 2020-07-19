#include "lxssex.WindowManager.Linux.hpp"

VFS_ENTRY LinuxWindowManagerVfsEntries[1] =
{
    {
        .Type = VfsNodeEntry,
        .Name = RTL_CONSTANT_STRING(L"/dev/lxssex.wm"),
        .Data = {
            .Node = {
                .Uid = 0,
                .Gid = 0,
                .Mode = S_IFCHR | S_IWUSR | S_IRUSR | S_IROTH | S_IWOTH,
                .MinorDeviceId = 301,
                .MajorDeviceId = 29
            }
        }
    }
};

VFS_MINOR_DEVICE_CALLBACKS LinuxWindowManagerDeviceCallbacks =
{
    .Open = LinuxWindowManagerOpen,
    .Uninitialize = LinuxWindowManagerUninitialize
};

VFS_FILE_CALLBACKS LinuxWindowManagerFileCallbacks =
{
    .Delete = LinuxWindowManagerDelete,
    .Ioctl = LinuxWindowManagerIoctl
};

NTSTATUS LinuxWindowManagerInitialize(_In_ PLX_INSTANCE instance)
{
    LONG error;
    PLINUX_WM_DEVICE device = (PLINUX_WM_DEVICE)(VfsDeviceMinorAllocate(&LinuxWindowManagerDeviceCallbacks, sizeof(LINUX_WM_DEVICE)));

    if (device == NULL) return LxpUtilTranslateStatus(STATUS_INSUFFICIENT_RESOURCES);

    device->LxInstance = instance;

    LxpDevMiscRegister(instance, &device->Base, 6578);
    // Registration takes a reference, drop the allocation one
    VfsDeviceMinorDereference(&device->Base);

    // Initialize the custom VFS entries
    error = VfsInitializeStartupEntries(instance, LinuxWindowManagerVfsEntries, RTL_NUMBER_OF(LinuxWindowManagerVfsEntries));

    return error;
}

VOID LinuxWindowManagerUninitialize(_In_ PVFS_MINOR_DEVICE Device)
{
    UNREFERENCED_PARAMETER(Device);
}

INT LinuxWindowManagerOpen(_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_MINOR_DEVICE Device, _In_ ULONG Flags, _Out_ PVFS_FILE* File)
{
    UNREFERENCED_PARAMETER(CallContext);
    UNREFERENCED_PARAMETER(Flags);

    PLINUX_WM_DEVICE device = (PLINUX_WM_DEVICE)Device;
    PLINUX_WM_FILE file = (PLINUX_WM_FILE)VfsFileAllocate(sizeof(LINUX_WM_FILE), &LinuxWindowManagerFileCallbacks);

    file->Device = device;

    *File = file;
    return LxpUtilTranslateStatus(STATUS_SUCCESS);
}

INT LinuxWindowManagerDelete(_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File)
{
    UNREFERENCED_PARAMETER(CallContext);
    UNREFERENCED_PARAMETER(File);

    return LxpUtilTranslateStatus(STATUS_SUCCESS);
}

INT LinuxWindowManagerIoctl(_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File, _In_ ULONG Ioctl, _Inout_ PVOID Buffer)
{
    UNREFERENCED_PARAMETER(CallContext);
    UNREFERENCED_PARAMETER(File);

    NTSTATUS status;

    switch (Ioctl)
    {
    case LXSSEX_WM_IOCTL::WMOpenChannelPair:
        status = IoctlOpenChannelPair(Buffer);
        break;

    case LXSSEX_WM_IOCTL::WMQueryUnhandledPairs:
        status = IoctlQueryUnhandledPairs(Buffer);
        break;

    default:
        status = STATUS_INVALID_PARAMETER;
    }

    return LxpUtilTranslateStatus(status);
}