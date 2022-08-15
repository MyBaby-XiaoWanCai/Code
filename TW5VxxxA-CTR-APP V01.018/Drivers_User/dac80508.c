/*
*********************************************************************************************************
*
*	ģ������ : dac80508 ����
*	�ļ����� : dac80508.c
*	��    �� : V1.0
*	˵    �� : DAC80508ģ���CPU֮�����Ӳ��SPI�ӿڡ�
*
*
*
*	�޸ļ�¼ :
*	�汾��  ����         ����     ˵��
*	V1.0    2020-03-02   ZYL      ��ʽ����
*********************************************************************************************************
*/

#include "main.h"

/*
	DAC80508��������:
	1������2.7 - 5.5V;  ������ʹ��5V��
	4���ⲿ�ο���ѹ2.5V�����浽5V

	��SPI��ʱ���ٶ�Ҫ��: �ߴ�50MHz�� �ٶȺܿ�,ʵ��ʹ��45MHz
	SCLK�½��ض�ȡ����, ÿ�δ���24bit���ݣ� ��λ�ȴ�
*/

/* ����GPIO�˿� */
#define DAC80508_PORT_CS1		GPIOE
#define DAC80508_PIN_CS1		GPIO_PIN_11

#define DAC80508_PORT_CS2		GPIOF
#define DAC80508_PIN_CS2		GPIO_PIN_10

/* ���������0����1�ĺ� */
#define DAC80508_CS1_0()		HAL_GPIO_WritePin(DAC80508_PORT_CS1, DAC80508_PIN_CS1, GPIO_PIN_RESET);
#define DAC80508_CS1_1()		HAL_GPIO_WritePin(DAC80508_PORT_CS1, DAC80508_PIN_CS1, GPIO_PIN_SET);
#define DAC80508_CS2_0()		HAL_GPIO_WritePin(DAC80508_PORT_CS2, DAC80508_PIN_CS2, GPIO_PIN_RESET);
#define DAC80508_CS2_1()		HAL_GPIO_WritePin(DAC80508_PORT_CS2, DAC80508_PIN_CS2, GPIO_PIN_SET);

static void DAC80508_ConfigGPIO(void);
static int32_t DAC80508_WriteCmd(uint32_t _cmd, uint8_t index);
/*
*********************************************************************************************************
*	�� �� ��: InitDAC80508
*	����˵��: ����STM32��GPIO��SPI�ӿڣ��������� DAC80508
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int32_t InitDAC80508(void)
{
	int32_t ret = 0;
	DAC80508_ConfigGPIO();

	/* ���üĴ��� ʹ���ⲿ�ο�����ʹ��CRCУ��*/
	ret =DAC80508_WriteCmd((3 << 16) | 0x100,0);
	if(ret!=0)
	return -1;
	/* ����Ĵ��� �ο���ѹ2.5V����2 */
	ret =DAC80508_WriteCmd((4 << 16) | 0xFF,0);
	if(ret!=0)
	return -1;
	/* ���üĴ��� ʹ���ⲿ�ο�����ʹ��CRCУ��*/
	ret =DAC80508_WriteCmd((3 << 16) | 0x100,1);
	if(ret!=0)
	return -1;
	/* ����Ĵ��� �ο���ѹ2.5V����2 */
	ret =DAC80508_WriteCmd((4 << 16) | 0xFF,1);
	if(ret!=0)
	return -1;
	/* ��λ2��DAC ���0V */
  ret =DAC80508_BRDCAST(0x0000,0);
	if(ret!=0)
	return -1;
	ret =DAC80508_BRDCAST(0x0000,1);
	if(ret!=0)
	return -1;
}

/*
*********************************************************************************************************
*	�� �� ��: DAC80508_ConfigGPIO
*	����˵��: ����GPIO
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DAC80508_ConfigGPIO(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	/* ��GPIOʱ�� */
	__HAL_RCC_GPIOE_CLK_ENABLE(); /* ��GPIOEʱ�� */
	__HAL_RCC_GPIOF_CLK_ENABLE(); /* ��GPIOFʱ�� */


	/* �����������IO */
	GPIO_Initure.Pin = DAC80508_PIN_CS1; /*������� */
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* ��Ϊ����ģʽ */
	GPIO_Initure.Pull = GPIO_NOPULL;         /* ���������費ʹ�� */
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO������ٶ� */
	HAL_GPIO_Init(GPIOE, &GPIO_Initure);     //GPIOE

		/* �����������IO */
	GPIO_Initure.Pin = DAC80508_PIN_CS2; /*������� */
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* ��Ϊ����ģʽ */
	GPIO_Initure.Pull = GPIO_NOPULL;         /* ���������費ʹ�� */
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO������ٶ� */
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);     //GPIOF

	DAC80508_CS1_1();
	DAC80508_CS2_1();
}

/*
*********************************************************************************************************
*	�� �� ��: DAC80508_WriteCmd
*	����˵��: ��SPI���߷���24��bit���ݡ�
*	��    ��: _cmd : ���� index:�豸���
*	�� �� ֵ: 0����
*********************************************************************************************************
*/

int32_t DAC80508_WriteCmd(uint32_t _cmd, uint8_t index)
{
	uint8_t Txdata[3]={_cmd>>16,(_cmd>>8)&0x00FF,_cmd&0x0000FF};
	uint8_t Rxdata[3]={0,0,0};
	int32_t ret = 0;
  if(index==0)
	{
    DAC80508_CS1_0();
    ret=HAL_SPI_TransmitReceive(&hspi4, Txdata, Rxdata,3,1000);
	  DAC80508_CS1_1();
	}
	else
	{
    DAC80508_CS2_0();
    ret=HAL_SPI_TransmitReceive(&hspi5, Txdata, Rxdata,3,1000);
	  DAC80508_CS2_1();
	}
	return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: DAC8562_SetDacData
*	����˵��: ����DAC���,��ͨ���������¡�
*	��    ��: _ch, ͨ��, 0 , 1
*		     _data : ���� index���豸����
*	�� �� ֵ: 0����
*********************************************************************************************************
*/
int32_t DAC80508_SetDacData(uint8_t _ch, uint16_t _dac, uint8_t index)
{
	int32_t ret = 0;
	ret=DAC80508_WriteCmd(((0x8+_ch) << 16) | _dac, index);
	HAL_Delay(10);
	return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: DAC80508_BRDCAST
*	����˵��: ����DAC���,�㲥���ͣ�ֵ��Ҫһ��
*	��    ��: _dac��ֵ index���豸����
*
*	�� �� ֵ: 0����
*********************************************************************************************************
*/
int32_t DAC80508_BRDCAST(uint16_t _dac, uint8_t index)
{
	int32_t ret = 0;
	ret=DAC80508_WriteCmd((0x6 << 16) | _dac, index);
	return ret;
}
/*****************************(END OF FILE) *********************************/
