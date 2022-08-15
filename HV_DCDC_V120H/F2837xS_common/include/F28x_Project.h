//###########################################################################
//
// FILE:   F28x_Project.h
//
// TITLE:  F28x Project Headerfile and Examples Include File
//
//###########################################################################
// $TI Release: F2837xS Support Library v210 $
// $Release Date: Tue Nov  1 15:35:23 CDT 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef F28X_PROJECT_H
#define F28X_PROJECT_H


#define INT8U       unsigned char
#define INT8S         signed char
#define INTOS       unsigned int                /* the smallest unit int the cpu    */
#define INT16U      unsigned int
#define INT16S        signed int
#define INT32U      unsigned long
#define INT32S        signed long
//
// Included Files
//
#include "F2837xS_Cla_typedefs.h"  // F2837xS CLA Type definitions
#include "F2837xS_device.h"        // F2837xS Headerfile Include File
#include "F2837xS_Examples.h"      // F2837xS Examples Include File

#include "Version.h"
#include "CAN_BSP.h"
#include "ADC_BSP.h"
#include "DCDC_BSP.h"
#include "GPIO_BSP.h"
#include "HRPWM.h"
#include "main.h"
#include "CAN_JN910.h"
#include "Ctr_Fan.h"
#include "Protect_Global.h"
#include "EEPROM_BSP.h"
#include "cla_shared.h"
#include "PI_CLA.h"
#include "PWMDRV_3phInv_CLA.h"
#include "SCI_BSP.h"
#include "AD7124_BSP.h"
#include "SFO_V8.h"
#include "FLASH_BSP.h"
#include "TaskAllot.h"
#include "Ctr_LED.h"

#endif  // end of F28X_PROJECT_H definition

//
// End of file
//
