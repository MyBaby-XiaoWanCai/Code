/**********************************************************************************************************
*
*	模块名称 : 硬件软件保护处理模块
*	文件名称 : bsp_Protection.h
*	版    本 : V1.0
*	说    明 : 头文件
*	作	 者：Robert.Li
**日	 期:  2021.7.2
**********************************************************************************************************/

#ifndef _BSP_PROTECTION_H_
#define _BSP_PROTECTION_H_

#ifdef __cplusplus
extern "C" {
#endif





/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "stm32f4xx_hal.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/

/********************************************************************************************************	
HW : JN-MPU-C022F 　Hardware fault signal PIN 映射关系
	
		HW fault Input signal Mapping：  
		MG1 	:PG0		MG2		:PF15
		MG3 	:PF14		MG4 	:PF13
		MG5 	:PF12		MG6 	:PF11
		MG7 	:PB1		MG8		:PB0
*硬件错误信号为，硬件检测到错误信号(模块过压、过流、过温、电池反接)输出给MCU ,MCU I/0 配置为输入，下拉，故障时候模块输出的是高电平*
	
		
*********************************************************************************************************/
#define PG8_Pin GPIO_PIN_0
#define PG8_GPIO_Port GPIOB
#define PG7_Pin GPIO_PIN_1
#define PG7_GPIO_Port GPIOB
#define PG6_Pin GPIO_PIN_11
#define PG6_GPIO_Port GPIOF
#define PG5_Pin GPIO_PIN_12
#define PG5_GPIO_Port GPIOF
#define PG4_Pin GPIO_PIN_13
#define PG4_GPIO_Port GPIOF
#define PG3_Pin GPIO_PIN_14
#define PG3_GPIO_Port GPIOF
#define PG2_Pin GPIO_PIN_15
#define PG2_GPIO_Port GPIOF
#define PG1_Pin GPIO_PIN_0
#define PG1_GPIO_Port GPIOG


#define	NORMAL	0
#define	COMPONENT_ERR1	1
#define	COMPONENT_ERR2	2
#define	COMPONENT_ERR3	3



/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	

void bsp_HW_Fault_InitGPIO(void);
uint8_t  Read_CH1_MG (void);
uint8_t  Read_CH2_MG (void);
uint8_t  Read_CH3_MG (void);
uint8_t  Read_CH4_MG (void);
void Hardware_IO_Protection(void);





#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

