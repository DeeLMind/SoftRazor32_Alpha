#pragma once

#include "common.h"
#include "vbstruct.h"

enum SRDBG_TS : signed int
{
  TS_Terminated = -1,
  TS_Running = 0,
  TS_Suspended = 1,
  TS_Wait = 2,
  TS_Unknown = 3
};

enum SRDBG_STATE : unsigned int
{
  DS_Idle,
  DS_Busy
};

typedef int (WINAPI * func_qsymbol)(int qaddr, wchar_t * iText, size_t maxc);

//类型定义:
typedef struct _LV_TYPE         //通用LV类型
{
  int                         Item;
  COLORREF                    ForeColor;
  COLORREF                    BackColor;
} LV_TYPE, *PLV_TYPE;

typedef struct _TV_TYPE         //通用TV类型
{
  DWORD                       Sign;
  WORD                        NCode;      //通知码
  WORD                        Index;      //索引
  HTREEITEM                   hItem;
  DWORD                       lParam;
} TV_TYPE, *PTV_TYPE;

typedef struct _CFPARAM
{
  HWND                        hCtl;
  INT                         SubItem;
} CFPARAM, *PCFPARAM;

typedef struct _MEMORY_LV_TYPE
{
  LV_TYPE                     LV;
  PVOID                       BlockAddress;
} MEMORY_LV_TYPE, *PMEMORY_LV_TYPE;

typedef struct _MODULE_LV_TYPE
{
  LV_TYPE                     LV;
  PVOID                       ModuleAddress;
} MODULE_LV_TYPE, *PMODULE_LV_TYPE;

typedef struct _THREAD_LV_TYPE
{
  LV_TYPE                     LV;
  DWORD                       TID;
  SRDBG_TS                    State;
} THREAD_LV_TYPE, *PTHREAD_LV_TYPE;

typedef struct _VB_LV_TYPE
{
  LV_TYPE                     LV;
} VB_LV_TYPE, *PVB_LV_TYPE;

typedef struct _VB_TV_TYPE
{
  TV_TYPE                     TV;
} VB_TV_TYPE, *PVB_TV_TYPE;

typedef struct _CLASS_TABLE
{
  HWND						            hWindow;
  PVOID						            pClass;
  struct _CLASS_TABLE			  * Next;
} CLASS_TABLE, *PCLASS_TABLE;

typedef struct _MEMORY_MODIFY
{
  PBYTE						            BeginAddr;        //开始地址
  PBYTE                       poMem;            //原始内存字节
  UINT						            BlockSize;        //块大小
  WORD                        Flag0;            //标志0
  WORD                        Flag1;            //标志1
  struct _MEMORY_MODIFY		  * Next;             //Next指针
} MEMORY_MODIFY, *PMEMORY_MODIFY;

typedef struct _THREAD_LIST
{
  DWORD                       TID;
  SRDBG_TS                    State;
} THREAD_LIST, *PTHREAD_LIST;

typedef struct _DISASM_ITEM
{
  PVOID						            VirtualAddress;
  UINT						            InstSize;
  TCHAR                       AddrText[8];
  PTCHAR                      pInstText;
  UINT                        itCount;
  PTCHAR                      pNote;
  struct _DISASM_ITEM         *Next;
} DISASM_ITEM, *PDISASM_ITEM, *PDAI , **PPDAI;

typedef struct _VB_PROC_LIST
{
  PVBC                        pVbCtl;
  WORD                        iIndex;    //事件Idx
  PVBPDI                      ProcDesc;
  PBYTE                       ProcEntry;
  PTCHAR                      EvtName;
  DWORD                       Flag0;
  DWORD                       Flag1;
  struct _VB_PROC_LIST      * Next;
} VB_PROC_LIST, VBPL, *PVBPL;

typedef struct _VB_OBJECT_LIST
{
  PVBHD                       VbHead;
  PVBO                        VbObj;
  DWORD                       iIndex;   //Obj Idx
  PTCHAR                      ObjName;
  PVBPL                       pl_Head;
  PVBPL                       pl_Last;
  DWORD                       Flag0;
  DWORD                       Flag1;
  struct _VB_OBJECT_LIST    * Next;
} VB_OBJECT_LIST, VBOBJL, *PVBOBJL;

typedef struct _PP_PROC_LIST
{
  PVOID                       pAddr;
  int                         cbidx;
  struct _PP_PROC_LIST      * Next;
  TCHAR                       ProcName[256];
} PP_PROC_LIST , PPPL , *PPPPL;

typedef struct _PCODE_DECODE_OBJECT
{
  PBYTE                       inp_opbuf;          //set in
  UINT                        inp_idx;            //in
  UINT                        inp_count;          //set in
  UINT                        opSize;             //out 标准指令长度
  UINT                        otSize;             //out 指令 + 变参 总长度
  UINT                        uFlag;              //out
  UINT                        opType;             //out
  UINT                        arType;             //out
  PTCHAR                      szMnem;             //set out  Mnemonic
  UINT                        szlen;              //set in
  PTCHAR                      szNote;             //set out
  UINT                        sznlen;             //set in
  UINT                        lastval;            //out
  UINT                        mbase;              //set in
  UINT                        msize;              //set in
  PVBPDI                      ppdi;               //set in
  LONG                        orgcp;              //原Const Pool
  func_qsymbol                func_qs;            //query symbol function
  UINT                        base;
  BYTE                        std;
  BYTE                        lead;
} PCODE_DECODE_OBJECT , PDO , *PPDO;

typedef union _OPDWORD
{
  BYTE      Byte[4];
  WORD      Word[2];
  DWORD     DWord;
} OPDWORD , *POPDWORD;

typedef enum _SYSTEM_INFORMATION_CLASS
{
  SystemBasicInformation = 0,
  SystemCpuInformation = 1,
  SystemPerformanceInformation = 2,
  SystemTimeOfDayInformation = 3, /* was SystemTimeInformation */
  Unknown4,
  SystemProcessInformation = 5,
  Unknown6,
  Unknown7,
  SystemProcessorPerformanceInformation = 8,
  Unknown9,
  Unknown10,
  SystemModuleInformation = 11,
  Unknown12,
  Unknown13,
  Unknown14,
  Unknown15,
  SystemHandleInformation = 16,
  Unknown17,
  SystemPageFileInformation = 18,
  Unknown19,
  Unknown20,
  SystemCacheInformation = 21,
  Unknown22,
  SystemInterruptInformation = 23,
  SystemDpcBehaviourInformation = 24,
  SystemFullMemoryInformation = 25,
  SystemNotImplemented6 = 25,
  SystemLoadImage = 26,
  SystemUnloadImage = 27,
  SystemTimeAdjustmentInformation = 28,
  SystemTimeAdjustment = 28,
  SystemSummaryMemoryInformation = 29,
  SystemNotImplemented7 = 29,
  SystemNextEventIdInformation = 30,
  SystemNotImplemented8 = 30,
  SystemEventIdsInformation = 31,
  SystemCrashDumpInformation = 32,
  SystemExceptionInformation = 33,
  SystemCrashDumpStateInformation = 34,
  SystemKernelDebuggerInformation = 35,
  SystemContextSwitchInformation = 36,
  SystemRegistryQuotaInformation = 37,
  SystemCurrentTimeZoneInformation = 44,
  SystemTimeZoneInformation = 44,
  SystemLookasideInformation = 45,
  SystemSetTimeSlipEvent = 46,
  SystemCreateSession = 47,
  SystemDeleteSession = 48,
  SystemInvalidInfoClass4 = 49,
  SystemRangeStartInformation = 50,
  SystemVerifierInformation = 51,
  SystemAddVerifier = 52,
  SystemSessionProcessesInformation = 53,
  SystemInformationClassMax
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

