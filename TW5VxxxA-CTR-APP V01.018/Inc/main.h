/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#include "string.h"
#include <math.h>
#include "variable.h"
#include "tim.h"
#include "spi.h"
#include "can.h"
#include "adc.h"
#include "usart.h"
#include "user_adc.h"
#include "ads1256.h"
#include "ad7124_8.h"
#include "ad7175_8.h"
#include "dac80508.h"
#include "eeprom.h"
#include "flash.h"
#include "pca9555.h"
#include "can_process.h"
#include "rs485.h"
#include "task_process.h"
#include "eth_ksz8041.h"
#include "eth_process.h"
#include "data_analysis.h"
#include "step_work.h"
#include "discharge.h"
#include "protection_judgment.h"
#include "crc_check.h"
#include "W5500_BSP.h"
#include "Net_Others.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED81_Pin GPIO_PIN_2
#define LED81_GPIO_Port GPIOE
#define LED82_Pin GPIO_PIN_3
#define LED82_GPIO_Port GPIOE
#define LED83_Pin GPIO_PIN_4
#define LED83_GPIO_Port GPIOE
#define RS485_DE_Pin GPIO_PIN_15
#define RS485_DE_GPIO_Port GPIOB
#define AD7175_RDY_Pin GPIO_PIN_0
#define AD7175_RDY_GPIO_Port GPIOE
#define AD7175_RDY_EXTI_IRQn EXTI0_IRQn
#define AD7124_RDY_Pin GPIO_PIN_1
#define AD7124_RDY_GPIO_Port GPIOE
#define AD7124_RDY_EXTI_IRQn EXTI1_IRQn
/* USER CODE BEGIN Private defines */
#define LED81 PEout(2)
#define LED82 PEout(3)
#define LED83 PEout(4)

#define LED0_G  PAout(5)
#define LED0_R  PAout(6)

#define Mod_PG1 PGin(0)
#define Mod_PG2 PFin(15)
#define Mod_PG3 PFin(14)
#define Mod_PG4 PFin(13)
#define Mod_PG5 PFin(12)
#define Mod_PG6 PFin(11)
#define Mod_PG7 PBin(1)
#define Mod_PG8 PBin(0)

#define ADDR1 PHin(6)
#define ADDR2 PBin(10)
#define ADDR3 PEin(15)
#define ADDR4 PEin(10)
#define ADDR5 PEin(9)
#define ADDR6 PEin(8)
#define ADDR7 PEin(7)
#define ADDR8 PGin(1)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
