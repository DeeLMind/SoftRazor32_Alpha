#pragma once

#include "../src/common.h"
#include "../src/g_type.h"

extern BOOL     AddClassTable(HWND hWnd, PVOID pClass);
extern PVOID    GetClassPointer(HWND hWnd);
extern BOOL     DelClassTable(HWND hWnd);

//类定义:

/* MDI多文档窗口基类 */
class MDIForm
{
protected:
  virtual HWND CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style) = 0;
  HWND m_hWnd;
  HWND hw_MDI;

public:
  MDIForm(HWND h_MDI) : hw_MDI(h_MDI), m_hWnd(NULL) {}
  ~MDIForm();

  HWND GetMainhWnd();
  BOOL SetTop();
  BOOL DestroyForm();
};

/* 浮动窗口基类 */
class FloatDialog
{
protected:
  virtual HWND CreateForm() = 0;
  HWND  m_hWnd;
  HWND  m_hOwner;

public:
  FloatDialog(HWND hOwner) : m_hWnd(NULL), m_hOwner(hOwner) {}
  ~FloatDialog();

  HWND GetMainhWnd();
  BOOL SetTop();
  BOOL DestroyForm();
};

/*模块窗口类*/
class MC_Module : public MDIForm
{
private:
  HWND mdllv;
  HWND sctlv;
  bool mdlASCEND;

  void mdlInsertColumn();
  void sctInsertColumn();
  void UpdateModuleList();
  void UpdateSectionInfo(HMODULE hMod);
  static void UpdatelParam(HWND hlv);
  static int CALLBACK mdl_CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam);

public:
  MC_Module(HWND hMDI) : mdllv(0), sctlv(0), mdlASCEND(0), MDIForm(hMDI) {}
  ~MC_Module();

  HWND CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style);
  HWND GetModuleListhWnd();
  static LRESULT CALLBACK mdlwndProc(HWND, UINT, WPARAM, LPARAM);
};


/*内存窗口类*/
class MC_Memory : public MDIForm
{
private:
  HWND memlv;
  HMENU PM_0;
  int iSelect;
  bool OnlyCommit;

  void memInsertColumn();
  void UpdateMemoryList();

public:
  MC_Memory(HWND hMDI) : PM_0(0), iSelect(-1), OnlyCommit(false), MDIForm(hMDI) {}
  ~MC_Memory();

  HWND CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style);
  static LRESULT CALLBACK memwndProc(HWND, UINT, WPARAM, LPARAM);
};


/*堆信息窗口类*/
class MC_Heap : public MDIForm
{
private:
  HWND hw_heaplv;

  void heapInsertColumn();
  void UpdateHeapList();

public:
  MC_Heap(HWND hMDI) : MDIForm(hMDI) {}
  ~MC_Heap();

  HWND CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style);
  static LRESULT CALLBACK heapwndProc(HWND, UINT, WPARAM, LPARAM);
};

/*线程窗口类*/
class MC_Thread : public MDIForm
{
private:
  HWND hw_thlv;
  HMENU PM_3;
  int iSelect;

  void threadInsertColumn();
  void UpdateThreadList();
public:
  MC_Thread(HWND hMDI) : PM_3(NULL), iSelect(-1), MDIForm(hMDI) {}
  ~MC_Thread();

  HWND CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style);
  static LRESULT CALLBACK threadwndProc(HWND, UINT, WPARAM, LPARAM);
};

/* 浮动寄存器窗口  */
class FW_RegWnd : public FloatDialog
{
private:
  HANDLE        cThread;

  void UpdateThreadInfo();
  void UpdateRegistersByHandle(HANDLE hThread);
  void UpdateRegistersById(DWORD TID);
  void ResetRegistersText();

public:
  FW_RegWnd(HWND hOwner) : cThread(NULL), FloatDialog(hOwner) {}
  ~FW_RegWnd();

  HWND          CreateForm();
  static        INT_PTR CALLBACK fw_regwnd(HWND, UINT, WPARAM, LPARAM);
};

/*反汇编窗口类*/
class MC_Disasm : public MDIForm
{
private:
  //PDAI    ItemArray;

  FW_RegWnd           * fw_reg;
  HWND                  hw_vsb;             //垂直滚动条
  HWND                  hw_reg;
  BOOL                  blDraw;             //是否绘制
  PVOID                 BaseAddr;           //反汇编基址
  UINT                  BlockSize;          //反汇编块大小
  PVOID                 DrawAddr;           //被绘制的地址
  PVOID                 SelectedAddrA;      //所选地址
  PVOID                 SelectedAddrB;      //所选地址
  UINT                  diCount;            //反汇编条目数量
  
  PPDAI                 ItemArray;

  WORD                  drWidth;
  WORD                  drHeight;
  WORD                  MaxItem;
  HeapMemoryManager   * hmm;
  HeapMemoryManager   * smm;

  HWND    CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style);
  PDAI    GetItemByIndex(UINT Idx);
  PDAI    GetItemByAddress(PVOID Address);
  UINT    GetItemIndexByAddress(PVOID Address);
  BOOL    DelItemByAddress(PVOID Address);
  void    DelItemArray();
  void    SetScrollBar(UINT CurrPos, UINT MaxPos);

public:
  MC_Disasm(HWND hMDI) : fw_reg(NULL), blDraw(TRUE), BaseAddr(0), BlockSize(0), DrawAddr(0),
    SelectedAddrA(0), diCount(0), ItemArray(0), drWidth(0), drHeight(0),
    MaxItem(0), hmm(0),  smm(0), MDIForm(hMDI)
  {
    hmm = new HeapMemoryManager(1, 524288);
    smm = new HeapMemoryManager(1, 524288);
  }
  ~MC_Disasm();

  int       AssembleOfAddress(PVOID Addr, t_asmmodel * tasm);
  BOOL      DisasmOfAddress(PBYTE dsAddress, DWORD Flag);
  BOOL      GotoAddress(PBYTE dsAddress, DWORD Flag);
  PVOID     GetSelectLineAddress();
  UINT      GetAsmTextByAddress(PVOID Addr, PTCHAR pat, UINT scount);
  UINT      GetOpLengthByAddress(PVOID Addr);
  void      RedrawForm();
  PDAI      HitTest(WORD px, WORD py);
  static    LRESULT CALLBACK disasmwndProc(HWND, UINT, WPARAM, LPARAM);
};

/*VB结构信息窗口类*/
class MC_VBInfo : public MDIForm
{
private:
  HWND vbsp;
  HWND vbokbtn;
  HWND vbshbtn;
  HWND vbtv;
  HWND vblv;
  HMENU PM_4;
  PVBHD vbhead;
  HTREEITEM hti_struct;
  HTREEITEM hti_object;
  HTREEITEM hti_frm;
  HTREEITEM hti_bas;
  HTREEITEM hti_cls;
  HTREEITEM hti_ctl;

  void ResetControl();
  void vbInsertColumn(DWORD iType);
  void RefreshStructList();

public:
  MC_VBInfo(HWND hMDI) : PM_4(0), vbhead(0), MDIForm(hMDI) {}
  ~MC_VBInfo();

  HWND CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style);
  void ParseHeader(PVBHD pVbHeader);
  void ParseModule(PVOID ModuleBase);
  static LRESULT CALLBACK vbwndProc(HWND, UINT, WPARAM, LPARAM);
};

/*PCode过程窗口类*/
class MC_PCodeProc : public MDIForm
{
private:
  HWND    hObjCbx;
  HWND    hEvtCbx;
  HWND    hList;
  PVBHD   CVBH;
  PPPPL   ProcListHead;
  PPPPL   ProcListLast;

  void    AddProc(PTCHAR pProcName, PVOID pAddr, int cb_idx);
  PVOID   GetProcAddrByName(PTCHAR pProcName);
  PVOID   GetProcAddrByIndex(int cbIdx);
  void    DelAllProc();
  void    InsertColumn();
  void    ClearList();
  void    UpdateList(PVBPDI pVbpDI);

  HWND    CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style);
  void    UpdateProc(PTCHAR pObjName);

public:
  MC_PCodeProc(HWND hMDI) : hObjCbx(0), hEvtCbx(0), hList(0), ProcListHead(0), ProcListLast(0), MDIForm(hMDI) {}
  ~MC_PCodeProc();

  BOOL ParseHeader(PVBHD pVbHeader);
  BOOL ParseProc(PVOID pProc);
  static LRESULT CALLBACK pcpwndProc(HWND, UINT, WPARAM, LPARAM);
};

/* 主窗口类 */
class MainForm
{
private:
  /* 唯一的类实例指针 */
  static MainForm           * p_mf;

  /* 是否初始化 */
  static BOOL                 IsInit;

  /* 调试器状态 */
  SRDBG_STATE                 SRDBGS;

  /* 状态栏文本颜色 */
  static COLORREF             stclr;

  /* 状态栏文本 */
  static TCHAR                StateText[64];
  
  MainForm(HINSTANCE hInst)
  {
    MdlBase = hInst;
    InitVar();
    LoadForm();
  }

  ~MainForm()
  {
    UnloadForm();
    p_mf = NULL;
  }

  void InitVar();
  BOOL LoadForm();
  BOOL DestroyHandle();
  BOOL UnloadForm();

public:
  static HINSTANCE            MdlBase;
  static ATOM                 rcls_sr;
  static ATOM                 rcls_mc_module;
  static ATOM                 rcls_mc_memory;
  static ATOM                 rcls_mc_heap;
  static ATOM                 rcls_mc_thread;
  static ATOM                 rcls_mc_disasm;
  static ATOM                 rcls_mc_vbinfo;
  static ATOM                 rcls_mc_pcodeproc;

  static HICON                hi_main;
  static HICON                hi_module;
  static HICON                hi_memory;
  static HICON                hi_thread;
  static HICON                hi_disasm;
  static HICON                hi_vbp;

  static HMENU                hm_main;
  static HMENU                hm_pop;
  static HMENU                hm_vb;

  static HIMAGELIST           hImageList;

  static int                  bpi_root;
  static int                  bpi_object;
  static int                  bpi_event;
  static int                  bpi_ukctl;
  static int                  bpi_vbs;
  static int                  bpi_frm;
  static int                  bpi_bas;
  static int                  bpi_cls;
  static int                  bpi_ctl;

  static HFONT                hf_sml;
  static HFONT                hf_med;
  static HFONT                hf_lar;
  static HFONT                hf_fix;

  static HWND                 hw_main;
  static HWND                 hw_progress;
  static HWND                 hw_cmdedit;
  static HWND                 hw_MDI;

  static MC_Disasm          * mc_disasm;
  static MC_Module          * mc_module;
  static MC_Memory          * mc_memory;
  static MC_Heap            * mc_heap;
  static MC_Thread          * mc_thread;
  static MC_VBInfo          * mc_vbinfo;
  static MC_PCodeProc       * mc_pcodeproc;

  static MemoryManager      * m_mer;   //内存管理器
  static HeapMemoryManager  * m_hmm;   //堆管理器

  static PVBOBJL              VbObjHeader;
  static PVBOBJL              VbObjLast;

  /* 获取类实例 */
  static MainForm           * GetInstance(HINSTANCE hInst)
  {
    if (p_mf == NULL) p_mf = new MainForm(hInst);
    return p_mf;
  }

  /* 主窗体过程 */
  static LRESULT CALLBACK MainProc(HWND, UINT, WPARAM, LPARAM);

  /* 在全局添加对象链表 */
  PVBOBJL           AddObjectList(PVBOBJL pobjlst);

  /* 根据对象表添加过程表 */
  PVBPL             AddProcListByObject(PVBOBJL pobjlst, PVBPL vbprclst);

  /* 根据对象名称添加过程表 */
  PVBPL             AddProcListByName(PTCHAR pObjName, PVBPL vbprclst);

  /* 根据对象指针获取对象链表 */
  PVBOBJL           GetObjectListByObject(PVBO pVbObj);

  /* 根据对象名称获取对象链表 */
  PVBOBJL           GetObjectListByName(PTCHAR pObjName);

  /* 根据过程描述获取过程链表 */
  PVBPL             GetProcListByAddress(PTCHAR pOptObjName, PVBPDI pProcDesc);

  /* 设置调试器状态文本 */
  int               SetStateText(PTCHAR pText, COLORREF tc);

  /* 设置调试器状态 */
  void              SetDbgState(SRDBG_STATE dbgs);

  /* 获取调试器状态 */
  SRDBG_STATE       GetDbgState();

  /* 创建窗体 */
  HWND              CreateForm(PTCHAR pwName, DWORD dwStyle, HINSTANCE hOwner);

  /* 获取主窗体句柄 */
  HWND              GetMainWindow();

  /* 销毁窗体 */
  BOOL              DestroyForm();

  /* 释放 */
  void              Release();
};