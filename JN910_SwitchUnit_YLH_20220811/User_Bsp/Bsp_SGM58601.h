/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Bsp_SGM58601.h
** Author      : Cai Heng
** Date        : 2022-07-11
** Description : SGM58601驱动模块
**********************************************************************************************************/

#ifndef _START_TASK_H_
#define _START_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/

/* SGM5801 REGISTER MAP */
#define STATUS_REG_ADDR					0x00		// STATUS: Status Register
#define MUX_REG_ADDR					0x01		// MUX: Input Multiplexer Control Register
#define ADCON_REG_ADDR					0x02		// ADCON: A/D Control Register
#define DRATE_REG_ADDR					0x03		// DRATE: A/D Data Rate
#define IO_REG_ADDR						0x04		// IO: GPIO Control Register
#define OFC0_REG_ADDR					0x05		// OFC0: Offset Calibration Byte 0, least significant byte
#define OFC1_REG_ADDR					0x06		// OFC1: Offset Calibration Byte 1
#define OFC2_REG_ADDR					0x07		// OFC2: Offset Calibration Byte 2, most significant byte
#define FSC0_REG_ADDR					0x08		// FSC0: Full-scale Calibration Byte 0, least significant byte
#define FSC1_REG_ADDR					0x09		// FSC1: Full-scale Calibration Byte 1
#define FSC2_REG_ADDR					0x0A		// FSC2: Full-scale Calibration Byte 2, most significant byte
#define STATUS2_REG_ADDR				0x0B		// STATUS2: Status2 Register

/* SGM5801 COMMAND DEFINITIONS */
#define WAKEUP_COMMAND					0x00		// Completes SYNC and Exits Standby Mode
#define RDATA_COMMAND					0x01		// Read Data
#define RDATAC_COMMAND					0x03		// Read Data Continuously
#define SDATAC_COMMAND					0x0F		// Stop Read Data Continuously
//#define RREG_COMMAND					0x1x		// Read from REG rrr
//#define WREG_COMMAND					0x5x		// Write to REG rrr
#define SELFCAL_COMMAND					0xF0		// Offset and Gain Self-Calibration
#define SELFOCAL_COMMAND				0xF1		// Offset Self-Calibration
#define SELFGCAL_COMMAND				0xF2		// Gain Self-Calibration
#define SYSOCAL_COMMAND					0xF3		// System Offset Calibration
#define SYSGCAL_COMMAND					0xF4		// System Gain Calibration
#define SYNC_COMMAND					0xFC		// Synchronize the A/D Conversion
#define STANDBY_COMMAND					0xFD		// Begin Standby Mode
#define RESET_COMMAND					0xFE		// Reset to Power-Up Values
//#define WAKEUP_COMMAND				0xFF		// Completes SYNC and Exits Standby Mode


// STATUS: Status Register
// Bit[3]
#define MSB_FIRST						((uint8_t)0x00)
#define LSB_FIRST						((uint8_t)0x08)
#define IS_ADC_ORDER(ORDER)				(((ORDER) == MSB_FIRST) || \
										 ((ORDER) == LSB_FIRST))
// Bit[2]	
#define AUTO_CALI_EN					((uint8_t)0x04)
#define AUTO_CALI_DISABLE				((uint8_t)0x00)
#define IS_ADC_AUTO_CALI(AUTO_CALI)		(((AUTO_CALI) == AUTO_CALI_EN) || \
										 ((AUTO_CALI) == AUTO_CALI_DISABLE))
// Bit[1]	
#define BUFFER_EN						((uint8_t)0x02)
#define BUFFER_DISABLE					((uint8_t)0x00)
#define IS_ADC_BUFFER(BUFFER)			(((BUFFER) == BUFFER_EN) || \
										 ((BUFFER) == BUFFER_DISABLE))

// MUX: Input Multiplexer Control Register
#define	AIN0							0x00
#define AIN1							0x01
#define AIN2							0x02
#define AIN3							0x03
#define AIN4							0x04
#define AIN5							0x05
#define AIN6							0x06
#define AIN7							0x07
#define AINCOM							0x08
#define IS_ADC_AIN(AIN)					(((AIN) == AIN0) || \
										 ((AIN) == AIN1) || \
										 ((AIN) == AIN2) || \
										 ((AIN) == AIN3) || \
										 ((AIN) == AIN4) || \
										 ((AIN) == AIN5) || \
										 ((AIN) == AIN6) || \
										 ((AIN) == AIN7) || \
										 ((AIN) == AINCOM))

// ADCON: A/D Control Register
// Bit[6:5] CLK[1:0]
#define CLKOUT_OFF						0x00
#define CLKOUT_FCLK_Prescaler_1			0x20
#define CLKOUT_FCLK_Prescaler_2			0x40
#define CLKOUT_FCLK_Prescaler_4			0x60
#define IS_ADC_CLKOUT(CLKOUT)			(((CLKOUT) == CLKOUT_OFF             ) || \
										 ((CLKOUT) == CLKOUT_FCLK_Prescaler_1) || \
										 ((CLKOUT) == CLKOUT_FCLK_Prescaler_2) || \
										 ((CLKOUT) == CLKOUT_FCLK_Prescaler_4))
// Bit[4:2] SDCS[1:0]
#define SDCS_OFF						0x00
#define SDCS_0P5UA						0x08
#define SDCS_2UA						0x10
#define SDCS_10UA						0x18
#define IS_ADC_SDCS(SDCS)				(((SDCS) == SDCS_OFF  ) || \
										 ((SDCS) == SDCS_0P5UA) || \
										 ((SDCS) == SDCS_2UA  ) || \
										 ((SDCS) == SDCS_10UA ))
// Bit[2:0] PGA[2:0]	
#define PGA_1							0x00
#define PGA_2							0x01
#define PGA_4							0x02
#define PGA_8							0x03
#define PGA_16							0x04
#define PGA_32							0x05
#define PGA_64							0x06
#define IS_ADC_PGA(PGA)					(((PGA) == PGA_1 ) || \
										 ((PGA) == PGA_2 ) || \
										 ((PGA) == PGA_4 ) || \
										 ((PGA) == PGA_8 ) || \
										 ((PGA) == PGA_16) || \
										 ((PGA) == PGA_32) || \
										 ((PGA) == PGA_64))

// DRATE: A/D Data Rate
#define DR_60000SPS						0xf1
#define DR_30000SPS						0xf0
#define DR_15000SPS						0xe0
#define DR_7500SPS						0xd0
#define DR_3750SPS						0xc0
#define DR_2000SPS						0xb0
#define DR_1000SPS						0xa1
#define DR_500SPS						0x92
#define DR_100SPS						0x82
#define DR_60SPS						0x72
#define DR_50SPS						0x63
#define DR_30SPS						0x53
#define DR_25SPS						0x43
#define DR_15SPS						0x33
#define DR_10SPS						0x23
#define DR_5SPS							0x13
#define DR_2p5SPS						0x03
#define IS_ADC_DRATE(DRATE)				(((DRATE) == DR_60000SPS) || \
										 ((DRATE) == DR_30000SPS) || \
										 ((DRATE) == DR_15000SPS) || \
										 ((DRATE) == DR_7500SPS ) || \
										 ((DRATE) == DR_3750SPS ) || \
										 ((DRATE) == DR_2000SPS ) || \
										 ((DRATE) == DR_1000SPS ) || \
										 ((DRATE) == DR_500SPS  ) || \
										 ((DRATE) == DR_100SPS  ) || \
										 ((DRATE) == DR_60SPS   ) || \
										 ((DRATE) == DR_50SPS   ) || \
										 ((DRATE) == DR_30SPS   ) || \
										 ((DRATE) == DR_25SPS   ) || \
										 ((DRATE) == DR_15SPS   ) || \
										 ((DRATE) == DR_10SPS   ) || \
										 ((DRATE) == DR_5SPS    ) || \
										 ((DRATE) == DR_2p5SPS  ))

// IO: GPIO Control Register
#define Direction_Input					0x01
#define Direction_Output				0x00
#define IS_ADC_IO_Direction(Direction)	(((Direction) == Direction_Input) || \
										 ((Direction) == Direction_Output))


/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
typedef struct
{
	uint8_t ORDER;
	uint8_t ACAL;
	uint8_t BUFEN;
} REG_STATUS_INIT_STRUCT;

typedef struct
{
	uint8_t PSEL;
	uint8_t NSEL;
} REG_MUX_INIT_STRUCT;

typedef struct
{
	uint8_t CLK;
	uint8_t SDCS;
	uint8_t PGA;
} REG_ADCON_INIT_STRUCT;

typedef struct
{
	uint8_t DR;
} REG_DRATE_INIT_STRUCT;

typedef struct
{
	uint8_t DIR3;
	uint8_t DIR2;
	uint8_t DIR1;
	uint8_t DIR0;
	uint8_t DIO3;
	uint8_t DIO2;
	uint8_t DIO1;
	uint8_t DIO0;
} REG_IO_INIT_STRUCT;

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/
extern uint32_t ADC1_DATA[8];
extern uint32_t ADC2_DATA[8];
extern uint32_t ADC3_DATA[8];

extern uint32_t ADC1_DATA_AVG[8];
extern uint32_t ADC2_DATA_AVG[8];
extern uint32_t ADC3_DATA_AVG[8];

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/
void SGM58601_INIT(SPI_HandleTypeDef* spi_handler);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
