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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
#ifdef YCJ
const uint8_t i_set[] = {2, 10, 20, 28};
const uint8_t i_normal[] = {1, 3, 9, 11, 18, 22, 26, 30};
#else
const uint8_t i_set[] = {2, 10, 50, 78};                  //�ϻ���������ֵ
const uint8_t i_normal[] = {1, 3, 9, 11, 48, 52, 76, 80}; //����У׼����ֵ
#endif

uint8_t  g_ucSysMode = SYS_NORMAL_MODE;     //ϵͳģʽ
uint8_t  ChannelErrorFlag = 0;              //ͨ�����ϱ�־
uint8_t  ChannelStopFlag = 0;               //ͨ��ֹͣ��־
uint8_t  ChannelError[8] = {0};             //ͨ������洢

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void Chg_Dis_On_Cfg(uint8_t ch, uint8_t dir, uint8_t vmod, float vol, float cur);
void Chg_Dis_Off_Cfg(uint8_t ch);
void Protect_Trigger_Judgemet(void);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : Chg_Dis_On_Cfg()
**	����˵�� : ��ŵ翪������
**	��    �� : ch - ͨ����dir - ����vmod - ģʽ��vol - ��ѹ��cur - ����
**	�� �� ֵ : ��
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
	
    for(int j = 0; j < CHANNEL_MAX; j++)
    {
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
                g_ucLedChSta[j + 1] = CH_CHARGE;
#else                
				g_ucLedChSta[j / 2 + 1] = CH_CHARGE;
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
                g_ucLedChSta[j + 1] = CH_DISCHARGE;
#else                
				g_ucLedChSta[j / 2 + 1] = CH_DISCHARGE;
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

    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        if(ch & (1 << j))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_REV_ENABLE);
        }
    }

    osDelay(10);

    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        if(ch & (1 << j))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_OUT_ENABLE);
        }
    }

    osDelay(10);

    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        if(ch & (1 << j))
        {
            DAC_SetLinearData(j, i_dac[j], 1);
        }
    }
}

/**********************************************************************************************************
**	�� �� �� : Chg_Dis_Off_Cfg()
**	����˵�� : ��ŵ�ر�����
**	��    �� : ch - ͨ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void Chg_Dis_Off_Cfg(uint8_t ch)
{
    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        if(ch & (1 << j))
        {
            DAC_SetLinearData(j, 0, 1);
        }
    }
    osDelay(5);

    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        if(ch & (1 << j))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_OUT_DISABLE);
        }
    }
    osDelay(5);

    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        if(ch & (1 << j))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_REV_DISABLE);
        }
    }
    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        if(ch & (1 << j))
        {
            DAC80508_SetDacData(j, 0, 0);
        }
    }
	
    for(int j = 0; j < CHANNEL_CFG; j++)
    {
		if((ChannelError[2 * j + 0] == NO_ERR) && (ChannelError[2 * j + 1] == NO_ERR))
		{
			g_ucLedChSta[j + 1] = CH_STANDBY;
		}
		else
		{
			g_ucLedChSta[j + 1] = CH_ERR_HARDWARE;
		}
    }	
}

uint8_t speed_break_cnt[8] = {0};

/**********************************************************************************************************
**	�� �� �� : Protect_Trigger_Judgemet()
**	����˵�� : �����жϴ����ж�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void Protect_Trigger_Judgemet(void)
{
   
    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        
#ifdef YCJ        
		if((Tier2.sampleCal.Vmod[j] >= 6.5f) || (Tier2.sampleCal.Iout[j] > 35.0f))
#else 
        if((Tier2.sampleCal.Vmod[j] >= 6.5f) || (Tier2.sampleCal.Iout[j] > 95.0f))
#endif        
//		if((Tier2.sampleCal.Vmod[j] >= 5.2f) || (Tier2.sampleCal.Iout[j] > 82.0f))
        {
            if(speed_break_cnt[j] > 5)
            {
                speed_break_cnt[j] = 0;

                ChannelError[j] |=  SPEED_BREAK_ERR;
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
		
		if((IP_Alarm_Flag[j] & IntelProtect_HPA) != 0)		// Ӳ������
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
                Chg_Dis_Off_Cfg(1 << j);
            }
		}
		
		ChannelStopFlag = 0;
		
//		Chg_Dis_Off_Cfg(0x0F);

//		Chg_Dis_Off_Cfg(0xF0);
	}


}

/**********************************************************************************************************
**	�� �� �� : SelfChecking()
**	����˵�� : �Լ�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void SelfChecking(void)
{
	g_ucSysMode = SYS_SELFCHECK_MODE;
	
    for(int j = 0; j < 8; j++)
    {
        /* ���ö˿ڵ�ѹģʽ */
        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_RANGE_MOD);
        /* ������������ģʽ */
        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_MODE_MASTER);
    }
    
    for(int j = 0; j < 8; j++)
    {
        if(ChannelErrorFlag == NO_ERR)
        {
            int chg_num = j;
            int dis_num = (j < 4) ? (j + 4) : (j - 4);
#ifdef YCJ
            Chg_Dis_On_Cfg(1 << dis_num, BOOST_MODE, V_MOD, 1.0, 30.0);    
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
                       if(((Tier2.sampleCal.Vmod[chg_num] > 0.5f)      && (Tier2.sampleCal.Vmod[chg_num] < 2.0f))            && \
                        ((Tier2.sampleCal.Vmod[dis_num] > 0.5f)        && (Tier2.sampleCal.Vmod[dis_num] < 1.5f))            && \
                        ((Tier2.sampleCal.Iout[chg_num] > i_normal[k]) && (Tier2.sampleCal.Iout[chg_num] < i_normal[k + 1])) && \
                        ((Tier2.sampleCal.Iout[dis_num] > i_normal[k]) && (Tier2.sampleCal.Iout[dis_num] < i_normal[k + 1])) == 0)
#else    
                    
                    if( ((Tier2.sampleCal.Vmod[chg_num] > 0.5f)            && (Tier2.sampleCal.Vmod[chg_num] < 2.0f))                && \
                        ((Tier2.sampleCal.Vmod[dis_num] > 0.5f)            && (Tier2.sampleCal.Vmod[dis_num] < 1.5f))                && \
                        ((Tier2.sampleCal.Iout[chg_num] > i_normal[2 * k]) && (Tier2.sampleCal.Iout[chg_num] < i_normal[2 * k + 1])) && \
                        ((Tier2.sampleCal.Iout[dis_num] > i_normal[2 * k]) && (Tier2.sampleCal.Iout[dis_num] < i_normal[2 * k + 1])) == 0)
#endif                    
                    {
                        ChannelError[chg_num] |= SELF_CHECKING_ERR;
						ChannelError[dis_num] |= SELF_CHECKING_ERR;
						ChannelErrorFlag |= SELF_CHECKING_ERR;
						ChannelStopFlag = 1;
                    }
                }
                else
                {
                    break;
                }
            }

            //�ȹرճ���·�����ѹ����������
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
**	�� �� �� : Ageing()
**	����˵�� : �ϻ�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void Ageing(void)
{
	g_ucSysMode = SYS_AGEING_MODE;

#ifdef YCJ   
	for(int i = 0; i < 8; i++)
	{
		Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_RANGE_MOD);
		/* ������������ģʽ */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_MODE_MASTER);
	}    
#else
	for(int i = 0; i < 8; i = i + 2)
	{
		Chg_Dis_Port_Cfg(Channel_Port_Table[i    ], PORT_RANGE_MOD);
		/* ������������ģʽ */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 0], PORT_MODE_MASTER);
		
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 1], PORT_RANGE_MOD);
		/* ���ôӻ�����ģʽ */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 1], PORT_MODE_SLAVE);
	}
#endif
    
#ifdef	YCJ
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 10.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 20.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 32.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 30.0);    
#else	
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 50.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 60.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 85.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 80.0);
#endif	
	int TimeCnt = 0;
	
	while(++TimeCnt < AGEING_TIME_CFG * SECOND_TIME_SCALE)
	{
		if(ChannelErrorFlag == NO_ERR)
		{
			if(TimeCnt > (5 * SECOND_TIME_SCALE))
			{	
				for(int i = 0; i < 4; i ++)
				{
					if(ChannelErrorFlag == NO_ERR)
					{
						int chg_num = i;
						int dis_num = i + 4;
#ifdef YCJ
						if( ((Tier2.sampleCal.Vmod[chg_num] > 3.7f)  && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))  && \
							((Tier2.sampleCal.Vmod[dis_num] > 3.7f)  && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
							((Tier2.sampleCal.Iout[chg_num] > 25.0f) && (Tier2.sampleCal.Iout[chg_num] < 35.0f)) && \
							((Tier2.sampleCal.Iout[dis_num] > 25.0f) && (Tier2.sampleCal.Iout[dis_num] < 35.0f)) == 0) 
#else						
						if( ((Tier2.sampleCal.Vmod[chg_num] > 3.7f)  && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))  && \
							((Tier2.sampleCal.Vmod[dis_num] > 3.7f)  && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
							((Tier2.sampleCal.Iout[chg_num] > 75.0f) && (Tier2.sampleCal.Iout[chg_num] < 85.0f)) && \
							((Tier2.sampleCal.Iout[dis_num] > 75.0f) && (Tier2.sampleCal.Iout[dis_num] < 85.0f)) == 0) 
#endif                        
						{
							ChannelError[chg_num] |= AGEING_ERR;
							ChannelError[dis_num] |= AGEING_ERR;
							ChannelErrorFlag |= AGEING_ERR;
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
	
	Chg_Dis_Off_Cfg(0x0F);
	
	Chg_Dis_Off_Cfg(0xF0);

/************************************************************************************************/
#ifdef	YCJ
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 10.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 20.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 32.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 30.0);    
#else
	
	Chg_Dis_On_Cfg(0x0F, BOOST_MODE, V_MOD, 1.0, 50.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BUCK_MODE, V_MOD, 4.5, 60.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BUCK_MODE, V_MOD, 4.5, 85.0);
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BOOST_MODE, V_MOD, 1.0, 80.0);
#endif    
	
	TimeCnt = 0;
	
	while(++TimeCnt < AGEING_TIME_CFG * SECOND_TIME_SCALE)
	{
		if(ChannelErrorFlag == NO_ERR)
		{
			if(TimeCnt > (5 * SECOND_TIME_SCALE))
			{	
				for(int i = 0; i < 4; i ++)
				{
					if(ChannelErrorFlag == NO_ERR)
					{
						int chg_num = i + 4;
						int dis_num = i;
#ifdef YCJ                        
						if( ((Tier2.sampleCal.Vmod[chg_num] > 3.7f)  && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))  && \
							((Tier2.sampleCal.Vmod[dis_num] > 3.7f)  && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
							((Tier2.sampleCal.Iout[chg_num] > 25.0f) && (Tier2.sampleCal.Iout[chg_num] < 35.0f)) && \
							((Tier2.sampleCal.Iout[dis_num] > 25.0f) && (Tier2.sampleCal.Iout[dis_num] < 35.0f)) == 0) 
#else						
						if( ((Tier2.sampleCal.Vmod[chg_num] > 3.7f)  && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))  && \
							((Tier2.sampleCal.Vmod[dis_num] > 3.7f)  && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
							((Tier2.sampleCal.Iout[chg_num] > 75.0f) && (Tier2.sampleCal.Iout[chg_num] < 85.0f)) && \
							((Tier2.sampleCal.Iout[dis_num] > 75.0f) && (Tier2.sampleCal.Iout[dis_num] < 85.0f)) == 0) 
#endif                        
						{
							ChannelError[chg_num] |= AGEING_ERR;
							ChannelError[dis_num] |= AGEING_ERR;
							ChannelErrorFlag |= AGEING_ERR;	
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
	
	Chg_Dis_Off_Cfg(0x0F);
	
	Chg_Dis_Off_Cfg(0xF0);
}

extern uint8_t flag_of_intelprot;
extern uint8_t test_flag;

/**********************************************************************************************************
**	�� �� �� : AppTaskAgeing()
**	����˵�� : �ϻ������߳�
**	��    �� : *argument - ����
**	�� �� ֵ : ��
**********************************************************************************************************/
void AppTaskAgeing(void *argument)
{
	static uint8_t ageing_run_flag = AGEING_ENABLE;
	
	int test_printf_ageing = 0;
	
	while(1)
	{
		IP_Scanning();  //��ǰ������ʱ����
		
		if(test_flag == 0)
		{
			if(flag_of_intelprot == 0)
			{
				if((W5500_IP_ADDRESS[2] == AGEING_LIB_ADDR) && (W5500_IP_ADDRESS[3] == AGEING_BIT_ADDR))
				{
					if(ageing_run_flag == AGEING_ENABLE)
					{
						ageing_run_flag = AGEING_DISABLE;
						
						SelfChecking();
						
						Ageing();
					}
				}
			}
		}
		
		if((++test_printf_ageing % 10) == 0)
		{
			printf("Ageing task is running!");
		}
		
		osDelay(AGEING_TASK_PERIOD);
	}
}


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
