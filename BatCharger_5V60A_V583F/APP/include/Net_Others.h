/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_Others.h
// Created on: 2020-4-3
//     Author: LuoAnJiang
/**************************************************************************************/


#ifndef APP_INCLUDE_NET_OTHERS_H_
#define APP_INCLUDE_NET_OTHERS_H_

#define TIMEOUT_SOCKET_WORK        750   //工作用socket无心跳包超时断开连接时间，20S


//charger status
#define cOFF                        0
#define cON                         1
#define cComplete                   2
#define cNotComplete                3
#define cPowerFailNotComplete        4

//结构体定义


//联合体定义


//对外部文件开放全局变量声明
extern Uint16 Num_Cabinet;
extern Uint16 Addr_Module;

//对外部文件开放函数声明
extern void Socket_Work(void);
extern Uint16 sCRC16N(unsigned char *Buffer,Uint16 uwlen);
extern void sTcpipNullCommand(void);
extern void sLifeSingalReload(void);
extern void sSysTimeSetCommand(void);
extern void sSysVersionInquireCommand(void);
extern void sSysCabNumInquireCommand(void);
extern void sSetChargerStatusCommand(void);
extern void sSetParallelModeCommand(void);
extern void sFaultClearSetCommand(void);
extern void sFaultInquireCommand(void);
extern void sSetPreChargerCommand(void);
extern void sInquirePowerOffEnCommand(void);
extern void sSetPowerOffModeCommand(void);

#endif /* APP_INCLUDE_NET_OTHERS_H_ */

//File end
