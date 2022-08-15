/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: ADC_BSP.c
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义


//函数声明


//函数定义
void InitADC_BSP(void)
{
    AdcRegs.ADCREFSEL.bit.REF_SEL = 0;
    AdcRegs.ADCTRL1.bit.RESET = 1;              // Reset the ADC module
    asm(" RPT #10 || NOP");                     // Must wait 12-cycles (worst-case) for ADC reset to take effect
    AdcRegs.ADCTRL3.all = 0x00C6;               // first power-up ref and bandgap circuits
    DELAY_US(7000);
    AdcRegs.ADCTRL3.bit.ADCPWDN = 1;            // Set ADCPWDN=1 to power main ADC
    DELAY_US(200);                              // Wait 20us before using the ADC
    AdcRegs.ADCMAXCONV.all = 0x000F;
    AdcRegs.ADCCHSELSEQ1.all = 0x3210;
    AdcRegs.ADCCHSELSEQ2.all = 0x7654;
    AdcRegs.ADCCHSELSEQ3.all = 0xBA98;
    AdcRegs.ADCCHSELSEQ4.all = 0xFEDC;
    AdcRegs.ADCTRL1.all = 0x2010;
    AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 1;
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;  //reset AD   0912
    DELAY_US(200);
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;   //start AD  0912
}
