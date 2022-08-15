/*
 * GPIO_BSP.h
 *
 *  Created on: 2021年6月5日
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_GPIO_BSP_H_
#define DSP28377D_BSP_INCLUDE_GPIO_BSP_H_

#define SET_FLASH_CS       {GpioDataRegs.GPBSET.bit.GPIO61   = 1;}
#define CLEAR_FLASH_CS     {GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;}
#define SetADRst()         GpioDataRegs.GPBSET.bit.GPIO34=1;//
#define ClrADRst()         GpioDataRegs.GPBCLEAR.bit.GPIO34=1;//ADRST
#define SetADConst()       GpioDataRegs.GPBSET.bit.GPIO32=1;
#define ClrADConst()       GpioDataRegs.GPBCLEAR.bit.GPIO32=1;//ADConst
#define LED_ON             GpioDataRegs.GPCCLEAR.bit.GPIO88   = 1;
#define LED_OFF            GpioDataRegs.GPCSET.bit.GPIO88 = 1;
#define LED_TOGGLE         GpioDataRegs.GPCTOGGLE.bit.GPIO88=1;
#define Rev_Port1_ON       GpioDataRegs.GPBSET.bit.GPIO44 = 1;
#define Rev_Port1_OFF      GpioDataRegs.GPBCLEAR.bit.GPIO44 = 1;
#define Rev_Port2_ON       GpioDataRegs.GPBSET.bit.GPIO45 = 1;
#define Rev_Port2_OFF      GpioDataRegs.GPBCLEAR.bit.GPIO45 = 1;
#define Rev_Port3_ON       GpioDataRegs.GPBSET.bit.GPIO46 = 1;
#define Rev_Port3_OFF      GpioDataRegs.GPBCLEAR.bit.GPIO46 = 1;

#define Rev_Port4_ON       GpioDataRegs.GPBSET.bit.GPIO47 = 1;
#define Rev_Port4_OFF      GpioDataRegs.GPBCLEAR.bit.GPIO47 = 1;

#define AD7124_CS_0()      GpioDataRegs.GPBCLEAR.bit.GPIO53   = 1;
#define AD7124_CS_1()      GpioDataRegs.GPBSET.bit.GPIO53   = 1;
#define LED_RUN_RedON      GpioDataRegs.GPCSET.bit.GPIO86 = 1;
#define LED_RUN_RedOFF     GpioDataRegs.GPCCLEAR.bit.GPIO86 = 1;
#define LED_RUN_GreenON    GpioDataRegs.GPCSET.bit.GPIO87 = 1;
#define LED_RUN_GreenOFF   GpioDataRegs.GPCCLEAR.bit.GPIO87 = 1;
#define LED_CH1_RedON      GpioDataRegs.GPBSET.bit.GPIO54 = 1;
#define LED_CH1_RedOFF     GpioDataRegs.GPBCLEAR.bit.GPIO54 = 1;
#define LED_CH1_GreenON    GpioDataRegs.GPBSET.bit.GPIO55 = 1;
#define LED_CH1_GreenOFF   GpioDataRegs.GPBCLEAR.bit.GPIO55 = 1;

#define LED_CH2_RedON      GpioDataRegs.GPBSET.bit.GPIO56 = 1;
#define LED_CH2_RedOFF     GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1;
#define LED_CH2_GreenON    GpioDataRegs.GPBSET.bit.GPIO57 = 1;
#define LED_CH2_GreenOFF   GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;

#define LED_CH3_RedON      GpioDataRegs.GPCSET.bit.GPIO64 = 1;
#define LED_CH3_RedOFF     GpioDataRegs.GPCCLEAR.bit.GPIO64 = 1;
#define LED_CH3_GreenON    GpioDataRegs.GPCSET.bit.GPIO65 = 1;
#define LED_CH3_GreenOFF   GpioDataRegs.GPCCLEAR.bit.GPIO65 = 1;

#define LED_CH4_RedON      GpioDataRegs.GPCSET.bit.GPIO66 = 1;
#define LED_CH4_RedOFF     GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;
#define LED_CH4_GreenON    GpioDataRegs.GPCSET.bit.GPIO67 = 1;
#define LED_CH4_GreenOFF   GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;



//对外部文件开放函数声明
extern void InitGPIO_BSP(void);
extern void ExintInit(void);
extern void ConfigSPIInit(void);
#endif /* DSP28377D_BSP_INCLUDE_GPIO_BSP_H_ */
