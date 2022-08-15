/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Headers_All.h
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef APP_INCLUDE_HEADERS_ALL_H_
#define APP_INCLUDE_HEADERS_ALL_H_

#include "DSP280x_Device.h"      // DSP280x Header file Include File
#include "DSP280x_Examples.h"     // DSP280x Examples Include File

#define BOOLEAN     unsigned char
#define INT8U       unsigned char
#define INT8S         signed char
#define INTOS       unsigned int                /* the smallest unit int the cpu    */
#define INT16U      unsigned int
#define INT16S        signed int
#define INT32U      unsigned long
#define INT32S        signed long

#include "Version.h"
#include "DCDC_BSP.h"
#include "EEPROM_BSP.h"
#include "FLASH_BSP.h"
#include "GPIO_BSP.h"
#include "SCI_BSP.h"
#include "W5500_BSP.h"
#include "ADC_BSP.h"
#include "ConstDefine.h"
#include "Protect_Global_Step.h"
#include "WorkStep_Flow.h"
#include "Parall_BSP.h"
#include "Net_Global.h"
#include "Net_WorkStep.h"
#include "Net_Others.h"
#include "Net_Debug.h"
#include "COM.h"
#include "Ctr_LED.h"
#include "Net_Cali.h"
#include "Net_Record.h"
#include "Net_Fluct.h"
#include "Protect_Global_Step.h"
#include "Watchdog_BSP.h"
#include "WorkStep_Record.h"
#include "Ctr_Fan.h"
#include "Auto_Aging.h"
#include "Compen_Cur.h"

#include "main.h"


#endif /* APP_INCLUDE_HEADERS_ALL_H_ */
