/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp.h
** Author      : Huang Cheng
** Date        : 2022-01-23
** Description : 
**********************************************************************************************************/
#ifndef _BSP_H_
#define _BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
/* DDL��ͷ�ļ�  */
#include "hc32_ddl.h"

/* ��׼��ͷ�ļ� */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* �ײ�����ͷ�ļ� */
#include "bsp_gpio.h"
#include "bsp_led.h" 
#include "bsp_protection.h"
#include "bsp_spi.h"
#include "bsp_exti.h"
#include "bsp_lan.h"
#include "bsp_ad7124.h"
#include "bsp_ad7175.h"
#include "bsp_dac80508.h"
#include "bsp_charge.h"
#include "bsp_flash.h"
#include "bsp_MB85RS2.h"
#include "bsp_eeprom.h"
#include "bsp_AD5245.h"
#include "bsp_adc3.h"
#include "bsp_timer.h"
#include "bsp_systick.h"
#include "bsp_dwt.h"
#include "bsp_swdt.h"
#include "bsp_acdc.h"
#include "bsp_debug.h"
#include "bsp_rng.h"
#include "ringbuffer.h"

#include "bsp_i2c.h"
#include "bsp_pca9555.h"
#include "bsp_tmp75.h"
#include "bsp_pwm.h"
#include "bsp_versmana.h"
#include "bsp_alarm.h"
//#include "bsp_9548.h"

#include "bsp_7606.h"
#include "bsp_SGM58601.h"
#include "bsp_dac.h"

/* ��������ͷ�ļ� */
#include "global.h"

#include "Can_Network_Global.h"
#include "Can_Network_Driver.h"
#include "Can_Network_Protocol.h"
#include "Can_Network_Transplant.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/

#ifndef TRUE
#define TRUE					1					//��
#endif

#ifndef FALSE
#define FALSE					0					//��
#endif

#ifndef DISABLE_CFG
#define DISABLE_CFG				0					//ʧ��
#endif

#ifndef ENABLE_CFG
#define ENABLE_CFG				1					//ʹ��
#endif

/* ����ȫ���ж� */
#define ENABLE_INT()			__set_PRIMASK(0)	//ʹ��ȫ���ж�
#define DISABLE_INT()			__set_PRIMASK(1)	//��ֹȫ���ж�

/* �ж����ȼ����� */
#define NVIC_PREEMPT_PRIORITY	4					//(Configurable, default: 4)

/* ����������� */
#define	Enable_EventRecorder	FALSE				//(Configurable)

/* ����ϵͳ���� */
#define	USE_RTX    				TRUE				//(Configurable)

/* ����Ӳ������ */
#define PHY_KSZ8041				FALSE				//(Configurable)

/* ���ڴ�ӡ����(��) */
#define PRINTF_CFG				ENABLE_CFG			//(Configurable)

/* BSP_Printf���� */
#define TIMER_PRINTF_CFG		DISABLE_CFG			//bsp_timer�ļ����ڴ�ӡ���� (Configurable)

/* BSP��ӡ���� */
#if (PRINTF_CFG == ENABLE_CFG)
#define BSP_Printf				printf
#else
#define BSP_Printf(...)
#endif

/* ��ʱ����ʼ�������� */
#define ERROR_HANDLER()			Error_Handler(__FILE__, __LINE__);

#if 0
/* CPU����ʱִ�еĺ��� */
#define CPU_IDLE()				bsp_Idle()
#endif

/* �������ͷ�ļ� */
#if (Enable_EventRecorder == TRUE)
#include "EventRecorder.h"
#endif

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/

/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
	
/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
void Dis_int(void);
void En_int(void);
void Dis_int_UserDefine(void);
void En_int_UserDefine(void);
void System_Init(void);
void bsp_Idle(void);
void Error_Handler(char *file, uint32_t line);

void PCA9555_Mode_Init(void);
#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
