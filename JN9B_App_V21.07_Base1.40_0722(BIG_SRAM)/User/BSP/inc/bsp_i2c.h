/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_i2c.h
** Author      : Huang Cheng
** Date        : 2022-01-12
** Description :
**********************************************************************************************************/

#ifndef _BSP_I2C_H_
#define _BSP_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "hc32_ddl.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
//IIC3���� - ����LED
#define LED_I2C3_SCL_GPIO		GPIO_PORT_H
#define LED_I2C3_SCL_PIN		GPIO_PIN_07		// I2C3_SCL : PH7 (Configurable)
#define LED_I2C3_SDA_GPIO		GPIO_PORT_H
#define LED_I2C3_SDA_PIN		GPIO_PIN_08		// I2C3_SDA : PH8 (Configurable)

#define I2C_LED_SCL_0()  	GPIO_ResetPins(LED_I2C3_SCL_GPIO, LED_I2C3_SCL_PIN)		// SCL = 0
#define I2C_LED_SCL_1()  	GPIO_SetPins(LED_I2C3_SCL_GPIO, LED_I2C3_SCL_PIN)			// SCL = 1
#define I2C_LED_SDA_0()  	GPIO_ResetPins(LED_I2C3_SDA_GPIO, LED_I2C3_SDA_PIN)		// SDA = 0
#define I2C_LED_SDA_1()  	GPIO_SetPins(LED_I2C3_SDA_GPIO, LED_I2C3_SDA_PIN)			// SDA = 1
#define I2C_LED_SCL_READ()  GPIO_ReadInputPins(LED_I2C3_SCL_GPIO, LED_I2C3_SCL_PIN)	// ��SCL����״̬
#define I2C_LED_SDA_READ()  GPIO_ReadInputPins(LED_I2C3_SDA_GPIO, LED_I2C3_SDA_PIN)	// ��SDA����״̬	// GPIO�ٶȵȼ� (Configurable)
#define I2C_LED_SDA_IN()    GPIO_OE(LED_I2C3_SDA_GPIO,LED_I2C3_SDA_PIN,Disable); bsp_DelayUS(2)
#define I2C_LED_SDA_OUT()   GPIO_OE(LED_I2C3_SDA_GPIO,LED_I2C3_SDA_PIN,Enable); bsp_DelayUS(2)
#define I2C_LED_SCL_IN()    GPIO_OE(LED_I2C3_SCL_GPIO,LED_I2C3_SCL_PIN,Disable); bsp_DelayUS(2)
#define I2C_LED_SCL_OUT()   GPIO_OE(LED_I2C3_SCL_GPIO,LED_I2C3_SCL_PIN,Enable); bsp_DelayUS(2)
//IIC2���� ��PCA9548A��SDA�ź�
#define I2C2_SCL_GPIO		GPIO_PORT_F
#define I2C2_SCL_PIN		GPIO_PIN_01		// I2C2_SCL : PF1 (Configurable)
#define I2C2_SDA_GPIO		GPIO_PORT_F
#define I2C2_SDA_PIN		GPIO_PIN_00		// I2C2_SDA : PF0 (Configurable)

#define I2C2_SMBA_GPIO 		GPIO_PORT_F		//��I2C������ (PCA9548A)�ĸ�λ�źţ�����Ч
#define I2C2_SMBA_PIN			GPIO_PIN_02

#define I2C2_INT_ADDR_SCL_0()  	 GPIO_ResetPins(I2C2_SCL_GPIO, I2C2_SCL_PIN)	// SCL = 0
#define I2C2_INT_ADDR_SCL_1()  	 GPIO_SetPins(I2C2_SCL_GPIO, I2C2_SCL_PIN)		// SCL = 1
#define I2C2_INT_ADDR_SDA_0()  	 GPIO_ResetPins(I2C2_SDA_GPIO, I2C2_SDA_PIN)	// SDA = 0
#define I2C2_INT_ADDR_SDA_1()  	 GPIO_SetPins(I2C2_SDA_GPIO, I2C2_SDA_PIN)		// SDA = 1
#define I2C2_INT_ADDR_SCL_READ()  GPIO_ReadInputPins(I2C2_SCL_GPIO, I2C2_SCL_PIN)	// ��SCL����״̬
#define I2C2_INT_ADDR_SDA_READ()  GPIO_ReadInputPins(I2C2_SDA_GPIO, I2C2_SDA_PIN)	// ��SDA����״̬
#define I2C2_INT_ADDR_SDA_IN()    GPIO_OE(I2C2_SDA_GPIO,I2C2_SDA_PIN,Disable); bsp_DelayUS(2)
#define I2C2_INT_ADDR_SDA_OUT()   GPIO_OE(I2C2_SDA_GPIO,I2C2_SDA_PIN,Enable); bsp_DelayUS(2)
#define I2C2_INT_ADDR_SCL_IN()    GPIO_OE(I2C2_SCL_GPIO,I2C2_SCL_PIN,Disable); bsp_DelayUS(2)
#define I2C2_INT_ADDR_SCL_OUT()   GPIO_OE(I2C2_SCL_GPIO,I2C2_SCL_PIN,Enable); bsp_DelayUS(2)
//IIC1����
#define I2C1_SCL_GPIO		GPIO_PORT_B
#define I2C1_SCL_PIN		GPIO_PIN_08
#define I2C1_SDA_GPIO		GPIO_PORT_B
#define I2C1_SDA_PIN		GPIO_PIN_09

#define I2C1_EE_TEMP_SCL_0()		GPIO_ResetPins(I2C1_SCL_GPIO, I2C1_SCL_PIN)
#define I2C1_EE_TEMP_SCL_1()		GPIO_SetPins(I2C1_SCL_GPIO, I2C1_SCL_PIN)
#define I2C1_EE_TEMP_SDA_0()		GPIO_ResetPins(I2C1_SDA_GPIO, I2C1_SDA_PIN)
#define I2C1_EE_TEMP_SDA_1()		GPIO_SetPins(I2C1_SDA_GPIO, I2C1_SDA_PIN)
#define I2C1_EE_TEMP_SCL_READ()     GPIO_ReadInputPins(I2C1_SCL_GPIO, I2C1_SCL_PIN)	// ��SCL����״̬
#define I2C1_EE_TEMP_SDA_READ()     GPIO_ReadInputPins(I2C1_SDA_GPIO, I2C1_SDA_PIN)	// ��SDA����״̬
#define I2C1_EE_TEMP_SDA_IN()       GPIO_OE(I2C1_SDA_GPIO,I2C1_SDA_PIN,Disable); bsp_DelayUS(2)
#define I2C1_EE_TEMP_SDA_OUT()      GPIO_OE(I2C1_SDA_GPIO,I2C1_SDA_PIN,Enable); bsp_DelayUS(2)
#define I2C1_EE_TEMP_SCL_IN()       GPIO_OE(I2C1_SCL_GPIO,I2C1_SCL_PIN,Disable); bsp_DelayUS(2)
#define I2C1_EE_TEMP_SCL_OUT()      GPIO_OE(I2C1_SCL_GPIO,I2C1_SCL_PIN,Enable); bsp_DelayUS(2)
#define I2C_WR			         (0)														            // д����λ
#define I2C_RD			         (1)														            // ������λ

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/
typedef enum
{
		I2C_LED = 0x00,
		I2C_PCA9548 = 0x01,
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
**                                         �ⲿ���ñ�����
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
**                                         �ⲿ���ú�����
**********************************************************************************************************/
void bsp_InitI2C(void);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
