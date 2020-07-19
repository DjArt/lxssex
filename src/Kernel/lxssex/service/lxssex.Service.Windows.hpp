#pragma once

#include "lxssex.Service.Shared.hpp"

#define NTDEVICE_SERVICE_NAME L"\\Device\\lxssex"
#define SYMBOLIC_SERVICE_NAME L"\\??\\lxssex"

#define INITGUID
#include <guiddef.h>
// {7D386D44-3EDE-492A-B486-1330596DECA3}
DEFINE_GUID(LXSSEX_INTERFACE_GUID, 0x7d386d44, 0x3ede, 0x492a, 0xb4, 0x86, 0x13, 0x30, 0x59, 0x6d, 0xec, 0xa3);
#undef INITGUID

typedef struct _WINDOWS_SERVICE_DEVICE
{
    UNICODE_STRING InterfaceSymbolicLinkName;
} WINDOWS_SERVICE_DEVICE, * PWINDOWS_SERVICE_DEVICE;

typedef struct _WINDOWS_SERVICE_FILE
{
    PCHANNEL Channel;
} WINDOWS_SERVICE_FILE, * PWINDOWS_SERVICE_FILE;

NTSTATUS WindowsServiceInitializate(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistyPath);
NTSTATUS WindowsServiceAddDevice(_In_ PDRIVER_OBJECT DriverObject, _In_ PDEVICE_OBJECT PhysicalDeviceObject);
DRIVER_UNLOAD WindowsServiceUninitializate;
DRIVER_DISPATCH WindowsServiceCreate;
DRIVER_DISPATCH WindowsServiceClose;
DRIVER_DISPATCH WindowsServiceCleanup;
DRIVER_DISPATCH WindowsServiceIOControl;