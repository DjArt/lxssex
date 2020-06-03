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
                .MinorDeviceId = 0,
                .MajorDeviceId = 29
            }
        }
    }
};

VFS_MINOR_DEVICE_CALLBACKS LinuxServiceDeviceCallbacks =
{
    LinuxServiceOpen,
    LinuxServiceUninitialize
};

VFS_FILE_CALLBACKS LinuxServiceFileCallbacks =
{
    LinuxServiceDelete,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    LinuxServiceMmap,
    LinuxServiceIoctl
};

INT LinuxServiceInitialize(_In_ PLX_INSTANCE instance)
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

INT LinuxServiceOpen(_In_ PVOID CallContext, _In_ PVFS_MINOR_DEVICE Device, _In_ ULONG Flags, _Out_ PVFS_FILE* File)
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

INT LinuxServiceDelete(_In_ PVOID CallContext, _In_ PVFS_FILE File)
{
    UNREFERENCED_PARAMETER(CallContext);

    PLINUX_SERVICE_FILE file = (PLINUX_SERVICE_FILE)File;
    SetChannelEvent(file->Channel, LinuxSide | ChannelClosed);
    file->Channel = NULL;

    return LxpUtilTranslateStatus(STATUS_SUCCESS);
}

INT LinuxServiceMmap(PVOID CallContext, _In_ PVFS_FILE File, _Inout_ PVOID* Unk0, _In_ LARGE_INTEGER Length, _In_ ULONG Protection, _In_ INT Flags, _In_ LARGE_INTEGER Start, _In_ ULONG End)
{
    NTSTATUS status;
    PLINUX_SERVICE_FILE file = (PLINUX_SERVICE_FILE)File;

    if (file->Channel)
    {
        PCHANNEL_CONTEXT context = FindChannelContext(file->Channel);
        if (context)
        {
            PLX_PICOCONTEXT picoContext;
            LARGE_INTEGER size;
            size.QuadPart = (Length.QuadPart + 4095) & 0xFFFFFFFFFFFFF000ui64;

            if (!LxpProcessGetCurrent(&picoContext))
            {
                status = LxpMmAllocateMapVm(CallContext, &picoContext->field_C0, context->SharedSection, File, Unk0, Length, Start, size, Protection, Flags, End);
                if (NT_SUCCESS(status))
                {
                    SetChannelEvent(file->Channel, LinuxSide | ChannelOpened);
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

INT LinuxServiceIoctl(_In_ PVOID CallContext, _In_ PVFS_FILE File, _In_ ULONG Ioctl, _Inout_ PVOID Buffer)
{
    UNREFERENCED_PARAMETER(CallContext);

    NTSTATUS status;
    PLINUX_SERVICE_FILE file = (PLINUX_SERVICE_FILE)File;

    switch (Ioctl)
    {
    case LINUX_SERVICE_FILE_IOCTL::OpenChannel:
        if (file->Channel == NULL)
        {
            __try
            {
                ProbeForRead(Buffer, sizeof(CHANNEL_REGISTRATION), sizeof(ULONG));
                ProbeForWrite(Buffer, sizeof(CHANNEL_REGISTRATION), sizeof(ULONG));
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

    case LINUX_SERVICE_FILE_IOCTL::SetEvent:
        if (file->Channel)
        {
            __try
            {
                ProbeForRead(Buffer, sizeof(CHANNEL_EVENT_TYPE), sizeof(ULONG));
                PCHANNEL_EVENT_TYPE eventType = (PCHANNEL_EVENT_TYPE)Buffer;
                switch (*eventType)
                {
                case ChannelSync:
                    SetChannelEvent(file->Channel, LinuxSide | ChannelSync);
                    status = STATUS_SUCCESS;
                    break;
                default:
                    status = STATUS_INVALID_PARAMETER;
                    break;
                }
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();
            }
        }
        else
        {
            status = STATUS_DEVICE_NOT_READY;
        }
        break;

    case LINUX_SERVICE_FILE_IOCTL::GetEvent:
        if (file->Channel)
        {
            __try
            {
                ProbeForWrite(Buffer, sizeof(CHANNEL_EVENT_TYPE), sizeof(ULONG));
                PCHANNEL_EVENT_TYPE eventType = (PCHANNEL_EVENT_TYPE)Buffer;
                PCHANNEL_CONTEXT context = FindChannelContext(file->Channel);
                *eventType = context != NULL ? context->LastEventType : ChannelClosed;
                status = STATUS_SUCCESS;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();
            }
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