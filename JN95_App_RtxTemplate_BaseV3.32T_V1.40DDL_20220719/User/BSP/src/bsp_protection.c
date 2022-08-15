/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_protection.c
** Author      : Huang Cheng
** Date        : 2021-11-08
** Description : bsp_protection c source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_protection.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
/* 硬件保护端口引脚查询表 */
PgPortPin_TypeDef PgPortPinTab[PG_MaxNum] = 
{
	{PG1_GPIO_Port, PG1_Pin},
	{PG2_GPIO_Port, PG2_Pin},
	{PG3_GPIO_Port, PG3_Pin},
	{PG4_GPIO_Port, PG4_Pin},
	{PG5_GPIO_Port, PG5_Pin},
	{PG6_GPIO_Port, PG6_Pin},
	{PG7_GPIO_Port, PG7_Pin},
	{PG8_GPIO_Port, PG8_Pin},
};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : bsp_HW_Fault_InitGPIO
**	功能说明 : 配置HW Fault Singal输入的GPIO
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_HW_Fault_InitGPIO(void)
{
	stc_gpio_init_t stcHwFaultGpioInit;
	
	/* 设置HW_Fault pin的属性:下拉输入 */
	(void)GPIO_StructInit(&stcHwFaultGpioInit);
	stcHwFaultGpioInit.u16PinDir = PIN_DIR_IN;
	stcHwFaultGpioInit.u16PullUp = PIN_PU_OFF;

	/* 写入HW_Fault pin的属性 */
	(void)GPIO_Init(PG1_GPIO_Port, PG1_Pin, &stcHwFaultGpioInit);
	(void)GPIO_Init(PG2_GPIO_Port, PG2_Pin, &stcHwFaultGpioInit);
	(void)GPIO_Init(PG3_GPIO_Port, PG3_Pin, &stcHwFaultGpioInit);
	(void)GPIO_Init(PG4_GPIO_Port, PG4_Pin, &stcHwFaultGpioInit);
	(void)GPIO_Init(PG5_GPIO_Port, PG5_Pin, &stcHwFaultGpioInit);
	(void)GPIO_Init(PG6_GPIO_Port, PG6_Pin, &stcHwFaultGpioInit);
	(void)GPIO_Init(PG7_GPIO_Port, PG7_Pin, &stcHwFaultGpioInit);
	(void)GPIO_Init(PG8_GPIO_Port, PG8_Pin, &stcHwFaultGpioInit);
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
