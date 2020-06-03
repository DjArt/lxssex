#include "lxssex.Service.Shared.hpp"

PAVL<PCHANNEL_CONTEXT> ChannelContextStore;

VOID InitializationRoutine()
{
    ChannelContextStore = AVL<PCHANNEL_CONTEXT>::InitializeAvl((AVL<PCHANNEL_CONTEXT>::AVL_COMPARE_ROUTINE)ChannelContextCompareRoutine, NULL);
}

EXTERN_C __declspec(dllexport) PCHANNEL CreateChannel(_In_ PVFS_PLUGIN Plugin, _In_ ULONG Id, _In_ SIZE_T SectionSize)
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

EXTERN_C __declspec(dllexport) VOID CloseChannel(_In_ PCHANNEL Channel)
{
    SetChannelEvent(Channel, ChannelClosed);
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

BOOLEAN SetChannelEvent(_In_ PCHANNEL Channel, _In_ CHANNEL_EVENT_TYPE EventType)
{
    BOOLEAN result = FALSE;
    PCHANNEL_CONTEXT context = FindChannelContext(Channel);
    if (context)
    {
        ExAcquireFastMutex(context->LastEventTypeMutex);
        switch (EventType)
        {
        case ChannelClosed:
            if (!(context->LastEventType & ChannelClosed))
            {
                context->LastEventType = EventType;
                if (context->LinuxChannelEvent)
                {
                    LxpEpollFileStateUpdate(context->LinuxChannelEvent, 0);
                }
                else
                {
                    context->LastEventType |= LinuxSide;
                }
                if (context->WindowsChannelEvent)
                {
                    KeSetEvent(context->WindowsChannelEvent, 0, FALSE);
                }
                else
                {
                    context->LastEventType |= WindowsSide;
                }
                goto BothChannelClosed;
            }
            result = TRUE;
            break;
        case LinuxSide | ChannelClosed:
        case WindowsSide | ChannelClosed:
            if (context->LastEventType & ChannelClosed)
            {
                context->LastEventType |= EventType;
            BothChannelClosed:
                if (context->LastEventType == LinuxSide | WindowsSide | ChannelClosed)
                {
                    ChannelContextStore->Remove(&context);
                    ZwClose(context->SharedSection);
                    ExFreePoolWithTag(Channel, TAG);
                    ExFreePoolWithTag(context, TAG);
                }
            }
            else
            {
                context->LastEventType = EventType;
            }
            result = TRUE;
            break;
        case LinuxSide | ChannelSync:
        case LinuxSide | ChannelOpened:
            if (context->LastEventType & ChannelClosed)
            {
                result = FALSE;
            }
            else
            {
                context->LastEventType = EventType;
                LxpEpollFileStateUpdate(context->LinuxChannelEvent, 0);
                result = TRUE;
            }
            break;
        case WindowsSide | ChannelSync:
        case WindowsSide | ChannelOpened:
            if (context->LastEventType & ChannelClosed)
            {
                result = FALSE;
            }
            else
            {
                context->LastEventType = EventType;
                KeSetEvent(context->WindowsChannelEvent, 0, FALSE);
                result = TRUE;
            }
            break;
        default:
            result = FALSE;
            break;
        }
        ExReleaseFastMutex(context->LastEventTypeMutex);
    }
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