/*
*********************************************************************************************************
*
*	模块名称 :
*	文件名称 :Flash.h
*	版    本 : V1.0
*	说    明 :
*	修改记录 :
*	版本号  日期         作者       说明
*	V1.0    2020-03-05   ZYL        正式发布
*
*********************************************************************************************************
*/

#ifndef _flash_H_
#define _flash_H_

#include "stdint.h"

/*******************************************************************************
*define
*******************************************************************************/
#define cDummData               0xff
#define cW25X_WriteEnable       0x06
#define cW25X_WriteDisable      0x04
#define cW25X_ReadStatusReg     0x05
#define cW25X_WriteStatusReg    0x01
#define cW25X_ReadData          0x03
#define cW25X_FastReadData      0x0B
#define cW25X_FastReadDual      0x3B
#define cW25X_PageProgram       0x02
#define cW25X_BlockErase        0xD8
#define cW25X_SectorErase       0x20
#define cW25X_ChipErase         0xC7
#define cW25X_PowerDown         0xB9
#define cW25X_ReleasePowerDown  0xAB
#define cW25X_DeviceID          0xAB
#define cW25X_ManufactDeviceID  0x90
#define cW25X_JedecDeviceID     0x9F
/*******************************************************************************
*
*******************************************************************************/
// extern uint8_t SPI_ReadWriteByte(uint16_t uwTxdata);

void SPI_FLASH_Write_Enable(void);
uint8_t SPI_ReadWriteByte(uint8_t uwTxdata);
uint16_t SPI_Flash_ReadSR(void);
void SPI_Flash_Wait_Busy(void);
void SPI_Flash_Erase_Chip(void);
void SPI_Flash_Erase_Sector(uint32_t udwDst_Addr);
uint16_t SPI_Flash_ReadID(void);
void SPI_Flash_Read(uint8_t* pBuffer,uint32_t udwReadAddr,uint16_t uwNumByteToRead);
void SPI_Flash_Write_Page(uint8_t* pBuffer,uint32_t udwWriteAddr,uint16_t uwNumByteToWrite);
void SPI_Flash_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void SPI_Flash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);


#endif

/***************************** (END OF FILE) *********************************/
