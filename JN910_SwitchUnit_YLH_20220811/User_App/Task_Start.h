/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Start.h
** Author      : Cai Heng
** Date        : 2022-07-07
** Description :
**********************************************************************************************************/

#ifndef _TASK_START_H_
#define _TASK_START_H_

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
#define START_TASK_PERIOD		1

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/
extern osSemaphoreId_t SemaDcdcCan;
extern osSemaphoreId_t SemaProtection;

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/
extern void AppTaskStart(void *argument);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
