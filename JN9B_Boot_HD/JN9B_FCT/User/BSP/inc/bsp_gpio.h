/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_gpio.h
** Author      : Huang Cheng
** Date        : 2021-11-04
** Description : bsp gpio head file
**********************************************************************************************************/

#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_

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
/********************************************************************************************************	
HW : JN-MPU-C022F 　Dial Switch  读取IP 地址(192.168.X.X) PIN 映射关系

FR_ADDR1	:PH6 	FR_ADDR2	:PB10 
FR_ADDR3 	:PE15	FR_ADDR4 	:PE10
FR_ADDR5 	:PE9
		
*********************************************************************************************************/
#define IP_ADDR3_END            0X03
#define IP_ADDR4_END            0X08

#define GPIO_PORT_FR_ADDR1		GPIO_PORT_H			
#define GPIO_PIN_FR_ADDR1		GPIO_PIN_06		

#define GPIO_PORT_FR_ADDR2		GPIO_PORT_B			
#define GPIO_PIN_FR_ADDR2		GPIO_PIN_10	

#define GPIO_PORT_FR_ADDR3		GPIO_PORT_E			
#define GPIO_PIN_FR_ADDR3		GPIO_PIN_15		

#define GPIO_PORT_FR_ADDR4		GPIO_PORT_E			
#define GPIO_PIN_FR_ADDR4		GPIO_PIN_10	

#define GPIO_PORT_FR_ADDR5		GPIO_PORT_E			
#define GPIO_PIN_FR_ADDR5		GPIO_PIN_09		

#define GPIO_PORT_FR_ADDR6		GPIO_PORT_E			
#define GPIO_PIN_FR_ADDR6		GPIO_PIN_08	

#define GPIO_PORT_FR_ADDR7		GPIO_PORT_E			
#define GPIO_PIN_FR_ADDR7		GPIO_PIN_07		

#define GPIO_PORT_FR_ADDR8		GPIO_PORT_G			
#define GPIO_PIN_FR_ADDR8		GPIO_PIN_01	


/********************************************************************************************************	
HW : JN-MPU-C022H

	M_LOOP映射关系	
*********************************************************************************************************/
#define GPIO_PORT_LOOP1		GPIO_PORT_A			
#define GPIO_PIN_LOOP1		GPIO_PIN_10	

#define GPIO_PORT_LOOP2		GPIO_PORT_A			
#define GPIO_PIN_LOOP2		GPIO_PIN_15	

#define GPIO_PORT_LOOP3		GPIO_PORT_I			
#define GPIO_PIN_LOOP3		GPIO_PIN_11	

#define GPIO_PORT_LOOP4		GPIO_PORT_E			
#define GPIO_PIN_LOOP4		GPIO_PIN_02	

#define GPIO_PORT_LOOP5		GPIO_PORT_C			
#define GPIO_PIN_LOOP5		GPIO_PIN_01	

#define GPIO_PORT_LOOP6		GPIO_PORT_I			
#define GPIO_PIN_LOOP6		GPIO_PIN_10	

#define GPIO_PORT_LOOP7		GPIO_PORT_A			
#define GPIO_PIN_LOOP7		GPIO_PIN_00	

#define GPIO_PORT_LOOP8		GPIO_PORT_A			
#define GPIO_PIN_LOOP8		GPIO_PIN_04	

/********************************************************************************************************	
HW : JN-MPU-C022H
HWDG: PB2
*********************************************************************************************************/
#define GPIO_PORT_HWDG      GPIO_PORT_B
#define GPIO_PIN_HWDG       GPIO_PIN_02

#define M_LOOP_ENABLE		1

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
#pragma pack(1)
typedef struct
{
	uint8_t  GpioPort;
	uint16_t GpioPin;
}LoopPortPin_TypeDef;
#pragma pack()

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
//测试
extern uint8_t W5500_IP_ADDRESS[4];
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void bsp_M_Loop_InitGPIO(void);
void Enable_Mloop(uint8_t _ch);
void Disable_Mloop(uint8_t _ch);
void bsp_FR_ADDR_InitGPIO(void);
void bsp_HWDG_InitGPIO(void);
void Feed_WDG(void);
void IP_Scanning(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


