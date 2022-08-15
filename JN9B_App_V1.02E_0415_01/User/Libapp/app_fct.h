/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : fct.h
** Author      :  
** Date        : 2022-02-18
** Description : 
**********************************************************************************************************/

#ifndef _APP_FCT_H_
#define _APP_FCT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include <stdint.h>
#include "hc32_ddl.h"
#include "bsp.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
//FCT���̲���궨��
#define FCTSTEP_START 			(0x00)  //FCT��ʼ
#define FCT_REGINSIGNAL			(0x01)	//��λ�źż��
#define FCT_VERSION 				(0x02) 	//�汾���
#define FCT_LAMPPLATE				(0x03)	//�ư���
#define FCT_FAN							(0x04)	//���ȼ��
#define FCT_TMP							(0x05)	//�¶Ȳ���
#define FCT_CHARGEDISC	 		(0x06)	//��ŵ���� 1��2�š�3��4�š�5��6�š�7��8��
#define FCT_DISCCHARGE 			(0x07)	//�ų����� 1��2�䡢3��4�䡢5��6�䡢7��8��
#define FCT_VSENSEFAST			(0x08)	//Vsense���ٻ�����
#define FCT_VSENSESLOW			(0x09)	//Vsense���ٻ�����
#define FCTSTEP_END					(0x0A)	//FCT����
#define FCT_END_NOP					(0x0B)

#define FCT_EE_ADDR_A_1			(0x7BFF)//FCT_EE_ADDR_A - 1 = 0x7C00
#define FCT_EE_ADDR_A 			(0x7C00)

#define FCT_TEST_ITEM_NUM				(0x25)	//FCT��������

#define CHARG_VOLT1				(4)		//����ѹ
#define DISC_VOLT1				(1)		//�ŵ��ѹ
#define CHARG_CUR1				(40)	//������1
#define CHARG_CUR2				(130)	//������2
#define DISC_CUR1					(30)	//�ŵ����1
#define DISC_CUR2					(120)	//�ŵ����2

#define ID_VER					0x00
#define PCB_VER					0x04
#define BOM_VER					0x02
#define PB_VER					0x02
#define LED_VER					(vers_mana.LED_Ver)

#define DETECT_TASK_PERIOD		5		//������������(ms)
#define SIGNAL_PRO_TASK_PERIOD	2		//MSG_PRO������������(ms)
/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/


/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
extern uint8_t FCT_ReadBuf[64];
extern uint8_t FCT_WriteBuf[64];
extern uint8_t FCTEndFlg;

/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
uint8_t WholeMachine_FCT(void);

uint8_t WholeMachine_FCTSatart(void);	//FCT��ʼ
uint8_t WholeMachine_FCTReginSignal(void);	//��λ�źż��
uint8_t WholeMachine_FCTVersion(void); //�汾���
uint8_t WholeMachine_FCTLampPlate(void); //�ư����
uint8_t WholeMachine_FCTFan(void); //���Ȳ���
uint8_t WholeMachine_FCTTmp75(void);	//TMP75����
uint8_t WholeMachine_FCTChargeDisc(void); //��ŵ����
uint8_t WholeMachine_FCTDischCharge(void);	//�ų�����
uint8_t WholeMachine_FCTVsenseRapidLoop(void);	//���ٻ�����
uint8_t WholeMachine_FCTVsenseSlowLoop(void);	//���ٻ�����
uint8_t WholeMachine_FCTEnd(void);	//FCT����

uint8_t WholeMachine_FCTChaDisFunc_1(uint8_t CHx1,uint8_t CHx2); //��ŵ���Թ��ܺ���

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
