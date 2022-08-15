/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_ads131.c
** Author      : Wang Liang
** Date        : 2022-01-10
** Description : bsp_ads131 source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "spi.h"
#include "bsp_ads131.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
/* CS�������ѡ����ַ�ʽ */
#define ADS131_CS_NUM 16u
ADS131_CsType ADS131_CsPortPinTab[ADS131_CS_NUM] = 
{
	{I_STM32_ADC_CS1_1_GPIO_Port, I_STM32_ADC_CS1_1_Pin},
	{V_STM32_ADC_CS1_2_GPIO_Port, V_STM32_ADC_CS1_2_Pin},
	{I_STM32_ADC_CS2_1_GPIO_Port, I_STM32_ADC_CS2_1_Pin},
	{V_STM32_ADC_CS2_2_GPIO_Port, V_STM32_ADC_CS2_2_Pin},
	{I_STM32_ADC_CS3_1_GPIO_Port, I_STM32_ADC_CS3_1_Pin},
	{V_STM32_ADC_CS3_2_GPIO_Port, V_STM32_ADC_CS3_2_Pin},
	{I_STM32_ADC_CS4_1_GPIO_Port, I_STM32_ADC_CS4_1_Pin},
	{V_STM32_ADC_CS4_2_GPIO_Port, V_STM32_ADC_CS4_2_Pin},
	{I_STM32_ADC_CS5_1_GPIO_Port, I_STM32_ADC_CS5_1_Pin},
	{V_STM32_ADC_CS5_2_GPIO_Port, V_STM32_ADC_CS5_2_Pin},
	{I_STM32_ADC_CS6_1_GPIO_Port, I_STM32_ADC_CS6_1_Pin},
	{V_STM32_ADC_CS6_2_GPIO_Port, V_STM32_ADC_CS6_2_Pin},
	{I_STM32_ADC_CS7_1_GPIO_Port, I_STM32_ADC_CS7_1_Pin},
	{V_STM32_ADC_CS7_2_GPIO_Port, V_STM32_ADC_CS7_2_Pin},
	{I_STM32_ADC_CS8_1_GPIO_Port, I_STM32_ADC_CS8_1_Pin},
	{V_STM32_ADC_CS8_2_GPIO_Port, V_STM32_ADC_CS8_2_Pin},
};

ADS131_DataType ADS131_Data = 
{
	.AdsCollectCnt = 0,
	.AdsCsFlag = 1,
	.Vbat[0] = 0, .Vbat[1] = 0, .Vbat[2] = 0, .Vbat[3] = 0, 
	.Vbat[4] = 0, .Vbat[5] = 0, .Vbat[6] = 0, .Vbat[7] = 0,
	.Vtz[0] = 0,  .Vtz[1] = 0,  .Vtz[2] = 0,  .Vtz[3] = 0,
	.Vtz[4] = 0,  .Vtz[5] = 0,  .Vtz[6] = 0,  .Vtz[7] = 0,
	.Imod[0] = 0, .Imod[1] = 0, .Imod[2] = 0, .Imod[3] = 0,
	.Imod[4] = 0, .Imod[5] = 0, .Imod[6] = 0, .Imod[7] = 0,
};

adc_channel_data AdsData = {0};                                 //ADS131M01��������

static uint16_t             registerMap[NUM_REGISTERS];         //ADS131M0x�Ĵ���ӳ���

const static uint8_t        wlength_byte_values[] = {2, 3, 4, 4}; // Array of SPI word lengths

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
*   �� �� ��: ADS131_CS_Disable
*   ����˵��: CSʧ��
*   ��    ��: CS_Signal,��������
*   �� �� ֵ: ��
**********************************************************************************************************/
void ADS131_CS_Disable(CS_Type CS_Signal)
{
    switch (CS_Signal)
    {
		case V_CS1:
			HAL_GPIO_WritePin(V_STM32_ADC_CS1_2_GPIO_Port,V_STM32_ADC_CS1_2_Pin,GPIO_PIN_SET);
			break;
		case V_CS2:
			HAL_GPIO_WritePin(V_STM32_ADC_CS2_2_GPIO_Port,V_STM32_ADC_CS2_2_Pin,GPIO_PIN_SET);
			break;
		case V_CS3:
			HAL_GPIO_WritePin(V_STM32_ADC_CS3_2_GPIO_Port,V_STM32_ADC_CS3_2_Pin,GPIO_PIN_SET);
			break;
		case V_CS4:
			HAL_GPIO_WritePin(V_STM32_ADC_CS4_2_GPIO_Port,V_STM32_ADC_CS4_2_Pin,GPIO_PIN_SET);
			break;
		case V_CS5:
			HAL_GPIO_WritePin(V_STM32_ADC_CS5_2_GPIO_Port,V_STM32_ADC_CS5_2_Pin,GPIO_PIN_SET);
			break;
		case V_CS6:
			HAL_GPIO_WritePin(V_STM32_ADC_CS6_2_GPIO_Port,V_STM32_ADC_CS6_2_Pin,GPIO_PIN_SET);
			break;
		case V_CS7:
			HAL_GPIO_WritePin(V_STM32_ADC_CS7_2_GPIO_Port,V_STM32_ADC_CS7_2_Pin,GPIO_PIN_SET);
			break;
		case V_CS8:
			HAL_GPIO_WritePin(V_STM32_ADC_CS8_2_GPIO_Port,V_STM32_ADC_CS8_2_Pin,GPIO_PIN_SET);
			break;
		case I_CS1:
			HAL_GPIO_WritePin(I_STM32_ADC_CS1_1_GPIO_Port,I_STM32_ADC_CS1_1_Pin,GPIO_PIN_SET);
			break;
		case I_CS2:
			HAL_GPIO_WritePin(I_STM32_ADC_CS2_1_GPIO_Port,I_STM32_ADC_CS2_1_Pin,GPIO_PIN_SET);
			break;
		case I_CS3:
			HAL_GPIO_WritePin(I_STM32_ADC_CS3_1_GPIO_Port,I_STM32_ADC_CS3_1_Pin,GPIO_PIN_SET);
			break;
		case I_CS4:
			HAL_GPIO_WritePin(I_STM32_ADC_CS4_1_GPIO_Port,I_STM32_ADC_CS4_1_Pin,GPIO_PIN_SET);
			break;
		case I_CS5:
			HAL_GPIO_WritePin(I_STM32_ADC_CS5_1_GPIO_Port,I_STM32_ADC_CS5_1_Pin,GPIO_PIN_SET);
			break;
		case I_CS6:
			HAL_GPIO_WritePin(I_STM32_ADC_CS6_1_GPIO_Port,I_STM32_ADC_CS6_1_Pin,GPIO_PIN_SET);
			break;
		case I_CS7:
			HAL_GPIO_WritePin(I_STM32_ADC_CS7_1_GPIO_Port,I_STM32_ADC_CS7_1_Pin,GPIO_PIN_SET);
			break;
		case I_CS8:
			HAL_GPIO_WritePin(I_STM32_ADC_CS8_1_GPIO_Port,I_STM32_ADC_CS8_1_Pin,GPIO_PIN_SET);
			break;
		case ALL_CS:
			HAL_GPIO_WritePin(V_STM32_ADC_CS1_2_GPIO_Port,V_STM32_ADC_CS1_2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS2_2_GPIO_Port,V_STM32_ADC_CS2_2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS3_2_GPIO_Port,V_STM32_ADC_CS3_2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS4_2_GPIO_Port,V_STM32_ADC_CS4_2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS5_2_GPIO_Port,V_STM32_ADC_CS5_2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS6_2_GPIO_Port,V_STM32_ADC_CS6_2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS7_2_GPIO_Port,V_STM32_ADC_CS7_2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS8_2_GPIO_Port,V_STM32_ADC_CS8_2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS1_1_GPIO_Port,I_STM32_ADC_CS1_1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS2_1_GPIO_Port,I_STM32_ADC_CS2_1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS3_1_GPIO_Port,I_STM32_ADC_CS3_1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS4_1_GPIO_Port,I_STM32_ADC_CS4_1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS5_1_GPIO_Port,I_STM32_ADC_CS5_1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS6_1_GPIO_Port,I_STM32_ADC_CS6_1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS7_1_GPIO_Port,I_STM32_ADC_CS7_1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS8_1_GPIO_Port,I_STM32_ADC_CS8_1_Pin,GPIO_PIN_SET);
			break;
		default:
			break;
    }
}

/**********************************************************************************************************
*   �� �� ��: ADS131_CS_Enable
*   ����˵��: CSʹ��
*   ��    ��: CS_Signal,��������
*   �� �� ֵ: ��
**********************************************************************************************************/
void ADS131_CS_Enable(CS_Type CS_Signal)
{
    switch (CS_Signal)
    {
		case V_CS1:
			HAL_GPIO_WritePin(V_STM32_ADC_CS1_2_GPIO_Port,V_STM32_ADC_CS1_2_Pin,GPIO_PIN_RESET);
			break;
		case V_CS2:
			HAL_GPIO_WritePin(V_STM32_ADC_CS2_2_GPIO_Port,V_STM32_ADC_CS2_2_Pin,GPIO_PIN_RESET);
			break;
		case V_CS3:
			HAL_GPIO_WritePin(V_STM32_ADC_CS3_2_GPIO_Port,V_STM32_ADC_CS3_2_Pin,GPIO_PIN_RESET);
			break;
		case V_CS4:
			HAL_GPIO_WritePin(V_STM32_ADC_CS4_2_GPIO_Port,V_STM32_ADC_CS4_2_Pin,GPIO_PIN_RESET);
			break;
		case V_CS5:
			HAL_GPIO_WritePin(V_STM32_ADC_CS5_2_GPIO_Port,V_STM32_ADC_CS5_2_Pin,GPIO_PIN_RESET);
			break;
		case V_CS6:
			HAL_GPIO_WritePin(V_STM32_ADC_CS6_2_GPIO_Port,V_STM32_ADC_CS6_2_Pin,GPIO_PIN_RESET);
			break;
		case V_CS7:
			HAL_GPIO_WritePin(V_STM32_ADC_CS7_2_GPIO_Port,V_STM32_ADC_CS7_2_Pin,GPIO_PIN_RESET);
			break;
		case V_CS8:
			HAL_GPIO_WritePin(V_STM32_ADC_CS8_2_GPIO_Port,V_STM32_ADC_CS8_2_Pin,GPIO_PIN_RESET);
			break;
		case I_CS1:
			HAL_GPIO_WritePin(I_STM32_ADC_CS1_1_GPIO_Port,I_STM32_ADC_CS1_1_Pin,GPIO_PIN_RESET);
			break;
		case I_CS2:
			HAL_GPIO_WritePin(I_STM32_ADC_CS2_1_GPIO_Port,I_STM32_ADC_CS2_1_Pin,GPIO_PIN_RESET);
			break;
		case I_CS3:
			HAL_GPIO_WritePin(I_STM32_ADC_CS3_1_GPIO_Port,I_STM32_ADC_CS3_1_Pin,GPIO_PIN_RESET);
			break;
		case I_CS4:
			HAL_GPIO_WritePin(I_STM32_ADC_CS4_1_GPIO_Port,I_STM32_ADC_CS4_1_Pin,GPIO_PIN_RESET);
			break;
		case I_CS5:
			HAL_GPIO_WritePin(I_STM32_ADC_CS5_1_GPIO_Port,I_STM32_ADC_CS5_1_Pin,GPIO_PIN_RESET);
			break;
		case I_CS6:
			HAL_GPIO_WritePin(I_STM32_ADC_CS6_1_GPIO_Port,I_STM32_ADC_CS6_1_Pin,GPIO_PIN_RESET);
			break;
		case I_CS7:
			HAL_GPIO_WritePin(I_STM32_ADC_CS7_1_GPIO_Port,I_STM32_ADC_CS7_1_Pin,GPIO_PIN_RESET);
			break;
		case I_CS8:
			HAL_GPIO_WritePin(I_STM32_ADC_CS8_1_GPIO_Port,I_STM32_ADC_CS8_1_Pin,GPIO_PIN_RESET);
			break;
		case ALL_CS:
			HAL_GPIO_WritePin(V_STM32_ADC_CS1_2_GPIO_Port,V_STM32_ADC_CS1_2_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS2_2_GPIO_Port,V_STM32_ADC_CS2_2_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS3_2_GPIO_Port,V_STM32_ADC_CS3_2_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS4_2_GPIO_Port,V_STM32_ADC_CS4_2_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS5_2_GPIO_Port,V_STM32_ADC_CS5_2_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS6_2_GPIO_Port,V_STM32_ADC_CS6_2_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS7_2_GPIO_Port,V_STM32_ADC_CS7_2_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(V_STM32_ADC_CS8_2_GPIO_Port,V_STM32_ADC_CS8_2_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS1_1_GPIO_Port,I_STM32_ADC_CS1_1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS2_1_GPIO_Port,I_STM32_ADC_CS2_1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS3_1_GPIO_Port,I_STM32_ADC_CS3_1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS4_1_GPIO_Port,I_STM32_ADC_CS4_1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS5_1_GPIO_Port,I_STM32_ADC_CS5_1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS6_1_GPIO_Port,I_STM32_ADC_CS6_1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS7_1_GPIO_Port,I_STM32_ADC_CS7_1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(I_STM32_ADC_CS8_1_GPIO_Port,I_STM32_ADC_CS8_1_Pin,GPIO_PIN_RESET);
			break;
		default:
			break;
    }
}

/**********************************************************************************************************
*   �� �� ��: restoreRegisterDefaults
*   ����˵��: ��λ�Ĵ���Ĭ��ֵ���Ĵ����洢��
*   ��    ��: ��
*   �� �� ֵ: ��
**********************************************************************************************************/
void restoreRegisterDefaults(void)
{
    registerMap[ID_ADDRESS]             =   0x00;               /* NOTE: This a read-only register */
    registerMap[STATUS_ADDRESS]         =   STATUS_DEFAULT;
    registerMap[MODE_ADDRESS]           =   MODE_DEFAULT;
    registerMap[CLOCK_ADDRESS]          =   CLOCK_DEFAULT;
    registerMap[GAIN1_ADDRESS]          =   GAIN1_DEFAULT;
    registerMap[GAIN2_ADDRESS]          =   GAIN2_DEFAULT;
    registerMap[CFG_ADDRESS]            =   CFG_DEFAULT;
    registerMap[THRSHLD_MSB_ADDRESS]    =   THRSHLD_MSB_DEFAULT;
    registerMap[THRSHLD_LSB_ADDRESS]    =   THRSHLD_LSB_DEFAULT;
    registerMap[CH0_CFG_ADDRESS]        =   CH0_CFG_DEFAULT;
    registerMap[CH0_OCAL_MSB_ADDRESS]   =   CH0_OCAL_MSB_DEFAULT;
    registerMap[CH0_OCAL_LSB_ADDRESS]   =   CH0_OCAL_LSB_DEFAULT;
    registerMap[CH0_GCAL_MSB_ADDRESS]   =   CH0_GCAL_MSB_DEFAULT;
    registerMap[CH0_GCAL_LSB_ADDRESS]   =   CH0_GCAL_LSB_DEFAULT;
#if (CHANNEL_COUNT > 1)
    registerMap[CH1_CFG_ADDRESS]        =   CH1_CFG_DEFAULT;
    registerMap[CH1_OCAL_MSB_ADDRESS]   =   CH1_OCAL_MSB_DEFAULT;
    registerMap[CH1_OCAL_LSB_ADDRESS]   =   CH1_OCAL_LSB_DEFAULT;
    registerMap[CH1_GCAL_MSB_ADDRESS]   =   CH1_GCAL_MSB_DEFAULT;
    registerMap[CH1_GCAL_LSB_ADDRESS]   =   CH1_GCAL_LSB_DEFAULT;
#endif
#if (CHANNEL_COUNT > 2)
    registerMap[CH2_CFG_ADDRESS]        =   CH2_CFG_DEFAULT;
    registerMap[CH2_OCAL_MSB_ADDRESS]   =   CH2_OCAL_MSB_DEFAULT;
    registerMap[CH2_OCAL_LSB_ADDRESS]   =   CH2_OCAL_LSB_DEFAULT;
    registerMap[CH2_GCAL_MSB_ADDRESS]   =   CH2_GCAL_MSB_DEFAULT;
    registerMap[CH2_GCAL_LSB_ADDRESS]   =   CH2_GCAL_LSB_DEFAULT;
#endif
#if (CHANNEL_COUNT > 3)
    registerMap[CH3_CFG_ADDRESS]        =   CH3_CFG_DEFAULT;
    registerMap[CH3_OCAL_MSB_ADDRESS]   =   CH3_OCAL_MSB_DEFAULT;
    registerMap[CH3_OCAL_LSB_ADDRESS]   =   CH3_OCAL_LSB_DEFAULT;
    registerMap[CH3_GCAL_MSB_ADDRESS]   =   CH3_GCAL_MSB_DEFAULT;
    registerMap[CH3_GCAL_LSB_ADDRESS]   =   CH3_GCAL_LSB_DEFAULT;
#endif
#if (CHANNEL_COUNT > 4)
    registerMap[CH4_CFG_ADDRESS]        =   CH4_CFG_DEFAULT;
    registerMap[CH4_OCAL_MSB_ADDRESS]   =   CH4_OCAL_MSB_DEFAULT;
    registerMap[CH4_OCAL_LSB_ADDRESS]   =   CH4_OCAL_LSB_DEFAULT;
    registerMap[CH4_GCAL_MSB_ADDRESS]   =   CH4_GCAL_MSB_DEFAULT;
    registerMap[CH4_GCAL_LSB_ADDRESS]   =   CH4_GCAL_LSB_DEFAULT;
#endif
#if (CHANNEL_COUNT > 5)
    registerMap[CH5_CFG_ADDRESS]        =   CH5_CFG_DEFAULT;
    registerMap[CH5_OCAL_MSB_ADDRESS]   =   CH5_OCAL_MSB_DEFAULT;
    registerMap[CH5_OCAL_LSB_ADDRESS]   =   CH5_OCAL_LSB_DEFAULT;
    registerMap[CH5_GCAL_MSB_ADDRESS]   =   CH5_GCAL_MSB_DEFAULT;
    registerMap[CH5_GCAL_LSB_ADDRESS]   =   CH5_GCAL_LSB_DEFAULT;
#endif
#if (CHANNEL_COUNT > 6)
    registerMap[CH6_CFG_ADDRESS]        =   CH6_CFG_DEFAULT;
    registerMap[CH6_OCAL_MSB_ADDRESS]   =   CH6_OCAL_MSB_DEFAULT;
    registerMap[CH6_OCAL_LSB_ADDRESS]   =   CH6_OCAL_LSB_DEFAULT;
    registerMap[CH6_GCAL_MSB_ADDRESS]   =   CH6_GCAL_MSB_DEFAULT;
    registerMap[CH6_GCAL_LSB_ADDRESS]   =   CH6_GCAL_LSB_DEFAULT;
#endif
#if (CHANNEL_COUNT > 7)
    registerMap[CH7_CFG_ADDRESS]        =   CH7_CFG_DEFAULT;
    registerMap[CH7_OCAL_MSB_ADDRESS]   =   CH7_OCAL_MSB_DEFAULT;
    registerMap[CH7_OCAL_LSB_ADDRESS]   =   CH7_OCAL_LSB_DEFAULT;
    registerMap[CH7_GCAL_MSB_ADDRESS]   =   CH7_GCAL_MSB_DEFAULT;
    registerMap[CH7_GCAL_LSB_ADDRESS]   =   CH7_GCAL_LSB_DEFAULT;
#endif
    registerMap[REGMAP_CRC_ADDRESS]     =   REGMAP_CRC_DEFAULT;
}

/*
*********************************************************************************************************
*   �� �� ��: SPI_ReadWriteByte
*   ����˵��: SPI���ֽڶ�д
*   ��    ��: data��������������
*   �� �� ֵ: �ӻ���������
*********************************************************************************************************
*/
static uint8_t SPI_ReadWriteByte(uint8_t data)
{
    uint8_t Rx_data = 0;
    HAL_SPI_TransmitReceive(&hspi4, &data, &Rx_data, 1, 0xff);
    bsp_DelayUS(1);
    return Rx_data;
}


/*
*********************************************************************************************************
*   �� �� ��: SPI_ReadWriteArrays
*   ����˵��: SPI���ֽڶ�д
*   ��    ��: dataTx[]��������������
              dataRx[]��������������
              byteLength�����������ֽڳ���
              CS_Signal: Ƭѡ�ź�
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void SPI_ReadWriteArrays(const uint8_t dataTx[], uint8_t dataRx[], const uint8_t byteLength, CS_Type CS_Signal)
{
    /*  --- INSERT YOUR CODE HERE ---
     *
     *  This function should send and receive multiple bytes over the SPI.
     *
     *  A typical SPI send/receive sequence may look like the following:
     *  1) Make sure SPI receive buffer is empty
     *  2) Set the /CS pin low (if controlled by GPIO)
     *  3) Send command bytes to SPI transmit buffer
     *  4) Wait for SPI receive interrupt
     *  5) Retrieve data from SPI receive buffer
     *  6) Set the /CS pin high (if controlled by GPIO)
     */

    // Require that dataTx and dataRx are not NULL pointers
    assert(dataTx && dataRx);

    // Set the nCS pin LOW
    ADS131_CS_Enable(CS_Signal);

    // Send all dataTx[] bytes on MOSI, and capture all MISO bytes in dataRx[]
    for (uint8_t i = 0; i < byteLength; i++)
    {
        dataRx[i] = SPI_ReadWriteByte(dataTx[i]);
    }

    // Set the nCS pin HIGH
    ADS131_CS_Disable(CS_Signal);
}

/******************************************************************************
*   �� �� ��: upperByte
*   ����˵��: ת��һ��16λ�ַ���MSB
*   ��    ��: ��ת������
*   �� �� ֵ: 8λMSB
*****************************************************************************/
uint8_t upperByte(uint16_t uint16_Word)
{
    uint8_t msByte;
    msByte = (uint8_t) ((uint16_Word >> 8) & 0x00FF);

    return msByte;
}


/******************************************************************************
*   �� �� ��: lowerByte
*   ����˵��: ת��һ��16λ�ַ���LSB
*   ��    ��: ��ת������
*   �� �� ֵ: 8λLSB
*****************************************************************************/
uint8_t lowerByte(uint16_t uint16_Word)
{
    uint8_t lsByte;
    lsByte = (uint8_t) (uint16_Word & 0x00FF);

    return lsByte;
}

/******************************************************************************
*   �� �� ��: combineBytes
*   ����˵��: ��2��8λ���ݼ�����16λ��
*   ��    ��: upperByte���ϳ��ֵ�MSB
              lowerByte���ϳ��ֵ�LSB
*   �� �� ֵ: 16λ�ϳ���
*****************************************************************************/
uint16_t combineBytes(uint8_t upperByte, uint8_t lowerByte)
{
    uint16_t combinedValue;
    combinedValue = ((uint16_t) upperByte << 8) | ((uint16_t) lowerByte);

    return combinedValue;
}


/******************************************************************************
*   �� �� ��: signExtend
*   ����˵��: ��ADCֵת����һ���з���32λ��
*   ��    ��: dataBytes��ָ��һ��uint8_t��ADCֵ���飬��һ��Ԫ����MSB
*   �� �� ֵ: ת�����32λ�з�����չֵ
*****************************************************************************/
int32_t signExtend(const uint8_t dataBytes[])
{

#ifdef WORD_LENGTH_24BIT

    int32_t upperByte   = ((int32_t) dataBytes[0] << 24);
    int32_t middleByte  = ((int32_t) dataBytes[1] << 16);
    int32_t lowerByte   = ((int32_t) dataBytes[2] << 8);

    return (((int32_t) (upperByte | middleByte | lowerByte)) >> 8);     // Right-shift of signed data maintains signed bit

#elif defined WORD_LENGTH_32BIT_SIGN_EXTEND

    int32_t signByte    = ((int32_t) dataBytes[0] << 24);
    int32_t upperByte   = ((int32_t) dataBytes[1] << 16);
    int32_t middleByte  = ((int32_t) dataBytes[2] << 8);
    int32_t lowerByte   = ((int32_t) dataBytes[3] << 0);

    return (signByte | upperByte | middleByte | lowerByte);

#elif defined WORD_LENGTH_32BIT_ZERO_PADDED

    int32_t upperByte   = ((int32_t) dataBytes[0] << 24);
    int32_t middleByte  = ((int32_t) dataBytes[1] << 16);
    int32_t lowerByte   = ((int32_t) dataBytes[2] << 8);

    return (((int32_t) (upperByte | middleByte | lowerByte)) >> 8);     // Right-shift of signed data maintains signed bit

#elif defined WORD_LENGTH_16BIT_TRUNCATED

    int32_t upperByte   = ((int32_t) dataBytes[0] << 24);
    int32_t lowerByte   = ((int32_t) dataBytes[1] << 16);

    return (((int32_t) (upperByte | lowerByte)) >> 16);                 // Right-shift of signed data maintains signed bit

#endif
}

/******************************************************************************
*   �� �� ��: calculateCRC
*   ����˵��: ����ѡ���CRC����ʽ����16λCRC��
*   ��    ��: dataBytes[]��ָ���У����������һ�ֽ�
              numberBytes��CRCУ���ֽ�����
              initialValue��У���ֵ��Ĭ��0xffff
*   �� �� ֵ: 16λУ��ֵ
*****************************************************************************/
uint16_t calculateCRC(const uint8_t dataBytes[], uint8_t numberBytes, uint16_t initialValue)
{
    /* Check that "dataBytes" is not a null pointer */
    assert(dataBytes != 0x00);

    int         bitIndex, byteIndex;
    bool        dataMSb;						/* Most significant bit of data byte */
    bool        crcMSb;						    /* Most significant bit of crc byte  */
    uint8_t     bytesPerWord = wlength_byte_values[WLENGTH];

    /*
     * Initial value of crc register
     * NOTE: The ADS131M0x defaults to 0xFFFF,
     * but can be set at function call to continue an on-going calculation
     */
    uint16_t crc = initialValue;

#ifdef CRC_CCITT
    /* CCITT CRC polynomial = x^16 + x^12 + x^5 + 1 */
    const uint16_t poly = 0x1021;
#endif

#ifdef CRC_ANSI
    /* ANSI CRC polynomial = x^16 + x^15 + x^2 + 1 */
    const uint16_t poly = 0x8005;
#endif

    //
    // CRC algorithm
    //

    // Loop through all bytes in the dataBytes[] array
    for (byteIndex = 0; byteIndex < numberBytes; byteIndex++)
    {
        // Point to MSb in byte
        bitIndex = 0x80u;

        // Loop through all bits in the current byte
        while (bitIndex > 0)
        {
            // Check MSB's of data and crc
            dataMSb = (bool) (dataBytes[byteIndex] & bitIndex);
            crcMSb  = (bool) (crc & 0x8000u);

            crc <<= 1;              /* Left shift CRC register */

            // Check if XOR operation of MSBs results in additional XOR operations
            if (dataMSb ^ crcMSb)
            {
                crc ^= poly;        /* XOR crc with polynomial */
            }

            /* Shift MSb pointer to the next data bit */
            bitIndex >>= 1;
        }
    }

    return crc;
}

/******************************************************************************
*   �� �� ��: getWordByteLength
*   ����˵��: ��ȡADS131���õ�SPIͨ���ֽڳ��ȣ���ӦregisterMap[]��MODE register
*   ��    ��: ��
*   �� �� ֵ: SPI �ֽڳ��� (2, 3, or 4)
*****************************************************************************/
uint8_t getWordByteLength(void)
{
    return wlength_byte_values[WLENGTH];
}

/******************************************************************************
*   �� �� ��: getRegisterValue
*   ����˵��: ��registerMap��ȡ���ر���ļĴ���ֵ��ע�������Ҫȡ�ص�ǰ�豸�Ĵ���ֵ��
              ����ʹ��readSingleRegister()
*   ��    ��: address���Ĵ�����ַ
*   �� �� ֵ: �޷�16λ�Ĵ���ֵ
*****************************************************************************/
uint16_t getRegisterValue(uint8_t address)
{
    assert(address < NUM_REGISTERS);
    return registerMap[address];
}


/******************************************************************************
*   �� �� ��: enforce_selected_device_modes
*   ����˵��: ����Ԥѡģʽ(RX_CRC_EN, WLENGTH, etc.)�޸�MODE�Ĵ�����ֵ
*   ��    ��: data:Ҫд��ļĴ���ֵ
*   �� �� ֵ: �޸ĺ�ļĴ���ֵ
*****************************************************************************/
uint16_t enforce_selected_device_modes(uint16_t data)
{


    ///////////////////////////////////////////////////////////////////////////
    // Enforce RX_CRC_EN setting

#ifdef ENABLE_CRC_IN
    // When writing to the MODE register, ensure RX_CRC_EN bit is ALWAYS set
    data |= MODE_RX_CRC_EN_ENABLED;
#else
    // When writing to the MODE register, ensure RX_CRC_EN bit is NEVER set
    data &= ~MODE_RX_CRC_EN_ENABLED;
#endif // ENABLE_CRC_IN


    ///////////////////////////////////////////////////////////////////////////
    // Enforce WLENGH setting

#ifdef WORD_LENGTH_24BIT
    // When writing to the MODE register, ensure WLENGTH bits are ALWAYS set to 01b
    data = (data & ~MODE_WLENGTH_MASK) | MODE_WLENGTH_24BIT;
#elif defined WORD_LENGTH_32BIT_SIGN_EXTEND
    // When writing to the MODE register, ensure WLENGH bits are ALWAYS set to 11b
    data = (data & ~MODE_WLENGTH_MASK) | MODE_WLENGTH_32BIT_MSB_SIGN_EXT;
#elif defined WORD_LENGTH_32BIT_ZERO_PADDED
    // When writing to the MODE register, ensure WLENGH bits are ALWAYS set to 10b
    data = (data & ~MODE_WLENGTH_MASK) | MODE_WLENGTH_32BIT_LSB_ZEROES;
#elif defined WORD_LENGTH_16BIT_TRUNCATED
    // When writing to the MODE register, ensure WLENGH bits are ALWAYS set to 00b
    data = (data & ~MODE_WLENGTH_MASK) | MODE_WLENGTH_16BIT;
#endif


    ///////////////////////////////////////////////////////////////////////////
    // Enforce DRDY_FMT setting

#ifdef DRDY_FMT_PULSE
    // When writing to the MODE register, ensure DRDY_FMT bit is ALWAYS set
    data = (data & ~MODE_DRDY_FMT_MASK) | MODE_DRDY_FMT_NEG_PULSE_FIXED_WIDTH;
#else
    // When writing to the MODE register, ensure DRDY_FMT bit is NEVER set
    data = (data & ~MODE_DRDY_FMT_MASK) | MODE_DRDY_FMT_LOGIC_LOW;
#endif


    ///////////////////////////////////////////////////////////////////////////
    // Enforce CRC_TYPE setting

#ifdef CRC_CCITT
    // When writing to the MODE register, ensure CRC_TYPE bit is NEVER set
    data = (data & ~STATUS_CRC_TYPE_MASK) | STATUS_CRC_TYPE_16BIT_CCITT;
#elif defined CRC_ANSI
    // When writing to the MODE register, ensure CRC_TYPE bit is ALWAYS set
    data = (data & ~STATUS_CRC_TYPE_MASK) | STATUS_CRC_TYPE_16BIT_ANSI;
#endif

    // Return modified register data
    return data;
}


/******************************************************************************
*   �� �� ��: readSingleRegister
*   ����˵��: �������Ĵ���ֵ
*   ��    ��: address��Ҫ�����ļĴ�����ַ
              CS_Signal:Ƭѡ�ź�
*   �� �� ֵ: ������ֵ
*****************************************************************************/
uint16_t readSingleRegister(uint8_t address,CS_Type CS_Signal)
{
    /* Check that the register address is in range */
    assert(address < NUM_REGISTERS);

// Build TX and RX byte array
#ifdef ENABLE_CRC_IN
    uint8_t dataTx[8] = { 0 };      // 2 words, up to 4 bytes each = 8 bytes maximum
    uint8_t dataRx[8] = { 0 };
#else
    uint8_t dataTx[4] = { 0 };      // 1 word, up to 4 bytes long = 4 bytes maximum
    uint8_t dataRx[4] = { 0 };
#endif
    uint16_t opcode = OPCODE_RREG | (((uint16_t) address) << 7);
    uint8_t numberOfBytes = buildSPIarray(&opcode, 1, dataTx);

    // [FRAME 1] Send RREG command
    SPI_ReadWriteArrays(dataTx, dataRx, numberOfBytes, CS_Signal);

    // [FRAME 2] Send NULL command to retrieve the register data
    registerMap[address] = sendCommand(OPCODE_NULL,CS_Signal);

    return registerMap[address];
}


/******************************************************************************
*   �� �� ��: writeSingleRegister
*   ����˵��: �Ե����Ĵ���д����
*   ��    ��: address��Ҫд��ļĴ�����ַ
              data��Ҫд��ļĴ���ֵ
              CS_Signal:Ƭѡ�ź�
*   �� �� ֵ: ��
*****************************************************************************/
void writeSingleRegister(uint8_t address, uint16_t data, CS_Type CS_Signal)
{
    /* Check that the register address is in range */
    assert(address < NUM_REGISTERS);

    // (OPTIONAL) Enforce certain register field values when
    // writing to the MODE register to fix the operation mode
    if (MODE_ADDRESS == address)
    {
        data = enforce_selected_device_modes(data);
    }

    // Build TX and RX byte array
#ifdef ENABLE_CRC_IN
    uint8_t dataTx[12] = { 0 };     // 3 words, up to 4 bytes each = 12 bytes maximum
    uint8_t dataRx[12] = { 0 };
#else
    uint8_t dataTx[8] = { 0 };      // 2 words, up to 4 bytes long = 8 bytes maximum
    uint8_t dataRx[8] = { 0 };
#endif
    uint16_t opcodes[2];
    opcodes[0] = OPCODE_WREG | (((uint16_t) address) << 7);
    opcodes[1] = data;
    uint8_t numberOfBytes = buildSPIarray(&opcodes[0], 2, dataTx);

    // Send command
    SPI_ReadWriteArrays(dataTx, dataRx, numberOfBytes, CS_Signal);

    // Update internal array
    registerMap[address] = data;

    // (RECOMMENDED) Read back register to confirm register write was successful
    readSingleRegister(address,CS_Signal);

    // NOTE: Enabling the CRC words in the SPI command will NOT prevent an invalid W
}


/******************************************************************************
*   �� �� ��: buildSPIarray
*   ����˵��: ���ݲ�����͵�ǰ�豸�ֳ�����TX������������
*   ��    ��: opcodeArray[]��ָ��һ��16λSPI��������������
              numberOpcodes�������ṩ�Ĳ���������
              byteArray[]��ָ��һ��Ҫ������豸��8λ��������
*   �� �� ֵ: byteArray[]����Ĵ�С
*****************************************************************************/
uint8_t buildSPIarray(const uint16_t opcodeArray[], uint8_t numberOpcodes, uint8_t byteArray[])
{
    /*
     * Frame size = opcode word(s) + optional CRC word
     * Number of bytes per word = 2, 3, or 4
     * Total bytes = bytes per word * number of words
     */
    uint8_t numberWords     = numberOpcodes + (SPI_CRC_ENABLED ? 1 : 0);
    uint8_t bytesPerWord    = getWordByteLength();
    uint8_t numberOfBytes   = numberWords * bytesPerWord;

    int i;
    for (i = 0; i < numberOpcodes; i++)
    {
        // NOTE: Be careful not to accidentally overflow the array here.
        // The array and opcodes are defined in the calling function, so
        // we are trusting that no mistakes were made in the calling function!
        byteArray[(i*bytesPerWord) + 0] = upperByte(opcodeArray[i]);
        byteArray[(i*bytesPerWord) + 1] = lowerByte(opcodeArray[i]);
    }

#ifdef ENABLE_CRC_IN
    // Calculate CRC and put it into TX array
    uint16_t crcWord = calculateCRC(&byteArray[0], numberOfBytes, 0xFFFF);
    byteArray[(i*bytesPerWord) + 0] = upperByte(crcWord);
    byteArray[(i*bytesPerWord) + 1] = lowerByte(crcWord);
#endif

    return numberOfBytes;
}

/******************************************************************************
*   �� �� ��: sendCommand
*   ����˵��: ���Ͳ���ָ�ADS131(NULL, STANDBY, or WAKEUP)
*   ��    ��: opcode ����ָ���ֽ�
              CS_Signal Ƭѡ����
*   �� �� ֵ: ADC��Ӧ�ֽ�
*****************************************************************************/
uint16_t sendCommand(uint16_t opcode,CS_Type CS_Signal)
{
    /* Assert if this function is used to send any of the following opcodes */
    assert(OPCODE_RREG != opcode);      /* Use "readSingleRegister()"   */
    assert(OPCODE_WREG != opcode);      /* Use "writeSingleRegister()"  */
    assert(OPCODE_LOCK != opcode);      /* Use "lockRegisters()"        */
    assert(OPCODE_UNLOCK != opcode);    /* Use "unlockRegisters()"      */
    assert(OPCODE_RESET != opcode);     /* Use "resetDevice()"          */

    // Build TX and RX byte array
#ifdef ENABLE_CRC_IN
    uint8_t dataTx[8] = { 0 };      // 2 words, up to 4 bytes each = 8 bytes maximum
    uint8_t dataRx[8] = { 0 };
#else
    uint8_t dataTx[4] = { 0 };      // 1 word, up to 4 bytes long = 4 bytes maximum
    uint8_t dataRx[4] = { 0 };
#endif
    uint8_t numberOfBytes = buildSPIarray(&opcode, 1, dataTx);

    /* Set the nCS pin LOW */
    ADS131_CS_Enable(CS_Signal);

    // Send the opcode (and crc word, if enabled)
    int i;
    for (i = 0; i < numberOfBytes; i++)
    {
        dataRx[i] = SPI_ReadWriteByte(dataTx[i]);
    }

    /* Set the nCS pin HIGH */
    ADS131_CS_Disable(CS_Signal);

    // Combine response bytes and return as a 16-bit word
    uint16_t adcResponse = combineBytes(dataRx[0], dataRx[1]);
    return adcResponse;
}


/******************************************************************************
*   �� �� ��: resetDevice
*   ����˵��: ��λ�豸
*   ��    ��: CS_Signal Ƭѡ����
*   �� �� ֵ: ��
*****************************************************************************/
void resetDevice(CS_Type CS_Signal)
{
    // Build TX and RX byte array
#ifdef ENABLE_CRC_IN
    uint8_t dataTx[8] = { 0 };      // 2 words, up to 4 bytes each = 8 bytes maximum
    //uint8_t dataRx[8] = { 0 };    // Only needed if capturing data
#else
    uint8_t dataTx[4] = { 0 };      // 1 word, up to 4 bytes long = 4 bytes maximum
    //uint8_t dataRx[4] = { 0 };    // Only needed if capturing data
#endif
    uint16_t opcode         = OPCODE_RESET;
    uint8_t numberOfBytes   = buildSPIarray(&opcode, 1, dataTx);

    uint8_t bytesPerWord    = wlength_byte_values[WLENGTH];
    uint8_t wordsInFrame    = CHANNEL_COUNT + 2;

    // Set the nCS pin LOW
    ADS131_CS_Enable(CS_Signal);

    // Send the opcode (and CRC word, if enabled)
    int i;
    for (i = 0; i < numberOfBytes; i++)
    {
        SPI_ReadWriteByte(dataTx[i]);
    }

    // Finish sending remaining bytes
    for (i = numberOfBytes; i < (wordsInFrame * bytesPerWord); i++)
    {
        SPI_ReadWriteByte(0x00);
    }

    // NOTE: The ADS131M0x's next response word should be (0xFF20 | CHANCNT),
    // if the response is 0x0011 (acknowledge of RESET command), then the device
    // did not receive a full SPI frame and the reset did not occur!

    // Set the nCS pin HIGH
    ADS131_CS_Disable(CS_Signal);

    // tSRLRST delay, ~1ms
    bsp_DelayMS(1);

    // Update register setting array to keep software in sync with device
    restoreRegisterDefaults();

    // Write to MODE register to enforce mode settings
    writeSingleRegister(MODE_ADDRESS, MODE_DEFAULT,CS_Signal);
}

/******************************************************************************
*   �� �� ��: lockRegisters
*   ����˵��: ���ͼĴ�������ָ�����֤����״̬
*   ��    ��: CS_Signal Ƭѡ����
*   �� �� ֵ: ����ture ��ʾERR
*****************************************************************************/
bool lockRegisters(CS_Type CS_Signal)
{
    bool b_lock_error;

    // Build TX and RX byte array
#ifdef ENABLE_CRC_IN
    uint8_t dataTx[8] = { 0 };      // 2 words, up to 4 bytes each = 8 bytes maximum
    uint8_t dataRx[8] = { 0 };
#else
    uint8_t dataTx[4] = { 0 };      // 1 word, up to 4 bytes long = 4 bytes maximum
    uint8_t dataRx[4] = { 0 };
#endif
    uint16_t opcode         = OPCODE_LOCK;
    uint8_t numberOfBytes   = buildSPIarray(&opcode, 1, dataTx);

    // Send command
    SPI_ReadWriteArrays(dataTx, dataRx, numberOfBytes,CS_Signal);

    /* (OPTIONAL) Check for SPI errors by sending the NULL command and checking STATUS */

    /* (OPTIONAL) Read back the STATUS register and check if LOCK bit is set... */
    readSingleRegister(STATUS_ADDRESS,CS_Signal);
    if (!SPI_LOCKED) {
        b_lock_error = true;
    }

    /* If the STATUS register is NOT read back,
     * then make sure to manually update the global register map variable... */
    //registerMap[STATUS_ADDRESS]  |= STATUS_LOCK_LOCKED;

    /* (OPTIONAL) Error handler */
    if (b_lock_error)
    {
        // Insert error handler function call here...
    }

    return b_lock_error;
}

/******************************************************************************
*   �� �� ��: unlockRegisters
*   ����˵��: ���ͼĴ�������ָ�����֤����״̬
*   ��    ��: CS_Signal Ƭѡ����
*   �� �� ֵ: ����ture ��ʾERR
*****************************************************************************/
bool unlockRegisters(CS_Type CS_Signal)
{
    bool b_unlock_error;

    // Build TX and RX byte array
#ifdef ENABLE_CRC_IN
    uint8_t dataTx[8] = { 0 };      // 2 words, up to 4 bytes each = 8 bytes maximum
    uint8_t dataRx[8] = { 0 };
#else
    uint8_t dataTx[4] = { 0 };      // 1 word, up to 4 bytes long = 4 bytes maximum
    uint8_t dataRx[4] = { 0 };
#endif
    uint16_t opcode = OPCODE_UNLOCK;
    uint8_t numberOfBytes = buildSPIarray(&opcode, 1, dataTx);

    // Send command
    SPI_ReadWriteArrays(dataTx, dataRx, numberOfBytes,CS_Signal);

    /* (OPTIONAL) Check for SPI errors by sending the NULL command and checking STATUS */

    /* (OPTIONAL) Read the STATUS register and check if LOCK bit is cleared... */
    readSingleRegister(STATUS_ADDRESS,CS_Signal);
    if (SPI_LOCKED) {
        b_unlock_error = true;
    }

    /* If the STATUS register is NOT read back,
     * then make sure to manually update the global register map variable... */
    //registerMap[STATUS_ADDRESS]  &= !STATUS_LOCK_LOCKED;

    /* (OPTIONAL) Error handler */
    if (b_unlock_error)
    {
        // Insert error handler function call here...
    }

    return b_unlock_error;
}


/******************************************************************************
*   �� �� ��: readData
*   ����˵��: ��ȡADCֵ���ú���������DRDYΪ0ʱ���ö���DRDY�ź��½���
*   ��    ��: *DataStruct ָ��adc_channel_data���ͽṹ��
              CS_Signal Ƭѡ����
*   �� �� ֵ: ����ture ���̽�⵽CRC����
*****************************************************************************/
bool readData(adc_channel_data *DataStruct,CS_Type CS_Signal)
{
    int i;
    uint8_t crcTx[4]                        = { 0 };
    uint8_t dataRx[4]                       = { 0 };
    uint8_t bytesPerWord                    = getWordByteLength();

#ifdef ENABLE_CRC_IN
    // Build CRC word (only if "RX_CRC_EN" register bit is enabled)
    uint16_t crcWordIn = calculateCRC(&DataTx[0], bytesPerWord * 2, 0xFFFF);
    crcTx[0] = upperByte(crcWordIn);
    crcTx[1] = lowerByte(crcWordIn);
#endif

    /* Set the nCS pin LOW */
    ADS131_CS_Enable(CS_Signal);

    // Send NULL word, receive response word
    for (i = 0; i < bytesPerWord; i++)
    {
        dataRx[i] = SPI_ReadWriteByte(0x00);
    }
    DataStruct->response = combineBytes(dataRx[0], dataRx[1]);

    // (OPTIONAL) Do something with the response (STATUS) word.
    // ...Here we only use the response for calculating the CRC-OUT
    //uint16_t crcWord = calculateCRC(&dataRx[0], bytesPerWord, 0xFFFF);

    // (OPTIONAL) Ignore CRC error checking
    uint16_t crcWord = 0;

    // Send 2nd word, receive channel 1 data
    for (i = 0; i < bytesPerWord; i++)
    {
        dataRx[i] = SPI_ReadWriteByte(crcTx[i]);
    }
    DataStruct->channel0 = signExtend(&dataRx[0]) & 0xFFFFFF;;
    //crcWord = calculateCRC(&dataRx[0], bytesPerWord, crcWord);

#if (CHANNEL_COUNT > 1)

    // Send 3rd word, receive channel 2 data
    for (i = 0; i < bytesPerWord; i++)
    {
        dataRx[i] = SPI_ReadWriteByte(0x00);
    }
    DataStruct->channel1 = 0xFFFFFF - (signExtend(&dataRx[0])&0xFFFFFF);
    //crcWord = calculateCRC(&dataRx[0], bytesPerWord, crcWord);

#endif
#if (CHANNEL_COUNT > 2)

    // Send 4th word, receive channel 3 data
    for (i = 0; i < bytesPerWord; i++)
    {
        dataRx[i] = spiSendReceiveByte(0x00);
    }
    DataStruct->channel2 = signExtend(&dataRx[0]);
    //crcWord = calculateCRC(&dataRx[0], bytesPerWord, crcWord);

#endif
#if (CHANNEL_COUNT > 3)

    // Send 5th word, receive channel 4 data
    for (i = 0; i < bytesPerWord; i++)
    {
        dataRx[i] = spiSendReceiveByte(0x00);
    }
    DataStruct->channel3 = signExtend(&dataRx[0]);
    //crcWord = calculateCRC(&dataRx[0], bytesPerWord, crcWord);

#endif
#if (CHANNEL_COUNT > 4)

    // Send 6th word, receive channel 5 data
    for (i = 0; i < bytesPerWord; i++)
    {
        dataRx[i] = spiSendReceiveByte(0x00);
    }
    DataStruct->channel4 = signExtend(&dataRx[0]);
    //crcWord = calculateCRC(&dataRx[0], bytesPerWord, crcWord);

#endif
#if (CHANNEL_COUNT > 5)

    // Send 7th word, receive channel 6 data
    for (i = 0; i < bytesPerWord; i++)
    {
        dataRx[i] = spiSendReceiveByte(0x00);
    }
    DataStruct->channel5 = signExtend(&dataRx[0]);
    //crcWord = calculateCRC(&dataRx[0], bytesPerWord, crcWord);

#endif
#if (CHANNEL_COUNT > 6)

    // Send 8th word, receive channel 7 data
    for (i = 0; i < bytesPerWord; i++)
    {
        dataRx[i] = spiSendReceiveByte(0x00);
    }
    DataStruct->channel6 = signExtend(&dataRx[0]);
    //crcWord = calculateCRC(&dataRx[0], bytesPerWord, crcWord);

#endif
#if (CHANNEL_COUNT > 7)

    // Send 9th word, receive channel 8 data
    for (i = 0; i < bytesPerWord; i++)
    {
        dataRx[i] = spiSendReceiveByte(0x00);
    }
    DataStruct->channel7 = signExtend(&dataRx[0]);
    //crcWord = calculateCRC(&dataRx[0], bytesPerWord, crcWord);

#endif

    // Send the next word, receive CRC data
    for (i = 0; i < bytesPerWord; i++)
    {
        dataRx[i] = SPI_ReadWriteByte(0x00);
    }
    DataStruct->crc = combineBytes(dataRx[0], dataRx[1]);

    /* NOTE: If we continue calculating the CRC with a matching CRC, the result should be zero.
     * Any non-zero result will indicate a mismatch.
     */
    //crcWord = calculateCRC(&dataRx[0], bytesPerWord, crcWord);

    /* Set the nCS pin HIGH */
    ADS131_CS_Disable(CS_Signal);

    // Returns true when a CRC error occurs
    return ((bool) crcWord);
}

/*
*********************************************************************************************************
*   �� �� ��: ADS131Ctrl_Init
*   ����˵��: IO��ʼ��
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ADS131Ctrl_Init(void)
{
    MX_SPI4_Init();		                               // ADS131�ӿ�����: SPI4
    ADS131_CS_Disable(ALL_CS);                         // CS Disable ALL
    __HAL_SPI_ENABLE(&hspi4);                          // ʹ��SPI4

    /*������������ADS131*/
    for(CS_Type i=V_CS1; i<=I_CS8; i=i<<1)
    {
        resetDevice(i);                                    //��λ�豸
        restoreRegisterDefaults();                         //���ؼĴ���Ĭ��ֵ��
        uint16_t response = sendCommand(OPCODE_NULL,i);    //���͵�һ����ָ��������õ���һ����Ӧ��
        writeSingleRegister(CLOCK_ADDRESS, (CLOCK_DEFAULT & ~CLOCK_OSR_MASK) | CLOCK_OSR_256,i);   //����ʱ�ӼĴ�����ʹ��channel0��1����������=256�������������16KSPS������ģʽ����
//        writeSingleRegister(MODE_ADDRESS, (MODE_DEFAULT & ~MODE_DRDY_SEL_MASK)|MODE_DRDY_SEL_LOGIC_OR,i); //����ģʽ�Ĵ�����ʹ�����ã�����CRC��24BIT����ģʽ������SPI��ʱ��RDYԴ=ʹ��ͨ�������㣬RDY��ת�����ݿ���
    }
}

/**********************************************************************************************************
**	�� �� �� : ADS131ReadData_Start()
**	����˵�� : ADS131��ʼ��ȡ���ݺ���
**	��    �� : CsFlag - CS��־λ
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADS131ReadData_Start(uint16_t CsFlag)
{
	ADS131_CS_Enable((CS_Type)CsFlag);
}

/**********************************************************************************************************
**	�� �� �� : ADS131DataHandle()
**	����˵�� : ADS131���ݴ�����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADS131DataHandle(void)
{
	//����ADS131ѭ����ȡ����
	ADS131ReadData_Start(ADS131_Data.AdsCsFlag);
	
	//�˲���������Ϊ100�Σ�ѭ���ɼ�100�κ���о�ֵ�˲���ת��
	if(ADS131_Data.AdsCollectCnt >= ADS131_FILTER_NUM)
	{
		ADS131_Data.AdsCollectCnt = 0x00;
		
		for(uint8_t i = 0; i < 8; i++)
		{
			//�����ص�ѹ��̽���ѹ����·����
			DebugInqPara.Ro.Vsense[i] = ((ADS131_Data.Vbat[i]/ADS131_FILTER_NUM)*ADS131_REF_VOL/(0x01<<23)) / GAIN_VOL;
			DebugInqPara.Ro.Vtz[i] = ((ADS131_Data.Vtz[i]/ADS131_FILTER_NUM)*ADS131_REF_VOL/(0x01<<23)) / GAIN_VOL;
			DebugInqPara.Ro.Ibp[i] = ((ADS131_Data.Imod[i]/ADS131_FILTER_NUM)*ADS131_REF_VOL/(0x01<<23)) / GAIN_IMOD;
		}
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

