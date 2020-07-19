#pragma once

#include <lxcore.h>

typedef struct _VFS_PLUGIN
{
	UNICODE_STRING PluginName;
} VFS_PLUGIN, * PVFS_PLUGIN;

typedef struct _VFS_PLUGIN_CALLBACKS
{
	PVFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK InitializationCallback;
} VFS_PLUGIN_CALLBACKS, * PVFS_PLUGIN_CALLBACKS;

EXTERN_C NTSTATUS VfsRegisterPlugin(_In_ PVFS_PLUGIN Plugin, _In_ PVFS_PLUGIN_CALLBACKS Callbacks);
EXTERN_C NTSTATUS VfsUnregisterPlugin(_In_ PVFS_PLUGIN Plugin);