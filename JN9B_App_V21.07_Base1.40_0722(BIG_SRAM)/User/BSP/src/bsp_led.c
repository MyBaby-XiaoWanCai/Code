/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_led.c
** Author      : Huang Cheng
** Date        : 2021-11-05
** Description : This file is used to initialize and drive led
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_led.h"
#include "bsp_pca9555.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/

/**********************************************************************************************************
**                                         静态变量定义区
**********************************************************************************************************/
//GPIO PORT查询表
static uint8_t LedGpioPortTab[LED_MAX_NUM+1] =
{
#ifdef YCJ
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, //

    GPIO_PORT_NOUSE, GPIO_PORT_LED82,GPIO_PORT_LED83
#else
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_NOUSE, GPIO_PORT_NOUSE, GPIO_PORT_NOUSE,
    GPIO_PORT_LED82,GPIO_PORT_LED83
#endif
};

//GPIO PIN查询表
static uint16_t LedGpioPinTab[LED_MAX_NUM+1] =
{
#ifdef YCJ
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,  GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, //

    GPIO_PIN_NOUSE, GPIO_PIN_LED82,GPIO_PIN_LED83
#else
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_NOUSE, GPIO_PIN_NOUSE, GPIO_PIN_NOUSE,
    GPIO_PIN_LED82,GPIO_PIN_LED83
#endif
};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void bsp_InitLed(void);
uint8_t Led_Shine_Ctrl(uint8_t _no, uint16_t t_on, uint16_t t_off, uint16_t t_scale);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : bsp_InitLed()
**	功能说明 : 配置LED指示灯相关的GPIO
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitLed(void)
{
    stc_gpio_init_t stcLedGpioInit;

    /* 设置板载Led pin的属性:推挽输出、初始化状态为SET等 */
    (void)GPIO_StructInit(&stcLedGpioInit);
    stcLedGpioInit.u16PinState = PIN_STATE_SET;
    stcLedGpioInit.u16PullUp = PIN_PU_ON;
    stcLedGpioInit.u16PinDir = PIN_DIR_OUT;
    stcLedGpioInit.u16PinDrv = PIN_DRV_HIGH;
    stcLedGpioInit.u16PinOType = PIN_OTYPE_CMOS;

    /* 写入板载Led pin的属性 */
    (void)GPIO_Init(GPIO_PORT_LED82, GPIO_PIN_LED82, &stcLedGpioInit);
    (void)GPIO_Init(GPIO_PORT_LED83, GPIO_PIN_LED83, &stcLedGpioInit);
}

//测试
void LedTest(void)
{
    bsp_LedToggle(RUN_LED_R_NO);
}

extern PcaDataType Pca9555_Data;
/*
	R-0,G-1 亮绿灯; R-1,G-0,亮红灯; R-1,G-1,灯熄灭
	亮红灯：先将绿灯位强置0，再将红灯位强置1
	亮绿灯：先将红灯位强置0，再将绿灯位强置1
*/

//C053A版灯板 灭灯
//uint8_t LampPlate_Off[18] = {0xBF,0x7F,0xFE,0xFE,0xFD,0xFD,0xFB,0xFB,
//						0xF7,0xF7,0xEF,0xEF,0xDF,0xDF,0xBF,0xBF,0x7F,0x7F};

//C053B版 灭灯
uint8_t LampPlate_Off[18] = {0x40,0x80,0x01,0x01,0x02,0x02,0x04,0x04,
                             0x08,0x08,0x10,0x10,0x20,0x20,0x40,0x40,0x80,0x80
                            };

////C053A版 亮红灯时,需将绿灯强制为0;
//uint8_t LampPlate_OldOn[18] = {0x7F,0xBF,0xFE,0xFE,0xFD,0xFD,0xFB,0xFB,0xF7,0xF7,
//											0xEF,0xEF,0xDF,0xDF,0xBF,0xBF,0x7F,0x7F};	//偶数位-绿灯置低; 基数位-红灯置低,其他位保持不变
////置高位
//uint8_t LampPlate_NewOn[18] = {0x40,0x80,0x01,0x01,0x02,0x04,0x04,0x08,0x08,
//											0x10,0x10,0x20,0x20,0x40,0x40,0x80,0x80};//偶数位-红灯置高; 基数位-绿灯置高,其他位保持不变
//C053B版 亮红灯时,需将绿灯强制为1; 两个版本的灯板控制逻辑是反的
uint8_t LampPlate_OldOn[18] = {0x7F,0xBF,0x01,0x01,0x02,0x04,0x04,0x08,0x08,
                               0x10,0x10,0x20,0x20,0x40,0x40,0x80,0x80
                              };	//偶数位-绿灯置低; 基数位-红灯置低,其他位保持不变
//置高位
uint8_t LampPlate_NewOn[18] = {0x80,0x40,0xFE,0xFE,0xFD,0xFD,0xFB,0xFB,0xF7,0xF7,
                               0xEF,0xEF,0xDF,0xDF,0xBF,0xBF,0x7F,0x7F
                              };//偶数位-红灯置高; 基数位-绿灯置高,其他位保持不变

/**********************************************************************************************************
**	函 数 名 : bsp_LedOff()
**	功能说明 : 熄灭指定的LED指示灯
**	形    参 : _no : 指示灯序号(0~12)
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_LedOff(uint8_t _no)
{
    /* Led number有效性判断 */
    if((_no == LED_NOUSE_NUM) || (_no > LED_MAX_NUM))
    {
        return;
    }

    if(_no == LED_82_NO || _no == LED_83_NO) //板载灯
    {
        GPIO_ResetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]);
    }
    else if(_no == RUN_LED_R_NO || _no == RUN_LED_G_NO) //run灯灭
    {
        Pca9555_RegRead(I2C_LED, PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].ReadData); //读取当前PCA9555数据

        if(_no == RUN_LED_R_NO)        //运行红灯灭
        {
            Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].ReadData[0] | LampPlate_Off[_no];
        }
        else if(_no == RUN_LED_G_NO)        //运行绿灯灭
        {
            Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].ReadData[0] | LampPlate_Off[_no];
        }

        Pca9555_RegWrite(I2C_LED,PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].WriteData);
    }
    else //通道运行指示灯
    {
        Pca9555_RegRead(I2C_LED, PCA9555_LED_ADDR,Pca9555_Data.LED_Board[1].ReadData); //读取当前PCA9555数据

        if(_no%2 == 0)        //偶数 - 灭红灯 2/4/6/8/10/12/14/16
        {
            Pca9555_Data.LED_Board[1].WriteData[0] = Pca9555_Data.LED_Board[1].ReadData[0] | LampPlate_Off[_no];
        }
        else if(_no%2 == 1)        //奇数 - 灭绿灯 3/5/7/9/11/13/15/17
        {
            Pca9555_Data.LED_Board[1].WriteData[1] = Pca9555_Data.LED_Board[1].ReadData[1] | LampPlate_Off[_no];
        }

        Pca9555_RegWrite(I2C_LED,PCA9555_LED_ADDR,Pca9555_Data.LED_Board[1].WriteData);
    }
}

/**********************************************************************************************************
**	函 数 名 : bsp_LedOn()
**	功能说明 : 点亮指定的LED指示灯
**	形    参 : _no : 指示灯序号(0~12)
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_LedOn(uint8_t _no)
{
    /* Led number有效性判断 */
    if((_no == LED_NOUSE_NUM) || (_no > LED_MAX_NUM))
    {
        return;
    }

    if(_no == RUN_LED_R_NO || _no == RUN_LED_G_NO) //run灯亮 0-红,1-绿
    {
        Pca9555_RegRead(I2C_LED, PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].ReadData); //读取当前PCA9555数据

#ifdef C042A_7606
        Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].ReadData[0] | LampPlate_OldOn[_no];//蓝灯强制置高
        Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].WriteData[0] & LampPlate_NewOn[_no];//红灯强制置低
        Pca9555_Data.LED_Board[0].WriteData[1] = Pca9555_Data.LED_Board[0].ReadData[1];
#else
        Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].ReadData[0] & LampPlate_OldOn[_no];//蓝灯强制置低
        Pca9555_Data.LED_Board[0].WriteData[0] = Pca9555_Data.LED_Board[0].WriteData[0] | LampPlate_NewOn[_no];//红灯强制置高
        Pca9555_Data.LED_Board[0].WriteData[1] = Pca9555_Data.LED_Board[0].ReadData[1];
#endif
        Pca9555_RegWrite(I2C_LED,PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].WriteData);	//run灯
    }
    else if(_no == LED_82_NO || _no == LED_83_NO)
    {
        GPIO_SetPins(LedGpioPortTab[_no], LedGpioPinTab[_no]); //板载灯
    }
    else //通道运行指示灯
    {
        Pca9555_RegRead(I2C_LED, PCA9555_LED_ADDR,Pca9555_Data.LED_Board[1].ReadData); //读取当前PCA9555数据

        if(_no%2 == 0)
        {
#ifdef C042A_7606
            Pca9555_Data.LED_Board[1].WriteData[0] = Pca9555_Data.LED_Board[1].ReadData[0] & LampPlate_NewOn[_no];
            Pca9555_Data.LED_Board[1].WriteData[1] = Pca9555_Data.LED_Board[1].ReadData[1] | LampPlate_OldOn[_no];
#else
            Pca9555_Data.LED_Board[1].WriteData[0] = Pca9555_Data.LED_Board[1].ReadData[0] | LampPlate_NewOn[_no];
            Pca9555_Data.LED_Board[1].WriteData[1] = Pca9555_Data.LED_Board[1].ReadData[1] & LampPlate_OldOn[_no];
#endif
        }
        else
        {
#ifdef C042A_7606
            Pca9555_Data.LED_Board[1].WriteData[1] = Pca9555_Data.LED_Board[1].ReadData[1] & LampPlate_NewOn[_no];
            Pca9555_Data.LED_Board[1].WriteData[0] = Pca9555_Data.LED_Board[1].ReadData[0] | LampPlate_OldOn[_no];
#else
            Pca9555_Data.LED_Board[1].WriteData[1] = Pca9555_Data.LED_Board[1].ReadData[1] | LampPlate_NewOn[_no];
            Pca9555_Data.LED_Board[1].WriteData[0] = Pca9555_Data.LED_Board[1].ReadData[0] & LampPlate_OldOn[_no];
#endif
        }

        Pca9555_RegWrite(I2C_LED,PCA9555_LED_ADDR,Pca9555_Data.LED_Board[1].WriteData);
    }
}

/**********************************************************************************************************
**	函 数 名 : bsp_LedToggle()
**	功能说明 : 翻转指定的LED指示灯
**	形    参 : _no : 指示灯序号(0~12)
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_LedToggle(uint8_t _no)
{
    /* Led number有效性判断 */
    if((_no == LED_NOUSE_NUM) || (_no > LED_MAX_NUM))
    {
        return;
    }

    GPIO_TogglePins(LedGpioPortTab[_no], LedGpioPinTab[_no]);
}


/**********************************************************************************************************
**	函 数 名 : Led_Shine_Ctrl()
**	功能说明 : 根据不同的工作状态控制LED进行相应动作
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
uint8_t Led_Shine_Ctrl(uint8_t _no, uint16_t t_on, uint16_t t_off, uint16_t t_scale)
{
		static uint16_t g_usLedCtrlCnt[18] = {0};   //8通道机器一共9个灯,每个灯2个颜色LED一共18种
	 
    if(t_scale)
    {
        uint16_t cnt_on = t_on / t_scale;
        uint16_t cnt_off = t_off / t_scale;

        if(cnt_on == 0)
        {
            bsp_LedOff(_no);
        }
        else if(cnt_off == 0)
        {
            bsp_LedOn(_no);
        }
        else
        {
            if(g_usLedCtrlCnt[_no] == 0)
            {
                bsp_LedOn(_no);
            }
            if(g_usLedCtrlCnt[_no] == cnt_on)
            {
                bsp_LedOff(_no);
            }

            if(g_usLedCtrlCnt[_no] < (cnt_on + cnt_off))
            {
                g_usLedCtrlCnt[_no]++;
            }
            else
            {
                g_usLedCtrlCnt[_no] = 0;
            }
        }
    }
    else
    {
        return 1;
    }

    return 0;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

