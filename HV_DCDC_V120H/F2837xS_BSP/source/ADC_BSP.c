/*
 * ADC_BSP.c
 *
 *  Created on: 2021年6月9日
 *      Author: JC
 */
#include "F28x_Project.h"

void ConfigureADC(void)
{
  EALLOW;

  //
  //write configurations
  //
  AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
  AdcbRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
  AdccRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
  AdcdRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
  AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
  AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
  AdcSetMode(ADC_ADCC, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
  AdcSetMode(ADC_ADCD, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
  //
  //Set pulse positions to late
  //
  AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
  AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;
  AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;
  AdcdRegs.ADCCTL1.bit.INTPULSEPOS = 1;
  //
  //power up the ADCs
  //
  AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
  AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;
  AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;
  AdcdRegs.ADCCTL1.bit.ADCPWDNZ = 1;

  AdcaRegs.ADCINTSEL1N2.bit.INT1CONT=1;
  AdcaRegs.ADCINTSEL1N2.bit.INT2CONT=1;
  AdcaRegs.ADCINTSEL3N4.bit.INT3CONT=1;
  AdcaRegs.ADCINTSEL3N4.bit.INT4CONT=1;
  //
  //delay for 1ms to allow ADC time to power up
  //
  DELAY_US(1000);

  EDIS;
}



void SetupADCSoftware(void)
{
    Uint16 acqps;

    //determine minimum acquisition window (in SYSCLKS) based on resolution
    //
    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION)
    {
        acqps = 14; //75ns
    }
    else //resolution is 16-bit
    {
        acqps = 63; //320ns
    }
    if(ADC_RESOLUTION_12BIT == AdcbRegs.ADCCTL2.bit.RESOLUTION)
    {
        acqps = 14; //75ns
    }
    else //resolution is 16-bit
    {
        acqps = 63; //320ns
    }
    if(ADC_RESOLUTION_12BIT == AdccRegs.ADCCTL2.bit.RESOLUTION)
    {
        acqps = 14; //75ns
    }
    else //resolution is 16-bit
    {
        acqps = 63; //320ns
    }
    if(ADC_RESOLUTION_12BIT == AdcdRegs.ADCCTL2.bit.RESOLUTION)
    {
        acqps = 14; //75ns
    }
    else //resolution is 16-bit
    {
        acqps = 63; //320ns
    }
    //
    //Select the channels to convert and end of conversion flag
    //ADCA
    //
    acqps=100;
    EALLOW;

      AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC0 will convert pin A0
      AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
      AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5; //trigger on Timer

      AdcaRegs.ADCSOC1CTL.bit.CHSEL = 1;  //SOC1 will convert pin A1
      AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
      AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 7; //trigger on Timer

      AdcaRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC2 will convert pin A2
      AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
      AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = 9; //trigger on Timer

      AdcaRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC3 will convert pin A3
      AdcaRegs.ADCSOC3CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
      AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = 11; //trigger on Timer

      AdcaRegs.ADCSOC4CTL.bit.CHSEL = 4;  //SOC4 will convert pin A4
      AdcaRegs.ADCSOC4CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
      AdcaRegs.ADCSOC4CTL.bit.TRIGSEL = 13; //trigger on Timer

      AdcaRegs.ADCSOC5CTL.bit.CHSEL = 5;  //SOC5 will convert pin A5
      AdcaRegs.ADCSOC5CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
      AdcaRegs.ADCSOC5CTL.bit.TRIGSEL = 15; //trigger on Timer

      AdcaRegs.ADCSOC14CTL.bit.CHSEL = 14;  //SOC14 will convert pin A14
      AdcaRegs.ADCSOC14CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
      AdcaRegs.ADCSOC14CTL.bit.TRIGSEL = 17; //trigger on Timer

      AdcaRegs.ADCSOC15CTL.bit.CHSEL = 15;  //SOC15 will convert pin A15
      AdcaRegs.ADCSOC15CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
      AdcaRegs.ADCSOC15CTL.bit.TRIGSEL = 19; //trigger on Timer

      AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC0 will set INT1 flag
      AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
      AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

      AdcaRegs.ADCINTSEL1N2.bit.INT2SEL = 3; //end of SOC0 will set INT1 flag
      AdcaRegs.ADCINTSEL1N2.bit.INT2E = 1;   //enable INT1 flag
      AdcaRegs.ADCINTFLGCLR.bit.ADCINT2 = 1; //make sure INT1 flag is cleared

      AdcaRegs.ADCINTSEL3N4.bit.INT3SEL = 5; //end of SOC0 will set INT1 flag
      AdcaRegs.ADCINTSEL3N4.bit.INT3E = 1;   //enable INT1 flag
      AdcaRegs.ADCINTFLGCLR.bit.ADCINT3 = 1; //make sure INT1 flag is cleared


      AdcaRegs.ADCINTSEL3N4.bit.INT4SEL = 15; //end of SOC0 will set INT1 flag
      AdcaRegs.ADCINTSEL3N4.bit.INT4E = 1;   //enable INT1 flag
      AdcaRegs.ADCINTFLGCLR.bit.ADCINT4 = 1; //make sure INT1 flag is cleared


      //ADCB
   AdcbRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC0 will convert pin B0
   AdcbRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is acqps +
   AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = 0; //trigger on Timer


   AdcbRegs.ADCSOC1CTL.bit.CHSEL = 1;  //SOC0 will convert pin B1
   AdcbRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is acqps +
   AdcbRegs.ADCSOC1CTL.bit.TRIGSEL = 0; //trigger on Timer

   AdcbRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC0 will convert pin B2
   AdcbRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is acqps +
   AdcbRegs.ADCSOC2CTL.bit.TRIGSEL = 0; //trigger on Timer

   AdcbRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC0 will convert pin B3
   AdcbRegs.ADCSOC3CTL.bit.ACQPS = acqps; //sample window is acqps +
   AdcbRegs.ADCSOC3CTL.bit.TRIGSEL =0; //trigger on Timer



   AdccRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC1 will convert pin C2
   AdccRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is acqps +
   AdccRegs.ADCSOC2CTL.bit.TRIGSEL = 0; //trigger on Timer

   AdccRegs.ADCSOC4CTL.bit.CHSEL = 4;  //SOC1 will convert pin C2
   AdccRegs.ADCSOC4CTL.bit.ACQPS = acqps; //sample window is acqps +
   AdccRegs.ADCSOC4CTL.bit.TRIGSEL = 0; //trigger on Timer



   AdcdRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC1 will convert pin D0
   AdcdRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is acqps +
   AdcdRegs.ADCSOC0CTL.bit.TRIGSEL = 0; //trigger on Timer

   AdcdRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC1 will convert pin D1
   AdcdRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is acqps +
   AdcdRegs.ADCSOC2CTL.bit.TRIGSEL = 0; //trigger on Timer

   AdcdRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC1 will convert pin D1
   AdcdRegs.ADCSOC3CTL.bit.ACQPS = acqps; //sample window is acqps +
   AdcdRegs.ADCSOC3CTL.bit.TRIGSEL = 0; //trigger on Timer
  EDIS;
}


void InitADC_BSP(void)
{
    //初始化ADC
    ConfigureADC();

    //设置ADC为软件触发
    SetupADCSoftware();

}
