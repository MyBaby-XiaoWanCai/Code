/*
*********************************************************************************************************
*
*	模块名称 : Net_Others
*	文件名称 : Net_Others.h
*	版    本 : V1.0
*	说    明 :
*	修改记录 :
*	版本号  日期         作者       说明
*	V1.0    2020-03-10   ZYL        正式发布
*
*********************************************************************************************************
*/
#include "main.h"
//全局变量定义
uint16_t Num_Cabinet=1;
uint16_t Addr_Module=1;

//函数声明
//以下供测试使用，收发数据已注明
//*********************************************************************************
//Description: 网口工作相关协议数据处理
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Socket_Work(void)
{

    if((TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive==0x01)&&(TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX==0))
    {
        uint16_t temp=TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];//接收数据包第一个字节
        uint16_t Data_CRC;
        if(temp==0x55)
        {
            //TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout    = 0;//Socket超时计数器清零//sSetCommunicationStatus(cSuccess);
            //TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus     = 1;//收到一次心跳包后才进行超时检测
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0] = 0x55;//发送数据包第一个字节
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[1] = 0xAA;
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=2;//发送两字节
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;//置发送标志
        }
        TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;//清除接收标志
    }
}

void Socket_Receive(void)
{
    if((!(TCP_SERVER_FLAG & 0x40))&&(TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive==0x01)) {
        memset(tcp_recvbuf, 0, TCP_RX_BUFFSIZE);
        tcp_recvlen = 0;

        if(TCP_Socket[SERV_TCP_WORK].Data_RX.Num_Recived <= TCP_RX_BUFFSIZE) {
            memcpy(tcp_recvbuf, TCP_Socket[SERV_TCP_WORK].Data_RX.Data, TCP_Socket[SERV_TCP_WORK].Data_RX.Num_Recived);
            tcp_recvlen = TCP_Socket[SERV_TCP_WORK].Data_RX.Num_Recived;
        } else {
            memcpy(tcp_recvbuf, TCP_Socket[SERV_TCP_WORK].Data_RX.Data, TCP_RX_BUFFSIZE);
            tcp_recvlen = TCP_RX_BUFFSIZE;
        }


       TCP_SERVER_FLAG |= 1<<6;                //标记接受到数据了
       TCP_SERVER_FLAG |= 0x01;

        TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;//清除接收标志
    }

}

void Socket_Sent(uint16_t len)
{
    if((TCP_SERVER_FLAG & 0x80) && (TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX==0)) {
    // while((TCP_SERVER_FLAG & 0x80) && (TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX==0)) {
        memcpy(TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data, tcp_sentbuf, len);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=len;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;//置发送标志
        TCP_SERVER_FLAG &= ~(1<<7);     //清楚数据发送标志
        TCP_SERVER_FLAG |= 0x01;
        // LED82 = ~LED82;
    }

    // W5500_Service_BSP2();
    sWrite_SOCK_Data_Buffer(TCP_Socket[SERV_TCP_WORK].Index_Socket,TCP_Socket[SERV_TCP_WORK].Data_TX[0].Data,TCP_Socket[SERV_TCP_WORK].Data_TX[0].Num_To_TX);
    TCP_Socket[SERV_TCP_WORK].Data_TX[0].Req_TX = 0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[0].Num_To_TX = 0;

}