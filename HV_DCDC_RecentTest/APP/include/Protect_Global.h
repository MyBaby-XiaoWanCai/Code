/*
 * Protect_Global.h
 *
 *  Created on: 2021年6月21日
 *      Author: JC
 */

#ifndef APP_INCLUDE_PROTECT_GLOBAL_H_
#define APP_INCLUDE_PROTECT_GLOBAL_H_
#define cStepGreater                0x01
#define cStepLess                   0x02

#define cProtectCCFault             0x00000002
#define cPowerVoltFault             0x00400000

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

typedef struct struct_Protect_Global
{
    struct_Protect_Value CH_Over_Cur;

    struct_Protect_Value CH_Over_Vol_Out;

} struct_Protect_Global;
//结构体定义
typedef struct struct_Fault_Code
{
    Uint32 Err_Global;
    Uint32 Err_Global_Bak;
    Uint32 Err_Step;
    Uint32 Err_Step_Bak;
    Uint32 Alarm;
    Uint32 Alarm_Bak;
    Uint16 Code;
} struct_Fault_Code;
//对外部文件开放全局变量声明
extern struct_Protect_Global Protect_Global[];
extern struct_Fault_Code  Fault_Code[];


#endif /* APP_INCLUDE_PROTECT_GLOBAL_H_ */
