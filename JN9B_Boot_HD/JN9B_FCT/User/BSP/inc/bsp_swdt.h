/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_swdt.h
** Author      : Huang Cheng
** Date        : 2021-11-14
** Description : 华大MCU专用看门狗计数器
                 SWDT是16位递减计数器，
                 计数值只能选择：256、4096、16384、65536这几种，不能任意设置。
                 时钟分频可选 ：1、16、32、64、128、256、4096。
                 看门狗复位时间(秒)=分频 * 计数值 / 10KHz
**********************************************************************************************************/

#ifndef _BSP_SWDT_H_
#define _BSP_SWDT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include "hc32_ddl.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
/* SWDT count cycle definition */
#define SWDT_COUNT_CYCLE                        (4096U)

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void bsp_InitSWDT(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
