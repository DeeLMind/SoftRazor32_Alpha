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
PTCHAR            WN_Module = _T("模块列表");
PTCHAR            WN_Memory = _T("内存映射");
PTCHAR            WN_Heap = _T("进程堆列表");
PTCHAR            WN_Thread = _T("线程列表");
PTCHAR            WN_Disasm = _T("反汇编");
PTCHAR            WN_VB = _T("VB结构分析");
PTCHAR            WN_PCP = _T("P-Code过程");
PTCHAR            CN_MDI = _T("MDIClient");
PTCHAR            CN_Static = _T("Static");
PTCHAR            CN_Edit = _T("Edit");
PTCHAR            CN_Button = _T("Button");
PTCHAR            t_fgf = _T("|");
PTCHAR            sc_ex = _T("可执行");
PTCHAR            sc_or = _T("只读");
PTCHAR            sc_rw = _T("读写");
PTCHAR            sc_sh = _T("共享");
PTCHAR            sc_dc = _T("可丢弃");
PTCHAR            sc_np = _T("不分页");
PTCHAR            sc_rt = _T("重定位");
PTCHAR            sc_id = _T("已初始化数据");
PTCHAR            sc_ud = _T("未初始化数据");
PTCHAR            sc_cd = _T("包含COMDAT");
PTCHAR            t_ref = _T("刷新");
PTCHAR            t_viw = _T("查看");

/*
PTCHAR            lrct[60] = { _T("我 曾经选择我的梦"), \
_T("到现在 我也不曾后悔过"), \
_T("我知道跟她不会有结果"), \
_T("但是我又能做些什么"), \
_T("年少的时候 我有一个梦"), \
_T("成为黑客 成为网络大牛"), \
_T("为了它 我是放弃了很多"), \
_T("学习，友情，爱情 还有朋友"), \
_T("就算这样我也不曾后悔"), \
_T("就算挥霍的时间再也无法追回"), \
_T("就算失去了爱的机会，读书的机会"), \
_T("我也只能虚伪的说着我无所谓"), \
_T("直到现在代价是否太大"), \
_T("人生的关卡 我被不停的冲刷"), \
_T("什么时候才能走上人生的上坡"), \
_T("什么时候才能真正悬崖勒马"), \
_T("对于那个女孩 没有所谓的内疚"), \
_T("曾经的甜蜜 却是目地的借口"), \
_T("我的原因就让它成为理由"), \
_T("请你松开我的手 不要 再回头"), \
_T("回想当初选择这个梦的时候"), \
_T("只是为了忘记学生时期的女友"), \
_T("不知道 她过得怎么样"), \
_T("或许有个更好的男生陪在她身旁"), \
_T("或许在认真学习 或许会迷茫"), \
_T("只希望她做个快乐公主或者新娘"), \
_T("到底有谁真的在乎我"), \
_T("我 曾经选择我的梦"), \
_T("到现在 我也不曾后悔过"), \
_T("不知道会有怎样的结果"), \
_T("只希望都能过得快乐"), \
_T("有谁在乎我 或许我已经引导了寂寞"), \
_T("替换了沉没 format也删不掉网络的垃圾"), \
_T("就让我潜水 假如孤独也一种美"), \
_T("就让我颓废 最后喝掉这一杯"), \
_T("装满美梦虚幻的cocktail"), \
_T("明天起床或许又是一个轮回"), \
_T("重生女神让我沉醉"), \
_T("最好 让我忘记那个谁"), \
_T("现在的我总想做些什么"), \
_T("商业化的黑客总是太多太多"), \
_T("我不想在罗嗦 也厌烦了炒作"), \
_T("只想安安稳稳的过我的生活"), \
_T("是不是 商业化了才能体现你的价值"), \
_T("值不值 当你做错的时 就是犯规的开始"), \
_T("其实 俺真的不是什么高手"), \
_T("只是喜欢在高手出没的地方 到处溜"), \
_T("出名 金钱 不是俺追求"), \
_T("只想有份简单的爱情就足够"), \
_T("回想当初 开始玩黑的经历"), \
_T("太多的快乐辛酸拥都上心底"), \
_T("command bash  RAT  XX PC  WEB服务器"), \
_T("LINUX  BSD 这些东西你是否同样经历"), \
_T("就算技术学得再好也入侵不了你心"), \
_T("OH GOODBYE MY BABY"), \
_T("我 曾经选择我的梦"), \
_T("到现在 我也不曾后悔过"), \
_T("我知道跟她不会有结果"), \
_T("但是我又能做些什么"), \
_T("黑客梦  -  晚上小飞贼") };
*/