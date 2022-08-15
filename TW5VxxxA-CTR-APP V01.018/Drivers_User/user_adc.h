/*
*********************************************************************************************************
*
*	模块名称 : user_adc
*	文件名称 : user_adc.h
*	版    本 : V1.0
*	说    明 :
*	修改记录 :
*	版本号  日期         作者       说明
*	V1.0    2020-03-10   ZYL        正式发布
*
*********************************************************************************************************
*/

#ifndef _user_adc_H_
#define _user_adc_H_

#include "stdint.h"

#define DMA_CH_samp         100//10
#define DMA_CH_samp_SUM     8*DMA_CH_samp

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

extern void ADC_Switch_ConfigGPIO(void);
extern void ADC_Start_Stop_DMA(void);


#endif

/***************************** (END OF FILE) *********************************/
