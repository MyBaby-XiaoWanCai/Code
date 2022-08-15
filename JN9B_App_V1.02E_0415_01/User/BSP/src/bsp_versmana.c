/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_versmana.c
** Author      :  
** Date        : 2022-01-18
** Description : bsp_versmana source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "global.h"
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
Vers_Mana vers_mana;

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/


/**********************************************************************************************************
**	函 数 名 : vVersion_Mana_Read()
**	功能说明 : 主控板BOOM、PCB、ID版本读取
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void Version_Mana_Read(void)
{
	uint8_t i = 0;
	
	vers_mana.Bom_Vers = 0;
	vers_mana.Id_Vers = 0;
	vers_mana.Pcb_Vers = 0;
	
	/* GPIO PORT查询表 */
	uint8_t GpioPort[8] = 
	{
		GPIO_PORT_MAINBOOM0, GPIO_PORT_MAINBOOM1, GPIO_PORT_MAINBOOM2, GPIO_PORT_MAINPCB0,
		GPIO_PORT_MAINPCB1, GPIO_PORT_MAINPCB2, GPIO_PORT_MAINID0, GPIO_PORT_MAINID1
	};
	
	/* GPIO PIN查询表 */
	uint16_t GpioPin[8] = 
	{
		GPIO_PIN_MAINBOOM0, GPIO_PIN_MAINBOOM1, GPIO_PIN_MAINBOOM2, GPIO_PIN_MAINPCB0,
		GPIO_PIN_MAINPCB1, GPIO_PIN_MAINPCB2, GPIO_PIN_MAINID0, GPIO_PIN_MAINID1
	};
	
	/* 读取BOM管理IO状态 */
	for(i = 0; i < BOOM_IO_NUM; i++) //0 1 2
	{
		if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
		{
			bsp_DelayMS(10);
			
			if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
			{
				vers_mana.Bom_Vers += (1 << i);	//记录BOM版本 000 -> 111
				
				printf("vers_mana.Bom_Vers =%d\r\n",vers_mana.Bom_Vers);
			}
		}
	}
	
	/* 读取PCB管理IO状态 */
	for(i = 3; i < (3 + PCB_IO_NUM); i++) //3 4 5
	{
		if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
		{
			bsp_DelayMS(10);
			
			if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
			{
				vers_mana.Pcb_Vers += (1 << (i - 3));	//记录PCB版本 000 -> 111
				
				printf("vers_mana.Pcb_Vers = %d\r\n",vers_mana.Pcb_Vers);
			}
		}
	}
	
	/* 读取ID管理IO状态 */
	for(i = 6; i < (5 + PCB_IO_NUM); i++) //6 7
	{
		if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
		{
			bsp_DelayMS(10);
			
			if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
			{
				vers_mana.Id_Vers += (1 << (i - 6));	//记录单板ID 00 -> 11
				
				printf("vers_mana.Id_Vers = %d\r\n",vers_mana.Id_Vers);
			}
		}
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
