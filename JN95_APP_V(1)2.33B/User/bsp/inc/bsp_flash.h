/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_flash.h
** Author      : Cai Heng
** Date        : 2021-06-15
** Description : 
**********************************************************************************************************/

#ifndef _BSP_FLASH_H_
#define _BSP_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "stdint.h"
#include "bsp_spi.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#define bsp_InitFlash()				bsp_InitSpi6()				//FLASH初始化                    (Configurable)
#define FLASH_CS_0()				FLASH_CS_ENABLE()			//FLASH片选拉低                  (Configurable)
#define FLASH_CS_1()				FLASH_CS_DISABLE()			//FLASH片选拉高                  (Configurable)
/* W25Q16JVSSIQ的基本特性 */
#define FLASH_SECTION_SIZE			4096						// 1个扇区=4KB                   (Configurable)
#define FLASH_BLOCK_SIZE			16 * FLASH_SECTION_SIZE		// 1个块=16个扇区                (Configurable)
#define FLASH_SIZE					32 * FLASH_BLOCK_SIZE		// 1片FLASH=32个块=512个扇区=2MB (Configurable)
#define FLASH_MAX_CLK				133000000					// FLASH最大时钟速率=133MHz      (Configurable)
/* W25Q16JVSSIQ寄存器地址 */
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
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern SPI_HandleTypeDef SPI6_Handler;	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
extern void bsp_InitSpi6(void);
extern uint16_t SPI_Flash_ReadID(void);
extern void SPI_Flash_Read(uint8_t* pBuffer,uint32_t udwReadAddr,uint16_t uwNumByteToRead);
extern void SPI_Flash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
uint8_t SPI_Flash_WriteWithCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
extern void SPI_Flash_Erase_Sector(uint32_t udwDst_Addr);
extern void SPI_Flash_Erase_Chip(void);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
