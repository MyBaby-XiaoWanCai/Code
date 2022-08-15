/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Compen_Cur.h
// Created on: 2020-7-10
//     Author: LuoAnJiang
/**************************************************************************************/
#ifndef APP_INCLUDE_COMPEN_CUR_H_
#define APP_INCLUDE_COMPEN_CUR_H_



//�ṹ�嶨��
typedef struct struct_Compen_Cur
{
   Uint16 Cnt_Compen_Cur;

   Uint16 T_Env;
   Uint16 T_Cali;
    int16 K_Coeff;
    int16 Cur_Offset_CH;
    int16 Cur_Offset_DIS;
} struct_Compen_Cur;

//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern struct_Compen_Cur Compen_Cur;

//���ⲿ�ļ����ź�������
extern void Function_Compen_Cur(void);
extern void Read_Data_Compen_Cur(void);
#endif /* APP_INCLUDE_COMPEN_CUR_RESISTER_H_ */

//File end
