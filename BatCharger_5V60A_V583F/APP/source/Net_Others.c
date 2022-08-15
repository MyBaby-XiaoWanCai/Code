/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Prot_Net_Others.c
// Created on: 2020-4-3
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义
Uint16 Num_Cabinet=1;
Uint16 Addr_Module=1;

//函数声明


//函数定义
typedef void (*ptrTcpipFun)();
static const  ptrTcpipFun sTcpipFunJump[] =
{
    sSetProtctCommand,                              //0x01
    sTcpipNullCommand,                              //0x02
    sInquireSetProtctCommand,                       //0x03
    sTcpipNullCommand,                              //0x04
    sTcpipNullCommand,                              //0x05
    sSetRecondConditionCommand,                     //0x06
    sTcpipNullCommand,                              //0x07
    sInquireRecondDataCommand,                      //0x08
    sTcpipNullCommand,                              //0x09
    sSetSetpsCommand,                               //0x0A
    sTcpipNullCommand,                              //0x0B
    sSetChargerStatusCommand,                       //0x0C
    sTcpipNullCommand,                              //0x0D
    sSysVersionInquireCommand,                      //0x0E
    sTcpipNullCommand,                              //0x0F
    sSysCabNumInquireCommand,                       //0x10
    sTcpipNullCommand,                              //0x11
    sSysTimeSetCommand,                             //0x12
    sTcpipNullCommand,                              //0x13
    sFaultClearSetCommand,                          //0x14
    sTcpipNullCommand,                              //0x15
    sSetParallelModeCommand,                        //0x16
    sTcpipNullCommand,                              //0x17
    sInquirePowerOffEnCommand,                      //0x18
    sTcpipNullCommand,                              //0x19
    sSetPowerOffModeCommand,                        //0x1A
    sTcpipNullCommand,                              //0x1B
    sSetFlucMutatProtctCommand,                     //0x1C
    sTcpipNullCommand,                              //0x1D
    sInquireFlucMutaProtctCommand,                  //0x1E
    sTcpipNullCommand,                              //0x1F
    sAgingCalibrationSetCommand,                    //0x20
    sTcpipNullCommand,                              //0x21
    sAgingCalibrationSetInqureCommand,              //0x22
    sTcpipNullCommand,                              //0x23
    sSetPreChargerCommand,                          //0x24
    sTcpipNullCommand,                              //0x25
    sLifeSingalReload,                              //0x26
    sTcpipNullCommand,                              //0x27
    sTcpipNullCommand,                              //0x28
    sFaultInquireCommand,                           //0x29
    sSetAllGlobalProtctCommand,                     //0x2A
    sTcpipNullCommand                               //0x2B
};


//*********************************************************************************
//Description: 网口工作相关协议数据处理
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Socket_Work(void)
{
    if((TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive==0x01)&&(TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX==FALSE))
    {
        Uint16 temp=TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
        Uint16 Data_CRC;
        if((temp>=3)&&(temp<=LEN_TCP_RX_BUFF)&&(TCP_Socket[SERV_TCP_WORK].Data_RX.Data[temp-1]==0x0D))
        {
            Data_CRC=((Uint16)TCP_Socket[SERV_TCP_WORK].Data_RX.Data[temp-3]<<8)+TCP_Socket[SERV_TCP_WORK].Data_RX.Data[temp-2];
            if(sCRC16N(TCP_Socket[SERV_TCP_WORK].Data_RX.pData,temp-3)==Data_CRC)
            {
                //跳转boot区
                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[1]==0xEE)&&(TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2]==0x01))
                {
                    DINT;
                    DRTM;
                    PWM1OFF;
                    PWM2OFF;
                    DC_OUT_CHA_OFF;
                    DC_OUT_CHB_OFF;
                    WriteBootFlag();
                    LED_RED_ON;
                    LED_GREEN_OFF;
                    JumpToBoot();
                }

                if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[1]>=cSetProtect)&&(TCP_Socket[SERV_TCP_WORK].Data_RX.Data[1]<=cFaultReply))
                {
                    sTcpipFunJump[TCP_Socket[SERV_TCP_WORK].Data_RX.Data[1]-1]();
                    TCP_Socket[SERV_TCP_WORK].Cnt_Socket_Timeout    = 0;//Socket超时计数器清零//sSetCommunicationStatus(cSuccess);
                    if(TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus   == FALSE)
                    {
                        Fault_Code.Alarm_Bak=0;
                        Fault_Code.Err_Global_Bak=0;
                        Fault_Code.Err_Step_Bak=0;
                        Fault_Code.Resend_Alarm = 1;
                    }

                    DINT;
                    TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus     = TRUE;//收到一次心跳包后才进行超时检测
                    EINT;

                }
				else
                {
                    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
                }
            }
            else
            {
                sSetProtctNACKCommand();
                TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
            }
        }
        else
        {
            sSetProtctNACKCommand();
            TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
        }
    }
}


void sSetPowerOffModeACKCommand(INT8U ubAck)
{
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex=0;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x06;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetPowerOffModeACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubAck;
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
}

//------------------------断电自运行类------------------//
//----------------------------------------------

void sSetPowerOffModeCommand(void)
{
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubPowerOffModeTemp = 0;
    INT8U   ubChargerStatusTemp = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    ubPowerOffModeTemp = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    ubChargerStatusTemp = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(TCP_Socket[SERV_TCP_WORK].Data_RX.pData,ubTcpCmdCnt-3);
    if((uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)&&((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)))
    {
        if(ubPowerOffModeTemp==cPoweOffModeCancel)
        {
            //sEepromStepCompleteSet(cComplete);
            sSetPowerOffModeACKCommand(cSuccess);
            EEPROM_Write_Uint16(cEepromStepComplete,cComplete);
            //debug OSEventSend(cPrioCom,eComEepromStepCompleteWrite);
        }
        else if((ubPowerOffModeTemp==cPoweOffModeRun)&&(ubChargerStatusTemp==cWorkStepRunStatus))
        {
            if((WorkStep_PowerFail.Step_State_EEPROM==cNotComplete)&&
               (DCDC_Service.Fault_Module==0)&&(Fault_Code.Alarm==0)&&(Fault_Code.Err_Global==0)&&(Fault_Code.Err_Step==0)&&
               ((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)))
            {
                WorkStep_Service.Req_WorkStep_Index 	= WorkStep_PowerFail.Step_Index_EEPROM;//sSetChargerStatusStepNumNow(sEepromPowerOffStepGet());
                WorkStep_Service.Req_PowerFailContine   = TRUE;
                WorkStep_Service.Req_CMD                = WORKSTEP_RUN;
                WorkStep_PowerFail.Step_State_EEPROM	= cComplete;
                EEPROM_Write_Uint16(cEepromStepComplete,cComplete);
                sSetPowerOffModeACKCommand(cSuccess);
                //debug OSEventSend(cPrioCom,eComFlashRead);
            }
            else
            {sSetPowerOffModeACKCommand(cFail);}

        }
        else
        {
            sSetPowerOffModeACKCommand(cFail);
        }
    }
    else
    {
        sSetPowerOffModeACKCommand(cFail);
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}



void sSetPreChargerACKCommand(INT8U ubAck)
{
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex=0;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetPreChargeStatusACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cPreChargeStatus;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubAck;
    uwTcpipIndex = uwTcpipIndex+1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);  //校验位HL
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

//-------------------预充指令----------------------
void sSetPreChargerCommand(void)
{
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubPreChargerType1 = 0;
    INT8U   ubPreChargerType2 = 0;
    INT8U   ubPreChargerType3 = 0;
    INT8U   ubPrepareChargeStatusTemp = 0;
    INT16U  uwPreChargerCurrentTemp0 = 0;
    INT16U  uwPreChargerCurrentTemp1 = 0;
    INT16U  uwPreChargerCurrentTemp = 0;
    INT16U  uwPreChargerTimeTemp0 = 0;
    INT16U  uwPreChargerTimeTemp1 = 0;
    INT16U  uwPreChargerTimeTemp = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    ubPreChargerType1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    ubPreChargerType2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    ubPreChargerType3 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[10];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[11];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if((uwTcpCmdCrcTemp==uwTcpCmdCrcCheck)&&((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)))
    {
        if((ubPreChargerType1==cPreChargeStatus)&&\
                (ubPreChargerType2==cPreChargeCur)&&\
                    (ubPreChargerType3==cPreChargeTime))
        {
            ubPrepareChargeStatusTemp = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
            //debug sSetPrepareChargeStatus(ubPrepareChargeStatusTemp);
            if(ubPrepareChargeStatusTemp==cON)
            {
                //debug OSEventSend(cPrioSuper,eSuperPreChargeOn);
            }
            uwPreChargerCurrentTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
            uwPreChargerCurrentTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
            uwPreChargerCurrentTemp = (uwPreChargerCurrentTemp0<<8)+uwPreChargerCurrentTemp1;
            //debug sSetPrepareChargeCurrent(uwPreChargerCurrentTemp);
            uwPreChargerTimeTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[8];
            uwPreChargerTimeTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[9];
            uwPreChargerTimeTemp = (uwPreChargerTimeTemp0<<8)+uwPreChargerTimeTemp1;
            //debug sSetPrepareChargeTime(uwPreChargerTimeTemp);
        }
        sSetPreChargerACKCommand(cSuccess);
    }
    else
    {
        sSetPreChargerACKCommand(cFail);
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}




void sFaultInquireCommand(void)
{
    INT16U  temp;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex=0;

    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcTemp==uwTcpCmdCrcCheck)
    {
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x0c; //字节长度11
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cFaultReply; //父功能
        uwTcpipIndex = uwTcpipIndex+1;

        temp = Fault_Code.Err_Global;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = (INT8U)((temp&0xFF00)>>8);
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = (INT8U)(temp&0x00FF);
        uwTcpipIndex = uwTcpipIndex+1;

        temp = (Fault_Code.Err_Global>>16)&0x0000FFFF;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = (INT8U)((temp&0xFF00)>>8);
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = (INT8U)(temp&0x00FF);
        uwTcpipIndex = uwTcpipIndex+1;

        temp = Data_Sum.Cur_CHA;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = (INT8U)((temp&0xFF00)>>8);
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = (INT8U)(temp&0x00FF);
        uwTcpipIndex = uwTcpipIndex+1;

        temp = Data_Sum.Cur_CHB;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = (INT8U)((temp&0xFF00)>>8);
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = (INT8U)(temp&0x00FF);
        uwTcpipIndex = uwTcpipIndex+1;

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
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}


//------------------------告警清除-----------------------//
void sFaultClearSetACKCommand(INT8U ubAck)
{
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex=0;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x06;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cFaultClearACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubAck;
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

void sFaultClearSetCommand(void)
{
    INT8U ubFaultClearStatusTemp = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        ubFaultClearStatusTemp = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
        if((ubFaultClearStatusTemp == cFaultCleraSet)&&(WorkStep_Service.Step_State!=cWorkStepRunStatus))
        {
            Interface_DCDC.Req_Mode=DCDC_STANDBY;
            Interface_DCDC.Req_ChangeMode=TRUE;
            DCDC_Service.Fault_Module=0;
            DCDC_Service.Fault_Slaver1=0;
            DCDC_Service.Fault_Slaver2=0;
            DCDC_Service.Fault_Slaver3=0;
            DCDC_Service.Fault_Slaver4=0;
            Fault_Code.Err_Global=0;
            Fault_Code.Err_Global_Bak=0;
            Fault_Code.Err_Step=0;
            Fault_Code.Err_Step_Bak=0;
            Fault_Code.Alarm=0;
            Fault_Code.Alarm_Bak=0;
            Fault_Code.Cnt_S2M_Fault_Mask=0;

            Fault_Code.Code=0;


            DCDC_Service.Cnt_Bus_H              = 0;
            DCDC_Service.Cnt_Bus_L              = 0;
            DCDC_Service.Cnt_Fault_AD7606       = 0;
            CH_A.Cnt_Out_OC                     = 0;
            CH_B.Cnt_Out_OC                     = 0;
            //待确定
            if(DCDC_Service.Mode_Parall==PARALL_MASTER)
            {
                DINT;
                Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Mode=DCDC_STANDBY;
                Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Fault_Clear=TRUE;
                Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_ChangeMode=TRUE;
                Parall_Master.ID1_To_Sx.Cnt_TX_Req_Change_Mode    =1;
                EINT;
            }
            sFaultClearSetACKCommand(cSuccess);
        }
        else
        {
            sFaultClearSetACKCommand(cFail);
        }
    }
    else
    {
        sFaultClearSetACKCommand(cFail);
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}





void sSetParallelModeACKCommand(INT8U ubAck)
{
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex=0;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x06;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetParallelModeACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubAck;
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

//------------------------并机类-----------------------//
void sSetParallelModeCommand(void)
{
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubParaModeTemp = 0;
    INT8U   ubParaNumTemp = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    ubParaModeTemp = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    ubParaNumTemp  = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];

    if(ubParaNumTemp==0) ubParaNumTemp=2;

    if(ubParaNumTemp>5) ubParaNumTemp=5;

    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if((uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)&&((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)))
    {
        if(ubParaModeTemp==cMasterMode)
        {
            Init_ID_Master();

            Parall_Master.Cnt_Period=0;
            Interface_DCDC.Mode_Parall=PARALL_MASTER;
            Interface_DCDC.Parall_Total=ubParaNumTemp;
            Interface_DCDC.Req_Mode = DCDC_STANDBY;

            DCDC_Service.Fault_Module=0;
            DCDC_Service.Fault_Slaver1=0;
            DCDC_Service.Fault_Slaver2=0;
            DCDC_Service.Fault_Slaver3=0;
            DCDC_Service.Fault_Slaver4=0;
            Fault_Code.Err_Global=0;
            Fault_Code.Err_Global_Bak=0;
            Fault_Code.Err_Step=0;
            Fault_Code.Err_Step_Bak=0;
            Fault_Code.Alarm=0;
            Fault_Code.Alarm_Bak=0;
            Fault_Code.Cnt_S2M_Fault_Mask=0;
            Fault_Code.Code=0;

            DCDC_Service.Cnt_Bus_H              = 0;
            DCDC_Service.Cnt_Bus_L              = 0;
            DCDC_Service.Cnt_Fault_AD7606       = 0;
            CH_A.Cnt_Out_OC                     = 0;
            CH_B.Cnt_Out_OC                     = 0;

            PI_Vol_Charge.Limit_Out_max = CUR_MAX_CHANNEL*2*Interface_DCDC.Parall_Total;
            PI_Vol_Charge.Limit_PI_max  = PI_Vol_Charge.Limit_Out_max<<PI_Vol_Charge.Q_Type;

            PI_Vol_Discharge.Limit_Out_max = CUR_MAX_CHANNEL*2*Interface_DCDC.Parall_Total;
            PI_Vol_Discharge.Limit_PI_max  = PI_Vol_Discharge.Limit_Out_max<<PI_Vol_Discharge.Q_Type;

            Interface_DCDC.Req_ChangeMode   = TRUE;

            Parall_Master.ID0_To_Sx.ID0_H.All=0;
            Parall_Master.ID0_To_Sx.ID0_H.Bit.Add_Master    =W5500_Service.SIPR[3];
            Parall_Master.ID0_To_Sx.ID0_H.Bit.Parall_Total  =Interface_DCDC.Parall_Total;
            Parall_Master.ID0_To_Sx.ID0_L.All=0;
            Parall_Master.ID0_To_Sx.ID0_L.Bit.Symbol=SYMBOL_ID0;

            Parall_Master.ID0_To_Sx.Req_TX=TRUE;
            sSetParallelModeACKCommand(cSuccess);

            EEPROM_Write_Char(cEepromParallMode,PARALL_MASTER);
            EEPROM_Write_Char(cEepromParallNum,ubParaNumTemp);
        }
        else if(ubParaModeTemp==cSlaveMode)
        {
            Init_ID_Slaver();
            Parall_Slaver.Cnt_Period=0;
            Parall_Slaver.Cnt_S_OT_M=0;
            Interface_DCDC.Mode_Parall=PARALL_SLAVER;
            Interface_DCDC.Req_Mode = DCDC_STANDBY;
            Interface_DCDC.Parall_Total=2;

            DCDC_Service.Fault_Module=0;
            DCDC_Service.Fault_Slaver1=0;
            DCDC_Service.Fault_Slaver2=0;
            DCDC_Service.Fault_Slaver3=0;
            DCDC_Service.Fault_Slaver4=0;
            Fault_Code.Err_Global=0;
            Fault_Code.Err_Global_Bak=0;
            Fault_Code.Err_Step=0;
            Fault_Code.Err_Step_Bak=0;
            Fault_Code.Alarm=0;
            Fault_Code.Alarm_Bak=0;
            Fault_Code.Cnt_S2M_Fault_Mask=0;
            Fault_Code.Code=0;

            DCDC_Service.Cnt_Bus_H              = 0;
            DCDC_Service.Cnt_Bus_L              = 0;
            DCDC_Service.Cnt_Fault_AD7606       = 0;
            CH_A.Cnt_Out_OC                     = 0;
            CH_B.Cnt_Out_OC                     = 0;

            Interface_DCDC.Req_ChangeMode   = TRUE;

            sSetParallelModeACKCommand(cSuccess);
        }
        else if(ubParaModeTemp==0)//cSingleMode)
        {
            Init_ID_Single();
            Parall_Master.Cnt_Period=0;
            Parall_Slaver.Cnt_Period=0;
            Parall_Slaver.Cnt_S_OT_M=0;
            Interface_DCDC.Mode_Parall=SINGLE;
            Interface_DCDC.Req_Mode = DCDC_STANDBY;
            Interface_DCDC.Parall_Total=1;
            Interface_DCDC.EN_Run    = TRUE;//20210111

            DCDC_Service.Fault_Module=0;
            DCDC_Service.Fault_Slaver1=0;
            DCDC_Service.Fault_Slaver2=0;
            DCDC_Service.Fault_Slaver3=0;
            DCDC_Service.Fault_Slaver4=0;
            Fault_Code.Err_Global=0;
            Fault_Code.Err_Global_Bak=0;
            Fault_Code.Err_Step=0;
            Fault_Code.Err_Step_Bak=0;
            Fault_Code.Alarm=0;
            Fault_Code.Alarm_Bak=0;
            Fault_Code.Cnt_S2M_Fault_Mask=0;
            Fault_Code.Code=0;

            DCDC_Service.Cnt_Bus_H              = 0;
            DCDC_Service.Cnt_Bus_L              = 0;
            DCDC_Service.Cnt_Fault_AD7606       = 0;
            CH_A.Cnt_Out_OC                     = 0;
            CH_B.Cnt_Out_OC                     = 0;

            PI_Vol_Charge.Limit_Out_max = CUR_MAX_CHANNEL*2*Interface_DCDC.Parall_Total;
            PI_Vol_Charge.Limit_PI_max  = PI_Vol_Charge.Limit_Out_max<<PI_Vol_Charge.Q_Type;

            PI_Vol_Discharge.Limit_Out_max = CUR_MAX_CHANNEL*2*Interface_DCDC.Parall_Total;
            PI_Vol_Discharge.Limit_PI_max  = PI_Vol_Discharge.Limit_Out_max<<PI_Vol_Discharge.Q_Type;

            Interface_DCDC.Req_ChangeMode       = TRUE;//20210111
            Interface_DCDC.EN_MOS_Out           = ON;//20210111

            sSetParallelModeACKCommand(cSuccess);

            EEPROM_Write_Char(cEepromParallMode,SINGLE);
            EEPROM_Write_Char(cEepromParallNum,1);
        }
        else
        {
            sSetParallelModeACKCommand(cFail);
        }
    }
    else
    {
        sSetParallelModeACKCommand(cFail);
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}

//------------------------运行指令回复----------------------//
void sSetChargerStatusAckCommand(INT8U ubAck)
{
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex=0;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetChargerStatusACK;
    uwTcpipIndex = uwTcpipIndex+1;

    if(WorkStep_Service.Req_CMD==WORKSTEP_RUN)
    {
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cWorkStepRunStatus;
    }
    else
    {
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cStopStatus;
    }

    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubAck;
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

void sSetChargerStatusCommand(void)//工步控制设置
{
    INT8U ubChargerStatusTemp = 0;
    INT16U uwStepNumNowTemp = 0;
    INT16U uwStepNumNowTemp0 = 0;
    INT16U uwStepNumNowTemp1 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    ubChargerStatusTemp = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    uwStepNumNowTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwStepNumNowTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwStepNumNowTemp = (uwStepNumNowTemp0<<8)+uwStepNumNowTemp1;
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)
    {
        if((DCDC_Service.Mode_Parall==SINGLE)||(DCDC_Service.Mode_Parall==PARALL_MASTER))
        {
            if(ubChargerStatusTemp==cWorkStepRunStatus)//启动工步
            {
                if((DCDC_Service.Fault_Module==0)&&(Fault_Code.Alarm==0)&&(Fault_Code.Err_Global==0)&&(Fault_Code.Err_Step==0)&&
                   (uwStepNumNowTemp>0)&&(uwStepNumNowTemp<=NUM_WORKSTEP_MAX))

                {
                    if((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)||(WorkStep_Service.Step_State==cPauseStatus))
                    {
                        WorkStep_Service.Req_CMD=WORKSTEP_RUN;
                    }
                    else
                    {
                        WorkStep_Service.Req_CMD_OnRunning=WORKSTEP_RUN;
                    }

                    WorkStep_Service.Req_WorkStep_Index=uwStepNumNowTemp;
                    WorkStep_Service.Req_Stop_Socket_Err=FALSE;

                    sSetChargerStatusAckCommand(cSuccess);
                }
                else
                {
                    sSetChargerStatusAckCommand(cFail);
                }
            }
            else if((ubChargerStatusTemp==cPauseStatus)&&(WorkStep_Data.Type!=cStepNegPressure))//暂停
            {
                WorkStep_Service.Req_CMD=WORKSTEP_PAUSE;
                sSetChargerStatusAckCommand(cSuccess);
            }
            else if(ubChargerStatusTemp==cSingalStepRunStatus)//单步
            {
                if((DCDC_Service.Fault_Module==0)&&(Fault_Code.Alarm==0)&&(Fault_Code.Err_Global==0)&&(Fault_Code.Err_Step==0))
                {
                    WorkStep_Service.Req_WorkStep_Index=uwStepNumNowTemp;
                    WorkStep_Service.Req_CMD=WORKSTEP_RUN;
                    WorkStep_Service.Req_SingleStep=TRUE;
                    sSetChargerStatusAckCommand(cSuccess);
                }
                else
                {
                    sSetChargerStatusAckCommand(cFail);
                }
            }
            else  if(ubChargerStatusTemp==cStopStatus) //其它异常值工步停止
            {
                WorkStep_Service.Req_CMD=WORKSTEP_STOP;
                sSetChargerStatusAckCommand(cSuccess);
            }
            else if(ubChargerStatusTemp==cStepNegPressureNext)//启动工步
            {
                uwStepNumNowTemp=WorkStep_Data.Index+1;
                if((DCDC_Service.Fault_Module==0)&&(Fault_Code.Alarm==0)&&(Fault_Code.Err_Global==0)&&(Fault_Code.Err_Step==0)&&
                   (uwStepNumNowTemp>0)&&(uwStepNumNowTemp<=NUM_WORKSTEP_MAX)&&(WorkStep_Service.Step_State==cWorkStepRunStatus)&&(WorkStep_Data.Type==cStepNegPressure))

                {
                    WorkStep_Service.Req_CMD_OnRunning=WORKSTEP_RUN;
                    WorkStep_Service.Req_WorkStep_Index=uwStepNumNowTemp;
                    WorkStep_Service.Req_Stop_Socket_Err=FALSE;

                    WorkStep_Data.Vol_Interval_FreshFlag=TRUE;
                    WorkStep_Data.Prot_Step_Vol_Interval_Pos.Last_Value = Image_StepEnd.Record.wBatSenseAVoltNew_UP;
                    WorkStep_Data.Prot_Step_Vol_Interval_Neg.Last_Value = Image_StepEnd.Record.wBatSenseAVoltNew_UP;
                    sSetChargerStatusAckCommand(cSuccess);
                }
                else
                {
                    sSetChargerStatusAckCommand(cFail);
                }
            }
            else
            {
                sSetChargerStatusAckCommand(cFail);
            }
        }
        else
        {
            sSetChargerStatusAckCommand(cFail);
        }
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}



//------------------------系统版本回复指令-------------------//
void sSysVersionReplyCommand(void)
{
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex=0;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x0A;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSysVersionReply;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubVerbNum[0];
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubVerbNum[1];
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubVerbNum[2];
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubVerbNum[3];
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubVerbNum[4];
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

//------------------------系统版本查询指令-------------------//
void sSysVersionInquireCommand(void)
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
        sSysVersionReplyCommand();
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}

//------------------------系统柜机号回复指令-------------------//
void sSysCabNumReplyCommand(void)
{
    INT16U uwCabinetsNumTemp = 0;
    INT8U ubCabinetsNumTemp0 = 0;
    INT8U ubCabinetsNumTemp1 = 0;
    INT16U uwModuleAddressTemp = 0;
    INT8U ubModuleAddressTemp0 = 0;
    INT8U ubModuleAddressTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex=0;

    uwCabinetsNumTemp = Num_Cabinet;//sGetCabinetsNum();
    ubCabinetsNumTemp0 = (INT8U)((uwCabinetsNumTemp&0xFF00)>>8);
    ubCabinetsNumTemp1 = (INT8U)(uwCabinetsNumTemp&0x00FF);
    uwModuleAddressTemp = Addr_Module;//sGetModuleAddress();
    ubModuleAddressTemp0 = (INT8U)((uwModuleAddressTemp&0xFF00)>>8);
    ubModuleAddressTemp1 = (INT8U)(uwModuleAddressTemp&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x09;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSysCabAddressReply;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCabinetsNumTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCabinetsNumTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubModuleAddressTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubModuleAddressTemp1;
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

//------------------------系统柜机号查询指令-------------------//
void sSysCabNumInquireCommand(void)
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
        sSysCabNumReplyCommand();
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}


void sSysTimeSetACKCommand(INT8U ubAck)
{
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex=0;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x06;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSysTimeSetACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubAck;
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

//------------------------系统时间设定指令------------------------//
void sSysTimeSetCommand(void)
{
    long long DateTime_Temp=0;
    long long Temp=0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[11];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[12];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if((uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)&&((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)))
    {
        DateTime_Temp=TCP_Socket[SERV_TCP_WORK].Data_RX.Data[10];
        Temp=TCP_Socket[SERV_TCP_WORK].Data_RX.Data[9];
        Temp=Temp<<8;
        DateTime_Temp=DateTime_Temp+Temp;

        Temp=TCP_Socket[SERV_TCP_WORK].Data_RX.Data[8];
        Temp=Temp<<16;
        DateTime_Temp=DateTime_Temp+Temp;

        Temp=TCP_Socket[SERV_TCP_WORK].Data_RX.Data[7];
        Temp=Temp<<24;
        DateTime_Temp=DateTime_Temp+Temp;

        Temp=TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
        Temp=Temp<<32;
        DateTime_Temp=DateTime_Temp+Temp;

        Temp=TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
        Temp=Temp<<40;
        DateTime_Temp=DateTime_Temp+Temp;

        DINT;
        DateTime=DateTime_Temp;
        EINT;

        sSysTimeSetACKCommand(cSuccess);
    }
    else
    {
        sSysTimeSetACKCommand(cFail);
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}


//-----------------心跳包--------------------------
void sLifeSingalReload(void)
{
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcTemp==uwTcpCmdCrcCheck)
    {


        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x05; //字节长度5
        uwTcpipIndex = uwTcpipIndex+1;
        TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cLifeSingalACK; //父功能
        uwTcpipIndex = uwTcpipIndex+1;
        uwTcpCmdCrcCheck = sCRC16N(TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data,uwTcpipIndex);
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
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}

//------------------------其他-----------------------//
void sTcpipNullCommand(void)
{
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}

//*********************************************************************************
//Description: 单字节CRC16校验
//Parameters :
//Return     :
//*********************************************************************************
Uint16 sCRC16(unsigned char ubDataIn,Uint16 uwCrcIn)
{
    Uint16 i;
    Uint16 uwTempCrc;
    uwTempCrc = uwCrcIn^(ubDataIn&0x00FF);
    for(i = 0; i < 8; i++)
    {
        if((uwTempCrc&0x0001)!=0 )
        {
            uwTempCrc =(uwTempCrc>>1)^0xA001;
        }
        else
        {
            uwTempCrc = uwTempCrc>>1;
        }
    }
    return uwTempCrc;
}

//*********************************************************************************
//Description: N字节CRC16校验
//Parameters :
//Return     :
//*********************************************************************************
Uint16 sCRC16N(unsigned char *Buffer,Uint16 uwlen)
{
    Uint16 i;
    Uint16 uwCrcTemp = 0xFFFF;
    for(i=0;i<uwlen;i++)
    {
        uwCrcTemp = sCRC16(*Buffer++,uwCrcTemp);
    }
    return uwCrcTemp;
}

//File end
