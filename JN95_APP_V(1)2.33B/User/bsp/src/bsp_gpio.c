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
#include "bsp.h"


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


void bsp_M_Loop_InitGPIO(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* ����LOOP */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* �����������ģʽ */
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;               /* ����*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO�ٶȵȼ� */

    GPIO_InitStruct.Pin	=	GPIO_PIN_LOOP1;
    HAL_GPIO_Init(GPIO_PORT_LOOP1, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_LOOP2;
    HAL_GPIO_Init(GPIO_PORT_LOOP2, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_LOOP3;
    HAL_GPIO_Init(GPIO_PORT_LOOP3, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_LOOP4;
    HAL_GPIO_Init(GPIO_PORT_LOOP4, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_LOOP5;
    HAL_GPIO_Init(GPIO_PORT_LOOP5, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_LOOP6;
    HAL_GPIO_Init(GPIO_PORT_LOOP6, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_LOOP7;
    HAL_GPIO_Init(GPIO_PORT_LOOP7, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_LOOP8;
    HAL_GPIO_Init(GPIO_PORT_LOOP8, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIO_PORT_LOOP1, GPIO_PIN_LOOP1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIO_PORT_LOOP2, GPIO_PIN_LOOP2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIO_PORT_LOOP3, GPIO_PIN_LOOP3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIO_PORT_LOOP4, GPIO_PIN_LOOP4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIO_PORT_LOOP5, GPIO_PIN_LOOP5, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIO_PORT_LOOP6, GPIO_PIN_LOOP6, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIO_PORT_LOOP7, GPIO_PIN_LOOP7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIO_PORT_LOOP8, GPIO_PIN_LOOP8, GPIO_PIN_RESET);

}


void Enable_Mloop(uint8_t _ch)
{
	switch(_ch)
	{
		case 0:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP1, GPIO_PIN_LOOP1, GPIO_PIN_SET);
		break;

		case 1:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP2, GPIO_PIN_LOOP2, GPIO_PIN_SET);
		break;
		
		case 2:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP3, GPIO_PIN_LOOP3, GPIO_PIN_SET);
		break;
		
		case 3:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP4, GPIO_PIN_LOOP4, GPIO_PIN_SET);
		break;
		
		case 4:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP5, GPIO_PIN_LOOP5, GPIO_PIN_SET);
		break;
		
		case 5:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP6, GPIO_PIN_LOOP6, GPIO_PIN_SET);
		break;
		
		case 6:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP7, GPIO_PIN_LOOP7, GPIO_PIN_SET);
		break;
		
		case 7:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP8, GPIO_PIN_LOOP8, GPIO_PIN_SET);
		break;
		
		default:
			
		break;
	}
}

void Disable_Mloop(uint8_t _ch)
{
	switch(_ch)
	{
		case 0:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP1, GPIO_PIN_LOOP1, GPIO_PIN_RESET);
		break;

		case 1:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP2, GPIO_PIN_LOOP2, GPIO_PIN_RESET);
		break;
		
		case 2:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP3, GPIO_PIN_LOOP3, GPIO_PIN_RESET);
		break;
		
		case 3:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP4, GPIO_PIN_LOOP4, GPIO_PIN_RESET);
		break;
		
		case 4:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP5, GPIO_PIN_LOOP5, GPIO_PIN_RESET);
		break;
		
		case 5:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP6, GPIO_PIN_LOOP6, GPIO_PIN_RESET);
		break;
		
		case 6:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP7, GPIO_PIN_LOOP7, GPIO_PIN_RESET);
		break;
		
		case 7:
			HAL_GPIO_WritePin(GPIO_PORT_LOOP8, GPIO_PIN_LOOP8, GPIO_PIN_RESET);
		break;
		
		default:
			
		break;
	}
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


    /* ����LED */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;   		/* ��������ģʽ */
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;               /* ����*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO�ٶȵȼ� */

    GPIO_InitStruct.Pin	=	GPIO_PIN_FR_ADDR1;
    HAL_GPIO_Init(GPIO_PORT_FR_ADDR1, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_FR_ADDR2;
    HAL_GPIO_Init(GPIO_PORT_FR_ADDR2, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_FR_ADDR3;
    HAL_GPIO_Init(GPIO_PORT_FR_ADDR3, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_FR_ADDR4;
    HAL_GPIO_Init(GPIO_PORT_FR_ADDR4, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_FR_ADDR5;
    HAL_GPIO_Init(GPIO_PORT_FR_ADDR5, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_FR_ADDR6;
    HAL_GPIO_Init(GPIO_PORT_FR_ADDR6, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_FR_ADDR7;
    HAL_GPIO_Init(GPIO_PORT_FR_ADDR7, &GPIO_InitStruct);

    GPIO_InitStruct.Pin	=	GPIO_PIN_FR_ADDR8;
    HAL_GPIO_Init(GPIO_PORT_FR_ADDR8, &GPIO_InitStruct);


}

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
    if(feed_hardware_watchdog_enable)
    {
        HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_2); //ι����Ӳ�����Ź���
    }
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
