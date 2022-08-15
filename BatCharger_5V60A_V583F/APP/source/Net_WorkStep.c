/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_WorkStep.c
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//ȫ�ֱ�������


//��������

//------------------------�������ûظ�ָָ��----------------------//
void sSetWorkStepsAckCommand(INT8U ubAck,INT16U StepNum_Set_Bak)
{
    INT16U  uwStepNumTemp = 0;
    INT8U  ubStepNumTemp0 = 0;
    INT8U  ubStepNumTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcCheck0 = 0;
    INT8U   ubTcpCmdCrcCheck1 = 0;
    INT16U  uwTcpipIndex =0;

    uwStepNumTemp = StepNum_Set_Bak;
    ubStepNumTemp0 = (INT8U)((uwStepNumTemp&0xFF00)>>8);
    ubStepNumTemp1 = (INT8U)(uwStepNumTemp&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x08;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetStepACK;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubStepNumTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubStepNumTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubAck;
    uwTcpipIndex = uwTcpipIndex+1;
    //У��λHL
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

//��������
//------------------------����ѭ������ָ��----------------------//
void sSetWorkStepsForCommand(void)
{
	INT16U  uwStepNumTemp = 0;
	INT16U  uwStepNumTemp0 = 0;
	INT16U  uwStepNumTemp1 = 0;
	INT8U   ubStepNameTypeTemp = 0;
	INT8U   ubStepCycleTypeTemp1 = 0;
	INT8U   ubStepCycleTypeTemp2 = 0;
	INT8U	ubTcpCmdCnt = 0;
	INT16U 	uwTcpCmdCrcTemp = 0;
	INT16U 	uwTcpCmdCrcTemp0 = 0;
	INT16U 	uwTcpCmdCrcTemp1 = 0;
	INT16U 	uwTcpCmdCrcCheck = 0;

	ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0]-3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0]-2];
	uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
	uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if((uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)&&((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)))
    {
		uwStepNumTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
		uwStepNumTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
		uwStepNumTemp = (uwStepNumTemp0<<8)+uwStepNumTemp1;
		ubStepNameTypeTemp = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
		ubStepCycleTypeTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
		ubStepCycleTypeTemp2 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[8];
		if(ubStepNameTypeTemp == cStepCycleStart)
		{
			if((ubStepCycleTypeTemp1 == cStepCycleIndex)&&((ubStepCycleTypeTemp2 == cStepCycleTimes)))
			{
				SPI_Flash_Write(SERV_FLASH_NET,(Uint32)(uwStepNumTemp-1)*0x1000,ubTcpCmdCnt,&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0]);
	            sSetWorkStepsAckCommand(cSuccess,uwStepNumTemp);
			}
			else
			{
		        sSetWorkStepsAckCommand(cFail,uwStepNumTemp);
			}
		}
		else if(ubStepNameTypeTemp == cStepCycleEnd)
		{
			if(ubStepCycleTypeTemp1 == cStepCycleIndex)
			{
				SPI_Flash_Write(SERV_FLASH_NET,(Uint32)(uwStepNumTemp-1)*0x1000,ubTcpCmdCnt,&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0]);
				sSetWorkStepsAckCommand(cSuccess,uwStepNumTemp);
			}
			else
			{
		        sSetWorkStepsAckCommand(cFail,uwStepNumTemp);
			}
		}
		else
		{
	        sSetWorkStepsAckCommand(cFail,uwStepNumTemp);
		}
	}
	else
	{
        sSetWorkStepsAckCommand(cFail,uwStepNumTemp);
	}
}




//------------------------��������ָ��----------------------//
void sSetWorkStepsCommand(void)
{
    INT32U  uwStepNumTemp = 0;
    INT16U  uwStepNumTemp0 = 0;
    INT16U  uwStepNumTemp1 = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];      //֡��91  112
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0]-3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0]-2];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if((uwTcpCmdCrcCheck == uwTcpCmdCrcTemp)&&((WorkStep_Service.Step_State==cStopStatus)||(WorkStep_Service.Step_State==cStepComplete)))
    {
        uwStepNumTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
        uwStepNumTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
        uwStepNumTemp = (uwStepNumTemp0<<8)+uwStepNumTemp1;
        if((uwStepNumTemp>=1)&&(uwStepNumTemp<=256))
        {
            //�����޽�ֹ�����ܾ���������
            if((TCP_Socket[SERV_TCP_WORK].Data_RX.Data[35]==0xFF)&&(TCP_Socket[SERV_TCP_WORK].Data_RX.Data[44]==0xFF)&&(TCP_Socket[SERV_TCP_WORK].Data_RX.Data[53]==0xFF)&&(TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4]!=0x0C))
            {
                sSetWorkStepsAckCommand(cFail,uwStepNumTemp);
                Fault_Code.Err_Global      |= cStepDataError;   //��Ҫ��һ��������
            }
            else
            {
                SPI_Flash_Write(SERV_FLASH_NET,(Uint32)(uwStepNumTemp-1)*0x1000,ubTcpCmdCnt,&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0]);
                sSetWorkStepsAckCommand(cSuccess,uwStepNumTemp);
            }
        }
        else
        {sSetWorkStepsAckCommand(cFail,uwStepNumTemp);}
    }
    else
    {
        sSetWorkStepsAckCommand(cFail,uwStepNumTemp);
    }
}

//--------------------------------------------------------
//------------------------������----------------------//
void sSetSetpsCommand(void)
{
    INT8U   ubStepTypeTemp = 0;
    ubStepTypeTemp = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    //д�빤��ʱFlash����ʱ��ϳ�������д���������ݻ�����
    if(Interface_SPI_Flash[SERV_FLASH_NET].Req_RW==REQ_SPI_FLASH_IDLE)
    {
        if((ubStepTypeTemp==cStepCycleStart)||(ubStepTypeTemp==cStepCycleEnd))
        {
            sSetWorkStepsForCommand();
        }
        else if((ubStepTypeTemp<=cStepDW)||(ubStepTypeTemp == cStepDCCtoDCV)||(ubStepTypeTemp==cStepNegPressure))
        {
            sSetWorkStepsCommand();
        }
        TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
    }
}


//File end
