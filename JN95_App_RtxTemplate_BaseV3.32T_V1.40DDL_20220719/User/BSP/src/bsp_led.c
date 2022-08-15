/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_led.c
** Author      : Huang Cheng
** Date        : 2021-11-05
** Description : This file is used to initialize and drive led
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_led.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/

/**********************************************************************************************************
**                                         ��̬����������
**********************************************************************************************************/
//GPIO PORT��ѯ��
static uint8_t LedGpioPortTab[LED_MAX_NUM+1] = 
{
#ifdef YCJ
    RUN_LED_PORT_R, RUN_LED_PORT_G, CH1_LED_PORT_R, CH1_LED_PORT_G,
    CH2_LED_PORT_R, CH2_LED_PORT_G, CH3_LED_PORT_R, CH3_LED_PORT_G,
    CH4_LED_PORT_R, CH4_LED_PORT_G, CH5_LED_PORT_R, CH5_LED_PORT_G,
    CH6_LED_PORT_R, CH6_LED_PORT_G, CH7_LED_PORT_R, CH7_LED_PORT_G,
    CH8_LED_PORT_R, CH8_LED_PORT_G, GPIO_PORT_NOUSE, GPIO_PORT_LED82,
    GPIO_PORT_LED83
#else
    RUN_LED_PORT_R, RUN_LED_PORT_G, CH1_LED_PORT_R, CH1_LED_PORT_G,
    CH2_LED_PORT_R, CH2_LED_PORT_G, CH3_LED_PORT_R, CH3_LED_PORT_G,
    CH4_LED_PORT_R, CH4_LED_PORT_G, GPIO_PORT_NOUSE, GPIO_PORT_LED82,
    GPIO_PORT_LED83
#endif
};
	
//GPIO PIN��ѯ��
static uint16_t LedGpioPinTab[LED_MAX_NUM+1] =  
{
	#ifdef YCJ
	RUN_LED_PIN_R, RUN_LED_PIN_G, CH1_LED_PIN_R, CH1_LED_PIN_G,
	CH2_LED_PIN_R, CH2_LED_PIN_G, CH3_LED_PIN_R, CH3_LED_PIN_G,
	CH4_LED_PIN_R, CH4_LED_PIN_G, CH5_LED_PIN_R, CH5_LED_PIN_G,
	CH6_LED_PIN_R, CH6_LED_PIN_G, CH7_LED_PIN_R, CH7_LED_PIN_G,
	CH8_LED_PIN_R, CH8_LED_PIN_G, GPIO_PIN_NOUSE, GPIO_PIN_LED82,
	GPIO_PIN_LED83
	#else
	RUN_LED_PIN_R, RUN_LED_PIN_G, CH1_LED_PIN_R, CH1_LED_PIN_G,
	CH2_LED_PIN_R, CH2_LED_PIN_G, CH3_LED_PIN_R, CH3_LED_PIN_G,
	CH4_LED_PIN_R, CH4_LED_PIN_G, GPIO_PIN_NOUSE, GPIO_PIN_LED82,
	GPIO_PIN_LED83
	#endif
};

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void bsp_InitLed(void);
uint8_t Led_Shine_Ctrl(uint8_t _no, uint16_t t_on, uint16_t t_off, uint16_t t_scale);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : bsp_InitLed()
**	����˵�� : ����LEDָʾ����ص�GPIO
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitLed(void)
{
	stc_gpio_init_t stcLedGpioInit;
	
	/* ���ð���Led pin������:�����������ʼ��״̬ΪSET�� */
	(void)GPIO_StructInit(&stcLedGpioInit);
	stcLedGpioInit.u16PinState = PIN_STATE_SET;
	stcLedGpioInit.u16PullUp = PIN_PU_ON;
	stcLedGpioInit.u16PinDir = PIN_DIR_OUT;
	stcLedGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcLedGpioInit.u16PinOType = PIN_OTYPE_CMOS;
	
	/* д�����Led pin������ */
    (void)GPIO_Init(GPIO_PORT_LED82, GPIO_PIN_LED82, &stcLedGpioInit);
    (void)GPIO_Init(GPIO_PORT_LED83, GPIO_PIN_LED83, &stcLedGpioInit);
	
	/* ���õư�Led pin������:�����������ʼ��״̬ΪRESET�� */
	(void)GPIO_StructInit(&stcLedGpioInit);
	stcLedGpioInit.u16PinState = PIN_STATE_RESET;
	stcLedGpioInit.u16PullUp = PIN_PU_ON;
	stcLedGpioInit.u16PinDir = PIN_DIR_OUT;
	stcLedGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcLedGpioInit.u16PinOType = PIN_OTYPE_CMOS;
	
	/* д��ư�Led pin������ */
    (void)GPIO_Init(RUN_LED_PORT_R,  RUN_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(RUN_LED_PORT_G,  RUN_LED_PIN_G,  &stcLedGpioInit);
#ifdef YCJ
    (void)GPIO_Init(CH1_LED_PORT_R,  CH1_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH1_LED_PORT_G,  CH1_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH2_LED_PORT_R,  CH2_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH2_LED_PORT_G,  CH2_LED_PIN_G,  &stcLedGpioInit);
	(void)GPIO_Init(CH3_LED_PORT_R,  CH3_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH3_LED_PORT_G,  CH3_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH4_LED_PORT_R,  CH4_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH4_LED_PORT_G,  CH4_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH5_LED_PORT_R,  CH5_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH5_LED_PORT_G,  CH5_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH6_LED_PORT_R,  CH6_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH6_LED_PORT_G,  CH6_LED_PIN_G,  &stcLedGpioInit);
	(void)GPIO_Init(CH7_LED_PORT_R,  CH7_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH7_LED_PORT_G,  CH7_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH8_LED_PORT_R,  CH8_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH8_LED_PORT_G,  CH8_LED_PIN_G,  &stcLedGpioInit);
	#else
    (void)GPIO_Init(CH1_LED_PORT_R,  CH1_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH1_LED_PORT_G,  CH1_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH2_LED_PORT_R,  CH2_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH2_LED_PORT_G,  CH2_LED_PIN_G,  &stcLedGpioInit);
	(void)GPIO_Init(CH3_LED_PORT_R,  CH3_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH3_LED_PORT_G,  CH3_LED_PIN_G,  &stcLedGpioInit);
    (void)GPIO_Init(CH4_LED_PORT_R,  CH4_LED_PIN_R,  &stcLedGpioInit);
    (void)GPIO_Init(CH4_LED_PORT_G,  CH4_LED_PIN_G,  &stcLedGpioInit);
	#endif
}

/**********************************************************************************************************
**	�� �� �� : LedTest()
**	����˵�� : ���ڲ���
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void LedTest(void)
{
	bsp_LedToggle(RUN_LED_R_NO);
}

/**********************************************************************************************************
**	�� �� �� : bsp_LedOff()
**	����˵�� : Ϩ��ָ����LEDָʾ��
**	��    �� : _no : ָʾ�����(0~12)
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_LedOff(uint8_t _no)
{
	/* Led number��Ч���ж� */
	if((_no == LED_NOUSE_NUM) || (_no > LED_MAX_NUM))
	{
		return;
	}
	
	if(_no < GPIO_PIN_NOUSE)
	{
		GPIO_SetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]);  //���õư�LEDϨ��
	}
	else
	{
		GPIO_ResetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]);    //���ð���LEDϨ��
	}
}

/**********************************************************************************************************
**	�� �� �� : bsp_LedOn()
**	����˵�� : ����ָ����LEDָʾ��
**	��    �� : _no : ָʾ�����(0~12)
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_LedOn(uint8_t _no)
{
	/* Led number��Ч���ж� */
	if((_no == LED_NOUSE_NUM) || (_no > LED_MAX_NUM))
	{
		return;
	}
	
	if(_no < GPIO_PIN_NOUSE)
	{
		GPIO_ResetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]);    //���õư�LED����
	}
	else
	{
		GPIO_SetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]);  //���ð���LED����
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
	/* Led number��Ч���ж� */
	if((_no == LED_NOUSE_NUM) || (_no > LED_MAX_NUM))
	{
		return;
	}
	
	GPIO_TogglePins(LedGpioPortTab[_no], LedGpioPinTab[_no]);
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

