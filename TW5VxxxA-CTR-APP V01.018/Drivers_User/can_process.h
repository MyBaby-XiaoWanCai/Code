/*
*********************************************************************************************************
*
*	ģ������ : can
*	�ļ����� : can_process.h
*	��    �� : V1.0
*	˵    �� :
*	�޸ļ�¼ :
*	�汾��  ����         ����       ˵��
*	V1.0    2020-03-06   ZYL        ��ʽ����
*
*********************************************************************************************************
*/

#ifndef _can_process_H_
#define _can_process_H_

#include "stdint.h"
#include "stm32f4xx_hal_can.h"

extern uint8_t  ACDC_Staste0[2];    //ACDC����״̬��0��������1�Ǵӻ�
extern uint8_t  ACDC_Staste1[2];    //ACDC����״̬��0��������1�Ǵӻ�
extern uint16_t ACDC_ACV[2];        //ACDC������ѹ��0��������1�Ǵӻ�  ʵ��ֵ10��
extern uint16_t ACDC_ACI[2];        //ACAI����������0��������1�Ǵӻ�  ʵ��ֵ10��
extern uint16_t ACDC_DCV[2];        //ACDCֱ����ѹ��0��������1�Ǵӻ�  ʵ��ֵ100�����������ϴ�
extern uint16_t ACDC_DCI[2];        //ACDIֱ��������0��������1�Ǵӻ�  ʵ��ֵ10�����������ϴ�

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
