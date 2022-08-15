/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: WorkStep_Flow.h
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef APP_INCLUDE_FLOW_WORKSTEP_H_
#define APP_INCLUDE_FLOW_WORKSTEP_H_

#define NUM_WORKSTEP_MAX        256                         //最大的工步数
#define NUM_CYCLE_MAX			3		//循环最大嵌套数

typedef struct struct_StepEnd
{
    Uint16 End_EN;                       //使能
     int32 End_Value;                     //工步结束参数
    Uint16 End_Logic;                    //工步结束逻辑
    Uint16 End_Result;                   //工步结束后转移方向
    Uint16 End_Jump_Index;                  //
    Uint16 Cnt_Check;
    Uint16 Num_Mask;					//工步转换后屏蔽工步结束检测时间
    Uint16 Cnt_Mask;
     int32 Now_Value;
    Uint16 IsStepEnd;                   //当前判断项工步结束结果
    Uint16 priority;
} struct_StepEnd;

typedef struct struct_Record_Time
{
    Uint16 EN;
    Uint32 Delta_Time;           //时间差记录数据
    Uint32 Cnt_Time;             //时间差记录数据计数器
    Uint16 IsRecord;            //是否到记录数据时间,后台任务使用完后清除
} struct_Record_Time;

typedef struct struct_Record_Condition
{
    Uint16 EN;
     int32 Value_Now;
     int32 Delta_Now;
     int32 Delta_Ref_Step;
     int32 Delta_Ref_Global;
     int32 Value_Last;
    Uint16 IsRecord;
} struct_Record_Condition;


//结构体定义
typedef struct struct_WorkStep_Data
{
    Uint16 Index;                       //当前执行的工步在存储器中的索引号
    Uint16 Type;                        //当前执行的工步的类型
    Uint16 Type_Last;                   //上一工步执行的工步的类型
     int32 Rated_Vol;                   //由存储的工步数据得到的目标电压
     int32 Rated_Cur;                   //由存储的工步数据得到的目标电流
    Uint16 Length;                      //工步数据长度
    Uint32 Formula_NumberH;
    Uint32 Formula_NumberL;

    int32 Pressure;                    //负压工艺需求的负压值的绝对值
    Uint16 Vol_Interval_FreshFlag;

    struct_Record_Time      Record_Time;                 //时间差数据记录
    struct_Record_Condition Record_Vol;                  //电压差数据记录
    struct_Record_Condition Record_Cur;                  //电流差数据记录

    struct_StepEnd StepEnd_Time;        //时间截止条件
    struct_StepEnd StepEnd_Vol;         //电压截止条件
    struct_StepEnd StepEnd_Cur;         //电流截止条件
    struct_StepEnd StepEnd_Ah;          //容量截止条件

    //工步相关保护
    struct_Protect_Value Prot_Step_CH_Over_Cur;
    struct_Protect_Value Prot_Step_CH_Over_Ah;
    struct_Protect_Value Prot_Step_CH_Over_Vol_Bat;
    struct_Protect_Value Prot_Step_CH_Over_Vol_Out;

    struct_Protect_Value Prot_Step_DIS_Over_Cur;
    struct_Protect_Value Prot_Step_DIS_Over_Ah;
    struct_Protect_Value Prot_Step_DIS_Under_Vol_Bat;
    struct_Protect_Value Prot_Step_DIS_Under_Vol_Out;

    struct_Protect_Value Prot_Step_Bat_Over_Temp;
    struct_Protect_Value Prot_Step_Bat_Under_Temp;

    struct_Protect_Delta Prot_Step_Cur_Delta_Pos;
    struct_Protect_Delta Prot_Step_Vol_Delta_Pos;
    struct_Protect_Delta Prot_Step_Cur_Delta_Neg;
    struct_Protect_Delta Prot_Step_Vol_Delta_Neg;

    struct_Protect_Range Prot_Step_Vol_Time;
    struct_Protect_Vol_Change Prot_Step_Vol_Change_Time;
    struct_Protect_Range Prot_Step_BatShort;

    struct_Protect_Value_Diff  Prot_Step_Vol_Diff_Over;
    struct_Protect_Value_Diff  Prot_Step_Cur_Diff_Over;
    struct_Protect_Interval    Prot_Step_Vol_Interval_Pos;
    struct_Protect_Interval    Prot_Step_Vol_Interval_Neg;


} struct_WorkStep_Data;


typedef struct struct_WorkStep_Service
{
    //工步运行状态机输入接口
    Uint16 Req_CMD;                     //!=0,表示请求动作命令
    Uint16 Req_CMD_OnRunning;           //工步运行中请求命令
    Uint16 Req_WorkStep_Index;          //请求运行的工步号
    Uint16 Req_SingleStep;              //请求单步运行
    Uint16 Req_PowerFailContine;        //掉电接续运行请求
    Uint16 Req_Cur_PreCharge;           //预充电请求冲电流
    Uint16 Req_Time_PreCharge;          //预充电时间
    Uint16 Req_PowerFailSave;           //掉电保存请求
    Uint16 Req_Stop_Socket_Err;			//心跳包超时单独请求，避免超时关机命令被覆盖

     int32 Cur_Limited_Set;                 //最大设定电流限制值

    //工步实时状态和数据
    Uint16 State;                       //工步流程控制状态机状态
    Uint16 Cnt_Delay;                   //模块内部延时计数用
    Uint16 Flag_SingleStep;             //单步运行标志
    Uint16 Step_Cnt;                    //工艺流程启动后累计执行的工步数
    Uint16 Step_State;                  //工步执行状态
    Uint32 Step_Time;                   //工步执行时间
    Uint32 Run_TimeTotal;                   //工步执行总时间
    Uint32 Step_Ah;                     //当前工步累计安时数
    Uint32 Step_Wh;                     //当前工步累计瓦时数
    Uint32 Total_Time;                  //工艺执行时间执行时间
     int32 Total_Ah;                    //工艺流程启动后累计安时数
    Uint32 Total_Wh;                    //工艺流程启动后累计瓦时数
     int32 Res_Contact;                 //接触电阻
     int32 Res_Loop;                    //输出回路电阻？
    Uint16 Flag_FlowEnd_Once;           //进入工艺结束状态后，上报一次数据用
    Uint16 Flag_Pause_Once;             //进入暂停状态后，上报一次数据标志
    Uint16 Flag_FlowStop_Once;          //进入工艺停止状态后，上报一次数据用
    //Uint16 TimeOut_Stop;              //超时停机

    Uint32 CC_Ah;                       //CCtoCV工步中CC阶段累计充电安时数
    Uint32 CCdivCV_Ah;                  //充电恒流比
    Uint32 DCC_Ah;                      //DCCtoDCV工步中CC阶段累计充电安时数
    Uint32 DCCdivDCV_Ah;                //放电恒流比

    Uint32 Remain_Ah;                   //Ah累计用
    Uint32 Remain_Wh;                   //Wh累计用
} struct_WorkStep_Service;

typedef struct struct_WorkStep_PowerFail
{
    Uint16 Step_State_Now;                  //异常掉电时工步执行状态
    Uint16 Step_State_EEPROM;                  //异常掉电时工步执行状态
    Uint16 Step_Index_EEPROM;                  //异常掉电时正在运行的工步号
    Uint32 Step_Time_EEPROM;                   //异常掉电时工步执行时间
    Uint32 Step_Ah_EEPROM;                     //异常掉电时工步累计安时数
    Uint32 Step_Wh_EEPROM;                     //异常掉电时工步累计瓦时数
     int32 Vol_T1_EEPROM;
} struct_WorkStep_PowerFail;

typedef struct struct_Cycle_Stack
{
	Uint16 Index_Cycle;					//循环序号
	Uint16 Num_Cycle;					//循环需要执行的次数
	Uint16 Cnt_Cycle;					//循环已经执行完成的次数
	Uint16 Start_Cycle;					//当前循环起始工步号
	Uint16 End_Cycle;                   //当前循环结束工步号
	Uint16 EndLogic_Cycle;              //循环结束后的逻辑
} struct_Cycle_Stack;

typedef struct struct_WorkStep_Cycle
{
	Uint16 Now_Cycle;					//循环嵌套次数，0-无循环，1-第1层循环
	struct_Cycle_Stack Cycle_Stack[NUM_CYCLE_MAX+1];	//工步循环嵌套用数据
} struct_WorkStep_Cycle;

//Step_State,工步完成状态
#define cComplete                   2
#define cNotComplete                3

//Req_CMD定义
#define WORKSTEP_RUN                 4       //debug 请求工步运行
#define WORKSTEP_PAUSE               3       //debug 请求工步运行
#define WORKSTEP_STOP                2       //debug 请求工步运行
#define WORKSTEP_PRECHARGE           1       //预充电请求

//-------------------运行状态设置类----------------------
#define cStopStatus                 0x00
#define cPauseStatus                0x01
#define cWorkStepRunStatus          0x02
#define cFaultStatus                0x03
#define cStepComplete               0x04
#define cSingalStepRunStatus        0x05
#define cStepChangeStatus           0x06
#define cStepContactCheck           0x07
#define cStepStandby                0x08
#define cStepFault                  0x09
#define cStepStart                  0x0A
#define cStepNegPressureNext        0x0B
#define cStatusErr                  0xFF


//State状态定义
#define STEP_STRUCT_INIT        0       //结构体数据初始化状态
#define STEP_STANDBY            1       //待机状态
#define STEP_REQ_READ           12      //请求读取数据
#define STEP_WAIT_DATA          2       //等待Flash数据读出状态
#define STEP_DATA_ANALYSE       3       //工步数据解析状态
#define STEP_END_CHECK          4       //对工步结束条件预检查，满足结束条件的工步直接跳到STEP_STEPEND
#define STEP_STANDBY2RUN        5       //待机状态转换到运行状态部分数据初始化
#define STEP_RUN                6       //工步处于运行状态，可以接受停止及暂停命令
#define STEP_PAUSE              7       //暂停状态
#define STEP_PAUSE2RUN          8       //暂停状态转换到运行状态前进行部分数据初始化
#define STEP_STEPEND            9       //工步运行结束处理，循环结束等
#define STEP_FLOW_END           10      //工艺流程结束状态，
#define STEP_CONTACT_CHECK      11      //接触检查状态
#define STEP_CONTACT_END        14      //接触检查结束
#define STEP_FLOW_STOP          13      //工艺流程未正常完成时停机进入此状态

//数据记录结构体
typedef struct struct_Image
{
    unsigned char ubChargerStatus;
    unsigned long udwSuperRunTime;
      signed long wBatSenseAVoltNew_UP;
      signed long dwOutPutCurrentNew_UP;
      signed long udwChargerCapacityNew;
    unsigned int  uwBatteryDegreeNew;
    unsigned int  uwWorkStepNum;
    unsigned char ubWorkStepName;
    unsigned int  uwStepCycleStartIndex;
    unsigned int  uwWorkStepTotalNum;
    unsigned long udwAH_CCdivCV;
    unsigned long udwAH_DCCdivDCV;
      signed long dwBatEnergy;
      signed long wOutPutVoltNew_UP;
    unsigned int  uwChannel1NTCDegreeNew;
      signed long Vol_Contact;//极耳电压
    unsigned int  Res_Contact;//接触电阻
    unsigned int  Res_Loop;//接触电阻
        long long DateTime;
      signed char NegPressure;
} struct_Image;

typedef struct struct_Image_TX
{
    Uint16 Req_TX;
    struct_Image Record;
} struct_Image_TX;

typedef struct struct_Filter_Elimin
{
    int32 Data_In;              //当前数据输入
    int32 Data_Last1;           //上一次数据输入
    int32 Data_Last2;           //上上一次数据输入
    int32 Data_Last3;           //上上上一次数据输入
    int32 Delta_Now;            //本次数据变化
    int32 Delta_Last;           //上次数据变化
    int32 Delta_Last1;          //上上次数据变化
    int32 Elimin_Abs_Max;       //数据跳变剔除幅度门限上限
    int32 Elimin_Abs_Min;       //数据跳变剔除幅度门限下限
    int32 Data_Out;             //数据输出
    Uint32 Data_Update_cnt;    //数据上抛或更新计数
} struct_Filter_Elimin;
typedef struct struct_Filter_Workstep
{
	unsigned char Type;
	int32  Rated_Cur;
	int32  Cur_End;
	unsigned int Voltage_Trend;//0稳定，1上升，2下降。初始化为0
}struct_Filter_Workstep;

typedef struct struct_Filter_Median
{
    int32 Data_In;              //当前数据输入
    int32 Data[5];              //
    Uint16 Cnt;                 //数据累计次数
    int32 Data_Out;             //数据输出
} struct_Filter_Median;

typedef struct struct_Rate_Cur_Compute
{
    int32 Data_In;              //当前电流数据输入

    int32 Data_Last;            //前一次电流数据

    int32 Data_Cur[11];        //0.1S间隔历史电流数据
   Uint16 Cnt_Delay;

    int32 Rate_Cur_Out;
    int32 Rate_Cur_Now;
    int32 Rate_Cur_1S;
    int32 Rate_Cur_Filter;
   Uint16 Cnt_Error;
   Uint16 Cnt_Error_Reset;
    int32 Rate_Max_Expect;
    int32 Rate_Min_Expect;
} struct_Rate_Cur_Compute;


//联合体定义


//对外部文件开放全局变量声明
extern long long DateTime;
extern struct_WorkStep_Data         WorkStep_Data;
extern struct_WorkStep_Service      WorkStep_Service;
extern struct_WorkStep_Cycle        WorkStep_Cycle;

extern struct_Image_TX Image_Protect;       //故障保护时保存数据镜像
extern struct_Image_TX Image_StepEnd;       //工步结束数据镜像
extern struct_Image_TX Image_UpData;        //需上报数据记录镜像

extern struct_WorkStep_PowerFail WorkStep_PowerFail;
extern Uint16 Middle_Cnt;

extern struct_Filter_Median        Median_Vol_Sense;
extern struct_Filter_Median        Median_Vol_Out;
extern struct_Filter_Median        Median_Cur_Total;

extern struct_Rate_Cur_Compute     Rate_Cur_Compute;

//对外部文件开放函数声明
extern void WorkStep_Flow(void);
extern void Process_Average_Analog(void);
extern Uint32 Char2Uint32(unsigned char *Ptr,Uint16 Offset);
extern  int32 Char2int32(unsigned char *Ptr,Uint16 Offset);
extern Uint16 Char2Uint16(unsigned char *Ptr,Uint16 Offset);
extern  int16 Char2int16(unsigned char *Ptr,Uint16 Offset);
extern void Init_WorkStep_Protect(void);
extern void Init_Global_Protect(void);
extern void WorkStep_End_Check(struct_StepEnd *Ptr);
extern void Ah_Accumulate(void);
extern void Wh_Accumulate(void);
extern void Disable_All_WorkStep_Protect(void);
extern void Enable_All_WorkStep_Protect(void);
extern void Res_Contact_Compute(void);
extern void Res_Loop_Compute(void);
extern void SaveDataToRecord(struct_Image *Ptr);
extern Uint16 sBatTemperatureRealCal(int32 dwBatTempSampleAvg);
extern INT16U sTemperature1SampleAvgCal_B3380(INT16U uwChannel1NTCSampleTemp);
extern INT16U sTemperature1SampleAvgCal(INT16U uwChannel1NTCSampleTemp);
extern void WorkStep_Service_Init(void);
extern void Cali_PT_5V(void);
extern void CopyImage(struct_Image *Source,struct_Image *Dest);
extern int32 Get_Middle_Value(void);
extern void Updata_Mutation_Check(void);
extern void StepEnd_TimeSet(Uint16 offset);
extern void StepEnd_VolSet(Uint16 offset);
extern void StepEnd_CurSet(Uint16 offset);
extern void StepEnd_AhSet(Uint16 offset);
extern void Function_Rate_Cur_Compute(void);


#endif /* APP_INCLUDE_FLOW_WORKSTEP_H_ */

//File end
