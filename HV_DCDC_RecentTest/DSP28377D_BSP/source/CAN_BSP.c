/*
 * CAN_BSP.c
 *
 *  Created on: 2021年6月5日
 *      Author: JC
 */
#include "F28x_Project.h"
#include "inc/hw_can.h"
#include "inc/hw_memmap.h"
#include "driverlib/can.h"

//全局变量定义
struct_CAN_JN910_TX        CAN_JN910_TX;
struct_CAN_JN910_TX        CAN_JN910_TX_Error;
struct_CAN_JN910_RX        CAN_JN910_RX;
struct_CAN_JN910_RX        CAN_JN910_RX_Broadcast;
struct_Active_Report        Active_Report;//主动上报数据周期


volatile unsigned long i;
volatile uint32_t txMsgCount = 0;
volatile uint32_t rxMsgCount = 0;
volatile uint32_t errorFlag = 0;
unsigned char txMsgData[4];
unsigned char rxMsgData[8];
tCANMsgObject sTXCANMessage;
tCANMsgObject sRXCANMessage;
void InitCAN_BSP(void)
{
    //GPIO62 -  CANRXA
     GPIO_SetupPinMux(62, GPIO_MUX_CPU1, 6);
     GPIO_SetupPinOptions(62, GPIO_INPUT, GPIO_ASYNC);
     //GPIO63 - CANTXA
     GPIO_SetupPinMux(63, GPIO_MUX_CPU1, 6);
     GPIO_SetupPinOptions(63, GPIO_OUTPUT, GPIO_PUSHPULL);
     // Initialize the CAN controller
     CANInit(CANA_BASE);

     // Setup CAN to be clocked off the M3/Master subsystem clock
     CANClkSourceSelect(CANA_BASE, 0);

     CANBitRateSet(CANA_BASE, 200000000, 500000);
    // CANIntEnable(CANA_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
     CANIntEnable(CANA_BASE, CAN_INT_MASTER | CAN_INT_ERROR );
     union union_ID_CAN ID_CAN;

     ID_CAN.ID_JN910.NC                         =0x00;
     ID_CAN.ID_JN910.Priority_ID                =0x00;
     ID_CAN.ID_JN910.Devices_Type               =0x00;
     ID_CAN.ID_JN910.Frame_Seque                =0x40;
     ID_CAN.ID_JN910.Src_Addr                   =0x00;
     ID_CAN.ID_JN910.Dst_Addr                   =0x00;



     sRXCANMessage.ui32MsgID =ID_CAN.All;
     sRXCANMessage.ui32MsgIDMask = 0;
     sRXCANMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE;
     sRXCANMessage.ui32MsgLen = MSG_DATA_LENGTH;
     sRXCANMessage.pucMsgData = rxMsgData;
     CANMessageSet(CANA_BASE, RX_MSG_OBJ_ID, &sRXCANMessage,
                   MSG_OBJ_TYPE_RX);
     CANGlobalIntEnable(CANA_BASE, CAN_GLB_INT_CANINT0);
     /*  txMsgData[0] = 0x12;
         txMsgData[1] = 0x34;
         txMsgData[2] = 0x56;
         txMsgData[3] = 0x78;*/

         //
         // Start CAN module A and B operations
         //
         CANEnable(CANA_BASE);


         //
         // Transmit messages from CAN-A to CAN-B
         //
}

void CAN_BSP(struct_CAN_JN910_TX * CAN_JN910_TX)
{
    sTXCANMessage.ui32MsgID = CAN_JN910_TX->ID_CAN.All;
    sTXCANMessage.ui32MsgIDMask = 0;
    sTXCANMessage.ui32Flags = 0;
    sTXCANMessage.ui32MsgLen = CAN_JN910_TX->Length_Data;
    sTXCANMessage.pucMsgData = CAN_JN910_TX->pucMsgData;
    CANMessageSet(CANA_BASE, TX_MSG_OBJ_ID, &sTXCANMessage,
                             MSG_OBJ_TYPE_TX);


}
__interrupt void
canbISR(void)
{


    uint32_t status;

    //
    // Read the CAN-B interrupt status to find the cause of the interrupt
    //
    status = CANIntStatus(CANA_BASE, CAN_INT_STS_CAUSE);

    //
    // If the cause is a controller status interrupt, then get the status
    //
    if(status == CAN_INT_INT0ID_STATUS)
    {
        //
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors.  Error processing
        // is not done in this example for simplicity.  Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.
        //
        status = CANStatusGet(CANA_BASE, CAN_STS_CONTROL);

        //
        // Check to see if an error occurred.
        //
        if(((status  & ~(CAN_ES_RXOK)) != 7) &&
           ((status  & ~(CAN_ES_RXOK)) != 0))
        {
            //
            // Set a flag to indicate some errors may have occurred.
            //
            errorFlag = 1;
        }
    }
    //
    // Check if the cause is the CAN-B receive message object 1
    //
    else if(status == RX_MSG_OBJ_ID)
    {
        //
        // Get the received message
        //
        CANMessageGet(CANA_BASE, RX_MSG_OBJ_ID, &sRXCANMessage, true);
        if(CAN_JN910_RX_Broadcast.Flag_RX==FALSE)
        {
            if(CAN_JN910_RX_Broadcast.Flag_Receiving==FALSE)
            {
                 if((rxMsgData[0]==0x5a)&&(rxMsgData[1]==0x5a))
                 {
                        Uint16 Temp;
                        CAN_JN910_RX_Broadcast.Data[0]=rxMsgData[0];
                        CAN_JN910_RX_Broadcast.Data[1]=rxMsgData[1];
                        CAN_JN910_RX_Broadcast.Data[2]=rxMsgData[2];
                        CAN_JN910_RX_Broadcast.Data[3]=rxMsgData[3];
                        CAN_JN910_RX_Broadcast.Data[4]=rxMsgData[4];
                        CAN_JN910_RX_Broadcast.Data[5]=rxMsgData[5];
                        CAN_JN910_RX_Broadcast.Data[6]=rxMsgData[6];
                        CAN_JN910_RX_Broadcast.Data[7]=rxMsgData[7];
                        CAN_JN910_RX_Broadcast.Length_Data   =(CAN_JN910_RX_Broadcast.Data[2]<<8)+CAN_JN910_RX_Broadcast.Data[3];
                        CAN_JN910_RX_Broadcast.Num_To_RX     =CAN_JN910_RX_Broadcast.Length_Data+4;
                        CAN_JN910_RX_Broadcast.Cnt_Byte_RX   =8;
                        CAN_JN910_RX_Broadcast.Flag_Receiving=TRUE;


                 }
            }
            else
            {
                 if(CAN_JN910_RX_Broadcast.Cnt_Byte_RX<LEN_BUFF_CAN-8)
                 {
                       CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX++]=rxMsgData[0];
                       CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX++]=rxMsgData[1];
                       CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX++]=rxMsgData[2];
                       CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX++]=rxMsgData[3];
                       CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX++]=rxMsgData[4];
                       CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX++]=rxMsgData[5];
                       CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX++]=rxMsgData[6];
                       CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX++]=rxMsgData[7];
                       CAN_JN910_RX_Broadcast.Cnt_Timeout=0;
                       if(CAN_JN910_RX_Broadcast.Cnt_Byte_RX>=CAN_JN910_RX_Broadcast.Num_To_RX)
                       {
                             CAN_JN910_RX_Broadcast.Cnt_Byte_RX=CAN_JN910_RX_Broadcast.Num_To_RX;

                             if((CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX-1]==0xa5)&&(CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX-2]==0xa5))
                             {
                                   CAN_JN910_RX_Broadcast.Flag_RX=TRUE;
                                   CAN_JN910_RX_Broadcast.Flag_Receiving=FALSE;
                                   CAN_JN910_RX_Broadcast.Error_Code=CAN_OK;
                             }
                             else
                             {
                                   CAN_JN910_RX_Broadcast.Flag_RX         =TRUE;
                                   CAN_JN910_RX_Broadcast.Flag_Receiving  =FALSE;
                                   CAN_JN910_RX_Broadcast.Error_Code      =CAN_ERR_DATA_LEN_CHECK;
                             }
                        }
                  }
                  else
                  {
                        //数据帧格式错误,上层去处理
                        CAN_JN910_RX_Broadcast.Flag_RX=TRUE;
                        CAN_JN910_RX_Broadcast.Flag_Receiving=FALSE;
                        CAN_JN910_RX_Broadcast.Error_Code=CAN_ERR_PACKAGE;
                  }
             }



        }
        //
        // Getting to this point means that the RX interrupt occurred on
        // message object 1, and the message RX is complete.  Clear the
        // message object interrupt.
        //
        CANIntClear(CANA_BASE, RX_MSG_OBJ_ID);


        //
        // Increment a counter to keep track of how many messages have been
        // received. In a real application this could be used to set flags to
        // indicate when a message is received.
        //
        rxMsgCount++;

        //
        // Since the message was received, clear any error flags.
        //
        errorFlag = 0;
    }
    //
    // If something unexpected caused the interrupt, this would handle it.
    //
    else
    {
        //
        // Spurious interrupt handling can go here.
        //
    }

    //
    // Clear the global interrupt flag for the CAN interrupt line
    //
    CANGlobalIntClear(CANA_BASE, CAN_GLB_INT_CANINT0);

    //
    // Acknowledge this interrupt located in group 9
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}
