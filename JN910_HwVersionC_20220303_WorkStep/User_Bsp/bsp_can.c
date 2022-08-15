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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "can.h"
#include "bsp_can.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
CAN_TxHeaderTypeDef CAN_TxMsg;
CAN_RxHeaderTypeDef CAN1_RxMsg;
CAN_RxHeaderTypeDef CAN2_RxMsg;

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/
/**********************************************************************************************************
**  �� �� �� : CanSendMsg()
**  ����˵�� : CAN�������ݺ���
**  ��    �� : *hcan - CAN���ͣ�*pMsg - ֡���ݻ������׵�ַ�� MsgLen - ֡���ݳ��ȣ�MsgId - ֡ID
**  �� �� ֵ : ��
**********************************************************************************************************/
void CanSendMsg(CAN_HandleTypeDef *hcan, uint8_t *pMsg, uint8_t MsgLen, uint32_t MsgId)
{
	CAN_TxMsg.DLC = MsgLen;
	CAN_TxMsg.StdId = 0x0000;
	CAN_TxMsg.ExtId = MsgId;
	CAN_TxMsg.IDE = CAN_ID_EXT;
	CAN_TxMsg.RTR = CAN_RTR_DATA;
	
	/* while(HAL_CAN_GetTxMailboxesFreeLevel(&CAN1_HANDLE) == 0)
	** �ȴ�������Ϻ��ٷ���,�����������ݶ���,����д���������ڵ�֡���ݷ���,
	** ����֡���ݷ���ʱ,�������������д��,�������ɶ�֡���ݷ�����������
	*/
	while(HAL_CAN_GetTxMailboxesFreeLevel(hcan) < 3);
	
	HAL_CAN_AddTxMessage(hcan, &CAN_TxMsg, pMsg, (uint32_t*)CAN_TX_MAILBOX0);
}

/**********************************************************************************************************
**  �� �� �� : HAL_CAN_RxFifo0MsgPendingCallback()
**  ����˵�� : CAN�����жϻص�����
**  ��    �� : *hcan - CAN����
**  �� �� ֵ : ��
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
			//�ж�ID����
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
			//�ж�ID����
			if(CAN_ID_STD == CAN2_RxMsg.IDE)
			{
				CanMsgId = CAN2_RxMsg.StdId;
			}
			else
			{
				CanMsgId = CAN2_RxMsg.ExtId;
			}
			
			//�ж�ID���Ƿ���ȷ
			if(RECV_MSG_ID == CanMsgId)
			{
				//�ͷ�CAN�����ź���
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

