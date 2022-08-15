/*
 * SCI_BSP.h
 *
 *  Created on: 2021年8月6日
 *      Author: JC
 */

#ifndef F2837XS_BSP_INCLUDE_SCI_BSP_H_
#define F2837XS_BSP_INCLUDE_SCI_BSP_H_

//常量定义
#define LEN_SCI_BUFF       64   //64字节
#define TIMEOVER_FRAME     100  //100mS,一帧数据内部字节之间最长时间间隔，超时清除不完整帧

//结构体定义
typedef struct struct_SCI_RX
{
    Uint16 Cnt_RX;                      //当前已经接收到的字节数
    Uint16 State_Recived;               //数据帧接收状态，FALSE-无数据，TRUE-有数据
    Uint16 Cnt_Timeout;                 //数据帧不完整接收超时计数器
    Uint16 Num_Timeout;                 //数据帧不完整接收超时时间
    unsigned char  Data[LEN_SCI_BUFF];
} struct_SCI_RX;

typedef struct struct_SCI_TX
{
    Uint16 State_TX;                    //数据发送状态，TRUE-正在发送，不要操作缓冲区，FALSE-发送完成可以操作
    Uint16 Num_To_TX;                   //缓冲区中需要发送的字节数
    Uint16 Cnt_Sended;                  //已经发送的字节数
    unsigned char  Data[LEN_SCI_BUFF];
} struct_SCI_TX;

//联合体定义


//对外部文件开放全局变量声明
extern struct struct_SCI_RX SCIA_RX;
extern struct struct_SCI_TX SCIA_TX;
extern void InitSCI_BSP(void);
extern interrupt void SCIAReceiveInterrupt(void);

#endif /* F2837XS_BSP_INCLUDE_SCI_BSP_H_ */
