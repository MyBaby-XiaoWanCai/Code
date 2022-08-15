/*
 * SCI_BSP.h
 *
 *  Created on: 2021��8��6��
 *      Author: JC
 */

#ifndef F2837XS_BSP_INCLUDE_SCI_BSP_H_
#define F2837XS_BSP_INCLUDE_SCI_BSP_H_

//��������
#define LEN_SCI_BUFF       64   //64�ֽ�
#define TIMEOVER_FRAME     100  //100mS,һ֡�����ڲ��ֽ�֮���ʱ��������ʱ���������֡

//�ṹ�嶨��
typedef struct struct_SCI_RX
{
    Uint16 Cnt_RX;                      //��ǰ�Ѿ����յ����ֽ���
    Uint16 State_Recived;               //����֡����״̬��FALSE-�����ݣ�TRUE-������
    Uint16 Cnt_Timeout;                 //����֡���������ճ�ʱ������
    Uint16 Num_Timeout;                 //����֡���������ճ�ʱʱ��
    unsigned char  Data[LEN_SCI_BUFF];
} struct_SCI_RX;

typedef struct struct_SCI_TX
{
    Uint16 State_TX;                    //���ݷ���״̬��TRUE-���ڷ��ͣ���Ҫ������������FALSE-������ɿ��Բ���
    Uint16 Num_To_TX;                   //����������Ҫ���͵��ֽ���
    Uint16 Cnt_Sended;                  //�Ѿ����͵��ֽ���
    unsigned char  Data[LEN_SCI_BUFF];
} struct_SCI_TX;

//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern struct struct_SCI_RX SCIA_RX;
extern struct struct_SCI_TX SCIA_TX;
extern void InitSCI_BSP(void);
extern interrupt void SCIAReceiveInterrupt(void);

#endif /* F2837XS_BSP_INCLUDE_SCI_BSP_H_ */
