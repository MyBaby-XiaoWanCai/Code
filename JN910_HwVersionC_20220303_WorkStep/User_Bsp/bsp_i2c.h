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
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"
#include <stdint.h>

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#define I2C_QUEUE_HEAD    0U
#define I2C_QUEUE_TAIL    3U
#define I2C_DEVICE_NUM    3U
#define I2C_DEVTAB_SIZE   4U                  //设备表表头不存存储设备信息，只存储游标信息
#define DELETE_ID         255U
#define I2C_DEVICE_OK     0U
#define I2C_DEVICE_ERR    1U

#define I2C_WR			  0U														           
#define I2C_RD			  1U														            
/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
/* I2C设备端口时钟类型定义 */
typedef struct
{
	GPIO_TypeDef* Port;
	uint32_t GPIO_CLK;
}I2C_PortClkType;

/* I2C设备PortPin定义 */
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

/* I2C设备表定义 */
#pragma pack(1)
typedef struct
{
	I2C_ParamType I2C_Param;
	uint8_t DeviceNext;
}I2C_ParamTabType;
#pragma pack()

/* I2C设备延时/读写Pin函数句柄 */
typedef struct
{
	void (*Delay)(uint8_t nus);
	void (*WritePin)(GPIO_TypeDef* Port, uint16_t Pin, GPIO_PinState PinState);
	GPIO_PinState (*ReadPin)(GPIO_TypeDef* Port, uint16_t Pin);
}I2C_RegFuncType;

/* I2C设备函数操作句柄 */
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

/* I2C设备创建/删除函数句柄 */
typedef struct
{
	uint8_t (*Create)(I2C_ParamType* I2C_Param, uint8_t* DevNum);
	uint8_t (*Delete)(uint8_t* DevNum);
}I2C_UserFuncType;

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern I2C_HandleFuncType I2C_HandleFunc;	
extern I2C_UserFuncType I2C_UserFunc;
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void I2C_ParamTabInit(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

