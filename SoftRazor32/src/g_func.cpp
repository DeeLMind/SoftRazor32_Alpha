#include "../sr_pch.h"

static DWORD address = 0x00401000;

ASM void TestFunc(DWORD IP)
{
  
  __asm
  {
      push ebp
      mov ebp,esp
      mov eax,[GetVersion]
      mov word ptr ds:[0x00401000],ax
      mov eax,[ecx]
      test eax,eax
      cmp eax,0
      push 0x00401000
      ret
  }
}

inline DWORD TestExpect(DWORD ExpectCode)
{
  return (ExpectCode == EXCEPTION_ACCESS_VIOLATION)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH;
}

BOOL HitModule(PVOID pAddr, PUINT mbase, PUINT msize)
{
  PIMAGE_NT_HEADERS32 pNtHeader;
  MEMORY_BASIC_INFORMATION MBI;

  if (!VirtualQuery(pAddr, &MBI, sizeof(MEMORY_BASIC_INFORMATION))) return FALSE;
  if (MBI.State != MEM_COMMIT) return FALSE;
  if (MBI.Type != MEM_IMAGE) return FALSE;

  pNtHeader = (PIMAGE_NT_HEADERS32)
    ((UINT)MBI.AllocationBase + ((PIMAGE_DOS_HEADER)MBI.AllocationBase)->e_lfanew);

  *mbase = (UINT)MBI.AllocationBase;

  if (pNtHeader->FileHeader.SizeOfOptionalHeader < 0x3C &&
    pNtHeader->FileHeader.SizeOfOptionalHeader > 0xE0) return FALSE;
  
  *msize = pNtHeader->OptionalHeader.SizeOfImage;
  return TRUE;
}

BOOL GetThreadDescriptor(HANDLE hThread, DWORD dwSelector, PDWORD pBase, PDWORD pLimit, PBOOL Is4KB)
{
  if (!hThread || !pBase || !pLimit) return FALSE;

  LDT_ENTRY ldte;

  if (!GetThreadSelectorEntry(hThread, dwSelector, &ldte)) return FALSE;

  *pBase = DWORD(ldte.HighWord.Bytes.BaseHi << 24 | ldte.HighWord.Bytes.BaseMid << 16 | ldte.BaseLow);
  *pLimit = DWORD(ldte.HighWord.Bits.LimitHi << 16 | ldte.LimitLow);

  if (ldte.HighWord.Bits.Granularity) *Is4KB = TRUE;
  else *Is4KB = FALSE;

  return TRUE;
}

UINT GetModuleSize(HMODULE hMod, BOOL FormImageSize)
{
  __try
  {
    if (FormImageSize)
    { 
      PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hMod;
      PIMAGE_NT_HEADERS32 pNtHeader;

      pNtHeader = (PIMAGE_NT_HEADERS32)((UINT)hMod + pDosHeader->e_lfanew);
      if (pNtHeader->FileHeader.SizeOfOptionalHeader >= 0x3C &&
        pNtHeader->FileHeader.SizeOfOptionalHeader <= 0xE0)
        return pNtHeader->OptionalHeader.SizeOfImage;
      else
        return 0;
    }
    else
    {
      PVOID AllocBase = (PVOID)hMod;
      PVOID CurBase = (PVOID)hMod;
      UINT cSize = 0;
      MEMORY_BASIC_INFORMATION MBI;

      while (VirtualQuery(CurBase, &MBI, sizeof(MEMORY_BASIC_INFORMATION)))
      {
        if (MBI.AllocationBase != AllocBase) return cSize;
        cSize += MBI.RegionSize;
        CurBase = (PVOID)((UINT)CurBase + MBI.RegionSize);
      }
      return 0;

    }
  }
  __except (TEXPECT_NOACCESS(GetExceptionCode()))
  {
    return 0;
  }
}

UINT GetModuleEntry(HMODULE hMod)
{
  PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hMod;
  PIMAGE_NT_HEADERS32 pNtHeader;

  __try
  {
    pNtHeader = (PIMAGE_NT_HEADERS32)((UINT)hMod + pDosHeader->e_lfanew);
    return (UINT)hMod + pNtHeader->OptionalHeader.AddressOfEntryPoint;
  }
  __except (TEXPECT_NOACCESS(GetExceptionCode()))
  {
    return 0;
  }
}

PVBHD GetVbHeaderByModule(HMODULE hMod)
{
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)hMod;
  if (pDos->e_magic != IMAGE_DOS_SIGNATURE) return NULL;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((DWORD)hMod + pDos->e_lfanew);
  PBYTE OEP = (PBYTE)((DWORD)hMod + pNt->OptionalHeader.AddressOfEntryPoint);
  PVBHD VBH = (PVBHD)*(PDWORD)((DWORD)OEP + 1);

  if (OEP[0] != 0x68 || OEP[5] != 0xE8) return NULL;
  if (VBH->Sign.dwSign != VB_MAGIC) return NULL;
  return VBH;
}

PIMAGE_SECTION_HEADER GetSectionBaseAddr(HMODULE hMod)
{
  if (!hMod) return NULL;

  PIMAGE_DOS_HEADER pDos;
  PIMAGE_NT_HEADERS32 pNtHeader;

  pDos = (PIMAGE_DOS_HEADER)hMod;
  if (pDos->e_magic != IMAGE_DOS_SIGNATURE) return NULL;
  pNtHeader = (PIMAGE_NT_HEADERS32)((UINT)hMod + pDos->e_lfanew);
  if (pNtHeader->Signature != IMAGE_NT_SIGNATURE) return NULL;
  return (PIMAGE_SECTION_HEADER)((UINT)pNtHeader + 0x18 + pNtHeader->FileHeader.SizeOfOptionalHeader);
}

WORD GetSectionNumber(HMODULE hMod)
{
  if (!hMod) return NULL;

  PIMAGE_DOS_HEADER pDos;
  PIMAGE_NT_HEADERS32 pNtHeader;

  pDos = (PIMAGE_DOS_HEADER)hMod;
  if (pDos->e_magic != IMAGE_DOS_SIGNATURE) return NULL;
  pNtHeader = (PIMAGE_NT_HEADERS32)((UINT)hMod + pDos->e_lfanew);
  if (pNtHeader->Signature != IMAGE_NT_SIGNATURE) return NULL;
  return pNtHeader->FileHeader.NumberOfSections;
}

PVOID GetSectionAddrByName(HMODULE hMod,PCHAR pSecName,DWORD * pSecSize)
{
  if (!hMod || !pSecName) return NULL;

  PIMAGE_DOS_HEADER pDos;
  PIMAGE_NT_HEADERS32 pNtHeader;
  PIMAGE_SECTION_HEADER pSection;
  WORD sNum;
  WORD i;

  pDos = (PIMAGE_DOS_HEADER)hMod;
  if (pDos->e_magic != IMAGE_DOS_SIGNATURE) return NULL;
  pNtHeader = (PIMAGE_NT_HEADERS32)((UINT)hMod + pDos->e_lfanew);
  if (pNtHeader->Signature != IMAGE_NT_SIGNATURE) return NULL;
  pSection = (PIMAGE_SECTION_HEADER)((UINT)pNtHeader + 0x18 + pNtHeader->FileHeader.SizeOfOptionalHeader);
  sNum = pNtHeader->FileHeader.NumberOfSections;

  for (i = 0; i < sNum;i++)
  {
    if (strcmp((PCHAR)pSection[i].Name, pSecName) == 0)
    { 
      *pSecSize = pSection[i].Misc.VirtualSize;
      return (PVOID)((UINT)hMod + pSection[i].VirtualAddress);
    }  
  }
  return NULL;
}

BOOL GetSectionBaseAddr(int dwAddr,PDWORD opSectAddr,PDWORD opSectSize)
{
  PIMAGE_DOS_HEADER pDosHeader;
  PIMAGE_NT_HEADERS32 pNtHeader;
  PIMAGE_SECTION_HEADER pSection;
  DWORD SectVa;
  WORD sNum;
  MEMORY_BASIC_INFORMATION MBI;

  __try
  {
    if (!VirtualQuery((LPVOID)dwAddr,&MBI,sizeof(MEMORY_BASIC_INFORMATION))) return FALSE;
    pDosHeader = (PIMAGE_DOS_HEADER)MBI.AllocationBase;
    pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)MBI.AllocationBase + pDosHeader->e_lfanew);
    sNum = pNtHeader->FileHeader.NumberOfSections;
    pSection = (PIMAGE_SECTION_HEADER)((DWORD)pNtHeader + 24 + pNtHeader->FileHeader.SizeOfOptionalHeader);

    while (sNum)
    {
      SectVa = (DWORD)MBI.AllocationBase + pSection->VirtualAddress;
      if ((DWORD)MBI.BaseAddress >= SectVa && (DWORD)MBI.BaseAddress < SectVa + pSection->Misc.VirtualSize)
      {
        *opSectAddr = SectVa;
        *opSectSize = pSection->Misc.VirtualSize;
        return TRUE;
      }
      sNum--;
      pSection++;
    }
    *opSectAddr = (DWORD)MBI.BaseAddress;
    *opSectSize = (DWORD)MBI.RegionSize;
    return TRUE;
  }
  __except (TEXPECT_NOACCESS(GetExceptionCode()))
  {
    return FALSE;
  }
}

void GetSectionName(HMODULE hMod,PVOID pSct,PCHAR OutName)
{
  PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hMod;
  PIMAGE_NT_HEADERS32 pNtHeader;
  WORD sNum;
  PIMAGE_SECTION_HEADER pSection;

  __try
  {
    pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)hMod + pDosHeader->e_lfanew);
    sNum = pNtHeader->FileHeader.NumberOfSections;
    pSection = (PIMAGE_SECTION_HEADER)((DWORD)pNtHeader + 24 + pNtHeader->FileHeader.SizeOfOptionalHeader);

    while (sNum)
    {
      if ((DWORD)hMod + pSection->VirtualAddress == (DWORD)pSct)
      {
        memcpy_s(OutName,8,&pSection->Name[0],8);
        OutName[8] = 0;
        return;
      }
      sNum--;
      pSection++;
    }
  }
  __except (TEXPECT_NOACCESS(GetExceptionCode()))
  {
    return;
  }
}

int ImageList_Add32bpp(HIMAGELIST hImageList, HINSTANCE hInst, UINT nId, UINT uType)
{
  HBITMAP hBitmap = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(nId), uType, 0, 0, LR_CREATEDIBSECTION);
  int i = ImageList_Add(hImageList, hBitmap, NULL);
  DeleteObject(hBitmap);
  return i;
}

void GetFileName(PTCHAR FilePath)
{
  DWORD fpl = _tcslen(FilePath);
  PTCHAR pfp = FilePath;
  PTCHAR los = 0;
  DWORD os;
  TCHAR stmp[_MAX_PATH];

  if (!FilePath || !fpl) return;

  while (*pfp)
  {
    if (memcmp(pfp,_T("\\\0"),sizeof(TCHAR)) == 0)
      los = pfp;
    pfp++;
    if (pfp > FilePath + _MAX_PATH * sizeof(TCHAR))
      break;
  }

  if (los)
  {
    los++;
    os = los - FilePath;
    os = fpl * sizeof(TCHAR) - os;
    ZeroMemory(stmp,_MAX_PATH * sizeof(TCHAR));
    memcpy(stmp,los,os);
    _tcscpy_s(FilePath,_MAX_PATH,stmp);
  }
}

void GetPageProtectString(DWORD PP,PTCHAR _Out,DWORD _Max)
{
  if(PP == 0)
  {
    _tcscpy_s(_Out,_Max,T_UNKNOWN);
    return;
  }

  if ((PP & PAGE_NOACCESS) == PAGE_NOACCESS)
  {
    _tcscpy_s(_Out,_Max,T_P_NOACCESS);
  }
  else if ((PP & PAGE_READONLY) == PAGE_READONLY)
  {
    _tcscpy_s(_Out,_Max,T_P_READONLY);
  }
  else if ((PP & PAGE_READWRITE) == PAGE_READWRITE)
  {
    _tcscpy_s(_Out,_Max,T_P_READWRITE);
  }
  else if ((PP & PAGE_WRITECOPY) == PAGE_WRITECOPY)
  {
    _tcscpy_s(_Out,_Max,T_P_WRITECOPY);
  }
  else if ((PP & PAGE_EXECUTE) == PAGE_EXECUTE)
  {
    _tcscpy_s(_Out,_Max,T_P_EXECUTE);
  }
  else if ((PP & PAGE_EXECUTE_READ) == PAGE_EXECUTE_READ)
  {
    _tcscpy_s(_Out,_Max,T_P_EXECUTE_R);
  }
  else if ((PP & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE)
  {
    _tcscpy_s(_Out,_Max,T_P_EXECUTE_RW);
  }
  else if ((PP & PAGE_EXECUTE_WRITECOPY) == PAGE_EXECUTE_WRITECOPY)
  {
    _tcscpy_s(_Out,_Max,T_P_EXECUTE_WC);
  }
  else
  {
    _tcscpy_s(_Out,_Max,T_UNKNOWN);
  }

  if ((PP & PAGE_GUARD) == PAGE_GUARD)
  {
    _tcscat_s(_Out,_Max,t_fgf);
    _tcscat_s(_Out,_Max,T_P_GUARD);
  }

  if ((PP & PAGE_NOCACHE) == PAGE_NOCACHE)
  {
    _tcscat_s(_Out,_Max,t_fgf);
    _tcscat_s(_Out,_Max,T_P_NOCACHE);
  }

  if ((PP & PAGE_WRITECOMBINE) == PAGE_WRITECOMBINE)
  {
    _tcscat_s(_Out,_Max,t_fgf);
    _tcscat_s(_Out,_Max,T_P_WRITECOMBINE);
  }
}

DWORD GetThreadInformation(HANDLE hThread,THREADINFOCLASS TIC,PVOID pData,DWORD dLen)
{
  DWORD OutVal;
  QueryInformationThread QIT = (QueryInformationThread)GetProcAddress(GetModuleHandle(T_NTDLL), "NtQueryInformationThread");

  if (!hThread) return FALSE;
  if (!QIT) return FALSE;

  if (QIT(hThread, TIC, pData, dLen, &OutVal) != STATUS_SUCCESS) return 0;
  return OutVal;
}

DWORD GetThreadStartAddress(HANDLE hThread)
{
  DWORD ret;

  if(GetThreadInformation(hThread,ThreadQuerySetWin32StartAddress,&ret,sizeof(DWORD)))
    return ret;
  else
    return 0;
}

PVOID GetThreadEnvironmentBlockAddress(HANDLE hThread)
{
  THREAD_BASIC_INFORMATION TBI;

  if(GetThreadInformation(hThread,ThreadBasicInformation,&TBI,sizeof(THREAD_BASIC_INFORMATION)))
    return TBI.TebBaseAddress;
  else
    return 0;
}

SRDBG_TS GetThreadState(ULONG ulPID, ULONG ulTID)
{
  QuerySystemInformation pNtQSI;
  ULONG n = 0x100;
  BOOL done = FALSE;

  pNtQSI = (QuerySystemInformation)GetProcAddress(GetModuleHandle(T_NTDLL),"NtQuerySystemInformation");
  if (pNtQSI == NULL) return TS_Unknown;

  //枚举得到所有进程

  PSYSTEM_PROCESSES sp = new SYSTEM_PROCESSES[n];
  PSYSTEM_PROCESSES p;

  while (pNtQSI(SystemProcessInformation , sp, n * sizeof(SYSTEM_PROCESSES), 0) == STATUS_INFO_LENGTH_MISMATCH)
  {
    delete[] sp;
    sp = new SYSTEM_PROCESSES[n = n * 2];
  }

  p = sp;

  //遍历进程列表
  while (!done)
  {
    if (p->ProcessId == ulPID)
    {
      ULONG i;

      for (i = 0; i < p->ThreadCount; i++)
      {
        SYSTEM_THREADS systemThread = p->Threads[i];

        if (systemThread.ClientId.UniqueThread == ulTID) //找到线程    
        {
          delete[] sp;

          if (systemThread.dwState == StateTerminated)
            return TS_Terminated;

          if (systemThread.dwState == StateWait)
          {
            if (systemThread.dwWaitReason == Suspended || systemThread.dwWaitReason == WrSuspended)
              return TS_Suspended;
            else
              return TS_Wait;
          }

          return TS_Running;
        }
      }
    }

    done = (p->NextEntryDelta == 0);
    p = PSYSTEM_PROCESSES(PCHAR(p) + p->NextEntryDelta);
  }

  delete[] sp;
  return TS_Unknown;
}

UINT casmlen(PWCHAR src, size_t maxcount)
{
  UINT i;
  size_t wlen;

  if (!src || !maxcount) return 0;

  for (i = 0; i < maxcount;i++)
  {
    /*如果类型符为0*/
    if (src[i] == 0) return i;
    i++;

    /*如果超过maxcount*/
    if (!(wlen = wcslen(&src[i]))) return i;
    if (i + wlen >= maxcount) return i;
    i += wlen;
  }
  return i;
}

void casmcpy(PWCHAR dest, PWCHAR src, size_t scount)
{
  size_t i;
  size_t wlen;

  if (!dest || !src || !scount) return;

  for (i = 0; i < scount; i++ /*0*/)
  {
    if (src[i] == 0)
    {
      dest[i] = 0;
      break;
    }
      
    dest[i] = src[i]; /*复制颜色索引*/
    i++;

    /*判断字符串长度*/
    if (!(wlen = wcslen(&src[i]))) break;
    /*如果当前位置加字符串的长度大于等于最大计数,则返回*/
    if (i + wlen >= scount) break;
    /*复制字串符*/
    wmemcpy_s(&dest[i], wlen, &src[i], wlen);
    /*为当前索引加上字符串长度*/
    i += wlen;
    /*置最后一位为0*/
    dest[i] = 0;
  }
}

void casmcpy_a(PWCHAR *dest,PWCHAR src,size_t scount)
{
  size_t i;
  size_t wlen;

  if (!dest || !src || !scount) return;
  PWCHAR pmem = (PWCHAR)malloc(scount * sizeof(wchar_t));
  if (!pmem) return;
  if (*dest) free(*dest);
  *dest = 0;

  for (i = 0;i < scount;i++)
  {
    if (src[i] == 0) break;
    pmem[i] = src[i];
    i++;
    
    if (!(wlen = wcslen(&src[i]))) break;
    if (i + wlen >= scount) break;
    wmemcpy_s(&pmem[i],wlen,&src[i],wlen);
    i += wlen;
    pmem[i] = 0;
  }
  *dest = pmem;
}

BOOL WCharIsHex(PWCHAR src)
{
  if (!src) return FALSE;
  UINT slen = wcslen(src);
  if (!slen) return FALSE;
  UINT i;

  for (i = 0; i < slen;i++)
  {
    if (!(src[i] >= L'0' && src[i] <= L'9' || src[i] >= L'A' && src[i] <= L'F' || src[i] >= L'a' && src[i] <= L'f'))
      return FALSE;
  }
  return TRUE;
}

BOOL WCharTrim(PWCHAR src)
{
  if (!src) return FALSE;
  UINT slen = wcslen(src);
  if (!slen) return FALSE;
  PWCHAR stmp = (PWCHAR)calloc(slen + 1,sizeof(wchar_t));
  UINT i;
  int cb = -1;
  int ce = -1;

  for (i = 0; i < slen;i++)
  {
    if (src[i] == L' ' || src[i] == L'\t') continue;
    if (cb == -1) cb = i;
    ce = i;
  }

  if (cb == -1)
  {
    free(stmp);
    return FALSE;
  }

  wmemcpy(stmp, &src[cb], ce - cb + 1);
  wcscpy_s(src,slen + 1,stmp);
  free(stmp);
  return TRUE;
}


PVOID WINAPI MemMem(PBYTE pMem1,SIZE_T MemSize1,PBYTE FindMem,SIZE_T fmSize)
{
  UINT i;

  if (fmSize >= MemSize1) return NULL;
  for (i = 0; i <= MemSize1 - fmSize;i++)
  {
    if (memcmp(&pMem1[i], FindMem, fmSize) == 0)
      return &pMem1[i];
  }
  return NULL;
}


ASM UINT WINAPI FindMemory(PBYTE destAddr,UINT destSize,PCMP_BYTE pCB,UINT cbCount,BOOL Absolute)
{
  __asm
  {
      push ebp
      mov ebp,esp
      push ebx
      push esi
      push edi
      push eax                            //i
      cmp destAddr,0                      //判断参数是否为NULL
      je BackErr
      cmp destSize,0
      je BackErr
      cmp pCB,0
      je BackErr
      cmp cbCount,0
      je BackErr
      mov eax,cbCount
      cmp eax,destSize
      ja BackErr
      mov edi,destAddr
dLoop:
      mov esi,pCB
      xor eax,eax
      mov [ebp-0x10],eax
sLoop:
      mov ax,word ptr [esi]               //取CMP_BYTE
      test al,al                          //判断IsValid
      je sNext
      mov ebx,[ebp-0x10]
      cmp ah,byte ptr [edi+ebx]
      jne dNext

sNext:
      inc dword ptr [ebp-0x10]            //自加计数
      add esi,0x02                        //自加结构指针
      mov eax,dword ptr [ebp-0x10]
      cmp eax,cbCount                     //比较cbCount和计数
      jb sLoop                            //计数 < cbCount 跳转
      mov eax,Absolute
      test eax,eax
      mov eax,edi
      jne Return                       
      sub eax,destAddr
      jmp Return
dNext:
      inc edi                             //srcAddr++
      mov eax,destSize                 
      sub eax,cbCount                     //eax = srcSize - cbCount
      add eax,destAddr
      cmp edi,eax                         //比较
      jbe dLoop   

BackErr:
      xor eax,eax
      dec eax
Return:
      pop ecx
      pop edi
      pop esi
      pop ebx
      mov esp,ebp
      pop ebp
      retn 0x14
  }
}
