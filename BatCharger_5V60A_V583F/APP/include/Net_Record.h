/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_Record.h
// Created on: 2020-4-8
//     Author: LuoAnJiang
/**************************************************************************************/
#ifndef APP_INCLUDE_NET_RECORD_H_
#define APP_INCLUDE_NET_RECORD_H_




//结构体定义


//联合体定义


//对外部文件开放全局变量声明
extern struct_Fault_Code       Fault_Code;

//对外部文件开放函数声明
extern void sSetRecondConditionCommand(void);
extern void sInquireRecondDataCommand(void);
extern void sReplyRecondDataCommand(struct_Image *Ptr,Uint16 CH_TX);//记录数据上返

#endif /* APP_INCLUDE_NET_RECORD_H_ */
