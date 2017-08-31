#include "../sr_pch.h"

MC_PCodeProc::~MC_PCodeProc()
{
  if (this->m_hWnd)
  {
    DestroyWindow(this->m_hWnd);
    this->m_hWnd = NULL;
  }
}

HWND MC_PCodeProc::CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style)
{
  if (this->m_hWnd)
  {
    this->SetTop();
    return this->m_hWnd;
  }

  MDICREATESTRUCT MCS;

  MCS.szClass = (PTCHAR)mf->rcls_mc_pcodeproc;
  MCS.szTitle = wName;
  MCS.hOwner = mf->MdlBase;
  MCS.style = Style;
  MCS.cx = cx;
  MCS.cy = cy;
  MCS.x = x;
  MCS.y = y;
  MCS.lParam = (LPARAM)this;

  this->m_hWnd = (HWND)::SendMessage(hw_MDI, WM_MDICREATE, 0, (LPARAM)&MCS);
  return this->m_hWnd;
}

void MC_PCodeProc::AddProc(PTCHAR pProcName, PVOID pAddr, int cb_idx)
{
  if (!pProcName || !pAddr) return;
  PPPPL ptmp = (PPPPL)malloc(sizeof(PPPL));
  if (!ptmp) return;

  ptmp->pAddr = pAddr;
  ptmp->cbidx = cb_idx;
  ptmp->Next = NULL;
  _tcscpy_s(ptmp->ProcName, 256, pProcName);

  if (ProcListLast)
  {
    ProcListLast->Next = ptmp;
    ProcListLast = ptmp;
  }
  else
  {
    ProcListHead = ptmp;
    ProcListLast = ptmp;
  }
}

PVOID MC_PCodeProc::GetProcAddrByName(PTCHAR pProcName)
{
  PPPPL pl = ProcListHead;

  while (pl)
  {
    if (_tcscmp(pProcName, pl->ProcName) == 0)
      return pl->pAddr;
    pl = pl->Next;
  }
  return 0;
}

PVOID MC_PCodeProc::GetProcAddrByIndex(int cbIdx)
{
  PPPPL pl = ProcListHead;

  if (cbIdx < 0) return 0;
  while (pl)
  {
    if (cbIdx == pl->cbidx)
      return pl->pAddr;
    pl = pl->Next;
  }
  return 0;
}

void MC_PCodeProc::DelAllProc()
{
  PPPPL pl = ProcListHead;
  PPPPL pd;

  while (pl)
  {
    pd = pl;
    pl = pl->Next;
    free(pd);
  }

  ProcListHead = NULL;
  ProcListLast = NULL;
}

void MC_PCodeProc::UpdateProc(PTCHAR pObjName)
{
  ComboBox_DeleteAll(hEvtCbx);
  DelAllProc();
  PVBOBJL pobjlst = mf->GetObjectListByName(pObjName);    //控件名取指针
  if (!pobjlst) return;

  PVBOOI pooi;    //VB对象可选信息头
  PVBC pctl;      //VB控件
  PPVBEP ppep;    //事件指针
  DWORD j;
  PDWORD pdw = NULL; //检测重复
  DWORD dcount = 0;
  int cbidx;
  UINT mbase;
  UINT msize;
  WORD i;
  TCHAR ptmp0[64];
  TCHAR ptmp1[64];
  TCHAR ptmp2[64];

  if (!HitModule((PVOID)(pobjlst->VbHead), &mbase, &msize)) return;

  //被点击的对象
  pooi = (PVBOOI)((DWORD)pobjlst->VbObj->pObjectInfo + sizeof(VBOI));
  pctl = pooi->pControlArray;

  if (pctl)
  {
    //循环控件事件
    for (i = 0; i < pooi->ControlCount; i++)
    {
      ppep = (PPVBEP)((DWORD)pctl[i].pEventTable + sizeof(VB_EVENT_TABLE));
      mf->SetDbgState(DS_Busy);

      for (j = 0; j < pctl[i].EventCount; j++)
      {
        //如果该事件地址和描述地址不为NULL
        if (ppep[j] == NULL || ppep[j]->pDescInfo == NULL) continue;

        PVBPL vbprclst;
        size_t nocc;

        pdw = (PDWORD)_recalloc(pdw, ++dcount, sizeof(DWORD));
        if (!pdw) return;

        if (vbprclst = mf->GetProcListByAddress(pobjlst->ObjName, ppep[j]->pDescInfo))
          _tcscpy_s(ptmp0, 64, vbprclst->EvtName);
        else
        {
          if (!GetMemberNameByIndex(*pctl[i].pGUID, (DWORD)j, ptmp1, 64, 0))
            _stprintf_s(ptmp0, 64, _T("Event_%08X"), ppep[j]->pDescInfo);
          else
          {
            mbstowcs_s(&nocc, ptmp2, (size_t)64, pctl[i].pName, _TRUNCATE);
            _stprintf_s(ptmp0, 64, _T("%s_%s"), ptmp2, ptmp1);
          }
        }
        pdw[dcount - 1] = (DWORD)ppep[j]->pDescInfo;
        cbidx = SendMessage(hEvtCbx, CB_ADDSTRING, 0, (LPARAM)ptmp0);
        if (cbidx >= 0)
          AddProc(ptmp0, (PBYTE)ppep[j]->pDescInfo, cbidx);
      }

      mf->SetDbgState(DS_Idle);
    }
  }

  mf->SetDbgState(DS_Busy);

  /* 如果存在模块过程 */
  if (pobjlst->VbObj->pObjectInfo->NumberOfProcs || pobjlst->VbObj->pObjectInfo->pProcArray)
  {
    /* 循环模块过程 */
    for (j = 0; j < pobjlst->VbObj->pObjectInfo->NumberOfProcs; j++)
    {
      DWORD pAddr = pobjlst->VbObj->pObjectInfo->pProcArray[j];

      if (pAddr == 0 || pAddr == (DWORD)-1 || pAddr < mbase || (pAddr >= mbase + msize)) continue;

      if (pdw && dcount)
      {
        DWORD di;
        BOOL ble = FALSE;

        for (di = 0; di < dcount; di++)
        {
          if (pAddr == pdw[di])
          {
            ble = TRUE;
            break;
          }
        }

        if (ble) break;
      }

      _stprintf_s(ptmp0, 64, _T("Proc_%08X"), pAddr);
      cbidx = SendMessage(hEvtCbx, CB_ADDSTRING, 0, (LPARAM)ptmp0);
      if (cbidx >= 0) AddProc(ptmp0, (PVOID)pAddr, cbidx);
    }
  }
  mf->SetDbgState(DS_Idle);
  if (pdw) free(pdw);
}

BOOL MC_PCodeProc::ParseHeader(PVBHD pVbHeader)
{
  if (!pVbHeader)
  {
    mf->SetStateText(_T("无效的VB头部指针!"), RGB(255, 0, 0));
    return FALSE;
  }

  if (pVbHeader->Sign.dwSign != VB_MAGIC)
  {
    mf->SetStateText(_T("VB结构标志不正确!"), RGB(255, 0, 0));
    return FALSE;
  }

  if (m_hWnd) SetTop();
  else CreateForm(CW_USEDEFAULT, CW_USEDEFAULT, 820, 420, WN_PCP, 0);

  PVBOBJL pVbObjL;
  PVBO pVbObject;
  SIZE_T BVal;
  WORD ObjCount;
  WORD wi;

  mf->SetDbgState(DS_Busy);
  pVbObject = pVbHeader->pProjInfo->pObjectTable->pObject;
  ObjCount = pVbHeader->pProjInfo->pObjectTable->ObjectCount1;

  for (wi = 0; wi < ObjCount; wi++)
  {
    PVBOBJL pVbObjL;

    if (!(pVbObjL = mf->GetObjectListByObject(&pVbObject[wi])))
    {
      VBOBJL VbObjL;
      PTCHAR pstmp;
      size_t slen = strlen(pVbObject[wi].aObjectName);

      memset(&VbObjL, 0, sizeof(VbObjL));

      if (!slen)
      {
        pstmp = (PTCHAR)mf->m_hmm->hcalloc(16, sizeof(TCHAR));
        _stprintf_s(pstmp, 16, _T("_obj_%hu"), pVbObject[wi].pObjectInfo->ObjectIndex);
      }
      else
      {
        pstmp = (PTCHAR)mf->m_hmm->hcalloc(slen + 1);
        mbstowcs_s((size_t *)&BVal, pstmp, slen + 1, pVbObject[wi].aObjectName, _TRUNCATE);
      }

      VbObjL.VbHead = pVbHeader;
      VbObjL.VbObj = &pVbObject[wi];
      VbObjL.iIndex = pVbObject[wi].pObjectInfo->ObjectIndex;
      VbObjL.ObjName = pstmp;

      pVbObjL = mf->AddObjectList(&VbObjL);
    }

    SendMessage(hObjCbx, CB_ADDSTRING, 0, (LPARAM)pVbObjL->ObjName);
  }

  mf->SetDbgState(DS_Idle);
  return TRUE;
}

/*
void aaa()
{
PVBOOI pooi;    //VB对象可选信息头
PVBC pctl;
PPVBEP ppep;
WORD wj;
WORD wk;
pooi = (PVBOOI)((DWORD)pVbObject[wi].pObjectInfo + sizeof(VBOI));
pctl = pooi->pControlArray;

for (wj = 0; wj < pooi->ControlCount; wj++)
{
ppep = (PPVBEP)((DWORD)pctl[wj].pEventTable + sizeof(VB_EVENT_TABLE));
for (wk = 0; wk < pctl[wj].EventCount; wk++)
{
if (ppep[wk] == NULL || ppep[wk]->pDescInfo == NULL) continue;
PVBPL pVbProcL;

if (!(pVbProcL = mf->GetProcListByAddress(pVbObjL->ObjName, ppep[wk]->pDescInfo)))
{
PTCHAR pstmp;
PVOID TT;
DWORD LE;
VBPL VbProcL;
TCHAR entmp[64];

memset(&VbProcL, 0, sizeof(VBPL));
pstmp = (PTCHAR)mf->mhmm->hcalloc(64, sizeof(TCHAR));

if (!GetMemberNameByIndex(*pctl[wj].pGUID, (DWORD)wk, entmp, 64, 0))
_stprintf_s(entmp, 64, _T("Event_%hu"), wk);
_stprintf_s(pstmp, 64, _T("%s_%s"), pVbObjL->ObjName, entmp);
VbProcL.pVbCtl = &pctl[wj];
VbProcL.iIndex = wk;
VbProcL.ProcDesc = ppep[wk]->pDescInfo;
VbProcL.EvtName = pstmp;
pVbProcL = mf->AddProcListByObject(pVbObjL, &VbProcL);
}
SendMessage(hEvtCbx, CB_ADDSTRING, 0, (LPARAM)pVbProcL->EvtName);
}
}
}
*/

void MC_PCodeProc::InsertColumn()
{
  LV_COLUMN lvc;
  ZeroMemory(&lvc, sizeof(LV_COLUMN));
  lvc.mask = LVCF_TEXT | LVCF_WIDTH;

  lvc.pszText = _T("标志");
  lvc.cx = 48;
  ListView_InsertColumn(hList, 0, &lvc);

  lvc.pszText = _T("地址");
  lvc.cx = DefWidth;
  ListView_InsertColumn(hList, 1, &lvc);

  lvc.pszText = _T("助记符");
  lvc.cx = 256;
  ListView_InsertColumn(hList, 2, &lvc);

  lvc.pszText = _T("相关引用");
  lvc.cx = 256;
  ListView_InsertColumn(hList, 3, &lvc);
}

void MC_PCodeProc::ClearList()
{
  ListView_DeleteAllItems(hList);
}

void MC_PCodeProc::UpdateList(PVBPDI ppdi /* 过程描述 */)
{
  UINT mb;
  UINT ms;
  BOOL blInit = TRUE;
  int bval;
  UINT lIdx = 0;
  PDO PCOBJ;
  LV_ITEM lvi;
  TCHAR stmp[128];
  TCHAR snote[128];
  TCHAR haddr[10];

  if (!HitModule(ppdi, &mb, &ms)) return;
  //清除全部列表项
  ClearList();

  pcd_initdeobj(&PCOBJ, stmp, 128, (PBYTE)((UINT)ppdi - ppdi->ProcSize), ppdi->ProcSize, mb, ms, ppdi);
  PCOBJ.szNote = snote;
  PCOBJ.sznlen = 128;

  while ((bval = pcd_disassemble(&PCOBJ, &blInit)) == 0)
  {
    lvi.mask = LVIF_TEXT | LVIF_PARAM;
    lvi.iItem = lIdx;

    lvi.iSubItem = 0;
    lvi.pszText = T_NULL;
    lvi.lParam = (LPARAM)((UINT)PCOBJ.inp_opbuf + PCOBJ.base);
    ListView_InsertItem(hList, &lvi);

    lvi.iSubItem = 1;
    lvi.mask = LVIF_TEXT;
    lvi.pszText = haddr;
    _stprintf_s(haddr, 10, HEXFORMAT, lvi.lParam);
    ListView_SetItem(hList, &lvi);

    lvi.iSubItem = 2;
    lvi.pszText = PCOBJ.szMnem;
    ListView_SetItem(hList, &lvi);

    lvi.iSubItem = 3;
    lvi.pszText = PCOBJ.szNote;
    ListView_SetItem(hList, &lvi);

    lIdx++;
  }

  if (bval != 1)
  {
    _stprintf_s(stmp, 128, _T("解析P-CODE错误! ErrorCode:[%d]"), bval);
    MessageBeep(MB_ICONHAND);
    mf->SetStateText(stmp, RGB(255, 0, 0));
  }
  else
  {
    lvi.mask = LVIF_TEXT | LVIF_PARAM;
    lvi.iItem = lIdx;

    lvi.iSubItem = 0;
    lvi.pszText = T_NULL;
    lvi.lParam = (LPARAM)((UINT)PCOBJ.inp_opbuf + PCOBJ.base);
    ListView_InsertItem(hList, &lvi);

    lvi.iSubItem = 1;
    lvi.mask = LVIF_TEXT;
    lvi.pszText = haddr;
    _stprintf_s(haddr, 10, HEXFORMAT, lvi.lParam);
    ListView_SetItem(hList, &lvi);

    lvi.iSubItem = 2;
    lvi.pszText = PCOBJ.szMnem;
    ListView_SetItem(hList, &lvi);

    lvi.iSubItem = 3;
    lvi.pszText = PCOBJ.szNote;
    ListView_SetItem(hList, &lvi);
  }
  return;
}

LRESULT CALLBACK MC_PCodeProc::pcpwndProc(HWND phWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  MC_PCodeProc *This = (MC_PCodeProc *)GetClassPointer(phWnd);

  switch (Msg)
  {
  case WM_CREATE:
  {
    LPMDICREATESTRUCT PMCS = *(LPMDICREATESTRUCT *)lParam;
    LONG ExtStyle = GetWindowLong(phWnd, GWL_EXSTYLE);

    This = (MC_PCodeProc *)PMCS->lParam;
    AddClassTable(phWnd, (PVOID)This);

    ExtStyle |= WS_EX_CLIENTEDGE;
    SetWindowLong(phWnd, GWL_EXSTYLE, ExtStyle);

    This->hObjCbx = CreateWindow(WC_COMBOBOX, NULL, CBS_DROPDOWNLIST | WS_VSCROLL | DEF_VCHILD,
      54, 3, 160, 20, phWnd, 0, DllBase, 0);
    This->hEvtCbx = CreateWindow(WC_COMBOBOX, NULL, CBS_DROPDOWNLIST | WS_VSCROLL | DEF_VCHILD,
      274, 3, 320, 20, phWnd, 0, DllBase, 0);

    This->hList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, LV_STYLE, 0, 0, 0, 0, phWnd, NULL, DllBase, 0);

    ListView_SetExtendedListViewStyle(This->hList, LVS_EX_FULLROWSELECT);

    //SendMessage(phWnd, WM_SETFONT, (WPARAM)mf->hf_fix, 1);

    if (This->hObjCbx)
    {
      SendMessage(This->hObjCbx, WM_SETFONT, (WPARAM)mf->hf_fix, 1);
      SendMessage(This->hObjCbx, CB_SETITEMHEIGHT, -1, 20);
    }

    if (This->hEvtCbx)
    {
      SendMessage(This->hEvtCbx, WM_SETFONT, (WPARAM)mf->hf_fix, 1);
      SendMessage(This->hEvtCbx, CB_SETITEMHEIGHT, -1, 20);
    }

    if (This->hList)
      SendMessage(This->hList, WM_SETFONT, (WPARAM)mf->hf_lar, 1);

    This->InsertColumn();
  }
    return 0;
  case WM_PAINT:
  {
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(phWnd, &ps);

    if (!hDC) break;
    SelectObject(hDC, MainForm::hf_fix);
    SetTextColor(hDC, RGB(0, 0, 63));
    TextOut(hDC, 4, 8, _T("对象:"), 3);
    TextOut(hDC, 226, 8, _T("过程:"), 3);
    EndPaint(phWnd, &ps);
  }
    return 0;
  case WM_COMMAND:
  {
    WORD wid = LOWORD(wParam);
    WORD wnc = HIWORD(wParam);

    switch (wnc)
    {
    case CBN_SELCHANGE:
    {
      if (This->hObjCbx == (HWND)lParam)
      {
        This->ClearList();

        int sLen = SendMessage(This->hObjCbx, WM_GETTEXTLENGTH, 0, 0);
        PTCHAR pstmp;

        if (sLen <= 0) return 0;
        pstmp = (PTCHAR)calloc(sLen + 2, sizeof(TCHAR));
        if (!pstmp) return 0;

        GetWindowText(This->hObjCbx, pstmp, sLen + 2);
        This->UpdateProc(pstmp);

        free(pstmp);
        return 0;
      }
      else if (This->hEvtCbx == (HWND)lParam)
      {
        int cIdx = SendMessage(This->hEvtCbx, CB_GETCURSEL, 0, 0);
        if (cIdx < 0) return 0;
        PVOID pAddr = This->GetProcAddrByIndex(cIdx);
        if (pAddr) This->UpdateList((PVBPDI)pAddr);
        return 0;
      }
    }
      break;
    }
  }
    break;
  case WM_SIZE:
    if (wParam != SIZE_MINIMIZED)
    {
      WORD Width = LOWORD(lParam);
      WORD Height = HIWORD(lParam);
      MoveWindow(This->hList, 1, 36, LOWORD(lParam) - 2, HIWORD(lParam) - 36, TRUE);
    }
    break;
  case WM_SIZING:
  {
    if (wParam >= WMSZ_LEFT && wParam <= WMSZ_BOTTOMRIGHT)
    {
      LPRECT lprc = (LPRECT)lParam;
      long Width = lprc->right - lprc->left;
      long Height = lprc->bottom - lprc->top;

      if (Width < 800) lprc->right = lprc->left + 800;
      if (Height < 400) lprc->bottom = lprc->top + 400;
    }
  }
    return 1;
  case WM_DESTROY:
    This->DelAllProc();
    DelClassTable(This->m_hWnd);
    This->m_hWnd = NULL;
    return 0;
  }

  return DefMDIChildProc(phWnd, Msg, wParam, lParam);
}