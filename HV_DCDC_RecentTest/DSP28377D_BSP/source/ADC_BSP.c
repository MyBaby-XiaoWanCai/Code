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
  AdcSetMode(ADC_ADCC, ADC_RESOLUTION_16BIT, ADC_SIGNALMODE_DIFFERENTIAL);
  AdcSetMode(ADC_ADCD, ADC_RESOLUTION_16BIT, ADC_SIGNALMODE_DIFFERENTIAL);
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
  //
  //delay for 1ms to allow ADC time to power up
  //
  DELAY_US(1000);

  EDIS;
}



void SetupADCSoftware(void)
{
    Uint16 acqps;
    Uint16 bcqps;

    //determine minimum acquisition window (in SYSCLKS) based on resolution
    //
    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION)
    {
        acqps = 14; //75ns
    }
    else //resolution is 16-bit
    {
        bcqps = 63; //320ns
    }
    if(ADC_RESOLUTION_12BIT == AdcbRegs.ADCCTL2.bit.RESOLUTION)
    {
        acqps = 14; //75ns
    }
    else //resolution is 16-bit
    {
        bcqps = 63; //320ns
    }
    if(ADC_RESOLUTION_12BIT == AdccRegs.ADCCTL2.bit.RESOLUTION)
    {
        acqps = 14; //75ns
    }
    else //resolution is 16-bit
    {
        bcqps = 63; //320ns
    }
    if(ADC_RESOLUTION_12BIT == AdcdRegs.ADCCTL2.bit.RESOLUTION)
    {
        acqps = 14; //75ns
    }
    else //resolution is 16-bit
    {
        bcqps = 63; //320ns
    }
    //
    //Select the channels to convert and end of conversion flag
    //ADCA
    //
    EALLOW;
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC0 will convert pin A0
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is acqps +
                                           //1 SYSCLK cycles
    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 1;  //SOC1 will convert pin A1
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is acqps +
                                           //1 SYSCLK cycles
    AdcaRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC1 will convert pin A2
    AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is acqps +

    AdcaRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC1 will convert pin A3
    AdcaRegs.ADCSOC3CTL.bit.ACQPS = acqps; //sample window is acqps +

    AdcaRegs.ADCSOC4CTL.bit.CHSEL = 4;  //SOC1 will convert pin A4
    AdcaRegs.ADCSOC4CTL.bit.ACQPS = acqps; //sample window is acqps +

    AdcaRegs.ADCSOC5CTL.bit.CHSEL = 5;  //SOC1 will convert pin A1
    AdcaRegs.ADCSOC5CTL.bit.ACQPS = acqps; //sample window is acqps +

    AdcaRegs.ADCSOC14CTL.bit.CHSEL = 14;  //SOC1 will convert pin A14
    AdcaRegs.ADCSOC14CTL.bit.ACQPS = acqps; //sample window is acqps +

    AdcaRegs.ADCSOC15CTL.bit.CHSEL = 15;  //SOC1 will convert pin A1
    AdcaRegs.ADCSOC15CTL.bit.ACQPS = acqps; //sample window is acqps +

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 15; //end of SOC15 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    //ADCB
    AdcbRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC0 will convert pin B0
    AdcbRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is acqps +

    AdcbRegs.ADCSOC1CTL.bit.CHSEL = 1;  //SOC0 will convert pin B1
    AdcbRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is acqps +

    AdcbRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC0 will convert pin B2
    AdcbRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is acqps +

    AdcbRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC0 will convert pin B3
    AdcbRegs.ADCSOC3CTL.bit.ACQPS = acqps; //sample window is acqps +

    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL = 3; //end of SOC1 will set INT1 flag
    AdcbRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

    AdccRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC1 will convert pin C2
    AdccRegs.ADCSOC2CTL.bit.ACQPS = bcqps; //sample window is acqps +
    AdccRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC1 will convert pin C2
    AdccRegs.ADCSOC3CTL.bit.ACQPS = bcqps; //sample window is acqps +

    AdccRegs.ADCINTSEL1N2.bit.INT1SEL = 3; //end of SOC15 will set INT1 flag
    AdccRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

    AdcdRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC1 will convert pin D0
    AdcdRegs.ADCSOC0CTL.bit.ACQPS = bcqps; //sample window is acqps +
   // AdcdRegs.ADCSOC1CTL.bit.CHSEL = 1;  //SOC1 will convert pin D0
   // AdcdRegs.ADCSOC1CTL.bit.ACQPS = bcqps; //sample window is acqps +
    AdcdRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC1 will convert pin D1
    AdcdRegs.ADCSOC2CTL.bit.ACQPS = bcqps; //sample window is acqps +
  //  AdcdRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC1 will convert pin D1
  //  AdcdRegs.ADCSOC3CTL.bit.ACQPS = bcqps; //sample window is acqps +

    AdcdRegs.ADCINTSEL1N2.bit.INT1SEL = 2; //end of SOC15 will set INT1 flag
    AdcdRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcdRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

    EDIS;
}


void InitADC_BSP(void)
{
    //初始化ADC
    ConfigureADC();

    //设置ADC为软件触发
    SetupADCSoftware();

}
