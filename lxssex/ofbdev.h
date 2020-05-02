#pragma once

#include "default.h"
//
// DEFINES
//

#define NTDEVICE_NAME_STRING      L"\\Device\\lxssex"
#define SYMBOLIC_NAME_STRING      L"\\DosDevices\\lxssex"
#define TAG (ULONG)'TEVE'

//
// DATA
//
typedef struct _DEVICE_EXTENSION {
    PDEVICE_OBJECT  Self;
    LIST_ENTRY      EventQueueHead; // where all the user notification requests are queued
    KSPIN_LOCK      QueueLock;
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;


typedef struct _NOTIFY_RECORD {
    LIST_ENTRY      ListEntry;
        PIRP        PendingIrp;
    KDPC            Dpc;
    KTIMER          Timer;
    PFILE_OBJECT    FileObject;
    PDEVICE_EXTENSION   DeviceExtension;
    BOOLEAN         CancelRoutineFreeMemory;
} NOTIFY_RECORD, * PNOTIFY_RECORD;

typedef struct _FILE_CONTEXT {
    //
    // Lock to rundown threads that are dispatching I/Os on a file handle 
    // while the cleanup for that handle is in progress.
    //
    IO_REMOVE_LOCK  FileRundownLock;
} FILE_CONTEXT, * PFILE_CONTEXT;

//
// Function prototypes
//


DRIVER_INITIALIZE OfbEntry;
_Dispatch_type_(IRP_MJ_CREATE)
_Dispatch_type_(IRP_MJ_CLOSE)
DRIVER_DISPATCH OfbCreateClose;
_Dispatch_type_(IRP_MJ_CLEANUP)
DRIVER_DISPATCH OfbCleanup;
_Dispatch_type_(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH OfbDispatchIoControl;
DRIVER_UNLOAD OfbUnload;
DRIVER_CANCEL OfbCancelRoutine;
KDEFERRED_ROUTINE CustomTimerDPC;
DRIVER_DISPATCH RegisterEventBasedNotification;
DRIVER_DISPATCH RegisterIrpBasedNotification;

typedef struct _REGISTER_EVENT
{
    HANDLE  hEvent;
    LARGE_INTEGER DueTime; // requested DueTime in 100-nanosecond units

} REGISTER_EVENT, * PREGISTER_EVENT;

#define SIZEOF_REGISTER_EVENT  sizeof(REGISTER_EVENT )

#define IOCTL_REGISTER_EVENT \
   CTL_CODE( FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS )