/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: W5500_BSP.c
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
INTERFACE_TCP_SOCKET          TCP_Socket[NUM_TCP];   //Socket0��Ϊ�����ã�Socket1��Ϊ������
STATUS_W5500_SERVICE          W5500_Service;  //W5500�����������

//��������


//��������
//*********************************************************************************
//Description: ������ָ����W5500����ĵ�ַд��1�ֽ�����SPI����,����ʽ����
//Parameters : Block   -����д������0-ͨ�üĴ�����n+1-Socket n�Ĵ�������n+2-Socket n���ͻ��棬n+3-Socket n���ջ���
//             Offset -��Ӧ�����ڵĵ�ַƫ��
//             Data   -1�ֽڴ�д������
//Return     : ��
//*********************************************************************************
void W5500_Write_1Byte_Start(Uint16 Block,Uint16 Offset,unsigned char Data1)
{
    Uint16 Temp;
    SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
    SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
    CLEAR_W5500_CS;
    Temp=Block+RWB_WRITE+OM_N_Byte;
    SpicRegs.SPITXBUF=Offset&0xFF00;
    SpicRegs.SPITXBUF=(Offset<<8)&0xFF00;
    SpicRegs.SPITXBUF=Temp<<8;
    SpicRegs.SPITXBUF=(((Uint16)Data1)<<8)&0xFF00;
    while(SpicRegs.SPIFFRX.bit.RXFFST<4);
    SET_W5500_CS;
}

//*********************************************************************************
//Description: SPI�ӿ�W5500��ʼ������
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void InitW5500_BSP(void)
{
    Uint16 Cnt=0,Cnt1=0;
    Uint32 temp1,temp2;

    W5500_Service.State=0;
    W5500_Service.Serv_Ready=0;
    W5500_Service.Cnt_TX_CH=0;

    W5500_Service.GAR[0]=192;
    W5500_Service.GAR[1]=168;
    W5500_Service.GAR[2]=1;
    W5500_Service.GAR[3]=1;

    W5500_Service.SUBR[0]=255;
    W5500_Service.SUBR[1]=255;
    W5500_Service.SUBR[2]=255;
    W5500_Service.SUBR[3]=0;

    W5500_Service.SHAR[0]=30;
    W5500_Service.SHAR[1]=30;
    W5500_Service.SHAR[2]=47;
    W5500_Service.SHAR[3]=89;
    W5500_Service.SHAR[4]=55;
    W5500_Service.SHAR[5]=67;

    W5500_Service.SIPR[0]=192;
    W5500_Service.SIPR[1]=168;
    W5500_Service.SIPR[2]=1;
    W5500_Service.SIPR[3]=11;

    Read_Para_Net();//debug

    TCP_Socket[SERV_TCP_WORK].Port_Source			= 5000;
    TCP_Socket[SERV_TCP_WORK].Num_Socket_Timeout	= TIMEOUT_SOCKET_WORK;//30S
    TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout	= 0;
    TCP_Socket[SERV_TCP_WORK].Req_To_Listen			= FALSE;
    TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus		= FALSE;

    TCP_Socket[SERV_TCP_DEBUG].Port_Source			= 5001;
    TCP_Socket[SERV_TCP_DEBUG].Num_Socket_Timeout	= 750;//30S
    TCP_Socket[SERV_TCP_DEBUG].Cnt_Socket_Timeout	= 0;
    TCP_Socket[SERV_TCP_DEBUG].Req_To_Listen		= FALSE;
    TCP_Socket[SERV_TCP_DEBUG].Flag_SocketStatus	= FALSE;

    temp1=(Uint32)TCP_Socket;
    temp2=sizeof(INTERFACE_TCP_SOCKET);

    //TCP����ӿ����ݽṹ��ʼ��
    for(Cnt=0;Cnt<NUM_TCP;Cnt++)
    {
        TCP_Socket[Cnt].Index_Socket=Cnt;
        TCP_Socket[Cnt].Num_TX_CH=NUM_TCP_TX_CH;
        TCP_Socket[Cnt].Data_RX.Status_Recive=0;
        TCP_Socket[Cnt].Data_RX.Len_RX_Buff=LEN_TCP_RX_BUFF;
        TCP_Socket[Cnt].Data_RX.pData    =&TCP_Socket[Cnt].Data_RX.Data[0];

        for(Cnt1=0;Cnt1<NUM_TCP_TX_CH;Cnt1++)
        {
            TCP_Socket[Cnt].Data_TX[Cnt1].Req_TX=FALSE;
            TCP_Socket[Cnt].Data_TX[Cnt1].Result_TX=0;
            TCP_Socket[Cnt].Data_TX[Cnt1].Len_TX_Buff=LEN_TCP_TX_BUFF;
            TCP_Socket[Cnt].Data_TX[Cnt1].pData = &TCP_Socket[Cnt].Data_TX[Cnt1].Data[0];
        }


        if(Cnt==NUM_TCP-1)
        {TCP_Socket[Cnt].Ptr_Next=(INTERFACE_TCP_SOCKET *)temp1;}
        else
        {TCP_Socket[Cnt].Ptr_Next=(INTERFACE_TCP_SOCKET *)(temp1+(Cnt+1)*temp2);}
    }

    W5500_Service.Ptr=(INTERFACE_TCP_SOCKET *)temp1;
    InitSpicGpio();

    SpicRegs.SPICCR.bit.SPISWRESET=0;
    SpicRegs.SPICCR.all =0x07;
    //SpicRegs.SPICCR.bit.SPILBK=1;
    SpicRegs.SPICTL.all =0x0E;
    SpicRegs.SPIBRR =0x0F;//20200414 ��֡����У������󽵵�SPIƵ��  SpicRegs.SPIBRR =0x07;
    SpicRegs.SPIFFTX.all=0xC008;
    SpicRegs.SPIFFRX.all=0x0008;
    SpicRegs.SPIFFCT.all=0x1;
    SpicRegs.SPIPRI.all=0x0010;
    SpicRegs.SPIPRI.bit.FREE = 1;
    SpicRegs.SPICCR.bit.SPISWRESET=1;
    SpicRegs.SPIFFTX.bit.TXFIFO=1;
    SpicRegs.SPIFFRX.bit.RXFIFORESET=1;

    EALLOW;
    CONFIG_GPIO_W5500_CS;
    EDIS;

    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_MR,0x80);
    DELAY_US(20000);

    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_GAR0,W5500_Service.GAR[0]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_GAR1,W5500_Service.GAR[1]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_GAR2,W5500_Service.GAR[2]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_GAR3,W5500_Service.GAR[3]);

    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SUBR0,W5500_Service.SUBR[0]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SUBR1,W5500_Service.SUBR[1]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SUBR2,W5500_Service.SUBR[2]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SUBR3,W5500_Service.SUBR[3]);

    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SHAR0,W5500_Service.SHAR[0]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SHAR1,W5500_Service.SHAR[1]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SHAR2,W5500_Service.SHAR[2]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SHAR3,W5500_Service.SHAR[3]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SHAR4,W5500_Service.SHAR[4]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SHAR5,W5500_Service.SHAR[5]);

    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SIPR0,W5500_Service.SIPR[0]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SIPR1,W5500_Service.SIPR[1]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SIPR2,W5500_Service.SIPR[2]);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_SIPR3,W5500_Service.SIPR[3]);

    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_PHYCFGR,0x78);
    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_PHYCFGR,0xF8);
    DELAY_US(20000);

    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_MR,0x02);

    W5500_Write_1Byte_Start(0+BL_REG_SOCK,REG_SOCK_RXBUF_SIZE,0x08);
    W5500_Write_1Byte_Start(0+BL_REG_SOCK,REG_SOCK_MR,0x01);
    W5500_Write_1Byte_Start(0+BL_REG_SOCK,REG_SOCK_KPALVTR,0x02);//Keep-Alive����

    W5500_Write_1Byte_Start((0x01<<5)+BL_REG_SOCK,REG_SOCK_RXBUF_SIZE,0x02);
    W5500_Write_1Byte_Start((0x01<<5)+BL_REG_SOCK,REG_SOCK_MR,0x01);
    W5500_Write_1Byte_Start((0x01<<5)+BL_REG_SOCK,REG_SOCK_KPALVTR,0x01);//Keep-Alive����

    W5500_Write_1Byte_Start((0x02<<5)+BL_REG_SOCK,REG_SOCK_RXBUF_SIZE,0x01);
    W5500_Write_1Byte_Start((0x03<<5)+BL_REG_SOCK,REG_SOCK_RXBUF_SIZE,0x01);
    W5500_Write_1Byte_Start((0x04<<5)+BL_REG_SOCK,REG_SOCK_RXBUF_SIZE,0x01);
    W5500_Write_1Byte_Start((0x05<<5)+BL_REG_SOCK,REG_SOCK_RXBUF_SIZE,0x01);
    W5500_Write_1Byte_Start((0x06<<5)+BL_REG_SOCK,REG_SOCK_RXBUF_SIZE,0x01);
    W5500_Write_1Byte_Start((0x07<<5)+BL_REG_SOCK,REG_SOCK_RXBUF_SIZE,0x01);
}

//*********************************************************************************
//Description: W5500�շ�����״̬��
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void W5500_Service_BSP(void)
{
    Uint16 Temp=0;
    switch(W5500_Service.State)
    {
        case W5500_IDLE:
        {
            //W5500_Service���ڿ���״̬,��鵱ǰָ���Socket�Ƿ���������Ҫ����
            if(W5500_Service.Cnt_TX_CH<W5500_Service.Ptr->Num_TX_CH)
            {
                if(W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Req_TX==TRUE)
                {   //��ǰָ���Socket����ͨ����������Ҫ����
                    SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
                    SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
                    CLEAR_W5500_CS;
                    SpicRegs.SPITXBUF   = REG_SOCK_IR&0xFF00;
                    SpicRegs.SPITXBUF   = (REG_SOCK_IR<<8)&0xFF00;
                    SpicRegs.SPITXBUF   = ((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_READ+OM_N_Byte)<<8;
                    SpicRegs.SPITXBUF   = 0xAA00;
                    SpicRegs.SPITXBUF   = 0xAA00;
                    W5500_Service.State = W5500_TX_S1;
                }
                else
                {++W5500_Service.Cnt_TX_CH;}
            }
            else
            {   //�޷������󣬼�鵱ǰָ���Socket�Ƿ�ӵ�������
                W5500_Service.Cnt_TX_CH=0;
                SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
                SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
                CLEAR_W5500_CS;
                SpicRegs.SPITXBUF   = REG_SOCK_IR&0xFF00;
                SpicRegs.SPITXBUF   = (REG_SOCK_IR<<8)&0xFF00;
                SpicRegs.SPITXBUF   = ((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_READ+OM_N_Byte)<<8;
                SpicRegs.SPITXBUF   = 0xAA00;
                SpicRegs.SPITXBUF   = 0xAA00;
                W5500_Service.State = W5500_RX_S1;
            }
            break;}

        case W5500_RX_S1:
        {
            SET_W5500_CS;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            W5500_Service.Ptr->Sn_IR=SpicRegs.SPIRXBUF&0x00FF;
            W5500_Service.Ptr->Sn_SR=SpicRegs.SPIRXBUF&0x00FF;

            //����ָ���Socket���������³�ʼ����SR_SOCK_LISTEN״̬
            if(W5500_Service.Ptr->Req_To_Listen==TRUE)
            {
                W5500_Service.Ptr->Cnt_Timeout  = 0;
                W5500_Service.State             = W5500_INIT_S1;
                return;
            }

            if(W5500_Service.Ptr->Sn_SR==SR_SOCK_ESTABLISHED)
            {
                if(((W5500_Service.Ptr->Sn_IR&IR_RECV)!=0)&&(W5500_Service.Ptr->Data_RX.Status_Recive==0))
                {   //����ָ���Socket�н���������Ҫ����
                    SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
                    SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
                    CLEAR_W5500_CS;
                    SpicRegs.SPITXBUF= REG_SOCK_TX_FSR0    &0xFF00;
                    SpicRegs.SPITXBUF=(REG_SOCK_TX_FSR0<<8)&0xFF00;
                    SpicRegs.SPITXBUF=((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_READ+OM_N_Byte)<<8;
                    SpicRegs.SPITXBUF=0xAA00;
                    SpicRegs.SPITXBUF=0xAA00;
                    SpicRegs.SPITXBUF=0xAA00;
                    SpicRegs.SPITXBUF=0xAA00;
                    SpicRegs.SPITXBUF=0xAA00;
                    SpicRegs.SPITXBUF=0xAA00;
                    SpicRegs.SPITXBUF=0xAA00;
                    SpicRegs.SPITXBUF=0xAA00;
                    SpicRegs.SPITXBUF=0xAA00;
                    SpicRegs.SPITXBUF=0xAA00;
                    SpicRegs.SPITXBUF=0xAA00;
                    SpicRegs.SPITXBUF=0xAA00;
                    W5500_Service.State=W5500_RX_S2;
                }
                else
                {
                    //����ָ���Socket�޽���������Ҫ��������ָ����һ��Socket
                    W5500_Service.Ptr=W5500_Service.Ptr->Ptr_Next;
                    W5500_Service.Cnt_TX_CH=0;
                    W5500_Service.State=W5500_IDLE;
                }
            }
            else if((W5500_Service.Ptr->Sn_SR==SR_SOCK_LISTEN)||(W5500_Service.Ptr->Sn_SR==SR_SOCK_SYNRECV))
            {
                //����ָ���Socket�����ӻ��������ڽ������账������ָ����һ��Socket
                W5500_Service.Ptr=W5500_Service.Ptr->Ptr_Next;
                W5500_Service.Cnt_TX_CH=0;
                W5500_Service.State=W5500_IDLE;
            }
            else
            {
                //����ָ���Socket��������״̬����Ҫ��Socket��ʼ����SR_SOCK_LISTEN״̬
                W5500_Service.Ptr->Cnt_Timeout=0;
               W5500_Service.Ptr->Flag_SocketStatus=FALSE;
                W5500_Service.State=W5500_INIT_S1;
            }
            break;}

        case W5500_RX_S2:
        {
            SET_W5500_CS;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_TX_FSR=Temp|(SpicRegs.SPIRXBUF&0x00FF);
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_TX_RD =Temp|(SpicRegs.SPIRXBUF&0x00FF);
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_TX_WR =Temp|(SpicRegs.SPIRXBUF&0x00FF);
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_RX_RSR=Temp|(SpicRegs.SPIRXBUF&0x00FF);
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_RX_RD =Temp|(SpicRegs.SPIRXBUF&0x00FF);
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_RX_WR =Temp|(SpicRegs.SPIRXBUF&0x00FF);

            SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
            CLEAR_W5500_CS;
            SpicRegs.SPITXBUF= W5500_Service.Socket_Reg.Sn_RX_RD    &0xFF00;
            SpicRegs.SPITXBUF=(W5500_Service.Socket_Reg.Sn_RX_RD<<8)&0xFF00;
            SpicRegs.SPITXBUF=((W5500_Service.Ptr->Index_Socket<<5)+BL_RAM_RX+RWB_READ+OM_N_Byte)<<8;
            SpicRegs.SPITXBUF=0xAA00;

            W5500_Service.Num_Last_RW=0;
            W5500_Service.Ptr->Data_RX.Num_Recived=0;
            W5500_Service.State=W5500_RX_S3_READY;
            break;}

        case W5500_RX_S3_READY:
        {
            SET_W5500_CS;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;

            //while((W5500_Service.Num_Last_RW>0)&&(W5500_Service.Ptr->Data_RX.Num_Recived<W5500_Service.Ptr->Data_RX.Len_RX_Buff))
            //{
                W5500_Service.Ptr->Data_RX.Data[0]=SpicRegs.SPIRXBUF&0x00FF;
                W5500_Service.Ptr->Data_RX.Len_Frame=W5500_Service.Ptr->Data_RX.Data[0];
                W5500_Service.Ptr->Data_RX.Num_Recived++;
                W5500_Service.Socket_Reg.Sn_RX_RD++;
                W5500_Service.Num_Last_RW=0;
            //}

            if((W5500_Service.Ptr->Data_RX.Len_Frame>W5500_Service.Socket_Reg.Sn_RX_RSR)||(W5500_Service.Ptr->Data_RX.Len_Frame==0))
            {
                W5500_Service.Ptr->Data_RX.Len_Frame=W5500_Service.Socket_Reg.Sn_RX_RSR;
            }

            SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
            CLEAR_W5500_CS;
            SpicRegs.SPITXBUF= W5500_Service.Socket_Reg.Sn_RX_RD    &0xFF00;
            SpicRegs.SPITXBUF=(W5500_Service.Socket_Reg.Sn_RX_RD<<8)&0xFF00;
            SpicRegs.SPITXBUF=((W5500_Service.Ptr->Index_Socket<<5)+BL_RAM_RX+RWB_READ+OM_N_Byte)<<8;


            if(W5500_Service.Ptr->Data_RX.Len_Frame<=13)
            {
                while(W5500_Service.Num_Last_RW<W5500_Service.Ptr->Data_RX.Len_Frame-1)
                {
                    SpicRegs.SPITXBUF=0xAA00;
                    W5500_Service.Num_Last_RW++;
                }
            }
            else
            {
                while(W5500_Service.Num_Last_RW<13)
                {
                    SpicRegs.SPITXBUF=0xAA00;
                    W5500_Service.Num_Last_RW++;
                }
            }
            W5500_Service.State=W5500_RX_S3;
            break;}

        case W5500_RX_S3:
        {
            SET_W5500_CS;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            //W5500_Service.Num_Last_RW--;
            while((W5500_Service.Num_Last_RW>0)&&(W5500_Service.Ptr->Data_RX.Num_Recived<W5500_Service.Ptr->Data_RX.Len_RX_Buff))
            {
                W5500_Service.Ptr->Data_RX.Data[W5500_Service.Ptr->Data_RX.Num_Recived++]=SpicRegs.SPIRXBUF&0x00FF;
                W5500_Service.Socket_Reg.Sn_RX_RD++;
                W5500_Service.Num_Last_RW--;
            }

            if((W5500_Service.Ptr->Data_RX.Num_Recived>=W5500_Service.Ptr->Data_RX.Len_Frame)||(W5500_Service.Ptr->Data_RX.Num_Recived>=W5500_Service.Ptr->Data_RX.Len_RX_Buff))
            {   //������ɻ򻺳�������
                SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
                SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
                CLEAR_W5500_CS;
                SpicRegs.SPITXBUF   =  REG_SOCK_RX_RD0    &0xFF00;
                SpicRegs.SPITXBUF   = (REG_SOCK_RX_RD0<<8)&0xFF00;
                SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_WRITE+OM_N_Byte)<<8;
                SpicRegs.SPITXBUF   =  W5500_Service.Socket_Reg.Sn_RX_RD    &0xFF00;
                SpicRegs.SPITXBUF   = (W5500_Service.Socket_Reg.Sn_RX_RD<<8)&0xFF00;
                W5500_Service.State =  W5500_RX_S4;
            }
            else
            {
                SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
                SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
                CLEAR_W5500_CS;
                SpicRegs.SPITXBUF   =  W5500_Service.Socket_Reg.Sn_RX_RD    &0xFF00;
                SpicRegs.SPITXBUF   = (W5500_Service.Socket_Reg.Sn_RX_RD<<8)&0xFF00;
                SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_RAM_RX+RWB_READ+OM_N_Byte)<<8;

                if(W5500_Service.Ptr->Data_RX.Len_Frame<W5500_Service.Ptr->Data_RX.Num_Recived+14)
                {
                    while(W5500_Service.Ptr->Data_RX.Num_Recived+W5500_Service.Num_Last_RW<W5500_Service.Ptr->Data_RX.Len_Frame)
                    {
                        SpicRegs.SPITXBUF=0xAA00;
                        W5500_Service.Num_Last_RW++;
                    }
                }
                else
                {
                    while(W5500_Service.Num_Last_RW<13)
                    {
                        SpicRegs.SPITXBUF=0xAA00;
                        W5500_Service.Num_Last_RW++;
                    }
                }
            }
            break;}

        case W5500_RX_S4:
        {
            SET_W5500_CS;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
            CLEAR_W5500_CS;
            SpicRegs.SPITXBUF   =  REG_SOCK_CR    &0xFF00;
            SpicRegs.SPITXBUF   = (REG_SOCK_CR<<8)&0xFF00;
            SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_WRITE+OM_N_Byte)<<8;
            SpicRegs.SPITXBUF   =  CMD_CR_RECV<<8;

            if(W5500_Service.Socket_Reg.Sn_RX_RSR<=W5500_Service.Ptr->Data_RX.Num_Recived)
            {
                W5500_Service.State =  W5500_RX_S5;
            }
            else
            {
                W5500_Service.State =  W5500_RX_S6;//��������δ��ȡ�꣬����������жϱ�־
            }
            break;}

        case W5500_RX_S5:
        {
            SET_W5500_CS;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
            CLEAR_W5500_CS;
            SpicRegs.SPITXBUF   =  REG_SOCK_IR    &0xFF00;
            SpicRegs.SPITXBUF   = (REG_SOCK_IR<<8)&0xFF00;
            SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_WRITE+OM_N_Byte)<<8;
            SpicRegs.SPITXBUF   =  IR_RECV<<8;
            W5500_Service.State =  W5500_RX_S6;
            break;}

        case W5500_RX_S6:
        {
            SET_W5500_CS;
            W5500_Service.Ptr->Data_RX.Status_Recive=0x01;//���ճɹ�
            W5500_Service.Ptr   = W5500_Service.Ptr->Ptr_Next;
            W5500_Service.State = W5500_IDLE;
            break;}

        case W5500_TX_S1:
        {
            SET_W5500_CS;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            W5500_Service.Ptr->Sn_IR=SpicRegs.SPIRXBUF&0x00FF;
            W5500_Service.Ptr->Sn_SR=SpicRegs.SPIRXBUF&0x00FF;
            if(W5500_Service.Ptr->Sn_SR==SR_SOCK_ESTABLISHED)
            {   //��W5500���ͻ��������пռ�
                SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
                SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
                CLEAR_W5500_CS;
                SpicRegs.SPITXBUF= REG_SOCK_TX_FSR0    &0xFF00;
                SpicRegs.SPITXBUF=(REG_SOCK_TX_FSR0<<8)&0xFF00;
                SpicRegs.SPITXBUF=((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_READ+OM_N_Byte)<<8;
                SpicRegs.SPITXBUF=0xAA00;
                SpicRegs.SPITXBUF=0xAA00;
                SpicRegs.SPITXBUF=0xAA00;
                SpicRegs.SPITXBUF=0xAA00;
                SpicRegs.SPITXBUF=0xAA00;
                SpicRegs.SPITXBUF=0xAA00;
                SpicRegs.SPITXBUF=0xAA00;
                SpicRegs.SPITXBUF=0xAA00;
                SpicRegs.SPITXBUF=0xAA00;
                SpicRegs.SPITXBUF=0xAA00;
                SpicRegs.SPITXBUF=0xAA00;
                SpicRegs.SPITXBUF=0xAA00;
                W5500_Service.State=W5500_TX_S2;
            }
            else
            {   //Socket���ڷ�����״̬������ʧ��
                W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Req_TX      = FALSE;//�巢�������־
                W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Result_TX   = 0x03;//����ʧ��
                W5500_Service.State=W5500_IDLE;
            }
            break;}

        case W5500_TX_S2:
        {
            SET_W5500_CS;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_TX_FSR=Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff();
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_TX_RD =Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff();
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_TX_WR =Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff();
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_RX_RSR=Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff()&0x0FFF;
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_RX_RD =Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff()&0x0FFF;
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            W5500_Service.Socket_Reg.Sn_RX_WR =Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff()&0x0FFF;
            if(W5500_Service.Socket_Reg.Sn_TX_FSR>=W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Num_To_TX)
            {   //W5500���ͻ��������пռ��㹻��������һ�׶�
                W5500_Service.Cnt_TX        = 0;
                W5500_Service.Num_Last_RW   = 0;
                W5500_Service.State         = W5500_TX_S3;
            }
            else
            {
                //W5500���ͻ��������пռ䲻����ֱ�ӽ�����ǰSocket�ķ������̽��������,�ȴ���һ����ѯʱ����
                W5500_Service.Cnt_TX_CH=W5500_Service.Ptr->Num_TX_CH;
                W5500_Service.State=W5500_IDLE;
            }
            break;}

        case W5500_TX_S3:
        {
            SET_W5500_CS;
            if((W5500_Service.Cnt_TX>=W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Num_To_TX)||(W5500_Service.Cnt_TX>=W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Len_TX_Buff))
            {   //����д��W5500����RAM��ɻ��ͼ���������DSP���ͻ��������ȣ���������д��
                W5500_Service.State=W5500_TX_S4;
            }
            else
            {
                CLEAR_W5500_CS;
                SpicRegs.SPITXBUF   =  W5500_Service.Socket_Reg.Sn_TX_WR    &0xFF00;
                SpicRegs.SPITXBUF   = (W5500_Service.Socket_Reg.Sn_TX_WR<<8)&0xFF00;
                SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_RAM_TX+RWB_WRITE+OM_N_Byte)<<8;

                if(W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Num_To_TX<W5500_Service.Cnt_TX+14)
                {   //ʣ���������������13�ֽ�
                    while(W5500_Service.Cnt_TX<W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Num_To_TX)
                    {
                        SpicRegs.SPITXBUF=W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Data[W5500_Service.Cnt_TX++]<<8;
                        W5500_Service.Socket_Reg.Sn_TX_WR++;
                    }
                    W5500_Service.State=W5500_TX_S4;
                }
                else
                {   //ʣ����������ݴ���13�ֽ�
                    W5500_Service.Num_Last_RW=0;
                    while(W5500_Service.Num_Last_RW<13)
                    {
                        SpicRegs.SPITXBUF=W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Data[W5500_Service.Cnt_TX++]<<8;
                        W5500_Service.Num_Last_RW++;
                        W5500_Service.Socket_Reg.Sn_TX_WR++;
                    }
                }
            }
            break;}

        case W5500_TX_S4:
        {
            //
            SET_W5500_CS;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
            CLEAR_W5500_CS;
            SpicRegs.SPITXBUF   =  REG_SOCK_TX_WR0    &0xFF00;
            SpicRegs.SPITXBUF   = (REG_SOCK_TX_WR0<<8)&0xFF00;
            SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_WRITE+OM_N_Byte)<<8;
            SpicRegs.SPITXBUF   =  W5500_Service.Socket_Reg.Sn_TX_WR    &0xFF00;
            SpicRegs.SPITXBUF   = (W5500_Service.Socket_Reg.Sn_TX_WR<<8)&0xFF00;
            W5500_Service.State =  W5500_TX_S5;
            break;}

        case W5500_TX_S5:
        {
            SET_W5500_CS;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
            CLEAR_W5500_CS;
            SpicRegs.SPITXBUF   =  REG_SOCK_CR    &0xFF00;
            SpicRegs.SPITXBUF   = (REG_SOCK_CR<<8)&0xFF00;
            SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_WRITE+OM_N_Byte)<<8;
            SpicRegs.SPITXBUF   =  CMD_CR_SEND<<8;
            W5500_Service.State =  W5500_TX_S6;
            W5500_Service.Ptr->Cnt_Timeout=0;
            break;}

        case W5500_TX_S6:
        {
            SET_W5500_CS;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
            CLEAR_W5500_CS;
            SpicRegs.SPITXBUF   =  REG_SOCK_IR&0xFF00;
            SpicRegs.SPITXBUF   = (REG_SOCK_IR<<8)&0xFF00;
            SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_READ+OM_N_Byte)<<8;
            SpicRegs.SPITXBUF   =  0xAA00;
            SpicRegs.SPITXBUF   =  0xAA00;
            W5500_Service.State =  W5500_TX_S7;
            break;}

        case W5500_TX_S7:
        {
            SET_W5500_CS;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            W5500_Service.Ptr->Sn_IR    = SpicRegs.SPIRXBUF&0x00FF;
            W5500_Service.Ptr->Sn_SR    = SpicRegs.SPIRXBUF&0x00FF;
            //10mS��ʱҲ��Ϊ���ͳɹ�
            if((W5500_Service.Ptr->Sn_IR&IR_SEND_OK!=0)||(++W5500_Service.Ptr->Cnt_Timeout>100))
            {
                W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Req_TX=FALSE;
                W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Result_TX=0x02;//���ͳɹ�
                W5500_Service.State=W5500_IDLE;
            }
            else if(W5500_Service.Ptr->Sn_IR&(IR_TIMEOUT|IR_DISCON)!=0)
            {
                W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Req_TX=FALSE;
                W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Result_TX=0x03;//����ʧ��
                W5500_Service.State=W5500_IDLE;
            }
            else
            {W5500_Service.State=W5500_TX_S6;}
            break;}

        case W5500_INIT_S1:
        {
            //��ȡ����ָ���Socket״̬�Ĵ���
            SET_W5500_CS;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
            CLEAR_W5500_CS;
            SpicRegs.SPITXBUF   =  REG_SOCK_IR&0xFF00;
            SpicRegs.SPITXBUF   = (REG_SOCK_IR<<8)&0xFF00;
            SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_READ+OM_N_Byte)<<8;
            SpicRegs.SPITXBUF   =  0xAA00;
            SpicRegs.SPITXBUF   =  0xAA00;
            W5500_Service.State =  W5500_INIT_S2;
            break;}

        case W5500_INIT_S2:
        {
            SET_W5500_CS;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            W5500_Service.Ptr->Sn_IR=SpicRegs.SPIRXBUF&0x00FF;
            W5500_Service.Ptr->Sn_SR=SpicRegs.SPIRXBUF&0x00FF;
            if(W5500_Service.Ptr->Sn_SR==SR_SOCK_CLOSED)
            {
                //Socket�Ѵ��ڹر�״̬��������һ��
                W5500_Service.Ptr->Cnt_Timeout=0;
                W5500_Service.State=W5500_INIT_S3;
            }
            else
            {
                //Socket���ڷǹر�״̬���·��ر�Socket����ٴμ��Socket��״̬
                CLEAR_W5500_CS;
                SpicRegs.SPITXBUF   =  REG_SOCK_CR&0xFF00;
                SpicRegs.SPITXBUF   = (REG_SOCK_CR<<8)&0xFF00;
                SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_WRITE+OM_N_Byte)<<8;
                SpicRegs.SPITXBUF   =  CMD_CR_CLOSE<<8;//>CMD_CR_CLOSE<<8;
                if(++W5500_Service.Ptr->Cnt_Timeout>3)
                {
                    W5500_Service.Ptr       = W5500_Service.Ptr->Ptr_Next;
                    W5500_Service.Cnt_TX_CH = 0;
                    W5500_Service.State     = W5500_IDLE;
                }
                else
                {
                    W5500_Service.State     = W5500_INIT_S1;
                }
            }
            break;}

        case W5500_INIT_S3:
        {
            SET_W5500_CS;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
            CLEAR_W5500_CS;
            SpicRegs.SPITXBUF   =  REG_SOCK_PORT0&0xFF00;
            SpicRegs.SPITXBUF   = (REG_SOCK_PORT0<<8)&0xFF00;
            SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_READ+OM_N_Byte)<<8;
            SpicRegs.SPITXBUF   =  0xAA00;
            SpicRegs.SPITXBUF   =  0xAA00;
            W5500_Service.State =  W5500_INIT_S4;
            break;}

        case W5500_INIT_S4:
        {
            SET_W5500_CS;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
            Temp=Temp|(SpicRegs.SPIRXBUF&0x00FF);
            if(W5500_Service.Ptr->Port_Source==Temp)
            {
                W5500_Service.Ptr->Cnt_Timeout=0;
                W5500_Service.State=W5500_INIT_S5;
            }
            else
            {
                //����TCP�����˿ں�
                SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
                SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
                CLEAR_W5500_CS;
                SpicRegs.SPITXBUF =  REG_SOCK_PORT0&0xFF00;
                SpicRegs.SPITXBUF = (REG_SOCK_PORT0<<8)&0xFF00;
                SpicRegs.SPITXBUF =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_WRITE+OM_N_Byte)<<8;
                SpicRegs.SPITXBUF =  W5500_Service.Ptr->Port_Source&0xFF00;
                SpicRegs.SPITXBUF = (W5500_Service.Ptr->Port_Source<<8)&0xFF00;
                if(++W5500_Service.Ptr->Cnt_Timeout>3)
                {
                    W5500_Service.Cnt_TX_CH = 0;
                    W5500_Service.Ptr       = W5500_Service.Ptr->Ptr_Next;
                    W5500_Service.State     = W5500_IDLE;
                }
                else
                {
                    W5500_Service.State=W5500_INIT_S3;
                }
            }
            break;}

        //
        case W5500_INIT_S5:
        {
            SET_W5500_CS;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
            CLEAR_W5500_CS;
            SpicRegs.SPITXBUF   =  REG_SOCK_IR&0xFF00;
            SpicRegs.SPITXBUF   = (REG_SOCK_IR<<8)&0xFF00;
            SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_READ+OM_N_Byte)<<8;
            SpicRegs.SPITXBUF   =  0xAA00;
            SpicRegs.SPITXBUF   =  0xAA00;
            W5500_Service.State =  W5500_INIT_S6;
            break;}

        case W5500_INIT_S6:
        {
            SET_W5500_CS;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            W5500_Service.Ptr->Sn_IR=SpicRegs.SPIRXBUF&0x00FF;
            W5500_Service.Ptr->Sn_SR=SpicRegs.SPIRXBUF&0x00FF;
            if(W5500_Service.Ptr->Sn_SR==SR_SOCK_INIT)
            {
                //Socket����ɣ�������һ��
                W5500_Service.Ptr->Cnt_Timeout  = 0;
                W5500_Service.State             = W5500_INIT_S7;
            }
            else
            {
                SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
                SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
                CLEAR_W5500_CS;
                SpicRegs.SPITXBUF   =  REG_SOCK_CR&0xFF00;
                SpicRegs.SPITXBUF   = (REG_SOCK_CR<<8)&0xFF00;
                SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_WRITE+OM_N_Byte)<<8;
                SpicRegs.SPITXBUF   =  CMD_CR_OPEN<<8;
                if(++W5500_Service.Ptr->Cnt_Timeout>3)
                {
                    W5500_Service.Cnt_TX_CH = 0;
                    W5500_Service.Ptr       = W5500_Service.Ptr->Ptr_Next;
                    W5500_Service.State     = W5500_IDLE;
                }
                else
                {
                    W5500_Service.State = W5500_INIT_S5;
                }
            }
            break;}

        case W5500_INIT_S7:
        {
            SET_W5500_CS;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
            SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
            CLEAR_W5500_CS;
            SpicRegs.SPITXBUF   =  REG_SOCK_IR&0xFF00;
            SpicRegs.SPITXBUF   = (REG_SOCK_IR<<8)&0xFF00;
            SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_READ+OM_N_Byte)<<8;
            SpicRegs.SPITXBUF   =  0xAA00;
            SpicRegs.SPITXBUF   =  0xAA00;
            W5500_Service.State = W5500_INIT_S8;
            break;}

        case W5500_INIT_S8:
        {
            SET_W5500_CS;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            Temp=SpicRegs.SPIRXBUF;
            W5500_Service.Ptr->Sn_IR=SpicRegs.SPIRXBUF&0x00FF;
            W5500_Service.Ptr->Sn_SR=SpicRegs.SPIRXBUF&0x00FF;
            if(W5500_Service.Ptr->Sn_SR==SR_SOCK_LISTEN)
            {
                //��ǰSocket��������״̬��Socket��ʼ����ɣ�����ָ����һ��Socket
                W5500_Service.Ptr->Req_To_Listen= FALSE;
                W5500_Service.Cnt_TX_CH         = 0;
                W5500_Service.Ptr->Cnt_Timeout  = 0;
                W5500_Service.Ptr               = W5500_Service.Ptr->Ptr_Next;
                W5500_Service.State             = W5500_IDLE;
            }
            else
            {
                CLEAR_W5500_CS;
                SpicRegs.SPITXBUF   =  REG_SOCK_CR&0xFF00;
                SpicRegs.SPITXBUF   = (REG_SOCK_CR<<8)&0xFF00;
                SpicRegs.SPITXBUF   =((W5500_Service.Ptr->Index_Socket<<5)+BL_REG_SOCK+RWB_WRITE+OM_N_Byte)<<8;
                SpicRegs.SPITXBUF   =  CMD_CR_LISTEN<<8;
                if(++W5500_Service.Ptr->Cnt_Timeout>3)
                {   //��ʱ����ʼ��ʧ�ܣ��¸�ѭ���ٴγ��Գ�ʼ��
                    W5500_Service.Cnt_TX_CH = 0;
                    W5500_Service.Ptr       = W5500_Service.Ptr->Ptr_Next;
                    W5500_Service.State     = W5500_IDLE;
                }
                else
                {
                    W5500_Service.State = W5500_INIT_S7;
                }
            }
            break;}

        case 0:
        {
            W5500_Service.State=W5500_IDLE;
            W5500_Service.Serv_Ready=0x01;
            break;}

        default:{W5500_Service.State=W5500_IDLE;}
    }

}


//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Socket_Work_Timeout_Check(void)
{
    if(Interface_DCDC.Req_Aging==FALSE)
    {
    	if(TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus==TRUE)
    	{
    		if(++TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout>=TCP_Socket[SERV_TCP_WORK].Num_Socket_Timeout)
    		{
    			TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout=0;

    			DINT;
    			TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus=FALSE;
    			EINT;

    			//��������ʱ�������Ͽ�socket����
    			if((TCP_Socket[SERV_TCP_WORK].Sn_SR!=SR_SOCK_LISTEN)&&((TCP_Socket[SERV_TCP_WORK].Sn_SR!=SR_SOCK_SYNRECV)))
    			{
    				TCP_Socket[SERV_TCP_WORK].Req_To_Listen = TRUE;
    			}
                if((DCDC_Service.Mode_Parall==SINGLE)||(DCDC_Service.Mode_Parall==PARALL_MASTER))
                {
    			   if(WorkStep_Service.Step_State==cWorkStepRunStatus)
    			   {
    				 WorkStep_Service.Req_Stop_Socket_Err=TRUE;
    			   }
    			   else if((DCDC_Service.Now_Mode!=DCDC_STANDBY)&&(DCDC_Service.Now_Mode!=DCDC_SHUTDOWN))
    			   {
    				Interface_DCDC.Req_Mode = DCDC_STANDBY;
    				Interface_DCDC.Req_ChangeMode = TRUE;
    			   }
                }
    		}
    	}
    	else
    	{
    	    if((DCDC_Service.Mode_Parall==SINGLE)||(DCDC_Service.Mode_Parall==PARALL_MASTER))
    	    {
			    if(WorkStep_Service.Step_State==cWorkStepRunStatus)
			    {
				   WorkStep_Service.Req_Stop_Socket_Err=TRUE;
			    }
			    else if((DCDC_Service.Now_Mode!=DCDC_STANDBY)&&(DCDC_Service.Now_Mode!=DCDC_SHUTDOWN))
			    {
				   Interface_DCDC.Req_Mode = DCDC_STANDBY;
				   Interface_DCDC.Req_ChangeMode = TRUE;
			    }
    		    TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout=0;
    	    }
    	}
    }
    else
    {
        TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout=0;
    }
}

//File end
