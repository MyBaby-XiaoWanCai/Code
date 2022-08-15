/*
 * Protect_Global.h
 *
 *  Created on: 2021��6��21��
 *      Author: JC
 */

#ifndef APP_INCLUDE_PROTECT_GLOBAL_H_
#define APP_INCLUDE_PROTECT_GLOBAL_H_
#define cStepGreater                0x01
#define cStepLess                   0x02

#define cProtectCCFault             0x00000002
#define cPowerVoltFault             0x00400000

typedef struct struct_Protect_Value
{
    Uint16 Protect_EN;                  //���ϼ��ʹ�ܿ���
    Uint16 Num_Change_Mask;             //����ת��������ʱ��
    Uint16 Cnt_Change_Mask;             //����ת��������ʱ�������
    Uint16 Num_Check;                   //������λ�жϴ���
    Uint16 Cnt_Check;                   //������λ�жϴ���������
    Uint16 Equation;                    //�Ƚ��жϹ�ϵ
     int32 Now_Value;                   //���Ƚ�����ǰֵ
     int32 Ref_Value;                   //�Ƚ�����
    Uint32 Mask;                        //���ϱ�־λ������
    Uint32 *Ptr;                        //��Ҫ��λ�ı���ָ��
} struct_Protect_Value;

typedef struct struct_Protect_Global
{
    struct_Protect_Value CH_Over_Cur;

    struct_Protect_Value CH_Over_Vol_Out;

} struct_Protect_Global;
//�ṹ�嶨��
typedef struct struct_Fault_Code
{
    Uint32 Err_Global;
    Uint32 Err_Global_Bak;
    Uint32 Err_Step;
    Uint32 Err_Step_Bak;
    Uint32 Alarm;
    Uint32 Alarm_Bak;
    Uint16 Code;
} struct_Fault_Code;
//���ⲿ�ļ�����ȫ�ֱ�������
extern struct_Protect_Global Protect_Global[];
extern struct_Fault_Code  Fault_Code[];


#endif /* APP_INCLUDE_PROTECT_GLOBAL_H_ */
