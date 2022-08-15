/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Workstep.h
** Author      : Cai Heng
** Date        : 2022-07-26
** Description : 
**********************************************************************************************************/

#ifndef _TASK_WORKSTEP_H_
#define _TASK_WORKSTEP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#define PROTECT_GLOBAL				0
#define PROTECT_INTERNAL			1
#define PROTECT_WORKSTEP			2
#define PROTECT_ACDC				3

/* 1通道定义 */
#define CHANNEL1_1					0x11
/* 2通道定义 */
#define CHANNEL2_1					0x21
#define CHANNEL2_2					0x22
/* 4通道定义 */
#define CHANNEL4_1					0x41
#define CHANNEL4_2					0x42
#define CHANNEL4_3					0x43
#define CHANNEL4_4					0x44
/* 8通道定义 */
#define CHANNEL8_1					0x81
#define CHANNEL8_2					0x82
#define CHANNEL8_3					0x83
#define CHANNEL8_4					0x84
#define CHANNEL8_5					0x85
#define CHANNEL8_6					0x86
#define CHANNEL8_7					0x87
#define CHANNEL8_8					0x88

#define CHANNEL_CFG					8
#define CHANNEL_MAX					8

#define WORKSTEP_TASK_PERIOD		1

#define WORK_VOL_MAX				(4.8 * 8)
#define WORK_VOL_MIN				(0.5 * 8)
#define WORK_CUR_MAX				(120.0  )

#define PRO_TIME_PERIOD				1000.0f		// 1ms * 1000 = 1s
#define RUN_TIME_PERIOD				1000.0f		// 1ms * 1000 = 1s

#define WORKSTEP_PROTECT_ENABLE		1
#define WORKSTEP_PROTECT_DISABLE	0

#define DEBUG_CH					0x04

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/
extern uint8_t  AcDumpConnectFlag;
extern uint8_t  start_state_flag[8];
extern uint8_t  abort_type_flag[8];
extern uint8_t  time_over_stop[8];
extern uint8_t  ProtectMaskFlag[CHANNEL_MAX];
extern uint8_t  record_data_upcast_flag[8];
extern uint8_t  protect_flag_upcast[8];
extern uint8_t  Protect_Flag_Global[CHANNEL_MAX];
extern uint8_t  Protect_Flag_Internal[CHANNEL_MAX];
extern uint8_t  Protect_Flag_Workstep[CHANNEL_MAX];
extern uint16_t workstep_flag_ErrWord[8];
extern uint64_t run_time_cnt[8];

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/
extern void Protect_Action_Func(uint8_t chx, uint8_t flag, uint8_t sta);
extern void AppTaskWorkstep(void *argument);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
