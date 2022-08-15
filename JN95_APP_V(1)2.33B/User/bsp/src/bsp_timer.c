/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_timer.c
** Author      : Cai Heng
** Date        : 2021-06-16
** Description : 配置systick定时器作为系统滴答定时器，缺省定时周期为1ms，具体如下：
**               1) 实现了多个软件定时器供主程序使用，可以通过修改TMR_COUNT增减定时器个数
**               2) 实现了毫秒级别延迟函数和微秒级延迟函数
**               3) 实现了系统运行时间函数
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
/* 保存TIM定时中断到后执行的回调函数指针 */
static void (*s_TIM_CallBack1)(void);
static void (*s_TIM_CallBack2)(void);
static void (*s_TIM_CallBack3)(void);
static void (*s_TIM_CallBack4)(void);

/* 用于bsp_DelayMS()函数的全局变量 */
static volatile uint8_t s_ucTimeOutFlag = 0;
static volatile uint32_t s_uiDelayCount = 0;

/* 用于软件定时器的结构体变量 */
static SOFT_TMR s_tTmr[TMR_COUNT] = {0};

/* 全局运行时间(单位1ms,最长可以表示20,576,131,687,242天) */
//__IO uint64_t g_iRunTime = 0;

static __IO uint8_t g_ucEnableSystickISR = 0;

/**********************************************************************************************************
**                                           函数声明区
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
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : bsp_InitTimer()
**	功能说明 : 配置systick中断，并初始化软件定时器变量
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitTimer(void)
{
    uint8_t i;

    /* 清零所有的软件定时器变量 */
    for (i = 0; i < TMR_COUNT; i++)
    {
        s_tTmr[i].Count = 0;
        s_tTmr[i].PreLoad = 0;
        s_tTmr[i].Flag = 0;
        s_tTmr[i].Mode = TMR_ONCE_MODE;
    }

    /* 配置systic中断周期为1ms，并启动systick中断。
    ** SystemCoreClock是固件中定义的系统内核时钟，对于STM32F429一般为168MHz
    ** SysTick_Config()函数的形参表示内核时钟多少个周期后触发一次Systick定时中断
    ** -- SystemCoreClock / 500  表示定时频率为500Hz， 也就是定时周期为2ms
    ** -- SystemCoreClock / 1000 表示定时频率为1000Hz，也就是定时周期为1ms
    ** -- SystemCoreClock / 2000 表示定时频率为2000Hz，也就是定时周期为500us
    ** 对于常规应用一般设置定时周期1ms，对于低速CPU或者低功耗应用可以设置定时周期为10ms
    */
#if USE_RTX == 0
    SysTick_Config(SystemCoreClock / 1000);
#endif

    /* 执行systick中断 */
    g_ucEnableSystickISR = 1;

    bsp_InitHardTimer();
}

/**********************************************************************************************************
**	函 数 名 : bsp_SoftTimerDec()
**	功能说明 : 每隔1ms对所有定时器变量减1(必须被SysTick_ISR周期性调用)
**	形    参 : _tmr - 定时器变量指针
**	返 回 值 : 无
**********************************************************************************************************/
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
    if (_tmr->Count > 0)
    {
        /* 如果定时器变量减到1则设置定时器到达标志 */
        if (--_tmr->Count == 0)
        {
            _tmr->Flag = 1;

            /* 如果是自动模式则自动重装计数器 */
            if(_tmr->Mode == TMR_AUTO_MODE)
            {
                _tmr->Count = _tmr->PreLoad;
            }
        }
    }
}

/**********************************************************************************************************
**	函 数 名 : SysTick_ISR()
**	功能说明 : SysTick中断服务程序(每隔1ms进入1次)
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void SysTick_ISR(void)
{
    static uint16_t s_count_10ms   = 0;
    static uint16_t s_count_25ms   = 0;
    static uint16_t s_count_100ms  = 0;
    static uint16_t s_count_500ms  = 0;
    static uint16_t s_count_1000ms = 0;
    uint8_t i;

    /* 每隔1ms进入1次(仅用于bsp_DelayMS) */
    if (s_uiDelayCount > 0)
    {
        if (--s_uiDelayCount == 0)
        {
            s_ucTimeOutFlag = 1;
        }
    }

    /* 每隔1ms对软件定时器的计数器进行减1操作 */
    for (i = 0; i < TMR_COUNT; i++)
    {
        bsp_SoftTimerDec(&s_tTmr[i]);
    }


    /* 每隔1ms调用此函数 */
    RunPer1ms();

    if (++s_count_10ms >= 10)
    {
        s_count_10ms = 0;

        /* 每隔10ms调用此函数 */
        RunPer10ms();
    }

    if (++s_count_25ms >= 25)
    {
        s_count_25ms = 0;

        /* 每隔25ms调用此函数 */
        RunPer25ms();
    }

    if (++s_count_100ms >= 100)
    {
        s_count_100ms = 0;

        /* 每隔100ms调用此函数 */
        RunPer100ms();
    }

    if (++s_count_500ms >= 500)
    {
        s_count_500ms = 0;

        /* 每隔500ms调用此函数 */
        RunPer500ms();
    }

    if (++s_count_1000ms >= 1000)
    {
        s_count_1000ms = 0;

        /* 每隔1000ms调用此函数 */
        RunPer1000ms();
    }
}

#if (USE_RTX == 0)
/**********************************************************************************************************
**	函 数 名 : bsp_DelayMS()
**	功能说明 : 毫秒级延迟(延迟精度为±1ms)
**	形    参 : n - 延迟时间(ms,n>2)
**	返 回 值 : 无
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

    DISABLE_INT();  			/* 关中断 */
    s_uiDelayCount = n;
    s_ucTimeOutFlag = 0;
    ENABLE_INT();  				/* 开中断 */

    while (1)
    {
        bsp_Idle();				/* CPU空闲执行的操作 */

        /* 等待延迟时间到，s_ucTimeOutFlag变量必须申明为volatile形式以防止被优化 */
        if (s_ucTimeOutFlag == 1)
        {
            break;
        }
    }
}

/**********************************************************************************************************
**	函 数 名 : bsp_DelayUS()
**	功能说明 : 微秒级延迟(必须在systick定时器启动后才能调用此函数)
**	形    参 : n - 延迟时间(us)
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_DelayUS(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt;
    uint32_t reload;

    reload = SysTick->LOAD;
    ticks = n * (SystemCoreClock / 1000000);	/* 需要的节拍数 */
    told = SysTick->VAL;             			/* 计数器初始值 */
    tcnt = 0;

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            /* SYSTICK是一个递减的计数器 */
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            /* 重新装载递减 */
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;

            /* 时间超过/等于要延迟的时间则退出 */
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}
#endif

/**********************************************************************************************************
**	函 数 名 : bsp_StartTimer()
**	功能说明 : 启动一个定时器，并设置定时周期
**	形    参 : _id     - 定时器ID(值域[0,TMR_COUNT-1])
**             _period - 定时周期(1ms)
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_StartTimer(uint8_t _id, uint32_t _period)
{
    if (_id >= TMR_COUNT)
    {
#if (TIMER_PRINTF_CFG == ENABLE)
        /* 打印出错的源代码文件名及函数名称 */
        BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
#endif
        /* 参数异常时进入死机等待看门狗复位 */
        while(1);
    }

    DISABLE_INT();  					/* 关中断 */
    s_tTmr[_id].Count = _period;		/* 计数器实时值初值 */
    s_tTmr[_id].PreLoad = _period;		/* 计数器自动重装值(仅自动模式起作用) */
    s_tTmr[_id].Flag = 0;				/* 计数器标志 */
    s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 单次工作模式 */
    ENABLE_INT();  						/* 开中断 */
}

/**********************************************************************************************************
**	函 数 名 : bsp_StartAutoTimer()
**	功能说明 : 启动一个自动定时器，并设置定时周期
**	形    参 : _id     - 定时器ID(值域[0,TMR_COUNT-1])
**             _period - 定时周期(10ms)
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period)
{
    if (_id >= TMR_COUNT)
    {
#if (TIMER_PRINTF_CFG == ENABLE)
        /* 打印出错的源代码文件名及函数名称 */
        BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
#endif
        /* 参数异常时进入死机等待看门狗复位 */
        while(1);
    }

    DISABLE_INT();  					/* 关中断 */
    s_tTmr[_id].Count = _period;		/* 计数器实时值初值 */
    s_tTmr[_id].PreLoad = _period;		/* 计数器自动重装值(仅自动模式起作用) */
    s_tTmr[_id].Flag = 0;				/* 计数器标志 */
    s_tTmr[_id].Mode = TMR_AUTO_MODE;	/* 自动工作模式 */
    ENABLE_INT();  						/* 开中断 */
}

/**********************************************************************************************************
**	函 数 名 : bsp_StopTimer()
**	功能说明 : 停止一个定时器
**	形    参 : _id - 定时器ID(值域[0,TMR_COUNT-1])
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_StopTimer(uint8_t _id)
{
    if (_id >= TMR_COUNT)
    {
#if (TIMER_PRINTF_CFG == ENABLE)
        /* 打印出错的源代码文件名及函数名称 */
        BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
#endif
        /* 参数异常时进入死机等待看门狗复位 */
        while(1);
    }

    DISABLE_INT();  					/* 关中断 */
    s_tTmr[_id].Count = 0;				/* 计数器实时值初值 */
    s_tTmr[_id].Flag = 0;				/* 计数器标志 */
    s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 单次工作模式 */
    ENABLE_INT();  						/* 开中断 */
}

/**********************************************************************************************************
**	函 数 名 : bsp_CheckTimer()
**	功能说明 : 检测定时器是否超时
**	形    参 : _id     - 定时器ID(值域[0,TMR_COUNT-1])
**             _period - 定时周期(1ms)
**	返 回 值 : 0 - 定时未到
**             1 - 定时已到
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
**	函 数 名 : bsp_GetRunTime()
**	功能说明 : 获取CPU运行时间(单位1ms,最长可以表示24.85天)
**	形    参 : 无
**	返 回 值 : CPU运行时间(1ms)
**********************************************************************************************************/
uint64_t bsp_GetRunTime(void)
{
    uint64_t runtime;

    DISABLE_INT();  		/* 关中断 */
    runtime = Tstamp;	/* 在Tim5中断中被改写(需要关中断进行保护) */
    ENABLE_INT();  			/* 开中断 */

    return runtime;
}

/**********************************************************************************************************
**	函 数 名 : bsp_CheckRunTime()
**	功能说明 : 计算当前运行时间和给定时刻之间的差值(处理了计数器循环)
**	形    参 : _LastTime - 上个时刻
**	返 回 值 : 当前时间和过去时间的差值(1ms)
**********************************************************************************************************/
uint64_t bsp_CheckRunTime(uint64_t _LastTime)
{
    uint64_t now_time;
    uint64_t time_diff;

    DISABLE_INT();  		/* 关中断 */
    now_time = Tstamp;	/* 在Tim5中断中被改写(需要关中断进行保护) */
    ENABLE_INT();  			/* 开中断 */

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
**	函 数 名 : SysTick_Handler()
**	功能说明 : 系统嘀嗒定时器中断服务程序(启动文件中引用了该函数)
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void SysTick_Handler(void)
{
    HAL_IncTick();	/* ST HAL库的滴答定时中断服务程序 */

    if (g_ucEnableSystickISR == 0)
    {
        return;
    }

    SysTick_ISR();	/* 滴答定时中断服务程序 */
}
#endif


#ifdef TIM_HARD		/* 使用1个TIM的4个捕获中断来实现4个硬件定时器 */
/**********************************************************************************************************
**	函 数 名 : bsp_InitHardTimer()
**	功能说明 : 配置TIMx(1 ~ 4)用于us级别硬件定时，TIMx将自由运行永不停止
**             TIMx有4个通道且挂在APB1总线上，APB1_Clk=SystemCoreClock/2
**	形    参 : 无
**	返 回 值 : 无
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
    ** system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：
    **   HCLK  = SYSCLK / 1     (AHB1Periph)
    **	 PCLK2 = HCLK   / 2     (APB2Periph)
    **	 PCLK1 = HCLK   / 4     (APB1Periph)
    **
    ** 因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
    ** 因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;
    **
    ** APB1 定时器有 TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
    ** APB2 定时器有 TIM1, TIM8, TIM9, TIM10, TIM11
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

    /* 配置定时器中断，给CC捕获比较中断使用 */
    {
        HAL_NVIC_SetPriority(TIM_HARD_IRQn, 0, 2);
        HAL_NVIC_EnableIRQ(TIM_HARD_IRQn);
    }

    /* 启动定时器 */
    HAL_TIM_Base_Start(&TimHandle);
}

/**********************************************************************************************************
**	函 数 名 : bsp_StartHardTimer()
**	功能说明 : 使用TIM2 ~ TIM5做单次定时器使用, 定时时间到后执行回调函数，可以同时启动4个定时器通道，互不干扰
**             定时精度为±1us(主要耗费在调用本函数的执行时间)
**             TIM2和TIM5是32位定时器，TIM3和TIM4是16位定时器
**	形    参 : _CC        - 捕获比较通道(1 ~ 4)
**             _uiTimeOut - 超时时间(us)，对于16位定时器来说最大是65.5ms，对于32位定时器来说最大是4294s
**             _pCallBack - 定时时间到后，被执行的函数
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack)
{
    uint32_t cnt_now;
    uint32_t cnt_tar;
    TIM_TypeDef* TIMx = TIM_HARD;

    /* 无需补偿延迟，实测精度±1us */
    cnt_now = TIMx->CNT;
    cnt_tar = cnt_now + _uiTimeOut;			/* 计算捕获的计数器值 */
    if (_CC == 1)
    {
        s_TIM_CallBack1 = (void (*)(void))_pCallBack;

        TIMx->CCR1 = cnt_tar; 			    /* 设置捕获比较计数器CC1 */
        TIMx->SR = (uint16_t)~TIM_IT_CC1;   /* 清除CC1中断标志 */
        TIMx->DIER |= TIM_IT_CC1;			/* 使能CC1中断 */
    }
    else if (_CC == 2)
    {
        s_TIM_CallBack2 = (void (*)(void))_pCallBack;

        TIMx->CCR2 = cnt_tar;				/* 设置捕获比较计数器CC2 */
        TIMx->SR = (uint16_t)~TIM_IT_CC2;	/* 清除CC2中断标志 */
        TIMx->DIER |= TIM_IT_CC2;			/* 使能CC2中断 */
    }
    else if (_CC == 3)
    {
        s_TIM_CallBack3 = (void (*)(void))_pCallBack;

        TIMx->CCR3 = cnt_tar;				/* 设置捕获比较计数器CC3 */
        TIMx->SR = (uint16_t)~TIM_IT_CC3;	/* 清除CC3中断标志 */
        TIMx->DIER |= TIM_IT_CC3;			/* 使能CC3中断 */
    }
    else if (_CC == 4)
    {
        s_TIM_CallBack4 = (void (*)(void))_pCallBack;

        TIMx->CCR4 = cnt_tar;				/* 设置捕获比较计数器CC4 */
        TIMx->SR = (uint16_t)~TIM_IT_CC4;	/* 清除CC4中断标志 */
        TIMx->DIER |= TIM_IT_CC4;			/* 使能CC4中断 */
    }
    else
    {
        return;
    }
}

/**********************************************************************************************************
**	函 数 名 : TIMx_IRQHandler()
**	功能说明 : TIM 中断服务程序
**	形    参 : 无
**	返 回 值 : 无
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
        TIMx->DIER &= (uint16_t)~TIM_IT_CC1;		/* 禁能CC1中断 */

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_TIM_CallBack1();
    }

    itstatus = TIMx->SR & TIM_IT_CC2;
    itenable = TIMx->DIER & TIM_IT_CC2;
    if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
    {
        TIMx->SR = (uint16_t)~TIM_IT_CC2;
        TIMx->DIER &= (uint16_t)~TIM_IT_CC2;		/* 禁能CC2中断 */

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_TIM_CallBack2();
    }

    itstatus = TIMx->SR & TIM_IT_CC3;
    itenable = TIMx->DIER & TIM_IT_CC3;
    if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
    {
        TIMx->SR = (uint16_t)~TIM_IT_CC3;
        TIMx->DIER &= (uint16_t)~TIM_IT_CC3;		/* 禁能CC2中断 */

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_TIM_CallBack3();
    }

    itstatus = TIMx->SR & TIM_IT_CC4;
    itenable = TIMx->DIER & TIM_IT_CC4;
    if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
    {
        TIMx->SR = (uint16_t)~TIM_IT_CC4;
        TIMx->DIER &= (uint16_t)~TIM_IT_CC4;		/* 禁能CC4中断 */

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_TIM_CallBack4();
    }
}

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
