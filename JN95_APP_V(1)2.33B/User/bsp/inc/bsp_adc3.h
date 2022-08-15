/**********************************************************************************************************
*
*	ģ������ :  adc3 ������
*	�ļ����� : bsp_adc3.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*	��	 �ߣ�Robert.Li
**��	 ��:  2021.7.7
**********************************************************************************************************/

#ifndef _BSP_ADC3_H_
#define _BSP_ADC3_H_

#ifdef __cplusplus
extern "C" {
#endif





/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "stm32f4xx.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************

HW : JN-MPU-C022F ��ADC3	ӳ���ϵ
ADC3 ���ò���ͨ������ �� /������� 12λģʽ ɨ��ģʽ ����ת�� ���������3Cycle �������� ����ģʽ�ģͣ�������ʽ����DMA2��			

ADC3 init Analog PIN��
		PF3     ------> ADC3_IN9   AD_BUS1			AD_BUS1 ���ߵ�ѹ1��ACDC1��	
		PF4     ------> ADC3_IN14  AD_BUS2			AD_BUS2 ���ߵ�ѹ2��ACDC2��
		PF5     ------> ADC3_IN15  AD_VMOD17		AD_VMOD ģ���ѹ������ѭ��Switch I/O�л���
		PF6     ------> ADC3_IN4   AD_VMOD28		
		PC0     ------> ADC3_IN10  AD_Temp17		AD_Temp ģ���¶� ������ѭ�� Switch I/O �л���
		PC2     ------> ADC3_IN12  AD_Temp28
		PC3     ------> ADC3_IN13  AD_Aux				AD_AUX ������Դ
		PA3     ------> ADC3_IN3   AD_Temp			AD_Temp �����¶�


ADC Ƭѡ���� I/0  ���������ģʽ�������������١�ͨ������SN74LV4052APW �л�ͨ������ֵ��ģ���¶ȣ�ģ���ѹAD_VOD ����ֵ ����task ������25ms �л�һ�Σ�ִ�С�

ADC_switch I/O��
		PH2     ------> SN_A
		PH3     ------> SN_B
						
*********************************************************************************************************/
#define VOUT_FILTER_CFG			10
	
#define DMA_CH_samp         	100					//10
#define DMA_CH_samp_SUM			(8 * DMA_CH_samp)
	
	
/* ����GPIO�˿� */	
#define SNA_PORT		   		GPIOH
#define SNA_PIN		        	GPIO_PIN_2
	
#define SNB_PORT		    	GPIOH
#define SNB_PIN		        	GPIO_PIN_3
	
/* ���������0����1�ĺ� */	
#define SNA_0()					HAL_GPIO_WritePin(SNA_PORT, SNA_PIN, GPIO_PIN_RESET);
#define SNA_1()					HAL_GPIO_WritePin(SNA_PORT, SNA_PIN, GPIO_PIN_SET);
#define SNB_0()					HAL_GPIO_WritePin(SNB_PORT, SNB_PIN, GPIO_PIN_RESET);
#define SNB_1()					HAL_GPIO_WritePin(SNB_PORT, SNB_PIN, GPIO_PIN_SET);



/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/






/**********************************************************************************************************
**                                         �ⲿ���ñ�����
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
**                                         �ⲿ���ú�����
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
