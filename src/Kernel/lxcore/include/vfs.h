#pragma once

#include <wdm.h>

typedef INT64 OFF_T, * POFF_T;

// Minor Devices in Memory Major Class
#define KMSG_MINOR          11

// Minor Devices in Miscellanous Class
#define ADSS_MINOR          50
#define ADSS_CLIENT_MINOR   51
#define LXDRV_MINOR         0xBAD

// Major Devices
#define MEM_MAJOR           1
#define MISC_MAJOR          10

// File Types
#define S_IFMT   0170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

// Owner Rights
#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

// Group Rights
#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

// Others Rights
#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

// Opaque types
typedef struct _VFS_MINOR_DEVICE
{
    UINT64 Reserved[6];
    ULONG MinorDeviceNumber;
    ULONG Reserved2;
    UINT64 Reserved3[25];
} VFS_MINOR_DEVICE, * PVFS_MINOR_DEVICE;

typedef struct _VFS_MMAP_FILE_CONTEXT
{
    EX_PUSH_LOCK PushLock;
    PVOID MMapContext;
} VFS_MMAP_FILE_CONTEXT, * PVFS_MMAP_FILE_CONTEXT;

typedef struct _LX_FILE
{
    UINT64 Reserved1;
    PVFS_MMAP_FILE_CONTEXT MmapContext;
    PHANDLE FileHandle;
    UINT64 Reserved2[29];
} LX_FILE, * PLX_FILE;

typedef struct _VFS_INODE VFS_INODE, * PVFS_INODE;

// VFS Minor Device Callbacks
typedef INT(VFS_MINOR_DEVICE_OPEN_CALLBACK) (_In_ PVOID CallContext, _In_ PVFS_MINOR_DEVICE MinorDevice, _In_ ULONG OpenFlags, _Out_ PVOID* OpenedFile);
typedef VFS_MINOR_DEVICE_OPEN_CALLBACK* PVFS_MINOR_DEVICE_OPEN_CALLBACK;
typedef VOID(VFS_MINOR_DEVICE_UNINITIALIZE_CALLBACK) (_In_ PVFS_MINOR_DEVICE MinorDevice);
typedef VFS_MINOR_DEVICE_UNINITIALIZE_CALLBACK* PVFS_MINOR_DEVICE_UNINITIALIZE_CALLBACK;
typedef struct _VFS_MINOR_DEVICE_CALLBACKS
{
    PVFS_MINOR_DEVICE_OPEN_CALLBACK Open;
    PVFS_MINOR_DEVICE_UNINITIALIZE_CALLBACK Uninitialize;

    PVOID Reserved[6];
} VFS_MINOR_DEVICE_CALLBACKS, * PVFS_MINOR_DEVICE_CALLBACKS;

// VFS File Callbacks
typedef INT(VFS_FILE_DELETE_CALLBACK) (_In_ PVOID CallContext, _In_ PVOID FileContext);
typedef INT(VFS_FILE_FLUSH_CALLBACK) (_In_ PVOID CallContext, _In_ PVOID FileContext);
typedef INT(VFS_FILE_IOCTL_CALLBACK) (_In_ PVOID CallContext, _In_ PVOID FileContext, _In_ ULONG Ioctl, _Inout_ PVOID Buffer);
typedef INT(VFS_FILE_READ_CALLBACK) (_In_ PVOID CallContext, _In_ PVOID FileContext, _Inout_ PVOID Buffer, _In_ SIZE_T Length, _In_ POFF_T POffset, _Out_ PSIZE_T PBytesTransferred);
typedef INT(VFS_FILE_READ_VECTOR_CALLBACK) (_In_ PVOID CallContext, _In_ PVOID FileContext, _Inout_ PVOID IoVector, _In_ SIZE_T Length, _In_ POFF_T POffset, _Out_ PSIZE_T PBytesTransferred);
typedef INT(VFS_FILE_RELEASE_CALLBACK) (_In_ PVOID CallContext, _In_ PVOID FileContext);
typedef INT(VFS_FILE_SEEK_CALLBACK) (_In_ PVOID CallContext, _In_ PVOID FileContext, _In_ OFF_T POffset, _In_ INT Whence, _Out_ POFF_T PResultOffset);
typedef INT(VFS_FILE_WRITE_CALLBACK) (_In_ PVOID CallContext, _In_ PVOID FileContext, _Inout_ PVOID Buffer, _In_ SIZE_T Length, _In_ POFF_T POffset, _Out_ PSIZE_T PBytesTransferred);
typedef INT(VFS_FILE_WRITE_VECTOR_CALLBACK) (_In_ PVOID CallContext, _In_ PVOID FileContext, _Inout_ PVOID IoVector, _In_ SIZE_T Length, _In_ POFF_T POffset, _Out_ PSIZE_T PBytesTransferred);
typedef INT(VFS_FILE_MMAP_CALLBACK) (PVOID CallContext, _In_ PVOID FileContext, _Inout_ PVOID* Unk0, LARGE_INTEGER Length, ULONG Protection, INT Flags, LARGE_INTEGER Start, ULONG End);

typedef VFS_FILE_DELETE_CALLBACK* PVFS_FILE_DELETE_CALLBACK;
typedef VFS_FILE_FLUSH_CALLBACK* PVFS_FILE_FLUSH_CALLBACK;
typedef VFS_FILE_IOCTL_CALLBACK* PVFS_FILE_IOCTL_CALLBACK;
typedef VFS_FILE_READ_CALLBACK* PVFS_FILE_READ_CALLBACK;
typedef VFS_FILE_READ_VECTOR_CALLBACK* PVFS_FILE_READ_VECTOR_CALLBACK;
typedef VFS_FILE_RELEASE_CALLBACK* PVFS_FILE_RELEASE_CALLBACK;
typedef VFS_FILE_SEEK_CALLBACK* PVFS_FILE_SEEK_CALLBACK;
typedef VFS_FILE_WRITE_CALLBACK* PVFS_FILE_WRITE_CALLBACK;
typedef VFS_FILE_WRITE_VECTOR_CALLBACK* PVFS_FILE_WRITE_VECTOR_CALLBACK;
typedef VFS_FILE_MMAP_CALLBACK* PVFS_FILE_MMAP_CALLBACK;

typedef struct _VFS_FILE_CALLBACKS
{
    PVFS_FILE_DELETE_CALLBACK Delete;
    PVFS_FILE_READ_CALLBACK Read;
    PVOID ReadDir;
    PVFS_FILE_WRITE_CALLBACK Write;
    PVFS_FILE_WRITE_VECTOR_CALLBACK WriteVector;
    PVOID Map;
    PVFS_FILE_MMAP_CALLBACK MapManual;
    PVFS_FILE_IOCTL_CALLBACK Ioctl;
    PVFS_FILE_FLUSH_CALLBACK Flush;
    PVOID Sync;
    PVFS_FILE_RELEASE_CALLBACK Release;
    PVFS_FILE_READ_VECTOR_CALLBACK ReadVector;
    PVOID Truncate;
    PVFS_FILE_SEEK_CALLBACK Seek;
    PVOID FilterPollRegistration;
    PVOID FAllocate;
    PVOID GetPath;
    PVOID GetNtDeviceType;

    PVOID Reserved[14];
} VFS_FILE_CALLBACKS, * PVFS_FILE_CALLBACKS;

// VFS Entry Structure and Type
typedef enum _VFS_ENTRY_TYPE
{
    VfsDirectoryEntry,  // VfsMakeDirectory
    VfsMountEntry,      // VfsMount
    VfsNodeEntry,       // VfsMakeNode
    VfsSymlinkEntry,    // VfsMakeSymbolicLink
    VfsFileEntry        // VfsOpenFile
} VFS_ENTRY_TYPE;

typedef struct _VFS_ENTRY
{
    VFS_ENTRY_TYPE Type;
    UNICODE_STRING Name;
    union
    {
        struct
        {
            ULONG Uid;
            ULONG Gid;
            ULONG Mode;
        } Directory;
        struct
        {
            ANSI_STRING FileSystem;
            UNICODE_STRING TargetDevice;
            ANSI_STRING Options;
            ULONG Uid;
            ULONG Gid;
            ULONG Mode;
            ULONGLONG MountFlags;
            ULONG Unknown;
        } Mount;
        struct
        {
            ULONG Uid;
            ULONG Gid;
            ULONG Mode;
            // kdev_t
            ULONG MinorDeviceId : 20;
            ULONG MajorDeviceId : 12;
        } Node;
        struct
        {
            UNICODE_STRING TargetEntry;
        } Symlink;
        struct
        {
            ULONG Mode;
        } File;
    };
} VFS_ENTRY, *PVFS_ENTRY;