/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_spi.h
** Author      : Huang Cheng
** Date        : 2021-11-25
** Description : 
**********************************************************************************************************/

#ifndef __BSP_SPI_H
#define __BSP_SPI_H

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

/* SPI wire mode, can be SPI_WIRE_4/SPI_WIRE_3 */
#define W5500_WIRE_MODE         (SPI_WIRE_3)
#define AD7124_WIRE_MODE        (SPI_WIRE_3)
#define AD7175_WIRE_MODE        (SPI_WIRE_3)
#define DAC1_WIRE_MODE          (SPI_WIRE_3)
#define DAC2_WIRE_MODE          (SPI_WIRE_3)
#define W25Q_WIRE_MODE          (SPI_WIRE_3)

/* SPI mode, can be SPI_MODE_0/SPI_MODE_1/SPI_MODE_2/SPI_MODE_3 */
#define W5500_SPI_MODE          (SPI_MODE_3)
#define AD7124_SPI_MODE         (SPI_MODE_3)
#define AD7175_SPI_MODE         (SPI_MODE_3)
#define DAC1_SPI_MODE           (SPI_MODE_2)
#define DAC2_SPI_MODE           (SPI_MODE_2)
#define W25Q_SPI_MODE           (SPI_MODE_3)

/* Baudrate = PCLK1/DIVx */
#define W5500_BR_DIV            (SPI_BR_PCLK1_DIV8)
#define AD7124_BR_DIV           (SPI_BR_PCLK1_DIV16)
#define AD7175_BR_DIV           (SPI_BR_PCLK1_DIV16)
#define DAC1_BR_DIV             (SPI_BR_PCLK1_DIV16)
#define DAC2_BR_DIV             (SPI_BR_PCLK1_DIV16)
#define W25Q_BR_DIV             (SPI_BR_PCLK1_DIV16)

/* SPI unit */
#define W5500_SPI_UNIT          (M4_SPI1)
#define AD7124_SPI_UNIT         (M4_SPI2)
#define AD7175_SPI_UNIT         (M4_SPI3)
#define DAC1_SPI_UNIT           (M4_SPI4)
#define DAC2_SPI_UNIT           (M4_SPI5)
#define W25Q_SPI_UNIT           (M4_SPI6)
#define MB85R_SPI_UNIT          (M4_SPI6)

/* SPI unit colck */
#define W5500_UNIT_CLOCK        (PWC_FCG1_SPI1)
#define AD7124_UNIT_CLOCK       (PWC_FCG1_SPI2)
#define AD7175_UNIT_CLOCK       (PWC_FCG1_SPI3)
#define DAC1_UNIT_CLOCK         (PWC_FCG1_SPI4)
#define DAC2_UNIT_CLOCK         (PWC_FCG1_SPI5)
#define W25Q_UNIT_CLOCK         (PWC_FCG1_SPI6)

/* SPI port definition for W5500 */
#define W5500_NSS_PORT          (GPIO_PORT_I)
#define W5500_NSS_PIN           (GPIO_PIN_06)
#define W5500_SCK_PORT          (GPIO_PORT_B)
#define W5500_SCK_PIN           (GPIO_PIN_03)
#define W5500_SCK_FUNC          (GPIO_FUNC_40_SPI1_SCK)
#define W5500_MISO_PORT         (GPIO_PORT_B)
#define W5500_MISO_PIN          (GPIO_PIN_04)
#define W5500_MISO_FUNC         (GPIO_FUNC_42_SPI1_MISO)
#define W5500_MOSI_PORT         (GPIO_PORT_B)
#define W5500_MOSI_PIN          (GPIO_PIN_05)
#define W5500_MOSI_FUNC         (GPIO_FUNC_41_SPI1_MOSI)

/* SPI port definition for AD7124 */
#define AD7124_NSS_PORT         (GPIO_PORT_I)
#define AD7124_NSS_PIN          (GPIO_PIN_00)
#define AD7124_SCK_PORT         (GPIO_PORT_I)
#define AD7124_SCK_PIN          (GPIO_PIN_01)
#define AD7124_SCK_FUNC         (GPIO_FUNC_43_SPI2_SCK)
#define AD7124_MISO_PORT        (GPIO_PORT_I)
#define AD7124_MISO_PIN         (GPIO_PIN_02)
#define AD7124_MISO_FUNC        (GPIO_FUNC_45_SPI2_MISO)
#define AD7124_MOSI_PORT        (GPIO_PORT_I)
#define AD7124_MOSI_PIN         (GPIO_PIN_03)
#define AD7124_MOSI_FUNC        (GPIO_FUNC_44_SPI2_MOSI)

/* SPI port definition for AD7175 */
#define AD7175_NSS_PORT         (GPIO_PORT_D)
#define AD7175_NSS_PIN          (GPIO_PIN_00)
#define AD7175_SCK_PORT         (GPIO_PORT_C)
#define AD7175_SCK_PIN          (GPIO_PIN_10)
#define AD7175_SCK_FUNC         (GPIO_FUNC_19_SPI3_SCK)
#define AD7175_MISO_PORT        (GPIO_PORT_C)
#define AD7175_MISO_PIN         (GPIO_PIN_11)
#define AD7175_MISO_FUNC        (GPIO_FUNC_19_SPI3_MISO)
#define AD7175_MOSI_PORT        (GPIO_PORT_C)
#define AD7175_MOSI_PIN         (GPIO_PIN_12) 
#define AD7175_MOSI_FUNC        (GPIO_FUNC_19_SPI3_MOSI)

/* SPI port definition for DAC80508_1 */
#define DAC1_NSS_PORT           (GPIO_PORT_E)
#define DAC1_NSS_PIN            (GPIO_PIN_11)
#define DAC1_SCK_PORT           (GPIO_PORT_E)
#define DAC1_SCK_PIN            (GPIO_PIN_12)
#define DAC1_SCK_FUNC           (GPIO_FUNC_43_SPI4_SCK)
#define DAC1_MISO_PORT          (GPIO_PORT_E)
#define DAC1_MISO_PIN           (GPIO_PIN_13)
#define DAC1_MISO_FUNC          (GPIO_FUNC_45_SPI4_MISO)
#define DAC1_MOSI_PORT          (GPIO_PORT_E)
#define DAC1_MOSI_PIN           (GPIO_PIN_14) 
#define DAC1_MOSI_FUNC          (GPIO_FUNC_44_SPI4_MOSI)

/* SPI port definition for DAC80508_2 */
#define DAC2_NSS_PORT           (GPIO_PORT_F)
#define DAC2_NSS_PIN            (GPIO_PIN_10)
#define DAC2_SCK_PORT           (GPIO_PORT_F)
#define DAC2_SCK_PIN            (GPIO_PIN_07)
#define DAC2_SCK_FUNC           (GPIO_FUNC_19_SPI5_SCK)
#define DAC2_MISO_PORT          (GPIO_PORT_F)
#define DAC2_MISO_PIN           (GPIO_PIN_08)
#define DAC2_MISO_FUNC          (GPIO_FUNC_19_SPI5_MISO)
#define DAC2_MOSI_PORT          (GPIO_PORT_F)
#define DAC2_MOSI_PIN           (GPIO_PIN_09)
#define DAC2_MOSI_FUNC          (GPIO_FUNC_19_SPI5_MOSI)

/* SPI port definition for W25Q16JVSSIQ and MB85RS2MTA */
#define MB85R_NSS_PORT          (GPIO_PORT_B)
#define MB85R_NSS_PIN           (GPIO_PIN_15)
#define W25Q_NSS_PORT           (GPIO_PORT_G)
#define W25Q_NSS_PIN            (GPIO_PIN_11)
#define W25Q_SCK_PORT           (GPIO_PORT_G)
#define W25Q_SCK_PIN            (GPIO_PIN_13)
#define W25Q_SCK_FUNC           (GPIO_FUNC_46_SPI6_SCK)
#define W25Q_MISO_PORT          (GPIO_PORT_G)
#define W25Q_MISO_PIN           (GPIO_PIN_12)
#define W25Q_MISO_FUNC          (GPIO_FUNC_48_SPI6_MISO)
#define W25Q_MOSI_PORT          (GPIO_PORT_G)
#define W25Q_MOSI_PIN           (GPIO_PIN_14)
#define W25Q_MOSI_FUNC          (GPIO_FUNC_47_SPI6_MOSI)

/* SPI NSS pin config */
#define W5500_NSS_1()           GPIO_SetPins(W5500_NSS_PORT, W5500_NSS_PIN)
#define W5500_NSS_0()           GPIO_ResetPins(W5500_NSS_PORT, W5500_NSS_PIN)
#define AD7124_NSS_1()          GPIO_SetPins(AD7124_NSS_PORT, AD7124_NSS_PIN)
#define AD7124_NSS_0()          GPIO_ResetPins(AD7124_NSS_PORT, AD7124_NSS_PIN)
#define AD7175_NSS_1()          GPIO_SetPins(AD7175_NSS_PORT, AD7175_NSS_PIN)
#define AD7175_NSS_0()          GPIO_ResetPins(AD7175_NSS_PORT, AD7175_NSS_PIN)
#define DAC1_NSS_1()            GPIO_SetPins(DAC1_NSS_PORT, DAC1_NSS_PIN)
#define DAC1_NSS_0()            GPIO_ResetPins(DAC1_NSS_PORT, DAC1_NSS_PIN)
#define DAC2_NSS_1()            GPIO_SetPins(DAC2_NSS_PORT, DAC2_NSS_PIN)
#define DAC2_NSS_0()            GPIO_ResetPins(DAC2_NSS_PORT, DAC2_NSS_PIN)
#define W25Q_NSS_1()            GPIO_SetPins(W25Q_NSS_PORT, W25Q_NSS_PIN)
#define W25Q_NSS_0()            GPIO_ResetPins(W25Q_NSS_PORT, W25Q_NSS_PIN)
#define MB85R_NSS_1()           GPIO_SetPins(MB85R_NSS_PORT, MB85R_NSS_PIN)
#define MB85R_NSS_0()           GPIO_ResetPins(MB85R_NSS_PORT, MB85R_NSS_PIN)

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void W5500_SpiInit(void);
void AD7124_SpiInit(void);
void AD7175_SpiInit(void);
void DAC1_SpiInit(void);
void DAC2_SpiInit(void);
void W25Q_MB85R_SpiInit(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

