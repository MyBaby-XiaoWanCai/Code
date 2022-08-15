/**
  ******************************************************************************
  * @file    rng.c
  * @brief   This file provides code for the configuration
  *          of the RNG instances.
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

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"

RNG_HandleTypeDef hrng;

/* USER CODE BEGIN 0 */

/**********************************************************************************************************
**	函 数 名 : RNG_Get_RandomRange()
**	功能说明 : 生成一个指定范围随机数
**	形    参 : min：范围下限 max：范围上限
**	返 回 值 : 生成数
**********************************************************************************************************/
uint32_t RNG_Get_RandomRange(uint32_t min,uint32_t max)

{

    return HAL_RNG_GetRandomNumber(&hrng)%(max-min+1) +min;

}
/* USER CODE END 0 */



/* RNG init function */
void bsp_InitRNG(void)
{

    /* USER CODE BEGIN RNG_Init 0 */

    /* USER CODE END RNG_Init 0 */

    /* USER CODE BEGIN RNG_Init 1 */

    /* USER CODE END RNG_Init 1 */
    hrng.Instance = RNG;
    if (HAL_RNG_Init(&hrng) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    /* USER CODE BEGIN RNG_Init 2 */

    /* USER CODE END RNG_Init 2 */

}

void HAL_RNG_MspInit(RNG_HandleTypeDef* rngHandle)
{

    if(rngHandle->Instance==RNG)
    {
        /* USER CODE BEGIN RNG_MspInit 0 */

        /* USER CODE END RNG_MspInit 0 */
        /* RNG clock enable */
        __HAL_RCC_RNG_CLK_ENABLE();
        /* USER CODE BEGIN RNG_MspInit 1 */

        /* USER CODE END RNG_MspInit 1 */
    }
}

void HAL_RNG_MspDeInit(RNG_HandleTypeDef* rngHandle)
{

    if(rngHandle->Instance==RNG)
    {
        /* USER CODE BEGIN RNG_MspDeInit 0 */

        /* USER CODE END RNG_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_RNG_CLK_DISABLE();
        /* USER CODE BEGIN RNG_MspDeInit 1 */

        /* USER CODE END RNG_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
