/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_Global.c
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义


//函数声明
//函数声明
//----------------------全局保护设置功能响应指令-------------------//
void sSetGlobalProtctACKCommand(void)
{
    INT8U   ubCmdType1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcTemp0 = 0;
    INT8U   ubTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpipIndex=0;
    ubCmdType1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x7F;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetGlobalProtectACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCmdType1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSuccess;
    uwTcpipIndex = uwTcpipIndex+1;
    //校验位HL
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
    ubTcpCmdCrcTemp0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
    ubTcpCmdCrcTemp1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
}


void sSetGlobalProtctNACKCommand(void)
{
    INT8U   ubCmdType1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcTemp0 = 0;
    INT8U   ubTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpipIndex=0;
    ubCmdType1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x7F;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetGlobalProtectACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCmdType1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cFail;
    uwTcpipIndex = uwTcpipIndex+1;
    //校验位HL
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
    ubTcpCmdCrcTemp0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
    ubTcpCmdCrcTemp1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
}


//函数定义

//----------------------保护设置功能响应指令-------------------//
void sSetProtctACKCommand(void)
{
    INT8U   ubCmdType1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcTemp0 = 0;
    INT8U   ubTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpipIndex=0;
    ubCmdType1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x7F;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCmdType1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSuccess;
    uwTcpipIndex = uwTcpipIndex+1;
    //校验位HL
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
    ubTcpCmdCrcTemp0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
    ubTcpCmdCrcTemp1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
}


void sSetProtctNACKCommand(void)
{
    INT8U   ubCmdType1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcTemp0 = 0;
    INT8U   ubTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpipIndex=0;
    ubCmdType1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x7F;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCmdType1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cFail;
    uwTcpipIndex = uwTcpipIndex+1;
    //校验位HL
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
    ubTcpCmdCrcTemp0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
    ubTcpCmdCrcTemp1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
}

void sSetProtctCVCommand(void)
{
    INT32U  udwSetProtctCVTemp = 0;
    INT32U  udwSetProtctCVTemp0 = 0;
    INT32U  udwSetProtctCVTemp1 = 0;
    INT32U  udwSetProtctCVTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctCVTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctCVTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctCVTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        udwSetProtctCVTemp = (udwSetProtctCVTemp0<<16)+\
                                (udwSetProtctCVTemp1<<8)+\
                                    udwSetProtctCVTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.CH_Over_Vol_Bat.Protect_EN=TRUE;
            Protect_Global.CH_Over_Vol_Bat.Ref_Value=udwSetProtctCVTemp;//sSetProtctCV(udwSetProtctCVTemp);
        }
        else
        {
            Protect_Global.CH_Over_Vol_Bat.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}
void sSetProtctCCCommand(void)
{
    INT32S  dwSetProtctCCTemp = 0;
    INT32S  dwSetProtctCCTemp0 = 0;
    INT32S  dwSetProtctCCTemp1 = 0;
    INT32S  dwSetProtctCCTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        dwSetProtctCCTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        dwSetProtctCCTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        dwSetProtctCCTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        dwSetProtctCCTemp = (dwSetProtctCCTemp0<<16)+\
                                (dwSetProtctCCTemp1<<8)+\
                                    dwSetProtctCCTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.CH_Over_Cur.Protect_EN=TRUE;
            Protect_Global.CH_Over_Cur.Ref_Value=dwSetProtctCCTemp;//sSetProtctCC(dwSetProtctCCTemp);
        }
        else
        {
            Protect_Global.CH_Over_Cur.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}
void sSetProtctDCVCommand(void)
{
    INT32S  dwSetProtctDCVTemp = 0;
    INT32S  dwSetProtctDCVTemp0 = 0;
    INT32S  dwSetProtctDCVTemp1 = 0;
    INT32S  dwSetProtctDCVTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        dwSetProtctDCVTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        dwSetProtctDCVTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        dwSetProtctDCVTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        dwSetProtctDCVTemp = (dwSetProtctDCVTemp0<<16)+\
                                (dwSetProtctDCVTemp1<<8)+\
                                    dwSetProtctDCVTemp2;
        //增加负值设定
        if(dwSetProtctDCVTemp0>=128)
        {
            dwSetProtctDCVTemp =dwSetProtctDCVTemp|0xFF000000;
        }



        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.DIS_Under_Vol_Bat.Protect_EN=TRUE;
            Protect_Global.DIS_Under_Vol_Bat.Ref_Value=(int32)dwSetProtctDCVTemp;//sSetProtctDCV(dwSetProtctDCVTemp);
        }
        else
        {
            Protect_Global.DIS_Under_Vol_Bat.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}
void sSetProtctDCCCommand(void)
{
    INT32S  dwSetProtctDCCTemp = 0;
    INT32S  dwSetProtctDCCTemp0 = 0;
    INT32S  dwSetProtctDCCTemp1 = 0;
    INT32S  dwSetProtctDCCTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        dwSetProtctDCCTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        dwSetProtctDCCTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        dwSetProtctDCCTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        dwSetProtctDCCTemp = (dwSetProtctDCCTemp0<<16)+\
                                (dwSetProtctDCCTemp1<<8)+\
                                    dwSetProtctDCCTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.DIS_Over_Cur.Protect_EN=TRUE;
            Protect_Global.DIS_Over_Cur.Ref_Value=dwSetProtctDCCTemp;//sSetProtctDCC(dwSetProtctDCCTemp);
        }
        else
        {
            Protect_Global.DIS_Over_Cur.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}
void sSetProtctCAHCommand(void)
{
    INT32U  udwSetProtctCAHTemp = 0;
    INT32U  udwSetProtctCAHTemp0 = 0;
    INT32U  udwSetProtctCAHTemp1 = 0;
    INT32U  udwSetProtctCAHTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctCAHTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctCAHTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctCAHTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        udwSetProtctCAHTemp = (udwSetProtctCAHTemp0<<16)+\
                                (udwSetProtctCAHTemp1<<8)+\
                                    udwSetProtctCAHTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.CH_Over_Ah.Protect_EN=TRUE;
            Protect_Global.CH_Over_Ah.Ref_Value=udwSetProtctCAHTemp*10;//sSetProtctCAH(udwSetProtctCAHTemp);
        }
        else
        {
            Protect_Global.CH_Over_Ah.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}
void sSetProtctDAHCommand(void)
{
    INT32U  udwSetProtctDAHTemp = 0;
    INT32U  udwSetProtctDAHTemp0 = 0;
    INT32U  udwSetProtctDAHTemp1 = 0;
    INT32U  udwSetProtctDAHTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctDAHTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctDAHTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctDAHTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        udwSetProtctDAHTemp = (udwSetProtctDAHTemp0<<16)+\
                                (udwSetProtctDAHTemp1<<8)+\
                                    udwSetProtctDAHTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.DIS_Over_Ah.Protect_EN=TRUE;
            Protect_Global.DIS_Over_Ah.Ref_Value=udwSetProtctDAHTemp*10;//sSetProtctDAH(udwSetProtctDAHTemp);
        }
        else
        {
            Protect_Global.DIS_Over_Ah.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctACKCommand();
    }
}
void sSetProtctOPCCommand(void)
{
    INT32U  udwSetProtctOPCTemp = 0;
    INT32U  udwSetProtctOPCTemp0 = 0;
    INT32U  udwSetProtctOPCTemp1 = 0;
    INT32U  udwSetProtctOPCTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctOPCTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctOPCTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctOPCTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        udwSetProtctOPCTemp = (udwSetProtctOPCTemp0<<16)+\
                                    (udwSetProtctOPCTemp1<<8)+\
                                        udwSetProtctOPCTemp2;
      /*   Protect_Global.Out_Cur_Over.Ref_Value=udwSetProtctOPCTemp;//sSetProtctOPC(udwSetProtctOPCTemp);

        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Out_Cur_Over.Protect_EN=TRUE;
        }
        else
        {
            Protect_Global.Out_Cur_Over.Protect_EN=FALSE;
        }
      */
        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}
void sSetProtctTCommand(void)
{
    INT16U  uwSetProtctTTemp = 0;
    INT32U  udwSetProtctTTemp0 = 0;
    INT32U  udwSetProtctTTemp1 = 0;
    INT32U  udwSetProtctTTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctTTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctTTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        uwSetProtctTTemp = (udwSetProtctTTemp0<<16)+\
                                (udwSetProtctTTemp1<<8)+\
                                    udwSetProtctTTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Bat_Over_Temp.Protect_EN=TRUE;
            Protect_Global.Bat_Over_Temp.Ref_Value=uwSetProtctTTemp;//sSetProtctT(uwSetProtctTTemp);
        }
        else
        {
            Protect_Global.Bat_Over_Temp.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}

void sSetProtctTAlarmCommand(void)
{
    INT16U  uwSetProtctTTemp = 0;
    INT32U  udwSetProtctTTemp0 = 0;
    INT32U  udwSetProtctTTemp1 = 0;
    INT32U  udwSetProtctTTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctTTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctTTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        uwSetProtctTTemp = (udwSetProtctTTemp0<<16)+\
                                (udwSetProtctTTemp1<<8)+\
                                    udwSetProtctTTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Bat_Temp_Alarm.Protect_EN=TRUE;
            Protect_Global.Bat_Temp_Alarm.Ref_Value=uwSetProtctTTemp;//sSetProtctT(uwSetProtctTTemp);
        }
        else
        {
            Protect_Global.Bat_Temp_Alarm.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}

void sSetProtctTOverAlarmCommand()
{
    INT16U  uwSetProtctTTemp = 0;
    INT32U  udwSetProtctTTemp0 = 0;
    INT32U  udwSetProtctTTemp1 = 0;
    INT32U  udwSetProtctTTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctTTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctTTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        uwSetProtctTTemp = (udwSetProtctTTemp0<<16)+\
                           (udwSetProtctTTemp1<<8)+\
                            udwSetProtctTTemp2;

        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
                Protect_Global.Bat_Temp_Over_Alarm.Protect_EN=TRUE;
                Protect_Global.Bat_Temp_Over_Alarm.Ref_Value=uwSetProtctTTemp;//sSetProtctT(uwSetProtctTTemp);
        }
        else
        {
                Protect_Global.Bat_Temp_Over_Alarm.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }

}

void sSetProtctLowTCommand(void)
{
    INT32S  uwSetProtctTTemp = 0;
    INT32U  udwSetProtctTTemp0 = 0;
    INT32U  udwSetProtctTTemp1 = 0;
    INT32U  udwSetProtctTTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctTTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctTTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        uwSetProtctTTemp = (udwSetProtctTTemp0<<16)+\
                                    (udwSetProtctTTemp1<<8)+\
                                        udwSetProtctTTemp2;
        if(udwSetProtctTTemp0>=128)
        {
            uwSetProtctTTemp =uwSetProtctTTemp|0xFF000000;
        }



        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Bat_Under_Temp.Protect_EN=TRUE;
            Protect_Global.Bat_Under_Temp.Ref_Value=uwSetProtctTTemp;//sSetProtctLowT(uwSetProtctTTemp);
        }
        else
        {
            Protect_Global.Bat_Under_Temp.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctACKCommand();
    }
}
//-----------反接保护设定值----------------
void sSetProtectBatReverseCommand(void)
{
    INT32S  uwSetProtctTTemp = 0;
    INT32S  udwSetProtctTTemp0 = 0;
    INT32S  udwSetProtctTTemp1 = 0;
    INT32S  udwSetProtctTTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctTTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctTTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        uwSetProtctTTemp = (udwSetProtctTTemp0<<16)+\
                                (udwSetProtctTTemp1<<8)+\
                                    udwSetProtctTTemp2;
        //20191217
        if(udwSetProtctTTemp0>=128)
        {
            uwSetProtctTTemp =uwSetProtctTTemp|0xFF000000;
        }



        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
             Protect_Global.Bat_Reverse_Sense.Protect_EN=TRUE;
             Protect_Global.Bat_Reverse_Out.Protect_EN=TRUE;
             Protect_Global.Bat_Reverse_Sense.Ref_Value=uwSetProtctTTemp;//sSetProtctBatReverse(uwSetProtctTTemp);
             Protect_Global.Bat_Reverse_Out.Ref_Value=uwSetProtctTTemp;
        }
        else
        {
             Protect_Global.Bat_Reverse_Sense.Protect_EN=FALSE;
              Protect_Global.Bat_Reverse_Out.Protect_EN=FALSE;
        }

         sSetProtctACKCommand();

        //20191217
    }
    else
    {
        sSetProtctNACKCommand();
    }
}
//-----------------------接触阻抗设置---------------------------
void sSetProtectImpedanceCommand(void)
{
    INT32S  uwSetProtectTemp = 0;
    INT32U  uwSetProtectTemp0 = 0;
    INT32U  uwSetProtectTemp1 = 0;
    INT32U  uwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        uwSetProtectTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        uwSetProtectTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        uwSetProtectTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        uwSetProtectTemp = (uwSetProtectTemp0<<16)+\
                                (uwSetProtectTemp1<<8)+\
                                     uwSetProtectTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Res_Contact_Over.Protect_EN=TRUE;
            Protect_Global.Res_Contact_Over.Ref_Value=uwSetProtectTemp*100;//sSetProtectImpedance(uwSetProtectTemp);
        }
        else
        {
            Protect_Global.Res_Contact_Over.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}
//----------------------电流超差设置----------------------------
void sSetProtectOCECommand(void)
{
    INT32S  uwSetProtectTemp = 0;
    INT32U  uwSetProtectTemp0 = 0;
    INT32U  uwSetProtectTemp1 = 0;
    INT32U  uwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        uwSetProtectTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        uwSetProtectTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        uwSetProtectTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        uwSetProtectTemp = (uwSetProtectTemp0<<16)+\
                                (uwSetProtectTemp1<<8)+\
                                     uwSetProtectTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Cur_Diff_Over.Protect_EN=TRUE;
            Protect_Global.Cur_Diff_Over.Ref_Value_Diff=uwSetProtectTemp;//sSetProtectOCE(uwSetProtectTemp);
        }
        else
        {
            Protect_Global.Cur_Diff_Over.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}
//----------------------电压超差保护设置------------------------
void sSetProtectOVECommand(void)
{
    INT32S  uwSetProtectTemp = 0;
    INT32U  uwSetProtectTemp0 = 0;
    INT32U  uwSetProtectTemp1 = 0;
    INT32U  uwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        uwSetProtectTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        uwSetProtectTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        uwSetProtectTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        uwSetProtectTemp = (uwSetProtectTemp0<<16)+\
                                (uwSetProtectTemp1<<8)+\
                                     uwSetProtectTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Vol_Diff_Over.Protect_EN=TRUE;
            Protect_Global.Vol_Diff_Over.Ref_Value_Diff=uwSetProtectTemp;//sSetProtectOVE(uwSetProtectTemp);
        }
        else
        {
            Protect_Global.Vol_Diff_Over.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}
void sSetProtectLoopImpedanceCommand(void)
{
    INT32S  uwSetProtectTemp = 0;
    INT32U  uwSetProtectTemp0 = 0;
    INT32U  uwSetProtectTemp1 = 0;
    INT32U  uwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        uwSetProtectTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        uwSetProtectTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        uwSetProtectTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        uwSetProtectTemp = (uwSetProtectTemp0<<16)+\
                                    (uwSetProtectTemp1<<8)+\
                                         uwSetProtectTemp2;


        Protect_Global.Res_Loop_Over.Protect_EN=FALSE;
        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Res_Loop_Over.Protect_EN=FALSE;
            Protect_Global.Res_Loop_Over.Ref_Value=uwSetProtectTemp*100;//sSetProtectLoopImpedance(uwSetProtectTemp);
        }
        else
        {
            Protect_Global.Res_Loop_Over.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();

    }
    else
    {
        sSetProtctNACKCommand();
    }
}

extern INT16S LineRes;//0.1mΩ/LSB
void sSetProtectPowerVoltCommand(void)
{
    INT32S  uwSetProtectTemp = 0;
    INT32U  uwSetProtectTemp0 = 0;
    INT32U  uwSetProtectTemp1 = 0;
    INT32U  uwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        uwSetProtectTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        uwSetProtectTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        uwSetProtectTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        uwSetProtectTemp = (uwSetProtectTemp0<<16)+\
                                    (uwSetProtectTemp1<<8)+\
                                         uwSetProtectTemp2;
     //   Protect_Global.LineRes=uwSetProtectTemp;//20191210 sSetProtectPowerVolt(uwSetProtectTemp);

     /*   if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.CH_Over_Vol_Out.Protect_EN=TRUE;
            Protect_Global.DIS_Under_Vol_Out.Protect_EN=TRUE;
            Protect_Global.LineVol=uwSetProtectTemp;
        }
        else
        {
            Protect_Global.CH_Over_Vol_Out.Protect_EN=FALSE;
            Protect_Global.DIS_Under_Vol_Out.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
        */
    }
    else
    {
        sSetProtctNACKCommand();
    }
}

void Set_Cur_Limited_Set(void)
{
    INT32S  uwSetProtectTemp = 0;
    INT32U  uwSetProtectTemp0 = 0;
    INT32U  uwSetProtectTemp1 = 0;
    INT32U  uwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        uwSetProtectTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        uwSetProtectTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        uwSetProtectTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        uwSetProtectTemp = (uwSetProtectTemp0<<16)+\
                                    (uwSetProtectTemp1<<8)+\
                                         uwSetProtectTemp2;
        WorkStep_Service.Cur_Limited_Set=uwSetProtectTemp;
        EEPROM_Write_Uint16(cEepromCurLimitedSet,WorkStep_Service.Cur_Limited_Set/10000);
        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}
void sSetProtectTotalAHCommand(void)
{
    INT32U  udwSetProtctTotalAHTemp = 0;
    INT32U  udwSetProtctTotalAHTemp0 = 0;
    INT32U  udwSetProtctTotalAHTemp1 = 0;
    INT32U  udwSetProtctTotalAHTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTotalAHTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctTotalAHTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctTotalAHTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        udwSetProtctTotalAHTemp = (udwSetProtctTotalAHTemp0<<16)+\
                                (udwSetProtctTotalAHTemp1<<8)+\
                                    udwSetProtctTotalAHTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Total_Over_Ah.Protect_EN=TRUE;
            Protect_Global.Total_Over_Ah.Ref_Value =udwSetProtctTotalAHTemp*10;//sSetProtctDAH(udwSetProtctDAHTemp);
        }
        else
        {
            Protect_Global.Total_Over_Ah.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctACKCommand();
    }
}

void sSetContacCheck_Vol_H_Command(void)
{
    INT32S  dwTemp = 0;
    INT32S  dwTemp0 = 0;
    INT32S  dwTemp1 = 0;
    INT32S  dwTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        dwTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        dwTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        dwTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        dwTemp = (dwTemp0<<16)+(dwTemp1<<8)+dwTemp2;
        //增加负值设定
        if(dwTemp0>=128)
        {
            dwTemp =dwTemp|0xFF000000;
        }



        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_ContactCheck.Protect_EN=TRUE;
            Protect_ContactCheck.Limit_Vol_H_Start=dwTemp;
        }
        else
        {
            Protect_ContactCheck.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}

void sSetContacCheck_Vol_L_Command(void)
{
    INT32S  dwTemp = 0;
    INT32S  dwTemp0 = 0;
    INT32S  dwTemp1 = 0;
    INT32S  dwTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        dwTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        dwTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        dwTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        dwTemp = (dwTemp0<<16)+(dwTemp1<<8)+dwTemp2;
        //增加负值设定
        if(dwTemp0>=128)
        {
            dwTemp =dwTemp|0xFF000000;
        }



        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_ContactCheck.Protect_EN=TRUE;
            Protect_ContactCheck.Limit_Vol_L_Start=dwTemp;
        }
        else
        {
            Protect_ContactCheck.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}

void sSetContacCheck_Time_Command(void)
{
    INT32S  dwTemp = 0;
    INT32S  dwTemp0 = 0;
    INT32S  dwTemp1 = 0;
    INT32S  dwTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        dwTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        dwTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        dwTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        dwTemp = (dwTemp0<<16)+(dwTemp1<<8)+dwTemp2;



        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_ContactCheck.Protect_EN=TRUE;
            Protect_ContactCheck.Time=dwTemp;
        }
        else
        {
            Protect_ContactCheck.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}

void sSetContacCheck_Cur_Command(void)
{
    INT32S  dwTemp = 0;
    INT32S  dwTemp0 = 0;
    INT32S  dwTemp1 = 0;
    INT32S  dwTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        dwTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        dwTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        dwTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        dwTemp = (dwTemp0<<16)+(dwTemp1<<8)+dwTemp2;



        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_ContactCheck.Protect_EN=TRUE;
            Protect_ContactCheck.Cur=dwTemp;
        }
        else
        {
            Protect_ContactCheck.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}

void sSetContacCheck_Vol_Rise_Command(void)
{
    INT32S  dwTemp = 0;
    INT32S  dwTemp0 = 0;
    INT32S  dwTemp1 = 0;
    INT32S  dwTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        dwTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        dwTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        dwTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        dwTemp = (dwTemp0<<16)+(dwTemp1<<8)+dwTemp2;
        //增加负值设定
        if(dwTemp0>=128)
        {
            dwTemp =dwTemp|0xFF000000;
        }



        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_ContactCheck.Protect_EN=TRUE;
            Protect_ContactCheck.Vol_Rise=dwTemp;
        }
        else
        {
            Protect_ContactCheck.Protect_EN=FALSE;
        }

        sSetProtctACKCommand();
    }
    else
    {
        sSetProtctNACKCommand();
    }
}

//设置所有的全局保护
void sSetAllGlobalProtctCommand(void)
{
    INT32S  udwSetProtctTemp = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0]-3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0]-2];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)||(WorkStep_Service.Step_State==cPauseStatus))
    {
        if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
        {
            Protect_Global.Flag_Protect_Para=TRUE;

            //充电过压保护
            if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x7F)==cSetProtectCV)
            {
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
                {
                    Protect_Global.CH_Over_Vol_Bat.Protect_EN=TRUE;
                }
                else
                {
                    Protect_Global.CH_Over_Vol_Bat.Protect_EN=FALSE;
                }
                TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]=0;
                if(Protect_Global.CH_Over_Vol_Bat.Protect_EN==TRUE)
                {
                    Protect_Global.CH_Over_Vol_Bat.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],2);
                }
            }
            else
            {
                Protect_Global.Flag_Protect_Para=FALSE;
            }
            //充电过流保护
            if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6]&0x7F)==cSetProtectCC)
            {
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6]&0x80)==0)
                {
                    Protect_Global.CH_Over_Cur.Protect_EN=TRUE;
                }
                else
                {
                    Protect_Global.CH_Over_Cur.Protect_EN=FALSE;
                }
                TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6]=0;
                if( Protect_Global.CH_Over_Cur.Protect_EN==TRUE)
                {
                    Protect_Global.CH_Over_Cur.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],6);
                }
            }
            else
            {
                Protect_Global.Flag_Protect_Para=FALSE;
            }
            //放电欠压保护
            if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[10]&0x7F)==cSetProtectDCV)
            {
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[10]&0x80)==0)
                {
                    Protect_Global.DIS_Under_Vol_Bat.Protect_EN=TRUE;
                }
                else
                {
                    Protect_Global.DIS_Under_Vol_Bat.Protect_EN=FALSE;
                }
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[11]&0x80)!=0)
                {
                    TCP_Socket[SERV_TCP_WORK].Data_RX.Data[10]=0xFF;
                }
                else
                {
                    TCP_Socket[SERV_TCP_WORK].Data_RX.Data[10]=0;
                }
                if(Protect_Global.DIS_Under_Vol_Bat.Protect_EN==TRUE)
                {
                    Protect_Global.DIS_Under_Vol_Bat.Ref_Value=Char2int32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],10);
                }
            }
            else
            {
                Protect_Global.Flag_Protect_Para=FALSE;
            }
           //放电过流保护
            if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[14]&0x7F)==cSetProtectDCC)
            {
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[14]&0x80)==0)
                {
                    Protect_Global.DIS_Over_Cur.Protect_EN=TRUE;
                }
                else
                {
                    Protect_Global.DIS_Over_Cur.Protect_EN=FALSE;
                }
                TCP_Socket[SERV_TCP_WORK].Data_RX.Data[14]=0;
                if(Protect_Global.DIS_Over_Cur.Protect_EN==TRUE)
                {
                    Protect_Global.DIS_Over_Cur.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],14);
                }
            }
            else
            {
                Protect_Global.Flag_Protect_Para=FALSE;
            }
           //充电过容保护设置
            if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[18]&0x7F)==cSetProtectCAH)
            {
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[18]&0x80)==0)
                {
                    Protect_Global.CH_Over_Ah.Protect_EN=TRUE;
                }
                else
                {
                    Protect_Global.CH_Over_Ah.Protect_EN=FALSE;
                }
                TCP_Socket[SERV_TCP_WORK].Data_RX.Data[18]=0;
                if(Protect_Global.CH_Over_Ah.Protect_EN==TRUE)
                {
                    Protect_Global.CH_Over_Ah.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],18)*10;
                }
            }
            else
            {
                Protect_Global.Flag_Protect_Para=FALSE;
            }
            //直流输出限流保护设置
          /*  if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[22]&0x7F)==cSetProtectOPC)
            {
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[22]&0x80)==0)
                {
                    Protect_Global.Out_Cur_Over.Protect_EN=TRUE;
                }
                else
                {
                    Protect_Global.Out_Cur_Over.Protect_EN=FALSE;
                }
                TCP_Socket[SERV_TCP_WORK].Data_RX.Data[22]=0;
                Protect_Global.Out_Cur_Over.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],22);
            }
            else
            {
                Protect_Global.Flag_Protect_Para=FALSE;
            }*/
            //过温保护设置
            if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[22]&0x7F)==cSetProtectT)
            {
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[22]&0x80)==0)
                {
                    Protect_Global.Bat_Over_Temp.Protect_EN=TRUE;
                }
                else
                {
                    Protect_Global.Bat_Over_Temp.Protect_EN=FALSE;
                }
                TCP_Socket[SERV_TCP_WORK].Data_RX.Data[22]=0;
                if( Protect_Global.Bat_Over_Temp.Protect_EN==TRUE)
                {
                    Protect_Global.Bat_Over_Temp.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],22);
                }
            }
            else
            {
                Protect_Global.Flag_Protect_Para=FALSE;
            }
            //电池反接保护设置
            if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[26]&0x7F)==cSetProtectBatReverse)
            {
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[26]&0x80)==0)
                {
                      Protect_Global.Bat_Reverse_Sense.Protect_EN=TRUE;
                      Protect_Global.Bat_Reverse_Out.Protect_EN=TRUE;
                }
                else
                {
                       Protect_Global.Bat_Reverse_Sense.Protect_EN=FALSE;
                       Protect_Global.Bat_Reverse_Out.Protect_EN=FALSE;
                }
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[27]&0x80)!=0)
                {
                       TCP_Socket[SERV_TCP_WORK].Data_RX.Data[26]=0xFF;
                }
                else
                {
                       TCP_Socket[SERV_TCP_WORK].Data_RX.Data[26]=0;
                }
                udwSetProtctTemp=Char2int32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],26);
                if((Protect_Global.Bat_Reverse_Sense.Protect_EN==TRUE)&&(Protect_Global.Bat_Reverse_Out.Protect_EN==TRUE))
                {
                       Protect_Global.Bat_Reverse_Sense.Ref_Value=udwSetProtctTemp;//sSetProtctBatReverse(uwSetProtctTTemp);
                       Protect_Global.Bat_Reverse_Out.Ref_Value=udwSetProtctTemp;
                }

           }
           else
           {
                Protect_Global.Flag_Protect_Para=FALSE;
           }

            //电池电压波动保护
           if(((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[30]&0x7F)==cSetProtectBatFluc)&&((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[34]&0x7F)==cSetProtectBatMutat))
           {
                 if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[30]&0x80)==0)
                 {
                     Protect_Global.Vol_Fluct_Over.Protect_EN=TRUE;
                 }
                 else
                 {
                     Protect_Global.Vol_Fluct_Over.Protect_EN=FALSE;
                 }
                 TCP_Socket[SERV_TCP_WORK].Data_RX.Data[30]=0;
                 if(Protect_Global.Vol_Fluct_Over.Protect_EN==TRUE)
                     Protect_Global.Vol_Fluct_Over.Ref_Delta=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],30);
                 TCP_Socket[SERV_TCP_WORK].Data_RX.Data[34]=0;
                 if(Protect_Global.Vol_Fluct_Over.Protect_EN==TRUE)
                     Protect_Global.Vol_Fluct_Over.Num_Check=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],34);;//Protect_Global.Vol_Fluct_Over.Num_Delta=udwSetIntervalTimeTemp;//sSetBatSenseFlucProtctIntervalTime(udwSetIntervalTimeTemp);
            }
            else
            {
                  Protect_Global.Flag_Protect_Para=FALSE;
            }


           //电池电流波动保护
           if(((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[38]&0x7F)==cSetProtectOPCFluc)&&((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[42]&0x7F)==cSetProtectOPCMutat))
           {
                 if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[38]&0x80)==0)
                 {
                      Protect_Global.Cur_Fluct_Over.Protect_EN=TRUE;
                 }
                 else
                 {
                      Protect_Global.Cur_Fluct_Over.Protect_EN=FALSE;
                 }
                 TCP_Socket[SERV_TCP_WORK].Data_RX.Data[38]=0;
                 if( Protect_Global.Cur_Fluct_Over.Protect_EN==TRUE)
                     Protect_Global.Cur_Fluct_Over.Ref_Delta=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],38);
                 TCP_Socket[SERV_TCP_WORK].Data_RX.Data[42]=0;
                 if( Protect_Global.Cur_Fluct_Over.Protect_EN==TRUE)
                     Protect_Global.Cur_Fluct_Over.Num_Check=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],42);

           }
           else
           {
                 Protect_Global.Flag_Protect_Para=FALSE;
           }




           //电池温度下限保护设置
           if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[46]&0x7F)==cSetProtectLowT)
           {
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[46]&0x80)==0)
                {
                       Protect_Global.Bat_Under_Temp.Protect_EN=TRUE;
                }
                else
                {
                       Protect_Global.Bat_Under_Temp.Protect_EN=FALSE;
                }
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[47]&0x80)!=0)
                {
                       TCP_Socket[SERV_TCP_WORK].Data_RX.Data[46]=0xFF;
                }
                else
                {
                       TCP_Socket[SERV_TCP_WORK].Data_RX.Data[46]=0;
                }
                if(Protect_Global.Bat_Under_Temp.Protect_EN==TRUE)
                    Protect_Global.Bat_Under_Temp.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],46);//sSetProtctLowT(uwSetProtctTTemp);
           }
           else
           {
                Protect_Global.Flag_Protect_Para=FALSE;
           }

           //放电过容保护设置
           if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[50]&0x7F)==cSetProtectDAH)
           {
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[50]&0x80)==0)
                {
                     Protect_Global.DIS_Over_Ah.Protect_EN=TRUE;
                }
                else
                {
                     Protect_Global.DIS_Over_Ah.Protect_EN=FALSE;
                }
                TCP_Socket[SERV_TCP_WORK].Data_RX.Data[50]=0;
                if(Protect_Global.DIS_Over_Ah.Protect_EN==TRUE)
                    Protect_Global.DIS_Over_Ah.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],50)*10;
           }
           else
           {
                Protect_Global.Flag_Protect_Para=FALSE;
           }
         //接触阻抗保护设置
           if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[54]&0x7F)==cSetProtectImpedance)
           {
               if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[54]&0x80)==0)
               {
                   Protect_Global.Res_Contact_Over.Protect_EN=TRUE;
               }
               else
               {
                   Protect_Global.Res_Contact_Over.Protect_EN=FALSE;
               }
               TCP_Socket[SERV_TCP_WORK].Data_RX.Data[54]=0;
               if(Protect_Global.Res_Contact_Over.Protect_EN==TRUE)
                   Protect_Global.Res_Contact_Over.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],54);
           }
           else
           {
               Protect_Global.Flag_Protect_Para=FALSE;
           }
        //回路阻抗保护
       /*    if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[46]&0x7F)==cSetProtectImpedance)
           {
               if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[46]&0x80)==0)
               {
                    Protect_Global.Res_Loop_Over.Protect_EN=TRUE;
               }
               else
               {
                    Protect_Global.Res_Loop_Over.Protect_EN=FALSE;
               }
               TCP_Socket[SERV_TCP_WORK].Data_RX.Data[46]=0;
               Protect_Global.Res_Loop_Over.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],46);
           }
           else
           {
               Protect_Global.Flag_Protect_Para=FALSE;
           }
           */
        //电源输出过压设置
           /*
           if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[50]&0x7F)==cSetProtectImpedance)
           {
               if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[50]&0x80)==0)
               {
                   Protect_Global.CH_Over_Vol_Out.Protect_EN=TRUE;
                   Protect_Global.DIS_Under_Vol_Out.Protect_EN=TRUE;
               }
               else
               {
                   Protect_Global.CH_Over_Vol_Out.Protect_EN=FALSE;
                   Protect_Global.DIS_Under_Vol_Out.Protect_EN=FALSE;
               }
               TCP_Socket[SERV_TCP_WORK].Data_RX.Data[50]=0;
               Protect_Global.LineRes=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],50);
          }
          else
          {
               Protect_Global.Flag_Protect_Para=FALSE;
          }*/
      //总电荷容量上限保护
          if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[58]&0x7F)==cSetProtectTotalAH)
          {

              if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[58]&0x80)==0)
              {
                   Protect_Global.Total_Over_Ah.Protect_EN=TRUE;
              }
              else
              {
                   Protect_Global.Total_Over_Ah.Protect_EN=FALSE;
              }
              TCP_Socket[SERV_TCP_WORK].Data_RX.Data[58]=0;
              if(Protect_Global.Total_Over_Ah.Protect_EN==TRUE)
                  Protect_Global.Total_Over_Ah.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],58)*10;
          }
          else
          {
               Protect_Global.Flag_Protect_Para=FALSE;
          }

     //初态电压上限保护
          if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[62]&0x7F)==cSetContacCheck_Vol_H)
          {
              if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[62]&0x80)==0)
              {
                   Protect_ContactCheck.Protect_EN=TRUE;
              }
              else
              {
                   Protect_ContactCheck.Protect_EN=FALSE;
              }
              if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[63]&0x80)!=0)
              {
                   TCP_Socket[SERV_TCP_WORK].Data_RX.Data[62]=0xFF;
              }
              else
              {
                   TCP_Socket[SERV_TCP_WORK].Data_RX.Data[62]=0;
              }
              if(Protect_ContactCheck.Protect_EN==TRUE)
                  Protect_ContactCheck.Limit_Vol_H_Start=Char2int32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],62);
          }
          else
          {
              Protect_Global.Flag_Protect_Para=FALSE;
          }
     //初态电压下限保护
          if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[66]&0x7F)==cSetContacCheck_Vol_L)
          {

              if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[66]&0x80)==0)
              {
                  Protect_ContactCheck.Protect_EN=TRUE;
              }
              else
              {
                  Protect_ContactCheck.Protect_EN=FALSE;
              }
              if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[67]&0x80)!=0)
              {
                  TCP_Socket[SERV_TCP_WORK].Data_RX.Data[66]=0xFF;
              }
              else
              {
                  TCP_Socket[SERV_TCP_WORK].Data_RX.Data[66]=0;
              }
              if(Protect_ContactCheck.Protect_EN==TRUE)
                  Protect_ContactCheck.Limit_Vol_L_Start=Char2int32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],66);
          }
          else
          {
              Protect_Global.Flag_Protect_Para=FALSE;
          }
     //接触时间设定
          if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[70]&0x7F)==cSetContacCheck_Time)
          {
              if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[70]&0x80)==0)
              {
                   Protect_ContactCheck.Protect_EN=TRUE;
              }
              else
              {
                   Protect_ContactCheck.Protect_EN=FALSE;
              }
              TCP_Socket[SERV_TCP_WORK].Data_RX.Data[70]=0;
              if(Protect_ContactCheck.Protect_EN==TRUE)
                  Protect_ContactCheck.Time=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],70);
          }
          else
          {
              Protect_Global.Flag_Protect_Para=FALSE;
          }
    //接触测试电流设定
          if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[74]&0x7F)==cSetContacCheck_Cur)
          {
              if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[74]&0x80)==0)
              {
                   Protect_ContactCheck.Protect_EN=TRUE;
              }
              else
              {
                   Protect_ContactCheck.Protect_EN=FALSE;
              }
              TCP_Socket[SERV_TCP_WORK].Data_RX.Data[74]=0;
              if(Protect_ContactCheck.Protect_EN==TRUE)
                   Protect_ContactCheck.Cur=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],74);
          }
          else
          {
              Protect_Global.Flag_Protect_Para=FALSE;
          }
    //接触测试电压变化量设定
          if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[78]&0x7F)==cSetContacCheck_Vol_Rise)
          {
              if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[78]&0x80)==0)
              {
                  Protect_ContactCheck.Protect_EN=TRUE;
              }
              else
              {
                  Protect_ContactCheck.Protect_EN=FALSE;
              }
              if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[79]&0x80)!=0)
              {
                  TCP_Socket[SERV_TCP_WORK].Data_RX.Data[78]=0xFF;
              }
              else
              {
                  TCP_Socket[SERV_TCP_WORK].Data_RX.Data[78]=0;
              }
              if(Protect_ContactCheck.Protect_EN==TRUE)
                  Protect_ContactCheck.Vol_Rise=Char2int32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],78);
          }
          else
          {
              Protect_Global.Flag_Protect_Para=FALSE;
          }

        //电池过温预警值设置指令
         if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[82]&0x7F)==cSetBatTempAlarm)
         {
             if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[82]&0x80)==0)
             {
                 Protect_Global.Bat_Temp_Alarm.Protect_EN=TRUE;
             }
             else
             {
                 Protect_Global.Bat_Temp_Alarm.Protect_EN=FALSE;
             }
             TCP_Socket[SERV_TCP_WORK].Data_RX.Data[82]=0;
             if(Protect_Global.Bat_Temp_Alarm.Protect_EN==TRUE)
                 Protect_Global.Bat_Temp_Alarm.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],82);
         }
         else
         {
             Protect_Global.Flag_Protect_Para=FALSE;
         }
         //电池超高温设置指令
         if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[86]&0x7F)==cSetBatTempOverAlarm)
         {
             if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[86]&0x80)==0)
             {
                  Protect_Global.Bat_Temp_Over_Alarm.Protect_EN=TRUE;
             }
             else
             {
                  Protect_Global.Bat_Temp_Over_Alarm.Protect_EN=FALSE;
             }
             TCP_Socket[SERV_TCP_WORK].Data_RX.Data[86]=0;
             if(Protect_Global.Bat_Temp_Over_Alarm.Protect_EN==TRUE)
                 Protect_Global.Bat_Temp_Over_Alarm.Ref_Value=Char2Uint32(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],86);
         }
         else
         {
             Protect_Global.Flag_Protect_Para=FALSE;
         }


         if(Protect_Global.Flag_Protect_Para==TRUE)
         {
             sSetGlobalProtctACKCommand();
         }
         else
         {
             sSetGlobalProtctNACKCommand();
         }


       }
       else
       {
           TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]=0;
           sSetGlobalProtctNACKCommand();
       }

    }
    else
    {
          TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]=0;
          sSetGlobalProtctNACKCommand();
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}



//------------------------保护类----------------------//
//------------------------保护设置功能指令----------------------//
void sSetProtctCommand(void)
{
    unsigned char    ubCmdType = 0;
    ubCmdType = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x7F;

    if((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)||(WorkStep_Service.Step_State==cPauseStatus))
    {
        if(ubCmdType==cSetProtectCV)
        {
            sSetProtctCVCommand();
        }
        else if(ubCmdType==cSetProtectCC)
        {
            sSetProtctCCCommand();
        }
        else if(ubCmdType==cSetProtectDCV)
        {
            sSetProtctDCVCommand();
        }
        else if(ubCmdType==cSetProtectDCC)
        {
            sSetProtctDCCCommand();
        }
        else if(ubCmdType==cSetProtectCAH)
        {
            sSetProtctCAHCommand();
        }
        else if(ubCmdType==cSetProtectOPC)
        {
            sSetProtctOPCCommand();
        }
        else if(ubCmdType==cSetProtectT)
        {
            sSetProtctTCommand();
        }
        else if(ubCmdType==cSetProtectBatReverse)
        {
            sSetProtectBatReverseCommand();
        }
        else if(ubCmdType==cSetProtectLowT)
        {
            sSetProtctLowTCommand();
        }
        else if(ubCmdType==cSetProtectDAH)
        {
            sSetProtctDAHCommand();
        }
        else if(ubCmdType==cSetProtectImpedance)
        {
            sSetProtectImpedanceCommand();
        }
        else if(ubCmdType==cSetProtectOCE)//电流超差设置
        {
            sSetProtectOCECommand();
        }
        else if(ubCmdType==cSetProtectOVE)//电压超差设置
        {
            sSetProtectOVECommand();
        }
        else if(ubCmdType==cSetProtectLoopImpedance)
        {
            sSetProtectLoopImpedanceCommand();
        }
        else if(ubCmdType==cSetProtectPowerVolt)
        {
            sSetProtectPowerVoltCommand();
        }
        //else if(ubCmdType==cSet_Cur_Limited_Set)
        //{
        //    Set_Cur_Limited_Set();
        //}
        else if(ubCmdType==cSetProtectTotalAH)
        {
            sSetProtectTotalAHCommand();
        }
        else if(ubCmdType==cSetContacCheck_Vol_H)
        {
            sSetContacCheck_Vol_H_Command();
        }
        else if(ubCmdType==cSetContacCheck_Vol_L)
        {
            sSetContacCheck_Vol_L_Command();
        }
        else if(ubCmdType==cSetContacCheck_Time)
        {
            sSetContacCheck_Time_Command();
        }
        else if(ubCmdType==cSetContacCheck_Cur)
        {
            sSetContacCheck_Cur_Command();
        }
        else if(ubCmdType==cSetContacCheck_Vol_Rise)
        {
            sSetContacCheck_Vol_Rise_Command();
        }
        else if(ubCmdType==cSetBatTempAlarm)
        {
            sSetProtctTAlarmCommand();
        }
        else if(ubCmdType==cSetBatTempOverAlarm)
        {
            sSetProtctTOverAlarmCommand();
        }
    }
    else
    {
        sSetProtctNACKCommand();
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}

/*void sReplyProtctCVCommand(void)
{
    INT32U  udwSetProtctCVTemp = 0;
    INT8U   ubSetProtctCVTemp0 = 0;
    INT8U   ubSetProtctCVTemp1 = 0;
    INT8U   ubSetProtctCVTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctCVTemp = Protect_Global.CH_Over_Vol_Bat.Ref_Value;
        ubSetProtctCVTemp0 = (INT8U)((udwSetProtctCVTemp&0xFF0000)>>16);
        ubSetProtctCVTemp1 = (INT8U)((udwSetProtctCVTemp&0x00FF00)>>8);
        ubSetProtctCVTemp2 = (INT8U)(udwSetProtctCVTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;

        if(Protect_Global.CH_Over_Vol_Bat.Protect_EN==TRUE)
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectCV;
        }
        else
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectCV+128;
        }

        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctCVTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctCVTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctCVTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
void sReplyProtctCCCommand(void)
{
    INT32S  dwSetProtctCCTemp = 0;
    INT8U   ubSetProtctCCTemp0 = 0;
    INT8U   ubSetProtctCCTemp1 = 0;
    INT8U   ubSetProtctCCTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        dwSetProtctCCTemp = Protect_Global.CH_Over_Cur.Ref_Value;
        ubSetProtctCCTemp0=(INT8U)((dwSetProtctCCTemp&0xFF0000)>>16);
        ubSetProtctCCTemp1=(INT8U)((dwSetProtctCCTemp&0x00FF00)>>8);;
        ubSetProtctCCTemp2=(INT8U)(dwSetProtctCCTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;

        if(Protect_Global.CH_Over_Vol_Bat.Protect_EN==TRUE)
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectCV;
        }
        else
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectCV+128;
        }

        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectCC;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctCCTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctCCTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctCCTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
void sReplyProtctDCVCommand(void)
{
    INT32S  dwSetProtctDCVTemp = 0;
    INT8U   ubSetProtctDCVTemp0 = 0;
    INT8U   ubSetProtctDCVTemp1 = 0;
    INT8U   ubSetProtctDCVTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        dwSetProtctDCVTemp = Protect_Global.DIS_Under_Vol_Bat.Ref_Value;
        ubSetProtctDCVTemp0=(INT8U)((dwSetProtctDCVTemp&0xFF0000)>>16);
        ubSetProtctDCVTemp1=(INT8U)((dwSetProtctDCVTemp&0x00FF00)>>8);;
        ubSetProtctDCVTemp2=(INT8U)(dwSetProtctDCVTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectDCV;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctDCVTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctDCVTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctDCVTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
void sReplyProtctDCCCommand(void)
{
    INT32S  dwSetProtctDCCTemp = 0;
    INT8U   ubSetProtctDCCTemp0 = 0;
    INT8U   ubSetProtctDCCTemp1 = 0;
    INT8U   ubSetProtctDCCTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        dwSetProtctDCCTemp = Protect_Global.DIS_Over_Cur.Ref_Value;//sGetProtctDCC();
        ubSetProtctDCCTemp0=(INT8U)((dwSetProtctDCCTemp&0xFF0000)>>16);
        ubSetProtctDCCTemp1=(INT8U)((dwSetProtctDCCTemp&0x00FF00)>>8);;
        ubSetProtctDCCTemp2=(INT8U)(dwSetProtctDCCTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectDCC;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctDCCTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctDCCTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctDCCTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
void sReplyProtctCAHCommand(void)
{
    INT32U  udwSetProtctCAHTemp = 0;
    INT8U   ubSetProtctCAHTemp0 = 0;
    INT8U   ubSetProtctCAHTemp1 = 0;
    INT8U   ubSetProtctCAHTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctCAHTemp = Protect_Global.CH_Over_Ah.Ref_Value/10;//sGetProtctCAH();
        ubSetProtctCAHTemp0=(INT8U)((udwSetProtctCAHTemp&0xFF0000)>>16);
        ubSetProtctCAHTemp1=(INT8U)((udwSetProtctCAHTemp&0x00FF00)>>8);;
        ubSetProtctCAHTemp2=(INT8U)(udwSetProtctCAHTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectCAH;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctCAHTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctCAHTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctCAHTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
void sReplyProtctOPCCommand(void)
{
    INT32U  udwSetProtctOPCTemp = 0;
    INT8U   ubSetProtctOPCTemp0 = 0;
    INT8U   ubSetProtctOPCTemp1 = 0;
    INT8U   ubSetProtctOPCTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctOPCTemp = Protect_Global.Out_Cur_Over.Ref_Value;//sGetProtctOPC();
        ubSetProtctOPCTemp0=(INT8U)((udwSetProtctOPCTemp&0xFF0000)>>16);
        ubSetProtctOPCTemp1=(INT8U)((udwSetProtctOPCTemp&0x00FF00)>>8);;
        ubSetProtctOPCTemp2=(INT8U)(udwSetProtctOPCTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectOPC;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctOPCTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctOPCTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctOPCTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
void sReplyProtctTCommand(void)
{
    INT32U  udwSetProtctTTemp = 0;
    INT8U   ubSetProtctTTemp0 = 0;
    INT8U   ubSetProtctTTemp1 = 0;
    INT8U   ubSetProtctTTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTTemp = Protect_Global.Bat_Over_Temp.Ref_Value;//sGetProtctT();
        ubSetProtctTTemp0=(INT8U)((udwSetProtctTTemp&0xFF0000)>>16);
        ubSetProtctTTemp1=(INT8U)((udwSetProtctTTemp&0x00FF00)>>8);
        ubSetProtctTTemp2=(INT8U)(udwSetProtctTTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectT;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
void sReplyProtctLowTCommand(void)
{
    INT32U  udwSetProtctTTemp = 0;
    INT8U   ubSetProtctTTemp0 = 0;
    INT8U   ubSetProtctTTemp1 = 0;
    INT8U   ubSetProtctTTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTTemp = Protect_Global.Bat_Under_Temp.Ref_Value;//sGetProtctLowT();
        ubSetProtctTTemp0=(INT8U)((udwSetProtctTTemp&0xFF0000)>>16);
        ubSetProtctTTemp1=(INT8U)((udwSetProtctTTemp&0x00FF00)>>8);
        ubSetProtctTTemp2=(INT8U)(udwSetProtctTTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectLowT;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
void sReplyProtctDAHCommand(void)
{
    INT32U  udwSetProtctDAHTemp = 0;
    INT8U   ubSetProtctDAHTemp0 = 0;
    INT8U   ubSetProtctDAHTemp1 = 0;
    INT8U   ubSetProtctDAHTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctDAHTemp = Protect_Global.DIS_Over_Ah.Ref_Value/10;//sGetProtctDAH();
        ubSetProtctDAHTemp0=(INT8U)((udwSetProtctDAHTemp&0xFF0000)>>16);
        ubSetProtctDAHTemp1=(INT8U)((udwSetProtctDAHTemp&0x00FF00)>>8);;
        ubSetProtctDAHTemp2=(INT8U)(udwSetProtctDAHTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectDAH;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctDAHTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctDAHTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctDAHTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
void sReplyProtectBatReVerse(void)
{
    INT32S  udwSetProtctTemp = 0;
    INT8U   ubSetProtctTTemp0 = 0;
    INT8U   ubSetProtctTTemp1 = 0;
    INT8U   ubSetProtctTTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTemp = Protect_Global.Bat_Reverse_Sense.Ref_Value;//sGetProtctBatReverse();
        ubSetProtctTTemp0=(INT8U)((udwSetProtctTemp&0xFF0000)>>16);
        ubSetProtctTTemp1=(INT8U)((udwSetProtctTemp&0x00FF00)>>8);
        ubSetProtctTTemp2=(INT8U)(udwSetProtctTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectBatReverse;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
//------------------接触阻抗查询-------------------------
void sReplyProtectImpedance(void)
{
    INT32S  udwSetProtectTemp = 0;
    INT8U   udwSetProtectTemp0 = 0;
    INT8U   udwSetProtectTemp1 = 0;
    INT8U   udwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtectTemp = Protect_Global.Res_Contact_Over.Ref_Value/100;//sGetProtectImpedance();//阻抗上返
        udwSetProtectTemp0=(INT8U)((udwSetProtectTemp&0xFF0000)>>16);
        udwSetProtectTemp1=(INT8U)((udwSetProtectTemp&0x00FF00)>>8);
        udwSetProtectTemp2=(INT8U)(udwSetProtectTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectImpedance;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
//---------------------电流超差保护值查询-----------------------
void sReplyProtectOCE(void)
{
    INT32S  udwSetProtectTemp = 0;
    INT8U   udwSetProtectTemp0 = 0;
    INT8U   udwSetProtectTemp1 = 0;
    INT8U   udwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtectTemp = Protect_Global.Cur_Diff_Over.Ref_Value_Diff;//sGetProtectOCE();//阻抗上返
        udwSetProtectTemp0=(INT8U)((udwSetProtectTemp&0xFF0000)>>16);
        udwSetProtectTemp1=(INT8U)((udwSetProtectTemp&0x00FF00)>>8);
        udwSetProtectTemp2=(INT8U)(udwSetProtectTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectOCE;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
//------------------电压超差设置查询--------------------
void sReplyProtectOVE(void)
{
    INT32S  udwSetProtectTemp = 0;
    INT8U   udwSetProtectTemp0 = 0;
    INT8U   udwSetProtectTemp1 = 0;
    INT8U   udwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtectTemp = Protect_Global.Vol_Diff_Over.Ref_Value_Diff;//sGetProtectOVE();
        udwSetProtectTemp0=(INT8U)((udwSetProtectTemp&0xFF0000)>>16);
        udwSetProtectTemp1=(INT8U)((udwSetProtectTemp&0x00FF00)>>8);
        udwSetProtectTemp2=(INT8U)(udwSetProtectTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectOVE;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
void sReplyProtectLoopImpedance(void)
{
    INT32S  udwSetProtectTemp = 0;
    INT8U   udwSetProtectTemp0 = 0;
    INT8U   udwSetProtectTemp1 = 0;
    INT8U   udwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtectTemp = Protect_Global.Res_Loop_Over.Ref_Value/100;//sGetProtectLoopImpedance();//阻抗上返
        udwSetProtectTemp0=(INT8U)((udwSetProtectTemp&0xFF0000)>>16);
        udwSetProtectTemp1=(INT8U)((udwSetProtectTemp&0x00FF00)>>8);
        udwSetProtectTemp2=(INT8U)(udwSetProtectTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectLoopImpedance;//cSetProtectImpedance;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}

void sReplyProtectPowerVolt(void)
{
    INT32S  udwSetProtectTemp = 0;
    INT8U   udwSetProtectTemp0 = 0;
    INT8U   udwSetProtectTemp1 = 0;
    INT8U   udwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {

        udwSetProtectTemp = Protect_Global.LineRes;//20191210 sGetProtectPowerVolt();
        udwSetProtectTemp0=(INT8U)((udwSetProtectTemp&0xFF0000)>>16);
        udwSetProtectTemp1=(INT8U)((udwSetProtectTemp&0x00FF00)>>8);
        udwSetProtectTemp2=(INT8U)(udwSetProtectTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectPowerVolt;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}

void sReply_Cur_Limited_Set(void)
{
    INT32S  udwSetProtectTemp = 0;
    INT8U   udwSetProtectTemp0 = 0;
    INT8U   udwSetProtectTemp1 = 0;
    INT8U   udwSetProtectTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {

        udwSetProtectTemp = WorkStep_Service.Cur_Limited_Set;
        udwSetProtectTemp0=(INT8U)((udwSetProtectTemp&0xFF0000)>>16);
        udwSetProtectTemp1=(INT8U)((udwSetProtectTemp&0x00FF00)>>8);
        udwSetProtectTemp2=(INT8U)(udwSetProtectTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSet_Cur_Limited_Set;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = udwSetProtectTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}
void sReplyProtctTotalAHCommand(void)
{
    INT32U  udwSetProtctTotalAHTemp = 0;
    INT8U   ubSetProtctTotalAHTemp0 = 0;
    INT8U   ubSetProtctTotalAHTemp1 = 0;
    INT8U   ubSetProtctTotalAHTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTotalAHTemp =Protect_Global.Total_Over_Ah.Ref_Value /10;//sGetProtctTotalAH();
        ubSetProtctTotalAHTemp0=(INT8U)((udwSetProtctTotalAHTemp&0xFF0000)>>16);
        ubSetProtctTotalAHTemp1=(INT8U)((udwSetProtctTotalAHTemp&0x00FF00)>>8);
        ubSetProtctTotalAHTemp2=(INT8U)(udwSetProtctTotalAHTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectTotalAH;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTotalAHTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTotalAHTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTotalAHTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}*/

void sReplyProtectGlobal(INT32S Data,INT8U Code,Uint16 Protect_EN)
{
    INT32S  udwSetProtctTemp = 0;
    INT8U   ubSetProtctTTemp0 = 0;
    INT8U   ubSetProtctTTemp1 = 0;
    INT8U   ubSetProtctTTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTemp = Data;//Protect_ContactCheck.Limit_Vol_H_Start;
        ubSetProtctTTemp0=(INT8U)((udwSetProtctTemp&0xFF0000)>>16);
        ubSetProtctTTemp1=(INT8U)((udwSetProtctTemp&0x00FF00)>>8);
        ubSetProtctTTemp2=(INT8U)(udwSetProtctTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;

        if(Protect_EN==TRUE)
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = Code;
        }
        else
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = Code+128;
        }

        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}


/*void sReplyProtectContacCheck(INT32S Data,INT8U Code)
{
    INT32S  udwSetProtctTemp = 0;
    INT8U   ubSetProtctTTemp0 = 0;
    INT8U   ubSetProtctTTemp1 = 0;
    INT8U   ubSetProtctTTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctTemp = Data;//Protect_ContactCheck.Limit_Vol_H_Start;
        ubSetProtctTTemp0=(INT8U)((udwSetProtctTemp&0xFF0000)>>16);
        ubSetProtctTTemp1=(INT8U)((udwSetProtctTemp&0x00FF00)>>8);
        ubSetProtctTTemp2=(INT8U)(udwSetProtctTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectReply;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = Code;//cSetContacCheck_Vol_H;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctTTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        //校验位HL
        uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
        ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
        ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
    }
}*/


//-----------------保护功能设置查询指令------------------//
void sInquireSetProtctCommand(void)
{
    INT8U   ubCmdType = 0;
    ubCmdType = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    Fault_Code.Err_Global_Bak=0;
    Fault_Code.Err_Step_Bak=0;
    Fault_Code.Alarm_Bak=0;
    if(ubCmdType==cSetProtectCV)
    {
        //sReplyProtctCVCommand();
        sReplyProtectGlobal(Protect_Global.CH_Over_Vol_Bat.Ref_Value,cSetProtectCV,Protect_Global.CH_Over_Vol_Bat.Protect_EN);
    }
    else if(ubCmdType==cSetProtectCC)
    {
        //sReplyProtctCCCommand();
        sReplyProtectGlobal(Protect_Global.CH_Over_Cur.Ref_Value,cSetProtectCC,Protect_Global.CH_Over_Cur.Protect_EN);
    }
    else if(ubCmdType==cSetProtectDCV)
    {
        //sReplyProtctDCVCommand();
        sReplyProtectGlobal(Protect_Global.DIS_Under_Vol_Bat.Ref_Value,cSetProtectDCV,Protect_Global.DIS_Under_Vol_Bat.Protect_EN);
    }
    else if(ubCmdType==cSetProtectDCC)
    {
        //sReplyProtctDCCCommand();
        sReplyProtectGlobal(Protect_Global.DIS_Over_Cur.Ref_Value,cSetProtectDCC,Protect_Global.DIS_Over_Cur.Protect_EN);
    }
    else if(ubCmdType==cSetProtectCAH)
    {
        //sReplyProtctCAHCommand();
        sReplyProtectGlobal(Protect_Global.CH_Over_Ah.Ref_Value/10,cSetProtectCAH,Protect_Global.CH_Over_Ah.Protect_EN);
    }
    else if(ubCmdType==cSetProtectOPC)
    {
        //sReplyProtctOPCCommand();
        sReplyProtectGlobal(Protect_Global.Out_Cur_Over.Ref_Value,cSetProtectOPC,Protect_Global.Out_Cur_Over.Protect_EN);
    }
    else if(ubCmdType==cSetProtectT)
    {
        //sReplyProtctTCommand();
        sReplyProtectGlobal(Protect_Global.Bat_Over_Temp.Ref_Value,cSetProtectT,Protect_Global.Bat_Over_Temp.Protect_EN);
    }
    else if(ubCmdType==cSetProtectLowT)
    {
        //sReplyProtctLowTCommand();
        sReplyProtectGlobal(Protect_Global.Bat_Under_Temp.Ref_Value,cSetProtectLowT,Protect_Global.Bat_Under_Temp.Protect_EN);
    }
    else if(ubCmdType==cSetProtectDAH)
    {
        //sReplyProtctDAHCommand();
        sReplyProtectGlobal(Protect_Global.DIS_Over_Ah.Ref_Value/10,cSetProtectDAH,Protect_Global.DIS_Over_Ah.Protect_EN);
    }
    else if(ubCmdType==cSetProtectBatReverse)
    {
        //sReplyProtectBatReVerse();
        sReplyProtectGlobal(Protect_Global.Bat_Reverse_Sense.Ref_Value,cSetProtectBatReverse,Protect_Global.Bat_Reverse_Sense.Protect_EN);
    }
    else if(ubCmdType==cSetProtectImpedance)
    {
        //sReplyProtectImpedance();
        sReplyProtectGlobal(Protect_Global.Res_Contact_Over.Ref_Value/100,cSetProtectImpedance,Protect_Global.Res_Contact_Over.Protect_EN);
    }
    else if(ubCmdType==cSetProtectOCE)
    {
        //sReplyProtectOCE();
        sReplyProtectGlobal(Protect_Global.Cur_Diff_Over.Ref_Value_Diff,cSetProtectOCE,Protect_Global.Cur_Diff_Over.Protect_EN);
    }
    else if(ubCmdType==cSetProtectOVE)
    {
        //sReplyProtectOVE();
        sReplyProtectGlobal(Protect_Global.Vol_Diff_Over.Ref_Value_Diff,cSetProtectOVE,Protect_Global.Vol_Diff_Over.Protect_EN);
    }
    else if(ubCmdType==cSetProtectLoopImpedance)
    {
        //sReplyProtectLoopImpedance();
        sReplyProtectGlobal(Protect_Global.Res_Loop_Over.Ref_Value/100,cSetProtectLoopImpedance,Protect_Global.Res_Loop_Over.Protect_EN);
    }
    else if(ubCmdType==cSetProtectPowerVolt)
    {
        //sReplyProtectPowerVolt();
        sReplyProtectGlobal(Protect_Global.LineRes,cSetProtectPowerVolt,Protect_Global.CH_Over_Vol_Out.Protect_EN);//debug?
    }
    //else if(ubCmdType==cSet_Cur_Limited_Set)
    //{
    //    sReply_Cur_Limited_Set();
    //}
    else if(ubCmdType==cSetProtectTotalAH)
    {
         //sReplyProtctTotalAHCommand();
         sReplyProtectGlobal(Protect_Global.Total_Over_Ah.Ref_Value /10,cSetProtectTotalAH,Protect_Global.Total_Over_Ah.Protect_EN);
    }
    else if(ubCmdType==cSetContacCheck_Vol_H)
    {
        sReplyProtectGlobal(Protect_ContactCheck.Limit_Vol_H_Start,cSetContacCheck_Vol_H,Protect_ContactCheck.Protect_EN);
    }
    else if(ubCmdType==cSetContacCheck_Vol_L)
    {
        sReplyProtectGlobal(Protect_ContactCheck.Limit_Vol_L_Start,cSetContacCheck_Vol_L,Protect_ContactCheck.Protect_EN);
    }
    else if(ubCmdType==cSetContacCheck_Time)
    {
        sReplyProtectGlobal(Protect_ContactCheck.Time,cSetContacCheck_Time,Protect_ContactCheck.Protect_EN);
    }
    else if(ubCmdType==cSetContacCheck_Cur)
    {
        sReplyProtectGlobal(Protect_ContactCheck.Cur,cSetContacCheck_Cur,Protect_ContactCheck.Protect_EN);
    }
    else if(ubCmdType==cSetContacCheck_Vol_Rise)
    {
        sReplyProtectGlobal(Protect_ContactCheck.Vol_Rise,cSetContacCheck_Vol_Rise,Protect_ContactCheck.Protect_EN);
    }
    else if(ubCmdType==cSetBatTempAlarm)
    {
        //sReplyProtectContacCheck(Protect_Global.Bat_Temp_Alarm.Ref_Value,cSetBatTempAlarm);
        sReplyProtectGlobal(Protect_Global.Bat_Temp_Alarm.Ref_Value,cSetBatTempAlarm,Protect_Global.Bat_Temp_Alarm.Protect_EN);
    }
    else if(ubCmdType==cSetBatTempOverAlarm)
    {
        sReplyProtectGlobal(Protect_Global.Bat_Temp_Over_Alarm.Ref_Value,cSetBatTempAlarm,Protect_Global.Bat_Temp_Over_Alarm.Protect_EN);
    }

    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}
//File end
