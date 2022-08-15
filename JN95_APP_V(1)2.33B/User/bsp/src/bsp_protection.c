/**********************************************************************************************************
*
*   ģ������ :  Ӳ�������������ģ��
*   �ļ����� : bsp_Protection.c
*   ��    �� : V1.0
*   ˵    �� : 1.HW and software protection function
*   ��   �ߣ�wl
*   ��   ��:  2021.9.14

**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp.h"


/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/*
*********************************************************************************************************
*   �� �� ��: bsp_HW_Fault_InitGPIO
*   ����˵��: ����HW Fault Singal �������GPIO,  �ú����� bsp_Init() ���á�
*   ��    ��:  ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_HW_Fault_InitGPIO(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct = {0};

    /* ��GPIOʱ�� */
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();


    /* ����HW Fault Signal */
    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = PG8_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(PG8_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = PG7_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(PG7_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : PFPin PFPin PFPin PFPin
                             PFPin */
    GPIO_InitStruct.Pin = PG6_Pin | PG5_Pin | PG4_Pin | PG3_Pin
                          | PG2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = PG1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(PG1_GPIO_Port, &GPIO_InitStruct);

}


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
