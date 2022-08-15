/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
// Project: BatCharger
// File name: main_cpu1.c
// Created on: 2020-4-20
// Author: Hu
/**************************************************************************************/
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "driverlib/can.h"
//全局变量定义
Uint16 Flag_Tick=0;
Uint16 Cnt_Tick=0;
// 其它参数定义

//
// Main
//
Uint16 *ExRamStart = (Uint16 *)0x300000;
extern void InitEmif1(void);
Uint16  ErrCount = 0;
float32  Test_Wave[1000];
interrupt void Cputimer2Interrupt(void);
interrupt void adca1_isr(void);
// Defines
Uint16 AdcaResult0;
Uint16 AdcaResult1;
Uint16 AdcaResult2;
Uint16 AdcaResult3;
Uint16 AdcaResult4;
Uint16 AdcaResult5;
Uint16 AdcaResult14;
Uint16 AdcaResult15;

Uint16 AdcbResult0;
Uint16 AdcbResult1;
Uint16 AdcbResult2;
Uint16 AdcbResult3;

Uint16 AdccResult2;
Uint16 AdccResult3;

Uint16 AdcdResult0;
Uint16 AdcdResult1;
Uint16 AdcdResult2;
Uint16 testdata;
interrupt void xint1_isr(void);
interrupt void xint2_isr(void);
interrupt void xint3_isr(void);
interrupt void xint4_isr(void);
Uint16 InterruptCount1 = 0,InterruptCount2 = 0,InterruptCount3 = 0,InterruptCount4 = 0;
#define SFO_INCOMPLETE      0
#define SFO_COMPLETE        1
#define SFO_ERROR           2
Uint16 status;
void main(void)
{
    Uint16 i=0;
   //Step 1. Initialize System Control:
   //PLL, WatchDog,enabe Peripheral Clocks
   //This example function is found in the f2838x_sysctrl.c file.
   //
   DINT;
   InitSysCtrl();
   //
   //Step 2. Initialize GPIO
   InitGpio();
   InitGPIO_BSP();
   //Step 3. Clear all __interrupts and initialize PIE vector table:
   //Disable CPU __interrupts
       //
   InitEPwmGpio();   // EPWM1A  EPWM1B  through EPWM9
   DINT;
   //Initialize the PIE control registers to their default state.
   //The default state is all PIE interrupts disabled and flags
   //are cleared.
   //
   InitPieCtrl();
   //Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;
   //Initialize the PIE vector table with pointers to the shell Interrupt
   //Service Routines (ISR).
   InitPieVectTable();
   PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
   PieCtrlRegs.PIEIER9.bit.INTx5 = 1;
   PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
   PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Group 1 INT4
   PieCtrlRegs.PIEIER1.bit.INTx5 = 1;          // Enable PIE Group 1 INT5
   PieCtrlRegs.PIEIER12.bit.INTx1 = 1;          // Enable PIE Group 12 INT1
   PieCtrlRegs.PIEIER12.bit.INTx2 = 1;          // Enable PIE Group 12 INT2
   EALLOW;  // This is needed to write to EALLOW protected registers
   PieVectTable.CANA0_INT = canbISR;
   PieVectTable.ADCA1_INT = &adca1_isr;
   PieVectTable.TIMER2_INT      = &Cputimer2Interrupt;
   PieVectTable.XINT1_INT = &xint1_isr;
   PieVectTable.XINT2_INT = &xint2_isr;
   PieVectTable.XINT3_INT = &xint3_isr;
   PieVectTable.XINT4_INT = &xint4_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers
       //Step 4. User specific code:
   // Step 4. Initialize all the Device Peripherals:
   InitCpuTimers();
   ConfigCpuTimer(&CpuTimer2, 200, 100);//1mS中断1次
   StartCpuTimer2();

   InitCAN_BSP();
   InitADC_BSP();
   InitI2C_BSP();
   InitEmif1();
   Init_CAN_JN910();
   InitDCDC_BSP();
   EALLOW;
   CONFIG_GPIO_RELAY;
   EDIS;
   IER |= M_INT14; //for Timer2
   IER |= M_INT1; //使能第一组中断
   IER |= M_INT9;
   IER |= M_INT12;
   HRPWM1_Config();
   HRPWM2_Config();
   // Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global interrupt INTM
   ERTM;   // Enable Global realtime interrupt DBGM
   //EEPROM_Write_Uint16(cEepromT_Cali,0x5a5a);
  // DELAY_US(10000);
  // testdata=EEPROM_Read_Uint16(cEepromT_Cali);
  // DELAY_US(10000);

 //  PWMON_Config(1);

 //  PWMOFF_Config(1);
 //  PWMON_Config(2);
/*
   for(i = 0; i < 0xFFFF; i++)
    {
        *(ExRamStart + i) = 0x5555;
        if(*(ExRamStart + i) != 0x5555)
        {
           while(1);
        }
    }
    asm(" ESTOP0");
    for(i = 0; i < 0xFFFF; i++)
    {
       *(ExRamStart + i) = 0xAAAA;
       if(*(ExRamStart + i) != 0xAAAA)
       {
          while(1);
       }
    }
    asm(" ESTOP0");
    for(i = 0; i< 0xFFFF; i++)
    {
       *(ExRamStart + i) = i;
       if(*(ExRamStart + i) != i)
       {
          while(1);
       }
       if(*ExRamStart == 0x4000)
       {
          while(1);
       }
    }

    asm(" ESTOP0");
*/
   while(1)
   {
       if((Flag_Tick==TRUE)&&(Cnt_Tick==0))
       {
           Flag_Tick=FALSE;
           CAN_APP_RX();
           CAN_APP_TX();
       }

       if((Flag_Tick==TRUE)&&(Cnt_Tick==1))
       {
           Flag_Tick=FALSE;

       }

       if((Flag_Tick==TRUE)&&(Cnt_Tick==2))
       {
           Flag_Tick=FALSE;

       }
       if((Flag_Tick==TRUE)&&(Cnt_Tick==3))
       {
           Flag_Tick=FALSE;

       }

  }



}

float32 Vol_out,Cur_out,Vdc_in;
interrupt void Cputimer2Interrupt(void)
{
      RELAY_TOGGLE;
     AdcaRegs.ADCSOCFRC1.all = 0xC03F; //SOC0 and SOC1
     AdcbRegs.ADCSOCFRC1.all = 0x000F; //SOC0 and SOC1
     AdccRegs.ADCSOCFRC1.all = 0x000C; //SOC0 and SOC1
     AdcdRegs.ADCSOCFRC1.all = 0x0005; //SOC0 and SOC1
    //wait for ADCA to complete, then acknowledge flag
         //
     while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0);
     AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

     while(AdcbRegs.ADCINTFLG.bit.ADCINT1 == 0);
     AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

     while(AdccRegs.ADCINTFLG.bit.ADCINT1 == 0);
     AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

     while(AdcdRegs.ADCINTFLG.bit.ADCINT1 == 0);
     AdcdRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
     //
     //store results
     //
     AdcaResult0 = AdcaResultRegs.ADCRESULT0;
     AdcaResult1 = AdcaResultRegs.ADCRESULT1;
     AdcaResult2 = AdcaResultRegs.ADCRESULT2;
     AdcaResult3 = AdcaResultRegs.ADCRESULT3;
     AdcaResult4 = AdcaResultRegs.ADCRESULT4;
     AdcaResult5 = AdcaResultRegs.ADCRESULT5;
     AdcaResult14 = AdcaResultRegs.ADCRESULT14;
     AdcaResult15 = AdcaResultRegs.ADCRESULT15;


     AdcbResult0 = AdcbResultRegs.ADCRESULT0;
     AdcbResult1 = AdcbResultRegs.ADCRESULT1;
     AdcbResult2 = AdcbResultRegs.ADCRESULT2;
     AdcbResult3 = AdcbResultRegs.ADCRESULT3;

     AdccResult2 = AdccResultRegs.ADCRESULT2;
     AdccResult3 = AdccResultRegs.ADCRESULT3;

     AdcdResult0 = AdcdResultRegs.ADCRESULT0;
  //   AdcdResult1 = AdcdResultRegs.ADCRESULT1;
     AdcdResult2 = AdcdResultRegs.ADCRESULT2;

     Vdc_in=AdccResult2;
     Vdc_in=(0.000030517578125*Vdc_in-1)*3;
     Vol_out=(AdcdResult0);
     Vol_out=(0.000030517578125*Vol_out-1)*3;

     Cur_out=(AdcdResult2);
     Cur_out=(0.000030517578125*Cur_out-1)*3;
     DCDC_Service_BSP();
     Test_Wave[Cnt_Tick++]=(float32)AdcaResult2*0.000732421875;



    //后台任务时间片分配
     if(Cnt_Tick>=999)
     {

         Cnt_Tick=0;

     //    LED_RED_TOGGLE;
     }
   //  Flag_Tick=TRUE;
     CpuTimer2Regs.TCR.bit.TIF   = 1;
}
//
// End of File
//
interrupt void adca1_isr(void)
{
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
interrupt void xint1_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    DELAY_US(1000);
    if(GpioDataRegs.GPADAT.bit.GPIO12 == 0) InterruptCount1++;
    if(InterruptCount1 > 9) InterruptCount1 = 0;

}

interrupt void xint2_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    DELAY_US(1000);
    if(GpioDataRegs.GPADAT.bit.GPIO13 == 0) InterruptCount2++;
    if(InterruptCount2 > 9) InterruptCount2 = 0;

}

interrupt void xint3_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
    DELAY_US(1000);
    if(GpioDataRegs.GPADAT.bit.GPIO14 == 0) InterruptCount3++;
    if(InterruptCount3 > 9) InterruptCount3 = 0;

}

interrupt void xint4_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
    DELAY_US(1000);
    if(GpioDataRegs.GPADAT.bit.GPIO15 == 0) InterruptCount4++;
    if(InterruptCount4 > 9) InterruptCount4 = 0;

}
