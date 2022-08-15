/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_debug.h
** Author      : Huang Cheng
** Date        : 2021-11-15
** Description : ���ô��ڴ�ӡ���ڵ���
**********************************************************************************************************/

#ifndef _BSP_DEBUG_H_
#define _BSP_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
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
**                                            �궨����
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
**                                           ���Ͷ�����
**********************************************************************************************************/

// DBG�ӿ����ݽṹ����

typedef struct	tag_DBGPort_Def
{
		uint8_t			rxFlag;                   //���ڽ��ձ�־
        uint16_t		txlen;                    //�������ݳ���
		uint16_t		rxlen;                    //�������ݳ���
		uint8_t			rxdBuf[DEF_DBG_BUFSIZE];  //����buffer
		uint8_t			rxdBuf1[DEF_DBG_BUFSIZE]; //����buffer1	 
}DBGPort_TypeDef;	

/**********************************************************************************************************
                                    ������Ϣ���� ��user configure��
***********************************************************************************************************/
#define	DBG_INFO_OUTPUTEN			 (1)

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
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
	
/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
void bsp_InitUSART(void);
void HC_DBGUART_RxTxISRHandler(void);
void DBGRS232_RxHandler(uint8_t rxData);
void HAL_DBGCmdProcess(void);
void HC_USART_RxIrq_Callback(void);
void HC_USART3_RxErr_IrqCallback(void);
int32_t fputc(int32_t ch, FILE *f);


/**********************************************************************************************************
**                                           ����������
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
