/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_led.h
** Author      : Huang Cheng
** Date        : 2022-01-13
** Description : 
**********************************************************************************************************/

#ifndef _BSP_LED_H_
#define _BSP_LED_H_

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

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
typedef enum
{
	RUN_LED_R = 0x00,
	RUN_LED_G,
	CH1_LED_R,
	CH1_LED_G,
	CH2_LED_R,
	CH2_LED_G,
	CH3_LED_R,
	CH3_LED_G,
	CH4_LED_R,
	CH4_LED_G,
	CH5_LED_R,
	CH5_LED_G,
	CH6_LED_R,
	CH6_LED_G,
	CH7_LED_R,
	CH7_LED_G,
	CH8_LED_R,
	CH8_LED_G,
}Led_Type;

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void bsp_LedInit(void);
void bsp_AllLedOff(void);
void bsp_LedOff(Led_Type LedType);
void bsp_LedOn(Led_Type LedType);
void bsp_LedToggle(uint8_t _no);
uint8_t Led_Shine_Ctrl(uint8_t _no, uint16_t t_on, uint16_t t_off, uint16_t t_scale);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
