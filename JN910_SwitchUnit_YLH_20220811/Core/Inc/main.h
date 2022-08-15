/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* 系统库头文件 */
#include "cmsis_os2.h"
#include "rtx_os.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*全局定义头文件*/
#include "global.h"
#include "spi.h"
#include "iic.h"
#include "can.h"

/*驱动模块头文件*/
#include "Bsp.h"
#include "Bsp_CanLink.h"
#include "Bsp_Demo.h"
#include "Bsp_Dwt.h"
#include "Bsp_MB85RS2.h"
//#include "Bsp_NCA9555.h"
#include "Bsp_SGM58601.h"
#include "Bsp_Timer.h"

/*任务模块头文件*/
#include "Can_Network_Global.h"
#include "Can_Network_Driver.h"
#include "Can_Network_Protocol.h"
#include "Can_Network_Transplant.h"
#include "Task_Dcdc.h"
#include "Task_Init.h"
#include "Task_Led.h"
#include "Task_Protection.h"
#include "Task_Protocol.h"
#include "Task_Start.h"
#include "Task_Workstep.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SBypassA_P3_MCU_Pin GPIO_PIN_2
#define SBypassA_P3_MCU_GPIO_Port GPIOE
#define SBypassA_P4_MCU_Pin GPIO_PIN_3
#define SBypassA_P4_MCU_GPIO_Port GPIOE
#define SBypassA_P5_MCU_Pin GPIO_PIN_4
#define SBypassA_P5_MCU_GPIO_Port GPIOE
#define SBypassA_P6_MCU_Pin GPIO_PIN_5
#define SBypassA_P6_MCU_GPIO_Port GPIOE
#define SBypassA_P7_MCU_Pin GPIO_PIN_6
#define SBypassA_P7_MCU_GPIO_Port GPIOE
#define SSwA_N3_MCU_Pin GPIO_PIN_13
#define SSwA_N3_MCU_GPIO_Port GPIOC
#define SSwA_N4_MCU_Pin GPIO_PIN_14
#define SSwA_N4_MCU_GPIO_Port GPIOC
#define SSwA_N5_MCU_Pin GPIO_PIN_15
#define SSwA_N5_MCU_GPIO_Port GPIOC

#define I2C2_SDA_INTPUT_Pin GPIO_PIN_0
#define I2C2_SDA_INTPUT_GPIO_Port GPIOF
#define I2C2_SCL_INTPUT_Pin GPIO_PIN_1
#define I2C2_SCL_INTPUT_GPIO_Port GPIOF

#define ADC1_RESET_Pin GPIO_PIN_4
#define ADC1_RESET_GPIO_Port GPIOF
#define ADC2_RESET_Pin GPIO_PIN_5
#define ADC2_RESET_GPIO_Port GPIOF
#define ADC3_RESET_Pin GPIO_PIN_6
#define ADC3_RESET_GPIO_Port GPIOF
#define INT_OTP_Pin GPIO_PIN_7
#define INT_OTP_GPIO_Port GPIOF
#define INT_OTP_EXTI_IRQn EXTI9_5_IRQn
#define INT_OCP_OVP_Pin GPIO_PIN_8
#define INT_OCP_OVP_GPIO_Port GPIOF
#define INT_OCP_OVP_EXTI_IRQn EXTI9_5_IRQn
#define INT_current_Pin GPIO_PIN_9
#define INT_current_GPIO_Port GPIOF
#define INT_current_EXTI_IRQn EXTI9_5_IRQn
#define ADC3_BUSY_Pin GPIO_PIN_10
#define ADC3_BUSY_GPIO_Port GPIOF
#define ADC3_BUSY_EXTI_IRQn EXTI15_10_IRQn
#define SSwA_P1_MCU_Pin GPIO_PIN_0
#define SSwA_P1_MCU_GPIO_Port GPIOC
#define SSwA_P2_MCU_Pin GPIO_PIN_1
#define SSwA_P2_MCU_GPIO_Port GPIOC
#define SSwA_P3_MCU_Pin GPIO_PIN_2
#define SSwA_P3_MCU_GPIO_Port GPIOC
#define SSwA_P4_MCU_Pin GPIO_PIN_3
#define SSwA_P4_MCU_GPIO_Port GPIOC
#define Fan_PWM_Pin GPIO_PIN_0
#define Fan_PWM_GPIO_Port GPIOA
#define RS485_EN_Pin GPIO_PIN_1
#define RS485_EN_GPIO_Port GPIOA
#define RS485_TX_Pin GPIO_PIN_2
#define RS485_TX_GPIO_Port GPIOA
#define RS485_RX_Pin GPIO_PIN_3
#define RS485_RX_GPIO_Port GPIOA
#define SPI1_NSS_ADC3_Pin GPIO_PIN_4
#define SPI1_NSS_ADC3_GPIO_Port GPIOA
#define SPI1_SCK_ADC3_Pin GPIO_PIN_5
#define SPI1_SCK_ADC3_GPIO_Port GPIOA
#define SPI1_MISO_ADC3_Pin GPIO_PIN_6
#define SPI1_MISO_ADC3_GPIO_Port GPIOA
#define SPI1_MOSI_ADC3_Pin GPIO_PIN_7
#define SPI1_MOSI_ADC3_GPIO_Port GPIOA
#define SSwA_P5_MCU_Pin GPIO_PIN_4
#define SSwA_P5_MCU_GPIO_Port GPIOC
#define SSwA_P6_MCU_Pin GPIO_PIN_5
#define SSwA_P6_MCU_GPIO_Port GPIOC
#define SNOE1_Pin GPIO_PIN_0
#define SNOE1_GPIO_Port GPIOB
#define SNOE2_Pin GPIO_PIN_1
#define SNOE2_GPIO_Port GPIOB
#define SNOE3_Pin GPIO_PIN_2
#define SNOE3_GPIO_Port GPIOB
#define SByoffA_P1_MCU_Pin GPIO_PIN_12
#define SByoffA_P1_MCU_GPIO_Port GPIOF
#define SByoffA_P2_MCU_Pin GPIO_PIN_13
#define SByoffA_P2_MCU_GPIO_Port GPIOF
#define SByoffA_P3_MCU_Pin GPIO_PIN_14
#define SByoffA_P3_MCU_GPIO_Port GPIOF
#define SByoffA_P4_MCU_Pin GPIO_PIN_15
#define SByoffA_P4_MCU_GPIO_Port GPIOF
#define SByoffA_P5_MCU_Pin GPIO_PIN_0
#define SByoffA_P5_MCU_GPIO_Port GPIOG
#define SByoffA_P6_MCU_Pin GPIO_PIN_1
#define SByoffA_P6_MCU_GPIO_Port GPIOG
#define SBypassA_P8_MCU_Pin GPIO_PIN_7
#define SBypassA_P8_MCU_GPIO_Port GPIOE
#define GPIO_LED2_Pin GPIO_PIN_9
#define GPIO_LED2_GPIO_Port GPIOE
#define GPIO_LED1_Pin GPIO_PIN_10
#define GPIO_LED1_GPIO_Port GPIOE
#define SPI4_NSS_ADC1_Pin GPIO_PIN_11
#define SPI4_NSS_ADC1_GPIO_Port GPIOE
#define SPI4_SCK_ADC1_Pin GPIO_PIN_12
#define SPI4_SCK_ADC1_GPIO_Port GPIOE
#define SPI4_MISO_ADC1_Pin GPIO_PIN_13
#define SPI4_MISO_ADC1_GPIO_Port GPIOE
#define SPI4_MOSI_ADC1_Pin GPIO_PIN_14
#define SPI4_MOSI_ADC1_GPIO_Port GPIOE
#define ADC1_BUSY_Pin GPIO_PIN_15
#define ADC1_BUSY_GPIO_Port GPIOE
#define ADC1_BUSY_EXTI_IRQn EXTI15_10_IRQn
#define ADC2_BUSY_Pin GPIO_PIN_11
#define ADC2_BUSY_GPIO_Port GPIOB
#define ADC2_BUSY_EXTI_IRQn EXTI15_10_IRQn
#define SPI2_NSS_ADC2_Pin GPIO_PIN_12
#define SPI2_NSS_ADC2_GPIO_Port GPIOB
#define SPI2_SCK_ADC2_Pin GPIO_PIN_13
#define SPI2_SCK_ADC2_GPIO_Port GPIOB
#define SPI2_MISO_ADC2_Pin GPIO_PIN_14
#define SPI2_MISO_ADC2_GPIO_Port GPIOB
#define SPI2_MOSI_ADC2_Pin GPIO_PIN_15
#define SPI2_MOSI_ADC2_GPIO_Port GPIOB
#define CTR_ON6_MCU_Pin GPIO_PIN_8
#define CTR_ON6_MCU_GPIO_Port GPIOD
#define CTR_ON7_MCU_Pin GPIO_PIN_9
#define CTR_ON7_MCU_GPIO_Port GPIOD
#define CTR_ON8_MCU_Pin GPIO_PIN_10
#define CTR_ON8_MCU_GPIO_Port GPIOD
#define CLRDOG_Pin GPIO_PIN_13
#define CLRDOG_GPIO_Port GPIOD

#define I2C1_SDA_EEPROM_Pin GPIO_PIN_14
#define I2C1_SDA_EEPROM_GPIO_Port GPIOD
#define I2C1_SCL_EEPROM_Pin GPIO_PIN_15
#define I2C1_SCL_EEPROM_GPIO_Port GPIOD

#define SByoffA_P7_MCU_Pin GPIO_PIN_2
#define SByoffA_P7_MCU_GPIO_Port GPIOG
#define SByoffA_P8_MCU_Pin GPIO_PIN_3
#define SByoffA_P8_MCU_GPIO_Port GPIOG
#define DA1_MAX_MCU_Pin GPIO_PIN_4
#define DA1_MAX_MCU_GPIO_Port GPIOG
#define DA2_MAX_MCU_Pin GPIO_PIN_5
#define DA2_MAX_MCU_GPIO_Port GPIOG
#define DA3_MAX_MCU_Pin GPIO_PIN_6
#define DA3_MAX_MCU_GPIO_Port GPIOG
#define DA4_MAX_MCU_Pin GPIO_PIN_7
#define DA4_MAX_MCU_GPIO_Port GPIOG
#define DA5_MAX_MCU_Pin GPIO_PIN_8
#define DA5_MAX_MCU_GPIO_Port GPIOG
#define SSwA_P7_MCU_Pin GPIO_PIN_6
#define SSwA_P7_MCU_GPIO_Port GPIOC
#define SSwA_P8_MCU_Pin GPIO_PIN_7
#define SSwA_P8_MCU_GPIO_Port GPIOC
#define SSwA_N1_MCU_Pin GPIO_PIN_8
#define SSwA_N1_MCU_GPIO_Port GPIOC
#define SSwA_N2_MCU_Pin GPIO_PIN_9
#define SSwA_N2_MCU_GPIO_Port GPIOC

#define SPI3_NSS_FRAM_Pin GPIO_PIN_15
#define SPI3_NSS_FRAM_GPIO_Port GPIOA
#define SPI3_SCK_FRAM_Pin GPIO_PIN_10
#define SPI3_SCK_FRAM_GPIO_Port GPIOC
#define SPI3_MISO_FRAM_Pin GPIO_PIN_11
#define SPI3_MISO_FRAM_GPIO_Port GPIOC
#define SPI3_MOSI_FRAM_Pin GPIO_PIN_12
#define SPI3_MOSI_FRAM_GPIO_Port GPIOC

#define SSwA_N6_MCU_Pin GPIO_PIN_0
#define SSwA_N6_MCU_GPIO_Port GPIOD
#define SSwA_N7_MCU_Pin GPIO_PIN_1
#define SSwA_N7_MCU_GPIO_Port GPIOD
#define SSwA_N8_MCU_Pin GPIO_PIN_2
#define SSwA_N8_MCU_GPIO_Port GPIOD
#define CTR_ON1_MCU_Pin GPIO_PIN_3
#define CTR_ON1_MCU_GPIO_Port GPIOD
#define CTR_ON2_MCU_Pin GPIO_PIN_4
#define CTR_ON2_MCU_GPIO_Port GPIOD
#define CTR_ON3_MCU_Pin GPIO_PIN_5
#define CTR_ON3_MCU_GPIO_Port GPIOD
#define CTR_ON4_MCU_Pin GPIO_PIN_6
#define CTR_ON4_MCU_GPIO_Port GPIOD
#define CTR_ON5_MCU_Pin GPIO_PIN_7
#define CTR_ON5_MCU_GPIO_Port GPIOD
#define DA6_MAX_MCU_Pin GPIO_PIN_9
#define DA6_MAX_MCU_GPIO_Port GPIOG
#define DA7_MAX_MCU_Pin GPIO_PIN_10
#define DA7_MAX_MCU_GPIO_Port GPIOG
#define DA8_MAX_MCU_Pin GPIO_PIN_11
#define DA8_MAX_MCU_GPIO_Port GPIOG
#define Fan1_error_Pin GPIO_PIN_12
#define Fan1_error_GPIO_Port GPIOG
#define Fan2_error_Pin GPIO_PIN_13
#define Fan2_error_GPIO_Port GPIOG
#define Fan3_error_Pin GPIO_PIN_14
#define Fan3_error_GPIO_Port GPIOG
#define Fan4_error_Pin GPIO_PIN_15
#define Fan4_error_GPIO_Port GPIOG

#define I2C3_SDA_LED_Pin GPIO_PIN_7
#define I2C3_SDA_LED_GPIO_Port GPIOB
#define I2C3_SCL_LED_Pin GPIO_PIN_8
#define I2C3_SCL_LED_GPIO_Port GPIOB

#define SBypassA_P1_MCU_Pin GPIO_PIN_0
#define SBypassA_P1_MCU_GPIO_Port GPIOE
#define SBypassA_P2_MCU_Pin GPIO_PIN_1
#define SBypassA_P2_MCU_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
