#include "../sr_pch.h"

HINSTANCE             MainForm::MdlBase = 0;
ATOM                  MainForm::rcls_sr = 0;
ATOM                  MainForm::rcls_mc_module = 0;
ATOM                  MainForm::rcls_mc_memory = 0;
ATOM                  MainForm::rcls_mc_heap = 0;
ATOM                  MainForm::rcls_mc_thread = 0;
ATOM                  MainForm::rcls_mc_disasm = 0;
ATOM                  MainForm::rcls_mc_vbinfo = 0;
ATOM                  MainForm::rcls_mc_pcodeproc = 0;

MainForm            * MainForm::p_mf = 0;
BOOL                  MainForm::IsInit = 0;
COLORREF              MainForm::stclr = RGB(0, 0, 0);

HICON                 MainForm::hi_main = 0;
HICON                 MainForm::hi_module = 0;
HICON                 MainForm::hi_memory = 0;
HICON                 MainForm::hi_thread = 0;
HICON                 MainForm::hi_disasm = 0;
HICON                 MainForm::hi_vbp = 0;
HMENU                 MainForm::hm_main = 0;
HMENU                 MainForm::hm_pop = 0;
HMENU                 MainForm::hm_vb = 0;
HIMAGELIST            MainForm::hImageList = 0;

int                   MainForm::bpi_root = -1;
int                   MainForm::bpi_object = -1;
int                   MainForm::bpi_event = -1;
int                   MainForm::bpi_ukctl = -1;
int                   MainForm::bpi_vbs = -1;
int                   MainForm::bpi_frm = -1;
int                   MainForm::bpi_bas = -1;
int                   MainForm::bpi_cls = -1;
int                   MainForm::bpi_ctl = -1;

HFONT                 MainForm::hf_sml = 0;
HFONT                 MainForm::hf_med = 0;
HFONT                 MainForm::hf_lar = 0;
HFONT                 MainForm::hf_fix = 0;

HWND                  MainForm::hw_main = 0;
HWND                  MainForm::hw_progress = 0;
HWND                  MainForm::hw_cmdedit = 0;
HWND                  MainForm::hw_MDI = 0;

TCHAR                 MainForm::StateText[64] = { 0 };

MC_Disasm          *  MainForm::mc_disasm = 0;
MC_Module          *  MainForm::mc_module = 0;
MC_Memory          *  MainForm::mc_memory = 0;
MC_Heap            *  MainForm::mc_heap = 0;
MC_Thread          *  MainForm::mc_thread = 0;
MC_VBInfo          *  MainForm::mc_vbinfo = 0;
MC_PCodeProc       *  MainForm::mc_pcodeproc = 0;

MemoryManager      *  MainForm::m_mer = 0;
HeapMemoryManager  *  MainForm::m_hmm = 0;

PVBOBJL               MainForm::VbObjHeader = 0;
PVBOBJL               MainForm::VbObjLast = 0;

void MainForm::InitVar()
{
  IsInit = FALSE;
  SRDBGS = DS_Idle;

  hi_main = NULL;
  hi_module = NULL;
  hi_memory = NULL;
  hi_thread = NULL;
  hi_disasm = NULL;
  hi_vbp = NULL;

  hm_main = NULL;
  hm_pop = NULL;
  hm_vb = NULL;

  hImageList = NULL;

  bpi_root = -1;
  bpi_object = -1;
  bpi_event = -1;
  bpi_ukctl = -1;
  bpi_vbs = -1;
  bpi_frm = -1;
  bpi_bas = -1;
  bpi_cls = -1;
  bpi_ctl = -1;

  hf_sml = NULL;
  hf_med = NULL;
  hf_lar = NULL;
  hf_fix = NULL;

  rcls_sr = NULL;
  rcls_mc_module = NULL;
  rcls_mc_memory = NULL;
  rcls_mc_heap = NULL;
  rcls_mc_thread = NULL;
  rcls_mc_disasm = NULL;
  rcls_mc_vbinfo = NULL;

  mc_disasm = NULL;
  mc_module = NULL;
  mc_memory = NULL;
  mc_heap = NULL;
  mc_thread = NULL;
  mc_vbinfo = NULL;
  m_mer = NULL;
  m_hmm = NULL;

  VbObjHeader = 0;
  VbObjLast = 0;
}

BOOL MainForm::LoadForm()
{
  BOOL ret = TRUE;
  WNDCLASSEX wcex;

  if (IsInit) return ret;

  /*加载基本图标*/
  _LoadIcon_16x16(WND_ICON, hi_main, ret);
  _LoadIcon_16x16(MDL_ICON, hi_module, ret);
  _LoadIcon_16x16(MEM_ICON, hi_memory, ret);
  _LoadIcon_16x16(THREAD_ICON, hi_thread, ret);
  _LoadIcon_16x16(DISASM_ICON, hi_disasm, ret);
  _LoadIcon_16x16(VBP_ICON, hi_vbp, ret);

  /*加载菜单*/
  hm_main = LoadMenu(DllBase, MAKEINTRESOURCE(MAIN_MENU));
  hm_pop = LoadMenu(DllBase, MAKEINTRESOURCE(POP_MENU));
  hm_vb = LoadMenu(DllBase, MAKEINTRESOURCE(VB_MENU));

  if (!hm_main || !hm_pop || !hm_vb)
    return DestroyHandle();

  /*创建ImageList*/
  hImageList = ImageList_Create(16, 16, ILC_COLOR32, 10, 1);
  if (!hImageList)
    return DestroyHandle();

  /*添加图像到ImageList*/
  bpi_root = ImageList_Add32bpp(hImageList, DllBase, IDB_ROOT, IMAGE_BITMAP);
  bpi_object = ImageList_Add32bpp(hImageList, DllBase, IDB_OBJECT, IMAGE_BITMAP);
  bpi_event = ImageList_Add32bpp(hImageList, DllBase, IDB_EVENT, IMAGE_BITMAP);
  bpi_ukctl = ImageList_Add32bpp(hImageList, DllBase, IDB_UKCTL, IMAGE_BITMAP);
  bpi_vbs = ImageList_Add32bpp(hImageList, DllBase, IDB_VBS, IMAGE_BITMAP);
  bpi_frm = ImageList_Add32bpp(hImageList, DllBase, IDB_FRM, IMAGE_BITMAP);
  bpi_bas = ImageList_Add32bpp(hImageList, DllBase, IDB_BAS, IMAGE_BITMAP);
  bpi_cls = ImageList_Add32bpp(hImageList, DllBase, IDB_CLS, IMAGE_BITMAP);
  bpi_ctl = ImageList_Add32bpp(hImageList, DllBase, IDB_CTL, IMAGE_BITMAP);

  /*创建字体*/
  hf_sml = CreateFont(13, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, FT_TAH);
  hf_med = CreateFont(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, FT_TAH);
  hf_lar = CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, FT_TAH);
  hf_fix = CreateFont(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, FT_FIX);

  /*注册窗口类*/
  ZeroMemory(&wcex, sizeof(wcex));
  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
  wcex.lpfnWndProc = this->MainProc;
  wcex.hInstance = DllBase;
  wcex.hIcon = hi_main;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = WC_Main;
  wcex.hIconSm = hi_main;
  rcls_sr = RegisterClassEx(&wcex);

  wcex.lpfnWndProc = MC_Module::mdlwndProc;
  wcex.hInstance = DllBase;
  wcex.hIcon = hi_module;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = WC_Module;
  wcex.hIconSm = hi_module;
  rcls_mc_module = RegisterClassEx(&wcex);

  wcex.lpfnWndProc = MC_Memory::memwndProc;
  wcex.hInstance = DllBase;
  wcex.hIcon = hi_memory;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = WC_Memory;
  wcex.hIconSm = hi_memory;
  rcls_mc_memory = RegisterClassEx(&wcex);

  wcex.lpfnWndProc = MC_Heap::heapwndProc;
  wcex.hInstance = DllBase;
  wcex.hIcon = NULL;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = WC_Heap;
  wcex.hIconSm = NULL;
  rcls_mc_heap = RegisterClassEx(&wcex);

  wcex.lpfnWndProc = MC_Thread::threadwndProc;
  wcex.hInstance = DllBase;
  wcex.hIcon = hi_thread;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = WC_Thread;
  wcex.hIconSm = hi_thread;
  rcls_mc_thread = RegisterClassEx(&wcex);

  wcex.lpfnWndProc = MC_Disasm::disasmwndProc;
  wcex.hInstance = DllBase;
  wcex.hIcon = hi_disasm;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = WC_Disasm;
  wcex.hIconSm = hi_disasm;
  rcls_mc_disasm = RegisterClassEx(&wcex);

  wcex.lpfnWndProc = MC_VBInfo::vbwndProc;
  wcex.hInstance = DllBase;
  wcex.hIcon = hi_vbp;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = WC_VB;
  wcex.hIconSm = hi_vbp;
  rcls_mc_vbinfo = RegisterClassEx(&wcex);

  wcex.lpfnWndProc = MC_PCodeProc::pcpwndProc;
  wcex.hInstance = DllBase;
  wcex.hIcon = hi_vbp;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = WC_PCP;
  wcex.hIconSm = hi_vbp;
  rcls_mc_pcodeproc = RegisterClassEx(&wcex);

  m_hmm = new HeapMemoryManager(0, 8097152);

  if (!rcls_sr || !rcls_mc_module || !rcls_mc_memory || !rcls_mc_heap ||
    !rcls_mc_thread || !rcls_mc_vbinfo || !rcls_mc_pcodeproc)
    return DestroyHandle();
  IsInit = TRUE;
  return ret;
}

/*销毁句柄,只返回FALSE*/
BOOL MainForm::DestroyHandle()
{
  if (m_mer) delete m_mer;
  if (m_hmm) delete m_hmm;
  /*删除子窗口对象*/
  if (mc_disasm) delete mc_disasm;
  if (mc_module) delete mc_module;
  if (mc_memory) delete mc_memory;
  if (mc_heap) delete mc_heap;
  if (mc_thread) delete mc_thread;
  if (mc_vbinfo) delete mc_vbinfo;
  if (mc_pcodeproc) delete mc_pcodeproc;
  /*删除字体*/
  if (hf_sml) DeleteObject(hf_sml);
  if (hf_med) DeleteObject(hf_med);
  if (hf_lar) DeleteObject(hf_lar);
  if (hf_fix) DeleteObject(hf_fix);
  /*销毁ImageList*/
  if (hImageList) ImageList_Destroy(hImageList);
  /*销毁HMENU*/
  if (hm_main) DestroyMenu(hm_main);
  if (hm_pop) DestroyMenu(hm_pop);
  if (hm_vb) DestroyMenu(hm_vb);
  /*销毁HICON*/
  if (hi_main) DestroyIcon(hi_main);
  if (hi_module) DestroyIcon(hi_module);
  if (hi_memory) DestroyIcon(hi_memory);
  if (hi_thread) DestroyIcon(hi_thread);
  if (hi_disasm) DestroyIcon(hi_disasm);
  /*取消注册窗口类*/
  if (rcls_sr) UnregisterClass((LPCTSTR)rcls_sr, DllBase);
  if (rcls_mc_module) UnregisterClass((LPCTSTR)rcls_mc_module, DllBase);
  if (rcls_mc_memory) UnregisterClass((LPCTSTR)rcls_mc_memory, DllBase);
  if (rcls_mc_heap) UnregisterClass((LPCTSTR)rcls_mc_heap, DllBase);
  if (rcls_mc_thread) UnregisterClass((LPCTSTR)rcls_mc_thread, DllBase);
  if (rcls_mc_vbinfo) UnregisterClass((LPCTSTR)rcls_mc_vbinfo, DllBase);
  if (rcls_mc_pcodeproc) UnregisterClass((LPCTSTR)rcls_mc_pcodeproc, DllBase);

  InitVar();
  return FALSE;
}

BOOL MainForm::UnloadForm()
{
  if (IsInit)
    DestroyHandle();
  return TRUE;
}

void MainForm::Release()
{
  UnloadForm();
  delete this;
}

HWND MainForm::CreateForm(PTCHAR pwName, DWORD dwStyle, HINSTANCE hOwner)
{
  if (!IsInit)
    return 0;

  if (hw_main == NULL)
    hw_main = CreateWindowEx(WS_EX_WINDOWEDGE, (PTCHAR)rcls_sr, pwName, dwStyle, CW_USEDEFAULT, 0, 1200, 600, 0, hm_main, hOwner, 0);
  UpdateWindow(hw_main);
  ShowWindow(hw_main, SW_SHOWMAXIMIZED);
  return hw_main;
}

HWND MainForm::GetMainWindow()
{
  return hw_main;
}

LRESULT CALLBACK MainForm::MainProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  switch (Msg)
  {
  case WM_CREATE:
  {
    CLIENTCREATESTRUCT CCS;

    CCS.idFirstChild = 5000;
    CCS.hWindowMenu = NULL;

    hw_progress = CreateWindowEx(WS_EX_CLIENTEDGE, PROGRESS_CLASS, _T("sr_progress"),
      WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 0, 0, 0, 0, hWnd, 0, DllBase, NULL);

    hw_cmdedit = CreateWindowEx(WS_EX_CLIENTEDGE, CN_Edit, NULL,
      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, DllBase, NULL);

    hw_MDI = CreateWindowEx(WS_EX_STATICEDGE, CN_MDI, NULL,
      WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE, 0, 0, 0, 0, hWnd, 0, DllBase, &CCS);

    SendMessage(hw_cmdedit, WM_SETFONT, (WPARAM)hf_lar, TRUE);
    SendMessage(hw_cmdedit, EM_SETLIMITTEXT, 256, 0);
    SendMessage(hw_MDI, WM_SETFONT, (WPARAM)hf_med, TRUE);

    mc_module = new MC_Module(hw_MDI);
    mc_memory = new MC_Memory(hw_MDI);
    mc_disasm = new MC_Disasm(hw_MDI);
    mc_heap = new MC_Heap(hw_MDI);
    mc_thread = new MC_Thread(hw_MDI);
    mc_vbinfo = new MC_VBInfo(hw_MDI);
    mc_pcodeproc = new MC_PCodeProc(hw_MDI);
    m_mer = new MemoryManager();
  }
    break;
  case WM_PAINT:
  {
    PAINTSTRUCT ps;
    SRDBG_STATE nds = mf->GetDbgState();
    HDC hDC = BeginPaint(hWnd, &ps);

    if (!hDC) break;
    SelectObject(hDC, hf_med);

    SetBkMode(hDC, TRANSPARENT);

    if (nds == DS_Idle)
    {
      SetTextColor(hDC, mf->stclr);
      if (_tcslen(StateText))
        TextOut(hDC, 2, ps.rcPaint.bottom - 20, StateText, _tcslen(StateText));
    }
    else if (nds == DS_Busy)
    {
      SetTextColor(hDC, RGB(223, 31, 63));
      TextOut(hDC, 2, ps.rcPaint.bottom - 20, T_DBGS_BUSY, _tcslen(T_DBGS_BUSY));
    }
    EndPaint(hWnd, &ps);
  }
    break;
  case WM_COMMAND:
  {
    switch (LOWORD(wParam))
    {
    case MENU_ABOUT:
      DialogBox(DllBase, MAKEINTRESOURCE(ABOUT_DLG), MainForm::hw_main, (DLGPROC)AboutProc);
      return 0;
    case MENU_MODULE:
      if (mc_module->GetMainhWnd())
        mc_module->SetTop();
      else
        mc_module->CreateForm(CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, WN_Module, 0);
      return 0;
    case MENU_MEMORY:
      if (mc_memory->GetMainhWnd())
        mc_memory->SetTop();
      else
        mc_memory->CreateForm(CW_USEDEFAULT, CW_USEDEFAULT, 1000, 500, WN_Memory, 0);
      return 0;
    case MENU_ENUMHEAP:
      if (mc_heap->GetMainhWnd())
        mc_heap->SetTop();
      else
        mc_heap->CreateForm(CW_USEDEFAULT, CW_USEDEFAULT, 800, 400, WN_Heap, 0);
      return 0;
    case MENU_THREAD:
      if (mc_thread->GetMainhWnd())
        mc_thread->SetTop();
      else
        mc_thread->CreateForm(CW_USEDEFAULT, CW_USEDEFAULT, 800, 400, WN_Thread, 0);
      return 0;
    case MENU_DISASM:
      if (mc_disasm->GetMainhWnd())
        mc_vbinfo->SetTop();
      else
        mc_disasm->GotoAddress((PBYTE)GetModuleEntry(GetModuleHandle(NULL)), 0);
      return 0;
    case MENU_VBANALYSIS:
      if (mc_vbinfo->GetMainhWnd())
        mc_vbinfo->SetTop();
      else
      {
        mc_vbinfo->CreateForm(CW_USEDEFAULT, CW_USEDEFAULT, 800, 400, WN_VB, 0);
        mc_vbinfo->ParseModule(GetModuleHandle(NULL));
      }
      return 0;
    case MENU_PCODEANS:
      if (mc_pcodeproc->GetMainhWnd())
        mc_pcodeproc->SetTop();
      else
        mc_pcodeproc->ParseHeader(GetVbHeaderByModule(GetModuleHandle(NULL)));
      return 0;
    }
  }
    break;
  case WM_SIZE:
    if (wParam != SIZE_MINIMIZED)
    {
      WORD Width = LOWORD(lParam);
      WORD Height = HIWORD(lParam);

      MoveWindow(hw_progress, Width - 400, Height - 22, 399, 20, TRUE);
      MoveWindow(hw_cmdedit, Width - 600, Height - 22, 199, 20, TRUE);

      MoveWindow(hw_MDI, 1, 1, Width - 2, Height - 24, TRUE);

      return 0;
    }
    break;
  case WM_SIZING:
  {
    if (wParam >= WMSZ_LEFT && wParam <= WMSZ_BOTTOMRIGHT)
    {
      LPRECT lprc = (LPRECT)lParam;
      long Width = lprc->right - lprc->left;
      long Height = lprc->bottom - lprc->top;

      if (Width < 950) lprc->right = lprc->left + 950;
      if (Height < 500) lprc->bottom = lprc->top + 500;
    }
  }
    return 1;
  case WM_CLOSE:
    DestroyWindow(MainForm::hw_main);
    return 0;
  case WM_DESTROY:
    MainForm::hw_main = NULL;
    if (mc_disasm) delete mc_disasm;
    if (mc_module) delete mc_module;
    if (mc_memory) delete mc_memory;
    if (mc_heap) delete mc_heap;
    if (mc_thread) delete mc_thread;
    if (mc_vbinfo) delete mc_vbinfo;
    if (m_mer) delete m_mer;

    mc_disasm = NULL;
    mc_module = NULL;
    mc_memory = NULL;
    mc_heap = NULL;
    mc_thread = NULL;
    mc_vbinfo = NULL;
    m_mer = NULL;

    PostQuitMessage(0);
    return 0;
  }

  return DefFrameProc(hWnd, hw_MDI, Msg, wParam, lParam);
}

int MainForm::SetStateText(PTCHAR pText, COLORREF tc)
{
  int ret;
  RECT rect;

  if (!_tcslen(pText) || !hw_main) return 0;
  stclr = tc;
  ret = _tcscpy_s(StateText, 64, pText);

  if (GetClientRect(hw_main, &rect))
  {
    rect.top = rect.bottom - 22;
    if (rect.right > 600) rect.right -= 600;

    InvalidateRect(hw_main, &rect, TRUE);
    UpdateWindow(hw_main);
  }
  return ret;
}

void MainForm::SetDbgState(SRDBG_STATE dbgs)
{
  RECT rect;
  SRDBGS = dbgs;

  if (!hw_main) return;
  if (GetClientRect(hw_main, &rect))
  {
    rect.top = rect.bottom - 22;
    if (rect.right > 600) rect.right -= 600;

    InvalidateRect(hw_main, &rect, TRUE);
    UpdateWindow(hw_main);
  }
}

SRDBG_STATE MainForm::GetDbgState()
{
  return SRDBGS;
}

PVBOBJL MainForm::AddObjectList(PVBOBJL pVBOBJL)
{
  if (!pVBOBJL || pVBOBJL->Next) return NULL;

  PVBOBJL ptmpobj = (PVBOBJL)m_hmm->hmalloc(sizeof(VBOBJL));
  if (!ptmpobj) return NULL;
  memcpy_s(ptmpobj, sizeof(VBOBJL), pVBOBJL, sizeof(VBOBJL));

  if (VbObjLast)
  {
    VbObjLast->Next = ptmpobj;
    VbObjLast = ptmpobj;
  }
  else
  {
    VbObjHeader = ptmpobj;
    VbObjLast = ptmpobj;
  }

  return ptmpobj;
}

PVBPL MainForm::AddProcListByObject(PVBOBJL pobjlst, PVBPL prclst)
{
  if (!pobjlst || !prclst || prclst->Next) return NULL;

  PVBPL ptmppl = (PVBPL)mf->m_hmm->hmalloc(sizeof(VBPL));
  if (!ptmppl) return NULL;
  memcpy_s(ptmppl, sizeof(VBPL), prclst, sizeof(VBPL));

  if (pobjlst->pl_Last)
  {
    pobjlst->pl_Last->Next = ptmppl;
    pobjlst->pl_Last = ptmppl;
  }
  else
  {
    pobjlst->pl_Head = ptmppl;
    pobjlst->pl_Last = ptmppl;
  }

  return ptmppl;
}

PVBPL MainForm::AddProcListByName(PTCHAR pObjName, PVBPL prclst)
{
  if (!pObjName || !prclst || prclst->Next) return NULL;
  PVBOBJL pVbObjL = VbObjHeader;

  while (pVbObjL)
  {
    if (_tcscmp(pObjName, pVbObjL->ObjName) == 0)
      return AddProcListByObject(pVbObjL, prclst);
    pVbObjL = pVbObjL->Next;
  }
  return NULL;
}

PVBOBJL MainForm::GetObjectListByObject(PVBO pVbObj)
{
  if (!pVbObj || !VbObjHeader) return NULL;
  PVBOBJL pVbObjL = VbObjHeader;

  while (pVbObjL)
  {
    if (pVbObjL->VbObj == pVbObj)
      return pVbObjL;
    pVbObjL = pVbObjL->Next;
  }
  return NULL;
}

PVBOBJL MainForm::GetObjectListByName(PTCHAR pObjName)
{
  if (!pObjName || !VbObjHeader) return NULL;
  PVBOBJL pVbObjL = VbObjHeader;

  while (pVbObjL)
  {
    if (_tcscmp(pObjName, pVbObjL->ObjName) == 0)
      return pVbObjL;
    pVbObjL = pVbObjL->Next;
  }
  return NULL;
}

PVBPL MainForm::GetProcListByAddress(PTCHAR pOptObjName, PVBPDI pProcDesc)
{
  if (!pProcDesc || !VbObjHeader) return NULL;
  PVBOBJL pVbObjL = VbObjHeader;
  PVBPL pVBPL;

  while (pVbObjL)
  {
    if (pOptObjName)
    {
      if (_tcscmp(pOptObjName, pVbObjL->ObjName) == 0)
      {
        pVBPL = pVbObjL->pl_Head;
        while (pVBPL)
        {
          if (pVBPL->ProcDesc == pProcDesc)
            return pVBPL;
          pVBPL = pVBPL->Next;
        }
        return NULL;
      }
    }
    else
    {
      pVBPL = pVbObjL->pl_Head;
      while (pVBPL)
      {
        if (pVBPL->ProcDesc == pProcDesc)
          return pVBPL;
        pVBPL = pVBPL->Next;
      }
    }
    pVbObjL = pVbObjL->Next;
  }

  return NULL;
}