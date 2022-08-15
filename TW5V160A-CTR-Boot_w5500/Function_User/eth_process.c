#include "lwip/init.h"
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "lwip/tcpip.h"
#include "lwip/ip_frag.h"
#include "lwip/ip.h"
#include "lwip/netif.h"
#include "lwip/snmp.h"
#include "lwip/def.h"
#include "lwip/stats.h"
#include "lwip/arch.h"
#include "netif/etharp.h"
#include "eth_process.h"
#include "ethernetif.h"
//#include "delay.h"
#include <string.h>
#include "main.h"

struct netif default_netif;
uint8_t IP_ADDRESS[4] = {192, 168, 1, 11};
uint8_t NETMASK_ADDRESS[4] = {255, 255, 0, 0};
uint8_t GATEWAY_ADDRESS[4] = {192, 168, 1, 1};
uint8_t REMOTE_IP[4];

struct tcp_pcb* server_pcb = NULL;
// struct tcp_pcb* connect_pcb;
struct tcp_server_struct* server_es = NULL;


//TCP Server 测试全局状态标记变量
//bit7:0,没有数据要发送;1,有数据要发送
//bit6:0,没有收到数据;1,收到数据了.
//bit5:0,没有客户端连接上;1,有客户端连接上了.
//bit4~0:保留
u8 TCP_SERVER_FLAG;

u8 tcp_recvbuf[TCP_RX_BUFFSIZE];        //TCP client 接受数据缓冲区
u8 tcp_sentbuf[TCP_TX_BUFFSIZE];        //TCP_client 发送数据缓冲区

static err_t TCP_Recv_Callback(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err);
static void TCP_Err_Callback(void* arg, err_t err);
static err_t TCP_Poll_Callback(void* arg, struct tcp_pcb* tpcb);
static void TCP_SendData(struct tcp_pcb* tpcb, struct tcp_server_struct* es);
static err_t TCP_Sent_Callbackk(void* arg, struct tcp_pcb* tpcb, u16_t len);
static err_t TCP_Accept_Callback(void* arg, struct tcp_pcb* newpcb, err_t err);

u8 LwIP_Init(void)
{
    struct netif *Netif_Init_Flag;  //调用netif_add()函数时的返回值,用于判断网络初始化是否成功
    struct ip_addr ipaddr;          //ip地址
    struct ip_addr netmask;         //子网掩码
    struct ip_addr gw;              //默认网关

    IP4_ADDR(&ipaddr, IP_ADDRESS[0], IP_ADDRESS[1], IP_ADDRESS[2], IP_ADDRESS[3]);
    IP4_ADDR(&netmask, NETMASK_ADDRESS[0], NETMASK_ADDRESS[1], NETMASK_ADDRESS[2], NETMASK_ADDRESS[3]);
    IP4_ADDR(&gw, GATEWAY_ADDRESS[0], GATEWAY_ADDRESS[1], GATEWAY_ADDRESS[2], GATEWAY_ADDRESS[3]);

    lwip_init();

    Netif_Init_Flag = netif_add(&default_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&ethernet_input);//向网卡列表中添加一个网口

    if(Netif_Init_Flag==NULL) { //网卡添加失败
        return 4;
    } else { //网口添加成功后,设置netif为默认值,并且打开netif网口
        netif_set_default(&default_netif);  //设置netif为默认网口

        if(netif_is_link_up(&default_netif)) {
            netif_set_up(&default_netif);       //打开netif网口
        } else {
            netif_set_down(&default_netif);
        }

    }
    return 0;//操作OK
}


/***************************************************************************************************
**  LwIP tcp_recv() 调用的回调函数
**
**
***************************************************************************************************/
static err_t TCP_Recv_Callback(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err)
{
    u32 data_len = 0;
    err_t ret_err;
    struct pbuf* q;
    struct tcp_server_struct* es;

    LWIP_ASSERT("arg != NULL", arg != NULL);

    es = (struct tcp_server_struct*)arg;

    if(p == NULL) {                                     //如果从客户端接受到空的数据帧就关闭连接
        es->state = ES_TCPSERVER_CLOSING;       //需要关闭 TCP 连接
        if(es->p == NULL) {
            /* we're done sending, close it */
            TCP_Connection_Close(tpcb, es);

            LED83 = 1;
        } else {
            /* we're not done yet */
            tcp_sent(tpcb, TCP_Sent_Callbackk);
            TCP_SendData(tpcb, es);
        }
        // es->p = p;
        ret_err = ERR_OK;
    } else if(err != ERR_OK) {                          //当收到一个非空帧，但 err != ERR_OK 则释放掉数据
        if(p != NULL) {
            es->p = NULL;
            pbuf_free(p);                       //释放掉 pbuf
        }
        ret_err = err;
    } else if(es->state == ES_TCPSERVER_ACCEPTED) {    //当处于连接且收到的数据不为空时则拷贝数据到缓存区
        if(!(TCP_SERVER_FLAG & 0x40)) {         //当前数据已处理完毕，可以接受新数据
            memset(tcp_recvbuf, 0, TCP_RX_BUFFSIZE);

            /* first data chunk in p->payload */
            // es->state = ES_TCPSERVER_RECEIVED;

            for(q=p; q!=NULL; q=q->next) {          //遍历整个 pbuf 链表
                if(q->len > (TCP_RX_BUFFSIZE-data_len)) {
                    memcpy(tcp_recvbuf+data_len, q->payload, (TCP_RX_BUFFSIZE-data_len));
                } else {
                    memcpy(tcp_recvbuf+data_len, q->payload, q->len);
                }
                data_len += q->len;
                if(data_len > TCP_RX_BUFFSIZE) {    //超出 TCP 接受数组范围则跳出
                    break;
                }
            }

            TCP_SERVER_FLAG |= 1<<6;                //标记接受到数据了

            // //记录远程的 IP
            // REMOTE_IP[0] = tpcb->remote_ip.addr & 0xff;
            // REMOTE_IP[1] = (tpcb->remote_ip.addr >> 8) & 0xff;
            // REMOTE_IP[2] = (tpcb->remote_ip.addr >> 16) & 0xff;
            // REMOTE_IP[3] = (tpcb->remote_ip.addr >> 24) & 0xff;

            tcp_recved(tpcb, p->tot_len);           //通知 LwIP 接收到了数据，更新接受窗口
            pbuf_free(p);
            ret_err = ERR_OK;
        }
    }
    // else if(es->state == ES_TCPSERVER_CLOSING) {
    //     tcp_recved(tpcb, p->tot_len);
    //     es->p = NULL;
    //     pbuf_free(p);
    //     ret_err = ERR_OK;
    // }
    else {                                            //接收到数据但是连接已经关闭
        tcp_recved(tpcb, p->tot_len);
        es->p = NULL;
        pbuf_free(p);
        ret_err = ERR_OK;
    }

    return ret_err;
}


/***************************************************************************************************
**  LwIP tcp_err() 调用的回调函数
**
**
***************************************************************************************************/
static void TCP_Err_Callback(void* arg, err_t err)
{
    struct tcp_server_struct* es;

    LWIP_UNUSED_ARG(err);

    // printf("tcp error:%x\r\n",(u32)arg);
    es = (struct tcp_server_struct*)arg;
    if(es != NULL) {
        mem_free(es);//释放内存
    }
}


/***************************************************************************************************
**  LwIP tcp_poll() 调用的回调函数
**
**
***************************************************************************************************/
static err_t TCP_Poll_Callback(void* arg, struct tcp_pcb* tpcb)
{
    err_t ret_err;
    struct tcp_server_struct* es;
    es = (struct tcp_server_struct*)arg;

    if(es != NULL) {
        if(es->state == ES_TCPSERVER_CLOSING) {
            TCP_Connection_Close(tpcb, es);

            LED83 = 1;
        }

        ret_err = ERR_OK;
    } else {
        /* nothing to be done */
        tcp_abort(tpcb);
        ret_err = ERR_ABRT;
    }

    return ret_err;
}


/***************************************************************************************************
**  最终用来发送数据的函数
**
**
***************************************************************************************************/
static void TCP_SendData(struct tcp_pcb* tpcb, struct tcp_server_struct* es)
{
    struct pbuf* ptr;
    err_t wr_err = ERR_OK;

    while((wr_err == ERR_OK) && (es->p != NULL) && (es->p->len <= tcp_sndbuf(tpcb)))
    {
        ptr = es->p;
        wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
        if(wr_err == ERR_OK) {
            uint16_t plen;
            uint8_t freed;

            plen = ptr->len;
            /* continue with next pbuf in chain (if any) */
            es->p = ptr->next;
            if(es->p != NULL) {
                /* new reference! */
                pbuf_ref(es->p);
            }
            /* chop first pbuf from chain */
            do {
                /* try hard to free pbuf */
                freed = pbuf_free(ptr);
            } while(freed == 0);

            /* we can read more data now */
            // tcp_recved(tpcb, plen);
            tcp_output(tpcb);
        } else if(wr_err == ERR_MEM) {
            es->p = ptr;
            tcp_output(tpcb);
        } else {
            /* other problem ?? */
        }
        // tcp_output(tpcb);
    }
}


/***************************************************************************************************
**  LwIP tcp_sent() 调用的回调函数
**
**
***************************************************************************************************/
static err_t TCP_Sent_Callbackk(void* arg, struct tcp_pcb* tpcb, u16_t len)
{
    struct tcp_server_struct* es;
    LWIP_UNUSED_ARG(len);

    es = (struct tcp_server_struct*)arg;
    es->retries = 0;
    if(es->p != NULL) {
        tcp_sent(tpcb, TCP_Sent_Callbackk);
        TCP_SendData(tpcb, es);
    } else {
        /* no more pbufs to send */
        if(es->state == ES_TCPSERVER_CLOSING)
        {
            TCP_Connection_Close(tpcb, es);

            LED83 = 1;
        }
    }

    // tcp_write(tpcb, tcp_sentbuf, strlen(tcp_sentbuf), 1);

    return ERR_OK;
}


/***************************************************************************************************
**  用户调用，用来发送数据
**
**
***************************************************************************************************/
// err_t TCP_UserSent(struct tcp_pcb* tpcb, u16_t len)
uint16_t TCP_UserSent(uint8_t* pData, uint16_t len)
{
    err_t ret_err = ERR_OK;
    // struct tcp_server_struct* es;
    // es = tpcb->callback_arg;

    struct tcp_pcb* pcb = NULL;
    if(server_es) {
        pcb = server_es->pcb;
    }

    // if((es != NULL) && (TCP_SERVER_FLAG & 0x80)) {
    if(pcb && (TCP_SERVER_FLAG & 0x80)){
    // if(TCP_SERVER_FLAG & 0x80) {
        // es->p = pbuf_alloc(PBUF_TRANSPORT, strlen((char*)tcp_sentbuf), PBUF_POOL);
        // pbuf_take(es->p, (char*)tcp_sentbuf, strlen((char*)tcp_sentbuf));
        // es->p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_POOL);
        // pbuf_take(es->p, (char*)tcp_sentbuf, len);

        ret_err = tcp_write(pcb, pData, len, 0);
        tcp_output(pcb);
        if(ret_err == ERR_OK) {

        } else if(ret_err == ERR_MEM) {
            tcp_output(pcb);
        } else {

        }
        // tcp_output(pcb);

        // tcp_sent(tpcb, TCP_Sent_Callbackk);
        // TCP_SendData(tpcb, es);

        // if(es->p) {
        //     pbuf_free(es->p);
        // }

        TCP_SERVER_FLAG &= ~(1<<7);     //清楚数据发送标志

        // ret_err = ERR_OK;

        return len;
    }
    // else {
    //     tcp_abort(tpcb);                //终止连接，删除 pcb 控制块
    //     ret_err = ERR_ABRT;
    // }

    // return ret_err;
    return 0;
}


// /***************************************************************************************************
// **  LwIP TCP 连接建立后调用的回调函数
// **
// **
// ***************************************************************************************************/
// static err_t TCP_Connected_Callback(void* arg, struct tcp_pcb* tpcb, err_t err)
// {
//     struct tcp_client_struct *es = NULL;

//     if(err == ERR_OK) {
//         es = (struct tcp_client_struct*)mem_malloc(sizeof(struct tcp_client_struct));
//         if(es) {
//             es->state = ES_TCPCLIENT_CONNECTED;    //修改状态为连接成功
//             es->pcb   = tpcb;
//             es->p     = NULL;
//             tcp_arg(tpcb, es);                      //使用 es 更新 tpcb 的 callback_arg
//             tcp_recv(tpcb, TCP_Recv_Callback);      //初始化 LwIP 的 tcp_recv 回调函数
//             tcp_err(tpcb, TCP_Err_Callback);        //初始化 tcp_err 的回调函数
//             tcp_sent(tpcb, TCP_Sent_Callback);      //初始化 tcp_sent 的回调函数
//             tcp_poll(tpcb, TCP_Poll_Callback);      //初始化 tcp_poll 的回调函数
//             TCP_Flag |= 1<<5;                       //标记连接到服务器
//             err = ERR_OK;
//         } else {
//             TCP_Connection_Close(tpcb, es);     //关闭连接
//             err = ERR_MEM;                      //返回内存分配错误
//         }
//     } else {
//         TCP_Connection_Close(tpcb, 0);          //关闭连接
//     }
//     return err;
// }


/***************************************************************************************************
**  LwIP tcp_accept() 调用的回调函数
**
**
***************************************************************************************************/
static err_t TCP_Accept_Callback(void* arg, struct tcp_pcb* newpcb, err_t err)
{
    err_t ret_err;
    struct tcp_server_struct* es;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    tcp_setprio(newpcb, TCP_PRIO_MIN);        //设置新创建的 pcb 的优先级

    es = (struct tcp_server_struct*)mem_malloc(sizeof(struct tcp_server_struct));
    server_es = es;

    if(es != NULL) {        //内存分配成功
        es->state = ES_TCPSERVER_ACCEPTED;
        es->pcb = newpcb;
        es->retries = 0;
        es->p = NULL;

        tcp_arg(newpcb, es);                        //使用 es 更新 tpcb 的 callback_arg
        tcp_recv(newpcb, TCP_Recv_Callback);        //初始化 LwIP 的 tcp_recv() 回调函数
        tcp_err(newpcb, TCP_Err_Callback);          //初始化 tcp_err() 的回调函数
        tcp_poll(newpcb, TCP_Poll_Callback, 0);     //初始化 tcp_poll() 的回调函数
        tcp_sent(newpcb, TCP_Sent_Callbackk);       //初始化 tcp_sent() 的回调函数

        //记录远程的 IP
        REMOTE_IP[0] = newpcb->remote_ip.addr & 0xff;
        REMOTE_IP[1] = (newpcb->remote_ip.addr >> 8) & 0xff;
        REMOTE_IP[2] = (newpcb->remote_ip.addr >> 16) & 0xff;
        REMOTE_IP[3] = (newpcb->remote_ip.addr >> 24) & 0xff;

        TCP_SERVER_FLAG |= 1<<5;       //标记有客户端连上了

        ret_err = ERR_OK;

        LED83 = 0;
    } else {
        TCP_Connection_Close(newpcb, es);
        ret_err = ERR_MEM;
    }

    return ret_err;

}


/***************************************************************************************************
**  LwIP TCP 初始化函数
**
**
***************************************************************************************************/
void TCP_Server_Init(void)
{
    err_t err;
    // struct tcp_pcb* server_pcb;
    // struct tcp_pcb* connect_pcb;

    server_pcb = tcp_new();                                         //创建一个新的 tcp pcb
    if(server_pcb) {            //创建成功
        err = tcp_bind(server_pcb, IP_ADDR_ANY, TCP_SERVER_PORT);    //绑定本地的 IP 地址和端口
        if(err == ERR_OK) {     //绑定成功
            // connect_pcb = tcp_listen(server_pcb);                   //进入监听状态
            // tcp_accept(connect_pcb, TCP_Accept_Callback);           //处理连接

            server_pcb = tcp_listen(server_pcb);                   //进入监听状态
            tcp_accept(server_pcb, TCP_Accept_Callback);           //处理连接
        }
    }
}


/***************************************************************************************************
**  LwIP TCP 关闭与服务器的连接
**
**
***************************************************************************************************/
void TCP_Connection_Close(struct tcp_pcb* tpcb, struct tcp_server_struct* es)
{
    struct tcp_pcb* active_pcb = NULL;
    if(server_es) {
        active_pcb = server_es->pcb;
    }
    if(active_pcb) {
        //移除回调函数
        tcp_arg(active_pcb, NULL);
        tcp_recv(active_pcb, NULL);
        tcp_err(active_pcb, NULL);
        tcp_poll(active_pcb, NULL, 0);
        tcp_sent(active_pcb, NULL);
        tcp_close(active_pcb);        //终止连接，删除 pcb 控制块
    }
    // //移除回调函数
    // tcp_close(tpcb);        //终止连接，删除 pcb 控制块
    // tcp_arg(tpcb, NULL);
    // tcp_recv(tpcb, NULL);
    // tcp_err(tpcb, NULL);
    // tcp_poll(tpcb, NULL, 0);
    // tcp_sent(tpcb, NULL);

    if(es != NULL) {
        mem_free(es);
    }
    tcp_close(tpcb);

    TCP_SERVER_FLAG &= ~(1<<5);    //标记连接断开了
}
