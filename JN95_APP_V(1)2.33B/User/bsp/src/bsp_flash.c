/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_flash.c
** Author      : Cai Heng
** Date        : 2021-06-15
** Description : This file is used to initialize and drive the flash (W25Q16JVSSIQ)
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
extern uint32_t IP_Alarm_Flag[8];       //�ڲ�������־λ
uint8_t uwFlashRxdata[FLASH_SECTION_SIZE] = {0};
uint32_t uwSecpos = 0;
uint16_t uwSecoff = 0;
uint16_t uwSecremain = 0;
uint16_t uwPageremain;

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
uint16_t SPI_Flash_ReadID(void);
void SPI_Flash_Erase_Sector(uint32_t udwDst_Addr);
void SPI_Flash_Read(uint8_t* pBuffer,uint32_t udwReadAddr,uint16_t uwNumByteToRead);
void SPI_Flash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
uint8_t SPI_Flash_WriteWithCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : SPI_ReadWriteByte()
**	����˵�� : ʵ��SPI��д�����ֽڹ���
**	��    �� : uwTxdata - SPI����д��������
**	�� �� ֵ : SPI���߶���������
**********************************************************************************************************/
uint8_t SPI_ReadWriteByte(uint8_t uwTxdata)
{
    uint8_t Txdata = {uwTxdata};
    uint8_t Rxdata = {0x00};
    HAL_SPI_TransmitReceive(&SPI6_Handler, &Txdata, &Rxdata, 1, 1000);
    return Rxdata;
}

/**********************************************************************************************************
**	�� �� �� : SPI_FLASH_Write_Enable()
**	����˵�� : ����FLASHдʹ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void SPI_FLASH_Write_Enable(void)
{
    FLASH_CS_0();
    SPI_ReadWriteByte(cW25X_WriteEnable);
    FLASH_CS_1();
}

/**********************************************************************************************************
**	�� �� �� : SPI_Flash_ReadSR()
**	����˵�� : ��ȡFLASH״̬�Ĵ���
**	��    �� : ��
**	�� �� ֵ : FLASH״̬�Ĵ�����ֵ
**********************************************************************************************************/
uint16_t SPI_Flash_ReadSR(void)
{
    uint16_t uwStatus;
    FLASH_CS_0();
    SPI_ReadWriteByte(cW25X_ReadStatusReg);
    uwStatus = SPI_ReadWriteByte(cDummData);
    FLASH_CS_1();
    return uwStatus;
}

/**********************************************************************************************************
**	�� �� �� : SPI_Flash_Wait_Busy()
**	����˵�� : �ȴ�����æ
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void SPI_Flash_Wait_Busy(void)
{
    while((SPI_Flash_ReadSR() & 0x01) == 0x01);
}

/**********************************************************************************************************
**	�� �� �� : SPI_Flash_Erase_Chip()
**	����˵�� : ����FLASH��Ƭ����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void SPI_Flash_Erase_Chip(void)
{
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    FLASH_CS_0();
    SPI_ReadWriteByte(cW25X_ChipErase);
    FLASH_CS_1();
    SPI_Flash_Wait_Busy();
    bsp_DelayMS(1);
}

/**********************************************************************************************************
**	�� �� �� : SPI_Flash_Erase_Sector()
**	����˵�� : ����FLASH��������
**	��    �� : udwDst_Addr - ������ַ
**	�� �� ֵ : ��
**********************************************************************************************************/
void SPI_Flash_Erase_Sector(uint32_t udwDst_Addr)
{
    udwDst_Addr *= FLASH_SECTION_SIZE;
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    FLASH_CS_0();
    SPI_ReadWriteByte(cW25X_SectorErase);
    SPI_ReadWriteByte((uint8_t)(udwDst_Addr >> 16));
    SPI_ReadWriteByte((uint8_t)(udwDst_Addr >> 8 ));
    SPI_ReadWriteByte((uint8_t)(udwDst_Addr >> 0 ));
    FLASH_CS_1();
    SPI_Flash_Wait_Busy();
    bsp_DelayUS(1);
}

/**********************************************************************************************************
**	�� �� �� : SPI_Flash_ReadID()
**	����˵�� : ��ȡFLASH����ID
**	��    �� : ��
**	�� �� ֵ : FLASH����ID��ֵ
**********************************************************************************************************/
uint16_t SPI_Flash_ReadID(void)
{
    uint16_t uwFlashID;
    uint16_t uwFlashID_H;
    uint16_t uwFlashID_L;
    FLASH_CS_0();
    SPI_ReadWriteByte(cW25X_ManufactDeviceID);
    SPI_ReadWriteByte(0x00);
    SPI_ReadWriteByte(0x00);
    SPI_ReadWriteByte(0x00);
    uwFlashID_H = SPI_ReadWriteByte(cDummData);
    uwFlashID_L = SPI_ReadWriteByte(cDummData);
    uwFlashID = (uwFlashID_H << 8) + uwFlashID_L;
    FLASH_CS_1();
    return uwFlashID;
}

/**********************************************************************************************************
**	�� �� �� : SPI_Flash_Read()
**	����˵�� : ʵ��FLASH��ȡ������ݹ���
**	��    �� : *pBuffer        - ��ȡ���ݻ������׵�ַ
**             udwReadAddr     - ��ȡ�����׵�ַ
**             uwNumByteToRead - ��ȡ�����ֽ���
**	�� �� ֵ : ��
**********************************************************************************************************/
void SPI_Flash_Read(uint8_t *pBuffer, uint32_t udwReadAddr, uint16_t uwNumByteToRead)
{
    uint16_t i;
    FLASH_CS_0();
    SPI_ReadWriteByte(cW25X_ReadData);
    SPI_ReadWriteByte((uint8_t)(udwReadAddr >> 16));
    SPI_ReadWriteByte((uint8_t)(udwReadAddr >> 8 ));
    SPI_ReadWriteByte((uint8_t)(udwReadAddr >> 0 ));
    for(i = 0; i < uwNumByteToRead; i++)
    {
        pBuffer[i] = SPI_ReadWriteByte(cDummData);
    }
    FLASH_CS_1();
}

/**********************************************************************************************************
**	�� �� �� : SPI_Flash_Write_Page()
**	����˵�� : ʵ��FLASHдҳ����
**	��    �� : *pBuffer         - д���ݻ������׵�ַ
**             udwWriteAddr     - д�����׵�ַ
**             uwNumByteToWrite - д�����ֽ���
**	�� �� ֵ : ��
**********************************************************************************************************/
void SPI_Flash_Write_Page(uint8_t *pBuffer, uint32_t udwWriteAddr, uint16_t uwNumByteToWrite)
{
    uint16_t i;
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    FLASH_CS_0();
    SPI_ReadWriteByte(cW25X_PageProgram);
    SPI_ReadWriteByte((uint8_t)(udwWriteAddr >> 16));
    SPI_ReadWriteByte((uint8_t)(udwWriteAddr >> 8 ));
    SPI_ReadWriteByte((uint8_t)(udwWriteAddr >> 0 ));
    for(i = 0; i < uwNumByteToWrite; i++)
    {
        SPI_ReadWriteByte(pBuffer[i]);
    }
    FLASH_CS_1();
    SPI_Flash_Wait_Busy();
    bsp_DelayUS(1);
}

/**********************************************************************************************************
**	�� �� �� : SPI_Flash_Write_NoCheck()
**	����˵�� : ʵ��FLASHд(�����)������ݹ���
**	��    �� : *pBuffer       - д���ݻ������׵�ַ
**             WriteAddr      - д�����׵�ַ
**             NumByteToWrite - д�����ֽ���
**	�� �� ֵ : ��
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
**	�� �� �� : SPI_Flash_Write()
**	����˵�� : ʵ��FLASHд������ݹ���
**	��    �� : *pBuffer       - д���ݻ������׵�ַ
**             WriteAddr      - д�����׵�ַ
**             NumByteToWrite - д�����ֽ���
**	�� �� ֵ : ��
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
        for(i = 0; i < uwSecremain; i++)		//У������
        {
            if(uwFlashRxdata[uwSecoff + i] != 0xFF)
                break;		//��Ҫ����
        }
        if(i < uwSecremain)						//��Ҫ����
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
        if(NumByteToWrite == uwSecremain)		//д�����
        {
            break;
        }
        else									//д��δ����
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
**	�� �� �� : SPI_Flash_WriteWithCheck()
**	����˵�� : ��checkʵ��FLASHд������ݹ���
**	��    �� : *pBuffer       - д���ݻ������׵�ַ
**             WriteAddr      - д�����׵�ַ
**             NumByteToWrite - д�����ֽ���
**	�� �� ֵ : 1:�ɹ�  0��ʧ��
**********************************************************************************************************/
uint8_t SPI_Flash_WriteWithCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t i;
    uint8_t readate;

    SPI_Flash_Write(pBuffer,WriteAddr,NumByteToWrite);//д��

    for(i=0; i<NumByteToWrite; i++) //д�����ݶ���У��
    {
        SPI_Flash_Read(&readate,WriteAddr+i,1);
        if(readate!=pBuffer[i])
        {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_FLE;  //flash����
            return 0;
        }
    }
    return 1;
}
/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
