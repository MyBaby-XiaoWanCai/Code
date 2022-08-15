/*
 * ADC_BSP.h
 *
 *  Created on: 2021Äê6ÔÂ9ÈÕ
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_ADC_BSP_H_
#define DSP28377D_BSP_INCLUDE_ADC_BSP_H_


#define SetAD2Const()   GpioDataRegs.GPBSET.bit.GPIO49=1;
#define ClrAD2Const()   GpioDataRegs.GPBCLEAR.bit.GPIO49=1;//AD2Const

extern void InitADC_BSP(void);
#endif /* DSP28377D_BSP_INCLUDE_ADC_BSP_H_ */
