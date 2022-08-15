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


#define cProtectCLFault             0x00000001   //��е��������������
#define cProtectCCFault             0x00000002   //�����������
#define cProtectCVFault             0x00000004   //�����ѹ����
#define cBusVoltHighFault           0x00000008   //����BUS�߹���
#define cBusVoltLowFault            0x00000010   //����BUSǷѹ����
#define cHardwareError              0x00000020   //Ӳ������
#define cFanFault                   0x00000040   //���ȹ���
#define cWatchdogFault              0x00000080   //���Ź�����
#define cBatPNFault                 0x00000100   //���ӹ���
#define cPowerVoltFault             0x00000200   //����˹�ѹ

#define Code_ProtectCL              0x01
#define Code_ProtectCC              0x02
#define Code_ProtectCV              0x03
#define Code_Protect_BusVoltHigh    0x04
#define Code_Protect_BusVoltLow     0x05
#define Code_Protect_HardwareError  0x06
#define Code_Protect_FanFault       0x07
#define Code_Protect_WatchdogFault  0x08
#define Code_Protect_BatPNFault     0x09
#define Code_Protect_PowerVoltFault     0x0A


typedef struct struct_Protect_Value
{
    Uint16 Protect_EN;                  //���ϼ��ʹ�ܿ���
    Uint16 Num_Change_Mask;             //����ת��������ʱ��
    Uint16 Cnt_Change_Mask;             //����ת��������ʱ�������
    Uint16 Num_Check;                   //������λ�жϴ���
    Uint16 Cnt_Check;                   //������λ�жϴ���������
    Uint16 Equation;                    //�Ƚ��жϹ�ϵ
     float32 Now_Value;                   //���Ƚ�����ǰֵ
     float32 Ref_Value;                   //�Ƚ�����
    Uint32 Mask;                        //���ϱ�־λ������
    Uint32 *Ptr;                        //��Ҫ��λ�ı���ָ��
} struct_Protect_Value;

typedef struct struct_Protect_Global
{
    struct_Protect_Value CH_Over_Cur[3];
    struct_Protect_Value CH_Over_Vol;
    struct_Protect_Value CH_Over_Vol_Bat;
    struct_Protect_Value Bat_Reverse_Out;

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
extern void Protect_Value(struct_Protect_Value *Ptr);
extern void Protect_Global_Init(void);
extern void Upload_Fault_Code(void);
extern void Data_Up(void);

#endif /* APP_INCLUDE_PROTECT_GLOBAL_H_ */
