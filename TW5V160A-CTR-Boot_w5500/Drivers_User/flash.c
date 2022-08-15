/*
*********************************************************************************************************
*
*	ģ������ : W25Q16JVSSIQ ����
*	�ļ����� : flash.c
*	��    �� : V1.0
*	˵    �� : ʹ��Ӳ��SPIʵ��
*
*
*
*	�޸ļ�¼ :
*	�汾��  ����         ����     ˵��
*	V1.0    2020-03-05   ZYL      ��ʽ����
*********************************************************************************************************
*/

#include "main.h"

/*
    W25Q16JVSSIQ��������:
1��4KbyteΪһ������sec
2��16������Ϊһ��BLOCK
3������Ϊ2M�ֽ�byte������32block��512������
4�����ʱ������133Mhz
*/

/* ����GPIO�˿� */
#define flash_PORT_CS   GPIOG
#define flash_PIN_CS    GPIO_PIN_11

/* ���������0����1�ĺ� */
#define flash_CS_0()    HAL_GPIO_WritePin(flash_PORT_CS, flash_PIN_CS, GPIO_PIN_RESET);
#define flash_CS_1()    HAL_GPIO_WritePin(flash_PORT_CS, flash_PIN_CS, GPIO_PIN_SET);

uint8_t uwFlashRxdata[4096]={0};
uint32_t uwSecpos = 0;
uint16_t uwSecoff = 0;
uint16_t uwSecremain = 0;
uint16_t uwPageremain;

/*
*********************************************************************************************************
*	�� �� ��: InitI2C
*	����˵��: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void InitFLASH(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	/* ��GPIOʱ�� */
	__HAL_RCC_GPIOG_CLK_ENABLE(); /* ��GPIOGʱ�� */
	
	GPIO_Initure.Pin = flash_PIN_CS; /*���� */
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* ��Ϊ����ģʽ */
	GPIO_Initure.Pull = GPIO_NOPULL;         /* ���������費ʹ�� */
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO������ٶ� */
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

//����ѭ����ѯ�ķ�ʽ�ȴ������ڲ�д�������
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
        for(i=0;i<uwSecremain;i++)  //У������
        {
            if(uwFlashRxdata[uwSecoff+i]!=0XFF)break;   //��Ҫ����
        }
        if(i<uwSecremain)   //��Ҫ����
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
        if(NumByteToWrite == uwSecremain) {     //д�����
            break;
        } else {        //д��δ����
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
