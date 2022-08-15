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
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include "hc32_ddl.h"
#include "main.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/

/*********************************************************************************************************/

#define FAN_FaultPinNum				(0x06)	//风扇故障检测IO数量
#define BATT_OverVolt_PinNum	(0x08)	//电池过压保护检测IO数量
#define MAIN_VerGpioNum				(0x08)	//主控板版本检测IO数量
#define MAINPOWER_SIGNALNUM		(0x09)	//主控板及功率板在位检测IO数量

//风扇故障检测IO定义
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

//主控制在位信号检测IO定义
#define GPIO_PORT_MAINBOARD			 GPIO_PORT_G
#define GPIO_PIN_MAINBOARD	 		 GPIO_PIN_00

//功率板在位信号检测IO定义
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

//主控板版本识别IO定义
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
	#define GPIO_PORT_MAINPCB1					 GPIO_PORT_I	//C402A版(PA4)与C401A此处IO不同
	#define GPIO_PIN_MAINPCB1				 	 	 GPIO_PIN_10

	#define GPIO_PORT_MAINPCB2					 GPIO_PORT_I	//C402A版(PA5)与C401A此处IO不同
	#define GPIO_PIN_MAINPCB2				 	 	 GPIO_PIN_11

//#endif

#define GPIO_PORT_MAINID0					 	 GPIO_PORT_A
#define GPIO_PIN_MAINID0				 	 	 GPIO_PIN_06

#define GPIO_PORT_MAINID1				 	 	 GPIO_PORT_A
#define GPIO_PIN_MAINID1				 	 	 GPIO_PIN_07

//电池过压信号IO 
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
#define GPIO_PIN_CVK_4							GPIO_PIN_06	//4路并机模式信号PIN脚

#define GPIO_PORT_CVK_8							GPIO_PORT_B	
#define GPIO_PIN_CVK_8							GPIO_PIN_07	//8路并机模式信号PIN脚

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
HW : JN-MPU-C022F 　Dial Switch  读取IP 地址(192.168.X.X) PIN 映射关系
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
**                                           类型定义区
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
	uint8_t FanFault[6];	//风扇故障信息
	uint8_t BattOverVolt[8];	//电池过压信息
	uint8_t MainPowerSignal[9]; //主控及功率板在位信息
}TestInformation;	//IO检测信息存储结构体

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
//测试
extern uint8_t W5500_IP_ADDRESS[4];
extern uint8_t dev_addr;	


/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void Enable_Mloop(uint8_t _ch);
void Disable_Mloop(uint8_t _ch);
void bsp_HWDG_InitGPIO(void);
void Feed_WDG(void);
void IP_Scanning(void);

void InfoGpio_Init(void);
void MainPowerSignalRead(void);	//在位信号读取
void FanFaultStateRead(void); //风扇故障状态读取
void BattVoerVoltStateRead(void); //电池过压状态读取
void AllIOInfo_Read(void);	//IO信息读取

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
