/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
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

#if 0
/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}
#endif

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

#if 0
/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}
#endif

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

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

  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(INT_OTP_Pin);
  HAL_GPIO_EXTI_IRQHandler(INT_OCP_OVP_Pin);
  HAL_GPIO_EXTI_IRQHandler(INT_current_Pin);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
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

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(ADC3_BUSY_Pin);
  HAL_GPIO_EXTI_IRQHandler(ADC2_BUSY_Pin);
  HAL_GPIO_EXTI_IRQHandler(ADC1_BUSY_Pin);
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
uint8_t time4_cnt = 0;
uint8_t time5_cnt = 0;

void TIM4_IRQHandler(void)
{	 
	uint8_t Process_Flag = 0;
	ErrCnt++;		
	if((TIM4->SR & TIM_FLAG_UPDATE) != RESET)
    {
		if(time4_cnt >= 25)  //25*8 = 200ms
	 {
			IntelProtectRunParmCheck();    //内部保护检测
            GlobleProtectRunParmCheck();   //全局保护检测
			
			for(uint8_t i = 0; i < CHANNEL_MAX; i++)
			{
				if((IP_Alarm_Flag[i] != 0)||(GP_Alarm_Flag[i] != 0))
				{
					Process_Flag = 1;
					break;
				}
			}
			if(Process_Flag == 0x01)
			{
				osSemaphoreRelease(SemaProtection);  //释放信号量，同步上抛故障信息
			}
			time4_cnt = 0;
		}
		else
		{
			time4_cnt++;
		}
		TIM4->SR = ~TIM_FLAG_UPDATE;
	}
}


void TIM5_IRQHandler(void)
{
    if((TIM5->SR & TIM_FLAG_UPDATE) != RESET)
    {
        TIM5->SR = ~TIM_FLAG_UPDATE;
		
        /* 全局运行时间每1ms增1 */
        Tstamp++;		

		if((Tstamp % 250) == 0)
		{
			//记录数据查询
			DcdcParaInq(DEBUG_CH, RecordDataInq, 0x00);
			CanSendMsg(&hcan2, &DcdcSendFrame.ChFlag, 0x08, DcdcCanId);	
		}
		
		Can_Network_Func(1000, 50, 50, 50, 1);
		
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
			
			if(record_data_upcast_flag[i] == 1)
			{
				if((Tier2.workstepAnalyze[i].StepAbtTime > 0) && \
					(Tier2.workstepRunning[i].Trun / RUN_TIME_PERIOD >= Tier2.workstepAnalyze[i].StepAbtTime) && \
					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_WAIT) && \
					(abort_type_flag[i] == 0) )
				{
					abort_type_flag[i] = SCMD_WORKSTEP_PARAM_TIME;
				}
				
				record_data_upcast_flag[i] = 0;
				
				RecordDatUpcast(i);
			}
			else if(((run_time_cnt[i] % ((uint64_t)(data_upcast_period * 1000))) == 0) && (run_time_cnt[i] != 0))
			{
				if( (Tier2.workstepAnalyze[i].StepAbtTime > 0) && \
					(Tier2.workstepRunning[i].Trun / RUN_TIME_PERIOD >= Tier2.workstepAnalyze[i].StepAbtTime) && \
					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_WAIT) && \
					(abort_type_flag[i] == 0) )
				{
					abort_type_flag[i] = SCMD_WORKSTEP_PARAM_TIME;
				}
				
				RecordDatUpcast(i);
			}
		}
		
		ExternalAdDatProcess();
		
		if(time5_cnt == 10)
		{
			time5_cnt = 0;
			
			for(int i = 0; i < CHANNEL_CFG; i++)
			{
				uint8_t ch_numb = CHANNEL_MAX / CHANNEL_CFG;    // 每个外部通道对应的内部通道数量

				Tier2.chRecord[i].Trun = (float)Tier2.workstepRunning[i].Trun;
				Tier2.chRecord[i].Vbat = Tier2.sampleCal.Vsense[ch_numb * i];
				Tier2.chRecord[i].Vtz = Tier2.sampleCal.Vtz[ch_numb * i];
				//Tier2.chRecord[i].Iout = Tier2.sampleCal.Iout[i];
				
				if( ((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_START)) && \
					(Tier2.workstepRunning[i].StepType != SCMD_WORKSTEP_TYPE_STANDBY))
				{
					if(Tier2.chRecord[i].Iout > (0.0005f * WORK_CUR_MAX))
					{
						Tier2.chRecord[i].Cbat += Tier2.chRecord[i].Iout * 0.01 / 3600.0f;
						Tier2.chRecord[i].Ebat += Tier2.chRecord[i].Vbat * Tier2.chRecord[i].Iout * 0.01 / 3600.0f;
					}
				}
			}
			
		}
		else
		{
			time5_cnt++;
		}
	}
}
/* USER CODE END 1 */
