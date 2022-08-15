/*
 * SCI_BSP.c
 *
 *  Created on: 2021Äê8ÔÂ6ÈÕ
 *      Author: JC
 */
#include "F28x_Project.h"



struct struct_SCI_RX SCIA_RX;
struct struct_SCI_TX SCIA_TX;


void InitSCI_BSP()
{

   // Initialize SCI-A:
   // One stop bit, no parity, 8-bit character length
   SciaRegs.SCICCR.all = 0x07;
   //Enable transmit and receive
   SciaRegs.SCICTL1.all = 0x03;
   //BAUD
 //  SciaRegs.SCIHBAUD=0x14;
  // SciaRegs.SCILBAUD=0x57;//2400
   SciaRegs.SCIHBAUD.all    =0x0002;  // 115200 baud @LSPCLK = 50MHz (200 MHz SYSCLK).
   SciaRegs.SCILBAUD.all    =0x008B;      // SCILBAUD=LSPCLK/(BAUD*8)-1
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


interrupt void SCIAReceiveInterrupt(void)
{
    unsigned char temp;
    if(SciaRegs.SCIRXST.bit.RXRDY==1)
    {
        temp=SciaRegs.SCIRXBUF.bit.SAR;
        SciaRegs.SCITXBUF.all =temp+1;
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
