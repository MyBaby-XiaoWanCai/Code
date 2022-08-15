/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_task_ageing.h
** Author      : Cai Heng
** Date        : 2021-08-10
** Description :
**********************************************************************************************************/

#ifndef _APP_TASK_AGEING_H_
#define _APP_TASK_AGEING_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "hc32_ddl.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
//自检老化任务周期设置(ms)
#define AGEING_TASK_PERIOD			100				// (Configure)

//g_uiErrFlag详细定义
#define NO_ERR						0				//无故障
#define SELF_CHECKING_ERR			(1 << 0)		//自检故障
#define AGEING_ERR					(1 << 1)		//老化故障
#define	SPEED_BREAK_ERR				(1 << 2)		//速断故障

//延时时间精度配置
#define SECOND_TIME_SCALE			100				//1秒钟包含的延时时间片数(100 * 10ms = 1s)
#define DELAY_TIME_SCALE			10				//延时时间精度(10ms)
#ifdef YCJ
#define AGEING_TIME_CFG				(6 * 3600)		//6Hour
#else
#define AGEING_TIME_CFG				(6 * 3600)		//6Hour
#endif

//自检老化使能标识
#define AGEING_ENABLE				1				//可进行自检老化
#define AGEING_DISABLE				0				//不可进行自检老化

//自检老化地址
#define AGEING_LIB_ADDR				8				//库号，当前控制板的IP第一位识别有问题，可能原因是MCU的PH6有问题
#define AGEING_BIT_ADDR				42				//位号

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/
extern uint8_t  g_ucSysMode;
extern uint8_t  ChannelErrorFlag;
extern uint8_t  ChannelStopFlag;
extern uint8_t  ChannelError[8];

extern float current_record_data[8];
extern float voltage_record_data[8];

extern uint8_t ageing_flag;		// 0 - 未执行老化  1 - 已执行老化

extern uint8_t  ChannelErrorFlag_Store;
extern uint8_t  ChannelError_Store[8];

extern float bus1_data;

extern float bus2_data;

//extern uint16_t device_err_flag;

extern uint16_t module_err_flag[8];
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/
extern void Chg_Dis_On_Cfg(uint8_t ch, uint8_t dir, uint8_t vmod, float vol, float cur);
extern void Chg_Dis_Off_Cfg(uint8_t ch);
extern void Protect_Trigger_Judgemet(void);
extern void AppTaskAgeing(void *argument);
	
#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
