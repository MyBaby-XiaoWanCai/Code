/**********************************************************************************************************
*
*	ģ������ : debug ������
*	�ļ����� : bsp_debug.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*	��	 �ߣ�Robert.Li
**��	 ��:  2021.7.5
**********************************************************************************************************/

#ifndef _BSP_DEBUG_H_
#define _BSP_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif





/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "stm32f4xx.h"


/**********************************************************************************************************

HW : JN-MPU-C022F ��Debug PIN (USART3)ӳ���ϵ
	
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
                                    ������Ϣ���� ��user configure��
***********************************************************************************************************/


#define	DBG_INFO_OUTPUTEN					(1)

#define SOFTWARE_VERSION   			 ("1.01")	








/**********************************************************************************************************
**                                    ���������б�( user configure)
**********************************************************************************************************/


#define	DBG_CMD_CODE_QVERB						("Q_VERB")				//  	�����ʽ����$Q_VERB\r\n"			��ѯ�汾��	

#define	DBG_CMD_CODE_QUEIP						("Q_IP")				//		�����ʽ����$Q_IP\r\n"			��ѯIP��ַ���˿ں�

#define	DBG_CMD_CODE_QUEADC_AMB					("Q_AD_Amb")			//  	�����ʽ����$Q_AD_Amb\r\n"		��ѯ�豸Amb ADC��ֵ

#define	DBG_CMD_CODE_QUEADC_AUX					("Q_AD_Aux")			//  	�����ʽ����$Q_AD_Aux\r\n"		��ѯ�豸Aux ADC��ֵ

#define	DBG_CMD_CODE_QUEADC_BUS1				("Q_AD_Bus1")			//  	�����ʽ����$Q_AD_Bus1\r\n"		��ѯ�豸BUS1 ADC��ֵ

#define	DBG_CMD_CODE_QUEADC_BUS2				("Q_AD_Bus2")			//  	�����ʽ����$Q_AD_Bus2\r\n"		��ѯ�豸BUS2 ADC��ֵ

#define DBG_CMD_CODE_QUEEEPROM					("Q_EEPROM")			//		�����ʽ����$Q_EEPROM\r\n"		��ѯ�豸EEPROM��ֵ	

#define DBG_CMD_CODE_QUEFLASH					("Q_FLASH")				//		�����ʽ����$Q_FLASH\r\n"		��ѯ�豸FLASH��ֵ

#define DBG_CMD_CODE_QUEAD7124					("Q_AD7124")			//		�����ʽ����$Q_AD7124\r\n"		��ѯ�豸AD7124��ֵ

#define DBG_CMD_CODE_QUEAD7175					("Q_AD7175")			//		�����ʽ����$Q_AD7175\r\n"		��ѯ�豸AD7175��ֵ

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/



// DBG�ӿ����ݽṹ����

typedef struct	tag_DBGPort_Def
{
		uint8_t			rxFlag;
		uint16_t			txlen;
		uint16_t			rxlen;
		uint8_t			rxdBuf[DEF_DBG_BUFSIZE];
			 
}DBGPort_TypeDef;	







/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
extern uint32_t ad7124_value[16];
extern uint32_t ad7175_value[16];	
	
/**********************************************************************************************************
**                                         �ⲿ���ú�����
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
