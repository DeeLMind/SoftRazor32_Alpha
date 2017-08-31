#include "../sr_pch.h"

#pragma section("PVM",read,execute)
#pragma code_seg("PVM")

void PVM_StdFunc();
void PVM_Le0Func();
void PVM_Le1Func();
void PVM_Le2Func();
void PVM_Le3Func();
void PVM_Le4Func();
void PVM_UnknowFunction();

ALLOC_SEG("PVM") int vmkeyopcode = 0x8524FF46;
ALLOC_SEG("PVM") int vmsignature = 0x0000133F;
ALLOC_SEG("PVM") int StdFuncTable[256] = { 0 };
ALLOC_SEG("PVM") int Le0FuncTable[256] = { 0 };
ALLOC_SEG("PVM") int Le1FuncTable[256] = { 0 };
ALLOC_SEG("PVM") int Le2FuncTable[256] = { 0 };
ALLOC_SEG("PVM") int Le3FuncTable[256] = { 0 };
ALLOC_SEG("PVM") int Le4FuncTable[256] = { 0 }; /*71*/
ALLOC_SEG("PVM") static BOOL  IsHook = FALSE;
ALLOC_SEG("PVM") static DWORD vmTid = 0;
ALLOC_SEG("PVM") static int * jtArray = 0;
ALLOC_SEG("PVM") WORD        BreakFlag = 0;
ALLOC_SEG("PVM") WORD        InstBreak = 0;

/*
���������߳�,ȡ���ΪGetModuleHandle(NULL)
*/
int SetPCodeVMHook()
{
  if (IsHook) return -1;

  HANDLE hSnapshot;
  HMODULE hVBVM = GetModuleHandle(_T("MSVBVM60.dll"));
  DWORD OldProtect;
  PVOID EngineBase;
  DWORD EngineSize;
  HANDLE hThread;
  DWORD i;
  THREADENTRY32 te;

  if (!hVBVM) return -2;

  EngineBase = GetSectionAddrByName(hVBVM, "ENGINE", &EngineSize);

  if (!EngineBase)    //δ��λ��ENGINE��,��������
  {
    PIMAGE_SECTION_HEADER pSection = GetSectionBaseAddr(hVBVM);
    PBYTE SecAddr;
    PVOID bAddr;
    PUINT iAddr;
    WORD sNum = GetSectionNumber(hVBVM);

    if (!pSection || !sNum) return -3;

    for (i = 0; i < sNum; i++)
    {
      if ((pSection[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) ||
        (pSection[i].Characteristics & IMAGE_SCN_CNT_CODE))
      {
        SecAddr = (PBYTE)((UINT)hVBVM + pSection[i].VirtualAddress);
        bAddr = MemMem(SecAddr, pSection[i].Misc.VirtualSize, (PBYTE)&vmkeyopcode, 4);
        if (!bAddr) continue;
        iAddr = (PUINT)((UINT)bAddr + 4);

        if (*iAddr >= (UINT)SecAddr && *iAddr < (UINT)SecAddr + pSection[i].Misc.VirtualSize)
        {
          EngineBase = SecAddr;
          EngineSize = pSection[i].Misc.VirtualSize;
          jtArray = (int *)*iAddr;
          break;
        }
      }
    }
    if (!EngineBase) return -4;
  }
  else          //�ҵ�ENGINE��,��λ��֧��
  {
    PVOID bAddr;
    PUINT iAddr;

    bAddr = MemMem((PBYTE)EngineBase, EngineSize, (PBYTE)&vmsignature, 4);

    if (bAddr)
      jtArray = (int *)((UINT)bAddr + 4);
    else
    {
      bAddr = MemMem((PBYTE)EngineBase, EngineSize, (PBYTE)&vmkeyopcode, 4);
      if (!bAddr) return -5;
      iAddr = (PUINT)((UINT)bAddr + 4);

      if (*iAddr < (UINT)EngineBase || *iAddr >= (UINT)EngineBase + EngineSize)
        return -6;
      jtArray = (int *)*iAddr;
    }
  }

  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE) return -7;

  te.dwSize = sizeof(THREADENTRY32);
  if (Thread32First(hSnapshot, &te))
  {
    do
    {
      if (te.th32OwnerProcessID == GetCurrentProcessId())
      {
        hThread = OpenThread(TAA, 0, te.th32ThreadID);
        if (!hThread) continue;

        /*��ǰ�߳�ΪVB�����߳�*/
        if (GetThreadStartAddress(hThread) == (DWORD)GetModuleHandle(NULL))
        {
          vmTid = te.th32ThreadID;
          SuspendThread(hThread);   //����VB PCODE�����߳�

          if (!VirtualProtect(jtArray, 0x151CU, PAGE_EXECUTE_READWRITE, &OldProtect))
          {
            CloseHandle(hThread);
            CloseHandle(hSnapshot);
            return -8;
          }

          /*���ݷ�֧��*/
          memcpy_s(StdFuncTable, sizeof(int)* 256, &jtArray[0x0000], sizeof(int)* 256);
          memcpy_s(Le0FuncTable, sizeof(int)* 256, &jtArray[0x0100], sizeof(int)* 256);
          memcpy_s(Le1FuncTable, sizeof(int)* 256, &jtArray[0x0200], sizeof(int)* 256);
          memcpy_s(Le2FuncTable, sizeof(int)* 256, &jtArray[0x0300], sizeof(int)* 256);
          memcpy_s(Le3FuncTable, sizeof(int)* 256, &jtArray[0x0400], sizeof(int)* 256);
          memcpy_s(Le4FuncTable, sizeof(int)* 71, &jtArray[0x0500], sizeof(int)* 71);

          for (i = 0x0047; i < 0x0100;i++)
            Le4FuncTable[i] = (int)PVM_UnknowFunction;

          for (i = 0x0000; i < 0x0100; i++)
            jtArray[i] = (int)PVM_StdFunc;

          for (i = 0x0100; i < 0x0200; i++)
            jtArray[i] = (int)PVM_Le0Func;

          for (i = 0x0200; i < 0x0300; i++)
            jtArray[i] = (int)PVM_Le1Func;

          for (i = 0x0300; i < 0x0400; i++)
            jtArray[i] = (int)PVM_Le2Func;

          for (i = 0x0400; i < 0x0500; i++)
            jtArray[i] = (int)PVM_Le3Func;

          for (i = 0x0500; i < 0x0547; i++)
            jtArray[i] = (int)PVM_Le4Func;

          /*��ԭ�ڴ�ҳ����*/
          VirtualProtect(jtArray, 0x151CU, OldProtect, &OldProtect);
          /*�ָ��߳�*/
          ResumeThread(hThread);
          CloseHandle(hThread);
          CloseHandle(hSnapshot);
          IsHook = TRUE;
          return 0;
        }
        CloseHandle(hThread);
      }
    } while (Thread32Next(hSnapshot, &te));
    return -9;
  }
  CloseHandle(hSnapshot);
  return -10;
}

BOOL UnVMHook()
{
  if (!IsHook) return FALSE;
  HANDLE hThread = OpenThread(TAA, 0, vmTid);
  DWORD OldProtect;
  if (!hThread) return FALSE;

  /*����VB PCODE �����߳�*/
  SuspendThread(hThread);
  if (!VirtualProtect(jtArray, 0x151CU, PAGE_EXECUTE_READWRITE, &OldProtect))
  {
    CloseHandle(hThread);
    return FALSE;
  }

  /*��ԭ��֧��*/
  memcpy_s(&jtArray[0x0000], sizeof(int)* 256, StdFuncTable, sizeof(int)* 256);
  memcpy_s(&jtArray[0x0100], sizeof(int)* 256, Le0FuncTable, sizeof(int)* 256);
  memcpy_s(&jtArray[0x0200], sizeof(int)* 256, Le1FuncTable, sizeof(int)* 256);
  memcpy_s(&jtArray[0x0300], sizeof(int)* 256, Le2FuncTable, sizeof(int)* 256);
  memcpy_s(&jtArray[0x0400], sizeof(int)* 256, Le3FuncTable, sizeof(int)* 256);
  memcpy_s(&jtArray[0x0500], sizeof(int)* 71, Le4FuncTable, sizeof(int)* 71);
  
  /*��ԭ�ڴ�ҳ����*/
  VirtualProtect(jtArray, 0x151CU, OldProtect, &OldProtect);
  /*�ָ��߳�*/
  ResumeThread(hThread);
  CloseHandle(hThread);
  IsHook = FALSE;
  return TRUE;
}

ASM void PVM_StdFunc()
{
  __asm int 3
}

ASM void PVM_Le0Func()
{
  __asm int 3
}

ASM void PVM_Le1Func()
{
  __asm int 3
}

ASM void PVM_Le2Func()
{
  __asm int 3
}

ASM void PVM_Le3Func()
{
  __asm int 3
}

ASM void PVM_Le4Func()
{
  __asm int 3
}

ASM void PVM_UnknowFunction()
{
  __asm int 3
}



#pragma code_seg()