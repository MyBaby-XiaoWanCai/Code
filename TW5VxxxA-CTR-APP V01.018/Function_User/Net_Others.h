/*
*********************************************************************************************************
*
*	ģ������ : Net_Others
*	�ļ����� : Net_Others.h
*	��    �� : V1.0
*	˵    �� :
*	�޸ļ�¼ :
*	�汾��  ����         ����       ˵��
*	V1.0    2020-03-10   ZYL        ��ʽ����
*
*********************************************************************************************************
*/

#ifndef _Net_Others_H_
#define _Net_Others_H_

#include "stdint.h"

#define TIMEOUT_SOCKET_WORK        750   //������socket����������ʱ�Ͽ�����ʱ�䣬20S


//charger status
#define cOFF                        0
#define cON                         1
#define cComplete                   2
#define cNotComplete                3
#define cPowerFailNotComplete       4

//�ṹ�嶨��


//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern uint16_t Num_Cabinet;
extern uint16_t Addr_Module;

//���ⲿ�ļ����ź�������
extern void Socket_Work(void);
extern uint16_t sCRC16N(uint8_t *Buffer,uint16_t uwlen);
//extern void sTcpipNullCommand(void);
//extern void sLifeSingalReload(void);
//extern void sSysTimeSetCommand(void);
//extern void sSysVersionInquireCommand(void);
//extern void sSysCabNumInquireCommand(void);
//extern void sSetChargerStatusCommand(void);
//extern void sSetParallelModeCommand(void);
//extern void sFaultClearSetCommand(void);
//extern void sFaultInquireCommand(void);
//extern void sSetPreChargerCommand(void);
//extern void sInquirePowerOffEnCommand(void);
//extern void sSetPowerOffModeCommand(void);

void Socket_Receive(void);
void Socket_Sent(uint16_t len);

#endif

//File end
