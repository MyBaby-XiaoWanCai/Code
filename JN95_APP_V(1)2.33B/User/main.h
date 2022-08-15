/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : main.h
** Author      : Cai Heng
** Date        : 2021-06-17
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
#include "rl_net.h"  
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
#define bsp_ProPer1ms  			SysTick_ISR

/* TCPnet�����¼���־ */
#define FLASH_MSG				(1 << 0)
#define EEPROM_MSG				(1 << 1)

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/


/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
extern osThreadId_t	ThreadIdTaskMsgPro;	

	
/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
extern void AppTaskLed(void *argument);
extern void AppTaskProtection(void *argument);
extern void AppTaskDebug(void *argument);
extern void AppTaskEth(void *argument);
extern void AppTaskEeprom(void *argument);
extern void AppACDC_alarm(void *argument);
extern void AppTaskProtocol(void *argument);
extern void AppTaskWorkstep(void *argument);
extern void AppTaskMsgPro(void *argument);
extern void AppTaskAgeing(void *argument);
extern void bsp_Init(void);
extern void System_Init(void);
extern void SysTick_ISR(void);
extern void	sys_cfg_set(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
