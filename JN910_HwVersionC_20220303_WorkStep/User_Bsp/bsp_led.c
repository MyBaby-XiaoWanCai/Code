/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_led.c
** Author      : Huang Cheng
** Date        : 2022-01-13
** Description : bsp_led source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_i2c.h"
#include "bsp_pca9555.h"
#include "bsp_led.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t LedOffSta[2] = {0xFF, 0xFF};

uint8_t I2cPcaLed = 0x00;                     //I2C控制LED的设备号

/* 通道指示灯的I2C设备参数 */
static I2C_ParamType LedParam = 
{
	I2C2_SCL_GPIO_Port,
	I2C2_SCL_Pin,
	I2C2_SDA_GPIO_Port,
	I2C2_SDA_Pin,
	2,
};

static I2C_UserFuncType *I2C_PcaLed = &I2C_UserFunc;     //I2C设备创建句柄
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : bsp_LedInit()
**	功能说明 : 灯板初始化函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_LedInit(void)
{
	uint8_t CreateSta = 0x00;
	
	//创建拨码I2C设备
	CreateSta = I2C_PcaLed->Create(&LedParam, &I2cPcaLed);
	
	//创建设备成功
	if(I2C_DEVICE_OK == CreateSta)
	{
		/* 配置PCA9555为输出模式 */
		PCA9555_ConfigOutputReg(I2cPcaLed);
	}
	else
	{
		//do nothing;
	}
	
	/* 端口初始化输出为高，即所有灯熄灭 */
	bsp_AllLedOff();
}

/**********************************************************************************************************
**	函 数 名 : bsp_AllLedOff()
**	功能说明 : 灯板所有灯设置为熄灭状态
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_AllLedOff(void)
{
	//Run灯熄灭
	HAL_GPIO_WritePin(LED0_R_STM32_GPIO_Port, LED0_R_STM32_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED0_G_STM32_GPIO_Port, LED0_G_STM32_Pin, GPIO_PIN_RESET);
	
	//通道灯熄灭
	Pca9555_RegWrite(I2cPcaLed, LedOffSta);
}

/**********************************************************************************************************
**	函 数 名 : bsp_LedOff()
**	功能说明 : 灯板指定的灯设置为熄灭状态
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_LedOff(Led_Type LedType)
{
	uint8_t ChLedSta[2] = {0};
	uint16_t LedSta = 0;
	
	switch(LedType)
	{
		case RUN_LED_R:
		{
			HAL_GPIO_WritePin(LED0_R_STM32_GPIO_Port, LED0_R_STM32_Pin, GPIO_PIN_RESET);
			break;
		}
		case RUN_LED_G:
		{
			HAL_GPIO_WritePin(LED0_G_STM32_GPIO_Port, LED0_G_STM32_Pin, GPIO_PIN_RESET);
			break;
		}
		case CH1_LED_R:
		case CH1_LED_G:
		case CH2_LED_R:
		case CH2_LED_G:
		case CH3_LED_R:
		case CH3_LED_G:
		case CH4_LED_R:
		case CH4_LED_G:
		case CH5_LED_R:
		case CH5_LED_G:
		case CH6_LED_R:
		case CH6_LED_G:
		case CH7_LED_R:
		case CH7_LED_G:
		case CH8_LED_R:
		case CH8_LED_G:
		{
			Pca9555_RegRead(I2cPcaLed, ChLedSta);
			LedSta = ((uint16_t)ChLedSta[1]<<8) | ChLedSta[0];
			LedSta |= (uint16_t)(1<<(LedType-CH1_LED_R));
			
			ChLedSta[0] =  LedSta & 0xFF;
			ChLedSta[1] =  LedSta >> 0x08;
			
			Pca9555_RegWrite(I2cPcaLed, ChLedSta);
			break;
		}
		default:
		{
			break;
		}
	}
}

/**********************************************************************************************************
**	函 数 名 : bsp_LedOn()
**	功能说明 : 灯板指定的灯设置亮状态
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_LedOn(Led_Type LedType)
{
	uint8_t ChLedSta[2] = {0};
	uint16_t LedSta = 0;
	
	switch(LedType)
	{
		case RUN_LED_R:
		{
			HAL_GPIO_WritePin(LED0_R_STM32_GPIO_Port, LED0_R_STM32_Pin, GPIO_PIN_SET);
			break;
		}
		case RUN_LED_G:
		{
			HAL_GPIO_WritePin(LED0_G_STM32_GPIO_Port, LED0_G_STM32_Pin, GPIO_PIN_SET);
			break;
		}
		case CH1_LED_R:
		case CH1_LED_G:
		case CH2_LED_R:
		case CH2_LED_G:
		case CH3_LED_R:
		case CH3_LED_G:
		case CH4_LED_R:
		case CH4_LED_G:
		case CH5_LED_R:
		case CH5_LED_G:
		case CH6_LED_R:
		case CH6_LED_G:
		case CH7_LED_R:
		case CH7_LED_G:
		case CH8_LED_R:
		case CH8_LED_G:
		{
			Pca9555_RegRead(I2cPcaLed, ChLedSta);
			LedSta = ((uint16_t)ChLedSta[1]<<8) | ChLedSta[0];
			LedSta &= ~((uint16_t)(1<<(LedType-CH1_LED_R)));
			
			ChLedSta[0] =  LedSta & 0xFF;
			ChLedSta[1] =  LedSta >> 0x08;
			
			Pca9555_RegWrite(I2cPcaLed, ChLedSta);
			break;
		}
		default:
		{
			break;
		}
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
	uint8_t LedStatus[2] = {0};
    static uint16_t ChLedStatus = 0;
	
	switch(_no)
	{
		case RUN_LED_R:
		{
			HAL_GPIO_TogglePin(LED0_R_STM32_GPIO_Port, LED0_R_STM32_Pin);
			break;
		}
		case RUN_LED_G:
		{
			HAL_GPIO_TogglePin(LED0_G_STM32_GPIO_Port, LED0_G_STM32_Pin);
			break;
		}
		case CH1_LED_R:
		case CH1_LED_G:
		case CH2_LED_R:
		case CH2_LED_G:
		case CH3_LED_R:
		case CH3_LED_G:
		case CH4_LED_R:
		case CH4_LED_G:
		case CH5_LED_R:
		case CH5_LED_G:
		case CH6_LED_R:
		case CH6_LED_G:
		case CH7_LED_R:
		case CH7_LED_G:
		case CH8_LED_R:
		case CH8_LED_G:
		{
			Pca9555_RegRead(I2cPcaLed, LedStatus);
			ChLedStatus = ((uint16_t)LedStatus[1]<<8) | LedStatus[0];
			ChLedStatus &= ~((uint16_t)(1<<(_no-CH1_LED_R)));
			
			LedStatus[0] =  ChLedStatus & 0xFF;
			LedStatus[1] =  ChLedStatus >> 0x08;
			
			Pca9555_RegWrite(I2cPcaLed, LedStatus);
			break;
		}
		default:
		{
			break;
		}
	}
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

