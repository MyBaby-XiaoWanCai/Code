/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_lan.h
** Author      : Robert
** Date        : 2021-07-22
** Description : 
**********************************************************************************************************/
#ifndef _BSP_LAN_H_
#define _BSP_LAN_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "stm32f4xx.h"
#include "string.h"
#include "socket.h"
#include "cmsis_os2.h"
/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/


#define LAN_DEBUG_ENABLE		1
#define LAN_DEBUG_DISABLE		0
#define LAN_PRINTF_CFG			LAN_DEBUG_ENABLE		//bsp_lan�ļ����ڴ�ӡ����   (Configurable)


#define W5500_SPI				SPI1					//W5500ʹ�õ�SPI  (Configurable)
#define W5500_CS_ENABLE()		SPI1_CS_ENABLE()		//W5500Ƭѡ����   (Configurable)
#define W5500_CS_DISABLE()		SPI1_CS_DISABLE()		//W5500Ƭѡ����   (Configurable)
#define W5500_SPI_INIT()		bsp_InitSpi1()			//W5500�ӿڳ�ʼ�� (Configurable)


#define IP4_ADDR_1			192
#define IP4_ADDR_2			168
#define IP4_ADDR_3			1
#define IP4_ADDR_4			11				//IPv4��ַ	(Configurable)

#define SUB_MASK_1			255
#define SUB_MASK_2			255
#define SUB_MASK_3			0
#define SUB_MASK_4			0				//��������	(Configurable)

#define DEF_GATE_1			192
#define DEF_GATE_2			168
#define DEF_GATE_3			1
#define DEF_GATE_4			1				//Ĭ������	(Configurable)

#define DNS_ADDR_1			8
#define DNS_ADDR_2			8
#define DNS_ADDR_3			8
#define DNS_ADDR_4			8				//DNS������	(Configurable)

#define SOCK_0_NUMB			0				//Socket 0  (Configurable)


#define SOCK_0_PORT			5000			//�������˿�    (Configurable)

#define DAT_BUF_SIZE		2048			//�����С  (Configurable)

#define PHYLINK_CHECKNUM	10				//PHY����״̬��������

#define GPIO_PORT_W5500_RST     GPIOI
#define GPIO_PIN_W5500_RST  GPIO_PIN_7

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/
typedef struct {
uint8_t g_ucDatBuff[DAT_BUF_SIZE];
uint16_t rcv_len;
} rcv_Struct;

/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
extern osSemaphoreId_t w5500_rcv_Semaphore;        /* ���ڽ����ź���ID */	
void W5500_RST_Init(void);

extern uint8_t client_flag;

/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	


#ifdef __cplusplus
}
#endif	

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
