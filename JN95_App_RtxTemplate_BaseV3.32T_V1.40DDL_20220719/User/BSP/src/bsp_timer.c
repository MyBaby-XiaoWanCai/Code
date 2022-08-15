/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_timer.c
** Author      : Huang Cheng
** Date        : 2021-11-11
** Description : bsp_timer source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_timer.h"
#include "bsp_protection.h"
#include "global.h"
#include "main.h"

//测试
#include "bsp_led.h"
#include "bsp_gpio.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
__IO uint8_t Clear_AM_Flg = 0;                //清除告警通知
uint8_t PG_state = 0;                         //PG状态，bit左高右低，表示PG8~PG1
uint8_t write_flag_disable = 0;               //ACDC故障写铁电标志位
extern  Net_Mod Net_Mod_State;
extern 	__IO uint32_t record_upcast_cnt;
extern 	__IO uint32_t heart_beat_cnt;
extern  __IO uint8_t Recon_TimStart;          //重连定时启动标志
extern __IO uint8_t Fram_Op;



#if (M_LOOP_ENABLE == 1)
uint8_t workstep_flag_CriticalZone_Pre[8] = {0};
uint8_t CriticalZoneFlag[8] = {0};
uint16_t CriticalZoneCnt[8] = {0};
#endif

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void TimerAUint1_IrqConfig(void);
static void TimerAUnit1_OvfIrqCallback(void);
static void TimerAUint2_IrqConfig(void);
static void TimerAUnit2_OvfIrqCallback(void);
static void TimerAUint3_IrqConfig(void);
static void TimerAUnit3_OvfIrqCallback(void);


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : TimerAUint1_Config
**	功能说明 : 定时器A单元1初始化配置
**	形    参 : ClkDiv - 时钟分频系数，PerVal - 设定计数值
**	返 回 值 : 无
**  说    明 ：周期为40ms
**********************************************************************************************************/
void TimerAUint1_Config(uint32_t ClkDiv, uint32_t PerVal)
{
    stc_tmra_init_t stcInit;

    /* 1. Enable TimerA peripheral clock. */
    PWC_Fcg2PeriphClockCmd(APP_TMRA1_PERIP_CLK, Enable);

    /* 2. Set a default initialization value for stcInit. */
    (void)TMRA_StructInit(&stcInit);

    /* 3. Modifies the initialization values depends on the application. */
    stcInit.u32ClkSrc    = APP_TMRA1_CLK;
    stcInit.u32PCLKDiv   = ClkDiv;
    stcInit.u32CntDir    = APP_TMRA1_DIR;
    stcInit.u32CntMode   = APP_TMRA1_MODE;
    stcInit.u32PeriodVal = PerVal;
    (void)TMRA_Init(APP_TMRA1_UNIT, &stcInit);

    /* 4. Configures IRQ if needed. */
    TimerAUint1_IrqConfig();
}

/**********************************************************************************************************
**	函 数 名 : TimerAUint2_Config
**	功能说明 : 定时器A单元2初始化配置
**	形    参 : ClkDiv - 时钟分频系数，PerVal - 设定计数值
**	返 回 值 : 无
**********************************************************************************************************/
void TimerAUint2_Config(uint32_t ClkDiv, uint32_t PerVal)
{
    stc_tmra_init_t stcInit;

    /* 1. Enable TimerA peripheral clock. */
    PWC_Fcg2PeriphClockCmd(APP_TMRA2_PERIP_CLK, Enable);

    /* 2. Set a default initialization value for stcInit. */
    (void)TMRA_StructInit(&stcInit);

    /* 3. Modifies the initialization values depends on the application. */
    stcInit.u32ClkSrc    = APP_TMRA2_CLK;
    stcInit.u32PCLKDiv   = ClkDiv;
    stcInit.u32CntDir    = APP_TMRA2_DIR;
    stcInit.u32CntMode   = APP_TMRA2_MODE;
    stcInit.u32PeriodVal = PerVal;
    (void)TMRA_Init(APP_TMRA2_UNIT, &stcInit);

    /* 4. Configures IRQ if needed. */
    TimerAUint2_IrqConfig();
}

/**********************************************************************************************************
**	函 数 名 : TimerAUint3_Config
**	功能说明 : 定时器A单元3初始化配置
**	形    参 : ClkDiv - 时钟分频系数，PerVal - 设定计数值
**	返 回 值 : 无
**********************************************************************************************************/
void TimerAUint3_Config(uint32_t ClkDiv, uint32_t PerVal)
{
    stc_tmra_init_t stcInit;

    /* 1. Enable TimerA peripheral clock. */
    PWC_Fcg2PeriphClockCmd(APP_TMRA3_PERIP_CLK, Enable);

    /* 2. Set a default initialization value for stcInit. */
    (void)TMRA_StructInit(&stcInit);

    /* 3. Modifies the initialization values depends on the application. */
    stcInit.u32ClkSrc    = APP_TMRA3_CLK;
    stcInit.u32PCLKDiv   = ClkDiv;
    stcInit.u32CntDir    = APP_TMRA3_DIR;
    stcInit.u32CntMode   = APP_TMRA3_MODE;
    stcInit.u32PeriodVal = PerVal;
    (void)TMRA_Init(APP_TMRA3_UNIT, &stcInit);

    /* 4. Configures IRQ if needed. */
    TimerAUint3_IrqConfig();
}

/**********************************************************************************************************
**	函 数 名 : TimerAUint1_IrqConfig
**	功能说明 : 定时器A单元1溢出中断配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void TimerAUint1_IrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = APP_TMRA1_INT_SRC;
    stcCfg.enIRQn      = APP_TMRA1_IRQn;
    stcCfg.pfnCallback = &TimerAUnit1_OvfIrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);

    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, APP_TMRA1_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of TimerA. */
    TMRA_IntCmd(APP_TMRA1_UNIT, APP_TMRA1_INT_TYPE, Enable);
}

/**********************************************************************************************************
**	函 数 名 : TimerAUint2_IrqConfig
**	功能说明 : 定时器A单元2溢出中断配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void TimerAUint2_IrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = APP_TMRA2_INT_SRC;
    stcCfg.enIRQn      = APP_TMRA2_IRQn;
    stcCfg.pfnCallback = &TimerAUnit2_OvfIrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);

    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, APP_TMRA2_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of TimerA. */
    TMRA_IntCmd(APP_TMRA2_UNIT, APP_TMRA2_INT_TYPE, Enable);
}

/**********************************************************************************************************
**	函 数 名 : TimerAUint3_IrqConfig
**	功能说明 : 定时器A单元3溢出中断配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void TimerAUint3_IrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = APP_TMRA3_INT_SRC;
    stcCfg.enIRQn      = APP_TMRA3_IRQn;
    stcCfg.pfnCallback = &TimerAUnit3_OvfIrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);

    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, APP_TMRA3_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of TimerA. */
    TMRA_IntCmd(APP_TMRA3_UNIT, APP_TMRA3_INT_TYPE, Enable);
}

/**********************************************************************************************************
**	函 数 名 : TimerAUnit1_OvfIrqCallback
**	功能说明 : 定时器A单元1溢出中断回调函数配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：替换原JN95的TIM3_IRQHandler
**********************************************************************************************************/
static void TimerAUnit1_OvfIrqCallback(void)
{
	//测试
	static uint16_t Cnt = 0;
	uint8_t Process_Flag = 0;
	
    if (TMRA_GetStatus(APP_TMRA1_UNIT, APP_TMRA1_FLAG) == Set)
    {
        TMRA_ClrStatus(APP_TMRA1_UNIT, APP_TMRA1_FLAG);
		
		Cnt++;
		
		#ifdef TEST
		if(Cnt >= 50)
		{
//			bsp_LedToggle(LED_82_NO);
            bsp_LedToggle(LED_83_NO);
		   
			Cnt = 0;
		}
		#else
		if(Cnt >= 50)
		{
			Cnt = 0;           
			bsp_LedToggle(LED_82_NO);
			bsp_LedToggle(LED_83_NO);
		   
			Feed_WDG();
			
			IntelProtectRunParmCheck();    //内部保护检测
			GlobleProtectRunParmCheck();   //全局保护检测
			
			if(Clear_AM_Flg)               //若收到清除告警，则清除所有告警状态
			{
				AlarmClear();
				Clear_AM_Flg = 0;
			}
			for(uint8_t i = 0; i < CHANNEL_CFG; i++)
			{
				if((IP_Alarm_Flag[i] != 0)||(GP_Alarm_Flag[i] != 0))
				{
				   Process_Flag=1;
				   break;
				}
			}
			if((Process_Flag)&&(ACDC_UpdateFlg==0))   //告警标志不为0,且ACDC不处于升级状态
			{
				osSemaphoreRelease(IPGP_Alarm_Semaphore);    //释放信号量，同步告警上抛任务
				NVIC_DisableIRQ(APP_TMRA1_IRQn); 
			}
		}
		#endif
    }
}

/**********************************************************************************************************
**	函 数 名 : TimerAUnit2_OvfIrqCallback
**	功能说明 : 定时器A单元2溢出中断回调函数配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：替换原JN95的TIM4_IRQHandler
**********************************************************************************************************/
static void TimerAUnit2_OvfIrqCallback(void)
{
	static uint8_t cnt[8];
    static uint8_t i = 0;
	static uint8_t TimCnt = 0;
	
	/*
	 *@通道类型：1、2、4、8      模块类型[1:8] -> 模块下标[0:7]
	 *@说明：1通道为8个模块并联，2通道为1-4通道并联、5-8通道并联...
	 
	 *@通道故障IP_Alarm_Flag[x]       <----   模块故障Alarm_Flag[x]
	 * 1通道   IP_Alarm_Flag[0]       <----   Alarm_Flag[0:7]
	 * 2通道   IP_Alarm_Flag[0,1]     <----   Alarm_Flag[0:3,4:7]
	 * 4通道   IP_Alarm_Flag[0,1,2,3] <----   Alarm_Flag[0:1,2:3,4:5,6:7]
	 * 8通道   IP_Alarm_Flag[0:7]     <----   Alarm_Flag[0:7]
	*/
    if (TMRA_GetStatus(APP_TMRA2_UNIT, APP_TMRA2_FLAG) == Set)
    {
        TMRA_ClrStatus(APP_TMRA2_UNIT, APP_TMRA2_FLAG);
		TimCnt++;
    }
	
	/* 200ms处理一次 */
	if(TimCnt >= 50)
	{
		TimCnt = 0x00;
		
		Feed_WDG();  //喂狗
        
		
		if(ACDC_UpdateFlg == 0)      //ACDC升级模式下，不检查硬件故障
		{
		
			/* 轮询PG1~8,判定故障通道*/
			i++;
					
			for(uint8_t PgNum = 0; PgNum < PG_MaxNum; PgNum++)
			{
				/* 当前通道无故障信号 */
				if(0x00 == GPIO_ReadInputPins(PgPortPinTab[PgNum].GpioPort, PgPortPinTab[PgNum].GpioPin))
				{
					PG_state &= ~(0x01 << PgNum);
					continue;
				}
				
				cnt[PgNum]++;
				if((cnt[PgNum] >= 2) && (i == 2))  //两次判断都有故障信号才算触发故障
				{
					PG_state |= 0x01 << PgNum;
					
					switch(CHANNEL_CFG)
					{
						case CHANNEL_1:
						{
							AuxRecordDatSync(HPA_ERR);
							IP_Alarm_Flag[PgNum/CHANNEL_8] |= IntelProtect_HPA;
							break;
						}
						case CHANNEL_2:
						{
							AuxRecordDatSync(HPA_ERR);
							IP_Alarm_Flag[PgNum/CHANNEL_4] |= IntelProtect_HPA;
							break;
						}
						case CHANNEL_4:
						{
							AuxRecordDatSync(HPA_ERR);
							IP_Alarm_Flag[PgNum/CHANNEL_2] |= IntelProtect_HPA;
							break;
						}
						case CHANNEL_8:
						{
							AuxRecordDatSync(HPA_ERR);
							IP_Alarm_Flag[PgNum/CHANNEL_1] |= IntelProtect_HPA;
							break;
						}
						default:
						{
							break;
						}
					}
				}
			}

			/* 清零故障计数 */
			if(i == 2)
			{
				i = 0;
				for(uint8_t j = 0; j < 8; j++)
				{
					cnt[j] = 0;
				}
			}
			
		}
		
		
		
		
	}
}

uint8_t continue_store_delay_flag[8] = {0};
uint16_t continue_store_delay_cnt[8] = {0};

uint16_t bus_under_vol_cnt = 0;
uint16_t bus_under_vol_flg = 0;

/**********************************************************************************************************
**	函 数 名 : TimerAUnit3_OvfIrqCallback
**	功能说明 : 定时器A单元3溢出中断回调函数配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：替换原JN95的TIM5_IRQHandler
**********************************************************************************************************/
static void TimerAUnit3_OvfIrqCallback(void)
{	
    if (TMRA_GetStatus(APP_TMRA3_UNIT, APP_TMRA3_FLAG) == Set)
    {
        TMRA_ClrStatus(APP_TMRA3_UNIT, APP_TMRA3_FLAG);
		
		Tstamp++;

		if(bus_under_vol_cnt < 30000)
		{
			bus_under_vol_cnt++;
		}
		else
		{
			bus_under_vol_flg = 1;
		}
		
		if((Tstamp % 100) == 0)
		{
			Feed_WDG();  //喂狗
		}		
		
		heart_beat_cnt++;
		
		for(int i = 0; i < CHANNEL_CFG; i++)
		{
			if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || \
				(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
				(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_START  ) || \
				(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_WAIT)  )
			{
				Tier2.workstepRunning[i].Trun++;
				run_time_cnt[i] = Tier2.workstepRunning[i].Trun;
			}
			else
			{
				run_time_cnt[i] ++;
			}
					
					
	
			if((Recon_TimStart != 1) || (ACDC_UpdateFlg != 0))
			{
				continue;
			}
			
			if(record_data_upcast_flag[i] == 1)
			{
				if( (Tier2.workstepAnalyze[i].StepAbtTime > 0) && \
					(Tier2.workstepRunning[i].Trun / RUN_TIME_PERIOD >= Tier2.workstepAnalyze[i].StepAbtTime) && \
					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_WAIT) && \
					(abort_type_flag[i] == 0) )
				{
					abort_type_flag[i] = SCMD_WORKSTEP_PARAM_TIME;
				}
				
				record_data_upcast_flag[i] = 0;
				
				RecordDatUpcast(i, 1);
//				upcast_period_flag[i] = 1;
			}
			else if(((run_time_cnt[i] % ((uint64_t)(data_upcast_period * 1000))) == 0) && (run_time_cnt[i] != 0))
			{
				if( (Tier2.workstepAnalyze[i].StepAbtTime > 0) && \
					(Tier2.workstepRunning[i].Trun / RUN_TIME_PERIOD >= Tier2.workstepAnalyze[i].StepAbtTime) && \
					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_WAIT) && \
					(abort_type_flag[i] == 0) )
				{
					abort_type_flag[i] = SCMD_WORKSTEP_PARAM_TIME;
				}
				
				RecordDatUpcast(i, 1);
//				upcast_period_flag[i] = 1;
			}
			
#if (M_LOOP_ENABLE == 1)			
			if((workstep_flag_CriticalZone[i] == 0) && (workstep_flag_CriticalZone_Pre[i] != 0))
			{
				/* MLOOP低 */
#ifdef YCJ
				Disable_Mloop(i);
#else				
				Disable_Mloop(2 * i + 0);
				Disable_Mloop(2 * i + 1);
#endif
				CriticalZoneFlag[i] = 0;
				CriticalZoneCnt[i] = 0;				
			}
			else if((workstep_flag_CriticalZone[i] == 3) && (workstep_flag_CriticalZone_Pre[i] != 3))
			{
				/* MLOOP高 */
				CriticalZoneFlag[i] = 1;
				CriticalZoneCnt[i] = 0;
			}
			workstep_flag_CriticalZone_Pre[i] = workstep_flag_CriticalZone[i];
			
			if(CriticalZoneFlag[i] == 1)
			{
				if(CriticalZoneCnt[i] < 2000)
				{
					CriticalZoneCnt[i]++;
				}
				else
				{
					if(Tier2.workstepAnalyze[i].StepRunCur - Tier2.chRecord[i].Iout >= 1.0f)
					{
#ifdef YCJ
						Enable_Mloop(i);
#else						
						Enable_Mloop(2 * i + 0);
						Enable_Mloop(2 * i + 1);	
#endif
						CriticalZoneFlag[i] = 0;
						CriticalZoneCnt[i] = 0;							
					}
				}
			}
#endif			
			
		}


//      if(Fram_Op == 0)
		{
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				if( (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || \
					(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
					(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_START  ) || \
					(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_WAIT   ) || \
					(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_PAUSE  ) )
				{
					if(refresh_continue[j] == 0)
					{
						if((power_off_continue[j] != 1) && (continue_store_delay_flag[j] == 0))
						{
							power_off_continue[j] = 1;
							
							continue_store_delay_flag[j] = 1;
						}
					}
				}
				else
				{
					if(refresh_continue[j] == 0)
					{
						if((power_off_continue[j] != 0) && (continue_store_delay_flag[j] == 0))
						{
							power_off_continue[j] = 0;
							
							continue_store_delay_flag[j] = 1;
						}
					}
				}


				if(continue_store_delay_flag[j] == 1)
				{
					if(++continue_store_delay_cnt[j] > 2000)
					{
						continue_store_delay_cnt[j] = 0;
						
						continue_store_delay_flag[j] = 0;
						
						MB85Ctrl_Write(8208 + j, &power_off_continue[j], 1);
					}
				}
				
				
				
			}
	
			if(!Net_Mod_State.NetMod_Disable)
			{
				Net_Mod_State.NetMod_cnt++;
			}
			
			record_upcast_cnt++;
        }
		
        bsp_ProPer1ms();
        ExternalAdDatProcess();			
		

		ADC_Start_Stop_DMA();

	}	
}

/**********************************************************************************************************
**	函 数 名 : AllTimerAUintStart
**	功能说明 : 启动定时器A所有配置单元
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AllTimerAUintStart(void)
{
    TMRA_Start(APP_TMRA1_UNIT);
	TMRA_Start(APP_TMRA2_UNIT);
	TMRA_Start(APP_TMRA3_UNIT);
}

/**********************************************************************************************************
**	函 数 名 : AllTimerAUintStop
**	功能说明 : 停止定时器A所有配置单元
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AllTimerAUintStop(void)
{
    TMRA_Stop(APP_TMRA1_UNIT);
	TMRA_Stop(APP_TMRA2_UNIT);
	TMRA_Stop(APP_TMRA3_UNIT);
}

/**********************************************************************************************************
**	函 数 名 : TimerAUintStart
**	功能说明 : 启动定时器A指定的单元
**	形    参 : UnitNum - 指定单元
**	返 回 值 : 无
**********************************************************************************************************/
void TimerAUintStart(uint8_t UnitNum)
{
	switch(UnitNum)
	{
		case TMRA1_UNIT:
		{
			TMRA_Start(APP_TMRA1_UNIT);
			break;
		}
		case TMRA2_UNIT:
		{
			TMRA_Start(APP_TMRA2_UNIT);
			break;
		}
		case TMRA3_UNIT:
		{
			TMRA_Start(APP_TMRA3_UNIT);
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**	函 数 名 : TimerAUintStop
**	功能说明 : 关闭定时器A指定的单元
**	形    参 : UnitNum - 指定单元
**	返 回 值 : 无
**********************************************************************************************************/
void TimerAUintStop(uint8_t UnitNum)
{
	switch(UnitNum)
	{
		case TMRA1_UNIT:
		{
			TMRA_Stop(APP_TMRA1_UNIT);
			break;
		}
		case TMRA2_UNIT:
		{
			TMRA_Stop(APP_TMRA2_UNIT);
			break;
		}
		case TMRA3_UNIT:
		{
			TMRA_Stop(APP_TMRA3_UNIT);
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**	函 数 名 : bsp_GetRunTime()
**	功能说明 : 获取CPU运行时间(单位1ms,最长可以表示24.85天)
**	形    参 : 无
**	返 回 值 : CPU运行时间(1ms)
**********************************************************************************************************/
uint64_t bsp_GetRunTime(void)
{
    uint64_t runtime;

    DISABLE_INT();  		/* 关中断 */
    runtime = Tstamp;	/* 在Tim5中断中被改写(需要关中断进行保护) */
    ENABLE_INT();  			/* 开中断 */

    return runtime;
}
/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
