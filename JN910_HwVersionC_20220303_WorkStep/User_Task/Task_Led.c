/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Led.c
** Author      : Huang Cheng
** Date        : 2022-03-08
** Description : 灯板任务模块
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t g_ucLedChSta[5] = {1, 0, 0, 0, 0};
uint8_t g_ucLedChR[8] = {CH1_LED_R, CH2_LED_R, CH3_LED_R, CH4_LED_R, CH5_LED_R, CH6_LED_R, CH7_LED_R, CH8_LED_R};
uint8_t g_ucLedChG[8] = {CH1_LED_G, CH2_LED_G, CH3_LED_G, CH4_LED_G, CH5_LED_G, CH6_LED_G, CH7_LED_G, CH8_LED_G};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void AppTaskLed(void *argument);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/


void LedStaInit(uint8_t sta)
{
	switch(sta)
	{
		case 0:		// 0. 全红
		{
			g_ucLedChSta[0] = RUN_R_ON;
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				g_ucLedChSta[j + 1] = CH_ERR_HARDWARE;
			}
		}
		break;
		
		case 1:		// 1. 全灭
		{
			g_ucLedChSta[0] = RUN_OFF;
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				g_ucLedChSta[j + 1] = CH_STANDBY;
			}		
		}
		break;
		
		case 2:		// 2. 全绿
		{
			g_ucLedChSta[0] = RUN_G_ON;
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				g_ucLedChSta[j + 1] = CH_COMPLETE;
			}
		}
		break;
		
		case 3:		// 3. 全灭
		{
			g_ucLedChSta[0] = RUN_OFF;
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				g_ucLedChSta[j + 1] = CH_STANDBY;
			}		
		}
		break;
		
		default:	// RUN绿闪
		{
			g_ucLedChSta[0] = RUN_G_SHINE;		
		}
		break;
	}
		
	osDelay(5000);
}

void LedStaSwFunc(void)
{
	if((g_ucSysMode == SYS_NORMAL_MODE) && (SysModeProtMaskFlag == 0))
	{
		for(int j = 0; j < CHANNEL_CFG; j++)
		{
			if(Protect_Flag_Global[j] != 0)						// 全局保护
			{
				SET_1(Tier2.record[j].ErrType, 0);
			}
			else
			{
				SET_0(Tier2.record[j].ErrType, 0);
			}
			if(Protect_Flag_Workstep[j] != 0)					// 单工步保护
			{
				SET_1(Tier2.record[j].ErrType, 1);
			}
			else
			{
				SET_0(Tier2.record[j].ErrType, 1);
			}
			if(Protect_Flag_Internal[j] != 0)					// 内部保护
			{
				SET_1(Tier2.record[j].ErrType, 2);
			}
			else
			{
				SET_0(Tier2.record[j].ErrType, 2);
			}
			
			if(Tier2.record[j].ErrType != 0)		// 有故障
			{
				if(ContactCheckFlag[j] == 0)		// 无接触测试
				{
					contact_test_once_flag[j] = 0;	// 不需要强行进入接触检查
				}
			}
			if(Tier2.record[j].ErrType != 0)		// 有故障
			{
				if(Tier2.record[j].ErrType & 0x01)			// 全局故障
				{
					g_ucLedChSta[j + 1] = CH_ERR_GLOBAL;
				}
				else if(Tier2.record[j].ErrType & 0x02)		// 单工步故障
				{
					g_ucLedChSta[j + 1] = CH_ERR_WORKSTEP;
				}
				else if(Tier2.record[j].ErrType & 0x04)		// 内部故障
				{
					if((IP_Alarm_Flag[j] & IntelProtect_HPA) != 0)	// 内部硬件故障
					{
						g_ucLedChSta[j + 1] = CH_ERR_HARDWARE;
					}
					else											// 内部其他故障
					{
						g_ucLedChSta[j + 1] = CH_ERR_OTHER;
					}
				}
				else if(Tier2.record[j].ErrType & 0x08)		// ACDC故障
				{
					g_ucLedChSta[j + 1] = CH_ERR_ACDC;
				}
			}
			else if((Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT)   || \
					((Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)  && \
					 ((Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CC)     || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CV) || \
					  (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CCTOCV) || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CP))))		// 充电
			{
				g_ucLedChSta[j + 1] = CH_CHARGE;
			}
			else if((Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
					((Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCC)      || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCV) || \
					 (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV) || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCP)))	// 放电
			{
				g_ucLedChSta[j + 1] = CH_DISCHARGE;
			}
			else if(Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)		// 完成
			{
				g_ucLedChSta[j + 1] = CH_COMPLETE;
			}
			else if(Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_PAUSE)		// 暂停
			{
				g_ucLedChSta[j + 1] = CH_PAUSE;
			}
			else																// 待机
			{
				g_ucLedChSta[j + 1] = CH_STANDBY;
			}
		}
	}
	else
	{
		g_ucLedChSta[0] = RUN_G_SHINE;
	}
}

void LedStaExecute(void)
{
	//RUN灯
	switch(g_ucLedChSta[0])
	{			
		case RUN_OFF:		/* 关闭指示灯 */
		{
			Led_Shine_Ctrl(RUN_LED_R, 0, 1000, LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G, 0, 1000, LED_TASK_PERIOD);			
		}
		break;
		
		case RUN_G_ON:		/* 绿灯 */
		{
			Led_Shine_Ctrl(RUN_LED_R, 0   , 1000, LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G, 1000, 0   , LED_TASK_PERIOD);			
		}
		break;
		
		case RUN_R_ON:		/* 红灯 */
		{
			Led_Shine_Ctrl(RUN_LED_R, 1000, 0   , LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G, 0   , 1000, LED_TASK_PERIOD);			
		}
		break;

		case RUN_G_SHINE:	/* 绿灯闪烁 */
		{
			Led_Shine_Ctrl(RUN_LED_R, 0  , 1000, LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G, 500, 500 , LED_TASK_PERIOD);
		}
		break;		
		
		case RUN_R_SHINE:	/* 红灯闪烁 */
		{
			Led_Shine_Ctrl(RUN_LED_R, 500, 500 , LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G, 0  , 1000, LED_TASK_PERIOD);
		}
		break;
		
		default:
		{
		
		}
		break;			
	}

	//CH灯
	for(int i = 0; i < CHANNEL_CFG; i++)
	{
		switch(g_ucLedChSta[i])
		{
			case CH_STANDBY:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0, 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0, 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_CHARGE:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0  , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 500, 500 , LED_TASK_PERIOD);
			}
			break;

			case CH_DISCHARGE:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0  , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 200, 200 , LED_TASK_PERIOD);
			}
			break;;

			case CH_COMPLETE:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0   , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 1000, 0   , LED_TASK_PERIOD);
			}
			break;;

			case CH_PAUSE:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0  , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 800, 200 , LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_GLOBAL:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 200, 200 , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0  , 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_WORKSTEP:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 500, 500 , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0  , 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_HARDWARE:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 1000, 0   , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0   , 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_OTHER:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 800, 200 , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0  , 1000, LED_TASK_PERIOD);
			}
			break;
			
			default:
			{

			}
			break;
		}
	}
}

/**********************************************************************************************************
**  函 数 名 : AppTaskLed()
**  功能说明 : 启动任务
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AppTaskLed(void *argument)
{
    while(1)
    {	 
		
        osDelay(500);		
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


