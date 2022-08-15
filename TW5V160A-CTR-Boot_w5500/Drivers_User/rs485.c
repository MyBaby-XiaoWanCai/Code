/*
*********************************************************************************************************
*
*	ģ������ : rs485 
*	�ļ����� : rs485.c
*	��    �� : V1.0
*	˵    �� : 
*			  
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0    2020-03-11   ZYL      ��ʽ����
*********************************************************************************************************
*/

#include "main.h"

uint8_t RxBuffer[20];

/*
*********************************************************************************************************
*	�� �� ��: Send_OneByte
*	����˵��: ����һ���ֽ�
*	��    ��: TxByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_Send_OneByte(uint8_t TxByte)
{
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart3, (uint8_t *)&TxByte, 1, 0xFFFF);//��ʱʱ��0xFF
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
}
/*
*********************************************************************************************************
*	�� �� ��: Send_One_MoreByte
*	����˵��: ���Ͷ���ֽ�
*	��    ��: TxByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Send_One_MoreByte(uint8_t *TxMoreByte,uint8_t cnt)
{
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart3, TxMoreByte, cnt, 0xFFFF);//��ʱʱ��0xFFFF
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
}
/*
*********************************************************************************************************
*	�� �� ��: HAL_UART_RxCpltCallback
*	����˵��: ���ڽ����жϻص�����
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t Rx_count_USART3=0;
uint8_t Rxbuffer[10];
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART3)
	{
		Rxbuffer[Rx_count_USART3]=RxBuffer[0];
	  if((Rxbuffer[0]==0x0D)&&(Rxbuffer[1]==0x0A))
		{
			
		 //Rx_Num_USART3=++Rx_count_USART3;
		 Rx_count_USART3=0;
		}
		else Rx_count_USART3++;
		if(HAL_UART_Receive_IT(&huart3,(uint8_t*)RxBuffer,1)!=HAL_OK)//���������жϣ�׼������
  	{Error_Handler();}
	}
	
}
/*****************************(END OF FILE) *********************************/
