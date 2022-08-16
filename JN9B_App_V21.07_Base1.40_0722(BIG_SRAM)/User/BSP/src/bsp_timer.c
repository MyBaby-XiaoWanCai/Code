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
extern uint8_t ACDC_UpdateFlg;
extern osSemaphoreId_t IPGP_Alarm_Semaphore;  //IPGP告警信号量ID
extern uint32_t GP_Alarm_Flag[8];             //全局保护标志位

#if (M_LOOP_ENABLE == 1)
uint8_t workstep_flag_CriticalZone_Pre[8] = {0};
uint8_t CriticalZoneFlag[8] = {0};
uint16_t CriticalZoneCnt[8] = {0};
#endif

uint8_t Temp75SampCnt = 0,TempSampCHx = 1;
uint64_t RunTime_S = 0;
extern uint8_t PG_Fault_Flg[8];	//硬件故障记录 0-无故障，1-有故障
extern uint8_t JumpSharp_Read_Start;
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void TimerAUint1_IrqConfig(void);
static void TimerAUnit1_OvfIrqCallback(void);
static void TimerAUint2_IrqConfig(void);
static void TimerAUnit2_OvfIrqCallback(void);
static void TimerAUint3_IrqConfig(void);
static void TimerAUnit3_OvfIrqCallback(void);
static void TimerAUint4_IrqConfig(void);
static void TimerAUnit4_OvfIrqCallback(void);

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
**	函 数 名 : TimerAUint4_Config
**	功能说明 : 定时器A单元4初始化配置
**	形    参 : ClkDiv - 时钟分频系数，PerVal - 设定计数值
**	返 回 值 : 无
**********************************************************************************************************/
void TimerAUint4_Config(uint32_t ClkDiv, uint32_t PerVal)
{
    stc_tmra_init_t stcInit;

    /* 1. Enable TimerA peripheral clock. */
    PWC_Fcg2PeriphClockCmd(APP_TMRA4_PERIP_CLK, Enable);

    /* 2. Set a default initialization value for stcInit. */
    (void)TMRA_StructInit(&stcInit);

    /* 3. Modifies the initialization values depends on the application. */
    stcInit.u32ClkSrc    = APP_TMRA4_CLK;
    stcInit.u32PCLKDiv   = ClkDiv;
    stcInit.u32CntDir    = APP_TMRA4_DIR;
    stcInit.u32CntMode   = APP_TMRA4_MODE;
    stcInit.u32PeriodVal = PerVal;
    (void)TMRA_Init(APP_TMRA4_UNIT, &stcInit);

    /* 4. Configures IRQ if needed. */
    TimerAUint4_IrqConfig();
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
**	函 数 名 : TimerAUint4_IrqConfig
**	功能说明 : 定时器A单元4溢出中断配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void TimerAUint4_IrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = APP_TMRA4_INT_SRC;
    stcCfg.enIRQn      = APP_TMRA4_IRQn;
    stcCfg.pfnCallback = &TimerAUnit4_OvfIrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);

    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, APP_TMRA4_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of TimerA. */
    TMRA_IntCmd(APP_TMRA4_UNIT, APP_TMRA4_INT_TYPE, Enable);
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
    static uint8_t PG_Cnt = 0;

    if (TMRA_GetStatus(APP_TMRA1_UNIT, APP_TMRA1_FLAG) == Set)
    {
        Cnt++;
        PG_Cnt++;
				Temp75SampCnt++;

        if(Cnt >= 50)	//50 * 8 = 400ms
        {
            Cnt = 0;

						if(g_ucSysMode != SYS_CALIBRATION_MODE)	//在校准模式下，不进行内部、全局保护检测
						{
							IntelProtectRunParmCheck();    //内部保护检测
							GlobleProtectRunParmCheck();   //全局保护检测
						}

            if(Clear_AM_Flg)               //若收到清除告警，则清除所有告警状态
            {
                AlarmClear();
                Clear_AM_Flg = 0;
            }
            for(uint8_t i = 0; i < CHANNEL_USE; i++)
            {
                if((IP_Alarm_Flag[i] != 0)||(GP_Alarm_Flag[i] != 0))
                {
                    Process_Flag = 1;
                    break;
                }
            }
//						if((Process_Flag)&&(ACDC_UpdateFlg==0))   //告警标志不为0,且ACDC不处于升级状态
            if(Process_Flag != 0)   //告警标志不为0
            {
                osSemaphoreRelease(IPGP_Alarm_Semaphore);    //释放信号量，同步告警上抛任务
                NVIC_DisableIRQ(APP_TMRA1_IRQn);
            }
        }

        if(PG_Cnt%10==0)
        {
            for(uint8_t i = 0; i<CHANNEL_USE; i++)
            {
                if(PG_Fault_Flg[i]!=0x00)
                {
                    HardwareFaultTypeRead(i);	//硬件故障读取
                }
            }
        }
				
				if(Temp75SampCnt >= 40)	//320ms
				{
					if(JumpSharp_Read_Start == 0x01)
					{
						bsp_LedToggle(LED_82_NO);	//板载灯
						bsp_LedToggle(LED_83_NO);
					}
					
					FanPwmControl();	//风扇调速
					AllIOInfo_Read();	//风扇故障、电池过压、主控和功率板在位信号定时读取
					
		//			SysTempSamp(TempSampCHx);	//设备温度采样
		//			TempSampCHx++;	//功率板采样通道切换
		//			if(TempSampCHx > 8)
		//				TempSampCHx = 1;

					Temp75SampCnt = 0;
				}
				
				TMRA_ClrStatus(APP_TMRA1_UNIT, APP_TMRA1_FLAG);
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
    static uint8_t Feed_WdgCnt = 0;

    if (TMRA_GetStatus(APP_TMRA2_UNIT, APP_TMRA2_FLAG) == Set)
    {
			Feed_WdgCnt++;

			Can_Network_Func(1000, 8, 100, 8, 4);

			if(Feed_WdgCnt >= 25)	//100ms
			{
					Feed_WDG();  //间隔喂狗
					Feed_WdgCnt = 0;
			}			
			
			for(int j = 0; j < CHANNEL_USE; j++)
			{
					if(ProtectMaskFlag[j] == 1)
					{
							if(++ProtectMaskCnt[j] > (1000 / 4))
							{
									ProtectMaskFlag[j] = 0;	//故障屏蔽标识
									ProtectMaskCnt[j] = 0;
							}
					}
			}
			
			TMRA_ClrStatus(APP_TMRA2_UNIT, APP_TMRA2_FLAG);
		}
}

uint8_t continue_store_delay_flag[8] = {0};
uint16_t continue_store_delay_cnt[8] = {0};

extern uint8_t heart_cnt_flag;

static uint64_t one_ms_cnt = 0;

/**********************************************************************************************************
**	函 数 名 : TimerAUnit3_OvfIrqCallback
**	功能说明 : 定时器A单元3溢出中断回调函数配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：替换原JN95的TIM5_IRQHandler
**********************************************************************************************************/
static void TimerAUnit3_OvfIrqCallback(void)
{
    static uint8_t AdcCnt = 0;

    if (TMRA_GetStatus(APP_TMRA3_UNIT, APP_TMRA3_FLAG) == Set)
    {
        AdcCnt++;

#ifdef C042A_7606
        if(AdcCnt%2 == 1)
            AD7606_StartConV(&AD7606_A0);
        else if(AdcCnt%2 == 0)
            AD7606_StartConV(&AD7606_A1);
#endif

				bsp_ProPer1ms();
				ExternalAdDatProcess();

//		/* 不采用osTimer方式来轮询，osTimer会导致TimerA运行不正常，具体原因待查 */
//		AdcCnt++;
//		if(AdcCnt >= 10)
//		{
				 ADC_Start_Stop_DMA();
//		}
				
				for(int j = 0; j < CHANNEL_USE; j++)
				{
					if(continue_store_delay_flag[j] == 1)
					{
						if(++continue_store_delay_cnt[j] > 2000)
						{
							continue_store_delay_cnt[j] = 0;

							continue_store_delay_flag[j] = 0;

							MB85Ctrl_Write(8208 + j, &power_off_continue[j], 1 ,0);
						}
					}
				}
				
				TMRA_ClrStatus(APP_TMRA3_UNIT, APP_TMRA3_FLAG);
    }
}

uint16_t bus_under_vol_cnt = 0;           //bus电压检测延时计数
uint16_t bus_under_vol_flg = 0;           //bus电压检测标志
/**********************************************************************************************************
**	函 数 名 : TimerAUnit4_OvfIrqCallback
**	功能说明 : 定时器A单元4溢出中断回调函数配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：作为运行时间计及喂狗使用
**********************************************************************************************************/
static void TimerAUnit4_OvfIrqCallback(void)
{
    static uint32_t RunTime_Ms = 0;
	
    if (TMRA_GetStatus(APP_TMRA4_UNIT, APP_TMRA4_FLAG) == Set)
    {
        if(bus_under_vol_cnt < 30000)           //开机等30秒再检测bus电压
				{
					bus_under_vol_cnt++;
				}
				else
				{
					bus_under_vol_flg = 1;
				}
				
        Tstamp++;
        RunTime_Ms++;
        if(RunTime_Ms%1000 == 0)
        {
            RunTime_S++;	//秒计时
            RunTime_Ms = 0;
        }
				
        for(int i = 0; i < CHANNEL_USE; i++)
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

//			if((Recon_TimStart != 1) || (ACDC_UpdateFlg != 0))
            if(Recon_TimStart != 1)	//重连定时启动标志
            {
//							continue;
            }

            if(record_data_upcast_flag[i] == 1)	//截止上抛
            {
                if( (Tier2.workstepAnalyze[i].StepAbtTime > 0) && (Tier2.workstepRunning[i].Trun / RUN_TIME_PERIOD >= Tier2.workstepAnalyze[i].StepAbtTime) && (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_WAIT)&& \
										(abort_type_flag[i] == 0) )
                {
                    abort_type_flag[i] = SCMD_WORKSTEP_PARAM_TIME;
                }

                record_data_upcast_flag[i] = 0;

                RecordDatUpcast(i, 1);
            }
            else if(((run_time_cnt[i] % ((uint64_t)(data_upcast_period * 1000))) == 0) && (run_time_cnt[i] != 0))
            {
                if(	(Tier2.workstepAnalyze[i].StepAbtTime > 0) && (Tier2.workstepRunning[i].Trun / RUN_TIME_PERIOD >= Tier2.workstepAnalyze[i].StepAbtTime) && (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_WAIT)&& \
										(abort_type_flag[i] == 0) )
                {
                    abort_type_flag[i] = SCMD_WORKSTEP_PARAM_TIME;
                }

                RecordDatUpcast(i, 1);
            }

#if (M_LOOP_ENABLE == 1)
            if((workstep_flag_CriticalZone[i] == 0) && (workstep_flag_CriticalZone_Pre[i] != 0))
            {
                /* MLOOP低 */
#ifdef YCJ
                Disable_Mloop(i);	//快速环设置
#else
                Disable_Mloop(2 * i + 0);
                Disable_Mloop(2 * i + 1);
#endif
                CriticalZoneFlag[i] = 0;
                CriticalZoneCnt[i] = 0;
            }
            else if((workstep_flag_CriticalZone[i] == 3) && (workstep_flag_CriticalZone_Pre[i] != 3))	//CV
            {
                /* MLOOP高 */
                CriticalZoneFlag[i] = 1;
                CriticalZoneCnt[i] = 0;
            }
            workstep_flag_CriticalZone_Pre[i] = workstep_flag_CriticalZone[i];

            if(CriticalZoneFlag[i] == 1)
            {
								if(CriticalZoneCnt[i] < 100)	
                {
                    CriticalZoneCnt[i]++;
                }
                else
                {
                    if(Tier2.workstepAnalyze[i].StepRunCur - Tier2.chRecord[i].Iout >= 1.0f)						
                    {
#ifdef YCJ
                        Enable_Mloop(i);	//慢速环设置
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

//  if(Fram_Op == 0)
        {
            for(int j = 0; j < CHANNEL_USE; j++)
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

//                if(continue_store_delay_flag[j] == 1)
//                {
//                    if(++continue_store_delay_cnt[j] > 2000)
//                    {
//                        continue_store_delay_cnt[j] = 0;

//                        continue_store_delay_flag[j] = 0;

//                        MB85Ctrl_Write(8208 + j, &power_off_continue[j], 1);
//                    }
//                }
            }

            if(!Net_Mod_State.NetMod_Disable)
            {
                Net_Mod_State.NetMod_cnt++;
            }

            record_upcast_cnt++;
        }

        if(ageing_cnt_flag == 1)
        {
            one_ms_cnt++;

            if((one_ms_cnt % 10) == 0)
            {
                ageing_cnt++;
            }
        }

        if(heart_cnt_flag == 1)
        {
            heart_beat_cnt++;
        }				
				
				TMRA_ClrStatus(APP_TMRA4_UNIT, APP_TMRA4_FLAG);
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
    TMRA_Start(APP_TMRA4_UNIT);
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
    TMRA_Stop(APP_TMRA4_UNIT);
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
    case TMRA4_UNIT:
    {
        TMRA_Start(APP_TMRA4_UNIT);
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
    case TMRA4_UNIT:
    {
        TMRA_Stop(APP_TMRA4_UNIT);
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
    uint64_t runtime_ms;

    DISABLE_INT();  		/* 关中断 */
    runtime_ms = Tstamp;	/* 在Tim5中断中被改写(需要关中断进行保护) */
    ENABLE_INT();  			/* 开中断 */

    return runtime_ms;
}

/**********************************************************************************************************
**	函 数 名 : bsp_GetRunTime_S()
**	功能说明 : 获取CPU运行时间(单位1s)
**	形    参 : 无
**	返 回 值 : CPU运行时间(1s)
**********************************************************************************************************/
uint64_t bsp_GetRunTime_S(void)
{
    uint64_t runtime_s;

    DISABLE_INT();  		/* 关中断 */
    runtime_s = RunTime_S;	/* 在Tim5中断中被改写(需要关中断进行保护) */
    ENABLE_INT();  			/* 开中断 */

    return runtime_s;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
