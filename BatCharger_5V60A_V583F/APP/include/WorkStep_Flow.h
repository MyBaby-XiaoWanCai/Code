/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: WorkStep_Flow.h
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef APP_INCLUDE_FLOW_WORKSTEP_H_
#define APP_INCLUDE_FLOW_WORKSTEP_H_

#define NUM_WORKSTEP_MAX        256                         //���Ĺ�����
#define NUM_CYCLE_MAX			3		//ѭ�����Ƕ����

typedef struct struct_StepEnd
{
    Uint16 End_EN;                       //ʹ��
     int32 End_Value;                     //������������
    Uint16 End_Logic;                    //���������߼�
    Uint16 End_Result;                   //����������ת�Ʒ���
    Uint16 End_Jump_Index;                  //
    Uint16 Cnt_Check;
    Uint16 Num_Mask;					//����ת�������ι����������ʱ��
    Uint16 Cnt_Mask;
     int32 Now_Value;
    Uint16 IsStepEnd;                   //��ǰ�ж�����������
    Uint16 priority;
} struct_StepEnd;

typedef struct struct_Record_Time
{
    Uint16 EN;
    Uint32 Delta_Time;           //ʱ����¼����
    Uint32 Cnt_Time;             //ʱ����¼���ݼ�����
    Uint16 IsRecord;            //�Ƿ񵽼�¼����ʱ��,��̨����ʹ��������
} struct_Record_Time;

typedef struct struct_Record_Condition
{
    Uint16 EN;
     int32 Value_Now;
     int32 Delta_Now;
     int32 Delta_Ref_Step;
     int32 Delta_Ref_Global;
     int32 Value_Last;
    Uint16 IsRecord;
} struct_Record_Condition;


//�ṹ�嶨��
typedef struct struct_WorkStep_Data
{
    Uint16 Index;                       //��ǰִ�еĹ����ڴ洢���е�������
    Uint16 Type;                        //��ǰִ�еĹ���������
    Uint16 Type_Last;                   //��һ����ִ�еĹ���������
     int32 Rated_Vol;                   //�ɴ洢�Ĺ������ݵõ���Ŀ���ѹ
     int32 Rated_Cur;                   //�ɴ洢�Ĺ������ݵõ���Ŀ�����
    Uint16 Length;                      //�������ݳ���
    Uint32 Formula_NumberH;
    Uint32 Formula_NumberL;

    int32 Pressure;                    //��ѹ��������ĸ�ѹֵ�ľ���ֵ
    Uint16 Vol_Interval_FreshFlag;

    struct_Record_Time      Record_Time;                 //ʱ������ݼ�¼
    struct_Record_Condition Record_Vol;                  //��ѹ�����ݼ�¼
    struct_Record_Condition Record_Cur;                  //���������ݼ�¼

    struct_StepEnd StepEnd_Time;        //ʱ���ֹ����
    struct_StepEnd StepEnd_Vol;         //��ѹ��ֹ����
    struct_StepEnd StepEnd_Cur;         //������ֹ����
    struct_StepEnd StepEnd_Ah;          //������ֹ����

    //������ر���
    struct_Protect_Value Prot_Step_CH_Over_Cur;
    struct_Protect_Value Prot_Step_CH_Over_Ah;
    struct_Protect_Value Prot_Step_CH_Over_Vol_Bat;
    struct_Protect_Value Prot_Step_CH_Over_Vol_Out;

    struct_Protect_Value Prot_Step_DIS_Over_Cur;
    struct_Protect_Value Prot_Step_DIS_Over_Ah;
    struct_Protect_Value Prot_Step_DIS_Under_Vol_Bat;
    struct_Protect_Value Prot_Step_DIS_Under_Vol_Out;

    struct_Protect_Value Prot_Step_Bat_Over_Temp;
    struct_Protect_Value Prot_Step_Bat_Under_Temp;

    struct_Protect_Delta Prot_Step_Cur_Delta_Pos;
    struct_Protect_Delta Prot_Step_Vol_Delta_Pos;
    struct_Protect_Delta Prot_Step_Cur_Delta_Neg;
    struct_Protect_Delta Prot_Step_Vol_Delta_Neg;

    struct_Protect_Range Prot_Step_Vol_Time;
    struct_Protect_Vol_Change Prot_Step_Vol_Change_Time;
    struct_Protect_Range Prot_Step_BatShort;

    struct_Protect_Value_Diff  Prot_Step_Vol_Diff_Over;
    struct_Protect_Value_Diff  Prot_Step_Cur_Diff_Over;
    struct_Protect_Interval    Prot_Step_Vol_Interval_Pos;
    struct_Protect_Interval    Prot_Step_Vol_Interval_Neg;


} struct_WorkStep_Data;


typedef struct struct_WorkStep_Service
{
    //��������״̬������ӿ�
    Uint16 Req_CMD;                     //!=0,��ʾ����������
    Uint16 Req_CMD_OnRunning;           //������������������
    Uint16 Req_WorkStep_Index;          //�������еĹ�����
    Uint16 Req_SingleStep;              //���󵥲�����
    Uint16 Req_PowerFailContine;        //���������������
    Uint16 Req_Cur_PreCharge;           //Ԥ�����������
    Uint16 Req_Time_PreCharge;          //Ԥ���ʱ��
    Uint16 Req_PowerFailSave;           //���籣������
    Uint16 Req_Stop_Socket_Err;			//��������ʱ�������󣬱��ⳬʱ�ػ��������

     int32 Cur_Limited_Set;                 //����趨��������ֵ

    //����ʵʱ״̬������
    Uint16 State;                       //�������̿���״̬��״̬
    Uint16 Cnt_Delay;                   //ģ���ڲ���ʱ������
    Uint16 Flag_SingleStep;             //�������б�־
    Uint16 Step_Cnt;                    //���������������ۼ�ִ�еĹ�����
    Uint16 Step_State;                  //����ִ��״̬
    Uint32 Step_Time;                   //����ִ��ʱ��
    Uint32 Run_TimeTotal;                   //����ִ����ʱ��
    Uint32 Step_Ah;                     //��ǰ�����ۼư�ʱ��
    Uint32 Step_Wh;                     //��ǰ�����ۼ���ʱ��
    Uint32 Total_Time;                  //����ִ��ʱ��ִ��ʱ��
     int32 Total_Ah;                    //���������������ۼư�ʱ��
    Uint32 Total_Wh;                    //���������������ۼ���ʱ��
     int32 Res_Contact;                 //�Ӵ�����
     int32 Res_Loop;                    //�����·���裿
    Uint16 Flag_FlowEnd_Once;           //���빤�ս���״̬���ϱ�һ��������
    Uint16 Flag_Pause_Once;             //������ͣ״̬���ϱ�һ�����ݱ�־
    Uint16 Flag_FlowStop_Once;          //���빤��ֹͣ״̬���ϱ�һ��������
    //Uint16 TimeOut_Stop;              //��ʱͣ��

    Uint32 CC_Ah;                       //CCtoCV������CC�׶��ۼƳ�簲ʱ��
    Uint32 CCdivCV_Ah;                  //��������
    Uint32 DCC_Ah;                      //DCCtoDCV������CC�׶��ۼƳ�簲ʱ��
    Uint32 DCCdivDCV_Ah;                //�ŵ������

    Uint32 Remain_Ah;                   //Ah�ۼ���
    Uint32 Remain_Wh;                   //Wh�ۼ���
} struct_WorkStep_Service;

typedef struct struct_WorkStep_PowerFail
{
    Uint16 Step_State_Now;                  //�쳣����ʱ����ִ��״̬
    Uint16 Step_State_EEPROM;                  //�쳣����ʱ����ִ��״̬
    Uint16 Step_Index_EEPROM;                  //�쳣����ʱ�������еĹ�����
    Uint32 Step_Time_EEPROM;                   //�쳣����ʱ����ִ��ʱ��
    Uint32 Step_Ah_EEPROM;                     //�쳣����ʱ�����ۼư�ʱ��
    Uint32 Step_Wh_EEPROM;                     //�쳣����ʱ�����ۼ���ʱ��
     int32 Vol_T1_EEPROM;
} struct_WorkStep_PowerFail;

typedef struct struct_Cycle_Stack
{
	Uint16 Index_Cycle;					//ѭ�����
	Uint16 Num_Cycle;					//ѭ����Ҫִ�еĴ���
	Uint16 Cnt_Cycle;					//ѭ���Ѿ�ִ����ɵĴ���
	Uint16 Start_Cycle;					//��ǰѭ����ʼ������
	Uint16 End_Cycle;                   //��ǰѭ������������
	Uint16 EndLogic_Cycle;              //ѭ����������߼�
} struct_Cycle_Stack;

typedef struct struct_WorkStep_Cycle
{
	Uint16 Now_Cycle;					//ѭ��Ƕ�״�����0-��ѭ����1-��1��ѭ��
	struct_Cycle_Stack Cycle_Stack[NUM_CYCLE_MAX+1];	//����ѭ��Ƕ��������
} struct_WorkStep_Cycle;

//Step_State,�������״̬
#define cComplete                   2
#define cNotComplete                3

//Req_CMD����
#define WORKSTEP_RUN                 4       //debug ���󹤲�����
#define WORKSTEP_PAUSE               3       //debug ���󹤲�����
#define WORKSTEP_STOP                2       //debug ���󹤲�����
#define WORKSTEP_PRECHARGE           1       //Ԥ�������

//-------------------����״̬������----------------------
#define cStopStatus                 0x00
#define cPauseStatus                0x01
#define cWorkStepRunStatus          0x02
#define cFaultStatus                0x03
#define cStepComplete               0x04
#define cSingalStepRunStatus        0x05
#define cStepChangeStatus           0x06
#define cStepContactCheck           0x07
#define cStepStandby                0x08
#define cStepFault                  0x09
#define cStepStart                  0x0A
#define cStepNegPressureNext        0x0B
#define cStatusErr                  0xFF


//State״̬����
#define STEP_STRUCT_INIT        0       //�ṹ�����ݳ�ʼ��״̬
#define STEP_STANDBY            1       //����״̬
#define STEP_REQ_READ           12      //�����ȡ����
#define STEP_WAIT_DATA          2       //�ȴ�Flash���ݶ���״̬
#define STEP_DATA_ANALYSE       3       //�������ݽ���״̬
#define STEP_END_CHECK          4       //�Թ�����������Ԥ��飬������������Ĺ���ֱ������STEP_STEPEND
#define STEP_STANDBY2RUN        5       //����״̬ת��������״̬�������ݳ�ʼ��
#define STEP_RUN                6       //������������״̬�����Խ���ֹͣ����ͣ����
#define STEP_PAUSE              7       //��ͣ״̬
#define STEP_PAUSE2RUN          8       //��ͣ״̬ת��������״̬ǰ���в������ݳ�ʼ��
#define STEP_STEPEND            9       //�������н�������ѭ��������
#define STEP_FLOW_END           10      //�������̽���״̬��
#define STEP_CONTACT_CHECK      11      //�Ӵ����״̬
#define STEP_CONTACT_END        14      //�Ӵ�������
#define STEP_FLOW_STOP          13      //��������δ�������ʱͣ�������״̬

//���ݼ�¼�ṹ��
typedef struct struct_Image
{
    unsigned char ubChargerStatus;
    unsigned long udwSuperRunTime;
      signed long wBatSenseAVoltNew_UP;
      signed long dwOutPutCurrentNew_UP;
      signed long udwChargerCapacityNew;
    unsigned int  uwBatteryDegreeNew;
    unsigned int  uwWorkStepNum;
    unsigned char ubWorkStepName;
    unsigned int  uwStepCycleStartIndex;
    unsigned int  uwWorkStepTotalNum;
    unsigned long udwAH_CCdivCV;
    unsigned long udwAH_DCCdivDCV;
      signed long dwBatEnergy;
      signed long wOutPutVoltNew_UP;
    unsigned int  uwChannel1NTCDegreeNew;
      signed long Vol_Contact;//������ѹ
    unsigned int  Res_Contact;//�Ӵ�����
    unsigned int  Res_Loop;//�Ӵ�����
        long long DateTime;
      signed char NegPressure;
} struct_Image;

typedef struct struct_Image_TX
{
    Uint16 Req_TX;
    struct_Image Record;
} struct_Image_TX;

typedef struct struct_Filter_Elimin
{
    int32 Data_In;              //��ǰ��������
    int32 Data_Last1;           //��һ����������
    int32 Data_Last2;           //����һ����������
    int32 Data_Last3;           //������һ����������
    int32 Delta_Now;            //�������ݱ仯
    int32 Delta_Last;           //�ϴ����ݱ仯
    int32 Delta_Last1;          //���ϴ����ݱ仯
    int32 Elimin_Abs_Max;       //���������޳�������������
    int32 Elimin_Abs_Min;       //���������޳�������������
    int32 Data_Out;             //�������
    Uint32 Data_Update_cnt;    //�������׻���¼���
} struct_Filter_Elimin;
typedef struct struct_Filter_Workstep
{
	unsigned char Type;
	int32  Rated_Cur;
	int32  Cur_End;
	unsigned int Voltage_Trend;//0�ȶ���1������2�½�����ʼ��Ϊ0
}struct_Filter_Workstep;

typedef struct struct_Filter_Median
{
    int32 Data_In;              //��ǰ��������
    int32 Data[5];              //
    Uint16 Cnt;                 //�����ۼƴ���
    int32 Data_Out;             //�������
} struct_Filter_Median;

typedef struct struct_Rate_Cur_Compute
{
    int32 Data_In;              //��ǰ������������

    int32 Data_Last;            //ǰһ�ε�������

    int32 Data_Cur[11];        //0.1S�����ʷ��������
   Uint16 Cnt_Delay;

    int32 Rate_Cur_Out;
    int32 Rate_Cur_Now;
    int32 Rate_Cur_1S;
    int32 Rate_Cur_Filter;
   Uint16 Cnt_Error;
   Uint16 Cnt_Error_Reset;
    int32 Rate_Max_Expect;
    int32 Rate_Min_Expect;
} struct_Rate_Cur_Compute;


//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern long long DateTime;
extern struct_WorkStep_Data         WorkStep_Data;
extern struct_WorkStep_Service      WorkStep_Service;
extern struct_WorkStep_Cycle        WorkStep_Cycle;

extern struct_Image_TX Image_Protect;       //���ϱ���ʱ�������ݾ���
extern struct_Image_TX Image_StepEnd;       //�����������ݾ���
extern struct_Image_TX Image_UpData;        //���ϱ����ݼ�¼����

extern struct_WorkStep_PowerFail WorkStep_PowerFail;
extern Uint16 Middle_Cnt;

extern struct_Filter_Median        Median_Vol_Sense;
extern struct_Filter_Median        Median_Vol_Out;
extern struct_Filter_Median        Median_Cur_Total;

extern struct_Rate_Cur_Compute     Rate_Cur_Compute;

//���ⲿ�ļ����ź�������
extern void WorkStep_Flow(void);
extern void Process_Average_Analog(void);
extern Uint32 Char2Uint32(unsigned char *Ptr,Uint16 Offset);
extern  int32 Char2int32(unsigned char *Ptr,Uint16 Offset);
extern Uint16 Char2Uint16(unsigned char *Ptr,Uint16 Offset);
extern  int16 Char2int16(unsigned char *Ptr,Uint16 Offset);
extern void Init_WorkStep_Protect(void);
extern void Init_Global_Protect(void);
extern void WorkStep_End_Check(struct_StepEnd *Ptr);
extern void Ah_Accumulate(void);
extern void Wh_Accumulate(void);
extern void Disable_All_WorkStep_Protect(void);
extern void Enable_All_WorkStep_Protect(void);
extern void Res_Contact_Compute(void);
extern void Res_Loop_Compute(void);
extern void SaveDataToRecord(struct_Image *Ptr);
extern Uint16 sBatTemperatureRealCal(int32 dwBatTempSampleAvg);
extern INT16U sTemperature1SampleAvgCal_B3380(INT16U uwChannel1NTCSampleTemp);
extern INT16U sTemperature1SampleAvgCal(INT16U uwChannel1NTCSampleTemp);
extern void WorkStep_Service_Init(void);
extern void Cali_PT_5V(void);
extern void CopyImage(struct_Image *Source,struct_Image *Dest);
extern int32 Get_Middle_Value(void);
extern void Updata_Mutation_Check(void);
extern void StepEnd_TimeSet(Uint16 offset);
extern void StepEnd_VolSet(Uint16 offset);
extern void StepEnd_CurSet(Uint16 offset);
extern void StepEnd_AhSet(Uint16 offset);
extern void Function_Rate_Cur_Compute(void);


#endif /* APP_INCLUDE_FLOW_WORKSTEP_H_ */

//File end
