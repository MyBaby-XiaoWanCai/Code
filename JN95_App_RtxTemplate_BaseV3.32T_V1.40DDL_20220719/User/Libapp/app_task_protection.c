/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     : JN95
** --------------------------------------------------------------------------------------------------------
** File name   : AppTaskprotection.c
** Author      : wl
** Date        : 2022-05��12
** Description : Protection application task file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "app_task_protection.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t  First_Alarm = 0;					// ��һ�α�����־(�����һ���ر�) 0-��һ�α��� 1-���ǵ�һ�α���
uint8_t  SysModeProtMaskFlag = 0;			// �������α�ʶ(0-�����ι��� 1-���ι���)
uint8_t  ucIProtCntPre[12][8] = {0};		// �ڲ����ϼ���ʱ(ǰ��)
uint8_t  ucIProtCntCur[12][8] = {0};		// �ڲ����ϼ���ʱ(��ǰ)
uint8_t  ucGProtCntPre[4][8] = {0};			// ȫ�ֹ��ϼ���ʱ(ǰ��)
uint8_t  ucGProtCntCur[4][8] = {0};			// ȫ�ֹ��ϼ���ʱ(��ǰ)
uint32_t IP_Alarm_Flag[8] = {0};			// �ڲ�������־λ
uint32_t GP_Alarm_Flag[8] = {0};			// ȫ�ֱ�����־λ
uint32_t IP_Alarm_Flag_History[8] = {0};	// �ڲ�������ʷ��־λ
uint32_t GP_Alarm_Flag_History[8] = {0};	// ȫ�ֱ�����ʷ��־λ
protect_alarmStruct Protect_Alarm;			// �ڲ�����֡


/*�ڲ�����������������ұ�*/
CMDProject IP_CheckList[] =
{
    {0x01, IntelProtect_OLA, 0, 0},			// �����������
    {0x02, IntelProtect_BOV, 0, 0},			// BUS��ѹ
    {0x03, IntelProtect_BUV, 0, 0},			// BUSǷѹ
    {0x04, IntelProtect_APA, 0, 0},			// �������ȸ澯
    {0x05, IntelProtect_UPA, 0, 0},			// ��ѹ���ȸ澯
    {0x06, IntelProtect_VDQ, 0, 0},			// ��·�����澯
    {0x07, IntelProtect_MOT, 0, 0},			// ģ�����
    {0x08, IntelProtect_EOT, 0, 0},			// ��������
    {0x09, IntelProtect_HPA, 0, 0},			// Ӳ������
    {0x0a, IntelProtect_FHE, ALLCH, ALLCH},	// ���ȹ���,û��ʹ���������Ĭ�Ͽ���
    {0x0b, IntelProtect_UHE, ALLCH, ALLCH},	// CPU�쳣,û��ʹ���������Ĭ�Ͽ���
    {0x0c, IntelProtect_C1E, ALLCH, ALLCH},	// can1�쳣,û��ʹ���������Ĭ�Ͽ���
    {0x0d, IntelProtect_C2E, ALLCH, ALLCH},	// can2�쳣,û��ʹ���������Ĭ�Ͽ���
    {0x0e, IntelProtect_DAE, ALLCH, ALLCH},	// DA�쳣,û��ʹ���������Ĭ�Ͽ���
    {0x0f, IntelProtect_ADE, ALLCH, ALLCH},	// AD�쳣,û��ʹ���������Ĭ�Ͽ���
    {0x10, IntelProtect_FLE, ALLCH, ALLCH},	// FLASH�쳣,û��ʹ���������Ĭ�Ͽ���
    {0x11, IntelProtect_EEE, ALLCH, ALLCH},	// Eeprom�쳣,û��ʹ���������Ĭ�Ͽ���
    {0x12, IntelProtect_FRE, ALLCH, ALLCH},	// FRAM�쳣,û��ʹ���������Ĭ�Ͽ���
    {0x13, IntelProtect_PME, ALLCH, ALLCH},	// ��λ���쳣,û��ʹ���������Ĭ�Ͽ���
    {0x14, IntelProtect_VNN, 0, 0},			// ��ѹ�쳣��Э������
    {0x15, IntelProtect_CVP, 0, 0},			// ��·ѹ�����Э������
    {0x16, IntelProtect_VUQ, 0, 0},			// ��ѹ����������Э������
    {0x17, IntelProtect_CNN, 0, 0},			// �����쳣��Э������
    {0x18, IntelProtect_CUQ, 0, 0},			// ��������������Э������
    {0x19, IntelProtect_LOV, 0, 0},			// ������ѹ�쳣��̽���ѹ����5.1V����Э������
    {0x1A, IntelProtect_MAC, 0, 0},			// MAC�쳣
	{0x1B, IntelProtect_CHE, 0, 0},			// ͨ��ģ���쳣
};

/*ȫ�ֱ���������������ұ�*/
CMDProject GP_CheckList[] =
{
    {0x01, GlobalProtect_OV  , 0, 0},		// ��ѹ����
    {0x02, GlobalProtect_UV  , 0, 0},		// Ƿѹ����
    {0x03, GlobalProtect_COA , 0, 0},		// ����������
    {0x04, GlobalProtect_DOA , 0, 0},		// �ŵ��������
    {0x05, GlobalProtect_OC  , 0, 0},		// ���ݱ���
    {0x06, GlobalProtect_MV  , 0, 0},		// ��ʼ��ѹ���ޱ���
    {0x07, GlobalProtect_LV  , 0, 0},		// ��ʼ��ѹ���ޱ���
    {0x08, IntelProtect_NONE1, 0, 0},		// δ����
    {0x09, IntelProtect_NONE2, 0, 0},		// δ����
    {0x0a, IntelProtect_VCV  , 0, 0},		// �Ӵ�����ѹ�仯��
    {0xfe, IntelProtect_OTE  , 0, 0},		// ��������
    {0xff, IntelProtect_INV  , 0, 0},		// ��Ч
};

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
uint32_t IntelProtectDeviceCheck(void);		//�ڲ�����Ӳ�����
uint32_t IntelProtectRunParmCheck(void);	//�ڲ��������в������
uint32_t GlobleProtectRunParmCheck(void);	//ȫ�ֱ������в������
void AppTaskProtection(void *argument);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : IntelProtectDeviceCheck()
**  ����˵�� : �ڲ������������,����cpu��can1��eeprom��fram,����ִ��һ��
**  ��    �� : ��
**  �� �� ֵ : �ڲ���������״̬
**********************************************************************************************************/
uint32_t IntelProtectDeviceCheck()
{
	if(SysModeProtMaskFlag == 0)	//�������α�ʶ��Ч
	{	
//		//CPU�쳣������Ƿ���REST��ƽ����ĸ�λ���Ҳ��ǵ������ϵ�����λ
//		if((bM4_RMU->RSTF0_b.PINRF == 1) && (bM4_RMU->RSTF0_b.PORF != 1))
////		if( (RMU_GetStatus(RMU_RST_POWER_ON) == Reset) && \
////			((RMU_GetStatus(RMU_RST_RESET_PIN) == Set) || (RMU_GetStatus(RMU_RST_WDT) == Set)) )
//		{
//			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
//			{ 
//				IP_Alarm_Flag[j] |= IntelProtect_UHE; 
//			}
//		}
//		PWC_Unlock(PWC_UNLOCK_CODE_1);                        
//		RMU_ClrStatus();	//���CLRF��λ��־
//		PWC_Lock(PWC_UNLOCK_CODE_1);   
		
		/* ���ACDC(can1)ģ��,���÷���ת�� */
		ACDC.Type = ACDC_Fan_Speed51_ModeTest();		//���ACDC���ͣ������÷��ȵ�51%
#ifdef YCJ		
        if(ACDC.Type == 0)												//8ͨ�����ͣ�ֻ��һ��ACDC�����ϱ�0
#else
        if((ACDC.Type == 0) || (ACDC.Type == 11) || (ACDC.Type == 22))	//4ͨ�����ͣ���2��ACDC����ACDC����=11��22��˫ACDC����=00
#endif  
		{
			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{
				IP_Alarm_Flag[j] |= IntelProtect_C1E;	//can1����
			}
		}

		/* ���DA,��ID,��AD7124_CfgADC(),AD7175_CfgADC()�ڲ��Ѽ�� */


		/* ���AD,��ID,��dacx0508_cfg()�ڲ��Ѽ�� */


		/* ���flash,��ID */

		
		/* ���fram,��ID */
		uint32_t FramID = 0;
		
		MB85Ctrl_ReadID(&FramID);	//��ȡFRAM��ID
		
		if((FramID & 0xff000000) != 0x04000000)		//ID����ֽ�0x04����������Ϊ��ʿ
		{
			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{
				IP_Alarm_Flag[j] |= IntelProtect_FRE;	//fram����
			}
		}

		/* ���EE,��д���������ֽ� */
		uint8_t TestDate = features;
		
		if(ee_WriteBytesWithCheck(&TestDate, Last_addr - 2, 1) == 0)
		{
			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{
				IP_Alarm_Flag[j] |= IntelProtect_EEE;	//ee����
			}
		}

		/* ����λ��,���üĴ�����������ֵ */
	}
	else							//�������α�ʶ��Ч
	{
		ACDC.Type = ACDC_Fan_Speed51_ModeTest();		//���ACDC���ͣ������÷��ȵ�51%	
	}

    return 1;
}

//extern uint16_t bus_under_vol_flg;

/**********************************************************************************************************
**  �� �� �� : IntelProtectRunParmCheck()
**  ����˵�� : �ڲ��������в������,�����������������bus��ѹ��busǷѹ�����в������,ִ������1ms
**  ��    �� : ��
**  �� �� ֵ : �ڲ���������״̬
**********************************************************************************************************/
uint32_t IntelProtectRunParmCheck(void)
{	
    if(SysModeProtMaskFlag == 0)	//�������α�ʶ��Ч
    {
		for(uint8_t i = 0; i < CHANNEL_CFG; i++)
		{
			//-------------------------------1.����������-------------------------------//
			/* 1.1 ����ǰ�ι��ϼ���ʱ */
			ucIProtCntPre[0][i] = ucIProtCntCur[0][i];	
			/* 1.2 ���ϼ���߼� */
			if((Tier2.iProt[i].IoutThr.isEnable & 0x80) == 0)
			{
				if( (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_STOP    ) && \
					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_COMPLETE) && \
					(ProtectMaskFlag[i] != 1) )
				{
					if(Tier2.chRecord[i].Iout > Tier2.iProt[i].IoutThr.value)
					{
						ucIProtCntCur[0][i]++;
						if(ucIProtCntCur[0][i] >= PROT_TRIG_MS(400, 400))
						{
							IP_Alarm_Flag[i] |= IntelProtect_OLA;
							ucIProtCntCur[0][i] = 0;
						}
					}
				}
			}
			/* 1.3 �жϹ����Ƿ�������� */
			if(ucIProtCntPre[0][i] == ucIProtCntCur[0][i])
			{
				ucIProtCntPre[0][i] = 0;
				ucIProtCntCur[0][i] = 0;
			}			

			//-------------------------------2.BUS��ѹ���-------------------------------//
			/* 2.1 ����ǰ�ι��ϼ���ʱ */
			ucIProtCntPre[1][i] = ucIProtCntCur[1][i];
			ucIProtCntPre[10][i] = ucIProtCntCur[10][i];
			/* 2.2 ���ϼ���߼� */			
			if((Tier2.iProt[i].VbusThrH.isEnable& 0x80) == 0)
			{
//				if(ProtectMaskFlag[i] != 1)
				{
					if(Tier2.sampleCal.Vbus1 > Tier2.iProt[i].VbusThrH.value)		//bus1��Ӧǰһ��ͨ��
					{
						ucIProtCntCur[1][i]++;
						if(ucIProtCntCur[1][i] >= PROT_TRIG_MS(2000, 400))
						{						
							for(uint8_t j = 0; j < (CHANNEL_CFG / 2); j++)
							{
								IP_Alarm_Flag[j] |= IntelProtect_BOV;
							}
							ucIProtCntCur[1][i] = 0;
						}
					}
					if(Tier2.sampleCal.Vbus2 > Tier2.iProt[i].VbusThrH.value)		//bus2��Ӧ��һ��ͨ��
					{
						ucIProtCntCur[10][i]++;
						if(ucIProtCntCur[10][i] >= PROT_TRIG_MS(2000, 400))
						{						
							for(uint8_t j = (CHANNEL_CFG / 2); j < CHANNEL_CFG; j++)
							{
								IP_Alarm_Flag[j] |= IntelProtect_BOV;
							}
							ucIProtCntCur[10][i] = 0;
						}
					}
				}
			}
			/* 2.3 �жϹ����Ƿ�������� */
			if(ucIProtCntPre[1][i] == ucIProtCntCur[1][i])
			{
				ucIProtCntPre[1][i] = 0;
				ucIProtCntCur[1][i] = 0;
			}
			if(ucIProtCntPre[10][i] == ucIProtCntCur[10][i])
			{
				ucIProtCntPre[10][i] = 0;
				ucIProtCntCur[10][i] = 0;
			}			
			
//			//-------------------------------3.BUSǷѹ���-------------------------------//
//			/* 3.1 ����ǰ�ι��ϼ���ʱ */
//			ucIProtCntPre[2][i] = ucIProtCntCur[2][i];	
//			ucIProtCntPre[11][i] = ucIProtCntCur[11][i];
//			/* 3.2 ���ϼ���߼� */
//			if((Tier2.iProt[i].VbusThrL.isEnable & 0x80) == 0)
//			{
////				if(ProtectMaskFlag[i] != 1)
//				if(bus_under_vol_flg == 1)
//				{				
//					if(Tier2.sampleCal.Vbus1 < Tier2.iProt[i].VbusThrL.value)		//bus1��Ӧǰһ��ͨ��
//					{
//						ucIProtCntCur[2][i]++;
//						if(ucIProtCntCur[2][i] >= PROT_TRIG_MS(400, 400))
//						{
//							for(uint8_t j = 0; j < (CHANNEL_CFG / 2); j++)
//							{
//								IP_Alarm_Flag[j] |= IntelProtect_BUV;
//							}
//							ucIProtCntCur[2][i] = 0;
//						}
//					}
//					if(Tier2.sampleCal.Vbus2 < Tier2.iProt[i].VbusThrL.value)		//bus2��Ӧ��һ��ͨ��
//					{
//						ucIProtCntCur[11][i]++;
//						if(ucIProtCntCur[11][i] >= PROT_TRIG_MS(400, 400))
//						{						
//							for(uint8_t j = (CHANNEL_CFG / 2); j < CHANNEL_CFG; j++)
//							{
//								IP_Alarm_Flag[j] |= IntelProtect_BUV;
//							}
//							ucIProtCntCur[11][i] = 0;
//						}
//					}
//				}
//			}
//			/* 3.3 �жϹ����Ƿ�������� */
//			if(ucIProtCntPre[2][i] == ucIProtCntCur[2][i])
//			{
//				ucIProtCntPre[2][i] = 0;
//				ucIProtCntCur[2][i] = 0;
//			}			
//			if(ucIProtCntPre[11][i] == ucIProtCntCur[11][i])
//			{
//				ucIProtCntPre[11][i] = 0;
//				ucIProtCntCur[11][i] = 0;
//			}	
			
			//-------------------------------4.��ѹ���ȼ��-------------------------------//
			/* 4.1 ����ǰ�ι��ϼ���ʱ */
			ucIProtCntPre[3][i] = ucIProtCntCur[3][i];	
			/* 4.2 ���ϼ���߼� */			
			if((Tier2.iProt[i].VcvDiff.isEnable & 0x80) == 0)
			{	
				if( (workstep_flag_CriticalZone[i] == 3) && \
					(ProtectMaskFlag[i] != 1) )
				{
					if(fabs(Tier2.chRecord[i].Vbat - Tier2.workstepAnalyze[i].StepRunVol) > Tier2.iProt[i].VcvDiff.value)
					{
						ucIProtCntCur[3][i]++;
						if(ucIProtCntCur[3][i] >= PROT_TRIG_MS(2000, 400))
						{
							IP_Alarm_Flag[i] |= IntelProtect_UPA;
							ucIProtCntCur[3][i] = 0;
						}
					}
				}
			}
			/* 4.3 �жϹ����Ƿ�������� */
			if(ucIProtCntPre[3][i] == ucIProtCntCur[3][i])
			{
				ucIProtCntPre[3][i] = 0;
				ucIProtCntCur[3][i] = 0;
			}
			
			//-------------------------------5.ģ����¼��-------------------------------//
			/* 5.1 ����ǰ�ι��ϼ���ʱ */
			ucIProtCntPre[4][i] = ucIProtCntCur[4][i];	
			/* 5.2 ���ϼ���߼� */	
			if((Tier2.iProt[i].Totmod.isEnable & 0x80) == 0)
			{
//				if(ProtectMaskFlag[i] != 1)
				{
					if(Tier2.chRecord[i].Tmod > Tier2.iProt[i].Totmod.value)
					{
						ucIProtCntCur[4][i]++;
						if(ucIProtCntCur[4][i] >= PROT_TRIG_MS(2000, 400))
						{
							IP_Alarm_Flag[i] |= IntelProtect_MOT;
							ucIProtCntCur[4][i] = 0;
						}
					}
				}
			}
			/* 5.3 �жϹ����Ƿ�������� */
			if(ucIProtCntPre[4][i] == ucIProtCntCur[4][i])
			{
				ucIProtCntPre[4][i] = 0;
				ucIProtCntCur[4][i] = 0;
			}			

			//-------------------------------6.�������¼��-------------------------------//
			/* 6.1 ����ǰ�ι��ϼ���ʱ */
			ucIProtCntPre[5][i] = ucIProtCntCur[5][i];	
			/* 6.2 ���ϼ���߼� */				
			if((Tier2.iProt[i].Totenv.isEnable & 0x80) == 0)
			{
//				if(ProtectMaskFlag[i] != 1)
				{
					if(Tier2.sampleCal.Tamb > Tier2.iProt[i].Totenv.value)
					{
						ucIProtCntCur[5][i]++;
						if(ucIProtCntCur[5][i] >= PROT_TRIG_MS(2000, 400))
						{						
							IP_Alarm_Flag[i] |= IntelProtect_EOT;
							ucIProtCntCur[5][i] = 0;
						}
					}
				}
			}
			/* 6.3 �жϹ����Ƿ�������� */
			if(ucIProtCntPre[5][i] == ucIProtCntCur[5][i])
			{
				ucIProtCntPre[5][i] = 0;
				ucIProtCntCur[5][i] = 0;
			}

			//-------------------------------7.��ѹ�쳣���-------------------------------//
			/* 7.1 ����ǰ�ι��ϼ���ʱ */
			ucIProtCntPre[6][i] = ucIProtCntCur[6][i];
			/* 7.2 ���ϼ���߼� */	
			if((Tier2.iProt[i].SNERR.isEnable & 0x80) == 0)
			{
				if( (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_START  ) && \
					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_CONTACT) && \
				   ((Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_RUNNING) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_STANDBY)) && \
					(ProtectMaskFlag[i] != 1) )
				{
					if( (fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vtz ) > (3 * Tier2.iProt[i].SNERR.value)) || \
						(fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vout) > (3 * Tier2.iProt[i].SNERR.value)) || \
						(fabs(Tier2.chRecord[i].Vtz  - Tier2.chRecord[i].Vout) > (3 * Tier2.iProt[i].SNERR.value)) )
					{
						ucIProtCntCur[6][i]++;
						if(ucIProtCntCur[6][i] >= PROT_TRIG_MS(4000, 400))
						{
							IP_Alarm_Flag[i] |= IntelProtect_VNN;
							ucIProtCntCur[6][i] = 0;
						}
					}
				}
			}
			/* 7.3 �жϹ����Ƿ�������� */
			if(ucIProtCntPre[6][i] == ucIProtCntCur[6][i])
			{
				ucIProtCntPre[6][i] = 0;
				ucIProtCntCur[6][i] = 0;
			}			
			
			//-------------------------------8.��·ѹ����-------------------------------//
			/* 8.1 ����ǰ�ι��ϼ���ʱ */
			ucIProtCntPre[7][i] = ucIProtCntCur[7][i];
			/* 8.2 ���ϼ���߼� */	
			if((Tier2.iProt[i].VloopDiff.isEnable & 0x80) == 0)
			{
				if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
					(ProtectMaskFlag[i] != 1) )
				{
					if(fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vout) > Tier2.iProt[i].VloopDiff.value)
					{
						ucIProtCntCur[7][i]++;
						if(ucIProtCntCur[7][i] >= PROT_TRIG_MS(400, 400))
						{
							IP_Alarm_Flag[i] |= IntelProtect_CVP;
							ucIProtCntCur[7][i] = 0;
						}						
					}			
				}
			}
			/* 8.3 �жϹ����Ƿ�������� */
			if(ucIProtCntPre[7][i] == ucIProtCntCur[7][i])
			{
				ucIProtCntPre[7][i] = 0;
				ucIProtCntCur[7][i] = 0;
			}
			
			//-------------------------------9.�������ȼ��-------------------------------//
			/* 9.1 ����ǰ�ι��ϼ���ʱ */
			ucIProtCntPre[8][i] = ucIProtCntCur[8][i];
			/* 9.2 ���ϼ���߼� */	
			if((Tier2.iProt[i].IccDiff.isEnable & 0x80) == 0)
			{
				if( (Tier2.workstepRunning[i].StepType != SCMD_WORKSTEP_TYPE_STANDBY) && \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING ) && \
					(workstep_flag_CriticalZone[i] == 0) && (ProtectMaskFlag[i] != 1) )
				{
//					if(fabs(Tier2.chRecord[i].Iout - Tier2.workstepAnalyze[i].StepRunCur) > Tier2.iProt[i].IccDiff.value)
					if(fabs(Tier2.chRecord[i].Iout - Tier2.workstepAnalyze[i].StepRunCur) > 0.32f)
					{
						ucIProtCntCur[8][i]++;
						if(ucIProtCntCur[8][i] >= PROT_TRIG_MS(2000, 400))
						{
#ifdef YCJ
							if(Tier2.chRecord[i].Iout < 0.1f)																//ģ��ͨ���쳣
							{
								IP_Alarm_Flag[i] |= IntelProtect_CHE;
							}
							else if(fabs(Tier2.chRecord[i].Iout - Tier2.workstepAnalyze[i].StepRunCur) > 2.0f)				//�����쳣
							{
								IP_Alarm_Flag[i] |= IntelProtect_CNN;
							}
							else																							//��������
							{
								IP_Alarm_Flag[i] |= IntelProtect_APA;
							}							
#else
							if(active_run_channel_numb == 1)	//��ģ�鹤��
							{
								if(Tier2.sampleCal.Iout[2 * i + 0] < 0.1f)													//ģ��ͨ���쳣
								{
									IP_Alarm_Flag[i] |= IntelProtect_CHE;
								}
								else if(fabs(Tier2.sampleCal.Iout[2 * i + 0] - Tier2.workstepAnalyze[i].StepRunCur) > 2.0f)	//�����쳣
								{
									IP_Alarm_Flag[i] |= IntelProtect_CNN;
								}
								else																						//��������
								{
									IP_Alarm_Flag[i] |= IntelProtect_APA;
								}
							
							}
							else								//˫ģ�鹤��
							{
								if((Tier2.sampleCal.Iout[2 * i + 0] < 0.1f) || (Tier2.sampleCal.Iout[2 * i + 1] < 0.1f))	//ģ��ͨ���쳣
								{
									IP_Alarm_Flag[i] |= IntelProtect_CHE;
								}
								else if(fabs(Tier2.chRecord[i].Iout - Tier2.workstepAnalyze[i].StepRunCur) > 2.0f)			//�����쳣
								{
									IP_Alarm_Flag[i] |= IntelProtect_CNN;
								}
								else																						//��������
								{
									IP_Alarm_Flag[i] |= IntelProtect_APA;
								}
							}
#endif	
							ucIProtCntCur[8][i] = 0;
						}
					}
				}
			}
			/* 9.3 �жϹ����Ƿ�������� */
			if(ucIProtCntPre[8][i] == ucIProtCntCur[8][i])
			{
				ucIProtCntPre[8][i]= 0;
				ucIProtCntCur[8][i] = 0;
			}
			
			//-------------------------------10.�����쳣���-------------------------------//
			/* 10.1 ����ǰ�ι��ϼ���ʱ */
			ucIProtCntPre[9][i] = ucIProtCntCur[9][i];
			/* 10.2 ���ϼ���߼� */	
			if( (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_START  ) && \
				(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_CONTACT) && \
			   ((Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_RUNNING) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_STANDBY)) && \
				(ProtectMaskFlag[i] != 1) )
			{
				if(Tier2.chRecord[i].Iout > 0.1f)
				{
					ucIProtCntCur[9][i]++;
					if(ucIProtCntCur[9][i] >= PROT_TRIG_MS(2000, 400))
					{
						IP_Alarm_Flag[i] |= IntelProtect_CNN;
						ucIProtCntCur[9][i] = 0;
					}
				}			
			}
			/* 10.3 �жϹ����Ƿ�������� */
			if(ucIProtCntPre[9][i] == ucIProtCntCur[9][i])
			{
				ucIProtCntPre[9][i] = 0;
				ucIProtCntCur[9][i] = 0;
			}
			
			//-------------------------------11.������ѹ�쳣���-------------------------------//

		}	
    }

    return 1;
}

/**********************************************************************************************************
**  �� �� �� : GlobleProtectRunParmCheck()
**  ����˵�� : ȫ�����в�����⣬���ڵ�ص�ѹ�����ޣ���·ѹ���ŵ���������в�����⣬ִ������1ms
**  ��    �� : ��
**  �� �� ֵ : �ڲ���������״̬
**********************************************************************************************************/
uint32_t GlobleProtectRunParmCheck(void)
{
	if(SysModeProtMaskFlag == 0)	//�������α�ʶ��Ч
	{
		for(uint8_t i = 0; i < CHANNEL_CFG; i++)
		{
			//-------------------------------1.��ع�ѹ���-------------------------------//
			/* 1.1 ����ǰ�ι��ϼ���ʱ */
			ucGProtCntPre[0][i] = ucGProtCntCur[0][i];	
			/* 1.2 ���ϼ���߼� */			
			if((Tier2.gProt[i].VbatThrH.isEnable & 0x80) == 0)
			{
				if(ProtectMaskFlag[i] != 1)
				{
					if(Tier2.chRecord[i].Vbat > Tier2.gProt[i].VbatThrH.value)
					{
						ucGProtCntCur[0][i]++;
						if(ucGProtCntCur[0][i] >= PROT_TRIG_MS(400, 400))
						{
							GP_Alarm_Flag[i] |= GlobalProtect_OV;
							ucGProtCntCur[0][i] = 0;
						}
					}
				}
			}
			/* 1.3 �жϹ����Ƿ�������� */
			if(ucGProtCntPre[0][i] == ucGProtCntCur[0][i])
			{
				ucGProtCntPre[0][i] = 0;
				ucGProtCntCur[0][i] = 0;
			}
			
			//-------------------------------2.���Ƿѹ���-------------------------------//
			/* 2.1 ����ǰ�ι��ϼ���ʱ */
			ucGProtCntPre[1][i] = ucGProtCntCur[1][i];	
			/* 2.2 ���ϼ���߼� */	
			if((Tier2.gProt[i].VbatThrL.isEnable & 0x80) == 0)
			{
				if( ((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)  || \
					 (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_WAIT   )) && \
					(ProtectMaskFlag[i] != 1) )
				{
					if(Tier2.chRecord[i].Vbat < Tier2.gProt[i].VbatThrL.value)
					{
						ucGProtCntCur[1][i]++;
						if(ucGProtCntCur[1][i] >= PROT_TRIG_MS(400, 400))
						{
							GP_Alarm_Flag[i] |= GlobalProtect_UV;
							ucGProtCntCur[1][i] = 0;
						}
					}
				}
			}
			/* 2.3 �жϹ����Ƿ�������� */
			if(ucGProtCntPre[1][i] == ucGProtCntCur[1][i])
			{
				ucGProtCntPre[1][i] = 0;
				ucGProtCntCur[1][i] = 0;
			}			
			
			//-------------------------------3.���������-------------------------------//
			/* 3.1 ����ǰ�ι��ϼ���ʱ */
			ucGProtCntPre[2][i] = ucGProtCntCur[2][i];	
			/* 3.2 ���ϼ���߼� */				
			if((Tier2.gProt[i].Iovcharge.isEnable & 0x80) == 0)
			{
				uint8_t err_check_flg = 0;
				
				if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) && \
					(ProtectMaskFlag[i] != 1) )		
				{
					err_check_flg = 1;
				}
				else if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING  ) && \
						 ((Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CC    ) || \
						  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CV    ) || \
						  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CCTOCV)) && \
						 (ProtectMaskFlag[i] != 1) )
				{
					err_check_flg = 1;
				}
				
				if(err_check_flg == 1)
				{
					if(Tier2.chRecord[i].Iout > Tier2.gProt[i].Iovcharge.value)
					{
						ucGProtCntCur[2][i]++;
						if(ucGProtCntCur[2][i] >= PROT_TRIG_MS(400, 400))
						{
							GP_Alarm_Flag[i] |= GlobalProtect_COA;
							ucGProtCntCur[2][i] = 0;
						}
					}
				}
			}
			/* 3.3 �жϹ����Ƿ�������� */
			if(ucGProtCntPre[2][i] == ucGProtCntCur[2][i])
			{
				ucGProtCntPre[2][i] = 0;
				ucGProtCntCur[2][i] = 0;
			}
			
			//-------------------------------4.�ŵ�������-------------------------------//
			/* 4.1 ����ǰ�ι��ϼ���ʱ */
			ucGProtCntPre[3][i] = ucGProtCntCur[3][i];	
			/* 4.2 ���ϼ���߼� */	
			if((Tier2.gProt[i].Iovdischg.isEnable & 0x80) == 0)
			{
				if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING    ) && \
					((Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCC     ) || \
					 (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCV     ) || \
					 (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV)) && \
					(ProtectMaskFlag[i] != 1) )
				{
					if(Tier2.chRecord[i].Iout > Tier2.gProt[i].Iovdischg.value)
					{
						ucGProtCntCur[3][i]++;
						if(ucGProtCntCur[3][i] >= PROT_TRIG_MS(400, 400))
						{
							GP_Alarm_Flag[i] |= GlobalProtect_DOA;
							ucGProtCntCur[3][i] = 0;
						}
					}			
				}		
			}
			/* 4.3 �жϹ����Ƿ�������� */
			if(ucGProtCntPre[3][i] == ucGProtCntCur[3][i])
			{
				ucGProtCntPre[3][i] = 0;
				ucGProtCntCur[3][i] = 0;
			}			

			//-------------------------------5.���������ݼ��-------------------------------//
		}
	}
	
    return 1;
}

/**********************************************************************************************************
**  �� �� �� : AlarmQuarry()
**  ����˵�� : ��ѯʵʱ����״̬
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void AlarmQuarry(void)
{
    uint8_t  i = 0;
    uint16_t cnt = 0;		//���������
    uint16_t lenth = 0;		//����
    uint16_t crc16 = 0;		//У����
    uint8_t AlarmSendBuf[1024] = {0};

    AlarmSendBuf[0] = 0x5B;
    AlarmSendBuf[1] = 0x5B;	//��λ������λ������֡ͷ0x5B5B
    AlarmSendBuf[2] = 0x09;
    AlarmSendBuf[3] = 0x00;	//�̶�����9
    AlarmSendBuf[4] = 0x00;	//�����룬ÿ���ϱ��Լ�1
    AlarmSendBuf[5] = 0x00;	//״̬��0-OK
    AlarmSendBuf[6] = 0x00;	//ͨ����

    IntelProtectRunParmCheck();    //�ڲ��������
    GlobleProtectRunParmCheck();   //ȫ�ֱ������

    /*�ڲ�������ȫ�ֱ�����ѯ���ף���ͨ����ѯ*/
    for(i = 0; i < CHANNEL_CFG; i++)
    {
        if(IP_Alarm_Flag[i] != 0)
        {
            AlarmSendBuf[4] = features++;	//������+1
            AlarmSendBuf[6] = 0x01 << i;	//�ڲ���������ͨ����
            AlarmSendBuf[7] = 0xA0;			//������ڲ���������,�̶�0xa0, ȫ�ֱ���������0x83
            for(uint8_t j = 0; j < (sizeof(IP_CheckList) / sizeof(CMDProject)); j++)
            {
                if(IP_CheckList[j].P_Flg & IP_Alarm_Flag[i])
                {
                    cnt++;
                    AlarmSendBuf[7 + cnt] = IP_CheckList[j].childCMD;
                }
            }
            lenth = 8 + cnt;
            AlarmSendBuf[2] = (lenth & 0x00ff) >> 0;	//����֡��
            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;
            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt);	//����CRCУ���ֶ�
            AlarmSendBuf[8 + cnt] = (crc16 & 0x00ff) >> 0;
            AlarmSendBuf[9 + cnt] = (crc16 & 0xff00) >> 8;
            AlarmSendBuf[10 + cnt] = 0xA5;				//���֡β
            AlarmSendBuf[11 + cnt] = 0xA5;
            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);		//���ڷ��ͱ�����Ϣ��0Ϊ������socket
            First_Alarm=1;
            cnt = 0;	//�������������
            bsp_DelayMS(1);
        }

        cnt = 0;		//�������������

        if(GP_Alarm_Flag[i] != 0)
        {
            AlarmSendBuf[4] = features++;	//������+1
            AlarmSendBuf[6] = 0x01 << i;	//����ͨ����
            AlarmSendBuf[7] = 0x83;			//������ڲ���������,�̶�0xa0, ȫ�ֱ���������0x83
            for(uint8_t j = 0; j < (sizeof(GP_CheckList) / sizeof(CMDProject)); j++)
            {
                if(GP_CheckList[j].P_Flg & GP_Alarm_Flag[i])
                {
                    cnt++;
                    AlarmSendBuf[7 + cnt] = GP_CheckList[j].childCMD;
                }
            }
            lenth = 8 + cnt;
            AlarmSendBuf[2] = (lenth & 0x00ff) >> 0;	//����֡��
            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;
            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt);	//����CRCУ���ֶ�
            AlarmSendBuf[8 + cnt] = (crc16 & 0x00ff) >> 0;
            AlarmSendBuf[9 + cnt] = (crc16 & 0xff00) >> 8;
            AlarmSendBuf[10 + cnt] = 0xA5;				//���֡β
            AlarmSendBuf[11 + cnt] = 0xA5;
            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);		//���ڷ��ͱ�����Ϣ��0Ϊ������socket
            First_Alarm=1;
            cnt = 0;   //�������������
            bsp_DelayMS(1);
        }
    }
	
    /*ACDC�澯��ѯ����*/
    uint16_t alarm1, alarm2;	//bit=1��λ���б仯��״̬Ϊ�澯��λ
    
    /*ACDC������֡ͷ2�ֽ�+���ݳ���2�ֽ�+������1�ֽ�+״̬��1�ֽ�+ͨ����־1�ֽ�+�����ֶ�2�ֽڣ�������+�����+У��2�ֽ�+֡β2�ֽ�*/
    acdc_AlarmBuf.FramHead     = 0x5B5B;	//��λ������λ������֡ͷ0x5B5B
    acdc_AlarmBuf.Datelen      = 0x09;		//�̶�����9
    acdc_AlarmBuf.FeatureCode  = 0x00;		//�����룬ÿ���ϱ��Լ�1
    acdc_AlarmBuf.StateCode    = 0x00;		//״̬��=0��OK
    acdc_AlarmBuf.ChanelFlag   = 0x0F;		//ACDC����ʱ���̶�0
    acdc_AlarmBuf.CommandCode  = 0xA1;		//ACDC����,�̶�
    acdc_AlarmBuf.ChildCommand = 0x00;		//����can�ϱ����ݺ��������붨��ȷ��
    acdc_AlarmBuf.CRC16Code    = 0x00;		//��������
    acdc_AlarmBuf.FramTail     = 0xA5A5;	//֡β

    //����ACDC��ǰ״̬���������ϱ����ݣ�ֵΪ1��bit
    alarm1 = ACDC.State[0];
    alarm2 = ACDC.State[1];

    //�жϹ�������ͨ��
    if(alarm1)
    {
        acdc_AlarmBuf.ChanelFlag |= 0x00;	//ACDC����������canid��ͨ����־Ϊ���ͨ��������ʱĬ��0
    }
    if(alarm2)
    {
        acdc_AlarmBuf.ChanelFlag |= 0x00;	//ACDC����������canid��ͨ����־Ϊ���ͨ��������ʱĬ��0
    }

    //����ʹ�õ�ACDC���ͣ�������Э���ֲ�ȷ����������
    switch(ACDC.Type)
    {
        case 1:
		case 11:		//���籨������
		{
            for(i = 0; i < 8; i++)
            {
                if(alarm1 & gdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = gdlist[i].ChildCom;	//����ȡ��������
                    acdc_AlarmBuf.FeatureCode = features++;		//������+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7);		//����CRCУ���ֶ�
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);		//���ڷ��ͱ�����Ϣ��0Ϊ������socket
                }
                if(alarm2 & gdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = gdlist[i].ChildCom;	//����ȡ��������
                    acdc_AlarmBuf.FeatureCode = features++;		//������+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7);		//����CRCУ���ֶ�
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);		//���ڷ��ͱ�����Ϣ��0Ϊ������socket
                }
            }
		}
		break;

        case 2:
		case 22:		//�յ±�������
		{
            for(i = 0; i < 11; i++)
            {
                if(alarm1 & pdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = pdlist[i].ChildCom;	//����ȡ��������
                    acdc_AlarmBuf.FeatureCode = features++;		//������+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7);		//����CRCУ���ֶ�
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);		//���ڷ��ͱ�����Ϣ��0Ϊ������socket
                }
                if(alarm2 & pdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = pdlist[i].ChildCom;	//����ȡ��������
                    acdc_AlarmBuf.FeatureCode = features++;		//������+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7);		//����CRCУ���ֶ�
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);		//���ڷ��ͱ�����Ϣ��0Ϊ������socket
                }
            }
		}
		break;
			
        default:
		{
		
		}
        break;	
	}

}

/**********************************************************************************************************
**  �� �� �� : Mod_En_Clear()
**  ����˵�� : ������Ӳ���澯
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void Mod_En_Clear(void)
{
    uint8_t i = 0;
    uint8_t chanal_flg = 0;	//����ͨ����־

    for(i = 0; i < CHANNEL_CFG; i++)
    {
        if((IP_Alarm_Flag[i]&IntelProtect_HPA) == IntelProtect_HPA)
        {
            chanal_flg |= (0x01 << i);
        }
    }

    switch(CHANNEL_CFG)
    {
        case 1:
		{
			if(chanal_flg)
			{
				GPIO_ResetPins(MOD_EN1_PORT, MOD_EN1_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN1_PORT, MOD_EN1_PIN);

				GPIO_ResetPins(MOD_EN2_PORT, MOD_EN2_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN2_PORT, MOD_EN2_PIN);

				GPIO_ResetPins(MOD_EN3_PORT, MOD_EN3_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN3_PORT, MOD_EN3_PIN);

				GPIO_ResetPins(MOD_EN4_PORT, MOD_EN4_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN4_PORT, MOD_EN4_PIN);

				GPIO_ResetPins(MOD_EN5_PORT, MOD_EN5_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN5_PORT, MOD_EN5_PIN);

				GPIO_ResetPins(MOD_EN6_PORT, MOD_EN6_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN6_PORT, MOD_EN6_PIN);

				GPIO_ResetPins(MOD_EN7_PORT, MOD_EN7_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN7_PORT, MOD_EN7_PIN);

				GPIO_ResetPins(MOD_EN8_PORT, MOD_EN8_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN8_PORT, MOD_EN8_PIN);
			}
		}
		break;

        case 2:
		{
			if(chanal_flg & 0x01)
			{
				GPIO_ResetPins(MOD_EN1_PORT, MOD_EN1_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN1_PORT, MOD_EN1_PIN);

				GPIO_ResetPins(MOD_EN2_PORT, MOD_EN2_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN2_PORT, MOD_EN2_PIN);

				GPIO_ResetPins(MOD_EN3_PORT, MOD_EN3_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN3_PORT, MOD_EN3_PIN);

				GPIO_ResetPins(MOD_EN4_PORT, MOD_EN4_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN4_PORT, MOD_EN4_PIN);
			}

			if((chanal_flg >> 1) & 0x01)
			{
				GPIO_ResetPins(MOD_EN5_PORT, MOD_EN5_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN5_PORT, MOD_EN5_PIN);

				GPIO_ResetPins(MOD_EN6_PORT, MOD_EN6_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN6_PORT, MOD_EN6_PIN);

				GPIO_ResetPins(MOD_EN7_PORT, MOD_EN7_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN7_PORT, MOD_EN7_PIN);

				GPIO_ResetPins(MOD_EN8_PORT, MOD_EN8_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN8_PORT, MOD_EN8_PIN);
			}
		}
		break;
		
        case 4:
		{
			if(chanal_flg & 0x01)
			{
				GPIO_ResetPins(MOD_EN1_PORT, MOD_EN1_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN1_PORT, MOD_EN1_PIN);

				GPIO_ResetPins(MOD_EN2_PORT, MOD_EN2_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN2_PORT, MOD_EN2_PIN);
			}

			if((chanal_flg >> 1) & 0x01)
			{
				GPIO_ResetPins(MOD_EN3_PORT, MOD_EN3_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN3_PORT, MOD_EN3_PIN);

				GPIO_ResetPins(MOD_EN4_PORT, MOD_EN4_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN4_PORT, MOD_EN4_PIN);
			}

			if((chanal_flg >> 2) & 0x01)
			{
				GPIO_ResetPins(MOD_EN5_PORT, MOD_EN5_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN5_PORT, MOD_EN5_PIN);

				GPIO_ResetPins(MOD_EN6_PORT, MOD_EN6_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN6_PORT, MOD_EN6_PIN);
			}

			if((chanal_flg >> 3) & 0x01)
			{
				GPIO_ResetPins(MOD_EN7_PORT, MOD_EN7_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN7_PORT, MOD_EN7_PIN);

				GPIO_ResetPins(MOD_EN8_PORT, MOD_EN8_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN8_PORT, MOD_EN8_PIN);
			}
		}
		break;
		
        case 8:
		{
			if(chanal_flg & 0x01)
			{
				GPIO_ResetPins(MOD_EN1_PORT, MOD_EN1_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN1_PORT, MOD_EN1_PIN);
			}

			if((chanal_flg >> 1) & 0x01)
			{
				GPIO_ResetPins(MOD_EN2_PORT, MOD_EN2_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN2_PORT, MOD_EN2_PIN);
			}

			if((chanal_flg >> 2) & 0x01)
			{
				GPIO_ResetPins(MOD_EN3_PORT, MOD_EN3_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN3_PORT, MOD_EN3_PIN);
			}

			if((chanal_flg >> 3) & 0x01)
			{
				GPIO_ResetPins(MOD_EN4_PORT, MOD_EN4_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN4_PORT, MOD_EN4_PIN);
			}

			if((chanal_flg >> 4) & 0x01)
			{
				GPIO_ResetPins(MOD_EN5_PORT, MOD_EN5_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN5_PORT, MOD_EN5_PIN);
			}

			if((chanal_flg >> 5) & 0x01)
			{
				GPIO_ResetPins(MOD_EN6_PORT, MOD_EN6_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN6_PORT, MOD_EN6_PIN);
			}

			if((chanal_flg >> 6) & 0x01)
			{
				GPIO_ResetPins(MOD_EN7_PORT, MOD_EN7_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN7_PORT, MOD_EN7_PIN);
			}

			if((chanal_flg >> 7) & 0x01)
			{
				GPIO_ResetPins(MOD_EN8_PORT, MOD_EN8_PIN);
				osDelay(1);
				GPIO_SetPins(MOD_EN8_PORT, MOD_EN8_PIN);
			}
		}
		break;
			
        default:
		{
		
		}
		break;
    }
}

/**********************************************************************************************************
**  �� �� �� : AlarmClear()
**  ����˵�� : ����ڲ���ȫ�ֱ���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void AlarmClear(void)
{
    uint8_t i = 0;

//	/*��ѯ���Ӳ�������澯*/
//	Mod_En_Clear();

	/*����յ¸澯״̬*/
	if((ACDC.Type == 2) || (ACDC.Type == 22))	//�յ�
	{
		uint32_t ID2 = 0xC600000B;
		uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		Tdata2[0] = 0x01;
		Tdata2[3] = 0x42;
		Tdata2[7] = 0x01;	//�㲥��澯
		CANx_SendData(APP_CAN1_UNIT, ID2, &Tdata2[0], 8);    
	}		
	
    /*���¸澯��־���ظ���������Զ�����*/
    for(i = 0; i < 8; i++)
    {
		IP_Alarm_Flag[i] = 0;
		IP_Alarm_Flag_History[i] = 0;
		GP_Alarm_Flag[i] = 0;
		GP_Alarm_Flag_History[i] = 0;
    }

    /*���ACDC�澯*/
    ACDC.State[0] = 0;
    ACDC.State[1] = 0;
}

/**********************************************************************************************************
**  �� �� �� : AppTaskProtection()
**  ����˵�� : ��������ִ�й���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void AppTaskProtection(void *argument)
{
    uint8_t i = 0;
    uint16_t cnt = 0;	//���������
    uint16_t lenth = 0;	//����
    uint16_t crc16 = 0;	//У����
    uint8_t AlarmSendBuf[1024] = {0};

    AlarmSendBuf[0] = 0x5B;		//��λ������λ������֡ͷ0x5B5B
    AlarmSendBuf[1] = 0x5B;
    AlarmSendBuf[2] = 0x09;		//�̶�����9
    AlarmSendBuf[3] = 0x00;
    AlarmSendBuf[4] = 0x00;		//�����룬ÿ���ϱ��Լ�1
    AlarmSendBuf[5] = 0x00;		//״̬��=0��OK
    AlarmSendBuf[6] = 0x00;		//ͨ����

    while(1)
    {
        if(osSemaphoreAcquire(IPGP_Alarm_Semaphore, osWaitForever) == osOK)	//�ȴ�ͬ���ź���
        {
			if(First_Alarm == 0)
            {
                /*�ڲ�������ȫ�ֱ�����ѯ���ף���ͨ����ѯ*/
                for(i = 0; i < CHANNEL_CFG; i++)
                {
                    if(IP_Alarm_Flag[i] != 0)	//�ڲ��澯��־��0
                    {
                        if(IP_Alarm_Flag[i] != IP_Alarm_Flag_History[i])	//�ڲ��澯��־��0�Ҳ�������ʷֵ,׼�����׸澯
                        {
							Protect_Action_Func(i, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP);	//ִ�и澯����
                            IP_Alarm_Flag_History[i] = IP_Alarm_Flag[i];	//�����µĸ澯��ʷ��¼
                            AlarmSendBuf[4] = features++;	//������+1
                            AlarmSendBuf[6] = 0x01 << i;	//�ڲ���������ͨ����
                            AlarmSendBuf[7] = 0xA0;			//������ڲ���������,�̶�0xa0, ȫ�ֱ���������0x83
                            for(uint8_t j = 0; j < (sizeof(IP_CheckList) / sizeof(CMDProject)); j++)
                            {
                                if(IP_CheckList[j].P_Flg & IP_Alarm_Flag[i])
                                {
                                    cnt++;
                                    AlarmSendBuf[7 + cnt] = IP_CheckList[j].childCMD;
                                }
                            }
                            lenth = 8 + cnt;
                            AlarmSendBuf[2] = (lenth & 0x00ff) >> 0;	//����֡��
                            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;
                            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt);	//����CRCУ���ֶ�
                            AlarmSendBuf[8 + cnt] = (crc16 & 0x00FF) >> 0;
                            AlarmSendBuf[9 + cnt] = (crc16 & 0xFF00) >> 8;
                            AlarmSendBuf[10 + cnt] = 0xa5;				//���֡β
                            AlarmSendBuf[11 + cnt] = 0xa5;
                            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);		//���ڷ��ͱ�����Ϣ��0Ϊ������socket
                            cnt = 0;	//�������������
                        }
                    }

                    cnt = 0;	//�������������

                    if(GP_Alarm_Flag[i] != 0)	//ȫ�ָ澯��־��0
                    {
                        if(GP_Alarm_Flag[i] != GP_Alarm_Flag_History[i])	//ȫ�ָ澯��־��0�Ҳ�������ʷֵ
                        {
                            Protect_Action_Func(i, PROTECT_GLOBAL, WORKSTEP_RUNSTA_REF_STOP);	//ִ�и澯����
                            GP_Alarm_Flag_History[i] = GP_Alarm_Flag[i];	//�����µĸ澯��ʷ��¼
                            AlarmSendBuf[4] = features++;	//������+1
                            AlarmSendBuf[6] = 0x01 << i;	//����ͨ����
                            AlarmSendBuf[7] = 0x83;			//������ڲ���������,�̶�0xa0, ȫ�ֱ���������0x83
                            for(uint8_t j = 0; j < (sizeof(GP_CheckList) / sizeof(CMDProject)); j++)
                            {
                                if(GP_CheckList[j].P_Flg & GP_Alarm_Flag[i])
                                {
                                    cnt++;
                                    AlarmSendBuf[7 + cnt] = GP_CheckList[j].childCMD;
                                }
                            }
                            lenth = 8 + cnt;
                            AlarmSendBuf[2] = (lenth & 0x00ff) >> 0;	//����֡��
                            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;
                            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt);	//����CRCУ���ֶ�
                            AlarmSendBuf[8 + cnt] = (crc16 & 0x00FF) >> 0;
                            AlarmSendBuf[9 + cnt] = (crc16 & 0xFF00) >> 8;
                            AlarmSendBuf[10 + cnt] = 0xa5;				//���֡β
                            AlarmSendBuf[11 + cnt] = 0xa5;
                            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);		//���ڷ��ͱ�����Ϣ��0Ϊ������socket
                            cnt = 0;	//�������������
                        }
                    }
                }
            }
            else
            {
                First_Alarm = 0;
                for(i = 0; i < CHANNEL_CFG; i++)
                {
                    if(IP_Alarm_Flag[i] != 0)	//�ڲ��澯��־��0
                    {
                        if(IP_Alarm_Flag[i] != IP_Alarm_Flag_History[i])	//�ڲ��澯��־��0�Ҳ�������ʷֵ,׼�����׸澯
                        {
                            Protect_Action_Func(i, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP);	//ִ�и澯��������
                            IP_Alarm_Flag_History[i] = IP_Alarm_Flag[i];	//�����µĸ澯��ʷ��¼
                        }
                    }

                    if(GP_Alarm_Flag[i] != 0)	//ȫ�ָ澯��־��0
                    {
                        if(GP_Alarm_Flag[i] != GP_Alarm_Flag_History[i])	//ȫ�ָ澯��־��0�Ҳ�������ʷֵ
                        {
                            Protect_Action_Func(i, PROTECT_GLOBAL, WORKSTEP_RUNSTA_REF_STOP);	//ִ�и澯����
                            GP_Alarm_Flag_History[i] = GP_Alarm_Flag[i];	//�����µĸ澯��ʷ��¼
                        }
                    }
                }
            }
			
            NVIC_EnableIRQ(APP_TMRA1_IRQn);
        }
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
