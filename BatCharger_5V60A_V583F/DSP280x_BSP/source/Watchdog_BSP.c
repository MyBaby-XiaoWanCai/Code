/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Watchdog_BSP.c
// Created on: 2020-4-8
//     Author: LuoAnJiang
/**************************************************************************************/
//ȫ�ֱ�������
#include "Headers_All.h"

//��������

void InitWD(void)
{
    EALLOW;
    SysCtrlRegs.SCSR =0x0001;
    SysCtrlRegs.WDCR =0x002F;
    EDIS;
}
//��������

//File end
