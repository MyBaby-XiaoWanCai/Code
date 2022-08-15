/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :JN95
** --------------------------------------------------------------------------------------------------------
** File name   : AppTaskLed.c
** Author      : Robert.Li
** Date        : 2021-06-30
** Description : Led application task file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
#ifdef YCJ
uint8_t g_ucLedChSta[9] = {1, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t g_ucLedChR[9] = {RUN_LED_R_NO, CH1_LED_R_NO, CH2_LED_R_NO, CH3_LED_R_NO, CH4_LED_R_NO, CH5_LED_R_NO, CH6_LED_R_NO, CH7_LED_R_NO, CH8_LED_R_NO};
uint8_t g_ucLedChG[9] = {RUN_LED_G_NO, CH1_LED_G_NO, CH2_LED_G_NO, CH3_LED_G_NO, CH4_LED_G_NO, CH5_LED_G_NO, CH6_LED_G_NO, CH7_LED_G_NO, CH8_LED_G_NO};
#else
uint8_t g_ucLedChSta[5] = {1, 0, 0, 0, 0};
uint8_t g_ucLedChR[5] = {RUN_LED_R_NO, CH1_LED_R_NO, CH2_LED_R_NO, CH3_LED_R_NO, CH4_LED_R_NO};
uint8_t g_ucLedChG[5] = {RUN_LED_G_NO, CH1_LED_G_NO, CH2_LED_G_NO, CH3_LED_G_NO, CH4_LED_G_NO};
#endif

extern uint32_t IP_Alarm_Flag[8];
extern uint8_t contact_test_once_flag[8];
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void AppTaskLed(void *argument);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

void LedStaInit(uint8_t sta)
{
	switch(sta)
	{
		case 0:		// 0. ȫ��
		{
			g_ucLedChSta[0] = RUN_R_ON;
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				g_ucLedChSta[j + 1] = CH_ERR_HARDWARE;
			}
		}
		break;
		
		case 1:		// 1. ȫ��
		{
			g_ucLedChSta[0] = RUN_OFF;
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				g_ucLedChSta[j + 1] = CH_STANDBY;
			}		
		}
		break;
		
		case 2:		// 2. ȫ��
		{
			g_ucLedChSta[0] = RUN_G_ON;
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				g_ucLedChSta[j + 1] = CH_COMPLETE;
			}
		}
		break;
		
		case 3:		// 3. ȫ��
		{
			g_ucLedChSta[0] = RUN_OFF;
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				g_ucLedChSta[j + 1] = CH_STANDBY;
			}		
		}
		break;
		
		default:	// RUN����
		{
			g_ucLedChSta[0] = RUN_G_SHINE;		
		}
		break;
	}
		
	osDelay(5000);
}

void LedStaSwFunc(void)
{
	if((g_ucSysMode == SYS_NORMAL_MODE) && (SysModeProtMaskFlag == 0))
	{
		for(int j = 0; j < CHANNEL_CFG; j++)
		{
			if(Protect_Flag_Global[j] != 0)						// ȫ�ֱ���
			{
				SET_1(Tier2.record[j].ErrType, 0);
			}
			else
			{
				SET_0(Tier2.record[j].ErrType, 0);
			}
			if(Protect_Flag_Workstep[j] != 0)					// ����������
			{
				SET_1(Tier2.record[j].ErrType, 1);
			}
			else
			{
				SET_0(Tier2.record[j].ErrType, 1);
			}
			if(Protect_Flag_Internal[j] != 0)					// �ڲ�����
			{
				SET_1(Tier2.record[j].ErrType, 2);
			}
			else
			{
				SET_0(Tier2.record[j].ErrType, 2);
			}
			if((ACDC.Type==1)||(ACDC.Type==11))   				//����
			{
				if((ACDC.State[0] & 0x0FF8) || (ACDC.State[1] & 0x0FF8))	// ACDC����
				{
					SET_1(Tier2.record[j].ErrType, 3);
				}
				else
				{
					SET_0(Tier2.record[j].ErrType, 3);
				}
			}
			else if((ACDC.Type==2)||(ACDC.Type==22))			//�յ�
			{
				if((ACDC.State[0] & 0xBFF0) || (ACDC.State[1] & 0xBFF0))	// ACDC����
				{
					SET_1(Tier2.record[j].ErrType, 3);
				}
				else
				{
					SET_0(Tier2.record[j].ErrType, 3);
				}	
			}
			if(Tier2.record[j].ErrType != 0)		// �й���
			{
				if(ContactCheckFlag[j] == 0)		// �޽Ӵ�����
				{
					contact_test_once_flag[j] = 0;	// ����Ҫǿ�н���Ӵ����
				}
			}
			if(Tier2.record[j].ErrType != 0)		// �й���
			{
				if(Tier2.record[j].ErrType & 0x01)			// ȫ�ֹ���
				{
					g_ucLedChSta[j + 1] = CH_ERR_GLOBAL;
				}
				else if(Tier2.record[j].ErrType & 0x02)		// ����������
				{
					g_ucLedChSta[j + 1] = CH_ERR_WORKSTEP;
				}
				else if(Tier2.record[j].ErrType & 0x04)		// �ڲ�����
				{
					if((IP_Alarm_Flag[j] & IntelProtect_HPA) != 0)	// �ڲ�Ӳ������
					{
						g_ucLedChSta[j + 1] = CH_ERR_HARDWARE;
					}
					else											// �ڲ���������
					{
						g_ucLedChSta[j + 1] = CH_ERR_OTHER;
					}
				}
				else if(Tier2.record[j].ErrType & 0x08)		// ACDC����
				{
					g_ucLedChSta[j + 1] = CH_ERR_ACDC;
				}
			}
			else if((Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT)   || \
					((Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)  && \
					 ((Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CC)     || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CV) || \
					  (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CCTOCV) || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_CP))))		// ���
			{
				g_ucLedChSta[j + 1] = CH_CHARGE;
			}
			else if((Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
					((Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCC)      || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCV) || \
					 (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV) || (Tier2.record[j].StepType == SCMD_WORKSTEP_TYPE_DCP)))	// �ŵ�
			{
				g_ucLedChSta[j + 1] = CH_DISCHARGE;
			}
			else if(Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)		// ���
			{
				g_ucLedChSta[j + 1] = CH_COMPLETE;
			}
			else if(Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_PAUSE)		// ��ͣ
			{
				g_ucLedChSta[j + 1] = CH_PAUSE;
			}
			else																// ����
			{
				g_ucLedChSta[j + 1] = CH_STANDBY;
			}
		}
	}
	else
	{
		g_ucLedChSta[0] = RUN_G_SHINE;
	}
}

void LedStaExecute(void)
{
	//RUN��
	switch(g_ucLedChSta[0])
	{			
		case RUN_OFF:		/* �ر�ָʾ�� */
		{
			Led_Shine_Ctrl(RUN_LED_R_NO, 0, 1000, LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G_NO, 0, 1000, LED_TASK_PERIOD);			
		}
		break;
		
		case RUN_G_ON:		/* �̵� */
		{
			Led_Shine_Ctrl(g_ucLedChR[0], 0   , 1000, LED_TASK_PERIOD);
			Led_Shine_Ctrl(g_ucLedChG[0], 1000, 0   , LED_TASK_PERIOD);			
		}
		break;
		
		case RUN_R_ON:		/* ��� */
		{
			Led_Shine_Ctrl(g_ucLedChR[0], 1000, 0   , LED_TASK_PERIOD);
			Led_Shine_Ctrl(g_ucLedChG[0], 0   , 1000, LED_TASK_PERIOD);			
		}
		break;

		case RUN_G_SHINE:	/* �̵���˸ */
		{
			Led_Shine_Ctrl(RUN_LED_R_NO, 0  , 1000, LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G_NO, 500, 500 , LED_TASK_PERIOD);
		}
		break;		
		
		case RUN_R_SHINE:	/* �����˸ */
		{
			Led_Shine_Ctrl(RUN_LED_R_NO, 500, 500 , LED_TASK_PERIOD);
			Led_Shine_Ctrl(RUN_LED_G_NO, 0  , 1000, LED_TASK_PERIOD);
		}
		break;
		
		default:
		{
		
		}
		break;			
	}

	//CH��
	for(int i = 1; i < CHANNEL_CFG+1; i++)
	{
		switch(g_ucLedChSta[i])
		{
			case CH_STANDBY:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0, 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0, 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_CHARGE:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0  , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 500, 500 , LED_TASK_PERIOD);
			}
			break;

			case CH_DISCHARGE:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0  , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 200, 200 , LED_TASK_PERIOD);
			}
			break;;

			case CH_COMPLETE:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0   , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 1000, 0   , LED_TASK_PERIOD);
			}
			break;;

			case CH_PAUSE:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 0  , 1000, LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 800, 200 , LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_GLOBAL:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 200, 200 , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0  , 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_WORKSTEP:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 500, 500 , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0  , 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_HARDWARE:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 1000, 0   , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0   , 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_OTHER:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 800, 200 , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0  , 1000, LED_TASK_PERIOD);
			}
			break;

			case CH_ERR_ACDC:
			{
				Led_Shine_Ctrl(g_ucLedChR[i], 200, 800 , LED_TASK_PERIOD);
				Led_Shine_Ctrl(g_ucLedChG[i], 0  , 1000, LED_TASK_PERIOD);
			}
			break;

			default:
			{

			}
			break;
		}
	}
}


/**********************************************************************************************************
**	�� �� �� : AppTaskLed()
**	����˵�� : LED����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void AppTaskLed(void *argument)
{
	uint8_t LedTaskCnt = 0;
	uint8_t AuxRecordCnt = 0;
	
    while(1)
    {
		if(err_send_flag == 1)
		{
			err_send_flag = 0;
			
			LedTaskCnt = 0;
		}		
		
		if((LedTaskCnt < 5) && (SysModeProtMaskFlag == 0))
		{
			LedStaInit(LedTaskCnt);
			
			LedTaskCnt++;
		}
		else
		{
			LedTaskCnt = 5;
			
			LedStaSwFunc();
		}
		
		LedStaExecute();

		Buffer_Data_To_Record();
		
		if(AuxRecordCnt < 50)
		{
			AuxRecordCnt++;
		}
		else
		{
			AuxRecordDatUpcast();
			AuxRecordCnt = 0;
		}
		
        osDelay(LED_TASK_PERIOD);
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
