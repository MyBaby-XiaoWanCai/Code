/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_Cali.h
// Created on: 2020-4-8
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef APP_INCLUDE_NET_CALI_H_
#define APP_INCLUDE_NET_CALI_H_




//�ṹ�嶨��


//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������


//���ⲿ�ļ����ź�������
extern void sAgingCalibrationSetCommand(void);
extern void sAgingCalibrationSetInqureCommand(void);


extern int32 Cur_Scale_L_K_Check(int32 Data);
extern int32 Cur_Scale_L_B_Check(int32 Data);
extern int32 Cur_Scale_H_K_Check(int32 Data);
extern int32 Cur_Scale_H_B_Check(int32 Data);
extern int32 Vol_Scale_K_Sense_Check(int32 Data);
extern int32 Vol_Scale_B_Sense_Check(int32 Data);
extern int32 Vol_Scale_K_Out_Check(int32 Data);
extern int32 Vol_Scale_B_Out_Check(int32 Data);

#endif /* APP_INCLUDE_NET_CALI_H_ */
