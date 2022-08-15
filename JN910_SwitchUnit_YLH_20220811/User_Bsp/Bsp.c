#include "Bsp.h"

/**********************************************************************************************************
**	函 数 名 : BspInit()
**	功能说明 : 板级初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void BspInit(void)
{
	uint16_t output = 0x0000;
	uint16_t input = 0xFFFF;

	bsp_InitDWT();	//DWT延时初始化
	
	ReadAddr();  //读取设备地址
	
	bsp_SetTIMforInt(TIM5,1000,1,0);	/*初始化运行时硬件定时器TIM5*/
	bsp_SetTIMforInt(TIM4,125,1,1);    /* 保护函数执行 */   
	
	SGM58601_INIT(&hspi4);  //ADC1初始化
    SGM58601_INIT(&hspi2);  //ADC2初始化
    SGM58601_INIT(&hspi1);  //ADC3初始化
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);  //开启nDRDY中断准备接收数据
	
	PCA9555_IOConfigReg(I2C_NCA9555, 0x40, &input);  //初始化IO为输入
	PCA9555_IOConfigReg(I2C_NCA9555, 0x42, &input);
	PCA9555_IOConfigReg(I2C_NCA9555, 0x44, &input);

	PCA9555_IOConfigReg(I2C_LED, 0x40, &input); 
	PCA9555_IOConfigReg(I2C_LED, 0x42, &output);       
	
	HAL_GPIO_WritePin(SNOE1_GPIO_Port, SNOE1_Pin, GPIO_PIN_RESET);	//使能SN74
	HAL_GPIO_WritePin(SNOE2_GPIO_Port, SNOE2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SNOE3_GPIO_Port, SNOE3_Pin, GPIO_PIN_RESET);
}