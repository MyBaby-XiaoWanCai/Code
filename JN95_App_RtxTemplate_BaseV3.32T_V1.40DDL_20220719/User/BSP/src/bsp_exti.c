/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_exti.c
** Author      : Huang Cheng
** Date        : 2021-11-09
** Description : 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_exti.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/

static void AD7175_EXINT_IrqCallback(void);
static void AD7124_EXINT_IrqCallback(void);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : AD7175_EXINT_IrqCallback()
**  功能说明 : AD7175数据中断回调函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void AD7175_EXINT_IrqCallback(void)
{
	static uint16_t ad7175_cnt = 0;
	
	if(Set == EXINT_GetExIntSrc(AD7175_EXINT_CH))
	{
		EXINT_ClrExIntSrc(AD7175_EXINT_CH);
		
		ad7175_cnt++;
		/* Disable interrupt */
		NVIC_DisableIRQ(AD7175_IRQn);
		
		/* 防止AD7175一直触发数据中断，导致其他任务无法正常运行 */
		if(0x00 == ad7175_cnt%4)
		{
			AD7175_Process();
		}
		
		AD7175_NSS_0();
		/* Enable interrupt */
		NVIC_EnableIRQ(AD7175_IRQn);	
	}
	else
	{
		//do nothing;
	}
}

/**********************************************************************************************************
**  函 数 名 : AD7175_EXINT_Init()
**  功能说明 : AD7175数据中断初始化函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AD7175_EXINT_Init(void)
{
    stc_exint_init_t stcExintInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    stc_gpio_init_t stcGpioInit;

    /* GPIO config */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16ExInt = PIN_EXINT_ON;
    stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(AD7175_EXTI_PORT, AD7175_EXTI_PIN, &stcGpioInit);

    /* Exint config */
    (void)EXINT_StructInit(&stcExintInit);
    stcExintInit.u32ExIntCh = AD7175_EXINT_CH;
    stcExintInit.u32ExIntLvl = EXINT_TRIGGER_FALLING;
    (void)EXINT_Init(&stcExintInit);

    /* IRQ sign-in */
    stcIrqSignConfig.enIntSrc = AD7175_INT_SRC;
    stcIrqSignConfig.enIRQn   = AD7175_IRQn;
    stcIrqSignConfig.pfnCallback = &AD7175_EXINT_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);

    /* NVIC config */
    NVIC_ClearPendingIRQ(AD7175_IRQn);
    NVIC_SetPriority(AD7175_IRQn, DDL_IRQ_PRIORITY_14);
    NVIC_DisableIRQ(AD7175_IRQn);
}

/**********************************************************************************************************
**  函 数 名 : AD7124_EXINT_IrqCallback()
**  功能说明 : AD7124数据中断回调函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void AD7124_EXINT_IrqCallback(void)
{
	static uint16_t ad7124_cnt = 0;
	
	if(Set == EXINT_GetExIntSrc(AD7124_EXINT_CH))
	{
		EXINT_ClrExIntSrc(AD7124_EXINT_CH);
		
		ad7124_cnt++;
		/* Disable interrupt */
		NVIC_DisableIRQ(AD7124_IRQn);
		
		/* 防止AD7124一直触发数据中断，导致其他任务无法正常运行 */
		if(0x00 == ad7124_cnt%4)
		{
			AD7124_Process();
		}	
		
		AD7124_NSS_0();
		/* Enable interrupt */
		NVIC_EnableIRQ(AD7124_IRQn);
	}
	else
	{
		//do nothing;
	}
}

/**********************************************************************************************************
**  函 数 名 : AD7124_EXINT_Init()
**  功能说明 : AD7124数据中断初始化函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AD7124_EXINT_Init(void)
{
    stc_exint_init_t stcExintInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    stc_gpio_init_t stcGpioInit;

    /* GPIO config */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16ExInt = PIN_EXINT_ON;
    stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(AD7124_EXTI_PORT, AD7124_EXTI_PIN, &stcGpioInit);

    /* Exint config */
    (void)EXINT_StructInit(&stcExintInit);
    stcExintInit.u32ExIntCh = AD7124_EXINT_CH;
    stcExintInit.u32ExIntLvl = EXINT_TRIGGER_FALLING;
    (void)EXINT_Init(&stcExintInit);

    /* IRQ sign-in */
    stcIrqSignConfig.enIntSrc = AD7124_INT_SRC;
    stcIrqSignConfig.enIRQn   = AD7124_IRQn;
    stcIrqSignConfig.pfnCallback = &AD7124_EXINT_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);

    /* NVIC config */
    NVIC_ClearPendingIRQ(AD7124_IRQn);
    NVIC_SetPriority(AD7124_IRQn, DDL_IRQ_PRIORITY_13);
    NVIC_DisableIRQ(AD7124_IRQn);
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/



