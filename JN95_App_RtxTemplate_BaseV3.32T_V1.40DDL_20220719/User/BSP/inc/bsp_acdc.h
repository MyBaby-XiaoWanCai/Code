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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
/* ѡ��CAN1����CAN2���˴�ѡ��CAN1 */
#define APP_CAN_SEL_U1                      (0U)
#define APP_CAN_SEL_U2                      (1U)
#define APP_CAN_SEL                         (APP_CAN_SEL_U1)

/*
 * ����ѡ��ͬ��CAN�����ò�ͬ��GPIO
 * CAN��ѡ�����ж�����IRQn: [Int000_IRQn, Int031_IRQn], [Int092_IRQn, Int097_IRQn].
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
 * CAN ID����: id�š���չid
 * �����˲�������
 * ֡������󳤶ȶ���
 */
#define POWER_SCIENCE_ID                    (0x10000000UL)
#define POWER_SCIENCE_ID_IDE                (1U)

#define POWER_LD_ID                         (0x06080000UL)
#define POWER_LD_ID_IDE                     (1U)

#define POWER_LD_UPDATE_ID                  (0x1c000000UL)
#define POWER_LD_UPDATE_ID_IDE              (1U)

#define APP_CAN_AF_SEL                      (CAN_AF1 | CAN_AF2 | CAN_AF3)

#define APP_CAN_AF1_ID                      (POWER_SCIENCE_ID)
#define APP_CAN_AF1_ID_MSK                  (0xE0FFFFFFUL)                                     /* Accept messages with ID 0x102xxxxx. */
#define APP_CAN_AF1_MSK_TYPE                (CAN_AF_MSK_EXT)                                /* Accept extended ID. */

#define APP_CAN_AF2_ID                      (POWER_LD_ID)
#define APP_CAN_AF2_ID_MSK                  (0xE000FFFFUL)                                     /* Accept messages with ID 0x060xxxxx. */
#define APP_CAN_AF2_MSK_TYPE                (CAN_AF_MSK_EXT)                                /* Accept extended ID. */

#define APP_CAN_AF3_ID                      (POWER_LD_UPDATE_ID)
#define APP_CAN_AF3_ID_MSK                  (0xE0FFFFUL)                                    /* Accept messages with ID 0x1cxxxxxx. */
#define APP_CAN_AF3_MSK_TYPE                (CAN_AF_MSK_EXT)                                /* Accept extended ID. */

#define MSG_MAX_NUM                         (1U)
#define APP_DATA_SIZE                       (8U)

 
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
    __IO float AACV[2];          //ACDC����ʱ�̽�����ѹ��0��Ӧģ��0��1��Ӧģ��1 ʵ��ֵV 
    __IO float ADCV[2];          //ACDC����ʱ��ֱ����ѹ��0��Ӧģ��0��1��Ӧģ��1 ʵ��ֵV 
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
extern ACDC_UpdateStrcut CanRet;	


/**********************************************************************************************************
**                                         �ⲿ���ú�����
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

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
