/*
*********************************************************************************************************
*
*	ģ������ : ad7175_8 ����ģ��(8��ȫ���ͨ����16������ͨ�� 24λADC�����λ�Ƿ���λ)
*	�ļ����� : ad7175_8.c
*	��    �� : V1.0
*	˵    �� : ad7175_8ģ���CPU֮�����SPI�ӿڡ��������������Ӳ��SPI��
*
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0    2020-03-27   ZYL      ��ʽ����
*********************************************************************************************************
*/

#include "main.h"
/*
    AD7175��������:
    1��ģ�ⲿ�ֹ���5V;
    2��SPI���ֽӿڵ�ƽ��3.3V
    3���ο���ѹ2.5V (���ã�
    4�������ѹ��Χ��
    5��SPIʱ�����10M��ʵ��ʹ��5.625M
*/

/* Error codes */

#define INVALID_VAL -1 /* Invalid argument */

#define COMM_ERR    -2 /* Communication error on receive */

#define TIMEOUT     -3 /* A timeout has occured */


/* ����GPIO�˿� */
#define AD7175_PORT_CS              GPIOD
#define AD7175_PIN_CS               GPIO_PIN_0

#define AD7175_PORT_CS_ERROR        GPIOD
#define AD7175_PIN_ERROR            GPIO_PIN_1

/* ���������0����1�ĺ� */

#define AD7175_CS_0()           HAL_GPIO_WritePin(AD7175_PORT_CS, AD7175_PIN_CS, GPIO_PIN_RESET);
#define AD7175_CS_1()           HAL_GPIO_WritePin(AD7175_PORT_CS, AD7175_PIN_CS, GPIO_PIN_SET);
#define AD7175_ERROR_IS_HIGH()  (HAL_GPIO_ReadPin(AD7175_PORT_ERROR, AD7175_PORT_ERROR) == SET)

uint8_t AD7175_Start_Read=0;


/*
*********************************************************************************************************
*	�� �� ��: Delay
*	����˵��:
*	��    ��:
*	�� �� ֵ: ��
*********************************************************************************************************
*/

static void Delay(uint32_t cnt)
{
    uint32_t i;
    for (i = 0; i < cnt; i++);
}
/*
*********************************************************************************************************
*	�� �� ��: InitAD7175
*	����˵��: ��ʼ��AD7175
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

int32_t InitAD7175(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    int32_t ret = 0;
    __HAL_RCC_GPIOD_CLK_ENABLE();                   /* ��GPIODʱ�� */

    GPIO_Initure.Pin =  AD7175_PIN_CS;              /*������� */
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;        /* ��Ϊ����ģʽ */
    GPIO_Initure.Pull = GPIO_NOPULL;                /* ���������費ʹ�� */
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;           /* IO������ٶ� */
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);            //GPIOD

    GPIO_Initure.Pin = AD7175_PIN_ERROR;            /*�������� */
    GPIO_Initure.Mode = GPIO_MODE_INPUT;            /* ��Ϊ����� */
    GPIO_Initure.Pull = GPIO_NOPULL;                /* ���������費ʹ�� */
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;           /* IO������ٶ� */
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);            //GPIOD
    AD7175_CS_1();
    ret=AD7175_CfgADC();
    if(AD7175_Start_Read==0)
    {
        AD7175_Start_Read=1;
        AD7175_CS_0();
    }
    return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: AD7175_Reset
*	����˵��: AD7175��λ
*	��    ��:
*
*	�� �� ֵ: -1����
*********************************************************************************************************
*/
int32_t AD7175_Reset(void)
{
    int32_t ret = 0;
    uint8_t Txdata[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    AD7175_CS_0();
    ret=HAL_SPI_TransmitReceive(&hspi3, Txdata, Rxdata,8,100*8);
    AD7175_CS_1();

    return ret;

}

/*
*********************************************************************************************************
*	�� �� ��: AD7175_WriteReg
*	����˵��: AD7175д�Ĵ���,
*	��    ��:
*
*	�� �� ֵ: -1����
*********************************************************************************************************
*/
int32_t AD7175_WriteReg(uint8_t addr,uint32_t Value,uint8_t size)
{
    int32_t ret = 0;
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t i        = 0;
    uint8_t crc8     = 0;

    /* Build the Command word */
    Txdata[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_WR | AD717X_COMM_REG_RA(addr);

    /* Fill the write buffer */
    for(i = 0; i < size; i++)
    {
        Txdata[size - i] = Value & 0xFF;
        Value >>= 8;
    }

    /* Compute the CRC */
    //if(useCRC != AD717X_DISABLE)
    {
        crc8 = AD7175_ComputeCRC8(Txdata, size + 1);
        Txdata[size + 1] = crc8;
    }

    /* Write data to the device */
    AD7175_CS_0();
    if(HAL_SPI_TransmitReceive(&hspi3, Txdata, Rxdata,size+2,100*(size+2))!=0)//1�ֽڵ�ַ��1�ֽ�У��
    ret=-1;
    AD7175_CS_1();

    return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: AD7175_ReadReg
*	����˵��: AD7175���Ĵ���
*	��    ��:
*
*	�� �� ֵ: -1����
*********************************************************************************************************
*/
int32_t AD7175_ReadReg(uint8_t addr,uint8_t * Value,uint8_t size)
{
    int32_t ret = 0;
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t msgBuf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t i        = 0;
    uint8_t crc8     = 0;

    /* Build the Command word */
    Txdata[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD | AD717X_COMM_REG_RA(addr);

  /* Read data from the device */
    AD7175_CS_0();
    if(HAL_SPI_TransmitReceive(&hspi3, Txdata, Rxdata,size+2,100*(size+2))!=0)//1�ֽ�У�飬1�ֽ�����
    ret=-1;
    AD7175_CS_1();

    /* Check the CRC */
    //if(useCRC == AD717X_USE_CRC)
    {
        msgBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD |AD717X_COMM_REG_RA(addr);
        for(i = 1; i < size + 2; ++i)
        {
            msgBuf[i] = Rxdata[i];
        }
        crc8 = AD7175_ComputeCRC8(msgBuf, size + 2);
    }

//	if(useCRC == AD717X_USE_XOR)
//	{
//		msgBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD | AD717X_COMM_REG_RA(addr);
//		for(i = 1; i < size + 2; ++i)
//		{
//			msgBuf[i] = Rxdata[i-1];
//		}
//		check8 = AD7175_ComputeXOR8(msgBuf,size + 2);
//	}
    if(crc8 != 0)
  {
        /* ReadRegister checksum failed. */
        ret=-2;
    }

    /* Build the result */
    for(i = 1; i < size + 1; i++)
    {
        Value[i-1]= Rxdata[i];
    }
    return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: AD7175_CfgADC
*	����˵��: ����ADC����
*	��    ��:
*
*	�� �� ֵ:
*********************************************************************************************************
*/
uint8_t AD7175_ID[3]={0,0,0};

int32_t AD7175_CfgADC(void)
{
    int32_t ret=0;
    HAL_NVIC_DisableIRQ(EXTI0_IRQn);//��ֹ�ж�
    /*  Reset the device interface.*/
    ret = AD7175_Reset();
    if (ret < 0)
    return ret;
    Delay(8000);
    /* Initialize ADC mode register. */
    ret = AD7175_WriteReg(AD717X_ADCMODE_REG,AD717X_ADCMODE_REG_DELAY(2)|AD717X_ADCMODE_SING_CYC,2);//ʹ���ⲿRef,����ת��ģʽ,�ڲ�����
    // ret = AD7175_WriteReg(AD717X_ADCMODE_REG,AD717X_ADCMODE_REG_HIDE_DELAY|AD717X_ADCMODE_SING_CYC,2);//ʹ���ⲿRef,����ת��ģʽ,�ڲ�����
    if(ret < 0)
    return ret;
    /* Initialize Interface mode register. */
    ret = AD7175_WriteReg(AD717X_IFMODE_REG,AD717X_IFMODE_REG_DOUT_RESET | AD717X_IFMODE_REG_DATA_STAT | AD717X_IFMODE_REG_CRC_EN,2);//ʹ��CRCУ��
    // ret = AD7175_WriteReg(AD717X_IFMODE_REG,AD717X_IFMODE_REG_DOUT_RESET | AD717X_IFMODE_REG_DATA_STAT | AD717X_IFMODE_REG_CRC_EN | AD717X_IFMODE_REG_DATA_WL16,2);//ʹ��CRCУ�� 16
    if(ret < 0)
    return ret;

    /* Initialize registers AD717X_GPIOCON_REG through AD717X_OFFSET0_REG */
    ret = AD7175_WriteReg(AD717X_GPIOCON_REG,0x0000,2);//�����źŹر�
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP0_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(0)| AD717X_CHMAP_REG_AINNEG(22),2);//����ͨ��0��0��Ϊ�����룬ѡ��REF-��Ϊ������
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP1_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(1)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP2_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(2)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP3_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(3)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP4_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(4)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP5_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(5)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP6_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(6)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP7_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(7)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP8_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(8)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP9_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(9)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP10_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(10)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP11_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(11)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP12_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(12)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP13_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(13)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP14_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(14)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_CHMAP15_REG,AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINPOS(15)| AD717X_CHMAP_REG_AINNEG(22),2);//
    if(ret < 0)
    return ret;
    //ret = AD7175_WriteReg(AD717X_SETUPCON0_REG,AD717X_SETUP_CONF_REG_REF_SEL(3),2);//����0������������
    ret = AD7175_WriteReg(AD717X_SETUPCON0_REG,AD717X_SETUP_CONF_REG_AIN_BUF(3)| AD717X_SETUP_CONF_REG_REF_SEL(0),2);//����0������������,�ⲿREFѡ��
    if(ret < 0)
    return ret;
    ret = AD7175_WriteReg(AD717X_FILTCON0_REG,AD717X_FILT_CONF_REG_ENHFILT(5) | AD717X_FILT_CONF_REG_ODR(5),2);//ͨ��0������� 250K�����˲�ʱ��
    if(ret < 0)
    return ret;
    // ret = AD7175_WriteReg(AD717X_GAIN0_REG,0x5559F0>>1,3);//�ı����棬Ĭ����0x5559F0
    // if(ret < 0)
    // return ret;

    /* Read ID register to identify the part */
    ret = AD7175_ReadReg(AD717X_ID_REG,AD7175_ID,2);
    if((ret < 0)||(AD7175_ID[0]!=0x3C)||((AD7175_ID[1]&0XD0)!=0xD0))
    ret=-1;
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);//ʹ���ⲿ�ж�
    return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: AD7175_ComputeCRC8
*	����˵��: AD7175 CRCУ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t AD7175_ComputeCRC8(uint8_t * pBuf,uint8_t bufSize)
{
    uint8_t i   = 0;
    uint8_t crc = 0;
    while(bufSize)
    {
        for(i = 0x80; i != 0; i >>= 1)
        {
            if(((crc & 0x80) != 0) != ((*pBuf & i) !=0))
            { /* MSB of CRC register XOR input Bit from Data */
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
/*
*********************************************************************************************************
*	�� �� ��: AD7175_ComputeXOR8
*	����˵��: AD7175���У��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t AD7175_ComputeXOR8(uint8_t * pBuf,uint8_t bufSize)
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
/*
*********************************************************************************************************
*	�� �� ��: AD7175_WaitForReady
*	����˵��: �ȴ�ѭ����ȡ
*	��    ��: ��ʱ��long timeout = 1000;
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int32_t AD7175_WaitForReady(uint32_t timeout)
{
    int32_t ret;
    int8_t ready = 0;
  uint8_t value[1] = {0};
    while(!ready && --timeout)
    {
        /* Read the value of the Status Register */
        ret = AD7175_ReadReg(AD717X_STATUS_REG,value,1);
        if(ret < 0)
        return ret;
        /* Check the RDY bit in the Status Register */
        ready = (value[0] & AD717X_STATUS_REG_RDY) == 0;
    }
    return timeout ? 0 : TIMEOUT;
}
/*
*********************************************************************************************************
*	�� �� ��: AD7175_ReadData
*	����˵��: AD7175��ȡ����
*	��    ��: �ĸ��ֽڵ�ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int32_t AD7175_ReadData(uint8_t* pData)
{
    int32_t ret;

    /* Read the value of the Status Register */
    ret = AD7175_ReadReg(AD717X_DATA_REG, pData, 4);
    return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: AD7175_Process
*	����˵��: AD7175�ɼ��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint32_t ad7175_value[16]={0};

void AD7175_Process(void)
{
    uint8_t value[4] = {0,0,0,0};
    uint32_t read=0;

     // HAL_GPIO_WritePin(LED81_GPIO_Port, LED81_Pin, GPIO_PIN_SET);

    if(AD7175_ReadData(value)==0)
    {
        /* �����������3���ֽڣ����ֽ���ǰ */
        read = (uint32_t)(value[0] << 16);
        read += (uint32_t)(value[1] << 8);
        read += value[2] << 0;
         if((value[3]&0xB0)==0)
            ad7175_value[value[3]&0x0F]=read;
    }
    else
    {
        //Add ������
    }

    // if(AD7175_ReadData(value)==0)
    // {
    // 	/* �����������3���ֽڣ����ֽ���ǰ */
    // 	read = (uint32_t)(value[0] << 8);
    // 	read += (uint32_t)(value[1] << 0);
    // 	//read += value[2] << 0;
    //  	if((value[2]&0xF0)==0)
    // 		ad7175_value[value[2]&0x0F]=read;
    // }
    // else
    // {
    // 	//Add ������
    // }
    AD7175_CS_0();
    // HAL_GPIO_WritePin(LED81_GPIO_Port, LED81_Pin, GPIO_PIN_RESET);
}
/*****************************(END OF FILE) *********************************/
