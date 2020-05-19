#include "default.h"
#include "fbdev.h"
#include <ntifs.h>

extern "C" VFS_ENTRY VfsFbEntries[1];

typedef struct _MMAP_C_TEST
{
    EX_PUSH_LOCK PushLock;
    PVOID MMapContext;
};

typedef struct _FB_DEVICE
{
    VFS_MINOR_DEVICE Base;
    HANDLE TempDirHandle;
    volatile UINT64 TempFileNameGenerator;
    HANDLE PrivateTMPFile;
    struct fb_var_screeninfo Var;
    struct fb_fix_screeninfo Fix;
    PUINT8 fbmem;
    OFF_T Offset;
    EX_PUSH_LOCK Lock;
} FB_DEVICE, *PFB_DEVICE;

typedef struct _FB_INFO
{
    _MMAP_C_TEST* MMFileContext;
    HANDLE BackingFileHandle;
    PFB_DEVICE AssociatedDevice;
} FB_INFO, *PFB_INFO;
//
// VFS File Callbacks for File Contexts on our Device
//
VFS_FILE_READ_CALLBACK FbRead;
VFS_FILE_WRITE_CALLBACK FbWrite;
VFS_FILE_IOCTL_CALLBACK FbIoctl;
VFS_FILE_MMAP_CALLBACK FbMmap;
VFS_FILE_CALLBACKS VfsFbCallbacks =
{
    .Read = FbRead,
    .Write = FbWrite,
    .MapManual = FbMmap,
    .Ioctl = FbIoctl,
};

//
// VFS Minor Device Callbacks for our Device
//
VFS_MINOR_DEVICE_OPEN_CALLBACK FbOpen;
VFS_MINOR_DEVICE_UNINITIALIZE_CALLBACK FbUninitialize;
VFS_MINOR_DEVICE_CALLBACKS VfsMyCallbacks =
{
    FbOpen,
    FbUninitialize
};


INT FbMmap(PVOID CallContext, _In_ PVOID FileContext, _Inout_ PVOID* Unk0, LARGE_INTEGER Length, ULONG Protection, INT Flags, LARGE_INTEGER Start, ULONG End)
{
    PFB_INFO fileContext = reinterpret_cast<PFB_INFO>(FileContext);
    PLX_PICOCONTEXT context;
    NTSTATUS status = 0;

    LARGE_INTEGER size;
    size.QuadPart = (Length.QuadPart + 4095) & 0xFFFFFFFFFFFFF000ui64;

    if (LxpProcessGetCurrent(&context))
    {
        HANDLE handle;
        /*UNICODE_STRING DestinationString;
        UNICODE_STRING v11;
        v11.Length = 512;
        v11.MaximumLength = 512;
        v11.Buffer = (PWCH)ExAllocatePoolWithTag(PagedPool, 512, 'TEST');
        FILE_ALLOCATION_INFORMATION fileInformation;
        fileInformation.AllocationSize = Length;
        RtlInitUnicodeString(&DestinationString, L"_shared");
        status = LxpUtilNameGeneratorNew(&fileContext->AssociatedDevice->TempFileNameGenerator, (PUNICODE_STRING)&v11, &DestinationString);
        if (NT_ERROR(status)) goto EXIT;
        status = LxpUtilCreateTemporarySparseFile(&v11, fileContext->AssociatedDevice->TempDirHandle, &fileInformation, &fileContext->BackingFileHandle);
        if (NT_ERROR(status)) goto EXIT;*/
        status = LxpUtilCreateSection(&handle, &size, fileContext->AssociatedDevice->PrivateTMPFile, 0);
        if (NT_ERROR(status)) goto EXIT;
        VfsFileSetMmFileContext(fileContext, fileContext->MMFileContext);
        status = LxpMmAllocateMapVm(CallContext, &context->field_C0, handle, FileContext, Unk0, Length, Start, size, Protection, Flags, End);
        if (NT_ERROR(status)) goto EXIT;
    }
EXIT:
    return LxpUtilTranslateStatus(status);
}

INT FbIoctl(_In_ PVOID CallContext, _In_ PVOID FileContext, _In_ ULONG Ioctl, _Inout_ PVOID Buffer)
{
    NTSTATUS status;
    PFB_INFO fileContext;

    //
    // Make sure this is really our file
    //
    UNREFERENCED_PARAMETER(CallContext);
    fileContext = reinterpret_cast<PFB_INFO>(FileContext);
    
    switch (Ioctl)
    {
    case FBIOGET_VSCREENINFO:
        DbgPrintEx(77, 0, "lxssex: FBIOGET_VSCREENINFO ioctl\n");
        __try 
        {
            ProbeForWrite(Buffer, sizeof(fb_var_screeninfo), sizeof(ULONG));
            memcpy_s(Buffer, sizeof(fb_var_screeninfo), &fileContext->AssociatedDevice->Var, sizeof(fb_var_screeninfo));
            status = STATUS_SUCCESS;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            status = GetExceptionCode();
            goto Quickie;
        }
        break;
    case FBIOPUT_VSCREENINFO:
        DbgPrintEx(77, 0, "lxssex: FBIOPUT_VSCREENINFO ioctl\n");
        __try
        {
            ProbeForRead(Buffer, sizeof(fb_var_screeninfo), sizeof(ULONG));
            fileContext->AssociatedDevice->Var = * reinterpret_cast<fb_var_screeninfo*>(Buffer);
            DbgPrintEx(77, 0, "lxssex: FBIOPUT_VSCREENINFO ioctl: x=%i y=%i\n", fileContext->AssociatedDevice->Var.xres, fileContext->AssociatedDevice->Var.yres);
            status = STATUS_SUCCESS;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            status = GetExceptionCode();
            goto Quickie;
        }
        break;
    case FBIOGET_FSCREENINFO:
        DbgPrintEx(77, 0, "lxssex: FBIOGET_FSCREENINFO ioctl\n");
        __try
        {
            ProbeForWrite(Buffer, sizeof(fb_fix_screeninfo), sizeof(ULONG));
            memcpy_s(Buffer, sizeof(fb_fix_screeninfo), &fileContext->AssociatedDevice->Fix, sizeof(fb_fix_screeninfo));
            status = STATUS_SUCCESS;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            status = GetExceptionCode();
            goto Quickie;
        }
        break;
    case FBIOPUTCMAP:
        DbgPrintEx(77, 0, "lxssex: FBIOPUTCMAP ioctl\n");

        break;
    case FBIOBLANK:
        DbgPrintEx(77, 0, "lxssex: FBIOBLANK ioctl\n");

        break;
    case FBIOPAN_DISPLAY:
        DbgPrintEx(77, 0, "lxssex: FBIOPAN_DISPLAY ioctl\n");

        break;
    case FBIOGET_CON2FBMAP:
        DbgPrintEx(77, 0, "lxssex: FBIOGET_CON2FBMAP ioctl\n");

        break;
    case FBIOPUT_CON2FBMAP:
        DbgPrintEx(77, 0, "lxssex: FBIOPUT_CON2FBMAP ioctl\n");

        break;
    default:
        DbgPrintEx(77, 0, "lxssex: UNKNOW ioctl\n");
        status = STATUS_INVALID_PARAMETER;
        goto Quickie;
    }

Quickie:
    //
    // Return the Linux error code
    //
    return LxpUtilTranslateStatus(status);
}

INT FbRead(_In_ PVOID CallContext, _In_ PVOID FileContext, _Inout_ PVOID Buffer, _In_ SIZE_T Length, _In_ POFF_T POffset, _Out_ PSIZE_T PBytesTransferred)
{
    PFB_INFO fileContext;

    fileContext = reinterpret_cast<PFB_INFO>(FileContext);
    OFF_T Offset, EndOffset;
    INT Error;

    *PBytesTransferred = 0;

    if (0 == POffset)
    {
        ExAcquirePushLockExclusive(&fileContext->AssociatedDevice->Lock);
        Offset = fileContext->AssociatedDevice->Offset;
    }
    else
        Offset = *POffset;
    EndOffset = Offset + Length;
    if (Offset > 1280 * 720 * 4)
        Offset = 1280 * 720 * 4;
    if (EndOffset > 1280 * 720 * 4)
        EndOffset = 1280 * 720 * 4;

    __try
    {
        ProbeForWrite(Buffer, EndOffset - Offset, 1);
        RtlCopyMemory(Buffer, fileContext->AssociatedDevice->fbmem + Offset, EndOffset - Offset);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        //Error = -EFAULT;
        Error = -14;
        goto exit;
    }

    if (0 == POffset)
    {
        fileContext->AssociatedDevice->Offset += Length;
        ExReleasePushLockExclusive(&fileContext->AssociatedDevice->Lock);
    }
    else
        *POffset += Length;

    *PBytesTransferred = Length;
    Error = 0;

exit:
    DbgPrintEx(77, 0, "(File=%p, Length=%lu, Offset=%lx, *PBytesTransferred=%lu) = %d",
        fileContext,
        (unsigned)Length,
        (unsigned)(0 != POffset ? *POffset : -1),
        (unsigned)*PBytesTransferred,
        Error);
    return Error;
}

INT FbWrite(_In_ PVOID CallContext, _In_ PVOID FileContext, _Inout_ PVOID Buffer, _In_ SIZE_T Length, _In_ POFF_T POffset, _Out_ PSIZE_T PBytesTransferred)
{
    PFB_INFO fileContext;

    fileContext = reinterpret_cast<PFB_INFO>(FileContext);
    OFF_T Offset, EndOffset;
    INT Error;

    *PBytesTransferred = 0;

    if (0 == POffset)
    {
        ExAcquirePushLockExclusive(&fileContext->AssociatedDevice->Lock);
        Offset = fileContext->AssociatedDevice->Offset;
    }
    else
        Offset = *POffset;
    EndOffset = Offset + Length;
    if (Offset > 1280 * 720 * 4)
        Offset = 1280 * 720 * 4;
    if (EndOffset > 1280 * 720 * 4)
        EndOffset = 1280 * 720 * 4;

    __try
    {
        ProbeForRead(Buffer, EndOffset - Offset, 1);
        RtlCopyMemory(fileContext->AssociatedDevice->fbmem + Offset, Buffer, EndOffset - Offset);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        //Error = -EFAULT;
        Error = -14;
        goto exit;
    }

    if (0 == POffset)
    {
        fileContext->AssociatedDevice->Offset += Length;
        ExReleasePushLockExclusive(&fileContext->AssociatedDevice->Lock);
    }
    else
        *POffset += Length;

    *PBytesTransferred = Length;
    Error = 0;

exit:
    DbgPrintEx(77,0,"(File=%p, Length=%lu, Offset=%lx, *PBytesTransferred=%lu) = %d",
        fileContext,
        (unsigned)Length,
        (unsigned)(0 != POffset ? *POffset : -1),
        (unsigned)*PBytesTransferred,
        Error);
    return Error;
}

INT FbOpen(_In_ PVOID Context, _In_ PVFS_MINOR_DEVICE Device, _In_ ULONG Flags, _Out_ PVOID* File)
{
    PFB_INFO fileContext;
    NTSTATUS status;
    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(Flags);

    DbgPrintEx(77, 0, "lxssex: FbOpen\n");

    //
    // Allocate a file context
    //
    fileContext = static_cast<PFB_INFO>(VfsFileAllocate(sizeof(*fileContext), &VfsFbCallbacks));

    if (fileContext == NULL)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Quickie;
    }

    //
    // Rememer which device we're on
    
    fileContext->AssociatedDevice = (PFB_DEVICE)Device;
    fileContext->MMFileContext = (_MMAP_C_TEST*)ExAllocatePoolWithTag(PagedPool, sizeof(_MMAP_C_TEST), 'MMAP');
    fileContext->MMFileContext->PushLock = NULL;
    fileContext->MMFileContext->MMapContext = NULL;

    //
    // Return the file back to VFS
    //
    *File = fileContext;
    status = STATUS_SUCCESS;
Quickie:
    //
    // Return the Linux error code
    //
    return LxpUtilTranslateStatus(status);
}

VOID FbUninitialize(_In_ PVFS_MINOR_DEVICE Device)
{
    DbgPrintEx(77, 0, "lxssex: FbUninitializate\n");
    //
    // Instance is being torn-down. We have nothing to do.
    //
    PFB_DEVICE dev = (PFB_DEVICE)Device;
    ExFreePoolWithTag(dev->fbmem, 'MMBF');
}

struct LX_INSTANCE_TEST
{
    struct _EX_RUNDOWN_REF field_0;
    __int64 field_8;
    UINT64 field_10;
    struct _EX_RUNDOWN_REF field_18;
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
    struct _EX_RUNDOWN_REF field_168;
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
    struct _EX_RUNDOWN_REF field_970;
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
};

INT FbInitialize(_In_ PLX_INSTANCE Instance)
{
    DbgPrintEx(77, 0, "lxssex: FbInitializate\n");
    LONG error;

    LX_INSTANCE_TEST* test = (LX_INSTANCE_TEST*)Instance;

    //
    // Allocate a minor device
    //
    DbgPrintEx(77, 0, "Try to allocate a minor device\n");
    PFB_DEVICE VfsMyDevice = (PFB_DEVICE)(VfsDeviceMinorAllocate(&VfsMyCallbacks, sizeof(FB_DEVICE)));
    if (VfsMyDevice == NULL)
    {
        DbgPrintEx(77, 0, "Try to allocate a minor device failed\n");
        error = LxpUtilTranslateStatus(STATUS_INSUFFICIENT_RESOURCES);
        goto Quickie;
    }

    VfsMyDevice->fbmem = (PUINT8)ExAllocatePoolWithQuotaTag(NonPagedPool, 1280 * 720 * 4, 'MMBF');
    int resx = 1280, resy = 720, pixsize = 4;

    VfsMyDevice->Var.xres = resx;                     /* visible resolution           */
    VfsMyDevice->Var.yres = resy;
    VfsMyDevice->Var.xres_virtual = resx;             /* virtual resolution           */
    VfsMyDevice->Var.yres_virtual = resy;
    VfsMyDevice->Var.xoffset = 0;                  /* offset from virtual to visible */
    VfsMyDevice->Var.yoffset = 0;                  /* resolution                   */
    VfsMyDevice->Var.bits_per_pixel = 32;           /* guess what                   */
    VfsMyDevice->Var.grayscale = 0;                /* 0 = color, 1 = grayscale,    */
    VfsMyDevice->Var.red.offset = 16;         /* bitfield in fb mem if true color, */
    VfsMyDevice->Var.red.length = 8;         /* bitfield in fb mem if true color, */
    VfsMyDevice->Var.green.offset = 8;       /* else only length is significant */
    VfsMyDevice->Var.green.length = 8;       /* else only length is significant */
    VfsMyDevice->Var.blue.offset = 0;
    VfsMyDevice->Var.blue.length = 8;
    VfsMyDevice->Var.transp.offset = 24;      /* transparency                 */
    VfsMyDevice->Var.transp.length = 8;      /* transparency                 */
    VfsMyDevice->Var.nonstd = 0;                   /* != 0 Non standard pixel format */
    VfsMyDevice->Var.activate = FB_ACTIVATE_NOW;                 /* see FB_ACTIVATE_*            */
    VfsMyDevice->Var.height = 450;                   /* height of picture in mm    */
    VfsMyDevice->Var.width = 800;                    /* width of picture in mm     */
    VfsMyDevice->Var.accel_flags = 0;              /* (OBSOLETE) see fb_info.flags */
    VfsMyDevice->Var.pixclock = 0;                 /* pixel clock in ps (pico seconds) */
    VfsMyDevice->Var.left_margin = 0;              /* time from sync to picture    */
    VfsMyDevice->Var.right_margin = 0;             /* time from picture to sync    */
    VfsMyDevice->Var.upper_margin = 0;             /* time from sync to picture    */
    VfsMyDevice->Var.lower_margin = 0;
    VfsMyDevice->Var.hsync_len = 0;                /* length of horizontal sync    */
    VfsMyDevice->Var.vsync_len = 0;                /* length of vertical sync      */
    VfsMyDevice->Var.sync = 0;                     /* see FB_SYNC_*                */
    VfsMyDevice->Var.vmode = FB_VMODE_NONINTERLACED;                    /* see FB_VMODE_*               */
    VfsMyDevice->Var.rotate = 0;                   /* angle we rotate counter clockwise */
    VfsMyDevice->Var.colorspace = 0;               /* colorspace for FOURCC-based modes */

    VfsMyDevice->Fix.smem_start = 0;       /* Start of frame buffer mem */
    VfsMyDevice->Fix.smem_len = resx * resy * pixsize;                 /* Length of frame buffer mem */
    VfsMyDevice->Fix.type = FB_TYPE_PACKED_PIXELS;                     /* see FB_TYPE_*                */
    VfsMyDevice->Fix.type_aux = 0;                 /* Interleave for interleaved Planes */
    VfsMyDevice->Fix.visual = FB_VISUAL_TRUECOLOR;                   /* see FB_VISUAL_*              */
    VfsMyDevice->Fix.xpanstep = 0;                 /* zero if no hardware panning  */
    VfsMyDevice->Fix.ypanstep = 0;                 /* zero if no hardware panning  */
    VfsMyDevice->Fix.ywrapstep = 0;                /* zero if no hardware ywrap    */
    VfsMyDevice->Fix.line_length = resx * pixsize;              /* length of a line in bytes    */
    VfsMyDevice->Fix.mmio_start = 0;       /* Start of Memory Mapped I/O   (physical address) */
    VfsMyDevice->Fix.mmio_len = resx * resy * pixsize;                 /* Length of Memory Mapped I/O  */
    VfsMyDevice->Fix.accel = FB_ACCEL_NONE;                    /* Indicate to driver which   specific chip/card we have  */
    VfsMyDevice->Fix.capabilities = 0;             /* see FB_CAP_*                 */

    UNICODE_STRING dirname;
    RtlInitUnicodeString(&dirname, L"_devfb");
    error = LxpUtilCreateTemporaryDirectory(test->RootDirectoryHandle, &VfsMyDevice->TempFileNameGenerator, &dirname, &VfsMyDevice->TempDirHandle);
    UNICODE_STRING DestinationString;
    UNICODE_STRING v11;
    v11.Length = 512;
    v11.MaximumLength = 512;
    v11.Buffer = (PWCH)ExAllocatePoolWithTag(PagedPool, 512, 'TEST');
    FILE_ALLOCATION_INFORMATION fileInformation;
    fileInformation.AllocationSize.QuadPart = resx * resy * pixsize;
    RtlInitUnicodeString(&DestinationString, L"_private");
    error = LxpUtilNameGeneratorNew(&VfsMyDevice->TempFileNameGenerator, (PUNICODE_STRING)&v11, &DestinationString);
    error = LxpUtilCreateTemporarySparseFile(&v11, VfsMyDevice->TempDirHandle, &fileInformation, &VfsMyDevice->PrivateTMPFile);
    //
    // Register it under /dev/misc with a minor code of 0xBAD
    //
    LxpDevMiscRegister(Instance, &VfsMyDevice->Base, 6578);

    //
    // Registration takes a reference, drop the allocation one
    //
    VfsDeviceMinorDereference(&VfsMyDevice->Base);

    //
    // Initialize the custom VFS entries
    //
    DbgPrintEx(77, 0, "Try to init custom VFS entry\n");
    error = VfsInitializeStartupEntries(Instance, VfsFbEntries, RTL_NUMBER_OF(VfsFbEntries));
    if (error != 0)
    {
        DbgPrintEx(77, 0, "Try to init custom VFS entry failed - %x\n", error);
        //goto Quickie;
    }

    //
    // Everything seems to have worked
    //
    error = 0;

Quickie:
    //
    // All done
    //
    return error;
}
