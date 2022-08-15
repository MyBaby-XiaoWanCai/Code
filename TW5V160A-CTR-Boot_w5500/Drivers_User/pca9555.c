/*
*********************************************************************************************************
*
*	模块名称 : pca9555 驱动
*	文件名称 : pca9555.c
*	版    本 : V1.0
*	说    明 : 使用软件I2C实现
*
*
*
*	修改记录 :
*	版本号  日期         作者     说明
*	V1.0    2020-03-10   ZYL      正式发布
*********************************************************************************************************
*/

#include "main.h"

/*
    pca9555RSMR基本特性:
  I2C时钟最大400Khz，跑370K
    设备地址为：(0x20<<1)+0
    2、3 写1置高、写0置低
    4、5 写1翻转、写0保持
*/
//实际使用370Khz左右，根据示波器测量
#define pca9555_SCL_Time   55

/* 定义GPIO端口 */
#define pca9555_PORT_SCL    GPIOH
#define pca9555_PIN_SCL		  GPIO_PIN_4

#define pca9555_PORT_SDA    GPIOH
#define pca9555_PIN_SDA		  GPIO_PIN_5

/* 定义口线置0和置1的宏 */
#define pca9555_SCL_0()		HAL_GPIO_WritePin(pca9555_PORT_SCL, pca9555_PIN_SCL, GPIO_PIN_RESET);
#define pca9555_SCL_1()		HAL_GPIO_WritePin(pca9555_PORT_SCL, pca9555_PIN_SCL, GPIO_PIN_SET);
#define pca9555_SDA_0()		HAL_GPIO_WritePin(pca9555_PORT_SDA, pca9555_PIN_SDA, GPIO_PIN_RESET);
#define pca9555_SDA_1()		HAL_GPIO_WritePin(pca9555_PORT_SDA, pca9555_PIN_SDA, GPIO_PIN_SET);

#define pca9555_SCL_state()	(HAL_GPIO_ReadPin(pca9555_PORT_SCL, pca9555_PIN_SCL))
#define pca9555_SDA_state()	(HAL_GPIO_ReadPin(pca9555_PORT_SDA, pca9555_PIN_SDA))


/*
*********************************************************************************************************
*	函 数 名: InitI2C_pca9555
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void InitI2C_pca9555(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    /* 打开GPIO时钟 */
    __HAL_RCC_GPIOH_CLK_ENABLE(); /* 打开GPIOH时钟 */

    GPIO_Initure.Pin = pca9555_PIN_SCL | pca9555_PIN_SDA; /*引脚 */
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD; /* 设为开漏模式 */
    GPIO_Initure.Pull = GPIO_NOPULL;         /* 上下拉电阻不使能 */
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO口最大速度 */
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);     //GPIOH

    /* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
    i2c_Stop_pca9555();

    pca9555_Config();
    pca9555SetOutput(0xffff);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参:
*	返 回 值: 无
*********************************************************************************************************
*/

static void i2c_Delay_pca9555(uint32_t cnt)
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
void i2c_Start_pca9555(void)
{
    /* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
    pca9555_SDA_1();
    pca9555_SCL_1();
    i2c_Delay_pca9555(pca9555_SCL_Time);
    pca9555_SDA_0();
    i2c_Delay_pca9555(pca9555_SCL_Time);

    pca9555_SCL_0();
    i2c_Delay_pca9555(pca9555_SCL_Time);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Stop
*	功能说明: CPU发起I2C总线停止信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Stop_pca9555(void)
{
    /* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
    pca9555_SDA_0();
    pca9555_SCL_1();
    i2c_Delay_pca9555(pca9555_SCL_Time);
    pca9555_SDA_1();
    i2c_Delay_pca9555(pca9555_SCL_Time);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参:  _ucByte ： 等待发送的字节
*	返 回 值: 0正常
*********************************************************************************************************
*/
uint8_t i2c_SendByte_pca9555(uint8_t _ucByte)
{
    uint8_t i;
    uint8_t reg=0;
    pca9555_SCL_0();
    i2c_Delay_pca9555(pca9555_SCL_Time);
    /* 先发送字节的高位bit7 */
    for (i = 0; i < 8; i++)
    {
        if (_ucByte & 0x80)
        {
            pca9555_SDA_1();
        }
        else
        {
            pca9555_SDA_0();
        }
        i2c_Delay_pca9555(pca9555_SCL_Time);
        pca9555_SCL_1();
        i2c_Delay_pca9555(pca9555_SCL_Time);
        _ucByte <<= 1;	/* 左移一个bit */
        pca9555_SCL_0();
        i2c_Delay_pca9555(pca9555_SCL_Time);
    }
    reg=i2c_WaitAck_pca9555();
    return reg;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参:  无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t i2c_ReadByte_pca9555(void)
{
    uint8_t i;
    uint8_t value;

    /* 读到第1个bit为数据的bit7 */
    value = 0;
    for (i = 0; i < 8; i++)
    {
        value <<= 1;
        pca9555_SCL_1();
        i2c_Delay_pca9555(pca9555_SCL_Time);
        value |= pca9555_SDA_state();
        pca9555_SCL_0();
        i2c_Delay_pca9555(pca9555_SCL_Time);
    }
    i2c_NAck_pca9555();
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
uint8_t i2c_WaitAck_pca9555(void)
{
    uint32_t i;
    pca9555_SDA_1();	/* CPU释放SDA总线 */
    i2c_Delay_pca9555(pca9555_SCL_Time);
    pca9555_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
    i2c_Delay_pca9555(pca9555_SCL_Time);
    while(pca9555_SDA_state()) //等待EEPROM 输出低电平,4ms后退出循环
    {
        i++;
        if (i >= 180000)
        {
            i2c_Stop_pca9555();
            return(1);
        }
    }
    pca9555_SCL_0();
    i2c_Delay_pca9555(pca9555_SCL_Time);
    return 0;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Ack_pca9555(void)
{
    pca9555_SDA_0();	/* CPU驱动SDA = 0 */
    i2c_Delay_pca9555(pca9555_SCL_Time);
    pca9555_SCL_1();	/* CPU产生1个时钟 */
    i2c_Delay_pca9555(pca9555_SCL_Time);
    pca9555_SCL_0();
    i2c_Delay_pca9555(pca9555_SCL_Time);
    pca9555_SDA_1();	/* CPU释放SDA总线 */
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_NAck_pca9555(void)
{
    pca9555_SDA_1();	/* CPU驱动SDA = 1 */
    i2c_Delay_pca9555(pca9555_SCL_Time);
    pca9555_SCL_1();	/* CPU产生1个时钟 */
    i2c_Delay_pca9555(pca9555_SCL_Time);
    pca9555_SCL_0();
    i2c_Delay_pca9555(pca9555_SCL_Time);
}
/*
*********************************************************************************************************
*	函 数 名: pca9555_Config
*	功能说明: 初始化配置成输出高电平
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void pca9555_Config(void)
{
    i2c_Start_pca9555();
    i2c_SendByte_pca9555(0X40);
    i2c_SendByte_pca9555(0x06);
    i2c_SendByte_pca9555(0x00);//全部设置为输出
    i2c_SendByte_pca9555(0x00);
    i2c_Stop_pca9555();
}
/*
*********************************************************************************************************
*	函 数 名: pca9555SetOutput
*	功能说明: 设置端口输出 对应 0.7~0.0 or 1.7~1.0
*	形    参: 端口值 0：低电平 1：高电平
*	返 回 值: 无
*********************************************************************************************************
*/
void pca9555SetOutput(uint16_t DataToWrite)
{
    i2c_Start_pca9555();
    i2c_SendByte_pca9555(0X40);
    i2c_SendByte_pca9555(0x02);
    i2c_SendByte_pca9555((uint8_t)(DataToWrite>>8));
    i2c_SendByte_pca9555((uint8_t)DataToWrite);
    i2c_Stop_pca9555();
}


/*****************************(END OF FILE) *********************************/
