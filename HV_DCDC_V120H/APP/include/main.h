/*
 * main.h
 *
 *  Created on: 2021年4月23日
 *      Author: JC
 */

#ifndef APP_INCLUDE_MAIN_H_
#define APP_INCLUDE_MAIN_H_

#define FALSE       0
#define TRUE        1
#define BUSY        2



extern volatile Uint16 Tm1ms;
extern volatile Uint16 Tm2ms;
extern volatile Uint16 Tm10ms;
extern volatile Uint16 Tm1s;
extern void Task1ms(void);
extern void Task2ms(void);
extern void Task10ms(void);
extern void Task1s(void);
extern void InitEmif1(void);
extern void InitSystem(void);
extern interrupt void EPWM1_INT_ISR_BSP(void);
extern __interrupt void cpu_timer0_isr(void);
extern void CLA_configClaMemory(void);
extern void CLA_initCpu1Cla1(void);
// 其它参数定义

#endif /* APP_INCLUDE_MAIN_H_ */
