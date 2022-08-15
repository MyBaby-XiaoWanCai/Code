/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_ad7124.c
** Author      : Cai Heng
** Date        : 2021-07-19
** Description : This file is used to initialize and drive AD7124(8ͨ����PGA��24λADC)
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "bsp.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t check_ready = 0;
uint8_t AD7124_Start_Read = 0;
uint32_t ad7124_value[16] = {0};

extern uint32_t IP_Alarm_Flag[8];       //�ڲ�������־λ
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void bsp_InitAD7124(void);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : Delay()
**	����˵�� : ��ʱ����
**	��    �� : cnt - ��ʱʱ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Delay(uint32_t cnt)
{
    uint32_t i;

    for (i = 0; i < cnt; i++);
}

/**********************************************************************************************************
**	�� �� �� : ComputeCRC8()
**	����˵�� : CRCУ��
**	��    �� : p_buf    - У������
**	           buf_size - У�鳤��
**	�� �� ֵ : CRCУ��ֵ
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
**	�� �� �� : AD7124_ReadReg()
**	����˵�� : ��AD7124�Ĵ���
**	��    �� : addr  -
**	           value -
**	           size  -
**	�� �� ֵ : -1 - ����
**	            0 - ��ȷ
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
**	�� �� �� : AD7124_ReadData()
**	����˵�� : AD7124��ȡ����
**	��    �� : pData - 4���ֽڵ�ָ��
**	�� �� ֵ : -1 - ����
**	            0 - ��ȷ
**********************************************************************************************************/
int32_t AD7124_ReadData(uint8_t* pData)
{
    int32_t ret;

    /* Read the value of the Status Register */
    ret = AD7124_ReadReg(AD7124_DATA_REG, pData, 4);

    return ret;
}

/**********************************************************************************************************
**	�� �� �� : AD7124_WriteReg()
**	����˵�� : дAD7124�Ĵ���
**	��    �� : addr  -
**	           value -
**	           size  -
**	�� �� ֵ : -1 - ����
**	            0 - ��ȷ
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
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC����
    }
    return ret;
}

/**********************************************************************************************************
**	�� �� �� : ad7124_wait_to_power_on()
**	����˵�� : �ȴ���Դ��
**	��    �� : timeout - �ȴ���ʱʱ��
**	�� �� ֵ : -1 - ����
**	            0 - ��ȷ
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
**	�� �� �� : AD7124_Reset()
**	����˵�� : AD7124��λ
**	��    �� : ��
**	�� �� ֵ : -1 - ����
**	            0 - ��ȷ
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
**	�� �� �� : AD7124_CfgADC()
**	����˵�� : AD7124�Ĵ�������
**	��    �� : ��
**	�� �� ֵ : -1 - ����
**	            0 - ��ȷ
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
#if  AD_uinpolar   //������ADC�������
    ret = AD7124_WriteReg(AD7124_CFG0_REG, AD7124_CFG_REG_AIN_BUFP | AD7124_CFG_REG_AINN_BUFM | AD7124_CFG_REG_REF_SEL(0) | AD7124_CFG_REG_PGA(0), 2);
    if(ret < 0)
    {
        return ret;
    }
#else            //˫����ADC���
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
			IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC����
		}
    }

    check_ready = 1;

    return ret;
}

/**********************************************************************************************************
**	�� �� �� : AD7124_WaitFor_SPI_Ready()
**	����˵�� : �ȴ�SPI׼����
**	��    �� : timeout - ��ʱʱ��
**	�� �� ֵ :  0 - δ��ʱ
**	           -3 - �ѳ�ʱ
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
**	�� �� �� : AD7124_WaitFor_Conv_Ready()
**	����˵�� : �ȴ�ת��׼����
**	��    �� : ch      - ͨ����
**	           timeout - ��ʱʱ��
**	�� �� ֵ :  0 - δ��ʱ
**	           -3 - �ѳ�ʱ
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
**	�� �� �� : AD7124_CfgINT()
**	����˵�� : AD7124���ⲿ�ж���������
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : bsp_InitAD7124()
**	����˵�� : AD7124��ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitAD7124(void)
{
    AD7124_INTERFACE_CFG();		/* AD7124�ӿ�����: SPI2 */

    AD7124_INTERRUPT_CFG();		/* AD7124�ⲿ�ж��������� */

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
**	�� �� �� : AD7124_Process()
**	����˵�� : AD7124������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void AD7124_Process(void)
{
    uint8_t value[4] = {0};
    uint32_t read = 0;

    if(AD7124_ReadData(value) == 0)
    {
        /* �����������3���ֽڣ����ֽ���ǰ */
        read = (uint32_t)(value[0] << 16);
        read += (uint32_t)(value[1] << 8);
        read += value[2] << 0;

//		/* ����������չ��24λ�з�������չΪ32λ�з����� */
//		if (read & 0x800000)//Ϊ��ֵ
//		{
//			read = read-0x800000;
//		}
//		else //Ϊ��ֵ
//		{
//			read = (read+0x800000)|0xFF000000;
//		}
        if((value[3] & 0xF0) == 0)
        {
            /* ��ʱ����ʱ��ȡ�������� */
            ad7124_value[value[3] & 0x0F] = read;
        }
    }
    AD7124_CS_0();
}

/**********************************************************************************************************
**	�� �� �� : EXTI1_IRQHandler()
**	����˵�� : �ⲿ�жϴ�����
**	��    �� :
**	�� �� ֵ :
**********************************************************************************************************/
void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(AD7124_RDY_PIN);
}

/**********************************************************************************************************
**	�� �� �� : HAL_GPIO_EXTI_Callback()
**	����˵�� : �ⲿ�жϻص�����(��HAL_GPIO_EXTI_IRQHandler()����,���û�ʵ�־��幦��)
**	��    �� :
**	�� �� ֵ :
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
            HAL_NVIC_DisableIRQ(EXTI0_IRQn);		/* ��ֹ�ⲿ�ж� */
            if(ad7175_cnt % 2 == 0)
            {
                AD7175_Process();   				/* AD7175���� */
            }
            HAL_NVIC_EnableIRQ(EXTI0_IRQn);			/* ʹ���ⲿ�ж� */
        }
    }

    if(GPIO_Pin == AD7124_RDY_PIN)
    {
        if(AD7124_Start_Read == 1)
        {
            ad7124_cnt++;
            HAL_NVIC_DisableIRQ(AD7124_EXTIIRQ);	/* ��ֹ�ⲿ�ж� */
            if(ad7124_cnt % 2 == 0)
            {
                AD7124_Process();					/* AD7124���� */
            }
            HAL_NVIC_EnableIRQ(AD7124_EXTIIRQ);		/* ʹ���ⲿ�ж� */
        }
    }
}


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

