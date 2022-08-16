/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_dac.c
** Author      : wang liang
** Date        : 2022-01-18
** Description : bsp_can source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                          �궨����
**********************************************************************************************************/



/**********************************************************************************************************
**                                          ȫ�ֱ�����
**********************************************************************************************************/
st_dac_handle_t gstDACHandle[DAC_Unit_Max] = {0};

st_dac_handle_t* Bovp;      //�����洢DAC��ʼ������ֵ

/**********************************************************************************************************
**	�� �� �� : DAC_PClkEnable()
**	����˵�� : DACʱ��ʹ��
**	��    �� : enUnit��    DAC_Unit1,
                           DAC_Unit2,
                           DAC_Unit_Max,
**	�� �� ֵ : ��
**********************************************************************************************************/
static void DAC_PClkEnable(en_dac_unit enUnit)
{
    uint32_t u32PClk;
    switch(enUnit)
    {
    case DAC_Unit1:
        u32PClk = PWC_FCG3_DAC1;
        break;
    case DAC_Unit2:
        u32PClk = PWC_FCG3_DAC2;
        break;
    default:
        u32PClk = PWC_FCG3_DAC1 | PWC_FCG3_DAC2;
        break;
    }
    /* Enable DAC peripheral clock. */
    PWC_Fcg3PeriphClockCmd(u32PClk, Enable);
}


/**********************************************************************************************************
**	�� �� �� : DAC_SingleConversionInit()
**	����˵�� : DA��ͨ��ת����ʼ��
**	��    �� : enUnit��    DAC_Unit1,
                           DAC_Unit2,
                           DAC_Unit_Max,
**	�� �� ֵ :  A pointer of DAC handler
**********************************************************************************************************/
static st_dac_handle_t* DAC_SingleConversionInit(en_dac_unit enUnit)
{
    uint8_t u8Port;
    uint16_t u16Pin;
    st_dac_handle_t* pDac;

    if(enUnit == DAC_Unit1)
    {
        pDac = &gstDACHandle[DAC_Unit1];
        pDac->pUnit = M4_DAC1;
    }
    else
    {
        pDac = &gstDACHandle[DAC_Unit2];
        pDac->pUnit = M4_DAC2;
    }
    DAC_PClkEnable(enUnit);     //ʹ��ʱ��

    pDac->enCvtType = E_Dac_Single;
#if (SINGLE_WAVE_DAC_CHN == DAC_CHN1)
    pDac->u16Ch = DAC_CH_1;
#else
    pDac->u16Ch = DAC_CH_2;
#endif

    /* Init DAC by default value: source from data register and output enabled*/
    DAC_DeInit(pDac->pUnit);
    stc_dac_init_t stInit;
    (void)DAC_StructInit(&stInit);
    (void)DAC_Init(pDac->pUnit, pDac->u16Ch, &stInit);   //DAC �����ʼ��
#if (DAC_DATA_ALIGN == DAC_DATA_ALIGN_12b_L)
    DAC_DataRegAlignConfig(pDac->pUnit,DAC_DATA_ALIGN_L);
#else
    DAC_DataRegAlignConfig(pDac->pUnit,DAC_DATA_ALIGN_R);
#endif

    /* Set DAC pin attribute to analog */
    if(enUnit == DAC_Unit1)
    {
        u8Port = DAC_UNIT1_PORT;
#if (SINGLE_WAVE_DAC_CHN == DAC_CHN1)
        u16Pin = DAC_UNIT1_CHN1_PIN;
#else
        u16Pin = DAC_UNIT1_CHN2_PIN;
#endif
    }
    else
    {
        u8Port = DAC_UNIT2_PORT;
#if (SINGLE_WAVE_DAC_CHN == DAC_CHN1)
        u16Pin = DAC_UNIT2_CHN1_PIN;
#else
        u16Pin = DAC_UNIT2_CHN2_PIN;
#endif
    }
    stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(u8Port, u16Pin, &stcGpioInit);        //DAC �ܽų�ʼ��

#ifdef SUPPORT_ADP
    /* Set ADC first */
    /* Enable DAC peripheral clock. */
    PWC_Fcg3PeriphClockCmd(PWC_FCG3_ADC1|PWC_FCG3_ADC2|PWC_FCG3_ADC3, Enable);
    if(M4_ADC1->STR == 0U)
    {
        if(M4_ADC2->STR == 0U)
        {
            if(M4_ADC3->STR == 0U)
            {
                DAC_ADCPrioConfig(pDac->pUnit, DAC_ADP_SELECT_ALL);
                DAC_ADCPrioCmd(pDac->pUnit, Enable);
            }
        }
    }
#endif
    return pDac;
}


/**
 * @brief    Start single DAC conversions
 * @param    [in] pDac       A pointer of DAC handler
 * @retval   None
 */

/**********************************************************************************************************
**	�� �� �� : DAC_StartSingleConversion()
**	����˵�� : DA��ͨ��ת������
**	��    �� : [in] pDac       A pointer of DAC handler
**	�� �� ֵ :  None
**********************************************************************************************************/
static void DAC_StartSingleConversion(const st_dac_handle_t* pDac)
{
    /* Enalbe AMP */
#ifdef SUPPORT_AMP
    (void)DAC_AMPCmd(pDac->pUnit,pDac->u16Ch,Enable);
#endif

    (void)DAC_Start(pDac->pUnit,pDac->u16Ch);

#ifdef SUPPORT_AMP
    /* delay 3us before setting data*/
    DDL_DelayMS(1U);
#endif
}


/**********************************************************************************************************
**	�� �� �� : DAC_SetSingleConversionData()
**	����˵�� : ����DA��ͨ��ת����
**	��    �� : [in] pDac       A pointer of DAC handler
               [in] pDataTable The data table to be converted
               [in] u32count   Number of data table items
**	�� �� ֵ :  None
**********************************************************************************************************/
__STATIC_INLINE void DAC_SetSingleConversionData(const st_dac_handle_t* pDac, uint32_t const pDataTable[],uint32_t u32count)
{
    for(uint32_t i = 0U; i < u32count; i++)
    {
#ifdef SUPPORT_ADP
        uint32_t u32TryCount = 100U;
        while(u32TryCount != 0U)
        {
            u32TryCount--;
            if(OperationInProgress != pfSingleWaveConvState(pDac->pUnit))
            {
                break;
            }
        }
#endif
        pfSingleWaveSetData(pDac->pUnit,(uint16_t)pDataTable[i]);
    }
}



/**********************************************************************************************************
**	�� �� �� : DAC_StopConversion()
**	����˵�� : ֹͣDACת��
**	��    �� : [in] pDac       A pointer of DAC handler
**	�� �� ֵ :  None
**********************************************************************************************************/
void DAC_StopConversion(const st_dac_handle_t* pDac)
{
    if(NULL == pDac)
    {
        DAC_DeInit(M4_DAC1);
        DAC_DeInit(M4_DAC2);
    }
    else if(pDac->enCvtType != E_Dac_Dual)
    {
        (void)DAC_Stop(pDac->pUnit,pDac->u16Ch);
    }
    else
    {
        DAC_DualChannelStop(pDac->pUnit);
    }
}

/**********************************************************************************************************
**	�� �� �� : bsp_bovpInit()
**	����˵�� : bovp�źŹ��ܳ�ʼ��
**	��    �� :
**	�� �� ֵ :  None
**********************************************************************************************************/
st_dac_handle_t* bsp_bovpInit(void)
{
    st_dac_handle_t* pDac;
    pDac = DAC_SingleConversionInit(DAC_Unit1);
    return pDac;
}

/**********************************************************************************************************
**	�� �� �� : AD5245_Vpro_Set()
**	����˵�� : ���ñ�����ѹ
**	��    �� : pDac DAC ��ʼ�����ؾ�� Vbovp ������ѹ ����λV
**	�� �� ֵ :  None
**********************************************************************************************************/
void AD5245_Vpro_Set(st_dac_handle_t* pDac, float Vbovp)
{
    DAC_StartSingleConversion(pDac);

    //������ѹת����ʽ Vset /2.5= Vbovp/10
    float Vset = 0;             //DA �����ѹ
    uint32_t Vset_Date = 0;   //Vset��Ӧ��ת����ֵ������12λ�����

//    Vset = Vbovp*0.41727f;  //��ϵ����C042B����ĳ� 2022.04.03 ���� 66�װ汾
		Vset = Vbovp*0.34804f;		//2022.04.03 630����֤ʹ��

    if(Vset > 2.5f)
        Vset = 2.5f;

    Vset_Date = (uint32_t)((float)(Vset*0x0FFF/2.5f)); //����ȡ��
    Vset_Date = Vset_Date<<4;           //����4λ����

    DAC_SetSingleConversionData(pDac, &Vset_Date , 1);   //����DA �����ѹ
}

