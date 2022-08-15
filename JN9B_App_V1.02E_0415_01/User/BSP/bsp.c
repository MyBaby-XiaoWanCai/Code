/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp.c
** Author      : Huang Cheng
** Date        : 2021-11-23
** Description : This file is used to initialize and drive the hardware 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
extern PcaDataType Pca9555_Data;
uint8_t Res_9548 = 2;
uint8_t Res_9555 = 3;
uint8_t xLedWriteData[2] = {0xFF,0x00}; /* {0xFF,0x00} LED2-9 亮红灯; {0x00,0xFF} LED2-9 亮绿灯 */

uint8_t xIOLEDVerConfigSet_1[2] = {0xFF,0xFF}; //灯板读版本PCA9555 IO设置
uint8_t xIOLEDVerConfigSet_2[2] = {0x3F,0xFF};

//uint8_t xPowPca9555_IOConfigSet_1[2] = {0xFF,0xE0};//输出模式配置 1-输入，0-输出 C033A、B
uint8_t xPowPca9555_IOConfigSet_1[2] = {0xFF,0xF0};//输出模式配置 1-输入，0-输出 C033C、D
uint8_t xPowPca9555_IOConfigSet_2[2] = {0x7F,0xF0};//输出模式配置 1-输入，0-输出
uint8_t xIOConfigWriteData[2] = {0x00,0x08};
extern Vers_Mana vers_mana;	 //主控板及灯板读取版本信息存储
extern AD7606_Object AD7606_A0;
extern AD7606_Object AD7606_A1;
extern st_dac_handle_t* Bovp;      //用来存储DAC初始化返回值
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void Peripheral_WE(void);
static void Peripheral_WP(void);
static void SystemClock_Config(void);
static void bsp_Init(void);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : Peripheral_WE()
**	功能说明 : MCU外设寄存器写使能函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void Peripheral_WE(void)
{
    /* Unlock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Unlock();
    /* Unlock PWC register: FCG0 */
    PWC_FCG0_Unlock();
    /* Unlock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Unlock(PWC_UNLOCK_CODE_0 | PWC_UNLOCK_CODE_1 | PWC_UNLOCK_CODE_2);

    /* Unlock all EFM registers */
    EFM_Unlock();
}

/**********************************************************************************************************
**	函 数 名 : Peripheral_WP()
**	功能说明 : MCU外设寄存器写保护函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void Peripheral_WP(void)
{
    /* Lock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Lock();
    /* Lock PWC register: FCG0 */
    PWC_FCG0_Lock();
    /* Lock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Lock(PWC_UNLOCK_CODE_0 | PWC_UNLOCK_CODE_1 | PWC_UNLOCK_CODE_2);
    /* Lock all EFM registers */
    EFM_Lock();
}

/**********************************************************************************************************
**	函 数 名 : PCA9555_Mode_Init()
**	功能说明 : 
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
uint8_t Pca9548ReadDat[8] = {0x00};

void PowerPCA9555_Mode_Init()
{
	uint8_t loop_ch = 0;
	
	GPIO_SetPins(I2C2_SMBA_GPIO,I2C2_SMBA_PIN);
	bsp_DelayMS(50);
	
	for(uint8_t ch_i = 0;ch_i < 8;ch_i++)
	{
		Res_9548 = Pca9548A_Select(I2C_PCA9548,ch_i,PCA_9555_ON);	//开机9548选择 通道1
		bsp_DelayUS(100);
		Pca9548A_Read_Ch(I2C_PCA9548,&Pca9548ReadDat[ch_i]);	//9548读 Test
		
		PCA9555_IOConfigReg(I2C_PCA9548,PCA9555_DEVICE_ADDR,xPowPca9555_IOConfigSet_1); //功率板PCA9555端口设置
		bsp_Tmp75Init(I2C_PCA9548,TMP75_ADDR);	//初始化功率板TMP75
		HardwareFaultTypeRead(ch_i);//功率板故障信号读取 - 清除中断、读取功率板版本号
		vers_mana.Pow_Ver[ch_i] = Pca9555_Data.Power_Board[ch_i].ReadData[0];	//功率板版本号缓存
		Pca9548A_Select(I2C_PCA9548,9,PCA_9555_ALLOFF);	//关闭所有通道
		bsp_DelayUS(100);
		
		//C033C、D版 IO方向重置
		Res_9548 = Pca9548A_Select(I2C_PCA9548,ch_i,PCA_9555_ON);	//开机9548选择 通道1
		PCA9555_IOConfigReg(I2C_PCA9548,PCA9555_DEVICE_ADDR,xPowPca9555_IOConfigSet_2); //功率板PCA9555端口输出方向重置
		Pca9548A_Select(I2C_PCA9548,9,PCA_9555_ALLOFF);	//关闭所有通道
		bsp_DelayUS(100);
		
		Feed_WDG();   
	}
	
	bsp_InitCharge();	//LM5170各控制端口初始化
	
	for(loop_ch = 0;loop_ch < 8;loop_ch++)
	{
		Disable_Mloop(loop_ch);	//MLoop为低，Loop为高
		bsp_DelayMS(2);
	}
	
	PCA9555_IOConfigReg(I2C_LED,PCA9555_VERDIAL_ADDR,xIOLEDVerConfigSet_1); //灯板1版本端口设置
	bsp_DelayMS(1);
	Pca9555_RegRead(I2C_LED,PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].ReadData);	//灯板1版本读取
	bsp_DelayMS(1);
	vers_mana.LED_Ver = Pca9555_Data.LED_Board[0].ReadData[0];	//灯板版本号缓存
	bsp_DelayMS(1);
	PCA9555_IOConfigReg(I2C_LED,PCA9555_VERDIAL_ADDR,xIOLEDVerConfigSet_2); //灯板PCA9555_1 IO方向重置
	PCA9555_ConfigOutputReg(I2C_LED,PCA9555_LED_ADDR); //灯板2设置为输出
	
	if((vers_mana.LED_Ver & 0xE0) == 0x00)	//8通道机型 000
	{
		used_channel_cfg = 8;	//通道数
	}
	else 
	if((vers_mana.LED_Ver & 0xE0) == 0x40)	//4通道机型 010
	{
		used_channel_cfg = 4;	//通道数
	}
	else if((vers_mana.LED_Ver & 0xE0) == 0x60)	//2通道机型
	{
		used_channel_cfg = 2;	//通道数
	}
	else if((vers_mana.LED_Ver & 0xE0) == 0x20)	//6通道机型
	{
		used_channel_cfg = 6;	//通道数
	}
}

/**********************************************************************************************************
**	函 数 名 : bsp_Init()
**	功能说明 : 初始化所有的硬件设备。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。只需要调用一次。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void bsp_Init(void)
{
	TIM6_4_PWMB_Config();	//初始化风扇PWM
	
	bsp_InitLed();
	bsp_InitI2C();
  bsp_Tmp75_AlertInit();         //初始化Alert脚    
  bsp_Tmp75Init(I2C_EE_TEMP,TMP75_ADDR);                         //初始化主控板TMP75
	PowerPCA9555_Mode_Init(); //功率板PCA9555初始化配置
	
	ADC_Switch_ConfigGPIO();
	bsp_InitUSART();
	bsp_InitRNG();
	bsp_GpioInit();
	IP_Scanning();
	bsp_dacx0508_init();
	
	/*检查设置条码MAC*/
	BarCode_Mac_CheckSet();  //条码MAC一致性检查并设置MAC后3字节
	W5500_RST_Init();
//	Lan_cfg();
	W5500_SpiInit();
//	bsp_InitW5500();

#ifdef C042A_7606
	bsp_AD7606_init();             //初始化7606
	bsp_SGM58601Init();            //初始化58601
	AD7606Reset(&AD7606_A0);
	AD7606Reset(&AD7606_A1);
	Bovp = bsp_bovpInit();	//初始化内部DAC
	AD5245_Vpro_Set(Bovp, 5.1f);	//设置内部DAC输出保护电压5.1v
#else
	bsp_AD7124Init();
	bsp_AD7175Init();
#endif

//	AD5245_Init();
	DMA_adc3_init();
	
	bsp_InitACDC();	//CANx初始化
//	bsp_InitSysTick(); 

	PowerFault_Init();	//功率板故障检测IO初始化

	TimerAUint1_Config(APP_TMRA1_PCLK_DIV, APP_TMRA1_PERIOD_VAL);  //TimerAUint1周期40ms
	TimerAUint2_Config(APP_TMRA2_PCLK_DIV, APP_TMRA2_PERIOD_VAL);  //TimerAUint2周期200ms
	TimerAUint3_Config(APP_TMRA3_PCLK_DIV, APP_TMRA3_PERIOD_VAL);  //TimerAUint3周期1ms
	TimerAUint4_Config(APP_TMRA4_PCLK_DIV, APP_TMRA4_PERIOD_VAL);  //TimerAUint4周期1ms
	
	TimerAUintStart(TMRA2_UNIT);
	TimerAUintStart(TMRA3_UNIT);
	TimerAUintStart(TMRA4_UNIT);

	Version_Mana_Read(); //主控板硬件版本读取
	AllIOInfo_Read();	//硬件检测IO信息读取

	RMU_CPULockUpCmd(Enable);
/******************************  并机测试 ***************************/
#if 0
	GPIO_ResetPins(GPIO_PORT_CVK_4,GPIO_PIN_CVK_4);
	GPIO_ResetPins(GPIO_PORT_CVK_8,GPIO_PIN_CVK_8);
	
	uint8_t MS_j = 0;
	for(MS_j = 0;MS_j < 8;MS_j++)
	{
		if(MS_j%2 == 0)
			Chg_Dis_Port_Cfg(Channel_Port_Table[MS_j], PORT_MODE_MASTER);	//奇数通道设置为主机模式
		else if(MS_j%2 == 1)
			Chg_Dis_Port_Cfg(Channel_Port_Table[MS_j], PORT_MODE_SLAVE);	//偶数通道设置为从机模式
	}
#endif
}

/**********************************************************************************************************
**	函 数 名 : sys_cfg_set()
**	功能说明 : 初始化系统各种默认参数配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void sys_cfg_set(void)
{
	
}

/**********************************************************************************************************
**	函 数 名 : Error_Handler()
**	功能说明 : 错误处理函数
**	形    参 : file - 源代码文件名称(关键字__FILE__表示源代码文件名)
**             line - 代码行号(关键字__LINE__表示源代码行号)
**	返 回 值 : 无
**********************************************************************************************************/
void Error_Handler(char *file, uint32_t line)
{
	printf("\r\nWrong parameters value: file %s on line %d\r\n", file, line);	
	
	/* 这是一个死循环，断言失败时程序会在此处死机，以便于用户查错 */
	if (line == 0)
	{
		return;
	}
	
	while(1)
	{
		
	}
}

/**********************************************************************************************************
**	函 数 名 : SystemClock_Config()
**	功能说明 : 初始化系统时钟
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：Clock source is XTAL   8MHz
               PLLHP_freq             ((PLL_source / PLLM) * PLLN) / PLLP = 240MHz
			   PLLM                   1
               PLLN                   120
               PLLP                   4
               PLLQ                   4
               PLLR                   4
               HCLK                   240MHz
               PCLK0                  240MHz
               PCLK1                  120MHz
               PCLK2                  60MHz
               PCLK3                  60MHz
               PCLK4                  120MHz
               EX_BUS                 120MHz
               EFM_WaitCycle          5
**********************************************************************************************************/
static void SystemClock_Config(void)
{
		stc_clk_pllh_init_t stcPLLHInit;
    stc_clk_xtal_init_t stcXtalInit;

    /* Configures XTAL. PLLH input source is XTAL. */
    (void)CLK_XtalStrucInit(&stcXtalInit);
    stcXtalInit.u8XtalState = CLK_XTAL_ON;
    stcXtalInit.u8XtalDrv   = CLK_XTALDRV_LOW;
    stcXtalInit.u8XtalMode  = CLK_XTALMODE_OSC;
    stcXtalInit.u8XtalStb   = CLK_XTALSTB_499US;
    (void)CLK_XtalInit(&stcXtalInit);

    /* PCLK0, HCLK  Max 240MHz */
    /* PCLK1, PCLK4 Max 120MHz */
    /* PCLK2, PCLK3 Max 60MHz  */
    /* EX BUS Max 120MHz */
    CLK_ClkDiv(CLK_CATE_ALL,                                        \
               (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 | \
                CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |  \
                CLK_HCLK_DIV1));

    (void)CLK_PLLHStrucInit(&stcPLLHInit);
    /* VCO = 8/1*120 = 960MHz*/
	 /*
     * PLLHP_freq 
	 *           = ((PLL_source / PLLM) * PLLN) / PLLP
     *           = (8 / 1) * 120 / 4
     *           = 240
	 * PLLP = PLLQ = PLLR -> PLLHP_freq = PLLHQ_freq = PLLHR_freq
     */
    stcPLLHInit.u8PLLState = CLK_PLLH_ON;
    stcPLLHInit.PLLCFGR    = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM = 1UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLN = 120UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLP = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLQ = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLR = 4UL - 1UL;

    /* stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLLSRC_XTAL; */
    (void)CLK_PLLHInit(&stcPLLHInit);
	
    /* Set EFM wait cycle. 5 wait cycles needed when system clock is 240MHz @ref User Guide's EFM */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE_5);

    (void)CLK_PLLHCmd(Enable);
    CLK_SetSysClkSrc(CLK_SYSCLKSOURCE_PLLH);
}

/**********************************************************************************************************
**  函 数 名 : User_Set_Vector()
**  功能说明 : 中断向量设置函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void User_Set_Vector(void)
{
	uint8_t APP_Copy_Flag = 0;
	MB85Ctrl_Read(APP_Copy_Flag_ADD,&APP_Copy_Flag,1);     //读取APP拷贝标志

	/*boot正常引导APP后 检查BOOT FLAG值，若为0xAA
	则拷贝中断向量表到APP偏移地址，APP程序正常使用中断*/
#if 1
	__disable_irq();
	SCB->VTOR = EFM_START_ADDR | 0x20000;          //设置中断向量表偏移量
	__enable_irq();
	while(APP_Copy_Flag !=0xAA)
	{
		APP_Copy_Flag=0xAA;
		MB85Ctrl_Write(APP_Copy_Flag_ADD,&APP_Copy_Flag,1);    //写APP拷贝标志
		Feed_WDG();                                            //喂狗
		MB85Ctrl_Read(APP_Copy_Flag_ADD,&APP_Copy_Flag,1);     //读取APP拷贝标志
		if(APP_Copy_Flag==0xAA)
			NVIC_SystemReset();	//系统重启
	}
#endif
}

/**********************************************************************************************************
**	函 数 名 : System_Init()
**	功能说明 : 系统初始化(主要包括MPU、Cache和Sys_Clk配置)
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void System_Init(void)
{
	Peripheral_WE();   //外设寄存器写使能
	SystemClock_Config();	//系统时钟配置
	GPIO_SetReadWaitCycle(GPIO_READ_WAIT_4);   //系统时钟240M，GPIO读操作需要等待4个时钟周期
	
	TimerAUintStop(TMRA1_UNIT);            //先停TIMA1
	NVIC_DisableIRQ(APP_TMRA1_IRQn);       //跳转前关单个中断，防止APP未使用该中断时再次开启总中断后找不到入口
	TimerAUintStop(TMRA2_UNIT);            //先停TIMA2
	NVIC_DisableIRQ(APP_TMRA2_IRQn);       //跳转前关单个中断，防止APP未使用该中断时再次开启总中断后找不到入口
	TimerAUintStop(TMRA3_UNIT);            //先停TIMA3
	NVIC_DisableIRQ(APP_TMRA3_IRQn);       //跳转前关单个中断，防止APP未使用该中断时再次开启总中断后找不到入口
	TimerAUintStop(TMRA4_UNIT);            //先停TIMA4
	NVIC_DisableIRQ(APP_TMRA4_IRQn);       //跳转前关单个中断，防止APP未使用该中断时再次开启总中断后找不到入口
	
	NVIC_DisableIRQ(AD7124_IRQn);          //关闭7124中断
	NVIC_DisableIRQ(AD7175_IRQn);          //关闭7175中断
	
	bsp_InitDWT();                             //初始化DWT
	bsp_InitSWDT();                            //初始化软件看门狗
	bsp_HWDG_InitGPIO();                       //初始化硬件看门狗IO

	Feed_WDG();                                //喂狗
	W25Q_MB85R_SpiInit();                      //初始化铁电 

	User_Set_Vector();                        //检查CP标志，设置中断向量偏移 20220228 屏蔽、暂不使用该功能	

	bsp_Init();	//bsp初始化
	
	Peripheral_WP();  //外设寄存器写保护
}

/**********************************************************************************************************
**	函 数 名 : bsp_Idle()
**	功能说明 : 空闲时执行的函数，一般主程序在for和while循环程序体中需要插入CPU_IDLE()宏来调用本函数。
**             本函数缺省为空操作，用户可以添加喂狗、设置CPU进入休眠模式等功能。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_Idle(void)
{
	/* 喂狗 */

	/* 让CPU进入休眠，由Systick定时中断唤醒或者其他中断唤醒 */
}

/**********************************************************************************************************
**  函 数 名 : Dis_int()
**  功能说明 : 关中断，运行时除外
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void Dis_int(void)
{
	NVIC_DisableIRQ(APP_TMRA1_IRQn); 
	NVIC_DisableIRQ(APP_TMRA2_IRQn);
	NVIC_DisableIRQ(APP_CAN1_IRQn);
	NVIC_DisableIRQ(APP_CAN2_IRQn); 	
	NVIC_DisableIRQ(Int012_IRQn);	//USART
	NVIC_DisableIRQ(Int013_IRQn);

	NVIC_DisableIRQ(SGM58601_IRQn);
	NVIC_DisableIRQ(AD7606_IRQn);
	NVIC_DisableIRQ(AD7606_Busy0_IRQn);
//	NVIC_DisableIRQ(AD7175_IRQn);
//	NVIC_DisableIRQ(AD7124_IRQn);

	NVIC_DisableIRQ(POWERFAULT_1_IRQn);
	NVIC_DisableIRQ(POWERFAULT_2_IRQn);
	NVIC_DisableIRQ(POWERFAULT_3_IRQn);
	NVIC_DisableIRQ(POWERFAULT_4_IRQn);
	NVIC_DisableIRQ(POWERFAULT_5_IRQn);
	NVIC_DisableIRQ(POWERFAULT_6_IRQn);
	NVIC_DisableIRQ(POWERFAULT_7_IRQn);
	NVIC_DisableIRQ(POWERFAULT_8_IRQn);

	NVIC_DisableIRQ(APP_TMRA3_IRQn);
	NVIC_DisableIRQ(APP_DMA_IRQn);
	NVIC_DisableIRQ(APP_TMRA4_IRQn);
	bsp_DelayUS(1);
}

/**********************************************************************************************************
**  函 数 名 : En_int()
**  功能说明 : 开中断，运行时除外
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void En_int(void)
{    
	NVIC_EnableIRQ(APP_TMRA1_IRQn); 
	NVIC_EnableIRQ(APP_TMRA2_IRQn);
	NVIC_EnableIRQ(APP_CAN1_IRQn);       
	NVIC_EnableIRQ(APP_CAN2_IRQn);
	NVIC_EnableIRQ(Int012_IRQn);
	NVIC_EnableIRQ(Int013_IRQn);

	NVIC_EnableIRQ(SGM58601_IRQn);
	NVIC_EnableIRQ(AD7606_IRQn);
	NVIC_EnableIRQ(AD7606_Busy0_IRQn);
//	NVIC_EnableIRQ(AD7175_IRQn);
//	NVIC_EnableIRQ(AD7124_IRQn);

	NVIC_EnableIRQ(POWERFAULT_1_IRQn);
	NVIC_EnableIRQ(POWERFAULT_2_IRQn);
	NVIC_EnableIRQ(POWERFAULT_3_IRQn);
	NVIC_EnableIRQ(POWERFAULT_4_IRQn);
	NVIC_EnableIRQ(POWERFAULT_5_IRQn);
	NVIC_EnableIRQ(POWERFAULT_6_IRQn);
	NVIC_EnableIRQ(POWERFAULT_7_IRQn);
	NVIC_EnableIRQ(POWERFAULT_8_IRQn);	
	
	NVIC_EnableIRQ(APP_DMA_IRQn);
	NVIC_EnableIRQ(APP_TMRA3_IRQn);
	NVIC_EnableIRQ(APP_TMRA4_IRQn);
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
