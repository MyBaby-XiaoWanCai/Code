/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Dcdc.c
** Author      : Huang Cheng
** Date        : 2022-01-20
** Description : 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Task_Dcdc.h"

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void AppTaskDcdc(void *argument);

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
static uint8_t CheckSum(uint8_t *pData, uint8_t DataLen);
static uint8_t Bit2ChNum(DCDC_FrameType *DcdcRxFrame);
static void DcdcSetCmdRep(DCDC_FrameType *DcdcRxFrame);
static void DcdcProtectInqRep(DCDC_FrameType *DcdcRxFrame);
static void DcdcProtectTrig(DCDC_FrameType *DcdcRxFrame);
static void DcdcRecordDataInqRep(DCDC_FrameType *DcdcRxFrame);
static void DcdcCtrlParamInqRep(DCDC_FrameType *DcdcRxFrame);
static void DcdcClibParamInqRep(DCDC_FrameType *DcdcRxFrame);
static void DcdcAdcDataReadRep(DCDC_FrameType *DcdcRxFrame);
static void DcdcSystemInqRep(DCDC_FrameType *DcdcRxFrame);
static DCDC_FrameValidType IsFrameValid(void);

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
DCDC_FrameType DcdcRecvBuffer = {0};
DCDC_FrameType DcdcRecvFrame = {0};
DCDC_FrameType DcdcSendFrame = {0};
DCDC_ParamType DcdcParam = {0};

uint8_t AppDataBuffer[1024] = {0};
uint8_t dcdc_err_flag = 0;

uint32_t DcdcCanId = 0;  //DCDC CANͨ��ID
uint32_t MaskCanId = 0;  //CAN������

//��������ת��
static void (*DcdcRepHandle[])(DCDC_FrameType*) = 
{
	DcdcSetCmdRep,          //����������Ӧ������
	DcdcProtectInqRep,      //�������ò�ѯ�ظ�������
	DcdcProtectTrig,        //��������������
	DcdcRecordDataInqRep,   //��¼���ݲ�ѯ�ظ�������
	DcdcSetCmdRep,          //���Ʋ���������Ӧ������
	DcdcSetCmdRep,          //����״̬������Ӧ������
	DcdcCtrlParamInqRep,    //���Ʋ�����ѯ�ظ�������
	DcdcSetCmdRep,          //У׼����������Ӧ������
	DcdcClibParamInqRep,    //У׼������ѯ�ظ�������
	DcdcAdcDataReadRep,     //AD���ݶ�ȡ�ظ�������
	DcdcSystemInqRep,       //ϵͳ��ѯ�ظ�������
	DcdcSetCmdRep,          //ϵͳ������Ӧ������
};

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/
/**********************************************************************************************************
**  �� �� �� : CheckSum()
**  ����˵�� : У��ͺ���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
static uint8_t CheckSum(uint8_t *pData, uint8_t DataLen)
{
	uint8_t Crc = 0;
	uint32_t TempSum = 0;
	
	for(uint8_t i = 0; i < DataLen; i++)
	{
		TempSum += pData[i];
	}
	
	Crc = TempSum & 0xFF;
	return Crc;
}

/**********************************************************************************************************
**  �� �� �� : Bit2ChNum()
**  ����˵�� : Bitת��ͨ����
**  ��    �� : *DcdcRxFrame - ��������֡
**  �� �� ֵ : ChNum - ͨ����
**********************************************************************************************************/
static uint8_t Bit2ChNum(DCDC_FrameType *DcdcRxFrame)
{
	uint8_t ChNum = 0x00;
	
	for(uint8_t i = 0; i < CHANNEL_NUM; i++)
	{
		if(0x01 == ((DcdcRxFrame->ChFlag>>i)&0x01))
		{
			ChNum = i + 1;
			break;
		}
		else
		{
			//do nothing;
		}
	}
	
	return ChNum;
}

/**********************************************************************************************************
**  �� �� �� : DcdcParaInq()
**  ����˵�� : DCDC������ѯ����
**  ��    �� : ChNum - ͨ������MainCmdType - �����������ͣ�SubCmdType - ������������
**  �� �� ֵ : ��
**  ˵    �� : ���еĲ�ѯ�������ô˺���
**********************************************************************************************************/
void DcdcParaInq(uint8_t ChNum, uint8_t MainCmdType, uint8_t SubCmdType)
{
	if((INVLID_SUB_CMD==MainCmdType) || (INVLID_SUB_CMD==SubCmdType))
	{
		return;
	}
	
	DcdcSendFrame.ChFlag = ChNum;
	DcdcSendFrame.MainCmd = MainCmdType;
	DcdcSendFrame.SubCmd = SubCmdType;
	memset(DcdcSendFrame.Data, 0, 4);
	DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
}

/**********************************************************************************************************
**  �� �� �� : DcdcProtectSet()
**  ����˵�� : DCDC�������ú���
**  ��    �� : ChNum - ͨ������ProtectType - �������ͣ�ProtectPara - ����ֵ
**  �� �� ֵ : ��
**********************************************************************************************************/
void DcdcProtectSet(uint8_t ChNum, uint8_t ProtectType, float ProtectPara)
{
	if(INVLID_SUB_CMD == ProtectType)
	{
		return;
	}
	
	DcdcSendFrame.ChFlag = ChNum;
	DcdcSendFrame.MainCmd = ProtectSet;
	DcdcSendFrame.SubCmd = ProtectType;
	memcpy(DcdcSendFrame.Data, &ProtectPara, 4);
	DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
}

/**********************************************************************************************************
**  �� �� �� : DcdcRunModeSet()
**  ����˵�� : DCDC����ģʽ���ú���
**  ��    �� : ChNum - ͨ������RunMode - ģʽ
**  �� �� ֵ : ��
**********************************************************************************************************/
void DcdcRunModeSet(uint8_t ChNum, uint8_t RunMode)
{
	if(INVLID_SUB_CMD == RunMode)
	{
		return;
	}
	
	DcdcSendFrame.ChFlag = ChNum;
	DcdcSendFrame.MainCmd = CtrlParamSet;
	DcdcSendFrame.SubCmd = 0x01;
	
	memset(DcdcSendFrame.Data, 0x00, 4);       //ģʽ����
	DcdcSendFrame.Data[0] = RunMode;
	
	DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
}

/**********************************************************************************************************
**  �� �� �� : DcdcCtrlParaSet()
**  ����˵�� : DCDC���Ʋ������ú���
**  ��    �� : ChNum - ͨ������ParaType - �������ͣ�RunPara - ���в���
**  �� �� ֵ : ��
**********************************************************************************************************/
void DcdcCtrlParaSet(uint8_t ChNum, uint8_t ParaType, float RunPara)
{
	if(INVLID_SUB_CMD == ParaType)
	{
		return;
	}
	
	DcdcSendFrame.ChFlag = ChNum;
	DcdcSendFrame.MainCmd = CtrlParamSet;
	DcdcSendFrame.SubCmd = ParaType;
	
	//����ģʽ��������Ϊ����
	if(CTRL_RUN_MODE == ParaType)
	{
		memset(DcdcSendFrame.Data, 0x00, 4);
		DcdcSendFrame.Data[0] = (uint8_t)RunPara;
	}
	else
	{
		memcpy(DcdcSendFrame.Data, &RunPara, 4);
	}
	
	DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
}

/**********************************************************************************************************
**  �� �� �� : DcdcRunStaSet()
**  ����˵�� : DCDC����״̬���ú���
**  ��    �� : ChNum - ͨ������RunSta - ����״̬��1���У�2ֹͣ
**  �� �� ֵ : ��
**********************************************************************************************************/
void DcdcRunStaSet(uint8_t ChNum, uint8_t RunSta)
{
	if(INVLID_SUB_CMD == RunSta)
	{
		return;
	}
	
	DcdcSendFrame.ChFlag = ChNum;
	DcdcSendFrame.MainCmd = RunStaSet;
	DcdcSendFrame.SubCmd = RunSta;
	memset(DcdcSendFrame.Data, 0x00, 4);
	DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
}

/**********************************************************************************************************
**  �� �� �� : DcdcClibParamSet()
**  ����˵�� : DCDCУ׼�������ú���
**  ��    �� : ChNum - ͨ������ParaType - ��������, ClibPara - У׼����
**  �� �� ֵ : ��
**********************************************************************************************************/
void DcdcClibParamSet(uint8_t ChNum, uint8_t ParaType, float ClibPara)
{
	if(INVLID_SUB_CMD == ParaType)
	{
		return;
	}
	
	DcdcSendFrame.ChFlag = ChNum;
	DcdcSendFrame.MainCmd = ClibParamSet;
	DcdcSendFrame.SubCmd = ParaType;
	memcpy(DcdcSendFrame.Data, &ClibPara, 4);
	DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
}

/**********************************************************************************************************
**  �� �� �� : DcdcAdcDataRead()
**  ����˵�� : DCDCУ׼�������ú���
**  ��    �� : ChNum - ͨ������ParaType - ��������
**  �� �� ֵ : ��
**********************************************************************************************************/
void DcdcAdcDataRead(uint8_t ChNum, uint8_t ParaType)
{
	if(INVLID_SUB_CMD == ParaType)
	{
		return;
	}
	
	DcdcSendFrame.ChFlag = ChNum;
	DcdcSendFrame.MainCmd = AdcDataRead;
	DcdcSendFrame.SubCmd = ParaType;
	memset(DcdcSendFrame.Data, 0x00, 4);
	DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
}

/**********************************************************************************************************
**  �� �� �� : DcdcSystemSet()
**  ����˵�� : DCDCϵͳ���ú���
**  ��    �� : ParaType - ��������
**  �� �� ֵ : ��
**********************************************************************************************************/
void DcdcSystemSet(uint8_t ParaType)
{
	if(INVLID_SUB_CMD == ParaType)
	{
		return;
	}
	
	DcdcSendFrame.ChFlag = 0x00;          //�˴�������ͨ��
	DcdcSendFrame.MainCmd = SystemSet;
	DcdcSendFrame.SubCmd = ParaType;
	memset(DcdcSendFrame.Data, 0x00, 4);
	DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
}

/**********************************************************************************************************
**  �� �� �� : DcdcSystemUpdate()
**  ����˵�� : DCDCϵͳ��������
**  ��    �� : ParaType - ��������
**  �� �� ֵ : ��
**********************************************************************************************************/
void DcdcSystemUpdate(uint8_t ParaType)
{
	if(INVLID_SUB_CMD == ParaType)
	{
		return;
	}
	
	//дDCDC��flash����Ҫ��������
	if(SYSTEM_WRITE_FLASH == ParaType)
	{
		//do something;
	}
	else
	{
		DcdcSendFrame.ChFlag = 0x00;          //�˴�������ͨ��
		DcdcSendFrame.MainCmd = SystemUpdate;
		DcdcSendFrame.SubCmd = ParaType;
		memset(DcdcSendFrame.Data, 0x00, 4);
		DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
	}
}

/**********************************************************************************************************
**  �� �� �� : DcdcSetCmdRep()
**  ����˵�� : DCDC����������Ӧ����
**  ��    �� : *DcdcRxFrame - ��������֡
**  �� �� ֵ : ��
**  ˵    �� : ���е���Ӧ�������ô˺���
**********************************************************************************************************/
static void DcdcSetCmdRep(DCDC_FrameType *DcdcRxFrame)
{
	if(SET_OK == DcdcRxFrame->Data[0])
	{
		//do nothing;
		__NOP();
	}
	else
	{
		__NOP();
	}
}

/**********************************************************************************************************
**  �� �� �� : DcdcProtectInqRep()
**  ����˵�� : DCDC����������ѯ�ظ�����
**  ��    �� : *DcdcRxFrame - ��������֡
**  �� �� ֵ : ��
**********************************************************************************************************/
static void DcdcProtectInqRep(DCDC_FrameType *DcdcRxFrame)
{
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	
	//ͨ����Ϊ��Чֵ
	if(0x00 == CurtCh)
	{
		return;
	}
	
	switch(DcdcRxFrame->SubCmd)
	{
		case PROTECT_LIMIT_VOL:
		{
			//���������ѹ������ֵ
			memcpy(&DcdcParam.OvpVal[CurtCh-1], DcdcRxFrame->Data, 4);
			break;
		}
		case PROTECT_LIMIT_CURT:
		{
			//�����������������ֵ
			memcpy(&DcdcParam.OcpVal[CurtCh-1], DcdcRxFrame->Data, 4);
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**  �� �� �� : DcdcProtectTrig()
**  ����˵�� : DCDC������������
**  ��    �� : *DcdcRxFrame - ��������֡
**  �� �� ֵ : ��
**********************************************************************************************************/
static void DcdcProtectTrig(DCDC_FrameType *DcdcRxFrame)
{
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	
	//ͨ����Ϊ��Чֵ
	if(0x00 == CurtCh)
	{
		return;
	}
	
	switch(DcdcRxFrame->SubCmd)
	{
		case TRIGGER_IND_CURT:
		case TRIGGER_OUT_VOL:
		case TRIGGER_OUT_CURT:
		case TRIGGER_BUS_HIGH:
		case TRIGGER_BUS_LOW:
		case TRIGGER_HARDWARE:
		case TRIGGER_FAN:
		case TRIGGER_WDG_RESET:
		case TRIGGER_REV_CONNET:
		{
			dcdc_err_flag = 1;
			
			if(0x01 == DcdcRxFrame->Data[0])
			{
				//������λ
				DcdcParam.ProtectAlarm[CurtCh-1] |= 0x01 << (DcdcRxFrame->SubCmd-1);
			}
			else
			{
				//�������
				DcdcParam.ProtectAlarm[CurtCh-1] &= ~(0x01<<(DcdcRxFrame->SubCmd-1));
			}
			
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**  �� �� �� : DcdcRecordDataInqRep()
**  ����˵�� : DCDC��¼������ѯ�ظ���Ӧ����
**  ��    �� : *DcdcRxFrame - ��������֡
**  �� �� ֵ : ��
**********************************************************************************************************/
static void DcdcRecordDataInqRep(DCDC_FrameType *DcdcRxFrame)
{
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	
	//ͨ����Ϊ��Чֵ
	if(0x00 == CurtCh)
	{
		return;
	}
	
	switch(DcdcRxFrame->SubCmd)
	{
		case RECORD_RUN_STATUS:
		{
			//������¼��������״̬
			DcdcParam.RunSta[CurtCh-1] = DcdcRxFrame->Data[0];
			DcdcParam.RunMode[CurtCh-1] = DcdcRxFrame->Data[1];
			memcpy(&DcdcParam.Fault[CurtCh-1], &DcdcRxFrame->Data[2], 2);
			break;
		}
		case RECORD_SENSE_VOL:
		{
			//����sense��ѹ
			memcpy(&DcdcParam.SenseVol[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case RECORD_OUT_CURT:
		{
			//�����������
			memcpy(&DcdcParam.OutCurt[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			
			for(int i = 0; i < CHANNEL_CFG; i++)
			{
				Tier2.chRecord[i].Iout = DcdcParam.OutCurt[CurtCh-1];
				Tier2.debug.inq1[i].IoutDebug = DcdcParam.OutCurt[CurtCh-1];
			}
			
			break;
		}
		case RECORD_OUT_VOL:
		{
			//���������ѹ
			memcpy(&DcdcParam.OutVol[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			
			for(int i = 0; i < CHANNEL_CFG; i++)
			{
				Tier2.chRecord[i].Vout = DcdcParam.OutVol[CurtCh-1];
				//Tier2.debug.inq1[i].VmodDebug = DcdcParam.OutVol[CurtCh-1];
			}			
			break;
		}
		case RECORD_BUS_VOL:
		{
			//����BUS��ѹ
			memcpy(&DcdcParam.BusVol[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			
			for(int i = 0; i < CHANNEL_CFG; i++)
			{
				Tier2.debug.inq2.Vbus1Debug = DcdcParam.BusVol[CurtCh-1];
				Tier2.debug.inq2.Vbus2Debug = DcdcParam.BusVol[CurtCh-1];
			}			
			break;
		}
		case RECORD_IL1_CURT:
		{
			//����IL1��е���
			memcpy(&DcdcParam.IL1Curt[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case RECORD_IL2_CURT:
		{
			//����IL2��е���
			memcpy(&DcdcParam.IL2Curt[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case RECORD_TEMP_VAL:
		{
			//�����¶�ֵ
			memcpy(&DcdcParam.TempVal, &DcdcRxFrame->Data[0], 4);
			break;
		}
		case RECORD_DUTY_CYCLE1:
		{
			//����ռ�ձ�1
			memcpy(&DcdcParam.DutyCycle1[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case RECORD_DUTY_CYCLE2:
		{
			//����ռ�ձ�2
			memcpy(&DcdcParam.DutyCycle2[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**  �� �� �� : DcdcRunStaSetRep()
**  ����˵�� : DCDC���Ʋ�����ѯ�ظ���Ӧ����
**  ��    �� : *DcdcRxFrame - ��������֡
**  �� �� ֵ : ��
**********************************************************************************************************/
static void DcdcCtrlParamInqRep(DCDC_FrameType *DcdcRxFrame)
{
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	
	//ͨ����Ϊ��Чֵ
	if(0x00 == CurtCh)
	{
		return;
	}
	
	switch(DcdcRxFrame->SubCmd)
	{
		case CTRL_RUN_MODE:
		{
			//��������ģʽ
			memcpy(&DcdcParam.RunMode[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case CTRL_RATE_VOL:
		{
			//�������ѹ
			memcpy(&DcdcParam.RatedVol[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case CTRL_RATE_CURT:
		{
			//���������
			memcpy(&DcdcParam.RatedCurt[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**  �� �� �� : DcdcClibParamInqRep()
**  ����˵�� : DCDCУ׼KBϵ����ѯ�ظ���Ӧ����
**  ��    �� : *DcdcRxFrame - ��������֡
**  �� �� ֵ : ��
**********************************************************************************************************/
static void DcdcClibParamInqRep(DCDC_FrameType *DcdcRxFrame)
{
	//��ȡУ׼��һ�������ĵ�ַ�����θ�ֵ
	float *ClibPara = (float*)&DcdcParam.SenseK[0];
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	uint8_t ClibParaIndex = 0x00;
	
	//ͨ����Ϊ��Чֵ
	if(0x00 == CurtCh)
	{
		return;
	}
	
	switch(DcdcRxFrame->SubCmd)
	{
		case CLIB_SENSE_VOL_K:
		case CLIB_SENSE_VOL_B:
		case CLIB_PORT_VOL_K:
		case CLIB_PORT_VOL_B:
		case CLIB_CHARG_CURT_K:
		case CLIB_CHARG_CURT_B:
		case CLIB_DISCHARG_CURT_K:
		case CLIB_DISCHARG_CURT_B:
		case CLIB_PROBE_VOL_K:
		case CLIB_PROBE_VOL_B:
		{
			ClibParaIndex = (DcdcRxFrame->SubCmd-1)*CHANNEL_NUM + (CurtCh-1);  //��ͬ���Ͳ�ͬͨ����KBֵ
			memcpy(&ClibPara[ClibParaIndex], DcdcRxFrame->Data, 4);
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**  �� �� �� : DcdcAdcDataReadRep()
**  ����˵�� : DCDC����AD�ۼ�ֵ��ѯ�ظ���Ӧ����
**  ��    �� : *DcdcRxFrame - ��������֡
**  �� �� ֵ : ��
**********************************************************************************************************/
static void DcdcAdcDataReadRep(DCDC_FrameType *DcdcRxFrame)
{
	//��ȡAD�ۼ�ֵ��һ�������ĵ�ַ�����θ�ֵ
	uint16_t *AdcVal = (uint16_t*)&DcdcParam.BatVolAdc;
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	uint8_t ClibParaIndex = 0x00;
	
	//ͨ����Ϊ��Чֵ
	if(0x00 == CurtCh)
	{
		return;
	}
	
	switch(DcdcRxFrame->SubCmd)
	{
		case CLIB_BAT_ADC_VAL:
		case CLIB_PORT_ADC_VAL:
		case CLIB_CURT_ADC_VAL:
		case CLIB_TEMP_ADC_VAL:
		case CLIB_PROBE_ADC_VAL:
		{
			ClibParaIndex = (DcdcRxFrame->SubCmd-1)*CHANNEL_NUM + (CurtCh-1); //��ͬ���Ͳ�ͬͨ����ADֵ
			memcpy(&AdcVal[ClibParaIndex], DcdcRxFrame->Data, 2);
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**  �� �� �� : DcdcSystemInqRep()
**  ����˵�� : DCDCϵͳ������ѯ�ظ���Ӧ����
**  ��    �� : *DcdcRxFrame - ��������֡
**  �� �� ֵ : ��
**********************************************************************************************************/
static void DcdcSystemInqRep(DCDC_FrameType *DcdcRxFrame)
{
	switch(DcdcRxFrame->SubCmd)
	{
		case SYSTEM_BASE_INF:
		{
			//��������汾��
			memcpy(&DcdcParam.VerNum[0], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case SYSTEM_ALARM_INF:
		{
			//memcpy
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**  �� �� �� : IsFrameValid()
**  ����˵�� : ����DCDC������ЧУ�麯��
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
static DCDC_FrameValidType IsFrameValid(void)
{
	DCDC_FrameValidType FrameSta = FrameOk;
	
	//���������������
	if((INVLID_SUB_CMD==DcdcRecvBuffer.MainCmd) || (INVLID_SUB_CMD==DcdcRecvBuffer.SubCmd))
	{
		return (FrameSta=CmdErr);
	}
	
	//CRC����
	if(CheckSum(&DcdcRecvBuffer.ChFlag, 0x07)!=DcdcRecvBuffer.Crc)
	{
		return (FrameSta=CrcErr);
	}
	
	//����֡�޴����򽫽��ջ��������ݿ�����������Ļ�����
	memcpy(&DcdcRecvFrame.ChFlag, &DcdcRecvBuffer.ChFlag, sizeof(DCDC_FrameType)/sizeof(uint8_t));
	
	return FrameSta;
}

/**********************************************************************************************************
**  �� �� �� : DcdcRecvHandle()
**  ����˵�� : DCDC���ݴ�����
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void DcdcRecvHandle(void)
{
	uint8_t FuncIndex = 0;
	
	//����֡��Ч
	if((FrameOk != IsFrameValid()) && (DcdcRecvFrame.MainCmd <= 0x81))
	{
		return;
	}
	
	//��ȡ�����±꣬��ִ�ж�Ӧ����
	FuncIndex = DcdcRecvFrame.MainCmd - 0x81;
	DcdcRepHandle[FuncIndex](&DcdcRecvFrame);
}

/**********************************************************************************************************
**  �� �� �� : AppTaskDcdc()
**  ����˵�� : DCDCͨѶ������������
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void AppTaskDcdc(void *argument)
{
	osStatus_t OsSemRt;
	
    while(1)
    {	 
		//��ȡ�ź�����״̬
		OsSemRt = osSemaphoreAcquire(SemaDcdcCan, osWaitForever);
		
		if(osOK == OsSemRt)
		{
			//����DCDC CAN���յ���Ϣ
			DcdcRecvHandle();
		}
		else
		{
			//do nothing;
		}
		
        osDelay(1);		
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
