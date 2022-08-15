/*
 * HRPWM.h
 *
 *  Created on: 2021Äê6ÔÂ20ÈÕ
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_HRPWM_H_
#define DSP28377D_BSP_INCLUDE_HRPWM_H_
#define PWM_CH  9
#define SFO_INCOMPLETE      0
#define SFO_COMPLETE        1
#define SFO_ERROR           2
extern void HRPWM1_Config(void);
extern void HRPWM2_Config(void);
extern void HRPWM3_Config(void);
extern void HRPWM4_Config(void);
extern void HRPWM5_Config(void);
extern void HRPWM6_Config(void);
extern void HRPWM7_Config(void);
extern void HRPWM8_Config(void);

extern void PWMON_Config(Uint16 CH);
extern void PWMOFF_Config(Uint16 CH);
extern void PWM9_Config(void);
extern void PWM10_Config(void);
extern void PWM11_Config(void);
extern void PWM12_Config(void);
extern void setupHrpwmMepScaleFactor(void);
extern uint32_t   MEP_ScaleFactor;
#endif /* DSP28377D_BSP_INCLUDE_HRPWM_H_ */
