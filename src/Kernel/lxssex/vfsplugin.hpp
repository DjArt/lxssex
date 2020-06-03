#pragma once

#include <lxcore.h>

#include "include/km/vfsplugin.h"

typedef struct _VFS_PLUGIN_ENTRY
{
	VFS_PLUGIN Plugin;
	VFS_PLUGIN_CALLBACKS Callbacks;
} VFS_PLUGIN_ENTRY, * PVFS_PLUGIN_ENTRY;

typedef struct _VFS_PLUGIN_SETTINGS
{
	PVFS_PLUGIN_ENTRY PluginEntry;
	BOOLEAN IsEnabled;
} VFS_PLUGIN_SETTINGS, * PVFS_PLUGIN_SETTINGS;

INT LoadPlugins(_In_ PLX_INSTANCE Instance);