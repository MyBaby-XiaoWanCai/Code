/*
*********************************************************************************************************
*
*	ģ������ : Net_Others
*	�ļ����� : Net_Others.h
*	��    �� : V1.0
*	˵    �� :
*	�޸ļ�¼ :
*	�汾��  ����         ����       ˵��
*	V1.0    2020-03-10   ZYL        ��ʽ����
*
*********************************************************************************************************
*/
#include "main.h"
//ȫ�ֱ�������
uint16_t Num_Cabinet=1;
uint16_t Addr_Module=1;

//��������
//���¹�����ʹ�ã��շ�������ע��
//*********************************************************************************
//Description: ���ڹ������Э�����ݴ���
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Socket_Work(void)
{

    if((TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive==0x01)&&(TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX==0))
    {
        uint16_t temp=TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];//�������ݰ���һ���ֽ�
        uint16_t Data_CRC;
        if(temp==0x55)
        {
            //TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout    = 0;//Socket��ʱ����������//sSetCommunicationStatus(cSuccess);
            //TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus     = 1;//�յ�һ����������Ž��г�ʱ���
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0] = 0x55;//�������ݰ���һ���ֽ�
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[1] = 0xAA;
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=2;//�������ֽ�
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;//�÷��ͱ�־
        }
        TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;//������ձ�־
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


       TCP_SERVER_FLAG |= 1<<6;                //��ǽ��ܵ�������
       TCP_SERVER_FLAG |= 0x01;

        TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;//������ձ�־
    }

}

void Socket_Sent(uint16_t len)
{
    if((TCP_SERVER_FLAG & 0x80) && (TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX==0)) {
    // while((TCP_SERVER_FLAG & 0x80) && (TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX==0)) {
        memcpy(TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data, tcp_sentbuf, len);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=len;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;//�÷��ͱ�־
        TCP_SERVER_FLAG &= ~(1<<7);     //������ݷ��ͱ�־
        TCP_SERVER_FLAG |= 0x01;
        // LED82 = ~LED82;
    }

    // W5500_Service_BSP2();
    sWrite_SOCK_Data_Buffer(TCP_Socket[SERV_TCP_WORK].Index_Socket,TCP_Socket[SERV_TCP_WORK].Data_TX[0].Data,TCP_Socket[SERV_TCP_WORK].Data_TX[0].Num_To_TX);
    TCP_Socket[SERV_TCP_WORK].Data_TX[0].Req_TX = 0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[0].Num_To_TX = 0;

}