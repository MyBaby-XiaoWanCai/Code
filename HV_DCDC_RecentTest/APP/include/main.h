/*
 * main.h
 *
 *  Created on: 2021年4月23日
 *      Author: JC
 */

#ifndef APP_INCLUDE_MAIN_H_
#define APP_INCLUDE_MAIN_H_

#define FALSE       0
#define TRUE        1

#define     DSP_CLOCK           200                     //100MHz时钟
#define     C_TIME_05MS         (DSP_CLOCK*500L)            //0.5ms对应的定时器1计数值
#define     C_TIME_20MS         (DSP_CLOCK*1000L)
#define     C_TIME_045MS        (DSP_CLOCK*450L)            //0.45ms对应的定时器1计数值
/************************************************************
    基本函数定义和引用
************************************************************/
#define  GetTime()  (CpuTimer1.RegsAddr->TIM.all)

 // 公共模块使用的数据结构
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
    Uint16    ADCInt;                                 //ADC中断执行时间
    Uint16    PWMInt;                                 //PWM中断执行时间
    Uint16    tmpBase;
    Uint16    tmpTime;
    Uint16    CpuCoff0Ms;
    Uint16    tmp0Ms;
    Uint16    Det05msClk;
    Uint16    CpuBusyCoff;                            // cpu时间片执行度系数
}CPU_TIME_STRUCT;       //统计模块执行时间的数据结构




// 其它参数定义
extern CPU_TIME_STRUCT             gCpuTime;
#endif /* APP_INCLUDE_MAIN_H_ */
