/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_Fluct.c
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义


//函数声明


//函数定义

void sSetFlucMutatProtctACKCommand(INT8U ubAck)
{
    INT8U   ubCmdType1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcTemp0 = 0;
    INT8U   ubTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpipIndex =0;

    ubCmdType1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x7F;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cetFlucMutatProtectACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCmdType1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubAck;
    uwTcpipIndex = uwTcpipIndex+1;
    //校验位HL
    uwTcpCmdCrcCheck = sCRC16N(TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data,uwTcpipIndex);
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


void sSetFlucProtectVCommand(void)
{
    INT32U  udwSetProtctVTemp = 0;
    INT32U  udwSetProtctVTemp0 = 0;
    INT32U  udwSetProtctVTemp1 = 0;
    INT32U  udwSetProtctVTemp2 = 0;
    INT32U  udwSetIntervalTimeTemp = 0;
    INT32U  udwSetIntervalTimeTemp0 = 0;
    INT32U  udwSetIntervalTimeTemp1 = 0;
    INT32U  udwSetIntervalTimeTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[10];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[11];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);

    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctVTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctVTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctVTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        udwSetProtctVTemp = (udwSetProtctVTemp0<<16)+\
                                (udwSetProtctVTemp1<<8)+\
                                    udwSetProtctVTemp2;

        udwSetIntervalTimeTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
        udwSetIntervalTimeTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[8];
        udwSetIntervalTimeTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[9];
        udwSetIntervalTimeTemp = (udwSetIntervalTimeTemp0<<16)+\
                                    (udwSetIntervalTimeTemp1<<8)+\
                                        udwSetIntervalTimeTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Vol_Fluct_Over.Protect_EN=TRUE;
            Protect_Global.Vol_Fluct_Over.Ref_Delta=udwSetProtctVTemp;//sSetBatSenseFlucProtctV(udwSetProtctVTemp);
            Protect_Global.Vol_Fluct_Over.Num_Check=udwSetIntervalTimeTemp;//Protect_Global.Vol_Fluct_Over.Num_Delta=udwSetIntervalTimeTemp;//sSetBatSenseFlucProtctIntervalTime(udwSetIntervalTimeTemp);
        }
        else
        {
            Protect_Global.Vol_Fluct_Over.Protect_EN=FALSE;
        }

        sSetFlucMutatProtctACKCommand(cSuccess);
    }
    else
    {
        sSetFlucMutatProtctACKCommand(cFail);
    }
}

void sSetMutatProtectVCommand(void)
{
    INT32U  udwSetProtctVTemp = 0;
    INT32U  udwSetProtctVTemp0 = 0;
    INT32U  udwSetProtctVTemp1 = 0;
    INT32U  udwSetProtctVTemp2 = 0;
    INT32U  udwSetIntervalTimeTemp = 0;
    INT32U  udwSetIntervalTimeTemp0 = 0;
    INT32U  udwSetIntervalTimeTemp1 = 0;
    INT32U  udwSetIntervalTimeTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[10];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[11];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctVTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctVTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctVTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        udwSetProtctVTemp = (udwSetProtctVTemp0<<16)+\
                                    (udwSetProtctVTemp1<<8)+\
                                        udwSetProtctVTemp2;
        Protect_Global.Vol_Mutat_Over.Ref_Delta=udwSetProtctVTemp;//sSetBatSenseMutatProtctV(udwSetProtctVTemp);
        udwSetIntervalTimeTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
        udwSetIntervalTimeTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[8];
        udwSetIntervalTimeTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[9];
        udwSetIntervalTimeTemp = (udwSetIntervalTimeTemp0<<16)+\
                                        (udwSetIntervalTimeTemp1<<8)+\
                                            udwSetIntervalTimeTemp2;
        Protect_Global.Vol_Mutat_Over.Num_Delta=udwSetIntervalTimeTemp;//sSetBatSenseMutatProtctIntervalTime(udwSetIntervalTimeTemp);

        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Vol_Mutat_Over.Protect_EN=TRUE;
        }
        else
        {
            Protect_Global.Vol_Mutat_Over.Protect_EN=FALSE;
        }

        sSetFlucMutatProtctACKCommand(cSuccess);
    }
    else
    {
        sSetFlucMutatProtctACKCommand(cFail);
    }
}

void sSetFlucProtectICommand(void)
{
    INT32U  udwSetProtctITemp = 0;
    INT32U  udwSetProtctITemp0 = 0;
    INT32U  udwSetProtctITemp1 = 0;
    INT32U  udwSetProtctITemp2 = 0;
    INT32U  udwSetIntervalTimeTemp = 0;
    INT32U  udwSetIntervalTimeTemp0 = 0;
    INT32U  udwSetIntervalTimeTemp1 = 0;
    INT32U  udwSetIntervalTimeTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[10];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[11];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctITemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctITemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctITemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        udwSetProtctITemp = (udwSetProtctITemp0<<16)+\
                                (udwSetProtctITemp1<<8)+\
                                    udwSetProtctITemp2;

        udwSetIntervalTimeTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
        udwSetIntervalTimeTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[8];
        udwSetIntervalTimeTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[9];
        udwSetIntervalTimeTemp = (udwSetIntervalTimeTemp0<<16)+\
                                    (udwSetIntervalTimeTemp1<<8)+\
                                        udwSetIntervalTimeTemp2;


        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Cur_Fluct_Over.Protect_EN=TRUE;
            Protect_Global.Cur_Fluct_Over.Ref_Delta=udwSetProtctITemp;//sSetOPCurrentFlucProtctI(udwSetProtctITemp);
            Protect_Global.Cur_Fluct_Over.Num_Check=udwSetIntervalTimeTemp;//Protect_Global.Cur_Fluct_Over.Num_Delta=udwSetIntervalTimeTemp;//sSetOPCurrentFlucProtctIntervalTime(udwSetIntervalTimeTemp);
        }
        else
        {
            Protect_Global.Cur_Fluct_Over.Protect_EN=FALSE;
        }

        sSetFlucMutatProtctACKCommand(cSuccess);
    }
    else
    {
        sSetFlucMutatProtctACKCommand(cFail);
    }
}

void sSetMutatProtectICommand(void)
{
    INT32U  udwSetProtctITemp = 0;
    INT32U  udwSetProtctITemp0 = 0;
    INT32U  udwSetProtctITemp1 = 0;
    INT32U  udwSetProtctITemp2 = 0;
    INT32U  udwSetIntervalTimeTemp = 0;
    INT32U  udwSetIntervalTimeTemp0 = 0;
    INT32U  udwSetIntervalTimeTemp1 = 0;
    INT32U  udwSetIntervalTimeTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[10];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[11];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctITemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        udwSetProtctITemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
        udwSetProtctITemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        udwSetProtctITemp = (udwSetProtctITemp0<<16)+\
                                (udwSetProtctITemp1<<8)+\
                                    udwSetProtctITemp2;
        Protect_Global.Cur_Mutat_Over.Ref_Delta=udwSetProtctITemp;//sSetOPCurrentMutatProtctI(udwSetProtctITemp);
        udwSetIntervalTimeTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
        udwSetIntervalTimeTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[8];
        udwSetIntervalTimeTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[9];
        udwSetIntervalTimeTemp = (udwSetIntervalTimeTemp0<<16)+\
                                    (udwSetIntervalTimeTemp1<<8)+\
                                        udwSetIntervalTimeTemp2;
        Protect_Global.Cur_Mutat_Over.Num_Delta=udwSetIntervalTimeTemp;//sSetOPCurrentMutatProtctIntervalTime(udwSetIntervalTimeTemp);

        if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x80)==0)
        {
            Protect_Global.Cur_Mutat_Over.Protect_EN=TRUE;
        }
        else
        {
            Protect_Global.Cur_Mutat_Over.Protect_EN=FALSE;
        }

        sSetFlucMutatProtctACKCommand(cSuccess);
    }
    else
    {
        sSetFlucMutatProtctACKCommand(cFail);
    }
}


//------------------------波动/突变保护类----------------------//
//------------------------波动/突变保护设置功能指令----------------------//
void sSetFlucMutatProtctCommand(void)
{
    INT8U   ubCmdType1 = 0;
    INT8U   ubCmdType2 = 0;
    ubCmdType1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x7F;
    ubCmdType2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6]&0x7F;

    if((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)||(WorkStep_Service.Step_State==cPauseStatus))
    {
        if((ubCmdType1==cSetFlucProtectV)&&(ubCmdType2==cSetProtectBatMutat))
        {
            sSetFlucProtectVCommand();
        }
        else if((ubCmdType1==cSetMutatProtectV)&&(ubCmdType2==cSetIntervalTime))
        {
            sSetMutatProtectVCommand();
        }
        else if((ubCmdType1==cSetFlucProtectI)&&(ubCmdType2==cSetProtectOPCMutat))
        {
            sSetFlucProtectICommand();
        }
        else if((ubCmdType1==cSetMutatProtectI)&&(ubCmdType2==cSetIntervalTime))
        {
            sSetMutatProtectICommand();
        }
        else
        {
            sSetFlucMutatProtctACKCommand(cFail);
        }
    }
    else
    {
        sSetFlucMutatProtctACKCommand(cFail);
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}

void sReplyFlucProtctVCommand(void)
{
    INT32U  udwSetProtctVTemp = 0;
    INT8U   ubSetProtctVTemp0 = 0;
    INT8U   ubSetProtctVTemp1 = 0;
    INT8U   ubSetProtctVTemp2 = 0;
    INT32U  udwSetIntervalTimeTemp = 0;
    INT8U   ubSetIntervalTimeTemp0 = 0;
    INT8U   ubSetIntervalTimeTemp1 = 0;
    INT8U   ubSetIntervalTimeTemp2 = 0;
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
        udwSetProtctVTemp = Protect_Global.Vol_Fluct_Over.Ref_Delta;//sGetBatSenseFlucProtctV();
        ubSetProtctVTemp0 = (INT8U)((udwSetProtctVTemp&0xFF0000)>>16);
        ubSetProtctVTemp1 = (INT8U)((udwSetProtctVTemp&0x00FF00)>>8);
        ubSetProtctVTemp2 = (INT8U)(udwSetProtctVTemp&0x0000FF);
        udwSetIntervalTimeTemp = Protect_Global.Vol_Fluct_Over.Num_Check;//Protect_Global.Vol_Fluct_Over.Num_Delta;//sGetBatSenseFlucProtctIntervalTime();
        ubSetIntervalTimeTemp0 = (INT8U)((udwSetIntervalTimeTemp&0xFF0000)>>16);
        ubSetIntervalTimeTemp1 = (INT8U)((udwSetIntervalTimeTemp&0x00FF00)>>8);
        ubSetIntervalTimeTemp2 = (INT8U)(udwSetIntervalTimeTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x0D;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cReplyFlucMutatData;
        uwTcpipIndex = uwTcpipIndex+1;

        if(Protect_Global.Vol_Fluct_Over.Protect_EN==TRUE)
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetFlucProtectV;
        }
        else
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetFlucProtectV+128;
        }

        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctVTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctVTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctVTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetIntervalTime;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp2;
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
void sReplyMutatProtctVCommand(void)
{
    INT32U  udwSetProtctVTemp = 0;
    INT8U   ubSetProtctVTemp0 = 0;
    INT8U   ubSetProtctVTemp1 = 0;
    INT8U   ubSetProtctVTemp2 = 0;
    INT32U  udwSetIntervalTimeTemp = 0;
    INT8U   ubSetIntervalTimeTemp0 = 0;
    INT8U   ubSetIntervalTimeTemp1 = 0;
    INT8U   ubSetIntervalTimeTemp2 = 0;
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
    uwTcpCmdCrcCheck = sCRC16N(TCP_Socket[SERV_TCP_WORK].Data_RX.Data,ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctVTemp = Protect_Global.Vol_Mutat_Over.Ref_Delta;//sGetBatSenseMutatProtctV();
        ubSetProtctVTemp0 = (INT8U)((udwSetProtctVTemp&0xFF0000)>>16);
        ubSetProtctVTemp1 = (INT8U)((udwSetProtctVTemp&0x00FF00)>>8);
        ubSetProtctVTemp2 = (INT8U)(udwSetProtctVTemp&0x0000FF);
        udwSetIntervalTimeTemp = Protect_Global.Vol_Mutat_Over.Num_Delta;//sGetBatSenseMutatProtctIntervalTime();
        ubSetIntervalTimeTemp0 = (INT8U)((udwSetIntervalTimeTemp&0xFF0000)>>16);
        ubSetIntervalTimeTemp1 = (INT8U)((udwSetIntervalTimeTemp&0x00FF00)>>8);
        ubSetIntervalTimeTemp2 = (INT8U)(udwSetIntervalTimeTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x0D;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cReplyFlucMutatData;
        uwTcpipIndex = uwTcpipIndex+1;

        if(Protect_Global.Vol_Mutat_Over.Protect_EN==TRUE)
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetMutatProtectV;
        }
        else
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetMutatProtectV+128;
        }

        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctVTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctVTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctVTemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetIntervalTime;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp2;
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
void sReplyFlucProtctICommand(void)
{
    INT32U  udwSetProtctITemp = 0;
    INT8U   ubSetProtctITemp0 = 0;
    INT8U   ubSetProtctITemp1 = 0;
    INT8U   ubSetProtctITemp2 = 0;
    INT32U  udwSetIntervalTimeTemp = 0;
    INT8U   ubSetIntervalTimeTemp0 = 0;
    INT8U   ubSetIntervalTimeTemp1 = 0;
    INT8U   ubSetIntervalTimeTemp2 = 0;
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
        udwSetProtctITemp = Protect_Global.Cur_Fluct_Over.Ref_Delta;//sGetOPCurrentFlucProtctI();
        ubSetProtctITemp0 = (INT8U)((udwSetProtctITemp&0xFF0000)>>16);
        ubSetProtctITemp1 = (INT8U)((udwSetProtctITemp&0x00FF00)>>8);
        ubSetProtctITemp2 = (INT8U)(udwSetProtctITemp&0x0000FF);
        udwSetIntervalTimeTemp = Protect_Global.Cur_Fluct_Over.Num_Check;//Protect_Global.Cur_Fluct_Over.Num_Delta;//sGetOPCurrentFlucProtctIntervalTime();
        ubSetIntervalTimeTemp0 = (INT8U)((udwSetIntervalTimeTemp&0xFF0000)>>16);
        ubSetIntervalTimeTemp1 = (INT8U)((udwSetIntervalTimeTemp&0x00FF00)>>8);
        ubSetIntervalTimeTemp2 = (INT8U)(udwSetIntervalTimeTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x0D;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cReplyFlucMutatData;
        uwTcpipIndex = uwTcpipIndex+1;

        if(Protect_Global.Cur_Fluct_Over.Protect_EN==TRUE)
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetFlucProtectI;
        }
        else
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetFlucProtectI+128;
        }

        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctITemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctITemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctITemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetIntervalTime;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp2;
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
void sReplyMutatProtctICommand(void)
{
    INT32U  udwSetProtctITemp = 0;
    INT8U   ubSetProtctITemp0 = 0;
    INT8U   ubSetProtctITemp1 = 0;
    INT8U   ubSetProtctITemp2 = 0;
    INT32U  udwSetIntervalTimeTemp = 0;
    INT8U   ubSetIntervalTimeTemp0 = 0;
    INT8U   ubSetIntervalTimeTemp1 = 0;
    INT8U   ubSetIntervalTimeTemp2 = 0;
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
    uwTcpCmdCrcCheck = sCRC16N(TCP_Socket[SERV_TCP_WORK].Data_RX.Data,ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        udwSetProtctITemp = Protect_Global.Cur_Mutat_Over.Ref_Delta;//sGetOPCurrentMutatProtctI();
        ubSetProtctITemp0 = (INT8U)((udwSetProtctITemp&0xFF0000)>>16);
        ubSetProtctITemp1 = (INT8U)((udwSetProtctITemp&0x00FF00)>>8);
        ubSetProtctITemp2 = (INT8U)(udwSetProtctITemp&0x0000FF);
        udwSetIntervalTimeTemp = Protect_Global.Cur_Mutat_Over.Num_Delta;//sGetOPCurrentMutatProtctIntervalTime();
        ubSetIntervalTimeTemp0 = (INT8U)((udwSetIntervalTimeTemp&0xFF0000)>>16);
        ubSetIntervalTimeTemp1 = (INT8U)((udwSetIntervalTimeTemp&0x00FF00)>>8);
        ubSetIntervalTimeTemp2 = (INT8U)(udwSetIntervalTimeTemp&0x0000FF);
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x0D;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cReplyFlucMutatData;
        uwTcpipIndex = uwTcpipIndex+1;

        if(Protect_Global.Cur_Mutat_Over.Protect_EN==TRUE)
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetMutatProtectI;
        }
        else
        {
            TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetMutatProtectI+128;
        }

        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctITemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctITemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetProtctITemp2;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetIntervalTime;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp0;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp1;
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSetIntervalTimeTemp2;
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

//-----------------波动/突变保护功能设置查询指令------------------//
void sInquireFlucMutaProtctCommand(void)
{
    INT8U   ubCmdType = 0;
    ubCmdType = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]&0x7F;
    if(ubCmdType==cSetFlucProtectV)
    {
        sReplyFlucProtctVCommand();
    }
    else if(ubCmdType==cSetMutatProtectV)
    {
        sReplyMutatProtctVCommand();
    }
    else if(ubCmdType==cSetFlucProtectI)
    {
        sReplyFlucProtctICommand();
    }
    else if(ubCmdType==cSetMutatProtectI)
    {
        sReplyMutatProtctICommand();
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}

//File end
void sInquirePowerOffEnCommand(void)
{
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwPowerOffStepTemp = 0;
    INT8U   ubPowerOffStepTemp0 = 0;
    INT8U   ubPowerOffStepTemp1 = 0;
    INT16U  uwTcpipIndex=0;

    uwPowerOffStepTemp = WorkStep_PowerFail.Step_Index_EEPROM;//debug sEepromPowerOffStepGet();
    ubPowerOffStepTemp0 = (INT8U)((uwPowerOffStepTemp&0xFF00)>>8);
    ubPowerOffStepTemp1 = (INT8U)(uwPowerOffStepTemp&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x08;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cReplyPowerOffEn;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0;//debug sEepromStepCompleteGet();
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubPowerOffStepTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubPowerOffStepTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    //校验位HL
    uwTcpCmdCrcCheck = sCRC16N(TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].pData,uwTcpipIndex);
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

    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}
