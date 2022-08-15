/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_task_protocol.h
** Author      : Cai Heng
** Date        : 2021-08-08
** Description :
**********************************************************************************************************/

#ifndef _APP_TASK_PROTOCOL_H_
#define _APP_TASK_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "hc32_ddl.h"
#include "cmsis_os2.h"
#include "bsp_lan.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
/* ֡ͷ֡β */
#define FRAME_HEAD_RECV						0x5A5A		/* ���ݷ�����λ�� -> ��λ�� */
#define	FRAME_TAIL_RECV						0xA5A5
#define FRAME_HEAD_SEND_OLD					0x5A5A		/* ���ݷ�����λ�� -> ��λ�� */
#define FRAME_HEAD_SEND_NEW					0x5B5B
#define	FRAME_TAIL_SEND_OLD					0xA5A5
#define	FRAME_TAIL_SEND_NEW					0xA5A5
/* ������� */
#define FRAME_IS_OK							0			/* ֡������ȷ */
#define FRAME_HEAD_ERR						1			/* ֡ͷ���� */
#define	FRAME_LEN_ERR						2			/* ֡������ */
#define FRAME_FEAT_ERR						3			/* ��������� */
#define FRAME_STATE_ERR						4			/* ״̬����� */
#define FRAME_CRC_ERR						5			/* У������� */
#define FRAME_TAIL_ERR						6			/* ֡β���� */
#define FRAME_PCMD_ERR						7			/* ����������� */
#define FRAME_SCMD_ERR						8			/* ����������� */
#define FRAME_PARAM_ERR						9			/* ֡�βδ��� */
#define FRAME_DATA_ERR						10			/* �������ݸ�ʽ���� */
/* ���ͷ�ʽ */
#define SEND_TOGETHER						0			/* �������� */
#define SEND_SEPARATE						1			/* �ְ����� */

/* �������� */
/* 1 ������ */
/* ȫ�ֱ��� */
#define	CMD_GLOBAL_PROTECT_SET				0x01		/* ȫ�ֱ����������� */
#define	CMD_GLOBAL_PROTECT_SET_REPLY		0x81		/* ȫ�ֱ������ûظ� */
#define	CMD_GLOBAL_PROTECT_INQ				0x02		/* ȫ�ֱ������ò�ѯ���� */
#define	CMD_GLOBAL_PROTECT_INQ_REPLY		0x82		/* ȫ�ֱ������ò�ѯ�ظ� */
#define	CMD_GLOBAL_PROTECT_TRIG				0x83		/* ȫ�ֱ����������� */
/* �ڲ����� */
#define CMD_INTERNAL_PROTECT_SET			0x0F		/* �ڲ������������� */
#define CMD_INTERNAL_PROTECT_SET_REPLY		0x8F		/* �ڲ��������ûظ� */
#define CMD_INTERNAL_PROTECT_INQ			0x10		/* �ڲ��������ò�ѯ���� */
#define CMD_INTERNAL_PROTECT_INQ_REPLY		0x90		/* �ڲ��������ò�ѯ�ظ� */
#define CMD_INTERNAL_PROTECT_TRIG			0xA0		/* �ڲ������������� */
/* AC/DC���� */
#define CMD_ACDC_PROTECT_TRIG				0xA1		/* AC/DC������������ */
/* 2 ��¼�� */
#define	CMD_RECORD_INQ						0x04		/* ��¼���ݲ�ѯ���� */
#define CMD_RECORD_INQ_REPLY				0x84		/* ��¼���ݲ�ѯ�ظ� */
/* 3 ������ */
#define CMD_WORKSTEP_SET					0x05		/* ������������ */
#define CMD_WORKSTEP_SET_REPLY				0x85		/* �������ûظ� */
#define CMD_WORKSTEP_RUNSTA_SET				0x06		/* ��������״̬�������� */
#define CMD_WORKSTEP_RUNSTA_SET_REPLY		0x86		/* ��������״̬���ûظ� */
#define CMD_WORKSTEP_INQ					0x07		/* ������ѯ���� */
#define CMD_WORKSTEP_INQ_REPLY				0x87		/* ������ѯ�ظ� */
#define CMD_WORKSTEP_PROTECT_TRIG			0x88		/* ���������������� */
/* 4 У׼�� */
#define	CMD_CALIBRATE_PARAM_SET				0x09		/* У׼�����趨���� */
#define	CMD_CALIBRATE_PARAM_SET_REPLY		0x89		/* У׼�����趨�ظ� */
#define	CMD_CALIBRATE_PARAM_INQ				0x0A		/* У׼������ѯ���� */
#define	CMD_CALIBRATE_PARAM_INQ_REPLY		0x8A		/* У׼������ѯ�ظ� */
#define	CMD_AD_DATA_INQ						0x0B		/* AD���ݶ�ȡ���� */
#define	CMD_AD_DATA_INQ_REPLY				0x8B		/* AD���ݶ�ȡ�ظ� */
/* 5 ϵͳ�� */
#define CMD_SYSTEM_INQ						0x0C		/* ϵͳ��ѯ���� */
#define CMD_SYSTEM_INQ_REPLY				0x8C		/* ϵͳ��ѯ�ظ� */
#define CMD_SYSTEM_SET						0x0D		/* ϵͳ�������� */
#define CMD_SYSTEM_SET_REPLY				0x8D		/* ϵͳ���ûظ� */
#define CMD_SYSTEM_UPDATE					0x0E		/* ϵͳ��������/�ظ� */
/* 6 ������ */
#define CMD_DEBUG_PARAM_SET					0x21		/* ���Բ�����������/�ظ� */
#define CMD_DEBUG_DATA_INQ					0x22		/* �������ݲ�ѯ����/�ظ� */
#define CMD_DEBUG_SYSTEM_SET				0x23		/* ����ϵͳ��������/�ظ� */
/* 7 ������ */
#define CMD_ACDC_UPDATE						0x60		/* AC/DC�������� */
#define CMD_ACDC_UPDATE_REPLY				0xE0		/* AC/DC�����ظ� */
/* 8 ��Ч�� */
#define	CMD_INVALID							0xFF		/* ��Ч���� */


/* ������ʧ��(����) */
#define SCMD_DISABLE						(1 << 7)

/* �������� */
/* 1 ������ */
/* ȫ�ֱ��� */
#define SCMD_GLOBAL_PROTECT_SET_BEGIN		0x00
#define SCMD_GLOBAL_PROTECT_SET_END			0x0B
#define SCMD_GLOBAL_PROTECT_NSET_BEGIN		(SCMD_GLOBAL_PROTECT_SET_BEGIN | SCMD_DISABLE)
#define SCMD_GLOBAL_PROTECT_NSET_END		(SCMD_GLOBAL_PROTECT_SET_END | SCMD_DISABLE)
#define	SCMD_GLOBAL_PROTECT_INQ_BEGIN		SCMD_GLOBAL_PROTECT_SET_BEGIN
#define SCMD_GLOBAL_PROTECT_INQ_END			SCMD_GLOBAL_PROTECT_SET_END
#define	SCMD_GLOBAL_PROTECT_TRIG_BEGIN		0x00
#define	SCMD_GLOBAL_PROTECT_TRIG_END		0x0C
/* �ڲ����� */
#define SCMD_INTERNAL_PROTECT_SET_BEGIN		0x00
#define SCMD_INTERNAL_PROTECT_SET_END		0x12
#define SCMD_INTERNAL_PROTECT_NSET_BEGIN	(SCMD_INTERNAL_PROTECT_SET_BEGIN | SCMD_DISABLE)
#define SCMD_INTERNAL_PROTECT_NSET_END		(SCMD_INTERNAL_PROTECT_SET_END | SCMD_DISABLE)
#define SCMD_INTERNAL_PROTECT_INQ_BEGIN		SCMD_INTERNAL_PROTECT_SET_BEGIN
#define SCMD_INTERNAL_PROTECT_INQ_END		SCMD_INTERNAL_PROTECT_SET_END
#define SCMD_INTERNAL_PROTECT_TRIG_BEGIN	0x00
#define SCMD_INTERNAL_PROTECT_TRIG_END		0x14
/* AC/DC���� */
#define SCMD_ACDC_PROTECT_TRIG_BEGIN		0x00
#define SCMD_ACDC_PROTECT_TRIG_END			0x0C
/* 3 ������ */
/* ������ */
#define SCMD_WORKSTEP_NUM_BEGIN				0x00
#define SCMD_WORKSTEP_NUM_END				0xFF
/* �������� */
#define SCMD_WORKSTEP_TYPE_BEGIN			0x00
#define SCMD_WORKSTEP_TYPE_STANDBY			0x01		/* ����     */
#define SCMD_WORKSTEP_TYPE_CC				0x02		/* CC       */
#define SCMD_WORKSTEP_TYPE_CV				0x03		/* CV       */
#define SCMD_WORKSTEP_TYPE_CCTOCV			0x04		/* CCtoCV   */
#define SCMD_WORKSTEP_TYPE_DCC				0x05		/* DCC      */
#define SCMD_WORKSTEP_TYPE_DCV				0x06		/* DCV      */
#define SCMD_WORKSTEP_TYPE_DCCTODCV			0x07		/* DCCtoDCV */
#define SCMD_WORKSTEP_TYPE_CP				0x08		/* CP       */
#define SCMD_WORKSTEP_TYPE_DCP				0x09		/* DCP      */
#define SCMD_WORKSTEP_TYPE_FINISH			0x0A		/* ����     */
#define SCMD_WORKSTEP_TYPE_END				0x0B
/* ����(����/��ֹ)�������� */
#define SCMD_WORKSTEP_PARAM_BEGIN			0x00
#define SCMD_WORKSTEP_PARAM_TIME			0x01		/* ʱ�� */
#define SCMD_WORKSTEP_PARAM_VOLTAGE			0x02		/* ��ѹ */
#define SCMD_WORKSTEP_PARAM_CURRENT			0x03		/* ���� */
#define SCMD_WORKSTEP_PARAM_CAPACITY		0x04		/* ���� */
//#define SCMD_WORKSTEP_PARAM_ELECTRIC		0x05		/* ���� */
#define SCMD_WORKSTEP_PARAM_END				0x05
/* ������������ */
#define SCMD_WORKSTEP_PROT_BEGIN			0x00
#define SCMD_WORKSTEP_PROT_VLIMITH			0x01		/* ��ѹ���ޱ��� */
#define SCMD_WORKSTEP_PROT_VLIMITL			0x02		/* ��ѹ���ޱ��� */
#define SCMD_WORKSTEP_PROT_OVI				0x03		/* ��������     */
#define SCMD_WORKSTEP_PROT_OVC				0x04		/* ���ݱ���     */
#define SCMD_WORKSTEP_PROT_VSUPERDEV		0x05		/* ��ѹ����� */
#define SCMD_WORKSTEP_PROT_ISUPERDEV		0x06		/* ��������� */
#define SCMD_WORKSTEP_PROT_END				0x0C
#define SCMD_WORKSTEP_NPROT_BEGIN			(SCMD_WORKSTEP_PROT_BEGIN | SCMD_DISABLE)
#define SCMD_WORKSTEP_NPROT_END				(SCMD_WORKSTEP_PROT_END | SCMD_DISABLE)
/* ��������״̬�������� */
#define WORKSTEP_RUNSTA_SET_STOP			0x00		/* ֹͣ */
#define WORKSTEP_RUNSTA_SET_PAUSE			0x01		/* ��ͣ */
#define WORKSTEP_RUNSTA_SET_START			0x02		/* ���� */
#define WORKSTEP_RUNSTA_SET_CONTINUE		0x03		/* ���� */
#define WORKSTEP_RUNSTA_SET_SKIP			0x04		/* ��ת */
#define WORKSTEP_RUNSTA_SET_END				0x05
/* ��������״̬�������� */
#define WORKSTEP_RUNSTA_REF_STOP			0x00		/* ֹͣ         */
#define WORKSTEP_RUNSTA_REF_PAUSE			0x01		/* ��ͣ         */
#define WORKSTEP_RUNSTA_REF_RUNNING			0x02		/* ������       */
#define WORKSTEP_RUNSTA_REF_COMPLETE		0x03		/* ���         */
#define WORKSTEP_RUNSTA_REF_START			0x04		/* �������̿�ʼ */
#define WORKSTEP_RUNSTA_REF_WAIT			0x05		/* �ȴ�         */
#define WORKSTEP_RUNSTA_REF_CONTACT			0x06		/* �Ӵ�����     */

/* 4 У׼�� */
#define SCMD_CALIBRATE_PARAM_SET_BEGIN		0x00
#define SCMD_CALIBRATE_PARAM_SET_END		0x25
#define SCMD_CALIBRATE_PARAM_INQ_BEGIN		SCMD_CALIBRATE_PARAM_SET_BEGIN
#define SCMD_CALIBRATE_PARAM_INQ_END		SCMD_CALIBRATE_PARAM_SET_END
#define	SCMD_AD_DATA_INQ_BEGIN				0x00
#define	SCMD_AD_DATA_INQ_END				0x05
/* 5 ϵͳ�� */
#define SCMD_SYSTEM_INQ_BEGIN				0x00
#define SCMD_SOFTWARE_VERSION				0x01		/* ����汾��ѯ */
#define SCMD_WARMMING_MESSAGE				0x02		/* �澯��Ϣ��ѯ */
#define SCMD_SYSTEM_MODE  					0x03        /* ϵͳģʽ��ѯ */
#define SCMD_SYSTEM_CLIENT					0x04        /* �ͻ���Զ�����ò�ѯ */
#define SCMD_SYSTEM_MAC_Q   	    	    0x05        /* MAC���ò�ѯ */
#define SCMD_SYSTEM_BAR_Q    	    	    0x06        /* �������ò�ѯ */
#define SCMD_SYSTEM_FRAM_Q  	    	    0x07        /* �������ò�ѯ */
#define SCMD_SYSTEM_EE_Q  	    	        0x08        /* EE���ò�ѯ */
#define SCMD_SYSTEM_FLASH_Q  	    	    0x09        /* flash���ò�ѯ */
#define SCMD_SYSTEM_PG_Q  	    	        0x0a        /* PG״̬��ѯ */
#define SCMD_SYSTEM_AC_Q  	    	        0x0b        /* AC���ѹ��ѯ */
#define SCMD_SYSTEM_ACDC_VERSION_Q  	    0x0c        /* ACDC�汾��ѯ */
#define SCMD_SYSTEM_INQ_END					0x0d

#define SCMD_SYSTEM_SET_BEGIN				0x00
#define SCMD_SYSTEM_SET_END					0x14
#define SCMD_SYSTEM_UPDATE_BEGIN			0x00
#define SCMD_SYSTEM_UPDATE_END				0x06
/* 6 ������ */
#define SCMD_DEBUG_SYSTEM_SET_BEGIN			0x01
#define SCMD_DEBUG_SYSTEM_SET_END			0x03
/* 7 ������ */
#define SCMD_ACDC_UPDATE_SET_BEGIN          0x01    	/*����������������ʼֵ*/
#define SCMD_ACDC_UPDATE_SET_END            0x03    	/*�����������������ֵ*/
#define SCMD_ACDC_UPDATE_INQ_BEGIN          0x01    	/*�������ûظ���������ʼֵ*/
#define SCMD_ACDC_UPDATE_INQ_END            0x03    	/*�������ûظ����������ֵ*/
/* 8 ��Ч�� */
#define	SCMD_INVALID						0xFF		/* ��Ч���� */

#ifdef YCJ
/* �汾�� */
#define SOFTWARE_PRIVERSION					0x0C	/* ���汾�� */
#define SOFTWARE_SUBVERSION					0x1B	/* �Ӱ汾�� */
#else
/* �汾��(V2.28) */
#define SOFTWARE_PRIVERSION					0x02	/* ���汾�� */
#define SOFTWARE_SUBVERSION					0x1C	/* �Ӱ汾�� */
#endif

/* ��λ����λ */
#define SET_0(x, y)							(x &= ~(1 << y))
#define SET_1(x, y)							(x |=  (1 << y))

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/
typedef struct
{
	uint16_t head;					/* ����֡ͷ  */
	uint16_t len;					/* ���ݳ���  */
	uint8_t  feat;					/* ������    */
	uint8_t  sta;					/* ״̬��    */
	uint8_t  chEn;					/* ͨ����־  */
	uint16_t crc;					/* CRCУ��ֵ */
	uint16_t tail;					/* ����֡β  */
} s_ProtocolStruct;

/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/
extern osSemaphoreId_t w5500_rcv_Semaphore;        /* ���ڽ����ź���ID */
extern rcv_Struct w5500_rcvbuf;

extern uint8_t workstep_flag_RunstaSet;
extern uint8_t workstep_flag_AbortPrio[8];
	
extern uint8_t workstep_flag_CriticalZone[8]; 

extern uint8_t heart_beat_flag;
	
extern uint8_t record_data_upcast_flag[8];

extern uint8_t err_send_flag;

/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/
extern uint16_t CRC16N(uint8_t *s, uint16_t len);

extern void RecordDatSync(uint8_t chx);

extern void RecordDatUpcast(uint8_t chx, uint8_t sync_flag);

extern void WorkstepProtUpcast(uint8_t chx);

extern void AppTaskProtocol(void *argument);


#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
