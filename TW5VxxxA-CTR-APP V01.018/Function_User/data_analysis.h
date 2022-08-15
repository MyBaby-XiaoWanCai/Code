#ifndef __DATA_ANALYSIS_H__
#define __DATA_ANALYSIS_H__

#include "eth_process.h"
#include "main.h"

#define DATABIG_ENDIAN  0

#define SOFTWARE_VERSION    1
#define SOFTWARE_SUBVERSION 19

// #define CHANNELS_1  1
// #define CHANNELS_2  1
#define CHANNELS_4  1
// #define CHANNELS_8  1

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

extern int64_t Time_Stamp;

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

//命令码
enum Command_ID {
    Global_Protection_Setting = 0x01,       // 全局保护设置
    Global_Protection_Query,                // 全局保护查询
    Global_Protection_Trigger,              // 全局保护触发
    Record_Query,                           // 记录数据查询
    WorkStep_Setting,                       // 工步设置
    WorkStep_State_Setting,                 // 工步运行状态设置
    WorkStep_Query,                         // 工步查询
    WorkStep_Protection_Trigger,            // 工步保护触发
    Calibration_Parameter_Setting,          // 校准参数设置
    Calibration_Parameter_Query,            // 校准参数查询
    AD_Data_Query,                          // AD 数据读取
    System_Query,                           // 系统查询
    System_Setting,                         // 系统设置
    System_Update,                          // 系统升级
    // Precharge_Setting,                      // 预充电设置/响应
    Command_Setting_ID_End,

    Debug_Parameter_Setting = 0x21,
    Debug_Data_Query,
    Debug_System_Setting,

    Command_Reply_ID_Start = 0x80,
    Global_Protection_Setting_Reply,           // 全局保护设置响应
    Global_Protection_Query_Reply,             // 全局保护查询回复
    Global_Protection_Trigger_Reply,           // 全局保护触发
    Record_Query_Reply,                        // 记录数据查询回复
    WorkStep_Setting_Reply,                    // 工步设置响应
    WorkStep_State_Setting_Reply,              // 工步运行状态设置响应
    WorkStep_Query_Reply,                      // 工步查询回复
    WorkStep_Protection_Trigger_Reply,         // 工步保护触发
    Calibration_Parameter_Setting_Reply,       // 校准参数设置响应
    Calibration_Parameter_Query_Reply,         // 校准参数查询回复
    AD_Data_Query_Reply,                       // AD 数据读取回复
    System_Query_Reply,                        // 系统查询回复
    System_Setting_Reply,                      // 系统设置响应
    System_Update_Reply,                       // 系统升级
    Command_Reply_ID_End,
};


/***************************************************************************************************
**  全局保护类相关数据结构
**
***************************************************************************************************/
enum global_protection_parament {
    charge_OverVoltage_setting = 0x01,              //充电过压保护
    charge_OverCurrent_setting,                     //充电过流保护
    charge_OverCapacitance_setting,                 //充电过容保护
    discharge_UnderVoltage_setting,                 //放电欠压保护
    discharge_OverCurrent_setting,                  //放电过流保护
    discharge_OverCapacitance_setting,              //放电过容保护
    // output_CurrentLimit_setting,                    //输出直流限流保护
    battery_UnderTemperature_alarm_setting,         //电池欠温报警保护
    battery_OverTemperature_warning_setting,        //电池过温预警保护
    battery_OverTemperature_alarm_setting,          //电池过温报警保护
    reverse_battery_setting,                        //电池反接保护
    contact_impedance_setting,                      //接触阻抗保护
    // current_OutOfTolerance_setting,                 //电流超差保护
    // voltage_OutOfTolerance_setting,                 //电压超差保护
    fluctuation_BatteryVoltage_setting,             //电池电压波动保护
    fluctuationtime_BatteryVoltage_setting,         //电池电压波动间隔时间
    fluctuation_OutputCurrent_setting,              //输出电流波动保护
    fluctuationtime_OutputCurrent_setting,          //输出电流波动间隔时间
    output_OverVoltage_resistance_setting,          //功率线过压电阻设置
    loop_impedance_setting,                         //回路阻抗保护
    sum_capacitance_setting,                        //总容量上限保护
    initial_voltage_high_setting,                   //初始电压上限
    initial_voltage_low_setting,                    //初始电压下限
    contact_time_setting,                           //接触测试时间
    contact_current_setting,                        //接触测试设置电流
    contact_voltage_variation_setting,              //接触测试电压变化量
    loop_voltage_difference_setting,                //回路压差保护

    // mutation_BatteryVoltage_setting,                //电池电压突变保护
    // MutationTime_BatteryVoltage_setting,            //电池电压突变间隔时间
    // mutation_OutputCurrent_setting,                 //输出电流突变保护
    // MutationTime_OutputCurrent_setting,             //输出电流突变间隔时间

    global_protection_parament_end,
};


enum global_protection_trigger {
    charge_OverVoltage_trigger = 0x01,              //充电过压保护
    charge_OverCurrent_trigger,                     //充电过流保护
    discharge_UnderVoltage_trigger,                 //放电欠压保护
    discharge_OverCurrent_trigger,                  //放电过流保护
    charge_OverCapacitance_trigger,                 //充电过容保护
    // output_CurrentLimit_trigger,                    //输出直流限流保护
    global_protection_invalid_symbols_1,
    battery_OverTemperature_alarm_trigger,          //电池过温报警保护-->电池温度上限保护
    reverse_battery_trigger,                        //电池反接保护
    global_protection_invalid_symbols_2,
    output_ShortCircuit_trigger,                    //输出端短路故障
    battery_ShortCircuit_trigger,                   //电池短路故障
    fan_fault_trigger,                              //风扇故障
    BUS_high_fault_trigger,                         //输入 BUS 高故障
    fluctuation_BatteryVoltage_trigger,             //电池电压波动保护
    global_protection_invalid_symbols_3,
    fluctuation_OutputCurrent_trigger,              //输出电流波动保护
    global_protection_invalid_symbols_4,
    global_protection_invalid_symbols_5,
    battery_UnderTemperature_alarm_trigger,         //电池欠温报警保护-->电池温度下限保护
    discharge_OverCapacitance_trigger,              //放电过容保护
    contact_impedance_trigger,                      //接触阻抗保护
    global_protection_invalid_symbols_6,
    global_protection_invalid_symbols_7,
    loop_impedance_trigger,                         //回路阻抗保护
    output_OverVoltage_trigger,                     //模块端口输出过压保护
    sum_capacitance_trigger,                        //总容量上限保护
    watch_dog_trigger,                              //看位门狗复位
    initial_voltage_high_trigger,                   //初始电压上限保护
    initial_voltage_low_trigger,                    //初始电压下限保护
    contact_voltage_variation_trigger,              //接触测试电压变化量异常保护
    loop_voltage_difference_trigger,                //回路压差保护
    module_OverTemperature_alarm_trigger,           //模块过温报警保护
    BUS_low_fault_trigger,                          //输入 BUS 低故障
    calibration_data_error_trigger,                 //校准数据错误报警
    step_data_error_trigger,                        //工步数据错误报警
    hardware_fault_trigger,                         //硬件保护
    battery_OverTemperature_warning_trigger,        //电池过温预警保护
    AC_dump_trigger,                                //AC掉电故障保护
    output_voltage_high_trigger,                    //输出端口过压保护
    other_fault_trigger,                            //其他故障报警

    global_protection_trigger_end,

    // current_OutOfTolerance_trigger,                 //电流超差保护
    // voltage_OutOfTolerance_trigger,                 //电压超差保护

};
typedef struct global_protection_struct {
    float charge_OverVoltage;                       //充电过压保护
    float charge_OverCurrent;                       //充电过流保护
    float charge_OverCapacitance;                   //充电过容保护
    float discharge_UnderVoltage;                   //放电欠压保护
    float discharge_OverCurrent;                    //放电过流保护
    float discharge_OverCapacitance;                //放电过容保护
    // float output_CurrentLimit;                      //直流输出限流保护
    float battery_UnderTemperature_alarm;           //电池欠温报警保护
    float battery_OverTemperature_warning;          //电池过温预警保护
    float battery_OverTemperature_alarm;            //电池过温报警保护
    float reverse_battery;                          //电池反接保护
    float contact_impedance;                        //接触阻抗保护
    // float current_OutOfTolerance;                   //电流超差保护
    // float voltage_OutOfTolerance;                   //电压超差保护
    float fluctuation_BatteryVoltage;               //电池电压波动保护
    float fluctuationtime_BatteryVoltage;           //电池电压波动间隔时间
    float fluctuation_OutputCurrent;                //输出电流波动保护
    float fluctuationtime_OutputCurrent;            //输出电流波动间隔时间
    float output_OverVoltage;                       //模块端口输出过压保护
    float loop_impedance;                           //回路阻抗保护
    float sum_capacitance;                          //总容量保护
    float initial_voltage_high;                     //初始电压上限
    float initial_voltage_low;                      //初始电压下限
    float contact_time;                             //接触测试时间
    float contact_current;                          //接触测试设置电流
    float contact_voltage_variation;                //接触测试电压变化量
    float loop_voltage_difference;                  //回路压差保护
} GlobalProtection;
//==================================================================================================


/***************************************************************************************************
**  记录类类相关数据结构
**
***************************************************************************************************/
typedef struct record_query_struct {
    // uint8_t time_stamp[9];                          //时间戳        年/月/日/时/分/秒/毫秒
    int64_t time_stamp;                             //时间戳
    uint8_t run_status;                             //运行状态
    uint8_t step_type;                              //工步类型
    uint16_t step_number;                           //工步号
    uint16_t step_sum;                              //累加工步数
    uint16_t circle_serial;                         //循环序号
    uint16_t circle_count;                          //循环次数
    float run_time;                                 //运行时间      s-秒
    float battery_voltage;                          //电池电压      V-伏
    float output_current;                           //输出电流      A-安培
    float output_voltage;                           //输出电压      V-伏
    float battery_capacitance;                      //电池容量      Ah-安时
    float battery_energy;                           //电池能量      VAs-焦耳
    float battery_temperature;                      //电池温度      摄氏度
    float module_temperature;                       //模块温度      摄氏度
    float crosscurrent_ratio;                       //恒流比
    float contact_impedance;                        //接触阻抗
    float loop_impedance;                           //回路阻抗
} RecordQuery;
//==================================================================================================


/***************************************************************************************************
**  工步类类相关数据结构
**
***************************************************************************************************/
enum step_type_parament {
    standby = 0x01,                                     //静置
    CC,                                                 //恒流充电
    CV,                                                 //恒压充电
    CC_to_CV,                                           //恒流恒压充电
    DCC,                                                //恒流放电
    DCV,                                                //恒压放电
    step_type_invalid_symbols_1,
    DCP,                                                //恒功率放电
    loop_begin,                                         //循环开始
    loop_end,                                           //循环结束
    DCC_to_DCV,                                         //恒流恒压放电
    CP,                                                 //恒功率充电
    end_step,                                           //结束工步

    step_type_parament_end,
};
enum step_work_abord_parament {
    step_time = 0x01,                                   //工步截止时间
    step_voltage,                                       //工步运行/截止电压
    step_current,                                       //工步运行/截止电流
    step_resistance,                                    //未使用
    step_power,                                         //未使用
    step_invalid_sysbols_1,
    step_capacitance,                                   //工步截止容量

    step_work_abort_parament_end,
};
// enum step_work_parament {
//     step_work_current = 0x01,                           //工步工作电流值
//     step_work_voltage,                                  //工步工作电压值
//     step_work_power,                                    //工步工作功率值

//     step_work_parament_end,
// };
// enum step_abort_parament {
//     step_abort_current = 0x01,                          //工步截止电流值
//     step_abort_voltage,                                 //工步截止电压值
//     step_abort_time,                                    //工步截止时间值
//     step_abort_capacitance,                             //工步截止容量值

//     step_abort_parament_end,
// };
enum step_record_parament {
    step_record_time = 0x01,                            //工步记录条件之时间
    step_record_voltage,                                //工步记录条件之电压
    step_record_current,                                //工步记录条件之电流

    step_record_parament_end,
};
enum step_protection_parament {
    step_OverTemperature_alarm = 0x01,                  //电池温度上限保护
    step_charge_capacitance,                            //充电容量保护
    step_discharge_capacitance,                         //放电容量保护
    step_voltage_high_limit,                            //电压上限保护
    step_voltage_low_limit,                             //电压下限保护
    step_charge_current_limit,                          //充电电流上限保护
    step_discharge_current_limit,                       //放电电流上限保护
    step_current_positive_jump_range,                   //电流正跳幅度保护
    step_current_positive_jump_number,                  //电流正跳次数
    step_current_negative_jump_range,                   //电流负跳幅度保护
    step_current_negative_jump_number,                  //电流负跳次数
    step_voltage_positive_jump_range,                   //电压正跳幅度保护
    step_voltage_positive_jump_number,                  //电压正跳次数
    step_voltage_negative_jump_range,                   //电压负跳幅度保护
    step_voltage_negative_jump_number,                  //电压负跳次数
    step_current_OutOfTolerance,                        //电流超差保护
    step_voltage_OutOfTolerance,                        //电压超差保护
    step_UnderTemperature_alarm,                        //电池温度下限保护
    step_timing_voltage_detection_time,                 //定时电压检测时间
    step_timing_voltage_high,                           //定时电压上限保护
    step_timing_voltage_low,                            //定时电压下限保护
    step_timing_voltage_change_detection_time1,         //定时电压变化检测时间1
    step_timing_voltage_change_detection_time2,         //定时电压变化检测时间2
    step_timing_voltage_change,                         //定时电压变化量
    step_battery_short_detection_time,                  //电池短路检测时间
    step_battery_short_voltage_high,                    //电池短路检测电压上限
    step_battery_short_voltage_low,                     //电池短路检测电压下限

    step_protection_parament_end,
};
enum step_protection_trigger {
    step_OverTemperature_alarm_trigger = 0x01,                  //电池温度上限保护
    step_charge_capacitance_trigger,                            //充电容量保护-->容量上限保护
    step_discharge_capacitance_trigger,                         //放电容量保护-->容量下限保护
    step_voltage_high_limit_trigger,                            //电压上限保护
    step_voltage_low_limit_trigger,                             //电压下限保护
    step_charge_current_limit_trigger,                          //充电电流上限保护-->电流上限保护
    step_discharge_current_limit_trigger,                       //放电电流上限保护-->电流下限保护
    step_current_positive_jump_trigger,                         //电流正跳保护
    step_current_negative_jump_trigger,                         //电流负跳保护
    step_voltage_positive_jump_trigger,                         //电压正跳保护
    step_voltage_negative_jump_trigger,                         //电压负跳保护
    step_current_OutOfTolerance_trigger,                        //电流超差保护
    step_voltage_OutOfTolerance_trigger,                        //电压超差保护
    step_abort_time_abnormal_trigger,                           //时间截止异常
    step_abort_voltage_abnormal_trigger,                        //电压截止异常
    step_abort_current_abnormal_trigger,                        //电压截止异常
    step_abort_capacitance_abnormal_trigger,                    //容量截止异常
    step_UnderTemperature_alarm_trigger,                        //电池温度下限保护
    step_timing_voltage_high_trigger,                           //定时电压上限保护
    step_timing_voltage_low_trigger,                            //定时电压下限保护
    step_timing_voltage_change_trigger,                         //定时电压变化量异常
    step_battery_short_voltage_high_trigger,                    //电池短路检测电压上限
    step_battery_short_voltage_low_trigger,                     //电池短路检测电压下限

    step_protection_trigger_end,
};
enum step_run_setting {
    step_stop_setting = 0x00,                           //工步设置为停止
    step_pause_setting,                                 //工步设置为暂停
    step_start_setting,                                 //工步设置为启动
    step_invalid_symbols_1,                             //未用
    step_complete_status,                               //工步状态为完成
    step_invalid_symbols_2,                             //未用
    step_invalid_symbols_3,                             //未用
    step_continue_setting,                              //工步设置为继续
    step_jump_setting,                                  //工步设置为跳转
    step_standby_status,                                //工步为待机模式
    step_start_flag,                                    //工艺流程开始
    step_contact_status,                                //接触测试阶段
    step_fault_status,                                  //故障

    step_run_setting_end,
};

#define step_running_status step_start_setting
#define step_stop_status step_stop_setting
#define step_pause_status step_pause_setting


// enum step_run_status {
//     step_running_status = 0x01,                         //工步状态为运行中
//     step_stop_status,                                   //工步状态为停止
//     step_pause_status,                                  //工步状态为暂停
//     step_complete_status,                               //工步状态为完成

//     step_run_status_end,
// };
typedef struct step_work_struct {
    uint16_t step_number;                               //工步号
    uint8_t step_type;                                  //工步类型
    float step_work_voltage;                            //工步工作电压值
    float step_work_current;                            //工步工作电流值
    float step_work_power;                              //工步工作功率值
    uint8_t step_abort_priority;                        //工步截止优先级标识
    float step_abort_time;                              //工步截止时间值
    float step_abort_voltage;                           //工步截止电压值
    float step_abort_current;                           //工步截止电流值
    float step_abort_capacitance;                       //工步截止容量值
} StepWork;
typedef struct step_record_condition_struct {
    float record_time;                                  //记录条件之时间值
    float record_voltage;                               //记录条件之电压值
    float record_current;                               //记录条件之电流值
} StepRecordCondition;
typedef struct step_protection_struct {
    float protection_OverTemperature_alarm;                         //电池温度上限保护
    float protection_charge_capacitance;                            //充电容量保护
    float protection_discharge_capacitance;                         //放电容量保护
    float protection_voltage_high_limit;                            //电压上限保护
    float protection_voltage_low_limit;                             //电压下限保护
    float protection_charge_current_limit;                          //充电电流上限保护
    float protection_discharge_current_limit;                       //放电电流上限保护
    float protection_current_positive_jump_range;                   //电流正跳幅度保护
    uint32_t protection_current_positive_jump_number;               //电流正跳次数
    float protection_current_negative_jump_range;                   //电流负跳幅度保护
    uint32_t protection_current_negative_jump_number;               //电流负跳次数
    float protection_voltage_positive_jump_range;                   //电压正跳幅度保护
    uint32_t protection_voltage_positive_jump_number;               //电压正跳次数
    float protection_voltage_negative_jump_range;                   //电压负跳幅度保护
    uint32_t protection_voltage_negative_jump_number;               //电压负跳次数
    float protection_current_OutOfTolerance;                        //电流超差保护
    float protection_voltage_OutOfTolerance;                        //电压超差保护
    float protection_UnderTemperature_alarm;                        //电池温度下限保护
    float protection_timing_voltage_detection_time;                 //定时电压检测时间
    float protection_timing_voltage_high;                           //定时电压上限保护
    float protection_timing_voltage_low;                            //定时电压下限保护
    float protection_timing_voltage_change_detection_time1;         //定时电压变化检测时间1
    float protection_timing_voltage_change_detection_time2;         //定时电压变化检测时间2
    float protection_timing_voltage_change;                         //定时电压变化量
    float protection_battery_short_detection_time;                  //电池短路检测时间
    float protection_battery_short_voltage_high;                    //电池短路检测电压上限
    float protection_battery_short_voltage_low;                     //电池短路检测电压下限
} StepProtection;
typedef struct step_run_struct {
    uint8_t run_status;
    uint16_t run_number;
} StepRun;

#define STEP_NUMBER_MAX 256
extern uint8_t STEP_STATUS_SETTING_FLAG;
//==================================================================================================


/***************************************************************************************************
**  校准类相关数据结构
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

#define AD_NUMBER 200
extern CalibrationAD calibration_ad_add[8];
extern CalibrationAD calibration_ad_record[8];
extern CalibrationRun calibration_run[8];
extern uint8_t Calibration_Read_Flag;
//==================================================================================================


/***************************************************************************************************
**  系统类相关数据结构
**
***************************************************************************************************/
extern uint8_t QUERY_PROTECTION_FLAG;
enum system_query {
    system_information = 0x01,                          //系统基本信息查询
    system_fault,                                       //系统故障码查询
    // power_off_query,                                    //断电自运行查询

    system_query_end,
};
enum system_setting {
    system_alarm_cleared = 0x01,                        //系统告警清除
    system_time_setting,                                //系统时间设定
    // system_parallel_setting,                            //系统并机设定
    // power_off_setting,                                  //断电自运行设定
    heartbeat_package,                                  //心跳包指令
    system_mode_setting,                                //系统模式设置
    battery_temperature_setting,                        //电池温度设置

    system_setting_end,
};
#define SYS_NORMAL_MODE         0x66
#define SYS_CALIBRATION_MODE    0x55
#define SYS_AGEING_MODE         0x44
#define SYS_RETEST_MODE         0x33
#define SYS_SELFTEST_MODE       0x22
extern uint8_t SYS_MODE;
//==================================================================================================


extern GlobalProtection global_protection[CHANNELS];

extern RecordQuery recode_query[CHANNELS];
extern RecordQuery protection_recode[CHANNELS];

extern StepWork step_work[CHANNELS];
extern StepRecordCondition step_record_condition[CHANNELS];
extern StepProtection step_protection[CHANNELS];
extern StepRun step_run[CHANNELS];

extern uint8_t heartbeat_flag;
extern uint16_t heartbeat_count;

void Receive_Data_Analysis(void);
void Record_Report(u8 ch);
// void Record_Report(u8 data_ch);
void Global_Protection_Report(u8 ch);
void Step_Protection_Report(u8 ch);
uint16_t Endia_Swap16(uint16_t data16);
uint32_t Endia_Swap32(uint32_t data32);
float Float_Swap(float ftemp);
int64_t Endia_Swap64(int64_t temp64);
void Boot_Process(void);
#endif
