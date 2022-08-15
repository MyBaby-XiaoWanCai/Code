/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : versmana.h
** Author      :  
** Date        : 2022-01-18
** Description : 
**********************************************************************************************************/

#ifndef _BSP_VERSMANA_H_
#define _BSP_VERSMANA_H_

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
#define BOOM_IO_NUM		(0x03)
#define PCB_IO_NUM		(0x03)
#define ID_IO_NUM			(0x02)

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
typedef struct
{
	uint8_t Bom_Vers;
	uint8_t Pcb_Vers;
	uint8_t Id_Vers;
	
	uint8_t LED_Ver;	//灯板版本号缓存
	uint8_t Pow_Ver[8];	//功率板版本号缓存	
}Vers_Mana;

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void Version_Mana_Read(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
