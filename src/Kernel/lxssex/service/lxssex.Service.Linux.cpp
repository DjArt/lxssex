#include "lxssex.Service.Linux.hpp"

VFS_ENTRY LinuxServiceVfsEntries[1] =
{
    {
        .Type = VfsNodeEntry,
        .Name = RTL_CONSTANT_STRING(L"/dev/lxssex"),
        .Data = {
            .Node = {
                .Uid = 0,
                .Gid = 0,
                .Mode = S_IFCHR | S_IWUSR | S_IRUSR | S_IROTH | S_IWOTH,
                .MinorDeviceId = 300,
                .MajorDeviceId = 29
            }
        }
    }
};

VFS_MINOR_DEVICE_CALLBACKS LinuxServiceDeviceCallbacks =
{
    .Open = LinuxServiceOpen,
    .Uninitialize = LinuxServiceUninitialize
};

VFS_FILE_CALLBACKS LinuxServiceFileCallbacks =
{
    .Delete = LinuxServiceDelete,
    .MapManual = LinuxServiceMmap,
    .Ioctl = LinuxServiceIoctl
};

NTSTATUS LinuxServiceInitialize(_In_ PLX_INSTANCE instance)
{
    LONG error;
    PLINUX_SERVICE_DEVICE device = (PLINUX_SERVICE_DEVICE)(VfsDeviceMinorAllocate(&LinuxServiceDeviceCallbacks, sizeof(LINUX_SERVICE_DEVICE)));

    if (device == NULL) return LxpUtilTranslateStatus(STATUS_INSUFFICIENT_RESOURCES);

    device->LxInstance = instance;

    LxpDevMiscRegister(instance, &device->Base, 6578);
    // Registration takes a reference, drop the allocation one
    VfsDeviceMinorDereference(&device->Base);

    // Initialize the custom VFS entries
    error = VfsInitializeStartupEntries(instance, LinuxServiceVfsEntries, RTL_NUMBER_OF(LinuxServiceVfsEntries));

    return error;
}

VOID LinuxServiceUninitialize(_In_ PVFS_MINOR_DEVICE Device)
{
    UNREFERENCED_PARAMETER(Device);
}

INT LinuxServiceOpen(_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_MINOR_DEVICE Device, _In_ ULONG Flags, _Out_ PVFS_FILE* File)
{
    UNREFERENCED_PARAMETER(CallContext);
    UNREFERENCED_PARAMETER(Flags);

    PLINUX_SERVICE_DEVICE device = (PLINUX_SERVICE_DEVICE)Device;
    PLINUX_SERVICE_FILE file = (PLINUX_SERVICE_FILE)VfsFileAllocate(sizeof(LINUX_SERVICE_FILE), &LinuxServiceFileCallbacks);

    file->Device = device;
    VfsFileSetMmFileContext(file, &file->MmapFileContext);

    *File = file;
    return LxpUtilTranslateStatus(STATUS_SUCCESS);
}

INT LinuxServiceDelete(_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File)
{
    UNREFERENCED_PARAMETER(CallContext);

    PLINUX_SERVICE_FILE file = (PLINUX_SERVICE_FILE)File;
    PCHANNEL_CONTEXT context = FindChannelContext(file->Channel);
    if (context) SetChannelEvent(context, LinuxSide | ChannelClosed);
    file->Channel = NULL;

    return LxpUtilTranslateStatus(STATUS_SUCCESS);
}

INT LinuxServiceMmap(PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File, _Inout_ PVOID* Unk0, _In_ LARGE_INTEGER Length, _In_ VFS_FILE_MMAP_PROTECTION Protection, _In_ VFS_FILE_MMAP_FLAGS Flags, _In_ OFFSET_T Offset, _In_ ULONG End)
{
    NTSTATUS status;
    PLINUX_SERVICE_FILE file = (PLINUX_SERVICE_FILE)File;

    if (file->Channel)
    {
        PCHANNEL_CONTEXT context = FindChannelContext(file->Channel);
        if (context)
        {
            PLX_PROCESS picoContext;
            LARGE_INTEGER size;
            size.QuadPart = (Length.QuadPart + 4095) & 0xFFFFFFFFFFFFF000ui64;

            if (!LxpProcessGetCurrent(&picoContext))
            {
                status = LxpMmAllocateMapVm(CallContext, &picoContext->Internal, context->SharedSection, File, Unk0, Length, Offset, size, Protection, Flags, End);
                if (NT_SUCCESS(status))
                {
                    SetChannelEvent(context, LinuxSide | ChannelOpened);
                }
            }
            else
            {
                status = STATUS_DATA_ERROR;
            }
        }
        else
        {
            status = STATUS_FILE_FORCED_CLOSED;
        }
    }
    else
    {
        status = STATUS_INVALID_PARAMETER;
    }

    return LxpUtilTranslateStatus(status);
}

INT LinuxServiceIoctl(_In_ PVFS_CALL_CONTEXT CallContext, _In_ PVFS_FILE File, _In_ ULONG Ioctl, _Inout_ PVOID Buffer)
{
    UNREFERENCED_PARAMETER(CallContext);

    NTSTATUS status;
    PLINUX_SERVICE_FILE file = (PLINUX_SERVICE_FILE)File;

    switch (Ioctl)
    {
    case LXSSEX_SERVICE_IOCTL::OpenChannel:
        if (file->Channel == NULL)
        {
            __try
            {
                ProbeForRead(Buffer, sizeof(CHANNEL_REGISTRATION), sizeof(PVOID));
                ProbeForWrite(Buffer, sizeof(CHANNEL_REGISTRATION), sizeof(PVOID));
                PCHANNEL_REGISTRATION registration = (PCHANNEL_REGISTRATION)Buffer;
                PCHANNEL_CONTEXT context = FindChannelContext((PCHANNEL)registration->ChannelHandle);
                if (context != NULL)
                {
                    file->Channel = context->Channel;
                    registration->Channel = *context->Channel;
                    status = STATUS_SUCCESS;
                }
                else
                {
                    status = STATUS_INVALID_PARAMETER;
                }
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();
            }
        }
        else
        {
            status = STATUS_DEVICE_ALREADY_ATTACHED;
        }
        break;

    case LXSSEX_SERVICE_IOCTL::SetEvent:
        if (file->Channel)
        {
            status = IoctlSetEvent(file->Channel, Buffer, LinuxSide);
        }
        else
        {
            status = STATUS_DEVICE_NOT_READY;
        }
        break;

    case LXSSEX_SERVICE_IOCTL::GetEvent:
        if (file->Channel)
        {
            status = IoctlGetEvent(file->Channel, Buffer);
        }
        else
        {
            status = STATUS_DEVICE_NOT_READY;
        }
        break;

    default:
        status = STATUS_INVALID_PARAMETER;
    }

    return LxpUtilTranslateStatus(status);
}