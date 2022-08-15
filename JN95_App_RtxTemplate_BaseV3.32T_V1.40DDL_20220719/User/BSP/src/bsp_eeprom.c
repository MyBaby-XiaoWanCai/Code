/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_eeprom.c
** Author      : Huang Cheng
** Date        : 2021-11-09
** Description : 用GPIO模拟I2C总线,实现24xx系列EEPROM的读写操作,写操作采用页写模式提高写入效率
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void i2c_Delay(void);
static void i2c_Start(void);
static void i2c_Stop(void);
static void i2c_SendByte(uint8_t _ucByte);
static uint8_t i2c_ReadByte(void);
static uint8_t i2c_WaitAck(void);
static void i2c_Ack(void);
static void i2c_NAck(void);
static uint8_t i2c_CheckDevice(uint8_t _Address);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : i2c_Delay()
**  功能说明 : 实现I2C延时功能
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_Delay(void)
{
    bsp_DelayUS(2);
}

/**********************************************************************************************************
**  函 数 名 : i2c_Start()
**  功能说明 : I2C总线启动信号
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_Start(void)
{
    /* 当SCL为高电平时,SDA出现一个下跳沿表示I2C总线启动信号 */
    I2C_SDA_1();
    I2C_SCL_1();
    i2c_Delay();
    I2C_SDA_0();
    i2c_Delay();

    I2C_SCL_0();
    i2c_Delay();
}

/**********************************************************************************************************
**  函 数 名 : i2c_Stop()
**  功能说明 : I2C总线停止信号
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_Stop(void)
{
    /* 当SCL高电平时,SDA出现一个上跳沿表示I2C总线停止信号 */
    I2C_SDA_0();
    i2c_Delay();
    I2C_SCL_1();
    i2c_Delay();
    I2C_SDA_1();
    i2c_Delay();
}

/**********************************************************************************************************
**  函 数 名 : i2c_SendByte()
**  功能说明 : CPU向I2C总线设备发送单个字节数据
**  形    参 : _ucByte - 等待发送的单个字节数据
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_SendByte(uint8_t _ucByte)
{
    uint8_t i;

    /* 先发送字节的高位bit7 */
    for(i = 0; i < 8; i++)
    {
        if(_ucByte & 0x80)
        {
            I2C_SDA_1();
        }
        else
        {
            I2C_SDA_0();
        }
        i2c_Delay();
        I2C_SCL_1();
        i2c_Delay();
        I2C_SCL_0();
        I2C_SCL_0();
        if(i == 7)
        {
            I2C_SDA_1();
        }
        _ucByte <<= 1;
    }
}

/**********************************************************************************************************
**  函 数 名 : i2c_ReadByte()
**  功能说明 : CPU从I2C总线设备读取单个字节数据
**  形    参 : 无
**  返 回 值 : 读取到的单个字节数据
**********************************************************************************************************/
static uint8_t i2c_ReadByte(void)
{
    uint8_t i;
    uint8_t value;

    /* 读取到的bit1为数据的bit7 */
    value = 0;
    for(i = 0; i < 8; i++)
    {
        value <<= 1;
        I2C_SCL_1();
        i2c_Delay();
        if(I2C_SDA_READ())
        {
            value++;
        }
        I2C_SCL_0();
        i2c_Delay();
    }
    return value;
}

/**********************************************************************************************************
**  函 数 名 : i2c_WaitAck()
**  功能说明 : CPU产生一个时钟,并读取器件的ACK应答信号
**  形    参 : 无
**  返 回 值 : 0 - 正确应答
**             1 - 无器件响应
**********************************************************************************************************/
static uint8_t i2c_WaitAck(void)
{
    uint8_t ret;

    I2C_SDA_1();
    i2c_Delay();
    I2C_SCL_1();
    i2c_Delay();

    if(I2C_SDA_READ())
    {
        ret = 1;
    }
    else
    {
        ret = 0;
    }
    I2C_SCL_0();
    i2c_Delay();
    return ret;
}

/**********************************************************************************************************
**  函 数 名 : i2c_Ack()
**  功能说明 : CPU产生一个ACK信号
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_Ack(void)
{
    I2C_SDA_0();
    i2c_Delay();
    I2C_SCL_1();
    i2c_Delay();
    I2C_SCL_0();
    i2c_Delay();
    I2C_SDA_1();

    i2c_Delay();
}

/**********************************************************************************************************
**  函 数 名 : i2c_NAck()
**  功能说明 : CPU产生一个NACK信号
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_NAck(void)
{
    I2C_SDA_1();
    i2c_Delay();
    I2C_SCL_1();
    i2c_Delay();
    I2C_SCL_0();
    i2c_Delay();
}

/**********************************************************************************************************
**  函 数 名 : i2c_CheckDevice()
**  功能说明 : 检测I2C总线设备(CPU发送设备地址,然后读取设备应答来判断该设备是否存在)
**  形    参 : 无
**  返 回 值 : 0 - I2C总线正常
**             1 - I2C总线异常
**********************************************************************************************************/
static uint8_t i2c_CheckDevice(uint8_t _Address)
{
    uint8_t ucAck;

    if(I2C_SDA_READ() && I2C_SCL_READ())
    {
        i2c_Start();

        /* 发送设备地址及读写控制位(0-写,1-读) */
        i2c_SendByte(_Address | I2C_WR);
        ucAck = i2c_WaitAck();

        i2c_Stop();

        return ucAck;
    }
    return 1;
}

/**********************************************************************************************************
**  函 数 名 : bsp_InitI2C()
**  功能说明 : 配置I2C总线的GPIO,采用模拟IO的方式实现
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void bsp_InitI2C(void)
{
    stc_gpio_init_t stcGpioCfg;
    GPIO_StructInit(&stcGpioCfg);

    /* 设置模拟I2C Gpio的属性: 开漏输出等 */
    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    stcGpioCfg.u16PullUp = PIN_PU_ON;
    stcGpioCfg.u16PinOType = PIN_OTYPE_NMOS;

    /* 写入模拟I2C Gpio的属性 */
    (void)GPIO_Init(I2C_SCL_GPIO, I2C_SCL_PIN, &stcGpioCfg);
    (void)GPIO_Init(I2C_SDA_GPIO, I2C_SDA_PIN, &stcGpioCfg);

    /* 给一个停止信号,复位I2C总线上的所有设备到待机模式 */
    i2c_Stop();
}

/**********************************************************************************************************
**  函 数 名 : ee_CheckOk()
**  功能说明 : 判断EERPOM是否正常
**  形    参 : 无
**  返 回 值 : 0 - 异常
**             1 - 正常
**********************************************************************************************************/
uint8_t ee_CheckOk(void)
{
    if(i2c_CheckDevice(EE_DEV_ADDR) == 0)
    {
        return 1;
    }
    else
    {
        i2c_Stop();     /* Necessary */
        return 0;
    }
}

#if 1
/**********************************************************************************************************
**  函 数 名 : ee_ReadSingleBytes()
**  功能说明 : 从EEPROM指定地址处读取单字节数据
**  形    参 : *_pReadBuf - 读取数据缓存区指针
**             _usAddress - 指定的读取起始地址
**  返 回 值 : 0 - 读取失败
**             1 - 读取成功
**********************************************************************************************************/
uint8_t ee_ReadSingleBytes(uint8_t *_pReadBuf, uint16_t _usAddress)
{
    /* 第1步：发起I2C总线启动信号 */
    i2c_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    i2c_SendByte(EE_DEV_ADDR | I2C_WR);

    /* 第3步：发送ACK */
    if(i2c_WaitAck() != 0)
    {
        return 0;               /* 执行失败 */
    }
    /* 第4,5步：写要读取得数据地址高低8bit*/
    i2c_SendByte(_usAddress >> 8);
    if(i2c_WaitAck() != 0)
    {
        return 0;               /* 执行失败 */
    }
    i2c_SendByte(_usAddress);
    if(i2c_WaitAck() != 0)
    {
        return 0;               /* 执行失败 */
    }
    i2c_Start();
    i2c_SendByte(EE_DEV_ADDR | I2C_RD);
    if(i2c_WaitAck() != 0)
    {
        return 0;               /* 执行失败 */
    }
    _pReadBuf[0] = i2c_ReadByte();

    i2c_Stop();
    bsp_DelayUS(25);	//10US
    return 1;               /* 执行成功 */
}

/**********************************************************************************************************
**  函 数 名 : ee_WriteSingleByte()
**  功能说明 : 向EEPROM指定地址写入单字节数据
**  形    参 : *_pWriteBuf - 写入数据缓存区指针
**             _usAddress  - 指定的写入起始地址
**
**  返 回 值 : 0 - 写入失败
**             1 - 写入成功
**********************************************************************************************************/
uint8_t ee_WriteSingleByte(uint8_t *_pWriteBuf, uint16_t _usAddress)
{

    /* 第1步：发起I2C总线启动信号 */
    i2c_Start();
    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    i2c_SendByte(EE_DEV_ADDR | I2C_WR);
    /* 第3步：发送一个时钟，判断器件是否正确应答 */
    if(i2c_WaitAck() != 0)
    {
        return 0;               /* 执行失败 */
    }
    /* 第4,5步：写数据地址高低8bit*/
    i2c_SendByte(_usAddress >> 8);
    if(i2c_WaitAck() != 0)
    {
        return 0;               /* 执行失败 */
    }
    i2c_SendByte(_usAddress);
    if(i2c_WaitAck() != 0)
    {
        return 0;               /* 执行失败 */
    }

    /* 第6步：开始写入数据 */
    i2c_SendByte(_pWriteBuf[0]);

    /* 第7步：发送ACK */
    if(i2c_WaitAck() != 0)
    {
        return 0;               /* 执行失败 */
    }

    i2c_Stop();
    bsp_DelayMS(4);         //页写入需要等待至少3ms
    return 1;               /* 执行成功 */
}

/**********************************************************************************************************
**  函 数 名 : ee_ReadBytes()
**  功能说明 : 从EEPROM指定地址处读取若干数据
**  形    参 : *_pReadBuf - 读取数据缓存区指针
**             _usAddress - 指定的读取起始地址
**             _usSize    - 读取数据长度(字节)
**  返 回 值 : 0 - 读取失败
**             1 - 读取成功
**********************************************************************************************************/
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{   
    int32_t ls, us;
    Dis_int();
    ls = osKernelLock();
    
    for (uint16_t i=0;i<_usSize;i++)
    ee_ReadSingleBytes(&_pReadBuf[i],_usAddress+i);
    
    us = osKernelUnlock();
    osKernelRestoreLock(us);
    osKernelRestoreLock(ls);
    En_int();
    bsp_DelayMS(2);
    return 1;    
}

/**********************************************************************************************************
**  函 数 名 : ee_WriteBytes()
**  功能说明 : 向EEPROM指定地址写入若干数据,采用页写操作提高写入效率
**  形    参 : *_pWriteBuf - 写入数据缓存区指针
**             _usAddress  - 指定的写入起始地址
**             _usSize     - 写入数据长度(字节)
**  返 回 值 : 0 - 写入失败
**             1 - 写入成功
**********************************************************************************************************/
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
    for (uint16_t i=0;i<_usSize;i++)
    ee_WriteSingleByte(&_pWriteBuf[i],_usAddress+i);
    return 1;
}

#endif

#if 0
/**********************************************************************************************************
**  函 数 名 : ee_ReadBytes()
**  功能说明 : 从EEPROM指定地址处读取若干数据
**  形    参 : *_pReadBuf - 读取数据缓存区指针
**             _usAddress - 指定的读取起始地址
**             _usSize    - 读取数据长度(字节)
**  返 回 值 : 0 - 读取失败
**             1 - 读取成功
**********************************************************************************************************/
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i;

    /* 采用EEPROM随即读取指令序列，连续读取若干字节 */

    /* 第1步：发起I2C总线启动信号 */
    i2c_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    i2c_SendByte(EE_DEV_ADDR | I2C_WR);

    /* 第3步：发送ACK */
    if(i2c_WaitAck() != 0)
    {
        goto cmd_fail;      /* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
    if(EE_ADDR_BYTES == 1)
    {
        i2c_SendByte((uint8_t)_usAddress);
        if(i2c_WaitAck() != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }
    }
    else
    {
        i2c_SendByte(_usAddress >> 8);
        if(i2c_WaitAck() != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }

        i2c_SendByte(_usAddress);
        if(i2c_WaitAck() != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }
    }

    /* 第6步：重新启动I2C总线，下面开始读取数据 */
    i2c_Start();

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    i2c_SendByte(EE_DEV_ADDR | I2C_RD);

    /* 第8步：发送ACK */
    if(i2c_WaitAck() != 0)
    {
        goto cmd_fail;      /* EEPROM器件无应答 */
    }

    /* 第9步：循环读取数据 */
    for(i = 0; i < _usSize; i++)
    {
        _pReadBuf[i] = i2c_ReadByte();

        /* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
        if(i != _usSize - 1)
        {
            i2c_Ack();      /* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
        }
        else
        {
            i2c_NAck();     /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        }
        bsp_DelayUS(25);
    }
    /* 发送I2C总线停止信号 */
    i2c_Stop();

    return 1;               /* 执行成功 */

    /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
cmd_fail:

    i2c_Stop();

    for(uint8_t j = 0; j < CHANNEL_CFG; j++)
    {
        IP_Alarm_Flag[j] |= IntelProtect_EEE;
    }  //ee故障

    return 0;               /* 执行失败 */
}

/**********************************************************************************************************
**  函 数 名 : ee_WriteBytes()
**  功能说明 : 向EEPROM指定地址写入若干数据,采用页写操作提高写入效率
**  形    参 : *_pWriteBuf - 写入数据缓存区指针
**             _usAddress  - 指定的写入起始地址
**             _usSize     - 写入数据长度(字节)
**  返 回 值 : 0 - 写入失败
**             1 - 写入成功
**********************************************************************************************************/
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i, m;
    uint16_t usAddr;

    usAddr = _usAddress;

    for(i = 0; i < _usSize; i++)
    {
        /* 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址 */
        if((i == 0) || (usAddr & (EE_PAGE_SIZE - 1)) == 0)
        {
            /*　第０步：发送停止信号，启动内部写操作　*/
            i2c_Stop();

            /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于10ms */
            for(m = 0; m < 10000; m++)
            {
                /* 第1步：发起I2C总线启动信号 */
                i2c_Start();

                /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
                i2c_SendByte(EE_DEV_ADDR | I2C_WR);

                /* 第3步：发送一个时钟，判断器件是否正确应答 */
                if(i2c_WaitAck() == 0)
                {
                    break;
                }
                bsp_DelayUS(1);
            }
            if(m  == 10000)
            {
                goto cmd_fail;          /* EEPROM器件写超时 */
            }

            /* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
            if(EE_ADDR_BYTES == 1)
            {
                i2c_SendByte((uint8_t)usAddr);
                if(i2c_WaitAck() != 0)
                {
                    goto cmd_fail;      /* EEPROM器件无应答 */
                }
            }
            else
            {
                i2c_SendByte(usAddr >> 8);
                if(i2c_WaitAck() != 0)
                {
                    goto cmd_fail;      /* EEPROM器件无应答 */
                }

                i2c_SendByte(usAddr);
                if(i2c_WaitAck() != 0)
                {
                    goto cmd_fail;      /* EEPROM器件无应答 */
                }
            }
        }

        /* 第6步：开始写入数据 */
        i2c_SendByte(_pWriteBuf[i]);

        /* 第7步：发送ACK */
        if(i2c_WaitAck() != 0)
        {
            goto cmd_fail;              /* EEPROM器件无应答 */
        }

        if((usAddr & (EE_PAGE_SIZE - 1))==0x3f) //低6位=1说明地址处于页边界
        {
            i2c_Stop();
            bsp_DelayMS(4);         //页写入需要等待至少3ms
        }

        usAddr++;
    }

    /* 命令执行成功，发送I2C总线停止信号 */
    i2c_Stop();
    bsp_DelayMS(4);					//页写入需要等待至少4ms

    return 1;

    /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
cmd_fail:

    i2c_Stop();

    for(uint8_t j = 0; j < CHANNEL_CFG; j++)
    {
        IP_Alarm_Flag[j] |= IntelProtect_EEE;
    }  //ee故障

    return 0;
}
#endif

/**********************************************************************************************************
**  函 数 名 : ee_WriteBytesWithCheck()
**  功能说明 : 带check向EEPROM指定地址写入若干数据,采用页写操作提高写入效率
**  形    参 : *_pWriteBuf - 写入数据缓存区指针
**             _usAddress  - 指定的写入起始地址
**             _usSize     - 写入数据长度(字节)
**  返 回 值 : 0 - 写入失败
**             1 - 写入成功
**********************************************************************************************************/
uint8_t ee_WriteBytesWithCheck(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint8_t readate;
    uint16_t i;
//    int32_t ls, us;
//    Dis_int();
//    ls = osKernelLock();
    ee_WriteBytes(_pWriteBuf, _usAddress, _usSize); //写入
//    us = osKernelUnlock();
//    osKernelRestoreLock(us);
//    osKernelRestoreLock(ls);
//    En_int();

    for(i = 0; i < _usSize; i++)
    {

        ee_ReadBytes(&readate, _usAddress + i, 1); //读出检测

        if(readate != _pWriteBuf[i])
        {
            for(uint8_t j = 0; j < CHANNEL_CFG; j++)
            {
                IP_Alarm_Flag[j] |= IntelProtect_EEE;
            }  //ee故障

            return 0;
        }
    }

    return 1;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

