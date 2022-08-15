/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_eeprom.c
** Author      : Huang Cheng
** Date        : 2022-01-24
** Description : 用GPIO模拟I2C总线,实现24xx系列EEPROM的读写操作,写操作采用页写模式提高写入效率
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_eeprom.h"
#include "bsp_dwt.h"
#include "global.h"
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : ee_CheckOk()
**  功能说明 : 判断EERPOM是否正常
**  形    参 : 无
**  返 回 值 : 0 - 异常
**             1 - 正常
**********************************************************************************************************/
uint8_t ee_CheckOk(void)
{
    if(i2c_CheckDevice(I2C_EE_TEMP,EE_DEV_ADDR) == 0)
    {
        return 1;
    }
    else
    {
        i2c_Stop(I2C_EE_TEMP);     /* Necessary */
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
	  int32_t ls, us;
    Dis_int();
    ls = osKernelLock();
	
    uint16_t i;

    /* 采用EEPROM随即读取指令序列，连续读取若干字节 */

    /* 第1步：发起I2C总线启动信号 */
    i2c_Start(I2C_EE_TEMP);

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    i2c_SendByte(I2C_EE_TEMP, EE_DEV_ADDR | I2C_WR);

    /* 第3步：发送ACK */
    if(i2c_WaitAck(I2C_EE_TEMP) != 0)
    {
        goto cmd_fail;      /* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
    if(EE_ADDR_BYTES == 1)
    {
        i2c_SendByte(I2C_EE_TEMP,(uint8_t)_usAddress);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }
    }
    else
    {
        i2c_SendByte(I2C_EE_TEMP, _usAddress >> 8);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }

        i2c_SendByte(I2C_EE_TEMP, _usAddress);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }
    }

    /* 第6步：重新启动I2C总线，下面开始读取数据 */
    i2c_Start(I2C_EE_TEMP);

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    i2c_SendByte(I2C_EE_TEMP,EE_DEV_ADDR | I2C_RD);

    /* 第8步：发送ACK */
    if(i2c_WaitAck(I2C_EE_TEMP) != 0)
    {
        goto cmd_fail;      /* EEPROM器件无应答 */
    }

    /* 第9步：循环读取数据 */
    for(i = 0; i < _usSize; i++)
    {
        _pReadBuf[i] = i2c_ReadByte(I2C_EE_TEMP);

        /* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
        if(i != _usSize - 1)
        {
            i2c_Ack(I2C_EE_TEMP);      /* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
        }
        else
        {
            i2c_NAck(I2C_EE_TEMP);     /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        }
    }
    /* 发送I2C总线停止信号 */
    i2c_Stop(I2C_EE_TEMP);
    us = osKernelUnlock();
    osKernelRestoreLock(us);
    osKernelRestoreLock(ls);
    En_int();
		bsp_DelayMS(2);
    return 1;               /* 执行成功 */

    /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
cmd_fail:

    i2c_Stop(I2C_EE_TEMP);
	
    for(uint8_t j = 0; j < CHANNEL_CFG; j++)
    { 
			IP_Alarm_Flag[j] |= IntelProtect_EEE; 
		}  //ee故障
	  us = osKernelUnlock();
    osKernelRestoreLock(us);
    osKernelRestoreLock(ls);
    En_int();
		bsp_DelayMS(2);
		return 0;               /* 执行失败 */
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
uint8_t ee_ReadBytes_wc(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
	
    uint16_t i;

    /* 采用EEPROM随即读取指令序列，连续读取若干字节 */

    /* 第1步：发起I2C总线启动信号 */
    i2c_Start(I2C_EE_TEMP);

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    i2c_SendByte(I2C_EE_TEMP, EE_DEV_ADDR | I2C_WR);

    /* 第3步：发送ACK */
    if(i2c_WaitAck(I2C_EE_TEMP) != 0)
    {
        goto cmd_fail;      /* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
    if(EE_ADDR_BYTES == 1)
    {
        i2c_SendByte(I2C_EE_TEMP,(uint8_t)_usAddress);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }
    }
    else
    {
        i2c_SendByte(I2C_EE_TEMP, _usAddress >> 8);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }

        i2c_SendByte(I2C_EE_TEMP, _usAddress);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }
    }

    /* 第6步：重新启动I2C总线，下面开始读取数据 */
    i2c_Start(I2C_EE_TEMP);

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    i2c_SendByte(I2C_EE_TEMP,EE_DEV_ADDR | I2C_RD);

    /* 第8步：发送ACK */
    if(i2c_WaitAck(I2C_EE_TEMP) != 0)
    {
        goto cmd_fail;      /* EEPROM器件无应答 */
    }

    /* 第9步：循环读取数据 */
    for(i = 0; i < _usSize; i++)
    {
        _pReadBuf[i] = i2c_ReadByte(I2C_EE_TEMP);

        /* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
        if(i != _usSize - 1)
        {
            i2c_Ack(I2C_EE_TEMP);      /* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
        }
        else
        {
            i2c_NAck(I2C_EE_TEMP);     /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        }
    }
    /* 发送I2C总线停止信号 */
    i2c_Stop(I2C_EE_TEMP);

    return 1;               /* 执行成功 */

    /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
cmd_fail:

    i2c_Stop(I2C_EE_TEMP);
	
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
            i2c_Stop(I2C_EE_TEMP);

            /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于10ms */
            for(m = 0; m < 1000; m++)
            {
                /* 第1步：发起I2C总线启动信号 */
                i2c_Start(I2C_EE_TEMP);

                /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
                i2c_SendByte(I2C_EE_TEMP, EE_DEV_ADDR | I2C_WR);

                /* 第3步：发送一个时钟，判断器件是否正确应答 */
                if(i2c_WaitAck(I2C_EE_TEMP) == 0)
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
                i2c_SendByte(I2C_EE_TEMP, (uint8_t)usAddr);
                if(i2c_WaitAck(I2C_EE_TEMP) != 0)
                {
                    goto cmd_fail;      /* EEPROM器件无应答 */
                }
            }
            else
            {
                i2c_SendByte(I2C_EE_TEMP, usAddr >> 8);
                if(i2c_WaitAck(I2C_EE_TEMP) != 0)
                {
                    goto cmd_fail;      /* EEPROM器件无应答 */
                }

                i2c_SendByte(I2C_EE_TEMP, usAddr);
                if(i2c_WaitAck(I2C_EE_TEMP) != 0)
                {
                    goto cmd_fail;      /* EEPROM器件无应答 */
                }
            }
        }

        /* 第6步：开始写入数据 */
        i2c_SendByte(I2C_EE_TEMP, _pWriteBuf[i]);

        /* 第7步：发送ACK */
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;              /* EEPROM器件无应答 */
        }

        usAddr++;
    }

    /* 命令执行成功，发送I2C总线停止信号 */
    i2c_Stop(I2C_EE_TEMP);

    /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于10ms */
    for(m = 0; m < 1000; m++)
    {
        /* 第1步：发起I2C总线启动信号 */
        i2c_Start(I2C_EE_TEMP);

        /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
#if EE_ADDR_A8 == 1
        i2c_SendByte(EE_DEV_ADDR | I2C_WR | ((_usAddress >> 7) & 0x0E));
#else
        i2c_SendByte(I2C_EE_TEMP, EE_DEV_ADDR | I2C_WR);
#endif

        /* 第3步：发送一个时钟，判断器件是否正确应答 */
        if(i2c_WaitAck(I2C_EE_TEMP) == 0)
        {
            break;
        }
    }
    if(m  == 1000)
    {
        goto cmd_fail;                  /* EEPROM器件写超时 */
    }

    /* 命令执行成功，发送I2C总线停止信号 */
    i2c_Stop(I2C_EE_TEMP);

    return 1;

    /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
cmd_fail:

    i2c_Stop(I2C_EE_TEMP);
	
    for(uint8_t j = 0; j < CHANNEL_CFG; j++)
    { 
			IP_Alarm_Flag[j] |= IntelProtect_EEE; 
		}  //ee故障
	
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
	
    int32_t ls, us;
    Dis_int();
    ls = osKernelLock(); 
	
    ee_WriteBytes(_pWriteBuf, _usAddress, _usSize); //写入
    us = osKernelUnlock();
    osKernelRestoreLock(us);
    osKernelRestoreLock(ls);
    En_int();
    for(i = 0; i < _usSize; i++)
    {
        ee_ReadBytes_wc(&readate, _usAddress + i, 1); //读出检测

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

