#include <wdm.h>
#include <lxcore.h>

typedef struct _VFS_PLUGIN
{
	PUNICODE_STRING PluginName;
	PVFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK InitializationCallback;
} VFS_PLUGIN, * PVFS_PLUGIN;

EXTERN_C NTSTATUS VFSRegisterPlugin(_In_ PVFS_PLUGIN Plugin);