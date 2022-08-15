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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "socket.h"
#include "w5500.h"
#include "wizchip_conf.h"
#include "bsp_lan.h"
#include "bsp_gpio.h"
#include "main.h"
/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t SN[3]={0x01,0x02,0x03};
rcv_Struct w5500_rcvbuf;
wiz_NetInfo gWIZNETINFO ;
extern uint8_t client_conFlg;

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void Lan_cfg(void);
void Net_Print(uint8_t *str);
void bsp_InitW5500(void);
void W5500_Poll(void);
void W5500_TCP_Client(void);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/


void Lan_cfg(void)
{
    gWIZNETINFO.mac[0] = MAC_ADDR_1;
    gWIZNETINFO.mac[1] = MAC_ADDR_2;
    gWIZNETINFO.mac[2] = MAC_ADDR_3;
    gWIZNETINFO.mac[3] = SN[0];           //��3λ=����SN��
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
**  �� �� �� : Net_Print()
**  ����˵�� : ͨ������ʵ���ַ�����ӡ����
**  ��    �� : *str - ����ӡ�ַ����׵�ַ
**  �� �� ֵ : ��
**********************************************************************************************************/
void Net_Print(uint8_t *str)
{
    uint32_t len = 0;

    len = strlen((const char *)str);

    send(SOCK_0_NUMB, str, len);
}

/**********************************************************************************************************
**  �� �� �� : SPI_CrisEnter()
**  ����˵�� : �����ٽ���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void SPI_CrisEnter(void)
{
    __set_PRIMASK(1);
}

/**********************************************************************************************************
**  �� �� �� : SPI_CrisExit()
**  ����˵�� : �˳��ٽ���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void SPI_CrisExit(void)
{
    __set_PRIMASK(0);
}

/**********************************************************************************************************
**  �� �� �� : SPI_CS_Select()
**  ����˵�� : Ƭѡ�ź�����͵�ƽ
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void SPI_CS_Select(void)
{
    W5500_CS_ENABLE();
}

/**********************************************************************************************************
**  �� �� �� : SPI_CS_Deselect()
**  ����˵�� : Ƭѡ�ź�����ߵ�ƽ
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void SPI_CS_Deselect(void)
{
    W5500_CS_DISABLE();
}

/**********************************************************************************************************
**  �� �� �� : SPI_WriteByte()
**  ����˵�� : дһ���ֽ����ݵ�SPI����
**  ��    �� : TxData - д�����ߵ�����
**  �� �� ֵ : ��
**********************************************************************************************************/
void SPI_WriteByte(uint8_t TxData)
{
    while((W5500_SPI->SR & 0x0002) == (uint16_t)RESET)
        ;     //�ȴ�������Ϊ��
    W5500_SPI->DR = TxData;                                 //����һ���ֽ�����
    while((W5500_SPI->SR & 0x0001) == (uint16_t)RESET)
        ;     //�ȴ�������һ���ֽ�����
    W5500_SPI->DR;
}

/**********************************************************************************************************
**  �� �� �� : SPI_ReadByte()
**  ����˵�� : ��SPI���߶�ȡһ���ֽ�����
**  ��    �� : ��
**  �� �� ֵ : ��SPI���߶�ȡ��������
**********************************************************************************************************/
uint8_t SPI_ReadByte(void)
{
    while((W5500_SPI->SR & 0x0002) == (uint16_t)RESET)
        ;     //�ȴ�������Ϊ��
    W5500_SPI->DR = 0xFF;                                   //����һ�������ݲ����������ݵ�ʱ��
    while((W5500_SPI->SR & 0x0001) == (uint16_t)RESET)
        ;     //�ȴ�������һ���ֽ�����
    return W5500_SPI->DR;
}

/**********************************************************************************************************
**  �� �� �� : network_init()
**  ����˵�� : Intialize the network information to be used in WIZCHIP
**  ��    �� : None
**  �� �� ֵ : None
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
**  �� �� �� : w5500_init()
**  ����˵�� : Intialize the w5500 module
**  ��    �� : None
**  �� �� ֵ : None
**********************************************************************************************************/
void w5500_init(void)
{
    uint8_t cnt = 0;    /* PHY link status check cnt, avoid drop-dead halt */
    uint8_t tmp = 0;
    uint8_t memsize[2][8] = {{8, 8, 0, 0, 0, 0, 0, 0},     //socket0��������socket1�ͻ��ˣ��շ������Ϊ8K
        {8, 8, 0, 0, 0, 0, 0, 0}
    };

    // ע��TCPͨѶ��صĻص�����
    // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP
    /* Critical section callback */
    reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);   // ע���ٽ�������
    /* Chip selection call back */
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);  // ע��SPIƬѡ�źź���
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
    reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);    // ע���д����

    // ��ʼ��оƬ����
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

    // ��ʼ������ͨѶ����(�磺mac��ip��)
    // Network initialization
    network_init();
}

/**********************************************************************************************************
**  �� �� �� : bsp_InitW5500()
**  ����˵�� : Intialize the spi interface and the w5500 module
**  ��    �� : None
**  �� �� ֵ : None
**********************************************************************************************************/
void bsp_InitW5500(void)
{
    W5500_SPI_INIT();
    IP_Scanning();    //����ǰ��岦��״̬����IP���2�ֽ�

    Lan_cfg();
    w5500_init();
}


/**********************************************************************************************************
**  �� �� �� : W5500_Poll()
**  ����˵�� : W5500��TCP server��ѯ����,�������ݽ��շ���,�趨��ִ��
**  ��    �� : None
**  �� �� ֵ : None
**********************************************************************************************************/
void W5500_Poll(void)
{
    uint16_t len = 0;
	uint8_t phy_connect;
    uint8_t j,sn;
    for(sn=0;sn<2;sn++){
    j = getSn_SR(sn);  // ��ȡSocket_0��״̬

    switch(j)
    {
        case SOCK_INIT:             // Socket_0���ڳ�ʼ�����(��)״̬
            listen(sn);    //������ģʽ����ʼ������               
            break;

        case SOCK_ESTABLISHED:      // Socket_0�������ӽ���״̬ 
            client_conFlg=1;        //�пͻ�������           
            if(getSn_IR(sn) & Sn_IR_CON)
            {
                setSn_IR(sn, Sn_IR_CON);               // �յ����ݣ�������жϱ�־
            }

            while((len = recv(sn, w5500_rcvbuf.g_ucDatBuff, sizeof(w5500_rcvbuf.g_ucDatBuff))) > 0)
            {
//#if LAN_PRINTF_CFG
//                printf("%s\r\n", w5500_rcvbuf.g_ucDatBuff);
//#endif
                w5500_rcvbuf.rcv_len = len;                  //��¼�������ݳ���

                
              while(SOCK_BUSY == send(1, w5500_rcvbuf.g_ucDatBuff, len));
            }
			
			phy_connect = WIZCHIP_READ(PHYCFGR) & 0x01;	// ��������Ƿ�����
			
			if(phy_connect == 0)								// �����ѶϿ�
			{
				disconnect(sn);
			}
			
            break;

        case SOCK_CLOSE_WAIT:       // Socket_0���ڵȴ��ر�״̬
            disconnect(sn);
            close(sn);
            break;

        case SOCK_CLOSED:           // Socket_0���ڹر�״̬
            
            socket(sn, Sn_MR_TCP, SOCK_0_PORT+sn, 0x00);  // ��Socket0,��һ�����ض˿�
            break;
        default:
            break;
    }
}  
}

/**********************************************************************************************************
**  �� �� �� : W5500_TCP_Client()
**  ����˵�� : W5500��TCP Client,ʹ��socket1
**  ��    �� : None
**  �� �� ֵ : None
**********************************************************************************************************/
void W5500_TCP_Client(void)
{	
    uint16_t len = 0; 
	static uint8_t Socketfirst_conflag=0;          //�������ӱ�־
	uint8_t cl_ip[4];  //������IP
    uint16_t cl_port;  //�������˿ں�  
	static uint16_t w5500_local_port = 5000;        //�ͻ��˱��ض˿�
    char f_message[50]={0};   //������Ϣ��    
    uint8_t  w5500_remote_ip[4] = {192,168,1,8};	//Զ�˷�����IP	
    uint16_t w5500_remote_port = 5000;	                //Զ�˷������˿�
    uint8_t SR_state;   //W5500״̬�Ĵ���    
    SR_state=getSn_SR(SOCK_0_NUMB);
    
	switch(SR_state)								 /* ��ȡsocket��״̬ */
	{
		case SOCK_CLOSED:											     /* socket���ڹر�״̬ */
			socket(SOCK_0_NUMB, Sn_MR_TCP, w5500_local_port++, Sn_MR_ND); /* ����һ��socket */
		  break;
		
		case SOCK_INIT:													   /* socket���ڳ�ʼ��״̬ */
			connect(SOCK_0_NUMB, w5500_remote_ip, w5500_remote_port);     /* ���ӷ����� */ 	
			break;
		
		case SOCK_ESTABLISHED: 										 /* socket�������ӽ���״̬ */
			
			if(Socketfirst_conflag==0)      /* �״����ӷ�Hel��+�ͻ���IP:�˿ڣ��������������� */
			{
				Socketfirst_conflag+=1; 
				
            getSn_DIPR(SOCK_0_NUMB,cl_ip);        // ������Ӷ����ip
            cl_port = getSn_DPORT(SOCK_0_NUMB);	 // ������Ӷ���Ķ˿ں�
		    sprintf(f_message,"Hello�������(IP:�˿�)�ǣ�%d.%d.%d.%d:%d\n",cl_ip[0],cl_ip[1],cl_ip[2],cl_ip[3],cl_port);	
  			send(SOCK_0_NUMB, (uint8_t *)f_message, strlen(f_message));	 /*��Server��������*/            
             IINCHIP_WRITE(Sn_KPALVTR(SOCK_0_NUMB), 0x01);  //�����Զ�����Keepalive 5sһ��
			}			
		
			if(getSn_IR(SOCK_0_NUMB) & Sn_IR_CON)
          {
            setSn_IR(SOCK_0_NUMB, Sn_IR_CON);                 // ��������жϱ�־λ
          }
		
            while((len = recv(SOCK_0_NUMB, w5500_rcvbuf.g_ucDatBuff, sizeof(w5500_rcvbuf.g_ucDatBuff))) > 0)
            {
#if LAN_PRINTF_CFG
                printf("%s\r\n", w5500_rcvbuf.g_ucDatBuff);
#endif
                w5500_rcvbuf.rcv_len = len;                  //��¼�������ݳ���

//                while(SOCK_BUSY == send(SOCK_0_NUMB, w5500_rcvbuf.g_ucDatBuff, len));
            }	  
		  break;
			
		case SOCK_CLOSE_WAIT: 					/*socket���ڵȴ��ر�״̬*/
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
