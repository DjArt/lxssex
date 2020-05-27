#include "vfsplugin.hpp"

#define PLUGIN_STORE_POOL_TAG 'TPSP'

PVFS_PLUGIN_SETTINGS PluginStore;
SIZE_T PluginStoreLength = 0;

VOID FreePluginStore()
{
	ExFreePoolWithTag(PluginStore, PLUGIN_STORE_POOL_TAG);
	PluginStoreLength = 0;
}

NTSTATUS VFSRegisterPlugin(_In_ PVFS_PLUGIN Plugin)
{
	SIZE_T count = PluginStoreLength + 1;
	PVFS_PLUGIN_SETTINGS tmp = (PVFS_PLUGIN_SETTINGS)ExAllocatePoolWithTag(PagedPool, count * sizeof(VFS_PLUGIN_SETTINGS), PLUGIN_STORE_POOL_TAG);
	for (int i0 = 0; i0 < count - 1; i0++)
	{
		tmp[i0] = PluginStore[i0];
	}
	tmp[count - 1] =
	{
		.Plugin = Plugin,
		.IsEnabled = true,
	};
	FreePluginStore();
	PluginStore = tmp;
	PluginStoreLength = count;
	return 0;
}

INT LoadPlugins(_In_ PLX_INSTANCE Instance)
{
	for (int i0 = 0; i0 < PluginStoreLength; i0++)
	{
		if (PluginStore[i0].IsEnabled)
		{
			PluginStore[i0].Plugin->InitializationCallback(Instance);
		}
	}
	return 0;
}