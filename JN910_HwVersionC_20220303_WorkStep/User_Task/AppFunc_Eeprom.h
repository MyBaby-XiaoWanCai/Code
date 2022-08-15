/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : AppEeprom_Func.h
** Author      : Huang Cheng 
** Date        : 2022-03-08
** Description :
**********************************************************************************************************/

#ifndef _APP_FUNC_EEPROM_H_
#define _APP_FUNC_EEPROM_H_

#ifdef __cplusplus                  //C++����Ƕ��
extern "C" {
#endif
    
/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
#define SYNC_TYPE_GPROT		                0
#define SYNC_TYPE_IPROT		                1
#define SYNC_TYPE_CALI		                2

//EEPROM������������(ms)
#define EEPROM_TASK_PERIOD					1000				// (Configure)

//�ṹ����Ϣ(�����ṹ��ο����¸�ʽ)
// 0. У���ʶ
#define CALIBRATE_USAGE_FLAG_ADDR			0
#define CALIBRATE_USED						0x2323									// У׼��Ҫ������ģ�
#define CALIBRATE_UNUSED					0x0000

#define PROTECT_GLOBAL_USAGE_FLAG_ADDR		(2048 - 16)
#define PROTECT_GLOBAL_USED					0x030A
#define PROTECT_GLOBAL_UNUSED				0x0000

#define PROTECT_INTERNAL_USAGE_FLAG_ADDR	(2048 - 14)
#define PROTECT_INTERNAL_USED				0x030A
#define PROTECT_INTERNAL_UNUSED				0x0000

// 1. У׼�ṹ��
#define CALIBRATE_ID(x)						(0 + x)									// (Configure) У׼�ṹ��ID
#define CALIBRATE_STRUCT(x)					Tier1.calibrate[x]						// (Configure) У׼�ṹ�����
#define CALIBRATE_BASEADDR(x)				(2 + x * sizeof(CALIBRATE_STRUCT(x)))	// (Configure) У׼�ṹ�����ַ

// 1. ȫ�ֱ����ṹ��
#define PROTECT_GLOBAL_ID(x)				(8 + x)									//
#define PROTECT_GLOBAL_STRUCT(x)			Tier1.gProt[x]							//
#define PROTECT_GLOBAL_BASEADDR(x)			(2048 + x * 256)						//

// 2. �ڲ������ṹ��
#define PROTECT_INTERNAL_ID(x)				(16 + x)								//
#define PROTECT_INTERNAL_STRUCT(x)			Tier1.iProt[x]							//
#define PROTECT_INTERNAL_BASEADDR(x)		(4096 + x * 256)						//

//EEPROM��ȡ��������(����ṹ���С���ܳ���)
#define EEPROM_BUF_SIZE						4096									// (Configure)

//���û�����EEPROM��ȡ������صĺ�
#define EEP_DAT_LOAD(x)						EepDatLoad(x)							// ����ָ���ṹ��
#define EEP_DAT_SYNC(x)						EepDatSync(x)							// ����ָ���ṹ��

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/

/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	

/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/
extern uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
extern uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
extern uint8_t ee_WriteBytesWithCheck(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
extern void EepDatLoad(uint8_t id);
extern void EepDatSync(uint8_t id);    
    
    
#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

