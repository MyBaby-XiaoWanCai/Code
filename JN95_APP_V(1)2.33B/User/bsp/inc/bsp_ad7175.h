/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_ad7175.h
** Author      : Cai Heng
** Date        : 2021-07-21
** Description : 
**********************************************************************************************************/
#ifndef _BSP_AD7175_H_
#define _BSP_AD7175_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "stm32f4xx.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
/* AD7175 Config */
#define AD7175_RDY_GPIO_CLK_ALLENABLE() {			\
		__HAL_RCC_GPIOE_CLK_ENABLE();				\
	};	

#define AD7175_RDY_PORT								GPIOE
#define AD7175_RDY_PIN								GPIO_PIN_0		/* AD7175_RDY : PE0 (Configurable) */
	
#define AD7175_ERR_GPIO_CLK_ALLENABLE() {			\
		__HAL_RCC_GPIOD_CLK_ENABLE();				\
	};	

#define AD7175_ERR_PORT								GPIOD
#define AD7175_ERR_PIN								GPIO_PIN_1

#define AD7175_CS_0()								SPI3_CS_ENABLE()
#define AD7175_CS_1()								SPI3_CS_DISABLE()		
#define AD7175_HANDLER								SPI3_Handler
#define AD7175_EXTIIRQ								EXTI0_IRQn
	
#define AD7175_CfgSPI()								bsp_InitSpi3()	
	
/* Error codes */
#define INVALID_VAL 								-1		/* Invalid argument */
#define COMM_ERR									-2		/* Communication error on receive */
#define TIMEOUT										-3		/* A timeout has occured */

/* AD717X Register Map */
#define AD717X_COMM_REG								0x00
#define AD717X_STATUS_REG							0x00
#define AD717X_ADCMODE_REG							0x01
#define AD717X_IFMODE_REG							0x02
#define AD717X_REGCHECK_REG							0x03
#define AD717X_DATA_REG								0x04
#define AD717X_GPIOCON_REG							0x06
#define AD717X_ID_REG								0x07
#define AD717X_CHMAP0_REG							0x10
#define AD717X_CHMAP1_REG							0x11
#define AD717X_CHMAP2_REG							0x12
#define AD717X_CHMAP3_REG							0x13
#define AD717X_CHMAP4_REG							0x14
#define AD717X_CHMAP5_REG							0x15
#define AD717X_CHMAP6_REG							0x16
#define AD717X_CHMAP7_REG							0x17
#define AD717X_CHMAP8_REG							0x18
#define AD717X_CHMAP9_REG							0x19
#define AD717X_CHMAP10_REG							0x1A
#define AD717X_CHMAP11_REG							0x1B
#define AD717X_CHMAP12_REG							0x1C
#define AD717X_CHMAP13_REG							0x1D
#define AD717X_CHMAP14_REG							0x1E
#define AD717X_CHMAP15_REG							0x1F
#define AD717X_SETUPCON0_REG						0x20
#define AD717X_SETUPCON1_REG						0x21
#define AD717X_SETUPCON2_REG						0x22
#define AD717X_SETUPCON3_REG						0x23
#define AD717X_SETUPCON4_REG						0x24
#define AD717X_SETUPCON5_REG						0x25
#define AD717X_SETUPCON6_REG						0x26
#define AD717X_SETUPCON7_REG						0x27
#define AD717X_FILTCON0_REG							0x28
#define AD717X_FILTCON1_REG							0x29
#define AD717X_FILTCON2_REG							0x2A
#define AD717X_FILTCON3_REG							0x2B
#define AD717X_FILTCON4_REG							0x2C
#define AD717X_FILTCON5_REG							0x2D
#define AD717X_FILTCON6_REG							0x2E
#define AD717X_FILTCON7_REG							0x2F
#define AD717X_OFFSET0_REG							0x30
#define AD717X_OFFSET1_REG							0x31
#define AD717X_OFFSET2_REG							0x32
#define AD717X_OFFSET3_REG							0x33
#define AD717X_OFFSET4_REG							0x34
#define AD717X_OFFSET5_REG							0x35
#define AD717X_OFFSET6_REG							0x36
#define AD717X_OFFSET7_REG							0x37
#define AD717X_GAIN0_REG							0x38
#define AD717X_GAIN1_REG							0x39
#define AD717X_GAIN2_REG							0x3A
#define AD717X_GAIN3_REG							0x3B
#define AD717X_GAIN4_REG							0x3C
#define AD717X_GAIN5_REG							0x3D
#define AD717X_GAIN6_REG							0x3E
#define AD717X_GAIN7_REG							0x3F

/* Communication Register bits */
#define AD717X_COMM_REG_WEN							(0 << 7)
#define AD717X_COMM_REG_WR							(0 << 6)
#define AD717X_COMM_REG_RD							(1 << 6)
#define AD717X_COMM_REG_RA(x)						((x) & 0x3F)

/* Status Register bits */
#define AD717X_STATUS_REG_RDY						(1 << 7)
#define AD717X_STATUS_REG_ADC_ERR					(1 << 6)
#define AD717X_STATUS_REG_CRC_ERR					(1 << 5)
#define AD717X_STATUS_REG_REG_ERR					(1 << 4)
#define AD717X_STATUS_REG_CH(x)						((x) & 0x0F)

/* ADC Mode Register bits */
#define AD717X_ADCMODE_REG_REF_EN					(1 << 15)
#define AD717X_ADCMODE_SING_CYC						(1 << 13)
#define AD717X_ADCMODE_REG_DELAY(x)					(((x) & 0x7) << 8)
#define AD717X_ADCMODE_REG_MODE(x)					(((x) & 0x7) << 4)
#define AD717X_ADCMODE_REG_CLKSEL(x)				(((x) & 0x3) << 2)

/* ADC Mode Register additional bits for AD7172-2, AD7172-4, AD4111 and AD4112 */
#define AD717X_ADCMODE_REG_HIDE_DELAY				(1 << 14)

/* Interface Mode Register bits */
#define AD717X_IFMODE_REG_ALT_SYNC					(1 << 12)
#define AD717X_IFMODE_REG_IOSTRENGTH				(1 << 11)
#define AD717X_IFMODE_REG_HIDE_DELAY				(1 << 10)
#define AD717X_IFMODE_REG_DOUT_RESET				(1 << 8)
#define AD717X_IFMODE_REG_CONT_READ					(1 << 7)
#define AD717X_IFMODE_REG_DATA_STAT					(1 << 6)
#define AD717X_IFMODE_REG_REG_CHECK					(1 << 5)
#define AD717X_IFMODE_REG_XOR_EN					(0x01 << 2)
#define AD717X_IFMODE_REG_CRC_EN					(0x02 << 2)
#define AD717X_IFMODE_REG_XOR_STAT(x)				(((x) & AD717X_IFMODE_REG_XOR_EN) == AD717X_IFMODE_REG_XOR_EN)
#define AD717X_IFMODE_REG_CRC_STAT(x)				(((x) & AD717X_IFMODE_REG_CRC_EN) == AD717X_IFMODE_REG_CRC_EN)
#define AD717X_IFMODE_REG_DATA_WL16					(1 << 0)

/* GPIO Configuration Register bits */
#define AD717X_GPIOCON_REG_MUX_IO					(1 << 12)
#define AD717X_GPIOCON_REG_SYNC_EN					(1 << 11)
#define AD717X_GPIOCON_REG_ERR_EN(x)				(((x) & 0x3) << 9)
#define AD717X_GPIOCON_REG_ERR_DAT					(1 << 8)
#define AD717X_GPIOCON_REG_IP_EN1					(1 << 5)
#define AD717X_GPIOCON_REG_IP_EN0					(1 << 4)
#define AD717X_GPIOCON_REG_OP_EN1					(1 << 3)
#define AD717X_GPIOCON_REG_OP_EN0					(1 << 2)
#define AD717X_GPIOCON_REG_DATA1					(1 << 1)
#define AD717X_GPIOCON_REG_DATA0					(1 << 0)

/* GPIO Configuration Register additional bits for AD7172-4, AD7173-8 */
#define AD717X_GPIOCON_REG_GP_DATA3					(1 << 7)
#define AD717X_GPIOCON_REG_GP_DATA2					(1 << 6)
#define AD717X_GPIOCON_REG_GP_DATA1					(1 << 1)
#define AD717X_GPIOCON_REG_GP_DATA0					(1 << 0)

/* GPIO Configuration Register additional bits for AD7173-8 */
#define AD717X_GPIOCON_REG_PDSW						(1 << 14)
#define AD717X_GPIOCON_REG_OP_EN2_3					(1 << 13)

/* GPIO Configuration Register additional bits for AD4111, AD4112 */
#define AD4111_GPIOCON_REG_OP_EN0_1					(1 << 13)
#define AD4111_GPIOCON_REG_DATA1					(1 << 7)
#define AD4111_GPIOCON_REG_DATA0					(1 << 6)

/* GPIO Configuration Register additional bits for AD4111 */
#define AD4111_GPIOCON_REG_OW_EN					(1 << 12)

/* Channel Map Register 0-3 bits */
#define AD717X_CHMAP_REG_CH_EN						(1 << 15)
#define AD717X_CHMAP_REG_SETUP_SEL(x)				(((x) & 0x7) << 12)
#define AD717X_CHMAP_REG_AINPOS(x)					(((x) & 0x1F) << 5)
#define AD717X_CHMAP_REG_AINNEG(x)					(((x) & 0x1F) << 0)

/* Channel Map Register additional bits for AD4111, AD4112 */
#define AD4111_CHMAP_REG_INPUT(x)					(((x) & 0x3FF) << 0)

/* Setup Configuration Register 0-3 bits */
#define AD717X_SETUP_CONF_REG_BI_UNIPOLAR			(1 << 12)
#define AD717X_SETUP_CONF_REG_REF_SEL(x)			(((x) & 0x3) << 4)

/* Setup Configuration Register additional bits for AD7173-8 */
#define AD717X_SETUP_CONF_REG_REF_BUF(x)			(((x) & 0x3) << 10)
#define AD717X_SETUP_CONF_REG_AIN_BUF(x)			(((x) & 0x3) << 8)
#define AD717X_SETUP_CONF_REG_BURNOUT_EN			(1 << 7)
#define AD717X_SETUP_CONF_REG_BUFCHOPMAX			(1 << 6)

/* Setup Configuration Register additional bits for AD7172-2, AD7172-4, AD7175-2 */
#define AD717X_SETUP_CONF_REG_REFBUF_P				(1 << 11)
#define AD717X_SETUP_CONF_REG_REFBUF_N				(1 << 10)
#define AD717X_SETUP_CONF_REG_AINBUF_P				(1 << 9)
#define AD717X_SETUP_CONF_REG_AINBUF_N				(1 << 8)

/* Setup Configuration Register additional bits for AD4111, AD4112 */
#define AD4111_SETUP_CONF_REG_REFPOS_BUF			(1 << 11)
#define AD4111_SETUP_CONF_REG_REFNEG_BUF			(1 << 10)
#define AD4111_SETUP_CONF_REG_AIN_BUF(x)			(((x) & 0x3) << 8)
#define AD4111_SETUP_CONF_REG_BUFCHOPMAX			(1 << 6)

/* Filter Configuration Register 0-3 bits */
#define AD717X_FILT_CONF_REG_SINC3_MAP				(1 << 15)
#define AD717X_FILT_CONF_REG_ENHFILTEN				(1 << 11)
#define AD717X_FILT_CONF_REG_ENHFILT(x)				(((x) & 0x7) << 8)
#define AD717X_FILT_CONF_REG_ORDER(x)				(((x) & 0x3) << 5)
#define AD717X_FILT_CONF_REG_ODR(x)					(((x) & 0x1F) << 0)

/* ID register mask for relevant bits */
#define AD717X_ID_REG_MASK							0xFFF0

/* AD7172-2 ID */
#define AD7172_2_ID_REG_VALUE						0x00D0

/* AD7172-4 ID */
#define AD7172_4_ID_REG_VALUE						0x2050

/* AD7173-8 ID */
#define AD7173_8_ID_REG_VALUE						0x30D0

/* AD7175-2 ID */
#define AD7175_2_ID_REG_VALUE						0x0CD0

/* AD7175-8 ID */
#define AD7175_8_ID_REG_VALUE						0x3CD0

/* AD7176-2 ID */
#define AD7176_2_ID_REG_VALUE						0x0C90

/* AD7177-2 ID */
#define AD7177_2_ID_REG_VALUE						0x4FD0

/* AD411x ID */
#define AD411X_ID_REG_VALUE							0x30D0

/* AD717X Constants */
#define AD717X_CRC8_POLYNOMIAL_REPRESENTATION		0x07		/* x8 + x2 + x + 1 */

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern uint8_t AD7175_ID[3];

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
extern int32_t AD7175_ReadReg(uint8_t addr,uint8_t * value,uint8_t size);

#ifdef __cplusplus
}
#endif	

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


