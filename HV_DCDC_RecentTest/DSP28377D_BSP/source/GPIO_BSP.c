/*
 * GPIO_BSP.c
 *
 *  Created on: 2021年6月5日
 *      Author: JC
 */
#include "F28x_Project.h"

//函数定义
void ExintInit()
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;          // input
    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 1;        // XINT1 Synch to SYSCLKOUT only
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 0;          // input
    GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 1;        // XINT1 Synch to SYSCLKOUT only
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO14 = 0;          // input
    GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 1;        // XINT1 Synch to SYSCLKOUT only
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO15 = 0;          // input
    GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 1;        // XINT1 Synch to SYSCLKOUT only
    EDIS;
    GPIO_SetupXINT1Gpio(12);    // GPIO12 is XINT1
    XintRegs.XINT1CR.bit.POLARITY = 0;      // Falling edge interrupt
    XintRegs.XINT1CR.bit.ENABLE = 1;        // Enable XINT1
    GPIO_SetupXINT2Gpio(13);    // GPIO13 is XINT2
    XintRegs.XINT2CR.bit.POLARITY = 0;      // Falling edge interrupt
    XintRegs.XINT2CR.bit.ENABLE = 1;        // Enable XINT1
    GPIO_SetupXINT3Gpio(14);    // GPIO14 is XINT3
    XintRegs.XINT3CR.bit.POLARITY = 0;      // Falling edge interrupt
    XintRegs.XINT3CR.bit.ENABLE = 1;        // Enable XINT1
    GPIO_SetupXINT4Gpio(15);
    XintRegs.XINT4CR.bit.POLARITY = 0;      // Falling edge interrupt
    XintRegs.XINT4CR.bit.ENABLE = 1;        // Enable XINT1
}
void InitGPIO_BSP(void)
{
    EALLOW;
   // CONFIG_GPIO_LED_RED;

    EDIS;
    ExintInit();

}
void CONFIG_GPIO_FAULT_FAN(void)
{
    //风机故障1
    GpioCtrlRegs.GPCPUD.bit.GPIO67 = 0;
    GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO67=0;

    //风机故障2
    GpioCtrlRegs.GPCPUD.bit.GPIO86 = 0;
    GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO86=0;

    //风机故障3
    GpioCtrlRegs.GPCPUD.bit.GPIO87 = 0;
    GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO87=0;

    //风机故障4
    GpioCtrlRegs.GPCPUD.bit.GPIO88 = 0;
    GpioCtrlRegs.GPCMUX2.bit.GPIO88 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO88=0;

    //风机故障5
    GpioCtrlRegs.GPCPUD.bit.GPIO93 = 0;
    GpioCtrlRegs.GPCMUX2.bit.GPIO93 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO93=0;

    //风机故障6
    GpioCtrlRegs.GPCPUD.bit.GPIO94 = 0;
    GpioCtrlRegs.GPCMUX2.bit.GPIO94 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO94=0;

    //风机故障7
    GpioCtrlRegs.GPDPUD.bit.GPIO99 = 0;
    GpioCtrlRegs.GPDMUX1.bit.GPIO99 = 0;
    GpioCtrlRegs.GPDDIR.bit.GPIO99=0;
    //风机故障8
    GpioCtrlRegs.GPEPUD.bit.GPIO133 = 0;
    GpioCtrlRegs.GPEMUX1.bit.GPIO133 = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO133=0;
}

