#include "lxssex.Service.Shared.hpp"

PAVL<PCHANNEL_CONTEXT> ChannelContextStore;

VOID InitializationRoutine()
{
    ChannelContextStore = AVL<PCHANNEL_CONTEXT>::InitializeAvl((AVL<PCHANNEL_CONTEXT>::AVL_COMPARE_ROUTINE)ChannelContextCompareRoutine, NULL);
}

PCHANNEL CreateChannel(_In_ PVFS_PLUGIN Plugin, _In_ ULONG Id, _In_ SIZE_T SectionSize)
{
    PCHANNEL_CONTEXT context = (PCHANNEL_CONTEXT)ExAllocatePoolWithTag(NonPagedPool, sizeof(CHANNEL_CONTEXT), TAG);
    context->Plugin = Plugin;
    context->LastEventType = ChannelOpened;
    context->LastEventTypeMutex = (PFAST_MUTEX)ExAllocatePoolWithTag(NonPagedPool, sizeof(FAST_MUTEX), TAG);
    ExInitializeFastMutex(context->LastEventTypeMutex);
    LARGE_INTEGER sectSize = { .QuadPart = (LONGLONG)SectionSize };
    ZwCreateSection(&context->SharedSection, SECTION_EXTEND_SIZE | SECTION_MAP_READ | SECTION_MAP_WRITE, NULL, &sectSize, PAGE_READWRITE, 0, NULL);
    context->Channel = (PCHANNEL)ExAllocatePoolWithTag(NonPagedPool, sizeof(CHANNEL), TAG);
    context->Channel->Id = Id;
    context->Channel->SectionSize = SectionSize;
    ChannelContextStore->Add(&context);
    return context->Channel;
}

BOOLEAN SetChannelClosedCallback(_In_ PCHANNEL Channel, _In_ PCHANNEL_CLOSING_CALLBACK Callback)
{
    PCHANNEL_CONTEXT context = FindChannelContext(Channel);
    if (context)
    {
        context->ChannelClosingCallback = Callback;
        return true;
    }
    else
    {
        return false;
    }
}

BOOLEAN CloseChannel(_In_ PCHANNEL Channel)
{
    BOOLEAN result = FALSE;
    PCHANNEL_CONTEXT context = FindChannelContext(Channel);
    if (context)
    {
        result = SetChannelEvent(context, ChannelClosed);
    }
    return result;
}

RTL_GENERIC_COMPARE_RESULTS ChannelContextCompareRoutine(_In_ PAVL<PCHANNEL_CONTEXT> table, _In_ PCHANNEL_CONTEXT* first, _In_ PCHANNEL_CONTEXT* second)
{
    UNREFERENCED_PARAMETER(table);
    if ((*first)->Channel < (*second)->Channel)
        return GenericLessThan;
    else if ((*first)->Channel > (*second)->Channel)
        return GenericGreaterThan;
    else
        return GenericEqual;
}

BOOLEAN SetChannelEvent(_In_ PCHANNEL_CONTEXT Context, _In_ CHANNEL_EVENT_TYPE EventType)
{
    BOOLEAN result = FALSE;
    ExAcquireFastMutex(Context->LastEventTypeMutex);
    switch (EventType)
    {
    case ChannelClosed:
        if (!(Context->LastEventType & ChannelClosed))
        {
            Context->LastEventType = EventType;
            if (Context->LinuxChannelEvent)
            {
                LxpEpollFileStateUpdate(Context->LinuxChannelEvent, Message);
            }
            else
            {
                Context->LastEventType |= LinuxSide;
            }
            if (Context->WindowsChannelEvent)
            {
                KeSetEvent(Context->WindowsChannelEvent, 0, FALSE);
            }
            else
            {
                Context->LastEventType |= WindowsSide;
            }
            goto BothChannelClosed;
        }
        result = TRUE;
        break;
    case LinuxSide | ChannelClosed:
    case WindowsSide | ChannelClosed:
        if (Context->LastEventType & ChannelClosed)
        {
            Context->LastEventType |= EventType;
        BothChannelClosed:
            if (Context->LastEventType == LinuxSide | WindowsSide | ChannelClosed)
            {
                if (Context->ChannelClosingCallback) Context->ChannelClosingCallback(Context->Channel);
                ChannelContextStore->Remove(&Context);
                ZwClose(Context->SharedSection);
                ExFreePoolWithTag(Context->Channel, TAG);
                ExFreePoolWithTag(Context, TAG);
            }
        }
        else
        {
            Context->LastEventType = EventType;
        }
        result = TRUE;
        break;
    case LinuxSide | ChannelSync:
    case LinuxSide | ChannelOpened:
        if (Context->LastEventType & ChannelClosed)
        {
            result = FALSE;
        }
        else
        {
            Context->LastEventType = EventType;
            LxpEpollFileStateUpdate(Context->LinuxChannelEvent, Message);
            result = TRUE;
        }
        break;
    case WindowsSide | ChannelSync:
    case WindowsSide | ChannelOpened:
        if (Context->LastEventType & ChannelClosed)
        {
            result = FALSE;
        }
        else
        {
            Context->LastEventType = EventType;
            KeSetEvent(Context->WindowsChannelEvent, 0, FALSE);
            result = TRUE;
        }
        break;
    default:
        result = FALSE;
        break;
    }
    ExReleaseFastMutex(Context->LastEventTypeMutex);
    return result;
}

PCHANNEL_CONTEXT FindChannelContext(_In_ PCHANNEL Channel)
{
    PCHANNEL_CONTEXT search = (PCHANNEL_CONTEXT)ExAllocatePoolWithTag(NonPagedPool, sizeof(CHANNEL_CONTEXT), TAG);
    search->Channel = Channel;
    PCHANNEL_CONTEXT* pContext = ChannelContextStore->Find(&search);
    ExFreePoolWithTag(search, TAG);
    return pContext == NULL ? NULL : *pContext;
}

NTSTATUS IoctlSetEvent(_In_ PCHANNEL Channel, _In_ PVOID UserBuffer, _In_ CHANNEL_EVENT_TYPE Side)
{
    NTSTATUS status;
    __try
    {
        ProbeForRead(UserBuffer, sizeof(CHANNEL_EVENT_TYPE), sizeof(PVOID));
        PCHANNEL_EVENT_TYPE eventType = (PCHANNEL_EVENT_TYPE)UserBuffer;
        PCHANNEL_CONTEXT context = FindChannelContext(Channel);
        if (context)
        {
            switch (*eventType)
            {
            case ChannelSync:
                SetChannelEvent(context, ChannelSync | Side);
                status = STATUS_SUCCESS;
                break;
            default:
                status = STATUS_INVALID_PARAMETER;
                break;
            }
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
    return status;
}

NTSTATUS IoctlGetEvent(_In_ PCHANNEL Channel, _In_ PVOID UserBuffer)
{
    NTSTATUS status;
    __try
    {
        ProbeForWrite(UserBuffer, sizeof(CHANNEL_EVENT_TYPE), sizeof(PVOID));
        PCHANNEL_EVENT_TYPE eventType = (PCHANNEL_EVENT_TYPE)UserBuffer;
        PCHANNEL_CONTEXT context = FindChannelContext(Channel);
        *eventType = context != NULL ? context->LastEventType : ChannelClosed;
        status = STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
    }
    return status;
}