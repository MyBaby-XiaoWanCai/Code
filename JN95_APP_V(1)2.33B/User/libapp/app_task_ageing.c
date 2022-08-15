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
const uint8_t i_set[] = {2, 10, 50, 78};
const uint8_t i_normal[] = {1, 3, 9, 11, 48, 52, 76, 80};
#endif

uint8_t  g_ucSysMode = SYS_NORMAL_MODE;
uint8_t  ChannelErrorFlag = 0;
uint8_t  ChannelStopFlag = 0;
uint8_t  ChannelError[8] = {0};

uint8_t  ChannelErrorFlag_Store = 0;
uint8_t  ChannelError_Store[8] = {0};

uint8_t sendMsg[256] = {0};

extern uint8_t client_conFlg;

uint8_t ageing_flag = 0;		// 0 - δִ���ϻ�  1 - ��ִ���ϻ�


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
**	����˵�� :
**	��    �� : ch - ͨ����bit����(0b_0000_0000 - 0b_1111_1111)
**	�� �� ֵ :
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
            if(dir == BUCK_MODE)		// �趨���ģʽ
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
				g_ucLedChSta[j / 2 + 1] = CH_CHARGE;		// ����
#endif
			}
            else if(dir == BOOST_MODE)	// �趨�ŵ�ģʽ
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
				g_ucLedChSta[j / 2 + 1] = CH_DISCHARGE;		// �ŵ��
#endif
            }
			v_temp = (int32_t)(vol * v_k + v_b);	// �����趨��ѹDAֵ
			i_temp = (int32_t)(cur * i_k + i_b);	// �����趨����DAֵ
			
			// ��ѹDA�޷�
			if(v_temp < 0)
			{
				v_temp = 0;
			}
			else if(v_temp > 65535)			
			{
				v_temp = 65535;
			}			
			
			// ����DA�޷�
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
		
			// ��ѹDA����
            DAC80508_SetDacData(j, v_dac[0], 0);
        }
    }

    for(int j = 0; j < CHANNEL_MAX; j++)	// ����ʹ��
    {
        if(ch & (1 << j))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_REV_ENABLE);
        }
    }

    osDelay(10);

    for(int j = 0; j < CHANNEL_MAX; j++)	// ���ʹ��
    {
        if(ch & (1 << j))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_OUT_ENABLE);
        }
    }

    osDelay(10);

    for(int j = 0; j < CHANNEL_MAX; j++)	// ����DA����
    {
        if(ch & (1 << j))
        {
            DAC_SetLinearData(j, i_dac[j], 1);
        }
    }
}

/**********************************************************************************************************
**	�� �� �� : Chg_Dis_Off_Cfg()
**	����˵�� :
**	��    �� :
**	�� �� ֵ :
**********************************************************************************************************/
void Chg_Dis_Off_Cfg(uint8_t ch)
{
	if(ageing_flag == 1)
	{
		ageing_flag = 0;
		
		ChannelErrorFlag_Store = ChannelErrorFlag;
		
		for(int j = 0; j < 8; j++)
		{
			ChannelError_Store[j] = ChannelError[j];		
		}
	}
	
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



extern uint32_t IP_Alarm_Flag[8];       // �ڲ�������־λ

uint32_t IP_Alarm_Flag_Pre[8] = {0};	

uint16_t device_err_flag = 0;

uint16_t module_err_flag[8] = {0};

uint8_t speed_break_cnt[8] = {0};		// ����ٶϼ���

uint8_t selfcheck_err_cnt[8] = {0};		// �Լ��쳣����

uint8_t ageing_err_cnt[8] = {0};		// �ϻ��쳣����

float current_record_data[8] = {0};

float voltage_record_data[8] = {0};

float bus1_data = 0;

float bus2_data = 0;


void data_sync_func()
{
	for(int j = 0; j < 8; j++)
	{
		current_record_data[j] = Tier2.sampleCal.Iout[j];
		voltage_record_data[j] = Tier2.sampleCal.Vmod[j];
	}
	
	bus1_data = Tier2.sampleCal.Vbus1;
	
	bus2_data = Tier2.sampleCal.Vbus2;
}

/**********************************************************************************************************
**	�� �� �� : Protect_Trigger_Judgemet()
**	����˵�� :
**	��    �� :
**	�� �� ֵ :
**********************************************************************************************************/
void Protect_Trigger_Judgemet(void)
{
//    for(int j = 0; j < CHANNEL_MAX; j++)
//    {
//#ifdef YCJ        
//		if((Tier2.sampleCal.Vmod[j] >= 6.5f) || (Tier2.sampleCal.Iout[j] > 35.0f))
//#else 
//        if((Tier2.sampleCal.Vmod[j] >= 6.5f) || (Tier2.sampleCal.Iout[j] > 95.0f))
//#endif
//        {
//            if(speed_break_cnt[j] > 50)		// ����50ms,��������ٶ�
//            {
//				speed_break_cnt[j] = 0;
//				
//				data_sync_func();
//				
//				if(client_flag == 1)
//				{
//					memset(sendMsg, 0, 256);
//					
//					sprintf((char *)sendMsg, "[��ʾ] : ͨ��%d�ٶ��쳣:��ѹ%fV, ����%fA!\r\n", j, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//						
//					send(0, sendMsg, sizeof(sendMsg));
//				}					
//				
//                ChannelError[j] |=  SPEED_BREAK_ERR;
//				ChannelErrorFlag |= SPEED_BREAK_ERR;
//				ChannelStopFlag = 1;
//            }
//			else
//			{
//				speed_break_cnt[j]++;
//			}
//        }
//        else
//        {
//            speed_break_cnt[j] = 0;
//        }
//    }
	
	for(int j = 0; j < CHANNEL_CFG; j++)
	{
		if(((IP_Alarm_Flag_Pre[j] & IntelProtect_HPA) == 0) && ((IP_Alarm_Flag[j] & IntelProtect_HPA) != 0))	// Ӳ������
		{
			IP_Alarm_Flag_Pre[j] |= IntelProtect_HPA;
			
			if(client_flag == 1)
			{
				data_sync_func();
				
				memset(sendMsg, 0, 256);
		
				sprintf((char *)sendMsg, "[��ʾ] : ͨ��%d��%dӲ������", 2 * j + 0, 2 * j + 1);
					
				send(0, sendMsg, sizeof(sendMsg));
			}		

			ChannelError[2 * j + 0] |=  SPEED_BREAK_ERR;
			ChannelError[2 * j + 1] |=  SPEED_BREAK_ERR;
			ChannelErrorFlag |= SPEED_BREAK_ERR;
			ChannelStopFlag = 1;
		}
	}

	if(ChannelStopFlag != 0)
	{
		if(client_flag == 1)
		{
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "Bus1��ѹ = %fV, Bus2��ѹ = %fV\r\n", bus1_data, bus1_data);
				
			send(0, sendMsg, sizeof(sendMsg));	
		}
		
		for(int j = 0; j < CHANNEL_MAX; j++)
		{
//			speed_break_cnt[j] = 0;
			
			if(ChannelError[j] != 0)
			{
				Chg_Dis_Off_Cfg(1 << j);
			}
		}
		
		ChannelStopFlag = 0;
	}
}


/**********************************************************************************************************
**	�� �� �� : SelfChecking()
**	����˵�� :
**	��    �� :
**	�� �� ֵ :
**********************************************************************************************************/
void SelfChecking(void)
{
	g_ucSysMode = SYS_SELFCHECK_MODE;
	
    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        /* ���ö˿ڵ�ѹģʽ */
        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_RANGE_MOD);
        /* ������������ģʽ */
        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_MODE_MASTER);
    }
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "******************************************************************\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));

		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "����ģ���������Ϊ�˿ڵ�ѹģʽ,����ģ�������������ģʽ,��ʼ�Լ졭��\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));

		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "******************************************************************\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));	
	}
    
	
	for(int j = 0; j < 4; j++)
	{
		for(int k = 0; k < 2; k++)
		{
			int chg_num = (k == 0) ? (j + 0) : (j + 4);
			int dis_num = (k == 0) ? (j + 4) : (j + 0);
			
			// Step 1: ���ģ��4.5V 50A, ��ʱ1000ms,YCJ 30A 
			if(ChannelError[chg_num] == 0)
			{
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
#ifdef YCJ					
					sprintf((char *)sendMsg, "ģ��%d���,����ѹ4.5V,������30.0A\r\n", chg_num);
#else
					sprintf((char *)sendMsg, "ģ��%d���,����ѹ4.5V,������50.0A\r\n", chg_num);                    
#endif                    
					send(0, sendMsg, sizeof(sendMsg));			
				}					
#ifdef YCJ 				
				Chg_Dis_On_Cfg(1 << chg_num, BUCK_MODE, V_MOD, 4.5, 30.0);
#else
                Chg_Dis_On_Cfg(1 << chg_num, BUCK_MODE, V_MOD, 4.5, 50.0);          
#endif				
				osDelay(1000);
			}
			else
			{
				SET_1(module_err_flag[chg_num], 0);		// 0 - ��Ӳ������δ���������
				
				if(client_flag == 1)
				{	
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[��ʾ] : ģ��%d���ģʽ������ʧ��\r\n", chg_num);
						
					send(0, sendMsg, sizeof(sendMsg));			
				}					
				
				continue;
			}

			// Step 2: ���ģ��4.5V 85A, ��ʱ3000ms,YCJ,30A
			if(ChannelError[chg_num] == 0)
			{		
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
#ifdef YCJ					
					sprintf((char *)sendMsg, "ģ��%d���,����ѹ4.5V,������30.0A\r\n", chg_num);
#else
                    sprintf((char *)sendMsg, "ģ��%d���,����ѹ4.5V,������85.0A\r\n", chg_num); 
#endif
                    
					send(0, sendMsg, sizeof(sendMsg));			
				}					
#ifdef YCJ				
				Chg_Dis_On_Cfg(1 << chg_num, BUCK_MODE, V_MOD, 4.5, 30.0);
#else
                Chg_Dis_On_Cfg(1 << chg_num, BUCK_MODE, V_MOD, 4.5, 85.0);
#endif                
				
				osDelay(3000);
			}
			else
			{	
				SET_1(module_err_flag[chg_num], 0);		// 0 - ģ��������������ʧ��
				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[��ʾ] : ģ��%d���ģʽ������ʧ��\r\n", chg_num);
						
					send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}

			// Step 3: ���ģ�� �˿ڵ�ѹ������������			
			if(ChannelError[chg_num] == 0)
			{
				// [�ж�]�������Ƿ��쳣
				selfcheck_err_cnt[chg_num] = 0;
				
				while((Tier2.sampleCal.Vmod[1] > 4.0f) && (Tier2.sampleCal.Vmod[1] < 5.2f) && \
					  (Tier2.sampleCal.Vmod[2] > 4.0f) && (Tier2.sampleCal.Vmod[2] < 5.2f) && \
					  (Tier2.sampleCal.Vmod[3] > 4.0f) && (Tier2.sampleCal.Vmod[3] < 5.2f) && \
					  (Tier2.sampleCal.Vmod[4] > 4.0f) && (Tier2.sampleCal.Vmod[4] < 5.2f))
				{
					if(++selfcheck_err_cnt[chg_num] > 20)
					{
						SET_1(device_err_flag, 0);		// 0 - ģ�����ߴ���
						
						if(client_flag == 1)
						{
							memset(sendMsg, 0, 256);
							
							sprintf((char *)sendMsg, "[��ʾ] : ģ��%d���ߴ���\r\n", chg_num);
								
							send(0, sendMsg, sizeof(sendMsg));			
						}								
						
						SET_1(module_err_flag[chg_num], 0);
						
						ChannelError[chg_num] |= SELF_CHECKING_ERR;
						ChannelErrorFlag |= SELF_CHECKING_ERR;
						ChannelStopFlag = 1;					
						
						break;
					}
					
					osDelay(DELAY_TIME_SCALE);
				}				
				
				// [�ж�]�������ѹ�Ƿ��쳣
				selfcheck_err_cnt[chg_num] = 0;
				
				while((Tier2.sampleCal.Vmod[chg_num] < 4.0f) || (Tier2.sampleCal.Vmod[chg_num] > 5.2f))
				{
					if(++selfcheck_err_cnt[chg_num] > 20)
					{
						SET_1(module_err_flag[chg_num], 1);		// 1 - ģ������޵�ѹ���
						
						if(client_flag == 1)
						{
							memset(sendMsg, 0, 256);
							
							sprintf((char *)sendMsg, "[��ʾ] : ģ��%d���ģʽ�µ�ѹ����쳣\r\n", chg_num);
								
							send(0, sendMsg, sizeof(sendMsg));			
						}								
						
						SET_1(module_err_flag[chg_num], 0);
						
						ChannelError[chg_num] |= SELF_CHECKING_ERR;
						ChannelErrorFlag |= SELF_CHECKING_ERR;
						ChannelStopFlag = 1;					
						
						break;
					}
					
					osDelay(DELAY_TIME_SCALE);
				}
				
				// [�ж�]����������Ƿ��쳣
				selfcheck_err_cnt[chg_num] = 0;
				
				while(Tier2.sampleCal.Iout[chg_num] > 1.0f)
				{
					if(++selfcheck_err_cnt[chg_num] > 20)
					{
						SET_1(device_err_flag, 1);		// 1 - ģ�����߶�·
						
						if(client_flag == 1)
						{
							memset(sendMsg, 0, 256);
							
							sprintf((char *)sendMsg, "[��ʾ] : ģ��%d���ģʽ�������·\r\n", chg_num);
								
							send(0, sendMsg, sizeof(sendMsg));			
						}							
						
						SET_1(module_err_flag[chg_num], 0);
						
						ChannelError[chg_num] |= SELF_CHECKING_ERR;
						ChannelErrorFlag |= SELF_CHECKING_ERR;
						ChannelStopFlag = 1;					
						
						break;
					}
					
					osDelay(DELAY_TIME_SCALE);				
				}
			}
			else
			{
				SET_1(module_err_flag[chg_num], 2);		// 2 - ģ�����������������
				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[��ʾ] : ģ��%d���ģʽ������ʧ��\r\n", chg_num);
						
					send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}
			
			if(ChannelError[chg_num] != 0)
			{
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[��ʾ] : ģ��%d���ģʽ������ʧ��\r\n", chg_num);
						
					send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}
			
			// Step 4: �ŵ�ģ��2.0V 50A, ��ʱ1000ms,YCJ,30A
			if(ChannelError[dis_num] == 0)
			{
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
#ifdef YCJ					
					sprintf((char *)sendMsg, "ģ��%d�ŵ�,�ŵ��ѹ2.0V,�ŵ����30.0A\r\n", dis_num);
#else
                    sprintf((char *)sendMsg, "ģ��%d�ŵ�,�ŵ��ѹ2.0V,�ŵ����50.0A\r\n", dis_num);
#endif
                    
					send(0, sendMsg, sizeof(sendMsg));			
				}						
#ifdef     YCJ		
				Chg_Dis_On_Cfg(1 << dis_num, BOOST_MODE, V_MOD, 2.0, 30.0);
#else
                
                Chg_Dis_On_Cfg(1 << dis_num, BOOST_MODE, V_MOD, 2.0, 50.0);
#endif				
				osDelay(1000);
			}
			else
			{
				SET_1(module_err_flag[dis_num], 3);		// 3 - ģ������������ŵ�ʧ��
				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[��ʾ] : ģ��%d�ŵ�ģʽ������ʧ��\r\n", dis_num);
						
					send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}
			
			// Step 5: �ŵ�ģ��2.0V 80A, ��ʱ2000ms,YCJ=30A
			if(ChannelError[dis_num] == 0)
			{				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
#ifdef YCJ					
					sprintf((char *)sendMsg, "ģ��%d�ŵ�,�ŵ��ѹ2.0V,�ŵ����30.0A\r\n", dis_num);
#else
                    sprintf((char *)sendMsg, "ģ��%d�ŵ�,�ŵ��ѹ2.0V,�ŵ����80.0A\r\n", dis_num);
#endif                    
					send(0, sendMsg, sizeof(sendMsg));			
				}					
#ifdef YCJ				
				Chg_Dis_On_Cfg(1 << dis_num, BOOST_MODE, V_MOD, 2.0, 30.0);
#else
                Chg_Dis_On_Cfg(1 << dis_num, BOOST_MODE, V_MOD, 2.0, 80.0);
#endif                
				
				osDelay(2000);			
			}
			else
			{
				SET_1(module_err_flag[dis_num], 3);		// 3 - ģ������������ŵ�ʧ��
				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[��ʾ] : ģ��%d�ŵ�ģʽ������ʧ��\r\n", dis_num);
						
					send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}
			
			// Step 6: �ŵ�ģ�� �˿ڵ�ѹ������������
			if(ChannelError[dis_num] == 0)
			{
				// [�ж�]����������Ƿ��쳣
				selfcheck_err_cnt[dis_num] = 0;
#ifdef YCJ				
				while((Tier2.sampleCal.Iout[dis_num] < 27.0f) || (Tier2.sampleCal.Iout[dis_num] > 33.0f))
#else
                while((Tier2.sampleCal.Iout[dis_num] < 75.0f) || (Tier2.sampleCal.Iout[dis_num] > 85.0f))
#endif                
				{
					if(++selfcheck_err_cnt[dis_num] > 20)
					{
						SET_1(module_err_flag[dis_num], 4);		// 4 - ģ��ŵ�����쳣
						
						if(client_flag == 1)
						{
							memset(sendMsg, 0, 256);
							
							sprintf((char *)sendMsg, "[��ʾ] : ģ��%d�ŵ�ģʽ�µ�������쳣\r\n", dis_num);
								
							send(0, sendMsg, sizeof(sendMsg));			
						}								
												
						SET_1(module_err_flag[dis_num], 1);
						
						ChannelError[dis_num] |= SELF_CHECKING_ERR;
						ChannelErrorFlag |= SELF_CHECKING_ERR;
						ChannelStopFlag = 1;					
						
						break;
					}
					
					osDelay(DELAY_TIME_SCALE);				
				}

				// [�ж�]�������Ƿ��쳣
				selfcheck_err_cnt[dis_num] = 0;

#ifdef YCJ				
				while((Tier2.sampleCal.Iout[dis_num] < 27.0f) || (Tier2.sampleCal.Iout[dis_num] > 33.0f))
#else
                while((Tier2.sampleCal.Iout[dis_num] < 75.0f) || (Tier2.sampleCal.Iout[dis_num] > 85.0f))
#endif 
				{
					if(++selfcheck_err_cnt[dis_num] > 20)
					{
						SET_1(device_err_flag, 0);		// 0 - ģ�����ߴ���
						
						if(client_flag == 1)
						{
							memset(sendMsg, 0, 256);
							
							sprintf((char *)sendMsg, "[��ʾ] : ģ��%d�ŵ�ģʽ���Լ�����쳣\r\n", dis_num);
								
							send(0, sendMsg, sizeof(sendMsg));			
						}							
						
						SET_1(module_err_flag[dis_num], 1);
						
						ChannelError[dis_num] |= SELF_CHECKING_ERR;
						ChannelErrorFlag |= SELF_CHECKING_ERR;
						ChannelStopFlag = 1;					
						
						break;
					}
					
					osDelay(DELAY_TIME_SCALE);				
				}
			}
			else
			{
				SET_1(module_err_flag[dis_num], 5);		// 5 - ģ�������ŵ���������
				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[��ʾ] : ģ��%d�ŵ�ģʽ������ʧ��\r\n", dis_num);
						
					send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}
			
			if(ChannelError[dis_num] != 0)
			{
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[��ʾ] : ģ��%d�ŵ�ģʽ������ʧ��\r\n", dis_num);
						
					send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}
			
			Chg_Dis_Off_Cfg(1 << chg_num);

			osDelay(1000);
			
			Chg_Dis_Off_Cfg(1 << dis_num);
			
			osDelay(2000);
		}
	}
	
	
	
	
}

/**********************************************************************************************************
**	�� �� �� : Ageing()
**	����˵�� :
**	��    �� :
**	�� �� ֵ :
**********************************************************************************************************/
void Ageing(void)
{
	g_ucSysMode = SYS_AGEING_MODE;
	
#ifdef YCJ   
	for(int i = 0; i < CHANNEL_MAX; i++)
	{
		Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_RANGE_MOD);
		/* ������������ģʽ */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_MODE_MASTER);
	}    
#else
	for(int i = 0; i < CHANNEL_MAX; i = i + 2)
	{
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 0], PORT_RANGE_MOD);
		/* ������������ģʽ */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 0], PORT_MODE_MASTER);
		
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 1], PORT_RANGE_MOD);
		/* ���ôӻ�����ģʽ */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 1], PORT_MODE_SLAVE);
	}
#endif
	
	if(ChannelErrorFlag != 0)
	{
		if(client_flag == 1)
		{		
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "[��ʾ] : ChannelErrorFlag = %d, ����ϴ����˳��ϻ�ģʽ\r\n", ChannelErrorFlag);
				
			send(0, sendMsg, sizeof(sendMsg));
		}
		
		return;
	}
	else
	{
		if(client_flag == 1)
		{
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "******************************************************************\r\n");
				
			send(0, sendMsg, sizeof(sendMsg));		
			
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "����ͨ����������Ϊ�˿ڵ�ѹģʽ,����ͨ���������ò���ģʽ,��ʼ�ϻ�����\r\n");
				
			send(0, sendMsg, sizeof(sendMsg));
			
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "******************************************************************\r\n");
				
			send(0, sendMsg, sizeof(sendMsg));		
		}	
	}

#ifdef	YCJ
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 10.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "1 ͨ��4567�ŵ�,�ŵ��ѹ1.0V,�ŵ����10.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 20.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "2 ͨ��0123���,����ѹ4.5V,������20.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 32.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "2 ͨ��0123���,����ѹ4.5V,������32.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 30.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "1 ͨ��4567�ŵ�,�ŵ��ѹ1.0V,�ŵ����30.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}  
#else
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 50.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "1 ͨ��4567�ŵ�,�ŵ��ѹ1.0V,�ŵ����50.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}	
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 60.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "2 ͨ��0123���,����ѹ4.5V,������60.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}	
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 85.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "3 ͨ��0123���,����ѹ4.5V,������85.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}		
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 80.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "4 ͨ��4567�ŵ�,�ŵ��ѹ1.0V,�ŵ����80.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}		
	
#endif
	int TimeCnt = 0;
	
	ageing_flag = 1;
	
	while(++TimeCnt < AGEING_TIME_CFG * SECOND_TIME_SCALE)
	{
		if(ChannelErrorFlag == NO_ERR)
		{
			if(TimeCnt > (5 * SECOND_TIME_SCALE))
			{	
				for(int j = 0; j < 4; j ++)
				{
					if(ChannelErrorFlag == NO_ERR)
					{
						int chg_num = j;
						int dis_num = j + 4;
#ifdef YCJ
						if( ( ((Tier2.sampleCal.Vmod[chg_num] > 4.0f) && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))   && \
							  ((Tier2.sampleCal.Vmod[dis_num] > 4.0f ) && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
							  ((Tier2.sampleCal.Iout[chg_num] > 25.0f) && (Tier2.sampleCal.Iout[chg_num] < 35.0f)) && \
							  ((Tier2.sampleCal.Iout[dis_num] > 25.0f) && (Tier2.sampleCal.Iout[dis_num] < 35.0f)) ) == 0) 
#else						
						if( ( ((Tier2.sampleCal.Vmod[chg_num] > 4.0f)  && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))  && \
							  ((Tier2.sampleCal.Vmod[dis_num] > 4.0f)  && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
							  ((Tier2.sampleCal.Iout[chg_num] > 75.0f) && (Tier2.sampleCal.Iout[chg_num] < 85.0f)) && \
							  ((Tier2.sampleCal.Iout[dis_num] > 75.0f) && (Tier2.sampleCal.Iout[dis_num] < 85.0f)) ) == 0) 
#endif                        
						{
							if(ageing_err_cnt[j] > 200)		// 200 * 10ms
							{
								ageing_err_cnt[j] = 0;
								
								if(client_flag == 1)
								{
									data_sync_func();
									
									memset(sendMsg, 0, 256);
									
									sprintf((char *)sendMsg, "[��ʾ] : ͨ��%d����ϻ��쳣,ͨ��%d�ŵ��ϻ��쳣!\r\n", chg_num, dis_num);
										
									send(0, sendMsg, sizeof(sendMsg));							
									
									memset(sendMsg, 0, 256);
									
									sprintf((char *)sendMsg, "[��ʾ] : ͨ��%d���,����ѹ%fV,������%fA!\r\n", chg_num, Tier2.sampleCal.Vmod[chg_num], Tier2.sampleCal.Iout[chg_num]);
										
									send(0, sendMsg, sizeof(sendMsg));
									
									memset(sendMsg, 0, 256);
									
									sprintf((char *)sendMsg, "[��ʾ] : ͨ��%d�ŵ�,�ŵ��ѹ%fV,�ŵ����%fA!\r\n", dis_num, Tier2.sampleCal.Vmod[dis_num], Tier2.sampleCal.Iout[dis_num]);
										
									send(0, sendMsg, sizeof(sendMsg));
								}								
								
								ChannelError[chg_num] |= AGEING_ERR;
								ChannelError[dis_num] |= AGEING_ERR;
								ChannelErrorFlag |= AGEING_ERR;
								ChannelStopFlag = 1;
							}
							else
							{
								ageing_err_cnt[j]++;
							}
						}
						else
						{
							ageing_err_cnt[j] = 0;
						}
					}
					else
					{
						break;
					}
				}
				
//				if(((TimeCnt % 100) == 0) && (ChannelErrorFlag == NO_ERR))
//				{
//					for(int j = 0; j < 8; j ++)
//					{
//						memset(sendMsg, 0, 256);
//						
//						sprintf((char *)sendMsg, "ͨ��%d����%d������:��ѹ%fV, ����%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//							
//						send(0, sendMsg, sizeof(sendMsg));	
//					}
//				}
				
//////				if(((TimeCnt % 100) == 0) && (ChannelErrorFlag == NO_ERR))
//////				{
//////					if(client_flag == 1)
//////					{
//////						for(int j = 0; j < 8; j ++)
//////						{
//////							memset(sendMsg, 0, 256);
//////							
//////							sprintf((char *)sendMsg, "ͨ��%d����%d������:��ѹ%fV, ����%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//////								
//////							send(0, sendMsg, sizeof(sendMsg));	
//////						}
//////						
//////						memset(sendMsg, 0, 256);
//////						
//////						sprintf((char *)sendMsg, "Bus1��ѹ = %fV, Bus2��ѹ = %fV\r\n", Tier2.sampleCal.Vbus1, Tier2.sampleCal.Vbus2);
//////							
//////						send(0, sendMsg, sizeof(sendMsg));			
//////						
//////					}
//////				}				
				
			}
		}
		else
		{
			break;
		}

//////		if((TimeCnt % 100) == 0)
//////		{
//////			if(client_flag == 1)
//////			{
//////				for(int j = 0; j < 8; j ++)
//////				{
//////					memset(sendMsg, 0, 256);
//////					
//////					sprintf((char *)sendMsg, "ͨ��%d����%d������:��ѹ%fV, ����%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//////						
//////					send(0, sendMsg, sizeof(sendMsg));	
//////				}
//////				
//////				memset(sendMsg, 0, 256);
//////				
//////				sprintf((char *)sendMsg, "Bus1��ѹ = %fV, Bus2��ѹ = %fV\r\n", Tier2.sampleCal.Vbus1, Tier2.sampleCal.Vbus2);
//////					
//////				send(0, sendMsg, sizeof(sendMsg));			
//////				
//////			}
//////		}

		osDelay(DELAY_TIME_SCALE);
	}
	
	Chg_Dis_Off_Cfg(0x0F);
	
	Chg_Dis_Off_Cfg(0xF0);

/************************************************************************************************/
	
	if(ChannelErrorFlag != 0)
	{
		if(client_flag == 1)
		{		
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "[��ʾ] : ChannelErrorFlag = %d, �����ϻ����,����ϴ����˳��ϻ�ģʽ\r\n", ChannelErrorFlag);
				
			send(0, sendMsg, sizeof(sendMsg));
		}
		
		return;
	}
	else
	{
		if(client_flag == 1)
		{
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "******************************************************************\r\n");
				
			send(0, sendMsg, sizeof(sendMsg));		
			
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "�����ϻ����,���߼����ϻ�����\r\n");
				
			send(0, sendMsg, sizeof(sendMsg));
			
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "******************************************************************\r\n");
				
			send(0, sendMsg, sizeof(sendMsg));		
		}	
	}
	
#ifdef	YCJ
	Chg_Dis_On_Cfg(0x0F, BOOST_MODE, V_MOD, 1.0, 10.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "1 ͨ��0123�ŵ�,�ŵ��ѹ1.0V,�ŵ����10.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}		
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BUCK_MODE, V_MOD, 4.5, 20.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "2 ͨ��4567���,����ѹ4.5V,������20.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}		
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BUCK_MODE, V_MOD, 4.5, 32.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "3 ͨ��4567���,����ѹ4.5V,������32.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BOOST_MODE, V_MOD, 1.0, 30.0);  
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "4 ͨ��0123�ŵ�,�ŵ��ѹ1.0V,�ŵ����30.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}
#else
	
	Chg_Dis_On_Cfg(0x0F, BOOST_MODE, V_MOD, 1.0, 50.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "1 ͨ��0123�ŵ�,�ŵ��ѹ1.0V,�ŵ����50.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}		
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BUCK_MODE, V_MOD, 4.5, 60.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "2 ͨ��4567���,����ѹ4.5V,������60.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}	
	
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BUCK_MODE, V_MOD, 4.5, 85.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "3 ͨ��4567���,����ѹ4.5V,������85.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}	
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BOOST_MODE, V_MOD, 1.0, 80.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "4 ͨ��0123�ŵ�,�ŵ��ѹ1.0V,�ŵ����80.0A!\r\n");
			
		send(0, sendMsg, sizeof(sendMsg));
	}		
	
#endif    
	
	TimeCnt = 0;
	
	ageing_flag = 1;
	
	while(++TimeCnt < AGEING_TIME_CFG * SECOND_TIME_SCALE)
	{
		if(ChannelErrorFlag == NO_ERR)
		{
			if(TimeCnt > (5 * SECOND_TIME_SCALE))
			{	
				for(int j = 0; j < 4; j ++)
				{
					if(ChannelErrorFlag == NO_ERR)
					{
						int chg_num = j + 4;
						int dis_num = j;
#ifdef YCJ                        
						if( ( ((Tier2.sampleCal.Vmod[chg_num] > 4.0f)  && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))  && \
							  ((Tier2.sampleCal.Vmod[dis_num] > 4.0f)  && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
							  ((Tier2.sampleCal.Iout[chg_num] > 25.0f) && (Tier2.sampleCal.Iout[chg_num] < 35.0f)) && \
							  ((Tier2.sampleCal.Iout[dis_num] > 25.0f) && (Tier2.sampleCal.Iout[dis_num] < 35.0f)) ) == 0) 
#else						
						if( ( ((Tier2.sampleCal.Vmod[chg_num] > 4.0f)  && (Tier2.sampleCal.Vmod[chg_num] < 5.0f))  && \
							  ((Tier2.sampleCal.Vmod[dis_num] > 4.0f)  && (Tier2.sampleCal.Vmod[dis_num] < 5.0f))  && \
							  ((Tier2.sampleCal.Iout[chg_num] > 75.0f) && (Tier2.sampleCal.Iout[chg_num] < 85.0f)) && \
							  ((Tier2.sampleCal.Iout[dis_num] > 75.0f) && (Tier2.sampleCal.Iout[dis_num] < 85.0f)) ) == 0) 
#endif                        
						{
							if(ageing_err_cnt[j] > 200)
							{
								ageing_err_cnt[j] = 0;
								
								if(client_flag == 1)
								{
									data_sync_func();
									
									memset(sendMsg, 0, 256);
									
									sprintf((char *)sendMsg, "[��ʾ] : ͨ��%d����ϻ��쳣,ͨ��%d�ŵ��ϻ��쳣!\r\n", chg_num, dis_num);
										
									send(0, sendMsg, sizeof(sendMsg));							
									
									memset(sendMsg, 0, 256);
									
									sprintf((char *)sendMsg, "[��ʾ] : ͨ��%d���,����ѹ%fV,������%fA!\r\n", chg_num, Tier2.sampleCal.Vmod[chg_num], Tier2.sampleCal.Iout[chg_num]);
										
									send(0, sendMsg, sizeof(sendMsg));
									
									memset(sendMsg, 0, 256);
									
									sprintf((char *)sendMsg, "[��ʾ] : ͨ��%d�ŵ�,�ŵ��ѹ%fV,�ŵ����%fA!\r\n", dis_num, Tier2.sampleCal.Vmod[dis_num], Tier2.sampleCal.Iout[dis_num]);
										
									send(0, sendMsg, sizeof(sendMsg));										
								}
								
								ChannelError[chg_num] |= AGEING_ERR;
								ChannelError[dis_num] |= AGEING_ERR;
								ChannelErrorFlag |= AGEING_ERR;	
								ChannelStopFlag = 1;
							}
							else
							{
								ageing_err_cnt[j]++;
							}								
						}
						else
						{
							ageing_err_cnt[j] = 0;
						}
					}
					else
					{
						break;
					}
				}
				
//////				if(((TimeCnt % 100) == 0) && (ChannelErrorFlag == NO_ERR))
//////				{
//////					for(int j = 0; j < 8; j ++)
//////					{
//////						memset(sendMsg, 0, 256);
//////						
//////						sprintf((char *)sendMsg, "ͨ��%d����%d������:��ѹ%fV, ����%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//////							
//////						send(0, sendMsg, sizeof(sendMsg));	
//////					}
//////				}

//////				if(((TimeCnt % 100) == 0) && (ChannelErrorFlag == NO_ERR))
//////				{
//////					if(client_flag == 1)
//////					{
//////						for(int j = 0; j < 8; j ++)
//////						{
//////							memset(sendMsg, 0, 256);
//////							
//////							sprintf((char *)sendMsg, "ͨ��%d����%d������:��ѹ%fV, ����%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//////								
//////							send(0, sendMsg, sizeof(sendMsg));	
//////						}
//////						
//////						memset(sendMsg, 0, 256);
//////						
//////						sprintf((char *)sendMsg, "Bus1��ѹ = %fV, Bus2��ѹ = %fV\r\n", Tier2.sampleCal.Vbus1, Tier2.sampleCal.Vbus2);
//////							
//////						send(0, sendMsg, sizeof(sendMsg));			
//////						
//////					}
//////				}	
			}
		}
		else
		{
			break;
		}

//////		if((TimeCnt % 100) == 0)
//////		{
//////			if(client_flag == 1)
//////			{
//////				for(int j = 0; j < 8; j ++)
//////				{
//////					memset(sendMsg, 0, 256);
//////					
//////					sprintf((char *)sendMsg, "ͨ��%d����%d������:��ѹ%fV, ����%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//////						
//////					send(0, sendMsg, sizeof(sendMsg));	
//////				}
//////				
//////				memset(sendMsg, 0, 256);
//////				
//////				sprintf((char *)sendMsg, "Bus1��ѹ = %fV, Bus2��ѹ = %fV\r\n", Tier2.sampleCal.Vbus1, Tier2.sampleCal.Vbus2);
//////					
//////				send(0, sendMsg, sizeof(sendMsg));					
//////			}
//////		}

		osDelay(DELAY_TIME_SCALE);
	}
	
	Chg_Dis_Off_Cfg(0xF0);
	
	Chg_Dis_Off_Cfg(0x0F);
}

extern uint8_t flag_of_intelprot;
extern uint8_t test_flag;

extern uint8_t SelfTest_flag;

/**********************************************************************************************************
**	�� �� �� : AppTaskAgeing()
**	����˵�� :
**	��    �� :
**	�� �� ֵ :
**********************************************************************************************************/
void AppTaskAgeing(void *argument)
{
	static uint8_t ageing_run_flag = AGEING_ENABLE;
		
	while(1)
	{
//		IP_Scanning();  //��ǰ������ʱ����
		
		if(test_flag == 0)
		{
//			if(flag_of_intelprot == 0)
			if(SelfTest_flag == 0)
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
				else if((W5500_IP_ADDRESS[2] == 8) && (W5500_IP_ADDRESS[3] == 41))
				{
					if(ageing_run_flag == AGEING_ENABLE)
					{
						ageing_run_flag = AGEING_DISABLE;
						
						SelfChecking();
					}				
				}
			}
		}
		
		osDelay(AGEING_TASK_PERIOD);
	}
}


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
