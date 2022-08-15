/**
  ******************************************************************************
  * @file    HAL/HAL_TimeBase/Src/stm32f4xx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "stm32f4xx_it.h"
#include "iwdg.h"
#include "main.h"
/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_EXTI
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ACDC_UpdateStrcut CanRet = {0, {0}, 0}; //ACDC����can�ظ��ṹ��
__IO uint8_t Clear_AM_Flg = 0;                //����澯֪ͨ
uint8_t ACDC_Reply = 0;
uint8_t PG_state = 0;   //PG״̬��bit����ҵͣ���ʾPG8~PG1
extern  Net_Mod Net_Mod_State;
extern  __IO uint32_t record_upcast_cnt;
extern  __IO uint32_t heart_beat_cnt;
extern  __IO uint8_t Recon_TimStart; //������ʱ������־
extern  __IO uint8_t Fram_Op;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/* External variables --------------------------------------------------------*/
//extern __IO uint64_t g_iRunTime;
extern DMA_HandleTypeDef hdma_adc3;
extern CAN_HandleTypeDef hcan1;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi6_tx;
extern DMA_HandleTypeDef hdma_spi6_rx;
extern SPI_HandleTypeDef SPI1_Handler;
extern SPI_HandleTypeDef SPI6_Handler;
extern CAN_RxHeaderTypeDef   RxMeg;
extern osSemaphoreId_t acdc_alarm_Semaphore;        /* ACDC�澯�ź���ID */
extern osSemaphoreId_t IPGP_Alarm_Semaphore;        /* ȫ���ڲ��澯�ź���ID */
extern __IO uint8_t ACDC_UpdateFlg;					/*ACDC������־*/
extern osMessageQueueId_t ACDC_UpdateQueue;
extern CMDProject IP_CheckList[];
extern uint32_t IP_Alarm_Flag[8];       //�ڲ�������־λ
extern uint32_t GP_Alarm_Flag[8];       //ȫ�ֱ�����־λ
extern uint32_t IP_Alarm_Flag_History[8];  //�ڲ�������ʷ��־λ
extern uint32_t GP_Alarm_Flag_History[8];  //ȫ�ֱ�����ʷ��־λ
//extern CAN_HandleTypeDef hcan1;




/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while(1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while(1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while(1)
    {
    }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/**
  * @brief This function handles USART3 global interrupt.
  */


void  DEBUG_USART_IRQHandler(void)
{

    /* USER CODE BEGIN USART3_IRQn 0 */
    /* USER CODE END USART3_IRQn 0 */
    HAL_DBGUART_RxTxISRHandler();
    /* USER CODE BEGIN USART3_IRQn 1 */

    /* USER CODE END USART3_IRQn 1 */

}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
    /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

    /* USER CODE END DMA2_Stream0_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_adc3);
    /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

    /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void DMA2_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi1_rx);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */

  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */

  /* USER CODE END DMA2_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
  /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

  /* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream5 global interrupt.
  */
void DMA2_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream5_IRQn 0 */

  /* USER CODE END DMA2_Stream5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi6_tx);
  /* USER CODE BEGIN DMA2_Stream5_IRQn 1 */

  /* USER CODE END DMA2_Stream5_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream6 global interrupt.
  */
void DMA2_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream6_IRQn 0 */

  /* USER CODE END DMA2_Stream6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi6_rx);
  /* USER CODE BEGIN DMA2_Stream6_IRQn 1 */

  /* USER CODE END DMA2_Stream6_IRQn 1 */
}

void SPI1_IRQHandler(void)
{
  /* USER CODE BEGIN SPI1_IRQn 0 */

  /* USER CODE END SPI1_IRQn 0 */
  HAL_SPI_IRQHandler(&SPI1_Handler);
  /* USER CODE BEGIN SPI1_IRQn 1 */

  /* USER CODE END SPI1_IRQn 1 */
}

/**
  * @brief This function handles SPI6 global interrupt.
  */
void SPI6_IRQHandler(void)
{
  /* USER CODE BEGIN SPI6_IRQn 0 */

  /* USER CODE END SPI6_IRQn 0 */
  HAL_SPI_IRQHandler(&SPI6_Handler);
  /* USER CODE BEGIN SPI6_IRQn 1 */

  /* USER CODE END SPI6_IRQn 1 */
}


/*
*********************************************************************************************************
*   �� �� ��: TIM4_IRQHandler
*   ����˵��: 200MS����ɨ��Ӳ�����ϸ澯��
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void TIM4_IRQHandler(void)
{
    static uint8_t cnt[8];
    static uint8_t i = 0;
    if((TIM4->SR & TIM_FLAG_UPDATE) != RESET)
    {
        TIM4->SR = ~ TIM_FLAG_UPDATE;
        Feed_WDG();  //ι��
		if(ACDC_UpdateFlg == 0)      //ACDC����ģʽ�£������Ӳ������
		{
        /* ��ѯPG1~8,�ж�����ͨ��*/
        i++;
        if(HAL_GPIO_ReadPin(PG1_GPIO_Port, PG1_Pin))
        {
            cnt[0]++;
            if((cnt[0] >= 2) && (i == 2))
            {
                PG_state |= 0x01 << 0;
                switch(CHANNEL_CFG)
                {
                    case 1:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 2:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 4:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 8:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    default
                            :
                        break;
                }
            }
        }
        else
        {
            PG_state &= ~(0x01 << 0);
        }

        if(HAL_GPIO_ReadPin(PG2_GPIO_Port, PG2_Pin))
        {
            cnt[1]++;
            if((cnt[1] >= 2) && (i == 2))
            {
                PG_state |= 0x01 << 1;
                switch(CHANNEL_CFG)
                {
                    case 1:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 2:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 4:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 8:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[1] |= IntelProtect_HPA;
                        break;
                    default
                            :
                        break;
                }
            }
        }
        else
        {
            PG_state &= ~(0x01 << 1);
        }

        if(HAL_GPIO_ReadPin(PG3_GPIO_Port, PG3_Pin))
        {
            cnt[2]++;
            if((cnt[2] >= 2) && (i == 2))
            {
                PG_state |= 0x01 << 2;
                switch(CHANNEL_CFG)
                {
                    case 1:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 2:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 4:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[1] |= IntelProtect_HPA;
                        break;
                    case 8:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[2] |= IntelProtect_HPA;
                        break;
                    default
                            :
                        break;
                }
            }
        }
        else
        {
            PG_state &= ~(0x01 << 2);
        }

        if(HAL_GPIO_ReadPin(PG4_GPIO_Port, PG4_Pin))
        {
            cnt[3]++;
            if((cnt[3] >= 2) && (i == 2))
            {
                PG_state |= 0x01 << 3;
                switch(CHANNEL_CFG)
                {
                    case 1:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 2:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 4:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[1] |= IntelProtect_HPA;
                        break;
                    case 8:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[3] |= IntelProtect_HPA;
                        break;
                    default
                            :
                        break;
                }
            }
        }
        else
        {
            PG_state &= ~(0x01 << 3);
        }

        if(HAL_GPIO_ReadPin(PG5_GPIO_Port, PG5_Pin))
        {
            cnt[4]++;
            if((cnt[4] >= 2) && (i == 2))
            {
                PG_state |= 0x01 << 4;
                switch(CHANNEL_CFG)
                {
                    case 1:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 2:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[1] |= IntelProtect_HPA;
                        break;
                    case 4:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[2] |= IntelProtect_HPA;
                        break;
                    case 8:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[4] |= IntelProtect_HPA;
                        break;
                    default
                            :
                        break;
                }
            }
        }
        else
        {
            PG_state &= ~(0x01 << 4);
        }

        if(HAL_GPIO_ReadPin(PG6_GPIO_Port, PG6_Pin))
        {
            cnt[5]++;
            if((cnt[5] >= 2) && (i == 2))
            {
                PG_state |= 0x01 << 5;
                switch(CHANNEL_CFG)
                {
                    case 1:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 2:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[1] |= IntelProtect_HPA;
                        break;
                    case 4:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[2] |= IntelProtect_HPA;
                        break;
                    case 8:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[5] |= IntelProtect_HPA;
                        break;
                    default
                            :
                        break;
                }
            }
        }
        else
        {
            PG_state &= ~(0x01 << 5);
        }

        if(HAL_GPIO_ReadPin(PG7_GPIO_Port, PG7_Pin))
        {
            cnt[6]++;
            if((cnt[6] >= 2) && (i == 2))
            {
                PG_state |= 0x01 << 6;
                switch(CHANNEL_CFG)
                {
                    case 1:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 2:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[1] |= IntelProtect_HPA;
                        break;
                    case 4:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[3] |= IntelProtect_HPA;
                        break;
                    case 8:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[6] |= IntelProtect_HPA;
                        break;
                    default
                            :
                        break;
                }
            }
        }
        else
        {
            PG_state &= ~(0x01 << 6);
        }

        if(HAL_GPIO_ReadPin(PG8_GPIO_Port, PG8_Pin))
        {
            cnt[7]++;
            if((cnt[7] >= 2) && (i == 2))
            {
                PG_state |= 0x01 << 7;
                switch(CHANNEL_CFG)
                {
                    case 1:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[0] |= IntelProtect_HPA;
                        break;
                    case 2:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[1] |= IntelProtect_HPA;
                        break;
                    case 4:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[3] |= IntelProtect_HPA;
                        break;
                    case 8:
						AuxRecordDatSync(HPA_ERR);
                        IP_Alarm_Flag[7] |= IntelProtect_HPA;
                        break;
                    default
                            :
                        break;
                }
            }
        }
        else
        {
            PG_state &= ~(0x01 << 7);
        }

        if(i == 2)
        {
            i = 0;
            for(uint8_t j = 0; j < 8; j++)
            {
                cnt[j] = 0;
            }
        }
		}
	}
}

/*
*********************************************************************************************************
*   �� �� ��: TIM5_IRQHandler
*   ����˵��: 1ms����ʱ����
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
//void TIM5_IRQHandler(void)
//{
//    if((TIM5->SR & TIM_FLAG_UPDATE) != RESET)
//    {
//        TIM5->SR = ~ TIM_FLAG_UPDATE;
//        /* ȫ������ʱ��ÿ1ms��1 */
//        // g_iRunTime++;
//        Tstamp++;

//        if(Fram_Op == 0)
//        {
//            for(int i = 0; i < CHANNEL_MAX; i++)
//            {
//                if((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || \
//                        (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT))
//                {
//                    Tier2.workstepRunning[i].Trun++;
//                    Tier2.workstepRunning[i].Tpro++;
//                }
//            }


//            if(!Net_Mod_State.NetMod_Disable)
//            { Net_Mod_State.NetMod_cnt++; }
//            record_upcast_cnt++;
//            heart_beat_cnt++;
//        }
//    }
//}

uint8_t upcast_period_flag[8] = {0};
extern void ExternalAdDatProcess(void);

#if (M_LOOP_ENABLE == 1)
uint8_t workstep_flag_CriticalZone_Pre[8] = {0};
uint8_t CriticalZoneFlag[8] = {0};
uint16_t CriticalZoneCnt[8] = {0};
#endif


uint8_t continue_store_delay_flag[8] = {0};
uint16_t continue_store_delay_cnt[8] = {0};

uint16_t bus_under_vol_cnt = 0;
uint16_t bus_under_vol_flg = 0;

void TIM5_IRQHandler(void)
{
    if((TIM5->SR & TIM_FLAG_UPDATE) != RESET)
    {
        TIM5->SR = ~ TIM_FLAG_UPDATE;
        /* ȫ������ʱ��ÿ1ms��1 */
		// g_iRunTime++;
        Tstamp++;
		
//		heart_beat_cnt++;
		
		if(bus_under_vol_cnt < 30000)
		{
			bus_under_vol_cnt++;
		}
		else
		{
			bus_under_vol_flg = 1;
		}		
		
		if((Tstamp % 100) == 0)
		{
			Feed_WDG();  //ι��
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
					
					
	
			if((Recon_TimStart != 1) || (ACDC_UpdateFlg != 0))
			{
				continue;
			}
			
			if(record_data_upcast_flag[i] == 1)
			{
				if( (Tier2.workstepAnalyze[i].StepAbtTime > 0) && \
					(Tier2.workstepRunning[i].Trun / RUN_TIME_PERIOD >= Tier2.workstepAnalyze[i].StepAbtTime) && \
					(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_WAIT) && \
					(abort_type_flag[i] == 0) )
				{
					abort_type_flag[i] = SCMD_WORKSTEP_PARAM_TIME;
				}
				
				record_data_upcast_flag[i] = 0;
				
				RecordDatUpcast(i, 1);
//				upcast_period_flag[i] = 1;
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
				
				RecordDatUpcast(i, 1);
//				upcast_period_flag[i] = 1;
			}
			
#if (M_LOOP_ENABLE == 1)			
			if((workstep_flag_CriticalZone[i] == 0) && (workstep_flag_CriticalZone_Pre[i] != 0))
			{
				/* MLOOP�� */
#ifdef YCJ
				Disable_Mloop(i);
#else				
				Disable_Mloop(2 * i + 0);
				Disable_Mloop(2 * i + 1);
#endif
				CriticalZoneFlag[i] = 0;
				CriticalZoneCnt[i] = 0;				
			}
			else if((workstep_flag_CriticalZone[i] == 3) && (workstep_flag_CriticalZone_Pre[i] != 3))
			{
				/* MLOOP�� */
				CriticalZoneFlag[i] = 1;
				CriticalZoneCnt[i] = 0;
			}
			workstep_flag_CriticalZone_Pre[i] = workstep_flag_CriticalZone[i];
			
			if(CriticalZoneFlag[i] == 1)
			{
				if(CriticalZoneCnt[i] < 2000)
				{
					CriticalZoneCnt[i]++;
				}
				else
				{
					if(Tier2.workstepAnalyze[i].StepRunCur - Tier2.chRecord[i].Iout >= 1.0f)
					{
#ifdef YCJ
						Enable_Mloop(i);
#else						
						Enable_Mloop(2 * i + 0);
						Enable_Mloop(2 * i + 1);	
#endif
						CriticalZoneFlag[i] = 0;
						CriticalZoneCnt[i] = 0;							
					}
				}
			}
#endif			
			
		}

		
//        if(Fram_Op == 0)
		{
			for(int j = 0; j < CHANNEL_CFG; j++)
			{
				if( (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || \
					(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
					(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_START  ) || \
					(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_WAIT   ) || \
					(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_PAUSE  ) )
				{
					if(refresh_continue[j] == 0)
					{
						if((power_off_continue[j] != 1) && (continue_store_delay_flag[j] == 0))
						{
							power_off_continue[j] = 1;
							
							continue_store_delay_flag[j] = 1;
						}
					}
				}
				else
				{
					if(refresh_continue[j] == 0)
					{
						if((power_off_continue[j] != 0) && (continue_store_delay_flag[j] == 0))
						{
							power_off_continue[j] = 0;
							
							continue_store_delay_flag[j] = 1;
						}
					}
				}


				if(continue_store_delay_flag[j] == 1)
				{
					if(++continue_store_delay_cnt[j] > 2000)
					{
						continue_store_delay_cnt[j] = 0;
						
						continue_store_delay_flag[j] = 0;
						
						MB85Ctrl_Write(8208 + j, &power_off_continue[j], 1);
					}
				}
				
				
				
			}
	
			if(!Net_Mod_State.NetMod_Disable)
			{
				Net_Mod_State.NetMod_cnt++;
			}
			
			record_upcast_cnt++;
        }
		
        bsp_ProPer1ms();
        ExternalAdDatProcess();	

		ADC_Start_Stop_DMA();
		
    }
}



/*
*********************************************************************************************************
*   �� �� ��: TIM3_IRQHandler
*   ����˵��: 40ms���и澯�������
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void TIM3_IRQHandler(void)
{
    uint8_t i;
    uint8_t Process_Flag = 0;
    static uint16_t cnt;
    //    static uint8_t VUQ_Count[CHANNEL_CFG] = {0};    // ��ѹ������������
    //    static uint8_t VDQ_Count[CHANNEL_CFG] = {0};    // ��ѹ�����½�����
    //    static uint8_t CUQ_Count[CHANNEL_CFG] = {0};    // ����������������
    //    static float lastVbat[CHANNEL_CFG] = {0};       // �ϴε�ѹֵ
    //    static float lastCmod[CHANNEL_CFG] = {0};       // �ϴε���ֵ
    if((TIM3->SR & TIM_FLAG_UPDATE) != RESET)
    {
        TIM3->SR = ~ TIM_FLAG_UPDATE;
        //     for(i = 0; i < CHANNEL_CFG; i++)
        //    {
        //        if(((Tier2.iProt[i].AmpVdrop.isEnable & 0x80) == 0) && ((Tier2.iProt[i].CntVdrop.isEnable & 0x80) == 0))
        //        {
        //            if((SystemStatus.Neilsbed == Neilsbed_Nofit) || (SystemStatus.Neilsbed == Neilsbed_Fitted))
        //            {
        //                if(lastVbat[i] - Tier2.chRecord[i].Vbat > Tier2.iProt[i].AmpVdrop.value)
        //                {
        //                    VDQ_Count[i]++;
        //                }
        //                else
        //                {
        //                    VDQ_Count[i] = 0;
        //                }
        //            }
        //            else
        //            {
        //                CUQ_Count[i] = 0;
        //            }
        //            if(VDQ_Count[i] >= Tier2.iProt[i].CntVdrop.value)
        //            {
        //                IP_Alarm_Flag[i] |= IntelProtect_VDQ;
        //            }
        //        }
        //        if(((Tier2.iProt[i].AmpVup.isEnable & 0x80) == 0) && ((Tier2.iProt[i].CntVup.isEnable & 0x80) == 0))
        //        {
        //            if((SystemStatus.Neilsbed == Neilsbed_Nofit) || (SystemStatus.Neilsbed == Neilsbed_Fitted))
        //            {
        //                if(Tier2.chRecord[i].Vbat - lastVbat[i] > Tier2.iProt[i].AmpVup.value)
        //                {
        //                    VUQ_Count[i]++;
        //                }
        //                else
        //                {
        //                    VUQ_Count[i] = 0;
        //                }
        //            }
        //            else
        //            {
        //                CUQ_Count[i] = 0;
        //            }
        //            if(VUQ_Count[i] >= Tier2.iProt[i].CntVup.value)
        //            {
        //                IP_Alarm_Flag[i] |= IntelProtect_VUQ;
        //            }
        //        }
        //        lastVbat[i] = Tier2.chRecord[i].Vbat;
        //        if(((Tier2.iProt[i].AmpCup.isEnable & 0x80) == 0) && \
        //                ((Tier2.iProt[i].CntCup.isEnable & 0x80) == 0))
        //        {
        //            if((SystemStatus.Neilsbed == Neilsbed_Nofit) || (SystemStatus.Neilsbed == Neilsbed_Fitted))
        //            {
        //                if(Tier2.chRecord[i].Iout - lastCmod[i]  > Tier2.iProt[i].AmpCup.value)
        //                {
        //                    CUQ_Count[i]++;
        //                }
        //                else
        //                {
        //                    CUQ_Count[i] = 0;
        //                }
        //            }
        //            else
        //            {
        //                CUQ_Count[i] = 0;
        //            }
        //            if(CUQ_Count[i] > Tier2.iProt[i].CntCup.value)
        //            {
        //                IP_Alarm_Flag[i] |= IntelProtect_CUQ;
        //            }
        //        }
        //        lastCmod[i] = Tier2.chRecord[i].Iout;
        //    }
        if(cnt == 10)	//400ms
        {
            cnt = 0;
            bsp_LedToggle(LED_82_NO);
			bsp_LedToggle(LED_83_NO);
			Feed_WDG();
            IntelProtectRunParmCheck();    //�ڲ��������
            GlobleProtectRunParmCheck();   //ȫ�ֱ������
            if(Clear_AM_Flg)               //���յ�����澯����������и澯״̬
            {
                AlarmClear();
                Clear_AM_Flg = 0;
            }
            for(i = 0; i < CHANNEL_CFG; i++)
            {
                if((IP_Alarm_Flag[i] != 0) || (GP_Alarm_Flag[i] != 0))
                {
                    Process_Flag = 1;
                    break;
                }
            }
            if((Process_Flag) && (ACDC_UpdateFlg == 0)) //�澯��־��Ϊ0,��ACDC����������״̬
            {
                osSemaphoreRelease(IPGP_Alarm_Semaphore);    //�ͷ��ź�����ͬ���澯��������
                HAL_NVIC_DisableIRQ(TIM3_IRQn);
            }
        }
        cnt++;
    }
}

/*
*********************************************************************************************************
*   �� �� ��: CAN1_RX0_IRQHandler
*   ����˵��: can1fifo0�����жϷ��񣬽��չ�������
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void CAN1_RX0_IRQHandler(void)
{
    /* USER CODE BEGIN CAN2_RX0_IRQn 0 */
    /* USER CODE END CAN2_RX0_IRQn 0 */
    HAL_CAN_IRQHandler(&hcan1);
    /* USER CODE BEGIN CAN2_RX0_IRQn 1 */
    uint8_t  Data[8];
    HAL_StatusTypeDef   HAL_RetVal;
    uint32_t  ID = 0;
    uint8_t i = 0;

    HAL_RetVal = HAL_CAN_GetRxMessage(&hcan1,  CAN_FIFO0, &RxMeg,  Data);
    ID = RxMeg.ExtId;
    /*can�ⲿecho����*/
#if 0
    if(HAL_OK == HAL_RetVal)
    {
        CANx_SendData(&hcan1, ID, Data, 8);
    }
#endif
    
        /*����ģʽ���ظ�����*/
    if((HAL_OK == HAL_RetVal) && (ACDC_UpdateFlg == 1))
    {
        if((ID & 0X1FF00000) == 0X10500000)          //����������
        {
            //�������ݵ��ṹ�壬�ṹ�巢��Ϣ����
            ID = (ID >> 24 & 0xFF) + ((ID >> 16 & 0xFF) << 8) + ((ID >> 8 & 0xFF) << 16) + ((ID & 0xFF) << 24); //ID��С��ת��
            CanRet.ID = ID;                   //��ȡ�ظ���ID
            memcpy(CanRet.Date, Data, 8);     //��ȡ�ظ�������
            CanRet.processedFlg = 1;          //1��ʾ�յ������ظ����ݴ�����
        }
    }
      
    /*������ģʽ���ظ�����*/
    if((HAL_OK == HAL_RetVal) && (ACDC_UpdateFlg == 0))
    {
        //MCU������ѯģ�鱻���ظ�����
        if(((ID & 0X1FFF0000) == 0X102B0000) && (Data[0] == 0XF0))
        {
            switch((ID >> 8) & 0xFF)
                //�ж�ģ��ID��Ӧ�洢�����±�
            {
                case 0x00:
                    i = 0;
                    break;
                case 0x01:
                    i = 1;
                    break;
                default
                        :
                    break;
            }

            switch(Data[1])
            {
                case 0x37://����ת�ٸ���
                    ACDC.FanSpd[i] = Data[7];
                    ACDC_Reply = 1;
                    break;
                case 0x53://ACDCֱ�����ѹ����
                    ACDC.DCV[i] = ((uint16_t)(Data[6] << 8) + Data[7]) / 10.0;
                    break;
                case 0x54://ACDCֱ�����������
                    ACDC.DCI[i] = ((uint16_t)(Data[6] << 8) + Data[7]) / 10.0;
                    break;
                case 0x59://ģ��������������
                    /*�������״̬�ޱ仯���򲻸���ACDC״̬*/
                    if(getSn_SR(SOCK_0_NUMB) == SOCK_ESTABLISHED)
                    {
                        if(ACDC.State[i] != ((uint16_t)(Data[6] << 8) + Data[7]))
                        {
                            ACDC.BitFlg[i] = ACDC.State[i] ^ ((uint16_t)(Data[6] << 8) + Data[7]); //��¼״̬�ı��bit
                            ACDC.State[i] = ((uint16_t)(Data[6] << 8) + Data[7]);  //����state�ֶ�
                            osSemaphoreRelease(acdc_alarm_Semaphore);    //�ͷ��ź�����ͬ����������
                        }
                    }
                    break;
                case 0x62://ACDC�������ѹ����
                    ACDC.ACV[i] = ((uint16_t)(Data[6] << 8) + Data[7]) / 10.0;
                    if(i==0)
                    ACDC.QueryWaitFlag=QueryACV1;        //���µȴ���־
                    else if(i==1)
                    ACDC.QueryWaitFlag=QueryACV2;        //���µȴ���־                     
                    break;                

                case 0x63://ACDC�������������
                    ACDC.ACI[i] = ((uint16_t)(Data[6] << 8) + Data[7]) / 10.0;
                    break;
                default
                        :
                    break;
            }
        }
    }

    /* USER CODE END CAN2_RX0_IRQn 1 */
}

/*
*********************************************************************************************************
*   �� �� ��: CAN1_RX1_IRQHandler
*   ����˵��: can1fifo1�����жϷ��񣬽����յ�ACDC����
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void CAN1_RX1_IRQHandler(void)
{
    /* USER CODE BEGIN CAN2_RX0_IRQn 0 */
    /* USER CODE END CAN2_RX0_IRQn 0 */
    HAL_CAN_IRQHandler(&hcan1);
    /* USER CODE BEGIN CAN2_RX0_IRQn 1 */
    uint8_t  Data[8];
    HAL_StatusTypeDef   HAL_RetVal;
    uint32_t  ID = 0;
    uint8_t i = 0;

    HAL_RetVal = HAL_CAN_GetRxMessage(&hcan1,  CAN_FIFO1, &RxMeg,  Data);
    ID = RxMeg.ExtId;

    /*����ģʽ���ظ�����*/
    if((HAL_OK == HAL_RetVal) && (ACDC_UpdateFlg == 1))
    {
        if((ID & 0X1F000000) == 0X1c000000)
        {
            //�������ݵ��ṹ�壬�ṹ�巢��Ϣ����
            ID = (ID >> 24 & 0xFF) + ((ID >> 16 & 0xFF) << 8) + ((ID >> 8 & 0xFF) << 16) + ((ID & 0xFF) << 24); //ID��С��ת��
            CanRet.ID = ID;
            memcpy(CanRet.Date, Data, 8);
            CanRet.processedFlg = 1;    //1��ʾ�յ������ظ����ݴ�����
        }
    }

    /*������ģʽ���ظ�����*/
    if((HAL_OK == HAL_RetVal) && (ACDC_UpdateFlg == 0))
    {
        //MCU������ѯģ�鱻���ظ�����
        if(((ID & 0X1FFF0000) == 0X06080000) && (Data[1] == 0X30))
        {
            switch((ID >> 3) & 0xFF)
                //�ж�ģ��ID��Ӧ�洢�����±�
            {
                case 0x01:
                    i = 0;
                    break;
                case 0x02:
                    i = 1;
                    break;
                default
                        :
                    break;
            }


            switch(Data[3])
            {
                case 0x01://ACDCֱ�����ѹ����
                    ACDC.DCV[i] = (uint32_t)((Data[4] << 24) + (Data[5] << 16) + (Data[6] << 8) + Data[7]) / 1024.0;
                    break;
                case 0x02://ACDCֱ�����������
                    ACDC.DCI[i] = (uint32_t)((Data[4] << 24) + (Data[5] << 16) + (Data[6] << 8) + Data[7]) / 1024.0;
                    break;
                case 0x43://ACDC����״̬���������Э��
                    /*�������״̬�ޱ仯���򲻸���ACDC״̬*/
                    if(ACDC.State[i] != ((uint16_t)(Data[6] << 8) + Data[7]))
                    {
                        ACDC.BitFlg[i] = ACDC.State[i] ^ ((uint16_t)(Data[6] << 8) + Data[7]); //��¼״̬�ı��bit
                        ACDC.State[i] = ((uint16_t)(Data[6] << 8) + Data[7]);   //����״̬
                        osSemaphoreRelease(acdc_alarm_Semaphore);     //�ͷ��ź�����ͬ����������
                    }
                    break;
                case 0x05://ACDC�������ѹ����
                    ACDC.ACV[i] = (uint32_t)((Data[4] << 24) + (Data[5] << 16) + (Data[6] << 8) + Data[7]) / 1024.0;
                    if(i==0)
                    ACDC.QueryWaitFlag=QueryACV1;        //���µȴ���־
                    else if(i==1)
                    ACDC.QueryWaitFlag=QueryACV2;        //���µȴ���־      				
                    break;
                case 0x06://ACDC�������������
                    ACDC.ACI[i] = (uint32_t)((Data[4] << 24) + (Data[5] << 16) + (Data[6] << 8) + Data[7]) / 1024.0;
                    break;                    
                case 0xBE://ACDC�̼��汾��ѯ����
                    for(uint8_t m=0;m<4;m++)
                    ACDC.VERSION[i][m]= Data[4+m];
                    if(i==0)
                    ACDC.QueryWaitFlag=QueryVERSION1;        //���µȴ���־
                    else if(i==1)
                    ACDC.QueryWaitFlag=QueryVERSION2;        //���µȴ���־  				
                    break;
                    
                default
                        :
                    break;
            }
        }
        //MCU��������ģ�鱻���ظ�����
        if(((ID & 0X1FFF0000) == 0X06080000) && (Data[1] == 0X10) && (Data[3] == 0x1f))
        {
            switch((ID >> 3) & 0xFF)
                //�ж�ģ��ID��Ӧ�洢�����±�
            {
                case 0x01:
                    i = 0;
                    break;
                case 0x02:
                    i = 1;
                    break;
                default
                        :
                    break;
            }
            ACDC.FanSpd[i] = Data[7];   //���·���ת��
            ACDC_Reply = 1;
        }
    }

    /* USER CODE END CAN2_RX0_IRQn 1 */
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
