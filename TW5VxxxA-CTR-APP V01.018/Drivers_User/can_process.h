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

extern uint8_t  ACDC_Staste0[2];    //ACDC工作状态，0是主机，1是从机
extern uint8_t  ACDC_Staste1[2];    //ACDC工作状态，0是主机，1是从机
extern uint16_t ACDC_ACV[2];        //ACDC交流电压，0是主机，1是从机  实际值10倍
extern uint16_t ACDC_ACI[2];        //ACAI交流电流，0是主机，1是从机  实际值10倍
extern uint16_t ACDC_DCV[2];        //ACDC直流电压，0是主机，1是从机  实际值100倍，不主动上传
extern uint16_t ACDC_DCI[2];        //ACDI直流电流，0是主机，1是从机  实际值10倍，不主动上传

extern uint16_t DCI;

// #define DCI ACDC_DCI[0]>ACDC_DCI[1]? ACDC_DCI[0] : ACDC_DCI[0];

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
