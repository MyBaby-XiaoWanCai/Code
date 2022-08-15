/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Can_Network_Transplant.h
** Author      : Cai Heng
** Date        : 2021-02-12
** Description : CAN����Э����ֲ�ļ�
**********************************************************************************************************/

#ifndef _CAN_NETWORK_TRANSPLANT_H_
#define _CAN_NETWORK_TRANSPLANT_H_

#ifdef __cplusplus
extern "C" {
#endif
    
/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
#define STD_FRM_ID							0x00001111
#define EXT_FRM_ID							0x11110000

#define DEV_TYPE_OFFSET						(26)			//bit 26
#define FEAT_CODE_OFFSET					(18)			//bit 18
#define FRAM_SEQ_OFFSET						(10)			//bit 10
#define DEV_ADDR_OFFSET						(2)				//bit 2
#define COM_TYPE_OFFSET						(1)				//bit 1
#define COM_DIR_OFFSET						(0)				//bit 0

#define DEV_TYPE_SIZE						(0x00000007)	//size 3
#define FEAT_CODE_SIZE						(0x000000FF)	//size 8
#define FRAM_SEQ_SIZE						(0x000000FF)	//size 8
#define DEV_ADDR_SIZE						(0x000000FF)	//size 8
#define COM_TYPE_SIZE						(0x00000001)	//size 1
#define COM_DIR_SIZE						(0x00000001)	//size 1

#define DEV_TYPE							(4)				//JN910
#define FEAT_CODE							(0)				//�ݲ�ʹ��
#define FRAM_SEQ							(0)				//�ݲ�ʹ��
//#define DEV_ADDR							(dev_addr)		//�Ͳ������
#define DEV_ADDR							(0)				//�Ͳ������
#define COM_TYPE_VOD						(1)				//�㲥
#define COM_TYPE_BROADCAST					(0)				//�㲥
#define COM_DIR_UP							(1)				//���з���(��λ������λ��)
#define COM_DIR_DOWN						(0)				//���з���(��λ������λ��)

#define M_VOD_ID							((DEV_TYPE << DEV_TYPE_OFFSET) | (DEV_ADDR << DEV_ADDR_OFFSET) | (COM_TYPE_VOD << COM_TYPE_OFFSET) | (COM_DIR_DOWN << COM_DIR_OFFSET))
#define M_BROADCAST_ID						((DEV_TYPE << DEV_TYPE_OFFSET) | (DEV_ADDR << DEV_ADDR_OFFSET) | (COM_TYPE_BROADCAST << COM_TYPE_OFFSET) | (COM_DIR_DOWN << COM_DIR_OFFSET))

#define APP_CAN1_AF1_ID						(M_VOD_ID)
#define APP_CAN1_AF1_ID_MSK					(~((DEV_TYPE_SIZE << DEV_TYPE_OFFSET) | (DEV_ADDR_SIZE << DEV_ADDR_OFFSET) | (COM_TYPE_SIZE << COM_TYPE_OFFSET) | (COM_DIR_SIZE << COM_DIR_OFFSET)))

#define APP_CAN1_AF2_ID                     (M_BROADCAST_ID)
#define APP_CAN1_AF2_ID_MSK					(~((DEV_TYPE_SIZE << DEV_TYPE_OFFSET) | (COM_TYPE_SIZE << COM_TYPE_OFFSET) | (COM_DIR_SIZE << COM_DIR_OFFSET)))


/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/


/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	


/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/
extern void Can_Data_Pack_Send(u32_t feature_id, u32_t id_type, u8_t *tx_dat, u16_t tx_len);
extern s8_t Can_Data_Pack_Analysis(u8_t *rx_dat, u16_t rx_len, u8_t* tx_dat, u16_t* tx_len);
extern void Err_Info_Pack(u8_t _no, u8_t* err_dat, u16_t* err_len, u32_t* _id);

#ifdef __cplusplus
}
#endif

#endif
