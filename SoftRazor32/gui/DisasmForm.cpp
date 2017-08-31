#include "../sr_pch.h"

MC_Disasm::~MC_Disasm()
{
  if (this->m_hWnd)
  {
    DestroyWindow(this->m_hWnd);
    this->m_hWnd = NULL;
  }

  delete hmm;
  delete smm;
}

PDAI MC_Disasm::GetItemByIndex(UINT Idx)
{
  if (Idx >= diCount) return NULL;
  return ItemArray[Idx];
}

UINT MC_Disasm::GetItemIndexByAddress(PVOID Address)
{
  UINT i;

  for (i = 0; i < this->diCount; i++)
  {
    if ((UINT)Address >= (UINT)ItemArray[i]->VirtualAddress &&
      (UINT)Address < (UINT)ItemArray[i]->VirtualAddress + (UINT)ItemArray[i]->InstSize)
      return i;
  }
  return (UINT)-1;
}

PDAI MC_Disasm::GetItemByAddress(PVOID Address)
{
  UINT i;
  UINT itemCount = diCount;
  if (!ItemArray) return NULL;
  if ((UINT)Address < (UINT)BaseAddr || (UINT)Address >= (UINT)BaseAddr + BlockSize) return NULL;

  for (i = 0; i < itemCount; i++)
  {
    if (ItemArray[i]->VirtualAddress == Address)
      return ItemArray[i];
    else if (ItemArray[i]->VirtualAddress == 0)
      break;
  }
  return NULL;
}

void MC_Disasm::DelItemArray()
{
  hmm->hreset(1, 524288);
  smm->hreset(1, 524288);
}


void MC_Disasm::SetScrollBar(UINT CurrPos, UINT MaxPos)
{
  if (MaxPos)
    SendMessage(hw_vsb, SBM_SETRANGE, 0, MaxPos - 1);
  SendMessage(hw_vsb, SBM_SETPOS, CurrPos, TRUE);
}

HWND MC_Disasm::CreateForm(int x, int y, int cx, int cy, PTCHAR wName, DWORD Style)
{
  if (this->m_hWnd)
  {
    this->SetTop();
    return this->m_hWnd;
  }

  MDICREATESTRUCT MCS;

  MCS.szClass = (PTCHAR)mf->rcls_mc_disasm;
  MCS.szTitle = wName;
  MCS.hOwner = mf->MdlBase;
  MCS.style = Style;
  MCS.cx = cx;
  MCS.cy = cy;
  MCS.x = x;
  MCS.y = y;
  MCS.lParam = (LPARAM)this;

  this->m_hWnd = (HWND)::SendMessage(hw_MDI, WM_MDICREATE, 0, (LPARAM)&MCS);
  if (m_hWnd && !fw_reg) fw_reg = new FW_RegWnd(m_hWnd);
  return this->m_hWnd;
}

int MC_Disasm::AssembleOfAddress(PVOID Addr, t_asmmodel * tasm)
{
  if (!Addr || !tasm || !tasm->length || tasm->length > 16) return -1;
  UINT oItem = GetItemIndexByAddress(Addr);
  if (oItem == (UINT)-1) return -2;

  PPDAI pItem = ItemArray;
  PDAI tmpItem;
  UINT ocl = 0;   //Original Code Length
  UINT oin = 1;   //Original Item Number
  UINT nin;
  UINT i;
  UINT j;

  PBYTE nCode;

  while (true)
  {
    if ((oItem + oin - 1) >= diCount)
      return -3;

    ocl += pItem[oItem + oin - 1]->InstSize;
    if (ocl >= (UINT)tasm->length) break;
    oin++;
  }

  nCode = (PBYTE)malloc(ocl);
  if (!nCode) return -4;
  memcpy_s(nCode, ocl, tasm->code, tasm->length);
  i = ocl - tasm->length;

  if (i)    //如果有多余长度
    memset(&nCode[tasm->length], 0x90, i);    //填充为nop

  mf->m_mer->TryModifyMemory((PBYTE)Addr, nCode, ocl);

  ud_t dt;
  /*初始化ud*/
  ud_init(&dt);
  ud_set_mode(&dt, 32);
  ud_set_syntax(&dt, UD_SYN_INTEL);
  ud_set_input_buffer(&dt, nCode, ocl);
  ud_set_pc(&dt, (uint64_t)Addr);

  mf->SetDbgState(DS_Busy);
  nin = ud_get_count(&dt);

  if (!nin)
  {
    free(nCode);
    mf->SetDbgState(DS_Idle);
    return -5;
  }

  ud_set_input_buffer(&dt, nCode, ocl);
  ud_set_pc(&dt, (uint64_t)Addr);

  blDraw = FALSE;

  for (i = 0; i < oin; i++)
  {
    if (pItem[oItem + i]->pInstText) smm->hfree(pItem[oItem + i]->pInstText);
    if (pItem[oItem + i]->pNote) smm->hfree(pItem[oItem + i]->pNote);
    if (pItem[oItem + i]) hmm->hfree(pItem[oItem + i]);
  }

  if (nin != oin)   //如果新数量不等于原条目数量 (重分配数组)
  {
    if (nin > oin)    //新大于原
    {
      //先重新分配数组,再移动指针
      ItemArray = (PPDAI)hmm->hrecalloc(ItemArray, diCount + (nin - oin), sizeof(PDAI));
      pItem = ItemArray;
      memmove(&pItem[oItem + nin], &pItem[oItem + oin], (diCount - (oItem + oin)) * sizeof(PDAI));
      diCount += nin - oin;
    }
    else              //原大于新
    {
      //先移动指针,再重新分配数组
      memmove(&pItem[oItem + nin], &pItem[oItem + oin], (diCount - (oItem + oin)) * sizeof(PDAI));
      ItemArray = (PPDAI)hmm->hrecalloc(ItemArray, diCount - (oin - nin), sizeof(PDAI));
      pItem = ItemArray;
      diCount -= oin - nin;
    }

    /*
    if (nin > oin)    //新大于原
    {
    //先重新分配数组,再移动指针
    ItemArray = (PPDAI)hmm->hrecalloc(ItemArray, diCount + (nin - oin), sizeof(PDAI));
    pItem = ItemArray;
    memmove(&pItem[oItem + nin], &pItem[oItem + oin], (diCount - (oItem + oin) + 1) * sizeof(PDAI));
    diCount += nin - oin;
    }
    else              //原大于新
    {
    //先移动指针,再重新分配数组
    memmove(&pItem[oItem + nin], &pItem[oItem + oin], (diCount - (oItem + oin) + 1) * sizeof(PDAI));
    ItemArray = (PPDAI)hmm->hrecalloc(ItemArray, diCount - (oin - nin), sizeof(PDAI));
    pItem = ItemArray;
    diCount -= oin - nin;
    }
    */
  }

  for (i = 0; i < nin; i++)
  {
    TCHAR stmp[10];

    /*处理消息*/
    CheckMessageQueue();
  ReDisasm:
    if (ud_disassemble(&dt) == 0)
    {
      if (dt.inp_end) break;
      dt.inp_buf_index++;
      goto ReDisasm;
    }

    tmpItem = (PDAI)hmm->hcalloc(sizeof(DISASM_ITEM));
    if (!tmpItem)
    {
      free(nCode);
      return -6;
    }

    tmpItem->VirtualAddress = (PVOID)dt.insn_offset;
    tmpItem->InstSize = dt.inp_ctr;

    _stprintf_s(stmp, 10, HEXFORMAT, tmpItem->VirtualAddress);
    memcpy(tmpItem->AddrText, stmp, 8 * sizeof(TCHAR));
    j = casmlen(dt.casm_buf, 128);
    tmpItem->pInstText = (PTCHAR)smm->hmalloc((j + 1) * sizeof(TCHAR));
    if (!tmpItem->pInstText)
    {
      free(nCode);
      return -7;
    }
    tmpItem->itCount = j + 1;
    casmcpy(tmpItem->pInstText, dt.casm_buf, j + 1);
    pItem[oItem + i] = tmpItem;
  }

  SelectedAddrA = Addr;
  SelectedAddrB = Addr;
  mf->SetDbgState(DS_Idle);
  blDraw = TRUE;
  RedrawForm();
  return 0;
}

BOOL MC_Disasm::DisasmOfAddress(PBYTE dsAddress, DWORD Flag)
{
  PBYTE cAddr;      //反汇编地址所在内存块基址
  PBYTE pData;      //原内存数据
  PWCHAR pText;
  PDAI pItem;
  ud_t dt;
  UINT i;
  UINT ui0;
  UINT ui1;
  MEMORY_BASIC_INFORMATION MBI;
  TCHAR stmp[10];

  /*设置为忙碌状态*/
  mf->SetDbgState(DS_Busy);

  /*验证地址的有效性*/
  if (!VirtualQuery(dsAddress, &MBI, sizeof(MEMORY_BASIC_INFORMATION))) goto ErrBack;
  if (MBI.State != MEM_COMMIT || MBI.RegionSize == 0) goto ErrBack;
  if (!GetSectionBaseAddr((int)dsAddress, (PDWORD)&cAddr, (PDWORD)&ui1)) goto ErrBack;

  /*初始化数据*/
  BaseAddr = cAddr;
  BlockSize = ui1;
  DrawAddr = NULL;
  SelectedAddrA = NULL;
  SelectedAddrB = NULL;

  /*申请内存,以存放原始内存块数据*/
  pData = (PBYTE)malloc(ui1);
  if (!pData) goto ErrBack;

  if (!mf->m_mer->GetOriginalMemory(cAddr, pData, ui1))
  {
    free(pData);
    goto ErrBack;
  }

  /*初始化ud*/
  ud_init(&dt);
  ud_set_mode(&dt, 32);
  ud_set_syntax(&dt, UD_SYN_INTEL);
  ud_set_input_buffer(&dt, pData, ui1);
  ud_set_pc(&dt, (uint64_t)cAddr);

  /*清空之前的反汇编条目*/
  hmm->hreset(1, 524288);
  smm->hreset(1, 524288);
  diCount = ud_get_count(&dt);          //取条目数量
  ItemArray = (PPDAI)hmm->hcalloc(diCount, sizeof(PDAI));

  ud_set_input_buffer(&dt, pData, ui1);
  ud_set_pc(&dt, (uint64_t)cAddr);      //设置内存块的基地址 (EIP)

  for (i = 0; i < diCount; i++)
  {
    /*处理消息*/
    CheckMessageQueue();
  ReDisasm:
    if (ud_disassemble(&dt) == 0)
    {
      if (dt.inp_end) break;
      dt.inp_buf_index++;
      goto ReDisasm;
    }

    pItem = (PDAI)hmm->hcalloc(sizeof(DISASM_ITEM));
    if (!pItem)
    {
      free(pData);
      goto ErrBack;
    }

    pItem->VirtualAddress = (PVOID)dt.insn_offset;
    pItem->InstSize = dt.inp_ctr;

    _stprintf_s(stmp, 10, HEXFORMAT, pItem->VirtualAddress);
    memcpy(pItem->AddrText, stmp, 8 * sizeof(TCHAR));
    ui0 = casmlen(dt.casm_buf, 128);
    pItem->pInstText = (PTCHAR)smm->hmalloc((ui0 + 1) * sizeof(TCHAR));
    if (!pItem->pInstText)
    {
      free(pData);
      goto ErrBack;
    }
    pItem->itCount = ui0 + 1;
    casmcpy(pItem->pInstText, dt.casm_buf, ui0 + 1);
    ItemArray[i] = pItem;
  }

  free(pData);
  DrawAddr = dsAddress;
  SelectedAddrA = dsAddress;
  SelectedAddrB = dsAddress;

  if (this->m_hWnd)
    this->SetTop();
  else
    this->CreateForm(CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, WN_Disasm, 0);

  RedrawForm();

  mf->SetDbgState(DS_Idle);
  return TRUE;

ErrBack:
  mf->SetDbgState(DS_Idle);
  return FALSE;
}

BOOL MC_Disasm::GotoAddress(PBYTE dsAddress, DWORD Flag)
{
  if (((UINT)dsAddress >= (UINT)BaseAddr) && ((UINT)dsAddress < (UINT)BaseAddr + BlockSize))
  {
    DrawAddr = dsAddress;
    SetTop();
    RedrawForm();
    return TRUE;
  }
  return DisasmOfAddress(dsAddress, Flag);
}

PVOID MC_Disasm::GetSelectLineAddress()
{
  return SelectedAddrA;
}

UINT MC_Disasm::GetAsmTextByAddress(PVOID Addr, PTCHAR pat, UINT scount)
{
  if (!Addr || !pat || !scount) return 0;
  pat[0] = 0;
  PDAI pItem = GetItemByAddress(Addr);
  if (!pItem) return 0;

  UINT i;
  UINT wlen;
  UINT cn = 0;

  for (i = 0; i < pItem->itCount && cn < scount; i++)
  {
    if (pItem->pInstText[i] == 0) break;
    i++;
    wlen = wcslen(&pItem->pInstText[i]);
    if (cn + wlen >= scount) break;
    wcscat_s(pat, scount, &pItem->pInstText[i]);
    i += wlen;
    cn += wlen;
  }
  return cn;
}

UINT MC_Disasm::GetOpLengthByAddress(PVOID Addr)
{
  if (!Addr) return 0;
  PDAI pItem = GetItemByAddress(Addr);
  if (!pItem) return 0;
  return pItem->InstSize;
}

void MC_Disasm::RedrawForm()
{
  RECT rect;

  if (GetClientRect(this->m_hWnd, &rect))
  {
    if (rect.bottom > 20) rect.bottom -= 20;
    if (rect.right > 20) rect.right -= 20;

    InvalidateRect(this->m_hWnd, &rect, TRUE);
    UpdateWindow(this->m_hWnd);
  }
}

PDAI MC_Disasm::HitTest(WORD px, WORD py)
{
  if (!drWidth || !drHeight || !DrawAddr) return NULL;
  if (px > drWidth || py > drHeight) return NULL;

  UINT Idx;
  UINT BaseIdx;

  Idx = py / 20;
  BaseIdx = GetItemIndexByAddress(DrawAddr);
  return GetItemByIndex(BaseIdx + Idx);
}

LRESULT CALLBACK MC_Disasm::disasmwndProc(HWND phWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  MC_Disasm *This = (MC_Disasm *)GetClassPointer(phWnd);

  switch (Msg)
  {
  case WM_CREATE:
  {
    LPMDICREATESTRUCT PMCS = *(LPMDICREATESTRUCT *)lParam;

    This = (MC_Disasm *)PMCS->lParam;
    AddClassTable(phWnd, This);

    This->hw_vsb = CreateWindowEx(0, WC_SCROLLBAR, NULL, DEF_VCHILD | SBS_VERT, 0, 0, 0, 0, phWnd, NULL, DllBase, 0);
    SendMessage(phWnd, WM_SETFONT, (WPARAM)MainForm::hf_med, TRUE);
  }
    return 0;
  case WM_PAINT:
    if (!This->blDraw) break;
    {
      PAINTSTRUCT ps;
      RECT rect;
      HDC hDC = BeginPaint(phWnd, &ps);
      UINT BaseIdx = This->GetItemIndexByAddress(This->DrawAddr);
      PPDAI ppItem = This->ItemArray;
      WORD i;
      WORD OverSize;

      if (!hDC) break;
      if (BaseIdx == -1 || !This->ItemArray) goto EP;

      SelectObject(hDC, MainForm::hf_fix);
      SelectObject(hDC, GetStockObject(WHITE_PEN));
      SelectObject(hDC, GetStockObject(WHITE_BRUSH));

      rect.top = 0;
      rect.bottom = This->drHeight + 2;
      rect.left = 0;
      rect.right = This->drWidth + 2;

      DrawEdge(hDC, &rect, EDGE_BUMP, BF_RECT);  //绘制反汇编区域边框

      for (i = 0; i < This->MaxItem; i++)    //绘制每个条目
      {
        if (BaseIdx + i >= This->diCount) break;
        OverSize = This->drHeight - i * 20;
        /*开始绘制条目背景*/
        rect.top = i * 20 + 3;
        rect.bottom = rect.top + ((OverSize / 20) ? 20 : (OverSize % 20 - 5));
        rect.left = 3;
        rect.right = This->drWidth - 60;

        if (This->SelectedAddrA && This->SelectedAddrB)
        {
          if ((UINT)ppItem[BaseIdx + i]->VirtualAddress >= (UINT)This->SelectedAddrA &&
            (UINT)ppItem[BaseIdx + i]->VirtualAddress <= (UINT)This->SelectedAddrB)
            FillRect(hDC, &rect, (HBRUSH)GetStockObject(GRAY_BRUSH));
          else
            FillRect(hDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
        }
        else
          FillRect(hDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

        /*开始绘制地址文本*/
        rect.top += 2;
        rect.left = 4;
        rect.right = 104;

        SetTextColor(hDC, RGB(0, 0, 0));
        SetBkMode(hDC, TRANSPARENT);//设置背景样式
        DrawText(hDC, ppItem[BaseIdx + i]->AddrText, 8, &rect, DT_LEFT | DT_SINGLELINE);
        /*开始绘制汇编指令*/
        rect.left = 104;
        rect.right = This->drWidth - 60;

        DrawCasmW(hDC, ppItem[BaseIdx + i]->pInstText,
          ppItem[BaseIdx + i]->itCount, 20, &rect, DT_LEFT | DT_SINGLELINE);
      }
      This->SetScrollBar(This->GetItemIndexByAddress(This->DrawAddr), This->diCount);
    EP:
      EndPaint(phWnd, &ps);
    }
    break;
  case WM_SIZE:
    if (wParam != SIZE_MINIMIZED)
    {
      WORD Width = LOWORD(lParam);
      WORD Height = HIWORD(lParam);

      //计算反汇编区域大小
      This->drWidth = Width - 22;
      This->drHeight = Height - 22;

      //计算反汇编条目数量
      This->MaxItem = This->drHeight / 20;
      if (This->drHeight % 20) This->MaxItem++;

      MoveWindow(This->hw_vsb, Width - 20, 0, 20, Height - 20, TRUE);
    }
    break;
  case WM_SIZING:
    if (wParam >= WMSZ_LEFT && wParam <= WMSZ_BOTTOMRIGHT)
    {
      LPRECT lprc = (LPRECT)lParam;
      long Width = lprc->right - lprc->left;
      long Height = lprc->bottom - lprc->top;

      if (Width < 600) lprc->right = lprc->left + 600;
      if (Height < 400) lprc->bottom = lprc->top + 400;
    }
    return 1;
  case WM_MOUSEWHEEL:
  {
    int16_t mwu = HIWORD(wParam);
    SCROLLINFO SI;

    if (mwu == 0) break;
    ZeroMemory(&SI, sizeof(SCROLLINFO));
    SI.cbSize = sizeof(SCROLLINFO);
    SI.fMask = SIF_RANGE | SIF_POS;
    GetScrollInfo((HWND)This->hw_vsb, SB_CTL, &SI);

    if (mwu < 0)
    {
      SI.nPos += 2;
      if (SI.nPos > SI.nMax) SI.nPos = SI.nMax;
    }

    if (mwu > 0)
    {
      SI.nPos -= 2;
      if (SI.nPos < SI.nMin) SI.nPos = SI.nMin;
    }

    This->DrawAddr = This->ItemArray[SI.nPos]->VirtualAddress;
    SetScrollPos((HWND)This->hw_vsb, SB_CTL, SI.nPos, TRUE);
    This->RedrawForm();
  }
    return 0;
  case WM_LBUTTONDOWN:
  {
    SetFocus(phWnd);

    PDAI HitItem = This->HitTest(LOWORD(lParam), HIWORD(lParam));

    if (!This->SelectedAddrA)
    {
      if (!HitItem)
        return 0;
      else
        This->SelectedAddrA = HitItem->VirtualAddress;
    }

    if (wParam & MK_SHIFT)    //Shift被按下
    {
      if (HitItem)            //有项目被击中
      {
        This->SelectedAddrA = VAMIN(This->SelectedAddrA, HitItem->VirtualAddress);
        This->SelectedAddrB = VAMAX(This->SelectedAddrA, HitItem->VirtualAddress);
      }
      else
        This->SelectedAddrB = This->SelectedAddrA;
    }
    else
    {
      if (HitItem)            //有项目被击中
      {
        This->SelectedAddrA = HitItem->VirtualAddress;
        This->SelectedAddrB = HitItem->VirtualAddress;
      }
    }
    This->RedrawForm();
  }
    return 0;
  case WM_LBUTTONDBLCLK:
  {
    PDAI HitItem = This->HitTest(LOWORD(lParam), HIWORD(lParam));
    if (HitItem)
    {
      This->SelectedAddrA = HitItem->VirtualAddress;
      This->SelectedAddrB = HitItem->VirtualAddress;
      DialogBox(DllBase, MAKEINTRESOURCE(ASM_DLG), phWnd, AsmProc);
    }
  }
    return 0;
  case WM_KEYDOWN:
  {
    if (GetKeyState(VK_CONTROL) < 0)
    {
      if (wParam == 'G')
      {
        DialogBox(DllBase, MAKEINTRESOURCE(GOTOADDR_DLG), phWnd, GotoAddrProc);
        return 0;
      }
      else if (wParam == 'R')
      {
        This->fw_reg->CreateForm();
      }
    }
  }
    break;
  case WM_VSCROLL:
  {
    if (lParam != (LPARAM)This->hw_vsb) break;

    int snPos;
    SCROLLINFO SI;

    ZeroMemory(&SI, sizeof(SCROLLINFO));
    SI.cbSize = sizeof(SCROLLINFO);
    SI.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
    GetScrollInfo((HWND)lParam, SB_CTL, &SI);
    if (SI.nMax == 0 || SI.nMax < SI.nMin) return 0;

    switch (LOWORD(wParam))
    {
    case SB_LINEUP:
      snPos = SI.nPos;
      snPos--;
      break;
    case SB_LINEDOWN:
      snPos = SI.nPos;
      snPos++;
      break;
    case SB_PAGEUP:
      snPos = SI.nPos;
      snPos -= This->MaxItem;
      break;
    case SB_PAGEDOWN:
      snPos = SI.nPos;
      snPos += This->MaxItem;
      break;
    case SB_THUMBTRACK:
      snPos = SI.nTrackPos;
      break;
    default:
      return 0;
    }
    if (snPos < SI.nMin) snPos = SI.nMin;
    if (snPos > SI.nMax) snPos = SI.nMax;

    This->DrawAddr = This->ItemArray[snPos]->VirtualAddress;
    SetScrollPos((HWND)lParam, SB_CTL, snPos, TRUE);
    This->RedrawForm();
  }
    return 0;
  case WM_DESTROY:
    mf->SetDbgState(DS_Busy);

    if (This->fw_reg)
    {
      This->fw_reg->DestroyForm();
      This->fw_reg = NULL;
    }

    This->DelItemArray();
    This->ItemArray = NULL;
    This->BaseAddr = NULL;
    This->BlockSize = NULL;
    This->DrawAddr = NULL;
    This->SelectedAddrA = NULL;
    This->drWidth = NULL;
    This->drHeight = NULL;
    This->MaxItem = NULL;

    DelClassTable(This->m_hWnd);
    This->m_hWnd = NULL;
    mf->SetDbgState(DS_Idle);
    return 0;
  }

  return DefMDIChildProc(phWnd, Msg, wParam, lParam);
}