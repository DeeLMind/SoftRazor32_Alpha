#include "../sr_pch.h"

extern const VB_PCODE_STD_INST PCodeStdInst[251];
extern const VB_PCODE_LEAD_INST PCodeLead0Inst[256];
extern const VB_PCODE_LEAD_INST PCodeLead1Inst[256];
extern const VB_PCODE_LEAD_INST PCodeLead2Inst[256];
extern const VB_PCODE_LEAD_INST PCodeLead3Inst[256];
extern const VB_PCODE_LEAD_INST PCodeLead4Inst[256];
extern const VB_PCODE_LEAD_INST * pvbinst[5];

inline BYTE GetByteFormPoint(void * pany)
{
  return *(BYTE *)pany;
}

inline signed short int GetSI16FormSI16(void * pint16)
{
  return *(signed short int *)pint16;
}

inline unsigned short int GetUI16FormUI16(void * pint16)
{
  return *(unsigned short int *)pint16;
}

inline unsigned int GetUI32FormSI16(void * pint16)
{
  return (unsigned int)*(signed short int *)pint16;
}

inline signed int GetSI32FormSI16(void * pint16)
{
  return (signed int)*(signed short int *)pint16;
}

inline signed int GetSI32FormSI32(void * pint32)
{
  return *(signed int *)pint32;
}

inline unsigned int GetUI32FormSI32(void * pint32)
{
  return (unsigned int)*(signed int *)pint32;
}

BOOL GetProcName(UINT hMod,UINT pAddr,PCHAR * oppchr,PCHAR * pmName)
{
  if (!hMod || !pAddr || !oppchr) return FALSE;

  PIMAGE_NT_HEADERS32 nthdr = (PIMAGE_NT_HEADERS32)(hMod + ((PIMAGE_DOS_HEADER)hMod)->e_lfanew);
  if (nthdr->FileHeader.SizeOfOptionalHeader < 0xE0) return FALSE;
  if (nthdr->OptionalHeader.NumberOfRvaAndSizes < 2) return FALSE;
  PIMAGE_IMPORT_DESCRIPTOR pImpDir = (PIMAGE_IMPORT_DESCRIPTOR)(hMod + nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
  PIMAGE_THUNK_DATA32 pOrgThunk;
  PIMAGE_THUNK_DATA32 pThunk;
  UINT i = 0;
  UINT j;

  while (pImpDir[i].OriginalFirstThunk)
  {
    j = 0;
    pOrgThunk = (PIMAGE_THUNK_DATA32)(hMod + pImpDir[i].OriginalFirstThunk);
    pThunk = (PIMAGE_THUNK_DATA32)(hMod + pImpDir[i].FirstThunk);

    while (pOrgThunk[j].u1.AddressOfData)
    {
      if (pThunk[j].u1.Function == pAddr)
      {
        if (IMAGE_SNAP_BY_ORDINAL32(pOrgThunk[j].u1.Ordinal))
          *oppchr = (PCHAR) (pOrgThunk[j].u1.Ordinal & 0xFFFF);
        else
          *oppchr = (PCHAR) &((PIMAGE_IMPORT_BY_NAME(hMod + pOrgThunk[j].u1.AddressOfData))->Name[0]);

        *pmName = (PCHAR)(hMod + pImpDir[i].Name);
        return TRUE;
      }
      j++;
    }
    i++;
  }
  return FALSE;
}

static void NTAPI MakeArg(signed int ival, wchar_t * iText, size_t maxc)
{
  wchar_t stmp[16];

  if (ival < 0)
    swprintf_s(stmp, 16, L"var_%X", -ival);
  else
    swprintf_s(stmp, 16, L"arg_%X", ival);

  wcscat_s(iText, maxc, stmp);
}

static void NTAPI MakeAddr(PPDO pcdo, UINT ival)
{
  static TCHAR stmp[260];
  signed int si32;

  stmp[0] = 0;
  if (ival >= pcdo->mbase && (ival < pcdo->mbase + pcdo->msize))
  {
    if (*(BYTE *)ival == 0xBA && *(BYTE *)(ival + 5) == 0xB9)
    {
      si32 = GetSI32FormSI32((void *)(ival + 1));
      if (!pcdo->func_qs || !pcdo->func_qs(si32, stmp, 260))
        _stprintf_s(stmp, 260, L"proc_%08X", si32);
    }
    else if (*(short *)ival == 0x25FF)
    {
      si32 = GetSI32FormSI32((void *)(ival + 2));
      _stprintf_s(stmp, 260, _T("extimp_%08X"), si32);
    }
    else
      _stprintf_s(stmp, 260, _T("unkn_%08X"), ival);
  }
  else
  {
    _stprintf_s(stmp, 260, _T("??=%08X"), ival);
  }

  _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);
}

static void NTAPI MakeAddrToVB(PPDO pcdo,UINT ival)
{
  static TCHAR stmp[260];
  signed int si32;

  if (ival >= pcdo->mbase && (ival < pcdo->mbase + pcdo->msize))
  {
    stmp[0] = 0;

    if (*(BYTE *)ival == 0xBA && *(BYTE *)(ival + 5) == 0xB9)
    {
      si32 = GetSI32FormSI32((void *)(ival + 1));
      if (!pcdo->func_qs || !pcdo->func_qs(si32, stmp, 260))
        _stprintf_s(stmp, 260, L"proc_%08X", si32);
    }
    else if (*(short *)ival == 0x25FF)
    {
      int ImpAddr;
      PCHAR pImp = 0;
      PCHAR pmName = 0;
      size_t bval;

      si32 = GetSI32FormSI32((void *)(ival + 2));
      
      ImpAddr = *(int *)si32;

      if (GetProcName(pcdo->mbase, ImpAddr, &pImp, &pmName))
      {
        mbstowcs_s(&bval, stmp, 260, pmName, _TRUNCATE);

        if ((UINT)pImp > 0xFFFF)
        {
          _tcscat_s(stmp, 260, _T("->"));
          _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);
          mbstowcs_s(&bval, stmp, 260, pImp, _TRUNCATE);
        }
        else
        {
          _tcscat_s(stmp, 260, _T("#>"));
          _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);
          _stprintf_s(stmp, 260, _T("%X"), pImp);
        }
      }
      else if (!pcdo->func_qs || !pcdo->func_qs(si32, stmp, 260))
        _stprintf_s(stmp, 260, _T("extimp_%08X"), si32);
    }
    else
      _stprintf_s(stmp, 260, _T("unkn_%08X"), ival);
  }
  else
  {
    _stprintf_s(stmp, 260, _T("??=%08X"), ival);
  }
  _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);
}

static void NTAPI ReturnApiCall(PPDO pcdo,UINT iAddr)
{
  static TCHAR stmp[260];
  unsigned int ui32;
  unsigned int ui32tmp;
  size_t bval;

  if ((*(char *)iAddr == 0xA1) && (*(char *)(iAddr + 11) == 0x68))
  {
    ui32 = GetUI32FormSI32((void *)(iAddr + 12));
    if ((ui32 >= pcdo->mbase) && (ui32 < pcdo->mbase + pcdo->msize))
    {
      ui32tmp = GetUI32FormSI32((void *)ui32);
      mbstowcs_s(&bval, stmp, 260, (char *)ui32tmp, _TRUNCATE);
      _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);

      _tcscat_s(pcdo->szMnem, pcdo->szlen, _T("->"));

      ui32tmp = GetUI32FormSI32((void *)(ui32 + 4));
      mbstowcs_s(&bval, stmp, 260, (char *)ui32tmp, _TRUNCATE);
      _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);
      return;
    }
  }
  MakeAddrToVB(pcdo, iAddr);
}

/*
    pcd_initdeobj
    初始化解码对象
    返回值:
    0为成功,非0失败
*/
int pcd_initdeobj(PPDO pcdo, PTCHAR pszBuf, UINT szsize, PBYTE inbuf, UINT bcount, UINT mbase, UINT msize, PVBPDI ppdi)
{
  if (!pcdo) return -1;
  if (!pszBuf || !szsize) return -2;
  if (!inbuf || !bcount) return -3;
  if (!mbase || !msize) return -4;
  if (!ppdi) return -5;

  pcdo->inp_opbuf = inbuf;
  pcdo->inp_idx = 0;
  pcdo->inp_count = bcount;
  pcdo->opSize = 0;
  pcdo->otSize = 0;
  pcdo->uFlag = 0;
  pcdo->opType = 0;
  pcdo->arType = 0;
  pcdo->szMnem = pszBuf;
  pcdo->szlen = szsize;
  pcdo->lastval = 0;
  pcdo->mbase = mbase;
  pcdo->msize = msize;
  pcdo->ppdi = ppdi;
  pcdo->orgcp = 0;
  pcdo->func_qs = NULL;
  pcdo->base = 0;
  return 0;
}

/*
    pcd_decode
    解码pcode
    返回值:
    0为成功,非0失败
*/
int pcd_decode(PPDO pcdo,UINT dcFlag)
{
  UINT idx;
  UINT max;
  PBYTE pbuf;
  BYTE opc;
  BYTE lidx;      //Lead Index

  if (!pcdo) return -1;
  if (!pcdo->inp_opbuf || !pcdo->inp_count) return -2;
  if (pcdo->inp_idx > pcdo->inp_count) return -3;      //End Of Buffer
  if (pcdo->inp_idx == pcdo->inp_count) return 1;      //End Of Buffer

  idx = pcdo->inp_idx;
  max = pcdo->inp_count;
  pbuf = pcdo->inp_opbuf;
  opc = pbuf[idx];            //[o][?][?]
  
  pcdo->base = idx;
  pcdo->std = opc;

  if (opc >= 0xFB && opc <= 0xFF)   //lead
  {
    if (++idx >= max) return -4;    //[*][o][?]
    lidx = opc - 0xFB;
    opc = pbuf[idx];
    pcdo->lead = opc;
    
    if (pvbinst[lidx][opc].InstSize < 0)      //size == -1 变参
    {
      if (idx + 3 > max) return -5;    
      USHORT vlen = *(USHORT *)(&pbuf[++idx]);

      if (vlen & 1)
      {
        pcdo->lastval |= PCEC_NOTEVEN;
        vlen &= 0xFFFE;
      }

      idx += 2;
      pcdo->opSize = 4;
      pcdo->otSize = 4 + vlen;
      pcdo->opType = pvbinst[lidx][opc].uType;
      idx += vlen;
    }
    else
    {
      idx += pvbinst[lidx][opc].InstSize;
      pcdo->opSize = 2;
      pcdo->otSize = pvbinst[lidx][opc].InstSize + 1;
      pcdo->opType = pvbinst[lidx][opc].uType;
    }
  }
  else
  {
    if (PCodeStdInst[opc].InstSize < 0)      //size == -1 变参
    {
      if (idx + 3 > max) return -6;
      USHORT vlen = *(USHORT *)(&pbuf[++idx]);

      if (vlen & 1)
      {
        pcdo->lastval |= PCEC_NOTEVEN;
        vlen &= 0xFFFE;
      }

      idx += 2;
      pcdo->opSize = 3;
      pcdo->otSize = 3 + vlen;
      pcdo->opType = PCodeStdInst[opc].uType;
      idx += vlen;
    }
    else
    {
      idx += PCodeStdInst[opc].InstSize;
      pcdo->opSize = 1;
      pcdo->otSize = PCodeStdInst[opc].InstSize;
      pcdo->opType = PCodeStdInst[opc].uType;
    }
  }

  if (idx > max)
  {
    pcdo->lastval |= PCEC_NOBYTE;
    return 2;
  }
  else if (idx == max) pcdo->lastval |= PCEC_DECEND;

  if ((dcFlag & 1) == 0) pcdo->inp_idx = idx;

  return 0;
}

/*
 
*/
int pcd_printarg(PPDO pcdo, PLONG cpool, LONG ocpool)
{
  static TCHAR stmp[260];

  if (pcdo->std >= 0xFB) return -33;
  if (PCodeStdInst[pcdo->std].ParamText == NULL) return -34;
  if (pcdo->base + pcdo->opSize >= pcdo->inp_count) return -35;
  if (pcdo->opSize >= pcdo->otSize) return -36;
  if (pcdo->base + pcdo->otSize > pcdo->inp_count) return -37;
  if (!pcdo->ppdi || !pcdo->ppdi->pTableInfo) return -38;
  if (!cpool) return -39;

  PBYTE vbuf = &pcdo->inp_opbuf[pcdo->base + pcdo->opSize];
  UINT vidx = 0;
  UINT vmax = pcdo->otSize - pcdo->opSize;
  PTCHAR prmt = PCodeStdInst[pcdo->std].ParamText;
  signed int si32;
  unsigned int ui32;
  signed short si16;
  unsigned short ui16;
  signed int si32tmp;

  while (*prmt)
  {
    if (*prmt != _T('%'))
    {
      _tcsncat_s(pcdo->szMnem, pcdo->szlen, prmt, 1);
      prmt++;
      continue;
    }

    if (*++prmt == 0) break;

    switch (*prmt)
    {
    case _T('a'):
      if (vidx + 2 > vmax) return -40;
      si32 = GetSI32FormSI16(&vbuf[vidx]);
      MakeArg(si32, pcdo->szMnem, pcdo->szlen);
      vidx += 2;
      break;
    case _T('c'):
      if (vidx + 2 > vmax) return -41;
      si16 = GetSI16FormSI16(&vbuf[vidx]);
      si32 = si16 * 4 + *cpool;
      si32 = GetSI32FormSI32((void *)si32);
      MakeAddrToVB(pcdo, (UINT)si32);
      vidx += 2;
      break;
    case _T('e'):
      if (vidx + 4 > vmax) return -42;
      si16 = GetSI16FormSI16(&vbuf[vidx]);
      si32 = si16 * 4 + *cpool;
      si32 = GetSI32FormSI32((void *)si32);
      si32 += GetSI32FormSI16(&vbuf[vidx + 2]);
      MakeAddr(pcdo, (UINT)si32);
      vidx += 4;
      break;
    case _T('x'):
      if (vidx + 2 > vmax) return -43;
      si16 = GetSI16FormSI16(&vbuf[vidx]);
      si32 = si16 * 4 + *cpool;
      si32 = GetSI32FormSI32((void *)si32);
      ReturnApiCall(pcdo, (UINT)si32);
      vidx += 2;
      break;
    case _T('s'):
      if (vidx + 2 > vmax) return -44;
      si16 = GetSI16FormSI16(&vbuf[vidx]);
      si32 = si16 * 4 + *cpool;
      si32 = GetSI32FormSI32((void *)si32);

      _stprintf_s(stmp, 260, _T("v_%08X"), si32);
      _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);

      if (pcdo->szNote && pcdo->sznlen)
      {
        _tcscpy_s(pcdo->szNote, pcdo->sznlen, _T(" \""));
        swprintf_s(stmp, 260, L"%s", si32);
        _tcscat_s(stmp, 260, _T("\" "));
        _tcscat_s(pcdo->szNote, pcdo->sznlen, stmp);
      }

      vidx += 2;
      break;
    case _T('l'):
      if (vidx + 2 > vmax) return -45;
      si32 = (int)pcdo->inp_opbuf + GetSI32FormSI16(&vbuf[vidx]);
      _stprintf_s(stmp, 260, _T("loc_%08X"), si32);
      _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);
      vidx += 2;
      break;
    case _T('1'):
      if (vidx + 1 > vmax) return -46;
      ui32 = (BYTE)*(unsigned int *)&vbuf[vidx];
      _stprintf_s(stmp, 260, _T("%02X"), ui32);
      _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);
      vidx++;
      break;
    case _T('2'):
      if (vidx + 2 > vmax) return -47;
      ui32 = (USHORT)*(unsigned int *)&vbuf[vidx];
      _stprintf_s(stmp, 260, _T("%04X"), ui32);
      _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);
      vidx += 2;
      break;
    case _T('4'):
      if (vidx + 4 > vmax) return -48;
      ui32 = *(unsigned int *)&vbuf[vidx];
      _stprintf_s(stmp, 260, _T("%08X"), ui32);
      _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);
      vidx += 4;
      break;
    case _T('t'):
      si16 = GetSI16FormSI16(&vbuf[vidx]);
      si32 = si16 * 4 + ocpool;
      si32 = GetSI32FormSI32((void *)si32);
      _stprintf_s(stmp, 260, _T("xxx_%08X"), si32);
      _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);
      *cpool = si32;
      break;
    case _T('v'):
      if (vidx + 4 > vmax) return -49;
      ui16 = GetUI16FormUI16(&vbuf[vidx]);
      vidx += 2;
      si16 = GetSI16FormSI16(&vbuf[vidx]);
      si32tmp = GetSI32FormSI32((void *)(si16 * 4 + *cpool));
      GetPropertyText(*(GUID *)si32tmp, ui16, pcdo->szMnem, pcdo->szlen, pcdo->szNote, pcdo->sznlen);
      break;
    default:
      break;
    }
    prmt++;
  }

  return 0;
}

int pcd_disassemble(PPDO pcdo, PBOOL blInit)
{
  int iret;
  size_t bval;
  static LONG bkcp;
  static LONG scp;
  PLONG pcp;
  BYTE cop; //current op
  BYTE opidx;

  if (!pcdo) return -65;
  if (!pcdo->inp_opbuf || !pcdo->inp_count) return -66;
  if (!pcdo->szMnem || pcdo->szlen == 0) return -67;
  if (pcdo->inp_idx > pcdo->inp_count) return -68;      //End Of Buffer
  if (!blInit) return -69;
  if (pcdo->inp_idx == pcdo->inp_count) return 1;

  iret = pcd_decode(pcdo, 0);
  if (iret != 0) return iret;

  pcdo->szMnem[0] = 0;
  if (pcdo->sznlen && pcdo->szNote) pcdo->szNote[0] = 0;

  if (*blInit)
  {
    bkcp = pcdo->ppdi->pTableInfo->ConstPool;
    scp = pcdo->ppdi->pTableInfo->ConstPool;
    *blInit = FALSE;
  }

  cop = pcdo->std;
  if (cop < 0xFB)   //标准指令
  {
    if (mbstowcs_s(&bval, pcdo->szMnem, pcdo->szlen, PCodeStdInst[cop].InstText, _TRUNCATE))
      return -70;

    if (PCodeStdInst[cop].ParamText)
    {
      switch (pcdo->opType & 0xFFFF)
      {
      case PCT_IDX:
        pcp = &scp;
        break;
      default:
        pcp = &bkcp;
        break;
      }

      iret = pcd_printarg(pcdo, pcp, pcdo->ppdi->pTableInfo->ConstPool);
      if (iret != 0) return iret;
    }
  }
  else
  {
    TCHAR stmp[128];

    opidx = cop - 0xFB;
    cop = pcdo->lead;

    _stprintf_s(pcdo->szMnem, pcdo->szlen, _T("#%1u:"), (UINT)opidx);

    if (mbstowcs_s(&bval, stmp, 128, pvbinst[opidx][cop].InstText, _TRUNCATE))
      return -71;

    _tcscat_s(pcdo->szMnem, pcdo->szlen, stmp);
  }

  if ((pcdo->lastval & PCEC_DECEND) == PCEC_DECEND) return 1;
  if ((pcdo->opType & PCF_ENDPROC) == PCF_ENDPROC) return 1;

  return 0;
}

