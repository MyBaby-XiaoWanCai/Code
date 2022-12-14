/**********************************************************************************************************
*
*	模块名称 : 通用GPIO配置
*	文件名称 : bsp_gpio.c
*	版    本 : V1.0
*	说    明 : 1.拨码开关GPIO的设置
*	作	 者：Robert.Li
*	日	 期:  2021.7.1

**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_gpio.h"
#include "main.h"
#include "iwdg.h"
/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t W5500_IP_ADDRESS[4] = {192, 168, 1, 11};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/*
*********************************************************************************************************
*	函 数 名: bsp_HWDG_InitGPIO
*	功能说明: 硬件看门狗IO初始化。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_HWDG_InitGPIO(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* 打开GPIO时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* 配置IO*/
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* 设置推挽输出模式 */
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;               /* 下拉*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO速度等级 */

    GPIO_InitStruct.Pin	=	GPIO_PIN_2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   

}

/*
*********************************************************************************************************
*	函 数 名: Feed_WDG
*	功能说明: 喂狗函数（硬件看门狗&独立看门狗）
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void Feed_WDG(void)
{
    HAL_IWDG_Refresh(&hiwdg);    //喂狗（独立看门狗）
    HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_2); //喂狗（硬件看门狗）
}

/*
*********************************************************************************************************
*	函 数 名: bsp_FR_ADDR_InitGPIO
*	功能说明: 配置拨码开关设置的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_FR_ADDR_InitGPIO(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* 打开GPIO时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();


    /* 配置拨码开关 */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;   		/* 设置输入模式 */
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;               /* 下拉*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO速度等级 */

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
*	函 数 名: IP_Scanning
*	功能说明: 读取系统的IP 设定,
*	形    参:  无
*	返 回 值: 无
*	说		明：
**********************************************************************************************************/

void IP_Scanning(void)
{
   uint8_t temp = 0;
//计算位号

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


//计算库号
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
**	函 数 名 :
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
