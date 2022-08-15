/*
 * main.h
 *
 *  Created on: 2021��4��23��
 *      Author: JC
 */

#ifndef APP_INCLUDE_MAIN_H_
#define APP_INCLUDE_MAIN_H_

#define FALSE       0
#define TRUE        1

#define     DSP_CLOCK           200                     //100MHzʱ��
#define     C_TIME_05MS         (DSP_CLOCK*500L)            //0.5ms��Ӧ�Ķ�ʱ��1����ֵ
#define     C_TIME_20MS         (DSP_CLOCK*1000L)
#define     C_TIME_045MS        (DSP_CLOCK*450L)            //0.45ms��Ӧ�Ķ�ʱ��1����ֵ
/************************************************************
    �����������������
************************************************************/
#define  GetTime()  (CpuTimer1.RegsAddr->TIM.all)

 // ����ģ��ʹ�õ����ݽṹ
typedef struct CPU_TIME_STRUCT_DEF {
    Uint32   MFA2msBase;
    Uint32   MFB2msBase;
    Uint32   MFC2msBase;
    Uint32   MFD2msBase;
    Uint32   Motor05MsBase;
    Uint32   ADCIntBase;
    Uint32   PWMIntBase;
    Uint16    MFA2ms;
    Uint16    MFB2ms;
    Uint16    MFC2ms;
    Uint16    MFD2ms;
    Uint16    Motor05Ms;
    Uint16    ADCInt;                                 //ADC�ж�ִ��ʱ��
    Uint16    PWMInt;                                 //PWM�ж�ִ��ʱ��
    Uint16    tmpBase;
    Uint16    tmpTime;
    Uint16    CpuCoff0Ms;
    Uint16    tmp0Ms;
    Uint16    Det05msClk;
    Uint16    CpuBusyCoff;                            // cpuʱ��Ƭִ�ж�ϵ��
}CPU_TIME_STRUCT;       //ͳ��ģ��ִ��ʱ������ݽṹ




// ������������
extern CPU_TIME_STRUCT             gCpuTime;
#endif /* APP_INCLUDE_MAIN_H_ */
