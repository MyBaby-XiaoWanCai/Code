/*
 * ADC_BSP.h
 *
 *  Created on: 2021Äê6ÔÂ9ÈÕ
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_ADC_BSP_H_
#define DSP28377D_BSP_INCLUDE_ADC_BSP_H_

#define SetADRst()      GpioDataRegs.GPBSET.bit.GPIO50=1;//
#define ClrADRst()      GpioDataRegs.GPBCLEAR.bit.GPIO50=1;//ADRST
#define SetAD1Const()   GpioDataRegs.GPBSET.bit.GPIO48=1;
#define ClrAD1Const()   GpioDataRegs.GPBCLEAR.bit.GPIO48=1;//AD1Const
#define SetAD2Const()   GpioDataRegs.GPBSET.bit.GPIO49=1;
#define ClrAD2Const()   GpioDataRegs.GPBCLEAR.bit.GPIO49=1;//AD2Const

extern void InitADC_BSP(void);
#endif /* DSP28377D_BSP_INCLUDE_ADC_BSP_H_ */
