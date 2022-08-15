/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_eeprom.c
** Author      : Cai Heng        Huang Cheng
** Date        : 2021-01-16      2022-01-17(modify)
** Description : ��GPIOģ��I2C����,ʵ��24xxϵ��EEPROM�Ķ�д����,д��������ҳдģʽ���д��Ч��
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_i2c.h"
#include "bsp_eeprom.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
static uint8_t I2cEeprom = 0x00;                        //I2C����EEPROM���豸��

/* EEPROM��I2C�豸���� */
static I2C_ParamType EepromParam = 
{
	I2C1_SCL_GPIO_Port,
	I2C1_SCL_Pin,
	I2C1_SDA_GPIO_Port,
	I2C1_SDA_Pin,
	2,
};

I2C_HandleFuncType *I2C_EepromHandle = &I2C_HandleFunc; //I2C�豸�����������
static I2C_UserFuncType *I2C_EEPROM = &I2C_UserFunc;    //I2C�豸�������

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : bsp_EepromInit()
**  ����˵�� : EERPOM��ʼ������
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void bsp_EepromInit(void)
{
	uint8_t CreateSta = 0x00;
	
	//��������I2C�豸
	CreateSta = I2C_EEPROM->Create(&EepromParam, &I2cEeprom);
	
	//�����豸�ɹ�
	if(I2C_DEVICE_OK == CreateSta)
	{
		/* ��һ��ֹͣ�ź�,��λI2C�����ϵ������豸������ģʽ */
		I2C_EepromHandle->Stop(I2cEeprom);
	}
	else
	{
		//do nothing;
	}
}

/**********************************************************************************************************
**  �� �� �� : ee_CheckOk()
**  ����˵�� : �ж�EERPOM�Ƿ�����
**  ��    �� : ��
**  �� �� ֵ : 0 - �쳣
**             1 - ����
**********************************************************************************************************/
uint8_t ee_CheckOk(void)
{
    if(I2C_EepromHandle->CheckDevice(I2cEeprom, EE_DEV_ADDR) == 0)
    {
        return 1;
    }
    else
    {
        I2C_EepromHandle->Stop(I2cEeprom);     /* Necessary */
        return 0;
    }
}

/**********************************************************************************************************
**  �� �� �� : ee_ReadBytes()
**  ����˵�� : ��EEPROMָ����ַ����ȡ��������
**  ��    �� : *_pReadBuf - ��ȡ���ݻ�����ָ��
**             _usAddress - ָ���Ķ�ȡ��ʼ��ַ
**             _usSize    - ��ȡ���ݳ���(�ֽ�)
**  �� �� ֵ : 0 - ��ȡʧ��
**             1 - ��ȡ�ɹ�
**********************************************************************************************************/
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i;

    /* ����EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */

    /* ��1��������I2C���������ź� */
    I2C_EepromHandle->Start(I2cEeprom);

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    I2C_EepromHandle->SendByte(I2cEeprom, EE_DEV_ADDR|I2C_WR);

    /* ��3��������ACK */
    if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
    {
        goto cmd_fail;      /* EEPROM������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
    if(EE_ADDR_BYTES == 1)
    {
        I2C_EepromHandle->SendByte(I2cEeprom, (uint8_t)_usAddress);
        if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
        {
            goto cmd_fail;  /* EEPROM������Ӧ�� */
        }
    }
    else
    {
        I2C_EepromHandle->SendByte(I2cEeprom, (_usAddress>>8));
        if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
        {
            goto cmd_fail;  /* EEPROM������Ӧ�� */
        }

        I2C_EepromHandle->SendByte(I2cEeprom, _usAddress);
        if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
        {
            goto cmd_fail;  /* EEPROM������Ӧ�� */
        }
    }

    /* ��6������������I2C���ߣ����濪ʼ��ȡ���� */
    I2C_EepromHandle->Start(I2cEeprom);

    /* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    I2C_EepromHandle->SendByte(I2cEeprom, EE_DEV_ADDR|I2C_RD);

    /* ��8��������ACK */
    if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
    {
        goto cmd_fail;      /* EEPROM������Ӧ�� */
    }

    /* ��9����ѭ����ȡ���� */
    for(i = 0; i < _usSize; i++)
    {
        _pReadBuf[i] = I2C_EepromHandle->ReadByte(I2cEeprom);

        /* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
        if(i != _usSize - 1)
        {
            I2C_EepromHandle->Ack(I2cEeprom);      /* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
        }
        else
        {
            I2C_EepromHandle->NAck(I2cEeprom);     /* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
        }
    }
    /* ����I2C����ֹͣ�ź� */
    I2C_EepromHandle->Stop(I2cEeprom);

    return 1;               /* ִ�гɹ� */

    /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
cmd_fail:

    I2C_EepromHandle->Stop(I2cEeprom);

    return 0;               /* ִ��ʧ�� */
}

/**********************************************************************************************************
**  �� �� �� : ee_WriteBytes()
**  ����˵�� : ��EEPROMָ����ַд����������,����ҳд�������д��Ч��
**  ��    �� : *_pWriteBuf - д�����ݻ�����ָ��
**             _usAddress  - ָ����д����ʼ��ַ
**             _usSize     - д�����ݳ���(�ֽ�)
**  �� �� ֵ : 0 - д��ʧ��
**             1 - д��ɹ�
**********************************************************************************************************/
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i, m;
    uint16_t usAddr;

    usAddr = _usAddress;
    for(i = 0; i < _usSize; i++)
    {
        /* �����͵�1���ֽڻ���ҳ���׵�ַʱ����Ҫ���·��������źź͵�ַ */
        if((i == 0) || (usAddr & (EE_PAGE_SIZE - 1)) == 0)
        {
            /*���ڣ���������ֹͣ�źţ������ڲ�д������*/
            I2C_EepromHandle->Stop(I2cEeprom);

            /* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С��10ms */
            for(m = 0; m < 1000; m++)
            {
                /* ��1��������I2C���������ź� */
                I2C_EepromHandle->Start(I2cEeprom);

                /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
                I2C_EepromHandle->SendByte(I2cEeprom, EE_DEV_ADDR|I2C_WR);

                /* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
                if(I2C_EepromHandle->WaitAck(I2cEeprom) == 0)
                {
                    break;
                }
            }
            if(m  == 1000)
            {
                goto cmd_fail;          /* EEPROM����д��ʱ */
            }

            /* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
            if(EE_ADDR_BYTES == 1)
            {
                I2C_EepromHandle->SendByte(I2cEeprom, (uint8_t)usAddr);
                if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
                {
                    goto cmd_fail;      /* EEPROM������Ӧ�� */
                }
            }
            else
            {
                I2C_EepromHandle->SendByte(I2cEeprom, (usAddr >> 8));
                if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
                {
                    goto cmd_fail;      /* EEPROM������Ӧ�� */
                }

                I2C_EepromHandle->SendByte(I2cEeprom, usAddr);
                if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
                {
                    goto cmd_fail;      /* EEPROM������Ӧ�� */
                }
            }
        }

        /* ��6������ʼд������ */
        I2C_EepromHandle->SendByte(I2cEeprom, _pWriteBuf[i]);

        /* ��7��������ACK */
        if(I2C_EepromHandle->WaitAck(I2cEeprom) != 0)
        {
            goto cmd_fail;              /* EEPROM������Ӧ�� */
        }

        usAddr++;
    }

    /* ����ִ�гɹ�������I2C����ֹͣ�ź� */
    I2C_EepromHandle->Stop(I2cEeprom);

    /* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С��10ms */
    for(m = 0; m < 1000; m++)
    {
        /* ��1��������I2C���������ź� */
        I2C_EepromHandle->Start(I2cEeprom);

        /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
#if EE_ADDR_A8 == 1
        i2c_SendByte(EE_DEV_ADDR | I2C_WR | ((_usAddress >> 7) & 0x0E));
#else
        I2C_EepromHandle->SendByte(I2cEeprom, EE_DEV_ADDR|I2C_WR);
#endif

        /* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
        if(I2C_EepromHandle->WaitAck(I2cEeprom) == 0)
        {
            break;
        }
    }
    if(m  == 1000)
    {
        goto cmd_fail;                  /* EEPROM����д��ʱ */
    }

    /* ����ִ�гɹ�������I2C����ֹͣ�ź� */
    I2C_EepromHandle->Stop(I2cEeprom);

    return 1;

    /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
cmd_fail:

    I2C_EepromHandle->Stop(I2cEeprom);

    return 0;
}

/**********************************************************************************************************
**  �� �� �� : ee_WriteBytesWithCheck()
**  ����˵�� : ��check��EEPROMָ����ַд����������,����ҳд�������д��Ч��
**  ��    �� : *_pWriteBuf - д�����ݻ�����ָ��
**             _usAddress  - ָ����д����ʼ��ַ
**             _usSize     - д�����ݳ���(�ֽ�)
**  �� �� ֵ : 0 - д��ʧ��
**             1 - д��ɹ�
**********************************************************************************************************/
uint8_t ee_WriteBytesWithCheck(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint8_t readate;
    uint16_t i;

    ee_WriteBytes(_pWriteBuf, _usAddress, _usSize); //д��

    for(i = 0; i < _usSize; i++)
    {
 
        ee_ReadBytes(&readate, _usAddress + i, 1); //�������
    }

    return 1;
}



/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

