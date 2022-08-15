/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_spi.h
** Author      : Cai Heng
** Date        : 2021-07-23
** Description : 
**********************************************************************************************************/

#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "stm32f4xx.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/


#ifndef DISABLE_CFG
#define DISABLE_CFG			0							//ʧ��
#endif
#ifndef ENABLE_CFG
#define ENABLE_CFG			1							//ʹ��
#endif

#define	SPI1_CFG			ENABLE_CFG					//����SPI1 (Configurable) ����W5500
#define	SPI2_CFG			ENABLE_CFG					//����SPI2 (Configurable) ����AD7124
#define	SPI3_CFG			ENABLE_CFG					//����SPI3 (Configurable) ����AD7175
#define	SPI4_CFG			ENABLE_CFG					//����SPI4 (Configurable) ����DAC80508_1
#define	SPI5_CFG			ENABLE_CFG					//����SPI5 (Configurable) ����DAC80508_2
#define	SPI6_CFG			ENABLE_CFG					//����SPI6 (Configurable) ����FRAM

/* SPI1 Configuration */
/* Used by W5500 */
#if (SPI1_CFG == ENABLE_CFG)
#define SPI1_GPIO_CLK_ALLENABLE() {		\
		__HAL_RCC_GPIOB_CLK_ENABLE();	\
		__HAL_RCC_GPIOI_CLK_ENABLE();	\
	};													//SPI1��Ӧ��GPIOʱ��ʹ�� (Configurable)

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************

HW : JN-MPU-C022F ��SPI1 W5500	ӳ���ϵ
SPI1 ����//����ģʽ  //ȫ˫�� //����λΪ8λ  //�������NSS //4��Ƶ��11.25MHz

SPI1 PIN��
		PB3	  	------> LAN_SCLK
		PB4 	------> LAN_MISO
		PB5 	------> LAN_MOSI 
		PI6 	------>	LAN_CS
		PI7 	------> LAN_RST
						
*********************************************************************************************************/

#define SPI1_AF_MODE		GPIO_MODE_AF_PP			
#define SPI1_AF_PULL		GPIO_PULLUP
#define SPI1_AF_SPEED		GPIO_SPEED_FAST				//SPI1����IO����  (Configurable)
	
#define SPI1_GP_MODE		GPIO_MODE_OUTPUT_PP		
#define SPI1_GP_PULL		GPIO_PULLUP				
#define SPI1_GP_SPEED		GPIO_SPEED_FREQ_HIGH		//SPI1��ͨIO����  (Configurable)
	
#define SPI1_SCLK_PORT		GPIOB
#define SPI1_SCLK_PIN		GPIO_PIN_3					//SPI1_SCLK : PB3 (Configurable)
	
#define SPI1_MISO_PORT		GPIOB
#define SPI1_MISO_PIN		GPIO_PIN_4					//SPI1_MISO : PB4 (Configurable)
	
#define SPI1_MOSI_PORT		GPIOB
#define SPI1_MOSI_PIN		GPIO_PIN_5					//SPI1_MOSI : PB5 (Configurable)

#define SPI1_CS_PORT		GPIOI
#define SPI1_CS_PIN			GPIO_PIN_6					//SPI1_CS   : PI6 (Configurable)

#define SPI1_CS_ENABLE()			HAL_GPIO_WritePin(SPI1_CS_PORT, SPI1_CS_PIN, GPIO_PIN_RESET);
#define SPI1_CS_DISABLE()			HAL_GPIO_WritePin(SPI1_CS_PORT, SPI1_CS_PIN, GPIO_PIN_SET);

#define SPI1_REG_CONFIG() {																																\
		SPI1_Handler.Instance = SPI1;									/* SPI1                                                                     */	\
		SPI1_Handler.Init.Mode = SPI_MODE_MASTER;             			/* ����SPI����ģʽ������Ϊ��ģʽ                                            */	\
		SPI1_Handler.Init.Direction = SPI_DIRECTION_2LINES;   			/* ����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ                          */	\
		SPI1_Handler.Init.DataSize = SPI_DATASIZE_8BIT;       			/* ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ                                   */	\
		SPI1_Handler.Init.CLKPolarity = SPI_POLARITY_LOW;    			/* ����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ                                           */	\
		SPI1_Handler.Init.CLKPhase = SPI_PHASE_1EDGE;					/* ����ͬ��ʱ�ӵĵ�һ�������أ��������½������ݱ�����                       */	\
		SPI1_Handler.Init.NSS = SPI_NSS_SOFT;                 			/* NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� */	\
		SPI1_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;	/* ���岨����Ԥ��Ƶ��ֵ                                                     */	\
		SPI1_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;        			/* ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ                     */	\
		SPI1_Handler.Init.TIMode = SPI_TIMODE_DISABLE;        			/* �ر�TIģʽ                                                               */	\
		SPI1_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;	/* �ر�Ӳ��CRCУ��                                                          */	\
		SPI1_Handler.Init.CRCPolynomial = 10;                  			/* CRCֵ����Ķ���ʽ                                                        */	\
	};													//SPI1�Ĵ�������  (Configurable)
#endif /* (SPI1_CFG == ENABLE) */
	
/* SPI2 Configuration */
/* Used by AD7124 */
#if (SPI2_CFG == ENABLE_CFG)
#define SPI2_GPIO_CLK_ALLENABLE() {		\
		__HAL_RCC_GPIOI_CLK_ENABLE();	\
	};													//SPI2��Ӧ��GPIOʱ��ʹ�� (Configurable)
	
/*********************************************************************************************************
**                                            �궨����
**********************************************************************************************************

HW : JN-MPU-C022F ��SPI2    ADC2 AD7124	ӳ���ϵ
SPI2 ����//����ģʽ  //ȫ˫�� //����λΪ8λ  //�������NSS //16��Ƶ��2.81MHz

SPI2 PIN��
		PI0     ------> ADC2_NSS
		PI1     ------> ADC2_SCLK
		PI2     ------> ADC2_MISO
		PI3     ------> ADC2_MOS
						
*********************************************************************************************************/

#define SPI2_AF_MODE		GPIO_MODE_AF_PP			
#define SPI2_AF_PULL		GPIO_PULLUP
#define SPI2_AF_SPEED		GPIO_SPEED_FAST				//SPI2����IO����  (Configurable)
	
#define SPI2_GP_MODE		GPIO_MODE_OUTPUT_PP		
#define SPI2_GP_PULL		GPIO_PULLUP				
#define SPI2_GP_SPEED		GPIO_SPEED_FREQ_HIGH		//SPI2��ͨIO����  (Configurable)	
	
#define SPI2_SCLK_PORT		GPIOI
#define SPI2_SCLK_PIN		GPIO_PIN_1					//SPI2_SCLK : PI1 (Configurable)
	
#define SPI2_MISO_PORT		GPIOI
#define SPI2_MISO_PIN		GPIO_PIN_2					//SPI2_MISO : PI2 (Configurable)
	
#define SPI2_MOSI_PORT		GPIOI
#define SPI2_MOSI_PIN		GPIO_PIN_3					//SPI2_MOSI : PI3 (Configurable)

#define SPI2_CS_PORT		GPIOI
#define SPI2_CS_PIN			GPIO_PIN_0					//SPI2_CS   : PI0 (Configurable)

#define SPI2_CS_ENABLE()			HAL_GPIO_WritePin(SPI2_CS_PORT, SPI2_CS_PIN, GPIO_PIN_RESET);
#define SPI2_CS_DISABLE()			HAL_GPIO_WritePin(SPI2_CS_PORT, SPI2_CS_PIN, GPIO_PIN_SET);	

#define SPI2_REG_CONFIG() {																																\
		SPI2_Handler.Instance = SPI2;									/* SPI2                                                                     */	\
		SPI2_Handler.Init.Mode = SPI_MODE_MASTER;             			/* ����SPI����ģʽ������Ϊ��ģʽ                                            */	\
		SPI2_Handler.Init.Direction = SPI_DIRECTION_2LINES;   			/* ����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ                          */	\
		SPI2_Handler.Init.DataSize = SPI_DATASIZE_8BIT;       			/* ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ                                   */	\
		SPI2_Handler.Init.CLKPolarity = SPI_POLARITY_LOW;    			/* ����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ                                           */	\
		SPI2_Handler.Init.CLKPhase = SPI_PHASE_1EDGE;					/* ����ͬ��ʱ�ӵĵ�һ�������أ��������½������ݱ�����                       */	\
		SPI2_Handler.Init.NSS = SPI_NSS_SOFT;                 			/* NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� */	\
		SPI2_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;	/* ���岨����Ԥ��Ƶ��ֵ                                                     */	\
		SPI2_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;        			/* ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ                     */	\
		SPI2_Handler.Init.TIMode = SPI_TIMODE_DISABLE;        			/* �ر�TIģʽ                                                               */	\
		SPI2_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;	/* �ر�Ӳ��CRCУ��                                                          */	\
		SPI2_Handler.Init.CRCPolynomial = 10;                  			/* CRCֵ����Ķ���ʽ                                                        */	\
	};													//SPI2�Ĵ�������  (Configurable)
#endif /* (SPI2_CFG == ENABLE_CFG) */
	
/* SPI3 Configuration */
/* Used by AD7175 */
#if (SPI3_CFG == ENABLE_CFG)
#define SPI3_GPIO_CLK_ALLENABLE() {		\
		__HAL_RCC_GPIOC_CLK_ENABLE();	\
		__HAL_RCC_GPIOD_CLK_ENABLE();	\
	};													//SPI3��Ӧ��GPIOʱ��ʹ�� (Configurable)
	
/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************

HW : JN-MPU-C022F ��SPI3 ADC1  AD7175	ӳ���ϵ
SPI3 ����//����ģʽ  //ȫ˫�� //����λΪ8λ  //�������NSS //4��Ƶ��11.25MHz

SPI3  PIN��
		PD0      ------> ADC1_CS
		PC10     ------> ADC1_SCLK
		PC11     ------> ADC1_MISO
		PC12     ------> ADC1_MOSI
						
*********************************************************************************************************/

#define SPI3_AF_MODE		GPIO_MODE_AF_PP			
#define SPI3_AF_PULL		GPIO_PULLUP
#define SPI3_AF_SPEED		GPIO_SPEED_FAST				//SPI3����IO����  (Configurable)
	
#define SPI3_GP_MODE		GPIO_MODE_OUTPUT_PP		
#define SPI3_GP_PULL		GPIO_PULLUP				
#define SPI3_GP_SPEED		GPIO_SPEED_FREQ_HIGH		//SPI3��ͨIO����  (Configurable)	
	
#define SPI3_SCLK_PORT		GPIOC
#define SPI3_SCLK_PIN		GPIO_PIN_10					//SPI3_SCLK : PC10 (Configurable)
	
#define SPI3_MISO_PORT		GPIOC
#define SPI3_MISO_PIN		GPIO_PIN_11					//SPI3_MISO : PC11 (Configurable)
	
#define SPI3_MOSI_PORT		GPIOC
#define SPI3_MOSI_PIN		GPIO_PIN_12					//SPI3_MOSI : PC12 (Configurable)

#define SPI3_CS_PORT		GPIOD
#define SPI3_CS_PIN			GPIO_PIN_0					//SPI3_CS   : PD0  (Configurable)

#define SPI3_CS_ENABLE()			HAL_GPIO_WritePin(SPI3_CS_PORT, SPI3_CS_PIN, GPIO_PIN_RESET);
#define SPI3_CS_DISABLE()			HAL_GPIO_WritePin(SPI3_CS_PORT, SPI3_CS_PIN, GPIO_PIN_SET);	

#define SPI3_REG_CONFIG() {																																\
		SPI3_Handler.Instance = SPI3;									/* SPI3                                                                     */	\
		SPI3_Handler.Init.Mode = SPI_MODE_MASTER;             			/* ����SPI����ģʽ������Ϊ��ģʽ                                            */	\
		SPI3_Handler.Init.Direction = SPI_DIRECTION_2LINES;   			/* ����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ                          */	\
		SPI3_Handler.Init.DataSize = SPI_DATASIZE_8BIT;       			/* ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ                                   */	\
		SPI3_Handler.Init.CLKPolarity = SPI_POLARITY_LOW;    			/* ����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ                                           */	\
		SPI3_Handler.Init.CLKPhase = SPI_PHASE_1EDGE;					/* ����ͬ��ʱ�ӵĵ�һ�������أ��������½������ݱ�����                       */	\
		SPI3_Handler.Init.NSS = SPI_NSS_SOFT;                 			/* NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� */	\
		SPI3_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;	/* ���岨����Ԥ��Ƶ��ֵ                                                     */	\
		SPI3_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;        			/* ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ                     */	\
		SPI3_Handler.Init.TIMode = SPI_TIMODE_DISABLE;        			/* �ر�TIģʽ                                                               */	\
		SPI3_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;	/* �ر�Ӳ��CRCУ��                                                          */	\
		SPI3_Handler.Init.CRCPolynomial = 10;                  			/* CRCֵ����Ķ���ʽ                                                        */	\
	};													//SPI3�Ĵ�������  (Configurable)
#endif /* (SPI3_CFG == ENABLE) */
	
/* SPI4 Configuration */

#if (SPI4_CFG == ENABLE_CFG)
#define SPI4_GPIO_CLK_ALLENABLE() {		\
		__HAL_RCC_GPIOE_CLK_ENABLE();	\
	};													//SPI4��Ӧ��GPIOʱ��ʹ�� (Configurable)
	
/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************

HW : JN-MPU-C022F ��SPI4 DAC1 DAC80508	ӳ���ϵ
SPI4 ����//����ģʽ  //ȫ˫�� //����λΪ8λ  //�������NSS //2��Ƶ��25MHz

SPI4  PIN��
				PE11     ------> DAC1_CS
				PE12     ------> DAC1_SCK
				PE13     ------> DAC1_MISO
				PE14     ------> DAC1_MOSI
						
*********************************************************************************************************/

#define SPI4_AF_MODE		GPIO_MODE_AF_PP			
#define SPI4_AF_PULL		GPIO_PULLUP
#define SPI4_AF_SPEED		GPIO_SPEED_FREQ_VERY_HIGH	//SPI4����IO����   (Configurable)
	
#define SPI4_GP_MODE		GPIO_MODE_OUTPUT_PP		
#define SPI4_GP_PULL		GPIO_NOPULL				
#define SPI4_GP_SPEED		GPIO_SPEED_HIGH				//SPI4��ͨIO����   (Configurable)	
	
#define SPI4_SCLK_PORT		GPIOE
#define SPI4_SCLK_PIN		GPIO_PIN_12					//SPI4_SCLK : PE12 (Configurable)
	
#define SPI4_MISO_PORT		GPIOE
#define SPI4_MISO_PIN		GPIO_PIN_13					//SPI4_MISO : PE13 (Configurable)
	
#define SPI4_MOSI_PORT		GPIOE
#define SPI4_MOSI_PIN		GPIO_PIN_14					//SPI4_MOSI : PE14 (Configurable)

#define SPI4_CS_PORT		GPIOE
#define SPI4_CS_PIN			GPIO_PIN_11					//SPI4_CS   : PE11 (Configurable)

#define SPI4_CS_ENABLE()	HAL_GPIO_WritePin(SPI4_CS_PORT, SPI4_CS_PIN, GPIO_PIN_RESET);
#define SPI4_CS_DISABLE()	HAL_GPIO_WritePin(SPI4_CS_PORT, SPI4_CS_PIN, GPIO_PIN_SET);	

#define SPI4_REG_CONFIG() {																																\
		SPI4_Handler.Instance = SPI4;									/* SPI4                                                                     */	\
		SPI4_Handler.Init.Mode = SPI_MODE_MASTER;             			/* ����SPI����ģʽ������Ϊ��ģʽ                                            */	\
		SPI4_Handler.Init.Direction = SPI_DIRECTION_2LINES;   			/* ����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ                          */	\
		SPI4_Handler.Init.DataSize = SPI_DATASIZE_8BIT;       			/* ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ                                   */	\
		SPI4_Handler.Init.CLKPolarity = SPI_POLARITY_HIGH;    			/* ����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ                                           */	\
		SPI4_Handler.Init.CLKPhase = SPI_PHASE_1EDGE;					/* ����ͬ��ʱ�ӵĵ�һ�������أ��������½������ݱ�����                       */	\
		SPI4_Handler.Init.NSS = SPI_NSS_SOFT;                 			/* NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� */	\
		SPI4_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;	/* ���岨����Ԥ��Ƶ��ֵ                                                     */	\
		SPI4_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;        			/* ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ                     */	\
		SPI4_Handler.Init.TIMode = SPI_TIMODE_DISABLE;        			/* �ر�TIģʽ                                                               */	\
		SPI4_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;	/* �ر�Ӳ��CRCУ��                                                          */	\
		SPI4_Handler.Init.CRCPolynomial = 10;                  			/* CRCֵ����Ķ���ʽ                                                        */	\
	};													//SPI4�Ĵ�������  (Configurable)
#endif /* (SPI4_CFG == ENABLE_CFG) */
	
/* SPI5 Configuration */

#if (SPI5_CFG == ENABLE_CFG)
#define SPI5_GPIO_CLK_ALLENABLE() {		\
		__HAL_RCC_GPIOF_CLK_ENABLE();	\
	};													//SPI5��Ӧ��GPIOʱ��ʹ�� (Configurable)
	
/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************

HW : JN-MPU-C022F ��SPI5 DAC2 DAC80508	ӳ���ϵ
SPI5 ����//����ģʽ  //ȫ˫�� //����λΪ8λ  //�������NSS //2��Ƶ��25MHz

SPI5  PIN��
			PF7     ------> DAC2_SCK
			PF8     ------> DAC2_MISO
			PF9     ------> DAC2_MOSI
			PF10    ------> DAC2_CS
						
*********************************************************************************************************/

#define SPI5_AF_MODE		GPIO_MODE_AF_PP			
#define SPI5_AF_PULL		GPIO_PULLUP
#define SPI5_AF_SPEED		GPIO_SPEED_FREQ_VERY_HIGH	//SPI5����IO����   (Configurable)
	
#define SPI5_GP_MODE		GPIO_MODE_OUTPUT_PP		
#define SPI5_GP_PULL		GPIO_NOPULL				
#define SPI5_GP_SPEED		GPIO_SPEED_HIGH				//SPI5��ͨIO����   (Configurable)
	
#define SPI5_SCLK_PORT		GPIOF
#define SPI5_SCLK_PIN		GPIO_PIN_7					//SPI5_SCLK : PF7  (Configurable)
	
#define SPI5_MISO_PORT		GPIOF
#define SPI5_MISO_PIN		GPIO_PIN_8					//SPI5_MISO : PF8  (Configurable)
	
#define SPI5_MOSI_PORT		GPIOF
#define SPI5_MOSI_PIN		GPIO_PIN_9					//SPI5_MOSI : PF9  (Configurable)

#define SPI5_CS_PORT		GPIOF
#define SPI5_CS_PIN			GPIO_PIN_10					//SPI5_CS   : PF10 (Configurable)

#define SPI5_CS_ENABLE()	HAL_GPIO_WritePin(SPI5_CS_PORT, SPI5_CS_PIN, GPIO_PIN_RESET);
#define SPI5_CS_DISABLE()	HAL_GPIO_WritePin(SPI5_CS_PORT, SPI5_CS_PIN, GPIO_PIN_SET);	

#define SPI5_REG_CONFIG() {																																\
		SPI5_Handler.Instance = SPI5;									/* SPI5                                                                     */	\
		SPI5_Handler.Init.Mode = SPI_MODE_MASTER;             			/* ����SPI����ģʽ������Ϊ��ģʽ                                            */	\
		SPI5_Handler.Init.Direction = SPI_DIRECTION_2LINES;   			/* ����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ                          */	\
		SPI5_Handler.Init.DataSize = SPI_DATASIZE_8BIT;       			/* ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ                                   */	\
		SPI5_Handler.Init.CLKPolarity = SPI_POLARITY_HIGH;    			/* ����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ                                           */	\
		SPI5_Handler.Init.CLKPhase = SPI_PHASE_1EDGE;					/* ����ͬ��ʱ�ӵĵ�һ�������أ��������½������ݱ�����                       */	\
		SPI5_Handler.Init.NSS = SPI_NSS_SOFT;                 			/* NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� */	\
		SPI5_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;	/* ���岨����Ԥ��Ƶ��ֵ                                                     */	\
		SPI5_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;        			/* ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ                     */	\
		SPI5_Handler.Init.TIMode = SPI_TIMODE_DISABLE;        			/* �ر�TIģʽ                                                               */	\
		SPI5_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;	/* �ر�Ӳ��CRCУ��                                                          */	\
		SPI5_Handler.Init.CRCPolynomial = 10;                  			/* CRCֵ����Ķ���ʽ                                                        */	\
	};													//SPI5�Ĵ�������  (Configurable)
#endif /* (SPI5_CFG == ENABLE_CFG) */
	
/* SPI6 Configuration */
/* Used by FLASH */
#define SPI6_GPIO_CLK_ALLENABLE() {		\
		__HAL_RCC_GPIOG_CLK_ENABLE();   \
        __HAL_RCC_GPIOB_CLK_ENABLE();	\
	};													//SPI6��Ӧ��GPIOʱ��ʹ�� (Configurable)

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************

HW : JN-MPU-C022F ��SPI6 FRAM	ӳ���ϵ
SPI6 ����//����ģʽ  //ȫ˫�� //����λΪ8λ  //�������NSS //4��Ƶ��22.5MHz

SPI6  PIN��
			PG11     ------> FLASH_CS
			PG12     ------> FLASH_MISO
			PG13     ------> FLASH_SCK
			PG14     ------> FLASH_MOSI 
						
*********************************************************************************************************/

#define SPI6_AF_MODE		GPIO_MODE_AF_PP			
#define SPI6_AF_PULL		GPIO_PULLUP
#define SPI6_AF_SPEED		GPIO_SPEED_FREQ_VERY_HIGH	//SPI6����IO����  (Configurable)
	
#define SPI6_GP_MODE		GPIO_MODE_OUTPUT_PP		
#define SPI6_GP_PULL		GPIO_NOPULL				
#define SPI6_GP_SPEED		GPIO_SPEED_HIGH				//SPI6��ͨIO����  (Configurable)	
	
#define SPI6_SCLK_PORT		GPIOG
#define SPI6_SCLK_PIN		GPIO_PIN_13					//SPI6_SCLK : PG13(Configurable)
	
#define SPI6_MISO_PORT		GPIOG
#define SPI6_MISO_PIN		GPIO_PIN_12					//SPI6_MISO : PG12(Configurable)
	
#define SPI6_MOSI_PORT		GPIOG
#define SPI6_MOSI_PIN		GPIO_PIN_14					//SPI6_MOSI : PG14(Configurable)

#define FLASH_CS_PORT	  	GPIOG
#define FLASH_CS_PIN		GPIO_PIN_11					//FLASH_CS(@SPI6)   : PG11(Configurable)	

#define FRAM_CS_PORT	  	GPIOB
#define FRAM_CS_PIN		    GPIO_PIN_15					//FRAM_CS(@SPI6)   : PB15(Configurable)	

#define FLASH_CS_ENABLE()	HAL_GPIO_WritePin(FRAM_CS_PORT, FRAM_CS_PIN, GPIO_PIN_SET);HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET)
#define FLASH_CS_DISABLE()	HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET)

#define FRAM_CS_ENABLE()	HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);HAL_GPIO_WritePin(FRAM_CS_PORT, FRAM_CS_PIN, GPIO_PIN_RESET)
#define FRAM_CS_DISABLE()	HAL_GPIO_WritePin(FRAM_CS_PORT, FRAM_CS_PIN, GPIO_PIN_SET)

#define SPI6_REG_CONFIG() {																																\
		SPI6_Handler.Instance = SPI6;									/* SPI6                                                                     */	\
		SPI6_Handler.Init.Mode = SPI_MODE_MASTER;             			/* ����SPI����ģʽ������Ϊ��ģʽ                                            */	\
		SPI6_Handler.Init.Direction = SPI_DIRECTION_2LINES;   			/* ����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ                          */	\
		SPI6_Handler.Init.DataSize = SPI_DATASIZE_8BIT;       			/* ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ                                   */	\
		SPI6_Handler.Init.CLKPolarity = SPI_POLARITY_LOW;    			/* ����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ                                           */	\
		SPI6_Handler.Init.CLKPhase = SPI_PHASE_1EDGE;					/* ����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����                       */	\
		SPI6_Handler.Init.NSS = SPI_NSS_SOFT;                 			/* NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� */	\
		SPI6_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;	/* ���岨����Ԥ��Ƶ��ֵ                                                     */	\
		SPI6_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;        			/* ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ                     */	\
		SPI6_Handler.Init.TIMode = SPI_TIMODE_DISABLE;        			/* �ر�TIģʽ                                                               */	\
		SPI6_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;	/* �ر�Ӳ��CRCУ��                                                          */	\
		SPI6_Handler.Init.CRCPolynomial = 10;                  			/* CRCֵ����Ķ���ʽ                                                        */	\
	};													//SPI6�Ĵ�������  (Configurable)

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/


/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
extern SPI_HandleTypeDef SPI2_Handler;	
extern SPI_HandleTypeDef SPI3_Handler;
extern SPI_HandleTypeDef SPI4_Handler;	
extern SPI_HandleTypeDef SPI5_Handler;	
	
/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
extern void bsp_InitSpi1(void);
extern void bsp_InitSpi2(void);
extern void bsp_InitSpi3(void);
extern void bsp_InitSpi4(void);
extern void bsp_InitSpi5(void);	

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
