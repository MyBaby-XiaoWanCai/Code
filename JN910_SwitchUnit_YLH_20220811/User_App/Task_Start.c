/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Start.c
** Author      : Cai Heng
** Date        : 2022-07-07
** Description : 启动任务模块
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "Task_Start.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
/* 任务句柄 */
osThreadId_t ThreadIdTaskWorkstep = NULL;
osThreadId_t ThreadIdTaskLed = NULL;
osThreadId_t ThreadIdTaskDcdc = NULL;
osThreadId_t ThreadIdTaskProtection = NULL;

/* 任务属性设置 */
const osThreadAttr_t ThreadWorkstep_Attr =
{
    .name = "osRtxWorkstepThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 10240,
};

const osThreadAttr_t ThreadLed_Attr =
{
    .name = "osRtxLedThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 2048,
};

const osThreadAttr_t ThreadDcdc_Attr =
{
    .name = "osRtxDcdcThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal2,
    .stack_size = 4096,
};

const osThreadAttr_t ThreadProtection_Attr =
{
    .name = "osRtxProtectionThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal2,
    .stack_size = 4096,
};

/* 信号量属性 */
osSemaphoreId_t SemaDcdcCan = NULL;
osSemaphoreId_t SemaProtection = NULL;

const osSemaphoreAttr_t DcdcCanSema_Attr = 
{
	.name = "osRtxDcdcCanSema,",
};

const osSemaphoreAttr_t ProtectionSema_Attr = 
{
	.name = "osRtxProtectionSema,",
};

/* 定时器属性 */

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void CreateTask(void);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : AppTaskStart()
**  功能说明 : 启动任务
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AppTaskStart(void *argument)
{
	uint16_t i = 0;
	
	EepromDatInit();
	
    /* 创建任务 */
    CreateTask();

    while(1)
    {
		if(i >= 1000)
		{
			i = 0;

			HAL_GPIO_TogglePin(GPIO_LED2_GPIO_Port, GPIO_LED2_Pin);				
		}
		else
		{
			i++;
		}
		
        osDelay(START_TASK_PERIOD);
    }
}

/**********************************************************************************************************
**	函 数 名 : CreateTask()
**	功能说明 : 创建应用任务
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void CreateTask(void)
{
	//创建DcdcCan信号量
	SemaDcdcCan = osSemaphoreNew(1, 0, &DcdcCanSema_Attr);
	SemaProtection = osSemaphoreNew(1, 0, &ProtectionSema_Attr);  //创建保护信息同步上抛信号量
	
	//创建任务
	ThreadIdTaskWorkstep = osThreadNew(AppTaskWorkstep, NULL, &ThreadWorkstep_Attr);
	ThreadIdTaskLed = osThreadNew(AppTaskLed, NULL, &ThreadLed_Attr);
	ThreadIdTaskDcdc = osThreadNew(AppTaskDcdc, NULL, &ThreadDcdc_Attr);
	ThreadIdTaskProtection = osThreadNew(AppTaskProtection, NULL, &ThreadProtection_Attr);
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
