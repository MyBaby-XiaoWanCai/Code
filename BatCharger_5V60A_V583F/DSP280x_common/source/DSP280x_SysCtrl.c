// TI File $Revision: /main/4 $
// Checkin $Date: September 8, 2005   17:32:45 $
//###########################################################################
//
// FILE:   DSP280x_SysCtrl.c
//
// TITLE:  DSP280x Device System Control Initialization & Support Functions.
//
// DESCRIPTION:
//
//         Example initialization of system resources.
//
//###########################################################################
// $TI Release: DSP280x V1.30 $
// $Release Date: February 10, 2006 $
//###########################################################################


#include "..\..\DSP280x_headers\include\DSP280x_Device.h"     // Headerfile Include File
#include "..\include\DSP280x_Examples.h"   // Examples Include File

// Functions that will be run from RAM need to be assigned to 
// a different section.  This section will then be mapped to a load and 
// run address using the linker cmd file.

#pragma CODE_SECTION(InitFlash, "ramfuncs");

//---------------------------------------------------------------------------
// InitSysCtrl: 
//---------------------------------------------------------------------------
// This function initializes the System Control registers to a known state.
// - Disables the watchdog
// - Set the PLLCR for proper SYSCLKOUT frequency 
// - Set the pre-scaler for the high and low frequency peripheral clocks
// - Enable the clocks to the peripherals

void InitSysCtrl(void)
{

   // Disable the watchdog        
   DisableDog();
   
   // Initialize the PLLCR to 0x04
   InitPll(0x04);//InitPll(0xA);

   // Initialize the peripheral clocks
   InitPeripheralClocks();
}


//---------------------------------------------------------------------------
// Example: InitFlash: 
//---------------------------------------------------------------------------
// This function initializes the Flash Control registers

//                   CAUTION 
// This function MUST be executed out of RAM. Executing it
// out of OTP/Flash will yield unpredictable results

void InitFlash(void)
{
   EALLOW;
   //Enable Flash Pipeline mode to improve performance
   //of code executed from Flash.
   FlashRegs.FOPT.bit.ENPIPE = 1;
   
   //                CAUTION
   //Minimum waitstates required for the flash operating
   //at a given CPU rate must be characterized by TI. 
   //Refer to the datasheet for the latest information.  

   //Set the Paged Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.PAGEWAIT = 3;

   //Set the Random Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.RANDWAIT = 3;
   
   //Set the Waitstate for the OTP
   FlashRegs.FOTPWAIT.bit.OTPWAIT = 5;
   
   //                CAUTION
   //ONLY THE DEFAULT VALUE FOR THESE 2 REGISTERS SHOULD BE USED
   FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF;       
   FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF;   
   EDIS;

   //Force a pipeline flush to ensure that the write to 
   //the last register configured occurs before returning.  

   asm(" RPT #7 || NOP");
}	


//---------------------------------------------------------------------------
// Example: ServiceDog: 
//---------------------------------------------------------------------------
// This function resets the watchdog timer.
// Enable this function for using ServiceDog in the application 
extern Uint16 Req_DogReset;
void ServiceDog(void)
{
    if(Req_DogReset!=0) return;

    EALLOW;
    SysCtrlRegs.WDKEY = 0x0055;
    SysCtrlRegs.WDKEY = 0x00AA;
    EDIS;
}

//---------------------------------------------------------------------------
// Example: DisableDog: 
//---------------------------------------------------------------------------
// This function disables the watchdog timer.

void DisableDog(void)
{
    EALLOW;
    SysCtrlRegs.WDCR= 0x0068;
    EDIS;
}

//---------------------------------------------------------------------------
// Example: InitPll: 
//---------------------------------------------------------------------------
// This function initializes the PLLCR register.

void InitPll(Uint16 val)
{
   volatile Uint16 iVol;   

   // Make sure the PLL is not running in limp mode
   if (SysCtrlRegs.PLLSTS.bit.MCLKSTS != 0)
   {
      // Missing external clock has been detected
      // Replace this line with a call to an appropriate
      // SystemShutdown(); function. 
      asm("        ESTOP0");
   }

   // CLKINDIV MUST be 0 before PLLCR can be changed from
   // 0x0000. It is set to 0 by an external reset XRSn
   if (SysCtrlRegs.PLLSTS.bit.CLKINDIV != 0)
   {
       SysCtrlRegs.PLLSTS.bit.CLKINDIV = 0;
   }
   
   // Change the PLLCR
   if (SysCtrlRegs.PLLCR.bit.DIV != val)
   {
       
      EALLOW;
      // Before setting PLLCR turn off missing clock detect logic
      SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;
      SysCtrlRegs.PLLCR.bit.DIV = val;
      EDIS;
   
      // Optional: Wait for PLL to lock.
      // During this time the CPU will switch to OSCCLK/2 until
      // the PLL is stable.  Once the PLL is stable the CPU will 
      // switch to the new PLL value. 
      //
      // This time-to-lock is monitored by a PLL lock counter.   
      //   
      // Code is not required to sit and wait for the PLL to lock.   
      // However, if the code does anything that is timing critical, 
      // and requires the correct clock be locked, then it is best to 
      // wait until this switching has completed.  
  
      // Wait for the PLL lock bit to be set.  
      // Note this bit is not available on 281x devices.  For those devices
      // use a software loop to perform the required count. 

      // The watchdog should be disabled before this loop, or fed within 
      // the loop via ServiceDog().   
      
	  // Uncomment to disable the watchdog
      DisableDog();
   
      while(SysCtrlRegs.PLLSTS.bit.PLLLOCKS != 1) 
      {
	      // Uncomment to service the watchdog
          // ServiceDog();
      }
       
      EALLOW;
      SysCtrlRegs.PLLSTS.bit.MCLKOFF = 0;
      EDIS;   
    }
}

//--------------------------------------------------------------------------
// Example: InitPeripheralClocks: 
//---------------------------------------------------------------------------
// This function initializes the clocks to the peripheral modules.
// First the high and low clock prescalers are set
// Second the clocks are enabled to each peripheral.
// To reduce power, leave clocks to unused peripherals disabled
//
// Note: If a peripherals clock is not enabled then you cannot 
// read or write to the registers for that peripheral 

void InitPeripheralClocks(void)
{
   EALLOW;

// HISPCP/LOSPCP prescale register settings, normally it will be set to default values
   SysCtrlRegs.HISPCP.all = 0x0000;
   SysCtrlRegs.LOSPCP.all = 0x0000;

// XCLKOUT to SYSCLKOUT ratio.  By default XCLKOUT = 1/4 SYSCLKOUT
   SysCtrlRegs.XCLK.bit.XCLKOUTDIV=2;
      	
// Peripheral clock enables set for the selected peripherals.   
// If you are not using a peripheral leave the clock off
// to save on power. 
// 
// Note: not all peripherals are available on all 280x derivates.
// Refer to the datasheet for your particular device. 

   SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;    // ADC
   SysCtrlRegs.PCLKCR0.bit.I2CAENCLK = 1;   // I2C
   SysCtrlRegs.PCLKCR0.bit.ECANAENCLK=1;    // eCAN-A
   SysCtrlRegs.PCLKCR1.bit.ECAP1ENCLK = 1;  // eCAP1
   SysCtrlRegs.PCLKCR1.bit.ECAP2ENCLK = 1;  // eCAP2
   SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;  // ePWM1
   SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;  // ePWM2
   SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1;  // ePWM3
   SysCtrlRegs.PCLKCR1.bit.EQEP1ENCLK = 1;  // eQEP1
   SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;   // SCI-A
   SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;   // SPI-A
   SysCtrlRegs.PCLKCR0.bit.SPIBENCLK = 1;   // SPI-B

   if(DevEmuRegs.PARTID.bit.PARTNO == PARTNO_2808 || 
      DevEmuRegs.PARTID.bit.PARTNO == PARTNO_2806 )
   {
	   SysCtrlRegs.PCLKCR1.bit.ECAP3ENCLK = 1;  // eCAP3
	   SysCtrlRegs.PCLKCR1.bit.ECAP4ENCLK = 1;  // eCAP4
	   SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK = 1;  // ePWM4
	   SysCtrlRegs.PCLKCR1.bit.EPWM5ENCLK = 1;  // ePWM5
	   SysCtrlRegs.PCLKCR1.bit.EPWM6ENCLK = 1;  // ePWM6
	   SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 1;   // SCI-B
	   SysCtrlRegs.PCLKCR0.bit.SPICENCLK = 1;   // SPI-C
	   SysCtrlRegs.PCLKCR0.bit.SPIDENCLK = 1;   // SPI-D
	   SysCtrlRegs.PCLKCR1.bit.EQEP2ENCLK = 1;  // eQEP2
 
   }
   if(DevEmuRegs.PARTID.bit.PARTNO == PARTNO_2808)
   {  
	   SysCtrlRegs.PCLKCR0.bit.ECANBENCLK=1;    // eCAN-B
   }

   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;       // Enable TBCLK within the ePWM  
                           
   EDIS;
}

	
//===========================================================================
// End of file.
//===========================================================================
