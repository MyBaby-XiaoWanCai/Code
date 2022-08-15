/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: main.h
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef APP_INCLUDE_MAIN_H_
#define APP_INCLUDE_MAIN_H_
#define FALSE       0
#define TRUE        1
#define BUSY        2

#define ON          1
#define OFF         0

#define Bootstartadr  0x3F7FF6
#define JumpToBoot()  (*((void(*)(void))Bootstartadr))()

//结构体定义


//联合体定义


//对外部文件开放全局变量声明
extern Uint16 Flag_Tick;
extern Uint16 Cnt_Tick;

//对外部文件开放函数声明
extern void     WriteBootFlag(void);
extern Uint16   ReadBootFlag(void);
extern void Fault_Code_DogReset_TX(void);
#endif /* APP_INCLUDE_MAIN_H_ */
