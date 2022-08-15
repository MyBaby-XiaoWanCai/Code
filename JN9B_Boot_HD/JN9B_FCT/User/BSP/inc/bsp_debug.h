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
#define	DEF_DBG_BUFSIZE							(1000)
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
		uint8_t			rxFlag;                   //串口接收标志
        uint16_t		txlen;                    //发送数据长度
		uint16_t		rxlen;                    //接收数据长度
		uint8_t			rxdBuf[DEF_DBG_BUFSIZE];  //接收buffer
		uint8_t			rxdBuf1[DEF_DBG_BUFSIZE]; //接收buffer1	 
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
int32_t fputc(int32_t ch, FILE *f);


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void Uart_Test_Fun(void);
void VBus_Test_Fun(void);
void Vs_Vt_Test_Fun(void);
void Imod_Test_Fun(void);
void Vout_Test_Fun(void);
void En_INT_Test_Fun(void);
void FanErr_Test_Fun(void);
void CSeat_Test_Fun(void);
void PSeat_Test_Fun(void);
void Can_Test_Fun(void);
void Led_I2C_Test_Fun(void);
void PowerBD_I2C_Test_Fun(void);
void CV4_CV8_Test_Fun(void);
void Version_Test_Fun(void);
void HWTD_Test_Fun(void);
void EE_Test_Fun(void);
void Fram_Test_Fun(void);
void Temp_Test_Fun(void);
void PM_Test_Fun(void);

void PVersion_Test_Fun(void);
void PTemp_Test_Fun(void);
void OVP_Test_Fun(void);
void OCP_Test_Fun(void);
void RPP_Test_Fun(void);
void CVmod_C_Test_Fun(void);
void CVsense_FD_Test_Fun(void);
void CVsense_SD_Test_Fun(void);
void CC_10A_Test_Fun(void);
void DCC_120A_Test_Fun(void);

void MB_Auto_Test_Fun(void);
void PB_Auto_Test_Fun(void);
void End_Test_Fun(void);
#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
