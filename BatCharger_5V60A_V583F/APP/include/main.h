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

//�ṹ�嶨��


//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern Uint16 Flag_Tick;
extern Uint16 Cnt_Tick;

//���ⲿ�ļ����ź�������
extern void     WriteBootFlag(void);
extern Uint16   ReadBootFlag(void);
extern void Fault_Code_DogReset_TX(void);
#endif /* APP_INCLUDE_MAIN_H_ */
