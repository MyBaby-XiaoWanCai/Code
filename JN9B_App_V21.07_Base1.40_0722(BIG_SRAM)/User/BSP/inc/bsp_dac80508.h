/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_dac80508.h
** Author      : Huang Cheng
** Date        : 2021-11-05
** Description : bsp_dac80508 head file
**********************************************************************************************************/
#ifndef __BSP_DAC80508_H
#define __BSP_DAC80508_H

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
#define	dac_1 	                                    0
#define	dac_2 	                                    1
	
/* Error codes */
#define INVALID_VAL								-1				/* Invalid argument */
#define COMM_ERR									-2				/* Communication error on receive */
#define TIMEOUT										-3				/* A timeout has occured */

/* DAC80508 Register Map */
#define DACx0508C_CONFIG_MODE                     ((3 << 16) | 0x100)
#define DACx0508C_GAIN_2X                         ((4 << 16) | 0xFF)
#define DACx0508C_DEVICE_ID                       0x01
#define DACx0508C_SYNC   	                        0x02
#define DACx0508C_CONFIG  	                      0x03
#define DACx0508C_GAIN	 	                        0x04
#define DACx0508C_TRIGGER	                        0x05
#define DACx0508C_BRDCAST_ADDRESS	                0x06
#define DACx0508C_STATUS	  		                	0x07
#define DACx0508C_DAC0	  		                    0x08
#define DACx0508C_DAC1	  		                    0x09
#define DACx0508C_DAC2	  		                    0x0A
#define DACx0508C_DAC3	  		                    0x0B
#define DACx0508C_DAC4	  		                    0x0C
#define DACx0508C_DAC5	  		                    0x0D
#define DACx0508C_DAC6	  		                    0x0E
#define DACx0508C_DAC7	  		                    0x0F
#define DACx0508C_READ	  		                    0x80

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern uint8_t DacVal[2];
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void bsp_dacx0508_init(void);
void DAC80508_SetDacData(uint8_t _ch, uint16_t _dac, uint8_t index);
void DAC_SetLinearData(uint8_t _ch, uint16_t _Target_Iout, uint8_t index);
void DAC_SetLinearData_2channl(uint8_t _ch1, uint8_t _ch2, uint16_t _Target_Iout1, uint16_t _Target_Iout2, uint8_t index);
void DAC80508_Read_DAC_Value(uint8_t Addr, uint8_t *Value, uint8_t size, uint8_t index);
void DAC80508_ReadDacData(uint8_t _ch, uint8_t *Value, uint8_t index);
void DAC80508_BRDCAST(uint16_t _dac, uint8_t index);

#ifdef __cplusplus
}
#endif	

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
