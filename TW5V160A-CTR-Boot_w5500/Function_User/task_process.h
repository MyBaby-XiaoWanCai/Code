/*
*********************************************************************************************************
*
*	ģ������ : ������
*	�ļ����� : task_process.h
*	��    �� : V1.0
*	˵    �� :
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0    2020-03-12   ZYL      ��ʽ����
*********************************************************************************************************
*/
#ifndef _task_process_H_
#define _task_process_H_

#include "stdint.h"


typedef  void ( *FTimer_P) ( void );

typedef struct {
    uint16_t    Task_Period;        //time out for calling function
    uint16_t    Task_Count;
    FTimer_P    Task_Function;      //Send function defines in application
}TaskTime;

extern void Timer_Task_Count(void); // �¼��������
extern void Task_RUN(void);         // ��������
extern void Task_Add(void);         // �������
extern void Task_LED(void);
extern void task_send_Rece(void);   // CANͨѶ�ķ���

#endif

/***************************** (END OF FILE) *********************************/
