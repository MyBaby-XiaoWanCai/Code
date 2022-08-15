/*
*********************************************************************************************************
*
*	模块名称 :
*	文件名称 :
*	版    本 : V1.0
*	说    明 :
*	修改记录 :
*	版本号  日期         作者       说明
*	V1.0    2020-03-04   ZYL        正式发布
*
*********************************************************************************************************
*/

#ifndef _eeprom_H_
#define _eeprom_H_

#include "stdint.h"
#include "main.h"

#define Boot_Flag_addr               0x7FFF
extern uint16_t Boot_Flag;

#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */

#define EE_DEV_ADDR			0xA0		/* 设备地址 */


void InitI2C(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);

uint8_t EEPROM_ReadOneByte(uint16_t ReadAddr);
void EEPROM_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);
void EEPROM_WriteLenByte(uint16_t WriteAddr, uint32_t DataToWrite, uint8_t Len);
uint32_t EEPROM_ReadLenByte(uint16_t ReadAddr, uint8_t Len);
#endif

/***************************** (END OF FILE) *********************************/
