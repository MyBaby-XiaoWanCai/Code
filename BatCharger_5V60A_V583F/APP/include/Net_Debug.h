/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_Debug.h
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/


#ifndef APP_INCLUDE_NET_DEBUG_H_
#define APP_INCLUDE_NET_DEBUG_H_


//结构体定义


//联合体定义


//对外部文件开放全局变量声明
extern int32 Simulate_Data0;
extern int32 Simulate_Data1;//debug
extern int32 Simulate_Data2;
extern int32 Simulate_Data3;
extern int32 Simulate_Data4;
extern int32 Simulate_Data5;
extern int32 Simulate_Data6;
extern int32 Simulate_Data7;

extern Uint16 Flag_Trigger;
extern Uint16 Cnt_Record;
extern Uint16 State_Record_TX;

//对外部文件开放函数声明
extern void Socket_Debug(void);
extern void Record_TX(void);
extern void Init_Record_TX_CAN(void);

#endif /* APP_INCLUDE_NET_DEBUG_H_ */

//File end
