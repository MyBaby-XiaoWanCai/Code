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
#include "adc.h"
#include "can.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_dwt.h"
#include "bsp_eeprom.h"
#include "bsp_gpio.h"
#include "bsp_charge.h"
#include "string.h"
#include "wizchip_conf.h"
#include "w5500.h"
#include "w5500socket.h"
#include "stdio.h"
#include "bsp_tcp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
typedef  void (*pFunction)(void);
typedef struct
{
    uint8_t g_ucDatBuff[1000];
    uint16_t rcv_len;
} rcv_Struct;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
wiz_NetInfo NetConf =                                       /*W5500网络配置*/
{
    {0x00, 0x28, 0xf4, 0x00, 0x00, 0x00}, // mac默认地址,后3位表示2021年10月生产，流水号0
    {192, 168, 1, 11},                    // 本地IP地址
    {255, 255, 0, 0},                     // 子网掩码
    {192, 168, 1, 1},                     // 网关地址
    {0, 0, 0, 0},                         // DNS服务器地址
    NETINFO_STATIC                        // 使用静态IP
};

uint8_t txbuf_size[8] = {16, 0, 0, 0, 0, 0, 0, 0};    // W5500发送缓存区分配数组，有效分配值范围：{1,2,4,8,16}
uint8_t rxbuf_size[8] = {16, 0, 0, 0, 0, 0, 0, 0};    // w5500接受缓存区分配数组
uint8_t MAC_ID[3]={0}; //MAC条码信息位，前10bit表示年月（2021-10起始，按月累加），后14bit表示流水号

pFunction Jump_To_Application;
uint32_t JumpAddress;
rcv_Struct w5500_rcvbuf;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/*
*********************************************************************************************************
*   函 数 名: W5500_init
*   功能说明: W5500初始化函数
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void W5500_init(void)
{
    /*配置接口注册函数*/
    reg_wizchip_cris_cbfunc(NULL, NULL);     //不注册自定义临界进出函数，使用默认函数，功能已改写
    reg_wizchip_cs_cbfunc(NULL, NULL);       //不注册自定义片选函数，使用默认函数，功能已改写
    reg_wizchip_spi_cbfunc(NULL, NULL);       //不注册自定义SPI单字节收发函数，使用默认函数，功能已改写,使用SPI1
    //reg_wizchip_spiburst_cbfunc(NULL,NULL);    //不注册自定义SPI块字节收发函数，使用默认函数，功能已改写
    /*检查网线链接*/
    uint8_t PHY_connect = 0; //网线连接状态
    uint16_t CNT = 0;
    while(PHY_connect == 0)
    {
        PHY_connect = 0x01 & IINCHIP_READ(PHYCFGR);
        HAL_Delay(1);
        Feed_WDG();       //喂狗
        if(++CNT > 5000) //尝试5000次(5S)，若网线未连接跳出
        {
            //           printf("网线未连接!\n\n");
            break;
        }
    }

    /*硬件复位*/
    //HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
    //HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_RESET);
    //bsp_DelayMS(1);
    //HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);

    /*软件复位（复位内部寄存器）*/
    wizchip_sw_reset();
    bsp_DelayMS(1);
    /*配置MAC，IP*/
    wiz_NetInfo conf;                                               // 用于存储回读网络参数
    ctlnetwork(CN_SET_NETINFO, (void *) &NetConf);                  // 配置网络参数
    ctlnetwork(CN_GET_NETINFO, (void *) &conf);                     // 回读
    if(memcmp(&conf, &NetConf, sizeof(wiz_NetInfo)) == 0)
    {
        //        printf("W5500网络参数配置成功!\n\n");      // 配置成功
    }
    else
    {
        //        printf("W5500网络参数配置失败!\n\n");      // 配置失败
    }
    /*设置socket收发缓存区大小*/
    wizchip_init(txbuf_size,  rxbuf_size);
    /*设置最大重发次数，超时时间*/
    wiz_NetTimeout to;
    to.retry_cnt = 8;     // 重试次数，默认8次
    to.time_100us = 2000; // 超时时间，默认2000*100us = 200ms
    wizchip_settimeout(&to);
}

/*
*********************************************************************************************************
*   函 数 名: Boot_JMP_APP
*   功能说明: 跳转到APP
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void Boot_JMP_APP(void)
{
    __set_PRIMASK(1)    /* 禁止全局中断 */;
    if(((*(__IO uint32_t *)APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000) //地址是否合法
    {
        /* Jump to user application */
        JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS + 4);
        Jump_To_Application = (pFunction) JumpAddress;
        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t *) APPLICATION_ADDRESS);
        Jump_To_Application();
    }
}

/*
*********************************************************************************************************
*   函 数 名: Loop_En_Init
*   功能说明: 初始化LOOP和EN脚
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void Loop_En_Init(void)
{
    HAL_GPIO_WritePin(M_Loop1_GPIO_Port, M_Loop1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M_Loop2_GPIO_Port, M_Loop2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M_Loop3_GPIO_Port, M_Loop3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M_Loop4_GPIO_Port, M_Loop4_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M_Loop5_GPIO_Port, M_Loop5_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M_Loop6_GPIO_Port, M_Loop6_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M_Loop7_GPIO_Port, M_Loop7_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M_Loop8_GPIO_Port, M_Loop8_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(MOD_EN1_GPIO_Port, MOD_EN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOD_EN2_GPIO_Port, MOD_EN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOD_EN3_GPIO_Port, MOD_EN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOD_EN4_GPIO_Port, MOD_EN4_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOD_EN5_GPIO_Port, MOD_EN5_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOD_EN6_GPIO_Port, MOD_EN6_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOD_EN7_GPIO_Port, MOD_EN7_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOD_EN8_GPIO_Port, MOD_EN8_Pin, GPIO_PIN_RESET);
}

/**********************************************************************************************************
**  函 数 名 : BarCode_Mac_CheckSet()
**  功能说明 : 条码Mac关联一致性检查设置函数，需要操作EE
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
uint8_t BarCode_Mac_CheckSet(void)
{
    uint8_t Read_En = 0;
    uint8_t Bar_Code[50] = {0};
    uint8_t Bar_Code_Len = 0;
    uint8_t Pyear, Pmonth; //生产年月
    uint16_t Pid; //流水号
    uint16_t Pym; //年月编码
    uint32_t Pmac = 0; //mac编号(低3字节有效)
    uint8_t err = 0;

    ee_ReadBytes(&Read_En, MacEn_flag_addr , 1);    //读EE，是否使能使用EE内部MAC
    if(Read_En == 1)
    {
        ee_ReadBytes(MAC_ID, MacID_flag_addr , 3);       //读MAC
        ee_ReadBytes(&Bar_Code_Len, BarCode_Len_flag_addr , 1);  //读条码长度
        if(Bar_Code_Len < 50)                       //条码字符串长度限制50字节
        {
            ee_ReadBytes(Bar_Code, BarCode_End_flag_addr - Bar_Code_Len + 1, Bar_Code_Len);  //根据长度读条码内容

            /*检查数据有效性*/
            if((Bar_Code[0] == 'J') && (Bar_Code[1] == 'N'))            //判读条码头, JN开头
            {
                for(uint8_t i = 0; i < 8; i++)
                {
                    if(Bar_Code[4 + i] > 0x39 || Bar_Code[4 + i] < 0x30) //判断要处理位(年月流水)是否为数字ASC码
                    {
                        err = 1;
                        break;
                    }
                }
            }
            else
            {
                err = 1;
            }
            /*检查关联MAC处理*/
            if(err == 0)
            {
                /*计算年月流水*/
                Pyear = (Bar_Code[4] - 0x30) * 10 + (Bar_Code[5] - 0x30);
                Pmonth = (Bar_Code[6] - 0x30) * 10 + (Bar_Code[7] - 0x30);
                if((Pyear < 21) || (Pmonth > 12) || ((Pyear == 21) && (Pmonth < 10))) //再次判断年月有效范围
                {
                    err = 1;
                }
                if(err == 0)
                {
                    Pid = (Bar_Code[8] - 0x30) * 1000 + (Bar_Code[9] - 0x30) * 100 + (Bar_Code[10] - 0x30) * 10 + (Bar_Code[11] - 0x30); //求流水号
                    Pym = (Pyear == 21) ? (Pmonth - 10) : ((Pyear - 22) * 12 + Pmonth + 2); //求年月编号
                    Pmac = (MAC_ID[0] << 16) + (MAC_ID[1] << 8) + MAC_ID[2]; //求MAC编号
                    if(Pmac != (Pym << 14) + Pid)
                    {
                        err = 1;
                    }
                }
            }
        }
        else
        {
            err = 1;
        }

        if(err)
        {
            MAC_ID[0] = 0;  //使用默认MAC
            MAC_ID[1] = 0;
            MAC_ID[2] = 0;
        }
    }
    return err;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    uint8_t Bootflg1;      //boot标志位1
    uint8_t Bootflg2;      //boot标志位2
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
  MX_SPI1_Init();
  MX_IWDG_Init();
  MX_TIM4_Init();
  MX_ADC3_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_SPI2_Init();
  MX_SPI3_Init();
  MX_SPI4_Init();
  MX_SPI5_Init();
  MX_SPI6_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
    bsp_InitDWT();
    Loop_En_Init();                  //初始化LOOP，EN脚电平
    __HAL_SPI_ENABLE(&hspi1);        //使能SPI1
    __HAL_TIM_ENABLE(&htim4);        //使能TIM4
    bsp_InitI2C();                   //初始化EE接口

    IP_Scanning();                   //扫描拨码，获取本地IP后2位
    NetConf.ip[2] = W5500_IP_ADDRESS[2];
    NetConf.ip[3] = W5500_IP_ADDRESS[3];
    
#if 1
    BarCode_Mac_CheckSet(); //检查条码MAC一致性，设置MAC
    NetConf.mac[3]=MAC_ID[0];
    NetConf.mac[4]=MAC_ID[1];
    NetConf.mac[5]=MAC_ID[2]; 
#else
    NetConf.mac[2]=NetConf.ip[0];    //mac后4位用IP表示
    NetConf.mac[3]=NetConf.ip[1];
    NetConf.mac[4]=NetConf.ip[2];
    NetConf.mac[5]=NetConf.ip[3]; 
#endif    
    W5500_init();                    //W5500初始化

    HAL_TIM_Base_Start_IT(&htim4);   //使用定时器的时候调用这个函数启动
    HAL_NVIC_EnableIRQ(TIM4_IRQn);


    /*APP跳转判断逻辑*/
    Feed_WDG();
    ee_ReadBytes(&Bootflg1, BOOT1_addr, 1);
    ee_ReadBytes(&Bootflg2, BOOT2_addr , 1);

#if 1

    if(((Bootflg1 == 0xAA) && (Bootflg2 == 0x55)) || \
            ((Bootflg1 == 0xFF) && (Bootflg2 == 0xFF)))
    {
        Feed_WDG();
        HAL_NVIC_DisableIRQ(EXTI0_IRQn);
        HAL_NVIC_DisableIRQ(EXTI1_IRQn);
        HAL_NVIC_DisableIRQ(TIM4_IRQn);     //跳转前关单个中断，防止APP未使用该中断时再次开启总中断后找不到入口
        DISABLE_INT();                      //关总中断
        Boot_JMP_APP();
    }
    else
        if((Bootflg1 == 0xA0) && (Bootflg2 == 0x50))
        {
            Bootflg1 = 0xff;
            Bootflg2 = 0xff;
            ee_WriteBytes(&Bootflg1, BOOT1_addr, 1);
            bsp_DelayMS(1);
            ee_WriteBytes(&Bootflg2, BOOT2_addr, 1);
            bsp_DelayMS(1);
            Feed_WDG();
            HAL_NVIC_DisableIRQ(EXTI0_IRQn);
            HAL_NVIC_DisableIRQ(EXTI1_IRQn);
            HAL_NVIC_DisableIRQ(TIM4_IRQn);   //跳转前关单个中断，防止APP未使用该中断时再次开启总中断后找不到入口
            DISABLE_INT();                    //关总中断
            Boot_JMP_APP();
        }

#endif
    //HAL_GPIO_WritePin(EN_245D_GPIO_Port, EN_245D_Pin, GPIO_PIN_RESET); //使能245D（仅G版硬件需要，H版不需要）


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while(1)
    {
        uint16_t Loop_cnt;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        W5500_TCP_Server();
        Socket_Receive();
        TCP_RX_Data_Analysis();
        Feed_WDG();       //喂狗
        HAL_Delay(1);
        Loop_cnt++;
        if(Loop_cnt >= 500)
        {
            Loop_cnt = 0;
            HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
            HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
        }
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
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7) {
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
    while(1)
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
