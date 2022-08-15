/**********************************************************************************************************
*
*	模块名称 : DACx0508C配置 
*	文件名称 : bsp_dac80508.h
*	版    本 : V1.0
*	说    明 : 头文件
*	作	 者：Robert.Li
**日	 期:  2021.7.26
**********************************************************************************************************/

#ifndef _BSP_DACx0508C_H_
#define _BSP_DACx0508C_H_

#ifdef __cplusplus
extern "C" {
#endif



/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "stm32f4xx_hal.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#define DACx0508C_DEBUG_ENABLE		1
#define DACx0508C_DEBUG_DISABLE		0
#define DACx0508C_PRINTF_CFG		DACx0508C_DEBUG_ENABLE		//bsp_lan文件串口打印配置   (Configurable)




/********************************************************************************************************	
HW : JN-MPU-C022F 　DACx0508C	 PIN 映射关系

	SPI4  PIN：
		PE11	 ------> DAC1_CS
		PE12	 ------> DAC1_SCK
		PE13	 ------> DAC1_MISO
		PE14	 ------> DAC1_MOSI


	SPI5  PIN：
		PF7 ------> DAC2_SCK
		PF8 ------> DAC2_MISO
		PF9 ------> DAC2_MOSI
		PF10 ------> DAC2_CS


	DACx0508C基本特性:
		1、供电2.7 - 5.5V;  【本例使用5V】
		4、外部参考电压2.5V，增益到5V

		对SPI的时钟速度要求: 高达50MHz， 速度很快,实际使用45MHz
		SCLK下降沿读取数据, 每次传送24bit数据， 高位先传

		
*********************************************************************************************************/

#define DAC1_SPI				SPI4					//DAC1使用的SPI  (Configurable)
#define DAC1_CS_ENABLE()		SPI4_CS_ENABLE()		//DAC1片选拉低   (Configurable)
#define DAC1_CS_DISABLE()		SPI4_CS_DISABLE()	//DAC1片选拉高   (Configurable)
#define DAC1_SPI_INIT()			bsp_InitSpi4()			//DAC1接口初始化 (Configurable)

#define DAC1_HANDLER								SPI4_Handler

#define DAC2_SPI				SPI5					//DAC2使用的SPI  (Configurable)
#define DAC2_CS_ENABLE()		SPI5_CS_ENABLE()		//DAC2片选拉低   (Configurable)
#define DAC2_CS_DISABLE()		SPI5_CS_DISABLE()	//DAC2片选拉高   (Configurable)
#define DAC2_SPI_INIT()			bsp_InitSpi5()			//DAC2接口初始化 (Configurable)

#define DAC2_HANDLER								SPI5_Handler

/* Error codes */
#define INVALID_VAL									-1				/* Invalid argument */
#define COMM_ERR									-2				/* Communication error on receive */
#define TIMEOUT										-3				/* A timeout has occured */



/* DAC80508 Register Map */
	
#define DACx0508C_CONFIG_MODE   ((3 << 16) | 0x100)
	
#define DACx0508C_GAIN_2X     ((4 << 16) | 0xFF)
	
	
	
	
#define DACx0508C_DEVICE_ID      0x01
#define DACx0508C_SYNC   	    0x02
#define DACx0508C_CONFIG  	    0x03
#define DACx0508C_GAIN	 	    0x04
#define DACx0508C_TRIGGER	    0x05
#define DACx0508C_BRDCAST_ADDRESS	    0x06
#define DACx0508C_STATUS	  		0x07
#define DACx0508C_DAC0	  		0x08
#define DACx0508C_DAC1	  		0x09
#define DACx0508C_DAC2	  		0x0A
#define DACx0508C_DAC3	  		0x0B
#define DACx0508C_DAC4	  		0x0C
#define DACx0508C_DAC5	  		0x0D
#define DACx0508C_DAC6	  		0x0E
#define DACx0508C_DAC7	  		0x0F
	
	
#define DACx0508C_READ	  		0x80


#define		dac_1 	0
#define		dac_2 	1




/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	

	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	

	extern void bsp_dac_spi_init(void);
	extern void bsp_dacx0508_init(void);
	
	extern void DAC80508_ReadDacData(uint8_t _ch, uint8_t * Value,uint8_t index);
	extern void DAC_SetLinearData(uint8_t _ch,uint16_t _Target_Iout, uint8_t index);
	extern void DAC_SetLinearData_2channl(uint8_t _ch1,uint8_t _ch2,uint16_t _Target_Iout1, uint16_t _Target_Iout2,uint8_t index);
	
	extern void DAC80508_Read_DAC_Value(uint8_t Addr,uint8_t * Value,uint8_t size,uint8_t index);
	extern void DAC80508_SetDacData(uint8_t _ch, uint16_t _dac, uint8_t index);
	extern void DAC80508_BRDCAST(uint16_t _dac, uint8_t index);


#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
