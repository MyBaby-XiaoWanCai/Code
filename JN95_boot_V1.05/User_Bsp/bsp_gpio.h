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


/**********************************************************************************************************
		
	JN-MPU-C022G ��charge and discharge control PIN ӳ���ϵ��

	Mod_DIR1 --> PE5	|	Mod_EN1 --> PH9		|	Mod_MS1 --> PD13	|	Mod_V_Range1 --> PH13	|	Mod_Rev_Port1 --> PG7
	Mod_DIR2 --> PE6	|	Mod_EN2 --> PH10	|	Mod_MS2 --> PD14	|	Mod_V_Range2 --> PH14	|	Mod_Rev_Port2 --> PG8
	Mod_DIR3 --> PI8	|	Mod_EN3 --> PH11	|	Mod_MS3 --> PD15	|	Mod_V_Range3 --> PH15	|	Mod_Rev_Port3 --> PC6
	Mod_DIR4 --> PC13	|	Mod_EN4 --> PH12	|	Mod_MS4 --> PG2		|	Mod_V_Range4 --> PG15	|	Mod_Rev_Port4 --> PC7
	Mod_DIR5 --> PI9	|	Mod_EN5 --> PB14	|	Mod_MS5 --> PG3		|	Mod_V_Range5 --> PB8	|	Mod_Rev_Port5 --> PC8
	Mod_DIR6 --> PF0	|	Mod_EN6 --> PD10	|	Mod_MS6 --> PG4		|	Mod_V_Range6 --> PB9	|	Mod_Rev_Port6 --> PC9
	Mod_DIR7 --> PF1	|	Mod_EN7 --> PD11	|	Mod_MS7 --> PG5		|	Mod_V_Range7 --> PI4	|	Mod_Rev_Port7 --> PA8
	Mod_DIR8 --> PF2	|	Mod_EN8 --> PD12	|	Mod_MS8 --> PG6		|	Mod_V_Range8 --> PI5	|	Mod_Rev_Port8 --> PA9

	Mod_DIR		: ���Ƶ�������(��绹�Ƿŵ�)
	Mod_EN		: ʹ��/ʧ�ܹ���
	MOD_MS		: ��������/�ӻ�(����/��������)
	Mod_V_Range	: �����Ƕ˿�/Senseģʽ  
	Mod_Rev_Port: ���Ʒ�ת�Ͽ�/�̵���ͨ·
	
	����I/O������ܽ�ͳͳ���ó������������
	
	IO_OE --> PA10 : SN74LVC16T245DGGR Enable
			
**********************************************************************************************************/

//Mod_DIRx(0 - BUCK; 1 - BOOST)
#define MOD_DIR1_PORT			GPIOE
#define MOD_DIR1_PIN			GPIO_PIN_5
#define MOD_DIR2_PORT			GPIOE
#define MOD_DIR2_PIN			GPIO_PIN_6
#define MOD_DIR3_PORT			GPIOI
#define MOD_DIR3_PIN			GPIO_PIN_8
#define MOD_DIR4_PORT			GPIOC
#define MOD_DIR4_PIN			GPIO_PIN_13
#define MOD_DIR5_PORT			GPIOI
#define MOD_DIR5_PIN			GPIO_PIN_9
#define MOD_DIR6_PORT			GPIOF
#define MOD_DIR6_PIN			GPIO_PIN_0
#define MOD_DIR7_PORT			GPIOF
#define MOD_DIR7_PIN			GPIO_PIN_1
#define MOD_DIR8_PORT			GPIOF
#define MOD_DIR8_PIN			GPIO_PIN_2

//Mod_ENx(0 - DISABLE; 1 - ENABLE)
#define MOD_EN1_PORT			GPIOH
#define MOD_EN1_PIN				GPIO_PIN_9
#define MOD_EN2_PORT			GPIOH
#define MOD_EN2_PIN				GPIO_PIN_10
#define MOD_EN3_PORT			GPIOH
#define MOD_EN3_PIN				GPIO_PIN_11
#define MOD_EN4_PORT			GPIOH
#define MOD_EN4_PIN				GPIO_PIN_12
#define MOD_EN5_PORT			GPIOB
#define MOD_EN5_PIN				GPIO_PIN_14
#define MOD_EN6_PORT			GPIOD
#define MOD_EN6_PIN				GPIO_PIN_10
#define MOD_EN7_PORT			GPIOD
#define MOD_EN7_PIN				GPIO_PIN_11
#define MOD_EN8_PORT			GPIOD
#define MOD_EN8_PIN				GPIO_PIN_12

//Mod_MSx(0 - MASTER; 1 - SLAVE)
#define MOD_MS1_PORT			GPIOD			
#define MOD_MS1_PIN				GPIO_PIN_13		
#define MOD_MS2_PORT			GPIOD			
#define MOD_MS2_PIN				GPIO_PIN_14		
#define MOD_MS3_PORT			GPIOD			
#define MOD_MS3_PIN				GPIO_PIN_15		
#define MOD_MS4_PORT			GPIOG			
#define MOD_MS4_PIN				GPIO_PIN_2		
#define MOD_MS5_PORT			GPIOG			
#define MOD_MS5_PIN				GPIO_PIN_3		
#define MOD_MS6_PORT			GPIOG			
#define MOD_MS6_PIN				GPIO_PIN_4		
#define MOD_MS7_PORT			GPIOG			
#define MOD_MS7_PIN				GPIO_PIN_5		
#define MOD_MS8_PORT			GPIOG			
#define MOD_MS8_PIN				GPIO_PIN_6	

//Mod_V_Rangex(0 - SENSE; 1 - MOD)
#define MOD_V_RANGE1_PORT		GPIOH
#define MOD_V_RANGE1_PIN		GPIO_PIN_13
#define MOD_V_RANGE2_PORT		GPIOH
#define MOD_V_RANGE2_PIN		GPIO_PIN_14
#define MOD_V_RANGE3_PORT		GPIOH
#define MOD_V_RANGE3_PIN		GPIO_PIN_15
#define MOD_V_RANGE4_PORT		GPIOG
#define MOD_V_RANGE4_PIN		GPIO_PIN_15
#define MOD_V_RANGE5_PORT		GPIOB
#define MOD_V_RANGE5_PIN		GPIO_PIN_8
#define MOD_V_RANGE6_PORT		GPIOB
#define MOD_V_RANGE6_PIN		GPIO_PIN_9
#define MOD_V_RANGE7_PORT		GPIOI
#define MOD_V_RANGE7_PIN		GPIO_PIN_4
#define MOD_V_RANGE8_PORT		GPIOI
#define MOD_V_RANGE8_PIN		GPIO_PIN_5

//Mod_Rev_Portx(0 - ENABLE; 1 - DISABLE)
#define MOD_REV_PORT1_PORT		GPIOG
#define MOD_REV_PORT1_PIN		GPIO_PIN_7
#define MOD_REV_PORT2_PORT		GPIOG
#define MOD_REV_PORT2_PIN		GPIO_PIN_8
#define MOD_REV_PORT3_PORT		GPIOC
#define MOD_REV_PORT3_PIN		GPIO_PIN_6
#define MOD_REV_PORT4_PORT		GPIOC
#define MOD_REV_PORT4_PIN		GPIO_PIN_7
#define MOD_REV_PORT5_PORT		GPIOC
#define MOD_REV_PORT5_PIN		GPIO_PIN_8
#define MOD_REV_PORT6_PORT		GPIOC
#define MOD_REV_PORT6_PIN		GPIO_PIN_9
#define MOD_REV_PORT7_PORT		GPIOA
#define MOD_REV_PORT7_PIN		GPIO_PIN_8
#define MOD_REV_PORT8_PORT		GPIOA
#define MOD_REV_PORT8_PIN		GPIO_PIN_9

//IO_OE
#define IO_OE_PORT				GPIOA			
#define IO_OE_PIN				GPIO_PIN_10	

//���ƿ�״̬
#define PORT_DIR_BUCK			1	
#define PORT_DIR_BOOST			2	
#define PORT_OUT_ENABLE 		4	
#define PORT_OUT_DISABLE		3	
#define PORT_MODE_MASTER 		5
#define PORT_MODE_SLAVE			6
#define PORT_REV_ENABLE 		7
#define PORT_REV_DISABLE		8
#define PORT_RANGE_SENSE		9
#define PORT_RANGE_MOD			10

//���ƿڱ�ʶ
#define PORT_NO1				1	
#define PORT_NO2				2	
#define PORT_NO3				3	
#define PORT_NO4				4	
#define PORT_NO5				5	
#define PORT_NO6				6	
#define PORT_NO7				7	
#define PORT_NO8				8	

//Mod_DIR_Flag�ı�ʶ
#define BUCK_MODE				1
#define BOOST_MODE				0

//Mod_V_Range_Flag�ı�ʶ
#define V_SENSE					1
#define V_MOD					0

//ϵͳ����ģʽ
#define SYS_SELFCHECK_MODE		0x22
#define SYS_RETEST_MODE			0x33
#define SYS_AGEING_MODE			0x44
#define SYS_CALIBRATION_MODE	0x55
#define SYS_NORMAL_MODE			0x66



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

void bsp_FR_ADDR_InitGPIO(void);
void IP_Scanning(void);
void Feed_WDG(void);
#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
