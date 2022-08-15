#include "iic.h"


/* 模拟I2C操作函数注册表 */
I2C_HandleFunc I2C_HandleCb =
{
    i2c_Start,
    i2c_Stop,
    i2c_SendByte,
    i2c_Ack,
    i2c_NAck,
    i2c_ReadByte,
    i2c_WaitAck,
    i2c_CheckDevice,
};


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/*********************************************************************************************************
*   函 数 名: IOSwitch(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t dir)
*   功能说明: 切换IO输入输出状态
*   形    参: GPIO:	GPIO结构体指针
*			  GPIO_Pin: GPIO对应Pin脚
*			  dir: 0: 输入，1: 输出 
*   返 回 值: 无
*********************************************************************************************************/
void IOSwitch(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t dir)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(dir == 1)
	{
		GPIO_InitStruct.Pin = GPIO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
	}
	else if(dir == 0)
	{
		GPIO_InitStruct.Pin = GPIO_Pin;                      //IIC的SDA引脚
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;                 //输入模式
		GPIO_InitStruct.Pull = GPIO_PULLUP;                     //上拉
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);   //设置SDA口
	}
}

/**********************************************************************************************************
**  函 数 名 : i2c_Delay()
**  功能说明 : 实现I2C延时功能
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void i2c_Delay(I2C_DevType DevNum)
{
    if(DevNum == I2C_EE_TEMP)
    {
        bsp_DelayUS(2);
    }
    else
    {
        bsp_DelayUS(4);
    }
}

/**********************************************************************************************************
**  函 数 名 : i2c_Start()
**  功能说明 : I2C总线启动信号
**  形    参 : DevNum - 设备类型
**  返 回 值 : 无
**********************************************************************************************************/
void i2c_Start(I2C_DevType DevNum)
{
    /* 当SCL为高电平时,SDA出现一个下跳沿表示I2C总线启动信号 */
    switch(DevNum)
    {
		case I2C_LED:
		{
			I2C_LED_SDA_1();
			I2C_LED_SCL_1();
			i2c_Delay(DevNum);
			I2C_LED_SDA_0();
			i2c_Delay(DevNum);
			I2C_LED_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		case I2C_NCA9555:
		{
			I2C2_INT_ADDR_SDA_1();
			I2C2_INT_ADDR_SCL_1();
			i2c_Delay(DevNum);
			I2C2_INT_ADDR_SDA_0();
			i2c_Delay(DevNum);
			I2C2_INT_ADDR_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		case I2C_EE_TEMP:
		{
			I2C1_EE_TEMP_SDA_1();
			I2C1_EE_TEMP_SCL_1();
			i2c_Delay(DevNum);
			I2C1_EE_TEMP_SDA_0();
			i2c_Delay(DevNum);
			I2C1_EE_TEMP_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		default:
		{
			break;
		}
    }
}

/**********************************************************************************************************
**  函 数 名 : i2c_Stop()
**  功能说明 : I2C总线停止信号
**  形    参 : DevNum - 设备类型
**  返 回 值 : 无
**********************************************************************************************************/
void i2c_Stop(I2C_DevType DevNum)
{
    /* 当SCL高电平时,SDA出现一个上跳沿表示I2C总线停止信号 */
    switch(DevNum)
    {
    case I2C_LED:
    {
        I2C_LED_SDA_0();
        i2c_Delay(DevNum);
        I2C_LED_SCL_1();
        i2c_Delay(DevNum);
        I2C_LED_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_NCA9555:
    {
        I2C2_INT_ADDR_SDA_0();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SCL_1();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_EE_TEMP:
    {
        I2C1_EE_TEMP_SDA_0();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SCL_1();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    default:
    {
        break;
    }
    }
}

/**********************************************************************************************************
**  函 数 名 : i2c_SendByte()
**  功能说明 : CPU向I2C总线设备发送单个字节数据
**  形    参 : DevNum - 设备类型，_ucByte - 等待发送的单个字节数据
**  返 回 值 : 无
**********************************************************************************************************/
void i2c_SendByte(I2C_DevType DevNum, uint8_t _ucByte)
{
    uint8_t i;

    /* 先发送字节的高位bit7 */
    switch(DevNum)
    {
    case I2C_LED:
    {
        for(i = 0; i < 8; i++)
        {
            if(_ucByte & 0x80)
            {
                I2C_LED_SDA_1();
            }
            else
            {
                I2C_LED_SDA_0();
            }

            i2c_Delay(DevNum);
            I2C_LED_SCL_1();
            i2c_Delay(DevNum);
            I2C_LED_SCL_0();
            I2C_LED_SCL_0();

            if(i == 7)
            {
                I2C_LED_SDA_1();
            }
            _ucByte <<= 1;
        }
        break;
    }
    case I2C_NCA9555:
    {
        for(i = 0; i < 8; i++)
        {
            if(_ucByte & 0x80)
            {
                I2C2_INT_ADDR_SDA_1();
            }
            else
            {
                I2C2_INT_ADDR_SDA_0();
            }

            i2c_Delay(DevNum);
            I2C2_INT_ADDR_SCL_1();
            i2c_Delay(DevNum);
            I2C2_INT_ADDR_SCL_0();
            I2C2_INT_ADDR_SCL_0();

            if(i == 7)
            {
                I2C2_INT_ADDR_SDA_1();
            }
            _ucByte <<= 1;
        }
        break;
    }
    case I2C_EE_TEMP:
    {
		I2C1_EE_TEMP_SCL_0();
		i2c_Delay(DevNum);
        for(i = 0; i < 8; i++)
        {
            if(_ucByte & 0x80)
            {
                I2C1_EE_TEMP_SDA_1();
            }
            else
            {
                I2C1_EE_TEMP_SDA_0();
            }

            i2c_Delay(DevNum);
            I2C1_EE_TEMP_SCL_1();
            i2c_Delay(DevNum);
            I2C1_EE_TEMP_SCL_0();
            i2c_Delay(DevNum);

            _ucByte <<= 1;
        }
		i2c_WaitAck(DevNum);
        break;
    }
    default:
    {
        break;
    }
    }
}

/**********************************************************************************************************
**  函 数 名 : i2c_ReadByte()
**  功能说明 : CPU从I2C总线设备读取单个字节数据
**  形    参 : DevNum - 设备类型
**  返 回 值 : 读取到的单个字节数据
**********************************************************************************************************/
uint8_t i2c_ReadByte(I2C_DevType DevNum)
{
    uint8_t i;
    uint8_t value = 0;

    /* 读取到的bit1为数据的bit7 */
    switch(DevNum)
    {
    case I2C_LED:
    {
        I2C_LED_SDA_IN();
        for(i = 0; i < 8; i++)
        {
            value <<= 1;
            I2C_LED_SCL_1();
            i2c_Delay(DevNum);

            if(I2C_LED_SDA_READ())
            {
                value++;
            }

            I2C_LED_SCL_0();
            i2c_Delay(DevNum);
        }
        I2C_LED_SDA_OUT();
        break;
    }
    case I2C_NCA9555:
    {
        I2C2_INT_ADDR_SDA_IN();
        for(i = 0; i < 8; i++)
        {
            value <<= 1;
            I2C2_INT_ADDR_SCL_1();
            i2c_Delay(DevNum);

            if(I2C2_INT_ADDR_SDA_READ())
            {
                value++;
            }

            I2C2_INT_ADDR_SCL_0();
            i2c_Delay(DevNum);
        }
        I2C2_INT_ADDR_SDA_OUT();
        break;
    }
    case I2C_EE_TEMP:
    {
        I2C1_EE_TEMP_SDA_IN();
        for(i = 0; i < 8; i++)
        {
            value <<= 1;
            I2C1_EE_TEMP_SCL_1();
            i2c_Delay(DevNum);
            if(I2C1_EE_TEMP_SDA_READ())
            {
                value++;
            }
            I2C1_EE_TEMP_SCL_0();
            i2c_Delay(DevNum);
        }
        I2C1_EE_TEMP_SDA_OUT();
		i2c_NAck(I2C_EE_TEMP);
        break;
    }
    default:
    {
        break;
    }
    }

    return value;
}

/**********************************************************************************************************
**  函 数 名 : i2c_WaitAck()
**  功能说明 : CPU产生一个时钟,并读取器件的ACK应答信号
**  形    参 : DevNum - 设备类型
**  返 回 值 : 0 - 正确应答
**             1 - 无器件响应
**********************************************************************************************************/
uint8_t i2c_WaitAck(I2C_DevType DevNum)
{
	uint8_t ret;
	uint32_t i = 0;

	switch(DevNum)
	{
		case I2C_LED:
		{
			I2C_LED_SDA_1();
			i2c_Delay(DevNum);
			I2C_LED_SCL_1();
			i2c_Delay(DevNum);
			I2C_LED_SDA_IN();
			if(I2C_LED_SDA_READ())
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			I2C_LED_SDA_OUT();
			I2C_LED_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		case I2C_NCA9555:
		{
			I2C2_INT_ADDR_SDA_1();
			i2c_Delay(DevNum);
			I2C2_INT_ADDR_SCL_1();
			i2c_Delay(DevNum);
			I2C2_INT_ADDR_SDA_IN();
			if(I2C2_INT_ADDR_SDA_READ())
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			I2C2_INT_ADDR_SDA_OUT();
			I2C2_INT_ADDR_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		case I2C_EE_TEMP:
		{
			I2C1_EE_TEMP_SDA_1();
			i2c_Delay(DevNum);
			I2C1_EE_TEMP_SCL_1();
			i2c_Delay(DevNum);
			I2C1_EE_TEMP_SDA_IN();
			i2c_Delay(DevNum);
			while(I2C1_EE_TEMP_SDA_READ())
			{
				i2c_Delay(DevNum);
				i++;
				if (i >= 1000)
				{
					break;
				}
			}
			if(i == 1000)
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			I2C1_EE_TEMP_SDA_OUT();
			I2C1_EE_TEMP_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		default:
		{
			break;
		}
	}
	return ret;
}

/**********************************************************************************************************
**  函 数 名 : i2c_Ack()
**  功能说明 : CPU产生一个ACK信号
**  形    参 : DevNum - 设备类型
**  返 回 值 : 无
**********************************************************************************************************/
void i2c_Ack(I2C_DevType DevNum)
{
    switch(DevNum)
    {
    case I2C_LED:
    {
        I2C_LED_SDA_0();
        i2c_Delay(DevNum);
        I2C_LED_SCL_1();
        i2c_Delay(DevNum);
        I2C_LED_SCL_0();
        i2c_Delay(DevNum);
        I2C_LED_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_NCA9555:
    {
        I2C2_INT_ADDR_SDA_0();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SCL_1();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SCL_0();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_EE_TEMP:
    {
        I2C1_EE_TEMP_SDA_0();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SCL_1();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SCL_0();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    default:
    {
        break;
    }
    }
}

/**********************************************************************************************************
**  函 数 名 : i2c_NAck()
**  功能说明 : CPU产生一个NACK信号
**  形    参 : DevNum - 设备类型
**  返 回 值 : 无
**********************************************************************************************************/
void i2c_NAck(I2C_DevType DevNum)
{
    switch(DevNum)
    {
    case I2C_LED:
    {
        I2C_LED_SDA_1();
        i2c_Delay(DevNum);
        I2C_LED_SCL_1();
        i2c_Delay(DevNum);
        I2C_LED_SCL_0();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_NCA9555:
    {
        I2C2_INT_ADDR_SDA_1();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SCL_1();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SCL_0();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_EE_TEMP:
    {
        I2C1_EE_TEMP_SDA_1();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SCL_1();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SCL_0();
        i2c_Delay(DevNum);
        break;
    }
    default:
    {
        break;
    }
    }
}

/**********************************************************************************************************
**  函 数 名 : i2c_CheckDevice()
**  功能说明 : 检测I2C总线设备(CPU发送设备地址,然后读取设备应答来判断该设备是否存在)
**  形    参 : DevNum - 设备类型， _Address - 设备地址
**  返 回 值 : 0 - I2C总线正常
**             1 - I2C总线异常
**********************************************************************************************************/
uint8_t i2c_CheckDevice(I2C_DevType DevNum, uint8_t _Address)
{
    uint8_t ucAck;

    switch(DevNum)
    {
    case I2C_LED:
    {
        I2C_LED_SDA_IN();
        I2C_LED_SCL_IN();
        if(I2C_LED_SDA_READ() && I2C_LED_SCL_READ())
        {
            I2C_LED_SDA_OUT();
            I2C_LED_SCL_OUT();
            i2c_Start(I2C_LED);

            /* 发送设备地址及读写控制位(0-写,1-读) */
            i2c_SendByte(I2C_LED, _Address | I2C_WR);
            ucAck = i2c_WaitAck(I2C_LED);

            i2c_Stop(I2C_LED);

            return ucAck;
        }
        break;
    }
    case I2C_NCA9555:
    {
        I2C2_INT_ADDR_SDA_IN();
        I2C2_INT_ADDR_SCL_IN();
        if(I2C2_INT_ADDR_SDA_READ() && I2C2_INT_ADDR_SCL_READ())
        {
            I2C2_INT_ADDR_SDA_OUT();
            I2C2_INT_ADDR_SCL_OUT();
            i2c_Start(I2C_NCA9555);

            /* 发送设备地址及读写控制位(0-写,1-读) */
            i2c_SendByte(I2C_NCA9555, _Address | I2C_WR);
            ucAck = i2c_WaitAck(I2C_NCA9555);

            i2c_Stop(I2C_NCA9555);

            return ucAck;
        }
        break;
    }
    case I2C_EE_TEMP:
    {
        I2C1_EE_TEMP_SDA_IN();
        I2C1_EE_TEMP_SCL_IN();
        if(I2C1_EE_TEMP_SDA_READ() && I2C1_EE_TEMP_SCL_READ())
        {
            I2C1_EE_TEMP_SDA_OUT();
            I2C1_EE_TEMP_SCL_OUT();
            i2c_Start(I2C_EE_TEMP);

            /* 发送设备地址及读写控制位(0-写,1-读) */
            i2c_SendByte(I2C_EE_TEMP, _Address | I2C_WR);
            ucAck = i2c_WaitAck(I2C_EE_TEMP);

            i2c_Stop(I2C_EE_TEMP);

            return ucAck;
        }
        break;
    }
    default:
    {
        break;
    }
    }

    return 1;
}

/**********************************************************************************************************
**  函 数 名 : bsp_InitI2C()
**  功能说明 : 配置I2C总线的GPIO,采用模拟IO的方式实现
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/

//void bsp_InitI2C(void)
//{
//    stc_gpio_init_t stcGpioCfg;

//    /* 设置模拟I2C3 Gpio的属性: 开漏输出等 */
//    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
//    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
//    stcGpioCfg.u16PullUp = PIN_PU_ON;
//    stcGpioCfg.u16PinOType = PIN_OTYPE_NMOS;

//    /* 写入模拟I2C3 Gpio的属性 */
//    (void)GPIO_Init(LED_I2C3_SCL_GPIO, LED_I2C3_SCL_PIN, &stcGpioCfg);
//    (void)GPIO_Init(LED_I2C3_SDA_GPIO, LED_I2C3_SDA_PIN, &stcGpioCfg);

//    /* 给一个停止信号,复位I2C1总线上的所有设备到待机模式 */
//    i2c_Stop(I2C_LED);

//    /* 设置模拟I2C2 Gpio的属性: 开漏输出等 */
//    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
//    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
//    stcGpioCfg.u16PullUp = PIN_PU_ON;
//    stcGpioCfg.u16PinOType = PIN_OTYPE_NMOS;

//    /* 写入模拟I2C2 Gpio的属性 */
//    (void)GPIO_Init(I2C2_SCL_GPIO, I2C2_SCL_PIN, &stcGpioCfg);
//    (void)GPIO_Init(I2C2_SDA_GPIO, I2C2_SDA_PIN, &stcGpioCfg);

//    /* 给一个停止信号,复位I2C1总线上的所有设备到待机模式 */
//    i2c_Stop(I2C_NCA9555);

//    /* 设置模拟I2C1 Gpio的属性: 开漏输出等 */
//    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
//    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
//    stcGpioCfg.u16PullUp = PIN_PU_ON;
//    stcGpioCfg.u16PinOType = PIN_OTYPE_NMOS;

//    /* 写入模拟I2C1 Gpio的属性 */
//    (void)GPIO_Init(I2C1_SCL_GPIO, I2C1_SCL_PIN, &stcGpioCfg);
//    (void)GPIO_Init(I2C1_SDA_GPIO, I2C1_SDA_PIN, &stcGpioCfg);

//    /* 给一个停止信号,复位I2C1总线上的所有设备到待机模式 */
//    i2c_Stop(I2C_EE_TEMP);

//    //I2C2_SMBA  9548_Reset IO初始化
//    stc_gpio_init_t I2C2SMBAGpioCfg;

//    (void)GPIO_StructInit(&I2C2SMBAGpioCfg);
//    I2C2SMBAGpioCfg.u16PinState = PIN_STATE_RESET;
//    I2C2SMBAGpioCfg.u16PullUp = PIN_PU_OFF;
//    I2C2SMBAGpioCfg.u16PinDir = PIN_DIR_OUT;
//    I2C2SMBAGpioCfg.u16PinDrv = PIN_DRV_HIGH;
//    I2C2SMBAGpioCfg.u16PinOType = PIN_OTYPE_CMOS;

//    (void)GPIO_Init(I2C2_SMBA_GPIO, I2C2_SMBA_PIN, &I2C2SMBAGpioCfg);

//    GPIO_ResetPins(I2C2_SMBA_GPIO,I2C2_SMBA_PIN);

//    bsp_DelayMS(500);
//    Feed_WDG();
//}
