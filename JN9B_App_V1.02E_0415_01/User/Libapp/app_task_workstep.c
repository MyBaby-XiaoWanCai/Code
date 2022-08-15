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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t Protect_Flag_Global[CHANNEL_MAX] = {0};   //ȫ�ֱ�����־λ
uint8_t Protect_Flag_Internal[CHANNEL_MAX] = {0}; //�ڲ�������־λ
uint8_t Protect_Flag_Workstep[CHANNEL_MAX] = {0}; //����������־λ
uint8_t Protect_Flag_ACDC[CHANNEL_MAX] = {0};     //ACDC������־λ

uint8_t  DeviceStateFlag[CHANNEL_MAX] = {0};	// �豸��ǰ״̬ 0 - Stop; 1 - Start
uint8_t  DeviceStateFlagPre[CHANNEL_MAX] = {0};	// �豸��һ״̬
uint8_t  ProtectMaskFlag[CHANNEL_MAX] = {0};	// �������α�ʶ 0 - No Mask; 1 - Mask
uint32_t ProtectMaskCnt[CHANNEL_MAX] = {0};		// �������μ���

float VcontactPre[CHANNEL_MAX];
float VcontactCur[CHANNEL_MAX];

uint8_t start_state_flag[8] = {0};
uint64_t run_time_cnt[8] = {0};
uint8_t time_over_stop[8] = {0};
uint8_t abort_type_flag[8] = {0};
uint8_t cv_flag_cnt[8] = {0};
uint8_t AcDumpConnectFlag = 1;		// ���������ʶ���״��ϵ磬��ʹ�й��ϣ�Ҳ����λ��������
uint8_t ContactCheckFlag[8] = {0};	// �Ӵ����Ա�ʶ��0 - �޽Ӵ����ԣ� 1 - �нӴ�����
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
extern uint32_t GP_Alarm_Flag[8];       //ȫ�ֱ�����־λ
extern uint8_t contact_test_once_flag[8];

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
int16_t Workstep_Switch_Func(uint8_t chx);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : VolAnalogToDigital()
**  ����˵�� : ��ѹģ����ת������
**  ��    �� : chx - ͨ����dir - ����vmod - ģʽ��v_analog - ��ѹģ������*v_digital - ��ѹ������
**  �� �� ֵ : 0 - �ɹ�������ʧ��
**********************************************************************************************************/
int16_t VolAnalogToDigital(uint8_t chx, uint8_t dir, uint8_t vmod, float v_analog, uint16_t *v_digital)
{
	float v_k = 0.0f;
	float v_b = 0.0f;
	int32_t v_temp = 0;
	uint8_t j = chx;		// chx : 0 ~ 7
	
	if(j >= PORT_NO8)		// �ڲ�ͨ���ų�����Χ
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
**  �� �� �� : CurAnalogToDigital()
**  ����˵�� : ����ģ����ת������
**  ��    �� : chx - ͨ����dir - ����vmod - ģʽ��i_analog - ����ģ������*i_digital - ����������
**  �� �� ֵ : 0 - �ɹ�������ʧ��
**********************************************************************************************************/
int16_t CurAnalogToDigital(uint8_t chx, uint8_t dir, float i_analog, uint16_t *i_digital)
{
	float i_k = 0.0f;
	float i_b = 0.0f;
	int32_t i_temp = 0;
	uint8_t j = chx;		// chx : 0 ~ 7

	if(j >= PORT_NO8)		// �ڲ�ͨ���ų�����Χ
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
**  �� �� �� : CurLinearOutputSetting()
**  ����˵�� : ���������������
**  ��    �� : chx - ͨ����i_targetĿ�����ֵ
**  �� �� ֵ : 0 - �ɹ�������ʧ��
**********************************************************************************************************/
int16_t CurLinearOutputSetting(uint8_t chx, float i_target)
{
	uint8_t j = chx;
	
	if(j >= PORT_NO8)		// �ڲ�ͨ���ų�����Χ
	{
		return -1;
	}	

	uint8_t  i_out[2] = {0};				// ������ʼ(DA)ֵ
	uint16_t i_set[CHANNEL_MAX] = {0};		// �����趨(DA)ֵ
	uint16_t i_dac[CHANNEL_MAX] = {0};		// ����Ŀ��(DA)ֵ
	
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

	for(j = 0; j < ch_cnt; j++)							// ��ȡ���ڲ�ͨ�������趨(DA)ֵ
    {
        DAC80508_ReadDacData(chx_id + j, i_out, 1);

        i_set[chx_id + j] = i_out[0] << 8 | i_out[1];
		
#if (DEBUG_SW == 1)
        i_read[chx_id + j] = i_set[chx_id + j];
#endif
    }

    for(j = 0; j < ch_cnt; j++)                     	// ������ڲ�ͨ������Ŀ��(DA)ֵ
    {
        CurAnalogToDigital(chx_id + j, Mod_DIR_Flag[chx_id + j], i_target, &i_dac[chx_id + j]);
		
#if (DEBUG_SW == 1)		
		i_write[chx_id + j] = i_dac[chx_id + j];
#endif		
    }

	uint8_t current_change_done = 0;		// �����仯��ɱ�ʶ: 0 - δ���; 1 - �����
	
	while(current_change_done == 0)
	{
		current_change_done = 1;
		
		for(j = 0; j < ch_cnt; j++)
		{
			if(i_set[chx_id + j] < i_dac[chx_id + j])
			{
				current_change_done = 0;	// �����仯δ���
				
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
				current_change_done = 0;	// �����仯δ���
				
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
**  �� �� �� : BatteryFormattingStop()
**  ����˵�� : ��س�ŵ�ֹͣ
**  ��    �� : chx - ͨ��
**  �� �� ֵ : 0 - �ɹ�������ʧ��
**********************************************************************************************************/
int16_t BatteryFormattingStop(uint8_t chx)
{
	uint8_t j = chx;
	
	if(j >= PORT_NO8)		// �ڲ�ͨ���ų�����Χ
	{
		return -1;
	}
	if(SysModeProtMaskFlag == 0)
	{
	    uint8_t ch_cfg = (ChannelTab[j] & 0xF0) >> 4;
	    uint8_t ch_id  = (ChannelTab[j] & 0x0F) >> 0;
	    uint8_t ch_cnt = CHANNEL_MAX / ch_cfg;
	    uint8_t chx_id = (ch_id - 1) * ch_cnt;

	    DeviceStateFlagPre[j] = DeviceStateFlag[j];		// ��¼�豸��һ״̬
	    DeviceStateFlag[j] = 0;							// ��¼�豸��ǰ״̬
	    if(DeviceStateFlagPre[j] != DeviceStateFlag[j])	// �豸״̬���ͱ仯
	    {
	        ProtectMaskFlag[j] = 1;				// �������α�ʶ��Ч
	        ProtectMaskCnt[j] = 0;				// ��������ʱ������
	        workstep_flag_CriticalZone[j] = 0;	// ���CVģʽ��ʶ
					cv_flag_cnt[j] = 0;
	    }	
	
	    CurLinearOutputSetting(j, 0);		// �����������(��0)

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
**  �� �� �� : BatteryFormattingProcess()
**  ����˵�� : ��س�ŵ�����
**  ��    �� : chx - ͨ����dir - ����vmod - ģʽ��v_target - ��ѹĿ��ֵ��i_target - ����Ŀ��ֵ
**  �� �� ֵ : 0 - �ɹ�������ʧ��
**********************************************************************************************************/
int16_t BatteryFormattingProcess(uint8_t chx, uint8_t dir, uint8_t vmod, float v_target, float i_target)
{
	uint8_t j = chx;
	
	if(j >= PORT_NO8)		// �ڲ�ͨ���ų�����Χ
	{
		return -1;
	}
	
    uint8_t  ch_cfg = (ChannelTab[j] & 0xF0) >> 4;
    uint8_t  ch_id  = (ChannelTab[j] & 0x0F) >> 0;
    uint8_t  ch_cnt = CHANNEL_MAX / ch_cfg;
    uint8_t  chx_id = (ch_id - 1) * ch_cnt;
    uint8_t  ch_run = 0;					// ʵ�ʹ������ڲ�ͨ����
    uint16_t v_dac[CHANNEL_MAX] = {0};		// ��ѹĿ��(DA)ֵ

    DeviceStateFlagPre[j] = DeviceStateFlag[j];		// ��¼�豸��һ״̬
    DeviceStateFlag[j] = 1;							// ��¼�豸��ǰ״̬
    if(DeviceStateFlagPre[j] != DeviceStateFlag[j])	// �豸״̬���ͱ仯
    {
        ProtectMaskFlag[j] = 1;		// �������α�ʶ��Ч
        ProtectMaskCnt[j] = 0;		// ��������ʱ������
    }

    if(i_target > ch_cnt)	// �趨����������������ʱ����ͨ������
    {
        ch_run = ch_cnt;
    }
    else					// �趨������������������ʱ��ͨ������
    {
        ch_run = 1;
    }

    for(j = 0; j < ch_cnt; j++)		// ��������
    {
        if(j == 0)		// �ڲ���һ·�����Ϊ����
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_MODE_MASTER);
        }
        else			// �ڲ����������Ϊ�ӻ�
        {
            Chg_Dis_Port_Cfg(Channel_Port_Table[chx_id + j], PORT_MODE_SLAVE);
        }
    }

    for(j = 0; j < ch_cnt; j++)		// ��������
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

    for(j = 0; j < ch_cnt; j++)		// ģʽ����
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

    for(j = 0; j < ch_cnt; j++)		// ��ȡ���ڲ�ͨ����ѹ�趨(DA)ֵ������
    {
        VolAnalogToDigital(chx_id + j, Mod_DIR_Flag[chx_id + j], Mod_V_Range_Flag[chx_id + j], v_target, &v_dac[chx_id + j]);

        DAC80508_SetDacData(chx_id + j, v_dac[chx_id + j], 0);
    }

    for(j = 0; j < ch_run; j++)		// ����ʵ��ͨ������������в���(��ͨ��/����ͨ��)
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
	
	CurLinearOutputSetting(j, i_target / ch_run);	// ��ȡ���ڲ�ͨ�������趨(DA)ֵ������
	
	osDelay(1);
	
	return 0;
}

/**********************************************************************************************************
**  �� �� �� : Protect_Action_Func()
**  ����˵�� : ��������
**  ��    �� : chx - ͨ����flag - ��־λ��sta - ״̬
**  �� �� ֵ : 0 - �ɹ�������ʧ��
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
**  �� �� �� : Protect_BitAction_Func()
**  ����˵�� : ͨ���Ű�bitλ����
**  ��    �� : chx - ͨ����flag - ��־λ��sta - ״̬
**  �� �� ֵ : ��
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
**  �� �� �� : Workstep_Protect_Func()
**  ����˵�� : ��������
**  ��    �� : chx - ͨ��
**  �� �� ֵ : 0 - �ɹ�������ʧ��
**********************************************************************************************************/
int16_t Workstep_Protect_Func(uint8_t chx)
{
	uint8_t j = chx;

	if(j >= PORT_NO8)		// �ڲ�ͨ���ų�����Χ
	{
		return -1;
	}

	if(g_ucSysMode == SYS_NORMAL_MODE)
	{
		if( (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_START  ) || \
			(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
			(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) )
		{
			/* ��ѹ���ޱ��� */
			if(Tier2.workstepAnalyze[j].StepProVlimitH_En == WORKSTEP_PROTECT_ENABLE)
			{
				float v_lim = Tier2.workstepAnalyze[j].StepProVlimitH;
				float v_bat = Tier2.chRecord[j].Vbat;

				if(v_bat >= v_lim)	// ��������
				{
					SET_1(workstep_flag_ErrWord[j], 0);
				}
				else				// ����δ����
				{
					SET_0(workstep_flag_ErrWord[j], 0);
				}
			}
			
			/* ��ѹ���ޱ��� */
			if(Tier2.workstepAnalyze[j].StepProVlimitL_En == WORKSTEP_PROTECT_ENABLE)
			{
				if(Tier2.workstepRunning[j].RunSta != WORKSTEP_RUNSTA_REF_CONTACT)
				{
					float v_lim = Tier2.workstepAnalyze[j].StepProVlimitL;
					float v_bat = Tier2.chRecord[j].Vbat;

					if(v_bat <= v_lim)	// ��������
					{
						SET_1(workstep_flag_ErrWord[j], 1);
					}
					else				// ����δ����
					{
						SET_0(workstep_flag_ErrWord[j], 1);
					}
				}
			}
			
			/* ��ѹ����� */
			if(Tier2.workstepAnalyze[j].StepProVsuperdev_En == WORKSTEP_PROTECT_ENABLE)
			{
				if( (workstep_flag_CriticalZone[j] == 3) && (ProtectMaskFlag[j] != 1) && \
					(Tier2.workstepRunning[j].StepType != SCMD_WORKSTEP_TYPE_STANDBY) )
				{
					float v_lim = Tier2.workstepAnalyze[j].StepProVsuperdev;
					float v_dif = fabs(Tier2.workstepAnalyze[j].StepRunVol - Tier2.chRecord[j].Vbat);

					if(v_dif >= v_lim)	// ��������
					{
						SET_1(workstep_flag_ErrWord[j], 4);
					}
					else				// ����δ����
					{
						SET_0(workstep_flag_ErrWord[j], 4);
					}
				}
			}
			
			/* �������� */
			if(Tier2.workstepAnalyze[j].StepProIov_En == WORKSTEP_PROTECT_ENABLE)
			{
				if(Tier2.workstepRunning[j].StepType != SCMD_WORKSTEP_TYPE_STANDBY)
				{
					float i_lim = Tier2.workstepAnalyze[j].StepProIov;
					float i_out = Tier2.chRecord[j].Iout;

					if(i_out >= i_lim)	// ��������
					{
						SET_1(workstep_flag_ErrWord[j], 2);
					}
					else				// ����δ����
					{
						SET_0(workstep_flag_ErrWord[j], 2);
					}
				}
			}
			
			/* ��������� */
			if(Tier2.workstepAnalyze[j].StepProIsuperdev_En == WORKSTEP_PROTECT_ENABLE)
			{
				if( (workstep_flag_CriticalZone[j] == 0) && (ProtectMaskFlag[j] != 1) && \
					(Tier2.workstepRunning[j].StepType != SCMD_WORKSTEP_TYPE_STANDBY) )
				{
					float i_lim = Tier2.workstepAnalyze[j].StepProIsuperdev;
					float i_dif = fabs(Tier2.workstepAnalyze[j].StepRunCur - Tier2.chRecord[j].Iout);

					if(i_dif >= i_lim)	// ��������
					{
						SET_1(workstep_flag_ErrWord[j], 5);
					}
					else				// ����δ����
					{
						SET_0(workstep_flag_ErrWord[j], 5);
					}
				}
			}
			
			/* ���ݱ��� */
			if(Tier2.workstepAnalyze[j].StepProCov_En == WORKSTEP_PROTECT_ENABLE)
			{
				if(Tier2.workstepRunning[j].StepType != SCMD_WORKSTEP_TYPE_STANDBY)
				{
					float c_lim = Tier2.workstepAnalyze[j].StepProCov;
					float c_bat = Tier2.chRecord[j].Cbat;

					if(c_bat >= c_lim)	// ��������
					{
						SET_1(workstep_flag_ErrWord[j], 3);
					}
					else				// ����δ����
					{
						SET_0(workstep_flag_ErrWord[j], 3);
					}
				}
			}
			
			/* �������������׼�¼���� */
			if(workstep_flag_ErrWord[j] != 0)
			{
				Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
			}
		}
  }
	
	return 0;
}

/**********************************************************************************************************
**  �� �� �� : Workstep_Execute_Func()
**  ����˵�� : ����ִ��
**  ��    �� : chx - ͨ��
**  �� �� ֵ : 0 - �ɹ�������ʧ��
**********************************************************************************************************/
int16_t Workstep_Execute_Func(uint8_t chx)
{
  uint8_t j = chx;
	
	if(j >= PORT_NO8)		// �ڲ�ͨ���ų�����Χ
	{
		return -1;
	}
	
	BatteryFormattingStop(j);
	
	osDelay(5);
	
    switch(Tier2.workstepAnalyze[j].StepType)
    {
        case SCMD_WORKSTEP_TYPE_STANDBY:        // ����
        {
			/* do nothing */
			time_over_stop[j] = 0;
        }
        break;

        case SCMD_WORKSTEP_TYPE_CC:             // �������
        {
			time_over_stop[j] = 0;
            BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_CV:             // ��ѹ���
        {
			time_over_stop[j] = 0;
            BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_CCTOCV:         // ����ת��ѹ���
        {
			time_over_stop[j] = 0;
            BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCC:            // �����ŵ�
        {
			time_over_stop[j] = 0;
            BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCV:            // ��ѹ�ŵ�
        {
			time_over_stop[j] = 0;
            BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCCTODCV:       // ����ת��ѹ�ŵ�
        {
			time_over_stop[j] = 0;
			BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_CP:             // �㹦�ʳ��
        {
			time_over_stop[j] = 0;
            /* do nothing */
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCP:            // �㹦�ʷŵ�
        {
			time_over_stop[j] = 0;
            /* do nothing */
        }
        break;

        case SCMD_WORKSTEP_TYPE_FINISH:         // ����
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
**  �� �� �� : Workstep_Reload_Func()
**  ����˵�� : ������������
**  ��    �� : chx - ͨ��
**  �� �� ֵ : 0 - �ɹ�������ʧ��
**********************************************************************************************************/
int16_t Workstep_Reload_Func(uint8_t chx)
{
    uint8_t j = chx;
	
	if(j >= PORT_NO8)		// �ڲ�ͨ���ų�����Χ
	{
		return -1;
	}

	/* �������������� */
	if(Tier2.workstep[j].Numb == 0)
	{
		if(Tier2.workstepCache1[j].Numb == 0)					/* workstep��workstepCache1��Ϊ��,������ */
		{
//			memset(&Tier2.workstep[j]      , 0, sizeof(Tier2.workstep[j]      ));
//			memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
			memset(&Tier2.workstepCache2[j], 0, sizeof(Tier2.workstepCache2[j]));
			return -1;
		}
		else /* if(Tier2.workstepCache1[j].Numb != 0) */		/* workstepΪ��,workstepCache1�ǿ�ʱ,����workstepCache1 */
		{
			memset(&Tier2.workstep[j], 0, sizeof(Tier2.workstep[j]));
			memcpy(&Tier2.workstep[j], &Tier2.workstepCache1[j], sizeof(Tier2.workstepCache1[j]));
		}
	}
	else /* if(Tier2.workstep[j].Numb != 0) */
	{
		if(Tier2.workstepCache1[j].Numb == 0)					/* workstep�ǿ�,workstepCache1Ϊ��ʱ,������ */
		{
//			memset(&Tier2.workstep[j]      , 0, sizeof(Tier2.workstep[j]      ));
//			memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
			memset(&Tier2.workstepCache2[j], 0, sizeof(Tier2.workstepCache2[j]));			
			return -1;
		}
		else /* if(Tier2.workstepCache1[j].Numb != 0) */
		{
				if(Tier2.workstepCache2[j].Numb == 0)				/* workstep��workstepCache1���ǿ�,workstepCache2Ϊ��ʱ */
				{
						if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_WAIT)
						{
			/* ����workstepCache1 */
								memset(&Tier2.workstep[j], 0, sizeof(Tier2.workstep[j]));
								memcpy(&Tier2.workstep[j], &Tier2.workstepCache1[j], sizeof(Tier2.workstepCache1[j]));
						}
						else /* if(Tier2.workstepRunning[j].RunSta != WORKSTEP_RUNSTA_REF_WAIT) */
						{
								/* ������,���workstepCache1 */
//                  memset(&Tier2.workstep[j], 0, sizeof(Tier2.workstep[j]));
//                  memcpy(&Tier2.workstep[j], &Tier2.workstepCache1[j], sizeof(Tier2.workstepCache1[j]));					
								memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
								return -1;
						}
				}
				else /* if(Tier2.workstepCache2[j].Numb != 0) */	/* workstep��workstepCache1��workstepCache2���ǿ�ʱ,����workstepCache2 */
				{
						memset(&Tier2.workstep[j]      , 0, sizeof(Tier2.workstep[j]));
						memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
						memcpy(&Tier2.workstep[j]      , &Tier2.workstepCache2[j], sizeof(Tier2.workstepCache2[j]));
						memcpy(&Tier2.workstepCache1[j], &Tier2.workstepCache2[j], sizeof(Tier2.workstepCache2[j]));
						memset(&Tier2.workstepCache2[j], 0, sizeof(Tier2.workstepCache2[j]));
				}
		}
	}

	/* ���ù�����Ϣ�����ṹ�� */
	memset(&Tier2.workstepAnalyze[j], 0, sizeof(Tier2.workstepAnalyze[j]));

	Tier2.workstepAnalyze[j].StepNumb = Tier2.workstep[j].Numb;			// ������

	Tier2.workstepAnalyze[j].StepType = Tier2.workstep[j].Type;			// ��������

/* ������������ */
	if(Tier2.workstepAnalyze[j].StepType == SCMD_WORKSTEP_TYPE_FINISH)
	{
			memset(&Tier2.workstep[j]      , 0, sizeof(Tier2.workstep[j]));
			memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
			memset(&Tier2.workstepCache2[j], 0, sizeof(Tier2.workstepCache2[j]));
			return 0;
	}
	
	/* �ǽ����������� */
	for(int i = 0; i < RUN_SEG_NUM; i++)			// ������������
	{
		float *p;
		
		switch(Tier2.workstep[j].RunSeg[i].type)
		{
			case SCMD_WORKSTEP_PARAM_VOLTAGE:		// �������е�ѹ
			{
				p = &Tier2.workstepAnalyze[j].StepRunVol;
			}
			break;
			
			case SCMD_WORKSTEP_PARAM_CURRENT:		// �������е���
			{
				p = &Tier2.workstepAnalyze[j].StepRunCur;
			}
			break;
			
			default :								// Ԥ��
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

    for(int i = 0; i < ABT_SEG_NUM; i++)			// ������ֹ����
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
            case SCMD_WORKSTEP_PARAM_TIME:          // ������ֹʱ��
            {
                p = &Tier2.workstepAnalyze[j].StepAbtTime;
				
				*p = Tier2.workstep[j].AbtSeg[i].data;
            }
            break;
			
            case SCMD_WORKSTEP_PARAM_VOLTAGE:       // ������ֹ��ѹ
            {
                p = &Tier2.workstepAnalyze[j].StepAbtVol;
				
				*p = Tier2.workstep[j].AbtSeg[i].data;
            }
            break;
			
            case SCMD_WORKSTEP_PARAM_CURRENT:       // ������ֹ����
            {
                p = &Tier2.workstepAnalyze[j].StepAbtCur;
				
				*p = Tier2.workstep[j].AbtSeg[i].data;
            }
            break;
			
            case SCMD_WORKSTEP_PARAM_CAPACITY:      // ������ֹ����
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

	for(int i = 0; i < PRO_SEG_NUM; i++)			// ������������
    {
        float   *p;
        uint8_t *q;
		
        switch(Tier2.workstep[j].ProSeg[i].type)
        {
            case SCMD_WORKSTEP_PROT_VLIMITH:                    // ����������ѹ����ʹ��
            case SCMD_WORKSTEP_PROT_VLIMITH | SCMD_DISABLE:     // ����������ѹ����ʧ��
            {
                p = &Tier2.workstepAnalyze[j].StepProVlimitH;
                q = &Tier2.workstepAnalyze[j].StepProVlimitH_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* ʹ�� */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* ʧ�� */
				}					
            }
            break;

            case SCMD_WORKSTEP_PROT_VLIMITL:                    // ����������ѹ����ʹ��
            case SCMD_WORKSTEP_PROT_VLIMITL | SCMD_DISABLE:     // ����������ѹ����ʧ��
            {
                p = &Tier2.workstepAnalyze[j].StepProVlimitL;
                q = &Tier2.workstepAnalyze[j].StepProVlimitL_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* ʹ�� */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* ʧ�� */
				}					
            }
            break;

            case SCMD_WORKSTEP_PROT_OVI:                        // ������������ʹ��
            case SCMD_WORKSTEP_PROT_OVI | SCMD_DISABLE:         // ������������ʧ��
            {
                p = &Tier2.workstepAnalyze[j].StepProIov;
                q = &Tier2.workstepAnalyze[j].StepProIov_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* ʹ�� */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* ʧ�� */
				}					
            }
            break;

            case SCMD_WORKSTEP_PROT_OVC:                        // ������������ʹ��
            case SCMD_WORKSTEP_PROT_OVC | SCMD_DISABLE:         // ������������ʧ��
            {
                p = &Tier2.workstepAnalyze[j].StepProCov;
                q = &Tier2.workstepAnalyze[j].StepProCov_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* ʹ�� */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* ʧ�� */
				}					
            }
            break;

            case SCMD_WORKSTEP_PROT_VSUPERDEV:                  // ����������ѹ����ʹ��
            case SCMD_WORKSTEP_PROT_VSUPERDEV | SCMD_DISABLE:   // ����������ѹ����ʧ��
            {
                p = &Tier2.workstepAnalyze[j].StepProVsuperdev;
                q = &Tier2.workstepAnalyze[j].StepProVsuperdev_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* ʹ�� */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* ʧ�� */
				}					
            }
            break;

            case SCMD_WORKSTEP_PROT_ISUPERDEV:                  // ����������������ʹ��
            case SCMD_WORKSTEP_PROT_ISUPERDEV | SCMD_DISABLE:   // ����������������ʧ��
            {
                p = &Tier2.workstepAnalyze[j].StepProIsuperdev;
                q = &Tier2.workstepAnalyze[j].StepProIsuperdev_En;
				
				if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_PROT_BEGIN) && \
					(Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_PROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 1;     /* ʹ�� */
				}
				else if( (Tier2.workstep[j].ProSeg[i].type > SCMD_WORKSTEP_NPROT_BEGIN) && \
						 (Tier2.workstep[j].ProSeg[i].type < SCMD_WORKSTEP_NPROT_END) )
				{
					*p = Tier2.workstep[j].ProSeg[i].data;
					*q = 0;     /* ʧ�� */
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
**  �� �� �� : Workstep_Analysis_Func()
**  ����˵�� : ������������
**  ��    �� : ��
**  �� �� ֵ : ��
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
                case WORKSTEP_RUNSTA_SET_STOP:          // ֹͣ
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

                case WORKSTEP_RUNSTA_SET_PAUSE:         // ��ͣ
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

                case WORKSTEP_RUNSTA_SET_START:         // ����
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
						/* �Ӵ����ʹ�� */
						if(((Tier2.gProt[j].Tcontact.isEnable & 0x80) == 0) && ((Tier2.gProt[j].Icontact.isEnable & 0x80) == 0))						
						{
							VcontactPre[j] = Tier2.chRecord[j].Vbat;
							
							/*��ʼ��ѹ���޼��*/
							if((Tier2.gProt[j].VinitThrH.isEnable & 0x80) == 0)
							{
								if(Tier2.record[j].Vbat > Tier2.gProt[j].VinitThrH.value)
								{
									GP_Alarm_Flag[j] |= GlobalProtect_MV;
								}
							}
							
                            /*��ʼ��ѹ���޼��*/
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

                case WORKSTEP_RUNSTA_SET_CONTINUE:      // ����
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

                case WORKSTEP_RUNSTA_SET_SKIP:          // ��ת
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
**  �� �� �� : Workstep_Switch_Func()
**  ����˵�� : �����л�
**  ��    �� : chx - ͨ��
**  �� �� ֵ : 0 - �ɹ�������ʧ��
**********************************************************************************************************/
int16_t Workstep_Switch_Func(uint8_t chx)
{
	uint8_t j = chx;
	
	if(j >= PORT_NO8)		// �ڲ�ͨ���ų�����Χ
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
			if(time_over_stop[j] == 0)		//����ʱ���ֹ��֡
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
**  �� �� �� : Workstep_Abort_Func()
**  ����˵�� : ����ֹͣ
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void Workstep_Abort_Func(void)
{
    for(int j = 0; j < CHANNEL_USE; j++)
    {
        float run_time = Tier2.workstepRunning[j].Trun / RUN_TIME_PERIOD;   // ����ʱ��
        float abt_time = Tier2.workstepAnalyze[j].StepAbtTime;              // ��ֹʱ��
        float run_vol  = Tier2.chRecord[j].Vbat;                            // ���е�ѹ
        float abt_vol  = Tier2.workstepAnalyze[j].StepAbtVol;               // ��ֹ��ѹ
        float run_cur  = Tier2.chRecord[j].Iout;                            // ���е���
        float abt_cur  = Tier2.workstepAnalyze[j].StepAbtCur;               // ��ֹ����
        float run_cap  = Tier2.chRecord[j].Cbat;                  			// ��������
        float abt_cap  = Tier2.workstepAnalyze[j].StepAbtCap;               // ��ֹ����

        if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
        {
            switch(Tier2.workstepRunning[j].StepType)
            {
                case SCMD_WORKSTEP_TYPE_STANDBY:        /* ���� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
                    {
						time_over_stop[j] = 1;
                        Workstep_Switch_Func(j);
						
//						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_TIME;
                    }
                }
                break;

                case SCMD_WORKSTEP_TYPE_CC:             /* ������� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_vol > 0) && (run_vol >= abt_vol))	/* ���е�ѹ > ��ֹ��ѹ */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_CV:             /* ��ѹ��� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_CCTOCV:		/* ��ѹ������� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_DCC:            /* �����ŵ� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_vol > 0) && (run_vol <= abt_vol))	/* ���е�ѹ < ��ֹ��ѹ */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_DCV:            /* ��ѹ�ŵ� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_DCCTODCV:           /* ��ѹ�����ŵ� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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
				
				if(run_time >= abt_time)	/* ����ʱ�� > ��ֹʱ�� */
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
**  �� �� �� : CCtoCVConvertFunc()
**  ����˵�� : CCģʽ�л�CVģʽ
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void CCtoCVConvertFunc(void)
{
	if(g_ucSysMode == SYS_NORMAL_MODE)
	{
		for(int j = 0; j < CHANNEL_USE; j++)    // �ж�ÿ��ͨ���Ƿ���¿�������
		{
			if((Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && (workstep_flag_CriticalZone[j] < 3))
			{
				if( (Tier2.workstepRunning[j].StepType == SCMD_WORKSTEP_TYPE_CV    ) || \
					(Tier2.workstepRunning[j].StepType == SCMD_WORKSTEP_TYPE_CCTOCV) )
				{
					if( (Tier2.workstepAnalyze[j].StepRunCur - Tier2.chRecord[j].Iout       >= IN_CV_I_LIMIT) && \
						(fabs(Tier2.workstepAnalyze[j].StepRunVol - Tier2.chRecord[j].Vbat) <= CC_TO_CV_V_LINIT) )			// ����CV��̬
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
							 (workstep_flag_CriticalZone[j] == 0) )															// ����CC������̬
					{
						workstep_flag_CriticalZone[j] = 1;
						cv_flag_cnt[j] = 0;
					}					
					else if( (Tier2.workstepAnalyze[j].StepRunCur - Tier2.chRecord[j].Iout       < CC_TO_CV_I_LIMIT) || \
							 (fabs(Tier2.workstepAnalyze[j].StepRunVol - Tier2.chRecord[j].Vbat) > CC_TO_CV_V_LINIT) )		// �ָ���ʼֵ
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
**  �� �� �� : AppTaskWorkstep()
**  ����˵�� : ���������߳�
**  ��    �� : *argument - ����
**  �� �� ֵ : ��
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
			SysTempSamp(TempSampCHx);	//�豸�¶Ȳ���
		}
    osDelay(WORKSTEP_TASK_PERIOD);
  }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
