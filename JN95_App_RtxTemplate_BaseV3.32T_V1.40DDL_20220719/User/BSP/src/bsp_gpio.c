/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_gpio.c
** Author      : Huang Cheng
** Date        : 2021-11-04
** Description : bsp_gpio source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_gpio.h"
#include "bsp_dwt.h"
#include "global.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t W5500_IP_ADDRESS[4] = {192, 168, 0, 0};   //IP��ַ����

/* Loop�˿����Ų�ѯ�� */
LoopPortPin_TypeDef LoopPortPinTab[8] = 
{
	{GPIO_PORT_LOOP1, GPIO_PIN_LOOP1}, {GPIO_PORT_LOOP2, GPIO_PIN_LOOP2},
	{GPIO_PORT_LOOP3, GPIO_PIN_LOOP3}, {GPIO_PORT_LOOP4, GPIO_PIN_LOOP4},
	{GPIO_PORT_LOOP5, GPIO_PIN_LOOP5}, {GPIO_PORT_LOOP6, GPIO_PIN_LOOP6},
	{GPIO_PORT_LOOP7, GPIO_PIN_LOOP7}, {GPIO_PORT_LOOP8, GPIO_PIN_LOOP8},
};

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : bsp_M_Loop_InitGPIO
**	����˵�� : Loop gpio��ʼ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_M_Loop_InitGPIO(void)
{
	stc_gpio_init_t stcLoopGpioInit;
	
	/* Set the JTDI to Normal GPIO for Loop2 */
	GPIO_SetDebugPort(GPIO_PIN_TDI, Disable);
	
	/* ����Loop pin������:�����������ʼ��״̬ΪRESET�� */
	(void)GPIO_StructInit(&stcLoopGpioInit);
	stcLoopGpioInit.u16PinState = PIN_STATE_RESET;
	stcLoopGpioInit.u16PinDir = PIN_DIR_OUT;
	stcLoopGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcLoopGpioInit.u16PinOType = PIN_OTYPE_CMOS;
	
	/* д��Loop pin������ */
    (void)GPIO_Init(GPIO_PORT_LOOP1, GPIO_PIN_LOOP1, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP2, GPIO_PIN_LOOP2, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP3, GPIO_PIN_LOOP3, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP4, GPIO_PIN_LOOP4, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP5, GPIO_PIN_LOOP5, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP6, GPIO_PIN_LOOP6, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP7, GPIO_PIN_LOOP7, &stcLoopGpioInit);
    (void)GPIO_Init(GPIO_PORT_LOOP8, GPIO_PIN_LOOP8, &stcLoopGpioInit);
}

/**********************************************************************************************************
**	�� �� �� : Enable_Mloop
**	����˵�� : Mloopʹ�ܺ���
**	��    �� : _ch - ���ű��
**	�� �� ֵ : ��
**********************************************************************************************************/
void Enable_Mloop(uint8_t _ch)
{
	if(_ch >= 8)
	{
		return;
	}
	
	GPIO_SetPins(LoopPortPinTab[_ch].GpioPort, LoopPortPinTab[_ch].GpioPin);
}

/**********************************************************************************************************
**	�� �� �� : Disable_Mloop
**	����˵�� : Mloop��ֹ����
**	��    �� : _ch - ���ű��
**	�� �� ֵ : ��
**********************************************************************************************************/
void Disable_Mloop(uint8_t _ch)
{
	if(_ch >= 8)
	{
		return;
	}
	
	GPIO_ResetPins(LoopPortPinTab[_ch].GpioPort, LoopPortPinTab[_ch].GpioPin);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_FR_ADDR_InitGPIO
*	����˵��: ���ò��뿪�����õ�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_FR_ADDR_InitGPIO(void)
{
	stc_gpio_init_t stcFrAddrGpioInit;
	
	/* ����FR_ADDR pin������:�������� */
	(void)GPIO_StructInit(&stcFrAddrGpioInit);
	stcFrAddrGpioInit.u16PinDir = PIN_DIR_IN;
	stcFrAddrGpioInit.u16PullUp = PIN_PU_OFF;

	/* д��FR_ADDR pin������ */
	(void)GPIO_Init(GPIO_PORT_FR_ADDR1, GPIO_PIN_FR_ADDR1, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR2, GPIO_PIN_FR_ADDR2, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR3, GPIO_PIN_FR_ADDR3, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR4, GPIO_PIN_FR_ADDR4, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR5, GPIO_PIN_FR_ADDR5, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR6, GPIO_PIN_FR_ADDR6, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR7, GPIO_PIN_FR_ADDR7, &stcFrAddrGpioInit);
	(void)GPIO_Init(GPIO_PORT_FR_ADDR8, GPIO_PIN_FR_ADDR8, &stcFrAddrGpioInit);
	
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_HWDG_InitGPIO
*	����˵��: Ӳ�����Ź�IO��ʼ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_HWDG_InitGPIO(void)
{
    stc_gpio_init_t stcHwdgGpioInit;
	
	/* ����Hwdg pin������:�����������ʼ��״̬ΪRESET�� */
	(void)GPIO_StructInit(&stcHwdgGpioInit);
	stcHwdgGpioInit.u16PinState = PIN_STATE_RESET;
	stcHwdgGpioInit.u16PinDir = PIN_DIR_OUT;
	stcHwdgGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcHwdgGpioInit.u16PinOType = PIN_OTYPE_CMOS;
	
	/* д��Hwdg pin������ */
    (void)GPIO_Init(GPIO_PORT_HWDG, GPIO_PIN_HWDG, &stcHwdgGpioInit);
}

/*
*********************************************************************************************************
*	�� �� ��: Feed_WDG
*	����˵��: ι��������Ӳ�����Ź�&�������Ź���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Feed_WDG(void)
{
	#if 1
//    HAL_IWDG_Refresh(&hiwdg);    //ι�����������Ź���
	SWDT_Feed();
	
	//ι����Ӳ�����Ź���
    if(feed_hardware_watchdog_enable)
    {
        GPIO_TogglePins(GPIO_PORT_HWDG, GPIO_PIN_HWDG); 
    }
	#endif
}

/**********************************************************************************************************
*	�� �� ��: IP_Scanning
*	����˵��: ��ȡϵͳ��IP�趨
*	��    ��: ��
*	�� �� ֵ: ��
**********************************************************************************************************/
void IP_Scanning(void)
{
	uint8_t i = 0;
	uint8_t AddrTmp = 0;
	
	/* GPIO PORT��ѯ�� */
	uint8_t GpioPortAddr[8] = 
	{
		GPIO_PORT_FR_ADDR1, GPIO_PORT_FR_ADDR2, GPIO_PORT_FR_ADDR3, GPIO_PORT_FR_ADDR4,
		GPIO_PORT_FR_ADDR5, GPIO_PORT_FR_ADDR6, GPIO_PORT_FR_ADDR7, GPIO_PORT_FR_ADDR8
	};
	
	/* GPIO PIN��ѯ�� */
	uint16_t GpioPinAddr[8] = 
	{
		GPIO_PIN_FR_ADDR1, GPIO_PIN_FR_ADDR2, GPIO_PIN_FR_ADDR3, GPIO_PIN_FR_ADDR4,
		GPIO_PIN_FR_ADDR5, GPIO_PIN_FR_ADDR6, GPIO_PIN_FR_ADDR7, GPIO_PIN_FR_ADDR8
	};
	
	/* ��ȡ���뿪��1-3״̬ */
	for(i = 0; i < IP_ADDR3_END; i++)
	{
		if(0x00 == GPIO_ReadInputPins(GpioPortAddr[i], GpioPinAddr[i]))
		{
			bsp_DelayMS(10);
			if(0x00 == GPIO_ReadInputPins(GpioPortAddr[i], GpioPinAddr[i]))
			{
				AddrTmp += 0x01 << (IP_ADDR3_END-i-1);
			}
		}
	}
	W5500_IP_ADDRESS[2] = AddrTmp + 1;
	AddrTmp = 0x00;
	
	/* ��ȡ���뿪��4-8״̬ */
	for(i = IP_ADDR3_END; i < IP_ADDR4_END; i++)
	{
		if(0x00 == GPIO_ReadInputPins(GpioPortAddr[i], GpioPinAddr[i]))
		{
			bsp_DelayMS(10);
			if(0x00 == GPIO_ReadInputPins(GpioPortAddr[i], GpioPinAddr[i]))
			{
				AddrTmp += 0x01 << (IP_ADDR4_END-i-1);
			}
		}
	}
	W5500_IP_ADDRESS[3] = AddrTmp + 11;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
