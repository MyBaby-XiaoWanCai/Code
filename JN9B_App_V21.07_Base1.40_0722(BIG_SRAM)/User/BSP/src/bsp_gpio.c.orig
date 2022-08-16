/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_gpio.c
** Author      : Huang Cheng
** Date        : 2021-11-04
** Description : bsp_gpio source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp.h"
#include "bsp_pca9555.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
//测试
uint8_t W5500_IP_ADDRESS[4] = {192, 168, 0, 0};

uint8_t dev_addr = 0;

/**********************************************************************************************************/
HCPortPin_TypeDef BattOverVoltPortPinTab[BATT_OverVolt_PinNum] = 
{
	{GPIO_PORT_BATTOVERVOLT_1, GPIO_PIN_BATTOVERVOLT_1},
	{GPIO_PORT_BATTOVERVOLT_2, GPIO_PIN_BATTOVERVOLT_2},
	{GPIO_PORT_BATTOVERVOLT_3, GPIO_PIN_BATTOVERVOLT_3},
	{GPIO_PORT_BATTOVERVOLT_4, GPIO_PIN_BATTOVERVOLT_4},
	{GPIO_PORT_BATTOVERVOLT_5, GPIO_PIN_BATTOVERVOLT_5},
	{GPIO_PORT_BATTOVERVOLT_6, GPIO_PIN_BATTOVERVOLT_6},
	{GPIO_PORT_BATTOVERVOLT_7, GPIO_PIN_BATTOVERVOLT_7},
	{GPIO_PORT_BATTOVERVOLT_8, GPIO_PIN_BATTOVERVOLT_8},
};	//电池过温过压报警检测IO

HCPortPin_TypeDef PowerBoardSignalPinTable[MAINPOWER_SIGNALNUM] = 
{
	{GPIO_PORT_MAINBOARD,GPIO_PIN_MAINBOARD},
	{GPIO_PORT_POWERBOARD_1, GPIO_PIN_POWERBOARD_1},
	{GPIO_PORT_POWERBOARD_2, GPIO_PIN_POWERBOARD_2},
	{GPIO_PORT_POWERBOARD_3, GPIO_PIN_POWERBOARD_3},
	{GPIO_PORT_POWERBOARD_4, GPIO_PIN_POWERBOARD_4},
	{GPIO_PORT_POWERBOARD_5, GPIO_PIN_POWERBOARD_5},
	{GPIO_PORT_POWERBOARD_6, GPIO_PIN_POWERBOARD_6},
	{GPIO_PORT_POWERBOARD_7, GPIO_PIN_POWERBOARD_7},
	{GPIO_PORT_POWERBOARD_8, GPIO_PIN_POWERBOARD_8},	
}; //主控及功率版在位信号检测IO

HCPortPin_TypeDef FanFaultPinTable[FAN_FaultPinNum] = 
{
	{GPIO_PORT_FAN1, GPIO_PIN_FAN1},
	{GPIO_PORT_FAN2, GPIO_PIN_FAN2},
	{GPIO_PORT_FAN3, GPIO_PIN_FAN3},
	{GPIO_PORT_FAN4, GPIO_PIN_FAN4},
	{GPIO_PORT_FAN5, GPIO_PIN_FAN5},
	{GPIO_PORT_FAN6, GPIO_PIN_FAN6},
};	//风扇故障检测IO

HCPortPin_TypeDef VersionPinTable[MAIN_VerGpioNum] = 
{
	{GPIO_PORT_MAINBOOM0, GPIO_PIN_MAINBOOM0},
	{GPIO_PORT_MAINBOOM1, GPIO_PIN_MAINBOOM1},
	{GPIO_PORT_MAINBOOM2, GPIO_PIN_MAINBOOM2},
	
	{GPIO_PORT_MAINPCB0, GPIO_PIN_MAINPCB0},
	{GPIO_PORT_MAINPCB1, GPIO_PIN_MAINPCB1},
	{GPIO_PORT_MAINPCB2, GPIO_PIN_MAINPCB2},
	
	{GPIO_PORT_MAINID0, GPIO_PIN_MAINID0},
	{GPIO_PORT_MAINID1, GPIO_PIN_MAINID1},
};	//版本识别IO

TestInformation IOTestInfo;	//检测IO信息存储
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : Enable_Mloop
**	功能说明 : Mloop使能函数
**	形    参 : _ch - 引脚编号
**	返 回 值 : 无
**********************************************************************************************************/
extern PcaDataType Pca9555_Data;
//uint8_t Set_Mloop = 0x10;			//C033A、B
//uint8_t Reset_Mloop = 0xEF;
uint8_t Set_Mloop = 0x80;			//C033C、D
uint8_t Reset_Mloop = 0x7F;

void Enable_Mloop(uint8_t _ch)
{
	if(_ch > 8)
	{
		return;
	}
	
//	Pca9555_Data.Power_Board[_ch].WriteData[0] = 0xFF; //C033A、B
//	Pca9555_Data.Power_Board[_ch].WriteData[1] = Set_Mloop;
	
	Pca9555_Data.Power_Board[_ch].WriteData[0] = Set_Mloop;	//C033C、D
	Pca9555_Data.Power_Board[_ch].WriteData[1] = 0xFF;	
	
	Pca9555_WRTo_M_Loop(I2C_PCA9548,_ch,1,Pca9555_Data.Power_Board[_ch].WriteData); //M_LOOP脚 置高
}

/**********************************************************************************************************
**	函 数 名 : Disable_Mloop
**	功能说明 : Mloop禁止函数
**	形    参 : _ch - 引脚编号
**	返 回 值 : 无
**********************************************************************************************************/
void Disable_Mloop(uint8_t _ch)
{
	if(_ch > 8)
	{
		return;
	}
	
//	Pca9555_Data.Power_Board[_ch].WriteData[0] = 0xFF;	//C033A、B
//	Pca9555_Data.Power_Board[_ch].WriteData[1] = Reset_Mloop;
	
	Pca9555_Data.Power_Board[_ch].WriteData[0] = Reset_Mloop;		//C033C、D
	Pca9555_Data.Power_Board[_ch].WriteData[1] = 0xFF;
	
	Pca9555_WRTo_M_Loop(I2C_PCA9548,_ch,0,Pca9555_Data.Power_Board[_ch].WriteData); //M_LOOP 脚置低位
}

/**********************************************************************************************************
**	函 数 名 : bsp_InitFanFault
**	功能说明 : 风扇故障检测IO初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitFanFault(void)
{
	uint8_t i= 0;
	
	stc_gpio_init_t stcFanGpioInit;

	/* 设置FAN故障引脚 pin的属性:推挽输出、初始化状态为RESET等 */
	(void)GPIO_StructInit(&stcFanGpioInit);
	stcFanGpioInit.u16PinDir = PIN_DIR_IN;
	stcFanGpioInit.u16PullUp = PIN_PU_ON;

	for(i = 0; i < 6;i++)
	{
		(void)GPIO_Init(FanFaultPinTable[i].GpioPort,  FanFaultPinTable[i].GpioPin,  &stcFanGpioInit);
		
		bsp_DelayUS(10);
	}
}
/**********************************************************************************************************
**	函 数 名 : bsp_InitFanFault
**	功能说明 : 主控及功率板在位检测IO初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitMainPowerSignalBoard(void)
{
	uint8_t i = 0;
	
	stc_gpio_init_t bsp_InitMainPowerSignalBoard;

	/* 初始化功率板故障及在位信号检测 pin的属性:推挽输出、初始化状态为等 */
	(void)GPIO_StructInit(&bsp_InitMainPowerSignalBoard);
	bsp_InitMainPowerSignalBoard.u16PinDir = PIN_DIR_IN;
	bsp_InitMainPowerSignalBoard.u16PullUp = PIN_PU_ON;

	for(i = 0;i < MAINPOWER_SIGNALNUM;i++)
	{
		(void)GPIO_Init(PowerBoardSignalPinTable[i].GpioPort,  PowerBoardSignalPinTable[i].GpioPin,  &bsp_InitMainPowerSignalBoard);
		
		bsp_DelayUS(10);
	}
}
/**********************************************************************************************************
**	函 数 名 : bsp_InitVersionIdent
**	功能说明 : 主控板硬件版本识别IO初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitVersionIdent(void)
{
	uint8_t i = 0;
	
	stc_gpio_init_t stcVersionIdentGpioInit;

	/* 设置硬件版本识别IO pin的属性:下拉输入、初始化状态为RESET等 */
	(void)GPIO_StructInit(&stcVersionIdentGpioInit);
	stcVersionIdentGpioInit.u16PinDir = PIN_DIR_IN;
	stcVersionIdentGpioInit.u16PullUp = PIN_PU_OFF;
	
	for(i = 0;i < MAIN_VerGpioNum;i++)
	{
		(void)GPIO_Init(VersionPinTable[i].GpioPort,  VersionPinTable[i].GpioPin,  &stcVersionIdentGpioInit);
		
		bsp_DelayUS(10);
	}
	
	Version_Mana_Read();
}

/**********************************************************************************************************
**	函 数 名 : bsp_InitBattOverVolt
**	功能说明 : 电池过压信号检测IO初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitBattOverVolt(void)
{
	uint8_t i = 0;
	
	stc_gpio_init_t stcBattOverVoltGpioInit;

	/* 设置电池过压引脚 pin的属性:下拉输入、初始化状态为RESET等 */
	(void)GPIO_StructInit(&stcBattOverVoltGpioInit);
	stcBattOverVoltGpioInit.u16PinDir = PIN_DIR_IN;
	stcBattOverVoltGpioInit.u16PullUp = PIN_PU_OFF;
	
	for(i = 0;i < BATT_OverVolt_PinNum;i++)
	{
		(void)GPIO_Init(BattOverVoltPortPinTab[i].GpioPort,  BattOverVoltPortPinTab[i].GpioPin,  &stcBattOverVoltGpioInit);
		
		bsp_DelayUS(10);
	}
}

/**********************************************************************************************************
**	函 数 名 : bsp_InitCVK
**	功能说明 : 并机模式信号引脚初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitCVK(void)
{
	stc_gpio_init_t stcCVKGpioInit;

	/* 设置并机信号 pin的属性:推挽输出、初始化状态为RESET等 */
	(void)GPIO_StructInit(&stcCVKGpioInit);
	stcCVKGpioInit.u16PinState = PIN_STATE_RESET;
	stcCVKGpioInit.u16PullUp = PIN_PU_OFF;
	stcCVKGpioInit.u16PinDir = PIN_DIR_OUT;
//	stcCVKGpioInit.u16PinDrv = PIN_DRV_HIGH;
//	stcCVKGpioInit.u16PinOType = PIN_OTYPE_CMOS;

	(void)GPIO_Init(GPIO_PORT_CVK_4,  GPIO_PIN_CVK_4,  &stcCVKGpioInit);
	(void)GPIO_Init(GPIO_PORT_CVK_8,  GPIO_PIN_CVK_8,  &stcCVKGpioInit);
}

/**********************************************************************************************************
**	函 数 名 : bsp_GpioInit
**	功能说明 : ALL 检测IO初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_GpioInit(void)
{
	bsp_InitVersionIdent();
	bsp_InitFanFault();
	bsp_InitMainPowerSignalBoard();
	bsp_InitBattOverVolt();
	bsp_InitCVK();
}

/*
*********************************************************************************************************
*	函 数 名: bsp_HWDG_InitGPIO
*	功能说明: 硬件看门狗IO初始化。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_HWDG_InitGPIO(void)
{
  stc_gpio_init_t stcHwdgGpioInit;
	
	/* 设置Hwdg pin的属性:推挽输出、初始化状态为RESET等 */
	(void)GPIO_StructInit(&stcHwdgGpioInit);
	stcHwdgGpioInit.u16PinState = PIN_STATE_RESET;
	stcHwdgGpioInit.u16PinDir = PIN_DIR_OUT;
	stcHwdgGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcHwdgGpioInit.u16PinOType = PIN_OTYPE_CMOS;
	
	/* 写入Hwdg pin的属性 */
  (void)GPIO_Init(GPIO_PORT_HWDG, GPIO_PIN_HWDG, &stcHwdgGpioInit);
}

/*
*********************************************************************************************************
*	函 数 名: Feed_WDG
*	功能说明: 喂狗函数（硬件看门狗&独立看门狗）
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Feed_WDG(void)
{
	#if 1
//    HAL_IWDG_Refresh(&hiwdg);    //喂狗（独立看门狗）
	SWDT_Feed();
	
	//喂狗（硬件看门狗）
    if(feed_hardware_watchdog_enable)
    {
        GPIO_TogglePins(GPIO_PORT_HWDG, GPIO_PIN_HWDG); 
    }
	#endif
}

/**********************************************************************************************************
*	函 数 名: IP_Scanning
*	功能说明: 读取系统的IP设定
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void IP_Scanning(void)
{
	uint8_t Res = 0; 
	uint8_t i = 0;
	uint8_t AddrTmp = 0;
	
	//灯板 版本信息以及拨码开关状态读取
	Res = Pca9555_RegRead(I2C_LED, PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].ReadData); //读取到的版本以及拨码信息存储于 Pca_Dat.ReadData
	if(Res == 0x01)
		return;
	
	dev_addr = 0;
	
	/* 获取拨码开关1-3状态 */
	for(i = 0; i < IP_ADDR3_END; i++)
	{
		if(i == 0)
		{
			if((Pca9555_Data.LED_Board[0].ReadData[1] & 0x01) == 0x00) //0000 0001
			{
				AddrTmp += 0x01 << (IP_ADDR3_END-i-1);
				
				dev_addr |= (1 << 7);
			}
		}
	  else if(i == 0x01)
		{
			if((Pca9555_Data.LED_Board[0].ReadData[1] & 0x02) == 0x00)	//0000 0010
			{
				AddrTmp += 0x01 << (IP_ADDR3_END-i-1);
				
				dev_addr |= (1 << 6);
			}
		}
		else if(i == 0x02)
		{
			if((Pca9555_Data.LED_Board[0].ReadData[1] & 0x04) == 0x00)	//0000 0100
			{
				AddrTmp += 0x01 << (IP_ADDR3_END-i-1);
				
				dev_addr |= (1 << 5);
			}
		}
	}
	W5500_IP_ADDRESS[2] = AddrTmp + 1;
	AddrTmp = 0x00;
	
	/* 获取拨码开关4-8状态 */
	for(i = IP_ADDR3_END; i < IP_ADDR4_END; i++)
	{
		if(i == 0x03)
		{
			if((Pca9555_Data.LED_Board[0].ReadData[1] & 0x08) == 0x00)	//0000 1000
			{
				AddrTmp += 0x01 << (IP_ADDR4_END-i-1);
				
				dev_addr |= (1 << 4);
			}
		}
		else if(i == 0x04)
		{
			if((Pca9555_Data.LED_Board[0].ReadData[1] & 0x10) == 0x00)	//0001 0000
			{
				AddrTmp += 0x01 << (IP_ADDR4_END-i-1);
				
				dev_addr |= (1 << 3);
			}
		}
		else if(i == 0x05)
		{
			if((Pca9555_Data.LED_Board[0].ReadData[1] & 0x20) == 0x00)	//0010 0000
			{
				AddrTmp += 0x01 << (IP_ADDR4_END-i-1);
				
				dev_addr |= (1 << 2);
			}
		}
		else if(i == 0x06)
		{
			if((Pca9555_Data.LED_Board[0].ReadData[1] & 0x40) == 0x00)	//0100 0000
			{
				AddrTmp += 0x01 << (IP_ADDR4_END-i-1);
				
				dev_addr |= (1 << 1);
			}
		}
		else if(i == 0x07)
		{
			if((Pca9555_Data.LED_Board[0].ReadData[1] & 0x80) == 0x00)	//1000 0000
			{
				AddrTmp += 0x01 << (IP_ADDR4_END-i-1);
				
				dev_addr |= (1 << 0);
			}		
		}
	}
	W5500_IP_ADDRESS[3] = AddrTmp + 11;
}

/**********************************************************************************************************
**	函 数 名 : MainPowerSignalRead()
**	功能说明 : 主控及功率板在位信号读取
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void MainPowerSignalRead()
{
	uint8_t reign_i = 0;
	static uint8_t SignalNum = 0x08;
	
	/* 主控及功率板在位信号检测 */
	for(reign_i = 0; reign_i < (CHANNEL_USE + 1); reign_i++)
	{
		if(GPIO_ReadInputPins(PowerBoardSignalPinTable[reign_i].GpioPort, PowerBoardSignalPinTable[reign_i].GpioPin) == 0x01)
		{
			bsp_DelayUS(10);
			
			if(GPIO_ReadInputPins(PowerBoardSignalPinTable[reign_i].GpioPort, PowerBoardSignalPinTable[reign_i].GpioPin) == 0x01)
			{
				IOTestInfo.MainPowerSignal[reign_i] = 1;
			}
		}
		else
		{
			IOTestInfo.MainPowerSignal[reign_i] = 0;
		}
	}
}

/**********************************************************************************************************
**	函 数 名 : FanFaultStateRead()
**	功能说明 : 读取风扇故障状态
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void FanFaultStateRead()
{
	uint8_t fan_i = 0;
	static uint8_t FanReadNum = 0x06;
	
	/* 读取风扇故障状态 */
	if(CHANNEL_USE == 0x08)
		FanReadNum = 0x06;
	else if(CHANNEL_USE == 0x04)
		FanReadNum = 0x03;
	
	/* 读取风扇故障状态 */
	for(fan_i = 0; fan_i < FanReadNum; fan_i++)
	{
		if(GPIO_ReadInputPins(FanFaultPinTable[fan_i].GpioPort, FanFaultPinTable[fan_i].GpioPin) == 0x01)
		{
			bsp_DelayUS(10);
			
			if(GPIO_ReadInputPins(FanFaultPinTable[fan_i].GpioPort, FanFaultPinTable[fan_i].GpioPin) == 0x01)
			{
				IOTestInfo.FanFault[fan_i] = 1;		//故障	
				
				IP_Alarm_Flag[fan_i] |= IntelProtect_FHE;
			}
		}
		else
		{
			IOTestInfo.FanFault[fan_i] = 0;	//无故障
		}
	}
}

/**********************************************************************************************************
**	函 数 名 : BattVoerVoltStateRead()
**	功能说明 : 电池过压状态读取
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void BattVoerVoltStateRead()
{
	uint8_t batt_i = 0;
	
	/* 电池过压状态读取 */
	for(batt_i = 0; batt_i < CHANNEL_USE; batt_i++)
	{
		if(GPIO_ReadInputPins(BattOverVoltPortPinTab[batt_i].GpioPort, BattOverVoltPortPinTab[batt_i].GpioPin) == 0x01)
		{
			bsp_DelayUS(10);
			
			if(GPIO_ReadInputPins(BattOverVoltPortPinTab[batt_i].GpioPort, BattOverVoltPortPinTab[batt_i].GpioPin) == 0x01)
			{
				IOTestInfo.BattOverVolt[batt_i] = 1;
				
				IP_Alarm_Flag[batt_i] |= IntelProtect_LOV; //电池过压
				
				Chg_Dis_Off_Cfg(1 << batt_i); //关机
			}
		}
		else
		{
			IOTestInfo.BattOverVolt[batt_i] = 0;
		}
	}
}

/**********************************************************************************************************
**	函 数 名 : AllIOInfo_Read()
**	功能说明 : 读取风扇故障状态、电池过压信号以及功率板在位信号
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AllIOInfo_Read(void)
{
	MainPowerSignalRead();
	FanFaultStateRead();
	BattVoerVoltStateRead();
}


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
