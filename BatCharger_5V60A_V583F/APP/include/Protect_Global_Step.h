/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Protect_Global_Step.h
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef APP_INCLUDE_PROTECT_GLOBAL_STEP_H_
#define APP_INCLUDE_PROTECT_GLOBAL_STEP_H_


//结构体定义
typedef struct struct_Fault_Code
{
    Uint32 Err_Global;
    Uint32 Err_Global_Bak;
    Uint32 Err_Step;
    Uint32 Err_Step_Bak;
    Uint32 Alarm;
    Uint32 Alarm_Bak;
    Uint16 Cnt_S2M_Fault_Mask;//主机清故障后屏蔽一段时间接收从机的故障码
    Uint16 Code;
    Uint16 Resend_Alarm;
} struct_Fault_Code;

/************************************************************************************
    Global Fault code  1
************************************************************************************/
#define cNoFault                    0x00000000        //系统无故障
#define cProtectCVFault             0x00000001        //充电CV保护
#define cProtectCCFault             0x00000002        //充电CC保护
#define cProtectDCVFault            0x00000004        //放电CV保护
#define cProtectDCCFault            0x00000008        //放电DCC保护
#define cChargeOverCAHFault         0x00000010        //充电过容保护
#define cOPCurrentLimitFault        0x00000020        //输出限流保护
#define cBatPNFault                 0x00000040        //电池反接!
#define cBusVoltHighFault           0x00000080        //输入Bus过压
#define cBatVoltFlucFault           0x00000100        //电池电压波动故障
#define cBatVoltMutatFault          0x00000200        //电池电压突变故障
#define cFanFault                   0x00000400        //风扇故障
#define cTerminalConnectFault       0x00000800        //输出端接插故障
#define cOPShortFault               0x00001000        //输出短路
#define cOPCurrentFlucFault         0x00002000        //输出电流波动故障
#define cOPCurrentMutatFault        0x00004000        //输出电流突变故障
#define cBatShortFault              0x00008000        //电池短路故障
/************************************************************************************
    Fault code  2
************************************************************************************/
#define cBatTempHighFault           0x00010000        //电池过温保护
#define cBatTempLowFault            0x01000000        //电池欠温保护  20191210
#define cImpedanceFault             0x00020000        //接触阻抗
#define cOPCOverDiffFault           0x00040000        //电流超差
#define cBatVOverDiffFault          0x00080000        //电压超差
#define cDisChargeOverAHFault       0x00100000        //放电过容保护
#define cLoopImpedanceFault         0x00200000        //回路阻抗
#define cPowerVoltFault             0x00400000        //回路阻抗
#define cModuleSinkHigh             0x00800000        //模块内部散热器过热保护  20191210
#define cBusLow                     0x02000000        //Bus掉电保护  20191220
#define cCaliDataError				0x04000000		  //校准数据异常
#define cStepDataError				0x08000000		  //工步数据异常
#define cHardwareError				0x10000000		  //硬件故障
#define cOtherError					0x80000000		  //其它故障
#define cTotalOverAHFault           0x20000000        //

/*********************************************************************************
                SStepFaultcode  1
*********************************************************************************/
#define cBatTempUpperLimit          0x00000001
#define cCapacityUpperLimit         0x00000002
#define cCapacityLowerLimit         0x00000004
#define cBatVoltUpperLimit          0x00000008
#define cBatVoltLowerLimit          0x00000010
#define cCurrUpperLimit             0x00000020
#define cCurrLowerLimit             0x00000040
#define cBatVoltPositiveJump        0x00000080
#define cBatVoltNegativeJump        0x00000100
#define cOPCPositiveJump            0x00000200
#define cOPCNegativeJump            0x00000400
#define cBatTempLowerLimt           0x00000800

#define cVol_Time_Error             0x00001000
#define cVol_Change_Time_Error      0x00002000
#define cBatShort                   0x00004000

#define cVol_Diff                   0x00010000
#define cCur_Diff                   0x00020000
#define cVoltIntervalPos            0x00040000
#define cVoltIntervalNeg            0x00080000


#define cStepEnd2Fault              0x10000000
#define cStepEnd3Fault              0x20000000
#define cBatErrorVol_Start          0x40000000
#define cBatErrorContacCheck        0x80000000

//告警标志位定义
#define cBatTempAlarmFault          0x10000000        //电池过温告警
#define cBatTempOverAlarmFault      0x20000000        //电池过温告警
#define cBatTempDownUpperLimit      0x00100000


typedef struct struct_Protect_Value
{
    Uint16 Protect_EN;                  //故障检测使能控制
    Uint16 Num_Change_Mask;             //工步转换后屏蔽时间
    Uint16 Cnt_Change_Mask;             //工步转换后屏蔽时间计数器
    Uint16 Num_Check;                   //故障置位判断次数
    Uint16 Cnt_Check;                   //故障置位判断次数计数器
    Uint16 Equation;                    //比较判断关系
     int32 Now_Value;                   //待比较量当前值
     int32 Ref_Value;                   //比较门限
    Uint32 Mask;                        //故障标志位定义码
    Uint32 *Ptr;                        //需要置位的变量指针
} struct_Protect_Value;


typedef struct struct_Protect_Value_Diff
{
    Uint16 Protect_EN;                  //故障检测使能控制
    Uint16 Num_Change_Mask;             //工步转换后屏蔽时间
    Uint16 Cnt_Change_Mask;             //工步转换后屏蔽时间计数器
    Uint16 Num_Check;                   //故障置位判断次数
    Uint16 Cnt_Check;                   //故障置位判断次数计数器
    Uint16 Equation;                    //比较判断关系
     int32 Now_Value;                   //当前值
     int32 Ref_Value;                   //变量目标值
     int32 Abs_Value_Diff;              //当前值与比较门限差值的绝对值
     int32 Ref_Value_Diff;              //比较门限
    Uint32 Mask;                        //故障标志位定义码
    Uint32 *Ptr;                        //需要置位的变量指针
} struct_Protect_Value_Diff;

typedef struct struct_Protect_Delta
{
    Uint16 Protect_EN;                  //故障检测使能控制
    Uint16 Flag_SynUpData;				//是否同步到上报数据标志
    Uint16 Num_Change_Mask;             //工步转换后屏蔽时间
    Uint16 Cnt_Change_Mask;             //工步转换后屏蔽时间计数器
    Uint16 Num_Check;                   //故障置位判断次数
    Uint16 Cnt_Check;                   //故障置位判断次数计数器
    Uint16 Equation;                    //比较判断关系
     int32 Now_Value;                   //变量当前值
     int32 Last_Value;                  //变量Num_Delta时间前的值
    Uint16 Num_Delta;                   //变量求差间隔时间
    Uint16 Cnt_Delta;                   //变量求差间隔时间计数器
     int32 Now_Delta;                   //Now_Value-Last_Value
     int32 Ref_Delta;               //求差限制值
    Uint32 Mask;                        //故障标志位定义码
    Uint32 *Ptr;                        //需要置位的变量指针
} struct_Protect_Delta;


typedef struct struct_Protect_Interval
{
    Uint16 Protect_EN;                  //故障检测使能控制
    Uint16 Time_Delay;                   //工步转换后比较时间
    Uint16 Equation;                    //比较判断关系
     int32 Last_Value;                   //变量当前值
     int32 Now_Value;                  //变量Num_Delta时间前的值
     int32 Now_Delta;                   //变量求差间隔时间
     int32 Ref_Delta;                  //求差限制值
    Uint32 Mask;                        //故障标志位定义码
    Uint32 *Ptr;                        //需要置位的变量指针
} struct_Protect_Interval;

typedef struct struct_Protect_Range
{
    Uint16 Protect_EN;                  //故障检测使能控制
    Uint32 Time_Delay;                  //工步转换后延迟时间
    Uint16 Equation;                    //比较判断关系
    Uint16 Flag_Check;                  //是否已经检查的标志
     int32 Now_Value;                   //待比较量当前值
     int32 Ref_Value_H;                 //比较门限上限
     int32 Ref_Value_L;                 //比较门限下限
    Uint32 Mask;                        //故障标志位定义码
    Uint32 *Ptr;                        //需要置位的变量指针
} struct_Protect_Range;

typedef struct struct_Protect_Vol_Change
{
    Uint16 Protect_EN;                  //故障检测使能控制
    Uint32 Time_Delay1;                  //工步转换后第一个采样时间点
    Uint32 Time_Delay2;                  //工步转换后第二个采样时间点
    Uint16 Equation;                    //比较判断关系
     int32 Now_Value;                   //待比较量当前值
     int32 Value_t1;                    //第一个采样时间点数值
     int32 Value_t2;                    //第二个采样时间点数值
     int32 Ref_Delta;                   //比较门限
     int32 Abs_Delta;                   //计算得到的数值变化量绝对值
    Uint16 Flag_Check;                  //是否已经检查的标志
    Uint32 Mask;                        //故障标志位定义码
    Uint32 *Ptr;                        //需要置位的变量指针
} struct_Protect_Vol_Change;

typedef struct struct_Protect_ContactCheck
{
     int32 Limit_Vol_H_Start;
     int32 Limit_Vol_L_Start;
     int32 Time;
     int32 Cur;
     int32 Vol_Rise;
     int32 Vol_Start;
     int32 Vol_End;
    Uint16 Protect_EN;
} struct_Protect_ContactCheck;


typedef struct struct_Protect_Global
{
    struct_Protect_Value CH_Over_Cur;
    struct_Protect_Value CH_Over_Ah;
    struct_Protect_Value CH_Over_Vol_Bat;
    struct_Protect_Value CH_Over_Vol_Out;

    struct_Protect_Value DIS_Over_Cur;
    struct_Protect_Value DIS_Over_Ah;
    struct_Protect_Value DIS_Under_Vol_Bat;
    struct_Protect_Value DIS_Under_Vol_Out;

    struct_Protect_Value Bat_Over_Temp;
    struct_Protect_Value Bat_Under_Temp;
    struct_Protect_Value Bat_Temp_Alarm;
    struct_Protect_Value Bat_Temp_Over_Alarm;

    struct_Protect_Value Out_Cur_Over;

    struct_Protect_Value Res_Contact_Over;
    struct_Protect_Value Res_Loop_Over;
    struct_Protect_Value Bat_Reverse_Sense;
    struct_Protect_Value Bat_Reverse_Out;
    struct_Protect_Value Total_Over_Ah;

    struct_Protect_Value_Diff Cur_Diff_Over;
    struct_Protect_Value_Diff Vol_Diff_Over;

    struct_Protect_Delta Cur_Fluct_Over;
    struct_Protect_Delta Vol_Fluct_Over;

    struct_Protect_Delta Cur_Mutat_Over;
    struct_Protect_Delta Vol_Mutat_Over;

    int32  Cur_Max_CV_DCV;

    Uint32 LineRes;
    Uint32 LineVol;
    Uint16 Flag_Protect_Para;
} struct_Protect_Global;

//联合体定义


//对外部文件开放全局变量声明
extern struct_Protect_Global Protect_Global;
extern struct_Protect_ContactCheck Protect_ContactCheck;

//对外部文件开放函数声明
extern void Protect_Value(struct_Protect_Value *Ptr);
extern void Protect_Value_Diff(struct_Protect_Value_Diff *Ptr);
extern void Protect_Mask_Counter(void);
extern void Fault_Mapping(void);
extern void Protect_Check(void);
extern void Protect_Check_Once(void);
extern void Protect_Check_Delta(void);
extern void Protect_Global_Init(void);
extern void Protect_Step_Init(void);
extern void Upload_Fault_Code(void);
extern void Protect_Configure_CC(void);
extern void Protect_Configure_CCtoCV(void);
extern void Protect_Configure_DCC(void);
extern void Protect_Configure_DCCtoDCV(void);
extern void Protect_Configure_STATIC(void);
extern void Protect_Global_Cnt_Reset(void);
extern void Protect_Step_Cnt_Reset(void);
extern void Protect_Configure_CV_PWM(void);
extern void Protect_Configure_DCV(void);
extern void Protect_CH_Imbalance(void);

#endif /* APP_INCLUDE_PROTECT_GLOBAL_STEP_H_ */

//File end
