/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_lan.c
** Author      : Huang Cheng
** Date        : 2021-11-04
** Description : bsp_lan source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_lan.h"
#include "bsp_led.h"
#include "bsp_dwt.h"
#include "bsp_gpio.h"
#include "rtx_os.h"
#include "ringbuffer.h"
#include "app_task_protection.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t SN[3] = {0};                            //条码
rcv_Struct w5500_rcvbuf;                        //W5500接收结构体
__IO uint8_t Recon_TimStart = 0;                //定时重连启动标志
uint8_t  w5500_remote_ip[4] = {192, 168, 1, 8}; //远端服务器IP
uint16_t w5500_remote_port = 5000;              //远端服务器端口
extern uint8_t First_Alarm;                     //首次报警标志                     
extern uint8_t client_conFlg;                   //客户端连接标志
extern osSemaphoreId_t w5500_rcv_Semaphore;     //网口接收信号量ID

wiz_NetInfo gWIZNETINFO =                       //网络参数
{	
	.mac  = {MAC_ADDR_1, MAC_ADDR_2, MAC_ADDR_3, MAC_ADDR_4, MAC_ADDR_5, MAC_ADDR_6},
	.ip   = {IP4_ADDR_1, IP4_ADDR_2, IP4_ADDR_3, IP4_ADDR_4},
	.sn   = {SUB_MASK_1, SUB_MASK_2, SUB_MASK_3, SUB_MASK_4},
	.gw   = {DEF_GATE_1, DEF_GATE_2, DEF_GATE_3, DEF_GATE_4},
	.dns  = {DNS_ADDR_1, DNS_ADDR_2, DNS_ADDR_3, DNS_ADDR_4},
	.dhcp = NETINFO_STATIC 
};

uint8_t txbuf_size[8] = {8, 8, 0, 0, 0, 0, 0, 0};    // W5500发送缓存区分配数组，有效分配值范围：{1,2,4,8,16}
uint8_t rxbuf_size[8] = {8, 8, 0, 0, 0, 0, 0, 0};    // w5500接受缓存区分配数组
uint8_t W5500_RxBuffer[DATA_BUF_SIZE] = {0x00};      // W5500接收缓冲区

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : W5500_RST_Init()
**	功能说明 : W5500硬件复位初始化函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void W5500_RST_Init(void)
{
	stc_gpio_init_t stcLanGpioInit;
	
	/* 设置板载LanRst pin的属性:推挽输出、初始化状态为SET等 */
	(void)GPIO_StructInit(&stcLanGpioInit);
	stcLanGpioInit.u16PinState = PIN_STATE_SET;
	stcLanGpioInit.u16PullUp = PIN_PU_ON;
	stcLanGpioInit.u16PinDir = PIN_DIR_OUT;
	stcLanGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcLanGpioInit.u16PinOType = PIN_OTYPE_CMOS;
	
	/* 写入板载LanRst pin的属性 */
    (void)GPIO_Init(GPIO_PORT_W5500_RST, GPIO_PIN_W5500_RST, &stcLanGpioInit);
}

/**********************************************************************************************************
**	函 数 名 : Lan_cfg()
**	功能说明 : Lan参数初始化函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void Lan_cfg(void)
{
    gWIZNETINFO.mac[0] = MAC_ADDR_1;
    gWIZNETINFO.mac[1] = MAC_ADDR_2;
#if 1    
    gWIZNETINFO.mac[2] = MAC_ADDR_3;
    gWIZNETINFO.mac[3] = SN[0];           //后3位=条码SN号
    gWIZNETINFO.mac[4] = SN[1];
    gWIZNETINFO.mac[5] = SN[2];
#else    
    gWIZNETINFO.mac[2] = W5500_IP_ADDRESS[0];        //本机IP作为后4位MAC
    gWIZNETINFO.mac[3] = W5500_IP_ADDRESS[1];          
    gWIZNETINFO.mac[4] = W5500_IP_ADDRESS[2];
    gWIZNETINFO.mac[5] = W5500_IP_ADDRESS[3];
#endif
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
**	函 数 名 : W5500_Init()
**	功能说明 : W5500初始化函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void W5500_Init(void)
{
    /*配置接口注册函数*/
    reg_wizchip_cris_cbfunc(NULL, NULL);     //不注册自定义临界进出函数，使用默认函数，功能已改写
    reg_wizchip_cs_cbfunc(NULL, NULL);       //不注册自定义片选函数，使用默认函数，功能已改写
    reg_wizchip_spi_cbfunc(NULL, NULL);       //不注册自定义SPI单字节收发函数，使用默认函数，功能已改写,使用SPI1

    /*检查网线链接*/
    uint8_t PHY_connect = 0; //网线连接状态
    uint8_t CNT=0;
    while(PHY_connect == 0)
    {
        PHY_connect = 0x01 & IINCHIP_READ(PHYCFGR);
        bsp_DelayMS(5);

        if(++CNT>10)   //尝试10次，若网线未连接跳出
        {
//        	printf("网线未连接!\n\n");
			break;
        }
    }

    /*软件复位（复位内部寄存器）*/
    wizchip_sw_reset();
    bsp_DelayMS(1);
    /*配置MAC，IP*/
    wiz_NetInfo conf;                                               // 用于存储回读网络参数
    ctlnetwork(CN_SET_NETINFO, (void *) &gWIZNETINFO);                  // 配置网络参数
    ctlnetwork(CN_GET_NETINFO, (void *) &conf);                     // 回读
    if(memcmp(&conf, &gWIZNETINFO, sizeof(wiz_NetInfo)) == 0)
    {
//  	printf("W5500网络参数配置成功!\n\n");      // 配置成功
		__NOP();
    }
    else
    {
//  	printf("W5500网络参数配置失败!\n\n");      // 配置失败
		__NOP();
    }
    /*设置socket收发缓存区大小*/
    wizchip_init(txbuf_size,  rxbuf_size);
    /*设置最大重发次数，超时时间*/
    wiz_NetTimeout to;
    to.retry_cnt = 8;     // 重试次数，默认8次
    to.time_100us = 2000; // 超时时间，默认2000*100us = 200ms
    wizchip_settimeout(&to);
}

/**********************************************************************************************************
**	函 数 名 : bsp_InitW5500()
**	功能说明 : W5500参数配置初始化函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitW5500(void)
{
	/* W5500_SpiInit不可重复初始化 */
//	W5500_SpiInit();
//	IP_Scanning();
//	Lan_cfg();
	W5500_Init();
	InitRingBuffer(w5500_rcvbuf.g_ucDatBuff, DATA_BUF_SIZE);   //初始化环形缓存，大小1300字节
}

/**********************************************************************************************************
**	函 数 名 : W5500_TcpServer()
**	功能说明 : W5500 TCP服务器函数
**	形    参 : 无
**	返 回 值 : 无
**  说    明 : 用于测试W5500
**********************************************************************************************************/
void W5500_TcpServer(void)
{
	uint16_t size = 0;

	switch(getSn_SR(SOCK_0_NUMB))
	{
		case SOCK_ESTABLISHED:
		{			
			if(getSn_IR(SOCK_0_NUMB) & Sn_IR_CON)
			{
				setSn_IR(SOCK_0_NUMB, Sn_IR_CON);
			}
	 
			if((size = getSn_RX_RSR(SOCK_0_NUMB)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
			{
				if(size > DATA_BUF_SIZE) 
				{
					size = DATA_BUF_SIZE;
				}

				if(W5500_recv(SOCK_0_NUMB, W5500_RxBuffer, size) == 0) 
				{
					break;
				}

				while(SOCK_BUSY == W5500_send(SOCK_0_NUMB, W5500_RxBuffer, size));   
			}
			break;
		}
		case SOCK_CLOSE_WAIT:
		{
			W5500_disconnect(SOCK_0_NUMB);
			break;
		}
		case SOCK_INIT:
		{
			W5500_listen(SOCK_0_NUMB);
			break;
		}
		case SOCK_CLOSED:
		{
			W5500_socket(SOCK_0_NUMB, Sn_MR_TCP, SOCK_0_PORT, 0x00);
			break;
		}  
		default:
		{
			break;
		}	
	}
}

/**********************************************************************************************************
**	函 数 名 : W5500_TcpClient()
**	功能说明 : W5500 TCP客户端函数
**	形    参 : 无
**	返 回 值 : 无
**  说    明 : 用于测试W5500
**********************************************************************************************************/
void W5500_TcpClient(void)
{
	uint16_t size = 0;
	uint8_t destip[4] = {192, 168, 1, 8};
	uint16_t destport = 5000;
	static uint16_t any_port = 5000;	

	switch(getSn_SR(SOCK_1_NUMB))
	{
		case SOCK_ESTABLISHED:
		{
			if(getSn_IR(SOCK_1_NUMB) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
			{
				setSn_IR(SOCK_1_NUMB, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
			}

			if((size = getSn_RX_RSR(SOCK_1_NUMB)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
			{
				if(size > DATA_BUF_SIZE) 
				{
					size = DATA_BUF_SIZE; // DATA_BUF_SIZE means user defined buffer size (array)
				}
					
				if(0x00 == W5500_recv(SOCK_1_NUMB, W5500_RxBuffer, size)) // Data Receive process (H/W Rx socket buffer -> User's buffer)
				{
					break;
				}
					
				while(SOCK_BUSY == W5500_send(SOCK_1_NUMB, W5500_RxBuffer, size));  
					
			}
			
			break;
		}
		case SOCK_CLOSE_WAIT:
		{
			W5500_disconnect(SOCK_1_NUMB);
			break;
		}
		case SOCK_INIT:
		{
			connect(SOCK_1_NUMB, destip, destport);	//Try to TCP connect to the TCP server (destination)
			break;
		}
		case SOCK_CLOSED:
		{
			W5500_socket(SOCK_1_NUMB, Sn_MR_TCP, any_port++, Sn_MR_ND);
			break;
		}
		default:
		{
			break;
		}
	}
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
    uint8_t phy_connect = 0;
    uint8_t buff[DATA_BUF_SIZE] = {0};
	
	switch(getSn_SR(SOCK_0_NUMB))
	{
		case SOCK_INIT:
		{
			W5500_listen(SOCK_0_NUMB);  
			break;
		}		
		case SOCK_ESTABLISHED:
		{
			client_conFlg = 1;      //有客户端连接
			if(getSn_IR(SOCK_0_NUMB) & Sn_IR_CON)
			{
				setSn_IR(SOCK_0_NUMB, Sn_IR_CON);               // 收到数据，清接收中断标志
			}

			while((len = W5500_recv(SOCK_0_NUMB, buff, sizeof(buff))) > 0)  //存入一级缓存
			{
				w5500_rcvbuf.rcv_len += len;                  //记录接收数据长度                
				if(WriteRingBuffer(buff,w5500_rcvbuf.rcv_len)==0)
				{
					osSemaphoreRelease(w5500_rcv_Semaphore);     //释放信号量，同步网口协议解析任务
				}
			}

			phy_connect = WIZCHIP_READ(PHYCFGR) & 0x01;        // 检查网线是否连接
			if(phy_connect == 0)                                                                // 网线已断开
			{
				W5500_disconnect(SOCK_0_NUMB);
			}

			break;
		}		
		case SOCK_CLOSE_WAIT:       
		{
			W5500_disconnect(SOCK_0_NUMB);
			break;
		}
		case SOCK_CLOSED:
		{
			W5500_socket(SOCK_0_NUMB, Sn_MR_TCP, SOCK_0_PORT, 0x00);
			break;
		}		
		default:
		{
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
//    uint8_t cl_ip[4];  //服务器IP
//    uint16_t cl_port;  //服务器端口号
    static uint16_t w5500_local_port = 5000;        //客户端本地端口
//    char f_message[50]= {0};  //连接信息包
    uint8_t buff[DATA_BUF_SIZE]={0};

    switch(getSn_SR(SOCK_1_NUMB))								 
    {		
		case SOCK_ESTABLISHED:
		{
			if(Socketfirst_conflag==0)      /* 首次链接发Hel！+客户端IP:端口，以启动心跳机制 */
			{
				Socketfirst_conflag += 1;
//				getSn_DIPR(SOCK_1_NUMB, cl_ip);        // 获得连接对象的ip
//				cl_port = getSn_DPORT(SOCK_1_NUMB);	 // 获得连接对象的端口号
//				sprintf(f_message, "Hello！，你的(IP:端口)是：%d.%d.%d.%d:%d\n", cl_ip[0], cl_ip[1], cl_ip[2], cl_ip[3], cl_port);
//				W5500_send(SOCK_1_NUMB, (uint8_t *)f_message, strlen(f_message));	 /*向Server发送数据*/
				AlarmQuarry(); //重连报警上抛
				Recon_TimStart=Socketfirst_conflag;
			}

			if(getSn_IR(SOCK_1_NUMB) & Sn_IR_CON)
			{
				setSn_IR(SOCK_1_NUMB, Sn_IR_CON);                 // 清除接收中断标志位
			}

			while((len = W5500_recv(SOCK_1_NUMB, buff, sizeof(buff))) > 0)
			{
				w5500_rcvbuf.rcv_len += len;                  //记录接收数据长度                
				if( WriteRingBuffer(buff,w5500_rcvbuf.rcv_len)==0)
				{
					osSemaphoreRelease(w5500_rcv_Semaphore);     //释放信号量，同步网口协议解析任务
				}
			}
			break;
		}		
		case SOCK_CLOSE_WAIT:
		{
			W5500_disconnect(SOCK_1_NUMB);
			Socketfirst_conflag = 0;
			Recon_TimStart=Socketfirst_conflag;
			break;
		}	
		case SOCK_INIT:
		{
			connect(SOCK_1_NUMB, w5500_remote_ip, w5500_remote_port);     
			break;
		}
		case SOCK_CLOSED:
		{
			Socketfirst_conflag = 0;
			Recon_TimStart = Socketfirst_conflag;
			W5500_socket(SOCK_1_NUMB, Sn_MR_TCP, w5500_local_port++, Sn_MR_ND); /* 建立一个socket */
			break;
		}
		default:
		{
			Socketfirst_conflag = 0;
			Recon_TimStart=Socketfirst_conflag;
			break;
		}
	}
}

#if 0
/**********************************************************************************************************
**  函 数 名 : W5500_Poll()
**  功能说明 : W5500做TCP server轮询函数,用于数据接收发送,需定期执行
**  形    参 : None
**  返 回 值 : None
**********************************************************************************************************/
void W5500_Poll(void)
{
    static uint8_t Socketfirst_conflag = 0;        //初次连接标志
    uint8_t cl_ip[4];  //客户端IP
    uint16_t cl_port;  //客户端端口号
    char f_message[50] = {0}; //连接信息包
    uint16_t len = 0;
    uint8_t phy_connect;
    uint8_t j, sn;
    uint8_t buff[1300]={0};
    for(sn = 0; sn < 2; sn++)
    {
        j = getSn_SR(sn);  // 获取Socket_0的状态

        switch(j)
        {
        case SOCK_INIT:             // Socket_0处于初始化完成(打开)状态
            W5500_listen(sn);    //服务器模式，开始监听。
            break;

        case SOCK_ESTABLISHED:      // Socket_0处于连接建立状态
            client_conFlg = 1;      //有客户端连接
            if(Socketfirst_conflag == 0)    /* 首次链接发Hel！+客户端IP:端口，以启动心跳机制 */
            {
                Socketfirst_conflag += 1;
                AlarmQuarry(); //重连报警上抛
                Recon_TimStart = Socketfirst_conflag;
                //            getSn_DIPR(SOCK_0_NUMB,cl_ip);        // 获得连接对象的ip
                //            cl_port = getSn_DPORT(SOCK_0_NUMB);         // 获得连接对象的端口号
                //                    sprintf(f_message,"Hello！，你的(IP:端口)是：%d.%d.%d.%d:%d\n",cl_ip[0],cl_ip[1],cl_ip[2],cl_ip[3],cl_port);
                //                          send(SOCK_0_NUMB, (uint8_t *)f_message, strlen(f_message));         /*向Server发送数据*/
                IINCHIP_WRITE(Sn_KPALVTR(SOCK_0_NUMB), 0x01);  //设置自动心跳Keepalive 5s一次

            }
            if(getSn_IR(sn) & Sn_IR_CON)
            {
                setSn_IR(sn, Sn_IR_CON);               // 收到数据，清接收中断标志
            }

            while((len = W5500_recv(sn, buff, sizeof(buff))) > 0)  //存入一级缓存
            {
                w5500_rcvbuf.rcv_len += len;                  //记录接收数据长度                
                if( WriteRingBuffer(buff,w5500_rcvbuf.rcv_len)==0)
                osSemaphoreRelease(w5500_rcv_Semaphore);     //释放信号量，同步网口协议解析任务

               // while(SOCK_BUSY == send(1, w5500_rcvbuf.g_ucDatBuff, len));
            }

            phy_connect = WIZCHIP_READ(PHYCFGR) & 0x01;        // 检查网线是否连接

            if(phy_connect == 0)                                                                // 网线已断开
            {
                W5500_disconnect(sn);
            }

            break;

        case SOCK_CLOSE_WAIT:       // Socket_0处于等待关闭状态
            W5500_disconnect(sn);
            W5500_close(sn);

            break;

        case SOCK_CLOSED:           // Socket_0处于关闭状态
            W5500_socket(sn, Sn_MR_TCP, SOCK_0_PORT + sn, 0x00); // 打开Socket0,打开一个本地端口
            Socketfirst_conflag = 0;
            break;

        default
                :
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
    char f_message[50]= {0};  //连接信息包
    uint8_t SR_state;   //W5500状态寄存器
    uint8_t buff[1300]={0};
    SR_state=getSn_SR(SOCK_0_NUMB);

    switch(SR_state)								 /* 获取socket的状态 */
    {
    case SOCK_CLOSED:											     /* socket处于关闭状态 */
        Socketfirst_conflag = 0;
        Recon_TimStart=Socketfirst_conflag;
        W5500_socket(SOCK_0_NUMB, Sn_MR_TCP, w5500_local_port++, Sn_MR_ND); /* 建立一个socket */
        break;

    case SOCK_INIT:													   /* socket处于初始化状态 */
        connect(SOCK_0_NUMB, w5500_remote_ip, w5500_remote_port);     /* 连接服务器 */
        break;

    case SOCK_ESTABLISHED: 										 /* socket处于连接建立状态 */
//			if(heart_beat_flag==2){
//            disconnect(SOCK_0_NUMB);
//            close(SOCK_0_NUMB);
//            heart_beat_flag=0;
//            }
        if(Socketfirst_conflag==0)      /* 首次链接发Hel！+客户端IP:端口，以启动心跳机制 */
        {
            Socketfirst_conflag+=1;
            getSn_DIPR(SOCK_0_NUMB,cl_ip);        // 获得连接对象的ip
            cl_port = getSn_DPORT(SOCK_0_NUMB);	 // 获得连接对象的端口号
            sprintf(f_message,"Hello！，你的(IP:端口)是：%d.%d.%d.%d:%d\n",cl_ip[0],cl_ip[1],cl_ip[2],cl_ip[3],cl_port);
            W5500_send(SOCK_0_NUMB, (uint8_t *)f_message, strlen(f_message));	 /*向Server发送数据*/
            AlarmQuarry(); //重连报警上抛
            Recon_TimStart=Socketfirst_conflag;
//				IINCHIP_WRITE(Sn_KPALVTR(SOCK_0_NUMB), 0x01);  //设置自动心跳Keepalive 5s一次
        }

        if(getSn_IR(SOCK_0_NUMB) & Sn_IR_CON)
        {
            setSn_IR(SOCK_0_NUMB, Sn_IR_CON);                 // 清除接收中断标志位
        }

        while((len = W5500_recv(SOCK_0_NUMB, buff, sizeof(buff))) > 0)
        {
//#if LAN_PRINTF_CFG
//            printf("%s\r\n", w5500_rcvbuf.g_ucDatBuff);
//#endif
                w5500_rcvbuf.rcv_len += len;                  //记录接收数据长度                
                if( WriteRingBuffer(buff,w5500_rcvbuf.rcv_len)==0)
            osSemaphoreRelease(w5500_rcv_Semaphore);     //释放信号量，同步网口协议解析任务
//                while(SOCK_BUSY == send(SOCK_0_NUMB, w5500_rcvbuf.g_ucDatBuff, len));
        }
        break;

    case SOCK_CLOSE_WAIT: 					/*socket处于等待关闭状态*/
        W5500_disconnect(SOCK_0_NUMB);
        W5500_close(SOCK_0_NUMB);
        Socketfirst_conflag = 0;
        Recon_TimStart=Socketfirst_conflag;
        break;

    default:
        Socketfirst_conflag = 0;
        Recon_TimStart=Socketfirst_conflag;
        break;
    }
}
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


