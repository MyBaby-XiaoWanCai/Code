/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_led.h
** Author      : Huang Cheng
** Date        : 2021-11-05
** Description : 
**********************************************************************************************************/
#ifndef __BSP_LED_H
#define __BSP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include "hc32_ddl.h"
#include "main.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/

/**********************************************************************************************************	
	JN-MPU-C022F : LED PIN映射关系
	
		板上:
		LED81 : PE2	(低电平点亮，高电平熄灭)
 		LED82 : PD15	(低电平点亮，高电平熄灭)
 		LED83 : PD14	(低电平点亮，高电平熄灭)		
		
		灯板:  
		LED0_R : PD2		LED0_G : PD3
		LED1_R : PD4		LED1_G : PD5  (高电平点亮，低电平熄灭)
		LED2_R : PD7		LED2_G : PD6  (高电平点亮，低电平熄灭)
		LED3_R : PG10		LED3_G : PG9  (高电平点亮，低电平熄灭)
		LED4_R : PB6		LED4_G : PB7  (高电平点亮，低电平熄灭)
		LED5_R : PA6		LED5_G : PA5
		LED6_R : PC5		LED6_G : PB11
		LED7_R : PA7		LED7_G : PC4
		LED8_R : PA2		LED8_G : PA1
**********************************************************************************************************/

#define LED_TEST

/*	LED81 : PE2
**	LED82 : PD14
**	LED83 : PD15
**/
#define GPIO_PORT_NOUSE    0
#define GPIO_PIN_NOUSE     0

#define GPIO_PORT_LED82		GPIO_PORT_D			
#define GPIO_PIN_LED82		GPIO_PIN_15

#define GPIO_PORT_LED83		GPIO_PORT_D			
#define GPIO_PIN_LED83		GPIO_PIN_14

/* LED通道号 */
#ifdef YCJ
	#define RUN_LED_R_NO		0
	#define RUN_LED_G_NO		1 
	#define CH1_LED_R_NO		2
	#define CH1_LED_G_NO		3
	#define CH2_LED_R_NO		4
	#define CH2_LED_G_NO		5
	#define CH3_LED_R_NO		6
	#define CH3_LED_G_NO		7
	#define CH4_LED_R_NO		8
	#define CH4_LED_G_NO		9
	#define CH5_LED_R_NO		10
	#define CH5_LED_G_NO		11
	#define CH6_LED_R_NO		12
	#define CH6_LED_G_NO		13
	#define CH7_LED_R_NO		14
	#define CH7_LED_G_NO		15
	#define CH8_LED_R_NO		16
	#define CH8_LED_G_NO		17
	#define LED_81_NO			18
	#define LED_82_NO			19
	#define LED_83_NO			20
#else
	#define RUN_LED_R_NO		0
	#define RUN_LED_G_NO		1 	
	#define CH1_LED_R_NO		2
	#define CH1_LED_G_NO		3
	#define CH2_LED_R_NO		4
	#define CH2_LED_G_NO		5
	#define CH3_LED_R_NO		6
	#define CH3_LED_G_NO		7
	#define CH4_LED_R_NO		8
	#define CH4_LED_G_NO		9
	#define LED_81_NO				10
	#define LED_82_NO				11
	#define LED_83_NO				12
#endif
	#define LED_NOUSE_NUM       LED_81_NO
	#define LED_MAX_NUM         LED_83_NO

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
//测试
void LedTest(void);

void bsp_InitLed(void);
void bsp_LedToggle(uint8_t _no);
void bsp_LedOn(uint8_t _no);
void bsp_LedOff(uint8_t _no);
uint8_t Led_Shine_Ctrl(uint8_t _no, uint16_t t_on, uint16_t t_off, uint16_t t_scale);

#ifdef __cplusplus
}
#endif	

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

