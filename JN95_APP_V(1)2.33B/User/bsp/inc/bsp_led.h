/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_led.h
** Author      : Cai Heng
** Date        : 2021-08-18
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
#include "stm32f4xx.h"

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

/*	LED81 : PE2
**	LED82 : PE3
**	LED83 : PE4
**/
//#define GPIO_PORT_LED81		GPIOE			
//#define GPIO_PIN_LED81		GPIO_PIN_2

#define GPIO_PORT_LED82		GPIOE			
#define GPIO_PIN_LED82		GPIO_PIN_3

#define GPIO_PORT_LED83		GPIOE			
#define GPIO_PIN_LED83		GPIO_PIN_4

/*	LED0_R : PD2
**	LED0_G : PD3
**/
#define GPIO_PORT_LED0_R	GPIOD
#define GPIO_PIN_LED0_R		GPIO_PIN_2

#define GPIO_PORT_LED0_G	GPIOD
#define GPIO_PIN_LED0_G		GPIO_PIN_3

/*	LED1_R : PD4
**	LED1_G : PD5
**/	
#define GPIO_PORT_LED1_R	GPIOD
#define GPIO_PIN_LED1_R		GPIO_PIN_4

#define GPIO_PORT_LED1_G	GPIOD
#define GPIO_PIN_LED1_G		GPIO_PIN_5

/*	LED2_R : PD7
**	LED2_G : PD6
**/
#define GPIO_PORT_LED2_R	GPIOD
#define GPIO_PIN_LED2_R		GPIO_PIN_7

#define GPIO_PORT_LED2_G	GPIOD
#define GPIO_PIN_LED2_G		GPIO_PIN_6
	
/*	LED3_R : PG10
**	LED3_G : PG9
**/	
#define GPIO_PORT_LED3_R	GPIOG
#define GPIO_PIN_LED3_R		GPIO_PIN_10

#define GPIO_PORT_LED3_G	GPIOG
#define GPIO_PIN_LED3_G		GPIO_PIN_9

/*	LED4_R : PB6
**	LED4_G : PB7
**/
#define GPIO_PORT_LED4_R	GPIOB
#define GPIO_PIN_LED4_R		GPIO_PIN_6

#define GPIO_PORT_LED4_G	GPIOB
#define GPIO_PIN_LED4_G		GPIO_PIN_7

/*	LED5_R : PA6
**	LED5_G : PA5
**/
#define GPIO_PORT_LED5_R	GPIOA
#define GPIO_PIN_LED5_R		GPIO_PIN_6

#define GPIO_PORT_LED5_G	GPIOA
#define GPIO_PIN_LED5_G		GPIO_PIN_5

/*	LED6_R : PC5
**	LED6_G : PB11
**/
#define GPIO_PORT_LED6_R	GPIOC
#define GPIO_PIN_LED6_R		GPIO_PIN_5

#define GPIO_PORT_LED6_G	GPIOB
#define GPIO_PIN_LED6_G		GPIO_PIN_11

/*	LED7_R :  PC4
**	LED7_G :  PA7
**/
#define GPIO_PORT_LED7_G	GPIOA
#define GPIO_PIN_LED7_G		GPIO_PIN_7

#define GPIO_PORT_LED7_R	GPIOC
#define GPIO_PIN_LED7_R		GPIO_PIN_4

/*	LED8_R : PA2
**	LED8_G : PA1
**/
#define GPIO_PORT_LED8_R	GPIOA
#define GPIO_PIN_LED8_R		GPIO_PIN_2

#define GPIO_PORT_LED8_G	GPIOA
#define GPIO_PIN_LED8_G		GPIO_PIN_1

/* LED配置区 */
#define RUN_LED_PORT_G		(GPIO_PORT_LED0_G)
#define RUN_LED_PIN_G		(GPIO_PIN_LED0_G)	  

#define RUN_LED_PORT_R		(GPIO_PORT_LED0_R)
#define RUN_LED_PIN_R		(GPIO_PIN_LED0_R)	

#ifdef YCJ
#define CH1_LED_RORT_G		(GPIO_PORT_LED1_G)
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
#define CH1_LED_RORT_G		(GPIO_PORT_LED2_G)
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
/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
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
