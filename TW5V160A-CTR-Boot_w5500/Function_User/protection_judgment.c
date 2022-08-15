#include "main.h"
#include "step_work.h"
#include "data_analysis.h"
#include "discharge.h"
#include "protection_judgment.h"
#include <string.h>
#include <math.h>

uint8_t Global_Protection_Flag[CHANNELS][global_protection_trigger_end] = {0};
uint8_t Step_Protection_Flag[CHANNELS][step_protection_trigger_end] = {0};

uint8_t GLOBAL_PROT_FLAG[CHANNELS] = {0};
uint8_t STEP_PROT_FLAG[CHANNELS] = {0};

uint8_t CV_mode[CHANNELS] = {0};
uint8_t DCV_mode[CHANNELS] = {0};

uint8_t fluctuation_voltage_flag[CHANNELS] = {0};
uint8_t fluctuation_current_flag[CHANNELS] = {0};

FluctuationProtection fluctuation_protection_old[CHANNELS] = {0};
FluctuationProtection fluctuation_protection_now[CHANNELS] = {0};

JumpProtection jump_protection_old[CHANNELS] = {0};
JumpProtection jump_protection_now[CHANNELS] = {0};
uint8_t jump_flag[CHANNELS] = {0};

uint8_t PG_count[CHANNELS] = {0};

void Protection_Action(uint8_t ch, uint8_t flag, uint8_t status)
{
    uint8_t data_ch;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        return;
    }
#endif
#if CHANNELS_4
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else {
        return;
    }
#endif
#if CHANNELS_8
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else if(ch & CH_5) {
        data_ch = 4;
    } else if(ch & CH_6) {
        data_ch = 5;
    } else if(ch & CH_7) {
        data_ch = 6;
    } else if(ch & CH_8) {
        data_ch = 7;
    } else {
        return;
    }
#endif

//     protection_recode[data_ch].time_stamp[0] = 0;
//     protection_recode[data_ch].run_status = running_step[data_ch].status;
//     protection_recode[data_ch].step_type = running_step[data_ch].type;
//     protection_recode[data_ch].step_number = running_step[data_ch].number;
//     protection_recode[data_ch].step_sum = running_step[data_ch].sum_number;
//     protection_recode[data_ch].circle_serial = running_circle[data_ch].circle_serial;
//     protection_recode[data_ch].circle_count = running_circle[data_ch].circle_count;

//     protection_recode[data_ch].run_time = running_step[data_ch].run_time / 1000.0;
//     protection_recode[data_ch].battery_voltage = record_data.channel_record_data[data_ch].battery_voltage;

//     protection_recode[data_ch].output_current = record_data.channel_record_data[data_ch].output_current;
// #if CHANNELS_1

// #endif
// #if CHANNELS_2

// #endif
// #if CHANNELS_4
//     if(Dir_flag[2*data_ch] == BOOST_MODE) {
//         protection_recode[data_ch].output_current = 0 - protection_recode[data_ch].output_current;
//     }

// #endif
// #if CHANNELS_8

// #endif

//     protection_recode[data_ch].output_voltage = record_data.channel_record_data[data_ch].output_voltage;
//     protection_recode[data_ch].battery_capacitance = record_data.channel_record_data[data_ch].battery_capacitance;
//     protection_recode[data_ch].battery_energy = record_data.channel_record_data[data_ch].battery_energy;
//     protection_recode[data_ch].battery_temperature = record_data.channel_record_data[data_ch].battery_temperature;
//     protection_recode[data_ch].module_temperature = record_data.channel_record_data[data_ch].module_temperature;
//     protection_recode[data_ch].crosscurrent_ratio = record_data.channel_record_data[data_ch].crosscurrent_ratio;
//     protection_recode[data_ch].contact_impedance = record_data.channel_record_data[data_ch].contact_impedance;
//     protection_recode[data_ch].loop_impedance = record_data.channel_record_data[data_ch].loop_impedance;

    Record_Report(ch);

    PORT_DIS(2*data_ch);PORT_DIS(2*data_ch+1);
    HAL_Delay(10);
    REV_PORT_DIS(2*data_ch);REV_PORT_DIS(2*data_ch+1);
    HAL_Delay(10);
    DAC80508_SetDacData(2*data_ch, 0, 0);
    DAC80508_SetDacData(2*data_ch, 0, 1);
    DAC80508_SetDacData(2*data_ch+1, 0, 0);
    DAC80508_SetDacData(2*data_ch+1, 0, 1);

    if(flag == global_protection_flag) {
        GLOBAL_PROT_FLAG[data_ch] = 0x01;
    } else if(flag == step_protection_flag) {
        STEP_PROT_FLAG[data_ch] = 0x01;
    }

    running_step[data_ch].status = status;
}

static void Standby_Protection(uint8_t ch)
{
    uint8_t data_ch;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        return;
    }
#endif
#if CHANNELS_4
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else {
        return;
    }
#endif
#if CHANNELS_8
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else if(ch & CH_5) {
        data_ch = 4;
    } else if(ch & CH_6) {
        data_ch = 5;
    } else if(ch & CH_7) {
        data_ch = 6;
    } else if(ch & CH_8) {
        data_ch = 7;
    } else {
        return;
    }
#endif

    if(SYS_MODE == SYS_NORMAL_MODE) {
        //单工步保护判断-----------------------------------------------------------------------------
        //电池温度上限保护
    #if STEP_STANDBY_battery_temperature_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_battery_temperature_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= step_protection[data_ch].protection_battery_temperature_limit)) {

            Protection_Action(ch, step_protection_flag, step_pause_status);
            // //执行相关动作---------------------------------------------------------------------------
            // PORT_DIS(2*data_ch);
            // PORT_DIS(2*data_ch+1);
            // DAC80508_SetDacData(2*data_ch, 0, 0);
            // DAC80508_SetDacData(2*data_ch, 0, 1);
            // DAC80508_SetDacData(2*data_ch+1, 0, 0);
            // DAC80508_SetDacData(2*data_ch+1, 0, 1);

            // STEP_PROT_FLAG = 1;
            // running_step[data_ch].status = step_pause_status;
            // //--------------------------------------------------------------------------------------

            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x00;
        }
    #endif

        //充电容量保护
    #if STEP_STANDBY_charge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_charge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x00;
        }
    #endif

        //放电容量保护
    #if STEP_STANDBY_discharge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_discharge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x00;
        }
    #endif

        //电压上限保护
    #if STEP_STANDBY_voltage_high_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_high_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= step_protection[data_ch].protection_voltage_high_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x00;
        }
    #endif

        //电压下限保护
    #if STEP_STANDBY_voltage_low_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_low_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= step_protection[data_ch].protection_voltage_low_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x00;
        }
    #endif

        //充电电流上限保护
    #if STEP_STANDBY_charge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_charge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x00;
        }
    #endif

        //放电电流上限保护
    #if STEP_STANDBY_discharge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_discharge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x00;
        }
    #endif

        //电流正跳幅度保护
    #if STEP_STANDBY_current_positive_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_positive_jump_range) > 0) && \
            (step_protection[data_ch].protection_current_positive_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) >= (step_protection[data_ch].protection_current_positive_jump_range)) {
                jump_protection_now[data_ch].current_positive_jump_number += 1;

                if(jump_protection_now[data_ch].current_positive_jump_number >= step_protection[data_ch].protection_current_positive_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
            }

        }
    #endif

        //电流负跳幅度保护
    #if STEP_STANDBY_current_negative_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_negative_jump_range) > 0) && \
            (step_protection[data_ch].protection_current_negative_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) <= (0 - step_protection[data_ch].protection_current_negative_jump_range)) {
                jump_protection_now[data_ch].current_negative_jump_number += 1;

                if(jump_protection_now[data_ch].current_negative_jump_number >= step_protection[data_ch].protection_current_negative_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
            }

        }
    #endif

        //电压正跳幅度保护
    #if STEP_STANDBY_voltage_positive_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_positive_jump_range) > 0) && \
            (step_protection[data_ch].protection_voltage_positive_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) >= (step_protection[data_ch].protection_voltage_positive_jump_range)) {
                jump_protection_now[data_ch].voltage_positive_jump_number += 1;

                if(jump_protection_now[data_ch].voltage_positive_jump_number >= step_protection[data_ch].protection_voltage_positive_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
            }

        }
    #endif

        //电压负跳幅度保护
    #if STEP_STANDBY_voltage_negative_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_negative_jump_range) > 0) && \
            (step_protection[data_ch].protection_voltage_negative_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) <= (0 - step_protection[data_ch].protection_voltage_negative_jump_range)) {
                jump_protection_now[data_ch].voltage_negative_jump_number += 1;

                if(jump_protection_now[data_ch].voltage_negative_jump_number >= step_protection[data_ch].protection_voltage_negative_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
            }

        }
    #endif

        //电流超差保护
    #if STEP_STANDBY_current_OutOfTolerance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_OutOfTolerance) > 0) && \
            (fabs(record_data.channel_record_data[data_ch].output_current-step_work[data_ch].step_work_current) >= step_protection[data_ch].protection_current_OutOfTolerance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x00;
        }
    #endif

        //电压超差保护
    #if STEP_STANDBY_voltage_OutOfTolerance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_OutOfTolerance) > 0) && \
            (fabs(record_data.channel_record_data[data_ch].battery_voltage-step_work[data_ch].step_work_voltage) >= step_protection[data_ch].protection_voltage_OutOfTolerance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x00;
        }
    #endif

        //------------------------------------------------------------------------------------------


        //全局保护判断-------------------------------------------------------------------------------
        //充电过压
    #if GLOBAL_STANDBY_charge_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= global_protection[data_ch].charge_OverVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x00;
        }
    #endif

        //充电过流
    #if GLOBAL_STANDBY_charge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].charge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //充电过容
    #if GLOBAL_STANDBY_charge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].charge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x00;
        }
    #endif

        //放电欠压
    #if GLOBAL_STANDBY_discharge_UnderVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_UnderVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].discharge_UnderVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x00;
        }
    #endif

        //放电过流
    #if GLOBAL_STANDBY_discharge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].discharge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //放电过容
    #if GLOBAL_STANDBY_discharge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].discharge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x00;
        }
    #endif

        //直流输出限流
    // #if GLOBAL_STANDBY_output_CurrentLimit
    //     if( (global_protection[data_ch].output_CurrentLimit != 0) && \
    //         (record_data.channel_record_data[data_ch].output_current) > global_protection[data_ch].output_CurrentLimit) {

    //         Protection_Action(ch, global_protection_flag, step_stop_status);

    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x01;
    //     } else {
    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x00;
    //     }
    // #endif

        //电池欠温报警
    #if GLOBAL_STANDBY_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_UnderTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature <= global_protection[data_ch].battery_UnderTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池过温预警
    #if GLOBAL_STANDBY_battery_OverTemperature_warning
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_warning) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_warning) && \
            (record_data.channel_record_data[data_ch].battery_temperature < global_protection[data_ch].battery_OverTemperature_alarm)) {

            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x01;

            GLOBAL_PROT_FLAG[data_ch] = 1;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x00;
        }
    #endif

        //电池过温报警
    #if GLOBAL_STANDBY_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池反接保护
    #if GLOBAL_STANDBY_reverse_battery
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].reverse_battery) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].reverse_battery)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x00;
        }
    #endif

        //接触阻抗保护
    #if GLOBAL_STANDBY_contact_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].contact_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].contact_impedance >= global_protection[data_ch].contact_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x00;
        }
    #endif

        //电池电压波动
    #if GLOBAL_STANDBY_fluctuation_BatteryVoltage
        if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_BatteryVoltage) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_BatteryVoltage) > 0)) {
            if((fluctuation_protection_now[data_ch].voltage_time - fluctuation_protection_old[data_ch].voltage_time) >= global_protection[data_ch].fluctuationtime_BatteryVoltage) {
                if(fluctuation_voltage_flag[data_ch] == 0) {
                    fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                    fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;

                    fluctuation_voltage_flag[data_ch] = 1;
                } else {
                    if(fabs(fluctuation_protection_now[data_ch].voltage - fluctuation_protection_old[data_ch].voltage) >= global_protection[data_ch].fluctuation_BatteryVoltage) {
                        Protection_Action(ch, global_protection_flag, step_stop_status);

                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x01;
                    } else {
                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;

                        fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                        fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;
                    }
                }

            } else {
                Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;
            }
        }
    #endif

        //输出电流波动保护
    #if GLOBAL_STANDBY_fluctuation_OutputCurrent
        if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_OutputCurrent) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_OutputCurrent) > 0)) {
            if((fluctuation_protection_now[data_ch].current_time - fluctuation_protection_old[data_ch].current_time) >= global_protection[data_ch].fluctuationtime_OutputCurrent) {
                if(fluctuation_current_flag[data_ch] == 0) {
                    fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                    fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;

                    fluctuation_current_flag[data_ch] = 1;
                } else {
                    if(fabs(fluctuation_protection_now[data_ch].current - fluctuation_protection_old[data_ch].current) >= global_protection[data_ch].fluctuation_OutputCurrent) {
                        Protection_Action(ch, global_protection_flag, step_stop_status);

                        Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x01;
                    } else {
                        Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;

                        fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                        fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;
                    }
                }

            } else {
                Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;
            }
        }
    #endif

        //模块端口输出过压
    #if GLOBAL_STANDBY_output_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].output_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].output_voltage >= (global_protection[data_ch].charge_OverVoltage+record_data.channel_record_data[data_ch].output_current*global_protection[data_ch].output_OverVoltage))) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x00;
        }
    #endif

        //回路阻抗保护
    #if GLOBAL_STANDBY_loop_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].loop_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].loop_impedance >= global_protection[data_ch].loop_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x00;
        }
    #endif

        //硬件保护
    #if GLOBAL_STANDBY_hardware_fault

    #if CHANNELS_4
        if(ch & CH_1) {
            if((Mod_PG1 == 1) || (Mod_PG2 == 1)) {
                PG_count[0]++;
                if(PG_count[0] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[0] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_2) {
            if((Mod_PG3 == 1) || (Mod_PG4 == 1)) {
                PG_count[1]++;
                if(PG_count[1] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[1] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_3) {
            if((Mod_PG5 == 1) || (Mod_PG6 == 1)) {
                PG_count[2]++;
                if(PG_count[2] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[2] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_4) {
            if((Mod_PG7 == 1) || (Mod_PG8 == 1)) {
                PG_count[3]++;
                if(PG_count[3] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[3] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        }
    #endif

    #endif

        //输出端短路故障
    #if GLOBAL_STANDBY_output_ShortCircuit
        if( (record_data.channel_record_data[data_ch].output_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //电池短路故障
    #if GLOBAL_STANDBY_battery_ShortCircuit
        if( (record_data.channel_record_data[data_ch].battery_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //风扇故障
    #if GLOBAL_STANDBY_fan_fault

    #endif

        //输入BUS低故障
    #if GLOBAL_STANDBY_BUS_low_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        }
    #endif

        //输入BUS高故障
    #if GLOBAL_STANDBY_BUS_high_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        }
    #endif

        //AC掉电故障
    #if GLOBAL_STANDBY_AC_dump

    #endif

        //模块过温报警
    #if GLOBAL_STANDBY_module_OverTemperature_alarm
        if(record_data.channel_record_data[data_ch].module_temperature >= MOD_OVER_TEMPERATURE) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //校准数据错误
    #if GLOBAL_STANDBY_calibration_data_error

    #endif

        //工步数据错误
    #if GLOBAL_STANDBY_step_data_error

    #endif

        //其它故障
    #if GLOBAL_STANDBY_other_fault

    #endif

        //------------------------------------------------------------------------------------------
    }
}

static void CC_Protection(uint8_t ch)
{
    uint8_t data_ch;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        return;
    }
#endif
#if CHANNELS_4
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else {
        return;
    }
#endif
#if CHANNELS_8
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else if(ch & CH_5) {
        data_ch = 4;
    } else if(ch & CH_6) {
        data_ch = 5;
    } else if(ch & CH_7) {
        data_ch = 6;
    } else if(ch & CH_8) {
        data_ch = 7;
    } else {
        return;
    }
#endif

    if(SYS_MODE == SYS_NORMAL_MODE) {
        //单工步保护判断-----------------------------------------------------------------------------
        //电池温度上限保护
    #if STEP_CC_battery_temperature_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_battery_temperature_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= step_protection[data_ch].protection_battery_temperature_limit)) {

            Protection_Action(ch, step_protection_flag, step_pause_status);

            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x00;
        }
    #endif

        //充电容量保护
    #if STEP_CC_charge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_charge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x00;
        }
    #endif

        //放电容量保护
    #if STEP_CC_discharge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_discharge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x00;
        }
    #endif

        //电压上限保护
    #if STEP_CC_voltage_high_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_high_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= step_protection[data_ch].protection_voltage_high_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x00;
        }
    #endif

        //电压下限保护
    #if STEP_CC_voltage_low_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_low_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= step_protection[data_ch].protection_voltage_low_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x00;
        }
    #endif

        //充电电流上限保护
    #if STEP_CC_charge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_charge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x00;
        }
    #endif

        //放电电流上限保护
    #if STEP_CC_discharge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_discharge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x00;
        }
    #endif

        //电流正跳幅度保护
    #if STEP_CC_current_positive_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_positive_jump_range) > 0) && \
            (step_protection[data_ch].protection_current_positive_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) >= (step_protection[data_ch].protection_current_positive_jump_range)) {
                jump_protection_now[data_ch].current_positive_jump_number += 1;

                if(jump_protection_now[data_ch].current_positive_jump_number >= step_protection[data_ch].protection_current_positive_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
            }

        }
    #endif

        //电流负跳幅度保护
    #if STEP_CC_current_negative_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_negative_jump_range) > 0) && \
            (step_protection[data_ch].protection_current_negative_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) <= (0 - step_protection[data_ch].protection_current_negative_jump_range)) {
                jump_protection_now[data_ch].current_negative_jump_number += 1;

                if(jump_protection_now[data_ch].current_negative_jump_number >= step_protection[data_ch].protection_current_negative_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
            }

        }
    #endif

        //电压正跳幅度保护
    #if STEP_CC_voltage_positive_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_positive_jump_range) > 0) && \
            (step_protection[data_ch].protection_voltage_positive_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) >= (step_protection[data_ch].protection_voltage_positive_jump_range)) {
                jump_protection_now[data_ch].voltage_positive_jump_number += 1;

                if(jump_protection_now[data_ch].voltage_positive_jump_number >= step_protection[data_ch].protection_voltage_positive_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
            }

        }
    #endif

        //电压负跳幅度保护
    #if STEP_CC_voltage_negative_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_negative_jump_range) > 0) && \
            (step_protection[data_ch].protection_voltage_negative_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) <= (0 - step_protection[data_ch].protection_voltage_negative_jump_range)) {
                jump_protection_now[data_ch].voltage_negative_jump_number += 1;

                if(jump_protection_now[data_ch].voltage_negative_jump_number >= step_protection[data_ch].protection_voltage_negative_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
            }

        }
    #endif

        //电流超差保护
    #if STEP_CC_current_OutOfTolerance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_OutOfTolerance) > 0) && \
            (fabs(record_data.channel_record_data[data_ch].output_current-step_work[data_ch].step_work_current) >= step_protection[data_ch].protection_current_OutOfTolerance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x00;
        }
    #endif

        //电压超差保护
    #if STEP_CC_voltage_OutOfTolerance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_OutOfTolerance) > 0) && \
            (fabs(record_data.channel_record_data[data_ch].battery_voltage-V_MAX) >= step_protection[data_ch].protection_voltage_OutOfTolerance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x00;
        }
    #endif

        //------------------------------------------------------------------------------------------


        //全局保护判断-------------------------------------------------------------------------------
        //充电过压
    #if GLOBAL_CC_charge_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= global_protection[data_ch].charge_OverVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x00;
        }
    #endif

        //充电过流
    #if GLOBAL_CC_charge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].charge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //充电过容
    #if GLOBAL_CC_charge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].charge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x00;
        }
    #endif

        //放电欠压
    #if GLOBAL_CC_discharge_UnderVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_UnderVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].discharge_UnderVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x00;
        }
    #endif

        //放电过流
    #if GLOBAL_CC_discharge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].discharge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //放电过容
    #if GLOBAL_CC_discharge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].discharge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x00;
        }
    #endif

    //     //直流输出限流
    // #if GLOBAL_CC_output_CurrentLimit
    //     if( (global_protection[data_ch].output_CurrentLimit != 0) && \
    //         (record_data.channel_record_data[data_ch].output_current) > global_protection[data_ch].output_CurrentLimit) {

    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x01;
    //     } else {
    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x00;
    //     }
    // #endif

        //电池欠温报警
    #if GLOBAL_STANDBY_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_UnderTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature <= global_protection[data_ch].battery_UnderTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池过温预警
    #if GLOBAL_CC_battery_OverTemperature_warning
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_warning) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_warning) && \
            (record_data.channel_record_data[data_ch].battery_temperature < global_protection[data_ch].battery_OverTemperature_alarm)) {

            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x01;

            GLOBAL_PROT_FLAG[data_ch] = 1;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x00;
        }
    #endif

        //电池过温报警
    #if GLOBAL_CC_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池反接保护
    #if GLOBAL_CC_reverse_battery
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].reverse_battery) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].reverse_battery)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x00;
        }
    #endif

        //接触阻抗保护
    #if GLOBAL_CC_contact_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].contact_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].contact_impedance >= global_protection[data_ch].contact_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x00;
        }
    #endif

        //电池电压波动
    #if GLOBAL_CC_fluctuation_BatteryVoltage
        if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_BatteryVoltage) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_BatteryVoltage) > 0)) {
            if((fluctuation_protection_now[data_ch].voltage_time - fluctuation_protection_old[data_ch].voltage_time) >= global_protection[data_ch].fluctuationtime_BatteryVoltage) {
                if(fluctuation_voltage_flag[data_ch] == 0) {
                    fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                    fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;

                    fluctuation_voltage_flag[data_ch] = 1;
                } else {
                    if(fabs(fluctuation_protection_now[data_ch].voltage - fluctuation_protection_old[data_ch].voltage) >= global_protection[data_ch].fluctuation_BatteryVoltage) {
                        Protection_Action(ch, global_protection_flag, step_stop_status);

                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x01;
                    } else {
                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;

                        fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                        fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;
                    }
                }
            } else {
                Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;
            }
        }
    #endif

        //输出电流波动保护
    #if GLOBAL_CC_fluctuation_OutputCurrent
        if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_OutputCurrent) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_OutputCurrent) > 0)) {
            if((fluctuation_protection_now[data_ch].current_time - fluctuation_protection_old[data_ch].current_time) >= global_protection[data_ch].fluctuationtime_OutputCurrent) {
                if(fluctuation_current_flag[data_ch] == 0) {
                    fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                    fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;

                    fluctuation_current_flag[data_ch] = 1;
                } else {
                    if(fabs(fluctuation_protection_now[data_ch].current - fluctuation_protection_old[data_ch].current) >= global_protection[data_ch].fluctuation_OutputCurrent) {
                        Protection_Action(ch, global_protection_flag, step_stop_status);

                        Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x01;
                    } else {
                        Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;

                        fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                        fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;
                    }
                }
            } else {
                Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;
            }
        }
    #endif

        //模块端口输出过压
    #if GLOBAL_CC_output_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].output_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].output_voltage >= (global_protection[data_ch].charge_OverVoltage+record_data.channel_record_data[data_ch].output_current*global_protection[data_ch].output_OverVoltage))) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x00;
        }
    #endif

        //回路阻抗保护
    #if GLOBAL_CC_loop_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].loop_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].loop_impedance >= global_protection[data_ch].loop_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x00;
        }
    #endif

        //硬件保护
    #if GLOBAL_CC_hardware_fault

    #if CHANNELS_4
        if(ch & CH_1) {
            if((Mod_PG1 == 1) || (Mod_PG2 == 1)) {
                PG_count[0]++;
                if(PG_count[0] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[0] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_2) {
            if((Mod_PG3 == 1) || (Mod_PG4 == 1)) {
                PG_count[1]++;
                if(PG_count[1] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[1] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_3) {
            if((Mod_PG5 == 1) || (Mod_PG6 == 1)) {
                PG_count[2]++;
                if(PG_count[2] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[2] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_4) {
            if((Mod_PG7 == 1) || (Mod_PG8 == 1)) {
                PG_count[3]++;
                if(PG_count[3] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[3] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        }
    #endif

    #endif

        //输出端短路故障
    #if GLOBAL_CC_output_ShortCircuit
        if( (record_data.channel_record_data[data_ch].output_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //电池短路故障
    #if GLOBAL_CC_battery_ShortCircuit
        if( (record_data.channel_record_data[data_ch].battery_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //风扇故障
    #if GLOBAL_CC_fan_fault

    #endif

        //输入BUS低故障
    #if GLOBAL_CC_BUS_low_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        }
    #endif

        //输入BUS高故障
    #if GLOBAL_CC_BUS_high_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        }
    #endif

        //AC掉电故障
    #if GLOBAL_CC_AC_dump

    #endif

        //模块过温报警
    #if GLOBAL_CC_module_OverTemperature_alarm
        if(record_data.channel_record_data[data_ch].module_temperature >= MOD_OVER_TEMPERATURE) {
            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //校准数据错误
    #if GLOBAL_CC_calibration_data_error

    #endif

        //工步数据错误
    #if GLOBAL_CC_step_data_error

    #endif

        //其它故障
    #if GLOBAL_CC_other_fault

    #endif

        //------------------------------------------------------------------------------------------
    }
}

static void CV_Protection(uint8_t ch)
{
    uint8_t data_ch;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        return;
    }
#endif
#if CHANNELS_4
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else {
        return;
    }
#endif
#if CHANNELS_8
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else if(ch & CH_5) {
        data_ch = 4;
    } else if(ch & CH_6) {
        data_ch = 5;
    } else if(ch & CH_7) {
        data_ch = 6;
    } else if(ch & CH_8) {
        data_ch = 7;
    } else {
        return;
    }
#endif

    if(SYS_MODE == SYS_NORMAL_MODE) {
        //单工步保护判断-----------------------------------------------------------------------------
        //电池温度上限保护
    #if STEP_CV_battery_temperature_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_battery_temperature_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= step_protection[data_ch].protection_battery_temperature_limit)) {

            Protection_Action(ch, step_protection_flag, step_pause_status);

            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x00;
        }
    #endif

        //充电容量保护
    #if STEP_CV_charge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_charge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x00;
        }
    #endif

        //放电容量保护
    #if STEP_CV_discharge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_discharge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x00;
        }
    #endif

        //电压上限保护
    #if STEP_CV_voltage_high_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_high_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= step_protection[data_ch].protection_voltage_high_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x00;
        }
    #endif

        //电压下限保护
    #if STEP_CV_voltage_low_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_low_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= step_protection[data_ch].protection_voltage_low_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x00;
        }
    #endif

        //充电电流上限保护
    #if STEP_CV_charge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_charge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x00;
        }
    #endif

        //放电电流上限保护
    #if STEP_CV_discharge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_discharge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x00;
        }
    #endif

        if(CV_mode[data_ch] == 0) {
            //电压正跳幅度保护
        #if STEP_CV_voltage_positive_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_positive_jump_range) > 0) && \
                (step_protection[data_ch].protection_voltage_positive_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) >= (step_protection[data_ch].protection_voltage_positive_jump_range)) {
                    jump_protection_now[data_ch].voltage_positive_jump_number += 1;

                    if(jump_protection_now[data_ch].voltage_positive_jump_number >= step_protection[data_ch].protection_voltage_positive_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
                }

            }
        #endif

            //电压负跳幅度保护
        #if STEP_CV_voltage_negative_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_negative_jump_range) > 0) && \
                (step_protection[data_ch].protection_voltage_negative_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) <= (0 - step_protection[data_ch].protection_voltage_negative_jump_range)) {
                    jump_protection_now[data_ch].voltage_negative_jump_number += 1;

                    if(jump_protection_now[data_ch].voltage_negative_jump_number >= step_protection[data_ch].protection_voltage_negative_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
                }

            }
        #endif

            //电流超差保护
        #if STEP_CV_current_OutOfTolerance
            if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_OutOfTolerance) > 0) && \
                (fabs(record_data.channel_record_data[data_ch].output_current-Ich_MAX) >= step_protection[data_ch].protection_current_OutOfTolerance)) {

                Protection_Action(ch, step_protection_flag, step_stop_status);

                Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x01;
            } else {
                Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x00;
            }
        #endif
        } else if(CV_mode[data_ch] == 1) {
            //电流正跳幅度保护
        #if STEP_CV_current_positive_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_positive_jump_range) > 0) && \
                (step_protection[data_ch].protection_current_positive_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) >= (step_protection[data_ch].protection_current_positive_jump_range)) {
                    jump_protection_now[data_ch].current_positive_jump_number += 1;

                    if(jump_protection_now[data_ch].current_positive_jump_number >= step_protection[data_ch].protection_current_positive_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
                }

            }
        #endif

            //电流负跳幅度保护
        #if STEP_CV_current_negative_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_negative_jump_range) > 0) && \
                (step_protection[data_ch].protection_current_negative_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) <= (0 - step_protection[data_ch].protection_current_negative_jump_range)) {
                    jump_protection_now[data_ch].current_negative_jump_number += 1;

                    if(jump_protection_now[data_ch].current_negative_jump_number >= step_protection[data_ch].protection_current_negative_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
                }

            }
        #endif

            //电压超差保护
        #if STEP_CV_voltage_OutOfTolerance
            if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_OutOfTolerance) > 0) && \
                (fabs(record_data.channel_record_data[data_ch].battery_voltage-step_work[data_ch].step_work_voltage) >= step_protection[data_ch].protection_voltage_OutOfTolerance)) {

                Protection_Action(ch, step_protection_flag, step_stop_status);

                Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x01;
            } else {
                Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x00;
            }
        #endif
        }

        //------------------------------------------------------------------------------------------


        //全局保护判断-------------------------------------------------------------------------------
        //充电过压
    #if GLOBAL_CV_charge_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= global_protection[data_ch].charge_OverVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x00;
        }
    #endif

        //充电过流
    #if GLOBAL_CV_charge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].charge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //充电过容
    #if GLOBAL_CV_charge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].charge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x00;
        }
    #endif

        //放电欠压
    #if GLOBAL_CV_discharge_UnderVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_UnderVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].discharge_UnderVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x00;
        }
    #endif

        //放电过流
    #if GLOBAL_CV_discharge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].discharge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //放电过容
    #if GLOBAL_CV_discharge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].discharge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x00;
        }
    #endif

    //     //直流输出限流
    // #if GLOBAL_CV_output_CurrentLimit
    //     if( (global_protection[data_ch].output_CurrentLimit != 0) && \
    //         (record_data.channel_record_data[data_ch].output_current) > global_protection[data_ch].output_CurrentLimit) {

    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x01;
    //     } else {
    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x00;
    //     }
    // #endif

        //电池欠温报警
    #if GLOBAL_STANDBY_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_UnderTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature <= global_protection[data_ch].battery_UnderTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池过温预警
    #if GLOBAL_CV_battery_OverTemperature_warning
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_warning) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_warning) && \
            (record_data.channel_record_data[data_ch].battery_temperature < global_protection[data_ch].battery_OverTemperature_alarm)) {

            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x01;

            GLOBAL_PROT_FLAG[data_ch] = 1;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x00;
        }
    #endif

        //电池过温报警
    #if GLOBAL_CV_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池反接保护
    #if GLOBAL_CV_reverse_battery
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].reverse_battery) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].reverse_battery)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x00;
        }
    #endif

        //接触阻抗保护
    #if GLOBAL_CV_contact_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].contact_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].contact_impedance >= global_protection[data_ch].contact_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x00;
        }
    #endif

        //电池电压波动
    #if GLOBAL_CV_fluctuation_BatteryVoltage
        if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_BatteryVoltage) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_BatteryVoltage) > 0)) {
            if((fluctuation_protection_now[data_ch].voltage_time - fluctuation_protection_old[data_ch].voltage_time) >= global_protection[data_ch].fluctuationtime_BatteryVoltage) {
                if(fluctuation_voltage_flag[data_ch] == 0) {
                    fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                    fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;

                    fluctuation_voltage_flag[data_ch] = 1;
                } else {
                    if(fabs(fluctuation_protection_now[data_ch].voltage - fluctuation_protection_old[data_ch].voltage) >= global_protection[data_ch].fluctuation_BatteryVoltage) {
                        Protection_Action(ch, global_protection_flag, step_stop_status);

                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x01;
                    } else {
                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;

                        fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                        fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;
                    }
                }
            } else {
                Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;
            }
        }
    #endif

        if(CV_mode[data_ch] == 0) {
            //输出电流波动保护
        #if GLOBAL_CV_fluctuation_OutputCurrent
            if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_OutputCurrent) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_OutputCurrent) > 0)) {
                if((fluctuation_protection_now[data_ch].current_time - fluctuation_protection_old[data_ch].current_time) >= global_protection[data_ch].fluctuationtime_OutputCurrent) {
                    if(fluctuation_current_flag[data_ch] == 0) {
                        fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                        fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;

                        fluctuation_current_flag[data_ch] = 1;
                    } else {
                        if(fabs(fluctuation_protection_now[data_ch].current - fluctuation_protection_old[data_ch].current) >= global_protection[data_ch].fluctuation_OutputCurrent) {
                            Protection_Action(ch, global_protection_flag, step_stop_status);

                            Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x01;
                        } else {
                            Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;

                            fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                            fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;
                        }
                    }
                } else {
                    Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;
                }
            }
        #endif
        }

    //模块端口输出过压
    #if GLOBAL_CV_output_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].output_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].output_voltage >= (global_protection[data_ch].charge_OverVoltage+record_data.channel_record_data[data_ch].output_current*global_protection[data_ch].output_OverVoltage))) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x00;
        }
    #endif

        //回路阻抗保护
    #if GLOBAL_CV_loop_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].loop_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].loop_impedance >= global_protection[data_ch].loop_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x00;
        }
    #endif

        //硬件保护
    #if GLOBAL_CV_hardware_fault

    #if CHANNELS_4
        if(ch & CH_1) {
            if((Mod_PG1 == 1) || (Mod_PG2 == 1)) {
                PG_count[0]++;
                if(PG_count[0] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[0] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_2) {
            if((Mod_PG3 == 1) || (Mod_PG4 == 1)) {
                PG_count[1]++;
                if(PG_count[1] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[1] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_3) {
            if((Mod_PG5 == 1) || (Mod_PG6 == 1)) {
                PG_count[2]++;
                if(PG_count[2] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[2] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_4) {
            if((Mod_PG7 == 1) || (Mod_PG8 == 1)) {
                PG_count[3]++;
                if(PG_count[3] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[3] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        }
    #endif

    #endif

        //输出端短路故障
    #if GLOBAL_CV_output_ShortCircuit
        if( (record_data.channel_record_data[data_ch].output_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //电池短路故障
    #if GLOBAL_CV_battery_ShortCircuit
        if( (record_data.channel_record_data[data_ch].battery_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //风扇故障
    #if GLOBAL_CV_fan_fault

    #endif

        //输入BUS低故障
    #if GLOBAL_CV_BUS_low_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        }
    #endif

        //输入BUS高故障
    #if GLOBAL_CV_BUS_high_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        }
    #endif

        //AC掉电故障
    #if GLOBAL_CV_AC_dump

    #endif

        //模块过温报警
    #if GLOBAL_CV_module_OverTemperature_alarm
        if(record_data.channel_record_data[data_ch].module_temperature >= MOD_OVER_TEMPERATURE) {
            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //校准数据错误
    #if GLOBAL_CV_calibration_data_error

    #endif

        //工步数据错误
    #if GLOBAL_CV_step_data_error

    #endif

        //其它故障
    #if GLOBAL_CV_other_fault

    #endif

        //------------------------------------------------------------------------------------------
    }
}

static void CC_to_CV_Protection(uint8_t ch)
{
    uint8_t data_ch;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        return;
    }
#endif
#if CHANNELS_4
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else {
        return;
    }
#endif
#if CHANNELS_8
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else if(ch & CH_5) {
        data_ch = 4;
    } else if(ch & CH_6) {
        data_ch = 5;
    } else if(ch & CH_7) {
        data_ch = 6;
    } else if(ch & CH_8) {
        data_ch = 7;
    } else {
        return;
    }
#endif

    if(SYS_MODE == SYS_NORMAL_MODE) {
        //单工步保护判断-----------------------------------------------------------------------------
        //电池温度上限保护
    #if STEP_CC_to_CV_battery_temperature_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_battery_temperature_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= step_protection[data_ch].protection_battery_temperature_limit)) {

            Protection_Action(ch, step_protection_flag, step_pause_status);

            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x00;
        }
    #endif

        //充电容量保护
    #if STEP_CC_to_CV_charge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_charge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x00;
        }
    #endif

        //放电容量保护
    #if STEP_CC_to_CV_discharge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_discharge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x00;
        }
    #endif

        //电压上限保护
    #if STEP_CC_to_CV_voltage_high_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_high_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= step_protection[data_ch].protection_voltage_high_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x00;
        }
    #endif

        //电压下限保护
    #if STEP_CC_to_CV_voltage_low_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_low_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= step_protection[data_ch].protection_voltage_low_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x00;
        }
    #endif

        //充电电流上限保护
    #if STEP_CC_to_CV_charge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_charge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x00;
        }
    #endif

        //放电电流上限保护
    #if STEP_CC_to_CV_discharge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_discharge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x00;
        }
    #endif

        if(CV_mode[data_ch] == 0) {
            //电压正跳幅度保护
        #if STEP_CC_to_CV_voltage_positive_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_positive_jump_range) > 0) && \
                (step_protection[data_ch].protection_voltage_positive_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) >= (step_protection[data_ch].protection_voltage_positive_jump_range)) {
                    jump_protection_now[data_ch].voltage_positive_jump_number += 1;

                    if(jump_protection_now[data_ch].voltage_positive_jump_number >= step_protection[data_ch].protection_voltage_positive_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
                }

            }
        #endif

            //电压负跳幅度保护
        #if STEP_CC_to_CV_voltage_negative_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_negative_jump_range) > 0) && \
                (step_protection[data_ch].protection_voltage_negative_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) <= (0 - step_protection[data_ch].protection_voltage_negative_jump_range)) {
                    jump_protection_now[data_ch].voltage_negative_jump_number += 1;

                    if(jump_protection_now[data_ch].voltage_negative_jump_number >= step_protection[data_ch].protection_voltage_negative_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
                }

            }
        #endif

            //电流超差保护
        #if STEP_CC_to_CV_current_OutOfTolerance
            if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_OutOfTolerance) > 0) && \
                (fabs(record_data.channel_record_data[data_ch].output_current-step_work[data_ch].step_work_current) >= step_protection[data_ch].protection_current_OutOfTolerance)) {

                Protection_Action(ch, step_protection_flag, step_stop_status);

                Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x01;
            } else {
                Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x00;
            }
        #endif
        } else if(CV_mode[data_ch] == 1) {
            //电流正跳幅度保护
        #if STEP_CC_to_CV_current_positive_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_positive_jump_range) > 0) && \
                (step_protection[data_ch].protection_current_positive_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) >= (step_protection[data_ch].protection_current_positive_jump_range)) {
                    jump_protection_now[data_ch].current_positive_jump_number += 1;

                    if(jump_protection_now[data_ch].current_positive_jump_number >= step_protection[data_ch].protection_current_positive_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
                }

            }
        #endif

            //电流负跳幅度保护
        #if STEP_CC_to_CV_current_negative_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_negative_jump_range) > 0) && \
                (step_protection[data_ch].protection_current_negative_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) <= (0 - step_protection[data_ch].protection_current_negative_jump_range)) {
                    jump_protection_now[data_ch].current_negative_jump_number += 1;

                    if(jump_protection_now[data_ch].current_negative_jump_number >= step_protection[data_ch].protection_current_negative_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
                }

            }
        #endif

            //电压超差保护
        #if STEP_CC_to_CV_voltage_OutOfTolerance
            if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_OutOfTolerance) > 0) && \
                (fabs(record_data.channel_record_data[data_ch].battery_voltage-step_work[data_ch].step_work_voltage) >= step_protection[data_ch].protection_voltage_OutOfTolerance)) {

                Protection_Action(ch, step_protection_flag, step_stop_status);

                Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x01;
            } else {
                Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x00;
            }
        #endif
        }




        //------------------------------------------------------------------------------------------


        //全局保护判断-------------------------------------------------------------------------------
        //充电过压
    #if GLOBAL_CC_to_CV_charge_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= global_protection[data_ch].charge_OverVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x00;
        }
    #endif

        //充电过流
    #if GLOBAL_CC_to_CV_charge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].charge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //充电过容
    #if GLOBAL_CC_to_CV_charge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].charge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x00;
        }
    #endif

        //放电欠压
    #if GLOBAL_CC_to_CV_discharge_UnderVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_UnderVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].discharge_UnderVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x00;
        }
    #endif

        //放电过流
    #if GLOBAL_CC_to_CV_discharge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].discharge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //放电过容
    #if GLOBAL_CC_to_CV_discharge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].discharge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x00;
        }
    #endif

    //     //直流输出限流
    // #if GLOBAL_CC_to_CV_output_CurrentLimit
    //     if( (global_protection[data_ch].output_CurrentLimit != 0) && \
    //         (record_data.channel_record_data[data_ch].output_current) > global_protection[data_ch].output_CurrentLimit) {

    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x01;
    //     } else {
    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x00;
    //     }
    // #endif

        //电池欠温报警
    #if GLOBAL_STANDBY_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_UnderTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature <= global_protection[data_ch].battery_UnderTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池过温预警
    #if GLOBAL_CC_to_CV_battery_OverTemperature_warning
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_warning) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_warning) && \
            (record_data.channel_record_data[data_ch].battery_temperature < global_protection[data_ch].battery_OverTemperature_alarm)) {

            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x01;

            GLOBAL_PROT_FLAG[data_ch] = 1;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x00;
        }
    #endif

        //电池过温报警
    #if GLOBAL_CC_to_CV_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池反接保护
    #if GLOBAL_CC_to_CV_reverse_battery
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].reverse_battery) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].reverse_battery)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x00;
        }
    #endif

        //接触阻抗保护
    #if GLOBAL_CC_to_CV_contact_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].contact_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].contact_impedance >= global_protection[data_ch].contact_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x00;
        }
    #endif

        //电池电压波动
    #if GLOBAL_CC_to_CV_fluctuation_BatteryVoltage
        if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_BatteryVoltage) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_BatteryVoltage) > 0)) {
            if((fluctuation_protection_now[data_ch].voltage_time - fluctuation_protection_old[data_ch].voltage_time) >= global_protection[data_ch].fluctuationtime_BatteryVoltage) {
                if(fluctuation_voltage_flag[data_ch] == 0) {
                    fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                    fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;

                    fluctuation_voltage_flag[data_ch] = 1;
                } else {
                    if(fabs(fluctuation_protection_now[data_ch].voltage - fluctuation_protection_old[data_ch].voltage) >= global_protection[data_ch].fluctuation_BatteryVoltage) {
                        Protection_Action(ch, global_protection_flag, step_stop_status);

                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x01;
                    } else {
                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;

                        fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                        fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;
                    }
                }
            } else {
                Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;
            }
        }
    #endif

        if(CV_mode[data_ch] == 0) {
            //输出电流波动保护
        #if GLOBAL_CC_to_CV_fluctuation_OutputCurrent
            if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_OutputCurrent) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_OutputCurrent) > 0)) {
                if((fluctuation_protection_now[data_ch].current_time - fluctuation_protection_old[data_ch].current_time) >= global_protection[data_ch].fluctuationtime_OutputCurrent) {
                    if(fluctuation_current_flag[data_ch] == 0) {
                        fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                        fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;

                        fluctuation_current_flag[data_ch] = 1;
                    } else {
                        if(fabs(fluctuation_protection_now[data_ch].current - fluctuation_protection_old[data_ch].current) >= global_protection[data_ch].fluctuation_OutputCurrent) {
                            Protection_Action(ch, global_protection_flag, step_stop_status);

                            Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x01;
                        } else {
                            Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;

                            fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                            fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;
                        }
                    }
                } else {
                    Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;
                }
            }
        #endif
        }


        //模块端口输出过压
    #if GLOBAL_CC_to_CV_output_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].output_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].output_voltage >= (global_protection[data_ch].charge_OverVoltage+record_data.channel_record_data[data_ch].output_current*global_protection[data_ch].output_OverVoltage))) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x00;
        }
    #endif

        //回路阻抗保护
    #if GLOBAL_CC_to_CV_loop_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].loop_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].loop_impedance >= global_protection[data_ch].loop_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x00;
        }
    #endif

        //硬件保护
    #if GLOBAL_CC_to_CV_hardware_fault

    #if CHANNELS_4
        if(ch & CH_1) {
            if((Mod_PG1 == 1) || (Mod_PG2 == 1)) {
                PG_count[0]++;
                if(PG_count[0] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[0] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_2) {
            if((Mod_PG3 == 1) || (Mod_PG4 == 1)) {
                PG_count[1]++;
                if(PG_count[1] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[1] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_3) {
            if((Mod_PG5 == 1) || (Mod_PG6 == 1)) {
                PG_count[2]++;
                if(PG_count[2] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[2] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_4) {
            if((Mod_PG7 == 1) || (Mod_PG8 == 1)) {
                PG_count[3]++;
                if(PG_count[3] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[3] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        }
    #endif

    #endif

        //输出端短路故障
    #if GLOBAL_CC_to_CV_output_ShortCircuit
        if( (record_data.channel_record_data[data_ch].output_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //电池短路故障
    #if GLOBAL_CC_to_CV_battery_ShortCircuit
        if( (record_data.channel_record_data[data_ch].battery_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //风扇故障
    #if GLOBAL_CC_to_CV_fan_fault

    #endif

        //输入BUS低故障
    #if GLOBAL_CC_to_CV_BUS_low_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        }
    #endif

        //输入BUS高故障
    #if GLOBAL_CC_to_CV_BUS_high_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        }
    #endif

        //AC掉电故障
    #if GLOBAL_CC_to_CV_AC_dump

    #endif

        //模块过温报警
    #if GLOBAL_CC_to_CV_module_OverTemperature_alarm
        if(record_data.channel_record_data[data_ch].module_temperature >= MOD_OVER_TEMPERATURE) {
            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //校准数据错误
    #if GLOBAL_CC_to_CV_calibration_data_error

    #endif

        //工步数据错误
    #if GLOBAL_CC_to_CV_step_data_error

    #endif

        //其它故障
    #if GLOBAL_CC_to_CV_other_fault

    #endif

        //------------------------------------------------------------------------------------------
    }
}

static void CP_Protection(uint8_t ch)
{
    uint8_t data_ch;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        return;
    }
#endif
#if CHANNELS_4
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else {
        return;
    }
#endif
#if CHANNELS_8
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else if(ch & CH_5) {
        data_ch = 4;
    } else if(ch & CH_6) {
        data_ch = 5;
    } else if(ch & CH_7) {
        data_ch = 6;
    } else if(ch & CH_8) {
        data_ch = 7;
    } else {
        return;
    }
#endif

    if(SYS_MODE == SYS_NORMAL_MODE) {
        //单工步保护判断-----------------------------------------------------------------------------
        //电池温度上限保护
    #if STEP_CP_battery_temperature_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_battery_temperature_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= step_protection[data_ch].protection_battery_temperature_limit)) {

            Protection_Action(ch, step_protection_flag, step_pause_status);

            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x00;
        }
    #endif

        //充电容量保护
    #if STEP_CP_charge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_charge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x00;
        }
    #endif

        //放电容量保护
    #if STEP_CP_discharge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_discharge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x00;
        }
    #endif

        //电压上限保护
    #if STEP_CP_voltage_high_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_high_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= step_protection[data_ch].protection_voltage_high_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x00;
        }
    #endif

        //电压下限保护
    #if STEP_CP_voltage_low_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_low_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= step_protection[data_ch].protection_voltage_low_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x00;
        }
    #endif

        //充电电流上限保护
    #if STEP_CP_charge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_charge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x00;
        }
    #endif

        //放电电流上限保护
    #if STEP_CP_discharge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_discharge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x00;
        }
    #endif

        //电流正跳幅度保护
    #if STEP_CP_current_positive_jump

    #endif

        //电流负跳幅度保护
    #if STEP_CP_current_negative_jump

    #endif

        //电压正跳幅度保护
    #if STEP_CP_voltage_positive_jump

    #endif

        //电压负跳幅度保护
    #if STEP_CP_voltage_negative_jump

    #endif

        //电流超差保护
    #if STEP_CP_current_OutOfTolerance

    #endif

        //电压超差保护
    #if STEP_CP_voltage_OutOfTolerance

    #endif

        //------------------------------------------------------------------------------------------


        //全局保护判断-------------------------------------------------------------------------------
        //充电过压
    #if GLOBAL_CP_charge_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= global_protection[data_ch].charge_OverVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x00;
        }
    #endif

        //充电过流
    #if GLOBAL_CP_charge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].charge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //充电过容
    #if GLOBAL_CP_charge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].charge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x00;
        }
    #endif

        //放电欠压
    #if GLOBAL_CP_discharge_UnderVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_UnderVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].discharge_UnderVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x00;
        }
    #endif

        //放电过流
    #if GLOBAL_CP_discharge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].discharge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //放电过容
    #if GLOBAL_CP_discharge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].discharge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x00;
        }
    #endif

    //     //直流输出限流
    // #if GLOBAL_CP_output_CurrentLimit
    //     if( (global_protection[data_ch].output_CurrentLimit != 0) && \
    //         (record_data.channel_record_data[data_ch].output_current) > global_protection[data_ch].output_CurrentLimit) {

    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x01;
    //     } else {
    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x00;
    //     }
    // #endif

        //电池欠温报警
    #if GLOBAL_STANDBY_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_UnderTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature <= global_protection[data_ch].battery_UnderTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池过温预警
    #if GLOBAL_CP_battery_OverTemperature_warning
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_warning) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_warning) && \
            (record_data.channel_record_data[data_ch].battery_temperature < global_protection[data_ch].battery_OverTemperature_alarm)) {

            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x01;

            GLOBAL_PROT_FLAG[data_ch] = 1;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x00;
        }
    #endif

        //电池过温报警
    #if GLOBAL_CP_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池反接保护
    #if GLOBAL_CP_reverse_battery
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].reverse_battery) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].reverse_battery)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x00;
        }
    #endif

        //接触阻抗保护
    #if GLOBAL_CP_contact_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].contact_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].contact_impedance >= global_protection[data_ch].contact_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x00;
        }
    #endif

        //电池电压波动
    #if GLOBAL_CP_fluctuation_BatteryVoltage

    #endif

        //输出电流波动保护
    #if GLOBAL_CP_fluctuation_OutputCurrent

    #endif

        //模块端口输出过压
    #if GLOBAL_CP_output_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].output_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].output_voltage >= global_protection[data_ch].output_OverVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x00;
        }
    #endif

        //回路阻抗保护
    #if GLOBAL_CP_loop_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].loop_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].loop_impedance >= global_protection[data_ch].loop_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x00;
        }
    #endif

        //硬件保护
    #if GLOBAL_CP_hardware_fault

    #endif

        //输出端短路故障
    #if GLOBAL_CP_output_ShortCircuit

    #endif

        //电池短路故障
    #if GLOBAL_CP_battery_ShortCircuit

    #endif

        //风扇故障
    #if GLOBAL_CP_fan_fault

    #endif

        //输入BUS低故障
    #if GLOBAL_CP_BUS_low_fault

    #endif

        //输入BUS高故障
    #if GLOBAL_CP_BUS_high_fault

    #endif

        //AC掉电故障
    #if GLOBAL_CP_AC_dump

    #endif

        //模块过温报警
    #if GLOBAL_CP_module_OverTemperature_alarm
        if(record_data.channel_record_data[data_ch].module_temperature >= MOD_OVER_TEMPERATURE) {
            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //校准数据错误
    #if GLOBAL_CP_calibration_data_error

    #endif

        //工步数据错误
    #if GLOBAL_CP_step_data_error

    #endif

        //其它故障
    #if GLOBAL_CP_other_fault

    #endif

        //------------------------------------------------------------------------------------------
    }
}

static void DCC_Protection(uint8_t ch)
{
    uint8_t data_ch;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        return;
    }
#endif
#if CHANNELS_4
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else {
        return;
    }
#endif
#if CHANNELS_8
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else if(ch & CH_5) {
        data_ch = 4;
    } else if(ch & CH_6) {
        data_ch = 5;
    } else if(ch & CH_7) {
        data_ch = 6;
    } else if(ch & CH_8) {
        data_ch = 7;
    } else {
        return;
    }
#endif

    if(SYS_MODE == SYS_NORMAL_MODE) {
        //单工步保护判断-----------------------------------------------------------------------------
        //电池温度上限保护
    #if STEP_DCC_battery_temperature_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_battery_temperature_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= step_protection[data_ch].protection_battery_temperature_limit)) {

            Protection_Action(ch, step_protection_flag, step_pause_status);

            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x00;
        }
    #endif

        //充电容量保护
    #if STEP_DCC_charge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_charge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x00;
        }
    #endif

        //放电容量保护
    #if STEP_DCC_discharge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_discharge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x00;
        }
    #endif

        //电压上限保护
    #if STEP_DCC_voltage_high_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_high_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= step_protection[data_ch].protection_voltage_high_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x00;
        }
    #endif

        //电压下限保护
    #if STEP_DCC_voltage_low_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_low_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= step_protection[data_ch].protection_voltage_low_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x00;
        }
    #endif

        //充电电流上限保护
    #if STEP_DCC_charge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_charge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x00;
        }
    #endif

        //放电电流上限保护
    #if STEP_DCC_discharge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_discharge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x00;
        }
    #endif

        //电流正跳幅度保护
    #if STEP_DCC_current_positive_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_positive_jump_range) > 0) && \
            (step_protection[data_ch].protection_current_positive_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) >= (step_protection[data_ch].protection_current_positive_jump_range)) {
                jump_protection_now[data_ch].current_positive_jump_number += 1;

                if(jump_protection_now[data_ch].current_positive_jump_number >= step_protection[data_ch].protection_current_positive_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
            }

        }
    #endif

        //电流负跳幅度保护
    #if STEP_DCC_current_negative_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_negative_jump_range) > 0) && \
            (step_protection[data_ch].protection_current_negative_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) <= (0 - step_protection[data_ch].protection_current_negative_jump_range)) {
                jump_protection_now[data_ch].current_negative_jump_number += 1;

                if(jump_protection_now[data_ch].current_negative_jump_number >= step_protection[data_ch].protection_current_negative_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
            }

        }
    #endif

        //电压正跳幅度保护
    #if STEP_DCC_voltage_positive_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_positive_jump_range) > 0) && \
            (step_protection[data_ch].protection_voltage_positive_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) >= (step_protection[data_ch].protection_voltage_positive_jump_range)) {
                jump_protection_now[data_ch].voltage_positive_jump_number += 1;

                if(jump_protection_now[data_ch].voltage_positive_jump_number >= step_protection[data_ch].protection_voltage_positive_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
            }

        }
    #endif

        //电压负跳幅度保护
    #if STEP_DCC_voltage_negative_jump
        if( (jump_flag[data_ch] == 1) && \
            (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_negative_jump_range) > 0) && \
            (step_protection[data_ch].protection_voltage_negative_jump_number > 0)) {

            jump_flag[data_ch] = 0;
            if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) <= (0 - step_protection[data_ch].protection_voltage_negative_jump_range)) {
                jump_protection_now[data_ch].voltage_negative_jump_number += 1;

                if(jump_protection_now[data_ch].voltage_negative_jump_number >= step_protection[data_ch].protection_voltage_negative_jump_number) {
                    Protection_Action(ch, step_protection_flag, step_stop_status);

                    Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x01;
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
                }
            } else {
                Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
            }

        }
    #endif

        //电流超差保护
    #if STEP_DCC_current_OutOfTolerance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_OutOfTolerance) > 0) && \
            (fabs(record_data.channel_record_data[data_ch].output_current-step_work[data_ch].step_work_current) >= step_protection[data_ch].protection_current_OutOfTolerance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x00;
        }
    #endif

        //电压超差保护
    #if STEP_DCC_voltage_OutOfTolerance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_OutOfTolerance) > 0) && \
            (fabs(record_data.channel_record_data[data_ch].battery_voltage-V_MAX) >= step_protection[data_ch].protection_voltage_OutOfTolerance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x00;
        }
    #endif

        //------------------------------------------------------------------------------------------


        //全局保护判断-------------------------------------------------------------------------------
        //充电过压
    #if GLOBAL_DCC_charge_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= global_protection[data_ch].charge_OverVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x00;
        }
    #endif

        //充电过流
    #if GLOBAL_DCC_charge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].charge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //充电过容
    #if GLOBAL_DCC_charge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].charge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x00;
        }
    #endif

        //放电欠压
    #if GLOBAL_DCC_discharge_UnderVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_UnderVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].discharge_UnderVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x00;
        }
    #endif

        //放电过流
    #if GLOBAL_DCC_discharge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].discharge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //放电过容
    #if GLOBAL_DCC_discharge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].discharge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x00;
        }
    #endif

    //     //直流输出限流
    // #if GLOBAL_DCC_output_CurrentLimit
    //     if( (global_protection[data_ch].output_CurrentLimit != 0) && \
    //         (record_data.channel_record_data[data_ch].output_current) > global_protection[data_ch].output_CurrentLimit) {

    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x01;
    //     } else {
    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x00;
    //     }
    // #endif

        //电池欠温报警
    #if GLOBAL_STANDBY_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_UnderTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature <= global_protection[data_ch].battery_UnderTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池过温预警
    #if GLOBAL_DCC_battery_OverTemperature_warning
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_warning) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_warning) && \
            (record_data.channel_record_data[data_ch].battery_temperature < global_protection[data_ch].battery_OverTemperature_alarm)) {

            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x01;

            GLOBAL_PROT_FLAG[data_ch] = 1;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x00;
        }
    #endif

        //电池过温报警
    #if GLOBAL_DCC_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池反接保护
    #if GLOBAL_DCC_reverse_battery
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].reverse_battery) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].reverse_battery)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x00;
        }
    #endif

        //接触阻抗保护
    #if GLOBAL_DCC_contact_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].contact_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].contact_impedance >= global_protection[data_ch].contact_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x00;
        }
    #endif

        //电池电压波动
    #if GLOBAL_DCC_fluctuation_BatteryVoltage
        if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_BatteryVoltage) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_BatteryVoltage) > 0)) {
            if((fluctuation_protection_now[data_ch].voltage_time - fluctuation_protection_old[data_ch].voltage_time) >= global_protection[data_ch].fluctuationtime_BatteryVoltage) {
                if(fluctuation_voltage_flag[data_ch] == 0) {
                    fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                    fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;

                    fluctuation_voltage_flag[data_ch] = 1;
                } else {
                    if(fabs(fluctuation_protection_now[data_ch].voltage - fluctuation_protection_old[data_ch].voltage) >= global_protection[data_ch].fluctuation_BatteryVoltage) {
                        Protection_Action(ch, global_protection_flag, step_stop_status);

                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x01;
                    } else {
                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;

                        fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                        fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;
                    }
                }
            } else {
                Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;
            }
        }
    #endif

        //输出电流波动保护
    #if GLOBAL_DCC_fluctuation_OutputCurrent
        if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_OutputCurrent) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_OutputCurrent) > 0)) {
            if((fluctuation_protection_now[data_ch].current_time - fluctuation_protection_old[data_ch].current_time) >= global_protection[data_ch].fluctuationtime_OutputCurrent) {
                if(fluctuation_current_flag[data_ch] == 0) {
                    fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                    fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;

                    fluctuation_current_flag[data_ch] = 1;
                } else {
                    if(fabs(fluctuation_protection_now[data_ch].current - fluctuation_protection_old[data_ch].current) >= global_protection[data_ch].fluctuation_OutputCurrent) {
                        Protection_Action(ch, global_protection_flag, step_stop_status);

                        Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x01;
                    } else {
                        Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;

                        fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                        fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;
                    }
                }
            } else {
                Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;
            }
        }
    #endif

        //模块端口输出欠压
    #if GLOBAL_DCC_output_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].output_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].output_voltage <= (global_protection[data_ch].discharge_UnderVoltage-record_data.channel_record_data[data_ch].output_current*global_protection[data_ch].output_OverVoltage))) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x00;
        }
    #endif

        //回路阻抗保护
    #if GLOBAL_DCC_loop_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].loop_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].loop_impedance >= global_protection[data_ch].loop_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x00;
        }
    #endif

        //硬件保护
    #if GLOBAL_DCC_hardware_fault

    #if CHANNELS_4
        if(ch & CH_1) {
            if((Mod_PG1 == 1) || (Mod_PG2 == 1)) {
                PG_count[0]++;
                if(PG_count[0] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[0] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_2) {
            if((Mod_PG3 == 1) || (Mod_PG4 == 1)) {
                PG_count[1]++;
                if(PG_count[1] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[1] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_3) {
            if((Mod_PG5 == 1) || (Mod_PG6 == 1)) {
                PG_count[2]++;
                if(PG_count[2] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[2] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_4) {
            if((Mod_PG7 == 1) || (Mod_PG8 == 1)) {
                PG_count[3]++;
                if(PG_count[3] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[3] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        }
    #endif

    #endif

        //输出端短路故障
    #if GLOBAL_DCC_output_ShortCircuit
        if( (record_data.channel_record_data[data_ch].output_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //电池短路故障
    #if GLOBAL_DCC_battery_ShortCircuit
        if( (record_data.channel_record_data[data_ch].battery_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //风扇故障
    #if GLOBAL_DCC_fan_fault

    #endif

        //输入BUS低故障
    #if GLOBAL_DCC_BUS_low_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        }
    #endif

        //输入BUS高故障
    #if GLOBAL_DCC_BUS_high_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        }
    #endif

        //AC掉电故障
    #if GLOBAL_DCC_AC_dump

    #endif

        //模块过温报警
    #if GLOBAL_DCC_module_OverTemperature_alarm
        if(record_data.channel_record_data[data_ch].module_temperature >= MOD_OVER_TEMPERATURE) {
            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //校准数据错误
    #if GLOBAL_DCC_calibration_data_error

    #endif

        //工步数据错误
    #if GLOBAL_DCC_step_data_error

    #endif

        //其它故障
    #if GLOBAL_DCC_other_fault

    #endif

        //------------------------------------------------------------------------------------------
    }
}

static void DCV_Protection(uint8_t ch)
{
    uint8_t data_ch;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        return;
    }
#endif
#if CHANNELS_4
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else {
        return;
    }
#endif
#if CHANNELS_8
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else if(ch & CH_5) {
        data_ch = 4;
    } else if(ch & CH_6) {
        data_ch = 5;
    } else if(ch & CH_7) {
        data_ch = 6;
    } else if(ch & CH_8) {
        data_ch = 7;
    } else {
        return;
    }
#endif

    if(SYS_MODE == SYS_NORMAL_MODE) {
        //单工步保护判断-----------------------------------------------------------------------------
        //电池温度上限保护
    #if STEP_DCV_battery_temperature_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_battery_temperature_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= step_protection[data_ch].protection_battery_temperature_limit)) {

            Protection_Action(ch, step_protection_flag, step_pause_status);

            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x00;
        }
    #endif

        //充电容量保护
    #if STEP_DCV_charge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_charge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x00;
        }
    #endif

        //放电容量保护
    #if STEP_DCV_discharge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_discharge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x00;
        }
    #endif

        //电压上限保护
    #if STEP_DCV_voltage_high_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_high_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= step_protection[data_ch].protection_voltage_high_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x00;
        }
    #endif

        //电压下限保护
    #if STEP_DCV_voltage_low_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_low_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= step_protection[data_ch].protection_voltage_low_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x00;
        }
    #endif

        //充电电流上限保护
    #if STEP_DCV_charge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_charge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x00;
        }
    #endif

        //放电电流上限保护
    #if STEP_DCV_discharge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_discharge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x00;
        }
    #endif

        if(DCV_mode[data_ch] == 0) {
             //电压正跳幅度保护
        #if STEP_DCV_voltage_positive_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_positive_jump_range) > 0) && \
                (step_protection[data_ch].protection_voltage_positive_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) >= (step_protection[data_ch].protection_voltage_positive_jump_range)) {
                    jump_protection_now[data_ch].voltage_positive_jump_number += 1;

                    if(jump_protection_now[data_ch].voltage_positive_jump_number >= step_protection[data_ch].protection_voltage_positive_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
                }

            }
        #endif

            //电压负跳幅度保护
        #if STEP_DCV_voltage_negative_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_negative_jump_range) > 0) && \
                (step_protection[data_ch].protection_voltage_negative_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) <= (0 - step_protection[data_ch].protection_voltage_negative_jump_range)) {
                    jump_protection_now[data_ch].voltage_negative_jump_number += 1;

                    if(jump_protection_now[data_ch].voltage_negative_jump_number >= step_protection[data_ch].protection_voltage_negative_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
                }

            }
        #endif

            //电流超差保护
        #if STEP_DCV_current_OutOfTolerance
            if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_OutOfTolerance) > 0) && \
                (fabs(record_data.channel_record_data[data_ch].output_current-Ich_MAX) >= step_protection[data_ch].protection_current_OutOfTolerance)) {

                Protection_Action(ch, step_protection_flag, step_stop_status);

                Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x01;
            } else {
                Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x00;
            }
        #endif
        } else if(DCV_mode[data_ch] == 1) {
            //电流正跳幅度保护
        #if STEP_DCV_current_positive_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_positive_jump_range) > 0) && \
                (step_protection[data_ch].protection_current_positive_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) >= (step_protection[data_ch].protection_current_positive_jump_range)) {
                    jump_protection_now[data_ch].current_positive_jump_number += 1;

                    if(jump_protection_now[data_ch].current_positive_jump_number >= step_protection[data_ch].protection_current_positive_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
                }

            }
        #endif

            //电流负跳幅度保护
        #if STEP_DCV_current_negative_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_negative_jump_range) > 0) && \
                (step_protection[data_ch].protection_current_negative_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) <= (0 - step_protection[data_ch].protection_current_negative_jump_range)) {
                    jump_protection_now[data_ch].current_negative_jump_number += 1;

                    if(jump_protection_now[data_ch].current_negative_jump_number >= step_protection[data_ch].protection_current_negative_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
                }

            }
        #endif

        //电压超差保护
        #if STEP_DCV_voltage_OutOfTolerance
            if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_OutOfTolerance) > 0) && \
                (fabs(record_data.channel_record_data[data_ch].battery_voltage-step_work[data_ch].step_work_voltage) >= step_protection[data_ch].protection_voltage_OutOfTolerance)) {

                Protection_Action(ch, step_protection_flag, step_stop_status);

                Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x01;
            } else {
                Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x00;
            }
        #endif
        }

        //------------------------------------------------------------------------------------------


        //全局保护判断-------------------------------------------------------------------------------
        //充电过压
    #if GLOBAL_DCV_charge_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= global_protection[data_ch].charge_OverVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x00;
        }
    #endif

        //充电过流
    #if GLOBAL_DCV_charge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].charge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //充电过容
    #if GLOBAL_DCV_charge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].charge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x00;
        }
    #endif

        //放电欠压
    #if GLOBAL_DCV_discharge_UnderVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_UnderVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].discharge_UnderVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x00;
        }
    #endif

        //放电过流
    #if GLOBAL_DCV_discharge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].discharge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //放电过容
    #if GLOBAL_DCV_discharge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].discharge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x00;
        }
    #endif

    //     //直流输出限流
    // #if GLOBAL_DCV_output_CurrentLimit
    //     if( (global_protection[data_ch].output_CurrentLimit != 0) && \
    //         (record_data.channel_record_data[data_ch].output_current) > global_protection[data_ch].output_CurrentLimit) {

    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x01;
    //     } else {
    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x00;
    //     }
    // #endif

        //电池欠温报警
    #if GLOBAL_STANDBY_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_UnderTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature <= global_protection[data_ch].battery_UnderTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池过温预警
    #if GLOBAL_DCV_battery_OverTemperature_warning
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_warning) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_warning) && \
            (record_data.channel_record_data[data_ch].battery_temperature < global_protection[data_ch].battery_OverTemperature_alarm)) {

            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x01;

            GLOBAL_PROT_FLAG[data_ch] = 1;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x00;
        }
    #endif

        //电池过温报警
    #if GLOBAL_DCV_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池反接保护
    #if GLOBAL_DCV_reverse_battery
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].reverse_battery) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].reverse_battery)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x00;
        }
    #endif

        //接触阻抗保护
    #if GLOBAL_DCV_contact_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].contact_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].contact_impedance >= global_protection[data_ch].contact_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x00;
        }
    #endif

        //电池电压波动
    #if GLOBAL_DCV_fluctuation_BatteryVoltage
        if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_BatteryVoltage) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_BatteryVoltage) > 0)) {
            if((fluctuation_protection_now[data_ch].voltage_time - fluctuation_protection_old[data_ch].voltage_time) >= global_protection[data_ch].fluctuationtime_BatteryVoltage) {
                if(fluctuation_voltage_flag[data_ch] == 0) {
                    fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                    fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;

                    fluctuation_voltage_flag[data_ch] = 1;
                } else {
                    if(fabs(fluctuation_protection_now[data_ch].voltage - fluctuation_protection_old[data_ch].voltage) >= global_protection[data_ch].fluctuation_BatteryVoltage) {
                        Protection_Action(ch, global_protection_flag, step_stop_status);

                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x01;
                    } else {
                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;

                        fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                        fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;
                    }
                }
            } else {
                Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;
            }
        }
    #endif

        if(DCV_mode[data_ch] == 0) {
            //输出电流波动保护
        #if GLOBAL_DCV_fluctuation_OutputCurrent
            if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_OutputCurrent) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_OutputCurrent) > 0)) {
                if((fluctuation_protection_now[data_ch].current_time - fluctuation_protection_old[data_ch].current_time) >= global_protection[data_ch].fluctuationtime_OutputCurrent) {
                    if(fluctuation_current_flag[data_ch] == 0) {
                        fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                        fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;

                        fluctuation_current_flag[data_ch] = 1;
                    } else {
                        if(fabs(fluctuation_protection_now[data_ch].current - fluctuation_protection_old[data_ch].current) >= global_protection[data_ch].fluctuation_OutputCurrent) {
                            Protection_Action(ch, global_protection_flag, step_stop_status);

                            Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x01;
                        } else {
                            Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;

                            fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                            fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;
                        }
                    }
                } else {
                    Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;
                }
            }
        #endif
        }


        //模块端口输出欠压
    #if GLOBAL_DCV_output_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].output_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].output_voltage <= (global_protection[data_ch].discharge_UnderVoltage-record_data.channel_record_data[data_ch].output_current*global_protection[data_ch].output_OverVoltage))) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x00;
        }
    #endif

        //回路阻抗保护
    #if GLOBAL_DCV_loop_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].loop_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].loop_impedance >= global_protection[data_ch].loop_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x00;
        }
    #endif

        //硬件保护
    #if GLOBAL_DCV_hardware_fault

    #if CHANNELS_4
        if(ch & CH_1) {
            if((Mod_PG1 == 1) || (Mod_PG2 == 1)) {
                PG_count[0]++;
                if(PG_count[0] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[0] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_2) {
            if((Mod_PG3 == 1) || (Mod_PG4 == 1)) {
                PG_count[1]++;
                if(PG_count[1] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[1] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_3) {
            if((Mod_PG5 == 1) || (Mod_PG6 == 1)) {
                PG_count[2]++;
                if(PG_count[2] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[2] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_4) {
            if((Mod_PG7 == 1) || (Mod_PG8 == 1)) {
                PG_count[3]++;
                if(PG_count[3] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[3] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        }
    #endif

    #endif

        //输出端短路故障
    #if GLOBAL_DCV_output_ShortCircuit
        if( (record_data.channel_record_data[data_ch].output_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //电池短路故障
    #if GLOBAL_DCV_battery_ShortCircuit
        if( (record_data.channel_record_data[data_ch].battery_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //风扇故障
    #if GLOBAL_DCV_fan_fault

    #endif

        //输入BUS低故障
    #if GLOBAL_DCV_BUS_low_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        }
    #endif

        //输入BUS高故障
    #if GLOBAL_DCV_BUS_high_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        }
    #endif

        //AC掉电故障
    #if GLOBAL_DCV_AC_dump

    #endif

        //模块过温报警
    #if GLOBAL_DCV_module_OverTemperature_alarm
        if(record_data.channel_record_data[data_ch].module_temperature >= MOD_OVER_TEMPERATURE) {
            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //校准数据错误
    #if GLOBAL_DCV_calibration_data_error

    #endif

        //工步数据错误
    #if GLOBAL_DCV_step_data_error

    #endif

        //其它故障
    #if GLOBAL_DCV_other_fault

    #endif

        //------------------------------------------------------------------------------------------
    }
}

static void DCC_to_DCV_Protection(uint8_t ch)
{
    uint8_t data_ch;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        return;
    }
#endif
#if CHANNELS_4
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else {
        return;
    }
#endif
#if CHANNELS_8
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else if(ch & CH_5) {
        data_ch = 4;
    } else if(ch & CH_6) {
        data_ch = 5;
    } else if(ch & CH_7) {
        data_ch = 6;
    } else if(ch & CH_8) {
        data_ch = 7;
    } else {
        return;
    }
#endif

    if(SYS_MODE == SYS_NORMAL_MODE) {
        //单工步保护判断-----------------------------------------------------------------------------
        //电池温度上限保护
    #if STEP_DCC_to_DCV_battery_temperature_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_battery_temperature_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= step_protection[data_ch].protection_battery_temperature_limit)) {

            Protection_Action(ch, step_protection_flag, step_pause_status);

            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x00;
        }
    #endif

        //充电容量保护
    #if STEP_DCC_to_DCV_charge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_charge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x00;
        }
    #endif

        //放电容量保护
    #if STEP_DCC_to_DCV_discharge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_discharge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x00;
        }
    #endif

        //电压上限保护
    #if STEP_DCC_to_DCV_voltage_high_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_high_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= step_protection[data_ch].protection_voltage_high_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x00;
        }
    #endif

        //电压下限保护
    #if STEP_DCC_to_DCV_voltage_low_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_low_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= step_protection[data_ch].protection_voltage_low_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x00;
        }
    #endif

        //充电电流上限保护
    #if STEP_DCC_to_DCV_charge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_charge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x00;
        }
    #endif

        //放电电流上限保护
    #if STEP_DCC_to_DCV_discharge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_discharge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x00;
        }
    #endif

        if(DCV_mode[data_ch] == 0) {
            //电压正跳幅度保护
        #if STEP_DCC_to_DCV_voltage_positive_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_positive_jump_range) > 0) && \
                (step_protection[data_ch].protection_voltage_positive_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) >= (step_protection[data_ch].protection_voltage_positive_jump_range)) {
                    jump_protection_now[data_ch].voltage_positive_jump_number += 1;

                    if(jump_protection_now[data_ch].voltage_positive_jump_number >= step_protection[data_ch].protection_voltage_positive_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_positive_jump_trigger] = 0x00;
                }

            }
        #endif

            //电压负跳幅度保护
        #if STEP_DCC_to_DCV_voltage_negative_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_negative_jump_range) > 0) && \
                (step_protection[data_ch].protection_voltage_negative_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].voltage - jump_protection_old[data_ch].voltage) <= (0 - step_protection[data_ch].protection_voltage_negative_jump_range)) {
                    jump_protection_now[data_ch].voltage_negative_jump_number += 1;

                    if(jump_protection_now[data_ch].voltage_negative_jump_number >= step_protection[data_ch].protection_voltage_negative_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_voltage_negative_jump_trigger] = 0x00;
                }

            }
        #endif

            //电流超差保护
        #if STEP_DCC_to_DCV_current_OutOfTolerance
            if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_OutOfTolerance) > 0) && \
                (fabs(record_data.channel_record_data[data_ch].output_current-step_work[data_ch].step_work_current) >= step_protection[data_ch].protection_current_OutOfTolerance)) {

                Protection_Action(ch, step_protection_flag, step_stop_status);

                Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x01;
            } else {
                Step_Protection_Flag[data_ch][step_current_OutOfTolerance_trigger] = 0x00;
            }
        #endif
        } else if(DCV_mode[data_ch] == 1) {
            //电流正跳幅度保护
        #if STEP_DCC_to_DCV_current_positive_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_positive_jump_range) > 0) && \
                (step_protection[data_ch].protection_current_positive_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) >= (step_protection[data_ch].protection_current_positive_jump_range)) {
                    jump_protection_now[data_ch].current_positive_jump_number += 1;

                    if(jump_protection_now[data_ch].current_positive_jump_number >= step_protection[data_ch].protection_current_positive_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_current_positive_jump_trigger] = 0x00;
                }

            }
        #endif

            //电流负跳幅度保护
        #if STEP_DCC_to_DCV_current_negative_jump
            if( (jump_flag[data_ch] == 1) && \
                (FLOAT_NOT_ZERO(step_protection[data_ch].protection_current_negative_jump_range) > 0) && \
                (step_protection[data_ch].protection_current_negative_jump_number > 0)) {

                jump_flag[data_ch] = 0;
                if((jump_protection_now[data_ch].current - jump_protection_old[data_ch].current) <= (0 - step_protection[data_ch].protection_current_negative_jump_range)) {
                    jump_protection_now[data_ch].current_negative_jump_number += 1;

                    if(jump_protection_now[data_ch].current_negative_jump_number >= step_protection[data_ch].protection_current_negative_jump_number) {
                        Protection_Action(ch, step_protection_flag, step_stop_status);

                        Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x01;
                    } else {
                        Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
                    }
                } else {
                    Step_Protection_Flag[data_ch][step_current_negative_jump_trigger] = 0x00;
                }

            }
        #endif

             //电压超差保护
        #if STEP_DCC_to_DCV_voltage_OutOfTolerance
            if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_OutOfTolerance) > 0) && \
                (fabs(record_data.channel_record_data[data_ch].battery_voltage-step_work[data_ch].step_work_voltage) >= step_protection[data_ch].protection_voltage_OutOfTolerance)) {

                Protection_Action(ch, step_protection_flag, step_stop_status);

                Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x01;
            } else {
                Step_Protection_Flag[data_ch][step_voltage_OutOfTolerance_trigger] = 0x00;
            }
        #endif
        }

        //------------------------------------------------------------------------------------------


        //全局保护判断-------------------------------------------------------------------------------
        //充电过压
    #if GLOBAL_DCC_to_DCV_charge_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= global_protection[data_ch].charge_OverVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x00;
        }
    #endif

        //充电过流
    #if GLOBAL_DCC_to_DCV_charge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].charge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //充电过容
    #if GLOBAL_DCC_to_DCV_charge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].charge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x00;
        }
    #endif

        //放电欠压
    #if GLOBAL_DCC_to_DCV_discharge_UnderVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_UnderVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].discharge_UnderVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x00;
        }
    #endif

        //放电过流
    #if GLOBAL_DCC_to_DCV_discharge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].discharge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //放电过容
    #if GLOBAL_DCC_to_DCV_discharge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].discharge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x00;
        }
    #endif

    //     //直流输出限流
    // #if GLOBAL_DCC_to_DCV_output_CurrentLimit
    //     if( (global_protection[data_ch].output_CurrentLimit != 0) && \
    //         (record_data.channel_record_data[data_ch].output_current) > global_protection[data_ch].output_CurrentLimit) {

    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x01;
    //     } else {
    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x00;
    //     }
    // #endif

        //电池欠温报警
    #if GLOBAL_STANDBY_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_UnderTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature <= global_protection[data_ch].battery_UnderTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池过温预警
    #if GLOBAL_DCC_to_DCV_battery_OverTemperature_warning
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_warning) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_warning) && \
            (record_data.channel_record_data[data_ch].battery_temperature < global_protection[data_ch].battery_OverTemperature_alarm)) {

            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x01;

            GLOBAL_PROT_FLAG[data_ch] = 1;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x00;
        }
    #endif

        //电池过温报警
    #if GLOBAL_DCC_to_DCV_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池反接保护
    #if GLOBAL_DCC_to_DCV_reverse_battery
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].reverse_battery) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].reverse_battery)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x00;
        }
    #endif

        //接触阻抗保护
    #if GLOBAL_DCC_to_DCV_contact_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].contact_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].contact_impedance >= global_protection[data_ch].contact_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x00;
        }
    #endif

        //电池电压波动
    #if GLOBAL_DCC_to_DCV_fluctuation_BatteryVoltage
        if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_BatteryVoltage) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_BatteryVoltage) > 0)) {
            if((fluctuation_protection_now[data_ch].voltage_time - fluctuation_protection_old[data_ch].voltage_time) >= global_protection[data_ch].fluctuationtime_BatteryVoltage) {
                if(fluctuation_voltage_flag[data_ch] == 0) {
                    fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                    fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;

                    fluctuation_voltage_flag[data_ch] = 1;
                } else {
                    if(fabs(fluctuation_protection_now[data_ch].voltage - fluctuation_protection_old[data_ch].voltage) >= global_protection[data_ch].fluctuation_BatteryVoltage) {
                        Protection_Action(ch, global_protection_flag, step_stop_status);

                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x01;
                    } else {
                        Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;

                        fluctuation_protection_old[data_ch].voltage_time = fluctuation_protection_now[data_ch].voltage_time;
                        fluctuation_protection_old[data_ch].voltage = fluctuation_protection_now[data_ch].voltage;
                    }
                }
            } else {
                Global_Protection_Flag[data_ch][fluctuation_BatteryVoltage_trigger] = 0x00;
            }
        }
    #endif

        if(DCV_mode[data_ch] == 0) {
            //输出电流波动保护
        #if GLOBAL_DCC_to_DCV_fluctuation_OutputCurrent
            if((FLOAT_NOT_ZERO(global_protection[data_ch].fluctuation_OutputCurrent) > 0) && (FLOAT_NOT_ZERO(global_protection[data_ch].fluctuationtime_OutputCurrent) > 0)) {
                if((fluctuation_protection_now[data_ch].current_time - fluctuation_protection_old[data_ch].current_time) >= global_protection[data_ch].fluctuationtime_OutputCurrent) {
                    if(fluctuation_current_flag[data_ch] == 0) {
                        fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                        fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;

                        fluctuation_current_flag[data_ch] = 1;
                    } else {
                        if(fabs(fluctuation_protection_now[data_ch].current - fluctuation_protection_old[data_ch].current) >= global_protection[data_ch].fluctuation_OutputCurrent) {
                            Protection_Action(ch, global_protection_flag, step_stop_status);

                            Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x01;
                        } else {
                            Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;

                            fluctuation_protection_old[data_ch].current_time = fluctuation_protection_now[data_ch].current_time;
                            fluctuation_protection_old[data_ch].current = fluctuation_protection_now[data_ch].current;
                        }
                    }
                } else {
                    Global_Protection_Flag[data_ch][fluctuation_OutputCurrent_trigger] = 0x00;
                }
            }
        #endif
        }


        //模块端口输出欠压
    #if GLOBAL_DCC_to_DCV_output_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].output_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].output_voltage <= (global_protection[data_ch].discharge_UnderVoltage-record_data.channel_record_data[data_ch].output_current*global_protection[data_ch].output_OverVoltage))) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x00;
        }
    #endif

        //回路阻抗保护
    #if GLOBAL_DCC_to_DCV_loop_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].loop_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].loop_impedance >= global_protection[data_ch].loop_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x00;
        }
    #endif

        //硬件保护
    #if GLOBAL_DCC_to_DCV_hardware_fault

    #if CHANNELS_4
        if(ch & CH_1) {
            if((Mod_PG1 == 1) || (Mod_PG2 == 1)) {
                PG_count[0]++;
                if(PG_count[0] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[0] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_2) {
            if((Mod_PG3 == 1) || (Mod_PG4 == 1)) {
                PG_count[1]++;
                if(PG_count[1] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[1] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_3) {
            if((Mod_PG5 == 1) || (Mod_PG6 == 1)) {
                PG_count[2]++;
                if(PG_count[2] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[2] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        } else if(ch & CH_4) {
            if((Mod_PG7 == 1) || (Mod_PG8 == 1)) {
                PG_count[3]++;
                if(PG_count[3] >= PG_NUMBER) {
                    Protection_Action(ch, global_protection_flag, step_stop_status);

                    Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x01;
                }
            } else {
                PG_count[3] = 0;
                Global_Protection_Flag[data_ch][hardware_fault_trigger] = 0x00;
            }
        }
    #endif

    #endif

        //输出端短路故障
    #if GLOBAL_DCC_to_DCV_output_ShortCircuit
        if( (record_data.channel_record_data[data_ch].output_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //电池短路故障
    #if GLOBAL_DCC_to_DCV_battery_ShortCircuit
        if( (record_data.channel_record_data[data_ch].battery_voltage <= 0.05) && \
            (record_data.channel_record_data[data_ch].output_current >= 165)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_ShortCircuit_trigger] = 0x00;
        }
    #endif

        //风扇故障
    #if GLOBAL_DCC_to_DCV_fan_fault

    #endif

        //输入BUS低故障
    #if GLOBAL_DCC_to_DCV_BUS_low_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 <= BUS_LOW_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_low_fault_trigger] = 0x00;
            }
        }
    #endif

        //输入BUS高故障
    #if GLOBAL_DCC_to_DCV_BUS_high_fault
        if((ch & CH_1) || (ch & CH_2)) {
            if(record_data.Bus1 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        } else if((ch & CH_3) || (ch & CH_4)) {
            if(record_data.Bus2 >= BUS_HIGH_LIMIT) {
                Protection_Action(ch, global_protection_flag, step_stop_status);

                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x01;
            } else {
                Global_Protection_Flag[data_ch][BUS_high_fault_trigger] = 0x00;
            }
        }
    #endif

        //AC掉电故障
    #if GLOBAL_DCC_to_DCV_AC_dump

    #endif

        //模块过温报警
    #if GLOBAL_DCC_to_DCV_module_OverTemperature_alarm
        if(record_data.channel_record_data[data_ch].module_temperature >= MOD_OVER_TEMPERATURE) {
            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //校准数据错误
    #if GLOBAL_DCC_to_DCV_calibration_data_error

    #endif

        //工步数据错误
    #if GLOBAL_DCC_to_DCV_step_data_error

    #endif

        //其它故障
    #if GLOBAL_DCC_to_DCV_other_fault

    #endif

        //------------------------------------------------------------------------------------------
    }
}

static void DCP_Protection(uint8_t ch)
{
    uint8_t data_ch;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        return;
    }
#endif
#if CHANNELS_4
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else {
        return;
    }
#endif
#if CHANNELS_8
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else if(ch & CH_3) {
        data_ch = 2;
    } else if(ch & CH_4) {
        data_ch = 3;
    } else if(ch & CH_5) {
        data_ch = 4;
    } else if(ch & CH_6) {
        data_ch = 5;
    } else if(ch & CH_7) {
        data_ch = 6;
    } else if(ch & CH_8) {
        data_ch = 7;
    } else {
        return;
    }
#endif

    if(SYS_MODE == SYS_NORMAL_MODE) {
        //单工步保护判断-----------------------------------------------------------------------------
        //电池温度上限保护
    #if STEP_DCP_battery_temperature_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_battery_temperature_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= step_protection[data_ch].protection_battery_temperature_limit)) {

            Protection_Action(ch, step_protection_flag, step_pause_status);

            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_battery_temperature_limit_trigger] = 0x00;
        }
    #endif

        //充电容量保护
    #if STEP_DCP_charge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_charge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_capacitance_trigger] = 0x00;
        }
    #endif

        //放电容量保护
    #if STEP_DCP_discharge_capacitance
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_capacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= step_protection[data_ch].protection_discharge_capacitance)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_capacitance_trigger] = 0x00;
        }
    #endif

        //电压上限保护
    #if STEP_DCP_voltage_high_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_high_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= step_protection[data_ch].protection_voltage_high_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_high_limit_trigger] = 0x00;
        }
    #endif

        //电压下限保护
    #if STEP_DCP_voltage_low_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_voltage_low_limit) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= step_protection[data_ch].protection_voltage_low_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_voltage_low_limit_trigger] = 0x00;
        }
    #endif

        //充电电流上限保护
    #if STEP_DCP_charge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_charge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_charge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_charge_current_limit_trigger] = 0x00;
        }
    #endif

        //放电电流上限保护
    #if STEP_DCP_discharge_current_limit
        if( (FLOAT_NOT_ZERO(step_protection[data_ch].protection_discharge_current_limit) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= step_protection[data_ch].protection_discharge_current_limit)) {

            Protection_Action(ch, step_protection_flag, step_stop_status);

            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x01;
        } else {
            Step_Protection_Flag[data_ch][step_discharge_current_limit_trigger] = 0x00;
        }
    #endif

        //电流正跳幅度保护
    #if STEP_DCP_current_positive_jump

    #endif

        //电流负跳幅度保护
    #if STEP_DCP_current_negative_jump

    #endif

        //电压正跳幅度保护
    #if STEP_DCP_voltage_positive_jump

    #endif

        //电压负跳幅度保护
    #if STEP_DCP_voltage_negative_jump

    #endif

        //电流超差保护
    #if STEP_DCP_current_OutOfTolerance

    #endif

        //电压超差保护
    #if STEP_DCP_voltage_OutOfTolerance

    #endif

        //------------------------------------------------------------------------------------------


        //全局保护判断-------------------------------------------------------------------------------
        //充电过压
    #if GLOBAL_DCP_charge_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage >= global_protection[data_ch].charge_OverVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverVoltage_trigger] = 0x00;
        }
    #endif

        //充电过流
    #if GLOBAL_DCP_charge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].charge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //充电过容
    #if GLOBAL_DCP_charge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].charge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].charge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][charge_OverCapacitance_trigger] = 0x00;
        }
    #endif

        //放电欠压
    #if GLOBAL_DCP_discharge_UnderVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_UnderVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].discharge_UnderVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_UnderVoltage_trigger] = 0x00;
        }
    #endif

        //放电过流
    #if GLOBAL_DCP_discharge_OverCurrent
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCurrent) > 0) && \
            (record_data.channel_record_data[data_ch].output_current >= global_protection[data_ch].discharge_OverCurrent)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCurrent_trigger] = 0x00;
        }
    #endif

        //放电过容
    #if GLOBAL_DCP_discharge_OverCapacitance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].discharge_OverCapacitance) > 0) && \
            (record_data.channel_record_data[data_ch].battery_capacitance >= global_protection[data_ch].discharge_OverCapacitance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][discharge_OverCapacitance_trigger] = 0x00;
        }
    #endif

    //     //直流输出限流
    // #if GLOBAL_DCP_output_CurrentLimit
    //     if( (global_protection[data_ch].output_CurrentLimit != 0) && \
    //         (record_data.channel_record_data[data_ch].output_current) > global_protection[data_ch].output_CurrentLimit) {

    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x01;
    //     } else {
    //         Global_Protection_Flag[data_ch][output_CurrentLimit_trigger] = 0x00;
    //     }
    // #endif

        //电池欠温报警
    #if GLOBAL_STANDBY_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_UnderTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature <= global_protection[data_ch].battery_UnderTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_UnderTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池过温预警
    #if GLOBAL_DCP_battery_OverTemperature_warning
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_warning) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_warning) && \
            (record_data.channel_record_data[data_ch].battery_temperature < global_protection[data_ch].battery_OverTemperature_alarm)) {

            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x01;

            GLOBAL_PROT_FLAG[data_ch] = 1;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_warning_trigger] = 0x00;
        }
    #endif

        //电池过温报警
    #if GLOBAL_DCP_battery_OverTemperature_alarm
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].battery_OverTemperature_alarm) > 0) && \
            (record_data.channel_record_data[data_ch].battery_temperature >= global_protection[data_ch].battery_OverTemperature_alarm)) {

            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][battery_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //电池反接保护
    #if GLOBAL_DCP_reverse_battery
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].reverse_battery) > 0) && \
            (record_data.channel_record_data[data_ch].battery_voltage <= global_protection[data_ch].reverse_battery)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][reverse_battery_trigger] = 0x00;
        }
    #endif

        //接触阻抗保护
    #if GLOBAL_DCP_contact_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].contact_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].contact_impedance >= global_protection[data_ch].contact_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][contact_impedance_trigger] = 0x00;
        }
    #endif

        //电池电压波动
    #if GLOBAL_DCP_fluctuation_BatteryVoltage

    #endif

        //输出电流波动保护
    #if GLOBAL_DCP_fluctuation_OutputCurrent

    #endif

        //模块端口输出欠压
    #if GLOBAL_DCP_output_OverVoltage
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].output_OverVoltage) > 0) && \
            (record_data.channel_record_data[data_ch].output_voltage <= global_protection[data_ch].output_OverVoltage)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][output_OverVoltage_trigger] = 0x00;
        }
    #endif

        //回路阻抗保护
    #if GLOBAL_DCP_loop_impedance
        if( (FLOAT_NOT_ZERO(global_protection[data_ch].loop_impedance) > 0) && \
            (record_data.channel_record_data[data_ch].loop_impedance >= global_protection[data_ch].loop_impedance)) {

            Protection_Action(ch, global_protection_flag, step_stop_status);

            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][loop_impedance_trigger] = 0x00;
        }
    #endif

        //硬件保护
    #if GLOBAL_DCP_hardware_fault

    #endif

        //输出端短路故障
    #if GLOBAL_DCP_output_ShortCircuit

    #endif

        //电池短路故障
    #if GLOBAL_DCP_battery_ShortCircuit

    #endif

        //风扇故障
    #if GLOBAL_DCP_fan_fault

    #endif

        //输入BUS低故障
    #if GLOBAL_DCP_BUS_low_fault

    #endif

        //输入BUS高故障
    #if GLOBAL_DCP_BUS_high_fault

    #endif

        //AC掉电故障
    #if GLOBAL_DCP_AC_dump

    #endif

        //模块过温报警
    #if GLOBAL_DCP_module_OverTemperature_alarm
        if(record_data.channel_record_data[data_ch].module_temperature >= MOD_OVER_TEMPERATURE) {
            Protection_Action(ch, global_protection_flag, step_pause_status);

            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x01;
        } else {
            Global_Protection_Flag[data_ch][module_OverTemperature_alarm_trigger] = 0x00;
        }
    #endif

        //校准数据错误
    #if GLOBAL_DCP_calibration_data_error

    #endif

        //工步数据错误
    #if GLOBAL_DCP_step_data_error

    #endif

        //其它故障
    #if GLOBAL_DCP_other_fault

    #endif

        //------------------------------------------------------------------------------------------
    }
}

// void Protection_Judgment(uint8_t ch)
void Protection_Judgment(void)
{
//     uint8_t data_ch;

// #if CHANNELS_1

// #endif
// #if CHANNELS_2

// #endif
// #if CHANNELS_4
//     if(ch & CH_1) {
//         data_ch = 0;
//     } else if(ch & CH_2) {
//         data_ch = 1;
//     } else if(ch & CH_3) {
//         data_ch = 2;
//     } else if(ch & CH_4) {
//         data_ch = 3;
//     } else {
//         return;
//     }
// #endif
// #if CHANNELS_8

// #endif

    for(int i = 0; i < CHANNELS; i++) {
        if(running_step[i].status == step_running_status) {
            if(running_step[i].protection_time >= 1000) {
                switch(running_step[i].type) {
                    case standby:
                    {
                        Standby_Protection(0x01<<i);
                        break;
                    }
                    case CC:
                    {
                        CC_Protection(0x01<<i);
                        break;
                    }
                    case CV:
                    {
                        CV_Protection(0x01<<i);
                        break;
                    }
                    case CC_to_CV:
                    {
                        CC_to_CV_Protection(0x01<<i);
                        break;
                    }
                    case CP:
                    {
                        CP_Protection(0x01<<i);
                        break;
                    }
                    case DCC:
                    {
                        DCC_Protection(0x01<<i);
                        break;
                    }
                    case DCV:
                    {
                        DCV_Protection(0x01<<i);
                        break;
                    }
                    case DCC_to_DCV:
                    {
                        DCC_to_DCV_Protection(0x01<<i);
                        break;
                    }
                    case DCP:
                    {
                        DCP_Protection(0x01<<i);
                        break;
                    }
                }
            }

        } else if((STEP_PROT_FLAG[i] == 0x00) && (GLOBAL_PROT_FLAG[i] == 0x00)) {
            //电池反接保护
            if( (FLOAT_NOT_ZERO(global_protection[i].reverse_battery) > 0) && \
                (record_data.channel_record_data[i].battery_voltage <= global_protection[i].reverse_battery)) {

                Protection_Action(0x01<<i, global_protection_flag, step_stop_status);

                Global_Protection_Flag[i][reverse_battery_trigger] = 0x01;
            } else {
                Global_Protection_Flag[i][reverse_battery_trigger] = 0x00;
            }

            //模块过温报警
            if(record_data.channel_record_data[i].module_temperature >= MOD_OVER_TEMPERATURE) {
                Protection_Action(0x01<<i, global_protection_flag, step_stop_status);

                Global_Protection_Flag[i][module_OverTemperature_alarm_trigger] = 0x01;
            } else {
                Global_Protection_Flag[i][module_OverTemperature_alarm_trigger] = 0x00;
            }

            //输出端短路故障

            //硬件保护
        #if CHANNELS_4
            if((0x01<<i) & CH_1) {
                if((Mod_PG1 == 1) || (Mod_PG2 == 1)) {
                    PG_count[0]++;
                    if(PG_count[0] >= PG_NUMBER) {
                        Protection_Action((0x01<<i), global_protection_flag, step_stop_status);

                        Global_Protection_Flag[i][hardware_fault_trigger] = 0x01;
                    }

                } else {
                    PG_count[0] = 0;
                    Global_Protection_Flag[i][hardware_fault_trigger] = 0x00;
                }
            } else if((0x01<<i) & CH_2) {
                if((Mod_PG3 == 1) || (Mod_PG4 == 1)) {
                    PG_count[1]++;
                    if(PG_count[1] >= PG_NUMBER) {
                        Protection_Action((0x01<<i), global_protection_flag, step_stop_status);

                        Global_Protection_Flag[i][hardware_fault_trigger] = 0x01;
                    }
                } else {
                    PG_count[1] = 0;
                    Global_Protection_Flag[i][hardware_fault_trigger] = 0x00;
                }
            } else if((0x01<<i) & CH_3) {
                if((Mod_PG5 == 1) || (Mod_PG6 == 1)) {
                    PG_count[2]++;
                    if(PG_count[2] >= PG_NUMBER) {
                        Protection_Action((0x01<<i), global_protection_flag, step_stop_status);

                        Global_Protection_Flag[i][hardware_fault_trigger] = 0x01;
                    }
                } else {
                    PG_count[2] = 0;
                    Global_Protection_Flag[i][hardware_fault_trigger] = 0x00;
                }
            } else if((0x01<<i) & CH_4) {
                if((Mod_PG7 == 1) || (Mod_PG8 == 1)) {
                    PG_count[3]++;
                    if(PG_count[3] >= PG_NUMBER) {
                        Protection_Action((0x01<<i), global_protection_flag, step_stop_status);

                        Global_Protection_Flag[i][hardware_fault_trigger] = 0x01;
                    }
                } else {
                    PG_count[3] = 0;
                    Global_Protection_Flag[i][hardware_fault_trigger] = 0x00;
                }
            }
        #endif

            //风扇故障

            //输入BUS低故障

            //输入BUS高故障

            //AC掉电故障

            //校准数据错误

            //工步数据错误

            //其它故障
        }

    }

}

