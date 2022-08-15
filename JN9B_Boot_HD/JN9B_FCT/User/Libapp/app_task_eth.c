/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_task_eth.c
** Author      : Cai Heng
** Date        : 2021-06-17
** Description : Ethnet application task file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
Net_Mod Net_Mod_State={0};  //����ģʽ���ƽṹ��

#if 0   //��δ�õ� 
/* TCPnet API�ķ���ֵ */
static const char *ReVal_Table[] =
{
    "netOK: Operation succeeded",
    "netBusy: Process is busy",
    "netError: Unspecified error",
    "netInvalidParameter: Invalid parameter specified",
    "netWrongState: Wrong state error",
    "netDriverError: Driver error",
    "netServerError: Server error",
    "netAuthenticationFailed: User authentication failed",
    "netDnsResolverError: DNS host resolver failed",
    "netFileError: File not found or file r/w error",
    "netTimeout: Operation timeout",
};
#endif

uint8_t client_conFlg = 0; //�ͻ������ӱ�־
/* ������ */
osThreadId_t ThreadIdTaskMsgPro = NULL;

/* ������������ */
const osThreadAttr_t ThreadMsgPro_Attr =
{
    .name = "osRtxMsgProThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal5,
    .stack_size = 2048,
};

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void AppTaskEth(void *argument);
#ifdef PHY_KSZ8081_USED
void AppTaskMsgPro(void *argument);
#endif
extern void Lan_cfg(void);
extern void w5500_init(void);
extern void W5500_TCP_Client(void);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : AppTaskEth()
**	����˵�� : ���߲��״̬�������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void AppTaskEth(void *argument)
{
    while(1)
    {
		if(!Net_Mod_State.Mode)
		{
			W5500_Poll();   //������ģʽ
//			W5500_TcpServer();
		}
		else
		{
			if(heart_beat_flag==2)
			{
				bsp_InitW5500();
				heart_beat_flag=0;  
			}			
			W5500_TCP_Client();//�ͻ���ģʽ
//			W5500_TcpClient();
		}
		if((Net_Mod_State.NetMod_cnt > 60*1000) && (client_conFlg == 0)) //������60�������������л����ͻ���ģʽ
		{
			Net_Mod_State.NetMod_cnt=0;        //��������
			Net_Mod_State.NetMod_Disable=1;    //ֹͣģʽ�л����ؼ���
			Net_Mod_State.Mode = 1;            //�޸�ģʽλ=�ͻ���
			W5500_close(SOCK_0_NUMB);          //�رշ�����socket0;
			
			bsp_InitW5500();                   //���³�ʼ��
		}
		
        osDelay(ETH_TASK_PERIOD);  
    }
}

#ifdef PHY_KSZ8081_USED
/**********************************************************************************************************
**	�� �� �� : tcp_cb_server()
**	����˵�� : TCP Socket�ص�����
**	��    �� : socket - ���
**             event  - �¼�����
**             *addr  - NET_ADDR���ͱ���(��¼IP��ַ���˿ں�)
**             *buf   - ���ջ������׵�ַ
**             len    - ���յ������ݸ���
**	�� �� ֵ : 0 - ��ֹ����
**             1 - ��������
**********************************************************************************************************/
uint32_t tcp_cb_server(int32_t socket, netTCP_Event event,
                       const NET_ADDR *addr, const uint8_t *buf, uint32_t len)
{
    switch (event)
    {
    case netTCP_EventConnect:
        if (addr->addr_type == NET_ADDR_IP4)
        {
#if (TASK_ETH_PRINTF_CFG == ENABLE_CFG)
            APP_Printf("Զ�̿ͻ�����������IP: %d.%d.%d.%d  �˿ں�:%d\r\n",
                       addr->addr[0],
                       addr->addr[1],
                       addr->addr[2],
                       addr->addr[3],
                       addr->port);
#endif
            return (1);
        }
        else if (addr->addr_type == NET_ADDR_IP6)
        {
            return (1);
        }

        return(0);

        /* SocketԶ�������Ѿ����� */
    case netTCP_EventEstablished:
#if (TASK_ETH_PRINTF_CFG == ENABLE_CFG)
        APP_Printf("Socket is connected to remote peer\r\n");
#endif
        break;

        /* ���ӶϿ� */
    case netTCP_EventClosed:
#if (TASK_ETH_PRINTF_CFG == ENABLE_CFG)
        APP_Printf("Connection has been closed\r\n");
#endif
        break;

        /* ������ֹ */
    case netTCP_EventAborted:
        break;

        /* ���͵������յ�Զ���豸Ӧ�� */
    case netTCP_EventACK:
        break;

        /* ���յ�TCP����֡��ptrָ�����ݵ�ַ��par��¼���ݳ��ȣ���λ�ֽ� */
    case netTCP_EventData:
#if (TASK_ETH_PRINTF_CFG == ENABLE_CFG)
        APP_Printf("Data length = %d\r\n", len);
        APP_Printf("%.*s\r\n",len, buf);
#endif
        break;
    }
    return (0);
}

/**********************************************************************************************************
**	�� �� �� : AppTaskMsgPro()
**	����˵�� : TCPnetӦ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void AppTaskMsgPro(void *argument)
{
    uint8_t *sendbuf;
    int32_t iCount;
    int32_t tcp_sock;
    uint32_t i;
    uint32_t maxlen;
    uint32_t EvtFlag;
    netStatus res;

    const uint16_t usMaxBlockTime = MSG_PRO_TASK_PERIOD;

    tcp_sock = netTCP_GetSocket(tcp_cb_server);

    if(tcp_sock > 0)
    {
        res = netTCP_Listen(tcp_sock, NET_PORT_NUM);
#if (TASK_ETH_PRINTF_CFG == ENABLE_CFG)
        APP_Printf("tcp listen res = %s\r\n", ReVal_Table[res]);
#endif
        /* ʹ��TCP_TYPE_KEEP_ALIVE����һֱ�������� */
        netTCP_SetOption(tcp_sock, netTCP_OptionKeepAlive, 1);
    }

    while(1)
    {
        /* �ȴ���Ϣ */
        EvtFlag = osThreadFlagsWait(0x00000007U, osFlagsWaitAny, usMaxBlockTime);

        /* ��Ϣ�Ĵ��� */
        if(netTCP_GetState(tcp_sock) == netTCP_StateESTABLISHED)
        {
            switch(EvtFlag)
            {
                /* ��Զ��TCP�ͻ��˷��Ͷ�ȡ��FLASH���� */
            case FLASH_MSG:
                iCount = 256;
                do
                {
                    if(netTCP_SendReady(tcp_sock) == true)
                    {
                        maxlen = netTCP_GetMaxSegmentSize(tcp_sock);

                        iCount -= maxlen;

                        if(iCount < 0)
                        {
                            maxlen = iCount + maxlen;
                        }

                        sendbuf = netTCP_GetBuffer(maxlen);

                        for(i = 0; i < maxlen - 1; i++)
                        {
                            sendbuf[i] = 0x0A;
                        }

                        /* ����ʹ��������ڴ�ռ� */
                        netTCP_Send(tcp_sock, sendbuf, maxlen);
                    }
                }
                while(iCount > 0);
                break;

                /* ��Զ��TCP�ͻ��˷��Ͷ�ȡ��EEPROM���� */
            case EEPROM_MSG:
                iCount = 256;
                do
                {
                    if(netTCP_SendReady(tcp_sock) == true)
                    {
                        maxlen = netTCP_GetMaxSegmentSize(tcp_sock);

                        iCount -= maxlen;

                        if(iCount < 0)
                        {
                            maxlen = iCount + maxlen;
                        }

                        sendbuf = netTCP_GetBuffer(maxlen);

                        /* ����ʹ��������ڴ�ռ� */
                        netTCP_Send(tcp_sock, sendbuf, maxlen);
                    }
                }
                while(iCount > 0);
                break;

                /* ������������� */
            default:
                break;
            } /* switch(EvtFlag) */
        }
    } /* while(1) */
}
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
