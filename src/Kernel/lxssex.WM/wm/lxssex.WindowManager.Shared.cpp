#include "lxssex.WindowManager.Shared.hpp"

PAVL<PWM_CHANNEL_PAIR_CONTEXT> ContextStore;

extern VFS_PLUGIN WindowManager;

VOID InitializationRoutine()
{
    ContextStore = AVL<PWM_CHANNEL_PAIR_CONTEXT>::InitializeAvl((AVL<PWM_CHANNEL_PAIR_CONTEXT>::AVL_COMPARE_ROUTINE)ChannelContextCompareRoutine, NULL);
}

PWM_CHANNEL_PAIR OpenChannelPair(_In_ PVFS_PLUGIN Plugin, _In_ PWM_CHANNEL_PAIR RegistrationContext, _In_ WM_CHANNEL_PAIR_FUNCTION ChannelPairFunction)
{
    PWM_CHANNEL_PAIR_CONTEXT context = (PWM_CHANNEL_PAIR_CONTEXT)ExAllocatePoolWithTag(NonPagedPool, sizeof(WM_CHANNEL_PAIR_CONTEXT), TAG);
    context->Plugin = Plugin;
    context->RegistrationContext = RegistrationContext;
    ExInitializeFastMutex(&context->UpdateMutex);
    context->ChannelPair = (PWM_CHANNEL_PAIR)ExAllocatePoolWithTag(NonPagedPool, sizeof(WM_CHANNEL_PAIR), TAG);
    context->ChannelPair->InputChannel = CreateChannel(&WindowManager, ChannelPairFunction, 4096);
    SetChannelClosedCallback(context->ChannelPair->InputChannel, ChannelClosingCallback);
    context->ChannelPair->OutputChannel = CreateChannel(&WindowManager, ChannelPairFunction, 4096);
    SetChannelClosedCallback(context->ChannelPair->OutputChannel, ChannelClosingCallback);
    ContextStore->Add(&context);
    return context->ChannelPair;
}

VOID ChannelClosingCallback(_In_ PCHANNEL Channel)
{
    PWM_CHANNEL_PAIR_CONTEXT context = FindContextByChannel(Channel);
    ContextStore->Remove(&context);
}

PWM_CHANNEL_PAIR_CONTEXT FindContext(_In_ PWM_CHANNEL_PAIR ChannelPair)
{
    PWM_CHANNEL_PAIR_CONTEXT search = (PWM_CHANNEL_PAIR_CONTEXT)ExAllocatePoolWithTag(NonPagedPool, sizeof(PWM_CHANNEL_PAIR_CONTEXT), TAG);
    search->ChannelPair = ChannelPair;
    PWM_CHANNEL_PAIR_CONTEXT* pContext = ContextStore->Find(&search);
    ExFreePoolWithTag(search, TAG);
    return pContext == NULL ? NULL : *pContext;
}

PWM_CHANNEL_PAIR_CONTEXT FindContextByChannel(_In_ PCHANNEL Channel)
{
    ULONG count = ContextStore->Count();
    for (ULONG i0 = 0; i0 < count; i0++)
    {
        PWM_CHANNEL_PAIR_CONTEXT context = *ContextStore->GetElementByIndex(i0);
        if (context->ChannelPair->InputChannel == Channel || context->ChannelPair->OutputChannel == Channel)
        {
            return context;
        }
    }
    return NULL;
}

NTSTATUS IoctlOpenChannelPair(PVOID UserBuffer)
{
    NTSTATUS status;
    __try
    {
        ProbeForRead(UserBuffer, sizeof(WM_CHANNEL_PAIR_REGISTRATION), sizeof(PVOID));
        ProbeForWrite(UserBuffer, sizeof(WM_CHANNEL_PAIR_REGISTRATION), sizeof(PVOID));
        PWM_CHANNEL_PAIR_REGISTRATION registration = (PWM_CHANNEL_PAIR_REGISTRATION)UserBuffer;
        PWM_CHANNEL_PAIR_CONTEXT context = registration->ChannelContext ? FindContext((PWM_CHANNEL_PAIR)registration->ChannelContext) : FindContext(&registration->WindowManagerChannel);
        if (context)
        {
            if (context->ClientPID == NULL)
            {
                registration->WindowManagerChannel = *context->ChannelPair;
                registration->ChannelContext = context->ChannelPair;
                registration->RegistrationContext = context->RegistrationContext;
                ExAcquireFastMutex(&context->UpdateMutex);
                context->ServerPID = PsGetCurrentProcess();
                ExReleaseFastMutex(&context->UpdateMutex);
                status = STATUS_SUCCESS;
            }
            else
            {
                status = STATUS_DEVICE_ALREADY_ATTACHED;
            }
        }
        else
        {
            PWM_CHANNEL_PAIR_CONTEXT registrationContextContext = NULL;
            PWM_CHANNEL_PAIR registrationContext = NULL;
            PWM_CHANNEL_PAIR opened = NULL;
            switch (registration->WindowManagerChannel.Function)
            {
            case WM_CHANNEL_PAIR_FUNCTION::EGL:
            case WM_CHANNEL_PAIR_FUNCTION::GLES:
                registrationContextContext = FindContext((PWM_CHANNEL_PAIR)registration->ChannelContext);
                if (registrationContextContext == NULL) goto DEFAULT;
                registrationContext = registrationContextContext->ChannelPair;
            case WM_CHANNEL_PAIR_FUNCTION::WM:
                opened = OpenChannelPair(&WindowManager, registrationContext, registration->WindowManagerChannel.Function);
                if (opened)
                {
                    registration->ChannelContext = opened;
                    registration->WindowManagerChannel = *opened;
                    context = FindContext(opened);
                    ExAcquireFastMutex(&context->UpdateMutex);
                    context->ClientPID = PsGetCurrentProcess();
                    ExReleaseFastMutex(&context->UpdateMutex);
                    status = STATUS_SUCCESS;
                }
                else
                {
                    status = STATUS_UNSUCCESSFUL;
                }
                break;

            default:
            DEFAULT:
                status = STATUS_INVALID_PARAMETER;
                break;
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
    }
    return status;
}

NTSTATUS IoctlQueryUnhandledPairs(PVOID UserBuffer)
{
    NTSTATUS status;
    __try
    {
        ProbeForRead(UserBuffer, sizeof(WM_CHANNEL_PAIR_QUERY), sizeof(PVOID));
        PWM_CHANNEL_PAIR_QUERY query = (PWM_CHANNEL_PAIR_QUERY)UserBuffer;
        ProbeForRead(UserBuffer, sizeof(WM_CHANNEL_PAIR_QUERY) + sizeof(WM_CHANNEL_PAIR_QUERY::Buffer) * (query->Count - 1), sizeof(PVOID));
        ProbeForWrite(UserBuffer, sizeof(WM_CHANNEL_PAIR_QUERY) + sizeof(WM_CHANNEL_PAIR_QUERY::Buffer) * (query->Count - 1), sizeof(PVOID));
        ULONG count = ContextStore->Count();
        ULONG i0 = 0;
        for (ULONG i1 = 0; i0 < query->Count && i1 < count; i1++)
        {
            PWM_CHANNEL_PAIR_CONTEXT context = *ContextStore->GetElementByIndex(i1);
            if ((context->ClientPID == NULL || context->ServerPID == NULL) && (query->RegistrationContext || context->RegistrationContext == query->RegistrationContext))
            {
                query->Buffer[i0++] = *context->ChannelPair;
            }
        }
        query->Count = i0;
        status = STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
    }
    return status;
}

RTL_GENERIC_COMPARE_RESULTS ChannelContextCompareRoutine(_In_ PAVL<PWM_CHANNEL_PAIR_CONTEXT> table, _In_ PWM_CHANNEL_PAIR_CONTEXT* first, _In_ PWM_CHANNEL_PAIR_CONTEXT* second)
{
    UNREFERENCED_PARAMETER(table);
    if ((*first)->ChannelPair < (*second)->ChannelPair)
        return GenericLessThan;
    else if ((*first)->ChannelPair > (*second)->ChannelPair)
        return GenericGreaterThan;
    else
        return GenericEqual;
}