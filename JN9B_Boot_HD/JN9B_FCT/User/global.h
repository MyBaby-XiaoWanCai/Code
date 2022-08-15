/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : global.h
** Author      : Cai Heng
** Date        : 2021-07-28
** Description : 
**********************************************************************************************************/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "hc32_ddl.h"


/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/
#define CH_NUM						8

#define RUN_SEG_NUM					3
#define ABT_SEG_NUM					4
#define PRO_SEG_NUM					6


#define CHANNEL_MAX		            8
#ifdef YCJ
#define CHANNEL_CFG		8
#else
#define CHANNEL_CFG		4
#endif
#define CHANNEL_1                   1
#define CHANNEL_2                   2
#define CHANNEL_4                   4
#define CHANNEL_8                   8

/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/
typedef struct
{
	uint8_t cmd;
	float   dat;
}s_Tier1_ProtectStruct;

typedef struct
{
	uint8_t isEnable;
	float   value;
	float   max;
	float   min;
}s_Tier2_ProtectStruct;

/* ȫ�ֱ��� */
typedef struct
{
	s_Tier1_ProtectStruct VbatThrH;			// 1. ��ص�ѹ����
	s_Tier1_ProtectStruct VbatThrL;			// 2. ��ص�ѹ����
//	s_Tier1_ProtectStruct VloopDiff;		//  . ��·ѹ������
	s_Tier1_ProtectStruct Iovcharge;		// 3. ����������
	s_Tier1_ProtectStruct Iovdischg;		// 4. �ŵ��������
	s_Tier1_ProtectStruct Cov;				// 5. ������������
	s_Tier1_ProtectStruct VinitThrH;		// 6. ��ʼ��ѹ����
	s_Tier1_ProtectStruct VinitThrL;		// 7. ��ʼ��ѹ����
	s_Tier1_ProtectStruct Tcontact;			// 8. �Ӵ����ʱ��
	s_Tier1_ProtectStruct Icontact;			// 9. �Ӵ�������
	s_Tier1_ProtectStruct VcontactDiff;		//10. �Ӵ����ѹ��					
}s_Tier1_GlobalProtectStruct;

typedef struct
{
	s_Tier2_ProtectStruct VbatThrH;			// 1. ��ص�ѹ����
	s_Tier2_ProtectStruct VbatThrL;			// 2. ��ص�ѹ����
//	s_Tier2_ProtectStruct VloopDiff;		//  . ��·ѹ������
	s_Tier2_ProtectStruct Iovcharge;		// 3. ����������
	s_Tier2_ProtectStruct Iovdischg;		// 4. �ŵ��������
	s_Tier2_ProtectStruct Cov;				// 5. ������������
	s_Tier2_ProtectStruct VinitThrH;		// 6. ��ʼ��ѹ����
	s_Tier2_ProtectStruct VinitThrL;		// 7. ��ʼ��ѹ����
	s_Tier2_ProtectStruct Tcontact;			// 8. �Ӵ����ʱ��
	s_Tier2_ProtectStruct Icontact;			// 9. �Ӵ�������
	s_Tier2_ProtectStruct VcontactDiff;		//10. �Ӵ����ѹ��					
}s_Tier2_GlobalProtectStruct;

/* �ڲ��������ýṹ�� */
typedef struct
{
	s_Tier1_ProtectStruct IoutThr;			// 1. ���������ֵ
	s_Tier1_ProtectStruct VbusThrH;			// 2. ĸ�ߵ�ѹ����
	s_Tier1_ProtectStruct VbusThrL;			// 3. ĸ�ߵ�ѹ����
	s_Tier1_ProtectStruct IccDiff;			// 4. �����ε����������
	s_Tier1_ProtectStruct VcvDiff;			// 5. ��ѹ�ε�ѹ�������
	s_Tier1_ProtectStruct AmpVdrop;			// 6. ��·��ѹ�����½���ֵ
	s_Tier1_ProtectStruct CntVdrop;			// 7. ��·��ѹ�����½�����
	s_Tier1_ProtectStruct Vovsense;			// 8. sense��ѹ��������
	s_Tier1_ProtectStruct Totmod;			// 9. ģ����±�������
	s_Tier1_ProtectStruct Totenv;			// 10. �������±�������
    s_Tier1_ProtectStruct VloopDiff;        // 11.��·ѹ������
    s_Tier1_ProtectStruct AmpVup;           // 12.��ѹ����������ֵ
    s_Tier1_ProtectStruct CntVup;           // 13.��ѹ������������
    s_Tier1_ProtectStruct AmpCup;           // 14.��������������ֵ
    s_Tier1_ProtectStruct CntCup;           // 15.����������������
	s_Tier1_ProtectStruct VauxThr;			// 16.������ѹ����ֵ
    s_Tier1_ProtectStruct SNERR;            // 17.��ѹ�쳣�������ֵ
}s_Tier1_InternalProtectStruct;

typedef struct
{
	s_Tier2_ProtectStruct IoutThr;			// 1. ���������ֵ
	s_Tier2_ProtectStruct VbusThrH;			// 2. ĸ�ߵ�ѹ����
	s_Tier2_ProtectStruct VbusThrL;			// 3. ĸ�ߵ�ѹ����
	s_Tier2_ProtectStruct IccDiff;			// 4. �����ε����������
	s_Tier2_ProtectStruct VcvDiff;			// 5. ��ѹ�ε�ѹ�������
	s_Tier2_ProtectStruct AmpVdrop;			// 6. ��·��ѹ�����½���ֵ
	s_Tier2_ProtectStruct CntVdrop;			// 7. ��·��ѹ�����½�����
	s_Tier2_ProtectStruct Vovsense;			// 8. sense��ѹ��������
	s_Tier2_ProtectStruct Totmod;			// 9. ģ����±�������
	s_Tier2_ProtectStruct Totenv;			//10. �������±�������
    s_Tier2_ProtectStruct VloopDiff;        //11.��·ѹ������
    s_Tier2_ProtectStruct AmpVup;           //12.��ѹ����������ֵ
    s_Tier2_ProtectStruct CntVup;           //13.��ѹ������������
    s_Tier2_ProtectStruct AmpCup;           //14.��������������ֵ
    s_Tier2_ProtectStruct CntCup;           //15.���������������� 
    s_Tier2_ProtectStruct VauxThr;			//16.������ѹ����ֵ
	s_Tier2_ProtectStruct SNERR;            //17.��ѹ�쳣�������ֵ
}s_Tier2_InternalProtectStruct;

#pragma pack(1)
/* ��¼���� */
typedef struct
{
	int64_t  Tstamp;						// 1. ʱ���
	uint8_t  RunSta;						// 2. ����״̬
	uint8_t  StepType;						// 3. ��������
	uint16_t StepNumb;						// 4. ������
	float    Trun;							// 5. ����ʱ��(s)
	float    Vbat;							// 6. ��ص�ѹ(V)
	float    Iout;							// 7. �������(A)
	float    Vout;							// 8. �����ѹ(V)
	float    Vtz;							// 9. ̽���ѹ(V)
	float    Cbat;							//10. �������(Ah)
	float    Ebat;							//11. �������(VAs/J)
	uint16_t ModFlag;						//12. ģʽ��ʶ
	uint8_t  ErrType;						//13. ��������
	uint8_t  AbortType;						//14. ��ֹ����
}s_RecordStruct;
#pragma pack()

typedef struct channel_record_data_struct {
	float Trun;								// ����ʱ��      ms-����
	float Vbat;								// ��ص�ѹ      V-����
	float Iout;								// �������      A-����
	float Vout;								// �����ѹ      V-����
	float Cbat;								// �������      Ah-��ʱ
	float Ebat;								// �������      VAh
	float Tbat;								// ����¶�      ���϶�
	float Tmod;								// ģ���¶�      ���϶�
	float Zcont;							// �Ӵ��迹
	float Zloop;							// ��·�迹      
	float Vtz;								// ̽���ѹ
}s_ChannelRecordStruct;

#pragma pack(1)
typedef struct __attribute__ ((__packed__))
{
	uint8_t type;
	float   data;
}s_WorkStepSegmentStruct;;
#pragma pack()

#pragma pack(1)
/* �������ò�ѯ�ṹ�� */
typedef struct __attribute__ ((__packed__))	//���ֽڶ��� 
{
	uint16_t Numb;							// ������
	
	uint8_t  Type;							// ��������(����ģʽ)
	
	s_WorkStepSegmentStruct RunSeg[RUN_SEG_NUM];	
											// ������������
	s_WorkStepSegmentStruct AbtSeg[ABT_SEG_NUM];	
											// ������ֹ����
	s_WorkStepSegmentStruct ProSeg[PRO_SEG_NUM];	
											// ������������	
	
    uint8_t  Status;                 		//����״̬��ֹͣ����ͣ����������Ч��
}s_WorkStepStruct;
#pragma pack()

#pragma pack(1)
typedef struct
{
	uint16_t StepNumb;						// ������
	uint8_t  StepType;						// ��������(����ģʽ)
	float    StepRunVol;					// �������е�ѹֵ
	float    StepRunCur;					// �������е���ֵ
	float    StepRunRev;					// �������б���ֵ
	float    StepAbtTime;					// ������ֹʱ��ֵ
	float    StepAbtVol;					// ������ֹ��ѹֵ
	float    StepAbtCur;					// ������ֹ����ֵ
	float    StepAbtCap;					// ������ֹ����ֵ
	float    StepProVlimitH;				// ����������ѹ����ֵ
	float    StepProVlimitL;				// ����������ѹ����ֵ
	float    StepProIov;					// ������������ֵ
	float    StepProCov;					// ������������ֵ
	float    StepProVsuperdev;				// ����������ѹ����ֵ
	float    StepProIsuperdev;				// ����������������ֵ
	uint8_t  StepProVlimitH_En;				// ����������ѹ����ʹ��
	uint8_t  StepProVlimitL_En;	            // ����������ѹ����ʹ��
	uint8_t  StepProIov_En;		            // ������������ʹ��
	uint8_t  StepProCov_En;		            // ������������ʹ��
	uint8_t  StepProVsuperdev_En;	        // ����������ѹ����ʹ��
	uint8_t  StepProIsuperdev_En;	        // ����������������ʹ��
}s_WorkStepAnalyzeStruct;
#pragma pack()

typedef struct
{
	uint16_t StepNumb;						// ������
	uint8_t  StepType;						// ������������
}s_WorkStepControlStruct;

#pragma pack(1)
typedef struct
{
	uint8_t  RunSta;						// ����״̬			
	uint8_t  StepType;						// ��������
	uint16_t StepNumb;						// ������
	uint32_t Trun;							// ����ʱ��
	uint32_t Tpro;							// ����ʱ��
}s_WorkStepRunningStruct;
#pragma pack()

typedef struct
{
	uint8_t Flag;
	uint8_t Statu;
	uint8_t Type;
	float   Current;
	float   Voltage;
}s_CalibrationRunStruct;

/* У׼���� */
typedef struct
{
	float VsenseSetK_C;						// 1. ���-��ص�ѹ��׼�趨����K
	float VsenseSetB_C;						// 2. ���-��ص�ѹ��׼�趨����B
	float VmodSetK_C;						// 3. ���-ģ��˿ڵ�ѹ��׼�趨����K
	float VmodSetB_C;						// 4. ���-ģ��˿ڵ�ѹ��׼�趨����B
	float IhSetK_C;							// 5. ���-�ߵ�λ������׼�趨����K
	float IhSetB_C;							// 6. ���-�ߵ�λ������׼�趨����B
	float IlSetK_C;							// 7. ���-�͵�λ������׼�趨����K
	float IlSetB_C;							// 8. ���-�͵�λ������׼�趨����B
	float VsenseK_C;						// 9. ���-��ص�ѹ��������K
	float VsenseB_C;						//10. ���-��ص�ѹ��������B
	float VmodK_C;							//11. ���-ģ��˿ڵ�ѹ��������K
	float VmodB_C;							//12. ���-ģ��˿ڵ�ѹ��������B
	float IhK_C;							//13. ���-��������ߵ�λ��������K
	float IhB_C;							//14. ���-��������ߵ�λ��������B
	float IlK_C;							//15. ���-��������͵�λ��������K
	float IlB_C;							//16. ���-��������͵�λ��������B
	float VtzK_C;							//17. ���-̽���ѹ��������K
	float VtzB_C;							//18. ���-̽���ѹ��������B
	float VsenseSetK_D;						//19. �ŵ�-��ص�ѹ��׼�趨����K
	float VsenseSetB_D;						//20. �ŵ�-��ص�ѹ��׼�趨����B
	float VmodSetK_D;						//21. �ŵ�-ģ��˿ڵ�ѹ��׼�趨����K
	float VmodSetB_D;						//22. �ŵ�-ģ��˿ڵ�ѹ��׼�趨����B
	float IhSetK_D;							//23. �ŵ�-�ߵ�λ������׼�趨����K
	float IhSetB_D;							//24. �ŵ�-�ߵ�λ������׼�趨����B
	float IlSetK_D;							//25. �ŵ�-�͵�λ������׼�趨����K
	float IlSetB_D;							//26. �ŵ�-�͵�λ������׼�趨����B
	float VsenseK_D;						//27. �ŵ�-��ص�ѹ��������K
	float VsenseB_D;						//28. �ŵ�-��ص�ѹ��������B
	float VmodK_D;							//29. �ŵ�-ģ��˿ڵ�ѹ��������K
	float VmodB_D;							//30. �ŵ�-ģ��˿ڵ�ѹ��������B
	float IhK_D;							//31. �ŵ�-��������ߵ�λ��������K
	float IhB_D;							//32. �ŵ�-��������ߵ�λ��������B
	float IlK_D;							//33. �ŵ�-��������͵�λ��������K
	float IlB_D;							//34. �ŵ�-��������͵�λ��������B
	float VtzK_D;							//35. �ŵ�-̽���ѹ��������K
	float VtzB_D;							//36. �ŵ�-̽���ѹ��������B
}s_CalibrateStruct;

/* ����AD�ۼ�ֵ */
typedef struct
{
	uint32_t VsenseAdSum;					// 1. ��ص�ѹ����AD�ۼ�ֵ
	uint32_t VmodAdSum;						// 2. �˿ڵ�ѹ����AD�ۼ�ֵ
	uint32_t IoutAdSum;						// 3. �����������AD�ۼ�ֵ
	uint32_t VtzAdSum;						// 4. ̽���ѹ����AD�ۼ�ֵ
}s_AdSumStruct;

/* Debug�ṹ�� */
#pragma pack(1)
typedef struct
{
	uint8_t PortEnable;						
	uint8_t PortDir;
	float Vtemp;
	float Itemp;
}s_DebugSetDatStruct;
#pragma pack()

typedef struct
{
	float VmodDebug;
	float IoutDebug;
	float VsenseDebug;
	float TmodDebug;
	float VtzDebug;
}s_DebugInqDatStruct1;

typedef struct
{
	float Vbus1Debug; 
	float Vbus2Debug; 
	float VauxDebug;
	float TambDebug;
}s_DebugInqDatStruct2;

typedef struct
{	
	s_DebugSetDatStruct  set;				//0x21. ���Բ�����������/�ظ�
	s_DebugInqDatStruct1 inq1[8];			//0x22. �������ݲ�ѯ����/�ظ�
	s_DebugInqDatStruct2 inq2;
}g_DebugStruct;

typedef enum 
{
Neilsbed_Nofit=0x01, //δѹ��
Neilsbed_Fitting,    //ѹ����
Neilsbed_Fitted,      //��ѹ��
}Neilsbed_Status; 

/*ϵͳ������ṹ��*/
typedef struct
{
   uint8_t  Device_SN[3];       //SN����ֵ������mac��3�ֽڸ�ֵ
   Neilsbed_Status Neilsbed;    //̽��ѹ��״̬
}s_SystemStruct;

extern s_SystemStruct SystemStatus;
/* �ڲ��ṹ����� */
typedef struct
{
	float Vbus1K;							// 1. Bus1��ѹ����K
	float Vbus1B;							// 2. Bus1��ѹ����B
	float Vbus2K;							// 3. Bus2��ѹ����K
	float Vbus2B;							// 4. Bus2��ѹ����B
	float VauxK;							// 5. ������ѹ����K
	float VauxB;							// 6. ������ѹ����B
	float TambK;							// 7. �����¶Ȳ���K
	float TambB;							// 8. �����¶Ȳ���B
	float TmodK;							// 9. ģ���¶Ȳ���K
	float TmodB;							//10. ģ���¶Ȳ���B
}s_CalibrateExStruct;

typedef struct
{
    uint32_t VsenseIns[8];					// 1. Sense��ѹ˲ʱ(AD)ֵ(ÿ��������������)
    uint32_t IoutIns[8];					// 2. �������˲ʱ(AD)ֵ
    uint32_t VtzIns[8];						// 3. ̽���ѹ˲ʱ(AD)ֵ
    uint32_t VsenseSum[8];					// 4. Sense��ѹ�ۼ�(AD)ֵ(ÿ���������ڲ�����)
    uint32_t VmodSum[8];					// 5. �˿ڵ�ѹ�ۼ�(AD)ֵ
    uint32_t IoutSum[8];					// 6. ��������ۼ�(AD)ֵ
    uint32_t VtzSum[8];						// 7. ̽���ѹ�ۼ�(AD)ֵ
	float Vsense[8];						// 8. Sense��ѹ����ص�ѹ
	float Iout[8];							// 9. �������
	float Vmod[8];							//10. �˿ڵ�ѹ
	float Vtz[8];							//11. ̽���ѹ
	float Tbat[8];							//12. ����¶�
	float Tmod[8];							//13. ģ���¶�
	float Vbus1;							//14. ĸ��1ֱѹ
	float Vbus2;							//15. ĸ��2ֱѹ
	float Vaux;								//16. ������ѹ
	float Tamb;								//17. �����¶�
}s_SampleCalStruct;

typedef struct
{
	s_Tier1_GlobalProtectStruct   gProt[8];
	s_Tier1_InternalProtectStruct iProt[8];
//	s_RecordStruct                record[8];
	s_WorkStepStruct              workstep[8];
//	s_WorkStepCtrlStruct          worksetpCtrl[8];
	s_CalibrateStruct             calibrate[8];
//	s_AdSumStruct                 adSum[8];
//	s_CalibrateExStruct           calibrateEx;
}s_Tier1_Struct;

typedef struct
{
	s_Tier2_GlobalProtectStruct   gProt[8];
	s_Tier2_InternalProtectStruct iProt[8];	
	s_RecordStruct                record[8];
	s_ChannelRecordStruct         chRecord[8];
	s_WorkStepStruct              workstepCache1[8];
	s_WorkStepStruct              workstepCache2[8];
	s_WorkStepStruct              workstep[8];
	s_WorkStepAnalyzeStruct       workstepAnalyze[8];
	s_WorkStepControlStruct       workstepControl[8];
	s_WorkStepRunningStruct       workstepRunning[8];
	s_CalibrationRunStruct        calibrationRun[8];
	s_CalibrateStruct             calibrate[8];
	s_AdSumStruct                 adSum[8];
	g_DebugStruct                 debug;
	s_CalibrateExStruct           calibrateEx;
	s_SampleCalStruct             sampleCal;
}s_Tier2_Struct;

extern s_Tier1_Struct Tier1;
extern s_Tier2_Struct Tier2;



/*ACDC���ױ���֡*/
typedef struct __attribute__ ((__packed__))	//���ֽڶ���
{
 uint16_t FramHead;   						//֡ͷ,0x5B5B
 uint16_t Datelen;    						//���ݳ���=0x09��������֡ͷ֡β
 uint8_t FeatureCode; 						//�����룬����֡��ͬ
 uint8_t StateCode;   						//״̬�룬0��OK
 uint8_t ChanelFlag;  						//ͨ���룺0x01,0x02,0x03
 uint8_t CommandCode; 						//�����룺0x91
 uint8_t ChildCommand;						//��������:0x02~0x0B,�ο�ACDC������������
 uint16_t CRC16Code;  						//У���ֽ� 
 uint16_t FramTail;   						//֡β��0xA5A5
}acdc_alarmStruct;

/*������������������*/
typedef struct
{
  uint8_t ChildCom;
  uint16_t state;
}arr_list;

/*ACDC�����ظ��ṹ��*/
typedef struct {
uint32_t ID;								// can ID
uint8_t Date[8];							//can Date
__IO uint8_t processedFlg;					//�ظ������־��1��������0���Ѵ���  
}ACDC_UpdateStrcut;            

/* �ڲ�������������ö�� */
enum IntelProtect_state    
{
    IntelProtect_OLA = 0x01<<0,				// 1.�����������
    IntelProtect_BOV = 0x01<<1,				// 2.BUS��ѹ
    IntelProtect_BUV = 0x01<<2,				// 3.BUSǷѹ
    IntelProtect_APA = 0x01<<3,				// 4.�������ȸ澯
    IntelProtect_UPA = 0x01<<4,				// 5.��ѹ���ȸ澯
    IntelProtect_VDQ = 0x01<<5,				// 6.��·�����澯
    IntelProtect_MOT = 0x01<<6,				// 7.ģ�����
    IntelProtect_EOT = 0x01<<7,				// 8.��������
    IntelProtect_HPA = 0x01<<8,				// 9.Ӳ������
    IntelProtect_FHE = 0x01<<9,				// 10.���ȹ���
    IntelProtect_UHE = 0x01<<10,			// 11.CPU�쳣
    IntelProtect_C1E = 0x01<<11,			// 12.can1�쳣
    IntelProtect_C2E = 0x01<<12,			// 13.can2�쳣
    IntelProtect_DAE = 0x01<<13,			// 14.DA�쳣
    IntelProtect_ADE = 0x01<<14,			// 15.AD�쳣
    IntelProtect_FLE = 0x01<<15,			// 16.FLASH�쳣
    IntelProtect_EEE = 0x01<<16,			// 17.Eeprom�쳣  
    IntelProtect_FRE = 0x01<<17,			// 18.FRAM�쳣
    IntelProtect_PME = 0x01<<18,			// 19.��λ���쳣
    IntelProtect_VNN = 0x01<<19,			// 20.��ѹ�쳣
    IntelProtect_CVP = 0x01<<20,			// 21.��·ѹ���
    IntelProtect_VUQ = 0x01<<21,			// 22.��ѹ��������
    IntelProtect_CNN = 0x01<<22,			// 23.�����쳣
    IntelProtect_CUQ = 0x01<<23,			// 24.������������ 
    IntelProtect_LOV = 0x01<<24,			// 25.������ѹ�쳣��̽���ѹ����5.1V��      
    IntelProtect_MAC = 0x01<<25,            // 26.MAC�쳣�������벻����    
};


/* ȫ�ֱ�����������ö�� */
enum GlobalProtect_state
{
    GlobalProtect_OV = 0x01<<0,				//��ѹ����
    GlobalProtect_UV = 0x01<<1,				//Ƿѹ����
    GlobalProtect_COA = 0x01<<2,			//����������
    GlobalProtect_DOA = 0x01<<3,			//�ŵ��������
    GlobalProtect_OC  = 0x01<<4,			//���ݱ���
    GlobalProtect_MV = 0x01<<5,				//��ʼ��ѹ���ޱ���
    GlobalProtect_LV = 0x01<<6,				//��ʼ��ѹ���ޱ���
    IntelProtect_NONE1 = 0x01<<7,			//δ����
    IntelProtect_NONE2 = 0x01<<8,			//δ����
    IntelProtect_VCV = 0x01<<9,			//�Ӵ�����ѹ�仯��
    IntelProtect_OTE = 0x01<<10,			//��������
    IntelProtect_INV = 0x01<<11,			//��Ч 
};

/* �ڲ���������״̬-������ṹ�� */
typedef struct
{
   uint8_t childCMD;          //�澯��������
   uint32_t P_Flg ;           //�澯��־λ
   uint8_t  Channel;          //��ǰ�澯ͨ��
   uint8_t  LastChannel;      //�ϴθ澯ͨ��
}CMDProject;

/*������������֡*/
typedef struct __attribute__ ((__packed__))	//���ֽڶ���
{
 uint16_t FramHead;							//֡ͷ,0x5B5B
 uint16_t Datelen;							//���ݳ���=0x09��������֡ͷ֡β
 uint8_t FeatureCode;						//�����룬����֡��ͬ
 uint8_t StateCode;							//״̬�룬0��OK
 uint8_t ChanelFlag;						//ͨ���룺0x01,0x02,0x03
 uint8_t CommandCode;						//�����룺0x91
// uint8_t ChildCommand;						//��������:0x02~0x0B,�ο�ACDC������������
// uint16_t CRC16Code;						//У���ֽ� 
// uint16_t FramTail;							//֡β��0xA5A5
}protect_alarmStruct;

typedef struct __attribute__ ((__packed__))	//���ֽڶ���
{
	uint8_t  RunSta;						// ����״̬			
	uint8_t  StepType;						// ��������
	uint16_t StepNumb;						// ������
	uint32_t Trun;							// ����ʱ��
	uint32_t Tpro;							// ����ʱ��


}WorkStepstore;                                     //����״̬�洢�ṹ��

/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
extern uint8_t feed_hardware_watchdog_enable;



/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
extern void RunPer10ms(void);
extern void RunPer100ms(void);
extern void RunPer1000ms(void);


#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
