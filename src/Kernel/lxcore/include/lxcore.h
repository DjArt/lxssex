#pragma once

#include <ntifs.h>
#include <wdm.h>

#include "lxmm.h"
#include "vfs.h"

#pragma warning(push)
#pragma warning(disable:4716)

#ifdef __LXCORE_STUB__
#define __I_DLLSPEC(x) __declspec(dllexport) x { }
#else
#define __I_DLLSPEC(x) NTKERNELAPI x
#endif

// Opaque types
typedef struct _LX_INSTANCE
{
    PEX_RUNDOWN_REF field_0;
    __int64 field_8;
    UINT64 field_10;
    PEX_RUNDOWN_REF field_18;
    UINT8 gap_20[8];
    UINT64 field_28;
    UINT8 gap_30[8];
    __int64 field_38;
    __int64 field_40;
    __int64 field_48;
    UINT8 gap_50[8];
    __int64 field_58;
    __int64 field_60;
    UINT32 field_68;
    UINT8 gap_6C[4];
    HANDLE RootDirectoryHandle;
    __int64 field_78;
    __int64 field_80;
    __int64 field_88;
    __int64 field_90;
    UINT8 gap_98[8];
    UINT64 field_A0;
    HANDLE field_A8;
    UINT8 gap_B0[24];
    int field_C8;
    UINT8 gap_CC[4];
    UINT64 field_D0;
    UINT8 gap_D8[16];
    const STRING** field_E8;
    __int64 field_F0;
    int field_F8;
    int field_FC;
    __int64 field_100;
    char field_108;
    UINT8 gap_109[7];
    __int64 field_110;
    __int64 field_118;
    __int64 field_120;
    __int64 field_128;
    __int64 field_130;
    UINT8 gap_138[8];
    __int64 field_140;
    __int64 field_148;
    __int64 field_150;
    PVOID field_158;
    __int64 field_160;
    PEX_RUNDOWN_REF field_168;
    __int64 field_170;
    __int64 field_178;
    UINT8 gap_180[16];
    UINT64* field_190;
    __int64 field_198;
    UINT8 gap_1A0[16];
    __int64 field_1B0;
    int field_1B8;
    int field_1BC;
    __int64 field_1C0;
    unsigned __int64 field_1C8;
    UINT8 gap_1D0[56];
    int field_208;
    int field_20C;
    __int64 field_210;
    unsigned __int64 field_218;
    UINT8 gap_220[56];
    UINT64 field_258;
    __int64 field_260;
    __int64 field_268;
    UINT64 field_270;
    UINT8 gap_278[24];
    __int64 field_290;
    __int64 field_298;
    char field_2A0;
    UINT8 gap_2A1[7];
    __int64 field_2A8;
    int field_2B0;
    int field_2B4;
    __int64 field_2B8;
    unsigned __int64 field_2C0;
    UINT8 gap_2C8[56];
    UINT64 field_300;
    UINT8 gap_308[96];
    __int64 field_368;
    UINT8 gap_370[32];
    __int64 field_390;
    __int64 field_398;
    __int64 field_3A0;
    char(__fastcall* field_3A8)(UINT64* a1, __int64 a2);
    __int64 field_3B0;
    __int64 field_3B8;
    __int64 field_3C0;
    __int64 field_3C8;
    __int64 field_3D0;
    __int64 field_3D8;
    __int64 field_3E0;
    __int64 field_3E8;
    __int64 field_3F0;
    __int64 field_3F8;
    volatile UINT64 field_400;
    UINT8 gap_408[8];
    int field_410;
    UINT8 gap_414[4];
    __int64* field_418;
    UINT8 gap_420[8];
    UINT64 field_428;
    UINT8 gap_430[8];
    __int64 field_438;
    __int64 field_440;
    __int64 field_448;
    __int64 field_450;
    UINT8 gap_458[1216];
    __int64 field_918;
    UINT8 gap_920[4];
    int field_924;
    int field_928;
    UINT8 gap_92C[20];
    int field_940;
    UINT8 gap_944[12];
    __int64 field_950;
    __int64 field_958;
    __int64 field_960;
    UINT8 gap_968[8];
    PEX_RUNDOWN_REF field_970;
    __int64 field_978;
    __int64 field_980;
    __int64 field_988;
    __int64 field_990;
    __int64 field_998;
    __int64(__fastcall* field_9A0)(__int64 a1, __int64 a2, IRP* a3, int* a4);
    __int64 field_9A8;
    char(__fastcall* field_9B0)(__int64 a1, __int64 a2);
    __int64 field_9B8;
    UINT8 gap_9C0[8];
    __int64 field_9C8;
    int field_9D0;
} LX_INSTANCE, *PLX_INSTANCE;

// Namespace Initialization Callback and Registration
typedef NTSTATUS VFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK(_In_ PLX_INSTANCE Instance);
typedef VFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK* PVFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK;
typedef struct _LXP_SUBSYSTEM_INFORMATION
{
    PVFS_MOUNT_NAMESPACE_INITIALIZE_CALLBACK MountNamespaceInitialize;
} LXP_SUBSYSTEM_INFORMATION, *PLXP_SUBSYSTEM_INFORMATION;

EXTERN_C_START

// LxCore Initialization APIs
__I_DLLSPEC(NTSTATUS NTAPI LxInitialize(_In_ PDRIVER_OBJECT DriverObject, _In_ PLXP_SUBSYSTEM_INFORMATION SubsystemInformation));
__I_DLLSPEC(LXSTATUS NTAPI VfsInitializeStartupEntries(_In_ PLX_INSTANCE Instance, _In_ PVFS_ENTRY Entries, _In_ ULONG EntryCount));

// LxCore VFS Minor Device APIs
__I_DLLSPEC(PVFS_MINOR_DEVICE NTAPI VfsDeviceMinorAllocate(_In_ PVFS_MINOR_DEVICE_CALLBACKS MinorDeviceCallbacks, _In_ ULONG MinorDeviceContextSize));
__I_DLLSPEC(VOID NTAPI VfsDeviceMinorDereference(_In_ PVFS_MINOR_DEVICE MinorDevice));
__I_DLLSPEC(BOOLEAN NTAPI LxpDevMiscRegister(_In_ PLX_INSTANCE Instance, _In_ PVFS_MINOR_DEVICE MinorDevice, _In_ ULONG MinorDeviceNumber));

// LxCore VFS File APIs
__I_DLLSPEC(PVFS_FILE NTAPI VfsFileAllocate(_In_ ULONG FileSize, _In_ PVFS_FILE_CALLBACKS FileCallbacks));
__I_DLLSPEC(VOID NTAPI VfsFileSetEpollState(_In_ PVFS_FILE File, _In_ PVFS_FILE_EPOLL_STATE EpollState));
__I_DLLSPEC(VOID NTAPI VfsFileSetMmFileContext(_In_ PVFS_FILE File, _In_ PVFS_FILE_MMAP_CONTEXT MmapContext));

// LxCore Epoll APIs
__I_DLLSPEC(PVOID NTAPI LxpEpollFileStateCleanup(_In_ PVFS_FILE_EPOLL_STATE EpollState));
__I_DLLSPEC(PVOID NTAPI LxpEpollFileStateInitialize(_In_ PVFS_FILE_EPOLL_STATE EpollState, _In_ VFS_FILE_EPOLL_STATE_FLAGS Flags));
__I_DLLSPEC(PVOID NTAPI LxpEpollFileStateUpdate(_In_ PVFS_FILE_EPOLL_STATE EpollState, _In_ VFS_FILE_EPOLL_STATE_FLAGS Flags));

// LxCore Mmap APIs
__I_DLLSPEC(NTSTATUS NTAPI LxpMmAllocateMapVm(_In_ PVFS_CALL_CONTEXT CallContext, _In_ PLX_PROCESS_INTERNAL ProcessInternal, _In_ HANDLE SectionHandle, _In_ PVFS_FILE File, _Out_ PVOID* BaseAddress, _In_ LARGE_INTEGER Length, _In_ OFFSET_T Offset, _In_ LARGE_INTEGER RegionSize, _In_ VFS_FILE_MMAP_PROTECTION Protection, _In_ VFS_FILE_MMAP_FLAGS Flags, _In_ ULONG End));
__I_DLLSPEC(PVOID NTAPI LxpMmFileContextCleanup(_In_ PVFS_FILE_MMAP_CONTEXT MmapContext));
__I_DLLSPEC(PVOID NTAPI LxpMmFileContextInitialize(_In_ PVFS_FILE_MMAP_CONTEXT MmapContext));

// LxCore Process APIs
__I_DLLSPEC(BOOLEAN NTAPI LxpProcessGetCurrent(PLX_PROCESS* Process));

// LxCore Utility APIs
__I_DLLSPEC(NTSTATUS NTAPI LxpUtilCreateSection(PHANDLE SectionHandle, PLARGE_INTEGER MaximumSize, HANDLE FileHandle, INT64 a3));
__I_DLLSPEC(NTSTATUS NTAPI LxpUtilCreateTemporaryDirectory(HANDLE DirectoryHandle, volatile UINT64* idk, PUNICODE_STRING Name, PHANDLE FileHandle));
__I_DLLSPEC(NTSTATUS NTAPI LxpUtilCreateTemporarySparseFile(PUNICODE_STRING FileName, HANDLE DirectoryHandle, PFILE_ALLOCATION_INFORMATION FileInformation, PHANDLE FileHandle));
__I_DLLSPEC(NTSTATUS NTAPI LxpUtilNameGeneratorNew(volatile UINT64 *a1, PUNICODE_STRING a2, const PUNICODE_STRING a3));

__I_DLLSPEC(LXSTATUS NTAPI LxpUtilTranslateStatus(_In_ NTSTATUS Status));

EXTERN_C_END

#pragma warning(pop)