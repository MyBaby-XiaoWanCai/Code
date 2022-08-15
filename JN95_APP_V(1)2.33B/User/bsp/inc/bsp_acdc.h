/**********************************************************************************************************
*
*	ģ������ : ACtoDC�������
*	�ļ����� : bsp_acdc.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*	��	 �ߣ�Robert.Li
**��	 ��:  2021.7.14
**********************************************************************************************************/

#ifndef _BSP_ACDC_H_
#define _BSP_ACDC_H_

#ifdef __cplusplus
extern "C" {
#endif
/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "stm32f4xx_hal.h"


/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/



/********************************************************************************************************	
HW : JN-MPU-C022F ��CAN1 PIN ӳ���ϵ


    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX

*********************************************************************************************************/


//���Ŷ���
/*******************************************************/
#define CAN_NUM                     CAN1
#define CAN_APB_CLK                 RCC_APB1Periph_CAN1


#define CAN_TX_PIN                  GPIO_Pin_12                 
#define CAN_TX_GPIO_PORT            GPIOA                     
#define CAN_TX_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define CAN_TX_AF                    GPIO_AF_CAN1
#define CAN_TX_SOURCE                GPIO_PinSource12


#define CAN_RX_PIN                  GPIO_Pin_11                 
#define CAN_RX_GPIO_PORT            GPIOA                     
#define CAN_RX_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define CAN_RX_AF                    GPIO_AF_CAN1
#define CAN_RX_SOURCE                GPIO_PinSource11

//NVIC

#define CAN_IRQ_Channel             CAN1_RX0_IRQn
#define CAN_TEST_IRQHandler         CAN1_RX0_IRQHandler   

//2��3����ȵ�FIFO
#define   CAN_FIFO0   CAN_RX_FIFO0
#define   CAN_FIFO1   CAN_RX_FIFO1  //
/*******************************************************/





/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/
typedef enum {
QueryWait=0,    
QueryACV1,
QueryACV2,
QueryACI,
QueryDCV,
QueryDCI,
QueryVERSION1,
QueryVERSION2,    
}QueryWaitState;

typedef struct {
__IO uint8_t  Type;          //ACDC���ͣ�1�����磬2���յ�  
__IO uint16_t  State[2];     //ACDC����״̬��0��Ӧģ��0��1��Ӧģ��1
__IO float ACV[2];           //ACDC������ѹ��0��Ӧģ��0��1��Ӧģ��1 ʵ��ֵV 
__IO float ACI[2];           //ACAI����������0��Ӧģ��0��1��Ӧģ��1 ʵ��ֵA
__IO float DCV[2];           //ACDCֱ����ѹ��0��Ӧģ��0��1��Ӧģ��1 ʵ��ֵV���������ϴ�
__IO float DCI[2];           //ACDIֱ��������0��Ӧģ��0��1��Ӧģ��1 ʵ��ֵA���������ϴ�
__IO uint8_t  FanSpd[2];     //���ת�٣�0��Ӧģ��0��1��Ӧģ��1�����ٷֱ���
__IO uint16_t  BitFlg[2];    //����״̬���£�bit=1����״̬λ�仯��0��Ӧģ��0��1��Ӧģ��1 
__IO uint8_t  VERSION[2][4];    //�汾�ţ��յ�VERSION[x][0]~VERSION[x][1] ��Ӳ���汾�� VERSION[x][2]~VERSION[x][3] ������汾
__IO QueryWaitState QueryWaitFlag;     //ACDC ��ѯ�ȴ���־
}ACDC_Inf;

/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
extern ACDC_Inf ACDC;


/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
extern void bsp_InitACDC(void);
extern void CAN_User_Init(CAN_HandleTypeDef* hcan );
extern void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
extern HAL_StatusTypeDef CANx_SendData(CAN_HandleTypeDef* hcan,uint32_t ID,uint8_t *pData,uint16_t Len);
extern uint8_t ACDC_Fan_Speed51_ModeTest(void);
extern void ACDC_Fan_Speed_CMD(uint8_t type, uint8_t addr, uint8_t Speed);
extern void ACDC_DCV_Query_CMD(uint8_t type, uint8_t addr);
extern void ACDC_DCI_Query_CMD(uint8_t type, uint8_t addr);
extern void ACDC_ACV_Query_CMD(uint8_t type, uint8_t addr);
extern void ACDC_ACI_Query_CMD(uint8_t type, uint8_t addr);
extern void ACDC_ACF_Query_CMD(uint8_t type, uint8_t addr);
extern void ACDC_Fault_Clear_CMD(uint8_t type, uint8_t addr);
extern void ACDC_FW_VERSION_CMD(uint8_t type, uint8_t addr);


#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
