#include "vfsplugin.hpp"

LIST_ENTRY PluginStore;
FAST_MUTEX PluginStoreSync;

VOID InitializeVfsPluginSystem()
{
	InitializeListHead(&PluginStore);
	ExInitializeFastMutex(&PluginStoreSync);
}

NTSTATUS VfsRegisterPlugin(_In_ PVFS_PLUGIN Plugin, _In_ PVFS_PLUGIN_CALLBACKS Callbacks)
{
	ExAcquireFastMutex(&PluginStoreSync);
	PVFS_PLUGIN_ENTRY entry = (PVFS_PLUGIN_ENTRY)ExAllocatePoolWithTag(NonPagedPool, sizeof(VFS_PLUGIN_ENTRY), 0);
	RtlCopyUnicodeString(&entry->Plugin.PluginName, &Plugin->PluginName);
	entry->Callbacks = *Callbacks;
	InsertHeadList(&PluginStore, &entry->ListEntry);
	ExReleaseFastMutex(&PluginStoreSync);
	return STATUS_SUCCESS;
}

NTSTATUS VfsUnregisterPlugin(_In_ PVFS_PLUGIN Plugin)
{
	ExAcquireFastMutex(&PluginStoreSync);
	PLIST_ENTRY head = &PluginStore;
	PLIST_ENTRY next;
	PVFS_PLUGIN_ENTRY search = NULL;
	for (PLIST_ENTRY current = head->Flink; current != head; current = next)
	{
		next = current->Flink;
		PVFS_PLUGIN_ENTRY entry = CONTAINING_RECORD(current, VFS_PLUGIN_ENTRY, ListEntry);
		if (RtlCompareUnicodeString(&Plugin->PluginName, &entry->Plugin.PluginName, TRUE))
		{
			search = entry;
			break;
		}
	}
	NTSTATUS status = search ? (RemoveEntryList(&search->ListEntry) ? STATUS_SUCCESS : STATUS_NOT_FOUND) : STATUS_NOT_FOUND;
	ExReleaseFastMutex(&PluginStoreSync);
	return status;
}

INT LoadPlugins(_In_ PLX_INSTANCE Instance)
{
	ExAcquireFastMutex(&PluginStoreSync);
	PLIST_ENTRY head = &PluginStore;
	PLIST_ENTRY next;
	for (PLIST_ENTRY current = head->Flink; current != head; current = next)
	{
		next = current->Flink;
		PVFS_PLUGIN_ENTRY entry = CONTAINING_RECORD(current, VFS_PLUGIN_ENTRY, ListEntry);
		NTSTATUS status = entry->Callbacks.InitializationCallback(Instance);
		if (!NT_SUCCESS(status)) return status;
	}
	ExReleaseFastMutex(&PluginStoreSync);
	return STATUS_SUCCESS;
}