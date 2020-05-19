#pragma once

typedef struct _LX_PICOCONTEXT_MEMORY
{
    SIZE_T field_0;
    char field_8;
    INT8 gap_9[7];
    INT64 field_10;
    EX_PUSH_LOCK PushLock;
    struct _KTHREAD* field_20;
    INT8 gap_28[16];
    HANDLE ProcessHandle;
    __int64 field_40;
    INT64 field_48;
    INT64 field_50;
    INT64 field_58;
    INT8 gap_60[16];
    __int64 field_70;
    __int64 field_78;
    INT64 field_80;
} LX_PICOCONTEXT_MEMORY, *PLX_PICOCONTEXT_MEMORY;

typedef struct _LX_PICOCONTEXT
{
    PVOID field_0;
    INT8 gap_8[40];
    __int64 field_30;
    PVOID field_38;
    INT8 gap_40[8];
    char field_48;
    INT8 gap_49[15];
    void* field_58;
    INT8 gap_60[96];
    LX_PICOCONTEXT_MEMORY field_C0;
} LX_PICOCONTEXT, * PLX_PICOCONTEXT;
