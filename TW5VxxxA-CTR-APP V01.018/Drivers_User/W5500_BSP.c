/*
*********************************************************************************************************
*
*	模块名称 : W5500驱动
*	文件名称 : W5500_BSP.c
*	版    本 : V1.0
*	说    明 :
*
*	修改记录 :
*		版本号  日期         作者     说明
*		V1.0    2020-03-12   ZYL      正式发布
*********************************************************************************************************
*/
#include "main.h"

/* 定义GPIO端口 */
#define W5500_PORT_CS              GPIOI
#define W5500_PIN_CS               GPIO_PIN_6

/* 定义口线置0和置1的宏 */
#define W5500_CS_0()           HAL_GPIO_WritePin(W5500_PORT_CS, W5500_PIN_CS, GPIO_PIN_RESET);
#define W5500_CS_1()           HAL_GPIO_WritePin(W5500_PORT_CS, W5500_PIN_CS, GPIO_PIN_SET);

void InitW5500Gpio(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOI_CLK_ENABLE(); /* 打开GPIOI时钟 */

    GPIO_Initure.Pin =  W5500_PIN_CS;         /*输出引脚 */
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  /* 设为推挽模式 */
    GPIO_Initure.Pull = GPIO_PULLUP;          /* 上拉电阻使能 */
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;/* IO口最大速度 */
    HAL_GPIO_Init(W5500_PORT_CS, &GPIO_Initure);
}

INTERFACE_TCP_SOCKET          TCP_Socket[NUM_TCP];   //Socket0作为工作用
STATUS_W5500_SERVICE          W5500_Service;         //W5500服务相关数据

//函数声明

//函数定义
//*********************************************************************************
//Description: 启动向指定的W5500区域的地址写入1字节数据SPI操作,阻塞式操作
//Parameters : Block   -数据写入区域，0-通用寄存器，n+1-Socket n寄存器区，n+2-Socket n发送缓存，n+3-Socket n接收缓存
//             Offset -对应区域内的地址偏移
//             Data   -1字节待写入数据
//Return     : 无
//*********************************************************************************
void W5500_Write_1Byte_Start(uint8_t Block,uint16_t Offset,uint8_t Data1)
{
    uint8_t Temp;
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    W5500_CS_0();
    Txdata[0]=(uint8_t)(Offset>>8);
    Txdata[1]=(uint8_t)Offset;
    Temp=Block+RWB_WRITE+OM_N_Byte;
    Txdata[2]=Temp;
    Txdata[3]=Data1;
    HAL_SPI_TransmitReceive(&hspi1, Txdata, Rxdata,4,100*4);
    W5500_CS_1();
}

void sWrite_W5500_SOCK_1Byte(uint8_t ubSocket,uint16_t uwReg,uint8_t ubDat)
{
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Txdata[0]=(uint8_t)(uwReg>>8);
    Txdata[1]=(uint8_t) uwReg;
    Txdata[2]=(uint8_t) (OM_1_Byte|RWB_WRITE|(ubSocket*0x20+0x08));
    Txdata[3]= ubDat;
    W5500_CS_0();
    HAL_SPI_TransmitReceive(&hspi1, Txdata, Rxdata,4,100*4);
    W5500_CS_1();
}

void sWrite_W5500_SOCK_2Byte(uint8_t ubSocket,uint16_t uwReg,uint16_t uwDat)
{
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Txdata[0]=(uint8_t)(uwReg>>8);
    Txdata[1]=(uint8_t) uwReg;
    Txdata[2]=(uint8_t) (OM_2_Byte|RWB_WRITE|(ubSocket*0x20+0x08));
    Txdata[3]=(uint8_t)(uwDat>>8);
    Txdata[4]=(uint8_t) uwDat;
    W5500_CS_0();
    HAL_SPI_TransmitReceive(&hspi1, Txdata, Rxdata,5,100*5);
    W5500_CS_1();
}
uint8_t sRead_W5500_SOCK_1Byte(uint8_t ubSocket, uint16_t uwReg)
{
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Txdata[0]=(uint8_t)(uwReg>>8);
    Txdata[1]=(uint8_t) uwReg;
    Txdata[2]=(uint8_t) (OM_1_Byte|RWB_READ|(ubSocket*0x20+0x08));
    Txdata[3]=0x00;
    W5500_CS_0();
    HAL_SPI_TransmitReceive(&hspi1, Txdata, Rxdata,4,100*4);
    W5500_CS_1();

    return Rxdata[3];
}
//*********************************************************************************
//Description: SPI接口W5500初始化函数
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void InitW5500_BSP(void)
{
    uint16_t Cnt=0,Cnt1=0;
    uint32_t temp1,temp2;

    W5500_Service.State=0;
    W5500_Service.Serv_Ready=0;
    W5500_Service.Cnt_TX_CH=0;

    W5500_Service.GAR[0]=GATEWAY_ADDRESS[0];
    W5500_Service.GAR[1]=GATEWAY_ADDRESS[1];
    W5500_Service.GAR[2]=GATEWAY_ADDRESS[2];
    W5500_Service.GAR[3]=GATEWAY_ADDRESS[3];

    W5500_Service.SUBR[0]=NETMASK_ADDRESS[0];
    W5500_Service.SUBR[1]=NETMASK_ADDRESS[1];
    W5500_Service.SUBR[2]=NETMASK_ADDRESS[2];
    W5500_Service.SUBR[3]=NETMASK_ADDRESS[3];

    W5500_Service.SHAR[0]=MAC_ADDR0;
    W5500_Service.SHAR[1]=MAC_ADDR1;
    W5500_Service.SHAR[2]=MAC_ADDR2;
    W5500_Service.SHAR[3]=MAC_ADDR3;
    W5500_Service.SHAR[4]=MAC_ADDR4;
    W5500_Service.SHAR[5]=MAC_ADDR5;

    W5500_Service.SIPR[0]=IP_ADDRESS[0];
    W5500_Service.SIPR[1]=IP_ADDRESS[1];
    W5500_Service.SIPR[2]=IP_ADDRESS[2];
    W5500_Service.SIPR[3]=IP_ADDRESS[3];

    //Read_Para_Net();//debug 读取网络相关地址

    TCP_Socket[SERV_TCP_WORK].Port_Source			= 5000;
    TCP_Socket[SERV_TCP_WORK].Num_Socket_Timeout	= TIMEOUT_SOCKET_WORK;//30S
    TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout	= 0;
    TCP_Socket[SERV_TCP_WORK].Req_To_Listen			= 0;
    TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus		= 0;

    temp1=(uint32_t)TCP_Socket;
    temp2=sizeof(INTERFACE_TCP_SOCKET);

    //TCP服务接口数据结构初始化
    for(Cnt=0;Cnt<NUM_TCP;Cnt++)
    {
        TCP_Socket[Cnt].Index_Socket=Cnt;
        TCP_Socket[Cnt].Num_TX_CH=NUM_TCP_TX_CH;
        TCP_Socket[Cnt].Data_RX.Status_Recive=0;
        TCP_Socket[Cnt].Data_RX.Len_RX_Buff=LEN_TCP_RX_BUFF;
        TCP_Socket[Cnt].Data_RX.pData    =&TCP_Socket[Cnt].Data_RX.Data[0];
        TCP_Socket[Cnt].RX_Incomplete = 0;

        for(Cnt1=0;Cnt1<NUM_TCP_TX_CH;Cnt1++)
        {
            TCP_Socket[Cnt].Data_TX[Cnt1].Req_TX=0;
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
    InitW5500Gpio();

    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_MR,0x80);
    HAL_Delay(20);

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
    HAL_Delay(20);

    W5500_Write_1Byte_Start(BL_REG_COMMON,REG_MR,0x02);

  //  W5500_Write_1Byte_Start(0+BL_REG_SOCK,REG_SOCK_MR,0x01);
  //  W5500_Write_1Byte_Start(0+BL_REG_SOCK,REG_SOCK_KPALVTR,0x01);

  //  W5500_Write_1Byte_Start((0x01<<5)+BL_REG_SOCK,REG_SOCK_MR,0x01);
  //  W5500_Write_1Byte_Start((0x01<<5)+BL_REG_SOCK,REG_SOCK_KPALVTR,0x01);

    sWrite_W5500_SOCK_2Byte(0, REG_SOCK_PORT0,TCP_Socket[SERV_TCP_WORK].Port_Source);
    sWrite_W5500_SOCK_1Byte(0, REG_SOCK_KPALVTR, 0x02);
    sWrite_W5500_SOCK_1Byte(0,REG_SOCK_MR,MR_TCP|ND_MC_MMB);

    sWrite_W5500_SOCK_1Byte(0,REG_SOCK_CR,CMD_CR_OPEN);
    if(sRead_W5500_SOCK_1Byte(0,REG_SOCK_SR) != SR_SOCK_INIT)
    {
        sWrite_W5500_SOCK_1Byte(0,REG_SOCK_CR,CMD_CR_CLOSE);
    }
    sWrite_W5500_SOCK_1Byte(1,REG_SOCK_CR,CMD_CR_OPEN);
    if(sRead_W5500_SOCK_1Byte(1,REG_SOCK_SR) != SR_SOCK_INIT)
    {
        sWrite_W5500_SOCK_1Byte(1,REG_SOCK_CR,CMD_CR_CLOSE);
    }
}
uint16_t sRead_W5500_SOCK_2Byte(uint8_t ubSocket, uint16_t uwReg)
{
    uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Txdata[0]=(uint8_t)(uwReg>>8);
    Txdata[1]=(uint8_t) uwReg;
    Txdata[2]=(uint8_t) (OM_2_Byte|RWB_READ|(ubSocket*0x20+0x08));
    Txdata[3]=0x00;
    Txdata[4]=0x00;
    W5500_CS_0();
    HAL_SPI_TransmitReceive(&hspi1, Txdata, Rxdata,5,100*5);
    W5500_CS_1();

    return (uint16_t)((uint16_t)(Rxdata[3]<<8)+Rxdata[4]);
}

void sW5500_Socket_Close(uint8_t ubSocket)
{
    sWrite_W5500_SOCK_1Byte(ubSocket,REG_SOCK_CR,CMD_CR_CLOSE);
    while(sRead_W5500_SOCK_1Byte(0,REG_SOCK_CR));
    sWrite_W5500_SOCK_1Byte(ubSocket,REG_SOCK_IR,0XFF);
}

uint8_t sSocket_Listen(uint8_t ubSocket)
{
    if(sRead_W5500_SOCK_1Byte(ubSocket,REG_SOCK_SR)==SR_SOCK_INIT)
    {
        sWrite_W5500_SOCK_1Byte(ubSocket,REG_SOCK_CR,CMD_CR_LISTEN);
        while(sRead_W5500_SOCK_1Byte(0,REG_SOCK_CR));
        return 1;
    }
    else
    {
        return 0;
    }
}

void sRead_SOCK_Data_Buffer(uint8_t ubSocket, uint8_t * Value,uint16_t Len)
{
    uint8_t Txdata[3+Len];
    uint8_t Rxdata[3+Len];
    uint16_t offset, Offset1;
    uint16_t i;
    offset=sRead_W5500_SOCK_2Byte(ubSocket,REG_SOCK_RX_RD0);
    W5500_CS_0();
    memset(Txdata, 0, Len+3);
    Txdata[0]=(uint8_t)(offset>>8);
    Txdata[1]=(uint8_t)offset;
    Txdata[2]=OM_N_Byte|RWB_READ|(ubSocket*0x20+0x18);

    HAL_SPI_TransmitReceive(&hspi1, Txdata, Rxdata,3+Len,100*(3+Len));
    W5500_CS_1();
    Offset1 = offset + Len;
    //Offset1 = Offset1%2048;
    sWrite_W5500_SOCK_2Byte(ubSocket, REG_SOCK_RX_RD0, Offset1);
    sWrite_W5500_SOCK_1Byte(ubSocket, REG_SOCK_CR, CMD_CR_RECV);
    /* Build the result */
    for(i = 3; i < (Len + 3); i++)
    {
        Value[i-3]= Rxdata[i];
    }
}

void sWrite_SOCK_Data_Buffer(uint8_t ubSocket, uint8_t *dat_ptr, uint16_t size)
{
    static uint8_t Txdata[1500];
    static uint8_t Rxdata[1500];
    memset(Txdata, 0, 1500);
    memset(Rxdata, 0, 1500);
    uint16_t i;
    uint8_t status;
    uint16_t offset, offset1;
    offset=sRead_W5500_SOCK_2Byte(ubSocket,REG_SOCK_TX_WR0);
    offset1=offset;
    W5500_CS_0();
    Txdata[0]=(uint8_t)(offset>>8);
    Txdata[1]=(uint8_t)offset;
    Txdata[2]=(uint8_t)(OM_N_Byte|RWB_WRITE|(ubSocket*0x20+0x10));
    HAL_SPI_TransmitReceive(&hspi1, Txdata, Rxdata,3,100*3);
    HAL_SPI_TransmitReceive(&hspi1, dat_ptr, Rxdata,size,100*size);
    W5500_CS_1();
    offset1+=size;
   // offset1 = offset1%2048;
    sWrite_W5500_SOCK_2Byte(ubSocket, REG_SOCK_TX_WR0, offset1);
    sWrite_W5500_SOCK_1Byte(ubSocket, REG_SOCK_CR, CMD_CR_SEND);
    while(sRead_W5500_SOCK_1Byte(ubSocket,REG_SOCK_CR));
    status = sRead_W5500_SOCK_1Byte(ubSocket,REG_SOCK_IR);
    while((status&IR_SEND_OK) != IR_SEND_OK )
    {
        status = sRead_W5500_SOCK_1Byte(ubSocket,REG_SOCK_SR);
        if ((status != SR_SOCK_ESTABLISHED) && (status != SR_SOCK_CLOSE_WAIT) )
        {
            sW5500_Socket_Close(ubSocket);
            return;
        }
    }
    sWrite_W5500_SOCK_1Byte(ubSocket, REG_SOCK_IR, IR_SEND_OK);
}

void W5500_Service_BSP2(void)
{
    uint8_t i,j;
    uint16_t uwCmdLen;
    uint16_t Frame_len;
    static uint8_t  Incomplete_Flag = 0;
    j = sRead_W5500_SOCK_1Byte(W5500_Service.Ptr->Index_Socket,REG_SOCK_SR);
    switch(j)
    {
        case SR_SOCK_CLOSED:

            sW5500_Socket_Close(W5500_Service.Ptr->Index_Socket);
            TCP_Socket[W5500_Service.Ptr->Index_Socket].Flag_SocketStatus     = 0;
            TCP_SERVER_FLAG &= ~(1<<5);    //标记连接断开了
            sWrite_W5500_SOCK_1Byte(W5500_Service.Ptr->Index_Socket,REG_SOCK_MR,MR_TCP|ND_MC_MMB);
            sWrite_W5500_SOCK_2Byte(W5500_Service.Ptr->Index_Socket, REG_SOCK_PORT0, W5500_Service.Ptr->Port_Source);
            sWrite_W5500_SOCK_1Byte(W5500_Service.Ptr->Index_Socket,REG_SOCK_CR,CMD_CR_OPEN);
            while(sRead_W5500_SOCK_1Byte(W5500_Service.Ptr->Index_Socket,REG_SOCK_CR));
            break;
        case SR_SOCK_INIT:
            sSocket_Listen(W5500_Service.Ptr->Index_Socket);
            break;
        case SR_SOCK_ESTABLISHED:
            TCP_SERVER_FLAG |= 1<<5;       //标记有客户端连上了

            //先检查是否有发送，然后检查是否有接收
           // if(W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Req_TX==TRUE)
            for(W5500_Service.Cnt_TX_CH = 0; W5500_Service.Cnt_TX_CH <W5500_Service.Ptr->Num_TX_CH; W5500_Service.Cnt_TX_CH++)
            {
                if(W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Req_TX==1)
                {

                    sWrite_SOCK_Data_Buffer(W5500_Service.Ptr->Index_Socket,&W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Data[0],W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Num_To_TX);
                    W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Req_TX = 0;
                    W5500_Service.Ptr->Data_TX[W5500_Service.Cnt_TX_CH].Num_To_TX = 0;
                }
            }
            //读取接收
            uwCmdLen = sRead_W5500_SOCK_2Byte(W5500_Service.Ptr->Index_Socket,REG_SOCK_RX_RSR0);
            if(uwCmdLen>0)
            {
//               if( W5500_Service.Ptr->RX_Incomplete == 0 )
//               {
//                  sRead_SOCK_Data_Buffer(W5500_Service.Ptr->Index_Socket,&W5500_Service.Ptr->Data_RX.Data[0],1);
//                  if(uwCmdLen >= W5500_Service.Ptr->Data_RX.Data[0])
//                  {
//                      sRead_SOCK_Data_Buffer(W5500_Service.Ptr->Index_Socket,&W5500_Service.Ptr->Data_RX.Data[1],W5500_Service.Ptr->Data_RX.Data[0]-1);
//                      W5500_Service.Ptr->Data_RX.Status_Recive=0x01;//接收成功
//                      W5500_Service.Ptr   = W5500_Service.Ptr->Ptr_Next;
//                  }
//                  else
//                  {
//                      W5500_Service.Ptr->RX_Incomplete = 1;
//                  }
//              }
//              else
//              {
//                  if(uwCmdLen >= W5500_Service.Ptr->Data_RX.Data[0])
//                  {
//                      sRead_SOCK_Data_Buffer(W5500_Service.Ptr->Index_Socket,&W5500_Service.Ptr->Data_RX.Data[1],W5500_Service.Ptr->Data_RX.Data[0]-1);
//                      W5500_Service.Ptr->Data_RX.Status_Recive=0x01;//接收成功
//                      W5500_Service.Ptr   = W5500_Service.Ptr->Ptr_Next;
//                      W5500_Service.Ptr->RX_Incomplete = 0;
//                  }
//             }
                            sRead_SOCK_Data_Buffer(W5500_Service.Ptr->Index_Socket,&W5500_Service.Ptr->Data_RX.Data[0],uwCmdLen);
                            W5500_Service.Ptr->Data_RX.Status_Recive=0x01;//接收成功
                            W5500_Service.Ptr->Data_RX.Num_Recived = uwCmdLen;
                            W5500_Service.Ptr   = W5500_Service.Ptr->Ptr_Next;
                            W5500_Service.Ptr->RX_Incomplete = 0;
            }
            else
            {
                W5500_Service.Ptr=W5500_Service.Ptr->Ptr_Next;
            }
            break;
        case SR_SOCK_CLOSE_WAIT:
            sW5500_Socket_Close(W5500_Service.Ptr->Index_Socket);
            TCP_Socket[W5500_Service.Ptr->Index_Socket].Flag_SocketStatus = 0;
            TCP_SERVER_FLAG &= ~(1<<5);    //标记连接断开了
            break;
        default:
            W5500_Service.Ptr   = W5500_Service.Ptr->Ptr_Next;
            break;
    }
}


//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Socket_Work_Timeout_Check(void)
{
    if(0)//根据需要自行添加
    {
        if(TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus==1)
        {
            if(1)
            {
                if(++TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout>=TCP_Socket[SERV_TCP_WORK].Num_Socket_Timeout)
                {
                    TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout=0;

                    DISABLE_INT();
                    TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus=0;
                    TCP_SERVER_FLAG &= ~(1<<5);    //标记连接断开了
                    ENABLE_INT();

                    //心跳包超时，主动断开socket连接
                    if((TCP_Socket[SERV_TCP_WORK].Sn_SR!=SR_SOCK_LISTEN)&&((TCP_Socket[SERV_TCP_WORK].Sn_SR!=SR_SOCK_SYNRECV)))
                    {
                        TCP_Socket[SERV_TCP_WORK].Req_To_Listen = 1;
                    }

                }
            }
            else
            {
                TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout = 0;
            }
        }
        else
        {
            TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout=0;
        }
    }
    else
    {
        TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout=0;
    }
}

//File end
