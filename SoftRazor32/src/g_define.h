#pragma once

#include "common.h"

//Global Define

#define MY_MAGIC            0x3940A512
#define VB_MAGIC            0x21354256

#define NVE_TEXT            _T("����: Soft Razor �޷��ڵ�ǰϵͳ������!\n\n���ϵͳ�汾(NT�汾):\n�ͻ���: Win XP        �����: WinServer 2003")
#define ERR_TITLE           _T("����")
#define DEF_VCHILD          WS_CHILD | WS_VISIBLE
#define DEF_WINDOW          WS_CHILD | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
#define LV_STYLE            DEF_VCHILD | LVS_REPORT
#define DEF_BTNSTYLE        DEF_VCHILD | BS_USERBUTTON
#define TV_STYLE            DEF_VCHILD | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES | TVS_SHOWSELALWAYS

#define VBPC_NULLITEM       0
#define VBPC_STRUCT         1
#define VBPC_OBJECT         2
#define VBPC_ROOTFORM       3
#define VBPC_FORM           4
#define VBPC_FRMCTL         5
#define VBPC_BAS            6
#define VBPC_CLS            7
#define VBPC_UC             8
#define VBPC_UCCTL          9

#define VBII_NULLITEM       0
#define VBII_VBH            1
#define VBII_PRJI           2
#define VBII_COMD           3
#define VBII_COMI           4
#define VBII_OBJTAB         5
#define VBII_OBJS           6

#define DefWidth            76

#define HEXFORMAT           _T("%08X")
#define FT_TAH              _T("Tahoma")
#define FT_FIX              _T("Fixedsys")
#define T_NTDLL             _T("ntdll.dll")

#define T_NULL              _T("\0\0")
#define T_UNKN1             _T("?")
#define T_UNKN2             _T("??")
#define T_UNKN4             _T("????")
#define T_UNKN8             _T("????????")
#define T_UNKNOWN           _T("δ֪")
#define T_OTHER             _T("����")
#define T_COMMIT            _T("�ѷ���")
#define T_RESERVE           _T("����")

#define T_FILEHEADER        _T(".[Header]")

#define T_M_IMAGE           _T("{ӳ��}")
#define T_M_MAPPED          _T("[ӳ��]")
#define T_M_PRIVATE         _T("(˽��)")

#define T_P_NOACCESS        _T("��ֹ")
#define T_P_READONLY        _T("ֻ��")
#define T_P_READWRITE       _T("��д")
#define T_P_WRITECOPY       _T("������")
#define T_P_EXECUTE         _T("��ִ��")
#define T_P_EXECUTE_R       _T("ֻ��ִ��")
#define T_P_EXECUTE_RW      _T("��дִ��")
#define T_P_EXECUTE_WC      _T("����ִ��")
#define T_P_GUARD           _T("�����쳣")
#define T_P_NOCACHE         _T("��ֹ����")
#define T_P_WRITECOMBINE    _T("���д��")

#define T_H_DEFAULT         _T("����Ĭ�϶�")
#define T_H_SHARED          _T("�����")

#define T_HB_FIXED          _T("�ڴ�鲻���ƶ�")
#define T_HB_FREE           _T("�ڴ��δʹ��")
#define T_HB_MOVEABLE       _T("�ڴ����ƶ�")

#define T_TP_AN             _T("���ڱ�׼")
#define T_TP_BN             _T("���ڱ�׼")
#define T_TP_HE             _T("���")
#define T_TP_IDLE           _T("����")
#define T_TP_LE             _T("���")
#define T_TP_NM             _T("��׼")
#define T_TP_TC             _T("ʵʱ")

#define T_TS_RUN            _T("��������")
#define T_TS_SP             _T("�ѹ���")
#define T_TS_WAIT           _T("�ȴ�����")
#define T_TS_TIM            _T("����ֹ")

#define T_DBGS_BUSY         _T("Soft Razor æµ��,��ȴ����̽���...")