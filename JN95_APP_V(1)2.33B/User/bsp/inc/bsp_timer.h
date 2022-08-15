/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_timer.h
** Author      : Cai Heng
** Date        : 2021-06-16
** Description : 
**********************************************************************************************************/

#ifndef __BSP_TIMER_H_
#define __BSP_TIMER_H_

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
#define	TMR_COUNT			4			//�����ʱ������(��ʱ��ID��Χ:[0,3]) (Configurable)

#define TIM2_SEL			2
#define TIM3_SEL			3
#define TIM4_SEL			4
#define TIM5_SEL			5
#define HARDTIME			TIM2_SEL	//ѡ��TIM2��ΪӲ����ʱ��             (Configurable)

#if   (HARDTIME == TIM2_SEL)
	#define USE_TIM2
#elif (HARDTIME == TIM3_SEL)
	#define USE_TIM3
#elif (HARDTIME == TIM4_SEL)
	#define USE_TIM4
#elif (HARDTIME == TIM5_SEL)
	#define USE_TIM5
#endif

#ifdef USE_TIM2
	#define TIM_HARD					TIM2
	#define	RCC_TIM_HARD_CLK_ENABLE()	__HAL_RCC_TIM2_CLK_ENABLE()
	#define TIM_HARD_IRQn				TIM2_IRQn
	#define TIM_HARD_IRQHandler			TIM2_IRQHandler
#endif

#ifdef USE_TIM3
	#define TIM_HARD					TIM3
	#define	RCC_TIM_HARD_CLK_ENABLE()	__HAL_RCC_TIM3_CLK_ENABLE()	
	#define TIM_HARD_IRQn				TIM3_IRQn
	#define TIM_HARD_IRQHandler			TIM3_IRQHandler
#endif

#ifdef USE_TIM4
	#define TIM_HARD					TIM4
	#define	RCC_TIM_HARD_CLK_ENABLE()	__HAL_RCC_TIM4_CLK_ENABLE()
	#define TIM_HARD_IRQn				TIM4_IRQn
	#define TIM_HARD_IRQHandler			TIM4_IRQHandler
#endif

#ifdef USE_TIM5
	#define TIM_HARD					TIM5
	#define	RCC_TIM_HARD_CLK_ENABLE()	__HAL_RCC_TIM5_CLK_ENABLE()
	#define TIM_HARD_IRQn				TIM5_IRQn
	#define TIM_HARD_IRQHandler			TIM5_IRQHandler
#endif


/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/

/* ��ʱ���ṹ�壬��Ա����������volatile������C�������Ż�ʱ���������� */
typedef enum
{
	TMR_ONCE_MODE = 0,				//���ι���ģʽ
	TMR_AUTO_MODE = 1				//�Զ�����ģʽ
} TMR_MODE_E;

/* ��ʱ���ṹ�壬��Ա����������volatile������C�������Ż�ʱ���������� */
typedef struct
{
	volatile uint8_t Mode;			//������ģʽ
	volatile uint8_t Flag;			//��������־
	volatile uint32_t Count;		//������ʵʱֵ
	volatile uint32_t PreLoad;		//������Ԥװֵ
} SOFT_TMR;

/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
extern void RunPer1ms(void);
extern void RunPer10ms(void);
extern void RunPer25ms(void);
extern void RunPer100ms(void);
extern void RunPer500ms(void);
extern void RunPer1000ms(void);
extern void Error_Handler(char *file, uint32_t line);

#ifdef __cplusplus
}
#endif	

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
