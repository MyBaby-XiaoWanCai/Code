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
**                                          头文件引用区
**********************************************************************************************************/
#include "Task_Dcdc.h"

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void AppTaskDcdc(void *argument);

/**********************************************************************************************************
**                                           函数声明区
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
**                                         全局变量定义区
**********************************************************************************************************/
DCDC_FrameType DcdcRecvBuffer = {0};
DCDC_FrameType DcdcRecvFrame = {0};
DCDC_FrameType DcdcSendFrame = {0};
DCDC_ParamType DcdcParam = {0};

uint8_t AppDataBuffer[1024] = {0};
uint8_t dcdc_err_flag = 0;

uint32_t DcdcCanId = 0;  //DCDC CAN通信ID
uint32_t MaskCanId = 0;  //CAN过滤码

//处理函数跳转表
static void (*DcdcRepHandle[])(DCDC_FrameType*) = 
{
	DcdcSetCmdRep,          //保护设置响应处理函数
	DcdcProtectInqRep,      //保护设置查询回复处理函数
	DcdcProtectTrig,        //保护触发处理函数
	DcdcRecordDataInqRep,   //记录数据查询回复处理函数
	DcdcSetCmdRep,          //控制参数设置响应处理函数
	DcdcSetCmdRep,          //运行状态设置响应处理函数
	DcdcCtrlParamInqRep,    //控制参数查询回复处理函数
	DcdcSetCmdRep,          //校准参数设置响应处理函数
	DcdcClibParamInqRep,    //校准参数查询回复处理函数
	DcdcAdcDataReadRep,     //AD数据读取回复处理函数
	DcdcSystemInqRep,       //系统查询回复处理函数
	DcdcSetCmdRep,          //系统设置响应处理函数
};

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/**********************************************************************************************************
**  函 数 名 : CheckSum()
**  功能说明 : 校验和函数
**  形    参 : 无
**  返 回 值 : 无
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
**  函 数 名 : Bit2ChNum()
**  功能说明 : Bit转换通道数
**  形    参 : *DcdcRxFrame - 接收数据帧
**  返 回 值 : ChNum - 通道数
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
**  函 数 名 : DcdcParaInq()
**  功能说明 : DCDC参数查询函数
**  形    参 : ChNum - 通道数，MainCmdType - 主命令码类型，SubCmdType - 子命令码类型
**  返 回 值 : 无
**  说    明 : 所有的查询函数都用此函数
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
**  函 数 名 : DcdcProtectSet()
**  功能说明 : DCDC保护设置函数
**  形    参 : ChNum - 通道数，ProtectType - 保护类型，ProtectPara - 保护值
**  返 回 值 : 无
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
**  函 数 名 : DcdcRunModeSet()
**  功能说明 : DCDC运行模式设置函数
**  形    参 : ChNum - 通道数，RunMode - 模式
**  返 回 值 : 无
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
	
	memset(DcdcSendFrame.Data, 0x00, 4);       //模式设置
	DcdcSendFrame.Data[0] = RunMode;
	
	DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
}

/**********************************************************************************************************
**  函 数 名 : DcdcCtrlParaSet()
**  功能说明 : DCDC控制参数设置函数
**  形    参 : ChNum - 通道数，ParaType - 参数类型，RunPara - 运行参数
**  返 回 值 : 无
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
	
	//运行模式参数类型为整型
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
**  函 数 名 : DcdcRunStaSet()
**  功能说明 : DCDC运行状态设置函数
**  形    参 : ChNum - 通道数，RunSta - 运行状态，1运行，2停止
**  返 回 值 : 无
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
**  函 数 名 : DcdcClibParamSet()
**  功能说明 : DCDC校准参数设置函数
**  形    参 : ChNum - 通道数，ParaType - 参数类型, ClibPara - 校准参数
**  返 回 值 : 无
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
**  函 数 名 : DcdcAdcDataRead()
**  功能说明 : DCDC校准参数设置函数
**  形    参 : ChNum - 通道数，ParaType - 参数类型
**  返 回 值 : 无
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
**  函 数 名 : DcdcSystemSet()
**  功能说明 : DCDC系统设置函数
**  形    参 : ParaType - 参数类型
**  返 回 值 : 无
**********************************************************************************************************/
void DcdcSystemSet(uint8_t ParaType)
{
	if(INVLID_SUB_CMD == ParaType)
	{
		return;
	}
	
	DcdcSendFrame.ChFlag = 0x00;          //此处不关心通道
	DcdcSendFrame.MainCmd = SystemSet;
	DcdcSendFrame.SubCmd = ParaType;
	memset(DcdcSendFrame.Data, 0x00, 4);
	DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
}

/**********************************************************************************************************
**  函 数 名 : DcdcSystemUpdate()
**  功能说明 : DCDC系统升级函数
**  形    参 : ParaType - 参数类型
**  返 回 值 : 无
**********************************************************************************************************/
void DcdcSystemUpdate(uint8_t ParaType)
{
	if(INVLID_SUB_CMD == ParaType)
	{
		return;
	}
	
	//写DCDC的flash，需要拆包和组包
	if(SYSTEM_WRITE_FLASH == ParaType)
	{
		//do something;
	}
	else
	{
		DcdcSendFrame.ChFlag = 0x00;          //此处不关心通道
		DcdcSendFrame.MainCmd = SystemUpdate;
		DcdcSendFrame.SubCmd = ParaType;
		memset(DcdcSendFrame.Data, 0x00, 4);
		DcdcSendFrame.Crc = CheckSum(&DcdcSendFrame.ChFlag, 0x07);
	}
}

/**********************************************************************************************************
**  函 数 名 : DcdcSetCmdRep()
**  功能说明 : DCDC设置命令响应函数
**  形    参 : *DcdcRxFrame - 接收数据帧
**  返 回 值 : 无
**  说    明 : 所有的响应函数都用此函数
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
**  函 数 名 : DcdcProtectInqRep()
**  功能说明 : DCDC保护参数查询回复函数
**  形    参 : *DcdcRxFrame - 接收数据帧
**  返 回 值 : 无
**********************************************************************************************************/
static void DcdcProtectInqRep(DCDC_FrameType *DcdcRxFrame)
{
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	
	//通道数为无效值
	if(0x00 == CurtCh)
	{
		return;
	}
	
	switch(DcdcRxFrame->SubCmd)
	{
		case PROTECT_LIMIT_VOL:
		{
			//拷贝输出限压保护的值
			memcpy(&DcdcParam.OvpVal[CurtCh-1], DcdcRxFrame->Data, 4);
			break;
		}
		case PROTECT_LIMIT_CURT:
		{
			//拷贝输出限流保护的值
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
**  函 数 名 : DcdcProtectTrig()
**  功能说明 : DCDC保护触发函数
**  形    参 : *DcdcRxFrame - 接收数据帧
**  返 回 值 : 无
**********************************************************************************************************/
static void DcdcProtectTrig(DCDC_FrameType *DcdcRxFrame)
{
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	
	//通道数为无效值
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
				//故障置位
				DcdcParam.ProtectAlarm[CurtCh-1] |= 0x01 << (DcdcRxFrame->SubCmd-1);
			}
			else
			{
				//故障清除
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
**  函 数 名 : DcdcRecordDataInqRep()
**  功能说明 : DCDC记录参数查询回复响应函数
**  形    参 : *DcdcRxFrame - 接收数据帧
**  返 回 值 : 无
**********************************************************************************************************/
static void DcdcRecordDataInqRep(DCDC_FrameType *DcdcRxFrame)
{
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	
	//通道数为无效值
	if(0x00 == CurtCh)
	{
		return;
	}
	
	switch(DcdcRxFrame->SubCmd)
	{
		case RECORD_RUN_STATUS:
		{
			//拷贝记录数据运行状态
			DcdcParam.RunSta[CurtCh-1] = DcdcRxFrame->Data[0];
			DcdcParam.RunMode[CurtCh-1] = DcdcRxFrame->Data[1];
			memcpy(&DcdcParam.Fault[CurtCh-1], &DcdcRxFrame->Data[2], 2);
			break;
		}
		case RECORD_SENSE_VOL:
		{
			//拷贝sense电压
			memcpy(&DcdcParam.SenseVol[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case RECORD_OUT_CURT:
		{
			//拷贝输出电流
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
			//拷贝输出电压
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
			//拷贝BUS电压
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
			//拷贝IL1电感电流
			memcpy(&DcdcParam.IL1Curt[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case RECORD_IL2_CURT:
		{
			//拷贝IL2电感电流
			memcpy(&DcdcParam.IL2Curt[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case RECORD_TEMP_VAL:
		{
			//拷贝温度值
			memcpy(&DcdcParam.TempVal, &DcdcRxFrame->Data[0], 4);
			break;
		}
		case RECORD_DUTY_CYCLE1:
		{
			//拷贝占空比1
			memcpy(&DcdcParam.DutyCycle1[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case RECORD_DUTY_CYCLE2:
		{
			//拷贝占空比2
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
**  函 数 名 : DcdcRunStaSetRep()
**  功能说明 : DCDC控制参数查询回复响应函数
**  形    参 : *DcdcRxFrame - 接收数据帧
**  返 回 值 : 无
**********************************************************************************************************/
static void DcdcCtrlParamInqRep(DCDC_FrameType *DcdcRxFrame)
{
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	
	//通道数为无效值
	if(0x00 == CurtCh)
	{
		return;
	}
	
	switch(DcdcRxFrame->SubCmd)
	{
		case CTRL_RUN_MODE:
		{
			//拷贝运行模式
			memcpy(&DcdcParam.RunMode[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case CTRL_RATE_VOL:
		{
			//拷贝额定电压
			memcpy(&DcdcParam.RatedVol[CurtCh-1], &DcdcRxFrame->Data[0], 4);
			break;
		}
		case CTRL_RATE_CURT:
		{
			//拷贝额定电流
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
**  函 数 名 : DcdcClibParamInqRep()
**  功能说明 : DCDC校准KB系数查询回复响应函数
**  形    参 : *DcdcRxFrame - 接收数据帧
**  返 回 值 : 无
**********************************************************************************************************/
static void DcdcClibParamInqRep(DCDC_FrameType *DcdcRxFrame)
{
	//获取校准第一个参数的地址，依次赋值
	float *ClibPara = (float*)&DcdcParam.SenseK[0];
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	uint8_t ClibParaIndex = 0x00;
	
	//通道数为无效值
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
			ClibParaIndex = (DcdcRxFrame->SubCmd-1)*CHANNEL_NUM + (CurtCh-1);  //不同类型不同通道的KB值
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
**  函 数 名 : DcdcAdcDataReadRep()
**  功能说明 : DCDC采样AD累加值查询回复响应函数
**  形    参 : *DcdcRxFrame - 接收数据帧
**  返 回 值 : 无
**********************************************************************************************************/
static void DcdcAdcDataReadRep(DCDC_FrameType *DcdcRxFrame)
{
	//获取AD累加值第一个参数的地址，依次赋值
	uint16_t *AdcVal = (uint16_t*)&DcdcParam.BatVolAdc;
	uint8_t CurtCh = Bit2ChNum(DcdcRxFrame);
	uint8_t ClibParaIndex = 0x00;
	
	//通道数为无效值
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
			ClibParaIndex = (DcdcRxFrame->SubCmd-1)*CHANNEL_NUM + (CurtCh-1); //不同类型不同通道的AD值
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
**  函 数 名 : DcdcSystemInqRep()
**  功能说明 : DCDC系统参数查询回复响应函数
**  形    参 : *DcdcRxFrame - 接收数据帧
**  返 回 值 : 无
**********************************************************************************************************/
static void DcdcSystemInqRep(DCDC_FrameType *DcdcRxFrame)
{
	switch(DcdcRxFrame->SubCmd)
	{
		case SYSTEM_BASE_INF:
		{
			//拷贝软件版本号
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
**  函 数 名 : IsFrameValid()
**  功能说明 : 接收DCDC数据有效校验函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static DCDC_FrameValidType IsFrameValid(void)
{
	DCDC_FrameValidType FrameSta = FrameOk;
	
	//主、子命令码错误
	if((INVLID_SUB_CMD==DcdcRecvBuffer.MainCmd) || (INVLID_SUB_CMD==DcdcRecvBuffer.SubCmd))
	{
		return (FrameSta=CmdErr);
	}
	
	//CRC错误
	if(CheckSum(&DcdcRecvBuffer.ChFlag, 0x07)!=DcdcRecvBuffer.Crc)
	{
		return (FrameSta=CrcErr);
	}
	
	//数据帧无错误，则将接收缓冲区数据拷贝到待处理的缓存中
	memcpy(&DcdcRecvFrame.ChFlag, &DcdcRecvBuffer.ChFlag, sizeof(DCDC_FrameType)/sizeof(uint8_t));
	
	return FrameSta;
}

/**********************************************************************************************************
**  函 数 名 : DcdcRecvHandle()
**  功能说明 : DCDC数据处理函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void DcdcRecvHandle(void)
{
	uint8_t FuncIndex = 0;
	
	//数据帧无效
	if((FrameOk != IsFrameValid()) && (DcdcRecvFrame.MainCmd <= 0x81))
	{
		return;
	}
	
	//获取函数下标，并执行对应函数
	FuncIndex = DcdcRecvFrame.MainCmd - 0x81;
	DcdcRepHandle[FuncIndex](&DcdcRecvFrame);
}

/**********************************************************************************************************
**  函 数 名 : AppTaskDcdc()
**  功能说明 : DCDC通讯解析处理任务
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AppTaskDcdc(void *argument)
{
	osStatus_t OsSemRt;
	
    while(1)
    {	 
		//获取信号量的状态
		OsSemRt = osSemaphoreAcquire(SemaDcdcCan, osWaitForever);
		
		if(osOK == OsSemRt)
		{
			//处理DCDC CAN接收的消息
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
