/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_pwm.h
** Author      :  
** Date        : 2022-01-17
** Description : 
**********************************************************************************************************/
#ifndef _BSP_PWM_H_
#define _BSP_PWM_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* Function of this example. */
#define FAN_FUNC_NORMAL_SINGLE_PWM          (0U)
#define APP_FUNC_SINGLE_EDGE_ALIGNED_PWM    (1U)
#define APP_FUNC_TOW_EDGE_SYMMETRIC_PWM     (2U)

/* Specify the function of the example. */
#define FAN_FUNC                            (FAN_FUNC_NORMAL_SINGLE_PWM)

/*
 * Define the configurations of PWM according to the function that selected.
 * In this example:
 *   1. System clock is XTAL(8MHz).
 *   2. Clock source of TimerA is PCLK(8MHz by default) and divided by 1.
 *   3. About PWM:
 *      APP_FUNC_NORMAL_SINGLE_PWM: frequecy 200KHz, high duty 30%
 *      APP_FUNC_SINGLE_EDGE_ALIGNED_PWM: frequecy 100KH, high duty 30%, 55%
 *      APP_FUNC_TOW_EDGE_SYMMETRIC_PWM: frequecy 50KHz, high duty 50%, 40%
 *
 * Sawtooth mode:
 *   Calculate the period value according to the frequency:
 *     PeriodVal = (TimerAClockFrequency(Hz) / PWMFreq) - 1
 *   Calculate the compare value according to the duty ratio:
 *     CmpVal = ((PeriodVal + 1) * Duty) - 1
 *
 * Triangle mode:
 *   Calculate the period value according to the frequency:
 *     PeriodVal = (TimerAClockFrequency(Hz) / (PWMFreq * 2))
 *   Calculate the compare value according to the duty ratio:
 *     CmpVal = (PeriodVal * Duty)
 */
 
#define FAN_TMRA_UNIT                   (M4_TMRA_4)
#define FAN_TMRA_PERIP_CLK              (PWC_FCG2_TMRA_4)
#define FAN_TMRA_PWM_A_CH               (TMRA_CH_2)

#define FAN_TMR6_4_PWMB_PORT      			(GPIO_PORT_D)
#define FAN_TMR6_4_PWMB_PIN             (GPIO_PIN_13)

#define FAN_TMRA_PWM_A_PIN_FUNC         (GPIO_FUNC_4_TIMA4_PWM2)	//TIMA_4_PWM2; PA9 - TIMA_1_PWM2
#define FAN_TMRA4_PCLK_DIV              (TMRA_PCLK_DIV8)	//分频系数

#define FAN_TMRA_MODE                   (TMRA_MODE_SAWTOOTH)	//TMRA_MODE_TRIANGLE、  TMRA_MODE_SAWTOOTH(锯齿波)
#define FAN_TMRA_DIR                    (TMRA_DIR_UP)	//向上计数-TMRA_DIR_UP   向下计数-TMRA_DIR_DOWN
#define FAN_TMRA_PERIOD_VAL             (600UL - 1UL) //周期
#define FAN_TMRA_PWM_A_CMP_VAL          (300UL - 1UL)


/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void TIM6_4_PWMB_Config(void);
//void FanPwmSet(uint32_t pwmval);
void FanPwmSet(float duty);
float ArrDataMax_Get(float *Arr,uint8_t len);
void FanPwmControl(void);
#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
