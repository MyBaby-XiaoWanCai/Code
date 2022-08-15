/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_ad7175.c
** Author      : Cai Heng
** Date        : 2021-07-21
** Description : This file is used to initialize and drive AD7175(8个全差分通道或16个单端通道的24位ADC)
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t AD7175_Start_Read = 0;
uint8_t AD7175_ID[3] = {0, 0, 0};
uint32_t ad7175_value[16] = {0};
extern uint32_t IP_Alarm_Flag[8];       //内部保护标志位
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : Delay()
**  功能说明 : 延时函数
**  形    参 : cnt - 延时时间
**  返 回 值 : 无
**********************************************************************************************************/
static void Delay(uint32_t cnt)
{
    uint32_t i;

    for(i = 0; i < cnt; i++)
        ;
}

/**********************************************************************************************************
**  函 数 名 : AD7175_ComputeCRC8()
**  功能说明 : AD7175 CRC校验
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t AD7175_ComputeCRC8(uint8_t *pBuf, uint8_t bufSize)
{
    uint8_t i   = 0;
    uint8_t crc = 0;

    while(bufSize)
    {
        for(i = 0x80; i != 0; i >>= 1)
        {
            if(((crc & 0x80) != 0) != ((*pBuf & i) != 0))
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
**  函 数 名 : AD7175_ComputeXOR8()
**  功能说明 : AD7175 异或校验
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t AD7175_ComputeXOR8(uint8_t *pBuf, uint8_t bufSize)
{
    uint8_t xor = 0;

    while(bufSize)
    {
        xor ^= *pBuf;
        pBuf++;
        bufSize--;
    }

    return xor;
}

/**********************************************************************************************************
**  函 数 名 : AD7175_ReadReg()
**  功能说明 : AD7175 读寄存器
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int32_t AD7175_ReadReg(uint8_t addr, uint8_t *value, uint8_t size)
{
    uint8_t i        = 0;
    uint8_t crc8     = 0;
    int32_t ret = 0;
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t msgBuf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /* Build the Command word */
    Txdata[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD | AD717X_COMM_REG_RA(addr);

    /* Read data from the device */
    AD7175_CS_0();
    ret = HAL_SPI_TransmitReceive(&SPI3_Handler, Txdata, Rxdata, size + 2, 100 * (size + 2));
    AD7175_CS_1();

    /* Check the CRC */
    //  if(useCRC == AD717X_USE_CRC)
    {
        msgBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD | AD717X_COMM_REG_RA(addr);
        for(i = 1; i < size + 2; ++i)
        {
            msgBuf[i] = Rxdata[i];
        }
        crc8 = AD7175_ComputeCRC8(msgBuf, size + 2);
    }
    //  if(useCRC == AD717X_USE_XOR)
    //  {
    //      msgBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD | AD717X_COMM_REG_RA(addr);
    //      for(i = 1; i < size + 2; ++i)
    //      {
    //          msgBuf[i] = Rxdata[i-1];
    //      }
    //      check8 = AD7175_ComputeXOR8(msgBuf, size + 2);
    //  }
    if(crc8 != 0)
    {
        /* ReadRegister checksum failed. */
        ret = -2;
    }
    /* Build the result */
    for(i = 1; i < size + 1; i++)
    {
        value[i - 1] = Rxdata[i];
    }

    return ret;
}

/**********************************************************************************************************
**  函 数 名 : AD7175_ReadData()
**  功能说明 : AD7175 读取数据
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int32_t AD7175_ReadData(uint8_t *pData)
{
    int32_t ret;

    /* Read the value of the Status Register */
    ret = AD7175_ReadReg(AD717X_DATA_REG, pData, 4);

    return ret;
}

/**********************************************************************************************************
**  函 数 名 : AD7175_WriteReg()
**  功能说明 : AD7175 写寄存器
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int32_t AD7175_WriteReg(uint8_t addr, uint32_t value, uint8_t size)
{
    uint8_t i        = 0;
    uint8_t crc8     = 0;
    int32_t ret = 0;
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
    //  if(useCRC != AD717X_DISABLE)
    {
        crc8 = AD7175_ComputeCRC8(Txdata, size + 1);
        Txdata[size + 1] = crc8;
    }

    /* Write data to the device */
    AD7175_CS_0();
    ret = HAL_SPI_TransmitReceive(&SPI3_Handler, Txdata, Rxdata, size + 2, 100 * (size + 2));
    AD7175_CS_1();

    if(ret == HAL_ERROR)
    {
        for(uint8_t j = 0; j < CHANNEL_CFG; j++)
        { IP_Alarm_Flag[j] |= IntelProtect_ADE; } //ADC故障
    }
    return ret;
}

/**********************************************************************************************************
**  函 数 名 : AD7175_Reset()
**  功能说明 : AD7175 复位
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int32_t AD7175_Reset(void)
{
    int32_t ret = 0;
    uint8_t Txdata[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    AD7175_CS_0();
    ret = HAL_SPI_TransmitReceive(&SPI3_Handler, Txdata, Rxdata, 8, 100 * 8);
    AD7175_CS_1();

    return ret;
}

/**********************************************************************************************************
**  函 数 名 : AD7175_CfgADC()
**  功能说明 : AD7175 参数配置
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int32_t AD7175_CfgADC(void)
{
    int32_t ret = 0;

    /* Disable interrupt */
    HAL_NVIC_DisableIRQ(EXTI0_IRQn);

    /*  Reset the device interface.*/
    ret = AD7175_Reset();
    if(ret < 0)
    { return ret; }

    Delay(8000);

    /* Initialize ADC mode register. */
    ret = AD7175_WriteReg(AD717X_ADCMODE_REG, AD717X_ADCMODE_REG_DELAY(2) | AD717X_ADCMODE_SING_CYC, 2);
    if(ret < 0)
    { return ret; }

    /* Initialize Interface mode register. */
    ret = AD7175_WriteReg(AD717X_IFMODE_REG, AD717X_IFMODE_REG_DOUT_RESET | AD717X_IFMODE_REG_DATA_STAT | AD717X_IFMODE_REG_CRC_EN, 2);
    if(ret < 0)
    { return ret; }

    /* Initialize registers AD717X_GPIOCON_REG through AD717X_OFFSET0_REG */
    ret = AD7175_WriteReg(AD717X_GPIOCON_REG, 0x0000, 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP0_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(0) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP1_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(1) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP2_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(2) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP3_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(3) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP4_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(4) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP5_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(5) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP6_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(6) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP7_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(7) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP8_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(8) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP9_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(9) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP10_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(10) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP11_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(11) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP12_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(12) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP13_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(13) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP14_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(14) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }

    ret = AD7175_WriteReg(AD717X_CHMAP15_REG, AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(15) | AD717X_CHMAP_REG_AINNEG(16), 2);
    if(ret < 0)
    { return ret; }
#if AD_uinpolar   //单极性ADC编码输出
    ret = AD7175_WriteReg(AD717X_SETUPCON0_REG, AD717X_SETUP_CONF_REG_AIN_BUF(3) | AD717X_SETUP_CONF_REG_REF_SEL(0), 2);
    if(ret < 0)
    { return ret; }
#else            //双极性ADC编码输出
    ret = AD7175_WriteReg(AD717X_SETUPCON0_REG, (1 << 12) | AD717X_SETUP_CONF_REG_AIN_BUF(3) | AD717X_SETUP_CONF_REG_REF_SEL(0), 2);
    if(ret < 0)
    { return ret; }
#endif
    ret = AD7175_WriteReg(AD717X_FILTCON0_REG, AD717X_FILT_CONF_REG_ENHFILT(5) | AD717X_FILT_CONF_REG_ODR(5), 2);   //25KSPS
    if(ret < 0)
    { return ret; }

    /* Read ID register to identify the part */
    ret = AD7175_ReadReg(AD717X_ID_REG, AD7175_ID, 2);
    if((ret < 0) || (AD7175_ID[0] != 0x3C) || ((AD7175_ID[1] & 0XD0) != 0xD0))
    {
        ret = -1;
        for(uint8_t j = 0; j < CHANNEL_CFG; j++)
        { IP_Alarm_Flag[j] |= IntelProtect_ADE; } //ADC故障
    }

    /* Enable interrupt */
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    return ret;
}

/**********************************************************************************************************
**  函 数 名 : AD7175_WaitForReady()
**  功能说明 : 等待循环读取
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int32_t AD7175_WaitForReady(uint32_t timeout)
{
    int32_t ret;
    int8_t ready = 0;
    uint8_t value[1] = {0};

    while(!ready && --timeout)
    {
        /* Read the value of the Status Register */
        ret = AD7175_ReadReg(AD717X_STATUS_REG, value, 1);
        if(ret < 0)
        { return ret; }
        /* Check the RDY bit in the Status Register */
        ready = (value[0] & AD717X_STATUS_REG_RDY) == 0;
    }
    return timeout ? 0 : TIMEOUT;
}

/**********************************************************************************************************
**  函 数 名 : AD7175_CfgINT()
**  功能说明 : AD7175的外部中断引脚配置
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AD7175_CfgINT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    AD7175_RDY_GPIO_CLK_ALLENABLE();

    GPIO_InitStruct.Pin = AD7175_RDY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(AD7175_RDY_PORT, &GPIO_InitStruct);
}

/**********************************************************************************************************
**  函 数 名 : AD7175_CfgERR()
**  功能说明 : AD7175的错误引脚配置
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AD7175_CfgERR(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    AD7175_ERR_GPIO_CLK_ALLENABLE();

    GPIO_InitStruct.Pin = AD7175_ERR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(AD7175_ERR_PORT, &GPIO_InitStruct);
}

/**********************************************************************************************************
**  函 数 名 : bsp_InitAD7175()
**  功能说明 : AD7175初始化配置
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void bsp_InitAD7175(void)
{
    AD7175_CfgSPI();

    AD7175_CfgINT();

    AD7175_CfgERR();

    AD7175_CS_1();

    AD7175_CfgADC();

    if(AD7175_Start_Read == 0)
    {
        AD7175_Start_Read = 1;
        AD7175_CS_0();
    }
}

/**********************************************************************************************************
**  函 数 名 : EXTI0_IRQHandler()
**  功能说明 : 外部中断处理函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(AD7175_RDY_PIN);
}

/**********************************************************************************************************
**  函 数 名 : AD7175_Process()
**  功能说明 : AD7175 采集处理程序
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
void AD7175_Process(void)
{
    uint8_t value[4] = {0, 0, 0, 0};
    uint32_t read = 0;

    if(AD7175_ReadData(value) == 0)
    {
        /* 读采样结果，3个字节，高字节在前 */
        read = (uint32_t)(value[0] << 16);
        read += (uint32_t)(value[1] << 8);
        read += value[2] << 0;
        if((value[3] & 0xB0) == 0)
        {
            ad7175_value[value[3] & 0x0F] = read;
        }
    }
    else
    {
        //错误处理
    }

    AD7175_CS_0();
}


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

