/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_AD7606.c
** Author      : wangliang
** Date        : 2021-11-05
** Description : bsp_dac80508 source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
AD7606_Sel whoIsConV = NoBody;
AD7606_Object AD7606_A0 = {AD7606_Iout,{0},1,{0},{0}}; //AD7606_Vsense
AD7606_Object AD7606_A1 = {AD7606_Vsense,{0},1,{0},{0}}; //

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
static void AD7606_Busy0_EXINT_IrqCallback(void);
static void AD7606_Busy1_EXINT_IrqCallback(void);

/**********************************************************************************************************
**	�� �� �� : AD7606_SpiInit()
**	����˵�� : AD7606 SPI���ó�ʼ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void AD7606_SpiInit(void)
{
    stc_gpio_init_t stcGpioCfg;
    stc_spi_init_t stcSpiInit;

    /* Configure peripheral clock */
    PWC_Fcg1PeriphClockCmd(AD7606_UNIT_CLOCK, Enable);

    /* Port configurate, High driving capacity for output pin. */
    
    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(AD7606_SCK_PORT,  AD7606_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(AD7606_MOSI_PORT, AD7606_MOSI_PIN, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(AD7606_MISO_PORT, AD7606_MISO_PIN, &stcGpioCfg);

    /* Configure SPI Port function for master */
    GPIO_SetFunc(AD7606_SCK_PORT,  AD7606_SCK_PIN,  AD7606_SCK_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(AD7606_MISO_PORT, AD7606_MISO_PIN, AD7606_MISO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(AD7606_MOSI_PORT, AD7606_MOSI_PIN, AD7606_MOSI_FUNC, PIN_SUBFUNC_DISABLE);

    /* SPI De-initialize */
    SPI_DeInit(AD7606_SPI_UNIT);
    /* Clear initialize structure */
    (void)SPI_StructInit(&stcSpiInit);

    stcSpiInit.u32WireMode          = SPI_WIRE_3;              //3��SPI
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;         //ȫ˫��
    stcSpiInit.u32MasterSlave       = SPI_MASTER;              //SPI����
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;   //SPIͨ�Ź���ر�
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;       //���ó���̽��
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;      //��żУ�����
    stcSpiInit.u32SpiMode           = SPI_MODE_3;              //ʹ��ģʽ3ʱ��
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_PCLK1_DIV8;      //������=PCLK1/8
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_16BIT;     //16λ���ݴ���ģʽ
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;           //MSB���ȴ���
    (void)SPI_Init(AD7606_SPI_UNIT, &stcSpiInit);

    /* Command SPI function */
    SPI_FunctionCmd(AD7606_SPI_UNIT, Enable);
}

/**********************************************************************************************************
**  �� �� �� : AD7606Reset()
**  ����˵�� : AD7606Ӳ����λ
**  ��    �� : OB: ��������ṹ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void AD7606Reset(AD7606_Object * OB)
{
    if(OB->Type==AD7606_Vsense)
    {
        /*��λA0*/
        A0_REST_0();
        bsp_DelayUS(1);
        A0_REST_1();
        bsp_DelayUS(1);
        A0_REST_0();
        bsp_DelayUS(1);
    } 
		else if(OB->Type==AD7606_Iout)
    {
        /*��λA1*/
        A1_REST_0();
        bsp_DelayUS(1);
        A1_REST_1();
        bsp_DelayUS(1);
        A1_REST_0();
        bsp_DelayUS(1);
    }
}

/**********************************************************************************************************
**  �� �� �� : bsp_AD7606_init()
**  ����˵�� : 2�� AD7606 spi ��ʼ��
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void bsp_AD7606_init(void)
{
    stc_gpio_init_t stcGpioInit;
    stc_exint_init_t stcExintInit;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* ��������źţ�����CS��REST��CONV */
    (void)GPIO_StructInit(&stcGpioInit);       //�ָ�defaultֵ
    stcGpioInit.u16PinState = PIN_STATE_SET;   //CS CONV���ó�ʼ��ƽ=1
    stcGpioInit.u16PullUp = PIN_PU_ON;         //������
    stcGpioInit.u16PinDir = PIN_DIR_OUT;       //���
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;      //����������
    stcGpioInit.u16PinOType = PIN_OTYPE_CMOS;  //��©�ر�

    (void)GPIO_Init(A0_Cs_Port,  A0_Cs_Pin,  &stcGpioInit); //��ʼ��7606�����sense��ѹ�ɼ���CS�ź�
    (void)GPIO_Init(A1_Cs_Port,  A1_Cs_Pin,  &stcGpioInit); //��ʼ��7606��ģ������ɼ���CS�ź�

    GPIO_SetPins(A0_Cs_Port,A0_Cs_Pin);         //Ƭѡ�����
    GPIO_SetPins(A1_Cs_Port,A1_Cs_Pin);         //Ƭѡ�����

    (void)GPIO_Init(A0_Cov_Port,  A0_Cov_Pin,  &stcGpioInit); //��ʼ��7606�����sense��ѹ�ɼ���ת���ź�
    (void)GPIO_Init(A1_Cov_Port,  A1_Cov_Pin,  &stcGpioInit); //��ʼ��7606��ģ������ɼ���ת���ź�

    GPIO_SetPins(A0_Cov_Port,A0_Cov_Pin);       //CONV�����
    GPIO_SetPins(A1_Cov_Port,A1_Cov_Pin);       //CONV�����

    stcGpioInit.u16PinState = PIN_STATE_RESET;   //Reset ���ó�ʼ��ƽ=0
    (void)GPIO_Init(A0_Reset_Port,  A0_Reset_Pin,  &stcGpioInit); //��ʼ��7606�����sense��ѹ�ɼ�����λ�ź�
    (void)GPIO_Init(A1_Reset_Port,  A1_Reset_Pin,  &stcGpioInit); //��ʼ��7606��ģ������ɼ�����λ�ź�

    GPIO_ResetPins(A0_Reset_Port,A0_Reset_Pin);   //Rest����ͣ���������Ч
    GPIO_ResetPins(A1_Reset_Port,A0_Reset_Pin);   //Rest����ͣ���������Ч

    /* ���������źţ�����BUSY */
    (void)GPIO_StructInit(&stcGpioInit);       //�ָ�defaultֵ
    stcGpioInit.u16ExInt = PIN_EXINT_ON;       //ʹ��IO�ⲿ�жϹ���
    stcGpioInit.u16PullUp = PIN_PU_OFF;        //Ĭ��������
    (void)GPIO_Init(AD7606_Busy_Port,  AD7606_Busy_Pin,  &stcGpioInit); //��ʼ��7606�����ã�Busy�ź�

    /* �ⲿ�ж����� */
    (void)EXINT_StructInit(&stcExintInit);
    stcExintInit.u32ExIntCh = AD7606_EXINT_CH;         //ѡ�񴥷�ͨ��
    stcExintInit.u32ExIntLvl = EXINT_TRIGGER_FALLING;  //�½��ش���
    (void)EXINT_Init(&stcExintInit);

    /* IRQע�� */
    stcIrqSignConfig.enIntSrc = AD7606_INT_SRC;        //ѡ�񴥷�Դ
    stcIrqSignConfig.enIRQn   = AD7606_IRQn;           //ѡ���жϺ�
    stcIrqSignConfig.pfnCallback = &AD7606_Busy1_EXINT_IrqCallback; //ע���жϻص�����
    (void)INTC_IrqSignIn(&stcIrqSignConfig);

    /* �ж����������� */
    NVIC_ClearPendingIRQ(AD7606_IRQn);                  //���жϱ�־
    NVIC_SetPriority(AD7606_IRQn, DDL_IRQ_PRIORITY_14); //�������ȼ�
    NVIC_DisableIRQ(AD7606_IRQn);                       //�ж�Ĭ�Ϲر�

/***************************************************************************************/
    /* ���������źţ�����BUSY0 */
    (void)GPIO_StructInit(&stcGpioInit);       //�ָ�defaultֵ
    stcGpioInit.u16ExInt = PIN_EXINT_ON;       //ʹ��IO�ⲿ�жϹ���
    stcGpioInit.u16PullUp = PIN_PU_OFF;        //Ĭ��������
    (void)GPIO_Init(AD7606_Busy0_Port,  AD7606_Busy0_Pin,  &stcGpioInit); //��ʼ��7606�����ã�Busy�ź�

    /* �ⲿ�ж����� */
    (void)EXINT_StructInit(&stcExintInit);
    stcExintInit.u32ExIntCh = AD7606_Busy0_EXINT_CH;   //ѡ�񴥷�ͨ��
    stcExintInit.u32ExIntLvl = EXINT_TRIGGER_FALLING;  //�½��ش���
    (void)EXINT_Init(&stcExintInit);

    /* IRQע�� */
    stcIrqSignConfig.enIntSrc = AD7606_Busy0_INT_SRC;        //ѡ�񴥷�Դ
    stcIrqSignConfig.enIRQn   = AD7606_Busy0_IRQn;           //ѡ���жϺ�
    stcIrqSignConfig.pfnCallback = &AD7606_Busy0_EXINT_IrqCallback; //ע���жϻص�����
    (void)INTC_IrqSignIn(&stcIrqSignConfig);

    /* �ж����������� */
    NVIC_ClearPendingIRQ(AD7606_Busy0_IRQn);                  //���жϱ�־
    NVIC_SetPriority(AD7606_Busy0_IRQn, DDL_IRQ_PRIORITY_14); //�������ȼ�
    NVIC_DisableIRQ(AD7606_Busy0_IRQn);                       //�ж�Ĭ�Ϲر�
/***************************************************************************************/

    /*��ʼ��SPI�ӿ�*/
    AD7606_SpiInit();
    
    NVIC_EnableIRQ(AD7606_IRQn);                        //�����ж�
		NVIC_EnableIRQ(AD7606_Busy0_IRQn);                        //�����ж�
}

/**********************************************************************************************************
**	�� �� �� : AD7606_ReadData()
**	����˵�� : AD7606 ��ͨ�����β������ݵ�����ṹ��
**	��    �� : OB:��������
**	�� �� ֵ :
**********************************************************************************************************/
uint32_t AD7606_ReadData(AD7606_Object * OB)
{
    uint8_t i = 0;

    uint32_t ret = 0;

    if(OB->Type==AD7606_Vsense)
    {
        /* �����sense��ѹ�ɼ�ADCֵ */
        A0_CS_0();
        for(i=0; i<AD7606_CH; i++)
        {
            ret = SPI_Receive(AD7606_SPI_UNIT,  &OB->ADC[i], 1);   //��SPI����
        }
        A0_CS_1();
    }
		else if(OB->Type==AD7606_Iout)
    {
        /* ��ģ������ɼ�ADCֵ */
        A1_CS_0();
        for(i=0; i<AD7606_CH; i++)
        {
            ret = SPI_Receive(AD7606_SPI_UNIT,  &OB->ADC[i], 1);   //��SPI����
        }
        A1_CS_1();
    }
    return ret;
}

/**********************************************************************************************************
**	�� �� �� : AD7606_StartConV()
**	����˵�� : AD7606 ����ADת��
**	��    �� : OB:��������
**	�� �� ֵ :
**********************************************************************************************************/
void AD7606_StartConV(AD7606_Object * OB)
{
    whoIsConV = OB->Type;  //����ת��ȫ�ֱ�־
    
    if(OB->Type==AD7606_Vsense)
    {
        /* ��sense��ѹ�ɼ�ADC����ת���ź� */        
        A0_CNV_0();
        bsp_DelayUS(1);
        A0_CNV_1();
        bsp_DelayUS(1);
    } 
		else if(OB->Type==AD7606_Iout)
    {
        /* ��ģ������ɼ�ADC����ת���ź� */
        A1_CNV_0();
        bsp_DelayUS(1);
        A1_CNV_1();
        bsp_DelayUS(1);
    }
}

/**********************************************************************************************************
**  �� �� �� : AD7606_Process()
**  ����˵�� : AD7606���ݴ�����
**  ��    �� : OB:����   AVG_num������ƽ������ 
**  �� �� ֵ : ��
**********************************************************************************************************/
void AD7606_Process(AD7606_Object * OB , uint16_t AVG_num)
{
    static uint16_t cnt = 0;

		if(cnt< AVG_num-1)   //δִ�е�Լ����������
    {
        /*���ͨ��������ʷ�ۼ�*/
        for(uint16_t i=0; i<AD7606_CH ; i++)
        {
            OB->SUM_ADC[i]+=OB->ADC[i];
        }
        cnt++;
    } 
		else	//ִ�е�Լ����������
    {
        /*���ͨ��������ʷ�ۼ�*/
        for(uint16_t i=0; i<AD7606_CH ; i++)
        {
            OB->SUM_ADC[i]+=OB->ADC[i];
        }
        /*���ͨ�������ƽ��*/
        for(uint16_t i=0; i<AD7606_CH ; i++)
        {
            OB->AVG_ADC[i]= OB->SUM_ADC[i]/AVG_num;
        }
        /*���ͨ�������ʷ�ۼ�*/
        for(uint16_t i=0; i<AD7606_CH ; i++)
        {
            OB->SUM_ADC[i]=0;
        }        
        OB->Sample_NUM = AVG_num;
        cnt=0;  //��ռ���
    }
}

/**********************************************************************************************************
**  �� �� �� : AD7606_Busy0_EXINT_IrqCallback()
**  ����˵�� : AD7606 Busy0 �жϻص�����
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
static void AD7606_Busy0_EXINT_IrqCallback(void)
{
	if(Set == EXINT_GetExIntSrc(AD7606_Busy0_EXINT_CH))	//PA15
	{
		EXINT_ClrExIntSrc(AD7606_Busy0_EXINT_CH);                  //���жϱ�־

		/* Disable interrupt */
//		NVIC_DisableIRQ(AD7606_Busy0_IRQn);                        //�ر��жϣ���ֹǶ��

		AD7606_ReadData(&AD7606_A0); //AD7606_A0	��ѹ����
		AD7606_Process(&AD7606_A0, SAMP_CNT);                        //�����ϴ�ת����ɵ�����

		whoIsConV=NoBody;
		/* Enable interrupt */
//		NVIC_EnableIRQ(AD7606_Busy0_IRQn);
	}
	else
	{
			//do nothing;
	}
}

/**********************************************************************************************************
**  �� �� �� : AD7606_EXINT_IrqCallback()
**  ����˵�� : AD7606 �жϻص�����
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
static void AD7606_Busy1_EXINT_IrqCallback(void)
{
	if(Set == EXINT_GetExIntSrc(AD7606_EXINT_CH))
	{
		EXINT_ClrExIntSrc(AD7606_EXINT_CH);                  //���жϱ�־

		/* Disable interrupt */
//		NVIC_DisableIRQ(AD7606_IRQn);                        //�ر��жϣ���ֹǶ��

		AD7606_ReadData(&AD7606_A1);		//��������
		AD7606_Process(&AD7606_A1, SAMP_CNT);                        //�����ϴ�ת����ɵ�����       

		whoIsConV=NoBody;
		/* Enable interrupt */
//		NVIC_EnableIRQ(AD7606_IRQn);
	}
	else
	{
			//do nothing;
	}
}

