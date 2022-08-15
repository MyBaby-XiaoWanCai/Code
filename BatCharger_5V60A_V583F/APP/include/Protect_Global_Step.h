/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Protect_Global_Step.h
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef APP_INCLUDE_PROTECT_GLOBAL_STEP_H_
#define APP_INCLUDE_PROTECT_GLOBAL_STEP_H_


//�ṹ�嶨��
typedef struct struct_Fault_Code
{
    Uint32 Err_Global;
    Uint32 Err_Global_Bak;
    Uint32 Err_Step;
    Uint32 Err_Step_Bak;
    Uint32 Alarm;
    Uint32 Alarm_Bak;
    Uint16 Cnt_S2M_Fault_Mask;//��������Ϻ�����һ��ʱ����մӻ��Ĺ�����
    Uint16 Code;
    Uint16 Resend_Alarm;
} struct_Fault_Code;

/************************************************************************************
    Global Fault code  1
************************************************************************************/
#define cNoFault                    0x00000000        //ϵͳ�޹���
#define cProtectCVFault             0x00000001        //���CV����
#define cProtectCCFault             0x00000002        //���CC����
#define cProtectDCVFault            0x00000004        //�ŵ�CV����
#define cProtectDCCFault            0x00000008        //�ŵ�DCC����
#define cChargeOverCAHFault         0x00000010        //�����ݱ���
#define cOPCurrentLimitFault        0x00000020        //�����������
#define cBatPNFault                 0x00000040        //��ط���!
#define cBusVoltHighFault           0x00000080        //����Bus��ѹ
#define cBatVoltFlucFault           0x00000100        //��ص�ѹ��������
#define cBatVoltMutatFault          0x00000200        //��ص�ѹͻ�����
#define cFanFault                   0x00000400        //���ȹ���
#define cTerminalConnectFault       0x00000800        //����˽Ӳ����
#define cOPShortFault               0x00001000        //�����·
#define cOPCurrentFlucFault         0x00002000        //���������������
#define cOPCurrentMutatFault        0x00004000        //�������ͻ�����
#define cBatShortFault              0x00008000        //��ض�·����
/************************************************************************************
    Fault code  2
************************************************************************************/
#define cBatTempHighFault           0x00010000        //��ع��±���
#define cBatTempLowFault            0x01000000        //���Ƿ�±���  20191210
#define cImpedanceFault             0x00020000        //�Ӵ��迹
#define cOPCOverDiffFault           0x00040000        //��������
#define cBatVOverDiffFault          0x00080000        //��ѹ����
#define cDisChargeOverAHFault       0x00100000        //�ŵ���ݱ���
#define cLoopImpedanceFault         0x00200000        //��·�迹
#define cPowerVoltFault             0x00400000        //��·�迹
#define cModuleSinkHigh             0x00800000        //ģ���ڲ�ɢ�������ȱ���  20191210
#define cBusLow                     0x02000000        //Bus���籣��  20191220
#define cCaliDataError				0x04000000		  //У׼�����쳣
#define cStepDataError				0x08000000		  //���������쳣
#define cHardwareError				0x10000000		  //Ӳ������
#define cOtherError					0x80000000		  //��������
#define cTotalOverAHFault           0x20000000        //

/*********************************************************************************
                SStepFaultcode  1
*********************************************************************************/
#define cBatTempUpperLimit          0x00000001
#define cCapacityUpperLimit         0x00000002
#define cCapacityLowerLimit         0x00000004
#define cBatVoltUpperLimit          0x00000008
#define cBatVoltLowerLimit          0x00000010
#define cCurrUpperLimit             0x00000020
#define cCurrLowerLimit             0x00000040
#define cBatVoltPositiveJump        0x00000080
#define cBatVoltNegativeJump        0x00000100
#define cOPCPositiveJump            0x00000200
#define cOPCNegativeJump            0x00000400
#define cBatTempLowerLimt           0x00000800

#define cVol_Time_Error             0x00001000
#define cVol_Change_Time_Error      0x00002000
#define cBatShort                   0x00004000

#define cVol_Diff                   0x00010000
#define cCur_Diff                   0x00020000
#define cVoltIntervalPos            0x00040000
#define cVoltIntervalNeg            0x00080000


#define cStepEnd2Fault              0x10000000
#define cStepEnd3Fault              0x20000000
#define cBatErrorVol_Start          0x40000000
#define cBatErrorContacCheck        0x80000000

//�澯��־λ����
#define cBatTempAlarmFault          0x10000000        //��ع��¸澯
#define cBatTempOverAlarmFault      0x20000000        //��ع��¸澯
#define cBatTempDownUpperLimit      0x00100000


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


typedef struct struct_Protect_Value_Diff
{
    Uint16 Protect_EN;                  //���ϼ��ʹ�ܿ���
    Uint16 Num_Change_Mask;             //����ת��������ʱ��
    Uint16 Cnt_Change_Mask;             //����ת��������ʱ�������
    Uint16 Num_Check;                   //������λ�жϴ���
    Uint16 Cnt_Check;                   //������λ�жϴ���������
    Uint16 Equation;                    //�Ƚ��жϹ�ϵ
     int32 Now_Value;                   //��ǰֵ
     int32 Ref_Value;                   //����Ŀ��ֵ
     int32 Abs_Value_Diff;              //��ǰֵ��Ƚ����޲�ֵ�ľ���ֵ
     int32 Ref_Value_Diff;              //�Ƚ�����
    Uint32 Mask;                        //���ϱ�־λ������
    Uint32 *Ptr;                        //��Ҫ��λ�ı���ָ��
} struct_Protect_Value_Diff;

typedef struct struct_Protect_Delta
{
    Uint16 Protect_EN;                  //���ϼ��ʹ�ܿ���
    Uint16 Flag_SynUpData;				//�Ƿ�ͬ�����ϱ����ݱ�־
    Uint16 Num_Change_Mask;             //����ת��������ʱ��
    Uint16 Cnt_Change_Mask;             //����ת��������ʱ�������
    Uint16 Num_Check;                   //������λ�жϴ���
    Uint16 Cnt_Check;                   //������λ�жϴ���������
    Uint16 Equation;                    //�Ƚ��жϹ�ϵ
     int32 Now_Value;                   //������ǰֵ
     int32 Last_Value;                  //����Num_Deltaʱ��ǰ��ֵ
    Uint16 Num_Delta;                   //���������ʱ��
    Uint16 Cnt_Delta;                   //���������ʱ�������
     int32 Now_Delta;                   //Now_Value-Last_Value
     int32 Ref_Delta;               //�������ֵ
    Uint32 Mask;                        //���ϱ�־λ������
    Uint32 *Ptr;                        //��Ҫ��λ�ı���ָ��
} struct_Protect_Delta;


typedef struct struct_Protect_Interval
{
    Uint16 Protect_EN;                  //���ϼ��ʹ�ܿ���
    Uint16 Time_Delay;                   //����ת����Ƚ�ʱ��
    Uint16 Equation;                    //�Ƚ��жϹ�ϵ
     int32 Last_Value;                   //������ǰֵ
     int32 Now_Value;                  //����Num_Deltaʱ��ǰ��ֵ
     int32 Now_Delta;                   //���������ʱ��
     int32 Ref_Delta;                  //�������ֵ
    Uint32 Mask;                        //���ϱ�־λ������
    Uint32 *Ptr;                        //��Ҫ��λ�ı���ָ��
} struct_Protect_Interval;

typedef struct struct_Protect_Range
{
    Uint16 Protect_EN;                  //���ϼ��ʹ�ܿ���
    Uint32 Time_Delay;                  //����ת�����ӳ�ʱ��
    Uint16 Equation;                    //�Ƚ��жϹ�ϵ
    Uint16 Flag_Check;                  //�Ƿ��Ѿ����ı�־
     int32 Now_Value;                   //���Ƚ�����ǰֵ
     int32 Ref_Value_H;                 //�Ƚ���������
     int32 Ref_Value_L;                 //�Ƚ���������
    Uint32 Mask;                        //���ϱ�־λ������
    Uint32 *Ptr;                        //��Ҫ��λ�ı���ָ��
} struct_Protect_Range;

typedef struct struct_Protect_Vol_Change
{
    Uint16 Protect_EN;                  //���ϼ��ʹ�ܿ���
    Uint32 Time_Delay1;                  //����ת�����һ������ʱ���
    Uint32 Time_Delay2;                  //����ת����ڶ�������ʱ���
    Uint16 Equation;                    //�Ƚ��жϹ�ϵ
     int32 Now_Value;                   //���Ƚ�����ǰֵ
     int32 Value_t1;                    //��һ������ʱ�����ֵ
     int32 Value_t2;                    //�ڶ�������ʱ�����ֵ
     int32 Ref_Delta;                   //�Ƚ�����
     int32 Abs_Delta;                   //����õ�����ֵ�仯������ֵ
    Uint16 Flag_Check;                  //�Ƿ��Ѿ����ı�־
    Uint32 Mask;                        //���ϱ�־λ������
    Uint32 *Ptr;                        //��Ҫ��λ�ı���ָ��
} struct_Protect_Vol_Change;

typedef struct struct_Protect_ContactCheck
{
     int32 Limit_Vol_H_Start;
     int32 Limit_Vol_L_Start;
     int32 Time;
     int32 Cur;
     int32 Vol_Rise;
     int32 Vol_Start;
     int32 Vol_End;
    Uint16 Protect_EN;
} struct_Protect_ContactCheck;


typedef struct struct_Protect_Global
{
    struct_Protect_Value CH_Over_Cur;
    struct_Protect_Value CH_Over_Ah;
    struct_Protect_Value CH_Over_Vol_Bat;
    struct_Protect_Value CH_Over_Vol_Out;

    struct_Protect_Value DIS_Over_Cur;
    struct_Protect_Value DIS_Over_Ah;
    struct_Protect_Value DIS_Under_Vol_Bat;
    struct_Protect_Value DIS_Under_Vol_Out;

    struct_Protect_Value Bat_Over_Temp;
    struct_Protect_Value Bat_Under_Temp;
    struct_Protect_Value Bat_Temp_Alarm;
    struct_Protect_Value Bat_Temp_Over_Alarm;

    struct_Protect_Value Out_Cur_Over;

    struct_Protect_Value Res_Contact_Over;
    struct_Protect_Value Res_Loop_Over;
    struct_Protect_Value Bat_Reverse_Sense;
    struct_Protect_Value Bat_Reverse_Out;
    struct_Protect_Value Total_Over_Ah;

    struct_Protect_Value_Diff Cur_Diff_Over;
    struct_Protect_Value_Diff Vol_Diff_Over;

    struct_Protect_Delta Cur_Fluct_Over;
    struct_Protect_Delta Vol_Fluct_Over;

    struct_Protect_Delta Cur_Mutat_Over;
    struct_Protect_Delta Vol_Mutat_Over;

    int32  Cur_Max_CV_DCV;

    Uint32 LineRes;
    Uint32 LineVol;
    Uint16 Flag_Protect_Para;
} struct_Protect_Global;

//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern struct_Protect_Global Protect_Global;
extern struct_Protect_ContactCheck Protect_ContactCheck;

//���ⲿ�ļ����ź�������
extern void Protect_Value(struct_Protect_Value *Ptr);
extern void Protect_Value_Diff(struct_Protect_Value_Diff *Ptr);
extern void Protect_Mask_Counter(void);
extern void Fault_Mapping(void);
extern void Protect_Check(void);
extern void Protect_Check_Once(void);
extern void Protect_Check_Delta(void);
extern void Protect_Global_Init(void);
extern void Protect_Step_Init(void);
extern void Upload_Fault_Code(void);
extern void Protect_Configure_CC(void);
extern void Protect_Configure_CCtoCV(void);
extern void Protect_Configure_DCC(void);
extern void Protect_Configure_DCCtoDCV(void);
extern void Protect_Configure_STATIC(void);
extern void Protect_Global_Cnt_Reset(void);
extern void Protect_Step_Cnt_Reset(void);
extern void Protect_Configure_CV_PWM(void);
extern void Protect_Configure_DCV(void);
extern void Protect_CH_Imbalance(void);

#endif /* APP_INCLUDE_PROTECT_GLOBAL_STEP_H_ */

//File end
