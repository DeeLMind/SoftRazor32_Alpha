#include "../sr_pch.h"

MC_VBInfo::~MC_VBInfo()
{
  if (this->m_hWnd)
  {
    DestroyWindow(this->m_hWnd);
    this->m_hWnd = NULL;
  }

  if (this->PM_4)
  {
    DestroyMenu(this->PM_4);
    PM_4 = NULL;
  }
}

HWND MC_VBInfo::CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style)
{
  if (this->m_hWnd)
  {
    this->SetTop();
    return this->m_hWnd;
  }

  MDICREATESTRUCT MCS;

  MCS.szClass = (PTCHAR)mf->rcls_mc_vbinfo;
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

void MC_VBInfo::ParseHeader(PVBHD pVbHeader)
{
  WORD frmCount = 0;
  WORD basCount = 0;
  WORD clsCount = 0;
  WORD ctlCount = 0;
  PVBO pVbObject;
  PVB_TV_TYPE pTv;
  WORD i;
  WORD ObjCount;
  DWORD j;
  WORD k;
  DWORD BakVal;
  HTREEITEM LastForm;
  HTREEITEM LastItem;
  TV_INSERTSTRUCT tvis;
  static TCHAR stmp0[64];
  static TCHAR stmp1[64];
  static TCHAR entmp[64];

  __try
  {
    if (pVbHeader->Sign.dwSign != VB_MAGIC)
    {
      mf->SetStateText(_T("VB结构标志不正确!"),RGB(255,0,0));
      return;
    }

    ResetControl();
    ZeroMemory(&tvis, sizeof(TV_INSERTSTRUCT));
    vbhead = pVbHeader;
    _stprintf_s(stmp0, 64, HEXFORMAT, vbhead);
    SendMessage(vbsp, WM_SETTEXT, 0, (LPARAM)stmp0);
    pVbObject = pVbHeader->pProjInfo->pObjectTable->pObject;
    ObjCount = pVbHeader->pProjInfo->pObjectTable->ObjectCount1;

    tvis.hParent = 0;
    tvis.hInsertAfter = TVI_ROOT;
    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
    tvis.item.iImage = MainForm::bpi_root;
    tvis.item.iSelectedImage = MainForm::bpi_root;

    ALLOC_PARAM(tvis, pTv, VBPC_NULLITEM, 0);
    tvis.item.pszText = _T("VB结构");
    hti_struct = TreeView_InsertItem(vbtv, &tvis);

    tvis.hParent = hti_struct;
    tvis.item.iImage = MainForm::bpi_vbs;
    tvis.item.iSelectedImage = MainForm::bpi_vbs;

    ALLOC_PARAM(tvis, pTv, VBPC_STRUCT, VBII_VBH);
    tvis.item.pszText = _T("VB 头部");                    //添加VB基础结构节点
    pTv->TV.hItem = TreeView_InsertItem(vbtv, &tvis);

    ALLOC_PARAM(tvis, pTv, VBPC_STRUCT, VBII_PRJI);
    tvis.item.pszText = _T("VB 工程信息");
    pTv->TV.hItem = TreeView_InsertItem(vbtv, &tvis);

    ALLOC_PARAM(tvis, pTv, VBPC_STRUCT, VBII_COMD);
    tvis.item.pszText = _T("VB COM注册数据");
    pTv->TV.hItem = TreeView_InsertItem(vbtv, &tvis);

    ALLOC_PARAM(tvis, pTv, VBPC_STRUCT, VBII_COMI);
    tvis.item.pszText = _T("VB COM注册信息");
    pTv->TV.hItem = TreeView_InsertItem(vbtv, &tvis);

    ALLOC_PARAM(tvis, pTv, VBPC_STRUCT, VBII_OBJTAB);
    tvis.item.pszText = _T("VB 对象描述表");
    pTv->TV.hItem = TreeView_InsertItem(vbtv, &tvis);

    ALLOC_PARAM(tvis, pTv, VBPC_STRUCT, VBII_OBJS);
    _stprintf_s(stmp0, 64, _T("VB 对象  (%hu)"), ObjCount);
    tvis.item.pszText = stmp0;
    hti_object = TreeView_InsertItem(vbtv, &tvis);
    pTv->TV.hItem = hti_object;

    tvis.hParent = hti_object;
    tvis.item.iImage = MainForm::bpi_object;
    tvis.item.iSelectedImage = MainForm::bpi_object;

    mf->SetDbgState(DS_Busy);

    for (i = 0; i < ObjCount; i++)            //第一次遍历Object，计数
    {
      ALLOC_PARAM(tvis, pTv, VBPC_OBJECT, i);
      tvis.item.pszText = (PTCHAR)pVbObject[i].aObjectName;
      pTv->TV.hItem = TreeView_InsertItemA(vbtv, &tvis);

      if ((pVbObject[i].ObjectType & 0x80) == 0x80)
      {
        frmCount++;
        continue;
      }

      if ((pVbObject[i].ObjectType & 0x02) == 0)
      {
        basCount++;
        continue;
      }

      if ((pVbObject[i].ObjectType & 0x001DA003) == 0x001DA003)
      {
        ctlCount++;
        continue;
      }

      if ((pVbObject[i].ObjectType & 0x00018003) == 0x00018003)
      {
        clsCount++;
        continue;
      }
    }

    tvis.hParent = 0;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.iImage = MainForm::bpi_root;
    tvis.item.iSelectedImage = MainForm::bpi_root;
    tvis.item.pszText = stmp0;

    ALLOC_PARAM(tvis, pTv, VBPC_NULLITEM, 1);
    _stprintf_s(stmp0, 64, _T("窗体  [Form]  (%hu)"), frmCount);
    hti_frm = TreeView_InsertItem(vbtv, &tvis);
    pTv->TV.hItem = hti_frm;

    ALLOC_PARAM(tvis, pTv, VBPC_NULLITEM, 2);
    _stprintf_s(stmp0, 64, _T("模块  [Module]  (%hu)"), basCount);
    hti_bas = TreeView_InsertItem(vbtv, &tvis);
    pTv->TV.hItem = hti_bas;

    ALLOC_PARAM(tvis, pTv, VBPC_NULLITEM, 3);
    _stprintf_s(stmp0, 64, _T("类模块  [Class]  (%hu)"), clsCount);
    hti_cls = TreeView_InsertItem(vbtv, &tvis);
    pTv->TV.hItem = hti_cls;

    ALLOC_PARAM(tvis, pTv, VBPC_NULLITEM, 4);
    _stprintf_s(stmp0, 64, _T("用户控件  [UserControl]  (%hu)"), ctlCount);
    hti_ctl = TreeView_InsertItem(vbtv, &tvis);
    pTv->TV.hItem = hti_ctl;

    for (i = 0; i < ObjCount; i++)        //第二次遍历Object
    {
      PVBOOI pooi;    //VB对象可选信息头
      PVBC pctl;
      PPVBEP ppep;
      PVBOBJL pVbObjL;

      if ((pVbObject[i].ObjectType & 0x80) == 0x80)   //判断窗口
      {
        tvis.hParent = hti_frm;
        tvis.item.iImage = MainForm::bpi_frm;
        tvis.item.iSelectedImage = MainForm::bpi_frm;
        tvis.item.pszText = stmp0;
        ALLOC_PARAM(tvis, pTv, VBPC_ROOTFORM, i);

        if (!(pVbObjL = mf->GetObjectListByObject(&pVbObject[i])))
        {
          VBOBJL VbObjL;
          PTCHAR pstmp;
          size_t slen = strlen(pVbObject[i].aObjectName);

          memset(&VbObjL, 0, sizeof(VbObjL));

          if (!slen)
          {
            pstmp = (PTCHAR)mf->m_hmm->hcalloc(16 , sizeof(TCHAR));
            _stprintf_s(pstmp, 16, _T("_frm_%hu"), pVbObject[i].pObjectInfo->ObjectIndex);
          }
          else
          {
            pstmp = (PTCHAR)mf->m_hmm->hcalloc(slen + 1);
            mbstowcs_s((size_t *)&BakVal, pstmp, slen + 1, pVbObject[i].aObjectName, _TRUNCATE);
          }

          VbObjL.VbHead = pVbHeader;
          VbObjL.VbObj = &pVbObject[i];
          VbObjL.iIndex = pVbObject[i].pObjectInfo->ObjectIndex;
          VbObjL.ObjName = pstmp;
          
          pVbObjL = mf->AddObjectList(&VbObjL);
        }
        tvis.item.pszText = pVbObjL->ObjName;
        LastForm = TreeView_InsertItem(vbtv, &tvis);
        pTv->TV.hItem = LastForm;

        pooi = (PVBOOI)((DWORD)pVbObject[i].pObjectInfo + sizeof(VBOI));
        pctl = pooi->pControlArray;

        for (j = 0; j < pooi->ControlCount; j++)          //遍历控件
        {
          tvis.hParent = LastForm;
          tvis.item.iImage = MainForm::bpi_ukctl;
          tvis.item.iSelectedImage = MainForm::bpi_ukctl;
          tvis.item.pszText = stmp0;
          ALLOC_PARAM(tvis, pTv, VBPC_FORM, j);
          if (strlen(pctl[j].pName) == 0)
            _stprintf_s(stmp0, 64, _T("_ctl%u"), j + 1);
          else
            mbstowcs_s((size_t *)&BakVal, stmp0, (size_t)64, pctl[j].pName, _TRUNCATE);
          LastItem = TreeView_InsertItem(vbtv, &tvis);
          pTv->TV.hItem = LastItem;       //保存当前控件
          _tcscpy_s(stmp1, 64, stmp0);      //备份控件名
          tvis.hParent = LastItem;

          ppep = (PPVBEP)((DWORD)pctl[j].pEventTable + sizeof(VB_EVENT_TABLE));
          for (k = 0; k < pctl[j].EventCount; k++)          //遍历控件事件
          {
            if (ppep[k] == NULL || ppep[k]->pDescInfo == NULL) continue;
            PVBPL pVbProcL;
            if (!(pVbProcL = mf->GetProcListByAddress(pVbObjL->ObjName, ppep[k]->pDescInfo)))
            {
              VBPL VbProcL;
              PTCHAR pstmp = 0;
              PVOID TT;
              DWORD LE;

              memset(&VbProcL,0,sizeof(VBPL));
              pstmp = (PTCHAR)mf->m_hmm->hcalloc(64 , sizeof(TCHAR));

              if (!GetMemberNameByIndex(*pctl[j].pGUID, (DWORD)k, entmp, 64, 0))
                _stprintf_s(entmp, 64, _T("Event_%hu"), k);
              _stprintf_s(pstmp, 64, _T("%s_%s"), stmp1, entmp);
              VbProcL.pVbCtl = &pctl[j];
              VbProcL.iIndex = k;
              VbProcL.ProcDesc = ppep[k]->pDescInfo;
              VbProcL.EvtName = pstmp;
              pVbProcL = mf->AddProcListByObject(pVbObjL, &VbProcL);
            }

            tvis.item.iImage = MainForm::bpi_event;
            tvis.item.iSelectedImage = MainForm::bpi_event;
            ALLOC_PARAM(tvis, pTv, VBPC_FRMCTL, k);
            tvis.item.pszText = pVbProcL->EvtName;
            pTv->TV.hItem = TreeView_InsertItem(vbtv, &tvis);
            tvis.item.pszText = stmp0;
          }
        }

      }
      else if ((pVbObject[i].ObjectType & 0x02) == 0)      //判断模块
      {
        tvis.hParent = hti_bas;
        tvis.item.iImage = MainForm::bpi_bas;
        tvis.item.iSelectedImage = MainForm::bpi_bas;
        ALLOC_PARAM(tvis, pTv, VBPC_BAS, i);

        if (strlen(pVbObject[i].aObjectName) == 0)
        {
          _stprintf_s(stmp0, 64, _T("_bas%hu"), i + 1);
          tvis.item.pszText = stmp0;
          pTv->TV.hItem = TreeView_InsertItem(vbtv, &tvis);
        }
        else
        {
          tvis.item.pszText = (PTCHAR)pVbObject[i].aObjectName;
          pTv->TV.hItem = TreeView_InsertItemA(vbtv, &tvis);
        }
      }
      else if ((pVbObject[i].ObjectType & 0x001DA003) == 0x001DA003)       //判断用户控件
      {
        tvis.hParent = hti_ctl;
        tvis.item.iImage = MainForm::bpi_ctl;
        tvis.item.iSelectedImage = MainForm::bpi_ctl;
        ALLOC_PARAM(tvis, pTv, VBPC_UC, i);

        if (strlen(pVbObject[i].aObjectName) == 0)
        {
          _stprintf_s(stmp0, 64, _T("_ctl%hu"), i + 1);
          tvis.item.pszText = stmp0;
          pTv->TV.hItem = TreeView_InsertItem(vbtv, &tvis);
        }
        else
        {
          tvis.item.pszText = (PTCHAR)pVbObject[i].aObjectName;
          pTv->TV.hItem = TreeView_InsertItemA(vbtv, &tvis);
        }
        continue;
      }
      else if ((pVbObject[i].ObjectType & 0x00018003) == 0x00018003)       //判断类模块
      {
        tvis.hParent = hti_cls;
        tvis.item.iImage = MainForm::bpi_cls;
        tvis.item.iSelectedImage = MainForm::bpi_cls;
        ALLOC_PARAM(tvis, pTv, VBPC_CLS, i);

        if (strlen(pVbObject[i].aObjectName) == 0)
        {
          _stprintf_s(stmp0, 64, _T("_cls%hu"), i + 1);
          tvis.item.pszText = stmp0;
          pTv->TV.hItem = TreeView_InsertItem(vbtv, &tvis);
        }
        else
        {
          tvis.item.pszText = (PTCHAR)pVbObject[i].aObjectName;
          pTv->TV.hItem = TreeView_InsertItemA(vbtv, &tvis);
        }

      }
      else
        continue;
    }
    mf->SetDbgState(DS_Idle);

  }

  __except (TestExpect(GetExceptionCode()))
  {
    ResetControl();
    mf->SetStateText(_T("在解析VB结构时访问内存错误."),RGB(255,0,0));
    return;
  }
}

void MC_VBInfo::ParseModule(PVOID ModuleBase)
{
  if (!this->m_hWnd) return;

  __try
  {
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)ModuleBase;
    if (pDos->e_magic != IMAGE_DOS_SIGNATURE) return;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((DWORD)ModuleBase + pDos->e_lfanew);
    PBYTE OEP = (PBYTE)((DWORD)ModuleBase + pNt->OptionalHeader.AddressOfEntryPoint);
    PVBHD VBH = (PVBHD)*(PDWORD)((DWORD)OEP + 1);

    if (OEP[0] != 0x68 || OEP[5] != 0xE8)
    {
      mf->SetStateText(_T("未定位到关于VB结构的代码!"), RGB(255, 0, 0));
      return;
    }
    if (VBH->Sign.dwSign != VB_MAGIC)
    {
      mf->SetStateText(_T("VB结构标志不正确!"), RGB(255, 0, 0));
      return;
    }
    ParseHeader(VBH);
  }
  __except (TestExpect(GetExceptionCode()))
  {
    mf->SetStateText(_T("在解析VB头部结构时读取内存错误."),RGB(255, 0, 0));
    return;
  }
}

LRESULT CALLBACK MC_VBInfo::vbwndProc(HWND phWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  MC_VBInfo *This = (MC_VBInfo *)GetClassPointer(phWnd);

  switch (Msg)
  {
  case WM_CREATE:
  {
    LPMDICREATESTRUCT PMCS = *(LPMDICREATESTRUCT *)lParam;

    This = (MC_VBInfo *)PMCS->lParam;
    AddClassTable(phWnd, (PVOID)This);

    This->vbsp = CreateWindowEx(WS_EX_CLIENTEDGE, CN_Edit, T_NULL, DEF_VCHILD | ES_CENTER, 80, 1, 80, 20, phWnd, 0, DllBase, 0);
    This->vbokbtn = CreateWindowEx(0, CN_Button, _T("分析"), DEF_VCHILD, 170, 1, 50, 20, phWnd, 0, DllBase, 0);
    This->vbshbtn = CreateWindowEx(0, CN_Button, _T("HOOK结构指针"), DEF_VCHILD, 230, 1, 100, 20, phWnd, 0, DllBase, 0);
    This->vbtv = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, TN_VB, TV_STYLE, 0, 0, 0, 0, phWnd, NULL, DllBase, 0);
    This->vblv = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, LN_VB, LV_STYLE, 0, 0, 0, 0, phWnd, NULL, DllBase, 0);
    ListView_SetExtendedListViewStyle(This->vblv, LVS_EX_FULLROWSELECT);
    This->PM_4 = GetSubMenu(MainForm::hm_pop, 4);

    if (This->vbsp)
    {
      SendMessage(This->vbsp, WM_SETFONT, (WPARAM)MainForm::hf_med, 1);
      SendMessage(This->vbsp, EM_SETLIMITTEXT, 8, 0);
    }

    if (This->vbokbtn)
    {
      SetWindowLong(This->vbokbtn, GWL_ID, PDIDC_VBOKBTN);
      SendMessage(This->vbokbtn, WM_SETFONT, (WPARAM)MainForm::hf_sml, 1);
    }

    if (This->vbshbtn)
    {
      SetWindowLong(This->vbshbtn, GWL_ID, PDIDC_VBSHBTN);
      SendMessage(This->vbshbtn, WM_SETFONT, (WPARAM)MainForm::hf_sml, 1);
    }

    if (This->vbtv)
    {
      SendMessage(This->vbtv, WM_SETFONT, (WPARAM)MainForm::hf_med, 1);
      SendMessage(This->vbtv, TVM_SETBKCOLOR, 0, RGB(225, 250, 250));
      SendMessage(This->vbtv, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)MainForm::hImageList);
    }
  }
    return 0;
  case WM_DESTROY:
    DelClassTable(This->m_hWnd);
    This->m_hWnd = NULL;
    This->vbsp = NULL;
    This->vbokbtn = NULL;
    This->vbtv = NULL;
    This->vblv = NULL;
    return 0;
  case WM_NOTIFY:
  {
    if (lParam == 0) break;

    LPNMHDR pNm = (LPNMHDR)lParam;

    switch (pNm->code)
    {
    case TVN_DELETEITEMA:
    {
      LPNMTREEVIEWA pnmtv = (LPNMTREEVIEWA)lParam;
      if (pnmtv->itemOld.lParam)
      {
        PVB_TV_TYPE pTv = (PVB_TV_TYPE)pnmtv->itemOld.lParam;
        if (pTv)
          if (pTv->TV.Sign == MY_MAGIC)
            free((PVOID)pnmtv->itemOld.lParam);
      }
    }
      break;
    case TVN_DELETEITEMW:
    {
      LPNMTREEVIEWW pnmtv = (LPNMTREEVIEWW)lParam;
      if (pnmtv->itemOld.lParam)
      {
        PVB_TV_TYPE pTv = (PVB_TV_TYPE)pnmtv->itemOld.lParam;
        if (pTv)
          if (pTv->TV.Sign == MY_MAGIC)
            free((PVOID)pnmtv->itemOld.lParam);
      }
    }
      break;
    case NM_RCLICK:
    {
      RECT rect;

      if (pNm->hwndFrom == This->vbtv)
      {
        PVB_TV_TYPE pTv;
        TVHITTESTINFO tvhti;
        TVITEM tvi;

        GetCursorPos(&tvhti.pt);
        GetWindowRect(This->vbtv, &rect);
        tvhti.pt.x -= rect.left;
        tvhti.pt.y -= rect.top;
        TreeView_HitTest(This->vbtv, &tvhti);
        if ((tvhti.flags & TVHT_ONITEM) == 0) return 1;
        TreeView_SelectItem(This->vbtv, tvhti.hItem);
        tvi.mask = TVIF_PARAM;
        tvi.hItem = tvhti.hItem;
        TreeView_GetItem(This->vbtv, &tvi);
        pTv = (PVB_TV_TYPE)tvi.lParam;
        if (!pTv || pTv->TV.Sign != MY_MAGIC) return 1;
        tvhti.pt.x += rect.left;
        tvhti.pt.y += rect.top;
        if (pTv->TV.NCode == VBPC_FRMCTL)
          TrackPopupMenu(This->PM_4, TPM_LEFTALIGN, tvhti.pt.x, tvhti.pt.y, 0, phWnd, NULL);
      }
    }
      return 1;
    }
  }
    break;
  case WM_SIZE:
    if (wParam != SIZE_MINIMIZED)
    {
      WORD Width = LOWORD(lParam);
      WORD Height = HIWORD(lParam);

      MoveWindow(This->vbtv, 0, 22, 280, Height - 22, TRUE);
      MoveWindow(This->vblv, 281, 22, Width - 281, Height - 22, TRUE);
    }
    break;
  case WM_PAINT:
  {
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(phWnd, &ps);

    if (!hDC) break;
    SelectObject(hDC, MainForm::hf_med);
    SetTextColor(hDC, RGB(0, 0, 255));
    TextOut(hDC, 4, 3, _T("VB结构指针:"), 7);
    EndPaint(phWnd, &ps);
  }
    return 0;
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
  }

  return DefMDIChildProc(phWnd, Msg, wParam, lParam);
}

void MC_VBInfo::ResetControl()
{
  BOOL result;

  TreeView_DeleteAllItems(vbtv);
  ListView_DeleteAllItems(vblv);

  result = ListView_DeleteColumn(vblv, 0);
  while (result)
  {
    result = ListView_DeleteColumn(vblv, 0);
  }
}

void MC_VBInfo::vbInsertColumn(DWORD iType)
{
  BOOL result;
  LV_COLUMN lvc;
  ZeroMemory(&lvc, sizeof(LV_COLUMN));
  lvc.mask = LVCF_TEXT | LVCF_WIDTH;

  ListView_DeleteAllItems(vblv);

  result = ListView_DeleteColumn(vblv, 0);
  while (result)
  {
    result = ListView_DeleteColumn(vblv, 0);
  }

  switch (iType)
  {
  case 1:
    lvc.pszText = _T("成员名称");
    lvc.cx = DefWidth;
    ListView_InsertColumn(vblv, 0, &lvc);

    lvc.pszText = _T("成员数值");
    lvc.cx = 128;
    ListView_InsertColumn(vblv, 1, &lvc);

    lvc.pszText = _T("成员描述");
    lvc.cx = 256;
    ListView_InsertColumn(vblv, 2, &lvc);
    break;
  default:
    lvc.pszText = _T("成员名称");
    lvc.cx = DefWidth;
    ListView_InsertColumn(vblv, 0, &lvc);

    lvc.pszText = _T("成员数值");
    lvc.cx = 128;
    ListView_InsertColumn(vblv, 1, &lvc);

    lvc.pszText = _T("成员描述");
    lvc.cx = 256;
    ListView_InsertColumn(vblv, 2, &lvc);
    break;
  }
}

void MC_VBInfo::RefreshStructList()
{
  //LV_ITEM lvi;

}