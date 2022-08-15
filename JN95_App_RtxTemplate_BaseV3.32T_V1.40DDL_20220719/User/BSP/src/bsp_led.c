/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_led.c
** Author      : Huang Cheng
** Date        : 2021-11-05
** Description : This file is used to initialize and drive led
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_led.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                         静态变量定义区
**********************************************************************************************************/
//GPIO PORT查询表
static uint8_t LedGpioPortTab[LED_MAX_NUM+1] = 
{
#ifdef YCJ
    RUN_LED_PORT_R, RUN_LED_PORT_G, CH1_LED_PORT_R, CH1_LED_PORT_G,
    CH2_LED_PORT_R, CH2_LED_PORT_G, CH3_LED_PORT_R, CH3_LED_PORT_G,
    CH4_LED_PORT_R, CH4_LED_PORT_G, CH5_LED_PORT_R, CH5_LED_PORT_G,
    CH6_LED_PORT_R, CH6_LED_PORT_G, CH7_LED_PORT_R, CH7_LED_PORT_G,
    CH8_LED_PORT_R, CH8_LED_PORT_G, GPIO_PORT_NOUSE, GPIO_PORT_LED82,
    GPIO_PORT_LED83
#else
    RUN_LED_PORT_R, RUN_LED_PORT_G, CH1_LED_PORT_R, CH1_LED_PORT_G,
    CH2_LED_PORT_R, CH2_LED_PORT_G, CH3_LED_PORT_R, CH3_LED_PORT_G,
    CH4_LED_PORT_R, CH4_LED_PORT_G, GPIO_PORT_NOUSE, GPIO_PORT_LED82,
    GPIO_PORT_LED83
#endif
};
	
//GPIO PIN查询表
static uint16_t LedGpioPinTab[LED_MAX_NUM+1] =  
{
	#ifdef YCJ
	RUN_LED_PIN_R, RUN_LED_PIN_G, CH1_LED_PIN_R, CH1_LED_PIN_G,
	CH2_LED_PIN_R, CH2_LED_PIN_G, CH3_LED_PIN_R, CH3_LED_PIN_G,
	CH4_LED_PIN_R, CH4_LED_PIN_G, CH5_LED_PIN_R, CH5_LED_PIN_G,
	CH6_LED_PIN_R, CH6_LED_PIN_G, CH7_LED_PIN_R, CH7_LED_PIN_G,
	CH8_LED_PIN_R, CH8_LED_PIN_G, GPIO_PIN_NOUSE, GPIO_PIN_LED82,
	GPIO_PIN_LED83
	#else
	RUN_LED_PIN_R, RUN_LED_PIN_G, CH1_LED_PIN_R, CH1_LED_PIN_G,
	CH2_LED_PIN_R, CH2_LED_PIN_G, CH3_LED_PIN_R, CH3_LED_PIN_G,
	CH4_LED_PIN_R, CH4_LED_PIN_G, GPIO_PIN_NOUSE, GPIO_PIN_LED82,
	GPIO_PIN_LED83
	#endif
};

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
	stc_gpio_init_t stcLedGpioInit;
	
	/* 设置板载Led pin的属性:推挽输出、初始化状态为SET等 */
	(void)GPIO_StructInit(&stcLedGpioInit);
	stcLedGpioInit.u16PinState = PIN_STATE_SET;
	stcLedGpioInit.u16PullUp = PIN_PU_ON;
	stcLedGpioInit.u16PinDir = PIN_DIR_OUT;
	stcLedGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcLedGpioInit.u16PinOType = PIN_OTYPE_CMOS;
	
	/* 写入板载Led pin的属性 */
    (void)GPIO_Init(GPIO_PORT_LED82, GPIO_PIN_LED82, &stcLedGpioInit);
    (void)GPIO_Init(GPIO_PORT_LED83, GPIO_PIN_LED83, &stcLedGpioInit);
	
	/* 设置灯板Led pin的属性:推挽输出、初始化状态为RESET等 */
	(void)GPIO_StructInit(&stcLedGpioInit);
	stcLedGpioInit.u16PinState = PIN_STATE_RESET;
	stcLedGpioInit.u16PullUp = PIN_PU_ON;
	stcLedGpioInit.u16PinDir = PIN_DIR_OUT;
	stcLedGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcLedGpioInit.u16PinOType = PIN_OTYPE_CMOS;
	
	/* 写入灯板Led pin的属性 */
    (void)GPIO_Init(RUN_LED_PORT_R,  RUN_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(RUN_LED_PORT_G,  RUN_LED_PIN_G,  &stcLedGpioInit);
#ifdef YCJ
    (void)GPIO_Init(CH1_LED_PORT_R,  CH1_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH1_LED_PORT_G,  CH1_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH2_LED_PORT_R,  CH2_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH2_LED_PORT_G,  CH2_LED_PIN_G,  &stcLedGpioInit);
	(void)GPIO_Init(CH3_LED_PORT_R,  CH3_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH3_LED_PORT_G,  CH3_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH4_LED_PORT_R,  CH4_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH4_LED_PORT_G,  CH4_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH5_LED_PORT_R,  CH5_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH5_LED_PORT_G,  CH5_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH6_LED_PORT_R,  CH6_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH6_LED_PORT_G,  CH6_LED_PIN_G,  &stcLedGpioInit);
	(void)GPIO_Init(CH7_LED_PORT_R,  CH7_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH7_LED_PORT_G,  CH7_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH8_LED_PORT_R,  CH8_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH8_LED_PORT_G,  CH8_LED_PIN_G,  &stcLedGpioInit);
	#else
    (void)GPIO_Init(CH1_LED_PORT_R,  CH1_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH1_LED_PORT_G,  CH1_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH2_LED_PORT_R,  CH2_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH2_LED_PORT_G,  CH2_LED_PIN_G,  &stcLedGpioInit);
	(void)GPIO_Init(CH3_LED_PORT_R,  CH3_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH3_LED_PORT_G,  CH3_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH4_LED_PORT_R,  CH4_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH4_LED_PORT_G,  CH4_LED_PIN_G,  &stcLedGpioInit);
	#endif
}

/**********************************************************************************************************
**	函 数 名 : LedTest()
**	功能说明 : 用于测试
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void LedTest(void)
{
	bsp_LedToggle(RUN_LED_R_NO);
}

/**********************************************************************************************************
**	函 数 名 : bsp_LedOff()
**	功能说明 : 熄灭指定的LED指示灯
**	形    参 : _no : 指示灯序号(0~12)
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_LedOff(uint8_t _no)
{
	/* Led number有效性判断 */
	if((_no == LED_NOUSE_NUM) || (_no > LED_MAX_NUM))
	{
		return;
	}
	
	if(_no < GPIO_PIN_NOUSE)
	{
		GPIO_SetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]);  //设置灯板LED熄灭
	}
	else
	{
		GPIO_ResetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]);    //设置板载LED熄灭
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
	/* Led number有效性判断 */
	if((_no == LED_NOUSE_NUM) || (_no > LED_MAX_NUM))
	{
		return;
	}
	
	if(_no < GPIO_PIN_NOUSE)
	{
		GPIO_ResetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]);    //设置灯板LED点亮
	}
	else
	{
		GPIO_SetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]);  //设置板载LED点亮
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
	/* Led number有效性判断 */
	if((_no == LED_NOUSE_NUM) || (_no > LED_MAX_NUM))
	{
		return;
	}
	
	GPIO_TogglePins(LedGpioPortTab[_no], LedGpioPinTab[_no]);
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

