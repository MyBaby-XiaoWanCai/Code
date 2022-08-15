/**********************************************************************************************************
*
*   模块名称 :  硬件软件保护处理模块
*   文件名称 : bsp_Protection.c
*   版    本 : V1.0
*   说    明 : 1.HW and software protection function
*   作   者：wl
*   日   期:  2021.9.14

**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp.h"


/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/*
*********************************************************************************************************
*   函 数 名: bsp_HW_Fault_InitGPIO
*   功能说明: 配置HW Fault Singal 设输入的GPIO,  该函数被 bsp_Init() 调用。
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_HW_Fault_InitGPIO(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct = {0};

    /* 打开GPIO时钟 */
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();


    /* 配置HW Fault Signal */
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
