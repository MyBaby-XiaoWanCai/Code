/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp.h
** Author      : 
** Date        : 2022-01-20
** Description : 
**********************************************************************************************************/
#ifndef _BSP_ALARM_H_
#define _BSP_ALARM_H_

#include "main.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
//功率板故障检测IO定义
#define GPIO_PORT_POWERFAULT_1	 GPIO_PORT_E
#define GPIO_PIN_POWERFAULT_1		 GPIO_PIN_03
#define POWERFAULT_1_EXINT_CH		 EXINT_CH03	//外部中断通道号
#define POWERFAULT_1_INT_SRC		 INT_PORT_EIRQ3
#define POWERFAULT_1_IRQn				 Int003_IRQn
#define PROIORITY_GROUP_1				 DDL_IRQ_PRIORITY_00

#define GPIO_PORT_POWERFAULT_2	 GPIO_PORT_E
#define GPIO_PIN_POWERFAULT_2	 	 GPIO_PIN_04
#define POWERFAULT_2_EXINT_CH		 EXINT_CH04	//外部中断通道号
#define POWERFAULT_2_INT_SRC		 INT_PORT_EIRQ4
#define POWERFAULT_2_IRQn				 Int004_IRQn
#define PROIORITY_GROUP_2				 DDL_IRQ_PRIORITY_01

#define GPIO_PORT_POWERFAULT_3	 GPIO_PORT_E
#define GPIO_PIN_POWERFAULT_3		 GPIO_PIN_05
#define POWERFAULT_3_EXINT_CH		 EXINT_CH05	//外部中断通道号
#define POWERFAULT_3_INT_SRC		 INT_PORT_EIRQ5
#define POWERFAULT_3_IRQn				 Int005_IRQn
#define PROIORITY_GROUP_3				 DDL_IRQ_PRIORITY_02

#define GPIO_PORT_POWERFAULT_4	 GPIO_PORT_E
#define GPIO_PIN_POWERFAULT_4		 GPIO_PIN_06
#define POWERFAULT_4_EXINT_CH		 EXINT_CH06	//外部中断通道号
#define POWERFAULT_4_INT_SRC		 INT_PORT_EIRQ6
#define POWERFAULT_4_IRQn				 Int006_IRQn
#define PROIORITY_GROUP_4				 DDL_IRQ_PRIORITY_07

#define GPIO_PORT_POWERFAULT_5	 GPIO_PORT_E
#define GPIO_PIN_POWERFAULT_5		 GPIO_PIN_07
#define POWERFAULT_5_EXINT_CH		 EXINT_CH07	//外部中断通道号
#define POWERFAULT_5_INT_SRC		 INT_PORT_EIRQ7
#define POWERFAULT_5_IRQn				 Int007_IRQn
#define PROIORITY_GROUP_5				 DDL_IRQ_PRIORITY_08

#define GPIO_PORT_POWERFAULT_6	 GPIO_PORT_E
#define GPIO_PIN_POWERFAULT_6		 GPIO_PIN_08
#define POWERFAULT_6_EXINT_CH		 EXINT_CH08	//外部中断通道号
#define POWERFAULT_6_INT_SRC		 INT_PORT_EIRQ8
#define POWERFAULT_6_IRQn				 Int008_IRQn
#define PROIORITY_GROUP_6				 DDL_IRQ_PRIORITY_09

#define GPIO_PORT_POWERFAULT_7	 GPIO_PORT_E
#define GPIO_PIN_POWERFAULT_7		 GPIO_PIN_09
#define POWERFAULT_7_EXINT_CH		 EXINT_CH09	//外部中断通道号
#define POWERFAULT_7_INT_SRC		 INT_PORT_EIRQ9
#define POWERFAULT_7_IRQn				 Int009_IRQn
#define PROIORITY_GROUP_7				 DDL_IRQ_PRIORITY_10

#define GPIO_PORT_POWERFAULT_8	 GPIO_PORT_E
#define GPIO_PIN_POWERFAULT_8		 GPIO_PIN_10
#define POWERFAULT_8_EXINT_CH		 EXINT_CH10	//外部中断通道号
#define POWERFAULT_8_INT_SRC		 INT_PORT_EIRQ10
#define POWERFAULT_8_IRQn				 Int011_IRQn		//Int010_IRQn 与ADC3共用了，需修改
#define PROIORITY_GROUP_8				 DDL_IRQ_PRIORITY_11

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void PowerFault1_EXINT_IrqCallback(void);
void PowerFault2_EXINT_IrqCallback(void);
void PowerFault3_EXINT_IrqCallback(void);
void PowerFault4_EXINT_IrqCallback(void);
void PowerFault5_EXINT_IrqCallback(void);
void PowerFault6_EXINT_IrqCallback(void);
void PowerFault7_EXINT_IrqCallback(void);
void PowerFault8_EXINT_IrqCallback(void);

void PowerFault_Init(void);
void PowerFaultExint_Init(uint8_t u8Port, uint16_t u16Pin,uint32_t Exint_ch,en_int_src_t Exint_src,IRQn_Type Exint_irqn,func_ptr_t Irqcallback,uint32_t Priority);
void HardwareFaultTypeRead(uint8_t CHx);

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
