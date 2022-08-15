/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: SCI_BSP.c
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义
struct struct_SCI_RX SCIA_RX;
struct struct_SCI_TX SCIA_TX;
unsigned char  ubVerbNum[5]=VER_FIRMWARE;


//函数声明
/******************************************************************************/
//Function name :
//Description   :
//Parameters    : None
//Return        : None
/******************************************************************************/
void SCIA_Start_TX(void)
{
    SCIA_TX.State_TX=TRUE;
    SCIA_TX.Cnt_Sended    =0;
    SciaRegs.SCICTL2.bit.TXINTENA=1;
    SciaRegs.SCITXBUF=SCIA_TX.Data[SCIA_TX.Cnt_Sended++];
}



//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void SCI_Service(void)
{

}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void InitSCI_BSP()
{
   InitSciaGpio();
   // Initialize SCI-A:
   // One stop bit, no parity, 8-bit character length
   SciaRegs.SCICCR.all = 0x07;
   //Enable transmit and receive
   SciaRegs.SCICTL1.all = 0x03;
   //BAUD
   SciaRegs.SCIHBAUD=0x14;
   SciaRegs.SCILBAUD=0x57;//2400
   //Enable Receive interrupt
   SciaRegs.SCICTL2.all = 0x03;
   //Relinquish SCI from Reset
   SciaRegs.SCICTL1.all = 0x0023;

   SCIA_RX.Cnt_RX=0;
   SCIA_RX.Cnt_Timeout=0;
   SCIA_RX.State_Recived=FALSE;
   SCIA_RX.Num_Timeout=100;//100mS

   SCIA_TX.Cnt_Sended=0;
   SCIA_TX.State_TX=FALSE;
   SCIA_TX.Num_To_TX=0;
}

/******************************************************************************/
//Function name : SCIAReceiveInterrupt
//Description   :
//Parameters    : None
//Return        : None
/******************************************************************************/
//#pragma CODE_SECTION(SCIAReceiveInterrupt,"ramfuncs");
interrupt void SCIAReceiveInterrupt(void)
{
    unsigned char temp;
    if(SciaRegs.SCIRXST.bit.RXRDY==1)
    {
        temp=SciaRegs.SCIRXBUF.bit.RXDT;
        if(SCIA_RX.State_Recived==FALSE)
        {
           if(SCIA_RX.Cnt_RX<LEN_SCI_BUFF)
           {
               SCIA_RX.Data[SCIA_RX.Cnt_RX++]=temp;
               if((SCIA_RX.Cnt_RX>=SCIA_RX.Data[0])||(SCIA_RX.Cnt_RX>=LEN_SCI_BUFF))
               {
                   SCIA_RX.State_Recived=TRUE;
               }
           }
        }
        SCIA_RX.Cnt_Timeout=0;
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}




/******************************************************************************/
//Function name : SCIATransmitInterrupt
//Description   :
//Parameters    : None
//Return        : None
/******************************************************************************/
//#pragma CODE_SECTION(SCIATransmitInterrupt,"ramfuncs");
interrupt void SCIATransmitInterrupt(void)
{
    if(SciaRegs.SCICTL2.bit.TXRDY==1)
    {
        if(SCIA_TX.State_TX==TRUE)
        {
            if((SCIA_TX.Cnt_Sended<SCIA_TX.Num_To_TX)&&(SCIA_TX.Cnt_Sended<LEN_SCI_BUFF))
            {
                SciaRegs.SCITXBUF=SCIA_TX.Data[SCIA_TX.Cnt_Sended++];
            }
            else
            {
                SCIA_TX.Cnt_Sended    =0;
                SCIA_TX.State_TX      =FALSE;
                SciaRegs.SCICTL2.bit.TXINTENA=0;
            }
        }
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}


//File end
