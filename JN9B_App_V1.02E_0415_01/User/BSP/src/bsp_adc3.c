/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_adc3.c
** Author      : Huang Cheng
** Date        : 2021-11-10
** Description : ADC数据滤波的次数 = 软件缓存次数(u8Time) * ADC自带平均滤波次数(APP_ADC_AVG_CNT)
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp.h"
#include "global.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
float AD_Amb=0;          //环境温度AD
float AD_Aux=0;          //辅助电源电压AD
float AD_Bus1=0;         //BUS1电压AD
float AD_Bus2=0;         //BUS2电压AD
float ADCx_Vmod[8]= {0}; //分别代表Vmod1~Vmod7
//float ADCx_Temp[8]= {0}; //分别代表Temp1~Temp7

#ifdef TEST
static uint16_t m_au16AdcSaVal[APP_DMA_BLOCK_SIZE];
static uint8_t m_u32AdcValUpdated = 0U;

static uint16_t uhADCxConvertedValue12[DMA_CH_samp_SUM]= {0};
static uint16_t uhADCxConvertedValue34[DMA_CH_samp_SUM]= {0};
static uint16_t uhADCxConvertedValue56[DMA_CH_samp_SUM]= {0};
static uint16_t uhADCxConvertedValue78[DMA_CH_samp_SUM]= {0};
#else
uint16_t m_au16AdcSaVal[APP_DMA_BLOCK_SIZE];
uint8_t m_u32AdcValUpdated = 0U;

uint16_t uhADCxConvertedValue12[DMA_CH_samp_SUM]= {0};
uint16_t uhADCxConvertedValue34[DMA_CH_samp_SUM]= {0};
uint16_t uhADCxConvertedValue56[DMA_CH_samp_SUM]= {0};
uint16_t uhADCxConvertedValue78[DMA_CH_samp_SUM]= {0};
#endif

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void AdcClockConfig(void);
static void AdcInitConfig(void);
static void AdcChannelConfig(void);
static void AdcDmaConfig(void);
static void AdcDmaIrqConfig(void);

static void DMA_Btc0_IrqCallback(void);
//static void DMA_Tc0_IrqCallback(void);

static void AdcSetChannelPinAnalogMode(const M4_ADC_TypeDef *ADCx, uint32_t u32Channel);
static void AdcSetPinAnalogMode(const M4_ADC_TypeDef *ADCx, uint8_t u8PinNum);
static en_result_t ADC_Start_DMA(M4_ADC_TypeDef *ADCx, uint16_t *pAdcValBuff, uint16_t *pAdcVal, uint8_t u8Time, uint8_t *UpdatedFlag);
static void ADC_Switch_Channel(uint8_t SW);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : DMA_adc3_init
**	功能说明 : DMA_ADC所有参数初始化配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void DMA_adc3_init(void)
{
	AdcClockConfig();
	AdcInitConfig();
	AdcChannelConfig();
	AdcDmaConfig();
	ADC_Start_Stop(APP_ADC_UNIT, ADC_START);
}

/**********************************************************************************************************
**	函 数 名 : ADC_Switch_ConfigGPIO
**	功能说明 : ADC采集通道切换配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_Switch_ConfigGPIO(void)
{	
	stc_gpio_init_t stcSnGpioInit;

	/* 设置Sn pin的属性:推挽输出、初始化状态为RESET等 */
	(void)GPIO_StructInit(&stcSnGpioInit);
	stcSnGpioInit.u16PinState = PIN_STATE_RESET;
	stcSnGpioInit.u16PinDir = PIN_DIR_OUT;
	stcSnGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcSnGpioInit.u16PinOType = PIN_OTYPE_CMOS;

	/* 写入Sn pin的属性 */
	(void)GPIO_Init(SNA_PORT, SNA_PIN, &stcSnGpioInit);
	(void)GPIO_Init(SNB_PORT, SNB_PIN, &stcSnGpioInit);
}

/**********************************************************************************************************
**	函 数 名 : ADC_Switch_Channel
**	功能说明 : 切换通道进行ADC采集DMA传输，滤波次数为32*4 = 128次
**	形    参 : SW - 通道切换开关，0 - ch1,ch2; 1 - ch3,ch4; 2 - ch5,ch6; 3 - ch7,ch8
**	返 回 值 : 无
**********************************************************************************************************/
static void ADC_Switch_Channel(uint8_t SW)
{
	uint8_t i;
	uint8_t j;
	uint16_t Avr_cnt = DMA_CH_samp*9;
	static uint32_t ADCxCHxValue_temp[8]= {0};

	switch(SW/10)
	{
		/* 获取Vmod_1/Vmod_2和Temp_1/Temp_2的adc值 */
		case 0:
		{	

			SNA_0();     //设置切换开关
			SNB_0();
			if(Ok != ADC_Start_DMA(APP_ADC_UNIT, uhADCxConvertedValue12, m_au16AdcSaVal, DMA_CH_samp, &m_u32AdcValUpdated))
			{
				for(uint8_t j = 0;j < CHANNEL_CFG; j++)
				{
					IP_Alarm_Flag[j] |= IntelProtect_ADE;  //ADC故障   
				}      
				Error_Handler(__FILE__, __LINE__);
			}
			if(SW != 0)
			{
				for (i = 0; i < DMA_CH_samp; i++)
				{
					for (j = 0; j < 8; j++)
					{
						ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j] + uhADCxConvertedValue12[j+i*8];
					}
				}
			}
			if(SW == 9)
			{				
				ADCx_Vmod[0] = (float)ADCxCHxValue_temp[4] / Avr_cnt;
				ADCx_Vmod[1] = (float)ADCxCHxValue_temp[5] / Avr_cnt;
//				ADCx_Temp[0] = (float)ADCxCHxValue_temp[6] / Avr_cnt;
//				ADCx_Temp[1] = (float)ADCxCHxValue_temp[7] / Avr_cnt;
				ADCxCHxValue_temp[4] = 0;
				ADCxCHxValue_temp[5] = 0;
				ADCxCHxValue_temp[6] = 0;
				ADCxCHxValue_temp[7] = 0;
			}
       
			break;
		}
		/* 获取Vmod_3/Vmod_4和Temp_3/Temp_4的adc值 */
		case 1:
		{
			SNA_1();     //设置切换开关
			SNB_0();			
			if(Ok != ADC_Start_DMA(APP_ADC_UNIT, uhADCxConvertedValue34, m_au16AdcSaVal, DMA_CH_samp, &m_u32AdcValUpdated))
			{
				for(uint8_t j = 0;j < CHANNEL_CFG; j++)
				{
					IP_Alarm_Flag[j] |= IntelProtect_ADE;  //ADC故障   
				}      
				Error_Handler(__FILE__, __LINE__);
			}
			if(SW != 10)
			{
				for (i = 0; i < DMA_CH_samp; i++)
				{
					for (j = 0; j < 8; j++)
					{
						ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j] + uhADCxConvertedValue34[j+i*8];
					}
				}
			}
			if(SW == 19)
			{
				ADCx_Vmod[2] = (float)ADCxCHxValue_temp[4] / Avr_cnt;
				ADCx_Vmod[3] = (float)ADCxCHxValue_temp[5] / Avr_cnt;
//				ADCx_Temp[2] = (float)ADCxCHxValue_temp[6] / Avr_cnt;
//				ADCx_Temp[3] = (float)ADCxCHxValue_temp[7] / Avr_cnt;
				ADCxCHxValue_temp[4] = 0;
				ADCxCHxValue_temp[5] = 0;
				ADCxCHxValue_temp[6] = 0;
				ADCxCHxValue_temp[7] = 0;
			}
			
			break;
		}
        /* 获取Vmod_5/Vmod_6和Temp_5/Temp_6的adc值 */
		case 2:
		{
			SNA_0();     //设置切换开关
			SNB_1();
			
			if(Ok != ADC_Start_DMA(APP_ADC_UNIT, uhADCxConvertedValue56, m_au16AdcSaVal, DMA_CH_samp, &m_u32AdcValUpdated))
			{
				for(uint8_t j = 0;j < CHANNEL_CFG; j++)
				{
					IP_Alarm_Flag[j] |= IntelProtect_ADE;  //ADC故障   
				}      
				Error_Handler(__FILE__, __LINE__);
			}
			if(SW != 20)
			{
				for (i = 0; i < DMA_CH_samp; i++)
				{
					for (j = 0; j < 8; j++)
					{
						ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j] + uhADCxConvertedValue56[j+i*8];
					}
				}
			}
			if(SW == 29)
			{
				ADCx_Vmod[4] = (float)ADCxCHxValue_temp[4] / Avr_cnt;
				ADCx_Vmod[5] = (float)ADCxCHxValue_temp[5] / Avr_cnt;
//				ADCx_Temp[4] = (float)ADCxCHxValue_temp[6] / Avr_cnt;
//				ADCx_Temp[5] = (float)ADCxCHxValue_temp[7] / Avr_cnt;
				ADCxCHxValue_temp[4] = 0;
				ADCxCHxValue_temp[5] = 0;
				ADCxCHxValue_temp[6] = 0;
				ADCxCHxValue_temp[7] = 0;
			}
			break;
		}
    /* 获取Vmod_7/Vmod_8和Temp_7/Temp_8的adc值 */
		case 3:
		{
			SNA_1();     //设置切换开关
			SNB_1();		
			if(Ok != ADC_Start_DMA(APP_ADC_UNIT, uhADCxConvertedValue78, m_au16AdcSaVal, DMA_CH_samp, &m_u32AdcValUpdated))
			{
				for(uint8_t j = 0;j < CHANNEL_CFG; j++)
				{
					IP_Alarm_Flag[j] |= IntelProtect_ADE;  //ADC故障   
				}      
				Error_Handler(__FILE__, __LINE__);
			}
			if(SW != 30)
			{
				for (i = 0; i < DMA_CH_samp; i++)
				{
					for (j = 0; j < 8; j++)
					{
						ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j] + uhADCxConvertedValue78[j+i*8];
					}
				}
			}
			if(SW == 39)
			{
				ADCx_Vmod[6] = (float)ADCxCHxValue_temp[4] / Avr_cnt;
				ADCx_Vmod[7] = (float)ADCxCHxValue_temp[5] / Avr_cnt;
//				ADCx_Temp[6] = (float)ADCxCHxValue_temp[6] / Avr_cnt;
//				ADCx_Temp[7] = (float)ADCxCHxValue_temp[7] / Avr_cnt;
				ADCxCHxValue_temp[4] = 0;
				ADCxCHxValue_temp[5] = 0;
				ADCxCHxValue_temp[6] = 0;
				ADCxCHxValue_temp[7] = 0;
			}
			
			break;
		}
       
		default:
		{
			break;
		}
	}

	/* ADC_Start_DMA中已获取过ADCxCHxValue_temp[0:3]的值 */
	AD_Amb = (float)ADCxCHxValue_temp[0] / DMA_CH_samp;
	AD_Aux = (float)ADCxCHxValue_temp[1] / DMA_CH_samp;
	AD_Bus1 = (float)ADCxCHxValue_temp[2] / DMA_CH_samp;
	AD_Bus2 = (float)ADCxCHxValue_temp[3] / DMA_CH_samp;
	ADCxCHxValue_temp[0] = 0;
	ADCxCHxValue_temp[1] = 0;
	ADCxCHxValue_temp[2] = 0;
	ADCxCHxValue_temp[3] = 0;
}

/**********************************************************************************************************
**	函 数 名 : ADC_Start()
**	功能说明 : ADC启停函数
**	形    参 : *ADCx - M4_ADC1/M4_ADC2/M4_ADC3, State - STOP/START
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_Start_Stop(M4_ADC_TypeDef *ADCx, uint8_t State)
{
	if(ADC_START == State)
	{
		WRITE_REG8(ADCx->STR, ADC_STR_STRT);
	}
	else if(ADC_STOP == State)
	{
		WRITE_REG8(ADCx->STR, 0U);
	}
	else
	{
		//do nothing;
	}
}

/**********************************************************************************************************
**	函 数 名 : ADC_Start_Stop_DMA
**	功能说明 : 周期调用启停DMA
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/

void ADC_Start_Stop_DMA(void)
{
	static uint8_t SW=0;
	
	//通道切换开关，0 - ch1,ch2; 1 - ch3,ch4; 2 - ch5,ch6; 3 - ch7,ch8
  ADC_Switch_Channel(SW);
	
	SW++;
    if(40 == SW)
	{
		SW = 0;
	}
}

/**********************************************************************************************************
**	函 数 名 : ADC_Start_DMA
**	功能说明 : 此函数类似于HAL_ADC_Start_DMA，可实现连续多次DMA传输，华大库函数不带此API函数故为自行编写
**	形    参 : *ADCx - M4_ADC1/M4_ADC2/M4_ADC3
**             *pAdcValBuff - ADC数据缓冲区
**             *pAdcVal - DMA传输的数据缓存
**             u8Time - ADC数据缓存次数
**             *UpdatedFlag - DMA传输中断标志位
**	返 回 值 : enRet - @ref en_result_t
**********************************************************************************************************/
static en_result_t ADC_Start_DMA(M4_ADC_TypeDef *ADCx, uint16_t *pAdcValBuff, uint16_t *pAdcVal, uint8_t u8Time, uint8_t *UpdatedFlag)
{
	uint8_t Index = 0U;
	uint32_t u32Timeout = 0U;
	en_result_t enRet = ErrorInvalidParameter;
	
	u32Timeout = u8Time * 400U;
	if ((pAdcValBuff == NULL) || (IS_ADDRESS_ALIGN_HALFWORD(&pAdcValBuff[0U]) == 0) || 
		(pAdcVal == NULL)     || (IS_ADDRESS_ALIGN_HALFWORD(&pAdcVal[0U]) == 0)     || 
		(u8Time == 0U)        || (u32Timeout == 0U))
    {
		return (enRet=Error);
    }
	
	/* if ADC stop, Start ADC.*/
	if(0x00 == READ_REG8(ADCx->STR))
	{
		WRITE_REG8(ADCx->STR, ADC_STR_STRT);
	}
    
	while(u8Time && u32Timeout)
	{
		if(*UpdatedFlag == 1U)
		{
			*UpdatedFlag = 0U;
			memcpy(&pAdcValBuff[8*Index], pAdcVal, 2*8);
			Index++;
			u8Time--;
		}
		else
		{
			//do nothing;
		}
		
		u32Timeout--;
	}
	
	if((u8Time == 0U) && (u32Timeout != 0U))
	{
		enRet = Ok;
	}
	else
	{
		enRet = ErrorTimeout;
		
		/* Stop ADC if timeout. */
		WRITE_REG8(ADCx->STR, 0U);
	}
	
    return enRet;
}

/**********************************************************************************************************
**	函 数 名 : AdcClockConfig
**	功能说明 : ADC时钟配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void AdcClockConfig(void)
{
    /*
     * 1. Configures the clock divider of PCLK2 and PCLK4 here or in the function of configuring the system clock.
     *    In this example, the default system clock is MRC(8MHz).
     *    PCLK4 is the digital interface clock, and PCLK2 is the analog circuit clock.
     *    PCLK4 : PCLK2 = 1:1, 2:1, 4:1, 8:1, 1:2, 1:4.
     *    PCLK2 is in range [1MHz, 60MHz].
     */
    CLK_ClkDiv((CLK_CATE_PCLK2 | CLK_CATE_PCLK4), (CLK_PCLK2_DIV4 | CLK_PCLK4_DIV2));

    /* 2. Specify the clock source of ADC. Only 'CLK_PERI_CLK_PCLK' can be used in this example. */
    CLK_PERI_ClkConfig(CLK_PERI_CLK_PCLK);
}

/**********************************************************************************************************
**	函 数 名 : AdcInitConfig
**	功能说明 : ADC初始化配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void AdcInitConfig(void)
{
    stc_adc_init_t stcInit;

    /* Set a default value. */
    (void)ADC_StructInit(&stcInit);
	
	/* 1. Modify the default value depends on the application. */
	stcInit.u16AutoClrCmd = ADC_AUTO_CLR_ENABLE;
	stcInit.u16DataAlign = ADC_DATA_ALIGN_RIGHT;
	stcInit.u16Resolution = ADC_RESOLUTION_12BIT;
	stcInit.u16ScanMode = ADC_MODE_SA_CONT;

    /* 2. Enable ADC peripheral clock. */
    PWC_Fcg3PeriphClockCmd(APP_ADC_PERIP_CLK, Enable);

    /* 3. Initializes ADC. */
    (void)ADC_Init(APP_ADC_UNIT, &stcInit);
}

/**********************************************************************************************************
**	函 数 名 : AdcChannelConfig
**	功能说明 : ADC通道配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void AdcChannelConfig(void)
{
    uint8_t au8AdcSASplTime[] = APP_ADC_SA_SAMPLE_TIME;
	
	/* 1. Remap the correspondence between channels and analog input pins. */
//    ADC_ChannelRemap(APP_ADC_UNIT, APP_ADC_REMAP_PIN1_DEST_CH, APP_ADC_REMAP_PIN1);
//    ADC_ChannelRemap(APP_ADC_UNIT, APP_ADC_REMAP_PIN2_DEST_CH, APP_ADC_REMAP_PIN2);
		ADC_ChannelRemap(APP_ADC_UNIT, APP_ADC_REMAP_PIN3_DEST_CH, APP_ADC_REMAP_PIN3);
    ADC_ChannelRemap(APP_ADC_UNIT, APP_ADC_REMAP_PIN4_DEST_CH, APP_ADC_REMAP_PIN4);
		ADC_ChannelRemap(APP_ADC_UNIT, APP_ADC_REMAP_PIN5_DEST_CH, APP_ADC_REMAP_PIN5);
    ADC_ChannelRemap(APP_ADC_UNIT, APP_ADC_REMAP_PIN6_DEST_CH, APP_ADC_REMAP_PIN6);
//		ADC_ChannelRemap(APP_ADC_UNIT, APP_ADC_REMAP_PIN7_DEST_CH, APP_ADC_REMAP_PIN7);
//    ADC_ChannelRemap(APP_ADC_UNIT, APP_ADC_REMAP_PIN8_DEST_CH, APP_ADC_REMAP_PIN8);

    /* 1. Set the ADC pin to analog input mode. */
    AdcSetChannelPinAnalogMode(APP_ADC_UNIT, APP_ADC_SA_CH);

    /* 2. Enable the ADC channels. */
    (void)ADC_ChannelCmd(APP_ADC_UNIT, ADC_SEQ_A, \
                         APP_ADC_SA_CH, au8AdcSASplTime, \
                         Enable);

    /* 3. Set the number of averaging sampling and enable the channel, if needed. */
#if (defined APP_ADC_AVG_CH) && (APP_ADC_AVG_CH != 0U)
    ADC_AvgChannelConfig(APP_ADC_UNIT, APP_ADC_AVG_CNT);
    ADC_AvgChannelCmd(APP_ADC_UNIT, APP_ADC_AVG_CH, Enable);
#endif
}

/**********************************************************************************************************
**	函 数 名 : AdcDmaConfig
**	功能说明 : ADC触发DMA配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void AdcDmaConfig(void)
{
    stc_dma_init_t stcDmaInit;
    stc_dma_rpt_init_t stcDmaRptInit;

    /* Enable DMA peripheral clock and AOS function. */
    PWC_Fcg0PeriphClockCmd((APP_DMA_PERIP_CLK | PWC_FCG0_AOS), Enable);

    DMA_SetTriggerSrc(APP_DMA_UNIT, APP_DMA_CH, APP_DMA_TRIG_SRC);

    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn     = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = APP_DMA_BLOCK_SIZE;
    stcDmaInit.u32TransCnt  = APP_DMA_TRANS_COUNT;
    stcDmaInit.u32DataWidth = APP_DMA_DATA_WIDTH;
    stcDmaInit.u32DestAddr  = (uint32_t)(&m_au16AdcSaVal[0U]);
    stcDmaInit.u32SrcAddr   = (uint32_t)APP_DMA_SRC_ADDR;
    stcDmaInit.u32SrcInc    = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestInc   = DMA_DEST_ADDR_INC;
    (void)DMA_Init(APP_DMA_UNIT, APP_DMA_CH, &stcDmaInit);

    (void)DMA_RepeatStructInit(&stcDmaRptInit);
    stcDmaRptInit.u32SrcRptEn    = DMA_SRC_RPT_ENABLE;
    stcDmaRptInit.u32SrcRptSize  = APP_DMA_BLOCK_SIZE;
    stcDmaRptInit.u32DestRptEn   = DMA_DEST_RPT_ENABLE;
    stcDmaRptInit.u32DestRptSize = APP_DMA_BLOCK_SIZE;
    (void)DMA_RepeatInit(APP_DMA_UNIT, APP_DMA_CH, &stcDmaRptInit);

    AdcDmaIrqConfig();

    DMA_Cmd(APP_DMA_UNIT, Enable);
    DMA_ChannelCmd(APP_DMA_UNIT, APP_DMA_CH, Enable);
}

/**********************************************************************************************************
**	函 数 名 : AdcDmaIrqConfig
**	功能说明 : ADC_DMA中断函数配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void AdcDmaIrqConfig(void)
{
    stc_irq_signin_config_t stcIrqSignConfig;

    stcIrqSignConfig.enIntSrc    = APP_DMA_IRQ_SRC;
    stcIrqSignConfig.enIRQn      = APP_DMA_IRQn;
    stcIrqSignConfig.pfnCallback = &DMA_Btc0_IrqCallback;

    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    DMA_ClearTransIntStatus(APP_DMA_UNIT, APP_DMA_TRANS_CPLT_FLAG);

    /* NVIC setting */
    NVIC_ClearPendingIRQ(APP_DMA_IRQn);
    NVIC_SetPriority(APP_DMA_IRQn, APP_DMA_INT_PRIO);
    NVIC_EnableIRQ(APP_DMA_IRQn);
}


/**********************************************************************************************************
**	函 数 名 : DMA_Btc0_IrqCallback
**	功能说明 : DMA传输块中断回调函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void DMA_Btc0_IrqCallback(void)
{
    DMA_ClearTransIntStatus(APP_DMA_UNIT, APP_DMA_TRANS_CPLT_FLAG);
    m_u32AdcValUpdated |= ADC_SEQ_FLAG_EOCA;
	
	/* Clear EOC flag. */
    SET_REG8_BIT(APP_ADC_UNIT->ISCLRR, ADC_ISCLRR_CLREOCAF);
}

/**********************************************************************************************************
**	函 数 名 : AdcSetChannelPinAnalogMode
**	功能说明 : 设置GPIO_PIN 映射到对应的ADC通道
**	形    参 : *ADCx - M4_ADC1/M4_ADC2/M4_ADC3， u32Channel - ADC_CH0...ADC_CH15
**	返 回 值 : 无
**********************************************************************************************************/
static void AdcSetChannelPinAnalogMode(const M4_ADC_TypeDef *ADCx, uint32_t u32Channel)
{
    uint8_t u8PinNum;
#if (defined APP_ADC_REMAP_CH)
    uint8_t u8RemapPinNum;
#endif

    u8PinNum = 0U;
    while (u32Channel != 0U)
    {
        if ((u32Channel & 0x1UL) != 0UL)
        {
#if (defined APP_ADC_REMAP_CH)
            u8RemapPinNum = ADC_GetChannelPinNum(ADCx, (0x1UL << u8PinNum));
            AdcSetPinAnalogMode(ADCx, u8RemapPinNum);
#else
            AdcSetPinAnalogMode(ADCx, u8PinNum);
#endif
        }

        u32Channel >>= 1U;
        u8PinNum++;
    }
}

/**********************************************************************************************************
**	函 数 名 : AdcSetPinAnalogMode
**	功能说明 : 设置GPIO_PIN为模拟输入模式
**	形    参 : *ADCx - M4_ADC1/M4_ADC2/M4_ADC3， u8PinNum - @ref ADC_Pin_Number
**	返 回 值 : 无
**********************************************************************************************************/
static void AdcSetPinAnalogMode(const M4_ADC_TypeDef *ADCx, uint8_t u8PinNum)
{
    typedef struct
    {
        uint8_t  u8Port;
        uint16_t u16Pin;
    } stc_adc_pin_t;

    stc_gpio_init_t stcGpioInit;

    stc_adc_pin_t astcADC12[ADC1_CH_COUNT] = { \
            {GPIO_PORT_A, GPIO_PIN_00}, {GPIO_PORT_A, GPIO_PIN_01}, \
            {GPIO_PORT_A, GPIO_PIN_02}, {GPIO_PORT_A, GPIO_PIN_03}, \
            {GPIO_PORT_A, GPIO_PIN_04}, {GPIO_PORT_A, GPIO_PIN_05}, \
            {GPIO_PORT_A, GPIO_PIN_06}, {GPIO_PORT_A, GPIO_PIN_07}, \
            {GPIO_PORT_B, GPIO_PIN_00}, {GPIO_PORT_B, GPIO_PIN_01}, \
            {GPIO_PORT_C, GPIO_PIN_00}, {GPIO_PORT_C, GPIO_PIN_01}, \
            {GPIO_PORT_C, GPIO_PIN_02}, {GPIO_PORT_C, GPIO_PIN_03}, \
            {GPIO_PORT_C, GPIO_PIN_04}, {GPIO_PORT_C, GPIO_PIN_05}, \
    };
    stc_adc_pin_t astcADC3[ADC3_CH_COUNT] = { \
            {GPIO_PORT_A, GPIO_PIN_00}, {GPIO_PORT_A, GPIO_PIN_01}, \
            {GPIO_PORT_A, GPIO_PIN_02}, {GPIO_PORT_A, GPIO_PIN_03}, \
            {GPIO_PORT_F, GPIO_PIN_06}, {GPIO_PORT_F, GPIO_PIN_07}, \
            {GPIO_PORT_F, GPIO_PIN_08}, {GPIO_PORT_F, GPIO_PIN_09}, \
            {GPIO_PORT_F, GPIO_PIN_10}, {GPIO_PORT_F, GPIO_PIN_03}, \
            {GPIO_PORT_C, GPIO_PIN_00}, {GPIO_PORT_C, GPIO_PIN_01}, \
            {GPIO_PORT_C, GPIO_PIN_02}, {GPIO_PORT_C, GPIO_PIN_03}, \
            {GPIO_PORT_F, GPIO_PIN_04}, {GPIO_PORT_F, GPIO_PIN_05}, \
            {GPIO_PORT_H, GPIO_PIN_02}, {GPIO_PORT_H, GPIO_PIN_03}, \
            {GPIO_PORT_H, GPIO_PIN_04}, {GPIO_PORT_H, GPIO_PIN_05}, \
    };

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;

    if ((ADCx == M4_ADC1) || (ADCx == M4_ADC2))
    {
        (void)GPIO_Init(astcADC12[u8PinNum].u8Port, astcADC12[u8PinNum].u16Pin, &stcGpioInit);
    }
    else
    {
        (void)GPIO_Init(astcADC3[u8PinNum].u8Port, astcADC3[u8PinNum].u16Pin, &stcGpioInit);
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

