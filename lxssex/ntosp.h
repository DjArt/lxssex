//#pragma once
//
//#include "default.h"
//
//#define GetRoutineWithType(mod, func)     (decltype(##func)*)RtlFindExportedRoutineByName(mod, #func)
//#define GetSystemRoutineWithType(func)      (decltype(##func)*)MmGetSystemRoutineAddress(&func##String)
//
//typedef struct _SYSTEM_MODULE_ENTRY
//{
//    HANDLE Section;
//    PVOID MappedBase;
//    PVOID ImageBase;
//    ULONG ImageSize;
//    ULONG Flags;
//    USHORT LoadOrderIndex;
//    USHORT InitOrderIndex;
//    USHORT LoadCount;
//    USHORT OffsetToFileName;
//    UCHAR FullPathName[256];
//} SYSTEM_MODULE_ENTRY, * PSYSTEM_MODULE_ENTRY;
//
//typedef struct _SYSTEM_MODULE_INFORMATION
//{
//    ULONG Count;
//    SYSTEM_MODULE_ENTRY Module[0];
//} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;
//
//typedef enum _SYSTEM_INFORMATION_CLASS
//{
//    SystemModuleInformation = 11,
//    SystemKernelDebuggerInformation = 35
//} SYSTEM_INFORMATION_CLASS, * PSYSTEM_INFORMATION_CLASS;
//
//
////
//// Undocumented Kernel (NT) APIs
////
//NTKERNELAPI PVOID NTAPI RtlFindExportedRoutineByName(_In_ PVOID ImageBase, _In_ PCCH RoutineName);
//NTKERNELAPI PVOID NTAPI MmGetSystemRoutineAddress(_In_ PUNICODE_STRING SystemRoutineName);
//NTKERNELAPI NTSTATUS NTAPI MmLoadSystemImage(_In_ PUNICODE_STRING ImageFileName, _In_ PUNICODE_STRING NamePrefix OPTIONAL, _In_ PUNICODE_STRING LoadedBaseName OPTIONAL, _In_ ULONG LoadFlags, _Out_ PVOID* Section, _Out_ PVOID* ImageBaseAddress);
//NTKERNELAPI VOID NTAPI MmUnloadSystemImage(_In_ PVOID Section);
//NTKERNELAPI NTSTATUS NTAPI ZwQuerySystemInformation(_In_ ULONG SystemInformationClass, _Out_ PVOID SystemInformation, _In_ ULONG SystemInformationLength, _Out_ PULONG ReturnLength);
//
//NTSTATUS FindProcessByName(CHAR* process_name, PEPROCESS* process);
//PVOID GetKernelModule(const char* module_name);