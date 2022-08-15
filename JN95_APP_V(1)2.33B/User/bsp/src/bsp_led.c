/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_led.c
** Author      : Cai Heng
** Date        : 2021-08-17
** Description : This file is used to initialize and drive led
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void bsp_InitLed(void);
uint8_t Led_Shine_Ctrl(uint8_t _no, uint16_t t_on, uint16_t t_off, uint16_t t_scale);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : bsp_InitLed()
**	功能说明 : 配置LED指示灯相关的GPIO
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitLed(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* 打开GPIO时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /*
    	配置所有的LED指示灯GPIO为推挽输出模式
    	由于将GPIO设置为输出时,GPIO输出寄存器的值缺省是0,
    	因此会驱动LED点亮,这是我们所不希望的,
    	因此在改变GPIO为输出前,先关闭LED指示灯
    */

    /* 配置LED */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;			/* 设置推挽输出 */
    GPIO_InitStruct.Pull = GPIO_PULLUP;					/* 上拉电阻*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;		/* GPIO速度等级 */

//    GPIO_InitStruct.Pin	= GPIO_PIN_LED81;
//    HAL_GPIO_Init(GPIO_PORT_LED81, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_LED82;
    HAL_GPIO_Init(GPIO_PORT_LED82, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_LED83;
    HAL_GPIO_Init(GPIO_PORT_LED83, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	= RUN_LED_PIN_R;
    HAL_GPIO_Init(RUN_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RUN_LED_PIN_G;
    HAL_GPIO_Init(RUN_LED_PORT_G, &GPIO_InitStruct);
    
#ifdef YCJ
    GPIO_InitStruct.Pin = CH1_LED_PIN_R;
    HAL_GPIO_Init(CH1_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH1_LED_PIN_G;
    HAL_GPIO_Init(CH1_LED_RORT_G, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH2_LED_PIN_R;
    HAL_GPIO_Init(CH2_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH2_LED_PIN_G;
    HAL_GPIO_Init(CH2_LED_PORT_G, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH3_LED_PIN_R;
    HAL_GPIO_Init(CH3_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH3_LED_PIN_G;
    HAL_GPIO_Init(CH3_LED_PORT_G, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH4_LED_PIN_R;
    HAL_GPIO_Init(CH4_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH4_LED_PIN_G;
    HAL_GPIO_Init(CH4_LED_PORT_G, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = CH5_LED_PIN_R;
    HAL_GPIO_Init(CH5_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH5_LED_PIN_G;
    HAL_GPIO_Init(CH5_LED_PORT_G, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH6_LED_PIN_R;
    HAL_GPIO_Init(CH6_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH6_LED_PIN_G;
    HAL_GPIO_Init(CH6_LED_PORT_G, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH7_LED_PIN_R;
    HAL_GPIO_Init(CH7_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH7_LED_PIN_G;
    HAL_GPIO_Init(CH7_LED_PORT_G, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH8_LED_PIN_R;
    HAL_GPIO_Init(CH8_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH8_LED_PIN_G;
    HAL_GPIO_Init(CH8_LED_PORT_G, &GPIO_InitStruct);
#else
    GPIO_InitStruct.Pin = CH1_LED_PIN_R;
    HAL_GPIO_Init(CH1_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH1_LED_PIN_G;
    HAL_GPIO_Init(CH1_LED_RORT_G, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH2_LED_PIN_R;
    HAL_GPIO_Init(CH2_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH2_LED_PIN_G;
    HAL_GPIO_Init(CH2_LED_PORT_G, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH3_LED_PIN_R;
    HAL_GPIO_Init(CH3_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH3_LED_PIN_G;
    HAL_GPIO_Init(CH3_LED_PORT_G, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH4_LED_PIN_R;
    HAL_GPIO_Init(CH4_LED_PORT_R, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CH4_LED_PIN_G;
    HAL_GPIO_Init(CH4_LED_PORT_G, &GPIO_InitStruct);
#endif    
}

/**********************************************************************************************************
**	函 数 名 : bsp_LedOff()
**	功能说明 : 熄灭指定的LED指示灯
**	形    参 : _no : 指示灯序号(0~12)
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_LedOff(uint8_t _no)
{
    if (_no == RUN_LED_R_NO)
    {
        RUN_LED_PORT_R->BSRR = (uint32_t)RUN_LED_PIN_R << 16U;
    }
    else if (_no == RUN_LED_G_NO)
    {
        RUN_LED_PORT_G->BSRR = (uint32_t)RUN_LED_PIN_G << 16U;
    }
    else if (_no == CH1_LED_R_NO)
    {
        CH1_LED_PORT_R->BSRR = (uint32_t)CH1_LED_PIN_R << 16U;
    }
    else if (_no == CH1_LED_G_NO)
    {
        CH1_LED_RORT_G->BSRR = (uint32_t)CH1_LED_PIN_G << 16U;
    }
    else if (_no == CH2_LED_R_NO)
    {
        CH2_LED_PORT_R->BSRR = (uint32_t)CH2_LED_PIN_R << 16U;
    }
    else if (_no == CH2_LED_G_NO)
    {
        CH2_LED_PORT_G->BSRR = (uint32_t)CH2_LED_PIN_G << 16U;
    }
    else if (_no == CH3_LED_R_NO)
    {
        CH3_LED_PORT_R->BSRR = (uint32_t)CH3_LED_PIN_R << 16U;
    }
    else if (_no == CH3_LED_G_NO)
    {
        CH3_LED_PORT_G->BSRR = (uint32_t)CH3_LED_PIN_G << 16U;
    }
    else if (_no == CH4_LED_R_NO)
    {
        CH4_LED_PORT_R->BSRR = (uint32_t)CH4_LED_PIN_R << 16U;
    }
    else if (_no == CH4_LED_G_NO)
    {
        CH4_LED_PORT_G->BSRR = (uint32_t)CH4_LED_PIN_G << 16U;
    }
#ifdef YCJ    
    else if (_no == CH5_LED_R_NO)
    {
        CH5_LED_PORT_R->BSRR = (uint32_t)CH5_LED_PIN_R << 16U;
    }
    else if (_no == CH5_LED_G_NO)
    {
        CH5_LED_PORT_G->BSRR = (uint32_t)CH5_LED_PIN_G << 16U;
    }
    else if (_no == CH6_LED_R_NO)
    {
        CH6_LED_PORT_R->BSRR = (uint32_t)CH6_LED_PIN_R << 16U;
    }
    else if (_no == CH6_LED_G_NO)
    {
        CH6_LED_PORT_G->BSRR = (uint32_t)CH6_LED_PIN_G << 16U;
    }
    else if (_no == CH7_LED_R_NO)
    {
        CH7_LED_PORT_R->BSRR = (uint32_t)CH7_LED_PIN_R << 16U;
    }
    else if (_no == CH7_LED_G_NO)
    {
        CH7_LED_PORT_G->BSRR = (uint32_t)CH7_LED_PIN_G << 16U;
    }
    else if (_no == CH8_LED_R_NO)
    {
        CH8_LED_PORT_R->BSRR = (uint32_t)CH8_LED_PIN_R << 16U;
    }
    else if (_no == CH8_LED_G_NO)
    {
        CH8_LED_PORT_G->BSRR = (uint32_t)CH8_LED_PIN_G << 16U;
    }
#endif

    else if (_no == LED_81_NO)
    {
//        GPIO_PORT_LED81->BSRR = GPIO_PIN_LED81;
    }
    else if (_no == LED_82_NO)
    {
        GPIO_PORT_LED82->BSRR = GPIO_PIN_LED82;
    }
    else if (_no == LED_83_NO)
    {
        GPIO_PORT_LED83->BSRR = GPIO_PIN_LED83;
    }
}

/**********************************************************************************************************
**	函 数 名 : bsp_LedOn()
**	功能说明 : 点亮指定的LED指示灯
**	形    参 : _no : 指示灯序号(0~12)
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_LedOn(uint8_t _no)
{
    if (_no == RUN_LED_R_NO)
    {
        RUN_LED_PORT_R->BSRR = RUN_LED_PIN_R;
    }
    else if (_no == RUN_LED_G_NO)
    {
        RUN_LED_PORT_G->BSRR = RUN_LED_PIN_G;
    }
    else if (_no == CH1_LED_R_NO)
    {
        CH1_LED_PORT_R->BSRR = CH1_LED_PIN_R;
    }
    else if (_no == CH1_LED_G_NO)
    {
        CH1_LED_RORT_G->BSRR = CH1_LED_PIN_G;
    }
    else if (_no == CH2_LED_R_NO)
    {
        CH2_LED_PORT_R->BSRR = CH2_LED_PIN_R;
    }
    else if (_no == CH2_LED_G_NO)
    {
        CH2_LED_PORT_G->BSRR = CH2_LED_PIN_G;
    }
    else if (_no == CH3_LED_R_NO)
    {
        CH3_LED_PORT_R->BSRR = CH3_LED_PIN_R;
    }
    else if (_no == CH3_LED_G_NO)
    {
        CH3_LED_PORT_G->BSRR = CH3_LED_PIN_G;
    }
    else if (_no == CH4_LED_R_NO)
    {
        CH4_LED_PORT_R->BSRR = CH4_LED_PIN_R;
    }
    else if (_no == CH4_LED_G_NO)
    {
        CH4_LED_PORT_G->BSRR = CH4_LED_PIN_G;
    }
#ifdef YCJ    
    else if (_no == CH5_LED_R_NO)
    {
        CH5_LED_PORT_R->BSRR = CH5_LED_PIN_R;
    }
    else if (_no == CH5_LED_G_NO)
    {
        CH5_LED_PORT_G->BSRR = CH5_LED_PIN_G;
    }
    else if (_no == CH6_LED_R_NO)
    {
        CH6_LED_PORT_R->BSRR = CH6_LED_PIN_R;
    }
    else if (_no == CH6_LED_G_NO)
    {
        CH6_LED_PORT_G->BSRR = CH6_LED_PIN_G;
    }
    else if (_no == CH7_LED_R_NO)
    {
        CH7_LED_PORT_R->BSRR = CH7_LED_PIN_R;
    }
    else if (_no == CH7_LED_G_NO)
    {
        CH7_LED_PORT_G->BSRR = CH7_LED_PIN_G;
    }
    else if (_no == CH8_LED_R_NO)
    {
        CH8_LED_PORT_R->BSRR = CH8_LED_PIN_R;
    }
    else if (_no == CH8_LED_G_NO)
    {
        CH8_LED_PORT_G->BSRR = CH8_LED_PIN_G;
    }
#endif    
    else if (_no == LED_81_NO)
    {
//        GPIO_PORT_LED81->BSRR = (uint32_t)GPIO_PIN_LED81 << 16U;
    }
    else if (_no == LED_82_NO)
    {
        GPIO_PORT_LED82->BSRR = (uint32_t)GPIO_PIN_LED82 << 16U;
    }
    else if (_no == LED_83_NO)
    {
        GPIO_PORT_LED83->BSRR = (uint32_t)GPIO_PIN_LED83 << 16U;
    }
}

/**********************************************************************************************************
**	函 数 名 : bsp_LedToggle()
**	功能说明 : 翻转指定的LED指示灯
**	形    参 : _no : 指示灯序号(0~12)
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_LedToggle(uint8_t _no)
{
    if (_no == RUN_LED_R_NO)
    {
        RUN_LED_PORT_R->ODR ^= RUN_LED_PIN_R;
    }
    else if (_no == RUN_LED_G_NO)
    {
        RUN_LED_PORT_G->ODR ^= RUN_LED_PIN_G;
    }
    else if (_no == CH1_LED_R_NO)
    {
        CH1_LED_PORT_R->ODR ^= CH1_LED_PIN_R;
    }
    else if (_no == CH1_LED_G_NO)
    {
        CH1_LED_RORT_G->ODR ^= CH1_LED_PIN_G;
    }
    else if (_no == CH2_LED_R_NO)
    {
        CH2_LED_PORT_R->ODR ^= CH2_LED_PIN_R;
    }
    else if (_no == CH2_LED_G_NO)
    {
        CH2_LED_PORT_G->ODR ^= CH2_LED_PIN_G;
    }
    else if (_no == CH3_LED_R_NO)
    {
        CH3_LED_PORT_R->ODR ^= CH3_LED_PIN_R;
    }
    else if (_no == CH3_LED_G_NO)
    {
        CH3_LED_PORT_G->ODR ^= CH3_LED_PIN_G;
    }
    else if (_no == CH4_LED_R_NO)
    {
        CH4_LED_PORT_R->ODR ^= CH4_LED_PIN_R;
    }
    else if (_no == CH4_LED_G_NO)
    {
        CH4_LED_PORT_G->ODR ^= CH4_LED_PIN_G;
    }
#ifdef YCJ    
    else if (_no == CH5_LED_R_NO)
    {
        CH5_LED_PORT_R->ODR ^= CH5_LED_PIN_R;
    }
    else if (_no == CH5_LED_G_NO)
    {
       CH5_LED_PORT_G->ODR ^= CH5_LED_PIN_G;
    }
    else if (_no == CH6_LED_R_NO)
    {
        CH6_LED_PORT_R->ODR ^= CH6_LED_PIN_R;
    }
    else if (_no == CH6_LED_G_NO)
    {
        CH6_LED_PORT_G->ODR ^= CH6_LED_PIN_G;
    }
    else if (_no == CH7_LED_R_NO)
    {
        CH7_LED_PORT_R->ODR ^= CH7_LED_PIN_R;
    }
    else if (_no == CH7_LED_G_NO)
    {
        CH7_LED_PORT_G->ODR ^= CH7_LED_PIN_G;
    }
    else if (_no == CH8_LED_R_NO)
    {
        CH8_LED_PORT_R->ODR ^= CH8_LED_PIN_R;
    }
    else if (_no == CH8_LED_G_NO)
    {
        CH8_LED_PORT_G->ODR ^= CH8_LED_PIN_G;
    }
#endif    
    else if (_no == LED_81_NO)
    {
//		GPIO_PORT_LED81->ODR ^= GPIO_PIN_LED81;
    }
    else if (_no == LED_82_NO)
    {
        GPIO_PORT_LED82->ODR ^= GPIO_PIN_LED82;
    }
    else if (_no == LED_83_NO)
    {
        GPIO_PORT_LED83->ODR ^= GPIO_PIN_LED83;
    }
}

/**********************************************************************************************************
**	函 数 名 : Led_Shine_Ctrl()
**	功能说明 : 根据不同的工作状态控制LED进行相应动作
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
uint8_t Led_Shine_Ctrl(uint8_t _no, uint16_t t_on, uint16_t t_off, uint16_t t_scale)
{
    static uint16_t g_usLedCtrlCnt[13] = {0};

    if(t_scale)
    {
        uint16_t cnt_on = t_on / t_scale;
        uint16_t cnt_off = t_off / t_scale;

        if(cnt_on == 0)
        {
            bsp_LedOff(_no);
        }
        else if(cnt_off == 0)
        {
            bsp_LedOn(_no);
        }
        else
        {
            if(g_usLedCtrlCnt[_no] == 0)
            {
                bsp_LedOn(_no);
            }
            if(g_usLedCtrlCnt[_no] == cnt_on)
            {
                bsp_LedOff(_no);
            }

            if(g_usLedCtrlCnt[_no] < (cnt_on + cnt_off))
            {
                g_usLedCtrlCnt[_no]++;
            }
            else
            {
                g_usLedCtrlCnt[_no] = 0;
            }
        }
    }
    else
    {
        return 1;
    }

    return 0;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
