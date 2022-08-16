/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : main.h
** Author      : Huang Cheng
** Date        : 2021-11-22
** Description : 
**********************************************************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/

/* ��׼��ͷ�ļ� */
#include  <math.h>
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdint.h>
#include  <stdlib.h>
/* ϵͳ��ͷ�ļ� */
#include "cmsis_os2.h"
#include "rtx_os.h"
/* ������ͷ�ļ� */
/* �汾˵���ļ� */
#include "version.h"

/* ������ͷ�ļ� */
#include "bsp.h"

/* Ӧ�ò�ͷ�ļ� */
#include "app_func_eeprom.h"
#include "app_task_protocol.h"
#include "app_task_ageing.h"
#include "app_task_eth.h"
#include "app_task_acdc_alarm.h"
#include "app_task_led.h"
#include "app_task_protection.h"
#include "app_task_debug.h"
#include "app_timer_Periodic_adc3.h"
#include "app_task_workstep.h"

#include "app_task_signaldetect.h"
#include "app_fct.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
#ifndef TRUE
#define TRUE						1					//��
#endif

#ifndef FALSE
#define FALSE						0					//��
#endif

#ifndef DISABLE_CFG
#define DISABLE_CFG					0					//ʧ��
#endif

#ifndef ENABLE_CFG
#define ENABLE_CFG					1					//ʹ��
#endif

/* APP��ӡ���� */
#if (PRINTF_CFG == ENABLE_CFG)
#define APP_Printf				printf
#else
#define APP_Printf(...)
#endif

/* APP_Printf���� */
#define TASK_ETH_PRINTF_CFG		DISABLE_CFG				//app_task_eth�ļ����ڴ�ӡ���� (Configurable)
#define ETH_INIT_PRINTF_CFG		DISABLE_CFG				//ETH_INIT�ļ����ڴ�ӡ����     (Configurable)

/* ���뼶�жϺ��� */
#define bsp_ProPer1ms  			SysTick_IrqHandler
/* TCPnet�����¼���־ */
#define FLASH_MSG					(1 << 0)
#define EEPROM_MSG				(1 << 1)

//���ذ�汾�궨��
//#define C041A_7175
//#define C041B_7175
#define C042A_7606

//���ذ�汾�궨��
#define LM5170_AD7175			(0x00)
#define LM5170_AD7606			(0x01)
#define LM5170_SGM58601		(0x02)

#define C042B_BOOM_VER		(0x02)
#define C042C_BOOM_VER		(0x04)

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/


/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
extern uint8_t used_channel_cfg;
extern uint8_t contact_test_once_flag[8];

extern uint8_t ChannelTab[8];
extern uint8_t ALLCH;
extern uint8_t HALFCH1;
extern uint8_t HALFCH2;
extern uint8_t Weaver_Config;
/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
extern void AppTaskLed(void *argument);
extern void AppTaskProtection(void *argument);
uint8_t BarCode_Mac_CheckSet(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
