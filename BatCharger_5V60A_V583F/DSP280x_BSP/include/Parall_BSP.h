/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Parall_BSP.h
// Created on: 2020-3-19
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef DSP280X_BSP_INCLUDE_PARALL_BSP_H_
#define DSP280X_BSP_INCLUDE_PARALL_BSP_H_

//�ṹ�嶨��
#define SYMBOL_ID0          0x5C
#define SYMBOL_ID1          0xC5

//ID0���ݽṹ����
struct struct_ID0_H
{
    Uint32 Add_Master:8;                 //������ַ
    Uint32 Parall_Total:8;               //��������
    Uint32 NC:8;                   //����Ĺ���ģʽ��ֻ��STANDY,CC,DCC,STATIC
    Uint32 NC1:8;                //����ӻ����ϱ�־
};

union union_ID0_H
{
    Uint32 All;
    struct struct_ID0_H Bit;
};


struct struct_ID0_L
{
    Uint32 NC:24;                         //���ƴӻ������MOS�Ƿ���
    Uint32 Symbol:8;                     //�����Ƿ������ֽ�Ϊ�����ַ�ģ��Ż����ӻ�ģʽ
};

union union_ID0_L
{
    Uint32 All;
    struct struct_ID0_L Bit;
};

struct struct_ID0
{
    Uint16 Req_TX;
    Uint16 Flag_First_TX;
    Uint16 Flag_RX;
    union  union_ID0_H ID0_H;
    union  union_ID0_L ID0_L;
};

//ID1���ݽṹ����

union union_ID1_H
{
    Uint32 All;
    Uint32 Set_Cur_Total;                //�ӻ���ǰ�ܵ���
};


struct struct_ID1_L
{
    Uint32 Req_Mode:8;                  //����ӻ�����ģʽ��ֻ��STANDY,CC,CCtoCV,DCC,DCCtoDCV,STATIC
    Uint32 Req_ChangeMode:1;            //����ı�ӻ�����ģʽ
    Uint32 Req_Refresh_Cur:1;           //����ˢ�´ӻ������趨ֵ
    Uint32 Req_Fault_Clear:1;           //����ӻ����ϱ�־
    Uint32 EN_Run:1;                    //���ƴӻ��Ƿ���
    Uint32 EN_MOS_Out:1;                //���ƴӻ������MOS�Ƿ���
    Uint32 NC:3;                        //
    Uint32 Now_Mode_Master:8;           //������ǰDCDC�Ĺ���״̬���ӻ����յ�����Ϣ���Լ���״̬�Ƚϣ��쳣ʱͣ��
    Uint32 Symbol:8;                    //�����Ƿ������ֽ�Ϊ�����ַ�ģ��Ż����ӻ�ģʽ
};

union union_ID1_L
{
    Uint32 All;
    struct struct_ID1_L Bit;
};

struct struct_ID1
{
    Uint16 Req_TX;
    Uint16 Flag_First_TX;
    Uint16 Flag_RX;
    Uint16 Cnt_TX_Req_Change_Mode;
    union  union_ID1_H ID1_H;
    union  union_ID1_L ID1_L;
};

//ID2~ID5�շ����ݽṹ����
struct struct_IDx_L
{
    Uint32 Fault_Module:16;              //�ӻ�������
    Uint32 Now_Mode:8;                   //�ӻ���ǰ����״̬
    Uint32 NC1:8;                        //�ӻ���ǰ����״̬
};

union union_IDx_L
{
    Uint32 All;
    struct struct_IDx_L Bit;
};

union union_IDx_H
{
    Uint32 All;
    Uint32 Now_Cur_AB;                //�ӻ���ǰABͨ���ܵ���
};

struct struct_IDx
{
    Uint16 Req_TX;
    Uint16 Flag_First_TX;
    Uint16 Flag_RX;
    union union_IDx_H IDx_H;
    union union_IDx_L IDx_L;
};

struct struct_IDx1_L
{
    Uint32 Temp_Sink:16;              //�ӻ�������
    Uint32 Temp_Shunt:16;                   //�ӻ���ǰ����״̬
};

//IDx1�շ����ݽṹ����
union union_IDx1_L
{
    Uint32 All;
    struct struct_IDx1_L IDx1_L;                //δʹ��
};

union union_IDx1_H
{
    Uint32 All;
    Uint32 Now_Vol_Sense;           //�ӻ�1��ǰSense��ѹ����Ϊ����������ѹ��
};

struct struct_IDx1
{
    Uint16 Req_TX;
    Uint16 Flag_First_TX;
    Uint16 Flag_RX;
    union union_IDx1_H IDx_H;
    union union_IDx1_L IDx_L;
};


//�ӻ�
typedef struct struct_Parall_Slaver
{
    Uint16 Addr_Master;                 //�ӻ��յ���������ַ�������
    Uint16 Parall_Total;                //�ӻ��յ����ܲ�������
    Uint16 Cnt_Period;                  //�ӻ����ڷ��ͼ�����
    Uint16 Cnt_S_OT_M;                  //�ӻ�ģʽ���ۼ�δ�յ�����CAN����ʱ��
    struct struct_ID0   ID0_Fr_M;       //���������յ�ID0����֡
    struct struct_ID1   ID1_Fr_M;       //���������յ�ID1����֡
    struct struct_IDx   IDx_To_M;       //��Ҫ���͸�����������֡
    struct struct_IDx1	IDx1_To_M;      //��Ҫ���͸�����������֡
} struct_Parall_Slaver;

typedef struct struct_Parall_Master
{
    Uint16 Cnt_Period;                  //�������ڷ��ͼ�����
    Uint16 Cnt_M_OT_S1;                 //����ģʽ���ۼ�δ�յ��ӻ�4��CAN����ʱ��
    Uint16 Cnt_M_OT_S2;                 //����ģʽ���ۼ�δ�յ��ӻ�4��CAN����ʱ��
    Uint16 Cnt_M_OT_S3;                 //����ģʽ���ۼ�δ�յ��ӻ�4��CAN����ʱ��
    Uint16 Cnt_M_OT_S4;                 //����ģʽ���ۼ�δ�յ��ӻ�4��CAN����ʱ��
     int32 Now_Cur_Total;               //������������������
     int32 Now_Cur_S1;                  //�ӻ�1��������������
     int32 Now_Cur_S2;                  //�ӻ�2��������������
     int32 Now_Cur_S3;                  //�ӻ�3��������������
     int32 Now_Cur_S4;                  //�ӻ�4��������������
     int32 Now_Sense_S1;				//�ӻ�1Sense��ѹ
    Uint16 EEPROM_Parall_Mode;			//EEPROM�д洢�Ĳ���ģʽ
    Uint16 EEPROM_Parall_Num; 			//EEPROM�д洢�Ĳ���������
    Uint16 Cnt_Delay;					//�ϵ����ʱ������������CAN���������
    struct struct_ID0   ID0_To_Sx;
    struct struct_ID1   ID1_To_Sx;
    struct struct_IDx   IDx_Fr_S1;
    struct struct_IDx   IDx_Fr_S2;
    struct struct_IDx   IDx_Fr_S3;
    struct struct_IDx   IDx_Fr_S4;
    struct struct_IDx1  IDx1_Fr_S1; 	//�ӻ�1����������Sense��ѹ
} struct_Parall_Master;

//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern struct_Parall_Slaver Parall_Slaver;
extern struct_Parall_Master Parall_Master;
extern     struct ECAN_REGS ECanaShadow;

//���ⲿ�ļ����ź�������
extern void Init_ID_Single(void);
extern void Init_ID_Master(void);
extern void Init_ID_Slaver(void);
extern void InitCAN_BSP(void);
extern void Parall_Service(void);
extern void Slaver_Abnormal_Check(void);
extern void Read_Data_Parall(void);
extern void Auto_To_Master(void);
extern void Set_By_CAN(void);

#endif /* DSP280X_BSP_INCLUDE_PARALL_BSP_H_ */
