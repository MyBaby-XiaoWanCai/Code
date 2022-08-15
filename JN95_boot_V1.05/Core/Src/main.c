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
**                                           ���Ͷ�����
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
**                                         ȫ�ֱ���������
**********************************************************************************************************/
wiz_NetInfo NetConf =                                       /*W5500��������*/
{
    {0x00, 0x28, 0xf4, 0x00, 0x00, 0x00}, // macĬ�ϵ�ַ,��3λ��ʾ2021��10����������ˮ��0
    {192, 168, 1, 11},                    // ����IP��ַ
    {255, 255, 0, 0},                     // ��������
    {192, 168, 1, 1},                     // ���ص�ַ
    {0, 0, 0, 0},                         // DNS��������ַ
    NETINFO_STATIC                        // ʹ�þ�̬IP
};

uint8_t txbuf_size[8] = {16, 0, 0, 0, 0, 0, 0, 0};    // W5500���ͻ������������飬��Ч����ֵ��Χ��{1,2,4,8,16}
uint8_t rxbuf_size[8] = {16, 0, 0, 0, 0, 0, 0, 0};    // w5500���ܻ�������������
uint8_t MAC_ID[3]={0}; //MAC������Ϣλ��ǰ10bit��ʾ���£�2021-10��ʼ�������ۼӣ�����14bit��ʾ��ˮ��

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
**                                           Ӧ�ú�����
**********************************************************************************************************/

/*
*********************************************************************************************************
*   �� �� ��: W5500_init
*   ����˵��: W5500��ʼ������
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void W5500_init(void)
{
    /*���ýӿ�ע�ắ��*/
    reg_wizchip_cris_cbfunc(NULL, NULL);     //��ע���Զ����ٽ����������ʹ��Ĭ�Ϻ����������Ѹ�д
    reg_wizchip_cs_cbfunc(NULL, NULL);       //��ע���Զ���Ƭѡ������ʹ��Ĭ�Ϻ����������Ѹ�д
    reg_wizchip_spi_cbfunc(NULL, NULL);       //��ע���Զ���SPI���ֽ��շ�������ʹ��Ĭ�Ϻ����������Ѹ�д,ʹ��SPI1
    //reg_wizchip_spiburst_cbfunc(NULL,NULL);    //��ע���Զ���SPI���ֽ��շ�������ʹ��Ĭ�Ϻ����������Ѹ�д
    /*�����������*/
    uint8_t PHY_connect = 0; //��������״̬
    uint16_t CNT = 0;
    while(PHY_connect == 0)
    {
        PHY_connect = 0x01 & IINCHIP_READ(PHYCFGR);
        HAL_Delay(1);
        Feed_WDG();       //ι��
        if(++CNT > 5000) //����5000��(5S)��������δ��������
        {
            //           printf("����δ����!\n\n");
            break;
        }
    }

    /*Ӳ����λ*/
    //HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
    //HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_RESET);
    //bsp_DelayMS(1);
    //HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);

    /*�����λ����λ�ڲ��Ĵ�����*/
    wizchip_sw_reset();
    bsp_DelayMS(1);
    /*����MAC��IP*/
    wiz_NetInfo conf;                                               // ���ڴ洢�ض��������
    ctlnetwork(CN_SET_NETINFO, (void *) &NetConf);                  // �����������
    ctlnetwork(CN_GET_NETINFO, (void *) &conf);                     // �ض�
    if(memcmp(&conf, &NetConf, sizeof(wiz_NetInfo)) == 0)
    {
        //        printf("W5500����������óɹ�!\n\n");      // ���óɹ�
    }
    else
    {
        //        printf("W5500�����������ʧ��!\n\n");      // ����ʧ��
    }
    /*����socket�շ���������С*/
    wizchip_init(txbuf_size,  rxbuf_size);
    /*��������ط���������ʱʱ��*/
    wiz_NetTimeout to;
    to.retry_cnt = 8;     // ���Դ�����Ĭ��8��
    to.time_100us = 2000; // ��ʱʱ�䣬Ĭ��2000*100us = 200ms
    wizchip_settimeout(&to);
}

/*
*********************************************************************************************************
*   �� �� ��: Boot_JMP_APP
*   ����˵��: ��ת��APP
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void Boot_JMP_APP(void)
{
    __set_PRIMASK(1)    /* ��ֹȫ���ж� */;
    if(((*(__IO uint32_t *)APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000) //��ַ�Ƿ�Ϸ�
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
*   �� �� ��: Loop_En_Init
*   ����˵��: ��ʼ��LOOP��EN��
*   ��    ��: ��
*   �� �� ֵ: ��
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
**  �� �� �� : BarCode_Mac_CheckSet()
**  ����˵�� : ����Mac����һ���Լ�����ú�������Ҫ����EE
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
uint8_t BarCode_Mac_CheckSet(void)
{
    uint8_t Read_En = 0;
    uint8_t Bar_Code[50] = {0};
    uint8_t Bar_Code_Len = 0;
    uint8_t Pyear, Pmonth; //��������
    uint16_t Pid; //��ˮ��
    uint16_t Pym; //���±���
    uint32_t Pmac = 0; //mac���(��3�ֽ���Ч)
    uint8_t err = 0;

    ee_ReadBytes(&Read_En, MacEn_flag_addr , 1);    //��EE���Ƿ�ʹ��ʹ��EE�ڲ�MAC
    if(Read_En == 1)
    {
        ee_ReadBytes(MAC_ID, MacID_flag_addr , 3);       //��MAC
        ee_ReadBytes(&Bar_Code_Len, BarCode_Len_flag_addr , 1);  //�����볤��
        if(Bar_Code_Len < 50)                       //�����ַ�����������50�ֽ�
        {
            ee_ReadBytes(Bar_Code, BarCode_End_flag_addr - Bar_Code_Len + 1, Bar_Code_Len);  //���ݳ��ȶ���������

            /*���������Ч��*/
            if((Bar_Code[0] == 'J') && (Bar_Code[1] == 'N'))            //�ж�����ͷ, JN��ͷ
            {
                for(uint8_t i = 0; i < 8; i++)
                {
                    if(Bar_Code[4 + i] > 0x39 || Bar_Code[4 + i] < 0x30) //�ж�Ҫ����λ(������ˮ)�Ƿ�Ϊ����ASC��
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
            /*������MAC����*/
            if(err == 0)
            {
                /*����������ˮ*/
                Pyear = (Bar_Code[4] - 0x30) * 10 + (Bar_Code[5] - 0x30);
                Pmonth = (Bar_Code[6] - 0x30) * 10 + (Bar_Code[7] - 0x30);
                if((Pyear < 21) || (Pmonth > 12) || ((Pyear == 21) && (Pmonth < 10))) //�ٴ��ж�������Ч��Χ
                {
                    err = 1;
                }
                if(err == 0)
                {
                    Pid = (Bar_Code[8] - 0x30) * 1000 + (Bar_Code[9] - 0x30) * 100 + (Bar_Code[10] - 0x30) * 10 + (Bar_Code[11] - 0x30); //����ˮ��
                    Pym = (Pyear == 21) ? (Pmonth - 10) : ((Pyear - 22) * 12 + Pmonth + 2); //�����±��
                    Pmac = (MAC_ID[0] << 16) + (MAC_ID[1] << 8) + MAC_ID[2]; //��MAC���
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
            MAC_ID[0] = 0;  //ʹ��Ĭ��MAC
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
    uint8_t Bootflg1;      //boot��־λ1
    uint8_t Bootflg2;      //boot��־λ2
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
    Loop_En_Init();                  //��ʼ��LOOP��EN�ŵ�ƽ
    __HAL_SPI_ENABLE(&hspi1);        //ʹ��SPI1
    __HAL_TIM_ENABLE(&htim4);        //ʹ��TIM4
    bsp_InitI2C();                   //��ʼ��EE�ӿ�

    IP_Scanning();                   //ɨ�貦�룬��ȡ����IP��2λ
    NetConf.ip[2] = W5500_IP_ADDRESS[2];
    NetConf.ip[3] = W5500_IP_ADDRESS[3];
    
#if 1
    BarCode_Mac_CheckSet(); //�������MACһ���ԣ�����MAC
    NetConf.mac[3]=MAC_ID[0];
    NetConf.mac[4]=MAC_ID[1];
    NetConf.mac[5]=MAC_ID[2]; 
#else
    NetConf.mac[2]=NetConf.ip[0];    //mac��4λ��IP��ʾ
    NetConf.mac[3]=NetConf.ip[1];
    NetConf.mac[4]=NetConf.ip[2];
    NetConf.mac[5]=NetConf.ip[3]; 
#endif    
    W5500_init();                    //W5500��ʼ��

    HAL_TIM_Base_Start_IT(&htim4);   //ʹ�ö�ʱ����ʱ����������������
    HAL_NVIC_EnableIRQ(TIM4_IRQn);


    /*APP��ת�ж��߼�*/
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
        HAL_NVIC_DisableIRQ(TIM4_IRQn);     //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
        DISABLE_INT();                      //�����ж�
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
            HAL_NVIC_DisableIRQ(TIM4_IRQn);   //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
            DISABLE_INT();                    //�����ж�
            Boot_JMP_APP();
        }

#endif
    //HAL_GPIO_WritePin(EN_245D_GPIO_Port, EN_245D_Pin, GPIO_PIN_RESET); //ʹ��245D����G��Ӳ����Ҫ��H�治��Ҫ��


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
        Feed_WDG();       //ι��
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
