/*
*********************************************************************************************************
*
*	模块名称 : AD7124_8驱动模块(8通道带PGA的24位ADC)
*	文件名称 : ad7124_8.c
*	版    本 : V1.0
*	说    明 : AD7124模块和CPU之间采用SPI接口。本驱动程序使用硬件SPI。
*			  
*
*	修改记录 :
*		版本号  日期         作者     说明
*		V1.0    2020-03-27   ZYL      正式发布
*********************************************************************************************************
*/

#include "main.h"
#include <stdbool.h>
/*
	AD7124基本特性:
	1、模拟部分供电5V;
	2、SPI数字接口电平：3.3V
	3、参考电压2.5V (内置）
	4、输入电压范围：
	5、SPI时钟最大5M，实际使用2.8125M
*/

/* Error codes */

#define INVALID_VAL -1 /* Invalid argument */

#define COMM_ERR    -2 /* Communication error on receive */

#define TIMEOUT     -3 /* A timeout has occured */

#define AD7124_POST_RESET_DELAY      4


/* 定义GPIO端口 */
	#define AD7124_PORT_CS		             GPIOI
	#define AD7124_PIN_CS		               GPIO_PIN_0
	
	/* 定义口线置0和置1的宏 */
	#define AD7124_CS_0()		HAL_GPIO_WritePin(AD7124_PORT_CS, AD7124_PIN_CS, GPIO_PIN_RESET);
	#define AD7124_CS_1()		HAL_GPIO_WritePin(AD7124_PORT_CS, AD7124_PIN_CS, GPIO_PIN_SET);

uint8_t AD7124_Start_Read=0;


/*
*********************************************************************************************************
*	函 数 名: Delay
*	功能说明: 
*	形    参:  
*	返 回 值: 无
*********************************************************************************************************
*/

static void Delay(uint32_t cnt)
{
	uint32_t i;
	for (i = 0; i < cnt; i++);	
}
/*
*********************************************************************************************************
*	函 数 名: InitAD7124
*	功能说明: 初始化AD7124
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int32_t InitAD7124(void)
{
	GPIO_InitTypeDef GPIO_Initure;
  	int32_t ret = 0;

	__HAL_RCC_GPIOI_CLK_ENABLE(); /* 打开GPIOD时钟 */

	GPIO_Initure.Pin =  AD7124_PIN_CS; /*输出引脚 */
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* 设为推挽模式 */
	GPIO_Initure.Pull = GPIO_NOPULL;         /* 上下拉电阻不使能 */
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO口最大速度 */
	HAL_GPIO_Init(AD7124_PORT_CS, &GPIO_Initure);     //GPIOI
	AD7124_CS_1();
  	ret=AD7124_CfgADC();
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);//使能外部中断
	if(AD7124_Start_Read==0)
  	{
	  	AD7124_Start_Read=1;
    	AD7124_CS_0();
  	}
	return ret;
}
/*
*********************************************************************************************************
*	函 数 名: ad7124_wait_to_power_on
*	功能说明: ad7124等待电源打开
*	形    参: uint32_t timeout
*			   
*	返 回 值: -1错误
*********************************************************************************************************
*/
int32_t ad7124_wait_to_power_on(uint32_t timeout)
{
	int32_t ret;
	int8_t powered_on = 0;
  uint8_t value[1] = {0};
	
	while(!powered_on && timeout--) 
	{
		ret = AD7124_ReadReg(AD7124_STATUS_REG,value,1);
		if(ret < 0)
		return ret;
		/* Check the POR_FLAG bit in the Status Register */
		powered_on = (value[0] &AD7124_STATUS_REG_POR_FLAG) == 0;
	}
	return (timeout || powered_on) ? 0 : -1;
}
/*
*********************************************************************************************************
*	函 数 名: AD7124_Reset
*	功能说明: AD7124复位
*	形    参: 
*			   
*	返 回 值: -1错误
*********************************************************************************************************
*/
int32_t AD7124_Reset(void)
{
	int32_t ret = 0;
	uint8_t Txdata[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
  AD7124_CS_0();
  ret=HAL_SPI_TransmitReceive(&hspi2, Txdata, Rxdata,8,100*8);
	AD7124_CS_1();
	if(ret!=0)
	return -1;		
	ret=ad7124_wait_to_power_on(800);
	if(ret!=0)
	return -1;	
	/* CRC is disabled after reset */
	//use_crc = AD7124_DISABLE_CRC;
	Delay(8000);
	return ret;

}
/*
*********************************************************************************************************
*	函 数 名: AD7124_WriteReg
*	功能说明: AD7124写寄存器
*	形    参: 
*			   
*	返 回 值: -1错误
*********************************************************************************************************
*/
int32_t AD7124_WriteReg(uint8_t addr,int32_t Value,uint8_t size)
{
	int32_t ret = 0;
	uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t i        = 0;
	uint8_t crc8     = 0;
	/* Build the Command word */
	Txdata[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_WR | AD7124_COMM_REG_RA(addr);

	/* Fill the write buffer */
	for(i = 0; i < size; i++) 
	{
		Txdata[size - i] = Value & 0xFF;
		Value >>= 8;
	}

	/* Compute the CRC */
	//if(useCRC != AD717X_DISABLE) 
	{
		crc8 = ad7124_compute_crc8(Txdata, size + 1);
		Txdata[size + 1] = crc8;
	}

	/* Write data to the device */
  AD7124_CS_0();
  ret=HAL_SPI_TransmitReceive(&hspi2, Txdata, Rxdata,size+2,100*(size+2));//1字节地址，1字节校验
	AD7124_CS_1();
	
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: AD7124_ReadReg
*	功能说明: AD7124读寄存器
*	形    参: 
*			   
*	返 回 值: -1错误
*********************************************************************************************************
*/
uint8_t check_ready = 0;
int32_t AD7124_ReadReg(uint8_t addr,uint8_t * Value,uint8_t size)
{
	int32_t ret = 0;
	uint8_t Txdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t Rxdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t msgBuf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t i        = 0;
	uint8_t crc8     = 0;
	
	/* Build the Command word */
	Txdata[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD | AD7124_COMM_REG_RA(addr);
	
  /* Read data from the device */
  AD7124_CS_0();
  ret=HAL_SPI_TransmitReceive(&hspi2, Txdata, Rxdata,size+2,100*(size+2));//1字节校验，1字节命令
	AD7124_CS_1();

	/* Check the CRC */
	//if(useCRC == AD717X_USE_CRC) 
	{
		msgBuf[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD | AD7124_COMM_REG_RA(addr);		
		for(i = 1; i < size + 2; ++i) 
		{
			msgBuf[i] = Rxdata[i];//
		}
		crc8 = ad7124_compute_crc8(msgBuf, size + 2);
	}
	if(check_ready==1)
	{	
		if(crc8 != 0) 
		{
			/* ReadRegister checksum failed. */
			return COMM_ERR;
		}
  }
	/* Build the result */
	for(i = 1; i < size + 1; i++) 
	{
		Value[i-1]= Rxdata[i];
	}
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: AD7175_CfgADC
*	功能说明: 配置ADC参数
*	形    参: 
*			   
*	返 回 值: 
*********************************************************************************************************
*/

int32_t AD7124_CfgADC(void)
{	
  int32_t ret=0;
	HAL_NVIC_DisableIRQ(EXTI1_IRQn);//禁止中断
	/*  Reset the device interface.*/
	ret = AD7124_Reset();
	if (ret < 0)
	return ret;
	
	/* Update the device structure with power-on/reset settings */
	/* Initialize ADC register. */
	ret = AD7124_WriteReg(AD7124_ADC_CTRL_REG,AD7124_ADC_CTRL_REG_DATA_STATUS | AD7124_ADC_CTRL_REG_POWER_MODE(2)| AD7124_ADC_CTRL_REG_MODE(0),2);//读取含状态标识、满功率模式、连续转换模式，
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_IO_CTRL1_REG,0x000000,3);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_IO_CTRL2_REG,0x0000,2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_ERREN_REG,AD7124_ERREN_REG_SPI_CRC_ERR_EN,3);//SPI_CRC_ERR_EN==1、SPI_IGNORE_ERR=0
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH0_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(0)|AD7124_CH_MAP_REG_AINM(17),2);//启用通道0,参考AVSS
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH1_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(1)|AD7124_CH_MAP_REG_AINM(17),2);
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH2_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(2)|AD7124_CH_MAP_REG_AINM(17),2);
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH3_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(3)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH4_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(4)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH5_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(5)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH6_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(6)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH7_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(7)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH8_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(8)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH9_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(9)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH10_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(10)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH11_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(11)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH12_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(12)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH13_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(13)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH14_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(14)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CH15_MAP_REG,AD7124_CH_MAP_REG_CH_ENABLE|AD7124_CH_MAP_REG_SETUP(0)|AD7124_CH_MAP_REG_AINP(15)|AD7124_CH_MAP_REG_AINM(17),2);//
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_CFG0_REG,AD7124_CFG_REG_AIN_BUFP|AD7124_CFG_REG_AINN_BUFM|AD7124_CFG_REG_REF_SEL(0)|AD7124_CFG_REG_PGA(0),2);//单极性、+-2.5V、外部参考1
	if(ret < 0)
	return ret;
	ret = AD7124_WriteReg(AD7124_FILT0_REG,AD7124_FILT_REG_FILTER(0)|AD7124_FILT_REG_REJ60|AD7124_FILT_REG_POST_FILTER(3)|AD7124_FILT_REG_FS(1),3);//滤波类型Sinc4、Output Data Rate 19200(SPS)
	if(ret < 0)
	return ret;
	check_ready = 1;
	return ret;
}
/*
*********************************************************************************************************
*	函 数 名: AD7124_ComputeCRC8
*	功能说明: AD7124 CRC校验
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t ad7124_compute_crc8(uint8_t * p_buf, uint8_t buf_size)
{
	uint8_t i = 0;
	uint8_t crc = 0;

	while(buf_size) 
  {
		for(i = 0x80; i != 0; i >>= 1) 
		{
			bool cmp1 = (crc & 0x80) != 0;
			bool cmp2 = (*p_buf & i) != 0;
			if(cmp1 != cmp2) 
			{ /* MSB of CRC register XOR input Bit from Data */
				crc <<= 1;
				crc ^= AD7124_CRC8_POLYNOMIAL_REPRESENTATION;
			} 
			else 
			{
				crc <<= 1;
			}
		}
		p_buf++;
		buf_size--;
	}
 return crc;
}
/*
*********************************************************************************************************
*	函 数 名: AD7124_WaitFor_SPI_Ready
*	功能说明: 等待SPI准备好
*	形    参: 超时：long timeout = 1000;
*	返 回 值: 无
*********************************************************************************************************
*/
int32_t AD7124_WaitFor_SPI_Ready(uint32_t timeout)
{
	int32_t ret;
	int8_t ready = 0;
  uint8_t value[3] = {0,0,0};
	while(!ready && --timeout) 
	{
		/* Read the value of the Status Register */
		ret = AD7124_ReadReg(AD7124_ERR_REG,value,3);
		if(ret < 0)
		return ret;
		/* Check the RDY bit in the Status Register */
		ready = (value[2] & AD7124_ERR_REG_SPI_IGNORE_ERR) == 0;
	}
	return timeout ? 0 : TIMEOUT;
}
/*
*********************************************************************************************************
*	函 数 名: AD7124_WaitFor_Conv_Ready
*	功能说明: 等待转换准备好
*	形    参: 通道号：超时：long timeout = 1000;
*	返 回 值: <1:错误
*********************************************************************************************************
*/
int32_t AD7124_WaitFor_Conv_Ready(uint8_t* ch,uint32_t timeout)
{
	int32_t ret;
	int8_t ready = 0;
  uint8_t value[1] = {0};
	while(!ready && --timeout) 
	{
		/* Read the value of the Status Register */
		ret = AD7124_ReadReg(AD7124_STATUS_REG,value,1);
		if(ret < 0)
		return ret;
		/* Check the RDY bit in the Status Register */
		ready = (value[0] & AD7124_STATUS_REG_RDY) == 0;
		ch[0]= value[0] & 0x0f;
	}
	return timeout ? 0 : TIMEOUT;
}
/*
*********************************************************************************************************
*	函 数 名: AD7124_ReadData
*	功能说明: AD7124读取数据
*	形    参: 四个字节的指针
*	返 回 值: 无
*********************************************************************************************************
*/
int32_t AD7124_ReadData(uint8_t* pData)
{
	int32_t ret;
	/* Read the value of the Status Register */
	ret = AD7124_ReadReg(AD7124_DATA_REG, pData, 4);
	return ret;
}
/*
*********************************************************************************************************
*	函 数 名: AD7124_Process
*	功能说明: AD7124读取处理
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint32_t ad7124_value[16]={0};
void AD7124_Process(void)
{
 uint8_t value[4] = {0,0,0,0};
 uint32_t read=0;
 if(AD7124_ReadData(value)==0)
 {
		/* 读采样结果，3个字节，高字节在前 */
		read = (uint32_t)(value[0] << 16);
		read += (uint32_t)(value[1] << 8);
		read += value[2] << 0; 
	 
////	 	/* 负数进行扩展。24位有符号数扩展为32位有符号数 */
//		if (read & 0x800000)//为正值
//		{
//			read = read-0x800000;
//		}	
//		else //为负值
//		{
//		  read = (read+0x800000)|0xFF000000;
//		}
		if((value[3]&0xF0)==0)
		{
		  ad7124_value[value[3]&0x0F]=read;
		}
 }
 AD7124_CS_0();
}
/*****************************(END OF FILE) *********************************/
