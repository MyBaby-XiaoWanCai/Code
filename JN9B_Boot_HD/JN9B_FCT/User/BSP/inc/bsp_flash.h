/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_flash.h
** Author      : Huang Cheng 
** Date        : 2021-11-08
** Description : bsp_flash head file
**********************************************************************************************************/

#ifndef _BSP_FLASH_H_
#define _BSP_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include <stdint.h>
#include "hc32_ddl.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
//#define FLASH_test

/* W25Q16JVSSIQ�Ļ������� */
#define FLASH_SECTION_SIZE			4096						// 1������=4KB                   (Configurable)
#define FLASH_BLOCK_SIZE			16 * FLASH_SECTION_SIZE		// 1����=16������                (Configurable)
#define FLASH_SIZE					32 * FLASH_BLOCK_SIZE		// 1ƬFLASH=32����=512������=2MB (Configurable)
#define FLASH_MAX_CLK				133000000					// FLASH���ʱ������=133MHz      (Configurable)
/* W25Q16JVSSIQ�Ĵ�����ַ */
#define cDummData					0xff
#define cW25X_WriteEnable			0x06
#define cW25X_WriteDisable			0x04
#define cW25X_ReadStatusReg			0x05
#define cW25X_WriteStatusReg		0x01
#define cW25X_ReadData				0x03
#define cW25X_FastReadData			0x0B
#define cW25X_FastReadDual			0x3B
#define cW25X_PageProgram			0x02
#define cW25X_BlockErase			0xD8
#define cW25X_SectorErase			0x20
#define cW25X_ChipErase				0xC7
#define cW25X_PowerDown				0xB9
#define cW25X_ReleasePowerDown		0xAB
#define cW25X_DeviceID				0xAB
#define cW25X_ManufactDeviceID		0x90
#define cW25X_JedecDeviceID			0x9F

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/


/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
uint16_t SPI_Flash_ReadID(void);
void SPI_Flash_Read(uint8_t* pBuffer,uint32_t udwReadAddr,uint16_t uwNumByteToRead);
void SPI_Flash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
uint8_t SPI_Flash_WriteWithCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_Flash_Erase_Sector(uint32_t udwDst_Addr);
void SPI_Flash_Erase_Chip(void);	

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
