#include "stdafx.h"

INT_PTR CALLBACK MainProc(HWND, UINT, WPARAM, LPARAM);
void RefreshProcess(HWND hWnd);
void InjectionSoftRazor(DWORD Pid);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPTSTR    lpCmdLine,
  _In_ int       nCmdShow)
{
  WORD NtVer;

  MeInstance = hInstance;

  __asm call dword ptr[GetVersion]
  __asm xchg al, ah
  __asm mov NtVer, ax

  PAA |= (NtVer >= _WIN32_WINNT_VISTA) ? 0xFFFF : 0x0FFF;
  setlocale(LC_ALL, NULL);
  InitCommonControls();
  DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDLG), 0, MainProc);
  return 1;
}

INT_PTR CALLBACK MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_INITDIALOG:
  {
    LV_COLUMN lvc;
    ZeroMemory(&lvc, sizeof(LV_COLUMN));
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;

    lvc.pszText = _T("PID");
    lvc.cx = 64;
    ListView_DlgInsertColumn(hWnd, IDC_PROCLIST, 0, &lvc);

    lvc.pszText = _T("进程名称");
    lvc.cx = 128;
    ListView_DlgInsertColumn(hWnd, IDC_PROCLIST, 1, &lvc);

    lvc.pszText = _T("进程路径");
    lvc.cx = 384;
    ListView_DlgInsertColumn(hWnd, IDC_PROCLIST, 2, &lvc);

    ListView_DlgSetExtendedListViewStyle(hWnd,IDC_PROCLIST, LVS_EX_FULLROWSELECT);
    SendDlgItemMessage(hWnd, IDC_PIDEDIT, EM_SETLIMITTEXT, 10, 0);

    RefreshProcess(hWnd);
  }
    return 1;
  case WM_COMMAND:
  {
    switch (LOWORD(wParam))
    {
    case IDC_Refresh:
      RefreshProcess(hWnd);
      break;
    case IDC_IBTN:
    {
      DWORD iPid;
      TCHAR stmp[16];

      if (GetWindowTextLength(GetDlgItem(hWnd, IDC_PIDEDIT)) <= 0) break;
      GetWindowText(GetDlgItem(hWnd, IDC_PIDEDIT), stmp, 16);
      if (_stscanf_s(stmp,_T("%u"),&iPid) != 1) break;
      InjectionSoftRazor(iPid);
    }
      break;
    case IDC_EBTN:
      EndDialog(hWnd, 0);
      break;
    }
  }
    break;
  case WM_NOTIFY:
  {
    if (lParam == 0) break;
    LPNMHDR pNm = (LPNMHDR)lParam;
    switch (pNm->code)
    {
    case LVN_ITEMCHANGED:
    {
      if (pNm->idFrom == IDC_PROCLIST)
      {
        int Idx = ListView_DlgGetNextItem(hWnd, IDC_PROCLIST, -1, LVNI_FOCUSED | LVNI_SELECTED);

        if (Idx != -1)
        {
          LV_ITEM lvi;
          TCHAR tPid[12];

          lvi.iItem = Idx;
          lvi.mask = LVIF_PARAM;

          ListView_DlgGetItem(hWnd, IDC_PROCLIST, &lvi);
          tPid[0] = 0;
          _stprintf_s(tPid, 12, _T("%u"), lvi.lParam);
          SetWindowText(GetDlgItem(hWnd, IDC_PIDEDIT), tPid);
        }
      }
    }
      break;
    }
  }
    break;
  case WM_CLOSE:
    EndDialog(hWnd, 1);
    break;
  }
  return 0;
}

void RefreshProcess(HWND hWnd)
{
  if (!hWnd) return;
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  if (hSnap == INVALID_HANDLE_VALUE)
  {
    MessageBox(hWnd, _T("创建进程快照失败!"), _T("错误"), 0);
    return;
  }

  UINT Idx = 0;
  HANDLE hProcess;
  LV_ITEM lvi;
  PROCESSENTRY32 pe32;
  TCHAR stmp[_MAX_PATH];

  ZeroMemory(&lvi, sizeof(LV_ITEM));
  pe32.dwSize = sizeof(PROCESSENTRY32);

  ListView_DlgDeleteAllItems(hWnd,IDC_PROCLIST);

  if (Process32First(hSnap, &pe32))
  {
    do 
    {
      lvi.iItem = Idx;
      lvi.mask = LVIF_TEXT | LVIF_PARAM;
      lvi.cchTextMax = _MAX_PATH;
      lvi.pszText = stmp;
      lvi.lParam = (LPARAM)pe32.th32ProcessID;

      lvi.iSubItem = 0;
      _stprintf_s(stmp, _MAX_PATH, _T("%u"), pe32.th32ProcessID);
      ListView_DlgInsertItem(hWnd, IDC_PROCLIST, &lvi);

      lvi.mask = LVIF_TEXT;
      lvi.iSubItem = 1;
      _stprintf_s(stmp, _MAX_PATH, _T("%s"), pe32.szExeFile);
      ListView_DlgSetItem(hWnd, IDC_PROCLIST, &lvi);

      hProcess = OpenProcess(PAA, 0, pe32.th32ProcessID);

      if (!hProcess)
      {
        Idx++;
        continue;
      }

      GetModuleFileNameEx(hProcess, NULL, stmp, MAX_PATH);

      lvi.iSubItem = 2;
      ListView_DlgSetItem(hWnd, IDC_PROCLIST, &lvi);
      CloseHandle(hProcess);
      Idx++;

    } while (Process32Next(hSnap,&pe32));
  }
  else
    MessageBox(hWnd, _T("枚举进程失败!"), _T("错误"), 0);

  CloseHandle(hSnap);
  return;
}

void InjectionSoftRazor(DWORD Pid)
{
  HANDLE hProc = OpenProcess(PAA,0,Pid);
  HANDLE hThread;
  DWORD i;
  PVOID paCode;
  PVOID paData;
  DWORD srmax;
  WCHAR stmp[_MAX_PATH];

  if (!hProc)
  {
    swprintf_s(stmp, _MAX_PATH, _T("打开进程[%u]失败!"), Pid);
    MessageBox(0, stmp, _T("错误"), 0);
    return;
  }

  paCode = VirtualAllocEx(hProc, 0, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  if (!paCode)
  {
    CloseHandle(hProc);
    MessageBox(0, _T("申请内存失败[0]!"), _T("错误"), 0);
    return;
  }

  paData = VirtualAllocEx(hProc, 0, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!paData)
  {
    VirtualFreeEx(hProc, paCode, 0, MEM_RELEASE);
    CloseHandle(hProc);
    MessageBox(0, _T("申请内存失败[1]!"), _T("错误"), 0);
    return;
  }

  if (!WriteProcessMemory(hProc,paCode,LoadSoftRazor,4096,&i))
  {
    VirtualFreeEx(hProc, paCode, 0, MEM_RELEASE);
    VirtualFreeEx(hProc, paData, 0, MEM_RELEASE);
    CloseHandle(hProc);
    MessageBox(0, _T("写入内存失败[0]!"), _T("错误"), 0);
    return;
  }

  GetModuleFileNameW(NULL, stmp, MAX_PATH);
  (wcsrchr(stmp, L'\\'))[1] = 0;
  wcscat_s(stmp, _MAX_PATH, L"srdbg.dll");
  srmax = wcslen(stmp) * sizeof(WCHAR);

  WriteProcessMemory(hProc, paData, &srmax, 2, &i);   //Write Length
  WriteProcessMemory(hProc,(PVOID)((UINT)paData + 2), &srmax, 2, &i);     //Write MaximumLength
  srmax = (DWORD)((DWORD)paData + 8);
  WriteProcessMemory(hProc, (PVOID)((UINT)paData + 4), &srmax, 4, &i);     //Write Buffer
  WriteProcessMemory(hProc, (PVOID)((UINT)paData + 8), stmp, _MAX_PATH * sizeof(WCHAR), &i);     //Write String
  hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)paCode, paData, 0, &i);

  if (!hThread)
  {
    VirtualFreeEx(hProc, paCode, 0, MEM_RELEASE);
    VirtualFreeEx(hProc, paData, 0, MEM_RELEASE);
    CloseHandle(hProc);
    MessageBox(0, _T("创建远线程失败!"), _T("错误"), 0);
    return;
  }
    
  WaitForSingleObject(hThread, INFINITE);
  VirtualFreeEx(hProc, paCode, 0, MEM_RELEASE);
  VirtualFreeEx(hProc, paData, 0, MEM_RELEASE);
  CloseHandle(hThread);
  CloseHandle(hProc);
  MessageBox(0,_T("创建成功!"),_T("提示"),0);
}