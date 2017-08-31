#pragma once

#include "common.h"

//Global Define

#define MY_MAGIC            0x3940A512
#define VB_MAGIC            0x21354256

#define NVE_TEXT            _T("警告: Soft Razor 无法在当前系统中运行!\n\n最低系统版本(NT版本):\n客户端: Win XP        服务端: WinServer 2003")
#define ERR_TITLE           _T("错误")
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
#define T_UNKNOWN           _T("未知")
#define T_OTHER             _T("其它")
#define T_COMMIT            _T("已分配")
#define T_RESERVE           _T("保留")

#define T_FILEHEADER        _T(".[Header]")

#define T_M_IMAGE           _T("{映像}")
#define T_M_MAPPED          _T("[映射]")
#define T_M_PRIVATE         _T("(私有)")

#define T_P_NOACCESS        _T("禁止")
#define T_P_READONLY        _T("只读")
#define T_P_READWRITE       _T("读写")
#define T_P_WRITECOPY       _T("仅共享")
#define T_P_EXECUTE         _T("仅执行")
#define T_P_EXECUTE_R       _T("只读执行")
#define T_P_EXECUTE_RW      _T("读写执行")
#define T_P_EXECUTE_WC      _T("共享执行")
#define T_P_GUARD           _T("初次异常")
#define T_P_NOCACHE         _T("禁止缓存")
#define T_P_WRITECOMBINE    _T("组合写入")

#define T_H_DEFAULT         _T("进程默认堆")
#define T_H_SHARED          _T("共享堆")

#define T_HB_FIXED          _T("内存块不可移动")
#define T_HB_FREE           _T("内存块未使用")
#define T_HB_MOVEABLE       _T("内存块可移动")

#define T_TP_AN             _T("高于标准")
#define T_TP_BN             _T("低于标准")
#define T_TP_HE             _T("最高")
#define T_TP_IDLE           _T("空闲")
#define T_TP_LE             _T("最低")
#define T_TP_NM             _T("标准")
#define T_TP_TC             _T("实时")

#define T_TS_RUN            _T("正在运行")
#define T_TS_SP             _T("已挂起")
#define T_TS_WAIT           _T("等待调度")
#define T_TS_TIM            _T("已终止")

#define T_DBGS_BUSY         _T("Soft Razor 忙碌中,请等待过程结束...")