/**********************************************************************************************************
*
*	模块名称 :  adc3 处理函数
*	文件名称 : bsp_adc3.c
*	版    本 : V1.0
*	说    明 : adc3
*	作	 者：Robert.Li
*	日	 期:  2021.7.7

**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
extern uint32_t IP_Alarm_Flag[8];       //内部保护标志位
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc3;

uint16_t uhADCxConvertedValue12[DMA_CH_samp_SUM]= {0};
uint16_t uhADCxConvertedValue34[DMA_CH_samp_SUM]= {0};
uint16_t uhADCxConvertedValue56[DMA_CH_samp_SUM]= {0};
uint16_t uhADCxConvertedValue78[DMA_CH_samp_SUM]= {0};
// uint16_t AD_Amb=0;
// uint16_t AD_Aux=0;
// uint16_t AD_Bus1=0;
// uint16_t AD_Bus2=0;
// uint16_t ADCx_Vmod[8]={0};//分别代表Vmod1~Vmod7
// uint16_t ADCx_Temp[8]={0};//分别代表Temp1~Temp7

float AD_Amb=0;
float AD_Aux=0;
float AD_Bus1=0;
float AD_Bus2=0;
float ADCx_Vmod[8]= {0}; //分别代表Vmod1~Vmod7
float ADCx_Temp[8]= {0}; //分别代表Temp1~Temp7



/*ADC通道采样顺序配置为

AD_Amb   :IN3    CH0
AD_Aux   :IN13   CH1
AD_Bus1  :IN9    CH2
AD_Bus2  :IN14   CH3
AD_Vmod17:IN15   CH4
AD_Vmod28:IN4    CH5
AD_Temp17:IN10   CH6
AD_Temp28:IN12   CH7
*/



/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/

static void MX_GPIO_Init(void);
static	void MX_DMA_Init(void);
static void MX_ADC3_Init(void);
void DMA_adc3_init(void);



/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */


/*
*********************************************************************************************************
*	函 数 名: ADC3 Initialization Function
*	功能说明: None
*	形    参:  None
*	返 回 值: None
*********************************************************************************************************
*/
static void MX_ADC3_Init(void)
{

    /* USER CODE BEGIN ADC3_Init 0 */

    /* USER CODE END ADC3_Init 0 */

    ADC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN ADC3_Init 1 */

    /* USER CODE END ADC3_Init 1 */
    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    hadc3.Instance = ADC3;
    hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc3.Init.Resolution = ADC_RESOLUTION_12B;
    hadc3.Init.ScanConvMode = ENABLE;
    hadc3.Init.ContinuousConvMode = ENABLE;
    hadc3.Init.DiscontinuousConvMode = DISABLE;
    hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc3.Init.NbrOfConversion = 8;
    hadc3.Init.DMAContinuousRequests = ENABLE;
    hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc3) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_13;
    sConfig.Rank = 2;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_9;
    sConfig.Rank = 3;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_14;
    sConfig.Rank = 4;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_15;
    sConfig.Rank = 5;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 6;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_10;
    sConfig.Rank = 7;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_12;
    sConfig.Rank = 8;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    /* USER CODE BEGIN ADC3_Init 2 */

    /* USER CODE END ADC3_Init 2 */

}

/*
*********************************************************************************************************
*	函 数 名: Enable DMA controller clock
*	功能说明: None
*	形    参:  None
*	返 回 值: None
*********************************************************************************************************
*/

static void MX_DMA_Init(void)
{

    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/*
*********************************************************************************************************
*	函 数 名: ADC3 GPIO Initialization Function
*	功能说明: None
*	形    参:  None
*	返 回 值: None
*********************************************************************************************************
*/
static void MX_GPIO_Init(void)
{

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

}

/*
*********************************************************************************************************
*	函 数 名: ADC MSP Initialization
*	功能说明: This function configures the hardware resources used in this example
*	形    参: hadc: ADC handle pointer
*	返 回 值: None
*********************************************************************************************************
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(hadc->Instance==ADC3)
    {
        /* USER CODE BEGIN ADC3_MspInit 0 */

        /* USER CODE END ADC3_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_ADC3_CLK_ENABLE();

        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**ADC3 GPIO Configuration
        PF3     ------> ADC3_IN9	AD_Bus1
        PF4     ------> ADC3_IN14	AD_Bus2
        PF5     ------> ADC3_IN15	AD_Vmod17
        PF6     ------> ADC3_IN4	AD_Vmod28
        PC0     ------> ADC3_IN10	AD_Temp17
        PC2     ------> ADC3_IN12	AD_Temp28
        PC3     ------> ADC3_IN13	AD_Aux
        PA3     ------> ADC3_IN3	AD_Amb
        */
        GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* ADC3 DMA Init */
        /* ADC3 Init */
        hdma_adc3.Instance = DMA2_Stream0;
        hdma_adc3.Init.Channel = DMA_CHANNEL_2;
        hdma_adc3.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc3.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc3.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc3.Init.Mode = DMA_CIRCULAR;
        hdma_adc3.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_adc3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_adc3) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc3);

        /* USER CODE BEGIN ADC3_MspInit 1 */

        /* USER CODE END ADC3_MspInit 1 */
    }

}

/*
*********************************************************************************************************
*	函 数 名: ADC MSP De-Initialization
*	功能说明: This function freeze the hardware resources used in this example
*	形    参: hadc: ADC handle pointer
*	返 回 值: None
*********************************************************************************************************
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance==ADC3)
    {
        /* USER CODE BEGIN ADC3_MspDeInit 0 */

        /* USER CODE END ADC3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_ADC3_CLK_DISABLE();

        /**ADC3 GPIO Configuration
        PF3     ------> ADC3_IN9
        PF4     ------> ADC3_IN14
        PF5     ------> ADC3_IN15
        PF6     ------> ADC3_IN4
        PC0     ------> ADC3_IN10
        PC2     ------> ADC3_IN12
        PC3     ------> ADC3_IN13
        PA3     ------> ADC3_IN3
        */
        HAL_GPIO_DeInit(GPIOF, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);

        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3);

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);

        /* ADC3 DMA DeInit */
        HAL_DMA_DeInit(hadc->DMA_Handle);
        /* USER CODE BEGIN ADC3_MspDeInit 1 */

        /* USER CODE END ADC3_MspDeInit 1 */
    }

}
/*
*********************************************************************************************************
*	函 数 名: Initialize ADC3 all configured peripherals
*	功能说明: None
*	形    参:  None
*	返 回 值: None
*********************************************************************************************************
*/

void DMA_adc3_init()
{
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC3_Init();
    ADC_Switch_ConfigGPIO();
}



/*
*********************************************************************************************************
*	函 数 名: ADC_Switch_ConfigGPIO
*	功能说明:
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ADC_Switch_ConfigGPIO(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    /* 打开GPIO时钟 */
    __HAL_RCC_GPIOH_CLK_ENABLE(); /* 打开GPIOH时钟 */

    /* 配置推挽输出IO */
    GPIO_Initure.Pin = SNA_PIN; /*输出引脚 */
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* 设为推挽模式 */
    GPIO_Initure.Pull = GPIO_NOPULL;         /* 上下拉电阻不使能 */
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO口最大速度 */
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);     //GPIOH

    /* 配置推挽输出IO */
    GPIO_Initure.Pin = SNB_PIN; /*输出引脚 */
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* 设为推挽模式 */
    GPIO_Initure.Pull = GPIO_NOPULL;         /* 上下拉电阻不使能 */
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO口最大速度 */
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);     //GPIOH

    SNA_1();
    SNB_1();
    if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue78,DMA_CH_samp_SUM)!= HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
}
/*
*********************************************************************************************************
*	函 数 名: ADC_Switch_Channel
*	功能说明: 切换通道DMA读取
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
float Vmod0_Filter[VOUT_FILTER_CFG];
float Vmod1_Filter[VOUT_FILTER_CFG];
float Vmod2_Filter[VOUT_FILTER_CFG];
float Vmod3_Filter[VOUT_FILTER_CFG];
float Vmod4_Filter[VOUT_FILTER_CFG];
float Vmod5_Filter[VOUT_FILTER_CFG];
float Vmod6_Filter[VOUT_FILTER_CFG];
float Vmod7_Filter[VOUT_FILTER_CFG];

uint8_t Vmod01_Filter_Cnt = 0;
uint8_t Vmod23_Filter_Cnt = 0;
uint8_t Vmod45_Filter_Cnt = 0;
uint8_t Vmod67_Filter_Cnt = 0;

void ADC_Switch_Channel(uint8_t SW)
{
    uint8_t i;
    uint8_t j;
    float ADCxCHxValue_temp[8]= {0};

    if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
    {
		for(uint8_t j=0;j<CHANNEL_CFG;j++)
		{
			IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障
		}
        Error_Handler(__FILE__, __LINE__);
    }

    switch(SW)
    {
		case 0:
		{
			for (i = 0; i < DMA_CH_samp; i++)
			{
				for (j = 0; j < 8; j++)
				{
					ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue78[j+i*8];
				}
			}
//			ADCx_Vmod[6]=ADCxCHxValue_temp[4]/DMA_CH_samp;
//			ADCx_Vmod[7]=ADCxCHxValue_temp[5]/DMA_CH_samp;
			Vmod6_Filter[Vmod67_Filter_Cnt]=ADCxCHxValue_temp[4]/DMA_CH_samp;
			Vmod7_Filter[Vmod67_Filter_Cnt]=ADCxCHxValue_temp[5]/DMA_CH_samp;
			float tmp6 = 0;
			float tmp7 = 0;
			for(uint8_t filter_tmp = 0; filter_tmp < VOUT_FILTER_CFG; filter_tmp++)
			{
				tmp6 += Vmod6_Filter[filter_tmp];
				tmp7 += Vmod7_Filter[filter_tmp];
			}
			ADCx_Vmod[6]=tmp6/VOUT_FILTER_CFG;
			ADCx_Vmod[7]=tmp7/VOUT_FILTER_CFG;
			ADCx_Temp[6]=ADCxCHxValue_temp[6]/DMA_CH_samp;
			ADCx_Temp[7]=ADCxCHxValue_temp[7]/DMA_CH_samp;

			Vmod67_Filter_Cnt++;
			if(Vmod67_Filter_Cnt >= VOUT_FILTER_CFG)
			{
				Vmod67_Filter_Cnt = 0;
			}
			
			SNA_0();
			SNB_0();
			if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue12,DMA_CH_samp_SUM)!= HAL_OK)
			{
				for(uint8_t j=0;j<CHANNEL_CFG;j++)
				{
					IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障
				}
				Error_Handler(__FILE__, __LINE__);
			}
			break;
		}
		
		case 1:
		{
			for (i = 0; i < DMA_CH_samp; i++)
			{
				for (j = 0; j < 8; j++)
				{
					ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue12[j+i*8];
				}
			}
//			ADCx_Vmod[0]=ADCxCHxValue_temp[4]/DMA_CH_samp;
//			ADCx_Vmod[1]=ADCxCHxValue_temp[5]/DMA_CH_samp;
			Vmod0_Filter[Vmod01_Filter_Cnt]=ADCxCHxValue_temp[4]/DMA_CH_samp;
			Vmod1_Filter[Vmod01_Filter_Cnt]=ADCxCHxValue_temp[5]/DMA_CH_samp;			
			float tmp0 = 0;
			float tmp1 = 0;
			for(uint8_t filter_tmp = 0; filter_tmp < VOUT_FILTER_CFG; filter_tmp++)
			{
				tmp0 += Vmod0_Filter[filter_tmp];
				tmp1 += Vmod1_Filter[filter_tmp];
			}			
			ADCx_Vmod[0]=tmp0/VOUT_FILTER_CFG;
			ADCx_Vmod[1]=tmp1/VOUT_FILTER_CFG;
			ADCx_Temp[0]=ADCxCHxValue_temp[6]/DMA_CH_samp;
			ADCx_Temp[1]=ADCxCHxValue_temp[7]/DMA_CH_samp;

			Vmod01_Filter_Cnt++;
			if(Vmod01_Filter_Cnt >= VOUT_FILTER_CFG)
			{
				Vmod01_Filter_Cnt = 0;
			}			
			
			SNA_1();
			SNB_0();
			if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue34,DMA_CH_samp_SUM)!= HAL_OK)
			{
				for(uint8_t j=0;j<CHANNEL_CFG;j++)
				{
					IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障
				}
				Error_Handler(__FILE__, __LINE__);
			}
			break;
		}
		
		case 2:
		{
			for (i = 0; i < DMA_CH_samp; i++)
			{
				for (j = 0; j < 8; j++)
				{
					ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue34[j+i*8];
				}
			}
//			ADCx_Vmod[2]=ADCxCHxValue_temp[4]/DMA_CH_samp;
//			ADCx_Vmod[3]=ADCxCHxValue_temp[5]/DMA_CH_samp;
			Vmod2_Filter[Vmod23_Filter_Cnt]=ADCxCHxValue_temp[4]/DMA_CH_samp;
			Vmod3_Filter[Vmod23_Filter_Cnt]=ADCxCHxValue_temp[5]/DMA_CH_samp;			
			float tmp2 = 0;
			float tmp3 = 0;
			for(uint8_t filter_tmp = 0; filter_tmp < VOUT_FILTER_CFG; filter_tmp++)
			{
				tmp2 += Vmod2_Filter[filter_tmp];
				tmp3 += Vmod3_Filter[filter_tmp];
			}			
			ADCx_Vmod[2]=tmp2/VOUT_FILTER_CFG;
			ADCx_Vmod[3]=tmp3/VOUT_FILTER_CFG;
			ADCx_Temp[2]=ADCxCHxValue_temp[6]/DMA_CH_samp;
			ADCx_Temp[3]=ADCxCHxValue_temp[7]/DMA_CH_samp;

			Vmod23_Filter_Cnt++;
			if(Vmod23_Filter_Cnt >= VOUT_FILTER_CFG)
			{
				Vmod23_Filter_Cnt = 0;
			}			
			
			SNA_0();
			SNB_1();
			if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue56,DMA_CH_samp_SUM)!= HAL_OK)
			{
				for(uint8_t j=0;j<CHANNEL_CFG;j++)
				{
					IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障
				}
				Error_Handler(__FILE__, __LINE__);
			}
			break;
		}
		
		case 3:
		{
			for (i = 0; i < DMA_CH_samp; i++)
			{
				for (j = 0; j < 8; j++)
				{
					ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue56[j+i*8];
				}
			}
//			ADCx_Vmod[4]=ADCxCHxValue_temp[4]/DMA_CH_samp;
//			ADCx_Vmod[5]=ADCxCHxValue_temp[5]/DMA_CH_samp;
			Vmod4_Filter[Vmod45_Filter_Cnt]=ADCxCHxValue_temp[4]/DMA_CH_samp;
			Vmod5_Filter[Vmod45_Filter_Cnt]=ADCxCHxValue_temp[5]/DMA_CH_samp;			
			float tmp4 = 0;
			float tmp5 = 0;
			for(uint8_t filter_tmp = 0; filter_tmp < VOUT_FILTER_CFG; filter_tmp++)
			{
				tmp4 += Vmod4_Filter[filter_tmp];
				tmp5 += Vmod5_Filter[filter_tmp];
			}			
			ADCx_Vmod[4]=tmp4/VOUT_FILTER_CFG;
			ADCx_Vmod[5]=tmp5/VOUT_FILTER_CFG;
			ADCx_Temp[4]=ADCxCHxValue_temp[6]/DMA_CH_samp;
			ADCx_Temp[5]=ADCxCHxValue_temp[7]/DMA_CH_samp;

			Vmod45_Filter_Cnt++;
			if(Vmod45_Filter_Cnt >= VOUT_FILTER_CFG)
			{
				Vmod45_Filter_Cnt = 0;
			}				
			
			SNA_1();
			SNB_1();
			if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue78,DMA_CH_samp_SUM)!= HAL_OK)
			{
				for(uint8_t j=0;j<CHANNEL_CFG;j++)
				{
					IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障
				}
				Error_Handler(__FILE__, __LINE__);
			}
			break;
		}
		
		default:
			
			break;
    }

    AD_Amb  = ADCxCHxValue_temp[0]/DMA_CH_samp;
    AD_Aux  = ADCxCHxValue_temp[1]/DMA_CH_samp;
    AD_Bus1 = ADCxCHxValue_temp[2]/DMA_CH_samp;
    AD_Bus2 = ADCxCHxValue_temp[3]/DMA_CH_samp;
}
#endif


#if 1
float ad_tmp[8] = {0};

void ADC_Switch_Channel(uint8_t SW)
{
    uint8_t i;
    uint8_t j;
    float ADCxCHxValue_temp[8]= {0};

    if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
    {
        for(uint8_t j=0;j<CHANNEL_CFG;j++)
			IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障
        Error_Handler(__FILE__, __LINE__);
    }
	
	if((SW == 0) || (SW == 20) || (SW == 40) ||  (SW == 60))
	{
		ad_tmp[0] = 0;
		ad_tmp[1] = 0;
		ad_tmp[2] = 0;
		ad_tmp[3] = 0;
		ad_tmp[4] = 0;
		ad_tmp[5] = 0;		
		ad_tmp[6] = 0;
		ad_tmp[7] = 0;
	}
	
	if(SW < 19)
	{
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue78[j+i*8];
            }
        }
        ad_tmp[6]+=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ad_tmp[7]+=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[6]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[7]=ADCxCHxValue_temp[7]/DMA_CH_samp;

        SNA_1();
        SNB_1();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue78,DMA_CH_samp_SUM)!= HAL_OK)
        {
            for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障         
            Error_Handler(__FILE__, __LINE__);
        }		
	}
	else if(SW == 19)
	{
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue78[j+i*8];
            }
        }
        ad_tmp[6]+=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ad_tmp[7]+=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[6]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[7]=ADCxCHxValue_temp[7]/DMA_CH_samp;

		ADCx_Vmod[6] = ad_tmp[6]/20;
		ADCx_Vmod[7] = ad_tmp[7]/20;
		
        SNA_0();
        SNB_0();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue12,DMA_CH_samp_SUM)!= HAL_OK)
        {
            for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障         
            Error_Handler(__FILE__, __LINE__);
        }	
	}
	else if(SW < 39)
	{
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue12[j+i*8];
            }
        }
        ad_tmp[0]+=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ad_tmp[1]+=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[0]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[1]=ADCxCHxValue_temp[7]/DMA_CH_samp;

        SNA_0();
        SNB_0();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue12,DMA_CH_samp_SUM)!= HAL_OK)
        {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障          
            Error_Handler(__FILE__, __LINE__);
        }		
	}
	else if(SW == 39)
	{
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue12[j+i*8];
            }
        }
        ad_tmp[0]+=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ad_tmp[1]+=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[0]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[1]=ADCxCHxValue_temp[7]/DMA_CH_samp;

		ADCx_Vmod[0] = ad_tmp[0]/20;
		ADCx_Vmod[1] = ad_tmp[1]/20;		
		
        SNA_1();
        SNB_0();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue34,DMA_CH_samp_SUM)!= HAL_OK)
        {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障          
            Error_Handler(__FILE__, __LINE__);
        }		
	}
	else if(SW < 59)
	{
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue34[j+i*8];
            }
        }
        ad_tmp[2]+=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ad_tmp[3]+=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[2]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[3]=ADCxCHxValue_temp[7]/DMA_CH_samp;

        SNA_1();
        SNB_0();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue34,DMA_CH_samp_SUM)!= HAL_OK)
        {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障          
            Error_Handler(__FILE__, __LINE__);
        }	
	}
	else if(SW == 59)
	{
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue34[j+i*8];
            }
        }
        ad_tmp[2]+=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ad_tmp[3]+=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[2]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[3]=ADCxCHxValue_temp[7]/DMA_CH_samp;

		ADCx_Vmod[2] = ad_tmp[2]/20;
		ADCx_Vmod[3] = ad_tmp[3]/20;			
		
        SNA_0();
        SNB_1();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue56,DMA_CH_samp_SUM)!= HAL_OK)
        {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障          
            Error_Handler(__FILE__, __LINE__);
        }		
	}
	else if(SW < 79)
	{
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue56[j+i*8];
            }
        }
        ad_tmp[4]+=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ad_tmp[5]+=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[4]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[5]=ADCxCHxValue_temp[7]/DMA_CH_samp;

        SNA_0();
        SNB_1();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue56,DMA_CH_samp_SUM)!= HAL_OK)
        {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障         
            Error_Handler(__FILE__, __LINE__);
        }	
	}
	else if(SW == 79)
	{
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue56[j+i*8];
            }
        }
        ad_tmp[4]+=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ad_tmp[5]+=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[4]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[5]=ADCxCHxValue_temp[7]/DMA_CH_samp;

		ADCx_Vmod[4] = ad_tmp[4]/20;
		ADCx_Vmod[5] = ad_tmp[5]/20;		
		
        SNA_1();
        SNB_1();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue78,DMA_CH_samp_SUM)!= HAL_OK)
        {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障         
            Error_Handler(__FILE__, __LINE__);
        }		
	}
	
    AD_Amb=ADCxCHxValue_temp[0]/DMA_CH_samp;
    AD_Aux=ADCxCHxValue_temp[1]/DMA_CH_samp;
    AD_Bus1=ADCxCHxValue_temp[2]/DMA_CH_samp;
    AD_Bus2=ADCxCHxValue_temp[3]/DMA_CH_samp;
}

#else

void ADC_Switch_Channel(uint8_t SW)
{
    uint8_t i;
    uint8_t j;
    float ADCxCHxValue_temp[8]= {0};

    if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
    {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障
        Error_Handler(__FILE__, __LINE__);
    }

    switch(SW)
    {
    case 0:
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue78[j+i*8];
            }
        }
        ADCx_Vmod[6]=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ADCx_Vmod[7]=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[6]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[7]=ADCxCHxValue_temp[7]/DMA_CH_samp;
        // if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
        // {Error_Handler();}
        SNA_0();
        SNB_0();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue12,DMA_CH_samp_SUM)!= HAL_OK)
        {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障         
            Error_Handler(__FILE__, __LINE__);
        }
        break;
    case 1:
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue12[j+i*8];
            }
        }
        ADCx_Vmod[0]=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ADCx_Vmod[1]=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[0]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[1]=ADCxCHxValue_temp[7]/DMA_CH_samp;

        // if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
        // {Error_Handler();}
        SNA_1();
        SNB_0();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue34,DMA_CH_samp_SUM)!= HAL_OK)
        {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障          
            Error_Handler(__FILE__, __LINE__);
        }
        break;
    case 2:
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue34[j+i*8];
            }
        }
        ADCx_Vmod[2]=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ADCx_Vmod[3]=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[2]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[3]=ADCxCHxValue_temp[7]/DMA_CH_samp;
        // if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
        // {Error_Handler();}
        SNA_0();
        SNB_1();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue56,DMA_CH_samp_SUM)!= HAL_OK)
        {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障          
            Error_Handler(__FILE__, __LINE__);
        }
        break;
    case 3:
        for (i = 0; i < DMA_CH_samp; i++)
        {
            for (j = 0; j < 8; j++)
            {
                ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue56[j+i*8];
            }
        }
        ADCx_Vmod[4]=ADCxCHxValue_temp[4]/DMA_CH_samp;
        ADCx_Vmod[5]=ADCxCHxValue_temp[5]/DMA_CH_samp;
        ADCx_Temp[4]=ADCxCHxValue_temp[6]/DMA_CH_samp;
        ADCx_Temp[5]=ADCxCHxValue_temp[7]/DMA_CH_samp;

        // if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
        // {Error_Handler();}
        SNA_1();
        SNB_1();
        if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue78,DMA_CH_samp_SUM)!= HAL_OK)
        {
                                 for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_ADE;  //ADC故障         
            Error_Handler(__FILE__, __LINE__);
        }
        break;
    default:
        break;
    }

    AD_Amb=ADCxCHxValue_temp[0]/DMA_CH_samp;
    AD_Aux=ADCxCHxValue_temp[1]/DMA_CH_samp;
    AD_Bus1=ADCxCHxValue_temp[2]/DMA_CH_samp;
    AD_Bus2=ADCxCHxValue_temp[3]/DMA_CH_samp;

    //printf("\r\n[ADC]:AD_Amb=%f,AD_Aux=%f,AD_Bus1=%f,AD_Bus2=%f",	AD_Amb,AD_Aux,AD_Bus1,AD_Bus2);
}

#endif

/*
*********************************************************************************************************
*	函 数 名: ADC_Start_Stop_DMA
*	功能说明: 周期调用启停DMA
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static uint8_t SW=0;
void ADC_Start_Stop_DMA(void)
{
	ADC_Switch_Channel(SW);
	
	SW++;
//	if(SW == 4)
//	{
//		SW = 0;
//	}
	if(SW == 80)
	{
		SW = 0;
	}	
}



/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
