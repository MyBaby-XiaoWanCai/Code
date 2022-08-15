/**********************************************************************************************************
*
*	模块名称 :  adc3 处理函数
*	文件名称 : bsp_adc3.h
*	版    本 : V1.0
*	说    明 : 头文件
*	作	 者：Robert.Li
**日	 期:  2021.7.7
**********************************************************************************************************/

#ifndef _BSP_ADC3_H_
#define _BSP_ADC3_H_

#ifdef __cplusplus
extern "C" {
#endif





/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "stm32f4xx.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************

HW : JN-MPU-C022F 　ADC3	映射关系
ADC3 配置采用通常配置 如 /软件触发 12位模式 扫描模式 连续转换 软件触发，3Cycle 采样周期 工作模式ＤＭＡ工作方式　（DMA2）			

ADC3 init Analog PIN：
		PF3     ------> ADC3_IN9   AD_BUS1			AD_BUS1 总线电压1（ACDC1）	
		PF4     ------> ADC3_IN14  AD_BUS2			AD_BUS2 总线电压2（ACDC2）
		PF5     ------> ADC3_IN15  AD_VMOD17		AD_VMOD 模块电压（任务循环Switch I/O切换）
		PF6     ------> ADC3_IN4   AD_VMOD28		
		PC0     ------> ADC3_IN10  AD_Temp17		AD_Temp 模块温度 （任务循环 Switch I/O 切换）
		PC2     ------> ADC3_IN12  AD_Temp28
		PC3     ------> ADC3_IN13  AD_Aux				AD_AUX 辅助电源
		PA3     ------> ADC3_IN3   AD_Temp			AD_Temp 环境温度


ADC 片选控制 I/0  　推挽输出模式，不上拉，高速。通过控制SN74LV4052APW 切换通道采样值，模块温度，模块电压AD_VOD 采样值 ，在task 任务中25ms 切换一次，执行。

ADC_switch I/O：
		PH2     ------> SN_A
		PH3     ------> SN_B
						
*********************************************************************************************************/
#define VOUT_FILTER_CFG			10
	
#define DMA_CH_samp         	100					//10
#define DMA_CH_samp_SUM			(8 * DMA_CH_samp)
	
	
/* 定义GPIO端口 */	
#define SNA_PORT		   		GPIOH
#define SNA_PIN		        	GPIO_PIN_2
	
#define SNB_PORT		    	GPIOH
#define SNB_PIN		        	GPIO_PIN_3
	
/* 定义口线置0和置1的宏 */	
#define SNA_0()					HAL_GPIO_WritePin(SNA_PORT, SNA_PIN, GPIO_PIN_RESET);
#define SNA_1()					HAL_GPIO_WritePin(SNA_PORT, SNA_PIN, GPIO_PIN_SET);
#define SNB_0()					HAL_GPIO_WritePin(SNB_PORT, SNB_PIN, GPIO_PIN_RESET);
#define SNB_1()					HAL_GPIO_WritePin(SNB_PORT, SNB_PIN, GPIO_PIN_SET);



/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/






/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern	ADC_HandleTypeDef hadc3;
extern	DMA_HandleTypeDef hdma_adc3;

extern uint16_t uhADCxConvertedValue12[DMA_CH_samp_SUM];
extern uint16_t uhADCxConvertedValue34[DMA_CH_samp_SUM];
extern uint16_t uhADCxConvertedValue56[DMA_CH_samp_SUM];
extern uint16_t uhADCxConvertedValue78[DMA_CH_samp_SUM];
// extern uint16_t AD_Amb;
// extern uint16_t AD_Aux;
// extern uint16_t AD_Bus1;
// extern uint16_t AD_Bus2;
// extern uint16_t ADCx_Vmod[8];
// extern uint16_t ADCx_Temp[8];
extern float AD_Amb;
extern float AD_Aux;
extern float AD_Bus1;
extern float AD_Bus2;
extern float ADCx_Vmod[8];
extern float ADCx_Temp[8];

	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
extern	void DMA_adc3_init(void);
extern void ADC_Switch_ConfigGPIO(void);
extern void ADC_Start_Stop_DMA(void);


#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
