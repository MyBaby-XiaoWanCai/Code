#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#include <stdint.h>
#include "hc32_ddl.h"
/*
 * CAN mode definitions.
 *
 * CAN_MODE_ELB: External loopback mode. Self transmits self receives, or communicates
 *               with other CAN modules.
 * CAN_MODE_NORMAL: Normal communication mode. Can only communicates with other CAN modules.
 */
#define APP_CAN_MODE                        (CAN_MODE_ILB)

/* Specify the function of the target. */
#if (APP_CAN_MODE != CAN_MODE_ILB) && (APP_CAN_MODE != CAN_MODE_NORMAL)
    #error "The mode is NOT supported in this example."
#endif

/* CAN FD function control. Non-zero to enable. */
#define APP_CAN_FD_ENABLE                   (0U)

/* Unit definition of CAN in this example. */
#define APP_CAN_SEL_U1                      (0U)
#define APP_CAN_SEL_U2                      (1U)

/* Select a CAN unit. */
#define APP_CAN_SEL                         (APP_CAN_SEL_U1)

/* Definitions according to 'APP_CAN'. */
#if (APP_CAN_SEL == APP_CAN_SEL_U1)
    #define APP_CAN_UNIT                    (M4_CAN1)
    #define APP_CAN_PERIP_CLK               (PWC_FCG1_CAN1)

    #define APP_CAN_TX_PORT                 (GPIO_PORT_D)
    #define APP_CAN_TX_PIN                  (GPIO_PIN_05)
    #define APP_CAN_TX_PIN_FUNC             (GPIO_FUNC_60_CAN1_TX)

    #define APP_CAN_RX_PORT                 (GPIO_PORT_D)
    #define APP_CAN_RX_PIN                  (GPIO_PIN_04)
    #define APP_CAN_RX_PIN_FUNC             (GPIO_FUNC_61_CAN1_RX)

    #define APP_CAN_CLK_CH                  (CLK_CAN_CH1)
    #define APP_CAN_CLK_DIV                 (CLK_CAN1_CLK_MCLK_DIV3)
#endif

/* Acceptance filter. */
#define APP_CAN_AF_SEL                      (CAN_AF1 | CAN_AF2 | CAN_AF3)

#define APP_CAN_AF1_ID                      (0xA1U)
#define APP_CAN_AF1_ID_MSK                  (0x00U)                 /* Only accept messages with ID '1010 0001'. */
#define APP_CAN_AF1_MSK_TYPE                (CAN_AF_MSK_STD)        /* Only accept standard ID. */

#define APP_CAN_AF2_ID                      (0xB2U)
#define APP_CAN_AF2_ID_MSK                  (0xB0U)                 /* Accept messages with ID 'x0xx 0010'. */
#define APP_CAN_AF2_MSK_TYPE                (CAN_AF_MSK_EXT)        /* Accept extended ID. */

#define APP_CAN_AF3_ID                      (0xC3U)
#define APP_CAN_AF3_ID_MSK                  (0x03U)                 /* Accept messages with ID '1100 00xx'. */
#define APP_CAN_AF3_MSK_TYPE                (CAN_AF_MSK_STD_EXT)    /* Accept standard ID and extended ID. */

/* Message ID definitions. */
#define APP_ID_1                            (0x9AC01UL)
#define APP_ID_1_IDE                        (1U)

#define APP_ID_2                            (0x52U)
#define APP_ID_2_IDE                        (0U)

#define APP_ID_3                            (APP_CAN_AF3_ID)
#define APP_ID_3_IDE_ENABLE                 (1U)
#define APP_ID_3_IDE_DISABLE                (0U)

/* Max data size of the frame. */
#if (APP_CAN_FD_ENABLE > 0U)
    #define APP_DATA_SIZE                   (64U)
#else
    #define APP_DATA_SIZE                   (8U)
#endif

/* Debug printing definition. */
#if (DDL_PRINT_ENABLE == DDL_ON)
#define DBG         (void)printf
#else
#define DBG(...)
#endif

#endif

