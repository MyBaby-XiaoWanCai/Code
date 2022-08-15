/*
 * DCDC_BSP.h
 *
 *  Created on: 2021��6��20��
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_DCDC_BSP_H_
#define DSP28377D_BSP_INCLUDE_DCDC_BSP_H_
#include "PI_CLA.h"
/*
 * DCDC_BSP.h
 *
 *  Created on: 2021��6��19��
 *      Author: JC
 */
#define FAULT_HARD         0x00000001
#define FAULT_BUS_H        0x00000002
#define FAULT_BUS_L        0x00000004
#define FAULT_PreCharg     0x00000008

#define RDADC_DAT      (volatile Uint32 *)0x380000
//DCDC��س�������
#define PWM_DEADREDTIME        30              //�ű����¹�����ʱ��160nS=16*10nS
#define PWM_DEADFEDTIME        35              //�ű����¹�����ʱ��160nS=16*10nS
#define PWM_PERIOD          1000            //��������20uS
#define PWM_PERIOD_AGE      1600            //�ϻ�ģʽ��������16uS
#define PWM_MAX_CV          1212            //CVģʽ��������ռ�ձ�
#define PWM_MAX_DCV         1480            //DCVģʽ��������ռ�ձ�
#define PWM_MAX_CHARGE      950          //���ģʽ���ռ�ձ�����,7.5V/13.2V*(2000-15)=1150
#define PWM_MAX_DISCH       1930            //�ŵ�ģʽ���ռ�ձ�����,(1-1.7/14.2)*(2000-15)=1747
#define CUR_SOFTOFF_END     1000            //��ضϽ�����������100mA��С�ڸ�ֱֵ�ӹ�PWM
//PI������ز���
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
#define DCDC_RUN                11       //�������ģʽ
#define DCDC_DEBUG              12      //����ģʽ������ģ��ֱ������PWMֵ
#define DCDC_Waitime            13      //���ģʽ1��Aͨ��������CVģʽ��Bͨ��������DCCģʽ
#define DCDC_Waitime1           14      //���ģʽ2��Bͨ��������CVģʽ��Aͨ��������DCCģʽ
#define DCDC_Waitime2           15      //���ģʽ��ضϽ׶Σ���ɺ���DCDC_PWM_OFF״̬
#define DCDC_SOFTOFF            16      //�ŵ�ģʽ��ضϽ׶Σ���ɺ���DCDC_PWM_OFF״̬
#define DCDC_PWM_OFF            17      //ֱ�ӹر�PWM״̬����ɺ���DCDC_SHUTDOWN
#define DCDC_READY1             18      //�ɴ���ģʽת����ģʽʱ���м�׼��״̬
#define DCDC_READY2             19      //�ɴ���ģʽת����ģʽʱ���м�׼��״̬
#define DCDC_READY3             20      //�ɴ���ģʽת����ģʽʱ���м�׼��״̬

#define CHANNELS 4
#define ISR_FREQUENCY   10
#define PI           3.14


#define VOL_BUS_H           94          //Bus��ѹ���߱���
#define VOL_BUS_L           70          //Bus��ѹ���ͱ���
/*typedef struct struct_PI
{
   Uint16 Cnt;                          //PI����������ڼ�����
   Uint16 Period;                       //PI�����������
   float32 Ref;                          //�ο�ֵ
   float32 Feed;                         //����ֵ
   float32 Error;                        //ƫ��
   float32 Limit_Err_Max;                //����ƫ������
   float32 Limit_Err_Min;                //����ƫ������
   float32 Kp;                           //��������
   float32 Ki;                           //���ֲ���
   float32 Out_P;                        //�������
   float32 Out_I;                        //�������
   float32 Out_PI;                       //PI�������
   float32 Limit_PI_max;                 //I�������
   float32 Limit_PI_min;                 //I�������
   float32 Limit_Out_max;                //Out�������
   float32 Limit_Out_min;                //Out�������
   float32 Out;                          //��С�޷�������
} struct_PI;*/

typedef struct INTERFACE_DCDC
{
    //�������ݣ�����״̬�ж�����������
    float Req_ILSet1;               //����������ʱ�ĵ���
    float Req_ILSet2;               //����������ʱ�ĵ���
    float Req_Ioutsense;
    float Req_VoutSense;
    //�������ݣ�ֻ��ģʽ�л�ʱ������
     Uint16 Req_Mode;                    //�������еĹ���ģʽ�������DCDC_STRUCT_INIT�ȶ���
     Uint16 Req_ChangeMode;              //DCDC����ģʽ�ı䣬����ģʽ�л�
     int32 Req_Cur_Total;               //����������ʱ�ĵ���,0.1mA/LSB
     int32 Req_Vol;                     //��������ĵ�ѹ,0.1mV/LSB
     Uint16 Req_Refresh_Cur;             //����ˢ�µ�������ֵ

} INTERFACE_DCDC;
typedef struct struct_Channel
{
     int32 Set_Cur;                     //��ǰͨ��Ŀ�����
    Uint16 EN_SynRect;                  //ͬ������ʹ�ܱ�־
    Uint16 PWMxA_Debug;                 //����ģʽ��,����ֱ���趨PWMxA��ռ�ձ�
    Uint16 PWMxB_Debug;                 //����ģʽ��,����ֱ���趨PWMxB��ռ�ձ�
    Uint16 Slew_Cur_ON;                 //��������������б��
    Uint16 Slew_Cur_OFF;                //���رյ����½�б


    Uint16 Cnt_FastStart;               //����������ʱ������
    //ģ���ڲ��������ⲿ����Ҫ����
    Uint16 Cnt_SynRect;                 //ͬ������������ʱ������
    Uint16 Flag_SynRect;                //ͬ��������������־
    Uint16 Cnt_SynRect_Off;             //ͬ�������ر���ʱ����
    Uint16 PWMxA;                       //��ǰ����ͨ��PWMxA��ռ�ձ�
    Uint16 PWMxAHR;
    Uint16 PWMxB;                       //��ǰ����ͨ��PWMxB��ռ�ձ�
    Uint16 PWMxBHR;
     int32 Now_Cur;                     //��ǰͨ������ת����ĵ���ֵ
    Uint16 Cnt_Out_OC;
    Uint16 Flag_MOS_Out;                //���MOS����״̬��־

    Uint16 Req_FastStart;             //������������
    Uint16 Req_FastStart_DCM;              //������������

} struct_Channel;
typedef struct struct_DCDC_SERVICE
{
     float Set_ILSet1;
     float Set_ILSet2;
     float Set_Iout;
     float Ref_Iout;
     float CV_Ref_Iout;
     float Ref_Iout_Min;
     float Ref_Vout;
     float Set_Vout;
    //ģ���ڲ�ʹ�ñ���
     Uint16 Now_Mode;                    //DCDC����״̬����ǰ״̬��
     Uint16 Run_Mode;
     Uint16 Flag_Mode;
     float32 Now_Vol_Out;                 //ģ������˵�ѹ
     float32 Now_Vol_Bus;                 //ģ������Bus��ѹ
     float32 Now_Vol_JR;
     float32 Now_Vol_Sense;
     float32 PWM_Ready;
     float32 Slew_Cur_ON;
     float32 Slew_Cur_OFF;
     Uint16 Cnt_Delay;
     Uint32 Fault_Module;
     Uint16 Type;
     Uint16 Flag_Swtich;
     Uint16 Flag_Once;
     Uint16 CV_Cnt;
     Uint16 CV_Period;
} struct_DCDC_SERVICE;
typedef struct struct_CH_PUBLIC_VARIABLE
{
    float32 Slew_Cur_ON;                 //��������������б��
    float32 Slew_Cur_OFF;                //���رյ����½�б��
    float32 Limit_Set_Cur;
    Uint16 Parall_Total;

    Uint16 Flag_Cur;
    Uint16 Flag_Record;
}struct_CH_PUBLIC_VARIABLE;

typedef struct struct_FILTER_COEFF
{
   float  FILTER_CC2;
   float  FILTER_CC1;
   float  LPF500;
   float  ts0;
   float  sampletime;
   Uint16  samplefreq;
   float  filter_u2[4][7];
   float filter_u1[4][7];
   float filter_i2[4][7];
   float filter_i1[4][7];
   float Ad_Result[4][7];
}struct_FILTER_COEFF;

typedef struct struct_Data_AD_Sum
{
    Uint16 Cnt_Sum;         //��ǰ�ۼӴ���������
    Uint16 Num_Avg;         //ƽ��ֵ�ۼӴ���

     int32 Sum_Vol_Sense_AD;   //�ۼӹ����ۼ�ֵ
     int32 Sum_Vol_Out_AD;
     int32 Sum_Vol_Bus_AD;
     int32 Sum_Cur_CHA_AD;
     int32 Sum_Cur_CHB_AD;
     int32 Sum_Cur_Out_AD;
     int32 Sum_Cur_Out_AD_Copy;
     int32 Sum_Vol_Sense_AD_Copy;//�ۼ�Num_Avg�κ���ۼ�ֵ
     int32 Sum_Vol_Out_AD_Copy;
     int32 Sum_Vol_Bus_AD_Copy;
     int32 Sum_Cur_CHA_AD_Copy;
     int32 Sum_Cur_CHB_AD_Copy;
} struct_Data_AD_Sum;

typedef struct struct_Data_Sum
{
    Uint16 Cnt_Sum;         //��ǰ�ۼӴ���������
    Uint16 Num_Avg;         //ƽ��ֵ�ۼӴ���
    Uint16 Flag_Refresh;    //�����ⲿѭ��ʹ�������ڼ����ݱ��ж��޸�



    float32 Vol_Sense;
    float32 Cur_Out;
    float32 Vol_Out;
    float32 Vol_Bus;
     float32 Vol_JR;     //������ѹ
     int32 Cur_CHA;
     int32 Cur_CHB;
     int32 Vol_5V;
     int32 AD_NTC_BATB;

     float32 Sum_Vol_Sense;   //�ۼӹ����ۼ�ֵ
     float32 Sum_Cur_Out;   //�ۼӹ����ۼ�ֵ
     float32 Sum_Vol_Out;
     float32 Sum_Vol_Bus;
     int32 Sum_Cur_CHA;
     int32 Sum_Cur_CHB;
     float32 Sum_Vol_JR;
     int32 Sum_AD_NTC1;
     int32 Sum_AD_NTC2;
     int32 Sum_AD_NTC_BATB;

     float32 Sum_Vol_Sense_Copy;//�ۼ�Num_Avg�κ���ۼ�ֵ
     float32 Sum_Cur_Out_Copy;//�ۼ�Num_Avg�κ���ۼ�ֵ
     float32 Sum_Vol_Out_Copy;
     float32 Sum_Vol_Bus_Copy;
     int32 Sum_Cur_CHA_Copy;
     int32 Sum_Cur_CHB_Copy;
     float32 Sum_Vol_JR_Copy;
     int32 Sum_AD_NTC1_Copy;
     int32 Sum_AD_NTC2_Copy;
     int32 Sum_AD_NTC_BATB_Copy;
} struct_Data_Sum;

extern volatile struct EPWM_REGS *ePWM[9];

extern struct_DCDC_SERVICE DCDC_Service[CHANNELS];       //DCDC״̬���������
extern INTERFACE_DCDC Interface_DCDC[CHANNELS];          //DCDCģ�鹤����������ӿ�
extern struct_Data_Sum Data_Sum[CHANNELS];
extern void DCDC_Service_BSP(void);
extern void InitDCDC_BSP(void);
extern struct_Data_AD_Sum Data_AD_Sum[CHANNELS];
extern struct_Channel CH_A[CHANNELS];
extern struct_Channel CH_B[CHANNELS];
//extern PI_CLA CH_A_PI_Cur[CHANNELS];
//extern PI_CLA CH_B_PI_Cur[CHANNELS];
extern PI_CLA CH_PI_Iout[CHANNELS];
extern PI_CLA CH_PI_Vout[CHANNELS];
extern PI_CLA CH_PI_Vsense[CHANNELS];
extern Uint32 gCputime0_time3,gCputime0_max;
extern struct_CH_PUBLIC_VARIABLE  CH_Public_Variable;
extern void SynRect_Buck(struct_Channel *Ptr);
extern void SynRect_Boost(struct_Channel *Ptr);
#endif /* DSP28377D_BSP_INCLUDE_DCDC_BSP_H_ */
