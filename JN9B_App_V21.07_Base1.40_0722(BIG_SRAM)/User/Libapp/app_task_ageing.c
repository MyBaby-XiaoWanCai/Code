/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_task_ageing.c
** Author      : Cai Heng
** Date        : 2021-08-10
** Description :
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
#ifdef YCJ
const uint8_t i_set[] = {10, 20, 60, 110};
const uint8_t i_normal[] = {8, 12, 18, 22, 57, 63, 106, 114};
#else
const uint8_t i_set[] = {2, 10, 50, 78};
const uint8_t i_normal[] = {1, 3, 9, 11, 48, 52, 76, 80};
#endif

uint8_t  g_ucSysMode = SYS_NORMAL_MODE;
uint8_t  ChannelErrorFlag = 0;
uint8_t  ChannelStopFlag = 0;
uint8_t  ChannelError[8] = {0};

uint8_t sendMsg[256] = {0};
uint16_t module_err_flag[8] = {0};
uint8_t  ChannelErrorFlag_Store = 0;
uint8_t  ChannelError_Store[8] = {0};
float bus1_data = 0;
float bus2_data = 0;
float current_record_data[8] = {0};
float voltage_record_data[8] = {0};

uint8_t ageing_cnt_flag = 0;
uint32_t ageing_cnt = 0;

extern uint8_t flag_of_intelprot;
extern uint8_t test_flag;

extern uint8_t FCT_ReadBuf[64];
uint8_t ageing_run_flag = AGEING_ENABLE;
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void Chg_Dis_On_Cfg(uint8_t ch, uint8_t dir, uint8_t vmod, float vol, float cur);
void Chg_Dis_Off_Cfg(uint8_t ch);
void Protect_Trigger_Judgemet(void);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : Chg_Dis_On_Cfg()
**	功能说明 : 打开充放电配置
**	形    参 : ch   - 通道
**			   		 dir  - 方向
**             vmod - 标志
**			   		 vol  - 电压
**             cur  - 电流
**********************************************************************************************************/
void Chg_Dis_On_Cfg(uint8_t ch, uint8_t dir, uint8_t vmod, float vol, float cur)
{
    uint16_t v_dac[1] = {0};
    uint16_t i_dac[8] = {0};
    float v_k = 0.0f;
    float v_b = 0.0f;
    float i_k = 0.0f;
    float i_b = 0.0f;
		int32_t v_temp = 0;
		int32_t i_temp = 0;
	
    for(int j = 0; j < CHANNEL_USE; j++)
    {
		uint8_t led_tmp = 0;
		if(CHANNEL_USE == 8)
			led_tmp = j;
		else
			led_tmp = 2 * j;		
		
        if(ch & (1 << j))
        {
            if(dir == BUCK_MODE)
            {
                if(Mod_DIR_Flag[j] != BUCK_MODE)
                {
                    Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_OUT_DISABLE);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_DIR_BUCK);
                }
                i_k = Tier2.calibrate[j].IhSetK_C;
                i_b = Tier2.calibrate[j].IhSetB_C;

                if(vmod == V_MOD)
                {
                    if(Mod_V_Range_Flag[j] != V_MOD)
                    {
                        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_RANGE_MOD);
                    }
                    v_k = Tier2.calibrate[j].VmodSetK_C;
                    v_b = Tier2.calibrate[j].VmodSetB_C;
                }
                else if(vmod == V_SENSE)
                {
                    if(Mod_V_Range_Flag[j] != V_SENSE)
                    {
                        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_RANGE_SENSE);
                    }
                    v_k = Tier2.calibrate[j].VsenseSetK_C;
                    v_b = Tier2.calibrate[j].VsenseSetB_C;
                }
#ifdef YCJ
                g_ucLedChSta[led_tmp + 1] = CH_CHARGE;
#else                
				g_ucLedChSta[led_tmp / 2 + 1] = CH_CHARGE;
#endif
            }
            else if(dir == BOOST_MODE)
            {
                if(Mod_DIR_Flag[j] != BOOST_MODE)
                {
                    Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_OUT_DISABLE);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_DIR_BOOST);
                }
                i_k = Tier2.calibrate[j].IhSetK_D;
                i_b = Tier2.calibrate[j].IhSetB_D;

                if(vmod == V_MOD)
                {
                    if(Mod_V_Range_Flag[j] != V_MOD)
                    {
                        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_RANGE_MOD);
                    }
                    v_k = Tier2.calibrate[j].VmodSetK_D;
                    v_b = Tier2.calibrate[j].VmodSetB_D;
                }
                else if(vmod == V_SENSE)
                {
                    if(Mod_V_Range_Flag[j] != V_SENSE)
                    {
                        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_RANGE_SENSE);
                    }
                    v_k = Tier2.calibrate[j].VsenseSetK_D;
                    v_b = Tier2.calibrate[j].VsenseSetB_D;
                }
#ifdef YCJ
                g_ucLedChSta[led_tmp + 1] = CH_DISCHARGE;
#else                
				g_ucLedChSta[led_tmp / 2 + 1] = CH_DISCHARGE;
#endif
            }
			v_temp = (int32_t)(vol * v_k + v_b);
			i_temp = (int32_t)(cur * i_k + i_b);
			
			if(v_temp < 0)
			{
				v_temp = 0;
			}
			else if(v_temp > 65535)
			{
				v_temp = 65535;
			}			
			
			if(i_temp < 0)
			{
				i_temp = 0;
			}
			else if(i_temp > 65535)
			{
				i_temp = 65535;
			}
			
			v_dac[0] = (uint16_t)v_temp;
			i_dac[j] = (uint16_t)i_temp;

			DAC80508_SetDacData(j, v_dac[0], 0);
        }
    }

    for(int j = 0; j < CHANNEL_USE; j++)
    {
        if(ch & (1 << j))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_REV_ENABLE);
        }
    }

    osDelay(10);

    for(int j = 0; j < CHANNEL_USE; j++)
    {
        if(ch & (1 << j))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_OUT_ENABLE);
        }
    }

    osDelay(10);

    for(int j = 0; j < CHANNEL_USE; j++)
    {
        if(ch & (1 << j))
        {
            DAC_SetLinearData(j, i_dac[j], 1);
        }
    }
}

/**********************************************************************************************************
**	函 数 名 : Chg_Dis_Off_Cfg()
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void Chg_Dis_Off_Cfg(uint8_t ch)
{
    for(int j = 0; j < CHANNEL_USE; j++)
    {
        if(ch & (1 << j))
        {
            DAC_SetLinearData(j, 0, 1);
        }
    }
    osDelay(5);

    for(int j = 0; j < CHANNEL_USE; j++)
    {
        if(ch & (1 << j))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_OUT_DISABLE);
        }
    }
    osDelay(5);

    for(int j = 0; j < CHANNEL_USE; j++)
    {
        if(ch & (1 << j))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_REV_DISABLE);
        }
    }
    for(int j = 0; j < CHANNEL_USE; j++)
    {
			if(ch & (1 << j))
			{
				DAC80508_SetDacData(j, 0, 0);
			}
    }
	
    for(int j = 0; j < CHANNEL_USE; j++)
    {
			uint8_t led_tmp = 0;
			if(CHANNEL_USE == 8)
				led_tmp = j;
			else
				led_tmp = 2 * j;			
			
			if(ChannelError[j] == NO_ERR)
			{
				g_ucLedChSta[led_tmp + 1] = CH_STANDBY;
			}
			else
			{
				g_ucLedChSta[led_tmp + 1] = CH_ERR_HARDWARE;
			}
    }	
}

uint8_t speed_break_cnt[8] = {0};

/**********************************************************************************************************
**	函 数 名 : Protect_Trigger_Judgemet()
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void Protect_Trigger_Judgemet(void)
{
    for(int j = 0; j < CHANNEL_USE; j++)
    {
#ifdef YCJ        
		if((Tier2.sampleCal.Vmod[j] >= 6.5f) || (Tier2.sampleCal.Iout[j] > 125.0f))
#else 
        if((Tier2.sampleCal.Vmod[j] >= 6.5f) || (Tier2.sampleCal.Iout[j] > 95.0f))
#endif
        {
            if(speed_break_cnt[j] > 5)
            {
                speed_break_cnt[j] = 0;

                ChannelError[j]  |=  SPEED_BREAK_ERR;
								ChannelErrorFlag |= SPEED_BREAK_ERR;
								ChannelStopFlag = 1;
            }
						else
						{
							speed_break_cnt[j]++;
						}
        }
        else
        {
            speed_break_cnt[j] = 0;
        }
		
				if((IP_Alarm_Flag[j] & IntelProtect_HPA) != 0)		// 硬件故障
				{
					ChannelError[2 * j + 0] |=  SPEED_BREAK_ERR;
					ChannelError[2 * j + 1] |=  SPEED_BREAK_ERR;
					ChannelErrorFlag |= SPEED_BREAK_ERR;
					ChannelStopFlag = 1;
				}		
    }

	if(ChannelStopFlag != 0)
	{
		for(int j = 0; j < CHANNEL_MAX; j++)
		{
			speed_break_cnt[j] = 0;
			if(ChannelError[j] != 0)
			{
//       Chg_Dis_Off_Cfg(1 << j);	//20220213
			}
		}
		
		ChannelStopFlag = 0;
		
//		Chg_Dis_Off_Cfg(0x0F);

//		Chg_Dis_Off_Cfg(0xF0);
	}
}

/**********************************************************************************************************
**	函 数 名 : SelfChecking()
**	功能说明 : 自检
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void SelfChecking(void)
{
	int chg_num;
	int dis_num;
	g_ucSysMode = SYS_SELFCHECK_MODE;
	
    for(int j = 0; j < CHANNEL_USE; j++)
    {
        /* 设置端口电压模式 */
        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_RANGE_MOD);
        /* 设置主机工作模式 */
        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_MODE_MASTER);
    }
    
    for(int j = 0; j < CHANNEL_USE; j++)
    {
        if(ChannelErrorFlag == NO_ERR)
        {
#ifdef NEW_ORDER	//0C1D、2C3D、4C5D、6C7D、0D1C、2D3C、4D5C、6D7C
			chg_num = (j < (CHANNEL_USE / 2)) ? (j * 2 + 0) : ((j - (CHANNEL_USE / 2)) * 2 + 1);
			dis_num = (j < (CHANNEL_USE / 2)) ? (j * 2 + 1) : ((j - (CHANNEL_USE / 2)) * 2 + 0);
#else				//0C4D、1C5D、2C6D、3C7D、4C0D、5C1D、6C2D、7C3D
            chg_num = j;
            dis_num = (j < (CHANNEL_USE / 2)) ? (j + (CHANNEL_USE / 2)) : (j - (CHANNEL_USE / 2));
#endif

#ifdef YCJ
            Chg_Dis_On_Cfg(1 << dis_num, BOOST_MODE, V_MOD, 1.0, 120.0);    
#else
            Chg_Dis_On_Cfg(1 << dis_num, BOOST_MODE, V_MOD, 1.0, 80.0);
#endif
            for(int k = 0; k < 4; k++)
            {
                if(ChannelErrorFlag == NO_ERR)
                {
                    Chg_Dis_On_Cfg(1 << chg_num, BUCK_MODE, V_MOD, 5.0, (float)i_set[k]);

                    int TimeCnt = 0;

                    while(++TimeCnt < (5 * SECOND_TIME_SCALE))
                    {
                        osDelay(DELAY_TIME_SCALE);
                    }
#ifdef YCJ
					if( (((Tier2.sampleCal.Vmod[chg_num] > 0.5f)            && (Tier2.sampleCal.Vmod[chg_num] < 2.0f))                && \
					     ((Tier2.sampleCal.Vmod[dis_num] > 0.5f)            && (Tier2.sampleCal.Vmod[dis_num] < 1.5f))                && \
					     ((Tier2.sampleCal.Iout[chg_num] > i_normal[2 * k]) && (Tier2.sampleCal.Iout[chg_num] < i_normal[2 * k + 1])) && \
					     ((Tier2.sampleCal.Iout[dis_num] > i_normal[2 * k]) && (Tier2.sampleCal.Iout[dis_num] < i_normal[2 * k + 1]))) == 0)
#else
                    if( (((Tier2.sampleCal.Vmod[chg_num] > 0.5f)            && (Tier2.sampleCal.Vmod[chg_num] < 2.0f))                && \
                         ((Tier2.sampleCal.Vmod[dis_num] > 0.5f)            && (Tier2.sampleCal.Vmod[dis_num] < 1.5f))                && \
                         ((Tier2.sampleCal.Iout[chg_num] > i_normal[2 * k]) && (Tier2.sampleCal.Iout[chg_num] < i_normal[2 * k + 1])) && \
                         ((Tier2.sampleCal.Iout[dis_num] > i_normal[2 * k]) && (Tier2.sampleCal.Iout[dis_num] < i_normal[2 * k + 1]))) == 0)
#endif                    
                    {
                        ChannelError[chg_num] |= SELF_CHECKING_ERR;
												ChannelError[dis_num] |= SELF_CHECKING_ERR;
												ChannelErrorFlag      |= SELF_CHECKING_ERR;
												ChannelStopFlag = 1;
                    }
                }
                else
                {
                    break;
                }
            }

            //先关闭充电回路避免电压充高引起故障
            Chg_Dis_Off_Cfg(1 << chg_num);
			
            Chg_Dis_Off_Cfg(1 << dis_num);
        }
        else
        {
            break;
        }
    }
}

/**********************************************************************************************************
**	函 数 名 : Ageing()
**	功能说明 : 老化
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void Ageing(void)
{
	uint8_t Arlam_1 = 0x00, Arlam_2 = 0x00, Arlam_3 = 0x00, Arlam_4 = 0x00;
	uint8_t Age_Res_Buf[17] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
														 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
	int32_t ls, us;
	Dis_int();
	ls = osKernelLock(); 
	
	ee_WriteBytesWithCheck(&Age_Res_Buf[0], AGE_RES_ADDR_1, 17);	//清除上一次老化结果
	
	us = osKernelUnlock();
	osKernelRestoreLock(us);
	osKernelRestoreLock(ls);
	En_int();											 
														 
	g_ucSysMode = SYS_AGEING_MODE;	//设置老化模式

#ifdef YCJ   
	for(int i = 0; i < CHANNEL_USE; i = i + 1)
	{
		Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_RANGE_MOD);
		/* 设置主机工作模式 */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_MODE_MASTER);
	}    
#else
	for(int i = 0; i < CHANNEL_USE; i = i + 2)
	{
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 0], PORT_RANGE_MOD);
		/* 设置主机工作模式 */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 0], PORT_MODE_MASTER);
		
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 1], PORT_RANGE_MOD);
		/* 设置从机工作模式 */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 1], PORT_MODE_SLAVE);
	}
#endif
    
#ifdef	YCJ

	uint8_t chg_tmp = 0;	//充电通道
	uint8_t dis_tmp = 0;	//放电通道
	
	#ifdef NEW_ORDER	
		for(int i = 0; i < CHANNEL_USE; i = i + 2)
		{
			chg_tmp |= 1 << (i + 0);
			dis_tmp |= 1 << (i + 1);
		}	
	#else
		for(int i = 0; i < CHANNEL_USE; i = i + 1)
		{
			if(i < (CHANNEL_USE / 2))
			{
				chg_tmp |= 1 << i;
			}
			else
			{
				dis_tmp |= 1 << i;
			}
		}
	#endif
	
	if(CHANNEL_CFG == 0x01)
	{
		Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 10.0);
		osDelay(1000);

		Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 50.0);
		osDelay(1000);
		
		Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 80.0);
		osDelay(1000);
		
		Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 75.0);		
		osDelay(5000);
	}	
	else
	{		
		Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 10.0);
		osDelay(1000);

		Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 60.0);
		osDelay(1000);
		
		Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 125.0);
		osDelay(1000);
		
		Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 120.0);		
		osDelay(5000);
	}
	
#else

	uint8_t chg_tmp = 0;
	uint8_t dis_tmp = 0;

	#ifdef NEW_ORDER
		for(int i = 0; i < CHANNEL_USE; i = i + 2)
		{
			chg_tmp |= 1 << (i + 0);
			dis_tmp |= 1 << (i + 1);
		}		
	#else
		for(int i = 0; i < CHANNEL_USE; i = i + 1)
		{
			if(i < (CHANNEL_USE / 2))
			{
				chg_tmp |= 1 << i;
			}
			else
			{
				dis_tmp |= 1 << i;
			}
		}		
	#endif
	
	Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 50.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 60.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 85.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 80.0);	
		
#endif

	ageing_cnt_flag = 1;

	ageing_cnt = 0;

	while(ageing_cnt < AGEING_TIME_CFG * SECOND_TIME_SCALE)
	{
		if(ChannelErrorFlag == NO_ERR)
		{
			if(ageing_cnt > (5 * SECOND_TIME_SCALE))
			{
#ifdef NEW_ORDER
				for(int i = 0; i < CHANNEL_USE; i = i + 2)
#else				
				for(int i = 0; i < (CHANNEL_USE / 2); i = i + 1)
#endif
				{
					if(ChannelErrorFlag == NO_ERR)
					{
#ifdef NEW_ORDER						
						int chg_num = i + 0;
						int dis_num = i + 1;
#else
						int chg_num = i + 0;
						int dis_num = i + 4;
#endif						
						if(CHANNEL_CFG == 0x01)
						{			
							if( (((Tier2.sampleCal.Vmod[chg_num] > 4.0f) && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))   && \
								 ((Tier2.sampleCal.Vmod[dis_num] > 4.0f) && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))   && \
								 ((Tier2.sampleCal.Iout[chg_num] > 70.0f) && (Tier2.sampleCal.Iout[chg_num] < 80.0f)) && \
								 ((Tier2.sampleCal.Iout[dis_num] > 70.0f) && (Tier2.sampleCal.Iout[dis_num] < 80.0f))) == 0)                       
							{
								ChannelError[chg_num] |= AGEING_ERR;
								ChannelError[dis_num] |= AGEING_ERR;
								ChannelErrorFlag      |= AGEING_ERR;
								ChannelStopFlag = 1;
							}						
						}
						else
						{
#ifdef YCJ							
						if( (((Tier2.sampleCal.Vmod[chg_num] > 4.0f) && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))   && \
							 ((Tier2.sampleCal.Vmod[dis_num] > 4.0f) && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))   && \
							 ((Tier2.sampleCal.Iout[chg_num] > 115.0f) && (Tier2.sampleCal.Iout[chg_num] < 125.0f)) && \
							 ((Tier2.sampleCal.Iout[dis_num] > 115.0f) && (Tier2.sampleCal.Iout[dis_num] < 125.0f))) == 0) 
#else						
						if( (((Tier2.sampleCal.Vmod[chg_num] > 3.7f)  && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))  && \
							 ((Tier2.sampleCal.Vmod[dis_num] > 3.7f)  && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
							 ((Tier2.sampleCal.Iout[chg_num] > 75.0f) && (Tier2.sampleCal.Iout[chg_num] < 85.0f)) && \
							 ((Tier2.sampleCal.Iout[dis_num] > 75.0f) && (Tier2.sampleCal.Iout[dis_num] < 85.0f))) == 0) 
#endif                        
						{
							ChannelError[chg_num] |= AGEING_ERR;
							ChannelError[dis_num] |= AGEING_ERR;
							ChannelErrorFlag      |= AGEING_ERR;
							ChannelStopFlag = 1;
						}
					}
					}
					else
					{						
						break;
					}
				}
			}
		}
		else
		{
			break;
		}
		
		osDelay(DELAY_TIME_SCALE);
	}
	
	ageing_cnt_flag = 0;
	
	/***  奇充偶放 老化结果检测   ***/
	for(uint8_t fault_ch = 0; fault_ch < CHANNEL_USE; fault_ch++)	
	{
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_NEG)	//反接保护
		{
			Arlam_1 = 0x01;
			Age_Res_Buf[fault_ch + 1] += Arlam_1;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OVP)	//硬件OVP
		{
			Arlam_2 = 0x02;
			Age_Res_Buf[fault_ch + 1] += Arlam_2;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OCP)	//硬件OCP
		{
			Arlam_3 = 0x04;
			Age_Res_Buf[fault_ch + 1] += Arlam_3;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OTP)	//硬件OTP
		{
			Arlam_4 = 0x08;
			Age_Res_Buf[fault_ch + 1] += Arlam_4;
		}
		else
		{
			Age_Res_Buf[fault_ch + 1] = 0x00;
		}
	}
	
	Chg_Dis_Off_Cfg(0x0F);
	
	Chg_Dis_Off_Cfg(0xF0);

/************************************************************************************************/
	
#ifdef	YCJ
	
	chg_tmp = 0;
	dis_tmp = 0;	
	
	#ifdef NEW_ORDER	
		for(int i = 0; i < CHANNEL_USE; i = i + 2)
		{
			dis_tmp |= 1 << (i + 0);
			chg_tmp |= 1 << (i + 1);
		}	
	#else
		for(int i = 0; i < CHANNEL_USE; i = i + 1)
		{
			if(i < (CHANNEL_USE / 2))
			{
				dis_tmp |= 1 << i;
			}
			else
			{
				chg_tmp |= 1 << i;
			}
		}
	#endif

		if(CHANNEL_CFG == 0x01)
		{
			Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 10.0);
			osDelay(1000);

			Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 50.0);
			osDelay(1000);
			
			Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 80.0);
			osDelay(1000);
			
			Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 75.0);		
			osDelay(5000);
		}	
		else
		{	
			Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 10.0);
			osDelay(1000);
			
			Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 60.0);
			osDelay(1000);

			Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 125.0);
			osDelay(1000);
			
			Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 120.0);
			osDelay(5000);
		}
		
#else
	
	chg_tmp = 0;
	dis_tmp = 0;

	#ifdef NEW_ORDER
		for(int i = 0; i < CHANNEL_USE; i = i + 2)
		{
			dis_tmp |= 1 << (i + 0);
			chg_tmp |= 1 << (i + 1);
		}		
	#else
		for(int i = 0; i < CHANNEL_USE; i = i + 1)
		{
			if(i < (CHANNEL_USE / 2))
			{
				dis_tmp |= 1 << i;
			}
			else
			{
				chg_tmp |= 1 << i;
			}
		}		
	#endif
	
	Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 50.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 60.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(chg_tmp, BUCK_MODE, V_MOD, 4.5, 85.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(dis_tmp, BOOST_MODE, V_MOD, 1.0, 80.0);
	
#endif    
	
	ageing_cnt_flag = 1;
	
	ageing_cnt = 0;	
	
	while(ageing_cnt < AGEING_TIME_CFG * SECOND_TIME_SCALE)
	{
		if(ChannelErrorFlag == NO_ERR)
		{
			if(ageing_cnt > (5 * SECOND_TIME_SCALE))
			{
#ifdef NEW_ORDER
				for(int i = 0; i < CHANNEL_USE; i = i + 2)
#else				
				for(int i = 0; i < (CHANNEL_USE / 2); i = i + 1)
#endif								
				{
					if(ChannelErrorFlag == NO_ERR)
					{
#ifdef NEW_ORDER						
						int dis_num = i + 0;
						int chg_num = i + 1;
#else
						int dis_num = i + 0;
						int chg_num = i + 4;
#endif							
						if(CHANNEL_CFG == 0x01)
						{			
							if( (((Tier2.sampleCal.Vmod[chg_num] > 4.0f) && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))   && \
								 ((Tier2.sampleCal.Vmod[dis_num] > 4.0f) && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))   && \
								 ((Tier2.sampleCal.Iout[chg_num] > 70.0f) && (Tier2.sampleCal.Iout[chg_num] < 80.0f)) && \
								 ((Tier2.sampleCal.Iout[dis_num] > 70.0f) && (Tier2.sampleCal.Iout[dis_num] < 80.0f))) == 0)                       
							{
								ChannelError[chg_num] |= AGEING_ERR;
								ChannelError[dis_num] |= AGEING_ERR;
								ChannelErrorFlag      |= AGEING_ERR;
								ChannelStopFlag = 1;
							}						
						}
						else	
						{							
#ifdef YCJ                        
							if( (((Tier2.sampleCal.Vmod[chg_num] > 4.0f) && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))   && \
								 ((Tier2.sampleCal.Vmod[dis_num] > 4.0f) && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))   && \
								 ((Tier2.sampleCal.Iout[chg_num] > 115.0f) && (Tier2.sampleCal.Iout[chg_num] < 125.0f)) && \
								 ((Tier2.sampleCal.Iout[dis_num] > 115.0f) && (Tier2.sampleCal.Iout[dis_num] < 125.0f))) == 0) 
	#else						
							if( (((Tier2.sampleCal.Vmod[chg_num] > 3.7f)  && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))  && \
								 ((Tier2.sampleCal.Vmod[dis_num] > 3.7f)  && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
								 ((Tier2.sampleCal.Iout[chg_num] > 75.0f) && (Tier2.sampleCal.Iout[chg_num] < 85.0f)) && \
								 ((Tier2.sampleCal.Iout[dis_num] > 75.0f) && (Tier2.sampleCal.Iout[dis_num] < 85.0f))) == 0) 
	#endif                        
							{
								ChannelError[chg_num] |= AGEING_ERR;
								ChannelError[dis_num] |= AGEING_ERR;
								ChannelErrorFlag      |= AGEING_ERR;	
								ChannelStopFlag = 1;							
							}
						}
					}
					else
					{
						break;
					}
				}
			}
		}
		else
		{			
			break;
		}
		
		osDelay(DELAY_TIME_SCALE);
	}
	
	ageing_cnt_flag = 0;
	
	/***  偶充奇放 老化结果检测   ***/
	for(uint8_t fault_ch = 0; fault_ch < CHANNEL_USE; fault_ch++)	
	{
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_NEG)	//反接保护
		{
			Arlam_1 = 0x01;
			Age_Res_Buf[fault_ch + 9] += Arlam_1;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OVP)	//硬件OVP
		{
			Arlam_2 = 0x02;
			Age_Res_Buf[fault_ch + 9] += Arlam_2;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OCP)	//硬件OCP
		{
			Arlam_3 = 0x04;
			Age_Res_Buf[fault_ch + 9] += Arlam_3;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OTP)	//硬件OTP
		{
			Arlam_4 = 0x08;
			Age_Res_Buf[fault_ch + 9] += Arlam_4;
		}
		else
		{
			Age_Res_Buf[fault_ch + 9] = 0x00;
		}
	}
	
	Chg_Dis_Off_Cfg(chg_tmp);
	
	Chg_Dis_Off_Cfg(dis_tmp);
	
/***************************************** 老化结束 标志写入 **************************************************/	
	
	uint8_t Res_Flag = 0,Res_Buf[2] = {0x00};
	for(uint8_t res_ch = 1; res_ch <= (CHANNEL_USE * 2); res_ch++)
	{
		if(Age_Res_Buf[res_ch] == 0)
		{
			Res_Flag++;
		}
	}
	
	if(Res_Flag == (CHANNEL_USE * 2))
	{
		Age_Res_Buf[0] = 0x00; //若无通道异常，则向地址B-1写0
	}
	else	//否则将地址B和B+1的值或之后写如地址B-1
	{
		ee_ReadBytes(&Res_Buf[0], AGE_RES_ADDR, 1);
		ee_ReadBytes(&Res_Buf[1], AGE_RES_ADDR + 1, 1);
		Age_Res_Buf[0] = Res_Buf[0] + Res_Buf[1];
	}	
	
	Dis_int();
	ls = osKernelLock(); 
	
	ee_WriteBytesWithCheck(&Age_Res_Buf[0], AGE_RES_ADDR_1, 17); 
	
	us = osKernelUnlock();
	osKernelRestoreLock(us);
	osKernelRestoreLock(ls);
	En_int();
	
	ageing_run_flag = AGEING_DISABLE;
}

/**********************************************************************************************************
**	函 数 名 : AppTaskAgeing()
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void AppTaskAgeing(void *argument)
{	
	ageing_run_flag = AGEING_ENABLE;	//老化模式使能
	
	while(1)
	{
		IP_Scanning();  //无前面板测试时屏蔽
		
		if(test_flag == 0)
		{
			if(flag_of_intelprot == 0)
			{
				if((W5500_IP_ADDRESS[2] == 8) && (W5500_IP_ADDRESS[3] == 42))
				{	
					ee_ReadBytes(&FCT_ReadBuf[0], FCT_EE_ADDR_A_1, 1);	//FCT结果读取
					
					g_ucSysMode = SYS_CALIBRATION_MODE;
					
//					if(Vaux_set != 7.2f)
					{
						Vaux_set = 7.2f;
						
						AD5245_Vpro_Set(Bovp, Vaux_set);
					}

					if(FCT_ReadBuf[0] == 0x00 && ageing_run_flag == AGEING_ENABLE)	//FCT合格自动进入老化模式
//					if(ageing_run_flag == AGEING_ENABLE)
					{
						if(CHANNEL_CFG == 0x04)
						{
							Ageing_Weaver_240A();	//2并机 240A老化
						}
						else
						{
							Ageing();	//单机 - 120A老化
						}
					}
				}
				else if((W5500_IP_ADDRESS[2] == 8) && (W5500_IP_ADDRESS[3] == 41))
				{					
					if(ageing_run_flag == AGEING_ENABLE)
					{
						ageing_run_flag = AGEING_DISABLE;

						g_ucSysMode = SYS_CALIBRATION_MODE;
						
//						if(Vaux_set != 7.2f)
						{
							Vaux_set = 7.2f;
							
							AD5245_Vpro_Set(Bovp, Vaux_set);
						}
						
						WholeMachine_FCT();	//进入FCT模式
					}
				}
			}
		}
		osDelay(AGEING_TASK_PERIOD);
	}
}

/**********************************************************************************************************
**	函 数 名 : Ageing_Weaver_240A()
**	功能说明 : 240A机型老化函数
**						 CH1&CH2充 CH3&CH4放 CH5&CH6充 CH7&CH8放
**						 CH1&CH2放 CH3&CH4充 CH5&CH6放 CH7&CH8充
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void Ageing_Weaver_240A(void)
{
	uint8_t Arlam_1 = 0x00, Arlam_2 = 0x00, Arlam_3 = 0x00, Arlam_4 = 0x00;
	uint8_t Age_Res_Buf[17] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
														 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

	if (CHANNEL_CFG == 0x04)	//2通道并机
	{
		GPIO_SetPins(GPIO_PORT_CVK_4,GPIO_PIN_CVK_4);
		GPIO_SetPins(GPIO_PORT_CVK_8,GPIO_PIN_CVK_8);
		
		for(uint8_t MS_j = 0;MS_j < 8;MS_j++)
		{
			if(MS_j%2 == 0)
				Chg_Dis_Port_Cfg(Channel_Port_Table[MS_j], PORT_MODE_MASTER);	//奇数通道设置为主机模式
			else if(MS_j%2 == 1)
				Chg_Dis_Port_Cfg(Channel_Port_Table[MS_j], PORT_MODE_SLAVE);	//偶数通道设置为从机模式
		}
	}	
														 
	int32_t ls, us;
	Dis_int();
	ls = osKernelLock(); 
	
	ee_WriteBytesWithCheck(&Age_Res_Buf[0], AGE_RES_ADDR_1, 17);	//清除上一次老化结果
	
	us = osKernelUnlock();
	osKernelRestoreLock(us);
	osKernelRestoreLock(ls);
	En_int();											 
														 
	g_ucSysMode = SYS_AGEING_MODE;	//设置老化模式
		
	Chg_Dis_On_Cfg(0xCC, BOOST_MODE, V_MOD, 1.0, 10.0);	//34 78放电														 
	osDelay(1000);

	Chg_Dis_On_Cfg(0x33, BUCK_MODE, V_MOD, 4.5, 60.0); //12 56充电
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x33, BUCK_MODE, V_MOD, 4.5, 125.0);
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xCC, BOOST_MODE, V_MOD, 1.0, 120.0);		
	osDelay(5000);

	ageing_cnt_flag = 1;

	ageing_cnt = 0;

	while(ageing_cnt < AGEING_TIME_CFG * SECOND_TIME_SCALE)
	{
		if(ChannelErrorFlag == NO_ERR)
		{
			if(ageing_cnt > (5 * SECOND_TIME_SCALE))
			{
#ifdef NEW_ORDER
				for(int i = 0; i < CHANNEL_USE; i = i + 2)
#else				
				for(int i = 0; i < (CHANNEL_USE / 2); i = i + 1)
#endif
				{
					if(ChannelErrorFlag == NO_ERR)
					{
#ifdef NEW_ORDER						
						int chg_num = i + 0;
						int dis_num = i + 1;
#else
						int chg_num = i + 0;
						int dis_num = i + 4;
#endif						
						
#ifdef YCJ
						if( (((Tier2.sampleCal.Vmod[chg_num] > 4.0f) && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))   && \
							 ((Tier2.sampleCal.Vmod[dis_num] > 4.0f) && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))   && \
							 ((Tier2.sampleCal.Iout[chg_num] > 110.0f) && (Tier2.sampleCal.Iout[chg_num] < 130.0f)) && \
							 ((Tier2.sampleCal.Iout[dis_num] > 110.0f) && (Tier2.sampleCal.Iout[dis_num] < 130.0f))) == 0) 
#else						
						if( (((Tier2.sampleCal.Vmod[chg_num] > 3.7f)  && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))  && \
							 ((Tier2.sampleCal.Vmod[dis_num] > 3.7f)  && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
							 ((Tier2.sampleCal.Iout[chg_num] > 75.0f) && (Tier2.sampleCal.Iout[chg_num] < 85.0f)) && \
							 ((Tier2.sampleCal.Iout[dis_num] > 75.0f) && (Tier2.sampleCal.Iout[dis_num] < 85.0f))) == 0) 
#endif                        
						{
							ChannelError[chg_num] |= AGEING_ERR;
							ChannelError[dis_num] |= AGEING_ERR;
							ChannelErrorFlag      |= AGEING_ERR;
							ChannelStopFlag = 1;
						}
					}
					else
					{						
						break;
					}
				}
			}
		}
		else
		{
			break;
		}
		
		osDelay(DELAY_TIME_SCALE);
	}
	
	ageing_cnt_flag = 0;
	
	/***  奇充偶放 老化结果检测   ***/
	for(uint8_t fault_ch = 0; fault_ch < CHANNEL_USE; fault_ch++)	
	{
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_NEG)	//反接保护
		{
			Arlam_1 = 0x01;
			Age_Res_Buf[fault_ch + 1] += Arlam_1;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OVP)	//硬件OVP
		{
			Arlam_2 = 0x02;
			Age_Res_Buf[fault_ch + 1] += Arlam_2;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OCP)	//硬件OCP
		{
			Arlam_3 = 0x04;
			Age_Res_Buf[fault_ch + 1] += Arlam_3;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OTP)	//硬件OTP
		{
			Arlam_4 = 0x08;
			Age_Res_Buf[fault_ch + 1] += Arlam_4;
		}
		else
		{
			Age_Res_Buf[fault_ch + 1] = 0x00;
		}
	}

	Chg_Dis_Off_Cfg(0xCC);	//先关放电通道
	osDelay(300);	
	Chg_Dis_Off_Cfg(0x33);
	osDelay(200);

/************************************************************************************************/
		
	Chg_Dis_On_Cfg(0x33, BOOST_MODE, V_MOD, 1.0, 10.0);	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xCC, BUCK_MODE, V_MOD, 4.5, 60.0);	
	osDelay(1000);

	Chg_Dis_On_Cfg(0xCC, BUCK_MODE, V_MOD, 4.5, 125.0);	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x33, BOOST_MODE, V_MOD, 1.0, 120.0);
	osDelay(5000);   
	
	ageing_cnt_flag = 1;
	
	ageing_cnt = 0;	
	
	while(ageing_cnt < AGEING_TIME_CFG * SECOND_TIME_SCALE)
	{
		if(ChannelErrorFlag == NO_ERR)
		{
			if(ageing_cnt > (5 * SECOND_TIME_SCALE))
			{
#ifdef NEW_ORDER
				for(int i = 0; i < CHANNEL_USE; i = i + 2)
#else				
				for(int i = 0; i < (CHANNEL_USE / 2); i = i + 1)
#endif								
				{
					if(ChannelErrorFlag == NO_ERR)
					{
#ifdef NEW_ORDER						
						int dis_num = i + 0;
						int chg_num = i + 1;
#else
						int dis_num = i + 0;
						int chg_num = i + 4;
#endif							
						
#ifdef YCJ                        
						if( (((Tier2.sampleCal.Vmod[chg_num] > 4.0f) && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))   && \
							 ((Tier2.sampleCal.Vmod[dis_num] > 4.0f) && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))   && \
							 ((Tier2.sampleCal.Iout[chg_num] > 110.0f) && (Tier2.sampleCal.Iout[chg_num] < 130.0f)) && \
							 ((Tier2.sampleCal.Iout[dis_num] > 110.0f) && (Tier2.sampleCal.Iout[dis_num] < 130.0f))) == 0) 
#else						
						if( (((Tier2.sampleCal.Vmod[chg_num] > 3.7f)  && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))  && \
							 ((Tier2.sampleCal.Vmod[dis_num] > 3.7f)  && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
							 ((Tier2.sampleCal.Iout[chg_num] > 75.0f) && (Tier2.sampleCal.Iout[chg_num] < 85.0f)) && \
							 ((Tier2.sampleCal.Iout[dis_num] > 75.0f) && (Tier2.sampleCal.Iout[dis_num] < 85.0f))) == 0) 
#endif                        
						{
							ChannelError[chg_num] |= AGEING_ERR;
							ChannelError[dis_num] |= AGEING_ERR;
							ChannelErrorFlag      |= AGEING_ERR;	
							ChannelStopFlag = 1;							
						}
					}
					else
					{
						break;
					}
				}
			}
		}
		else
		{			
			break;
		}
		
		osDelay(DELAY_TIME_SCALE);
	}
	
	ageing_cnt_flag = 0;
	
	/***  偶充奇放 老化结果检测   ***/
	for(uint8_t fault_ch = 0; fault_ch < CHANNEL_USE; fault_ch++)	
	{
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_NEG)	//反接保护
		{
			Arlam_1 = 0x01;
			Age_Res_Buf[fault_ch + 9] += Arlam_1;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OVP)	//硬件OVP
		{
			Arlam_2 = 0x02;
			Age_Res_Buf[fault_ch + 9] += Arlam_2;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OCP)	//硬件OCP
		{
			Arlam_3 = 0x04;
			Age_Res_Buf[fault_ch + 9] += Arlam_3;
		}
		else 
		if(IP_Alarm_Flag[fault_ch] & IntelProtect_OTP)	//硬件OTP
		{
			Arlam_4 = 0x08;
			Age_Res_Buf[fault_ch + 9] += Arlam_4;
		}
		else
		{
			Age_Res_Buf[fault_ch + 9] = 0x00;
		}
	}

	Chg_Dis_Off_Cfg(0x33);	//先关放电通道
	osDelay(300);	
	Chg_Dis_Off_Cfg(0xCC);
	osDelay(200);
	
/***************************************** 老化结束 标志写入 **************************************************/	
	
	uint8_t Res_Flag = 0,Res_Buf[2] = {0x00};
	for(uint8_t res_ch = 1; res_ch <= (CHANNEL_USE * 2); res_ch++)
	{
		if(Age_Res_Buf[res_ch] == 0)
		{
			Res_Flag++;
		}
	}
	
	if(Res_Flag == (CHANNEL_USE * 2))
	{
		Age_Res_Buf[0] = 0x00; //若无通道异常，则向地址B-1写0
	}
	else	//否则将地址B和B+1的值或之后写如地址B-1
	{
		ee_ReadBytes(&Res_Buf[0], AGE_RES_ADDR, 1);
		ee_ReadBytes(&Res_Buf[1], AGE_RES_ADDR + 1, 1);
		Age_Res_Buf[0] = Res_Buf[0] + Res_Buf[1];
	}	
	
	Dis_int();
	ls = osKernelLock(); 
	
	ee_WriteBytesWithCheck(&Age_Res_Buf[0], AGE_RES_ADDR_1, 17); 
	
	us = osKernelUnlock();
	osKernelRestoreLock(us);
	osKernelRestoreLock(ls);
	En_int();
	
	ageing_run_flag = AGEING_DISABLE;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
