/**********************************************************************************************************
*
*	模块名称 : charge or discharge
*	文件名称 : bsp_charge.c
*	版    本 : V1.0
*	说    明 :
*	作	 者：Robert.Li
*	日	 期:  2021.7.2

**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp.h"



/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t Mod_DIR_Flag[8];
uint8_t Mod_V_Range_Flag[8];
uint8_t Channel_Port_Table[8] = {PORT_NO1, PORT_NO2, PORT_NO3, PORT_NO4, PORT_NO5, PORT_NO6, PORT_NO7, PORT_NO8};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void Chg_Dis_Port_Cfg(uint8_t _no, uint8_t sta);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/*
*********************************************************************************************************
*	函 数 名: bsp_InitCharge
*	功能说明: 配置充放电控制相关的GPIO,该函数被bsp_Init()调用
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitCharge(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* 打开GPIO时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    /* 配置GPIO电气属性 */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;		/* 推挽输出模式 */
    GPIO_InitStruct.Pull = GPIO_PULLUP;				/* 上拉*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;	/* GPIO速度等级 */

    /* 配置Mod_DIRx */
    GPIO_InitStruct.Pin = MOD_DIR1_PIN;
    HAL_GPIO_Init(MOD_DIR1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_DIR2_PIN;
    HAL_GPIO_Init(MOD_DIR2_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_DIR3_PIN;
    HAL_GPIO_Init(MOD_DIR3_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_DIR4_PIN;
    HAL_GPIO_Init(MOD_DIR4_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_DIR5_PIN;
    HAL_GPIO_Init(MOD_DIR5_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_DIR6_PIN;
    HAL_GPIO_Init(MOD_DIR6_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_DIR7_PIN;
    HAL_GPIO_Init(MOD_DIR7_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_DIR8_PIN;
    HAL_GPIO_Init(MOD_DIR8_PORT, &GPIO_InitStruct);

    /* 配置Mod_ENx */
    GPIO_InitStruct.Pin = MOD_EN1_PIN;
    HAL_GPIO_Init(MOD_EN1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_EN2_PIN;
    HAL_GPIO_Init(MOD_EN2_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_EN3_PIN;
    HAL_GPIO_Init(MOD_EN3_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_EN4_PIN;
    HAL_GPIO_Init(MOD_EN4_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_EN5_PIN;
    HAL_GPIO_Init(MOD_EN5_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_EN6_PIN;
    HAL_GPIO_Init(MOD_EN6_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_EN7_PIN;
    HAL_GPIO_Init(MOD_EN7_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOD_EN8_PIN;
    HAL_GPIO_Init(MOD_EN8_PORT, &GPIO_InitStruct);

    /* 配置Mod_MSx */
    GPIO_InitStruct.Pin	= MOD_MS1_PIN;
    HAL_GPIO_Init(MOD_MS1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_MS2_PIN;
    HAL_GPIO_Init(MOD_MS2_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_MS3_PIN;
    HAL_GPIO_Init(MOD_MS3_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_MS4_PIN;
    HAL_GPIO_Init(MOD_MS4_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_MS5_PIN;
    HAL_GPIO_Init(MOD_MS5_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_MS6_PIN;
    HAL_GPIO_Init(MOD_MS6_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_MS7_PIN;
    HAL_GPIO_Init(MOD_MS7_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_MS8_PIN;
    HAL_GPIO_Init(MOD_MS8_PORT, &GPIO_InitStruct);

    /* 配置Mod_V_Rangex */
    GPIO_InitStruct.Pin	= MOD_V_RANGE1_PIN;
    HAL_GPIO_Init(MOD_V_RANGE1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_V_RANGE2_PIN;
    HAL_GPIO_Init(MOD_V_RANGE2_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_V_RANGE3_PIN;
    HAL_GPIO_Init(MOD_V_RANGE3_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_V_RANGE4_PIN;
    HAL_GPIO_Init(MOD_V_RANGE4_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_V_RANGE5_PIN;
    HAL_GPIO_Init(MOD_V_RANGE5_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_V_RANGE6_PIN;
    HAL_GPIO_Init(MOD_V_RANGE6_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_V_RANGE7_PIN;
    HAL_GPIO_Init(MOD_V_RANGE7_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_V_RANGE8_PIN;
    HAL_GPIO_Init(MOD_V_RANGE8_PORT, &GPIO_InitStruct);

    /* 配置Mod_Rev_Portx */
    GPIO_InitStruct.Pin	= MOD_REV_PORT1_PIN;
    HAL_GPIO_Init(MOD_REV_PORT1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_REV_PORT2_PIN;
    HAL_GPIO_Init(MOD_REV_PORT2_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_REV_PORT3_PIN;
    HAL_GPIO_Init(MOD_REV_PORT3_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_REV_PORT4_PIN;
    HAL_GPIO_Init(MOD_REV_PORT4_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_REV_PORT5_PIN;
    HAL_GPIO_Init(MOD_REV_PORT5_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_REV_PORT6_PIN;
    HAL_GPIO_Init(MOD_REV_PORT6_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_REV_PORT7_PIN;
    HAL_GPIO_Init(MOD_REV_PORT7_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin	= MOD_REV_PORT8_PIN;
    HAL_GPIO_Init(MOD_REV_PORT8_PORT, &GPIO_InitStruct);

    /* 配置GPIO电气属性 */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;		/* 推挽输出模式 */
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;			/* 下拉 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;	/* GPIO速度等级 */

    /* 配置IO_OE */
    GPIO_InitStruct.Pin	= IO_OE_PIN;
    HAL_GPIO_Init(IO_OE_PORT, &GPIO_InitStruct);


    //设置各IO口初始状态
    //Mod_DIRx - BUCK - BSRR[31:16] - Reset
    for(int i = 0; i < 8; i++)
    {
        Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_DIR_BUCK);
        Mod_DIR_Flag[i] = BUCK_MODE;
    }

    //Mod_ENx - DISABLE - BSRR[31:16] - Reset
    for(int i = 0; i < 8; i++)
    {
        Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_OUT_DISABLE);
    }

    //Mod_MSx - MASTER - BSRR[31:16] - Reset
    for(int i = 0; i < 8; i++)
    {
        Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_MODE_MASTER);
    }

    //Mod_V_Rangex - SNESE - BSRR[31:16] - Reset
    for(int i = 0; i < 8; i++)
    {
        Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_RANGE_SENSE);
        Mod_V_Range_Flag[i] = V_SENSE;
    }

    //Rev_Port - DISABLE - BSRR[15:0] - Set
    for(int i = 0; i < 8; i++)
    {
        Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_REV_DISABLE);
    }

    //IO_OE
    HAL_GPIO_WritePin(IO_OE_PORT, IO_OE_PIN, GPIO_PIN_RESET);
}

/*
*********************************************************************************************************
*	函 数 名: Chg_Dis_Port_Cfg
*	功能说明:
*	形    参:
*	返 回 值:
*********************************************************************************************************
*/
void Chg_Dis_Port_Cfg(uint8_t _no, uint8_t sta)
{
    if (sta == PORT_DIR_BUCK)				//复位
    {
        if (_no == PORT_NO1)
        {
            MOD_DIR1_PORT->BSRR = (uint32_t)MOD_DIR1_PIN << 16U;
        }
        else if(_no == PORT_NO2)
        {
            MOD_DIR2_PORT->BSRR = (uint32_t)MOD_DIR2_PIN << 16U;
        }
        else if(_no == PORT_NO3)
        {
            MOD_DIR3_PORT->BSRR = (uint32_t)MOD_DIR3_PIN << 16U;
        }
        else if(_no == PORT_NO4)
        {
            MOD_DIR4_PORT->BSRR = (uint32_t)MOD_DIR4_PIN << 16U;
        }
        else if(_no == PORT_NO5)
        {
            MOD_DIR5_PORT->BSRR = (uint32_t)MOD_DIR5_PIN << 16U;
        }
        else if(_no == PORT_NO6)
        {
            MOD_DIR6_PORT->BSRR = (uint32_t)MOD_DIR6_PIN << 16U;
        }
        else if(_no == PORT_NO7)
        {
            MOD_DIR7_PORT->BSRR = (uint32_t)MOD_DIR7_PIN << 16U;
        }
        else if(_no == PORT_NO8)
        {
            MOD_DIR8_PORT->BSRR = (uint32_t)MOD_DIR8_PIN << 16U;
        }
        Mod_DIR_Flag[_no - 1] = BUCK_MODE;
    }
    else if (sta == PORT_DIR_BOOST)			//置位
    {
        if (_no == PORT_NO1)
        {
            MOD_DIR1_PORT->BSRR = (uint32_t)MOD_DIR1_PIN;
        }
        else if (_no == PORT_NO2)
        {
            MOD_DIR2_PORT->BSRR = (uint32_t)MOD_DIR2_PIN;
        }
        else if (_no == PORT_NO3)
        {
            MOD_DIR3_PORT->BSRR = (uint32_t)MOD_DIR3_PIN;
        }
        else if (_no == PORT_NO4)
        {
            MOD_DIR4_PORT->BSRR = (uint32_t)MOD_DIR4_PIN;
        }
        else if (_no == PORT_NO5)
        {
            MOD_DIR5_PORT->BSRR = (uint32_t)MOD_DIR5_PIN;
        }
        else if (_no == PORT_NO6)
        {
            MOD_DIR6_PORT->BSRR = (uint32_t)MOD_DIR6_PIN;
        }
        else if (_no == PORT_NO7)
        {
            MOD_DIR7_PORT->BSRR = (uint32_t)MOD_DIR7_PIN;
        }
        else if (_no == PORT_NO8)
        {
            MOD_DIR8_PORT->BSRR = (uint32_t)MOD_DIR8_PIN;
        }
        Mod_DIR_Flag[_no - 1] = BOOST_MODE;
    }
    else if (sta == PORT_OUT_DISABLE)		//复位
    {	
        if (_no == PORT_NO1)
        {
            MOD_EN1_PORT->BSRR = (uint32_t)MOD_EN1_PIN << 16U;
        }
        else if (_no == PORT_NO2)
        {
            MOD_EN2_PORT->BSRR = (uint32_t)MOD_EN2_PIN << 16U;
        }
        else if (_no == PORT_NO3)
        {
            MOD_EN3_PORT->BSRR = (uint32_t)MOD_EN3_PIN << 16U;
        }
        else if (_no == PORT_NO4)
        {
            MOD_EN4_PORT->BSRR = (uint32_t)MOD_EN4_PIN << 16U;
        }
        else if (_no == PORT_NO5)
        {
            MOD_EN5_PORT->BSRR = (uint32_t)MOD_EN5_PIN << 16U;
        }
        else if (_no == PORT_NO6)
        {
            MOD_EN6_PORT->BSRR = (uint32_t)MOD_EN6_PIN << 16U;
        }
        else if (_no == PORT_NO7)
        {
            MOD_EN7_PORT->BSRR = (uint32_t)MOD_EN7_PIN << 16U;
        }
        else if (_no == PORT_NO8)
        {
            MOD_EN8_PORT->BSRR = (uint32_t)MOD_EN8_PIN << 16U;
        }
    }
    else if (sta == PORT_OUT_ENABLE)		//置位
    {
        if (_no == PORT_NO1)
        {
            MOD_EN1_PORT->BSRR = (uint32_t)MOD_EN1_PIN;
        }
        else if (_no == PORT_NO2)
        {
            MOD_EN2_PORT->BSRR = (uint32_t)MOD_EN2_PIN;
        }
        else if (_no == PORT_NO3)
        {
            MOD_EN3_PORT->BSRR = (uint32_t)MOD_EN3_PIN;
        }
        else if (_no == PORT_NO4)
        {
            MOD_EN4_PORT->BSRR = (uint32_t)MOD_EN4_PIN;
        }
        else if (_no == PORT_NO5)
        {
            MOD_EN5_PORT->BSRR = (uint32_t)MOD_EN5_PIN;
        }
        else if (_no == PORT_NO6)
        {
            MOD_EN6_PORT->BSRR = (uint32_t)MOD_EN6_PIN;
        }
        else if (_no == PORT_NO7)
        {
            MOD_EN7_PORT->BSRR = (uint32_t)MOD_EN7_PIN;
        }
        else if (_no == PORT_NO8)
        {
            MOD_EN8_PORT->BSRR = (uint32_t)MOD_EN8_PIN;
        }
    }
    else if (sta == PORT_MODE_MASTER)		//复位
    {
        if (_no == PORT_NO1)
        {
            MOD_MS1_PORT->BSRR = (uint32_t)MOD_MS1_PIN << 16U;
        }
        else if (_no == PORT_NO2)
        {
            MOD_MS2_PORT->BSRR = (uint32_t)MOD_MS2_PIN << 16U;
        }
        else if (_no == PORT_NO3)
        {
            MOD_MS3_PORT->BSRR = (uint32_t)MOD_MS3_PIN << 16U;
        }
        else if (_no == PORT_NO4)
        {
            MOD_MS4_PORT->BSRR = (uint32_t)MOD_MS4_PIN << 16U;
        }
        else if (_no == PORT_NO5)
        {
            MOD_MS5_PORT->BSRR = (uint32_t)MOD_MS5_PIN << 16U;
        }
        else if (_no == PORT_NO6)
        {
            MOD_MS6_PORT->BSRR = (uint32_t)MOD_MS6_PIN << 16U;
        }
        else if (_no == PORT_NO7)
        {
            MOD_MS7_PORT->BSRR = (uint32_t)MOD_MS7_PIN << 16U;
        }
        else if (_no == PORT_NO8)
        {
            MOD_MS8_PORT->BSRR = (uint32_t)MOD_MS8_PIN << 16U;
        }
    }
    else if (sta == PORT_MODE_SLAVE)		//置位
    {
        if (_no == PORT_NO1)
        {
            MOD_MS1_PORT->BSRR = (uint32_t)MOD_MS1_PIN;
        }
        else if (_no == PORT_NO2)
        {
            MOD_MS2_PORT->BSRR = (uint32_t)MOD_MS2_PIN;
        }
        else if (_no == PORT_NO3)
        {
            MOD_MS3_PORT->BSRR = (uint32_t)MOD_MS3_PIN;
        }
        else if (_no == PORT_NO4)
        {
            MOD_MS4_PORT->BSRR = (uint32_t)MOD_MS4_PIN;
        }
        else if (_no == PORT_NO5)
        {
            MOD_MS5_PORT->BSRR = (uint32_t)MOD_MS5_PIN;
        }
        else if (_no == PORT_NO6)
        {
            MOD_MS6_PORT->BSRR = (uint32_t)MOD_MS6_PIN;
        }
        else if (_no == PORT_NO7)
        {
            MOD_MS7_PORT->BSRR = (uint32_t)MOD_MS7_PIN;
        }
        else if (_no == PORT_NO8)
        {
            MOD_MS8_PORT->BSRR = (uint32_t)MOD_MS8_PIN;
        }
    }
    else if (sta == PORT_REV_ENABLE)		//复位
    {
        if (_no == PORT_NO1)
        {
            MOD_REV_PORT1_PORT->BSRR = (uint32_t)MOD_REV_PORT1_PIN << 16U;
        }
        else if (_no == PORT_NO2)
        {
            MOD_REV_PORT2_PORT->BSRR = (uint32_t)MOD_REV_PORT2_PIN << 16U;
        }
        else if (_no == PORT_NO3)
        {
            MOD_REV_PORT3_PORT->BSRR = (uint32_t)MOD_REV_PORT3_PIN << 16U;
        }
        else if (_no == PORT_NO4)
        {
            MOD_REV_PORT4_PORT->BSRR = (uint32_t)MOD_REV_PORT4_PIN << 16U;
        }
        else if (_no == PORT_NO5)
        {
            MOD_REV_PORT5_PORT->BSRR = (uint32_t)MOD_REV_PORT5_PIN << 16U;
        }
        else if (_no == PORT_NO6)
        {
            MOD_REV_PORT6_PORT->BSRR = (uint32_t)MOD_REV_PORT6_PIN << 16U;
        }
        else if (_no == PORT_NO7)
        {
            MOD_REV_PORT7_PORT->BSRR = (uint32_t)MOD_REV_PORT7_PIN << 16U;
        }
        else if (_no == PORT_NO8)
        {
            MOD_REV_PORT8_PORT->BSRR = (uint32_t)MOD_REV_PORT8_PIN << 16U;
        }
    }
    else if (sta == PORT_REV_DISABLE)		//置位
    {			
        if (_no == PORT_NO1)
        {
            MOD_REV_PORT1_PORT->BSRR = (uint32_t)MOD_REV_PORT1_PIN;
        }
        else if (_no == PORT_NO2)
        {
            MOD_REV_PORT2_PORT->BSRR = (uint32_t)MOD_REV_PORT2_PIN;
        }
        else if (_no == PORT_NO3)
        {
            MOD_REV_PORT3_PORT->BSRR = (uint32_t)MOD_REV_PORT3_PIN;
        }
        else if (_no == PORT_NO4)
        {
            MOD_REV_PORT4_PORT->BSRR = (uint32_t)MOD_REV_PORT4_PIN;
        }
        else if (_no == PORT_NO5)
        {
            MOD_REV_PORT5_PORT->BSRR = (uint32_t)MOD_REV_PORT5_PIN;
        }
        else if (_no == PORT_NO6)
        {
            MOD_REV_PORT6_PORT->BSRR = (uint32_t)MOD_REV_PORT6_PIN;
        }
        else if (_no == PORT_NO7)
        {
            MOD_REV_PORT7_PORT->BSRR = (uint32_t)MOD_REV_PORT7_PIN;
        }
        else if (_no == PORT_NO8)
        {
            MOD_REV_PORT8_PORT->BSRR = (uint32_t)MOD_REV_PORT8_PIN;
        }
    }
    else if (sta == PORT_RANGE_SENSE)		//复位
    {
        if (_no == PORT_NO1)
        {
            MOD_V_RANGE1_PORT->BSRR = (uint32_t)MOD_V_RANGE1_PIN << 16U;
        }
        else if (_no == PORT_NO2)
        {
            MOD_V_RANGE2_PORT->BSRR = (uint32_t)MOD_V_RANGE2_PIN << 16U;
        }
        else if (_no == PORT_NO3)
        {
            MOD_V_RANGE3_PORT->BSRR = (uint32_t)MOD_V_RANGE3_PIN << 16U;
        }
        else if (_no == PORT_NO4)
        {
            MOD_V_RANGE4_PORT->BSRR = (uint32_t)MOD_V_RANGE4_PIN << 16U;
        }
        else if (_no == PORT_NO5)
        {
            MOD_V_RANGE5_PORT->BSRR = (uint32_t)MOD_V_RANGE5_PIN << 16U;
        }
        else if (_no == PORT_NO6)
        {
            MOD_V_RANGE6_PORT->BSRR = (uint32_t)MOD_V_RANGE6_PIN << 16U;
        }
        else if (_no == PORT_NO7)
        {
            MOD_V_RANGE7_PORT->BSRR = (uint32_t)MOD_V_RANGE7_PIN << 16U;
        }
        else if (_no == PORT_NO8)
        {
            MOD_V_RANGE8_PORT->BSRR = (uint32_t)MOD_V_RANGE8_PIN << 16U;
        }
        Mod_V_Range_Flag[_no - 1] = V_SENSE;
    }
    else if (sta == PORT_RANGE_MOD)			//置位
    {
        if (_no == PORT_NO1)
        {
            MOD_V_RANGE1_PORT->BSRR = (uint32_t)MOD_V_RANGE1_PIN;
        }
        else if (_no == PORT_NO2)
        {
            MOD_V_RANGE2_PORT->BSRR = (uint32_t)MOD_V_RANGE2_PIN;
        }
        else if (_no == PORT_NO3)
        {
            MOD_V_RANGE3_PORT->BSRR = (uint32_t)MOD_V_RANGE3_PIN;
        }
        else if (_no == PORT_NO4)
        {
            MOD_V_RANGE4_PORT->BSRR = (uint32_t)MOD_V_RANGE4_PIN;
        }
        else if (_no == PORT_NO5)
        {
            MOD_V_RANGE5_PORT->BSRR = (uint32_t)MOD_V_RANGE5_PIN;
        }
        else if (_no == PORT_NO6)
        {
            MOD_V_RANGE6_PORT->BSRR = (uint32_t)MOD_V_RANGE6_PIN;
        }
        else if (_no == PORT_NO7)
        {
            MOD_V_RANGE7_PORT->BSRR = (uint32_t)MOD_V_RANGE7_PIN;
        }
        else if (_no == PORT_NO8)
        {
            MOD_V_RANGE8_PORT->BSRR = (uint32_t)MOD_V_RANGE8_PIN;
        }
        Mod_V_Range_Flag[_no - 1] = V_MOD;
    }

}


GPIO_PinState Chg_Dis_Port_Check(uint8_t _no, uint8_t sta)
{
	GPIO_PinState Pin_Sta;
	
    if ((sta == PORT_DIR_BUCK) || (sta == PORT_DIR_BOOST))
    {
        if (_no == PORT_NO1)
        {
			Pin_Sta = HAL_GPIO_ReadPin(MOD_DIR1_PORT, MOD_DIR1_PIN);
        }
        else if(_no == PORT_NO2)
        {
			Pin_Sta = HAL_GPIO_ReadPin(MOD_DIR2_PORT, MOD_DIR2_PIN);
        }
        else if(_no == PORT_NO3)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_DIR3_PORT, MOD_DIR3_PIN);
        }
        else if(_no == PORT_NO4)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_DIR4_PORT, MOD_DIR4_PIN);
        }
        else if(_no == PORT_NO5)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_DIR5_PORT, MOD_DIR5_PIN);
        }
        else if(_no == PORT_NO6)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_DIR6_PORT, MOD_DIR6_PIN);
        }
        else if(_no == PORT_NO7)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_DIR7_PORT, MOD_DIR7_PIN);
        }
        else if(_no == PORT_NO8)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_DIR8_PORT, MOD_DIR8_PIN);
        }
    }
    else if ((sta == PORT_OUT_DISABLE) || (sta == PORT_OUT_ENABLE))
    {	
        if (_no == PORT_NO1)
        {
			Pin_Sta = HAL_GPIO_ReadPin(MOD_EN1_PORT, MOD_EN1_PIN);
        }
        else if (_no == PORT_NO2)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_EN2_PORT, MOD_EN2_PIN);
        }
        else if (_no == PORT_NO3)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_EN3_PORT, MOD_EN3_PIN);
        }
        else if (_no == PORT_NO4)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_EN4_PORT, MOD_EN4_PIN);
        }
        else if (_no == PORT_NO5)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_EN5_PORT, MOD_EN5_PIN);
        }
        else if (_no == PORT_NO6)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_EN6_PORT, MOD_EN6_PIN);
        }
        else if (_no == PORT_NO7)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_EN7_PORT, MOD_EN7_PIN);
        }
        else if (_no == PORT_NO8)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_EN8_PORT, MOD_EN8_PIN);
        }
    }
    else if ((sta == PORT_MODE_MASTER) || (sta == PORT_MODE_SLAVE))
    {
        if (_no == PORT_NO1)
        {
			Pin_Sta = HAL_GPIO_ReadPin(MOD_MS1_PORT, MOD_MS1_PIN);
        }
        else if (_no == PORT_NO2)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_MS2_PORT, MOD_MS2_PIN);
        }
        else if (_no == PORT_NO3)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_MS3_PORT, MOD_MS3_PIN);
        }
        else if (_no == PORT_NO4)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_MS4_PORT, MOD_MS4_PIN);
        }
        else if (_no == PORT_NO5)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_MS5_PORT, MOD_MS5_PIN);
        }
        else if (_no == PORT_NO6)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_MS6_PORT, MOD_MS6_PIN);
        }
        else if (_no == PORT_NO7)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_MS7_PORT, MOD_MS7_PIN);
        }
        else if (_no == PORT_NO8)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_MS8_PORT, MOD_MS8_PIN);
        }
    }
    else if ((sta == PORT_REV_ENABLE) || (sta == PORT_REV_DISABLE))
    {
        if (_no == PORT_NO1)
        {
			Pin_Sta = HAL_GPIO_ReadPin(MOD_REV_PORT1_PORT, MOD_REV_PORT1_PIN);
        }
        else if (_no == PORT_NO2)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_REV_PORT2_PORT, MOD_REV_PORT2_PIN);
        }
        else if (_no == PORT_NO3)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_REV_PORT3_PORT, MOD_REV_PORT3_PIN);
        }
        else if (_no == PORT_NO4)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_REV_PORT4_PORT, MOD_REV_PORT4_PIN);
        }
        else if (_no == PORT_NO5)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_REV_PORT5_PORT, MOD_REV_PORT5_PIN);
        }
        else if (_no == PORT_NO6)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_REV_PORT6_PORT, MOD_REV_PORT6_PIN);
        }
        else if (_no == PORT_NO7)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_REV_PORT7_PORT, MOD_REV_PORT7_PIN);
        }
        else if (_no == PORT_NO8)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_REV_PORT8_PORT, MOD_REV_PORT8_PIN);
        }
    }
    else if ((sta == PORT_RANGE_SENSE) || (sta == PORT_RANGE_MOD))
    {
        if (_no == PORT_NO1)
        {
			Pin_Sta = HAL_GPIO_ReadPin(MOD_V_RANGE1_PORT, MOD_V_RANGE1_PIN);
        }
        else if (_no == PORT_NO2)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_V_RANGE2_PORT, MOD_V_RANGE2_PIN);
        }
        else if (_no == PORT_NO3)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_V_RANGE3_PORT, MOD_V_RANGE3_PIN);
        }
        else if (_no == PORT_NO4)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_V_RANGE4_PORT, MOD_V_RANGE4_PIN);
        }
        else if (_no == PORT_NO5)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_V_RANGE5_PORT, MOD_V_RANGE5_PIN);
        }
        else if (_no == PORT_NO6)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_V_RANGE6_PORT, MOD_V_RANGE6_PIN);
        }
        else if (_no == PORT_NO7)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_V_RANGE7_PORT, MOD_V_RANGE7_PIN);
        }
        else if (_no == PORT_NO8)
        {
            Pin_Sta = HAL_GPIO_ReadPin(MOD_V_RANGE8_PORT, MOD_V_RANGE8_PIN);
        }
    }

	return Pin_Sta;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
