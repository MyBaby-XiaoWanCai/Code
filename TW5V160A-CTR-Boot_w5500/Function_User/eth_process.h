#ifndef __ETH_PROCESS_H__
#define __ETH_PROCESS_H__

#include "sys.h"
#include "eth_ksz8041.h"
#include "err.h"

extern u8 TCP_SERVER_FLAG;

extern struct netif default_netif;
extern uint8_t IP_ADDRESS[4];
extern uint8_t NETMASK_ADDRESS[4];
extern uint8_t GATEWAY_ADDRESS[4];

//extern struct tcp_pcb* server_pcb;
// extern struct tcp_pcb* connect_pcb;

#define TCP_RX_BUFFSIZE 2000
#define TCP_TX_BUFFSIZE 2000
#define TCP_SERVER_PORT 5000    //定义 tcp client 要连接的远程端口

extern u8 tcp_recvbuf[TCP_RX_BUFFSIZE];
extern u8 tcp_sentbuf[TCP_TX_BUFFSIZE];

// //tcp 客户端连接状态
// enum tcp_client_states {
//     ES_TCPCLIENT_NONE = 0,      //没有连接
//     ES_TCPCLIENT_CONNECTED,     //连接到服务器了
//     ES_TCPCLIENT_CLOSING,       //关闭连接
// };

// //LwIP TCP 客户端回调函数使用的结构体
// struct tcp_client_struct {
//     u8 state;               //当前连接状态
//     struct tcp_pcb *pcb;    //指向当前的pcb
//     struct pbuf *p;         //指向 接受/发送 的 pbuf
// };

//tcp服务器连接状态
enum tcp_server_states {
    ES_TCPSERVER_NONE = 0,      //没有连接
    ES_TCPSERVER_ACCEPTED,      //有客户端连接上了
    ES_TCPSERVER_RECEIVED,      //接收到数据了
    ES_TCPSERVER_CLOSING,       //即将关闭连接
};
//LWIP回调函数使用的结构体
struct tcp_server_struct {
    u8 state;                   //当前连接状
    u8 retries;
    struct tcp_pcb *pcb;        //指向当前的pcb
    /* pbuf (chain) to recycle */
    struct pbuf *p;             //指向接收/或传输的pbuf
};


u8 LwIP_Init(void);
// err_t TCP_UserSent(struct tcp_pcb* tpcb, u16_t len);
uint16_t TCP_UserSent(uint8_t* pData, uint16_t len);
void TCP_Connection_Close(struct tcp_pcb* tpcb, struct tcp_server_struct* es);

#endif
