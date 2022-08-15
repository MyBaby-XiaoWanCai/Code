/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_gpio.h
** Author      : Huang Cheng
** Date        : 2022-01-04
** Description : bsp gpio head file
**********************************************************************************************************/

#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include <stdint.h>
#include "hc32_ddl.h"
#include "main.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/

/*********************************************************************************************************/

#define FAN_FaultPinNum				(0x06)	//���ȹ��ϼ��IO����
#define BATT_OverVolt_PinNum	(0x08)	//��ع�ѹ�������IO����
#define MAIN_VerGpioNum				(0x08)	//���ذ�汾���IO����
#define MAINPOWER_SIGNALNUM		(0x09)	//���ذ弰���ʰ���λ���IO����

//���ȹ��ϼ��IO����
#define GPIO_PORT_FAN1		GPIO_PORT_D	
#define GPIO_PIN_FAN1			GPIO_PIN_02

#define GPIO_PORT_FAN2		GPIO_PORT_D	
#define GPIO_PIN_FAN2			GPIO_PIN_03

#define GPIO_PORT_FAN3		GPIO_PORT_D	
#define GPIO_PIN_FAN3			GPIO_PIN_04

#define GPIO_PORT_FAN4		GPIO_PORT_D	
#define GPIO_PIN_FAN4			GPIO_PIN_05

#define GPIO_PORT_FAN5		GPIO_PORT_D	
#define GPIO_PIN_FAN5			GPIO_PIN_06

#define GPIO_PORT_FAN6		GPIO_PORT_D	
#define GPIO_PIN_FAN6			GPIO_PIN_07

//��������λ�źż��IO����
#define GPIO_PORT_MAINBOARD			 GPIO_PORT_G
#define GPIO_PIN_MAINBOARD	 		 GPIO_PIN_00

//���ʰ���λ�źż��IO����
#define GPIO_PORT_POWERBOARD_1	 GPIO_PORT_G
#define GPIO_PIN_POWERBOARD_1		 GPIO_PIN_01

#define GPIO_PORT_POWERBOARD_2	 GPIO_PORT_G
#define GPIO_PIN_POWERBOARD_2		 GPIO_PIN_02

#define GPIO_PORT_POWERBOARD_3	 GPIO_PORT_G
#define GPIO_PIN_POWERBOARD_3		 GPIO_PIN_03

#define GPIO_PORT_POWERBOARD_4	 GPIO_PORT_G
#define GPIO_PIN_POWERBOARD_4		 GPIO_PIN_04

#define GPIO_PORT_POWERBOARD_5	 GPIO_PORT_G
#define GPIO_PIN_POWERBOARD_5		 GPIO_PIN_05

#define GPIO_PORT_POWERBOARD_6	 GPIO_PORT_G
#define GPIO_PIN_POWERBOARD_6		 GPIO_PIN_06

#define GPIO_PORT_POWERBOARD_7	 GPIO_PORT_G
#define GPIO_PIN_POWERBOARD_7		 GPIO_PIN_07

#define GPIO_PORT_POWERBOARD_8	 GPIO_PORT_G
#define GPIO_PIN_POWERBOARD_8		 GPIO_PIN_08

//���ذ�汾ʶ��IO����
#define GPIO_PORT_MAINBOOM0					 GPIO_PORT_A
#define GPIO_PIN_MAINBOOM0					 GPIO_PIN_00

#define GPIO_PORT_MAINBOOM1					 GPIO_PORT_A
#define GPIO_PIN_MAINBOOM1					 GPIO_PIN_01

#define GPIO_PORT_MAINBOOM2					 GPIO_PORT_A
#define GPIO_PIN_MAINBOOM2				 	 GPIO_PIN_02

#define GPIO_PORT_MAINPCB0					 GPIO_PORT_A
#define GPIO_PIN_MAINPCB0					 	 GPIO_PIN_03

//#ifdef C041A_7175
//	#define GPIO_PORT_MAINPCB1					 GPIO_PORT_A
//	#define GPIO_PIN_MAINPCB1				 	 	 GPIO_PIN_04

//	#define GPIO_PORT_MAINPCB2					 GPIO_PORT_A
//	#define GPIO_PIN_MAINPCB2				 	 	 GPIO_PIN_05

//#else
	#define GPIO_PORT_MAINPCB1					 GPIO_PORT_I	//C402A��(PA4)��C401A�˴�IO��ͬ
	#define GPIO_PIN_MAINPCB1				 	 	 GPIO_PIN_10

	#define GPIO_PORT_MAINPCB2					 GPIO_PORT_I	//C402A��(PA5)��C401A�˴�IO��ͬ
	#define GPIO_PIN_MAINPCB2				 	 	 GPIO_PIN_11

//#endif

#define GPIO_PORT_MAINID0					 	 GPIO_PORT_A
#define GPIO_PIN_MAINID0				 	 	 GPIO_PIN_06

#define GPIO_PORT_MAINID1				 	 	 GPIO_PORT_A
#define GPIO_PIN_MAINID1				 	 	 GPIO_PIN_07

//��ع�ѹ�ź�IO 
#define GPIO_PORT_BATTOVERVOLT_1		GPIO_PORT_C
#define GPIO_PIN_BATTOVERVOLT_1			GPIO_PIN_00

#define GPIO_PORT_BATTOVERVOLT_2		GPIO_PORT_C
#define GPIO_PIN_BATTOVERVOLT_2			GPIO_PIN_01

#define GPIO_PORT_BATTOVERVOLT_3		GPIO_PORT_C
#define GPIO_PIN_BATTOVERVOLT_3			GPIO_PIN_02

#define GPIO_PORT_BATTOVERVOLT_4		GPIO_PORT_C
#define GPIO_PIN_BATTOVERVOLT_4			GPIO_PIN_03

#define GPIO_PORT_BATTOVERVOLT_5		GPIO_PORT_C
#define GPIO_PIN_BATTOVERVOLT_5			GPIO_PIN_04

#define GPIO_PORT_BATTOVERVOLT_6		GPIO_PORT_C
#define GPIO_PIN_BATTOVERVOLT_6			GPIO_PIN_05

#define GPIO_PORT_BATTOVERVOLT_7		GPIO_PORT_C
#define GPIO_PIN_BATTOVERVOLT_7			GPIO_PIN_06

#define GPIO_PORT_BATTOVERVOLT_8		GPIO_PORT_C
#define GPIO_PIN_BATTOVERVOLT_8			GPIO_PIN_07

#define GPIO_PORT_CVK_4							GPIO_PORT_B
#define GPIO_PIN_CVK_4							GPIO_PIN_06	//4·����ģʽ�ź�PIN��

#define GPIO_PORT_CVK_8							GPIO_PORT_B	
#define GPIO_PIN_CVK_8							GPIO_PIN_07	//8·����ģʽ�ź�PIN��

#pragma pack(1)
typedef struct
{
	uint8_t  GpioPort;
	uint16_t GpioPin;
}HCPortPin_TypeDef;
#pragma pack()

void bsp_InitFanFault(void);
void bsp_InitMainPowerSignalBoard(void);
void bsp_InitVersionIdent(void);
void bsp_InitBattOverVolt(void);
void bsp_InitCVK(void);

void bsp_GpioInit(void);

/*********************************************************************************************************/

/**********************************************************************************************************	
HW : JN-MPU-C022F ��Dial Switch  ��ȡIP ��ַ(192.168.X.X) PIN ӳ���ϵ
**********************************************************************************************************/
#define IP_ADDR3_END            0X03
#define IP_ADDR4_END            0X08

/********************************************************************************************************	
HW : JN-MPU-C022H
HWDG: PB2
*********************************************************************************************************/
#define GPIO_PORT_HWDG      GPIO_PORT_B
#define GPIO_PIN_HWDG       GPIO_PIN_02

#define M_LOOP_ENABLE		1

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/
#pragma pack(1)
typedef struct
{
	uint8_t  GpioPort;
	uint16_t GpioPin;
}LoopPortPin_TypeDef;
#pragma pack()

typedef struct 
{
	uint8_t FanFault[6];	//���ȹ�����Ϣ
	uint8_t BattOverVolt[8];	//��ع�ѹ��Ϣ
	uint8_t MainPowerSignal[9]; //���ؼ����ʰ���λ��Ϣ
}TestInformation;	//IO�����Ϣ�洢�ṹ��

/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
//����
extern uint8_t W5500_IP_ADDRESS[4];
extern uint8_t dev_addr;	


/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
void Enable_Mloop(uint8_t _ch);
void Disable_Mloop(uint8_t _ch);
void bsp_HWDG_InitGPIO(void);
void Feed_WDG(void);
void IP_Scanning(void);

void InfoGpio_Init(void);
void MainPowerSignalRead(void);	//��λ�źŶ�ȡ
void FanFaultStateRead(void); //���ȹ���״̬��ȡ
void BattVoerVoltStateRead(void); //��ع�ѹ״̬��ȡ
void AllIOInfo_Read(void);	//IO��Ϣ��ȡ

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
