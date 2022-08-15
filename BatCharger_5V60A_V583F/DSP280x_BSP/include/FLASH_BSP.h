/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: FLASH_BSP.h
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef DSP2803X_BSP_INCLUDE_FLASH_BSP_H_
#define DSP2803X_BSP_INCLUDE_FLASH_BSP_H_

//��������
#define NUM_SERVICE_FLASH       3                           //Flash��д����ӿ�����
#define LEN_SPI_FLASH_BUFF      177                         //Flash��д�������ݻ���������

#define SERV_FLASH_WORKSTEP     0                           //�������̹���ģ��ʹ�õ�FLASH��������
#define SERV_FLASH_NET          1                           //���ڱ��湤������ʹ�õ�FLASH��������
#define SERV_FLASH_DEBUG        2                           //���ڵ���ʹ�õ�FLASH��������

//�ṹ�嶨��
typedef struct INTERFACE_SPI_FLASH
{
                 Uint16 Req_RW;                             //client->service��д����Req_RWΪ����ʱ����ʹ��
                 Uint16 Result_RW;                          //service->client��д���
                 Uint32 Addr_RW;                            //����д�����ֽ���FLASH�е���ʼ��ַ
                 Uint32 Offset_Sector;                       //������Sector������˳���
                 Uint32 Offset_In_Sector;                   //����д�����ֽ���Sector��ʼ�ֽڵ�ַ
                 Uint16 Num_Bytes;                          //����д�ֽ���,����С�ڵ���LEN_SPI_FLASH_BUFF
                 Uint16 Num_Overtime;                       //��д��ʱʱ��,50uS/LSB
                 Uint16 Len_Buff;                           //��ǰ�ӿ����ݶ���д�뻺������С
          unsigned char Data[LEN_SPI_FLASH_BUFF];           //���ݶ���д�뻺����
    struct INTERFACE_SPI_FLASH *Ptr_Next;
} INTERFACE_SPI_FLASH;

typedef struct STATUS_SPI_FLASH_SERVICE
{
                Uint16 State;                               //Flash��д״̬��״̬��־
                Uint16 Cnt_Bytes;                           //�Ѷ�д����ֽ���������
                Uint16 Cnt_Page_Write;                      //
                Uint16 Num_Last_RW;                         //ǰ�ζ�д�ֽ���
                Uint16 Cnt_Overtime;                        //��ʱʱ�������
                Uint16 Cnt_Error;                           //���߳�������ۼƼ��������ڳ���һ��ֵ��λSPIģ�飬��ȷ��ȡһ�κ�����
                Uint16 Serv_Ready;                          //�����Ƿ����ʹ�ñ�־��0-�����ԣ�1-����
                Uint32 Addr_RW_Now;                             //��ǰ������д��ַ�ۼ���
 struct INTERFACE_SPI_FLASH *Ptr;                           //��ǰSPI�ӿ��������������������ָ��
} STATUS_SPI_FLASH_SERVICE;

//client->service������
#define REQ_SPI_FLASH_IDLE            0           //0x00-����
#define REQ_SPI_FLASH_WRITE           1           //0x01-����д
#define REQ_SPI_FLASH_READ            2           //0x02-�����

//service->client��д�������
#define SPI_FLASH_IDLE                0           //0x00-����
#define SPI_FLASH_WORKING             1           //0x01-���ڶ�д״̬
#define SPI_FLASH_WRITE_OK            2           //0x02-д�ɹ�
#define SPI_FLASH_READ_OK             3           //0x03-���ɹ�
#define SPI_FLASH_WRITE_ERR           4           //0x04-дʧ��
#define SPI_FLASH_READ_ERR            5           //0x05-��ʧ��


//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern INTERFACE_SPI_FLASH Interface_SPI_Flash[];

//���ⲿ�ļ����ź�������
extern void   InitFlash_BSP(void);
extern void   SPI_Flash_Service(void);
extern Uint16 Reset_SPI_FLASH_Service(Uint16 Index);
extern Uint16 Qurey_SPI_Flash_Read(Uint16 Index);
extern Uint16 SPI_Flash_Read(Uint16 Index,Uint32 Addr,Uint16 Num);
extern Uint16 Qurey_SPI_Flash_Write(Uint16 Index);
extern Uint16 SPI_Flash_Write(Uint16 Index,Uint32 Addr,Uint16 Num,unsigned char *Ptr);
#endif /* DSP2803X_BSP_INCLUDE_FLASH_BSP_H_ */
