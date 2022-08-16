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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "global.h"
#include "bsp.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
//Vers_Mana vers_mana __attribute__((at(0x1FFEEE8B)));
//Vers_Mana vers_mana;// __attribute__((at(0x200F0010))); //0x200F0000
uint8_t Bom_Vers = 0x00;
uint8_t Pcb_Vers = 0x00;
uint8_t Id_Vers = 0x00;
uint8_t LED_Ver = 0x00;
uint8_t Pow_Ver[8] = {0x00};

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/


/**********************************************************************************************************
**	�� �� �� : vVersion_Mana_Read()
**	����˵�� : ���ذ�BOOM��PCB��ID�汾��ȡ
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
extern HCPortPin_TypeDef VersionPinTable[MAIN_VerGpioNum];

void Version_Mana_Read(void)
{
    uint8_t i = 0;

    Bom_Vers = 0;
    Id_Vers = 0;
    Pcb_Vers = 0;

    /* ��ȡBOM����IO״̬ */
    for(i = 0; i < BOOM_IO_NUM; i++) //0 1 2
    {
        if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
        {
            bsp_DelayMS(5);

            if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
            {
                Bom_Vers += (1 << i);	//��¼BOM�汾 000 -> 111

//                printf("vers_mana.Bom_Vers =%d\r\n",vers_mana.Bom_Vers);
            }
        }
    }

    /* ��ȡPCB����IO״̬ */
    for(i = 3; i < (3 + PCB_IO_NUM); i++) //3 4 5
    {
        if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
        {
            bsp_DelayMS(5);

            if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
            {
                Pcb_Vers += (1 << (i - 3));	//��¼PCB�汾 000 -> 111

//                printf("vers_mana.Pcb_Vers = %d\r\n",vers_mana.Pcb_Vers);
            }
        }
    }

    /* ��ȡID����IO״̬ */
    for(i = 6; i < (5 + PCB_IO_NUM); i++) //6 7
    {
        if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
        {
            bsp_DelayMS(10);

            if(GPIO_ReadInputPins(VersionPinTable[i].GpioPort, VersionPinTable[i].GpioPin) == 0x01)
            {
                Id_Vers += (1 << (i - 6));	//��¼����ID 00 -> 11

//                printf("vers_mana.Id_Vers = %d\r\n",vers_mana.Id_Vers);
            }
        }
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
