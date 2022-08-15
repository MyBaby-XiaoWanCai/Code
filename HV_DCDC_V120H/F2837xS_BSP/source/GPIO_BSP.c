/*
 * GPIO_BSP.c
 *
 *  Created on: 2021年6月5日
 *      Author: JC
 */
#include "F28x_Project.h"


void InitGPIO_BSP(void)
{

   //AD7606
    GPIO_SetupPinMux(32, GPIO_MUX_CPU1, 0);//converst转换
    GPIO_SetupPinOptions(32, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(33, GPIO_MUX_CPU1, 0);//busy信号
    GPIO_SetupPinOptions(33, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(34, GPIO_MUX_CPU1, 0);//AD7606复位
    GPIO_SetupPinOptions(34, GPIO_OUTPUT, GPIO_PUSHPULL);//

  //SPI
    EALLOW;
    GpioCtrlRegs.GPBPUD.bit.GPIO50 = 0;   // Enable pull-up on GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPBPUD.bit.GPIO51 = 0;   // Enable pull-up on GPIO17 (SPISOMIA)

    GpioCtrlRegs.GPBPUD.bit.GPIO52 = 0;   // Enable pull-up on GPIO18 (SPICLKA)
    GpioCtrlRegs.GPBPUD.bit.GPIO53 = 0;   // Enable pull-up on GPIO19 (SPISTEA)

    GpioCtrlRegs.GPBQSEL2.bit.GPIO50 = 3; // Asynch input GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO51 = 3; // Asynch input GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO52 = 3; // Asynch input GPIO18 (SPICLKA)


   /* Configure SPI-A pins using GPIO regs*/
   // This specifies which of the possible GPIO pins will be SPI functional pins.
   // Comment out other unwanted lines.

    GpioCtrlRegs.GPBGMUX2.bit.GPIO50 = 1; // Configure GPIO16 as SPISIMOA
    GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 2;

    GpioCtrlRegs.GPBGMUX2.bit.GPIO51 = 1; // Configure GPIO17 as SPISOMIA
    GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 2;

    GpioCtrlRegs.GPBGMUX2.bit.GPIO52 = 1; // Configure GPIO18 as SPICLKA
    GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 2;

    GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO53 = 1;
    GpioDataRegs.GPBSET.bit.GPIO53 = 1;

    EDIS;

    //SCI
    GPIO_SetupPinMux(49, GPIO_MUX_CPU1, 6);
    GPIO_SetupPinOptions(49, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(48, GPIO_MUX_CPU1, 6);
    GPIO_SetupPinOptions(48, GPIO_OUTPUT, GPIO_PUSHPULL);

    //TZ硬件故障锁存
    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO24 = 0;          // input
    GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 1;        // XINT1 Synch to SYSCLKOUT only

   // GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;         // GPIO
  //  GpioCtrlRegs.GPADIR.bit.GPIO24 = 0;          // input



    GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO25 = 0;          // input
    GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 1;        // XINT1 Synch to SYSCLKOUT only

    GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO26 = 0;          // input
    GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 1;        // XINT1 Synch to SYSCLKOUT only

    GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO27 = 0;          // input
    GpioCtrlRegs.GPAQSEL2.bit.GPIO27 = 1;        // XINT1 Synch to SYSCLKOUT only

    GpioCtrlRegs.GPACTRL.bit.QUALPRD3=0xFF;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO24=2;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO25=2;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO26=2;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO27=2;


    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO29 = 0;          // input
    GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 1;        // XINT1 Synch to SYSCLKOUT only

    EDIS;

    EALLOW;
    InputXbarRegs.INPUT1SELECT = 24;       //Set XINT1 source to GPIO-pin
    InputXbarRegs.INPUT2SELECT = 25;       //Set XINT1 source to GPIO-pin
    InputXbarRegs.INPUT3SELECT = 26;       //Set XINT1 source to GPIO-pin
    InputXbarRegs.INPUT6SELECT = 27;       //Set XINT1 source to GPIO-pin
    EDIS;

// GPIO防反接开关
    EALLOW;
    GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO44 = 1;
    GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO45 = 1;
    GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO46 = 1;
    GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO47 = 1;

/*
    GpioCtrlRegs.GPCMUX2.bit.GPIO89 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO89=1;
    GpioCtrlRegs.GPCCSEL4.bit.GPIO89=1;
    GpioCtrlRegs.GPCMUX2.bit.GPIO90 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO90=1;
    GpioCtrlRegs.GPCCSEL4.bit.GPIO90=1;
    GpioCtrlRegs.GPCMUX2.bit.GPIO91 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO91=1;
    GpioCtrlRegs.GPCCSEL4.bit.GPIO91=1;
    GpioCtrlRegs.GPCMUX2.bit.GPIO92 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO92=1;
    GpioCtrlRegs.GPCCSEL4.bit.GPIO92=1;*/
    EDIS;
    Rev_Port1_OFF;
    Rev_Port2_OFF;
    Rev_Port3_OFF;
    Rev_Port4_OFF;
    //LED设置
    EALLOW;



   GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0;
   GpioCtrlRegs.GPBDIR.bit.GPIO54 = 1; //LED1 red
   GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;
   GpioCtrlRegs.GPBDIR.bit.GPIO55 = 1; //LED1 green

   GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0;
   GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1; //LED2 red
   GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;
   GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1; //LED2 green

   GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO64 = 1; //LED3 red
   GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO65 = 1; //LED3 green

   GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO66 = 1; //LED4 red
   GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1; //LED4 green

   GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO86 = 1; //LED_RUN red
   GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO87 = 1; //LED_RUN green

   GpioCtrlRegs.GPCMUX2.bit.GPIO88 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO88 = 1; //DSP闪烁灯


 /*GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;
   GpioCtrlRegs.GPBDIR.bit.GPIO55=1;
   GpioCtrlRegs.GPBCSEL3.bit.GPIO55=1;
   GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;
   GpioCtrlRegs.GPBDIR.bit.GPIO57=1;
   GpioCtrlRegs.GPBCSEL4.bit.GPIO57=1;
   GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO65=1;
   GpioCtrlRegs.GPCCSEL1.bit.GPIO65=1;
   GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO67=1;
   GpioCtrlRegs.GPCCSEL1.bit.GPIO67=1;
 */

   EDIS;

 //风机故障输入
   EALLOW;
   GpioCtrlRegs.GPCPUD.bit.GPIO89 = 0;//fan1
   GpioCtrlRegs.GPCMUX2.bit.GPIO89 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO89=0;
   GpioCtrlRegs.GPCPUD.bit.GPIO90 = 0;//fan2
   GpioCtrlRegs.GPCMUX2.bit.GPIO90 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO90=0;
   GpioCtrlRegs.GPCPUD.bit.GPIO91 = 0;//fan3
   GpioCtrlRegs.GPCMUX2.bit.GPIO91 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO91=0;
   GpioCtrlRegs.GPCPUD.bit.GPIO92 = 0;//fan4
   GpioCtrlRegs.GPCMUX2.bit.GPIO92 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO92=0;
   GpioCtrlRegs.GPCPUD.bit.GPIO93 = 0;//fan5
   GpioCtrlRegs.GPCMUX2.bit.GPIO93 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO93=0;
   GpioCtrlRegs.GPCPUD.bit.GPIO94 = 0;//fan6
   GpioCtrlRegs.GPCMUX2.bit.GPIO94 = 0;
   GpioCtrlRegs.GPCDIR.bit.GPIO94=0;
   GpioCtrlRegs.GPDPUD.bit.GPIO99 = 0;//fan7
   GpioCtrlRegs.GPDMUX1.bit.GPIO99 = 0;
   GpioCtrlRegs.GPDDIR.bit.GPIO99=0;
   GpioCtrlRegs.GPEPUD.bit.GPIO133 = 0;//fan8
   GpioCtrlRegs.GPEMUX1.bit.GPIO133 = 0;
   GpioCtrlRegs.GPEDIR.bit.GPIO133=0;
   EDIS;

}

void ExintInit(void)
{
    //AD7124转换完成信号
    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO29 = 0;          // input
    GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 1;        // XINT1 Synch to SYSCLKOUT only
   /*
   GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 0;         // GPIO
   GpioCtrlRegs.GPBDIR.bit.GPIO62 = 0;          // input
   GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 1;        // XINT1 Synch to SYSCLKOUT only

   GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 0;         // GPIO
   GpioCtrlRegs.GPBDIR.bit.GPIO63 = 0;          // input
   GpioCtrlRegs.GPBQSEL2.bit.GPIO63 = 1;        // XINT1 Synch to SYSCLKOUT only

   GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 0;         // GPIO
   GpioCtrlRegs.GPCDIR.bit.GPIO64 = 0;          // input
   GpioCtrlRegs.GPCQSEL1.bit.GPIO64 = 1;        // XINT1 Synch to SYSCLKOUT only

   GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;         // GPIO
   GpioCtrlRegs.GPCDIR.bit.GPIO65 = 0;          // input
   GpioCtrlRegs.GPCQSEL1.bit.GPIO65 = 1;        // XINT1 Synch to SYSCLKOUT only
   GpioCtrlRegs.GPBCTRL.bit.QUALPRD3=0xFF;
   GpioCtrlRegs.GPCCTRL.bit.QUALPRD0=0xFF;
   GpioCtrlRegs.GPBQSEL2.bit.GPIO62=2;
   GpioCtrlRegs.GPBQSEL2.bit.GPIO63=2;

   GpioCtrlRegs.GPCCSEL1.bit.GPIO64=2;
   GpioCtrlRegs.GPCCSEL1.bit.GPIO65=2;*/

   EDIS;

      // GpioDataRegs.GPBDAT.bit.GPIO62


 /* GPIO_SetupXINT1Gpio(62);    //
    GPIO_SetupXINT2Gpio(63);    //
    GPIO_SetupXINT3Gpio(64);
    GPIO_SetupXINT4Gpio(65);*/

}

void ConfigSPIInit(void)
{
    EALLOW;
    GpioCtrlRegs.GPBPUD.bit.GPIO51 = 0;   // Enable pull-up on GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO51 = 3; // Asynch input GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPBGMUX2.bit.GPIO51 = 1; // Configure GPIO17 as SPISOMIA
    GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 2;
    EDIS;
   // XintRegs.XINT1CR.bit.ENABLE = 0;        // Enable XINT1


}
