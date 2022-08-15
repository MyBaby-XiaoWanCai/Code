/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     : JN95
** --------------------------------------------------------------------------------------------------------
** File name   : AppTaskprotection.c
** Author      : wl
** Date        : 2021-09��02
** Description : Protection application task file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"
#include <math.h>

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint32_t IP_Alarm_Flag[8] = {0};			// �ڲ�������־λ
uint32_t GP_Alarm_Flag[8] = {0};			// ȫ�ֱ�����־λ
uint32_t IP_Alarm_Flag_History[8] = {0};	// �ڲ�������ʷ��־λ
uint32_t GP_Alarm_Flag_History[8] = {0};	// ȫ�ֱ�����ʷ��־λ
uint8_t  First_Alarm = 0;					// ��һ�α�����־(�����һ���ر�) 0-��һ�α��� 1-���ǵ�һ�α���
uint8_t  SysModeProtMaskFlag = 0;			// �������α�ʶ(0-�����ι��� 1-���ι���)
protect_alarmStruct Protect_Alarm;			// �ڲ�����֡

extern uint8_t features;					// ������
extern osSemaphoreId_t IPGP_Alarm_Semaphore;// IPGP�澯�ź���ID

extern acdc_alarmStruct acdc_AlarmBuf;      //����ACDC״̬��������buf
extern arr_list gdlist[];
extern arr_list pdlist[];

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
};

/*ȫ�ֱ���������������ұ�*/
CMDProject GP_CheckList[] =
{
    {0x01, GlobalProtect_OV, 0, 0},      	// ��ѹ����
    {0x02, GlobalProtect_UV, 0, 0},      	// Ƿѹ����
    {0x03, GlobalProtect_COA, 0, 0},     	// ����������
    {0x04, GlobalProtect_DOA, 0, 0},     	// �ŵ��������
    {0x05, GlobalProtect_OC, 0, 0},      	// ���ݱ���
    {0x06, GlobalProtect_MV, 0, 0},      	// ��ʼ��ѹ���ޱ���
    {0x07, GlobalProtect_LV, 0, 0},      	// ��ʼ��ѹ���ޱ���
    {0x08, IntelProtect_NONE1, 0, 0},    	// δ����
    {0x09, IntelProtect_NONE2, 0, 0},    	// δ����
    {0x0a, IntelProtect_VCV, 0, 0},      	// �Ӵ�����ѹ�仯��
    {0xfe, IntelProtect_OTE, 0, 0},      	// ��������
    {0xff, IntelProtect_INV, 0, 0},      	// ��Ч
};

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
uint32_t IntelProtectDeviceCheck(void);         //�ڲ�����Ӳ�����
uint32_t IntelProtectRunParmCheck(void);        //�ڲ��������в������
uint32_t GlobleProtectRunParmCheck(void);       //ȫ�ֱ������в������
void AppTaskProtection(void *argument);

extern uint64_t bsp_GetRunTime(void);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : IntelProtectDeviceCheck()
**  ����˵�� : �ڲ�����������⣬����can1,DA,AD,flash,ee,fram,��λ����⣬����ִ��һ��
**  ��    �� : ��
**  �� �� ֵ : �ڲ���������״̬
**********************************************************************************************************/
uint32_t IntelProtectDeviceCheck()
{
	if(SysModeProtMaskFlag == 0)
	{
		#if 0
		/* ���CPU�쳣,�ж��Ƿ����ⲿ���Ź���λ */
		if((RCC->CSR & RCC_CSR_PINRSTF) && (!(RCC->CSR & RCC_CSR_PORRSTF)))
		{
			RCC->CSR |= RCC_CSR_RMVF;				//���RCC_CSR_PINRSTF��־
			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{ IP_Alarm_Flag[j] |= IntelProtect_UHE; }
		}
		else
		{
			RCC->CSR |= RCC_CSR_RMVF;				//���RCC_CSR_PINRSTF��־
		}
		#endif
		
		//���NRST��λ���ź��ϵ���縴λ״̬
		if((bM4_RMU->RSTF0_b.PINRF&RMU_RSTF0_PINRF) && ((!(bM4_RMU->RSTF0_b.PORF&RMU_RSTF0_PORF)) || (!(bM4_RMU->RSTF0_b.PDRF&RMU_RSTF0_PDRF))))
		{
			bM4_RMU->RSTF0_b.CLRF |= RMU_RSTF0_CLRF;   //���CLRF��λ��־
			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{ 
				IP_Alarm_Flag[j] |= IntelProtect_UHE; 
			}
		}
		else
		{
			bM4_RMU->RSTF0_b.CLRF |= RMU_RSTF0_CLRF;   //���CLRF��λ��־
		}

		/* ���ACDC(can1)ģ��,���÷���ת�� */
		ACDC.Type = ACDC_Fan_Speed51_ModeTest();		//���ACDC���ͣ������÷��ȵ�51%
		if(ACDC.Type == 0)
		{
			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{ IP_Alarm_Flag[j] |= IntelProtect_C1E; } //can1����
		}

		/* ���DA,��ID,��AD7124_CfgADC(),AD7175_CfgADC()�ڲ��Ѽ�� */


		/* ���AD,��ID,��dacx0508_cfg()�ڲ��Ѽ�� */


		/* ���flash,��ID */
		uint16_t FlashID = 0;
		FlashID = SPI_Flash_ReadID();
		if((FlashID & 0xff00) != 0xEF00)			//ID����ֽ�0xEF����������Winbond
		{
			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{ IP_Alarm_Flag[j] |= IntelProtect_FLE; } //flash����
		}

		/*���fram,��ID*/
		uint32_t FramID = 0;
		MB85Ctrl_ReadID(&FramID);
		if((FramID & 0xff000000) != 0x04000000)		//ID����ֽ�0x04����������Ϊ��ʿ
		{
			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{ IP_Alarm_Flag[j] |= IntelProtect_FRE; }   //fram����
		}

		/*���EE,��д���������ֽ�*/
		uint8_t TestDate = features;
    if(ee_WriteBytesWithCheck(&TestDate, Last_addr - 2, 1) == 0)
		{
			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{ IP_Alarm_Flag[j] |= IntelProtect_EEE; }  //ee����
		}

//		/*����λ��,���üĴ�����������ֵ*/
//		if(AD5245_Vpro_Set(4.35) == 0)
//		{
//			IntelProtectFlg |= IntelProtect_PME;	//��λ������
//		}
	}

    return 1;
}

uint8_t ucProtCntPre[8][8] = {0};
uint8_t ucProtCntCur[8][8] = {0};
#define PROT_TRIG_TIME	5

/**********************************************************************************************************
**  �� �� �� : IntelProtectRunParmCheck()
**  ����˵�� : �ڲ��������в�����⣬���������������,bus��ѹ,busǷѹ�����в�����⣬ִ������1ms
**  ��    �� : ��
**  �� �� ֵ : �ڲ���������״̬
**********************************************************************************************************/
uint32_t IntelProtectRunParmCheck(void)
{	
    if(SysModeProtMaskFlag == 0)
    {
		for(uint8_t i = 0; i < CHANNEL_CFG; i++)
		{
			/* ���������� */
			if((Tier2.iProt[i].IoutThr.isEnable & 0x80) == 0)
			{
				if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_START  ) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_WAIT   ) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_PAUSE  ) )
				{
					if(Tier2.chRecord[i].Iout > Tier2.iProt[i].IoutThr.value)
					{
                        IP_Alarm_Flag[i] |= IntelProtect_OLA;
					}
				}
			}

			/* BUS��ѹ���,bus1��Ӧǰһ��ͨ��,bus2��Ӧ��һ��ͨ�� */
			if((Tier2.iProt[i].VbusThrH.isEnable& 0x80) == 0)
			{
				if(Tier2.sampleCal.Vbus1 > Tier2.iProt[i].VbusThrH.value)
				{
                    for(uint8_t j = 0; j < (CHANNEL_CFG / 2); j++)
                    {
						IP_Alarm_Flag[j] |= IntelProtect_BOV;
					}
				}
				if(Tier2.sampleCal.Vbus2 > Tier2.iProt[i].VbusThrH.value)
				{
                    for(uint8_t j = (CHANNEL_CFG / 2); j < CHANNEL_CFG; j++)
                    {
						IP_Alarm_Flag[j] |= IntelProtect_BOV;
					}
				}			
			}
			
			/* BUSǷѹ���,bus1��Ӧǰ4ͨ��,bus2��Ӧ��4ͨ�� */
			if((Tier2.iProt[i].VbusThrL.isEnable & 0x80) == 0)
			{
				if(Tier2.sampleCal.Vbus1 < Tier2.iProt[i].VbusThrL.value)
				{
                    for(uint8_t j = 0; j < (CHANNEL_CFG / 2); j++)
                    {
						IP_Alarm_Flag[j] |= IntelProtect_BUV;
					}
				}
				if(Tier2.sampleCal.Vbus2 < Tier2.iProt[i].VbusThrL.value)
				{
                    for(uint8_t j = (CHANNEL_CFG / 2); j < CHANNEL_CFG; j++)
                    {
						IP_Alarm_Flag[j] |= IntelProtect_BUV;
					}
				}		
			}

			/* ��ѹ���ȼ�� */
			ucProtCntPre[0][i] = ucProtCntCur[0][i];
			if((Tier2.iProt[i].VcvDiff.isEnable & 0x80) == 0)
			{	
				if( ((Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CV ) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CCTOCV  )  || \
					 (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCV) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV)) && \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
					(workstep_flag_CriticalZone[i] == 3) && (ProtectMaskFlag[i] != 1) )
				{
					if(fabs(Tier2.chRecord[i].Vbat - Tier2.workstepAnalyze[i].StepRunVol) > Tier2.iProt[i].VcvDiff.value)
					{
						ucProtCntCur[0][i]++;
						
						if(ucProtCntCur[0][i] >= PROT_TRIG_TIME)
						{
							IP_Alarm_Flag[i] |= IntelProtect_UPA;
							ucProtCntCur[0][i] = 0;
						}
					}
				}
			}
			if(ucProtCntPre[0][i] == ucProtCntCur[0][i])
			{
				ucProtCntPre[0][i] = 0;
				ucProtCntCur[0][i] = 0;
			}
					
			/* ģ����¼�� */
			if((Tier2.iProt[i].Totmod.isEnable & 0x80) == 0)
			{
				if(Tier2.chRecord[i].Tmod > Tier2.iProt[i].Totmod.value)
				{
                    IP_Alarm_Flag[i] |= IntelProtect_MOT;
				}
			}

			/* �������¼�� */
			if((Tier2.iProt[i].Totenv.isEnable & 0x80) == 0)
			{
				if(Tier2.sampleCal.Tamb > Tier2.iProt[i].Totenv.value)
				{
                    IP_Alarm_Flag[i] |= IntelProtect_EOT;
				}
			}

			/* ��ѹ�쳣��� */
			ucProtCntPre[1][i] = ucProtCntCur[1][i];
			if(SystemStatus.Neilsbed == Neilsbed_Nofit)			// �봲δѹ��
			{
				if((Tier2.iProt[i].SNERR.isEnable & 0x80) == 0)
				{
					if( (Tier2.chRecord[i].Vbat > Tier2.iProt[i].SNERR.value) || \
						(Tier2.chRecord[i].Vout > Tier2.iProt[i].SNERR.value) || \
						(Tier2.chRecord[i].Vtz  > Tier2.iProt[i].SNERR.value) )
					{
						ucProtCntCur[1][i]++;
						
						if(ucProtCntCur[1][i] >= PROT_TRIG_TIME)
						{
							IP_Alarm_Flag[i] |= IntelProtect_VNN;
							ucProtCntCur[1][i] = 0;
						}
					}
				}
			}
			else if(SystemStatus.Neilsbed == Neilsbed_Fitted)	// �봲��ѹ��
			{
				if( (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_RUNNING) && \
					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_CONTACT) && \
					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_START) && (ProtectMaskFlag[i] != 1))	// �ǳ�ŵ繤����
				{
					if((Tier2.iProt[i].SNERR.isEnable & 0x80) == 0)
					{
						if( (fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vtz ) > (2 * Tier2.iProt[i].SNERR.value)) || \
							(fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vout) > (2 * Tier2.iProt[i].SNERR.value)) || \
							(fabs(Tier2.chRecord[i].Vtz  - Tier2.chRecord[i].Vout) > (2 * Tier2.iProt[i].SNERR.value)) )
						{
							ucProtCntCur[1][i]++;
							
							if(ucProtCntCur[1][i] >= PROT_TRIG_TIME)
							{
								IP_Alarm_Flag[i] |= IntelProtect_VNN;
								ucProtCntCur[1][i] = 0;
							}
						}
					}
				}
			}
			if(ucProtCntPre[1][i] == ucProtCntCur[1][i])
			{
				ucProtCntPre[1][i] = 0;
				ucProtCntCur[1][i] = 0;
			}			
			
			/*��·ѹ����*/
			ucProtCntPre[2][i] = ucProtCntCur[2][i];
			if((Tier2.iProt[i].VloopDiff.isEnable & 0x80) == 0)
			{
				if((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && (ProtectMaskFlag[i] != 1))
				{
					if(fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vout) > Tier2.iProt[i].VloopDiff.value)
					{
						ucProtCntCur[2][i]++;
						
						if(ucProtCntCur[2][i] >= PROT_TRIG_TIME)
						{
							IP_Alarm_Flag[i] |= IntelProtect_CVP;
							ucProtCntCur[2][i] = 0;
						}						
					}			
				}
			}
			if(ucProtCntPre[2][i] == ucProtCntCur[2][i])
			{
				ucProtCntPre[2][i] = 0;
				ucProtCntCur[2][i] = 0;
			}
			
			/* �������ȼ�� */
			ucProtCntPre[3][i] = ucProtCntCur[3][i];
			if((Tier2.iProt[i].IccDiff.isEnable & 0x80) == 0)
			{
				if( ((Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CC ) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCC     )  || \
					 (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CV ) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CCTOCV  )  || \
					 (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCV) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV)) && \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
					(workstep_flag_CriticalZone[i] == 0) && (ProtectMaskFlag[i] != 1))
				{
					if(fabs(Tier2.chRecord[i].Iout - Tier2.workstepAnalyze[i].StepRunCur) > Tier2.iProt[i].IccDiff.value)
					{
						ucProtCntCur[3][i]++;
						
						if(ucProtCntCur[3][i] >= PROT_TRIG_TIME)
						{
							IP_Alarm_Flag[i] |= IntelProtect_APA;
							ucProtCntCur[3][i] = 0;
						}
					}
				}		
			}		
			if(ucProtCntPre[3][i] == ucProtCntCur[3][i])
			{
				ucProtCntPre[3][i]= 0;
				ucProtCntCur[3][i] = 0;
			}
			
			/*�����쳣���*/
			ucProtCntPre[4][i] = ucProtCntCur[4][i];
			if( (SystemStatus.Neilsbed == Neilsbed_Nofit) || \
				(SystemStatus.Neilsbed == Neilsbed_Fitting) )	// �봲δѹ��/ѹ����
			{
				if(Tier2.chRecord[i].Iout > 0.1f)
				{
					ucProtCntCur[4][i]++;
					
					if(ucProtCntCur[4][i] >= PROT_TRIG_TIME)
					{
						IP_Alarm_Flag[i] |= IntelProtect_CNN;
						ucProtCntCur[4][i] = 0;
					}
				}
			}
			else if(SystemStatus.Neilsbed == Neilsbed_Fitted)	// �봲��ѹ��
			{
				if( (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_RUNNING) && \
					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_CONTACT) && \
					(ProtectMaskFlag[i] != 1) )					// �ǳ�ŵ繤����
				{
					if(Tier2.chRecord[i].Iout > 0.1f)
					{
						ucProtCntCur[4][i]++;
						
						if(ucProtCntCur[4][i] >= PROT_TRIG_TIME)
						{
							IP_Alarm_Flag[i] |= IntelProtect_CNN;
							ucProtCntCur[4][i] = 0;
						}
					}			
				}
			}
			if(ucProtCntPre[4][i] == ucProtCntCur[4][i])
			{
				ucProtCntPre[4][i] = 0;
				ucProtCntCur[4][i] = 0;
			}
			
//			/*������ѹ�쳣��⣨��̽���ѹ�����*/
//			if(Tier2.chRecord[i].Vtz > Tier2.iProt[i].VauxThr.value)
//			{
//                IP_Alarm_Flag[i] |= IntelProtect_LOV;
//			}
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
	if(SysModeProtMaskFlag == 0)
	{
		for(uint8_t i = 0; i < CHANNEL_CFG; i++)
		{
			/*��ع�ѹ���*/
			if((Tier2.gProt[i].VbatThrH.isEnable & 0x80) == 0)
			{
				if(Tier2.chRecord[i].Vbat > Tier2.gProt[i].VbatThrH.value)
				{
					GP_Alarm_Flag[i] |= GlobalProtect_OV;
				}
			}

			/*���Ƿѹ���*/
			if((Tier2.gProt[i].VbatThrL.isEnable & 0x80) == 0)
			{
				if(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
				{
					if(Tier2.chRecord[i].Vbat < Tier2.gProt[i].VbatThrL.value)
					{
						GP_Alarm_Flag[i] |= GlobalProtect_UV;
					}
				}
			}

			/*���������*/
			if((Tier2.gProt[i].Iovcharge.isEnable & 0x80) == 0)
			{
				if(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT)
				{
					if(Tier2.chRecord[i].Iout > Tier2.gProt[i].Iovcharge.value)
					{
						GP_Alarm_Flag[i] |= GlobalProtect_COA;
					}			
				}
				if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
					( (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CC    ) || \
					  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CV    ) || \
					  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CCTOCV) ) )
				{
					if(Tier2.chRecord[i].Iout > Tier2.gProt[i].Iovcharge.value)
					{
						GP_Alarm_Flag[i] |= GlobalProtect_COA;
					}			
				}
			}

			/*�ŵ�������*/
			if((Tier2.gProt[i].Iovdischg.isEnable & 0x80) == 0)
			{
				if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
					( (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCC     ) || \
					  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCV     ) || \
					  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV) ) )
				{
					if(Tier2.chRecord[i].Iout > Tier2.gProt[i].Iovdischg.value)
					{
						GP_Alarm_Flag[i] |= GlobalProtect_DOA;
					}			
				}		
			}

//			/*���������ݼ��*/
//			if((Tier2.gProt[i].Cov.isEnable & 0x80) == 0)
//			{
//				if(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
//				{
//					if(Tier2.record[i].Cbat > Tier2.gProt[i].Cov.value)
//					{
//						GP_Alarm_Flag[i] |= GlobalProtect_OC;
//					}			
//				}
//			}
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
    uint8_t  i   = 0;
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
            for(uint8_t j = 0; j < sizeof(IP_CheckList) / sizeof(CMDProject); j++)
            {
                if(IP_CheckList[j].P_Flg & IP_Alarm_Flag[i])
                {
                    cnt++;
                    AlarmSendBuf[7 + cnt] = IP_CheckList[j].childCMD;
                }
            }
            lenth = 8 + cnt;
            AlarmSendBuf[2] = lenth & 0x00ff;	//����֡��
            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;
            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt);	//����CRCУ���ֶ�
            AlarmSendBuf[8 + cnt] = crc16 & 0x00ff;
            AlarmSendBuf[9 + cnt] = (crc16 & 0xff00) >> 8;
            AlarmSendBuf[10 + cnt] = 0xa5;		//���֡β
            AlarmSendBuf[11 + cnt] = 0xa5;
            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);	//���ڷ��ͱ�����Ϣ��0Ϊ������socket
            First_Alarm=1;
            cnt = 0;			//�������������
            bsp_DelayMS(1);
        }

        cnt = 0;		//�������������

        if(GP_Alarm_Flag[i] != 0)
        {
            AlarmSendBuf[4] = features++;        //������+1
            AlarmSendBuf[6] = 0x01 << i; //����ͨ����
            AlarmSendBuf[7] = 0x83;   //������ڲ���������,�̶�0xa0, ȫ�ֱ���������0x83
            for(uint8_t j = 0; j < sizeof(GP_CheckList) / sizeof(CMDProject); j++)
                //���������
            {
                if(GP_CheckList[j].P_Flg & GP_Alarm_Flag[i])
                {
                    cnt++;
                    AlarmSendBuf[7 + cnt] = GP_CheckList[j].childCMD;
                }
            }
            lenth = 8 + cnt;
            AlarmSendBuf[2] = lenth & 0x0ff; //����֡��
            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;

            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt); //����CRCУ���ֶ�
            AlarmSendBuf[8 + cnt] = crc16 & 0x00ff;
            AlarmSendBuf[9 + cnt] = (crc16 & 0xff00) >> 8;

            AlarmSendBuf[10 + cnt] = 0xa5; //���֡β
            AlarmSendBuf[11 + cnt] = 0xa5;

            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);  //���ڷ��ͱ�����Ϣ��0Ϊ������socket
            First_Alarm=1;
            cnt = 0;   //�������������
            bsp_DelayMS(1);
        }
    }
	
    /*ACDC�澯��ѯ����*/
    uint16_t alarm1, alarm2; //bit=1��λ���б仯��״̬Ϊ�澯��λ
    
    /*ACDC������֡ͷ2�ֽ�+���ݳ���2�ֽ�+������1�ֽ�+״̬��1�ֽ�+ͨ����־1�ֽ�+�����ֶ�2�ֽڣ�������+�����+У��2�ֽ�+֡β2�ֽ�*/
    acdc_AlarmBuf.FramHead = 0x5B5B;   //��λ������λ������֡ͷ0x5B5B
    acdc_AlarmBuf.Datelen = 0x09;      //�̶�����9
    acdc_AlarmBuf.FeatureCode = 0x00;  //�����룬ÿ���ϱ��Լ�1
    acdc_AlarmBuf.StateCode = 0x00;    //״̬��=0��OK
    acdc_AlarmBuf.ChanelFlag = 0x0F;   //ACDC����ʱ���̶�0
    acdc_AlarmBuf.CommandCode = 0xA1;  //ACDC����,�̶�
    acdc_AlarmBuf.ChildCommand = 0x00; //����can�ϱ����ݺ��������붨��ȷ��
    acdc_AlarmBuf.CRC16Code = 0x00;    //��������
    acdc_AlarmBuf.FramTail = 0xA5A5;   //֡β

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
        case 1:    //���籨������
		{
            for(i = 0; i < 8; i++)
            {
                if(alarm1 & gdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = gdlist[i].ChildCom; //����ȡ��������
                    acdc_AlarmBuf.FeatureCode = features++;         //������+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //����CRCУ���ֶ�
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //���ڷ��ͱ�����Ϣ��0Ϊ������socket
                }
                if(alarm2 & gdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = gdlist[i].ChildCom; //����ȡ��������
                    acdc_AlarmBuf.FeatureCode = features++;         //������+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //����CRCУ���ֶ�
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //���ڷ��ͱ�����Ϣ��0Ϊ������socket
                }
            }
		}
		break;

        case 2:   //�յ±�������
		{
            for(i = 0; i < 11; i++)
            {
                if(alarm1 & pdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = pdlist[i].ChildCom; //����ȡ��������
                    acdc_AlarmBuf.FeatureCode = features++;         //������+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //����CRCУ���ֶ�
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //���ڷ��ͱ�����Ϣ��0Ϊ������socket
                }
                if(alarm2 & pdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = pdlist[i].ChildCom; //����ȡ��������
                    acdc_AlarmBuf.FeatureCode = features++;         //������+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //����CRCУ���ֶ�
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //���ڷ��ͱ�����Ϣ��0Ϊ������socket
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
    uint8_t i;
    uint8_t chanal_flg = 0; //����ͨ����־

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
            break;

        case 2:
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

            break;
        case 4:
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
            break;
		
        case 8:
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

            break;
			
        default:
			
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
    uint8_t i;

    /*��ѯ���Ӳ�������澯*/
//    Mod_En_Clear();

	/*����յ¸澯״̬*/
	if(ACDC.Type==2)
	{
	 uint32_t ID2 = 0xC600000B;
	 uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	 Tdata2[0] = 0x01;
	 Tdata2[3] = 0x42;
	 Tdata2[7] = 0x01;      //�㲥��澯
	 CANx_SendData(APP_CAN_UNIT, ID2, &Tdata2[0], 8);    
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
    uint8_t i;
    uint16_t cnt = 0; //���������
    uint16_t lenth;  //����
    uint16_t crc16;  //У����

    uint8_t AlarmSendBuf[1024] = {0};

    AlarmSendBuf[0] = 0x5B;
    AlarmSendBuf[1] = 0x5B;    //��λ������λ������֡ͷ0x5B5B

    AlarmSendBuf[2] = 0x09;
    AlarmSendBuf[3] = 0x00;    //�̶�����9

    AlarmSendBuf[4] = 0x00;    //�����룬ÿ���ϱ��Լ�1
    AlarmSendBuf[5] = 0x00;    //״̬��=0��OK
    AlarmSendBuf[6] = 0x00;    //ͨ����


    while(1)
    {
        if(osSemaphoreAcquire(IPGP_Alarm_Semaphore, osWaitForever) == osOK) //�ȴ�ͬ���ź���
        {
            if(First_Alarm == 0)
            {
                /*�ڲ�������ȫ�ֱ�����ѯ���ף���ͨ����ѯ*/
                for(i = 0; i < CHANNEL_CFG; i++)
                {
                    if(IP_Alarm_Flag[i] != 0) //�ڲ��澯��־��0
                    {

                        if(IP_Alarm_Flag[i] != IP_Alarm_Flag_History[i]) //�ڲ��澯��־��0�Ҳ�������ʷֵ,׼�����׸澯
                        {
                            Protect_Action_Func(i, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP);  //ִ�и澯����
                            IP_Alarm_Flag_History[i] = IP_Alarm_Flag[i]; //�����µĸ澯��ʷ��¼
                            AlarmSendBuf[4] = features++;        //������+1
                            AlarmSendBuf[6] = 0x01 << i; //�ڲ���������ͨ����
                            AlarmSendBuf[7] = 0xA0;   //������ڲ���������,�̶�0xa0, ȫ�ֱ���������0x83
                            for(uint8_t j = 0; j < sizeof(IP_CheckList) / sizeof(CMDProject); j++)
                                //���������
                            {
                                if(IP_CheckList[j].P_Flg & IP_Alarm_Flag[i])
                                {
                                    cnt++;
                                    AlarmSendBuf[7 + cnt] = IP_CheckList[j].childCMD;
                                }
                            }
                            lenth = 8 + cnt;
                            AlarmSendBuf[2] = lenth & 0x00ff; //����֡��
                            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;

                            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt); //����CRCУ���ֶ�
                            AlarmSendBuf[8 + cnt] = crc16 & 0x00FF ;
                            AlarmSendBuf[9 + cnt] = (crc16 & 0xFF00) >> 8;

                            AlarmSendBuf[10 + cnt] = 0xa5; //���֡β
                            AlarmSendBuf[11 + cnt] = 0xa5;

                            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);  //���ڷ��ͱ�����Ϣ��0Ϊ������socket

                            cnt = 0;   //�������������

                        }
                    }

                    cnt = 0;   //�������������

                    if(GP_Alarm_Flag[i] != 0) //ȫ�ָ澯��־��0
                    {

                        if(GP_Alarm_Flag[i] != GP_Alarm_Flag_History[i]) //ȫ�ָ澯��־��0�Ҳ�������ʷֵ
                        {
                            Protect_Action_Func(i, PROTECT_GLOBAL, WORKSTEP_RUNSTA_REF_STOP);//ִ�и澯����
                            GP_Alarm_Flag_History[i] = GP_Alarm_Flag[i]; //�����µĸ澯��ʷ��¼
                            AlarmSendBuf[4] = features++;        //������+1
                            AlarmSendBuf[6] = 0x01 << i; //����ͨ����
                            AlarmSendBuf[7] = 0x83;   //������ڲ���������,�̶�0xa0, ȫ�ֱ���������0x83
                            for(uint8_t j = 0; j < sizeof(GP_CheckList) / sizeof(CMDProject); j++)
                                //���������
                            {
                                if(GP_CheckList[j].P_Flg & GP_Alarm_Flag[i])
                                {
                                    cnt++;
                                    AlarmSendBuf[7 + cnt] = GP_CheckList[j].childCMD;
                                }
                            }
                            lenth = 8 + cnt;
                            AlarmSendBuf[2] = (lenth & 0x00ff); //����֡��
                            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;

                            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt); //����CRCУ���ֶ�
                            AlarmSendBuf[8 + cnt] = crc16 & 0x00FF;
                            AlarmSendBuf[9 + cnt] = (crc16 & 0xFF00) >> 8;

                            AlarmSendBuf[10 + cnt] = 0xa5; //���֡β
                            AlarmSendBuf[11 + cnt] = 0xa5;

                            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);  //���ڷ��ͱ�����Ϣ��0Ϊ������socket
                            cnt = 0;   //�������������

                        }
                    }
                }


            }
            else
            {
                First_Alarm = 0;
                for(i = 0; i < CHANNEL_CFG; i++)
                {
                    if(IP_Alarm_Flag[i] != 0) //�ڲ��澯��־��0
                    {
                        if(IP_Alarm_Flag[i] != IP_Alarm_Flag_History[i]) //�ڲ��澯��־��0�Ҳ�������ʷֵ,׼�����׸澯
                        {
                            Protect_Action_Func(i, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP);  //ִ�и澯��������
                            IP_Alarm_Flag_History[i] = IP_Alarm_Flag[i]; //�����µĸ澯��ʷ��¼
                        }
                    }

                    if(GP_Alarm_Flag[i] != 0) //ȫ�ָ澯��־��0
                    {

                        if(GP_Alarm_Flag[i] != GP_Alarm_Flag_History[i]) //ȫ�ָ澯��־��0�Ҳ�������ʷֵ
                        {
                            Protect_Action_Func(i, PROTECT_GLOBAL, WORKSTEP_RUNSTA_REF_STOP);//ִ�и澯����
                            GP_Alarm_Flag_History[i] = GP_Alarm_Flag[i]; //�����µĸ澯��ʷ��¼
                        }
                    }
                }

            }
			
            NVIC_EnableIRQ(APP_TMRA1_IRQn);//HAL_NVIC_EnableIRQ(TIM3_IRQn);
        }
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
