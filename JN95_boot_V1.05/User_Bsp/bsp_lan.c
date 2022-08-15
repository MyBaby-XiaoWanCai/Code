/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_lan.c
** Author      : Robert
** Date        : 2021-07-22
** Description : This file is used to initialize and drive the w5500 module
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "socket.h"
#include "w5500.h"
#include "wizchip_conf.h"
#include "bsp_lan.h"
#include "bsp_gpio.h"
#include "main.h"
/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t SN[3]={0x01,0x02,0x03};
rcv_Struct w5500_rcvbuf;
wiz_NetInfo gWIZNETINFO ;
extern uint8_t client_conFlg;

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void Lan_cfg(void);
void Net_Print(uint8_t *str);
void bsp_InitW5500(void);
void W5500_Poll(void);
void W5500_TCP_Client(void);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/


void Lan_cfg(void)
{
    gWIZNETINFO.mac[0] = MAC_ADDR_1;
    gWIZNETINFO.mac[1] = MAC_ADDR_2;
    gWIZNETINFO.mac[2] = MAC_ADDR_3;
    gWIZNETINFO.mac[3] = SN[0];           //后3位=条码SN号
    gWIZNETINFO.mac[4] = SN[1];
    gWIZNETINFO.mac[5] = SN[2];

    gWIZNETINFO.ip[0] = W5500_IP_ADDRESS[0];
    gWIZNETINFO.ip[1] = W5500_IP_ADDRESS[1];
    gWIZNETINFO.ip[2] = W5500_IP_ADDRESS[2];
    gWIZNETINFO.ip[3] = W5500_IP_ADDRESS[3];

    gWIZNETINFO.sn[0] = SUB_MASK_1;
    gWIZNETINFO.sn[1] = SUB_MASK_2;
    gWIZNETINFO.sn[2] = SUB_MASK_3;
    gWIZNETINFO.sn[3] = SUB_MASK_4;

    gWIZNETINFO.gw[0] = DEF_GATE_1;
    gWIZNETINFO.gw[1] = DEF_GATE_2;
    gWIZNETINFO.gw[2] = DEF_GATE_3;
    gWIZNETINFO.gw[3] = DEF_GATE_4;

    gWIZNETINFO.dns[0] = DNS_ADDR_1;
    gWIZNETINFO.dns[1] = DNS_ADDR_2;
    gWIZNETINFO.dns[2] = DNS_ADDR_3;
    gWIZNETINFO.dns[3] = DNS_ADDR_4;

    gWIZNETINFO.dhcp = NETINFO_STATIC;
}

/**********************************************************************************************************
**  函 数 名 : Net_Print()
**  功能说明 : 通过网口实现字符串打印功能
**  形    参 : *str - 待打印字符串首地址
**  返 回 值 : 无
**********************************************************************************************************/
void Net_Print(uint8_t *str)
{
    uint32_t len = 0;

    len = strlen((const char *)str);

    send(SOCK_0_NUMB, str, len);
}

/**********************************************************************************************************
**  函 数 名 : SPI_CrisEnter()
**  功能说明 : 进入临界区
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void SPI_CrisEnter(void)
{
    __set_PRIMASK(1);
}

/**********************************************************************************************************
**  函 数 名 : SPI_CrisExit()
**  功能说明 : 退出临界区
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void SPI_CrisExit(void)
{
    __set_PRIMASK(0);
}

/**********************************************************************************************************
**  函 数 名 : SPI_CS_Select()
**  功能说明 : 片选信号输出低电平
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void SPI_CS_Select(void)
{
    W5500_CS_ENABLE();
}

/**********************************************************************************************************
**  函 数 名 : SPI_CS_Deselect()
**  功能说明 : 片选信号输出高电平
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void SPI_CS_Deselect(void)
{
    W5500_CS_DISABLE();
}

/**********************************************************************************************************
**  函 数 名 : SPI_WriteByte()
**  功能说明 : 写一个字节数据到SPI总线
**  形    参 : TxData - 写到总线的数据
**  返 回 值 : 无
**********************************************************************************************************/
void SPI_WriteByte(uint8_t TxData)
{
    while((W5500_SPI->SR & 0x0002) == (uint16_t)RESET)
        ;     //等待发送区为空
    W5500_SPI->DR = TxData;                                 //发送一个字节数据
    while((W5500_SPI->SR & 0x0001) == (uint16_t)RESET)
        ;     //等待接收完一个字节数据
    W5500_SPI->DR;
}

/**********************************************************************************************************
**  函 数 名 : SPI_ReadByte()
**  功能说明 : 从SPI总线读取一个字节数据
**  形    参 : 无
**  返 回 值 : 从SPI总线读取到的数据
**********************************************************************************************************/
uint8_t SPI_ReadByte(void)
{
    while((W5500_SPI->SR & 0x0002) == (uint16_t)RESET)
        ;     //等待发送区为空
    W5500_SPI->DR = 0xFF;                                   //发送一个空数据产生输入数据的时钟
    while((W5500_SPI->SR & 0x0001) == (uint16_t)RESET)
        ;     //等待接收完一个字节数据
    return W5500_SPI->DR;
}

/**********************************************************************************************************
**  函 数 名 : network_init()
**  功能说明 : Intialize the network information to be used in WIZCHIP
**  形    参 : None
**  返 回 值 : None
**********************************************************************************************************/
void network_init(void)
{
    uint8_t tmpstr[6];

    ctlnetwork(CN_SET_NETINFO, (void *)&gWIZNETINFO);
    ctlnetwork(CN_GET_NETINFO, (void *)&gWIZNETINFO);

    // Display Network Information
    ctlwizchip(CW_GET_ID, (void *)tmpstr);
#if LAN_PRINTF_CFG

    printf("\r\n=== %s NET CONF ===\r\n", (char *)tmpstr);
    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", gWIZNETINFO.mac[0], gWIZNETINFO.mac[1], gWIZNETINFO.mac[2],
           gWIZNETINFO.mac[3], gWIZNETINFO.mac[4], gWIZNETINFO.mac[5]);
    printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
    printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1], gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
    printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1], gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
    printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0], gWIZNETINFO.dns[1], gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
    printf("======================\r\n");
#endif
}

/**********************************************************************************************************
**  函 数 名 : w5500_init()
**  功能说明 : Intialize the w5500 module
**  形    参 : None
**  返 回 值 : None
**********************************************************************************************************/
void w5500_init(void)
{
    uint8_t cnt = 0;    /* PHY link status check cnt, avoid drop-dead halt */
    uint8_t tmp = 0;
    uint8_t memsize[2][8] = {{8, 8, 0, 0, 0, 0, 0, 0},     //socket0服务器，socket1客户端，收发缓存均为8K
        {8, 8, 0, 0, 0, 0, 0, 0}
    };

    // 注册TCP通讯相关的回调函数
    // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP
    /* Critical section callback */
    reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);   // 注册临界区函数
    /* Chip selection call back */
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);  // 注册SPI片选信号函数
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
    reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);  // CS must be tried with LOW.
#else
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
#error "Unknown _WIZCHIP_IO_MODE_"
#else
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#endif
#endif
    /* SPI Read & Write callback function */
    reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);    // 注册读写函数

    // 初始化芯片参数
    // WIZCHIP SOCKET Buffer initialize
    if(ctlwizchip(CW_INIT_WIZCHIP, (void *)memsize) == -1)
    {
#if LAN_PRINTF_CFG
        printf("WIZCHIP Initialized fail.\r\n");
#endif
        while(1)
            ;
    }

    /* PHY link status check */
    do
    {
        if(ctlwizchip(CW_GET_PHYLINK, (void *)&tmp) == -1)
        {
#if LAN_PRINTF_CFG
            printf("Unknown PHY Link stauts.\r\n");
#endif
        }
        /* avoid drop-dead halt */
        if(++cnt > PHYLINK_CHECKNUM)
        {
#if LAN_PRINTF_CFG
            printf("PHY Link Failed.\r\n");
#endif
            break;
        }
    }
    while(tmp == PHY_LINK_OFF);

    // 初始化网络通讯参数(如：mac、ip等)
    // Network initialization
    network_init();
}

/**********************************************************************************************************
**  函 数 名 : bsp_InitW5500()
**  功能说明 : Intialize the spi interface and the w5500 module
**  形    参 : None
**  返 回 值 : None
**********************************************************************************************************/
void bsp_InitW5500(void)
{
    W5500_SPI_INIT();
    IP_Scanning();    //根据前面板拨码状态设置IP最后2字节

    Lan_cfg();
    w5500_init();
}


/**********************************************************************************************************
**  函 数 名 : W5500_Poll()
**  功能说明 : W5500做TCP server轮询函数,用于数据接收发送,需定期执行
**  形    参 : None
**  返 回 值 : None
**********************************************************************************************************/
void W5500_Poll(void)
{
    uint16_t len = 0;
	uint8_t phy_connect;
    uint8_t j,sn;
    for(sn=0;sn<2;sn++){
    j = getSn_SR(sn);  // 获取Socket_0的状态

    switch(j)
    {
        case SOCK_INIT:             // Socket_0处于初始化完成(打开)状态
            listen(sn);    //服务器模式，开始监听。               
            break;

        case SOCK_ESTABLISHED:      // Socket_0处于连接建立状态 
            client_conFlg=1;        //有客户端连接           
            if(getSn_IR(sn) & Sn_IR_CON)
            {
                setSn_IR(sn, Sn_IR_CON);               // 收到数据，清接收中断标志
            }

            while((len = recv(sn, w5500_rcvbuf.g_ucDatBuff, sizeof(w5500_rcvbuf.g_ucDatBuff))) > 0)
            {
//#if LAN_PRINTF_CFG
//                printf("%s\r\n", w5500_rcvbuf.g_ucDatBuff);
//#endif
                w5500_rcvbuf.rcv_len = len;                  //记录接收数据长度

                
              while(SOCK_BUSY == send(1, w5500_rcvbuf.g_ucDatBuff, len));
            }
			
			phy_connect = WIZCHIP_READ(PHYCFGR) & 0x01;	// 检查网线是否连接
			
			if(phy_connect == 0)								// 网线已断开
			{
				disconnect(sn);
			}
			
            break;

        case SOCK_CLOSE_WAIT:       // Socket_0处于等待关闭状态
            disconnect(sn);
            close(sn);
            break;

        case SOCK_CLOSED:           // Socket_0处于关闭状态
            
            socket(sn, Sn_MR_TCP, SOCK_0_PORT+sn, 0x00);  // 打开Socket0,打开一个本地端口
            break;
        default:
            break;
    }
}  
}

/**********************************************************************************************************
**  函 数 名 : W5500_TCP_Client()
**  功能说明 : W5500做TCP Client,使用socket1
**  形    参 : None
**  返 回 值 : None
**********************************************************************************************************/
void W5500_TCP_Client(void)
{	
    uint16_t len = 0; 
	static uint8_t Socketfirst_conflag=0;          //初次连接标志
	uint8_t cl_ip[4];  //服务器IP
    uint16_t cl_port;  //服务器端口号  
	static uint16_t w5500_local_port = 5000;        //客户端本地端口
    char f_message[50]={0};   //连接信息包    
    uint8_t  w5500_remote_ip[4] = {192,168,1,8};	//远端服务器IP	
    uint16_t w5500_remote_port = 5000;	                //远端服务器端口
    uint8_t SR_state;   //W5500状态寄存器    
    SR_state=getSn_SR(SOCK_0_NUMB);
    
	switch(SR_state)								 /* 获取socket的状态 */
	{
		case SOCK_CLOSED:											     /* socket处于关闭状态 */
			socket(SOCK_0_NUMB, Sn_MR_TCP, w5500_local_port++, Sn_MR_ND); /* 建立一个socket */
		  break;
		
		case SOCK_INIT:													   /* socket处于初始化状态 */
			connect(SOCK_0_NUMB, w5500_remote_ip, w5500_remote_port);     /* 连接服务器 */ 	
			break;
		
		case SOCK_ESTABLISHED: 										 /* socket处于连接建立状态 */
			
			if(Socketfirst_conflag==0)      /* 首次链接发Hel！+客户端IP:端口，以启动心跳机制 */
			{
				Socketfirst_conflag+=1; 
				
            getSn_DIPR(SOCK_0_NUMB,cl_ip);        // 获得连接对象的ip
            cl_port = getSn_DPORT(SOCK_0_NUMB);	 // 获得连接对象的端口号
		    sprintf(f_message,"Hello！，你的(IP:端口)是：%d.%d.%d.%d:%d\n",cl_ip[0],cl_ip[1],cl_ip[2],cl_ip[3],cl_port);	
  			send(SOCK_0_NUMB, (uint8_t *)f_message, strlen(f_message));	 /*向Server发送数据*/            
             IINCHIP_WRITE(Sn_KPALVTR(SOCK_0_NUMB), 0x01);  //设置自动心跳Keepalive 5s一次
			}			
		
			if(getSn_IR(SOCK_0_NUMB) & Sn_IR_CON)
          {
            setSn_IR(SOCK_0_NUMB, Sn_IR_CON);                 // 清除接收中断标志位
          }
		
            while((len = recv(SOCK_0_NUMB, w5500_rcvbuf.g_ucDatBuff, sizeof(w5500_rcvbuf.g_ucDatBuff))) > 0)
            {
#if LAN_PRINTF_CFG
                printf("%s\r\n", w5500_rcvbuf.g_ucDatBuff);
#endif
                w5500_rcvbuf.rcv_len = len;                  //记录接收数据长度

//                while(SOCK_BUSY == send(SOCK_0_NUMB, w5500_rcvbuf.g_ucDatBuff, len));
            }	  
		  break;
			
		case SOCK_CLOSE_WAIT: 					/*socket处于等待关闭状态*/
			disconnect(SOCK_0_NUMB);
			close(SOCK_0_NUMB);
		  break;

		default:
			Socketfirst_conflag = 0;			
			break;
	}
}
/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
