/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     : JN95
** --------------------------------------------------------------------------------------------------------
** File name   : AppTaskprotection.c
** Author      : wl
** Date        : 2021-09—02
** Description : Protection application task file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"
#include <math.h>

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
extern uint8_t TW_HisAlam;          //图为历史告警
extern uint8_t TW_HisStatus;        //图为历史状态
uint64_t IP_Alarm_Flag[8] = {0};			// 内部保护标志位
uint32_t GP_Alarm_Flag[8] = {0};			// 全局保护标志位
uint32_t IP_Alarm_Flag_History[8] = {0};	// 内部保护历史标志位
uint32_t GP_Alarm_Flag_History[8] = {0};	// 全局保护历史标志位
uint8_t  First_Alarm = 0;					// 第一次报警标志(避免第一次重报) 0-第一次报警 1-不是第一次报警
uint8_t  SysModeProtMaskFlag = 0;			// 故障屏蔽标识(0-不屏蔽故障 1-屏蔽故障)
uint8_t  ucIProtCntPre[12][8] = {0};		// 内部故障检测计时(前次)
uint8_t  ucIProtCntCur[12][8] = {0};		// 内部故障检测计时(当前)
uint8_t  ucGProtCntPre[4][8] = {0};			// 全局故障检测计时(前次)
uint8_t  ucGProtCntCur[4][8] = {0};			// 全局故障检测计时(当前)
protect_alarmStruct Protect_Alarm;			// 内部报警帧

extern uint8_t features;					// 特征码
extern osSemaphoreId_t IPGP_Alarm_Semaphore;// IPGP告警信号量ID

extern acdc_alarmStruct acdc_AlarmBuf;      //网口ACDC状态报警发送buf
extern arr_list gdlist[];
extern arr_list pdlist[];
extern arr_list twlist[];
extern uint8_t Reconnection_Flg;

///*内部保护触发子命令查找表*/
//CMDProject IP_CheckList[] =
//{
//    {0x01, IntelProtect_OLA, 0, 0},			// 输出限流保护
//    {0x02, IntelProtect_BOV, 0, 0},			// BUS过压
//    {0x03, IntelProtect_BUV, 0, 0},			// BUS欠压
//    {0x04, IntelProtect_APA, 0, 0},			// 电流精度告警
//    {0x05, IntelProtect_UPA, 0, 0},			// 电压精度告警
//    {0x06, IntelProtect_VDQ, 0, 0},			// 短路保护告警
//    {0x07, IntelProtect_MOT, 0, 0},			// 模块过温
//    {0x08, IntelProtect_EOT, 0, 0},			// 环境过温
//    {0x09, IntelProtect_HPA, 0, 0},			// 硬件保护
//    {0x0a, IntelProtect_FHE, ALLCH, ALLCH},	// 风扇故障,没有使能设置命令，默认开启
//    {0x0b, IntelProtect_UHE, ALLCH, ALLCH},	// CPU异常,没有使能设置命令，默认开启
//    {0x0c, IntelProtect_C1E, ALLCH, ALLCH},	// can1异常,没有使能设置命令，默认开启
//    {0x0d, IntelProtect_C2E, ALLCH, ALLCH},	// can2异常,没有使能设置命令，默认开启
//    {0x0e, IntelProtect_DAE, ALLCH, ALLCH},	// DA异常,没有使能设置命令，默认开启
//    {0x0f, IntelProtect_ADE, ALLCH, ALLCH},	// AD异常,没有使能设置命令，默认开启
//    {0x10, IntelProtect_FLE, ALLCH, ALLCH},	// FLASH异常,没有使能设置命令，默认开启
//    {0x11, IntelProtect_EEE, ALLCH, ALLCH},	// Eeprom异常,没有使能设置命令，默认开启
//    {0x12, IntelProtect_FRE, ALLCH, ALLCH},	// FRAM异常,没有使能设置命令，默认开启
//    {0x13, IntelProtect_PME, ALLCH, ALLCH},	// 电位器异常,没有使能设置命令，默认开启
//		
//    {0x14, IntelProtect_VNN, 0, 0},			// 电压异常，协议新增
//    {0x15, IntelProtect_CVP, 0, 0},			// 回路压差保护，协议新增
//    {0x16, IntelProtect_VUQ, 0, 0},			// 电压快速上升，协议新增
//    {0x17, IntelProtect_CNN, 0, 0},			// 电流异常，协议新增
//    {0x18, IntelProtect_CUQ, 0, 0},			// 电流快速上升，协议新增
//    {0x19, IntelProtect_LOV, 0, 0},			// 辅助电压异常（探针电压大于5.1V），协议新增
//    {0x1A, IntelProtect_MAC, 0, 0},			// MAC异常
//		{0x1B, IntelProtect_MAC, 0, 0},			//通道模块异常  
//		{0x1C, IntelProtect_OTP, 0, 0},			// 硬件保护 - OTP
//		{0x1D, IntelProtect_OCP, 0, 0},			// 硬件保护 - OCP
//		{0x1E, IntelProtect_OVP, 0, 0},			// 硬件保护 - OVP
//		{0x1F, IntelProtect_NEG, 0, 0},			// 硬件保护 - 反接
//};

/*全局保护触发子命令查找表*/
CMDProject GP_CheckList[] =
{
    {0x01, GlobalProtect_OV, 0, 0},      	// 过压保护
    {0x02, GlobalProtect_UV, 0, 0},      	// 欠压保护
    {0x03, GlobalProtect_COA, 0, 0},     	// 充电过流保护
    {0x04, GlobalProtect_DOA, 0, 0},     	// 放电过流保护
    {0x05, GlobalProtect_OC, 0, 0},      	// 过容保护
    {0x06, GlobalProtect_MV, 0, 0},      	// 初始电压上限保护
    {0x07, GlobalProtect_LV, 0, 0},      	// 初始电压下限保护
    {0x08, IntelProtect_NONE1, 0, 0},    	// 未定义
    {0x09, IntelProtect_NONE2, 0, 0},    	// 未定义
    {0x0a, IntelProtect_VCV, 0, 0},      	// 接触检测电压变化量
    {0xfe, IntelProtect_OTE, 0, 0},      	// 其他故障
    {0xff, IntelProtect_INV, 0, 0},      	// 无效
};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
uint32_t IntelProtectDeviceCheck(void);         //内部保护硬件检测
uint32_t IntelProtectRunParmCheck(void);        //内部保护运行参数检测
uint32_t GlobleProtectRunParmCheck(void);       //全局保护运行参数检测
void AppTaskProtection(void *argument);

extern uint64_t bsp_GetRunTime(void);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/**********************************************************************************************************
**  函 数 名 : IntelProtectDeviceCheck()
**  功能说明 : 内部保护开机检测,用于cpu、can1、eeprom、fram,开机执行一次
**  形    参 : 无
**  返 回 值 : 内部保护报警状态
**********************************************************************************************************/
uint32_t IntelProtectDeviceCheck()
{
	if(SysModeProtMaskFlag == 0)	//故障屏蔽标识无效
	{	
//		//CPU异常：检测是否是REST电平引起的复位，且不是掉电再上电引起复位
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
//		RMU_ClrStatus();	//清除CLRF复位标志
//		PWC_Lock(PWC_UNLOCK_CODE_1);   
		
//		/* 检测ACDC(can1)模块,设置风扇转速 */
//
//#ifdef YCJ		
//        if(ACDC.Type == 0)												//8通道机型，只有一个ACDC，故障报0
//#else
//        if((ACDC.Type == 0) || (ACDC.Type == 11) || (ACDC.Type == 22))	//4通道机型，有2个ACDC，单ACDC故障=11或22，双ACDC故障=00
//#endif  
//		{
//			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
//			{
//				IP_Alarm_Flag[j] |= IntelProtect_C1E;	//can1故障
//			}
//		}

		/* 检测DA,读ID,在AD7124_CfgADC(),AD7175_CfgADC()内部已检测 */


		/* 检测AD,读ID,在dacx0508_cfg()内部已检测 */


		/* 检测flash,读ID */

		
		/* 检测fram,读ID */
		uint32_t FramID = 0;
		
		MB85Ctrl_ReadID(&FramID ,0);	//读取FRAM的ID
		
		if((FramID & 0xff000000) != 0x04000000)		//ID最高字节0x04代表制造商为富士
		{
			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{
				IP_Alarm_Flag[j] |= IntelProtect_FRE;	//fram故障
			}
		}

		/* 检测EE,读写倒数第三字节 */
		uint8_t TestDate = features;
		
		if(ee_WriteBytesWithCheck(&TestDate, Last_addr - 2, 1) == 0)
		{
			for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{
				IP_Alarm_Flag[j] |= IntelProtect_EEE;	//ee故障
			}
		}

		/* 检测电位器,设置寄存器读出设置值 */
	}
	else							//故障屏蔽标识有效
	{
		ACDC.Type = ACDC_Fan_Speed51_ModeTest();		//检查ACDC类型，并设置风扇到51%	
	}

  return 1;
}

extern uint16_t bus_under_vol_flg;
/**********************************************************************************************************
**  函 数 名 : IntelProtectRunParmCheck()
**  功能说明 : 内部保护运行参数检测,用于输出限流保护、bus过压、bus欠压等运行参数检测,执行周期1ms
**  形    参 : 无
**  返 回 值 : 内部保护报警状态
**********************************************************************************************************/
uint32_t IntelProtectRunParmCheck(void)
{	
    if(SysModeProtMaskFlag == 0)	//故障屏蔽标识无效
    {
		for(uint8_t i = 0; i < CHANNEL_CFG; i++)
		{
			//-------------------------------1.输出限流检测-------------------------------//
			/* 1.1 更新前次故障检测计时 */
			ucIProtCntPre[0][i] = ucIProtCntCur[0][i];	
			/* 1.2 故障检测逻辑 */
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
			/* 1.3 判断故障是否持续发生 */
			if(ucIProtCntPre[0][i] == ucIProtCntCur[0][i])
			{
				ucIProtCntPre[0][i] = 0;
				ucIProtCntCur[0][i] = 0;
			}			

			//-------------------------------2.BUS过压检测-------------------------------//
			/* 2.1 更新前次故障检测计时 */
			ucIProtCntPre[1][i] = ucIProtCntCur[1][i];
			ucIProtCntPre[10][i] = ucIProtCntCur[10][i];
			/* 2.2 故障检测逻辑 */			
			if((Tier2.iProt[i].VbusThrH.isEnable& 0x80) == 0)
			{
//				if(ProtectMaskFlag[i] != 1)              
				{
					if(Tier2.sampleCal.Vbus1 > Tier2.iProt[i].VbusThrH.value)		//bus1对应前一半通道
					{
						ucIProtCntCur[1][i]++;
						if(ucIProtCntCur[1][i] >= PROT_TRIG_MS(2000, 400))
						{						
							for(uint8_t j = 0; j < CHANNEL_CFG; j++)
							{
								IP_Alarm_Flag[j] |= IntelProtect_BOV;
							}
							ucIProtCntCur[1][i] = 0;
						}
					}
//					if(Tier2.sampleCal.Vbus2 > Tier2.iProt[i].VbusThrH.value)		//bus2对应后一半通道
//					{
//						ucIProtCntCur[10][i]++;
//						if(ucIProtCntCur[10][i] >= PROT_TRIG_MS(2000, 400))
//						{						
//							for(uint8_t j = (CHANNEL_CFG / 2); j < CHANNEL_CFG; j++)
//							{
//								IP_Alarm_Flag[j] |= IntelProtect_BOV;
//							}
//							ucIProtCntCur[10][i] = 0;
//						}
//					}
				}
			}
			/* 2.3 判断故障是否持续发生 */
			if(ucIProtCntPre[1][i] == ucIProtCntCur[1][i])
			{
				ucIProtCntPre[1][i] = 0;
				ucIProtCntCur[1][i] = 0;
			}
//			if(ucIProtCntPre[10][i] == ucIProtCntCur[10][i])
//			{
//				ucIProtCntPre[10][i] = 0;
//				ucIProtCntCur[10][i] = 0;
//			}		
			
#if 0			
			//-------------------------------3.BUS欠压检测-------------------------------//
			/* 3.1 更新前次故障检测计时 */
			ucIProtCntPre[2][i] = ucIProtCntCur[2][i];	
			ucIProtCntPre[11][i] = ucIProtCntCur[11][i];
			/* 3.2 故障检测逻辑 */
			if((Tier2.iProt[i].VbusThrL.isEnable & 0x80) == 0)
			{
//				if(ProtectMaskFlag[i] != 1)
        if(bus_under_vol_flg == 1)
				{				
					if(Tier2.sampleCal.Vbus1 < Tier2.iProt[i].VbusThrL.value)		//bus1对应前一半通道
					{
						ucIProtCntCur[2][i]++;
						if(ucIProtCntCur[2][i] >= PROT_TRIG_MS(400, 400))
						{
							for(uint8_t j = 0; j < CHANNEL_CFG; j++)
							{
								IP_Alarm_Flag[j] |= IntelProtect_BUV;
							}
							ucIProtCntCur[2][i] = 0;
						}
					}
				}
			}
			/* 3.3 判断故障是否持续发生 */
			if(ucIProtCntPre[2][i] == ucIProtCntCur[2][i])
			{
				ucIProtCntPre[2][i] = 0;
				ucIProtCntCur[2][i] = 0;
			}			
#endif		
			
			//-------------------------------4.电压精度检测-------------------------------//
			/* 4.1 更新前次故障检测计时 */
			ucIProtCntPre[3][i] = ucIProtCntCur[3][i];	
			/* 4.2 故障检测逻辑 */			
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
			/* 4.3 判断故障是否持续发生 */
			if(ucIProtCntPre[3][i] == ucIProtCntCur[3][i])
			{
				ucIProtCntPre[3][i] = 0;
				ucIProtCntCur[3][i] = 0;
			}
			
			//-------------------------------5.模块过温检测-------------------------------//
			/* 5.1 更新前次故障检测计时 */
			ucIProtCntPre[4][i] = ucIProtCntCur[4][i];	
			/* 5.2 故障检测逻辑 */	
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
			/* 5.3 判断故障是否持续发生 */
			if(ucIProtCntPre[4][i] == ucIProtCntCur[4][i])
			{
				ucIProtCntPre[4][i] = 0;
				ucIProtCntCur[4][i] = 0;
			}			

			//-------------------------------6.环境过温检测-------------------------------//
			/* 6.1 更新前次故障检测计时 */
			ucIProtCntPre[5][i] = ucIProtCntCur[5][i];	
			/* 6.2 故障检测逻辑 */				
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
			/* 6.3 判断故障是否持续发生 */
			if(ucIProtCntPre[5][i] == ucIProtCntCur[5][i])
			{
				ucIProtCntPre[5][i] = 0;
				ucIProtCntCur[5][i] = 0;
			}	

			//-------------------------------7.电压异常检测-------------------------------//
			/* 7.1 更新前次故障检测计时 */
			ucIProtCntPre[6][i] = ucIProtCntCur[6][i];
			/* 7.2 故障检测逻辑 */	
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
			/* 7.3 判断故障是否持续发生 */
			if(ucIProtCntPre[6][i] == ucIProtCntCur[6][i])
			{
				ucIProtCntPre[6][i] = 0;
				ucIProtCntCur[6][i] = 0;
			}			
			
			//-------------------------------8.回路压差检测-------------------------------//
			/* 8.1 更新前次故障检测计时 */
			ucIProtCntPre[7][i] = ucIProtCntCur[7][i];
			/* 8.2 故障检测逻辑 */	
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
			/* 8.3 判断故障是否持续发生 */
			if(ucIProtCntPre[7][i] == ucIProtCntCur[7][i])
			{
				ucIProtCntPre[7][i] = 0;
				ucIProtCntCur[7][i] = 0;
			}
			
			//-------------------------------9.电流精度检测-------------------------------//
			/* 9.1 更新前次故障检测计时 */
			ucIProtCntPre[8][i] = ucIProtCntCur[8][i];
			/* 9.2 故障检测逻辑 */	
			if((Tier2.iProt[i].IccDiff.isEnable & 0x80) == 0)
			{
				if( (Tier2.workstepRunning[i].StepType != SCMD_WORKSTEP_TYPE_STANDBY) && \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING ) && \
					(workstep_flag_CriticalZone[i] == 0) && (ProtectMaskFlag[i] != 1) )
				{
//					if(fabs(Tier2.chRecord[i].Iout - Tier2.workstepAnalyze[i].StepRunCur) > Tier2.iProt[i].IccDiff.value)
					if(fabs(Tier2.chRecord[i].Iout - Tier2.workstepAnalyze[i].StepRunCur) > (0.32*(CHANNEL_MAX / CHANNEL_CFG)))
					{
						ucIProtCntCur[8][i]++;
						if(ucIProtCntCur[8][i] >= PROT_TRIG_MS(2000, 400))
						{
#ifdef YCJ
							if(Tier2.chRecord[i].Iout < (0.1f*(CHANNEL_MAX / CHANNEL_CFG)))	//模块通道异常
							{
								IP_Alarm_Flag[i] |= IntelProtect_CHE;
							}
							else if(fabs(Tier2.chRecord[i].Iout - Tier2.workstepAnalyze[i].StepRunCur) > (2.0f*(CHANNEL_MAX / CHANNEL_CFG)))				//电流异常
							{
								IP_Alarm_Flag[i] |= IntelProtect_CNN;
							}
							else	//电流精度
							{
								IP_Alarm_Flag[i] |= IntelProtect_APA;
							}							
#else
							if(active_run_channel_numb == 1)	//单模块工作
							{
								if(Tier2.sampleCal.Iout[2 * i + 0] < 0.1f)													//模块通道异常
								{
									IP_Alarm_Flag[i] |= IntelProtect_CHE;
								}
								else if(fabs(Tier2.sampleCal.Iout[2 * i + 0] - Tier2.workstepAnalyze[i].StepRunCur) > 2.0f)	//电流异常
								{
									IP_Alarm_Flag[i] |= IntelProtect_CNN;
								}
								else																						//电流精度
								{
									IP_Alarm_Flag[i] |= IntelProtect_APA;
								}
							
							}
							else								//双模块工作
							{
								if((Tier2.sampleCal.Iout[2 * i + 0] < 0.1f) || (Tier2.sampleCal.Iout[2 * i + 1] < 0.1f))	//模块通道异常
								{
									IP_Alarm_Flag[i] |= IntelProtect_CHE;
								}
								else if(fabs(Tier2.chRecord[i].Iout - Tier2.workstepAnalyze[i].StepRunCur) > 2.0f)			//电流异常
								{
									IP_Alarm_Flag[i] |= IntelProtect_CNN;
								}
								else																						//电流精度
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
			/* 9.3 判断故障是否持续发生 */
			if(ucIProtCntPre[8][i] == ucIProtCntCur[8][i])
			{
				ucIProtCntPre[8][i]= 0;
				ucIProtCntCur[8][i] = 0;
			}
			
			//-------------------------------10.电流异常检测-------------------------------//
			/* 10.1 更新前次故障检测计时 */
			ucIProtCntPre[9][i] = ucIProtCntCur[9][i];
			/* 10.2 故障检测逻辑 */	
			if( (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_START  ) && \
				(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_CONTACT) && \
			   ((Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_RUNNING) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_STANDBY)) && \
				(ProtectMaskFlag[i] != 1) )
			{
				if(Tier2.chRecord[i].Iout > (0.1f*(CHANNEL_MAX / CHANNEL_CFG)))
				{
					ucIProtCntCur[9][i]++;
					if(ucIProtCntCur[9][i] >= PROT_TRIG_MS(2000, 400))
					{
						IP_Alarm_Flag[i] |= IntelProtect_CNN;
						ucIProtCntCur[9][i] = 0;
					}
				}			
			}
			/* 10.3 判断故障是否持续发生 */
			if(ucIProtCntPre[9][i] == ucIProtCntCur[9][i])
			{
				ucIProtCntPre[9][i] = 0;
				ucIProtCntCur[9][i] = 0;
			}
			
			//-------------------------------11.辅助电压异常检测-------------------------------//

		}	
    }

    return 1;
}

/**********************************************************************************************************
**  函 数 名 : GlobleProtectRunParmCheck()
**  功能说明 : 全局运行参数检测，用于电池电压上下限，回路压差，冲放电过流等运行参数检测，执行周期1ms
**  形    参 : 无
**  返 回 值 : 内部保护报警状态
**********************************************************************************************************/
uint32_t GlobleProtectRunParmCheck(void)
{
	if(SysModeProtMaskFlag == 0)	//故障屏蔽标识无效
	{
		for(uint8_t i = 0; i < CHANNEL_CFG; i++)
		{
			//-------------------------------1.电池过压检测-------------------------------//
			/* 1.1 更新前次故障检测计时 */
			ucGProtCntPre[0][i] = ucGProtCntCur[0][i];	
			/* 1.2 故障检测逻辑 */			
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
			/* 1.3 判断故障是否持续发生 */
			if(ucGProtCntPre[0][i] == ucGProtCntCur[0][i])
			{
				ucGProtCntPre[0][i] = 0;
				ucGProtCntCur[0][i] = 0;
			}
			
			//-------------------------------2.电池欠压检测-------------------------------//
			/* 2.1 更新前次故障检测计时 */
			ucGProtCntPre[1][i] = ucGProtCntCur[1][i];	
			/* 2.2 故障检测逻辑 */	
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
			/* 2.3 判断故障是否持续发生 */
			if(ucGProtCntPre[1][i] == ucGProtCntCur[1][i])
			{
				ucGProtCntPre[1][i] = 0;
				ucGProtCntCur[1][i] = 0;
			}			
			
			//-------------------------------3.充电过流检测-------------------------------//
			/* 3.1 更新前次故障检测计时 */
			ucGProtCntPre[2][i] = ucGProtCntCur[2][i];	
			/* 3.2 故障检测逻辑 */				
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
			/* 3.3 判断故障是否持续发生 */
			if(ucGProtCntPre[2][i] == ucGProtCntCur[2][i])
			{
				ucGProtCntPre[2][i] = 0;
				ucGProtCntCur[2][i] = 0;
			}
			
			//-------------------------------4.放电过流检测-------------------------------//
			/* 4.1 更新前次故障检测计时 */
			ucGProtCntPre[3][i] = ucGProtCntCur[3][i];	
			/* 4.2 故障检测逻辑 */	
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
			/* 4.3 判断故障是否持续发生 */
			if(ucGProtCntPre[3][i] == ucGProtCntCur[3][i])
			{
				ucGProtCntPre[3][i] = 0;
				ucGProtCntCur[3][i] = 0;
			}			

			//-------------------------------5.单工步过容检测-------------------------------//
		}
	}
	
  return 1;
}
#if 0
/**********************************************************************************************************
**  函 数 名 : IntelProtectDeviceCheck()
**  功能说明 : 内部保护开机检测，用于can1,DA,AD,flash,ee,fram,电位器检测，开机执行一次
**  形    参 : 无
**  返 回 值 : 内部保护报警状态
**********************************************************************************************************/
uint32_t IntelProtectDeviceCheck()
{
	uint32_t state;
	if(SysModeProtMaskFlag == 0)
	{	
		#if 0
		/* 检测CPU异常,判断是否发生外部看门狗复位 */
		if((RCC->CSR & RCC_CSR_PINRSTF) && (!(RCC->CSR & RCC_CSR_PORRSTF)))
		{
			RCC->CSR |= RCC_CSR_RMVF;				//清除RCC_CSR_PINRSTF标志
			for(uint8_t j = 0; j < CHANNEL_USE; j++)
			{ IP_Alarm_Flag[j] |= IntelProtect_UHE; }
		}
		else
		{
			RCC->CSR |= RCC_CSR_RMVF;				//清除RCC_CSR_PINRSTF标志
		}
		#endif
		
//		//检测是否是REST电平引起的复位，且不是掉电再上电引起复位
////		if( (bM4_RMU->RSTF0_b.PINRF == 0)&&(bM4_RMU->RSTF0_b.PORF !=0) )
//        if((RMU_GetStatus(RMU_RST_POWER_ON)==Reset)&&((RMU_GetStatus(RMU_RST_RESET_PIN)==Set)||(RMU_GetStatus(RMU_RST_WDT)==Set)))        
//		{
//			for(uint8_t j = 0; j < CHANNEL_USE; j++)
//			{ 
////				IP_Alarm_Flag[j] |= IntelProtect_UHE; 
//			}
//		}
//		PWC_Unlock(PWC_UNLOCK_CODE_1);			
//		RMU_ClrStatus(); //清除CLRF复位标志
		
//		/* 检测ACDC(can1)模块,设置风扇转速 */
//		ACDC.Type = ACDC_Fan_Speed51_ModeTest();		//检查ACDC类型，并设置风扇到51%
//#ifdef YCJ		
//       if(ACDC.Type == 0)        //8通道机型，只有一个ACDC，故障报0
//#else
//        if((ACDC.Type == 0)||(ACDC.Type == 11)||(ACDC.Type == 22)) //4通道机型有2个ACDC，单ACDC故障=11或22，双ACDC故障=00
//#endif  
//		{
//			for(uint8_t j = 0; j < CHANNEL_USE; j++)
//
//			{ IP_Alarm_Flag[j] |= IntelProtect_C1E; } //can1故障
//
//		}

		/* 检测DA,读ID,在AD7124_CfgADC(),AD7175_CfgADC()内部已检测 */


		/* 检测AD,读ID,在dacx0508_cfg()内部已检测 */

		
//		/* 检测flash,读ID */
//		uint16_t FlashID = 0;
//		FlashID = SPI_Flash_ReadID();
//		if((FlashID & 0xff00) != 0xEF00)			//ID最高字节0xEF代表制造商Winbond
//		{
//			for(uint8_t j = 0; j < CHANNEL_USE; j++)
//			{ 
//				IP_Alarm_Flag[j] |= IntelProtect_FLE; 
//			} //flash故障
//		}

		/*检测fram,读ID*/
		uint32_t FramID = 0;
		MB85Ctrl_ReadID(&FramID);
		if((FramID & 0xff000000) != 0x04000000)		//ID最高字节0x04代表制造商为富士
		{
			for(uint8_t j = 0; j < CHANNEL_USE; j++)
			{ 
				IP_Alarm_Flag[j] |= IntelProtect_FRE; 
			}   //fram故障
		}

		/*检测EE,读写倒数第三字节*/
		uint8_t TestDate = features;
		if(ee_WriteBytesWithCheck(&TestDate, Last_addr - 2, 1) == 0)
		{
			for(uint8_t j = 0; j < CHANNEL_USE; j++)
			{ 
				IP_Alarm_Flag[j] |= IntelProtect_EEE; 
			}  //ee故障
		}

//		/*检测电位器,设置寄存器读出设置值*/
//		if(AD5245_Vpro_Set(4.35) == 0)
//		{
//			IntelProtectFlg |= IntelProtect_PME;	//电位器故障
//		}
	}
//	else
//	{
//		ACDC.Type = ACDC_Fan_Speed51_ModeTest();		//检查ACDC类型，并设置风扇到51%	
//	}

    return 1;
}

uint8_t ucProtCntPre[8][8] = {0};
uint8_t ucProtCntCur[8][8] = {0};
#define PROT_TRIG_TIME	5

/**********************************************************************************************************
**  函 数 名 : IntelProtectRunParmCheck()
**  功能说明 : 内部保护运行参数检测，用于输出限流保护,bus过压,bus欠压等运行参数检测，执行周期1ms
**  形    参 : 无
**  返 回 值 : 内部保护报警状态
**********************************************************************************************************/
uint32_t IntelProtectRunParmCheck(void)
{	
	if(SysModeProtMaskFlag == 0)
	{
		for(uint8_t i = 0; i < CHANNEL_CFG; i++) //CHANNEL_USE 2022.04.20修改
		{
			/* 输出限流检测 */
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

			/* BUS过压检测 */
			if((Tier2.iProt[i].VbusThrH.isEnable& 0x80) == 0)
			{
				if(Tier2.sampleCal.Vbus1 > Tier2.iProt[i].VbusThrH.value)
				{
					for(uint8_t j = 0; j < CHANNEL_CFG; j++)
					{
						IP_Alarm_Flag[j] |= IntelProtect_BOV;
					}
				}
			}
			
			/* BUS欠压检测 */
			if((Tier2.iProt[i].VbusThrL.isEnable & 0x80) == 0)
			{
				if(Tier2.sampleCal.Vbus1 < Tier2.iProt[i].VbusThrL.value)
				{
					for(uint8_t j = 0; j < CHANNEL_CFG; j++)	
					{
						IP_Alarm_Flag[j] |= IntelProtect_BUV;
					}
				}
			}

			/* 电压精度检测 */
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
					
			/* 模块过温检测 */
			if((Tier2.iProt[i].Totmod.isEnable & 0x80) == 0)
			{
				if(Tier2.chRecord[i].Tmod > Tier2.iProt[i].Totmod.value)
				{
					IP_Alarm_Flag[i] |= IntelProtect_MOT;
					
					BatteryFormattingStop(i);	//停止电池充放电
				}
			}

			/* 环境过温检测 */
			if((Tier2.iProt[i].Totenv.isEnable & 0x80) == 0)
			{
				if(Tier2.sampleCal.Tamb > Tier2.iProt[i].Totenv.value)
				{
					IP_Alarm_Flag[i] |= IntelProtect_EOT;
				}
			}

//			/* 电压异常检测 */
//			ucProtCntPre[1][i] = ucProtCntCur[1][i];
//			if(SystemStatus.Neilsbed == Neilsbed_Nofit)			// 针床未压合
//			{
//				if((Tier2.iProt[i].SNERR.isEnable & 0x80) == 0)
//				{
//					if( (Tier2.chRecord[i].Vbat > Tier2.iProt[i].SNERR.value) || \
//						(Tier2.chRecord[i].Vout > Tier2.iProt[i].SNERR.value) || \
//						(Tier2.chRecord[i].Vtz  > Tier2.iProt[i].SNERR.value) )
//					{
//						ucProtCntCur[1][i]++;
//						
//						if(ucProtCntCur[1][i] >= PROT_TRIG_TIME)
//						{
////							IP_Alarm_Flag[i] |= IntelProtect_VNN;
//							ucProtCntCur[1][i] = 0;
//						}
//					}
//				}
//			}
//			else 
//			if(SystemStatus.Neilsbed == Neilsbed_Fitted)	// 针床已压合
//			{
//				if( (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_RUNNING) && \
//					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_CONTACT) && \
//					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_START) && (ProtectMaskFlag[i] != 1))	// 非充放电工步下
//				{
//					if((Tier2.iProt[i].SNERR.isEnable & 0x80) == 0)
//					{
//						if( (fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vtz ) > (2 * Tier2.iProt[i].SNERR.value)) || \
//							(fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vout) > (2 * Tier2.iProt[i].SNERR.value)) || \
//							(fabs(Tier2.chRecord[i].Vtz  - Tier2.chRecord[i].Vout) > (2 * Tier2.iProt[i].SNERR.value)) )
//						{
//							ucProtCntCur[1][i]++;
//							
//							if(ucProtCntCur[1][i] >= PROT_TRIG_TIME)
//							{
////								IP_Alarm_Flag[i] |= IntelProtect_VNN;
//								ucProtCntCur[1][i] = 0;
//							}
//						}
//					}
//				}
//			}
//			if(ucProtCntPre[1][i] == ucProtCntCur[1][i])
//			{
//				ucProtCntPre[1][i] = 0;
//				ucProtCntCur[1][i] = 0;
//			}			

			/* 电压异常检测 */
			ucProtCntPre[1][i] = ucProtCntCur[1][i];
			if( ((Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_RUNNING) || \
				((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_STANDBY))) && \
				(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_CONTACT ) && \
				(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_START   ) && \
				(ProtectMaskFlag[i] != 1) )	// 非充放电工步下
			{
				if((Tier2.iProt[i].SNERR.isEnable & 0x80) == 0)
				{
					if( (fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vtz ) > (2 * Tier2.iProt[i].SNERR.value)) || \
						(fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vout) > (2 * Tier2.iProt[i].SNERR.value)) || \
						(fabs(Tier2.chRecord[i].Vtz  - Tier2.chRecord[i].Vout) > (2 * Tier2.iProt[i].SNERR.value)) )
					{
						if(ProtectMaskFlag[i] != 1)
						{
							ucProtCntCur[1][i]++;
							if(ucProtCntCur[1][i] >= (PROT_TRIG_TIME * 2))
							{
//								IP_Alarm_Flag[i] |= IntelProtect_VNN;
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

			/*回路压差检测*/
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
			
			/* 电流精度检测 */
			ucProtCntPre[3][i] = ucProtCntCur[3][i];
			if((Tier2.iProt[i].IccDiff.isEnable & 0x80) == 0)
			{
				if( ((Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CC ) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCC)  || \
					 (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CV ) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CCTOCV  )  || \
					 (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCV) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV)) && \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
					(workstep_flag_CriticalZone[i] == 0) && (ProtectMaskFlag[i] != 1) )
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

			/*电流异常检测*/
			ucProtCntPre[4][i] = ucProtCntCur[4][i];
			if( ((Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_RUNNING) || \
							((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_STANDBY))) && \
							(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_CONTACT ) && \
							(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_START   ) && \
							(ProtectMaskFlag[i] != 1) )        // 非充放电工步下
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
			if(ucProtCntPre[4][i] == ucProtCntCur[4][i])
			{
				ucProtCntPre[4][i] = 0;
				ucProtCntCur[4][i] = 0;
			}
			
//			/*电流异常检测*/
//			ucProtCntPre[4][i] = ucProtCntCur[4][i];
//			if( (SystemStatus.Neilsbed == Neilsbed_Nofit) || \
//				(SystemStatus.Neilsbed == Neilsbed_Fitting) )	// 针床未压合/压合中
//			{
//				if(Tier2.chRecord[i].Iout > 0.1f)
//				{
//					ucProtCntCur[4][i]++;
//					
//					if(ucProtCntCur[4][i] >= PROT_TRIG_TIME)
//					{
//						IP_Alarm_Flag[i] |= IntelProtect_CNN;
//						ucProtCntCur[4][i] = 0;
//					}
//				}
//			}
//			else 
//			if(SystemStatus.Neilsbed == Neilsbed_Fitted)	// 针床已压合
//			{
//				if( (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_RUNNING) && \
//					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_CONTACT) && \
//					(ProtectMaskFlag[i] != 1) )					// 非充放电工步下
//				{
//					if( Tier2.chRecord[i].Iout > ((CHANNEL_MAX / CHANNEL_CFG)*0.1f))
////					if(Tier2.chRecord[i].Iout > 0.1f)	
//					{
//						ucProtCntCur[4][i]++;
//						
//						if(ucProtCntCur[4][i] >= PROT_TRIG_TIME)
//						{
//							IP_Alarm_Flag[i] |= IntelProtect_CNN;
//							ucProtCntCur[4][i] = 0;
//						}
//					}			
//				}
//			}
//			if(ucProtCntPre[4][i] == ucProtCntCur[4][i])
//			{
//				ucProtCntPre[4][i] = 0;
//				ucProtCntCur[4][i] = 0;
//			}
			
//			/*辅助电压异常检测（用探针电压替代）*/
//			if(Tier2.chRecord[i].Vtz > Tier2.iProt[i].VauxThr.value)
//			{
//                IP_Alarm_Flag[i] |= IntelProtect_LOV;
//			}
		}	
  }
  return 1;
}

/**********************************************************************************************************
**  函 数 名 : GlobleProtectRunParmCheck()
**  功能说明 : 全局运行参数检测，用于电池电压上下限，回路压差，冲放电过流等运行参数检测，执行周期1ms
**  形    参 : 无
**  返 回 值 : 内部保护报警状态
**********************************************************************************************************/
uint32_t GlobleProtectRunParmCheck(void)
{
	if(SysModeProtMaskFlag == 0)
	{
		for(uint8_t i = 0; i < CHANNEL_USE; i++)
		{
			/*电池过压检测*/
			if((Tier2.gProt[i].VbatThrH.isEnable & 0x80) == 0)
			{
				if(Tier2.chRecord[i].Vbat > Tier2.gProt[i].VbatThrH.value)
				{
					GP_Alarm_Flag[i] |= GlobalProtect_OV;
				}
			}

//			/*电池欠压检测*/
//			if((Tier2.gProt[i].VbatThrL.isEnable & 0x80) == 0)
//			{
//				if(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
//				{
//					if(Tier2.chRecord[i].Vbat < Tier2.gProt[i].VbatThrL.value)
//					{
//						GP_Alarm_Flag[i] |= GlobalProtect_UV;
//					}
//				}
//			}

			/*电池欠压检测*/
			if((Tier2.gProt[i].VbatThrL.isEnable & 0x80) == 0)
			{
				if((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && (ProtectMaskFlag[i] != 1))
				{
					if(Tier2.chRecord[i].Vbat < Tier2.gProt[i].VbatThrL.value)
					{
						GP_Alarm_Flag[i] |= GlobalProtect_UV;
					}
				}
			}
            
			/*充电过流检测*/
			if((Tier2.gProt[i].Iovcharge.isEnable & 0x80) == 0)
			{
				if((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) && (ProtectMaskFlag[i] != 1))
				{
					if(Tier2.chRecord[i].Iout > Tier2.gProt[i].Iovcharge.value)
					{
						GP_Alarm_Flag[i] |= GlobalProtect_COA;
					}			
				}
				if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
					( (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CC    ) || \
					  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CV    ) || \
					  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CCTOCV) ) && (ProtectMaskFlag[i] != 1) )
				{
					if(Tier2.chRecord[i].Iout > Tier2.gProt[i].Iovcharge.value)
					{
						GP_Alarm_Flag[i] |= GlobalProtect_COA;
					}			
				}
			}

			/*放电过流检测*/
			if((Tier2.gProt[i].Iovdischg.isEnable & 0x80) == 0)
			{
				if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) && \
					( (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCC     ) || \
					  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCV     ) || \
					  (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV) ) && (ProtectMaskFlag[i] != 1) )
				{
					if(Tier2.chRecord[i].Iout > Tier2.gProt[i].Iovdischg.value)
					{
						GP_Alarm_Flag[i] |= GlobalProtect_DOA;
					}			
				}		
			}
		}
	}
	
  return 1;
}

#endif

/**********************************************************************************************************
**  函 数 名 : AlarmQuarry()
**  功能说明 : 查询实时报警状态
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AlarmQuarry(void)
{
    uint8_t  i   = 0;
    uint16_t cnt = 0;		//子命令计数
    uint16_t lenth = 0;		//长度
    uint16_t crc16 = 0;		//校验码
    uint8_t AlarmSendBuf[1024] = {0};

    AlarmSendBuf[0] = 0x5B;
    AlarmSendBuf[1] = 0x5B;	//下位机给中位机发，帧头0x5B5B
    AlarmSendBuf[2] = 0x09;
    AlarmSendBuf[3] = 0x00;	//固定长度9
    AlarmSendBuf[4] = 0x00;	//特征码，每次上报自加1
    AlarmSendBuf[5] = 0x00;	//状态码0-OK
    AlarmSendBuf[6] = 0x00;	//通道号

    IntelProtectRunParmCheck();    //内部保护检测
    GlobleProtectRunParmCheck();   //全局保护检测

    /*内部保护，全局保护查询上抛，按通道查询*/
    for(i = 0; i < CHANNEL_USE; i++)
    {
        if(IP_Alarm_Flag[i] != 0)
        {
            AlarmSendBuf[4] = features++;	//特征码+1
            AlarmSendBuf[6] = 0x01 << i;	//内部保护报警通道号
            AlarmSendBuf[7] = 0xA0;			//主命令，内部保护报警,固定0xa0, 全局保护报警：0x83
            for(uint8_t j = 0; j < sizeof(IP_CheckList) / sizeof(CMDProject); j++)
            {
                if(IP_CheckList[j].P_Flg & IP_Alarm_Flag[i])
                {
                    cnt++;
                    AlarmSendBuf[7 + cnt] = IP_CheckList[j].childCMD;
                }
            }
            lenth = 8 + cnt;
            AlarmSendBuf[2] = lenth & 0x00ff;	//更新帧长
            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;
            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt);	//计算CRC校验字段
            AlarmSendBuf[8 + cnt] = crc16 & 0x00ff;
            AlarmSendBuf[9 + cnt] = (crc16 & 0xff00) >> 8;
            AlarmSendBuf[10 + cnt] = 0xa5;		//添加帧尾
            AlarmSendBuf[11 + cnt] = 0xa5;
            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);	//网口发送报警信息，0为服务器socket
			
						Can_Network_Upcast((uint8_t *)&AlarmSendBuf, lenth + 4);
			
            First_Alarm=1;
            cnt = 0;			//子命令计数清零
            bsp_DelayMS(1);
        }

        cnt = 0;		//子命令计数清零

        if(GP_Alarm_Flag[i] != 0)
        {
            AlarmSendBuf[4] = features++;        //特征码+1
            AlarmSendBuf[6] = 0x01 << i; //报警通道号
            AlarmSendBuf[7] = 0x83;   //主命令，内部保护报警,固定0xa0, 全局保护报警：0x83
            for(uint8_t j = 0; j < sizeof(GP_CheckList) / sizeof(CMDProject); j++)
                //添加子命令
            {
                if(GP_CheckList[j].P_Flg & GP_Alarm_Flag[i])
                {
                    cnt++;
                    AlarmSendBuf[7 + cnt] = GP_CheckList[j].childCMD;
                }
            }
            lenth = 8 + cnt;
            AlarmSendBuf[2] = lenth & 0x0ff; //更新帧长
            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;

            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt); //计算CRC校验字段
            AlarmSendBuf[8 + cnt] = crc16 & 0x00ff;
            AlarmSendBuf[9 + cnt] = (crc16 & 0xff00) >> 8;

            AlarmSendBuf[10 + cnt] = 0xa5; //添加帧尾
            AlarmSendBuf[11 + cnt] = 0xa5;

            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);  //网口发送报警信息，0为服务器socket
			
						Can_Network_Upcast((uint8_t *)&AlarmSendBuf, lenth + 4);
			
            First_Alarm=1;
            cnt = 0;   //子命令计数清零
            bsp_DelayMS(1);
        }
    }
	
    /*ACDC告警查询上抛*/
    uint16_t alarm1, alarm2; //bit=1的位是有变化且状态为告警的位
    
    /*ACDC报警，帧头2字节+数据长度2字节+特征码1字节+状态码1字节+通道标志1字节+功能字段2字节（命令码+子命令）+校验2字节+帧尾2字节*/
    acdc_AlarmBuf.FramHead = 0x5B5B;   //下位机给中位机发，帧头0x5B5B
    acdc_AlarmBuf.Datelen = 0x09;      //固定长度9
    acdc_AlarmBuf.FeatureCode = 0x00;  //特征码，每次上报自加1
    acdc_AlarmBuf.StateCode = 0x00;    //状态码=0，OK
    acdc_AlarmBuf.ChanelFlag = 0x0F;   //ACDC报警时，固定0
    acdc_AlarmBuf.CommandCode = 0xA1;  //ACDC报警,固定
    acdc_AlarmBuf.ChildCommand = 0x00; //根据can上报内容和子命令码定义确定
    acdc_AlarmBuf.CRC16Code = 0x00;    //需最后计算
    acdc_AlarmBuf.FramTail = 0xA5A5;   //帧尾

    //解析ACDC当前状态，向网口上报数据，值为1的bit
    alarm1 = ACDC.State[0];
    alarm2 = ACDC.State[1];

    //判断故障所在通道
    if(alarm1)
    {
        acdc_AlarmBuf.ChanelFlag |= 0x00;	//ACDC报警不区分canid，通道标志为输出通道，报警时默认0
    }
    if(alarm2)
    {
        acdc_AlarmBuf.ChanelFlag |= 0x00;	//ACDC报警不区分canid，通道标志为输出通道，报警时默认0
    }

    //根据使用的ACDC类型，按厂家协议手册确定报警类型
    switch(ACDC.Type)
    {
    case 1:
		case 11:		//国电报警处理
		{
            for(i = 0; i < 8; i++)
            {
                if(alarm1 & gdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = gdlist[i].ChildCom; //查表获取子命令码
                    acdc_AlarmBuf.FeatureCode = features++;         //特征码+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //计算CRC校验字段
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //网口发送报警信息，0为服务器socket
					
										Can_Network_Upcast((uint8_t *)&acdc_AlarmBuf.FramHead, 13);
                }
                if(alarm2 & gdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = gdlist[i].ChildCom; //查表获取子命令码
                    acdc_AlarmBuf.FeatureCode = features++;         //特征码+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //计算CRC校验字段
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //网口发送报警信息，0为服务器socket
					
										Can_Network_Upcast((uint8_t *)&acdc_AlarmBuf.FramHead, 13);
                }
            }
		}
		break;

    case 2:
		case 22:		//普德报警处理
		{
            for(i = 0; i < 11; i++)
            {
                if(alarm1 & pdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = pdlist[i].ChildCom; //查表获取子命令码
                    acdc_AlarmBuf.FeatureCode = features++;         //特征码+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //计算CRC校验字段
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //网口发送报警信息，0为服务器socket
					
										Can_Network_Upcast((uint8_t *)&acdc_AlarmBuf.FramHead, 13);
                }
                if(alarm2 & pdlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = pdlist[i].ChildCom; //查表获取子命令码
                    acdc_AlarmBuf.FeatureCode = features++;         //特征码+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //计算CRC校验字段
                    W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //网口发送报警信息，0为服务器socket
					
										Can_Network_Upcast((uint8_t *)&acdc_AlarmBuf.FramHead, 13);
                }
            }
		}
		break;
    case 3:   //图为告警处理

            for(i = 0; i < 8; i++)
            {
                if(TW_HisAlam & twlist[i].state)
                {
                    Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                    acdc_AlarmBuf.ChildCommand = twlist[i].ChildCom; //查表获取子命令码
                    acdc_AlarmBuf.FeatureCode = features++;         //特征码+1
                    acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //计算CRC校验字段
                    Can_Network_Upcast((uint8_t *)&acdc_AlarmBuf.FramHead, 13);
                }
            }

            if((TW_HisStatus & 0x01)==0)
            {
                Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);
                acdc_AlarmBuf.ChildCommand = twlist[8].ChildCom; //查表获取子命令码
                acdc_AlarmBuf.FeatureCode = features++;         //特征码+1
                acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //计算CRC校验字段
                Can_Network_Upcast((uint8_t *)&acdc_AlarmBuf.FramHead, 13);
            }



        break;
			
    default:
		{
		
		}
        break;	
	}
}

/**********************************************************************************************************
**  函 数 名 : Mod_En_Clear()
**  功能说明 : 检查清除硬件告警
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
extern uint64_t PG_Fault_Weaver_Record[8];

void Mod_En_Clear(void)
{
//    uint8_t i;
//    uint8_t chanal_flg = 0; //故障通道标志
	
#if 0
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
#endif
	
	
	for(uint8_t j = 0;j < 8;j++)
	{
		if(	(PG_Fault_Weaver_Record[j] & IntelProtect_OCP) || (PG_Fault_Weaver_Record[j] & IntelProtect_OVP) )
		{
			Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_OUT_DISABLE);
			PG_Fault_Weaver_Record[j] = 0;
		}		
	}
}

/**********************************************************************************************************
**  函 数 名 : AlarmClear()
**  功能说明 : 清除内部和全局报警
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AlarmClear(void)
{
		uint8_t i;

		/*查询清除硬件保护告警*/
		Mod_En_Clear();

//	/*清除普德告警状态*/
//	if((ACDC.Type==2)||(ACDC.Type==22))//普德
//	{
//		uint32_t ID2 = 0xC600000B;
//		uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//		Tdata2[0] = 0x01;
//		Tdata2[3] = 0x42;
//		Tdata2[7] = 0x01;      //广播清告警
//		CANx_SendData(APP_CAN2_UNIT, ID2, &Tdata2[0], 8);    
//	}		

	/*更新告警标志，重复检测结果不自动清零*/
	for(i = 0; i < 8; i++)
	{
		IP_Alarm_Flag[i] = 0;
		IP_Alarm_Flag_History[i] = 0;
		GP_Alarm_Flag[i] = 0;
		GP_Alarm_Flag_History[i] = 0;
	}

	/*清除ACDC告警*/
	ACDC.State[0] = 0;
	ACDC.State[1] = 0;
	TW_HisAlam = 0;              //图为历史告警
	TW_HisStatus |= 0x01;        //图为历史状态
}

/**********************************************************************************************************
**  函 数 名 : AppTaskProtection()
**  功能说明 : 保护任务执行功能
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AppTaskProtection(void *argument)
{
    uint8_t i;
    uint16_t cnt = 0; //子命令计数
    uint16_t lenth;  //长度
    uint16_t crc16;  //校验码

    uint8_t AlarmSendBuf[1024] = {0};

    AlarmSendBuf[0] = 0x5B;
    AlarmSendBuf[1] = 0x5B;    //下位机给中位机发，帧头0x5B5B

    AlarmSendBuf[2] = 0x09;
    AlarmSendBuf[3] = 0x00;    //固定长度9

    AlarmSendBuf[4] = 0x00;    //特征码，每次上报自加1
    AlarmSendBuf[5] = 0x00;    //状态码=0，OK
    AlarmSendBuf[6] = 0x00;    //通道号

    while(1)
    {
        if(osSemaphoreAcquire(IPGP_Alarm_Semaphore, osWaitForever) == osOK) //等待同步信号量
        {
						if(Reconnection_Flg == 0x01)
						{
							AlarmQuarry();	//重连告警上抛
							Reconnection_Flg = 0x00;
						}
						
            if(First_Alarm == 0)
            {
                /*内部保护，全局保护查询上抛，按通道查询*/
                for(i = 0; i < CHANNEL_USE; i++)
                {
                    if(IP_Alarm_Flag[i] != 0) //内部告警标志非0
                    {
                        if(IP_Alarm_Flag[i] != IP_Alarm_Flag_History[i]) //内部告警标志非0且不等于历史值,准备上抛告警
                        {
                            Protect_Action_Func(i, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP);  //执行告警动作
                            IP_Alarm_Flag_History[i] = IP_Alarm_Flag[i]; //更新新的告警历史记录
                            AlarmSendBuf[4] = features++;        //特征码+1
                            AlarmSendBuf[6] = 0x01 << i; //内部保护报警通道号
                            AlarmSendBuf[7] = 0xA0;   //主命令，内部保护报警,固定0xa0, 全局保护报警：0x83
                            for(uint8_t j = 0; j < sizeof(IP_CheckList) / sizeof(CMDProject); j++)
                            //添加子命令
                            {
                                if(IP_CheckList[j].P_Flg & IP_Alarm_Flag[i])
                                {
                                    cnt++;
                                    AlarmSendBuf[7 + cnt] = IP_CheckList[j].childCMD;
                                }
                            }
                            lenth = 8 + cnt;
                            AlarmSendBuf[2] = lenth & 0x00ff; //更新帧长
                            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;

                            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt); //计算CRC校验字段
                            AlarmSendBuf[8 + cnt] = crc16 & 0x00FF ;
                            AlarmSendBuf[9 + cnt] = (crc16 & 0xFF00) >> 8;

                            AlarmSendBuf[10 + cnt] = 0xa5; //添加帧尾
                            AlarmSendBuf[11 + cnt] = 0xa5;

                            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);  //网口发送报警信息，0为服务器socket
							
														Can_Network_Upcast((uint8_t *)&AlarmSendBuf, lenth + 4);

                            cnt = 0;   //子命令计数清零
                        }
                    }

                    cnt = 0;   //子命令计数清零

                    if(GP_Alarm_Flag[i] != 0) //全局告警标志非0
                    {
                        if(GP_Alarm_Flag[i] != GP_Alarm_Flag_History[i]) //全局告警标志非0且不等于历史值
                        {
                            Protect_Action_Func(i, PROTECT_GLOBAL, WORKSTEP_RUNSTA_REF_STOP);//执行告警动作
                            GP_Alarm_Flag_History[i] = GP_Alarm_Flag[i]; //更新新的告警历史记录
                            AlarmSendBuf[4] = features++;        //特征码+1
                            AlarmSendBuf[6] = 0x01 << i; //报警通道号
                            AlarmSendBuf[7] = 0x83;   //主命令，内部保护报警,固定0xa0, 全局保护报警：0x83
                            for(uint8_t j = 0; j < sizeof(GP_CheckList) / sizeof(CMDProject); j++)
                                //添加子命令
                            {
                                if(GP_CheckList[j].P_Flg & GP_Alarm_Flag[i])
                                {
                                    cnt++;
                                    AlarmSendBuf[7 + cnt] = GP_CheckList[j].childCMD;
                                }
                            }
                            lenth = 8 + cnt;
                            AlarmSendBuf[2] = (lenth & 0x00ff); //更新帧长
                            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;

                            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt); //计算CRC校验字段
                            AlarmSendBuf[8 + cnt] = crc16 & 0x00FF;
                            AlarmSendBuf[9 + cnt] = (crc16 & 0xFF00) >> 8;

                            AlarmSendBuf[10 + cnt] = 0xa5; //添加帧尾
                            AlarmSendBuf[11 + cnt] = 0xa5;

                            W5500_send(0, (uint8_t *)&AlarmSendBuf, lenth + 4);  //网口发送报警信息，0为服务器socket
							
														Can_Network_Upcast((uint8_t *)&AlarmSendBuf, lenth + 4);
							
                            cnt = 0;   //子命令计数清零
                        }
                    }
                }
            }
            else
            {
                First_Alarm = 0;
                for(i = 0; i < CHANNEL_USE; i++)
                {
                    if(IP_Alarm_Flag[i] != 0) //内部告警标志非0
                    {
                        if(IP_Alarm_Flag[i] != IP_Alarm_Flag_History[i]) //内部告警标志非0且不等于历史值,准备上抛告警
                        {
                            Protect_Action_Func(i, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP);  //执行告警保护动作
                            IP_Alarm_Flag_History[i] = IP_Alarm_Flag[i]; //更新新的告警历史记录
                        }
                    }

                    if(GP_Alarm_Flag[i] != 0) //全局告警标志非0
                    {
                        if(GP_Alarm_Flag[i] != GP_Alarm_Flag_History[i]) //全局告警标志非0且不等于历史值
                        {
                            Protect_Action_Func(i, PROTECT_GLOBAL, WORKSTEP_RUNSTA_REF_STOP);//执行告警动作
                            GP_Alarm_Flag_History[i] = GP_Alarm_Flag[i]; //更新新的告警历史记录
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
