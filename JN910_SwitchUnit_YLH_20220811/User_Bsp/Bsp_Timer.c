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
**                                          头文件引用区
**********************************************************************************************************/
#include "Bsp_Timer.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
*	函 数 名: bsp_RCC_TIM_Enable
*	功能说明: 使能TIM RCC 时钟
*	形    参: TIMx TIM1 - TIM14
*	返 回 值: 无
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
*	函 数 名: bsp_RCC_TIM_Disable
*	功能说明: 关闭TIM RCC 时钟
*	形    参: TIMx TIM1 - TIM14
*	返 回 值: TIM外设时钟名
**********************************************************************************************************/
void bsp_RCC_TIM_Disable(TIM_TypeDef* TIMx)
{
    /*
    	APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
    	APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
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
*	函 数 名: bsp_SetTIMforInt
*	功能说明: 配置TIM和NVIC，用于简单的定时中断，开启定时中断。另外注意中断服务程序需要由用户应用程序实现。
*	形    参: TIMx : 定时器
*			  _ulFreq : 定时频率 （Hz）。 0 表示关闭。
*			  _PreemptionPriority : 抢占优先级
*			  _SubPriority : 子优先级
*	返 回 值: 无
**********************************************************************************************************/
void bsp_SetTIMforInt(TIM_TypeDef* TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority)
{
    TIM_HandleTypeDef   TimHandle = {0};
    uint16_t usPeriod;
    uint16_t usPrescaler;
    uint32_t uiTIMxCLK;

    /* 使能TIM时钟 */
    bsp_RCC_TIM_Enable(TIMx);

    /*-----------------------------------------------------------------------
    	system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

    	HCLK = SYSCLK / 1     (AHB1Periph)
    	PCLK2 = HCLK / 2      (APB2Periph)
    	PCLK1 = HCLK / 4      (APB1Periph)

    	因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
    	因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

    	APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
    	APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11

    ----------------------------------------------------------------------- */
    if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
    {
        /* APB2 定时器时钟 = 168M */
        uiTIMxCLK = SystemCoreClock;
    }
    else
    {
        /* APB1 定时器 = 84M */
        uiTIMxCLK = SystemCoreClock / 2;
    }

    if (_ulFreq < 100)
    {
        usPrescaler = 10000 - 1;					/* 分频比 = 10000 */
        usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* 自动重装的值 */
    }
    else if (_ulFreq < 3000)
    {
        usPrescaler = 100 - 1;					/* 分频比 = 100 */
        usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* 自动重装的值 */
    }
    else	/* 大于4K的频率，无需分频 */
    {
        usPrescaler = 0;					/* 分频比 = 1 */
        usPeriod = uiTIMxCLK / _ulFreq - 1;	/* 自动重装的值 */
    }

    /*
       定时器中断更新周期 = TIMxCLK / usPrescaler + 1）/usPeriod + 1）
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

    /* 使能定时器中断  */
    __HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);


    /* 配置TIM定时更新中断 (Update) */
    {
        uint8_t irq = 0;	/* 中断号, 定义在 stm32h7xx.h */

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
