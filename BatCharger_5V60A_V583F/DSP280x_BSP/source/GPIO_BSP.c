/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: GPIO_BSP.c
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义


//函数声明


//函数定义
void InitGPIO_BSP(void)
{
    EALLOW;
    CONFIG_GPIO_LED_RED;
    CONFIG_GPIO_LED_GREEN;
    CONFIG_GPIO_CD4053_RANGE;
    CONFIG_GPIO_W5500_REQ;
    CONFIG_GPIO_FAULT_FAN;
    CONFIG_GPIO_AD7606_RST;
    CONFIG_GPIO_AD7606_BUSY;
    CONFIG_GPIO_AD7606_CONV;
    //CONFIG_GPIO_FLASH_CS;
    CONFIG_GPIO_W5500_CS;
    CONFIG_GPIO_CTR_OUT_CHA;
    CONFIG_GPIO_CTR_OUT_CHB;
    EDIS;

    CLEAR_AD7606_RST;
}





//File end
