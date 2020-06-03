#pragma once

#include "lxssex.Service.Shared.hpp"

typedef struct _LINUX_SERVICE_DEVICE
{
    VFS_MINOR_DEVICE Base;
    PLX_INSTANCE LxInstance;
} LINUX_SERVICE_DEVICE, * PLINUX_SERVICE_DEVICE;

typedef struct _LINUX_SERVICE_FILE : VFS_FILE
{
    PLINUX_SERVICE_DEVICE Device;
    VFS_MMAP_FILE_CONTEXT MmapFileContext;
    EPOLL_FILE_STATE EpollFileState;
    PCHANNEL Channel;
} LINUX_SERVICE_FILE, * PLINUX_SERVICE_FILE;

VFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK LinuxServiceInitialize;
VFS_MINOR_DEVICE_OPEN_CALLBACK LinuxServiceOpen;
VFS_MINOR_DEVICE_UNINITIALIZE_CALLBACK LinuxServiceUninitialize;
VFS_FILE_DELETE_CALLBACK LinuxServiceDelete;
VFS_FILE_IOCTL_CALLBACK LinuxServiceIoctl;
VFS_FILE_MMAP_CALLBACK LinuxServiceMmap;

enum LINUX_SERVICE_FILE_IOCTL
{
    OpenChannel = 0x0,
    SetEvent = 0x1,
    GetEvent = 0x2,
};