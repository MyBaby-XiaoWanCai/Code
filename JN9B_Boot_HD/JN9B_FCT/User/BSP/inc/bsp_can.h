/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_can.h
** Author      :  
** Date        : 2022-01-18
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

/*
 * 根据选择不同的CAN来配置不同的GPIO
 * CAN可选独立中断向量IRQn: [Int000_IRQn, Int031_IRQn], [Int092_IRQn, Int097_IRQn].
 */

    #define APP_CAN1_UNIT                    (M4_CAN1)
    #define APP_CAN1_PERIP_CLK               (PWC_FCG1_CAN1)

    #define APP_CAN1_TX_PORT                 (GPIO_PORT_A)
    #define APP_CAN1_TX_PIN                  (GPIO_PIN_12)
    #define APP_CAN1_TX_PIN_FUNC             (GPIO_FUNC_60_CAN1_TX)

    #define APP_CAN1_RX_PORT                 (GPIO_PORT_A)
    #define APP_CAN1_RX_PIN                  (GPIO_PIN_11)
    #define APP_CAN1_RX_PIN_FUNC             (GPIO_FUNC_61_CAN1_RX)

    #define APP_CAN1_CLK_CH                  (CLK_CAN_CH1)
    #define APP_CAN1_CLK_DIV                 (CLK_CAN1_CLK_MCLK_DIV6)

    #define APP_CAN1_INT_TYPE                (CAN_INT_RB_ALMOST_FULL)
    #define APP_CAN1_INT_PRIO                (DDL_IRQ_PRIORITY_02)
    #define APP_CAN1_INT_SRC                 (INT_CAN1_HOST)
    #define APP_CAN1_IRQn                    (Int092_IRQn)
		

    #define APP_CAN2_UNIT                    (M4_CAN2)
    #define APP_CAN2_PERIP_CLK               (PWC_FCG1_CAN2)

    #define APP_CAN2_TX_PORT                 (GPIO_PORT_B)
    #define APP_CAN2_TX_PIN                  (GPIO_PIN_13)
    #define APP_CAN2_TX_PIN_FUNC             (GPIO_FUNC_62_CAN2_TX)

    #define APP_CAN2_RX_PORT                 (GPIO_PORT_B)
    #define APP_CAN2_RX_PIN                  (GPIO_PIN_12)
    #define APP_CAN2_RX_PIN_FUNC             (GPIO_FUNC_63_CAN2_RX)

    #define APP_CAN2_CLK_CH                  (CLK_CAN_CH2)
    #define APP_CAN2_CLK_DIV                 (CLK_CAN2_CLK_MCLK_DIV6)

    #define APP_CAN2_INT_TYPE                (CAN_INT_RB_ALMOST_FULL)
    #define APP_CAN2_INT_PRIO                (DDL_IRQ_PRIORITY_02)
    #define APP_CAN2_INT_SRC                 (INT_CAN2_HOST)
    #define APP_CAN2_IRQn                    (Int093_IRQn)


/*
 * CAN ID定义: id号、扩展id
 * 接收滤波器配置
 * 帧数据最大长度定义
 */
#define POWER_SCIENCE_ID                    (0x10200000UL)    //国电非升级模式用ID

#define POWER_LD_ID                         (0x06000000UL)    //普德非升级模式用ID

#define POWER_LD_UPDATE_ID                  (0x1c000000UL)    //普德升级用ID

#define M_VOD_ID                            (0x10000002)     //中位机点播ID

#define M_BROADCAST_ID                      (0x10000000)     //中位机广播ID


#define APP_CAN1_AF1_ID                      (POWER_SCIENCE_ID)
#define APP_CAN1_AF1_ID_MSK                  (0xFFFFFUL)                                     /* Accept messages with ID 0x102xxxxx. */
#define APP_CAN1_AF1_MSK_TYPE                (CAN_AF_MSK_EXT)                                /* Accept extended ID. */

#define APP_CAN1_AF2_ID                      (POWER_LD_ID)
#define APP_CAN1_AF2_ID_MSK                  (0xFFFFFUL)                                     /* Accept messages with ID 0x060xxxxx. */
#define APP_CAN1_AF2_MSK_TYPE                (CAN_AF_MSK_EXT)                                /* Accept extended ID. */

#define APP_CAN1_AF3_ID                      (POWER_LD_UPDATE_ID)
#define APP_CAN1_AF3_ID_MSK                  (0xFFFFFFUL)                                    /* Accept messages with ID 0x1cxxxxxx. */
#define APP_CAN1_AF3_MSK_TYPE                (CAN_AF_MSK_EXT)                                /* Accept extended ID. */



#define APP_CAN2_AF1_ID                      (M_VOD_ID)
#define APP_CAN2_AF1_ID_MSK                  (0xE3FFFC00UL)                                  /* Accept messages with ID 0x102xxxxx. */
#define APP_CAN2_AF1_MSK_TYPE                (CAN_AF_MSK_EXT)                                /* Accept extended ID. */

#define APP_CAN2_AF2_ID                      (M_BROADCAST_ID)
#define APP_CAN2_AF2_ID_MSK                  (0xE3FFFFFCUL)                                  /* Accept messages with ID 0x060xxxxx. */
#define APP_CAN2_AF2_MSK_TYPE                (CAN_AF_MSK_EXT)                                /* Accept extended ID. */

#define MSG_MAX_NUM                         (1U)
#define APP_DATA_SIZE                       (8U)

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
uint8_t CAN1_SendData(uint32_t ID, uint8_t *pData , uint16_t len);
uint8_t CAN2_SendData(uint32_t ID, uint8_t *pData, uint16_t len);

void bsp_InitCAN1(void);
void bsp_InitCAN2(void);
void Can1Config(void);
void Can2Config(void);
void CanDebugInfPrint(void);
void CAN1_ResvData( stc_can_rx_t* stcRx );
void CAN2_ResvData( stc_can_rx_t* stcRx );

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

