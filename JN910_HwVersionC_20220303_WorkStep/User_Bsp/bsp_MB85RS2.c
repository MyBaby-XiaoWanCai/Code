/*
*********************************************************************************************************
*
*   ģ������ : MB85RS2MTA����ģ��(��ʿͨ����洢��2M��256K*8��bits)
*   �ļ����� : bsp_MB85RS2.c
*   ��    �� : V1.0
*   ˵    �� : MB85RS2MTAģ���CPU֮�����SPI�ӿڡ�����������ʹ��Ӳ��SPI��
*
*
*   �޸ļ�¼ :
*       �汾��  ����         ����     ˵��
*       V1.0    2021-08-09    WL      ��ʽ����
*********************************************************************************************************
*/
#include "main.h"
#include "spi.h"
#include <stdlib.h>

__IO uint8_t Fram_Op=0; //���������־ 1�����ڲ����� 0:û�в���
/*
*********************************************************************************************************
*   �� �� ��: MB85Ctrl_Init
*   ����˵��: IO��ʼ��
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void MB85Ctrl_Init(void)
{
    MX_SPI3_Init();		                            
    MB85_CS_SET();                                  
    __HAL_SPI_ENABLE(&hspi3);                       
}
/*
*********************************************************************************************************
*   �� �� ��: SPI_ReadWriteByte
*   ����˵��: SPI�ֽڶ�д
*   ��    ��: data��������������
*   �� �� ֵ: �ӻ���������
*********************************************************************************************************
*/
static uint8_t SPI_ReadWriteByte(uint8_t data)
{
    uint8_t Rx_data = 0;
    HAL_SPI_TransmitReceive(&hspi3, &data, &Rx_data, 1, 0xff);
   
    bsp_DelayUS(1);    
    return Rx_data;
}


/*
*********************************************************************************************************
*   �� �� ��: SPI_DMA_WriteByte
*   ����˵��: SPI�ֽڶ�д
*   ��    ��: data��������������, ͨ��DMAд��
*   �� �� ֵ: �ӻ���������
*********************************************************************************************************
*/
static uint8_t SPI_DMA_ReadWriteByte(uint8_t *data,uint16_t Size)
{
    uint8_t Rx_data = 0; 
    HAL_SPI_TransmitReceive_DMA(&hspi3,data, &Rx_data, Size);
    bsp_DelayUS(1); 
    return Rx_data;
}

/*
*********************************************************************************************************
*   �� �� ��: SPI_DMA_WriteByte
*   ����˵��: SPI�ֽڶ�д
*   ��    ��: data��������������, ͨ��DMAд��
*   �� �� ֵ: �ӻ���������
*********************************************************************************************************
*/
static uint8_t SPI_DMA_WriteByte(uint8_t *data,uint16_t Size)
{
    uint8_t Rx_data = 0; 
    HAL_SPI_Transmit_DMA(&hspi3, data, Size);
    bsp_DelayUS(Size); 
    return Rx_data;
}
/*
*********************************************************************************************************
*   �� �� ��: MB85Ctrl_Write
*   ����˵��: ��FRAMָ����ַд����
*   ��    ��: addr��Ҫд��ĵ�ַ��buf��Ҫд�������ָ�룬len�����ݳ����ֽ�
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void MB85Ctrl_Write(uint32_t addr, uint8_t *buf, uint16_t len)
{
    uint8_t addr_H, addr_M, addr_L;
    uint8_t  CMD[4]={0};
    addr_H = (uint8_t)((addr & 0xff0000) >> 16);
    addr_M = (uint8_t)((addr & 0xff00) >> 8);
    addr_L = (uint8_t)(addr & 0x00ff);

    MB85_CS_CLR();
    SPI_ReadWriteByte(MB85RS2_CMD_WREN);
    MB85_CS_SET();
    bsp_DelayUS(1);
    MB85_CS_CLR();
    
 #ifndef Use_DMA                 
    SPI_ReadWriteByte(MB85RS2_CMD_WRITE);
    SPI_ReadWriteByte(addr_H);
    SPI_ReadWriteByte(addr_M);
    SPI_ReadWriteByte(addr_L);    
    for(; len > 0; len--)
    {
        SPI_ReadWriteByte(*buf++);
    }
#else                                //DMAģʽ
    CMD[0]=MB85RS2_CMD_WRITE;
    CMD[1]=addr_H;
    CMD[2]=addr_M;
    CMD[3]=addr_L;
    SPI_DMA_WriteByte(CMD,4);    //дָ��,��ַ
    SPI_DMA_WriteByte(buf,len);  //д�����ݲ���DMA����
#endif  
    MB85_CS_SET();  
}

/*
*********************************************************************************************************
*   �� �� ��: MB85Ctrl_Read
*   ����˵��: ����FRAMָ����ַ����
*   ��    ��: addr��Ҫ�����ĵ�ַ��buf�����������ݴ洢ָ�룬len�����ݳ����ֽ�
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void MB85Ctrl_Read(uint32_t addr, uint8_t *buf, uint16_t len)
{
    uint8_t addr_H, addr_M, addr_L;

    addr_H = (uint8_t)((addr & 0xff0000) >> 16);
    addr_M = (uint8_t)((addr & 0xff00) >> 8);
    addr_L = (uint8_t)(addr & 0x00ff);

    MB85_CS_CLR();
    SPI_ReadWriteByte(MB85RS2_CMD_READ);
    SPI_ReadWriteByte(addr_H);
    SPI_ReadWriteByte(addr_M);
    SPI_ReadWriteByte(addr_L);

    for(; len > 0; len--)
    {
        *buf++ = SPI_ReadWriteByte(0x00);
    }
    
    MB85_CS_SET();
}

/*
*********************************************************************************************************
*   �� �� ��: MB85Ctrl_ReadID
*   ����˵��: ����FRAM����32λ����ID
*   ��    ��: buf�����������ݴ洢ָ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void MB85Ctrl_ReadID(uint32_t *buf)
{
    uint8_t i;
    uint8_t bytebuf[4]= {0};
    MB85_CS_CLR();
    SPI_ReadWriteByte(MB85RS2_CMD_RDID);
    for(i=0; i < 4; i++)
    {
        bytebuf[i] = SPI_ReadWriteByte(0x00);
    }
    *buf=(bytebuf[0]<<24)+(bytebuf[1]<<16)+(bytebuf[2]<<8)+(bytebuf[3]);
    MB85_CS_SET();
}



/*
*********************************************************************************************************
*   �� �� ��: MB85Ctrl_CheckWrite
*   ����˵��: ��FRAMָ����ַд����,���������ȷ��д���Ƿ���ȷ
*   ��    ��: addr��Ҫд��ĵ�ַ��buf��Ҫд�������ָ�룬len�����ݳ����ֽ�
*   �� �� ֵ: 1��д��ɹ���0��д��ʧ��
*********************************************************************************************************
*/
uint8_t MB85Ctrl_CheckWrite(uint32_t addr, uint8_t *buf, uint16_t len)
{
    int32_t ls, us;
    Fram_Op=1;
    /*α�����ֹ�ж�*/
    ls = osKernelLock();         //������������ֹ�л�
#if 0     /*ʹ��malloc,free���������´���*/
    uint8_t *date = (uint8_t *)malloc(len);
    uint8_t i;
    uint8_t eq = 1;
    MB85Ctrl_Write(addr, buf, len);
    MB85Ctrl_Read(addr, date, len);

    for(i = 0; i < len; i++)
    {
        if(date[i] != buf[i])
        {
            eq = 0;
            break;
        }
    }
    free(date);
    return eq;
#elif 1
    /*��ʹ��malloc,free���������´���*/
    uint8_t date;
    uint16_t i;
    uint8_t eq = 1;
    MB85Ctrl_Write(addr, buf, len);    //д���ݵ�����ָ����ַ
    for(i = 0; i < len; i++)           //�ض�ȷ��
    {
        MB85Ctrl_Read(addr + i, &date, 1);
        if(date != buf[i])
        {
            eq = 0;
            /*α����fram����*/
            break;
        }
    }
    us = osKernelUnlock();    //���������
    osKernelRestoreLock(us);
    osKernelRestoreLock(ls);
	osDelay(1);
    Fram_Op=0;    
      /*α����ʹ���ж�*/
    return eq;
#endif
}

