/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_acdc.h
** Author      : Huang Cheng
** Date        : 2021-11-15
** Description : 
**********************************************************************************************************/

#ifndef _BSP_ACDC_H_
#define _BSP_ACDC_H_

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

#define CAN_BAUDRATE                                                500

/*
 * 根据选择不同的CAN来配置不同的GPIO
 * CAN可选独立中断向量IRQn: [Int000_IRQn, Int031_IRQn], [Int092_IRQn, Int097_IRQn].
 */
#if (APP_CAN_SEL == APP_CAN_SEL_U1)
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

    #define APP_CAN1_INT_TYPE                (CAN_INT_RB_ALMOST_FULL|CAN_INT_ERR_INT|CAN_INT_ARB_LOST)
    #define APP_CAN1_INT_PRIO                (DDL_IRQ_PRIORITY_02)
    #define APP_CAN1_INT_SRC                 (INT_CAN1_HOST)
    #define APP_CAN1_IRQn                    (Int092_IRQn)
	
		/* Modify by CAI HENG (Begin) */	
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

    #define APP_CAN2_INT_TYPE                (CAN_INT_RB_ALMOST_FULL|CAN_INT_ERR_INT|CAN_INT_ARB_LOST)
    #define APP_CAN2_INT_PRIO                (DDL_IRQ_PRIORITY_02)
    #define APP_CAN2_INT_SRC                 (INT_CAN2_HOST)
    #define APP_CAN2_IRQn                    (Int093_IRQn)
		/* Modify by CAI HENG (End) */
	
#elif (APP_CAN_SEL == APP_CAN_SEL_U2)
    #define APP_CAN_UNIT                    (M4_CAN2)
    #define APP_CAN_PERIP_CLK               (PWC_FCG1_CAN2)

    #define APP_CAN_TX_PORT                 (GPIO_PORT_B)
    #define APP_CAN_TX_PIN                  (GPIO_PIN_13)
    #define APP_CAN_TX_PIN_FUNC             (GPIO_FUNC_62_CAN2_TX)

    #define APP_CAN_RX_PORT                 (GPIO_PORT_B)
    #define APP_CAN_RX_PIN                  (GPIO_PIN_12)
    #define APP_CAN_RX_PIN_FUNC             (GPIO_FUNC_63_CAN2_RX)

    #define APP_CAN_CLK_CH                  (CLK_CAN_CH2)
    #define APP_CAN_CLK_DIV                 (CLK_CAN2_CLK_MCLK_DIV6)

    #define APP_CAN_INT_TYPE                (CAN_INT_ALL)
    #define APP_CAN_INT_PRIO                (DDL_IRQ_PRIORITY_06)
    #define APP_CAN_INT_SRC                 (INT_CAN2_HOST)
    #define APP_CAN_IRQn                    (Int092_IRQn)
#endif

/*
 * CAN ID定义: id号、扩展id
 * 接收滤波器配置
 * 帧数据最大长度定义
 */
#define POWER_SCIENCE_ID                    (0x10000000UL)
#define POWER_SCIENCE_ID_IDE                (1U)

#define POWER_LD_ID                         (0x06080000UL)
#define POWER_LD_ID_IDE                     (1U)

#define POWER_LD_UPDATE_ID                  (0x1c000000UL)
#define POWER_LD_UPDATE_ID_IDE              (1U)

/*
 * CAN2 ID定义: id号、扩展id
 * 接收滤波器配置
 * 帧数据最大长度定义
 */
#define TW_ALARM_ID                         (0x00000380UL)        //图为电源告警报文ID，bit10~8表示报文类型，0x3代表告警报文\
                                                                    bit7固定=1,bit6~0表示电源组ID，有效范围0x00~0x15,单台电源\
                                                                    （未组网）组ID=0

#define TW_STATUS_ID                        (0x00000480UL)        //图为电源组状态报文ID，bit10~8表示报文类型，0x4代表状态报文\
                                                                    bit7固定=1,bit6~0表示电源组ID，有效范围0x00~0x15,单台电源\
                                                                    （未组网）组ID=0

/* Modify by CAI HENG (Begin) */
#define DEV_TYPE_OFFSET						(26)			//bit 26
#define FEAT_CODE_OFFSET					(18)			//bit 18
#define FRAM_SEQ_OFFSET						(10)			//bit 10
#define DEV_ADDR_OFFSET						(2)				//bit 2
#define COM_TYPE_OFFSET						(1)				//bit 1
#define COM_DIR_OFFSET						(0)				//bit 0

#define DEV_TYPE_SIZE						(0x00000007)	//size 3
#define FEAT_CODE_SIZE					(0x000000FF)	//size 8
#define FRAM_SEQ_SIZE						(0x000000FF)	//size 8
#define DEV_ADDR_SIZE						(0x000000FF)	//size 8
#define COM_TYPE_SIZE						(0x00000001)	//size 1
#define COM_DIR_SIZE						(0x00000001)	//size 1

#define DEV_TYPE							(4)				//JN910
#define FEAT_CODE							(0)				//暂不使用
#define FRAM_SEQ							(0)				//暂不使用
#define DEV_ADDR							(dev_addr)		//和拨码关联
#define COM_TYPE_VOD						(1)				//点播
#define COM_TYPE_BROADCAST					(0)				//广播
#define COM_DIR_UP							(1)				//上行方向(下位机到中位机)
#define COM_DIR_DOWN						(0)				//下行方向(中位机到下位机)

#define M_VOD_ID									((DEV_TYPE << DEV_TYPE_OFFSET) | (DEV_ADDR << DEV_ADDR_OFFSET) | (COM_TYPE_VOD << COM_TYPE_OFFSET) | (COM_DIR_DOWN << COM_DIR_OFFSET))
#define M_BROADCAST_ID						((DEV_TYPE << DEV_TYPE_OFFSET) | (DEV_ADDR << DEV_ADDR_OFFSET) | (COM_TYPE_BROADCAST << COM_TYPE_OFFSET) | (COM_DIR_DOWN << COM_DIR_OFFSET))

//#define M_VOD_ID                            (0x10000002)     //中位机点播ID
//#define M_BROADCAST_ID                      (0x10000000)     //中位机广播ID
/* Modify by CAI HENG (End) */

/* Modify by CAI HENG (Begin) */
#define APP_CAN1_AF1_ID                     (M_VOD_ID)
#define APP_CAN1_AF1_ID_MSK									(~((DEV_TYPE_SIZE << DEV_TYPE_OFFSET) | (DEV_ADDR_SIZE << DEV_ADDR_OFFSET) | (COM_TYPE_SIZE << COM_TYPE_OFFSET) | (COM_DIR_SIZE << COM_DIR_OFFSET)))
//#define APP_CAN1_AF1_ID_MSK                 (0xE3FFFC00UL)                                  /* Accept messages with ID 0x102xxxxx. */
#define APP_CAN1_AF1_MSK_TYPE               (CAN_AF_MSK_EXT)                                /* Accept extended ID. */
#define APP_CAN1_AF2_ID                     (M_BROADCAST_ID)
#define APP_CAN1_AF2_ID_MSK									(~((DEV_TYPE_SIZE << DEV_TYPE_OFFSET) | (COM_TYPE_SIZE << COM_TYPE_OFFSET) | (COM_DIR_SIZE << COM_DIR_OFFSET)))
//#define APP_CAN1_AF2_ID_MSK                 (0xE3FFFFFCUL)                                  /* Accept messages with ID 0x060xxxxx. */
#define APP_CAN1_AF2_MSK_TYPE               (CAN_AF_MSK_EXT)                                /* Accept extended ID. */
/* Modify by CAI HENG (End) */

#define APP_CAN2_AF_SEL                      (CAN_AF1 | CAN_AF2)

#define APP_CAN2_AF1_ID                      (TW_ALARM_ID)
#define APP_CAN2_AF1_ID_MSK                  (0x00UL)                                  /* Accept messages with ID 0xxxxxx380. */
#define APP_CAN2_AF1_MSK_TYPE                (CAN_AF_MSK_STD)                                /* Accept standardID. */

#define APP_CAN2_AF2_ID                      (TW_STATUS_ID)
#define APP_CAN2_AF2_ID_MSK                  (0x00UL)                                  /* Accept messages with ID 0x060xxxxx. */
#define APP_CAN2_AF2_MSK_TYPE                (CAN_AF_MSK_STD)                                /* Accept extended ID. */

#define MSG_MAX_NUM                         (1U)
#define APP_DATA_SIZE                       (8U)

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
void bsp_InitACDC(void);
en_result_t CANx_SendData(M4_CAN_TypeDef *CANx,  uint32_t ID, uint8_t *pData, uint16_t Len);
void CanDebugInfPrint(void);

uint8_t ACDC_Fan_Speed51_ModeTest(void);
void ACDC_Fan_Speed_CMD(uint8_t type, uint8_t addr, uint8_t Speed);
void ACDC_DCV_Query_CMD(uint8_t type, uint8_t addr);
void ACDC_DCI_Query_CMD(uint8_t type, uint8_t addr);
void ACDC_ACV_Query_CMD(uint8_t type, uint8_t addr);
void ACDC_ACI_Query_CMD(uint8_t type, uint8_t addr);
void ACDC_ACF_Query_CMD(uint8_t type, uint8_t addr);
void ACDC_Fault_Clear_CMD(uint8_t type, uint8_t addr);
void ACDC_FW_VERSION_CMD(uint8_t type, uint8_t addr);
uint8_t CAN1_SendData(uint32_t ID, uint8_t *pData, uint16_t len);
uint8_t CAN2_SendData(uint32_t ID, uint8_t *pData, uint16_t len);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
