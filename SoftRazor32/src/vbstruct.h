#pragma once
#include "common.h"

#define PCEC_NOTEVEN          0x01U             //��γ��Ȳ���ż��
#define PCEC_NOBYTE           0x02U             //ָ��ȳ������ֵ
#define PCEC_DECEND           0x00010000U       //�������

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

//VB�ؼ�
typedef struct _VB_CONTROL
{//sizeof() = 0x28  �ṹ��С
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

//VB��ѡ������Ϣ
typedef struct _VB_OPTIONAL_OBJECT_INFO // ����ǿ�ѡ��OBJECT_INFO��PEHEADER���OPTIONAL_HEADER���ƣ��Ƿ���Ҫ��ÿ��Object_t�����ObjectTyper����ĵ����ڶ���λ����ϸ���ϱ�
{   //sizeof() = 0x40
  DWORD       fDesigner;                        // ��������ֵ��2���ʾ��һ��designer
  LPCLSID     pObjectCLSID;                      //ָ��CLSID����
  DWORD       Null1;                            //û���õ���䶫��
  DWORD       aGuidObjectGUI;                    //?????????????????????????????
  DWORD       lObjectDefaultIIDCount;            //  01 00 00 00 ???????????????????????????
  DWORD       pObjectEventsIIDTable;            //ָ�������ΪIID��
  DWORD       lObjectEventsIIDCount;            //������ΪIID����
  DWORD       pObjectDefaultIIDTable;            //ָ��Ĭ�϶���IID��
  DWORD       ControlCount;                      //�ؼ�����
  PVBC        pControlArray;                    //ָ��ؼ���
  WORD        iEventCount;                        // ��Ϊ�ĸ������Ƚ���Ҫ��֪���м�����Ϊ
  WORD        iPCodeCount;                        // PCode����
  WORD        oInitializeEvent;                  // ƫ��,��aEventLinkTableָ���ʼ����Ϊ
  WORD        oTerminateEvent;                  // ƫ��,��aEventLinkTableָ����ֹ��Ϊ
  PVBEL       pEventLinkArray;                  //ָ��ָ��EventLink��ָ��
  DWORD       aBasicClassObject;                // ָ��ָ��һ���ڴ���
  DWORD       Null3;                            //û���õ���䶫��
  DWORD       Flag2;                            //һ�㶼�ǿյ�
} VB_OPTIONAL_OBJECT_INFO, VBOOI, *PVB_OPTIONAL_OBJECT_INFO, *PVBOOI;

//VB������Ϣ
typedef struct _VB_OBJECT_INFO //�������ʾ���OBJECT��Ϣ�Ľṹ,ÿһ��OBJECT����һ��
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
  PDWORD pProcArray;           //ָ����̱�
  WORD iConstantsCount;      // Number of Constants
  WORD iMaxConstants;        // Maximum Constants to allocate.
  DWORD Flag5;
  WORD Flag6;
  WORD Flag7;
  WORD aConstantPool;
  //���VB_OBJECT.ObjectType������,ʣ�µľ��ǿ�ѡ��Ŀ[VB_OPTIONAL_OBJECT_INFO]
} VB_OBJECT_INFO, VBOI, *PVB_OBJECT_INFO, *PVBOI;

//VB����
typedef struct _VB_OBJECT   //�������ÿ��OBJECT�Ľṹ 
{   // sizeof() = 0x30
  PVBOI pObjectInfo;              //VA ָ��һ��ObjectInfo_t����,����ʾ���OBJECT������
  DWORD Const1;                   //û���õ���䶫��
  DWORD aPublicBytes;             //VA ָ���ñ������С
  DWORD aStaticBytes;             //VA ָ��̬�������ַ
  DWORD aModulePublic;            //VA ָ���ñ�����
  DWORD aModuleStatic;            //VA ָ��̬������
  PCHAR aObjectName;              //VA �ַ���,���OBJECT������
  DWORD ProcCount;                //events, funcs, subs(ʱ��\����\����)��Ŀ
  DWORD aProcNamesArray;          //VA һ�㶼��0
  DWORD oStaticVar;               //OFFSET  ��aModuleStaticָ��ľ�̬������ƫ��
  DWORD ObjectType;               //�Ƚ���Ҫ��ʾ�����OBJECT��ʵ��,������±�
  DWORD Null3;                    //û���õ���䶫��
} VB_OBJECT, VBO, *PVB_OBJECT, *PVBO;

//VB�����
typedef struct _VB_OBJECT_TABLE //�����OBJECT ���ܱ����������Ժ��ÿ��OBJECT  sizeof() = 0x54
{
  DWORD lNull1;                                        //û���õ���䶫��
  DWORD aExecProj;                                            //VAָ��һ���ڴ�ṹ(�о������Ȳ�û�������������ʲô�ô�
  DWORD aProjectInfo2;                                        //VAָ��Project Info 2
  DWORD Const1;                                               //û���õ���䶫��
  DWORD Null2;                                                //û���õ���䶫��
  DWORD lpProjectObject;                                      // 0x14
  DWORD Flag1;                                                //��־1
  DWORD Flag2;                                                //��־2
  DWORD Flag3;                                                //��־3
  DWORD Flag4;                                                //��־4
  WORD fCompileType;                                          //Internal flag used during compilation
  WORD ObjectCount1;                                          //OBEJCT����1????
  WORD iCompiledObjects;                                      //�����OBJECT����
  WORD iObjectsInUse;                                         //Updated in the IDE to correspond the total number ' but will go up or down when initializing/unloading modules.
  PVBO pObject;                                               //VAָ���һ��VB_OBJECT�ṹ������Ҫ
  DWORD Null3;                                                //û���õ���䶫��
  DWORD Null4;                                                //û���õ���䶫��
  DWORD Null5;                                                //û���õ���䶫��
  DWORD aProjectName;                                         //ִ�й������ֵ��ַ���
  DWORD LangID1;                                              //language ID1
  DWORD LangID2;                                              //language ID2
  DWORD Null6;                                                //û���õ���䶫��
  DWORD Const3;                                               //û���õ���䶫��
} VB_OBJECT_TABLE, VBOT, *PVB_OBJECT_TABLE, *PVBOT;

//VB�ⲿ��
typedef struct _VB_EXTERNAL_LIBRARY
{
  DWORD pLibraryName;         // VA ָ��  NTS
  DWORD pLibraryFunction;     // VA ָ��  NTS
} VB_EXTERNAL_LIBRARY, VBEXL, *PVB_EXTERNAL_LIBRARY, *PVBEXL;

//VB�ⲿ��
typedef struct _VB_EXTERNAL_TABLE
{
  DWORD Flag;                 // ��־
  PVBEL pExternalLibrary;     // VA ָ��ExternalLibrary�ṹ�ĵ�ַָ��
} VB_EXTERNAL_TABLE, VBEXT, *PVB_EXTERNAL_TABLE, *PVBEXT;

//VB������Ϣ
typedef struct _VB_PROJECT_INFO // sizeof() = 0x23c  
{
  union {
    char    crSign[4];                  // �ṹ��ǩ������,��ħ���ַ�����
    DWORD   dwSign;
  } Sign;
  PVBOT pObjectTable;                     // VA �ṹָ�������б�ĵ�ַָ�루����Ҫ��!��7.����  
  DWORD Null1;                            // ?û���õĶ���  
  DWORD pStartOfCode;                     // VA ���뿪ʼ��,����PEHEAD->EntryPoint���������VB����ʵ�ʵĿ�ʼ��  
  DWORD pEndOfCode;                       //VA ���������  
  DWORD Flag1;                            // ��־1  
  DWORD ThreadSpace;                      // ���̵߳Ŀռ�?????????????????  
  DWORD pVBAExcrptionhandler;             // VA VBA���⴦�������ַָ��  
  DWORD pNativeCode;                      // VA ���ػ����뿪ʼλ�õĵ�ַָ��  
  WORD oProjectLocation;                  // Offset ����λ��?????????????????  
  WORD Flag2;                             // ��־2  
  WORD Flag3;                             // ��־3  
  BYTE OriginalPathName[MAX_PATH * 2];    // ԭ�ļ���ַ,һ���ַ���,�����ΪMAX_PATH  
  BYTE NullSpacer;                        // ���õĶ���,����ռλ��????????????????????  
  PVBET pExternalTable;                   // VA ���ñ��ָ���ַ  
  DWORD ExternalCount;                    // ���ñ��С(����)  
} VB_PROJECT_INFO, VBPI, *PVB_PROJECT_INFO, *PVBPI;

//VB GUI��
typedef struct _VB_GUI_TABLE // sizeof() =  0x50
{
  DWORD lStructSize;      // ����ṹ���ܴ�С
  BYTE uuidObjectGUI[15];  // Object GUI��UUID
  DWORD Unknown1;          // ???????????????????????????????????
  DWORD Unknown2;          // ???????????????????????????????????
  DWORD Unknown3;          // ???????????????????????????????????
  DWORD Unknown4;          // ???????????????????????????????????
  DWORD lObjectID;        // ��ǰ���̵����ID
  DWORD Unknown5;          // ???????????????????????????????????
  DWORD fOLEMisc;          // OLEMisc��־
  BYTE uuidObject[15];    // �����UUID
  DWORD Unknown6;          // ???????????????????????????????????
  DWORD Unknown7;          // ???????????????????????????????????
  DWORD pFormPointer;      // VA ָ��GUI Object Info�ṹ�ĵ�ַָ��
  DWORD Unknown8;          // ???????????????????????????????????
} VB_GUI_TABLE, VBGT, *PVB_GUI_TABLE, *PVBGT;

//VB COMע������
typedef struct _VB_COM_REG_DATA // sizeof() = 0x30
{
  DWORD oRegInfo;                    //Offset ָ��COM Interfaces Info�ṹ��COM�ӿ���Ϣ��
  DWORD oNTSProjectName;            //Offset ָ��Project/Typelib Name����������
  DWORD oNTSHelpDirectory;          //Offset ָ��Help Directory�������ļ�Ŀ¼��
  DWORD oNTSProjectDescription;      //Offset ָ��Project Description������������
  BYTE uuidProjectClsId[15];        //Project/Typelib��CLSID
  DWORD lTlbLcid;                    //Type Library��LCID
  WORD iPadding1;                    //û���õ��ڴ����ռ�1
  WORD iTlbVerMajor;                //Typelib ���汾
  WORD iTlbVerMinor;                //Typelib �ΰ汾
  WORD iPadding2;                    //û���õ��ڴ����ռ�2
  DWORD lPadding3;                  //û���õ��ڴ����ռ�3
} VB_COM_REG_DATA, VBCRD, *PVB_COM_REG_DATA, *PVBCRD;

//VB COMע����Ϣ
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

//VB ��ʼ��ͷ���ṹ
typedef struct _VB_HEADER
{
  union
  {
    char    crSign[4];    // �ĸ��ֽڵ�ǩ�����ţ���PEHEADER����Ǹ�signature���������ʵĶ�����VB�ļ�����"VB5!"
    DWORD   dwSign;
  } Sign;
  WORD RtBuild;          // ����ʱ�����ı��������Ʊ����ʱ�䣩
  BYTE LangDLL[14];      // ����DLL�ļ������֣������0x2A�Ļ��ʹ����ǿջ�����Ĭ�ϵģ�
  BYTE BakLangDLL[14];  // ����DLL�����ļ������֣������0x7F�Ļ��ʹ����ǿջ�����Ĭ�ϵģ��ı����ֵ��EXE�ļ�������û�����ã�
  WORD RtDLLVer;        // ������DLL�ļ��İ汾
  DWORD LangID;          // ���Ե�ID
  DWORD BakLangID;      // �������Ե�ID��ֻ�е�����ID����ʱ���Ŵ��ڣ�
  DWORD pSubMain;        // RVA��ʵ���о�������VA�� sub main���̵ĵ�ַָ�루3.���������0��������EXEʱ��Form�����ļ���ʼ���еģ�
  PVBPI pProjInfo;      // VA ������Ϣ�ĵ�ַָ�룬ָ��һ��VB_PROJECT_INFO�ṹ��2.��
  DWORD fMDLIntObjs;    // ?��ϸ��"MDL �ڲ��齨�ı�־��"
  DWORD fMDLIntObjs2;    // ?��ϸ��"MDL �ڲ��齨�ı�־��"
  DWORD ThreadFlags;    // �̵߳ı�־
  //* ��ǵĶ��壨ThreadFlags��ֵ�ĺ��壩
  //+-------+----------------+--------------------------------------------------------+
  //| ֵ    | ����           | ����                                                   |
  //+-------+----------------+--------------------------------------------------------+
  //|  0x01 | ApartmentModel | �ر𻯵Ķ��߳�ʹ��һ���ֿ���ģ��                        |
  //|  0x02 | RequireLicense | �ر���Ҫ������֤(ֻ��OCX)                             |
  //|  0x04 | Unattended     | �ر𻯵�û��GUIͼ�ν����Ԫ����Ҫ��ʼ��                |
  //|  0x08 | SingleThreaded | �ر𻯵ľ�̬��ʱ���̵߳�                                |
  //|  0x10 | Retained       | �ر𻯵Ľ��ļ��������ڴ���(ֻ��Unattended)               |
  //+-------+----------------+--------------------------------------------------------+
  //ex: �����0x15�ͱ�ʾ��һ�����ж��߳�,�ڴ泣פ,����û��GUIԪ��Ҫ��ʼ��
  DWORD ThreadCount;    // �̸߳���
  WORD FrmCount;        // �������
  WORD pExternalComponentCount;    // VA �ⲿ���ø�������WINSOCK���������
  DWORD ThunkCount;      // ?������ڴ������صĶ���
  PVBGT pGUITable;      // VA GUIԪ�ر�ĵ�ַָ�루ָ��һ��GUITable_t�ṹ��4.�ģ���
  DWORD pExternalComponentTable;    // VA �ⲿ���ñ�ĵ�ַָ��
  //  DWORD pProjDep;      // VA ���̵������ĵ�ַָ�루�����ʵû�У�
  DWORD pComRegData;    // VA COMע�����ݵĵ�ַָ��
  DWORD oProjExename;    // Offset ָ�򹤳�EXE���ֵ��ַ���
  DWORD oProjTitle;      // Offset ָ�򹤳̱�����ַ���
  DWORD oHelpFile;      // Offset ָ������ļ����ַ���
  DWORD oProjName;      // Offset ָ�򹤳������ַ���
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



//* MDL �ڲ��齨�ı�־��
//+---------+------------+---------------+
//|   ID    |    ֵ      | �齨����      |
//+---------+------------+---------------+
//|                           ��һ����־ |
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
//|                          �ڶ�����־  |
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


//Object Type part of tCOMRegInfo//û�з�������壬��û�з��룬ֻ�ǿ�������
//+-------+---------------+-------------------------------------------+
//| Value | Name          | Description                               |
//+-------+---------------+-------------------------------------------+
//|  0x02 | Designer      | A Visual Basic Designer for an Add.in     |
//|  0x10 | Class Module  | A Visual Basic Class                      |
//|  0x20 | User Control  | A Visual Basic ActiveX User Control (OCX) |
//|  0x80 | User Document | A Visual Basic User Document              |
//+-------+---------------+-------------------------------------------+


//Object_t.ObjectTyper ����...//��Ҫ�����Ա���
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
//HasPublicInterface ---+ ||     |  |    | |    |  ���й��õĽӿڣ�
//HasPublicEvents --------+|     |  |    | |    |  ���й��õ��¼���
//IsCreatable/Visible? ----+     |  |    | |    |  ���Ƿ���Դ������ɼ���
//Same as "HasPublicEvents" -----+  |    | |    |  
//[aLfa]                         |  |    | |    |
//usercontrol (1) ---------------+  |    | |    |  ���û����ƣ�
//ocx/dll (1) ----------------------+    | |    |  ��OCX/DLL��
//form (1) ------------------------------+ |    |  ���ǲ���FORM�Ǿ���1��
//vb5 (1) ---------------------------------+    |  ���ǲ���VB5�Ǿ���1��
//HasOptInfo (1) -------------------------------+  ����û�ж������Ϣ��Ϣ�ɾ���1,�����ǲ���ָ��OptionalObjectInfo_t������PEHEAD���Optional��Ϣһ����
//                                              |
//module(0) ------------------------------------+   �������Moduleģ���������0��