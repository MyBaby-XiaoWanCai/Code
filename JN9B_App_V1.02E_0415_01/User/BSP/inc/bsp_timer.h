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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include <stdint.h>
#include <string.h>
#include "hc32_ddl.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
#define TMRA1_UNIT                           (1U)
#define TMRA2_UNIT                           (2U)
#define TMRA3_UNIT                           (3U)
#define TMRA4_UNIT                           (4U)

/* 
 * APP_TIMER_UNIT: ��ѡTIMER0(M4_TMR0_1 - M4_TMR0_2)/TIMER2(M4_TMR2_1 - M4_TMR2_4)/TIMERA(M4_TMRA_1 - M4_TMRA_12)
 * APP_TIMER_CH:   ��ѡTIMER0(CHA-CHB)/TIMER2(CHA-CHB)/TIMERA(CH1-CH4)
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
 * ��ʱ��TimerA�ж�ģʽ��    ��������ж�
 * ��ʱ��TimerA�ж��������飺M4_TMRA_x(x=1, 2): [Int000_IRQn, Int031_IRQn], [Int074_IRQn, Int079_IRQn];
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
 * TimerAʱ�ӣ�    PLCK0(240MHz) - TMRA_1 ~ TIRA4, PCLK1(120MHz) - TMRA_5 ~ TIRA12
 * TimerA��Ƶϵ����TMRA_PCLK_DIV8(��Ƶϵ����ѡ1/2/4/8/16/32/64/128/256/512/1024)
 * TimerA����ģʽ��TMRA_MODE_SAWTOOTH(�����)����ѡ����͡������ͣ�
 * TimerA��������TMRA_DIR_UP����ѡ���ϡ����¼�����������ֵ���16bit
 * TimerA��ʱʱ����(APP_TMRA_PERIOD_VAL+1)*(1/(PLCK0/TMRA_PCLK_DIV8)) = 1ms
*/
#define APP_TMRA1_CLK                        (TMRA_CLK_PCLK)
#define APP_TMRA1_PCLK_DIV                   (TMRA_PCLK_DIV64)
#define APP_TMRA1_MODE                       (TMRA_MODE_SAWTOOTH)
#define APP_TMRA1_DIR                        (TMRA_DIR_UP)
#define APP_TMRA1_PERIOD_VAL                 (30000UL - 1UL)            //8ms�ж�һ��

#define APP_TMRA2_CLK                        (TMRA_CLK_PCLK)
#define APP_TMRA2_PCLK_DIV                   (TMRA_PCLK_DIV32)
#define APP_TMRA2_MODE                       (TMRA_MODE_SAWTOOTH)
#define APP_TMRA2_DIR                        (TMRA_DIR_UP)
#define APP_TMRA2_PERIOD_VAL                 (30000UL - 1UL)           //4ms�ж�һ��

#define APP_TMRA3_CLK                        (TMRA_CLK_PCLK)
#define APP_TMRA3_PCLK_DIV                   (TMRA_PCLK_DIV8)
#define APP_TMRA3_MODE                       (TMRA_MODE_SAWTOOTH)
#define APP_TMRA3_DIR                        (TMRA_DIR_UP)
#define APP_TMRA3_PERIOD_VAL                 (30000UL - 1UL)           //1ms�ж�һ��

#define APP_TMRA4_CLK                        (TMRA_CLK_PCLK)
#define APP_TMRA4_PCLK_DIV                   (TMRA_PCLK_DIV8)
#define APP_TMRA4_MODE                       (TMRA_MODE_SAWTOOTH)
#define APP_TMRA4_DIR                        (TMRA_DIR_UP)
#define APP_TMRA4_PERIOD_VAL                 (30000UL - 1UL)           //1ms?D??��?��?
/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/


/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
extern uint8_t TempSampCHx;	
extern uint8_t Aralm_Read_CHx;

/**********************************************************************************************************
**                                         �ⲿ���ú�����
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
uint64_t bsp_GetRunTime_S(void);	//��ȡ��ǰ����ʱ�� S

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
