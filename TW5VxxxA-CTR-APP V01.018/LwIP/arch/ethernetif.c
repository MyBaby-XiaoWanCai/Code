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
    netif->hwaddr_len = ETHARP_HWADDR_LEN; //����MAC��ַ����,Ϊ6���ֽ�
    //��ʼ��MAC��ַ,����ʲô��ַ���û��Լ�����,���ǲ����������������豸MAC��ַ�ظ�
    netif->hwaddr[0] = ETH_Handler.Init.MACAddr[0];
    netif->hwaddr[1] = ETH_Handler.Init.MACAddr[1];
    netif->hwaddr[2] = ETH_Handler.Init.MACAddr[2];
    netif->hwaddr[3] = ETH_Handler.Init.MACAddr[3];
    netif->hwaddr[4] = ETH_Handler.Init.MACAddr[4];
    netif->hwaddr[5] = ETH_Handler.Init.MACAddr[5];
    //��������䵥Ԫ,����������㲥��ARP����
    netif->mtu   = 1500;
    // netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP;

    #if LWIP_ARP
        netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
    #else
        netif->flags |= NETIF_FLAG_BROADCAST;
    #endif /* LWIP_ARP */

#endif  /* LWIP_ARP || LWIP_ETHERNET */

    HAL_ETH_Start(&ETH_Handler); //����MAC��DMA
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

    //�жϴ˷����������Ƿ���Ч�����жϴ˷����������Ƿ����̫��DMA����
    if((DmaTxDesc->Status&ETH_DMATXDESC_OWN)!=(uint32_t)RESET)
    {
        errval=ERR_USE;
        goto error;             //������������Ч��������
    }

    //��pbuf�п���Ҫ���͵�����
    for(q=p;q!=NULL;q=q->next)
    {
        byteslefttocopy=q->len;     //Ҫ���͵����ݳ���
        payloadoffset=0;

        //��pbuf��Ҫ���͵�����д�뵽��̫�������������У���ʱ������Ҫ���͵����ݿ��ܴ���һ����̫��
        //��������Tx Buffer�����������Ҫ�ֶ�ν����ݿ��������������������
        while((byteslefttocopy+bufferoffset)>ETH_TX_BUF_SIZE )
        {
            //�����ݿ�������̫��������������Tx Buffer��
            memcpy((uint8_t*)((uint8_t*)buffer+bufferoffset),(uint8_t*)((uint8_t*)q->payload+payloadoffset),(ETH_TX_BUF_SIZE-bufferoffset));

            //DmaTxDscָ����һ������������
            DmaTxDesc=(ETH_DMADescTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);
            //����µķ����������Ƿ���Ч
            if((DmaTxDesc->Status&ETH_DMATXDESC_OWN)!=(uint32_t)RESET)
            {
                errval = ERR_USE;
                goto error;     //������������Ч��������
            }

            buffer=(uint8_t *)(DmaTxDesc->Buffer1Addr);   //����buffer��ַ��ָ���µķ�����������Tx Buffer
            byteslefttocopy=byteslefttocopy-(ETH_TX_BUF_SIZE-bufferoffset);
            payloadoffset=payloadoffset+(ETH_TX_BUF_SIZE-bufferoffset);
            framelength=framelength+(ETH_TX_BUF_SIZE-bufferoffset);
            bufferoffset=0;
        }

        //����ʣ�������
        memcpy( (uint8_t*)((uint8_t*)buffer+bufferoffset),(uint8_t*)((uint8_t*)q->payload+payloadoffset),byteslefttocopy );
        bufferoffset=bufferoffset+byteslefttocopy;
        framelength=framelength+byteslefttocopy;
    }

    //������Ҫ���͵����ݶ��Ž�������������Tx Buffer�Ժ�Ϳɷ��ʹ�֡��
    HAL_ETH_TransmitFrame(&ETH_Handler,framelength);
    errval = ERR_OK;

error:
    //���ͻ������������磬һ�����ͻ�������������TxDMA��������״̬
    if((ETH_Handler.Instance->DMASR&ETH_DMASR_TUS)!=(uint32_t)RESET)
    {
        //��������־
        ETH_Handler.Instance->DMASR = ETH_DMASR_TUS;
        //������֡�г�����������ʱ��TxDMA�������ʱ����Ҫ��DMATPDR�Ĵ���
        //���д��һ��ֵ�����份�ѣ��˴�����д0
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

    if(HAL_ETH_GetReceivedFrame(&ETH_Handler)!=HAL_OK)  //�ж��Ƿ���յ�����
        return NULL;

    len=ETH_Handler.RxFrameInfos.length;                //��ȡ���յ�����̫��֡����
    buffer=(uint8_t *)ETH_Handler.RxFrameInfos.buffer;  //��ȡ���յ�����̫��֡������buffer

    if(len>0) p=pbuf_alloc(PBUF_RAW,len,PBUF_POOL);     //����pbuf
    if(p!=NULL)                                        //pbuf����ɹ�
    {
        dmarxdesc=ETH_Handler.RxFrameInfos.FSRxDesc;    //��ȡ���������������еĵ�һ��������
        bufferoffset = 0;
        for(q=p;q!=NULL;q=q->next)
        {
            byteslefttocopy=q->len;
            payloadoffset=0;
            //��������������Rx Buffer�����ݿ�����pbuf��
            while((byteslefttocopy+bufferoffset)>ETH_RX_BUF_SIZE )
            {
                //�����ݿ�����pbuf��
                memcpy((uint8_t*)((uint8_t*)q->payload+payloadoffset),(uint8_t*)((uint8_t*)buffer+bufferoffset),(ETH_RX_BUF_SIZE-bufferoffset));
                 //dmarxdesc����һ������������
                dmarxdesc=(ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
                //����buffer��ַ��ָ���µĽ�����������Rx Buffer
                buffer=(uint8_t *)(dmarxdesc->Buffer1Addr);

                byteslefttocopy=byteslefttocopy-(ETH_RX_BUF_SIZE-bufferoffset);
                payloadoffset=payloadoffset+(ETH_RX_BUF_SIZE-bufferoffset);
                bufferoffset=0;
            }
            //����ʣ�������
            memcpy((uint8_t*)((uint8_t*)q->payload+payloadoffset),(uint8_t*)((uint8_t*)buffer+bufferoffset),byteslefttocopy);
            bufferoffset=bufferoffset+byteslefttocopy;
        }
    }

    //�ͷ�DMA������
    dmarxdesc=ETH_Handler.RxFrameInfos.FSRxDesc;
    for(i=0;i<ETH_Handler.RxFrameInfos.SegCount; i++)
    {
        dmarxdesc->Status|=ETH_DMARXDESC_OWN;       //�����������DMA����
        dmarxdesc=(ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }
    ETH_Handler.RxFrameInfos.SegCount =0;           //����μ�����
    if((ETH_Handler.Instance->DMASR&ETH_DMASR_RBUS)!=(uint32_t)RESET)  //���ջ�����������
    {
        //������ջ����������ñ�־
        ETH_Handler.Instance->DMASR = ETH_DMASR_RBUS;
        //�����ջ����������õ�ʱ��RxDMA���ȥ����״̬��ͨ����DMARPDRд������һ��ֵ������Rx DMA
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

    p=low_level_input(netif);   //����low_level_input������������
    if(p==NULL) return ERR_MEM;

    err=netif->input(p, netif); //����netif�ṹ���е�input�ֶ�(һ������)���������ݰ�
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
    netif->hostname="lwip";     //��ʼ������
#endif

    netif->name[0]    = IFNAME0;             //��ʼ������netif��name�ֶ�
    netif->name[1]    = IFNAME1;             //���ļ��ⶨ�����ﲻ�ù��ľ���ֵ
    netif->output     = etharp_output;       //IP�㷢�����ݰ�����
    netif->linkoutput = low_level_output;    //ARPģ�鷢�����ݰ�����

    low_level_init(netif);                   //�ײ�Ӳ����ʼ������
    return ERR_OK;
}














