#pragma once

#include "lxstatus.h"

typedef INT64 OFFSET_T, * POFFSET_T;

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

//Epoll events
#define EPOLLIN 0x00000001
#define EPOLLPRI 0x00000002
#define EPOLLOUT 0x00000004
#define EPOLLERR 0x00000008
#define EPOLLHUP 0x00000010
#define EPOLLNVAL 0x00000020
#define EPOLLRDNORM 0x00000040
#define EPOLLRDBAND 0x00000080
#define EPOLLWRNORM 0x00000100
#define EPOLLWRBAND 0x00000200
#define EPOLLMSG 0x00000400
#define EPOLLRDHUP 0x00002000
#define EPOLLWAKEUP 0x10000000
#define EPOLLEXCLUSIVE 0x20000000
#define EPOLLONESHOT 0x40000000
#define EPOLLET 0x80000000

// Opaque types
typedef struct _VFS_INODE VFS_INODE, * PVFS_INODE;
typedef struct _VFS_CALL_CONTEXT VFS_CALL_CONTEXT, * PVFS_CALL_CONTEXT;

typedef struct _VFS_MINOR_DEVICE
{
    UINT64 Reserved[6];
    ULONG MinorDeviceNumber;
    ULONG Reserved2;
    UINT64 Reserved3[25];
} VFS_MINOR_DEVICE, * PVFS_MINOR_DEVICE;

#ifdef __cplusplus
typedef struct _VFS_FILE
{

} VFS_FILE, * PVFS_FILE;
#else
typedef struct _VFS_FILE VFS_FILE, * PVFS_FILE;
#endif

typedef struct _VFS_FILE_EPOLL_STATE
{
    UINT64 Unknown0;
    UINT64 Unknown1;
    UINT64 Unknown2;
    UINT64 Unknown3;
    UINT64 Unknown4;
    UINT64 Unknown5;
    UINT64 Unknown6;
} VFS_FILE_EPOLL_STATE, * PVFS_FILE_EPOLL_STATE;

typedef struct _VFS_FILE_MMAP_CONTEXT
{
    EX_PUSH_LOCK PushLock;
    PVOID MmapContext;
} VFS_FILE_MMAP_CONTEXT, * PVFS_FILE_MMAP_CONTEXT;

#ifdef __cplusplus
typedef enum _VFS_FILE_EPOLL_STATE_FLAGS : UINT64
{
    In = EPOLLIN,
    PRI = EPOLLPRI,
    Out = EPOLLOUT,
    Error = EPOLLERR,
    HangUp = EPOLLHUP,
    NoValue = EPOLLNVAL,
    RDNORM = EPOLLRDNORM,
    RDBAND = EPOLLRDBAND,
    WRNORM = EPOLLWRNORM,
    WRBAND = EPOLLWRBAND,
    Message = EPOLLMSG,
    RDHUP = EPOLLRDHUP,
    Exclusive = EPOLLEXCLUSIVE,
    OneShot = EPOLLONESHOT,
    EdgeTriggered = EPOLLET
} VFS_FILE_EPOLL_STATE_FLAGS, * PVFS_FILE_EPOLL_STATE_FLAGS;
#else
typedef UINT64 VFS_FILE_EPOLL_STATE_FLAGS, * PVFS_FILE_EPOLL_STATE_FLAGS;
#endif

typedef enum _VFS_FILE_MMAP_FLAGS
{

} VFS_FILE_MMAP_FLAGS, * PVFS_FILE_MMAP_FLAGS;

typedef enum _VFS_FILE_MMAP_PROTECTION
{
    
} VFS_FILE_MMAP_PROTECTION, * PVFS_FILE_MMAP_PROTECTION;

typedef struct _VFS_FILE_IO_VECTOR_BUFFER
{
    PVOID Buffer;
    SIZE_T Length;
} VFS_FILE_IO_VECTOR_BUFFER, * PVFS_FILE_IO_VECTOR_BUFFER;

typedef struct _VFS_FILE_IO_VECTOR
{
    INT Count;
    PVFS_FILE_IO_VECTOR_BUFFER Data;
} VFS_FILE_IO_VECTOR, * PVFS_FILE_IO_VECTOR;

// VFS Minor Device Callbacks
typedef LXSTATUS(VFS_MINOR_DEVICE_OPEN_CALLBACK) (_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_MINOR_DEVICE Device, _In_ ULONG OpenFlags, _Out_ PVFS_FILE* File);
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
typedef LXSTATUS(VFS_FILE_DELETE_CALLBACK) (_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File);
typedef LXSTATUS(VFS_FILE_FLUSH_CALLBACK) (_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File);
typedef LXSTATUS(VFS_FILE_IOCTL_CALLBACK) (_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File, _In_ ULONG Ioctl, _Inout_ PVOID Buffer);
typedef LXSTATUS(VFS_FILE_MMAP_CALLBACK) (_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File, _Inout_ PVOID* BaseAddress, LARGE_INTEGER Length, VFS_FILE_MMAP_PROTECTION Protection, VFS_FILE_MMAP_FLAGS Flags, OFFSET_T Offset, ULONG End);
typedef LXSTATUS(VFS_FILE_READ_CALLBACK) (_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File, _Inout_ PVOID Buffer, _In_ SIZE_T Length, _In_ POFFSET_T Offset, _Out_ PSIZE_T BytesTransferred);
typedef LXSTATUS(VFS_FILE_READ_VECTOR_CALLBACK) (_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File, _Inout_ PVFS_FILE_IO_VECTOR IoVector, _In_ SIZE_T Length, _In_ POFFSET_T Offset, _Out_ PSIZE_T BytesTransferred);
typedef LXSTATUS(VFS_FILE_RELEASE_CALLBACK) (_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File);
typedef LXSTATUS(VFS_FILE_SEEK_CALLBACK) (_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File, _In_ OFFSET_T Offset, _In_ INT Whence, _Out_ POFFSET_T ResultOffset);
typedef LXSTATUS(VFS_FILE_WRITE_CALLBACK) (_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File, _Inout_ PVOID Buffer, _In_ SIZE_T Length, _In_ POFFSET_T Offset, _Out_ PSIZE_T BytesTransferred);
typedef LXSTATUS(VFS_FILE_WRITE_VECTOR_CALLBACK) (_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File, _Inout_ PVFS_FILE_IO_VECTOR IoVector, _In_ SIZE_T Length, _In_ POFFSET_T Offset, _Out_ PSIZE_T BytesTransferred);

typedef VFS_FILE_DELETE_CALLBACK* PVFS_FILE_DELETE_CALLBACK;
typedef VFS_FILE_FLUSH_CALLBACK* PVFS_FILE_FLUSH_CALLBACK;
typedef VFS_FILE_IOCTL_CALLBACK* PVFS_FILE_IOCTL_CALLBACK;
typedef VFS_FILE_MMAP_CALLBACK* PVFS_FILE_MMAP_CALLBACK;
typedef VFS_FILE_READ_CALLBACK* PVFS_FILE_READ_CALLBACK;
typedef VFS_FILE_READ_VECTOR_CALLBACK* PVFS_FILE_READ_VECTOR_CALLBACK;
typedef VFS_FILE_RELEASE_CALLBACK* PVFS_FILE_RELEASE_CALLBACK;
typedef VFS_FILE_SEEK_CALLBACK* PVFS_FILE_SEEK_CALLBACK;
typedef VFS_FILE_WRITE_CALLBACK* PVFS_FILE_WRITE_CALLBACK;
typedef VFS_FILE_WRITE_VECTOR_CALLBACK* PVFS_FILE_WRITE_VECTOR_CALLBACK;

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
} VFS_ENTRY_TYPE, *PVFS_ENTRY_TYPE;

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
    } Data;
} VFS_ENTRY, *PVFS_ENTRY;