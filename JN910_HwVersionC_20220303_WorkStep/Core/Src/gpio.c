/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, STM32_DAC_CS3_Pin|STM32_DAC_CS4_Pin|STM32_DAC_CS5_Pin|STM32_DAC_CS6_Pin
                          |STM32_DAC_CS7_Pin|STM32_DAC_CS8_Pin|FRAM_CS_Pin|GPIO_LED1_Pin
                          |STM32_DAC_CS1_Pin|STM32_DAC_CS2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, V_STM32_ADC_CS8_2_Pin|I_STM32_ADC_CS1_1_Pin|I_STM32_ADC_CS2_1_Pin|V_STM32_ADC_CS1_2_Pin
                          |V_STM32_ADC_CS2_2_Pin|V_STM32_ADC_CS4_2_Pin|V_STM32_ADC_CS3_2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, I2C2_SDA_Pin|I2C2_SCL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, STM32_SSwA_P_1_Pin|STM32_SSwA_P_2_Pin|STM32_SSwA_P_3_Pin|STM32_SSwA_P_4_Pin
                          |STM32_SSwA_P_5_Pin|STM32_SSwA_P_6_Pin|STM32_SSwA_P_7_Pin|STM32_SSwA_P_8_Pin
                          |STM32_SSwA_N_1_Pin|STM32_SSwA_N_2_Pin|STM32_SSwA_N_3_Pin|STM32_SSwA_N_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, STM32_N_1_Pin|STM32_N_2_Pin|STM32_N_3_Pin|STM32_N_4_Pin
                          |STM32_N_5_Pin|STM32_N_6_Pin|STM32_N_7_Pin|STM32_N_8_Pin
                          |LED0_R_STM32_Pin|LED0_G_STM32_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, STM32_SSwA_N_6_Pin|STM32_SSwA_N_7_Pin|STM32_SSwA_N_8_Pin|STM32_P_7_Pin
                          |STM32_P_8_Pin|STM32_SSwA_N_5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, V_STM32_ADC_CS5_2_Pin|I2C3_SDA_Pin|I2C3_SCL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, STM32_P_2_Pin|STM32_P_4_Pin|STM32_P_5_Pin|STM32_P_6_Pin
                          |STM32_P_3_Pin|STM32_P_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, I2C1_SDA_Pin|I2C1_SCL_Pin|I_STM32_ADC_CS3_1_Pin|I_STM32_ADC_CS4_1_Pin
                          |I_STM32_ADC_CS5_1_Pin|I_STM32_ADC_CS6_1_Pin|I_STM32_ADC_CS7_1_Pin|I_STM32_ADC_CS8_1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SNOE1_GPIO_Port, SNOE1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, V_STM32_ADC_CS6_2_Pin|V_STM32_ADC_CS7_2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin
                           PEPin PEPin PEPin PEPin
                           PEPin */
  GPIO_InitStruct.Pin = STM32_DAC_CS3_Pin|STM32_DAC_CS4_Pin|STM32_DAC_CS5_Pin|STM32_DAC_CS6_Pin
                          |STM32_DAC_CS7_Pin|STM32_DAC_CS8_Pin|FRAM_CS_Pin|STM32_DAC_CS1_Pin
                          |STM32_DAC_CS2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = V_STM32_ADC_CS8_2_Pin|I_STM32_ADC_CS1_1_Pin|I_STM32_ADC_CS2_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin */
  GPIO_InitStruct.Pin = I2C2_SDA_Pin|I2C2_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin PFPin PFPin
                           PFPin PFPin PFPin PFPin
                           PFPin PFPin PFPin PFPin */
  GPIO_InitStruct.Pin = STM32_SSwA_P_1_Pin|STM32_SSwA_P_2_Pin|STM32_SSwA_P_3_Pin|STM32_SSwA_P_4_Pin
                          |STM32_SSwA_P_5_Pin|STM32_SSwA_P_6_Pin|STM32_SSwA_P_7_Pin|STM32_SSwA_P_8_Pin
                          |STM32_SSwA_N_1_Pin|STM32_SSwA_N_2_Pin|STM32_SSwA_N_3_Pin|STM32_SSwA_N_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin */
  GPIO_InitStruct.Pin = V_STM32_ADC_DRDY5_1_Pin|I_STM32_ADC_DRDY5_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = V_STM32_ADC_DRDY4_1_Pin|I_STM32_ADC_DRDY4_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin */
  GPIO_InitStruct.Pin = V_STM32_ADC_DRDY3_1_Pin|I_STM32_ADC_DRDY3_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = V_STM32_ADC_DRDY1_1_Pin|I_STM32_ADC_DRDY1_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin PGPin PGPin
                           PGPin PGPin PGPin PGPin
                           PGPin PGPin PGPin PGPin */
  GPIO_InitStruct.Pin = STM32_N_1_Pin|STM32_N_2_Pin|STM32_N_3_Pin|STM32_N_4_Pin
                          |STM32_N_5_Pin|STM32_N_6_Pin|STM32_N_7_Pin|STM32_N_8_Pin
                          |V_STM32_ADC_CS6_2_Pin|V_STM32_ADC_CS7_2_Pin|LED0_R_STM32_Pin|LED0_G_STM32_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = Fan_error_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(Fan_error_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin */
  GPIO_InitStruct.Pin = V_STM32_ADC_DRDY6_1_Pin|I_STM32_ADC_DRDY6_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = GPIO_LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO_LED1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin
                           PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = STM32_SSwA_N_6_Pin|STM32_SSwA_N_7_Pin|STM32_SSwA_N_8_Pin|V_STM32_ADC_CS5_2_Pin
                          |STM32_P_7_Pin|STM32_P_8_Pin|STM32_SSwA_N_5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin
                           PDPin PDPin PDPin PDPin
                           PDPin PDPin PDPin PDPin */
  GPIO_InitStruct.Pin = STM32_P_2_Pin|STM32_P_4_Pin|STM32_P_5_Pin|STM32_P_6_Pin
                          |I_STM32_ADC_CS3_1_Pin|I_STM32_ADC_CS4_1_Pin|I_STM32_ADC_CS5_1_Pin|I_STM32_ADC_CS6_1_Pin
                          |I_STM32_ADC_CS7_1_Pin|I_STM32_ADC_CS8_1_Pin|STM32_P_3_Pin|STM32_P_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin */
  GPIO_InitStruct.Pin = V_STM32_ADC_DRDY7_1_Pin|I_STM32_ADC_DRDY7_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin */
  GPIO_InitStruct.Pin = I2C1_SDA_Pin|I2C1_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin PGPin PGPin */
  GPIO_InitStruct.Pin = V_STM32_ADC_DRDY2_1_Pin|I_STM32_ADC_DRDY2_2_Pin|V_STM32_ADC_DRDY8_1_Pin|I_STM32_ADC_DRDY8_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = V_STM32_ADC_CS1_2_Pin|V_STM32_ADC_CS2_2_Pin|V_STM32_ADC_CS4_2_Pin|V_STM32_ADC_CS3_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SNOE1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SNOE1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = STM32_EXTI_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(STM32_EXTI_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = I2C3_SDA_Pin|I2C3_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
//  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
//  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 1, 0);
//  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 1, 0);
//  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 1, 0);
//  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
//  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
//  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* USER CODE BEGIN 2 */
void DisableAllExtIRQ(void)
{
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	HAL_NVIC_DisableIRQ(EXTI3_IRQn);
	HAL_NVIC_DisableIRQ(EXTI4_IRQn);
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}

void EnableExtIRQ(uint16_t IRQNum)
{
	switch(IRQNum)
	{
		case (0x01<<0):
		{
			HAL_NVIC_EnableIRQ(EXTI0_IRQn);
			break;
		}
		case (0x01<<1):
		{
			HAL_NVIC_EnableIRQ(EXTI1_IRQn);
			break;
		}
		case (0x01<<2):
		{
			HAL_NVIC_EnableIRQ(EXTI2_IRQn);
			break;
		}
		case (0x01<<3):
		{
			HAL_NVIC_EnableIRQ(EXTI3_IRQn);
			break;
		}
		case (0x01<<4):
		{
			HAL_NVIC_EnableIRQ(EXTI4_IRQn);
			break;
		}
		case (0x01<<5):
		case (0x01<<6):
		case (0x01<<7):
		case (0x01<<8):
		case (0x01<<9):
		{
			HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		}
		case (0x01<<10):
		case (0x01<<11):
		case (0x01<<12):
		case (0x01<<13):
		case (0x01<<14):
		case (0x01<<15):
		{
			HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		}
		default:
		{
			break;
		}
	}
}

void DisableExtIRQ(uint16_t IRQNum)
{
	switch(IRQNum)
	{
		case (0x01<<0):
		{
			HAL_NVIC_DisableIRQ(EXTI0_IRQn);
			break;
		}
		case (0x01<<1):
		{
			HAL_NVIC_DisableIRQ(EXTI1_IRQn);
			break;
		}
		case (0x01<<2):
		{
			HAL_NVIC_DisableIRQ(EXTI2_IRQn);
			break;
		}
		case (0x01<<3):
		{
			HAL_NVIC_DisableIRQ(EXTI3_IRQn);
			break;
		}
		case (0x01<<4):
		{
			HAL_NVIC_DisableIRQ(EXTI4_IRQn);
			break;
		}
		case (0x01<<5):
		case (0x01<<6):
		case (0x01<<7):
		case (0x01<<8):
		case (0x01<<9):
		{
			HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
			break;
		}
		case (0x01<<10):
		case (0x01<<11):
		case (0x01<<12):
		case (0x01<<13):
		case (0x01<<14):
		case (0x01<<15):
		{
			HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
			break;
		}
		default:
		{
			break;
		}
	}
}
/* USER CODE END 2 */
