/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : main.h
** Author      : Huang Cheng
** Date        : 2021-11-22
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

#include "app_task_signaldetect.h"
#include "app_fct.h"

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
#define bsp_ProPer1ms  			SysTick_IrqHandler
/* TCPnet任务事件标志 */
#define FLASH_MSG					(1 << 0)
#define EEPROM_MSG				(1 << 1)

//主控板版本宏定义
//#define C041A_7175
//#define C041B_7175
#define C042A_7606

//主控板版本宏定义
#define LM5170_AD7175			(0x00)
#define LM5170_AD7606			(0x01)
#define LM5170_SGM58601		(0x02)

#define C042B_BOOM_VER		(0x02)
#define C042C_BOOM_VER		(0x04)

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern uint8_t used_channel_cfg;
extern uint8_t contact_test_once_flag[8];

extern uint8_t ChannelTab[8];
extern uint8_t ALLCH;
extern uint8_t HALFCH1;
extern uint8_t HALFCH2;
extern uint8_t Weaver_Config;
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
extern void AppTaskLed(void *argument);
extern void AppTaskProtection(void *argument);
uint8_t BarCode_Mac_CheckSet(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
