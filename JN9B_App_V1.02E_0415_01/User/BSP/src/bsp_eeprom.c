/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_eeprom.c
** Author      : Huang Cheng
** Date        : 2022-01-24
** Description : ��GPIOģ��I2C����,ʵ��24xxϵ��EEPROM�Ķ�д����,д��������ҳдģʽ���д��Ч��
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_eeprom.h"
#include "bsp_dwt.h"
#include "global.h"
#include "bsp.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : ee_CheckOk()
**  ����˵�� : �ж�EERPOM�Ƿ�����
**  ��    �� : ��
**  �� �� ֵ : 0 - �쳣
**             1 - ����
**********************************************************************************************************/
uint8_t ee_CheckOk(void)
{
    if(i2c_CheckDevice(I2C_EE_TEMP,EE_DEV_ADDR) == 0)
    {
        return 1;
    }
    else
    {
        i2c_Stop(I2C_EE_TEMP);     /* Necessary */
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
	  int32_t ls, us;
    Dis_int();
    ls = osKernelLock();
	
    uint16_t i;

    /* ����EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */

    /* ��1��������I2C���������ź� */
    i2c_Start(I2C_EE_TEMP);

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    i2c_SendByte(I2C_EE_TEMP, EE_DEV_ADDR | I2C_WR);

    /* ��3��������ACK */
    if(i2c_WaitAck(I2C_EE_TEMP) != 0)
    {
        goto cmd_fail;      /* EEPROM������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
    if(EE_ADDR_BYTES == 1)
    {
        i2c_SendByte(I2C_EE_TEMP,(uint8_t)_usAddress);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM������Ӧ�� */
        }
    }
    else
    {
        i2c_SendByte(I2C_EE_TEMP, _usAddress >> 8);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM������Ӧ�� */
        }

        i2c_SendByte(I2C_EE_TEMP, _usAddress);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM������Ӧ�� */
        }
    }

    /* ��6������������I2C���ߣ����濪ʼ��ȡ���� */
    i2c_Start(I2C_EE_TEMP);

    /* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    i2c_SendByte(I2C_EE_TEMP,EE_DEV_ADDR | I2C_RD);

    /* ��8��������ACK */
    if(i2c_WaitAck(I2C_EE_TEMP) != 0)
    {
        goto cmd_fail;      /* EEPROM������Ӧ�� */
    }

    /* ��9����ѭ����ȡ���� */
    for(i = 0; i < _usSize; i++)
    {
        _pReadBuf[i] = i2c_ReadByte(I2C_EE_TEMP);

        /* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
        if(i != _usSize - 1)
        {
            i2c_Ack(I2C_EE_TEMP);      /* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
        }
        else
        {
            i2c_NAck(I2C_EE_TEMP);     /* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
        }
    }
    /* ����I2C����ֹͣ�ź� */
    i2c_Stop(I2C_EE_TEMP);
    us = osKernelUnlock();
    osKernelRestoreLock(us);
    osKernelRestoreLock(ls);
    En_int();
		bsp_DelayMS(2);
    return 1;               /* ִ�гɹ� */

    /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
cmd_fail:

    i2c_Stop(I2C_EE_TEMP);
	
    for(uint8_t j = 0; j < CHANNEL_CFG; j++)
    { 
			IP_Alarm_Flag[j] |= IntelProtect_EEE; 
		}  //ee����
	  us = osKernelUnlock();
    osKernelRestoreLock(us);
    osKernelRestoreLock(ls);
    En_int();
		bsp_DelayMS(2);
		return 0;               /* ִ��ʧ�� */
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
uint8_t ee_ReadBytes_wc(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
	
    uint16_t i;

    /* ����EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */

    /* ��1��������I2C���������ź� */
    i2c_Start(I2C_EE_TEMP);

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    i2c_SendByte(I2C_EE_TEMP, EE_DEV_ADDR | I2C_WR);

    /* ��3��������ACK */
    if(i2c_WaitAck(I2C_EE_TEMP) != 0)
    {
        goto cmd_fail;      /* EEPROM������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
    if(EE_ADDR_BYTES == 1)
    {
        i2c_SendByte(I2C_EE_TEMP,(uint8_t)_usAddress);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM������Ӧ�� */
        }
    }
    else
    {
        i2c_SendByte(I2C_EE_TEMP, _usAddress >> 8);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM������Ӧ�� */
        }

        i2c_SendByte(I2C_EE_TEMP, _usAddress);
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;  /* EEPROM������Ӧ�� */
        }
    }

    /* ��6������������I2C���ߣ����濪ʼ��ȡ���� */
    i2c_Start(I2C_EE_TEMP);

    /* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    i2c_SendByte(I2C_EE_TEMP,EE_DEV_ADDR | I2C_RD);

    /* ��8��������ACK */
    if(i2c_WaitAck(I2C_EE_TEMP) != 0)
    {
        goto cmd_fail;      /* EEPROM������Ӧ�� */
    }

    /* ��9����ѭ����ȡ���� */
    for(i = 0; i < _usSize; i++)
    {
        _pReadBuf[i] = i2c_ReadByte(I2C_EE_TEMP);

        /* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
        if(i != _usSize - 1)
        {
            i2c_Ack(I2C_EE_TEMP);      /* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
        }
        else
        {
            i2c_NAck(I2C_EE_TEMP);     /* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
        }
    }
    /* ����I2C����ֹͣ�ź� */
    i2c_Stop(I2C_EE_TEMP);

    return 1;               /* ִ�гɹ� */

    /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
cmd_fail:

    i2c_Stop(I2C_EE_TEMP);
	
    for(uint8_t j = 0; j < CHANNEL_CFG; j++)
    { 
			IP_Alarm_Flag[j] |= IntelProtect_EEE; 
		}  //ee����
	
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
            i2c_Stop(I2C_EE_TEMP);

            /* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С��10ms */
            for(m = 0; m < 1000; m++)
            {
                /* ��1��������I2C���������ź� */
                i2c_Start(I2C_EE_TEMP);

                /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
                i2c_SendByte(I2C_EE_TEMP, EE_DEV_ADDR | I2C_WR);

                /* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
                if(i2c_WaitAck(I2C_EE_TEMP) == 0)
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
                i2c_SendByte(I2C_EE_TEMP, (uint8_t)usAddr);
                if(i2c_WaitAck(I2C_EE_TEMP) != 0)
                {
                    goto cmd_fail;      /* EEPROM������Ӧ�� */
                }
            }
            else
            {
                i2c_SendByte(I2C_EE_TEMP, usAddr >> 8);
                if(i2c_WaitAck(I2C_EE_TEMP) != 0)
                {
                    goto cmd_fail;      /* EEPROM������Ӧ�� */
                }

                i2c_SendByte(I2C_EE_TEMP, usAddr);
                if(i2c_WaitAck(I2C_EE_TEMP) != 0)
                {
                    goto cmd_fail;      /* EEPROM������Ӧ�� */
                }
            }
        }

        /* ��6������ʼд������ */
        i2c_SendByte(I2C_EE_TEMP, _pWriteBuf[i]);

        /* ��7��������ACK */
        if(i2c_WaitAck(I2C_EE_TEMP) != 0)
        {
            goto cmd_fail;              /* EEPROM������Ӧ�� */
        }

        usAddr++;
    }

    /* ����ִ�гɹ�������I2C����ֹͣ�ź� */
    i2c_Stop(I2C_EE_TEMP);

    /* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С��10ms */
    for(m = 0; m < 1000; m++)
    {
        /* ��1��������I2C���������ź� */
        i2c_Start(I2C_EE_TEMP);

        /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
#if EE_ADDR_A8 == 1
        i2c_SendByte(EE_DEV_ADDR | I2C_WR | ((_usAddress >> 7) & 0x0E));
#else
        i2c_SendByte(I2C_EE_TEMP, EE_DEV_ADDR | I2C_WR);
#endif

        /* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
        if(i2c_WaitAck(I2C_EE_TEMP) == 0)
        {
            break;
        }
    }
    if(m  == 1000)
    {
        goto cmd_fail;                  /* EEPROM����д��ʱ */
    }

    /* ����ִ�гɹ�������I2C����ֹͣ�ź� */
    i2c_Stop(I2C_EE_TEMP);

    return 1;

    /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
cmd_fail:

    i2c_Stop(I2C_EE_TEMP);
	
    for(uint8_t j = 0; j < CHANNEL_CFG; j++)
    { 
			IP_Alarm_Flag[j] |= IntelProtect_EEE; 
		}  //ee����
	
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
	
    int32_t ls, us;
    Dis_int();
    ls = osKernelLock(); 
	
    ee_WriteBytes(_pWriteBuf, _usAddress, _usSize); //д��
    us = osKernelUnlock();
    osKernelRestoreLock(us);
    osKernelRestoreLock(ls);
    En_int();
    for(i = 0; i < _usSize; i++)
    {
        ee_ReadBytes_wc(&readate, _usAddress + i, 1); //�������

        if(readate != _pWriteBuf[i])
        {
					for(uint8_t j = 0; j < CHANNEL_CFG; j++)
					{ 
						IP_Alarm_Flag[j] |= IntelProtect_EEE; 
					}  //ee����
					
					return 0;
        }
    }
    return 1;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

