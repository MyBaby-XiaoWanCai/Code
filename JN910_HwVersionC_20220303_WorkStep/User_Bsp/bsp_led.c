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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_i2c.h"
#include "bsp_pca9555.h"
#include "bsp_led.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t LedOffSta[2] = {0xFF, 0xFF};

uint8_t I2cPcaLed = 0x00;                     //I2C����LED���豸��

/* ͨ��ָʾ�Ƶ�I2C�豸���� */
static I2C_ParamType LedParam = 
{
	I2C2_SCL_GPIO_Port,
	I2C2_SCL_Pin,
	I2C2_SDA_GPIO_Port,
	I2C2_SDA_Pin,
	2,
};

static I2C_UserFuncType *I2C_PcaLed = &I2C_UserFunc;     //I2C�豸�������
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : bsp_LedInit()
**	����˵�� : �ư��ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_LedInit(void)
{
	uint8_t CreateSta = 0x00;
	
	//��������I2C�豸
	CreateSta = I2C_PcaLed->Create(&LedParam, &I2cPcaLed);
	
	//�����豸�ɹ�
	if(I2C_DEVICE_OK == CreateSta)
	{
		/* ����PCA9555Ϊ���ģʽ */
		PCA9555_ConfigOutputReg(I2cPcaLed);
	}
	else
	{
		//do nothing;
	}
	
	/* �˿ڳ�ʼ�����Ϊ�ߣ������е�Ϩ�� */
	bsp_AllLedOff();
}

/**********************************************************************************************************
**	�� �� �� : bsp_AllLedOff()
**	����˵�� : �ư����е�����ΪϨ��״̬
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_AllLedOff(void)
{
	//Run��Ϩ��
	HAL_GPIO_WritePin(LED0_R_STM32_GPIO_Port, LED0_R_STM32_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED0_G_STM32_GPIO_Port, LED0_G_STM32_Pin, GPIO_PIN_RESET);
	
	//ͨ����Ϩ��
	Pca9555_RegWrite(I2cPcaLed, LedOffSta);
}

/**********************************************************************************************************
**	�� �� �� : bsp_LedOff()
**	����˵�� : �ư�ָ���ĵ�����ΪϨ��״̬
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : bsp_LedOn()
**	����˵�� : �ư�ָ���ĵ�������״̬
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : bsp_LedToggle()
**	����˵�� : ��תָ����LEDָʾ��
**	��    �� : _no : ָʾ�����(0~12)
**	�� �� ֵ : ��
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
**	�� �� �� : Led_Shine_Ctrl()
**	����˵�� : ���ݲ�ͬ�Ĺ���״̬����LED������Ӧ����
**	��    �� : ��
**	�� �� ֵ : ��
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

