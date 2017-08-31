#pragma once
#define __UD_STANDALONE__

/*����ָ������*/
#define UDCA_Null             0x0000    //��
#define UDCA_Ignore           0x0001    //�����Ե�
#define UDCA_Inst             0x0002    //һ��ָ�� (mov add xor)
#define UDCA_StackOp          0x0003    //ջ����ָ�� (push pop)
#define UDCA_FMSOp            0x0004    //FPU MMX SSE �Ĵ������ָ��
#define UDCA_CJump            0x0005    //������ת
#define UDCA_Jump             0x0006    //��ת
#define UDCA_Call             0x0007    //����
#define UDCA_Ret              0x0008    //����
#define UDCA_PrivInst         0x0009    //��Ȩָ��
#define UDCA_Prefix           0x000A    //ǰ׺
#define UDCA_DefClr           0x000B    //Ĭ����ɫ
#define UDCA_Int              0x000C    //INT�ж�
#define UDCA_Data             0x000D    //����
#define UDCA_Text             0x000E    //�ı�
#define UDCA_Symbol           0x000F    //��������
#define UDCA_Error            0x0010    //�Ƿ��������ָ��
#define UDCA_GPRs             0x0011    //ͨ�üĴ���
#define UDCA_FSRs             0x0012    //FPU SSE �Ĵ���
#define UDCA_SSRs             0x0013    //�Ρ�ϵͳ �Ĵ���
#define UDCA_MStack           0x0014    //ջ�ڴ��ַ
#define UDCA_MOther           0x0015    //�����ڴ��ַ
#define UDCA_IMM              0x0016    //������
#define UDCA_IMMAddr          0x0017    //��ַ������
#define UDCA_Change           0x0020    //���޸ĵ�