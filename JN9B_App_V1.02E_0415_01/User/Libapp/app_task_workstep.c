/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_task_workstep.c
** Author      : Cai Heng
** Date        : 2021-09-19
** Description :
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t Protect_Flag_Global[CHANNEL_MAX] = {0};   //全局保护标志位
uint8_t Protect_Flag_Internal[CHANNEL_MAX] = {0}; //内部保护标志位
uint8_t Protect_Flag_Workstep[CHANNEL_MAX] = {0}; //工步保护标志位
uint8_t Protect_Flag_ACDC[CHANNEL_MAX] = {0};     //ACDC保护标志位

uint8_t  DeviceStateFlag[CHANNEL_MAX] = {0};	// 设备当前状态 0 - Stop; 1 - Start
uint8_t  DeviceStateFlagPre[CHANNEL_MAX] = {0};	// 设备上一状态
uint8_t  ProtectMaskFlag[CHANNEL_MAX] = {0};	// 故障屏蔽标识 0 - No Mask; 1 - Mask
uint32_t ProtectMaskCnt[CHANNEL_MAX] = {0};		// 故障屏蔽计数

float VcontactPre[CHANNEL_MAX];
float VcontactCur[CHANNEL_MAX];

uint8_t start_state_flag[8] = {0};
uint64_t run_time_cnt[8] = {0};
uint8_t time_over_stop[8] = {0};
uint8_t abort_type_flag[8] = {0};
uint8_t cv_flag_cnt[8] = {0};
uint8_t AcDumpConnectFlag = 1;		// 掉电接续标识，首次上电，即使有故障，也不复位工步数据
uint8_t ContactCheckFlag[8] = {0};	// 接触测试标识，0 - 无接触测试， 1 - 有接触测试
uint8_t protect_flag_upcast[8] = {0};


uint8_t ChannelTab[CHANNEL_CFG] =
{
    CHANNEL1
#if (CHANNEL_CFG > 1)
    , CHANNEL2
#endif
#if (CHANNEL_CFG > 2)
    , CHANNEL3
    , CHANNEL4
#endif
#if (CHANNEL_CFG > 4)
    , CHANNEL5
    , CHANNEL6
    , CHANNEL7
    , CHANNEL8
#endif
};

#if (DEBUG_SW == 1)
uint16_t i_read[CHANNEL_MAX] = {0};
uint16_t i_write[CHANNEL_MAX] = {0};
#endif

extern CMDProject GP_CheckList[];
extern uint32_t GP_Alarm_Flag[8];       //全局保护标志位
extern uint8_t contact_test_once_flag[8];

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
int16_t Workstep_Switch_Func(uint8_t chx);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : VolAnalogToDigital()
**  功能说明 : 电压模拟量转数字量
**  形    参 : chx - 通道，dir - 方向，vmod - 模式，v_analog - 电压模拟量，*v_digital - 电压数字量
**  返 回 值 : 0 - 成功，其他失败
**********************************************************************************************************/
int16_t VolAnalogToDigital(uint8_t chx, uint8_t dir, uint8_t vmod, float v_analog, uint16_t *v_digital)
{
	float v_k = 0.0f;
	float v_b = 0.0f;
	int32_t v_temp = 0;
	uint8_t j = chx;		// chx : 0 ~ 7
	
	if(j >= PORT_NO8)		// 内部通道号超出范围
	{
		return -1;
	}

	if(dir == BUCK_MODE)
	{
		if(vmod == V_MOD)
		{
			v_k = Tier2.calibrate[j].VmodSetK_C;
			v_b = Tier2.calibrate[j].VmodSetB_C;
		}
		else if(vmod == V_SENSE)
		{
			v_k = Tier2.calibrate[j].VsenseSetK_C;
			v_b = Tier2.calibrate[j].VsenseSetB_C;
		}
	}
	else if(dir == BOOST_MODE)
	{
		if(vmod == V_MOD)
		{
			v_k = Tier2.calibrate[j].VmodSetK_D;
			v_b = Tier2.calibrate[j].VmodSetB_D;
		}
		else if(vmod == V_SENSE)
		{
			v_k = Tier2.calibrate[j].VsenseSetK_D;
			v_b = Tier2.calibrate[j].VsenseSetB_D;
		}
	}

	v_temp = (int32_t)(v_k * v_analog + v_b);

	if(v_temp < 0)
	{
		v_temp = 0;
	}
    else if(v_temp > 65535)
	{
		v_temp = 65535;
	}

	*v_digital = (uint16_t)v_temp;

	return 0;
}

/**********************************************************************************************************
**  函 数 名 : CurAnalogToDigital()
**  功能说明 : 电流模拟量转数字量
**  形    参 : chx - 通道，dir - 方向，vmod - 模式，i_analog - 电流模拟量，*i_digital - 电流数字量
**  返 回 值 : 0 - 成功，其他失败
**********************************************************************************************************/
int16_t CurAnalogToDigital(uint8_t chx, uint8_t dir, float i_analog, uint16_t *i_digital)
{
	float i_k = 0.0f;
	float i_b = 0.0f;
	int32_t i_temp = 0;
	uint8_t j = chx;		// chx : 0 ~ 7

	if(j >= PORT_NO8)		// 内部通道号超出范围
	{
		return -1;
	}

	if(dir == BUCK_MODE)
	{
		i_k = Tier2.calibrate[j].IhSetK_C;
		i_b = Tier2.calibrate[j].IhSetB_C;
	}
    else if(dir == BOOST_MODE)
	{
		i_k = Tier2.calibrate[j].IhSetK_D;
		i_b = Tier2.calibrate[j].IhSetB_D;
	}

	i_temp = (int32_t)(i_k * i_analog + i_b);

	if(i_temp < 0)
	{
		i_temp = 0;
	}
	else if(i_temp > 65535)
	{
		i_temp = 65535;
	}

	*i_digital = (uint16_t)i_temp;

	return 0;
}

/**********************************************************************************************************
**  函 数 名 : CurLinearOutputSetting()
**  功能说明 : 电流线性输出设置
**  形    参 : chx - 通道，i_target目标电流值
**  返 回 值 : 0 - 成功，其他失败
**********************************************************************************************************/
int16_t CurLinearOutputSetting(uint8_t chx, float i_target)
{
	uint8_t j = chx;
	
	if(j >= PORT_NO8)		// 内部通道号超出范围
	{
		return -1;
	}	

	uint8_t  i_out[2] = {0};				// 电流初始(DA)值
	uint16_t i_set[CHANNEL_MAX] = {0};		// 电流设定(DA)值
	uint16_t i_dac[CHANNEL_MAX] = {0};		// 电流目标(DA)值
	
	uint8_t ch_cfg = (ChannelTab[j] & 0xF0) >> 4;		// 4 4 4 4
	uint8_t ch_id  = (ChannelTab[j] & 0x0F) >> 0;		// 1 2 3 4
	uint8_t ch_cnt = CHANNEL_MAX / ch_cfg;				// 2
	uint8_t chx_id = (ch_id - 1) * ch_cnt;				// 0 2 4 6

#if (TASK_PROTECT_SW == 1)
	int32_t ls, us;
#endif

#if (TASK_PROTECT_SW == 1)	
	Dis_int();
	ls = osKernelLock();
#endif

	for(j = 0; j < ch_cnt; j++)							// 获取各内部通道电流设定(DA)值
    {
        DAC80508_ReadDacData(chx_id + j, i_out, 1);

        i_set[chx_id + j] = i_out[0] << 8 | i_out[1];
		
#if (DEBUG_SW == 1)
        i_read[chx_id + j] = i_set[chx_id + j];
#endif
    }

    for(j = 0; j < ch_cnt; j++)                     	// 计算各内部通道电流目标(DA)值
    {
        CurAnalogToDigital(chx_id + j, Mod_DIR_Flag[chx_id + j], i_target, &i_dac[chx_id + j]);
		
#if (DEBUG_SW == 1)		
		i_write[chx_id + j] = i_dac[chx_id + j];
#endif		
    }

	uint8_t current_change_done = 0;		// 电流变化完成标识: 0 - 未完成; 1 - 已完成
	
	while(current_change_done == 0)
	{
		current_change_done = 1;
		
		for(j = 0; j < ch_cnt; j++)
		{
			if(i_set[chx_id + j] < i_dac[chx_id + j])
			{
				current_change_done = 0;	// 电流变化未完成
				
				if((i_dac[chx_id + j] - i_set[chx_id + j]) > INCREASE_STEP)
				{
					i_set[chx_id + j] += INCREASE_STEP;
				}
				else
				{
					i_set[chx_id + j] = i_dac[chx_id + j];
				}
				
				DAC80508_SetDacData(chx_id + j, i_set[chx_id + j], 1);
			}
			else if(i_set[chx_id + j] > i_dac[chx_id + j])
			{
				current_change_done = 0;	// 电流变化未完成
				
				if((i_set[chx_id + j] - i_dac[chx_id + j]) > DECREASE_STEP)
				{
					i_set[chx_id + j] -= DECREASE_STEP;
				}
				else
				{
					i_set[chx_id + j] = i_dac[chx_id + j];
				}

				DAC80508_SetDacData(chx_id + j, i_set[chx_id + j], 1);
			}
		}
	}
	
#if (TASK_PROTECT_SW == 1)	
	us = osKernelUnlock();
	osKernelRestoreLock(us);
	osKernelRestoreLock(ls);
	En_int();
#endif
	
	return 0;
}

/**********************************************************************************************************
**  函 数 名 : BatteryFormattingStop()
**  功能说明 : 电池充放电停止
**  形    参 : chx - 通道
**  返 回 值 : 0 - 成功，其他失败
**********************************************************************************************************/
int16_t BatteryFormattingStop(uint8_t chx)
{
	uint8_t j = chx;
	
	if(j >= PORT_NO8)		// 内部通道号超出范围
	{
		return -1;
	}
	if(SysModeProtMaskFlag == 0)
	{
	    uint8_t ch_cfg = (ChannelTab[j] & 0xF0) >> 4;
	    uint8_t ch_id  = (ChannelTab[j] & 0x0F) >> 0;
	    uint8_t ch_cnt = CHANNEL_MAX / ch_cfg;
	    uint8_t chx_id = (ch_id - 1) * ch_cnt;

	    DeviceStateFlagPre[j] = DeviceStateFlag[j];		// 记录设备上一状态
	    DeviceStateFlag[j] = 0;							// 记录设备当前状态
	    if(DeviceStateFlagPre[j] != DeviceStateFlag[j])	// 设备状态发送变化
	    {
	        ProtectMaskFlag[j] = 1;				// 故障屏蔽标识有效
	        ProtectMaskCnt[j] = 0;				// 故障屏蔽时间清零
	        workstep_flag_CriticalZone[j] = 0;	// 清除CV模式标识
					cv_flag_cnt[j] = 0;
	    }	
	
	    CurLinearOutputSetting(j, 0);		// 电流线性输出(至0)

	    osDelay(5);

	    for(j = 0; j < ch_cnt; j++)
	    {
	        Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_OUT_DISABLE);
	    }

	    osDelay(5);

	    for(j = 0; j < ch_cnt; j++)
	    {
	        Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_REV_DISABLE);
	    }
	
	//	for(j = 0; j < ch_cnt; j++)
	//	{
	//		Tier2.workstepRunning[chx_id + j].Trun = 0;
	//	}
	
	}
	return 0;
}

/**********************************************************************************************************
**  函 数 名 : BatteryFormattingProcess()
**  功能说明 : 电池充放电运行
**  形    参 : chx - 通道，dir - 方向，vmod - 模式，v_target - 电压目标值，i_target - 电流目标值
**  返 回 值 : 0 - 成功，其他失败
**********************************************************************************************************/
int16_t BatteryFormattingProcess(uint8_t chx, uint8_t dir, uint8_t vmod, float v_target, float i_target)
{
	uint8_t j = chx;
	
	if(j >= PORT_NO8)		// 内部通道号超出范围
	{
		return -1;
	}
	
    uint8_t  ch_cfg = (ChannelTab[j] & 0xF0) >> 4;
    uint8_t  ch_id  = (ChannelTab[j] & 0x0F) >> 0;
    uint8_t  ch_cnt = CHANNEL_MAX / ch_cfg;
    uint8_t  chx_id = (ch_id - 1) * ch_cnt;
    uint8_t  ch_run = 0;					// 实际工作的内部通道数
    uint16_t v_dac[CHANNEL_MAX] = {0};		// 电压目标(DA)值

    DeviceStateFlagPre[j] = DeviceStateFlag[j];		// 记录设备上一状态
    DeviceStateFlag[j] = 1;							// 记录设备当前状态
    if(DeviceStateFlagPre[j] != DeviceStateFlag[j])	// 设备状态发送变化
    {
        ProtectMaskFlag[j] = 1;		// 故障屏蔽标识有效
        ProtectMaskCnt[j] = 0;		// 故障屏蔽时间清零
    }

    if(i_target > ch_cnt)	// 设定电流大于启动电流时所有通道工作
    {
        ch_run = ch_cnt;
    }
    else					// 设定电流不大于启动电流时单通道工作
    {
        ch_run = 1;
    }

    for(j = 0; j < ch_cnt; j++)		// 主从配置
    {
        if(j == 0)		// 内部第一路输出作为主机
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_MODE_MASTER);
        }
        else			// 内部其余输出作为从机
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_MODE_SLAVE);
        }
    }

    for(j = 0; j < ch_cnt; j++)		// 方向配置
    {
        if((dir == BOOST_MODE) && (Mod_DIR_Flag[chx_id + j] != BOOST_MODE))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_DIR_BOOST);
        }
        if((dir == BUCK_MODE)  && (Mod_DIR_Flag[chx_id + j] != BUCK_MODE))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_DIR_BUCK);
        }
    }

    for(j = 0; j < ch_cnt; j++)		// 模式配置
    {
        if((vmod == V_MOD)   && (Mod_V_Range_Flag[chx_id + j] != V_MOD))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_RANGE_MOD);
        }
        if((vmod == V_SENSE) && (Mod_V_Range_Flag[chx_id + j] != V_SENSE))
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_RANGE_SENSE);
        }
    }

    for(j = 0; j < ch_cnt; j++)		// 获取各内部通道电压设定(DA)值并设置
    {
        VolAnalogToDigital(chx_id + j, Mod_DIR_Flag[chx_id + j], Mod_V_Range_Flag[chx_id + j], v_target, &v_dac[chx_id + j]);

        DAC80508_SetDacData(chx_id + j, v_dac[chx_id + j], 0);
    }

    for(j = 0; j < ch_run; j++)		// 根据实际通道工作情况进行操作(单通道/所有通道)
    {
        Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_REV_ENABLE);
    }

    osDelay(10);

    for(j = 0; j < ch_run; j++)
    {
        Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_OUT_ENABLE);
    }

    osDelay(10);

	j = chx;
	
	CurLinearOutputSetting(j, i_target / ch_run);	// 获取各内部通道电流设定(DA)值并设置
	
	osDelay(1);
	
	return 0;
}

/**********************************************************************************************************
**  函 数 名 : Protect_Action_Func()
**  功能说明 : 保护动作
**  形    参 : chx - 通道，flag - 标志位，sta - 状态
**  返 回 值 : 0 - 成功，其他失败
**********************************************************************************************************/
void Protect_Action_Func(uint8_t chx, uint8_t flag, uint8_t sta)
{
	if(SysModeProtMaskFlag == 0)
	{
		uint8_t j = chx;

		if(flag == PROTECT_GLOBAL)
		{
			Protect_Flag_Global[j] = 0x01;
		}
		else if(flag == PROTECT_INTERNAL)
		{
			Protect_Flag_Internal[j] = 0x01;
		}
		else if(flag == PROTECT_WORKSTEP)
		{
			Protect_Flag_Workstep[j] = 0x01;
			WorkstepProtUpcast(j);
		}
		else if(flag == PROTECT_ACDC)
		{
			Protect_Flag_ACDC[j] = 0x01;
		}
		
		if(protect_flag_upcast[j] == 0)
		{
			protect_flag_upcast[j] = 1;
//			RecordDatUpcast(j, 1);
			record_data_upcast_flag[j] = 1;
			osDelay(1);
		}

		if(AcDumpConnectFlag == 0)
		{
			Tier2.workstepRunning[j].RunSta = sta;
		}

		BatteryFormattingStop(j);

		if(AcDumpConnectFlag == 0)
		{
			memset(&Tier2.workstepAnalyze[j], 0, sizeof(Tier2.workstepAnalyze[j]));
			memset(&Tier2.workstepRunning[j], 0, sizeof(Tier2.workstepRunning[j]));
			memset(&Tier2.workstep[j]       , 0, sizeof(Tier2.workstep[j]));
			memset(&Tier2.workstepCache1[j] , 0, sizeof(Tier2.workstepCache1[j]));
			memset(&Tier2.workstepCache2[j] , 0, sizeof(Tier2.workstepCache2[j]));
		}
	}
}

/**********************************************************************************************************
**  函 数 名 : Protect_BitAction_Func()
**  功能说明 : 通道号按bit位设置
**  形    参 : chx - 通道，flag - 标志位，sta - 状态
**  返 回 值 : 无
**********************************************************************************************************/
void Protect_BitAction_Func(uint8_t chx, uint8_t flag, uint8_t sta)
{
    for(uint8_t i = 0; i < CHANNEL_USE; i++)
    {
        if((chx >> i) & 0x01)
        {
            Protect_Action_Func(i, flag, sta);
        }
    }
}

/**********************************************************************************************************
**  函 数 名 : Workstep_Protect_Func()
**  功能说明 : 工步保护
**  形    参 : chx - 通道
**  返 回 值 : 0 - 成功，其他失败
**********************************************************************************************************/
int16_t Workstep_Protect_Func(uint8_t chx)
{
	uint8_t j = chx;

	if(j >= PORT_NO8)		// 内部通道号超出范围
	{
		return -1;
	}

	if(g_ucSysMode == SYS_NORMAL_MODE)
	{
		if( (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_START  ) || \
			(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
			(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) )
		{
			/* 电压上限保护 */
			if(Tier2.workstepAnalyze[j].StepProVlimitH_En == WORKSTEP_PROTECT_ENABLE)
			{
				float v_lim = Tier2.workstepAnalyze[j].StepProVlimitH;
				float v_bat = Tier2.chRecord[j].Vbat;

				if(v_bat >= v_lim)	// 保护触发
				{
					SET_1(workstep_flag_ErrWord[j], 0);
				}
				else				// 保护未触发
				{
					SET_0(workstep_flag_ErrWord[j], 0);
				}
			}
			
			/* 电压下限保护 */
			if(Tier2.workstepAnalyze[j].StepProVlimitL_En == WORKSTEP_PROTECT_ENABLE)
			{
				if(Tier2.workstepRunning[j].RunSta != WORKSTEP_RUNSTA_REF_CONTACT)
				{
					float v_lim = Tier2.workstepAnalyze[j].StepProVlimitL;
					float v_bat = Tier2.chRecord[j].Vbat;

					if(v_bat <= v_lim)	// 保护触发
					{
						SET_1(workstep_flag_ErrWord[j], 1);
					}
					else				// 保护未触发
					{
						SET_0(workstep_flag_ErrWord[j], 1);
					}
				}
			}
			
			/* 电压超差保护 */
			if(Tier2.workstepAnalyze[j].StepProVsuperdev_En == WORKSTEP_PROTECT_ENABLE)
			{
				if( (workstep_flag_CriticalZone[j] == 3) && (ProtectMaskFlag[j] != 1) && \
					(Tier2.workstepRunning[j].StepType != SCMD_WORKSTEP_TYPE_STANDBY) )
				{
					float v_lim = Tier2.workstepAnalyze[j].StepProVsuperdev;
					float v_dif = fabs(Tier2.workstepAnalyze[j].StepRunVol - Tier2.chRecord[j].Vbat);

					if(v_dif >= v_lim)	// 保护触发
					{
						SET_1(workstep_flag_ErrWord[j], 4);
					}
					else				// 保护未触发
					{
						SET_0(workstep_flag_ErrWord[j], 4);
					}
				}
			}
			
			/* 过流保护 */
			if(Tier2.workstepAnalyze[j].StepProIov_En == WORKSTEP_PROTECT_ENABLE)
			{
				if(Tier2.workstepRunning[j].StepType != SCMD_WORKSTEP_TYPE_STANDBY)
				{
					float i_lim = Tier2.workstepAnalyze[j].StepProIov;
					float i_out = Tier2.chRecord[j].Iout;

					if(i_out >= i_lim)	// 保护触发
					{
						SET_1(workstep_flag_ErrWord[j], 2);
					}
					else				// 保护未触发
					{
						SET_0(workstep_flag_ErrWord[j], 2);
					}
				}
			}
			
			/* 电流超差保护 */
			if(Tier2.workstepAnalyze[j].StepProIsuperdev_En == WORKSTEP_PROTECT_ENABLE)
			{
				if( (workstep_flag_CriticalZone[j] == 0) && (ProtectMaskFlag[j] != 1) && \
					(Tier2.workstepRunning[j].StepType != SCMD_WORKSTEP_TYPE_STANDBY) )
				{
					float i_lim = Tier2.workstepAnalyze[j].StepProIsuperdev;
					float i_dif = fabs(Tier2.workstepAnalyze[j].StepRunCur - Tier2.chRecord[j].Iout);

					if(i_dif >= i_lim)	// 保护触发
					{
						SET_1(workstep_flag_ErrWord[j], 5);
					}
					else				// 保护未触发
					{
						SET_0(workstep_flag_ErrWord[j], 5);
					}
				}
			}
			
			/* 过容保护 */
			if(Tier2.workstepAnalyze[j].StepProCov_En == WORKSTEP_PROTECT_ENABLE)
			{
				if(Tier2.workstepRunning[j].StepType != SCMD_WORKSTEP_TYPE_STANDBY)
				{
					float c_lim = Tier2.workstepAnalyze[j].StepProCov;
					float c_bat = Tier2.chRecord[j].Cbat;

					if(c_bat >= c_lim)	// 保护触发
					{
						SET_1(workstep_flag_ErrWord[j], 3);
					}
					else				// 保护未触发
					{
						SET_0(workstep_flag_ErrWord[j], 3);
					}
				}
			}
			
			/* 单工步故障上抛记录数据 */
			if(workstep_flag_ErrWord[j] != 0)
			{
				Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
			}
		}
  }
	
	return 0;
}

/**********************************************************************************************************
**  函 数 名 : Workstep_Execute_Func()
**  功能说明 : 工步执行
**  形    参 : chx - 通道
**  返 回 值 : 0 - 成功，其他失败
**********************************************************************************************************/
int16_t Workstep_Execute_Func(uint8_t chx)
{
  uint8_t j = chx;
	
	if(j >= PORT_NO8)		// 内部通道号超出范围
	{
		return -1;
	}
	
	BatteryFormattingStop(j);
	
	osDelay(5);
	
    switch(Tier2.workstepAnalyze[j].StepType)
    {
        case SCMD_WORKSTEP_TYPE_STANDBY:        // 静置
        {
			/* do nothing */
			time_over_stop[j] = 0;
        }
        break;

        case SCMD_WORKSTEP_TYPE_CC:             // 恒流充电
        {
			time_over_stop[j] = 0;
            BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_CV:             // 恒压充电
        {
			time_over_stop[j] = 0;
            BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_CCTOCV:         // 恒流转恒压充电
        {
			time_over_stop[j] = 0;
            BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCC:            // 恒流放电
        {
			time_over_stop[j] = 0;
            BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCV:            // 恒压放电
        {
			time_over_stop[j] = 0;
            BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCCTODCV:       // 恒流转恒压放电
        {
			time_over_stop[j] = 0;
			BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_CP:             // 恒功率充电
        {
			time_over_stop[j] = 0;
            /* do nothing */
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCP:            // 恒功率放电
        {
			time_over_stop[j] = 0;
            /* do nothing */
        }
        break;

        case SCMD_WORKSTEP_TYPE_FINISH:         // 结束
        {
            /* do nothing */
        }
        break;

        default:
		{
			/* do nothing */
		}
		break;
    }
	
	return 0;
}

/**********************************************************************************************************
**  函 数 名 : Workstep_Reload_Func()
**  功能说明 : 工步参数加载
**  形    参 : chx - 通道
**  返 回 值 : 0 - 成功，其他失败
**********************************************************************************************************/
int16_t Workstep_Reload_Func(uint8_t chx)
{
    uint8_t j = chx;
	
	if(j >= PORT_NO8)		// 内部通道号超出范围
	{
		return -1;
	}

	/* 工步缓存区处理 */
	if(Tier2.workstep[j].Numb == 0)
	{
		if(Tier2.workstepCache1[j].Numb == 0)					/* workstep及workstepCache1均为空,不解析 */
		{
//			memset(&Tier2.workstep[j]      , 0, sizeof(Tier2.workstep[j]      ));
//			memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
			memset(&Tier2.workstepCache2[j], 0, sizeof(Tier2.workstepCache2[j]));
			return -1;
		}
		else /* if(Tier2.workstepCache1[j].Numb != 0) */		/* workstep为空,workstepCache1非空时,解析workstepCache1 */
		{
			memset(&Tier2.workstep[j], 0, sizeof(Tier2.workstep[j]));
			memcpy(&Tier2.workstep[j], &Tier2.workstepCache1[j], sizeof(Tier2.workstepCache1[j]));
		}
	}
	else /* if(Tier2.workstep[j].Numb != 0) */
	{
		if(Tier2.workstepCache1[j].Numb == 0)					/* workstep非空,workstepCache1为空时,不解析 */
		{
//			memset(&Tier2.workstep[j]      , 0, sizeof(Tier2.workstep[j]      ));
//			memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
			memset(&Tier2.workstepCache2[j], 0, sizeof(Tier2.workstepCache2[j]));			
			return -1;
		}
		else /* if(Tier2.workstepCache1[j].Numb != 0) */
		{
				if(Tier2.workstepCache2[j].Numb == 0)				/* workstep及workstepCache1均非空,workstepCache2为空时 */
				{
						if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_WAIT)
						{
			/* 解析workstepCache1 */
								memset(&Tier2.workstep[j], 0, sizeof(Tier2.workstep[j]));
								memcpy(&Tier2.workstep[j], &Tier2.workstepCache1[j], sizeof(Tier2.workstepCache1[j]));
						}
						else /* if(Tier2.workstepRunning[j].RunSta != WORKSTEP_RUNSTA_REF_WAIT) */
						{
								/* 不解析,清空workstepCache1 */
//                  memset(&Tier2.workstep[j], 0, sizeof(Tier2.workstep[j]));
//                  memcpy(&Tier2.workstep[j], &Tier2.workstepCache1[j], sizeof(Tier2.workstepCache1[j]));					
								memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
								return -1;
						}
				}
				else /* if(Tier2.workstepCache2[j].Numb != 0) */	/* workstep、workstepCache1及workstepCache2均非空时,解析workstepCache2 */
				{
						memset(&Tier2.workstep[j]      , 0, sizeof(Tier2.workstep[j]));
						memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
						memcpy(&Tier2.workstep[j]      , &Tier2.workstepCache2[j], sizeof(Tier2.workstepCache2[j]));
						memcpy(&Tier2.workstepCache1[j], &Tier2.workstepCache2[j], sizeof(Tier2.workstepCache2[j]));
						memset(&Tier2.workstepCache2[j], 0, sizeof(Tier2.workstepCache2[j]));
				}
		}
	}

	/* 重置工步信息解析结构体 */
	memset(&Tier2.workstepAnalyze[j], 0, sizeof(Tier2.workstepAnalyze[j]));

	Tier2.workstepAnalyze[j].StepNumb = Tier2.workstep[j].Numb;			// 工步号

	Tier2.workstepAnalyze[j].StepType = Tier2.workstep[j].Type;			// 工步类型

/* 结束工步解析 */
	if(Tier2.workstepAnalyze[j].StepType == SCMD_WORKSTEP_TYPE_FINISH)
	{
			memset(&Tier2.workstep[j]      , 0, sizeof(Tier2.workstep[j]));
			memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
			memset(&Tier2.workstepCache2[j], 0, sizeof(Tier2.workstepCache2[j]));
			return 0;
	}
	
	/* 非结束工步解析 */
	for(int i = 0; i < RUN_SEG_NUM; i++)			// 工步运行数据
	{
		float *p;
		
		switch(Tier2.workstep[j].RunSeg[i].type)
		{
			case SCMD_WORKSTEP_PARAM_VOLTAGE:		// 工步运行电压
			{
				p = &Tier2.workstepAnalyze[j].StepRunVol;
			}
			break;
			
			case SCMD_WORKSTEP_PARAM_CURRENT:		// 工步运行电流
			{
				p = &Tier2.workstepAnalyze[j].StepRunCur;
			}
			break;
			
			default :								// 预留
			{
				/* do nothing */
			}
			break;;
		}
		
		*p = Tier2.workstep[j].RunSeg[i].data;
	}

    if(Tier2.workstepAnalyze[j].StepType == SCMD_WORKSTEP_TYPE_CC)
    {
        Tier2.workstepAnalyze[j].StepRunVol = WORK_VOL_MAX;
    }
    else if(Tier2.workstepAnalyze[j].StepType == SCMD_WORKSTEP_TYPE_DCC)
		{
			Tier2.workstepAnalyze[j].StepRunVol = WORK_VOL_MIN;
		}
		else if( (Tier2.workstepAnalyze[j].StepType == SCMD_WORKSTEP_TYPE_CV) || \
				 (Tier2.workstepAnalyze[j].StepType == SCMD_WORKSTEP_TYPE_DCV) )
		{
			if(Tier2.workstepAnalyze[j].StepRunCur == 0)
			{
				Tier2.workstepAnalyze[j].StepRunCur = WORK_CUR_MAX;
			}
		}

	workstep_flag_AbortPrio[j] = 0;

    for(int i = 0; i < ABT_SEG_NUM; i++)			// 工步截止数据
    {		
		uint8_t step_abort_condition = Tier2.workstep[j].AbtSeg[i].type;
		uint8_t step_abort_priority  = (step_abort_condition >> 4) & 0x0F;
		uint8_t step_abort_type      = (step_abort_condition >> 0) & 0x0F;
		
		if((step_abort_type > SCMD_WORKSTEP_PARAM_BEGIN) && (step_abort_type < SCMD_WORKSTEP_PARAM_END))
		{
			if(step_abort_priority == 0x00)
			{
				if(i == 0)
				{
					SET_1(workstep_flag_AbortPrio[j], (step_abort_type - 1));
				}
			}
			else if(step_abort_priority == 0x0F)
			{
				SET_1(workstep_flag_AbortPrio[j], (step_abort_type - 1));
			}
			else if(step_abort_priority == 0x01)
			{
				SET_1(workstep_flag_AbortPrio[j], (step_abort_type - 1));
			}		
		}
	
		float *p;
		
        switch(step_abort_type)
        {
            case SCMD_WORKSTEP_PARAM_TIME:          // 工步截止时间
            {
                p = &Tier2.workstepAnalyze[j].StepAbtTime;
				
				*p = Tier2.workstep[j].AbtSeg[i].data;
            }
            break;
			
            case SCMD_WORKSTEP_PARAM_VOLTAGE:       // 工步截止电压
            {
                p = &Tier2.workstepAnalyze[j].StepAbtVol;
				
				*p = Tier2.workstep[j].AbtSeg[i].data;
            }
            break;
			
            case SCMD_WORKSTEP_PARAM_CURRENT:       // 工步截止电流
            {
                p = &Tier2.workstepAnalyze[j].StepAbtCur;
				
				*p = Tier2.workstep[j].AbtSeg[i].data;
            }
            break;
			
            case SCMD_WORKSTEP_PARAM_CAPACITY:      // 工步截止容量
            {
                p = &Tier2.workstepAnalyze[j].StepAbtCap;
				
				*p = Tier2.workstep[j].AbtSeg[i].data;
            }
            break;
			
            default:
			{
				/* do nothing */
			}
			break;;
        }	
    }

	for(int i = 0; i < PRO_SEG_NUM; i++)			// 工步保护数据
    {
        float   *p;
        uint8_t *q;
		
        switch(Tier2.workstep[j].ProSeg[i].type)
        {
            case SCMD_WORKSTEP_PROT_VLIMITH:                    // 工步保护电压上限使能
            case SCMD_WORKSTEP_PROT_VLIMITH | SCMD_DISABLE:     // 工步保护电压上限失能
            {
                p = &Tier2.workstepAnalyze[j].StepProVlimitH;
                q = &Tier2.workstepAnalyze[j].StepProVlimitH_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* 使能 */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* 失能 */
				}					
            }
            break;

            case SCMD_WORKSTEP_PROT_VLIMITL:                    // 工步保护电压下限使能
            case SCMD_WORKSTEP_PROT_VLIMITL | SCMD_DISABLE:     // 工步保护电压下限失能
            {
                p = &Tier2.workstepAnalyze[j].StepProVlimitL;
                q = &Tier2.workstepAnalyze[j].StepProVlimitL_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* 使能 */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* 失能 */
				}					
            }
            break;

            case SCMD_WORKSTEP_PROT_OVI:                        // 工步保护过流使能
            case SCMD_WORKSTEP_PROT_OVI | SCMD_DISABLE:         // 工步保护过流失能
            {
                p = &Tier2.workstepAnalyze[j].StepProIov;
                q = &Tier2.workstepAnalyze[j].StepProIov_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* 使能 */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* 失能 */
				}					
            }
            break;

            case SCMD_WORKSTEP_PROT_OVC:                        // 工步保护过容使能
            case SCMD_WORKSTEP_PROT_OVC | SCMD_DISABLE:         // 工步保护过容失能
            {
                p = &Tier2.workstepAnalyze[j].StepProCov;
                q = &Tier2.workstepAnalyze[j].StepProCov_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* 使能 */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* 失能 */
				}					
            }
            break;

            case SCMD_WORKSTEP_PROT_VSUPERDEV:                  // 工步保护电压超差使能
            case SCMD_WORKSTEP_PROT_VSUPERDEV | SCMD_DISABLE:   // 工步保护电压超差失能
            {
                p = &Tier2.workstepAnalyze[j].StepProVsuperdev;
                q = &Tier2.workstepAnalyze[j].StepProVsuperdev_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* 使能 */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* 失能 */
				}					
            }
            break;

            case SCMD_WORKSTEP_PROT_ISUPERDEV:                  // 工步保护电流超差使能
            case SCMD_WORKSTEP_PROT_ISUPERDEV | SCMD_DISABLE:   // 工步保护电流超差失能
            {
                p = &Tier2.workstepAnalyze[j].StepProIsuperdev;
                q = &Tier2.workstepAnalyze[j].StepProIsuperdev_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* 使能 */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* 失能 */
				}				
            }
            break;

            default:
			{
				/* do nothing */
			}
			break;;
        }
    }

    return 0;
}

/**********************************************************************************************************
**  函 数 名 : Workstep_Analysis_Func()
**  功能说明 : 工步参数解析
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void Workstep_Analysis_Func(void)
{
    for(int j = 0; j < CHANNEL_USE; j++)
    {
        uint8_t umask = 0x01 << j;

        if(workstep_flag_RunstaSet & umask)
        {
            switch(Tier2.workstepControl[j].StepType)
            {
                case WORKSTEP_RUNSTA_SET_STOP:          // 停止
                {
					ContactCheckFlag[j] = 0;
					contact_test_once_flag[j] = 0;
//					RecordDatUpcast(j, 1);
					record_data_upcast_flag[j] = 1;
					osDelay(1);
					BatteryFormattingStop(j);
//					Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_STOP;					
					memset(&Tier2.workstep[j]       , 0, sizeof(Tier2.workstep[j]       ));
					memset(&Tier2.workstepCache1[j] , 0, sizeof(Tier2.workstepCache1[j] ));
					memset(&Tier2.workstepCache2[j] , 0, sizeof(Tier2.workstepCache2[j] ));
					memset(&Tier2.workstepAnalyze[j], 0, sizeof(Tier2.workstepAnalyze[j]));
					memset(&Tier2.workstepRunning[j], 0, sizeof(Tier2.workstepRunning[j]));						
					Tier2.chRecord[j].Cbat = 0;
					Tier2.chRecord[j].Ebat = 0;
                }
                break;

                case WORKSTEP_RUNSTA_SET_PAUSE:         // 暂停
                {
					if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT)
					{
						ContactCheckFlag[j] = 1;
					}
//					RecordDatUpcast(j, 1);
					record_data_upcast_flag[j] = 1;
					osDelay(1);					
					BatteryFormattingStop(j);
					Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_PAUSE;
                }
                break;

                case WORKSTEP_RUNSTA_SET_START:         // 启动
                {
					ContactCheckFlag[j] = 0;
                    Tier2.chRecord[j].Cbat = 0;
                    Tier2.chRecord[j].Ebat = 0;
					
					if(start_state_flag[j] == 1)
					{
						start_state_flag[j] = 2;
					}
                    if(Tier2.workstepRunning[j].RunSta != WORKSTEP_RUNSTA_REF_CONTACT)
                    {				
                        Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_CONTACT;
						/* 接触检测使能 */
						if(((Tier2.gProt[j].Tcontact.isEnable & 0x80) == 0) && ((Tier2.gProt[j].Icontact.isEnable & 0x80) == 0))						
						{
							VcontactPre[j] = Tier2.chRecord[j].Vbat;
							
							/*初始电压上限检测*/
							if((Tier2.gProt[j].VinitThrH.isEnable & 0x80) == 0)
							{
								if(Tier2.record[j].Vbat > Tier2.gProt[j].VinitThrH.value)
								{
									GP_Alarm_Flag[j] |= GlobalProtect_MV;
								}
							}
							
                            /*初始电压下限检测*/
							if((Tier2.gProt[j].VinitThrL.isEnable & 0x80) == 0)
							{
								if(Tier2.record[j].Vbat < Tier2.gProt[j].VinitThrL.value)
								{
									GP_Alarm_Flag[j] |= GlobalProtect_LV;
								}
							}
							
							if(((GP_Alarm_Flag[j] & GlobalProtect_MV) != 0) || ((GP_Alarm_Flag[j] & GlobalProtect_LV) != 0))
							{
								Tier2.workstepControl[j].StepType = 0xFF;
								SET_0(workstep_flag_RunstaSet, j);
								break;								
							}
							
							memset(&Tier2.workstepAnalyze[j], 0, sizeof(Tier2.workstepAnalyze[j]));
							
							Tier2.workstepAnalyze[j].StepNumb = Tier2.workstepCache1[j].Numb;
							Tier2.workstepAnalyze[j].StepType = SCMD_WORKSTEP_TYPE_CC;
							Tier2.workstepAnalyze[j].StepRunVol = WORK_VOL_MAX;
							Tier2.workstepAnalyze[j].StepRunCur = Tier2.gProt[j].Icontact.value;
							Tier2.workstepAnalyze[j].StepAbtTime = Tier2.gProt[j].Tcontact.value;

							Tier2.workstepRunning[j].StepType = Tier2.workstepAnalyze[j].StepType;
							Tier2.workstepRunning[j].StepNumb = Tier2.workstepAnalyze[j].StepNumb;
							Tier2.workstepRunning[j].Trun     = 0;
							run_time_cnt[j] = 0;
							
							Workstep_Execute_Func(j);
                        }
                        else
                        {
                            return;
                        }
                    }
                    else
                    {
						memset(&Tier2.workstepAnalyze[j], 0, sizeof(Tier2.workstepAnalyze[j]));
						memset(&Tier2.workstepRunning[j], 0, sizeof(Tier2.workstepRunning[j]));
						memset(&Tier2.workstep[j]       , 0, sizeof(Tier2.workstep[j]       ));
//						memset(&Tier2.workstepCache1[j] , 0, sizeof(Tier2.workstepCache1[j] ));
//						memset(&Tier2.workstepCache2[j] , 0, sizeof(Tier2.workstepCache2[j] ));						
						Workstep_Reload_Func(j);
						Workstep_Execute_Func(j);

						Tier2.workstepRunning[j].StepType = Tier2.workstepAnalyze[j].StepType;
						Tier2.workstepRunning[j].StepNumb = Tier2.workstepAnalyze[j].StepNumb;
						Tier2.workstepRunning[j].RunSta   = WORKSTEP_RUNSTA_REF_RUNNING;                       
						Tier2.workstepRunning[j].Trun     = 0;
						run_time_cnt[j] = 0;
                    }
                }
                break;

                case WORKSTEP_RUNSTA_SET_CONTINUE:      // 继续
                {
					Workstep_Execute_Func(j);
					
					if(ContactCheckFlag[j] == 0)
					{
						Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_RUNNING;
					}
					else if(ContactCheckFlag[j] == 1)
					{
						ContactCheckFlag[j] = 0;
						Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_CONTACT;
					}
                }
                break;

                case WORKSTEP_RUNSTA_SET_SKIP:          // 跳转
                {
                    Workstep_Switch_Func(j);
                }
				break;

                default:
				{
					/* do nothing */
				}
				break;
            }
            Tier2.workstepControl[j].StepType = 0xFF;
        }
    }
    workstep_flag_RunstaSet = 0;
}

/**********************************************************************************************************
**  函 数 名 : Workstep_Switch_Func()
**  功能说明 : 工步切换
**  形    参 : chx - 通道
**  返 回 值 : 0 - 成功，其他失败
**********************************************************************************************************/
int16_t Workstep_Switch_Func(uint8_t chx)
{
	uint8_t j = chx;
	
	if(j >= PORT_NO8)		// 内部通道号超出范围
	{
		return -1;
	}
	
	if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
	{
		if(Workstep_Reload_Func(j) == -1)
		{
//			RecordDatUpcast(j, 1);
			if(time_over_stop[j] == 0)
			{
				record_data_upcast_flag[j] = 1;
				osDelay(1);		
			}				
			BatteryFormattingStop(j);
			Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_WAIT;
			Tier2.workstepRunning[j].Trun = 0;
			run_time_cnt[j] = 0;
			return -1;
		}
		else
		{
//			RecordDatUpcast(j, 1);
			if(time_over_stop[j] == 0)		//避免时间截止重帧
			{
				record_data_upcast_flag[j] = 1;
				osDelay(1);
			}
			Workstep_Execute_Func(j);
		}		
	}
	
	if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_WAIT)
	{
		if(Workstep_Reload_Func(j) == -1)
		{
			return -1;
		}
		else
		{
			Workstep_Execute_Func(j);
			Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_RUNNING;
		}	
	}
	
	Tier2.chRecord[j].Cbat = 0;
	Tier2.chRecord[j].Ebat = 0;
	Tier2.workstepRunning[j].StepType = Tier2.workstepAnalyze[j].StepType;
	Tier2.workstepRunning[j].StepNumb = Tier2.workstepAnalyze[j].StepNumb;
	Tier2.workstepRunning[j].Trun = 0;
	run_time_cnt[j] = 0;

	if(Tier2.workstepRunning[j].StepType == SCMD_WORKSTEP_TYPE_FINISH)
	{
		BatteryFormattingStop(j);
		Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_COMPLETE;
	}

	return 0;
}

/**********************************************************************************************************
**  函 数 名 : Workstep_Abort_Func()
**  功能说明 : 工步停止
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void Workstep_Abort_Func(void)
{
    for(int j = 0; j < CHANNEL_USE; j++)
    {
        float run_time = Tier2.workstepRunning[j].Trun / RUN_TIME_PERIOD;   // 运行时间
        float abt_time = Tier2.workstepAnalyze[j].StepAbtTime;              // 截止时间
        float run_vol  = Tier2.chRecord[j].Vbat;                            // 运行电压
        float abt_vol  = Tier2.workstepAnalyze[j].StepAbtVol;               // 截止电压
        float run_cur  = Tier2.chRecord[j].Iout;                            // 运行电流
        float abt_cur  = Tier2.workstepAnalyze[j].StepAbtCur;               // 截止电流
        float run_cap  = Tier2.chRecord[j].Cbat;                  			// 运行容量
        float abt_cap  = Tier2.workstepAnalyze[j].StepAbtCap;               // 截止容量

        if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
        {
            switch(Tier2.workstepRunning[j].StepType)
            {
                case SCMD_WORKSTEP_TYPE_STANDBY:        /* 静置 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						time_over_stop[j] = 1;
                        Workstep_Switch_Func(j);
						
//						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_TIME;
                    }
                }
                break;

                case SCMD_WORKSTEP_TYPE_CC:             /* 恒流充电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						time_over_stop[j] = 1;
						
//						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_TIME;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)))
                        {
							SET_0(workstep_flag_ErrWord[j], 6);
							
                            Workstep_Switch_Func(j);
                        }
                        else
                        {
                            SET_1(workstep_flag_ErrWord[j], 6);
							
                            Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
                        }
                    }
					else if((abt_vol > 0) && (run_vol >= abt_vol))	/* 运行电压 > 截止电压 */
					{
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_VOLTAGE;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_VOLTAGE - 1)))
						{
							SET_0(workstep_flag_ErrWord[j], 7);
							
							Workstep_Switch_Func(j);

						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 7);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
							SET_0(workstep_flag_ErrWord[j], 9);
							
							Workstep_Switch_Func(j);
						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 9);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
                }
                break;

                case SCMD_WORKSTEP_TYPE_CV:             /* 恒压充电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						time_over_stop[j] = 1;
						
//						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_TIME;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)) )
                        {
							SET_0(workstep_flag_ErrWord[j], 6);
							
                            Workstep_Switch_Func(j);
                        }
                        else
                        {
                            SET_1(workstep_flag_ErrWord[j], 6);
							
                            Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
                        }
                    }
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* 运行电流 < 截止电流 */
					{
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CURRENT;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CURRENT - 1)) )
						{
							SET_0(workstep_flag_ErrWord[j], 8);

							Workstep_Switch_Func(j);
						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 8);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
							SET_0(workstep_flag_ErrWord[j], 9);
							
							Workstep_Switch_Func(j);
						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 9);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
                }
                break;

                case SCMD_WORKSTEP_TYPE_CCTOCV:		/* 恒压恒流充电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						time_over_stop[j] = 1;
						
//						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_TIME;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)))
                        {
							SET_0(workstep_flag_ErrWord[j], 6);
							
                            Workstep_Switch_Func(j);
                        }
                        else
                        {
                            SET_1(workstep_flag_ErrWord[j], 6);
							
                            Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
                        }
                    }
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* 运行电流 < 截止电流 */
					{
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CURRENT;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CURRENT - 1)))
						{
							SET_0(workstep_flag_ErrWord[j], 8);

							Workstep_Switch_Func(j);
						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 8);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
							SET_0(workstep_flag_ErrWord[j], 9);
							
							Workstep_Switch_Func(j);
						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 9);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
                }
                break;

                case SCMD_WORKSTEP_TYPE_DCC:            /* 恒流放电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						time_over_stop[j] = 1;
						
//						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_TIME;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)))
                        {
							SET_0(workstep_flag_ErrWord[j], 6);
							
                            Workstep_Switch_Func(j);
                        }
                        else
                        {
                            SET_1(workstep_flag_ErrWord[j], 6);
							
                            Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
                        }
                    }
					else if((abt_vol > 0) && (run_vol <= abt_vol))	/* 运行电压 < 截止电压 */
					{
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_VOLTAGE;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_VOLTAGE - 1)))
						{
							SET_0(workstep_flag_ErrWord[j], 7);
							
							Workstep_Switch_Func(j);

						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 7);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{		
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
							SET_0(workstep_flag_ErrWord[j], 9);
							
							Workstep_Switch_Func(j);
						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 9);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
                }
                break;

                case SCMD_WORKSTEP_TYPE_DCV:            /* 恒压放电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {		
						time_over_stop[j] = 1;
						
//						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_TIME;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)))
                        {
							SET_0(workstep_flag_ErrWord[j], 6);
							
                            Workstep_Switch_Func(j);
                        }
                        else
                        {
                            SET_1(workstep_flag_ErrWord[j], 6);
							
                            Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
                        }
                    }
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* 运行电流 < 截止电流 */
					{	
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CURRENT;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CURRENT - 1)))
						{
							SET_0(workstep_flag_ErrWord[j], 8);

							Workstep_Switch_Func(j);
						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 8);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
							SET_0(workstep_flag_ErrWord[j], 9);
							
							Workstep_Switch_Func(j);
						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 9);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
				}
                break;

                case SCMD_WORKSTEP_TYPE_DCCTODCV:           /* 恒压恒流放电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {	
						time_over_stop[j] = 1;
						
//						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_TIME;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)))
                        {
							SET_0(workstep_flag_ErrWord[j], 6);
							
                            Workstep_Switch_Func(j);
                        }
                        else
                        {
                            SET_1(workstep_flag_ErrWord[j], 6);
							
                            Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
                        }
                    }
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* 运行电流 < 截止电流 */
					{	
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CURRENT;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CURRENT - 1)))
						{
							SET_0(workstep_flag_ErrWord[j], 8);

							Workstep_Switch_Func(j);
						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 8);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
							SET_0(workstep_flag_ErrWord[j], 9);
							
							Workstep_Switch_Func(j);
						}
						else
						{
							SET_1(workstep_flag_ErrWord[j], 9);
							
							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
						}
					}
                }
                break;

                case SCMD_WORKSTEP_TYPE_CP:
                {
					if( ((abt_time > 0) && (run_time >= abt_time)) || \
						((abt_vol  > 0) && (run_vol  >= abt_vol )) || \
						((abt_cap  > 0) && (run_cap  >= abt_cap )) )
                    {
                        Workstep_Switch_Func(j);
                    }
                }
                break;

                case SCMD_WORKSTEP_TYPE_DCP:
                {
					if( ((abt_time > 0) && (run_time >= abt_time)) || \
						((abt_vol  > 0) && (run_vol  <= abt_vol )) || \
						((abt_cap  > 0) && (run_cap  >= abt_cap )) )
					{		
						Workstep_Switch_Func(j);
					}
                }
                break;

                default:
				{
					/* do nothing */
				}
				break;
			}
		}
        else if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_WAIT)
		{
			Workstep_Switch_Func(j);
		}
		else if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT)
		{
			if( (((Tier2.gProt[j].Tcontact.isEnable & 0x80) == 0) && ((Tier2.gProt[j].Icontact.isEnable & 0x80) == 0)) || \
				(contact_test_once_flag[j] == 1) )			
			{
				abt_time = Tier2.gProt[j].Tcontact.value;
				
				if(run_time >= abt_time)	/* 运行时间 > 截止时间 */
				{
					time_over_stop[j] = 1;
					
//					abort_type_flag[j] = SCMD_WORKSTEP_PARAM_TIME;
					
					contact_test_once_flag[j] = 0;

					BatteryFormattingStop(j);
					
					VcontactCur[j] = Tier2.chRecord[j].Vbat;
					
					if((Tier2.gProt[j].VcontactDiff.isEnable & 0x80) == 0)
					{

						if((VcontactCur[j] - VcontactPre[j]) < Tier2.gProt[j].VcontactDiff.value)
						{
							GP_Alarm_Flag[j] |= IntelProtect_VCV;
						}
						else if(((GP_Alarm_Flag[j] & GlobalProtect_MV) == 0) && ((GP_Alarm_Flag[j] & GlobalProtect_LV) == 0))
						{
							GP_Alarm_Flag[j] &= ~(IntelProtect_VCV);
							
							SET_1(workstep_flag_RunstaSet, j);
							
							Tier2.workstepControl[j].StepType = WORKSTEP_RUNSTA_SET_START;
						}
					}
					else if(((GP_Alarm_Flag[j] & GlobalProtect_MV) == 0) && ((GP_Alarm_Flag[j] & GlobalProtect_LV) == 0))
					{
						SET_1(workstep_flag_RunstaSet, j);
						
						Tier2.workstepControl[j].StepType = WORKSTEP_RUNSTA_SET_START;
					}
				}
			}
		}
    }
}

/**********************************************************************************************************
**  函 数 名 : CCtoCVConvertFunc()
**  功能说明 : CC模式切换CV模式
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void CCtoCVConvertFunc(void)
{
	if(g_ucSysMode == SYS_NORMAL_MODE)
	{
		for(int j = 0; j < CHANNEL_USE; j++)    // 判断每个通道是否更新控制命令
		{
			if((Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && (workstep_flag_CriticalZone[j] < 3))
			{
				if( (Tier2.workstepRunning[j].StepType == SCMD_WORKSTEP_TYPE_CV    ) || \
					(Tier2.workstepRunning[j].StepType == SCMD_WORKSTEP_TYPE_CCTOCV) )
				{
					if( (Tier2.workstepAnalyze[j].StepRunCur - Tier2.chRecord[j].Iout       >= IN_CV_I_LIMIT) && \
						(fabs(Tier2.workstepAnalyze[j].StepRunVol - Tier2.chRecord[j].Vbat) <= CC_TO_CV_V_LINIT) )			// 进入CV稳态
					{
						if(cv_flag_cnt[j] > 100)		/* 100 * 1ms = 100ms */
						{
							cv_flag_cnt[j] = 0;
							workstep_flag_CriticalZone[j] = 3;
						}
						else
						{
							cv_flag_cnt[j]++;
						}
					}
					else if( (Tier2.workstepAnalyze[j].StepRunCur - Tier2.chRecord[j].Iout       >= CC_TO_CV_I_LIMIT) && \
							 (fabs(Tier2.workstepAnalyze[j].StepRunVol - Tier2.chRecord[j].Vbat) <= CC_TO_CV_V_LINIT) && \
							 (workstep_flag_CriticalZone[j] == 0) )															// 进入CC结束暂态
					{
						workstep_flag_CriticalZone[j] = 1;
						cv_flag_cnt[j] = 0;
					}					
					else if( (Tier2.workstepAnalyze[j].StepRunCur - Tier2.chRecord[j].Iout       < CC_TO_CV_I_LIMIT) || \
							 (fabs(Tier2.workstepAnalyze[j].StepRunVol - Tier2.chRecord[j].Vbat) > CC_TO_CV_V_LINIT) )		// 恢复初始值
					{
						workstep_flag_CriticalZone[j] = 0;
						cv_flag_cnt[j] = 0;
					}
				}
				else if( (Tier2.workstepRunning[j].StepType == SCMD_WORKSTEP_TYPE_DCV     ) || \
						 (Tier2.workstepRunning[j].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV) )
				{
					if( (Tier2.workstepAnalyze[j].StepRunCur - Tier2.chRecord[j].Iout       >= IN_DCV_I_LIMIT) && \
						(fabs(Tier2.workstepAnalyze[j].StepRunVol - Tier2.chRecord[j].Vbat) <= DCC_TO_DCV_V_LIMIT) )
					{
						if(cv_flag_cnt[j] > 100)		/* 100 * 1ms = 100ms */
						{
							cv_flag_cnt[j] = 0;
							workstep_flag_CriticalZone[j] = 3;
						}
						else
						{
							cv_flag_cnt[j]++;
						}
					}
					else if( (Tier2.workstepAnalyze[j].StepRunCur - Tier2.chRecord[j].Iout       >= DCC_TO_DCV_I_LIMIT) && \
							 (fabs(Tier2.workstepAnalyze[j].StepRunVol - Tier2.chRecord[j].Vbat) <= DCC_TO_DCV_V_LIMIT) && \
							 (workstep_flag_CriticalZone[j] == 0) )
					{
						workstep_flag_CriticalZone[j] = 1;
						cv_flag_cnt[j] = 0;
					}					
					else if( (Tier2.workstepAnalyze[j].StepRunCur - Tier2.chRecord[j].Iout       < DCC_TO_DCV_I_LIMIT) || \
							 (fabs(Tier2.workstepAnalyze[j].StepRunVol - Tier2.chRecord[j].Vbat) > DCC_TO_DCV_V_LIMIT) )
					{
						workstep_flag_CriticalZone[j] = 0;
						cv_flag_cnt[j] = 0;
					}
				}
			}
		}
	}
}

/**********************************************************************************************************
**  函 数 名 : AppTaskWorkstep()
**  功能说明 : 工步任务线程
**  形    参 : *argument - 参数
**  返 回 值 : 无
**********************************************************************************************************/
extern uint8_t TempSampCHx;
void AppTaskWorkstep(void *argument)
{
	uint32_t tmp_cnt = 0;
	
	while(1)
	{
		Workstep_Analysis_Func();

		Workstep_Abort_Func();

		CCtoCVConvertFunc();

		for(int j = 0; j < CHANNEL_USE; j++)
		{
			if( ((debug_ch_onoff_state >> (2 * j)) & 0x03) == 1)
			{
				debug_ch_onoff_state |= (3 << (2 * j));
				
				Chg_Dis_Off_Cfg(1 << j);
				
				debug_ch_onoff_state &= ~(3 << (2 * j));
			}				
			else if( ((debug_ch_onoff_state >> (2 * j)) & 0x03) == 2)
			{
				debug_ch_onoff_state |= (3 << (2 * j));
				
				Chg_Dis_On_Cfg(1 << j, Tier2.debug.set.PortDir, Mod_V_Range_Flag[j], Tier2.debug.set.Vtemp, Tier2.debug.set.Itemp);
				
				debug_ch_onoff_state &= ~(3 << (2 * j));
			}
//			if(ProtectMaskFlag[j] == 1)
//			{
//				if(++ProtectMaskCnt[j] > (2000 * WORKSTEP_TASK_PERIOD))
//				{
//					ProtectMaskFlag[j] = 0;
//					ProtectMaskCnt[j] = 0;
//				}
//			}
			
			Workstep_Protect_Func(j);
		}

		tmp_cnt++;
		if(tmp_cnt%300 == 0)
		{
			TempSampCHx++;
			if(TempSampCHx > 8)
				TempSampCHx = 1;
			SysTempSamp(TempSampCHx);	//设备温度采样
		}
    osDelay(WORKSTEP_TASK_PERIOD);
  }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
