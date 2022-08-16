/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_charge.c
** Author      : Huang Cheng
** Date        : 2021-11-08
** Description : bsp_charge source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_charge.h"
#include "bsp_pca9555.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t Mod_DIR_Flag[8];
uint8_t Mod_V_Range_Flag[8];

uint8_t Channel_Port_Table[8] =
{
    PORT_NO1, PORT_NO2, PORT_NO3, PORT_NO4,
    PORT_NO5, PORT_NO6, PORT_NO7, PORT_NO8
};

//extern PCA_DATA Pca_Dat;
extern PcaDataType Pca9555_Data;
/* 0-Dir 1-EN 2-MS 3-Rev_Port 4-V_Range */
uint8_t Power9555_ResetData[5] = {0xFB,0x00,0xF7,0xFE,0xFD}; //Port 置0
uint8_t Power9555_SetData[5] = {0x04,0x00,0x08,0x01,0x02};	//Port 置1

/* MOD Port_Pin 查询表 */
ModPortPin_TypeDef ModPortPinTab[1] =
{
//	{MOD_DIR1_PORT, MOD_DIR2_PORT, MOD_DIR3_PORT, MOD_DIR4_PORT,
//	 MOD_DIR5_PORT, MOD_DIR6_PORT, MOD_DIR7_PORT, MOD_DIR8_PORT,
//	 MOD_DIR1_PIN,  MOD_DIR2_PIN,  MOD_DIR3_PIN,  MOD_DIR4_PIN,
//	 MOD_DIR5_PIN,  MOD_DIR6_PIN,  MOD_DIR7_PIN,  MOD_DIR8_PIN},

    {   MOD_EN1_PORT,  MOD_EN2_PORT,  MOD_EN3_PORT,  MOD_EN4_PORT,
        MOD_EN5_PORT,  MOD_EN6_PORT,  MOD_EN7_PORT,  MOD_EN8_PORT,
        MOD_EN1_PIN,   MOD_EN2_PIN,   MOD_EN3_PIN,   MOD_EN4_PIN,
        MOD_EN5_PIN,   MOD_EN6_PIN,   MOD_EN7_PIN,   MOD_EN8_PIN
    },

//	{MOD_MS1_PORT,  MOD_MS2_PORT,  MOD_MS3_PORT,  MOD_MS4_PORT,
//	 MOD_MS5_PORT,  MOD_MS6_PORT,  MOD_MS7_PORT,  MOD_MS8_PORT,
//	 MOD_MS1_PIN,   MOD_MS2_PIN,   MOD_MS3_PIN,   MOD_MS4_PIN,
//	 MOD_MS5_PIN,   MOD_MS6_PIN,   MOD_MS7_PIN,   MOD_MS8_PIN},
//
//	{MOD_REV_PORT1_PORT, MOD_REV_PORT2_PORT, MOD_REV_PORT3_PORT, MOD_REV_PORT4_PORT,
//	 MOD_REV_PORT5_PORT, MOD_REV_PORT6_PORT, MOD_REV_PORT7_PORT, MOD_REV_PORT8_PORT,
//	 MOD_REV_PORT1_PIN,  MOD_REV_PORT2_PIN,  MOD_REV_PORT3_PIN,  MOD_REV_PORT4_PIN,
//	 MOD_REV_PORT5_PIN,  MOD_REV_PORT6_PIN,  MOD_REV_PORT7_PIN,  MOD_REV_PORT8_PIN},
//
//	{MOD_V_RANGE1_PORT,  MOD_V_RANGE2_PORT,  MOD_V_RANGE3_PORT,  MOD_V_RANGE4_PORT,
//	 MOD_V_RANGE5_PORT,  MOD_V_RANGE6_PORT,  MOD_V_RANGE7_PORT,  MOD_V_RANGE8_PORT,
//	 MOD_V_RANGE1_PIN,   MOD_V_RANGE2_PIN,   MOD_V_RANGE3_PIN,   MOD_V_RANGE4_PIN,
//	 MOD_V_RANGE5_PIN,   MOD_V_RANGE6_PIN,   MOD_V_RANGE7_PIN,   MOD_V_RANGE8_PIN},
};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/*
*********************************************************************************************************
*	函 数 名: bsp_InitCharge
*	功能说明: 充放电控制相关的GPIO初始化,该函数被bsp_Init()调用
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitCharge(void)
{
    stc_gpio_init_t stcChargeGpioInit;

    /* 设置板载Mod pin的属性:推挽输出、初始化状态为RESET等 */
    (void)GPIO_StructInit(&stcChargeGpioInit);
    stcChargeGpioInit.u16PullUp = PIN_PU_ON;
    stcChargeGpioInit.u16PinDir = PIN_DIR_OUT;
    stcChargeGpioInit.u16PinDrv = PIN_DRV_HIGH;
    stcChargeGpioInit.u16PinOType = PIN_OTYPE_CMOS;

    /* 配置Mod_ENx JN8B 仍沿用IO控制输出的方式 */
    (void)GPIO_Init(MOD_EN1_PORT, MOD_EN1_PIN, &stcChargeGpioInit);
    (void)GPIO_Init(MOD_EN2_PORT, MOD_EN2_PIN, &stcChargeGpioInit);
    (void)GPIO_Init(MOD_EN3_PORT, MOD_EN3_PIN, &stcChargeGpioInit);
    (void)GPIO_Init(MOD_EN4_PORT, MOD_EN4_PIN, &stcChargeGpioInit);
    (void)GPIO_Init(MOD_EN5_PORT, MOD_EN5_PIN, &stcChargeGpioInit);
    (void)GPIO_Init(MOD_EN6_PORT, MOD_EN6_PIN, &stcChargeGpioInit);
    (void)GPIO_Init(MOD_EN7_PORT, MOD_EN7_PIN, &stcChargeGpioInit);
    (void)GPIO_Init(MOD_EN8_PORT, MOD_EN8_PIN, &stcChargeGpioInit);

    //设置各IO口初始状态
    //Mod_DIRx - BUCK - BSRR[31:16] - Reset
    for(uint8_t i = 0; i < 8; i++)
    {
        Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_DIR_BUCK);
        Mod_DIR_Flag[i] = BUCK_MODE;
        bsp_DelayMS(2);
    }

    //Mod_ENx - DISABLE - BSRR[31:16] - Reset
    for(uint8_t i = 0; i < 8; i++)
    {
        Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_OUT_DISABLE);
        bsp_DelayMS(2);
    }

    //Mod_MSx - MASTER - BSRR[31:16] - Reset JN9B默认为SET
    for(uint8_t i = 0; i < 8; i++)
    {
        Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_MODE_MASTER);
        bsp_DelayMS(2);
    }

//	//Rev_Port - DISABLE - BSRR[15:0] - Set
    for(uint8_t i = 1; i < 9; i++)
    {
//	Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_REV_ENABLE);

        Pca9555_Data.Power_Board[i].WriteData[0] = 0xFF;
        Pca9555_Data.Power_Board[i].WriteData[1] = 0x01;

        PCA_9548WRTo_9555(I2C_PCA9548,i,PCA_WRITE,Pca9555_Data.Power_Board[i].WriteData,0x01);
        bsp_DelayMS(2);
    }

    //Mod_V_Rangex - SNESE - BSRR[31:16] - Reset
    for(uint8_t i = 0; i < 8; i++)
    {
        Chg_Dis_Port_Cfg(Channel_Port_Table[i], PORT_RANGE_SENSE);
        Mod_V_Range_Flag[i] = V_SENSE;
        bsp_DelayMS(2);
    }
}

/*
*********************************************************************************************************
*	函 数 名: Chg_Dis_Port_Cfg
*	功能说明: 充放电控制相关的GPIO端口输出状态设置
*	形    参: _no - 端口号(1-8)对应通道号，sta - 工作状态
*	返 回 值: 无
*********************************************************************************************************
*/
void Chg_Dis_Port_Cfg(uint8_t _no, uint8_t sta)
{
    int32_t ls, us;
    Dis_int_UserDefine();
    ls = osKernelLock();

    uint8_t CtrlPortState = 0x00;      //0-复位，1-置位
    uint8_t CtrlPortType = 0x00;       //当前需要控制的Port类型 DIR、MS、DIR、Vange、Rev_Port 当控制端口类型为1时,控制EN IO

    /* 检查输入参数的有效性 */
    if((_no > PORT_NO_MAX) || (sta > PORT_STA_MAX))
    {
        return;
    }

    /* 解析传入的控制状态 */
    if((sta%2) == 0x00)
    {
        CtrlPortState = 0x01;
        CtrlPortType = (sta/2) - 1;
    }
    else
    {
        CtrlPortState = 0x00;
        CtrlPortType = ((sta+1)/2) - 1;
    }

    /* 根据解析的端口类型和端口状态对应查表赋值 */
    if(0x00 == CtrlPortState)
    {
        //复位 置0
        if(CtrlPortType == MOD_EN_TYPE)	//EN端口IO控制
        {
            GPIO_ResetPins(ModPortPinTab[0].GpioPort[_no-1], ModPortPinTab[0].GpioPin[_no-1]);
        }
        else //PCA9555控制  CtrlPortType = 0,2,3,4
        {
            Pca9555_Data.Power_Board[_no].WriteData[0] = 0xFF;
            Pca9555_Data.Power_Board[_no].WriteData[1] = Power9555_ResetData[CtrlPortType];

            PCA_9548WRTo_9555(I2C_PCA9548,_no,PCA_WRITE,Pca9555_Data.Power_Board[_no].WriteData,CtrlPortState);
        }

        /* 标志位赋值 */
        if(PORT_DIR_BUCK == sta)
        {
            Mod_DIR_Flag[_no - 1] = BUCK_MODE;
        }

        if(PORT_RANGE_SENSE == sta)
        {
            Mod_V_Range_Flag[_no - 1] = V_SENSE;
        }
    }
    else
    {
        //置位 1
        if(CtrlPortType == MOD_EN_TYPE)	//EN端口IO控制
        {
            GPIO_SetPins(ModPortPinTab[0].GpioPort[_no-1], ModPortPinTab[0].GpioPin[_no-1]);
        }
        else//PCA9555控制  CtrlPortType = 0,2,3,4
        {
            Pca9555_Data.Power_Board[_no].WriteData[0] = 0xFF;
            Pca9555_Data.Power_Board[_no].WriteData[1] = Power9555_SetData[CtrlPortType];

            PCA_9548WRTo_9555(I2C_PCA9548,_no,PCA_WRITE,Pca9555_Data.Power_Board[_no].WriteData,CtrlPortState);
        }

        /* 标志位赋值 */
        if(PORT_DIR_BOOST == sta)
        {
            Mod_DIR_Flag[_no - 1] = BOOST_MODE;
        }

        if(PORT_RANGE_MOD == sta)
        {
            Mod_V_Range_Flag[_no - 1] = V_MOD;
        }
    }

    us =osKernelUnlock();
    osKernelRestoreLock(us);
    osKernelRestoreLock(ls);
    En_int_UserDefine();
    bsp_DelayMS(1);	//必须添加，切换任务重置任务锁
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
