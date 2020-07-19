#pragma once

#include <avl.hpp>

#include "../include/km/service/lxssex.Service.h"

#define TAG 'VRES'

typedef struct _CHANNEL_CONTEXT
{
    PVFS_PLUGIN Plugin;
    PCHANNEL Channel;
    HANDLE SharedSection;
    PVOID WindowsSharedSectionAddress;
    HANDLE WindowsSharedSectionOwner;
    CHANNEL_EVENT_TYPE LastEventType;
    PFAST_MUTEX LastEventTypeMutex;
    PVFS_FILE_EPOLL_STATE LinuxChannelEvent;
    PKEVENT WindowsChannelEvent;
    PCHANNEL_CLOSING_CALLBACK ChannelClosingCallback;
} CHANNEL_CONTEXT, * PCHANNEL_CONTEXT;

VOID InitializationRoutine();

BOOLEAN SetChannelEvent(_In_ PCHANNEL_CONTEXT Context, _In_ CHANNEL_EVENT_TYPE EventType);
PCHANNEL_CONTEXT FindChannelContext(_In_ PCHANNEL Channel);

NTSTATUS IoctlSetEvent(_In_ PCHANNEL Channel, _In_ PVOID UserBuffer, _In_ CHANNEL_EVENT_TYPE Side);
NTSTATUS IoctlGetEvent(_In_ PCHANNEL Channel, _In_ PVOID UserBuffer);

RTL_GENERIC_COMPARE_RESULTS ChannelContextCompareRoutine(_In_ PAVL<PCHANNEL_CONTEXT> table, _In_ PCHANNEL_CONTEXT* first, _In_ PCHANNEL_CONTEXT* second);