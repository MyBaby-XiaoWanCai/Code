/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : ad7606.h
** Author      : wangliang
** Date        : 2021-08-23
** Description : ad7606 head file
**********************************************************************************************************/

#ifndef __BSP_AD7606_H
#define __BSP_AD7606_H

#ifdef __cplusplus
extern "C" {
#endif
/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#define AD7606_Busy0_Port								 GPIO_PORT_A
#define AD7606_Busy0_Pin                 GPIO_PIN_15
/* 定义中断通道 Busy0 */
#define AD7606_Busy0_EXINT_CH            (EXINT_CH15)
#define AD7606_Busy0_INT_SRC             (INT_PORT_EIRQ15)
#define AD7606_Busy0_IRQn                (Int015_IRQn)

/*ADC忙信号，2片7606公用*/
#define AD7606_Busy_Port                 GPIO_PORT_E   
#define AD7606_Busy_Pin                  GPIO_PIN_00 

/*ADC数据输入，2片7606公用*/
#define AD7606_Din_Port                  GPIO_PORT_C
#define AD7606_Din_Pin                   GPIO_PIN_12

/*ADC数据输出，2片7606公用*/           
#define AD7606_Dout_Port                 GPIO_PORT_C
#define AD7606_Dout_Pin                  GPIO_PIN_11

/*ADC数据时钟，2片7606公用*/
#define AD7606_Clk_Port                  GPIO_PORT_C
#define AD7606_Clk_Pin                   GPIO_PIN_10

/*ADC0片选信号*/
#define A0_Cs_Port                       GPIO_PORT_D
#define A0_Cs_Pin                        GPIO_PIN_00

/*ADC0复位信号*/
#define A0_Reset_Port                    GPIO_PORT_F
#define A0_Reset_Pin                     GPIO_PIN_11

/*ADC0转换信号*/
#define A0_Cov_Port                      GPIO_PORT_F
#define A0_Cov_Pin                       GPIO_PIN_12

/*ADC1片选信号*/
#define A1_Cs_Port                       GPIO_PORT_D
#define A1_Cs_Pin                        GPIO_PIN_01

/*ADC0复位信号*/
#define A1_Reset_Port                    GPIO_PORT_F
#define A1_Reset_Pin                     GPIO_PIN_13

/*ADC1转换信号*/
#define A1_Cov_Port                      GPIO_PORT_F
#define A1_Cov_Pin                       GPIO_PIN_14

/*定义中断通道*/
#define AD7606_EXINT_CH                  (EXINT_CH00)
#define AD7606_INT_SRC                   (INT_PORT_EIRQ0)
#define AD7606_IRQn                      (Int001_IRQn)

/*-------------------------控制信号宏操作-----------------------*/
/*A0——CS信号宏操作*/
#define A0_CS_1()          GPIO_SetPins(A0_Cs_Port, A0_Cs_Pin)
#define A0_CS_0()          GPIO_ResetPins(A0_Cs_Port, A0_Cs_Pin)

/*A1——CS信号宏操作*/
#define A1_CS_1()          GPIO_SetPins(A1_Cs_Port, A1_Cs_Pin)
#define A1_CS_0()          GPIO_ResetPins(A1_Cs_Port, A1_Cs_Pin)

/*A0——REST信号宏操作*/
#define A0_REST_1()        GPIO_SetPins(A0_Reset_Port, A0_Reset_Pin)
#define A0_REST_0()        GPIO_ResetPins(A0_Reset_Port, A0_Reset_Pin)

/*A1——REST信号宏操作*/
#define A1_REST_1()        GPIO_SetPins(A1_Reset_Port, A1_Reset_Pin)
#define A1_REST_0()        GPIO_ResetPins(A1_Reset_Port, A1_Reset_Pin)

/*A0——CNV信号宏操作*/
#define A0_CNV_1()        GPIO_SetPins(A0_Cov_Port, A0_Cov_Pin)
#define A0_CNV_0()        GPIO_ResetPins(A0_Cov_Port, A0_Cov_Pin)

/*A1——CNV信号宏操作*/
#define A1_CNV_1()        GPIO_SetPins(A1_Cov_Port, A1_Cov_Pin)
#define A1_CNV_0()        GPIO_ResetPins(A1_Cov_Port, A1_Cov_Pin)

/*AD7606 SPI接口配置*/

#define AD7606_SPI_UNIT         (M4_SPI3)
#define AD7606_UNIT_CLOCK       (PWC_FCG1_SPI3)

#define AD7606_SCK_PORT         (GPIO_PORT_C)
#define AD7606_SCK_PIN          (GPIO_PIN_10)
#define AD7606_SCK_FUNC         (GPIO_FUNC_19_SPI3_SCK)
#define AD7606_MISO_PORT        (GPIO_PORT_C)
#define AD7606_MISO_PIN         (GPIO_PIN_11)
#define AD7606_MISO_FUNC        (GPIO_FUNC_19_SPI3_MISO)
#define AD7606_MOSI_PORT        (GPIO_PORT_C)
#define AD7606_MOSI_PIN         (GPIO_PIN_12) 
#define AD7606_MOSI_FUNC        (GPIO_FUNC_19_SPI3_MOSI)

#define AD7606_CH               (8U)        //转换通道数
/**********************************************************************************************************
**                                            类型申明区
**********************************************************************************************************/
typedef enum AD7606_Type
{
	AD7606_Vsense=1,
	AD7606_Iout,
	NoBody   
}AD7606_Sel;

typedef struct
{
	AD7606_Sel Type;             //对象类型，1表示用于Vsense测量，2表示用于Iout测量
	int16_t ADC[AD7606_CH];     //通道数据缓存
	uint16_t Sample_NUM;         //平均采样次数    
	int32_t SUM_ADC[AD7606_CH]; //历史累加值    
	int32_t AVG_ADC[AD7606_CH]; //通道数据平均计算值  
}AD7606_Object;

extern AD7606_Object AD7606_A0;
extern AD7606_Object AD7606_A1;

#define SAMP_CNT			(1)

/**********************************************************************************************************
**                                            函数申明区
**********************************************************************************************************/   
void bsp_AD7606_init(void);
void AD7606Reset(AD7606_Object * OB);
uint32_t AD7606_ReadData(AD7606_Object * OB);
void AD7606_StartConV(AD7606_Object * OB);
void AD7606_Process(AD7606_Object * OB , uint16_t AVG_num);

#ifdef __cplusplus
}
#endif	
	
#endif


