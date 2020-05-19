//#include "default.h"
//
//UNICODE_STRING Win32kDriverNameString = RTL_CONSTANT_STRING(L"\\SystemRoot\\System32\\win32kfull.sys");
//
//PVOID GetKernelModule(const char* module_name)
//{
//    ULONG bytes = 0;
//    NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);
//    if (!bytes)
//        return 0;
//
//    PSYSTEM_MODULE_INFORMATION modules = (PSYSTEM_MODULE_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, bytes, 0x454E4F45); // 'ENON'
//    status = ZwQuerySystemInformation(SystemModuleInformation, modules, bytes, &bytes);
//    if (!NT_SUCCESS(status))
//        return 0;
//
//    PSYSTEM_MODULE_ENTRY module = modules->Module;
//    PVOID module_base = 0, module_size = 0;
//    for (ULONG i = 0; i < modules->Count; i++)
//    {
//        if (strcmp((char*)module[i].FullPathName, module_name) == 0)
//        {
//            module_base = module[i].ImageBase;
//            module_size = (PVOID)module[i].ImageSize;
//            break;
//        }
//    }
//
//    if (modules)
//        ExFreePoolWithTag(modules, 0x454E4F45);
//
//    if (module_base <= 0)
//        return 0;
//
//    return module_base;
//}
//
//NTSTATUS FindProcessByName(CHAR* process_name, PEPROCESS* process)
//{
//    PEPROCESS sys_process = PsInitialSystemProcess;
//    PEPROCESS cur_entry = sys_process;
//
//    CHAR image_name[15];
//
//    do
//    {
//        RtlCopyMemory((PVOID)(&image_name), (PVOID)((uintptr_t)cur_entry + 0x450) /*EPROCESS->ImageFileName*/, sizeof(image_name));
//
//        if (strstr(image_name, process_name))
//        {
//            DWORD active_threads;
//            RtlCopyMemory((PVOID)&active_threads, (PVOID)((uintptr_t)cur_entry + 0x498) /*EPROCESS->ActiveThreads*/, sizeof(active_threads));
//            if (active_threads)
//            {
//                *process = cur_entry;
//                return STATUS_SUCCESS;
//            }
//        }
//
//        PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(cur_entry)+0x2F0) /*EPROCESS->ActiveProcessLinks*/;
//        cur_entry = (PEPROCESS)((uintptr_t)list->Flink - 0x2F0);
//
//    } while (cur_entry != sys_process);
//
//    return STATUS_NOT_FOUND;
//}