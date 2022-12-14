/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_task_acdc_alarm.h
** Author      : wl
** Date        : 2021-08-20
** Description :
**********************************************************************************************************/

#ifndef _APP_TASK_ACDC_ALARM_H_
#define _APP_TASK_ACDC_ALARM_H_

#ifdef __cplusplus
extern "C" {
#endif

    /**********************************************************************************************************
    **                                          头文件引用区
    **********************************************************************************************************/
#include "stm32f4xx.h"
#include "cmsis_os2.h"
    /**********************************************************************************************************
    **                                            宏定义区
    **********************************************************************************************************/


    /**********************************************************************************************************
    **                                           类型定义区
    **********************************************************************************************************/


    /**********************************************************************************************************
    **                                         外部引用变量区
    **********************************************************************************************************/
    extern osSemaphoreId_t acdc_alarm_Semaphore;        /* ACDC告警信号量ID */

    /**********************************************************************************************************
    **                                         外部引用函数区
    **********************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
