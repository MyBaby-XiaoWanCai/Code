/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Bsp_SGM58601.c
** Author      : Cai Heng
** Date        : 2022-07-11
** Description : SGM58601驱动模块
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "Bsp_SGM58601.h"
#include "Bsp_NCA9555.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint32_t ADC1_DATA[8] = {0};
uint32_t ADC2_DATA[8] = {0};
uint32_t ADC3_DATA[8] = {0};

uint64_t ADC1_DATA_SUM[8] = {0};
uint64_t ADC2_DATA_SUM[8] = {0};
uint64_t ADC3_DATA_SUM[8] = {0};

uint32_t ADC1_DATA_AVG[8] = {0};
uint32_t ADC2_DATA_AVG[8] = {0};
uint32_t ADC3_DATA_AVG[8] = {0};

uint16_t ADC1Count;  //取平均数计数
uint16_t ADC2Count;
uint16_t ADC3Count;

uint8_t CurrOk[2];

uint8_t DRDY_NUM[3] = {0};
uint8_t CONV_CNT[3] = {0};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void ADC_WRITE_TO_ONE_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr, uint8_t reg_data);
uint8_t ADC_READ_FROM_ONE_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr);
void ADC_COMMAND_SET(SPI_HandleTypeDef* spi_handler, uint8_t command);
void ADC_SET_CHANNEL(SPI_HandleTypeDef* spi_handler, uint8_t AIN_P, uint8_t AIN_N);
void ADC_SET_PGA(SPI_HandleTypeDef* spi_handler, uint8_t PGA);
void ADC_SET_DATARATE(SPI_HandleTypeDef* spi_handler, uint8_t DATARATE);
void ADC_SET_SDCS(SPI_HandleTypeDef* spi_handler, uint8_t SDCS_UA);
void ADC_SET_CLKOUT(SPI_HandleTypeDef* spi_handler, uint8_t CLKOUT_F);
void ADC_EN_INPUTBUFFER(SPI_HandleTypeDef* spi_handler);
void ADC_DISABLE_INPUTBUFFER(SPI_HandleTypeDef* spi_handler);
void ADC_EN_AUTOCALIBRATION(SPI_HandleTypeDef* spi_handler);
void ADC_DISABLE_AUTOCALIBRATION(SPI_HandleTypeDef* spi_handler);
void ADC_MSB_FIRST(SPI_HandleTypeDef* spi_handler);
void ADC_LSB_FIRST(SPI_HandleTypeDef* spi_handler);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : 
**	功能说明 : 
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
void SGM58601_CS_0(SPI_HandleTypeDef* spi_handler)
{
	 if(spi_handler == &hspi4)
	 {
		 HAL_GPIO_WritePin(SPI4_NSS_ADC1_GPIO_Port, SPI4_NSS_ADC1_Pin, GPIO_PIN_RESET);
	 }
	 else if(spi_handler == &hspi2)
	 {
		 HAL_GPIO_WritePin(SPI2_NSS_ADC2_GPIO_Port, SPI2_NSS_ADC2_Pin, GPIO_PIN_RESET);
	 }
	 else if(spi_handler == &hspi1)
	 {
		 HAL_GPIO_WritePin(SPI1_NSS_ADC3_GPIO_Port, SPI1_NSS_ADC3_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 ;
	 }
	/* CS LOW (modify) */
}

/**********************************************************************************************************
**	函 数 名 : 
**	功能说明 : 
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
void SGM58601_CS_1(SPI_HandleTypeDef* spi_handler)
{
	 if(spi_handler == &hspi4)
	 {
		 HAL_GPIO_WritePin(SPI4_NSS_ADC1_GPIO_Port, SPI4_NSS_ADC1_Pin, GPIO_PIN_SET);
	 }
	 else if(spi_handler == &hspi2)
	 {
		 HAL_GPIO_WritePin(SPI2_NSS_ADC2_GPIO_Port, SPI2_NSS_ADC2_Pin, GPIO_PIN_SET);
	 }
	 else if(spi_handler == &hspi1)
	 {
		 HAL_GPIO_WritePin(SPI1_NSS_ADC3_GPIO_Port, SPI1_NSS_ADC3_Pin, GPIO_PIN_SET);
	 }
	 else
	 {
		 ;
	 }
	/* CS HIGH (modify) */
}

/**********************************************************************************************************
**	函 数 名 : 
**	功能说明 : 
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
void SGM58601_SPI_INIT()
{
	/* SPI INIT (modify) */
}

/**********************************************************************************************************
**	函 数 名 : SGM58601_REG_INIT(SPI_HandleTypeDef* spi_handler)
**	功能说明 : SGM58601寄存器初始化
**	形    参 : spi_handler: 指向SPI模块寄存器指针
**	返 回 值 : 无
**********************************************************************************************************/
void SGM58601_REG_INIT(SPI_HandleTypeDef* spi_handler)
{
	uint8_t REG_STATUS_DAT = 0;
	uint8_t REG_MUX_DAT = 0;
	uint8_t REG_ADCON_DAT = 0;
	uint8_t REG_DRATE_DAT = 0;
	uint8_t REG_IO_DAT = 0;
	uint8_t txData = 0;
	uint8_t rxData = 0;
	
	REG_STATUS_INIT_STRUCT	REG_STATUS_STR;
	REG_MUX_INIT_STRUCT		REG_MUX_STR;
	REG_ADCON_INIT_STRUCT	REG_ADCON_STR;
	REG_DRATE_INIT_STRUCT	REG_DRATE_STR;
	REG_IO_INIT_STRUCT		REG_IO_STR;

	/* SGM58601 Register Config */
	// STATUS: Status Register
	REG_STATUS_STR.ORDER = MSB_FIRST;			// 数据输出MSB先出
	REG_STATUS_STR.ACAL  = AUTO_CALI_DISABLE;	// 自动校准关闭
	REG_STATUS_STR.BUFEN = BUFFER_DISABLE;		// 模拟输入buffer关闭
	// MUX: Input Multiplexer Control Register
	REG_MUX_STR.PSEL     = AIN0;				// 输入正极通道选中
	REG_MUX_STR.NSEL     = AINCOM;				// 输入负极通道选中
	// ADCON: A/D Control Register
	REG_ADCON_STR.CLK    = CLKOUT_OFF;			//时钟输出关闭
	REG_ADCON_STR.SDCS   = SDCS_OFF;			//电流源探测关闭
	REG_ADCON_STR.PGA    = PGA_1;				//可编程增益放大器选中模式1
	// DRATE: A/D Data Rate
	REG_DRATE_STR.DR     = DR_7500SPS;			//数据输出速率60KSPS
	// IO: GPIO Control Register
	REG_IO_STR.DIR3      = Direction_Input;		//GPIO输入
	REG_IO_STR.DIR2      = Direction_Input;		//GPIO输入
	REG_IO_STR.DIR1      = Direction_Input;		//GPIO输入
	REG_IO_STR.DIR0      = Direction_Input;		//GPIO输入
	REG_IO_STR.DIO3      = 0;					//做输出时，IO电平
	REG_IO_STR.DIO2      = 1;					//做输出时，IO电平
	REG_IO_STR.DIO1      = 0;					//做输出时，IO电平
	REG_IO_STR.DIO0      = 1;					//做输出时，IO电平
	
	REG_STATUS_DAT = REG_STATUS_STR.ORDER | REG_STATUS_STR.ACAL | REG_STATUS_STR.BUFEN;
	REG_MUX_DAT    = (REG_MUX_STR.PSEL << 4) | REG_MUX_STR.NSEL;
	REG_ADCON_DAT  = REG_ADCON_STR.CLK | REG_ADCON_STR.SDCS | REG_ADCON_STR.PGA;
	REG_DRATE_DAT  = REG_DRATE_STR.DR;
	REG_IO_DAT     = (REG_IO_STR.DIR3 << 7) | (REG_IO_STR.DIR2 << 6) | (REG_IO_STR.DIR1 << 5) | (REG_IO_STR.DIR0 << 4) | \
					 (REG_IO_STR.DIO3 << 3) | (REG_IO_STR.DIO2 << 2) | (REG_IO_STR.DIO1 << 1) | (REG_IO_STR.DIO0 << 0);
	
	/* SGM58601 Register Update */
	txData = 0x50 | STATUS_REG_ADDR;
	HAL_SPI_TransmitReceive(spi_handler, &txData, &rxData, 1, 0xff);
	
	txData = 0x04;
	HAL_SPI_TransmitReceive(spi_handler, &txData, &rxData, 1, 0xff);
	
	txData = REG_STATUS_DAT;
	HAL_SPI_TransmitReceive(spi_handler, &txData, &rxData, 1, 0xff);
	
	txData = REG_MUX_DAT;
	HAL_SPI_TransmitReceive(spi_handler, &txData, &rxData, 1, 0xff);
	
	txData = REG_ADCON_DAT;
	HAL_SPI_TransmitReceive(spi_handler, &txData, &rxData, 1, 0xff);	
	
	txData = REG_DRATE_DAT;
	HAL_SPI_TransmitReceive(spi_handler, &txData, &rxData, 1, 0xff);	
	
	txData = REG_IO_DAT;
	HAL_SPI_TransmitReceive(spi_handler, &txData, &rxData, 1, 0xff);		
}

/**********************************************************************************************************
**	函 数 名 : 
**	功能说明 : 
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
void SGM58601_EXTI_INIT()
{
	/* EXTI INIT (modify) */
}

/**********************************************************************************************************
**	函 数 名 : SGM58601_INIT(SPI_HandleTypeDef* spi_handler)
**	功能说明 : SGM58601初始化
**	形    参 : spi_handler: 指向SPI模块寄存器指针
**	返 回 值 : 无
**********************************************************************************************************/
void SGM58601_INIT(SPI_HandleTypeDef* spi_handler)
{
//	SGM58601_SPI_INIT();
//	SGM58601_EXTI_INIT();
	SGM58601_CS_0(spi_handler);
	SGM58601_REG_INIT(spi_handler);
	SGM58601_CS_1(spi_handler);
	bsp_DelayMS(10);
//	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**********************************************************************************************************
**	函 数 名 : SGM58601_SWITCH_CH(SPI_HandleTypeDef* spi_handler, uint8_t CH)
**	功能说明 : SGM58601通道采样切换
**	形    参 : spi_handler: 指向SPI模块寄存器指针
**             CH: 通道编号
**	返 回 值 : 
**********************************************************************************************************/
void SGM58601_SWITCH_CH(SPI_HandleTypeDef* spi_handler, uint8_t CH)
{
	SGM58601_CS_0(spi_handler);
	ADC_SET_CHANNEL(spi_handler, CH, AINCOM);
	SGM58601_CS_1(spi_handler);
}

/**********************************************************************************************************
**	函 数 名 : SGM58601_READ_DATA(SPI_HandleTypeDef* spi_handler)
**	功能说明 : SGM58601读取数据
**	形    参 : spi_handler: 指向SPI模块寄存器指针
**	返 回 值 : ret: 低24位有效
**********************************************************************************************************/
uint32_t SGM58601_READ_DATA(SPI_HandleTypeDef* spi_handler)
{
	uint32_t ret = 0;
	uint8_t txData = 0;
	uint8_t rxData[3] = {0};
	
	SGM58601_CS_0(spi_handler);
	ADC_COMMAND_SET(spi_handler, RDATA_COMMAND);
	HAL_SPI_TransmitReceive(spi_handler, &txData, &rxData[0], 1, 0xff);
	HAL_SPI_TransmitReceive(spi_handler, &txData, &rxData[1], 1, 0xff);
	HAL_SPI_TransmitReceive(spi_handler, &txData, &rxData[2], 1, 0xff);
	SGM58601_CS_1(spi_handler);
	ret = (rxData[0] << 16) + (rxData[1] << 8) + rxData[0];
	
	return ret;
}

/**********************************************************************************************************
**	函 数 名 : SGM58601_READ_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr)
**	功能说明 : SGM58601读寄存器
**	形    参 : spi_handler: 指向SPI模块寄存器指针
**             reg_addr: 寄存器地址
**	返 回 值 : 
**********************************************************************************************************/
uint8_t SGM58601_READ_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr)
{
	uint8_t ret = 0;
	
	SGM58601_CS_0(spi_handler);
	ret = ADC_READ_FROM_ONE_REG(spi_handler, reg_addr);
	SGM58601_CS_1(spi_handler);
	
	return ret;
}

/**********************************************************************************************************
**	函 数 名 : SGM58601_READ_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr)
**	功能说明 : SGM58601读寄存器
**	形    参 : spi_handler: 指向SPI模块寄存器指针
**             reg_addr: 寄存器地址
**	返 回 值 : 
**********************************************************************************************************/
void SGM58601_WRITE_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr, uint8_t reg_data)
{
	SGM58601_CS_0(spi_handler);
	ADC_WRITE_TO_ONE_REG(spi_handler, reg_addr, reg_data);
	SGM58601_CS_1(spi_handler);
}

/**********************************************************************************************************
**	函 数 名 : HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
**	功能说明 : 数据读取函数
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint32_t *adc_dat_ptr;
	uint8_t *drdy_num_ptr;
	uint8_t *conv_cnt_ptr;
	uint8_t i = 0;
	SPI_HandleTypeDef *spi_ptr;
	
	uint8_t temp = 0;
	if(GPIO_Pin == ADC1_BUSY_Pin)
	{	
		spi_ptr = &hspi4;
		adc_dat_ptr = &ADC1_DATA[0];
		drdy_num_ptr = &DRDY_NUM[0];
		conv_cnt_ptr = &CONV_CNT[0];
		temp = 1;
		
		for(i = 0; i < 8; i++)
		{
			if(ADC1_DATA[i] > 0x7FFFFF)
			{
				ADC1_DATA[i] = 0;
			}
			ADC1_DATA_SUM[i] = ADC1_DATA_SUM[i] + ADC1_DATA[i];
		}
		ADC1Count++;
		if(ADC1Count == FILTERSUM)
		{
			for(i = 0; i < 8; i++)
			{
				ADC1_DATA_AVG[i] = ADC1_DATA_SUM[i] / FILTERSUM;
				ADC1_DATA_SUM[i] = 0;
			}
			ADC1Count = 0;
		}
	}
	else if(GPIO_Pin == ADC2_BUSY_Pin)
	{
		spi_ptr = &hspi2;
		adc_dat_ptr = &ADC2_DATA[0];
		drdy_num_ptr = &DRDY_NUM[1];
		conv_cnt_ptr = &CONV_CNT[1];
		temp = 1;	
		
		for(i = 0; i < 8; i++)
		{
			if(ADC2_DATA[i] > 0x7FFFFF)
			{
				ADC2_DATA[i] = 0;
			}
			ADC2_DATA_SUM[i] = ADC2_DATA_SUM[i] + ADC2_DATA[i];
		}
		ADC2Count++;
		if(ADC2Count == FILTERSUM)
		{
			for(i = 0; i < 8; i++)
			{
				ADC2_DATA_AVG[i] = ADC2_DATA_SUM[i] / FILTERSUM;
				ADC2_DATA_SUM[i] = 0;
			}
			ADC2Count = 0;
		}		
	}
	else if(GPIO_Pin == ADC3_BUSY_Pin)
	{
		spi_ptr = &hspi1;
		adc_dat_ptr = &ADC3_DATA[0];
		drdy_num_ptr = &DRDY_NUM[2];
		conv_cnt_ptr = &CONV_CNT[2];
		temp = 1;
		
		for(i = 0; i < 8; i++)
		{
			if(ADC3_DATA[i] > 0x7FFFFF)
			{
				ADC3_DATA[i] = 0;
			}
			ADC3_DATA_SUM[i] = ADC3_DATA_SUM[i] + ADC3_DATA[i];
		}
		ADC3Count++;
		if(ADC3Count == FILTERSUM)
		{
			for(i = 0; i < 8; i++)
			{
				ADC3_DATA_AVG[i] = ADC3_DATA_SUM[i] / FILTERSUM;
				ADC3_DATA_SUM[i] = 0;
			}
			ADC3Count = 0;
		}	
	}
	else if(GPIO_Pin == INT_current_Pin)
	{
		Pca9555_RegRead(I2C_NCA9555, 0x40, &CurrOk);
	}
	if(temp == 1)
	{
		NVIC_DisableIRQ(EXTI15_10_IRQn);
		
		switch(*conv_cnt_ptr)
		{
			case 0:
			case 1:
			case 4:
			{
				*conv_cnt_ptr = *conv_cnt_ptr + 1;
			}
			break;
			
			case 2:
			{
				*(adc_dat_ptr + *drdy_num_ptr) = SGM58601_READ_DATA(spi_ptr);
				
				if(*drdy_num_ptr < 7)
				{
					*drdy_num_ptr = *drdy_num_ptr + 1;
				}
				else
				{
					*drdy_num_ptr = 0;
				}
				
				*conv_cnt_ptr = *conv_cnt_ptr + 1;
			}
			break;
			
			case 3:
			{
				SGM58601_SWITCH_CH(spi_ptr, *drdy_num_ptr);
				
				*conv_cnt_ptr = *conv_cnt_ptr + 1;
			}
			break;
			
			default:
			{
				*conv_cnt_ptr = 0;
			}
			break;
		}
		
		NVIC_EnableIRQ(EXTI15_10_IRQn);	
	}
}



/**********************************************************************************************************
**	函 数 名 : ADC_WRITE_TO_ONE_REG()
**	功能说明 : SGM58601 SPI写一个寄存器
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_WRITE_TO_ONE_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr, uint8_t reg_data)
{
	uint8_t txData[3] = {0};
	uint8_t rxData = 0;
	uint8_t wreg = 0x50;

	wreg = wreg | reg_addr;	// 合成写地址
	txData[0] = wreg;		// 写地址
	txData[1] = 0;			// 写字节数
	txData[2] = reg_data;	// 写数据
	HAL_SPI_TransmitReceive(spi_handler, &txData[0], &rxData, 1, 0xff);
	HAL_SPI_TransmitReceive(spi_handler, &txData[1], &rxData, 1, 0xff);
	HAL_SPI_TransmitReceive(spi_handler, &txData[2], &rxData, 1, 0xff);
}

/**********************************************************************************************************
**	函 数 名 : ADC_READ_FROM_ONE_REG()
**	功能说明 : SGM58601 SPI读一个寄存器
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
uint8_t ADC_READ_FROM_ONE_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr)
{
	uint8_t txData[3] = {0};
	uint8_t rxData = 0;
	uint8_t rreg = 0x10;

	rreg = rreg | reg_addr;	// 合成读地址
	txData[0] = rreg;		// 读地址
	txData[1] = 0;			// 读字节数
	HAL_SPI_TransmitReceive(spi_handler, &txData[0], &rxData, 1, 0xff);
	HAL_SPI_TransmitReceive(spi_handler, &txData[1], &rxData, 1, 0xff);
	bsp_DelayUS(10);
	HAL_SPI_TransmitReceive(spi_handler, &txData[2], &rxData, 1, 0xff);

	return rxData;
}

/**********************************************************************************************************
**	函 数 名 : ADC_COMMAND_SET
**	功能说明 : SGM58601指令设置
**	形    参 : command - WAKEUP_COMMAND   : Completes SYNC and Exits Standby Mode
**						 RDATA_COMMAND    : Read Data
**						 RDATAC_COMMAND   : Read Data Continuously
**						 SDATAC_COMMAND   : Stop Read Data Continuously
**						 SELFCAL_COMMAND  : Offset and Gain Self-Calibration
**						 SELFOCAL_COMMAND : Offset Self-Calibration
**						 SELFGCAL_COMMAND : Gain Self-Calibration
**						 SYSOCAL_COMMAND  : System Offset Calibration
**						 SYSGCAL_COMMAND  : System Gain Calibration
**						 SYNC_COMMAND     : Synchronize the A/D Conversion
**						 STANDBY_COMMAND  : Begin Standby Mode
**						 RESET_COMMAND    : Reset to Power-Up Values
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_COMMAND_SET(SPI_HandleTypeDef* spi_handler, uint8_t command)
{
	uint8_t txDate = command;
	uint8_t rxDate = 0;
	HAL_SPI_TransmitReceive(spi_handler, &txDate, &rxDate, 1, 0xff);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SET_CHANNEL()
**	功能说明 : SGM58601 选择输入通道
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SET_CHANNEL(SPI_HandleTypeDef* spi_handler, uint8_t AIN_P, uint8_t AIN_N)
{
	uint8_t channel = (AIN_P << 4) + AIN_N;

	ADC_WRITE_TO_ONE_REG(spi_handler, MUX_REG_ADDR, channel);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SET_PGA()
**	功能说明 : SGM58601 设置PGA
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SET_PGA(SPI_HandleTypeDef* spi_handler, uint8_t PGA)
{
	uint8_t temp = 0;

	temp = ADC_READ_FROM_ONE_REG(spi_handler, ADCON_REG_ADDR);
	temp = temp & 0xf8;
	temp = temp | PGA;
	ADC_WRITE_TO_ONE_REG(spi_handler, ADCON_REG_ADDR, temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SET_DATARATE()
**	功能说明 : SGM58601 设置数据输出速率
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SET_DATARATE(SPI_HandleTypeDef* spi_handler, uint8_t DATARATE)
{
	ADC_WRITE_TO_ONE_REG(spi_handler, DRATE_REG_ADDR, DATARATE);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SET_SDCS()
**	功能说明 : SGM58601 设置传感器开短路检测
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SET_SDCS(SPI_HandleTypeDef* spi_handler, uint8_t SDCS_UA)
{
	uint8_t temp = 0;

	temp = ADC_READ_FROM_ONE_REG(spi_handler, ADCON_REG_ADDR);
	temp = temp & 0xe7;
	temp = temp | SDCS_UA;
	ADC_WRITE_TO_ONE_REG(spi_handler, ADCON_REG_ADDR, temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SET_CLKOUT()
**	功能说明 : SGM58601 设置CLK频率
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SET_CLKOUT(SPI_HandleTypeDef* spi_handler, uint8_t CLKOUT_F)
{
	uint8_t temp = 0;

	temp = ADC_READ_FROM_ONE_REG(spi_handler, ADCON_REG_ADDR);
	temp = temp & 0x9f;
	temp = temp | CLKOUT_F;
	ADC_WRITE_TO_ONE_REG(spi_handler, ADCON_REG_ADDR, temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_EN_INPUTBUFFER()
**	功能说明 : SGM58601 使能输入缓存
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_EN_INPUTBUFFER(SPI_HandleTypeDef* spi_handler)
{
	uint8_t temp = 0;

	temp = ADC_READ_FROM_ONE_REG(spi_handler, STATUS_REG_ADDR);
	temp = temp | 0x02;
	ADC_WRITE_TO_ONE_REG(spi_handler, STATUS_REG_ADDR, temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_DISABLE_INPUTBUFFER()
**	功能说明 : SGM58601 失能输入缓存
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_DISABLE_INPUTBUFFER(SPI_HandleTypeDef* spi_handler)
{
	uint8_t temp = 0;
	
	temp = ADC_READ_FROM_ONE_REG(spi_handler, STATUS_REG_ADDR);
	temp = temp & 0xfd;
	ADC_WRITE_TO_ONE_REG(spi_handler, STATUS_REG_ADDR, temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_EN_AUTOCALIBRATION()
**	功能说明 : SGM58601 使能自动校准
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_EN_AUTOCALIBRATION(SPI_HandleTypeDef* spi_handler)
{
	uint8_t temp = 0;
	
	temp = ADC_READ_FROM_ONE_REG(spi_handler, STATUS_REG_ADDR);
	temp = temp | 0x04;
	ADC_WRITE_TO_ONE_REG(spi_handler, STATUS_REG_ADDR, temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_DISABLE_AUTOCALIBRATION()
**	功能说明 : SGM58601 失能自动校准
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_DISABLE_AUTOCALIBRATION(SPI_HandleTypeDef* spi_handler)
{
	uint8_t temp = 0;
	
	temp = ADC_READ_FROM_ONE_REG(spi_handler, STATUS_REG_ADDR);
	temp = temp & 0xfb;
	ADC_WRITE_TO_ONE_REG(spi_handler, STATUS_REG_ADDR, temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_MSB_FIRST()
**	功能说明 : SGM58601  设置数据输出顺序MSB_FIRST
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_MSB_FIRST(SPI_HandleTypeDef* spi_handler)
{
	uint8_t temp = 0;
	temp = ADC_READ_FROM_ONE_REG(spi_handler, STATUS_REG_ADDR);
	temp = temp & 0xf7;
	ADC_WRITE_TO_ONE_REG(spi_handler, STATUS_REG_ADDR, temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_LSB_FIRST()
**	功能说明 : SGM58601  设置数据输出顺序LSB_FIRST
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_LSB_FIRST(SPI_HandleTypeDef* spi_handler)
{
	uint8_t temp = 0;
	temp = ADC_READ_FROM_ONE_REG(spi_handler, STATUS_REG_ADDR);
	temp = temp | 0x08;
	ADC_WRITE_TO_ONE_REG(spi_handler, STATUS_REG_ADDR, temp);
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
