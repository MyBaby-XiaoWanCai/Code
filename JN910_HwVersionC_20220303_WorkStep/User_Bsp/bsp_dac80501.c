/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_dac80501.c
** Author      : Huang Cheng
** Date        : 2022-01-06
** Description : bsp_dac80501 source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include <math.h>
#include "spi.h"
#include "main.h"
#include "bsp_dwt.h"
#include "bsp_dac80501.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
DAC80501_CsType DAC80501_CsPortPinTab[DAC80501_CS_NUM] = 
{
	{STM32_DAC_CS1_GPIO_Port, STM32_DAC_CS1_Pin},
	{STM32_DAC_CS2_GPIO_Port, STM32_DAC_CS2_Pin},
	{STM32_DAC_CS3_GPIO_Port, STM32_DAC_CS3_Pin},
	{STM32_DAC_CS4_GPIO_Port, STM32_DAC_CS4_Pin},
	{STM32_DAC_CS5_GPIO_Port, STM32_DAC_CS5_Pin},
	{STM32_DAC_CS6_GPIO_Port, STM32_DAC_CS6_Pin},
	{STM32_DAC_CS7_GPIO_Port, STM32_DAC_CS7_Pin},
	{STM32_DAC_CS8_GPIO_Port, STM32_DAC_CS8_Pin},
};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void DAC80501_CS_SET(uint8_t Dev);
static void DAC80501_CS_CLR(uint8_t Dev);
static void Dac80501_Wait(void);
static void DAC80501_RegCofig(uint8_t Dev);
static void DAC80501_WriteCmd(uint8_t Dev, uint32_t Cmd);
static void DAC80501_ExactTimeStep(uint16_t StepNum, uint8_t InterpNum, uint16_t TimeStep, float TimeDec);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/**********************************************************************************************************
**	函 数 名 : bsp_Dac80501Init()
**	功能说明 : DAC80501初始化函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_Dac80501Init(void)
{
	//spi初始化
	MX_SPI2_Init();
	
	//所有DAC80501芯片的片选都拉高
	for(uint8_t i = 0; i < DAC80501_CS_NUM; i++)
	{
		DAC80501_CS_SET(i);
	}
	
	__HAL_SPI_ENABLE(&hspi2);
	
	//配置所有DAC80501芯片的寄存器
	for(uint8_t i = 0; i < DAC80501_CS_NUM; i++)
	{
		DAC80501_RegCofig(i);
	}
}

/**********************************************************************************************************
**	函 数 名 : DAC80501_CS_SET()
**	功能说明 : DAC80501片选禁止
**	形    参 : Dev - 设备类型
**	返 回 值 : 无
**********************************************************************************************************/
static void DAC80501_CS_SET(uint8_t Dev)
{
	/* 传入参数合法性判断 */
	if(Dev >= DAC80501_CS_NUM)
	{
		return;
	}
	
	HAL_GPIO_WritePin((GPIO_TypeDef *)DAC80501_CsPortPinTab[Dev].CsPort, DAC80501_CsPortPinTab[Dev].Cspin, GPIO_PIN_SET);      
}

/**********************************************************************************************************
**	函 数 名 : DAC80501_CS_CLR()
**	功能说明 : DAC80501片选选中
**	形    参 : Dev - 设备类型
**	返 回 值 : 无
**********************************************************************************************************/
static void DAC80501_CS_CLR(uint8_t Dev)
{
	/* 传入参数合法性判断 */
	if(Dev >= DAC80501_CS_NUM)
	{
		return;
	}
	
	HAL_GPIO_WritePin((GPIO_TypeDef *)DAC80501_CsPortPinTab[Dev].CsPort, DAC80501_CsPortPinTab[Dev].Cspin, GPIO_PIN_RESET);      
}

/**********************************************************************************************************
**	函 数 名 : Dac80501_Wait()
**	功能说明 : Ads131芯片复位后等待函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void Dac80501_Wait(void)
{
	bsp_DelayMS(1);
}

/**********************************************************************************************************
**	函 数 名 : DAC80501_RegCofig()
**	功能说明 : DAC80501寄存器配置函数
**	形    参 : Dev - 设备类型
**	返 回 值 : 无
**********************************************************************************************************/
static void DAC80501_RegCofig(uint8_t Dev)
{
	//软件复位DAC80501
	DAC80501_WriteCmd(Dev, (DAC80501_TRIGGER|TRIGGER_SOFT_RESET));
	
	//软件复位等待时间，暂时给1ms
	Dac80501_Wait();
	
	//配置选择外部参考电压2.5V
	DAC80501_WriteCmd(Dev, (DAC80501_CONFIG|CONFIG_REF_PWDWN));
	
	//配置选择电压不分压，输出增益为2即最大输出值为5V
	DAC80501_WriteCmd(Dev, (DAC80501_GAIN|GAIN_REF_DIV|GAIN_BUFF_GAIN));
	
	//设置DAC80501输出电压为0V
	DAC80501_WriteCmd(Dev, (DAC80501_DAC|DAC_DEFDATA_SET));
}

/**********************************************************************************************************
**	函 数 名 : DAC80501_WriteCmd()
**	功能说明 : DAC80501写指令函数
**	形    参 : Dev - 设备类型，Cmd - 命令数据
**	返 回 值 : 无
**********************************************************************************************************/
static void DAC80501_WriteCmd(uint8_t Dev, uint32_t Cmd)
{
	uint8_t TxData[3] = {(Cmd>>16)&0xFF, (Cmd>>8)&0xFF, (Cmd&0xFF)};
	HAL_StatusTypeDef SpiRet = HAL_OK;
	
	DAC80501_CS_CLR(Dev);      //片选拉低
	SpiRet = HAL_SPI_Transmit(&hspi2, TxData, sizeof(TxData), 1000);
	DAC80501_CS_SET(Dev);      //片选拉高
	
	if(SpiRet != HAL_OK)
	{
		//错误处理
	}
}

/**********************************************************************************************************
**	函 数 名 : DAC80501_ExactTimeStep()
**	功能说明 : DAC80501单步精确延时函数
**	形    参 : StepNum - 步数，InterpNum - 插补次数，TimeStep - 单步延时时间，TimeDec - 延时单步时间小数
**	返 回 值 : 无
**  说    明 ：当计算延时单步时间为整数时不需要插补;
               当延时单步时间为小数时，则采用10步一插补算法，依次把上一个10步的小数时间补回来;
               应用该算法总时间误差为0.038467%，直接对时间取整总时间误差为3.846159%，
               当步数为1000步，单步时间为10.40001us时。
**********************************************************************************************************/
static void DAC80501_ExactTimeStep(uint16_t StepNum, uint8_t InterpNum, uint16_t TimeStep, float TimeDec)
{
	static uint16_t DelayCnt = 0; //延时次数
	static uint8_t  InterpCnt = 0;//插补计数次数
	
	DelayCnt++;
	if(DelayCnt > StepNum)
	{
		DelayCnt = 0;
	}
	
	//小数部分等于0
	if((TimeDec >= -FLAOT_ZERO) && (TimeDec <= FLAOT_ZERO))
	{
		bsp_DelayUS(TimeStep);
	}
	else
	{		
		//延时时间小数插补
		if(0x01<=((DelayCnt/10)) && (InterpCnt<InterpNum))
		{
			InterpCnt++;
			bsp_DelayUS(TimeStep + 0x01);
		}
		else
		{
			bsp_DelayUS(TimeStep);
		}
		
		//清零插补计数次数
		if(0x00==((DelayCnt%10)) && (InterpCnt>=InterpNum))
		{
			InterpCnt = 0;
		}
	}
}

/**********************************************************************************************************
**	函 数 名 : DAC80501_SetOutVol()
**	功能说明 : DAC80501设置输出电压函数
**	形    参 : Dev - 设备类型，DacVal - DA值
**	返 回 值 : 无
**********************************************************************************************************/
void DAC80501_SetOutVol(uint8_t Dev, uint16_t DacVal)
{
	DAC80501_WriteCmd(Dev, (DAC80501_DAC|DacVal));
}

/**********************************************************************************************************
**	函 数 名 : DAC80501_SwitchCtrl()
**	功能说明 : DA切换控制函数
**	形    参 : Dev - 设备类型，Nms - DA控制时间，StepNum - 步数
               DacVolMin - DA电压最小值，DacVolMax - DA电压最大值
**	返 回 值 : 无
**********************************************************************************************************/
void DAC80501_SwitchCtrl(uint8_t Dev, uint16_t Nms, uint16_t StepNum, float DacVolMin, float DacVolMax)
{
	uint16_t DacValMin = 0;
	uint16_t DacValMax = 0;
	float DacOutVal = 0;
	float DacValStep = 0.0;
	float DacTimeStep = 0.0;
	
	/* 入口参数合法性的检查 */
//	if(((DacVolMax-DacVolMin) <= FLAOT_ZERO) || ((DAC_VOL_MAX-DacVolMax) <= FLAOT_ZERO) ||  \
//	   ((DacVolMin <= FLAOT_ZERO) || (DacVolMax <= FLAOT_ZERO)) || (Dev >= DAC80501_CS_NUM))
//	{
//		return;
//	}
	
	//将电压值转换成DA值
	DacValMin = (uint16_t)((DacVolMin/DAC_VOL_MAX) * 65535.0);
	DacValMax = (uint16_t)((DacVolMax/DAC_VOL_MAX) * 65535.0);
	
	//计算DA单步值、DA单步时间us
	DacValStep = (float)((DacValMax-DacValMin)) / StepNum;
	DacTimeStep = (float)((Nms*1000.0)/StepNum) - RESP_TIME;
	DacOutVal = DacValMin;
	
	//设置DAC线性输出
	for(uint16_t i = 0; i< StepNum; i++)
	{
		DacOutVal += DacValStep;
		
		if((uint16_t)DacOutVal >= DacValMax)
		{
			DAC80501_SetOutVol(Dev, DacValMax);
			break;
		}
		else
		{
			DAC80501_SetOutVol(Dev, (uint16_t)DacOutVal);
			bsp_DelayUS((uint16_t)DacTimeStep);           //粗略延时，如果对延时要求不高可采用
		}
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

