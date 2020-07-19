#pragma once

#include "lxssex.WindowManager.Shared.hpp"

#define NTDEVICE_SERVICE_NAME L"\\Device\\lxssex\\WindowManager"
#define SYMBOLIC_SERVICE_NAME L"\\DosDevices\\lxssex\\WindowManager"

#define INITGUID
#include <guiddef.h>
// {6BD9E6EA-5F8D-47ED-949A-4141CF417A66}
DEFINE_GUID(LXSSEX_WM_INTERFACE_GUID, 0x6bd9e6ea, 0x5f8d, 0x47ed, 0x94, 0x9a, 0x41, 0x41, 0xcf, 0x41, 0x7a, 0x66);
#undef INITGUID

typedef struct _WINDOWS_WM_DEVICE
{
    UNICODE_STRING InterfaceSymbolicLinkName;
} WINDOWS_WM_DEVICE, * PWINDOWS_WM_DEVICE;

typedef struct _WINDOWS_WM_FILE
{

} WINDOWS_WM_FILE, * PWINDOWS_WM_FILE;

NTSTATUS WindowsWindowManagerInitializate(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistyPath);
NTSTATUS WindowsWindowManagerAddDevice(_In_ PDRIVER_OBJECT DriverObject, _In_ PDEVICE_OBJECT PhysicalDeviceObject);
DRIVER_UNLOAD WindowsWindowManagerUninitializate;
DRIVER_DISPATCH WindowsWindowManagerCreate;
DRIVER_DISPATCH WindowsWindowManagerClose;
DRIVER_DISPATCH WindowsWindowManagerCleanup;
DRIVER_DISPATCH WindowsWindowManagerIOControl;