/*
*********************************************************************************************************
*
*   ģ������ : MB85RS2MTA����ģ��(��ʿͨ����洢��2M��256K*8��bits)
*   �ļ����� : bsp_MB85RS2.c
*   ��    �� : V1.0
*   ˵    �� : MB85RS2MTAģ���CPU֮�����SPI�ӿڡ�����������ʹ��Ӳ��SPI��
*
*
*   �޸ļ�¼ :
*       �汾��  ����         ����     ˵��
*       V1.0    2021-08-09    WL      ��ʽ����
*********************************************************************************************************
*/

#ifndef _MB85RS2_H_
#define _MB85RS2_H_

#include "stdint.h"
#include "bsp_spi.h"

#define MB85RS2_CMD_WREN 		0x06      //дʹ��
#define MB85RS2_CMD_WRDI		0x04      //д��ֹ
#define MB85RS2_CMD_RDSR		0x05      //��״̬�Ĵ���
#define MB85RS2_CMD_WRSR		0x01      //д״̬�Ĵ���
#define MB85RS2_CMD_READ		0x03      //24λ��ַģʽ������ 
#define MB85RS2_CMD_WRITE		0x02      //д����
#define MB85RS2_CMD_FSTRD       0x0B      //32λ��ַģʽ����λ���룬��λ��0��������
#define MB85RS2_CMD_RDID		0x9f      //��ID��32λID
#define MB85RS2_CMD_SLEEP		0xB9      //�͹���ģʽ

#define MB85_CS_PIN		GPIO_PIN_15
#define MB85_CS_PORT 	GPIOB
#define MB85_CS_SET	    FRAM_CS_DISABLE()
#define MB85_CS_CLR		FRAM_CS_ENABLE()

void MB85Ctrl_Init(void);
void MB85Ctrl_Read(uint32_t addr, uint8_t *buf, uint16_t len);
void MB85Ctrl_Write(uint32_t addr, uint8_t *buf, uint16_t len);
void MB85Ctrl_ReadID(uint32_t *buf);
extern uint8_t MB85Ctrl_CheckWrite(uint32_t addr, uint8_t *buf, uint16_t len);

#define StartADD_WorkStatus	0        // ��һ�ζϵ繤��״̬��¼��������ʼ��ַ��8�ֽڣ�
#define StartADD_MB85RS2DeviceSN   8        // �豸SN�����ţ�3�ֽڣ�
#define MB85RS2_Size  0x40000          //256K Byte
//#define RemoteAdd  (MB85RS2_Size-10)             //Զ�˷�������ַ
//#define RemotePort (RemoteAdd+4)                //Զ�˷������˿�
//#define RemoteEn   (RemoteAdd+6)                //Զ�˷���������ʹ��   

//#define MACAdd  (MB85RS2_Size-15)             //���루MAC��3�ֽڣ��洢��ַ
//#define MACEn   (MACAdd+3)                    //��������ʹ��

#endif
