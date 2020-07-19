#pragma once

#include "../vfsplugin.h"
#include "../../um/service/lxssex.Service.Windows.h"

typedef VOID CHANNEL_CLOSING_CALLBACK(_In_ PCHANNEL Channel);
typedef CHANNEL_CLOSING_CALLBACK* PCHANNEL_CLOSING_CALLBACK;

EXTERN_C __declspec(dllexport) PCHANNEL CreateChannel(_In_ PVFS_PLUGIN Plugin, _In_ ULONG Id, _In_ SIZE_T SectionSize);
EXTERN_C __declspec(dllexport) BOOLEAN SetChannelClosedCallback(_In_ PCHANNEL Channel, _In_ PCHANNEL_CLOSING_CALLBACK Callback);
EXTERN_C __declspec(dllexport) BOOLEAN CloseChannel(_In_ PCHANNEL Channel);