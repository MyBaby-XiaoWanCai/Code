/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : AppEeprom_Func.h
** Author      : Huang Cheng 
** Date        : 2022-03-08
** Description :
**********************************************************************************************************/

#ifndef _APP_FUNC_EEPROM_H_
#define _APP_FUNC_EEPROM_H_

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
#define SYNC_TYPE_GPROT		                0
#define SYNC_TYPE_IPROT		                1
#define SYNC_TYPE_CALI		                2

//EEPROM任务周期设置(ms)
#define EEPROM_TASK_PERIOD					1000				// (Configure)

//结构体信息(新增结构体参考以下格式)
// 0. 校验标识
#define CALIBRATE_USAGE_FLAG_ADDR			0
#define CALIBRATE_USED						0x2323									// 校准后不要随意更改！
#define CALIBRATE_UNUSED					0x0000

#define PROTECT_GLOBAL_USAGE_FLAG_ADDR		(2048 - 16)
#define PROTECT_GLOBAL_USED					0x030A
#define PROTECT_GLOBAL_UNUSED				0x0000

#define PROTECT_INTERNAL_USAGE_FLAG_ADDR	(2048 - 14)
#define PROTECT_INTERNAL_USED				0x030A
#define PROTECT_INTERNAL_UNUSED				0x0000

// 1. 校准结构体
#define CALIBRATE_ID(x)						(0 + x)									// (Configure) 校准结构体ID
#define CALIBRATE_STRUCT(x)					Tier1.calibrate[x]						// (Configure) 校准结构体变量
#define CALIBRATE_BASEADDR(x)				(2 + x * sizeof(CALIBRATE_STRUCT(x)))	// (Configure) 校准结构体基地址

// 1. 全局保护结构体
#define PROTECT_GLOBAL_ID(x)				(8 + x)									//
#define PROTECT_GLOBAL_STRUCT(x)			Tier1.gProt[x]							//
#define PROTECT_GLOBAL_BASEADDR(x)			(2048 + x * 256)						//

// 2. 内部保护结构体
#define PROTECT_INTERNAL_ID(x)				(16 + x)								//
#define PROTECT_INTERNAL_STRUCT(x)			Tier1.iProt[x]							//
#define PROTECT_INTERNAL_BASEADDR(x)		(4096 + x * 256)						//

//EEPROM存取缓存数组(缓存结构体大小不能超限)
#define EEPROM_BUF_SIZE						4096									// (Configure)

//供用户调用EEPROM存取功能相关的宏
#define EEP_DAT_LOAD(x)						EepDatLoad(x)							// 加载指定结构体
#define EEP_DAT_SYNC(x)						EepDatSync(x)							// 保存指定结构体

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/
extern uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
extern uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
extern uint8_t ee_WriteBytesWithCheck(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
extern void EepDatLoad(uint8_t id);
extern void EepDatSync(uint8_t id);    
    
    
#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

