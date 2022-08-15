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
#include "bsp.h"


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


void bsp_M_Loop_InitGPIO(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* 配置LOOP */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* 设置输出推挽模式 */
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;               /* 下拉*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO速度等级 */

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


    /* 配置LED */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;   		/* 设置输入模式 */
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;               /* 下拉*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO速度等级 */

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
    if(feed_hardware_watchdog_enable)
    {
        HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_2); //喂狗（硬件看门狗）
    }
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
