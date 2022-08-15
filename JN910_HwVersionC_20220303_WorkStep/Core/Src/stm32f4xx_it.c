/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "iwdg.h"
#include "gpio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
s_DebugParamStruct *Ads131_Data = &debugParam;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern DMA_HandleTypeDef hdma_spi3_tx;
extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim11;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim14;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */
//	HAL_GPIO_EXTI_IRQHandler(I2C2_SDA_Pin);
	HAL_GPIO_EXTI_IRQHandler(V_STM32_ADC_DRDY1_1_Pin);
  /* USER CODE END EXTI0_IRQn 0 */
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles EXTI line1 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */
	//HAL_GPIO_EXTI_IRQHandler(I2C2_SCL_Pin);
	HAL_GPIO_EXTI_IRQHandler(I_STM32_ADC_DRDY1_2_Pin);
  /* USER CODE END EXTI1_IRQn 0 */
  /* USER CODE BEGIN EXTI1_IRQn 1 */

  /* USER CODE END EXTI1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line2 interrupt.
  */
void EXTI2_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_IRQn 0 */
//	HAL_GPIO_EXTI_IRQHandler(STM32_DAC_CS3_Pin);
	HAL_GPIO_EXTI_IRQHandler(V_STM32_ADC_DRDY2_1_Pin);
  /* USER CODE END EXTI2_IRQn 0 */
  /* USER CODE BEGIN EXTI2_IRQn 1 */

  /* USER CODE END EXTI2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line3 interrupt.
  */
void EXTI3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI3_IRQn 0 */
//	HAL_GPIO_EXTI_IRQHandler(STM32_DAC_CS4_Pin);
	HAL_GPIO_EXTI_IRQHandler(I_STM32_ADC_DRDY2_2_Pin);
  /* USER CODE END EXTI3_IRQn 0 */
  /* USER CODE BEGIN EXTI3_IRQn 1 */

  /* USER CODE END EXTI3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_IRQn 0 */
	//HAL_GPIO_EXTI_IRQHandler(STM32_DAC_CS5_Pin);
	HAL_GPIO_EXTI_IRQHandler(V_STM32_ADC_DRDY3_1_Pin);
  /* USER CODE END EXTI4_IRQn 0 */
  /* USER CODE BEGIN EXTI4_IRQn 1 */

  /* USER CODE END EXTI4_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream5 global interrupt.
  */
void DMA1_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream5_IRQn 0 */

  /* USER CODE END DMA1_Stream5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi3_tx);
  /* USER CODE BEGIN DMA1_Stream5_IRQn 1 */

  /* USER CODE END DMA1_Stream5_IRQn 1 */
}

/**
  * @brief This function handles ADC1, ADC2 and ADC3 global interrupts.
  */
void ADC_IRQHandler(void)
{
  /* USER CODE BEGIN ADC_IRQn 0 */

  /* USER CODE END ADC_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC_IRQn 1 */
	
  /* USER CODE END ADC_IRQn 1 */
}

/**
  * @brief This function handles CAN1 RX0 interrupts.
  */
void CAN1_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN CAN1_RX0_IRQn 0 */

  /* USER CODE END CAN1_RX0_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan1);
  /* USER CODE BEGIN CAN1_RX0_IRQn 1 */

  /* USER CODE END CAN1_RX0_IRQn 1 */
}

/**
  * @brief This function handles CAN1 RX1 interrupt.
  */
void CAN1_RX1_IRQHandler(void)
{
  /* USER CODE BEGIN CAN1_RX1_IRQn 0 */

  /* USER CODE END CAN1_RX1_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan1);
  /* USER CODE BEGIN CAN1_RX1_IRQn 1 */

  /* USER CODE END CAN1_RX1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */
//	HAL_GPIO_EXTI_IRQHandler(STM32_DAC_CS6_Pin);
//	HAL_GPIO_EXTI_IRQHandler(STM32_DAC_CS7_Pin);
//	HAL_GPIO_EXTI_IRQHandler(STM32_SSwA_P_6_Pin);
//	HAL_GPIO_EXTI_IRQHandler(V_STM32_ADC_DRDY5_1_Pin);
//	HAL_GPIO_EXTI_IRQHandler(I_STM32_ADC_DRDY5_2_Pin);
	HAL_GPIO_EXTI_IRQHandler(I_STM32_ADC_DRDY3_2_Pin);
	HAL_GPIO_EXTI_IRQHandler(V_STM32_ADC_DRDY4_1_Pin);
	HAL_GPIO_EXTI_IRQHandler(I_STM32_ADC_DRDY4_2_Pin);
	HAL_GPIO_EXTI_IRQHandler(V_STM32_ADC_DRDY5_1_Pin);
	HAL_GPIO_EXTI_IRQHandler(I_STM32_ADC_DRDY5_2_Pin);
  /* USER CODE END EXTI9_5_IRQn 0 */

  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  HAL_TIM_IRQHandler(&htim10);
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */
	//风扇发生堵转或其他故障，则停止风扇
	if(FAN_ERR == bsp_FanErrDetect())
	{
		bsp_FanStop();
	}
	else
	{
		bsp_FanSpdSet(0);
	}
	
	//如果检测到过流故障，则通过I2C读取过流的通道
	if(Curt_Fault == bsp_OcfDetect())
	{
		//写入故障信息缓冲区
		bsp_HwErrRead();
	}
  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles TIM1 trigger and commutation interrupts and TIM11 global interrupt.
  */
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_TRG_COM_TIM11_IRQn 0 */

  /* USER CODE END TIM1_TRG_COM_TIM11_IRQn 0 */
  HAL_TIM_IRQHandler(&htim11);
  /* USER CODE BEGIN TIM1_TRG_COM_TIM11_IRQn 1 */
	static uint16_t Cnt = 0x00;
	
	//喂独立看门狗
	HAL_IWDG_Refresh(&hiwdg);
	
	//开启ADS131循环采样，1ms周期
//	ADS131DataHandle();
	
	Cnt++;
	if(Cnt >= 100)
	{
		Cnt = 0x00;
		
		GetTempVal();
	}
	
	for(int i = 0; i < CHANNEL_CFG; i++)
	{
		if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || \
			(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
			(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_START  ) || \
			(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_WAIT)  )
		{
			Tier2.workstepRunning[i].Trun++;
			run_time_cnt[i] = Tier2.workstepRunning[i].Trun;
		}
		else
		{
			run_time_cnt[i] ++;
		}
	}
  /* USER CODE END TIM1_TRG_COM_TIM11_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */
	static uint8_t USART_RX_STA = 0x00;
	static uint8_t RxCnt = 0;
	
	if(0x00 == RxFlag)
	{
		if(RxData != 0x0a)
		{
			USART_RX_BUF[USART_RX_STA++]=RxData;
		}
		else
		{
			USART_RX_BUF[USART_RX_STA-1] = '\0';
			USART_RX_STA = 0;
			
			DebugPara[RxCnt++] = atoi((char*)USART_RX_BUF);
			memset(USART_RX_BUF, 0, 10);
			if(RxCnt >= 4)
			{
				RxFlag = 0x01;
				RxCnt = 0x00;
			}
		}
	}

	HAL_UART_Receive_IT(&huart1, &RxData, 1);
  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */
//	HAL_GPIO_EXTI_IRQHandler(STM32_SSwA_P_7_Pin);
//	HAL_GPIO_EXTI_IRQHandler(STM32_SSwA_P_8_Pin);
//	HAL_GPIO_EXTI_IRQHandler(STM32_SSwA_N_1_Pin);
//	HAL_GPIO_EXTI_IRQHandler(V_STM32_ADC_CS8_2_Pin);
//	HAL_GPIO_EXTI_IRQHandler(I_STM32_ADC_CS1_1_Pin);
//	HAL_GPIO_EXTI_IRQHandler(I_STM32_ADC_CS2_1_Pin);
	HAL_GPIO_EXTI_IRQHandler(V_STM32_ADC_DRDY6_1_Pin);
	HAL_GPIO_EXTI_IRQHandler(I_STM32_ADC_DRDY6_2_Pin);
	HAL_GPIO_EXTI_IRQHandler(V_STM32_ADC_DRDY7_1_Pin);
	HAL_GPIO_EXTI_IRQHandler(I_STM32_ADC_DRDY7_2_Pin);
	HAL_GPIO_EXTI_IRQHandler(V_STM32_ADC_DRDY8_1_Pin);
	HAL_GPIO_EXTI_IRQHandler(I_STM32_ADC_DRDY8_2_Pin);
  /* USER CODE END EXTI15_10_IRQn 0 */

  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles TIM8 trigger and commutation interrupts and TIM14 global interrupt.
  */
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 0 */

  /* USER CODE END TIM8_TRG_COM_TIM14_IRQn 0 */
  HAL_TIM_IRQHandler(&htim14);
  /* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 1 */
	
  /* USER CODE END TIM8_TRG_COM_TIM14_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
  * @brief This function handles CAN2 RX0 interrupts.
  */
void CAN2_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN CAN2_RX0_IRQn 0 */

  /* USER CODE END CAN2_RX0_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan2);
  /* USER CODE BEGIN CAN2_RX0_IRQn 1 */

  /* USER CODE END CAN2_RX0_IRQn 1 */
}

/**
  * @brief This function handles CAN2 RX1 interrupt.
  */
void CAN2_RX1_IRQHandler(void)
{
  /* USER CODE BEGIN CAN2_RX1_IRQn 0 */

  /* USER CODE END CAN2_RX1_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan2);
  /* USER CODE BEGIN CAN2_RX1_IRQn 1 */

  /* USER CODE END CAN2_RX1_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		case V_STM32_ADC_DRDY1_1_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_1 Vbat and Vj Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Vbat[0] += AdsData.channel0;
			ADS131_Data.Vtz[0] += AdsData.channel1;
			ADS131_Data.AdsCsFlag = 0x01 << 1;
			
			break;
		}
		case I_STM32_ADC_DRDY1_2_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_2 Imod Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Imod[0] += AdsData.channel0;	
			ADS131_Data.AdsCsFlag = 0x01 << 2;
			
			break;
		}
		case V_STM32_ADC_DRDY2_1_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_3 Vbat and Vj Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Vbat[1] += AdsData.channel0;
			ADS131_Data.Vtz[1] += AdsData.channel1;	
			ADS131_Data.AdsCsFlag = 0x01 << 3;
			
			break;
		}
		case I_STM32_ADC_DRDY2_2_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_4 Imod Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Imod[1] += AdsData.channel0;
			ADS131_Data.AdsCsFlag = 0x01 << 4;
			
			break;
		}
		case V_STM32_ADC_DRDY3_1_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_5 Vbat and Vj Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Vbat[2] += AdsData.channel0;
			ADS131_Data.Vtz[2] += AdsData.channel1;	
			ADS131_Data.AdsCsFlag = 0x01 << 5;
			
			break;
		}
		case I_STM32_ADC_DRDY3_2_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_6 Imod Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Imod[2] += AdsData.channel0;
			ADS131_Data.AdsCsFlag = 0x01 << 6;
			
			break;
		}
		case V_STM32_ADC_DRDY4_1_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_7 Vbat and Vj Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Vbat[3] += AdsData.channel0;
			ADS131_Data.Vtz[3] += AdsData.channel1;
			ADS131_Data.AdsCsFlag = 0x01 << 7;
			
			break;
		}
		case I_STM32_ADC_DRDY4_2_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_8 Imod Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Imod[3] += AdsData.channel0;
			ADS131_Data.AdsCsFlag = 0x01 << 8;
			
			break;
		}
		case V_STM32_ADC_DRDY5_1_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_9 Vbat and Vj Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Vbat[4] += AdsData.channel0;
			ADS131_Data.Vtz[4] += AdsData.channel1;
			ADS131_Data.AdsCsFlag = 0x01 << 9;
			
			break;
		}
		case I_STM32_ADC_DRDY5_2_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_10 Imod Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Imod[4] += AdsData.channel0;
			ADS131_Data.AdsCsFlag = 0x01 << 10;
			
			break;
		}
		case V_STM32_ADC_DRDY6_1_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_11 Vbat and Vj Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Vbat[5] += AdsData.channel0;
			ADS131_Data.Vtz[5] += AdsData.channel1;
			ADS131_Data.AdsCsFlag = 0x01 << 11;
			
			break;
		}
		case I_STM32_ADC_DRDY6_2_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_12 Imod Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Imod[5] += AdsData.channel0;
			ADS131_Data.AdsCsFlag = 0x01 << 12;
			
			break;
		}
		case V_STM32_ADC_DRDY7_1_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_13 Vbat and Vj Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Vbat[6] += AdsData.channel0;
			ADS131_Data.Vtz[6] += AdsData.channel1;
			ADS131_Data.AdsCsFlag = 0x01 << 13;
			
			break;
		}
		case I_STM32_ADC_DRDY7_2_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_14 Imod Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Imod[6] += AdsData.channel0;
			ADS131_Data.AdsCsFlag = 0x01 << 14;
			
			break;
		}
		case V_STM32_ADC_DRDY8_1_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_15 Vbat and Vj Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Vbat[7] += AdsData.channel0;
			ADS131_Data.Vtz[7] += AdsData.channel1;
			ADS131_Data.AdsCsFlag = 0x01 << 15;
			break;
		}
		case I_STM32_ADC_DRDY8_2_Pin:
		{
			//关闭当前的中断触发源，防止重复进入中断服务函数
			DisableExtIRQ(ADS131_Data.AdsCsFlag);
			
			//Read ADS_16 Imod Data
			readData(&AdsData, (CS_Type)ADS131_Data.AdsCsFlag);
			ADS131_Data.Imod[7] += AdsData.channel0;
			ADS131_Data.AdsCsFlag = 0x01 << 0;
			ADS131_Data.AdsCollectCnt++;
			break;
		}
		default:
		{
			break;
		}
	}
}
/* USER CODE END 1 */

