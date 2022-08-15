/**********************************************************************************************************
*
*	ģ������ : ͨ��GPIO���� 
*	�ļ����� : bsp_gpio.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*	��	 �ߣ�Robert.Li
**��	 ��:  2021.7.1
**********************************************************************************************************/

#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif







/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "stm32f4xx_hal.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/



/********************************************************************************************************	
HW : JN-MPU-C022F ��Dial Switch  ��ȡIP ��ַ(192.168.X.X) PIN ӳ���ϵ



		FR_ADDR1	:PH6 	FR_ADDR2	:PB10 
		FR_ADDR3 	:PE15	FR_ADDR4 	:PE10
		FR_ADDR5 	:PE9

		1-5 ��ӦIP ��ַ�������һλ 
		�磺1-5 ȫ���������� 192.168.X.11
		������1������ ��ӦIP ��ַ���� 192.168.X.12
		������2������ ��ӦIP ��ַ���� 192.168.X.13
		������1��2������ ��ӦIP ��ַ���� 192.168.X.14 


		FR_ADDR6 	:PE8
		FR_ADDR7 	:PE7	FR_ADDR8	:PG1

		6-8��Ӧ�Ŀ��
		�磺6-8 ȫ���������� 192.168.1.X
		������6������ ��ӦIP ��ַ���� 192.168.2.X
		������7������ ��ӦIP ��ַ���� 192.168.3.X
		������6��7������ ��ӦIP ��ַ���� 192.168.4.X
		
*********************************************************************************************************/

#define GPIO_PORT_FR_ADDR1		GPIOH			
#define GPIO_PIN_FR_ADDR1		GPIO_PIN_6		

#define GPIO_PORT_FR_ADDR2		GPIOB			
#define GPIO_PIN_FR_ADDR2		GPIO_PIN_10	

#define GPIO_PORT_FR_ADDR3		GPIOE			
#define GPIO_PIN_FR_ADDR3		GPIO_PIN_15		

#define GPIO_PORT_FR_ADDR4		GPIOE			
#define GPIO_PIN_FR_ADDR4		GPIO_PIN_10	

#define GPIO_PORT_FR_ADDR5		GPIOE			
#define GPIO_PIN_FR_ADDR5		GPIO_PIN_9		




#define GPIO_PORT_FR_ADDR6		GPIOE			
#define GPIO_PIN_FR_ADDR6		GPIO_PIN_8	

#define GPIO_PORT_FR_ADDR7		GPIOE			
#define GPIO_PIN_FR_ADDR7		GPIO_PIN_7		

#define GPIO_PORT_FR_ADDR8		GPIOG			
#define GPIO_PIN_FR_ADDR8		GPIO_PIN_1	


/********************************************************************************************************	
HW : JN-MPU-C022H

	M_LOOPӳ���ϵ	
*********************************************************************************************************/
#define GPIO_PORT_LOOP1		GPIOA			
#define GPIO_PIN_LOOP1		GPIO_PIN_10	

#define GPIO_PORT_LOOP2		GPIOA			
#define GPIO_PIN_LOOP2		GPIO_PIN_15	

#define GPIO_PORT_LOOP3		GPIOI			
#define GPIO_PIN_LOOP3		GPIO_PIN_11	

#define GPIO_PORT_LOOP4		GPIOE			
#define GPIO_PIN_LOOP4		GPIO_PIN_2	

#define GPIO_PORT_LOOP5		GPIOC			
#define GPIO_PIN_LOOP5		GPIO_PIN_1	

#define GPIO_PORT_LOOP6		GPIOI			
#define GPIO_PIN_LOOP6		GPIO_PIN_10	

#define GPIO_PORT_LOOP7		GPIOA			
#define GPIO_PIN_LOOP7		GPIO_PIN_0	

#define GPIO_PORT_LOOP8		GPIOA			
#define GPIO_PIN_LOOP8		GPIO_PIN_4	


#define M_LOOP_ENABLE		1

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/


/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
extern uint8_t W5500_IP_ADDRESS[4];
	
/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
void bsp_M_Loop_InitGPIO(void);
void Enable_Mloop(uint8_t _ch);
void Disable_Mloop(uint8_t _ch);
void bsp_FR_ADDR_InitGPIO(void);
void bsp_HWDG_InitGPIO(void);
void Feed_WDG(void);
extern void IP_Scanning(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
