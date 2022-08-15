#ifndef __DATA_ANALYSIS_H__
#define __DATA_ANALYSIS_H__

#include "eth_process.h"
#include "main.h"

#define DATABIG_ENDIAN  0

#define SOFTWARE_VERSION    0x01
#define SOFTWARE_SUBVERSION 0X03

#define CHANNELS_4  1

#if CHANNELS_1
    #define CHANNELS    1
#endif
#if CHANNELS_2
    #define CHANNELS    2
#endif
#if CHANNELS_4
    #define CHANNELS    4
#endif
#if CHANNELS_8
    #define CHANNELS    8
#endif

extern long Time_Stamp;

#define CH_1        0x01
#define CH_2        0x02
#define CH_3        0x04
#define CH_4        0X08
#define CH_5        0x10
#define CH_6        0x20
#define CH_7        0x40
#define CH_8        0X80

#define CH(i)   {   switch(i) { \
                        case 1: CH_1;break; \
                        case 2: CH_2;break; \
                        case 3: CH_3;break; \
                        case 4: CH_4;break; \
                        case 5: CH_5;break; \
                        case 6: CH_6;break; \
                        case 7: CH_7;break; \
                        case 8: CH_8;break; \
                        default: break; \
                    } }

#define FRAME_HEADER    0x5a5a
#define END_OF_FRAME    0xa5a5

//������
enum Command_ID {
    Global_Protection_Setting = 0x01,       // ȫ�ֱ�������
    Global_Protection_Query,                // ȫ�ֱ�����ѯ
    Global_Protection_Trigger,              // ȫ�ֱ�������
    Record_Query,                           // ��¼���ݲ�ѯ
    WorkStep_Setting,                       // ��������
    WorkStep_State_Setting,                 // ��������״̬����
    WorkStep_Query,                         // ������ѯ
    WorkStep_Protection_Trigger,            // ������������
    Calibration_Parameter_Setting,          // У׼��������
    Calibration_Parameter_Query,            // У׼������ѯ
    AD_Data_Query,                          // AD ���ݶ�ȡ
    System_Query,                           // ϵͳ��ѯ
    System_Setting,                         // ϵͳ����
    System_Update,                          // ϵͳ����
    // Precharge_Setting,                      // Ԥ�������/��Ӧ
    Command_Setting_ID_End,

    Debug_Parameter_Setting = 0x21,
    Debug_Data_Query,
    Debug_System_Setting,

    Command_Reply_ID_Start = 0x80,
    Global_Protection_Setting_Reply,           // ȫ�ֱ���������Ӧ
    Global_Protection_Query_Reply,             // ȫ�ֱ�����ѯ�ظ�
    Global_Protection_Trigger_Reply,           // ȫ�ֱ�������
    Record_Query_Reply,                        // ��¼���ݲ�ѯ�ظ�
    WorkStep_Setting_Reply,                    // ����������Ӧ
    WorkStep_State_Setting_Reply,              // ��������״̬������Ӧ
    WorkStep_Query_Reply,                      // ������ѯ�ظ�
    WorkStep_Protection_Trigger_Reply,         // ������������
    Calibration_Parameter_Setting_Reply,       // У׼����������Ӧ
    Calibration_Parameter_Query_Reply,         // У׼������ѯ�ظ�
    AD_Data_Query_Reply,                       // AD ���ݶ�ȡ�ظ�
    System_Query_Reply,                        // ϵͳ��ѯ�ظ�
    System_Setting_Reply,                      // ϵͳ������Ӧ
    System_Update_Reply,                       // ϵͳ����
    Command_Reply_ID_End,
};


/***************************************************************************************************
**  ȫ�ֱ�����������ݽṹ
**
***************************************************************************************************/
enum global_protection_parament {
    charge_OverVoltage_setting = 0x01,              //����ѹ����
    charge_OverCurrent_setting,                     //����������
    charge_OverCapacitance_setting,                 //�����ݱ���
    discharge_UnderVoltage_setting,                 //�ŵ�Ƿѹ����
    discharge_OverCurrent_setting,                  //�ŵ��������
    discharge_OverCapacitance_setting,              //�ŵ���ݱ���
    // output_CurrentLimit_setting,                    //���ֱ����������
    battery_UnderTemperature_alarm_setting,         //���Ƿ�±�������
    battery_OverTemperature_warning_setting,        //��ع���Ԥ������
    battery_OverTemperature_alarm_setting,          //��ع��±�������
    reverse_battery_setting,                        //��ط��ӱ���
    contact_impedance_setting,                      //�Ӵ��迹����
    // current_OutOfTolerance_setting,                 //���������
    // voltage_OutOfTolerance_setting,                 //��ѹ�����
    fluctuation_BatteryVoltage_setting,             //��ص�ѹ��������
    fluctuationtime_BatteryVoltage_setting,         //��ص�ѹ�������ʱ��
    fluctuation_OutputCurrent_setting,              //���������������
    fluctuationtime_OutputCurrent_setting,          //��������������ʱ��
    output_OverVoltage_resistance_setting,          //�����߹�ѹ��������
    loop_impedance_setting,                         //��·�迹����

    // mutation_BatteryVoltage_setting,                //��ص�ѹͻ�䱣��
    // MutationTime_BatteryVoltage_setting,            //��ص�ѹͻ����ʱ��
    // mutation_OutputCurrent_setting,                 //�������ͻ�䱣��
    // MutationTime_OutputCurrent_setting,             //�������ͻ����ʱ��

    global_protection_parament_end,
};
enum global_protection_trigger {
    charge_OverVoltage_trigger = 0x01,              //����ѹ����
    charge_OverCurrent_trigger,                     //����������
    discharge_UnderVoltage_trigger,                 //�ŵ�Ƿѹ����
    discharge_OverCurrent_trigger,                  //�ŵ��������
    charge_OverCapacitance_trigger,                 //�����ݱ���
    // output_CurrentLimit_trigger,                    //���ֱ����������
    global_protection_invalid_symbols_1,
    battery_OverTemperature_alarm_trigger,          //��ع��±�������-->����¶����ޱ���
    reverse_battery_trigger,                        //��ط��ӱ���
    global_protection_invalid_symbols_2,
    output_ShortCircuit_trigger,                    //����˶�·����
    battery_ShortCircuit_trigger,                   //��ض�·����
    fan_fault_trigger,                              //���ȹ���
    BUS_high_fault_trigger,                         //���� BUS �߹���
    fluctuation_BatteryVoltage_trigger,             //��ص�ѹ��������
    global_protection_invalid_symbols_3,
    fluctuation_OutputCurrent_trigger,              //���������������
    global_protection_invalid_symbols_4,
    global_protection_invalid_symbols_5,
    battery_UnderTemperature_alarm_trigger,         //���Ƿ�±�������-->����¶����ޱ���
    discharge_OverCapacitance_trigger,              //�ŵ���ݱ���
    contact_impedance_trigger,                      //�Ӵ��迹����
    global_protection_invalid_symbols_6,
    global_protection_invalid_symbols_7,
    loop_impedance_trigger,                         //��·�迹����
    output_OverVoltage_trigger,                     //ģ��˿������ѹ����
    global_protection_invalid_symbols_8,
    global_protection_invalid_symbols_9,
    global_protection_invalid_symbols_10,
    global_protection_invalid_symbols_11,
    global_protection_invalid_symbols_12,
    global_protection_invalid_symbols_13,
    module_OverTemperature_alarm_trigger,           //ģ����±�������
    BUS_low_fault_trigger,                          //���� BUS �͹���
    calibration_data_error_trigger,                 //У׼���ݴ��󱨾�
    step_data_error_trigger,                        //�������ݴ��󱨾�
    hardware_fault_trigger,                         //Ӳ������
    battery_OverTemperature_warning_trigger,        //��ع���Ԥ������
    AC_dump_trigger,                                //AC������ϱ���
    other_fault_trigger,                            //�������ϱ���

    global_protection_trigger_end,

    // current_OutOfTolerance_trigger,                 //���������
    // voltage_OutOfTolerance_trigger,                 //��ѹ�����

};
typedef struct global_protection_struct {
    float charge_OverVoltage;                       //����ѹ����
    float charge_OverCurrent;                       //����������
    float charge_OverCapacitance;                   //�����ݱ���
    float discharge_UnderVoltage;                   //�ŵ�Ƿѹ����
    float discharge_OverCurrent;                    //�ŵ��������
    float discharge_OverCapacitance;                //�ŵ���ݱ���
    // float output_CurrentLimit;                      //ֱ�������������
    float battery_UnderTemperature_alarm;           //���Ƿ�±�������
    float battery_OverTemperature_warning;          //��ع���Ԥ������
    float battery_OverTemperature_alarm;            //��ع��±�������
    float reverse_battery;                          //��ط��ӱ���
    float contact_impedance;                        //�Ӵ��迹����
    // float current_OutOfTolerance;                   //���������
    // float voltage_OutOfTolerance;                   //��ѹ�����
    float fluctuation_BatteryVoltage;               //��ص�ѹ��������
    float fluctuationtime_BatteryVoltage;           //��ص�ѹ�������ʱ��
    float fluctuation_OutputCurrent;                //���������������
    float fluctuationtime_OutputCurrent;            //��������������ʱ��
    float output_OverVoltage;                       //ģ��˿������ѹ����
    float loop_impedance;                           //��·�迹����
} GlobalProtection;
//==================================================================================================


/***************************************************************************************************
**  ��¼����������ݽṹ
**
***************************************************************************************************/
typedef struct record_query_struct {
    // uint8_t time_stamp[9];                          //ʱ���        ��/��/��/ʱ/��/��/����
    int64_t time_stamp;                            //ʱ���
    uint8_t run_status;                             //����״̬
    uint8_t step_type;                              //��������
    uint16_t step_number;                           //������
    uint16_t step_sum;                              //�ۼӹ�����
    uint16_t circle_serial;                         //ѭ�����
    uint16_t circle_count;                          //ѭ������
    float run_time;                                 //����ʱ��      s-��
    float battery_voltage;                          //��ص�ѹ      V-��
    float output_current;                           //�������      A-����
    float output_voltage;                           //�����ѹ      V-��
    float battery_capacitance;                      //�������      Ah-��ʱ
    float battery_energy;                           //�������      VAs-����
    float battery_temperature;                      //����¶�      ���϶�
    float module_temperature;                       //ģ���¶�      ���϶�
    float crosscurrent_ratio;                       //������
    float contact_impedance;                        //�Ӵ��迹
    float loop_impedance;                           //��·�迹
} RecordQuery;
//==================================================================================================


/***************************************************************************************************
**  ��������������ݽṹ
**
***************************************************************************************************/
enum step_type_parament {
    standby = 0x01,                                     //����
    CC,                                                 //�������
    CV,                                                 //��ѹ���
    CC_to_CV,                                           //������ѹ���
    DCC,                                                //�����ŵ�
    DCV,                                                //��ѹ�ŵ�
    step_type_invalid_symbols_1,
    DCP,                                                //�㹦�ʷŵ�
    loop_begin,                                         //ѭ����ʼ
    loop_end,                                           //ѭ������
    DCC_to_DCV,                                         //������ѹ�ŵ�
    CP,                                                 //�㹦�ʳ��
    end_step,                                           //��������

    step_type_parament_end,
};
enum step_work_abord_parament {
    step_time = 0x01,                                   //������ֹʱ��
    step_voltage,                                       //��������/��ֹ��ѹ
    step_current,                                       //��������/��ֹ����
    step_resistance,                                    //δʹ��
    step_power,                                         //δʹ��
    step_invalid_sysbols_1,
    step_capacitance,                                   //������ֹ����

    step_work_abort_parament_end,
};
// enum step_work_parament {
//     step_work_current = 0x01,                           //������������ֵ
//     step_work_voltage,                                  //����������ѹֵ
//     step_work_power,                                    //������������ֵ

//     step_work_parament_end,
// };
// enum step_abort_parament {
//     step_abort_current = 0x01,                          //������ֹ����ֵ
//     step_abort_voltage,                                 //������ֹ��ѹֵ
//     step_abort_time,                                    //������ֹʱ��ֵ
//     step_abort_capacitance,                             //������ֹ����ֵ

//     step_abort_parament_end,
// };
enum step_record_parament {
    step_record_time = 0x01,                            //������¼����֮ʱ��
    step_record_voltage,                                //������¼����֮��ѹ
    step_record_current,                                //������¼����֮����

    step_record_parament_end,
};
enum step_protection_parament {
    step_battery_temperature_limit = 0x01,              //����¶����ޱ���
    step_charge_capacitance,                            //�����������
    step_discharge_capacitance,                         //�ŵ���������
    step_voltage_high_limit,                            //��ѹ���ޱ���
    step_voltage_low_limit,                             //��ѹ���ޱ���
    step_charge_current_limit,                          //���������ޱ���
    step_discharge_current_limit,                       //�ŵ�������ޱ���
    step_current_positive_jump_range,                   //�����������ȱ���
    step_current_positive_jump_number,                  //������������
    step_current_negative_jump_range,                   //�����������ȱ���
    step_current_negative_jump_number,                  //������������
    step_voltage_positive_jump_range,                   //��ѹ�������ȱ���
    step_voltage_positive_jump_number,                  //��ѹ��������
    step_voltage_negative_jump_range,                   //��ѹ�������ȱ���
    step_voltage_negative_jump_number,                  //��ѹ��������
    step_current_OutOfTolerance,                        //���������
    step_voltage_OutOfTolerance,                        //��ѹ�����

    step_protection_parament_end,
};
enum step_protection_trigger {
    step_battery_temperature_limit_trigger = 0x01,              //����¶����ޱ���
    step_charge_capacitance_trigger,                            //�����������-->�������ޱ���
    step_discharge_capacitance_trigger,                         //�ŵ���������-->�������ޱ���
    step_voltage_high_limit_trigger,                            //��ѹ���ޱ���
    step_voltage_low_limit_trigger,                             //��ѹ���ޱ���
    step_charge_current_limit_trigger,                          //���������ޱ���-->�������ޱ���
    step_discharge_current_limit_trigger,                       //�ŵ�������ޱ���-->�������ޱ���
    step_current_positive_jump_trigger,                         //������������
    step_current_negative_jump_trigger,                         //������������
    step_voltage_positive_jump_trigger,                         //��ѹ��������
    step_voltage_negative_jump_trigger,                         //��ѹ��������
    step_current_OutOfTolerance_trigger,                        //���������
    step_voltage_OutOfTolerance_trigger,                        //��ѹ�����

    step_protection_trigger_end,
};
enum step_run_setting {
    step_stop_setting = 0x00,                           //��������Ϊֹͣ
    step_pause_setting,                                 //��������Ϊ��ͣ
    step_start_setting,                                 //��������Ϊ����
    step_fault_status,                                  //δ��
    step_complete_status,                               //����״̬Ϊ���
    step_single_setting,                                //δ��
    step_change,                                        //δ��
    step_continue_setting,                              //��������Ϊ����
    step_jump_setting,                                  //��������Ϊ��ת

    step_run_setting_end,
};

#define step_running_status step_start_setting
#define step_stop_status step_stop_setting
#define step_pause_status step_pause_setting


// enum step_run_status {
//     step_running_status = 0x01,                         //����״̬Ϊ������
//     step_stop_status,                                   //����״̬Ϊֹͣ
//     step_pause_status,                                  //����״̬Ϊ��ͣ
//     step_complete_status,                               //����״̬Ϊ���

//     step_run_status_end,
// };
typedef struct step_work_struct {
    uint16_t step_number;                               //������
    uint8_t step_type;                                  //��������
    float step_work_voltage;                            //����������ѹֵ
    float step_work_current;                            //������������ֵ
    float step_work_power;                              //������������ֵ
    float step_abort_time;                              //������ֹʱ��ֵ
    float step_abort_voltage;                           //������ֹ��ѹֵ
    float step_abort_current;                           //������ֹ����ֵ
    float step_abort_capacitance;                       //������ֹ����ֵ
} StepWork;
typedef struct step_record_condition_struct {
    float record_time;                                  //��¼����֮ʱ��ֵ
    float record_voltage;                               //��¼����֮��ѹֵ
    float record_current;                               //��¼����֮����ֵ
} StepRecordCondition;
typedef struct step_protection_struct {
    float protection_battery_temperature_limit;         //����¶����ޱ���
    float protection_charge_capacitance;                //�����������
    float protection_discharge_capacitance;             //�ŵ���������
    float protection_voltage_high_limit;                //��ѹ���ޱ���
    float protection_voltage_low_limit;                 //��ѹ���ޱ���
    float protection_charge_current_limit;              //���������ޱ���
    float protection_discharge_current_limit;           //�ŵ�������ޱ���
    float protection_current_positive_jump_range;       //�����������ȱ���
    uint32_t protection_current_positive_jump_number;   //������������
    float protection_current_negative_jump_range;       //�����������ȱ���
    uint32_t protection_current_negative_jump_number;   //������������
    float protection_voltage_positive_jump_range;       //��ѹ�������ȱ���
    uint32_t protection_voltage_positive_jump_number;   //��ѹ��������
    float protection_voltage_negative_jump_range;       //��ѹ�������ȱ���
    uint32_t protection_voltage_negative_jump_number;   //��ѹ��������
    float protection_current_OutOfTolerance;            //���������
    float protection_voltage_OutOfTolerance;            //��ѹ�����
} StepProtection;
typedef struct step_run_struct {
    uint8_t run_status;
    uint16_t run_number;
} StepRun;

#define STEP_NUMBER_MAX 256
extern uint8_t STEP_STATUS_SETTING_FLAG;
//==================================================================================================


/***************************************************************************************************
**  У׼��������ݽṹ
**
***************************************************************************************************/
enum calibration_paraments {
    buck_vset_sense_K = 0x01,
    buck_vset_sense_B,
    buck_vset_mod_K,
    buck_vset_mod_B,
    buck_iset_HK,
    buck_iset_HB,
    buck_iset_LK,
    buck_iset_LB,
    buck_vsense_K ,
    buck_vsense_B,
    buck_vmod_K,
    buck_vmod_B,
    buck_iout_HK,
    buck_iout_HB,
    buck_iout_LK,
    buck_iout_LB,
    buck_vj_K,
    buck_vj_B,

    boost_vset_sense_K,
    boost_vset_sense_B,
    boost_vset_mod_K,
    boost_vset_mod_B,
    boost_iset_HK,
    boost_iset_HB,
    boost_iset_LK,
    boost_iset_LB,
    boost_vsense_K ,
    boost_vsense_B,
    boost_vmod_K,
    boost_vmod_B,
    boost_iout_HK,
    boost_iout_HB,
    boost_iout_LK,
    boost_iout_LB,
    boost_vj_K,
    boost_vj_B,

    calibration_paraments_end,

    bus1_K = 0x01,
    bus1_B,
    bus2_K,
    bus2_B,
    aux_K,
    aux_B,
    amb_K,
    amb_B,
    temperature_K,
    temperature_B,
};
#define CALIBRATION_PORT_LEN 144     //36*4
#define CALIBRATION_FLAG    0x2323

enum calibration_ad_paraments {
    battery_voltage_ad = 0x01,
    module_voltage_ad,
    output_current_ad,
    vj_ad,

    calibration_ad_paraments_end,
};

typedef struct calibration_paraments_struct {
    float Buck_Vset_sense_K;
    float Buck_Vset_sense_B;
    float Buck_Vset_mod_K;
    float Buck_Vset_mod_B;
    float Buck_Iset_HK;
    float Buck_Iset_HB;
    float Buck_Iset_LK;
    float Buck_Iset_LB;
    float Buck_Vsense_K;
    float Buck_Vsense_B;
    float Buck_Vmod_K;
    float Buck_Vmod_B;
    float Buck_Iout_HK;
    float Buck_Iout_HB;
    float Buck_Iout_LK;
    float Buck_Iout_LB;
    float Buck_Vj_K;
    float Buck_Vj_B;

    float Boost_Vset_sense_K;
    float Boost_Vset_sense_B;
    float Boost_Vset_mod_K;
    float Boost_Vset_mod_B;
    float Boost_Iset_HK;
    float Boost_Iset_HB;
    float Boost_Iset_LK;
    float Boost_Iset_LB;
    float Boost_Vsense_K;
    float Boost_Vsense_B;
    float Boost_Vmod_K;
    float Boost_Vmod_B;
    float Boost_Iout_HK;
    float Boost_Iout_HB;
    float Boost_Iout_LK;
    float Boost_Iout_LB;
    float Boost_Vj_K;
    float Boost_Vj_B;
} CalibrationParaments;

typedef struct special_calibration_struct {
    float Bus1_K;
    float Bus1_B;
    float Bus2_K;
    float Bus2_B;
    float Aux_K;
    float Aux_B;
    float Amb_K;
    float Amb_B;
    float Temperature_K;
    float Temperature_B;
} SpecialCalibration;

typedef struct calibration_ad_struct {
    uint32_t Vsense_AD;
    uint32_t Vmod_AD;
    uint32_t Iout_AD;
    uint32_t Vj_AD;
} CalibrationAD;

typedef struct calibration_run_struct {
    uint8_t run_flag;
    uint8_t status;
    uint8_t type;
    float current;
    float voltage;
} CalibrationRun;

extern CalibrationParaments calibration[8];
extern SpecialCalibration special_calibration;

#define AD_NUMBER 100
extern CalibrationAD calibration_ad_add[8];
extern CalibrationAD calibration_ad_record[8];
extern CalibrationRun calibration_run[8];
extern uint8_t Calibration_Read_Flag;
//==================================================================================================


/***************************************************************************************************
**  ϵͳ��������ݽṹ
**
***************************************************************************************************/
enum system_query {
    system_information = 0x01,                          //ϵͳ������Ϣ��ѯ
    // power_off_query,                                    //�ϵ������в�ѯ
    // system_fault,                                       //ϵͳ�������ѯ

    system_query_end,
};
enum system_setting {
    system_alarm_cleared = 0x01,                        //ϵͳ�澯���
    system_time_setting,                                //ϵͳʱ���趨
    // system_parallel_setting,                            //ϵͳ�����趨
    // power_off_setting,                                  //�ϵ��������趨
    heartbeat_package,                                  //������ָ��
    system_mode_setting,                                //ϵͳģʽ����

    system_setting_end,
};
#define SYS_NORMAL_MODE         0x66
#define SYS_CALIBRATION_MODE    0x55
extern uint8_t SYS_MODE;
//==================================================================================================


#if CHANNELS_1
    extern GlobalProtection global_protection[CHANNELS];

    extern RecordQuery recode_query[CHANNELS];

    extern StepWork step_work[CHANNELS];
    extern StepRecordCondition step_record_condition[CHANNELS];
    extern StepProtection step_protection[CHANNELS];
    extern StepRun step_run[CHANNELS];
#endif
#if CHANNELS_2
    extern GlobalProtection global_protection[CHANNELS];

    extern RecordQuery recode_query[CHANNELS];

    extern StepWork step_work[CHANNELS];
    extern StepRecordCondition step_record_condition[CHANNELS];
    extern StepProtection step_protection[CHANNELS];
    extern StepRun step_run[CHANNELS];
#endif
#if CHANNELS_4
    // extern float global_protection_ch1[GLOBAL_PROTECTION_NUMBER];
    // extern float global_protection_ch2[GLOBAL_PROTECTION_NUMBER];
    // extern float global_protection_ch3[GLOBAL_PROTECTION_NUMBER];
    // extern float global_protection_ch4[GLOBAL_PROTECTION_NUMBER];
    extern GlobalProtection global_protection[CHANNELS];

    extern RecordQuery recode_query[CHANNELS];
    extern RecordQuery protection_recode[CHANNELS];

    extern StepWork step_work[CHANNELS];
    extern StepRecordCondition step_record_condition[CHANNELS];
    extern StepProtection step_protection[CHANNELS];
    extern StepRun step_run[CHANNELS];
#endif
#if CHANNELS_8
    extern GlobalProtection global_protection[CHANNELS];

    extern RecordQuery recode_query[CHANNELS];

    extern StepWork step_work[CHANNELS];
    extern StepRecordCondition step_record_condition[CHANNELS];
    extern StepProtection step_protection[CHANNELS];
    extern StepRun step_run[CHANNELS];
#endif

void Receive_Data_Analysis(void);
void Record_Report(u8 ch);
// void Record_Report(u8 data_ch);
void Global_Protection_Report(u8 ch);
void Step_Protection_Report(u8 ch);
uint16_t Endia_Swap16(uint16_t data16);
uint32_t Endia_Swap32(uint32_t data32);
float Float_Swap(float ftemp);
int64_t Endia_Swap64(int64_t temp64);

#endif
