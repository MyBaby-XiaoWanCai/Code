/**********************************************************************************************************
** File name   : i2c.c
** Author      : Huang Cheng
** Date        : 2022-01-17
** Description : ���ģ��I2C���ײ�ע�ắ������Cubemx HAL�⺯��ʵ�֣���ʹ�ñ�׼�⺯�����������޸�
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_dwt.h"
#include "bsp_i2c.h"

/**********************************************************************************************************
**                                           ����������
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
**                                         ȫ�ֱ���������
**********************************************************************************************************/
/* I2Cע���豸������ */
static uint8_t I2C_DevNum = 0x00;

/* I2C�豸�˿�ʱ��ʹ�ܱ� */
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

/* I2Cע���豸��������ͷֻ�洢�α���Ϣ���α귨ʵ������ */
I2C_ParamTabType   I2C_ParamTab[I2C_DEVTAB_SIZE] = {0};

/* I2CӲ��ע�ắ�� */
I2C_RegFuncType    I2C_RegFunc = 
{
	i2c_Delay, 
	i2c_WritePin, 
	i2c_ReadPin,
};

/* I2C�������� */
I2C_HandleFuncType I2C_HandleFunc = 
{
	i2c_Init,         //HAL���Ѿ���ʼ�����˴�ʵ�ʲ��ã�ֻΪ�������error
	i2c_Start,
	i2c_Stop,
	i2c_SendByte,
	i2c_Ack,
	i2c_NAck,
	i2c_ReadByte,
	i2c_WaitAck,
	i2c_CheckDevice,
};

/* I2C�豸����/ɾ������ */
I2C_UserFuncType I2C_UserFunc = 
{
	i2c_Create,
	i2c_Delete,
};

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/
/**********************************************************************************************************
**  �� �� �� : I2C_ParamTabInit()
**  ����˵�� : I2C������ʼ������
**  ��    �� : ��
**  �� �� ֵ : ��
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
**  �� �� �� : i2c_Init()
**  ����˵�� : I2C���߳�ʼ������
**  ��    �� : Port - �˿ڣ� Pin - ����
**  �� �� ֵ : ��
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
**  �� �� �� : i2c_Delay()
**  ����˵�� : ʵ��I2C��ʱ����
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
static void i2c_Delay(uint8_t nus)
{
	//ע����ʱ����������systick/dwt��ʱ
    bsp_DelayUS(nus);
}

/**********************************************************************************************************
**  �� �� �� : i2c_WritePin()
**  ����˵�� : I2C����д�˿����ź���
**  ��    �� : Port - �˿ڣ� Pin - ����, PinState - ����״̬
**  �� �� ֵ : ��
**********************************************************************************************************/
static void i2c_WritePin(GPIO_TypeDef* Port, uint16_t Pin, GPIO_PinState PinState)
{
	//ע��GPIO Pinд����
	HAL_GPIO_WritePin((GPIO_TypeDef*)Port, Pin, PinState);
}

/**********************************************************************************************************
**  �� �� �� : i2c_ReadPin()
**  ����˵�� : I2C���߶��˿����ź���
**  ��    �� : Port - �˿ڣ� Pin - ����
**  �� �� ֵ : PinSta - ����״̬
**********************************************************************************************************/
static GPIO_PinState i2c_ReadPin(GPIO_TypeDef* Port, uint16_t Pin)
{
	//ע��GPIO Pin������
	GPIO_PinState PinSta = GPIO_PIN_RESET;
	
	PinSta = HAL_GPIO_ReadPin((GPIO_TypeDef*)Port, Pin);
	
	return PinSta;
}

/**********************************************************************************************************
**  �� �� �� : i2c_Create()
**  ����˵�� : I2C�����豸��������
**  ��    �� : I2C_Param - ��������
**  �� �� ֵ : ��
**********************************************************************************************************/
static uint8_t i2c_Create(I2C_ParamType* I2C_Param, uint8_t* DevNum)
{
	uint8_t TempCur = 0x00;
	
	if((I2C_DevNum>=I2C_DEVTAB_SIZE) || (NULL==I2C_Param) || (NULL==DevNum))
	{
		return I2C_DEVICE_ERR;
	}
	
	//�������ͷ���洢�豸��Ϣ
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
**  �� �� �� : i2c_Delete()
**  ����˵�� : I2C�����豸ɾ������
**  ��    �� : DevNum - I2C�����豸ID
**  �� �� ֵ : ��
**********************************************************************************************************/
static uint8_t i2c_Delete(uint8_t* DevNum)
{
	if((I2C_DevNum <= 0x00) || ((I2C_DevNum>=I2C_DEVTAB_SIZE)) || (NULL==DevNum))
	{
		return I2C_DEVICE_ERR;
	}
	
	/* �����ǰ�豸�ڲ�����Ĳ��������޸��α�ָ��ֵ */
	I2C_ParamTab[*DevNum].I2C_Param.SCL_Port = NULL;
	I2C_ParamTab[*DevNum].I2C_Param.SCL_Pin = 0x00;
	I2C_ParamTab[*DevNum].I2C_Param.SDA_Port = NULL;
	I2C_ParamTab[*DevNum].I2C_Param.SDA_Pin = 0x00;
	I2C_ParamTab[*DevNum].I2C_Param.DelayTime = 0x00;
	
	I2C_ParamTab[*DevNum].DeviceNext = I2C_ParamTab[I2C_QUEUE_HEAD].DeviceNext;
	I2C_ParamTab[I2C_QUEUE_HEAD].DeviceNext = *DevNum;
	
	/* �豸����ڽ���ɾ������������Ϊ��ɾ���ı�� */
	*DevNum = DELETE_ID;
	I2C_DevNum--;
	
	return I2C_DEVICE_OK;
}

/**********************************************************************************************************
**  �� �� �� : i2c_Start()
**  ����˵�� : I2C���������ź�
**  ��    �� : DevNum - �豸����
**  �� �� ֵ : ��
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
**  �� �� �� : i2c_Stop()
**  ����˵�� : I2C����ֹͣ�ź�
**  ��    �� : DevNum - �豸����
**  �� �� ֵ : ��
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
**  �� �� �� : i2c_SendByte()
**  ����˵�� : CPU��I2C�����豸���͵����ֽ�����
**  ��    �� : DevNum - �豸���ͣ�_ucByte - �ȴ����͵ĵ����ֽ�����
**  �� �� ֵ : ��
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
**  �� �� �� : i2c_ReadByte()
**  ����˵�� : CPU��I2C�����豸��ȡ�����ֽ�����
**  ��    �� : DevNum - �豸����
**  �� �� ֵ : ��ȡ���ĵ����ֽ�����
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
**  �� �� �� : i2c_WaitAck()
**  ����˵�� : CPU����һ��ʱ��,����ȡ������ACKӦ���ź�
**  ��    �� : DevNum - �豸����
**  �� �� ֵ : 0 - ��ȷӦ��
**             1 - ��������Ӧ
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
**  �� �� �� : i2c_Ack()
**  ����˵�� : CPU����һ��ACK�ź�
**  ��    �� : DevNum - �豸����
**  �� �� ֵ : ��
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
**  �� �� �� : i2c_NAck()
**  ����˵�� : CPU����һ��NACK�ź�
**  ��    �� : DevNum - �豸����
**  �� �� ֵ : ��
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
**  �� �� �� : i2c_CheckDevice()
**  ����˵�� : ���I2C�����豸(CPU�����豸��ַ,Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����)
**  ��    �� : DevNum - �豸���ͣ� _Address - �豸��ַ
**  �� �� ֵ : 0 - I2C��������
**             1 - I2C�����쳣
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

		/* �����豸��ַ����д����λ(0-д,1-��) */
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

