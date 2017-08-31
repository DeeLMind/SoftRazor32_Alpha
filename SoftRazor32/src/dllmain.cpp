#include "../sr_pch.h"

BOOL WINAPI DllMain(HMODULE hModule,DWORD call_type,LPVOID lpReserved)
{
  switch (call_type)
  {
  case DLL_PROCESS_ATTACH:
    {
      if (NtVer < _WIN32_WINNT_WINXP)
      {
        MessageBox(0,NVE_TEXT,ERR_TITLE,MB_ICONERROR);
        return FALSE;
      }

      PAA |= (NtVer >= _WIN32_WINNT_VISTA)?0xFFFF:0x0FFF;
      TAA |= (NtVer >= _WIN32_WINNT_VISTA)?0xFFFF:0x03FF;
      setlocale(LC_ALL,NULL);
      InitCommonControls();
    }
    break;
  case DLL_PROCESS_DETACH:
    if (mf) mf->Release();
    break;
  }
  return TRUE;
}