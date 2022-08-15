/*
*********************************************************************************************************
*
*	ģ������ : W5500����
*	�ļ����� : W5500_BSP.h
*	��    �� : V1.0
*	˵    �� :
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0    2020-03-12   ZYL      ��ʽ����
*********************************************************************************************************
*/

#ifndef _W5500_BSP_H_
#define _W5500_BSP_H_

#include "stdint.h"

#define w5500_EN                1

//��������
//Socket���ó�������
#define NUM_TCP                 2           //Socket������
#define NUM_TCP_TX_CH           3           //TCP��������ͨ������

#define LEN_TCP_RX_BUFF         1460        //TCP���ջ���������
#define LEN_TCP_TX_BUFF         1460        //TCP���ͻ���������

#define SERV_TCP_WORK           0           //����ʹ��TCP�ӿ�Socket0

//SOCKET0����ͨ��ʹ�ö���
#define CH_TX_RETURN            0           //����������Ӧ
#define CH_TX_UPDATA            1           //�ϱ�������
#define CH_TX_ALARM             2           //���Ϻ͸澯�����ϱ���

//�ṹ�嶨��
//TCP�׽��ֽӿڶ���
typedef struct TCP_SOCKET_RX
{
    unsigned char Data[LEN_TCP_RX_BUFF];    //�������ݻ�����
    unsigned char *pData;                   //debug ָ��Data���ݻ�������ʼ��ָ��
    uint16_t Status_Recive;                 //0-DSP���ջ����������Ѵ����꣬�����W5500���ջ�����DSP���ջ��濽������,
                                            //1-���ݿ�����ɣ��ȴ�DSP�������ݣ�ֹͣ��W5500���ջ�����DSP���ջ��濽������
    uint16_t Num_Recived;                   //��W5500���ջ�����DSP���ջ����ѿ�����������
    uint16_t Len_RX_Buff;                   //DSP�������ݻ���������

} TCP_SOCKET_RX;

typedef struct TCP_SOCKET_TX
{
    unsigned char Data[LEN_TCP_TX_BUFF];    //���ͻ�����
    unsigned char *pData;                   //ָ��Data���ݻ�������ʼ��ָ��
    uint16_t Req_TX;                        //0-��TCP��������1-��TCP��������
    uint16_t Num_To_TX;                     //��Ҫ���͵��ֽ���
    uint16_t Result_TX;                     //TCP���ͽ��
    uint16_t Len_TX_Buff;                   //DSP���ͻ���������

} TCP_SOCKET_TX;


typedef struct INTERFACE_TCP_SOCKET
{
    TCP_SOCKET_RX Data_RX;                  //���ջ��������� �ṹ
    TCP_SOCKET_TX Data_TX[NUM_TCP_TX_CH];   //���ͻ��������ݽṹ
    uint16_t Sn_IR;                         //��W5500�ж�ȡ����Socket�жϱ�־
    uint16_t Sn_SR;                         //��W5500�ж�ȡ����Socket����״̬��
    uint16_t Port_Source;                   //��Socket����ʹ�õı���TCP�˿ں�
    unsigned char IP_Dest[4];               //��Socket���ӵ�Զ��IP��ַ
    unsigned char MAC_Dest[6];              //��Socket���ӵ�Զ��MAC��ַ
    uint16_t Port_Dest;                     //��Socket���ӵ�Զ�˶˿ں�
    uint16_t Num_TX_CH;                     //��Socket����ͨ������
    uint16_t Index_Socket;                  //��ǰSocket��������,0~7
    uint16_t Cnt_Timeout;                   //Socket�ڲ�����ʱ������
    uint16_t Req_To_Listen;                 //����Socket���½���Listen״̬
    uint16_t Num_Socket_Timeout;            //Socket���ӳ�ʱʱ�䣬40mS/LSB
    uint16_t Cnt_Socket_Timeout;            //Socket���ӳ�ʱ������
    uint16_t Flag_SocketStatus;             //Socket����״̬
    uint16_t RX_Incomplete;

  struct INTERFACE_TCP_SOCKET *Ptr_Next;    //ָ����һ��TCP����ָ��
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


//W5500����ӿڶ���
typedef struct STATUS_W5500_SERVICE
{
    unsigned char GAR[4];                   //����IP��ַ
    unsigned char SUBR[4];                  //��������
    unsigned char SHAR[6];                  //����MAC��ַ
    unsigned char SIPR[4];                  //����IP��ַ
    uint16_t State;                         //���ڷ���״̬��״̬
    // uint16_t W5500_SIR;                     //��ȡ����W5500SIR�Ĵ���ֵ
    uint16_t Cnt_TX;                        //�����ֽ�����������
    uint16_t Cnt_RX;                        //�����ֽ�����������
    uint16_t Cnt_TX_CH;                     //��ǰָ���Socket�ķ���ͨ�����
    uint16_t Num_Last_RW;                   //��һ�ζ�ȡ�����ֽ���
    uint16_t Cnt_Overtime;                  //��ʱ�ü�����
    uint16_t Serv_Ready;                    //�����Ƿ����ʹ�ñ�־��0-�����ԣ�1-����
  DATA_SOCKET_REG Socket_Reg;               //��ǰָ���socket�ļĴ���
  INTERFACE_TCP_SOCKET *Ptr;                //ָ��ǰ���ڷ����Socket
} STATUS_W5500_SERVICE;

//W5500״̬��״̬����
#define W5500_STRUCT_INIT       0x00        //�ṹ�����ݳ�ʼ��
#define W5500_IDLE              0x01        //����״̬
#define W5500_RX_S1             0x02        //���ղ���1
#define W5500_RX_S2             0x03        //
#define W5500_RX_S3             0x05        //
#define W5500_RX_S4             0x06        //
#define W5500_RX_S5             0x07        //
#define W5500_RX_S6             0x08        //

#define W5500_TX_S1             0x0D        //���Ͳ���1
#define W5500_TX_S2             0x0E        //
#define W5500_TX_S3             0x0F        //
#define W5500_TX_S4             0x10        //
#define W5500_TX_S5             0x11        //
#define W5500_TX_S6             0x12        //
#define W5500_TX_S7             0x13        //

#define W5500_INIT_S1           0x14        //��ʼ��Socket��Listen����1
#define W5500_INIT_S2           0x15        //
#define W5500_INIT_S3           0x16        //
#define W5500_INIT_S4           0x17        //
#define W5500_INIT_S5           0x18        //
#define W5500_INIT_S6           0x19        //
#define W5500_INIT_S7           0x1A        //
#define W5500_INIT_S8           0x1B        //
//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern STATUS_W5500_SERVICE W5500_Service;
extern INTERFACE_TCP_SOCKET TCP_Socket[];

//���ⲿ�ļ����ź�������
extern uint16_t Qurey_W5500_RX(INTERFACE_TCP_SOCKET *Ptr);
extern void Clear_TCP_RX(INTERFACE_TCP_SOCKET *Ptr);
extern uint16_t W5500_TX(INTERFACE_TCP_SOCKET *Ptr,uint16_t Num,unsigned char *Ptr_Data);
extern uint16_t Qurey_W5500_TX(INTERFACE_TCP_SOCKET *Ptr);
extern void W5500_Service_BSP2(void);
extern void InitW5500_BSP(void);
extern uint16_t W5500_Read_1Byte(uint16_t Block,uint16_t Offset);
extern void W5500_Write_1Byte_Start(uint8_t Block,uint16_t Offset,uint8_t Data1);
extern void Socket_Work_Timeout_Check(void);

//W5500��д����ѡ��λ����
#define BL_REG_COMMON           0x00
#define BL_REG_SOCK             0x08
#define BL_RAM_TX               0x10
#define BL_RAM_RX               0x18

//W5500���ݶ�дλ����
#define RWB_READ                0x00
#define RWB_WRITE               0x04

//W5500���ݳ���ģʽ����
#define OM_N_Byte               0x00
#define OM_1_Byte               0x01
#define OM_2_Byte               0x02
#define OM_4_Byte               0x03

//W5500ͨ�üĴ�������ַƫ�ƶ���
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

//W5500 Socket�Ĵ�����ַƫ�ƶ���
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

//W5500 Socket Sn_MR �Ĵ��������
#define MULTI_MFEN      0x80
#define BCASTB          0x40
#define ND_MC_MMB       0x20
#define UCASTB_MIP6B    0x10
#define MR_CLOSE        0x00
#define MR_TCP          0x01
#define MR_UDP          0x02
#define MR_MACRAW       0x04
//W5500 Socket����Ĵ��������
#define CMD_CR_OPEN             0x0001
#define CMD_CR_LISTEN           0x0002
#define CMD_CR_CONNECT          0x0004
#define CMD_CR_DISCON           0x0008
#define CMD_CR_CLOSE            0x0010
#define CMD_CR_SEND             0x0020
#define CMD_CR_SEND_MAC         0x0021
#define CMD_CR_SEND_KEEP        0x0022
#define CMD_CR_RECV             0x0040

//W5500 Socket�жϼĴ�������λ����
#define IR_CON                  0x01
#define IR_DISCON               0x02
#define IR_RECV                 0x04
#define IR_TIMEOUT              0x08
#define IR_SEND_OK              0x10

//W5500 Socket״̬�Ĵ���״̬����
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
