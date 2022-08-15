/*
*********************************************************************************************************
*
*	模块名称 : W5500驱动
*	文件名称 : W5500_BSP.h
*	版    本 : V1.0
*	说    明 :
*
*	修改记录 :
*		版本号  日期         作者     说明
*		V1.0    2020-03-12   ZYL      正式发布
*********************************************************************************************************
*/

#ifndef _W5500_BSP_H_
#define _W5500_BSP_H_

#include "stdint.h"

#define w5500_EN                1

//常量定义
//Socket配置常量定义
#define NUM_TCP                 2           //Socket总数量
#define NUM_TCP_TX_CH           3           //TCP发送请求通道数量

#define LEN_TCP_RX_BUFF         1460        //TCP接收缓冲区长度
#define LEN_TCP_TX_BUFF         1460        //TCP发送缓冲区长度

#define SERV_TCP_WORK           0           //工作使用TCP接口Socket0

//SOCKET0发送通道使用定义
#define CH_TX_RETURN            0           //网口命令响应
#define CH_TX_UPDATA            1           //上报数据用
#define CH_TX_ALARM             2           //故障和告警代码上报用

//结构体定义
//TCP套接字接口定义
typedef struct TCP_SOCKET_RX
{
    unsigned char Data[LEN_TCP_RX_BUFF];    //接收数据缓冲区
    unsigned char *pData;                   //debug 指向Data数据缓冲区起始的指针
    uint16_t Status_Recive;                 //0-DSP接收缓存区数据已处理完，允许从W5500接收缓存向DSP接收缓存拷贝数据,
                                            //1-数据拷贝完成，等待DSP处理数据，停止从W5500接收缓存向DSP接收缓存拷贝数据
    uint16_t Num_Recived;                   //从W5500接收缓存向DSP接收缓存已拷贝数据数量
    uint16_t Len_RX_Buff;                   //DSP接收数据缓冲区长度

} TCP_SOCKET_RX;

typedef struct TCP_SOCKET_TX
{
    unsigned char Data[LEN_TCP_TX_BUFF];    //发送缓存区
    unsigned char *pData;                   //指向Data数据缓冲区起始的指针
    uint16_t Req_TX;                        //0-无TCP发送请求，1-有TCP发送请求
    uint16_t Num_To_TX;                     //需要发送的字节数
    uint16_t Result_TX;                     //TCP发送结果
    uint16_t Len_TX_Buff;                   //DSP发送缓存区长度

} TCP_SOCKET_TX;


typedef struct INTERFACE_TCP_SOCKET
{
    TCP_SOCKET_RX Data_RX;                  //接收缓冲区数据 结构
    TCP_SOCKET_TX Data_TX[NUM_TCP_TX_CH];   //发送缓冲区数据结构
    uint16_t Sn_IR;                         //从W5500中读取到的Socket中断标志
    uint16_t Sn_SR;                         //从W5500中读取到的Socket连接状态，
    uint16_t Port_Source;                   //此Socket连接使用的本地TCP端口号
    unsigned char IP_Dest[4];               //此Socket连接的远端IP地址
    unsigned char MAC_Dest[6];              //此Socket连接的远端MAC地址
    uint16_t Port_Dest;                     //此Socket连接的远端端口号
    uint16_t Num_TX_CH;                     //此Socket发送通道数量
    uint16_t Index_Socket;                  //当前Socket的索引号,0~7
    uint16_t Cnt_Timeout;                   //Socket内部管理超时计数用
    uint16_t Req_To_Listen;                 //请求Socket重新进入Listen状态
    uint16_t Num_Socket_Timeout;            //Socket连接超时时间，40mS/LSB
    uint16_t Cnt_Socket_Timeout;            //Socket连接超时计数器
    uint16_t Flag_SocketStatus;             //Socket连接状态
    uint16_t RX_Incomplete;

  struct INTERFACE_TCP_SOCKET *Ptr_Next;    //指向下一个TCP服务指针
} INTERFACE_TCP_SOCKET;

typedef struct DATA_SOCKET_REG
{
    unsigned char Sn_MR;
    unsigned char Sn_IR;
    unsigned char Sn_SR;
    uint16_t Sn_PORT;
    unsigned char Sn_DHAR[6];
    unsigned char Sn_DIPR[4];
    uint16_t Sn_DPORT;
    uint16_t Sn_MSSR;
    unsigned char Sn_TOS;
    unsigned char Sn_TTL;
    uint16_t Sn_RXBUF_SIZE;
    uint16_t Sn_TXBUF_SIZE;
    uint16_t Sn_TX_FSR;
    uint16_t Sn_TX_RD;
    uint16_t Sn_TX_WR;
    uint16_t Sn_RX_RSR;
    uint16_t Sn_RX_RD;
    uint16_t Sn_RX_WR;
    uint16_t Sn_IMR;
    uint16_t Sn_FRAG;
    uint16_t Sn_KPALVTR;
}DATA_SOCKET_REG;


//W5500服务接口定义
typedef struct STATUS_W5500_SERVICE
{
    unsigned char GAR[4];                   //网关IP地址
    unsigned char SUBR[4];                  //子网掩码
    unsigned char SHAR[6];                  //本地MAC地址
    unsigned char SIPR[4];                  //本地IP地址
    uint16_t State;                         //网口服务状态机状态
    // uint16_t W5500_SIR;                     //读取到的W5500SIR寄存器值
    uint16_t Cnt_TX;                        //发送字节数量计数器
    uint16_t Cnt_RX;                        //接收字节数量计数器
    uint16_t Cnt_TX_CH;                     //当前指向的Socket的发送通道序号
    uint16_t Num_Last_RW;                   //上一次读取数据字节数
    uint16_t Cnt_Overtime;                  //超时用计数器
    uint16_t Serv_Ready;                    //服务是否可以使用标志，0-不可以，1-可以
  DATA_SOCKET_REG Socket_Reg;               //当前指向的socket的寄存器
  INTERFACE_TCP_SOCKET *Ptr;                //指向当前正在服务的Socket
} STATUS_W5500_SERVICE;

//W5500状态机状态定义
#define W5500_STRUCT_INIT       0x00        //结构体数据初始化
#define W5500_IDLE              0x01        //空闲状态
#define W5500_RX_S1             0x02        //接收步骤1
#define W5500_RX_S2             0x03        //
#define W5500_RX_S3             0x05        //
#define W5500_RX_S4             0x06        //
#define W5500_RX_S5             0x07        //
#define W5500_RX_S6             0x08        //

#define W5500_TX_S1             0x0D        //发送步骤1
#define W5500_TX_S2             0x0E        //
#define W5500_TX_S3             0x0F        //
#define W5500_TX_S4             0x10        //
#define W5500_TX_S5             0x11        //
#define W5500_TX_S6             0x12        //
#define W5500_TX_S7             0x13        //

#define W5500_INIT_S1           0x14        //初始化Socket到Listen步骤1
#define W5500_INIT_S2           0x15        //
#define W5500_INIT_S3           0x16        //
#define W5500_INIT_S4           0x17        //
#define W5500_INIT_S5           0x18        //
#define W5500_INIT_S6           0x19        //
#define W5500_INIT_S7           0x1A        //
#define W5500_INIT_S8           0x1B        //
//联合体定义


//对外部文件开放全局变量声明
extern STATUS_W5500_SERVICE W5500_Service;
extern INTERFACE_TCP_SOCKET TCP_Socket[];

//对外部文件开放函数声明
extern uint16_t Qurey_W5500_RX(INTERFACE_TCP_SOCKET *Ptr);
extern void Clear_TCP_RX(INTERFACE_TCP_SOCKET *Ptr);
extern uint16_t W5500_TX(INTERFACE_TCP_SOCKET *Ptr,uint16_t Num,unsigned char *Ptr_Data);
extern uint16_t Qurey_W5500_TX(INTERFACE_TCP_SOCKET *Ptr);
extern void W5500_Service_BSP2(void);
extern void InitW5500_BSP(void);
extern uint16_t W5500_Read_1Byte(uint16_t Block,uint16_t Offset);
extern void W5500_Write_1Byte_Start(uint8_t Block,uint16_t Offset,uint8_t Data1);
extern void Socket_Work_Timeout_Check(void);

//W5500读写区域选择位定义
#define BL_REG_COMMON           0x00
#define BL_REG_SOCK             0x08
#define BL_RAM_TX               0x10
#define BL_RAM_RX               0x18

//W5500数据读写位定义
#define RWB_READ                0x00
#define RWB_WRITE               0x04

//W5500数据长度模式定义
#define OM_N_Byte               0x00
#define OM_1_Byte               0x01
#define OM_2_Byte               0x02
#define OM_4_Byte               0x03

//W5500通用寄存器区地址偏移定义
#define REG_MR                  0x0000
#define REG_GAR0                0x0001
#define REG_GAR1                0x0002
#define REG_GAR2                0x0003
#define REG_GAR3                0x0004
#define REG_SUBR0               0x0005
#define REG_SUBR1               0x0006
#define REG_SUBR2               0x0007
#define REG_SUBR3               0x0008
#define REG_SHAR0               0x0009
#define REG_SHAR1               0x000A
#define REG_SHAR2               0x000B
#define REG_SHAR3               0x000C
#define REG_SHAR4               0x000D
#define REG_SHAR5               0x000E
#define REG_SIPR0               0x000F
#define REG_SIPR1               0x0010
#define REG_SIPR2               0x0011
#define REG_SIPR3               0x0012
#define REG_INTLEVEL0           0x0013
#define REG_INTLEVEL1           0x0014
#define REG_IR                  0x0015
#define REG_IMR                 0x0016
#define REG_SIR                 0x0017
#define REG_SIMR                0x0018
#define REG_RTR0                0x0019
#define REG_RTR1                0x001A
#define REG_RCR                 0x001B
#define REG_PTIMER              0x001C
#define REG_PMAGIC              0x001D
#define REG_PHAR0               0x001E
#define REG_PHAR1               0x001F
#define REG_PHAR2               0x0020
#define REG_PHAR3               0x0021
#define REG_PHAR4               0x0022
#define REG_PHAR5               0x0023
#define REG_PSID0               0x0024
#define REG_PSID1               0x0025
#define REG_PMRU0               0x0026
#define REG_PMRU1               0x0027
#define REG_UIPR0               0x0028
#define REG_UIPR1               0x0029
#define REG_UIPR2               0x002A
#define REG_UIPR3               0x002B
#define REG_UPORT0              0x002C
#define REG_UPORT1              0x002D
#define REG_PHYCFGR             0x002E
#define REG_VERSIONR            0x0039

//W5500 Socket寄存器地址偏移定义
#define REG_SOCK_MR             0x0000
#define REG_SOCK_CR             0x0001
#define REG_SOCK_IR             0x0002
#define REG_SOCK_SR             0x0003
#define REG_SOCK_PORT0          0x0004
#define REG_SOCK_PORT1          0x0005
#define REG_SOCK_DHAR0          0x0006
#define REG_SOCK_DHAR1          0x0007
#define REG_SOCK_DHAR2          0x0008
#define REG_SOCK_DHAR3          0x0009
#define REG_SOCK_DHAR4          0x000A
#define REG_SOCK_DHAR5          0x000B
#define REG_SOCK_DIPR0          0x000C
#define REG_SOCK_DIPR1          0x000D
#define REG_SOCK_DIPR2          0x000E
#define REG_SOCK_DIPR3          0x000F
#define REG_SOCK_DPORT0         0x0010
#define REG_SOCK_DPORT1         0x0011
#define REG_SOCK_MSSR0          0x0012
#define REG_SOCK_MSSR1          0x0013
#define REG_SOCK_TOS            0x0015
#define REG_SOCK_TTL            0x0016
#define REG_SOCK_RXBUF_SIZE     0x001E
#define REG_SOCK_TXBUF_SIZE     0x001F
#define REG_SOCK_TX_FSR0        0x0020
#define REG_SOCK_TX_FSR1        0x0021
#define REG_SOCK_TX_RD0         0x0022
#define REG_SOCK_TX_RD1         0x0023
#define REG_SOCK_TX_WR0         0x0024
#define REG_SOCK_TX_WR1         0x0025
#define REG_SOCK_RX_RSR0        0x0026
#define REG_SOCK_RX_RSR1        0x0027
#define REG_SOCK_RX_RD0         0x0028
#define REG_SOCK_RX_RD1         0x0029
#define REG_SOCK_RX_WR0         0x002A
#define REG_SOCK_RX_WR1         0x002B
#define REG_SOCK_IMR            0x002C
#define REG_SOCK_FRAG0          0x002D
#define REG_SOCK_FRAG1          0x002E
#define REG_SOCK_KPALVTR        0x002F

//W5500 Socket Sn_MR 寄存器命令定义
#define MULTI_MFEN      0x80
#define BCASTB          0x40
#define ND_MC_MMB       0x20
#define UCASTB_MIP6B    0x10
#define MR_CLOSE        0x00
#define MR_TCP          0x01
#define MR_UDP          0x02
#define MR_MACRAW       0x04
//W5500 Socket命令寄存器命令定义
#define CMD_CR_OPEN             0x0001
#define CMD_CR_LISTEN           0x0002
#define CMD_CR_CONNECT          0x0004
#define CMD_CR_DISCON           0x0008
#define CMD_CR_CLOSE            0x0010
#define CMD_CR_SEND             0x0020
#define CMD_CR_SEND_MAC         0x0021
#define CMD_CR_SEND_KEEP        0x0022
#define CMD_CR_RECV             0x0040

//W5500 Socket中断寄存器比特位定义
#define IR_CON                  0x01
#define IR_DISCON               0x02
#define IR_RECV                 0x04
#define IR_TIMEOUT              0x08
#define IR_SEND_OK              0x10

//W5500 Socket状态寄存器状态定义
#define SR_SOCK_CLOSED          0x00
#define SR_SOCK_INIT            0x13
#define SR_SOCK_LISTEN          0x14
#define SR_SOCK_ESTABLISHED     0x17
#define SR_SOCK_CLOSE_WAIT      0x1C
#define SR_SOCK_UDP             0x22
#define SR_SOCK_MACRAW          0x42
#define SR_SOCK_SYNSENT         0x15
#define SR_SOCK_SYNRECV         0x16
#define SR_SOCK_FIN_WAIT        0x18
#define SR_SOCK_CLOSING         0x1A
#define SR_SOCK_TIME_WAIT       0x1B
#define SR_SOCK_LAST_ACK        0x1D

#endif
