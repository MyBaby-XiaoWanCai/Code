/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_eeprom.c
** Author      : Cai Heng        Huang Cheng
** Date        : 2021-01-16      2022-01-17(modify)
** Description : 用GPIO模拟I2C总线,实现24xx系列EEPROM的读写操作,写操作采用页写模式提高写入效率
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_i2c.h"
#include "bsp_eeprom.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
static uint8_t I2cEeprom = 0x00;                        //I2C控制EEPROM的设备号

/* EEPROM的I2C设备参数 */
static I2C_ParamType EepromParam = 
{
	I2C1_SCL_GPIO_Port,
	I2C1_SCL_Pin,
	I2C1_SDA_GPIO_Port,
	I2C1_SDA_Pin,
	2,
};

I2C_HandleFuncType *I2C_EepromHandle = &I2C_HandleFunc; //I2C设备操作函数句柄
static I2C_UserFuncType *I2C_EEPROM = &I2C_UserFunc;    //I2C设备创建句柄

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : bsp_EepromInit()
**  功能说明 : EERPOM初始化函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void bsp_EepromInit(void)
{
	uint8_t CreateSta = 0x00;
	
	//创建拨码I2C设备
	CreateSta = I2C_EEPROM->Create(&EepromParam, &I2cEeprom);
	
	//创建设备成功
	if(I2C_DEVICE_OK == CreateSta)
	{
		/* 给一个停止信号,复位I2C总线上的所有设备到待机模式 */
		I2C_EepromHandle->Stop(I2cEeprom);
	}
	else
	{
		//do nothing;
	}
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
    if(I2C_EepromHandle->CheckDevice(I2cEeprom, EE_DEV_ADDR) == 0)
    {
        return 1;
    }
    else
    {
        I2C_EepromHandle->Stop(I2cEeprom);     /* Necessary */
        return 0;
    }
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
    uint16_t i;

    /* 采用EEPROM随即读取指令序列，连续读取若干字节 */

    /* 第1步：发起I2C总线启动信号 */
    I2C_EepromHandle->Start(I2cEeprom);

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    I2C_EepromHandle->SendByte(I2cEeprom, EE_DEV_ADDR|I2C_WR);

    /* 第3步：发送ACK */
    if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
    {
        goto cmd_fail;      /* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
    if(EE_ADDR_BYTES == 1)
    {
        I2C_EepromHandle->SendByte(I2cEeprom, (uint8_t)_usAddress);
        if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }
    }
    else
    {
        I2C_EepromHandle->SendByte(I2cEeprom, (_usAddress>>8));
        if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }

        I2C_EepromHandle->SendByte(I2cEeprom, _usAddress);
        if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }
    }

    /* 第6步：重新启动I2C总线，下面开始读取数据 */
    I2C_EepromHandle->Start(I2cEeprom);

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    I2C_EepromHandle->SendByte(I2cEeprom, EE_DEV_ADDR|I2C_RD);

    /* 第8步：发送ACK */
    if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
    {
        goto cmd_fail;      /* EEPROM器件无应答 */
    }

    /* 第9步：循环读取数据 */
    for(i = 0; i < _usSize; i++)
    {
        _pReadBuf[i] = I2C_EepromHandle->ReadByte(I2cEeprom);

        /* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
        if(i != _usSize - 1)
        {
            I2C_EepromHandle->Ack(I2cEeprom);      /* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
        }
        else
        {
            I2C_EepromHandle->NAck(I2cEeprom);     /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        }
    }
    /* 发送I2C总线停止信号 */
    I2C_EepromHandle->Stop(I2cEeprom);

    return 1;               /* 执行成功 */

    /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
cmd_fail:

    I2C_EepromHandle->Stop(I2cEeprom);

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
            I2C_EepromHandle->Stop(I2cEeprom);

            /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于10ms */
            for(m = 0; m < 1000; m++)
            {
                /* 第1步：发起I2C总线启动信号 */
                I2C_EepromHandle->Start(I2cEeprom);

                /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
                I2C_EepromHandle->SendByte(I2cEeprom, EE_DEV_ADDR|I2C_WR);

                /* 第3步：发送一个时钟，判断器件是否正确应答 */
                if(I2C_EepromHandle->WaitAck(I2cEeprom) == 0)
                {
                    break;
                }
            }
            if(m  == 1000)
            {
                goto cmd_fail;          /* EEPROM器件写超时 */
            }

            /* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
            if(EE_ADDR_BYTES == 1)
            {
                I2C_EepromHandle->SendByte(I2cEeprom, (uint8_t)usAddr);
                if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
                {
                    goto cmd_fail;      /* EEPROM器件无应答 */
                }
            }
            else
            {
                I2C_EepromHandle->SendByte(I2cEeprom, (usAddr >> 8));
                if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
                {
                    goto cmd_fail;      /* EEPROM器件无应答 */
                }

                I2C_EepromHandle->SendByte(I2cEeprom, usAddr);
                if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
                {
                    goto cmd_fail;      /* EEPROM器件无应答 */
                }
            }
        }

        /* 第6步：开始写入数据 */
        I2C_EepromHandle->SendByte(I2cEeprom, _pWriteBuf[i]);

        /* 第7步：发送ACK */
        if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
        {
            goto cmd_fail;              /* EEPROM器件无应答 */
        }

        usAddr++;
    }

    /* 命令执行成功，发送I2C总线停止信号 */
    I2C_EepromHandle->Stop(I2cEeprom);

    /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于10ms */
    for(m = 0; m < 1000; m++)
    {
        /* 第1步：发起I2C总线启动信号 */
        I2C_EepromHandle->Start(I2cEeprom);

        /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
#if EE_ADDR_A8 == 1
        i2c_SendByte(EE_DEV_ADDR | I2C_WR | ((_usAddress >> 7) & 0x0E));
#else
        I2C_EepromHandle->SendByte(I2cEeprom, EE_DEV_ADDR|I2C_WR);
#endif

        /* 第3步：发送一个时钟，判断器件是否正确应答 */
        if(I2C_EepromHandle->WaitAck(I2cEeprom) == 0)
        {
            break;
        }
    }
    if(m  == 1000)
    {
        goto cmd_fail;                  /* EEPROM器件写超时 */
    }

    /* 命令执行成功，发送I2C总线停止信号 */
    I2C_EepromHandle->Stop(I2cEeprom);

    return 1;

    /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
cmd_fail:

    I2C_EepromHandle->Stop(I2cEeprom);

    return 0;
}

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

    ee_WriteBytes(_pWriteBuf, _usAddress, _usSize); //写入

    for(i = 0; i < _usSize; i++)
    {
 
        ee_ReadBytes(&readate, _usAddress + i, 1); //读出检测
    }

    return 1;
}



/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

