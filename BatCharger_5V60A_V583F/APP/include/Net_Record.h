/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_Record.h
// Created on: 2020-4-8
//     Author: LuoAnJiang
/**************************************************************************************/
#ifndef APP_INCLUDE_NET_RECORD_H_
#define APP_INCLUDE_NET_RECORD_H_




//�ṹ�嶨��


//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern struct_Fault_Code       Fault_Code;

//���ⲿ�ļ����ź�������
extern void sSetRecondConditionCommand(void);
extern void sInquireRecondDataCommand(void);
extern void sReplyRecondDataCommand(struct_Image *Ptr,Uint16 CH_TX);//��¼�����Ϸ�

#endif /* APP_INCLUDE_NET_RECORD_H_ */
