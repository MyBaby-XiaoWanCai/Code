/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Debug.c
** Author      : Yi Li Hua
** Date        : 2022-08-01
** Description : 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "Task_Debug.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : void SwitchCtr(uint8_t CH)
**  功能说明 : 切换开关控制函数
**  形    参 : CH: 切换通道编号
**			   CurrOK: 功率板返回主控板信号
**  返 回 值 : 
**********************************************************************************************************/
void SwitchCtr(void)
{
	uint8_t i = 0;
	uint8_t Currr_OK;
	Currr_OK =  ~CurrOK[0];
	for (i = 0; i < 8; i++)                      
	{
		if((st_DebugParameter.CHCtrEN & 0x01) == 0x01)
		{
			/* 强制主路开启关闭  */
			if ((st_DebugParameter.MainRoadCtr & 0x01) == 0x01)
			{
				HAL_GPIO_WritePin(SSwA_N_PortPin[2].Port, SSwA_N_PortPin[2].Pin, GPIO_PIN_SET);  //打开主路开关
				HAL_GPIO_WritePin(SSwA_P_PortPin[2].Port, SSwA_P_PortPin[2].Pin, GPIO_PIN_SET);  //打开主路开关
				
			}
			else if((st_DebugParameter.MainRoadCtr & 0x01) == 0x00)
			{
				HAL_GPIO_WritePin(SSwA_N_PortPin[i].Port, SSwA_N_PortPin[i].Pin, GPIO_PIN_RESET);  //关闭主路开关
				HAL_GPIO_WritePin(SSwA_P_PortPin[i].Port, SSwA_P_PortPin[i].Pin, GPIO_PIN_RESET);  //关闭主路开关
			}

			/* 强制旁路开启关闭 */
			if ((st_DebugParameter.ByPassCtr & 0x01) == 0x01) 
			{
				HAL_GPIO_WritePin(CTR_ON_PortPin[i].Port, CTR_ON_PortPin[i].Pin, GPIO_PIN_RESET); //打开旁路开关
				bsp_DelayMS(1);
				HAL_GPIO_WritePin(CTR_ON_PortPin[i].Port, CTR_ON_PortPin[i].Pin, GPIO_PIN_SET);
			}
			else if ((st_DebugParameter.ByPassCtr & 0x01) == 0x00) 
			{
				;
			}
		}
		
		if((Currr_OK & 0x01) == 0x01)	//旁路后接收到关主路信号
		{
			HAL_GPIO_WritePin(SSwA_N_PortPin[i].Port, SSwA_N_PortPin[i].Pin, GPIO_PIN_RESET);   
			HAL_GPIO_WritePin(SSwA_P_PortPin[i].Port, SSwA_P_PortPin[i].Pin, GPIO_PIN_RESET);     
		}
		
		Currr_OK = Currr_OK >> 1;
		st_DebugParameter.CHCtrEN = st_DebugParameter.CHCtrEN >> 1;
	}
	st_DebugParameter.ByPassCtr = 0;
	st_DebugParameter.MainRoadCtr = 0;
}

/**********************************************************************************************************
**  函 数 名 : VoltageCalibration()
**  功能说明 : Sense电压校准函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void VoltageCalibration(void)
{
	uint8_t i;
	float Vcm[8] = {0, 0, 0, 0, 0, 0, 0, 0};  //Vsense共模电压
	for(i = 0; i < 8; i++)
	{
		Tier2.debug.inq1[i].VsenseDebug = (float)(ADC2_DATA_AVG[i] * Tier2.calibrate[i].VsenseSetK_C) / 0x7FFFFF +  Tier2.calibrate[i].VsenseSetB_C;
        Tier2.debug.inq1[i].VtzDebug = (float)(ADC1_DATA_AVG[i] * Tier2.calibrate[i].VmodSetK_C) / 0x7FFFFF + Tier2.calibrate[i].VmodSetB_C;

		/* 共模电压计算 */
		if(i == 0)
		{
			Vcm[0] = 0;
		}
		else
		{    
			Vcm[i] = Vcm[i  - 1] + Tier2.debug.inq1[i - 1].VsenseDebug;
		}
		Tier2.debug.inq1[i].TmodDebug = Tier2.debug.inq1[i].VsenseDebug + (Vcm[i] * Tier2.calibrate[i].IhSetK_C + Tier2.calibrate[i].IhSetB_C);
	}
	if(UpDataFlag == 1)  //校准数据更新 写入FRAM
	{
		MB85Ctrl_Write(0x00, &Tier2, sizeof(Tier2));
	}
}

/**********************************************************************************************************
**  函 数 名 : AppTaskDebug()
**  功能说明 : 调试任务，手动切换开关
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AppTaskDebug(void *argument)
{
	while(1)
	{
		SwitchCtr();
		VoltageCalibration();
		osDelay(DEBUG_TASK_PERIOD);
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
