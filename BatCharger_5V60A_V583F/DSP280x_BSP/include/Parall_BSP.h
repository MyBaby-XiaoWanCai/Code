/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Parall_BSP.h
// Created on: 2020-3-19
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef DSP280X_BSP_INCLUDE_PARALL_BSP_H_
#define DSP280X_BSP_INCLUDE_PARALL_BSP_H_

//结构体定义
#define SYMBOL_ID0          0x5C
#define SYMBOL_ID1          0xC5

//ID0数据结构定义
struct struct_ID0_H
{
    Uint32 Add_Master:8;                 //主机地址
    Uint32 Parall_Total:8;               //并机数量
    Uint32 NC:8;                   //请求的工作模式，只限STANDY,CC,DCC,STATIC
    Uint32 NC1:8;                //清除从机故障标志
};

union union_ID0_H
{
    Uint32 All;
    struct struct_ID0_H Bit;
};


struct struct_ID0_L
{
    Uint32 NC:24;                         //控制从机的输出MOS是否开启
    Uint32 Symbol:8;                     //特殊标记符，此字节为特殊字符模块才会进入从机模式
};

union union_ID0_L
{
    Uint32 All;
    struct struct_ID0_L Bit;
};

struct struct_ID0
{
    Uint16 Req_TX;
    Uint16 Flag_First_TX;
    Uint16 Flag_RX;
    union  union_ID0_H ID0_H;
    union  union_ID0_L ID0_L;
};

//ID1数据结构定义

union union_ID1_H
{
    Uint32 All;
    Uint32 Set_Cur_Total;                //从机当前总电流
};


struct struct_ID1_L
{
    Uint32 Req_Mode:8;                  //请求从机工作模式，只限STANDY,CC,CCtoCV,DCC,DCCtoDCV,STATIC
    Uint32 Req_ChangeMode:1;            //请求改变从机工作模式
    Uint32 Req_Refresh_Cur:1;           //请求刷新从机电流设定值
    Uint32 Req_Fault_Clear:1;           //清除从机故障标志
    Uint32 EN_Run:1;                    //控制从机是否工作
    Uint32 EN_MOS_Out:1;                //控制从机的输出MOS是否开启
    Uint32 NC:3;                        //
    Uint32 Now_Mode_Master:8;           //主机当前DCDC的工作状态，从机接收到此信息与自己的状态比较，异常时停机
    Uint32 Symbol:8;                    //特殊标记符，此字节为特殊字符模块才会进入从机模式
};

union union_ID1_L
{
    Uint32 All;
    struct struct_ID1_L Bit;
};

struct struct_ID1
{
    Uint16 Req_TX;
    Uint16 Flag_First_TX;
    Uint16 Flag_RX;
    Uint16 Cnt_TX_Req_Change_Mode;
    union  union_ID1_H ID1_H;
    union  union_ID1_L ID1_L;
};

//ID2~ID5收发数据结构定义
struct struct_IDx_L
{
    Uint32 Fault_Module:16;              //从机故障码
    Uint32 Now_Mode:8;                   //从机当前工作状态
    Uint32 NC1:8;                        //从机当前工作状态
};

union union_IDx_L
{
    Uint32 All;
    struct struct_IDx_L Bit;
};

union union_IDx_H
{
    Uint32 All;
    Uint32 Now_Cur_AB;                //从机当前AB通道总电流
};

struct struct_IDx
{
    Uint16 Req_TX;
    Uint16 Flag_First_TX;
    Uint16 Flag_RX;
    union union_IDx_H IDx_H;
    union union_IDx_L IDx_L;
};

struct struct_IDx1_L
{
    Uint32 Temp_Sink:16;              //从机故障码
    Uint32 Temp_Shunt:16;                   //从机当前工作状态
};

//IDx1收发数据结构定义
union union_IDx1_L
{
    Uint32 All;
    struct struct_IDx1_L IDx1_L;                //未使用
};

union union_IDx1_H
{
    Uint32 All;
    Uint32 Now_Vol_Sense;           //从机1当前Sense电压，作为主机极耳电压用
};

struct struct_IDx1
{
    Uint16 Req_TX;
    Uint16 Flag_First_TX;
    Uint16 Flag_RX;
    union union_IDx1_H IDx_H;
    union union_IDx1_L IDx_L;
};


//从机
typedef struct struct_Parall_Slaver
{
    Uint16 Addr_Master;                 //从机收到的主机地址编号数据
    Uint16 Parall_Total;                //从机收到的总并机数量
    Uint16 Cnt_Period;                  //从机周期发送计数器
    Uint16 Cnt_S_OT_M;                  //从机模式，累计未收到主机CAN数据时间
    struct struct_ID0   ID0_Fr_M;       //从主机接收的ID0数据帧
    struct struct_ID1   ID1_Fr_M;       //从主机接收的ID1数据帧
    struct struct_IDx   IDx_To_M;       //将要发送给主机的数据帧
    struct struct_IDx1	IDx1_To_M;      //将要发送给主机的数据帧
} struct_Parall_Slaver;

typedef struct struct_Parall_Master
{
    Uint16 Cnt_Period;                  //主机周期发送计数器
    Uint16 Cnt_M_OT_S1;                 //主机模式，累计未收到从机4的CAN数据时间
    Uint16 Cnt_M_OT_S2;                 //主机模式，累计未收到从机4的CAN数据时间
    Uint16 Cnt_M_OT_S3;                 //主机模式，累计未收到从机4的CAN数据时间
    Uint16 Cnt_M_OT_S4;                 //主机模式，累计未收到从机4的CAN数据时间
     int32 Now_Cur_Total;               //并机总输出或输入电流
     int32 Now_Cur_S1;                  //从机1总输出或输入电流
     int32 Now_Cur_S2;                  //从机2总输出或输入电流
     int32 Now_Cur_S3;                  //从机3总输出或输入电流
     int32 Now_Cur_S4;                  //从机4总输出或输入电流
     int32 Now_Sense_S1;				//从机1Sense电压
    Uint16 EEPROM_Parall_Mode;			//EEPROM中存储的并机模式
    Uint16 EEPROM_Parall_Num; 			//EEPROM中存储的并机总数量
    Uint16 Cnt_Delay;					//上电后延时启动主机发送CAN命令计数器
    struct struct_ID0   ID0_To_Sx;
    struct struct_ID1   ID1_To_Sx;
    struct struct_IDx   IDx_Fr_S1;
    struct struct_IDx   IDx_Fr_S2;
    struct struct_IDx   IDx_Fr_S3;
    struct struct_IDx   IDx_Fr_S4;
    struct struct_IDx1  IDx1_Fr_S1; 	//从机1发给主机的Sense电压
} struct_Parall_Master;

//联合体定义


//对外部文件开放全局变量声明
extern struct_Parall_Slaver Parall_Slaver;
extern struct_Parall_Master Parall_Master;
extern     struct ECAN_REGS ECanaShadow;

//对外部文件开放函数声明
extern void Init_ID_Single(void);
extern void Init_ID_Master(void);
extern void Init_ID_Slaver(void);
extern void InitCAN_BSP(void);
extern void Parall_Service(void);
extern void Slaver_Abnormal_Check(void);
extern void Read_Data_Parall(void);
extern void Auto_To_Master(void);
extern void Set_By_CAN(void);

#endif /* DSP280X_BSP_INCLUDE_PARALL_BSP_H_ */
