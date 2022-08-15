/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Bsp_Timer.c
** Author      : Cai Heng
** Date        : 2022-08-01
** Description : 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Bsp_Timer.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
*	�� �� ��: bsp_RCC_TIM_Enable
*	����˵��: ʹ��TIM RCC ʱ��
*	��    ��: TIMx TIM1 - TIM14
*	�� �� ֵ: ��
**********************************************************************************************************/
void bsp_RCC_TIM_Enable(TIM_TypeDef* TIMx)
{
    if (TIMx == TIM1) __HAL_RCC_TIM1_CLK_ENABLE();
    else if (TIMx == TIM2) __HAL_RCC_TIM2_CLK_ENABLE();
    else if (TIMx == TIM3) __HAL_RCC_TIM3_CLK_ENABLE();
    else if (TIMx == TIM4) __HAL_RCC_TIM4_CLK_ENABLE();
    else if (TIMx == TIM5) __HAL_RCC_TIM5_CLK_ENABLE();
    else if (TIMx == TIM6) __HAL_RCC_TIM6_CLK_ENABLE();
    else if (TIMx == TIM7) __HAL_RCC_TIM7_CLK_ENABLE();
    else if (TIMx == TIM8) __HAL_RCC_TIM8_CLK_ENABLE();
    else if (TIMx == TIM9) __HAL_RCC_TIM9_CLK_ENABLE();
    else if (TIMx == TIM10) __HAL_RCC_TIM10_CLK_ENABLE();
    else if (TIMx == TIM11) __HAL_RCC_TIM11_CLK_ENABLE();
    else if (TIMx == TIM12) __HAL_RCC_TIM12_CLK_ENABLE();
    else if (TIMx == TIM13) __HAL_RCC_TIM13_CLK_ENABLE();
    else if (TIMx == TIM14) __HAL_RCC_TIM14_CLK_ENABLE();
}

/**********************************************************************************************************
*	�� �� ��: bsp_RCC_TIM_Disable
*	����˵��: �ر�TIM RCC ʱ��
*	��    ��: TIMx TIM1 - TIM14
*	�� �� ֵ: TIM����ʱ����
**********************************************************************************************************/
void bsp_RCC_TIM_Disable(TIM_TypeDef* TIMx)
{
    /*
    	APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
    	APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11
    */
    if (TIMx == TIM1) __HAL_RCC_TIM3_CLK_DISABLE();
    else if (TIMx == TIM2) __HAL_RCC_TIM2_CLK_DISABLE();
    else if (TIMx == TIM3) __HAL_RCC_TIM3_CLK_DISABLE();
    else if (TIMx == TIM4) __HAL_RCC_TIM4_CLK_DISABLE();
    else if (TIMx == TIM5) __HAL_RCC_TIM5_CLK_DISABLE();
    else if (TIMx == TIM6) __HAL_RCC_TIM6_CLK_DISABLE();
    else if (TIMx == TIM7) __HAL_RCC_TIM7_CLK_DISABLE();
    else if (TIMx == TIM8) __HAL_RCC_TIM8_CLK_DISABLE();
    else if (TIMx == TIM9) __HAL_RCC_TIM9_CLK_DISABLE();
    else if (TIMx == TIM10) __HAL_RCC_TIM10_CLK_DISABLE();
    else if (TIMx == TIM11) __HAL_RCC_TIM11_CLK_DISABLE();
    else if (TIMx == TIM12) __HAL_RCC_TIM12_CLK_DISABLE();
    else if (TIMx == TIM13) __HAL_RCC_TIM13_CLK_DISABLE();
    else if (TIMx == TIM14) __HAL_RCC_TIM14_CLK_DISABLE();
}

/**********************************************************************************************************
*	�� �� ��: bsp_SetTIMforInt
*	����˵��: ����TIM��NVIC�����ڼ򵥵Ķ�ʱ�жϣ�������ʱ�жϡ�����ע���жϷ��������Ҫ���û�Ӧ�ó���ʵ�֡�
*	��    ��: TIMx : ��ʱ��
*			  _ulFreq : ��ʱƵ�� ��Hz���� 0 ��ʾ�رա�
*			  _PreemptionPriority : ��ռ���ȼ�
*			  _SubPriority : �����ȼ�
*	�� �� ֵ: ��
**********************************************************************************************************/
void bsp_SetTIMforInt(TIM_TypeDef* TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority)
{
    TIM_HandleTypeDef   TimHandle = {0};
    uint16_t usPeriod;
    uint16_t usPrescaler;
    uint32_t uiTIMxCLK;

    /* ʹ��TIMʱ�� */
    bsp_RCC_TIM_Enable(TIMx);

    /*-----------------------------------------------------------------------
    	system_stm32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�

    	HCLK = SYSCLK / 1     (AHB1Periph)
    	PCLK2 = HCLK / 2      (APB2Periph)
    	PCLK1 = HCLK / 4      (APB1Periph)

    	��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
    	��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;

    	APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
    	APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11

    ----------------------------------------------------------------------- */
    if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
    {
        /* APB2 ��ʱ��ʱ�� = 168M */
        uiTIMxCLK = SystemCoreClock;
    }
    else
    {
        /* APB1 ��ʱ�� = 84M */
        uiTIMxCLK = SystemCoreClock / 2;
    }

    if (_ulFreq < 100)
    {
        usPrescaler = 10000 - 1;					/* ��Ƶ�� = 10000 */
        usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* �Զ���װ��ֵ */
    }
    else if (_ulFreq < 3000)
    {
        usPrescaler = 100 - 1;					/* ��Ƶ�� = 100 */
        usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* �Զ���װ��ֵ */
    }
    else	/* ����4K��Ƶ�ʣ������Ƶ */
    {
        usPrescaler = 0;					/* ��Ƶ�� = 1 */
        usPeriod = uiTIMxCLK / _ulFreq - 1;	/* �Զ���װ��ֵ */
    }

    /*
       ��ʱ���жϸ������� = TIMxCLK / usPrescaler + 1��/usPeriod + 1��
    */
    TimHandle.Instance = TIMx;
    TimHandle.Init.Prescaler         = usPrescaler;
    TimHandle.Init.Period            = usPeriod;
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
//		Error_Handler(__FILE__, __LINE__);
    }

    /* ʹ�ܶ�ʱ���ж�  */
    __HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);


    /* ����TIM��ʱ�����ж� (Update) */
    {
        uint8_t irq = 0;	/* �жϺ�, ������ stm32h7xx.h */

        if (TIMx == TIM1) irq = TIM1_UP_TIM10_IRQn;
        else if (TIMx == TIM2) irq = TIM2_IRQn;
        else if (TIMx == TIM3) irq = TIM3_IRQn;
        else if (TIMx == TIM4) irq = TIM4_IRQn;
        else if (TIMx == TIM5) irq = TIM5_IRQn;
        else if (TIMx == TIM6) irq = TIM6_DAC_IRQn;
        else if (TIMx == TIM7) irq = TIM7_IRQn;
        else if (TIMx == TIM8) irq = TIM8_UP_TIM13_IRQn;
        else if (TIMx == TIM9) irq = TIM1_BRK_TIM9_IRQn;
        else if (TIMx == TIM10) irq = TIM1_UP_TIM10_IRQn;
        else if (TIMx == TIM11) irq =  TIM1_TRG_COM_TIM11_IRQn;
        else if (TIMx == TIM12) irq = TIM8_BRK_TIM12_IRQn;
        else if (TIMx == TIM13) irq = TIM8_UP_TIM13_IRQn;
        else if (TIMx == TIM14) irq = TIM8_TRG_COM_TIM14_IRQn;
        else
        {
//			Error_Handler(__FILE__, __LINE__);
        }
        HAL_NVIC_SetPriority((IRQn_Type)irq, _PreemptionPriority, _SubPriority);
        HAL_NVIC_EnableIRQ((IRQn_Type)irq);
    }

    __HAL_TIM_ENABLE_DMA(&TimHandle, TIM_DMA_UPDATE);

    HAL_TIM_Base_Start(&TimHandle);
}


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
