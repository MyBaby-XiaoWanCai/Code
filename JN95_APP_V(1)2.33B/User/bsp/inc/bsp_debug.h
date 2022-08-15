/**********************************************************************************************************
*
*	模块名称 : debug 处理函数
*	文件名称 : bsp_debug.h
*	版    本 : V1.0
*	说    明 : 头文件
*	作	 者：Robert.Li
**日	 期:  2021.7.5
**********************************************************************************************************/

#ifndef _BSP_DEBUG_H_
#define _BSP_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif





/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "stm32f4xx.h"


/**********************************************************************************************************

HW : JN-MPU-C022F 　Debug PIN (USART3)映射关系
	
		DEBUG_TX --> PD8    |   DEBUG_RX --> PD9 		
						
*********************************************************************************************************/




#define DEBUG_USART_BAUDRATE                    115200

#define DEBUG_USART                             USART3
#define DEBUG_USART_CLK_ENABLE()                __USART3_CLK_ENABLE();

#define RCC_PERIPHCLK_UARTx                     RCC_PERIPHCLK_USART3
#define RCC_UARTxCLKSOURCE_SYSCLK               RCC_USART3CLKSOURCE_SYSCLK

#define DEBUG_USART_RX_GPIO_PORT                GPIOD
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __GPIOA_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_9
#define DEBUG_USART_RX_AF                       GPIO_AF7_USART3

#define DEBUG_USART_TX_GPIO_PORT                GPIOD
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()        __GPIOD_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_8
#define DEBUG_USART_TX_AF                       GPIO_AF7_USART3

#define DEBUG_USART_IRQHandler                  USART3_IRQHandler
#define DEBUG_USART_IRQ                 		    USART3_IRQn






/**********Cmd Define***********/



#define	dbstart			0
#define	valid			1
#define	end				2


#define	DBG_CMD_HEAD_START						('$')
#define	DBG_CMD_HEAD_END							(':')
#define	DBG_CMD_CODE_SIZE							20
#define	DEF_DBG_BUFSIZE								50
#define	DBG_CMD_PARA_SIZE							50




/**********************************************************************************************************
                                    调试信息控制 （user configure）
***********************************************************************************************************/


#define	DBG_INFO_OUTPUTEN					(1)

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
**                                           类型定义区
**********************************************************************************************************/



// DBG接口数据结构定义

typedef struct	tag_DBGPort_Def
{
		uint8_t			rxFlag;
		uint16_t			txlen;
		uint16_t			rxlen;
		uint8_t			rxdBuf[DEF_DBG_BUFSIZE];
			 
}DBGPort_TypeDef;	







/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern uint32_t ad7124_value[16];
extern uint32_t ad7175_value[16];	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	

void Usart_SendString(uint8_t *str);
void	HAL_DBGUART_RxTxISRHandler (void);
void	HAL_DBGCmdProcess(void);
void bsp_debug_usart_config(void);
extern UART_HandleTypeDef UartHandle;


#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
