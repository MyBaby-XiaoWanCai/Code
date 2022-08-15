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
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"
#include "bsp_dwt.h"
#include "bsp_dac80501.h"
#include "bsp_charge.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
/* S1开关端口引脚表 */
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

/* S2开关端口引脚表 */
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

/* P1开关端口引脚表 */
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

/* P2开关端口引脚表 */
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

/* 充电调试参数 */
s_DebugParamStruct debugParam = {0};
s_DebugParamStruct *ChargeDebug = &debugParam;

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/**********************************************************************************************************
**	函 数 名 : bsp_ChargeInit()
**	功能说明 : 充电控制初始化函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_ChargeInit(void)
{
	//禁止SNOE1信号
	HAL_GPIO_WritePin(SNOE1_GPIO_Port, SNOE1_Pin, GPIO_PIN_SET);
	
	//所有控制引脚都输出低电平
	for(uint8_t i = 0; i < POWER_MODULE_NUM; i++)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[i].S1_Port, Charge_S1PortPinTab[i].S1_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[i].S2_Port, Charge_S2PortPinTab[i].S2_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P1PortPinTab[i].P1_Port, Charge_P1PortPinTab[i].P1_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[i].P2_Port, Charge_P2PortPinTab[i].P2_pin, GPIO_PIN_RESET); 
	}
}

/**********************************************************************************************************
**	函 数 名 : Charge_EnAllModule()
**	功能说明 : 充电模式下使能所有通道
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void Charge_EnAllModule(void)
{
	//在闭合主路S1、S2开关之前，必须确认旁路P1、P2开关断开
	for(uint8_t i = 0; i < POWER_MODULE_NUM; i++)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P1PortPinTab[i].P1_Port, Charge_P1PortPinTab[i].P1_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[i].P2_Port, Charge_P2PortPinTab[i].P2_pin, GPIO_PIN_RESET);
	}
	
	//延时一小段时间
	bsp_DelayMS(1);
	
	//依次闭合主路S1、S2开关
	for(uint8_t i = 0; i < POWER_MODULE_NUM; i++)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[i].S1_Port, Charge_S1PortPinTab[i].S1_pin, GPIO_PIN_SET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[i].S2_Port, Charge_S2PortPinTab[i].S2_pin, GPIO_PIN_SET); 
	}
	
	//最后使能SNOE1信号
	HAL_GPIO_WritePin(SNOE1_GPIO_Port, SNOE1_Pin, GPIO_PIN_RESET);
}

/**********************************************************************************************************
**	函 数 名 : Charge_DisableAllModule()
**	功能说明 : 充电模式下禁止所有通道
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void Charge_DisableAllModule(void)
{
	//禁止SNOE1信号
	HAL_GPIO_WritePin(SNOE1_GPIO_Port, SNOE1_Pin, GPIO_PIN_SET);
	
	//所有控制引脚都输出低电平
	for(uint8_t i = 0; i < POWER_MODULE_NUM; i++)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[i].S1_Port, Charge_S1PortPinTab[i].S1_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[i].S2_Port, Charge_S2PortPinTab[i].S2_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P1PortPinTab[i].P1_Port, Charge_P1PortPinTab[i].P1_pin, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[i].P2_Port, Charge_P2PortPinTab[i].P2_pin, GPIO_PIN_RESET); 
	}
}

/**********************************************************************************************************
**	函 数 名 : Charge_EnModule()
**	功能说明 : 充电模式下使能单个通道
**	形    参 : Num - 通道号
**	返 回 值 : 无
**********************************************************************************************************/
void Charge_EnModule(uint8_t Num)
{
	/* 传入参数合法性检查 */
	if(Num >= POWER_MODULE_NUM) 
	{
		return;
	}
	
	//在闭合主路S1、S2开关之前，必须确认旁路P1、P2开关断开
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P1PortPinTab[Num].P1_Port, Charge_P1PortPinTab[Num].P1_pin, GPIO_PIN_RESET); 
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[Num].P2_Port, Charge_P2PortPinTab[Num].P2_pin, GPIO_PIN_RESET);
	
	//延时一小段时间
	bsp_DelayMS(1);
	
	//依次闭合主路S1、S2开关
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[Num].S1_Port, Charge_S1PortPinTab[Num].S1_pin, GPIO_PIN_SET); 
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[Num].S2_Port, Charge_S2PortPinTab[Num].S2_pin, GPIO_PIN_SET); 
	
	//最后使能SNOE1信号
	HAL_GPIO_WritePin(SNOE1_GPIO_Port, SNOE1_Pin, GPIO_PIN_RESET);
}

/**********************************************************************************************************
**	函 数 名 : Charge_DisableModule()
**	功能说明 : 充电模式下禁止单个通道
**	形    参 : Num - 通道号
**	返 回 值 : 无
**********************************************************************************************************/
void Charge_DisableModule(uint8_t Num)
{
	/* 传入参数合法性检查 */
	if(Num >= POWER_MODULE_NUM) 
	{
		return;
	}
	
	//在闭合旁路P1、P2开关之前，必须确认主路S1、S2开关断开
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[Num].S1_Port, Charge_S1PortPinTab[Num].S1_pin, GPIO_PIN_RESET); 
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[Num].S2_Port, Charge_S2PortPinTab[Num].S2_pin, GPIO_PIN_RESET);
	
	//延时一小段时间
	bsp_DelayMS(1);
	
	//依次闭合旁路P1、P2开关
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P1PortPinTab[Num].P1_Port, Charge_P1PortPinTab[Num].P1_pin, GPIO_PIN_SET); 
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[Num].P2_Port, Charge_P2PortPinTab[Num].P2_pin, GPIO_PIN_SET);
}

/**********************************************************************************************************
**	函 数 名 : Charge_BatSwitch()
**	功能说明 : 充电模式下切出单串电池
**	形    参 : Num - 通道号
**	返 回 值 : 无
**********************************************************************************************************/
void Charge_BatSwitch(uint8_t Num)
{
	/* 传入参数合法性检查 */
	if(Num >= POWER_MODULE_NUM) 
	{
		return;
	}
	
	//首先闭合旁路P1开关
	DAC80501_SwitchCtrl(Num, 10, 1000, 0.0, 0.38);
	
	//断开主路S2开关
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S2PortPinTab[Num].S2_Port, Charge_S2PortPinTab[Num].S2_pin, GPIO_PIN_RESET); 
	
	bsp_DelayMS(1);

	//断开主路S1开关
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_S1PortPinTab[Num].S1_Port, Charge_S1PortPinTab[Num].S1_pin, GPIO_PIN_RESET);
	
	bsp_DelayMS(1);
	
	//闭合旁路P2开关
	HAL_GPIO_WritePin((GPIO_TypeDef *)Charge_P2PortPinTab[Num].P2_Port, Charge_P2PortPinTab[Num].P2_pin, GPIO_PIN_SET);

}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

