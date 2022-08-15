/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_timer.h
** Author      : Huang Cheng
** Date        : 2021-11-11
** Description : bsp_timer head file
**********************************************************************************************************/

#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include <string.h>
#include "hc32_ddl.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#define TMRA1_UNIT                           (1U)
#define TMRA2_UNIT                           (2U)
#define TMRA3_UNIT                           (3U)
#define TMRA4_UNIT                           (4U)

/* 
 * APP_TIMER_UNIT: 可选TIMER0(M4_TMR0_1 - M4_TMR0_2)/TIMER2(M4_TMR2_1 - M4_TMR2_4)/TIMERA(M4_TMRA_1 - M4_TMRA_12)
 * APP_TIMER_CH:   可选TIMER0(CHA-CHB)/TIMER2(CHA-CHB)/TIMERA(CH1-CH4)
*/
#define APP_TMRA1_UNIT                       (M4_TMRA_1)
#define APP_TMRA1_PERIP_CLK                  (PWC_FCG2_TMRA_1)
#define APP_TMRA2_UNIT                       (M4_TMRA_2)
#define APP_TMRA2_PERIP_CLK                  (PWC_FCG2_TMRA_2)
#define APP_TMRA3_UNIT                       (M4_TMRA_3)
#define APP_TMRA3_PERIP_CLK                  (PWC_FCG2_TMRA_3)
#define APP_TMRA4_UNIT                       (M4_TMRA_4)
#define APP_TMRA4_PERIP_CLK                  (PWC_FCG2_TMRA_4)

/*
 * 定时器TimerA中断模式：    向上溢出中断
 * 定时器TimerA中断向量分组：M4_TMRA_x(x=1, 2): [Int000_IRQn, Int031_IRQn], [Int074_IRQn, Int079_IRQn];
 *                           M4_TMRA_x(x=3, 4): [Int000_IRQn, Int031_IRQn], [Int080_IRQn, Int085_IRQn];
 *                           M4_TMRA_x(x=5 ~ 8): [Int000_IRQn, Int031_IRQn], [Int092_IRQn, Int097_IRQn];
 *                           M4_TMRA_x(x=9 ~ 12): [Int000_IRQn, Int031_IRQn], [Int098_IRQn, Int103_IRQn];
*/
#define APP_TMRA1_FLAG                       (TMRA_FLAG_OVF)
#define APP_TMRA1_INT_TYPE                   (TMRA_INT_OVF)
#define APP_TMRA1_INT_PRIO                   (DDL_IRQ_PRIORITY_03)
#define APP_TMRA1_INT_SRC                    (INT_TMRA_1_OVF)
#define APP_TMRA1_IRQn                       (Int074_IRQn)

#define APP_TMRA2_FLAG                       (TMRA_FLAG_OVF)
#define APP_TMRA2_INT_TYPE                   (TMRA_INT_OVF)
#define APP_TMRA2_INT_PRIO                   (DDL_IRQ_PRIORITY_04)
#define APP_TMRA2_INT_SRC                    (INT_TMRA_2_OVF)
#define APP_TMRA2_IRQn                       (Int075_IRQn)

#define APP_TMRA3_FLAG                       (TMRA_FLAG_OVF)
#define APP_TMRA3_INT_TYPE                   (TMRA_INT_OVF)
#define APP_TMRA3_INT_PRIO                   (DDL_IRQ_PRIORITY_05)
#define APP_TMRA3_INT_SRC                    (INT_TMRA_3_OVF)
#define APP_TMRA3_IRQn                       (Int080_IRQn)

#define APP_TMRA4_FLAG                       (TMRA_FLAG_OVF)
#define APP_TMRA4_INT_TYPE                   (TMRA_INT_OVF)
#define APP_TMRA4_INT_PRIO                   (DDL_IRQ_PRIORITY_15)
#define APP_TMRA4_INT_SRC                    (INT_TMRA_3_OVF)
#define APP_TMRA4_IRQn                       (Int082_IRQn)
/*
 * TimerA时钟：    PLCK0(240MHz) - TMRA_1 ~ TIRA4, PCLK1(120MHz) - TMRA_5 ~ TIRA12
 * TimerA分频系数：TMRA_PCLK_DIV8(分频系数可选1/2/4/8/16/32/64/128/256/512/1024)
 * TimerA计数模式：TMRA_MODE_SAWTOOTH(锯齿型)（可选锯齿型、三角型）
 * TimerA计数方向：TMRA_DIR_UP（可选向上、向下计数），计数值最大16bit
 * TimerA定时时基：(APP_TMRA_PERIOD_VAL+1)*(1/(PLCK0/TMRA_PCLK_DIV8)) = 1ms
*/
#define APP_TMRA1_CLK                        (TMRA_CLK_PCLK)
#define APP_TMRA1_PCLK_DIV                   (TMRA_PCLK_DIV64)
#define APP_TMRA1_MODE                       (TMRA_MODE_SAWTOOTH)
#define APP_TMRA1_DIR                        (TMRA_DIR_UP)
#define APP_TMRA1_PERIOD_VAL                 (30000UL - 1UL)            //8ms中断一次

#define APP_TMRA2_CLK                        (TMRA_CLK_PCLK)
#define APP_TMRA2_PCLK_DIV                   (TMRA_PCLK_DIV32)
#define APP_TMRA2_MODE                       (TMRA_MODE_SAWTOOTH)
#define APP_TMRA2_DIR                        (TMRA_DIR_UP)
#define APP_TMRA2_PERIOD_VAL                 (30000UL - 1UL)           //4ms中断一次

#define APP_TMRA3_CLK                        (TMRA_CLK_PCLK)
#define APP_TMRA3_PCLK_DIV                   (TMRA_PCLK_DIV8)
#define APP_TMRA3_MODE                       (TMRA_MODE_SAWTOOTH)
#define APP_TMRA3_DIR                        (TMRA_DIR_UP)
#define APP_TMRA3_PERIOD_VAL                 (30000UL - 1UL)           //1ms中断一次

#define APP_TMRA4_CLK                        (TMRA_CLK_PCLK)
#define APP_TMRA4_PCLK_DIV                   (TMRA_PCLK_DIV8)
#define APP_TMRA4_MODE                       (TMRA_MODE_SAWTOOTH)
#define APP_TMRA4_DIR                        (TMRA_DIR_UP)
#define APP_TMRA4_PERIOD_VAL                 (30000UL - 1UL)           //1ms?D??ò?′?
/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern uint8_t TempSampCHx;	
extern uint8_t Aralm_Read_CHx;

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
#define TIMER_TEST

void TimerAUint1_Config(uint32_t ClkDiv, uint32_t PerVal);
void TimerAUint2_Config(uint32_t ClkDiv, uint32_t PerVal);
void TimerAUint3_Config(uint32_t ClkDiv, uint32_t PerVal);
void TimerAUint4_Config(uint32_t ClkDiv, uint32_t PerVal);
void AllTimerAUintStart(void);
void AllTimerAUintStop(void);
void TimerAUintStart(uint8_t UnitNum);
void TimerAUintStop(uint8_t UnitNum);
uint64_t bsp_GetRunTime(void);
uint64_t bsp_GetRunTime_S(void);	//获取当前运行时间 S

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
