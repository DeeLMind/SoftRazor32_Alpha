#include "../sr_pch.h"

static WORD iCount = 0;
static HRSRC hRsrc = NULL;
static HGLOBAL hGlobal = NULL;
static LPVOID midBuffer = NULL;
static TCHAR AsmText[128];

void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
  iCount++;
  if (iCount >= 60) iCount = 0;
  //SendDlgItemMessage(hWnd, REM_STATIC, WM_SETTEXT, 0, (LPARAM)lrct[iCount]);
}

INT_PTR CALLBACK AboutProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  switch (Msg)
  {
  case WM_INITDIALOG:
  {
    DWORD dwSize;

    hRsrc = FindResource(DllBase, MAKEINTRESOURCE(IDR_MIDI), _T("MIDI"));
    if (hRsrc == NULL) return 1;
    dwSize = SizeofResource(DllBase, hRsrc);
    if (dwSize == 0) return 1;
    hGlobal = LoadResource(DllBase, hRsrc);
    if (hGlobal == NULL) return 1;
    midBuffer = LockResource(hGlobal);
    if (midBuffer == NULL) return 1;
    PlayMIDI((PBYTE)midBuffer, dwSize);
  }
    return 1;
  case WM_CTLCOLORSTATIC:
    if ((HWND)lParam == GetDlgItem(hDlg, IDC_DONATEME))
    {
      SetBkMode((HDC)wParam, TRANSPARENT);
      SetTextColor((HDC)wParam, RGB(0, 0, 250));//设置字体颜色
      return (INT_PTR)GetStockObject(NULL_BRUSH);
    }
    break;
  case WM_COMMAND:
  {
    if (HIWORD(wParam) == STN_CLICKED)
    {
      if (LOWORD(wParam) == IDC_DONATEME)
      {
        ShellExecute(hDlg, _T("open"), _T("https://me.alipay.com/csersoft"), T_NULL, T_NULL, SW_SHOW);
        return 0;
      }
    }
    if (LOWORD(wParam) == ABOUT_OK)
    {
      EndDialog(hDlg, 1);
      return 0;
    }
  }
    break;
  case WM_CLOSE:
    EndDialog(hDlg, 1);
    return 0;
  case WM_DESTROY:
    StopMIDI();
    if (midBuffer) UnlockResource(midBuffer);
    if (hGlobal) FreeResource(hGlobal);
    midBuffer = NULL;
    hGlobal = NULL;
    KillTimer(hDlg, 1);
    return 0;
  }
  return 0;
}

INT_PTR CALLBACK GotoAddrProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam)
{
  switch (Msg)
  {
  case WM_INITDIALOG:
    SendDlgItemMessage(hDlg,IDC_EXPEDIT,EM_SETLIMITTEXT,512,0);
    SetFocus(GetDlgItem(hDlg,IDC_EXPEDIT));
    return 1;
  case WM_COMMAND:
    switch (LOWORD(wParam))
    {
    case IDC_GOTOBTN:
    {
      int tlen = GetWindowTextLength(GetDlgItem(hDlg, IDC_EXPEDIT));
      if (tlen <= 0) break;
      PTCHAR stmp = (PTCHAR)malloc((tlen + 1) * sizeof(TCHAR));
      if (!tlen) break;

      GetWindowText(GetDlgItem(hDlg, IDC_EXPEDIT), stmp, tlen + 1);
      WCharTrim(stmp);
      if (tlen = _tcslen(stmp) <= 8)
      {
        if (WCharIsHex(stmp))     //是16进制
        {
          int addr;
          MEMORY_BASIC_INFORMATION mbi;

          if (_stscanf_s(stmp, _T("%x"), &addr) != 1)
          {
            SetWindowText(GetDlgItem(hDlg, IDC_GTAD_STATUS), _T("无效参数!"));
            free(stmp);
            break;
          }
          VirtualQuery((LPVOID)addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
          if (mbi.State != MEM_COMMIT)
          {
            SetWindowText(GetDlgItem(hDlg, IDC_GTAD_STATUS), _T("无效的地址!"));
            free(stmp);
            break;
          }
          mf->mc_disasm->GotoAddress((PBYTE)addr, 0);
          free(stmp);
          EndDialog(hDlg, 1);
        }
      }
    }
      break;
    case IDC_GOBACKBTN:
      EndDialog(hDlg,0);
      break;
    }
    break;
  case WM_CTLCOLORSTATIC:
    if ((HWND)lParam == GetDlgItem(hDlg, IDC_GTAD_STATUS))
    {
      SetBkMode((HDC)wParam, TRANSPARENT);
      SetTextColor((HDC)wParam, RGB(250, 32, 32));//设置字体颜色
      return (INT_PTR)GetStockObject(NULL_BRUSH);
    }
    break;
  case WM_CLOSE:
    EndDialog(hDlg,1);
    break;
  }
  return 0;
}

INT_PTR CALLBACK AsmProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  switch (Msg)
  {
  case WM_INITDIALOG:
  {
    PVOID cAddr;

    SetFocus(GetDlgItem(hDlg, IDC_EDIT_ASM));
    cAddr = mf->mc_disasm->GetSelectLineAddress();
    if (!cAddr) EndDialog(hDlg, 0);
    SendDlgItemMessage(hDlg, IDC_EDIT_ASM, EM_SETLIMITTEXT, 126,0);
    //SendDlgItemMessage(hDlg, IDC_EDIT_ASM, WM_SETFONT, (WPARAM)mf->hf_lar, 1);
    SendDlgItemMessage(hDlg, IDC_ASM_MSG, WM_SETFONT, (WPARAM)mf->hf_med, 1);
    _stprintf_s(AsmText, 128, _T("在0x%08X处汇编以下代码(原指令长度:%u):"), cAddr, mf->mc_disasm->GetOpLengthByAddress(cAddr));
    SendDlgItemMessage(hDlg, IDC_ASM_TITLE, WM_SETTEXT, 0, (LPARAM)AsmText);
    mf->mc_disasm->GetAsmTextByAddress(cAddr, AsmText, 128);
    SendDlgItemMessage(hDlg, IDC_EDIT_ASM, WM_SETTEXT, 0, (LPARAM)AsmText);

  }
    return 1;
  case WM_COMMAND:
    switch (LOWORD(wParam))
    {
    case IDOK:
    {
      int bl;
      PVOID cAddr = mf->mc_disasm->GetSelectLineAddress();
      PWCHAR WAsmText;
      PCHAR MAsmText;
      PCHAR ErrText;
      t_asmmodel tam;

      bl = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT_ASM));
      if (!bl || !cAddr)
      {
        EndDialog(hDlg, 1);
        break;
      }
        
      WAsmText = (PWCHAR)malloc((bl + 1) * sizeof(WCHAR));
      MAsmText = (PCHAR)malloc(bl + 32);
      ErrText = (PCHAR)malloc(128);

      if (!WAsmText || !MAsmText || !ErrText)
      {
        if (WAsmText) free(WAsmText);
        if (MAsmText) free(MAsmText);
        if (ErrText) free(ErrText);
        EndDialog(hDlg, 1);
        break;
      }

      GetWindowText(GetDlgItem(hDlg, IDC_EDIT_ASM), WAsmText, bl + 1);
      if (wcscmp(AsmText, WAsmText) == 0)
      {
        free(WAsmText);
        free(MAsmText);
        free(ErrText);
        EndDialog(hDlg, 1);
        break;
      }
      wcstombs_s((size_t *)&bl, MAsmText, bl + 32, WAsmText, _TRUNCATE);
      free(WAsmText);
      
      ErrText[0] = 0;
      if (Assemble(MAsmText, (ulong)cAddr, &tam, 0, 0, ErrText) <= 0)
      {
        if (!strlen(ErrText))
          SetWindowText(GetDlgItem(hDlg, IDC_ASM_MSG), _T("汇编字节码失败!"));
        else
          SetWindowTextA(GetDlgItem(hDlg, IDC_ASM_MSG), ErrText);
        break;
      }

      if (mf->mc_disasm->AssembleOfAddress(cAddr, &tam) == 0)
      {
        EndDialog(hDlg, 1);
        break;
      }
    }
      break;
    case IDCANCEL:
      EndDialog(hDlg, 0);
      break;
    }
    break;
  case WM_CLOSE:
    EndDialog(hDlg, 0);
    break;
  }
  return 0;
}