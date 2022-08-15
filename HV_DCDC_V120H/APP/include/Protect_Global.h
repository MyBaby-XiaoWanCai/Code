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


#define cProtectCLFault             0x00000001   //电感电流过流软件保护
#define cProtectCCFault             0x00000002   //输出限流保护
#define cProtectCVFault             0x00000004   //输出限压保护
#define cBusVoltHighFault           0x00000008   //输入BUS高故障
#define cBusVoltLowFault            0x00000010   //输入BUS欠压故障
#define cHardwareError              0x00000020   //硬件故障
#define cFanFault                   0x00000040   //风扇故障
#define cWatchdogFault              0x00000080   //看门狗故障
#define cBatPNFault                 0x00000100   //反接故障
#define cPowerVoltFault             0x00000200   //输出端过压

#define Code_ProtectCL              0x01
#define Code_ProtectCC              0x02
#define Code_ProtectCV              0x03
#define Code_Protect_BusVoltHigh    0x04
#define Code_Protect_BusVoltLow     0x05
#define Code_Protect_HardwareError  0x06
#define Code_Protect_FanFault       0x07
#define Code_Protect_WatchdogFault  0x08
#define Code_Protect_BatPNFault     0x09
#define Code_Protect_PowerVoltFault     0x0A


typedef struct struct_Protect_Value
{
    Uint16 Protect_EN;                  //故障检测使能控制
    Uint16 Num_Change_Mask;             //工步转换后屏蔽时间
    Uint16 Cnt_Change_Mask;             //工步转换后屏蔽时间计数器
    Uint16 Num_Check;                   //故障置位判断次数
    Uint16 Cnt_Check;                   //故障置位判断次数计数器
    Uint16 Equation;                    //比较判断关系
     float32 Now_Value;                   //待比较量当前值
     float32 Ref_Value;                   //比较门限
    Uint32 Mask;                        //故障标志位定义码
    Uint32 *Ptr;                        //需要置位的变量指针
} struct_Protect_Value;

typedef struct struct_Protect_Global
{
    struct_Protect_Value CH_Over_Cur[3];
    struct_Protect_Value CH_Over_Vol;
    struct_Protect_Value CH_Over_Vol_Bat;
    struct_Protect_Value Bat_Reverse_Out;

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
extern void Protect_Value(struct_Protect_Value *Ptr);
extern void Protect_Global_Init(void);
extern void Upload_Fault_Code(void);
extern void Data_Up(void);

#endif /* APP_INCLUDE_PROTECT_GLOBAL_H_ */
