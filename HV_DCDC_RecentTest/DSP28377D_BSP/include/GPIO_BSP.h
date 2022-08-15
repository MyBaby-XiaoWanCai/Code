/*
 * GPIO_BSP.h
 *
 *  Created on: 2021年6月5日
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_GPIO_BSP_H_
#define DSP28377D_BSP_INCLUDE_GPIO_BSP_H_

#define CONFIG_GPIO_LED_RED     {GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;GpioDataRegs.GPASET.bit.GPIO0 = 1;}
#define LED_RED_ON              {GpioDataRegs.GPACLEAR.bit.GPIO0   = 1;}
#define LED_RED_OFF             {GpioDataRegs.GPASET.bit.GPIO0 = 1;}
#define LED_RED_TOGGLE          {GpioDataRegs.GPATOGGLE.bit.GPIO0=1;}


#define CONFIG_GPIO_RELAY     {GpioCtrlRegs.GPAPUD.bit.GPIO16 = 1;GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0;GpioCtrlRegs.GPADIR.bit.GPIO16 = 1;GpioDataRegs.GPASET.bit.GPIO16 = 1;}
#define RELAY_ON              {GpioDataRegs.GPACLEAR.bit.GPIO16   = 1;}
#define RELAY_OFF             {GpioDataRegs.GPASET.bit.GPIO16 = 1;}
#define RELAY_TOGGLE          {GpioDataRegs.GPATOGGLE.bit.GPIO16=1;}

#define IS_FAULT_FAN            GpioDataRegs.GPADAT.bit.GPIO12==0
//对外部文件开放函数声明
extern void InitGPIO_BSP(void);
#endif /* DSP28377D_BSP_INCLUDE_GPIO_BSP_H_ */
