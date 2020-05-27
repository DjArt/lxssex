#pragma once

#include <lxcore.h>

#include "include/vfsplugin.h"

typedef struct _VFS_PLUGIN_SETTINGS
{
	PVFS_PLUGIN Plugin;
	BOOLEAN IsEnabled;
} VFS_PLUGIN_SETTINGS, * PVFS_PLUGIN_SETTINGS;

extern PVFS_PLUGIN_SETTINGS PluginStore;
extern SIZE_T PluginStoreLength;

INT LoadPlugins(_In_ PLX_INSTANCE Instance);