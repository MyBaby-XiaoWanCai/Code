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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_lan.h"
#include "bsp_led.h"
#include "bsp_dwt.h"
#include "bsp_gpio.h"
#include "rtx_os.h"
#include "ringbuffer.h"
#include "app_task_protection.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t SN[3] = {0};                            //����
rcv_Struct w5500_rcvbuf;                        //W5500���սṹ��
__IO uint8_t Recon_TimStart = 0;                //��ʱ����������־
uint8_t  w5500_remote_ip[4] = {192, 168, 1, 8}; //Զ�˷�����IP
uint16_t w5500_remote_port = 5000;              //Զ�˷������˿�
extern uint8_t First_Alarm;                     //�״α�����־                     
extern uint8_t client_conFlg;                   //�ͻ������ӱ�־
extern osSemaphoreId_t w5500_rcv_Semaphore;     //���ڽ����ź���ID

wiz_NetInfo gWIZNETINFO =                       //�������
{	
	.mac  = {MAC_ADDR_1, MAC_ADDR_2, MAC_ADDR_3, MAC_ADDR_4, MAC_ADDR_5, MAC_ADDR_6},
	.ip   = {IP4_ADDR_1, IP4_ADDR_2, IP4_ADDR_3, IP4_ADDR_4},
	.sn   = {SUB_MASK_1, SUB_MASK_2, SUB_MASK_3, SUB_MASK_4},
	.gw   = {DEF_GATE_1, DEF_GATE_2, DEF_GATE_3, DEF_GATE_4},
	.dns  = {DNS_ADDR_1, DNS_ADDR_2, DNS_ADDR_3, DNS_ADDR_4},
	.dhcp = NETINFO_STATIC 
};

uint8_t txbuf_size[8] = {8, 8, 0, 0, 0, 0, 0, 0};    // W5500���ͻ������������飬��Ч����ֵ��Χ��{1,2,4,8,16}
uint8_t rxbuf_size[8] = {8, 8, 0, 0, 0, 0, 0, 0};    // w5500���ܻ�������������
uint8_t W5500_RxBuffer[DATA_BUF_SIZE] = {0x00};      // W5500���ջ�����

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : W5500_RST_Init()
**	����˵�� : W5500Ӳ����λ��ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void W5500_RST_Init(void)
{
	stc_gpio_init_t stcLanGpioInit;
	
	/* ���ð���LanRst pin������:�����������ʼ��״̬ΪSET�� */
	(void)GPIO_StructInit(&stcLanGpioInit);
	stcLanGpioInit.u16PinState = PIN_STATE_SET;
	stcLanGpioInit.u16PullUp = PIN_PU_ON;
	stcLanGpioInit.u16PinDir = PIN_DIR_OUT;
	stcLanGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcLanGpioInit.u16PinOType = PIN_OTYPE_CMOS;
	
	/* д�����LanRst pin������ */
    (void)GPIO_Init(GPIO_PORT_W5500_RST, GPIO_PIN_W5500_RST, &stcLanGpioInit);
}

/**********************************************************************************************************
**	�� �� �� : Lan_cfg()
**	����˵�� : Lan������ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void Lan_cfg(void)
{
    gWIZNETINFO.mac[0] = MAC_ADDR_1;
    gWIZNETINFO.mac[1] = MAC_ADDR_2;
#if 1    
    gWIZNETINFO.mac[2] = MAC_ADDR_3;
    gWIZNETINFO.mac[3] = SN[0];           //��3λ=����SN��
    gWIZNETINFO.mac[4] = SN[1];
    gWIZNETINFO.mac[5] = SN[2];
#else    
    gWIZNETINFO.mac[2] = W5500_IP_ADDRESS[0];        //����IP��Ϊ��4λMAC
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
**	�� �� �� : W5500_Init()
**	����˵�� : W5500��ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void W5500_Init(void)
{
    /*���ýӿ�ע�ắ��*/
    reg_wizchip_cris_cbfunc(NULL, NULL);     //��ע���Զ����ٽ����������ʹ��Ĭ�Ϻ����������Ѹ�д
    reg_wizchip_cs_cbfunc(NULL, NULL);       //��ע���Զ���Ƭѡ������ʹ��Ĭ�Ϻ����������Ѹ�д
    reg_wizchip_spi_cbfunc(NULL, NULL);       //��ע���Զ���SPI���ֽ��շ�������ʹ��Ĭ�Ϻ����������Ѹ�д,ʹ��SPI1

    /*�����������*/
    uint8_t PHY_connect = 0; //��������״̬
    uint8_t CNT=0;
    while(PHY_connect == 0)
    {
        PHY_connect = 0x01 & IINCHIP_READ(PHYCFGR);
        bsp_DelayMS(5);

        if(++CNT>10)   //����10�Σ�������δ��������
        {
//        	printf("����δ����!\n\n");
			break;
        }
    }

    /*�����λ����λ�ڲ��Ĵ�����*/
    wizchip_sw_reset();
    bsp_DelayMS(1);
    /*����MAC��IP*/
    wiz_NetInfo conf;                                               // ���ڴ洢�ض��������
    ctlnetwork(CN_SET_NETINFO, (void *) &gWIZNETINFO);                  // �����������
    ctlnetwork(CN_GET_NETINFO, (void *) &conf);                     // �ض�
    if(memcmp(&conf, &gWIZNETINFO, sizeof(wiz_NetInfo)) == 0)
    {
//  	printf("W5500����������óɹ�!\n\n");      // ���óɹ�
		__NOP();
    }
    else
    {
//  	printf("W5500�����������ʧ��!\n\n");      // ����ʧ��
		__NOP();
    }
    /*����socket�շ���������С*/
    wizchip_init(txbuf_size,  rxbuf_size);
    /*��������ط���������ʱʱ��*/
    wiz_NetTimeout to;
    to.retry_cnt = 8;     // ���Դ�����Ĭ��8��
    to.time_100us = 2000; // ��ʱʱ�䣬Ĭ��2000*100us = 200ms
    wizchip_settimeout(&to);
}

/**********************************************************************************************************
**	�� �� �� : bsp_InitW5500()
**	����˵�� : W5500�������ó�ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitW5500(void)
{
	/* W5500_SpiInit�����ظ���ʼ�� */
//	W5500_SpiInit();
//	IP_Scanning();
//	Lan_cfg();
	W5500_Init();
	InitRingBuffer(w5500_rcvbuf.g_ucDatBuff, DATA_BUF_SIZE);   //��ʼ�����λ��棬��С1300�ֽ�
}

/**********************************************************************************************************
**	�� �� �� : W5500_TcpServer()
**	����˵�� : W5500 TCP����������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� : ���ڲ���W5500
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
**	�� �� �� : W5500_TcpClient()
**	����˵�� : W5500 TCP�ͻ��˺���
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� : ���ڲ���W5500
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
**  �� �� �� : W5500_Poll()
**  ����˵�� : W5500��TCP server��ѯ����,�������ݽ��շ���,�趨��ִ��
**  ��    �� : None
**  �� �� ֵ : None
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
			client_conFlg = 1;      //�пͻ�������
			if(getSn_IR(SOCK_0_NUMB) & Sn_IR_CON)
			{
				setSn_IR(SOCK_0_NUMB, Sn_IR_CON);               // �յ����ݣ�������жϱ�־
			}

			while((len = W5500_recv(SOCK_0_NUMB, buff, sizeof(buff))) > 0)  //����һ������
			{
				w5500_rcvbuf.rcv_len += len;                  //��¼�������ݳ���                
				if(WriteRingBuffer(buff,w5500_rcvbuf.rcv_len)==0)
				{
					osSemaphoreRelease(w5500_rcv_Semaphore);     //�ͷ��ź�����ͬ������Э���������
				}
			}

			phy_connect = WIZCHIP_READ(PHYCFGR) & 0x01;        // ��������Ƿ�����
			if(phy_connect == 0)                                                                // �����ѶϿ�
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
**  �� �� �� : W5500_TCP_Client()
**  ����˵�� : W5500��TCP Client,ʹ��socket1
**  ��    �� : None
**  �� �� ֵ : None
**********************************************************************************************************/
void W5500_TCP_Client(void)
{
    uint16_t len = 0;
    static uint8_t Socketfirst_conflag=0;          //�������ӱ�־
//    uint8_t cl_ip[4];  //������IP
//    uint16_t cl_port;  //�������˿ں�
    static uint16_t w5500_local_port = 5000;        //�ͻ��˱��ض˿�
//    char f_message[50]= {0};  //������Ϣ��
    uint8_t buff[DATA_BUF_SIZE]={0};

    switch(getSn_SR(SOCK_1_NUMB))								 
    {		
		case SOCK_ESTABLISHED:
		{
			if(Socketfirst_conflag==0)      /* �״����ӷ�Hel��+�ͻ���IP:�˿ڣ��������������� */
			{
				Socketfirst_conflag += 1;
//				getSn_DIPR(SOCK_1_NUMB, cl_ip);        // ������Ӷ����ip
//				cl_port = getSn_DPORT(SOCK_1_NUMB);	 // ������Ӷ���Ķ˿ں�
//				sprintf(f_message, "Hello�������(IP:�˿�)�ǣ�%d.%d.%d.%d:%d\n", cl_ip[0], cl_ip[1], cl_ip[2], cl_ip[3], cl_port);
//				W5500_send(SOCK_1_NUMB, (uint8_t *)f_message, strlen(f_message));	 /*��Server��������*/
				AlarmQuarry(); //������������
				Recon_TimStart=Socketfirst_conflag;
			}

			if(getSn_IR(SOCK_1_NUMB) & Sn_IR_CON)
			{
				setSn_IR(SOCK_1_NUMB, Sn_IR_CON);                 // ��������жϱ�־λ
			}

			while((len = W5500_recv(SOCK_1_NUMB, buff, sizeof(buff))) > 0)
			{
				w5500_rcvbuf.rcv_len += len;                  //��¼�������ݳ���                
				if( WriteRingBuffer(buff,w5500_rcvbuf.rcv_len)==0)
				{
					osSemaphoreRelease(w5500_rcv_Semaphore);     //�ͷ��ź�����ͬ������Э���������
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
			W5500_socket(SOCK_1_NUMB, Sn_MR_TCP, w5500_local_port++, Sn_MR_ND); /* ����һ��socket */
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
**  �� �� �� : W5500_Poll()
**  ����˵�� : W5500��TCP server��ѯ����,�������ݽ��շ���,�趨��ִ��
**  ��    �� : None
**  �� �� ֵ : None
**********************************************************************************************************/
void W5500_Poll(void)
{
    static uint8_t Socketfirst_conflag = 0;        //�������ӱ�־
    uint8_t cl_ip[4];  //�ͻ���IP
    uint16_t cl_port;  //�ͻ��˶˿ں�
    char f_message[50] = {0}; //������Ϣ��
    uint16_t len = 0;
    uint8_t phy_connect;
    uint8_t j, sn;
    uint8_t buff[1300]={0};
    for(sn = 0; sn < 2; sn++)
    {
        j = getSn_SR(sn);  // ��ȡSocket_0��״̬

        switch(j)
        {
        case SOCK_INIT:             // Socket_0���ڳ�ʼ�����(��)״̬
            W5500_listen(sn);    //������ģʽ����ʼ������
            break;

        case SOCK_ESTABLISHED:      // Socket_0�������ӽ���״̬
            client_conFlg = 1;      //�пͻ�������
            if(Socketfirst_conflag == 0)    /* �״����ӷ�Hel��+�ͻ���IP:�˿ڣ��������������� */
            {
                Socketfirst_conflag += 1;
                AlarmQuarry(); //������������
                Recon_TimStart = Socketfirst_conflag;
                //            getSn_DIPR(SOCK_0_NUMB,cl_ip);        // ������Ӷ����ip
                //            cl_port = getSn_DPORT(SOCK_0_NUMB);         // ������Ӷ���Ķ˿ں�
                //                    sprintf(f_message,"Hello�������(IP:�˿�)�ǣ�%d.%d.%d.%d:%d\n",cl_ip[0],cl_ip[1],cl_ip[2],cl_ip[3],cl_port);
                //                          send(SOCK_0_NUMB, (uint8_t *)f_message, strlen(f_message));         /*��Server��������*/
                IINCHIP_WRITE(Sn_KPALVTR(SOCK_0_NUMB), 0x01);  //�����Զ�����Keepalive 5sһ��

            }
            if(getSn_IR(sn) & Sn_IR_CON)
            {
                setSn_IR(sn, Sn_IR_CON);               // �յ����ݣ�������жϱ�־
            }

            while((len = W5500_recv(sn, buff, sizeof(buff))) > 0)  //����һ������
            {
                w5500_rcvbuf.rcv_len += len;                  //��¼�������ݳ���                
                if( WriteRingBuffer(buff,w5500_rcvbuf.rcv_len)==0)
                osSemaphoreRelease(w5500_rcv_Semaphore);     //�ͷ��ź�����ͬ������Э���������

               // while(SOCK_BUSY == send(1, w5500_rcvbuf.g_ucDatBuff, len));
            }

            phy_connect = WIZCHIP_READ(PHYCFGR) & 0x01;        // ��������Ƿ�����

            if(phy_connect == 0)                                                                // �����ѶϿ�
            {
                W5500_disconnect(sn);
            }

            break;

        case SOCK_CLOSE_WAIT:       // Socket_0���ڵȴ��ر�״̬
            W5500_disconnect(sn);
            W5500_close(sn);

            break;

        case SOCK_CLOSED:           // Socket_0���ڹر�״̬
            W5500_socket(sn, Sn_MR_TCP, SOCK_0_PORT + sn, 0x00); // ��Socket0,��һ�����ض˿�
            Socketfirst_conflag = 0;
            break;

        default
                :
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
    char f_message[50]= {0};  //������Ϣ��
    uint8_t SR_state;   //W5500״̬�Ĵ���
    uint8_t buff[1300]={0};
    SR_state=getSn_SR(SOCK_0_NUMB);

    switch(SR_state)								 /* ��ȡsocket��״̬ */
    {
    case SOCK_CLOSED:											     /* socket���ڹر�״̬ */
        Socketfirst_conflag = 0;
        Recon_TimStart=Socketfirst_conflag;
        W5500_socket(SOCK_0_NUMB, Sn_MR_TCP, w5500_local_port++, Sn_MR_ND); /* ����һ��socket */
        break;

    case SOCK_INIT:													   /* socket���ڳ�ʼ��״̬ */
        connect(SOCK_0_NUMB, w5500_remote_ip, w5500_remote_port);     /* ���ӷ����� */
        break;

    case SOCK_ESTABLISHED: 										 /* socket�������ӽ���״̬ */
//			if(heart_beat_flag==2){
//            disconnect(SOCK_0_NUMB);
//            close(SOCK_0_NUMB);
//            heart_beat_flag=0;
//            }
        if(Socketfirst_conflag==0)      /* �״����ӷ�Hel��+�ͻ���IP:�˿ڣ��������������� */
        {
            Socketfirst_conflag+=1;
            getSn_DIPR(SOCK_0_NUMB,cl_ip);        // ������Ӷ����ip
            cl_port = getSn_DPORT(SOCK_0_NUMB);	 // ������Ӷ���Ķ˿ں�
            sprintf(f_message,"Hello�������(IP:�˿�)�ǣ�%d.%d.%d.%d:%d\n",cl_ip[0],cl_ip[1],cl_ip[2],cl_ip[3],cl_port);
            W5500_send(SOCK_0_NUMB, (uint8_t *)f_message, strlen(f_message));	 /*��Server��������*/
            AlarmQuarry(); //������������
            Recon_TimStart=Socketfirst_conflag;
//				IINCHIP_WRITE(Sn_KPALVTR(SOCK_0_NUMB), 0x01);  //�����Զ�����Keepalive 5sһ��
        }

        if(getSn_IR(SOCK_0_NUMB) & Sn_IR_CON)
        {
            setSn_IR(SOCK_0_NUMB, Sn_IR_CON);                 // ��������жϱ�־λ
        }

        while((len = W5500_recv(SOCK_0_NUMB, buff, sizeof(buff))) > 0)
        {
//#if LAN_PRINTF_CFG
//            printf("%s\r\n", w5500_rcvbuf.g_ucDatBuff);
//#endif
                w5500_rcvbuf.rcv_len += len;                  //��¼�������ݳ���                
                if( WriteRingBuffer(buff,w5500_rcvbuf.rcv_len)==0)
            osSemaphoreRelease(w5500_rcv_Semaphore);     //�ͷ��ź�����ͬ������Э���������
//                while(SOCK_BUSY == send(SOCK_0_NUMB, w5500_rcvbuf.g_ucDatBuff, len));
        }
        break;

    case SOCK_CLOSE_WAIT: 					/*socket���ڵȴ��ر�״̬*/
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


