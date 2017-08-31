#pragma once

#include "src/common.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <shlwapi.h>
#include <ShellAPI.h>

#include "resource.h"
#include "src/dmusici.h"
#include "src/g_define.h"
#include "src/g_type.h"
#include "src/comcls.hpp"
#include "src/kernel.h"
#include "src/vbstruct.h"
#include "src/hmm.h"
#include "src/udis86.h"
#include "src/extasm.h"
#include "gui/wndcls.hpp"

#pragma comment(linker, "/ENTRY:DllEntry") 
#pragma comment(linker, "/SECTION:.pcode,RW")
#pragma comment(linker, "/SECTION:PVM,RWE")

//Static Library
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"oleaut32.lib")

#pragma warning(disable:4101)
#pragma warning(disable:4996)

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32'   name='Microsoft.Windows.Common-Controls'   version='6.0.0.0'   processorArchitecture='x86'   publicKeyToken='6595b64144ccf1df'   language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32'   name='Microsoft.Windows.Common-Controls'   version='6.0.0.0'   processorArchitecture='ia64'   publicKeyToken='6595b64144ccf1df'   language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32'   name='Microsoft.Windows.Common-Controls'   version='6.0.0.0'   processorArchitecture='amd64'   publicKeyToken='6595b64144ccf1df'   language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32'   name='Microsoft.Windows.Common-Controls'   version='6.0.0.0'   processorArchitecture='*'   publicKeyToken='6595b64144ccf1df'   language='*'\"")
#endif

#ifdef __cplusplus
extern "C" {
#endif
  BOOL WINAPI _DllMainCRTStartup(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved);
#ifdef __cplusplus
}
#endif

#ifdef _INC_COMMCTRL

#define ListView_InsertItemA(hwnd, pItem) (int)::SendMessageA((hwnd), LVM_INSERTITEMA, 0, (LPARAM)(const LV_ITEMA *)(pItem))

#define ListView_SetItemA(hwnd, pItem) (BOOL)::SendMessageA((hwnd), LVM_SETITEMA, 0, (LPARAM)(const LV_ITEMA *)(pItem))

#define ListView_DlgDeleteColumn(hwnd, dlgitem, iCol) \
  (BOOL)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_DELETECOLUMN, (WPARAM)(int)(iCol), 0)

#define ListView_DlgSetExtendedListViewStyle(hwnd, dlgitem, dw)\
  (DWORD)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw)

#define ListView_DlgInsertColumn(hwnd, dlgitem, iCol, pcol) \
  (int)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_INSERTCOLUMN, (WPARAM)(int)(iCol), (LPARAM)(const LV_COLUMN *)(pcol))

#define ListView_DlgDeleteAllItems(hwnd, dlgitem) \
  (BOOL)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_DELETEALLITEMS, 0, 0L)

#define ListView_DlgInsertItemA(hwnd, dlgitem, pitem)   \
  (int)::SendDlgItemMessageA((hwnd),(int)(dlgitem),LVM_INSERTITEMA, 0, (LPARAM)(const LVITEMA *)(pitem))

#define ListView_DlgInsertItemW(hwnd, dlgitem, pitem)   \
  (int)::SendDlgItemMessageW((hwnd),(int)(dlgitem),LVM_INSERTITEMW, 0, (LPARAM)(const LVITEMW *)(pitem))

#define ListView_DlgInsertItem(hwnd, dlgitem, pitem)   \
  (int)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_INSERTITEM, 0, (LPARAM)(const LV_ITEM *)(pitem))

#define ListView_DlgSetItemA(hwnd, dlgitem,pitem) \
  (BOOL)::SendDlgItemMessageA((hwnd),(int)(dlgitem),LVM_SETITEMA, 0, (LPARAM)(const LVITEMA *)(pitem))

#define ListView_DlgSetItemW(hwnd, dlgitem,pitem) \
  (BOOL)::SendDlgItemMessageW((hwnd),(int)(dlgitem),LVM_SETITEMW, 0, (LPARAM)(const LVITEMW *)(pitem))

#define ListView_DlgSetItem(hwnd, dlgitem,pitem) \
  (BOOL)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(pitem))

#define ListView_DlgGetItemCount(hwnd, dlgitem) \
  (int)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_GETITEMCOUNT, 0, 0L)

#define TreeView_InsertItemA(hwnd, lpis) \
  (HTREEITEM)::SendMessageA((hwnd), TVM_INSERTITEMA, 0, (LPARAM)(LPTV_INSERTSTRUCT)(lpis))

#define ComboBox_DeleteAll(hwnd) \
  while ((int)SNDMSG((HWND)(hwnd), CB_DELETESTRING , 0, 0) > 0) {} 

#define ComboBox_DlgDeleteAll(hwnd, dlgitem) \
  while ((int)::SendDlgItemMessage((HWND)(hwnd), (int)(dlgitem), CB_DELETESTRING , 0, 0) > 0) {} 

#define _LoadIcon_16x16(iIcon, hIcon, bret) \
  hIcon = (HICON)::LoadImage(DllBase, MAKEINTRESOURCE(iIcon),IMAGE_ICON,16,16,LR_DEFAULTCOLOR | LR_SHARED | LR_CREATEDIBSECTION); \
  if (!hIcon) bret = FALSE 

#define ALLOC_PARAM(tvis,ptv,ncode,idx) \
  ptv = (PVB_TV_TYPE)calloc(1,sizeof(VB_TV_TYPE)); \
  ptv->TV.Sign = MY_MAGIC; \
  ptv->TV.NCode = (WORD)ncode; \
  ptv->TV.Index = (WORD)idx; \
  tvis.item.lParam = (LPARAM)ptv

#else
#error "未能成功包含commctrl.h"
#endif

/* pcode type 0x0000 - 0xFFFF */
#define PCT_STD                   1
#define PCT_IDX                   2
#define PCT_CLEARMEMORY           3
#define PCT_ONESTRING             4
#define PCT_RETONEVAR             5
#define PCT_ADDARGTOATACK         6
#define PCT_NONE                  99
/* pcode flag 0x00010000 - 0xFFFF0000 */
#define PCF_ENDPROC               0x00010000

#define VAMIN(inta,intb)        ((UINT)inta < (UINT)intb)?inta:intb

#define VAMAX(inta,intb)        ((UINT)inta > (UINT)intb)?inta:intb

#define ALLOC_SEG(sn)           __declspec(allocate(sn)) 

#define TEXPECT_NOACCESS(EC)    (EC == EXCEPTION_ACCESS_VIOLATION)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH

#define F_SUCCESS(val)          (val == 0)

extern const GUID               vbclsid;

extern WORD                     NtVer;
extern HMODULE                  DllBase;
extern DWORD                    PAA;      //Process All Access
extern DWORD                    TAA;      //Thread All Access

/*P-Code 指令集相关*/
extern const char                 * pcit_ive;
extern const char                 * pcit_unk;
extern const VB_PCODE_STD_INST      PCodeStdInst[251];
extern const VB_PCODE_LEAD_INST     PCodeLead0Inst[];
extern const VB_PCODE_LEAD_INST     PCodeLead1Inst[];
extern const VB_PCODE_LEAD_INST     PCodeLead2Inst[];
extern const VB_PCODE_LEAD_INST     PCodeLead3Inst[];
extern const VB_PCODE_LEAD_INST     PCodeLead4Inst[];

/*反汇编语法颜色*/
extern COLORREF     DCF_Inst;
extern COLORREF     DCF_StackOp;
extern COLORREF     DCF_FMSOp;
extern COLORREF     DCF_CJump;
extern COLORREF     DCF_Jump;
extern COLORREF     DCF_Call;
extern COLORREF     DCF_Ret;
extern COLORREF     DCF_PrivInst;
extern COLORREF     DCF_Prefix;
extern COLORREF     DCF_DefClr;
extern COLORREF     DCF_Int;
extern COLORREF     DCF_Data;
extern COLORREF     DCF_Text;
extern COLORREF     DCF_Symbol;
extern COLORREF     DCF_Error;
extern COLORREF     DCF_GPRs;
extern COLORREF     DCF_FSRs;
extern COLORREF     DCF_SSRs;
extern COLORREF     DCF_MStack;
extern COLORREF     DCF_MOther;
extern COLORREF     DCF_IMM;
extern COLORREF     DCF_IMMAddr;
extern COLORREF     DCF_Change;

extern COLORREF     DCB_Inst;
extern COLORREF     DCB_StackOp;
extern COLORREF     DCB_FMSOp;
extern COLORREF     DCB_CJump;
extern COLORREF     DCB_Jump;
extern COLORREF     DCB_Call;
extern COLORREF     DCB_Ret;
extern COLORREF     DCB_PrivInst;
extern COLORREF     DCB_Prefix;
extern COLORREF     DCB_DefClr;
extern COLORREF     DCB_Int;
extern COLORREF     DCB_Data;
extern COLORREF     DCB_Text;
extern COLORREF     DCB_Symbol;
extern COLORREF     DCB_Error;
extern COLORREF     DCB_GPRs;
extern COLORREF     DCB_FSRs;
extern COLORREF     DCB_SSRs;
extern COLORREF     DCB_MStack;
extern COLORREF     DCB_MOther;
extern COLORREF     DCB_IMM;
extern COLORREF     DCB_IMMAddr;
extern COLORREF     DCB_Change;


//About Thread
extern UINT         ht_main;
extern UINT         tid_main;
//const Text
extern PTCHAR       WC_Main;
extern PTCHAR       WN_Main;
extern PTCHAR       WC_Module;
extern PTCHAR       WN_Module;
extern PTCHAR       WC_Memory;
extern PTCHAR       WN_Memory;
extern PTCHAR       WC_Heap;
extern PTCHAR       WN_Heap;
extern PTCHAR       WC_Thread;
extern PTCHAR       WN_Thread;
extern PTCHAR       WC_Disasm;
extern PTCHAR       WN_Disasm;
extern PTCHAR	      WC_VB;
extern PTCHAR	      WN_VB;
extern PTCHAR       WC_PCP;
extern PTCHAR       WN_PCP;
extern PTCHAR       LN_Module;
extern PTCHAR       LN_Section;
extern PTCHAR       LN_Memory;
extern PTCHAR       LN_Heap;
extern PTCHAR       LN_Thread;
extern PTCHAR       TN_VB;
extern PTCHAR       LN_VB;
extern PTCHAR       CN_MDI;
extern PTCHAR       CN_Static;
extern PTCHAR       CN_Edit;
extern PTCHAR       CN_Button;
extern PTCHAR       t_fgf;
extern PTCHAR       sc_ex;
extern PTCHAR       sc_or;
extern PTCHAR       sc_rw;
extern PTCHAR       sc_sh;
extern PTCHAR       sc_dc;
extern PTCHAR       sc_np;
extern PTCHAR       sc_rt;
extern PTCHAR       sc_id;
extern PTCHAR       sc_ud;
extern PTCHAR       sc_cd;
extern PTCHAR       t_ref;
extern PTCHAR       t_viw;

extern MainForm   * mf;

extern bool         mdlASCEND;

//extern func
extern void TestFunc(DWORD);

extern BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
extern int ImageList_Add32bpp(HIMAGELIST hImageList, HINSTANCE hInst, UINT nId, UINT uType);
extern BOOL RegisterAllClass();
extern BOOL UnregisterAllClass();
extern BOOL LoadAllMenu();
extern BOOL DestroyAllMenu();
extern BOOL LoadAllImage();
extern BOOL DestroyAllImage();
extern BOOL InitBaseControl();
extern BOOL FreeBaseControl();

extern BOOL WINAPI LaunchDebugger(long LaunchParam);
extern int SetStatusText(PTCHAR pText);
extern bool PlayMIDI(PBYTE data, UINT size);
extern void StopMIDI();
extern DWORD GetThreadInformation(DWORD Tid, THREADINFOCLASS TIC, PVOID pData, DWORD dLen);

extern COLORREF scGetForeColor(DWORD sc);
extern COLORREF scGetBackColor(DWORD sc);
extern int CALLBACK mdl_CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM SubItem);
extern void CommonUpdatelParam(HWND hlv);
extern void ShowCurrentFocus();

extern LRESULT CALLBACK MainProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
extern LRESULT CALLBACK mdlwndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
extern LRESULT CALLBACK memwndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

extern BOOL CheckMessageQueue();
extern DWORD TestExpect(DWORD ExpectCode);
extern BOOL HitModule(PVOID pAddr, PUINT mbase, PUINT msize);
extern BOOL GetThreadDescriptor(HANDLE hThread, DWORD dwSelector, PDWORD pBase, PDWORD pLimit, PBOOL Is4KB);
extern UINT GetModuleSize(HMODULE hMod, BOOL FormImageSize);
extern UINT GetModuleEntry(HMODULE hMod);
extern BOOL GetSectionBaseAddr(int dwAddr, PDWORD opSectAddr, PDWORD opSectSize);
extern void GetFileName(PTCHAR FilePath);
extern void GetSectionName(HMODULE hMod, PVOID pSct, PCHAR OutName);
extern void GetPageProtectString(DWORD PP, PTCHAR _Out, DWORD _Max);
extern DWORD GetThreadInformation(HANDLE hThread, THREADINFOCLASS TIC, PVOID pData, DWORD dLen);
extern DWORD GetThreadStartAddress(HANDLE hThread);
extern PVOID GetThreadEnvironmentBlockAddress(HANDLE hThread);
extern SRDBG_TS GetThreadState(ULONG ulPID, ULONG ulTID);
extern UINT WINAPI FindMemory(PBYTE destAddr, UINT destSize, PCMP_BYTE pCB, UINT cbCount, BOOL Absolute);
extern BOOL GetMemberNameByIndex(GUID ClsID, DWORD Idx, PWCHAR pText, DWORD tCount, PDWORD pMemberID);
extern BOOL GetEventNameByIndex(GUID ClsID, DWORD Idx, PWCHAR pText, DWORD tCount, PDWORD pMemberID);
extern BOOL GetEventNameByMemberID(GUID ClsID, DWORD MemberID, PWCHAR pText, DWORD tCount);
extern BOOL GetPropertyText(GUID guid, WORD VTOS, PWCHAR pText, DWORD tMax, PWCHAR pNote, DWORD nMax);

extern UINT casmlen(PWCHAR src, size_t maxcount);
extern void casmcpy(PWCHAR dest, PWCHAR src, size_t scount);
extern void casmcpy_a(PWCHAR *dest, PWCHAR src, size_t scount);
extern int DrawCasmW(HDC hdc, PWCHAR pCasm, size_t bufcount, LONG wspace, LPRECT lprc, UINT format);

extern BOOL WCharTrim(PWCHAR src);
extern BOOL WCharIsHex(PWCHAR src);

extern INT_PTR CALLBACK GotoAddrProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
extern INT_PTR CALLBACK AsmProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
extern INT_PTR CALLBACK AboutProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);

extern int NTAPI SR_SetVectoredExceptionHandler(PVECTORED_EXCEPTION_HANDLER Handler);
extern LONG NTAPI FirstExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo);
extern LONG NTAPI LastExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo);

extern PVBHD GetVbHeaderByModule(HMODULE hMod);
extern PIMAGE_SECTION_HEADER GetSectionBaseAddr(HMODULE hMod);
extern WORD GetSectionNumber(HMODULE hMod);
extern PVOID GetSectionAddrByName(HMODULE hMod, PCHAR pSecName, DWORD * pSecSize);
extern PVOID WINAPI MemMem(PBYTE pMem1, SIZE_T MemSize1, PBYTE FindMem, SIZE_T fmSize);

extern int pcd_initdeobj(PPDO pcdo, PTCHAR pszBuf, UINT szsize, PBYTE inbuf, UINT bcount, UINT mbase, UINT msize, PVBPDI ppdi);
extern int pcd_decode(PPDO pcdo, UINT dcFlag);
extern int pcd_disassemble(PPDO pcdo, PBOOL blInit);