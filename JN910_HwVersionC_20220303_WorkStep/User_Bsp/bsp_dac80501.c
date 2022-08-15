/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_dac80501.c
** Author      : Huang Cheng
** Date        : 2022-01-06
** Description : bsp_dac80501 source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include <math.h>
#include "spi.h"
#include "main.h"
#include "bsp_dwt.h"
#include "bsp_dac80501.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
DAC80501_CsType DAC80501_CsPortPinTab[DAC80501_CS_NUM] = 
{
	{STM32_DAC_CS1_GPIO_Port, STM32_DAC_CS1_Pin},
	{STM32_DAC_CS2_GPIO_Port, STM32_DAC_CS2_Pin},
	{STM32_DAC_CS3_GPIO_Port, STM32_DAC_CS3_Pin},
	{STM32_DAC_CS4_GPIO_Port, STM32_DAC_CS4_Pin},
	{STM32_DAC_CS5_GPIO_Port, STM32_DAC_CS5_Pin},
	{STM32_DAC_CS6_GPIO_Port, STM32_DAC_CS6_Pin},
	{STM32_DAC_CS7_GPIO_Port, STM32_DAC_CS7_Pin},
	{STM32_DAC_CS8_GPIO_Port, STM32_DAC_CS8_Pin},
};

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
static void DAC80501_CS_SET(uint8_t Dev);
static void DAC80501_CS_CLR(uint8_t Dev);
static void Dac80501_Wait(void);
static void DAC80501_RegCofig(uint8_t Dev);
static void DAC80501_WriteCmd(uint8_t Dev, uint32_t Cmd);
static void DAC80501_ExactTimeStep(uint16_t StepNum, uint8_t InterpNum, uint16_t TimeStep, float TimeDec);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/
/**********************************************************************************************************
**	�� �� �� : bsp_Dac80501Init()
**	����˵�� : DAC80501��ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_Dac80501Init(void)
{
	//spi��ʼ��
	MX_SPI2_Init();
	
	//����DAC80501оƬ��Ƭѡ������
	for(uint8_t i = 0; i < DAC80501_CS_NUM; i++)
	{
		DAC80501_CS_SET(i);
	}
	
	__HAL_SPI_ENABLE(&hspi2);
	
	//��������DAC80501оƬ�ļĴ���
	for(uint8_t i = 0; i < DAC80501_CS_NUM; i++)
	{
		DAC80501_RegCofig(i);
	}
}

/**********************************************************************************************************
**	�� �� �� : DAC80501_CS_SET()
**	����˵�� : DAC80501Ƭѡ��ֹ
**	��    �� : Dev - �豸����
**	�� �� ֵ : ��
**********************************************************************************************************/
static void DAC80501_CS_SET(uint8_t Dev)
{
	/* ��������Ϸ����ж� */
	if(Dev >= DAC80501_CS_NUM)
	{
		return;
	}
	
	HAL_GPIO_WritePin((GPIO_TypeDef *)DAC80501_CsPortPinTab[Dev].CsPort, DAC80501_CsPortPinTab[Dev].Cspin, GPIO_PIN_SET);      
}

/**********************************************************************************************************
**	�� �� �� : DAC80501_CS_CLR()
**	����˵�� : DAC80501Ƭѡѡ��
**	��    �� : Dev - �豸����
**	�� �� ֵ : ��
**********************************************************************************************************/
static void DAC80501_CS_CLR(uint8_t Dev)
{
	/* ��������Ϸ����ж� */
	if(Dev >= DAC80501_CS_NUM)
	{
		return;
	}
	
	HAL_GPIO_WritePin((GPIO_TypeDef *)DAC80501_CsPortPinTab[Dev].CsPort, DAC80501_CsPortPinTab[Dev].Cspin, GPIO_PIN_RESET);      
}

/**********************************************************************************************************
**	�� �� �� : Dac80501_Wait()
**	����˵�� : Ads131оƬ��λ��ȴ�����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Dac80501_Wait(void)
{
	bsp_DelayMS(1);
}

/**********************************************************************************************************
**	�� �� �� : DAC80501_RegCofig()
**	����˵�� : DAC80501�Ĵ������ú���
**	��    �� : Dev - �豸����
**	�� �� ֵ : ��
**********************************************************************************************************/
static void DAC80501_RegCofig(uint8_t Dev)
{
	//�����λDAC80501
	DAC80501_WriteCmd(Dev, (DAC80501_TRIGGER|TRIGGER_SOFT_RESET));
	
	//�����λ�ȴ�ʱ�䣬��ʱ��1ms
	Dac80501_Wait();
	
	//����ѡ���ⲿ�ο���ѹ2.5V
	DAC80501_WriteCmd(Dev, (DAC80501_CONFIG|CONFIG_REF_PWDWN));
	
	//����ѡ���ѹ����ѹ���������Ϊ2��������ֵΪ5V
	DAC80501_WriteCmd(Dev, (DAC80501_GAIN|GAIN_REF_DIV|GAIN_BUFF_GAIN));
	
	//����DAC80501�����ѹΪ0V
	DAC80501_WriteCmd(Dev, (DAC80501_DAC|DAC_DEFDATA_SET));
}

/**********************************************************************************************************
**	�� �� �� : DAC80501_WriteCmd()
**	����˵�� : DAC80501дָ���
**	��    �� : Dev - �豸���ͣ�Cmd - ��������
**	�� �� ֵ : ��
**********************************************************************************************************/
static void DAC80501_WriteCmd(uint8_t Dev, uint32_t Cmd)
{
	uint8_t TxData[3] = {(Cmd>>16)&0xFF, (Cmd>>8)&0xFF, (Cmd&0xFF)};
	HAL_StatusTypeDef SpiRet = HAL_OK;
	
	DAC80501_CS_CLR(Dev);      //Ƭѡ����
	SpiRet = HAL_SPI_Transmit(&hspi2, TxData, sizeof(TxData), 1000);
	DAC80501_CS_SET(Dev);      //Ƭѡ����
	
	if(SpiRet != HAL_OK)
	{
		//������
	}
}

/**********************************************************************************************************
**	�� �� �� : DAC80501_ExactTimeStep()
**	����˵�� : DAC80501������ȷ��ʱ����
**	��    �� : StepNum - ������InterpNum - �岹������TimeStep - ������ʱʱ�䣬TimeDec - ��ʱ����ʱ��С��
**	�� �� ֵ : ��
**  ˵    �� ����������ʱ����ʱ��Ϊ����ʱ����Ҫ�岹;
               ����ʱ����ʱ��ΪС��ʱ�������10��һ�岹�㷨�����ΰ���һ��10����С��ʱ�䲹����;
               Ӧ�ø��㷨��ʱ�����Ϊ0.038467%��ֱ�Ӷ�ʱ��ȡ����ʱ�����Ϊ3.846159%��
               ������Ϊ1000��������ʱ��Ϊ10.40001usʱ��
**********************************************************************************************************/
static void DAC80501_ExactTimeStep(uint16_t StepNum, uint8_t InterpNum, uint16_t TimeStep, float TimeDec)
{
	static uint16_t DelayCnt = 0; //��ʱ����
	static uint8_t  InterpCnt = 0;//�岹��������
	
	DelayCnt++;
	if(DelayCnt > StepNum)
	{
		DelayCnt = 0;
	}
	
	//С�����ֵ���0
	if((TimeDec >= -FLAOT_ZERO) && (TimeDec <= FLAOT_ZERO))
	{
		bsp_DelayUS(TimeStep);
	}
	else
	{		
		//��ʱʱ��С���岹
		if(0x01<=((DelayCnt/10)) && (InterpCnt<InterpNum))
		{
			InterpCnt++;
			bsp_DelayUS(TimeStep + 0x01);
		}
		else
		{
			bsp_DelayUS(TimeStep);
		}
		
		//����岹��������
		if(0x00==((DelayCnt%10)) && (InterpCnt>=InterpNum))
		{
			InterpCnt = 0;
		}
	}
}

/**********************************************************************************************************
**	�� �� �� : DAC80501_SetOutVol()
**	����˵�� : DAC80501���������ѹ����
**	��    �� : Dev - �豸���ͣ�DacVal - DAֵ
**	�� �� ֵ : ��
**********************************************************************************************************/
void DAC80501_SetOutVol(uint8_t Dev, uint16_t DacVal)
{
	DAC80501_WriteCmd(Dev, (DAC80501_DAC|DacVal));
}

/**********************************************************************************************************
**	�� �� �� : DAC80501_SwitchCtrl()
**	����˵�� : DA�л����ƺ���
**	��    �� : Dev - �豸���ͣ�Nms - DA����ʱ�䣬StepNum - ����
               DacVolMin - DA��ѹ��Сֵ��DacVolMax - DA��ѹ���ֵ
**	�� �� ֵ : ��
**********************************************************************************************************/
void DAC80501_SwitchCtrl(uint8_t Dev, uint16_t Nms, uint16_t StepNum, float DacVolMin, float DacVolMax)
{
	uint16_t DacValMin = 0;
	uint16_t DacValMax = 0;
	float DacOutVal = 0;
	float DacValStep = 0.0;
	float DacTimeStep = 0.0;
	
	/* ��ڲ����Ϸ��Եļ�� */
//	if(((DacVolMax-DacVolMin) <= FLAOT_ZERO) || ((DAC_VOL_MAX-DacVolMax) <= FLAOT_ZERO) ||  \
//	   ((DacVolMin <= FLAOT_ZERO) || (DacVolMax <= FLAOT_ZERO)) || (Dev >= DAC80501_CS_NUM))
//	{
//		return;
//	}
	
	//����ѹֵת����DAֵ
	DacValMin = (uint16_t)((DacVolMin/DAC_VOL_MAX) * 65535.0);
	DacValMax = (uint16_t)((DacVolMax/DAC_VOL_MAX) * 65535.0);
	
	//����DA����ֵ��DA����ʱ��us
	DacValStep = (float)((DacValMax-DacValMin)) / StepNum;
	DacTimeStep = (float)((Nms*1000.0)/StepNum) - RESP_TIME;
	DacOutVal = DacValMin;
	
	//����DAC�������
	for(uint16_t i = 0; i< StepNum; i++)
	{
		DacOutVal += DacValStep;
		
		if((uint16_t)DacOutVal >= DacValMax)
		{
			DAC80501_SetOutVol(Dev, DacValMax);
			break;
		}
		else
		{
			DAC80501_SetOutVol(Dev, (uint16_t)DacOutVal);
			bsp_DelayUS((uint16_t)DacTimeStep);           //������ʱ���������ʱҪ�󲻸߿ɲ���
		}
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

