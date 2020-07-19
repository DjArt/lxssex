#pragma once

#include <ntifs.h>

template<typename T> struct AVL
{
public:
    using AVL_COMPARE_ROUTINE = RTL_GENERIC_COMPARE_RESULTS (*)(AVL* table, T* first, T* second);
    using AVL_ALLOCATE_ROUTINE = PVOID(*)(AVL* table, CLONG byteSize);
    using AVL_FREE_ROUTINE = VOID(*)(AVL* table, PVOID buffer);

private:
    RTL_AVL_TABLE Table;
    FAST_MUTEX Mutex;

    static PVOID DefaultAllocateRoutine(_In_ PRTL_AVL_TABLE Table, _In_ CLONG Size)
    {
        UNREFERENCED_PARAMETER(Table);
        return ExAllocatePoolWithTag(PagedPool, Size, 0);
    }

    static VOID DefaultFreeRoutine(_In_ PRTL_AVL_TABLE Table, _In_ PVOID Buffer)
    {
        UNREFERENCED_PARAMETER(Table);
        ExFreePoolWithTag(Buffer, 0);
    }
public:
    static AVL* InitializeAvl(_In_ AVL_COMPARE_ROUTINE compareRoutine, _In_opt_ PVOID TableContext)
    {
        AVL* result = (AVL*)ExAllocatePoolWithTag(NonPagedPool, sizeof(AVL), 0);
        ExInitializeFastMutex(&result->Mutex);
        RtlInitializeGenericTableAvl(&result->Table, (PRTL_AVL_COMPARE_ROUTINE)compareRoutine, (PRTL_AVL_ALLOCATE_ROUTINE)DefaultAllocateRoutine, (PRTL_AVL_FREE_ROUTINE)DefaultFreeRoutine, TableContext);
        return result;
    }

    static AVL* InitializeAvl(_In_ AVL_COMPARE_ROUTINE compareRoutine, _In_ AVL_ALLOCATE_ROUTINE allocateRoutine, _In_ AVL_FREE_ROUTINE freeRoutine, _In_opt_ PVOID TableContext)
    {
        AVL* result = (AVL*)ExAllocatePoolWithTag(NonPagedPool, sizeof(AVL), 0);
        ExInitializeFastMutex(&result->Mutex);
        RtlInitializeGenericTableAvl(&result->Table, (PRTL_AVL_COMPARE_ROUTINE)compareRoutine, (PRTL_AVL_ALLOCATE_ROUTINE)allocateRoutine, (PRTL_AVL_FREE_ROUTINE)freeRoutine, TableContext);
        return result;
    }

    static void FreeAvl(AVL* tree)
    {
        PVOID Element = NULL;
        ExAcquireFastMutex(&tree->Mutex);
        while (!RtlIsGenericTableEmptyAvl(&tree->Table))
        {
            Element = RtlGetElementGenericTableAvl(&tree->Table, 0);
            if (Element) RtlDeleteElementGenericTableAvl(&tree->Table, Element);
        }
        ExReleaseFastMutex(&tree->Mutex);
        ExFreePoolWithTag(tree, 0);
    }

    BOOLEAN Add(T* element)
    {
        BOOLEAN newElement = FALSE;
        ExAcquireFastMutex(&Mutex);
        RtlInsertElementGenericTableAvl(&Table, element, sizeof(T), &newElement);
        ExReleaseFastMutex(&Mutex);
        return newElement;
    }

    ULONG Count()
    {
        return RtlNumberGenericTableElementsAvl(&Table);
    }

    T* GetElementByIndex(ULONG index)
    {
        ExAcquireFastMutex(&Mutex);
        T* result = NULL;
        if (index >= 0 && index < Count())
        {
            result = (T*)RtlGetElementGenericTableAvl(&Table, index);
        }
        ExReleaseFastMutex(&Mutex);
        return result;
    }

    T* Find(T* forCompare)
    {
        ExAcquireFastMutex(&Mutex);
        T* result = (T*)RtlLookupElementGenericTableAvl(&Table, forCompare);
        ExReleaseFastMutex(&Mutex);
        return result;
    }

    BOOLEAN Remove(T* element)
    {
        BOOLEAN removed = FALSE;
        ExAcquireFastMutex(&Mutex);
        removed = RtlDeleteElementGenericTableAvl(&Table, element);
        ExReleaseFastMutex(&Mutex);
        return removed;
    }

};

template<typename T> using PAVL = AVL<T>*;