#ifndef __DISCHARGE_H__
#define __DISCHARGE_H__

#include "sys.h"
#include "stm32f4xx_hal.h"

#define DAC80508_CH1 1
#define DAC80508_CH2 2
#define DAC80508_CH3 3
#define DAC80508_CH4 4
#define DAC80508_CH5 5
#define DAC80508_CH6 6
#define DAC80508_CH7 7
#define DAC80508_CH8 8

#define PORT(n) n+1

//通道使能
#define PORT1_EN {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9, GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);}
#define PORT2_EN {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);}
#define PORT3_EN {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);}
#define PORT4_EN {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_SET);}
#define PORT5_EN {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_SET);}
#define PORT6_EN {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_SET);}
#define PORT7_EN {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_SET);}
#define PORT8_EN {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);}

#define PORT1A_EN {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9, GPIO_PIN_SET);}
#define PORT2A_EN {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_SET);}
#define PORT3A_EN {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_SET);}
#define PORT4A_EN {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_SET);}
#define PORT5A_EN {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);}
#define PORT6A_EN {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);}
#define PORT7A_EN {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);}
#define PORT8A_EN {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);}

//#define PORT1B_EN {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);}
//#define PORT2B_EN {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);}
//#define PORT3B_EN {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);}
//#define PORT4B_EN {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_SET);}
//#define PORT5B_EN {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_SET);}
//#define PORT6B_EN {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_SET);}
//#define PORT7B_EN {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_SET);}
//#define PORT8B_EN {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);}

#define PORT_EN(n)  {switch(n+1) { \
                        case 1: PORT1_EN;break; \
                        case 2: PORT2_EN;break; \
                        case 3: PORT3_EN;break; \
                        case 4: PORT4_EN;break; \
                        case 5: PORT5_EN;break; \
                        case 6: PORT6_EN;break; \
                        case 7: PORT7_EN;break; \
                        case 8: PORT8_EN;break; \
                        default: break; }}

//通道失能
#define PORT1_DIS {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);}
#define PORT2_DIS {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);}
#define PORT3_DIS {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);}
#define PORT4_DIS {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_RESET);}
#define PORT5_DIS {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_RESET);}
#define PORT6_DIS {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_RESET);}
#define PORT7_DIS {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_RESET);}
#define PORT8_DIS {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);}

#define PORT1A_DIS {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9, GPIO_PIN_RESET);}
#define PORT2A_DIS {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_RESET);}
#define PORT3A_DIS {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_RESET);}
#define PORT4A_DIS {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_RESET);}
#define PORT5A_DIS {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);}
#define PORT6A_DIS {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);}
#define PORT7A_DIS {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);}
#define PORT8A_DIS {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);}

//#define PORT1B_DIS {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);}
//#define PORT2B_DIS {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);}
//#define PORT3B_DIS {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);}
//#define PORT4B_DIS {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_RESET);}
//#define PORT5B_DIS {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_RESET);}
//#define PORT6B_DIS {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_RESET);}
//#define PORT7B_DIS {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_RESET);}
//#define PORT8B_DIS {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);}

#define PORT_DIS(n) {switch(n+1) { \
                        case 1: PORT1_DIS;break; \
                        case 2: PORT2_DIS;break; \
                        case 3: PORT3_DIS;break; \
                        case 4: PORT4_DIS;break; \
                        case 5: PORT5_DIS;break; \
                        case 6: PORT6_DIS;break; \
                        case 7: PORT7_DIS;break; \
                        case 8: PORT8_DIS;break; \
                        default: break; }}

//通道方向切换
#define BUCK_MODE   1
#define BOOST_MODE  0

#define BUCK_SET    0
#define BOOST_SET   1

extern uint8_t Dir_flag[8];

#define PORT1_DIR   PEout(5)
#define PORT2_DIR   PEout(6)
#define PORT3_DIR   PIout(8)
#define PORT4_DIR   PCout(13)
#define PORT5_DIR   PIout(9)
#define PORT6_DIR   PFout(0)
#define PORT7_DIR   PFout(1)
#define PORT8_DIR   PFout(2)

//将通道设为 BUCK 模式
#define PORT1_BUCK {HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);}
#define PORT2_BUCK {HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);}
#define PORT3_BUCK {HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_RESET);}
#define PORT4_BUCK {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);}
#define PORT5_BUCK {HAL_GPIO_WritePin(GPIOI, GPIO_PIN_9, GPIO_PIN_RESET);}
#define PORT6_BUCK {HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET);}
#define PORT7_BUCK {HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET);}
#define PORT8_BUCK {HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_RESET);}

//将通道设为 BOOST 模式
#define PORT1_BOOST {HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);}
#define PORT2_BOOST {HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);}
#define PORT3_BOOST {HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_SET);}
#define PORT4_BOOST {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);}
#define PORT5_BOOST {HAL_GPIO_WritePin(GPIOI, GPIO_PIN_9, GPIO_PIN_SET);}
#define PORT6_BOOST {HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET);}
#define PORT7_BOOST {HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET);}
#define PORT8_BOOST {HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_SET);}



#define DAC80508_Vref   5
#define DAC80508_Iref   12

#define V_MAX 4.5
#define V_MIN 2.5
#define I_MAX 161.0
// #define I_MAX 80.0
#define I_RANGE 10.0

#define Ich_MAX I_MAX
#define Iport_MAX   Ich_MAX/(8/CHANNELS)
// #define Iport_MAX   40

//电流高低档切换
#define I_HIGH  0
#define I_LOW   1

#define I_HIGH_SET  1
#define I_LOW_SET   0

extern uint8_t Irange_flag[8];

#define I1_OUT  PDout(2)
#define I2_OUT  PDout(3)
#define I3_OUT  PDout(4)
#define I4_OUT  PDout(5)
#define I5_OUT  PDout(6)
#define I6_OUT  PDout(7)
#define I7_OUT  PGout(9)
#define I8_OUT  PGout(10)

#define I1OUT_H {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);}
#define I2OUT_H {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);}
#define I3OUT_H {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);}
#define I4OUT_H {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET);}
#define I5OUT_H {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);}
#define I6OUT_H {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);}
#define I7OUT_H {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_SET);}
#define I8OUT_H {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);}

#define I1OUT_L {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);}
#define I2OUT_L {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);}
#define I3OUT_L {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);}
#define I4OUT_L {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET);}
#define I5OUT_L {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);}
#define I6OUT_L {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);}
#define I7OUT_L {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_RESET);}
#define I8OUT_L {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);}

//电压模式切换
#define V_SENSE 1
#define V_MOD   0

#define V_SENSE_SET 0
#define V_MOD_SET   1

extern uint8_t Vrange_flag[8];

#define V1_OUT  PHout(13)
#define V2_OUT  PHout(14)
#define V3_OUT  PGout(15)
#define V4_OUT  PBout(5)
#define V5_OUT  PBout(8)
#define V6_OUT  PBout(9)
#define V7_OUT  PIout(4)
#define V8_OUT  PIout(5)

#define V1_SENSE    {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_13, GPIO_PIN_RESET);}
#define V2_SENSE    {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_RESET);}
#define V3_SENSE    {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_15, GPIO_PIN_RESET);}
#define V4_SENSE    {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15, GPIO_PIN_RESET);}
#define V5_SENSE    {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);}
#define V6_SENSE    {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);}
#define V7_SENSE    {HAL_GPIO_WritePin(GPIOI, GPIO_PIN_4, GPIO_PIN_RESET);}
#define V8_SENSE    {HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_RESET);}

#define V1_MOD      {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_13, GPIO_PIN_SET);}
#define V2_MOD      {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_SET);}
#define V3_MOD      {HAL_GPIO_WritePin(GPIOH, GPIO_PIN_15, GPIO_PIN_SET);}
#define V4_MOD      {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15, GPIO_PIN_SET);}
#define V5_MOD      {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);}
#define V6_MOD      {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);}
#define V7_MOD      {HAL_GPIO_WritePin(GPIOI, GPIO_PIN_4, GPIO_PIN_SET);}
#define V8_MOD      {HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_SET);}

//防反接模式控制
#define REV_ENABLE  0
#define REV_DISABLE 1

#define REV_PORT1   PGout(7)
#define REV_PORT2   PGout(8)
#define REV_PORT3   PCout(6)
#define REV_PORT4   PCout(7)
#define REV_PORT5   PCout(8)
#define REV_PORT6   PCout(9)
#define REV_PORT7   PAout(8)
#define REV_PORT8   PAout(9)

#define REV_PORT1_EN    {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);}
#define REV_PORT2_EN    {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_RESET);}
#define REV_PORT3_EN    {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);}
#define REV_PORT4_EN    {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);}
#define REV_PORT5_EN    {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);}
#define REV_PORT6_EN    {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);}
#define REV_PORT7_EN    {HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);}
#define REV_PORT8_EN    {HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);}

#define REV_PORT_EN(n)  {switch(n+1) { \
                            case 1: REV_PORT1_EN;break; \
                            case 2: REV_PORT2_EN;break; \
                            case 3: REV_PORT3_EN;break; \
                            case 4: REV_PORT4_EN;break; \
                            case 5: REV_PORT5_EN;break; \
                            case 6: REV_PORT6_EN;break; \
                            case 7: REV_PORT7_EN;break; \
                            case 8: REV_PORT8_EN;break; \
                            default: break; }}

#define REV_PORT1_DIS   {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);}
#define REV_PORT2_DIS   {HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_SET);}
#define REV_PORT3_DIS   {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);}
#define REV_PORT4_DIS   {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);}
#define REV_PORT5_DIS   {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);}
#define REV_PORT6_DIS   {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);}
#define REV_PORT7_DIS   {HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);}
#define REV_PORT8_DIS   {HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);}

#define REV_PORT_DIS(n) {switch(n+1) { \
                            case 1: REV_PORT1_DIS;break; \
                            case 2: REV_PORT2_DIS;break; \
                            case 3: REV_PORT3_DIS;break; \
                            case 4: REV_PORT4_DIS;break; \
                            case 5: REV_PORT5_DIS;break; \
                            case 6: REV_PORT6_DIS;break; \
                            case 7: REV_PORT7_DIS;break; \
                            case 8: REV_PORT8_DIS;break; \
                            default: break; }}

// //模块故障信号读取
// #define PG1 PGin(0)
// #define PG2 PFin(15)
// #define PG3 PFin(14)
// #define PG4 PFin(13)
// #define PG5 PFin(12)
// #define PG6 PFin(11)
// #define PG7 PBin(1)
// #define PG8 PBin(0)

#define PG1_READ {HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_0);}
#define PG2_READ {HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_15);}
#define PG3_READ {HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_14);}
#define PG4_READ {HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_13);}
#define PG5_READ {HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_12);}
#define PG6_READ {HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_11);}
#define PG7_READ {HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);}
#define PG8_READ {HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);}


void Discharge_Init(void);
void Discharge_Control(uint8_t port, uint8_t dir, float V_Set, float I_Set);

void Calibration_Read(void);
void Calibration_Run(void);

#endif
