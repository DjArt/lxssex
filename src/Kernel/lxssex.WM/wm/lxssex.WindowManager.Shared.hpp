#pragma once

#include <avl.hpp>

#include "../include/km/wm/lxssex.WindowManager.h"

#define TAG 'MWXL'

typedef struct _WM_CHANNEL_PAIR_CONTEXT
{
    FAST_MUTEX UpdateMutex;
    PVFS_PLUGIN Plugin;
    PWM_CHANNEL_PAIR RegistrationContext;
    PWM_CHANNEL_PAIR ChannelPair;
    HANDLE ClientPID;
    HANDLE ServerPID;
} WM_CHANNEL_PAIR_CONTEXT, * PWM_CHANNEL_PAIR_CONTEXT;

VOID InitializationRoutine();
PWM_CHANNEL_PAIR OpenChannelPair(_In_ PVFS_PLUGIN Plugin, _In_ PWM_CHANNEL_PAIR RegistrationContext, _In_ WM_CHANNEL_PAIR_FUNCTION ChannelPairFunction);
PWM_CHANNEL_PAIR_CONTEXT FindContext(_In_ PWM_CHANNEL_PAIR ChannelPair);
PWM_CHANNEL_PAIR_CONTEXT FindContextByChannel(_In_ PCHANNEL Channel);

NTSTATUS IoctlOpenChannelPair(PVOID UserBuffer);
NTSTATUS IoctlQueryUnhandledPairs(PVOID UserBuffer);

CHANNEL_CLOSING_CALLBACK ChannelClosingCallback;

RTL_GENERIC_COMPARE_RESULTS ChannelContextCompareRoutine(_In_ PAVL<PWM_CHANNEL_PAIR_CONTEXT> table, _In_ PWM_CHANNEL_PAIR_CONTEXT* first, _In_ PWM_CHANNEL_PAIR_CONTEXT* second);