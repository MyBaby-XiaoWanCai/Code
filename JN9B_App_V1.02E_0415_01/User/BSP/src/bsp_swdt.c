/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_swdt.c
** Author      : Huang Cheng
** Date        : 2021-11-14
** Description : bsp_swdt source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_swdt.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : bsp_InitSWDT()
**	功能说明 : 初始化SWDT配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：专用看门狗最大喂狗时间 = 4096 / (10000/16) = 6.5536s
**********************************************************************************************************/
void bsp_InitSWDT(void)
{
    stc_swdt_init_t stcSwdtInit;

    /* SWDT configure */
    stcSwdtInit.u32CountCycle    = SWDT_COUNTER_CYCLE_4096;
    stcSwdtInit.u32ClockDivision = SWDT_CLOCK_DIV16;
    stcSwdtInit.u32RefreshRange  = SWDT_RANGE_0TO100PCT;
    stcSwdtInit.u32LPModeCountEn = SWDT_LPM_COUNT_STOP;
    stcSwdtInit.u32TrigType      = SWDT_TRIG_EVENT_RESET;
    (void)SWDT_Init(&stcSwdtInit);
	
		/* First reload counter to start SWDT */
    SWDT_Feed();
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
