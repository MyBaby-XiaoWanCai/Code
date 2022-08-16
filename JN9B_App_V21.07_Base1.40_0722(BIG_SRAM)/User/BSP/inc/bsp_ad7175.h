/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : ad7175.h
** Author      : Huang Cheng
** Date        : 2021-08-23
** Description : ad7175 head file
**********************************************************************************************************/

#ifndef __BSP_AD7175_H
#define __BSP_AD7175_H

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include "hc32_ddl.h"
#include "bsp_spi.h"
#include "bsp_exti.h"
/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
/* Error codes */
#define AD7175_CRC_ERR                              13u

#define AD7175_WR                                   0u
#define AD7175_RD                                   1u

#define SINGLE_COLT_RESET                           0u
#define SINGLE_COLT_SET                             1u

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
typedef enum
{
	COMMS = 0x00,
	STATUS = 0x00,
	ADCMODE,
	IFMODE,
	REGCHECK,
	DATA,
	RESERVED_5,
	GPIOCON,
	ID,
	RESERVED_8,
	RESERVED_9,
	RESERVED_10,
	RESERVED_11,
	RESERVED_12,
	RESERVED_13,
	RESERVED_14,
	RESERVED_15,
	CH0,
	CH1,
	CH2,
	CH3,
	CH4,
	CH5,
	CH6,
	CH7,
	CH8,
	CH9, 
	CH10,
	CH11,
	CH12,
	CH13,
	CH14,
	CH15,
	SETUPCON0,
	SETUPCON1,
	SETUPCON2,
	SETUPCON3,
	SETUPCON4,
	SETUPCON5,
	SETUPCON6,
	SETUPCON7,
	FILTCON0,
	FILTCON1,
	FILTCON2,
	FILTCON3,
	FILTCON4,
	FILTCON5,
	FILTCON6,
	FILTCON7,
	OFFSET0,
	OFFSET1,
	OFFSET2,
	OFFSET3,
	OFFSET4,
	OFFSET5,
	OFFSET6,
	OFFSET7,
	GAIN0,
	GAIN1,
	GAIN2,
	GAIN3,
	GAIN4,
	GAIN5,
	GAIN6,
	GAIN7,
	
	REG_NUM_MAX
}AD7175_REG;

typedef struct
{
	uint8_t  RegAddr;
	uint8_t  ReadWrite;
	uint8_t  DataLen;
	uint32_t RegData;
}AD7175_REG_DATASTR;

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
extern uint8_t AD7175_ID[3];
extern uint8_t AD7175_Start_Read;
extern uint32_t ad7175_value[16];
extern uint8_t Ad7175_Flag;

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void bsp_AD7175Init(void);
void AD7175_Process(void);

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/







