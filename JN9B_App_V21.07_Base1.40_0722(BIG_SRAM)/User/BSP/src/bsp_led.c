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
#include "bsp_pca9555.h"

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
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, //

    GPIO_PORT_NOUSE, GPIO_PORT_LED82,GPIO_PORT_LED83
#else
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_LED82,GPIO_PORT_LED83
#endif
};

//GPIO PIN��ѯ��
static uint16_t LedGpioPinTab[LED_MAX_NUM+1] =
{
#ifdef YCJ
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,  GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, //

    GPIO_PIN_NOUSE, GPIO_PIN_LED82,GPIO_PIN_LED83
#else
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_LED82,GPIO_PIN_LED83
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
}

//����
void LedTest(void)
{
    bsp_LedToggle(RUN_LED_R_NO);
}

extern PcaDataType Pca9555_Data;
/*
	R-0,G-1 ���̵�; R-1,G-0,�����; R-1,G-1,��Ϩ��
	����ƣ��Ƚ��̵�λǿ��0���ٽ����λǿ��1
	���̵ƣ��Ƚ����λǿ��0���ٽ��̵�λǿ��1
*/

//C053A��ư� ���
//uint8_t LampPlate_Off[18] = {0xBF,0x7F,0xFE,0xFE,0xFD,0xFD,0xFB,0xFB,
//						0xF7,0xF7,0xEF,0xEF,0xDF,0xDF,0xBF,0xBF,0x7F,0x7F};

//C053B�� ���
uint8_t LampPlate_Off[18] = {0x40,0x80,0x01,0x01,0x02,0x02,0x04,0x04,
                             0x08,0x08,0x10,0x10,0x20,0x20,0x40,0x40,0x80,0x80
                            };

////C053A�� �����ʱ,�轫�̵�ǿ��Ϊ0;
//uint8_t LampPlate_OldOn[18] = {0x7F,0xBF,0xFE,0xFE,0xFD,0xFD,0xFB,0xFB,0xF7,0xF7,
//											0xEF,0xEF,0xDF,0xDF,0xBF,0xBF,0x7F,0x7F};	//ż��λ-�̵��õ�; ����λ-����õ�,����λ���ֲ���
////�ø�λ
//uint8_t LampPlate_NewOn[18] = {0x40,0x80,0x01,0x01,0x02,0x04,0x04,0x08,0x08,
//											0x10,0x10,0x20,0x20,0x40,0x40,0x80,0x80};//ż��λ-����ø�; ����λ-�̵��ø�,����λ���ֲ���
//C053B�� �����ʱ,�轫�̵�ǿ��Ϊ1; �����汾�ĵư�����߼��Ƿ���
uint8_t LampPlate_OldOn[18] = {0x7F,0xBF,0x01,0x01,0x02,0x04,0x04,0x08,0x08,
                               0x10,0x10,0x20,0x20,0x40,0x40,0x80,0x80
                              };	//ż��λ-�̵��õ�; ����λ-����õ�,����λ���ֲ���
//�ø�λ
uint8_t LampPlate_NewOn[18] = {0x80,0x40,0xFE,0xFE,0xFD,0xFD,0xFB,0xFB,0xF7,0xF7,
                               0xEF,0xEF,0xDF,0xDF,0xBF,0xBF,0x7F,0x7F
                              };//ż��λ-����ø�; ����λ-�̵��ø�,����λ���ֲ���

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

    if(_no == LED_82_NO || _no == LED_83_NO) //���ص�
    {
        GPIO_ResetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]);
    }
    else if(_no == RUN_LED_R_NO || _no == RUN_LED_G_NO) //run����
    {
        Pca9555_RegRead(I2C_LED, PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].ReadData); //��ȡ��ǰPCA9555����

        if(_no == RUN_LED_R_NO)        //���к����
        {
            Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].ReadData[0] | LampPlate_Off[_no];
        }
        else if(_no == RUN_LED_G_NO)        //�����̵���
        {
            Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].ReadData[0] | LampPlate_Off[_no];
        }

        Pca9555_RegWrite(I2C_LED,PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].WriteData);
    }
    else //ͨ������ָʾ��
    {
        Pca9555_RegRead(I2C_LED, PCA9555_LED_ADDR,Pca9555_Data.LED_Board[1].ReadData); //��ȡ��ǰPCA9555����

        if(_no%2 == 0)        //ż�� - ���� 2/4/6/8/10/12/14/16
        {
            Pca9555_Data.LED_Board[1].WriteData[0] = Pca9555_Data.LED_Board[1].ReadData[0] | LampPlate_Off[_no];
        }
        else if(_no%2 == 1)        //���� - ���̵� 3/5/7/9/11/13/15/17
        {
            Pca9555_Data.LED_Board[1].WriteData[1] = Pca9555_Data.LED_Board[1].ReadData[1] | LampPlate_Off[_no];
        }

        Pca9555_RegWrite(I2C_LED,PCA9555_LED_ADDR,Pca9555_Data.LED_Board[1].WriteData);
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

    if(_no == RUN_LED_R_NO || _no == RUN_LED_G_NO) //run���� 0-��,1-��
    {
        Pca9555_RegRead(I2C_LED, PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].ReadData); //��ȡ��ǰPCA9555����

#ifdef C042A_7606
        Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].ReadData[0] | LampPlate_OldOn[_no];//����ǿ���ø�
        Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].WriteData[0] & LampPlate_NewOn[_no];//���ǿ���õ�
        Pca9555_Data.LED_Board[0].WriteData[1] = Pca9555_Data.LED_Board[0].ReadData[1];
#else
        Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].ReadData[0] & LampPlate_OldOn[_no];//����ǿ���õ�
        Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].WriteData[0] | LampPlate_NewOn[_no];//���ǿ���ø�
        Pca9555_Data.LED_Board[0].WriteData[1] = Pca9555_Data.LED_Board[0].ReadData[1];
#endif
        Pca9555_RegWrite(I2C_LED,PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].WriteData);	//run��
    }
    else if(_no == LED_82_NO || _no == LED_83_NO)
    {
        GPIO_SetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]); //���ص�
    }
    else //ͨ������ָʾ��
    {
        Pca9555_RegRead(I2C_LED, PCA9555_LED_ADDR,Pca9555_Data.LED_Board[1].ReadData); //��ȡ��ǰPCA9555����

        if(_no%2 == 0)
        {
#ifdef C042A_7606
            Pca9555_Data.LED_Board[1].WriteData[0] = Pca9555_Data.LED_Board[1].ReadData[0] & LampPlate_NewOn[_no];
            Pca9555_Data.LED_Board[1].WriteData[1] = Pca9555_Data.LED_Board[1].ReadData[1] | LampPlate_OldOn[_no];
#else
            Pca9555_Data.LED_Board[1].WriteData[0] = Pca9555_Data.LED_Board[1].ReadData[0] | LampPlate_NewOn[_no];
            Pca9555_Data.LED_Board[1].WriteData[1] = Pca9555_Data.LED_Board[1].ReadData[1] & LampPlate_OldOn[_no];
#endif
        }
        else
        {
#ifdef C042A_7606
            Pca9555_Data.LED_Board[1].WriteData[1] = Pca9555_Data.LED_Board[1].ReadData[1] & LampPlate_NewOn[_no];
            Pca9555_Data.LED_Board[1].WriteData[0] = Pca9555_Data.LED_Board[1].ReadData[0] | LampPlate_OldOn[_no];
#else
            Pca9555_Data.LED_Board[1].WriteData[1] = Pca9555_Data.LED_Board[1].ReadData[1] | LampPlate_NewOn[_no];
            Pca9555_Data.LED_Board[1].WriteData[0] = Pca9555_Data.LED_Board[1].ReadData[0] & LampPlate_OldOn[_no];
#endif
        }

        Pca9555_RegWrite(I2C_LED,PCA9555_LED_ADDR,Pca9555_Data.LED_Board[1].WriteData);
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
		static uint16_t g_usLedCtrlCnt[18] = {0};   //8ͨ������һ��9����,ÿ����2����ɫLEDһ��18��
	 
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

