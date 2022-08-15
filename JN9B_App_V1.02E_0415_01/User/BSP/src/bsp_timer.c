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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_timer.h"
#include "bsp_protection.h"
#include "global.h"
#include "main.h"

//����
#include "bsp_led.h"
#include "bsp_gpio.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
__IO uint8_t Clear_AM_Flg = 0;                //����澯֪ͨ
uint8_t PG_state = 0;                         //PG״̬��bit����ҵͣ���ʾPG8~PG1
uint8_t write_flag_disable = 0;               //ACDC����д�����־λ
extern  Net_Mod Net_Mod_State;
extern 	__IO uint32_t record_upcast_cnt;
extern 	__IO uint32_t heart_beat_cnt;
extern  __IO uint8_t Recon_TimStart;          //������ʱ������־
extern __IO uint8_t Fram_Op;
extern uint8_t ACDC_UpdateFlg;
extern osSemaphoreId_t IPGP_Alarm_Semaphore;  //IPGP�澯�ź���ID
extern uint32_t GP_Alarm_Flag[8];             //ȫ�ֱ�����־λ

#if (M_LOOP_ENABLE == 1)
uint8_t workstep_flag_CriticalZone_Pre[8] = {0};
uint8_t CriticalZoneFlag[8] = {0};
uint16_t CriticalZoneCnt[8] = {0};
#endif

uint8_t Temp75SampCnt = 0,TempSampCHx = 1;
extern uint8_t PG_Fault_Flg[8];	//Ӳ�������жϱ�־		
uint64_t RunTime_S = 0;
/**********************************************************************************************************
**                                           ����������
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
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : TimerAUint1_Config
**	����˵�� : ��ʱ��A��Ԫ1��ʼ������
**	��    �� : ClkDiv - ʱ�ӷ�Ƶϵ����PerVal - �趨����ֵ
**	�� �� ֵ : ��
**  ˵    �� ������Ϊ40ms
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
**	�� �� �� : TimerAUint2_Config
**	����˵�� : ��ʱ��A��Ԫ2��ʼ������
**	��    �� : ClkDiv - ʱ�ӷ�Ƶϵ����PerVal - �趨����ֵ
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUint3_Config
**	����˵�� : ��ʱ��A��Ԫ3��ʼ������
**	��    �� : ClkDiv - ʱ�ӷ�Ƶϵ����PerVal - �趨����ֵ
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUint4_Config
**	����˵�� : ��ʱ��A��Ԫ4��ʼ������
**	��    �� : ClkDiv - ʱ�ӷ�Ƶϵ����PerVal - �趨����ֵ
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUint1_IrqConfig
**	����˵�� : ��ʱ��A��Ԫ1����ж�����
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUint2_IrqConfig
**	����˵�� : ��ʱ��A��Ԫ2����ж�����
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUint3_IrqConfig
**	����˵�� : ��ʱ��A��Ԫ3����ж�����
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUint4_IrqConfig
**	����˵�� : ��ʱ��A��Ԫ4����ж�����
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUnit1_OvfIrqCallback
**	����˵�� : ��ʱ��A��Ԫ1����жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� ���滻ԭJN95��TIM3_IRQHandler
**********************************************************************************************************/
static void TimerAUnit1_OvfIrqCallback(void)
{
	//����
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
		if(Cnt >= 50)	//5 * 8 = 40ms
		{
			Cnt = 0;           
			bsp_LedToggle(LED_82_NO);
			bsp_LedToggle(LED_83_NO);
		   
			IntelProtectRunParmCheck();    //�ڲ��������
			GlobleProtectRunParmCheck();   //ȫ�ֱ������
			
			if(Clear_AM_Flg)               //���յ�����澯����������и澯״̬
			{
				AlarmClear();
				Clear_AM_Flg = 0;
			}
			for(uint8_t i = 0; i < CHANNEL_USE; i++)
			{
				if((IP_Alarm_Flag[i] != 0)||(GP_Alarm_Flag[i] != 0))
				{
				   Process_Flag=1;
				   break;
				}
			}
//			if((Process_Flag)&&(ACDC_UpdateFlg==0))   //�澯��־��Ϊ0,��ACDC����������״̬
			if(Process_Flag != 0)   //�澯��־��Ϊ0
			{
				osSemaphoreRelease(IPGP_Alarm_Semaphore);    //�ͷ��ź�����ͬ���澯��������
				NVIC_DisableIRQ(APP_TMRA1_IRQn); 
			}
		}
		#endif
    }
}

/**********************************************************************************************************
**	�� �� �� : TimerAUnit2_OvfIrqCallback
**	����˵�� : ��ʱ��A��Ԫ2����жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� ���滻ԭJN95��TIM4_IRQHandler
**********************************************************************************************************/
uint8_t Aralm_Read_CHx = 0;

static void TimerAUnit2_OvfIrqCallback(void)
{
	static uint8_t cnt[8];
	static uint8_t i = 0;
	static uint8_t TimCnt = 0;
	
	static uint8_t Feed_WdgCnt = 0;	
		
	Feed_WdgCnt++;
	
	Can_Network_Func(1000, 8, 8, 8, 4);
	
	if(Feed_WdgCnt >= 25)	//100ms
	{
		Feed_WDG();  //���ι��	
		Feed_WdgCnt = 0;
	}
	
	if (TMRA_GetStatus(APP_TMRA2_UNIT, APP_TMRA2_FLAG) == Set)
	{
			TMRA_ClrStatus(APP_TMRA2_UNIT, APP_TMRA2_FLAG);
			TimCnt++;
		
			Temp75SampCnt++;
	}

	if(TimCnt >= 20)
	{
		for(uint8_t i = 0;i < 8;i++)
		{
			if(PG_Fault_Flg[i] == 0x01)
			{
				Aralm_Read_CHx = i;
			}
			else
			{
				Aralm_Read_CHx = 0x08;
			}
		}
		
		AllIOInfo_Read();	//���ȹ��ϡ���ع�ѹ�����غ͹��ʰ���λ�źŶ�ʱ��ȡ
		TimCnt = 0;
	}
	
	if(Temp75SampCnt >= 50)	//4ms * 250 = 1s ����һ��
	{
		FanPwmControl();
//		TempSampCHx++;	//���ʰ����ͨ���л�
//		if(TempSampCHx > 8)
//			TempSampCHx = 1;
		
		Temp75SampCnt = 0;
	}
	
	for(int j = 0; j < CHANNEL_USE; j++)
	{
		if(ProtectMaskFlag[j] == 1)
		{
			if(++ProtectMaskCnt[j] > (2000 / 4))
			{
				ProtectMaskFlag[j] = 0;
				ProtectMaskCnt[j] = 0;
			}
		}
	}	
	
}

uint8_t continue_store_delay_flag[8] = {0};
uint16_t continue_store_delay_cnt[8] = {0};

extern uint8_t heart_cnt_flag;

static uint64_t one_ms_cnt = 0;

/**********************************************************************************************************
**	�� �� �� : TimerAUnit3_OvfIrqCallback
**	����˵�� : ��ʱ��A��Ԫ3����жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� ���滻ԭJN95��TIM5_IRQHandler
**********************************************************************************************************/
static void TimerAUnit3_OvfIrqCallback(void)
{
	static uint8_t AdcCnt = 0;
//	static uint8_t Feed_WdgCnt = 0;	
	
	
	
	if (TMRA_GetStatus(APP_TMRA3_UNIT, APP_TMRA3_FLAG) == Set)
	{
		TMRA_ClrStatus(APP_TMRA3_UNIT, APP_TMRA3_FLAG);
		
		AdcCnt++;
		
#ifdef C042A_7606
		 if(AdcCnt%2 == 1)
				 AD7606_StartConV(&AD7606_A0);   
		 else 
			 if(AdcCnt%2 == 0)
				 AD7606_StartConV(&AD7606_A1);
#endif
			 
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
			if(Recon_TimStart != 1)	//������ʱ������־
			{
//				continue;
			}
			
			if(record_data_upcast_flag[i] == 1)
			{
				if((Tier2.workstepAnalyze[i].StepAbtTime > 0) && (Tier2.workstepRunning[i].Trun / RUN_TIME_PERIOD >= Tier2.workstepAnalyze[i].StepAbtTime) && (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_WAIT))
				{
					abort_type_flag[i] = SCMD_WORKSTEP_PARAM_TIME;
				}
				
				record_data_upcast_flag[i] = 0;
				
				RecordDatUpcast(i, 1);
			}
			else if(((run_time_cnt[i] % ((uint64_t)(data_upcast_period * 1000))) == 0) && (run_time_cnt[i] != 0))
			{
				if((Tier2.workstepAnalyze[i].StepAbtTime > 0) && (Tier2.workstepRunning[i].Trun / RUN_TIME_PERIOD >= Tier2.workstepAnalyze[i].StepAbtTime) && (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_WAIT))
				{
					abort_type_flag[i] = SCMD_WORKSTEP_PARAM_TIME;
				}
				
				RecordDatUpcast(i, 1);
			}
			
#if (M_LOOP_ENABLE == 1)			
			if((workstep_flag_CriticalZone[i] == 0) && (workstep_flag_CriticalZone_Pre[i] != 0))
			{
				/* MLOOP�� */
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
				/* MLOOP�� */
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
		
//		/* ������osTimer��ʽ����ѯ��osTimer�ᵼ��TimerA���в�����������ԭ����� */
//		AdcCnt++;
//		if(AdcCnt >= 10)
//		{
				ADC_Start_Stop_DMA();
//		}
	
	
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
	}	
}

/**********************************************************************************************************
**	�� �� �� : TimerAUnit4_OvfIrqCallback
**	����˵�� : ��ʱ��A��Ԫ4����жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� ����Ϊ����ʱ��Ƽ�ι��ʹ��
**********************************************************************************************************/
static void TimerAUnit4_OvfIrqCallback(void)
{
	static uint8_t Feed_WdgCnt = 0;	
	static uint32_t RunTime_Ms = 0;
	
	if (TMRA_GetStatus(APP_TMRA4_UNIT, APP_TMRA4_FLAG) == Set)
	{
		TMRA_ClrStatus(APP_TMRA4_UNIT, APP_TMRA4_FLAG);
		
		Tstamp++;
		
		RunTime_Ms++;
		if(RunTime_Ms%1000 == 0)
		{
			RunTime_S++;
			RunTime_Ms = 0;
		}
		
//		Can_Network_Func(1000, 5, 5, 5, 1);
		
//		Feed_WdgCnt++;
//	
//		if(Feed_WdgCnt >= 100)	//100ms
//		{
//			Feed_WDG();  //���ι��	
//			Feed_WdgCnt = 0;
//		}
	}
}

/**********************************************************************************************************
**	�� �� �� : AllTimerAUintStart
**	����˵�� : ������ʱ��A�������õ�Ԫ
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void AllTimerAUintStart(void)
{
	TMRA_Start(APP_TMRA1_UNIT);
	TMRA_Start(APP_TMRA2_UNIT);
	TMRA_Start(APP_TMRA3_UNIT);
}

/**********************************************************************************************************
**	�� �� �� : AllTimerAUintStop
**	����˵�� : ֹͣ��ʱ��A�������õ�Ԫ
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void AllTimerAUintStop(void)
{
  TMRA_Stop(APP_TMRA1_UNIT);
	TMRA_Stop(APP_TMRA2_UNIT);
	TMRA_Stop(APP_TMRA3_UNIT);
}

/**********************************************************************************************************
**	�� �� �� : TimerAUintStart
**	����˵�� : ������ʱ��Aָ���ĵ�Ԫ
**	��    �� : UnitNum - ָ����Ԫ
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUintStop
**	����˵�� : �رն�ʱ��Aָ���ĵ�Ԫ
**	��    �� : UnitNum - ָ����Ԫ
**	�� �� ֵ : ��
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
**	�� �� �� : bsp_GetRunTime()
**	����˵�� : ��ȡCPU����ʱ��(��λ1ms,����Ա�ʾ24.85��)
**	��    �� : ��
**	�� �� ֵ : CPU����ʱ��(1ms)
**********************************************************************************************************/
uint64_t bsp_GetRunTime(void)
{
	uint64_t runtime_ms;

	DISABLE_INT();  		/* ���ж� */
	runtime_ms = Tstamp;	/* ��Tim5�ж��б���д(��Ҫ���жϽ��б���) */
	ENABLE_INT();  			/* ���ж� */

	return runtime_ms;
}

/**********************************************************************************************************
**	�� �� �� : bsp_GetRunTime_S()
**	����˵�� : ��ȡCPU����ʱ��(��λ1s)
**	��    �� : ��
**	�� �� ֵ : CPU����ʱ��(1s)
**********************************************************************************************************/
uint64_t bsp_GetRunTime_S(void)
{
	uint64_t runtime_s;

	DISABLE_INT();  		/* ���ж� */
	runtime_s = RunTime_S;	/* ��Tim5�ж��б���д(��Ҫ���жϽ��б���) */
	ENABLE_INT();  			/* ���ж� */

	return runtime_s;
}
/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
