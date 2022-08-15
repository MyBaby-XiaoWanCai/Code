/*
*********************************************************************************************************
*
*	模块名称 : 24AA256T-E/ST 驱动
*	文件名称 : eeprom.c
*	版    本 : V1.0
*	说    明 : 使用软件I2C实现
*
*
*
*	修改记录 :
*	版本号  日期         作者     说明
*	V1.0    2020-03-04   ZYL      正式发布
*********************************************************************************************************
*/
#include "eeprom.h"
#include "main.h"

uint16_t Boot_Flag=0;

/*
    24AA256T-E/ST基本特性:
  400K左右即可
  实际使用370Khz左右，根据示波器测量
*/

#define SCL_Time   55

/* 定义GPIO端口 */
#define eeprom_PORT_SCL    GPIOH
#define eeprom_PIN_SCL		 GPIO_PIN_7

#define eeprom_PORT_SDA    GPIOH
#define eeprom_PIN_SDA		 GPIO_PIN_8

/* 定义口线置0和置1的宏 */
#define eeprom_SCL_0()		HAL_GPIO_WritePin(eeprom_PORT_SCL, eeprom_PIN_SCL, GPIO_PIN_RESET);
#define eeprom_SCL_1()		HAL_GPIO_WritePin(eeprom_PORT_SCL, eeprom_PIN_SCL, GPIO_PIN_SET);
#define eeprom_SDA_0()		HAL_GPIO_WritePin(eeprom_PORT_SDA, eeprom_PIN_SDA, GPIO_PIN_RESET);
#define eeprom_SDA_1()		HAL_GPIO_WritePin(eeprom_PORT_SDA, eeprom_PIN_SDA, GPIO_PIN_SET);

#define eeprom_SCL_state()	(HAL_GPIO_ReadPin(eeprom_PORT_SCL, eeprom_PIN_SCL))
#define eeprom_SDA_state()	(HAL_GPIO_ReadPin(eeprom_PORT_SDA, eeprom_PIN_SDA))


/*
*********************************************************************************************************
*	函 数 名: InitI2C
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void InitI2C(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    /* 打开GPIO时钟 */
    __HAL_RCC_GPIOH_CLK_ENABLE(); /* 打开GPIOH时钟 */

    GPIO_Initure.Pin = eeprom_PIN_SCL | eeprom_PIN_SDA; /*引脚 */
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD; /* 设为开漏模式 */
    GPIO_Initure.Pull = GPIO_NOPULL;         /* 上下拉电阻不使能 */
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO口最大速度 */
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);     //GPIOH

    /* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
    i2c_Stop();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参:
*	返 回 值: 无
*********************************************************************************************************
*/

static void i2c_Delay(uint32_t cnt)
{
    uint32_t i;

    /*　
        CPU主频180MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
        实际应用选择400KHz左右的速率即可
    */
    for (i = 0; i < cnt; i++);

}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Start(void)
{
    /* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
    eeprom_SDA_1();
    eeprom_SCL_1();
    i2c_Delay(SCL_Time);
    eeprom_SDA_0();
    i2c_Delay(SCL_Time);

    eeprom_SCL_0();
    i2c_Delay(SCL_Time);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Stop
*	功能说明: CPU发起I2C总线停止信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Stop(void)
{
    /* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
    eeprom_SDA_0();
    eeprom_SCL_1();
    i2c_Delay(SCL_Time);
    eeprom_SDA_1();
    i2c_Delay(SCL_Time);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参:  _ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
    uint8_t i;
     eeprom_SCL_0();
    i2c_Delay(SCL_Time);
    /* 先发送字节的高位bit7 */
    for (i = 0; i < 8; i++)
    {
        if (_ucByte & 0x80)
        {
            eeprom_SDA_1();
        }
        else
        {
            eeprom_SDA_0();
        }
        i2c_Delay(SCL_Time);
        eeprom_SCL_1();
        i2c_Delay(SCL_Time);
        _ucByte <<= 1;	/* 左移一个bit */
        eeprom_SCL_0();
        i2c_Delay(SCL_Time);
    }
    i2c_WaitAck();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参:  无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
    uint8_t i;
    uint8_t value;

    /* 读到第1个bit为数据的bit7 */
    value = 0;
    for (i = 0; i < 8; i++)
    {
        value <<= 1;
        eeprom_SCL_1();
        i2c_Delay(SCL_Time);
        value |= eeprom_SDA_state();
        eeprom_SCL_0();
        i2c_Delay(SCL_Time);
    }
    i2c_NAck();
    return value;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参:  无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
      uint32_t i;
    eeprom_SDA_1();	/* CPU释放SDA总线 */
    i2c_Delay(SCL_Time);
    eeprom_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
    i2c_Delay(SCL_Time);
    while(eeprom_SDA_state()) //等待EEPROM 输出低电平,4ms后退出循环
    {
        i++;
            if (i >= 180000)
            {
                i2c_Stop();
                return(1);
            }
    }
    eeprom_SCL_0();
    i2c_Delay(SCL_Time);
    return(0);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Ack(void)
{
    eeprom_SDA_0();	/* CPU驱动SDA = 0 */
    i2c_Delay(SCL_Time);
    eeprom_SCL_1();	/* CPU产生1个时钟 */
    i2c_Delay(SCL_Time);
    eeprom_SCL_0();
    i2c_Delay(SCL_Time);
    eeprom_SDA_1();	/* CPU释放SDA总线 */
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_NAck(void)
{
    eeprom_SDA_1();	/* CPU驱动SDA = 1 */
    i2c_Delay(SCL_Time);
    eeprom_SCL_1();	/* CPU产生1个时钟 */
    i2c_Delay(SCL_Time);
    eeprom_SCL_0();
    i2c_Delay(SCL_Time);
}


uint8_t EEPROM_ReadOneByte(uint16_t ReadAddr)
{
    uint8_t temp=0;
    i2c_Start();
    i2c_SendByte(0XA0);
    i2c_SendByte(ReadAddr>>8);
    i2c_SendByte(ReadAddr);
    i2c_Start();
    i2c_SendByte(0XA1);
    temp=i2c_ReadByte();
    i2c_Stop();
    i2c_Delay(100);
    return temp;
}

void EEPROM_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{
    i2c_Start();
    i2c_SendByte(0XA0);
    i2c_SendByte(WriteAddr>>8);
    i2c_SendByte(WriteAddr);
    i2c_SendByte(DataToWrite);
    i2c_Stop();
    // i2c_Delay(200022);
    HAL_Delay(5);
}

void EEPROM_WriteLenByte(uint16_t WriteAddr, uint32_t DataToWrite, uint8_t Len)
{
    u8 t;
    for(t = 0; t < Len; t++) {
        EEPROM_WriteOneByte(WriteAddr+t, (DataToWrite>>(8*t))&0xff);
    }
}

uint32_t EEPROM_ReadLenByte(uint16_t ReadAddr, uint8_t Len)
{
    u8 t;
    u32 temp;
    for(t = 0; t < Len; t++) {
        temp <<= 8;
        temp += EEPROM_ReadOneByte(ReadAddr+Len-1-t);
    }
    return temp;
}


/*****************************(END OF FILE) *********************************/
