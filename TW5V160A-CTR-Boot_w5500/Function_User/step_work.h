#ifndef __STEP_WORK_H__
#define __STEP_WORK_H__

#include "data_analysis.h"
#include "discharge.h"

typedef struct running_step_struct {
    uint8_t status;
    uint8_t type;
    uint16_t number;
    uint16_t sum_number;
    uint32_t run_time;
    uint32_t protection_time;
} RunningStep;

typedef struct running_circle_struct {
    uint16_t circle_start;
    uint16_t circle_serial;
    uint16_t circle_number;
    uint16_t circle_count;
    uint8_t  circle_end_flag;
} RunningCircle;

typedef struct sample_data_struct {
    float v_sense;
    float i_out;
    float v_mod;
    float v_j;
    float battery_temperature;
    float mod_tempetature;
} SampleData;

#define SAMPLE_NUMBER 100

typedef struct special_sapmple_struct {
    float Bus1;
    float Bus2;
    float Aux;
    float Amb;
} SpecialSample;

typedef struct ad_sample_struct {
    uint32_t v_sense;
    uint32_t i_out;
    int32_t v_j;
} AdSample;

typedef struct channel_record_data_struct {
    float run_time;                                 //运行时间      ms-毫秒
    float battery_voltage;                          //电池电压      V-伏特
    float output_current;                           //输出电流      A-安培
    float output_voltage;                           //输出电压      V-伏特
    float battery_capacitance;                      //电池容量      Ah-安时
    float battery_energy;                           //电池能量      VAh
    float battery_temperature;                      //电池温度      摄氏度
    float module_temperature;                       //模块温度      摄氏度
    float crosscurrent_ratio;                       //恒流比
    float contact_impedance;                        //接触阻抗
    float loop_impedance;                           //回路阻抗
    float v_j;
} ChannelRecordData;

typedef struct record_data_struct {
    struct channel_record_data_struct channel_record_data[CHANNELS];
    float Bus1;
    float Bus2;
    float Aux;
    float Amb;
} RecordData;

#if CHANNELS_1
    extern RunningStep running_step[CHANNELS];
#endif
#if CHANNELS_2
    extern RunningStep running_step[CHANNELS];
#endif
#if CHANNELS_4
    // extern uint8_t  running_step_type[CHANNELS];
    // extern uint8_t  running_step_status[CHANNELS];
    // extern uint16_t running_step_number[CHANNELS];
    // extern uint16_t running_step_number_sum[CHANNELS];
    // extern uint32_t running_step_time[CHANNELS];

    extern RunningStep running_step[CHANNELS];
    extern StepRecordCondition last_record_condition[CHANNELS];
    extern StepRecordCondition this_record_condition[CHANNELS];
    extern RunningCircle running_circle[CHANNELS];
    // extern RecordData record_data[CHANNELS];
    extern RecordData record_data;
#endif
#if CHANNELS_8
    extern RunningStep running_step[CHANNELS];
#endif

#define INTEGRAL_TIME   0.1
#define CACHE_COUNT 30

extern SampleData accumulated_data_1ms[8];
extern AdSample ad_sample[8];
extern SampleData filter_data_old[8];
extern SampleData filter_data_now[8];
extern SpecialSample special_sample;
extern RecordQuery record_query_cache[CACHE_COUNT][CHANNELS];

void Step_Data_Read(uint8_t ch, uint16_t number);
void Step_Run_Execute(uint8_t ch);
void Step_Run_Switch(uint8_t run_ch, uint16_t run_number);
void Running_Switch(void);
void Step_Turn_Off(uint8_t ch);

#endif
