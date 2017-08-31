#include "../sr_pch.h"
#include "../resource.h"

MainForm  * mf = NULL;

UINT WINAPI DbgWndThread(PVOID Param)
{
  BOOL gmret;
  MSG dwmsg;

  DllMain(DllBase, DLL_THREAD_ATTACH, 0);
  InitCommonControls();

  if (!mf)
  {
    mf = MainForm::GetInstance(DllBase);
    if (!mf) return 0;
  }
  mf->CreateForm(WN_Main, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_MAXIMIZE, DllBase);
  if (mf->GetMainWindow() == NULL) return 0;

  while ((gmret = GetMessage(&dwmsg, 0, 0, 0)) != 0)
  {
    if (gmret == -1)
      return -1;

    if (mf->GetDbgState() != DS_Idle)
    {
      switch (dwmsg.message)
      {
      /* 不处理的消息 */
      case WM_KEYDOWN:
      case WM_KEYUP:
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_RBUTTONDOWN:
      case WM_RBUTTONUP:
      case WM_CLOSE:
        continue;
      default:
        break;
      }
    }
    TranslateMessage(&dwmsg);
    DispatchMessage(&dwmsg);
  }

  DllMain(DllBase, DLL_THREAD_DETACH, 0);
  return 1;
}

BOOL WINAPI LaunchDebugger(long LaunchParam)
{
  if (ht_main) return FALSE;
  ht_main = _beginthreadex(0, 4194304, &DbgWndThread, NULL, 0, &tid_main);
  return (ht_main == 0 || ht_main == -1) ? FALSE : TRUE;
}

BOOL WINAPI TerminationDebugger(BOOL blEnforce)
{
  if (ht_main == 0 || ht_main == -1) return FALSE;
  if (mf) mf->Release();
  _endthreadex(ht_main);
  ht_main = NULL;
  tid_main = 0;
  return TRUE;

}

