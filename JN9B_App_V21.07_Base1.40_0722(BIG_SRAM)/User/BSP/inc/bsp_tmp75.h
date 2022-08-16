/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_tmp75.h
** Author      : Huang Cheng
** Date        : 2022-01-18
** Description : 
**********************************************************************************************************/

#ifndef _BSP_TMP75_H_
#define _BSP_TMP75_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include "main.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
//环境温度过温中断IO
#define GPIO_PORT_AMBIENTTEMP 	GPIO_PORT_E
#define GPIO_PIN_AMBIENTTEMP		GPIO_PIN_02

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#define TMP75_ADDR_MAIN		(0x90)			//设备地址，主控板TMP75地址

#define TMP_Reg     			(0x00)          //温度寄存器地址
#define CFG_Reg     			(0x01)          //配置寄存器地址
#define TL_Reg      			(0x02)          //低温限制寄存器地址
#define TH_Reg      			(0x03)          //高温限制寄存器地址

#define TMP75_ADDR_CH1		(0x90)
#define TMP75_ADDR_CH2		(0x92)
#define TMP75_ADDR_CH3		(0x94)
#define TMP75_ADDR_CH4		(0x96)
#define TMP75_ADDR_CH5		(0x98)
#define TMP75_ADDR_CH6		(0x9A)
#define TMP75_ADDR_CH7		(0x9C)
#define TMP75_ADDR_CH8		(0x9E)

extern uint8_t Tmp75AddrArr[8];

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void TmpI2C_SendData( I2C_DevType DevNum , uint8_t SlaveAddr , uint8_t Reg ,uint16_t date);
void TmpI2C_ReadData(I2C_DevType DevNum , uint8_t SlaveAddr , uint8_t Reg , uint16_t* Temp);
void bsp_Tmp75_AlertInit(void);
void bsp_Tmp75Init(I2C_DevType DevNum , uint8_t TmpI2CAddr);
float Tmp75Value_Get(I2C_DevType DevNum , uint8_t TmpI2CAddr);
void Tmp75Value_SetTH(I2C_DevType DevNum , uint8_t TmpI2CAddr ,float tempH);
void Tmp75Value_SetTL(I2C_DevType DevNum , uint8_t TmpI2CAddr ,float tempL);

float PowerTmp75Value_Get(I2C_DevType DevNum,uint8_t TmpI2CAddr,uint8_t CHx);
void SysTempSamp(uint8_t CHx);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


