/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Watchdog_BSP.c
// Created on: 2020-4-8
//     Author: LuoAnJiang
/**************************************************************************************/
//全局变量定义
#include "Headers_All.h"

//函数声明

void InitWD(void)
{
    EALLOW;
    SysCtrlRegs.SCSR =0x0001;
    SysCtrlRegs.WDCR =0x002F;
    EDIS;
}
//函数定义

//File end
