/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "cc.h"
#include "lwip/lwip_sys.h"
#include "eth_process.h"


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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void IDWG_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
    uint16_t led_control = 0xffff;

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
    MX_ADC3_Init();
    MX_SPI1_Init();
    MX_SPI2_Init();
    MX_SPI3_Init();
    MX_SPI4_Init();
    MX_SPI5_Init();
    MX_SPI6_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
    // MX_TIM7_Init();
    MX_USART3_UART_Init();

    /* USER CODE BEGIN 2 */
    InitI2C();
    InitFLASH();
    ADC_Switch_ConfigGPIO();
    CAN_User_Init(&hcan1);
    if(HAL_UART_Receive_IT(&huart3,(uint8_t*)RxBuffer,1)!=HAL_OK) {Error_Handler();}//开启接收中断，准备接收
    InitAD7175();
    InitAD7124();
    InitDAC80508();
    InitI2C_pca9555();
    Discharge_Init();
    Task_Add();

    //Boot 相关操作---------------------------------------------------------------------------------
    DISABLE_INT();
    SCB->VTOR = FLASH_BASE | 0x20000;           //基址偏移
    ENABLE_INT();
    Boot_Flag=EEPROM_ReadLenByte(Boot_Flag_addr-1,2);
    if((Boot_Flag==0x50A0)||(Boot_Flag==0xFFFF))
    {
        EEPROM_WriteOneByte(Boot_Flag_addr-1,0xAA);
        EEPROM_WriteOneByte(Boot_Flag_addr,0x55);
        __set_PRIMASK(1);//关中断
        __set_FAULTMASK(1);//关闭中断异常
        NVIC_SystemReset();
    }
    //----------------------------------------------------------------------------------------------

    HAL_TIM_Base_Start_IT(&htim1);      //启动定时器1
    // HAL_TIM_Base_Stop_IT(&htim1);    //关闭定时器1
    HAL_TIM_Base_Start_IT(&htim2);      //启动定时器2

    SYS_MODE = SYS_NORMAL_MODE;         //设系统模式为正常模式
    for(int i = 0; i < CHANNELS; i++) { //设各通道为待机模式
        running_step[i].status = step_standby_status;
    }
    led_control = NORMAL_LED^(RUN_LED_G);
    pca9555SetOutput(led_control);      //设置运行灯位绿色
    ACDC_Fan_Speed_CMD(0xff, 20);       //设置风扇速度

    // SPI_Flash_Erase_Chip();              //清除外部 Flash 信息，即清除存储的工步信息
    // EEPROM_WriteLenByte(0, 0xFFFF, 2);   //清除 EEPROM 的标志位信息
    // for(int i = 0; i < 36*8; i++) {      //清除校准参数
    //     EEPROM_WriteLenByte(2+i*4, 0xFFFFFFFF, 4);
    // }

    Calibration_Write();                //写入默认校准参数到 EEPROM
    Calibration_Read();                 //读取校准参数
    /* USER CODE END 2 */


    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    ip_scanning();                      //读取拨码开关信号，设置 IP 地址

    if((IP_ADDRESS[2] == 8) && (IP_ADDRESS[3] == 42)) {
        Self_Calibration();             //进行老化自检及老化
    } else {
    #if w5500_EN
        InitW5500_BSP();
    #else
        // LwIP_Init();                    //lwip 初始化
        // TCP_Server_Init();              //tcp初始化
        ETH_KSZ8041_Init();
        while(1) {
            uint32_t temp32 = 0x00;
            temp32 = KSZ8041_ReadPHY(0x01);

            if(temp32 & 0x04) {
                LwIP_Init();                    //lwip 初始化
                TCP_Server_Init();              //tcp初始化

                break;
            }
        }
    #endif
    }

    HAL_Delay(5000);
    led_control = NORMAL_LED^(RUN_LED_G+CH1_LED_G+CH2_LED_G+CH3_LED_G);
    pca9555SetOutput(led_control);
    CH4_LED_G = 1;

    //判断是否发生独立看门狗复位
    if(RCC->CSR & RCC_CSR_IWDGRSTF) {
        GLOBAL_PROT_FLAG[0] = 0x01;
        Global_Protection_Flag[0][watch_dog_trigger] = 0x01;
        RCC->CSR |= RCC_CSR_RMVF;
    } else {
        Global_Protection_Flag[0][watch_dog_trigger] = 0x00;
    }
    IDWG_Init();

    while (1)
    {
        Receive_Data_Analysis();
        Boot_Process();

        if(Calibration_Read_Flag == 1) {
            Calibration_Read();
            Calibration_Read_Flag = 0;
        }

        if(SYS_MODE == SYS_NORMAL_MODE) {
            Step_Run_Analysis();
        } else if(SYS_MODE == SYS_CALIBRATION_MODE) {
            Calibration_Run();
        }

        Task_RUN();

        if(SYS_MODE == SYS_NORMAL_MODE) {
            //判断记录数据上传条件，满足条件则上传记录数据----------------------------------------------
            for(int i = 0; i < CHANNELS; i++) {
                if((running_step[i].status == step_running_status) || (running_step[i].status == step_contact_status)) {
                    this_record_condition[i].record_current = record_data.channel_record_data[i].output_current;
                    this_record_condition[i].record_voltage = record_data.channel_record_data[i].battery_voltage;
                    this_record_condition[i].record_time    = running_step[i].run_time;

                    if( ((FLOAT_NOT_ZERO(step_record_condition[i].record_current) > 0) && (fabs(this_record_condition[i].record_current - last_record_condition[i].record_current) >= step_record_condition[i].record_current)) ||
                        ((FLOAT_NOT_ZERO(step_record_condition[i].record_voltage) > 0) && (fabs(this_record_condition[i].record_voltage - last_record_condition[i].record_voltage) >= step_record_condition[i].record_voltage)) ||
                        ((FLOAT_NOT_ZERO(step_record_condition[i].record_time) > 0) && ((this_record_condition[i].record_time - last_record_condition[i].record_time) >= (step_record_condition[i].record_time*1000))) ) {

                        if(TCP_SERVER_FLAG & 0x20) {
                            Record_Report(0x01 << i);
                        }

                        last_record_condition[i].record_current = this_record_condition[i].record_current;
                        last_record_condition[i].record_voltage = this_record_condition[i].record_voltage;
                        last_record_condition[i].record_time = this_record_condition[i].record_time;

                        //波动保护数据记录------------------------------------------------------------
                        fluctuation_protection_now[i].voltage = fabs(recode_query[i].battery_voltage);
                        // fluctuation_protection_now[i].voltage_time = recode_query[i].run_time;
                        fluctuation_protection_now[i].voltage_time = running_step[i].protection_time;
                        fluctuation_protection_now[i].current = fabs(recode_query[i].output_current);
                        // fluctuation_protection_now[i].current_time = recode_query[i].run_time;
                        fluctuation_protection_now[i].current_time = running_step[i].protection_time;
                        //--------------------------------------------------------------------------

                        //跳变保护数据记录-----------------------------------------------------------
                        if(running_step[i].protection_time <= 1000) {
                            jump_protection_old[i].current = fabs(recode_query[i].output_current);
                            jump_protection_old[i].voltage = fabs(recode_query[i].battery_voltage);
                        } else {
                            jump_protection_old[i].current = jump_protection_now[i].current;
                            jump_protection_old[i].voltage = jump_protection_now[i].voltage;
                        }

                        jump_protection_now[i].current = fabs(recode_query[i].output_current);
                        jump_protection_now[i].voltage = fabs(recode_query[i].battery_voltage);

                        jump_flag[i][0] = 1;
                        jump_flag[i][1] = 1;
                        jump_flag[i][2] = 1;
                        jump_flag[i][3] = 1;
                        //--------------------------------------------------------------------------

                    }
                }

                // if(QUERY_PROTECTION_FLAG & (0x01<<i)) {
                //     if(STEP_PROT_FLAG[i] != 0x00) {
                //         Step_Protection_Report(0x01<<i);
                //     }
                //     if(GLOBAL_PROT_FLAG[i] != 0x00) {
                //         Global_Protection_Report(0x01<<i);
                //     }

                //     QUERY_PROTECTION_FLAG &= ~(0x01<<i);
                // }
            }
            //--------------------------------------------------------------------------------------

            //判断工步运行截止条件，满足条件则切换工步--------------------------------------------------
            Running_Switch();
            //--------------------------------------------------------------------------------------


        }


    #if w5500_EN
        W5500_Service_BSP2();
        Socket_Receive();
    #else
        __disable_irq();
        sys_check_timeouts();       //处理 lwip 中定时事件
        __enable_irq();
    #endif
    }
    /* USER CODE END WHILE */


    /* USER CODE BEGIN 3 */

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

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
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

  /** Initializes the CPU, AHB and APB busses clocks
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

//为LWIP提供计时
u32_t sys_now(void)
{
    return HAL_GetTick();
}

//独立看门狗初始化
void IDWG_Init(void)
{
    IWDG->KR = 0X5555;      //使能对 IWDG->PR 和 IWDG->RLR 的写
    IWDG->PR = 4;           //设置分频系数
    IWDG->RLR = 500;        //重加载寄存器 IWDG->RLR
    IWDG->KR = 0XAAAA;      //reload
    IWDG->KR = 0XCCCC;      //使能看门狗
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

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
        tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
