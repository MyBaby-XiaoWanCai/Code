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

#ifndef _MB85RS2_H_
#define _MB85RS2_H_

#include "stdint.h"
#include "bsp_spi.h"

#define MB85RS2_CMD_WREN 		0x06      //写使能
#define MB85RS2_CMD_WRDI		0x04      //写禁止
#define MB85RS2_CMD_RDSR		0x05      //读状态寄存器
#define MB85RS2_CMD_WRSR		0x01      //写状态寄存器
#define MB85RS2_CMD_READ		0x03      //24位地址模式读数据 
#define MB85RS2_CMD_WRITE		0x02      //写数据
#define MB85RS2_CMD_FSTRD       0x0B      //32位地址模式（高位对齐，低位填0）读数据
#define MB85RS2_CMD_RDID		0x9f      //读ID，32位ID
#define MB85RS2_CMD_SLEEP		0xB9      //低功耗模式

#define MB85_CS_PIN		GPIO_PIN_15
#define MB85_CS_PORT 	GPIOB
#define MB85_CS_SET	    FRAM_CS_DISABLE()
#define MB85_CS_CLR		FRAM_CS_ENABLE()

void MB85Ctrl_Init(void);
void MB85Ctrl_Read(uint32_t addr, uint8_t *buf, uint16_t len);
void MB85Ctrl_Write(uint32_t addr, uint8_t *buf, uint16_t len);
void MB85Ctrl_ReadID(uint32_t *buf);
extern uint8_t MB85Ctrl_CheckWrite(uint32_t addr, uint8_t *buf, uint16_t len);

#define StartADD_WorkStatus	0        // 上一次断电工步状态记录，铁电起始地址（8字节）
#define StartADD_MB85RS2DeviceSN   8        // 设备SN条码编号（3字节）
#define MB85RS2_Size  0x40000          //256K Byte
//#define RemoteAdd  (MB85RS2_Size-10)             //远端服务器地址
//#define RemotePort (RemoteAdd+4)                //远端服务器端口
//#define RemoteEn   (RemoteAdd+6)                //远端服务器配置使能   

//#define MACAdd  (MB85RS2_Size-15)             //条码（MAC后3字节）存储地址
//#define MACEn   (MACAdd+3)                    //条码配置使能

#endif
