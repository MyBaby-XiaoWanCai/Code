/*
*********************************************************************************************************
*
*	ģ������ : AD7124_8����ģ��(8ͨ����PGA��24λADC)
*	�ļ����� : ad7124_8.c
*	��    �� : V1.0
*	˵    �� : AD7124ģ���CPU֮�����SPI�ӿڡ�����������ʹ��Ӳ��SPI��
*			  
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0    2020-03-27   ZYL      ��ʽ����
*********************************************************************************************************
*/

#include "main.h"
#include <stdbool.h>
/*
	AD7124��������:
	1��ģ�ⲿ�ֹ���5V;
	2��SPI���ֽӿڵ�ƽ��3.3V
	3���ο���ѹ2.5V (���ã�
	4�������ѹ��Χ��
	5��SPIʱ�����5M��ʵ��ʹ��2.8125M
*/

/* Error codes */

#define INVALID_VAL -1 /* Invalid argument */

#define COMM_ERR    -2 /* Communication error on receive */

#define TIMEOUT     -3 /* A timeout has occured */

#define AD7124_POST_RESET_DELAY      4


/* ����GPIO�˿� */
	#define AD7124_PORT_CS		             GPIOI
	#define AD7124_PIN_CS		               GPIO_PIN_0
	
	/* ���������0����1�ĺ� */
	#define AD7124_CS_0()		HAL_GPIO_WritePin(AD7124_PORT_CS, AD7124_PIN_CS, GPIO_PIN_RESET);
	#define AD7124_CS_1()		HAL_GPIO_WritePin(AD7124_PORT_CS, AD7124_PIN_CS, GPIO_PIN_SET);

uint8_t AD7124_Start_Read=0;


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
*	�� �� ��: InitAD7124
*	����˵��: ��ʼ��AD7124
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int32_t InitAD7124(void)
{
	GPIO_InitTypeDef GPIO_Initure;
  	int32_t ret = 0;

	__HAL_RCC_GPIOI_CLK_ENABLE(); /* ��GPIODʱ�� */

	GPIO_Initure.Pin =  AD7124_PIN_CS; /*������� */
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* ��Ϊ����ģʽ */
	GPIO_Initure.Pull = GPIO_NOPULL;         /* ���������費ʹ�� */
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO������ٶ� */
	HAL_GPIO_Init(AD7124_PORT_CS, &GPIO_Initure);     //GPIOI
	AD7124_CS_1();
  	ret=AD7124_CfgADC();
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);//ʹ���ⲿ�ж�
	if(AD7124_Start_Read==0)
  	{
	  	AD7124_Start_Read=1;
    	AD7124_CS_0();
  	}
	return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: ad7124_wait_to_power_on
*	����˵��: ad7124�ȴ���Դ��
*	��    ��: uint32_t timeout
*			   
*	�� �� ֵ: -1����
*********************************************************************************************************
*/
int32_t ad7124_wait_to_power_on(uint32_t timeout)
{
	int32_t ret;
	int8_t powered_on = 0;
  uint8_t value[1] = {0};
	
	while(!powered_on && timeout--) 
	{
		ret = AD7124_ReadReg(AD7124_STATUS_REG,value,1);
		if(ret < 0)
		return ret;
		/* Check the POR_FLAG bit in the Status Register */
		powered_on = (value[0] &AD7124_STATUS_REG_POR_FLAG) == 0;
	}
	return (timeout || powered_on) ? 0 : -1;
}
/*
*********************************************************************************************************
*	�� �� ��: AD7124_Reset
*	����˵��: AD7124��λ
*	��    ��: 
*			   
*	�� �� ֵ: -1����
*********************************************************************************************************
*/
int32_t AD7124_Reset(void)
{
	int32_t ret = 0;
	uint8_t Txdata[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
  AD7124_CS_0();
  ret=HAL_SPI_TransmitReceive(&hspi2, Txdata, Rxdata,8,100*8);
	AD7124_CS_1();
	if(ret!=0)
	return -1;		
	ret=ad7124_wait_to_power_on(800);
	if(ret!=0)
	return -1;	
	/* CRC is disabled after reset */
	//use_crc = AD7124_DISABLE_CRC;
	Delay(8000);
	return ret;

}
/*
*********************************************************************************************************
*	�� �� ��: AD7124_WriteReg
*	����˵��: AD7124д�Ĵ���
*	��    ��: 
*			   
*	�� �� ֵ: -1����
*********************************************************************************************************
*/
int32_t AD7124_WriteReg(uint8_t addr,int32_t Value,uint8_t size)
{
	int32_t ret = 0;
	uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t i        = 0;
	uint8_t crc8     = 0;
	/* Build the Command word */
	Txdata[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_WR | AD7124_COMM_REG_RA(addr);

	/* Fill the write buffer */
	for(i = 0; i < size; i++) 
	{
		Txdata[size - i] = Value & 0xFF;
		Value >>= 8;
	}

	/* Compute the CRC */
	//if(useCRC != AD717X_DISABLE) 
	{
		crc8 = ad7124_compute_crc8(Txdata, size + 1);
		Txdata[size + 1] = crc8;
	}

	/* Write data to the device */
  AD7124_CS_0();
  ret=HAL_SPI_TransmitReceive(&hspi2, Txdata, Rxdata,size+2,100*(size+2));//1�ֽڵ�ַ��1�ֽ�У��
	AD7124_CS_1();
	
	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: AD7124_ReadReg
*	����˵��: AD7124���Ĵ���
*	��    ��: 
*			   
*	�� �� ֵ: -1����
*********************************************************************************************************
*/
uint8_t check_ready = 0;
int32_t AD7124_ReadReg(uint8_t addr,uint8_t * Value,uint8_t size)
{
	int32_t ret = 0;
	uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t msgBuf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t i        = 0;
	uint8_t crc8     = 0;
	
	/* Build the Command word */
	Txdata[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD | AD7124_COMM_REG_RA(addr);
	
  /* Read data from the device */
  AD7124_CS_0();
  ret=HAL_SPI_TransmitReceive(&hspi2, Txdata, Rxdata,size+2,100*(size+2));//1�ֽ�У�飬1�ֽ�����
	AD7124_CS_1();

	/* Check the CRC */
	//if(useCRC == AD717X_USE_CRC) 
	{
		msgBuf[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD | AD7124_COMM_REG_RA(addr);		
		for(i = 1; i < size + 2; ++i) 
		{
			msgBuf[i] = Rxdata[i];//
		}
		crc8 = ad7124_compute_crc8(msgBuf, size + 2);
	}
	if(check_ready==1)
	{	
		if(crc8 != 0) 
		{
			/* ReadRegister checksum failed. */
			return COMM_ERR;
		}
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

int32_t AD7124_CfgADC(void)
{	
  int32_t ret=0;
	HAL_NVIC_DisableIRQ(EXTI1_IRQn);//��ֹ�ж�
	/*  Reset the device interface.*/
	ret = AD7124_Reset();
	if (ret < 0)
	return ret;
	
	/* Update the device structure with power-on/reset settings */
	/* Initialize ADC register. */
	ret = AD7124_WriteReg(AD7124_ADC_CTRL_REG,AD7124_ADC_CTRL_REG_DATA_STATUS | AD7124_ADC_CTRL_REG_POWER_MODE(2)| AD7124_ADC_CTRL_REG_MODE(0),2);//��ȡ��״̬��ʶ��������ģʽ������ת��ģʽ��
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_IO_CTRL1_REG,0x000000,3);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_IO_CTRL2_REG,0x0000,2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_ERREN_REG,AD7124_ERREN_REG_SPI_CRC_ERR_EN,3);//SPI_CRC_ERR_EN==1��SPI_IGNORE_ERR=0
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH0_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(0)|AD7124_CH_MAP_REG_AINM(17),2);//����ͨ��0,�ο�AVSS
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH1_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(1)|AD7124_CH_MAP_REG_AINM(17),2);
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH2_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(2)|AD7124_CH_MAP_REG_AINM(17),2);
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH3_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(3)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH4_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(4)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH5_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(5)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH6_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(6)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH7_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(7)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH8_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(8)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH9_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(9)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH10_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(10)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH11_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(11)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH12_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(12)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH13_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(13)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH14_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(14)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH15_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(15)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CFG0_REG,AD7124_CFG_REG_AIN_BUFP|AD7124_CFG_REG_AINN_BUFM|AD7124_CFG_REG_REF_SEL(0)|AD7124_CFG_REG_PGA(0),2);//�����ԡ�+-2.5V���ⲿ�ο�1
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_FILT0_REG,AD7124_FILT_REG_FILTER(0)|AD7124_FILT_REG_REJ60|AD7124_FILT_REG_POST_FILTER(3)|AD7124_FILT_REG_FS(1),3);//�˲�����Sinc4��Output Data Rate 19200(SPS)
	if(ret < 0)
	return ret;
	check_ready = 1;
	return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: AD7124_ComputeCRC8
*	����˵��: AD7124 CRCУ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t ad7124_compute_crc8(uint8_t * p_buf, uint8_t buf_size)
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
			{ /* MSB of CRC register XOR input Bit from Data */
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
/*
*********************************************************************************************************
*	�� �� ��: AD7124_WaitFor_SPI_Ready
*	����˵��: �ȴ�SPI׼����
*	��    ��: ��ʱ��long timeout = 1000;
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int32_t AD7124_WaitFor_SPI_Ready(uint32_t timeout)
{
	int32_t ret;
	int8_t ready = 0;
  uint8_t value[3] = {0,0,0};
	while(!ready && --timeout) 
	{
		/* Read the value of the Status Register */
		ret = AD7124_ReadReg(AD7124_ERR_REG,value,3);
		if(ret < 0)
		return ret;
		/* Check the RDY bit in the Status Register */
		ready = (value[2] & AD7124_ERR_REG_SPI_IGNORE_ERR) == 0;
	}
	return timeout ? 0 : TIMEOUT;
}
/*
*********************************************************************************************************
*	�� �� ��: AD7124_WaitFor_Conv_Ready
*	����˵��: �ȴ�ת��׼����
*	��    ��: ͨ���ţ���ʱ��long timeout = 1000;
*	�� �� ֵ: <1:����
*********************************************************************************************************
*/
int32_t AD7124_WaitFor_Conv_Ready(uint8_t* ch,uint32_t timeout)
{
	int32_t ret;
	int8_t ready = 0;
  uint8_t value[1] = {0};
	while(!ready && --timeout) 
	{
		/* Read the value of the Status Register */
		ret = AD7124_ReadReg(AD7124_STATUS_REG,value,1);
		if(ret < 0)
		return ret;
		/* Check the RDY bit in the Status Register */
		ready = (value[0] & AD7124_STATUS_REG_RDY) == 0;
		ch[0]= value[0] & 0x0f;
	}
	return timeout ? 0 : TIMEOUT;
}
/*
*********************************************************************************************************
*	�� �� ��: AD7124_ReadData
*	����˵��: AD7124��ȡ����
*	��    ��: �ĸ��ֽڵ�ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int32_t AD7124_ReadData(uint8_t* pData)
{
	int32_t ret;
	/* Read the value of the Status Register */
	ret = AD7124_ReadReg(AD7124_DATA_REG, pData, 4);
	return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: AD7124_Process
*	����˵��: AD7124��ȡ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint32_t ad7124_value[16]={0};
void AD7124_Process(void)
{
 uint8_t value[4] = {0,0,0,0};
 uint32_t read=0;
 if(AD7124_ReadData(value)==0)
 {
		/* �����������3���ֽڣ����ֽ���ǰ */
		read = (uint32_t)(value[0] << 16);
		read += (uint32_t)(value[1] << 8);
		read += value[2] << 0; 
	 
////	 	/* ����������չ��24λ�з�������չΪ32λ�з����� */
//		if (read & 0x800000)//Ϊ��ֵ
//		{
//			read = read-0x800000;
//		}	
//		else //Ϊ��ֵ
//		{
//		  read = (read+0x800000)|0xFF000000;
//		}
		if((value[3]&0xF0)==0)
		{
		  ad7124_value[value[3]&0x0F]=read;
		}
 }
 AD7124_CS_0();
}
/*****************************(END OF FILE) *********************************/
