/*
 * DCDC_BSP.h
 *
 *  Created on: 2021��6��20��
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_DCDC_BSP_H_
#define DSP28377D_BSP_INCLUDE_DCDC_BSP_H_

/*
 * DCDC_BSP.h
 *
 *  Created on: 2021��6��19��
 *      Author: JC
 */

//DCDC��س�������
#define PWM_DEADTIME        23              //�ű����¹�����ʱ��160nS=16*10nS
#define PWM_PERIOD          2000            //��������20uS
#define PWM_PERIOD_AGE      1600            //�ϻ�ģʽ��������16uS
#define PWM_MAX_CV          1212            //CVģʽ��������ռ�ձ�
#define PWM_MAX_DCV         1480            //DCVģʽ��������ռ�ձ�
#define PWM_MAX_CHARGE      1150            //���ģʽ���ռ�ձ�����,7.5V/13.2V*(2000-15)=1150
#define PWM_MAX_DISCH       1930            //�ŵ�ģʽ���ռ�ձ�����,(1-1.7/14.2)*(2000-15)=1747
#define CUR_SOFTOFF_END     1000            //��ضϽ�����������100mA��С�ڸ�ֱֵ�ӹ�PWM
//PI������ز���
#define DCDC_STRUCT_INIT        0       //DCDC״̬�����ݽṹ��ʼ��״̬
#define DCDC_SHUTDOWN           1       //PWM�����MOSȫ�����ڹر�״̬�����ϱ���ʱ�����״̬
#define DCDC_STANDBY            2       //����״̬��PWM���������Сռ�ձ�ʹ���MOS���ڷ�ƫ״̬
//#define DCDC_READY              3       //�ɴ���ģʽת����ģʽʱ���м�׼��״̬N
#define DCDC_READY1             18      //�ɴ���ģʽת����ģʽʱ���м�׼��״̬
#define DCDC_READY2             19      //�ɴ���ģʽת����ģʽʱ���м�׼��״̬
#define DCDC_DEBUG              3      //����ģʽ������ģ��ֱ������PWMֵ
#define DCDC_SOFTOFF_CH         15      //���ģʽ��ضϽ׶Σ���ɺ���DCDC_PWM_OFF״̬
#define DCDC_SOFTOFF_DISCH      16      //�ŵ�ģʽ��ضϽ׶Σ���ɺ���DCDC_PWM_OFF״̬
#define DCDC_PWM_OFF            17      //ֱ�ӹر�PWM״̬����ɺ���DCDC_SHUTDOWN
#define CHANNELS 4

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
    int32 Forward_Out;                  //ǰ������
    int32 Out;                          //��С�޷�������
} struct_PI;

typedef struct INTERFACE_DCDC
{
    //�������ݣ�����״̬�ж�����������
    float Req_ILSet1_Pu;               //����������ʱ�ĵ���
    float Req_ILSet2_Pu;               //����������ʱ�ĵ���
    float Req_Ioutsense_Pu;
    float Req_VoutSense_Pu;
    //�������ݣ�ֻ��ģʽ�л�ʱ������
     Uint16 Req_Mode;                    //�������еĹ���ģʽ�������DCDC_STRUCT_INIT�ȶ���
     Uint16 Req_ChangeMode;              //DCDC����ģʽ�ı䣬����ģʽ�л�
} INTERFACE_DCDC;
typedef struct struct_Channel
{
     int32 Set_Cur;                     //��ǰͨ��Ŀ�����

    Uint16 EN_Debug;                    //����ģʽʹ�ܱ�־
    Uint16 PWMxA_Debug;                 //����ģʽ��,����ֱ���趨PWMxA��ռ�ձ�
    Uint16 PWMxB_Debug;                 //����ģʽ��,����ֱ���趨PWMxB��ռ�ձ�


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

} struct_Channel;
typedef struct struct_DCDC_SERVICE
{
     float Set_ILSet1_Pu;
     float Set_ILSet2_Pu;
     float Set_Ioutsense_Pu;
     float Set_VoutSense_Pu;
    //ģ���ڲ�ʹ�ñ���
     Uint16 Now_Mode;                    //DCDC����״̬����ǰ״̬��
     float32 Now_Vol_Out;                 //ģ������˵�ѹ
     float32 Now_Vol_Bus;                 //ģ������Bus��ѹ
     float32 PWM_Ready;
     Uint16 Cnt_Delay;
     Uint32 Fault_Module;
} struct_DCDC_SERVICE;
extern volatile struct EPWM_REGS *ePWM[9];
extern struct_DCDC_SERVICE DCDC_Service[CHANNELS];       //DCDC״̬���������
extern INTERFACE_DCDC Interface_DCDC[CHANNELS];          //DCDCģ�鹤����������ӿ�
extern void DCDC_Service_BSP(void);
extern void InitDCDC_BSP(void);
extern Uint16 CMPAValue[4],CMPBValue[4];
#endif /* DSP28377D_BSP_INCLUDE_DCDC_BSP_H_ */
