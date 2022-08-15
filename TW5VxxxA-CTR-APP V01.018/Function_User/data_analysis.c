#include "main.h"
#include "data_analysis.h"
#include "discharge.h"
#include "step_work.h"
#include <string.h>


GlobalProtection global_protection[CHANNELS] = {0};

RecordQuery recode_query[CHANNELS] = {0};
RecordQuery protection_recode[CHANNELS] = {0};

StepWork step_work[CHANNELS] = {0};
StepRecordCondition step_record_condition[CHANNELS] = {0};
StepProtection step_protection[CHANNELS] = {0};
StepRun step_run[CHANNELS] = {0};

uint8_t SYS_MODE = SYS_NORMAL_MODE;
int64_t Time_Stamp = 0;
uint8_t QUERY_PROTECTION_FLAG = 0x00;

uint8_t heartbeat_flag = 0x00;
uint16_t heartbeat_count = 0;

CalibrationParaments calibration[8];

// = { [0].Vsense_K = 0, [1].Vsense_K = 0, [2].Vsense_K = 0, [3].Vsense_K = 0, [4].Vsense_K = 0, [5].Vsense_K = 0, [6].Vsense_K = 0, [7].Vsense_K = 0,
//                                         [0].Vsense_B = 0, [1].Vsense_B = 0, [2].Vsense_B = 0, [3].Vsense_B = 0, [4].Vsense_B = 0, [5].Vsense_B = 0, [6].Vsense_B = 0, [7].Vsense_B = 0,
//                                         [0].Vmod_K   = 0, [1].Vmod_K   = 0, [2].Vmod_K   = 0, [3].Vmod_K   = 0, [4].Vmod_K   = 0, [5].Vmod_K   = 0, [6].Vmod_K   = 0, [7].Vmod_K   = 0,
//                                         [0].Vmod_B   = 0, [1].Vmod_B   = 0, [2].Vmod_B   = 0, [3].Vmod_B   = 0, [4].Vmod_B   = 0, [5].Vmod_B   = 0, [6].Vmod_B   = 0, [7].Vmod_B   = 0,
//                                         [0].Iout_HK  = 0, [1].Iout_HK  = 0, [2].Iout_HK  = 0, [3].Iout_HK  = 0, [4].Iout_HK  = 0, [5].Iout_HK  = 0, [6].Iout_HK  = 0, [7].Iout_HK  = 0,
//                                         [0].Iout_HB  = 0, [1].Iout_HB  = 0, [2].Iout_HB  = 0, [3].Iout_HB  = 0, [4].Iout_HB  = 0, [5].Iout_HB  = 0, [6].Iout_HB  = 0, [7].Iout_HB  = 0,
//                                         [0].Iout_LK  = 0, [1].Iout_LK  = 0, [2].Iout_LK  = 0, [3].Iout_LK  = 0, [4].Iout_LK  = 0, [5].Iout_LK  = 0, [6].Iout_LK  = 0, [7].Iout_LK  = 0,
//                                         [0].Iout_LB  = 0, [1].Iout_LB  = 0, [2].Iout_LB  = 0, [3].Iout_LB  = 0, [4].Iout_LB  = 0, [5].Iout_LB  = 0, [6].Iout_LB  = 0, [7].Iout_LB  = 0,
//                                         [0].Vset_sense_K = 0, [1].Vset_sense_K = 0, [2].Vset_sense_K = 0, [3].Vset_sense_K = 0, [4].Vset_sense_K = 0, [5].Vset_sense_K = 0, [6].Vset_sense_K = 0, [7].Vset_sense_K = 0,
//                                         [0].Vset_sense_B = 0, [1].Vset_sense_B = 0, [2].Vset_sense_B = 0, [3].Vset_sense_B = 0, [4].Vset_sense_B = 0, [5].Vset_sense_B = 0, [6].Vset_sense_B = 0, [7].Vset_sense_B = 0,
//                                         [0].Vset_mod_K = 0, [1].Vset_mod_K = 0, [2].Vset_mod_K = 0, [3].Vset_mod_K = 0, [4].Vset_mod_K = 0, [5].Vset_mod_K = 0, [6].Vset_mod_K = 0, [7].Vset_mod_K = 0,
//                                         [0].Vset_mod_B = 0, [1].Vset_mod_B = 0, [2].Vset_mod_B = 0, [3].Vset_mod_B = 0, [4].Vset_mod_B = 0, [5].Vset_mod_B = 0, [6].Vset_mod_B = 0, [7].Vset_mod_B = 0,
//                                         [0].Iset_HK = 0, [1].Iset_HK = 0, [2].Iset_HK = 0, [3].Iset_HK = 0, [4].Iset_HK = 0, [5].Iset_HK = 0, [6].Iset_HK = 0, [7].Iset_HK = 0,
//                                         [0].Iset_HB = 0, [1].Iset_HB = 0, [2].Iset_HB = 0, [3].Iset_HB = 0, [4].Iset_HB = 0, [5].Iset_HB = 0, [6].Iset_HB = 0, [7].Iset_HB = 0,
//                                         [0].Iset_LK = 0, [1].Iset_LK = 0, [2].Iset_LK = 0, [3].Iset_LK = 0, [4].Iset_LK = 0, [5].Iset_LK = 0, [6].Iset_LK = 0, [7].Iset_LK = 0,
//                                         [0].Iset_LB = 0, [1].Iset_LB = 0, [2].Iset_LB = 0, [3].Iset_LB = 0, [4].Iset_LB = 0, [5].Iset_LB = 0, [6].Iset_LB = 0, [7].Iset_LB = 0,
//                                         [0].Vj_K    = 0, [1].Vj_K    = 0, [2].Vj_K    = 0, [3].Vj_K    = 0, [4].Vj_K    = 0, [5].Vj_K    = 0, [6].Vj_K    = 0, [7].Vj_K    = 0,
//                                         [0].Vj_B    = 0, [1].Vj_B    = 0, [2].Vj_B    = 0, [3].Vj_B    = 0, [4].Vj_B    = 0, [5].Vj_B    = 0, [6].Vj_B    = 0, [7].Vj_B    = 0,
//                                       };

SpecialCalibration special_calibration = {  0, 0,
                                            0, 0,
                                            0, 0,
                                            0, 0,
                                            0, 0,};

CalibrationAD calibration_ad_add[8] = { 0,0,0,0,
                                        0,0,0,0,
                                        0,0,0,0,
                                        0,0,0,0,
                                        0,0,0,0,
                                        0,0,0,0,
                                        0,0,0,0,
                                        0,0,0,0,};
CalibrationAD calibration_ad_record[8] = {  0,0,0,0,
                                            0,0,0,0,
                                            0,0,0,0,
                                            0,0,0,0,
                                            0,0,0,0,
                                            0,0,0,0,
                                            0,0,0,0,
                                            0,0,0,0,};

CalibrationRun calibration_run[8] = {[0].run_flag = 0, [0].status = 0x00,
                                     [1].run_flag = 0, [1].status = 0x00,
                                     [2].run_flag = 0, [2].status = 0x00,
                                     [3].run_flag = 0, [3].status = 0x00,
                                     [4].run_flag = 0, [4].status = 0x00,
                                     [5].run_flag = 0, [5].status = 0x00,
                                     [6].run_flag = 0, [6].status = 0x00,
                                     [7].run_flag = 0, [7].status = 0x00,};

uint8_t Calibration_Read_Flag = 0;

static uint8_t Analysis_Flag = 0x00;


/***************************************************************************************************
**  全局保护类 --> 全局保护设置解析函数
**
**
***************************************************************************************************/
static void Global_Protection_Setting_Analysis(uint8_t ch, u8* data, uint16_t data_len, u8* temporary_buffer)
{
    uint8_t number = data_len / 5;
    //uint32_t temp32;
    uint16_t len = 0;
    uint16_t sent_len = 4;
    uint8_t sub_id;
    float ftemp;

    if(ch != 0x00) {
        for(int i = 0; i < CHANNELS; i++) {
            if(ch & (0x01<<i)) {
                for(int j = 0; j < number; j++) {
                    temporary_buffer[sent_len+2] = data[len];
                    sub_id = data[len];

                    if((sub_id >= charge_OverVoltage_setting) && (sub_id < global_protection_parament_end)) {
                        memcpy(&ftemp, data+len+1, 4);

                        //进行大小端转换
                    #if DATABIG_ENDIAN
                        ftemp = Float_Swap(ftemp);
                    #endif

                        switch(sub_id) {
                            case charge_OverVoltage_setting: {
                                global_protection[i].charge_OverVoltage = ftemp;
                                break;
                            }
                            case charge_OverCurrent_setting: {
                                global_protection[i].charge_OverCurrent = ftemp;
                                break;
                            }
                            case charge_OverCapacitance_setting: {
                                global_protection[i].charge_OverCapacitance = ftemp;
                                break;
                            }
                            case discharge_UnderVoltage_setting: {
                                global_protection[i].discharge_UnderVoltage = ftemp;
                                break;
                            }
                            case discharge_OverCurrent_setting: {
                                global_protection[i].discharge_OverCurrent = ftemp;
                                break;
                            }
                            case discharge_OverCapacitance_setting: {
                                global_protection[i].discharge_OverCapacitance = ftemp;
                                break;
                            }
                            case battery_UnderTemperature_alarm_setting: {
                                global_protection[i].battery_UnderTemperature_alarm = ftemp;
                                break;
                            }
                            case battery_OverTemperature_warning_setting: {
                                global_protection[i].battery_OverTemperature_warning = ftemp;
                                break;
                            }
                            case battery_OverTemperature_alarm_setting: {
                                global_protection[i].battery_OverTemperature_alarm = ftemp;
                                break;
                            }
                            case reverse_battery_setting: {
                                global_protection[i].reverse_battery = ftemp;
                                break;
                            }
                            case contact_impedance_setting: {
                                global_protection[i].contact_impedance = ftemp;
                                break;
                            }
                            case fluctuation_BatteryVoltage_setting: {
                                global_protection[i].fluctuation_BatteryVoltage = ftemp;
                                break;
                            }
                            case fluctuationtime_BatteryVoltage_setting: {
                                global_protection[i].fluctuationtime_BatteryVoltage = ftemp;
                                break;
                            }
                            case fluctuation_OutputCurrent_setting: {
                                global_protection[i].fluctuation_OutputCurrent = ftemp;
                                break;
                            }
                            case fluctuationtime_OutputCurrent_setting: {
                                global_protection[i].fluctuationtime_OutputCurrent = ftemp;
                                break;
                            }
                            case output_OverVoltage_resistance_setting: {
                                global_protection[i].output_OverVoltage = ftemp;
                                break;
                            }
                            case loop_impedance_setting: {
                                global_protection[i].loop_impedance = ftemp;
                                break;
                            }
                            case sum_capacitance_setting: {
                                global_protection[i].sum_capacitance = ftemp;
                                break;
                            }
                            case initial_voltage_high_setting: {
                                global_protection[i].initial_voltage_high = ftemp;
                                break;
                            }
                            case initial_voltage_low_setting: {
                                global_protection[i].initial_voltage_low = ftemp;
                                break;
                            }
                            case contact_time_setting: {
                                global_protection[i].contact_time = ftemp;
                                break;
                            }
                            case contact_current_setting: {
                                global_protection[i].contact_current = ftemp;
                                break;
                            }
                            case contact_voltage_variation_setting: {
                                global_protection[i].contact_voltage_variation = ftemp;
                                break;
                            }
                            case loop_voltage_difference_setting: {
                                global_protection[i].loop_voltage_difference = ftemp;
                                break;
                            }
                            default: {
                                break;
                            }
                        }

                        temporary_buffer[sent_len+3] = 0x01;
                    } else {
                        temporary_buffer[sent_len+3] = 0x00;
                    }

                    len += 5;
                    sent_len += 2;
                }
            }
        }

        sent_len += 2;
    #if DATABIG_ENDIAN
        sent_len = Endia_Swap16(sent_len);
    #endif
        memcpy(temporary_buffer+2, &sent_len, 2);

        Analysis_Flag = 0x01;
    } else {
        Analysis_Flag = 0x00;
    }

}


/***************************************************************************************************
**  全局保护类 --> 全局保护查询解析函数
**
**
***************************************************************************************************/
static void Global_Protection_Query_Analysis(uint8_t ch, u8* data, uint16_t data_len, u8* temporary_buffer)
{
    uint8_t number = data_len;
    //uint32_t temp32;
    uint8_t data_ch = 0;
    uint16_t len = 0;
    uint16_t sent_len = 4;
    uint8_t sub_id;
    float ftemp;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        Analysis_Flag = 0x00;
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        Analysis_Flag = 0x00;
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
        Analysis_Flag = 0x00;
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
        Analysis_Flag = 0x00;
        return;
    }
#endif

    for(int i = 0; i < number; i++) {
        temporary_buffer[sent_len+2] = data[len];
        sub_id = data[len];

        if((sub_id >= charge_OverVoltage_setting) && (sub_id < global_protection_parament_end)) {
            switch(sub_id) {
                case charge_OverVoltage_setting: {
                    ftemp = global_protection[data_ch].charge_OverVoltage;
                    break;
                }
                case charge_OverCurrent_setting: {
                    ftemp = global_protection[data_ch].charge_OverCurrent;
                    break;
                }
                case charge_OverCapacitance_setting: {
                    ftemp = global_protection[data_ch].charge_OverCapacitance;
                    break;
                }
                case discharge_UnderVoltage_setting: {
                    ftemp = global_protection[data_ch].discharge_UnderVoltage;
                    break;
                }
                case discharge_OverCurrent_setting: {
                    ftemp = global_protection[data_ch].discharge_OverCurrent;
                    break;
                }
                case discharge_OverCapacitance_setting: {
                    ftemp = global_protection[data_ch].discharge_OverCapacitance;
                    break;
                }
                case battery_UnderTemperature_alarm_setting: {
                    ftemp = global_protection[data_ch].battery_UnderTemperature_alarm;
                    break;
                }
                case battery_OverTemperature_warning_setting: {
                    ftemp = global_protection[data_ch].battery_OverTemperature_warning;
                    break;
                }
                case battery_OverTemperature_alarm_setting: {
                    ftemp = global_protection[data_ch].battery_OverTemperature_alarm;
                    break;
                }
                case reverse_battery_setting: {
                    ftemp = global_protection[data_ch].reverse_battery;
                    break;
                }
                case contact_impedance_setting: {
                    ftemp = global_protection[data_ch].contact_impedance;
                    break;
                }
                case fluctuation_BatteryVoltage_setting: {
                    ftemp = global_protection[data_ch].fluctuation_BatteryVoltage;
                    break;
                }
                case fluctuationtime_BatteryVoltage_setting: {
                    ftemp = global_protection[data_ch].fluctuationtime_BatteryVoltage;
                    break;
                }
                case fluctuation_OutputCurrent_setting: {
                    ftemp = global_protection[data_ch].fluctuation_OutputCurrent;
                    break;
                }
                case fluctuationtime_OutputCurrent_setting: {
                    ftemp = global_protection[data_ch].fluctuationtime_OutputCurrent;
                    break;
                }
                case output_OverVoltage_resistance_setting: {
                    ftemp = global_protection[data_ch].output_OverVoltage;
                    break;
                }
                case loop_impedance_setting: {
                    ftemp = global_protection[data_ch].loop_impedance;
                    break;
                }
                default: {
                    ftemp = 0.0;
                    break;
                }
            }

            //进行大小端转换
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif

            memcpy(temporary_buffer+2+sent_len+1, &ftemp, 4);
        } else {

        }

        len += 1;
        sent_len += 5;
    }

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;
}


// /***************************************************************************************************
// **  记录类 --> 记录条件设置解析函数
// **
// **
// ***************************************************************************************************/
// static void Record_Setting_Analysis(uint8_t ch, u8* data, uint16_t data_len, u8* temporary_buffer)
// {
//     uint8_t number = data_len / 5;
//     uint32_t temporary_data;
//     uint16_t len = 0;
//     uint16_t sent_len = 4;
//     float temp;
//     uint16_t CRC_16_code = 0x0000;

//     for(uint8_t i = 0; i < number; i++) {
//         temporary_buffer[sent_len+2] = data[len];

//         if(data[len] != 0xff) {
//             memcpy(temporary_data, data+1+len, 4);
//             temp = temporary_data / 10000;

//         #if CHANNELS_1
//             record_ch1[data[len]] = temp;
//         #endif
//         #if CHANNELS_2
//             if(ch & 0x01) {
//                 record_ch1[data[len]] = temp;
//             }
//             if(ch & 0x02) {
//                 record_ch2[data[len]] = temp;
//             }
//         #endif
//         #if CHANNELS_4
//             if(ch & 0x01) {
//                 record_ch1[data[len]] = temp;
//             }
//             if(ch & 0x02) {
//                 record_ch2[data[len]] = temp;
//             }
//             if(ch & 0x04) {
//                 record_ch3[data[len]] = temp;
//             }
//             if(ch & 0x08) {
//                 record_ch4[data[len]] = temp;
//             }
//         #endif
//         #if CHANNELS_8
//             if(ch & 0x01) {
//                 record_ch1[data[len]] = temp;
//             }
//             if(ch & 0x02) {
//                 record_ch2[data[len]] = temp;
//             }
//             if(ch & 0x04) {
//                 record_ch3[data[len]] = temp;
//             }
//             if(ch & 0x08) {
//                 record_ch4[data[len]] = temp;
//             }
//             if(ch & 0x10) {
//                 record_ch5[data[len]] = temp;
//             }
//             if(ch & 0x20) {
//                 record_ch6[data[len]] = temp;
//             }
//             if(ch & 0x40) {
//                 record_ch7[data[len]] = temp;
//             }
//             if(ch & 0x80) {
//                 record_ch8[data[len]] = temp;
//             }
//         #endif

//             temporary_buffer[sent_len+3] = 0x01;
//         } else {
//             temporary_buffer[sent_len+3] = 0x00;
//         }

//         len += 5;
//         sent_len += 2;
//     }

//     memcpy(temporary_buffer+sent_len+2, &CRC_16_code, 2);
//     sent_len += 2;
//     memcpy(temporary_buffer+2, &sent_len, 2);
// }


/***************************************************************************************************
**  记录类 --> 记录数据查询解析函数
**
**
***************************************************************************************************/
static void Record_Query_Analysis(uint8_t query_ch, u8* temporary_buffer)
{
    uint8_t ch = 0;
    uint16_t sent_len = 4;
    //uint32_t temp32;
    uint16_t temp16;
    float ftemp;
    int64_t temp64;
    //uint16_t CRC_16_code = 0x0000;

#if CHANNELS_1
    if(query_ch & 0x01) {
        ch = 0;
    } else {
        Analysis_Flag = 0x00;
        return;
    }
#endif
#if CHANNELS_2
    if(query_ch & 0x01) {
        ch = 0;
    } else if(query_ch & 0x02) {
        ch = 1;
    } else {
        Analysis_Flag = 0x00;
        return;
    }
#endif
#if CHANNELS_4
    if(query_ch & 0x01) {
        ch = 0;
    } else if(query_ch & 0x02) {
        ch = 1;
    } else if(query_ch & 0x04) {
        ch = 2;
    } else if(query_ch & 0x08) {
        ch = 3;
    } else {
        Analysis_Flag = 0x00;
        return;
    }
#endif
#if CHANNELS_8
    if(query_ch & 0x01) {
        ch = 0;
    } else if(query_ch & 0x02) {
        ch = 1;
    } else if(query_ch & 0x04) {
        ch = 2;
    } else if(query_ch & 0x08) {
        ch = 3;
    } else if(query_ch & 0x10) {
        ch = 4;
    } else if(query_ch & 0x20) {
        ch = 5;
    } else if(query_ch & 0x40) {
        ch = 6;
    } else if(query_ch & 0x80) {
        ch = 7;
    } else {
        Analysis_Flag = 0x00;
        return;
    }
#endif

    recode_query[ch].time_stamp = Time_Stamp;
    recode_query[ch].run_status = running_step[ch].status;
    recode_query[ch].step_type = running_step[ch].type;
    recode_query[ch].step_number = running_step[ch].number;
    recode_query[ch].step_sum = running_step[ch].sum_number;

    if((running_circle[ch][2].circle_flag == 0x01) && (running_circle[ch][2].circle_error == 0x00)) {
        recode_query[ch].circle_serial = running_circle[ch][2].circle_serial;
        recode_query[ch].circle_count = running_circle[ch][2].circle_count;
    } else if((running_circle[ch][1].circle_flag == 0x01) && (running_circle[ch][1].circle_error == 0x00)) {
        recode_query[ch].circle_serial = running_circle[ch][1].circle_serial;
        recode_query[ch].circle_count = running_circle[ch][1].circle_count;
    } else if((running_circle[ch][0].circle_flag == 0x01) && (running_circle[ch][0].circle_error == 0x00)) {
        recode_query[ch].circle_serial = running_circle[ch][0].circle_serial;
        recode_query[ch].circle_count = running_circle[ch][0].circle_count;
    } else {
        recode_query[ch].circle_serial = 0;
        recode_query[ch].circle_count = 0;
    }

    // recode_query[ch].run_time = record_data.channel_record_data[ch].run_time / 1000;
    recode_query[ch].run_time = running_step[ch].run_time / 1000.0;
    recode_query[ch].battery_voltage = record_data.channel_record_data[ch].battery_voltage;

    // recode_query[ch].output_current = record_data.channel_record_data[ch].output_current;
    if((recode_query[ch].run_status == step_running_status) && (recode_query[ch].step_type != standby)) {
        recode_query[ch].output_current = record_data.channel_record_data[ch].output_current;
    #if CHANNELS_1
        if(Dir_flag[8*ch] == BOOST_MODE) {
            recode_query[ch].output_current = 0 - record_data.channel_record_data[ch].output_current;
        }
    #endif
    #if CHANNELS_2
        if(Dir_flag[4*ch] == BOOST_MODE) {
            recode_query[ch].output_current = 0 - record_data.channel_record_data[ch].output_current;
        }
    #endif
    #if CHANNELS_4
        if(Dir_flag[2*ch] == BOOST_MODE) {
            recode_query[ch].output_current = 0 - record_data.channel_record_data[ch].output_current;
        }

    #endif
    #if CHANNELS_8
        if(Dir_flag[ch] == BOOST_MODE) {
            recode_query[ch].output_current = 0 - record_data.channel_record_data[ch].output_current;
        }
    #endif
    } else {
        recode_query[ch].output_current = 0.0;
    }

    recode_query[ch].output_voltage = record_data.channel_record_data[ch].output_voltage;
    recode_query[ch].battery_capacitance = record_data.channel_record_data[ch].battery_capacitance;
    recode_query[ch].battery_energy = record_data.channel_record_data[ch].battery_energy;
    recode_query[ch].battery_temperature = record_data.channel_record_data[ch].battery_temperature;
    recode_query[ch].module_temperature = record_data.channel_record_data[ch].module_temperature;
    recode_query[ch].crosscurrent_ratio = record_data.channel_record_data[ch].crosscurrent_ratio;

    recode_query[ch].contact_impedance = record_data.channel_record_data[ch].contact_impedance;
    recode_query[ch].loop_impedance = record_data.channel_record_data[ch].loop_impedance;
    // if(recode_query[ch].output_current >= 5.0) {
    //     recode_query[ch].contact_impedance = record_data.channel_record_data[ch].contact_impedance;
    //     recode_query[ch].loop_impedance = record_data.channel_record_data[ch].loop_impedance;
    // } else {
    //     recode_query[ch].contact_impedance = 0;
    //     recode_query[ch].loop_impedance = 0;
    // }

    temp64 = recode_query[ch].time_stamp;
#if DATABIG_ENDIAN
    temp64 = Endia_Swap64(temp64);
#endif
    memcpy(temporary_buffer+sent_len+2, &temp64, 8);         sent_len += 8;

    temporary_buffer[sent_len+2] = recode_query[ch].run_status;
    temporary_buffer[sent_len+3] = recode_query[ch].step_type;
    sent_len += 2;

    temp16 = recode_query[ch].step_number;
#if DATABIG_ENDIAN
    temp16 = Endia_Swap16(temp16);
#endif
    memcpy(temporary_buffer+sent_len+2, &temp16, 2);        sent_len += 2;

    temp16 = recode_query[ch].step_sum;
#if DATABIG_ENDIAN
    temp16 = Endia_Swap16(temp16);
#endif
    memcpy(temporary_buffer+sent_len+2, &temp16, 2);        sent_len += 2;

    temp16 = recode_query[ch].circle_serial;
#if DATABIG_ENDIAN
    temp16 = Endia_Swap16(temp16);
#endif
    memcpy(temporary_buffer+sent_len+2, &temp16, 2);        sent_len += 2;

    temp16 = recode_query[ch].circle_count;
#if DATABIG_ENDIAN
    temp16 = Endia_Swap16(temp16);
#endif
    memcpy(temporary_buffer+sent_len+2, &temp16, 2);        sent_len += 2;

    // sent_len += 8;

    ftemp = recode_query[ch].run_time;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(temporary_buffer+sent_len+2, &ftemp, 4);         sent_len += 4;

    ftemp = recode_query[ch].battery_voltage;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(temporary_buffer+sent_len+2, &ftemp, 4);         sent_len += 4;

    ftemp = recode_query[ch].output_current;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(temporary_buffer+sent_len+2, &ftemp, 4);         sent_len += 4;

    ftemp = recode_query[ch].output_voltage;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(temporary_buffer+sent_len+2, &ftemp, 4);         sent_len += 4;

    ftemp = recode_query[ch].battery_capacitance;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(temporary_buffer+sent_len+2, &ftemp, 4);         sent_len += 4;

    ftemp = recode_query[ch].battery_energy;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(temporary_buffer+sent_len+2, &ftemp, 4);         sent_len += 4;

    ftemp = recode_query[ch].battery_temperature;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(temporary_buffer+sent_len+2, &ftemp, 4);         sent_len += 4;

    ftemp = recode_query[ch].module_temperature;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(temporary_buffer+sent_len+2, &ftemp, 4);         sent_len += 4;

    ftemp = recode_query[ch].crosscurrent_ratio;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(temporary_buffer+sent_len+2, &ftemp, 4);         sent_len += 4;

    ftemp = recode_query[ch].contact_impedance;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(temporary_buffer+sent_len+2, &ftemp, 4);         sent_len += 4;

    ftemp = recode_query[ch].loop_impedance;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(temporary_buffer+sent_len+2, &ftemp, 4);         sent_len += 4;

    // sent_len += 44;

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;
}


/***************************************************************************************************
**  系统类 --> 系统查询解析函数
**
**
***************************************************************************************************/
static void System_Query_Analysis(uint8_t ch, uint8_t sub_id, u8* temporary_buffer)
{
    uint16_t sent_len = 5;
    //uint16_t CRC_16_code = 0x0000;

    if(sub_id == system_information) {          //系统基本信息
        //软件版本号
        temporary_buffer[7] = SOFTWARE_VERSION;
        temporary_buffer[8] = SOFTWARE_SUBVERSION;
        sent_len += 2;
        //模块柜号
    } else if(sub_id == system_fault) {                 //系统故障码查询
        for(int i = 0; i < CHANNELS; i++) {
            if(ch & (0x01<<i)) {
                // QUERY_PROTECTION_FLAG |= (0x01<<i);

                if(STEP_PROT_FLAG[i] != 0x00) {
                    Step_Protection_Report(0x01<<i);
                }
                if(GLOBAL_PROT_FLAG[i] != 0x00) {
                    Global_Protection_Report(0x01<<i);
                }
            }
        }
        Analysis_Flag = 0x02;
        return;
    } else if(sub_id == 0x03) {

    }

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;
}


/***************************************************************************************************
**  系统类 --> 系统设置解析函数
**
**
***************************************************************************************************/
static void System_Setting_Analysis(uint8_t ch, u8* data, uint16_t data_len, u8* temporary_buffer)
{
    //uint32_t temporary_data;
    uint16_t len = 0;
    uint16_t sent_len = 4;
    float ftemp;
    int64_t temp64;
    //uint16_t CRC_16_code = 0x0000;

    temporary_buffer[sent_len+2] = data[0];             //填充子命令
    len += 1;
    sent_len += 1;

    if(data[0] == system_alarm_cleared) {           //清除系统告警
        if(ch) {
            for(int i = 0; i < CHANNELS; i++) {
                if(ch & (0x01<<i)) {
                    STEP_PROT_FLAG[i] = 0;
                    GLOBAL_PROT_FLAG[i] = 0;
                    memset(Global_Protection_Flag[i], 0, global_protection_trigger_end);
                    memset(Step_Protection_Flag[i], 0, step_protection_trigger_end);

                    running_step[i].status = step_standby_status;

                    led_run_setting((0x01<<i), led_normal, &(led_control));
                }
            }

    #if CHANNELS_1
        if((STEP_PROT_FLAG[0] == 0) && (GLOBAL_PROT_FLAG[0] == 0)) {
            led_run_setting(0, led_normal, &(led_control));
        }
    #endif
    #if CHANNELS_2
        if( ((STEP_PROT_FLAG[0] == 0) && (GLOBAL_PROT_FLAG[0] == 0)) && \
            ((STEP_PROT_FLAG[1] == 0) && (GLOBAL_PROT_FLAG[1] == 0))) {
            led_run_setting(0, led_normal, &(led_control));
        }
    #endif
    #if CHANNELS_4
        if( ((STEP_PROT_FLAG[0] == 0) && (GLOBAL_PROT_FLAG[0] == 0)) && \
            ((STEP_PROT_FLAG[1] == 0) && (GLOBAL_PROT_FLAG[1] == 0)) && \
            ((STEP_PROT_FLAG[2] == 0) && (GLOBAL_PROT_FLAG[2] == 0)) && \
            ((STEP_PROT_FLAG[3] == 0) && (GLOBAL_PROT_FLAG[3] == 0))) {
            led_run_setting(0, led_normal, &(led_control));
        }
    #endif
    #if CHANNELS_8
        if( ((STEP_PROT_FLAG[0] == 0) && (GLOBAL_PROT_FLAG[0] == 0)) && \
            ((STEP_PROT_FLAG[1] == 0) && (GLOBAL_PROT_FLAG[1] == 0)) && \
            ((STEP_PROT_FLAG[2] == 0) && (GLOBAL_PROT_FLAG[2] == 0)) && \
            ((STEP_PROT_FLAG[3] == 0) && (GLOBAL_PROT_FLAG[3] == 0)) && \
            ((STEP_PROT_FLAG[4] == 0) && (GLOBAL_PROT_FLAG[4] == 0)) && \
            ((STEP_PROT_FLAG[5] == 0) && (GLOBAL_PROT_FLAG[5] == 0)) && \
            ((STEP_PROT_FLAG[6] == 0) && (GLOBAL_PROT_FLAG[6] == 0)) && \
            ((STEP_PROT_FLAG[7] == 0) && (GLOBAL_PROT_FLAG[7] == 0))) {
            led_run_setting(0, led_normal, &(led_control));
        }
    #endif


            temporary_buffer[sent_len+2] = 0x01;
        } else {
            temporary_buffer[sent_len+2] = 0x00;
        }

        sent_len += 1;
    } else if(data[0] == system_time_setting) {     //系统时间设定
        memcpy(&temp64, data+1, 8);
    #if DATABIG_ENDIAN
        temp64 = Endia_Swap64(temp64);
    #endif
        Time_Stamp = temp64;

        temporary_buffer[sent_len+2] = 0x01;
        sent_len += 1;
    } else if(data[0] == heartbeat_package) {       //系统心跳包
        heartbeat_flag = 0x01;

        temporary_buffer[sent_len+2] = 0x01;
        sent_len += 1;
    } else if(data[0] == system_mode_setting) {     //设定运行模式
    #if CHANNELS_1
        if((data[1] == 0x01) && ((running_step[0].status == step_stop_status) || (running_step[0].status == step_complete_status) || (running_step[0].status == step_standby_status) )) {
            MODE_PORT1_MASTER;MODE_PORT2_SLAVE;MODE_PORT3_SLAVE;MODE_PORT4_SLAVE;
            MODE_PORT5_SLAVE;MODE_PORT6_SLAVE;MODE_PORT7_SLAVE;MODE_PORT8_SLAVE;

            SYS_MODE = SYS_NORMAL_MODE;
            temporary_buffer[sent_len+2] = 0x01;
        } else if((data[1] == 0x02) && ((running_step[0].status == step_stop_status) || (running_step[0].status == step_complete_status) || (running_step[0].status == step_standby_status) )) {
            MODE_PORT1_MASTER;MODE_PORT2_MASTER;MODE_PORT3_MASTER;MODE_PORT4_MASTER;
            MODE_PORT5_MASTER;MODE_PORT6_MASTER;MODE_PORT7_MASTER;MODE_PORT8_MASTER;

            SYS_MODE = SYS_CALIBRATION_MODE;
            temporary_buffer[sent_len+2] = 0x01;
        } else {
            MODE_PORT1_MASTER;MODE_PORT2_SLAVE;MODE_PORT3_SLAVE;MODE_PORT4_SLAVE;
            MODE_PORT5_SLAVE;MODE_PORT6_SLAVE;MODE_PORT7_SLAVE;MODE_PORT8_SLAVE;

            SYS_MODE = SYS_NORMAL_MODE;
            temporary_buffer[sent_len+2] = 0x00;
        }
        sent_len += 1;
    #endif
    #if CHANNELS_2
        if((data[1] == 0x01) && (   ((running_step[0].status == step_stop_status) || (running_step[0].status == step_complete_status) || (running_step[0].status == step_standby_status)) &&
                                    ((running_step[1].status == step_stop_status) || (running_step[1].status == step_complete_status) || (running_step[1].status == step_standby_status)) )) {
            MODE_PORT1_MASTER;MODE_PORT2_SLAVE;MODE_PORT3_SLAVE;MODE_PORT4_SLAVE;
            MODE_PORT5_MASTER;MODE_PORT6_SLAVE;MODE_PORT7_SLAVE;MODE_PORT8_SLAVE;

            SYS_MODE = SYS_NORMAL_MODE;
            temporary_buffer[sent_len+2] = 0x01;
        } else if((data[1] == 0x02) && (((running_step[0].status == step_stop_status) || (running_step[0].status == step_complete_status) || (running_step[0].status == step_standby_status)) &&
                                        ((running_step[1].status == step_stop_status) || (running_step[1].status == step_complete_status) || (running_step[1].status == step_standby_status)) )) {
            MODE_PORT1_MASTER;MODE_PORT2_MASTER;MODE_PORT3_MASTER;MODE_PORT4_MASTER;
            MODE_PORT5_MASTER;MODE_PORT6_MASTER;MODE_PORT7_MASTER;MODE_PORT8_MASTER;

            SYS_MODE = SYS_CALIBRATION_MODE;
            temporary_buffer[sent_len+2] = 0x01;
        } else {
            MODE_PORT1_MASTER;MODE_PORT2_SLAVE;MODE_PORT3_SLAVE;MODE_PORT4_SLAVE;
            MODE_PORT5_MASTER;MODE_PORT6_SLAVE;MODE_PORT7_SLAVE;MODE_PORT8_SLAVE;

            SYS_MODE = SYS_NORMAL_MODE;
            temporary_buffer[sent_len+2] = 0x00;
        }
        sent_len += 1;
    #endif
    #if CHANNELS_4
        if((data[1] == 0x01) && (   ((running_step[0].status == step_stop_status) || (running_step[0].status == step_complete_status) || (running_step[0].status == step_standby_status)) &&
                                    ((running_step[1].status == step_stop_status) || (running_step[1].status == step_complete_status) || (running_step[1].status == step_standby_status)) &&
                                    ((running_step[2].status == step_stop_status) || (running_step[2].status == step_complete_status) || (running_step[2].status == step_standby_status)) &&
                                    ((running_step[3].status == step_stop_status) || (running_step[3].status == step_complete_status) || (running_step[3].status == step_standby_status)) )) {
            MODE_PORT1_MASTER;MODE_PORT2_SLAVE;
            MODE_PORT3_MASTER;MODE_PORT4_SLAVE;
            MODE_PORT5_MASTER;MODE_PORT6_SLAVE;
            MODE_PORT7_MASTER;MODE_PORT8_SLAVE;

            SYS_MODE = SYS_NORMAL_MODE;
            temporary_buffer[sent_len+2] = 0x01;
        } else if((data[1] == 0x02) && (((running_step[0].status == step_stop_status) || (running_step[0].status == step_complete_status) || (running_step[0].status == step_standby_status)) &&
                                        ((running_step[1].status == step_stop_status) || (running_step[1].status == step_complete_status) || (running_step[1].status == step_standby_status)) &&
                                        ((running_step[2].status == step_stop_status) || (running_step[2].status == step_complete_status) || (running_step[2].status == step_standby_status)) &&
                                        ((running_step[3].status == step_stop_status) || (running_step[3].status == step_complete_status) || (running_step[3].status == step_standby_status)) )) {
            MODE_PORT1_MASTER;MODE_PORT2_MASTER;MODE_PORT3_MASTER;MODE_PORT4_MASTER;
            MODE_PORT5_MASTER;MODE_PORT6_MASTER;MODE_PORT7_MASTER;MODE_PORT8_MASTER;

            SYS_MODE = SYS_CALIBRATION_MODE;
            temporary_buffer[sent_len+2] = 0x01;
        } else {
            MODE_PORT1_MASTER;MODE_PORT2_SLAVE;
            MODE_PORT3_MASTER;MODE_PORT4_SLAVE;
            MODE_PORT5_MASTER;MODE_PORT6_SLAVE;
            MODE_PORT7_MASTER;MODE_PORT8_SLAVE;

            SYS_MODE = SYS_NORMAL_MODE;
            temporary_buffer[sent_len+2] = 0x00;
        }
        sent_len += 1;
    #endif
    #if CHANNELS_8
        if((data[1] == 0x01) && (   ((running_step[0].status == step_stop_status) || (running_step[0].status == step_complete_status) || (running_step[0].status == step_standby_status)) &&
                                    ((running_step[1].status == step_stop_status) || (running_step[1].status == step_complete_status) || (running_step[1].status == step_standby_status)) &&
                                    ((running_step[2].status == step_stop_status) || (running_step[2].status == step_complete_status) || (running_step[2].status == step_standby_status)) &&
                                    ((running_step[3].status == step_stop_status) || (running_step[3].status == step_complete_status) || (running_step[3].status == step_standby_status)) &&
                                    ((running_step[4].status == step_stop_status) || (running_step[4].status == step_complete_status) || (running_step[4].status == step_standby_status)) &&
                                    ((running_step[5].status == step_stop_status) || (running_step[5].status == step_complete_status) || (running_step[5].status == step_standby_status)) &&
                                    ((running_step[6].status == step_stop_status) || (running_step[6].status == step_complete_status) || (running_step[6].status == step_standby_status)) &&
                                    ((running_step[7].status == step_stop_status) || (running_step[7].status == step_complete_status) || (running_step[7].status == step_standby_status)) )) {
            MODE_PORT1_MASTER;MODE_PORT2_MASTER;MODE_PORT3_MASTER;MODE_PORT4_MASTER;
            MODE_PORT5_MASTER;MODE_PORT6_MASTER;MODE_PORT7_MASTER;MODE_PORT8_MASTER;

            SYS_MODE = SYS_NORMAL_MODE;
            temporary_buffer[sent_len+2] = 0x01;
        } else if((data[1] == 0x02) && (((running_step[0].status == step_stop_status) || (running_step[0].status == step_complete_status) || (running_step[0].status == step_standby_status)) &&
                                        ((running_step[1].status == step_stop_status) || (running_step[1].status == step_complete_status) || (running_step[1].status == step_standby_status)) &&
                                        ((running_step[2].status == step_stop_status) || (running_step[2].status == step_complete_status) || (running_step[2].status == step_standby_status)) &&
                                        ((running_step[3].status == step_stop_status) || (running_step[3].status == step_complete_status) || (running_step[3].status == step_standby_status)) &&
                                        ((running_step[4].status == step_stop_status) || (running_step[4].status == step_complete_status) || (running_step[4].status == step_standby_status)) &&
                                        ((running_step[5].status == step_stop_status) || (running_step[5].status == step_complete_status) || (running_step[5].status == step_standby_status)) &&
                                        ((running_step[6].status == step_stop_status) || (running_step[6].status == step_complete_status) || (running_step[6].status == step_standby_status)) &&
                                        ((running_step[7].status == step_stop_status) || (running_step[7].status == step_complete_status) || (running_step[7].status == step_standby_status)) )) {
            MODE_PORT1_MASTER;MODE_PORT2_MASTER;MODE_PORT3_MASTER;MODE_PORT4_MASTER;
            MODE_PORT5_MASTER;MODE_PORT6_MASTER;MODE_PORT7_MASTER;MODE_PORT8_MASTER;

            SYS_MODE = SYS_CALIBRATION_MODE;
            temporary_buffer[sent_len+2] = 0x01;
        } else {
            MODE_PORT1_MASTER;MODE_PORT2_MASTER;MODE_PORT3_MASTER;MODE_PORT4_MASTER;
            MODE_PORT5_MASTER;MODE_PORT6_MASTER;MODE_PORT7_MASTER;MODE_PORT8_MASTER;

            SYS_MODE = SYS_NORMAL_MODE;
            temporary_buffer[sent_len+2] = 0x00;
        }
        sent_len += 1;
    #endif

    } else if(data[0] == battery_temperature_setting) {     //电池温度
        if(ch) {
            for(int i = 0; i < CHANNELS; i++) {
                if(ch & (0x01<<i)) {
                    memcpy(&ftemp, data+len, 4);
                #if DATABIG_ENDIAN
                    ftemp = Floap_Swap(ftemp);
                #endif
                    record_data.channel_record_data[i].battery_temperature = ftemp;
                }

                len += 4;
            }

            temporary_buffer[sent_len+2] = 0x01;
        } else {
            temporary_buffer[sent_len+2] = 0x00;
        }

        sent_len += 1;
    }

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;
}


/***************************************************************************************************
**  工步类 --> 工步设置解析函数
**
**
***************************************************************************************************/
//工步存储
// static void Step_SPI_Write(u8* data, uint16_t data_len, uint32_t offset_address)
// {
//     uint8_t i;
//     uint8_t number = data_len / 5;
//     uint32_t temporary_data;
//     uint16_t len = 3;
//     uint8_t sub_id;
//     uint16_t offset_len = 3;
//     float temp;
//     uint32_t no_value = 0x00000000;

//     for(i = 0; i< 3; i++) {
//         sub_id = data[len];
//         memcpy(&temporary_data, data+len+1, 4);
//         //大小端转换
//         /*  code  */

//         if(sub_id != 0xff) {
//             temp = (float)temporary_data / 10000;
//             SPI_Flash_Write(&temp, offset_address+offset_len, 4);
//         } else {
//             SPI_Flash_Write(&no_value, offset_address+offset_len, 4);
//         }

//         len += 5;
//         offset_len += 4;
//     }
//     for(i = 0; i< 4; i++) {
//         sub_id = data[len];

//         if(sub_id != 0xff) {
//             memcpy(&temporary_data, data+len+1, 4);
//             //大小端转换
//             /*  code  */

//             if(sub_id == step_abort_time) {
//                 temp = (float)temporary_data / 1000;
//             } else {
//                 temp = (float)temporary_data / 10000;
//             }

//             SPI_Flash_Write(&temp, offset_address+offset_len, 4);
//         } else {
//             SPI_Flash_Write(&no_value, offset_address+offset_len, 4);
//         }

//         len += 5;
//         offset_len += 4;
//     }
//     for(i = 0; i< 3; i++) {
//         sub_id = data[len];
//         if(sub_id != 0xff) {
//             memcpy(&temporary_data, data+len+1, 4);
//             //大小端转换
//             /*  code  */

//             if(sub_id == step_record_time) {
//                 temp = (float)temporary_data / 1000;
//             } else {
//                 temp = (float)temporary_data / 10000;
//             }

//             SPI_Flash_Write(&temp, offset_address+offset_len, 4);
//         } else {
//             SPI_Flash_Write(&no_value, offset_address+offset_len, 4);
//         }

//         len += 5;
//         offset_len += 4;
//     }
//     for(i = 0; i < (number-3-4-3); i++) {
//         sub_id = data[len];
//         if(sub_id != 0xff) {
//             memcpy(&temporary_data, data+len+1, 4);
//             //大小端转换
//             /*  code  */

//             if(sub_id == step_OverTemperature_alarm) {
//                 temp = (float)temporary_data / 10;
//                 SPI_Flash_Write(&temp, offset_address+offset_len, 4);
//             } else if(sub_id == step_current_positive_jump_number || \
//                         sub_id == step_current_negative_jump_number || \
//                         sub_id == step_voltage_positive_jump_number || \
//                         sub_id == step_voltage_negative_jump_number) {
//                 SPI_Flash_Write(&temporary_data, offset_address+offset_len, 4);
//             } else {
//                 temp = (float)temporary_data / 10000;
//                 SPI_Flash_Write(&temp, offset_address+offset_len, 4);
//             }
//         } else {
//             SPI_Flash_Write(&no_value, offset_address+offset_len, 4);
//         }

//         len += 5;
//         offset_len += 4;
//     }
// }

static uint16_t step_sum = 0;

static void WorkStep_Setting_Analysis(uint8_t ch, u8* data, uint16_t data_len, u8* temporary_buffer)
{
    static uint8_t ok_flag = 0;

    uint8_t i = 0;
    uint16_t len = 0;
    uint16_t sent_len = 4;
    uint8_t number = 0;
    uint32_t offset_address = 0;
    uint16_t offset_len = 0;
    uint16_t step_num = 0;
    uint16_t temp16;
    uint32_t temp32;
    float ftemp;
    uint8_t sub_id;
    //uint16_t CRC_16_code = 0x0000;
    uint32_t no_value = 0xffffffff;
    uint8_t temporary_step_datas[257] = {0x00};


    memcpy(&temp16, data, 2);
#if DATABIG_ENDIAN
    temp16 = Endia_Swap16(temp16);
#endif
    step_num = temp16;

    if((step_num > 0) && (step_num <= STEP_NUMBER_MAX)) {
        if((data[2] > 0) && (data[2] < step_type_parament_end)) {
            memcpy(temporary_step_datas, &step_num, 2);                         //工步号
            memcpy(temporary_step_datas+2, &data[2], 1);                        //工步类型
            len += 3;
            offset_len += 3;

            if((data[2] != step_type_invalid_symbols_1) && (data[2] != loop_begin) && (data[2] != loop_end) && (data[2] != end_step)) {
                number = (data_len-3) / 5;

                if(number >= 10) {
                    for(i = 0; i< 3; i++) {                 //工步运行设置
                        sub_id = data[len];

                        if((sub_id >= step_time) && (sub_id < step_work_abort_parament_end)) {
                            memcpy(&ftemp, data+len+1, 4);
                        #if DATABIG_ENDIAN
                            ftemp = Float_Swap(ftemp);
                        #endif
                            temporary_step_datas[offset_len] = sub_id;
                            memcpy(temporary_step_datas+offset_len+1, &ftemp, 4);
                        } else {
                            temporary_step_datas[offset_len] = 0xff;
                            memcpy(temporary_step_datas+offset_len+1, &no_value, 4);
                        }

                        len += 5;
                        offset_len += 5;
                    }
                    for(i = 0; i< 4; i++) {                 //工步截止设置
                        sub_id = data[len];

                        if((sub_id >= step_time) && (sub_id < step_work_abort_parament_end)) {
                            memcpy(&ftemp, data+len+1, 4);
                        #if DATABIG_ENDIAN
                            ftemp = Float_Swap(ftemp);
                        #endif
                            temporary_step_datas[offset_len] = sub_id;
                            memcpy(temporary_step_datas+offset_len+1, &ftemp, 4);
                        } else {
                            temporary_step_datas[offset_len] = 0xff;
                            memcpy(temporary_step_datas+offset_len+1, &no_value, 4);
                        }

                        len += 5;
                        offset_len += 5;
                    }
                    for(i = 0; i< 3; i++) {                 //记录条件设置
                        sub_id = data[len];

                        if((sub_id >= step_record_time) && (sub_id < step_record_parament_end)) {
                            memcpy(&ftemp, data+len+1, 4);
                        #if DATABIG_ENDIAN
                            ftemp = Float_Swap(ftemp);
                        #endif
                            temporary_step_datas[offset_len] = sub_id;
                            memcpy(temporary_step_datas+offset_len+1, &ftemp, 4);
                        } else {
                            temporary_step_datas[offset_len] = 0xff;
                            memcpy(temporary_step_datas+offset_len+1, &no_value, 4);
                        }

                        len += 5;
                        offset_len += 5;
                    }
                    for(i = 0; i < (number-3-4-3); i++) {   //单工步保护设置
                        sub_id = data[len];
                        if(sub_id != 0xff) {
                            if(sub_id == step_current_positive_jump_number || \
                                        sub_id == step_current_negative_jump_number || \
                                        sub_id == step_voltage_positive_jump_number || \
                                        sub_id == step_voltage_negative_jump_number) {
                                memcpy(&temp32, data+len+1, 4);
                            #if DATABIG_ENDIAN
                                temp32 = Endia_Swap32(temp32);
                            #endif
                                temporary_step_datas[offset_len] = sub_id;
                                memcpy(temporary_step_datas+offset_len+1, &temp32, 4);
                            } else {
                                memcpy(&ftemp, data+len+1, 4);
                            #if DATABIG_ENDIAN
                                ftemp = Float_Swap(ftemp);
                            #endif
                                temporary_step_datas[offset_len] = sub_id;
                                memcpy(temporary_step_datas+offset_len+1, &ftemp, 4);
                            }
                        } else {
                            temporary_step_datas[offset_len] = 0xff;
                            memcpy(temporary_step_datas+offset_len+1, &no_value, 4);
                        }

                        len += 5;
                        offset_len += 5;
                    }

                    ok_flag = 1;
                    step_sum += 1;
                } else {
                    ok_flag = 0;
                }

            }
            // else if(data[2] == loop_begin) {
            //     //循环序号
            //     memcpy(&temp16, data+len, 2);
            // #if DATABIG_ENDIAN
            //     temp16 = Endia_Swap16(temp16);
            // #endif
            //     memcpy(temporary_step_datas+offset_len, &temp16, 2);
            //     len += 2;
            //     offset_len += 2;

            //     //循环次数
            //     memcpy(&temp16, data+len, 2);
            // #if DATABIG_ENDIAN
            //     temp16 = Endia_Swap16(temp16);
            // #endif
            //     memcpy(temporary_step_datas+offset_len, &temp16, 2);
            //     len += 2;
            //     offset_len += 2;

            //     ok_flag = 1;
            //     step_sum += 1;
            // }
            else if(data[2] == loop_end) {

                for(int i = 0; i < 3; i++) {
                    memcpy(&temp16, data+len, 2);
                #if DATABIG_ENDIAN
                    temp16 = Endia_Swap16(temp16);
                #endif
                    memcpy(temporary_step_datas+offset_len, &temp16, 2);
                    len += 2;
                    offset_len += 2;
                }

            //     //循环序号
            //     memcpy(&temp16, data+len, 2);
            // #if DATABIG_ENDIAN
            //     temp16 = Endia_Swap16(temp16);
            // #endif
            //     memcpy(temporary_step_datas+offset_len, &temp16, 2);
            //     len += 2;
            //     offset_len += 2;

            //     //循环次数
            //     memcpy(&temp16, data+len, 2);
            // #if DATABIG_ENDIAN
            //     temp16 = Endia_Swap16(temp16);
            // #endif
            //     memcpy(temporary_step_datas+offset_len, &temp16, 2);
            //     len += 2;
            //     offset_len += 2;

            //     //循环开始工步号
            //     memcpy(&temp16, data+len, 2);
            // #if DATABIG_ENDIAN
            //     temp16 = Endia_Swap16(temp16);
            // #endif
            //     memcpy(temporary_step_datas+offset_len, &temp16, 2);
            //     len += 2;
            //     offset_len += 2;

                ok_flag = 1;
                step_sum += 1;
            }
            else if(data[2] == end_step) {        //结束
                ok_flag = 1;
                step_sum += 1;
            } else {
                ok_flag = 0;
            }


            if(ok_flag == 1) {
            #if CHANNELS_1
                if(ch & CH_1) {
                    offset_address = (step_num-1) * 256;
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
            #endif
            #if CHANNELS_2
                if(ch & CH_1) {
                    offset_address = (step_num-1) * 256;
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
                if(ch & CH_2) {
                    offset_address = 16 * 4096 + (step_num-1) * 256;
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
            #endif
            #if CHANNELS_4
                if(ch & CH_1) {
                    offset_address = (step_num-1) * 256;
                    // SPI_Flash_Write(temporary_step_datas, offset_address, offset_len);
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
                if(ch & CH_2) {
                    offset_address = 16 * 4096 + (step_num-1) * 256;
                    // SPI_Flash_Write(temporary_step_datas, offset_address, offset_len);
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
                if(ch & CH_3) {
                    offset_address = 2 * 16 * 4096 + (step_num-1) * 256;
                    // SPI_Flash_Write(temporary_step_datas, offset_address, offset_len);
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
                if(ch & CH_4) {
                    offset_address = 3 * 16 * 4096 + (step_num-1) * 256;
                    // SPI_Flash_Write(temporary_step_datas, offset_address, offset_len);
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
            #endif
            #if CHANNELS_8
                if(ch & CH_1) {
                    offset_address = (step_num-1) * 256;
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
                if(ch & CH_2) {
                    offset_address = 16 * 4096 + (step_num-1) * 256;
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
                if(ch & CH_3) {
                    offset_address = 2 * 16 * 4096 + (step_num-1) * 256;
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
                if(ch & CH_4) {
                    offset_address = 3 * 16 * 4096 + (step_num-1) * 256;
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
                if(ch & CH_5) {
                    offset_address = 4 * 16 * 4096 + (step_num-1) * 256;
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
                if(ch & CH_6) {
                    offset_address = 5 * 16 * 4096 + (step_num-1) * 256;
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
                if(ch & CH_7) {
                    offset_address = 6 * 16 * 4096 + (step_num-1) * 256;
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
                if(ch & CH_8) {
                    offset_address = 7 * 16 * 4096 + (step_num-1) * 256;
                    SPI_Flash_Write(temporary_step_datas, offset_address, 256);
                }
            #endif
            }
        }

    } else {
        Analysis_Flag = 0x00;
        return;
    }

#if DATABIG_ENDIAN
    step_num = Endia_Swap16(step_num);
#endif
    memcpy(temporary_buffer+sent_len+2, &step_num, 2);

    if(ok_flag == 1) {
        temp16 = step_sum;
    #if DATABIG_ENDIAN
        temp16 = Endia_Swap16(temp16);
    #endif
        memcpy(temporary_buffer+sent_len+4, &temp16, 2);
        temporary_buffer[sent_len+6] = 0x01;

    } else {
        temporary_buffer[sent_len+4] = 0x00;
        temporary_buffer[sent_len+5] = 0x00;
        temporary_buffer[sent_len+6] = 0x00;
    }

    sent_len += 5;

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;

}


/***************************************************************************************************
**  工步类 --> 工步运行状态设置解析函数
**
**
***************************************************************************************************/
uint8_t STEP_STATUS_SETTING_FLAG = 0X00;

static void WorkStep_Status_Setting_Analysis(uint8_t ch, u8* data, u8* temporary_buffer)
{
    // uint16_t len = 0;
    uint16_t sent_len = 4;
    uint16_t temp16 = 0;
    uint16_t temporary_step_number;
    uint8_t tempporary_run_status;

    STEP_STATUS_SETTING_FLAG = 0x00;

    tempporary_run_status = data[0];
    if((tempporary_run_status >= step_stop_setting) && (tempporary_run_status < step_run_setting_end)) {
        if((tempporary_run_status == step_start_setting) || (tempporary_run_status == step_jump_setting)) {
            memcpy(&temporary_step_number, data+1, 2);
        #if DATABIG_ENDIAN
            temporary_step_number = Endia_Swap16(temporary_step_number);
        #endif
        } else {
            temporary_step_number = 0;
        }

        for(int i = 0; i < CHANNELS; i++) {
            // if(ch & (0x01<<i)) {
            if((ch & (0x01<<i)) && (STEP_PROT_FLAG[i] == 0) && (GLOBAL_PROT_FLAG[i] == 0)) {
                STEP_STATUS_SETTING_FLAG |= (0x01<<i);
                step_run[i].run_status = tempporary_run_status;
                step_run[i].run_number = temporary_step_number;

                temporary_buffer[sent_len+6] = 0x01;
            }
            // else {
            //     temporary_buffer[sent_len+6] = 0x00;
            // }
        }


    #if DATABIG_ENDIAN
        temporary_step_number = Endia_Swap16(temporary_step_number);
    #endif
        memcpy(temporary_buffer+sent_len+2, &temporary_step_number, 2);

        temp16 = step_sum;
    #if DATABIG_ENDIAN
        temp16 = Endia_Swap16(temp16);
    #endif
        memcpy(temporary_buffer+sent_len+4, &temp16, 2);
        // temporary_buffer[sent_len+6] = 0x01;
        sent_len += 5;
    } else {
        temporary_step_number = 0;

        memcpy(temporary_buffer+sent_len+2, &temporary_step_number, 2);
        memcpy(temporary_buffer+sent_len+4, &temporary_step_number, 2);
        temporary_buffer[sent_len+6] = 0x00;
        sent_len += 5;
    }

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;

}

/***************************************************************************************************
**  工步类 --> 工步参数查询解析函数
**
**
***************************************************************************************************/
static void WorkStep_Query_Analysis(uint8_t ch, uint16_t number, u8* temporary_buffer)
{
    uint8_t data_ch = 0;
    uint32_t data_address;
    uint16_t sent_len = 4;
    uint16_t read_len = 0;
    uint8_t temp8;
    uint16_t temp16;
    uint32_t temp32;
    float ftemp;
    uint8_t temp_data[257] = {0x00};
    uint8_t i = 0;

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {
        Analysis_Flag = 0x00;
        return;
    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {
        Analysis_Flag = 0x00;
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
        Analysis_Flag = 0x00;
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
        Analysis_Flag = 0x00;
        return;
    }
#endif

    if((number > 0) && (number <= STEP_NUMBER_MAX)) {
        data_address = data_ch * 16 * 4096 + (number-1) * 256;
        SPI_Flash_Read(temp_data, data_address, 256);
        memcpy(&temp16, temp_data, 2);
    } else {
        temp16 = 0x00;
    }

    if(temp16 == number) {
    #if DATABIG_ENDIAN
        temp16 = Endia_Swap16(temp16);
    #endif
        memcpy(temporary_buffer+sent_len+2, &temp16, 2);            sent_len += 2;  read_len += 2;
        temporary_buffer[sent_len+2] = temp_data[2];                sent_len += 1;  read_len += 1;

        if(temp_data[2] == end_step) {

        } else if(temp_data[2] == loop_begin) {
            memcpy(&temp16, temp_data+read_len, 2);
        #if DATABIG_ENDIAN
            temp16 = Endia_Swap16(temp16);
        #endif
            memcpy(temporary_buffer+sent_len+2, &temp16, 2);
            sent_len += 2;
            read_len += 2;

            memcpy(&temp16, temp_data+read_len, 2);
        #if DATABIG_ENDIAN
            temp16 = Endia_Swap16(temp16);
        #endif
            memcpy(temporary_buffer+sent_len+2, &temp16, 2);
            sent_len += 2;
            read_len += 2;
        } else if(temp_data[2] == loop_end) {
            memcpy(&temp16, temp_data+read_len, 2);
        #if DATABIG_ENDIAN
            temp16 = Endia_Swap16(temp16);
        #endif
            memcpy(temporary_buffer+sent_len+2, &temp16, 2);
            sent_len += 2;
            read_len += 2;
        } else {
            for(int i = 0; i < (3+4+3+step_protection_parament_end-1); i++) {
                memcpy(&ftemp, temp_data+read_len+1, 4);
            #if DATABIG_ENDIAN
                ftemp = Float_Swap(ftemp);
            #endif
                memcpy(&temp32, &ftemp, 4);
                if(temp32 == 0xffffffff) {
                    temporary_buffer[sent_len+2] = 0xff;
                    ftemp = 0;
                    memcpy(temporary_buffer+sent_len+3, &ftemp, 4);
                } else {
                    temporary_buffer[sent_len+2] = temp_data[read_len];
                    memcpy(temporary_buffer+sent_len+3, &ftemp, 4);
                }
                sent_len += 5;  read_len += 5;
            }

            // for(i = 1; i < step_work_parament_end; i++) {
            //     memcpy(&ftemp, temp_data+read_len, 4);
            // #if DATABIG_ENDIAN
            //     ftemp = Float_Swap(ftemp);
            // #endif
            //     memcpy(&temp32, &ftemp, 4);
            //     if(temp32 == 0xffffffff) {
            //         temporary_buffer[sent_len+2] = 0xff;
            //         ftemp = 0;
            //         memcpy(temporary_buffer+sent_len+3, &ftemp, 4);
            //     } else {
            //         temporary_buffer[sent_len+2] = i;
            //         memcpy(temporary_buffer+sent_len+3, &ftemp, 4);
            //     }
            //     sent_len += 5;  read_len += 4;
            // }
            // for(i = 1; i < step_abort_parament_end; i++) {
            //     memcpy(&ftemp, temp_data+read_len, 4);
            // #if DATABIG_ENDIAN
            //     ftemp = Float_Swap(ftemp);
            // #endif
            //     memcpy(&temp32, &ftemp, 4);
            //     if(temp32 == 0xffffffff) {
            //         temporary_buffer[sent_len+2] = 0xff;
            //         ftemp = 0;
            //         memcpy(temporary_buffer+sent_len+3, &ftemp, 4);
            //     } else {
            //         temporary_buffer[sent_len+2] = i;
            //         memcpy(temporary_buffer+sent_len+3, &ftemp, 4);
            //     }
            //     sent_len += 5;  read_len += 4;
            // }
            // for(i = 1; i < step_record_parament_end; i++) {
            //     memcpy(&ftemp, temp_data+read_len, 4);
            // #if DATABIG_ENDIAN
            //     ftemp = Float_Swap(ftemp);
            // #endif
            //     memcpy(&temp32, &ftemp, 4);
            //     if(temp32 == 0xffffffff) {
            //         temporary_buffer[sent_len+2] = 0xff;
            //         ftemp = 0;
            //         memcpy(temporary_buffer+sent_len+3, &ftemp, 4);
            //     } else {
            //         temporary_buffer[sent_len+2] = i;
            //         memcpy(temporary_buffer+sent_len+3, &ftemp, 4);
            //     }
            //     sent_len += 5;  read_len += 4;
            // }
            // for(i = 1; i < step_protection_parament_end; i++) {
            //     memcpy(&ftemp, temp_data+read_len, 4);
            // #if DATABIG_ENDIAN
            //     ftemp = Float_Swap(ftemp);
            // #endif
            //     memcpy(&temp32, &ftemp, 4);
            //     if(temp32 == 0xffffffff) {
            //         temporary_buffer[sent_len+2] = 0xff;
            //         ftemp = 0;
            //         memcpy(temporary_buffer+sent_len+3, &ftemp, 4);
            //     } else {
            //         temporary_buffer[sent_len+2] = i;
            //         memcpy(temporary_buffer+sent_len+3, &ftemp, 4);
            //     }
            //     sent_len += 5;  read_len += 4;
            // }

        }

        sent_len += 2;
    #if DATABIG_ENDIAN
        sent_len = Endia_Swap16(sent_len);
    #endif
        memcpy(temporary_buffer+2, &sent_len, 2);

        Analysis_Flag = 0x01;
    } else {
    //     temporary_buffer[sent_len+2] = 0x00;
    //     temporary_buffer[sent_len+3] = 0x00;
    //     sent_len += 2;

    //     sent_len += 2;
    // #if DATABIG_ENDIAN
    //     sent_len = Endia_Swap16(sent_len);
    // #endif
    //     memcpy(temporary_buffer+2, &sent_len, 2);

        Analysis_Flag = 0x00;
        return;
    }

}


/***************************************************************************************************
**  校准类 --> 校准参数设置解析函数
**
**
***************************************************************************************************/
static void Calibration_Parameter_Setting_Analysis(uint8_t ch, u8* data, uint16_t data_len, u8* temporary_buffer)
{
    uint8_t number = data_len / 5;
    uint16_t len = 0;
    uint16_t sent_len = 4;
    uint8_t sub_id;
    uint8_t port = 0;
    float ftemp;
    uint32_t temp32;
    //uint16_t CRC_16_code = 0x0000;

    if(ch & CH_1) {
        port = DAC80508_CH1;
    } else if(ch & CH_2) {
        port = DAC80508_CH2;
    } else if(ch & CH_3) {
        port = DAC80508_CH3;
    } else if(ch & CH_4) {
        port = DAC80508_CH4;
    } else if(ch & CH_5) {
        port = DAC80508_CH5;
    } else if(ch & CH_6) {
        port = DAC80508_CH6;
    } else if(ch & CH_7) {
        port = DAC80508_CH7;
    } else if(ch & CH_8) {
        port = DAC80508_CH8;
    } else {
        Analysis_Flag = 0x00;
        return;
    }

    EEPROM_WriteLenByte(0, CALIBRATION_FLAG, 2);
    Calibration_Read_Flag = 1;

    for(int i = 0; i < number; i++) {
        sub_id = data[len];
        temporary_buffer[sent_len+2] = sub_id;

        if(sub_id != 0xff) {
            if((sub_id > 0) && (sub_id < calibration_paraments_end)) {
                memcpy(&temp32, data+len+1, 4);
            #if DATABIG_ENDIAN
                temp32 = Endia_Swap32(temp32);
            #endif
                EEPROM_WriteLenByte(((port-1)*CALIBRATION_PORT_LEN+(sub_id-1)*4+2), temp32, 4);

                temporary_buffer[sent_len+3] = 0x01;
            } else {
                temporary_buffer[sent_len+3] = 0x00;
            }

        } else {
            temporary_buffer[sent_len+3] = 0x00;
        }


        len += 5;
        sent_len += 2;
    }

    // CRC_16_code = Endia_Swap16(CRC_16_code);
    // memcpy(temporary_buffer+sent_len+2, &CRC_16_code, 2);

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;
}

/***************************************************************************************************
**  校准类 --> 校准参数查询解析函数
**
**
***************************************************************************************************/
static void Calibration_Parameter_Query_Analysis(uint8_t ch, u8* data, uint16_t data_len, u8* temporary_buffer)
{
    uint8_t number = data_len;
    uint16_t len = 0;
    uint16_t sent_len = 4;
    uint8_t sub_id;
    uint8_t port = 0;
    float ftemp;
    uint16_t temp16;
    uint32_t temp32;

    if(ch & CH_1) {
        port = DAC80508_CH1;
    } else if(ch & CH_2) {
        port = DAC80508_CH2;
    } else if(ch & CH_3) {
        port = DAC80508_CH3;
    } else if(ch & CH_4) {
        port = DAC80508_CH4;
    } else if(ch & CH_5) {
        port = DAC80508_CH5;
    } else if(ch & CH_6) {
        port = DAC80508_CH6;
    } else if(ch & CH_7) {
        port = DAC80508_CH7;
    } else if(ch & CH_8) {
        port = DAC80508_CH8;
    } else {
        Analysis_Flag = 0x00;
        return;
    }

    temp16 = EEPROM_ReadLenByte(0, 2);
    if(temp16 == CALIBRATION_FLAG) {
        for(int i = 0; i < number; i++) {
            sub_id = data[len];
            temporary_buffer[sent_len+2] = sub_id;

            if(sub_id != 0xff) {
                if((sub_id > 0) && (sub_id < calibration_paraments_end)) {
                    temp32 = EEPROM_ReadLenByte(((port-1)*CALIBRATION_PORT_LEN+(sub_id-1)*4+2), 4);
                #if DATABIG_ENDIAN
                    temp32 = Endia_Swap32(temp32);
                #endif
                    memcpy(temporary_buffer+sent_len+3, &temp32, 4);
                } else {
                    temp32 = 0xffffffff;
                    memcpy(temporary_buffer+sent_len+3, &temp32, 4);
                }

            } else {
                temp32 = 0xffffffff;
                memcpy(temporary_buffer+sent_len+3, &temp32, 4);
            }

            sent_len += 5;
            len += 1;
        }
    } else {
        temporary_buffer[sent_len+2] = 0x00;
        sent_len += 1;
    }

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;
}

/***************************************************************************************************
**  校准类 --> AD数据读取解析函数
**
**
***************************************************************************************************/
static void Calibration_AD_Data_Query_Analysis(uint8_t ch, u8* data, uint16_t data_len, u8* temporary_buffer)
{
    uint8_t number = data_len;
    uint16_t len = 0;
    uint16_t sent_len = 4;
    uint8_t sub_id;
    uint8_t port = 0;
    uint16_t temp16;
    uint32_t temp32;

    if(ch & CH_1) {
        port = DAC80508_CH1;
    } else if(ch & CH_2) {
        port = DAC80508_CH2;
    } else if(ch & CH_3) {
        port = DAC80508_CH3;
    } else if(ch & CH_4) {
        port = DAC80508_CH4;
    } else if(ch & CH_5) {
        port = DAC80508_CH5;
    } else if(ch & CH_6) {
        port = DAC80508_CH6;
    } else if(ch & CH_7) {
        port = DAC80508_CH7;
    } else if(ch & CH_8) {
        port = DAC80508_CH8;
    } else {
        Analysis_Flag = 0x00;
        return;
    }

    for(int i = 0; i < number; i++) {
        sub_id = data[len];
        temporary_buffer[sent_len+2] = sub_id;

        if(sub_id != 0xff) {
            if(sub_id == battery_voltage_ad) {
                temp32 = calibration_ad_record[port-1].Vsense_AD;
            } else if(sub_id == module_voltage_ad) {
                temp32 = calibration_ad_record[port-1].Vmod_AD;
            } else if(sub_id == output_current_ad) {
                temp32 = calibration_ad_record[port-1].Iout_AD;
            } else if(sub_id == vj_ad) {
                temp32 = calibration_ad_record[port-1].Vj_AD;
            } else {
                temp32 = 0;
            }
        } else {
            temp32 = 0;
        }

    #if DATABIG_ENDIAN
        temp32 = Endia_Swap32(temp32);
    #endif
        memcpy(temporary_buffer+sent_len+3, &temp32, 4);

        sent_len += 5;
        len += 1;
    }

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;

}


/***************************************************************************************************
**  校准类 --> 校准运行参数设置解析函数
**
**
***************************************************************************************************/
static void Calibration_Step_Analysis(uint8_t ch, u8* data, u8* temporary_buffer)
{
    uint32_t temporary_data;
    uint16_t len = 2;
    uint16_t sent_len = 4;
    uint8_t sub_id;
    uint8_t temp_type;
    float ftemp_current;
    float ftemp_voltage;
    //uint16_t CRC_16_code = 0x0000;

    memcpy(&temporary_data, data, 2);

    if(temporary_data == 0x0000) {
        temp_type = data[len];

        sub_id = data[len+1];
        if(sub_id != 0xff) {
            memcpy(&ftemp_current, data+len+2, 4);
        #if DATABIG_ENDIAN
            ftemp_current = Float_Swap(ftemp_current);
        #endif
        } else {
            ftemp_current = 0.0;
        }

        len += 6;

        sub_id = data[len];
        if(sub_id != 0xff) {
            memcpy(&ftemp_voltage, data+len+1, 4);
        #if DATABIG_ENDIAN
            ftemp_voltage = Float_Swap(ftemp_voltage);
        #endif
        } else {
            ftemp_voltage = 0.0;
        }


        if(ch & CH_1) {
            calibration_run[0].type = temp_type;
            calibration_run[0].current = ftemp_current;
            calibration_run[0].voltage = ftemp_voltage;
        }
        if(ch & CH_2) {
            calibration_run[1].type = temp_type;
            calibration_run[1].current = ftemp_current;
            calibration_run[1].voltage = ftemp_voltage;
        }
        if(ch & CH_3) {
            calibration_run[2].type = temp_type;
            calibration_run[2].current = ftemp_current;
            calibration_run[2].voltage = ftemp_voltage;
        }
        if(ch & CH_4) {
            calibration_run[3].type = temp_type;
            calibration_run[3].current = ftemp_current;
            calibration_run[3].voltage = ftemp_voltage;
        }
        if(ch & CH_5) {
            calibration_run[4].type = temp_type;
            calibration_run[4].current = ftemp_current;
            calibration_run[4].voltage = ftemp_voltage;
        }
        if(ch & CH_6) {
            calibration_run[5].type = temp_type;
            calibration_run[5].current = ftemp_current;
            calibration_run[5].voltage = ftemp_voltage;
        }
        if(ch & CH_7) {
            calibration_run[6].type = temp_type;
            calibration_run[6].current = ftemp_current;
            calibration_run[6].voltage = ftemp_voltage;
        }
        if(ch & CH_8) {
            calibration_run[7].type = temp_type;
            calibration_run[7].current = ftemp_current;
            calibration_run[7].voltage = ftemp_voltage;
        }

        temporary_buffer[sent_len+6] = 0x01;
    } else {
        temporary_buffer[sent_len+6] = 0x00;
    }

    sent_len += 5;

    // CRC_16_code = Endia_Swap16(CRC_16_code);
    // memcpy(temporary_buffer+sent_len+2, &CRC_16_code, 2);

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;
}

/***************************************************************************************************
**  校准类 --> 校准运行设置解析函数
**
**
***************************************************************************************************/
static void Calibration_Run_Analysis(uint8_t ch, u8* data, u8* temporary_buffer)
{
    uint16_t sent_len = 4;
    // uint16_t CRC_16_code;

    if(ch & CH_1) {
        calibration_run[0].status = data[0];
        calibration_run[0].run_flag = 1;
    }
    if(ch & CH_2) {
        calibration_run[1].status = data[0];
        calibration_run[1].run_flag = 1;
    }
    if(ch & CH_3) {
        calibration_run[2].status = data[0];
        calibration_run[2].run_flag = 1;
    }
    if(ch & CH_4) {
        calibration_run[3].status = data[0];
        calibration_run[3].run_flag = 1;
    }
    if(ch & CH_5) {
        calibration_run[4].status = data[0];
        calibration_run[4].run_flag = 1;
    }
    if(ch & CH_6) {
        calibration_run[5].status = data[0];
        calibration_run[5].run_flag = 1;
    }
    if(ch & CH_7) {
        calibration_run[6].status = data[0];
        calibration_run[6].run_flag = 1;
    }
    if(ch & CH_8) {
        calibration_run[7].status = data[0];
        calibration_run[7].run_flag = 1;
    }

    temporary_buffer[sent_len+6] = 0x01;

    sent_len += 5;

    // CRC_16_code = Endia_Swap16(CRC_16_code);
    // memcpy(temporary_buffer+sent_len+2, &CRC_16_code, 2);

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;
}


/***************************************************************************************************
**  调试相关的数据解析函数
**
**
***************************************************************************************************/
static void Debug_Parameter_Setting_Analysis(uint8_t ch, u8* data, uint16_t data_len, u8* temporary_buffer)
{
    uint16_t len = 0;
    uint16_t sent_len = 4;
    uint16_t temp16;
    uint32_t temp32;
    float ftemp;
    float ftemp_v;
    float ftemp_i;
    uint8_t port_enable;
    uint8_t port_dir;
    uint16_t Vout_num = 0;
    uint16_t Iout_num = 0;

    if(ch == 0x00) {
        Analysis_Flag = 0x00;
        return;
    } else {
        // MODE_PORT1_MASTER;MODE_PORT2_MASTER;MODE_PORT3_MASTER;MODE_PORT4_MASTER;
        // MODE_PORT5_MASTER;MODE_PORT6_MASTER;MODE_PORT7_MASTER;MODE_PORT8_MASTER;

        port_enable = data[0];
        port_dir = data[1];
        memcpy(&ftemp, data+2, 4);
    #if DATABIG_ENDIAN
        ftemp_v = Float_Swap(ftemp);
    #else
        ftemp_v = ftemp;
    #endif
        memcpy(&ftemp, data+6, 4);
    #if DATABIG_ENDIAN
        ftemp_i = Float_Swap(ftemp);
    #else
        ftemp_i = ftemp;
    #endif

        if(ch & 0x01) {
            if((ftemp_v <= 6.5) && (ftemp_i <= 85.0)) {
                if((port_dir == BUCK_MODE) && (Dir_flag[0] != BUCK_MODE)) {
                    PORT1_DIS;
                    PORT1_DIR = BUCK_SET;
                    Dir_flag[0] = BUCK_MODE;
                } else if((port_dir == BOOST_MODE) && (Dir_flag[0] != BOOST_MODE)) {
                    PORT1_DIS;
                    PORT1_DIR = BOOST_SET;
                    Dir_flag[0] = BOOST_MODE;
                }


                // if(ftemp_i <= I_RANGE) {
                //     I1_OUT = I_LOW_SET;
                //     Irange_flag[0] = I_LOW;
                // } else {
                //     I1_OUT = I_HIGH_SET;
                //     Irange_flag[0] = I_HIGH;
                // }

                if(Dir_flag[0] == BUCK_MODE) {
                    if(Vrange_flag[0] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[0].Buck_Vset_sense_K + calibration[0].Buck_Vset_sense_B);
                    } else if(Vrange_flag[0] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[0].Buck_Vset_mod_K + calibration[0].Buck_Vset_mod_B);
                    }

                    if(Irange_flag[0] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[0].Buck_Iset_HK + calibration[0].Buck_Iset_HB);
                    } else if(Irange_flag[0] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[0].Buck_Iset_LK + calibration[0].Buck_Iset_LB);
                    }
                } else if(Dir_flag[0] == BOOST_MODE) {
                    if(Vrange_flag[0] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[0].Boost_Vset_sense_K + calibration[0].Boost_Vset_sense_B);
                    } else if(Vrange_flag[0] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[0].Boost_Vset_mod_K + calibration[0].Boost_Vset_mod_B);
                    }

                    if(Irange_flag[0] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[0].Boost_Iset_HK + calibration[0].Boost_Iset_HB);
                    } else if(Irange_flag[0] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[0].Boost_Iset_LK + calibration[0].Boost_Iset_LB);
                    }
                }


                DAC80508_SetDacData(0, Vout_num, 0);
                DAC80508_SetDacData(0, Iout_num, 1);

                if(port_enable == 0x01) {
                    REV_PORT1_EN;
                    HAL_Delay(10);
                    PORT1_EN;
                    // PORT1A_EN;
                    // PORT1B_DIS;
                } else if(port_enable == 0x02) {
                    REV_PORT1_EN;
                    HAL_Delay(10);
                    PORT1_EN;
                    // PORT1A_DIS;
                    // PORT1B_EN;
                } else if(port_enable == 0x03) {
                    REV_PORT1_EN;
                    HAL_Delay(10);
                    PORT1_EN;
                } else {
                    DAC80508_SetDacData(0, 0, 1);
                    HAL_Delay(20);

                    PORT1_DIS;
                    // HAL_Delay(10);
                    REV_PORT1_DIS;
                }

                temporary_buffer[sent_len+2] =  0x01;
            } else {
                temporary_buffer[sent_len+2] =  0x00;
            }

        }
        if(ch & 0x02) {
            if((ftemp_v <= 6.5) && (ftemp_i <= 85.0)) {
                if((port_dir == BUCK_MODE) && (Dir_flag[1] != BUCK_MODE)) {
                    PORT2_DIS;
                    PORT2_DIR = BUCK_SET;
                    Dir_flag[1] = BUCK_MODE;
                } else if((port_dir == BOOST_MODE) && (Dir_flag[1] != BOOST_MODE)) {
                    PORT2_DIS;
                    PORT2_DIR = BOOST_SET;
                    Dir_flag[1] = BOOST_MODE;
                }

                // if(ftemp_i <= I_RANGE) {
                //     I2_OUT = I_LOW_SET;
                //     Irange_flag[1] = I_LOW;
                // } else {
                //     I2_OUT = I_HIGH_SET;
                //     Irange_flag[1] = I_HIGH;
                // }

                if(Dir_flag[1] == BUCK_MODE) {
                    if(Vrange_flag[1] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[1].Buck_Vset_sense_K + calibration[1].Buck_Vset_sense_B);
                    } else if(Vrange_flag[1] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[1].Buck_Vset_mod_K + calibration[1].Buck_Vset_mod_B);
                    }

                    if(Irange_flag[1] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[1].Buck_Iset_HK + calibration[1].Buck_Iset_HB);
                    } else if(Irange_flag[1] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[1].Buck_Iset_LK + calibration[1].Buck_Iset_LB);
                    }
                } else if(Dir_flag[1] == BOOST_MODE) {
                    if(Vrange_flag[1] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[1].Boost_Vset_sense_K + calibration[1].Boost_Vset_sense_B);
                    } else if(Vrange_flag[1] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[1].Boost_Vset_mod_K + calibration[1].Boost_Vset_mod_B);
                    }

                    if(Irange_flag[1] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[1].Boost_Iset_HK + calibration[1].Boost_Iset_HB);
                    } else if(Irange_flag[1] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[1].Boost_Iset_LK + calibration[1].Boost_Iset_LB);
                    }
                }

                DAC80508_SetDacData(1, Vout_num, 0);
                DAC80508_SetDacData(1, Iout_num, 1);

                if(port_enable == 0x01) {
                    REV_PORT2_EN;
                    HAL_Delay(10);
                    PORT2_EN;
                    // PORT2A_EN;
                    // PORT2B_DIS;
                } else if(port_enable == 0x02) {
                    REV_PORT2_EN;
                    HAL_Delay(10);
                    PORT2_EN;
                    // PORT2A_DIS;
                    // PORT2B_EN;
                } else if(port_enable == 0x03) {
                    REV_PORT2_EN;
                    HAL_Delay(10);
                    PORT2_EN;
                } else {
                    DAC80508_SetDacData(1, 0, 1);
                    HAL_Delay(20);

                    PORT2_DIS;
                    // HAL_Delay(10);
                    REV_PORT2_DIS;


                }

                temporary_buffer[sent_len+2] =  0x01;
            } else {
                temporary_buffer[sent_len+2] =  0x00;
            }

        }
        if(ch & 0x04) {
            if((ftemp_v <= 6.5) && (ftemp_i <= 85.0)) {
                if((port_dir == BUCK_MODE) && (Dir_flag[2] != BUCK_MODE)) {
                    PORT3_DIS;
                    PORT3_DIR = BUCK_SET;
                    Dir_flag[2] = BUCK_MODE;
                } else if((port_dir == BOOST_MODE) && (Dir_flag[2] != BOOST_MODE)) {
                    PORT3_DIS;
                    PORT3_DIR = BOOST_SET;
                    Dir_flag[2] = BOOST_MODE;
                }

                // if(ftemp_i <= I_RANGE) {
                //     I3_OUT = I_LOW_SET;
                //     Irange_flag[2] = I_LOW;
                // } else {
                //     I3_OUT = I_HIGH_SET;
                //     Irange_flag[2] = I_HIGH;
                // }

                if(Dir_flag[2] == BUCK_MODE) {
                    if(Vrange_flag[2] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[2].Buck_Vset_sense_K + calibration[2].Buck_Vset_sense_B);
                    } else if(Vrange_flag[2] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[2].Buck_Vset_mod_K + calibration[2].Buck_Vset_mod_B);
                    }

                    if(Irange_flag[2] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[2].Buck_Iset_HK + calibration[2].Buck_Iset_HB);
                    } else if(Irange_flag[2] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[2].Buck_Iset_LK + calibration[2].Buck_Iset_LB);
                    }
                } else if(Dir_flag[2] == BOOST_MODE) {
                    if(Vrange_flag[2] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[2].Boost_Vset_sense_K + calibration[2].Boost_Vset_sense_B);
                    } else if(Vrange_flag[2] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[2].Boost_Vset_mod_K + calibration[2].Boost_Vset_mod_B);
                    }

                    if(Irange_flag[2] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[2].Boost_Iset_HK + calibration[2].Boost_Iset_HB);
                    } else if(Irange_flag[2] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[2].Boost_Iset_LK + calibration[2].Boost_Iset_LB);
                    }
                }

                DAC80508_SetDacData(2, Vout_num, 0);
                DAC80508_SetDacData(2, Iout_num, 1);

                if(port_enable == 0x01) {
                    REV_PORT3_EN;
                    HAL_Delay(10);
                    PORT3_EN;
                    // PORT3A_EN;
                    // PORT3B_DIS;
                } else if(port_enable == 0x02) {
                    REV_PORT3_EN;
                    HAL_Delay(10);
                    PORT3_EN;
                    // PORT3A_DIS;
                    // PORT3B_EN;
                } else if(port_enable == 0x03) {
                    REV_PORT3_EN;
                    HAL_Delay(10);
                    PORT3_EN;
                } else {
                    DAC80508_SetDacData(2, 0, 1);
                    HAL_Delay(20);

                    PORT3_DIS;
                    // HAL_Delay(10);
                    REV_PORT3_DIS;
                }

                temporary_buffer[sent_len+2] =  0x01;
            } else {
                temporary_buffer[sent_len+2] =  0x00;
            }

        }
        if(ch & 0x08) {
            if((ftemp_v <= 6.5) && (ftemp_i <= 85.0)) {
                if((port_dir == BUCK_MODE) && (Dir_flag[3] != BUCK_MODE)) {
                    PORT4_DIS;
                    PORT4_DIR = BUCK_SET;
                    Dir_flag[3] = BUCK_MODE;
                } else if((port_dir == BOOST_MODE) && (Dir_flag[3] != BOOST_MODE)) {
                    PORT4_DIS;
                    PORT4_DIR = BOOST_SET;
                    Dir_flag[3] = BOOST_MODE;
                }

                // if(ftemp_i <= I_RANGE) {
                //     I4_OUT = I_LOW_SET;
                //     Irange_flag[3] = I_LOW;
                // } else {
                //     I4_OUT = I_HIGH_SET;
                //     Irange_flag[3] = I_HIGH;
                // }

                if(Dir_flag[3] == BUCK_MODE) {
                    if(Vrange_flag[3] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[3].Buck_Vset_sense_K + calibration[3].Buck_Vset_sense_B);
                    } else if(Vrange_flag[3] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[3].Buck_Vset_mod_K + calibration[3].Buck_Vset_mod_B);
                    }

                    if(Irange_flag[3] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[3].Buck_Iset_HK + calibration[3].Buck_Iset_HB);
                    } else if(Irange_flag[3] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[3].Buck_Iset_LK + calibration[3].Buck_Iset_LB);
                    }
                } else if(Dir_flag[3] == BOOST_MODE) {
                    if(Vrange_flag[3] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[3].Boost_Vset_sense_K + calibration[3].Boost_Vset_sense_B);
                    } else if(Vrange_flag[3] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[3].Boost_Vset_mod_K + calibration[3].Boost_Vset_mod_B);
                    }

                    if(Irange_flag[3] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[3].Boost_Iset_HK + calibration[3].Boost_Iset_HB);
                    } else if(Irange_flag[3] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[3].Boost_Iset_LK + calibration[3].Boost_Iset_LB);
                    }
                }

                DAC80508_SetDacData(3, Vout_num, 0);
                DAC80508_SetDacData(3, Iout_num, 1);

                if(port_enable == 0x01) {
                    REV_PORT4_EN;
                    HAL_Delay(10);
                    PORT4_EN;
                    // PORT4A_EN;
                    // PORT4B_DIS;
                } else if(port_enable == 0x02) {
                    REV_PORT4_EN;
                    HAL_Delay(10);
                    PORT4_EN;
                    // PORT4A_DIS;
                    // PORT4B_EN;
                } else if(port_enable == 0x03) {
                    REV_PORT4_EN;
                    HAL_Delay(10);
                    PORT4_EN;
                } else {
                    DAC80508_SetDacData(3, 0, 1);
                    HAL_Delay(20);

                    PORT4_DIS;
                    // HAL_Delay(10);
                    REV_PORT4_DIS;
                }

                temporary_buffer[sent_len+2] =  0x01;
            } else {
                temporary_buffer[sent_len+2] =  0x00;
            }

        }
        if(ch & 0x10) {
            if((ftemp_v <= 6.5) && (ftemp_i <= 85.0)) {
                if((port_dir == BUCK_MODE) && (Dir_flag[4] != BUCK_MODE)) {
                    PORT5_DIS;
                    PORT5_DIR = BUCK_SET;
                    Dir_flag[4] = BUCK_MODE;
                } else if((port_dir == BOOST_MODE) && (Dir_flag[4] != BOOST_MODE)) {
                    PORT5_DIS;
                    PORT5_DIR = BOOST_SET;
                    Dir_flag[4] = BOOST_MODE;
                }

                // if(ftemp_i <= I_RANGE) {
                //     I5_OUT = I_LOW_SET;
                //     Irange_flag[4] = I_LOW;
                // } else {
                //     I5_OUT = I_HIGH_SET;
                //     Irange_flag[4] = I_HIGH;
                // }

                if(Dir_flag[4] == BUCK_MODE) {
                    if(Vrange_flag[4] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[4].Buck_Vset_sense_K + calibration[4].Buck_Vset_sense_B);
                    } else if(Vrange_flag[4] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[4].Buck_Vset_mod_K + calibration[4].Buck_Vset_mod_B);
                    }

                    if(Irange_flag[4] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[4].Buck_Iset_HK + calibration[4].Buck_Iset_HB);
                    } else if(Irange_flag[4] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[4].Buck_Iset_LK + calibration[4].Buck_Iset_LB);
                    }
                } else if(Dir_flag[4] == BOOST_MODE) {
                    if(Vrange_flag[4] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[4].Boost_Vset_sense_K + calibration[4].Boost_Vset_sense_B);
                    } else if(Vrange_flag[4] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[4].Boost_Vset_mod_K + calibration[4].Boost_Vset_mod_B);
                    }

                    if(Irange_flag[4] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[4].Boost_Iset_HK + calibration[4].Boost_Iset_HB);
                    } else if(Irange_flag[4] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[4].Boost_Iset_LK + calibration[4].Boost_Iset_LB);
                    }
                }

                DAC80508_SetDacData(4, Vout_num, 0);
                DAC80508_SetDacData(4, Iout_num, 1);

                if(port_enable == 0x01) {
                    REV_PORT5_EN;
                    HAL_Delay(10);
                    PORT5_EN;
                    // PORT5A_EN;
                    // PORT5B_DIS;
                } else if(port_enable == 0x02) {
                    REV_PORT5_EN;
                    HAL_Delay(10);
                    PORT5_EN;
                    // PORT5A_DIS;
                    // PORT5B_EN;
                } else if(port_enable == 0x03) {
                    REV_PORT5_EN;
                    HAL_Delay(10);
                    PORT5_EN;
                } else {
                    DAC80508_SetDacData(4, 0, 1);
                    HAL_Delay(20);

                    PORT5_DIS;
                    // HAL_Delay(10);
                    REV_PORT5_DIS;
                }

                temporary_buffer[sent_len+2] =  0x01;
            } else {
                temporary_buffer[sent_len+2] =  0x00;
            }

        }
        if(ch & 0x20) {
            if((ftemp_v <= 6.5) && (ftemp_i <= 85.0)) {
                if((port_dir == BUCK_MODE) && (Dir_flag[5] != BUCK_MODE)) {
                    PORT6_DIS;
                    PORT6_DIR = BUCK_SET;
                    Dir_flag[5] = BUCK_MODE;
                } else if((port_dir == BOOST_MODE) && (Dir_flag[5] != BOOST_MODE)) {
                    PORT6_DIS;
                    PORT6_DIR = BOOST_SET;
                    Dir_flag[5] = BOOST_MODE;
                }

                // if(ftemp_i <= I_RANGE) {
                //     I6_OUT = I_LOW_SET;
                //     Irange_flag[5] = I_LOW;
                // } else {
                //     I6_OUT = I_HIGH_SET;
                //     Irange_flag[5] = I_HIGH;
                // }

                if(Dir_flag[5] == BUCK_MODE) {
                    if(Vrange_flag[5] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[5].Buck_Vset_sense_K + calibration[5].Buck_Vset_sense_B);
                    } else if(Vrange_flag[5] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[5].Buck_Vset_mod_K + calibration[5].Buck_Vset_mod_B);
                    }

                    if(Irange_flag[5] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[5].Buck_Iset_HK + calibration[5].Buck_Iset_HB);
                    } else if(Irange_flag[5] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[5].Buck_Iset_LK + calibration[5].Buck_Iset_LB);
                    }
                } else if(Dir_flag[5] == BOOST_MODE) {
                    if(Vrange_flag[5] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[5].Boost_Vset_sense_K + calibration[5].Boost_Vset_sense_B);
                    } else if(Vrange_flag[5] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[5].Boost_Vset_mod_K + calibration[5].Boost_Vset_mod_B);
                    }

                    if(Irange_flag[5] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[5].Boost_Iset_HK + calibration[5].Boost_Iset_HB);
                    } else if(Irange_flag[5] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[5].Boost_Iset_LK + calibration[5].Boost_Iset_LB);
                    }
                }

                DAC80508_SetDacData(5, Vout_num, 0);
                DAC80508_SetDacData(5, Iout_num, 1);

                if(port_enable == 0x01) {
                    REV_PORT6_EN;
                    HAL_Delay(10);
                    PORT6_EN;
                    // PORT6A_EN;
                    // PORT6B_DIS;
                } else if(port_enable == 0x02) {
                    REV_PORT6_EN;
                    HAL_Delay(10);
                    PORT6_EN;
                    // PORT6A_DIS;
                    // PORT6B_EN;
                } else if(port_enable == 0x03) {
                    REV_PORT6_EN;
                    HAL_Delay(10);
                    PORT6_EN;
                } else {
                    DAC80508_SetDacData(5, 0, 1);
                    HAL_Delay(20);

                    PORT6_DIS;
                    // HAL_Delay(10);
                    REV_PORT6_DIS;
                }

                temporary_buffer[sent_len+2] =  0x01;
            } else {
                temporary_buffer[sent_len+2] =  0x00;
            }

        }
        if(ch & 0x40) {
            if((ftemp_v <= 6.5) && (ftemp_i <= 85.0)) {
                if((port_dir == BUCK_MODE) && (Dir_flag[6] != BUCK_MODE)) {
                    PORT7_DIS;
                    PORT7_DIR = BUCK_SET;
                    Dir_flag[6] = BUCK_MODE;
                } else if((port_dir == BOOST_MODE) && (Dir_flag[6] != BOOST_MODE)) {
                    PORT7_DIS;
                    PORT7_DIR = BOOST_SET;
                    Dir_flag[6] = BOOST_MODE;
                }

                // if(ftemp_i <= I_RANGE) {
                //     I7_OUT = I_LOW_SET;
                //     Irange_flag[6] = I_LOW;
                // } else {
                //     I7_OUT = I_HIGH_SET;
                //     Irange_flag[6] = I_HIGH;
                // }

                if(Dir_flag[6] == BUCK_MODE) {
                    if(Vrange_flag[6] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[6].Buck_Vset_sense_K + calibration[6].Buck_Vset_sense_B);
                    } else if(Vrange_flag[6] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[6].Buck_Vset_mod_K + calibration[6].Buck_Vset_mod_B);
                    }

                    if(Irange_flag[6] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[6].Buck_Iset_HK + calibration[6].Buck_Iset_HB);
                    } else if(Irange_flag[6] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[6].Buck_Iset_LK + calibration[6].Buck_Iset_LB);
                    }
                } else if(Dir_flag[6] == BOOST_MODE) {
                    if(Vrange_flag[6] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[6].Boost_Vset_sense_K + calibration[6].Boost_Vset_sense_B);
                    } else if(Vrange_flag[6] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[6].Boost_Vset_mod_K + calibration[6].Boost_Vset_mod_B);
                    }

                    if(Irange_flag[6] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[6].Boost_Iset_HK + calibration[6].Boost_Iset_HB);
                    } else if(Irange_flag[6] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[6].Boost_Iset_LK + calibration[6].Boost_Iset_LB);
                    }
                }

                DAC80508_SetDacData(6, Vout_num, 0);
                DAC80508_SetDacData(6, Iout_num, 1);

                if(port_enable == 0x01) {
                    REV_PORT7_EN;
                    HAL_Delay(10);
                    PORT7_EN;
                    // PORT7A_EN;
                    // PORT7B_DIS;
                } else if(port_enable == 0x02) {
                    REV_PORT7_EN;
                    HAL_Delay(10);
                    PORT7_EN;
                    // PORT7A_DIS;
                    // PORT7B_EN;
                } else if(port_enable == 0x03) {
                    REV_PORT7_EN;
                    HAL_Delay(10);
                    PORT7_EN;
                } else {
                    DAC80508_SetDacData(6, 0, 1);
                    HAL_Delay(20);

                    PORT7_DIS;
                    // HAL_Delay(10);
                    REV_PORT7_DIS;
                }

                temporary_buffer[sent_len+2] =  0x01;
            } else {
                temporary_buffer[sent_len+2] =  0x00;
            }

        }
        if(ch & 0x80) {
            if((ftemp_v <= 6.5) && (ftemp_i <= 85.0)) {
                if((port_dir == BUCK_MODE) && (Dir_flag[7] != BUCK_MODE)) {
                    PORT8_DIS;
                    PORT8_DIR = BUCK_SET;
                    Dir_flag[7] = BUCK_MODE;
                } else if((port_dir == BOOST_MODE) && (Dir_flag[7] != BOOST_MODE)) {
                    PORT8_DIS;
                    PORT8_DIR = BOOST_SET;
                    Dir_flag[7] = BOOST_MODE;
                }

                // if(ftemp_i <= I_RANGE) {
                //     I8_OUT = I_LOW_SET;
                //     Irange_flag[7] = I_LOW;
                // } else {
                //     I8_OUT = I_HIGH_SET;
                //     Irange_flag[7] = I_HIGH;
                // }

                if(Dir_flag[7] == BUCK_MODE) {
                    if(Vrange_flag[7] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[7].Buck_Vset_sense_K + calibration[7].Buck_Vset_sense_B);
                    } else if(Vrange_flag[7] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[7].Buck_Vset_mod_K + calibration[7].Buck_Vset_mod_B);
                    }

                    if(Irange_flag[7] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[7].Buck_Iset_HK + calibration[7].Buck_Iset_HB);
                    } else if(Irange_flag[7] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[7].Buck_Iset_LK + calibration[7].Buck_Iset_LB);
                    }
                } else if(Dir_flag[7] == BOOST_MODE) {
                    if(Vrange_flag[7] == V_SENSE) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[7].Boost_Vset_sense_K + calibration[7].Boost_Vset_sense_B);
                    } else if(Vrange_flag[7] == V_MOD) {
                        Vout_num = (uint16_t)(ftemp_v * calibration[7].Boost_Vset_mod_K + calibration[7].Boost_Vset_mod_B);
                    }

                    if(Irange_flag[7] == I_HIGH) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[7].Boost_Iset_HK + calibration[7].Boost_Iset_HB);
                    } else if(Irange_flag[7] == I_LOW) {
                        Iout_num = (uint16_t)(ftemp_i * calibration[7].Boost_Iset_LK + calibration[7].Boost_Iset_LB);
                    }
                }

                DAC80508_SetDacData(7, Vout_num, 0);
                DAC80508_SetDacData(7, Iout_num, 1);

                if(port_enable == 0x01) {
                    REV_PORT8_EN;
                    HAL_Delay(10);
                    PORT8_EN;
                    // PORT8A_EN;
                    // PORT8B_DIS;
                } else if(port_enable == 0x02) {
                    REV_PORT8_EN;
                    HAL_Delay(10);
                    PORT8_EN;
                    // PORT8A_DIS;
                    // PORT8B_EN;
                } else if(port_enable == 0x03) {
                    REV_PORT8_EN;
                    HAL_Delay(10);
                    PORT8_EN;
                } else {
                    DAC80508_SetDacData(7, 0, 1);
                    HAL_Delay(20);

                    PORT8_DIS;
                    // HAL_Delay(10);
                    REV_PORT8_DIS;
                }

                temporary_buffer[sent_len+2] =  0x01;
            } else {
                temporary_buffer[sent_len+2] =  0x00;
            }

        }

        sent_len += 1;
    }

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;

}

static void Debug_Data_Query_Analysis(uint8_t ch, u8* temporary_buffer)
{
    uint16_t sent_len = 4;
    uint32_t temp32;
    float ftemp;

    if(ch == 0x00) {
        ftemp = filter_data_now[0].v_mod;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[0].i_out;
        if(Dir_flag[0] == BOOST_MODE) {
            ftemp = 0 - ftemp;
        }
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[0].v_sense;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[0].mod_tempetature;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[0].v_j;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

        ftemp = filter_data_now[1].v_mod;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[1].i_out;
        if(Dir_flag[1] == BOOST_MODE) {
            ftemp = 0 - ftemp;
        }
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[1].v_sense;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[1].mod_tempetature;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[1].v_j;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

        ftemp = filter_data_now[2].v_mod;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[2].i_out;
        if(Dir_flag[2] == BOOST_MODE) {
            ftemp = 0 - ftemp;
        }
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[2].v_sense;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[2].mod_tempetature;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[2].v_j;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

        ftemp = filter_data_now[3].v_mod;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[3].i_out;
        if(Dir_flag[3] == BOOST_MODE) {
            ftemp = 0 - ftemp;
        }
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[3].v_sense;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[3].mod_tempetature;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[3].v_j;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

        ftemp = filter_data_now[4].v_mod;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[4].i_out;
        if(Dir_flag[4] == BOOST_MODE) {
            ftemp = 0 - ftemp;
        }
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[4].v_sense;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[4].mod_tempetature;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[4].v_j;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

        ftemp = filter_data_now[5].v_mod;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[5].i_out;
        if(Dir_flag[5] == BOOST_MODE) {
            ftemp = 0 - ftemp;
        }
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[5].v_sense;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[5].mod_tempetature;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[5].v_j;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

        ftemp = filter_data_now[6].v_mod;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[6].i_out;
        if(Dir_flag[6] == BOOST_MODE) {
            ftemp = 0 - ftemp;
        }
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[6].v_sense;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[6].mod_tempetature;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[6].v_j;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

        ftemp = filter_data_now[7].v_mod;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[7].i_out;
        if(Dir_flag[7] == BOOST_MODE) {
            ftemp = 0 - ftemp;
        }
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[7].v_sense;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[7].mod_tempetature;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = filter_data_now[7].v_j;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

        ftemp = special_sample.Bus1;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = special_sample.Bus2;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = special_sample.Aux;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        ftemp = special_sample.Amb;
    #if DATABIG_ENDIAN
        ftemp = Float_Swap(ftemp);
    #endif
        memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
    } else {
        if(ch & 0x01) {
            ftemp = filter_data_now[0].v_mod;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[0].i_out;
            if(Dir_flag[0] == BOOST_MODE) {
                ftemp = 0 - ftemp;
            }
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[0].v_sense;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[0].mod_tempetature;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[0].v_j;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Bus1;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Aux;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Amb;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        } else if(ch & 0x02) {
            ftemp = filter_data_now[1].v_mod;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[1].i_out;
            if(Dir_flag[1] == BOOST_MODE) {
                ftemp = 0 - ftemp;
            }
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[1].v_sense;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[1].mod_tempetature;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[1].v_j;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Bus1;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Aux;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Amb;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        } else if(ch & 0x04) {
            ftemp = filter_data_now[2].v_mod;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[2].i_out;
            if(Dir_flag[2] == BOOST_MODE) {
                ftemp = 0 - ftemp;
            }
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[2].v_sense;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[2].mod_tempetature;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[2].v_j;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Bus1;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Aux;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Amb;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        } else if(ch & 0x08) {
            ftemp = filter_data_now[3].v_mod;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[3].i_out;
            if(Dir_flag[3] == BOOST_MODE) {
                ftemp = 0 - ftemp;
            }
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[3].v_sense;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[3].mod_tempetature;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[3].v_j;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Bus1;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Aux;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Amb;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        } else if(ch & 0x10) {
            ftemp = filter_data_now[4].v_mod;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[4].i_out;
            if(Dir_flag[4] == BOOST_MODE) {
                ftemp = 0 - ftemp;
            }
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[4].v_sense;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[4].mod_tempetature;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[4].v_j;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Bus2;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Aux;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Amb;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        } else if(ch & 0x20) {
            ftemp = filter_data_now[5].v_mod;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[5].i_out;
            if(Dir_flag[5] == BOOST_MODE) {
                ftemp = 0 - ftemp;
            }
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[5].v_sense;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[5].mod_tempetature;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[5].v_j;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Bus2;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Aux;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Amb;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        } else if(ch & 0x40) {
            ftemp = filter_data_now[6].v_mod;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[6].i_out;
            if(Dir_flag[6] == BOOST_MODE) {
                ftemp = 0 - ftemp;
            }
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[6].v_sense;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[6].mod_tempetature;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[6].v_j;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Bus2;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Aux;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Amb;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        } else if(ch & 0x80) {
            ftemp = filter_data_now[7].v_mod;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[7].i_out;
            if(Dir_flag[7] == BOOST_MODE) {
                ftemp = 0 - ftemp;
            }
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[7].v_sense;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[7].mod_tempetature;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = filter_data_now[7].v_j;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Bus2;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Aux;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;

            ftemp = special_sample.Amb;
        #if DATABIG_ENDIAN
            ftemp = Float_Swap(ftemp);
        #endif
            memcpy(temporary_buffer+sent_len+2, &ftemp, 4);     sent_len += 4;
        }
    }

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;
}

static void Debug_System_Setting_Analysis(uint8_t ch, u8* data, uint16_t data_len, u8* temporary_buffer)
{
    uint16_t len = 0;
    uint16_t sent_len = 4;
    uint8_t sub_id;
    uint8_t temp8;

    sub_id = data[0];
    temp8 = data[1];
    temporary_buffer[sent_len+2] = sub_id;      sent_len += 1;

    if(sub_id == 0x01) {
        if(temp8 & 0x01) {
            V1_MOD;
            Vrange_flag[0] = V_MOD;
        } else {
            V1_SENSE;
            Vrange_flag[0] = V_SENSE;
        }

        if(temp8 & 0x02) {
            V2_MOD;
            Vrange_flag[1] = V_MOD;
        } else {
            V2_SENSE;
            Vrange_flag[1] = V_SENSE;
        }

        if(temp8 & 0x04) {
            V3_MOD;
            Vrange_flag[2] = V_MOD;
        } else {
            V3_SENSE;
            Vrange_flag[2] = V_SENSE;
        }

        if(temp8 & 0x08) {
            V4_MOD;
            Vrange_flag[3] = V_MOD;
        } else {
            V4_SENSE;
            Vrange_flag[3] = V_SENSE;
        }

        if(temp8 & 0x10) {
            V5_MOD;
            Vrange_flag[4] = V_MOD;
        } else {
            V5_SENSE;
            Vrange_flag[4] = V_SENSE;
        }

        if(temp8 & 0x20) {
            V6_MOD;
            Vrange_flag[5] = V_MOD;
        } else {
            V6_SENSE;
            Vrange_flag[5] = V_SENSE;
        }

        if(temp8 & 0x40) {
            V7_MOD;
            Vrange_flag[6] = V_MOD;
        } else {
            V7_SENSE;
            Vrange_flag[6] = V_SENSE;
        }

        if(temp8 & 0x80) {
            V8_MOD;
            Vrange_flag[7] = V_MOD;
        } else {
            V8_SENSE;
            Vrange_flag[7] = V_SENSE;
        }

        temporary_buffer[sent_len+2] = 0x01;        sent_len += 1;
    } else if(sub_id == 0x02) {
        if(temp8 & 0x01) {
            I1OUT_H;
            Irange_flag[0] = I_HIGH;
        } else {
            I1OUT_L;
            Irange_flag[0] = I_LOW;
        }

        if(temp8 & 0x02) {
            I2OUT_H;
            Irange_flag[1] = I_HIGH;
        } else {
            I2OUT_L;
            Irange_flag[1] = I_LOW;
        }

        if(temp8 & 0x04) {
            I3OUT_H;
            Irange_flag[2] = I_HIGH;
        } else {
            I3OUT_L;
            Irange_flag[2] = I_LOW;
        }

        if(temp8 & 0x08) {
            I4OUT_H;
            Irange_flag[3] = I_HIGH;
        } else {
            I4OUT_L;
            Irange_flag[3] = I_LOW;
        }

        if(temp8 & 0x10) {
            I5OUT_H;
            Irange_flag[4] = I_HIGH;
        } else {
            I5OUT_L;
            Irange_flag[4] = I_LOW;
        }

        if(temp8 & 0x20) {
            I6OUT_H;
            Irange_flag[5] = I_HIGH;
        } else {
            I6OUT_L;
            Irange_flag[5] = I_LOW;
        }

        if(temp8 & 0x40) {
            I7OUT_H;
            Irange_flag[6] = I_HIGH;
        } else {
            I7OUT_L;
            Irange_flag[6] = I_LOW;
        }

        if(temp8 & 0x80) {
            I8OUT_H;
            Irange_flag[7] = I_HIGH;
        } else {
            I8OUT_L;
            Irange_flag[7] = I_LOW;
        }

        temporary_buffer[sent_len+2] = 0x01;        sent_len += 1;
    } else if(sub_id == 0x03) {
        if(temp8 & 0x01) {
            MODE_PORT1_SLAVE;
        } else {
            MODE_PORT1_MASTER;
        }

        if(temp8 & 0x02) {
            MODE_PORT2_SLAVE;
        } else {
            MODE_PORT2_MASTER;
        }

        if(temp8 & 0x04) {
            MODE_PORT3_SLAVE;
        } else {
            MODE_PORT3_MASTER;
        }

        if(temp8 & 0x08) {
            MODE_PORT4_SLAVE;
        } else {
            MODE_PORT4_MASTER;
        }

        if(temp8 & 0x10) {
            MODE_PORT5_SLAVE;
        } else {
            MODE_PORT5_MASTER;
        }

        if(temp8 & 0x20) {
            MODE_PORT6_SLAVE;
        } else {
            MODE_PORT6_MASTER;
        }

        if(temp8 & 0x40) {
            MODE_PORT7_SLAVE;
        } else {
            MODE_PORT7_MASTER;
        }

        if(temp8 & 0x80) {
            MODE_PORT8_SLAVE;
        } else {
            MODE_PORT8_MASTER;
        }

        temporary_buffer[sent_len+2] = 0x01;        sent_len += 1;
    }else {
        temporary_buffer[sent_len+2] = 0x00;        sent_len += 1;
    }

    sent_len += 2;
#if DATABIG_ENDIAN
    sent_len = Endia_Swap16(sent_len);
#endif
    memcpy(temporary_buffer+2, &sent_len, 2);

    Analysis_Flag = 0x01;
}

//==================================================================================================


uint8_t  Boot_flag=0;
/***************************************************************************************************
**  与中位机通信的数据解析函数
**
**
***************************************************************************************************/
void Receive_Data_Analysis(void)
{
    uint16_t frame_head, frame_tail;
    uint16_t frame_len;
    uint8_t  channel_flag;
    uint8_t  command_id, subcommand_id;
    uint16_t CRC_check;
    uint16_t CRC_16_code = 0x0000;
    uint16_t temp16;
    static u8 function_data[TCP_RX_BUFFSIZE];
    static u8 send_buffer[TCP_TX_BUFFSIZE];
    uint16_t index = 0;

    if(TCP_SERVER_FLAG & 0x40) {        //如果接收到了数据
        while(index < (tcp_recvlen-9)) {
            memset(function_data, 0, TCP_RX_BUFFSIZE);
            memset(send_buffer, 0, TCP_TX_BUFFSIZE);

            memcpy(&frame_head, tcp_recvbuf+index, 2);          //获取帧头
        #if DATABIG_ENDIAN
            frame_head = Endia_Swap16(frame_head);
        #endif

            memcpy(&frame_len, tcp_recvbuf+index+2, 2);         //获取帧数据段长度
        #if DATABIG_ENDIAN
            frame_len = Endia_Swap16(frame_len);
        #endif

            // if(frame_len < (TCP_RX_BUFFSIZE-4)) {
            if((frame_len+index) <= (tcp_recvlen-4)) {      //长度在范围内
                memcpy(&frame_tail, tcp_recvbuf+index+frame_len+2, 2);  //获取帧尾
            #if DATABIG_ENDIAN
                frame_tail = Endia_Swap16(frame_tail);
            #endif

                memcpy(&CRC_check, tcp_recvbuf+index+frame_len, 2);     //获取校验码
            #if DATABIG_ENDIAN
                CRC_check = Endia_Swap16(CRC_check);
            #endif

                //进行校验计算和验证
                CRC_16_code = CRC16N(tcp_recvbuf+index+2, frame_len-2); //计算校验码
            } else {
                frame_tail = 0x0000;
                CRC_16_code = 0x0000;
            }

            // if((frame_head == FRAME_HEADER) && (frame_tail == END_OF_FRAME) && (CRC_16_code == CRC_check)) {
            if((frame_head == FRAME_HEADER) && (frame_tail == END_OF_FRAME)) {  //帧格式正确
                memcpy(&channel_flag, tcp_recvbuf+index+4, 1);
                memcpy(&command_id, tcp_recvbuf+index+5, 1);

                memcpy(function_data, tcp_recvbuf+index+6, frame_len-6);
                index += (frame_len+4);

                send_buffer[0] = (FRAME_HEADER >> 8) & 0xff;
                send_buffer[1] = FRAME_HEADER & 0xff;
                send_buffer[4] = channel_flag;

                if(command_id == Global_Protection_Setting) {           //全局保护参数设置
                    // memcpy(function_data, tcp_recvbuf+6, frame_len-6);
                    // send_buffer[5] = Global_Protection_Setting;
                    send_buffer[5] = Global_Protection_Setting_Reply;
                    Global_Protection_Setting_Analysis(channel_flag, function_data, frame_len-6, send_buffer);
                } else if(command_id == Global_Protection_Query) {      //全局保护参数查询
                    // memcpy(function_data, tcp_recvbuf+6, frame_len-6);
                    // send_buffer[5] = Global_Protection_Query;
                    send_buffer[5] = Global_Protection_Query_Reply;
                    Global_Protection_Query_Analysis(channel_flag, function_data, frame_len-6, send_buffer);
                } else if(command_id == WorkStep_Setting) {             //工步设置
                    // memcpy(function_data, tcp_recvbuf+6, frame_len-6);
                    // send_buffer[5] = WorkStep_Setting;
                    send_buffer[5] = WorkStep_Setting_Reply;
                    if(SYS_MODE == SYS_NORMAL_MODE) {               //正常模式下的工步设置解析
                        WorkStep_Setting_Analysis(channel_flag, function_data, frame_len-6, send_buffer);
                    } else if(SYS_MODE == SYS_CALIBRATION_MODE) {   //校准模式下的工步设置解析
                        Calibration_Step_Analysis(channel_flag, function_data, send_buffer);
                    }

                } else if(command_id == WorkStep_State_Setting) {       //工步运行状态设置
                    // memcpy(function_data, tcp_recvbuf+6, frame_len-6);
                    // send_buffer[5] = WorkStep_State_Setting;
                    send_buffer[5] = WorkStep_State_Setting_Reply;
                    if(SYS_MODE == SYS_NORMAL_MODE) {               //正常模式下的工步运行设置解析
                        WorkStep_Status_Setting_Analysis(channel_flag, function_data, send_buffer);
                    } else if(SYS_MODE == SYS_CALIBRATION_MODE) {   //校准模式下的工步运行设置解析
                        Calibration_Run_Analysis(channel_flag, function_data, send_buffer);
                    }

                } else if(command_id == WorkStep_Query) {           //工步参数查询
                    // send_buffer[5] = WorkStep_Query;
                    send_buffer[5] = WorkStep_Query_Reply;
                    memcpy(&temp16, function_data, 2);
                #if DATABIG_ENDIAN
                    temp16 = Endia_Swap16(temp16);
                #endif
                    WorkStep_Query_Analysis(channel_flag, temp16, send_buffer);
                } else if(command_id == Calibration_Parameter_Setting) {    //校准参数设置
                    // memcpy(function_data, tcp_recvbuf+6, frame_len-6);
                    // send_buffer[5] = Calibration_Parameter_Setting;
                    send_buffer[5] = Calibration_Parameter_Setting_Reply;
                    Calibration_Parameter_Setting_Analysis(channel_flag, function_data, frame_len-6, send_buffer);
                } else if(command_id == Calibration_Parameter_Query) {      //校准参数查询
                    // memcpy(function_data, tcp_recvbuf+6, frame_len-6);
                    // send_buffer[5] = Calibration_Parameter_Query;
                    send_buffer[5] = Calibration_Parameter_Query_Reply;
                    Calibration_Parameter_Query_Analysis(channel_flag, function_data, frame_len-6, send_buffer);
                } else if(command_id == AD_Data_Query) {
                    // memcpy(function_data, tcp_recvbuf+6, frame_len-6);
                    send_buffer[5] = AD_Data_Query_Reply;
                    Calibration_AD_Data_Query_Analysis(channel_flag, function_data, frame_len-6, send_buffer);
                } else if(command_id == Record_Query) {                 //记录信息查询
                    // send_buffer[5] = Record_Query;
                    send_buffer[5] = Record_Query_Reply;
                    Record_Query_Analysis(channel_flag, send_buffer);
                } else if(command_id == System_Query) {                 //系统信息查询
                    // subcommand_id = tcp_recvbuf[6];
                    // // send_buffer[5] = System_Query;
                    // send_buffer[5] = System_Query_Reply;
                    // send_buffer[6] = subcommand_id;

                    send_buffer[5] = System_Query_Reply;
                    subcommand_id = function_data[0];
                    send_buffer[6] = subcommand_id;
                    System_Query_Analysis(channel_flag, subcommand_id, send_buffer);
                } else if(command_id == System_Setting) {               //系统参数设置
                    // memcpy(function_data, tcp_recvbuf+6, frame_len-6);
                    // send_buffer[5] = System_Setting;
                    send_buffer[5] = System_Setting_Reply;
                    System_Setting_Analysis(channel_flag, function_data, frame_len-6, send_buffer);
                } else if(command_id == System_Update) {                //系统升级
                    // subcommand_id = tcp_recvbuf[6];
                    subcommand_id = function_data[0];
                    if(subcommand_id == 0x01) {
                        //跳转到 bootload
                        Boot_flag=1;
                        Analysis_Flag = 0x01;
                    } else {
                        Analysis_Flag = 0x00;
                    }
                }

                //调试指令解析---------------------------------------------------------------------------
                else if(command_id == Debug_Parameter_Setting) {
                    // memcpy(function_data, tcp_recvbuf+6, frame_len-6);
                    send_buffer[5] = Debug_Parameter_Setting;
                    Debug_Parameter_Setting_Analysis(channel_flag, function_data, frame_len-6, send_buffer);
                } else if(command_id == Debug_Data_Query) {
                    send_buffer[5] = Debug_Data_Query;
                    Debug_Data_Query_Analysis(channel_flag, send_buffer);
                } else if(command_id == Debug_System_Setting) {
                    // memcpy(function_data, tcp_recvbuf+6, frame_len-6);
                    send_buffer[5] = Debug_System_Setting;
                    Debug_System_Setting_Analysis(channel_flag, function_data, frame_len-6, send_buffer);
                }
                //--------------------------------------------------------------------------------------

                memcpy(&frame_len, send_buffer+2, 2);
            #if DATABIG_ENDIAN
                frame_len = Endia_Swap16(frame_len);
            #endif

                //进行校验计算
                CRC_16_code = CRC16N(send_buffer+2, frame_len-2);
            #if DATABIG_ENDIAN
                CRC_16_code = Endia_Swap16(CRC_16_code);
            #endif
                memcpy(send_buffer+frame_len, &CRC_16_code, 2);

                send_buffer[frame_len+2] = (END_OF_FRAME >> 8) & 0xff;
                send_buffer[frame_len+3] = END_OF_FRAME & 0xff;

            } else {
                // send_buffer[0] = (FRAME_HEADER >> 8) & 0xff;
                // send_buffer[1] = FRAME_HEADER & 0xff;
                // send_buffer[2] = 0xff;
                // send_buffer[3] = 0xff;
                // send_buffer[4] = (END_OF_FRAME >> 8) & 0xff;
                // send_buffer[5] = END_OF_FRAME & 0xff;

                // frame_len = 2;

                TCP_SERVER_FLAG &= ~(1<<6);     //标记数据已被处理
                return;
            }

            if(Analysis_Flag == 0x01) {
                Analysis_Flag == 0x00;
                // TCP_SERVER_FLAG &= ~(1<<6);     //标记数据已被处理

                // if(!(TCP_SERVER_FLAG & 0x80)) {
                while(!(TCP_SERVER_FLAG & 0x80)) {
                    memcpy(tcp_sentbuf, send_buffer, frame_len+4);
                    // tcp_sentbuf[frame_len+4] = '\n';
                    TCP_SERVER_FLAG |= 1<<7;    //标记要发送数据

                }
                // TCP_UserSent(server_pcb, frame_len+4);
            #if w5500_EN
                Socket_Sent(frame_len+4);
            #else
                TCP_UserSent(tcp_sentbuf, frame_len+4);
            #endif

            } else if(Analysis_Flag == 0x02) {
                Analysis_Flag == 0x00;
            } else {
                TCP_SERVER_FLAG &= ~(1<<6);     //标记数据已被处理
                return;
            }
        }

        TCP_SERVER_FLAG &= ~(1<<6);     //标记数据已被处理

    }
}


/***************************************************************************************************
**  记录数据上报
**
**
***************************************************************************************************/
void Record_Report(u8 ch)
// void Record_Report(u8 data_ch)
{
    uint16_t len = 6;
    u8 data_ch;
    static u8 record_buffer[257];
    u16 temp16;
    u16 CRC16_code = 0x0000;
    float ftemp;
    int64_t temp64;

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

    recode_query[data_ch].time_stamp = Time_Stamp;
    recode_query[data_ch].run_status = running_step[data_ch].status;
    recode_query[data_ch].step_type = running_step[data_ch].type;
    recode_query[data_ch].step_number = running_step[data_ch].number;
    recode_query[data_ch].step_sum = running_step[data_ch].sum_number;

    if((running_circle[data_ch][2].circle_flag == 0x01) && (running_circle[data_ch][2].circle_error == 0x00)) {
        recode_query[data_ch].circle_serial = running_circle[data_ch][2].circle_serial;
        recode_query[data_ch].circle_count = running_circle[data_ch][2].circle_count;
    } else if((running_circle[data_ch][1].circle_flag == 0x01) && (running_circle[data_ch][1].circle_error == 0x00)) {
        recode_query[data_ch].circle_serial = running_circle[data_ch][1].circle_serial;
        recode_query[data_ch].circle_count = running_circle[data_ch][1].circle_count;
    } else if((running_circle[data_ch][0].circle_flag == 0x01) && (running_circle[data_ch][0].circle_error == 0x00)) {
        recode_query[data_ch].circle_serial = running_circle[data_ch][0].circle_serial;
        recode_query[data_ch].circle_count = running_circle[data_ch][0].circle_count;
    } else {
        recode_query[data_ch].circle_serial = 0;
        recode_query[data_ch].circle_count = 0;
    }


    // recode_query[data_ch].run_time = record_data.channel_record_data[data_ch].run_time / 1000.0;
    recode_query[data_ch].run_time = running_step[data_ch].run_time / 1000.0;
    recode_query[data_ch].battery_voltage = record_data.channel_record_data[data_ch].battery_voltage;

    // recode_query[data_ch].output_current = record_data.channel_record_data[data_ch].output_current;
    if(((recode_query[data_ch].run_status == step_running_status) || (recode_query[data_ch].run_status == step_contact_status)) && (recode_query[data_ch].step_type != standby)) {
        recode_query[data_ch].output_current = record_data.channel_record_data[data_ch].output_current;
    #if CHANNELS_1
         if(Dir_flag[8*data_ch] == BOOST_MODE) {
            recode_query[data_ch].output_current = 0 - recode_query[data_ch].output_current;
        }
    #endif
    #if CHANNELS_2
         if(Dir_flag[4*data_ch] == BOOST_MODE) {
            recode_query[data_ch].output_current = 0 - recode_query[data_ch].output_current;
        }
    #endif
    #if CHANNELS_4
        if(Dir_flag[2*data_ch] == BOOST_MODE) {
            recode_query[data_ch].output_current = 0 - recode_query[data_ch].output_current;
        }
    #endif
    #if CHANNELS_8
         if(Dir_flag[data_ch] == BOOST_MODE) {
            recode_query[data_ch].output_current = 0 - recode_query[data_ch].output_current;
        }
    #endif
    } else {
        recode_query[data_ch].output_current = 0.0;
    }

    recode_query[data_ch].output_voltage = record_data.channel_record_data[data_ch].output_voltage;
    recode_query[data_ch].battery_capacitance = record_data.channel_record_data[data_ch].battery_capacitance;
    recode_query[data_ch].battery_energy = record_data.channel_record_data[data_ch].battery_energy;
    recode_query[data_ch].battery_temperature = record_data.channel_record_data[data_ch].battery_temperature;
    recode_query[data_ch].module_temperature = record_data.channel_record_data[data_ch].module_temperature;
    recode_query[data_ch].crosscurrent_ratio = record_data.channel_record_data[data_ch].crosscurrent_ratio;

    recode_query[data_ch].contact_impedance = record_data.channel_record_data[data_ch].contact_impedance;
    recode_query[data_ch].loop_impedance = record_data.channel_record_data[data_ch].loop_impedance;
    // if((recode_query[data_ch].output_current >= 5.0) || (recode_query[data_ch].output_current <= -5.0)) {
    //     recode_query[data_ch].contact_impedance = record_data.channel_record_data[data_ch].contact_impedance;
    //     recode_query[data_ch].loop_impedance = record_data.channel_record_data[data_ch].loop_impedance;
    // } else {
    //     recode_query[data_ch].contact_impedance = 0;
    //     recode_query[data_ch].loop_impedance = 0;
    // }


    record_buffer[0] = (FRAME_HEADER >> 8) & 0xff;
    record_buffer[1] = FRAME_HEADER & 0xff;
    // record_buffer[4] = 0x01 << data_ch;
    record_buffer[4] = ch;
    record_buffer[5] = Record_Query_Reply;

    temp64 = recode_query[data_ch].time_stamp;
#if DATABIG_ENDIAN
    temp64 = Endia_Swap64(temp64);
#endif
    memcpy(record_buffer+len, &temp64, 8);              len += 8;

    record_buffer[len] = recode_query[data_ch].run_status;
    record_buffer[len+1] = recode_query[data_ch].step_type;
    len += 2;

    temp16 = recode_query[data_ch].step_number;
#if DATABIG_ENDIAN
    temp16 = Endia_Swap16(temp16);
#endif
    memcpy(record_buffer+len, &temp16, 2);              len += 2;

    temp16 = recode_query[data_ch].step_sum;
#if DATABIG_ENDIAN
    temp16 = Endia_Swap16(temp16);
#endif
    memcpy(record_buffer+len, &temp16, 2);              len += 2;

    temp16 = recode_query[data_ch].circle_serial;
#if DATABIG_ENDIAN
    temp16 = Endia_Swap16(temp16);
#endif
    memcpy(record_buffer+len, &temp16, 2);              len += 2;

    temp16 = recode_query[data_ch].circle_count;
#if DATABIG_ENDIAN
    temp16 = Endia_Swap16(temp16);
#endif
    memcpy(record_buffer+len, &temp16, 2);              len += 2;

    // len += 8;

    ftemp = recode_query[data_ch].run_time;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(record_buffer+len, &ftemp, 4);               len += 4;

    ftemp = recode_query[data_ch].battery_voltage;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(record_buffer+len, &ftemp, 4);               len += 4;

    ftemp = recode_query[data_ch].output_current;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(record_buffer+len, &ftemp, 4);               len += 4;

    ftemp = recode_query[data_ch].output_voltage;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(record_buffer+len, &ftemp, 4);               len += 4;

    ftemp = recode_query[data_ch].battery_capacitance;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(record_buffer+len, &ftemp, 4);               len += 4;

    ftemp = recode_query[data_ch].battery_energy;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(record_buffer+len, &ftemp, 4);               len += 4;

    ftemp = recode_query[data_ch].battery_temperature;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(record_buffer+len, &ftemp, 4);               len += 4;

    ftemp = recode_query[data_ch].module_temperature;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(record_buffer+len, &ftemp, 4);               len += 4;

    ftemp = recode_query[data_ch].crosscurrent_ratio;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(record_buffer+len, &ftemp, 4);               len += 4;

    ftemp = recode_query[data_ch].contact_impedance;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(record_buffer+len, &ftemp, 4);               len += 4;

    ftemp = recode_query[data_ch].loop_impedance;
#if DATABIG_ENDIAN
    ftemp = Float_Swap(ftemp);
#endif
    memcpy(record_buffer+len, &ftemp, 4);               len += 4;

    // len += 44;

#if DATABIG_ENDIAN
    len = Endia_Swap16(len);
#endif
    memcpy(record_buffer+2, &len, 2);

#if DATABIG_ENDIAN
    len = Endia_Swap16(len);
#endif

    //进行校验计算
    CRC16_code = CRC16N(record_buffer+2, len-2);
#if DATABIG_ENDIAN
    CRC16_code = Endia_Swap16(CRC16_code);
#endif
    memcpy(record_buffer+len, &CRC16_code, 2);

    record_buffer[len+2] = (END_OF_FRAME >> 8) & 0xff;
    record_buffer[len+3] = END_OF_FRAME & 0xff;

    while(!(TCP_SERVER_FLAG & 0x80)) {
        memcpy(tcp_sentbuf, record_buffer, len+4);
        TCP_SERVER_FLAG |= 1<<7;    //标记要发送数据

    }

#if w5500_EN
    Socket_Sent(len+4);
#else
    // TCP_UserSent(server_pcb, len+4);
    TCP_UserSent(tcp_sentbuf, len+4);
#endif


    // //检测 CC 转 CV---------------------------------------------------------------------------------
    // if((recode_query[data_ch].run_status == step_running_status) && (CV_mode[data_ch] == 0) && (DCV_mode[data_ch] == 0)) {
    //     // if(recode_query[data_ch].run_status == step_running_status) {
    //         if((recode_query[data_ch].step_type == CV) || (recode_query[data_ch].step_type == CC_to_CV)) {
    //             if(recode_query[data_ch].battery_voltage >= step_work[data_ch].step_work_voltage) {
    //                 CV_mode[data_ch] = 1;
    //             }
    //         } else if((recode_query[data_ch].step_type == DCV) || (recode_query[data_ch].step_type == DCC_to_DCV)) {
    //             if(recode_query[data_ch].battery_voltage <= step_work[data_ch].step_work_voltage) {
    //                 DCV_mode[data_ch] = 1;
    //             }
    //         }
    //     // }
    // }

    // //----------------------------------------------------------------------------------------------
}


/***************************************************************************************************
**  全局保护上报
**
**
***************************************************************************************************/
void Global_Protection_Report(u8 ch)
{
    uint16_t len = 6;
    u8 data_ch;
    u8 temp8;
    u16 temp16;
    u16 CRC16_code = 0x0000;
    static u8 protection_buffer[128];


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

    protection_buffer[0] = (FRAME_HEADER >> 8) & 0xff;
    protection_buffer[1] = FRAME_HEADER & 0xff;
    protection_buffer[4] = ch;
    protection_buffer[5] = Global_Protection_Trigger_Reply;

    for(uint8_t i = 1; i < global_protection_trigger_end; i++) {
        if(Global_Protection_Flag[data_ch][i] == 0x01) {
            protection_buffer[len] = i;
            if(i == other_fault_trigger) {
                protection_buffer[len] = 0xFE;
            }
            len += 1;
        }
        // else {
        //     protection_buffer[len] = 0x00;
        // }
    }

#if DATABIG_ENDIAN
    len = Endia_Swap16(len);
#endif
    memcpy(protection_buffer+2, &len, 2);

#if DATABIG_ENDIAN
    len = Endia_Swap16(len);
#endif

    //进行校验计算
    CRC16_code = CRC16N(protection_buffer+2, len-2);
#if DATABIG_ENDIAN
    CRC16_code = Endia_Swap16(CRC16_code);
#endif
    memcpy(protection_buffer+len, &CRC16_code, 2);

    protection_buffer[len+2] = (END_OF_FRAME >> 8) & 0xff;
    protection_buffer[len+3] = END_OF_FRAME & 0xff;

    while(!(TCP_SERVER_FLAG & 0x80)) {
        memcpy(tcp_sentbuf, protection_buffer, len+4);
        TCP_SERVER_FLAG |= 1<<7;    //标记要发送数据

    }

#if w5500_EN
    Socket_Sent(len+4);
#else
    // TCP_UserSent(server_pcb, len+4);
    TCP_UserSent(tcp_sentbuf, len+4);
#endif
}

/***************************************************************************************************
**  单工步保护上报
**
**
***************************************************************************************************/
void Step_Protection_Report(u8 ch)
{
    uint16_t len = 6;
    u8 data_ch;
    u8 temp8;
    u16 temp16;
    u16 CRC16_code = 0x0000;
    static u8 protection_buffer[128];


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

    protection_buffer[0] = (FRAME_HEADER >> 8) & 0xff;
    protection_buffer[1] = FRAME_HEADER & 0xff;
    protection_buffer[4] = ch;
    protection_buffer[5] = WorkStep_Protection_Trigger_Reply;

    for(uint8_t i = 1; i < step_protection_trigger_end; i++) {
        if(Step_Protection_Flag[data_ch][i] == 0x01) {
            protection_buffer[len] = i;
            len += 1;
        }
        // else {
        //     protection_buffer[len] = 0x00;
        // }
    }

#if DATABIG_ENDIAN
    len = Endia_Swap16(len);
#endif
    memcpy(protection_buffer+2, &len, 2);

#if DATABIG_ENDIAN
    len = Endia_Swap16(len);
#endif

    //进行校验计算
    CRC16_code = CRC16N(protection_buffer+2, len-2);
#if DATABIG_ENDIAN
    CRC16_code = Endia_Swap16(CRC16_code);
#endif
    memcpy(protection_buffer+len, &CRC16_code, 2);

    protection_buffer[len+2] = (END_OF_FRAME >> 8) & 0xff;
    protection_buffer[len+3] = END_OF_FRAME & 0xff;

    while(!(TCP_SERVER_FLAG & 0x80)) {
        memcpy(tcp_sentbuf, protection_buffer, len+4);
        TCP_SERVER_FLAG |= 1<<7;    //标记要发送数据

    }

#if w5500_EN
    Socket_Sent(len+4);
#else
    // TCP_UserSent(server_pcb, len+4);
    TCP_UserSent(tcp_sentbuf, len+4);
#endif
}

uint16_t Endia_Swap16(uint16_t data16)
{
    // assert(sizeof(uint16_t) == 2);

    return ((data16 & 0xff) << 8) | (data16 >> 8);
}

uint32_t Endia_Swap32(uint32_t data32)
{
    // assert(sizeof(uint32_t) == 4);

    return ((data32 & 0x000000ff) << 24) | ((data32 & 0x0000ff00) << 8) | ((data32 & 0x00ff0000) >> 8) | ((data32 & 0xff000000) >> 24);
}

float Float_Swap(float ftemp)
{
    uint32_t temp32;
    memcpy(&temp32, &ftemp, 4);
    temp32 = ((temp32 & 0x000000ff) << 24) | ((temp32 & 0x0000ff00) << 8) | ((temp32 & 0x00ff0000) >> 8) | ((temp32 & 0xff000000) >> 24);
    memcpy(&ftemp, &temp32, 4);
    return ftemp;
}

int64_t Endia_Swap64(int64_t data64)
{
    int32_t hdata = 0,ldata = 0;
    int64_t temp64 = 0;
    hdata = (data64 & 0xffffffff00000000) >> 32;
    ldata = data64 & 0xffffffff;

    hdata = ((hdata & 0x000000ff) << 24) | ((hdata & 0x0000ff00) << 8) | ((hdata & 0x00ff0000) >> 8) | ((hdata & 0xff000000) >> 24);
    ldata = ((ldata & 0x000000ff) << 24) | ((ldata & 0x0000ff00) << 8) | ((ldata & 0x00ff0000) >> 8) | ((ldata & 0xff000000) >> 24);

    temp64 = ldata & 0xffffffff;

    return ((temp64 << 32) + (hdata & 0xffffffff));
}

void Boot_Process(void)
{
    if(Boot_flag==1)
    {
        EEPROM_WriteOneByte(Boot_Flag_addr-1,0xFF);
        EEPROM_WriteOneByte(Boot_Flag_addr,0xFF);
        __set_PRIMASK(1);//关中断
        __set_FAULTMASK(1);//关闭中断异常
        NVIC_SystemReset();
    }
    // else
    // {
    //     if((Boot_Flag==0x50A0)||(Boot_Flag==0xFFFF))
    //     {
    //         EEPROM_WriteOneByte(Boot_Flag_addr-1,0xAA);
    //         EEPROM_WriteOneByte(Boot_Flag_addr,0x55);
    //     }
    // }
}