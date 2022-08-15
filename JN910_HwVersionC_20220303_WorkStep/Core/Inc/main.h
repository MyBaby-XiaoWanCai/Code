/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* 版本头文件 */
#include "version.h"

/* 系统库头文件 */
#include "cmsis_os2.h"
#include "rtx_os.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

/*全局定义头文件*/
#include "Global.h"

/*任务模块头文件*/
#include "Task_Start.h"
#include "Task_Dcdc.h"
#include "Task_Debug.h"

/*驱动模块头文件*/
#include "bsp_MB85RS2.h"
#include "bsp_dwt.h"
#include "bsp_ads131.h"
#include "bsp_can.h"
#include "bsp_dac80501.h"
#include "bsp_fan.h"
#include "bsp_temp.h"
#include "bsp_rng.h"
#include "bsp_charge.h"
#include "bsp_i2c.h"
#include "bsp_pca9555.h"
#include "bsp_led.h"
#include "bsp_AddrErr.h"
#include "bsp_eeprom.h"
#include "bsp_debug.h"

/* 应用层头文件 */
#include "Task_Protocol.h"
#include "Task_Workstep.h"
#include "Task_Protection.h"
#include "AppFunc_Eeprom.h"
#include "Task_Led.h"
#include "Task_Dcdc.h"

/* CAN协议头文件 */
#include "Can_Network_Global.h"
#include "Can_Network_Driver.h"
#include "Can_Network_Protocol.h"
#include "Can_Network_Transplant.h"
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
#define STM32_DAC_CS3_Pin GPIO_PIN_2
#define STM32_DAC_CS3_GPIO_Port GPIOE
#define STM32_DAC_CS4_Pin GPIO_PIN_3
#define STM32_DAC_CS4_GPIO_Port GPIOE
#define STM32_DAC_CS5_Pin GPIO_PIN_4
#define STM32_DAC_CS5_GPIO_Port GPIOE
#define STM32_DAC_CS6_Pin GPIO_PIN_5
#define STM32_DAC_CS6_GPIO_Port GPIOE
#define STM32_DAC_CS7_Pin GPIO_PIN_6
#define STM32_DAC_CS7_GPIO_Port GPIOE
#define V_STM32_ADC_CS8_2_Pin GPIO_PIN_13
#define V_STM32_ADC_CS8_2_GPIO_Port GPIOC
#define I_STM32_ADC_CS1_1_Pin GPIO_PIN_14
#define I_STM32_ADC_CS1_1_GPIO_Port GPIOC
#define I_STM32_ADC_CS2_1_Pin GPIO_PIN_15
#define I_STM32_ADC_CS2_1_GPIO_Port GPIOC
#define I2C2_SDA_Pin GPIO_PIN_0
#define I2C2_SDA_GPIO_Port GPIOF
#define I2C2_SCL_Pin GPIO_PIN_1
#define I2C2_SCL_GPIO_Port GPIOF
#define STM32_SSwA_P_1_Pin GPIO_PIN_2
#define STM32_SSwA_P_1_GPIO_Port GPIOF
#define STM32_SSwA_P_2_Pin GPIO_PIN_3
#define STM32_SSwA_P_2_GPIO_Port GPIOF
#define STM32_SSwA_P_3_Pin GPIO_PIN_4
#define STM32_SSwA_P_3_GPIO_Port GPIOF
#define STM32_SSwA_P_4_Pin GPIO_PIN_5
#define STM32_SSwA_P_4_GPIO_Port GPIOF
#define STM32_SSwA_P_5_Pin GPIO_PIN_6
#define STM32_SSwA_P_5_GPIO_Port GPIOF
#define STM32_SSwA_P_6_Pin GPIO_PIN_7
#define STM32_SSwA_P_6_GPIO_Port GPIOF
#define V_STM32_ADC_DRDY5_1_Pin GPIO_PIN_8
#define V_STM32_ADC_DRDY5_1_GPIO_Port GPIOF
#define V_STM32_ADC_DRDY5_1_EXTI_IRQn EXTI9_5_IRQn
#define I_STM32_ADC_DRDY5_2_Pin GPIO_PIN_9
#define I_STM32_ADC_DRDY5_2_GPIO_Port GPIOF
#define I_STM32_ADC_DRDY5_2_EXTI_IRQn EXTI9_5_IRQn
#define STM32_SSwA_P_7_Pin GPIO_PIN_10
#define STM32_SSwA_P_7_GPIO_Port GPIOF
#define TEMP1_Pin GPIO_PIN_0
#define TEMP1_GPIO_Port GPIOC
#define TEMP2_Pin GPIO_PIN_1
#define TEMP2_GPIO_Port GPIOC
#define TEMP7_Pin GPIO_PIN_2
#define TEMP7_GPIO_Port GPIOC
#define TEMP8_Pin GPIO_PIN_3
#define TEMP8_GPIO_Port GPIOC
#define TEMP3_Pin GPIO_PIN_0
#define TEMP3_GPIO_Port GPIOA
#define TEMP4_Pin GPIO_PIN_1
#define TEMP4_GPIO_Port GPIOA
#define TEMP5_Pin GPIO_PIN_2
#define TEMP5_GPIO_Port GPIOA
#define TEMP6_Pin GPIO_PIN_3
#define TEMP6_GPIO_Port GPIOA
#define AD_Temp_Pin GPIO_PIN_4
#define AD_Temp_GPIO_Port GPIOA
#define Fan_PWM_Pin GPIO_PIN_5
#define Fan_PWM_GPIO_Port GPIOA
#define V_STM32_ADC_DRDY4_1_Pin GPIO_PIN_6
#define V_STM32_ADC_DRDY4_1_GPIO_Port GPIOA
#define V_STM32_ADC_DRDY4_1_EXTI_IRQn EXTI9_5_IRQn
#define I_STM32_ADC_DRDY4_2_Pin GPIO_PIN_7
#define I_STM32_ADC_DRDY4_2_GPIO_Port GPIOA
#define I_STM32_ADC_DRDY4_2_EXTI_IRQn EXTI9_5_IRQn
#define V_STM32_ADC_DRDY3_1_Pin GPIO_PIN_4
#define V_STM32_ADC_DRDY3_1_GPIO_Port GPIOC
#define V_STM32_ADC_DRDY3_1_EXTI_IRQn EXTI4_IRQn
#define I_STM32_ADC_DRDY3_2_Pin GPIO_PIN_5
#define I_STM32_ADC_DRDY3_2_GPIO_Port GPIOC
#define I_STM32_ADC_DRDY3_2_EXTI_IRQn EXTI9_5_IRQn
#define V_STM32_ADC_DRDY1_1_Pin GPIO_PIN_0
#define V_STM32_ADC_DRDY1_1_GPIO_Port GPIOB
#define V_STM32_ADC_DRDY1_1_EXTI_IRQn EXTI0_IRQn
#define I_STM32_ADC_DRDY1_2_Pin GPIO_PIN_1
#define I_STM32_ADC_DRDY1_2_GPIO_Port GPIOB
#define I_STM32_ADC_DRDY1_2_EXTI_IRQn EXTI1_IRQn
#define STM32_SSwA_P_8_Pin GPIO_PIN_11
#define STM32_SSwA_P_8_GPIO_Port GPIOF
#define STM32_SSwA_N_1_Pin GPIO_PIN_12
#define STM32_SSwA_N_1_GPIO_Port GPIOF
#define STM32_SSwA_N_2_Pin GPIO_PIN_13
#define STM32_SSwA_N_2_GPIO_Port GPIOF
#define STM32_SSwA_N_3_Pin GPIO_PIN_14
#define STM32_SSwA_N_3_GPIO_Port GPIOF
#define STM32_SSwA_N_4_Pin GPIO_PIN_15
#define STM32_SSwA_N_4_GPIO_Port GPIOF
#define STM32_N_1_Pin GPIO_PIN_0
#define STM32_N_1_GPIO_Port GPIOG
#define STM32_N_2_Pin GPIO_PIN_1
#define STM32_N_2_GPIO_Port GPIOG
#define STM32_DAC_CS8_Pin GPIO_PIN_7
#define STM32_DAC_CS8_GPIO_Port GPIOE
#define FRAM_CS_Pin GPIO_PIN_8
#define FRAM_CS_GPIO_Port GPIOE
#define Fan_error_Pin GPIO_PIN_9
#define Fan_error_GPIO_Port GPIOE
#define V_STM32_ADC_DRDY6_1_Pin GPIO_PIN_10
#define V_STM32_ADC_DRDY6_1_GPIO_Port GPIOE
#define V_STM32_ADC_DRDY6_1_EXTI_IRQn EXTI15_10_IRQn
#define I_STM32_ADC_DRDY6_2_Pin GPIO_PIN_11
#define I_STM32_ADC_DRDY6_2_GPIO_Port GPIOE
#define I_STM32_ADC_DRDY6_2_EXTI_IRQn EXTI15_10_IRQn
#define STM32_ADC_CLK_Pin GPIO_PIN_12
#define STM32_ADC_CLK_GPIO_Port GPIOE
#define STM32_ADC_DOUT_Pin GPIO_PIN_13
#define STM32_ADC_DOUT_GPIO_Port GPIOE
#define STM32_ADC_DIN_Pin GPIO_PIN_14
#define STM32_ADC_DIN_GPIO_Port GPIOE
#define GPIO_LED1_Pin GPIO_PIN_15
#define GPIO_LED1_GPIO_Port GPIOE
#define STM32_SSwA_N_6_Pin GPIO_PIN_10
#define STM32_SSwA_N_6_GPIO_Port GPIOB
#define STM32_SSwA_N_7_Pin GPIO_PIN_11
#define STM32_SSwA_N_7_GPIO_Port GPIOB
#define STM32_SSwA_N_8_Pin GPIO_PIN_12
#define STM32_SSwA_N_8_GPIO_Port GPIOB
#define STM32_DAC_CLK_Pin GPIO_PIN_13
#define STM32_DAC_CLK_GPIO_Port GPIOB
#define V_STM32_ADC_CS5_2_Pin GPIO_PIN_14
#define V_STM32_ADC_CS5_2_GPIO_Port GPIOB
#define STM32_DAC_MOSI_Pin GPIO_PIN_15
#define STM32_DAC_MOSI_GPIO_Port GPIOB
#define STM32_P_2_Pin GPIO_PIN_8
#define STM32_P_2_GPIO_Port GPIOD
#define STM32_P_4_Pin GPIO_PIN_9
#define STM32_P_4_GPIO_Port GPIOD
#define STM32_P_5_Pin GPIO_PIN_10
#define STM32_P_5_GPIO_Port GPIOD
#define STM32_P_6_Pin GPIO_PIN_11
#define STM32_P_6_GPIO_Port GPIOD
#define V_STM32_ADC_DRDY7_1_Pin GPIO_PIN_12
#define V_STM32_ADC_DRDY7_1_GPIO_Port GPIOD
#define V_STM32_ADC_DRDY7_1_EXTI_IRQn EXTI15_10_IRQn
#define I_STM32_ADC_DRDY7_2_Pin GPIO_PIN_13
#define I_STM32_ADC_DRDY7_2_GPIO_Port GPIOD
#define I_STM32_ADC_DRDY7_2_EXTI_IRQn EXTI15_10_IRQn
#define I2C1_SDA_Pin GPIO_PIN_14
#define I2C1_SDA_GPIO_Port GPIOD
#define I2C1_SCL_Pin GPIO_PIN_15
#define I2C1_SCL_GPIO_Port GPIOD
#define V_STM32_ADC_DRDY2_1_Pin GPIO_PIN_2
#define V_STM32_ADC_DRDY2_1_GPIO_Port GPIOG
#define V_STM32_ADC_DRDY2_1_EXTI_IRQn EXTI2_IRQn
#define I_STM32_ADC_DRDY2_2_Pin GPIO_PIN_3
#define I_STM32_ADC_DRDY2_2_GPIO_Port GPIOG
#define I_STM32_ADC_DRDY2_2_EXTI_IRQn EXTI3_IRQn
#define STM32_N_3_Pin GPIO_PIN_4
#define STM32_N_3_GPIO_Port GPIOG
#define STM32_N_4_Pin GPIO_PIN_5
#define STM32_N_4_GPIO_Port GPIOG
#define STM32_N_5_Pin GPIO_PIN_6
#define STM32_N_5_GPIO_Port GPIOG
#define STM32_N_6_Pin GPIO_PIN_7
#define STM32_N_6_GPIO_Port GPIOG
#define STM32_N_7_Pin GPIO_PIN_8
#define STM32_N_7_GPIO_Port GPIOG
#define V_STM32_ADC_CS1_2_Pin GPIO_PIN_6
#define V_STM32_ADC_CS1_2_GPIO_Port GPIOC
#define V_STM32_ADC_CS2_2_Pin GPIO_PIN_7
#define V_STM32_ADC_CS2_2_GPIO_Port GPIOC
#define V_STM32_ADC_CS4_2_Pin GPIO_PIN_8
#define V_STM32_ADC_CS4_2_GPIO_Port GPIOC
#define V_STM32_ADC_CS3_2_Pin GPIO_PIN_9
#define V_STM32_ADC_CS3_2_GPIO_Port GPIOC
#define SNOE1_Pin GPIO_PIN_8
#define SNOE1_GPIO_Port GPIOA
#define STM32_EXTI_INT_Pin GPIO_PIN_15
#define STM32_EXTI_INT_GPIO_Port GPIOA
#define FRAM_SCK_Pin GPIO_PIN_10
#define FRAM_SCK_GPIO_Port GPIOC
#define FRAM_MISO_Pin GPIO_PIN_11
#define FRAM_MISO_GPIO_Port GPIOC
#define FRAM_MOSI_Pin GPIO_PIN_12
#define FRAM_MOSI_GPIO_Port GPIOC
#define I_STM32_ADC_CS3_1_Pin GPIO_PIN_0
#define I_STM32_ADC_CS3_1_GPIO_Port GPIOD
#define I_STM32_ADC_CS4_1_Pin GPIO_PIN_1
#define I_STM32_ADC_CS4_1_GPIO_Port GPIOD
#define I_STM32_ADC_CS5_1_Pin GPIO_PIN_2
#define I_STM32_ADC_CS5_1_GPIO_Port GPIOD
#define I_STM32_ADC_CS6_1_Pin GPIO_PIN_3
#define I_STM32_ADC_CS6_1_GPIO_Port GPIOD
#define I_STM32_ADC_CS7_1_Pin GPIO_PIN_4
#define I_STM32_ADC_CS7_1_GPIO_Port GPIOD
#define I_STM32_ADC_CS8_1_Pin GPIO_PIN_5
#define I_STM32_ADC_CS8_1_GPIO_Port GPIOD
#define STM32_P_3_Pin GPIO_PIN_6
#define STM32_P_3_GPIO_Port GPIOD
#define STM32_P_1_Pin GPIO_PIN_7
#define STM32_P_1_GPIO_Port GPIOD
#define STM32_N_8_Pin GPIO_PIN_9
#define STM32_N_8_GPIO_Port GPIOG
#define V_STM32_ADC_CS6_2_Pin GPIO_PIN_10
#define V_STM32_ADC_CS6_2_GPIO_Port GPIOG
#define V_STM32_ADC_CS7_2_Pin GPIO_PIN_11
#define V_STM32_ADC_CS7_2_GPIO_Port GPIOG
#define LED0_R_STM32_Pin GPIO_PIN_12
#define LED0_R_STM32_GPIO_Port GPIOG
#define LED0_G_STM32_Pin GPIO_PIN_13
#define LED0_G_STM32_GPIO_Port GPIOG
#define V_STM32_ADC_DRDY8_1_Pin GPIO_PIN_14
#define V_STM32_ADC_DRDY8_1_GPIO_Port GPIOG
#define V_STM32_ADC_DRDY8_1_EXTI_IRQn EXTI15_10_IRQn
#define I_STM32_ADC_DRDY8_2_Pin GPIO_PIN_15
#define I_STM32_ADC_DRDY8_2_GPIO_Port GPIOG
#define I_STM32_ADC_DRDY8_2_EXTI_IRQn EXTI15_10_IRQn
#define STM32_P_7_Pin GPIO_PIN_3
#define STM32_P_7_GPIO_Port GPIOB
#define STM32_P_8_Pin GPIO_PIN_4
#define STM32_P_8_GPIO_Port GPIOB
#define I2C3_SDA_Pin GPIO_PIN_7
#define I2C3_SDA_GPIO_Port GPIOB
#define I2C3_SCL_Pin GPIO_PIN_8
#define I2C3_SCL_GPIO_Port GPIOB
#define STM32_SSwA_N_5_Pin GPIO_PIN_9
#define STM32_SSwA_N_5_GPIO_Port GPIOB
#define STM32_DAC_CS1_Pin GPIO_PIN_0
#define STM32_DAC_CS1_GPIO_Port GPIOE
#define STM32_DAC_CS2_Pin GPIO_PIN_1
#define STM32_DAC_CS2_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
