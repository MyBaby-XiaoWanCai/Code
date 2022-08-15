/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_Global.h
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef APP_INCLUDE_NET_GLOBAL_H_
#define APP_INCLUDE_NET_GLOBAL_H_



//结构体定义


//联合体定义


//对外部文件开放全局变量声明


//对外部文件开放函数声明
extern void sSetProtctNACKCommand(void);
extern void sSetProtctACKCommand(void);
extern void sSetGlobalProtctNACKCommand(void);
extern void sSetGlobalProtctACKCommand(void);
extern void sSetProtctCommand(void);
extern void sInquireSetProtctCommand(void);
extern void sSetAllGlobalProtctCommand(void);

#endif /* APP_INCLUDE_NET_GLOBAL_H_ */

//File end
