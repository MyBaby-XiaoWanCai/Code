/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_can.c
** Author      : Huang Cheng
** Date        : 2022-01-06
** Description : bsp_can source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "can.h"
#include "bsp_can.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
CAN_TxHeaderTypeDef CAN_TxMsg;
CAN_RxHeaderTypeDef CAN1_RxMsg;
CAN_RxHeaderTypeDef CAN2_RxMsg;

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/**********************************************************************************************************
**  函 数 名 : CanSendMsg()
**  功能说明 : CAN发送数据函数
**  形    参 : *hcan - CAN类型，*pMsg - 帧数据缓冲区首地址， MsgLen - 帧数据长度，MsgId - 帧ID
**  返 回 值 : 无
**********************************************************************************************************/
void CanSendMsg(CAN_HandleTypeDef *hcan, uint8_t *pMsg, uint8_t MsgLen, uint32_t MsgId)
{
	CAN_TxMsg.DLC = MsgLen;
	CAN_TxMsg.StdId = 0x0000;
	CAN_TxMsg.ExtId = MsgId;
	CAN_TxMsg.IDE = CAN_ID_EXT;
	CAN_TxMsg.RTR = CAN_RTR_DATA;
	
	/* while(HAL_CAN_GetTxMailboxesFreeLevel(&CAN1_HANDLE) == 0)
	** 等待发送完毕后再发送,否则会造成数据丢包,上述写法仅适用于单帧数据发送,
	** 当多帧数据发送时,需采用下面这种写法,否则会造成多帧数据发送乱序的情况
	*/
	while(HAL_CAN_GetTxMailboxesFreeLevel(hcan) < 3);
	
	HAL_CAN_AddTxMessage(hcan, &CAN_TxMsg, pMsg, (uint32_t*)CAN_TX_MAILBOX0);
}

/**********************************************************************************************************
**  函 数 名 : HAL_CAN_RxFifo0MsgPendingCallback()
**  功能说明 : CAN接收中断回调函数
**  形    参 : *hcan - CAN类型
**  返 回 值 : 无
**********************************************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	uint8_t  Can1Data[8] = {0}; 
	uint32_t CanMsgId = 0;
	HAL_StatusTypeDef CanStatus = HAL_OK;
	
	if(hcan == &hcan1)
	{
		CanStatus = HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN1_RxMsg, Can1Data);
		
		if(HAL_OK == CanStatus)
		{
			//判断ID类型
			if(CAN_ID_STD == CAN1_RxMsg.IDE)
			{
				CanMsgId = CAN1_RxMsg.StdId;
			}
			else
			{
				CanMsgId = CAN1_RxMsg.ExtId;
			}
			
			Can_Network_Rx_Process(CanMsgId, Can1Data);
		}
	}
	
	if(hcan == &hcan2)
	{
		CanStatus = HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &CAN2_RxMsg, &DcdcRecvBuffer.ChFlag);
		
		if(HAL_OK == CanStatus)
		{
			//判断ID类型
			if(CAN_ID_STD == CAN2_RxMsg.IDE)
			{
				CanMsgId = CAN2_RxMsg.StdId;
			}
			else
			{
				CanMsgId = CAN2_RxMsg.ExtId;
			}
			
			//判断ID号是否正确
			if(RECV_MSG_ID == CanMsgId)
			{
				//释放CAN接收信号量
				osSemaphoreRelease(SemaDcdcCan);
			}
			else
			{
				//do nothing;
			}
		}
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

