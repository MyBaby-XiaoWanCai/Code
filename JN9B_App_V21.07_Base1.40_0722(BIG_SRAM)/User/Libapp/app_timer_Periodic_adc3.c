/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :JN95
** --------------------------------------------------------------------------------------------------------
** File name   : app_timer_periodic_adc3.c
** Author      : Robert.Li
** Date        : 2021-07-14
** Description : adc3 call back task file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
int64_t Tstamp = 0;
extern uint8_t SelfTest_flag;
extern uint32_t GP_Alarm_Flag[8];       //全局保护标志位
extern __IO uint8_t ACDC_UpdateFlg;

//测试
extern uint8_t CanTestCnt;

//static uint8_t VUQ_Count[8] = {0};    // 电压快速上升计数
//static uint8_t VDQ_Count[8] = {0};    // 电压快速下降计数
//static uint8_t CUQ_Count[8] = {0};    // 电流快速上升计数
//static float lastVbat[8] = {0};       // 上次电压值
//static float lastCmod[8] = {0};       // 上次电流值

extern uint8_t JumpSharp_Read_Start;
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
float Voltage_Value_Soft_Process(float vol_value);      //小电压处理

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : 	timer_Periodic_Adc3()
**	功能说明 :  定时轮询ADC3
**	形    参 : 	无
**	返 回 值 : 	无
**********************************************************************************************************/
void timer_Periodic_Adc3(void *argument)
{
    ADC_Start_Stop_DMA();
}

/**********************************************************************************************************
**	函 数 名 : ExternalAdDatProcess()
**	功能说明 : 外部AD采集数据处理
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ExternalAdDatProcess(void)
{
    static uint16_t ins_cnt = 0;
    static uint16_t sum_cnt = 0;
	
    for(int i = 0; i < CHANNEL_USE; i++)
    {
		#ifdef C042A_7606
			if(SGM58601_Data[i] > 0x7FFFFF)
			{
				SGM58601_Data[i] = 0;
			}		
			Tier2.sampleCal.VsenseIns[i] += AD7606_A0.AVG_ADC[i];
			Tier2.sampleCal.IoutIns[i] += AD7606_A1.AVG_ADC[i];
			Tier2.sampleCal.VtzIns[i] += SGM58601_Data[i];

			Tier2.sampleCal.VsenseSum[i] += AD7606_A0.AVG_ADC[i];
			Tier2.sampleCal.IoutSum[i] += AD7606_A1.AVG_ADC[i];
			if(SGM58601_Data[i] > 0x7FFFFF)
			{
				SGM58601_Data[i] = 0;
			}
			Tier2.sampleCal.VtzSum[i] += SGM58601_Data[i];
		#else
        Tier2.sampleCal.VsenseIns[i] += ad7175_value[i];
        Tier2.sampleCal.IoutIns[i] += ad7175_value[i + 8];
        Tier2.sampleCal.VtzIns[i] += ad7124_value[i * 2];

        Tier2.sampleCal.VsenseSum[i] += ad7175_value[i];
        Tier2.sampleCal.IoutSum[i] += ad7175_value[i + 8];
        Tier2.sampleCal.VtzSum[i] += ad7124_value[i * 2];
		#endif	
			
				Tier2.sampleCal.VmodSum[i] += ADCx_Vmod[i];
    }
    ins_cnt++;
    sum_cnt++;

    if(ins_cnt >= INS_SAMPLE_NUM)
    {
        for(int i = 0; i < CHANNEL_USE; i++)
        {           
#if AD_uinpolar     //单极性ADC数据处理
			if(Mod_DIR_Flag[i] == BUCK_MODE)
            {
                Tier2.sampleCal.Vsense[i] = (float)((Tier2.calibrate[i].VsenseK_C * (Tier2.sampleCal.VsenseIns[i] / ins_cnt)) / 16777216) + Tier2.calibrate[i].VsenseB_C;
                Tier2.sampleCal.Iout[i]   = (float)((Tier2.calibrate[i].IhK_C     * (Tier2.sampleCal.IoutIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].IhB_C;
                Tier2.sampleCal.Vtz[i]   = (float)((Tier2.calibrate[i].VtzK_C   * (Tier2.sampleCal.VtzIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].VtzB_C;
            }
            else if(Mod_DIR_Flag[i] == BOOST_MODE)
            {
                Tier2.sampleCal.Vsense[i] = (float)((Tier2.calibrate[i].VsenseK_D * (Tier2.sampleCal.VsenseIns[i] / ins_cnt)) / 16777216) + Tier2.calibrate[i].VsenseB_D;
                Tier2.sampleCal.Iout[i]   = (float)((Tier2.calibrate[i].IhK_D     * (Tier2.sampleCal.IoutIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].IhB_D;
                Tier2.sampleCal.Vtz[i]   = (float)((Tier2.calibrate[i].VtzK_C   * (Tier2.sampleCal.VtzIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].VtzB_C;
            }
#else       //双极性ADC数据处理
						if(Mod_DIR_Flag[i] == BUCK_MODE)
            {
							#ifdef C042A_7606
								Tier2.sampleCal.Vsense[i] = (float)((Tier2.calibrate[i].VsenseK_C * (Tier2.sampleCal.VsenseIns[i]/ ins_cnt))*1024 / 16777216 + Tier2.calibrate[i].VsenseB_C);															
								Tier2.sampleCal.Iout[i] = (float)((Tier2.calibrate[i].IhK_C * (Tier2.sampleCal.IoutIns[i] / ins_cnt))*1024 / 16777216) + Tier2.calibrate[i].IhB_C;
								Tier2.sampleCal.Vtz[i]   = (float)((Tier2.calibrate[i].VtzK_C * (Tier2.sampleCal.VtzIns[i] / ins_cnt))*4 / 16777216) + Tier2.calibrate[i].VtzB_C;
							#else
                Tier2.sampleCal.Vsense[i] = (float)((Tier2.calibrate[i].VsenseK_C * 2 * (Tier2.sampleCal.VsenseIns[i] / ins_cnt)) / 16777216) + Tier2.calibrate[i].VsenseB_C - Tier2.calibrate[i].VsenseK_C;
                Tier2.sampleCal.Iout[i]   = (float)((Tier2.calibrate[i].IhK_C * 2 * (Tier2.sampleCal.IoutIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].IhB_C - Tier2.calibrate[i].IhK_C;
                Tier2.sampleCal.Vtz[i]   = (float)((Tier2.calibrate[i].VtzK_C * 2 * (Tier2.sampleCal.VtzIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].VtzB_C - Tier2.calibrate[i].VtzK_C;
							#endif
						}
            else if(Mod_DIR_Flag[i] == BOOST_MODE)
            {
							#ifdef C042A_7606
								Tier2.sampleCal.Vsense[i] = (float)((Tier2.calibrate[i].VsenseK_D * (Tier2.sampleCal.VsenseIns[i] / ins_cnt))*1024 / 16777216 + Tier2.calibrate[i].VsenseB_D);															
                Tier2.sampleCal.Iout[i] = (float)((Tier2.calibrate[i].IhK_D * (Tier2.sampleCal.IoutIns[i] / ins_cnt))*1024 / 16777216) + Tier2.calibrate[i].IhB_D;
								Tier2.sampleCal.Vtz[i]   = (float)((Tier2.calibrate[i].VtzK_D * (Tier2.sampleCal.VtzIns[i] / ins_cnt))*4 / 16777216) + Tier2.calibrate[i].VtzB_D;
							#else
                Tier2.sampleCal.Vsense[i] = (float)((Tier2.calibrate[i].VsenseK_D * 2 * (Tier2.sampleCal.VsenseIns[i] / ins_cnt)) / 16777216) + Tier2.calibrate[i].VsenseB_D - Tier2.calibrate[i].VsenseK_D;
                Tier2.sampleCal.Iout[i]   = (float)((Tier2.calibrate[i].IhK_D * 2 * (Tier2.sampleCal.IoutIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].IhB_D - Tier2.calibrate[i].IhK_D;
								Tier2.sampleCal.Vtz[i]   = (float)((Tier2.calibrate[i].VtzK_C * 2 * (Tier2.sampleCal.VtzIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].VtzB_C - Tier2.calibrate[i].VtzK_C;
							#endif
						}
#endif
						else
						{
								Tier2.sampleCal.Vsense[i] = 0;
								Tier2.sampleCal.Iout[i]   = 0;
								Tier2.sampleCal.Vtz[i]   = 0;
						}
            Tier2.sampleCal.VsenseIns[i] = 0;
            Tier2.sampleCal.IoutIns[i]   = 0;
            Tier2.sampleCal.VtzIns[i]   = 0;

//#ifdef V_TZ_SP
				if(((SystemStatus.Neilsbed == Neilsbed_Nofit) || ((Tier2.sampleCal.Vsense[i] < 0.010f) && (Tier2.sampleCal.Vsense[i] > -0.010f))) && (g_ucSysMode == SYS_NORMAL_MODE))
				{
					Tier2.sampleCal.Vtz[i] = Voltage_Value_Soft_Process(Tier2.sampleCal.Vtz[i]);
				}
//#endif	                        
                        
					/* debug模式使用 */
					Tier2.debug.inq1[i].VsenseDebug = Tier2.sampleCal.Vsense[i];
					if(Mod_DIR_Flag[i] == BUCK_MODE)
					{
						Tier2.debug.inq1[i].IoutDebug = Tier2.sampleCal.Iout[i];
					}
					else if(Mod_DIR_Flag[i] == BOOST_MODE)
					{
						Tier2.debug.inq1[i].IoutDebug = -Tier2.sampleCal.Iout[i];
					}
					Tier2.debug.inq1[i].VtzDebug   = Tier2.sampleCal.Vtz[i];					
        }
        ins_cnt = 0;
    }

		/**************************************** 电压、电流骤升骤降测试 ********************************************/
#if 0		
		for(uint8_t cntcnt = 0; cntcnt < CHANNEL_CFG; cntcnt++)
		{
			if(JumpSharp_Read_Start == 0x01)	//检测开始标志
			{
				if(((Tier2.iProt[cntcnt].AmpVdrop.isEnable & 0x80) == 0) && ((Tier2.iProt[cntcnt].CntVdrop.isEnable & 0x80) == 0))
				{
						if((SystemStatus.Neilsbed == Neilsbed_Nofit) || (SystemStatus.Neilsbed == Neilsbed_Fitted))
						{
								if(lastVbat[cntcnt] - Tier2.chRecord[cntcnt].Vbat > Tier2.iProt[cntcnt].AmpVdrop.value)
								{
										VDQ_Count[cntcnt]++;
								}
								else
								{
										VDQ_Count[cntcnt] = 0;
								}
						}
						else
						{
								CUQ_Count[cntcnt] = 0;
						}
						if(VDQ_Count[cntcnt] >= Tier2.iProt[cntcnt].CntVdrop.value)
						{
								IP_Alarm_Flag[cntcnt] |= IntelProtect_VDQ;
						}
				}
				if(((Tier2.iProt[cntcnt].AmpVup.isEnable & 0x80) == 0) && ((Tier2.iProt[cntcnt].CntVup.isEnable & 0x80) == 0))
				{
						if((SystemStatus.Neilsbed == Neilsbed_Nofit) || (SystemStatus.Neilsbed == Neilsbed_Fitted))
						{
								if(Tier2.chRecord[cntcnt].Vbat - lastVbat[cntcnt] > Tier2.iProt[cntcnt].AmpVup.value)
								{
										VUQ_Count[cntcnt]++;
								}
								else
								{
										VUQ_Count[cntcnt] = 0;
								}
						}
						else
						{
								CUQ_Count[cntcnt] = 0;
						}
						if(VUQ_Count[cntcnt] >= Tier2.iProt[cntcnt].CntVup.value)
						{
								IP_Alarm_Flag[cntcnt] |= IntelProtect_VUQ;
						}
				}
				lastVbat[cntcnt] = Tier2.chRecord[cntcnt].Vbat;
				if(((Tier2.iProt[cntcnt].AmpCup.isEnable & 0x80) == 0) && \
								((Tier2.iProt[cntcnt].CntCup.isEnable & 0x80) == 0))
				{
						if((SystemStatus.Neilsbed == Neilsbed_Nofit) || (SystemStatus.Neilsbed == Neilsbed_Fitted))
						{
								if(Tier2.chRecord[cntcnt].Iout - lastCmod[cntcnt]  > Tier2.iProt[cntcnt].AmpCup.value)
								{
										CUQ_Count[cntcnt]++;
								}
								else
								{
										CUQ_Count[cntcnt] = 0;
								}
						}
						else
						{
								CUQ_Count[cntcnt] = 0;
						}
						if(CUQ_Count[cntcnt] > Tier2.iProt[cntcnt].CntCup.value)
						{
								IP_Alarm_Flag[cntcnt] |= IntelProtect_CUQ;
						}
				}
				lastCmod[cntcnt] = Tier2.chRecord[cntcnt].Iout;			
			}
		}			
#endif		
		/**************************************** 电压、电流骤升骤降测试 ********************************************/			
		
    if(sum_cnt >= SUM_SAMPLE_NUM)
    {
        for(int i = 0; i < CHANNEL_USE; i++)
        {
#if AD_uinpolar   //单极性ADC编码输出            
            Tier2.adSum[i].VsenseAdSum = Tier2.sampleCal.VsenseSum[i]/SUM_SAMPLE_NUM;
            Tier2.adSum[i].VmodAdSum   = Tier2.sampleCal.VmodSum[i];
            Tier2.adSum[i].IoutAdSum   = Tier2.sampleCal.IoutSum[i];
            Tier2.adSum[i].VtzAdSum   = Tier2.sampleCal.VtzSum[i];
#else       //7124，7175双极性输出模式
					#ifdef C042A_7606
						Tier2.adSum[i].VsenseAdSum = Tier2.sampleCal.VsenseSum[i]/SUM_SAMPLE_NUM * 1024;
            Tier2.adSum[i].IoutAdSum   = Tier2.sampleCal.IoutSum[i] /SUM_SAMPLE_NUM* 1024;
            Tier2.adSum[i].VtzAdSum   = Tier2.sampleCal.VtzSum[i]/SUM_SAMPLE_NUM * 4;
								
						Tier2.adSum[i].VmodAdSum   = Tier2.sampleCal.VmodSum[i]/SUM_SAMPLE_NUM;	
					#else
            if(Tier2.sampleCal.VsenseSum[i] > (0x7FFFFF * SUM_SAMPLE_NUM))
            {
                Tier2.adSum[i].VsenseAdSum = 2 * (Tier2.sampleCal.VsenseSum[i] - (0x7FFFFF * SUM_SAMPLE_NUM));
            }
            else
            {
                Tier2.adSum[i].VsenseAdSum = 0;
            }
						
            Tier2.adSum[i].VmodAdSum   = Tier2.sampleCal.VmodSum[i];
						
            if(Tier2.sampleCal.IoutSum[i] > (0x7FFFFF * SUM_SAMPLE_NUM))
            {
                Tier2.adSum[i].IoutAdSum = 2 * (Tier2.sampleCal.IoutSum[i] - (0x7FFFFF * SUM_SAMPLE_NUM));
            }
            else
            { 
							Tier2.adSum[i].IoutAdSum = 0; 
						}
            if(Tier2.sampleCal.VtzSum[i] > (0x7FFFFF * SUM_SAMPLE_NUM))
            {
                Tier2.adSum[i].VtzAdSum = 2 * (Tier2.sampleCal.VtzSum[i] - (0x7FFFFF * SUM_SAMPLE_NUM));
            }
            else
            {
                Tier2.adSum[i].VtzAdSum = 0;
            }
						#endif
#endif
            Tier2.sampleCal.VsenseSum[i] = 0;
            Tier2.sampleCal.VmodSum[i] = 0;
            Tier2.sampleCal.IoutSum[i] = 0;
            Tier2.sampleCal.VtzSum[i] = 0;
        }
        sum_cnt = 0;
    }

//    if(g_ucSysMode == SYS_NORMAL_MODE)
//    {
//        for(int i = 0; i < CHANNEL_MAX; i++)
//        {
//            if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || \
//                    (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) )
//            {
//                Tier2.workstepRunning[i].Trun++;
//            }
//        }
//    }
}

/**********************************************************************************************************
**	函 数 名 : RunPer1ms()
**	功能说明 : 该函数每隔1ms被Systick中断调用1次
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void RunPer1ms(void)
{
    /* do nothing */
}

#define CBAT_PERIOD		0.01f
#define EBAT_PERIOD		0.01f

/**********************************************************************************************************
**	函 数 名 : Voltage_Value_Soft_Process()
**	功能说明 : 用于小信号处理
**	形    参 : 原始数据
**	返 回 值 : 处理后输出数据
**********************************************************************************************************/
float Voltage_Value_Soft_Process(float vol_value)
{
	float ret_val = 0.0f;
	
	if(vol_value < -0.100f)			//【-x , -100】=>【-x, -100】
	{
		ret_val = vol_value;
	}
	else if(vol_value < -0.050f)	//【-100, 8 】
	{
		ret_val = 0.008f + (vol_value + 0.050f) * 2.16f;
	}
	else if(vol_value < -0.040f)	//【8 ,   -2】
	{
		ret_val = -0.002f - (vol_value + 0.040f);
	}
	else if(vol_value < -0.030f)	//【-2, 8   】
	{
		ret_val = 0.008f + (vol_value + 0.030f);
	}
	else if(vol_value < -0.020f)	//【8 , -2  】
	{
		ret_val = -0.002f - (vol_value + 0.020f);
	}
	else if(vol_value < -0.010f)	//【-2, 8   】
	{
		ret_val = 0.008f + (vol_value + 0.010f);
	}
	else if(vol_value < 0.000f)		//【8 , -2  】
	{
		ret_val = -0.002f - (vol_value + 0.000f);
	}
	else if(vol_value < 0.010f)		//【-2, 8   】
	{
		ret_val = -0.002f + (vol_value - 0.000f);
	}
	else if(vol_value < 0.020f)		//【8 , -2  】
	{
		ret_val = 0.008f - (vol_value - 0.010f);
	}
	else if(vol_value < 0.030f)		//【-2, 8   】
	{
		ret_val = -0.002f + (vol_value - 0.020f);
	}
	else if(vol_value < 0.040f)		//【8 , -2  】
	{
		ret_val = 0.008f - (vol_value - 0.030f);
	}
	else if(vol_value < 0.050f)		//【-2, 8   】
	{
		ret_val = -0.002f + (vol_value - 0.040f);
	}
	else if(vol_value < 0.100f)		//【8 , 100 】
	{
		ret_val = 0.008f + (vol_value - 0.050f) * 1.84f;
	}
	else
	{
		ret_val = vol_value;
	}
		
	return ret_val;
}


extern uint16_t bus_under_vol_flg;
extern uint8_t  ucIProtCntPre[12][8];		// 内部故障检测计时(前次)
extern uint8_t  ucIProtCntCur[12][8];		// 内部故障检测计时(当前)

/**********************************************************************************************************
**	函 数 名 : RunPer10ms()
**	功能说明 : 该函数每隔10ms被Systick中断调用1次
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static uint8_t Vbus_Cnt = 0;

void RunPer10ms(void)
{	
		Vbus_Cnt++;
	
    for(int i = 0; i < CHANNEL_USE; i++)
    {
        if(Mod_DIR_Flag[i] == BUCK_MODE)
        {
            Tier2.sampleCal.Vmod[i] = (float)((Tier2.calibrate[i].VmodK_C * ADCx_Vmod[i]) / 4096) + Tier2.calibrate[i].VmodB_C;
        }
        else if(Mod_DIR_Flag[i] == BOOST_MODE)
        {
            Tier2.sampleCal.Vmod[i] = (float)((Tier2.calibrate[i].VmodK_D * ADCx_Vmod[i]) / 4096) + Tier2.calibrate[i].VmodB_D;
        }
//#ifdef V_MOD_SP
				if(((SystemStatus.Neilsbed == Neilsbed_Nofit) || ((Tier2.sampleCal.Vsense[i] < 0.010f) && (Tier2.sampleCal.Vsense[i] > -0.010f))) && (g_ucSysMode == SYS_NORMAL_MODE))
				{
					Tier2.sampleCal.Vmod[i] = Voltage_Value_Soft_Process(Tier2.sampleCal.Vmod[i]);
				}
//#endif	        
				
				if(Tier2.sampleCal.Vmod[i] > 6.6f)	//检测到Vmod电压过高，EN信号置高
				{
					Chg_Dis_Port_Cfg(i + 1, PORT_OUT_ENABLE);	//2022.03.08
				}
//        Tier2.sampleCal.Tmod[i] = (float)((Tier2.calibrateEx.TmodK * ADCx_Temp[i]) / 4096) + Tier2.calibrateEx.TmodB;	//模块温度

        /* debug模式使用 */
        Tier2.debug.inq1[i].VmodDebug = Tier2.sampleCal.Vmod[i];
        Tier2.debug.inq1[i].TmodDebug = Tier2.sampleCal.Tmod[i];
    }

		if(Vbus_Cnt >= 5)
		{
			Tier2.sampleCal.Vbus1 = (float)((Tier2.calibrateEx.Vbus1K * AD_Bus1) / 4096) + Tier2.calibrateEx.Vbus1B;
			Tier2.debug.inq2.Vbus1Debug = Tier2.sampleCal.Vbus1;
			Vbus_Cnt = 0;
		}
//    Tier2.sampleCal.Vbus2 = (float)((Tier2.calibrateEx.Vbus2K * AD_Bus2) / 4096) + Tier2.calibrateEx.Vbus2B;
//    Tier2.sampleCal.Vaux  = (float)((Tier2.calibrateEx.VauxK  * AD_Aux ) / 4096) + Tier2.calibrateEx.VauxB;
//    Tier2.sampleCal.Tamb  = (float)((Tier2.calibrateEx.TambK  * AD_Amb ) / 4096) + Tier2.calibrateEx.TambB;	//环境温度

//    Tier2.debug.inq2.Vbus2Debug = Tier2.sampleCal.Vbus2;
//    Tier2.debug.inq2.VauxDebug  = Tier2.sampleCal.Vaux;
    Tier2.debug.inq2.TambDebug  = Tier2.sampleCal.Tamb;
		
    for(int i = 0; i < CHANNEL_CFG; i++)
    {
        uint8_t ch_numb = CHANNEL_MAX / CHANNEL_CFG;	// 每个外部通道对应的内部通道数量

        Tier2.chRecord[i].Trun = (float)Tier2.workstepRunning[i].Trun;
        Tier2.chRecord[i].Vbat = Tier2.sampleCal.Vsense[ch_numb * i];
        Tier2.chRecord[i].Iout = 0; //数据监控界面数据
        Tier2.chRecord[i].Tmod = 0;
		
				if(ch_numb == 1)
				{
					Tier2.chRecord[i].Iout = Tier2.sampleCal.Iout[i];	
				}
				else if(ch_numb == 2)
				{
					Tier2.chRecord[i].Iout = Tier2.sampleCal.Iout[2 * i + 0] + Tier2.sampleCal.Iout[2 * i + 1];
				}
				else if(ch_numb == 4)
				{
					Tier2.chRecord[i].Iout = Tier2.sampleCal.Iout[4 * i + 0] + Tier2.sampleCal.Iout[4 * i + 1] + Tier2.sampleCal.Iout[4 * i + 2] + Tier2.sampleCal.Iout[4 * i + 3];
				}
				else if(ch_numb == 8)
				{
					Tier2.chRecord[i].Iout = Tier2.sampleCal.Iout[0] + Tier2.sampleCal.Iout[1] + Tier2.sampleCal.Iout[2] + Tier2.sampleCal.Iout[3] + Tier2.sampleCal.Iout[4] + Tier2.sampleCal.Iout[5] + Tier2.sampleCal.Iout[6] + Tier2.sampleCal.Iout[7];
				}
		
        for(int j = 0; j < ch_numb; j++)
        {
//            Tier2.chRecord[i].Iout += Tier2.sampleCal.Iout[ch_numb * i + j];
            Tier2.chRecord[i].Tmod = (Tier2.sampleCal.Tmod[ch_numb * i + j] > Tier2.chRecord[i].Tmod ) ? \
                                     Tier2.sampleCal.Tmod[ch_numb * i + j] : Tier2.chRecord[i].Tmod;
        }
        Tier2.chRecord[i].Vout = Tier2.sampleCal.Vmod[ch_numb * i];
        Tier2.chRecord[i].Vtz = Tier2.sampleCal.Vtz[ch_numb * i];

        if( ((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_START)) && \
						(Tier2.workstepRunning[i].StepType != SCMD_WORKSTEP_TYPE_STANDBY))
        {
//			Tier2.chRecord[i].Cbat += Tier2.chRecord[i].Iout * CBAT_PERIOD / 3600.0f;
//			Tier2.chRecord[i].Ebat += Tier2.chRecord[i].Vbat * Tier2.chRecord[i].Iout * EBAT_PERIOD / 3600.0f;			
					if(Tier2.chRecord[i].Iout > (0.0005f * WORK_CUR_MAX))
					{
						Tier2.chRecord[i].Cbat += Tier2.chRecord[i].Iout * CBAT_PERIOD / 3600.0f;
						Tier2.chRecord[i].Ebat += Tier2.chRecord[i].Vbat * Tier2.chRecord[i].Iout * EBAT_PERIOD / 3600.0f;			
					}
        }

				if(SysModeProtMaskFlag == 0)
				{
						/*单工步过容检测*/
						if((Tier2.gProt[i].Cov.isEnable & 0x80) == 0)
						{
								if(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
								{
				//					if(Tier2.record[i].Cbat > (Tier2.gProt[i].Cov.value))
										if(Tier2.chRecord[i].Cbat > Tier2.gProt[i].Cov.value)
										{
												BatteryFormattingStop(i);
												GP_Alarm_Flag[i] |= GlobalProtect_OC;
										}			
								}
						}
				}			
		
        if(Tier2.chRecord[i].Iout >= 5.0f)
        {
            Tier2.chRecord[i].Zcont = fabs(Tier2.chRecord[i].Vtz - Tier2.chRecord[i].Vbat) / Tier2.chRecord[i].Iout;
            Tier2.chRecord[i].Zloop = fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vout) / Tier2.chRecord[i].Iout;
        }
        else
        {
            Tier2.chRecord[i].Zcont = 0;
            Tier2.chRecord[i].Zloop = 0;
        }
    }
		
	  if(SysModeProtMaskFlag == 0)	//故障屏蔽标识无效
    {
			for(uint8_t i = 0; i < CHANNEL_CFG; i++)
			{
					//-------------------------------3.BUS欠压检测-------------------------------//
					/* 3.1 更新前次故障检测计时 */
					ucIProtCntPre[2][i] = ucIProtCntCur[2][i];	

					/* 3.2 故障检测逻辑 */
					if((Tier2.iProt[i].VbusThrL.isEnable & 0x80) == 0)
					{
						if(bus_under_vol_flg == 1)
						{				
							if(Tier2.sampleCal.Vbus1 < Tier2.iProt[i].VbusThrL.value)		//bus1对应前一半通道
							{
								ucIProtCntCur[2][i]++;
								if(ucIProtCntCur[2][i] >= PROT_TRIG_MS(100, 10))
								{
									for(uint8_t j = 0; j < CHANNEL_CFG; j++)
									{
										IP_Alarm_Flag[j] |= IntelProtect_BUV;
									}
									ucIProtCntCur[2][i] = 0;
								}
							}
						}
					}
					/* 3.3 判断故障是否持续发生 */
					if(ucIProtCntPre[2][i] == ucIProtCntCur[2][i])
					{
						ucIProtCntPre[2][i] = 0;
						ucIProtCntCur[2][i] = 0;
					}			
       }
    } 
}

/**********************************************************************************************************
**	函 数 名 : RunPer25ms()
**	功能说明 : 该函数每隔10ms被Systick中断调用1次
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void RunPer25ms(void)
{
//	ADC_Start_Stop_DMA();
}

/**********************************************************************************************************
**	函 数 名 : RunPer100ms()
**	功能说明 : 该函数每隔10ms被Systick中断调用1次
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void RunPer100ms(void)
{

}

/**********************************************************************************************************
**	函 数 名 : RunPer500ms()
**	功能说明 : 该函数每隔10ms被Systick中断调用1次
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void RunPer500ms(void)
{

}

uint16_t fan_pwm = 0;
/**********************************************************************************************************
**  函 数 名 : RunPer1000ms()
**  功能说明 : 该函数每隔1000ms被Systick中断调用1次
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void RunPer1000ms(void)
{
//    float temp = 0;
	
//    for(int i = 0; i < 8; i++)
//    {
//        temp = (temp > Tier2.sampleCal.Iout[i]) ? temp : Tier2.sampleCal.Iout[i];
//    }
//#ifdef YCJ
//    temp = ((70 + temp) > 100) ? 100 : (40 + temp * 2);
//#else
//    temp = ((20 + temp) > 100) ? 100 : (20 + temp);
//#endif
//    if((SelfTest_flag == 0) && (ACDC_UpdateFlg == 0)) //自检结束，且非升级模式，控制风扇
//    {
//#ifdef YCJ
//		if(ACDC_UpdateFlg == 0)       //升级模式下，不发风扇调速
//		{
//        	ACDC_Fan_Speed_CMD(ACDC.Type, ACDC.Type - 1, (uint16_t)temp);
//		}
//#else
//		fan_pwm = (uint16_t)temp;
//		
//		if(ACDC_UpdateFlg == 0)
//		{
//				if((ACDC.Type == 1) || (ACDC.Type == 11)) //国电
//				{
//						ACDC_Fan_Speed_CMD(1, 0, fan_pwm); //模块0风扇设置到20%
//						ACDC_Fan_Speed_CMD(1, 1, fan_pwm); //模块2风扇设置到20%
//				}
//				else if((ACDC.Type == 2) || (ACDC.Type == 22)) //普德
//				{
//					ACDC_Fan_Speed_CMD(2, 1, fan_pwm); //模块1风扇设置到20%
//						ACDC_Fan_Speed_CMD(2, 2, fan_pwm); //模块2风扇设置到20%
//				}
//		}
//#endif
//    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
