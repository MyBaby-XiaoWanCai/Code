/*
*********************************************************************************************************
*
*	模块名称 : pca9555 驱动
*	文件名称 : pca9555.h
*	版    本 : V1.0
*	说    明 :
*	修改记录 :
*	版本号  日期         作者       说明
*	V1.0    2020-03-10   ZYL        正式发布
*
*********************************************************************************************************
*/

#ifndef _pca9555_H_
#define _pca9555_H_

#include "stdint.h"


void InitI2C_pca9555(void);
void i2c_Start_pca9555(void);
void i2c_Stop_pca9555(void);
uint8_t i2c_SendByte_pca9555(uint8_t _ucByte);
uint8_t i2c_ReadByte_pca9555(void);
uint8_t i2c_WaitAck_pca9555(void);
void i2c_Ack_pca9555(void);
void i2c_NAck_pca9555(void);


extern void pca9555SetOutput(uint16_t DataToWrite);
extern void pca9555_Config(void);


#endif

/***************************** (END OF FILE) *********************************/
