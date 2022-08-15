/*
*********************************************************************************************************
*
*	模块名称 : 任务处理
*	文件名称 : task_process.h
*	版    本 : V1.0
*	说    明 :
*
*	修改记录 :
*		版本号  日期         作者     说明
*		V1.0    2020-03-12   ZYL      正式发布
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

extern void Timer_Task_Count(void); // 事件任务计数
extern void Task_RUN(void);         // 任务运行
extern void Task_Add(void);         // 任务添加
extern void Task_LED(void);
extern void task_send_Rece(void);   // CAN通讯的发送

#endif

/***************************** (END OF FILE) *********************************/
