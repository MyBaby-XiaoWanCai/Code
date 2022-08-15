/*
*********************************************************************************************************
*
*	ģ������ : 24AA256T-E/ST ����
*	�ļ����� : eeprom.c
*	��    �� : V1.0
*	˵    �� : ʹ�����I2Cʵ��
*
*
*
*	�޸ļ�¼ :
*	�汾��  ����         ����     ˵��
*	V1.0    2020-03-04   ZYL      ��ʽ����
*********************************************************************************************************
*/
#include "eeprom.h"
#include "main.h"

uint16_t Boot_Flag=0;

/*
    24AA256T-E/ST��������:
  400K���Ҽ���
  ʵ��ʹ��370Khz���ң�����ʾ��������
*/

#define SCL_Time   55

/* ����GPIO�˿� */
#define eeprom_PORT_SCL    GPIOH
#define eeprom_PIN_SCL		 GPIO_PIN_7

#define eeprom_PORT_SDA    GPIOH
#define eeprom_PIN_SDA		 GPIO_PIN_8

/* ���������0����1�ĺ� */
#define eeprom_SCL_0()		HAL_GPIO_WritePin(eeprom_PORT_SCL, eeprom_PIN_SCL, GPIO_PIN_RESET);
#define eeprom_SCL_1()		HAL_GPIO_WritePin(eeprom_PORT_SCL, eeprom_PIN_SCL, GPIO_PIN_SET);
#define eeprom_SDA_0()		HAL_GPIO_WritePin(eeprom_PORT_SDA, eeprom_PIN_SDA, GPIO_PIN_RESET);
#define eeprom_SDA_1()		HAL_GPIO_WritePin(eeprom_PORT_SDA, eeprom_PIN_SDA, GPIO_PIN_SET);

#define eeprom_SCL_state()	(HAL_GPIO_ReadPin(eeprom_PORT_SCL, eeprom_PIN_SCL))
#define eeprom_SDA_state()	(HAL_GPIO_ReadPin(eeprom_PORT_SDA, eeprom_PIN_SDA))


/*
*********************************************************************************************************
*	�� �� ��: InitI2C
*	����˵��: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void InitI2C(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    /* ��GPIOʱ�� */
    __HAL_RCC_GPIOH_CLK_ENABLE(); /* ��GPIOHʱ�� */

    GPIO_Initure.Pin = eeprom_PIN_SCL | eeprom_PIN_SDA; /*���� */
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD; /* ��Ϊ��©ģʽ */
    GPIO_Initure.Pull = GPIO_NOPULL;         /* ���������費ʹ�� */
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO������ٶ� */
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);     //GPIOH

    /* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
    i2c_Stop();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Delay
*	����˵��: I2C����λ�ӳ٣����400KHz
*	��    ��:
*	�� �� ֵ: ��
*********************************************************************************************************
*/

static void i2c_Delay(uint32_t cnt)
{
    uint32_t i;

    /*��
        CPU��Ƶ180MHzʱ�����ڲ�Flash����, MDK���̲��Ż�����̨ʽʾ�����۲Ⲩ�Ρ�
        ʵ��Ӧ��ѡ��400KHz���ҵ����ʼ���
    */
    for (i = 0; i < cnt; i++);

}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C���������ź�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Start(void)
{
    /* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
    eeprom_SDA_1();
    eeprom_SCL_1();
    i2c_Delay(SCL_Time);
    eeprom_SDA_0();
    i2c_Delay(SCL_Time);

    eeprom_SCL_0();
    i2c_Delay(SCL_Time);
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Stop
*	����˵��: CPU����I2C����ֹͣ�ź�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Stop(void)
{
    /* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
    eeprom_SDA_0();
    eeprom_SCL_1();
    i2c_Delay(SCL_Time);
    eeprom_SDA_1();
    i2c_Delay(SCL_Time);
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    ��:  _ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
    uint8_t i;
     eeprom_SCL_0();
    i2c_Delay(SCL_Time);
    /* �ȷ����ֽڵĸ�λbit7 */
    for (i = 0; i < 8; i++)
    {
        if (_ucByte & 0x80)
        {
            eeprom_SDA_1();
        }
        else
        {
            eeprom_SDA_0();
        }
        i2c_Delay(SCL_Time);
        eeprom_SCL_1();
        i2c_Delay(SCL_Time);
        _ucByte <<= 1;	/* ����һ��bit */
        eeprom_SCL_0();
        i2c_Delay(SCL_Time);
    }
    i2c_WaitAck();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_ReadByte
*	����˵��: CPU��I2C�����豸��ȡ8bit����
*	��    ��:  ��
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
    uint8_t i;
    uint8_t value;

    /* ������1��bitΪ���ݵ�bit7 */
    value = 0;
    for (i = 0; i < 8; i++)
    {
        value <<= 1;
        eeprom_SCL_1();
        i2c_Delay(SCL_Time);
        value |= eeprom_SDA_state();
        eeprom_SCL_0();
        i2c_Delay(SCL_Time);
    }
    i2c_NAck();
    return value;
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    ��:  ��
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
      uint32_t i;
    eeprom_SDA_1();	/* CPU�ͷ�SDA���� */
    i2c_Delay(SCL_Time);
    eeprom_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
    i2c_Delay(SCL_Time);
    while(eeprom_SDA_state()) //�ȴ�EEPROM ����͵�ƽ,4ms���˳�ѭ��
    {
        i++;
            if (i >= 180000)
            {
                i2c_Stop();
                return(1);
            }
    }
    eeprom_SCL_0();
    i2c_Delay(SCL_Time);
    return(0);
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Ack(void)
{
    eeprom_SDA_0();	/* CPU����SDA = 0 */
    i2c_Delay(SCL_Time);
    eeprom_SCL_1();	/* CPU����1��ʱ�� */
    i2c_Delay(SCL_Time);
    eeprom_SCL_0();
    i2c_Delay(SCL_Time);
    eeprom_SDA_1();	/* CPU�ͷ�SDA���� */
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_NAck(void)
{
    eeprom_SDA_1();	/* CPU����SDA = 1 */
    i2c_Delay(SCL_Time);
    eeprom_SCL_1();	/* CPU����1��ʱ�� */
    i2c_Delay(SCL_Time);
    eeprom_SCL_0();
    i2c_Delay(SCL_Time);
}


uint8_t EEPROM_ReadOneByte(uint16_t ReadAddr)
{
    uint8_t temp=0;
    i2c_Start();
    i2c_SendByte(0XA0);
    i2c_SendByte(ReadAddr>>8);
    i2c_SendByte(ReadAddr);
    i2c_Start();
    i2c_SendByte(0XA1);
    temp=i2c_ReadByte();
    i2c_Stop();
    i2c_Delay(100);
    return temp;
}

void EEPROM_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{
    i2c_Start();
    i2c_SendByte(0XA0);
    i2c_SendByte(WriteAddr>>8);
    i2c_SendByte(WriteAddr);
    i2c_SendByte(DataToWrite);
    i2c_Stop();
    // i2c_Delay(200022);
    HAL_Delay(5);
}

void EEPROM_WriteLenByte(uint16_t WriteAddr, uint32_t DataToWrite, uint8_t Len)
{
    u8 t;
    for(t = 0; t < Len; t++) {
        EEPROM_WriteOneByte(WriteAddr+t, (DataToWrite>>(8*t))&0xff);
    }
}

uint32_t EEPROM_ReadLenByte(uint16_t ReadAddr, uint8_t Len)
{
    u8 t;
    u32 temp;
    for(t = 0; t < Len; t++) {
        temp <<= 8;
        temp += EEPROM_ReadOneByte(ReadAddr+Len-1-t);
    }
    return temp;
}


/*****************************(END OF FILE) *********************************/
