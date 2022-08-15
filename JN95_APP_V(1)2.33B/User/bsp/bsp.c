/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp.c
** Author      : Cai Heng
** Date        : 2021-06-16
** Description : This file is used to initialize and drive the hardware 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void bsp_Init(void);
void Error_Handler(char *file, uint32_t line);
void System_Init(void);
void bsp_Idle(void);
void sys_cfg_set(void);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : bsp_Init()
**	功能说明 : 初始化所有的硬件设备。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。只需要调用一次。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_Init(void)
{
	bsp_InitDWT();
    bsp_FR_ADDR_InitGPIO();     /* 初始化前面板GPIO*/ 
    bsp_HWDG_InitGPIO();        /* 初始化硬件看门狗IO*/
	bsp_InitEeprom();			/* 初始化EEPROM */
	bsp_InitFlash();			/* 初始化FLASH，FRAM，共用SPI6 */
	bsp_InitLed();				/* 初始化LED */
	bsp_HW_Fault_InitGPIO();	/* 初始化硬件故障触发脚 */
	bsp_InitCharge();			/* 初始化通用GPIO*/
	bsp_debug_usart_config();	/* 初始化 UART 3*/
	DMA_adc3_init();			/* 初始化 ADC3 DMA2 通道*/
	bsp_InitTimer();			/* 初始化软定时器 */
	bsp_InitRNG();              /* 初始化随机数生成器*/
	bsp_M_Loop_InitGPIO();      /* 初始化LOOP脚*/
//#if (PHY_KSZ8041 == FALSE)
//	bsp_InitW5500();	/* 初始化W5500 */
//#endif
	bsp_InitAD7124();
	bsp_InitAD7175();
	bsp_dacx0508_init();        /*有2块DAC*/
    AD5245_Init();				/* 初始化AD5245 */
	
//	AD5245_Vpro_Set(4.35);		//不使用固定值
	
	osDelay(1000);
	
    bsp_SetTIMforInt(TIM5,1000,1,0);	/*初始化运行时硬件定时器TIM5*/
	bsp_SetTIMforInt(TIM4,5,1,0); /*初始化硬件故障检测定时器TIM4，周期200ms*/
//    bsp_InitACDC();
}


/**********************************************************************************************************
**	函 数 名 : sys_cfg_set()
**	功能说明 : 初始化系统各种默认参数配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void sys_cfg_set(void)
{

	

}

/**********************************************************************************************************
**	函 数 名 : Error_Handler()
**	功能说明 : 错误处理函数
**	形    参 : file - 源代码文件名称(关键字__FILE__表示源代码文件名)
**             line - 代码行号(关键字__LINE__表示源代码行号)
**	返 回 值 : 无
**********************************************************************************************************/
void Error_Handler(char *file, uint32_t line)
{
	
	#if(DBG_INFO_OUTPUTEN > 0)
	printf("\r\nWrong parameters value: file %s on line %d\r\n", file, line);	
	#endif
	
	/* 这是一个死循环，断言失败时程序会在此处死机，以便于用户查错 */
	if (line == 0)
	{
		return;
	}
	
	while(1)
	{
		
	}
}

/**********************************************************************************************************
**	函 数 名 : SystemClock_Config()
**	功能说明 : 初始化系统时钟
**                 System Clock source            = PLL (HSE)
**                 SYSCLK(Hz)                     = 168000000 (CPU Clock)
**                 HCLK = SYSCLK / 1              = 168000000 (AHB1Periph)
**                 PCLK2 = HCLK / 2               = 84000000  (APB2Periph)
**                 PCLK1 = HCLK / 4               = 42000000  (APB1Periph)
**                 HSE Frequency(Hz)              = 25000000
**                 PLL_M                          = 8
**                 PLL_N                          = 336
**                 PLL_P                          = 2
**                 PLL_Q                          = 4
**                 VDD(V)                         = 3.3
**                 Flash Latency(WS)              = 5
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};

	
	/* 芯片内部的LDO稳压器输出的电压范围，选用的PWR_REGULATOR_VOLTAGE_SCALE1 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* 使能HSE，并选择HSE作为PLL时钟源 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;			//
	RCC_OscInitStruct.PLL.PLLN = 180;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
        Error_Handler(__FILE__, __LINE__);
	}

	/* 选择PLL的输出作为系统时钟
	** - HCLK  = SYSCLK / 1  (AHB1Periph)
	** - PCLK2 = HCLK   / 2  (APB2Periph)
	** - PCLK1 = HCLK   / 4  (APB1Periph)
    */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	/* 更新SystemCoreClock并重新配置HAL_InitTick */
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
        Error_Handler(__FILE__, __LINE__);
	}

    /* 使能SYS时钟和IO补偿 */
	__HAL_RCC_SYSCFG_CLK_ENABLE() ;

	HAL_EnableCompensationCell();
}

/**********************************************************************************************************
**	函 数 名 : System_Init()
**	功能说明 : 系统初始化(主要包括MPU、Cache和Sys_Clk配置)
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void System_Init(void)
{
	/* STM32F429的HAL库初始化，此时系统使用F429的HSI时钟(16MHz):
	** - 调用函数HAL_InitTick，初始化滴答时钟中断(1ms)
	** - 设置NVIC优先级分组为4
	**/
	HAL_Init();

	/* 配置系统时钟为168MHz:
    ** - 切换使用HSE时钟
    ** - 更新全局变量SystemCoreClock并重新配置HAL_InitTick
    */
	SystemClock_Config();

	/* Event Recorder：
	** - 可用于代码执行时间测量(MDK5.25及其以上版本才支持)
	** - 默认不开启
	*/	
#if Enable_EventRecorder == 1  
	/* 初始化EventRecorder并开启 */
	EventRecorderInitialize(EventRecordAll, 1U);
	EventRecorderStart();
#endif
}

/**********************************************************************************************************
**	函 数 名 : bsp_Idle()
**	功能说明 : 空闲时执行的函数，一般主程序在for和while循环程序体中需要插入CPU_IDLE()宏来调用本函数。
**             本函数缺省为空操作，用户可以添加喂狗、设置CPU进入休眠模式等功能。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_Idle(void)
{
	/* 喂狗 */

	/* 让CPU进入休眠，由Systick定时中断唤醒或者其他中断唤醒 */
}

/**********************************************************************************************************
**	函 数 名 : HAL_Delay()
**	功能说明 : 重定向毫秒延迟函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
#if 0
void HAL_Delay(uint32_t Delay)
{
	bsp_DelayUS(Delay * 1000);
}
#endif

/**********************************************************************************************************
**        函 数 名 : Dis_int()
**        功能说明 : 关中断，运行时除外
**        形    参 : 无
**        返 回 值 : 无
**********************************************************************************************************/
void Dis_int(void)
{
    HAL_NVIC_DisableIRQ(TIM4_IRQn); 
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);     
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);    
}

/**********************************************************************************************************
**        函 数 名 : En_int()
**        功能说明 : 开中断，运行时除外
**        形    参 : 无
**        返 回 值 : 无
**********************************************************************************************************/
void En_int(void)
{    
    HAL_NVIC_EnableIRQ(TIM4_IRQn); 
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);     
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);   
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
