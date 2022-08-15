/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_systick.c
** Author      : Huang Cheng
** Date        : 2021-11-12
** Description : bsp_systick source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_systick.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
/* 周期轮询调用表 */
RunPerd_TypeDef RunPerdTable[RunPerd_NumMax] = 
{
	{0, RUN1MS_NUM,    NULL},
	{0, RUN10MS_NUM,   Run10MS_TASK},
	{0, RUN25MS_NUM,   NULL},
	{0, RUN100MS_NUM,  Run100MS_TASK},
	{0, RUN500MS_NUM,  NULL},
	{0, RUN1000MS_NUM, Run1000MS_TASK},
};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : bsp_InitSysTick()
**	功能说明 : 初始化SysTick, 时基为1ms, 该函数被bsp_Init()调用
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitSysTick(void)
{
	uint32_t ticks = 0;
	
	ticks = HCLK_VALUE / 1000;  /* (240MHz/1000) / 240MHz = 1ms */
	
	if((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
	{
		return;                                                       /* Reload value impossible */
	}

	SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
	NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
	SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
				     SysTick_CTRL_TICKINT_Msk   |
				     SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
}

/**********************************************************************************************************
**	函 数 名 : SysTick_IrqHandler()
**	功能说明 : 显式定义弱函数SysTick_IrqHandler(被SysTick_Handler调用)，用于多组软件定时器中断处理
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void SysTick_IrqHandler(void)
{	
	/* 查表法来周期性执行函数 */
	for(uint8_t i = 0; i < RunPerd_NumMax; i++)
	{
		RunPerdTable[i].RunPerdCnt++;
		
		if(RunPerdTable[i].RunPerdCnt >= RunPerdTable[i].RunPerdNum)
		{
			RunPerdTable[i].RunPerdCnt = 0x00;
			
			if(NULL != RunPerdTable[i].RunPerdFuncCb)
			{
				RunPerdTable[i].RunPerdFuncCb();
			}
			else
			{
				//do nothing;
			}
		}
		else
		{
			//do nothing;
		}
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
