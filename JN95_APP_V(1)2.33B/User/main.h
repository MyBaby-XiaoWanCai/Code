/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : main.h
** Author      : Cai Heng
** Date        : 2021-06-17
** Description : 
**********************************************************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/

/* 标准库头文件 */
#include  <math.h>
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdint.h>
#include  <stdlib.h>
/* 系统库头文件 */
#include "cmsis_os2.h"
#include "rtx_os.h"
#include "rl_net.h"  
/* 其他库头文件 */
/* 版本说明文件 */
#include "version.h"
/* 驱动层头文件 */
#include "bsp.h"
/* 应用层头文件 */
#include "app_func_eeprom.h"
#include "app_task_protocol.h"
#include "app_task_ageing.h"
#include "app_task_eth.h"
#include "app_task_acdc_alarm.h"
#include "app_task_led.h"
#include "app_task_protection.h"
#include "app_task_debug.h"
#include "app_timer_Periodic_adc3.h"
#include "app_task_workstep.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#ifndef TRUE
#define TRUE						1					//真
#endif

#ifndef FALSE
#define FALSE						0					//假
#endif

#ifndef DISABLE_CFG
#define DISABLE_CFG					0					//失能
#endif

#ifndef ENABLE_CFG
#define ENABLE_CFG					1					//使能
#endif

/* APP打印功能 */
#if (PRINTF_CFG == ENABLE_CFG)
#define APP_Printf				printf
#else
#define APP_Printf(...)
#endif

/* APP_Printf配置 */
#define TASK_ETH_PRINTF_CFG		DISABLE_CFG				//app_task_eth文件串口打印配置 (Configurable)
#define ETH_INIT_PRINTF_CFG		DISABLE_CFG				//ETH_INIT文件串口打印配置     (Configurable)

/* 毫秒级中断函数 */
#define bsp_ProPer1ms  			SysTick_ISR

/* TCPnet任务事件标志 */
#define FLASH_MSG				(1 << 0)
#define EEPROM_MSG				(1 << 1)

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern osThreadId_t	ThreadIdTaskMsgPro;	

	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
extern void AppTaskLed(void *argument);
extern void AppTaskProtection(void *argument);
extern void AppTaskDebug(void *argument);
extern void AppTaskEth(void *argument);
extern void AppTaskEeprom(void *argument);
extern void AppACDC_alarm(void *argument);
extern void AppTaskProtocol(void *argument);
extern void AppTaskWorkstep(void *argument);
extern void AppTaskMsgPro(void *argument);
extern void AppTaskAgeing(void *argument);
extern void bsp_Init(void);
extern void System_Init(void);
extern void SysTick_ISR(void);
extern void	sys_cfg_set(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
