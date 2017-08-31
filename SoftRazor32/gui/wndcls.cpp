#include "../sr_pch.h"
#include "../udis86/udisconfig.h"

static PCLASS_TABLE ClassTableHeader = NULL;
static PCLASS_TABLE LastClassTable = NULL;

BOOL AddClassTable(HWND hWnd, PVOID pClass)
{
  if (!pClass) return FALSE;

  PCLASS_TABLE ptmp = (PCLASS_TABLE)malloc(sizeof(CLASS_TABLE));

  if (!ptmp) return FALSE;

  ptmp->hWindow = hWnd;
  ptmp->pClass = pClass;
  ptmp->Next = NULL;

  if (LastClassTable)
  {
    LastClassTable->Next = ptmp;
    LastClassTable = ptmp;
  }
  else
  {
    ClassTableHeader = ptmp;
    LastClassTable = ptmp;
  }

  return TRUE;
}

PVOID GetClassPointer(HWND hWnd)
{
  PCLASS_TABLE PCT = ClassTableHeader;

  while (PCT)
  {
    if (PCT->hWindow == hWnd) return PCT->pClass;
    PCT = PCT->Next;
  }

  return NULL;
}

BOOL DelClassTable(HWND hWnd)
{
  PCLASS_TABLE PCT = ClassTableHeader;
  PCLASS_TABLE PCTL = ClassTableHeader; // 0

  if (!PCT) return FALSE;

  if (PCT->hWindow == hWnd) //Header
  {
    /* 如果最后指针等于表头 */
    if (LastClassTable == ClassTableHeader)
    {
      ClassTableHeader = ClassTableHeader->Next;
      LastClassTable = LastClassTable->Next;
    }
    else
    {
      ClassTableHeader = ClassTableHeader->Next;
    }

    free(PCT);
    return TRUE;
  }

  PCT = PCT->Next;  // 下一个结构 1

  while (PCT)
  {
    if (PCT->hWindow == hWnd)
    {
      PCTL->Next = PCT->Next;

      if (LastClassTable == PCT) //if LastClassTable == PCT , PCT->Next == NULL
        LastClassTable = PCTL;

      free(PCT);
      return TRUE;
    }

    PCTL = PCT;
    PCT = PCT->Next;
  }

  return FALSE;
}

MDIForm::~MDIForm()
{
  if (this->m_hWnd)
  {
    DestroyWindow(this->m_hWnd);
    this->m_hWnd = NULL;
  }
}

COLORREF QueryForeColor(short stype)
{
  switch (stype)
  {
  case UDCA_Inst: return DCF_Inst;
  case UDCA_StackOp: return DCF_StackOp;
  case UDCA_FMSOp: return DCF_FMSOp;
  case UDCA_CJump: return DCF_CJump;
  case UDCA_Jump: return DCF_Jump;
  case UDCA_Call: return DCF_Call;
  case UDCA_Ret: return DCF_Ret;
  case UDCA_PrivInst: return DCF_PrivInst;
  case UDCA_Prefix: return DCF_Prefix;
  case UDCA_DefClr: return DCF_DefClr;
  case UDCA_Int: return DCF_Int;
  case UDCA_Data: return DCF_Data;
  case UDCA_Text: return DCF_Text;
  case UDCA_Symbol: return DCF_Symbol;
  case UDCA_Error: return DCF_Error;
  case UDCA_GPRs: return DCF_GPRs;
  case UDCA_FSRs: return DCF_FSRs;
  case UDCA_SSRs: return DCF_SSRs;
  case UDCA_MStack: return DCF_MStack;
  case UDCA_MOther: return DCF_MOther;
  case UDCA_IMM: return DCF_IMM;
  case UDCA_IMMAddr: return DCF_IMMAddr;
  case UDCA_Change: return DCF_Change;
  default: return DCF_DefClr;
  };
}

COLORREF QueryBackColor(short stype)
{
  switch (stype)
  {
  case UDCA_Inst: return DCB_Inst;
  case UDCA_StackOp: return DCB_StackOp;
  case UDCA_FMSOp: return DCB_FMSOp;
  case UDCA_CJump: return DCB_CJump;
  case UDCA_Jump: return DCB_Jump;
  case UDCA_Call: return DCB_Call;
  case UDCA_Ret: return DCB_Ret;
  case UDCA_PrivInst: return DCB_PrivInst;
  case UDCA_Prefix: return DCB_Prefix;
  case UDCA_DefClr: return DCB_DefClr;
  case UDCA_Int: return DCB_Int;
  case UDCA_Data: return DCB_Data;
  case UDCA_Text: return DCB_Text;
  case UDCA_Symbol: return DCB_Symbol;
  case UDCA_Error: return DCB_Error;
  case UDCA_GPRs: return DCB_GPRs;
  case UDCA_FSRs: return DCB_FSRs;
  case UDCA_SSRs: return DCB_SSRs;
  case UDCA_MStack: return DCB_MStack;
  case UDCA_MOther: return DCB_MOther;
  case UDCA_IMM: return DCB_IMM;
  case UDCA_IMMAddr: return DCB_IMMAddr;
  case UDCA_Change: return DCB_Change;
  default: return DCB_DefClr;
  };
}

int DrawCasmW(HDC hdc, PWCHAR pCasm, size_t bufcount, LONG wspace, LPRECT lprc, UINT format)
{
  COLORREF fc = GetTextColor(hdc);
  COLORREF bc = GetBkColor(hdc);
  COLORREF cbc;
  PWCHAR pwcs = pCasm;
  int dcount = 0;
  size_t i;
  size_t wlen;
  SIZE wsize;

  if (!pwcs) return 0;

  for (i = 0; i < bufcount; i++)
  {
    if (pwcs[i] == 0) break;
    SetTextColor(hdc, QueryForeColor(pwcs[i]));  //获取并设置前景色
    cbc = QueryBackColor(pwcs[i]);

    if ((cbc & 0x00FFFFFF) == 0x00FFFFFF)
      SetBkMode(hdc, TRANSPARENT);//设置背景样式
    else
    {
      SetBkMode(hdc, OPAQUE);//设置背景样式
      SetBkColor(hdc, cbc);  //设置背景色
    }

    i++;    //移动索引到字符开始
    if (!(wlen = wcslen(&pwcs[i]))) break;  //取字符长度
    if (i + wlen >= bufcount) break;  //判断是否超出最大值
    GetTextExtentPointW(hdc, &pwcs[i], wlen, &wsize);
    lprc->right = lprc->left + wsize.cx /*+ wspace*/;
    DrawTextW(hdc, &pwcs[i], -1, lprc, format);
    dcount++;
    lprc->left = lprc->right;
    i += wlen;
  }
  SetTextColor(hdc, fc);
  SetBkColor(hdc, bc);
  return dcount;
}

HWND MDIForm::GetMainhWnd()
{
  return this->m_hWnd;
}

BOOL MDIForm::SetTop()
{
  return SetWindowPos(this->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

BOOL MDIForm::DestroyForm()
{
  BOOL ret;

  if (!this->m_hWnd) return FALSE;
  ret = DestroyWindow(this->m_hWnd);
  this->m_hWnd = NULL;
  return ret;
}