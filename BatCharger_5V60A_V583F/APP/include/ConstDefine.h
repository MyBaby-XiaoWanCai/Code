/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: ConstDefine.h
// Created on: 2020-3-18
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef APP_INCLUDE_CONSTDEFINE_H_
#define APP_INCLUDE_CONSTDEFINE_H_

#if   VER_PRODUCT == PRODUCT_5V20A
    #define CUR_MAX_CHANNEL         100000L         //��ͨ��Ӳ������������,10A
#elif VER_PRODUCT == PRODUCT_5V30A
    #define CUR_MAX_CHANNEL         150000L         //��ͨ��Ӳ������������,15A
#elif VER_PRODUCT == PRODUCT_5V60A
    #define CUR_MAX_CHANNEL         300000L         //��ͨ��Ӳ������������,30A
#elif VER_PRODUCT == PRODUCT_5V80A
    #define CUR_MAX_CHANNEL         400000L         //��ͨ��Ӳ������������,40A
#endif

#define VOL_MAX_SENSE               60000L          //Sense�����趨����ߵ�ѹ,6V

//CC��CCtoCVģʽ������PI����
#define PI_Cur_Charge_Kp            19  //205 //64*4/10  //����ǰ�迼�����ݳ˷���������Ŀ�����
#define PI_Cur_Charge_Ki            3  //38  //12*4/10  //����ǰ�迼�����ݳ˷���������Ŀ�����

//DCC��DCCtoDCVģʽ������PI����
#define PI_Cur_Discharge_Kp         40 //320 //100*4/10 //����ǰ�迼�����ݳ˷���������Ŀ�����
#define PI_Cur_Discharge_Ki         3 //32  //10*4/10  //����ǰ�迼�����ݳ˷���������Ŀ�����

//CCtoCVģʽ��ѹ��PI����
#define PI_Vol_Charge_Kp            0  //650/1024*50    //����ǰ�迼�����ݳ˷���������Ŀ�����
#define PI_Vol_Charge_Ki            1000 //70   //150/1024*50    //����ǰ�迼�����ݳ˷���������Ŀ�����

//DCCtoDCVģʽ��ѹ��PI����
#define PI_Vol_Discharge_Kp         0  //1050/1024*50  //����ǰ�迼�����ݳ˷���������Ŀ�����
#define PI_Vol_Discharge_Ki         1600 //1000 //210  //650/1024*50    //����ǰ�迼�����ݳ˷���������Ŀ�����

//CVģʽ��ѹ��PI����
#define PI_Vol_CV_Kp                104 //65*16/10  //����ǰ�迼�����ݳ˷���������Ŀ�����
#define PI_Vol_CV_Ki                24  //15*16/10  //����ǰ�迼�����ݳ˷���������Ŀ�����

//DCVģʽ��ѹ��PI����
#define PI_Vol_DCV_Kp               65
#define PI_Vol_DCV_Ki               15

//A��Bͨ���ߵ�����λУ׼����Ĭ��ֵ�����Ʒ�Χ����
#define CUR_H_K_DEFAULT             28200
#define CUR_H_K_MIN                 25200
#define CUR_H_K_MAX                 30200

#define CUR_H_B_DEFAULT             0
#define CUR_H_B_MIN                -500
#define CUR_H_B_MAX                 500

//A��Bͨ���͵�����λУ׼����Ĭ��ֵ�����Ʒ�Χ����
#define CUR_L_K_DEFAULT             5650
#define CUR_L_K_MIN                 4100
#define CUR_L_K_MAX                 7100

#define CUR_L_B_DEFAULT             0
#define CUR_L_B_MIN                -500
#define CUR_L_B_MAX                 500

//Out��ѹУ׼����Ĭ��ֵ�����Ʒ�Χ����
#define VOL_K_OUT_DEFAULT           6834
#define VOL_K_OUT_MIN               6300
#define VOL_K_OUT_MAX               7300

#define VOL_B_OUT_DEFAULT           2
#define VOL_B_OUT_MIN              -500
#define VOL_B_OUT_MAX               500

//Sense��ѹУ׼����Ĭ��ֵ�����Ʒ�Χ����
#define VOL_K_SENSE_DEFAULT         6834
#define VOL_K_SENSE_MIN             6300
#define VOL_K_SENSE_MAX             7300

#define VOL_B_SENSE_DEFAULT         2
#define VOL_B_SENSE_MIN            -500
#define VOL_B_SENSE_MAX             500

//ֱ������BUS��ѹ����Kֵ
#if VER_HARDWARE == C017D || VER_HARDWARE == C017D_
    #define VOL_K_BUS_DEFAULT       4500
#elif VER_HARDWARE==C017 || VER_HARDWARE==C017_ || VER_HARDWARE==C017B
    #define VOL_K_BUS_DEFAULT       4191
#endif

#endif /* APP_INCLUDE_CONSTDEFINE_H_ */
