#pragma once

#include "../vfsplugin.h"
#include "../../um/service/lxssex.Service.Windows.h"

EXTERN_C __declspec(dllexport) PCHANNEL CreateChannel(_In_ PVFS_PLUGIN Plugin, _In_ ULONG Id, _In_ SIZE_T SectionSize);
EXTERN_C __declspec(dllexport) VOID CloseChannel(_In_ PCHANNEL Channel);