/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_adc3.h
** Author      : Huang Cheng
** Date        : 2021-11-10
** Description : bsp_adc3 head file
**********************************************************************************************************/

#ifndef _BSP_ADC3_H_
#define _BSP_ADC3_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include <string.h>
#include "hc32_ddl.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
/*
 * ADC unit instance for this example.
 * Marco 'APP_ADC_UNIT' can be defined as M4_ADCx(x=1, 2, 3).
 * Definition of 'APP_PERIP_CLK_ADC' depends on 'APP_ADC_UNIT'.
 */
#define APP_ADC_UNIT                        (M4_ADC3)
#define APP_ADC_PERIP_CLK                   (PWC_FCG3_ADC3)

/* 
 * ADC remaping channels definition for this example.
 * 'APP_ADC_REMAP_PINx' depends on 'APP_ADC_UNIT'
 * NOTE!!! Only channel 0 ~ channel 15 can be remaped.
 */
//#define APP_ADC_REMAP_PIN1                  (ADC123_IN3)
//#define APP_ADC_REMAP_PIN1_DEST_CH          (ADC_CH0)

//#define APP_ADC_REMAP_PIN2                  (ADC123_IN13)
//#define APP_ADC_REMAP_PIN2_DEST_CH          (ADC_CH1)

#define APP_ADC_REMAP_PIN3                  (ADC3_IN9)
#define APP_ADC_REMAP_PIN3_DEST_CH          (ADC_CH2)

#define APP_ADC_REMAP_PIN4                  (ADC3_IN14)
#define APP_ADC_REMAP_PIN4_DEST_CH          (ADC_CH3)

#define APP_ADC_REMAP_PIN5                  (ADC3_IN15)
#define APP_ADC_REMAP_PIN5_DEST_CH          (ADC_CH4)

#define APP_ADC_REMAP_PIN6                  (ADC3_IN4)
#define APP_ADC_REMAP_PIN6_DEST_CH          (ADC_CH5)

//#define APP_ADC_REMAP_PIN7                  (ADC123_IN10)
//#define APP_ADC_REMAP_PIN7_DEST_CH          (ADC_CH6)

//#define APP_ADC_REMAP_PIN8                  (ADC123_IN12)
//#define APP_ADC_REMAP_PIN8_DEST_CH          (ADC_CH7)

//#define APP_ADC_REMAP_CH                    (  APP_ADC_REMAP_PIN1_DEST_CH \
//                                             | APP_ADC_REMAP_PIN2_DEST_CH \
//																						 | APP_ADC_REMAP_PIN3_DEST_CH \
//																						 | APP_ADC_REMAP_PIN4_DEST_CH \
//																						 | APP_ADC_REMAP_PIN5_DEST_CH \
//																						 | APP_ADC_REMAP_PIN6_DEST_CH \
//											                       | APP_ADC_REMAP_PIN7_DEST_CH \
//											                       | APP_ADC_REMAP_PIN8_DEST_CH )
#define APP_ADC_REMAP_CH                    (  APP_ADC_REMAP_PIN3_DEST_CH \
																						 | APP_ADC_REMAP_PIN4_DEST_CH \
																						 | APP_ADC_REMAP_PIN5_DEST_CH \
																						 | APP_ADC_REMAP_PIN6_DEST_CH )

/*
 * Specifies the ADC channels according to the application.
 * NOTE!!! Sequence A and sequence B CANNOT contain the same channel.
 */
#define APP_ADC_SA_CH                       (APP_ADC_REMAP_CH)
#define APP_ADC_SA_CH_COUNT                 (8U)

#define APP_ADC_SA_CH_NUM_MIN               (ADC_CH_NUM_0)
#define APP_ADC_SA_CH_NUM_MAX               (ADC_CH_NUM_7)

/* Sampling time of ADC channels. */
/*                                          ADC_CH0        ~        ADC_CH7 */
#define APP_ADC_SA_SAMPLE_TIME              { 30, 30, 30, 30, 30, 30, 30, 30 }

/*
 * Add the channels which were included in sequence A or sequence B to average channel if needed.
 * The average channels will be sampled a specified number of times(specified by 'APP_ADC_AVG_CNT'),\
 *   and the final ADC value is the average of the specified number of samples.
 * Define 'APP_ADC_AVG_CH' as 0 to disable average channel.
 */
#define APP_ADC_AVG_CNT                     (ADC_AVG_CNT_128)	//ADC滤波次数 x 次
#define APP_ADC_AVG_CH                      (APP_ADC_SA_CH)

/*
 * Definitions of DMA.
 * 'APP_DMA_BLOCK_SIZE': 1~1024, inclusive. 1~16 for ADC1 and ADC2; 1~20 for ADC3.
 * 'APP_DMA_TRANS_COUNT': 0~65535, inclusive. 0: always transmit.
 */
#define APP_DMA_UNIT                        (M4_DMA2)
#define APP_DMA_CH                          (DMA_CH0)
#define APP_DMA_PERIP_CLK                   (PWC_FCG0_DMA2)
#define APP_DMA_BLOCK_SIZE                  (APP_ADC_SA_CH_NUM_MAX - APP_ADC_SA_CH_NUM_MIN + 1U)
#define APP_DMA_TRANS_COUNT                 (0U)
#define APP_DMA_DATA_WIDTH                  (DMA_DATAWIDTH_16BIT)
#define APP_DMA_TRIG_SRC                    (EVT_ADC3_EOCA)
#define APP_DMA_SRC_ADDR                    (&M4_ADC3->DR0)

#define APP_DMA_IRQ_SRC                     (INT_DMA2_BTC0)
//#define APP_DMA_IRQ_SRC                     (INT_DMA2_TC0)
#define APP_DMA_IRQn                        (Int010_IRQn)
#define APP_DMA_INT_PRIO                    (DDL_IRQ_PRIORITY_02)
#define APP_DMA_TRANS_CPLT_FLAG             (DMA_BTC_INT_CH0)
//#define APP_DMA_TRANS_CPLT_FLAG             (DMA_TC_INT_CH0)

#define DMA_CH_samp                         10					
#define DMA_CH_samp_SUM                     (DMA_CH_samp*(APP_DMA_BLOCK_SIZE))

#define ADC_START                           1U
#define ADC_STOP                            0U

/* 定义GPIO端口 */
#define SNA_PORT		                    	GPIO_PORT_H
#define SNA_PIN		                        GPIO_PIN_02

#define SNB_PORT		                    	GPIO_PORT_H
#define SNB_PIN		                        GPIO_PIN_03

/* 定义口线置0和置1的宏 */
#define SNA_0()		                        GPIO_ResetPins(SNA_PORT, SNA_PIN)
#define SNA_1()		                        GPIO_SetPins(SNA_PORT, SNA_PIN)
#define SNB_0()		                        GPIO_ResetPins(SNB_PORT, SNB_PIN)
#define SNB_1()		                        GPIO_SetPins(SNB_PORT, SNB_PIN)

#define SN_SET_CNT												(100)	//SN置位次数

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern float AD_Amb;
extern float AD_Aux;
extern float AD_Bus1;
extern float AD_Bus2;
extern float ADCx_Vmod[8];
//extern float ADCx_Temp[8];	

//测试
extern uint16_t m_au16AdcSaVal[APP_DMA_BLOCK_SIZE];
extern uint8_t m_u32AdcValUpdated;

extern uint16_t uhADCxConvertedValue12[DMA_CH_samp_SUM];
extern uint16_t uhADCxConvertedValue34[DMA_CH_samp_SUM];
extern uint16_t uhADCxConvertedValue56[DMA_CH_samp_SUM];
extern uint16_t uhADCxConvertedValue78[DMA_CH_samp_SUM];
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void DMA_adc3_init(void);
void ADC_Switch_ConfigGPIO(void);
void ADC_Start_Stop(M4_ADC_TypeDef *ADCx, uint8_t State);
void ADC_Start_Stop_DMA(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

