/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_flash.c
** Author      : Huang Cheng
** Date        : 2021-11-08
** Description : bsp_flash source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_flash.h"
#include "bsp_spi.h"
#include "bsp_dwt.h"
#include "global.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t uwFlashRxdata[FLASH_SECTION_SIZE] = {0};
uint32_t uwSecpos = 0;
uint16_t uwSecoff = 0;
uint16_t uwSecremain = 0;
uint16_t uwPageremain;

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static uint8_t SPI_ReadWriteByte(uint8_t uwTxdata);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : SPI_ReadWriteByte()
**	功能说明 : 实现SPI读写单个字节功能
**	形    参 : uwTxdata - SPI总线写操作数据
**	返 回 值 : SPI总线读操作数据
**********************************************************************************************************/
static uint8_t SPI_ReadWriteByte(uint8_t uwTxdata)
{
    uint8_t Txdata = {uwTxdata};
    uint8_t Rxdata = {0x00};
    SPI_TransmitReceive(W25Q_SPI_UNIT, &Txdata, &Rxdata, 1);
    return Rxdata;
}

/**********************************************************************************************************
**	函 数 名 : SPI_FLASH_Write_Enable()
**	功能说明 : 配置FLASH写使能
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void SPI_FLASH_Write_Enable(void)
{
    W25Q_NSS_0();
    SPI_ReadWriteByte(cW25X_WriteEnable);
    W25Q_NSS_1();
}

/**********************************************************************************************************
**	函 数 名 : SPI_Flash_ReadSR()
**	功能说明 : 读取FLASH状态寄存器
**	形    参 : 无
**	返 回 值 : FLASH状态寄存器的值
**********************************************************************************************************/
uint16_t SPI_Flash_ReadSR(void)
{
    uint16_t uwStatus;
    W25Q_NSS_0();
    SPI_ReadWriteByte(cW25X_ReadStatusReg);
    uwStatus = SPI_ReadWriteByte(cDummData);
    W25Q_NSS_1();
    return uwStatus;
}

/**********************************************************************************************************
**	函 数 名 : SPI_Flash_Wait_Busy()
**	功能说明 : 等待总线忙
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void SPI_Flash_Wait_Busy(void)
{
    while((SPI_Flash_ReadSR() & 0x01) == 0x01);
}

/**********************************************************************************************************
**	函 数 名 : SPI_Flash_Erase_Chip()
**	功能说明 : 擦除FLASH整片操作
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void SPI_Flash_Erase_Chip(void)
{
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    W25Q_NSS_0();
    SPI_ReadWriteByte(cW25X_ChipErase);
    W25Q_NSS_1();
    SPI_Flash_Wait_Busy();
    bsp_DelayUS(1);
}

/**********************************************************************************************************
**	函 数 名 : SPI_Flash_Erase_Sector()
**	功能说明 : 擦除FLASH扇区操作
**	形    参 : udwDst_Addr - 扇区地址
**	返 回 值 : 无
**********************************************************************************************************/
void SPI_Flash_Erase_Sector(uint32_t udwDst_Addr)
{
    udwDst_Addr *= FLASH_SECTION_SIZE;
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    W25Q_NSS_0();
    SPI_ReadWriteByte(cW25X_SectorErase);
    SPI_ReadWriteByte((uint8_t)(udwDst_Addr >> 16));
    SPI_ReadWriteByte((uint8_t)(udwDst_Addr >> 8 ));
    SPI_ReadWriteByte((uint8_t)(udwDst_Addr >> 0 ));
    W25Q_NSS_1();
    SPI_Flash_Wait_Busy();
    bsp_DelayUS(1);
}

/**********************************************************************************************************
**	函 数 名 : SPI_Flash_ReadID()
**	功能说明 : 读取FLASH器件ID
**	形    参 : 无
**	返 回 值 : FLASH器件ID的值
**********************************************************************************************************/
uint16_t SPI_Flash_ReadID(void)
{
    uint16_t uwFlashID;
    uint16_t uwFlashID_H;
    uint16_t uwFlashID_L;
    W25Q_NSS_0();
    SPI_ReadWriteByte(cW25X_ManufactDeviceID);
    SPI_ReadWriteByte(0x00);
    SPI_ReadWriteByte(0x00);
    SPI_ReadWriteByte(0x00);
    uwFlashID_H = SPI_ReadWriteByte(cDummData);
    uwFlashID_L = SPI_ReadWriteByte(cDummData);
    uwFlashID = (uwFlashID_H << 8) + uwFlashID_L;
    W25Q_NSS_1();
    return uwFlashID;
}

/**********************************************************************************************************
**	函 数 名 : SPI_Flash_Read()
**	功能说明 : 实现FLASH读取多个数据功能
**	形    参 : *pBuffer        - 读取数据缓存区首地址
**             udwReadAddr     - 读取数据首地址
**             uwNumByteToRead - 读取数据字节数
**	返 回 值 : 无
**********************************************************************************************************/
void SPI_Flash_Read(uint8_t *pBuffer, uint32_t udwReadAddr, uint16_t uwNumByteToRead)
{
    uint16_t i;
    W25Q_NSS_0();
    SPI_ReadWriteByte(cW25X_ReadData);
    SPI_ReadWriteByte((uint8_t)(udwReadAddr >> 16));
    SPI_ReadWriteByte((uint8_t)(udwReadAddr >> 8 ));
    SPI_ReadWriteByte((uint8_t)(udwReadAddr >> 0 ));
    for(i = 0; i < uwNumByteToRead; i++)
    {
        pBuffer[i] = SPI_ReadWriteByte(cDummData);
    }
    W25Q_NSS_1();
}

/**********************************************************************************************************
**	函 数 名 : SPI_Flash_Write_Page()
**	功能说明 : 实现FLASH写页功能
**	形    参 : *pBuffer         - 写数据缓存区首地址
**             udwWriteAddr     - 写数据首地址
**             uwNumByteToWrite - 写数据字节数
**	返 回 值 : 无
**********************************************************************************************************/
void SPI_Flash_Write_Page(uint8_t *pBuffer, uint32_t udwWriteAddr, uint16_t uwNumByteToWrite)
{
    uint16_t i;
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    W25Q_NSS_0();
    SPI_ReadWriteByte(cW25X_PageProgram);
    SPI_ReadWriteByte((uint8_t)(udwWriteAddr >> 16));
    SPI_ReadWriteByte((uint8_t)(udwWriteAddr >> 8 ));
    SPI_ReadWriteByte((uint8_t)(udwWriteAddr >> 0 ));
    for(i = 0; i < uwNumByteToWrite; i++)
    {
        SPI_ReadWriteByte(pBuffer[i]);
    }
    W25Q_NSS_1();
    SPI_Flash_Wait_Busy();
    bsp_DelayUS(1);
}

/**********************************************************************************************************
**	函 数 名 : SPI_Flash_Write_NoCheck()
**	功能说明 : 实现FLASH写(不检查)多个数据功能
**	形    参 : *pBuffer       - 写数据缓存区首地址
**             WriteAddr      - 写数据首地址
**             NumByteToWrite - 写数据字节数
**	返 回 值 : 无
**********************************************************************************************************/
void SPI_Flash_Write_NoCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uwPageremain = 256 - WriteAddr % 256;
    if(NumByteToWrite <= uwPageremain)
    {
        uwPageremain = NumByteToWrite;
    }
    while(1)
    {
        SPI_Flash_Write_Page(pBuffer, WriteAddr, uwPageremain);
        if(NumByteToWrite == uwPageremain)
        {
            break;
        }
        else
        {
            pBuffer += uwPageremain;
            WriteAddr += uwPageremain;
            NumByteToWrite -= uwPageremain;
            if(NumByteToWrite > 256)
            {
                uwPageremain = 256;
            }
            else
            {
                uwPageremain = NumByteToWrite;
            }
        }
    }
}

/**********************************************************************************************************
**	函 数 名 : SPI_Flash_Write()
**	功能说明 : 实现FLASH写多个数据功能
**	形    参 : *pBuffer       - 写数据缓存区首地址
**             WriteAddr      - 写数据首地址
**             NumByteToWrite - 写数据字节数
**	返 回 值 : 无
**********************************************************************************************************/
void SPI_Flash_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t i;

    uwSecpos = WriteAddr / FLASH_SECTION_SIZE;
    uwSecoff = WriteAddr % FLASH_SECTION_SIZE;
    uwSecremain = FLASH_SECTION_SIZE - uwSecoff;
    if(NumByteToWrite <= uwSecremain)
    {
        uwSecremain = NumByteToWrite;
    }
    while(1)
    {
        SPI_Flash_Read(uwFlashRxdata, uwSecpos * FLASH_SECTION_SIZE, FLASH_SECTION_SIZE);
        for(i = 0; i < uwSecremain; i++)		//校验数据
        {
            if(uwFlashRxdata[uwSecoff + i] != 0xFF)
                break;		//需要擦除
        }
        if(i < uwSecremain)						//需要擦除
        {
            SPI_Flash_Erase_Sector(uwSecpos);
            for(i = 0; i < uwSecremain; i++)
            {
                uwFlashRxdata[i + uwSecoff] = pBuffer[i];
            }
            SPI_Flash_Write_NoCheck(uwFlashRxdata, uwSecpos * FLASH_SECTION_SIZE, FLASH_SECTION_SIZE);
        }
        else
        {
            SPI_Flash_Write_NoCheck(pBuffer, WriteAddr, uwSecremain);
        }
        if(NumByteToWrite == uwSecremain)		//写入结束
        {
            break;
        }
        else									//写入未结束
        {
            uwSecpos++;
            uwSecoff = 0;
            pBuffer += uwSecremain;
            WriteAddr += uwSecremain;
            NumByteToWrite -= uwSecremain;
            if(NumByteToWrite > FLASH_SECTION_SIZE)
            {
                uwSecremain = FLASH_SECTION_SIZE;
            }
            else
            {
                uwSecremain = NumByteToWrite;
            }
        }
    }
}

/**********************************************************************************************************
**	函 数 名 : SPI_Flash_WriteWithCheck()
**	功能说明 : 带check实现FLASH写多个数据功能
**	形    参 : *pBuffer       - 写数据缓存区首地址
**             WriteAddr      - 写数据首地址
**             NumByteToWrite - 写数据字节数
**	返 回 值 : 1:成功  0：失败
**********************************************************************************************************/
uint8_t SPI_Flash_WriteWithCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t i;
    uint8_t readate;

    SPI_Flash_Write(pBuffer,WriteAddr,NumByteToWrite);//写入

    for(i=0; i<NumByteToWrite; i++) //写入数据读出校验
    {
        SPI_Flash_Read(&readate,WriteAddr+i,1);
        if(readate!=pBuffer[i])
        {
            for(uint8_t j=0; j<CHANNEL_CFG; j++)
            {
                IP_Alarm_Flag[j] |= IntelProtect_FLE;  //flash故障
            }

            return 0;
        }
    }
    return 1;
}
/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
