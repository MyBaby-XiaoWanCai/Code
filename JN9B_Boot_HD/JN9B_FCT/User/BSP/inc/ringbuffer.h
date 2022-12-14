/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : ringbuffer.h
** Author      : wang liang
** Date        : 2021-06-16
** Description : 
**********************************************************************************************************/
#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
/* HAL库头文件  */
#include "hc32_ddl.h"
/* 标准库头文件 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/


#define RINGBUFSIZE (100*13)



/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void InitRingBuffer(uint8_t* buffer,uint16_t size);
int WriteRingBuffer(uint8_t* buffer,uint16_t length);
int ReadRingBuffer(uint8_t* buffer,uint16_t length);
uint16_t GetRingBufferValidLen(void);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


