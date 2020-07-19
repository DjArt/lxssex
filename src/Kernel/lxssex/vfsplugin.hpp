#pragma once

#include "include/km/vfsplugin.h"

typedef struct _VFS_PLUGIN_ENTRY
{
	VFS_PLUGIN Plugin;
	VFS_PLUGIN_CALLBACKS Callbacks;
	LIST_ENTRY ListEntry;
} VFS_PLUGIN_ENTRY, * PVFS_PLUGIN_ENTRY;

typedef struct _VFS_PLUGIN_SETTINGS
{
	PVFS_PLUGIN_ENTRY PluginEntry;
	BOOLEAN IsEnabled;
} VFS_PLUGIN_SETTINGS, * PVFS_PLUGIN_SETTINGS;

VOID InitializeVfsPluginSystem();
INT LoadPlugins(_In_ PLX_INSTANCE Instance);