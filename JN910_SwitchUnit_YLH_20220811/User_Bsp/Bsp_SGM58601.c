/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Bsp_SGM58601.c
** Author      : Cai Heng
** Date        : 2022-07-11
** Description : SGM58601����ģ��
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Bsp_SGM58601.h"
#include "Bsp_NCA9555.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
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

uint16_t ADC1Count;  //ȡƽ��������
uint16_t ADC2Count;
uint16_t ADC3Count;

uint8_t CurrOk[2];

uint8_t DRDY_NUM[3] = {0};
uint8_t CONV_CNT[3] = {0};

/**********************************************************************************************************
**                                           ����������
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
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : 
**	����˵�� : 
**	��    �� : 
**	�� �� ֵ : 
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
**	�� �� �� : 
**	����˵�� : 
**	��    �� : 
**	�� �� ֵ : 
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
**	�� �� �� : 
**	����˵�� : 
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
void SGM58601_SPI_INIT()
{
	/* SPI INIT (modify) */
}

/**********************************************************************************************************
**	�� �� �� : SGM58601_REG_INIT(SPI_HandleTypeDef* spi_handler)
**	����˵�� : SGM58601�Ĵ�����ʼ��
**	��    �� : spi_handler: ָ��SPIģ��Ĵ���ָ��
**	�� �� ֵ : ��
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
	REG_STATUS_STR.ORDER = MSB_FIRST;			// �������MSB�ȳ�
	REG_STATUS_STR.ACAL  = AUTO_CALI_DISABLE;	// �Զ�У׼�ر�
	REG_STATUS_STR.BUFEN = BUFFER_DISABLE;		// ģ������buffer�ر�
	// MUX: Input Multiplexer Control Register
	REG_MUX_STR.PSEL     = AIN0;				// ��������ͨ��ѡ��
	REG_MUX_STR.NSEL     = AINCOM;				// ���븺��ͨ��ѡ��
	// ADCON: A/D Control Register
	REG_ADCON_STR.CLK    = CLKOUT_OFF;			//ʱ������ر�
	REG_ADCON_STR.SDCS   = SDCS_OFF;			//����Դ̽��ر�
	REG_ADCON_STR.PGA    = PGA_1;				//�ɱ������Ŵ���ѡ��ģʽ1
	// DRATE: A/D Data Rate
	REG_DRATE_STR.DR     = DR_7500SPS;			//�����������60KSPS
	// IO: GPIO Control Register
	REG_IO_STR.DIR3      = Direction_Input;		//GPIO����
	REG_IO_STR.DIR2      = Direction_Input;		//GPIO����
	REG_IO_STR.DIR1      = Direction_Input;		//GPIO����
	REG_IO_STR.DIR0      = Direction_Input;		//GPIO����
	REG_IO_STR.DIO3      = 0;					//�����ʱ��IO��ƽ
	REG_IO_STR.DIO2      = 1;					//�����ʱ��IO��ƽ
	REG_IO_STR.DIO1      = 0;					//�����ʱ��IO��ƽ
	REG_IO_STR.DIO0      = 1;					//�����ʱ��IO��ƽ
	
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
**	�� �� �� : 
**	����˵�� : 
**	��    �� : 
**	�� �� ֵ : 
**********************************************************************************************************/
void SGM58601_EXTI_INIT()
{
	/* EXTI INIT (modify) */
}

/**********************************************************************************************************
**	�� �� �� : SGM58601_INIT(SPI_HandleTypeDef* spi_handler)
**	����˵�� : SGM58601��ʼ��
**	��    �� : spi_handler: ָ��SPIģ��Ĵ���ָ��
**	�� �� ֵ : ��
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
**	�� �� �� : SGM58601_SWITCH_CH(SPI_HandleTypeDef* spi_handler, uint8_t CH)
**	����˵�� : SGM58601ͨ�������л�
**	��    �� : spi_handler: ָ��SPIģ��Ĵ���ָ��
**             CH: ͨ�����
**	�� �� ֵ : 
**********************************************************************************************************/
void SGM58601_SWITCH_CH(SPI_HandleTypeDef* spi_handler, uint8_t CH)
{
	SGM58601_CS_0(spi_handler);
	ADC_SET_CHANNEL(spi_handler, CH, AINCOM);
	SGM58601_CS_1(spi_handler);
}

/**********************************************************************************************************
**	�� �� �� : SGM58601_READ_DATA(SPI_HandleTypeDef* spi_handler)
**	����˵�� : SGM58601��ȡ����
**	��    �� : spi_handler: ָ��SPIģ��Ĵ���ָ��
**	�� �� ֵ : ret: ��24λ��Ч
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
**	�� �� �� : SGM58601_READ_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr)
**	����˵�� : SGM58601���Ĵ���
**	��    �� : spi_handler: ָ��SPIģ��Ĵ���ָ��
**             reg_addr: �Ĵ�����ַ
**	�� �� ֵ : 
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
**	�� �� �� : SGM58601_READ_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr)
**	����˵�� : SGM58601���Ĵ���
**	��    �� : spi_handler: ָ��SPIģ��Ĵ���ָ��
**             reg_addr: �Ĵ�����ַ
**	�� �� ֵ : 
**********************************************************************************************************/
void SGM58601_WRITE_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr, uint8_t reg_data)
{
	SGM58601_CS_0(spi_handler);
	ADC_WRITE_TO_ONE_REG(spi_handler, reg_addr, reg_data);
	SGM58601_CS_1(spi_handler);
}

/**********************************************************************************************************
**	�� �� �� : HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
**	����˵�� : ���ݶ�ȡ����
**	��    �� : 
**	�� �� ֵ : 
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
**	�� �� �� : ADC_WRITE_TO_ONE_REG()
**	����˵�� : SGM58601 SPIдһ���Ĵ���
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_WRITE_TO_ONE_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr, uint8_t reg_data)
{
	uint8_t txData[3] = {0};
	uint8_t rxData = 0;
	uint8_t wreg = 0x50;

	wreg = wreg | reg_addr;	// �ϳ�д��ַ
	txData[0] = wreg;		// д��ַ
	txData[1] = 0;			// д�ֽ���
	txData[2] = reg_data;	// д����
	HAL_SPI_TransmitReceive(spi_handler, &txData[0], &rxData, 1, 0xff);
	HAL_SPI_TransmitReceive(spi_handler, &txData[1], &rxData, 1, 0xff);
	HAL_SPI_TransmitReceive(spi_handler, &txData[2], &rxData, 1, 0xff);
}

/**********************************************************************************************************
**	�� �� �� : ADC_READ_FROM_ONE_REG()
**	����˵�� : SGM58601 SPI��һ���Ĵ���
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
uint8_t ADC_READ_FROM_ONE_REG(SPI_HandleTypeDef* spi_handler, uint8_t reg_addr)
{
	uint8_t txData[3] = {0};
	uint8_t rxData = 0;
	uint8_t rreg = 0x10;

	rreg = rreg | reg_addr;	// �ϳɶ���ַ
	txData[0] = rreg;		// ����ַ
	txData[1] = 0;			// ���ֽ���
	HAL_SPI_TransmitReceive(spi_handler, &txData[0], &rxData, 1, 0xff);
	HAL_SPI_TransmitReceive(spi_handler, &txData[1], &rxData, 1, 0xff);
	bsp_DelayUS(10);
	HAL_SPI_TransmitReceive(spi_handler, &txData[2], &rxData, 1, 0xff);

	return rxData;
}

/**********************************************************************************************************
**	�� �� �� : ADC_COMMAND_SET
**	����˵�� : SGM58601ָ������
**	��    �� : command - WAKEUP_COMMAND   : Completes SYNC and Exits Standby Mode
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
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_COMMAND_SET(SPI_HandleTypeDef* spi_handler, uint8_t command)
{
	uint8_t txDate = command;
	uint8_t rxDate = 0;
	HAL_SPI_TransmitReceive(spi_handler, &txDate, &rxDate, 1, 0xff);
}

/**********************************************************************************************************
**	�� �� �� : ADC_SET_CHANNEL()
**	����˵�� : SGM58601 ѡ������ͨ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SET_CHANNEL(SPI_HandleTypeDef* spi_handler, uint8_t AIN_P, uint8_t AIN_N)
{
	uint8_t channel = (AIN_P << 4) + AIN_N;

	ADC_WRITE_TO_ONE_REG(spi_handler, MUX_REG_ADDR, channel);
}

/**********************************************************************************************************
**	�� �� �� : ADC_SET_PGA()
**	����˵�� : SGM58601 ����PGA
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : ADC_SET_DATARATE()
**	����˵�� : SGM58601 ���������������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SET_DATARATE(SPI_HandleTypeDef* spi_handler, uint8_t DATARATE)
{
	ADC_WRITE_TO_ONE_REG(spi_handler, DRATE_REG_ADDR, DATARATE);
}

/**********************************************************************************************************
**	�� �� �� : ADC_SET_SDCS()
**	����˵�� : SGM58601 ���ô���������·���
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : ADC_SET_CLKOUT()
**	����˵�� : SGM58601 ����CLKƵ��
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : ADC_EN_INPUTBUFFER()
**	����˵�� : SGM58601 ʹ�����뻺��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_EN_INPUTBUFFER(SPI_HandleTypeDef* spi_handler)
{
	uint8_t temp = 0;

	temp = ADC_READ_FROM_ONE_REG(spi_handler, STATUS_REG_ADDR);
	temp = temp | 0x02;
	ADC_WRITE_TO_ONE_REG(spi_handler, STATUS_REG_ADDR, temp);
}

/**********************************************************************************************************
**	�� �� �� : ADC_DISABLE_INPUTBUFFER()
**	����˵�� : SGM58601 ʧ�����뻺��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_DISABLE_INPUTBUFFER(SPI_HandleTypeDef* spi_handler)
{
	uint8_t temp = 0;
	
	temp = ADC_READ_FROM_ONE_REG(spi_handler, STATUS_REG_ADDR);
	temp = temp & 0xfd;
	ADC_WRITE_TO_ONE_REG(spi_handler, STATUS_REG_ADDR, temp);
}

/**********************************************************************************************************
**	�� �� �� : ADC_EN_AUTOCALIBRATION()
**	����˵�� : SGM58601 ʹ���Զ�У׼
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_EN_AUTOCALIBRATION(SPI_HandleTypeDef* spi_handler)
{
	uint8_t temp = 0;
	
	temp = ADC_READ_FROM_ONE_REG(spi_handler, STATUS_REG_ADDR);
	temp = temp | 0x04;
	ADC_WRITE_TO_ONE_REG(spi_handler, STATUS_REG_ADDR, temp);
}

/**********************************************************************************************************
**	�� �� �� : ADC_DISABLE_AUTOCALIBRATION()
**	����˵�� : SGM58601 ʧ���Զ�У׼
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_DISABLE_AUTOCALIBRATION(SPI_HandleTypeDef* spi_handler)
{
	uint8_t temp = 0;
	
	temp = ADC_READ_FROM_ONE_REG(spi_handler, STATUS_REG_ADDR);
	temp = temp & 0xfb;
	ADC_WRITE_TO_ONE_REG(spi_handler, STATUS_REG_ADDR, temp);
}

/**********************************************************************************************************
**	�� �� �� : ADC_MSB_FIRST()
**	����˵�� : SGM58601  �����������˳��MSB_FIRST
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_MSB_FIRST(SPI_HandleTypeDef* spi_handler)
{
	uint8_t temp = 0;
	temp = ADC_READ_FROM_ONE_REG(spi_handler, STATUS_REG_ADDR);
	temp = temp & 0xf7;
	ADC_WRITE_TO_ONE_REG(spi_handler, STATUS_REG_ADDR, temp);
}

/**********************************************************************************************************
**	�� �� �� : ADC_LSB_FIRST()
**	����˵�� : SGM58601  �����������˳��LSB_FIRST
**	��    �� : ��
**	�� �� ֵ : ��
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
