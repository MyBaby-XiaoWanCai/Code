#include "netif/etharp.h"
#include "string.h"
#include "ethernetif.h"
#include "eth_ksz8041.h"
#include "opt.h"

// struct ethernetif {
//     struct eth_addr *ethaddr;
//     /* Add whatever per-interface state that is needed here. */
// };


/***************************************************************************************************
** In this function, the hardware should be initialized.
** Called from ethernetif_init().
**
** @param netif the already initialized lwip network interface structure
**        for this ethernetif
***************************************************************************************************/
static void low_level_init(struct netif *netif)
{
    HAL_StatusTypeDef ksz8041_init_status;

    ksz8041_init_status = ETH_KSZ8041_Init();
    if (ksz8041_init_status == HAL_OK)
    {
        /* Set netif link flag */
        netif->flags |= NETIF_FLAG_LINK_UP;
    }

#if LWIP_ARP || LWIP_ETHERNET
    netif->hwaddr_len = ETHARP_HWADDR_LEN; //设置MAC地址长度,为6个字节
    //初始化MAC地址,设置什么地址由用户自己设置,但是不能与网络中其他设备MAC地址重复
    netif->hwaddr[0] = ETH_Handler.Init.MACAddr[0];
    netif->hwaddr[1] = ETH_Handler.Init.MACAddr[1];
    netif->hwaddr[2] = ETH_Handler.Init.MACAddr[2];
    netif->hwaddr[3] = ETH_Handler.Init.MACAddr[3];
    netif->hwaddr[4] = ETH_Handler.Init.MACAddr[4];
    netif->hwaddr[5] = ETH_Handler.Init.MACAddr[5];
    //最大允许传输单元,允许该网卡广播和ARP功能
    netif->mtu   = 1500;
    // netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP;

    #if LWIP_ARP
        netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
    #else
        netif->flags |= NETIF_FLAG_BROADCAST;
    #endif /* LWIP_ARP */

#endif  /* LWIP_ARP || LWIP_ETHERNET */

    HAL_ETH_Start(&ETH_Handler); //开启MAC和DMA
}


/***************************************************************************************************
** This function should do the actual transmission of the packet. The packet is
** contained in the pbuf that is passed to the function. This pbuf
** might be chained.
**
** @param netif the lwip network interface structure for this ethernetif
** @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
** @return ERR_OK if the packet could be sent
**         an err_t value if the packet couldn't be sent
**
** @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
**       strange results. You might consider waiting for space in the DMA queue
**       to become availale since the stack doesn't retry to send a packet
**       dropped because of memory failure (except for the TCP timers).
***************************************************************************************************/
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    err_t errval;
    struct pbuf *q;

    uint32_t framelength = 0;
    uint32_t bufferoffset = 0;
    uint32_t byteslefttocopy = 0;
    uint32_t payloadoffset = 0;

    __IO ETH_DMADescTypeDef *DmaTxDesc;
    uint8_t *buffer=(uint8_t *)(ETH_Handler.TxDesc->Buffer1Addr);

    DmaTxDesc = ETH_Handler.TxDesc;
    bufferoffset = 0;

    //判断此发送描述符是否有效，即判断此发送描述符是否归以太网DMA所有
    if((DmaTxDesc->Status&ETH_DMATXDESC_OWN)!=(uint32_t)RESET)
    {
        errval=ERR_USE;
        goto error;             //发送描述符无效，不可用
    }

    //从pbuf中拷贝要发送的数据
    for(q=p;q!=NULL;q=q->next)
    {
        byteslefttocopy=q->len;     //要发送的数据长度
        payloadoffset=0;

        //将pbuf中要发送的数据写入到以太网发送描述符中，有时候我们要发送的数据可能大于一个以太网
        //描述符的Tx Buffer，因此我们需要分多次将数据拷贝到多个发送描述符中
        while((byteslefttocopy+bufferoffset)>ETH_TX_BUF_SIZE )
        {
            //将数据拷贝到以太网发送描述符的Tx Buffer中
            memcpy((uint8_t*)((uint8_t*)buffer+bufferoffset),(uint8_t*)((uint8_t*)q->payload+payloadoffset),(ETH_TX_BUF_SIZE-bufferoffset));

            //DmaTxDsc指向下一个发送描述符
            DmaTxDesc=(ETH_DMADescTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);
            //检查新的发送描述符是否有效
            if((DmaTxDesc->Status&ETH_DMATXDESC_OWN)!=(uint32_t)RESET)
            {
                errval = ERR_USE;
                goto error;     //发送描述符无效，不可用
            }

            buffer=(uint8_t *)(DmaTxDesc->Buffer1Addr);   //更新buffer地址，指向新的发送描述符的Tx Buffer
            byteslefttocopy=byteslefttocopy-(ETH_TX_BUF_SIZE-bufferoffset);
            payloadoffset=payloadoffset+(ETH_TX_BUF_SIZE-bufferoffset);
            framelength=framelength+(ETH_TX_BUF_SIZE-bufferoffset);
            bufferoffset=0;
        }

        //拷贝剩余的数据
        memcpy( (uint8_t*)((uint8_t*)buffer+bufferoffset),(uint8_t*)((uint8_t*)q->payload+payloadoffset),byteslefttocopy );
        bufferoffset=bufferoffset+byteslefttocopy;
        framelength=framelength+byteslefttocopy;
    }

    //当所有要发送的数据都放进发送描述符的Tx Buffer以后就可发送此帧了
    HAL_ETH_TransmitFrame(&ETH_Handler,framelength);
    errval = ERR_OK;

error:
    //发送缓冲区发生下溢，一旦发送缓冲区发生下溢TxDMA会进入挂起状态
    if((ETH_Handler.Instance->DMASR&ETH_DMASR_TUS)!=(uint32_t)RESET)
    {
        //清除下溢标志
        ETH_Handler.Instance->DMASR = ETH_DMASR_TUS;
        //当发送帧中出现下溢错误的时候TxDMA会挂起，这时候需要向DMATPDR寄存器
        //随便写入一个值来将其唤醒，此处我们写0
        ETH_Handler.Instance->DMATPDR=0;
    }

    return errval;
}


/***************************************************************************************************
** Should allocate a pbuf and transfer the bytes of the incoming
** packet from the interface into the pbuf.
**
** @param netif the lwip network interface structure for this ethernetif
** @return a pbuf filled with the received packet (including MAC header)
**         NULL on memory error
***************************************************************************************************/
static struct pbuf * low_level_input(struct netif *netif)
{
    struct pbuf *p = NULL;
    struct pbuf *q = NULL;
    uint16_t len;
    uint8_t *buffer;
    __IO ETH_DMADescTypeDef *dmarxdesc;
    uint32_t bufferoffset=0;
    uint32_t payloadoffset=0;
    uint32_t byteslefttocopy=0;
    uint32_t i=0;

    if(HAL_ETH_GetReceivedFrame(&ETH_Handler)!=HAL_OK)  //判断是否接收到数据
        return NULL;

    len=ETH_Handler.RxFrameInfos.length;                //获取接收到的以太网帧长度
    buffer=(uint8_t *)ETH_Handler.RxFrameInfos.buffer;  //获取接收到的以太网帧的数据buffer

    if(len>0) p=pbuf_alloc(PBUF_RAW,len,PBUF_POOL);     //申请pbuf
    if(p!=NULL)                                        //pbuf申请成功
    {
        dmarxdesc=ETH_Handler.RxFrameInfos.FSRxDesc;    //获取接收描述符链表中的第一个描述符
        bufferoffset = 0;
        for(q=p;q!=NULL;q=q->next)
        {
            byteslefttocopy=q->len;
            payloadoffset=0;
            //将接收描述符中Rx Buffer的数据拷贝到pbuf中
            while((byteslefttocopy+bufferoffset)>ETH_RX_BUF_SIZE )
            {
                //将数据拷贝到pbuf中
                memcpy((uint8_t*)((uint8_t*)q->payload+payloadoffset),(uint8_t*)((uint8_t*)buffer+bufferoffset),(ETH_RX_BUF_SIZE-bufferoffset));
                 //dmarxdesc向下一个接收描述符
                dmarxdesc=(ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
                //更新buffer地址，指向新的接收描述符的Rx Buffer
                buffer=(uint8_t *)(dmarxdesc->Buffer1Addr);

                byteslefttocopy=byteslefttocopy-(ETH_RX_BUF_SIZE-bufferoffset);
                payloadoffset=payloadoffset+(ETH_RX_BUF_SIZE-bufferoffset);
                bufferoffset=0;
            }
            //拷贝剩余的数据
            memcpy((uint8_t*)((uint8_t*)q->payload+payloadoffset),(uint8_t*)((uint8_t*)buffer+bufferoffset),byteslefttocopy);
            bufferoffset=bufferoffset+byteslefttocopy;
        }
    }

    //释放DMA描述符
    dmarxdesc=ETH_Handler.RxFrameInfos.FSRxDesc;
    for(i=0;i<ETH_Handler.RxFrameInfos.SegCount; i++)
    {
        dmarxdesc->Status|=ETH_DMARXDESC_OWN;       //标记描述符归DMA所有
        dmarxdesc=(ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }
    ETH_Handler.RxFrameInfos.SegCount =0;           //清除段计数器
    if((ETH_Handler.Instance->DMASR&ETH_DMASR_RBUS)!=(uint32_t)RESET)  //接收缓冲区不可用
    {
        //清除接收缓冲区不可用标志
        ETH_Handler.Instance->DMASR = ETH_DMASR_RBUS;
        //当接收缓冲区不可用的时候RxDMA会进去挂起状态，通过向DMARPDR写入任意一个值来唤醒Rx DMA
        ETH_Handler.Instance->DMARPDR=0;
    }
    return p;
}


/***************************************************************************************************
** This function should be called when a packet is ready to be read
** from the interface. It uses the function low_level_input() that
** should handle the actual reception of bytes from the network
** interface. Then the type of the received packet is determined and
** the appropriate input function is called.
**
** @param netif the lwip network interface structure for this ethernetif
***************************************************************************************************/
err_t ethernetif_input(struct netif *netif)
{
    err_t err;
    struct pbuf *p;

    p=low_level_input(netif);   //调用low_level_input函数接收数据
    if(p==NULL) return ERR_MEM;

    err=netif->input(p, netif); //调用netif结构体中的input字段(一个函数)来处理数据包
    if(err!=ERR_OK)
    {
        LWIP_DEBUGF(NETIF_DEBUG,("ethernetif_input: IP input error\n"));
        pbuf_free(p);
        p = NULL;
    }

    return err;
}


/***************************************************************************************************
** Should be called at the beginning of the program to set up the
** network interface. It calls the function low_level_init() to do the
** actual setup of the hardware.
**
** This function should be passed as a parameter to netif_add().
**
** @param netif the lwip network interface structure for this ethernetif
** @return ERR_OK if the loopif is initialized
**         ERR_MEM if private data couldn't be allocated
**         any other err_t on error
***************************************************************************************************/
err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif!=NULL",(netif!=NULL));

#if LWIP_NETIF_HOSTNAME         //LWIP_NETIF_HOSTNAME
    netif->hostname="lwip";     //初始化名称
#endif

    netif->name[0]    = IFNAME0;             //初始化变量netif的name字段
    netif->name[1]    = IFNAME1;             //在文件外定义这里不用关心具体值
    netif->output     = etharp_output;       //IP层发送数据包函数
    netif->linkoutput = low_level_output;    //ARP模块发送数据包函数

    low_level_init(netif);                   //底层硬件初始化函数
    return ERR_OK;
}














