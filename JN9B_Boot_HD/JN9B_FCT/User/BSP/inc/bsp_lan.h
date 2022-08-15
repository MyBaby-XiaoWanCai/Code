/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_lan.h
** Author      : Huang Cheng
** Date        : 2021-11-04
** Description : bsp lan head file
**********************************************************************************************************/

#ifndef __BSP_LAN_H
#define __BSP_LAN_H

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include <string.h>
#include "hc32_ddl.h"
#include "wizchip_conf.h"
#include "w5500.h"
#include "w5500socket.h"
#include "bsp_spi.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
/*
#define MAC_ADDR_1			0x0c
#define MAC_ADDR_2			0x29
#define MAC_ADDR_3			0xab
#define MAC_ADDR_4			0x7c
#define MAC_ADDR_5			0x04
#define MAC_ADDR_6			0x01			//MAC Addr	(Configurable)
*/

#define MAC_ADDR_1			30
#define MAC_ADDR_2			30
#define MAC_ADDR_3			47
#define MAC_ADDR_4			89
#define MAC_ADDR_5			55
#define MAC_ADDR_6			67			//MAC Addr	(Configurable)

#define IP4_ADDR_1			192
#define IP4_ADDR_2			168
#define IP4_ADDR_3			1
#define IP4_ADDR_4			11				//IPv4 Addr	(Configurable)

#define SUB_MASK_1			255
#define SUB_MASK_2			255
#define SUB_MASK_3			0
#define SUB_MASK_4			0				//????	(Configurable)

#define DEF_GATE_1			192
#define DEF_GATE_2			168
#define DEF_GATE_3			1
#define DEF_GATE_4			1				//????	(Configurable)

#define DNS_ADDR_1			8
#define DNS_ADDR_2			8
#define DNS_ADDR_3			8
#define DNS_ADDR_4			8				//DNS	(Configurable)

#define SOCK_0_NUMB			0				//Socket 0  (Configurable)
#define SOCK_1_NUMB			1				//Socket 1  (Configurable)
#define SOCK_0_PORT			5000			//Port Num  (Configurable)
#define DATA_BUF_SIZE		2048			//BUFF SIZE (Configurable)

#define GPIO_PORT_W5500_RST GPIO_PORT_I
#define GPIO_PIN_W5500_RST  GPIO_PIN_07

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
typedef struct {
uint8_t g_ucDatBuff[DATA_BUF_SIZE];
uint16_t rcv_len;
} rcv_Struct;

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	

void W5500_RST_Init(void);
void Lan_cfg(void);
void bsp_InitW5500(void);
void W5500_TcpServer(void);
void W5500_TcpClient(void);
void W5500_Poll(void);
void W5500_TCP_Client(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

