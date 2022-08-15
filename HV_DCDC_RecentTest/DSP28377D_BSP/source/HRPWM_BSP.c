/*
 * HRPWM_BSP.c
 *
 *  Created on: 2021Äê6ÔÂ20ÈÕ
 *      Author: JC
 */
#include "F28x_Project.h"

volatile struct EPWM_REGS *ePWM[PWM_CH] =
             {  &EPwm1Regs, &EPwm1Regs, &EPwm2Regs, &EPwm3Regs, &EPwm4Regs,
                &EPwm5Regs, &EPwm6Regs, &EPwm7Regs, &EPwm8Regs};
// HRPWM_Config - Configures all ePWM channels and sets up HRPWM
// on ePWMxA / ePWMxB  channels
//
void HRPWM1_Config(void)
{
      EPwm1Regs.TZSEL.all            = 0x0100;
      EPwm1Regs.TZCTL.all            = 0x000A;
      EPwm1Regs.TZFRC.all            = 0x0004;

      //
      EPwm1Regs.TBPRD                = PWM_PERIOD;             // Set timer period
      EPwm1Regs.TBPHS.bit.TBPHS     = 0x0000;           // Phase is 0
      EPwm1Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm1Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm1Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm1Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm1Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm1Regs.CMPA.bit.CMPA       = PWM_DEADTIME;
      EPwm1Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADTIME;//PWM_DEADTIME
      // Set actions
      EPwm1Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm1Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm1Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm1Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm1Regs.AQCTLB.bit.ZRO       = AQ_SET;
      EPwm1Regs.AQCTLB.bit.PRD       = AQ_CLEAR;

      // Active Low PWMs - Setup Deadband
      EPwm1Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm1Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm1Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm1Regs.DBRED.bit.DBRED                = PWM_DEADTIME+1;
      EPwm1Regs.DBFED.bit.DBFED                = PWM_DEADTIME+1;
      EPwm1Regs.TBPHS.bit.TBPHS     = 0;
      EPwm1Regs.TBCTL.bit.SYNCOSEL   = TB_CTR_ZERO;
}

void HRPWM2_Config(void)
{
      EPwm2Regs.TZSEL.all            = 0x0100;
      EPwm2Regs.TZCTL.all            = 0x000A;
      EPwm2Regs.TZFRC.all            = 0x0004;

      //
      EPwm2Regs.TBPRD                = PWM_PERIOD;       // Set timer period
      EPwm2Regs.TBPHS.bit.TBPHS     = 0x0000;           // Phase is 0
      EPwm2Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm2Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm2Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm2Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm2Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm2Regs.CMPA.bit.CMPA       = PWM_DEADTIME;
      EPwm2Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADTIME;
      // Set actions
      EPwm2Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm2Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm2Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm2Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm2Regs.AQCTLB.bit.PRD       = AQ_CLEAR;
      EPwm2Regs.AQCTLB.bit.ZRO       = AQ_SET;

      // Active Low PWMs - Setup Deadband
      EPwm2Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm2Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm2Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm2Regs.DBRED.bit.DBRED      = PWM_DEADTIME+1;
      EPwm2Regs.DBFED.bit.DBFED      = PWM_DEADTIME+1;

      EPwm2Regs.TBPHS.bit.TBPHS     = 1000;
      EPwm2Regs.TBCTL.bit.PHSEN      = TB_ENABLE;       // Disable phase loading

}



void PWMON_Config(Uint16 CH)
{
    EALLOW;
    (*ePWM[CH]).TZSEL.all=0x0000;
    (*ePWM[CH]).TZCLR.all=0x0007;
    (*ePWM[CH]).TZFRC.all=0x0000;
    EDIS;
}

void PWMOFF_Config(Uint16 CH)
{
    EALLOW;
    (*ePWM[CH]).TZSEL.all=0x0100;
    (*ePWM[CH]).TZCTL.all=0x000A;
    (*ePWM[CH]).TZFRC.all=0x0004;
    EDIS;
    (*ePWM[CH]).CMPA.bit.CMPA=PWM_DEADTIME;
    (*ePWM[CH]).CMPB.bit.CMPB=PWM_PERIOD;
}
