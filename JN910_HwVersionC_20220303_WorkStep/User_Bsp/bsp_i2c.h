/**********************************************************************************************************
** File name   : i2c.h
** Author      : Huang Cheng
** Date        : 2022-01-17
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
#include "main.h"
#include <stdint.h>

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
#define I2C_QUEUE_HEAD    0U
#define I2C_QUEUE_TAIL    3U
#define I2C_DEVICE_NUM    3U
#define I2C_DEVTAB_SIZE   4U                  //�豸���ͷ����洢�豸��Ϣ��ֻ�洢�α���Ϣ
#define DELETE_ID         255U
#define I2C_DEVICE_OK     0U
#define I2C_DEVICE_ERR    1U

#define I2C_WR			  0U														           
#define I2C_RD			  1U														            
/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/
/* I2C�豸�˿�ʱ�����Ͷ��� */
typedef struct
{
	GPIO_TypeDef* Port;
	uint32_t GPIO_CLK;
}I2C_PortClkType;

/* I2C�豸PortPin���� */
#pragma pack(1)
typedef struct
{
	GPIO_TypeDef* SCL_Port;
	uint16_t SCL_Pin;
	GPIO_TypeDef* SDA_Port;
	uint16_t SDA_Pin;
	uint8_t DelayTime;
}I2C_ParamType;
#pragma pack()

/* I2C�豸���� */
#pragma pack(1)
typedef struct
{
	I2C_ParamType I2C_Param;
	uint8_t DeviceNext;
}I2C_ParamTabType;
#pragma pack()

/* I2C�豸��ʱ/��дPin������� */
typedef struct
{
	void (*Delay)(uint8_t nus);
	void (*WritePin)(GPIO_TypeDef* Port, uint16_t Pin, GPIO_PinState PinState);
	GPIO_PinState (*ReadPin)(GPIO_TypeDef* Port, uint16_t Pin);
}I2C_RegFuncType;

/* I2C�豸����������� */
typedef struct
{
	void (*Init)(GPIO_TypeDef* Port, uint16_t Pin);
	void (*Start)(uint8_t DevNum);
	void (*Stop)(uint8_t DevNum);
	void (*SendByte)(uint8_t DevNum, uint8_t _ucByte);
	void (*Ack)(uint8_t DevNum);
	void (*NAck)(uint8_t DevNum);
	uint8_t (*ReadByte)(uint8_t DevNum); 
	uint8_t (*WaitAck)(uint8_t DevNum);
	uint8_t (*CheckDevice)(uint8_t DevNum, uint8_t DevAddr);
}I2C_HandleFuncType;

/* I2C�豸����/ɾ��������� */
typedef struct
{
	uint8_t (*Create)(I2C_ParamType* I2C_Param, uint8_t* DevNum);
	uint8_t (*Delete)(uint8_t* DevNum);
}I2C_UserFuncType;

/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
extern I2C_HandleFuncType I2C_HandleFunc;	
extern I2C_UserFuncType I2C_UserFunc;
	
/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
void I2C_ParamTabInit(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

