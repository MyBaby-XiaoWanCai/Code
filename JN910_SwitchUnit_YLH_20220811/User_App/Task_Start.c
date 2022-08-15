/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Start.c
** Author      : Cai Heng
** Date        : 2022-07-07
** Description : ��������ģ��
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Task_Start.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
/* ������ */
osThreadId_t ThreadIdTaskWorkstep = NULL;
osThreadId_t ThreadIdTaskLed = NULL;
osThreadId_t ThreadIdTaskDcdc = NULL;
osThreadId_t ThreadIdTaskProtection = NULL;

/* ������������ */
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

/* �ź������� */
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

/* ��ʱ������ */

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void CreateTask(void);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : AppTaskStart()
**  ����˵�� : ��������
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void AppTaskStart(void *argument)
{
	uint16_t i = 0;
	
	EepromDatInit();
	
    /* �������� */
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
**	�� �� �� : CreateTask()
**	����˵�� : ����Ӧ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void CreateTask(void)
{
	//����DcdcCan�ź���
	SemaDcdcCan = osSemaphoreNew(1, 0, &DcdcCanSema_Attr);
	SemaProtection = osSemaphoreNew(1, 0, &ProtectionSema_Attr);  //����������Ϣͬ�������ź���
	
	//��������
	ThreadIdTaskWorkstep = osThreadNew(AppTaskWorkstep, NULL, &ThreadWorkstep_Attr);
	ThreadIdTaskLed = osThreadNew(AppTaskLed, NULL, &ThreadLed_Attr);
	ThreadIdTaskDcdc = osThreadNew(AppTaskDcdc, NULL, &ThreadDcdc_Attr);
	ThreadIdTaskProtection = osThreadNew(AppTaskProtection, NULL, &ThreadProtection_Attr);
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
