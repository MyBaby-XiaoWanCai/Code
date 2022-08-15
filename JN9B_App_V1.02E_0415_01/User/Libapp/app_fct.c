/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_fct.c
** Author      : 
** Date        : 2022-02-21
** Description : app_fct source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "global.h"
#include "bsp.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t FCT_ReadBuf[64] = {0};
uint8_t FCT_WriteBuf[64] = {0};
uint8_t FCT_ResClear[64] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
														0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
extern PcaDataType Pca9555_Data;
extern TestInformation IOTestInfo;	//���IO��Ϣ�洢
extern Vers_Mana vers_mana;			//���ذ��ȡ�汾��Ϣ�洢
uint8_t FCTEndFlg = 0x01;
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCT()
**	����˵�� : ����FCT���ܺ���-������ô˹���ʱ,�����"main.h"�д򿪶�Ӧ�궨��
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCT()
{
	static uint8_t StepTemp = FCTSTEP_START;
	uint8_t tmp = 0;
	
while(StepTemp != FCT_END_NOP)
{
	switch(StepTemp)
	{
		case FCTSTEP_START:	//fct step 0
		{
			tmp = WholeMachine_FCTSatart(); //����׼������
			
			if(tmp == 0)
			{
				StepTemp = FCT_REGINSIGNAL;
			}
			else
			{
				StepTemp = FCT_END_NOP;
			}
			break;
		}
		case FCT_REGINSIGNAL: //������1
		{
			memset(FCT_WriteBuf,0x00,64);
			ee_WriteBytesWithCheck(&FCT_ResClear[0], FCT_EE_ADDR_A, 57);	//�����һ��FCT���
			osDelay(200);
			tmp = WholeMachine_FCTReginSignal();	// - ��λ�źż��
			StepTemp = FCT_VERSION;		//FCT_VERSION
			break;
		}
		case FCT_VERSION: //������2
		{
			tmp = WholeMachine_FCTVersion();	//�汾���
			StepTemp = FCT_LAMPPLATE;
			break;
		}
		case FCT_LAMPPLATE: //������3
		{
			tmp = WholeMachine_FCTLampPlate();	//������3 - �ư����
			StepTemp = FCT_FAN;
			break;
		}
		case FCT_FAN: //������4
		{
			tmp = WholeMachine_FCTFan(); //���Ȳ���
			StepTemp = FCT_TMP;
			break;
		}
		case FCT_TMP: //������5
		{
			tmp = WholeMachine_FCTTmp75(); //�¶Ȳ���
			StepTemp = FCT_CHARGEDISC;	//FCT_CHARGEDISC
			break;
		}
		case FCT_CHARGEDISC: //������6
		{
			tmp = WholeMachine_FCTChargeDisc(); //��ŵ����
			StepTemp = FCT_DISCCHARGE;
			break;
		}
		case FCT_DISCCHARGE: //������7
		{
			tmp = WholeMachine_FCTDischCharge(); //�ų�����
			StepTemp = FCT_VSENSEFAST; //FCT_VSENSEFAST
			break;
		}
		case FCT_VSENSEFAST:	//������8
		{
			tmp = WholeMachine_FCTVsenseRapidLoop(); //Vsense���ٻ�����
			StepTemp = FCT_VSENSESLOW; //FCT_VSENSESLOW
			break;
		}
		case FCT_VSENSESLOW:	//������9
		{
			tmp = WholeMachine_FCTVsenseSlowLoop(); //Vsense���ٻ�����
			StepTemp = FCTSTEP_END;
			break;
		}
		case FCTSTEP_END:	//FCT����
		{
			WholeMachine_FCTEnd();
			StepTemp = FCT_END_NOP;
			break;
		}
		case FCT_END_NOP:
		{
			break;
		}
		default:
			break;
	}
}
	return StepTemp;
}

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTSatart()
**	����˵�� : ����FCT start����
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTSatart()
{
	uint8_t err_tmp = 0;

	/* ��ѯeeprom��ַA-1,���Ϊ0������Զ��ϻ�ģʽ,�����0��ʼ�Լ���� */
	ee_ReadBytes(&FCT_ReadBuf[0], FCT_EE_ADDR_A_1, 1);
	
	if(FCT_ReadBuf[0] == 0x00)
	{
		FCTEndFlg = 0x01;
		
		err_tmp = 1;
		Ageing();		//�����ϻ�ģʽ
	}
//	else
//	{
//		SelfChecking();	//�����Լ�ģʽ
//	}

	return err_tmp;
}

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTReginSignal()
**	����˵�� : ����FCT���Բ��� - ��λ�źż��
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTReginSignal()
{
	uint8_t err_tmp = 0;
	uint8_t ReginSignalCnt = 0;
	
	/* ��ȡ���ư弰���ʰ���λ�ź� */
	MainPowerSignalRead();	//IOTestInfo.MainPowerSignal[reign_i] ��λ�źŴ洢
	
	for(ReginSignalCnt = 0;ReginSignalCnt < 9; ReginSignalCnt++)
	{
		if(ReginSignalCnt < (1 + CHANNEL_USE))
		{
			if(IOTestInfo.MainPowerSignal[ReginSignalCnt] == 0x01)	//����λ - д1
			{
				err_tmp = 1;
				if(ReginSignalCnt == 0)	//����
				{
					FCT_WriteBuf[1] = 0x01;

				}
				else					//���ʰ岻��λ
				{
					FCT_WriteBuf[ReginSignalCnt + 1] = 0x01;

				}
			}
			else	//��λ - д0 
			{
				if(ReginSignalCnt == 0)	//������λ
				{
					FCT_WriteBuf[1] = 0x00;

				}
				else					//���ʰ���λ buf: 2 - 9
				{
					FCT_WriteBuf[ReginSignalCnt + 1] = 0x00;

				}
			}
		}
		else
		{
			FCT_WriteBuf[ReginSignalCnt + 1] = 0x00;

		}
	}
	
	return err_tmp;
}

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTVersion()
**	����˵�� : ����FCT���Բ��� - �汾���
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTVersion()
{
	uint8_t err_tmp = 0;
	uint8_t VerReadCnt = 0;
	
	/* ���ư�汾���ж� */
	if((vers_mana.Bom_Vers == BOM_VER) && (vers_mana.Id_Vers == ID_VER) && (vers_mana.Pcb_Vers == PCB_VER))	
	{
		FCT_WriteBuf[10] = 0x00;	//�汾����ȷ,��eeprom��ַA+9д0
	}
	else
	{
		err_tmp = 1;
//		FCT_WriteBuf[10] = 0x01;	//�汾�Ŵ���,��eeprom��ַA+9д1
		FCT_WriteBuf[10] = 0x00;
	}
	FCT_WriteBuf[10] = 0x00;

	
	/* ���ʰ�汾�ŵĶ�ȡ���ж� */
	for(VerReadCnt = 1; VerReadCnt <= 8; VerReadCnt++)
	{
		if(VerReadCnt <= CHANNEL_USE)
		{		
			/* ���ʰ�汾���ж� */
			if(vers_mana.Pow_Ver[VerReadCnt - 1] == PB_VER)	//���ʰ�汾���ж�
			{
				FCT_WriteBuf[10 + VerReadCnt] = 0x00;	//�汾����ȷ,��eeprom��ַ(A+9+VerReadCnt)д0
			}
			else
			{
				err_tmp = 1;
//				FCT_WriteBuf[10 + VerReadCnt] = 0x01;	//�汾�Ŵ���,��eeprom��ַ(A+9+VerReadCnt)д1
				FCT_WriteBuf[10 + VerReadCnt] = 0x00;
			}

		}
		else
		{
			FCT_WriteBuf[10 + VerReadCnt] = 0x00;		//�汾����ȷ,��eeprom��ַ(A+9+VerReadCnt)д0

		}
	}
	
	return err_tmp;
}

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTLampPlate()
**	����˵�� : ����FCT���Բ��� - �ư����
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTLampPlate()
{
	uint8_t err_tmp = 0;
	uint8_t xLedWriteDataRed[2]   = {0xFF,0x00}; 
	uint8_t xLedWriteDataGreen[2] = {0x00,0xFF}; 
	uint8_t xLedWriteDataOFF[2]   = {0xFF,0xFF};
	uint8_t LedWriteRes_1 = 2, LedWriteRes_2 = 2, LedWriteRes_3 = 2, LedWriteRes_4 = 2;	
	
//	/* �ư�汾�ŵĶ�ȡ���ж� */
//	if(vers_mana.LED_Ver == LED_VER)
//	{
//		//�汾��ȷ ��eeprom��ַA+18д0
//		FCT_WriteBuf[19] = 0x00;
//	}
//	else //��eeprom��ַA+18д0
//	{
//		FCT_WriteBuf[19] = 0x01;
//		
//		err_tmp = 1;
//	}
	
	FCT_WriteBuf[19] = 0x00;

	
	/* �ư��Լ��߼�:�����1S��1S,�̵���1S��1Sѭ��1�� */
	LedWriteRes_1 = Pca9555_RegWrite(I2C_LED, PCA9555_LED_ADDR, xLedWriteDataRed);
	osDelay(1000);
	LedWriteRes_2 = Pca9555_RegWrite(I2C_LED, PCA9555_LED_ADDR, xLedWriteDataOFF);
	osDelay(1000);
	LedWriteRes_3 = Pca9555_RegWrite(I2C_LED, PCA9555_LED_ADDR, xLedWriteDataGreen);
	osDelay(1000);
	LedWriteRes_4 = Pca9555_RegWrite(I2C_LED, PCA9555_LED_ADDR, xLedWriteDataOFF);
	osDelay(1000);
	
	if((LedWriteRes_1 || LedWriteRes_2 || LedWriteRes_3 || LedWriteRes_4) == 0)
	{
		FCT_WriteBuf[20] = 0x00;	//�ư�����ȷ,��eeprom��ַ(A+19)д0
	}
	else
	{
		err_tmp = 1;
		FCT_WriteBuf[20] = 0x01;	//�ư������,��eeprom��ַ(A+19)д1
	}

	
	return err_tmp;
}

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTFan()
**	����˵�� : ����FCT���Բ��� - ���ȿ���
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTFan()
{
	uint8_t err_tmp = 0;
	uint8_t FanTestCnt = 0, FanCnt1 = 0, FanCnt2 = 0;
	uint8_t FanDiv = 1;
	
	/* 30%PWM���� */
	FanPwmSet(0.3);
	/* IO������״̬��ȡ */
	FanFaultStateRead();
	
	if(CHANNEL_USE == 8)
	{
		FanDiv = 1;
	}
	else if(CHANNEL_USE == 4)
	{
		FanDiv = 2;
	}
	else if(CHANNEL_USE == 2)
	{
		FanDiv = 3;
	}
	
	for(FanTestCnt = 0; FanTestCnt < (FAN_FaultPinNum / FanDiv); FanTestCnt++)
	{
		if(IOTestInfo.FanFault[FanTestCnt] == 0x00)
		{
			FanCnt1++;
		}
	}

	/* 100%PWM���� */
	FanPwmSet(1);
	/* IO������״̬��ȡ */
	FanFaultStateRead();
	for(FanTestCnt = 0; FanTestCnt < (FAN_FaultPinNum / FanDiv); FanTestCnt++)
	{
		if(IOTestInfo.FanFault[FanTestCnt] == 0x00)
		{
			FanCnt2++;
		}
	}
	
	if((FanCnt1 == (FAN_FaultPinNum / FanDiv)) && (FanCnt2 == (FAN_FaultPinNum / FanDiv)))
	{
		FCT_WriteBuf[21] = 0x00;	//���ȿ�������,��eeprom��ַA+20д0
	}
	else
	{
		err_tmp = 1;
		FCT_WriteBuf[21] = 0x01;	//���ȿ����쳣,��eeprom��ַA+20д1
	}

	
	return err_tmp;
}

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTChargeDisc()
**	����˵�� : ����FCT���Բ��� - �¶ȼ��
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTTmp75()
{
	uint8_t i = 0,err_tmp = 0;;
	float Tmp_Arr[9] = {0};
	
	Tmp_Arr[0] = Tier2.sampleCal.Tamb;
	
	for(i = 1;i < (CHANNEL_USE + 1);i++)
	{
		Tmp_Arr[i] = Tier2.sampleCal.Tmod[i -1];
	}
	
	/*	��ȡ���ư��¶Ⱥ͹���ģ���¶ȣ��ж������¶ȵļ���ֵ������10��		*/

	float ArrMax = Tmp_Arr[0],ArrMin = Tmp_Arr[0];
	
	for(i = 0;i < (CHANNEL_USE + 1);i++)
	{
		if(Tmp_Arr[i] > ArrMax)
		{
			ArrMax = Tmp_Arr[i];	//ȡ�¶����ֵ
		}
	}
	
	for(i = 0;i < (CHANNEL_USE + 1);i++)
	{
		if(Tmp_Arr[i] < ArrMin)
		{
			ArrMin = Tmp_Arr[i];	//ȡ�¶���Сֵ
		}
	}
	
	if(ArrMax - ArrMin <= 10)	//��eeprom��ַA+21д0
	{
		FCT_WriteBuf[22] = 0x00;
	}
	else	//��eeprom��ַA+21д1
	{
		FCT_WriteBuf[22] = 0x01;
		
		err_tmp = 1;
	}

	
	return err_tmp;
}


/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTChargeDisc()
**	����˵�� : ����FCT���Բ��� - �ų����� 2/4/6/8 ��� 1/3/5/7 �ŵ�
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTChargeDisc()
{
	uint8_t err_tmp = 0;
	uint8_t i =0;
	
	for( i = 0;i < CHANNEL_USE;i++)
	{
		Protect_Flag_Internal[i] = 0;
		IP_Alarm_Flag[i] = 0;
	}
	for(uint8_t CHx_Temp = 1; CHx_Temp <= CHANNEL_USE; CHx_Temp += 2)
	{
		uint8_t tmp = WholeMachine_FCTChaDisFunc_1(CHx_Temp, CHx_Temp + 1);
		
		if(tmp != 0)
		{
			err_tmp = tmp;
		}		
		
		osDelay(100);
	}
	
	return err_tmp;
}

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTDischCharge()
**	����˵�� : ����FCT���Բ��� - �ų����� 2/4/6/8 ��� 1/3/5/7 �ŵ�
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTDischCharge()
{
	uint8_t err_tmp = 0;
	uint8_t i =0;
	
	for( i = 0;i < CHANNEL_USE;i++)
	{
		Protect_Flag_Internal[i] = 0;
		IP_Alarm_Flag[i] = 0;
	}
	for(uint8_t CHx_Temp = 2; CHx_Temp <= CHANNEL_USE; CHx_Temp += 2)
	{
		uint8_t tmp = WholeMachine_FCTChaDisFunc_1(CHx_Temp, CHx_Temp - 1);
		
		if(tmp != 0)
		{
			err_tmp = tmp;
		}
		
		osDelay(100);
	}
	
	return err_tmp;
}

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTVsenseRapid()
**	����˵�� : ����FCT���Բ��� - ��������ͨ��ΪVsense���ٻ�����������ģ��1CV4V2A��������ʱ2S
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTVsenseRapidLoop()
{
	uint8_t err_tmp = 0;
	uint8_t WriteBaseAddr = 38;	//���Խ��д�����ַƫ��
	uint8_t Arlam_1 = 0x00,Arlam_2 = 0x00,Arlam_3 = 0x00,Arlam_4 = 0x00,Arlam_5 = 0x00,Arlam_6 = 0x00,Arlam_7 = 0x00;
	uint8_t i;
	//����8��ģ��ΪVsense���ٻ� -> LOOP �ø�,MLoop�����õ�
	for( i = 0;i < CHANNEL_USE;i++)
	{
		Protect_Flag_Internal[i] = 0;
		IP_Alarm_Flag[i] = 0;
		Disable_Mloop(i); //�з���
		osDelay(10);
	}

	for( i = 0; i < CHANNEL_USE; i += 2)
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			Chg_Dis_On_Cfg((1<<i), BUCK_MODE, V_SENSE, 4.0, 2.0);
		}
	}
	osDelay(2000);	
	
	/* ����ģ��1 CV4V2A��������ʱ2S���ж϶˿ڵ�ѹ��sense��ѹ��̽���ѹ�Ƿ���4V����0.5V��Χ�ڣ������Ƿ���50mA�ڣ�������ڣ���ȡ�Ƿ���Ӳ�����ϣ�������Ӳ������ */
	for( i = 0; i < CHANNEL_USE; i += 2)	//(CHANNEL_USE - 1) �������ã���Ϊ7ģ���ǻ���
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			if( (3.5f <= Tier2.sampleCal.Vmod[i] && Tier2.sampleCal.Vmod[i] <= 4.5f) 
			 && (3.5f <= Tier2.sampleCal.Vsense[i] && Tier2.sampleCal.Vsense[i] <= 4.5f)
			 && (3.5f <= Tier2.sampleCal.Vtz[i] && Tier2.sampleCal.Vtz[i] <= 4.5f) 
			 && (Tier2.sampleCal.Iout[i] <= 1.0f) ) //A+38
			{
				FCT_WriteBuf[WriteBaseAddr + 1 + i] = 0x00;
				
				err_tmp = 0x00;
			}
			else
			{
				if(Tier2.sampleCal.Iout[i] > 1.0f)	//�������ϸ�
				{
					Arlam_1 = 0x01;
				}
				if(3.5f > Tier2.sampleCal.Vmod[i] || Tier2.sampleCal.Vmod[i] > 4.5f)	//�˿ڵ�ѹ���ϸ�
				{
					Arlam_2 = 0x02;
				}
				
				if(IP_Alarm_Flag[i] & IntelProtect_OCP)	//Ӳ��OCP bit2 = 1
				{
					Arlam_3 = 0x04;
				}
				else if(IP_Alarm_Flag[i] & IntelProtect_OVP) //Ӳ��OVP bit3 = 1 
				{
					Arlam_4 = 0x08;
				}
				else if(IP_Alarm_Flag[i] & IntelProtect_OTP)	//Ӳ��OTP bit4 = 1
				{
					Arlam_5 = 0x10;
				}
				
				if(3.5f > Tier2.sampleCal.Vsense[i] || Tier2.sampleCal.Vsense[i] > 4.5f)	//Vsense��ѹ���ϸ�
				{
					Arlam_6 = 0x20;
				}
				if(3.5f > Tier2.sampleCal.Vtz[i] || Tier2.sampleCal.Vtz[i] > 4.5f)	//Vtz��ѹ���ϸ�
				{
					Arlam_7 = 0x40;
				}
				
				err_tmp = 0x01;
				FCT_WriteBuf[WriteBaseAddr + 1 + i] = Arlam_1 + Arlam_2 + Arlam_3 + Arlam_4 + Arlam_5 + Arlam_6 + Arlam_7;
			}

		}
	}
	for( i = 0; i < CHANNEL_USE; i++)
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			Chg_Dis_Off_Cfg(1<<i);
		}
		Protect_Flag_Internal[i] = 0;
		IP_Alarm_Flag[i] = 0;
	}	
	osDelay(100);

	for( i = 1; i < CHANNEL_USE; i += 2)
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			Chg_Dis_On_Cfg((1<<i), BUCK_MODE, V_SENSE, 4.0, 2.0);
		}
	}
	osDelay(2000);		
	
	/* ����ģ��1 CV4V2A��������ʱ2S���ж϶˿ڵ�ѹ��sense��ѹ��̽���ѹ�Ƿ���4V����0.5V��Χ�ڣ������Ƿ���50mA�ڣ�������ڣ���ȡ�Ƿ���Ӳ�����ϣ�������Ӳ������ */
	for( i = 1; i < CHANNEL_USE; i += 2)	//(CHANNEL_USE - 1) �������ã���Ϊ7ģ���ǻ���
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			if( (3.5f <= Tier2.sampleCal.Vmod[i] && Tier2.sampleCal.Vmod[i] <= 4.5f) 
			 && (3.5f <= Tier2.sampleCal.Vsense[i] && Tier2.sampleCal.Vsense[i] <= 4.5f)
			 && (3.5f <= Tier2.sampleCal.Vtz[i] && Tier2.sampleCal.Vtz[i] <= 4.5f) 
			 && (Tier2.sampleCal.Iout[i] <= 1.0f) ) //A+38
			{
				FCT_WriteBuf[WriteBaseAddr + 1 + i] = 0x00;
				
				err_tmp = 0x00;
			}
			else
			{
				if(Tier2.sampleCal.Iout[i] > 1.0f)	//�������ϸ�
				{
					Arlam_1 = 0x01;
				}
				if(3.5f > Tier2.sampleCal.Vmod[i] || Tier2.sampleCal.Vmod[i] > 4.5f)	//�˿ڵ�ѹ���ϸ�
				{
					Arlam_2 = 0x02;
				}
				
				if(IP_Alarm_Flag[i] & IntelProtect_OCP)	//Ӳ��OCP bit2 = 1
				{
					Arlam_3 = 0x04;
				}
				else if(IP_Alarm_Flag[i] & IntelProtect_OVP) //Ӳ��OVP bit3 = 1 
				{
					Arlam_4 = 0x08;
				}
				else if(IP_Alarm_Flag[i] & IntelProtect_OTP)	//Ӳ��OTP bit4 = 1
				{
					Arlam_5 = 0x10;
				}
				
				if(3.5f > Tier2.sampleCal.Vsense[i] || Tier2.sampleCal.Vsense[i] > 4.5f)	//Vsense��ѹ���ϸ�
				{
					Arlam_6 = 0x20;
				}
				if(3.5f > Tier2.sampleCal.Vtz[i] || Tier2.sampleCal.Vtz[i] > 4.5f)	//Vtz��ѹ���ϸ�
				{
					Arlam_7 = 0x40;
				}
				
				err_tmp = 0x01;
				FCT_WriteBuf[WriteBaseAddr + 1 + i] = Arlam_1 + Arlam_2 + Arlam_3 + Arlam_4 + Arlam_5 + Arlam_6 + Arlam_7;
			}

		}
	}
	for( i = 1; i < CHANNEL_USE; i++)
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			Chg_Dis_Off_Cfg(1<<i);
		}
	}	
	osDelay(100);	
	
	return err_tmp;
}

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTVsenseSlow()
**	����˵�� : ����FCT���Բ��� - ��������ͨ��ΪVsense���ٻ�����������ģ��1CV4V2A��������ʱ2S
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTVsenseSlowLoop()
{
	uint8_t WriteBaseAddr = 46;
	uint8_t err_tmp = 0;
	uint8_t i;
	
	//����8��ģ��ΪVsense���ٻ� -> LOOP �õ�
	for( i = 0;i < CHANNEL_USE;i++)
	{
		Protect_Flag_Internal[i] = 0;
		IP_Alarm_Flag[i] = 0;
		Enable_Mloop(i);//�з���
		osDelay(10);
	}
	
	for( i = 0; i < CHANNEL_USE; i += 2)
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			Chg_Dis_On_Cfg((1<<i), BUCK_MODE, V_SENSE, 4.0, 2.0);
		}
	}
	osDelay(3000);	
		
	//����ģ�� CV4V2A��������ʱ2S���ж϶˿ڵ�ѹ��sense��ѹ��̽���ѹ�Ƿ���4V����0.5V��Χ�ڣ�������ڣ���ȡ�Ƿ���Ӳ�����ϣ��������Ӳ��OVP�澯����OK������NG
	for( i = 0; i < CHANNEL_USE; i +=2 )
	{	
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			if( (3.5f <= Tier2.sampleCal.Vmod[i] && Tier2.sampleCal.Vmod[i] <= 4.5f) 
			 && (3.5f <= Tier2.sampleCal.Vsense[i] && Tier2.sampleCal.Vsense[i] <= 4.5f)
			 && (3.5f <= Tier2.sampleCal.Vtz[i] && Tier2.sampleCal.Vtz[i] <= 4.5f) )
			{
				FCT_WriteBuf[WriteBaseAddr + 1 + i] = 0x01;
				err_tmp = 0x01;
			}
			else
			{
				if( (IP_Alarm_Flag[i] & IntelProtect_OVP) || (IP_Alarm_Flag[i] & IntelProtect_LOV) ) //Ӳ��OVP bit3 = 1
				{
					FCT_WriteBuf[WriteBaseAddr + 1 + i] = 0x00;
					err_tmp = 0x00;
				}
				else
				{
					FCT_WriteBuf[WriteBaseAddr + 1 + i] = 0x01;
					err_tmp = 0x01;
				}
			}

		}
	}
	for( i = 0; i < CHANNEL_USE; i++)
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			Chg_Dis_Off_Cfg(1<<i);
		}
	}	
	osDelay(100);
	
	for( i = 1; i < CHANNEL_USE; i += 2)
	{
		Protect_Flag_Internal[i] = 0;
		IP_Alarm_Flag[i] = 0;
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			Chg_Dis_On_Cfg((1<<i), BUCK_MODE, V_SENSE, 4.0, 2.0);
		}
	}
	osDelay(3000);	
		
	//����ģ�� CV4V2A��������ʱ2S���ж϶˿ڵ�ѹ��sense��ѹ��̽���ѹ�Ƿ���4V����0.5V��Χ�ڣ�������ڣ���ȡ�Ƿ���Ӳ�����ϣ��������Ӳ��OVP�澯����OK������NG
	for( i = 1; i < CHANNEL_USE; i +=2 )
	{	
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			if( (3.5f <= Tier2.sampleCal.Vmod[i] && Tier2.sampleCal.Vmod[i] <= 4.5f) 
			 && (3.5f <= Tier2.sampleCal.Vsense[i] && Tier2.sampleCal.Vsense[i] <= 4.5f)
			 && (3.5f <= Tier2.sampleCal.Vtz[i] && Tier2.sampleCal.Vtz[i] <= 4.5f) )
			{
				FCT_WriteBuf[WriteBaseAddr + 1 + i] = 0x01;
				err_tmp = 0x01;
			}
			else
			{
				if( (IP_Alarm_Flag[i] & IntelProtect_OVP) || (IP_Alarm_Flag[i] & IntelProtect_LOV) )	//Ӳ��OVP bit3 = 1
				{
					FCT_WriteBuf[WriteBaseAddr + 1 + i] = 0x00;
					err_tmp = 0x00;
				}
				else
				{
					FCT_WriteBuf[WriteBaseAddr + 1 + i] = 0x01;
					err_tmp = 0x01;				
				}
			}

		}
	}
	for( i = 1; i < CHANNEL_USE; i++)
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			Chg_Dis_Off_Cfg(1<<i);
		}
	}	
	osDelay(100);	
	return err_tmp;
}

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTEnd()
**	����˵�� : ����FCT���Բ��� - ����FCT
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTEnd()
{
	uint8_t Test_Cnt = 0, NG_Cnt = 0;
	static uint8_t FCT_Test_Num = 54;
	
	uint8_t LEDON_G[2] = {0xFF,0x00};
	uint8_t LEDON_R[2] = {0x00,0xFF};

	FCTEndFlg = 0x00;
	
	if(CHANNEL_USE == 0x08)
	{
		FCT_Test_Num = 0x36;
	}
	else if(CHANNEL_USE == 0x04)
	{
		FCT_Test_Num = 0x1E;
	}
	
	for(Test_Cnt = 0; Test_Cnt < FCT_Test_Num; Test_Cnt++)	//NG����
	{
		if(FCT_WriteBuf[Test_Cnt] == 0x01)
		{
			NG_Cnt +=1;
		}
	}
	

	FCT_WriteBuf[0] = NG_Cnt;

	int32_t ls, us;
	Dis_int();
	ls = osKernelLock(); 
	
	ee_WriteBytesWithCheck(&FCT_WriteBuf[0], FCT_EE_ADDR_A_1, 58);
	
	us = osKernelUnlock();
	osKernelRestoreLock(us);
	osKernelRestoreLock(ls);
	En_int();
	bsp_DelayMS(1);

	if(NG_Cnt == 0x00)	//FCT�������NG��
	{
		Pca9555_RegWrite(I2C_LED, PCA9555_LED_ADDR, LEDON_G);	//���̵�
	}
	else
	{
		Pca9555_RegWrite(I2C_LED, PCA9555_LED_ADDR, LEDON_R);	//�����
	}
	
	return 0;
}

/**********************************************************************************************************
**	�� �� �� : WholeMachine_FCTChaDisFunc()
**	����˵�� : ����FCT��ŵ���Թ��ܺ��� - ��ŵ���Լ������ж�
**			   1/3/5/7 ��� 2/4/6/8 �ŵ�;	2/4/6/8 ��� 1/3/5/7�ŵ�
**	��    �� : CHx1 - ���ͨ��   CHx2 - �ŵ�ͨ��
**	�� �� ֵ : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTChaDisFunc_1(uint8_t CHx1, uint8_t CHx2)
{
	uint8_t err_tmp = 0;
	uint8_t Arlam_1 = 0x00, Arlam_2 = 0x00, Arlam_3 = 0x00, Arlam_4 = 0x00, Arlam_5 = 0x00;
	uint8_t WriteBaseAddr = 0;

	uint8_t i = 0;
	for( i = 0;i < CHANNEL_USE;i++)
	{
		Protect_Flag_Internal[i] = 0;
		IP_Alarm_Flag[i] = 0;
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//��λ�ź��ж� 1-8 0λΪ���ذ���λ�źţ�0��λ��1����λ
		{
			Chg_Dis_Off_Cfg(1<<i);
		}
		osDelay(5);
	}
	
	if(CHx1%2 == 1)			//1/3/5/7 ���; 2/4/6/8 �ŵ�
	{
		WriteBaseAddr = 21;	//���Խ��д����ʼƫ�Ƶ�ַ
	}
	else if(CHx1%2 == 0)	//2/4/6/8 ���; 1/3/5/7 �ŵ�
	{
		WriteBaseAddr = 28;	//���Խ��д����ʼƫ�Ƶ�ַ
	}
	
	/* 1��2��30A����:����ģ��1CV4V40A,����ģ��2DCC1V30A,��ʱ1S,�ж϶˿ڵ�ѹ�Ƿ���4V����0.5V��Χ��,�����Ƿ���30A����2A��,�������,��ȡ�Ƿ���Ӳ������ */
	
	/* ��λ�ź��ж� 1-8 0λΪ���ذ���λ�ź� */
	if((IOTestInfo.MainPowerSignal[CHx1] || IOTestInfo.MainPowerSignal[CHx2]) != 0x00)
	{
		return 1;	//���ʰ岻��λ��ִ��
	}
	Chg_Dis_On_Cfg(1 << (CHx2 - 1), BOOST_MODE, V_MOD, DISC_VOLT1 , DISC_CUR1 );	//CHx2 1V30A�ŵ�
	osDelay(1000);
	Chg_Dis_On_Cfg(1 << (CHx1 - 1), BUCK_MODE , V_MOD, CHARG_VOLT1, CHARG_CUR1);	//CHx1 4V40A���
	osDelay(1500);
	
	if( ((CHARG_VOLT1 - 0.5) < Tier2.sampleCal.Vmod[CHx1 - 1]) && \
		(Tier2.sampleCal.Vmod[CHx1 - 1] < (CHARG_VOLT1 + 0.5)) && \
		((DISC_CUR1 - 2) < Tier2.sampleCal.Iout[CHx1 - 1])     && \
		(Tier2.sampleCal.Iout[CHx1 - 1] < (DISC_CUR1 + 2)) )
	{
		FCT_WriteBuf[WriteBaseAddr + 1 + CHx1] = 0x00;	//������ѹ�ڹ涨��Χ��,��eeprom��ַA+21д0
	}
	else
	{
		err_tmp = 2;
		
		//���ڹ涨��Χ�����趨λ��������
		if( ((DISC_CUR1 - 2) > Tier2.sampleCal.Iout[CHx1 - 1]) || \
			(Tier2.sampleCal.Iout[CHx1 - 1] > (DISC_CUR1 + 2)) )		//�������ϸ� bit0 = 1
		{
			Arlam_1 = 0x01;
		}
		if( ((CHARG_VOLT1 - 0.5) > Tier2.sampleCal.Vmod[CHx1 -1]) || \
			(Tier2.sampleCal.Vmod[CHx1 - 1] > (CHARG_VOLT1 + 0.5)) )	//��ѹ���ϸ� bit1 = 1
		{
			Arlam_2 = 0x02;
		}
		if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OCP)					//Ӳ��OCP bit2 = 1
		{
			Arlam_3 = 0x04;
		}
		else if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OVP)			//Ӳ��OVP bit3 = 1 
		{
			Arlam_4 = 0x08;
		}
		else if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OTP)			//Ӳ��OTP bit4 = 1
		{
			Arlam_5 = 0x10;
		}
		
		FCT_WriteBuf[WriteBaseAddr + 1 + CHx1] = Arlam_1 + Arlam_2 + Arlam_3 + Arlam_4 + Arlam_5;
	}

	
	/* 1��2��120A����:����ģ��1CV4V130A,����ģ��2DCC1V120A,��ʱ1S,�ж϶˿ڵ�ѹ�Ƿ���4V����0.5V��Χ��,�����Ƿ���120A����5A��,������ڶ�ȡ�Ƿ���Ӳ������ */
	Chg_Dis_On_Cfg(1 << (CHx1 - 1), BUCK_MODE , V_MOD, CHARG_VOLT1, CHARG_CUR2);	//CHx1 4V130A���
	osDelay(300);
	Chg_Dis_On_Cfg(1 << (CHx2 - 1), BOOST_MODE, V_MOD, DISC_VOLT1 , DISC_CUR2 );	//CHx2 1V120A�ŵ�
	osDelay(1500);
	
	if( ((CHARG_VOLT1 - 0.5) < Tier2.sampleCal.Vmod[CHx1 - 1]) && \
		(Tier2.sampleCal.Vmod[CHx1 - 1] < (CHARG_VOLT1 + 0.5)) && \
		((DISC_CUR2 - 5) < Tier2.sampleCal.Iout[CHx1 - 1]) && \
		(Tier2.sampleCal.Iout[CHx1 - 1] < (DISC_CUR2 + 5)) )	//������ѹ�ڹ涨��Χ��,��eeprom��ַA+22д0
	{
		FCT_WriteBuf[WriteBaseAddr + 2 + CHx1] = 0x00;
	}
	else
	{
		err_tmp = 3;
		
		//���ڹ涨��Χ�����趨λ��������
		if( ((DISC_CUR2 - 5) > Tier2.sampleCal.Iout[CHx1 - 1]) || \
			(Tier2.sampleCal.Iout[CHx1 - 1] > (DISC_CUR2 + 5)) )		//�������ϸ� bit0 = 1
		{
			Arlam_1 = 0x01;
		}
		if( ((CHARG_VOLT1 - 0.5) > Tier2.sampleCal.Vmod[CHx1 -1]) || \
			(Tier2.sampleCal.Vmod[CHx1 - 1] > (CHARG_VOLT1 + 0.5)) )	//��ѹ���ϸ� bit1 = 1
		{
			Arlam_2 = 0x02;
		}
		if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OCP)					//Ӳ��OCP bit2 = 1
		{
			Arlam_3 = 0x04;
		}
		else if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OVP)			//Ӳ��OVP bit3 = 1 
		{
			Arlam_4 = 0x08;
		}
		else if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OTP)			//Ӳ��OTP bit4 = 1
		{
			Arlam_5 = 0x10;
		}
		
		FCT_WriteBuf[CHx1 + 21 + 1] = Arlam_1 + Arlam_2 + Arlam_3 + Arlam_4 + Arlam_5;
	}

	
	//��ŵ�ر�
	Chg_Dis_Off_Cfg(1 << (CHx1 - 1));
	osDelay(100);
	Chg_Dis_Off_Cfg(1 << (CHx2 - 1));
	osDelay(200);
	
	uint8_t j = 0;
	for( j = 0;j < CHANNEL_USE;j++)
	{
		Protect_Flag_Internal[j] = 0;
		IP_Alarm_Flag[j] = 0;
	}	
	
	return err_tmp;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
