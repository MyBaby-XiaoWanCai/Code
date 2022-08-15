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

struct_Active_Report       Active_Report;//主动上报数据周期


volatile unsigned long i;
volatile uint32_t txMsgCount = 0;
volatile uint32_t rxMsgCount = 0;
volatile uint32_t errorFlag = 0;

tCANMsgObject sTXCANMessage;
tCANMsgObject sRXCANMessage;
tCANMsgObject sTXCAN_JN910;
tCANMsgObject sRXCAN_JN910;
tCANMsgObject sTXRecord_JN910;

canQue_Typedef      que1Con;
canQue_Typedef      que2Con;

Record_Que_Typedef  que5Con;
CAN_APP_PACK pack;
CAN_APP_PACK pack_tx;
CAN_APP_PACK pack_tx_record;

//CAN队列进入初始化
void    BSP_CAN_Que_Init(canQue_Typedef *que)
{
    que->OSQStart        = 0;
    que->OSQEnd          = QUE_CAN_BUF_SIZE;
    que->OSQIn           = 0;
    que->OSQOut          = 0;
    que->OSQSize         = QUE_CAN_BUF_SIZE;
    que->OSQEntries      = 0;
}

//录波队列初始化
void  BSP_Record_Que_Init(Record_Que_Typedef *que)
{
    que->OSQStart        = 0;
    que->OSQEnd          = QUE_Record_BUF_SIZE;
    que->OSQIn           = 0;
    que->OSQOut          = 0;
    que->OSQSize         = QUE_Record_BUF_SIZE;
    que->OSQEntries      = 0;
}

//CAN初始化
void InitCAN_BSP(void)
{

    //CANB 初始化
    //GPIO39 -  CANRXB
     GPIO_SetupPinMux(39, GPIO_MUX_CPU1, 6);
     GPIO_SetupPinOptions(39, GPIO_INPUT, GPIO_ASYNC);
     //GPIO38 - CANTXB
     GPIO_SetupPinMux(38, GPIO_MUX_CPU1, 6);
     GPIO_SetupPinOptions(38, GPIO_OUTPUT, GPIO_PUSHPULL);
     // Initialize the CAN controller
     CANInit(CANB_BASE);

     // Setup CAN to be clocked off the M3/Master subsystem clock
     CANClkSourceSelect(CANB_BASE, 0);

     CANBitRateSet(CANB_BASE, 200000000, 500000);
    // CANIntEnable(CANA_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
     CANIntEnable(CANB_BASE, CAN_INT_MASTER | CAN_INT_ERROR );

     //CANA 初始化
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





     sRXCANMessage.ui32MsgID =0x00400000;
     sRXCANMessage.ui32MsgIDMask = 0;
     sRXCANMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE;
     sRXCANMessage.ui32MsgLen = MSG_DATA_LENGTH;
     sRXCANMessage.pucMsgData = &sRXCAN_JN910.Data[0];
     CANMessageSet(CANB_BASE, RX_MSG_OBJ_ID, &sRXCANMessage,
                   MSG_OBJ_TYPE_RX);
     CANGlobalIntEnable(CANB_BASE, CAN_GLB_INT_CANINT0);

         //
         // Start CAN module A and B operations
         //
     CANEnable(CANB_BASE);


         //
         // Transmit messages from CAN-A to CAN-B
         //
     BSP_CAN_Que_Init(&que1Con);
     BSP_CAN_Que_Init(&que2Con);
     BSP_Record_Que_Init(&que5Con);


}


//队列循环保存

int16_t  BSP_CAN_Que_In(canQue_Typedef *que,tCANMsgObject *inMsg)
{
        if (que->OSQEntries >= que->OSQSize)
        {

                que->buf[que->OSQIn].id     =   inMsg->ui32MsgID;
                que->buf[que->OSQIn].data[0] =  inMsg->Data[0];
                que->buf[que->OSQIn].data[1] =  inMsg->Data[1];
                que->buf[que->OSQIn].data[2] =  inMsg->Data[2];
                que->buf[que->OSQIn].data[3] =  inMsg->Data[3];
                que->buf[que->OSQIn].data[4] =  inMsg->Data[4];
                que->buf[que->OSQIn].data[5] =  inMsg->Data[5];
                que->buf[que->OSQIn].data[6] =  inMsg->Data[6];
                que->buf[que->OSQIn].data[7] =  inMsg->Data[7];

                que->OSQIn++;
                if (que->OSQIn == que->OSQEnd)
                {
                    que->OSQIn = que->OSQStart;
                }
                que->OSQOut++;
                if (que->OSQOut == que->OSQEnd)
                {
                    que->OSQOut = que->OSQStart;
                }
                return  DEF_CAN_QUE_OK;

        }
        else
        {
                que->buf[que->OSQIn].id     =   inMsg->ui32MsgID;
                que->buf[que->OSQIn].data[0] =  inMsg->Data[0];
                que->buf[que->OSQIn].data[1] =  inMsg->Data[1];
                que->buf[que->OSQIn].data[2] =  inMsg->Data[2];
                que->buf[que->OSQIn].data[3] =  inMsg->Data[3];
                que->buf[que->OSQIn].data[4] =  inMsg->Data[4];
                que->buf[que->OSQIn].data[5] =  inMsg->Data[5];
                que->buf[que->OSQIn].data[6] =  inMsg->Data[6];
                que->buf[que->OSQIn].data[7] =  inMsg->Data[7];

                que->OSQIn++;
                que->OSQEntries++;
                if (que->OSQIn == que->OSQEnd)
                {
                     que->OSQIn = que->OSQStart;
                }
      }
      return  DEF_CAN_QUE_OK;
}

//队列移出数据
int16_t  BSP_CAN_Que_Out(canQue_Typedef *que, CAN_APP_PACK *outData)
{
        if (que->OSQEntries != 0)
        {
                outData->id    = que->buf[que->OSQOut].id;
                outData->flen  = que->buf[que->OSQOut].flen;
                outData->stamp = que->buf[que->OSQOut].stamp;

                outData->data[0] = que->buf[que->OSQOut].data[0];
                outData->data[1] = que->buf[que->OSQOut].data[1];
                outData->data[2] = que->buf[que->OSQOut].data[2];
                outData->data[3] = que->buf[que->OSQOut].data[3];
                outData->data[4] = que->buf[que->OSQOut].data[4];
                outData->data[5] = que->buf[que->OSQOut].data[5];
                outData->data[6] = que->buf[que->OSQOut].data[6];
                outData->data[7] = que->buf[que->OSQOut].data[7];

                que->OSQOut++;
                que->OSQEntries--;
                if (que->OSQOut == que->OSQEnd)
                {
                    que->OSQOut = que->OSQStart;
                }
                return  DEF_CAN_QUE_OK;
      }
        else
        {
                return  DEF_CAN_QUE_EMPTY;
      }
}

int16_t  BSP_Record_Que_In(Record_Que_Typedef *que,tCANMsgObject *inMsg)
{
        if (que->OSQEntries >= que->OSQSize)
        {

                que->buf[que->OSQIn].id     =   inMsg->ui32MsgID;
                que->buf[que->OSQIn].data[0] =  inMsg->Data[0];
                que->buf[que->OSQIn].data[1] =  inMsg->Data[1];
                que->buf[que->OSQIn].data[2] =  inMsg->Data[2];
                que->buf[que->OSQIn].data[3] =  inMsg->Data[3];
                que->buf[que->OSQIn].data[4] =  inMsg->Data[4];
                que->buf[que->OSQIn].data[5] =  inMsg->Data[5];
                que->buf[que->OSQIn].data[6] =  inMsg->Data[6];
                que->buf[que->OSQIn].data[7] =  inMsg->Data[7];

                que->OSQIn++;
                if (que->OSQIn == que->OSQEnd)
                {
                    que->OSQIn = que->OSQStart;
                }
                que->OSQOut++;
                if (que->OSQOut == que->OSQEnd)
                {
                    que->OSQOut = que->OSQStart;
                }
                return  DEF_CAN_QUE_OK;

        }
        else
        {
                que->buf[que->OSQIn].id     =   inMsg->ui32MsgID;
                que->buf[que->OSQIn].data[0] =  inMsg->Data[0];
                que->buf[que->OSQIn].data[1] =  inMsg->Data[1];
                que->buf[que->OSQIn].data[2] =  inMsg->Data[2];
                que->buf[que->OSQIn].data[3] =  inMsg->Data[3];
                que->buf[que->OSQIn].data[4] =  inMsg->Data[4];
                que->buf[que->OSQIn].data[5] =  inMsg->Data[5];
                que->buf[que->OSQIn].data[6] =  inMsg->Data[6];
                que->buf[que->OSQIn].data[7] =  inMsg->Data[7];

                que->OSQIn++;
                que->OSQEntries++;
                if (que->OSQIn == que->OSQEnd)
                {
                     que->OSQIn = que->OSQStart;
                }
      }
      return  DEF_CAN_QUE_OK;
}



int16_t  BSP_Record_Que_Out(Record_Que_Typedef *que, CAN_APP_PACK *outData)
{
        if (que->OSQEntries != 0)
        {
                outData->id    = que->buf[que->OSQOut].id;
                outData->flen  = que->buf[que->OSQOut].flen;
                outData->stamp = que->buf[que->OSQOut].stamp;

                outData->data[0] = que->buf[que->OSQOut].data[0];
                outData->data[1] = que->buf[que->OSQOut].data[1];
                outData->data[2] = que->buf[que->OSQOut].data[2];
                outData->data[3] = que->buf[que->OSQOut].data[3];
                outData->data[4] = que->buf[que->OSQOut].data[4];
                outData->data[5] = que->buf[que->OSQOut].data[5];
                outData->data[6] = que->buf[que->OSQOut].data[6];
                outData->data[7] = que->buf[que->OSQOut].data[7];

                que->OSQOut++;
                que->OSQEntries--;
                if (que->OSQOut == que->OSQEnd)
                {
                    que->OSQOut = que->OSQStart;
                }
                return  DEF_CAN_QUE_OK;
      }
        else
        {
                return  DEF_CAN_QUE_EMPTY;
      }
}


void CAN_BSP(CAN_APP_PACK* pack_tx)
{
    sTXCANMessage.ui32MsgID = pack_tx->id;
    sTXCANMessage.ui32MsgIDMask = 0;
    sTXCANMessage.ui32Flags = MSG_OBJ_EXTENDED_ID;
    sTXCANMessage.ui32MsgLen = 8;
    sTXCANMessage.pucMsgData = &(pack_tx->data[0]);

    CANMessageSet(CANB_BASE, TX_MSG_OBJ_ID, &sTXCANMessage,
                             MSG_OBJ_TYPE_TX);


}


__interrupt void canbISR(void)
{


    uint32_t status;

    //
    // Read the CAN-B interrupt status to find the cause of the interrupt
    //
    status = CANIntStatus(CANB_BASE, CAN_INT_STS_CAUSE);

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
        status = CANStatusGet(CANB_BASE, CAN_STS_CONTROL);

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
        CANMessageGet(CANB_BASE, RX_MSG_OBJ_ID, &sRXCANMessage, true);
        if(BSP_CAN_Que_In(&que1Con,&sRXCAN_JN910) == DEF_CAN_QUE_FULL)
        {

        }
        //
        // Getting to this point means that the RX interrupt occurred on
        // message object 1, and the message RX is complete.  Clear the
        // message object interrupt.
        //
        CANIntClear(CANB_BASE, RX_MSG_OBJ_ID);


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
    CANGlobalIntClear(CANB_BASE, CAN_GLB_INT_CANINT0);

    //
    // Acknowledge this interrupt located in group 9
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}
