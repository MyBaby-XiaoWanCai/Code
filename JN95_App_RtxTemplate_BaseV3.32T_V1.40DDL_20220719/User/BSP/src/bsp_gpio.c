/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_gpio.c
** Author      : Huang Cheng
** Date        : 2021-11-04
** Description : bsp_gpio source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_gpio.h"
#include "bsp_dwt.h"
#include "global.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t W5500_IP_ADDRESS[4] = {192, 168, 0, 0};   //IP地址参数

/* Loop端口引脚查询表 */
LoopPortPin_TypeDef LoopPortPinTab[8] = 
{
	{GPIO_PORT_LOOP1, GPIO_PIN_LOOP1}, {GPIO_PORT_LOOP2, GPIO_PIN_LOOP2},
	{GPIO_PORT_LOOP3, GPIO_PIN_LOOP3}, {GPIO_PORT_LOOP4, GPIO_PIN_LOOP4},
	{GPIO_PORT_LOOP5, GPIO_PIN_LOOP5}, {GPIO_PORT_LOOP6, GPIO_PIN_LOOP6},
	{GPIO_PORT_LOOP7, GPIO_PIN_LOOP7}, {GPIO_PORT_LOOP8, GPIO_PIN_LOOP8},
};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : bsp_M_Loop_InitGPIO
**	功能说明 : Loop gpio初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_M_Loop_InitGPIO(void)
{
	stc_gpio_init_t stcLoopGpioInit;
	
	/* Set the JTDI to Normal GPIO for Loop2 */
	GPIO_SetDebugPort(GPIO_PIN_TDI, Disable);
	
	/* 设置Loop pin的属性:推挽输出、初始化状态为RESET等 */
	(void)GPIO_StructInit(&stcLoopGpioInit);
	stcLoopGpioInit.u16PinState = PIN_STATE_RESET;
	stcLoopGpioInit.u16PinDir = PIN_DIR_OUT;
	stcLoopGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcLoopGpioInit.u16PinOType = PIN_OTYPE_CMOS;
	
	/* 写入Loop pin的属性 */
    (void)GPIO_Init(GPIO_PORT_LOOP1, GPIO_PIN_LOOP1, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP2, GPIO_PIN_LOOP2, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP3, GPIO_PIN_LOOP3, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP4, GPIO_PIN_LOOP4, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP5, GPIO_PIN_LOOP5, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP6, GPIO_PIN_LOOP6, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP7, GPIO_PIN_LOOP7, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP8, GPIO_PIN_LOOP8, &stcLoopGpioInit);
}

/**********************************************************************************************************
**	函 数 名 : Enable_Mloop
**	功能说明 : Mloop使能函数
**	形    参 : _ch - 引脚编号
**	返 回 值 : 无
**********************************************************************************************************/
void Enable_Mloop(uint8_t _ch)
{
	if(_ch >= 8)
	{
		return;
	}
	
	GPIO_SetPins(LoopPortPinTab[_ch].GpioPort, LoopPortPinTab[_ch].GpioPin);
}

/**********************************************************************************************************
**	函 数 名 : Disable_Mloop
**	功能说明 : Mloop禁止函数
**	形    参 : _ch - 引脚编号
**	返 回 值 : 无
**********************************************************************************************************/
void Disable_Mloop(uint8_t _ch)
{
	if(_ch >= 8)
	{
		return;
	}
	
	GPIO_ResetPins(LoopPortPinTab[_ch].GpioPort, LoopPortPinTab[_ch].GpioPin);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_FR_ADDR_InitGPIO
*	功能说明: 配置拨码开关设置的GPIO,  该函数被 bsp_Init() 调用。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_FR_ADDR_InitGPIO(void)
{
	stc_gpio_init_t stcFrAddrGpioInit;
	
	/* 设置FR_ADDR pin的属性:下拉输入 */
	(void)GPIO_StructInit(&stcFrAddrGpioInit);
	stcFrAddrGpioInit.u16PinDir = PIN_DIR_IN;
	stcFrAddrGpioInit.u16PullUp = PIN_PU_OFF;

	/* 写入FR_ADDR pin的属性 */
	(void)GPIO_Init(GPIO_PORT_FR_ADDR1, GPIO_PIN_FR_ADDR1, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR2, GPIO_PIN_FR_ADDR2, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR3, GPIO_PIN_FR_ADDR3, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR4, GPIO_PIN_FR_ADDR4, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR5, GPIO_PIN_FR_ADDR5, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR6, GPIO_PIN_FR_ADDR6, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR7, GPIO_PIN_FR_ADDR7, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR8, GPIO_PIN_FR_ADDR8, &stcFrAddrGpioInit);
	
}

/*
*********************************************************************************************************
*	函 数 名: bsp_HWDG_InitGPIO
*	功能说明: 硬件看门狗IO初始化。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_HWDG_InitGPIO(void)
{
    stc_gpio_init_t stcHwdgGpioInit;
	
	/* 设置Hwdg pin的属性:推挽输出、初始化状态为RESET等 */
	(void)GPIO_StructInit(&stcHwdgGpioInit);
	stcHwdgGpioInit.u16PinState = PIN_STATE_RESET;
	stcHwdgGpioInit.u16PinDir = PIN_DIR_OUT;
	stcHwdgGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcHwdgGpioInit.u16PinOType = PIN_OTYPE_CMOS;
	
	/* 写入Hwdg pin的属性 */
    (void)GPIO_Init(GPIO_PORT_HWDG, GPIO_PIN_HWDG, &stcHwdgGpioInit);
}

/*
*********************************************************************************************************
*	函 数 名: Feed_WDG
*	功能说明: 喂狗函数（硬件看门狗&独立看门狗）
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Feed_WDG(void)
{
	#if 1
//    HAL_IWDG_Refresh(&hiwdg);    //喂狗（独立看门狗）
	SWDT_Feed();
	
	//喂狗（硬件看门狗）
    if(feed_hardware_watchdog_enable)
    {
        GPIO_TogglePins(GPIO_PORT_HWDG, GPIO_PIN_HWDG); 
    }
	#endif
}

/**********************************************************************************************************
*	函 数 名: IP_Scanning
*	功能说明: 读取系统的IP设定
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void IP_Scanning(void)
{
	uint8_t i = 0;
	uint8_t AddrTmp = 0;
	
	/* GPIO PORT查询表 */
	uint8_t GpioPortAddr[8] = 
	{
		GPIO_PORT_FR_ADDR1, GPIO_PORT_FR_ADDR2, GPIO_PORT_FR_ADDR3, GPIO_PORT_FR_ADDR4,
		GPIO_PORT_FR_ADDR5, GPIO_PORT_FR_ADDR6, GPIO_PORT_FR_ADDR7, GPIO_PORT_FR_ADDR8
	};
	
	/* GPIO PIN查询表 */
	uint16_t GpioPinAddr[8] = 
	{
		GPIO_PIN_FR_ADDR1, GPIO_PIN_FR_ADDR2, GPIO_PIN_FR_ADDR3, GPIO_PIN_FR_ADDR4,
		GPIO_PIN_FR_ADDR5, GPIO_PIN_FR_ADDR6, GPIO_PIN_FR_ADDR7, GPIO_PIN_FR_ADDR8
	};
	
	/* 读取拨码开关1-3状态 */
	for(i = 0; i < IP_ADDR3_END; i++)
	{
		if(0x00 == GPIO_ReadInputPins(GpioPortAddr[i], GpioPinAddr[i]))
		{
			bsp_DelayMS(10);
			if(0x00 == GPIO_ReadInputPins(GpioPortAddr[i], GpioPinAddr[i]))
			{
				AddrTmp += 0x01 << (IP_ADDR3_END-i-1);
			}
		}
	}
	W5500_IP_ADDRESS[2] = AddrTmp + 1;
	AddrTmp = 0x00;
	
	/* 读取拨码开关4-8状态 */
	for(i = IP_ADDR3_END; i < IP_ADDR4_END; i++)
	{
		if(0x00 == GPIO_ReadInputPins(GpioPortAddr[i], GpioPinAddr[i]))
		{
			bsp_DelayMS(10);
			if(0x00 == GPIO_ReadInputPins(GpioPortAddr[i], GpioPinAddr[i]))
			{
				AddrTmp += 0x01 << (IP_ADDR4_END-i-1);
			}
		}
	}
	W5500_IP_ADDRESS[3] = AddrTmp + 11;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
