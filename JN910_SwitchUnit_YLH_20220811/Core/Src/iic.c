#include "iic.h"


/* ģ��I2C��������ע��� */
I2C_HandleFunc I2C_HandleCb =
{
    i2c_Start,
    i2c_Stop,
    i2c_SendByte,
    i2c_Ack,
    i2c_NAck,
    i2c_ReadByte,
    i2c_WaitAck,
    i2c_CheckDevice,
};


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/*********************************************************************************************************
*   �� �� ��: IOSwitch(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t dir)
*   ����˵��: �л�IO�������״̬
*   ��    ��: GPIO:	GPIO�ṹ��ָ��
*			  GPIO_Pin: GPIO��ӦPin��
*			  dir: 0: ���룬1: ��� 
*   �� �� ֵ: ��
*********************************************************************************************************/
void IOSwitch(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t dir)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(dir == 1)
	{
		GPIO_InitStruct.Pin = GPIO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
	}
	else if(dir == 0)
	{
		GPIO_InitStruct.Pin = GPIO_Pin;                      //IIC��SDA����
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;                 //����ģʽ
		GPIO_InitStruct.Pull = GPIO_PULLUP;                     //����
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);   //����SDA��
	}
}

/**********************************************************************************************************
**  �� �� �� : i2c_Delay()
**  ����˵�� : ʵ��I2C��ʱ����
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void i2c_Delay(I2C_DevType DevNum)
{
    if(DevNum == I2C_EE_TEMP)
    {
        bsp_DelayUS(2);
    }
    else
    {
        bsp_DelayUS(4);
    }
}

/**********************************************************************************************************
**  �� �� �� : i2c_Start()
**  ����˵�� : I2C���������ź�
**  ��    �� : DevNum - �豸����
**  �� �� ֵ : ��
**********************************************************************************************************/
void i2c_Start(I2C_DevType DevNum)
{
    /* ��SCLΪ�ߵ�ƽʱ,SDA����һ�������ر�ʾI2C���������ź� */
    switch(DevNum)
    {
		case I2C_LED:
		{
			I2C_LED_SDA_1();
			I2C_LED_SCL_1();
			i2c_Delay(DevNum);
			I2C_LED_SDA_0();
			i2c_Delay(DevNum);
			I2C_LED_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		case I2C_NCA9555:
		{
			I2C2_INT_ADDR_SDA_1();
			I2C2_INT_ADDR_SCL_1();
			i2c_Delay(DevNum);
			I2C2_INT_ADDR_SDA_0();
			i2c_Delay(DevNum);
			I2C2_INT_ADDR_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		case I2C_EE_TEMP:
		{
			I2C1_EE_TEMP_SDA_1();
			I2C1_EE_TEMP_SCL_1();
			i2c_Delay(DevNum);
			I2C1_EE_TEMP_SDA_0();
			i2c_Delay(DevNum);
			I2C1_EE_TEMP_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		default:
		{
			break;
		}
    }
}

/**********************************************************************************************************
**  �� �� �� : i2c_Stop()
**  ����˵�� : I2C����ֹͣ�ź�
**  ��    �� : DevNum - �豸����
**  �� �� ֵ : ��
**********************************************************************************************************/
void i2c_Stop(I2C_DevType DevNum)
{
    /* ��SCL�ߵ�ƽʱ,SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
    switch(DevNum)
    {
    case I2C_LED:
    {
        I2C_LED_SDA_0();
        i2c_Delay(DevNum);
        I2C_LED_SCL_1();
        i2c_Delay(DevNum);
        I2C_LED_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_NCA9555:
    {
        I2C2_INT_ADDR_SDA_0();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SCL_1();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_EE_TEMP:
    {
        I2C1_EE_TEMP_SDA_0();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SCL_1();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    default:
    {
        break;
    }
    }
}

/**********************************************************************************************************
**  �� �� �� : i2c_SendByte()
**  ����˵�� : CPU��I2C�����豸���͵����ֽ�����
**  ��    �� : DevNum - �豸���ͣ�_ucByte - �ȴ����͵ĵ����ֽ�����
**  �� �� ֵ : ��
**********************************************************************************************************/
void i2c_SendByte(I2C_DevType DevNum, uint8_t _ucByte)
{
    uint8_t i;

    /* �ȷ����ֽڵĸ�λbit7 */
    switch(DevNum)
    {
    case I2C_LED:
    {
        for(i = 0; i < 8; i++)
        {
            if(_ucByte & 0x80)
            {
                I2C_LED_SDA_1();
            }
            else
            {
                I2C_LED_SDA_0();
            }

            i2c_Delay(DevNum);
            I2C_LED_SCL_1();
            i2c_Delay(DevNum);
            I2C_LED_SCL_0();
            I2C_LED_SCL_0();

            if(i == 7)
            {
                I2C_LED_SDA_1();
            }
            _ucByte <<= 1;
        }
        break;
    }
    case I2C_NCA9555:
    {
        for(i = 0; i < 8; i++)
        {
            if(_ucByte & 0x80)
            {
                I2C2_INT_ADDR_SDA_1();
            }
            else
            {
                I2C2_INT_ADDR_SDA_0();
            }

            i2c_Delay(DevNum);
            I2C2_INT_ADDR_SCL_1();
            i2c_Delay(DevNum);
            I2C2_INT_ADDR_SCL_0();
            I2C2_INT_ADDR_SCL_0();

            if(i == 7)
            {
                I2C2_INT_ADDR_SDA_1();
            }
            _ucByte <<= 1;
        }
        break;
    }
    case I2C_EE_TEMP:
    {
		I2C1_EE_TEMP_SCL_0();
		i2c_Delay(DevNum);
        for(i = 0; i < 8; i++)
        {
            if(_ucByte & 0x80)
            {
                I2C1_EE_TEMP_SDA_1();
            }
            else
            {
                I2C1_EE_TEMP_SDA_0();
            }

            i2c_Delay(DevNum);
            I2C1_EE_TEMP_SCL_1();
            i2c_Delay(DevNum);
            I2C1_EE_TEMP_SCL_0();
            i2c_Delay(DevNum);

            _ucByte <<= 1;
        }
		i2c_WaitAck(DevNum);
        break;
    }
    default:
    {
        break;
    }
    }
}

/**********************************************************************************************************
**  �� �� �� : i2c_ReadByte()
**  ����˵�� : CPU��I2C�����豸��ȡ�����ֽ�����
**  ��    �� : DevNum - �豸����
**  �� �� ֵ : ��ȡ���ĵ����ֽ�����
**********************************************************************************************************/
uint8_t i2c_ReadByte(I2C_DevType DevNum)
{
    uint8_t i;
    uint8_t value = 0;

    /* ��ȡ����bit1Ϊ���ݵ�bit7 */
    switch(DevNum)
    {
    case I2C_LED:
    {
        I2C_LED_SDA_IN();
        for(i = 0; i < 8; i++)
        {
            value <<= 1;
            I2C_LED_SCL_1();
            i2c_Delay(DevNum);

            if(I2C_LED_SDA_READ())
            {
                value++;
            }

            I2C_LED_SCL_0();
            i2c_Delay(DevNum);
        }
        I2C_LED_SDA_OUT();
        break;
    }
    case I2C_NCA9555:
    {
        I2C2_INT_ADDR_SDA_IN();
        for(i = 0; i < 8; i++)
        {
            value <<= 1;
            I2C2_INT_ADDR_SCL_1();
            i2c_Delay(DevNum);

            if(I2C2_INT_ADDR_SDA_READ())
            {
                value++;
            }

            I2C2_INT_ADDR_SCL_0();
            i2c_Delay(DevNum);
        }
        I2C2_INT_ADDR_SDA_OUT();
        break;
    }
    case I2C_EE_TEMP:
    {
        I2C1_EE_TEMP_SDA_IN();
        for(i = 0; i < 8; i++)
        {
            value <<= 1;
            I2C1_EE_TEMP_SCL_1();
            i2c_Delay(DevNum);
            if(I2C1_EE_TEMP_SDA_READ())
            {
                value++;
            }
            I2C1_EE_TEMP_SCL_0();
            i2c_Delay(DevNum);
        }
        I2C1_EE_TEMP_SDA_OUT();
		i2c_NAck(I2C_EE_TEMP);
        break;
    }
    default:
    {
        break;
    }
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
uint8_t i2c_WaitAck(I2C_DevType DevNum)
{
	uint8_t ret;
	uint32_t i = 0;

	switch(DevNum)
	{
		case I2C_LED:
		{
			I2C_LED_SDA_1();
			i2c_Delay(DevNum);
			I2C_LED_SCL_1();
			i2c_Delay(DevNum);
			I2C_LED_SDA_IN();
			if(I2C_LED_SDA_READ())
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			I2C_LED_SDA_OUT();
			I2C_LED_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		case I2C_NCA9555:
		{
			I2C2_INT_ADDR_SDA_1();
			i2c_Delay(DevNum);
			I2C2_INT_ADDR_SCL_1();
			i2c_Delay(DevNum);
			I2C2_INT_ADDR_SDA_IN();
			if(I2C2_INT_ADDR_SDA_READ())
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			I2C2_INT_ADDR_SDA_OUT();
			I2C2_INT_ADDR_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		case I2C_EE_TEMP:
		{
			I2C1_EE_TEMP_SDA_1();
			i2c_Delay(DevNum);
			I2C1_EE_TEMP_SCL_1();
			i2c_Delay(DevNum);
			I2C1_EE_TEMP_SDA_IN();
			i2c_Delay(DevNum);
			while(I2C1_EE_TEMP_SDA_READ())
			{
				i2c_Delay(DevNum);
				i++;
				if (i >= 1000)
				{
					break;
				}
			}
			if(i == 1000)
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			I2C1_EE_TEMP_SDA_OUT();
			I2C1_EE_TEMP_SCL_0();
			i2c_Delay(DevNum);
			break;
		}
		default:
		{
			break;
		}
	}
	return ret;
}

/**********************************************************************************************************
**  �� �� �� : i2c_Ack()
**  ����˵�� : CPU����һ��ACK�ź�
**  ��    �� : DevNum - �豸����
**  �� �� ֵ : ��
**********************************************************************************************************/
void i2c_Ack(I2C_DevType DevNum)
{
    switch(DevNum)
    {
    case I2C_LED:
    {
        I2C_LED_SDA_0();
        i2c_Delay(DevNum);
        I2C_LED_SCL_1();
        i2c_Delay(DevNum);
        I2C_LED_SCL_0();
        i2c_Delay(DevNum);
        I2C_LED_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_NCA9555:
    {
        I2C2_INT_ADDR_SDA_0();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SCL_1();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SCL_0();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_EE_TEMP:
    {
        I2C1_EE_TEMP_SDA_0();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SCL_1();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SCL_0();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SDA_1();
        i2c_Delay(DevNum);
        break;
    }
    default:
    {
        break;
    }
    }
}

/**********************************************************************************************************
**  �� �� �� : i2c_NAck()
**  ����˵�� : CPU����һ��NACK�ź�
**  ��    �� : DevNum - �豸����
**  �� �� ֵ : ��
**********************************************************************************************************/
void i2c_NAck(I2C_DevType DevNum)
{
    switch(DevNum)
    {
    case I2C_LED:
    {
        I2C_LED_SDA_1();
        i2c_Delay(DevNum);
        I2C_LED_SCL_1();
        i2c_Delay(DevNum);
        I2C_LED_SCL_0();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_NCA9555:
    {
        I2C2_INT_ADDR_SDA_1();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SCL_1();
        i2c_Delay(DevNum);
        I2C2_INT_ADDR_SCL_0();
        i2c_Delay(DevNum);
        break;
    }
    case I2C_EE_TEMP:
    {
        I2C1_EE_TEMP_SDA_1();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SCL_1();
        i2c_Delay(DevNum);
        I2C1_EE_TEMP_SCL_0();
        i2c_Delay(DevNum);
        break;
    }
    default:
    {
        break;
    }
    }
}

/**********************************************************************************************************
**  �� �� �� : i2c_CheckDevice()
**  ����˵�� : ���I2C�����豸(CPU�����豸��ַ,Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����)
**  ��    �� : DevNum - �豸���ͣ� _Address - �豸��ַ
**  �� �� ֵ : 0 - I2C��������
**             1 - I2C�����쳣
**********************************************************************************************************/
uint8_t i2c_CheckDevice(I2C_DevType DevNum, uint8_t _Address)
{
    uint8_t ucAck;

    switch(DevNum)
    {
    case I2C_LED:
    {
        I2C_LED_SDA_IN();
        I2C_LED_SCL_IN();
        if(I2C_LED_SDA_READ() && I2C_LED_SCL_READ())
        {
            I2C_LED_SDA_OUT();
            I2C_LED_SCL_OUT();
            i2c_Start(I2C_LED);

            /* �����豸��ַ����д����λ(0-д,1-��) */
            i2c_SendByte(I2C_LED, _Address | I2C_WR);
            ucAck = i2c_WaitAck(I2C_LED);

            i2c_Stop(I2C_LED);

            return ucAck;
        }
        break;
    }
    case I2C_NCA9555:
    {
        I2C2_INT_ADDR_SDA_IN();
        I2C2_INT_ADDR_SCL_IN();
        if(I2C2_INT_ADDR_SDA_READ() && I2C2_INT_ADDR_SCL_READ())
        {
            I2C2_INT_ADDR_SDA_OUT();
            I2C2_INT_ADDR_SCL_OUT();
            i2c_Start(I2C_NCA9555);

            /* �����豸��ַ����д����λ(0-д,1-��) */
            i2c_SendByte(I2C_NCA9555, _Address | I2C_WR);
            ucAck = i2c_WaitAck(I2C_NCA9555);

            i2c_Stop(I2C_NCA9555);

            return ucAck;
        }
        break;
    }
    case I2C_EE_TEMP:
    {
        I2C1_EE_TEMP_SDA_IN();
        I2C1_EE_TEMP_SCL_IN();
        if(I2C1_EE_TEMP_SDA_READ() && I2C1_EE_TEMP_SCL_READ())
        {
            I2C1_EE_TEMP_SDA_OUT();
            I2C1_EE_TEMP_SCL_OUT();
            i2c_Start(I2C_EE_TEMP);

            /* �����豸��ַ����д����λ(0-д,1-��) */
            i2c_SendByte(I2C_EE_TEMP, _Address | I2C_WR);
            ucAck = i2c_WaitAck(I2C_EE_TEMP);

            i2c_Stop(I2C_EE_TEMP);

            return ucAck;
        }
        break;
    }
    default:
    {
        break;
    }
    }

    return 1;
}

/**********************************************************************************************************
**  �� �� �� : bsp_InitI2C()
**  ����˵�� : ����I2C���ߵ�GPIO,����ģ��IO�ķ�ʽʵ��
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/

//void bsp_InitI2C(void)
//{
//    stc_gpio_init_t stcGpioCfg;

//    /* ����ģ��I2C3 Gpio������: ��©����� */
//    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
//    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
//    stcGpioCfg.u16PullUp = PIN_PU_ON;
//    stcGpioCfg.u16PinOType = PIN_OTYPE_NMOS;

//    /* д��ģ��I2C3 Gpio������ */
//    (void)GPIO_Init(LED_I2C3_SCL_GPIO, LED_I2C3_SCL_PIN, &stcGpioCfg);
//    (void)GPIO_Init(LED_I2C3_SDA_GPIO, LED_I2C3_SDA_PIN, &stcGpioCfg);

//    /* ��һ��ֹͣ�ź�,��λI2C1�����ϵ������豸������ģʽ */
//    i2c_Stop(I2C_LED);

//    /* ����ģ��I2C2 Gpio������: ��©����� */
//    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
//    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
//    stcGpioCfg.u16PullUp = PIN_PU_ON;
//    stcGpioCfg.u16PinOType = PIN_OTYPE_NMOS;

//    /* д��ģ��I2C2 Gpio������ */
//    (void)GPIO_Init(I2C2_SCL_GPIO, I2C2_SCL_PIN, &stcGpioCfg);
//    (void)GPIO_Init(I2C2_SDA_GPIO, I2C2_SDA_PIN, &stcGpioCfg);

//    /* ��һ��ֹͣ�ź�,��λI2C1�����ϵ������豸������ģʽ */
//    i2c_Stop(I2C_NCA9555);

//    /* ����ģ��I2C1 Gpio������: ��©����� */
//    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
//    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
//    stcGpioCfg.u16PullUp = PIN_PU_ON;
//    stcGpioCfg.u16PinOType = PIN_OTYPE_NMOS;

//    /* д��ģ��I2C1 Gpio������ */
//    (void)GPIO_Init(I2C1_SCL_GPIO, I2C1_SCL_PIN, &stcGpioCfg);
//    (void)GPIO_Init(I2C1_SDA_GPIO, I2C1_SDA_PIN, &stcGpioCfg);

//    /* ��һ��ֹͣ�ź�,��λI2C1�����ϵ������豸������ģʽ */
//    i2c_Stop(I2C_EE_TEMP);

//    //I2C2_SMBA  9548_Reset IO��ʼ��
//    stc_gpio_init_t I2C2SMBAGpioCfg;

//    (void)GPIO_StructInit(&I2C2SMBAGpioCfg);
//    I2C2SMBAGpioCfg.u16PinState = PIN_STATE_RESET;
//    I2C2SMBAGpioCfg.u16PullUp = PIN_PU_OFF;
//    I2C2SMBAGpioCfg.u16PinDir = PIN_DIR_OUT;
//    I2C2SMBAGpioCfg.u16PinDrv = PIN_DRV_HIGH;
//    I2C2SMBAGpioCfg.u16PinOType = PIN_OTYPE_CMOS;

//    (void)GPIO_Init(I2C2_SMBA_GPIO, I2C2_SMBA_PIN, &I2C2SMBAGpioCfg);

//    GPIO_ResetPins(I2C2_SMBA_GPIO,I2C2_SMBA_PIN);

//    bsp_DelayMS(500);
//    Feed_WDG();
//}
