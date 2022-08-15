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

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/

/**********************************************************************************************************	
	JN-MPU-C022F : LED PIN映射关系
	
		板上:
		LED81 : PE2	(低电平点亮，高电平熄灭)
 		LED82 : PE3	(低电平点亮，高电平熄灭)
 		LED83 : PE4	(低电平点亮，高电平熄灭)		
		
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
**	LED82 : PE3
**	LED83 : PE4
**/
#define GPIO_PORT_NOUSE    0
#define GPIO_PIN_NOUSE     0

#define GPIO_PORT_LED82		GPIO_PORT_E			
#define GPIO_PIN_LED82		GPIO_PIN_03

#define GPIO_PORT_LED83		GPIO_PORT_E			
#define GPIO_PIN_LED83		GPIO_PIN_04

/*	LED0_R : PD2
**	LED0_G : PD3
**/
#define GPIO_PORT_LED0_R	GPIO_PORT_D
#define GPIO_PIN_LED0_R		GPIO_PIN_02

#define GPIO_PORT_LED0_G	GPIO_PORT_D
#define GPIO_PIN_LED0_G		GPIO_PIN_03

/*	LED1_R : PD4
**	LED1_G : PD5
**/	
#define GPIO_PORT_LED1_R	GPIO_PORT_D
#define GPIO_PIN_LED1_R		GPIO_PIN_04

#define GPIO_PORT_LED1_G	GPIO_PORT_D
#define GPIO_PIN_LED1_G		GPIO_PIN_05

/*	LED2_R : PD7
**	LED2_G : PD6
**/
#define GPIO_PORT_LED2_R	GPIO_PORT_D
#define GPIO_PIN_LED2_R		GPIO_PIN_07

#define GPIO_PORT_LED2_G	GPIO_PORT_D
#define GPIO_PIN_LED2_G		GPIO_PIN_06
	
/*	LED3_R : PG10
**	LED3_G : PG9
**/	
#define GPIO_PORT_LED3_R	GPIO_PORT_G
#define GPIO_PIN_LED3_R		GPIO_PIN_10

#define GPIO_PORT_LED3_G	GPIO_PORT_G
#define GPIO_PIN_LED3_G		GPIO_PIN_09

/*	LED4_R : PB6
**	LED4_G : PB7
**/
#define GPIO_PORT_LED4_R	GPIO_PORT_B
#define GPIO_PIN_LED4_R		GPIO_PIN_06

#define GPIO_PORT_LED4_G	GPIO_PORT_B
#define GPIO_PIN_LED4_G		GPIO_PIN_07

/*	LED5_R : PA6
**	LED5_G : PA5
**/
#define GPIO_PORT_LED5_R	GPIO_PORT_A
#define GPIO_PIN_LED5_R		GPIO_PIN_06

#define GPIO_PORT_LED5_G	GPIO_PORT_A
#define GPIO_PIN_LED5_G		GPIO_PIN_05

/*	LED6_R : PC5
**	LED6_G : PB11
**/
#define GPIO_PORT_LED6_R	GPIO_PORT_C
#define GPIO_PIN_LED6_R		GPIO_PIN_05

#define GPIO_PORT_LED6_G	GPIO_PORT_B
#define GPIO_PIN_LED6_G		GPIO_PIN_11

/*	LED7_R : PA7
**	LED7_G : PC4
**/
#define GPIO_PORT_LED7_G	GPIO_PORT_A
#define GPIO_PIN_LED7_G		GPIO_PIN_07

#define GPIO_PORT_LED7_R	GPIO_PORT_C
#define GPIO_PIN_LED7_R		GPIO_PIN_04

/*	LED8_R : PA2
**	LED8_G : PA1
**/
#define GPIO_PORT_LED8_R	GPIO_PORT_A
#define GPIO_PIN_LED8_R		GPIO_PIN_02

#define GPIO_PORT_LED8_G	GPIO_PORT_A
#define GPIO_PIN_LED8_G		GPIO_PIN_01

/* LED配置区 */
#define RUN_LED_PORT_G		(GPIO_PORT_LED0_G)
#define RUN_LED_PIN_G		(GPIO_PIN_LED0_G)	  

#define RUN_LED_PORT_R		(GPIO_PORT_LED0_R)
#define RUN_LED_PIN_R		(GPIO_PIN_LED0_R)	

#ifdef YCJ
	#define CH1_LED_PORT_G		(GPIO_PORT_LED1_G)
	#define CH1_LED_PIN_G		(GPIO_PIN_LED1_G)

	#define CH1_LED_PORT_R		(GPIO_PORT_LED1_R)
	#define CH1_LED_PIN_R		(GPIO_PIN_LED1_R)

	#define CH2_LED_PORT_G		(GPIO_PORT_LED2_G)
	#define CH2_LED_PIN_G		(GPIO_PIN_LED2_G)

	#define CH2_LED_PORT_R		(GPIO_PORT_LED2_R)
	#define CH2_LED_PIN_R		(GPIO_PIN_LED2_R)

	#define CH3_LED_PORT_G		(GPIO_PORT_LED3_G)
	#define CH3_LED_PIN_G		(GPIO_PIN_LED3_G)

	#define CH3_LED_PORT_R		(GPIO_PORT_LED3_R)
	#define CH3_LED_PIN_R		(GPIO_PIN_LED3_R)

	#define CH4_LED_PORT_G		(GPIO_PORT_LED4_G)
	#define CH4_LED_PIN_G		(GPIO_PIN_LED4_G)

	#define CH4_LED_PORT_R		(GPIO_PORT_LED4_R)
	#define CH4_LED_PIN_R		(GPIO_PIN_LED4_R)

	#define CH5_LED_PORT_G		(GPIO_PORT_LED5_G)
	#define CH5_LED_PIN_G		(GPIO_PIN_LED5_G)

	#define CH5_LED_PORT_R		(GPIO_PORT_LED5_R)
	#define CH5_LED_PIN_R		(GPIO_PIN_LED5_R)

	#define CH6_LED_PORT_G		(GPIO_PORT_LED6_G)
	#define CH6_LED_PIN_G		(GPIO_PIN_LED6_G)

	#define CH6_LED_PORT_R		(GPIO_PORT_LED6_R)
	#define CH6_LED_PIN_R		(GPIO_PIN_LED6_R)

	#define CH7_LED_PORT_G		(GPIO_PORT_LED7_G)
	#define CH7_LED_PIN_G		(GPIO_PIN_LED7_G)

	#define CH7_LED_PORT_R		(GPIO_PORT_LED7_R)
	#define CH7_LED_PIN_R		(GPIO_PIN_LED7_R)

	#define CH8_LED_PORT_G		(GPIO_PORT_LED8_G)
	#define CH8_LED_PIN_G		(GPIO_PIN_LED8_G)

	#define CH8_LED_PORT_R		(GPIO_PORT_LED8_R)
	#define CH8_LED_PIN_R		(GPIO_PIN_LED8_R)
#else
	#define CH1_LED_PORT_G		(GPIO_PORT_LED2_G)
	#define CH1_LED_PIN_G		(GPIO_PIN_LED2_G)

	#define CH1_LED_PORT_R		(GPIO_PORT_LED2_R)
	#define CH1_LED_PIN_R		(GPIO_PIN_LED2_R)

	#define CH2_LED_PORT_G		(GPIO_PORT_LED4_G)
	#define CH2_LED_PIN_G		(GPIO_PIN_LED4_G)

	#define CH2_LED_PORT_R		(GPIO_PORT_LED4_R)
	#define CH2_LED_PIN_R		(GPIO_PIN_LED4_R)

	#define CH3_LED_PORT_G		(GPIO_PORT_LED6_G)
	#define CH3_LED_PIN_G		(GPIO_PIN_LED6_G)

	#define CH3_LED_PORT_R		(GPIO_PORT_LED6_R)
	#define CH3_LED_PIN_R		(GPIO_PIN_LED6_R)

	#define CH4_LED_PORT_G		(GPIO_PORT_LED8_G)
	#define CH4_LED_PIN_G		(GPIO_PIN_LED8_G)

	#define CH4_LED_PORT_R		(GPIO_PORT_LED8_R)
	#define CH4_LED_PIN_R		(GPIO_PIN_LED8_R)
#endif

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
	#define LED_81_NO			10
	#define LED_82_NO			11
	#define LED_83_NO			12
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

