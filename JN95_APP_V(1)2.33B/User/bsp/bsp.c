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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void bsp_Init(void);
void Error_Handler(char *file, uint32_t line);
void System_Init(void);
void bsp_Idle(void);
void sys_cfg_set(void);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : bsp_Init()
**	����˵�� : ��ʼ�����е�Ӳ���豸���ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����ֻ��Ҫ����һ�Ρ�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_Init(void)
{
	bsp_InitDWT();
    bsp_FR_ADDR_InitGPIO();     /* ��ʼ��ǰ���GPIO*/ 
    bsp_HWDG_InitGPIO();        /* ��ʼ��Ӳ�����Ź�IO*/
	bsp_InitEeprom();			/* ��ʼ��EEPROM */
	bsp_InitFlash();			/* ��ʼ��FLASH��FRAM������SPI6 */
	bsp_InitLed();				/* ��ʼ��LED */
	bsp_HW_Fault_InitGPIO();	/* ��ʼ��Ӳ�����ϴ����� */
	bsp_InitCharge();			/* ��ʼ��ͨ��GPIO*/
	bsp_debug_usart_config();	/* ��ʼ�� UART 3*/
	DMA_adc3_init();			/* ��ʼ�� ADC3 DMA2 ͨ��*/
	bsp_InitTimer();			/* ��ʼ����ʱ�� */
	bsp_InitRNG();              /* ��ʼ�������������*/
	bsp_M_Loop_InitGPIO();      /* ��ʼ��LOOP��*/
//#if (PHY_KSZ8041 == FALSE)
//	bsp_InitW5500();	/* ��ʼ��W5500 */
//#endif
	bsp_InitAD7124();
	bsp_InitAD7175();
	bsp_dacx0508_init();        /*��2��DAC*/
    AD5245_Init();				/* ��ʼ��AD5245 */
	
//	AD5245_Vpro_Set(4.35);		//��ʹ�ù̶�ֵ
	
	osDelay(1000);
	
    bsp_SetTIMforInt(TIM5,1000,1,0);	/*��ʼ������ʱӲ����ʱ��TIM5*/
	bsp_SetTIMforInt(TIM4,5,1,0); /*��ʼ��Ӳ�����ϼ�ⶨʱ��TIM4������200ms*/
//    bsp_InitACDC();
}


/**********************************************************************************************************
**	�� �� �� : sys_cfg_set()
**	����˵�� : ��ʼ��ϵͳ����Ĭ�ϲ�������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void sys_cfg_set(void)
{

	

}

/**********************************************************************************************************
**	�� �� �� : Error_Handler()
**	����˵�� : ��������
**	��    �� : file - Դ�����ļ�����(�ؼ���__FILE__��ʾԴ�����ļ���)
**             line - �����к�(�ؼ���__LINE__��ʾԴ�����к�)
**	�� �� ֵ : ��
**********************************************************************************************************/
void Error_Handler(char *file, uint32_t line)
{
	
	#if(DBG_INFO_OUTPUTEN > 0)
	printf("\r\nWrong parameters value: file %s on line %d\r\n", file, line);	
	#endif
	
	/* ����һ����ѭ��������ʧ��ʱ������ڴ˴��������Ա����û���� */
	if (line == 0)
	{
		return;
	}
	
	while(1)
	{
		
	}
}

/**********************************************************************************************************
**	�� �� �� : SystemClock_Config()
**	����˵�� : ��ʼ��ϵͳʱ��
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
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};

	
	/* оƬ�ڲ���LDO��ѹ������ĵ�ѹ��Χ��ѡ�õ�PWR_REGULATOR_VOLTAGE_SCALE1 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* ʹ��HSE����ѡ��HSE��ΪPLLʱ��Դ */
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

	/* ѡ��PLL�������Ϊϵͳʱ��
	** - HCLK  = SYSCLK / 1  (AHB1Periph)
	** - PCLK2 = HCLK   / 2  (APB2Periph)
	** - PCLK1 = HCLK   / 4  (APB1Periph)
    */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	/* ����SystemCoreClock����������HAL_InitTick */
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
        Error_Handler(__FILE__, __LINE__);
	}

    /* ʹ��SYSʱ�Ӻ�IO���� */
	__HAL_RCC_SYSCFG_CLK_ENABLE() ;

	HAL_EnableCompensationCell();
}

/**********************************************************************************************************
**	�� �� �� : System_Init()
**	����˵�� : ϵͳ��ʼ��(��Ҫ����MPU��Cache��Sys_Clk����)
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void System_Init(void)
{
	/* STM32F429��HAL���ʼ������ʱϵͳʹ��F429��HSIʱ��(16MHz):
	** - ���ú���HAL_InitTick����ʼ���δ�ʱ���ж�(1ms)
	** - ����NVIC���ȼ�����Ϊ4
	**/
	HAL_Init();

	/* ����ϵͳʱ��Ϊ168MHz:
    ** - �л�ʹ��HSEʱ��
    ** - ����ȫ�ֱ���SystemCoreClock����������HAL_InitTick
    */
	SystemClock_Config();

	/* Event Recorder��
	** - �����ڴ���ִ��ʱ�����(MDK5.25�������ϰ汾��֧��)
	** - Ĭ�ϲ�����
	*/	
#if Enable_EventRecorder == 1  
	/* ��ʼ��EventRecorder������ */
	EventRecorderInitialize(EventRecordAll, 1U);
	EventRecorderStart();
#endif
}

/**********************************************************************************************************
**	�� �� �� : bsp_Idle()
**	����˵�� : ����ʱִ�еĺ�����һ����������for��whileѭ������������Ҫ����CPU_IDLE()�������ñ�������
**             ������ȱʡΪ�ղ������û��������ι��������CPU��������ģʽ�ȹ��ܡ�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_Idle(void)
{
	/* ι�� */

	/* ��CPU�������ߣ���Systick��ʱ�жϻ��ѻ��������жϻ��� */
}

/**********************************************************************************************************
**	�� �� �� : HAL_Delay()
**	����˵�� : �ض�������ӳٺ���
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
#if 0
void HAL_Delay(uint32_t Delay)
{
	bsp_DelayUS(Delay * 1000);
}
#endif

/**********************************************************************************************************
**        �� �� �� : Dis_int()
**        ����˵�� : ���жϣ�����ʱ����
**        ��    �� : ��
**        �� �� ֵ : ��
**********************************************************************************************************/
void Dis_int(void)
{
    HAL_NVIC_DisableIRQ(TIM4_IRQn); 
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);     
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);    
}

/**********************************************************************************************************
**        �� �� �� : En_int()
**        ����˵�� : ���жϣ�����ʱ����
**        ��    �� : ��
**        �� �� ֵ : ��
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
