/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Debug.c
** Author      : Yi Li Hua
** Date        : 2022-08-01
** Description : 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Task_Debug.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : void SwitchCtr(uint8_t CH)
**  ����˵�� : �л����ؿ��ƺ���
**  ��    �� : CH: �л�ͨ�����
**			   CurrOK: ���ʰ巵�����ذ��ź�
**  �� �� ֵ : 
**********************************************************************************************************/
void SwitchCtr(void)
{
	uint8_t i = 0;
	uint8_t Currr_OK;
	Currr_OK =  ~CurrOK[0];
	for (i = 0; i < 8; i++)                      
	{
		if((st_DebugParameter.CHCtrEN & 0x01) == 0x01)
		{
			/* ǿ����·�����ر�  */
			if ((st_DebugParameter.MainRoadCtr & 0x01) == 0x01)
			{
				HAL_GPIO_WritePin(SSwA_N_PortPin[2].Port, SSwA_N_PortPin[2].Pin, GPIO_PIN_SET);  //����·����
				HAL_GPIO_WritePin(SSwA_P_PortPin[2].Port, SSwA_P_PortPin[2].Pin, GPIO_PIN_SET);  //����·����
				
			}
			else if((st_DebugParameter.MainRoadCtr & 0x01) == 0x00)
			{
				HAL_GPIO_WritePin(SSwA_N_PortPin[i].Port, SSwA_N_PortPin[i].Pin, GPIO_PIN_RESET);  //�ر���·����
				HAL_GPIO_WritePin(SSwA_P_PortPin[i].Port, SSwA_P_PortPin[i].Pin, GPIO_PIN_RESET);  //�ر���·����
			}

			/* ǿ����·�����ر� */
			if ((st_DebugParameter.ByPassCtr & 0x01) == 0x01) 
			{
				HAL_GPIO_WritePin(CTR_ON_PortPin[i].Port, CTR_ON_PortPin[i].Pin, GPIO_PIN_RESET); //����·����
				bsp_DelayMS(1);
				HAL_GPIO_WritePin(CTR_ON_PortPin[i].Port, CTR_ON_PortPin[i].Pin, GPIO_PIN_SET);
			}
			else if ((st_DebugParameter.ByPassCtr & 0x01) == 0x00) 
			{
				;
			}
		}
		
		if((Currr_OK & 0x01) == 0x01)	//��·����յ�����·�ź�
		{
			HAL_GPIO_WritePin(SSwA_N_PortPin[i].Port, SSwA_N_PortPin[i].Pin, GPIO_PIN_RESET);   
			HAL_GPIO_WritePin(SSwA_P_PortPin[i].Port, SSwA_P_PortPin[i].Pin, GPIO_PIN_RESET);     
		}
		
		Currr_OK = Currr_OK >> 1;
		st_DebugParameter.CHCtrEN = st_DebugParameter.CHCtrEN >> 1;
	}
	st_DebugParameter.ByPassCtr = 0;
	st_DebugParameter.MainRoadCtr = 0;
}

/**********************************************************************************************************
**  �� �� �� : VoltageCalibration()
**  ����˵�� : Sense��ѹУ׼����
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void VoltageCalibration(void)
{
	uint8_t i;
	float Vcm[8] = {0, 0, 0, 0, 0, 0, 0, 0};  //Vsense��ģ��ѹ
	for(i = 0; i < 8; i++)
	{
		Tier2.debug.inq1[i].VsenseDebug = (float)(ADC2_DATA_AVG[i] * Tier2.calibrate[i].VsenseSetK_C) / 0x7FFFFF +  Tier2.calibrate[i].VsenseSetB_C;
        Tier2.debug.inq1[i].VtzDebug = (float)(ADC1_DATA_AVG[i] * Tier2.calibrate[i].VmodSetK_C) / 0x7FFFFF + Tier2.calibrate[i].VmodSetB_C;

		/* ��ģ��ѹ���� */
		if(i == 0)
		{
			Vcm[0] = 0;
		}
		else
		{    
			Vcm[i] = Vcm[i  - 1] + Tier2.debug.inq1[i - 1].VsenseDebug;
		}
		Tier2.debug.inq1[i].TmodDebug = Tier2.debug.inq1[i].VsenseDebug + (Vcm[i] * Tier2.calibrate[i].IhSetK_C + Tier2.calibrate[i].IhSetB_C);
	}
	if(UpDataFlag == 1)  //У׼���ݸ��� д��FRAM
	{
		MB85Ctrl_Write(0x00, &Tier2, sizeof(Tier2));
	}
}

/**********************************************************************************************************
**  �� �� �� : AppTaskDebug()
**  ����˵�� : ���������ֶ��л�����
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void AppTaskDebug(void *argument)
{
	while(1)
	{
		SwitchCtr();
		VoltageCalibration();
		osDelay(DEBUG_TASK_PERIOD);
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
