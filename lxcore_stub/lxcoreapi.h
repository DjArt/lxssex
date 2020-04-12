#pragma once

#pragma warning(push)
#pragma warning(disable:4716)

#ifdef __LXCORE_STUB__
#define __I_DLLSPEC(x) __declspec(dllexport) x { }
#else
#define __I_DLLSPEC(x) NTKERNELAPI x
#endif

// Opaque types
typedef struct _LX_INSTANCE* PLX_INSTANCE;

// Namespace Initialization Callback and Registration
typedef INT(VFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK) (_In_ PLX_INSTANCE Instance);
typedef VFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK* PVFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK;
typedef struct _LXP_SUBSYSTEM_INFORMATION
{
    PVFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK MountNamespaceInitialize;
} LXP_SUBSYSTEM_INFORMATION, *PLXP_SUBSYSTEM_INFORMATION;

EXTERN_C_START

// LxCore Initialization APIs
__I_DLLSPEC(NTSTATUS NTAPI LxInitialize(_In_ PDRIVER_OBJECT DriverObject, _In_ PLXP_SUBSYSTEM_INFORMATION SubsystemInformation));
__I_DLLSPEC(INT NTAPI VfsInitializeStartupEntries(_In_ PLX_INSTANCE Instance, _In_ PVFS_ENTRY Entries, _In_ ULONG EntryCount));

// LxCore VFS Minor Device APIs
__I_DLLSPEC(PVFS_MINOR_DEVICE NTAPI VfsDeviceMinorAllocate(_In_ PVFS_MINOR_DEVICE_CALLBACKS MinorDeviceCallbacks, _In_ ULONG MinorDeviceContextSize));
__I_DLLSPEC(VOID NTAPI VfsDeviceMinorDereference(_In_ PVFS_MINOR_DEVICE MinorDevice));
__I_DLLSPEC(BOOLEAN NTAPI LxpDevMiscRegister(_In_ PLX_INSTANCE Instance, _In_ PVFS_MINOR_DEVICE MinorDevice, _In_ ULONG MinorDeviceNumber));

// LxCore VFS File APIs
__I_DLLSPEC(PVOID NTAPI VfsFileAllocate(_In_ ULONG FileContextSize, _In_ PVFS_FILE_CALLBACKS FileCallbacks));
__I_DLLSPEC(VOID NTAPI VfsFileSetMmFileContext(_In_ PVOID File, _In_ PVOID MMFileContext));

// LXCore File APIs
__I_DLLSPEC(PVOID NTAPI LxpEpollFileStateInitialize(_In_ PVOID file, _In_ UINT64 flags));
__I_DLLSPEC(PVOID NTAPI LxpEpollFileStateUpdate(_In_ PVOID file, _In_ UINT64 flags));

// LxCore Utility APIs
__I_DLLSPEC(INT NTAPI LxpUtilTranslateStatus(_In_ NTSTATUS Status));
__I_DLLSPEC(NTSTATUS NTAPI LxpUtilCreateSection(PHANDLE SectionHandle, PLARGE_INTEGER MaximumSize, HANDLE FileHandle, INT64 a3));
__I_DLLSPEC(NTSTATUS NTAPI LxpUtilNameGeneratorNew(volatile UINT64 *a1, PUNICODE_STRING a2, const PUNICODE_STRING a3));
__I_DLLSPEC(NTSTATUS NTAPI LxpUtilCreateTemporarySparseFile(PUNICODE_STRING FileName, HANDLE DirectoryHandle, PFILE_ALLOCATION_INFORMATION FileInformation, PHANDLE FileHandle));
__I_DLLSPEC(NTSTATUS NTAPI LxpUtilCreateTemporaryDirectory(HANDLE DirectoryHandle, volatile UINT64* idk, PUNICODE_STRING Name, PHANDLE FileHandle));

//LxCore Memory API
__I_DLLSPEC(INT NTAPI LxpMmAllocateMapVm(PVOID CallContext, PLX_PICOCONTEXT_MEMORY a1, HANDLE SectionHandle, PVOID FileContext, PVOID a4, LARGE_INTEGER Length, LARGE_INTEGER a6, LARGE_INTEGER SectionMaximumSize, ULONG a8, INT a9, ULONG Protect));

__I_DLLSPEC(BOOLEAN NTAPI LxpProcessGetCurrent(PLX_PICOCONTEXT* context));

EXTERN_C_END

#pragma warning(pop)