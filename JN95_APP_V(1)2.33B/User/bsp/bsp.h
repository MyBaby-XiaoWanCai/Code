/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp.h
** Author      : Cai Heng
** Date        : 2021-06-16
** Description : 
**********************************************************************************************************/
#ifndef _BSP_H_
#define _BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
/* HAL库头文件  */
#include "stm32f4xx_hal.h"
/* 标准库头文件 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* 底层驱动头文件 */
#include "bsp_demo.h"
#include "bsp_rng.h"
#include "bsp_led.h"
#include "bsp_gpio.h"
#include "bsp_protection.h"
#include "bsp_charge.h"
#include "bsp_debug.h"
#include "bsp_adc3.h"
#include "bsp_ad7124.h"
#include "bsp_ad7175.h"
#include "bsp_acdc.h"
#include "bsp_dac80508.h"
#include "bsp_AD5245.h"
#include "bsp_MB85RS2.h"
#include "bsp_dwt.h"
#include "bsp_eeprom.h"
#include "bsp_flash.h"
#include "bsp_lan.h"
#include "bsp_spi.h"
#include "bsp_tim_pwm.h"
#include "bsp_timer.h"
#include "stm32f4xx_hal_timebase.h"
#include "iwdg.h"
#include "ringbuffer.h"
/* 其他类型头文件 */
#include "global.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#ifndef TRUE
#define TRUE					1					//真
#endif

#ifndef FALSE
#define FALSE					0					//假
#endif



#ifndef DISABLE_CFG
#define DISABLE_CFG				0					//失能
#endif

#ifndef ENABLE_CFG
#define ENABLE_CFG				1					//使能
#endif

/* 开关全局中断 */
#define ENABLE_INT()			__set_PRIMASK(0)	//使能全局中断
#define DISABLE_INT()			__set_PRIMASK(1)	//禁止全局中断

/* 中断优先级分组 */
#define NVIC_PREEMPT_PRIORITY	4					//(Configurable, default: 4)

/* 调试组件配置 */
#define	Enable_EventRecorder	FALSE				//(Configurable)

/* 操作系统配置 */
#define	USE_RTX    				TRUE				//(Configurable)

/* 网口硬件配置 */
#define PHY_KSZ8041				FALSE				//(Configurable)

/* 串口打印配置(总) */
#define PRINTF_CFG				ENABLE_CFG			//(Configurable)

/* BSP_Printf配置 */
#define TIMER_PRINTF_CFG		DISABLE_CFG			//bsp_timer文件串口打印配置 (Configurable)

/* BSP打印功能 */
#if (PRINTF_CFG == ENABLE_CFG)
#define BSP_Printf				printf
#else
#define BSP_Printf(...)
#endif

/* 定时器初始化错误处理 */
#define ERROR_HANDLER()			Error_Handler(__FILE__, __LINE__);

#if 0
/* CPU空闲时执行的函数 */
#define CPU_IDLE()				bsp_Idle()
#endif

/* 未知 */
#define EXTI9_5_ISR_MOVE_OUT

/* 调试组件头文件 */
#if (Enable_EventRecorder == TRUE)
#include "EventRecorder.h"
#endif



/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
extern void bsp_InitDWT(void);
extern void bsp_InitI2C(void);
extern void bsp_InitSpi6(void);
extern void bsp_InitTimer(void);
extern void bsp_InitLed(void);
extern void bsp_InitW5500(void);
extern void bsp_InitAD7124(void);
extern void bsp_InitAD7175(void);
extern void Dis_int(void);
extern void En_int(void);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
