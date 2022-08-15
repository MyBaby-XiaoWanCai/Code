/*
*********************************************************************************************************
*
*	模块名称 : W25Q16JVSSIQ 驱动
*	文件名称 : flash.c
*	版    本 : V1.0
*	说    明 : 使用硬件SPI实现
*
*
*
*	修改记录 :
*	版本号  日期         作者     说明
*	V1.0    2020-03-05   ZYL      正式发布
*********************************************************************************************************
*/

#include "main.h"

/*
    W25Q16JVSSIQ基本特性:
1、4Kbyte为一个扇区sec
2、16个扇区为一个BLOCK
3、容量为2M字节byte，共有32block，512个扇区
4、最大时钟速率133Mhz
*/

/* 定义GPIO端口 */
#define flash_PORT_CS   GPIOG
#define flash_PIN_CS    GPIO_PIN_11

/* 定义口线置0和置1的宏 */
#define flash_CS_0()    HAL_GPIO_WritePin(flash_PORT_CS, flash_PIN_CS, GPIO_PIN_RESET);
#define flash_CS_1()    HAL_GPIO_WritePin(flash_PORT_CS, flash_PIN_CS, GPIO_PIN_SET);

uint8_t uwFlashRxdata[4096]={0};
uint32_t uwSecpos = 0;
uint16_t uwSecoff = 0;
uint16_t uwSecremain = 0;
uint16_t uwPageremain;

/*
*********************************************************************************************************
*	函 数 名: InitI2C
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void InitFLASH(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	/* 打开GPIO时钟 */
	__HAL_RCC_GPIOG_CLK_ENABLE(); /* 打开GPIOG时钟 */
	
	GPIO_Initure.Pin = flash_PIN_CS; /*引脚 */
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* 设为推挽模式 */
	GPIO_Initure.Pull = GPIO_NOPULL;         /* 上下拉电阻不使能 */
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO口最大速度 */
	HAL_GPIO_Init(flash_PORT_CS, &GPIO_Initure);//GPIOG
	flash_CS_1();
}

void SPI_FLASH_Write_Enable(void)
{
    flash_CS_0();
    SPI_ReadWriteByte(cW25X_WriteEnable);
    flash_CS_1();
}
uint8_t SPI_ReadWriteByte(uint8_t uwTxdata)
{
    uint8_t Txdata[1] = {uwTxdata};
    uint8_t Rxdata[1] = {0X00};
    HAL_SPI_TransmitReceive(&hspi6, Txdata, Rxdata, 1,1000);
    return Rxdata[0];
}

//采用循环查询的方式等待器件内部写操作完成
uint16_t SPI_Flash_ReadSR(void)
{
    uint16_t uwStatus;
    flash_CS_0();
    SPI_ReadWriteByte(cW25X_ReadStatusReg);
    uwStatus = SPI_ReadWriteByte(cDummData);
    flash_CS_1();
    return uwStatus;
}
void SPI_Flash_Wait_Busy(void)
{
    while((SPI_Flash_ReadSR()&0x01)==0x01);
}
/////////////////////////////////////////////
void SPI_Flash_Erase_Chip(void)
{
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    flash_CS_0();
    SPI_ReadWriteByte(cW25X_ChipErase);
    flash_CS_1();
    SPI_Flash_Wait_Busy();
}
void SPI_Flash_Erase_Sector(uint32_t udwDst_Addr)
{
    udwDst_Addr*=4096;
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    flash_CS_0();
    SPI_ReadWriteByte(cW25X_SectorErase);
    SPI_ReadWriteByte((uint8_t)(udwDst_Addr>>16));
    SPI_ReadWriteByte((uint8_t)(udwDst_Addr>>8));
    SPI_ReadWriteByte((uint8_t)udwDst_Addr);
    flash_CS_1();
    SPI_Flash_Wait_Busy();
}
///////////////////////////////////////////////////
uint16_t SPI_Flash_ReadID(void)
{
    uint16_t uwFlashID;
    uint16_t uwFlashID_H;
    uint16_t uwFlashID_L;
    flash_CS_0();
    SPI_ReadWriteByte(cW25X_ManufactDeviceID);
    SPI_ReadWriteByte(0x00);
    SPI_ReadWriteByte(0x00);
    SPI_ReadWriteByte(0x00);
    uwFlashID_H = SPI_ReadWriteByte(cDummData);
    uwFlashID_L = SPI_ReadWriteByte(cDummData);
    uwFlashID = (uwFlashID_H<<8)+uwFlashID_L;
    flash_CS_1();
    return uwFlashID;
}
//---------------------------------------
void SPI_Flash_Read(uint8_t* pBuffer,uint32_t udwReadAddr,uint16_t uwNumByteToRead)
{
    uint16_t i;
    flash_CS_0();
    SPI_ReadWriteByte(cW25X_ReadData);
    SPI_ReadWriteByte((uint8_t)((udwReadAddr)>>16));
    SPI_ReadWriteByte((uint8_t)((udwReadAddr)>>8));
    SPI_ReadWriteByte((uint8_t)udwReadAddr);
    for(i=0;i<uwNumByteToRead;i++)
    {
        pBuffer[i] = SPI_ReadWriteByte(cDummData);
    }
    flash_CS_1();
}
//-------------------------------------------
void SPI_Flash_Write_Page(uint8_t* pBuffer,uint32_t udwWriteAddr,uint16_t uwNumByteToWrite)
{
    uint16_t i;
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    flash_CS_0();
    SPI_ReadWriteByte(cW25X_PageProgram);
    SPI_ReadWriteByte((uint8_t)(udwWriteAddr>>16));
    SPI_ReadWriteByte((uint8_t)(udwWriteAddr>>8));
    SPI_ReadWriteByte((uint8_t)udwWriteAddr);
    for(i=0;i<uwNumByteToWrite;i++)
    {
        SPI_ReadWriteByte(pBuffer[i]);
    }
    flash_CS_1();
    SPI_Flash_Wait_Busy();
}
void SPI_Flash_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    uwPageremain=256-WriteAddr%256;
    if(NumByteToWrite<=uwPageremain)
    {
        uwPageremain=NumByteToWrite;
    }
    while(1)
    {
        SPI_Flash_Write_Page(pBuffer,WriteAddr,uwPageremain);
        if(NumByteToWrite==uwPageremain)
        {
            break;
        }
        else
        {
            pBuffer+=uwPageremain;
            WriteAddr+=uwPageremain;
            NumByteToWrite-=uwPageremain;
            if(NumByteToWrite>256)
            {
                uwPageremain=256;
            }
            else
            {
                uwPageremain=NumByteToWrite;
            }
        }

    }
}

void SPI_Flash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    uint16_t i;

    uwSecpos=WriteAddr/4096;
    uwSecoff=WriteAddr%4096;
    uwSecremain=4096-uwSecoff;
    if(NumByteToWrite<=uwSecremain)
    {
        uwSecremain=NumByteToWrite;
    }
    while(1) {
        SPI_Flash_Read(uwFlashRxdata,uwSecpos*4096,4096);
        for(i=0;i<uwSecremain;i++)  //校验数据
        {
            if(uwFlashRxdata[uwSecoff+i]!=0XFF)break;   //需要擦除
        }
        if(i<uwSecremain)   //需要擦除
        {
            SPI_Flash_Erase_Sector(uwSecpos);
            for(i=0;i<uwSecremain;i++)
            {
                uwFlashRxdata[i+uwSecoff]=pBuffer[i];
            }
            SPI_Flash_Write_NoCheck(uwFlashRxdata,uwSecpos*4096,4096);
        }
        else
        {
            SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,uwSecremain);
        }
        if(NumByteToWrite == uwSecremain) {     //写入结束
            break;
        } else {        //写入未结束
            uwSecpos++;
            uwSecoff = 0;
            pBuffer += uwSecremain;
            WriteAddr += uwSecremain;
            NumByteToWrite -= uwSecremain;
            if(NumByteToWrite > 4096) {
                uwSecremain = 4096;
            } else {
                uwSecremain = NumByteToWrite;
            }
        }
    }



}
/*****************************(END OF FILE) *********************************/
