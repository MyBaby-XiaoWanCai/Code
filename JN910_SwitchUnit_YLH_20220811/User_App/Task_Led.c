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
**                                          头文件引用区
**********************************************************************************************************/
#include "Task_Led.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
un_AddrAnalysis_Type    un_AddrAnalysis;
un_DcdcIdAnalysis_Type  un_DcdcIdAnalysis;
volatile uint32_t heart_beat_cnt = 0;

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void ReadAddr(void);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : AppTaskLed()
**	功能说明 : LED任务
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AppTaskLed(void *argument)
{
    uint16_t TaskCnt = 0;
	while(1)
	{
		if(clear_flag == 1)
		{
			clear_flag = 0;	
            
			//清除单工步告警标志位
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                Protect_Flag_Global[j]   = 0;
                Protect_Flag_Internal[j] = 0;
                Protect_Flag_Workstep[j] = 0;
                workstep_flag_ErrWord[j] = 0;

                protect_flag_upcast[j] = 0;
            }

            AlarmClear();  //清除内部告警和全局告警	
			
			dcdc_err_flag = 0;

			//DCDC告警清除指令
			DcdcRunStaSet(DEBUG_CH, CLEAR_FAULT);
			CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);
		}
        if(TaskCnt > 10)  //1S读取拨码开关地址
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
**  函 数 名 : ReadAddr()
**  功能说明 : 读取电路板ID号计算CAN ID
**  形    参 : 无
**  返 回 值 : 无
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
	
    un_AddrAnalysis.Addr = Addr1;  //读取拨码开关地址

    /* 计算CAN ID地址*/
	un_DcdcIdAnalysis.All.None = 0x00;
    un_DcdcIdAnalysis.All.DeviceType = 0x04;  //切换单元
	un_DcdcIdAnalysis.All.WarehouseID = un_AddrAnalysis.All.WarehouseID;
    un_DcdcIdAnalysis.All.SourceAddr = un_AddrAnalysis.All.SourceID % 4;
    un_DcdcIdAnalysis.All.TargetAddr = un_AddrAnalysis.All.SourceID / 4;
    DcdcCanId = un_DcdcIdAnalysis.Addr;  //CAN ID

//    un_DcdcIdAnalysis.All.None = 0x00;
//    un_DcdcIdAnalysis.All.DeviceType = 0x03;  //DCDC
//    un_DcdcIdAnalysis.All.SourceAddr = un_AddrAnalysis.Addr / 4;
//    un_DcdcIdAnalysis.All.TargetAddr = un_AddrAnalysis.Addr % 4;
//    MaskCanId = un_DcdcIdAnalysis.Addr;  //CAN过滤码
}


/**********************************************************************************************************
**  函 数 名 :
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
void ExternalAdDatProcess(void)
{
    static uint16_t ins_cnt = 0;
    static uint16_t sum_cnt = 0;
	float Vcm[8] = {0, 0, 0, 0, 0, 0, 0, 0};   //sense共模电压
    float Vcm1[8] = {0, 0, 0, 0, 0, 0, 0, 0};  //探针共模电压

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
            /* 差模电压计算 */
			Tier2.sampleCal.Vsense[i] = (float)((Tier2.sampleCal.VsenseIns[i] / ins_cnt) * Tier2.calibrate[i].VsenseK_C) / 0x7FFFFF +  Tier2.calibrate[i].VsenseB_C;
			Tier2.sampleCal.Vtz[i] = (float)((Tier2.sampleCal.VtzIns[i] / ins_cnt) * Tier2.calibrate[i].VtzK_C) / 0x7FFFFF + Tier2.calibrate[i].VtzB_C;
			/* Debug模式显示 */
            Tier2.debug.inq1[i].VsenceDebug_C = Tier2.sampleCal.Vsense[i];
            Tier2.debug.inq1[i].VtzDebug = Tier2.sampleCal.Vtz[i];
            /* 共模电压计算 */
            if(en_SysMode == Normal)  //普通模式
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
            else if(en_SysMode == Calibration)  //校准模式
            {
                Vcm[i] = st_DebugParameter.SetVsense;
                Vcm1[i] = st_DebugParameter.SetVtz;
            }
            
            /* 共模电压补偿 */
            Tier2.sampleCal.Vsense[i] = Tier2.sampleCal.Vsense[i] + Vcm[i] * Tier2.calibrate[i].VsenseK_D + Tier2.calibrate[i].VsenseB_D; 
            Tier2.sampleCal.Vtz[i] = Tier2.sampleCal.Vtz[i] + Vcm[i] * Tier2.calibrate[i].VtzK_D + Tier2.calibrate[i].VtzB_D;
           
            /* Debug模式显示 */
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
