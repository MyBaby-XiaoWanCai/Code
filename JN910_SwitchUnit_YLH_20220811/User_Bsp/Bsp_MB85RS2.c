/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Bsp_MB85RS2.c
** Author      : Cai Heng
** Date        : 2022-07-07
** Description : MB85RS2����ģ��
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Bsp_MB85RS2.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/


/*********************************************************************************************************
*   �� �� ��: SPI_ReadWriteByte
*   ����˵��: SPI�ֽڶ�д
*   ��    ��: data��������������
*   �� �� ֵ: �ӻ���������
*********************************************************************************************************/
static uint8_t SPI_ReadWriteByte(uint8_t data)
{
    uint8_t Rx_data = 0;
    HAL_SPI_TransmitReceive(&hspi3, &data, &Rx_data, 1, 0xFF);
    bsp_DelayUS(1);
    return Rx_data;
}

/*********************************************************************************************************
*   �� �� ��: MB85Ctrl_Write
*   ����˵��: ��FRAMָ����ַд����
*   ��    ��: addr��Ҫд��ĵ�ַ��buf��Ҫд�������ָ�룬len�����ݳ����ֽ�
*   �� �� ֵ: ��
*********************************************************************************************************/
void MB85Ctrl_Write(uint32_t addr, uint8_t *buf, uint16_t len)
{
    uint8_t addr_H, addr_M, addr_L;

    addr_H = (uint8_t)((addr & 0xff0000) >> 16);
    addr_M = (uint8_t)((addr & 0xff00) >> 8);
    addr_L = (uint8_t)(addr & 0x00ff);

    MB85R_NSS_0();
    SPI_ReadWriteByte(MB85RS2_CMD_WREN);
    MB85R_NSS_1();

    bsp_DelayUS(1);
    MB85R_NSS_0();
    SPI_ReadWriteByte(MB85RS2_CMD_WRITE);
    SPI_ReadWriteByte(addr_H);
    SPI_ReadWriteByte(addr_M);
    SPI_ReadWriteByte(addr_L);

    for(; len > 0; len--)
    {
        SPI_ReadWriteByte(*buf++);
    }
    MB85R_NSS_1();
}

/*********************************************************************************************************
*   �� �� ��: MB85Ctrl_Read
*   ����˵��: ����FRAMָ����ַ����
*   ��    ��: addr��Ҫ�����ĵ�ַ��buf�����������ݴ洢ָ�룬len�����ݳ����ֽ�
*   �� �� ֵ: ��
*********************************************************************************************************/
void MB85Ctrl_Read(uint32_t addr, uint8_t *buf, uint16_t len)
{
    uint8_t addr_H, addr_M, addr_L;

    addr_H = (uint8_t)((addr & 0xff0000) >> 16);
    addr_M = (uint8_t)((addr & 0xff00) >> 8);
    addr_L = (uint8_t)(addr & 0x00ff);

    MB85R_NSS_0();
    SPI_ReadWriteByte(MB85RS2_CMD_READ);
    SPI_ReadWriteByte(addr_H);
    SPI_ReadWriteByte(addr_M);
    SPI_ReadWriteByte(addr_L);

    for(; len > 0; len--)
    {
        *buf++ = SPI_ReadWriteByte(0x00);
    }
    MB85R_NSS_1();
}

/*********************************************************************************************************
*   �� �� ��: MB85Ctrl_ReadID
*   ����˵��: ����FRAM����32λ����ID
*   ��    ��: buf�����������ݴ洢ָ��
*   �� �� ֵ: ��
*********************************************************************************************************/
void MB85Ctrl_ReadID(uint32_t *buf)
{
    uint8_t i;
    uint8_t bytebuf[4]= {0};

    MB85R_NSS_0();
    SPI_ReadWriteByte(MB85RS2_CMD_RDID);
    for(i=0; i < 4; i++)
    {
        bytebuf[i] = SPI_ReadWriteByte(0x00);
    }
    *buf=(bytebuf[0]<<24)+(bytebuf[1]<<16)+(bytebuf[2]<<8)+(bytebuf[3]);
    MB85R_NSS_1();
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/