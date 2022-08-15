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
#define M_Loop4_Pin GPIO_PIN_2
#define M_Loop4_GPIO_Port GPIOE
#define LED2_Pin GPIO_PIN_3
#define LED2_GPIO_Port GPIOE
#define LED3_Pin GPIO_PIN_4
#define LED3_GPIO_Port GPIOE
#define MOD_DIR1_Pin GPIO_PIN_5
#define MOD_DIR1_GPIO_Port GPIOE
#define MOD_DIR2_Pin GPIO_PIN_6
#define MOD_DIR2_GPIO_Port GPIOE
#define MOD_DIR3_Pin GPIO_PIN_8
#define MOD_DIR3_GPIO_Port GPIOI
#define MOD_DIR4_Pin GPIO_PIN_13
#define MOD_DIR4_GPIO_Port GPIOC
#define MOD_DIR5_Pin GPIO_PIN_9
#define MOD_DIR5_GPIO_Port GPIOI
#define M_Loop6_Pin GPIO_PIN_10
#define M_Loop6_GPIO_Port GPIOI
#define M_Loop3_Pin GPIO_PIN_11
#define M_Loop3_GPIO_Port GPIOI
#define MOD_DIR6_Pin GPIO_PIN_0
#define MOD_DIR6_GPIO_Port GPIOF
#define MOD_DIR7_Pin GPIO_PIN_1
#define MOD_DIR7_GPIO_Port GPIOF
#define MOD_DIR8_Pin GPIO_PIN_2
#define MOD_DIR8_GPIO_Port GPIOF
#define AD_Bus1_Pin GPIO_PIN_3
#define AD_Bus1_GPIO_Port GPIOF
#define AD_Bus2_Pin GPIO_PIN_4
#define AD_Bus2_GPIO_Port GPIOF
#define AD_Vmod17_Pin GPIO_PIN_5
#define AD_Vmod17_GPIO_Port GPIOF
#define AD_Vmod28_Pin GPIO_PIN_6
#define AD_Vmod28_GPIO_Port GPIOF
#define DAC2_SCK_Pin GPIO_PIN_7
#define DAC2_SCK_GPIO_Port GPIOF
#define DAC2_MISO_Pin GPIO_PIN_8
#define DAC2_MISO_GPIO_Port GPIOF
#define DAC2_MOSI_Pin GPIO_PIN_9
#define DAC2_MOSI_GPIO_Port GPIOF
#define DAC2_CS_Pin GPIO_PIN_10
#define DAC2_CS_GPIO_Port GPIOF
#define AD_Temp17_Pin GPIO_PIN_0
#define AD_Temp17_GPIO_Port GPIOC
#define M_Loop5_Pin GPIO_PIN_1
#define M_Loop5_GPIO_Port GPIOC
#define AD_Temp28_Pin GPIO_PIN_2
#define AD_Temp28_GPIO_Port GPIOC
#define AD_Aux_Pin GPIO_PIN_3
#define AD_Aux_GPIO_Port GPIOC
#define M_Loop7_Pin GPIO_PIN_0
#define M_Loop7_GPIO_Port GPIOA
#define CH8_G_Pin GPIO_PIN_1
#define CH8_G_GPIO_Port GPIOA
#define CH8_R_Pin GPIO_PIN_2
#define CH8_R_GPIO_Port GPIOA
#define SNA_Pin GPIO_PIN_2
#define SNA_GPIO_Port GPIOH
#define SNB_Pin GPIO_PIN_3
#define SNB_GPIO_Port GPIOH
#define AD5245_SDA_Pin GPIO_PIN_4
#define AD5245_SDA_GPIO_Port GPIOH
#define AD5245_SCL_Pin GPIO_PIN_5
#define AD5245_SCL_GPIO_Port GPIOH
#define AD_Amb_Pin GPIO_PIN_3
#define AD_Amb_GPIO_Port GPIOA
#define M_Loop8_Pin GPIO_PIN_4
#define M_Loop8_GPIO_Port GPIOA
#define CH5_G_Pin GPIO_PIN_5
#define CH5_G_GPIO_Port GPIOA
#define CH5_R_Pin GPIO_PIN_6
#define CH5_R_GPIO_Port GPIOA
#define CH7_G_Pin GPIO_PIN_7
#define CH7_G_GPIO_Port GPIOA
#define CH7_R_Pin GPIO_PIN_4
#define CH7_R_GPIO_Port GPIOC
#define CH6_R_Pin GPIO_PIN_5
#define CH6_R_GPIO_Port GPIOC
#define MOD_PG8_Pin GPIO_PIN_0
#define MOD_PG8_GPIO_Port GPIOB
#define MOD_PG7_Pin GPIO_PIN_1
#define MOD_PG7_GPIO_Port GPIOB
#define WDG_OUT_Pin GPIO_PIN_2
#define WDG_OUT_GPIO_Port GPIOB
#define MOD_PG6_Pin GPIO_PIN_11
#define MOD_PG6_GPIO_Port GPIOF
#define MOD_PG5_Pin GPIO_PIN_12
#define MOD_PG5_GPIO_Port GPIOF
#define MOD_PG4_Pin GPIO_PIN_13
#define MOD_PG4_GPIO_Port GPIOF
#define MOD_PG3_Pin GPIO_PIN_14
#define MOD_PG3_GPIO_Port GPIOF
#define MOD_PG2_Pin GPIO_PIN_15
#define MOD_PG2_GPIO_Port GPIOF
#define MOD_PG1_Pin GPIO_PIN_0
#define MOD_PG1_GPIO_Port GPIOG
#define ADD_8_Pin GPIO_PIN_1
#define ADD_8_GPIO_Port GPIOG
#define ADD_7_Pin GPIO_PIN_7
#define ADD_7_GPIO_Port GPIOE
#define ADD_6_Pin GPIO_PIN_8
#define ADD_6_GPIO_Port GPIOE
#define ADD_5_Pin GPIO_PIN_9
#define ADD_5_GPIO_Port GPIOE
#define ADD_4_Pin GPIO_PIN_10
#define ADD_4_GPIO_Port GPIOE
#define DAC1_CS_Pin GPIO_PIN_11
#define DAC1_CS_GPIO_Port GPIOE
#define DAC1_SCK_Pin GPIO_PIN_12
#define DAC1_SCK_GPIO_Port GPIOE
#define DAC1_MISO_Pin GPIO_PIN_13
#define DAC1_MISO_GPIO_Port GPIOE
#define DAC1_MOSI_Pin GPIO_PIN_14
#define DAC1_MOSI_GPIO_Port GPIOE
#define ADD_3_Pin GPIO_PIN_15
#define ADD_3_GPIO_Port GPIOE
#define ADD_2_Pin GPIO_PIN_10
#define ADD_2_GPIO_Port GPIOB
#define CH6_G_Pin GPIO_PIN_11
#define CH6_G_GPIO_Port GPIOB
#define ADD_1_Pin GPIO_PIN_6
#define ADD_1_GPIO_Port GPIOH
#define EE_SCL_Pin GPIO_PIN_7
#define EE_SCL_GPIO_Port GPIOH
#define EE_SDA_Pin GPIO_PIN_8
#define EE_SDA_GPIO_Port GPIOH
#define MOD_EN1_Pin GPIO_PIN_9
#define MOD_EN1_GPIO_Port GPIOH
#define MOD_EN2_Pin GPIO_PIN_10
#define MOD_EN2_GPIO_Port GPIOH
#define MOD_EN3_Pin GPIO_PIN_11
#define MOD_EN3_GPIO_Port GPIOH
#define MOD_EN4_Pin GPIO_PIN_12
#define MOD_EN4_GPIO_Port GPIOH
#define MOD_EN5_Pin GPIO_PIN_14
#define MOD_EN5_GPIO_Port GPIOB
#define Flash_CS_Pin GPIO_PIN_15
#define Flash_CS_GPIO_Port GPIOB
#define DeBug_TX3_Pin GPIO_PIN_8
#define DeBug_TX3_GPIO_Port GPIOD
#define DeBug_RX3_Pin GPIO_PIN_9
#define DeBug_RX3_GPIO_Port GPIOD
#define MOD_EN6_Pin GPIO_PIN_10
#define MOD_EN6_GPIO_Port GPIOD
#define MOD_EN7_Pin GPIO_PIN_11
#define MOD_EN7_GPIO_Port GPIOD
#define MOD_EN8_Pin GPIO_PIN_12
#define MOD_EN8_GPIO_Port GPIOD
#define MOD_MS1_Pin GPIO_PIN_13
#define MOD_MS1_GPIO_Port GPIOD
#define MOD_MS2_Pin GPIO_PIN_14
#define MOD_MS2_GPIO_Port GPIOD
#define MOD_MS3_Pin GPIO_PIN_15
#define MOD_MS3_GPIO_Port GPIOD
#define MOD_MS4_Pin GPIO_PIN_2
#define MOD_MS4_GPIO_Port GPIOG
#define MOD_MS5_Pin GPIO_PIN_3
#define MOD_MS5_GPIO_Port GPIOG
#define MOD_MS6_Pin GPIO_PIN_4
#define MOD_MS6_GPIO_Port GPIOG
#define MOD_MS7_Pin GPIO_PIN_5
#define MOD_MS7_GPIO_Port GPIOG
#define MOD_MS8_Pin GPIO_PIN_6
#define MOD_MS8_GPIO_Port GPIOG
#define MOD_REV1_Pin GPIO_PIN_7
#define MOD_REV1_GPIO_Port GPIOG
#define MOD_REV2_Pin GPIO_PIN_8
#define MOD_REV2_GPIO_Port GPIOG
#define MOD_REV3_Pin GPIO_PIN_6
#define MOD_REV3_GPIO_Port GPIOC
#define MOD_REV4_Pin GPIO_PIN_7
#define MOD_REV4_GPIO_Port GPIOC
#define MOD_REV5_Pin GPIO_PIN_8
#define MOD_REV5_GPIO_Port GPIOC
#define MOD_REV6_Pin GPIO_PIN_9
#define MOD_REV6_GPIO_Port GPIOC
#define MOD_REV7_Pin GPIO_PIN_8
#define MOD_REV7_GPIO_Port GPIOA
#define MOD_REV8_Pin GPIO_PIN_9
#define MOD_REV8_GPIO_Port GPIOA
#define M_Loop1_Pin GPIO_PIN_10
#define M_Loop1_GPIO_Port GPIOA
#define MOD_V_Range1_Pin GPIO_PIN_13
#define MOD_V_Range1_GPIO_Port GPIOH
#define MOD_V_Range2_Pin GPIO_PIN_14
#define MOD_V_Range2_GPIO_Port GPIOH
#define MOD_V_Range3_Pin GPIO_PIN_15
#define MOD_V_Range3_GPIO_Port GPIOH
#define AD7124_CS_Pin GPIO_PIN_0
#define AD7124_CS_GPIO_Port GPIOI
#define AD7124_SCK_Pin GPIO_PIN_1
#define AD7124_SCK_GPIO_Port GPIOI
#define AD7124_MISO_Pin GPIO_PIN_2
#define AD7124_MISO_GPIO_Port GPIOI
#define AD7124_MOSI_Pin GPIO_PIN_3
#define AD7124_MOSI_GPIO_Port GPIOI
#define M_Loop2_Pin GPIO_PIN_15
#define M_Loop2_GPIO_Port GPIOA
#define AD7175_SCK_Pin GPIO_PIN_10
#define AD7175_SCK_GPIO_Port GPIOC
#define AD7175_MISO_Pin GPIO_PIN_11
#define AD7175_MISO_GPIO_Port GPIOC
#define AD7175_MOSI_Pin GPIO_PIN_12
#define AD7175_MOSI_GPIO_Port GPIOC
#define AD7175_CS_Pin GPIO_PIN_0
#define AD7175_CS_GPIO_Port GPIOD
#define AD7175_ERR_Pin GPIO_PIN_1
#define AD7175_ERR_GPIO_Port GPIOD
#define RUN_R_Pin GPIO_PIN_2
#define RUN_R_GPIO_Port GPIOD
#define RUN_G_Pin GPIO_PIN_3
#define RUN_G_GPIO_Port GPIOD
#define CH1_R_Pin GPIO_PIN_4
#define CH1_R_GPIO_Port GPIOD
#define CH1_G_Pin GPIO_PIN_5
#define CH1_G_GPIO_Port GPIOD
#define CH2_G_Pin GPIO_PIN_6
#define CH2_G_GPIO_Port GPIOD
#define CH2_R_Pin GPIO_PIN_7
#define CH2_R_GPIO_Port GPIOD
#define CH3_G_Pin GPIO_PIN_9
#define CH3_G_GPIO_Port GPIOG
#define CH3_R_Pin GPIO_PIN_10
#define CH3_R_GPIO_Port GPIOG
#define FRAM_CS_Pin GPIO_PIN_11
#define FRAM_CS_GPIO_Port GPIOG
#define FRAM_MISO_Pin GPIO_PIN_12
#define FRAM_MISO_GPIO_Port GPIOG
#define FRAM_SCK_Pin GPIO_PIN_13
#define FRAM_SCK_GPIO_Port GPIOG
#define FRAM_MOSI_Pin GPIO_PIN_14
#define FRAM_MOSI_GPIO_Port GPIOG
#define MOD_V_Range4_Pin GPIO_PIN_15
#define MOD_V_Range4_GPIO_Port GPIOG
#define W5500_SCK_Pin GPIO_PIN_3
#define W5500_SCK_GPIO_Port GPIOB
#define W5500_MISO_Pin GPIO_PIN_4
#define W5500_MISO_GPIO_Port GPIOB
#define W5500_MOSI_Pin GPIO_PIN_5
#define W5500_MOSI_GPIO_Port GPIOB
#define CH4_R_Pin GPIO_PIN_6
#define CH4_R_GPIO_Port GPIOB
#define CH4_G_Pin GPIO_PIN_7
#define CH4_G_GPIO_Port GPIOB
#define MOD_V_Range5_Pin GPIO_PIN_8
#define MOD_V_Range5_GPIO_Port GPIOB
#define MOD_V_Range6_Pin GPIO_PIN_9
#define MOD_V_Range6_GPIO_Port GPIOB
#define AD7175_RDY_Pin GPIO_PIN_0
#define AD7175_RDY_GPIO_Port GPIOE
#define AD7175_RDY_EXTI_IRQn EXTI0_IRQn
#define AD7124_RDY_Pin GPIO_PIN_1
#define AD7124_RDY_GPIO_Port GPIOE
#define AD7124_RDY_EXTI_IRQn EXTI1_IRQn
#define MOD_V_Range7_Pin GPIO_PIN_4
#define MOD_V_Range7_GPIO_Port GPIOI
#define MOD_V_Range8_Pin GPIO_PIN_5
#define MOD_V_Range8_GPIO_Port GPIOI
#define W5500_CS_Pin GPIO_PIN_6
#define W5500_CS_GPIO_Port GPIOI
#define W5500_RST_Pin GPIO_PIN_7
#define W5500_RST_GPIO_Port GPIOI
/* USER CODE BEGIN Private defines */
#define Boot_Flag_addr 0x7FFF


#define APPLICATION_ADDRESS   (uint32_t)0x08020000
#define W5500_localport 5000
/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

void Boot_JMP_APP(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
