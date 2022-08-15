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
Vers_Mana vers_mana;

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
void Version_Mana_Read(void)
{
	uint8_t i = 0;
	
	vers_mana.Bom_Vers = 0;
	vers_mana.Id_Vers = 0;
	vers_mana.Pcb_Vers = 0;
	
	/* GPIO PORT��ѯ�� */
	uint8_t GpioPort[8] = 
	{
		GPIO_PORT_MAINBOOM0, GPIO_PORT_MAINBOOM1, GPIO_PORT_MAINBOOM2, GPIO_PORT_MAINPCB0,
		GPIO_PORT_MAINPCB1, GPIO_PORT_MAINPCB2, GPIO_PORT_MAINID0, GPIO_PORT_MAINID1
	};
	
	/* GPIO PIN��ѯ�� */
	uint16_t GpioPin[8] = 
	{
		GPIO_PIN_MAINBOOM0, GPIO_PIN_MAINBOOM1, GPIO_PIN_MAINBOOM2, GPIO_PIN_MAINPCB0,
		GPIO_PIN_MAINPCB1, GPIO_PIN_MAINPCB2, GPIO_PIN_MAINID0, GPIO_PIN_MAINID1
	};
	
	/* ��ȡBOM����IO״̬ */
	for(i = 0; i < BOOM_IO_NUM; i++) //0 1 2
	{
		if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
		{
			bsp_DelayMS(10);
			
			if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
			{
				vers_mana.Bom_Vers += (1 << i);	//��¼BOM�汾 000 -> 111
				
				printf("vers_mana.Bom_Vers =%d\r\n",vers_mana.Bom_Vers);
			}
		}
	}
	
	/* ��ȡPCB����IO״̬ */
	for(i = 3; i < (3 + PCB_IO_NUM); i++) //3 4 5
	{
		if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
		{
			bsp_DelayMS(10);
			
			if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
			{
				vers_mana.Pcb_Vers += (1 << (i - 3));	//��¼PCB�汾 000 -> 111
				
				printf("vers_mana.Pcb_Vers = %d\r\n",vers_mana.Pcb_Vers);
			}
		}
	}
	
	/* ��ȡID����IO״̬ */
	for(i = 6; i < (5 + PCB_IO_NUM); i++) //6 7
	{
		if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
		{
			bsp_DelayMS(10);
			
			if(GPIO_ReadInputPins(GpioPort[i], GpioPin[i]) == 0x01)
			{
				vers_mana.Id_Vers += (1 << (i - 6));	//��¼����ID 00 -> 11
				
				printf("vers_mana.Id_Vers = %d\r\n",vers_mana.Id_Vers);
			}
		}
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
