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


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/

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
 void Peripheral_WE(void)
{
    /* Unlock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Unlock();
    /* Unlock PWC register: FCG0 */
    PWC_FCG0_Unlock();
    /* Unlock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Unlock(PWC_UNLOCK_CODE_0 | PWC_UNLOCK_CODE_1 | PWC_UNLOCK_CODE_2);
  
    /* Unlock all EFM registers */
    EFM_Unlock();                    //解除所有的EFM寄存器写保护  
    /* Unlock EFM register: FWMC */
    EFM_FWMC_Unlock();               //解除FWMC寄存器写保护，FWMC寄存器：配置编程擦除模式，总线状态等  

}

/**********************************************************************************************************
**	函 数 名 : Peripheral_WP()
**	功能说明 : MCU外设寄存器写保护函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
 void Peripheral_WP(void)
{
    /* Lock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Lock();
    /* Lock PWC register: FCG0 */
    PWC_FCG0_Lock();
    /* Lock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Lock(PWC_UNLOCK_CODE_0 | PWC_UNLOCK_CODE_1 | PWC_UNLOCK_CODE_2);
    
    /* lock EFM register: FWMC */
    EFM_FWMC_Lock();               //解除FWMC寄存器写保护，FWMC寄存器：配置编程擦除模式，总线状态等  
    /* lock all EFM registers */
    EFM_Lock();                    //解除所有的EFM寄存器写保护  

    
    
   
}

/**********************************************************************************************************
**	函 数 名 : bsp_Init()
**	功能说明 : 初始化所有的硬件设备。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。只需要调用一次。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void bsp_Init(void)
{
	bsp_InitDWT();                 //初始化DWT
	bsp_InitLed();                 //初始化LED灯IO 
	bsp_InitSWDT();                //初始化软件看门狗
	bsp_FR_ADDR_InitGPIO();        //初始化前面板拨码IO
	bsp_HWDG_InitGPIO();           //初始化硬件看门狗
	W25Q_MB85R_SpiInit();          //初始化铁电flash公用SPI接口
	TimerAUint1_Config(APP_TMRA1_PCLK_DIV, APP_TMRA1_PERIOD_VAL);  //设置TimerAUint1周期8ms    
    TimerAUintStart(TMRA1_UNIT);                                   //开启TimerAUint1

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
**	函 数 名 : System_Init()
**	功能说明 : 系统初始化(主要包括MPU、Cache和Sys_Clk配置)
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void System_Init(void)
{

	
	SystemClock_Config();   //配置系统时钟
	bsp_Init();             //配置系统外设
	

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
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
