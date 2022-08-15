/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_can.h
** Author      : 
** Date        : 2022-01-19
** Description : 
**********************************************************************************************************/

#ifndef _BSP_CAN_H_
#define _BSP_CAN_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include "hc32_ddl.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
/* 选择CAN1或者CAN2，此处选择CAN1 */
#define APP_CAN_SEL_U1                      (0U)
#define APP_CAN_SEL_U2                      (1U)
#define APP_CAN_SEL                         (APP_CAN_SEL_U1)

/*
 * 根据选择不同的CAN来配置不同的GPIO
 * CAN可选独立中断向量IRQn: [Int000_IRQn, Int031_IRQn], [Int092_IRQn, Int097_IRQn].
 */
//#if (APP_CAN_SEL == APP_CAN_SEL_U1)
//    #define APP_CAN_UNIT                    (M4_CAN1)	//用于和ACDC通信
//    #define APP_CAN_PERIP_CLK               (PWC_FCG1_CAN1)

//    #define APP_CAN_TX_PORT                 (GPIO_PORT_A)
//    #define APP_CAN_TX_PIN                  (GPIO_PIN_12)
//    #define APP_CAN_TX_PIN_FUNC             (GPIO_FUNC_60_CAN1_TX)

//    #define APP_CAN_RX_PORT                 (GPIO_PORT_A)
//    #define APP_CAN_RX_PIN                  (GPIO_PIN_11)
//    #define APP_CAN_RX_PIN_FUNC             (GPIO_FUNC_61_CAN1_RX)

//    #define APP_CAN_CLK_CH                  (CLK_CAN_CH1)
//    #define APP_CAN_CLK_DIV                 (CLK_CAN1_CLK_MCLK_DIV6)

//    #define APP_CAN_INT_TYPE                (CAN_INT_RB_ALMOST_FULL)
//    #define APP_CAN_INT_PRIO                (DDL_IRQ_PRIORITY_02)
//    #define APP_CAN_INT_SRC                 (INT_CAN1_HOST)
//    #define APP_CAN_IRQn                    (Int092_IRQn)
//		
//#elif (APP_CAN_SEL == APP_CAN_SEL_U2)	//用于和中位机通信
//    #define APP_CAN2_UNIT                    (M4_CAN2)
//    #define APP_CAN2_PERIP_CLK               (PWC_FCG1_CAN2)

//    #define APP_CAN2_TX_PORT                 (GPIO_PORT_B)
//    #define APP_CAN2_TX_PIN                  (GPIO_PIN_13)
//    #define APP_CAN2_TX_PIN_FUNC             (GPIO_FUNC_62_CAN2_TX)

//    #define APP_CAN2_RX_PORT                 (GPIO_PORT_B)
//    #define APP_CAN2_RX_PIN                  (GPIO_PIN_12)
//    #define APP_CAN2_RX_PIN_FUNC             (GPIO_FUNC_63_CAN2_RX)

//    #define APP_CAN2_CLK_CH                  (CLK_CAN_CH2)
//    #define APP_CAN2_CLK_DIV                 (CLK_CAN2_CLK_MCLK_DIV6)

//    #define APP_CAN2_INT_TYPE                (CAN_INT_ALL)
//    #define APP_CAN2_INT_PRIO                (DDL_IRQ_PRIORITY_06)
//    #define APP_CAN2_INT_SRC                 (INT_CAN2_HOST)
//    #define APP_CAN2_IRQn                    (Int092_IRQn)
//#endif

///*
// * CAN ID定义: id号、扩展id
// * 接收滤波器配置
// * 帧数据最大长度定义
// */
//#define POWER_SCIENCE_ID                    (0x10200000UL)
//#define POWER_SCIENCE_ID_IDE                (1U)

//#define POWER_LD_ID                         (0x06000000UL)
//#define POWER_LD_ID_IDE                     (1U)
//#define POWER_LD_UPDATE_ID                  (0x1c000000UL)
//#define POWER_LD_UPDATE_ID_IDE              (1U)

//#define APP_CAN_AF_SEL                      (CAN_AF1 | CAN_AF2 | CAN_AF3)
//#define APP_CAN_AF1_ID                      (POWER_SCIENCE_ID)
//#define APP_CAN_AF1_ID_MSK                  (0xFFFFFUL)                                     /* Accept messages with ID 0x102xxxxx. */
//#define APP_CAN_AF1_MSK_TYPE                (CAN_AF_MSK_EXT)                                /* Accept extended ID. */

//#define APP_CAN_AF2_ID                      (POWER_LD_ID)
//#define APP_CAN_AF2_ID_MSK                  (0xFFFFFUL)                                     /* Accept messages with ID 0x060xxxxx. */
//#define APP_CAN_AF2_MSK_TYPE                (CAN_AF_MSK_EXT)                                /* Accept extended ID. */

//#define APP_CAN_AF3_ID                      (POWER_LD_UPDATE_ID)
//#define APP_CAN_AF3_ID_MSK                  (0xFFFFFFUL)                                    /* Accept messages with ID 0x1cxxxxxx. */
//#define APP_CAN_AF3_MSK_TYPE                (CAN_AF_MSK_EXT)                                /* Accept extended ID. */

//#define MSG_MAX_NUM                         (1U)
//#define APP_DATA_SIZE                       (8U)

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
//void bsp_CANInit(void);
//en_result_t CANx_SendData(M4_CAN_TypeDef *CANx,  uint32_t ID, uint8_t *pData, uint16_t Len);
//void CanDebugInfPrint(void);

//void Can1Config(void);
//void Can1IrqConfig(void);
//void CAN1_IrqCallback(void);
//void CANx_ResvData(M4_CAN_TypeDef *CANx);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
