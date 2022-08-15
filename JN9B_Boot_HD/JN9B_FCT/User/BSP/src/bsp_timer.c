/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_timer.c
** Author      : Huang Cheng
** Date        : 2021-11-11
** Description : bsp_timer source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_timer.h"
#include "bsp_protection.h"
#include "global.h"
#include "main.h"

//测试
#include "bsp_led.h"
#include "bsp_gpio.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void TimerAUint1_IrqConfig(void);
static void TimerAUnit1_OvfIrqCallback(void);
static void TimerAUint2_IrqConfig(void);
static void TimerAUnit2_OvfIrqCallback(void);
static void TimerAUint3_IrqConfig(void);
static void TimerAUnit3_OvfIrqCallback(void);


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : TimerAUint1_Config
**	功能说明 : 定时器A单元1初始化配置
**	形    参 : ClkDiv - 时钟分频系数，PerVal - 设定计数值
**	返 回 值 : 无
**  说    明 ：周期为40ms
**********************************************************************************************************/
void TimerAUint1_Config(uint32_t ClkDiv, uint32_t PerVal)
{
    stc_tmra_init_t stcInit;

    /* 1. Enable TimerA peripheral clock. */
    PWC_Fcg2PeriphClockCmd(APP_TMRA1_PERIP_CLK, Enable);

    /* 2. Set a default initialization value for stcInit. */
    (void)TMRA_StructInit(&stcInit);

    /* 3. Modifies the initialization values depends on the application. */
    stcInit.u32ClkSrc    = APP_TMRA1_CLK;
    stcInit.u32PCLKDiv   = ClkDiv;
    stcInit.u32CntDir    = APP_TMRA1_DIR;
    stcInit.u32CntMode   = APP_TMRA1_MODE;
    stcInit.u32PeriodVal = PerVal;
    (void)TMRA_Init(APP_TMRA1_UNIT, &stcInit);

    /* 4. Configures IRQ if needed. */
    TimerAUint1_IrqConfig();
}

/**********************************************************************************************************
**	函 数 名 : TimerAUint2_Config
**	功能说明 : 定时器A单元2初始化配置
**	形    参 : ClkDiv - 时钟分频系数，PerVal - 设定计数值
**	返 回 值 : 无
**********************************************************************************************************/
void TimerAUint2_Config(uint32_t ClkDiv, uint32_t PerVal)
{
    stc_tmra_init_t stcInit;

    /* 1. Enable TimerA peripheral clock. */
    PWC_Fcg2PeriphClockCmd(APP_TMRA2_PERIP_CLK, Enable);

    /* 2. Set a default initialization value for stcInit. */
    (void)TMRA_StructInit(&stcInit);

    /* 3. Modifies the initialization values depends on the application. */
    stcInit.u32ClkSrc    = APP_TMRA2_CLK;
    stcInit.u32PCLKDiv   = ClkDiv;
    stcInit.u32CntDir    = APP_TMRA2_DIR;
    stcInit.u32CntMode   = APP_TMRA2_MODE;
    stcInit.u32PeriodVal = PerVal;
    (void)TMRA_Init(APP_TMRA2_UNIT, &stcInit);

    /* 4. Configures IRQ if needed. */
    TimerAUint2_IrqConfig();
}

/**********************************************************************************************************
**	函 数 名 : TimerAUint3_Config
**	功能说明 : 定时器A单元3初始化配置
**	形    参 : ClkDiv - 时钟分频系数，PerVal - 设定计数值
**	返 回 值 : 无
**********************************************************************************************************/
void TimerAUint3_Config(uint32_t ClkDiv, uint32_t PerVal)
{
    stc_tmra_init_t stcInit;

    /* 1. Enable TimerA peripheral clock. */
    PWC_Fcg2PeriphClockCmd(APP_TMRA3_PERIP_CLK, Enable);

    /* 2. Set a default initialization value for stcInit. */
    (void)TMRA_StructInit(&stcInit);

    /* 3. Modifies the initialization values depends on the application. */
    stcInit.u32ClkSrc    = APP_TMRA3_CLK;
    stcInit.u32PCLKDiv   = ClkDiv;
    stcInit.u32CntDir    = APP_TMRA3_DIR;
    stcInit.u32CntMode   = APP_TMRA3_MODE;
    stcInit.u32PeriodVal = PerVal;
    (void)TMRA_Init(APP_TMRA3_UNIT, &stcInit);

    /* 4. Configures IRQ if needed. */
    TimerAUint3_IrqConfig();
}

/**********************************************************************************************************
**	函 数 名 : TimerAUint1_IrqConfig
**	功能说明 : 定时器A单元1溢出中断配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void TimerAUint1_IrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = APP_TMRA1_INT_SRC;
    stcCfg.enIRQn      = APP_TMRA1_IRQn;
    stcCfg.pfnCallback = &TimerAUnit1_OvfIrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);

    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, APP_TMRA1_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of TimerA. */
    TMRA_IntCmd(APP_TMRA1_UNIT, APP_TMRA1_INT_TYPE, Enable);
}

/**********************************************************************************************************
**	函 数 名 : TimerAUint2_IrqConfig
**	功能说明 : 定时器A单元2溢出中断配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void TimerAUint2_IrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = APP_TMRA2_INT_SRC;
    stcCfg.enIRQn      = APP_TMRA2_IRQn;
    stcCfg.pfnCallback = &TimerAUnit2_OvfIrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);

    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, APP_TMRA2_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of TimerA. */
    TMRA_IntCmd(APP_TMRA2_UNIT, APP_TMRA2_INT_TYPE, Enable);
}

/**********************************************************************************************************
**	函 数 名 : TimerAUint3_IrqConfig
**	功能说明 : 定时器A单元3溢出中断配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void TimerAUint3_IrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = APP_TMRA3_INT_SRC;
    stcCfg.enIRQn      = APP_TMRA3_IRQn;
    stcCfg.pfnCallback = &TimerAUnit3_OvfIrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);

    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, APP_TMRA3_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of TimerA. */
    TMRA_IntCmd(APP_TMRA3_UNIT, APP_TMRA3_INT_TYPE, Enable);
}

/**********************************************************************************************************
**	函 数 名 : TimerAUnit1_OvfIrqCallback
**	功能说明 : 定时器A单元1溢出中断回调函数配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：替换原JN95的TIM3_IRQHandler
**********************************************************************************************************/
static void TimerAUnit1_OvfIrqCallback(void)
{

	
    if (TMRA_GetStatus(APP_TMRA1_UNIT, APP_TMRA1_FLAG) == Set)
    {
        TMRA_ClrStatus(APP_TMRA1_UNIT, APP_TMRA1_FLAG);		
        Feed_WDG();                                       //定时器40ms喂狗
    }
}

/**********************************************************************************************************
**	函 数 名 : TimerAUnit2_OvfIrqCallback
**	功能说明 : 定时器A单元2溢出中断回调函数配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：替换原JN95的TIM4_IRQHandler
**********************************************************************************************************/
static void TimerAUnit2_OvfIrqCallback(void)
{
    if (TMRA_GetStatus(APP_TMRA2_UNIT, APP_TMRA2_FLAG) == Set)
    {
        TMRA_ClrStatus(APP_TMRA2_UNIT, APP_TMRA2_FLAG);
		
    }	
}

/**********************************************************************************************************
**	函 数 名 : TimerAUnit3_OvfIrqCallback
**	功能说明 : 定时器A单元3溢出中断回调函数配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：替换原JN95的TIM5_IRQHandler
**********************************************************************************************************/
static void TimerAUnit3_OvfIrqCallback(void)
{

	
    if (TMRA_GetStatus(APP_TMRA3_UNIT, APP_TMRA3_FLAG) == Set)
    {
        TMRA_ClrStatus(APP_TMRA3_UNIT, APP_TMRA3_FLAG);
		

	}	
}

/**********************************************************************************************************
**	函 数 名 : AllTimerAUintStart
**	功能说明 : 启动定时器A所有配置单元
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AllTimerAUintStart(void)
{
    TMRA_Start(APP_TMRA1_UNIT);
	TMRA_Start(APP_TMRA2_UNIT);
	TMRA_Start(APP_TMRA3_UNIT);
}

/**********************************************************************************************************
**	函 数 名 : AllTimerAUintStop
**	功能说明 : 停止定时器A所有配置单元
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AllTimerAUintStop(void)
{
    TMRA_Stop(APP_TMRA1_UNIT);
	TMRA_Stop(APP_TMRA2_UNIT);
	TMRA_Stop(APP_TMRA3_UNIT);
}

/**********************************************************************************************************
**	函 数 名 : TimerAUintStart
**	功能说明 : 启动定时器A指定的单元
**	形    参 : UnitNum - 指定单元
**	返 回 值 : 无
**********************************************************************************************************/
void TimerAUintStart(uint8_t UnitNum)
{
	switch(UnitNum)
	{
		case TMRA1_UNIT:
		{
			TMRA_Start(APP_TMRA1_UNIT);
			break;
		}
		case TMRA2_UNIT:
		{
			TMRA_Start(APP_TMRA2_UNIT);
			break;
		}
		case TMRA3_UNIT:
		{
			TMRA_Start(APP_TMRA3_UNIT);
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**	函 数 名 : TimerAUintStop
**	功能说明 : 关闭定时器A指定的单元
**	形    参 : UnitNum - 指定单元
**	返 回 值 : 无
**********************************************************************************************************/
void TimerAUintStop(uint8_t UnitNum)
{
	switch(UnitNum)
	{
		case TMRA1_UNIT:
		{
			TMRA_Stop(APP_TMRA1_UNIT);
			break;
		}
		case TMRA2_UNIT:
		{
			TMRA_Stop(APP_TMRA2_UNIT);
			break;
		}
		case TMRA3_UNIT:
		{
			TMRA_Stop(APP_TMRA3_UNIT);
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**	函 数 名 : bsp_GetRunTime()
**	功能说明 : 获取CPU运行时间(单位1ms,最长可以表示24.85天)
**	形    参 : 无
**	返 回 值 : CPU运行时间(1ms)
**********************************************************************************************************/
uint64_t bsp_GetRunTime(void)
{
    uint64_t runtime;

    DISABLE_INT();  		/* 关中断 */
//    runtime = Tstamp;	/* 在Tim5中断中被改写(需要关中断进行保护) */
    ENABLE_INT();  			/* 开中断 */

    return runtime;
}
/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
