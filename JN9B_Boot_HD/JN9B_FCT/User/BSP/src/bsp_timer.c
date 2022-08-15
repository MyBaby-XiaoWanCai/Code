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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_timer.h"
#include "bsp_protection.h"
#include "global.h"
#include "main.h"

//����
#include "bsp_led.h"
#include "bsp_gpio.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
static void TimerAUint1_IrqConfig(void);
static void TimerAUnit1_OvfIrqCallback(void);
static void TimerAUint2_IrqConfig(void);
static void TimerAUnit2_OvfIrqCallback(void);
static void TimerAUint3_IrqConfig(void);
static void TimerAUnit3_OvfIrqCallback(void);


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : TimerAUint1_Config
**	����˵�� : ��ʱ��A��Ԫ1��ʼ������
**	��    �� : ClkDiv - ʱ�ӷ�Ƶϵ����PerVal - �趨����ֵ
**	�� �� ֵ : ��
**  ˵    �� ������Ϊ40ms
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
**	�� �� �� : TimerAUint2_Config
**	����˵�� : ��ʱ��A��Ԫ2��ʼ������
**	��    �� : ClkDiv - ʱ�ӷ�Ƶϵ����PerVal - �趨����ֵ
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUint3_Config
**	����˵�� : ��ʱ��A��Ԫ3��ʼ������
**	��    �� : ClkDiv - ʱ�ӷ�Ƶϵ����PerVal - �趨����ֵ
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUint1_IrqConfig
**	����˵�� : ��ʱ��A��Ԫ1����ж�����
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUint2_IrqConfig
**	����˵�� : ��ʱ��A��Ԫ2����ж�����
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUint3_IrqConfig
**	����˵�� : ��ʱ��A��Ԫ3����ж�����
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUnit1_OvfIrqCallback
**	����˵�� : ��ʱ��A��Ԫ1����жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� ���滻ԭJN95��TIM3_IRQHandler
**********************************************************************************************************/
static void TimerAUnit1_OvfIrqCallback(void)
{

	
    if (TMRA_GetStatus(APP_TMRA1_UNIT, APP_TMRA1_FLAG) == Set)
    {
        TMRA_ClrStatus(APP_TMRA1_UNIT, APP_TMRA1_FLAG);		
        Feed_WDG();                                       //��ʱ��40msι��
    }
}

/**********************************************************************************************************
**	�� �� �� : TimerAUnit2_OvfIrqCallback
**	����˵�� : ��ʱ��A��Ԫ2����жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� ���滻ԭJN95��TIM4_IRQHandler
**********************************************************************************************************/
static void TimerAUnit2_OvfIrqCallback(void)
{
    if (TMRA_GetStatus(APP_TMRA2_UNIT, APP_TMRA2_FLAG) == Set)
    {
        TMRA_ClrStatus(APP_TMRA2_UNIT, APP_TMRA2_FLAG);
		
    }	
}

/**********************************************************************************************************
**	�� �� �� : TimerAUnit3_OvfIrqCallback
**	����˵�� : ��ʱ��A��Ԫ3����жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� ���滻ԭJN95��TIM5_IRQHandler
**********************************************************************************************************/
static void TimerAUnit3_OvfIrqCallback(void)
{

	
    if (TMRA_GetStatus(APP_TMRA3_UNIT, APP_TMRA3_FLAG) == Set)
    {
        TMRA_ClrStatus(APP_TMRA3_UNIT, APP_TMRA3_FLAG);
		

	}	
}

/**********************************************************************************************************
**	�� �� �� : AllTimerAUintStart
**	����˵�� : ������ʱ��A�������õ�Ԫ
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void AllTimerAUintStart(void)
{
    TMRA_Start(APP_TMRA1_UNIT);
	TMRA_Start(APP_TMRA2_UNIT);
	TMRA_Start(APP_TMRA3_UNIT);
}

/**********************************************************************************************************
**	�� �� �� : AllTimerAUintStop
**	����˵�� : ֹͣ��ʱ��A�������õ�Ԫ
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void AllTimerAUintStop(void)
{
    TMRA_Stop(APP_TMRA1_UNIT);
	TMRA_Stop(APP_TMRA2_UNIT);
	TMRA_Stop(APP_TMRA3_UNIT);
}

/**********************************************************************************************************
**	�� �� �� : TimerAUintStart
**	����˵�� : ������ʱ��Aָ���ĵ�Ԫ
**	��    �� : UnitNum - ָ����Ԫ
**	�� �� ֵ : ��
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
**	�� �� �� : TimerAUintStop
**	����˵�� : �رն�ʱ��Aָ���ĵ�Ԫ
**	��    �� : UnitNum - ָ����Ԫ
**	�� �� ֵ : ��
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
**	�� �� �� : bsp_GetRunTime()
**	����˵�� : ��ȡCPU����ʱ��(��λ1ms,����Ա�ʾ24.85��)
**	��    �� : ��
**	�� �� ֵ : CPU����ʱ��(1ms)
**********************************************************************************************************/
uint64_t bsp_GetRunTime(void)
{
    uint64_t runtime;

    DISABLE_INT();  		/* ���ж� */
//    runtime = Tstamp;	/* ��Tim5�ж��б���д(��Ҫ���жϽ��б���) */
    ENABLE_INT();  			/* ���ж� */

    return runtime;
}
/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
