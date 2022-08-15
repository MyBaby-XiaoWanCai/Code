#ifndef __BSP_SGM58601_H
#define __BSP_SGM58601_H

#include "main.h"

#define SGM58601_SPI_UNIT         (M4_SPI2)
#define SGM58601_UNIT_CLOCK       PWC_FCG1_SPI2

#define SGM58601_CS_PORT         (GPIO_PORT_I)
#define SGM58601_CS_PIN          (GPIO_PIN_00)

#define SGM58601_SCK_PORT         (GPIO_PORT_I)
#define SGM58601_SCK_PIN          (GPIO_PIN_01)
#define SGM58601_SCK_FUNC         (GPIO_FUNC_43_SPI2_SCK)

#define SGM58601_MISO_PORT        (GPIO_PORT_I)
#define SGM58601_MISO_PIN         (GPIO_PIN_02)
#define SGM58601_MISO_FUNC        (GPIO_FUNC_45_SPI2_MISO)

#define SGM58601_MOSI_PORT        (GPIO_PORT_I)
#define SGM58601_MOSI_PIN         (GPIO_PIN_03)
#define SGM58601_MOSI_FUNC        (GPIO_FUNC_44_SPI2_MOSI)

#define SGM58601_EXTI_PORT        (GPIO_PORT_E)
#define SGM58601_EXTI_PIN         (GPIO_PIN_01)
#define SGM58601_EXINT_CH         (EXINT_CH01)
#define SGM58601_INT_SRC          (INT_PORT_EIRQ1)
#define SGM58601_IRQn             (Int002_IRQn)

#define STATUS_REG_ADDR 0x00
#define MUX_REG_ADDR    0x01
#define ADCON_REG_ADDR  0x02
#define DRATE_REG_ADDR  0x03
#define IO_REG_ADDR     0x04
#define OFC0_REG_ADDR 	0x05
#define OFC1_REG_ADDR   0x06
#define OFC2_REG_ADDR   0x07
#define FSC0_REG_ADDR   0x08
#define FSC1_REG_ADDR   0x09
#define FSC2_REG_ADDR   0x0A

#define WAKEUP_COMMAND   0x00
#define RDATA_COMMAND    0x01
#define RDATAC_COMMAND   0x03
#define SDATAC_COMMAND   0x0F
//#define RREG_COMMAND   0x1x
//#define WREG_COMMAND   0x5x
#define SELFCAL_COMMAND  0xF0
#define SELFOCAL_COMMAND 0xF1
#define SELFGCAL_COMMAND 0xF2
#define SYSOCAL_COMMAND  0xF3
#define SYSGCAL_COMMAND  0xF4
#define SYNC_COMMAND     0xFC
#define STANDBY_COMMAND  0xFD
#define RESET_COMMAND    0xFE
//#define WAKEUP_COMMAND   0xFF


#define MSB_FIRST                   ((uint8_t)0x00)
#define LSB_FIRST                   ((uint8_t)0x08)
#define IS_ADC_ORDER(ORDER) (((ORDER) == MSB_FIRST) || \
                             ((ORDER) == LSB_FIRST))

#define AUTO_CALI_EN                   ((uint8_t)0x04)
#define AUTO_CALI_DISABLE              ((uint8_t)0x00)
#define IS_ADC_AUTO_CALI(AUTO_CALI) (((AUTO_CALI) == AUTO_CALI_EN ) || \
                                     ((AUTO_CALI) == AUTO_CALI_DISABLE))

#define BUFFER_EN                   ((uint8_t)0x02)
#define BUFFER_DISABLE             ((uint8_t)0x00)
#define IS_ADC_BUFFER(BUFFER) (((BUFFER) == BUFFER_EN) || \
                               ((BUFFER) == BUFFER_DISABLE))

#define DR_60000SPS 0xf1
#define DR_30000SPS 0xf0
#define DR_15000SPS 0xe0
#define DR_7500SPS  0xd0
#define DR_3750SPS  0xc0
#define DR_2000SPS  0xb0
#define DR_1000SPS  0xa1
#define DR_500SPS   0x92
#define DR_100SPS   0x82
#define DR_60SPS    0x72
#define DR_50SPS    0x63
#define DR_30SPS    0x53
#define DR_25SPS    0x43
#define DR_15SPS    0x33
#define DR_10SPS    0x23
#define DR_5SPS     0x13
#define DR_2p5SPS   0x03
#define IS_ADC_DRATE(DRATE) (((DRATE) == DR_60000SPS ) || \
                            ((DRATE) == DR_30000SPS ) || \
														((DRATE) == DR_15000SPS ) || \
														((DRATE) == DR_7500SPS ) || \
														((DRATE) == DR_3750SPS ) || \
														((DRATE) == DR_2000SPS ) || \
														((DRATE) == DR_1000SPS ) || \
														((DRATE) == DR_500SPS ) || \
														((DRATE) == DR_100SPS ) || \
                            ((DRATE) == DR_60SPS ) || \
														((DRATE) == DR_50SPS ) || \
														((DRATE) == DR_30SPS ) || \
														((DRATE) == DR_25SPS ) || \
														((DRATE) == DR_15SPS ) || \
														((DRATE) == DR_10SPS ) || \
														((DRATE) == DR_5SPS ) || \
														((DRATE) == DR_2p5SPS ) )

#define AIN0 0x00
#define AIN1 0x01
#define AIN2 0x02
#define AIN3 0x03
#define AIN4 0x04
#define AIN5 0x05
#define AIN6 0x06
#define AIN7 0x07
#define AINCOM 0x08
#define IS_ADC_AIN(AIN) (((AIN) == AIN0 ) || \
												((AIN) == AIN1 ) || \
												((AIN) == AIN2 ) || \
												((AIN) == AIN3 ) || \
												((AIN) == AIN4 ) || \
												((AIN) == AIN5 ) || \
												((AIN) == AIN6 ) || \
												((AIN) == AIN7 ) || \
												((AIN) == AINCOM ) )

#define PGA_1  0x00
#define PGA_2  0x01
#define PGA_4  0x02
#define PGA_8  0x03
#define PGA_16 0x04
#define PGA_32 0x05
#define PGA_64 0x06
#define IS_ADC_PGA(PGA)    (((PGA) == PGA_1 ) || \
                            ((PGA) == PGA_2 ) || \
														((PGA) == PGA_4 ) || \
														((PGA) == PGA_8 ) || \
														((PGA) == PGA_16 ) || \
														((PGA) == PGA_32 ) || \
														((PGA) == PGA_64 ) )

#define SDCS_OFF   0x00
#define SDCS_0P5UA 0x08
#define SDCS_2UA   0x10
#define SDCS_10UA  0x18
#define IS_ADC_SDCS(SDCS)   (((SDCS) == SDCS_OFF ) || \
                            ((SDCS) == SDCS_0P5UA ) || \
														((SDCS) == SDCS_2UA ) || \
														((SDCS) == SDCS_10UA ) )

#define CLKOUT_OFF   0x00
#define CLKOUT_FCLK_Prescaler_1  0x20
#define CLKOUT_FCLK_Prescaler_2  0x40
#define CLKOUT_FCLK_Prescaler_4  0x60
#define IS_ADC_CLKOUT(CLKOUT)  (((CLKOUT) == CLKOUT_OFF ) || \
																((CLKOUT) == CLKOUT_FCLK_Prescaler_1 ) || \
																((CLKOUT) == CLKOUT_FCLK_Prescaler_2 ) || \
																((CLKOUT) == CLKOUT_FCLK_Prescaler_4 ) )

#define Direction_Input 0x01
#define Direction_Output 0x00
#define IS_ADC_IO_Direction(Direction) (((Direction) == Direction_Input) || \
                                       ((Direction) == Direction_Output))

#define SYNC 				PAout(1)
#define ADC_INPUT  	PCout(3)

#define SGM58601_CS_1()          GPIO_SetPins(SGM58601_CS_PORT, SGM58601_CS_PIN)
#define SGM58601_CS_0()          GPIO_ResetPins(SGM58601_CS_PORT, SGM58601_CS_PIN)

typedef struct
{
    uint8_t ORDER;
    uint8_t AUTO_CALI;
    uint8_t BUFFER;
} STATUS_REGInit_Struct;


typedef struct
{
    uint8_t AIN_P;
    uint8_t AIN_N;
} MUX_REGInit_Struct;


typedef struct
{
    uint8_t CLK_OUT;
    uint8_t SENSOR_DETECT;
    uint8_t PGA;
} ADCON_REGInit_Struct;

typedef struct
{
    uint8_t DRATE;
} DRATE_REGInit_Struct;

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
} GPIO_REGInit_Struct;



void ADC_WHITE_TO_ONE_REG(uint8_t reg_addr,uint8_t reg_data);
uint8_t ADC_READ_FROM_ONE_REG(uint8_t reg_addr);

void ADC_RDATA_COMMAND(void);  //DRDY为低时执行，读取ADC转换数据命令
void ADC_RDATAC_COMMAND(void); //DRDY为低时执行，连续读取ADC转换数据命令
void ADC_SDATAC_COMMAND(void); //DRDY为低时执行，停止连续读取ADC

uint32_t ADC_GET_CONDATA(void);
uint32_t ADC_GET_OFCDATA(void);
uint32_t ADC_GET_FSCDATA(void);

void ADC_SYNC_COMMAND(void);
void ADC_WAKEUP_COMMAND(void);
void ADC_STANDBY_COMMAND(void);
void ADC_RESET_COMMAND(void);

void ADC_SELF_CALIBRATION(void);
void ADC_SELF_OFFSET_CALIBRATION(void);
void ADC_SELF_GAIN_CALIBRATION(void);
void ADC_SYS_OFFSET_CALIBRATION(void);
void ADC_SYS_GAIN_CALIBRATION(void);

//void ADC_ONE_SHOT_MODE(void);  //单次采样模式

void ADC_SET_CHANNEL(uint8_t AIN_P,uint8_t AIN_N);
void ADC_SET_PGA(uint8_t PGA);
void ADC_SET_DATARATE(uint8_t DATARATE);
void ADC_SET_SDCS(uint8_t SDCS_UA);
void ADC_SET_CLKOUT(uint8_t CLKOUT_F);

void ADC_EN_INPUTBUFFER(void);
void ADC_DISABLE_INPUTBUFFER(void);
void ADC_EN_AUTOCALIBRATION(void);
void ADC_DISABLE_AUTOCALIBRATION(void);
void ADC_MSB_FIRST(void);
void ADC_LSB_FIRST(void);

void ADC_SGM58601_Init(void);
void SGM58601_EXTIX_Init(void);
void EXTI3_IRQHandler(void);

void bsp_SGM58601Init(void);
void SGM58601_Switch_Ch(uint8_t CH);
uint32_t SGM58601_Read_Data(void);
uint8_t SGM58601_Read_Reg(uint8_t reg_add);
void SGM58601_Write_Reg(uint8_t reg_add ,uint8_t Date);

extern uint32_t SGM58601_Data[8];

#endif

