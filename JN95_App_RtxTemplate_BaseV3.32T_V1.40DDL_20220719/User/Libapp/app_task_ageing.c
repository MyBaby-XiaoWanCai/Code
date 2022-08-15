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
const uint8_t i_set[] = {2, 10, 20, 28};
const uint8_t i_normal[] = {1, 3, 9, 11, 18, 22, 26, 30};
#else
const uint8_t i_set[] = {2, 10, 50, 78};                  //老化电流设置值
const uint8_t i_normal[] = {1, 3, 9, 11, 48, 52, 76, 80}; //采样校准电流值
#endif

uint8_t  g_ucSysMode = SYS_NORMAL_MODE;     //系统模式
uint8_t  ChannelErrorFlag = 0;              //通道故障标志
uint8_t  ChannelStopFlag = 0;               //通道停止标志
uint8_t  ChannelError[8] = {0};             //通道错误存储

uint8_t  ChannelErrorFlag_Store = 0;
uint8_t  ChannelError_Store[8] = {0};

uint8_t sendMsg[256] = {0};

extern uint8_t client_conFlg;

uint8_t ageing_flag = 0;		// 0 - 未执行老化  1 - 已执行老化


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
**	功能说明 : 充放电开启配置
**	形    参 : ch - 通道，dir - 方向，vmod - 模式，vol - 电压，cur - 电流
**	返 回 值 : 无
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
			
			// 电压DA限幅
			if(v_temp < 0)
			{
				v_temp = 0;
			}
			else if(v_temp > 65535)
			{
				v_temp = 65535;
			}			
			
			// 电流DA限幅
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
		
			// 电压DA给定
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
**	函 数 名 : Chg_Dis_Off_Cfg()
**	功能说明 : 充放电关闭配置
**	形    参 : ch - 通道
**	返 回 值 : 无
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



extern uint32_t IP_Alarm_Flag[8];       // 内部保护标志位

uint32_t IP_Alarm_Flag_Pre[8] = {0};	

//uint16_t device_err_flag = 0;

uint16_t module_err_flag[8] = {0};

uint8_t speed_break_cnt[8] = {0};		// 软件速断计数

uint8_t selfcheck_err_cnt[8] = {0};		// 自检异常计数

uint8_t ageing_err_cnt[8] = {0};		// 老化异常计数

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
**	函 数 名 : Protect_Trigger_Judgemet()
**	功能说明 : 保护判断触发判断
**	形    参 : 无
**	返 回 值 : 无
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
//            if(speed_break_cnt[j] > 50)		// 持续50ms,触发软件速断
//            {
//				speed_break_cnt[j] = 0;
//				
//				data_sync_func();
//				
//				if(client_flag == 1)
//				{
//					memset(sendMsg, 0, 256);
//					
//					sprintf((char *)sendMsg, "[提示] : 通道%d速断异常:电压%fV, 电流%fA!\r\n", j, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//						
//					W5500_send(0, sendMsg, sizeof(sendMsg));
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
		if(((IP_Alarm_Flag_Pre[j] & IntelProtect_HPA) == 0) && ((IP_Alarm_Flag[j] & IntelProtect_HPA) != 0))	// 硬件故障
		{
			IP_Alarm_Flag_Pre[j] |= IntelProtect_HPA;
			
			if(client_flag == 1)
			{
				data_sync_func();
				
				memset(sendMsg, 0, 256);
		
				sprintf((char *)sendMsg, "[提示] : 通道%d及%d硬件故障", 2 * j + 0, 2 * j + 1);
					
				W5500_send(0, sendMsg, sizeof(sendMsg));
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
			
			sprintf((char *)sendMsg, "Bus1电压 = %fV, Bus2电压 = %fV\r\n", bus1_data, bus1_data);
				
			W5500_send(0, sendMsg, sizeof(sendMsg));	
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
**	函 数 名 : SelfChecking()
**	功能说明 : 自检
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void SelfChecking(void)
{
	g_ucSysMode = SYS_SELFCHECK_MODE;
	
    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        /* 设置端口电压模式 */
        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_RANGE_MOD);
        /* 设置主机工作模式 */
        Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_MODE_MASTER);
    }
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "******************************************************************\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));

		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "所有模块均已设置为端口电压模式,所有模块均已设置主机模式,开始自检……\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));

		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "******************************************************************\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));	
	}
	
    
	for(int j = 0; j < 4; j++)
	{
		for(int k = 0; k < 2; k++)
		{
			int chg_num = (k == 0) ? (j + 0) : (j + 4);
			int dis_num = (k == 0) ? (j + 4) : (j + 0);
			
			// Step 1: 充电模块4.5V 50A, 延时1000ms,YCJ 30A 
			if(ChannelError[chg_num] == 0)
			{
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
#ifdef YCJ					
					sprintf((char *)sendMsg, "模块%d充电,充电电压4.5V,充电电流30.0A\r\n", chg_num);
#else
					sprintf((char *)sendMsg, "模块%d充电,充电电压4.5V,充电电流50.0A\r\n", chg_num);                    
#endif                    
					W5500_send(0, sendMsg, sizeof(sendMsg));			
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
				SET_1(module_err_flag[chg_num], 0);		// 0 - 因硬件故障未能启动充电
				
				if(client_flag == 1)
				{	
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[提示] : 模块%d充电模式下运行失败\r\n", chg_num);
						
					W5500_send(0, sendMsg, sizeof(sendMsg));			
				}					
				
				continue;
			}

			// Step 2: 充电模块4.5V 85A, 延时3000ms,YCJ,30A
			if(ChannelError[chg_num] == 0)
			{		
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
#ifdef YCJ					
					sprintf((char *)sendMsg, "模块%d充电,充电电压4.5V,充电电流30.0A\r\n", chg_num);
#else
                    sprintf((char *)sendMsg, "模块%d充电,充电电压4.5V,充电电流85.0A\r\n", chg_num); 
#endif
                    
					W5500_send(0, sendMsg, sizeof(sendMsg));			
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
				SET_1(module_err_flag[chg_num], 0);		// 0 - 模块因故障启动充电失败
				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[提示] : 模块%d充电模式下运行失败\r\n", chg_num);
						
					W5500_send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}

			// Step 3: 充电模块 端口电压及输出电流检测			
			if(ChannelError[chg_num] == 0)
			{
				// [判断]：接线是否异常
				selfcheck_err_cnt[chg_num] = 0;
				
				while((Tier2.sampleCal.Vmod[1] > 4.0f) && (Tier2.sampleCal.Vmod[1] < 5.2f) && \
					  (Tier2.sampleCal.Vmod[2] > 4.0f) && (Tier2.sampleCal.Vmod[2] < 5.2f) && \
					  (Tier2.sampleCal.Vmod[3] > 4.0f) && (Tier2.sampleCal.Vmod[3] < 5.2f) && \
					  (Tier2.sampleCal.Vmod[4] > 4.0f) && (Tier2.sampleCal.Vmod[4] < 5.2f))
				{
					if(++selfcheck_err_cnt[chg_num] > 20)
					{
						SET_1(device_err_flag, 0);		// 0 - 模块间接线错误
						
						if(client_flag == 1)
						{
							memset(sendMsg, 0, 256);
							
							sprintf((char *)sendMsg, "[提示] : 模块%d接线错误\r\n", chg_num);
								
							W5500_send(0, sendMsg, sizeof(sendMsg));			
						}								
						
						SET_1(module_err_flag[chg_num], 0);
						
						ChannelError[chg_num] |= SELF_CHECKING_ERR;
						ChannelErrorFlag |= SELF_CHECKING_ERR;
						ChannelStopFlag = 1;					
						
						break;
					}
					
					osDelay(DELAY_TIME_SCALE);
				}				
				
				// [判断]：输出电压是否异常
				selfcheck_err_cnt[chg_num] = 0;
				
				while((Tier2.sampleCal.Vmod[chg_num] < 4.0f) || (Tier2.sampleCal.Vmod[chg_num] > 5.2f))
				{
					if(++selfcheck_err_cnt[chg_num] > 20)
					{
						SET_1(module_err_flag[chg_num], 1);		// 1 - 模块充电后无电压输出
						
						if(client_flag == 1)
						{
							memset(sendMsg, 0, 256);
							
							sprintf((char *)sendMsg, "[提示] : 模块%d充电模式下电压输出异常\r\n", chg_num);
								
							W5500_send(0, sendMsg, sizeof(sendMsg));			
						}								
						
						SET_1(module_err_flag[chg_num], 0);
						
						ChannelError[chg_num] |= SELF_CHECKING_ERR;
						ChannelErrorFlag |= SELF_CHECKING_ERR;
						ChannelStopFlag = 1;					
						
						break;
					}
					
					osDelay(DELAY_TIME_SCALE);
				}
				
				// [判断]：输出电流是否异常
				selfcheck_err_cnt[chg_num] = 0;
				
				while(Tier2.sampleCal.Iout[chg_num] > 1.0f)
				{
					if(++selfcheck_err_cnt[chg_num] > 20)
					{
						SET_1(device_err_flag, 1);		// 1 - 模块间接线短路
						
						if(client_flag == 1)
						{
							memset(sendMsg, 0, 256);
							
							sprintf((char *)sendMsg, "[提示] : 模块%d充电模式下输出短路\r\n", chg_num);
								
							W5500_send(0, sendMsg, sizeof(sendMsg));			
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
				SET_1(module_err_flag[chg_num], 2);		// 2 - 模块启动充电后产生故障
				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[提示] : 模块%d充电模式下运行失败\r\n", chg_num);
						
					W5500_send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}
			
			if(ChannelError[chg_num] != 0)
			{
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[提示] : 模块%d充电模式下运行失败\r\n", chg_num);
						
					W5500_send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}
			
			// Step 4: 放电模块2.0V 50A, 延时1000ms,YCJ,30A
			if(ChannelError[dis_num] == 0)
			{
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
#ifdef YCJ					
					sprintf((char *)sendMsg, "模块%d放电,放电电压2.0V,放电电流30.0A\r\n", dis_num);
#else
                    sprintf((char *)sendMsg, "模块%d放电,放电电压2.0V,放电电流50.0A\r\n", dis_num);
#endif
                    
					W5500_send(0, sendMsg, sizeof(sendMsg));			
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
				SET_1(module_err_flag[dis_num], 3);		// 3 - 模块因故障启动放电失败
				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[提示] : 模块%d放电模式下运行失败\r\n", dis_num);
						
					W5500_send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}
			
			// Step 5: 放电模块2.0V 80A, 延时2000ms,YCJ=30A
			if(ChannelError[dis_num] == 0)
			{				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
#ifdef YCJ					
					sprintf((char *)sendMsg, "模块%d放电,放电电压2.0V,放电电流30.0A\r\n", dis_num);
#else
                    sprintf((char *)sendMsg, "模块%d放电,放电电压2.0V,放电电流80.0A\r\n", dis_num);
#endif                    
					W5500_send(0, sendMsg, sizeof(sendMsg));			
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
				SET_1(module_err_flag[dis_num], 3);		// 3 - 模块因故障启动放电失败
				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[提示] : 模块%d放电模式下运行失败\r\n", dis_num);
						
					W5500_send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}
			
			// Step 6: 放电模块 端口电压及输出电流检测
			if(ChannelError[dis_num] == 0)
			{
				// [判断]：输出电流是否异常
				selfcheck_err_cnt[dis_num] = 0;
#ifdef YCJ				
				while((Tier2.sampleCal.Iout[dis_num] < 27.0f) || (Tier2.sampleCal.Iout[dis_num] > 33.0f))
#else
                while((Tier2.sampleCal.Iout[dis_num] < 75.0f) || (Tier2.sampleCal.Iout[dis_num] > 85.0f))
#endif                
				{
					if(++selfcheck_err_cnt[dis_num] > 20)
					{
						SET_1(module_err_flag[dis_num], 4);		// 4 - 模块放电电流异常
						
						if(client_flag == 1)
						{
							memset(sendMsg, 0, 256);
							
							sprintf((char *)sendMsg, "[提示] : 模块%d放电模式下电流输出异常\r\n", dis_num);
								
							W5500_send(0, sendMsg, sizeof(sendMsg));			
						}								
												
						SET_1(module_err_flag[dis_num], 1);
						
						ChannelError[dis_num] |= SELF_CHECKING_ERR;
						ChannelErrorFlag |= SELF_CHECKING_ERR;
						ChannelStopFlag = 1;					
						
						break;
					}
					
					osDelay(DELAY_TIME_SCALE);				
				}

				// [判断]：接线是否异常
				selfcheck_err_cnt[dis_num] = 0;

#ifdef YCJ				
				while((Tier2.sampleCal.Iout[dis_num] < 27.0f) || (Tier2.sampleCal.Iout[dis_num] > 33.0f))
#else
                while((Tier2.sampleCal.Iout[dis_num] < 75.0f) || (Tier2.sampleCal.Iout[dis_num] > 85.0f))
#endif 
				{
					if(++selfcheck_err_cnt[dis_num] > 20)
					{
						SET_1(device_err_flag, 0);		// 0 - 模块间接线错误
						
						if(client_flag == 1)
						{
							memset(sendMsg, 0, 256);
							
							sprintf((char *)sendMsg, "[提示] : 模块%d放电模式下自检接线异常\r\n", dis_num);
								
							W5500_send(0, sendMsg, sizeof(sendMsg));			
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
				SET_1(module_err_flag[dis_num], 5);		// 5 - 模块启动放电后产生故障
				
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[提示] : 模块%d放电模式下运行失败\r\n", dis_num);
						
					W5500_send(0, sendMsg, sizeof(sendMsg));			
				}	
				
				continue;
			}
			
			if(ChannelError[dis_num] != 0)
			{
				if(client_flag == 1)
				{
					memset(sendMsg, 0, 256);
					
					sprintf((char *)sendMsg, "[提示] : 模块%d放电模式下运行失败\r\n", dis_num);
						
					W5500_send(0, sendMsg, sizeof(sendMsg));			
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
**	函 数 名 : Ageing()
**	功能说明 : 老化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void Ageing(void)
{
	g_ucSysMode = SYS_AGEING_MODE;

#ifdef YCJ   
	for(int i = 0; i < CHANNEL_MAX; i++)
	{
		Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_RANGE_MOD);
		/* 设置主机工作模式 */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_MODE_MASTER);
	}    
#else
	for(int i = 0; i < CHANNEL_MAX; i = i + 2)
	{
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 0], PORT_RANGE_MOD);
		/* 设置主机工作模式 */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 0], PORT_MODE_MASTER);
		
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 1], PORT_RANGE_MOD);
		/* 设置从机工作模式 */
		Chg_Dis_Port_Cfg(Channel_Port_Table[i + 1], PORT_MODE_SLAVE);
	}
#endif

	if(ChannelErrorFlag != 0)
	{
		if(client_flag == 1)
		{		
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "[提示] : ChannelErrorFlag = %d, 因故障存在退出老化模式\r\n", ChannelErrorFlag);
				
			W5500_send(0, sendMsg, sizeof(sendMsg));
		}
		
		return;
	}
	else
	{
		if(client_flag == 1)
		{
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "******************************************************************\r\n");
				
			W5500_send(0, sendMsg, sizeof(sendMsg));		
			
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "所有通道均已设置为端口电压模式,所有通道均已设置并机模式,开始老化……\r\n");
				
			W5500_send(0, sendMsg, sizeof(sendMsg));
			
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "******************************************************************\r\n");
				
			W5500_send(0, sendMsg, sizeof(sendMsg));		
		}	
	}
    
#ifdef	YCJ
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 10.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "1 通道4567放电,放电电压1.0V,放电电流10.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 20.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "2 通道0123充电,充电电压4.5V,充电电流20.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 32.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "2 通道0123充电,充电电压4.5V,充电电流32.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 30.0);  
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "1 通道4567放电,放电电压1.0V,放电电流30.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	} 	  
#else	
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 50.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "1 通道4567放电,放电电压1.0V,放电电流50.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}	
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 60.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "2 通道0123充电,充电电压4.5V,充电电流60.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}	
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BUCK_MODE, V_MOD, 4.5, 85.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "3 通道0123充电,充电电压4.5V,充电电流85.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BOOST_MODE, V_MOD, 1.0, 80.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "4 通道4567放电,放电电压1.0V,放电电流80.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
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
									
									sprintf((char *)sendMsg, "[提示] : 通道%d充电老化异常,通道%d放电老化异常!\r\n", chg_num, dis_num);
										
									W5500_send(0, sendMsg, sizeof(sendMsg));							
									
									memset(sendMsg, 0, 256);
									
									sprintf((char *)sendMsg, "[提示] : 通道%d充电,充电电压%fV,充电电流%fA!\r\n", chg_num, Tier2.sampleCal.Vmod[chg_num], Tier2.sampleCal.Iout[chg_num]);
										
									W5500_send(0, sendMsg, sizeof(sendMsg));
									
									memset(sendMsg, 0, 256);
									
									sprintf((char *)sendMsg, "[提示] : 通道%d放电,放电电压%fV,放电电流%fA!\r\n", dis_num, Tier2.sampleCal.Vmod[dis_num], Tier2.sampleCal.Iout[dis_num]);
										
									W5500_send(0, sendMsg, sizeof(sendMsg));
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
//						sprintf((char *)sendMsg, "通道%d运行%d秒数据:电压%fV, 电流%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
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
//////							sprintf((char *)sendMsg, "通道%d运行%d秒数据:电压%fV, 电流%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//////								
//////							send(0, sendMsg, sizeof(sendMsg));	
//////						}
//////						
//////						memset(sendMsg, 0, 256);
//////						
//////						sprintf((char *)sendMsg, "Bus1电压 = %fV, Bus2电压 = %fV\r\n", Tier2.sampleCal.Vbus1, Tier2.sampleCal.Vbus2);
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
//////					sprintf((char *)sendMsg, "通道%d运行%d秒数据:电压%fV, 电流%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//////						
//////					send(0, sendMsg, sizeof(sendMsg));	
//////				}
//////				
//////				memset(sendMsg, 0, 256);
//////				
//////				sprintf((char *)sendMsg, "Bus1电压 = %fV, Bus2电压 = %fV\r\n", Tier2.sampleCal.Vbus1, Tier2.sampleCal.Vbus2);
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
			
			sprintf((char *)sendMsg, "[提示] : ChannelErrorFlag = %d, 单边老化完成,因故障存在退出老化模式\r\n", ChannelErrorFlag);
				
			W5500_send(0, sendMsg, sizeof(sendMsg));
		}
		
		return;
	}
	else
	{
		if(client_flag == 1)
		{
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "******************************************************************\r\n");
				
			W5500_send(0, sendMsg, sizeof(sendMsg));		
			
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "单边老化完成,换边继续老化……\r\n");
				
			W5500_send(0, sendMsg, sizeof(sendMsg));
			
			memset(sendMsg, 0, 256);
			
			sprintf((char *)sendMsg, "******************************************************************\r\n");
				
			W5500_send(0, sendMsg, sizeof(sendMsg));		
		}	
	}
	
#ifdef	YCJ
	Chg_Dis_On_Cfg(0x0F, BOOST_MODE, V_MOD, 1.0, 10.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "1 通道0123放电,放电电压1.0V,放电电流10.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BUCK_MODE, V_MOD, 4.5, 20.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "2 通道4567充电,充电电压4.5V,充电电流20.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}		
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BUCK_MODE, V_MOD, 4.5, 32.0);
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "3 通道4567充电,充电电压4.5V,充电电流32.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BOOST_MODE, V_MOD, 1.0, 30.0);  
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "4 通道0123放电,放电电压1.0V,放电电流30.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}	  
#else
	
	Chg_Dis_On_Cfg(0x0F, BOOST_MODE, V_MOD, 1.0, 50.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "1 通道0123放电,放电电压1.0V,放电电流50.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BUCK_MODE, V_MOD, 4.5, 60.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "2 通道4567充电,充电电压4.5V,充电电流60.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0xF0, BUCK_MODE, V_MOD, 4.5, 85.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "3 通道4567充电,充电电压4.5V,充电电流85.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
	}
	
	osDelay(1000);
	
	Chg_Dis_On_Cfg(0x0F, BOOST_MODE, V_MOD, 1.0, 80.0);
	
	if(client_flag == 1)
	{
		memset(sendMsg, 0, 256);
		
		sprintf((char *)sendMsg, "4 通道0123放电,放电电压1.0V,放电电流80.0A!\r\n");
			
		W5500_send(0, sendMsg, sizeof(sendMsg));
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
									
									sprintf((char *)sendMsg, "[提示] : 通道%d充电老化异常,通道%d放电老化异常!\r\n", chg_num, dis_num);
										
									W5500_send(0, sendMsg, sizeof(sendMsg));							
									
									memset(sendMsg, 0, 256);
									
									sprintf((char *)sendMsg, "[提示] : 通道%d充电,充电电压%fV,充电电流%fA!\r\n", chg_num, Tier2.sampleCal.Vmod[chg_num], Tier2.sampleCal.Iout[chg_num]);
										
									W5500_send(0, sendMsg, sizeof(sendMsg));
									
									memset(sendMsg, 0, 256);
									
									sprintf((char *)sendMsg, "[提示] : 通道%d放电,放电电压%fV,放电电流%fA!\r\n", dis_num, Tier2.sampleCal.Vmod[dis_num], Tier2.sampleCal.Iout[dis_num]);
										
									W5500_send(0, sendMsg, sizeof(sendMsg));										
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
//////						sprintf((char *)sendMsg, "通道%d运行%d秒数据:电压%fV, 电流%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
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
//////							sprintf((char *)sendMsg, "通道%d运行%d秒数据:电压%fV, 电流%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//////								
//////							send(0, sendMsg, sizeof(sendMsg));	
//////						}
//////						
//////						memset(sendMsg, 0, 256);
//////						
//////						sprintf((char *)sendMsg, "Bus1电压 = %fV, Bus2电压 = %fV\r\n", Tier2.sampleCal.Vbus1, Tier2.sampleCal.Vbus2);
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
//////					sprintf((char *)sendMsg, "通道%d运行%d秒数据:电压%fV, 电流%fA\r\n", j, TimeCnt/100, Tier2.sampleCal.Vmod[j], Tier2.sampleCal.Iout[j]);
//////						
//////					send(0, sendMsg, sizeof(sendMsg));	
//////				}
//////				
//////				memset(sendMsg, 0, 256);
//////				
//////				sprintf((char *)sendMsg, "Bus1电压 = %fV, Bus2电压 = %fV\r\n", Tier2.sampleCal.Vbus1, Tier2.sampleCal.Vbus2);
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
**	函 数 名 : AppTaskAgeing()
**	功能说明 : 老化任务线程
**	形    参 : *argument - 参数
**	返 回 值 : 无
**********************************************************************************************************/
void AppTaskAgeing(void *argument)
{
	static uint8_t ageing_run_flag = AGEING_ENABLE;
	
	while(1)
	{
	
		if(test_flag == 0)
		{
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




uint8_t Ctrl_Board_FCT_Test()
{
	return 1;
}

uint8_t CB_Start()
{
	return 1;
}

uint8_t CB_Tenv_Test(void)
{	
	printf("第二项测试：环境温度测试开始！\r\n");
	
	float Tenv_tmp = 0.0f;		/* 添加TMP235环境温度读取操作 */
	
	printf("======>控制板环境温度：%f\r\n", Tenv_tmp);
	printf("======>判断规格：15°C~35°C");

	if((Tenv_tmp > 35.0f) || (Tenv_tmp < 15.0f))
	{
		printf("======>环境温度测试结果：FAIL！");	
		return 1;
	}
	
	printf("======>环境温度测试结果：PASS！");	
	
	for(uint8_t m_cnt = 0; m_cnt < CHANNEL_CFG; m_cnt++)
	{
		float Tmod_tmp = 0.0f;		/* 添加模块温度读取操作 */
		
		if(((Tmod_tmp + 5.0f) < Tenv_tmp) || ((Tenv_tmp + 5.0f) < Tmod_tmp))
		{
			printf("======>模块%d温度测试结果：FAIL！", m_cnt);
			return 1;
		}
		
		printf("======>模块%d温度测试结果：PASS！", m_cnt);
	}
	
	return 0;
}

uint8_t CB_Vbus_Vaux_Test()
{
	printf("第三项测试：BUS电压及辅助源电压测试开始！\r\n");
	
	float Vbus1_tmp = 0.0f;		/* 添加BUS1电压读取操作 */
	float Vbus2_tmp = 0.0f;		/* 添加BUS2电压读取操作 */
	
	printf("======>BUS1电压采样：%f\r\n", Vbus1_tmp);
	printf("======>BUS2电压采样：%f\r\n", Vbus2_tmp);
	printf("======>判断规格：18V~22V");
	
	if((Vbus1_tmp < 18.0f) || (Vbus1_tmp > 22.0f))
	{
		printf("======>BUS1电压采样测试结果：FAIL！");
	}
	else
	{
		printf("======>BUS1电压采样测试结果：PASS！");
	}
	
	if((Vbus2_tmp < 18.0f) || (Vbus2_tmp > 22.0f))
	{
		printf("======>BUS2电压采样测试结果：FAIL！");
	}
	else
	{
		printf("======>BUS2电压采样测试结果：PASS！");
	}
	
	float Vaux_tmp = 0.0f;		/* 添加辅助电源电压读取操作 */
	
	printf("======>辅助电源电压采样：%f\r\n", Vaux_tmp);
	printf("======>判断规格：10V~14V");

	if((Vaux_tmp < 10.0f) || (Vaux_tmp > 14.0f))
	{
		printf("======>辅助电源电压采样测试结果：FAIL！");
	}
	else
	{
		printf("======>辅助电源电压采样测试结果：PASS！");
	}	
	
	return 0;
}

uint8_t CB_Eeprom_Test()
{
	printf("第四项测试：EEPROM测试开始！\r\n");
	
	/* 添加：读取EEPROM ID是否正确 */
	
	uint8_t eeprom_read_dat[2] = {0};
	uint8_t eeprom_write_dat[2] = {0};
	uint8_t eeprom_check_dat[2] = {0};
	
	ee_ReadBytes(eeprom_read_dat, 0x10, 2);
	
	eeprom_write_dat[0] = ~eeprom_read_dat[0];
	eeprom_write_dat[1] = ~eeprom_read_dat[1];
	
	ee_WriteBytes(eeprom_write_dat, 0x10, 2);
	
	ee_ReadBytes(eeprom_check_dat, 0x10, 2);
	
	if((eeprom_write_dat[0] != eeprom_check_dat[0]) || (eeprom_write_dat[1] != eeprom_check_dat[1]))
	{
		printf("======>EEPROM测试结果：FAIL！");	
		return 1;		
	}
	
	ee_WriteBytes(eeprom_read_dat, 0x10, 2);
	
	ee_ReadBytes(eeprom_check_dat, 0x10, 2);
	
	if((eeprom_read_dat[0] != eeprom_check_dat[0]) || (eeprom_read_dat[1] != eeprom_check_dat[1]))
	{
		printf("======>EEPROM测试结果：FAIL！");	
		return 1;		
	}
	
	printf("======>EEPROM测试结果：PASS！");
	
	return 0;
}

uint8_t CB_Fram_Test()
{
	printf("第五项测试：FRAM测试开始！\r\n");
	
	/* 添加：读取FRAM ID是否正确 */
	
	uint8_t fram_read_dat[2] = {0};
	uint8_t fram_write_dat[2] = {0};
	uint8_t fram_check_dat[2] = {0};
	
	MB85Ctrl_Read(0x10, fram_read_dat, 2);
	
	fram_write_dat[0] = ~fram_read_dat[0];
	fram_write_dat[1] = ~fram_read_dat[1];
	
	MB85Ctrl_Write(0x10, fram_write_dat, 2);
	
	MB85Ctrl_Read(0x10, fram_check_dat, 2);
	
	if((fram_write_dat[0] != fram_check_dat[0]) || (fram_write_dat[1] != fram_check_dat[1]))
	{
		printf("======>FRAM测试结果：FAIL！");	
		return 1;		
	}
	
	MB85Ctrl_Write(0x10, fram_read_dat, 2);
	
	MB85Ctrl_Read(0x10, fram_check_dat, 2);
	
	if((fram_read_dat[0] != fram_check_dat[0]) || (fram_read_dat[1] != fram_check_dat[1]))
	{
		printf("======>FRAM测试结果：FAIL！");	
		return 1;		
	}
	
	printf("======>FRAM测试结果：PASS！");
	
	return 0;
}

uint8_t CB_Can_Test()
{
	printf("第六项测试：CAN1收发测试开始！\r\n");
	
	/* 添加：CAN1收发测试(电网电压) */
	
	return 0;
}

uint8_t CB_Module_Vmod_Load_Test()
{
	printf("第七项测试：模块带载测试(端口电压模式)测试开始！\r\n");
	
	/* 配置为并机状态主机模式 */
	Chg_Dis_Port_Cfg(PORT_NO1, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO3, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO5, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO7, PORT_MODE_MASTER);
	/* 配置为并机状态从机模式 */
	Chg_Dis_Port_Cfg(PORT_NO2, PORT_MODE_SLAVE);
	Chg_Dis_Port_Cfg(PORT_NO4, PORT_MODE_SLAVE);
	Chg_Dis_Port_Cfg(PORT_NO6, PORT_MODE_SLAVE);
	Chg_Dis_Port_Cfg(PORT_NO8, PORT_MODE_SLAVE);
	
	/* 配置为端口电压模式 */
	Chg_Dis_Port_Cfg(PORT_NO1, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO2, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO3, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO4, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO5, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO6, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO7, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO8, PORT_RANGE_MOD);
	
	/* 配置1、2、5、6为4.5V70A充电模式 */
	Chg_Dis_On_Cfg(0x33, BUCK_MODE, V_MOD, 4.5f, 70.0f);
	osDelay(1000);
	
	/* 配置3、4、7、8为2.0V60A放电模式 */
	Chg_Dis_On_Cfg(0xCC, BOOST_MODE, V_MOD, 2.0f, 60.0f);
	osDelay(1000);

	float Vout_tmp[8] = {0.0f};
	float Iout_tmp[8] = {0.0f};
	
	for(uint8_t ch = 0; ch < CHANNEL_MAX; ch++)
	{
		Vout_tmp[ch] = 0.0f;
		
		printf("模块%d端口电压值：%f\r\n", ch + 1, Vout_tmp[ch]);
		
		printf("判断规格：4.2V~4.8V\r\n");
		
		if((Vout_tmp[ch] < 4.2f) || (Vout_tmp[ch] > 4.8f))
		{
			printf("======>模块%d端口电压测试结果：FAIL！", ch + 1);
		}
		else	
		{
			printf("======>模块%d端口电压测试结果：PASS！", ch + 1);
		}
	}
	
	for(uint8_t ch = 0; ch < CHANNEL_MAX; ch++)
	{
		Iout_tmp[ch] = 0.0f;
		
		printf("模块%d输出电流值：%f\r\n", ch + 1, Iout_tmp[ch]);
		
		printf("判断规格：4.2V~4.8V\r\n");
		
		if((Iout_tmp[ch] < 58.0f) || (Iout_tmp[ch] > 62.0f))
		{
			printf("======>模块%d输出电流测试结果：FAIL！", ch + 1);
		}
		else	
		{
			printf("======>模块%d输出电流测试结果：PASS！", ch + 1);
		}
	}	
	
	/* 关闭1、2、5、6(充电模块) */
	Chg_Dis_Off_Cfg(0x33);
	osDelay(1000);	
	
	/* 关闭3、4、7、8(放电模块) */
	Chg_Dis_Off_Cfg(0xCC);
	osDelay(1000);		
	
	return 0;
}

uint8_t CB_Module_Vsense_Load_Test()
{
	printf("第八项测试：模块带载测试(Sense电压模式)测试开始！\r\n");
	
	/* 配置为并机状态主机模式 */
	Chg_Dis_Port_Cfg(PORT_NO1, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO3, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO5, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO7, PORT_MODE_MASTER);
	/* 配置为并机状态从机模式 */
	Chg_Dis_Port_Cfg(PORT_NO2, PORT_MODE_SLAVE);
	Chg_Dis_Port_Cfg(PORT_NO4, PORT_MODE_SLAVE);
	Chg_Dis_Port_Cfg(PORT_NO6, PORT_MODE_SLAVE);
	Chg_Dis_Port_Cfg(PORT_NO8, PORT_MODE_SLAVE);

	/* 配置为Sense电压模式 */
	Chg_Dis_Port_Cfg(PORT_NO1, PORT_RANGE_SENSE);
	Chg_Dis_Port_Cfg(PORT_NO2, PORT_RANGE_SENSE);
	Chg_Dis_Port_Cfg(PORT_NO3, PORT_RANGE_SENSE);
	Chg_Dis_Port_Cfg(PORT_NO4, PORT_RANGE_SENSE);
	Chg_Dis_Port_Cfg(PORT_NO5, PORT_RANGE_SENSE);
	Chg_Dis_Port_Cfg(PORT_NO6, PORT_RANGE_SENSE);
	Chg_Dis_Port_Cfg(PORT_NO7, PORT_RANGE_SENSE);
	Chg_Dis_Port_Cfg(PORT_NO8, PORT_RANGE_SENSE);

	/* 配置1、2、5、6为4.5V70A充电模式 */
	Chg_Dis_On_Cfg(0x33, BUCK_MODE, V_MOD, 4.5f, 70.0f);
	osDelay(1000);
	
	/* 配置3、4、7、8为2.0V60A放电模式 */
	Chg_Dis_On_Cfg(0xCC, BOOST_MODE, V_MOD, 2.0f, 60.0f);
	osDelay(1000);	
	
	float Vsense_tmp[8] = {0.0f};
	float Vtz_tmp[8] = {0.0f};
	
	for(uint8_t ch = 0; ch < CHANNEL_MAX; ch++)
	{
		Vsense_tmp[ch] = 0.0f;
		
		printf("模块%dSense电压值：%f\r\n", ch + 1, Vsense_tmp[ch]);
		
		printf("判断规格：4.2V~4.8V\r\n");
		
		if((Vsense_tmp[ch] < 4.2f) || (Vsense_tmp[ch] > 4.8f))
		{
			printf("======>模块%dSense电压测试结果：FAIL！", ch + 1);
		}
		else	
		{
			printf("======>模块%dSense电压测试结果：PASS！", ch + 1);
		}
	}
	
	for(uint8_t ch = 0; ch < CHANNEL_MAX; ch++)
	{
		Vtz_tmp[ch] = 0.0f;
		
		printf("模块%d探针电压值：%f\r\n", ch + 1, Vtz_tmp[ch]);
		
		printf("判断规格：4.2V~4.8V\r\n");
		
		if((Vtz_tmp[ch] < 4.2f) || (Vtz_tmp[ch] > 4.8f))
		{
			printf("======>模块%d探针电压测试结果：FAIL！", ch + 1);
		}
		else	
		{
			printf("======>模块%d探针电压测试结果：PASS！", ch + 1);
		}
	}		
	
	/* 配置3、4、7、8为2.0V10A放电模式 */
	Chg_Dis_On_Cfg(0xCC, BOOST_MODE, V_MOD, 2.0f, 10.0f);
	osDelay(1000);
	
	float Iout_tmp[8] = {0.0f};
	
	for(uint8_t ch = 0; ch < CHANNEL_MAX; ch++)
	{
		Iout_tmp[ch] = 0.0f;
		
		printf("模块%d输出电流值：%f\r\n", ch + 1, Iout_tmp[ch]);
		
		printf("判断规格：8.0A~12A\r\n");
		
		if((Iout_tmp[ch] < 8.0f) || (Iout_tmp[ch] > 12.0f))
		{
			printf("======>模块%d输出电流测试结果：FAIL！", ch + 1);
		}
		else	
		{
			printf("======>模块%d输出电流测试结果：PASS！", ch + 1);
		}
	}
	
	/* 关闭1、2、5、6(充电模块) */
	Chg_Dis_Off_Cfg(0x33);
	osDelay(1000);	
	
	/* 关闭3、4、7、8(放电模块) */
	Chg_Dis_Off_Cfg(0xCC);
	osDelay(1000);			
	
	/* 配置3、4、7、8为4.5V20A充电模式 */
	Chg_Dis_On_Cfg(0xCC, BUCK_MODE, V_MOD, 4.5f, 20.0f);
	osDelay(1000);
	
	/* 配置1、2、5、6为2.0V10A放电模式 */
	Chg_Dis_On_Cfg(0x33, BOOST_MODE, V_MOD, 2.0f, 10.0f);
	osDelay(1000);		
	
	for(uint8_t ch = 0; ch < CHANNEL_MAX; ch++)
	{
		Vsense_tmp[ch] = 0.0f;
		
		printf("模块%dSense电压值：%f\r\n", ch + 1, Vsense_tmp[ch]);
		
		printf("判断规格：4.2V~4.8V\r\n");
		
		if((Vsense_tmp[ch] < 4.2f) || (Vsense_tmp[ch] > 4.8f))
		{
			printf("======>模块%dSense电压测试结果：FAIL！", ch + 1);
		}
		else	
		{
			printf("======>模块%dSense电压测试结果：PASS！", ch + 1);
		}
	}	
	
	for(uint8_t ch = 0; ch < CHANNEL_MAX; ch++)
	{
		Iout_tmp[ch] = 0.0f;
		
		printf("模块%d输出电流值：%f\r\n", ch + 1, Iout_tmp[ch]);
		
		printf("判断规格：8.0A~12A\r\n");
		
		if((Iout_tmp[ch] < 8.0f) || (Iout_tmp[ch] > 12.0f))
		{
			printf("======>模块%d输出电流测试结果：FAIL！", ch + 1);
		}
		else	
		{
			printf("======>模块%d输出电流测试结果：PASS！", ch + 1);
		}
	}	

	/* 关闭3、4、7、8(充电模块) */
	Chg_Dis_Off_Cfg(0xCC);
	osDelay(1000);		

	/* 关闭1、2、5、6(放电模块) */
	Chg_Dis_Off_Cfg(0x33);
	osDelay(1000);
	
	return 0;
}

uint8_t CB_PG_Test()
{
	/* 配置为单机模式 */
	Chg_Dis_Port_Cfg(PORT_NO1, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO2, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO3, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO4, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO5, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO6, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO7, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO8, PORT_MODE_MASTER);

	/* 配置为Sense电压模式 */
	Chg_Dis_Port_Cfg(PORT_NO1, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO2, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO3, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO4, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO5, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO6, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO7, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO8, PORT_RANGE_MOD);	
	
	for(uint8_t ch = 0; ch < CHANNEL_MAX; ch++)
	{
		uint8_t offset[4] = {2, 0, 6, 4};
		uint8_t ch_1 = 1 << ch;
		uint8_t ch_2 = 3 << offset[ch / 2];
		
		printf("第八项测试：故障信号PG%d测试测试开始！\r\n", ch + 1);
		
		/* 配置ch_1为4.5V50A充电模式 */
		Chg_Dis_On_Cfg(ch_1, BUCK_MODE, V_MOD, 4.5f, 50.0f);
		osDelay(200);
		
		/* 配置ch_1为4.5V85A充电模式 */
		Chg_Dis_On_Cfg(ch_1, BUCK_MODE, V_MOD, 4.5f, 85.0f);
		osDelay(1000);
		
		/* 配置ch_2为2.0V80A放电模式 */
		Chg_Dis_On_Cfg(ch_2, BOOST_MODE, V_MOD, 2.0f, 80.0f);
		osDelay(1000);
		
		uint8_t pg_tmp = 0;
		
		if(pg_tmp != 0)
		{
			printf("======>故障信号PG%d测试结果：FAIL！", ch + 1);
		}
		else
		{
			printf("======>故障信号PG%d测试结果：PASS！", ch + 1);
		}
		
		/* 关闭ch_1(充电模块) */
		Chg_Dis_Off_Cfg(ch_1);
		osDelay(1000);		

		/* 关闭ch_2(放电模块) */
		Chg_Dis_Off_Cfg(ch_2);
		osDelay(1000);		
	}
		
	return 0;
}

uint8_t CB_Vsense_Ov_Test()
{
	printf("第九项测试：电池电压过压告警及保护测试开始！\r\n");
	
	/* 配置为单机模式 */
	Chg_Dis_Port_Cfg(PORT_NO1, PORT_MODE_MASTER);
	Chg_Dis_Port_Cfg(PORT_NO5, PORT_MODE_MASTER);

	/* 配置为Sense电压模式 */
	Chg_Dis_Port_Cfg(PORT_NO1, PORT_RANGE_MOD);
	Chg_Dis_Port_Cfg(PORT_NO5, PORT_RANGE_MOD);	
	
	/* 添加：配置Vsense过压值为4V */
	
	/* 配置1、5为4.5V50A充电模式 */
	Chg_Dis_On_Cfg(0x11, BUCK_MODE, V_MOD, 4.5f, 50.0f);
	osDelay(1000);	
	
	for(uint8_t ch = 0; ch < CHANNEL_MAX; ch++)
	{
		uint8_t pg_tmp = 0;
		
		if(pg_tmp != 0)
		{
			printf("======>故障信号PG%d测试结果：FAIL！", ch + 1);
		}
		else
		{
			printf("======>故障信号PG%d测试结果：PASS！", ch + 1);
		}	
	}
	
	/* 关闭1、5充电模块 */
	Chg_Dis_Off_Cfg(0x11);
	osDelay(1000);	
	
	/* 添加：配置Vsense过压值为5.1V */
	
	return 0;
}

uint8_t CB_Fan_Test()
{
	return 1;
}

uint8_t CB_HWDG_Test()
{
	return 1;
}

uint8_t CB_End()
{
	return 1;
}
/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
