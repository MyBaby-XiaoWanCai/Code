/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Workstep.h
** Author      : Yi Li Hua
** Date        : 2022-08-08
** Description : 
**********************************************************************************************************/

#ifndef _TASK_PROTECTION_H_
#define _TASK_PROTECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
#define PROTECTION_TASK_PERIOD					1
#define PROT_TRIG_MS(trig_time, period_time)	(trig_time / period_time)
#define ALLCH  									0xFF
/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/


/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/
extern uint8_t  SysModeProtMaskFlag;
extern uint64_t IP_Alarm_Flag[8];         // �ڲ�������־λ
extern uint32_t GP_Alarm_Flag[8];         // ȫ�ֱ�����־λ


/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/
uint32_t IntelProtectDeviceCheck(void);
uint32_t IntelProtectRunParmCheck(void);
uint32_t GlobleProtectRunParmCheck(void);
void AppTaskProtection(void *argument);
void AlarmClear(void);


#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/