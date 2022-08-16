/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_debug.h
** Author      : Huang Cheng
** Date        : 2021-11-15
** Description : 配置串口打印用于调试
**********************************************************************************************************/

#ifndef _BSP_DEBUG_H_
#define _BSP_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "hc32_ddl.h"

#define	dbstart		(0)
#define	valid		(1)
#define	end			(2)

#define	DBG_CMD_HEAD_START						('$')
#define	DBG_CMD_HEAD_END						(':')
#define	DBG_CMD_CODE_SIZE						(20)
#define	DEF_DBG_BUFSIZE							(50)
#define	DBG_CMD_PARA_SIZE						(50)

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
/* USART RX/TX Port/Pin definition */
#define USART_RX_PORT                   (GPIO_PORT_D)   /* PD9: USART3_RX */
#define USART_RX_PIN                    (GPIO_PIN_09)
#define USART_RX_GPIO_FUNC              (GPIO_FUNC_37_USART3_RX)

#define USART_TX_PORT                   (GPIO_PORT_D)   /* PD8: USART3_TX */
#define USART_TX_PIN                    (GPIO_PIN_08)
#define USART_TX_GPIO_FUNC              (GPIO_FUNC_36_USART3_TX)

/* USART unit definition */
#define USART_UNIT                      (M4_USART3)
#define USART_FUNCTION_CLK_GATE         (PWC_FCG3_USART3)

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/

// DBG接口数据结构定义

typedef struct	tag_DBGPort_Def
{
		uint8_t			rxFlag;
		uint16_t		txlen;
		uint16_t		rxlen;
		uint8_t			rxdBuf[DEF_DBG_BUFSIZE];
			 
}DBGPort_TypeDef;	

/**********************************************************************************************************
                                    调试信息控制 （user configure）
***********************************************************************************************************/
#define	DBG_INFO_OUTPUTEN			 (1)

#define SOFTWARE_VERSION   			 ("1.01")

/**********************************************************************************************************
**                                    调试命令列表( user configure)
**********************************************************************************************************/
#define	DBG_CMD_CODE_QVERB						("Q_VERB")				//  	命令格式：“$Q_VERB\r\n"			查询版本号	

#define	DBG_CMD_CODE_QUEIP						("Q_IP")				//		命令格式：“$Q_IP\r\n"			查询IP地址及端口号

#define	DBG_CMD_CODE_QUEADC_AMB					("Q_AD_Amb")			//  	命令格式：”$Q_AD_Amb\r\n"		查询设备Amb ADC数值

#define	DBG_CMD_CODE_QUEADC_AUX					("Q_AD_Aux")			//  	命令格式：”$Q_AD_Aux\r\n"		查询设备Aux ADC数值

#define	DBG_CMD_CODE_QUEADC_BUS1				("Q_AD_Bus1")			//  	命令格式：”$Q_AD_Bus1\r\n"		查询设备BUS1 ADC数值

#define	DBG_CMD_CODE_QUEADC_BUS2				("Q_AD_Bus2")			//  	命令格式：”$Q_AD_Bus2\r\n"		查询设备BUS2 ADC数值

#define DBG_CMD_CODE_QUEEEPROM					("Q_EEPROM")			//		命令格式：”$Q_EEPROM\r\n"		查询设备EEPROM数值	

#define DBG_CMD_CODE_QUEFLASH					("Q_FLASH")				//		命令格式：”$Q_FLASH\r\n"		查询设备FLASH数值

#define DBG_CMD_CODE_QUEAD7124					("Q_AD7124")			//		命令格式：”$Q_AD7124\r\n"		查询设备AD7124数值

#define DBG_CMD_CODE_QUEAD7175					("Q_AD7175")			//		命令格式：”$Q_AD7175\r\n"		查询设备AD7175数值

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void bsp_InitUSART(void);
void HC_DBGUART_RxTxISRHandler(void);
void DBGRS232_RxHandler(uint8_t rxData);
void HAL_DBGCmdProcess(void);
void HC_USART_RxIrq_Callback(void);
void HC_USART3_RxErr_IrqCallback(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
