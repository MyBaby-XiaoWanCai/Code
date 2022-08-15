/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_spi.c
** Author      : Cai Heng
** Date        : 2021-07-23
** Description : This file is used to initialize the spi interface
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
SPI_HandleTypeDef SPI1_Handler;         //SPI1���(����W5500)
SPI_HandleTypeDef SPI2_Handler;         //SPI2���(����AD7124)
SPI_HandleTypeDef SPI3_Handler;         //SPI3���(����AD7175)
SPI_HandleTypeDef SPI4_Handler;         //SPI4���(����DAC80508_1)
SPI_HandleTypeDef SPI5_Handler;         //SPI5���(����DAC80508_2)
SPI_HandleTypeDef SPI6_Handler;         //SPI6���(����FLASH)

DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi6_tx;
DMA_HandleTypeDef hdma_spi6_rx;
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void bsp_InitSpi1(void);
void bsp_InitSpi2(void);
void bsp_InitSpi3(void);
void bsp_InitSpi4(void);
void bsp_InitSpi5(void);
void bsp_InitSpi6(void);
void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle);
void SPI1_DMA_init(void); 
void SPI6_DMA_init(void); 
/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : bsp_InitSpi1()
**  ����˵�� : ����SPI1�ӿ�(GPIO���ü�SPI����)
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitSpi1()
{
#if (SPI1_CFG == ENABLE_CFG)
#ifdef Use_W5500_DMA    
    SPI1_DMA_init();                                //��ʼ��DMA
#endif    
    SPI1_REG_CONFIG();                              //SPI1�Ĵ�������

    if(HAL_SPI_Init(&SPI1_Handler) != HAL_OK)       //��ʼ��SPI1
    {
        Error_Handler(__FILE__, __LINE__);
    }

    SPI1_CS_DISABLE();                              //SPI1Ƭѡ�ź�����

    __HAL_SPI_ENABLE(&SPI1_Handler);                //ʹ��SPI1

  
#endif /* (SPI1_CFG == ENABLE) */
}

/**********************************************************************************************************
**  �� �� �� : bsp_InitSpi2()
**  ����˵�� : ����SPI2�ӿ�(GPIO���ü�SPI����)
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitSpi2()
{
#if (SPI2_CFG == ENABLE_CFG)

    SPI2_REG_CONFIG();                              //SPI2�Ĵ�������

    if(HAL_SPI_Init(&SPI2_Handler) != HAL_OK)       //��ʼ��SPI2
    {
        Error_Handler(__FILE__, __LINE__);
    }

    SPI2_CS_DISABLE();                              //SPI2Ƭѡ�ź�����

    __HAL_SPI_ENABLE(&SPI2_Handler);                //ʹ��SPI2

#endif /* (SPI2_CFG == ENABLE) */
}

/**********************************************************************************************************
**  �� �� �� : bsp_InitSpi3()
**  ����˵�� : ����SPI3�ӿ�(GPIO���ü�SPI����)
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitSpi3()
{
#if (SPI3_CFG == ENABLE_CFG)

    SPI3_REG_CONFIG();                              //SPI3�Ĵ�������

    if(HAL_SPI_Init(&SPI3_Handler) != HAL_OK)       //��ʼ��SPI3
    {
        Error_Handler(__FILE__, __LINE__);
    }

    SPI3_CS_DISABLE();                              //SPI3Ƭѡ�ź�����

    __HAL_SPI_ENABLE(&SPI3_Handler);                //ʹ��SPI3

#endif /* (SPI3_CFG == ENABLE) */
}

/**********************************************************************************************************
**  �� �� �� : bsp_InitSpi4()
**  ����˵�� : ����SPI4�ӿ�(GPIO���ü�SPI����)
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitSpi4()
{
#if (SPI4_CFG == ENABLE_CFG)

    SPI4_REG_CONFIG();                              //SPI4�Ĵ�������

    if(HAL_SPI_Init(&SPI4_Handler) != HAL_OK)       //��ʼ��SPI4
    {
        Error_Handler(__FILE__, __LINE__);
    }

    SPI4_CS_DISABLE();                              //SPI4Ƭѡ�ź�����

    __HAL_SPI_ENABLE(&SPI4_Handler);                //ʹ��SPI4

#endif /* (SPI4_CFG == ENABLE) */
}

/**********************************************************************************************************
**  �� �� �� : bsp_InitSpi5()
**  ����˵�� : ����SPI5�ӿ�(GPIO���ü�SPI����)
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitSpi5()
{
#if (SPI5_CFG == ENABLE_CFG)

    SPI5_REG_CONFIG();                              //SPI5�Ĵ�������

    if(HAL_SPI_Init(&SPI5_Handler) != HAL_OK)       //��ʼ��SPI5
    {
        Error_Handler(__FILE__, __LINE__);
    }

    SPI5_CS_DISABLE();                              //SPI5Ƭѡ�ź�����

    __HAL_SPI_ENABLE(&SPI5_Handler);                //ʹ��SPI5

#endif /* (SPI5_CFG == ENABLE) */
}

/**********************************************************************************************************
**  �� �� �� : bsp_InitSpi6()
**  ����˵�� : ����SPI6�ӿ�(GPIO���ü�SPI����)
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitSpi6(void)
{
#if (SPI6_CFG == ENABLE_CFG)
#ifdef Use_FRAM_DMA     
    SPI6_DMA_init();                                //��ʼ��DMA
#endif
    
    SPI6_REG_CONFIG();                              //SPI6�Ĵ�������

    if(HAL_SPI_Init(&SPI6_Handler) != HAL_OK)       //��ʼ��SPI6
    {
        Error_Handler(__FILE__, __LINE__);
    }
    FLASH_CS_DISABLE();                             //SPI6Ƭѡ�ź�����
    FRAM_CS_DISABLE();
    __HAL_SPI_ENABLE(&SPI6_Handler);                //ʹ��SPI6

#endif /* (SPI6_CFG == ENABLE) */
}

/*
*********************************************************************************************************
*   �� �� ��: HAL_SPI_MspInit
*   ����˵��: This function configures the hardware resources used in this example
*   ��    ��: hspi: SPI handle
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(spiHandle->Instance == SPI1)
    {
        /* USER CODE BEGIN SPI1_MspInit 0 */
#if (SPI1_CFG == ENABLE_CFG)
        /* ��SPI1���GPIOʱ�� */
        SPI1_GPIO_CLK_ALLENABLE();
        /* ��SPI1����ʱ�� */
        __HAL_RCC_SPI1_CLK_ENABLE();

        /**SPI1 GPIO Configuration
        PB3     ------> SPI1_SCK
        PB4     ------> SPI1_MISO
        PB5     ------> SPI1_MOSI
        */

        /* ���ø���GPIO��������(Configurable) */
        GPIO_InitStruct.Mode = SPI1_AF_MODE;
        GPIO_InitStruct.Pull = SPI1_AF_PULL;
        GPIO_InitStruct.Speed = SPI1_AF_SPEED;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        GPIO_InitStruct.Pin = SPI1_SCLK_PIN;
        HAL_GPIO_Init(SPI1_SCLK_PORT, &GPIO_InitStruct);                //��ʼ��SPI1_SCLK��Ӧ��GPIO
        GPIO_InitStruct.Pin = SPI1_MISO_PIN;
        HAL_GPIO_Init(SPI1_MISO_PORT, &GPIO_InitStruct);                //��ʼ��SPI1_MISO��Ӧ��GPIO
        GPIO_InitStruct.Pin = SPI1_MOSI_PIN;
        HAL_GPIO_Init(SPI1_MOSI_PORT, &GPIO_InitStruct);                //��ʼ��SPI1_MOSI��Ӧ��GPIO

        /*������ͨGPIO��������(Configurable) */
        GPIO_InitStruct.Mode = SPI1_GP_MODE;
        GPIO_InitStruct.Pull = SPI1_GP_PULL;
        GPIO_InitStruct.Speed = SPI1_GP_SPEED;
        GPIO_InitStruct.Pin = SPI1_CS_PIN;
        HAL_GPIO_Init(SPI1_CS_PORT, &GPIO_InitStruct);                  //��ʼ��SPI1_CS��Ӧ��GPIO
#endif
        /* USER CODE END SPI1_MspInit 0 */
 #ifdef Use_W5500_DMA       
        /* SPI1 DMA Init */
        /* SPI1_TX Init */
        hdma_spi1_tx.Instance = DMA2_Stream3;
        hdma_spi1_tx.Init.Channel = DMA_CHANNEL_3;
        hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi1_tx.Init.Mode = DMA_NORMAL;
        hdma_spi1_tx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_spi1_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
        hdma_spi1_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        hdma_spi1_tx.Init.MemBurst = DMA_MBURST_SINGLE;
        hdma_spi1_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
        if(HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        __HAL_LINKDMA(spiHandle, hdmatx, hdma_spi1_tx);
        /* SPI1_RX Init */
        hdma_spi1_rx.Instance = DMA2_Stream2;
        hdma_spi1_rx.Init.Channel = DMA_CHANNEL_3;
        hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi1_rx.Init.Mode = DMA_NORMAL;
        hdma_spi1_rx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_spi1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if(HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        __HAL_LINKDMA(spiHandle, hdmarx, hdma_spi1_rx);
        /* USER CODE BEGIN SPI1_MspInit 1 */
        HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI1_IRQn);        
        /* USER CODE END SPI1_MspInit 1 */
#endif          
    }
    else
        if(spiHandle->Instance == SPI2)
        {
            /* USER CODE BEGIN SPI2_MspInit 0 */
#if (SPI2_CFG == ENABLE_CFG)
            /* ��SPI2���GPIOʱ�� */
            SPI2_GPIO_CLK_ALLENABLE();
            /* ��SPI1����ʱ�� */
            __HAL_RCC_SPI2_CLK_ENABLE();

            /* ���ø���GPIO��������(Configurable) */
            GPIO_InitStruct.Mode = SPI2_AF_MODE;
            GPIO_InitStruct.Pull = SPI2_AF_PULL;
            GPIO_InitStruct.Speed = SPI2_AF_SPEED;
            GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
            GPIO_InitStruct.Pin = SPI2_SCLK_PIN;
            HAL_GPIO_Init(SPI2_SCLK_PORT, &GPIO_InitStruct);                //��ʼ��SPI2_SCLK��Ӧ��GPIO
            GPIO_InitStruct.Pin = SPI2_MISO_PIN;
            HAL_GPIO_Init(SPI2_MISO_PORT, &GPIO_InitStruct);                //��ʼ��SPI2_MISO��Ӧ��GPIO
            GPIO_InitStruct.Pin = SPI2_MOSI_PIN;
            HAL_GPIO_Init(SPI2_MOSI_PORT, &GPIO_InitStruct);                //��ʼ��SPI2_MOSI��Ӧ��GPIO

            /* ������ͨGPIO��������(Configurable) */
            GPIO_InitStruct.Mode = SPI2_GP_MODE;
            GPIO_InitStruct.Pull = SPI2_GP_PULL;
            GPIO_InitStruct.Speed = SPI2_GP_SPEED;
            GPIO_InitStruct.Pin = SPI2_CS_PIN;
            HAL_GPIO_Init(SPI2_CS_PORT, &GPIO_InitStruct);                  //��ʼ��SPI2_CS��Ӧ��GPIO
#endif
            /* USER CODE END SPI2_MspInit 0 */

            /* USER CODE BEGIN SPI2_MspInit 1 */

            /* USER CODE END SPI2_MspInit 1 */
        }
        else
            if(spiHandle->Instance == SPI3)
            {
                /* USER CODE BEGIN SPI3_MspInit 0 */
#if (SPI3_CFG == ENABLE_CFG)
                /* ��SPI3���GPIOʱ�� */
                SPI3_GPIO_CLK_ALLENABLE();
                /* ��SPI1����ʱ�� */
                __HAL_RCC_SPI3_CLK_ENABLE();

                /* ���ø���GPIO��������(Configurable) */
                GPIO_InitStruct.Mode = SPI3_AF_MODE;
                GPIO_InitStruct.Pull = SPI3_AF_PULL;
                GPIO_InitStruct.Speed = SPI3_AF_SPEED;
                GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
                GPIO_InitStruct.Pin = SPI3_SCLK_PIN;
                HAL_GPIO_Init(SPI3_SCLK_PORT, &GPIO_InitStruct);                //��ʼ��SPI3_SCLK��Ӧ��GPIO
                GPIO_InitStruct.Pin = SPI3_MISO_PIN;
                HAL_GPIO_Init(SPI3_MISO_PORT, &GPIO_InitStruct);                //��ʼ��SPI3_MISO��Ӧ��GPIO
                GPIO_InitStruct.Pin = SPI3_MOSI_PIN;
                HAL_GPIO_Init(SPI3_MOSI_PORT, &GPIO_InitStruct);                //��ʼ��SPI3_MOSI��Ӧ��GPIO

                /* ������ͨGPIO��������(Configurable) */
                GPIO_InitStruct.Mode = SPI3_GP_MODE;
                GPIO_InitStruct.Pull = SPI3_GP_PULL;
                GPIO_InitStruct.Speed = SPI3_GP_SPEED;
                GPIO_InitStruct.Pin = SPI3_CS_PIN;
                HAL_GPIO_Init(SPI3_CS_PORT, &GPIO_InitStruct);                  //��ʼ��SPI3_CS��Ӧ��GPIO
#endif
                /* USER CODE END SPI3_MspInit 0 */

                /* USER CODE BEGIN SPI3_MspInit 1 */

                /* USER CODE END SPI3_MspInit 1 */
            }
            else
                if(spiHandle->Instance == SPI4)
                {
                    /* USER CODE BEGIN SPI4_MspInit 0 */
#if (SPI4_CFG == ENABLE_CFG)
                    /* ��SPI4���GPIOʱ�� */
                    SPI4_GPIO_CLK_ALLENABLE();
                    /* ��SPI1����ʱ�� */
                    __HAL_RCC_SPI4_CLK_ENABLE();

                    /* ���ø���GPIO��������(Configurable) */
                    GPIO_InitStruct.Mode = SPI4_AF_MODE;
                    GPIO_InitStruct.Pull = SPI4_AF_PULL;
                    GPIO_InitStruct.Speed = SPI4_AF_SPEED;
                    GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
                    GPIO_InitStruct.Pin = SPI4_SCLK_PIN;
                    HAL_GPIO_Init(SPI4_SCLK_PORT, &GPIO_InitStruct);                //��ʼ��SPI4_SCLK��Ӧ��GPIO
                    GPIO_InitStruct.Pin = SPI4_MISO_PIN;
                    HAL_GPIO_Init(SPI4_MISO_PORT, &GPIO_InitStruct);                //��ʼ��SPI4_MISO��Ӧ��GPIO
                    GPIO_InitStruct.Pin = SPI4_MOSI_PIN;
                    HAL_GPIO_Init(SPI4_MOSI_PORT, &GPIO_InitStruct);                //��ʼ��SPI4_MOSI��Ӧ��GPIO

                    /* ������ͨGPIO��������(Configurable) */
                    GPIO_InitStruct.Mode = SPI4_GP_MODE;
                    GPIO_InitStruct.Pull = SPI4_GP_PULL;
                    GPIO_InitStruct.Speed = SPI4_GP_SPEED;
                    GPIO_InitStruct.Pin = SPI4_CS_PIN;
                    HAL_GPIO_Init(SPI4_CS_PORT, &GPIO_InitStruct);                  //��ʼ��SPI4_CS��Ӧ��GPIO
#endif
                    /* USER CODE END SPI4_MspInit 0 */

                    /* USER CODE BEGIN SPI4_MspInit 1 */

                    /* USER CODE END SPI4_MspInit 1 */
                }
                else
                    if(spiHandle->Instance == SPI5)
                    {
                        /* USER CODE BEGIN SPI5_MspInit 0 */
#if (SPI5_CFG == ENABLE_CFG)
                        /* ��SPI5���GPIOʱ�� */
                        SPI5_GPIO_CLK_ALLENABLE();
                        /* ��SPI1����ʱ�� */
                        __HAL_RCC_SPI5_CLK_ENABLE();

                        /* ���ø���GPIO��������(Configurable) */
                        GPIO_InitStruct.Mode = SPI5_AF_MODE;
                        GPIO_InitStruct.Pull = SPI5_AF_PULL;
                        GPIO_InitStruct.Speed = SPI5_AF_SPEED;
                        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
                        GPIO_InitStruct.Pin = SPI5_SCLK_PIN;
                        HAL_GPIO_Init(SPI5_SCLK_PORT, &GPIO_InitStruct);                //��ʼ��SPI5_SCLK��Ӧ��GPIO
                        GPIO_InitStruct.Pin = SPI5_MISO_PIN;
                        HAL_GPIO_Init(SPI5_MISO_PORT, &GPIO_InitStruct);                //��ʼ��SPI5_MISO��Ӧ��GPIO
                        GPIO_InitStruct.Pin = SPI5_MOSI_PIN;
                        HAL_GPIO_Init(SPI5_MOSI_PORT, &GPIO_InitStruct);                //��ʼ��SPI5_MOSI��Ӧ��GPIO

                        /* ������ͨGPIO��������(Configurable) */
                        GPIO_InitStruct.Mode = SPI5_GP_MODE;
                        GPIO_InitStruct.Pull = SPI5_GP_PULL;
                        GPIO_InitStruct.Speed = SPI5_GP_SPEED;
                        GPIO_InitStruct.Pin = SPI5_CS_PIN;
                        HAL_GPIO_Init(SPI5_CS_PORT, &GPIO_InitStruct);                  //��ʼ��SPI5_CS��Ӧ��GPIO
#endif
                        /* USER CODE END SPI5_MspInit 0 */

                        /* USER CODE BEGIN SPI5_MspInit 1 */

                        /* USER CODE END SPI5_MspInit 1 */
                    }
                    else
                        if(spiHandle->Instance == SPI6)
                        {
                            /* USER CODE BEGIN SPI6_MspInit 0 */
#if (SPI6_CFG == ENABLE_CFG)
                            /* ��SPI6���GPIOʱ�� */
                            SPI6_GPIO_CLK_ALLENABLE();
                            /* ��SPI6����ʱ�� */
                            __HAL_RCC_SPI6_CLK_ENABLE();

                            /* ���ø���GPIO��������(Configurable) */
                            GPIO_InitStruct.Mode = SPI6_AF_MODE;
                            GPIO_InitStruct.Pull = SPI6_AF_PULL;
                            GPIO_InitStruct.Speed = SPI6_AF_SPEED;
                            GPIO_InitStruct.Alternate = GPIO_AF5_SPI6;
                            GPIO_InitStruct.Pin = SPI6_SCLK_PIN;
                            HAL_GPIO_Init(SPI6_SCLK_PORT, &GPIO_InitStruct);                //��ʼ��SPI6_SCLK��Ӧ��GPIO
                            GPIO_InitStruct.Pin = SPI6_MISO_PIN;
                            HAL_GPIO_Init(SPI6_MISO_PORT, &GPIO_InitStruct);                //��ʼ��SPI6_MISO��Ӧ��GPIO
                            GPIO_InitStruct.Pin = SPI6_MOSI_PIN;
                            HAL_GPIO_Init(SPI6_MOSI_PORT, &GPIO_InitStruct);                //��ʼ��SPI6_MOSI��Ӧ��GPIO

                            /* ������ͨGPIO��������(Configurable) */
                            GPIO_InitStruct.Mode = SPI6_GP_MODE;
                            GPIO_InitStruct.Pull = SPI6_GP_PULL;
                            GPIO_InitStruct.Speed = SPI6_GP_SPEED;
                            GPIO_InitStruct.Pin = FLASH_CS_PIN;
                            HAL_GPIO_Init(FLASH_CS_PORT, &GPIO_InitStruct);                 //��ʼ��FLASH_CS��Ӧ��GPIO

                            GPIO_InitStruct.Mode = SPI6_GP_MODE;
                            GPIO_InitStruct.Pull = SPI6_GP_PULL;
                            GPIO_InitStruct.Speed = SPI6_GP_SPEED;
                            GPIO_InitStruct.Pin = FRAM_CS_PIN;
                            HAL_GPIO_Init(FRAM_CS_PORT, &GPIO_InitStruct);                  //��ʼ��FRAM_CS��Ӧ��GPIO
#endif
                            /* USER CODE END SPI6_MspInit 0 */
#ifdef Use_FRAM_DMA                              
                            /* SPI6 DMA Init */
                            /* SPI6_TX Init */
                            hdma_spi6_tx.Instance = DMA2_Stream5;
                            hdma_spi6_tx.Init.Channel = DMA_CHANNEL_1;
                            hdma_spi6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
                            hdma_spi6_tx.Init.PeriphInc = DMA_PINC_DISABLE;
                            hdma_spi6_tx.Init.MemInc = DMA_MINC_ENABLE;
                            hdma_spi6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
                            hdma_spi6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
                            hdma_spi6_tx.Init.Mode = DMA_NORMAL;
                            hdma_spi6_tx.Init.Priority = DMA_PRIORITY_LOW;
                            hdma_spi6_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
                            hdma_spi6_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
                            hdma_spi6_tx.Init.MemBurst = DMA_MBURST_SINGLE;
                            hdma_spi6_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
                            if(HAL_DMA_Init(&hdma_spi6_tx) != HAL_OK)
                            {
                                Error_Handler(__FILE__, __LINE__);
                            }

                            __HAL_LINKDMA(spiHandle, hdmatx, hdma_spi6_tx);

                            /* SPI6_RX Init */
                            hdma_spi6_rx.Instance = DMA2_Stream6;
                            hdma_spi6_rx.Init.Channel = DMA_CHANNEL_1;
                            hdma_spi6_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
                            hdma_spi6_rx.Init.PeriphInc = DMA_PINC_DISABLE;
                            hdma_spi6_rx.Init.MemInc = DMA_MINC_ENABLE;
                            hdma_spi6_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
                            hdma_spi6_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
                            hdma_spi6_rx.Init.Mode = DMA_NORMAL;
                            hdma_spi6_rx.Init.Priority = DMA_PRIORITY_LOW;
                            hdma_spi6_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
                            if(HAL_DMA_Init(&hdma_spi6_rx) != HAL_OK)
                            {
                                Error_Handler(__FILE__, __LINE__);
                            }

                            __HAL_LINKDMA(spiHandle, hdmarx, hdma_spi6_rx);
                            /* USER CODE BEGIN SPI6_MspInit 1 */
                            HAL_NVIC_SetPriority(SPI6_IRQn, 0, 0);
                            HAL_NVIC_EnableIRQ(SPI6_IRQn);
                            /* USER CODE END SPI6_MspInit 1 */
#endif                              
                        }
}

/*
*********************************************************************************************************
*   �� �� ��: HAL_SPI_MspDeInit
*   ����˵��: This function freeze the hardware resources used in this example
*   ��    ��: hspi: SPI handle
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle)
{
    if(spiHandle->Instance == SPI1)
    {
        /* USER CODE BEGIN SPI1_MspDeInit 0 */

        /* USER CODE END SPI1_MspDeInit 0 */

        /* Peripheral clock disable */
        __HAL_RCC_SPI1_CLK_DISABLE();

        /**SPI1 GPIO Configuration
        PB3     ------> SPI1_SCK
        PB4     ------> SPI1_MISO
        PB5     ------> SPI1_MOSI
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

        /* USER CODE BEGIN SPI1_MspDeInit 1 */

        /* USER CODE END SPI1_MspDeInit 1 */
    }
    else
        if(spiHandle->Instance == SPI2)
        {
            /* USER CODE BEGIN SPI2_MspDeInit 0 */

            /* USER CODE END SPI2_MspDeInit 0 */

            /* Peripheral clock disable */
            __HAL_RCC_SPI2_CLK_DISABLE();

            /**SPI2 GPIO Configuration
            PI1     ------> SPI2_SCK
            PI2     ------> SPI2_MISO
            PI3     ------> SPI2_MOSI
            */
            HAL_GPIO_DeInit(GPIOI, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

            /* USER CODE BEGIN SPI2_MspDeInit 1 */

            /* USER CODE END SPI2_MspDeInit 1 */
        }
        else
            if(spiHandle->Instance == SPI3)
            {
                /* USER CODE BEGIN SPI3_MspDeInit 0 */

                /* USER CODE END SPI3_MspDeInit 0 */

                /* Peripheral clock disable */
                __HAL_RCC_SPI3_CLK_DISABLE();

                /**SPI3 GPIO Configuration
                PC10     ------> SPI3_SCK
                PC11     ------> SPI3_MISO
                PC12     ------> SPI3_MOSI
                */
                HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);

                /* USER CODE BEGIN SPI3_MspDeInit 1 */

                /* USER CODE END SPI3_MspDeInit 1 */
            }
            else
                if(spiHandle->Instance == SPI4)
                {
                    /* USER CODE BEGIN SPI4_MspDeInit 0 */

                    /* USER CODE END SPI4_MspDeInit 0 */

                    /* Peripheral clock disable */
                    __HAL_RCC_SPI4_CLK_DISABLE();

                    /**SPI4 GPIO Configuration
                    PE12     ------> SPI4_SCK
                    PE13     ------> SPI4_MISO
                    PE14     ------> SPI4_MOSI
                    */
                    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);

                    /* USER CODE BEGIN SPI4_MspDeInit 1 */

                    /* USER CODE END SPI4_MspDeInit 1 */
                }
                else
                    if(spiHandle->Instance == SPI5)
                    {
                        /* USER CODE BEGIN SPI5_MspDeInit 0 */

                        /* USER CODE END SPI5_MspDeInit 0 */

                        /* Peripheral clock disable */
                        //      __HAL_RCC_SPI5_CLK_DISABLE();

                        /**SPI5 GPIO Configuration
                        PF7     ------> SPI5_SCK
                        PF8     ------> SPI5_MISO
                        PF9     ------> SPI5_MOSI
                        */
                        //      HAL_GPIO_DeInit(GPIOF, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);

                        /* USER CODE BEGIN SPI5_MspDeInit 1 */

                        /* USER CODE END SPI5_MspDeInit 1 */
                    }
                    else
                        if(spiHandle->Instance == SPI6)
                        {
                            /* USER CODE BEGIN SPI6_MspDeInit 0 */

                            /* USER CODE END SPI6_MspDeInit 0 */

                            /* Peripheral clock disable */
                            __HAL_RCC_SPI6_CLK_DISABLE();

                            /**SPI6 GPIO Configuration
                            PG12     ------> SPI6_MISO
                            PG13     ------> SPI6_SCK
                            PG14     ------> SPI6_MOSI
                            */
                            HAL_GPIO_DeInit(GPIOG, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);

                            /* USER CODE BEGIN SPI6_MspDeInit 1 */

                            /* USER CODE END SPI6_MspDeInit 1 */
                        }
}

/* USER CODE BEGIN 1 */
void SPI1_DMA_init()     
{   
    __HAL_RCC_DMA2_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    /* DMA2_Stream5_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
}

void SPI6_DMA_init()     
{   
    __HAL_RCC_DMA2_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    /* DMA2_Stream5_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
}
/* USER CODE END 1 */

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
