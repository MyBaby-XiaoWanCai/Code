/*
 * CAN_BSP.h
 *
 *  Created on: 2021��6��5��
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_CAN_BSP_H_
#define DSP28377D_BSP_INCLUDE_CAN_BSP_H_
//�ṹ�嶨��

//CAN ID���ݽṹ����
typedef struct struct_ID_JN910
{
    Uint32 Dst_Addr:8;
    Uint32 Src_Addr:8;
    Uint32 Frame_Seque:8;
    Uint32 Devices_Type:3;
    Uint32 Priority_ID:2;
    Uint32 NC:3;
} struct_ID_JN910;

union union_ID_CAN
{
    Uint32 All;
    struct_ID_JN910 ID_JN910;
};

#define LEN_BUFF_CAN  72

//����֡�ṹ����
typedef struct struct_CAN_JN910_TX
{
    Uint16 Req_TX;                          //TRUE-����������
    Uint16 Num_Byte;                        //�������ֽ�����
    Uint16 Flag_Sending;                    //���ڷ��Ͷ�֡����
    Uint16 Cnt_Timeout;                     //��֡���ݷ��ͳ�ʱ������
    Uint16 Num_Timeout;                     //��֡���ݳ�ʱʱ��,1uS/LSB
    Uint16 Cnt_Byte_Sent;                   //�Ѿ����͵��ֽ���������
    Uint16 Length_Data;                     //��֡����ʱ��Tag+CMD+Data�ֽ���
    Uint16 First_TX_MBOX;                   //�����״η��ͱ��
    unsigned char pucMsgData[8];
    unsigned char Data[LEN_BUFF_CAN];
    union union_ID_CAN ID_CAN;
} struct_CAN_JN910_TX;

typedef struct struct_CAN_JN910_RX
{
    Uint16 Flag_RX;                         //TRUE-���յ�һ��Ϻ���ʽ������
    Uint16 Cnt_Byte_RX;                     //���յ��������ֽ�������֡Ϊ8����֡ΪTag+CMD+Data+1�ֽ���
    Uint16 Num_To_RX;                       //��֡����ʱ��Tag+CMD+Data+1�ֽ���
    Uint16 Length_Data;                     //��֡����ʱ��Tag+CMD+Data�ֽ���
    Uint16 Flag_Receiving;                  //���ڽ��ն�֡����
    Uint16 Cnt_Timeout;                     //��֡���ݽ��ճ�ʱ������
    Uint16 Num_Timeout;                     //��֡���ݳ�ʱʱ��,1uS/LSB
     int16 Error_Code;                      //�������ݴ��������
    unsigned char Data[LEN_BUFF_CAN];
    union union_ID_CAN ID_CAN;
} struct_CAN_JN910_RX;

#define BUFF_RX_NUM             3


#define CAN_BUSY                2            //�ϲ��߼�æ
#define CAN_END                 1            //
#define CAN_OK                  0            //
#define CAN_ERR_DATA_LEN_CHECK -1            //
#define CAN_ERR_DATA_CHECK     -2            //
#define CAN_ERR_DATA_RANGE     -3            //
#define CAN_ERR_PARSER         -4            //
#define CAN_ERR_OPERATE        -5            //
#define CAN_ERR_CODE           -6            //
#define CAN_ERR_EXEC           -7            //
#define CAN_ERR_PACKAGE        -8            //
#define CAN_ERR_NO_PACKAGE     -9            //

//��֡�����
#define CMD_CODE_MULTI_FRAME    0x0000
#define Q_MOD_ADD               0x0001
#define EMERGENCY_STOP          0x0023
#define CLR_PROTECT             0x0024
#define TEST_CAN                0x01FE

//���������
#define Q_IDN_MULTI             0x0001
#define Q_MOD_VIP_MULTI         0x0002
#define Q_ERR_CODE_MULTI        0x0004
#define Q_CALI_STATUS_MULTI     0x0005

#define SET_OUTPUT_MULTI        0x0021
#define SET_REPORT_MULTI        0x0031

#define TEST_CAN_MULTI          0x007E

//JN910����ģʽ����
#define CC_C_JN910             0x00
#define CCCV_C_JN910           0x01
#define CC_D_JN910             0x02
#define CCCV_D_JN910           0x03
#define REST_JN910             0x04
#define CV_C_JN910             0x07
#define CV_D_JN910             0x08

typedef struct struct_Active_Report
{
    Uint32 Cnt_Period;          //1mS/LSB
    Uint32 Num_Period;          //1mS/LSB
    Uint16 Req_Report;
    Uint16 Flag_Status_Error_Report;
    Uint16 Flag_Vol_Cur_Report;
    Uint16 Flag_Temp_Bat_Report;
} struct_Active_Report;

//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern struct_CAN_JN910_TX      CAN_JN910_TX;
extern struct_CAN_JN910_RX      CAN_JN910_RX;
extern struct_Active_Report     Active_Report;
extern struct_CAN_JN910_RX      CAN_JN910_RX_Broadcast;
extern struct_CAN_JN910_TX      CAN_JN910_TX_Error;
//���ⲿ�ļ����ź�������
extern void InitCAN_BSP(void);
extern __interrupt void canbISR(void);
extern void CAN_Driver_JN910(void);
extern void CAN_APP_JN910(void);






#define TXCOUNT  100
#define MSG_DATA_LENGTH  8
#define TX_MSG_OBJ_ID    1
#define RX_MSG_OBJ_ID    2
//extern tCANMsgObject sTXCANMessage;
//extern tCANMsgObject sRXCANMessage;

#endif /* DSP28377D_BSP_INCLUDE_CAN_BSP_H_ */
