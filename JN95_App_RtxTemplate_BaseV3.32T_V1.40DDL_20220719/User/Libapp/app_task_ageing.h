/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_task_ageing.h
** Author      : Cai Heng
** Date        : 2021-08-10
** Description :
**********************************************************************************************************/

#ifndef _APP_TASK_AGEING_H_
#define _APP_TASK_AGEING_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "hc32_ddl.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
//�Լ��ϻ�������������(ms)
#define AGEING_TASK_PERIOD			100				// (Configure)

//g_uiErrFlag��ϸ����
#define NO_ERR						0				//�޹���
#define SELF_CHECKING_ERR			(1 << 0)		//�Լ����
#define AGEING_ERR					(1 << 1)		//�ϻ�����
#define	SPEED_BREAK_ERR				(1 << 2)		//�ٶϹ���

//��ʱʱ�侫������
#define SECOND_TIME_SCALE			100				//1���Ӱ�������ʱʱ��Ƭ��(100 * 10ms = 1s)
#define DELAY_TIME_SCALE			10				//��ʱʱ�侫��(10ms)
#ifdef YCJ
#define AGEING_TIME_CFG				(6 * 3600)		//6Hour
#else
#define AGEING_TIME_CFG				(6 * 3600)		//6Hour
#endif

//�Լ��ϻ�ʹ�ܱ�ʶ
#define AGEING_ENABLE				1				//�ɽ����Լ��ϻ�
#define AGEING_DISABLE				0				//���ɽ����Լ��ϻ�

//�Լ��ϻ���ַ
#define AGEING_LIB_ADDR				8				//��ţ���ǰ���ư��IP��һλʶ�������⣬����ԭ����MCU��PH6������
#define AGEING_BIT_ADDR				42				//λ��

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/


/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/
extern uint8_t  g_ucSysMode;
extern uint8_t  ChannelErrorFlag;
extern uint8_t  ChannelStopFlag;
extern uint8_t  ChannelError[8];

extern float current_record_data[8];
extern float voltage_record_data[8];

extern uint8_t ageing_flag;		// 0 - δִ���ϻ�  1 - ��ִ���ϻ�

extern uint8_t  ChannelErrorFlag_Store;
extern uint8_t  ChannelError_Store[8];

extern float bus1_data;

extern float bus2_data;

//extern uint16_t device_err_flag;

extern uint16_t module_err_flag[8];
/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/
extern void Chg_Dis_On_Cfg(uint8_t ch, uint8_t dir, uint8_t vmod, float vol, float cur);
extern void Chg_Dis_Off_Cfg(uint8_t ch);
extern void Protect_Trigger_Judgemet(void);
extern void AppTaskAgeing(void *argument);
	
#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
