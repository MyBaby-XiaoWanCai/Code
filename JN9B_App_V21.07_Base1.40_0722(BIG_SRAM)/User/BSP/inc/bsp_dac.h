/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_dac.h
** Author      : wang liang
** Date        : 2021-11-25
** Description : 
**********************************************************************************************************/

#ifndef __BSP_DAC_H
#define __BSP_DAC_H

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <stdint.h>
#include "main.h"

/**********************************************************************************************************
**                                         类型申明区
**********************************************************************************************************/
typedef enum
{
    DAC_Unit1,    
    DAC_Unit2,
    DAC_Unit_Max,
}en_dac_unit;

typedef enum
{
    E_Dac_Single,
    E_Dac_Dual,
}en_dac_cvt_t;

typedef struct
{
    M4_DAC_TypeDef* pUnit;
    en_dac_cvt_t enCvtType;
    uint16_t u16Ch;
}st_dac_handle_t;

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define DAC_UNIT1_PORT                               (GPIO_PORT_A)
#define DAC_UNIT2_PORT                               (GPIO_PORT_C)

#define DAC_UNIT1_CHN1_PIN                           (GPIO_PIN_04)
#define DAC_UNIT1_CHN2_PIN                           (GPIO_PIN_05)
#define DAC_UNIT2_CHN1_PIN                           (GPIO_PIN_04)
#define DAC_UNIT2_CHN2_PIN                           (GPIO_PIN_05)
#define VREFH                                        (3.3F)
#define DAC_CHN1                                     (0U)
#define DAC_CHN2                                     (1U)
#define DAC_DATA_ALIGN_12b_R                         (0U)
#define DAC_DATA_ALIGN_12b_L                         (1U)

/* Open AMP function on  BSP_EV_HC32F4A0_LQFP176 board */
#define SUPPORT_AMP
//#define SUPPORT_ADP
#define SINGLE_WAVE_DAC_CHN                          (DAC_CHN2)            //bovp 所在脚是PA5
#define DAC_DATA_ALIGN                               (DAC_DATA_ALIGN_12b_L)

#define SINE_DOT_NUMBER                              (4096U)
#define SINE_NEGATIVE_TO_POSITVE                     (1.0F)

#if (SINGLE_WAVE_DAC_CHN == DAC_CHN1)
#define pfSingleWaveSetData                          (DAC_SetChannel1Data)
#define pfSingleWaveConvState                        (DAC_GetChannel1ConvState)
#else
#define pfSingleWaveSetData                          (DAC_SetChannel2Data)
#define pfSingleWaveConvState                        (DAC_GetChannel2ConvState)
#endif

st_dac_handle_t* bsp_bovpInit(void);
void AD5245_Vpro_Set(st_dac_handle_t* pDac, float Vbovp);

extern st_dac_handle_t* Bovp;      //用来存储DAC初始化返回值
#ifdef __cplusplus
}
#endif	
	
#endif



/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

