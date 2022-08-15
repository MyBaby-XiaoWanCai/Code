/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : fct.h
** Author      :  
** Date        : 2022-02-18
** Description : 
**********************************************************************************************************/

#ifndef _APP_FCT_H_
#define _APP_FCT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include "hc32_ddl.h"
#include "bsp.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
//FCT流程步骤宏定义
#define FCTSTEP_START 			(0x00)  //FCT开始
#define FCT_REGINSIGNAL			(0x01)	//在位信号检测
#define FCT_VERSION 				(0x02) 	//版本检测
#define FCT_LAMPPLATE				(0x03)	//灯板检测
#define FCT_FAN							(0x04)	//风扇检测
#define FCT_TMP							(0x05)	//温度测试
#define FCT_CHARGEDISC	 		(0x06)	//充放电测试 1充2放、3充4放、5充6放、7充8放
#define FCT_DISCCHARGE 			(0x07)	//放充电测试 1放2充、3放4充、5放6充、7放8充
#define FCT_VSENSEFAST			(0x08)	//Vsense快速环测试
#define FCT_VSENSESLOW			(0x09)	//Vsense慢速环测试
#define FCTSTEP_END					(0x0A)	//FCT结束
#define FCT_END_NOP					(0x0B)

#define FCT_EE_ADDR_A_1			(0x7BFF)//FCT_EE_ADDR_A - 1 = 0x7C00
#define FCT_EE_ADDR_A 			(0x7C00)

#define FCT_TEST_ITEM_NUM				(0x25)	//FCT测试项数

#define CHARG_VOLT1				(4)		//充电电压
#define DISC_VOLT1				(1)		//放电电压
#define CHARG_CUR1				(40)	//充电电流1
#define CHARG_CUR2				(130)	//充电电流2
#define DISC_CUR1					(30)	//放电电流1
#define DISC_CUR2					(120)	//放电电流2

#define ID_VER					0x00
#define PCB_VER					0x04
#define BOM_VER					0x02
#define PB_VER					0x02
#define LED_VER					(vers_mana.LED_Ver)

#define DETECT_TASK_PERIOD		5		//任务周期设置(ms)
#define SIGNAL_PRO_TASK_PERIOD	2		//MSG_PRO任务周期设置(ms)
/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern uint8_t FCT_ReadBuf[64];
extern uint8_t FCT_WriteBuf[64];
extern uint8_t FCTEndFlg;

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
uint8_t WholeMachine_FCT(void);

uint8_t WholeMachine_FCTSatart(void);	//FCT开始
uint8_t WholeMachine_FCTReginSignal(void);	//在位信号检测
uint8_t WholeMachine_FCTVersion(void); //版本检测
uint8_t WholeMachine_FCTLampPlate(void); //灯板测试
uint8_t WholeMachine_FCTFan(void); //风扇测试
uint8_t WholeMachine_FCTTmp75(void);	//TMP75测试
uint8_t WholeMachine_FCTChargeDisc(void); //充放电测试
uint8_t WholeMachine_FCTDischCharge(void);	//放充电测试
uint8_t WholeMachine_FCTVsenseRapidLoop(void);	//快速环测试
uint8_t WholeMachine_FCTVsenseSlowLoop(void);	//慢速环测试
uint8_t WholeMachine_FCTEnd(void);	//FCT结束

uint8_t WholeMachine_FCTChaDisFunc_1(uint8_t CHx1,uint8_t CHx2); //充放电测试功能函数

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
