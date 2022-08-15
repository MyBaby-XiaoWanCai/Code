/**********************************************************************************************************
** File name   : i2c.c
** Author      : Huang Cheng
** Date        : 2022-01-17
** Description : 软件模拟I2C，底层注册函数基于Cubemx HAL库函数实现，若使用标准库函数则需自行修改
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_dwt.h"
#include "bsp_i2c.h"

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void i2c_Init(GPIO_TypeDef* Port, uint16_t Pin);
static void i2c_Delay(uint8_t nus);
static void i2c_WritePin(GPIO_TypeDef* Port, uint16_t Pin, GPIO_PinState PinState);
static GPIO_PinState i2c_ReadPin(GPIO_TypeDef* Port, uint16_t Pin);
static void i2c_Start(uint8_t DevNum);
static void i2c_Stop(uint8_t DevNum);
static void i2c_SendByte(uint8_t DevNum, uint8_t _ucByte);
static uint8_t i2c_ReadByte(uint8_t DevNum);
static uint8_t i2c_WaitAck(uint8_t DevNum);
static void i2c_Ack(uint8_t DevNum);
static void i2c_NAck(uint8_t DevNum);
static uint8_t i2c_CheckDevice(uint8_t DevNum, uint8_t _Address);
static uint8_t i2c_Create(I2C_ParamType* I2C_Param, uint8_t* DevNum);
static uint8_t i2c_Delete(uint8_t* DevNum);

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
/* I2C注册设备的数量 */
static uint8_t I2C_DevNum = 0x00;

/* I2C设备端口时钟使能表 */
I2C_PortClkType I2C_PortClkTab[] = 
{
	{GPIOA, RCC_AHB1ENR_GPIOAEN},
	{GPIOB, RCC_AHB1ENR_GPIOBEN},
	{GPIOC, RCC_AHB1ENR_GPIOCEN},
	{GPIOD, RCC_AHB1ENR_GPIODEN},
	{GPIOE, RCC_AHB1ENR_GPIOEEN},
	{GPIOF, RCC_AHB1ENR_GPIOFEN},
	{GPIOG, RCC_AHB1ENR_GPIOGEN},
	{GPIOH, RCC_AHB1ENR_GPIOHEN},
	{GPIOI, RCC_AHB1ENR_GPIOIEN},
	{GPIOJ, RCC_AHB1ENR_GPIOJEN},
	{GPIOK, RCC_AHB1ENR_GPIOKEN},
};

/* I2C注册设备参数表，表头只存储游标信息，游标法实现链表 */
I2C_ParamTabType   I2C_ParamTab[I2C_DEVTAB_SIZE] = {0};

/* I2C硬件注册函数 */
I2C_RegFuncType    I2C_RegFunc = 
{
	i2c_Delay, 
	i2c_WritePin, 
	i2c_ReadPin,
};

/* I2C操作函数 */
I2C_HandleFuncType I2C_HandleFunc = 
{
	i2c_Init,         //HAL库已经初始化，此处实际不用，只为解决编译error
	i2c_Start,
	i2c_Stop,
	i2c_SendByte,
	i2c_Ack,
	i2c_NAck,
	i2c_ReadByte,
	i2c_WaitAck,
	i2c_CheckDevice,
};

/* I2C设备创建/删除函数 */
I2C_UserFuncType I2C_UserFunc = 
{
	i2c_Create,
	i2c_Delete,
};

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/**********************************************************************************************************
**  函 数 名 : I2C_ParamTabInit()
**  功能说明 : I2C参数初始化函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void I2C_ParamTabInit(void)
{
	uint8_t i = 0;
	
	for(i = 0; i < (I2C_DEVTAB_SIZE-1); i++)
	{
		I2C_ParamTab[i].I2C_Param.SCL_Port = NULL;
		I2C_ParamTab[i].I2C_Param.SCL_Pin = 0;
		I2C_ParamTab[i].I2C_Param.SDA_Port = NULL;
		I2C_ParamTab[i].I2C_Param.SDA_Pin = 0;
		I2C_ParamTab[i].I2C_Param.DelayTime = 0;
		I2C_ParamTab[i].DeviceNext = i+1;
	}
	
	I2C_ParamTab[i].I2C_Param.SCL_Port = NULL;
	I2C_ParamTab[i].I2C_Param.SCL_Pin = 0;
	I2C_ParamTab[i].I2C_Param.SDA_Port = NULL;
	I2C_ParamTab[i].I2C_Param.SDA_Pin = 0;
	I2C_ParamTab[i].I2C_Param.DelayTime = 0;
	I2C_ParamTab[i].DeviceNext = 0;
}

/**********************************************************************************************************
**  函 数 名 : i2c_Init()
**  功能说明 : I2C总线初始化函数
**  形    参 : Port - 端口， Pin - 引脚
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_Init(GPIO_TypeDef* Port, uint16_t Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	__IO uint32_t tmpreg = 0x00U;

	/* GPIO Ports Clock Enable */
	for(uint8_t i = 0; i < sizeof(I2C_PortClkTab)/sizeof(I2C_PortClkTab[0]); i++)
	{
	  if(Port == I2C_PortClkTab[i].Port)
	  {		  
		  SET_BIT(RCC->AHB1ENR, I2C_PortClkTab[i].GPIO_CLK);
		  /* Delay after an RCC peripheral clock enabling */ 
		  tmpreg = READ_BIT(RCC->AHB1ENR, I2C_PortClkTab[i].GPIO_CLK);
		  UNUSED(tmpreg);
		  break;
	  }
	}

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin((GPIO_TypeDef*)Port, Pin, GPIO_PIN_SET);

	/*Configure GPIO pins */
	GPIO_InitStruct.Pin = Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init((GPIO_TypeDef*)Port, &GPIO_InitStruct);
}

/**********************************************************************************************************
**  函 数 名 : i2c_Delay()
**  功能说明 : 实现I2C延时功能
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_Delay(uint8_t nus)
{
	//注册延时函数，可用systick/dwt延时
    bsp_DelayUS(nus);
}

/**********************************************************************************************************
**  函 数 名 : i2c_WritePin()
**  功能说明 : I2C总线写端口引脚函数
**  形    参 : Port - 端口， Pin - 引脚, PinState - 引脚状态
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_WritePin(GPIO_TypeDef* Port, uint16_t Pin, GPIO_PinState PinState)
{
	//注册GPIO Pin写函数
	HAL_GPIO_WritePin((GPIO_TypeDef*)Port, Pin, PinState);
}

/**********************************************************************************************************
**  函 数 名 : i2c_ReadPin()
**  功能说明 : I2C总线读端口引脚函数
**  形    参 : Port - 端口， Pin - 引脚
**  返 回 值 : PinSta - 引脚状态
**********************************************************************************************************/
static GPIO_PinState i2c_ReadPin(GPIO_TypeDef* Port, uint16_t Pin)
{
	//注册GPIO Pin读函数
	GPIO_PinState PinSta = GPIO_PIN_RESET;
	
	PinSta = HAL_GPIO_ReadPin((GPIO_TypeDef*)Port, Pin);
	
	return PinSta;
}

/**********************************************************************************************************
**  函 数 名 : i2c_Create()
**  功能说明 : I2C总线设备创建函数
**  形    参 : I2C_Param - 创建参数
**  返 回 值 : 无
**********************************************************************************************************/
static uint8_t i2c_Create(I2C_ParamType* I2C_Param, uint8_t* DevNum)
{
	uint8_t TempCur = 0x00;
	
	if((I2C_DevNum>=I2C_DEVTAB_SIZE) || (NULL==I2C_Param) || (NULL==DevNum))
	{
		return I2C_DEVICE_ERR;
	}
	
	//参数表表头不存储设备信息
	TempCur = I2C_ParamTab[I2C_QUEUE_HEAD].DeviceNext;
	I2C_ParamTab[TempCur].I2C_Param.SCL_Port = I2C_Param->SCL_Port;
	I2C_ParamTab[TempCur].I2C_Param.SCL_Pin = I2C_Param->SCL_Pin;
	I2C_ParamTab[TempCur].I2C_Param.SDA_Port = I2C_Param->SDA_Port;
	I2C_ParamTab[TempCur].I2C_Param.SDA_Pin = I2C_Param->SDA_Pin;
	I2C_ParamTab[TempCur].I2C_Param.DelayTime = I2C_Param->DelayTime;
	*DevNum =  TempCur;
	
	I2C_ParamTab[I2C_QUEUE_HEAD].DeviceNext = I2C_ParamTab[TempCur].DeviceNext;
	
	I2C_DevNum++;
	
	return I2C_DEVICE_OK;
}

/**********************************************************************************************************
**  函 数 名 : i2c_Delete()
**  功能说明 : I2C总线设备删除函数
**  形    参 : DevNum - I2C总线设备ID
**  返 回 值 : 无
**********************************************************************************************************/
static uint8_t i2c_Delete(uint8_t* DevNum)
{
	if((I2C_DevNum <= 0x00) || ((I2C_DevNum>=I2C_DEVTAB_SIZE)) || (NULL==DevNum))
	{
		return I2C_DEVICE_ERR;
	}
	
	/* 清除当前设备在参数表的参数，并修改游标指向值 */
	I2C_ParamTab[*DevNum].I2C_Param.SCL_Port = NULL;
	I2C_ParamTab[*DevNum].I2C_Param.SCL_Pin = 0x00;
	I2C_ParamTab[*DevNum].I2C_Param.SDA_Port = NULL;
	I2C_ParamTab[*DevNum].I2C_Param.SDA_Pin = 0x00;
	I2C_ParamTab[*DevNum].I2C_Param.DelayTime = 0x00;
	
	I2C_ParamTab[*DevNum].DeviceNext = I2C_ParamTab[I2C_QUEUE_HEAD].DeviceNext;
	I2C_ParamTab[I2C_QUEUE_HEAD].DeviceNext = *DevNum;
	
	/* 设备编号在进行删除操作后需标记为已删除的编号 */
	*DevNum = DELETE_ID;
	I2C_DevNum--;
	
	return I2C_DEVICE_OK;
}

/**********************************************************************************************************
**  函 数 名 : i2c_Start()
**  功能说明 : I2C总线启动信号
**  形    参 : DevNum - 设备类型
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_Start(uint8_t DevNum)
{
	I2C_RegFuncType *RegFunc = &I2C_RegFunc;

	if((DevNum<=I2C_QUEUE_HEAD) && (DevNum>I2C_QUEUE_TAIL))
	{
		return;
	}
	
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin, GPIO_PIN_SET);
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_SET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin, GPIO_PIN_RESET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_RESET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
}

/**********************************************************************************************************
**  函 数 名 : i2c_Stop()
**  功能说明 : I2C总线停止信号
**  形    参 : DevNum - 设备类型
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_Stop(uint8_t DevNum)
{
	I2C_RegFuncType *RegFunc = &I2C_RegFunc;
	
	if((DevNum<=I2C_QUEUE_HEAD) && (DevNum>I2C_QUEUE_TAIL))
	{
		return;
	}
	
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin, GPIO_PIN_RESET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_SET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin, GPIO_PIN_SET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
}

/**********************************************************************************************************
**  函 数 名 : i2c_SendByte()
**  功能说明 : CPU向I2C总线设备发送单个字节数据
**  形    参 : DevNum - 设备类型，_ucByte - 等待发送的单个字节数据
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_SendByte(uint8_t DevNum, uint8_t _ucByte)
{
	I2C_RegFuncType *RegFunc = &I2C_RegFunc;
	
	if((DevNum<=I2C_QUEUE_HEAD) && (DevNum>I2C_QUEUE_TAIL))
	{
		return;
	}
	
	for(uint8_t i = 0; i < 8; i++)
	{
		if(_ucByte & 0x80)
		{
			RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin, GPIO_PIN_SET);
		}
		else
		{
			RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin, GPIO_PIN_RESET);
		}
		
		RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
		RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_SET);
		RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
		RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_RESET);
		RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_RESET);
		
		if(i == 7)
		{
			RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin, GPIO_PIN_SET);
		}
		_ucByte <<= 1;
	}
}

/**********************************************************************************************************
**  函 数 名 : i2c_ReadByte()
**  功能说明 : CPU从I2C总线设备读取单个字节数据
**  形    参 : DevNum - 设备类型
**  返 回 值 : 读取到的单个字节数据
**********************************************************************************************************/
static uint8_t i2c_ReadByte(uint8_t DevNum)
{
    I2C_RegFuncType *RegFunc = &I2C_RegFunc;
	uint8_t value = 0;
	
	if((DevNum>I2C_QUEUE_HEAD) && (DevNum<=I2C_QUEUE_TAIL))
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			value <<= 1;
			RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_SET);
			RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
			
			if(RegFunc->ReadPin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin))
			{
				value++;
			}
			
			RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_RESET);
			RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
		}
	}
	else
	{
		//do nothing;
	}

    return value;
}

/**********************************************************************************************************
**  函 数 名 : i2c_WaitAck()
**  功能说明 : CPU产生一个时钟,并读取器件的ACK应答信号
**  形    参 : DevNum - 设备类型
**  返 回 值 : 0 - 正确应答
**             1 - 无器件响应
**********************************************************************************************************/
static uint8_t i2c_WaitAck(uint8_t DevNum)
{
    I2C_RegFuncType *RegFunc = &I2C_RegFunc;
	uint8_t ret = I2C_DEVICE_OK;
	
	if((DevNum<=I2C_QUEUE_HEAD) && (DevNum>I2C_QUEUE_TAIL))
	{
		return I2C_DEVICE_ERR;
	}
	
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin, GPIO_PIN_SET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_SET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
	
	if(RegFunc->ReadPin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin))
	{
		ret = I2C_DEVICE_ERR;
	}
	else
	{
		ret = I2C_DEVICE_OK;
	}
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_RESET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
		
    return ret;
}

/**********************************************************************************************************
**  函 数 名 : i2c_Ack()
**  功能说明 : CPU产生一个ACK信号
**  形    参 : DevNum - 设备类型
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_Ack(uint8_t DevNum)
{
	I2C_RegFuncType *RegFunc = &I2C_RegFunc;
	
	if((DevNum<=I2C_QUEUE_HEAD) && (DevNum>I2C_QUEUE_TAIL))
	{
		return;
	}
	
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin, GPIO_PIN_RESET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_SET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_RESET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin, GPIO_PIN_SET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
}

/**********************************************************************************************************
**  函 数 名 : i2c_NAck()
**  功能说明 : CPU产生一个NACK信号
**  形    参 : DevNum - 设备类型
**  返 回 值 : 无
**********************************************************************************************************/
static void i2c_NAck(uint8_t DevNum)
{
	I2C_RegFuncType *RegFunc = &I2C_RegFunc;
	
	if((DevNum<=I2C_QUEUE_HEAD) && (DevNum>I2C_QUEUE_TAIL))
	{
		return;
	}
	
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin, GPIO_PIN_SET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_SET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
	RegFunc->WritePin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin, GPIO_PIN_RESET);
	RegFunc->Delay(I2C_ParamTab[DevNum].I2C_Param.DelayTime);
}

/**********************************************************************************************************
**  函 数 名 : i2c_CheckDevice()
**  功能说明 : 检测I2C总线设备(CPU发送设备地址,然后读取设备应答来判断该设备是否存在)
**  形    参 : DevNum - 设备类型， _Address - 设备地址
**  返 回 值 : 0 - I2C总线正常
**             1 - I2C总线异常
**********************************************************************************************************/
static uint8_t i2c_CheckDevice(uint8_t DevNum, uint8_t _Address)
{
    I2C_RegFuncType *RegFunc = &I2C_RegFunc;
	uint8_t ucAck = I2C_DEVICE_OK;
	
	if((DevNum<=I2C_QUEUE_HEAD) && (DevNum>I2C_QUEUE_TAIL))
	{
		return I2C_DEVICE_ERR;
	}
	
	if(RegFunc->ReadPin(I2C_ParamTab[DevNum].I2C_Param.SDA_Port, I2C_ParamTab[DevNum].I2C_Param.SDA_Pin) && 
	   RegFunc->ReadPin(I2C_ParamTab[DevNum].I2C_Param.SCL_Port, I2C_ParamTab[DevNum].I2C_Param.SCL_Pin))
	{
		i2c_Start(DevNum);

		/* 发送设备地址及读写控制位(0-写,1-读) */
		i2c_SendByte(DevNum, _Address | I2C_WR);
		ucAck = i2c_WaitAck(DevNum);

		i2c_Stop(DevNum);

		return ucAck;
	}
    
    return I2C_DEVICE_ERR;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

