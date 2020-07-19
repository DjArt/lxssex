#pragma once

#include "lxssex.WindowManager.Shared.hpp"

typedef struct _LINUX_WM_DEVICE
{
    VFS_MINOR_DEVICE Base;
    PLX_INSTANCE LxInstance;
} LINUX_WM_DEVICE, * PLINUX_WM_DEVICE;

typedef struct _LINUX_WM_FILE : VFS_FILE
{
    PLINUX_WM_DEVICE Device;
    PWM_CHANNEL_PAIR ChannelPair;
} LINUX_WM_FILE, * PLINUX_WM_FILE;

VFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK LinuxWindowManagerInitialize;
VFS_MINOR_DEVICE_OPEN_CALLBACK LinuxWindowManagerOpen;
VFS_MINOR_DEVICE_UNINITIALIZE_CALLBACK LinuxWindowManagerUninitialize;
VFS_FILE_DELETE_CALLBACK LinuxWindowManagerDelete;
VFS_FILE_IOCTL_CALLBACK LinuxWindowManagerIoctl;
VFS_FILE_MMAP_CALLBACK LinuxWindowManagerMmap;