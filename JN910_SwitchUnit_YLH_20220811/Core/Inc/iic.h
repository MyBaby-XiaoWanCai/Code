/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : iic.h
** Author      : YI LI HUA
** Date        : 2022-07-20
** Description :
**********************************************************************************************************/

#ifndef _IIC_H_
#define _IIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

void IOSwitch(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t dir);
	
/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
//IIC3定义 - 用于LED
#define LED_I2C3_SCL_GPIO		I2C3_SCL_LED_GPIO_Port
#define LED_I2C3_SCL_PIN		I2C3_SCL_LED_Pin		// I2C3_SCL : PH7 (Configurable)
#define LED_I2C3_SDA_GPIO		I2C3_SDA_LED_GPIO_Port
#define LED_I2C3_SDA_PIN		I2C3_SDA_LED_Pin		// I2C3_SDA : PH8 (Configurable)

#define I2C_LED_SCL_0()  		HAL_GPIO_WritePin(LED_I2C3_SCL_GPIO, LED_I2C3_SCL_PIN, GPIO_PIN_RESET)		// SCL = 0
#define I2C_LED_SCL_1()  		HAL_GPIO_WritePin(LED_I2C3_SCL_GPIO, LED_I2C3_SCL_PIN, GPIO_PIN_SET)		// SCL = 1
#define I2C_LED_SDA_0()  		HAL_GPIO_WritePin(LED_I2C3_SDA_GPIO, LED_I2C3_SDA_PIN, GPIO_PIN_RESET)		// SDA = 0
#define I2C_LED_SDA_1()  		HAL_GPIO_WritePin(LED_I2C3_SDA_GPIO, LED_I2C3_SDA_PIN, GPIO_PIN_SET)		// SDA = 1
#define I2C_LED_SCL_READ()  	HAL_GPIO_ReadPin(LED_I2C3_SCL_GPIO, LED_I2C3_SCL_PIN)						// 读SCL口线状态
#define I2C_LED_SDA_READ()  	HAL_GPIO_ReadPin(LED_I2C3_SDA_GPIO, LED_I2C3_SDA_PIN)						// 读SDA口线状态	// GPIO速度等级 (Configurable)
#define I2C_LED_SDA_IN()    	IOSwitch(LED_I2C3_SDA_GPIO,LED_I2C3_SDA_PIN,0); bsp_DelayUS(2)
#define I2C_LED_SDA_OUT()   	IOSwitch(LED_I2C3_SDA_GPIO,LED_I2C3_SDA_PIN,1); bsp_DelayUS(2)
#define I2C_LED_SCL_IN()    	IOSwitch(LED_I2C3_SCL_GPIO,LED_I2C3_SCL_PIN,0); bsp_DelayUS(2)
#define I2C_LED_SCL_OUT()   	IOSwitch(LED_I2C3_SCL_GPIO,LED_I2C3_SCL_PIN,1); bsp_DelayUS(2)

//IIC2定义 接NCA95555
#define I2C2_SCL_GPIO			I2C2_SCL_INTPUT_GPIO_Port
#define I2C2_SCL_PIN			I2C2_SCL_INTPUT_Pin			// I2C2_SCL : PF1 (Configurable)
#define I2C2_SDA_GPIO			I2C2_SDA_INTPUT_GPIO_Port
#define I2C2_SDA_PIN			I2C2_SDA_INTPUT_Pin			// I2C2_SDA : PF0 (Configurable)

#define I2C2_INT_ADDR_SCL_0()  	HAL_GPIO_WritePin(I2C2_SCL_GPIO, I2C2_SCL_PIN, GPIO_PIN_RESET)		// SCL = 0
#define I2C2_INT_ADDR_SCL_1()  	HAL_GPIO_WritePin(I2C2_SCL_GPIO, I2C2_SCL_PIN, GPIO_PIN_SET)		// SCL = 1
#define I2C2_INT_ADDR_SDA_0()  	HAL_GPIO_WritePin(I2C2_SDA_GPIO, I2C2_SDA_PIN, GPIO_PIN_RESET)		// SDA = 0
#define I2C2_INT_ADDR_SDA_1()  	HAL_GPIO_WritePin(I2C2_SDA_GPIO, I2C2_SDA_PIN, GPIO_PIN_SET)		// SDA = 1
#define I2C2_INT_ADDR_SCL_READ()HAL_GPIO_ReadPin(I2C2_SCL_GPIO, I2C2_SCL_PIN)						// 读SCL口线状态
#define I2C2_INT_ADDR_SDA_READ()HAL_GPIO_ReadPin(I2C2_SDA_GPIO, I2C2_SDA_PIN)						// 读SDA口线状态
#define I2C2_INT_ADDR_SDA_IN()  IOSwitch(I2C2_SDA_GPIO,I2C2_SDA_PIN,0); bsp_DelayUS(2)
#define I2C2_INT_ADDR_SDA_OUT() IOSwitch(I2C2_SDA_GPIO,I2C2_SDA_PIN,1); bsp_DelayUS(2)
#define I2C2_INT_ADDR_SCL_IN()  IOSwitch(I2C2_SCL_GPIO,I2C2_SCL_PIN,0); bsp_DelayUS(2)
#define I2C2_INT_ADDR_SCL_OUT() IOSwitch(I2C2_SCL_GPIO,I2C2_SCL_PIN,1); bsp_DelayUS(2)

//IIC1定义
#define I2C1_SCL_GPIO			I2C1_SCL_EEPROM_GPIO_Port
#define I2C1_SCL_PIN			I2C1_SCL_EEPROM_Pin
#define I2C1_SDA_GPIO			I2C1_SDA_EEPROM_GPIO_Port
#define I2C1_SDA_PIN			I2C1_SDA_EEPROM_Pin

#define I2C1_EE_TEMP_SCL_0()	HAL_GPIO_WritePin(I2C1_SCL_GPIO, I2C1_SCL_PIN, GPIO_PIN_RESET)
#define I2C1_EE_TEMP_SCL_1()	HAL_GPIO_WritePin(I2C1_SCL_GPIO, I2C1_SCL_PIN, GPIO_PIN_SET)
#define I2C1_EE_TEMP_SDA_0()	HAL_GPIO_WritePin(I2C1_SDA_GPIO, I2C1_SDA_PIN, GPIO_PIN_RESET)
#define I2C1_EE_TEMP_SDA_1()	HAL_GPIO_WritePin(I2C1_SDA_GPIO, I2C1_SDA_PIN, GPIO_PIN_SET)
#define I2C1_EE_TEMP_SCL_READ() HAL_GPIO_ReadPin(I2C1_SCL_GPIO, I2C1_SCL_PIN)	
#define I2C1_EE_TEMP_SDA_READ() HAL_GPIO_ReadPin(I2C1_SDA_GPIO, I2C1_SDA_PIN)	
#define I2C1_EE_TEMP_SDA_IN()   IOSwitch(I2C1_SDA_GPIO,I2C1_SDA_PIN,0); bsp_DelayUS(2)
#define I2C1_EE_TEMP_SDA_OUT()  IOSwitch(I2C1_SDA_GPIO,I2C1_SDA_PIN,1); bsp_DelayUS(2)
#define I2C1_EE_TEMP_SCL_IN()   IOSwitch(I2C1_SCL_GPIO,I2C1_SCL_PIN,0); bsp_DelayUS(2)
#define I2C1_EE_TEMP_SCL_OUT()  IOSwitch(I2C1_SCL_GPIO,I2C1_SCL_PIN,1); bsp_DelayUS(2)
#define I2C_WR			        (0)														            // 写控制位
#define I2C_RD			        (1)														            // 读控制位

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
typedef enum
{
		I2C_LED = 0x00,
		I2C_NCA9555 = 0x01,
		I2C_EE_TEMP = 0x02,
}
I2C_DevType;

typedef struct
{
		void (*Start)(I2C_DevType DevNum);
		void (*Stop)(I2C_DevType DevNum);
		void (*SendByte)(I2C_DevType DevNum, uint8_t _ucByte);
		void (*Ack)(I2C_DevType DevNum);
		void (*NAck)(I2C_DevType DevNum);
		uint8_t (*ReadByte)(I2C_DevType DevNum);
		uint8_t (*WaitAck)(I2C_DevType DevNum);
		uint8_t (*CheckDevice)(I2C_DevType DevNum, uint8_t DevAddr);
} I2C_HandleFunc;

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/
void i2c_Delay(I2C_DevType DevNum);
void i2c_Start(I2C_DevType DevNum);
void i2c_Stop(I2C_DevType DevNum);
void i2c_SendByte(I2C_DevType DevNum, uint8_t _ucByte);
uint8_t i2c_ReadByte(I2C_DevType DevNum);
uint8_t i2c_WaitAck(I2C_DevType DevNum);
void i2c_Ack(I2C_DevType DevNum);
void i2c_NAck(I2C_DevType DevNum);
uint8_t i2c_CheckDevice(I2C_DevType DevNum, uint8_t _Address);

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/
void bsp_InitI2C(void);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/