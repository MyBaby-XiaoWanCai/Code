/**********************************************************************************************************
*
*	模块名称 : ACtoDC风机驱动
*	文件名称 : bsp_acdc.h
*	版    本 : V1.0
*	说    明 : 头文件
*	作	 者：Robert.Li
**日	 期:  2021.7.14
**********************************************************************************************************/

#ifndef _BSP_ACDC_H_
#define _BSP_ACDC_H_

#ifdef __cplusplus
extern "C" {
#endif
/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "stm32f4xx_hal.h"


/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/



/********************************************************************************************************	
HW : JN-MPU-C022F 　CAN1 PIN 映射关系


    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX

*********************************************************************************************************/


//引脚定义
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

//2个3级深度的FIFO
#define   CAN_FIFO0   CAN_RX_FIFO0
#define   CAN_FIFO1   CAN_RX_FIFO1  //
/*******************************************************/





/**********************************************************************************************************
**                                           类型定义区
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
__IO uint8_t  Type;          //ACDC类型，1：国电，2：普德  
__IO uint16_t  State[2];     //ACDC工作状态，0对应模块0，1对应模块1
__IO float ACV[2];           //ACDC交流电压，0对应模块0，1对应模块1 实际值V 
__IO float ACI[2];           //ACAI交流电流，0对应模块0，1对应模块1 实际值A
__IO float DCV[2];           //ACDC直流电压，0对应模块0，1对应模块1 实际值V，不主动上传
__IO float DCI[2];           //ACDI直流电流，0对应模块0，1对应模块1 实际值A，不主动上传
__IO uint8_t  FanSpd[2];     //风机转速，0对应模块0，1对应模块1，按百分比算
__IO uint16_t  BitFlg[2];    //工作状态更新，bit=1则有状态位变化，0对应模块0，1对应模块1 
__IO uint8_t  VERSION[2][4];    //版本号，普德VERSION[x][0]~VERSION[x][1] ：硬件版本， VERSION[x][2]~VERSION[x][3] ：软件版本
__IO QueryWaitState QueryWaitFlag;     //ACDC 查询等待标志
}ACDC_Inf;

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern ACDC_Inf ACDC;


/**********************************************************************************************************
**                                         外部引用函数区
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
