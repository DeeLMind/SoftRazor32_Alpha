#include "../sr_pch.h"

MC_Module::~MC_Module()
{
  if (this->m_hWnd)
  {
    DestroyWindow(this->m_hWnd);
    this->m_hWnd = NULL;
  }
}

HWND MC_Module::CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style)
{
  if (this->m_hWnd)
  {
    this->SetTop();
    return this->m_hWnd;
  }

  MDICREATESTRUCT MCS;

  MCS.szClass = (PTCHAR)mf->rcls_mc_module;
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

HWND MC_Module::GetModuleListhWnd()
{
  return this->mdllv;
}

int CALLBACK MC_Module::mdl_CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam)
{
  PCFPARAM PCP = (PCFPARAM)lParam;
  MC_Module *This = (MC_Module *)GetClassPointer(PCP->hCtl);
  PMODULE_LV_TYPE pm1 = (PMODULE_LV_TYPE)lParam1;
  PMODULE_LV_TYPE pm2 = (PMODULE_LV_TYPE)lParam2;
  LV_ITEM lvi;
  TCHAR buffer1[256];
  TCHAR buffer2[256];

  lvi.mask = LVIF_TEXT;
  lvi.pszText = buffer1;
  lvi.cchTextMax = 256;

  lvi.iSubItem = PCP->SubItem;

  SendMessage(This->mdllv,LVM_GETITEMTEXT,pm1->LV.Item,(LPARAM)&lvi);
  lstrcpy(buffer2,buffer1);
  SendMessage(This->mdllv,LVM_GETITEMTEXT,pm2->LV.Item,(LPARAM)&lvi);

  if (This->mdlASCEND)
    return  lstrcmpi(buffer1,buffer2);
  else
    return  lstrcmpi(buffer2,buffer1);
}

void MC_Module::UpdatelParam(HWND hlv)
{
  LV_ITEM lvi;
  PMODULE_LV_TYPE pmlt;
  int nCount = ListView_GetItemCount(hlv);

  lvi.mask = LVIF_PARAM;
  lvi.iSubItem = 0;
  lvi.iItem = 0;

  while(nCount > 0)
  {
    ListView_GetItem(hlv,&lvi);
    pmlt = (PMODULE_LV_TYPE)lvi.lParam;
    if (pmlt)
      pmlt->LV.Item = lvi.iItem;
    lvi.iItem++;
    nCount--;
  }
}

LRESULT CALLBACK MC_Module::mdlwndProc(HWND phWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
  MC_Module *This = (MC_Module *)GetClassPointer(phWnd);

  switch (Msg)
  {
  case WM_CREATE:
    {
      LPMDICREATESTRUCT PMCS = *(LPMDICREATESTRUCT *)lParam;

      This = (MC_Module *)PMCS->lParam;
      AddClassTable(phWnd, (PVOID)This);

      This->mdllv = CreateWindowEx(WS_EX_CLIENTEDGE,WC_LISTVIEW,LN_Module,LV_STYLE,0,0,0,0,phWnd,NULL,DllBase,0);
      ListView_SetExtendedListViewStyle(This->mdllv,LVS_EX_FULLROWSELECT);

      This->sctlv = CreateWindowEx(WS_EX_CLIENTEDGE,WC_LISTVIEW,LN_Section,LV_STYLE,0,0,0,0,phWnd,NULL,DllBase,0);
      ListView_SetExtendedListViewStyle(This->sctlv,LVS_EX_FULLROWSELECT);

      This->mdlInsertColumn();
      This->sctInsertColumn();
      This->UpdateModuleList();
    }
    return 0;
  case WM_SIZE:
    if (wParam != SIZE_MINIMIZED)
    {
      MoveWindow(This->mdllv,0,0,LOWORD(lParam),HIWORD(lParam) - 130,TRUE);
      MoveWindow(This->sctlv,0,HIWORD(lParam) - 130,LOWORD(lParam),130,TRUE);
    }
    break;
  case WM_NOTIFY:
    {
      if (lParam == 0) break;

      LPNMHDR pNm = (LPNMHDR)lParam;

      switch(pNm->code)
      {
      case LVN_COLUMNCLICK:
        {
          LPNMLISTVIEW lpLV = (LPNMLISTVIEW)lParam;

          if (pNm->hwndFrom == This->mdllv)
          {
            CFPARAM CP;

            CP.hCtl = phWnd;
            CP.SubItem = lpLV->iSubItem;
            UpdatelParam(This->GetModuleListhWnd());
            This->mdlASCEND = !This->mdlASCEND;
            ListView_SortItems(This->mdllv,mdl_CompareFunc,&CP);
          }
        }
        break;
      case LVN_ITEMCHANGED:
        {
          if (pNm->hwndFrom == This->mdllv)
          {
            int Idx = ListView_GetNextItem(This->mdllv,-1,LVNI_FOCUSED);
            if (Idx != -1)
            {
              TCHAR mbbuff[10];
              HMODULE hMod = NULL;

              ListView_GetItemText(This->mdllv,Idx,0,mbbuff,10);
              if (_stscanf_s(mbbuff,HEXFORMAT,&hMod))
                This->UpdateSectionInfo(hMod);
            }
          }
        }
        break;
      case NM_CUSTOMDRAW:
        {
          LPNMLVCUSTOMDRAW lpnmcd = (LPNMLVCUSTOMDRAW)lParam;

          switch(lpnmcd->nmcd.dwDrawStage)
          {
          case CDDS_PREPAINT:
            if (pNm->hwndFrom == This->mdllv || pNm->hwndFrom == This->sctlv)
              return CDRF_NOTIFYITEMDRAW;
            break;
          case CDDS_ITEMPREPAINT:
            {
              if (pNm->hwndFrom == This->mdllv)
              {
                PLV_TYPE plv = (PLV_TYPE)lpnmcd->nmcd.lItemlParam;
                lpnmcd->clrText = plv->ForeColor;
                lpnmcd->clrTextBk = plv->BackColor;
              }
              else if (pNm->hwndFrom == This->sctlv)
              {
                TCHAR stbuff[10];
                DWORD sc;

                ListView_GetItemText(This->sctlv,lpnmcd->nmcd.dwItemSpec,5,stbuff,10);
                if (_stscanf_s(stbuff,HEXFORMAT,&sc))
                {
                  lpnmcd->clrText = scGetForeColor(sc);
                  lpnmcd->clrTextBk = scGetBackColor(sc);
                }
              }
              return CDRF_DODEFAULT;
            }
            break;
          }
        }
        break;
      case LVN_DELETEALLITEMS:
        return 0;
      case LVN_DELETEITEM:
        {
          LPNMLISTVIEW pnl = (LPNMLISTVIEW)lParam;
          delete (PMEMORY_LV_TYPE)pnl->lParam;
        }
        break;
      }
    }
    break;
  case WM_SIZING:
    {
      if (wParam >= WMSZ_LEFT && wParam <= WMSZ_BOTTOMRIGHT)
      {
        LPRECT lprc = (LPRECT)lParam;
        long Width = lprc->right - lprc->left;
        long Height = lprc->bottom - lprc->top;

        if (Width < 600) lprc->right = lprc->left + 600;
        if (Height < 400) lprc->bottom = lprc->top + 400;
      }
    }
    break;
  case WM_DESTROY:
    DelClassTable(This->m_hWnd);
    This->m_hWnd = NULL;
    This->mdllv = NULL;
    This->sctlv = NULL;
    return 0;
  }

  return DefMDIChildProc(phWnd,Msg,wParam,lParam);
}

void MC_Module::mdlInsertColumn()
{
  LV_COLUMN lvc;
  ZeroMemory(&lvc,sizeof(LV_COLUMN));
  lvc.mask = LVCF_TEXT | LVCF_WIDTH;

  lvc.pszText = _T("ģ���ַ");
  lvc.cx = DefWidth;
  ListView_InsertColumn(mdllv,0,&lvc);

  lvc.pszText = _T("ģ���С");
  lvc.cx = DefWidth;
  ListView_InsertColumn(mdllv,1,&lvc);

  lvc.pszText = _T("ģ�����");
  lvc.cx = DefWidth;
  ListView_InsertColumn(mdllv,2,&lvc);

  lvc.pszText = _T("ģ��·��");
  lvc.cx = 350;
  ListView_InsertColumn(mdllv,3,&lvc);
} 

void MC_Module::sctInsertColumn()
{
  LV_COLUMN lvc;
  ZeroMemory(&lvc,sizeof(LV_COLUMN));
  lvc.mask = LVCF_TEXT | LVCF_WIDTH;

  lvc.pszText = _T("��������");
  lvc.cx = 70;
  ListView_InsertColumn(sctlv,0,&lvc);

  lvc.pszText = _T("�ڴ��ַ");
  lvc.cx = DefWidth;
  ListView_InsertColumn(sctlv,1,&lvc);

  lvc.pszText = _T("�ڴ��С");
  lvc.cx = DefWidth;
  ListView_InsertColumn(sctlv,2,&lvc);

  lvc.pszText = _T("�ļ�ƫ��");
  lvc.cx = DefWidth;
  ListView_InsertColumn(sctlv,3,&lvc);

  lvc.pszText = _T("�ļ���С");
  lvc.cx = DefWidth;
  ListView_InsertColumn(sctlv,4,&lvc);

  lvc.pszText = _T("��������");
  lvc.cx = DefWidth;
  ListView_InsertColumn(sctlv,5,&lvc);

  lvc.pszText = _T("����ժҪ");
  lvc.cx = 190;
  ListView_InsertColumn(sctlv,6,&lvc);
} 

void MC_Module::UpdateModuleList()
{
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
  DWORD ModEntry;
  DWORD i = 0;
  PMODULE_LV_TYPE pmlv;
  TCHAR stmp[10];
  LV_ITEM lvi;
  MODULEENTRY32 me;

  ZeroMemory(&lvi,sizeof(LV_ITEM));
  me.dwSize = sizeof(MODULEENTRY32);

  ListView_DeleteAllItems(mdllv);
  ListView_DeleteAllItems(sctlv);

  if (hSnapshot == INVALID_HANDLE_VALUE) return;

  if (Module32First(hSnapshot,&me))
  {
    do
    {
      lvi.iItem = i;
      lvi.mask = LVIF_TEXT | LVIF_PARAM;
      lvi.cchTextMax = 10;
      lvi.pszText = stmp;
      pmlv = new MODULE_LV_TYPE;
      if(pmlv)
      {
        pmlv->LV.Item = i;

        if (me.hModule == DllBase)
          pmlv->LV.ForeColor = RGB(0,250,0);
        else if (me.hModule == GetModuleHandle(NULL))
          pmlv->LV.ForeColor = RGB(0,0,255);
        else
          pmlv->LV.ForeColor = RGB(0,0,0);
        pmlv->LV.BackColor = RGB(255,255,255); 
        pmlv->ModuleAddress = me.hModule;
      }
      lvi.lParam = (LPARAM)pmlv;

      lvi.iSubItem = 0;
      _stprintf_s(stmp,10,HEXFORMAT,me.hModule);
      ListView_InsertItem(mdllv,&lvi);

      lvi.mask = LVIF_TEXT;
      lvi.iSubItem = 1;
      _stprintf_s(stmp,10,HEXFORMAT,me.modBaseSize);
      ListView_SetItem(mdllv,&lvi);

      lvi.iSubItem = 2;
      ModEntry = GetModuleEntry(me.hModule);
      _stprintf_s(stmp,10,HEXFORMAT,ModEntry);
      ListView_SetItem(mdllv,&lvi);

      lvi.iSubItem = 3;
      lvi.cchTextMax = _MAX_PATH;
      lvi.pszText = me.szExePath;
      ListView_SetItem(mdllv,&lvi);

      i++;
    } while (Module32Next(hSnapshot,&me));
  }
  CloseHandle(hSnapshot);
}

void MC_Module::UpdateSectionInfo(HMODULE hMod)
{
  ListView_DeleteAllItems(sctlv);

  if (!hMod) return;

  WORD sNum;
  WORD i;
  LV_ITEM lvi;
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)hMod;
  PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)((DWORD)hMod + pDos->e_lfanew);
  PIMAGE_SECTION_HEADER pSection;
  TCHAR sbuff[50];

  sNum = pNt->FileHeader.NumberOfSections;
  if (!sNum) return;

  ZeroMemory(&lvi,sizeof(LV_ITEM));

  lvi.mask = LVIF_TEXT;
  lvi.cchTextMax = 50;

  for (i = 0;i < sNum;i++)
  {
    lvi.iItem = i;
    lvi.pszText = sbuff;
    pSection = (PIMAGE_SECTION_HEADER)((DWORD)pNt + 24 + pNt->FileHeader.SizeOfOptionalHeader + i * 40);

    memset(sbuff,0,10);

    lvi.iSubItem = 0;
    memcpy_s(sbuff,8,&pSection->Name[0],8);
    ListView_InsertItemA(sctlv,&lvi);

    lvi.iSubItem = 1;
    _stprintf_s(sbuff,10,HEXFORMAT,(DWORD)hMod + pSection->VirtualAddress);
    ListView_SetItem(sctlv,&lvi);

    lvi.iSubItem = 2;
    _stprintf_s(sbuff,10,HEXFORMAT,pSection->Misc.VirtualSize);
    ListView_SetItem(sctlv,&lvi);

    lvi.iSubItem = 3;
    _stprintf_s(sbuff,10,HEXFORMAT,pSection->PointerToRawData);
    ListView_SetItem(sctlv,&lvi);

    lvi.iSubItem = 4;
    _stprintf_s(sbuff,10,HEXFORMAT,pSection->SizeOfRawData);
    ListView_SetItem(sctlv,&lvi);

    lvi.iSubItem = 5;
    _stprintf_s(sbuff,10,HEXFORMAT,pSection->Characteristics);
    ListView_SetItem(sctlv,&lvi);

    if ((pSection->Characteristics & IMAGE_SCN_MEM_WRITE) == IMAGE_SCN_MEM_WRITE)
      _tcscpy_s(sbuff,50,sc_rw);
    else
      _tcscpy_s(sbuff,50,sc_or);

    if ((pSection->Characteristics & IMAGE_SCN_CNT_CODE) == IMAGE_SCN_CNT_CODE || (pSection->Characteristics & IMAGE_SCN_MEM_EXECUTE) == IMAGE_SCN_MEM_EXECUTE)
    {
      _tcscat_s(sbuff,50,t_fgf);
      _tcscat_s(sbuff,50,sc_ex);
    }

    if ((pSection->Characteristics & IMAGE_SCN_MEM_SHARED) == IMAGE_SCN_MEM_SHARED)
    {
      _tcscat_s(sbuff,50,t_fgf);
      _tcscat_s(sbuff,50,sc_sh);
    }

    if ((pSection->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) == IMAGE_SCN_CNT_INITIALIZED_DATA)
    {
      _tcscat_s(sbuff,50,t_fgf);
      _tcscat_s(sbuff,50,sc_id);
    }
    else if ((pSection->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) == IMAGE_SCN_CNT_UNINITIALIZED_DATA)
    {
      _tcscat_s(sbuff,50,t_fgf);
      _tcscat_s(sbuff,50,sc_ud);
    }

    if ((pSection->Characteristics & IMAGE_SCN_LNK_COMDAT) == IMAGE_SCN_LNK_COMDAT)
    {
      _tcscat_s(sbuff,50,t_fgf);
      _tcscat_s(sbuff,50,sc_cd);
    }

    if ((pSection->Characteristics & IMAGE_SCN_LNK_NRELOC_OVFL) == IMAGE_SCN_LNK_NRELOC_OVFL)
    {
      _tcscat_s(sbuff,50,t_fgf);
      _tcscat_s(sbuff,50,sc_rt);
    }

    if ((pSection->Characteristics & IMAGE_SCN_MEM_NOT_PAGED) == IMAGE_SCN_MEM_NOT_PAGED)
    {
      _tcscat_s(sbuff,50,t_fgf);
      _tcscat_s(sbuff,50,sc_np);
    }

    if ((pSection->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) == IMAGE_SCN_MEM_DISCARDABLE)
    {
      _tcscat_s(sbuff,50,t_fgf);
      _tcscat_s(sbuff,50,sc_dc);
    }

    lvi.iSubItem = 6;
    ListView_SetItem(sctlv,&lvi);
  }
}

