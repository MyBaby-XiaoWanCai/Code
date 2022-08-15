/**********************************************************************************************************
*
*	ģ������ : ͨ��GPIO����
*	�ļ����� : bsp_gpio.c
*	��    �� : V1.0
*	˵    �� : 1.���뿪��GPIO������
*	��	 �ߣ�Robert.Li
*	��	 ��:  2021.7.1

**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_gpio.h"
#include "main.h"
#include "iwdg.h"
/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t W5500_IP_ADDRESS[4] = {192, 168, 1, 11};

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/
/*
*********************************************************************************************************
*	�� �� ��: bsp_HWDG_InitGPIO
*	����˵��: Ӳ�����Ź�IO��ʼ����
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_HWDG_InitGPIO(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* ��GPIOʱ�� */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* ����IO*/
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* �����������ģʽ */
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;               /* ����*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO�ٶȵȼ� */

    GPIO_InitStruct.Pin	=	GPIO_PIN_2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   

}

/*
*********************************************************************************************************
*	�� �� ��: Feed_WDG
*	����˵��: ι��������Ӳ�����Ź�&�������Ź���
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Feed_WDG(void)
{
    HAL_IWDG_Refresh(&hiwdg);    //ι�����������Ź���
    HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_2); //ι����Ӳ�����Ź���
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_FR_ADDR_InitGPIO
*	����˵��: ���ò��뿪�����õ�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_FR_ADDR_InitGPIO(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* ��GPIOʱ�� */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();


    /* ���ò��뿪�� */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;   		/* ��������ģʽ */
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;               /* ����*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO�ٶȵȼ� */

    GPIO_InitStruct.Pin	=	ADD_1_Pin;
    HAL_GPIO_Init(ADD_1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	ADD_2_Pin;
    HAL_GPIO_Init(ADD_2_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	ADD_3_Pin;
    HAL_GPIO_Init(ADD_3_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	ADD_4_Pin;
    HAL_GPIO_Init(ADD_4_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	ADD_5_Pin;
    HAL_GPIO_Init(ADD_5_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	ADD_6_Pin;
    HAL_GPIO_Init(ADD_6_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	ADD_7_Pin;
    HAL_GPIO_Init(ADD_7_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	ADD_8_Pin;
    HAL_GPIO_Init(ADD_8_GPIO_Port, &GPIO_InitStruct);


}



/**********************************************************************************************************
*	�� �� ��: IP_Scanning
*	����˵��: ��ȡϵͳ��IP �趨,
*	��    ��:  ��
*	�� �� ֵ: ��
*	˵		����
**********************************************************************************************************/

void IP_Scanning(void)
{
   uint8_t temp = 0;
//����λ��

    if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR3,GPIO_PIN_FR_ADDR3)==0)
    {
        HAL_Delay(10);
        if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR3,GPIO_PIN_FR_ADDR3)==0)
        {
            temp += (0x01<<0);
        }
    }

    if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR2,GPIO_PIN_FR_ADDR2)==0)
    {
        HAL_Delay(10);
        if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR2,GPIO_PIN_FR_ADDR2)==0)
        {
            temp += (0x01<<1);
        }
    }

    if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR1,GPIO_PIN_FR_ADDR1)==0)
    {
        HAL_Delay(10);
        if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR1,GPIO_PIN_FR_ADDR1)==0)
        {
            temp += (0x01<<2);
        }
    }
    temp += 1;
    W5500_IP_ADDRESS[2] = temp;


//������
    temp = 0;

    if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR8,GPIO_PIN_FR_ADDR8)==0)
    {
        HAL_Delay(10);
        if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR8,GPIO_PIN_FR_ADDR8)==0)
        {
            temp += (0x01<<0);
        }
    }
    if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR7,GPIO_PIN_FR_ADDR7)==0)
    {
        HAL_Delay(10);
        if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR7,GPIO_PIN_FR_ADDR7)==0)
        {
            temp += (0x01<<1);
        }
    }
    if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR6,GPIO_PIN_FR_ADDR6)==0)
    {
        HAL_Delay(10);
        if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR6,GPIO_PIN_FR_ADDR6)==0)
        {
            temp += (0x01<<2);
        }
    }
        if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR5,GPIO_PIN_FR_ADDR5)==0)
    {
        HAL_Delay(10);
        if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR5,GPIO_PIN_FR_ADDR5)==0)
        {
            temp += (0x01<<3);
        }
    }    
        if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR4,GPIO_PIN_FR_ADDR4)==0)
    {
        HAL_Delay(10);
        if(HAL_GPIO_ReadPin(GPIO_PORT_FR_ADDR4,GPIO_PIN_FR_ADDR4)==0)
        {
            temp += (0x01<<4);
        }
    }
    
    temp += 11;
    W5500_IP_ADDRESS[3] = temp;

}




/**********************************************************************************************************
**	�� �� �� :
**	����˵�� :
**	��    �� :
**	�� �� ֵ :
**********************************************************************************************************/


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
