/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Workstep.c
** Author      : Cai Heng
** Date        : 2022-07-26
** Description : 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "Task_Workstep.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
//uint8_t  SysModeProtMaskFlag = 0;			// 故障屏蔽标识(0-不屏蔽故障 1-屏蔽故障)
uint8_t  AcDumpConnectFlag = 1;				// 掉电接续标识，首次上电，即使有故障，也不复位工步数据
uint8_t  ContactCheckFlag[8] = {0};			// 接触测试标识，0 - 无接触测试， 1 - 有接触测试
uint8_t  start_state_flag[8] = {0};
uint8_t  contact_test_once_flag[8] = {0};
uint8_t  record_data_upcast_flag[8] = {0};	// 数据上抛标识
uint8_t  time_over_stop[8] = {0};
uint8_t  abort_type_flag[8] = {0};
uint8_t  Protect_Flag_Global[CHANNEL_MAX] = {0};
uint8_t  Protect_Flag_Internal[CHANNEL_MAX] = {0};
uint8_t  Protect_Flag_Workstep[CHANNEL_MAX] = {0};
uint8_t  Protect_Flag_ACDC[CHANNEL_MAX] = {0};
uint8_t  protect_flag_upcast[8] = {0};
uint8_t  DeviceStateFlag[CHANNEL_MAX] = {0};	// 设备当前状态 0 - Stop; 1 - Start
uint8_t  DeviceStateFlagPre[CHANNEL_MAX] = {0};	// 设备上一状态
uint8_t  ProtectMaskFlag[CHANNEL_MAX] = {0};	// 故障屏蔽标识 0 - No Mask; 1 - Mask
uint32_t ProtectMaskCnt[CHANNEL_MAX] = {0};		// 故障屏蔽计数
uint8_t  workstep_flag_AbortPrio[8] = {1, 1, 1, 1, 1, 1, 1, 1};
uint8_t  dcdc_on_off_state = 0;					// 0 - dcdc on; 1 - dcdc off
uint16_t workstep_flag_ErrWord[8] = {0};
uint64_t run_time_cnt[8] = {0};
float VcontactPre[CHANNEL_MAX];
float VcontactCur[CHANNEL_MAX];

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : BatteryFormattingStop()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int16_t BatteryFormattingStop(uint8_t chx)
{
	//运行停止
	DcdcRunStaSet(DEBUG_CH, STOP_STATUS);
	CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
	osDelay(100);	
	
	dcdc_on_off_state = 0;
	
	return 0;
}

/**********************************************************************************************************
**  函 数 名 : BatteryFormattingProcess()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int16_t BatteryFormattingProcess(uint8_t chx, uint8_t dir, uint8_t vmod, float v_target, float i_target)
{	
	return 0;
}

/**********************************************************************************************************
**  函 数 名 : Protect_Action_Func()
**  功能说明 :
**  形    参 :
**  返 回 值 :
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
			record_data_upcast_flag[j] = 1;
			osDelay(1);
		}

		if(AcDumpConnectFlag == 0)
		{
			Tier2.workstepRunning[j].RunSta = sta; 
		}

		BatteryFormattingStop(j);

		Tier2.chRecord[j].Cbat = 0;		//20220506新增,避免故障停机时容量及能量不清除的异常
		Tier2.chRecord[j].Ebat = 0;		
		
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
**  函 数 名 : Workstep_Protect_Func()
**  功能说明 : 
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int16_t Workstep_Protect_Func(uint8_t chx)
{
    uint8_t j = chx;
	
	if(dcdc_err_flag == 1)
	{
		dcdc_err_flag = 0;
		
		SET_1(workstep_flag_ErrWord[0], 4);
		SET_1(workstep_flag_ErrWord[1], 4);
		SET_1(workstep_flag_ErrWord[2], 4);
		SET_1(workstep_flag_ErrWord[3], 4);
		SET_1(workstep_flag_ErrWord[4], 4);
		SET_1(workstep_flag_ErrWord[5], 4);
		SET_1(workstep_flag_ErrWord[6], 4);
		SET_1(workstep_flag_ErrWord[7], 4);
		
		Protect_Action_Func(0, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);	
	}	
	
	if( (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_START  ) || \
		(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
		(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) )
	{
		/* 电压上限保护 */
		if(Tier2.workstepAnalyze[j].StepProVlimitH_En == WORKSTEP_PROTECT_ENABLE)
		{
			if(ProtectMaskFlag[j] != 1)
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
		}
		
		/* 电压下限保护 */
		if(Tier2.workstepAnalyze[j].StepProVlimitL_En == WORKSTEP_PROTECT_ENABLE)
		{
			if((Tier2.workstepRunning[j].RunSta != WORKSTEP_RUNSTA_REF_CONTACT) && (ProtectMaskFlag[j] != 1))
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
			/* do nothing */
		}
		
		/* 过流保护 */
		if(Tier2.workstepAnalyze[j].StepProIov_En == WORKSTEP_PROTECT_ENABLE)
		{
			if((Tier2.workstepRunning[j].StepType != SCMD_WORKSTEP_TYPE_STANDBY) && (ProtectMaskFlag[j] != 1))
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
			if( (ProtectMaskFlag[j] != 1) && \
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

	return 0;
}

/**********************************************************************************************************
**  函 数 名 : Workstep_Execute_Func()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int16_t Workstep_Execute_Func(uint8_t chx)
{
    uint8_t j = chx;
	
//	BatteryFormattingStop(j);
	
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
			
//			BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
			
			if(dcdc_on_off_state == 0)
			{
				//DCDC模式设置
				DcdcRunModeSet(DEBUG_CH, CTRL_RUN_CC_MODE);
				CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
				osDelay(100);	
				
				if((Tier2.gProt[j].VbatThrH.isEnable & 0x80) == 0)
				{
					//DCDC限制电压电流设置
					DcdcProtectSet(DEBUG_CH, PROTECT_LIMIT_VOL, Tier2.gProt[j].VbatThrH.isEnable);     //电压上限保护值
					CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
					osDelay(100);
				}
				
				if((Tier2.gProt[j].Iovcharge.isEnable & 0x80) == 0)
				{
					DcdcProtectSet(DEBUG_CH, PROTECT_LIMIT_CURT, Tier2.gProt[j].Iovcharge.value);        //过流保护值
					CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
					osDelay(100);	
				}					
				
				//DCDC额定电压电流设置
				DcdcCtrlParaSet(DEBUG_CH, CTRL_RATE_VOL, Tier2.workstepAnalyze[j].StepRunVol);
				CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
				osDelay(100);
				DcdcCtrlParaSet(DEBUG_CH, CTRL_RATE_CURT, Tier2.workstepAnalyze[j].StepRunCur);
				CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
				osDelay(100);
				
				
				//开始运行
				DcdcRunStaSet(DEBUG_CH, RUN_STATUS);
				CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
				osDelay(100);	
				
				dcdc_on_off_state = 1;
			}				
			
        }
        break;

        case SCMD_WORKSTEP_TYPE_CV:             // 恒压充电
        {
			time_over_stop[j] = 0;
			
//			BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_CCTOCV:         // 恒流转恒压充电
        {
			time_over_stop[j] = 0;
			
//			BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCC:            // 恒流放电
        {
			time_over_stop[j] = 0;
			
//			BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCV:            // 恒压放电
        {
			time_over_stop[j] = 0;
			
//			BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCCTODCV:       // 恒流转恒压放电
        {
			time_over_stop[j] = 0;
			
//			BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
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
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int16_t Workstep_Reload_Func(uint8_t chx)
{
    uint8_t j = chx;
	
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
		switch(Tier2.workstep[j].RunSeg[i].type)
		{
			case SCMD_WORKSTEP_PARAM_VOLTAGE:		// 工步运行电压
			{
				Tier2.workstepAnalyze[j].StepRunVol = Tier2.workstep[j].RunSeg[i].data;
			}
			break;
			
			case SCMD_WORKSTEP_PARAM_CURRENT:		// 工步运行电流
			{
				Tier2.workstepAnalyze[j].StepRunCur = Tier2.workstep[j].RunSeg[i].data;
			}
			break;
			
			default :								// 预留
			{
				/* do nothing */
			}
			break;;
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

    if(Tier2.workstepAnalyze[j].StepType == SCMD_WORKSTEP_TYPE_CC)
    {
//		if(Tier2.workstepAnalyze[j].StepAbtVol > WORK_VOL_MAX)
//		{
//			Tier2.workstepAnalyze[j].StepRunVol = WORK_VOL_MAX + 0.1f;
//		}
//		else if(Tier2.workstepAnalyze[j].StepAbtVol < WORK_VOL_MIN)
//		{
//			Tier2.workstepAnalyze[j].StepRunVol = WORK_VOL_MIN + 0.1f;
//		}
//		else
//		{
//			Tier2.workstepAnalyze[j].StepRunVol = Tier2.workstepAnalyze[j].StepAbtVol + 0.1f;
//		}
    }
    else if(Tier2.workstepAnalyze[j].StepType == SCMD_WORKSTEP_TYPE_DCC)
	{
//		if(Tier2.workstepAnalyze[j].StepAbtVol > WORK_VOL_MAX)
//		{
//			Tier2.workstepAnalyze[j].StepRunVol = WORK_VOL_MAX - 0.1f;
//		}
//		else if(Tier2.workstepAnalyze[j].StepAbtVol < WORK_VOL_MIN)
//		{
//			Tier2.workstepAnalyze[j].StepRunVol = WORK_VOL_MIN - 0.1f;
//		}
//		else
//		{
//			Tier2.workstepAnalyze[j].StepRunVol = Tier2.workstepAnalyze[j].StepAbtVol - 0.1f;
//		}
	}
	else if( (Tier2.workstepAnalyze[j].StepType == SCMD_WORKSTEP_TYPE_CV) || \
			 (Tier2.workstepAnalyze[j].StepType == SCMD_WORKSTEP_TYPE_DCV) )
	{
		if(Tier2.workstepAnalyze[j].StepRunCur == 0)
		{
			Tier2.workstepAnalyze[j].StepRunCur = WORK_CUR_MAX;
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
**  函 数 名 : Workstep_Switch_Func()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
int16_t Workstep_Switch_Func(uint8_t chx)
{
    uint8_t j = chx;
	if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
	{
		if(Workstep_Reload_Func(j) == -1)  //工步缓存中没有数据，直接进入等待状态
		{
			if(time_over_stop[j] != 1)
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
		else  //工步缓存
		{
			if(time_over_stop[j] != 1)		//避免时间截止重帧
			{
				record_data_upcast_flag[j] = 1;
				osDelay(1);
			}
			Workstep_Execute_Func(j);
		}		
	}
	else if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_WAIT)
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
**  函 数 名 : Workstep_Analysis_Func()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
void Workstep_Analysis_Func(void)
{
	uint8_t workstep_flag_RunstaClr = 0;
	
    for(int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;

        if(workstep_flag_RunstaSet & umask)
        {
			workstep_flag_RunstaClr |= umask;
			
            switch(Tier2.workstepControl[j].StepType)
            {
                case WORKSTEP_RUNSTA_SET_STOP:          // 停止
                {
					ContactCheckFlag[j] = 0;
					contact_test_once_flag[j] = 0;
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
                        Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_CONTACT;		// 接触测试
									
						/*初始电压上限检测*/
						if((Tier2.gProt[j].VinitThrH.isEnable & 0x80) == 0)
						{
							if(Tier2.record[j].Vbat > Tier2.gProt[j].VinitThrH.value)
							{
//								GP_Alarm_Flag[j] |= GlobalProtect_MV;
							}
						}

						/*初始电压下限检测*/
						if((Tier2.gProt[j].VinitThrL.isEnable & 0x80) == 0)
						{
							if(Tier2.record[j].Vbat < Tier2.gProt[j].VinitThrL.value)
							{
//								GP_Alarm_Flag[j] |= GlobalProtect_LV;
							}
						}

//						if(((GP_Alarm_Flag[j] & GlobalProtect_MV) != 0) || ((GP_Alarm_Flag[j] & GlobalProtect_LV) != 0))
						if(0)
						{
							Tier2.workstepControl[j].StepType = 0xFF;
							SET_0(workstep_flag_RunstaSet, j);
							break;								
						}						

						/* 接触检测使能 */
						if(((Tier2.gProt[j].Tcontact.isEnable & 0x80) == 0) && ((Tier2.gProt[j].Icontact.isEnable & 0x80) == 0))						
						{
							VcontactPre[j] = Tier2.chRecord[j].Vbat;

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
						
//						if(dcdc_on_off_state == 0)
//						{
//							//DCDC模式设置
//							DcdcRunModeSet(DEBUG_CH, CTRL_RUN_CC_MODE);
//							CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
//							osDelay(100);	
//							
//							//DCDC限制电压电流设置
//							DcdcProtectSet(DEBUG_CH, PROTECT_LIMIT_VOL, Tier2.workstepAnalyze[j].StepProVlimitH);     //电压上限保护值
//							CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
//							osDelay(100);
//							DcdcProtectSet(DEBUG_CH, PROTECT_LIMIT_CURT, Tier2.workstepAnalyze[j].StepProIov);        //过流保护值
//							CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
//							osDelay(100);					
//							
//							//DCDC额定电压电流设置
//							DcdcCtrlParaSet(DEBUG_CH, CTRL_RATE_VOL, Tier2.workstepAnalyze[j].StepRunVol);
//							CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
//							osDelay(100);
//							DcdcCtrlParaSet(DEBUG_CH, CTRL_RATE_CURT, Tier2.workstepAnalyze[j].StepRunCur);
//							CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
//							osDelay(100);
//						}
	
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
	workstep_flag_RunstaSet ^= workstep_flag_RunstaClr;
}

/**********************************************************************************************************
**  函 数 名 : Workstep_Abort_Func()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
#ifndef SW_UNIT
void Workstep_Abort_Func(void)		// 统一截止（适用于截止方案）
{
	uint8_t abort_type_tmp = 0;		// 0-无截止、1-时间截止、2-电压截止、3-电流截止、4-容量截止
	uint8_t abort_info_tmp = 0;		// 0-无异常、1-时间截止异常、2-电压截止异常、3-电流截止异常、4-容量截止截止
	
    for(int j = 0; j < CHANNEL_CFG; j++)
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
//						time_over_stop[j] = 1;
//						
//                      Workstep_Switch_Func(j);

						abort_type_tmp = 1;
						abort_info_tmp = 0;
                    }
                }
                break;

                case SCMD_WORKSTEP_TYPE_CC:             /* 恒流充电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						if(time_over_stop[j] == 2)  //避免时间截止重帧
						{
							break;
						}
						
						time_over_stop[j] = 1;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)))  //判断当前截止条件优先级是否为最高
                        {
//							SET_0(workstep_flag_ErrWord[j], 6);
//							
//							Workstep_Switch_Func(j);

							abort_type_tmp = 1;
							abort_info_tmp = 0;
                        }
                        else  //当前截止条件优先级非最高优先级
                        {
//							SET_1(workstep_flag_ErrWord[j], 6);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 1;
							abort_info_tmp = 1;
                        }
											
                    }
					else if((abt_vol > 0) && (run_vol >= abt_vol))	/* 运行电压 > 截止电压 */
					{
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_VOLTAGE;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_VOLTAGE - 1)))
						{
//							SET_0(workstep_flag_ErrWord[j], 7);
//							
//							Workstep_Switch_Func(j);
							
							abort_type_tmp = 2;
							abort_info_tmp = 0;							
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 7);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 2;
							abort_info_tmp = 2;								
						}
						
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
//							SET_0(workstep_flag_ErrWord[j], 9);
//							
//							Workstep_Switch_Func(j);
							
							abort_type_tmp = 4;
							abort_info_tmp = 0;								
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 9);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 4;
							abort_info_tmp = 4;								
						}
											
					}
                }
                break;

                case SCMD_WORKSTEP_TYPE_CV:             /* 恒压充电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						if(time_over_stop[j] == 2)
						{
							break;
						}						
						
						time_over_stop[j] = 1;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)) )
                        {
//							SET_0(workstep_flag_ErrWord[j], 6);
//							
//                          Workstep_Switch_Func(j);
							
							abort_type_tmp = 1;
							abort_info_tmp = 0;								
                        }
                        else
                        {
//                          SET_1(workstep_flag_ErrWord[j], 6);
//							
//                          Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 1;
							abort_info_tmp = 1;								
                        }
                    }
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* 运行电流 < 截止电流 */
					{
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CURRENT;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CURRENT - 1)) )
						{
//							SET_0(workstep_flag_ErrWord[j], 8);
//
//							Workstep_Switch_Func(j);

							abort_type_tmp = 3;
							abort_info_tmp = 0;								
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 8);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 3;
							abort_info_tmp = 3;								
						}
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
//							SET_0(workstep_flag_ErrWord[j], 9);
//							
//							Workstep_Switch_Func(j);
							
							abort_type_tmp = 4;
							abort_info_tmp = 0;								
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 9);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 4;
							abort_info_tmp = 4;								
						}
					}
                }
                break;

                case SCMD_WORKSTEP_TYPE_CCTOCV:			/* 恒压恒流充电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						if(time_over_stop[j] == 2)
						{
							break;
						}						
						
						time_over_stop[j] = 1;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)))
                        {
//							SET_0(workstep_flag_ErrWord[j], 6);
//							
//                          Workstep_Switch_Func(j);
							
							abort_type_tmp = 1;
							abort_info_tmp = 0;
                        }
                        else
                        {
//                          SET_1(workstep_flag_ErrWord[j], 6);
//							
//                          Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 1;
							abort_info_tmp = 1;
                        }
                    }
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* 运行电流 < 截止电流 */
					{
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CURRENT;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CURRENT - 1)))
						{
//							SET_0(workstep_flag_ErrWord[j], 8);
//
//							Workstep_Switch_Func(j);
							
							abort_type_tmp = 3;
							abort_info_tmp = 0;
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 8);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 3;
							abort_info_tmp = 3;
						}
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
//							SET_0(workstep_flag_ErrWord[j], 9);
//							
//							Workstep_Switch_Func(j);
							
							abort_type_tmp = 4;
							abort_info_tmp = 0;
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 9);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 4;
							abort_info_tmp = 4;
						}
					}
                }
                break;

                case SCMD_WORKSTEP_TYPE_DCC:            /* 恒流放电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						if(time_over_stop[j] == 2)
						{
							break;
						}
						
						time_over_stop[j] = 1;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)))
                        {
//							SET_0(workstep_flag_ErrWord[j], 6);
//							
//                          Workstep_Switch_Func(j);
							
							abort_type_tmp = 1;
							abort_info_tmp = 0;
                        }
                        else
                        {
//                          SET_1(workstep_flag_ErrWord[j], 6);
//							
//                          Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 1;
							abort_info_tmp = 1;
                        }
                    }
					else if((abt_vol > 0) && (run_vol <= abt_vol))	/* 运行电压 < 截止电压 */
					{
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_VOLTAGE;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_VOLTAGE - 1)))
						{
//							SET_0(workstep_flag_ErrWord[j], 7);
//							
//							Workstep_Switch_Func(j);

							abort_type_tmp = 2;
							abort_info_tmp = 0;
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 7);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 2;
							abort_info_tmp = 2;
						}
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{		
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
//							SET_0(workstep_flag_ErrWord[j], 9);
//							
//							Workstep_Switch_Func(j);
							
							abort_type_tmp = 4;
							abort_info_tmp = 0;
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 9);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 4;
							abort_info_tmp = 4;
						}
					}
                }
                break;

                case SCMD_WORKSTEP_TYPE_DCV:            /* 恒压放电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						if(time_over_stop[j] == 2)
						{
							break;
						}
						
						time_over_stop[j] = 1;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)))
                        {
//							SET_0(workstep_flag_ErrWord[j], 6);
//							
//                          Workstep_Switch_Func(j);
							
							abort_type_tmp = 1;
							abort_info_tmp = 0;
                        }
                        else
                        {
//                          SET_1(workstep_flag_ErrWord[j], 6);
//							
//                          Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 1;
							abort_info_tmp = 1;
                        }
                    }
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* 运行电流 < 截止电流 */
					{	
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CURRENT;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CURRENT - 1)))
						{
//							SET_0(workstep_flag_ErrWord[j], 8);
//
//							Workstep_Switch_Func(j);
							
							abort_type_tmp = 3;
							abort_info_tmp = 0;
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 8);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 3;
							abort_info_tmp = 3;
						}
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
//							SET_0(workstep_flag_ErrWord[j], 9);
//							
//							Workstep_Switch_Func(j);
							
							abort_type_tmp = 4;
							abort_info_tmp = 0;
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 9);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 4;
							abort_info_tmp = 4;
						}
					}
				}
                break;

                case SCMD_WORKSTEP_TYPE_DCCTODCV:		/* 恒压恒流放电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {	
						if(time_over_stop[j] == 2)
						{
							break;
						}						
						
						time_over_stop[j] = 1;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)))
                        {
//							SET_0(workstep_flag_ErrWord[j], 6);
//							
//                           Workstep_Switch_Func(j);
							
							abort_type_tmp = 1;
							abort_info_tmp = 0;
                        }
                        else
                        {
//                           SET_1(workstep_flag_ErrWord[j], 6);
//							
//                           Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 1;
							abort_info_tmp = 1;
                        }
                    }
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* 运行电流 < 截止电流 */
					{	
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CURRENT;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CURRENT - 1)))
						{
//							SET_0(workstep_flag_ErrWord[j], 8);
//
//							Workstep_Switch_Func(j);
							
							abort_type_tmp = 3;
							abort_info_tmp = 0;
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 8);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 3;
							abort_info_tmp = 3;
						}
					}
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* 运行容量 > 截止容量 */
					{
						time_over_stop[j] = 2;
						
						abort_type_flag[j] = SCMD_WORKSTEP_PARAM_CAPACITY;
						
						if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_CAPACITY - 1)))
						{
//							SET_0(workstep_flag_ErrWord[j], 9);
//							
//							Workstep_Switch_Func(j);
							
							abort_type_tmp = 4;
							abort_info_tmp = 0;
						}
						else
						{
//							SET_1(workstep_flag_ErrWord[j], 9);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 4;
							abort_info_tmp = 4;
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
			osDelay(50);
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
	
	if(abort_type_tmp == 1)
	{
		if(abort_info_tmp == 0)
		{
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				osDelay(50);
				SET_0(workstep_flag_ErrWord[j], 6);
				Workstep_Switch_Func(j);
			}		
		}
		else
		{
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				osDelay(50);
				SET_1(workstep_flag_ErrWord[j], 6);
				Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);	
			}			
		}
	}
	else if(abort_type_tmp == 2)
	{
		if(abort_info_tmp == 0)
		{
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				osDelay(50);
				SET_0(workstep_flag_ErrWord[j], 7);
				Workstep_Switch_Func(j);
			}	
		}
		else
		{
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				osDelay(50);
				SET_1(workstep_flag_ErrWord[j], 7);
				Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);	
			}			
		}	
	}
	else if(abort_type_tmp == 3)
	{
		if(abort_info_tmp == 0)
		{
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				osDelay(50);
				SET_0(workstep_flag_ErrWord[j], 8);
				Workstep_Switch_Func(j);
			}	
		}
		else
		{
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				osDelay(50);
				SET_1(workstep_flag_ErrWord[j], 8);
				Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);	
			}			
		}	
	}
	else if(abort_type_tmp == 4)
	{
		if(abort_info_tmp == 0)
		{
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				osDelay(50);
				SET_0(workstep_flag_ErrWord[j], 9);
				Workstep_Switch_Func(j);
			}	
		}
		else
		{
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				osDelay(50);
				SET_1(workstep_flag_ErrWord[j], 9);
				Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);	
			}			
		}		
	}	
}
#else
void Workstep_Abort_Func(void)		// 独立截止（适用于切换方案）
{
    for(int j = 0; j < CHANNEL_CFG; j++)
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
                    }
                }
                break;

                case SCMD_WORKSTEP_TYPE_CC:             /* 恒流充电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						if(time_over_stop[j] == 2)
						{
							break;
						}
						
						time_over_stop[j] = 1;
						
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
						time_over_stop[j] = 2;
						
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
						time_over_stop[j] = 2;
						
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
						if(time_over_stop[j] == 2)
						{
							break;
						}						
						
						time_over_stop[j] = 1;
						
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
						time_over_stop[j] = 2;
						
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
						time_over_stop[j] = 2;
						
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

                case SCMD_WORKSTEP_TYPE_CCTOCV:			/* 恒压恒流充电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {
						if(time_over_stop[j] == 2)
						{
							break;
						}						
						
						time_over_stop[j] = 1;
						
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
						time_over_stop[j] = 2;
						
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
						time_over_stop[j] = 2;
						
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
						if(time_over_stop[j] == 2)
						{
							break;
						}
						
						time_over_stop[j] = 1;
						
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
						time_over_stop[j] = 2;
						
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
						time_over_stop[j] = 2;
						
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
						if(time_over_stop[j] == 2)
						{
							break;
						}
						
						time_over_stop[j] = 1;
						
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
						time_over_stop[j] = 2;
						
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
						time_over_stop[j] = 2;
						
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

                case SCMD_WORKSTEP_TYPE_DCCTODCV:		/* 恒压恒流放电 */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* 运行时间 > 截止时间 */
                    {	
						if(time_over_stop[j] == 2)
						{
							break;
						}						
						
						time_over_stop[j] = 1;
						
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
						time_over_stop[j] = 2;
						
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
						time_over_stop[j] = 2;
						
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
			osDelay(50);
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

					contact_test_once_flag[j] = 0;

					BatteryFormattingStop(j);
					
					VcontactCur[j] = Tier2.chRecord[j].Vbat;
					
//					if((Tier2.gProt[j].VcontactDiff.isEnable & 0x80) == 0)
//					{
//						if((VcontactCur[j] - VcontactPre[j]) < Tier2.gProt[j].VcontactDiff.value)
//						{
//							GP_Alarm_Flag[j] |= IntelProtect_VCV;
//						}
//						else if(((GP_Alarm_Flag[j] & GlobalProtect_MV) == 0) && ((GP_Alarm_Flag[j] & GlobalProtect_LV) == 0))
//						{
//							GP_Alarm_Flag[j] &= ~(IntelProtect_VCV);
//							
//							SET_1(workstep_flag_RunstaSet, j);
//							
//							Tier2.workstepControl[j].StepType = WORKSTEP_RUNSTA_SET_START;
//						}
//					}
//					else if(((GP_Alarm_Flag[j] & GlobalProtect_MV) == 0) && ((GP_Alarm_Flag[j] & GlobalProtect_LV) == 0))
//					{
//						SET_1(workstep_flag_RunstaSet, j);
//						
//						Tier2.workstepControl[j].StepType = WORKSTEP_RUNSTA_SET_START;
//					}
					
				}
			}
		}
    }
}
#endif

void channel_runsta_sync()
{
	uint8_t err_stop = 0;
	uint8_t wait_stop = 0;
	
	for(int i = 0; i < 8; i++)
	{
		if(Tier2.record[i].ErrType != 0)
		{
			err_stop = 1;
			
			break;
		}
	}
	
	if(err_stop == 1)
	{
		for(int j = 0; j < CHANNEL_CFG; j++)
		{
			if(Tier2.workstepRunning[j].RunSta != WORKSTEP_RUNSTA_REF_STOP)
			{
				osDelay(50);
				
				Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);	
			}
		}
		
		return;
	}
	
	for(int i = 0; i < 8; i++)
	{
		if(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_WAIT)
		{
			wait_stop = 1;
			
			break;
		}
	}
	
	if(wait_stop == 1)
	{
		for(int j = 0; j < CHANNEL_CFG; j++)
		{
			if(Tier2.workstepRunning[j].RunSta != WORKSTEP_RUNSTA_REF_WAIT)
			{
				osDelay(50);
				
				Workstep_Switch_Func(j);
			}
		}
		
		return;
	}	
	
}


/**********************************************************************************************************
**  函 数 名 : AppTaskWorkstep()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
void AppTaskWorkstep(void *argument)
{
	while(1)
	{
		Workstep_Analysis_Func();

		Workstep_Abort_Func();

		for(int j = 0; j < 8; j++)
		{
			Workstep_Protect_Func(j);
		}

		channel_runsta_sync();
		
		osDelay(WORKSTEP_TASK_PERIOD);
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
