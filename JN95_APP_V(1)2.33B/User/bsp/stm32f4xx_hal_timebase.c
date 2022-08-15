/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : stm32f4xx_hal_timbase.c
** Author      : Cai Heng
** Date        : 2021-06-16
** Description : This file is used to provide timebase to the HAL library
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
#if (HAL_TIMEBASE == TIM7_HAL_TIMEBASE)
TIM_HandleTypeDef   TimHandle = {0};
#endif

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void HAL_Delay(uint32_t Delay);
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);
uint32_t HAL_GetTick(void);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

#if (HAL_TIMEBASE == TIM7_HAL_TIMEBASE)		/* ����TIM7���� */

/**********************************************************************************************************
**	�� �� �� : HAL_InitTick()
**	����˵�� : ΪHAL������1ms��ʱ���׼(�˺����ᱻHAL_Init��HAL_RCC_ClockConfig����)
**	��    �� : TickPriority - ��ʱ�����ȼ�
**	�� �� ֵ : HAL_ERROR - �쳣
**             HAL_OK    - ����
**********************************************************************************************************/
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
	RCC_ClkInitTypeDef    clkconfig;
	uint32_t              uwTimclock, uwAPB1Prescaler = 0U;
	uint32_t              uwPrescalerValue = 0U;
	uint32_t              pFLatency;

	/* ��λ��ʱ��7 */
	TimHandle.Instance = TIM7;
	if(HAL_TIM_Base_DeInit(&TimHandle) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}
	
	/* ����TIM7���ж����ȼ���ʹ�� */
	HAL_NVIC_SetPriority(TIM7_IRQn, 5, 0U);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);

	/* ʹ��TIM7ʱ�� */
	__HAL_RCC_TIM7_CLK_ENABLE();

	/*-----------------------------------------------------------------------
        System Clock source       = PLL (HSE)
        SYSCLK(Hz)                = 400000000 (CPU Clock)
        HCLK(Hz)                  = 200000000 (AXI and AHBs Clock)
        AHB Prescaler             = 2
        D1 APB3 Prescaler         = 2 (APB3 Clock  100MHz)
        D2 APB1 Prescaler         = 2 (APB1 Clock  100MHz)
        D2 APB2 Prescaler         = 2 (APB2 Clock  100MHz)
        D3 APB4 Prescaler         = 2 (APB4 Clock  100MHz)

        ��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = APB1 x 2 = 200MHz;
        ��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = APB2 x 2 = 200MHz;
        APB4�����TIMxCLKû�з�Ƶ�����Ծ���100MHz;

        APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14��LPTIM1
        APB2 ��ʱ���� TIM1, TIM8 , TIM15, TIM16��TIM17
        APB4 ��ʱ���� LPTIM2��LPTIM3��LPTIM4��LPTIM5
	----------------------------------------------------------------------- */
	
	/* ��ȡʱ������ */
	HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

	/* ��ȡAPB1ʱ�� */
	uwAPB1Prescaler = clkconfig.APB1CLKDivider;

	/* ���TIM7ʱ�� */
	if (uwAPB1Prescaler == RCC_HCLK_DIV1) 
	{
		uwTimclock = HAL_RCC_GetPCLK1Freq();
	}
	else
	{
		uwTimclock = 2 * HAL_RCC_GetPCLK1Freq();
	}

	/* TIM7��Ƶ��1MHz */
	uwPrescalerValue = (uint32_t)((uwTimclock / 1000000U) - 1U);

	/* TIM7CLK = uwTimclock / (Period + 1) / (Prescaler + 1) = 1KHz */
	TimHandle.Init.Period = (1000000U / 1000U) - 1U;
	TimHandle.Init.Prescaler = uwPrescalerValue;
	TimHandle.Init.ClockDivision = 0;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	if(HAL_TIM_Base_Init(&TimHandle) == HAL_OK)
	{
		/* ����TIM7 */
		return HAL_TIM_Base_Start_IT(&TimHandle);
	}

	/* ���ش��� */
	return HAL_ERROR;
}

/**********************************************************************************************************
**	�� �� �� : HAL_SuspendTick()
**	����˵�� : �ر�TIM7
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void HAL_SuspendTick(void)
{
	__HAL_TIM_DISABLE_IT(&TimHandle, TIM_IT_UPDATE);
}

/**********************************************************************************************************
**	�� �� �� : HAL_ResumeTick()
**	����˵�� : ʹ��TIM7
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void HAL_ResumeTick(void)
{
	__HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);
}

/**********************************************************************************************************
**	�� �� �� : TIM7_IRQHandler()
**	����˵�� : TIM7��ʱ���жϷ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void TIM7_IRQHandler(void)
{
	if((TIM7->SR & TIM_FLAG_UPDATE) != RESET)
	{
		 TIM7->SR = ~TIM_FLAG_UPDATE;
		 HAL_IncTick();
	}
}

#else										/* ����RTX5�ں�ʱ�ӷ��� */

/**********************************************************************************************************
**	�� �� �� : HAL_Delay()
**	����˵�� : �ض�������ӳٺ������滻HAL�еĺ�������ΪHAL�е�ȱʡ����������Systick�жϣ������USB��SD��
**             �ж������ӳٺ������������Ҳ����ͨ������HAL_NVIC_SetPriority����Systick�жϡ�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void HAL_Delay(uint32_t Delay)
{
	bsp_DelayMS(Delay);
}

/**********************************************************************************************************
**	�� �� �� : HAL_InitTick()
**	����˵�� : ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
	return HAL_OK;
}

/**********************************************************************************************************
**	�� �� �� : HAL_GetTick()
**	����˵�� : ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
uint32_t HAL_GetTick(void) 
{
	static uint32_t ticks = 0U;
	uint32_t i;

	if (osKernelGetState() == osKernelRunning)
	{
		return ((uint32_t)osKernelGetTickCount());
	}

	/* ���RTX5��û�����У��������淽ʽ */
	for (i = (SystemCoreClock >> 14U); i > 0U; i--) 
	{
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
	}
	
	return ++ticks;
}

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/