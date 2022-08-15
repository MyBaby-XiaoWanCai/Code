/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_charge.c
** Author      : Huang Cheng
** Date        : 2022-01-11
** Description : bsp_charge source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"
#include "bsp_dwt.h"
#include "bsp_dac80501.h"
#include "bsp_charge.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
/* S1���ض˿����ű� */
Charge_S1Type Charge_S1PortPinTab[POWER_MODULE_NUM] = 
{
	{STM32_SSwA_P_1_GPIO_Port, STM32_SSwA_P_1_Pin},
	{STM32_SSwA_P_2_GPIO_Port, STM32_SSwA_P_2_Pin},
	{STM32_SSwA_P_3_GPIO_Port, STM32_SSwA_P_3_Pin},
	{STM32_SSwA_P_4_GPIO_Port, STM32_SSwA_P_4_Pin},
	{STM32_SSwA_P_5_GPIO_Port, STM32_SSwA_P_5_Pin},
	{STM32_SSwA_P_6_GPIO_Port, STM32_SSwA_P_6_Pin},
	{STM32_SSwA_P_7_GPIO_Port, STM32_SSwA_P_7_Pin},
	{STM32_SSwA_P_8_GPIO_Port, STM32_SSwA_P_8_Pin},
};

/* S2���ض˿����ű� */
Charge_S2Type Charge_S2PortPinTab[POWER_MODULE_NUM] = 
{
	{STM32_SSwA_N_1_GPIO_Port, STM32_SSwA_N_1_Pin},
	{STM32_SSwA_N_2_GPIO_Port, STM32_SSwA_N_2_Pin},
	{STM32_SSwA_N_3_GPIO_Port, STM32_SSwA_N_3_Pin},
	{STM32_SSwA_N_4_GPIO_Port, STM32_SSwA_N_4_Pin},
	{STM32_SSwA_N_5_GPIO_Port, STM32_SSwA_N_5_Pin},
	{STM32_SSwA_N_6_GPIO_Port, STM32_SSwA_N_6_Pin},
	{STM32_SSwA_N_7_GPIO_Port, STM32_SSwA_N_7_Pin},
	{STM32_SSwA_N_8_GPIO_Port, STM32_SSwA_N_8_Pin},
};

/* P1���ض˿����ű� */
Charge_P1Type Charge_P1PortPinTab[POWER_MODULE_NUM] = 
{
	{STM32_P_1_GPIO_Port, STM32_P_1_Pin},
	{STM32_P_2_GPIO_Port, STM32_P_2_Pin},
	{STM32_P_3_GPIO_Port, STM32_P_3_Pin},
	{STM32_P_4_GPIO_Port, STM32_P_4_Pin},
	{STM32_P_5_GPIO_Port, STM32_P_5_Pin},
	{STM32_P_6_GPIO_Port, STM32_P_6_Pin},
	{STM32_P_7_GPIO_Port, STM32_P_7_Pin},
	{STM32_P_8_GPIO_Port, STM32_P_8_Pin},
};

/* P2���ض˿����ű� */
Charge_P2Type Charge_P2PortPinTab[POWER_MODULE_NUM] = 
{
	{STM32_N_1_GPIO_Port, STM32_N_1_Pin},
	{STM32_N_2_GPIO_Port, STM32_N_2_Pin},
	{STM32_N_3_GPIO_Port, STM32_N_3_Pin},
	{STM32_N_4_GPIO_Port, STM32_N_4_Pin},
	{STM32_N_5_GPIO_Port, STM32_N_5_Pin},
	{STM32_N_6_GPIO_Port, STM32_N_6_Pin},
	{STM32_N_7_GPIO_Port, STM32_N_7_Pin},
	{STM32_N_8_GPIO_Port, STM32_N_8_Pin},
};

/* �����Բ��� */
s_DebugParamStruct debugParam = {0};
s_DebugParamStruct *ChargeDebug = &debugParam;

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/
/**********************************************************************************************************
**	�� �� �� : bsp_ChargeInit()
**	����˵�� : �����Ƴ�ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_ChargeInit(void)
{
	//��ֹSNOE1�ź�
	HAL_GPIO_WritePin(SNOE1_GPIO_Port, SNOE1_Pin, GPIO_PIN_SET);
	
	//���п������Ŷ�����͵�ƽ
	for(uint8_t i = 0; i < POWER_MODULE_NUM; i++)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[i].S1_Port, Charge_S1PortPinTab[i].S1_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[i].S2_Port, Charge_S2PortPinTab[i].S2_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P1PortPinTab[i].P1_Port, Charge_P1PortPinTab[i].P1_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[i].P2_Port, Charge_P2PortPinTab[i].P2_pin, GPIO_PIN_RESET); 
	}
}

/**********************************************************************************************************
**	�� �� �� : Charge_EnAllModule()
**	����˵�� : ���ģʽ��ʹ������ͨ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void Charge_EnAllModule(void)
{
	//�ڱպ���·S1��S2����֮ǰ������ȷ����·P1��P2���ضϿ�
	for(uint8_t i = 0; i < POWER_MODULE_NUM; i++)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P1PortPinTab[i].P1_Port, Charge_P1PortPinTab[i].P1_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[i].P2_Port, Charge_P2PortPinTab[i].P2_pin, GPIO_PIN_RESET);
	}
	
	//��ʱһС��ʱ��
	bsp_DelayMS(1);
	
	//���αպ���·S1��S2����
	for(uint8_t i = 0; i < POWER_MODULE_NUM; i++)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[i].S1_Port, Charge_S1PortPinTab[i].S1_pin, GPIO_PIN_SET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[i].S2_Port, Charge_S2PortPinTab[i].S2_pin, GPIO_PIN_SET); 
	}
	
	//���ʹ��SNOE1�ź�
	HAL_GPIO_WritePin(SNOE1_GPIO_Port, SNOE1_Pin, GPIO_PIN_RESET);
}

/**********************************************************************************************************
**	�� �� �� : Charge_DisableAllModule()
**	����˵�� : ���ģʽ�½�ֹ����ͨ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void Charge_DisableAllModule(void)
{
	//��ֹSNOE1�ź�
	HAL_GPIO_WritePin(SNOE1_GPIO_Port, SNOE1_Pin, GPIO_PIN_SET);
	
	//���п������Ŷ�����͵�ƽ
	for(uint8_t i = 0; i < POWER_MODULE_NUM; i++)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[i].S1_Port, Charge_S1PortPinTab[i].S1_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[i].S2_Port, Charge_S2PortPinTab[i].S2_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P1PortPinTab[i].P1_Port, Charge_P1PortPinTab[i].P1_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[i].P2_Port, Charge_P2PortPinTab[i].P2_pin, GPIO_PIN_RESET); 
	}
}

/**********************************************************************************************************
**	�� �� �� : Charge_EnModule()
**	����˵�� : ���ģʽ��ʹ�ܵ���ͨ��
**	��    �� : Num - ͨ����
**	�� �� ֵ : ��
**********************************************************************************************************/
void Charge_EnModule(uint8_t Num)
{
	/* ��������Ϸ��Լ�� */
	if(Num >= POWER_MODULE_NUM) 
	{
		return;
	}
	
	//�ڱպ���·S1��S2����֮ǰ������ȷ����·P1��P2���ضϿ�
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P1PortPinTab[Num].P1_Port, Charge_P1PortPinTab[Num].P1_pin, GPIO_PIN_RESET); 
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[Num].P2_Port, Charge_P2PortPinTab[Num].P2_pin, GPIO_PIN_RESET);
	
	//��ʱһС��ʱ��
	bsp_DelayMS(1);
	
	//���αպ���·S1��S2����
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[Num].S1_Port, Charge_S1PortPinTab[Num].S1_pin, GPIO_PIN_SET); 
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[Num].S2_Port, Charge_S2PortPinTab[Num].S2_pin, GPIO_PIN_SET); 
	
	//���ʹ��SNOE1�ź�
	HAL_GPIO_WritePin(SNOE1_GPIO_Port, SNOE1_Pin, GPIO_PIN_RESET);
}

/**********************************************************************************************************
**	�� �� �� : Charge_DisableModule()
**	����˵�� : ���ģʽ�½�ֹ����ͨ��
**	��    �� : Num - ͨ����
**	�� �� ֵ : ��
**********************************************************************************************************/
void Charge_DisableModule(uint8_t Num)
{
	/* ��������Ϸ��Լ�� */
	if(Num >= POWER_MODULE_NUM) 
	{
		return;
	}
	
	//�ڱպ���·P1��P2����֮ǰ������ȷ����·S1��S2���ضϿ�
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[Num].S1_Port, Charge_S1PortPinTab[Num].S1_pin, GPIO_PIN_RESET); 
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[Num].S2_Port, Charge_S2PortPinTab[Num].S2_pin, GPIO_PIN_RESET);
	
	//��ʱһС��ʱ��
	bsp_DelayMS(1);
	
	//���αպ���·P1��P2����
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P1PortPinTab[Num].P1_Port, Charge_P1PortPinTab[Num].P1_pin, GPIO_PIN_SET); 
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[Num].P2_Port, Charge_P2PortPinTab[Num].P2_pin, GPIO_PIN_SET);
}

/**********************************************************************************************************
**	�� �� �� : Charge_BatSwitch()
**	����˵�� : ���ģʽ���г��������
**	��    �� : Num - ͨ����
**	�� �� ֵ : ��
**********************************************************************************************************/
void Charge_BatSwitch(uint8_t Num)
{
	/* ��������Ϸ��Լ�� */
	if(Num >= POWER_MODULE_NUM) 
	{
		return;
	}
	
	//���ȱպ���·P1����
	DAC80501_SwitchCtrl(Num, 10, 1000, 0.0, 0.38);
	
	//�Ͽ���·S2����
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[Num].S2_Port, Charge_S2PortPinTab[Num].S2_pin, GPIO_PIN_RESET); 
	
	bsp_DelayMS(1);

	//�Ͽ���·S1����
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[Num].S1_Port, Charge_S1PortPinTab[Num].S1_pin, GPIO_PIN_RESET);
	
	bsp_DelayMS(1);
	
	//�պ���·P2����
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[Num].P2_Port, Charge_P2PortPinTab[Num].P2_pin, GPIO_PIN_SET);

}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

