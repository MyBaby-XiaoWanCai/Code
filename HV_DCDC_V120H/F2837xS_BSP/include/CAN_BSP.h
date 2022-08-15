/*
 * CAN_BSP.h
 *
 *  Created on: 2021年6月5日
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_CAN_BSP_H_
#define DSP28377D_BSP_INCLUDE_CAN_BSP_H_
//结构体定义
#define TXCOUNT  100
#define MSG_DATA_LENGTH  8
#define TX_MSG_OBJ_ID    1
#define RX_MSG_OBJ_ID    2
#define QUE_CAN_BUF_SIZE                50

#define QUE_Record_BUF_SIZE             1500

/////////////////////Queue define /////////////////////////////////////////
#define DEF_CAN_QUE_OVERFLOW            ((int16_t)(-4))
#define DEF_CAN_QUE_ERROR               ((int16_t)(-3))
#define DEF_CAN_QUE_EMPTY               ((int16_t)(-2))
#define DEF_CAN_QUE_FULL                ((int16_t)(-1))
#define DEF_CAN_QUE_OK                  ((int16_t)(0))

typedef struct
{
    Uint32 id;
    Uint16 flen;
    Uint32 stamp;
    unsigned char data[8];
}CAN_APP_PACK;


/* Can receive data struct */
typedef struct  tag_canQue_Def
{
    Uint16     OSQStart;                                                   /* 队列开始位置*/
    Uint16     OSQEnd;                                                     /* 队列结束位置 */
    Uint16     OSQIn;                                                      /* 队列头指针*/
    Uint16     OSQOut;                                                     /* 队列尾指针*/
    Uint16     OSQSize;                                                    /* 队列总大小       */
    Uint16     OSQEntries;                                                 /*  队列当前存储数据包数 */
    CAN_APP_PACK    buf[QUE_CAN_BUF_SIZE];                              /*队列缓冲区  */
}canQue_Typedef;

typedef struct  tag_recordQue_Def
{
    Uint16     OSQStart;                                                   /* 队列开始位置*/
    Uint16     OSQEnd;                                                     /* 队列结束位置 */
    Uint16     OSQIn;                                                      /* 队列头指针*/
    Uint16     OSQOut;                                                     /* 队列尾指针*/
    Uint16     OSQSize;                                                    /* 队列总大小       */
    Uint16     OSQEntries;                                                 /*  队列当前存储数据包数 */
    CAN_APP_PACK    buf[QUE_Record_BUF_SIZE];                              /*队列缓冲区  */
}Record_Que_Typedef;


typedef struct
{
        //! The CAN message identifier used for 11 or 29 bit identifiers.
        uint32_t ui32MsgID;

        //! The message identifier mask used when identifier filtering is enabled.
        uint32_t ui32MsgIDMask;

        //! This value holds various status flags and settings specified by
        //! tCANObjFlags.
        uint32_t ui32Flags;

        //! This value is the number of bytes of data in the message object.
        uint32_t ui32MsgLen;

        unsigned char Data[8];
        //! This is a pointer to the message object's data.
        unsigned char *pucMsgData;
}
tCANMsgObject;

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


extern canQue_Typedef      que1Con;
extern canQue_Typedef      que2Con;
extern Record_Que_Typedef  que5Con;
extern tCANMsgObject sTXCANMessage;
extern tCANMsgObject sRXCANMessage;
extern tCANMsgObject sTXCAN_JN910;
extern tCANMsgObject sTXRecord_JN910;
extern CAN_APP_PACK pack;
extern CAN_APP_PACK pack_tx;
extern CAN_APP_PACK pack_tx_record;
extern int16_t  BSP_CAN_Que_Out(canQue_Typedef *que, CAN_APP_PACK *outData);
extern int16_t  BSP_CAN_Que_In(canQue_Typedef *que,tCANMsgObject *inMsg);
extern int16_t  BSP_Record_Que_In(Record_Que_Typedef *que,tCANMsgObject *inMsg);
extern int16_t  BSP_Record_Que_Out(Record_Que_Typedef *que, CAN_APP_PACK *outData);
extern void InitCAN_BSP(void);
extern __interrupt void canbISR(void);






#endif /* DSP28377D_BSP_INCLUDE_CAN_BSP_H_ */
