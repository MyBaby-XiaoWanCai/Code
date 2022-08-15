/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Led.c
** Author      : Cai Heng
** Date        : 2022-08-01
** Description : 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Task_Led.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
un_AddrAnalysis_Type    un_AddrAnalysis;
un_DcdcIdAnalysis_Type  un_DcdcIdAnalysis;
volatile uint32_t heart_beat_cnt = 0;

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void ReadAddr(void);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : AppTaskLed()
**	����˵�� : LED����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void AppTaskLed(void *argument)
{
    uint16_t TaskCnt = 0;
	while(1)
	{
		if(clear_flag == 1)
		{
			clear_flag = 0;	
            
			//����������澯��־λ
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                Protect_Flag_Global[j]   = 0;
                Protect_Flag_Internal[j] = 0;
                Protect_Flag_Workstep[j] = 0;
                workstep_flag_ErrWord[j] = 0;

                protect_flag_upcast[j] = 0;
            }

            AlarmClear();  //����ڲ��澯��ȫ�ָ澯	
			
			dcdc_err_flag = 0;

			//DCDC�澯���ָ��
			DcdcRunStaSet(DEBUG_CH, CLEAR_FAULT);
			CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
		}
        if(TaskCnt > 10)  //1S��ȡ���뿪�ص�ַ
        {
            ReadAddr();
            TaskCnt = 0;
        }
        else
        {
            TaskCnt++;
        }
		
		osDelay(LED_TASK_PERIOD);
	}
}
/**********************************************************************************************************
**  �� �� �� : ReadAddr()
**  ����˵�� : ��ȡ��·��ID�ż���CAN ID
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void ReadAddr(void)
{
	uint8_t Addr[2] = {0, 0};
	uint8_t Addr1 = 0;
    Pca9555_RegRead(I2C_LED, 0x40, &Addr);
	Addr1 = ~Addr[1];
	
	Addr1=(Addr1<<4)|(Addr1>>4);
	Addr1=((Addr1<<2)&0xcc)|((Addr1>>2)&0x33);
	Addr1=((Addr1<<1)&0xaa)|((Addr1>>1)&0x55);
	
    un_AddrAnalysis.Addr = Addr1;  //��ȡ���뿪�ص�ַ

    /* ����CAN ID��ַ*/
	un_DcdcIdAnalysis.All.None = 0x00;
    un_DcdcIdAnalysis.All.DeviceType = 0x04;  //�л���Ԫ
	un_DcdcIdAnalysis.All.WarehouseID = un_AddrAnalysis.All.WarehouseID;
    un_DcdcIdAnalysis.All.SourceAddr = un_AddrAnalysis.All.SourceID % 4;
    un_DcdcIdAnalysis.All.TargetAddr = un_AddrAnalysis.All.SourceID / 4;
    DcdcCanId = un_DcdcIdAnalysis.Addr;  //CAN ID

//    un_DcdcIdAnalysis.All.None = 0x00;
//    un_DcdcIdAnalysis.All.DeviceType = 0x03;  //DCDC
//    un_DcdcIdAnalysis.All.SourceAddr = un_AddrAnalysis.Addr / 4;
//    un_DcdcIdAnalysis.All.TargetAddr = un_AddrAnalysis.Addr % 4;
//    MaskCanId = un_DcdcIdAnalysis.Addr;  //CAN������
}


/**********************************************************************************************************
**  �� �� �� :
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
void ExternalAdDatProcess(void)
{
    static uint16_t ins_cnt = 0;
    static uint16_t sum_cnt = 0;
	float Vcm[8] = {0, 0, 0, 0, 0, 0, 0, 0};   //sense��ģ��ѹ
    float Vcm1[8] = {0, 0, 0, 0, 0, 0, 0, 0};  //̽�빲ģ��ѹ

    for(int i = 0; i < CHANNEL_MAX; i++)
    {
		if(ADC1_DATA[i] > 0x7FFFFF)
		{
			ADC1_DATA[i] = 0;
		}
		if(ADC2_DATA[i] > 0x7FFFFF)
		{
			ADC2_DATA[i] = 0;
		}
		
		Tier2.sampleCal.VtzIns[i] += ADC1_DATA[i];
        Tier2.sampleCal.VsenseIns[i] += ADC2_DATA[i];
        
		Tier2.sampleCal.VtzSum[i] += ADC1_DATA[i];
        Tier2.sampleCal.VsenseSum[i] += ADC2_DATA[i];
    }
    ins_cnt++;
    sum_cnt++;

    if(ins_cnt >= INS_SAMPLE_NUM)
    {
        for(int i = 0; i < CHANNEL_MAX; i++)
        {
            /* ��ģ��ѹ���� */
			Tier2.sampleCal.Vsense[i] = (float)((Tier2.sampleCal.VsenseIns[i] / ins_cnt) * Tier2.calibrate[i].VsenseK_C) / 0x7FFFFF +  Tier2.calibrate[i].VsenseB_C;
			Tier2.sampleCal.Vtz[i] = (float)((Tier2.sampleCal.VtzIns[i] / ins_cnt) * Tier2.calibrate[i].VtzK_C) / 0x7FFFFF + Tier2.calibrate[i].VtzB_C;
			/* Debugģʽ��ʾ */
            Tier2.debug.inq1[i].VsenceDebug_C = Tier2.sampleCal.Vsense[i];
            Tier2.debug.inq1[i].VtzDebug = Tier2.sampleCal.Vtz[i];
            /* ��ģ��ѹ���� */
            if(en_SysMode == Normal)  //��ͨģʽ
            {
                if(i == 0)
                {
                    Vcm[0] = 0;
                    Vcm1[0] = 0;
                }
                else
                {    
                    Vcm[i] = Vcm[i  - 1] + Tier2.sampleCal.Vsense[i];
                    Vcm1[i] = Vcm1[i  - 1] + Tier2.sampleCal.Vtz[i];
                }
            }
            else if(en_SysMode == Calibration)  //У׼ģʽ
            {
                Vcm[i] = st_DebugParameter.SetVsense;
                Vcm1[i] = st_DebugParameter.SetVtz;
            }
            
            /* ��ģ��ѹ���� */
            Tier2.sampleCal.Vsense[i] = Tier2.sampleCal.Vsense[i] + Vcm[i] * Tier2.calibrate[i].VsenseK_D + Tier2.calibrate[i].VsenseB_D; 
            Tier2.sampleCal.Vtz[i] = Tier2.sampleCal.Vtz[i] + Vcm[i] * Tier2.calibrate[i].VtzK_D + Tier2.calibrate[i].VtzB_D;
           
            /* Debugģʽ��ʾ */
            Tier2.debug.inq1[i].VcmDebug = Vcm[i];
            Tier2.debug.inq1[i].VsenseDebug_D = Tier2.sampleCal.Vsense[i];
			Tier2.debug.inq1[i].IoutDebug = Tier2.sampleCal.Iout[i];

            Tier2.sampleCal.VsenseIns[i] = 0;
            Tier2.sampleCal.VtzIns[i] = 0;				
        }
        ins_cnt = 0;
    }

    if(sum_cnt >= SUM_SAMPLE_NUM)
    {
        for(int i = 0; i < CHANNEL_MAX; i++)
        {
            if(Tier2.sampleCal.VsenseSum[i ] > (0x7FFFFF * SUM_SAMPLE_NUM))
            {
                Tier2.adSum[i].VsenseAdSum = 2 * (Tier2.sampleCal.VsenseSum[i] - (0x7FFFFF * SUM_SAMPLE_NUM)) / SUM_SAMPLE_NUM;
            }
            else
            {
                Tier2.adSum[i].VsenseAdSum = 0;
            }
			
            Tier2.adSum[i].VmodAdSum   = Tier2.sampleCal.VmodSum[i] / SUM_SAMPLE_NUM;
			
            if(Tier2.sampleCal.IoutSum[i] > (0x7FFFFF * SUM_SAMPLE_NUM))
            {
                Tier2.adSum[i].IoutAdSum = 2 * (Tier2.sampleCal.IoutSum[i] - (0x7FFFFF * SUM_SAMPLE_NUM)) / SUM_SAMPLE_NUM;
            }
            else
            {
				Tier2.adSum[i].IoutAdSum = 0;
			}

            if(Tier2.sampleCal.VtzSum[i] > (0x7FFFFF * SUM_SAMPLE_NUM))
            {
                Tier2.adSum[i].VtzAdSum = 2 * (Tier2.sampleCal.VtzSum[i] - (0x7FFFFF * SUM_SAMPLE_NUM)) / SUM_SAMPLE_NUM;
            }
            else
            {
                Tier2.adSum[i].VtzAdSum = 0;
            }
			
            Tier2.sampleCal.VsenseSum[i] = 0;
            Tier2.sampleCal.VmodSum[i] = 0;
            Tier2.sampleCal.IoutSum[i] = 0;
            Tier2.sampleCal.VtzSum[i] = 0;
        }
        sum_cnt = 0;
    }
}



/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
