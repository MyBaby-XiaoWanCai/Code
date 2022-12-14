/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Protection_Task.h
** Author      : Huang Cheng
** Date        : 2022-03-08
** Description :
**********************************************************************************************************/

#ifndef _TASK_PROTECTION_H_
#define _TASK_PROTECTION_H_

#ifdef __cplusplus                  //C++代码嵌入
extern "C" {
#endif
    
/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern uint32_t GP_Alarm_Flag[8];
extern uint32_t IP_Alarm_Flag[8];
extern uint8_t  SysModeProtMaskFlag;

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/
extern void AppTaskProtection(void *argument);
    
    
    
#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

