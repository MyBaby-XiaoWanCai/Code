/*
*********************************************************************************************************
*
*	ģ������ : user_adc
*	�ļ����� : user_adc.c
*	��    �� : V1.0
*	˵    �� :
*
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0    2020-03-10   ZYL      ��ʽ����
*********************************************************************************************************
*/

#include "main.h"
uint16_t uhADCxConvertedValue12[DMA_CH_samp_SUM]={0};
uint16_t uhADCxConvertedValue34[DMA_CH_samp_SUM]={0};
uint16_t uhADCxConvertedValue56[DMA_CH_samp_SUM]={0};
uint16_t uhADCxConvertedValue78[DMA_CH_samp_SUM]={0};
// uint16_t AD_Amb=0;
// uint16_t AD_Aux=0;
// uint16_t AD_Bus1=0;
// uint16_t AD_Bus2=0;
// uint16_t ADCx_Vmod[8]={0};//�ֱ����Vmod1~Vmod7
// uint16_t ADCx_Temp[8]={0};//�ֱ����Temp1~Temp7
float AD_Amb=0;
float AD_Aux=0;
float AD_Bus1=0;
float AD_Bus2=0;
float ADCx_Vmod[8]={0};//�ֱ����Vmod1~Vmod7
float ADCx_Temp[8]={0};//�ֱ����Temp1~Temp7

/*ADCͨ������˳������Ϊ
AD_Amb   :IN3    CH0
AD_Aux   :IN13   CH1
AD_Bus1  :IN9    CH2
AD_Bus2  :IN14   CH3
AD_Vmod17:IN15   CH4
AD_Vmod28:IN4    CH5
AD_Temp17:IN10   CH6
AD_Temp28:IN12   CH7
*/

/* ����GPIO�˿� */
#define SNA_PORT		      GPIOH
#define SNA_PIN		          GPIO_PIN_2

#define SNB_PORT		      GPIOH
#define SNB_PIN		          GPIO_PIN_3

/* ���������0����1�ĺ� */
#define SNA_0()		HAL_GPIO_WritePin(SNA_PORT, SNA_PIN, GPIO_PIN_RESET);
#define SNA_1()		HAL_GPIO_WritePin(SNA_PORT, SNA_PIN, GPIO_PIN_SET);
#define SNB_0()		HAL_GPIO_WritePin(SNB_PORT, SNB_PIN, GPIO_PIN_RESET);
#define SNB_1()		HAL_GPIO_WritePin(SNB_PORT, SNB_PIN, GPIO_PIN_SET);
/*
*********************************************************************************************************
*	�� �� ��: ADC_Switch_ConfigGPIO
*	����˵��:
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ADC_Switch_ConfigGPIO(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    /* ��GPIOʱ�� */
    __HAL_RCC_GPIOH_CLK_ENABLE(); /* ��GPIOHʱ�� */

    /* �����������IO */
    GPIO_Initure.Pin = SNA_PIN; /*������� */
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* ��Ϊ����ģʽ */
    GPIO_Initure.Pull = GPIO_NOPULL;         /* ���������費ʹ�� */
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO������ٶ� */
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);     //GPIOH

        /* �����������IO */
    GPIO_Initure.Pin = SNB_PIN; /*������� */
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; /* ��Ϊ����ģʽ */
    GPIO_Initure.Pull = GPIO_NOPULL;         /* ���������費ʹ�� */
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    /* IO������ٶ� */
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);     //GPIOH

    SNA_1();
    SNB_1();
    if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue78,DMA_CH_samp_SUM)!= HAL_OK)
    {Error_Handler();}
}
/*
*********************************************************************************************************
*	�� �� ��: ADC_Switch_Channel
*	����˵��: �л�ͨ��DMA��ȡ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ADC_Switch_Channel(uint8_t SW)
{
    uint8_t i;
    uint8_t j;
    float ADCxCHxValue_temp[8]={0};

    if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
    {Error_Handler();}

    switch(SW)
    {
        case 0:
            for (i = 0; i < DMA_CH_samp; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue78[j+i*8];
                }
            }
            ADCx_Vmod[6]=ADCxCHxValue_temp[4]/DMA_CH_samp;
            ADCx_Vmod[7]=ADCxCHxValue_temp[5]/DMA_CH_samp;
            ADCx_Temp[6]=ADCxCHxValue_temp[6]/DMA_CH_samp;
            ADCx_Temp[7]=ADCxCHxValue_temp[7]/DMA_CH_samp;
            // if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
            // {Error_Handler();}
            SNA_0();
            SNB_0();
            if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue12,DMA_CH_samp_SUM)!= HAL_OK)
            {Error_Handler();}
            break;
        case 1:
            for (i = 0; i < DMA_CH_samp; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue12[j+i*8];
                }
            }
            ADCx_Vmod[0]=ADCxCHxValue_temp[4]/DMA_CH_samp;
            ADCx_Vmod[1]=ADCxCHxValue_temp[5]/DMA_CH_samp;
            ADCx_Temp[0]=ADCxCHxValue_temp[6]/DMA_CH_samp;
            ADCx_Temp[1]=ADCxCHxValue_temp[7]/DMA_CH_samp;

            // if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
            // {Error_Handler();}
            SNA_1();
            SNB_0();
            if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue34,DMA_CH_samp_SUM)!= HAL_OK)
            {Error_Handler();}
            break;
        case 2:
            for (i = 0; i < DMA_CH_samp; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue34[j+i*8];
                }
            }
            ADCx_Vmod[2]=ADCxCHxValue_temp[4]/DMA_CH_samp;
            ADCx_Vmod[3]=ADCxCHxValue_temp[5]/DMA_CH_samp;
            ADCx_Temp[2]=ADCxCHxValue_temp[6]/DMA_CH_samp;
            ADCx_Temp[3]=ADCxCHxValue_temp[7]/DMA_CH_samp;
            // if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
            // {Error_Handler();}
            SNA_0();
            SNB_1();
            if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue56,DMA_CH_samp_SUM)!= HAL_OK)
            {Error_Handler();}
            break;
        case 3:
            for (i = 0; i < DMA_CH_samp; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue56[j+i*8];
                }
            }
            ADCx_Vmod[4]=ADCxCHxValue_temp[4]/DMA_CH_samp;
            ADCx_Vmod[5]=ADCxCHxValue_temp[5]/DMA_CH_samp;
            ADCx_Temp[4]=ADCxCHxValue_temp[6]/DMA_CH_samp;
            ADCx_Temp[5]=ADCxCHxValue_temp[7]/DMA_CH_samp;

            // if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
            // {Error_Handler();}
            SNA_1();
            SNB_1();
            if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue78,DMA_CH_samp_SUM)!= HAL_OK)
            {Error_Handler();}
            break;
        default:
           break;
    }

    AD_Amb=ADCxCHxValue_temp[0]/DMA_CH_samp;
    AD_Aux=ADCxCHxValue_temp[1]/DMA_CH_samp;
    AD_Bus1=ADCxCHxValue_temp[2]/DMA_CH_samp;
    AD_Bus2=ADCxCHxValue_temp[3]/DMA_CH_samp;
}
/*
*********************************************************************************************************
*	�� �� ��: ADC_Start_Stop_DMA
*	����˵��: ���ڵ�����ͣDMA
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static uint8_t SW=0;
void ADC_Start_Stop_DMA(void)
{
    // uint8_t i;
    // uint8_t j;
    // float ADCxCHxValue_temp[8]={0};

    // if(HAL_ADC_Stop_DMA(&hadc3)!= HAL_OK)
    // {Error_Handler();}

    // for (i = 0; i < DMA_CH_samp; i++)
    // {
    //     for (j = 0; j < 8; j++)
    //     {
    //         ADCxCHxValue_temp[j] = ADCxCHxValue_temp[j]+uhADCxConvertedValue56[j+i*8];
    //     }
    // }
    // ADCx_Vmod[4]=ADCxCHxValue_temp[4]/DMA_CH_samp;
    // ADCx_Vmod[5]=ADCxCHxValue_temp[5]/DMA_CH_samp;
    // ADCx_Temp[4]=ADCxCHxValue_temp[6]/DMA_CH_samp;
    // ADCx_Temp[5]=ADCxCHxValue_temp[7]/DMA_CH_samp;
    // AD_Amb=ADCxCHxValue_temp[0]/DMA_CH_samp;
    // AD_Aux=ADCxCHxValue_temp[1]/DMA_CH_samp;
    // AD_Bus1=ADCxCHxValue_temp[2]/DMA_CH_samp;
    // AD_Bus2=ADCxCHxValue_temp[3]/DMA_CH_samp;

    // SNA_0();
    // SNB_1();
    // if(HAL_ADC_Start_DMA(&hadc3,(uint32_t*)uhADCxConvertedValue56,DMA_CH_samp_SUM)!= HAL_OK)
    // {Error_Handler();}

    ADC_Switch_Channel(SW);
    SW++;
    if(SW==4)
        SW=0;
}
/*****************************(END OF FILE) *********************************/
