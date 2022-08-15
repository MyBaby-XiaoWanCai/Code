/*
*********************************************************************************************************
*
*	模块名称 : can 
*	文件名称 : can_process.h
*	版    本 : V1.0
*	说    明 :
*	修改记录 :
*	版本号  日期         作者       说明
*	V1.0    2020-03-06   ZYL        正式发布
*
*********************************************************************************************************
*/

#ifndef _can_process_H_
#define _can_process_H_

#include "stdint.h"
#include "stm32f4xx_hal_can.h"

extern void CAN_User_Init(CAN_HandleTypeDef* hcan );
extern void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
extern void CANx_SendData(CAN_HandleTypeDef* hcan,uint32_t ID,uint8_t *pData,uint16_t Len);

extern void ACDC_ONOFF_CMD(uint8_t addr,uint8_t ONOFF);
extern void ACDC_Fan_Speed_CMD(uint8_t addr,uint8_t Speed);
extern void ACDC_DCV_Query_CMD(uint8_t addr);
extern void ACDC_DCI_Query_CMD(uint8_t addr);
extern void ACDC_ACV_Query_CMD(uint8_t addr);
extern void ACDC_ACI_Query_CMD(uint8_t addr);
extern void ACDC_ACF_Query_CMD(uint8_t addr);
extern void ACDC_Fault_Clear_CMD(uint8_t addr);
#endif

/***************************** (END OF FILE) *********************************/
