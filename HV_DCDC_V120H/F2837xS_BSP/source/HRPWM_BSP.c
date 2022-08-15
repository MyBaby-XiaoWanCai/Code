/*
 * HRPWM_BSP.c
 *
 *  Created on: 2021Äê6ÔÂ20ÈÕ
 *      Author: JC
 */
#include "F28x_Project.h"
#include "inc/hw_memmap.h"
#include "inc/hw_epwm.h"
#include "driverlib/epwm.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
volatile struct EPWM_REGS *ePWM[PWM_CH] =
             {  &EPwm1Regs, &EPwm1Regs, &EPwm2Regs, &EPwm3Regs, &EPwm4Regs,
                &EPwm5Regs, &EPwm6Regs, &EPwm7Regs, &EPwm8Regs};
// HRPWM_Config - Configures all ePWM channels and sets up HRPWM
//                on ePWMxA / ePWMxB  channels
//
uint32_t   MEP_ScaleFactor;
uint16_t             BTS_sfoStatus;

void HRPWM1_Config(void)
{
   // PWMON_Config(1);
    //  EPwm1Regs.TZSEL.all            = 0x0100;
    //  EPwm1Regs.TZCTL.all            = 0x000A;
    //  EPwm1Regs.TZFRC.all            = 0x0004;
    //  EALLOW;
   //   ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = 0x00;
   //   EDIS;

      EPwm1Regs.TBPRD                = PWM_PERIOD;             // Set timer period
      EPwm1Regs.TBPHS.bit.TBPHS     =  0x0000;           // Phase is 0
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
      EPwm1Regs.TBCTL.bit.SYNCOSEL   = TB_CTR_ZERO;

      // Setup compare
      EPwm1Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
      EPwm1Regs.CMPA.bit.CMPAHR     = 0;
      EPwm1Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
      EPwm1Regs.CMPB.bit.CMPBHR     = 0;
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


      EPwm1Regs.DBRED.bit.DBRED                = PWM_DEADREDTIME+1;
      EPwm1Regs.DBFED.bit.DBFED                = PWM_DEADFEDTIME+1;
      EPwm1Regs.TBPHS.bit.TBPHS     = 0;


      EPwm1Regs.CMPCTL2.bit.SHDWCMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm1Regs.CMPCTL2.bit.LOADCMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      EPwm1Regs.ETSEL.bit.SOCAEN    = 1;    // Disable SOC on A group
      EPwm1Regs.ETSEL.bit.SOCASEL    = 4;   // Select SOC on up-count
      EPwm1Regs.ETSEL.bit.SOCASELCMP  = 1;
      EPwm1Regs.ETPS.bit.SOCAPRD = 1;       // Generate pulse on 1st event*/

      EPwm1Regs.CMPC =0;

      EPwm1Regs.ETSEL.bit.INTEN=1;
      EPwm1Regs.ETSEL.bit.INTSEL=ET_CTR_PRD;
      EPwm1Regs.ETPS.bit.INTPSSEL  = 1;
      EPwm1Regs.ETINTPS.bit.INTPRD2=0x05;

      EPwm1Regs.ETCLR.all           |= 0x01;

      EALLOW;
      EPwm1Regs.HRCNFG.all = 0x0;
      EPwm1Regs.HRCNFG.bit.EDGMODE = HR_FEP;
      EPwm1Regs.HRCNFG.bit.CTLMODE = HR_CMP;
      EPwm1Regs.HRCNFG.bit.HRLOAD  = HR_CTR_PRD;

      EPwm1Regs.HRCNFG.bit.EDGMODEB = HR_REP;
      EPwm1Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
      EPwm1Regs.HRCNFG.bit.HRLOADB  = HR_CTR_PRD;
      EDIS;

      // Select TRIPIN1 (INPUT XBAR INPUT1) as the source for DCAH
   /*   EPWM_selectDigitalCompareTripInput( EPWM1_BASE, EPWM_DC_TRIP_TRIPIN1, EPWM_DC_TYPE_DCAH);

      EPWM_enableTripZoneSignals(EPWM1_BASE, EPWM_TZ_SIGNAL_DCAEVT1);
        //
      // DCAEVT1 is generated when DCAH is low
        //
      EPWM_setTripZoneDigitalCompareEventCondition(
      EPWM1_BASE, EPWM_TZ_DC_OUTPUT_A1, EPWM_TZ_EVENT_DCXH_LOW);

      // DCAEVT1 uses the unfiltered version of DCAEVT1
      EPWM_setDigitalCompareEventSource(
      EPWM1_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_SOURCE_ORIG_SIGNAL);



      EPWM_setDigitalCompareEventSyncMode(EPWM1_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_INPUT_NOT_SYNCED);

      EPWM_setTripZoneAction(EPWM1_BASE,
                                  EPWM_TZ_ACTION_EVENT_TZB,
                                  EPWM_TZ_ACTION_LOW);
      EPWM_setTripZoneAction(EPWM1_BASE,
                                          EPWM_TZ_ACTION_EVENT_TZA,
                                          EPWM_TZ_ACTION_LOW);
*/
        //
        // Enable TZ interrupt
        //
       // EPWM_enableTripZoneInterrupt(EPWM1_BASE, EPWM_TZ_INTERRUPT_DCAEVT1);
}

void HRPWM2_Config(void)
{
      EPwm2Regs.TZSEL.all            = 0x0100;
      EPwm2Regs.TZCTL.all            = 0x000A;
      EPwm2Regs.TZFRC.all            = 0x0004;

      //
      EPwm2Regs.TBPRD                = PWM_PERIOD;       // Set timer period
      EPwm2Regs.TBPHS.bit.TBPHS      = 0x0000;           // Phase is 0
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
      EPwm2Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
      EPwm2Regs.CMPA.bit.CMPAHR     =0;
      EPwm2Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
      EPwm2Regs.CMPB.bit.CMPBHR     =0;
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

      EPwm2Regs.DBRED.bit.DBRED      = PWM_DEADREDTIME+1;
      EPwm2Regs.DBFED.bit.DBFED      = PWM_DEADFEDTIME+1;

      EPwm2Regs.TBPHS.bit.TBPHS     = 500;
      EPwm2Regs.TBCTL.bit.PHSEN      = TB_ENABLE;       // Disable phase loading


      EPwm2Regs.CMPCTL2.bit.SHDWCMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm2Regs.CMPCTL2.bit.LOADCMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      EPwm2Regs.ETSEL.bit.SOCAEN    = 1;    // Disable SOC on A group
      EPwm2Regs.ETSEL.bit.SOCASEL    = 4;   // Select SOC on up-count
      EPwm2Regs.ETSEL.bit.SOCASELCMP  = 1;
      EPwm2Regs.ETPS.bit.SOCAPRD = 1;       // Generate pulse on 1st event*/
      EPwm2Regs.CMPC =0;

      EALLOW;
      EPwm2Regs.HRCNFG.all = 0x0;
      EPwm2Regs.HRCNFG.bit.EDGMODE = HR_FEP;
      EPwm2Regs.HRCNFG.bit.CTLMODE = HR_CMP;
      EPwm2Regs.HRCNFG.bit.HRLOAD  = HR_CTR_PRD;

      EPwm2Regs.HRCNFG.bit.EDGMODEB = HR_REP;
      EPwm2Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
      EPwm2Regs.HRCNFG.bit.HRLOADB  = HR_CTR_PRD;
      EDIS;

  /*  EPWM_selectDigitalCompareTripInput(EPWM2_BASE, EPWM_DC_TRIP_TRIPIN1, EPWM_DC_TYPE_DCAH);

      EPWM_enableTripZoneSignals(EPWM2_BASE, EPWM_TZ_SIGNAL_DCAEVT1);
      EPWM_enableTripZoneSignals(EPWM2_BASE, EPWM_TZ_SIGNAL_DCBEVT1);

      // DCAEVT1 is generated when DCAH is low
      EPWM_setTripZoneDigitalCompareEventCondition(EPWM2_BASE, EPWM_TZ_DC_OUTPUT_A1, EPWM_TZ_EVENT_DCXH_LOW);

      // DCAEVT1 uses the unfiltered version of DCAEVT1
      EPWM_setDigitalCompareEventSource(EPWM2_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_SOURCE_ORIG_SIGNAL);

      // DCAEVT1 is asynchronous
      EPWM_setDigitalCompareEventSyncMode(EPWM2_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_INPUT_NOT_SYNCED);

      EPWM_setTripZoneAction(EPWM2_BASE, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);

      EPWM_setTripZoneAction(EPWM2_BASE, EPWM_TZ_ACTION_EVENT_TZB, EPWM_TZ_ACTION_LOW);
 */



}

void HRPWM3_Config(void)
{
   // PWMON_Config(1);
    //  EPwm1Regs.TZSEL.all            = 0x0100;
    //  EPwm1Regs.TZCTL.all            = 0x000A;
    //  EPwm1Regs.TZFRC.all            = 0x0004;


      EPwm3Regs.TBPRD                = PWM_PERIOD;             // Set timer period
      EPwm3Regs.TBPHS.bit.TBPHS     = 0x0000;           // Phase is 0
      EPwm3Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm3Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm3Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm3Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm3Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm3Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
      EPwm3Regs.CMPA.bit.CMPAHR     =0;
      EPwm3Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
      EPwm3Regs.CMPB.bit.CMPBHR    =0;
      // Set actions
      EPwm3Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm3Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm3Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm3Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm3Regs.AQCTLB.bit.ZRO       = AQ_SET;
      EPwm3Regs.AQCTLB.bit.PRD       = AQ_CLEAR;

      // Active Low PWMs - Setup Deadband
      EPwm3Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm3Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm3Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm3Regs.DBRED.bit.DBRED                = PWM_DEADREDTIME+1;
      EPwm3Regs.DBFED.bit.DBFED                = PWM_DEADFEDTIME+1;
      EPwm3Regs.TBPHS.bit.TBPHS     = 0;
      EPwm3Regs.TBCTL.bit.PHSEN      = TB_ENABLE;       // Disable phase loading

      EPwm3Regs.CMPCTL2.bit.SHDWCMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm3Regs.CMPCTL2.bit.LOADCMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      EPwm3Regs.ETSEL.bit.SOCAEN    = 1;    // Disable SOC on A group
      EPwm3Regs.ETSEL.bit.SOCASEL    = 4;   // Select SOC on up-count
      EPwm3Regs.ETSEL.bit.SOCASELCMP  = 1;
      EPwm3Regs.ETPS.bit.SOCAPRD = 1;       // Generate pulse on 1st event*/

      EPwm3Regs.CMPC =0;

      EALLOW;
      EPwm3Regs.HRCNFG.all = 0x0;
      EPwm3Regs.HRCNFG.bit.EDGMODE = HR_FEP;
      EPwm3Regs.HRCNFG.bit.CTLMODE = HR_CMP;
      EPwm3Regs.HRCNFG.bit.HRLOAD  = HR_CTR_PRD;

      EPwm3Regs.HRCNFG.bit.EDGMODEB = HR_REP;
      EPwm3Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
      EPwm3Regs.HRCNFG.bit.HRLOADB  = HR_CTR_PRD;
      EDIS;

      EPWM_selectDigitalCompareTripInput(EPWM3_BASE, EPWM_DC_TRIP_TRIPIN2, EPWM_DC_TYPE_DCAH);

      EPWM_enableTripZoneSignals(EPWM3_BASE, EPWM_TZ_SIGNAL_DCAEVT1);
      EPWM_enableTripZoneSignals(EPWM3_BASE, EPWM_TZ_SIGNAL_DCBEVT1);

      // DCAEVT1 is generated when DCAH is low
      EPWM_setTripZoneDigitalCompareEventCondition(EPWM3_BASE, EPWM_TZ_DC_OUTPUT_A1, EPWM_TZ_EVENT_DCXH_LOW);

      // DCAEVT1 uses the unfiltered version of DCAEVT1
      EPWM_setDigitalCompareEventSource(EPWM3_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_SOURCE_ORIG_SIGNAL);

      // DCAEVT1 is asynchronous
      EPWM_setDigitalCompareEventSyncMode(EPWM3_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_INPUT_NOT_SYNCED);

      EPWM_setTripZoneAction(EPWM3_BASE, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);

      EPWM_setTripZoneAction(EPWM3_BASE, EPWM_TZ_ACTION_EVENT_TZB, EPWM_TZ_ACTION_LOW);


}


void HRPWM4_Config(void)
{


      //
      EPwm4Regs.TBPRD                = PWM_PERIOD;       // Set timer period
      EPwm4Regs.TBPHS.bit.TBPHS     = 0x0000;           // Phase is 0
      EPwm4Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm4Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm4Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm4Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm4Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm4Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
      EPwm4Regs.CMPA.bit.CMPAHR     =0;
      EPwm4Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
      EPwm4Regs.CMPB.bit.CMPBHR     =0;
      // Set actions
      EPwm4Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm4Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm4Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm4Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm4Regs.AQCTLB.bit.PRD       = AQ_CLEAR;
      EPwm4Regs.AQCTLB.bit.ZRO       = AQ_SET;

      // Active Low PWMs - Setup Deadband
      EPwm4Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm4Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm4Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm4Regs.DBRED.bit.DBRED      = PWM_DEADREDTIME+1;
      EPwm4Regs.DBFED.bit.DBFED      = PWM_DEADFEDTIME+1;

      EPwm4Regs.TBPHS.bit.TBPHS     = 500;
      EPwm4Regs.TBCTL.bit.PHSEN      = TB_ENABLE;       // Disable phase loading


      EPwm4Regs.CMPCTL2.bit.SHDWCMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm4Regs.CMPCTL2.bit.LOADCMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      EPwm4Regs.ETSEL.bit.SOCAEN    = 1;    // Disable SOC on A group
      EPwm4Regs.ETSEL.bit.SOCASEL    = 4;   // Select SOC on up-count
      EPwm4Regs.ETSEL.bit.SOCASELCMP  = 1;
      EPwm4Regs.ETPS.bit.SOCAPRD = 1;       // Generate pulse on 1st event*/
      EPwm4Regs.CMPC =0;


      EALLOW;
      EPwm4Regs.HRCNFG.all = 0x0;
      EPwm4Regs.HRCNFG.bit.EDGMODE = HR_FEP;
      EPwm4Regs.HRCNFG.bit.CTLMODE = HR_CMP;
      EPwm4Regs.HRCNFG.bit.HRLOAD  = HR_CTR_PRD;

      EPwm4Regs.HRCNFG.bit.EDGMODEB = HR_REP;
      EPwm4Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
      EPwm4Regs.HRCNFG.bit.HRLOADB  = HR_CTR_PRD;
      EDIS;

      EPWM_selectDigitalCompareTripInput(EPWM4_BASE, EPWM_DC_TRIP_TRIPIN2, EPWM_DC_TYPE_DCAH);

      EPWM_enableTripZoneSignals(EPWM4_BASE, EPWM_TZ_SIGNAL_DCAEVT1);
      EPWM_enableTripZoneSignals(EPWM4_BASE, EPWM_TZ_SIGNAL_DCBEVT1);

      // DCAEVT1 is generated when DCAH is low
      EPWM_setTripZoneDigitalCompareEventCondition(EPWM4_BASE, EPWM_TZ_DC_OUTPUT_A1, EPWM_TZ_EVENT_DCXH_LOW);

      // DCAEVT1 uses the unfiltered version of DCAEVT1
      EPWM_setDigitalCompareEventSource(EPWM4_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_SOURCE_ORIG_SIGNAL);

      // DCAEVT1 is asynchronous
      EPWM_setDigitalCompareEventSyncMode(EPWM4_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_INPUT_NOT_SYNCED);

      EPWM_setTripZoneAction(EPWM4_BASE, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);

      EPWM_setTripZoneAction(EPWM4_BASE, EPWM_TZ_ACTION_EVENT_TZB, EPWM_TZ_ACTION_LOW);



}

void HRPWM5_Config(void)
{


      EPwm5Regs.TBPRD                = PWM_PERIOD;             // Set timer period
      EPwm5Regs.TBPHS.bit.TBPHS     = 0x0000;           // Phase is 0
      EPwm5Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm5Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm5Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm5Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm5Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm5Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
      EPwm5Regs.CMPA.bit.CMPAHR     =0;
      EPwm5Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
      EPwm5Regs.CMPB.bit.CMPBHR    =0;
      // Set actions
      EPwm5Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm5Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm5Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm5Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm5Regs.AQCTLB.bit.ZRO       = AQ_SET;
      EPwm5Regs.AQCTLB.bit.PRD       = AQ_CLEAR;

      // Active Low PWMs - Setup Deadband
      EPwm5Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm5Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm5Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm5Regs.DBRED.bit.DBRED                = PWM_DEADREDTIME+1;
      EPwm5Regs.DBFED.bit.DBFED                = PWM_DEADFEDTIME+1;
      EPwm5Regs.TBPHS.bit.TBPHS     = 0;
      EPwm5Regs.TBCTL.bit.PHSEN      = TB_ENABLE;       // Disable phase loading

      EPwm5Regs.CMPCTL2.bit.SHDWCMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm5Regs.CMPCTL2.bit.LOADCMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      EPwm5Regs.ETSEL.bit.SOCAEN    = 1;    // Disable SOC on A group
      EPwm5Regs.ETSEL.bit.SOCASEL    = 4;   // Select SOC on up-count
      EPwm5Regs.ETSEL.bit.SOCASELCMP  = 1;
      EPwm5Regs.ETPS.bit.SOCAPRD = 1;       // Generate pulse on 1st event*/
      EPwm5Regs.CMPC =0;

      EALLOW;
      EPwm5Regs.HRCNFG.all = 0x0;
      EPwm5Regs.HRCNFG.bit.EDGMODE = HR_FEP;
      EPwm5Regs.HRCNFG.bit.CTLMODE = HR_CMP;
      EPwm5Regs.HRCNFG.bit.HRLOAD  = HR_CTR_PRD;

      EPwm5Regs.HRCNFG.bit.EDGMODEB = HR_REP;
      EPwm5Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
      EPwm5Regs.HRCNFG.bit.HRLOADB  = HR_CTR_PRD;
      EDIS;

      EPWM_selectDigitalCompareTripInput(EPWM5_BASE, EPWM_DC_TRIP_TRIPIN3, EPWM_DC_TYPE_DCAH);

      EPWM_enableTripZoneSignals(EPWM5_BASE, EPWM_TZ_SIGNAL_DCAEVT1);
      EPWM_enableTripZoneSignals(EPWM5_BASE, EPWM_TZ_SIGNAL_DCBEVT1);

      // DCAEVT1 is generated when DCAH is low
      EPWM_setTripZoneDigitalCompareEventCondition(EPWM5_BASE, EPWM_TZ_DC_OUTPUT_A1, EPWM_TZ_EVENT_DCXH_LOW);

      // DCAEVT1 uses the unfiltered version of DCAEVT1
      EPWM_setDigitalCompareEventSource(EPWM5_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_SOURCE_ORIG_SIGNAL);

      // DCAEVT1 is asynchronous
      EPWM_setDigitalCompareEventSyncMode(EPWM5_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_INPUT_NOT_SYNCED);

      EPWM_setTripZoneAction(EPWM5_BASE, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);

      EPWM_setTripZoneAction(EPWM5_BASE, EPWM_TZ_ACTION_EVENT_TZB, EPWM_TZ_ACTION_LOW);



}

void HRPWM6_Config(void)
{

      //
      EPwm6Regs.TBPRD                = PWM_PERIOD;       // Set timer period
      EPwm6Regs.TBPHS.bit.TBPHS     = 0x0000;           // Phase is 0
      EPwm6Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm6Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm6Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm6Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm6Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm6Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
      EPwm6Regs.CMPA.bit.CMPAHR     =0;
      EPwm6Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
      EPwm6Regs.CMPB.bit.CMPBHR     =0;
      // Set actions
      EPwm6Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm6Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm6Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm6Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm6Regs.AQCTLB.bit.PRD       = AQ_CLEAR;
      EPwm6Regs.AQCTLB.bit.ZRO       = AQ_SET;

      // Active Low PWMs - Setup Deadband
      EPwm6Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm6Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm6Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm6Regs.DBRED.bit.DBRED      = PWM_DEADREDTIME+1;
      EPwm6Regs.DBFED.bit.DBFED      = PWM_DEADFEDTIME+1;

      EPwm6Regs.TBPHS.bit.TBPHS     = 500;
      EPwm6Regs.TBCTL.bit.PHSEN      = TB_ENABLE;       // Disable phase loading

      EPwm6Regs.CMPCTL2.bit.SHDWCMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm6Regs.CMPCTL2.bit.LOADCMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      EPwm6Regs.ETSEL.bit.SOCAEN    = 1;    // Disable SOC on A group
      EPwm6Regs.ETSEL.bit.SOCASEL    = 4;   // Select SOC on up-count
      EPwm6Regs.ETSEL.bit.SOCASELCMP  = 1;
      EPwm6Regs.ETPS.bit.SOCAPRD = 1;       // Generate pulse on 1st event*/
      EPwm6Regs.CMPC =0;

      EALLOW;
      EPwm6Regs.HRCNFG.all = 0x0;
      EPwm6Regs.HRCNFG.bit.EDGMODE = HR_FEP;
      EPwm6Regs.HRCNFG.bit.CTLMODE = HR_CMP;
      EPwm6Regs.HRCNFG.bit.HRLOAD  = HR_CTR_PRD;

      EPwm6Regs.HRCNFG.bit.EDGMODEB = HR_REP;
      EPwm6Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
      EPwm6Regs.HRCNFG.bit.HRLOADB  = HR_CTR_PRD;
      EDIS;

      EPWM_selectDigitalCompareTripInput(EPWM6_BASE, EPWM_DC_TRIP_TRIPIN3, EPWM_DC_TYPE_DCAH);

      EPWM_enableTripZoneSignals(EPWM6_BASE, EPWM_TZ_SIGNAL_DCAEVT1);
      EPWM_enableTripZoneSignals(EPWM6_BASE, EPWM_TZ_SIGNAL_DCBEVT1);

      // DCAEVT1 is generated when DCAH is low
      EPWM_setTripZoneDigitalCompareEventCondition(EPWM6_BASE, EPWM_TZ_DC_OUTPUT_A1, EPWM_TZ_EVENT_DCXH_LOW);

      // DCAEVT1 uses the unfiltered version of DCAEVT1
      EPWM_setDigitalCompareEventSource(EPWM6_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_SOURCE_ORIG_SIGNAL);

      // DCAEVT1 is asynchronous
      EPWM_setDigitalCompareEventSyncMode(EPWM6_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_INPUT_NOT_SYNCED);

      EPWM_setTripZoneAction(EPWM6_BASE, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);

      EPWM_setTripZoneAction(EPWM6_BASE, EPWM_TZ_ACTION_EVENT_TZB, EPWM_TZ_ACTION_LOW);
}

void HRPWM7_Config(void)
{
   // PWMON_Config(1);
    //  EPwm1Regs.TZSEL.all            = 0x0100;
    //  EPwm1Regs.TZCTL.all            = 0x000A;
    //  EPwm1Regs.TZFRC.all            = 0x0004;


      EPwm7Regs.TBPRD                = PWM_PERIOD;             // Set timer period
      EPwm7Regs.TBPHS.bit.TBPHS     =  0x0000;           // Phase is 0
      EPwm7Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm7Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm7Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm7Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm7Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm7Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm7Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm7Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm7Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm7Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
      EPwm7Regs.CMPA.bit.CMPAHR     = 0;
      EPwm7Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
      EPwm7Regs.CMPB.bit.CMPBHR     =  0;
      // Set actions
      EPwm7Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm7Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm7Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm7Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm7Regs.AQCTLB.bit.ZRO       = AQ_SET;
      EPwm7Regs.AQCTLB.bit.PRD       = AQ_CLEAR;

      // Active Low PWMs - Setup Deadband
      EPwm7Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm7Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm7Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm7Regs.DBRED.bit.DBRED                = PWM_DEADREDTIME+1;
      EPwm7Regs.DBFED.bit.DBFED                = PWM_DEADFEDTIME+1;
      EPwm7Regs.TBPHS.bit.TBPHS     = 0;
      EPwm7Regs.TBCTL.bit.PHSEN      = TB_ENABLE;       // Disable phase loading

      EPwm7Regs.CMPCTL2.bit.SHDWCMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm7Regs.CMPCTL2.bit.LOADCMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      EPwm7Regs.ETSEL.bit.SOCAEN    = 1;    // Disable SOC on A group
      EPwm7Regs.ETSEL.bit.SOCASEL    = 4;   // Select SOC on up-count
      EPwm7Regs.ETSEL.bit.SOCASELCMP  = 1;
      EPwm7Regs.ETPS.bit.SOCAPRD = 1;       // Generate pulse on 1st event*/
      EPwm7Regs.CMPC =0;

      EALLOW;
      EPwm7Regs.HRCNFG.all = 0x0;
      EPwm7Regs.HRCNFG.bit.EDGMODE = HR_FEP;
      EPwm7Regs.HRCNFG.bit.CTLMODE = HR_CMP;
      EPwm7Regs.HRCNFG.bit.HRLOAD  = HR_CTR_PRD;

      EPwm7Regs.HRCNFG.bit.EDGMODEB = HR_REP;
      EPwm7Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
      EPwm7Regs.HRCNFG.bit.HRLOADB  = HR_CTR_PRD;
      EDIS;

      EPWM_selectDigitalCompareTripInput(EPWM7_BASE, EPWM_DC_TRIP_TRIPIN6, EPWM_DC_TYPE_DCAH);

      EPWM_enableTripZoneSignals(EPWM7_BASE, EPWM_TZ_SIGNAL_DCAEVT1);
      EPWM_enableTripZoneSignals(EPWM7_BASE, EPWM_TZ_SIGNAL_DCBEVT1);

      // DCAEVT1 is generated when DCAH is low
      EPWM_setTripZoneDigitalCompareEventCondition(EPWM7_BASE, EPWM_TZ_DC_OUTPUT_A1, EPWM_TZ_EVENT_DCXH_LOW);

      // DCAEVT1 uses the unfiltered version of DCAEVT1
      EPWM_setDigitalCompareEventSource(EPWM7_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_SOURCE_ORIG_SIGNAL);

      // DCAEVT1 is asynchronous
      EPWM_setDigitalCompareEventSyncMode(EPWM7_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_INPUT_NOT_SYNCED);

      EPWM_setTripZoneAction(EPWM7_BASE, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);

      EPWM_setTripZoneAction(EPWM7_BASE, EPWM_TZ_ACTION_EVENT_TZB, EPWM_TZ_ACTION_LOW);


}

void HRPWM8_Config(void)
{
   // PWMON_Config(1);
    //  EPwm1Regs.TZSEL.all            = 0x0100;
    //  EPwm1Regs.TZCTL.all            = 0x000A;
    //  EPwm1Regs.TZFRC.all            = 0x0004;


      EPwm8Regs.TBPRD                = PWM_PERIOD;             // Set timer period
      EPwm8Regs.TBPHS.bit.TBPHS     =  0x0000;           // Phase is 0
      EPwm8Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm8Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm8Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm8Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm8Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm8Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm8Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm8Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm8Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm8Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
      EPwm8Regs.CMPA.bit.CMPAHR     =0;
      EPwm8Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
      EPwm8Regs.CMPB.bit.CMPBHR    =0;
      // Set actions
      EPwm8Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm8Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm8Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm8Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm8Regs.AQCTLB.bit.ZRO       = AQ_SET;
      EPwm8Regs.AQCTLB.bit.PRD       = AQ_CLEAR;

      // Active Low PWMs - Setup Deadband
      EPwm8Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm8Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm8Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm8Regs.DBRED.bit.DBRED                = PWM_DEADREDTIME+1;
      EPwm8Regs.DBFED.bit.DBFED                = PWM_DEADFEDTIME+1;
      EPwm8Regs.TBPHS.bit.TBPHS     = 500;
      EPwm8Regs.TBCTL.bit.PHSEN      = TB_ENABLE;       // Disable phase loading

      EPwm8Regs.CMPCTL2.bit.SHDWCMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm8Regs.CMPCTL2.bit.LOADCMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      EPwm8Regs.ETSEL.bit.SOCAEN    = 1;    // Disable SOC on A group
      EPwm8Regs.ETSEL.bit.SOCASEL    = 4;   // Select SOC on up-count
      EPwm8Regs.ETSEL.bit.SOCASELCMP  = 1;
      EPwm8Regs.ETPS.bit.SOCAPRD = 1;       // Generate pulse on 1st event*/
      EPwm8Regs.CMPC =0;

      EALLOW;
      EPwm8Regs.HRCNFG.all = 0x0;
      EPwm8Regs.HRCNFG.bit.EDGMODE = HR_FEP;
      EPwm8Regs.HRCNFG.bit.CTLMODE = HR_CMP;
      EPwm8Regs.HRCNFG.bit.HRLOAD  = HR_CTR_PRD;

      EPwm8Regs.HRCNFG.bit.EDGMODEB = HR_REP;
      EPwm8Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
      EPwm8Regs.HRCNFG.bit.HRLOADB  = HR_CTR_PRD;
      EDIS;

      EPWM_selectDigitalCompareTripInput(EPWM8_BASE, EPWM_DC_TRIP_TRIPIN6, EPWM_DC_TYPE_DCAH);

      EPWM_enableTripZoneSignals(EPWM8_BASE, EPWM_TZ_SIGNAL_DCAEVT1);
      EPWM_enableTripZoneSignals(EPWM8_BASE, EPWM_TZ_SIGNAL_DCBEVT1);

      // DCAEVT1 is generated when DCAH is low
      EPWM_setTripZoneDigitalCompareEventCondition(EPWM8_BASE, EPWM_TZ_DC_OUTPUT_A1, EPWM_TZ_EVENT_DCXH_LOW);

      // DCAEVT1 uses the unfiltered version of DCAEVT1
      EPWM_setDigitalCompareEventSource(EPWM8_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_SOURCE_ORIG_SIGNAL);

      // DCAEVT1 is asynchronous
      EPWM_setDigitalCompareEventSyncMode(EPWM8_BASE, EPWM_DC_MODULE_A, EPWM_DC_EVENT_1, EPWM_DC_EVENT_INPUT_NOT_SYNCED);

      EPWM_setTripZoneAction(EPWM8_BASE, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);

      EPWM_setTripZoneAction(EPWM8_BASE, EPWM_TZ_ACTION_EVENT_TZB, EPWM_TZ_ACTION_LOW);


}

void PWM9_Config(void)
{
      EPwm9Regs.TZCLR.all=0x0007;
      EPwm9Regs.TBPRD                = 10000;       // Set timer period
      EPwm9Regs.TBPHS.bit.TBPHS     = 0x0000;           // Phase is 0
      EPwm9Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm9Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm9Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm9Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm9Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm9Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm9Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm9Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm9Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm9Regs.CMPA.bit.CMPA       = 10000;
      EPwm9Regs.CMPB.bit.CMPB       = 0;
      // Set actions
      EPwm9Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm9Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm9Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm9Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm9Regs.AQCTLB.bit.PRD       = AQ_CLEAR;
      EPwm9Regs.AQCTLB.bit.ZRO       = AQ_SET;

      // Active Low PWMs - Setup Deadband
      EPwm9Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm9Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm9Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm9Regs.DBRED.bit.DBRED      = PWM_DEADREDTIME+1;
      EPwm9Regs.DBFED.bit.DBFED      = PWM_DEADFEDTIME+1;

      EPwm9Regs.TBPHS.bit.TBPHS     = 0;
      EPwm9Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
}


void PWM10_Config(void)
{

      EPwm10Regs.TZCLR.all=0x0007;
      EPwm10Regs.TBPRD                = 10000;       // Set timer period
      EPwm10Regs.TBPHS.bit.TBPHS     = 0x0000;           // Phase is 0
      EPwm10Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm10Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm10Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm10Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm10Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm10Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm10Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm10Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm10Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm10Regs.CMPA.bit.CMPA       = 10000;
      EPwm10Regs.CMPB.bit.CMPB       = 0;
      // Set actions
      EPwm10Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm10Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm10Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm10Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm10Regs.AQCTLB.bit.PRD       = AQ_CLEAR;
      EPwm10Regs.AQCTLB.bit.ZRO       = AQ_SET;

      // Active Low PWMs - Setup Deadband
      EPwm10Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm10Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm10Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm10Regs.DBRED.bit.DBRED      = PWM_DEADREDTIME+1;
      EPwm10Regs.DBFED.bit.DBFED      = PWM_DEADFEDTIME+1;

      EPwm10Regs.TBPHS.bit.TBPHS     = 0;
      EPwm10Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading

}


void PWM11_Config(void)
{

      EPwm11Regs.TZCLR.all=0x0007;
      EPwm11Regs.TBPRD                = 10000;       // Set timer period
      EPwm11Regs.TBPHS.bit.TBPHS     = 0x0000;           // Phase is 0
      EPwm11Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm11Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm11Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm11Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm11Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm11Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm11Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm11Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm11Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm11Regs.CMPA.bit.CMPA       = 10000;
      EPwm11Regs.CMPB.bit.CMPB       = 0;
      // Set actions
      EPwm11Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm11Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm11Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm11Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm11Regs.AQCTLB.bit.PRD       = AQ_CLEAR;
      EPwm11Regs.AQCTLB.bit.ZRO       = AQ_SET;

      // Active Low PWMs - Setup Deadband
      EPwm11Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm11Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm11Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm11Regs.DBRED.bit.DBRED      = PWM_DEADREDTIME+1;
      EPwm11Regs.DBFED.bit.DBFED      = PWM_DEADFEDTIME+1;

      EPwm11Regs.TBPHS.bit.TBPHS     = 0;
      EPwm11Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading

}

void PWM12_Config(void)
{
     // EPwm11Regs.TZSEL.all            = 0x0100;
    //  EPwm11Regs.TZCTL.all            = 0x000A;
    //  EPwm11Regs.TZFRC.all            = 0x0004;
      EPwm12Regs.TZCLR.all=0x0007;
      EPwm12Regs.TBPRD                = 10000;       // Set timer period
      EPwm12Regs.TBPHS.bit.TBPHS     = 0x0000;           // Phase is 0
      EPwm12Regs.TBCTR                = 0x0000;           // Clear counter

      // Setup TBCLK
      EPwm12Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
      EPwm12Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
      EPwm12Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
      EPwm12Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

      EPwm12Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
      EPwm12Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
      EPwm12Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
      EPwm12Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

      // Setup compare
      EPwm12Regs.CMPA.bit.CMPA       =10000 ;
      EPwm12Regs.CMPB.bit.CMPB       = 0;
      // Set actions
      EPwm12Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
      EPwm12Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
      EPwm12Regs.AQCTLA.bit.ZRO       = AQ_SET;
      EPwm12Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
      EPwm12Regs.AQCTLB.bit.PRD       = AQ_CLEAR;
      EPwm12Regs.AQCTLB.bit.ZRO       = AQ_SET;

      // Active Low PWMs - Setup Deadband
      EPwm12Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
      EPwm12Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
      EPwm12Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

      EPwm12Regs.DBRED.bit.DBRED      = PWM_DEADREDTIME+1;
      EPwm12Regs.DBFED.bit.DBFED      = PWM_DEADFEDTIME+1;

      EPwm12Regs.TBPHS.bit.TBPHS     = 0;
      EPwm12Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading

}




void PWMON_Config(Uint16 CH)
{
    EALLOW;
   //(*ePWM[CH]).TZSEL.all=0x0000;

 //   (*ePWM[CH]).TZCLR.bit.OST=1;
  //  (*ePWM[CH]).TZFRC.bit.OST=0;
    (*ePWM[CH]).TZCLR.all=0x0007;
    (*ePWM[CH]).TZFRC.all=0;
    //   (*ePWM[CH]).TZFRC.all=0x0000;
  //  (*ePWM[CH]).TZCLR.bit.OST=1;
    EDIS;
}

void PWMOFF_Config(Uint16 CH)
{
    EALLOW;


  //  (*ePWM[CH]).TZSEL.all=0xC100;
    (*ePWM[CH]).TZCTL.all=0x000A;
    (*ePWM[CH]).TZFRC.all=0x0004;//0x0028;

   /* (*ePWM[CH]).TZSEL.bit.DCAEVT1=1;
    (*ePWM[CH]).TZSEL.bit.DCBEVT1=1;

    (*ePWM[CH]).TZCTL.bit.DCAEVT1 =2;
    (*ePWM[CH]).TZCTL.bit.DCBEVT1=2;*/
   // (*ePWM[CH]).TZFRC.bit.DCAEVT1   = 1;
   // (*ePWM[CH]).TZFRC.bit.DCBEVT1  = 1;

   /* (*ePWM[CH]).TZSEL.bit.OSHT1=1;
    (*ePWM[CH]).TZCTL.bit.TZA=2;
    (*ePWM[CH]).TZCTL.bit.TZB=2;*/
   // (*ePWM[CH]).TZFRC.bit.OST=1;
    EDIS;
    (*ePWM[CH]).CMPA.bit.CMPA=PWM_DEADREDTIME;
   (*ePWM[CH]).CMPB.bit.CMPB=PWM_PERIOD;
}

void setupHrpwmMepScaleFactor(void)
{
        do {
            BTS_sfoStatus = SFO();
        } while(BTS_sfoStatus == SFO_INCOMPLETE);


}
