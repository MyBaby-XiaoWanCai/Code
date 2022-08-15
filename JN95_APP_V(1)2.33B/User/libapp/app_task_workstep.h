/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_task_workstep.h
** Author      : Cai Heng
** Date        : 
** Description : 
**********************************************************************************************************/

#ifndef _APP_TASK_WORKSTEP_H_
#define _APP_TASK_WORKSTEP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "stm32f4xx.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#define TASK_PROTECT_SW				1
#define DEBUG_SW					1

#define PROTECT_GLOBAL				0
#define PROTECT_INTERNAL			1
#define PROTECT_WORKSTEP			2
#define PROTECT_ACDC			    3

#ifdef YCJ
#define DECREASE_STEP				60
#define INCREASE_STEP				40
#else
#define DECREASE_STEP				60
#define INCREASE_STEP				50
#endif

/* 1通道定义 */
#define CHANNEL1_1		0x11
/* 2通道定义 */
#define CHANNEL2_1		0x21
#define CHANNEL2_2		0x22
/* 4通道定义 */
#define CHANNEL4_1		0x41
#define CHANNEL4_2		0x42
#define CHANNEL4_3		0x43
#define CHANNEL4_4		0x44
/* 8通道定义 */
#define CHANNEL8_1		0x81
#define CHANNEL8_2		0x82
#define CHANNEL8_3		0x83
#define CHANNEL8_4		0x84
#define CHANNEL8_5		0x85
#define CHANNEL8_6		0x86
#define CHANNEL8_7		0x87
#define CHANNEL8_8		0x88

#define CHANNEL_MAX		8

#ifdef YCJ
#define CHANNEL_CFG		8
#else
#define CHANNEL_CFG		4
#endif

#if (CHANNEL_CFG == 1)
#define CHANNEL1		CHANNEL1_1
#define ALLCH           0x01
#define HALFCH1         0x01
#define HALFCH2         0x01
#elif (CHANNEL_CFG == 2)
#define CHANNEL1		CHANNEL2_1
#define CHANNEL2		CHANNEL2_2
#define ALLCH           0x03
#define HALFCH1         0x01
#define HALFCH2         0x02
#elif (CHANNEL_CFG == 4)
#define CHANNEL1		CHANNEL4_1
#define CHANNEL2		CHANNEL4_2
#define CHANNEL3		CHANNEL4_3
#define CHANNEL4		CHANNEL4_4
#define ALLCH           0x0F
#define HALFCH1         0x03
#define HALFCH2         0x0c
#elif (CHANNEL_CFG == 8)
#define CHANNEL1		CHANNEL8_1
#define CHANNEL2		CHANNEL8_2
#define CHANNEL3		CHANNEL8_3
#define CHANNEL4		CHANNEL8_4
#define CHANNEL5		CHANNEL8_5
#define CHANNEL6		CHANNEL8_6
#define CHANNEL7		CHANNEL8_7
#define CHANNEL8		CHANNEL8_8
#define ALLCH           0xff
#define HALFCH1         0x0F
#define HALFCH2         0xF0
#endif

#define WORKSTEP_TASK_PERIOD	1

#define WORKSTEP_PROTECT_ENABLE		1
#define WORKSTEP_PROTECT_DISABLE	0

#define WORK_VOL_MAX				4.8f
#define WORK_VOL_MIN				0.5f
#ifdef YCJ
#define WORK_CUR_MAX				30.0f
#else
#define WORK_CUR_MAX				160.0f
#endif

#define PRO_TIME_PERIOD				1000.0f		// 1ms * 1000 = 1s
#define RUN_TIME_PERIOD				1000.0f		// 1ms * 1000 = 1s

#define CC_TO_CV_I_LIMIT        	0.08f
#define IN_CV_I_LIMIT				0.5f
#define CC_TO_CV_V_LINIT        	0.005f
#define DCC_TO_DCV_I_LIMIT      	0.08f
#define IN_DCV_I_LIMIT				0.5f
#define DCC_TO_DCV_V_LIMIT      	0.005f

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern uint8_t ChannelTab[CHANNEL_CFG];

extern uint8_t Protect_Flag_Global[8];
extern uint8_t Protect_Flag_Internal[8];
extern uint8_t Protect_Flag_Workstep[8];

extern uint16_t workstep_flag_ErrWord[8];

extern uint8_t  DeviceStateFlag[8];		// 0 - Stop;     1 - Start
extern uint8_t  DeviceStateFlagPre[8];
extern uint8_t  ProtectMaskFlag[8];		// 0 - No Mask;  1 - Mask
extern uint32_t ProtectMaskCnt[8];

extern uint8_t AcDumpConnectFlag;
extern uint8_t ContactCheckFlag[8];

extern uint8_t start_state_flag[8];

extern uint8_t time_over_stop[8];
extern uint8_t abort_type_flag[8];

extern uint8_t protect_flag_upcast[8];

extern uint64_t run_time_cnt[8];

extern uint8_t active_run_channel_numb;

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
extern void Protect_Action_Func(uint8_t chx, uint8_t flag, uint8_t sta);
extern void Protect_BitAction_Func(uint8_t chx, uint8_t flag, uint8_t sta);
extern int16_t BatteryFormattingStop(uint8_t chx);
#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
