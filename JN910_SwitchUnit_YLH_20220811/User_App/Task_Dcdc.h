/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Dcdc.h
** Author      : Huang Cheng
** Date        : 2022-01-20
** Description :
**********************************************************************************************************/

#ifndef _TASK_DCDC_H_
#define _TASK_DCDC_H_

#ifdef __cplusplus
extern "C" {
#endif
    
/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#define PROTECT_LIMIT_VOL       0x01      //输出限压保护
#define PROTECT_LIMIT_CURT      0x02      //输出限流保护

#define TRIGGER_IND_CURT        0x01      //电感电流过流保护触发
#define TRIGGER_OUT_VOL         0x02      //输出限压保护触发
#define TRIGGER_OUT_CURT        0x03      //输出限流保护触发
#define TRIGGER_BUS_HIGH        0x04      //输入BUS高故障触发
#define TRIGGER_BUS_LOW         0x05      //输入BUS低故障触发
#define TRIGGER_HARDWARE        0x06      //硬件故障触发
#define TRIGGER_FAN             0x07      //风扇故障触发
#define TRIGGER_WDG_RESET       0x08      //看门狗复位故障触发
#define TRIGGER_REV_CONNET      0x09      //反接保护触发

#define RECORD_RUN_STATUS       0x01      //运行状态、输出模式、故障
#define RECORD_SENSE_VOL        0x02      //Sense电压
#define RECORD_OUT_CURT         0x03      //输出电流
#define RECORD_OUT_VOL          0x04      //输出电压
#define RECORD_BUS_VOL          0x05      //BUS电压
#define RECORD_IL1_CURT         0x06      //IL1电流
#define RECORD_IL2_CURT         0x07      //IL2电流
#define RECORD_TEMP_VAL         0x08      //温度
#define RECORD_DUTY_CYCLE1      0x09      //占空比1
#define RECORD_DUTY_CYCLE2      0x0A      //占空比2

#define CTRL_RUN_MODE           0x01      //运行模式
#define CTRL_RATE_VOL           0x02      //额定输出电压
#define CTRL_RATE_CURT          0x03      //额定输出电流
#define CTRL_RUN_CC_MODE        0x01      //CC模式
#define CTRL_RUN_CV_MODE        0x02      //CV模式
#define CTRL_RUN_DCC_MODE       0x03      //DCC模式
#define CTRL_RUN_DCV_MODE       0x04      //DCV模式
#define RUN_STATUS              0x01      //运行状态
#define STOP_STATUS             0x02      //停止状态
#define CLEAR_FAULT             0x03      //清除故障

#define CLIB_SENSE_VOL_K        0x01      //Sense电压校准系数K
#define CLIB_SENSE_VOL_B        0x02      //Sense电压校准系数B
#define CLIB_PORT_VOL_K         0x03      //端口电压校准系数K
#define CLIB_PORT_VOL_B         0x04      //端口电压校准系数B
#define CLIB_CHARG_CURT_K       0x05      //充电输出电压校准系数K
#define CLIB_CHARG_CURT_B       0x06      //充电输出电压校准系数B
#define CLIB_DISCHARG_CURT_K    0x07      //放电输出电压校准系数K
#define CLIB_DISCHARG_CURT_B    0x08      //放电输出电压校准系数B
#define CLIB_PROBE_VOL_K        0x09      //探针电压校准系数K
#define CLIB_PROBE_VOL_B        0x0A      //探针电压校准系数B
#define CLIB_BAT_ADC_VAL        0x01      //电池电压采样AD累加值
#define CLIB_PORT_ADC_VAL       0x02      //端口电压采样AD累加值
#define CLIB_CURT_ADC_VAL       0x03      //输出电流采样AD累加值
#define CLIB_TEMP_ADC_VAL       0x04      //环境温度采样AD累加值
#define CLIB_PROBE_ADC_VAL      0x05      //探针电压采样AD累加值

#define SYSTEM_BASE_INF         0x01      //系统基本信息
#define SYSTEM_ALARM_INF        0x02      //系统告警信息
#define SYSTEM_ALARM_CLR        0x01      //系统告警清除
#define SYSTEM_HEART_BEAT       0x02      //心跳包
#define SYSTEM_MODE_SET         0x03      //系统模式设置
#define SYSTEM_JUMP_BOOT        0x01      //跳转boot模式
#define SYSTEM_REQUEST_BOOT     0x02      //查询boot标志
#define SYSTEM_ERASE_FLASH      0x03      //擦除flash
#define SYSTEM_WRITE_FLASH      0x04      //写flash
#define SYSTEM_JUMP_APP         0x05      //跳转APP模式

#define INVLID_SUB_CMD          0xFF      //无效子命令码
#define CHANNEL_NUM             0x04      //DCDC总通道数

#define RECV_MSG_ID             0x00000001

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
#pragma pack(1)
typedef struct
{
	uint8_t ChFlag;           //通道标志
	uint8_t MainCmd;          //主命令码
	uint8_t SubCmd;           //子命令码
	uint8_t Data[4];          //数据
	uint8_t Crc;              //校验和
}DCDC_FrameType;
#pragma pack()

typedef enum
{
	ProtectSet    = 0x01,     //保护设置
	ProtectSetInq = 0x02,     //保护设置查询
	RecordDataInq = 0x04,     //记录数据查询
	CtrlParamSet  = 0x05,     //控制参数设置
	RunStaSet     = 0x06,     //运行状态设置
	CtrlParamInq  = 0x07,     //控制参数查询
	ClibParamSet  = 0x08,     //校准参数设置
	ClibParamInq  = 0x09,     //校准参数查询
	AdcDataRead   = 0x0A,     //AD数据读取
	SystemInq     = 0x0B,     //系统查询
	SystemSet     = 0x0C,     //系统设置
	SystemUpdate  = 0x0D,     //系统升级设置
}DCDC_SetFrameType;

typedef enum
{
	ProtectSetRep    = 0x81,   //保护设置响应
	ProtectInqRep    = 0x82,   //保护设置查询回复
	ProtectTrig      = 0x83,   //保护触发
	RecordDataInqRep = 0x84,   //记录数据查询回复
	CtrlParamSetRep  = 0x85,   //控制参数设置响应
	RunStaSetRep     = 0x86,   //运行状态设置响应
	CtrlParamInqRep  = 0x87,   //控制参数查询回复
	ClibParamSetRep  = 0x88,   //校准参数设置响应
	ClibParamInqRep  = 0x89,   //校准参数查询回复
	AdcDataReadRep   = 0x8A,   //AD数据读取回复
	SystemInqRep     = 0x8B,   //系统查询回复
	SystemSetRep     = 0x8C,   //系统设置响应
}DCDC_RepFrameType;

typedef enum
{
//	SET_OK    = 0x00,
//	SET_NOTOK = !SET_OK,       //新版本协议
	SET_NOTOK = 0x00,
	SET_OK    = !SET_NOTOK,    //旧版本协议
}DCDC_SetRepSta;

typedef enum
{
	FrameOk = 0x00,
	CmdErr,
	CrcErr,
}DCDC_FrameValidType;

#pragma pack(1)
typedef struct
{
	float    OvpVal[4];      //输出限压保护值，四个通道
	float    OcpVal[4];      //输出限流保护值，四个通道
	uint16_t ProtectAlarm[4];//保护告警，按bit来处理，四个通道
	uint8_t  RunSta[4];      //运行状态，四个通道
	uint8_t  RunMode[4];     //运行模式，四个通道
	uint16_t Fault[4];       //故障记录，四个通道
	float    SenseVol[4];    //Sense电压，四个通道
	float    OutCurt[4];     //输出电流，四个通道
	float    OutVol[4];      //输出电压，四个通道
	float    BusVol[4];      //BUS电压，四个通道
	float    IL1Curt[4];     //IL1电流，四个通道
	float    IL2Curt[4];     //IL2电流，四个通道
	float    TempVal;        //温度
	float    DutyCycle1[4];  //占空比1，四个通道
	float    DutyCycle2[4];  //占空比2，四个通道
	float    RatedVol[4];    //额定电压，四个通道
	float    RatedCurt[4];   //额定电流，四个通道
	float    SenseK[4];      //Sense校准系数K，四个通道
	float    SenseB[4];      //Sense校准系数B，四个通道
	float    PortVolK[4];    //端口电压校准系数K，四个通道
	float    PortVolB[4];    //端口电压校准系数B，四个通道
	float    ChCurtK[4];     //充电电流校准系数K，四个通道
	float    ChCurtB[4];     //充电电流校准系数B，四个通道
	float    DisChCurtK[4];  //放电电流校准系数K，四个通道
	float    DisChCurtB[4];  //放电电流校准系数B，四个通道
	float    ProbeVolK[4];   //探针电压校准系数K，四个通道
	float    ProbeVolB[4];   //探针电压校准系数B，四个通道
	uint16_t BatVolAdc[4];   //电池电压AD累加值，四个通道
	uint16_t PortVolAdc[4];  //端口电压AD累加值，四个通道 
	uint16_t TempAdc[4];     //温度AD累加值，四个通道
	uint16_t ProbeVolAdc[4]; //探针电压AD累加值，四个通道 
	uint8_t  VerNum[4];      //软件版本号
	
}DCDC_ParamType;
#pragma pack()

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/
extern DCDC_FrameType DcdcRecvBuffer;
extern DCDC_FrameType DcdcSendFrame;
extern DCDC_FrameType DcdcRecvFrame;
extern DCDC_ParamType DcdcParam;

extern uint32_t DcdcCanId;  //DCDC CAN通信ID
extern uint32_t MaskCanId;
extern uint8_t dcdc_err_flag;

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/
void DcdcRunModeSet(uint8_t ChNum, uint8_t RunMode);
void DcdcAdcDataRead(uint8_t ChNum, uint8_t ParaType);
void DcdcSystemSet(uint8_t ParaType);
void DcdcSystemUpdate(uint8_t ParaType);
void DcdcClibParamSet(uint8_t ChNum, uint8_t ParaType, float ClibPara);
void DcdcProtectSet(uint8_t ChNum, uint8_t ProtectType, float ProtectPara);
void DcdcRunStaSet(uint8_t ChNum, uint8_t RunSta);
void DcdcCtrlParaSet(uint8_t ChNum, uint8_t ParaType, float RunPara);
void DcdcParaInq(uint8_t ChNum, uint8_t MainCmdType, uint8_t SubCmdType);
void DcdcRecvHandle(void);
extern void AppTaskDcdc(void *argument);
    
#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
