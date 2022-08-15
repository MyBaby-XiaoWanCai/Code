/*
*********************************************************************************************************
*
*	模块名称 : dac80508 驱动
*	文件名称 : dac80508.c
*	版    本 : V1.0
*	说    明 : DAC80508模块和CPU之间采用硬件SPI接口。
*
*
*
*	修改记录 :
*	版本号  日期         作者     说明
*	V1.0    2020-03-02   ZYL      正式发布
*********************************************************************************************************
*/

#include "main.h"

/*
	DAC80508基本特性:
	1、供电2.7 - 5.5V;  【本例使用5V】
	4、外部参考电压2.5V，增益到5V

	对SPI的时钟速度要求: 高达50MHz， 速度很快,实际使用45MHz
	SCLK下降沿读取数据, 每次传送24bit数据， 高位先传
*/

/* 定义GPIO端口 */
#define DAC80508_PORT_CS1		GPIOE
#define DAC80508_PIN_CS1		GPIO_PIN_11

#define DAC80508_PORT_CS2		GPIOF
#define DAC80508_PIN_CS2		GPIO_PIN_10

/* 定义口线置0和置1的宏 */
#define DAC80508_CS1_0()		HAL_GPIO_WritePin(DAC80508_PORT_CS1, DAC80508_PIN_CS1, GPIO_PIN_RESET);
#define DAC80508_CS1_1()		HAL_GPIO_WritePin(DAC80508_PORT_CS1, DAC80508_PIN_CS1, GPIO_PIN_SET);
#define DAC80508_CS2_0()		HAL_GPIO_WritePin(DAC80508_PORT_CS2, DAC80508_PIN_CS2, GPIO_PIN_RESET);
#define DAC80508_CS2_1()		HAL_GPIO_WritePin(DAC80508_PORT_CS2, DAC80508_PIN_CS2, GPIO_PIN_SET);

static void DAC80508_ConfigGPIO(void);
static int32_t DAC80508_WriteCmd(uint32_t _cmd, uint8_t index);
/*
*********************************************************************************************************
*	函 数 名: InitDAC80508
*	功能说明: 配置STM32的GPIO和SPI接口，用于连接 DAC80508
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int32_t InitDAC80508(void)
{
	int32_t ret = 0;
	DAC80508_ConfigGPIO();

	/* 配置寄存器 使用外部参考，不使用CRC校验*/
	ret =DAC80508_WriteCmd((3 << 16) | 0x100,0);
	if(ret!=0)
	return -1;
	/* 增益寄存器 参考电压2.5V乘以2 */
	ret =DAC80508_WriteCmd((4 << 16) | 0xFF,0);
	if(ret!=0)
	return -1;
	/* 配置寄存器 使用外部参考，不使用CRC校验*/
	ret =DAC80508_WriteCmd((3 << 16) | 0x100,1);
	if(ret!=0)
	return -1;
	/* 增益寄存器 参考电压2.5V乘以2 */
	ret =DAC80508_WriteCmd((4 << 16) | 0xFF,1);
	if(ret!=0)
	return -1;
	/* 复位2个DAC 输出0V */
  ret =DAC80508_BRDCAST(0x0000,0);
	if(ret!=0)
	return -1;
	ret =DAC80508_BRDCAST(0x0000,1);
	if(ret!=0)
	return -1;
}

/*
*********************************************************************************************************
*	函 数 名: DAC80508_ConfigGPIO
*	功能说明: 配置GPIO
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DAC80508_ConfigGPIO(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	/* 打开GPIO时钟 */
	__HAL_RCC_GPIOE_CLK_ENABLE(); /* 打开GPIOE时钟 */
	__HAL_RCC_GPIOF_CLK_ENABLE(); /* 打开GPIOF时钟 */


	/* 配置推挽输出IO */
	GPIO_Initure.Pin = DAC80508_PIN_CS1; /*输出引脚 */
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* 设为推挽模式 */
	GPIO_Initure.Pull = GPIO_NOPULL;         /* 上下拉电阻不使能 */
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO口最大速度 */
	HAL_GPIO_Init(GPIOE, &GPIO_Initure);     //GPIOE

		/* 配置推挽输出IO */
	GPIO_Initure.Pin = DAC80508_PIN_CS2; /*输出引脚 */
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* 设为推挽模式 */
	GPIO_Initure.Pull = GPIO_NOPULL;         /* 上下拉电阻不使能 */
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO口最大速度 */
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);     //GPIOF

	DAC80508_CS1_1();
	DAC80508_CS2_1();
}

/*
*********************************************************************************************************
*	函 数 名: DAC80508_WriteCmd
*	功能说明: 向SPI总线发送24个bit数据。
*	形    参: _cmd : 数据 index:设备编号
*	返 回 值: 0正常
*********************************************************************************************************
*/

int32_t DAC80508_WriteCmd(uint32_t _cmd, uint8_t index)
{
	uint8_t Txdata[3]={_cmd>>16,(_cmd>>8)&0x00FF,_cmd&0x0000FF};
	uint8_t Rxdata[3]={0,0,0};
	int32_t ret = 0;
  if(index==0)
	{
    DAC80508_CS1_0();
    ret=HAL_SPI_TransmitReceive(&hspi4, Txdata, Rxdata,3,1000);
	  DAC80508_CS1_1();
	}
	else
	{
    DAC80508_CS2_0();
    ret=HAL_SPI_TransmitReceive(&hspi5, Txdata, Rxdata,3,1000);
	  DAC80508_CS2_1();
	}
	return ret;
}
/*
*********************************************************************************************************
*	函 数 名: DAC8562_SetDacData
*	功能说明: 设置DAC输出,单通道立即更新。
*	形    参: _ch, 通道, 0 , 1
*		     _data : 数据 index：设备索引
*	返 回 值: 0正常
*********************************************************************************************************
*/
int32_t DAC80508_SetDacData(uint8_t _ch, uint16_t _dac, uint8_t index)
{
	int32_t ret = 0;
	ret=DAC80508_WriteCmd(((0x8+_ch) << 16) | _dac, index);
	HAL_Delay(10);
	return ret;
}
/*
*********************************************************************************************************
*	函 数 名: DAC80508_BRDCAST
*	功能说明: 设置DAC输出,广播发送，值需要一样
*	形    参: _dac：值 index：设备索引
*
*	返 回 值: 0正常
*********************************************************************************************************
*/
int32_t DAC80508_BRDCAST(uint16_t _dac, uint8_t index)
{
	int32_t ret = 0;
	ret=DAC80508_WriteCmd((0x6 << 16) | _dac, index);
	return ret;
}
/*****************************(END OF FILE) *********************************/
