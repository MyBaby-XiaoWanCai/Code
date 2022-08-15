/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : global.h
** Author      : Cai Heng
** Date        : 2021-07-28
** Description : 
**********************************************************************************************************/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "hc32_ddl.h"


/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#define CH_NUM						8

#define RUN_SEG_NUM					3
#define ABT_SEG_NUM					4
#define PRO_SEG_NUM					6


#define CHANNEL_MAX		            8
#ifdef YCJ
#define CHANNEL_CFG		8
#else
#define CHANNEL_CFG		4
#endif
#define CHANNEL_1                   1
#define CHANNEL_2                   2
#define CHANNEL_4                   4
#define CHANNEL_8                   8

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
typedef struct
{
	uint8_t cmd;
	float   dat;
}s_Tier1_ProtectStruct;

typedef struct
{
	uint8_t isEnable;
	float   value;
	float   max;
	float   min;
}s_Tier2_ProtectStruct;

/* 全局保护 */
typedef struct
{
	s_Tier1_ProtectStruct VbatThrH;			// 1. 电池电压上限
	s_Tier1_ProtectStruct VbatThrL;			// 2. 电池电压下限
//	s_Tier1_ProtectStruct VloopDiff;		//  . 回路压差门限
	s_Tier1_ProtectStruct Iovcharge;		// 3. 充电过流门限
	s_Tier1_ProtectStruct Iovdischg;		// 4. 放电过流门限
	s_Tier1_ProtectStruct Cov;				// 5. 工步过容门限
	s_Tier1_ProtectStruct VinitThrH;		// 6. 初始电压上限
	s_Tier1_ProtectStruct VinitThrL;		// 7. 初始电压下限
	s_Tier1_ProtectStruct Tcontact;			// 8. 接触检测时间
	s_Tier1_ProtectStruct Icontact;			// 9. 接触检测电流
	s_Tier1_ProtectStruct VcontactDiff;		//10. 接触检测压差					
}s_Tier1_GlobalProtectStruct;

typedef struct
{
	s_Tier2_ProtectStruct VbatThrH;			// 1. 电池电压上限
	s_Tier2_ProtectStruct VbatThrL;			// 2. 电池电压下限
//	s_Tier2_ProtectStruct VloopDiff;		//  . 回路压差门限
	s_Tier2_ProtectStruct Iovcharge;		// 3. 充电过流门限
	s_Tier2_ProtectStruct Iovdischg;		// 4. 放电过流门限
	s_Tier2_ProtectStruct Cov;				// 5. 工步过容门限
	s_Tier2_ProtectStruct VinitThrH;		// 6. 初始电压上限
	s_Tier2_ProtectStruct VinitThrL;		// 7. 初始电压下限
	s_Tier2_ProtectStruct Tcontact;			// 8. 接触检测时间
	s_Tier2_ProtectStruct Icontact;			// 9. 接触检测电流
	s_Tier2_ProtectStruct VcontactDiff;		//10. 接触检测压差					
}s_Tier2_GlobalProtectStruct;

/* 内部保护设置结构体 */
typedef struct
{
	s_Tier1_ProtectStruct IoutThr;			// 1. 输出电流限值
	s_Tier1_ProtectStruct VbusThrH;			// 2. 母线电压上限
	s_Tier1_ProtectStruct VbusThrL;			// 3. 母线电压下限
	s_Tier1_ProtectStruct IccDiff;			// 4. 恒流段电流误差门限
	s_Tier1_ProtectStruct VcvDiff;			// 5. 恒压段电压误差门限
	s_Tier1_ProtectStruct AmpVdrop;			// 6. 短路电压快速下降阈值
	s_Tier1_ProtectStruct CntVdrop;			// 7. 短路电压快速下降次数
	s_Tier1_ProtectStruct Vovsense;			// 8. sense过压保护门限
	s_Tier1_ProtectStruct Totmod;			// 9. 模块过温保护门限
	s_Tier1_ProtectStruct Totenv;			// 10. 环境过温保护门限
    s_Tier1_ProtectStruct VloopDiff;        // 11.回路压差门限
    s_Tier1_ProtectStruct AmpVup;           // 12.电压快速上升阈值
    s_Tier1_ProtectStruct CntVup;           // 13.电压快速上升次数
    s_Tier1_ProtectStruct AmpCup;           // 14.电流快速上升阈值
    s_Tier1_ProtectStruct CntCup;           // 15.电流快速上升次数
	s_Tier1_ProtectStruct VauxThr;			// 16.辅助电压门限值
    s_Tier1_ProtectStruct SNERR;            // 17.电压异常误差门限值
}s_Tier1_InternalProtectStruct;

typedef struct
{
	s_Tier2_ProtectStruct IoutThr;			// 1. 输出电流限值
	s_Tier2_ProtectStruct VbusThrH;			// 2. 母线电压上限
	s_Tier2_ProtectStruct VbusThrL;			// 3. 母线电压下限
	s_Tier2_ProtectStruct IccDiff;			// 4. 恒流段电流误差门限
	s_Tier2_ProtectStruct VcvDiff;			// 5. 恒压段电压误差门限
	s_Tier2_ProtectStruct AmpVdrop;			// 6. 短路电压快速下降幅值
	s_Tier2_ProtectStruct CntVdrop;			// 7. 短路电压快速下降次数
	s_Tier2_ProtectStruct Vovsense;			// 8. sense过压保护门限
	s_Tier2_ProtectStruct Totmod;			// 9. 模块过温保护门限
	s_Tier2_ProtectStruct Totenv;			//10. 环境过温保护门限
    s_Tier2_ProtectStruct VloopDiff;        //11.回路压差门限
    s_Tier2_ProtectStruct AmpVup;           //12.电压快速上升阈值
    s_Tier2_ProtectStruct CntVup;           //13.电压快速上升次数
    s_Tier2_ProtectStruct AmpCup;           //14.电流快速上升阈值
    s_Tier2_ProtectStruct CntCup;           //15.电流快速上升次数 
    s_Tier2_ProtectStruct VauxThr;			//16.辅助电压门限值
	s_Tier2_ProtectStruct SNERR;            //17.电压异常误差门限值
}s_Tier2_InternalProtectStruct;

#pragma pack(1)
/* 记录数据 */
typedef struct
{
	int64_t  Tstamp;						// 1. 时间戳
	uint8_t  RunSta;						// 2. 运行状态
	uint8_t  StepType;						// 3. 工步类型
	uint16_t StepNumb;						// 4. 工步号
	float    Trun;							// 5. 运行时间(s)
	float    Vbat;							// 6. 电池电压(V)
	float    Iout;							// 7. 输出电流(A)
	float    Vout;							// 8. 输出电压(V)
	float    Vtz;							// 9. 探针电压(V)
	float    Cbat;							//10. 电池容量(Ah)
	float    Ebat;							//11. 电池能量(VAs/J)
	uint16_t ModFlag;						//12. 模式标识
	uint8_t  ErrType;						//13. 故障类型
	uint8_t  AbortType;						//14. 截止类型
}s_RecordStruct;
#pragma pack()

typedef struct channel_record_data_struct {
	float Trun;								// 运行时间      ms-毫秒
	float Vbat;								// 电池电压      V-伏特
	float Iout;								// 输出电流      A-安培
	float Vout;								// 输出电压      V-伏特
	float Cbat;								// 电池容量      Ah-安时
	float Ebat;								// 电池能量      VAh
	float Tbat;								// 电池温度      摄氏度
	float Tmod;								// 模块温度      摄氏度
	float Zcont;							// 接触阻抗
	float Zloop;							// 回路阻抗      
	float Vtz;								// 探针电压
}s_ChannelRecordStruct;

#pragma pack(1)
typedef struct __attribute__ ((__packed__))
{
	uint8_t type;
	float   data;
}s_WorkStepSegmentStruct;;
#pragma pack()

#pragma pack(1)
/* 工步设置查询结构体 */
typedef struct __attribute__ ((__packed__))	//单字节对齐 
{
	uint16_t Numb;							// 工步号
	
	uint8_t  Type;							// 工步类型(工作模式)
	
	s_WorkStepSegmentStruct RunSeg[RUN_SEG_NUM];	
											// 工步运行类型
	s_WorkStepSegmentStruct AbtSeg[ABT_SEG_NUM];	
											// 工步截止类型
	s_WorkStepSegmentStruct ProSeg[PRO_SEG_NUM];	
											// 工步保护类型	
	
    uint8_t  Status;                 		//工步状态（停止，暂停，启动，无效）
}s_WorkStepStruct;
#pragma pack()

#pragma pack(1)
typedef struct
{
	uint16_t StepNumb;						// 工步号
	uint8_t  StepType;						// 工步类型(工作模式)
	float    StepRunVol;					// 工步运行电压值
	float    StepRunCur;					// 工步运行电流值
	float    StepRunRev;					// 工步运行保留值
	float    StepAbtTime;					// 工步截止时间值
	float    StepAbtVol;					// 工步截止电压值
	float    StepAbtCur;					// 工步截止电流值
	float    StepAbtCap;					// 工步截止容量值
	float    StepProVlimitH;				// 工步保护电压上限值
	float    StepProVlimitL;				// 工步保护电压下限值
	float    StepProIov;					// 工步保护过流值
	float    StepProCov;					// 工步保护过容值
	float    StepProVsuperdev;				// 工步保护电压超差值
	float    StepProIsuperdev;				// 工步保护电流超差值
	uint8_t  StepProVlimitH_En;				// 工步保护电压上限使能
	uint8_t  StepProVlimitL_En;	            // 工步保护电压下限使能
	uint8_t  StepProIov_En;		            // 工步保护过流使能
	uint8_t  StepProCov_En;		            // 工步保护过容使能
	uint8_t  StepProVsuperdev_En;	        // 工步保护电压超差使能
	uint8_t  StepProIsuperdev_En;	        // 工步保护电流超差使能
}s_WorkStepAnalyzeStruct;
#pragma pack()

typedef struct
{
	uint16_t StepNumb;						// 工步号
	uint8_t  StepType;						// 工步运行类型
}s_WorkStepControlStruct;

#pragma pack(1)
typedef struct
{
	uint8_t  RunSta;						// 运行状态			
	uint8_t  StepType;						// 工步类型
	uint16_t StepNumb;						// 工步号
	uint32_t Trun;							// 运行时间
	uint32_t Tpro;							// 保护时间
}s_WorkStepRunningStruct;
#pragma pack()

typedef struct
{
	uint8_t Flag;
	uint8_t Statu;
	uint8_t Type;
	float   Current;
	float   Voltage;
}s_CalibrationRunStruct;

/* 校准参数 */
typedef struct
{
	float VsenseSetK_C;						// 1. 充电-电池电压基准设定参数K
	float VsenseSetB_C;						// 2. 充电-电池电压基准设定参数B
	float VmodSetK_C;						// 3. 充电-模块端口电压基准设定参数K
	float VmodSetB_C;						// 4. 充电-模块端口电压基准设定参数B
	float IhSetK_C;							// 5. 充电-高档位电流基准设定参数K
	float IhSetB_C;							// 6. 充电-高档位电流基准设定参数B
	float IlSetK_C;							// 7. 充电-低档位电流基准设定参数K
	float IlSetB_C;							// 8. 充电-低档位电流基准设定参数B
	float VsenseK_C;						// 9. 充电-电池电压采样参数K
	float VsenseB_C;						//10. 充电-电池电压采样参数B
	float VmodK_C;							//11. 充电-模块端口电压采样参数K
	float VmodB_C;							//12. 充电-模块端口电压采样参数B
	float IhK_C;							//13. 充电-输出电流高档位采样参数K
	float IhB_C;							//14. 充电-输出电流高档位采样参数B
	float IlK_C;							//15. 充电-输出电流低档位采样参数K
	float IlB_C;							//16. 充电-输出电流低档位采样参数B
	float VtzK_C;							//17. 充电-探针电压采样参数K
	float VtzB_C;							//18. 充电-探针电压采样参数B
	float VsenseSetK_D;						//19. 放电-电池电压基准设定参数K
	float VsenseSetB_D;						//20. 放电-电池电压基准设定参数B
	float VmodSetK_D;						//21. 放电-模块端口电压基准设定参数K
	float VmodSetB_D;						//22. 放电-模块端口电压基准设定参数B
	float IhSetK_D;							//23. 放电-高档位电流基准设定参数K
	float IhSetB_D;							//24. 放电-高档位电流基准设定参数B
	float IlSetK_D;							//25. 放电-低档位电流基准设定参数K
	float IlSetB_D;							//26. 放电-低档位电流基准设定参数B
	float VsenseK_D;						//27. 放电-电池电压采样参数K
	float VsenseB_D;						//28. 放电-电池电压采样参数B
	float VmodK_D;							//29. 放电-模块端口电压采样参数K
	float VmodB_D;							//30. 放电-模块端口电压采样参数B
	float IhK_D;							//31. 放电-输出电流高档位采样参数K
	float IhB_D;							//32. 放电-输出电流高档位采样参数B
	float IlK_D;							//33. 放电-输出电流低档位采样参数K
	float IlB_D;							//34. 放电-输出电流低档位采样参数B
	float VtzK_D;							//35. 放电-探针电压采样参数K
	float VtzB_D;							//36. 放电-探针电压采样参数B
}s_CalibrateStruct;

/* 采样AD累加值 */
typedef struct
{
	uint32_t VsenseAdSum;					// 1. 电池电压采样AD累加值
	uint32_t VmodAdSum;						// 2. 端口电压采样AD累加值
	uint32_t IoutAdSum;						// 3. 输出电流采样AD累加值
	uint32_t VtzAdSum;						// 4. 探针电压采样AD累加值
}s_AdSumStruct;

/* Debug结构体 */
#pragma pack(1)
typedef struct
{
	uint8_t PortEnable;						
	uint8_t PortDir;
	float Vtemp;
	float Itemp;
}s_DebugSetDatStruct;
#pragma pack()

typedef struct
{
	float VmodDebug;
	float IoutDebug;
	float VsenseDebug;
	float TmodDebug;
	float VtzDebug;
}s_DebugInqDatStruct1;

typedef struct
{
	float Vbus1Debug; 
	float Vbus2Debug; 
	float VauxDebug;
	float TambDebug;
}s_DebugInqDatStruct2;

typedef struct
{	
	s_DebugSetDatStruct  set;				//0x21. 调试参数设置命令/回复
	s_DebugInqDatStruct1 inq1[8];			//0x22. 调试数据查询命令/回复
	s_DebugInqDatStruct2 inq2;
}g_DebugStruct;

typedef enum 
{
Neilsbed_Nofit=0x01, //未压合
Neilsbed_Fitting,    //压合中
Neilsbed_Fitted,      //已压合
}Neilsbed_Status; 

/*系统设置类结构体*/
typedef struct
{
   uint8_t  Device_SN[3];       //SN设置值，用于mac后3字节赋值
   Neilsbed_Status Neilsbed;    //探针压合状态
}s_SystemStruct;

extern s_SystemStruct SystemStatus;
/* 内部结构体变量 */
typedef struct
{
	float Vbus1K;							// 1. Bus1电压参数K
	float Vbus1B;							// 2. Bus1电压参数B
	float Vbus2K;							// 3. Bus2电压参数K
	float Vbus2B;							// 4. Bus2电压参数B
	float VauxK;							// 5. 辅助电压参数K
	float VauxB;							// 6. 辅助电压参数B
	float TambK;							// 7. 环境温度参数K
	float TambB;							// 8. 环境温度参数B
	float TmodK;							// 9. 模块温度参数K
	float TmodB;							//10. 模块温度参数B
}s_CalibrateExStruct;

typedef struct
{
    uint32_t VsenseIns[8];					// 1. Sense电压瞬时(AD)值(每个计算周期清零)
    uint32_t IoutIns[8];					// 2. 输出电流瞬时(AD)值
    uint32_t VtzIns[8];						// 3. 探针电压瞬时(AD)值
    uint32_t VsenseSum[8];					// 4. Sense电压累计(AD)值(每个计算周期不清零)
    uint32_t VmodSum[8];					// 5. 端口电压累计(AD)值
    uint32_t IoutSum[8];					// 6. 输出电流累计(AD)值
    uint32_t VtzSum[8];						// 7. 探针电压累计(AD)值
	float Vsense[8];						// 8. Sense电压，电池电压
	float Iout[8];							// 9. 输出电流
	float Vmod[8];							//10. 端口电压
	float Vtz[8];							//11. 探针电压
	float Tbat[8];							//12. 电池温度
	float Tmod[8];							//13. 模块温度
	float Vbus1;							//14. 母线1直压
	float Vbus2;							//15. 母线2直压
	float Vaux;								//16. 辅助电压
	float Tamb;								//17. 环境温度
}s_SampleCalStruct;

typedef struct
{
	s_Tier1_GlobalProtectStruct   gProt[8];
	s_Tier1_InternalProtectStruct iProt[8];
//	s_RecordStruct                record[8];
	s_WorkStepStruct              workstep[8];
//	s_WorkStepCtrlStruct          worksetpCtrl[8];
	s_CalibrateStruct             calibrate[8];
//	s_AdSumStruct                 adSum[8];
//	s_CalibrateExStruct           calibrateEx;
}s_Tier1_Struct;

typedef struct
{
	s_Tier2_GlobalProtectStruct   gProt[8];
	s_Tier2_InternalProtectStruct iProt[8];	
	s_RecordStruct                record[8];
	s_ChannelRecordStruct         chRecord[8];
	s_WorkStepStruct              workstepCache1[8];
	s_WorkStepStruct              workstepCache2[8];
	s_WorkStepStruct              workstep[8];
	s_WorkStepAnalyzeStruct       workstepAnalyze[8];
	s_WorkStepControlStruct       workstepControl[8];
	s_WorkStepRunningStruct       workstepRunning[8];
	s_CalibrationRunStruct        calibrationRun[8];
	s_CalibrateStruct             calibrate[8];
	s_AdSumStruct                 adSum[8];
	g_DebugStruct                 debug;
	s_CalibrateExStruct           calibrateEx;
	s_SampleCalStruct             sampleCal;
}s_Tier2_Struct;

extern s_Tier1_Struct Tier1;
extern s_Tier2_Struct Tier2;



/*ACDC上抛报警帧*/
typedef struct __attribute__ ((__packed__))	//单字节对齐
{
 uint16_t FramHead;   						//帧头,0x5B5B
 uint16_t Datelen;    						//数据长度=0x09，不包含帧头帧尾
 uint8_t FeatureCode; 						//特征码，相邻帧不同
 uint8_t StateCode;   						//状态码，0：OK
 uint8_t ChanelFlag;  						//通道码：0x01,0x02,0x03
 uint8_t CommandCode; 						//命令码：0x91
 uint8_t ChildCommand;						//子命令码:0x02~0x0B,参考ACDC报警子命令码
 uint16_t CRC16Code;  						//校验字节 
 uint16_t FramTail;   						//帧尾，0xA5A5
}acdc_alarmStruct;

/*报警上抛子命令索引*/
typedef struct
{
  uint8_t ChildCom;
  uint16_t state;
}arr_list;

/*ACDC升级回复结构体*/
typedef struct {
uint32_t ID;								// can ID
uint8_t Date[8];							//can Date
__IO uint8_t processedFlg;					//回复处理标志，1：待处理，0：已处理  
}ACDC_UpdateStrcut;            

/* 内部保护触发类型枚举 */
enum IntelProtect_state    
{
    IntelProtect_OLA = 0x01<<0,				// 1.输出限流保护
    IntelProtect_BOV = 0x01<<1,				// 2.BUS过压
    IntelProtect_BUV = 0x01<<2,				// 3.BUS欠压
    IntelProtect_APA = 0x01<<3,				// 4.电流精度告警
    IntelProtect_UPA = 0x01<<4,				// 5.电压精度告警
    IntelProtect_VDQ = 0x01<<5,				// 6.短路保护告警
    IntelProtect_MOT = 0x01<<6,				// 7.模块过温
    IntelProtect_EOT = 0x01<<7,				// 8.环境过温
    IntelProtect_HPA = 0x01<<8,				// 9.硬件保护
    IntelProtect_FHE = 0x01<<9,				// 10.风扇故障
    IntelProtect_UHE = 0x01<<10,			// 11.CPU异常
    IntelProtect_C1E = 0x01<<11,			// 12.can1异常
    IntelProtect_C2E = 0x01<<12,			// 13.can2异常
    IntelProtect_DAE = 0x01<<13,			// 14.DA异常
    IntelProtect_ADE = 0x01<<14,			// 15.AD异常
    IntelProtect_FLE = 0x01<<15,			// 16.FLASH异常
    IntelProtect_EEE = 0x01<<16,			// 17.Eeprom异常  
    IntelProtect_FRE = 0x01<<17,			// 18.FRAM异常
    IntelProtect_PME = 0x01<<18,			// 19.电位器异常
    IntelProtect_VNN = 0x01<<19,			// 20.电压异常
    IntelProtect_CVP = 0x01<<20,			// 21.回路压差保护
    IntelProtect_VUQ = 0x01<<21,			// 22.电压快速上升
    IntelProtect_CNN = 0x01<<22,			// 23.电流异常
    IntelProtect_CUQ = 0x01<<23,			// 24.电流快速上升 
    IntelProtect_LOV = 0x01<<24,			// 25.辅助电压异常（探针电压大于5.1V）      
    IntelProtect_MAC = 0x01<<25,            // 26.MAC异常（与条码不符）    
};


/* 全局保护触发类型枚举 */
enum GlobalProtect_state
{
    GlobalProtect_OV = 0x01<<0,				//过压保护
    GlobalProtect_UV = 0x01<<1,				//欠压保护
    GlobalProtect_COA = 0x01<<2,			//充电过流保护
    GlobalProtect_DOA = 0x01<<3,			//放电过流保护
    GlobalProtect_OC  = 0x01<<4,			//过容保护
    GlobalProtect_MV = 0x01<<5,				//初始电压上限保护
    GlobalProtect_LV = 0x01<<6,				//初始电压下限保护
    IntelProtect_NONE1 = 0x01<<7,			//未定义
    IntelProtect_NONE2 = 0x01<<8,			//未定义
    IntelProtect_VCV = 0x01<<9,			//接触检测电压变化量
    IntelProtect_OTE = 0x01<<10,			//其他故障
    IntelProtect_INV = 0x01<<11,			//无效 
};

/* 内部保护触发状态-子命令结构体 */
typedef struct
{
   uint8_t childCMD;          //告警子命令码
   uint32_t P_Flg ;           //告警标志位
   uint8_t  Channel;          //当前告警通道
   uint8_t  LastChannel;      //上次告警通道
}CMDProject;

/*保护触发报警帧*/
typedef struct __attribute__ ((__packed__))	//单字节对齐
{
 uint16_t FramHead;							//帧头,0x5B5B
 uint16_t Datelen;							//数据长度=0x09，不包含帧头帧尾
 uint8_t FeatureCode;						//特征码，相邻帧不同
 uint8_t StateCode;							//状态码，0：OK
 uint8_t ChanelFlag;						//通道码：0x01,0x02,0x03
 uint8_t CommandCode;						//命令码：0x91
// uint8_t ChildCommand;						//子命令码:0x02~0x0B,参考ACDC报警子命令码
// uint16_t CRC16Code;						//校验字节 
// uint16_t FramTail;							//帧尾，0xA5A5
}protect_alarmStruct;

typedef struct __attribute__ ((__packed__))	//单字节对齐
{
	uint8_t  RunSta;						// 运行状态			
	uint8_t  StepType;						// 工步类型
	uint16_t StepNumb;						// 工步号
	uint32_t Trun;							// 运行时间
	uint32_t Tpro;							// 保护时间


}WorkStepstore;                                     //工步状态存储结构体

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern uint8_t feed_hardware_watchdog_enable;



/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
extern void RunPer10ms(void);
extern void RunPer100ms(void);
extern void RunPer1000ms(void);


#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
