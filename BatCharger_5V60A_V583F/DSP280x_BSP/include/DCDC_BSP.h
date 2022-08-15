/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: DCDC_BSP.h
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef DSP2803X_BSP_INCLUDE_DCDC_BSP_H_
#define DSP2803X_BSP_INCLUDE_DCDC_BSP_H_
//DCDC��س�������
#define PWM_DEADTIME        16              //�ű����¹�����ʱ��160nS=16*10nS
#define PWM_PERIOD          2000            //��������20uS
#define PWM_PERIOD_AGE      1600            //�ϻ�ģʽ��������16uS
#define PWM_MAX_CV          1212            //CVģʽ��������ռ�ձ�
#define PWM_MAX_DCV         1480            //DCVģʽ��������ռ�ձ�
#define PWM_MAX_CHARGE      1150            //���ģʽ���ռ�ձ�����,7.5V/13.2V*(2000-15)=1150
#define PWM_MAX_DISCH       1930            //�ŵ�ģʽ���ռ�ձ�����,(1-1.7/14.2)*(2000-15)=1747
#define CUR_SOFTOFF_END     1000            //��ضϽ�����������100mA��С�ڸ�ֱֵ�ӹ�PWM

#if   VER_PRODUCT == PRODUCT_5V20A
    #define CUR_TO_SINGLE_CH    50000L      //�����򲢻�ģʽ��ÿͨ��ƽ������С�ڸ�ֵ���л�����ͨ��ģʽ
    #define CUR_TO_LOW_RANGE    100000      //�л��������̵�����λ����,����ͨ��
#elif VER_PRODUCT == PRODUCT_5V30A
    #define CUR_TO_SINGLE_CH    20000L      //�����򲢻�ģʽ��ÿͨ��ƽ������С�ڸ�ֵ���л�����ͨ��ģʽ
    #define CUR_TO_LOW_RANGE    80000       //�л��������̵�����λ����,����ͨ��
#elif VER_PRODUCT == PRODUCT_5V60A
    #define CUR_TO_SINGLE_CH    20000L      //�����򲢻�ģʽ��ÿͨ��ƽ������С�ڸ�ֵ���л�����ͨ��ģʽ
    #define CUR_TO_LOW_RANGE    80000       //�л��������̵�����λ����,����ͨ��
#elif VER_PRODUCT == PRODUCT_5V80A
    #define CUR_TO_SINGLE_CH    20000L      //�����򲢻�ģʽ��ÿͨ��ƽ������С�ڸ�ֵ���л�����ͨ��ģʽ
    #define CUR_TO_LOW_RANGE    80000       //�л��������̵�����λ����,����ͨ��
#endif

#define CUR_FAST_OC_H       28900           //���ٹ�������AD����ֵ�ż���Լ50A/CH
#define CUR_FAST_OC_L       32700           //���ٹ�������AD����ֵ�ż���Լ11.3A/CH
#define CUR_SYNRECT_OFF     50000           //ͬ�������رյ���
#define VOL_BUS_H           165000          //Bus��ѹ���߱���
#define VOL_BUS_L           115000          //Bus��ѹ���ͱ���
#define CUR_TO_FAST_START	20000			//
#define CUR_SLEW_MAX        500             //
#define CUR_SLEW_DEFAULT    420             //

//PWMʹ�ܽӿڶ���
#define PWM1ON     {EALLOW; EPwm1Regs.TZSEL.all=0x0000; EPwm1Regs.TZCLR.all=0x0007;   \
                    EPwm1Regs.TZFRC.all=0x0000; EDIS;}
#define PWM1OFF    {EALLOW; EPwm1Regs.TZSEL.all=0x0100; EPwm1Regs.TZCTL.all=0x000A;   \
                    EPwm1Regs.TZFRC.all=0x0004; EDIS; EPwm1Regs.CMPA.half.CMPA=PWM_DEADTIME; EPwm1Regs.CMPB=PWM_PERIOD;}

#define PWM2ON     {EALLOW; EPwm2Regs.TZSEL.all=0x0000; EPwm2Regs.TZCLR.all=0x0007;   \
                    EPwm2Regs.TZFRC.all=0x0; EDIS;}
#define PWM2OFF    {EALLOW; EPwm2Regs.TZSEL.all=0x0100; EPwm2Regs.TZCTL.all=0x000A;   \
                    EPwm2Regs.TZFRC.all=0x0004; EDIS; EPwm2Regs.CMPA.half.CMPA=PWM_DEADTIME; EPwm2Regs.CMPB=PWM_PERIOD;}

//�ṹ�嶨��
//��SPI�ڶ�ȡ������ADֵ��ͨ���ű���ṹ��
typedef struct struct_Data_AD
{
     int16 AD7606_CH0;
     int16 AD7606_CH1;
     int16 AD7606_CH2;
     int16 AD7606_CH3;
     int16 AD7606_CH4;
     int16 AD7606_CH5;
     int16 AD7606_CH6;
     int16 AD7606_CH7;
     int16 F2808_CH0;
     int16 F2808_CH1;
     int16 F2808_CH2;
} struct_Data_AD;

//����ת�����ģ��������ƽ��ֵ
typedef struct struct_Data_Sum
{
    Uint16 Cnt_Sum;         //��ǰ�ۼӴ���������
    Uint16 Num_Avg;         //ƽ��ֵ�ۼӴ���
    Uint16 Flag_Refresh;    //�����ⲿѭ��ʹ�������ڼ����ݱ��ж��޸�

     int16 AD_PT;           //ADֵ��ƽ��ֵ
     int16 AD_NTC1;
     int16 AD_NTC2;
     Uint16 Temp_PT;         //������
     Uint16 Temp_NTC1;
     Uint16 Temp_NTC2;
     Uint16 Temp_Sink_S1;
     Uint16 Temp_Shunt_S1;

     int32 Vol_Sense;
     int32 Vol_Out;
     int32 Vol_Bus;
     int32 Vol_Contact;		//�����ѹ
     int32 Cur_CHA;
     int32 Cur_CHB;
     int32 Vol_5V;
     int32 AD_NTC_BATB;

     int32 Sum_Vol_Sense;   //�ۼӹ����ۼ�ֵ
     int32 Sum_Vol_Out;
     int32 Sum_Vol_Bus;
     int32 Sum_Cur_CHA;
     int32 Sum_Cur_CHB;
     int32 Sum_AD_PT;
     int32 Sum_AD_NTC1;
     int32 Sum_AD_NTC2;
     int32 Sum_AD_NTC_BATB;

     int32 Sum_Vol_Sense_Copy;//�ۼ�Num_Avg�κ���ۼ�ֵ
     int32 Sum_Vol_Out_Copy;
     int32 Sum_Vol_Bus_Copy;
     int32 Sum_Cur_CHA_Copy;
     int32 Sum_Cur_CHB_Copy;
     int32 Sum_AD_PT_Copy;
     int32 Sum_AD_NTC1_Copy;
     int32 Sum_AD_NTC2_Copy;
     int32 Sum_AD_NTC_BATB_Copy;
} struct_Data_Sum;

//����ת�����ģ��������ƽ��ֵ
typedef struct struct_Data_AD_Sum
{
    Uint16 Cnt_Sum;         //��ǰ�ۼӴ���������
    Uint16 Num_Avg;         //ƽ��ֵ�ۼӴ���

     int32 Sum_Vol_Sense_AD;   //�ۼӹ����ۼ�ֵ
     int32 Sum_Vol_Out_AD;
     int32 Sum_Vol_Bus_AD;
     int32 Sum_Cur_CHA_AD;
     int32 Sum_Cur_CHB_AD;

     int32 Sum_Vol_Sense_AD_Copy;//�ۼ�Num_Avg�κ���ۼ�ֵ
     int32 Sum_Vol_Out_AD_Copy;
     int32 Sum_Vol_Bus_AD_Copy;
     int32 Sum_Cur_CHA_AD_Copy;
     int32 Sum_Cur_CHB_AD_Copy;
} struct_Data_AD_Sum;

typedef struct struct_Cur_Scale
{
     int32 K_Now;       //���ݶ���ʵ��ʹ��Kֵ����
     int32 B_Now;       //���ݶ���ʵ��ʹ��Bֵ������B_Now=B_ROM*10+B_C_Env+B_C_Cur+B_C_Res
     int32 K_ROM_CH;    //EEPROM��ȡ������Kֵ
     int32 B_ROM_CH;    //EEPROM��ȡ������Bֵ
     int32 K_ROM_DIS;   //EEPROM��ȡ������Kֵ
     int32 B_ROM_DIS;   //EEPROM��ȡ������Bֵ
     int32 B_C_Env;     //������������Bֵ��
     int32 B_C_Cur;     //������������Bֵ��
     int32 B_C_Res;     //��ģ��������貹��Bֵ��
} struct_Cur_Scale;

typedef struct struct_Vol_Scale
{
     int32 K_ROM_Out;
     int32 B_ROM_Out;
     int32 K_Now_Out;
     int32 B_Now_Out;

     int32 K_ROM_Sense;
     int32 B_ROM_Sense;
     int32 K_Now_Sense;
     int32 B_Now_Sense;

     int32 K_ROM_Bus;
     int32 K_Now_Bus;
} struct_Vol_Scale;

//PI������ز���
typedef struct struct_PI
{
   Uint16 Cnt;                          //PI����������ڼ�����
   Uint16 Period;                       //PI�����������
    int32 Ref;                          //�ο�ֵ
    int32 Feed;                         //����ֵ
    int32 Error;                        //ƫ��
    int32 Limit_Err_Max;                //����ƫ������
    int32 Limit_Err_Min;                //����ƫ������
   Uint32 Kp;                           //��������
   Uint32 Ki;                           //���ֲ���
    int32 Out_P;                        //�������
    int32 Out_I;                        //�������
    int32 Out_PI;                       //PI�������
    int32 Limit_PI_max;                 //I�������
    int32 Limit_PI_min;                 //I�������
    int16 Q_Type;                       //Out_PI��Out����λ��
    int32 Limit_Out_max;                //Out�������
    int32 Limit_Out_min;                //Out�������
    int32 Forward_Out;					//ǰ������
    int32 Out;                          //��С�޷�������
} struct_PI;

//���ٱ����ã���ֹ��Դ��
typedef struct struct_Prot_Fast
{
    Uint32 Limit_Cur_Out;               //�����������ֵ���ٹ�����������
    Uint16 Limit_Cur_Out_Num;           //�����������������Ҫ���������Ĵ���
    Uint16 Limit_Cur_Out_Cnt;           //����������޼�����
    Uint32 Limit_Vol_Out;               //�����ѹ���ٹ�ѹ��������
    Uint16 Limit_Vol_Out_Num;           //�����ѹ��ѹ������Ҫ���������Ĵ���
    Uint16 Limit_Vol_Out_Cnt;           //�����ѹ���޼�����
    Uint32 Limit_Vol_Bus;               //�����ѹ���ٹ�ѹ��������
    Uint16 Limit_Vol_Bus_Num;           //�����ѹ��ѹ������Ҫ���������Ĵ���
    Uint16 Limit_Vol_Bus_Cnt;           //�����ѹ���޼�����
} struct_Prot_Fast;

typedef struct INTERFACE_DCDC
{
    //�������ݣ�����״̬�ж�����������
     int32 Req_Cur_Total;               //����������ʱ�ĵ���,0.1mA/LSB
     int32 Req_Vol;                     //��������ĵ�ѹ,0.1mV/LSB
    Uint16 Req_Refresh_Cur;             //����ˢ�µ�������ֵ
    Uint16 Req_Refresh_Vol;             //����ˢ�µ�ѹ����ֵ
    Uint16 Req_CurRange;               //������λ,0-�ߵ�λ��1-�͵�λ

    //�������ݣ�ֻ��ģʽ�л�ʱ������
    Uint16 Req_Mode;                    //�������еĹ���ģʽ�������DCDC_STRUCT_INIT�ȶ���
    Uint16 Req_ChangeMode;              //DCDC����ģʽ�ı䣬����ģʽ�л�
    Uint16 Parall_Total;                //��������1-������2-˫����3-������4-�Ļ�
    Uint16 EN_Run;                      //ģ���Ƿ��������б�־
    Uint16 EN_MOS_Out;                  //���MOS�Ƿ���������־
    Uint16 EN_CHA_Comm;                 //ͨѶ�ڻ�����ģ�����CHAͨ���Ƿ��������б�־
    Uint16 EN_CHA_SynRect;              //ͨѶ�ڻ�����ģ�����CHAͨ���Ƿ�����ͬ��������־
    Uint16 EN_CHB_Comm;                 //ͨѶ�ڻ�����ģ�����CHBͨ���Ƿ��������б�־
    Uint16 EN_CHB_SynRect;              //ͨѶ�ڻ�����ģ�����CHBͨ���Ƿ�����ͬ��������־
    Uint16 Mode_Parall;                 //ģ�鲢���򵥻�����ģʽ��0-����ģʽ��1-����������2-�����ӻ�
    Uint16 Req_Aging;                   //�ϻ�ģʽ��־
    Uint16 EN_MOS_Out_Slaver;           //��������ģʽ��ͨѶ�ڻ�����ģ����ƴӻ����MOS�Ƿ���
    Uint16 EN_Run_Slaver;               //��������ģʽ��ͨѶ�ڻ�����ģ����ƴӻ��Ƿ���������
} INTERFACE_DCDC;

//Req_Mode,DCDC_Service״̬��״̬����
#define DCDC_STRUCT_INIT        0       //DCDC״̬�����ݽṹ��ʼ��״̬
#define DCDC_SHUTDOWN           1       //PWM�����MOSȫ�����ڹر�״̬�����ϱ���ʱ�����״̬
#define DCDC_STANDBY            2       //����״̬��PWM���������Сռ�ձ�ʹ���MOS���ڷ�ƫ״̬
#define DCDC_READY              3       //�ɴ���ģʽת����ģʽʱ���м�׼��״̬
#define DCDC_CC                 4       //�������ģʽ
#define DCDC_CCtoCV             5       //����ת��ѹ���ģʽ
#define DCDC_DCC                6       //�����ŵ�ģʽ
#define DCDC_DCCtoDCV           7       //����ת��ѹ�ŵ�ģʽ
#define DCDC_STATIC             8       //����ģʽ��PWM���������Сռ�ձ�ʹ���MOS���ڷ�ƫ״̬
#define DCDC_CV                 9       //��ѹ���ģʽ�������ѹ����ֱ�ӿ���PWM����Ҫ�Ͽ�ķ�����
#define DCDC_DCV                10      //��ѹ����ģʽ�������ѹ����ֱ�ӿ���PWM����Ҫ�����ķ�����
#define DCDC_DIAGN_A2B          12      //���ģʽ1��Aͨ��������CVģʽ��Bͨ��������DCCģʽ
#define DCDC_DIAGN_B2A          13      //���ģʽ2��Bͨ��������CVģʽ��Aͨ��������DCCģʽ
#define DCDC_DEBUG              14      //����ģʽ������ģ��ֱ������PWMֵ
#define DCDC_SOFTOFF_CH         15      //���ģʽ��ضϽ׶Σ���ɺ���DCDC_PWM_OFF״̬
#define DCDC_SOFTOFF_DISCH      16      //�ŵ�ģʽ��ضϽ׶Σ���ɺ���DCDC_PWM_OFF״̬
#define DCDC_PWM_OFF            17      //ֱ�ӹر�PWM״̬����ɺ���DCDC_SHUTDOWN
#define DCDC_READY1             18      //�ɴ���ģʽת����ģʽʱ���м�׼��״̬
#define DCDC_READY2             19      //�ɴ���ģʽת����ģʽʱ���м�׼��״̬

//Mode_Parall����ģʽ����
#define SINGLE                  0
#define PARALL_MASTER           1
#define PARALL_SLAVER           2

typedef struct struct_DCDC_SERVICE
{
     int32 Set_Cur_Total;               //Ŀ�����,��ģʽת��ʱ�ӽӿڵ�Req_Cur_Total��������
     int32 Set_Vol;                     //Ŀ���ѹ,��ģʽת��ʱ�ӽӿڵ�Req_Vol��������
    Uint16 Parall_Total;                //��������,��ģʽת��ʱ�ӽӿڵ�Num_Parral��������
    Uint16 Mode_Parall;                 //ģ�鲢���򵥻�����ģʽ��0-����ģʽ��1-����������2-�����ӻ�
    Uint16 Flag_Aging;                  //�ϻ�ģʽʹ�ܱ�־

    //ģ���ڲ�ʹ�ñ���
    Uint16 Now_Mode;                    //DCDC����״̬����ǰ״̬��
     int32 Now_Cur_AB;                  //����A��Bͨ������ƽ��ֵ�ĺ�
     int32 Now_Vol_Sense;               //��ǰSense�˵�ѹ
     int32 Now_Vol_Out;                 //ģ������˵�ѹ
     int32 Now_Vol_Bus;                 //ģ������Bus��ѹ
    // int16 T_Sink1;                     //ɢ����1�¶�,��ӦJP?����
    Uint16 PWM_Ready;                   //Ready1״̬PWM��Ҫ����������,����Ԥ��������ݵ�ѹ
    Uint16 PWM_DCM;						//DCMģʽ������Ŀ�������������Ҫ�ﵽ��PWM����
    Uint16 Cnt_Delay;                   //״̬���ڲ���ʱ��
    Uint16 Cnt_CCtoCV;                  //CCתCV��ʱ������
    Uint16 Num_CCtoCV;                  //CCתCV���ټ����Ҫ���Ĵ���
    Uint16 Cnt_CCtoCV2;                  //CCתCV��ʱ������
    Uint16 Flag_CV_Step;                //�����ѹ���׶α�־
    Uint16 Cnt_DCCtoDCV;                //DCCתDCV��ʱ������
    Uint16 Num_DCCtoDCV;
    Uint16 Cnt_DCCtoDCV2;                //DCCתDCV��ʱ������
    Uint16 Flag_DCV_Step;               //�����ѹ�ŵ�׶α�־
     int32 Cur_Fast_OC_AD;              //���ٹ��������ż�ADֵ
    Uint16 Cnt_FastRefresh;             //����ת����ǰһ�����ˢ������
    Uint16 Cnt_Time_CV_DCV;             //�����ѹ�׶�����ʱ��

    Uint16 Flag_CV_Step2;                //�����ѹ���׶α�־2
    Uint16 Flag_DCV_Step2;               //�����ѹ�ŵ�׶α�־2
    Uint16 Flag_SingleCH;               //��ͨ������ģʽ��־
    Uint16 Flag_CurRange;               //������λ,0-�ߵ�λ��1-�͵�λ
    Uint16 Flag_Soft_OFF;               //��ض��Ѿ����б�־
     int32 Limit_Set_Vol;               //�����ѹ����,�ϵ�����ģ���ͺų�ʼ��һ��,0.1mV/LSB
    Uint16 Slew_Vol_ON;                 //CVģʽ��������ѹ����б��
    Uint16 Cnt_Bus_H;
    Uint32 Cnt_Bus_L;
    Uint16 Cnt_Out_H;                   //ģ������˿ڿ��ٹ�ѹ����
    Uint16 Cnt_Fault_AD7606;            //AD7606���ݶ�ȡ���������
    Uint16 Cnt_Timeout_Busy;            //AD7606��Busy�źų�ʱ������
    Uint32 Fault_Module;                //ģ��ǻ��ɹ�����ع���
    Uint32 Fault_Slaver1;               //ģ��1�ǻ��ɹ�����ع���
    Uint32 Fault_Slaver2;               //ģ��2�ǻ��ɹ�����ع���
    Uint32 Fault_Slaver3;               //ģ��3�ǻ��ɹ�����ع���
    Uint32 Fault_Slaver4;               //ģ��4�ǻ��ɹ�����ع���
     int32 Sum_Vol_Sense;               //��ѹ�������ѹƽ��ֵ��
     int32 Vol_Sense_LPF;               //20mS��ѹƽ��ֵ
     int32 Vol_Sense_LPF_Last;
    Uint16 Cnt_Sum_Vol_Sense;           //Sense��ѹƽ���ü�����
    Uint16 Num_Sum_Vol_Sense;           //Sense��ѹƽ������
    Uint32 Time_Running;                //ͬһ������������ʱ�䣬20uS/LSB

    struct_PI *Ptr_PI_Vol_Charge;       //CCtoCVģʽ��ѹ��PI�������ݽṹ��ָ��
    struct_PI *Ptr_PI_Vol_Discharge;    //DCCtoDCVģʽ��ѹ��PI�������ݽṹ��ָ��
    struct_PI *Ptr_PI_Vol_CV;           //CVģʽ��ѹ��PI�������ݽṹ��ָ��
    struct_PI *Ptr_PI_Vol_DCV;          //DCVģʽ��ѹ��PI�������ݽṹ��ָ��
    struct_Vol_Scale Vol_Scale;         //Out��ѹ��Sense��ѹУ׼����
} struct_DCDC_SERVICE;

//�ߵ͵�����λ��־����,Flag_CurRange
#define CUR_RANGE_H         0           //�ߵ�����λ��־����
#define CUR_RANGE_L         1           //�͵�����λ��־����

//ģ����ϱ��ض���,Fault_Module
#define FAULT_OC_CHA        0x00000001      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ��Aͨ�����������������
#define FAULT_OC_CHB        0x00000002      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ��Bͨ�����������������
#define FAULT_FAN           0x00000004      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ����ȹ���
#define FAULT_OT_SINK       0x00000008      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ���ڲ����ȱ���
#define FAULT_BUS_H         0x00000010      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ������Bus��ѹ���߱���
#define FAULT_BUS_L         0x00000020      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ������Bus��ѹ���ͱ������ϵ���ӳ�2S��ʼ���
#define FAULT_DATA_CALI     0x00000040      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ��У׼�����쳣
#define FAULT_SLAVER_CAN    0x00000080      //�����ӻ�ģʽ��ģ��CANͨѶ����
#define FAULT_AD7606        0x00000100      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ���ȡAD7606���ݴ���
#define FAULT_CH_IMBALA     0x00000200      //����ģʽ����������ģʽ�������ӻ�ģʽ�����ϻ�ģʽ��CV������ͨ����������Χ
//#define FAULT_FLASH         0x00000400      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ�����FLASH��д����
#define FAULT_OUT_VOL_HIGH  0x00000400      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ�����FLASH��д����
#define FAULT_OUTMOS        0x00000800      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ���������MOS����
#define FAULT_ANALOG_CHA    0x00001000      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ��Aͨ����������ģ���·�쳣
#define FAULT_ANALOG_CHB    0x00002000      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ��Bͨ����������ģ���·�쳣
#define FAULT_ANALOG_PT     0x00004000      //����ģʽ����������ģʽ��ģ�����¶Ȳ���ģ���·�쳣
#define FAULT_ANALOG_NTC    0x00008000      //����ģʽ����������ģʽ�������ӻ�ģʽ��ģ��ɢ�����¶Ȳ���ģ���·�쳣

#define FAULT_S1_CAN        0x01000000      //�����������ӻ�1CANͨѶ���ϱ�־
#define FAULT_S2_CAN        0x02000000      //�����������ӻ�2CANͨѶ���ϱ�־
#define FAULT_S3_CAN        0x04000000      //�����������ӻ�3CANͨѶ���ϱ�־
#define FAULT_S4_CAN        0x08000000      //�����������ӻ�4CANͨѶ���ϱ�־
#define FAULT_SLAVER1       0x10000000      //�����������ӻ�1���ϱ�־
#define FAULT_SLAVER2       0x20000000      //�����������ӻ�2���ϱ�־
#define FAULT_SLAVER3       0x40000000      //�����������ӻ�3���ϱ�־
#define FAULT_SLAVER4       0x80000000      //�����������ӻ�4���ϱ�־

typedef struct struct_Channel
{
     int32 Set_Cur;                     //��ǰͨ��Ŀ�����
    Uint16 EN_Run;                      //ģ���ڲ��趨ͨ������ʹ�ܱ�־
    Uint16 EN_Run_Comm;                 //�ⲿͨѶ���趨ͨ������ʹ�ܱ�־
    Uint16 EN_SynRect;                  //ͬ������ʹ�ܱ�־
    Uint16 EN_Debug;                    //����ģʽʹ�ܱ�־
    Uint16 EN_Forward;					//������ǰ��ʹ�ܿ���

    Uint16 PWMxA_Debug;                 //����ģʽ��,����ֱ���趨PWMxA��ռ�ձ�
    Uint16 PWMxB_Debug;                 //����ģʽ��,����ֱ���趨PWMxB��ռ�ձ�
    Uint16 Slew_Cur_ON;                 //��������������б��
    Uint16 Slew_Cur_OFF;                //���رյ����½�б��
     int32 Limit_Set_Cur;               //��ͨ�������������
    Uint16 Req_FastStart;				//������������
    Uint16 Req_FastStart_DCM;              //������������
    Uint16 Cnt_FastStart;				//����������ʱ������
    //Uint16 Num_FastStart;				//���������ۼӴ���
    //Uint16 ACC_FastStart;				//���������ۼ�ֵ

    //ģ���ڲ��������ⲿ����Ҫ����
    Uint16 Cnt_SynRect;                 //ͬ������������ʱ������
    Uint16 Flag_SynRect;                //ͬ��������������־
    Uint16 Cnt_SynRect_Off;             //ͬ�������ر���ʱ����
    Uint16 PWMxA;                       //��ǰ����ͨ��PWMxA��ռ�ձ�
    Uint16 PWMxB;                       //��ǰ����ͨ��PWMxB��ռ�ձ�
     int32 Now_Cur;                     //��ǰͨ������ת����ĵ���ֵ
    Uint16 Cnt_Out_OC;
    Uint16 Flag_MOS_Out;                //���MOS����״̬��־
    struct_PI *Ptr_PI_Cur_Charge;       //���ģʽ������PI���ƽṹ��ָ�룬�ڽṹ���ʼ��ʱָ��PI_Cur_Charge
    struct_PI *Ptr_PI_Cur_Discharge;    //�ŵ�ģʽ������PI���ƽṹ��ָ�룬�ڽṹ���ʼ��ʱָ��PI_Cur_Disharge
    struct_Cur_Scale Cur_Scale_H;       //��ǰͨ�������ߵ�λУ׼�������������
    struct_Cur_Scale Cur_Scale_L;       //��ǰͨ�������͵�λУ׼�������������
} struct_Channel;

typedef struct struct_Bus_Forward
{
	Uint16 Cnt_Sample;
	 int32 Vol_Bus_Slow;
	 int32 Vol_Bus_Slow_Last;
	 int32 Vol_Bus_Fast;
	 int32 Vol_Bus_Fast_Last;
	 int32 Vol_Bus_Var;					//Bus��ѹ�仯��
	 int32 K_Forward;					//��ǰ��ǿ��ϵ��
	 int32 K_Forward_Charge;            //���״̬ǰ��ǿ��ϵ��
     int32 K_Forward_Discharge;         //�ŵ�״̬ǰ��ǿ��ϵ��
	 int32 Limit_Pos;					//�����޷�
	 int32 Limit_Neg;					//�����޷�
	 int32 PWM_Forward;					//ǰ��ֵ
} struct_Bus_Forward;

typedef struct struct_Bus_Fluct_Check
{
    Uint16 Flag_Bus_Fluct;
    Uint16 Flag_Bus_Low;
    Uint16 Cnt_Delay;
    Uint16 Cnt_Mask;
    Uint16 Num_Mask;
} struct_Bus_Fluct_Check;

typedef struct struct_Adjust_PI_Vol
{
    Uint16 Cnt_Rise;
    Uint16 Cnt_Fall;
    Uint16 Num;
     int32 Ref_Vol_Error;
     int32 Limit_Max;
     int32 Limit_Min;
} struct_Adjust_PI_Vol;



//���ⲿ�ļ�����ȫ�ֱ�������
extern Uint32 Flag_Tasks_1mS;
extern Uint32 Flag_Tasks_20mS;
extern Uint32 Flag_Tasks_500mS;
extern struct_Data_Sum Data_Sum;           //����ת���������������ƽ����õ���ֵ
extern struct_DCDC_SERVICE DCDC_Service;
extern INTERFACE_DCDC Interface_DCDC;
extern struct_Channel CH_A;               //CHA�������
extern struct_Channel CH_B;               //CHB�������
extern struct_Data_AD_Sum Data_AD_Sum;
extern struct_PI PI_Vol_Charge;                //CCtoCVģʽ��ѹ��PI�������ݽṹ��
extern struct_PI PI_Vol_Discharge;             //DCCtoDCVģʽ��ѹ��PI�������ݽṹ��
extern struct_Data_AD Data_AD;            //��AD7606��F2808������ADԭʼ����
extern struct_Bus_Forward Bus_Forward;
//���ⲿ�ļ����ź�������
extern interrupt void EPWM1_INT_ISR_BSP(void);
extern void InitDCDC_BSP(void);
extern void InitSPIB_BSP(void);
extern void Function_DCDC_CCtoCV(void);
extern void Function_DCDC_DCCtoDCV(void);
extern void Auto_Adjust_PI_Vol_Charge(void);
extern void Auto_Adjust_PI_Vol_Discharge(void);

#endif /* DSP2803X_BSP_INCLUDE_DCDC_BSP_H_ */
