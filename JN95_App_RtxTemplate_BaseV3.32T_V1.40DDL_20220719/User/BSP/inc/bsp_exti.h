/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_exti.h
** Author      : Huang Cheng
** Date        : 2021-11-09
** Description : 
**********************************************************************************************************/

#ifndef __BSP_EXTI_H
#define __BSP_EXTI_H

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/

#include <stdint.h>
#include "hc32_ddl.h"
#include "bsp_ad7124.h"
#include "bsp_ad7175.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/

#define AD7175_EXTI_PORT      (GPIO_PORT_E)
#define AD7175_EXTI_PIN       (GPIO_PIN_00)
#define AD7175_EXINT_CH       (EXINT_CH00)
#define AD7175_INT_SRC        (INT_PORT_EIRQ0)
#define AD7175_IRQn           (Int001_IRQn)

#define AD7124_EXTI_PORT      (GPIO_PORT_E)
#define AD7124_EXTI_PIN       (GPIO_PIN_01)
#define AD7124_EXINT_CH       (EXINT_CH01)
#define AD7124_INT_SRC        (INT_PORT_EIRQ1)
#define AD7124_IRQn           (Int002_IRQn)

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void AD7175_EXINT_Init(void);
void AD7124_EXINT_Init(void);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/



