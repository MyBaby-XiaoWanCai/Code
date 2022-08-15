/*
 * CAN_BSP.h
 *
 *  Created on: 2021年6月5日
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_CAN_BSP_H_
#define DSP28377D_BSP_INCLUDE_CAN_BSP_H_
//结构体定义

//CAN ID数据结构定义
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

//数据帧结构定义
typedef struct struct_CAN_JN910_TX
{
    Uint16 Req_TX;                          //TRUE-请求发送数据
    Uint16 Num_Byte;                        //待发送字节数量
    Uint16 Flag_Sending;                    //正在发送多帧数据
    Uint16 Cnt_Timeout;                     //多帧数据发送超时计数器
    Uint16 Num_Timeout;                     //多帧数据超时时间,1uS/LSB
    Uint16 Cnt_Byte_Sent;                   //已经发送的字节数计数器
    Uint16 Length_Data;                     //多帧数据时，Tag+CMD+Data字节数
    Uint16 First_TX_MBOX;                   //邮箱首次发送标记
    unsigned char pucMsgData[8];
    unsigned char Data[LEN_BUFF_CAN];
    union union_ID_CAN ID_CAN;
} struct_CAN_JN910_TX;

typedef struct struct_CAN_JN910_RX
{
    Uint16 Flag_RX;                         //TRUE-接收到一组合乎格式的数据
    Uint16 Cnt_Byte_RX;                     //接收到的数据字节数，单帧为8，多帧为Tag+CMD+Data+1字节数
    Uint16 Num_To_RX;                       //多帧数据时，Tag+CMD+Data+1字节数
    Uint16 Length_Data;                     //多帧数据时，Tag+CMD+Data字节数
    Uint16 Flag_Receiving;                  //正在接收多帧数据
    Uint16 Cnt_Timeout;                     //多帧数据接收超时计数器
    Uint16 Num_Timeout;                     //多帧数据超时时间,1uS/LSB
     int16 Error_Code;                      //接收数据错误故障码
    unsigned char Data[LEN_BUFF_CAN];
    union union_ID_CAN ID_CAN;
} struct_CAN_JN910_RX;

#define BUFF_RX_NUM             3


#define CAN_BUSY                2            //上层逻辑忙
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

//单帧命令定义
#define CMD_CODE_MULTI_FRAME    0x0000
#define Q_MOD_ADD               0x0001
#define EMERGENCY_STOP          0x0023
#define CLR_PROTECT             0x0024
#define TEST_CAN                0x01FE

//单机命令定义
#define Q_IDN_MULTI             0x0001
#define Q_MOD_VIP_MULTI         0x0002
#define Q_ERR_CODE_MULTI        0x0004
#define Q_CALI_STATUS_MULTI     0x0005

#define SET_OUTPUT_MULTI        0x0021
#define SET_REPORT_MULTI        0x0031

#define TEST_CAN_MULTI          0x007E

//JN910工作模式定义
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

//联合体定义


//对外部文件开放全局变量声明
extern struct_CAN_JN910_TX      CAN_JN910_TX;
extern struct_CAN_JN910_RX      CAN_JN910_RX;
extern struct_Active_Report     Active_Report;
extern struct_CAN_JN910_RX      CAN_JN910_RX_Broadcast;
extern struct_CAN_JN910_TX      CAN_JN910_TX_Error;
//对外部文件开放函数声明
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
