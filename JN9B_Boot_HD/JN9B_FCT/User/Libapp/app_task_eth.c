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
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
Net_Mod Net_Mod_State={0};  //网口模式控制结构体

#if 0   //暂未用到 
/* TCPnet API的返回值 */
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

uint8_t client_conFlg = 0; //客户端连接标志
/* 任务句柄 */
osThreadId_t ThreadIdTaskMsgPro = NULL;

/* 任务属性设置 */
const osThreadAttr_t ThreadMsgPro_Attr =
{
    .name = "osRtxMsgProThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal5,
    .stack_size = 2048,
};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void AppTaskEth(void *argument);
#ifdef PHY_KSZ8081_USED
void AppTaskMsgPro(void *argument);
#endif
extern void Lan_cfg(void);
extern void w5500_init(void);
extern void W5500_TCP_Client(void);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : AppTaskEth()
**	功能说明 : 网线插拔状态检测任务
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AppTaskEth(void *argument)
{
    while(1)
    {
		if(!Net_Mod_State.Mode)
		{
			W5500_Poll();   //服务器模式
//			W5500_TcpServer();
		}
		else
		{
			if(heart_beat_flag==2)
			{
				bsp_InitW5500();
				heart_beat_flag=0;  
			}			
			W5500_TCP_Client();//客户端模式
//			W5500_TcpClient();
		}
		if((Net_Mod_State.NetMod_cnt > 60*1000) && (client_conFlg == 0)) //服务器60秒内无连接则切换到客户端模式
		{
			Net_Mod_State.NetMod_cnt=0;        //计数清零
			Net_Mod_State.NetMod_Disable=1;    //停止模式切换，关计数
			Net_Mod_State.Mode = 1;            //修改模式位=客户端
			W5500_close(SOCK_0_NUMB);          //关闭服务器socket0;
			
			bsp_InitW5500();                   //重新初始化
		}
		
        osDelay(ETH_TASK_PERIOD);  
    }
}

#ifdef PHY_KSZ8081_USED
/**********************************************************************************************************
**	函 数 名 : tcp_cb_server()
**	功能说明 : TCP Socket回调函数
**	形    参 : socket - 句柄
**             event  - 事件类型
**             *addr  - NET_ADDR类型变量(记录IP地址及端口号)
**             *buf   - 接收缓存区首地址
**             len    - 接收到的数据个数
**	返 回 值 : 0 - 禁止连接
**             1 - 允许连接
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
            APP_Printf("远程客户端请求连接IP: %d.%d.%d.%d  端口号:%d\r\n",
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

        /* Socket远程连接已经建立 */
    case netTCP_EventEstablished:
#if (TASK_ETH_PRINTF_CFG == ENABLE_CFG)
        APP_Printf("Socket is connected to remote peer\r\n");
#endif
        break;

        /* 连接断开 */
    case netTCP_EventClosed:
#if (TASK_ETH_PRINTF_CFG == ENABLE_CFG)
        APP_Printf("Connection has been closed\r\n");
#endif
        break;

        /* 连接终止 */
    case netTCP_EventAborted:
        break;

        /* 发送的数据收到远程设备应答 */
    case netTCP_EventACK:
        break;

        /* 接收到TCP数据帧，ptr指向数据地址，par记录数据长度，单位字节 */
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
**	函 数 名 : AppTaskMsgPro()
**	功能说明 : TCPnet应用任务
**	形    参 : 无
**	返 回 值 : 无
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
        /* 使能TCP_TYPE_KEEP_ALIVE，会一直保持连接 */
        netTCP_SetOption(tcp_sock, netTCP_OptionKeepAlive, 1);
    }

    while(1)
    {
        /* 等待消息 */
        EvtFlag = osThreadFlagsWait(0x00000007U, osFlagsWaitAny, usMaxBlockTime);

        /* 消息的处理 */
        if(netTCP_GetState(tcp_sock) == netTCP_StateESTABLISHED)
        {
            switch(EvtFlag)
            {
                /* 给远程TCP客户端发送读取的FLASH数据 */
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

                        /* 必须使用申请的内存空间 */
                        netTCP_Send(tcp_sock, sendbuf, maxlen);
                    }
                }
                while(iCount > 0);
                break;

                /* 给远程TCP客户端发送读取的EEPROM数据 */
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

                        /* 必须使用申请的内存空间 */
                        netTCP_Send(tcp_sock, sendbuf, maxlen);
                    }
                }
                while(iCount > 0);
                break;

                /* 其他情况不处理 */
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
