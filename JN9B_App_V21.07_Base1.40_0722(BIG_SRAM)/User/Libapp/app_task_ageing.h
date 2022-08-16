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
#define NO_ERR								0				//�޹���
#define SELF_CHECKING_ERR			(1 << 0)		//�Լ����
#define AGEING_ERR						(1 << 1)		//�ϻ�����
#define	SPEED_BREAK_ERR				(1 << 2)		//�ٶϹ���NN

//��ʱʱ�侫������
#define SECOND_TIME_SCALE			100				//1���Ӱ�������ʱʱ��Ƭ��(100 * 10ms = 1s)
#define DELAY_TIME_SCALE			10				//��ʱʱ�侫��(10ms)
#ifdef YCJ
#define AGEING_TIME_CFG				(3 * 3600)      //25*360 (2.5 * 3600)		//5Hour
//#define AGEING_TIME_CFG				(60)
#else
#define AGEING_TIME_CFG				(6 * 3600)		//6hour
#endif

//�Լ��ϻ�ʹ�ܱ�ʶ
#define AGEING_ENABLE					1				//�ɽ����Լ��ϻ�
#define AGEING_DISABLE				0				//���ɽ����Լ��ϻ�

//�Լ��ϻ���ַ
#define AGEING_LIB_ADDR					8				//���
#define AGEING_BIT_ADDR					42				//λ��

#define AGE_RES_ADDR_1 		0x7C80	
#define AGE_RES_ADDR 			0x7C81	

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

extern uint16_t module_err_flag[8];
extern uint8_t  ChannelErrorFlag_Store;
extern uint8_t  ChannelError_Store[8];
extern float bus1_data;
extern float bus2_data;
extern float current_record_data[8];
extern float voltage_record_data[8];

extern uint8_t ageing_cnt_flag;
extern uint32_t ageing_cnt;

/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/
extern void Chg_Dis_On_Cfg(uint8_t ch, uint8_t dir, uint8_t vmod, float vol, float cur);
extern void Chg_Dis_Off_Cfg(uint8_t ch);
extern void Protect_Trigger_Judgemet(void);
extern void AppTaskAgeing(void *argument);
void Ageing(void);
void SelfChecking(void);
void Ageing_Weaver_240A(void);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
