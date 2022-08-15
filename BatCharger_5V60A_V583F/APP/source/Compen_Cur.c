/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Compen_Cur.c
// Created on: 2020-7-10
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义
struct_Compen_Cur Compen_Cur;

//函数声明


//函数定义
//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Function_Compen_Cur(void)
{
    if(++Compen_Cur.Cnt_Compen_Cur>=5)
    {
        int32 Temp;
        Compen_Cur.Cnt_Compen_Cur=0;

        #if VER_HARDWARE==C017 || VER_HARDWARE==C017_
        //模块输出电阻消耗电流补偿计算
        Temp=Data_Sum.Vol_Out/470-33;

        if(Temp<0)
        {
            Temp=0;
        }

        if(Temp>138)
        {
            Temp=138;
        }

        if((CH_B.EN_Run==FALSE)||(CH_B.EN_Run_Comm==FALSE))
        {
            CH_A.Cur_Scale_L.B_C_Res=Temp;
            CH_A.Cur_Scale_H.B_C_Res=Temp;
        }
        else
        {
            CH_A.Cur_Scale_L.B_C_Res=0;
            CH_A.Cur_Scale_H.B_C_Res=0;
        }

        if((CH_A.EN_Run==FALSE)||(CH_A.EN_Run_Comm==FALSE))
        {
            CH_B.Cur_Scale_L.B_C_Res=Temp;
            CH_B.Cur_Scale_H.B_C_Res=Temp;
        }
        else
        {
            CH_B.Cur_Scale_L.B_C_Res=0;
            CH_B.Cur_Scale_H.B_C_Res=0;
        }
        #else
        CH_A.Cur_Scale_L.B_C_Res=0;
        CH_A.Cur_Scale_H.B_C_Res=0;
        CH_B.Cur_Scale_L.B_C_Res=0;
        CH_B.Cur_Scale_H.B_C_Res=0;
        #endif

        #if VER_HARDWARE==C017 && VER_PRODUCT==PRODUCT_5V60A
        //环境温升补偿计算
        if(Interface_DCDC.Req_Aging==FALSE)
        {
            Temp=(int32)Compen_Cur.T_Env-Compen_Cur.T_Cali;
            Temp=-Temp*Compen_Cur.K_Coeff;
            CH_A.Cur_Scale_H.B_C_Env=Temp*(Data_Sum.Cur_CHA/100)/100000;
            if(CH_A.Cur_Scale_H.B_C_Env<-800)
            {
                CH_A.Cur_Scale_H.B_C_Env=-800;
            }

            if(CH_A.Cur_Scale_H.B_C_Env>800)
            {
                CH_A.Cur_Scale_H.B_C_Env=800;
            }
            CH_A.Cur_Scale_L.B_C_Env=CH_A.Cur_Scale_H.B_C_Env;

            CH_B.Cur_Scale_H.B_C_Env=Temp*(Data_Sum.Cur_CHB/100)/100000;
            if(CH_B.Cur_Scale_H.B_C_Env<-800)
            {
                CH_B.Cur_Scale_H.B_C_Env=-800;
            }

            if(CH_B.Cur_Scale_H.B_C_Env>800)
            {
                CH_B.Cur_Scale_H.B_C_Env=800;
            }
            CH_B.Cur_Scale_L.B_C_Env=CH_B.Cur_Scale_H.B_C_Env;
        }
        else
        {
            CH_A.Cur_Scale_H.B_C_Env=0;
            CH_A.Cur_Scale_L.B_C_Env=0;
            CH_B.Cur_Scale_H.B_C_Env=0;
            CH_B.Cur_Scale_L.B_C_Env=0;
        }

        #else
        CH_A.Cur_Scale_L.B_C_Env=0;
        CH_A.Cur_Scale_H.B_C_Env=0;
        CH_B.Cur_Scale_L.B_C_Env=0;
        CH_B.Cur_Scale_H.B_C_Env=0;
        #endif

        //电流温升补偿计算
        CH_A.Cur_Scale_L.B_C_Cur=0;
        CH_A.Cur_Scale_H.B_C_Cur=0;
        CH_B.Cur_Scale_L.B_C_Cur=0;
        CH_B.Cur_Scale_H.B_C_Cur=0;

        //补偿值叠加
        if((DCDC_Service.Now_Mode==DCDC_DCC)||(DCDC_Service.Now_Mode==DCDC_DCCtoDCV)||(DCDC_Service.Now_Mode==DCDC_DCV))
        {
            //放电模式电流闭环B_Now叠加负值增大灌入的电流，上传的数据值不变
            Temp    = -Cur_Scale_L_B_Check(CH_A.Cur_Scale_L.B_ROM_DIS)+CH_A.Cur_Scale_L.B_C_Res-CH_A.Cur_Scale_L.B_C_Env-CH_A.Cur_Scale_L.B_C_Cur;
            DINT;
            CH_A.Cur_Scale_L.B_Now      = Temp;
            EINT;

            Temp    = -Cur_Scale_H_B_Check(CH_A.Cur_Scale_H.B_ROM_DIS)+CH_A.Cur_Scale_H.B_C_Res-CH_A.Cur_Scale_H.B_C_Env-CH_A.Cur_Scale_H.B_C_Cur;
            DINT;
            CH_A.Cur_Scale_H.B_Now      = Temp;
            EINT;

            Temp    = -Cur_Scale_L_B_Check(CH_B.Cur_Scale_L.B_ROM_DIS)+CH_B.Cur_Scale_L.B_C_Res-CH_B.Cur_Scale_L.B_C_Env-CH_B.Cur_Scale_L.B_C_Cur;
            DINT;
            CH_B.Cur_Scale_L.B_Now      = Temp;
            EINT;

            Temp    = -Cur_Scale_H_B_Check(CH_B.Cur_Scale_H.B_ROM_DIS)+CH_B.Cur_Scale_H.B_C_Res-CH_B.Cur_Scale_H.B_C_Env-CH_B.Cur_Scale_H.B_C_Cur;
            DINT;
            CH_B.Cur_Scale_H.B_Now = Temp;
            EINT;
        }
        else
        {
            //充电模式电流环闭环B_Now叠加正值增大输出的电流，上传的电流数据值不变；电流环开环(老化CV模式)B_Now叠加正值输出的电流不受影响，上传的电流数据值减小
            Temp    = Cur_Scale_L_B_Check(CH_A.Cur_Scale_L.B_ROM_CH)+CH_A.Cur_Scale_L.B_C_Res+CH_A.Cur_Scale_L.B_C_Env+CH_A.Cur_Scale_L.B_C_Cur;
            DINT;
            CH_A.Cur_Scale_L.B_Now      = Temp;
            EINT;

            Temp    = Cur_Scale_H_B_Check(CH_A.Cur_Scale_H.B_ROM_CH)+CH_A.Cur_Scale_H.B_C_Res+CH_A.Cur_Scale_H.B_C_Env+CH_A.Cur_Scale_H.B_C_Cur;
            DINT;
            CH_A.Cur_Scale_H.B_Now      = Temp;
            EINT;

            Temp    = Cur_Scale_L_B_Check(CH_B.Cur_Scale_L.B_ROM_CH)+CH_B.Cur_Scale_L.B_C_Res+CH_B.Cur_Scale_L.B_C_Env+CH_B.Cur_Scale_L.B_C_Cur;
            DINT;
            CH_B.Cur_Scale_L.B_Now      = Temp;
            EINT;

            Temp    = Cur_Scale_H_B_Check(CH_B.Cur_Scale_H.B_ROM_CH)+CH_B.Cur_Scale_H.B_C_Res+CH_B.Cur_Scale_H.B_C_Env+CH_B.Cur_Scale_H.B_C_Cur;
            DINT;
            CH_B.Cur_Scale_H.B_Now      = Temp;
            EINT;
        }
    }
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Read_Data_Compen_Cur(void)
{
    Uint16 Temp1;
    int16  Temp2;
    Temp1=EEPROM_Read_Uint16(cEepromT_Cali);
    if(Temp1>=700)
    {
        Temp1=700;
    }
    Compen_Cur.T_Cali=Temp1;
    Compen_Cur.T_Env=Compen_Cur.T_Cali;

    Temp2=(int16)EEPROM_Read_Uint16(cEepromK_Coeff);
    if(Temp2>70)
    {
        Temp2=70;
    }
    else if(Temp2<-70)
    {
        Temp2=-70;
    }

    Compen_Cur.K_Coeff=Temp2;

    Compen_Cur.Cur_Offset_CH=0;

    Compen_Cur.Cur_Offset_DIS=0;

    Compen_Cur.Cnt_Compen_Cur=0;

}

//File end
