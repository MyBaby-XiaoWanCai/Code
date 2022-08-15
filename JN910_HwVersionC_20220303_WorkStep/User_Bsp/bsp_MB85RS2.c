/*
*********************************************************************************************************
*
*   模块名称 : MB85RS2MTA驱动模块(富士通铁电存储器2M（256K*8）bits)
*   文件名称 : bsp_MB85RS2.c
*   版    本 : V1.0
*   说    明 : MB85RS2MTA模块和CPU之间采用SPI接口。本驱动程序使用硬件SPI。
*
*
*   修改记录 :
*       版本号  日期         作者     说明
*       V1.0    2021-08-09    WL      正式发布
*********************************************************************************************************
*/
#include "main.h"
#include "spi.h"
#include <stdlib.h>

__IO uint8_t Fram_Op=0; //铁电操作标志 1：正在操作， 0:没有操作
/*
*********************************************************************************************************
*   函 数 名: MB85Ctrl_Init
*   功能说明: IO初始化
*   形    参: 无
*   返 回 值: 无
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
*   函 数 名: SPI_ReadWriteByte
*   功能说明: SPI字节读写
*   形    参: data：主机发送数据
*   返 回 值: 从机返回数据
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
*   函 数 名: SPI_DMA_WriteByte
*   功能说明: SPI字节读写
*   形    参: data：主机发送数据, 通过DMA写入
*   返 回 值: 从机返回数据
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
*   函 数 名: SPI_DMA_WriteByte
*   功能说明: SPI字节读写
*   形    参: data：主机发送数据, 通过DMA写入
*   返 回 值: 从机返回数据
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
*   函 数 名: MB85Ctrl_Write
*   功能说明: 往FRAM指定地址写数据
*   形    参: addr：要写入的地址，buf：要写入的数据指针，len：数据长度字节
*   返 回 值: 无
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
#else                                //DMA模式
    CMD[0]=MB85RS2_CMD_WRITE;
    CMD[1]=addr_H;
    CMD[2]=addr_M;
    CMD[3]=addr_L;
    SPI_DMA_WriteByte(CMD,4);    //写指令,地址
    SPI_DMA_WriteByte(buf,len);  //写入数据采用DMA传输
#endif  
    MB85_CS_SET();  
}

/*
*********************************************************************************************************
*   函 数 名: MB85Ctrl_Read
*   功能说明: 读出FRAM指定地址数据
*   形    参: addr：要读出的地址，buf：读出的数据存储指针，len：数据长度字节
*   返 回 值: 无
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
*   函 数 名: MB85Ctrl_ReadID
*   功能说明: 读出FRAM厂家32位制造ID
*   形    参: buf：读出的数据存储指针
*   返 回 值: 无
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
*   函 数 名: MB85Ctrl_CheckWrite
*   功能说明: 往FRAM指定地址写数据,并读出检查确认写入是否正确
*   形    参: addr：要写入的地址，buf：要写入的数据指针，len：数据长度字节
*   返 回 值: 1：写入成功，0：写入失败
*********************************************************************************************************
*/
uint8_t MB85Ctrl_CheckWrite(uint32_t addr, uint8_t *buf, uint16_t len)
{
    int32_t ls, us;
    Fram_Op=1;
    /*伪代码禁止中断*/
    ls = osKernelLock();         //任务上锁，禁止切换
#if 0     /*使用malloc,free，可用以下代码*/
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
    /*不使用malloc,free，可用以下代码*/
    uint8_t date;
    uint16_t i;
    uint8_t eq = 1;
    MB85Ctrl_Write(addr, buf, len);    //写数据到铁电指定地址
    for(i = 0; i < len; i++)           //回读确认
    {
        MB85Ctrl_Read(addr + i, &date, 1);
        if(date != buf[i])
        {
            eq = 0;
            /*伪代码fram故障*/
            break;
        }
    }
    us = osKernelUnlock();    //解除任务锁
    osKernelRestoreLock(us);
    osKernelRestoreLock(ls);
	osDelay(1);
    Fram_Op=0;    
      /*伪代码使能中断*/
    return eq;
#endif
}

