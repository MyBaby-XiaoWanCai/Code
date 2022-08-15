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
**                                          头文件引用区
**********************************************************************************************************/
#include "global.h"
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t FCT_ReadBuf[64] = {0};
uint8_t FCT_WriteBuf[64] = {0};
uint8_t FCT_ResClear[64] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
														0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
extern PcaDataType Pca9555_Data;
extern TestInformation IOTestInfo;	//检测IO信息存储
extern Vers_Mana vers_mana;			//主控板读取版本信息存储
uint8_t FCTEndFlg = 0x01;
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : WholeMachine_FCT()
**	功能说明 : 整机FCT功能函数-当需调用此功能时,需进入"main.h"中打开对应宏定义
**	形    参 : 
**	返 回 值 : 
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
			tmp = WholeMachine_FCTSatart(); //启动准备工作
			
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
		case FCT_REGINSIGNAL: //测试项1
		{
			memset(FCT_WriteBuf,0x00,64);
			ee_WriteBytesWithCheck(&FCT_ResClear[0], FCT_EE_ADDR_A, 57);	//清除上一次FCT结果
			osDelay(200);
			tmp = WholeMachine_FCTReginSignal();	// - 在位信号检测
			StepTemp = FCT_VERSION;		//FCT_VERSION
			break;
		}
		case FCT_VERSION: //测试项2
		{
			tmp = WholeMachine_FCTVersion();	//版本检测
			StepTemp = FCT_LAMPPLATE;
			break;
		}
		case FCT_LAMPPLATE: //测试项3
		{
			tmp = WholeMachine_FCTLampPlate();	//测试项3 - 灯板测试
			StepTemp = FCT_FAN;
			break;
		}
		case FCT_FAN: //测试项4
		{
			tmp = WholeMachine_FCTFan(); //风扇测试
			StepTemp = FCT_TMP;
			break;
		}
		case FCT_TMP: //测试项5
		{
			tmp = WholeMachine_FCTTmp75(); //温度测试
			StepTemp = FCT_CHARGEDISC;	//FCT_CHARGEDISC
			break;
		}
		case FCT_CHARGEDISC: //测试项6
		{
			tmp = WholeMachine_FCTChargeDisc(); //充放电测试
			StepTemp = FCT_DISCCHARGE;
			break;
		}
		case FCT_DISCCHARGE: //测试项7
		{
			tmp = WholeMachine_FCTDischCharge(); //放充电测试
			StepTemp = FCT_VSENSEFAST; //FCT_VSENSEFAST
			break;
		}
		case FCT_VSENSEFAST:	//测试项8
		{
			tmp = WholeMachine_FCTVsenseRapidLoop(); //Vsense快速环测试
			StepTemp = FCT_VSENSESLOW; //FCT_VSENSESLOW
			break;
		}
		case FCT_VSENSESLOW:	//测试项9
		{
			tmp = WholeMachine_FCTVsenseSlowLoop(); //Vsense慢速环测试
			StepTemp = FCTSTEP_END;
			break;
		}
		case FCTSTEP_END:	//FCT结束
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
**	函 数 名 : WholeMachine_FCTSatart()
**	功能说明 : 整机FCT start函数
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTSatart()
{
	uint8_t err_tmp = 0;

	/* 查询eeprom地址A-1,如果为0则进入自动老化模式,如果非0则开始自检测试 */
	ee_ReadBytes(&FCT_ReadBuf[0], FCT_EE_ADDR_A_1, 1);
	
	if(FCT_ReadBuf[0] == 0x00)
	{
		FCTEndFlg = 0x01;
		
		err_tmp = 1;
		Ageing();		//进入老化模式
	}
//	else
//	{
//		SelfChecking();	//进入自检模式
//	}

	return err_tmp;
}

/**********************************************************************************************************
**	函 数 名 : WholeMachine_FCTReginSignal()
**	功能说明 : 整机FCT测试步骤 - 在位信号检测
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTReginSignal()
{
	uint8_t err_tmp = 0;
	uint8_t ReginSignalCnt = 0;
	
	/* 读取控制板及功率板在位信号 */
	MainPowerSignalRead();	//IOTestInfo.MainPowerSignal[reign_i] 在位信号存储
	
	for(ReginSignalCnt = 0;ReginSignalCnt < 9; ReginSignalCnt++)
	{
		if(ReginSignalCnt < (1 + CHANNEL_USE))
		{
			if(IOTestInfo.MainPowerSignal[ReginSignalCnt] == 0x01)	//不在位 - 写1
			{
				err_tmp = 1;
				if(ReginSignalCnt == 0)	//主控
				{
					FCT_WriteBuf[1] = 0x01;

				}
				else					//功率板不在位
				{
					FCT_WriteBuf[ReginSignalCnt + 1] = 0x01;

				}
			}
			else	//在位 - 写0 
			{
				if(ReginSignalCnt == 0)	//主控在位
				{
					FCT_WriteBuf[1] = 0x00;

				}
				else					//功率板在位 buf: 2 - 9
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
**	函 数 名 : WholeMachine_FCTVersion()
**	功能说明 : 整机FCT测试步骤 - 版本检测
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTVersion()
{
	uint8_t err_tmp = 0;
	uint8_t VerReadCnt = 0;
	
	/* 控制板版本号判断 */
	if((vers_mana.Bom_Vers == BOM_VER) && (vers_mana.Id_Vers == ID_VER) && (vers_mana.Pcb_Vers == PCB_VER))	
	{
		FCT_WriteBuf[10] = 0x00;	//版本号正确,向eeprom地址A+9写0
	}
	else
	{
		err_tmp = 1;
//		FCT_WriteBuf[10] = 0x01;	//版本号错误,向eeprom地址A+9写1
		FCT_WriteBuf[10] = 0x00;
	}
	FCT_WriteBuf[10] = 0x00;

	
	/* 功率板版本号的读取并判断 */
	for(VerReadCnt = 1; VerReadCnt <= 8; VerReadCnt++)
	{
		if(VerReadCnt <= CHANNEL_USE)
		{		
			/* 功率板版本号判断 */
			if(vers_mana.Pow_Ver[VerReadCnt - 1] == PB_VER)	//功率板版本号判断
			{
				FCT_WriteBuf[10 + VerReadCnt] = 0x00;	//版本号正确,向eeprom地址(A+9+VerReadCnt)写0
			}
			else
			{
				err_tmp = 1;
//				FCT_WriteBuf[10 + VerReadCnt] = 0x01;	//版本号错误,向eeprom地址(A+9+VerReadCnt)写1
				FCT_WriteBuf[10 + VerReadCnt] = 0x00;
			}

		}
		else
		{
			FCT_WriteBuf[10 + VerReadCnt] = 0x00;		//版本号正确,向eeprom地址(A+9+VerReadCnt)写0

		}
	}
	
	return err_tmp;
}

/**********************************************************************************************************
**	函 数 名 : WholeMachine_FCTLampPlate()
**	功能说明 : 整机FCT测试步骤 - 灯板测试
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTLampPlate()
{
	uint8_t err_tmp = 0;
	uint8_t xLedWriteDataRed[2]   = {0xFF,0x00}; 
	uint8_t xLedWriteDataGreen[2] = {0x00,0xFF}; 
	uint8_t xLedWriteDataOFF[2]   = {0xFF,0xFF};
	uint8_t LedWriteRes_1 = 2, LedWriteRes_2 = 2, LedWriteRes_3 = 2, LedWriteRes_4 = 2;	
	
//	/* 灯板版本号的读取并判断 */
//	if(vers_mana.LED_Ver == LED_VER)
//	{
//		//版本正确 向eeprom地址A+18写0
//		FCT_WriteBuf[19] = 0x00;
//	}
//	else //向eeprom地址A+18写0
//	{
//		FCT_WriteBuf[19] = 0x01;
//		
//		err_tmp = 1;
//	}
	
	FCT_WriteBuf[19] = 0x00;

	
	/* 灯板自检逻辑:红灯亮1S灭1S,绿灯亮1S灭1S循环1次 */
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
		FCT_WriteBuf[20] = 0x00;	//灯板检测正确,向eeprom地址(A+19)写0
	}
	else
	{
		err_tmp = 1;
		FCT_WriteBuf[20] = 0x01;	//灯板检测错误,向eeprom地址(A+19)写1
	}

	
	return err_tmp;
}

/**********************************************************************************************************
**	函 数 名 : WholeMachine_FCTFan()
**	功能说明 : 整机FCT测试步骤 - 风扇控制
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTFan()
{
	uint8_t err_tmp = 0;
	uint8_t FanTestCnt = 0, FanCnt1 = 0, FanCnt2 = 0;
	uint8_t FanDiv = 1;
	
	/* 30%PWM控制 */
	FanPwmSet(0.3);
	/* IO检测故障状态读取 */
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

	/* 100%PWM控制 */
	FanPwmSet(1);
	/* IO检测故障状态读取 */
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
		FCT_WriteBuf[21] = 0x00;	//风扇控制正常,向eeprom地址A+20写0
	}
	else
	{
		err_tmp = 1;
		FCT_WriteBuf[21] = 0x01;	//风扇控制异常,向eeprom地址A+20写1
	}

	
	return err_tmp;
}

/**********************************************************************************************************
**	函 数 名 : WholeMachine_FCTChargeDisc()
**	功能说明 : 整机FCT测试步骤 - 温度检测
**	形    参 : 
**	返 回 值 : 
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
	
	/*	读取控制板温度和功率模块温度，判断所有温度的极差值不超过10℃		*/

	float ArrMax = Tmp_Arr[0],ArrMin = Tmp_Arr[0];
	
	for(i = 0;i < (CHANNEL_USE + 1);i++)
	{
		if(Tmp_Arr[i] > ArrMax)
		{
			ArrMax = Tmp_Arr[i];	//取温度最大值
		}
	}
	
	for(i = 0;i < (CHANNEL_USE + 1);i++)
	{
		if(Tmp_Arr[i] < ArrMin)
		{
			ArrMin = Tmp_Arr[i];	//取温度最小值
		}
	}
	
	if(ArrMax - ArrMin <= 10)	//向eeprom地址A+21写0
	{
		FCT_WriteBuf[22] = 0x00;
	}
	else	//向eeprom地址A+21写1
	{
		FCT_WriteBuf[22] = 0x01;
		
		err_tmp = 1;
	}

	
	return err_tmp;
}


/**********************************************************************************************************
**	函 数 名 : WholeMachine_FCTChargeDisc()
**	功能说明 : 整机FCT测试步骤 - 放充电测试 2/4/6/8 充电 1/3/5/7 放电
**	形    参 : 
**	返 回 值 : 
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
**	函 数 名 : WholeMachine_FCTDischCharge()
**	功能说明 : 整机FCT测试步骤 - 放充电测试 2/4/6/8 充电 1/3/5/7 放电
**	形    参 : 
**	返 回 值 : 
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
**	函 数 名 : WholeMachine_FCTVsenseRapid()
**	功能说明 : 整机FCT测试步骤 - 配置所有通道为Vsense快速环、依次设置模块1CV4V2A启动，延时2S
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTVsenseRapidLoop()
{
	uint8_t err_tmp = 0;
	uint8_t WriteBaseAddr = 38;	//测试结果写入基地址偏移
	uint8_t Arlam_1 = 0x00,Arlam_2 = 0x00,Arlam_3 = 0x00,Arlam_4 = 0x00,Arlam_5 = 0x00,Arlam_6 = 0x00,Arlam_7 = 0x00;
	uint8_t i;
	//设置8个模块为Vsense快速环 -> LOOP 置高,MLoop则需置低
	for( i = 0;i < CHANNEL_USE;i++)
	{
		Protect_Flag_Internal[i] = 0;
		IP_Alarm_Flag[i] = 0;
		Disable_Mloop(i); //有反向
		osDelay(10);
	}

	for( i = 0; i < CHANNEL_USE; i += 2)
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
		{
			Chg_Dis_On_Cfg((1<<i), BUCK_MODE, V_SENSE, 4.0, 2.0);
		}
	}
	osDelay(2000);	
	
	/* 设置模块1 CV4V2A启动，延时2S，判断端口电压、sense电压、探针电压是否在4V正负0.5V范围内，电流是否在50mA内，如果不在，读取是否有硬件故障，读具体硬件故障 */
	for( i = 0; i < CHANNEL_USE; i += 2)	//(CHANNEL_USE - 1) 作测试用，因为7模块是坏的
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
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
				if(Tier2.sampleCal.Iout[i] > 1.0f)	//电流不合格
				{
					Arlam_1 = 0x01;
				}
				if(3.5f > Tier2.sampleCal.Vmod[i] || Tier2.sampleCal.Vmod[i] > 4.5f)	//端口电压不合格
				{
					Arlam_2 = 0x02;
				}
				
				if(IP_Alarm_Flag[i] & IntelProtect_OCP)	//硬件OCP bit2 = 1
				{
					Arlam_3 = 0x04;
				}
				else if(IP_Alarm_Flag[i] & IntelProtect_OVP) //硬件OVP bit3 = 1 
				{
					Arlam_4 = 0x08;
				}
				else if(IP_Alarm_Flag[i] & IntelProtect_OTP)	//硬件OTP bit4 = 1
				{
					Arlam_5 = 0x10;
				}
				
				if(3.5f > Tier2.sampleCal.Vsense[i] || Tier2.sampleCal.Vsense[i] > 4.5f)	//Vsense电压不合格
				{
					Arlam_6 = 0x20;
				}
				if(3.5f > Tier2.sampleCal.Vtz[i] || Tier2.sampleCal.Vtz[i] > 4.5f)	//Vtz电压不合格
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
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
		{
			Chg_Dis_Off_Cfg(1<<i);
		}
		Protect_Flag_Internal[i] = 0;
		IP_Alarm_Flag[i] = 0;
	}	
	osDelay(100);

	for( i = 1; i < CHANNEL_USE; i += 2)
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
		{
			Chg_Dis_On_Cfg((1<<i), BUCK_MODE, V_SENSE, 4.0, 2.0);
		}
	}
	osDelay(2000);		
	
	/* 设置模块1 CV4V2A启动，延时2S，判断端口电压、sense电压、探针电压是否在4V正负0.5V范围内，电流是否在50mA内，如果不在，读取是否有硬件故障，读具体硬件故障 */
	for( i = 1; i < CHANNEL_USE; i += 2)	//(CHANNEL_USE - 1) 作测试用，因为7模块是坏的
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
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
				if(Tier2.sampleCal.Iout[i] > 1.0f)	//电流不合格
				{
					Arlam_1 = 0x01;
				}
				if(3.5f > Tier2.sampleCal.Vmod[i] || Tier2.sampleCal.Vmod[i] > 4.5f)	//端口电压不合格
				{
					Arlam_2 = 0x02;
				}
				
				if(IP_Alarm_Flag[i] & IntelProtect_OCP)	//硬件OCP bit2 = 1
				{
					Arlam_3 = 0x04;
				}
				else if(IP_Alarm_Flag[i] & IntelProtect_OVP) //硬件OVP bit3 = 1 
				{
					Arlam_4 = 0x08;
				}
				else if(IP_Alarm_Flag[i] & IntelProtect_OTP)	//硬件OTP bit4 = 1
				{
					Arlam_5 = 0x10;
				}
				
				if(3.5f > Tier2.sampleCal.Vsense[i] || Tier2.sampleCal.Vsense[i] > 4.5f)	//Vsense电压不合格
				{
					Arlam_6 = 0x20;
				}
				if(3.5f > Tier2.sampleCal.Vtz[i] || Tier2.sampleCal.Vtz[i] > 4.5f)	//Vtz电压不合格
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
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
		{
			Chg_Dis_Off_Cfg(1<<i);
		}
	}	
	osDelay(100);	
	
	return err_tmp;
}

/**********************************************************************************************************
**	函 数 名 : WholeMachine_FCTVsenseSlow()
**	功能说明 : 整机FCT测试步骤 - 配置所有通道为Vsense慢速环、依次设置模块1CV4V2A启动，延时2S
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
uint8_t WholeMachine_FCTVsenseSlowLoop()
{
	uint8_t WriteBaseAddr = 46;
	uint8_t err_tmp = 0;
	uint8_t i;
	
	//设置8个模块为Vsense慢速环 -> LOOP 置低
	for( i = 0;i < CHANNEL_USE;i++)
	{
		Protect_Flag_Internal[i] = 0;
		IP_Alarm_Flag[i] = 0;
		Enable_Mloop(i);//有反向
		osDelay(10);
	}
	
	for( i = 0; i < CHANNEL_USE; i += 2)
	{
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
		{
			Chg_Dis_On_Cfg((1<<i), BUCK_MODE, V_SENSE, 4.0, 2.0);
		}
	}
	osDelay(3000);	
		
	//设置模块 CV4V2A启动，延时2S，判断端口电压、sense电压、探针电压是否在4V正负0.5V范围内，如果不在，读取是否有硬件故障，如果触发硬件OVP告警，则OK，否则NG
	for( i = 0; i < CHANNEL_USE; i +=2 )
	{	
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
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
				if( (IP_Alarm_Flag[i] & IntelProtect_OVP) || (IP_Alarm_Flag[i] & IntelProtect_LOV) ) //硬件OVP bit3 = 1
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
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
		{
			Chg_Dis_Off_Cfg(1<<i);
		}
	}	
	osDelay(100);
	
	for( i = 1; i < CHANNEL_USE; i += 2)
	{
		Protect_Flag_Internal[i] = 0;
		IP_Alarm_Flag[i] = 0;
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
		{
			Chg_Dis_On_Cfg((1<<i), BUCK_MODE, V_SENSE, 4.0, 2.0);
		}
	}
	osDelay(3000);	
		
	//设置模块 CV4V2A启动，延时2S，判断端口电压、sense电压、探针电压是否在4V正负0.5V范围内，如果不在，读取是否有硬件故障，如果触发硬件OVP告警，则OK，否则NG
	for( i = 1; i < CHANNEL_USE; i +=2 )
	{	
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
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
				if( (IP_Alarm_Flag[i] & IntelProtect_OVP) || (IP_Alarm_Flag[i] & IntelProtect_LOV) )	//硬件OVP bit3 = 1
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
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
		{
			Chg_Dis_Off_Cfg(1<<i);
		}
	}	
	osDelay(100);	
	return err_tmp;
}

/**********************************************************************************************************
**	函 数 名 : WholeMachine_FCTEnd()
**	功能说明 : 整机FCT测试步骤 - 结束FCT
**	形    参 : 
**	返 回 值 : 
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
	
	for(Test_Cnt = 0; Test_Cnt < FCT_Test_Num; Test_Cnt++)	//NG项数
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

	if(NG_Cnt == 0x00)	//FCT完成且无NG项
	{
		Pca9555_RegWrite(I2C_LED, PCA9555_LED_ADDR, LEDON_G);	//亮绿灯
	}
	else
	{
		Pca9555_RegWrite(I2C_LED, PCA9555_LED_ADDR, LEDON_R);	//亮红灯
	}
	
	return 0;
}

/**********************************************************************************************************
**	函 数 名 : WholeMachine_FCTChaDisFunc()
**	功能说明 : 整机FCT充放电测试功能函数 - 充放电测试及故障判断
**			   1/3/5/7 充电 2/4/6/8 放电;	2/4/6/8 充电 1/3/5/7放电
**	形    参 : CHx1 - 充电通道   CHx2 - 放电通道
**	返 回 值 : 
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
		if(IOTestInfo.MainPowerSignal[i + 1] == 0x00)	//在位信号判断 1-8 0位为主控板在位信号，0在位，1不在位
		{
			Chg_Dis_Off_Cfg(1<<i);
		}
		osDelay(5);
	}
	
	if(CHx1%2 == 1)			//1/3/5/7 充电; 2/4/6/8 放电
	{
		WriteBaseAddr = 21;	//测试结果写入起始偏移地址
	}
	else if(CHx1%2 == 0)	//2/4/6/8 充电; 1/3/5/7 放电
	{
		WriteBaseAddr = 28;	//测试结果写入起始偏移地址
	}
	
	/* 1充2放30A测试:设置模块1CV4V40A,设置模块2DCC1V30A,延时1S,判断端口电压是否在4V正负0.5V范围内,电流是否在30A正负2A内,如果不在,读取是否有硬件故障 */
	
	/* 在位信号判断 1-8 0位为主控板在位信号 */
	if((IOTestInfo.MainPowerSignal[CHx1] || IOTestInfo.MainPowerSignal[CHx2]) != 0x00)
	{
		return 1;	//功率板不在位则不执行
	}
	Chg_Dis_On_Cfg(1 << (CHx2 - 1), BOOST_MODE, V_MOD, DISC_VOLT1 , DISC_CUR1 );	//CHx2 1V30A放电
	osDelay(1000);
	Chg_Dis_On_Cfg(1 << (CHx1 - 1), BUCK_MODE , V_MOD, CHARG_VOLT1, CHARG_CUR1);	//CHx1 4V40A充电
	osDelay(1500);
	
	if( ((CHARG_VOLT1 - 0.5) < Tier2.sampleCal.Vmod[CHx1 - 1]) && \
		(Tier2.sampleCal.Vmod[CHx1 - 1] < (CHARG_VOLT1 + 0.5)) && \
		((DISC_CUR1 - 2) < Tier2.sampleCal.Iout[CHx1 - 1])     && \
		(Tier2.sampleCal.Iout[CHx1 - 1] < (DISC_CUR1 + 2)) )
	{
		FCT_WriteBuf[WriteBaseAddr + 1 + CHx1] = 0x00;	//电流电压在规定范围内,向eeprom地址A+21写0
	}
	else
	{
		err_tmp = 2;
		
		//不在规定范围内则需定位故障在哪
		if( ((DISC_CUR1 - 2) > Tier2.sampleCal.Iout[CHx1 - 1]) || \
			(Tier2.sampleCal.Iout[CHx1 - 1] > (DISC_CUR1 + 2)) )		//电流不合格 bit0 = 1
		{
			Arlam_1 = 0x01;
		}
		if( ((CHARG_VOLT1 - 0.5) > Tier2.sampleCal.Vmod[CHx1 -1]) || \
			(Tier2.sampleCal.Vmod[CHx1 - 1] > (CHARG_VOLT1 + 0.5)) )	//电压不合格 bit1 = 1
		{
			Arlam_2 = 0x02;
		}
		if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OCP)					//硬件OCP bit2 = 1
		{
			Arlam_3 = 0x04;
		}
		else if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OVP)			//硬件OVP bit3 = 1 
		{
			Arlam_4 = 0x08;
		}
		else if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OTP)			//硬件OTP bit4 = 1
		{
			Arlam_5 = 0x10;
		}
		
		FCT_WriteBuf[WriteBaseAddr + 1 + CHx1] = Arlam_1 + Arlam_2 + Arlam_3 + Arlam_4 + Arlam_5;
	}

	
	/* 1充2放120A测试:设置模块1CV4V130A,设置模块2DCC1V120A,延时1S,判断端口电压是否在4V正负0.5V范围内,电流是否在120A正负5A内,如果不在读取是否有硬件故障 */
	Chg_Dis_On_Cfg(1 << (CHx1 - 1), BUCK_MODE , V_MOD, CHARG_VOLT1, CHARG_CUR2);	//CHx1 4V130A充电
	osDelay(300);
	Chg_Dis_On_Cfg(1 << (CHx2 - 1), BOOST_MODE, V_MOD, DISC_VOLT1 , DISC_CUR2 );	//CHx2 1V120A放电
	osDelay(1500);
	
	if( ((CHARG_VOLT1 - 0.5) < Tier2.sampleCal.Vmod[CHx1 - 1]) && \
		(Tier2.sampleCal.Vmod[CHx1 - 1] < (CHARG_VOLT1 + 0.5)) && \
		((DISC_CUR2 - 5) < Tier2.sampleCal.Iout[CHx1 - 1]) && \
		(Tier2.sampleCal.Iout[CHx1 - 1] < (DISC_CUR2 + 5)) )	//电流电压在规定范围内,向eeprom地址A+22写0
	{
		FCT_WriteBuf[WriteBaseAddr + 2 + CHx1] = 0x00;
	}
	else
	{
		err_tmp = 3;
		
		//不在规定范围内则需定位故障在哪
		if( ((DISC_CUR2 - 5) > Tier2.sampleCal.Iout[CHx1 - 1]) || \
			(Tier2.sampleCal.Iout[CHx1 - 1] > (DISC_CUR2 + 5)) )		//电流不合格 bit0 = 1
		{
			Arlam_1 = 0x01;
		}
		if( ((CHARG_VOLT1 - 0.5) > Tier2.sampleCal.Vmod[CHx1 -1]) || \
			(Tier2.sampleCal.Vmod[CHx1 - 1] > (CHARG_VOLT1 + 0.5)) )	//电压不合格 bit1 = 1
		{
			Arlam_2 = 0x02;
		}
		if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OCP)					//硬件OCP bit2 = 1
		{
			Arlam_3 = 0x04;
		}
		else if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OVP)			//硬件OVP bit3 = 1 
		{
			Arlam_4 = 0x08;
		}
		else if(IP_Alarm_Flag[CHx1 - 1] & IntelProtect_OTP)			//硬件OTP bit4 = 1
		{
			Arlam_5 = 0x10;
		}
		
		FCT_WriteBuf[CHx1 + 21 + 1] = Arlam_1 + Arlam_2 + Arlam_3 + Arlam_4 + Arlam_5;
	}

	
	//充放电关闭
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
