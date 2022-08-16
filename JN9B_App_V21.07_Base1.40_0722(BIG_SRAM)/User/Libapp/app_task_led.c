/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     : JN95
** --------------------------------------------------------------------------------------------------------
** File name   : AppTaskLed.c
** Author      : Robert.Li
** Date        : 2021-06-30
** Description : Led application task file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
#ifdef YCJ
uint8_t g_ucLedChSta[9] = {1, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t g_ucLedChR[9] = {RUN_LED_R_NO, CH1_LED_R_NO, CH2_LED_R_NO, CH3_LED_R_NO, CH4_LED_R_NO, CH5_LED_R_NO, CH6_LED_R_NO, CH7_LED_R_NO, CH8_LED_R_NO};
uint8_t g_ucLedChG[9] = {RUN_LED_G_NO, CH1_LED_G_NO, CH2_LED_G_NO, CH3_LED_G_NO, CH4_LED_G_NO, CH5_LED_G_NO, CH6_LED_G_NO, CH7_LED_G_NO, CH8_LED_G_NO};
#else
uint8_t g_ucLedChSta[5] = {1, 0, 0, 0, 0};      //Led状态
uint8_t g_ucLedChR[5] = {RUN_LED_R_NO, CH1_LED_R_NO, CH2_LED_R_NO, CH3_LED_R_NO, CH4_LED_R_NO};  //红灯编号
uint8_t g_ucLedChG[5] = {RUN_LED_G_NO, CH1_LED_G_NO, CH2_LED_G_NO, CH3_LED_G_NO, CH4_LED_G_NO};  //绿灯编号
#endif

extern uint8_t TW_HisAlam;          //??????
extern uint8_t TW_HisStatus;        //??????
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void LedStaInit(uint8_t sta);
static void LedStaSwFunc(void);
static void LedStaExecute(void);
void AppTaskLed(void *argument);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : LedStaInit()
**	功能说明 : LED指示灯状态初始化函数
**	形    参 : sta - 状态参数
**	返 回 值 : 无
**********************************************************************************************************/
static void LedStaInit(uint8_t sta)
{
	switch(sta)
	{
		case 0:		// 0. 全红
		{
			g_ucLedChSta[0] = RUN_R_ON;
			for(int j = 0; j < CHANNEL_USE; j++)
			{
				g_ucLedChSta[j + 1] = CH_ERR_HARDWARE;
			}
			break;
		}
		case 1:		// 1. 全灭
		{
			g_ucLedChSta[0] = RUN_OFF;
			for(int j = 0; j < CHANNEL_USE; j++)
			{
				g_ucLedChSta[j + 1] = CH_STANDBY;
			}	
			break;
		}
		case 2:		// 2. 全绿
		{
			g_ucLedChSta[0] = RUN_G_ON;
			for(int j = 0; j < CHANNEL_USE; j++)
			{
				g_ucLedChSta[j + 1] = CH_COMPLETE;
			}
			break;
		}
		case 3:		// 3. 全灭
		{
			g_ucLedChSta[0] = RUN_OFF;
			for(int j = 0; j < CHANNEL_USE; j++)
			{
				g_ucLedChSta[j + 1] = CH_STANDBY;
			}	
			break;
		}
		default:	// RUN绿闪
		{
			g_ucLedChSta[0] = RUN_G_SHINE;
			break;			
		}
	}
		
	for(int m = 0; m < (5000 / LED_TASK_PERIOD); m++)
	{
		LedStaExecute();
		osDelay(LED_TASK_PERIOD);
	}
}

/**********************************************************************************************************
**	函 数 名 : LedStaSwFunc()
**	功能说明 : LED指示灯状态切换函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void LedStaSwFunc(void)
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
			
			if((ACDC.Type == 1) || (ACDC.Type == 11))			// 国电
			{
				if((ACDC.State[0] & 0x0FF8) || (ACDC.State[1] & 0x0FF8))	// ACDC保护
				{
					SET_1(Tier2.record[j].ErrType, 3);
				}
				else
				{
					SET_0(Tier2.record[j].ErrType, 3);
				}
			}
			else if((ACDC.Type == 2) || (ACDC.Type == 22))		// 普德
			{
				if((ACDC.State[0] & 0xBFF0) || (ACDC.State[1] & 0xBFF0))	// ACDC保护
				{
					SET_1(Tier2.record[j].ErrType, 3);
				}
				else
				{
					SET_0(Tier2.record[j].ErrType, 3);
				}	
			}
			else if(ACDC.Type == 3)		                       // ??
			{
					if((TW_HisAlam & 0xFF) || ((TW_HisStatus & 0x01)==0x0))	   // ACDC??
					{
							SET_1(Tier2.record[j].ErrType, 3);
					}
					else
					{
							SET_0(Tier2.record[j].ErrType, 3);
					}
			}
			
			if(Tier2.record[j].ErrType != 0)					// 有故障
			{
				if(ContactCheckFlag[j] == 0)					// 无接触测试
				{
					contact_test_once_flag[j] = 0;				// 不需要强行进入接触检查
				}
			}
			
			uint8_t led_tmp = 0;
			
			if(CHANNEL_USE == 8)
			{
				led_tmp = j;
			}
			else
			{
				led_tmp = 2 * j;
			}
			
			// LED判断
			if(Tier2.record[j].ErrType != 0)		// 有故障
			{	
				if(Tier2.record[j].ErrType & 0x01)			// 全局故障
				{
					g_ucLedChSta[led_tmp + 1] = CH_ERR_GLOBAL;
				}
				else if(Tier2.record[j].ErrType & 0x02)		// 单工步故障
				{
					g_ucLedChSta[led_tmp + 1] = CH_ERR_WORKSTEP;
				}
				else if(Tier2.record[j].ErrType & 0x04)		// 内部故障
				{
					if((IP_Alarm_Flag[j] & IntelProtect_HPA) != 0)	// 内部硬件故障
					{
						g_ucLedChSta[led_tmp + 1] = CH_ERR_HARDWARE;
					}
					else											// 内部其他故障
					{
						g_ucLedChSta[led_tmp + 1] = CH_ERR_OTHER;
					}
				}
				else if(Tier2.record[j].ErrType & 0x08)		// ACDC故障
				{
					g_ucLedChSta[led_tmp + 1] = CH_ERR_ACDC;
				}
			}
			else if((Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT)   || \
					((Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)  && \
					 ((Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CC)     || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CV) || \
					  (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CCTOCV) || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CP))))		// 充电
			{
				g_ucLedChSta[led_tmp + 1] = CH_CHARGE;
			}
			else if((Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
					((Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCC)      || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCV) || \
					 (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV) || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCP)))	// 放电
			{
				g_ucLedChSta[led_tmp + 1] = CH_DISCHARGE;
			}
			else if(Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)		// 完成
			{
				g_ucLedChSta[led_tmp + 1] = CH_COMPLETE;
			}
			else if(Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_PAUSE)		// 暂停
			{
				g_ucLedChSta[led_tmp + 1] = CH_PAUSE;
			}
			else	// 待机
			{
				g_ucLedChSta[led_tmp + 1] = CH_STANDBY;
			}
		}
	}
	else
	{
		g_ucLedChSta[0] = RUN_G_SHINE;
	}
}

/**********************************************************************************************************
**	函 数 名 : LedStaExecute()
**	功能说明 : LED指示灯动作执行函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void LedStaExecute(void)
{
	//RUN灯
	switch(g_ucLedChSta[0])
	{			
		case RUN_OFF:		/* 关闭指示灯 */
		{
			Led_Shine_Ctrl(RUN_LED_R_NO, 0, 1000, LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G_NO, 0, 1000, LED_TASK_PERIOD);			
		}
		break;
		
		case RUN_G_ON:		/* 绿灯 */
		{
			Led_Shine_Ctrl(g_ucLedChR[0], 0   , 1000, LED_TASK_PERIOD);
			Led_Shine_Ctrl(g_ucLedChG[0], 1000, 0   , LED_TASK_PERIOD);			
		}
		break;
		
		case RUN_R_ON:		/* 红灯 */
		{
			Led_Shine_Ctrl(g_ucLedChR[0], 1000, 0   , LED_TASK_PERIOD);
			Led_Shine_Ctrl(g_ucLedChG[0], 0   , 1000, LED_TASK_PERIOD);			
		}
		break;

		case RUN_G_SHINE:	/* 绿灯闪烁 */
		{
			Led_Shine_Ctrl(RUN_LED_R_NO, 0  , 1000, LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G_NO, 500, 500 , LED_TASK_PERIOD);
		}
		break;		
		
		case RUN_R_SHINE:	/* 红灯闪烁 */
		{
			Led_Shine_Ctrl(RUN_LED_R_NO, 500, 500 , LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G_NO, 0  , 1000, LED_TASK_PERIOD);
		}
		break;
		
		default:
		{
		
		}
		break;			
	}

	//CH灯
	for(int i = 1; i < CHANNEL_USE+1; i++)
	{
		switch(g_ucLedChSta[i])
		{
			case CH_STANDBY:	//待机 - CH灯熄灭
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0, 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0, 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_CHARGE:		//充电 - 绿灯慢闪(1s一次)
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0  , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 500, 500 , LED_TASK_PERIOD);
			}
			break;

			case CH_DISCHARGE:	//放电 - 绿灯快闪(0.4s一次)
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0  , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 200, 200 , LED_TASK_PERIOD);
			}
			break;

			case CH_COMPLETE:	//完成 - 绿灯常亮
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0   , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 1000, 0   , LED_TASK_PERIOD);
			}
			break;

			case CH_PAUSE:	//暂停 - 绿灯亮0.8s,灭0.2s
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0  , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 800, 200 , LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_GLOBAL:	//全局故障 - 红灯快闪(0.4s一次)
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 200, 200 , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0  , 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_WORKSTEP:	//单工步故障 - 红灯慢闪(1s一次)
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 500, 500 , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0  , 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_HARDWARE:	//(内部)硬件故障 - 红灯常亮
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 1000, 0   , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0   , 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_OTHER:	//(内部)其他故障 - 红灯亮0.8s,灭0.2s
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 800, 200 , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0  , 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_ACDC:	//ACDC故障 - 红灯亮0.2s,灭0.8s
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 200, 800 , LED_TASK_PERIOD);
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
**	函 数 名 : AppTaskLed()
**	功能说明 : LED任务
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AppTaskLed(void *argument)
{
	uint8_t LedTaskCnt = 0;
	
	while(1)
	{
		if(FCTRun_State != 0x02)
		{
			if(err_send_flag == 1)
			{
				err_send_flag = 0;
					
				LedTaskCnt = 0;
			}		
				
			if((LedTaskCnt < 5) && (SysModeProtMaskFlag == 0))
			{
				LedStaInit(LedTaskCnt);
				
				LedTaskCnt++;
			}
			else
			{
				LedTaskCnt = 5;
				
				LedStaSwFunc();
			}
				
			LedStaExecute();
		}
		else
		{
			Led_Shine_Ctrl(RUN_LED_R_NO, 0  , 1000, LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G_NO, 500, 500 , LED_TASK_PERIOD);
		}
		
		osDelay(LED_TASK_PERIOD);
  }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
