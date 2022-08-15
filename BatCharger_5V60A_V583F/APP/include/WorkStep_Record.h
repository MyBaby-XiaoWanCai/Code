/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: WorkStep_Record.h
// Created on: 2020-4-11
//     Author: LuoAnJiang
/**************************************************************************************/
#ifndef APP_INCLUDE_WORKSTEP_RECORD_H_
#define APP_INCLUDE_WORKSTEP_RECORD_H_
#include "WorkStep_Flow.h"

#define NUM_PAST_DATA   11

//结构体定义
typedef struct struct_Past_Data
{
    struct_Image        Image;
    struct_Image       *Ptr_Image;
    struct struct_Past_Data   *Ptr_Next;
} struct_Past_Data;

typedef struct struct_Past_Data_Manage
{
    struct_Past_Data *Ptr_In;
    struct_Past_Data *Ptr_Out;
    Uint16 Period_Save;
    Uint16 Cnt_Save;
    Uint16 Req_Save;
    Uint16 Req_TX;
    Uint16 EN;
    Uint16 Req_Reset;
} struct_Past_Data_Manage;
//联合体定义


//对外部文件开放全局变量声明
extern struct_Past_Data_Manage Past_Data_Manage;
extern struct_Past_Data        Past_Data[];
//对外部文件开放函数声明
extern void Function_Record(void);
extern void Data_Up(void);
extern void Past_Data_Manage_Init(void);


#endif /* APP_INCLUDE_WORKSTEP_RECORD_H_ */

//File end
