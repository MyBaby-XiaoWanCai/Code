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
//Vers_Mana vers_mana __attribute__((at(0x1FFEEE8B)));
//Vers_Mana vers_mana;// __attribute__((at(0x200F0010))); //0x200F0000
uint8_t Bom_Vers = 0x00;
uint8_t Pcb_Vers = 0x00;
uint8_t Id_Vers = 0x00;
uint8_t LED_Ver = 0x00;
uint8_t Pow_Ver[8] = {0x00};

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
extern HCPortPin_TypeDef VersionPinTable[MAIN_VerGpioNum];

void Version_Mana_Read(void)
{
    uint8_t i = 0;

    Bom_Vers = 0;
    Id_Vers = 0;
    Pcb_Vers = 0;

    /* 读取BOM管理IO状态 */
    for(i = 0; i < BOOM_IO_NUM; i++) //0 1 2
    {
        if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
        {
            bsp_DelayMS(5);

            if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
            {
                Bom_Vers += (1 << i);	//记录BOM版本 000 -> 111

//                printf("vers_mana.Bom_Vers =%d\r\n",vers_mana.Bom_Vers);
            }
        }
    }

    /* 读取PCB管理IO状态 */
    for(i = 3; i < (3 + PCB_IO_NUM); i++) //3 4 5
    {
        if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
        {
            bsp_DelayMS(5);

            if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
            {
                Pcb_Vers += (1 << (i - 3));	//记录PCB版本 000 -> 111

//                printf("vers_mana.Pcb_Vers = %d\r\n",vers_mana.Pcb_Vers);
            }
        }
    }

    /* 读取ID管理IO状态 */
    for(i = 6; i < (5 + PCB_IO_NUM); i++) //6 7
    {
        if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
        {
            bsp_DelayMS(10);

            if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
            {
                Id_Vers += (1 << (i - 6));	//记录单板ID 00 -> 11

//                printf("vers_mana.Id_Vers = %d\r\n",vers_mana.Id_Vers);
            }
        }
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
