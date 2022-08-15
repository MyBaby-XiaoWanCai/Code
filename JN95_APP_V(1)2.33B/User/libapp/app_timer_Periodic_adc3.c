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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
int64_t Tstamp = 0;
extern uint8_t SelfTest_flag;
extern uint32_t GP_Alarm_Flag[8];       //ȫ�ֱ�����־λ
extern __IO uint8_t ACDC_UpdateFlg;
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
float Voltage_Value_Soft_Process(float vol_value);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� :  timer_Periodic_Adc3()
**  ����˵�� :
**  ��    �� :  ��
**  �� �� ֵ :  ��
**********************************************************************************************************/
void timer_Periodic_Adc3(void *argument)
{
//    ADC_Start_Stop_DMA();
}

/**********************************************************************************************************
**  �� �� �� :
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
void ExternalAdDatProcess(void)
{
    static uint16_t ins_cnt = 0;
    static uint16_t sum_cnt = 0;

    for(int i = 0; i < CHANNEL_MAX; i++)
    {
        Tier2.sampleCal.VsenseIns[i] += ad7175_value[i];
        Tier2.sampleCal.IoutIns[i] += ad7175_value[i + 8];
        Tier2.sampleCal.VtzIns[i] += ad7124_value[i * 2];

        Tier2.sampleCal.VsenseSum[i] += ad7175_value[i];
        Tier2.sampleCal.VmodSum[i] += ADCx_Vmod[i];
        Tier2.sampleCal.IoutSum[i] += ad7175_value[i + 8];
        Tier2.sampleCal.VtzSum[i] += ad7124_value[i * 2];
    }
    ins_cnt++;
    sum_cnt++;

    if(ins_cnt >= INS_SAMPLE_NUM)
    {
        for(int i = 0; i < CHANNEL_MAX; i++)
        {
#if AD_uinpolar     //������ADC���ݴ���
			if(Mod_DIR_Flag[i] == BUCK_MODE)
            {
                Tier2.sampleCal.Vsense[i] = (float)((Tier2.calibrate[i].VsenseK_C * (Tier2.sampleCal.VsenseIns[i] / ins_cnt)) / 16777216) + Tier2.calibrate[i].VsenseB_C;
                Tier2.sampleCal.Iout[i]   = (float)((Tier2.calibrate[i].IhK_C     * (Tier2.sampleCal.IoutIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].IhB_C;
				Tier2.sampleCal.Vtz[i]    = (float)((Tier2.calibrate[i].VtzK_C    * (Tier2.sampleCal.VtzIns[i]    / ins_cnt)) / 16777216) + Tier2.calibrate[i].VtzB_C;
            }
            else if(Mod_DIR_Flag[i] == BOOST_MODE)
			{
				Tier2.sampleCal.Vsense[i] = (float)((Tier2.calibrate[i].VsenseK_D * (Tier2.sampleCal.VsenseIns[i] / ins_cnt)) / 16777216) + Tier2.calibrate[i].VsenseB_D;
				Tier2.sampleCal.Iout[i]   = (float)((Tier2.calibrate[i].IhK_D     * (Tier2.sampleCal.IoutIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].IhB_D;
				Tier2.sampleCal.Vtz[i]    = (float)((Tier2.calibrate[i].VtzK_C    * (Tier2.sampleCal.VtzIns[i]    / ins_cnt)) / 16777216) + Tier2.calibrate[i].VtzB_C;
			}
#else       //˫����ADC���ݴ���
			if(Mod_DIR_Flag[i] == BUCK_MODE)
            {

                Tier2.sampleCal.Vsense[i] = (float)((Tier2.calibrate[i].VsenseK_C * 2 * (Tier2.sampleCal.VsenseIns[i] / ins_cnt)) / 16777216) + Tier2.calibrate[i].VsenseB_C - Tier2.calibrate[i].VsenseK_C;
                Tier2.sampleCal.Iout[i]   = (float)((Tier2.calibrate[i].IhK_C * 2 * (Tier2.sampleCal.IoutIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].IhB_C - Tier2.calibrate[i].IhK_C;
                Tier2.sampleCal.Vtz[i]   = (float)((Tier2.calibrate[i].VtzK_C * 2 * (Tier2.sampleCal.VtzIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].VtzB_C - Tier2.calibrate[i].VtzK_C;
            }
            else if(Mod_DIR_Flag[i] == BOOST_MODE)
			{
				Tier2.sampleCal.Vsense[i] = (float)((Tier2.calibrate[i].VsenseK_D * 2 * (Tier2.sampleCal.VsenseIns[i] / ins_cnt)) / 16777216) + Tier2.calibrate[i].VsenseB_D - Tier2.calibrate[i].VsenseK_D;
				Tier2.sampleCal.Iout[i]   = (float)((Tier2.calibrate[i].IhK_D * 2 * (Tier2.sampleCal.IoutIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].IhB_D - Tier2.calibrate[i].IhK_D;
				Tier2.sampleCal.Vtz[i]   = (float)((Tier2.calibrate[i].VtzK_C * 2 * (Tier2.sampleCal.VtzIns[i]   / ins_cnt)) / 16777216) + Tier2.calibrate[i].VtzB_C - Tier2.calibrate[i].VtzK_C;
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

#ifdef V_TZ_SP
			if(((SystemStatus.Neilsbed == Neilsbed_Nofit) || ((Tier2.sampleCal.Vsense[i] < 0.010f) && (Tier2.sampleCal.Vsense[i] > -0.010f))) && (g_ucSysMode == SYS_NORMAL_MODE))
			{
				Tier2.sampleCal.Vtz[i] = Voltage_Value_Soft_Process(Tier2.sampleCal.Vtz[i]);
			}
#endif					
			
            /* debugģʽʹ�� */
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

    if(sum_cnt >= SUM_SAMPLE_NUM)
    {
        for(int i = 0; i < CHANNEL_MAX; i++)
        {
#if AD_uinpolar   //������ADC�������            
            Tier2.adSum[i].VsenseAdSum = Tier2.sampleCal.VsenseSum[i] / SUM_SAMPLE_NUM;
            Tier2.adSum[i].VmodAdSum   = Tier2.sampleCal.VmodSum[i] / SUM_SAMPLE_NUM;
            Tier2.adSum[i].IoutAdSum   = Tier2.sampleCal.IoutSum[i] / SUM_SAMPLE_NUM;
            Tier2.adSum[i].VtzAdSum   = Tier2.sampleCal.VtzSum[i] / SUM_SAMPLE_NUM;
#else        //7124��7175˫�������ģʽ
            if(Tier2.sampleCal.VsenseSum[i] > (0x7FFFFF * SUM_SAMPLE_NUM))
            {
                Tier2.adSum[i].VsenseAdSum = 2 * (Tier2.sampleCal.VsenseSum[i] - (0x7FFFFF * SUM_SAMPLE_NUM)) / SUM_SAMPLE_NUM;
            }
            else
            {
                Tier2.adSum[i].VsenseAdSum = 0;
            }
			
            Tier2.adSum[i].VmodAdSum   = Tier2.sampleCal.VmodSum[i] / SUM_SAMPLE_NUM;
			
            if(Tier2.sampleCal.IoutSum[i] > (0x7FFFFF * SUM_SAMPLE_NUM))
            {
                Tier2.adSum[i].IoutAdSum = 2 * (Tier2.sampleCal.IoutSum[i] - (0x7FFFFF * SUM_SAMPLE_NUM)) / SUM_SAMPLE_NUM;
            }
            else
            {
				Tier2.adSum[i].IoutAdSum = 0;
			}

            if(Tier2.sampleCal.VtzSum[i] > (0x7FFFFF * SUM_SAMPLE_NUM))
            {
                Tier2.adSum[i].VtzAdSum = 2 * (Tier2.sampleCal.VtzSum[i] - (0x7FFFFF * SUM_SAMPLE_NUM)) / SUM_SAMPLE_NUM;
            }
            else
            {
                Tier2.adSum[i].VtzAdSum = 0;
            }
#endif
            Tier2.sampleCal.VsenseSum[i] = 0;
            Tier2.sampleCal.VmodSum[i] = 0;
            Tier2.sampleCal.IoutSum[i] = 0;
            Tier2.sampleCal.VtzSum[i] = 0;
        }
        sum_cnt = 0;
    }
}


/**********************************************************************************************************
**  �� �� �� : RunPer1ms()
**  ����˵�� : �ú���ÿ��1ms��Systick�жϵ���1��
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void RunPer1ms(void)
{
    /* do nothing */
}

#define CBAT_PERIOD     0.01f
#define EBAT_PERIOD     0.01f




float Voltage_Value_Soft_Process(float vol_value)
{
	float ret_val = 0.0f;
	
	if(vol_value < -0.200f)
	{
		ret_val = vol_value;
	}
	else if(vol_value < -0.100f)
	{
		ret_val = 2 * (vol_value + 0.100f);
	}
	else if(vol_value < 0.100f)
	{
		if(vol_value < -0.060f)
		{
			ret_val = vol_value + 0.080f;
		}
		else if(vol_value < -0.020f)
		{
			ret_val = vol_value + 0.040f;
		}
		else if (vol_value < 0.020f)
		{
			ret_val = vol_value;
		}
		else if(vol_value < 0.060f)
		{
			ret_val = vol_value - 0.040f;
		}
		else
		{
			ret_val = vol_value - 0.080f;
		}
		
		if(ret_val < -0.010f)
		{
			ret_val = -0.020f - ret_val;
		}
		else if(ret_val < 0.010f)
		{
			ret_val = ret_val;
		}
		else
		{
			ret_val = 0.020f - ret_val;
		}
	}
	else if(vol_value < 0.200f)
	{
		ret_val = 2 * (vol_value - 0.100f);
	}
	else
	{
		ret_val = vol_value;
	}
		
	return ret_val;
}

//float Voltage_Value_Soft_Process(float vol_value)
//{
//	float ret_val = 0.0f;
//	
//	if(vol_value < -0.100f)			//��-x , -100��=>��-x, -100��
//	{
//		ret_val = vol_value;
//	}
//	else if(vol_value < -0.050f)	//��-100, 8 ��
//	{
//		ret_val = 0.008f + (vol_value + 0.050f) * 2.16f;
//	}
//	else if(vol_value < -0.040f)	//��8 ,   -2��
//	{
//		ret_val = -0.002f - (vol_value + 0.040f);
//	}
//	else if(vol_value < -0.030f)	//��-2, 8   ��
//	{
//		ret_val = 0.008f + (vol_value + 0.030f);
//	}
//	else if(vol_value < -0.020f)	//��8 , -2  ��
//	{
//		ret_val = -0.002f - (vol_value + 0.020f);
//	}
//	else if(vol_value < -0.010f)	//��-2, 8   ��
//	{
//		ret_val = 0.008f + (vol_value + 0.010f);
//	}
//	else if(vol_value < 0.000f)		//��8 , -2  ��
//	{
//		ret_val = -0.002f - (vol_value + 0.000f);
//	}
//	else if(vol_value < 0.010f)		//��-2, 8   ��
//	{
//		ret_val = -0.002f + (vol_value - 0.000f);
//	}
//	else if(vol_value < 0.020f)		//��8 , -2  ��
//	{
//		ret_val = 0.008f - (vol_value - 0.010f);
//	}
//	else if(vol_value < 0.030f)		//��-2, 8   ��
//	{
//		ret_val = -0.002f + (vol_value - 0.020f);
//	}
//	else if(vol_value < 0.040f)		//��8 , -2  ��
//	{
//		ret_val = 0.008f - (vol_value - 0.030f);
//	}
//	else if(vol_value < 0.050f)		//��-2, 8   ��
//	{
//		ret_val = -0.002f + (vol_value - 0.040f);
//	}
//	else if(vol_value < 0.100f)		//��8 , 100 ��
//	{
//		ret_val = 0.008f + (vol_value - 0.050f) * 1.84f;
//	}
//	else
//	{
//		ret_val = vol_value;
//	}
//		
//	return ret_val;
//}

//uint8_t v_bus1_uv_cnt[8] = {0};
//uint8_t v_bus2_uv_cnt[8] = {0};
uint8_t v_bus1_uv_cnt = 0;
uint8_t v_bus2_uv_cnt = 0;

extern uint16_t bus_under_vol_flg;
extern uint32_t IP_Alarm_Flag[8];       //�ڲ�������־λ

/**********************************************************************************************************
**  �� �� �� : RunPer10ms()
**  ����˵�� : �ú���ÿ��10ms��Systick�жϵ���1��
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void RunPer10ms(void)
{
    for(int i = 0; i < CHANNEL_MAX; i++)
    {
        if(Mod_DIR_Flag[i] == BUCK_MODE)
        {
            Tier2.sampleCal.Vmod[i] = (float)((Tier2.calibrate[i].VmodK_C * ADCx_Vmod[i]) / 4096) + Tier2.calibrate[i].VmodB_C;
        }
        else if(Mod_DIR_Flag[i] == BOOST_MODE)
		{
			Tier2.sampleCal.Vmod[i] = (float)((Tier2.calibrate[i].VmodK_D * ADCx_Vmod[i]) / 4096) + Tier2.calibrate[i].VmodB_D;
		}
		
#ifdef V_MOD_SP
		if(((SystemStatus.Neilsbed == Neilsbed_Nofit) || ((Tier2.sampleCal.Vsense[i] < 0.010f) && (Tier2.sampleCal.Vsense[i] > -0.010f))) && (g_ucSysMode == SYS_NORMAL_MODE))
		{
			Tier2.sampleCal.Vmod[i] = Voltage_Value_Soft_Process(Tier2.sampleCal.Vmod[i]);
		}
#endif		
		
        Tier2.sampleCal.Tmod[i] = (float)((Tier2.calibrateEx.TmodK * ADCx_Temp[i]) / 4096) + Tier2.calibrateEx.TmodB;

        /* debugģʽʹ�� */
        Tier2.debug.inq1[i].VmodDebug = Tier2.sampleCal.Vmod[i];
        Tier2.debug.inq1[i].TmodDebug = Tier2.sampleCal.Tmod[i];
    }

    Tier2.sampleCal.Vbus1 = (float)((Tier2.calibrateEx.Vbus1K * AD_Bus1) / 4096) + Tier2.calibrateEx.Vbus1B;
    Tier2.sampleCal.Vbus2 = (float)((Tier2.calibrateEx.Vbus2K * AD_Bus2) / 4096) + Tier2.calibrateEx.Vbus2B;
    Tier2.sampleCal.Vaux  = (float)((Tier2.calibrateEx.VauxK  * AD_Aux) / 4096) + Tier2.calibrateEx.VauxB;
    Tier2.sampleCal.Tamb  = (float)((Tier2.calibrateEx.TambK  * AD_Amb) / 4096) + Tier2.calibrateEx.TambB;

    Tier2.debug.inq2.Vbus1Debug = Tier2.sampleCal.Vbus1;
    Tier2.debug.inq2.Vbus2Debug = Tier2.sampleCal.Vbus2;
    Tier2.debug.inq2.VauxDebug  = Tier2.sampleCal.Vaux;
    Tier2.debug.inq2.TambDebug  = Tier2.sampleCal.Tamb;



////	for(uint8_t i = 0; i < CHANNEL_CFG; i++)
////	{	
////		/* BUSǷѹ���,bus1��Ӧǰ4ͨ��,bus2��Ӧ��4ͨ�� */
////		if((Tier2.iProt[i].VbusThrL.isEnable & 0x80) == 0)
////		{
////			if(bus_under_vol_flg == 1)
////			{
////				if(Tier2.sampleCal.Vbus1 < Tier2.iProt[i].VbusThrL.value)
////				{
////					v_bus1_uv_cnt[i]++;
////					
////					if(v_bus1_uv_cnt[i] >= 20)
////					{
////						for(uint8_t j = 0; j < (CHANNEL_CFG / 2); j++)
////						{
////							AuxRecordDatSync(BUV_ERR);
////							
////							if((IP_Alarm_Flag[j] & IntelProtect_FHE) == 0)		// �޷��ȹ���ʱ����BUSǷѹ
////							{
////								IP_Alarm_Flag[j] |= IntelProtect_BUV;
////							}
////							
////							v_bus1_uv_cnt[i] = 0;
////						}
////					}
////				}
////				else
////				{
////					v_bus1_uv_cnt[i] = 0;
////				}	
////				
////				if(Tier2.sampleCal.Vbus2 < Tier2.iProt[i].VbusThrL.value)
////				{
////					v_bus2_uv_cnt[i]++;
////					
////					if(v_bus2_uv_cnt[i] >= 20)
////					{
////						for(uint8_t j = (CHANNEL_CFG / 2); j < CHANNEL_CFG; j++)
////						{
////							AuxRecordDatSync(BUV_ERR);
////							
////							if((IP_Alarm_Flag[j] & IntelProtect_FHE) == 0)		// �޷��ȹ���ʱ����BUSǷѹ
////							{
////								IP_Alarm_Flag[j] |= IntelProtect_BUV;
////							}
////							
////							v_bus2_uv_cnt[i] = 0;
////						}
////					}
////				}
////				else
////				{
////					v_bus2_uv_cnt[i] = 0;
////				}
////			}					
////		}
////	}

    /* busǷѹ ��Ԥ���8��ͨ��1��bus,���ɶ���4ͨ��2��bus*/
#ifdef YCJ
    if((Tier2.iProt[0].VbusThrL.isEnable & 0x80) == 0)
    {
        if(bus_under_vol_flg == 1)
        {
            if(Tier2.sampleCal.Vbus1 < Tier2.iProt[0].VbusThrL.value)		//bus1��Ӧȫ��ͨ��
            {
                v_bus1_uv_cnt++;

                if(v_bus1_uv_cnt >= 20)                                    //����20��С���趨��Сֵ
                {
                    for(uint8_t j = 0; j < CHANNEL_CFG; j++)               //�澯��־��λ����¼����
                    {
                        AuxRecordDatSync(BUV_ERR);
						
						if((IP_Alarm_Flag[j] & IntelProtect_FHE) == 0)
						{
							IP_Alarm_Flag[j] |= IntelProtect_BUV;
						}
                    }
                    
                    v_bus1_uv_cnt=0;
                }
            }
            else
            {
                v_bus1_uv_cnt = 0;
            }
        }
    }
#else
    if((Tier2.iProt[0].VbusThrL.isEnable & 0x80) == 0)
    {
        if(bus_under_vol_flg == 1)
        {
            if(Tier2.sampleCal.Vbus1 < Tier2.iProt[0].VbusThrL.value)		//bus1��Ӧǰһ��ͨ��
            {
                v_bus1_uv_cnt++;

                if(v_bus1_uv_cnt >= 20)                                    //����20��С���趨��Сֵ
                {
                    for(uint8_t j = 0; j < (CHANNEL_CFG/2); j++)               //�澯��־��λ����¼����
                    {
                        AuxRecordDatSync(BUV_ERR);
	
						if((IP_Alarm_Flag[j] & IntelProtect_FHE) == 0)
						{
							IP_Alarm_Flag[j] |= IntelProtect_BUV;
						}
                    }
                    
                    v_bus1_uv_cnt=0;
                }
            }
            else
            {
                v_bus1_uv_cnt = 0;
            }
    
			if(Tier2.sampleCal.Vbus2 < Tier2.iProt[0].VbusThrL.value)		//bus2��Ӧ��һ��ͨ��
            {
                v_bus2_uv_cnt++;

                if(v_bus2_uv_cnt >= 20)                                    //����20��С���趨��Сֵ
                {
                    for(uint8_t j = (CHANNEL_CFG / 2); j < CHANNEL_CFG; j++)               //�澯��־��λ����¼����
                    {
                        AuxRecordDatSync(BUV_ERR);
						
						if((IP_Alarm_Flag[j] & IntelProtect_FHE) == 0)
						{
							IP_Alarm_Flag[j] |= IntelProtect_BUV;
						}
                    }
                    
                    v_bus2_uv_cnt=0;
                }
            }
            else
            {
                v_bus2_uv_cnt = 0;
            }
         
        }    
    }
#endif
	
	
    for(int i = 0; i < CHANNEL_CFG; i++)
    {
        uint8_t ch_numb = CHANNEL_MAX / CHANNEL_CFG;    // ÿ���ⲿͨ����Ӧ���ڲ�ͨ������

        Tier2.chRecord[i].Trun = (float)Tier2.workstepRunning[i].Trun;
        Tier2.chRecord[i].Vbat = Tier2.sampleCal.Vsense[ch_numb * i];
        Tier2.chRecord[i].Iout = 0;
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
//          Tier2.chRecord[i].Iout += Tier2.sampleCal.Iout[ch_numb * i + j];
            Tier2.chRecord[i].Tmod = (Tier2.sampleCal.Tmod[ch_numb * i + j] > Tier2.chRecord[i].Tmod) ? \
                                     Tier2.sampleCal.Tmod[ch_numb * i + j] : Tier2.chRecord[i].Tmod;
        }
#ifdef YCJ
		Tier2.chRecord[i].Vout = Tier2.sampleCal.Vmod[ch_numb * i];
#else		
		if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
		    ((Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CC) || \
			 (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CV) || \
			 (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CCTOCV)) )
		{
			if(Tier2.sampleCal.Vmod[ch_numb * i] > Tier2.sampleCal.Vmod[ch_numb * i + 1])
			{
				Tier2.chRecord[i].Vout = Tier2.sampleCal.Vmod[ch_numb * i];
			}
			else
			{
				Tier2.chRecord[i].Vout = Tier2.sampleCal.Vmod[ch_numb * i + 1];
			}
		}
		else if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
				 ((Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCC) || \
				  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCV) || \
				  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV)) )
		{
			if(Tier2.sampleCal.Vmod[ch_numb * i] < Tier2.sampleCal.Vmod[ch_numb * i + 1])
			{
				Tier2.chRecord[i].Vout = Tier2.sampleCal.Vmod[ch_numb * i];
			}
			else
			{
				Tier2.chRecord[i].Vout = Tier2.sampleCal.Vmod[ch_numb * i + 1];
			}		
		}
		else
		{
			Tier2.chRecord[i].Vout = Tier2.sampleCal.Vmod[ch_numb * i];
		}
#endif
        Tier2.chRecord[i].Vtz = Tier2.sampleCal.Vtz[ch_numb * i];

        if( ((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_START)) && \
			(Tier2.workstepRunning[i].StepType != SCMD_WORKSTEP_TYPE_STANDBY))
        {
            if(Tier2.chRecord[i].Iout > (0.0005f * WORK_CUR_MAX))
            {
                Tier2.chRecord[i].Cbat += Tier2.chRecord[i].Iout * CBAT_PERIOD / 3600.0f;
                Tier2.chRecord[i].Ebat += Tier2.chRecord[i].Vbat * Tier2.chRecord[i].Iout * EBAT_PERIOD / 3600.0f;
            }
        }

        if(SysModeProtMaskFlag == 0)
        {
            if((Tier2.gProt[i].Cov.isEnable & 0x80) == 0)
            {
                if(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
                {
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
}

/**********************************************************************************************************
**  �� �� �� : RunPer25ms()
**  ����˵�� : �ú���ÿ��10ms��Systick�жϵ���1��
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void RunPer25ms(void)
{
    //  ADC_Start_Stop_DMA();
	
	
}

/**********************************************************************************************************
**  �� �� �� : RunPer100ms()
**  ����˵�� : �ú���ÿ��10ms��Systick�жϵ���1��
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void RunPer100ms(void)
{

}

/**********************************************************************************************************
**  �� �� �� : RunPer500ms()
**  ����˵�� : �ú���ÿ��10ms��Systick�жϵ���1��
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void RunPer500ms(void)
{

}

uint16_t fan_pwm = 0;

/**********************************************************************************************************
**  �� �� �� : RunPer1000ms()
**  ����˵�� : �ú���ÿ��10ms��Systick�жϵ���1��
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void RunPer1000ms(void)
{
    float temp = 0;
    //    static uint8_t cnt=1;
    for(int i = 0; i < 8; i++)
    {
        temp = (temp > Tier2.sampleCal.Iout[i]) ? temp : Tier2.sampleCal.Iout[i];
    }
#ifdef YCJ
    temp = ((70 + temp) > 100) ? 100 : (40 + temp * 2);
#else
    temp = ((20 + temp) > 100) ? 100 : (20 + temp);
#endif
   
	if((SelfTest_flag == 0) && (ACDC_UpdateFlg == 0)) //�Լ�������ҷ�����ģʽ�����Ʒ���
    {
#ifdef YCJ
        if(ACDC_UpdateFlg == 0)       //����ģʽ�£��������ȵ���
        {
            ACDC_Fan_Speed_CMD(ACDC.Type, ACDC.Type - 1, (uint16_t)temp);
        }
#else
		fan_pwm = (uint16_t)temp;
		
        if(ACDC_UpdateFlg == 0)
        {			
            if((ACDC.Type == 1) || (ACDC.Type == 11)) //����
            {
                ACDC_Fan_Speed_CMD(1, 0, fan_pwm); //ģ��0�������õ�20%
                ACDC_Fan_Speed_CMD(1, 1, fan_pwm); //ģ��2�������õ�20%
            }
            else if((ACDC.Type == 2) || (ACDC.Type == 22)) //�յ�
			{
				ACDC_Fan_Speed_CMD(2, 1, fan_pwm); //ģ��1�������õ�20%
				ACDC_Fan_Speed_CMD(2, 2, fan_pwm); //ģ��2�������õ�20%
			}
        }
#endif
        //        if(ACDC_Reply == 1)   //�յ��ظ�
        //        {
        //            ACDC_Reply = 0;  //����ظ���־
        //            cnt--;           //������1
        //        }
        //        else
        //        {
        //            cnt++;           //δ�յ��ظ���������1
        //        }
        //        if(cnt == 0)        //��֤����ֵ����С��0
        //        {
        //        cnt = 1;
        //        }
        //        if(cnt == 10)       //����10��δ�յ��ظ���can1�澯
        //        {
        //            cnt = 0;
        //            for(uint8_t j = 0; j < CHANNEL_CFG; j++)
        //            { IP_Alarm_Flag[j] |= IntelProtect_C1E; } //can1����
    }
}


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
