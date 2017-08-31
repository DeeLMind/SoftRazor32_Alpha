#include "sr_pch.h"

const GUID vbclsid = { 0xFCFB3D2E, 0xA0FA, 0x1068, 0xA7, 0x38, 0x08, 0x00, 0x2B, 0x33, 0x71, 0xB5 };

WORD              NtVer = NULL;
HMODULE           DllBase = NULL;
DWORD             PAA = (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE);
DWORD             TAA = (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE);

COLORREF          DCF_Inst = RGB(24, 24, 24);
COLORREF          DCF_StackOp = RGB(0, 0, 255);
COLORREF          DCF_FMSOp = RGB(128, 64, 0);
COLORREF          DCF_CJump = RGB(255, 0, 0);
COLORREF          DCF_Jump = RGB(0, 0, 0);
COLORREF          DCF_Call = RGB(32, 0, 0);
COLORREF          DCF_Ret = RGB(0, 32, 0);
COLORREF          DCF_PrivInst = RGB(255, 64, 64);
COLORREF          DCF_Prefix = RGB(64, 255, 64);
COLORREF          DCF_DefClr = RGB(0, 0, 0);
COLORREF          DCF_Int = RGB(64, 128, 230);
COLORREF          DCF_Data = RGB(160, 160, 0);
COLORREF          DCF_Text = RGB(160, 32, 32);
COLORREF          DCF_Symbol = RGB(32, 255, 160);
COLORREF          DCF_Error = RGB(255, 255, 255);
COLORREF          DCF_GPRs = RGB(128, 0, 128);
COLORREF          DCF_FSRs = RGB(0, 128, 128);
COLORREF          DCF_SSRs = RGB(255, 0, 255);
COLORREF          DCF_MStack = RGB(32, 64, 160);
COLORREF          DCF_MOther = RGB(32, 64, 160);
COLORREF          DCF_IMM = RGB(64, 160, 96);
COLORREF          DCF_IMMAddr = RGB(128, 0, 0);
COLORREF          DCF_Change = RGB(255, 0, 0);

COLORREF          DCB_Inst = RGB(255, 255, 255);
COLORREF          DCB_StackOp = RGB(255, 255, 255);
COLORREF          DCB_FMSOp = RGB(255, 255, 255);
COLORREF          DCB_CJump = RGB(255, 255, 64);
COLORREF          DCB_Jump = RGB(255, 255, 0);
COLORREF          DCB_Call = RGB(32, 255, 255);
COLORREF          DCB_Ret = RGB(255, 32, 255);
COLORREF          DCB_PrivInst = RGB(128, 255, 255);
COLORREF          DCB_Prefix = RGB(255, 160, 255);
COLORREF          DCB_DefClr = RGB(255, 255, 255);
COLORREF          DCB_Int = RGB(255, 255, 255);
COLORREF          DCB_Data = RGB(255, 255, 255);
COLORREF          DCB_Text = RGB(255, 255, 255);
COLORREF          DCB_Symbol = RGB(255, 255, 255);
COLORREF          DCB_Error = RGB(255, 0, 0);
COLORREF          DCB_GPRs = RGB(255, 255, 255);
COLORREF          DCB_FSRs = RGB(255, 255, 255);
COLORREF          DCB_SSRs = RGB(255, 255, 255);
COLORREF          DCB_MStack = RGB(128, 128, 0);
COLORREF          DCB_MOther = RGB(255, 255, 255);
COLORREF          DCB_IMM = RGB(255, 255, 255);
COLORREF          DCB_IMMAddr = RGB(255, 255, 255);
COLORREF          DCB_Change = RGB(255, 255, 255);

UINT              ht_main = NULL;
UINT              tid_main = NULL;

PTCHAR            WC_Main = _T("sr_window");
PTCHAR            WC_Module = _T("sr_module");
PTCHAR            WC_Memory = _T("sr_memory");
PTCHAR            WC_Heap = _T("sr_heap");
PTCHAR            WC_Thread = _T("sr_thread");
PTCHAR            WC_Disasm = _T("sr_disasm");
PTCHAR            WC_VB = _T("sr_vbstruct");
PTCHAR            WC_PCP = _T("sr_pcodeproc");

PTCHAR            LN_Module = _T("ProcessListView");
PTCHAR            LN_Section = _T("SectionListView");
PTCHAR            LN_Memory = _T("MemoryMapListView");
PTCHAR            LN_Heap = _T("HeapListView");
PTCHAR            LN_Thread = _T("ThreadListView");
PTCHAR            TN_VB = _T("VBTreeView");
PTCHAR            LN_VB = _T("StructListView");
PTCHAR            WN_Main = _T("Soft Razor Win32");
PTCHAR            WN_Module = _T("ģ���б�");
PTCHAR            WN_Memory = _T("�ڴ�ӳ��");
PTCHAR            WN_Heap = _T("���̶��б�");
PTCHAR            WN_Thread = _T("�߳��б�");
PTCHAR            WN_Disasm = _T("�����");
PTCHAR            WN_VB = _T("VB�ṹ����");
PTCHAR            WN_PCP = _T("P-Code����");
PTCHAR            CN_MDI = _T("MDIClient");
PTCHAR            CN_Static = _T("Static");
PTCHAR            CN_Edit = _T("Edit");
PTCHAR            CN_Button = _T("Button");
PTCHAR            t_fgf = _T("|");
PTCHAR            sc_ex = _T("��ִ��");
PTCHAR            sc_or = _T("ֻ��");
PTCHAR            sc_rw = _T("��д");
PTCHAR            sc_sh = _T("����");
PTCHAR            sc_dc = _T("�ɶ���");
PTCHAR            sc_np = _T("����ҳ");
PTCHAR            sc_rt = _T("�ض�λ");
PTCHAR            sc_id = _T("�ѳ�ʼ������");
PTCHAR            sc_ud = _T("δ��ʼ������");
PTCHAR            sc_cd = _T("����COMDAT");
PTCHAR            t_ref = _T("ˢ��");
PTCHAR            t_viw = _T("�鿴");

/*
PTCHAR            lrct[60] = { _T("�� ����ѡ���ҵ���"), \
_T("������ ��Ҳ������ڹ�"), \
_T("��֪�����������н��"), \
_T("������������Щʲô"), \
_T("���ٵ�ʱ�� ����һ����"), \
_T("��Ϊ�ڿ� ��Ϊ�����ţ"), \
_T("Ϊ���� ���Ƿ����˺ܶ�"), \
_T("ѧϰ�����飬���� ��������"), \
_T("����������Ҳ�������"), \
_T("����ӻ���ʱ����Ҳ�޷�׷��"), \
_T("����ʧȥ�˰��Ļ��ᣬ����Ļ���"), \
_T("��Ҳֻ����α��˵��������ν"), \
_T("ֱ�����ڴ����Ƿ�̫��"), \
_T("�����Ĺؿ� �ұ���ͣ�ĳ�ˢ"), \
_T("ʲôʱ�������������������"), \
_T("ʲôʱ�����������������"), \
_T("�����Ǹ�Ů�� û����ν���ھ�"), \
_T("���������� ȴ��Ŀ�صĽ��"), \
_T("�ҵ�ԭ���������Ϊ����"), \
_T("�����ɿ��ҵ��� ��Ҫ �ٻ�ͷ"), \
_T("���뵱��ѡ������ε�ʱ��"), \
_T("ֻ��Ϊ������ѧ��ʱ�ڵ�Ů��"), \
_T("��֪�� ��������ô��"), \
_T("�����и����õ���������������"), \
_T("����������ѧϰ �������ã"), \
_T("ֻϣ�����������ֹ�����������"), \
_T("������˭����ں���"), \
_T("�� ����ѡ���ҵ���"), \
_T("������ ��Ҳ������ڹ�"), \
_T("��֪�����������Ľ��"), \
_T("ֻϣ�����ܹ��ÿ���"), \
_T("��˭�ں��� �������Ѿ������˼�į"), \
_T("�滻�˳�û formatҲɾ�������������"), \
_T("������Ǳˮ ����¶�Ҳһ����"), \
_T("�������Ƿ� ���ȵ���һ��"), \
_T("װ��������õ�cocktail"), \
_T("�����𴲻�������һ���ֻ�"), \
_T("����Ů�����ҳ���"), \
_T("��� ���������Ǹ�˭"), \
_T("���ڵ���������Щʲô"), \
_T("��ҵ���ĺڿ�����̫��̫��"), \
_T("�Ҳ��������� Ҳ�ᷳ�˳���"), \
_T("ֻ�밲�����ȵĹ��ҵ�����"), \
_T("�ǲ��� ��ҵ���˲���������ļ�ֵ"), \
_T("ֵ��ֵ ���������ʱ ���Ƿ���Ŀ�ʼ"), \
_T("��ʵ ����Ĳ���ʲô����"), \
_T("ֻ��ϲ���ڸ��ֳ�û�ĵط� ������"), \
_T("���� ��Ǯ ���ǰ�׷��"), \
_T("ֻ���зݼ򵥵İ�����㹻"), \
_T("���뵱�� ��ʼ��ڵľ���"), \
_T("̫��Ŀ�������ӵ�����ĵ�"), \
_T("command bash  RAT  XX PC  WEB������"), \
_T("LINUX  BSD ��Щ�������Ƿ�ͬ������"), \
_T("���㼼��ѧ���ٺ�Ҳ���ֲ�������"), \
_T("OH GOODBYE MY BABY"), \
_T("�� ����ѡ���ҵ���"), \
_T("������ ��Ҳ������ڹ�"), \
_T("��֪�����������н��"), \
_T("������������Щʲô"), \
_T("�ڿ���  -  ����С����") };
*/