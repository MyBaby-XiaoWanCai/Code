/*
*********************************************************************************************************
*
*	模块名称 : rs485 
*	文件名称 : rs485.c
*	版    本 : V1.0
*	说    明 : 
*			  
*
*	修改记录 :
*		版本号  日期         作者     说明
*		V1.0    2020-03-11   ZYL      正式发布
*********************************************************************************************************
*/

#include "main.h"

uint8_t RxBuffer[20];

/*
*********************************************************************************************************
*	函 数 名: Send_OneByte
*	功能说明: 发送一个字节
*	形    参: TxByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_Send_OneByte(uint8_t TxByte)
{
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart3, (uint8_t *)&TxByte, 1, 0xFFFF);//超时时间0xFF
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
}
/*
*********************************************************************************************************
*	函 数 名: Send_One_MoreByte
*	功能说明: 发送多个字节
*	形    参: TxByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void Send_One_MoreByte(uint8_t *TxMoreByte,uint8_t cnt)
{
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart3, TxMoreByte, cnt, 0xFFFF);//超时时间0xFFFF
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
}
/*
*********************************************************************************************************
*	函 数 名: HAL_UART_RxCpltCallback
*	功能说明: 串口接收中断回调函数
*	形    参: 
*	返 回 值: 无
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
		if(HAL_UART_Receive_IT(&huart3,(uint8_t*)RxBuffer,1)!=HAL_OK)//开启接收中断，准备接收
  	{Error_Handler();}
	}
	
}
/*****************************(END OF FILE) *********************************/
