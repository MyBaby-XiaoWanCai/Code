/*
*********************************************************************************************************
*
*	模块名称 : 
*	文件名称 :
*	版    本 : V1.0
*	说    明 :
*	修改记录 :
*	版本号  日期         作者       说明
*	V1.0    2020-03-02   ZYL        正式发布
*
*********************************************************************************************************
*/

#ifndef _dac80508_H_
#define _dac80508_H_

#include "stdint.h"

extern int32_t InitDAC80508(void);
extern int32_t DAC80508_SetDacData(uint8_t _ch, uint16_t _dac, uint8_t index);
extern int32_t DAC80508_BRDCAST(uint16_t _dac, uint8_t index);

#endif

/***************************** (END OF FILE) *********************************/
