/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_charge.h
** Author      : Huang Cheng
** Date        : 2021-11-08
** Description : bsp_charge head file
**********************************************************************************************************/

#ifndef _BSP_CHARGE_H_
#define _BSP_CHARGE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include "hc32_ddl.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
/**********************************************************************************************************
		
	JN-MPU-C022F 　charge and discharge control PIN 映射关系：

	Mod_DIR1 --> PE5	|	Mod_EN1 --> PH9		|	Mod_MS1 --> PD13	|	Mod_V_Range1 --> PH13	|	Mod_Rev_Port1 --> PG7
	Mod_DIR2 --> PE6	|	Mod_EN2 --> PH10	|	Mod_MS2 --> PD14	|	Mod_V_Range2 --> PH14	|	Mod_Rev_Port2 --> PG8
	Mod_DIR3 --> PI8	|	Mod_EN3 --> PH11	|	Mod_MS3 --> PD15	|	Mod_V_Range3 --> PH15	|	Mod_Rev_Port3 --> PC6
	Mod_DIR4 --> PC13	|	Mod_EN4 --> PH12	|	Mod_MS4 --> PG2		|	Mod_V_Range4 --> PG15	|	Mod_Rev_Port4 --> PC7
	Mod_DIR5 --> PI9	|	Mod_EN5 --> PB14	|	Mod_MS5 --> PG3		|	Mod_V_Range5 --> PB8	|	Mod_Rev_Port5 --> PC8
	Mod_DIR6 --> PF0	|	Mod_EN6 --> PD10	|	Mod_MS6 --> PG4		|	Mod_V_Range6 --> PB9	|	Mod_Rev_Port6 --> PC9
	Mod_DIR7 --> PF1	|	Mod_EN7 --> PD11	|	Mod_MS7 --> PG5		|	Mod_V_Range7 --> PI4	|	Mod_Rev_Port7 --> PA8
	Mod_DIR8 --> PF2	|	Mod_EN8 --> PD12	|	Mod_MS8 --> PG6		|	Mod_V_Range8 --> PI5	|	Mod_Rev_Port8 --> PA9

	Mod_EN		: 使能/失能工作

	Mod_DIR		: 控制电流方向(充电还是放电)
	MOD_MS		: 设置主机/从机(单机/并机工作)
	Mod_V_Range	: 控制是端口/Sense模式  
	Mod_Rev_Port: 控制反转断开/继电器通路
	
	所有I/O输出功能脚统统配置成上拉推挽输出		
**********************************************************************************************************/

#define MOD_DIR_TYPE					(0x00) 	//写1 按位或 0x04,	写0 按位与 0xFB; 
#define MOD_EN_TYPE						(0x01)	
#define MOD_MS_TYPE						(0x02)	//写1 按位或 0x08,	写0 按位与 0xF7
#define MOD_V_RANGE						(0x03)	//写1 按位或 0x02,  写0 按位与 0xFD
#define MOD_REV_PORT_TYPE			(0x04)	//写1 按位或 0x01,  写0 按位与 0xFE

//Mod_ENx(0 - DISABLE; 1 - ENABLE)
#define MOD_EN1_PORT			GPIO_PORT_H
#define MOD_EN1_PIN				GPIO_PIN_09
#define MOD_EN2_PORT			GPIO_PORT_H
#define MOD_EN2_PIN				GPIO_PIN_10
#define MOD_EN3_PORT			GPIO_PORT_H
#define MOD_EN3_PIN				GPIO_PIN_11
#define MOD_EN4_PORT			GPIO_PORT_H
#define MOD_EN4_PIN				GPIO_PIN_12
#define MOD_EN5_PORT			GPIO_PORT_B
#define MOD_EN5_PIN				GPIO_PIN_14
#define MOD_EN6_PORT			GPIO_PORT_D
#define MOD_EN6_PIN				GPIO_PIN_10
#define MOD_EN7_PORT			GPIO_PORT_D
#define MOD_EN7_PIN				GPIO_PIN_11
#define MOD_EN8_PORT			GPIO_PORT_D
#define MOD_EN8_PIN				GPIO_PIN_12

//控制口状态
#define PORT_DIR_BUCK				1		//充电
#define PORT_DIR_BOOST			2		//放电
#define PORT_OUT_DISABLE		3
#define PORT_OUT_ENABLE 		4		
//#define PORT_MODE_MASTER 		5
//#define PORT_MODE_SLAVE			6
#define PORT_MODE_MASTER 		6	//与JN95相反
#define PORT_MODE_SLAVE			5
#define PORT_REV_ENABLE 		7
#define PORT_REV_DISABLE		8
#define PORT_RANGE_SENSE		9
#define PORT_RANGE_MOD			10
#define PORT_STA_MAX        PORT_RANGE_MOD

//控制口标识
#define PORT_NO1				1	
#define PORT_NO2				2	
#define PORT_NO3				3	
#define PORT_NO4				4	
#define PORT_NO5				5	
#define PORT_NO6				6	
#define PORT_NO7				7	
#define PORT_NO8				8
#define PORT_NO_MAX     PORT_NO8

//Mod_DIR_Flag的标识
#define BUCK_MODE				1		//BUCK模式 - 充电模式
#define BOOST_MODE			0		//BOOST模式 - 放电模式

//Mod_V_Range_Flag的标识
#define V_SENSE					1
#define V_MOD						0

//系统工作模式
#define SYS_SELFCHECK_MODE		0x22
#define SYS_RETEST_MODE				0x33
#define SYS_AGEING_MODE				0x44
#define SYS_CALIBRATION_MODE	0x55
#define SYS_NORMAL_MODE				0x66

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
enum
{
	Mod_DIR= 0x00,
	Mod_EN,
	Mod_MS,
	Mod_Rev,
	Mod_V_Range,
	
	ModPortNumMax
};

#pragma pack(1)
typedef struct
{
	uint8_t  GpioPort[8];
	uint16_t GpioPin[8];
}ModPortPin_TypeDef;
#pragma pack()

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern uint8_t Mod_DIR_Flag[8];
extern uint8_t Mod_V_Range_Flag[8];
extern uint8_t Channel_Port_Table[8];	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void bsp_InitCharge(void);
void Chg_Dis_Port_Cfg(uint8_t _no, uint8_t sta);
	

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
