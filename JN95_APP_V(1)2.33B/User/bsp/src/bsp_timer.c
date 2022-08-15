/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_timer.c
** Author      : Cai Heng
** Date        : 2021-06-16
** Description : ����systick��ʱ����Ϊϵͳ�δ�ʱ����ȱʡ��ʱ����Ϊ1ms���������£�
**               1) ʵ���˶�������ʱ����������ʹ�ã�����ͨ���޸�TMR_COUNT������ʱ������
**               2) ʵ���˺��뼶���ӳٺ�����΢�뼶�ӳٺ���
**               3) ʵ����ϵͳ����ʱ�亯��
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
/* ����TIM��ʱ�жϵ���ִ�еĻص�����ָ�� */
static void (*s_TIM_CallBack1)(void);
static void (*s_TIM_CallBack2)(void);
static void (*s_TIM_CallBack3)(void);
static void (*s_TIM_CallBack4)(void);

/* ����bsp_DelayMS()������ȫ�ֱ��� */
static volatile uint8_t s_ucTimeOutFlag = 0;
static volatile uint32_t s_uiDelayCount = 0;

/* ���������ʱ���Ľṹ����� */
static SOFT_TMR s_tTmr[TMR_COUNT] = {0};

/* ȫ������ʱ��(��λ1ms,����Ա�ʾ20,576,131,687,242��) */
//__IO uint64_t g_iRunTime = 0;

static __IO uint8_t g_ucEnableSystickISR = 0;

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void bsp_InitTimer(void);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void bsp_StopTimer(uint8_t _id);
uint8_t bsp_CheckTimer(uint8_t _id);
uint64_t bsp_GetRunTime(void);
uint64_t bsp_CheckRunTime(uint64_t _LastTime);
void bsp_InitHardTimer(void);
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : bsp_InitTimer()
**	����˵�� : ����systick�жϣ�����ʼ�������ʱ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitTimer(void)
{
    uint8_t i;

    /* �������е������ʱ������ */
    for (i = 0; i < TMR_COUNT; i++)
    {
        s_tTmr[i].Count = 0;
        s_tTmr[i].PreLoad = 0;
        s_tTmr[i].Flag = 0;
        s_tTmr[i].Mode = TMR_ONCE_MODE;
    }

    /* ����systic�ж�����Ϊ1ms��������systick�жϡ�
    ** SystemCoreClock�ǹ̼��ж����ϵͳ�ں�ʱ�ӣ�����STM32F429һ��Ϊ168MHz
    ** SysTick_Config()�������βα�ʾ�ں�ʱ�Ӷ��ٸ����ں󴥷�һ��Systick��ʱ�ж�
    ** -- SystemCoreClock / 500  ��ʾ��ʱƵ��Ϊ500Hz�� Ҳ���Ƕ�ʱ����Ϊ2ms
    ** -- SystemCoreClock / 1000 ��ʾ��ʱƵ��Ϊ1000Hz��Ҳ���Ƕ�ʱ����Ϊ1ms
    ** -- SystemCoreClock / 2000 ��ʾ��ʱƵ��Ϊ2000Hz��Ҳ���Ƕ�ʱ����Ϊ500us
    ** ���ڳ���Ӧ��һ�����ö�ʱ����1ms�����ڵ���CPU���ߵ͹���Ӧ�ÿ������ö�ʱ����Ϊ10ms
    */
#if USE_RTX == 0
    SysTick_Config(SystemCoreClock / 1000);
#endif

    /* ִ��systick�ж� */
    g_ucEnableSystickISR = 1;

    bsp_InitHardTimer();
}

/**********************************************************************************************************
**	�� �� �� : bsp_SoftTimerDec()
**	����˵�� : ÿ��1ms�����ж�ʱ��������1(���뱻SysTick_ISR�����Ե���)
**	��    �� : _tmr - ��ʱ������ָ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
    if (_tmr->Count > 0)
    {
        /* �����ʱ����������1�����ö�ʱ�������־ */
        if (--_tmr->Count == 0)
        {
            _tmr->Flag = 1;

            /* ������Զ�ģʽ���Զ���װ������ */
            if(_tmr->Mode == TMR_AUTO_MODE)
            {
                _tmr->Count = _tmr->PreLoad;
            }
        }
    }
}

/**********************************************************************************************************
**	�� �� �� : SysTick_ISR()
**	����˵�� : SysTick�жϷ������(ÿ��1ms����1��)
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void SysTick_ISR(void)
{
    static uint16_t s_count_10ms   = 0;
    static uint16_t s_count_25ms   = 0;
    static uint16_t s_count_100ms  = 0;
    static uint16_t s_count_500ms  = 0;
    static uint16_t s_count_1000ms = 0;
    uint8_t i;

    /* ÿ��1ms����1��(������bsp_DelayMS) */
    if (s_uiDelayCount > 0)
    {
        if (--s_uiDelayCount == 0)
        {
            s_ucTimeOutFlag = 1;
        }
    }

    /* ÿ��1ms�������ʱ���ļ��������м�1���� */
    for (i = 0; i < TMR_COUNT; i++)
    {
        bsp_SoftTimerDec(&s_tTmr[i]);
    }


    /* ÿ��1ms���ô˺��� */
    RunPer1ms();

    if (++s_count_10ms >= 10)
    {
        s_count_10ms = 0;

        /* ÿ��10ms���ô˺��� */
        RunPer10ms();
    }

    if (++s_count_25ms >= 25)
    {
        s_count_25ms = 0;

        /* ÿ��25ms���ô˺��� */
        RunPer25ms();
    }

    if (++s_count_100ms >= 100)
    {
        s_count_100ms = 0;

        /* ÿ��100ms���ô˺��� */
        RunPer100ms();
    }

    if (++s_count_500ms >= 500)
    {
        s_count_500ms = 0;

        /* ÿ��500ms���ô˺��� */
        RunPer500ms();
    }

    if (++s_count_1000ms >= 1000)
    {
        s_count_1000ms = 0;

        /* ÿ��1000ms���ô˺��� */
        RunPer1000ms();
    }
}

#if (USE_RTX == 0)
/**********************************************************************************************************
**	�� �� �� : bsp_DelayMS()
**	����˵�� : ���뼶�ӳ�(�ӳپ���Ϊ��1ms)
**	��    �� : n - �ӳ�ʱ��(ms,n>2)
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_DelayMS(uint32_t n)
{
    if (n == 0)
    {
        return;
    }
    else if (n == 1)
    {
        n = 2;
    }

    DISABLE_INT();  			/* ���ж� */
    s_uiDelayCount = n;
    s_ucTimeOutFlag = 0;
    ENABLE_INT();  				/* ���ж� */

    while (1)
    {
        bsp_Idle();				/* CPU����ִ�еĲ��� */

        /* �ȴ��ӳ�ʱ�䵽��s_ucTimeOutFlag������������Ϊvolatile��ʽ�Է�ֹ���Ż� */
        if (s_ucTimeOutFlag == 1)
        {
            break;
        }
    }
}

/**********************************************************************************************************
**	�� �� �� : bsp_DelayUS()
**	����˵�� : ΢�뼶�ӳ�(������systick��ʱ����������ܵ��ô˺���)
**	��    �� : n - �ӳ�ʱ��(us)
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_DelayUS(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt;
    uint32_t reload;

    reload = SysTick->LOAD;
    ticks = n * (SystemCoreClock / 1000000);	/* ��Ҫ�Ľ����� */
    told = SysTick->VAL;             			/* ��������ʼֵ */
    tcnt = 0;

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            /* SYSTICK��һ���ݼ��ļ����� */
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            /* ����װ�صݼ� */
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;

            /* ʱ�䳬��/����Ҫ�ӳٵ�ʱ�����˳� */
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}
#endif

/**********************************************************************************************************
**	�� �� �� : bsp_StartTimer()
**	����˵�� : ����һ����ʱ���������ö�ʱ����
**	��    �� : _id     - ��ʱ��ID(ֵ��[0,TMR_COUNT-1])
**             _period - ��ʱ����(1ms)
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_StartTimer(uint8_t _id, uint32_t _period)
{
    if (_id >= TMR_COUNT)
    {
#if (TIMER_PRINTF_CFG == ENABLE)
        /* ��ӡ�����Դ�����ļ������������� */
        BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
#endif
        /* �����쳣ʱ���������ȴ����Ź���λ */
        while(1);
    }

    DISABLE_INT();  					/* ���ж� */
    s_tTmr[_id].Count = _period;		/* ������ʵʱֵ��ֵ */
    s_tTmr[_id].PreLoad = _period;		/* �������Զ���װֵ(���Զ�ģʽ������) */
    s_tTmr[_id].Flag = 0;				/* ��������־ */
    s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* ���ι���ģʽ */
    ENABLE_INT();  						/* ���ж� */
}

/**********************************************************************************************************
**	�� �� �� : bsp_StartAutoTimer()
**	����˵�� : ����һ���Զ���ʱ���������ö�ʱ����
**	��    �� : _id     - ��ʱ��ID(ֵ��[0,TMR_COUNT-1])
**             _period - ��ʱ����(10ms)
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period)
{
    if (_id >= TMR_COUNT)
    {
#if (TIMER_PRINTF_CFG == ENABLE)
        /* ��ӡ�����Դ�����ļ������������� */
        BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
#endif
        /* �����쳣ʱ���������ȴ����Ź���λ */
        while(1);
    }

    DISABLE_INT();  					/* ���ж� */
    s_tTmr[_id].Count = _period;		/* ������ʵʱֵ��ֵ */
    s_tTmr[_id].PreLoad = _period;		/* �������Զ���װֵ(���Զ�ģʽ������) */
    s_tTmr[_id].Flag = 0;				/* ��������־ */
    s_tTmr[_id].Mode = TMR_AUTO_MODE;	/* �Զ�����ģʽ */
    ENABLE_INT();  						/* ���ж� */
}

/**********************************************************************************************************
**	�� �� �� : bsp_StopTimer()
**	����˵�� : ֹͣһ����ʱ��
**	��    �� : _id - ��ʱ��ID(ֵ��[0,TMR_COUNT-1])
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_StopTimer(uint8_t _id)
{
    if (_id >= TMR_COUNT)
    {
#if (TIMER_PRINTF_CFG == ENABLE)
        /* ��ӡ�����Դ�����ļ������������� */
        BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
#endif
        /* �����쳣ʱ���������ȴ����Ź���λ */
        while(1);
    }

    DISABLE_INT();  					/* ���ж� */
    s_tTmr[_id].Count = 0;				/* ������ʵʱֵ��ֵ */
    s_tTmr[_id].Flag = 0;				/* ��������־ */
    s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* ���ι���ģʽ */
    ENABLE_INT();  						/* ���ж� */
}

/**********************************************************************************************************
**	�� �� �� : bsp_CheckTimer()
**	����˵�� : ��ⶨʱ���Ƿ�ʱ
**	��    �� : _id     - ��ʱ��ID(ֵ��[0,TMR_COUNT-1])
**             _period - ��ʱ����(1ms)
**	�� �� ֵ : 0 - ��ʱδ��
**             1 - ��ʱ�ѵ�
**********************************************************************************************************/
uint8_t bsp_CheckTimer(uint8_t _id)
{
    if (_id >= TMR_COUNT)
    {
        return 0;
    }

    if (s_tTmr[_id].Flag == 1)
    {
        s_tTmr[_id].Flag = 0;
        return 1;
    }
    else
    {
        return 0;
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
    runtime = Tstamp;	/* ��Tim5�ж��б���д(��Ҫ���жϽ��б���) */
    ENABLE_INT();  			/* ���ж� */

    return runtime;
}

/**********************************************************************************************************
**	�� �� �� : bsp_CheckRunTime()
**	����˵�� : ���㵱ǰ����ʱ��͸���ʱ��֮��Ĳ�ֵ(�����˼�����ѭ��)
**	��    �� : _LastTime - �ϸ�ʱ��
**	�� �� ֵ : ��ǰʱ��͹�ȥʱ��Ĳ�ֵ(1ms)
**********************************************************************************************************/
uint64_t bsp_CheckRunTime(uint64_t _LastTime)
{
    uint64_t now_time;
    uint64_t time_diff;

    DISABLE_INT();  		/* ���ж� */
    now_time = Tstamp;	/* ��Tim5�ж��б���д(��Ҫ���жϽ��б���) */
    ENABLE_INT();  			/* ���ж� */

    if (now_time >= _LastTime)
    {
        time_diff = now_time - _LastTime;
    }
    else
    {
        time_diff = 0xFFFFFFFFFFFFFFFF - _LastTime + now_time;
    }

    return time_diff;
}

#if USE_RTX == 0
/**********************************************************************************************************
**	�� �� �� : SysTick_Handler()
**	����˵�� : ϵͳ��શ�ʱ���жϷ������(�����ļ��������˸ú���)
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void SysTick_Handler(void)
{
    HAL_IncTick();	/* ST HAL��ĵδ�ʱ�жϷ������ */

    if (g_ucEnableSystickISR == 0)
    {
        return;
    }

    SysTick_ISR();	/* �δ�ʱ�жϷ������ */
}
#endif


#ifdef TIM_HARD		/* ʹ��1��TIM��4�������ж���ʵ��4��Ӳ����ʱ�� */
/**********************************************************************************************************
**	�� �� �� : bsp_InitHardTimer()
**	����˵�� : ����TIMx(1 ~ 4)����us����Ӳ����ʱ��TIMx��������������ֹͣ
**             TIMx��4��ͨ���ҹ���APB1�����ϣ�APB1_Clk=SystemCoreClock/2
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitHardTimer(void)
{
    TIM_HandleTypeDef TimHandle = {0};
    uint32_t usPeriod;
    uint16_t usPrescaler;
    uint32_t uiTIMxCLK;
    TIM_TypeDef* TIMx = TIM_HARD;

    RCC_TIM_HARD_CLK_ENABLE();

    /*-----------------------------------------------------------------------
    ** system_stm32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�
    **   HCLK  = SYSCLK / 1     (AHB1Periph)
    **	 PCLK2 = HCLK   / 2     (APB2Periph)
    **	 PCLK1 = HCLK   / 4     (APB1Periph)
    **
    ** ��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
    ** ��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;
    **
    ** APB1 ��ʱ���� TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
    ** APB2 ��ʱ���� TIM1, TIM8, TIM9, TIM10, TIM11
    ----------------------------------------------------------------------- */

    uiTIMxCLK = SystemCoreClock / 2;

    usPrescaler = uiTIMxCLK / 1000000 - 1;

    if (TIMx == TIM2 || TIMx == TIM5)
    {
        usPeriod = 0xFFFFFFFF;
    }
    else
    {
        usPeriod = 0xFFFF;
    }

    TimHandle.Instance = TIMx;
    TimHandle.Init.Prescaler         = usPrescaler;
    TimHandle.Init.Period            = usPeriod;
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* ���ö�ʱ���жϣ���CC����Ƚ��ж�ʹ�� */
    {
        HAL_NVIC_SetPriority(TIM_HARD_IRQn, 0, 2);
        HAL_NVIC_EnableIRQ(TIM_HARD_IRQn);
    }

    /* ������ʱ�� */
    HAL_TIM_Base_Start(&TimHandle);
}

/**********************************************************************************************************
**	�� �� �� : bsp_StartHardTimer()
**	����˵�� : ʹ��TIM2 ~ TIM5�����ζ�ʱ��ʹ��, ��ʱʱ�䵽��ִ�лص�����������ͬʱ����4����ʱ��ͨ������������
**             ��ʱ����Ϊ��1us(��Ҫ�ķ��ڵ��ñ�������ִ��ʱ��)
**             TIM2��TIM5��32λ��ʱ����TIM3��TIM4��16λ��ʱ��
**	��    �� : _CC        - ����Ƚ�ͨ��(1 ~ 4)
**             _uiTimeOut - ��ʱʱ��(us)������16λ��ʱ����˵�����65.5ms������32λ��ʱ����˵�����4294s
**             _pCallBack - ��ʱʱ�䵽�󣬱�ִ�еĺ���
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack)
{
    uint32_t cnt_now;
    uint32_t cnt_tar;
    TIM_TypeDef* TIMx = TIM_HARD;

    /* ���貹���ӳ٣�ʵ�⾫�ȡ�1us */
    cnt_now = TIMx->CNT;
    cnt_tar = cnt_now + _uiTimeOut;			/* ���㲶��ļ�����ֵ */
    if (_CC == 1)
    {
        s_TIM_CallBack1 = (void (*)(void))_pCallBack;

        TIMx->CCR1 = cnt_tar; 			    /* ���ò���Ƚϼ�����CC1 */
        TIMx->SR = (uint16_t)~TIM_IT_CC1;   /* ���CC1�жϱ�־ */
        TIMx->DIER |= TIM_IT_CC1;			/* ʹ��CC1�ж� */
    }
    else if (_CC == 2)
    {
        s_TIM_CallBack2 = (void (*)(void))_pCallBack;

        TIMx->CCR2 = cnt_tar;				/* ���ò���Ƚϼ�����CC2 */
        TIMx->SR = (uint16_t)~TIM_IT_CC2;	/* ���CC2�жϱ�־ */
        TIMx->DIER |= TIM_IT_CC2;			/* ʹ��CC2�ж� */
    }
    else if (_CC == 3)
    {
        s_TIM_CallBack3 = (void (*)(void))_pCallBack;

        TIMx->CCR3 = cnt_tar;				/* ���ò���Ƚϼ�����CC3 */
        TIMx->SR = (uint16_t)~TIM_IT_CC3;	/* ���CC3�жϱ�־ */
        TIMx->DIER |= TIM_IT_CC3;			/* ʹ��CC3�ж� */
    }
    else if (_CC == 4)
    {
        s_TIM_CallBack4 = (void (*)(void))_pCallBack;

        TIMx->CCR4 = cnt_tar;				/* ���ò���Ƚϼ�����CC4 */
        TIMx->SR = (uint16_t)~TIM_IT_CC4;	/* ���CC4�жϱ�־ */
        TIMx->DIER |= TIM_IT_CC4;			/* ʹ��CC4�ж� */
    }
    else
    {
        return;
    }
}

/**********************************************************************************************************
**	�� �� �� : TIMx_IRQHandler()
**	����˵�� : TIM �жϷ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void TIM_HARD_IRQHandler(void)
{
    uint16_t itstatus = 0x0, itenable = 0x0;
    TIM_TypeDef* TIMx = TIM_HARD;

    itstatus = TIMx->SR & TIM_IT_CC1;
    itenable = TIMx->DIER & TIM_IT_CC1;

    if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
    {
        TIMx->SR = (uint16_t)~TIM_IT_CC1;
        TIMx->DIER &= (uint16_t)~TIM_IT_CC1;		/* ����CC1�ж� */

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack1();
    }

    itstatus = TIMx->SR & TIM_IT_CC2;
    itenable = TIMx->DIER & TIM_IT_CC2;
    if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
    {
        TIMx->SR = (uint16_t)~TIM_IT_CC2;
        TIMx->DIER &= (uint16_t)~TIM_IT_CC2;		/* ����CC2�ж� */

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack2();
    }

    itstatus = TIMx->SR & TIM_IT_CC3;
    itenable = TIMx->DIER & TIM_IT_CC3;
    if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
    {
        TIMx->SR = (uint16_t)~TIM_IT_CC3;
        TIMx->DIER &= (uint16_t)~TIM_IT_CC3;		/* ����CC2�ж� */

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack3();
    }

    itstatus = TIMx->SR & TIM_IT_CC4;
    itenable = TIMx->DIER & TIM_IT_CC4;
    if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
    {
        TIMx->SR = (uint16_t)~TIM_IT_CC4;
        TIMx->DIER &= (uint16_t)~TIM_IT_CC4;		/* ����CC4�ж� */

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack4();
    }
}

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
