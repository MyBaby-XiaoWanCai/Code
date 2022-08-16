/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : ad7175.c
** Author      : Huang Cheng
** Date        : 2021-08-23
** Description : ad7175 source file 用于采集sense电压 和 IoutIns
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_ad7175.h"
#include "global.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t AD7175_Start_Read = 0;
uint8_t AD7175_ID[3] = {0, 0, 0};
uint32_t ad7175_value[16] = {0};
uint8_t Ad7175_Flag = 0x00;

AD7175_REG_DATASTR AD7175_RegData[] =
{
    /*
    	SETUPCON/FILTCON/OFFSET/GAIN仅需要写对应的即可，
    	根据CH的SETUP_SEL0位配置来选。
    */
    {STATUS,      AD7175_RD, 1, NULL},
    {ADCMODE,     AD7175_WR, 2, AD717X_ADCMODE_REG_DELAY(2) | AD717X_ADCMODE_SING_CYC},
    {IFMODE,      AD7175_WR, 2, AD717X_IFMODE_REG_DOUT_RESET | AD717X_IFMODE_REG_DATA_STAT | AD717X_IFMODE_REG_CRC_EN},
    {REGCHECK,    AD7175_RD, 3, NULL},
    {DATA,        AD7175_RD, 3+1, NULL},    //3字节数据(MSB->LSB)+1字节通道数
    {RESERVED_5,  AD7175_RD, 0, NULL},      //无此寄存器，小心入坑！！！
    {GPIOCON,     AD7175_WR, 2, 0x0000},
    {ID,          AD7175_RD, 2, NULL},
    {RESERVED_8,  AD7175_RD, 0, NULL},      //无此寄存器，小心入坑！！！
    {RESERVED_9,  AD7175_RD, 0, NULL},      //无此寄存器，小心入坑！！！
    {RESERVED_10, AD7175_RD, 0, NULL},      //无此寄存器，小心入坑！！！
    {RESERVED_11, AD7175_RD, 0, NULL},      //无此寄存器，小心入坑！！！
    {RESERVED_12, AD7175_RD, 0, NULL},      //无此寄存器，小心入坑！！！
    {RESERVED_13, AD7175_RD, 0, NULL},      //无此寄存器，小心入坑！！！
    {RESERVED_14, AD7175_RD, 0, NULL},      //无此寄存器，小心入坑！！！
    {RESERVED_15, AD7175_RD, 0, NULL},      //无此寄存器，小心入坑！！！
    {CH0,         AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(0)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH1,         AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(1)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH2,         AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(2)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH3,         AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(3)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH4,         AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(4)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH5,         AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(5)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH6,         AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(6)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH7,         AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(7)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH8,         AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(8)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH9,         AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(9)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH10,        AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(10)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH11,        AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(11)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH12,        AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(12)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH13,        AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(13)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH14,        AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(14)| AD717X_CHMAP_REG_AINNEG(16)},
    {CH15,        AD7175_WR, 2, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(15)| AD717X_CHMAP_REG_AINNEG(16)},
#if AD_uinpolar   //单极性ADC编码输出
    {SETUPCON0,   AD7175_WR, 2, AD717X_SETUP_CONF_REG_AIN_BUF(3)| AD717X_SETUP_CONF_REG_REF_SEL(0)},     //此处SETUPCON0和实际数组下标不对应，请勿用SETUPCON0索引！
#else             //双极性ADC编码输出
    {SETUPCON0,   AD7175_WR, 2, (1 << 12) | AD717X_SETUP_CONF_REG_AIN_BUF(3) | AD717X_SETUP_CONF_REG_REF_SEL(0)},
#endif
    {FILTCON0,    AD7175_WR, 2, AD717X_FILT_CONF_REG_ENHFILT(5) | AD717X_FILT_CONF_REG_ODR(5)},          //此处FILTCON0和实际数组下标不对应，请勿用FILTCON0索引！
};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : Delay()
**	功能说明 : 延时函数
**	形    参 : cnt - 延时时间
**	返 回 值 : 无
**********************************************************************************************************/
static void Delay(uint32_t cnt)
{
    uint32_t i;

    for (i = 0; i < cnt; i++);
}

/**********************************************************************************************************
**	函 数 名 : AD7175_ComputeCRC8()
**	功能说明 : AD7175 CRC校验
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
uint8_t AD7175_ComputeCRC8(uint8_t *pBuf, uint8_t bufSize)
{
    uint8_t i   = 0;
    uint8_t crc = 0;

    while(bufSize)
    {
        for(i = 0x80; i != 0; i >>= 1)
        {
            if(((crc & 0x80) != 0) != ((*pBuf & i) !=0))
            {
                /* MSB of CRC register XOR input Bit from Data */
                crc <<= 1;
                crc ^= AD717X_CRC8_POLYNOMIAL_REPRESENTATION;
            }
            else
            {
                crc <<= 1;
            }
        }
        pBuf++;
        bufSize--;
    }
    return crc;
}

/**********************************************************************************************************
**	函 数 名 : AD7175_ReadReg()
**	功能说明 : AD7175 读寄存器
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
uint32_t AD7175_ReadReg(uint8_t addr,uint8_t * value,uint8_t size)
{
    uint8_t i = 0;
    uint8_t crc8 = 0;
    uint32_t ret = 0;
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    uint8_t msgBuf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /* Build the Command word */
    Txdata[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD | AD717X_COMM_REG_RA(addr);

    /* Read data from the device */
    AD7175_NSS_0();
    ret = SPI_TransmitReceive(AD7175_SPI_UNIT, Txdata, Rxdata, size + 2);  //size+2 = Size(Data-4byte) + Cmd(1byte) + Crc(1byte)
    AD7175_NSS_1();

    /* Check the CRC */
    msgBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD | AD717X_COMM_REG_RA(addr);
    for(i = 1; i < size + 2; ++i)
    {
        msgBuf[i] = Rxdata[i];
    }

    crc8 = AD7175_ComputeCRC8(msgBuf, size + 1);
    if(crc8 != msgBuf[size+1])
    {
        /* ReadRegister checksum failed. */
        ret = AD7175_CRC_ERR;
    }

    /* Build the result */
    for(i = 1; i < size + 1; i++)
    {
        value[i - 1] = Rxdata[i];
    }

    return ret;
}

/**********************************************************************************************************
**	函 数 名 : AD7175_ReadData()
**	功能说明 : AD7175 读取数据
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
uint32_t AD7175_ReadData(uint8_t* pData)
{
    uint32_t ret;

    /* Read the value of the Status Register */
    ret = AD7175_ReadReg(AD7175_RegData[DATA].RegAddr, pData, AD7175_RegData[DATA].DataLen);

    return ret;
}

/**********************************************************************************************************
**	函 数 名 : AD7175_WriteReg()
**	功能说明 : AD7175 写寄存器
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
uint32_t AD7175_WriteReg(uint8_t addr, uint32_t value, uint8_t size)
{
    uint8_t i        = 0;
    uint8_t crc8     = 0;
    uint32_t ret = 0;
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /* Build the Command word */
    Txdata[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_WR | AD717X_COMM_REG_RA(addr);

    /* Fill the write buffer */
    for(i = 0; i < size; i++)
    {
        Txdata[size - i] = value & 0xFF;
        value >>= 8;
    }

    /* Compute the CRC */
    crc8 = AD7175_ComputeCRC8(Txdata, size + 1);
    Txdata[size + 1] = crc8;

    /* Write data to the device */
    AD7175_NSS_0();
    ret = SPI_TransmitReceive(AD7175_SPI_UNIT, Txdata, Rxdata, size + 2);
    AD7175_NSS_1();

    return ret;
}

/**********************************************************************************************************
**	函 数 名 : AD7175_Reset()
**	功能说明 : AD7175 复位
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
uint32_t AD7175_Reset(void)
{
    int32_t ret = 0;
    uint8_t Txdata[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    AD7175_NSS_0();
    ret = SPI_TransmitReceive(AD7175_SPI_UNIT, Txdata, Rxdata, 8);
    AD7175_NSS_1();

    return ret;
}

/**********************************************************************************************************
**	函 数 名 : AD7175_CfgADC()
**	功能说明 : AD7175 参数配置
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
uint32_t AD7175_CfgADC(void)
{
    uint32_t ret = 0;

    /* Disable interrupt */
    NVIC_DisableIRQ(AD7175_IRQn);

    /*  Reset the device interface.*/
    ret = AD7175_Reset();
    if(ret > 0)
    {
        return ret;
    }
    Delay(8000);

    for(uint8_t i = 0; i < sizeof(AD7175_RegData)/sizeof(AD7175_RegData[0]); i++)
    {
        if(AD7175_RegData[i].ReadWrite == AD7175_RD)
        {
            continue;
        }

        /* AD7175_RegData[i].RegAddr不能被i代替，因为SETUPCON0和FILTCON0不对应数组下标 */
        ret = AD7175_WriteReg(AD7175_RegData[i].RegAddr, AD7175_RegData[i].RegData, AD7175_RegData[i].DataLen);
        if(ret > 0)
        {
            return ret;
        }
    }

    /* Read ID register to identify the part */
    ret = AD7175_ReadReg(AD7175_RegData[ID].RegAddr, AD7175_ID, AD7175_RegData[ID].DataLen);
    if((ret > 0) || (AD7175_ID[0] != 0x3C) || ((AD7175_ID[1] & 0xD0) != 0xD0))
    {
        ret = Error;

        for(uint8_t j = 0; j < CHANNEL_CFG; j++)
        {
            IP_Alarm_Flag[j] |= IntelProtect_ADE;  //ADC故障
        }
    }

    return ret;
}

/**********************************************************************************************************
**	函 数 名 : AD7175_WaitForReady()
**	功能说明 : 等待循环读取
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
uint32_t AD7175_WaitForReady(uint32_t timeout)
{
    uint32_t ret;
    int8_t ready = 0;
    uint8_t value[1] = {0};

    while(!ready && --timeout)
    {
        /* Read the value of the Status Register */
        ret = AD7175_ReadReg(AD7175_RegData[STATUS].RegAddr, value, AD7175_RegData[STATUS].DataLen);
        if(ret > 0)
            return ret;
        /* Check the RDY bit in the Status Register */
        ready = (value[0] & AD717X_STATUS_REG_RDY) == 0;
    }
    return timeout ? Ok : ErrorTimeout;
}

/**********************************************************************************************************
**	函 数 名 : bsp_AD7175Init()
**	功能说明 : AD7175初始化配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_AD7175Init(void)
{
    AD7175_SpiInit();
    AD7175_EXINT_Init();
    AD7175_NSS_1();

    AD7175_CfgADC();

    if(AD7175_Start_Read == 0)
    {
        AD7175_Start_Read = 1;
        AD7175_NSS_0();

        /* Enable interrupt */
        NVIC_EnableIRQ(AD7175_IRQn);
    }
}

/**********************************************************************************************************
**	函 数 名 : AD7175_Process()
**	功能说明 : AD7175 采集处理程序
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void AD7175_Process(void)
{
    uint8_t ChIndex = 0x00;
    uint8_t value[4] = {0, 0, 0, 0};
    uint32_t read = 0;

    if((AD7175_ReadData(value)==0) && (AD7175_Start_Read==1))
    {
        /* 读采样结果，3个字节，高字节在前 */
        read = (uint32_t)(value[0] << 16);
        read += (uint32_t)(value[1] << 8);
        read += value[2] << 0;

        ChIndex = value[3] & 0x0F;

        ad7175_value[ChIndex] = read;
    }
    else
    {
        //错误处理
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

