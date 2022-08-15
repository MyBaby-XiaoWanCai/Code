/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_ad7124.c
** Author      : Cai Heng
** Date        : 2021-07-19
** Description : This file is used to initialize and drive AD7124(8通道带PGA的24位ADC)
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t check_ready = 0;
uint8_t AD7124_Start_Read = 0;
uint32_t ad7124_value[16] = {0};

extern uint32_t IP_Alarm_Flag[8];       //内部保护标志位
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void bsp_InitAD7124(void);

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
**	函 数 名 : ComputeCRC8()
**	功能说明 : CRC校验
**	形    参 : p_buf    - 校验数据
**	           buf_size - 校验长度
**	返 回 值 : CRC校验值
**********************************************************************************************************/
uint8_t ComputeCRC8(uint8_t* p_buf, uint8_t buf_size)
{
    uint8_t i = 0;
    uint8_t crc = 0;

    while(buf_size)
    {
        for(i = 0x80; i != 0; i >>= 1)
        {
            bool cmp1 = (crc & 0x80) != 0;
            bool cmp2 = (*p_buf & i) != 0;

            if(cmp1 != cmp2)
            {
                /* MSB of CRC register XOR input Bit from Data */
                crc <<= 1;
                crc ^= AD7124_CRC8_POLYNOMIAL_REPRESENTATION;
            }
            else
            {
                crc <<= 1;
            }
        }
        p_buf++;
        buf_size--;
    }
    return crc;
}

/**********************************************************************************************************
**	函 数 名 : AD7124_ReadReg()
**	功能说明 : 读AD7124寄存器
**	形    参 : addr  -
**	           value -
**	           size  -
**	返 回 值 : -1 - 错误
**	            0 - 正确
**********************************************************************************************************/
int32_t AD7124_ReadReg(uint8_t addr, uint8_t* value, uint8_t size)
{
    uint8_t i    = 0;
    uint8_t crc8 = 0;
    int32_t ret  = 0;
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t msgBuf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /* Build the Command word */
    Txdata[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD | AD7124_COMM_REG_RA(addr);

    /* Read data from the device */
    AD7124_CS_0();
    ret = HAL_SPI_TransmitReceive(&AD7124_HANDLER, Txdata, Rxdata, size + 2, 100 * (size + 2));
    AD7124_CS_1();

    /* Check the CRC */
//	if(useCRC == AD717X_USE_CRC)
    {
        msgBuf[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD | AD7124_COMM_REG_RA(addr);
        for(i = 1; i < size + 2; ++i)
        {
            msgBuf[i] = Rxdata[i];
        }
        crc8 = ComputeCRC8(msgBuf, size + 2);
    }
    if(check_ready == 1)
    {
        if(crc8 != 0)
        {
            /* ReadRegister checksum failed */
            return COMM_ERR;
        }
    }
    /* Build the result */
    for(i = 1; i < size + 1; i++)
    {
        value[i - 1] = Rxdata[i];
    }
    return ret;
}

/**********************************************************************************************************
**	函 数 名 : AD7124_ReadData()
**	功能说明 : AD7124读取数据
**	形    参 : pData - 4个字节的指针
**	返 回 值 : -1 - 错误
**	            0 - 正确
**********************************************************************************************************/
int32_t AD7124_ReadData(uint8_t* pData)
{
    int32_t ret;

    /* Read the value of the Status Register */
    ret = AD7124_ReadReg(AD7124_DATA_REG, pData, 4);

    return ret;
}

/**********************************************************************************************************
**	函 数 名 : AD7124_WriteReg()
**	功能说明 : 写AD7124寄存器
**	形    参 : addr  -
**	           value -
**	           size  -
**	返 回 值 : -1 - 错误
**	            0 - 正确
**********************************************************************************************************/
int32_t AD7124_WriteReg(uint8_t addr, int32_t value, uint8_t size)
{
    uint8_t i    = 0;
    uint8_t crc8 = 0;
    int32_t ret  = 0;
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /* Build the Command word */
    Txdata[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_WR | AD7124_COMM_REG_RA(addr);

    /* Fill the write buffer */
    for(i = 0; i < size; i++)
    {
        Txdata[size - i] = value & 0xFF;
        value >>= 8;
    }

    /* Compute the CRC */
//	if(useCRC != AD717X_DISABLE)
    {
        crc8 = ComputeCRC8(Txdata, size + 1);
        Txdata[size + 1] = crc8;
    }

    /* Write data to the device */
    AD7124_CS_0();
    ret = HAL_SPI_TransmitReceive(&AD7124_HANDLER, Txdata, Rxdata, size + 2, 100 * (size + 2));
    AD7124_CS_1();

    if(ret==HAL_ERROR)
    {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障
    }
    return ret;
}

/**********************************************************************************************************
**	函 数 名 : ad7124_wait_to_power_on()
**	功能说明 : 等待电源打开
**	形    参 : timeout - 等待超时时间
**	返 回 值 : -1 - 错误
**	            0 - 正确
**********************************************************************************************************/
int32_t ad7124_wait_to_power_on(uint32_t timeout)
{
    int32_t ret;
    int8_t powered_on = 0;
    uint8_t value[1] = {0};

    while(!powered_on && timeout--)
    {
        ret = AD7124_ReadReg(AD7124_STATUS_REG, value, 1);
        if(ret < 0)
        {
            return ret;
        }

        /* Check the POR_FLAG bit in the Status Register */
        powered_on = (value[0] & AD7124_STATUS_REG_POR_FLAG) == 0;
    }
    return (timeout || powered_on) ? 0 : -1;
}

/**********************************************************************************************************
**	函 数 名 : AD7124_Reset()
**	功能说明 : AD7124复位
**	形    参 : 无
**	返 回 值 : -1 - 错误
**	            0 - 正确
**********************************************************************************************************/
int32_t AD7124_Reset(void)
{
    int32_t ret = 0;
    uint8_t Txdata[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    AD7124_CS_0();
    ret = HAL_SPI_TransmitReceive(&AD7124_HANDLER, Txdata, Rxdata, 8, 100 * 8);
    AD7124_CS_1();
    if(ret!=0)
    {
        return -1;
    }

    ret = ad7124_wait_to_power_on(800);
    if(ret!=0)
    {
        return -1;
    }

    /* CRC is disabled after reset */
//	use_crc = AD7124_DISABLE_CRC;
    Delay(8000);

    return ret;
}

/**********************************************************************************************************
**	函 数 名 : AD7124_CfgADC()
**	功能说明 : AD7124寄存器配置
**	形    参 : 无
**	返 回 值 : -1 - 错误
**	            0 - 正确
**********************************************************************************************************/
int32_t AD7124_CfgADC(void)
{
    int32_t ret=0;

    /* Disable interrupt */
    HAL_NVIC_DisableIRQ(AD7124_EXTIIRQ);

    /* Reset the device interface */
    ret = AD7124_Reset();
    if (ret < 0)
    {
        return ret;
    }

    /* Update the device structure with power-on/reset settings */
    /* Initialize ADC register. */
    ret = AD7124_WriteReg(AD7124_ADC_CTRL_REG, AD7124_ADC_CTRL_REG_DATA_STATUS | AD7124_ADC_CTRL_REG_POWER_MODE(2) | AD7124_ADC_CTRL_REG_MODE(0), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_IO_CTRL1_REG, 0, 3);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_IO_CTRL2_REG, 0, 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_ERREN_REG, AD7124_ERREN_REG_SPI_CRC_ERR_EN, 3);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH0_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(0) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH1_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(1) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH2_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(2) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH3_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(3) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH4_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(4) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH5_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(5) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH6_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(6) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH7_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(7) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH8_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(8) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH9_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(9) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH10_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(10) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH11_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(11) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH12_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(12) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH13_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(13) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH14_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(14) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }

    ret = AD7124_WriteReg(AD7124_CH15_MAP_REG, AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(15) | AD7124_CH_MAP_REG_AINM(17), 2);
    if(ret < 0)
    {
        return ret;
    }
#if  AD_uinpolar   //单极性ADC编码输出
    ret = AD7124_WriteReg(AD7124_CFG0_REG, AD7124_CFG_REG_AIN_BUFP | AD7124_CFG_REG_AINN_BUFM | AD7124_CFG_REG_REF_SEL(0) | AD7124_CFG_REG_PGA(0), 2);
    if(ret < 0)
    {
        return ret;
    }
#else            //双极性ADC输出
    ret = AD7124_WriteReg(AD7124_CFG0_REG, (1<<11)|AD7124_CFG_REG_AIN_BUFP | AD7124_CFG_REG_AINN_BUFM | AD7124_CFG_REG_REF_SEL(0) | AD7124_CFG_REG_PGA(0), 2);
    if(ret < 0)
    {
        return ret;
    }
#endif 
    
#ifdef AD7124_HARDWARE_FILTER
	ret = AD7124_WriteReg(AD7124_FILT0_REG, AD7124_FILT_REG_FILTER(7) | AD7124_FILT_REG_REJ60 | AD7124_FILT_REG_POST_FILTER(5) | AD7124_FILT_REG_FS(320), 3);
#else	
    ret = AD7124_WriteReg(AD7124_FILT0_REG, AD7124_FILT_REG_FILTER(0) | AD7124_FILT_REG_REJ60 | AD7124_FILT_REG_POST_FILTER(3) | AD7124_FILT_REG_FS(1), 3);
#endif
	
    if(ret < 0)
    {
        return ret;
    }

    /* Read ID register to identify the part */
    uint8_t AD7124_ID[2]= {0,0};
    ret = AD7124_ReadReg(AD7124_ID_REG,AD7124_ID, 2);
    if((ret < 0) || (AD7124_ID[0] != 0x14) || ((AD7124_ID[1] & 0X76) != 0x76))
    {   ret=-1;
        for(uint8_t j=0;j<CHANNEL_CFG;j++)
		{
			IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障
		}
    }

    check_ready = 1;

    return ret;
}

/**********************************************************************************************************
**	函 数 名 : AD7124_WaitFor_SPI_Ready()
**	功能说明 : 等待SPI准备好
**	形    参 : timeout - 超时时间
**	返 回 值 :  0 - 未超时
**	           -3 - 已超时
**********************************************************************************************************/
int32_t AD7124_WaitFor_SPI_Ready(uint32_t timeout)
{
    int32_t ret;
    int8_t ready = 0;
    uint8_t value[3] = {0};

    while(!ready && --timeout)
    {
        /* Read the value of the Status Register */
        ret = AD7124_ReadReg(AD7124_ERR_REG, value, 3);
        if(ret < 0)
        {
            return ret;
        }
        /* Check the RDY bit in the Status Register */
        ready = (value[2] & AD7124_ERR_REG_SPI_IGNORE_ERR) == 0;
    }
    return timeout ? 0 : TIMEOUT;
}

/**********************************************************************************************************
**	函 数 名 : AD7124_WaitFor_Conv_Ready()
**	功能说明 : 等待转换准备好
**	形    参 : ch      - 通道号
**	           timeout - 超时时间
**	返 回 值 :  0 - 未超时
**	           -3 - 已超时
**********************************************************************************************************/
int32_t AD7124_WaitFor_Conv_Ready(uint8_t* ch, uint32_t timeout)
{
    int32_t ret;
    int8_t ready = 0;
    uint8_t value[1] = {0};

    while(!ready && --timeout)
    {
        /* Read the value of the Status Register */
        ret = AD7124_ReadReg(AD7124_STATUS_REG, value, 1);
        if(ret < 0)
        {
            return ret;
        }
        /* Check the RDY bit in the Status Register */
        ready = (value[0] & AD7124_STATUS_REG_RDY) == 0;
        ch[0] = value[0] & 0x0F;
    }
    return timeout ? 0 : TIMEOUT;
}

/**********************************************************************************************************
**	函 数 名 : AD7124_CfgINT()
**	功能说明 : AD7124的外部中断引脚配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AD7124_CfgINT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    AD7124_RDY_GPIO_CLK_ALLENABLE();

    GPIO_InitStruct.Pin = AD7124_RDY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(AD7124_RDY_PORT, &GPIO_InitStruct);
}

/**********************************************************************************************************
**	函 数 名 : bsp_InitAD7124()
**	功能说明 : AD7124初始化配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitAD7124(void)
{
    AD7124_INTERFACE_CFG();		/* AD7124接口配置: SPI2 */

    AD7124_INTERRUPT_CFG();		/* AD7124外部中断引脚配置 */

    AD7124_CS_1();

    AD7124_CfgADC();

    HAL_NVIC_SetPriority(AD7124_EXTIIRQ, AD7124_EXTIIT_PP, AD7124_EXTIIT_SP);

    /* Enable interrupt */
    HAL_NVIC_EnableIRQ(AD7124_EXTIIRQ);

    if(AD7124_Start_Read == 0)
    {
        AD7124_Start_Read = 1;
        AD7124_CS_0();
    }
}

/**********************************************************************************************************
**	函 数 名 : AD7124_Process()
**	功能说明 : AD7124处理函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AD7124_Process(void)
{
    uint8_t value[4] = {0};
    uint32_t read = 0;

    if(AD7124_ReadData(value) == 0)
    {
        /* 读采样结果，3个字节，高字节在前 */
        read = (uint32_t)(value[0] << 16);
        read += (uint32_t)(value[1] << 8);
        read += value[2] << 0;

//		/* 负数进行扩展。24位有符号数扩展为32位有符号数 */
//		if (read & 0x800000)//为正值
//		{
//			read = read-0x800000;
//		}
//		else //为负值
//		{
//			read = (read+0x800000)|0xFF000000;
//		}
        if((value[3] & 0xF0) == 0)
        {
            /* 定时器定时读取采样数据 */
            ad7124_value[value[3] & 0x0F] = read;
        }
    }
    AD7124_CS_0();
}

/**********************************************************************************************************
**	函 数 名 : EXTI1_IRQHandler()
**	功能说明 : 外部中断处理函数
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(AD7124_RDY_PIN);
}

/**********************************************************************************************************
**	函 数 名 : HAL_GPIO_EXTI_Callback()
**	功能说明 : 外部中断回调函数(被HAL_GPIO_EXTI_IRQHandler()调用,由用户实现具体功能)
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint16_t ad7124_cnt = 0;
    static uint16_t ad7175_cnt = 0;

    if(GPIO_Pin == AD7175_RDY_PIN)
    {
        if(AD7175_Start_Read == 1)
        {
            ad7175_cnt++;
            HAL_NVIC_DisableIRQ(EXTI0_IRQn);		/* 禁止外部中断 */
            if(ad7175_cnt % 2 == 0)
            {
                AD7175_Process();   				/* AD7175处理 */
            }
            HAL_NVIC_EnableIRQ(EXTI0_IRQn);			/* 使能外部中断 */
        }
    }

    if(GPIO_Pin == AD7124_RDY_PIN)
    {
        if(AD7124_Start_Read == 1)
        {
            ad7124_cnt++;
            HAL_NVIC_DisableIRQ(AD7124_EXTIIRQ);	/* 禁止外部中断 */
            if(ad7124_cnt % 2 == 0)
            {
                AD7124_Process();					/* AD7124处理 */
            }
            HAL_NVIC_EnableIRQ(AD7124_EXTIIRQ);		/* 使能外部中断 */
        }
    }
}


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

