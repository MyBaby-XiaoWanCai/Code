/*
*********************************************************************************************************
*
*   模块名称 : AD5245驱动模块(256位可编程电位器)
*   文件名称 : bsp_AD5245.h
*   版    本 : V1.0
*   说    明 : AD5245模块和CPU之间采用IIC接口。本驱动程序使用模拟IIC。
*
*
*   修改记录 :
*       版本号  日期         作者     说明
*       V1.0    2021-08-09   WL      正式发布
*********************************************************************************************************
*/

#ifndef _AD5245_H_
#define _AD5245_H_

#include "stdint.h"


/*写从机地址字节点定义*/
#define Write_A0_Add  0x58   //从机地址=0

/*读从机地址字节点定义*/
#define Read_A0_Add  0x59   //从机地址=0

/*指令字节定义*/
#define Normal_SET   0x00   //游标普通设置 
#define RS_SD_SET    0x60   //游标复位，关断
#define RS_SET       0x40   //游标复位
#define SD_SET       0x20   //关断

/*超时等待时间*/
#define IIC_TIMOUT   1000

//IO操作宏
#define AD5245_SDA_Pin GPIO_PIN_4
#define AD5245_SDA_GPIO_Port GPIOH
#define AD5245_SCL_Pin GPIO_PIN_5
#define AD5245_SCL_GPIO_Port GPIOH

#define IIC_SCL(state)    HAL_GPIO_WritePin(GPIOH,AD5245_SCL_Pin,(state))
#define IIC_SDA(state)    HAL_GPIO_WritePin(GPIOH,AD5245_SDA_Pin,(state))
#define	SDA_IN            GPIOH->MODER&=0xFFFFFCFF
#define	SDA_OUT           GPIOH->MODER|=1<<8
#define READ_SDA          HAL_GPIO_ReadPin(GPIOH,AD5245_SDA_Pin)

//IIC基本时序操作函数
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(uint8_t ack);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);

extern void AD5245_Init(void);
extern int AD5245_Value_Set(uint8_t addr, uint8_t value);
extern int AD5245_Vpro_Set(float Vpro);
extern uint8_t AD5245_Value_Read(uint8_t addr);
extern void AD5245_Full_RS(uint8_t addr);
extern void AD5245_Half_RS(uint8_t addr);
extern void AD5245_SD(uint8_t addr);

#endif
