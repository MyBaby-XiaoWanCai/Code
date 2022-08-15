/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Dcdc.h
** Author      : Huang Cheng
** Date        : 2022-01-20
** Description :
**********************************************************************************************************/

#ifndef _TASK_DCDC_H_
#define _TASK_DCDC_H_

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
#define PROTECT_LIMIT_VOL       0x01      //�����ѹ����
#define PROTECT_LIMIT_CURT      0x02      //�����������

#define TRIGGER_IND_CURT        0x01      //��е���������������
#define TRIGGER_OUT_VOL         0x02      //�����ѹ��������
#define TRIGGER_OUT_CURT        0x03      //���������������
#define TRIGGER_BUS_HIGH        0x04      //����BUS�߹��ϴ���
#define TRIGGER_BUS_LOW         0x05      //����BUS�͹��ϴ���
#define TRIGGER_HARDWARE        0x06      //Ӳ�����ϴ���
#define TRIGGER_FAN             0x07      //���ȹ��ϴ���
#define TRIGGER_WDG_RESET       0x08      //���Ź���λ���ϴ���
#define TRIGGER_REV_CONNET      0x09      //���ӱ�������

#define RECORD_RUN_STATUS       0x01      //����״̬�����ģʽ������
#define RECORD_SENSE_VOL        0x02      //Sense��ѹ
#define RECORD_OUT_CURT         0x03      //�������
#define RECORD_OUT_VOL          0x04      //�����ѹ
#define RECORD_BUS_VOL          0x05      //BUS��ѹ
#define RECORD_IL1_CURT         0x06      //IL1����
#define RECORD_IL2_CURT         0x07      //IL2����
#define RECORD_TEMP_VAL         0x08      //�¶�
#define RECORD_DUTY_CYCLE1      0x09      //ռ�ձ�1
#define RECORD_DUTY_CYCLE2      0x0A      //ռ�ձ�2

#define CTRL_RUN_MODE           0x01      //����ģʽ
#define CTRL_RATE_VOL           0x02      //������ѹ
#define CTRL_RATE_CURT          0x03      //��������
#define CTRL_RUN_CC_MODE        0x01      //CCģʽ
#define CTRL_RUN_CV_MODE        0x02      //CVģʽ
#define CTRL_RUN_DCC_MODE       0x03      //DCCģʽ
#define CTRL_RUN_DCV_MODE       0x04      //DCVģʽ
#define RUN_STATUS              0x01      //����״̬
#define STOP_STATUS             0x02      //ֹͣ״̬
#define CLEAR_FAULT             0x03      //�������

#define CLIB_SENSE_VOL_K        0x01      //Sense��ѹУ׼ϵ��K
#define CLIB_SENSE_VOL_B        0x02      //Sense��ѹУ׼ϵ��B
#define CLIB_PORT_VOL_K         0x03      //�˿ڵ�ѹУ׼ϵ��K
#define CLIB_PORT_VOL_B         0x04      //�˿ڵ�ѹУ׼ϵ��B
#define CLIB_CHARG_CURT_K       0x05      //��������ѹУ׼ϵ��K
#define CLIB_CHARG_CURT_B       0x06      //��������ѹУ׼ϵ��B
#define CLIB_DISCHARG_CURT_K    0x07      //�ŵ������ѹУ׼ϵ��K
#define CLIB_DISCHARG_CURT_B    0x08      //�ŵ������ѹУ׼ϵ��B
#define CLIB_PROBE_VOL_K        0x09      //̽���ѹУ׼ϵ��K
#define CLIB_PROBE_VOL_B        0x0A      //̽���ѹУ׼ϵ��B
#define CLIB_BAT_ADC_VAL        0x01      //��ص�ѹ����AD�ۼ�ֵ
#define CLIB_PORT_ADC_VAL       0x02      //�˿ڵ�ѹ����AD�ۼ�ֵ
#define CLIB_CURT_ADC_VAL       0x03      //�����������AD�ۼ�ֵ
#define CLIB_TEMP_ADC_VAL       0x04      //�����¶Ȳ���AD�ۼ�ֵ
#define CLIB_PROBE_ADC_VAL      0x05      //̽���ѹ����AD�ۼ�ֵ

#define SYSTEM_BASE_INF         0x01      //ϵͳ������Ϣ
#define SYSTEM_ALARM_INF        0x02      //ϵͳ�澯��Ϣ
#define SYSTEM_ALARM_CLR        0x01      //ϵͳ�澯���
#define SYSTEM_HEART_BEAT       0x02      //������
#define SYSTEM_MODE_SET         0x03      //ϵͳģʽ����
#define SYSTEM_JUMP_BOOT        0x01      //��תbootģʽ
#define SYSTEM_REQUEST_BOOT     0x02      //��ѯboot��־
#define SYSTEM_ERASE_FLASH      0x03      //����flash
#define SYSTEM_WRITE_FLASH      0x04      //дflash
#define SYSTEM_JUMP_APP         0x05      //��תAPPģʽ

#define INVLID_SUB_CMD          0xFF      //��Ч��������
#define CHANNEL_NUM             0x04      //DCDC��ͨ����

#define RECV_MSG_ID             0x00000001

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/
#pragma pack(1)
typedef struct
{
	uint8_t ChFlag;           //ͨ����־
	uint8_t MainCmd;          //��������
	uint8_t SubCmd;           //��������
	uint8_t Data[4];          //����
	uint8_t Crc;              //У���
}DCDC_FrameType;
#pragma pack()

typedef enum
{
	ProtectSet    = 0x01,     //��������
	ProtectSetInq = 0x02,     //�������ò�ѯ
	RecordDataInq = 0x04,     //��¼���ݲ�ѯ
	CtrlParamSet  = 0x05,     //���Ʋ�������
	RunStaSet     = 0x06,     //����״̬����
	CtrlParamInq  = 0x07,     //���Ʋ�����ѯ
	ClibParamSet  = 0x08,     //У׼��������
	ClibParamInq  = 0x09,     //У׼������ѯ
	AdcDataRead   = 0x0A,     //AD���ݶ�ȡ
	SystemInq     = 0x0B,     //ϵͳ��ѯ
	SystemSet     = 0x0C,     //ϵͳ����
	SystemUpdate  = 0x0D,     //ϵͳ��������
}DCDC_SetFrameType;

typedef enum
{
	ProtectSetRep    = 0x81,   //����������Ӧ
	ProtectInqRep    = 0x82,   //�������ò�ѯ�ظ�
	ProtectTrig      = 0x83,   //��������
	RecordDataInqRep = 0x84,   //��¼���ݲ�ѯ�ظ�
	CtrlParamSetRep  = 0x85,   //���Ʋ���������Ӧ
	RunStaSetRep     = 0x86,   //����״̬������Ӧ
	CtrlParamInqRep  = 0x87,   //���Ʋ�����ѯ�ظ�
	ClibParamSetRep  = 0x88,   //У׼����������Ӧ
	ClibParamInqRep  = 0x89,   //У׼������ѯ�ظ�
	AdcDataReadRep   = 0x8A,   //AD���ݶ�ȡ�ظ�
	SystemInqRep     = 0x8B,   //ϵͳ��ѯ�ظ�
	SystemSetRep     = 0x8C,   //ϵͳ������Ӧ
}DCDC_RepFrameType;

typedef enum
{
//	SET_OK    = 0x00,
//	SET_NOTOK = !SET_OK,       //�°汾Э��
	SET_NOTOK = 0x00,
	SET_OK    = !SET_NOTOK,    //�ɰ汾Э��
}DCDC_SetRepSta;

typedef enum
{
	FrameOk = 0x00,
	CmdErr,
	CrcErr,
}DCDC_FrameValidType;

#pragma pack(1)
typedef struct
{
	float    OvpVal[4];      //�����ѹ����ֵ���ĸ�ͨ��
	float    OcpVal[4];      //�����������ֵ���ĸ�ͨ��
	uint16_t ProtectAlarm[4];//�����澯����bit�������ĸ�ͨ��
	uint8_t  RunSta[4];      //����״̬���ĸ�ͨ��
	uint8_t  RunMode[4];     //����ģʽ���ĸ�ͨ��
	uint16_t Fault[4];       //���ϼ�¼���ĸ�ͨ��
	float    SenseVol[4];    //Sense��ѹ���ĸ�ͨ��
	float    OutCurt[4];     //����������ĸ�ͨ��
	float    OutVol[4];      //�����ѹ���ĸ�ͨ��
	float    BusVol[4];      //BUS��ѹ���ĸ�ͨ��
	float    IL1Curt[4];     //IL1�������ĸ�ͨ��
	float    IL2Curt[4];     //IL2�������ĸ�ͨ��
	float    TempVal;        //�¶�
	float    DutyCycle1[4];  //ռ�ձ�1���ĸ�ͨ��
	float    DutyCycle2[4];  //ռ�ձ�2���ĸ�ͨ��
	float    RatedVol[4];    //���ѹ���ĸ�ͨ��
	float    RatedCurt[4];   //��������ĸ�ͨ��
	float    SenseK[4];      //SenseУ׼ϵ��K���ĸ�ͨ��
	float    SenseB[4];      //SenseУ׼ϵ��B���ĸ�ͨ��
	float    PortVolK[4];    //�˿ڵ�ѹУ׼ϵ��K���ĸ�ͨ��
	float    PortVolB[4];    //�˿ڵ�ѹУ׼ϵ��B���ĸ�ͨ��
	float    ChCurtK[4];     //������У׼ϵ��K���ĸ�ͨ��
	float    ChCurtB[4];     //������У׼ϵ��B���ĸ�ͨ��
	float    DisChCurtK[4];  //�ŵ����У׼ϵ��K���ĸ�ͨ��
	float    DisChCurtB[4];  //�ŵ����У׼ϵ��B���ĸ�ͨ��
	float    ProbeVolK[4];   //̽���ѹУ׼ϵ��K���ĸ�ͨ��
	float    ProbeVolB[4];   //̽���ѹУ׼ϵ��B���ĸ�ͨ��
	uint16_t BatVolAdc[4];   //��ص�ѹAD�ۼ�ֵ���ĸ�ͨ��
	uint16_t PortVolAdc[4];  //�˿ڵ�ѹAD�ۼ�ֵ���ĸ�ͨ�� 
	uint16_t TempAdc[4];     //�¶�AD�ۼ�ֵ���ĸ�ͨ��
	uint16_t ProbeVolAdc[4]; //̽���ѹAD�ۼ�ֵ���ĸ�ͨ�� 
	uint8_t  VerNum[4];      //����汾��
	
}DCDC_ParamType;
#pragma pack()

/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/
extern DCDC_FrameType DcdcRecvBuffer;
extern DCDC_FrameType DcdcSendFrame;
extern DCDC_FrameType DcdcRecvFrame;
extern DCDC_ParamType DcdcParam;

extern uint32_t DcdcCanId;  //DCDC CANͨ��ID
extern uint32_t MaskCanId;
extern uint8_t dcdc_err_flag;

/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/
void DcdcRunModeSet(uint8_t ChNum, uint8_t RunMode);
void DcdcAdcDataRead(uint8_t ChNum, uint8_t ParaType);
void DcdcSystemSet(uint8_t ParaType);
void DcdcSystemUpdate(uint8_t ParaType);
void DcdcClibParamSet(uint8_t ChNum, uint8_t ParaType, float ClibPara);
void DcdcProtectSet(uint8_t ChNum, uint8_t ProtectType, float ProtectPara);
void DcdcRunStaSet(uint8_t ChNum, uint8_t RunSta);
void DcdcCtrlParaSet(uint8_t ChNum, uint8_t ParaType, float RunPara);
void DcdcParaInq(uint8_t ChNum, uint8_t MainCmdType, uint8_t SubCmdType);
void DcdcRecvHandle(void);
extern void AppTaskDcdc(void *argument);
    
#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
