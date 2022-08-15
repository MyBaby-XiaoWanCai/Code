/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "can.h"
#include "dma.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/*-------------------------------RTX操作系统对象------------------------------*/
/* 任务句柄 */
osThreadId_t ThreadIdStart = NULL;                   //起始任务
osThreadId_t ThreadIdDebug = NULL;                   //调试任务
osThreadId_t ThreadIdDcdc = NULL;                    //DCDC任务
osThreadId_t ThreadIdProtection = NULL;              //保护任务
osThreadId_t ThreadIdProtocol = NULL;                //协议栈任务
osThreadId_t ThreadIdWorkstep = NULL;                //工步任务

/* 任务属性设置 */
const osThreadAttr_t ThreadStart_Attr =               //起始任务参数配置
{
    .name = "osRtxStartThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 2048,
};

/* 任务属性设置 */
const osThreadAttr_t ThreadDebug_Attr =             //调试任务参数配置
{
    .name = "osRtxDebugThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 2048,
};

/* 任务属性设置 */
const osThreadAttr_t ThreadDcdc_Attr =              //DCDC任务参数配置
{
    .name = "osRtxDcdcThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal2,
    .stack_size = 4096,
};

/* 任务属性设置 */                                 //保护任务参数配置
const osThreadAttr_t ThreadProtection_Attr =
{
    .name = "osRtxProtectionThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 4096,
};

/* 任务属性设置 */                                 //协议栈任务参数配置
const osThreadAttr_t ThreadProtocol_Attr =
{
    .name = "osRtxProtocolThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 4096,
};

/* 任务属性设置 */                                 //工步任务参数配置
const osThreadAttr_t ThreadWorkstep_Attr =
{
    .name = "osRtxWorkstepThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 10240,
}; 

/* 信号量句柄 */
osSemaphoreId_t SemaDcdcCan = NULL;

/* DCDC CAN接收信号量属性设置 */
const osSemaphoreAttr_t DcdcCanSema_Attr = 
{
	.name = "osRtxDcdcCanSema,",
};

//测试EEPROM
uint8_t EepromTest[2] = {0x08, 0x0a};
uint8_t FramTest[2] = {0x09, 0x0b};
uint8_t AddrVal = 0x00;

/* 信号量属性 */

/* 定时器属性 */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void CreateTask(void)
{
	//创建DcdcCan信号量
	SemaDcdcCan = osSemaphoreNew(1, 0, &DcdcCanSema_Attr);
	
	//创建任务
	ThreadIdDebug = osThreadNew(AppTaskDebug, NULL, &ThreadDebug_Attr);
	ThreadIdDcdc = osThreadNew(AppTaskDcdc, NULL, &ThreadDcdc_Attr);
	ThreadIdProtection = osThreadNew(AppTaskProtection, NULL, &ThreadProtection_Attr);
	ThreadIdProtocol = osThreadNew(AppTaskProtocol, NULL, &ThreadProtocol_Attr);
	ThreadIdWorkstep = osThreadNew(AppTaskWorkStep, NULL, &ThreadWorkstep_Attr);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
       
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_IWDG_Init();
  MX_TIM10_Init();
//  MX_USART1_UART_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
	bsp_InitDWT();
	
	//I2C设备初始化必须放在bsp_InitDWT初始化之后，I2C设备初始化调用了延时函数
	I2C_ParamTabInit();
	bsp_LedInit();
	bsp_AddrErrInit();
    
    /*铁电初始化*/
    MB85Ctrl_Init();
	bsp_EepromInit();
	
	//测试
	ee_WriteBytes(EepromTest, 0x00, 0x02);
	bsp_DelayMS(1);
	EepromTest[0] = 0x00;
	EepromTest[1] = 0x00;
	ee_ReadBytes(EepromTest, 0x00, 0x02);
	
	//测试
	MB85Ctrl_Write(0x00, FramTest, 0x02);
	bsp_DelayMS(1);
	FramTest[0] = 0x00;
	FramTest[1] = 0x00;
	MB85Ctrl_Read(0x00, FramTest, 0x02);
	
	bsp_FanInit();
	bsp_TempInit();
	
	//测试
	AddrVal = bsp_AddrRead();
	
	HAL_TIM_Base_Start_IT(&htim10);
	HAL_TIM_Base_Start_IT(&htim11);
//	HAL_UART_Receive_IT(&huart1, &RxData, 1);

    /* 内核开启前关闭HAL的时间基准 */
    HAL_SuspendTick();

    /* 内核初始化 */
    osKernelInitialize();

    /* 创建启动任务 */
    ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);

    /* 开启多任务 */
    osKernelStart();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

