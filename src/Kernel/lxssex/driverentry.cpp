#include "vfsplugin.hpp"

INT InitializationRoutine(_In_ PLX_INSTANCE Instance)
{
    return LoadPlugins(Instance);
}

EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    NTSTATUS status;

    LXP_SUBSYSTEM_INFORMATION subsystemInformation
    {
        .MountNamespaceInitialize = InitializationRoutine
    };
    status = LxInitialize(DriverObject, &subsystemInformation);
    if (status != STATUS_TOO_LATE)
    {
        return status;
    }
    return STATUS_SUCCESS;
}