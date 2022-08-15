/*
*********************************************************************************************************
*
*   ģ������ : AD5245����ģ��(256λ�ɱ�̵�λ��)
*   �ļ����� : bsp_AD5245.h
*   ��    �� : V1.0
*   ˵    �� : AD5245ģ���CPU֮�����IIC�ӿڡ�����������ʹ��ģ��IIC��
*
*
*   �޸ļ�¼ :
*       �汾��  ����         ����     ˵��
*       V1.0    2021-08-09   WL      ��ʽ����
*********************************************************************************************************
*/

#ifndef _AD5245_H_
#define _AD5245_H_

#include "stdint.h"


/*д�ӻ���ַ�ֽڵ㶨��*/
#define Write_A0_Add  0x58   //�ӻ���ַ=0

/*���ӻ���ַ�ֽڵ㶨��*/
#define Read_A0_Add  0x59   //�ӻ���ַ=0

/*ָ���ֽڶ���*/
#define Normal_SET   0x00   //�α���ͨ���� 
#define RS_SD_SET    0x60   //�α긴λ���ض�
#define RS_SET       0x40   //�α긴λ
#define SD_SET       0x20   //�ض�

/*��ʱ�ȴ�ʱ��*/
#define IIC_TIMOUT   1000

//IO������
#define AD5245_SDA_Pin GPIO_PIN_4
#define AD5245_SDA_GPIO_Port GPIOH
#define AD5245_SCL_Pin GPIO_PIN_5
#define AD5245_SCL_GPIO_Port GPIOH

#define IIC_SCL(state)    HAL_GPIO_WritePin(GPIOH,AD5245_SCL_Pin,(GPIO_PinState)(state))
#define IIC_SDA(state)    HAL_GPIO_WritePin(GPIOH,AD5245_SDA_Pin,(GPIO_PinState)(state))
#define	SDA_IN            GPIOH->MODER&=0xFFFFFCFF
#define	SDA_OUT           GPIOH->MODER|=1<<8
#define READ_SDA          HAL_GPIO_ReadPin(GPIOH,AD5245_SDA_Pin)

//IIC����ʱ���������
static void IIC_Init(void);
static void IIC_Start(void);
static void IIC_Stop(void);
static void IIC_Send_Byte(uint8_t txd);
static uint8_t IIC_Read_Byte(uint8_t ack);
static uint8_t IIC_Wait_Ack(void);
static void IIC_Ack(void);
static void IIC_NAck(void);

void AD5245_Init(void);
int AD5245_Value_Set(uint8_t addr, uint8_t value);
int AD5245_Vpro_Set(float Vpro);
uint8_t AD5245_Value_Read(uint8_t addr);
void AD5245_Full_RS(uint8_t addr);
void AD5245_Half_RS(uint8_t addr);
void AD5245_SD(uint8_t addr);

#endif
