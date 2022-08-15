/*
*********************************************************************************************************
*
*	模块名称 : Net_Others
*	文件名称 : Net_Others.h
*	版    本 : V1.0
*	说    明 :
*	修改记录 :
*	版本号  日期         作者       说明
*	V1.0    2020-03-10   ZYL        正式发布
*
*********************************************************************************************************
*/

#ifndef _Net_Others_H_
#define _Net_Others_H_

#include "stdint.h"

#define TIMEOUT_SOCKET_WORK        750   //工作用socket无心跳包超时断开连接时间，20S


//charger status
#define cOFF                        0
#define cON                         1
#define cComplete                   2
#define cNotComplete                3
#define cPowerFailNotComplete       4

//结构体定义


//联合体定义


//对外部文件开放全局变量声明
extern uint16_t Num_Cabinet;
extern uint16_t Addr_Module;

//对外部文件开放函数声明
extern void Socket_Work(void);
extern uint16_t sCRC16N(uint8_t *Buffer,uint16_t uwlen);
//extern void sTcpipNullCommand(void);
//extern void sLifeSingalReload(void);
//extern void sSysTimeSetCommand(void);
//extern void sSysVersionInquireCommand(void);
//extern void sSysCabNumInquireCommand(void);
//extern void sSetChargerStatusCommand(void);
//extern void sSetParallelModeCommand(void);
//extern void sFaultClearSetCommand(void);
//extern void sFaultInquireCommand(void);
//extern void sSetPreChargerCommand(void);
//extern void sInquirePowerOffEnCommand(void);
//extern void sSetPowerOffModeCommand(void);

void Socket_Receive(void);
void Socket_Sent(uint16_t len);

#endif

//File end
