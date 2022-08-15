/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_Record.c
// Created on: 2020-4-8
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义


//函数声明


//函数定义

void sSetRecondACKCommand(INT8U ubCmd)
{
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetRecondACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCmd;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSuccess;
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


void sSetRecondNACKCommand(INT8U ubCmd)
{
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetRecondACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCmd;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cFail;
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



//------------------------记录类--------------------------------//
//------------------------记录条件设置指令----------------------//
void sSetRecondConditionCommand(void)
{
    INT8U   ubCmdType0 = 0;
    INT8U   ubCmdType1 = 0;
    INT8U   ubCmdType2 = 0;
    //INT32U  udwRecondTimeTemp = 0;
    //INT32U  udwRecondTimeTemp0 = 0;
    //INT32U  udwRecondTimeTemp1 = 0;
    //INT32U  udwRecondTimeTemp2 = 0;
    //INT32U  udwRecondTimeTemp3 = 0;
    //INT32S  dwRecondVoltTemp = 0;
    //INT32S  dwRecondVoltTemp0 = 0;
    //INT32S  dwRecondVoltTemp1 = 0;
    //INT32S  dwRecondVoltTemp2 = 0;
    //INT32S  dwRecondCurrentTemp = 0;
    //INT32S  dwRecondCurrentTemp0 = 0;
    //INT32S  dwRecondCurrentTemp1 = 0;
    //INT32S  dwRecondCurrentTemp2 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[15];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[16];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if((uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)&&((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)))
    {
        ubCmdType0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
        ubCmdType1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
        ubCmdType2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[11];
        /*if(ubCmdType0==cRecondTime)
        {
            udwRecondTimeTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
            udwRecondTimeTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
            udwRecondTimeTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
            udwRecondTimeTemp3 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
            udwRecondTimeTemp = (udwRecondTimeTemp0<<24)+\
                                    (udwRecondTimeTemp1<<16)+\
                                        (udwRecondTimeTemp2<<8)+\
                                            udwRecondTimeTemp3;
            sSetRecondTimeSet(udwRecondTimeTemp);
            sSetRecondTimeEn(cON);
        }
        else
        {
            sSetRecondTimeEn(cOFF);
        }
        if(ubCmdType1==cRecondVolt)
        {
            dwRecondVoltTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[8];
            dwRecondVoltTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[9];
            dwRecondVoltTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[10];
            dwRecondVoltTemp = (dwRecondVoltTemp0<<16)+\
                                    (dwRecondVoltTemp1<<8)+\
                                        dwRecondVoltTemp2;

            dwRecondVoltTemp = dwRecondVoltTemp/10;
            sSetRecondVoltSet(dwRecondVoltTemp);
            sSetRecondVoltEn(cON);
        }
        else
        {
            sSetRecondVoltEn(cOFF);
        }
        if(ubCmdType2==cRecondCurrent)
        {
            dwRecondCurrentTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[12];
            dwRecondCurrentTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[13];
            dwRecondCurrentTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[14];
            dwRecondCurrentTemp = (dwRecondCurrentTemp0<<16)+\
                                    (dwRecondCurrentTemp1<<8)+\
                                        dwRecondCurrentTemp2;
            dwRecondCurrentTemp = dwRecondCurrentTemp/10;
            sSetRecondCurrentSet(dwRecondCurrentTemp);
            sSetRecondCurrentEn(cON);
        }
        else
        {
            sSetRecondCurrentEn(cOFF);
        }*/

        if((ubCmdType0==cRecondTime)&&\
            (ubCmdType1==cRecondErr)&&\
                (ubCmdType2==cRecondErr))
        {
            sSetRecondACKCommand(cRecondTime);
        }
        else if((ubCmdType0==cRecondErr)&&\
                (ubCmdType1==cRecondVolt)&&\
                    (ubCmdType2==cRecondErr))
        {
            sSetRecondACKCommand(cRecondVolt);
        }
        else if((ubCmdType0==cRecondErr)&&\
                    (ubCmdType1==cRecondErr)&&\
                    (ubCmdType2==cRecondCurrent))
        {
            sSetRecondACKCommand(cRecondCurrent);
        }
        else if((ubCmdType0==cRecondTime)&&\
                        (ubCmdType1==cRecondVolt)&&\
                            (ubCmdType2==cRecondErr))
        {
            sSetRecondACKCommand(cRecondTimeVolt);
        }
        else if((ubCmdType0==cRecondTime)&&\
                            (ubCmdType1==cRecondErr)&&\
                                (ubCmdType2==cRecondCurrent))
        {
            sSetRecondACKCommand(cRecondTimeCurrent);
        }
        else if((ubCmdType0==cRecondErr)&&\
                                (ubCmdType1==cRecondVolt)&&\
                                    (ubCmdType2==cRecondCurrent))
        {
            sSetRecondACKCommand(cRecondVoltCurrent);
        }
        else if((ubCmdType0==cRecondTime)&&\
                                    (ubCmdType1==cRecondVolt)&&\
                                        (ubCmdType2==cRecondCurrent))
        {
            sSetRecondACKCommand(cRecondTimeVoltCurrent);
        }
        else
        {
            sSetRecondNACKCommand(cRecondErr);
        }
    }
    else
    {
        sSetRecondNACKCommand(cRecondErr);
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}


void sInquireRecondDataCommand(void)//记录数据上返
{
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        SaveDataToRecord((struct_Image *)&Image_UpData.Record);
        if((Image_UpData.Record.ubWorkStepName==cStepStatic)||(Image_UpData.Record.ubWorkStepName==cStepNegPressure))
        {
            Image_UpData.Record.dwOutPutCurrentNew_UP=0;
        }

        if(WorkStep_Service.State!=STEP_RUN)
        {
            Image_UpData.Record.dwOutPutCurrentNew_UP=0;
        }
        sReplyRecondDataCommand((struct_Image *)&Image_UpData.Record,CH_TX_RETURN);
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}

INT32S  udwEnergyTemp=0;
INT8U   ubEnergyTemp0=0;
INT8U   ubEnergyTemp1=0;
INT8U   ubEnergyTemp2=0;
void sReplyRecondDataCommand(struct_Image *Ptr,Uint16 CH_TX)//记录数据上返
{
    INT8U   ubCmdNumTemp = 0x53;//0x4B;//INT8U   ubCmdNumTemp = 0x3F;//
    INT8U   ubRunModeTemp = 0;
    INT32U  udwRunTimeTemp = 0;
    INT8U   ubRunTimeTemp0 = 0;
    INT8U   ubRunTimeTemp1 = 0;
    INT8U   ubRunTimeTemp2 = 0;
    INT8U   ubRunTimeTemp3 = 0;
    INT32S  dwBatVoltTemp = 0;
    INT8U   ubBatVoltTemp0 = 0;
    INT8U   ubBatVoltTemp1 = 0;
    INT8U   ubBatVoltTemp2 = 0;
    INT32S  dwRealCurrent=0;
    INT8U   ubRealCurrent0=0;
    INT8U   ubRealCurrent1=0;
    INT8U   ubRealCurrent2=0;
    INT32U  udwBatAH = 0;
    INT8U   ubBatAH0 = 0;
    INT8U   ubBatAH1 = 0;
    INT8U   ubBatAH2 = 0;
    INT8U   ubBatAH3 = 0;
    INT16U  uwBatTemp = 0;
    INT8U   ubBatTemp0 = 0;
    INT8U   ubBatTemp1 = 0;
    INT16U  uwStepNumTemp = 0;
    INT8U   ubStepNumTemp0 = 0;
    INT8U   ubStepNumTemp1 = 0;
    INT8U   ubStepNameTemp = 0;
    INT16U  uwCycleIndexTemp = 0;
    INT8U   ubCycleIndexTemp0 = 0;
    INT8U   ubCycleIndexTemp1 = 0;

    //20191212
    long long DateTime_Temp=0;
    INT8U DateTime_1=0;
    INT8U DateTime_2=0;
    INT8U DateTime_3=0;
    INT8U DateTime_4=0;
    INT8U DateTime_5=0;
    INT8U DateTime_6=0;
    //20191212

    INT16U  uwStepTotalTemp = 0;
    INT8U   ubStepTotalTemp0 = 0;
    INT8U   ubStepTotalTemp1 = 0;

    INT32U  udwRatioTemp=0;
    INT8U   ubRatioTemp0=0;
    INT8U   ubRatioTemp1=0;
    INT8U   ubRatioTemp2=0;
//----------------------------------
    INT32S  dwPowerVolt=0;
    INT8U   ubPowerVoltH=0;
    INT8U   ubPowerVoltM=0;
    INT8U   ubPowerVoltL=0;
    //--------------------------------------
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    ubRunModeTemp = Ptr->ubChargerStatus;//20191226 sGetChargerStatus();

    udwRunTimeTemp = Ptr->udwSuperRunTime;//20191226 udwSuperRunTime;
    ubRunTimeTemp0 = (INT8U)((udwRunTimeTemp&0xFF000000)>>24);
    ubRunTimeTemp1 = (INT8U)((udwRunTimeTemp&0x00FF0000)>>16);
    ubRunTimeTemp2 = (INT8U)((udwRunTimeTemp&0x0000FF00)>>8);
    ubRunTimeTemp3 = (INT8U)(udwRunTimeTemp&0x000000FF);

    dwBatVoltTemp =Ptr->wBatSenseAVoltNew_UP;//20191226 Vol_SenseA_1S;
    //if(dwBatVoltTemp<=0)dwBatVoltTemp = 0;
    /*if(((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==TRUE))||
       ((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==TRUE)))
    {
        //只在恒压阶段并且无故障时对上报电压进行处理
        if((Fault_Code.Err_Global==0)&&(Fault_Code.Err_Step==0)&&(DCDC_Service.Fault_Module==0)&&(Fault_Code.Alarm==0))
        {
            int32 Delta=dwBatVoltTemp-DCDC_Service.Set_Vol;
            if((Delta<-2)&&(Delta>-25))
            {
                dwBatVoltTemp=DCDC_Service.Set_Vol-2;
            }
            else if((Delta>2)&&(Delta<25))
            {
                dwBatVoltTemp=DCDC_Service.Set_Vol+2;
            }
        }
    }*/


    ubBatVoltTemp0 = (INT8U)((dwBatVoltTemp&0xFF0000)>>16);
    ubBatVoltTemp1 = (INT8U)((dwBatVoltTemp&0x00FF00)>>8);
    ubBatVoltTemp2 = (INT8U)(dwBatVoltTemp&0x0000FF);



    dwRealCurrent=Ptr->dwOutPutCurrentNew_UP;//20191226 0;
    ubRealCurrent0=(INT8U)((dwRealCurrent&0xFF0000)>>16);
    ubRealCurrent1=(INT8U)((dwRealCurrent&0x00FF00)>>8);
    ubRealCurrent2=(INT8U)(dwRealCurrent&0x0000FF);

    udwBatAH = Ptr->udwChargerCapacityNew;//20191226 sGetChargerCapacityNew();
    ubBatAH0 = (INT8U)((udwBatAH&0xFF000000)>>24);
    ubBatAH1 = (INT8U)((udwBatAH&0x00FF0000)>>16);
    ubBatAH2 = (INT8U)((udwBatAH&0x0000FF00)>>8);
    ubBatAH3 = (INT8U)(udwBatAH&0x000000FF);

    uwBatTemp = Ptr->uwBatteryDegreeNew;//20191226 sBatTemperatureGet();
    ubBatTemp0 = (INT8U)((uwBatTemp&0xFF00)>>8);
    ubBatTemp1 = (INT8U)(uwBatTemp&0x00FF);

    uwStepNumTemp = Ptr->uwWorkStepNum;//201911226 sGetWorkStepNum();
    ubStepNumTemp0 = (INT8U)((uwStepNumTemp&0xFF00)>>8);
    ubStepNumTemp1 = (INT8U)(uwStepNumTemp&0x00FF);

    ubStepNameTemp = Ptr->ubWorkStepName;//20191226 sGetWorkStepName();//ubChargerMode;

    uwCycleIndexTemp = Ptr->uwStepCycleStartIndex;//PI_Vol_Charge.Ki;//debug
    ubCycleIndexTemp0 = (INT8U)((uwCycleIndexTemp&0xFF00)>>8);
    ubCycleIndexTemp1 = (INT8U)(uwCycleIndexTemp&0x00FF);

    //20191212
    DateTime_Temp=Ptr->DateTime;//20191226 DateTime;
    DateTime_1=DateTime_Temp&0x00000000000000FF;
    DateTime_2=(DateTime_Temp>>8 )&0x00000000000000FF;
    DateTime_3=(DateTime_Temp>>16)&0x00000000000000FF;
    DateTime_4=(DateTime_Temp>>24)&0x00000000000000FF;
    DateTime_5=(DateTime_Temp>>32)&0x00000000000000FF;
    DateTime_6=(DateTime_Temp>>40)&0x00000000000000FF;
    //20191212

    uwStepTotalTemp = Ptr->uwWorkStepTotalNum;//PI_Vol_Discharge.Ki;//debug
    ubStepTotalTemp0 = (INT8U)((uwStepTotalTemp&0xFF00)>>8);
    ubStepTotalTemp1 = (INT8U)(uwStepTotalTemp&0x00FF);
    //----------------HWK0428---------------
    if(DCDC_Service.Now_Mode==DCDC_CCtoCV)
    {
        udwRatioTemp = Ptr->udwAH_CCdivCV;//20191226 sGetOutPutConstantCurrentRatioCCdivCV();
    }
    else if(DCDC_Service.Now_Mode==DCDC_DCCtoDCV)
    {
        udwRatioTemp = Ptr->udwAH_DCCdivDCV;//20191226 sGetOutPutConstantCurrentRatioDCCdivDCV();
    }
    else
    {
        udwRatioTemp = 0;
    }
    ubRatioTemp0 = (INT8U)((udwRatioTemp&0x00FF0000)>>16);
    ubRatioTemp1 = (INT8U)((udwRatioTemp&0x0000FF00)>>8);
    ubRatioTemp2 = (INT8U)(udwRatioTemp&0x000000FF);

    udwEnergyTemp=Ptr->dwBatEnergy;//20191226 sGetBatEnergy();
    ubEnergyTemp0 = (INT8U)((udwEnergyTemp&0x00FF0000)>>16);
    ubEnergyTemp1 = (INT8U)((udwEnergyTemp&0x0000FF00)>>8);
    ubEnergyTemp2 = (INT8U)(udwEnergyTemp&0x000000FF);

    dwPowerVolt = Ptr->wOutPutVoltNew_UP;//20191226 Vol_Out_1S;
    ubPowerVoltH = (INT8U)((dwPowerVolt&0x00FF0000)>>16);
    ubPowerVoltM = (INT8U)((dwPowerVolt&0x0000FF00)>>8);
    ubPowerVoltL = (INT8U)((dwPowerVolt&0x000000FF));
    //--------------------------------------
    INT16U uwTcpipIndex=0;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubCmdNumTemp;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cReplyRecondData;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cRunMode;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubRunModeTemp;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cRuntime;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubRunTimeTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubRunTimeTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubRunTimeTemp2;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubRunTimeTemp3;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cBatVolt;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubBatVoltTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubBatVoltTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubBatVoltTemp2;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cOPCurrent;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubRealCurrent0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubRealCurrent1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubRealCurrent2;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cCAH;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubBatAH0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubBatAH1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubBatAH2;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubBatAH3;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cBatTemp;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubBatTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubBatTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cSetpNum;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubStepNumTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubStepNumTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cSetpName;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubStepNameTemp;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cCycleNum;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubCycleIndexTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubCycleIndexTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cSysTime;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = 0;//20191212 TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubYearTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = 0;//20191212 TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubYearTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = 0;//20191212 TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubMonthTemp;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = DateTime_6;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = DateTime_5;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = DateTime_4;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = DateTime_3;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = DateTime_2;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = DateTime_1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cStepTotalNum;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubStepTotalTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubStepTotalTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
//-------------------------HWK0428----------------------
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cRatio;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubRatioTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubRatioTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubRatioTemp2;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cEnergy;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubEnergyTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubEnergyTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubEnergyTemp2;
    uwTcpipIndex = uwTcpipIndex+1;
//--------------------HWK0508-------------------------
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cOPVolt;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubPowerVoltH;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubPowerVoltM;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubPowerVoltL;
    uwTcpipIndex = uwTcpipIndex+1;

    //20190921 udwChannel1TempSampleAvg
    INT16U uwChannel1NTCDegreeNewTemp;
    uwChannel1NTCDegreeNewTemp=Data_Sum.Temp_NTC2;//Ptr->uwChannel1NTCDegreeNew;//20191226
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (uwChannel1NTCDegreeNewTemp>>8)&0x00FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (uwChannel1NTCDegreeNewTemp)&0x00FF;
    uwTcpipIndex = uwTcpipIndex+1;

    //20190921

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (Ptr->Vol_Contact>>24)&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (Ptr->Vol_Contact>>16)&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (Ptr->Vol_Contact>> 8)&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (Ptr->Vol_Contact    )&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (Ptr->Res_Contact>>8)&0x00FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (Ptr->Res_Contact   )&0x00FF;
    uwTcpipIndex = uwTcpipIndex+1;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (Ptr->Res_Loop>>8)&0x00FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (Ptr->Res_Loop   )&0x00FF;
    uwTcpipIndex = uwTcpipIndex+1;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Cnt_Cycle>>8)&0x00FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Cnt_Cycle)&0x00FF;;
    uwTcpipIndex = uwTcpipIndex+1;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = Fault_Code.Code&0x00FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = Ptr->NegPressure;
    uwTcpipIndex = uwTcpipIndex+1;

//
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (WorkStep_Service.Run_TimeTotal>>24)&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (WorkStep_Service.Run_TimeTotal>>16)&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (WorkStep_Service.Run_TimeTotal>>8)&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (WorkStep_Service.Run_TimeTotal)&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (WorkStep_Data.Formula_NumberL>>24)&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (WorkStep_Data.Formula_NumberL>>16)&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (WorkStep_Data.Formula_NumberL>>8)&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = (WorkStep_Data.Formula_NumberL)&0x000000FF;
    uwTcpipIndex = uwTcpipIndex+1;



//-----------------------------------------------------
    //校验位HL
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[0],uwTcpipIndex);
    ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
    ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Data[uwTcpipIndex] = cEnd;
    uwTcpipIndex = uwTcpipIndex+1;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Num_To_TX=uwTcpipIndex;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Result_TX=0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX].Req_TX=1;
}

//File end

