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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Task_Workstep.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
//uint8_t  SysModeProtMaskFlag = 0;			// �������α�ʶ(0-�����ι��� 1-���ι���)
uint8_t  AcDumpConnectFlag = 1;				// ���������ʶ���״��ϵ磬��ʹ�й��ϣ�Ҳ����λ��������
uint8_t  ContactCheckFlag[8] = {0};			// �Ӵ����Ա�ʶ��0 - �޽Ӵ����ԣ� 1 - �нӴ�����
uint8_t  start_state_flag[8] = {0};
uint8_t  contact_test_once_flag[8] = {0};
uint8_t  record_data_upcast_flag[8] = {0};	// �������ױ�ʶ
uint8_t  time_over_stop[8] = {0};
uint8_t  abort_type_flag[8] = {0};
uint8_t  Protect_Flag_Global[CHANNEL_MAX] = {0};
uint8_t  Protect_Flag_Internal[CHANNEL_MAX] = {0};
uint8_t  Protect_Flag_Workstep[CHANNEL_MAX] = {0};
uint8_t  Protect_Flag_ACDC[CHANNEL_MAX] = {0};
uint8_t  protect_flag_upcast[8] = {0};
uint8_t  DeviceStateFlag[CHANNEL_MAX] = {0};	// �豸��ǰ״̬ 0 - Stop; 1 - Start
uint8_t  DeviceStateFlagPre[CHANNEL_MAX] = {0};	// �豸��һ״̬
uint8_t  ProtectMaskFlag[CHANNEL_MAX] = {0};	// �������α�ʶ 0 - No Mask; 1 - Mask
uint32_t ProtectMaskCnt[CHANNEL_MAX] = {0};		// �������μ���
uint8_t  workstep_flag_AbortPrio[8] = {1, 1, 1, 1, 1, 1, 1, 1};
uint8_t  dcdc_on_off_state = 0;					// 0 - dcdc on; 1 - dcdc off
uint16_t workstep_flag_ErrWord[8] = {0};
uint64_t run_time_cnt[8] = {0};
float VcontactPre[CHANNEL_MAX];
float VcontactCur[CHANNEL_MAX];

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : BatteryFormattingStop()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
int16_t BatteryFormattingStop(uint8_t chx)
{
	//����ֹͣ
	DcdcRunStaSet(DEBUG_CH, STOP_STATUS);
	CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
	osDelay(100);	
	
	dcdc_on_off_state = 0;
	
	return 0;
}

/**********************************************************************************************************
**  �� �� �� : BatteryFormattingProcess()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
int16_t BatteryFormattingProcess(uint8_t chx, uint8_t dir, uint8_t vmod, float v_target, float i_target)
{	
	return 0;
}

/**********************************************************************************************************
**  �� �� �� : Protect_Action_Func()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
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

		Tier2.chRecord[j].Cbat = 0;		//20220506����,�������ͣ��ʱ������������������쳣
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
**  �� �� �� : Workstep_Protect_Func()
**  ����˵�� : 
**  ��    �� :
**  �� �� ֵ :
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
		/* ��ѹ���ޱ��� */
		if(Tier2.workstepAnalyze[j].StepProVlimitH_En == WORKSTEP_PROTECT_ENABLE)
		{
			if(ProtectMaskFlag[j] != 1)
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
		}
		
		/* ��ѹ���ޱ��� */
		if(Tier2.workstepAnalyze[j].StepProVlimitL_En == WORKSTEP_PROTECT_ENABLE)
		{
			if((Tier2.workstepRunning[j].RunSta != WORKSTEP_RUNSTA_REF_CONTACT) && (ProtectMaskFlag[j] != 1))
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
			/* do nothing */
		}
		
		/* �������� */
		if(Tier2.workstepAnalyze[j].StepProIov_En == WORKSTEP_PROTECT_ENABLE)
		{
			if((Tier2.workstepRunning[j].StepType != SCMD_WORKSTEP_TYPE_STANDBY) && (ProtectMaskFlag[j] != 1))
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
			if( (ProtectMaskFlag[j] != 1) && \
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

	return 0;
}

/**********************************************************************************************************
**  �� �� �� : Workstep_Execute_Func()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
int16_t Workstep_Execute_Func(uint8_t chx)
{
    uint8_t j = chx;
	
//	BatteryFormattingStop(j);
	
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
			
//			BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
			
			if(dcdc_on_off_state == 0)
			{
				//DCDCģʽ����
				DcdcRunModeSet(DEBUG_CH, CTRL_RUN_CC_MODE);
				CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
				osDelay(100);	
				
				if((Tier2.gProt[j].VbatThrH.isEnable & 0x80) == 0)
				{
					//DCDC���Ƶ�ѹ��������
					DcdcProtectSet(DEBUG_CH, PROTECT_LIMIT_VOL, Tier2.gProt[j].VbatThrH.isEnable);     //��ѹ���ޱ���ֵ
					CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
					osDelay(100);
				}
				
				if((Tier2.gProt[j].Iovcharge.isEnable & 0x80) == 0)
				{
					DcdcProtectSet(DEBUG_CH, PROTECT_LIMIT_CURT, Tier2.gProt[j].Iovcharge.value);        //��������ֵ
					CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
					osDelay(100);	
				}					
				
				//DCDC���ѹ��������
				DcdcCtrlParaSet(DEBUG_CH, CTRL_RATE_VOL, Tier2.workstepAnalyze[j].StepRunVol);
				CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
				osDelay(100);
				DcdcCtrlParaSet(DEBUG_CH, CTRL_RATE_CURT, Tier2.workstepAnalyze[j].StepRunCur);
				CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
				osDelay(100);
				
				
				//��ʼ����
				DcdcRunStaSet(DEBUG_CH, RUN_STATUS);
				CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
				osDelay(100);	
				
				dcdc_on_off_state = 1;
			}				
			
        }
        break;

        case SCMD_WORKSTEP_TYPE_CV:             // ��ѹ���
        {
			time_over_stop[j] = 0;
			
//			BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_CCTOCV:         // ����ת��ѹ���
        {
			time_over_stop[j] = 0;
			
//			BatteryFormattingProcess(j, BUCK_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCC:            // �����ŵ�
        {
			time_over_stop[j] = 0;
			
//			BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCV:            // ��ѹ�ŵ�
        {
			time_over_stop[j] = 0;
			
//			BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
        }
        break;

        case SCMD_WORKSTEP_TYPE_DCCTODCV:       // ����ת��ѹ�ŵ�
        {
			time_over_stop[j] = 0;
			
//			BatteryFormattingProcess(j, BOOST_MODE, V_SENSE, Tier2.workstepAnalyze[j].StepRunVol, Tier2.workstepAnalyze[j].StepRunCur);
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
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
int16_t Workstep_Reload_Func(uint8_t chx)
{
    uint8_t j = chx;
	
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
		switch(Tier2.workstep[j].RunSeg[i].type)
		{
			case SCMD_WORKSTEP_PARAM_VOLTAGE:		// �������е�ѹ
			{
				Tier2.workstepAnalyze[j].StepRunVol = Tier2.workstep[j].RunSeg[i].data;
			}
			break;
			
			case SCMD_WORKSTEP_PARAM_CURRENT:		// �������е���
			{
				Tier2.workstepAnalyze[j].StepRunCur = Tier2.workstep[j].RunSeg[i].data;
			}
			break;
			
			default :								// Ԥ��
			{
				/* do nothing */
			}
			break;;
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
**  �� �� �� : Workstep_Switch_Func()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
int16_t Workstep_Switch_Func(uint8_t chx)
{
    uint8_t j = chx;
	if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
	{
		if(Workstep_Reload_Func(j) == -1)  //����������û�����ݣ�ֱ�ӽ���ȴ�״̬
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
		else  //��������
		{
			if(time_over_stop[j] != 1)		//����ʱ���ֹ��֡
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
**  �� �� �� : Workstep_Analysis_Func()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
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
                case WORKSTEP_RUNSTA_SET_STOP:          // ֹͣ
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

                case WORKSTEP_RUNSTA_SET_PAUSE:         // ��ͣ
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
                        Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_CONTACT;		// �Ӵ�����
									
						/*��ʼ��ѹ���޼��*/
						if((Tier2.gProt[j].VinitThrH.isEnable & 0x80) == 0)
						{
							if(Tier2.record[j].Vbat > Tier2.gProt[j].VinitThrH.value)
							{
//								GP_Alarm_Flag[j] |= GlobalProtect_MV;
							}
						}

						/*��ʼ��ѹ���޼��*/
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

						/* �Ӵ����ʹ�� */
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
//							//DCDCģʽ����
//							DcdcRunModeSet(DEBUG_CH, CTRL_RUN_CC_MODE);
//							CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
//							osDelay(100);	
//							
//							//DCDC���Ƶ�ѹ��������
//							DcdcProtectSet(DEBUG_CH, PROTECT_LIMIT_VOL, Tier2.workstepAnalyze[j].StepProVlimitH);     //��ѹ���ޱ���ֵ
//							CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
//							osDelay(100);
//							DcdcProtectSet(DEBUG_CH, PROTECT_LIMIT_CURT, Tier2.workstepAnalyze[j].StepProIov);        //��������ֵ
//							CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
//							osDelay(100);					
//							
//							//DCDC���ѹ��������
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
	workstep_flag_RunstaSet ^= workstep_flag_RunstaClr;
}

/**********************************************************************************************************
**  �� �� �� : Workstep_Abort_Func()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
#ifndef SW_UNIT
void Workstep_Abort_Func(void)		// ͳһ��ֹ�������ڽ�ֹ������
{
	uint8_t abort_type_tmp = 0;		// 0-�޽�ֹ��1-ʱ���ֹ��2-��ѹ��ֹ��3-������ֹ��4-������ֹ
	uint8_t abort_info_tmp = 0;		// 0-���쳣��1-ʱ���ֹ�쳣��2-��ѹ��ֹ�쳣��3-������ֹ�쳣��4-������ֹ��ֹ
	
    for(int j = 0; j < CHANNEL_CFG; j++)
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
//						time_over_stop[j] = 1;
//						
//                      Workstep_Switch_Func(j);

						abort_type_tmp = 1;
						abort_info_tmp = 0;
                    }
                }
                break;

                case SCMD_WORKSTEP_TYPE_CC:             /* ������� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
                    {
						if(time_over_stop[j] == 2)  //����ʱ���ֹ��֡
						{
							break;
						}
						
						time_over_stop[j] = 1;
						
                        if(workstep_flag_AbortPrio[j] & (1 << (SCMD_WORKSTEP_PARAM_TIME - 1)))  //�жϵ�ǰ��ֹ�������ȼ��Ƿ�Ϊ���
                        {
//							SET_0(workstep_flag_ErrWord[j], 6);
//							
//							Workstep_Switch_Func(j);

							abort_type_tmp = 1;
							abort_info_tmp = 0;
                        }
                        else  //��ǰ��ֹ�������ȼ���������ȼ�
                        {
//							SET_1(workstep_flag_ErrWord[j], 6);
//							
//							Protect_Action_Func(j, PROTECT_WORKSTEP, WORKSTEP_RUNSTA_REF_STOP);
							
							abort_type_tmp = 1;
							abort_info_tmp = 1;
                        }
											
                    }
					else if((abt_vol > 0) && (run_vol >= abt_vol))	/* ���е�ѹ > ��ֹ��ѹ */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_CV:             /* ��ѹ��� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_CCTOCV:			/* ��ѹ������� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_DCC:            /* �����ŵ� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_vol > 0) && (run_vol <= abt_vol))	/* ���е�ѹ < ��ֹ��ѹ */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_DCV:            /* ��ѹ�ŵ� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_DCCTODCV:		/* ��ѹ�����ŵ� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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
				
				if(run_time >= abt_time)	/* ����ʱ�� > ��ֹʱ�� */
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
void Workstep_Abort_Func(void)		// ������ֹ���������л�������
{
    for(int j = 0; j < CHANNEL_CFG; j++)
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
                    }
                }
                break;

                case SCMD_WORKSTEP_TYPE_CC:             /* ������� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_vol > 0) && (run_vol >= abt_vol))	/* ���е�ѹ > ��ֹ��ѹ */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_CV:             /* ��ѹ��� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_CCTOCV:			/* ��ѹ������� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_DCC:            /* �����ŵ� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_vol > 0) && (run_vol <= abt_vol))	/* ���е�ѹ < ��ֹ��ѹ */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_DCV:            /* ��ѹ�ŵ� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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

                case SCMD_WORKSTEP_TYPE_DCCTODCV:		/* ��ѹ�����ŵ� */
                {
                    if((abt_time > 0) && (run_time >= abt_time))	/* ����ʱ�� > ��ֹʱ�� */
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
					else if((abt_cur > 0) && (run_cur <= abt_cur) && (ProtectMaskFlag[j] == 0))	/* ���е��� < ��ֹ���� */
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
					else if((abt_cap > 0) && (run_cap >= abt_cap))	/* �������� > ��ֹ���� */
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
				
				if(run_time >= abt_time)	/* ����ʱ�� > ��ֹʱ�� */
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
**  �� �� �� : AppTaskWorkstep()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
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
