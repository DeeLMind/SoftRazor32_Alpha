#pragma once
#include "common.h"

#define PCEC_NOTEVEN          0x01U             //变参长度不是偶数
#define PCEC_NOBYTE           0x02U             //指令长度超过最大值
#define PCEC_DECEND           0x00010000U       //解码完成

typedef struct _VB_PROC_TABLE_INFO
{
  long                  data0;
  long                  Record;
  long                  data8;
  long                  data0C;
  long                  data10;
  long                  Owner;
  long                  rtl;
  long                  data1C;
  long                  data20;
  long                  data24;
  short                 JmpCnt;
  short                 data2A;
  long                  data2C;
  long                  data30;
  long                  ConstPool;
} VB_PROC_TABLE_INFO, VBPTI, *PVBPTI;

typedef struct _VB_PROC_DESC_INFO
{
  PVBPTI              pTableInfo;
  short               field_4;
  USHORT              FrameSize;
  USHORT              ProcSize;
  short               field_A;
  short               field_C;
  short               field_E;
  short               field_10;
  short               field_12;
  short               field_14;
  short               field_16;
  short               field_18;
  short               field_1A;
  short               Flag;
} VB_PROC_DESC_INFO, VBPDI, *PVBPDI;

#pragma pack(push,1)
typedef struct _VB_EVENT_POINTER
{//sizeof() = 0x0E
  BYTE        Const1;
  DWORD       Flag1;
  DWORD       Const2;
  BYTE        Const3;
  PVBPDI      pDescInfo;
} VB_EVENT_POINTER, VBEP, *PVB_EVENT_POINTER, *PVBEP, **PPVBEP;
#pragma pack(pop)

typedef struct _VB_EVENT_TABLE
{//sizeof() = 0x18
  DWORD                       Null1;
  struct _VB_CONTROL        * pControl;
  struct _VB_OBJECT_INFO    * pObjectInfo;
  DWORD                       aQueryInterface;
  DWORD                       aAddRef;
  DWORD                       aRelease;
//PVBEP     pEventPointer[aControl.EventCount - 1]
} VB_EVENT_TABLE, VBET, *PVB_EVENT_TABLE, *PVBET;

typedef struct _VB_EVENT_LINK
{//sizeof() = 0x0D
  WORD    Const1;
  BYTE    CompileType;
  PVBET   pEvent;
  BYTE    PushCmd;
  DWORD   PushAddress;
  BYTE    Const;
} VB_EVENT_LINK, VBEL, *PVB_EVENT_LINK, *PVBEL;

//VB控件
typedef struct _VB_CONTROL
{//sizeof() = 0x28  结构大小
  WORD        Flag1;             //Integer  ' 0x00
  WORD        EventCount;        //Integer  ' 0x02
  DWORD       Flag2;            //Long     ' 0x04
  LPGUID      pGUID;            //Long     ' 0x08
  WORD        Index;             //Integer  ' 0x0C
  WORD        Const1;            //Integer  ' 0x0E
  DWORD       Null1;            //Long     ' 0x10
  DWORD       Null2;            //Long     ' 0x14
  PVBET       pEventTable;      //Long     ' 0x18
  BYTE        Flag3;             //Byte     ' 0x1C
  BYTE        Const2;            //Byte     ' 0x1D
  WORD        Const3;            //Integer  ' 0x1E
  PCHAR       pName;            //Long     ' 0x20
  WORD        Index2;            //Integer  ' 0x24
  WORD        Const1Copy;        //Integer  ' 0x26
} VB_CONTROL, VBC, *PVB_CONTROL, *PVBC;

//VB可选对象信息
typedef struct _VB_OPTIONAL_OBJECT_INFO // 这个是可选的OBJECT_INFO和PEHEADER里的OPTIONAL_HEADER类似，是否有要看每个Object_t里面的ObjectTyper表里的倒数第二个位（详细看上表）
{   //sizeof() = 0x40
  DWORD       fDesigner;                        // 如果这个数值是2则表示是一个designer
  LPCLSID     pObjectCLSID;                      //指向CLSID对象
  DWORD       Null1;                            //没有用的填充东西
  DWORD       aGuidObjectGUI;                    //?????????????????????????????
  DWORD       lObjectDefaultIIDCount;            //  01 00 00 00 ???????????????????????????
  DWORD       pObjectEventsIIDTable;            //指向对象行为IID表
  DWORD       lObjectEventsIIDCount;            //对象行为IID个数
  DWORD       pObjectDefaultIIDTable;            //指向默认对象IID表
  DWORD       ControlCount;                      //控件个数
  PVBC        pControlArray;                    //指向控件表
  WORD        iEventCount;                        // 行为的个数，比较重要，知道有几个行为
  WORD        iPCodeCount;                        // PCode个数
  WORD        oInitializeEvent;                  // 偏移,从aEventLinkTable指向初始化行为
  WORD        oTerminateEvent;                  // 偏移,从aEventLinkTable指向终止行为
  PVBEL       pEventLinkArray;                  //指针指向EventLink的指针
  DWORD       aBasicClassObject;                // 指针指向一个内存中
  DWORD       Null3;                            //没有用的填充东西
  DWORD       Flag2;                            //一般都是空的
} VB_OPTIONAL_OBJECT_INFO, VBOOI, *PVB_OPTIONAL_OBJECT_INFO, *PVBOOI;

//VB对象信息
typedef struct _VB_OBJECT_INFO //这个是显示这个OBJECT信息的结构,每一个OBJECT都有一个
{   // sizeof() =  0x38
  WORD Flag1;
  WORD ObjectIndex;
  DWORD aObjectTable;
  DWORD Null1;
  DWORD aSmallRecord;        // when it is a module this value is -1 [better name?]
  DWORD Const1;
  DWORD Null2;
  DWORD aObject;
  DWORD RunTimeLoaded;        //[can someone verify this?]
  DWORD NumberOfProcs;
  PDWORD pProcArray;           //指向过程表
  WORD iConstantsCount;      // Number of Constants
  WORD iMaxConstants;        // Maximum Constants to allocate.
  DWORD Flag5;
  WORD Flag6;
  WORD Flag7;
  WORD aConstantPool;
  //如果VB_OBJECT.ObjectType声明了,剩下的就是可选项目[VB_OPTIONAL_OBJECT_INFO]
} VB_OBJECT_INFO, VBOI, *PVB_OBJECT_INFO, *PVBOI;

//VB对象
typedef struct _VB_OBJECT   //这个就是每个OBJECT的结构 
{   // sizeof() = 0x30
  PVBOI pObjectInfo;              //VA 指向一个ObjectInfo_t类型,来显示这个OBJECT的数据
  DWORD Const1;                   //没有用的填充东西
  DWORD aPublicBytes;             //VA 指向公用变量表大小
  DWORD aStaticBytes;             //VA 指向静态变量表地址
  DWORD aModulePublic;            //VA 指向公用变量表
  DWORD aModuleStatic;            //VA 指向静态变量表
  PCHAR aObjectName;              //VA 字符串,这个OBJECT的名字
  DWORD ProcCount;                //events, funcs, subs(时间\函数\过程)数目
  DWORD aProcNamesArray;          //VA 一般都是0
  DWORD oStaticVar;               //OFFSET  从aModuleStatic指向的静态变量表偏移
  DWORD ObjectType;               //比较重要显示了这个OBJECT的实行,具体见下表
  DWORD Null3;                    //没有用的填充东西
} VB_OBJECT, VBO, *PVB_OBJECT, *PVBO;

//VB对象表
typedef struct _VB_OBJECT_TABLE //这个是OBJECT 的总表，可以索引以后的每个OBJECT  sizeof() = 0x54
{
  DWORD lNull1;                                        //没有用的填充东西
  DWORD aExecProj;                                            //VA指向一块内存结构(研究下来既不没见着这个东西由什么用处
  DWORD aProjectInfo2;                                        //VA指向Project Info 2
  DWORD Const1;                                               //没有用的填充东西
  DWORD Null2;                                                //没有用的填充东西
  DWORD lpProjectObject;                                      // 0x14
  DWORD Flag1;                                                //标志1
  DWORD Flag2;                                                //标志2
  DWORD Flag3;                                                //标志3
  DWORD Flag4;                                                //标志4
  WORD fCompileType;                                          //Internal flag used during compilation
  WORD ObjectCount1;                                          //OBEJCT数量1????
  WORD iCompiledObjects;                                      //编译后OBJECT数量
  WORD iObjectsInUse;                                         //Updated in the IDE to correspond the total number ' but will go up or down when initializing/unloading modules.
  PVBO pObject;                                               //VA指向第一个VB_OBJECT结构，很重要
  DWORD Null3;                                                //没有用的填充东西
  DWORD Null4;                                                //没有用的填充东西
  DWORD Null5;                                                //没有用的填充东西
  DWORD aProjectName;                                         //执行工程名字的字符串
  DWORD LangID1;                                              //language ID1
  DWORD LangID2;                                              //language ID2
  DWORD Null6;                                                //没有用的填充东西
  DWORD Const3;                                               //没有用的填充东西
} VB_OBJECT_TABLE, VBOT, *PVB_OBJECT_TABLE, *PVBOT;

//VB外部库
typedef struct _VB_EXTERNAL_LIBRARY
{
  DWORD pLibraryName;         // VA 指向  NTS
  DWORD pLibraryFunction;     // VA 指向  NTS
} VB_EXTERNAL_LIBRARY, VBEXL, *PVB_EXTERNAL_LIBRARY, *PVBEXL;

//VB外部表
typedef struct _VB_EXTERNAL_TABLE
{
  DWORD Flag;                 // 标志
  PVBEL pExternalLibrary;     // VA 指向ExternalLibrary结构的地址指针
} VB_EXTERNAL_TABLE, VBEXT, *PVB_EXTERNAL_TABLE, *PVBEXT;

//VB工程信息
typedef struct _VB_PROJECT_INFO // sizeof() = 0x23c  
{
  union {
    char    crSign[4];                  // 结构的签名特性,和魔术字符类似
    DWORD   dwSign;
  } Sign;
  PVBOT pObjectTable;                     // VA 结构指向的组件列表的地址指针（很重要的!（7.））  
  DWORD Null1;                            // ?没有用的东西  
  DWORD pStartOfCode;                     // VA 代码开始点,类似PEHEAD->EntryPoint这里告诉了VB代码实际的开始点  
  DWORD pEndOfCode;                       //VA 代码结束点  
  DWORD Flag1;                            // 标志1  
  DWORD ThreadSpace;                      // 多线程的空间?????????????????  
  DWORD pVBAExcrptionhandler;             // VA VBA意外处理机器地址指针  
  DWORD pNativeCode;                      // VA 本地机器码开始位置的地址指针  
  WORD oProjectLocation;                  // Offset 工程位置?????????????????  
  WORD Flag2;                             // 标志2  
  WORD Flag3;                             // 标志3  
  BYTE OriginalPathName[MAX_PATH * 2];    // 原文件地址,一个字符串,长度最长为MAX_PATH  
  BYTE NullSpacer;                        // 无用的东西,用来占位置????????????????????  
  PVBET pExternalTable;                   // VA 引用表的指针地址  
  DWORD ExternalCount;                    // 引用表大小(个数)  
} VB_PROJECT_INFO, VBPI, *PVB_PROJECT_INFO, *PVBPI;

//VB GUI表
typedef struct _VB_GUI_TABLE // sizeof() =  0x50
{
  DWORD lStructSize;      // 这个结构的总大小
  BYTE uuidObjectGUI[15];  // Object GUI的UUID
  DWORD Unknown1;          // ???????????????????????????????????
  DWORD Unknown2;          // ???????????????????????????????????
  DWORD Unknown3;          // ???????????????????????????????????
  DWORD Unknown4;          // ???????????????????????????????????
  DWORD lObjectID;        // 当前工程的组件ID
  DWORD Unknown5;          // ???????????????????????????????????
  DWORD fOLEMisc;          // OLEMisc标志
  BYTE uuidObject[15];    // 组件的UUID
  DWORD Unknown6;          // ???????????????????????????????????
  DWORD Unknown7;          // ???????????????????????????????????
  DWORD pFormPointer;      // VA 指向GUI Object Info结构的地址指针
  DWORD Unknown8;          // ???????????????????????????????????
} VB_GUI_TABLE, VBGT, *PVB_GUI_TABLE, *PVBGT;

//VB COM注册数据
typedef struct _VB_COM_REG_DATA // sizeof() = 0x30
{
  DWORD oRegInfo;                    //Offset 指向COM Interfaces Info结构（COM接口信息）
  DWORD oNTSProjectName;            //Offset 指向Project/Typelib Name（工程名）
  DWORD oNTSHelpDirectory;          //Offset 指向Help Directory（帮助文件目录）
  DWORD oNTSProjectDescription;      //Offset 指向Project Description（工程描述）
  BYTE uuidProjectClsId[15];        //Project/Typelib的CLSID
  DWORD lTlbLcid;                    //Type Library的LCID
  WORD iPadding1;                    //没有用的内存对齐空间1
  WORD iTlbVerMajor;                //Typelib 主版本
  WORD iTlbVerMinor;                //Typelib 次版本
  WORD iPadding2;                    //没有用的内存对齐空间2
  DWORD lPadding3;                  //没有用的内存对齐空间3
} VB_COM_REG_DATA, VBCRD, *PVB_COM_REG_DATA, *PVBCRD;

//VB COM注册信息
typedef struct _VB_COM_REG_INFO // sizeof() = 0x44
{
  DWORD oNextObject;            //Offset to COM Interfaces Info
  DWORD oObjectName;            //Offset to Object Name
  DWORD oObjectDescription;      //Offset to Object Description
  DWORD lInstancing;            //Instancing Mode
  DWORD lObjectID;              //Current Object ID in the Project
  BYTE uuidObjectClsID[15];      //CLSID of Object
  DWORD fIsInterface;            // Specifies if the next CLSID is valid
  DWORD oObjectClsID;            // Offset to CLSID of Object Interface
  DWORD oControlClsID;          // Offset to CLSID of Control Interface
  DWORD fIsControl;              // Specifies if the CLSID above is valid
  DWORD lMiscStatus;            // OLEMISC Flags (see MSDN docs)
  BYTE fClassType;              // Class Type
  BYTE fObjectType;              // Flag identifying the Object Type
  WORD iToolboxBitmap32;        // Control Bitmap ID in Toolbox
  WORD iDefaultIcon;            // Minimized Icon of Control Window
  WORD fIsDesigner;              // Specifies whether this is a Designer
  DWORD oDesignerData;          // Offset to Designer Data
} VB_COM_REG_INFO, VBCRI, *PVB_COM_REG_INFO, *PVBCRI;

//VB 初始化头部结构
typedef struct _VB_HEADER
{
  union
  {
    char    crSign[4];    // 四个字节的签名符号，和PEHEADER里的那个signature是类似性质的东西，VB文件都是"VB5!"
    DWORD   dwSign;
  } Sign;
  WORD RtBuild;          // 运行时创立的变量（类似编译的时间）
  BYTE LangDLL[14];      // 语言DLL文件的名字（如果是0x2A的话就代表是空或者是默认的）
  BYTE BakLangDLL[14];  // 备份DLL语言文件的名字（如果是0x7F的话就代表是空或者是默认的，改变这个值堆EXE文件的运行没有作用）
  WORD RtDLLVer;        // 运行是DLL文件的版本
  DWORD LangID;          // 语言的ID
  DWORD BakLangID;      // 备份语言的ID（只有当语言ID存在时它才存在）
  DWORD pSubMain;        // RVA（实际研究下来是VA） sub main过程的地址指针（3.）（如果是0则代表这个EXE时从Form窗体文件开始运行的）
  PVBPI pProjInfo;      // VA 工程信息的地址指针，指向一个VB_PROJECT_INFO结构（2.）
  DWORD fMDLIntObjs;    // ?详细见"MDL 内部组建的标志表"
  DWORD fMDLIntObjs2;    // ?详细见"MDL 内部组建的标志表"
  DWORD ThreadFlags;    // 线程的标志
  //* 标记的定义（ThreadFlags数值的含义）
  //+-------+----------------+--------------------------------------------------------+
  //| 值    | 名字           | 描述                                                   |
  //+-------+----------------+--------------------------------------------------------+
  //|  0x01 | ApartmentModel | 特别化的多线程使用一个分开的模型                        |
  //|  0x02 | RequireLicense | 特别化需要进行认证(只对OCX)                             |
  //|  0x04 | Unattended     | 特别化的没有GUI图形界面的元素需要初始化                |
  //|  0x08 | SingleThreaded | 特别化的静态区时单线程的                                |
  //|  0x10 | Retained       | 特别化的将文件保存在内存中(只对Unattended)               |
  //+-------+----------------+--------------------------------------------------------+
  //ex: 如果是0x15就表示是一个既有多线程,内存常驻,并且没有GUI元素要初始化
  DWORD ThreadCount;    // 线程个数
  WORD FrmCount;        // 窗体个数
  WORD pExternalComponentCount;    // VA 外部引用个数例如WINSOCK组件的引用
  DWORD ThunkCount;      // ?大概是内存对齐相关的东西
  PVBGT pGUITable;      // VA GUI元素表的地址指针（指向一个GUITable_t结构（4.四））
  DWORD pExternalComponentTable;    // VA 外部引用表的地址指针
  //  DWORD pProjDep;      // VA 工程的描述的地址指针（这个其实没有）
  DWORD pComRegData;    // VA COM注册数据的地址指针
  DWORD oProjExename;    // Offset 指向工程EXE名字的字符串
  DWORD oProjTitle;      // Offset 指向工程标题的字符串
  DWORD oHelpFile;      // Offset 指向帮助文件的字符串
  DWORD oProjName;      // Offset 指向工程名的字符串
} VB_HEADER, VBHD, *PVB_HEADER, *PVBHD;

/* other */
union PIFLAG
{
  BYTE          Byte[4];
  WORD          Word[2];
  DWORD         DWord;
};

typedef struct _VB_PCODE_STD_INST
{
  const char      * InstText;
  const PTCHAR      ParamText;
  UINT              uType;
  signed short int  InstSize;
} VB_PCODE_STD_INST, VBPCSINS, *PVBPCSINS;

typedef struct _VB_PCODE_LEAD_INST
{
  const char      * InstText;
  UINT              uType;
  signed char       InstSize;
} VB_PCODE_LEAD_INST, VBPCLINS, *PVBPCLINS, **PPVBPCLINS;


/*
typedef struct _VB_PCODE_LEAD_INST
{
  const char      * InstText;
  PIFLAG            pif;
  signed char       InstSize;
  BYTE              Opcode;
} VB_PCODE_LEAD_INST, VBPCLINS, *PVBPCLINS;
*/



//* MDL 内部组建的标志表
//+---------+------------+---------------+
//|   ID    |    值      | 组建名称      |
//+---------+------------+---------------+
//|                           第一个标志 |
//+---------+------------+---------------+
//|    0x00 | 0x00000001 | PictureBox    |
//|    0x01 | 0x00000002 | Label         |
//|    0x02 | 0x00000004 | TextBox       |
//|    0x03 | 0x00000008 | Frame         |
//|    0x04 | 0x00000010 | CommandButton |
//|    0x05 | 0x00000020 | CheckBox      |
//|    0x06 | 0x00000040 | OptionButton  |
//|    0x07 | 0x00000080 | ComboBox      |
//|    0x08 | 0x00000100 | ListBox       |
//|    0x09 | 0x00000200 | HScrollBar    |
//|    0x0A | 0x00000400 | VScrollBar    |
//|    0x0B | 0x00000800 | Timer         |
//|    0x0C | 0x00001000 | Print         |
//|    0x0D | 0x00002000 | Form          |
//|    0x0E | 0x00004000 | Screen        |
//|    0x0F | 0x00008000 | Clipboard     |
//|    0x10 | 0x00010000 | Drive         |
//|    0x11 | 0x00020000 | Dir           |
//|    0x12 | 0x00040000 | FileListBox   |
//|    0x13 | 0x00080000 | Menu          |
//|    0x14 | 0x00100000 | MDIForm       |
//|    0x15 | 0x00200000 | App           |
//|    0x16 | 0x00400000 | Shape         |
//|    0x17 | 0x00800000 | Line          |
//|    0x18 | 0x01000000 | Image         |
//|    0x19 | 0x02000000 | Unsupported   |
//|    0x1A | 0x04000000 | Unsupported   |
//|    0x1B | 0x08000000 | Unsupported   |
//|    0x1C | 0x10000000 | Unsupported   |
//|    0x1D | 0x20000000 | Unsupported   |
//|    0x1E | 0x40000000 | Unsupported   |
//|    0x1F | 0x80000000 | Unsupported   |
//+---------+------------+---------------+
//|                          第二个标志  |
//+---------+------------+---------------+
//|    0x20 | 0x00000001 | Unsupported   |
//|    0x21 | 0x00000002 | Unsupported   |
//|    0x22 | 0x00000004 | Unsupported   |
//|    0x23 | 0x00000008 | Unsupported   |
//|    0x24 | 0x00000010 | Unsupported   |
//|    0x25 | 0x00000020 | DataQuery     |
//|    0x26 | 0x00000040 | OLE           |
//|    0x27 | 0x00000080 | Unsupported   |
//|    0x28 | 0x00000100 | UserControl   |
//|    0x29 | 0x00000200 | PropertyPage  |
//|    0x2A | 0x00000400 | Document      |
//|    0x2B | 0x00000800 | Unsupported   |
//+---------+------------+---------------+


//Object Type part of tCOMRegInfo//没有翻译的意义，就没有翻译，只是看看而已
//+-------+---------------+-------------------------------------------+
//| Value | Name          | Description                               |
//+-------+---------------+-------------------------------------------+
//|  0x02 | Designer      | A Visual Basic Designer for an Add.in     |
//|  0x10 | Class Module  | A Visual Basic Class                      |
//|  0x20 | User Control  | A Visual Basic ActiveX User Control (OCX) |
//|  0x80 | User Document | A Visual Basic User Document              |
//+-------+---------------+-------------------------------------------+


//Object_t.ObjectTyper 属性...//重要的属性表部分
//#########################################################
//form:              0000 0001 1000 0000 1000 0011 --> 18083
//                   0000 0001 1000 0000 1010 0011 --> 180A3
//                   0000 0001 1000 0000 1100 0011 --> 180C3
//module:            0000 0001 1000 0000 0000 0001 --> 18001
//                   0000 0001 1000 0000 0010 0001 --> 18021
//class:             0001 0001 1000 0000 0000 0011 --> 118003
//                   0001 0011 1000 0000 0000 0011 --> 138003
//                   0000 0001 1000 0000 0010 0011 --> 18023
//                   0000 0001 1000 1000 0000 0011 --> 18803
//                   0001 0001 1000 1000 0000 0011 --> 118803
//usercontrol:       0001 1101 1010 0000 0000 0011 --> 1DA003
//                   0001 1101 1010 0000 0010 0011 --> 1DA023
//                   0001 1101 1010 1000 0000 0011 --> 1DA803
//propertypage:      0001 0101 1000 0000 0000 0011 --> 158003
//                      | ||     |  |    | |    |
//[moog]                | ||     |  |    | |    |
//HasPublicInterface ---+ ||     |  |    | |    |  （有公用的接口）
//HasPublicEvents --------+|     |  |    | |    |  （有公用的事件）
//IsCreatable/Visible? ----+     |  |    | |    |  （是否可以创建，可见）
//Same as "HasPublicEvents" -----+  |    | |    |  
//[aLfa]                         |  |    | |    |
//usercontrol (1) ---------------+  |    | |    |  （用户控制）
//ocx/dll (1) ----------------------+    | |    |  （OCX/DLL）
//form (1) ------------------------------+ |    |  （是不是FORM是就是1）
//vb5 (1) ---------------------------------+    |  （是不是VB5是就是1）
//HasOptInfo (1) -------------------------------+  （有没有额外的信息信息由就是1,决定是不是指向OptionalObjectInfo_t类似与PEHEAD里的Optional信息一样）
//                                              |
//module(0) ------------------------------------+   （如果是Module模块就这里是0）