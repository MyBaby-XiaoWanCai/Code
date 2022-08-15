#include "main.h"
#include "step_work.h"
#include "data_analysis.h"
#include <string.h>
#include "discharge.h"


RunningStep running_step[CHANNELS];
RunningCircle running_circle[CHANNELS][3];
StepRecordCondition last_record_condition[CHANNELS];
StepRecordCondition this_record_condition[CHANNELS];

RecordData record_data;

SampleData filter_data_old[8] = {   [0].v_sense = 0.0, [1].v_sense = 0.0, [2].v_sense = 0.0, [3].v_sense = 0.0, [4].v_sense = 0.0, [5].v_sense = 0.0, [6].v_sense = 0.0, [7].v_sense = 0.0,
                                    [0].i_out = 0.0, [1].i_out = 0.0, [2].i_out = 0.0, [3].i_out = 0.0, [4].i_out = 0.0, [5].i_out = 0.0, [6].i_out = 0.0, [7].i_out = 0.0,
                                    [0].v_mod = 0.0, [1].v_mod = 0.0, [2].v_mod = 0.0, [3].v_mod = 0.0, [4].v_mod = 0.0, [5].v_mod = 0.0, [6].v_mod = 0.0, [7].v_mod = 0.0,
                                    [0].v_j = 0.0,   [1].v_j = 0.0,   [2].v_j = 0.0,   [3].v_j = 0.0,   [4].v_j = 0.0,   [5].v_j = 0.0,   [6].v_j = 0.0,   [7].v_j = 0.0,
                                    [0].battery_temperature = 0.0, [1].battery_temperature = 0.0, [2].battery_temperature = 0.0, [3].battery_temperature = 0.0,
                                    [4].battery_temperature = 0.0, [5].battery_temperature = 0.0, [6].battery_temperature = 0.0, [7].battery_temperature = 0.0,
                                    [0].mod_tempetature = 0.0,     [1].mod_tempetature = 0.0,     [2].mod_tempetature = 0.0,     [3].mod_tempetature = 0.0,
                                    [4].mod_tempetature = 0.0,     [5].mod_tempetature = 0.0,     [6].mod_tempetature = 0.0,     [7].mod_tempetature = 0.0, };

SampleData filter_data_now[8] = {   [0].v_sense = 0.0, [1].v_sense = 0.0, [2].v_sense = 0.0, [3].v_sense = 0.0, [4].v_sense = 0.0, [5].v_sense = 0.0, [6].v_sense = 0.0, [7].v_sense = 0.0,
                                    [0].i_out = 0.0, [1].i_out = 0.0, [2].i_out = 0.0, [3].i_out = 0.0, [4].i_out = 0.0, [5].i_out = 0.0, [6].i_out = 0.0, [7].i_out = 0.0,
                                    [0].v_mod = 0.0, [1].v_mod = 0.0, [2].v_mod = 0.0, [3].v_mod = 0.0, [4].v_mod = 0.0, [5].v_mod = 0.0, [6].v_mod = 0.0, [7].v_mod = 0.0,
                                    [0].v_j = 0.0,   [1].v_j = 0.0,   [2].v_j = 0.0,   [3].v_j = 0.0,   [4].v_j = 0.0,   [5].v_j = 0.0,   [6].v_j = 0.0,   [7].v_j = 0.0,
                                    [0].battery_temperature = 0.0, [1].battery_temperature = 0.0, [2].battery_temperature = 0.0, [3].battery_temperature = 0.0,
                                    [4].battery_temperature = 0.0, [5].battery_temperature = 0.0, [6].battery_temperature = 0.0, [7].battery_temperature = 0.0,
                                    [0].mod_tempetature = 0.0,     [1].mod_tempetature = 0.0,     [2].mod_tempetature = 0.0,     [3].mod_tempetature = 0.0,
                                    [4].mod_tempetature = 0.0,     [5].mod_tempetature = 0.0,     [6].mod_tempetature = 0.0,     [7].mod_tempetature = 0.0, };
AdSample ad_sample[8] = {   [0].v_sense = 0, [0].i_out = 0, [0].v_j = 0,
                            [1].v_sense = 0, [1].i_out = 0, [1].v_j = 0,
                            [2].v_sense = 0, [2].i_out = 0, [2].v_j = 0,
                            [3].v_sense = 0, [3].i_out = 0, [3].v_j = 0,
                            [4].v_sense = 0, [4].i_out = 0, [4].v_j = 0,
                            [5].v_sense = 0, [5].i_out = 0, [5].v_j = 0,
                            [6].v_sense = 0, [6].i_out = 0, [6].v_j = 0,
                            [7].v_sense = 0, [7].i_out = 0, [7].v_j = 0,};

SpecialSample special_sample = {0, 0, 0, 0};

RecordQuery record_query_cache[CACHE_COUNT][CHANNELS];

void Step_Data_Read(uint8_t ch, uint16_t number)
{
    uint8_t data_ch;
    uint32_t data_address;
    uint16_t read_len = 0;
    uint8_t temp8;
    uint16_t temp16;
    uint32_t temp32;
    float ftemp;
    uint8_t temp_data[257] = {0x00};

#if CHANNELS_1
    if(ch & CH_1) {
        data_ch = 0;
    } else {

    }
#endif
#if CHANNELS_2
    if(ch & CH_1) {
        data_ch = 0;
    } else if(ch & CH_2) {
        data_ch = 1;
    } else {

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
        //清除上一工步的值----------------------------------------------------------------------------
        step_work[data_ch].step_number = 0;
        step_work[data_ch].step_type = 0;
        step_work[data_ch].step_work_voltage = 0;
        step_work[data_ch].step_work_current = Ich_MAX;
        step_work[data_ch].step_work_power = 0;
        step_work[data_ch].step_abort_priority = step_time;
        step_work[data_ch].step_abort_time = 0;
        step_work[data_ch].step_abort_voltage = 0;
        step_work[data_ch].step_abort_current = 0;
        step_work[data_ch].step_abort_capacitance = 0;

        step_record_condition[data_ch].record_time = 0;
        step_record_condition[data_ch].record_voltage = 0;
        step_record_condition[data_ch].record_current = 0;

        step_protection[data_ch].protection_OverTemperature_alarm = 0;
        step_protection[data_ch].protection_charge_capacitance = 0;
        step_protection[data_ch].protection_discharge_capacitance = 0;
        step_protection[data_ch].protection_voltage_high_limit = 0;
        step_protection[data_ch].protection_voltage_low_limit = 0;
        step_protection[data_ch].protection_charge_current_limit = 0;
        step_protection[data_ch].protection_discharge_current_limit = 0;
        step_protection[data_ch].protection_current_positive_jump_range = 0;
        step_protection[data_ch].protection_current_positive_jump_number = 0;
        step_protection[data_ch].protection_current_negative_jump_range = 0;
        step_protection[data_ch].protection_current_negative_jump_number = 0;
        step_protection[data_ch].protection_voltage_positive_jump_range = 0;
        step_protection[data_ch].protection_voltage_positive_jump_number = 0;
        step_protection[data_ch].protection_voltage_negative_jump_range = 0;
        step_protection[data_ch].protection_voltage_negative_jump_number = 0;
        step_protection[data_ch].protection_current_OutOfTolerance = 0;
        step_protection[data_ch].protection_voltage_OutOfTolerance = 0;
        step_protection[data_ch].protection_UnderTemperature_alarm = 0;
        step_protection[data_ch].protection_timing_voltage_detection_time = 0;
        step_protection[data_ch].protection_timing_voltage_high = 0;
        step_protection[data_ch].protection_timing_voltage_low = 0;
        step_protection[data_ch].protection_timing_voltage_change_detection_time1 = 0;
        step_protection[data_ch].protection_timing_voltage_change_detection_time2 = 0;
        step_protection[data_ch].protection_timing_voltage_change = 0;
        step_protection[data_ch].protection_battery_short_detection_time = 0;
        step_protection[data_ch].protection_battery_short_voltage_high = 0;
        step_protection[data_ch].protection_battery_short_voltage_low = 0;
        //------------------------------------------------------------------------------------------

        step_work[data_ch].step_number = number;
        step_work[data_ch].step_type = temp_data[2];
        read_len += 3;

        if(step_work[data_ch].step_type == end_step) {

        }
        // else if(step_work[data_ch].step_type == loop_begin) {
        //     memcpy(&running_circle[data_ch].circle_serial, temp_data+read_len, 2);      read_len += 2;
        //     memcpy(&running_circle[data_ch].circle_number, temp_data+read_len, 2);      read_len += 2;
        // }
        else if(step_work[data_ch].step_type == loop_end) {
            uint16_t cir_serial = 0;
            uint16_t cir_number = 0;
            uint16_t cir_start  = 0;

            memcpy(&temp16, temp_data+read_len, 2);         read_len += 2;
            cir_serial = temp16;        //循环序号
            memcpy(&temp16, temp_data+read_len, 2);         read_len += 2;
            cir_number = temp16;        //循环次数
            memcpy(&temp16, temp_data+read_len, 2);         read_len += 2;
            cir_start  = temp16;        //循环开始工步号

            if(running_circle[data_ch][0].circle_flag == 0x00) {

                running_circle[data_ch][0].circle_serial = cir_serial;
                running_circle[data_ch][0].circle_number = cir_number;
                running_circle[data_ch][0].circle_start  = cir_start;

                running_circle[data_ch][0].circle_flag = 0x01;
            } else if(running_circle[data_ch][1].circle_flag == 0x00) {
                if(cir_serial != running_circle[data_ch][0].circle_serial) {
                    if(cir_start > running_circle[data_ch][0].circle_start) {
                        running_circle[data_ch][1].circle_serial = cir_serial;
                        running_circle[data_ch][1].circle_number = cir_number;
                        running_circle[data_ch][1].circle_start  = cir_start;

                        running_circle[data_ch][1].circle_flag = 0x01;
                    } else {
                        running_circle[data_ch][0].circle_error = 0x01;
                    }

                }
            } else if(running_circle[data_ch][2].circle_flag == 0x00) {
                if(cir_serial != running_circle[data_ch][1].circle_serial) {
                    if(cir_start > running_circle[data_ch][1].circle_start) {
                        running_circle[data_ch][2].circle_serial = cir_serial;
                        running_circle[data_ch][2].circle_number = cir_number;
                        running_circle[data_ch][2].circle_start  = cir_start;

                        running_circle[data_ch][2].circle_flag = 0x01;
                    } else {
                        running_circle[data_ch][1].circle_error = 0x01;
                    }
                }
            } else {
                if(cir_serial != running_circle[data_ch][2].circle_serial) {
                    running_circle[data_ch][2].circle_error = 0x01;
                }
            }
        }
        else {
            //step_work-----------------------------------------------------------------------------
            for(int i = 0; i < 3; i++) {
                temp8 = temp_data[read_len];
                memcpy(&ftemp, temp_data+read_len+1, 4);                        read_len += 5;
                memcpy(&temp32, &ftemp, 4);
                if(temp32 == 0xffffffff) {
                    ftemp = 0.0;
                }

                switch(temp8) {
                    case step_voltage:
                    {
                        step_work[data_ch].step_work_voltage = ftemp;
                        break;
                    }
                    case step_current:
                    {
                        step_work[data_ch].step_work_current = ftemp;
                        break;
                    }
                    case step_power: {
                        step_work[data_ch].step_work_power = ftemp;
                        break;
                    }
                    default:
                        break;
                }
            }
            //--------------------------------------------------------------------------------------

            //step_abort----------------------------------------------------------------------------
            for(int i = 0; i < 4; i++) {
                temp8 = temp_data[read_len];
                memcpy(&ftemp, temp_data+read_len+1, 4);                        read_len += 5;
                memcpy(&temp32, &ftemp, 4);
                if(temp32 == 0xffffffff) {
                    ftemp = 0.0;
                }

                if(i == 0) {
                    step_work[data_ch].step_abort_priority = temp8;
                }

                switch(temp8) {
                    case step_time: {
                        step_work[data_ch].step_abort_time = ftemp;
                        break;
                    }
                    case step_voltage: {
                        step_work[data_ch].step_abort_voltage = ftemp;
                        break;
                    }
                    case step_current: {
                        step_work[data_ch].step_abort_current = ftemp;
                        break;
                    }
                    case step_capacitance: {
                        step_work[data_ch].step_abort_capacitance = ftemp;
                        break;
                    }
                    default:
                        break;
                }
            }
            //--------------------------------------------------------------------------------------

            //step_record_condition-----------------------------------------------------------------
            for(int i = 0; i < 3; i++) {
                temp8 = temp_data[read_len];
                memcpy(&ftemp, temp_data+read_len+1, 4);                        read_len += 5;
                memcpy(&temp32, &ftemp, 4);
                if(temp32 == 0xffffffff) {
                    ftemp = 0.0;
                }

                switch(temp8) {
                    case step_record_time: {
                        step_record_condition[data_ch].record_time = ftemp;
                        break;
                    }
                    case step_record_voltage:
                    {
                        step_record_condition[data_ch].record_voltage = ftemp;
                        break;
                    }
                    case step_record_current:
                    {
                        step_record_condition[data_ch].record_current = ftemp;
                        break;
                    }
                    default:
                        break;
                }
            }
            //--------------------------------------------------------------------------------------

            //step_protection-----------------------------------------------------------------------
            for(int i = 0; i < (step_protection_parament_end-1); i++) {
                temp8 = temp_data[read_len];
                memcpy(&ftemp, temp_data+read_len+1, 4);                        read_len += 5;
                memcpy(&temp32, &ftemp, 4);
                if(temp32 == 0xffffffff) {
                    ftemp = 0.0;
                }

                switch(temp8) {
                    case step_OverTemperature_alarm: {
                        step_protection[data_ch].protection_OverTemperature_alarm = ftemp;
                        break;
                    }
                    case step_charge_capacitance: {
                        step_protection[data_ch].protection_charge_capacitance = ftemp;
                        break;
                    }
                    case step_discharge_capacitance: {
                        step_protection[data_ch].protection_discharge_capacitance = ftemp;
                        break;
                    }
                    case step_voltage_high_limit: {
                        step_protection[data_ch].protection_voltage_high_limit = ftemp;
                        break;
                    }
                    case step_voltage_low_limit: {
                        step_protection[data_ch].protection_voltage_low_limit = ftemp;
                        break;
                    }
                    case step_charge_current_limit: {
                        step_protection[data_ch].protection_charge_current_limit = ftemp;
                        break;
                    }
                    case step_discharge_current_limit: {
                        step_protection[data_ch].protection_discharge_current_limit = ftemp;
                        break;
                    }
                    case step_current_positive_jump_range: {
                        step_protection[data_ch].protection_current_positive_jump_range = ftemp;
                        break;
                    }
                    case step_current_positive_jump_number: {
                        step_protection[data_ch].protection_current_positive_jump_number = (uint32_t)ftemp;
                        break;
                    }
                    case step_current_negative_jump_range: {
                        step_protection[data_ch].protection_current_negative_jump_range = ftemp;
                        break;
                    }
                    case step_current_negative_jump_number: {
                        step_protection[data_ch].protection_current_negative_jump_number = (uint32_t)ftemp;
                        break;
                    }
                    case step_voltage_positive_jump_range: {
                        step_protection[data_ch].protection_voltage_positive_jump_range = ftemp;
                        break;
                    }
                    case step_voltage_positive_jump_number: {
                        step_protection[data_ch].protection_voltage_positive_jump_number = (uint32_t)ftemp;
                        break;
                    }
                    case step_voltage_negative_jump_range: {
                        step_protection[data_ch].protection_voltage_negative_jump_range = ftemp;
                        break;
                    }
                    case step_voltage_negative_jump_number: {
                        step_protection[data_ch].protection_voltage_negative_jump_number = (uint32_t)ftemp;
                        break;
                    }
                    case step_current_OutOfTolerance: {
                        step_protection[data_ch].protection_current_OutOfTolerance = ftemp;
                        break;
                    }
                    case step_voltage_OutOfTolerance: {
                        step_protection[data_ch].protection_voltage_OutOfTolerance = ftemp;
                        break;
                    }
                    case step_UnderTemperature_alarm: {
                        step_protection[data_ch].protection_UnderTemperature_alarm = ftemp;
                        break;
                    }
                    case step_timing_voltage_detection_time: {
                        step_protection[data_ch].protection_timing_voltage_detection_time = ftemp;
                        break;
                    }
                    case step_timing_voltage_high: {
                        step_protection[data_ch].protection_timing_voltage_high = ftemp;
                        break;
                    }
                    case step_timing_voltage_low: {
                        step_protection[data_ch].protection_timing_voltage_low = ftemp;
                        break;
                    }
                    case step_timing_voltage_change_detection_time1: {
                        step_protection[data_ch].protection_timing_voltage_change_detection_time1 = ftemp;
                        break;
                    }
                    case step_timing_voltage_change_detection_time2: {
                        step_protection[data_ch].protection_timing_voltage_change_detection_time2 = ftemp;
                        break;
                    }
                    case step_timing_voltage_change: {
                        step_protection[data_ch].protection_timing_voltage_change = ftemp;
                        break;
                    }
                    case step_battery_short_detection_time: {
                        step_protection[data_ch].protection_battery_short_detection_time = ftemp;
                        break;
                    }
                    case step_battery_short_voltage_high: {
                        step_protection[data_ch].protection_battery_short_voltage_high = ftemp;
                        break;
                    }
                    case step_battery_short_voltage_low: {
                        step_protection[data_ch].protection_battery_short_voltage_low = ftemp;
                        break;
                    }
                    default:
                        break;
                }
            }
            //--------------------------------------------------------------------------------------

        }
    } else {
        step_work[data_ch].step_number = 0;
        step_work[data_ch].step_type = 0;
    }

    // if((step_work[data_ch].step_number != 0x0000) && (step_work[data_ch].step_number != 0xFFFF)) {
    //     if(step_work[data_ch].step_type == loop_begin) {
    //         memcpy(&running_circle[data_ch].circle_serial, temp_data+read_len, 2);      read_len += 2;
    //         memcpy(&running_circle[data_ch].circle_number, temp_data+read_len, 2);      read_len += 2;
    //     } else if(step_work[data_ch].step_type == loop_end) {
    //         memcpy(&temp16, temp_data+read_len, 2);         read_len += 2;
    //         if(temp16 == running_circle[data_ch].circle_serial) {
    //             running_circle[data_ch].circle_end_flag = 0x01;
    //         }
    //     } else if(step_work[data_ch].step_type == end_step) {

    //     } else {
    //         memcpy(&(step_work[data_ch].step_work_current), temp_data+read_len, 4);             read_len += 4;
    //         memcpy(&(step_work[data_ch].step_work_voltage), temp_data+read_len, 4);             read_len += 4;
    //         memcpy(&(step_work[data_ch].step_work_power), temp_data+read_len, 4);               read_len += 4;
    //         memcpy(&(step_work[data_ch].step_abort_current), temp_data+read_len, 4);            read_len += 4;
    //         memcpy(&(step_work[data_ch].step_abort_voltage), temp_data+read_len, 4);            read_len += 4;
    //         memcpy(&(step_work[data_ch].step_abort_time), temp_data+read_len, 4);               read_len += 4;
    //         memcpy(&(step_work[data_ch].step_abort_capacitance), temp_data+read_len, 4);        read_len += 4;

    //         memcpy(&(step_record_condition[data_ch].record_current), temp_data+read_len, 4);              read_len += 4;
    //         memcpy(&(step_record_condition[data_ch].record_voltage), temp_data+read_len, 4);              read_len += 4;
    //         memcpy(&(step_record_condition[data_ch].record_time), temp_data+read_len, 4);                 read_len += 4;

    //         memcpy(&(step_protection[data_ch].protection_OverTemperature_alarm), temp_data+read_len, 4);        read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_charge_capacitance), temp_data+read_len, 4);               read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_discharge_capacitance), temp_data+read_len, 4);            read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_voltage_high_limit), temp_data+read_len, 4);               read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_voltage_low_limit), temp_data+read_len, 4);                read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_charge_current_limit), temp_data+read_len, 4);             read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_discharge_current_limit), temp_data+read_len, 4);          read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_current_positive_jump_range), temp_data+read_len, 4);      read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_current_positive_jump_number), temp_data+read_len, 4);     read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_current_negative_jump_range), temp_data+read_len, 4);      read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_current_negative_jump_number), temp_data+read_len, 4);     read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_voltage_positive_jump_range), temp_data+read_len, 4);      read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_voltage_positive_jump_number), temp_data+read_len, 4);     read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_voltage_negative_jump_range), temp_data+read_len, 4);      read_len += 4;
    //         memcpy(&(step_protection[data_ch].protection_voltage_negative_jump_number), temp_data+read_len, 4);     read_len += 4;

    //     }
    // }


}


void Step_Run_Execute(uint8_t ch)
{
    uint16_t V_number = 0, I_number = 0;;

#if CHANNELS_1
    if(ch & CH_1) {
        if(step_work[0].step_type == standby) {             //静置
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(0, 0, 0);
            // DAC80508_SetDacData(0, 0, 1);
            // DAC80508_SetDacData(1, 0, 0);
            // DAC80508_SetDacData(1, 0, 1);
            // DAC80508_SetDacData(2, 0, 0);
            // DAC80508_SetDacData(2, 0, 1);
            // DAC80508_SetDacData(3, 0, 0);
            // DAC80508_SetDacData(3, 0, 1);
            // DAC80508_SetDacData(4, 0, 0);
            // DAC80508_SetDacData(4, 0, 1);
            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);
            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);
            // DAC80508_SetDacData(7, 0, 0);
            // DAC80508_SetDacData(7, 0, 1);
        } else if(step_work[0].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, V_MAX, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, V_MAX, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH2, BUCK_MODE, V_MAX, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH3, BUCK_MODE, V_MAX, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH4, BUCK_MODE, V_MAX, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH5, BUCK_MODE, V_MAX, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH6, BUCK_MODE, V_MAX, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH7, BUCK_MODE, V_MAX, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH8, BUCK_MODE, V_MAX, step_work[0].step_work_current/8);

                REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
                REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
                PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
            }

        } else if(step_work[0].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH2, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH3, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH4, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH5, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH6, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH7, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH8, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);

            REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
            REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
            HAL_Delay(20);
            PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
            PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
        } else if(step_work[0].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH2, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH3, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH4, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH5, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH6, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH7, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH8, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);

                REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
                REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
                PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
            }

        } else if(step_work[0].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[0].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, V_MIN, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, V_MIN, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH2, BOOST_MODE, V_MIN, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH3, BOOST_MODE, V_MIN, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH4, BOOST_MODE, V_MIN, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH5, BOOST_MODE, V_MIN, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH6, BOOST_MODE, V_MIN, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH7, BOOST_MODE, V_MIN, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH8, BOOST_MODE, V_MIN, step_work[0].step_work_current/8);

                REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
                REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
                PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
            }

        } else if(step_work[0].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH2, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH3, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH4, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH5, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH6, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH7, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH8, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);

            REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
            REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
            HAL_Delay(20);
            PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
            PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
        } else if(step_work[0].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH2, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH3, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH4, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH5, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH6, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH7, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);
                Discharge_Control(DAC80508_CH8, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/8);


                REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
                REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
                PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
            }

        } else if(step_work[0].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[0].step_type == loop_begin) {   //循环模式开始
        //     running_circle[0].circle_start = step_work[0].step_number;
        //     Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //     Step_Run_Execute(CH_1);
        //     running_circle[0].circle_count = 1;
        } else if(step_work[0].step_type == loop_end) {     //循环模式结束
            if(running_circle[0][2].circle_flag == 0x01) {
                if(running_circle[0][2].circle_error == 0x00) {
                    running_circle[0][2].circle_count += 1;
                    if(running_circle[0][2].circle_count < running_circle[0][2].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[0][1].circle_flag == 0x01) {
                if(running_circle[0][1].circle_error == 0x00) {
                    running_circle[0][1].circle_count += 1;
                    if(running_circle[0][1].circle_count < running_circle[0][1].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[0][0].circle_flag == 0x01) {
                if(running_circle[0][0].circle_error == 0x00) {
                    running_circle[0][0].circle_count += 1;
                    if(running_circle[0][0].circle_count < running_circle[0][0].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_1, global_protection_flag, step_stop_status);
                Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[0].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(0, 0, 0);
            // DAC80508_SetDacData(0, 0, 1);
            // DAC80508_SetDacData(1, 0, 0);
            // DAC80508_SetDacData(1, 0, 1);
            // DAC80508_SetDacData(2, 0, 0);
            // DAC80508_SetDacData(2, 0, 1);
            // DAC80508_SetDacData(3, 0, 0);
            // DAC80508_SetDacData(3, 0, 1);
            // DAC80508_SetDacData(4, 0, 0);
            // DAC80508_SetDacData(4, 0, 1);
            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);
            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);
            // DAC80508_SetDacData(7, 0, 0);
            // DAC80508_SetDacData(7, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(0, 0, 0);
            // DAC80508_SetDacData(0, 0, 1);
            // DAC80508_SetDacData(1, 0, 0);
            // DAC80508_SetDacData(1, 0, 1);
            // DAC80508_SetDacData(2, 0, 0);
            // DAC80508_SetDacData(2, 0, 1);
            // DAC80508_SetDacData(3, 0, 0);
            // DAC80508_SetDacData(3, 0, 1);
            // DAC80508_SetDacData(4, 0, 0);
            // DAC80508_SetDacData(4, 0, 1);
            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);
            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);
            // DAC80508_SetDacData(7, 0, 0);
            // DAC80508_SetDacData(7, 0, 1);
        }
    }
#endif

#if CHANNELS_2
    if(ch & CH_1) {
        if(step_work[0].step_type == standby) {             //静置
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(0, 0, 0);
            // DAC80508_SetDacData(0, 0, 1);
            // DAC80508_SetDacData(1, 0, 0);
            // DAC80508_SetDacData(1, 0, 1);
            // DAC80508_SetDacData(2, 0, 0);
            // DAC80508_SetDacData(2, 0, 1);
            // DAC80508_SetDacData(3, 0, 0);
            // DAC80508_SetDacData(3, 0, 1);
        } else if(step_work[0].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, V_MAX, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, V_MAX, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH2, BUCK_MODE, V_MAX, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH3, BUCK_MODE, V_MAX, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH4, BUCK_MODE, V_MAX, step_work[0].step_work_current/4);

                REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
            }


        } else if(step_work[0].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH2, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH3, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH4, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);

            REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
            HAL_Delay(20);
            PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
        } else if(step_work[0].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH2, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH3, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH4, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/4);

                REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
            }

        } else if(step_work[0].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[0].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, V_MIN, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, V_MIN, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH2, BOOST_MODE, V_MIN, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH3, BOOST_MODE, V_MIN, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH4, BOOST_MODE, V_MIN, step_work[0].step_work_current/4);

                REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
            }

        } else if(step_work[0].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH2, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH3, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH4, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);

            REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
            HAL_Delay(20);
            PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
        } else if(step_work[0].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH2, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH3, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/4);
                Discharge_Control(DAC80508_CH4, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/4);

                REV_PORT1_EN;REV_PORT2_EN;REV_PORT3_EN;REV_PORT4_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;PORT3_EN;PORT4_EN;
            }

        } else if(step_work[0].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[0].step_type == loop_begin) {   //循环模式开始
        //     running_circle[0].circle_start = step_work[0].step_number;
        //     Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //     Step_Run_Execute(CH_1);
        //     running_circle[0].circle_count = 1;
        // }
        else if(step_work[0].step_type == loop_end) {     //循环模式结束
            if(running_circle[0][2].circle_flag == 0x01) {
                if(running_circle[0][2].circle_error == 0x00) {
                    running_circle[0][2].circle_count += 1;
                    if(running_circle[0][2].circle_count < running_circle[0][2].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[0][1].circle_flag == 0x01) {
                if(running_circle[0][1].circle_error == 0x00) {
                    running_circle[0][1].circle_count += 1;
                    if(running_circle[0][1].circle_count < running_circle[0][1].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[0][0].circle_flag == 0x01) {
                if(running_circle[0][0].circle_error == 0x00) {
                    running_circle[0][0].circle_count += 1;
                    if(running_circle[0][0].circle_count < running_circle[0][0].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_1, global_protection_flag, step_stop_status);
                Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[0].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(0, 0, 0);
            // DAC80508_SetDacData(0, 0, 1);
            // DAC80508_SetDacData(1, 0, 0);
            // DAC80508_SetDacData(1, 0, 1);
            // DAC80508_SetDacData(2, 0, 0);
            // DAC80508_SetDacData(2, 0, 1);
            // DAC80508_SetDacData(3, 0, 0);
            // DAC80508_SetDacData(3, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(0, 0, 0);
            // DAC80508_SetDacData(0, 0, 1);
            // DAC80508_SetDacData(1, 0, 0);
            // DAC80508_SetDacData(1, 0, 1);
            // DAC80508_SetDacData(2, 0, 0);
            // DAC80508_SetDacData(2, 0, 1);
            // DAC80508_SetDacData(3, 0, 0);
            // DAC80508_SetDacData(3, 0, 1);
        }
    } else if(ch & CH_2) {
        if(step_work[1].step_type == standby) {             //静置
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(4, 0, 0);
            // DAC80508_SetDacData(4, 0, 1);
            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);
            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);
            // DAC80508_SetDacData(7, 0, 0);
            // DAC80508_SetDacData(7, 0, 1);
        } else if(step_work[1].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[1].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH5, BUCK_MODE, V_MAX, step_work[1].step_work_current);
                REV_PORT5_EN;
                HAL_Delay(20);
                PORT5_EN;
            } else {
                Discharge_Control(DAC80508_CH5, BUCK_MODE, V_MAX, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH6, BUCK_MODE, V_MAX, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH7, BUCK_MODE, V_MAX, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH8, BUCK_MODE, V_MAX, step_work[1].step_work_current/4);

                REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
            }

        } else if(step_work[1].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BUCK_MODE, step_work[1].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH6, BUCK_MODE, step_work[1].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH7, BUCK_MODE, step_work[1].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH8, BUCK_MODE, step_work[1].step_work_voltage, Iport_MAX);


            REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
            HAL_Delay(20);
            PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
        } else if(step_work[1].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[1].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH5, BUCK_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current);
                REV_PORT5_EN;
                HAL_Delay(20);
                PORT5_EN;
            } else {
                Discharge_Control(DAC80508_CH5, BUCK_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH6, BUCK_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH7, BUCK_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH8, BUCK_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/4);

                REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
            }

        } else if(step_work[1].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[1].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[1].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH5, BOOST_MODE, V_MIN, step_work[1].step_work_current);
                REV_PORT5_EN;
                HAL_Delay(20);
                PORT5_EN;
            } else {
                Discharge_Control(DAC80508_CH5, BOOST_MODE, V_MIN, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH6, BOOST_MODE, V_MIN, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH7, BOOST_MODE, V_MIN, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH8, BOOST_MODE, V_MIN, step_work[1].step_work_current/4);

                REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
            }

        } else if(step_work[1].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BOOST_MODE, step_work[1].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH6, BOOST_MODE, step_work[1].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH7, BOOST_MODE, step_work[1].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH8, BOOST_MODE, step_work[1].step_work_voltage, Iport_MAX);

            REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
            HAL_Delay(20);
            PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
        } else if(step_work[1].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[1].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH5, BOOST_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current);
                REV_PORT5_EN;
                HAL_Delay(20);
                PORT5_EN;
            } else {
                Discharge_Control(DAC80508_CH5, BOOST_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH6, BOOST_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH7, BOOST_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/4);
                Discharge_Control(DAC80508_CH8, BOOST_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/4);

                REV_PORT5_EN;REV_PORT6_EN;REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT5_EN;PORT6_EN;PORT7_EN;PORT8_EN;
            }

        } else if(step_work[1].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[1].step_type == loop_begin) {   //循环模式开始
        //     running_circle[1].circle_start = step_work[1].step_number;
        //     running_circle[1].circle_count = 1;
        //     Step_Data_Read(CH_2, running_circle[1].circle_start+1);
        //     Step_Run_Execute(CH_2);
        // }
        else if(step_work[1].step_type == loop_end) {     //循环模式结束
            if(running_circle[1][2].circle_flag == 0x01) {
                if(running_circle[1][2].circle_error == 0x00) {
                    running_circle[1][2].circle_count += 1;
                    if(running_circle[1][2].circle_count < running_circle[1][2].circle_number) {
                        Step_Run_Switch(CH_2, running_circle[1][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_2, step_work[1].step_number);
                    }
                } else {
                    Protection_Action(CH_2, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[1][1].circle_flag == 0x01) {
                if(running_circle[1][1].circle_error == 0x00) {
                    running_circle[1][1].circle_count += 1;
                    if(running_circle[1][1].circle_count < running_circle[1][1].circle_number) {
                        Step_Run_Switch(CH_2, running_circle[1][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_2, step_work[1].step_number);
                    }
                } else {
                    Protection_Action(CH_2, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[1][0].circle_flag == 0x01) {
                if(running_circle[1][0].circle_error == 0x00) {
                    running_circle[1][0].circle_count += 1;
                    if(running_circle[1][0].circle_count < running_circle[1][0].circle_number) {
                        Step_Run_Switch(CH_2, running_circle[1][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_2, step_work[1].step_number);
                    }
                } else {
                    Protection_Action(CH_2, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_2, global_protection_flag, step_stop_status);
                Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[1].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(4, 0, 0);
            // DAC80508_SetDacData(4, 0, 1);
            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);
            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);
            // DAC80508_SetDacData(7, 0, 0);
            // DAC80508_SetDacData(7, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(4, 0, 0);
            // DAC80508_SetDacData(4, 0, 1);
            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);
            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);
            // DAC80508_SetDacData(7, 0, 0);
            // DAC80508_SetDacData(7, 0, 1);
        }
    }
#endif

#if CHANNELS_4
    if(ch & CH_1) {
        if(step_work[0].step_type == standby) {             //静置
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(0, 0, 0);
            // DAC80508_SetDacData(0, 0, 1);
            // DAC80508_SetDacData(1, 0, 0);
            // DAC80508_SetDacData(1, 0, 1);
        } else if(step_work[0].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, V_MAX, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, V_MAX, step_work[0].step_work_current/2);
                Discharge_Control(DAC80508_CH2, BUCK_MODE, V_MAX, step_work[0].step_work_current/2);

                REV_PORT1_EN;REV_PORT2_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;
            }

        } else if(step_work[0].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH2, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);

            REV_PORT1_EN;REV_PORT2_EN;
            HAL_Delay(20);
            PORT1_EN;PORT2_EN;
        } else if(step_work[0].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/2);
                Discharge_Control(DAC80508_CH2, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/2);

                REV_PORT1_EN;REV_PORT2_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;
            }

        } else if(step_work[0].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[0].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, V_MIN, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, V_MIN, step_work[0].step_work_current/2);
                Discharge_Control(DAC80508_CH2, BOOST_MODE, V_MIN, step_work[0].step_work_current/2);

                REV_PORT1_EN;REV_PORT2_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;
            }

        } else if(step_work[0].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH2, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);

            REV_PORT1_EN;REV_PORT2_EN;
            HAL_Delay(20);
            PORT1_EN;PORT2_EN;
        } else if(step_work[0].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            if(step_work[0].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current);
                REV_PORT1_EN;
                HAL_Delay(20);
                PORT1_EN;
            } else {
                Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/2);
                Discharge_Control(DAC80508_CH2, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/2);

                REV_PORT1_EN;REV_PORT2_EN;
                HAL_Delay(20);
                PORT1_EN;PORT2_EN;
            }

        } else if(step_work[0].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[0].step_type == loop_begin) {     //循环模式开始
        //     running_circle[0].circle_start = step_work[0].step_number;
        //     Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //     Step_Run_Execute(CH_1);
        //     running_circle[0].circle_count = 1;
        // }
        else if(step_work[0].step_type == loop_end) {     //循环模式结束
            if(running_circle[0][2].circle_flag == 0x01) {
                if(running_circle[0][2].circle_error == 0x00) {
                    running_circle[0][2].circle_count += 1;
                    if(running_circle[0][2].circle_count < running_circle[0][2].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[0][1].circle_flag == 0x01) {
                if(running_circle[0][1].circle_error == 0x00) {
                    running_circle[0][1].circle_count += 1;
                    if(running_circle[0][1].circle_count < running_circle[0][1].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[0][0].circle_flag == 0x01) {
                if(running_circle[0][0].circle_error == 0x00) {
                    running_circle[0][0].circle_count += 1;
                    if(running_circle[0][0].circle_count < running_circle[0][0].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_1, global_protection_flag, step_stop_status);
                Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[0].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(0, 0, 0);
            // DAC80508_SetDacData(0, 0, 1);
            // DAC80508_SetDacData(1, 0, 0);
            // DAC80508_SetDacData(1, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(0, 0, 0);
            // DAC80508_SetDacData(0, 0, 1);
            // DAC80508_SetDacData(1, 0, 0);
            // DAC80508_SetDacData(1, 0, 1);
        }
    } else if(ch & CH_2) {
        if(step_work[1].step_type == standby) {             //静置
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(2, 0, 0);
            // DAC80508_SetDacData(2, 0, 1);
            // DAC80508_SetDacData(3, 0, 0);
            // DAC80508_SetDacData(3, 0, 1);
        } else if(step_work[1].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            if(step_work[1].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH3, BUCK_MODE, V_MAX, step_work[1].step_work_current);
                REV_PORT3_EN;
                HAL_Delay(20);
                PORT3_EN;
            } else {
                Discharge_Control(DAC80508_CH3, BUCK_MODE, V_MAX, step_work[1].step_work_current/2);
                Discharge_Control(DAC80508_CH4, BUCK_MODE, V_MAX, step_work[1].step_work_current/2);

                REV_PORT3_EN;REV_PORT4_EN;
                HAL_Delay(20);
                PORT3_EN;PORT4_EN;
            }

        } else if(step_work[1].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH3, BUCK_MODE, step_work[1].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH4, BUCK_MODE, step_work[1].step_work_voltage, Iport_MAX);

            REV_PORT3_EN;REV_PORT4_EN;
            HAL_Delay(20);
            PORT3_EN;PORT4_EN;
        } else if(step_work[1].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            if(step_work[1].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH3, BUCK_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current);
                REV_PORT3_EN;
                HAL_Delay(20);
                PORT3_EN;
            } else {
                Discharge_Control(DAC80508_CH3, BUCK_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/2);
                Discharge_Control(DAC80508_CH4, BUCK_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/2);

                REV_PORT3_EN;REV_PORT4_EN;
                HAL_Delay(20);
                PORT3_EN;PORT4_EN;
            }

        } else if(step_work[1].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[1].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            if(step_work[1].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH3, BOOST_MODE, V_MIN, step_work[1].step_work_current);
                REV_PORT3_EN;
                HAL_Delay(20);
                PORT3_EN;
            } else {
                Discharge_Control(DAC80508_CH3, BOOST_MODE, V_MIN, step_work[1].step_work_current/2);
                Discharge_Control(DAC80508_CH4, BOOST_MODE, V_MIN, step_work[1].step_work_current/2);

                REV_PORT3_EN;REV_PORT4_EN;
                HAL_Delay(20);
                PORT3_EN;PORT4_EN;
            }

        } else if(step_work[1].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH3, BOOST_MODE, step_work[1].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH4, BOOST_MODE, step_work[1].step_work_voltage, Iport_MAX);

            REV_PORT3_EN;REV_PORT4_EN;
            HAL_Delay(20);
            PORT3_EN;PORT4_EN;
        } else if(step_work[1].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            if(step_work[1].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH3, BOOST_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current);
                REV_PORT3_EN;
                HAL_Delay(20);
                PORT3_EN;
            } else {
                Discharge_Control(DAC80508_CH3, BOOST_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/2);
                Discharge_Control(DAC80508_CH4, BOOST_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/2);

                REV_PORT3_EN;REV_PORT4_EN;
                HAL_Delay(20);
                PORT3_EN;PORT4_EN;
            }

        } else if(step_work[1].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[1].step_type == loop_begin) {   //循环模式开始
        //     running_circle[1].circle_start = step_work[1].step_number;
        //     running_circle[1].circle_count = 1;
        //     Step_Data_Read(CH_2, running_circle[1].circle_start+1);
        //     Step_Run_Execute(CH_2);
        // }
        else if(step_work[1].step_type == loop_end) {     //循环模式结束
            if(running_circle[1][2].circle_flag == 0x01) {
                if(running_circle[1][2].circle_error == 0x00) {
                    running_circle[1][2].circle_count += 1;
                    if(running_circle[1][2].circle_count < running_circle[1][2].circle_number) {
                        Step_Run_Switch(CH_2, running_circle[1][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_2, step_work[1].step_number);
                    }
                } else {
                    Protection_Action(CH_2, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[1][1].circle_flag == 0x01) {
                if(running_circle[1][1].circle_error == 0x00) {
                    running_circle[1][1].circle_count += 1;
                    if(running_circle[1][1].circle_count < running_circle[1][1].circle_number) {
                        Step_Run_Switch(CH_2, running_circle[1][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_2, step_work[1].step_number);
                    }
                } else {
                    Protection_Action(CH_2, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[1][0].circle_flag == 0x01) {
                if(running_circle[1][0].circle_error == 0x00) {
                    running_circle[1][0].circle_count += 1;
                    if(running_circle[1][0].circle_count < running_circle[1][0].circle_number) {
                        Step_Run_Switch(CH_2, running_circle[1][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_2, step_work[1].step_number);
                    }
                } else {
                    Protection_Action(CH_2, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_2, global_protection_flag, step_stop_status);
                Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[1].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(2, 0, 0);
            // DAC80508_SetDacData(2, 0, 1);
            // DAC80508_SetDacData(3, 0, 0);
            // DAC80508_SetDacData(3, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(2, 0, 0);
            // DAC80508_SetDacData(2, 0, 1);
            // DAC80508_SetDacData(3, 0, 0);
            // DAC80508_SetDacData(3, 0, 1);
        }
    } else if(ch & CH_3) {
        if(step_work[2].step_type == standby) {             //静置
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(4, 0, 0);
            // DAC80508_SetDacData(4, 0, 1);
            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);
        } else if(step_work[2].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            if(step_work[2].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH5, BUCK_MODE, V_MAX, step_work[2].step_work_current);
                REV_PORT5_EN;
                HAL_Delay(20);
                PORT5_EN;
            } else {
                Discharge_Control(DAC80508_CH5, BUCK_MODE, V_MAX, step_work[2].step_work_current/2);
                Discharge_Control(DAC80508_CH6, BUCK_MODE, V_MAX, step_work[2].step_work_current/2);

                REV_PORT5_EN;REV_PORT6_EN;
                HAL_Delay(20);
                PORT5_EN;PORT6_EN;
            }

        } else if(step_work[2].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BUCK_MODE, step_work[2].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH6, BUCK_MODE, step_work[2].step_work_voltage, Iport_MAX);

            REV_PORT5_EN;REV_PORT6_EN;
            HAL_Delay(20);
            PORT5_EN;PORT6_EN;
        } else if(step_work[2].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            if(step_work[2].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH5, BUCK_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current);
                REV_PORT5_EN;
                HAL_Delay(20);
                PORT5_EN;
            } else {
                Discharge_Control(DAC80508_CH5, BUCK_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current/2);
                Discharge_Control(DAC80508_CH6, BUCK_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current/2);

                REV_PORT5_EN;REV_PORT6_EN;
                HAL_Delay(20);
                PORT5_EN;PORT6_EN;
            }

        } else if(step_work[2].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[2].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            if(step_work[2].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH5, BOOST_MODE, V_MIN, step_work[2].step_work_current);
                REV_PORT5_EN;
                HAL_Delay(20);
                PORT5_EN;
            } else {
                Discharge_Control(DAC80508_CH5, BOOST_MODE, V_MIN, step_work[2].step_work_current/2);
                Discharge_Control(DAC80508_CH6, BOOST_MODE, V_MIN, step_work[2].step_work_current/2);

                REV_PORT5_EN;REV_PORT6_EN;
                HAL_Delay(20);
                PORT5_EN;PORT6_EN;
            }

        } else if(step_work[2].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BOOST_MODE, step_work[2].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH6, BOOST_MODE, step_work[2].step_work_voltage, Iport_MAX);

            REV_PORT5_EN;REV_PORT6_EN;
            HAL_Delay(20);
            PORT5_EN;PORT6_EN;
        } else if(step_work[2].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            if(step_work[2].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH5, BOOST_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current);
                REV_PORT5_EN;
                HAL_Delay(20);
                PORT5_EN;
            } else {
                Discharge_Control(DAC80508_CH5, BOOST_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current/2);
                Discharge_Control(DAC80508_CH6, BOOST_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current/2);

                REV_PORT5_EN;REV_PORT6_EN;
                HAL_Delay(20);
                PORT5_EN;PORT6_EN;
            }

        } else if(step_work[2].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[2].step_type == loop_begin) {   //循环模式开始
        //     running_circle[2].circle_start = step_work[2].step_number;
        //     running_circle[2].circle_count = 1;
        //     Step_Data_Read(CH_3, running_circle[2].circle_start+1);
        //     Step_Run_Execute(CH_3);
        // }
        else if(step_work[2].step_type == loop_end) {     //循环模式结束
            if(running_circle[2][2].circle_flag == 0x01) {
                if(running_circle[2][2].circle_error == 0x00) {
                    running_circle[2][2].circle_count += 1;
                    if(running_circle[2][2].circle_count < running_circle[2][2].circle_number) {
                        Step_Run_Switch(CH_3, running_circle[2][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_3, step_work[2].step_number);
                    }
                } else {
                    Protection_Action(CH_3, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[2][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[2][1].circle_flag == 0x01) {
                if(running_circle[2][1].circle_error == 0x00) {
                    running_circle[2][1].circle_count += 1;
                    if(running_circle[2][1].circle_count < running_circle[2][1].circle_number) {
                        Step_Run_Switch(CH_3, running_circle[2][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_3, step_work[2].step_number);
                    }
                } else {
                    Protection_Action(CH_3, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[2][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[2][0].circle_flag == 0x01) {
                if(running_circle[2][0].circle_error == 0x00) {
                    running_circle[2][0].circle_count += 1;
                    if(running_circle[2][0].circle_count < running_circle[2][0].circle_number) {
                        Step_Run_Switch(CH_3, running_circle[2][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_3, step_work[2].step_number);
                    }
                } else {
                    Protection_Action(CH_3, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[2][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_3, global_protection_flag, step_stop_status);
                Global_Protection_Flag[2][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[2].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(4, 0, 0);
            // DAC80508_SetDacData(4, 0, 1);
            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(4, 0, 0);
            // DAC80508_SetDacData(4, 0, 1);
            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);
        }
    } else if(ch & CH_4) {
        if(step_work[3].step_type == standby) {             //静置
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);
            // DAC80508_SetDacData(7, 0, 0);
            // DAC80508_SetDacData(7, 0, 1);
        } else if(step_work[3].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[3].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH7, BUCK_MODE, V_MAX, step_work[3].step_work_current);
                REV_PORT7_EN;
                HAL_Delay(20);
                PORT7_EN;
            } else {
                Discharge_Control(DAC80508_CH7, BUCK_MODE, V_MAX, step_work[3].step_work_current/2);
                Discharge_Control(DAC80508_CH8, BUCK_MODE, V_MAX, step_work[3].step_work_current/2);

                REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT7_EN;PORT8_EN;
            }

        } else if(step_work[3].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BUCK_MODE, step_work[3].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH8, BUCK_MODE, step_work[3].step_work_voltage, Iport_MAX);

            REV_PORT7_EN;REV_PORT8_EN;
            HAL_Delay(20);
            PORT7_EN;PORT8_EN;
        } else if(step_work[3].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[3].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH7, BUCK_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current);
                REV_PORT7_EN;
                HAL_Delay(20);
                PORT7_EN;
            } else {
                Discharge_Control(DAC80508_CH7, BUCK_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current/2);
                Discharge_Control(DAC80508_CH8, BUCK_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current/2);

                REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT7_EN;PORT8_EN;
            }

        } else if(step_work[3].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[3].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[3].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH7, BOOST_MODE, V_MIN, step_work[3].step_work_current);
                REV_PORT7_EN;
                HAL_Delay(20);
                PORT7_EN;
            } else {
                Discharge_Control(DAC80508_CH7, BOOST_MODE, V_MIN, step_work[3].step_work_current/2);
                Discharge_Control(DAC80508_CH8, BOOST_MODE, V_MIN, step_work[3].step_work_current/2);

                REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT7_EN;PORT8_EN;
            }

        } else if(step_work[3].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BOOST_MODE, step_work[3].step_work_voltage, Iport_MAX);
            Discharge_Control(DAC80508_CH8, BOOST_MODE, step_work[3].step_work_voltage, Iport_MAX);

            REV_PORT7_EN;REV_PORT8_EN;
            HAL_Delay(20);
            PORT7_EN;PORT8_EN;
        } else if(step_work[3].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            if(step_work[3].step_work_current <= 2.0) {
                Discharge_Control(DAC80508_CH7, BOOST_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current);
                REV_PORT7_EN;
                HAL_Delay(20);
                PORT7_EN;
            } else {
                Discharge_Control(DAC80508_CH7, BOOST_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current/2);
                Discharge_Control(DAC80508_CH8, BOOST_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current/2);

                REV_PORT7_EN;REV_PORT8_EN;
                HAL_Delay(20);
                PORT7_EN;PORT8_EN;
            }

        } else if(step_work[3].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[3].step_type == loop_begin) {   //循环模式开始
        //     running_circle[3].circle_start = step_work[3].step_number;
        //     running_circle[3].circle_count = 1;
        //     Step_Data_Read(CH_4, running_circle[3].circle_start+1);
        //     Step_Run_Execute(CH_4);
        // }
        else if(step_work[3].step_type == loop_end) {     //循环模式结束
            if(running_circle[3][2].circle_flag == 0x01) {
                if(running_circle[3][2].circle_error == 0x00) {
                    running_circle[3][2].circle_count += 1;
                    if(running_circle[3][2].circle_count < running_circle[3][2].circle_number) {
                        Step_Run_Switch(CH_4, running_circle[3][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_4, step_work[3].step_number);
                    }
                } else {
                    Protection_Action(CH_4, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[3][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[3][1].circle_flag == 0x01) {
                if(running_circle[3][1].circle_error == 0x00) {
                    running_circle[3][1].circle_count += 1;
                    if(running_circle[3][1].circle_count < running_circle[3][1].circle_number) {
                        Step_Run_Switch(CH_4, running_circle[3][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_4, step_work[3].step_number);
                    }
                } else {
                    Protection_Action(CH_4, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[3][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[3][0].circle_flag == 0x01) {
                if(running_circle[3][0].circle_error == 0x00) {
                    running_circle[3][0].circle_count += 1;
                    if(running_circle[3][0].circle_count < running_circle[3][0].circle_number) {
                        Step_Run_Switch(CH_4, running_circle[3][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_4, step_work[3].step_number);
                    }
                } else {
                    Protection_Action(CH_4, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[3][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_4, global_protection_flag, step_stop_status);
                Global_Protection_Flag[3][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[3].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);
            // DAC80508_SetDacData(7, 0, 0);
            // DAC80508_SetDacData(7, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);
            // DAC80508_SetDacData(7, 0, 0);
            // DAC80508_SetDacData(7, 0, 1);
        }
    }
#endif
#if CHANNELS_8
    if(ch & CH_1) {
        if(step_work[0].step_type == standby) {             //静置
            DAC80508_SetDacData(0, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(0, 0, 0);
            DAC80508_SetDacData(0, 0, 1);
        } else if(step_work[0].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(0, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BUCK_MODE, V_MAX, step_work[0].step_work_current);

            REV_PORT1_EN;
            HAL_Delay(20);
            PORT1_EN;
        } else if(step_work[0].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(0, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, Iport_MAX);

            REV_PORT1_EN;
            HAL_Delay(20);
            PORT1_EN;
        } else if(step_work[0].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(0, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current);

            REV_PORT1_EN;
            HAL_Delay(20);
            PORT1_EN;
        } else if(step_work[0].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[0].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(0, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BOOST_MODE, V_MIN, step_work[0].step_work_current);

            REV_PORT1_EN;
            HAL_Delay(20);
            PORT1_EN;
        } else if(step_work[0].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(0, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, Iport_MAX);

            REV_PORT1_EN;
            HAL_Delay(20);
            PORT1_EN;
        } else if(step_work[0].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(0, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current);

            REV_PORT1_EN;
            HAL_Delay(20);
            PORT1_EN;
        } else if(step_work[0].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[0].step_type == loop_begin) {   //循环模式开始
        //     running_circle[0].circle_start = step_work[0].step_number;
        //     Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //     Step_Run_Execute(CH_1);
        //     running_circle[0].circle_count = 1;
        // }
        else if(step_work[0].step_type == loop_end) {     //循环模式结束
            if(running_circle[0][2].circle_flag == 0x01) {
                if(running_circle[0][2].circle_error == 0x00) {
                    running_circle[0][2].circle_count += 1;
                    if(running_circle[0][2].circle_count < running_circle[0][2].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[0][1].circle_flag == 0x01) {
                if(running_circle[0][1].circle_error == 0x00) {
                    running_circle[0][1].circle_count += 1;
                    if(running_circle[0][1].circle_count < running_circle[0][1].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[0][0].circle_flag == 0x01) {
                if(running_circle[0][0].circle_error == 0x00) {
                    running_circle[0][0].circle_count += 1;
                    if(running_circle[0][0].circle_count < running_circle[0][0].circle_number) {
                        Step_Run_Switch(CH_1, running_circle[0][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_1, step_work[0].step_number);
                    }
                } else {
                    Protection_Action(CH_1, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_1, global_protection_flag, step_stop_status);
                Global_Protection_Flag[0][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[0].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(0, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(0, 0, 0);
            // DAC80508_SetDacData(0, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(0, 0, 1);
            HAL_Delay(20);

            PORT1_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(0, 0, 0);
            // DAC80508_SetDacData(0, 0, 1);
        }
    } else if(ch & CH_2) {
        if(step_work[1].step_type == standby) {             //静置
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT2_DIS;
            HAL_Delay(10);
            REV_PORT2_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(1, 0, 0);
            DAC80508_SetDacData(1, 0, 1);
        } else if(step_work[1].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT2_DIS;
            HAL_Delay(10);
            REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH2, BUCK_MODE, V_MAX, step_work[1].step_work_current);

            REV_PORT2_EN;
            HAL_Delay(20);
            PORT2_EN;
        } else if(step_work[1].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT2_DIS;
            HAL_Delay(10);
            REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH2, BUCK_MODE, step_work[1].step_work_voltage, Iport_MAX);

            REV_PORT2_EN;
            HAL_Delay(20);
            PORT2_EN;
        } else if(step_work[1].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT2_DIS;
            HAL_Delay(10);
            REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH2, BUCK_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current);

            REV_PORT2_EN;
            HAL_Delay(20);
            PORT2_EN;
        } else if(step_work[1].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[1].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT2_DIS;
            HAL_Delay(10);
            REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH2, BOOST_MODE, V_MIN, step_work[1].step_work_current);

            REV_PORT2_EN;
            HAL_Delay(20);
            PORT2_EN;
        } else if(step_work[1].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT2_DIS;
            HAL_Delay(10);
            REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH2, BOOST_MODE, step_work[1].step_work_voltage, Iport_MAX);

            REV_PORT2_EN;
            HAL_Delay(20);
            PORT2_EN;
        } else if(step_work[1].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT2_DIS;
            HAL_Delay(10);
            REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH2, BOOST_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current);

            REV_PORT2_EN;
            HAL_Delay(20);
            PORT2_EN;
        } else if(step_work[1].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[0].step_type == loop_begin) {   //循环模式开始
        //     running_circle[0].circle_start = step_work[0].step_number;
        //     Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //     Step_Run_Execute(CH_1);
        //     running_circle[0].circle_count = 1;
        // }
        else if(step_work[1].step_type == loop_end) {     //循环模式结束
            if(running_circle[1][2].circle_flag == 0x01) {
                if(running_circle[1][2].circle_error == 0x00) {
                    running_circle[1][2].circle_count += 1;
                    if(running_circle[1][2].circle_count < running_circle[1][2].circle_number) {
                        Step_Run_Switch(CH_2, running_circle[1][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_2, step_work[1].step_number);
                    }
                } else {
                    Protection_Action(CH_2, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[1][1].circle_flag == 0x01) {
                if(running_circle[1][1].circle_error == 0x00) {
                    running_circle[1][1].circle_count += 1;
                    if(running_circle[1][1].circle_count < running_circle[1][1].circle_number) {
                        Step_Run_Switch(CH_2, running_circle[1][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_2, step_work[1].step_number);
                    }
                } else {
                    Protection_Action(CH_2, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[1][0].circle_flag == 0x01) {
                if(running_circle[1][0].circle_error == 0x00) {
                    running_circle[1][0].circle_count += 1;
                    if(running_circle[1][0].circle_count < running_circle[1][0].circle_number) {
                        Step_Run_Switch(CH_2, running_circle[1][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_2, step_work[1].step_number);
                    }
                } else {
                    Protection_Action(CH_2, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_2, global_protection_flag, step_stop_status);
                Global_Protection_Flag[1][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[1].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT2_DIS;
            HAL_Delay(10);
            REV_PORT2_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(1, 0, 0);
            // DAC80508_SetDacData(1, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(1, 0, 1);
            HAL_Delay(20);

            PORT2_DIS;
            HAL_Delay(10);
            REV_PORT2_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(1, 0, 0);
            // DAC80508_SetDacData(1, 0, 1);
        }
    } else if(ch & CH_3) {
        if(step_work[2].step_type == standby) {             //静置
            DAC80508_SetDacData(2, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(2, 0, 0);
            DAC80508_SetDacData(2, 0, 1);
        } else if(step_work[2].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(2, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH3, BUCK_MODE, V_MAX, step_work[2].step_work_current);

            REV_PORT3_EN;
            HAL_Delay(20);
            PORT3_EN;
        } else if(step_work[2].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(2, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH3, BUCK_MODE, step_work[2].step_work_voltage, Iport_MAX);

            REV_PORT3_EN;
            HAL_Delay(20);
            PORT3_EN;
        } else if(step_work[2].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(2, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH3, BUCK_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current);

            REV_PORT3_EN;
            HAL_Delay(20);
            PORT3_EN;
        } else if(step_work[2].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[2].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(2, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH3, BOOST_MODE, V_MIN, step_work[2].step_work_current);

            REV_PORT3_EN;
            HAL_Delay(20);
            PORT3_EN;
        } else if(step_work[2].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(2, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH3, BOOST_MODE, step_work[2].step_work_voltage, Iport_MAX);

            REV_PORT3_EN;
            HAL_Delay(20);
            PORT3_EN;
        } else if(step_work[2].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(2, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH3, BOOST_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current);

            REV_PORT3_EN;
            HAL_Delay(20);
            PORT3_EN;
        } else if(step_work[2].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[0].step_type == loop_begin) {   //循环模式开始
        //     running_circle[0].circle_start = step_work[0].step_number;
        //     Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //     Step_Run_Execute(CH_1);
        //     running_circle[0].circle_count = 1;
        // }
        else if(step_work[2].step_type == loop_end) {     //循环模式结束
            if(running_circle[2][2].circle_flag == 0x01) {
                if(running_circle[2][2].circle_error == 0x00) {
                    running_circle[2][2].circle_count += 1;
                    if(running_circle[2][2].circle_count < running_circle[2][2].circle_number) {
                        Step_Run_Switch(CH_3, running_circle[2][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_3, step_work[2].step_number);
                    }
                } else {
                    Protection_Action(CH_3, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[2][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[2][1].circle_flag == 0x01) {
                if(running_circle[2][1].circle_error == 0x00) {
                    running_circle[2][1].circle_count += 1;
                    if(running_circle[2][1].circle_count < running_circle[2][1].circle_number) {
                        Step_Run_Switch(CH_3, running_circle[2][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_3, step_work[2].step_number);
                    }
                } else {
                    Protection_Action(CH_3, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[2][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[2][0].circle_flag == 0x01) {
                if(running_circle[2][0].circle_error == 0x00) {
                    running_circle[2][0].circle_count += 1;
                    if(running_circle[2][0].circle_count < running_circle[2][0].circle_number) {
                        Step_Run_Switch(CH_3, running_circle[2][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_3, step_work[2].step_number);
                    }
                } else {
                    Protection_Action(CH_3, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[2][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_3, global_protection_flag, step_stop_status);
                Global_Protection_Flag[2][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[2].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(2, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(2, 0, 0);
            // DAC80508_SetDacData(2, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(2, 0, 1);
            HAL_Delay(20);

            PORT3_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(2, 0, 0);
            // DAC80508_SetDacData(2, 0, 1);
        }
    } else if(ch & CH_4) {
        if(step_work[3].step_type == standby) {             //静置
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT4_DIS;
            HAL_Delay(10);
            REV_PORT4_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(3, 0, 0);
            DAC80508_SetDacData(3, 0, 1);
        } else if(step_work[3].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT4_DIS;
            HAL_Delay(10);
            REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH4, BUCK_MODE, V_MAX, step_work[3].step_work_current);

            REV_PORT4_EN;
            HAL_Delay(20);
            PORT4_EN;
        } else if(step_work[3].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT4_DIS;
            HAL_Delay(10);
            REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH4, BUCK_MODE, step_work[3].step_work_voltage, Iport_MAX);

            REV_PORT4_EN;
            HAL_Delay(20);
            PORT4_EN;
        } else if(step_work[3].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT4_DIS;
            HAL_Delay(10);
            REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH4, BUCK_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current);

            REV_PORT4_EN;
            HAL_Delay(20);
            PORT4_EN;
        } else if(step_work[3].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[3].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT4_DIS;
            HAL_Delay(10);
            REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH4, BOOST_MODE, V_MIN, step_work[3].step_work_current);

            REV_PORT4_EN;
            HAL_Delay(20);
            PORT4_EN;
        } else if(step_work[3].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT4_DIS;
            HAL_Delay(10);
            REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH4, BOOST_MODE, step_work[3].step_work_voltage, Iport_MAX);

            REV_PORT4_EN;
            HAL_Delay(20);
            PORT4_EN;
        } else if(step_work[3].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT4_DIS;
            HAL_Delay(10);
            REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH4, BOOST_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current);

            REV_PORT4_EN;
            HAL_Delay(20);
            PORT4_EN;
        } else if(step_work[3].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[0].step_type == loop_begin) {   //循环模式开始
        //     running_circle[0].circle_start = step_work[0].step_number;
        //     Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //     Step_Run_Execute(CH_1);
        //     running_circle[0].circle_count = 1;
        // }
        else if(step_work[3].step_type == loop_end) {     //循环模式结束
            if(running_circle[3][2].circle_flag == 0x01) {
                if(running_circle[3][2].circle_error == 0x00) {
                    running_circle[3][2].circle_count += 1;
                    if(running_circle[3][2].circle_count < running_circle[3][2].circle_number) {
                        Step_Run_Switch(CH_4, running_circle[3][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_4, step_work[3].step_number);
                    }
                } else {
                    Protection_Action(CH_4, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[3][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[3][1].circle_flag == 0x01) {
                if(running_circle[3][1].circle_error == 0x00) {
                    running_circle[3][1].circle_count += 1;
                    if(running_circle[3][1].circle_count < running_circle[3][1].circle_number) {
                        Step_Run_Switch(CH_4, running_circle[3][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_4, step_work[3].step_number);
                    }
                } else {
                    Protection_Action(CH_4, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[3][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[3][0].circle_flag == 0x01) {
                if(running_circle[3][0].circle_error == 0x00) {
                    running_circle[3][0].circle_count += 1;
                    if(running_circle[3][0].circle_count < running_circle[3][0].circle_number) {
                        Step_Run_Switch(CH_4, running_circle[3][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_4, step_work[3].step_number);
                    }
                } else {
                    Protection_Action(CH_4, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[3][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_4, global_protection_flag, step_stop_status);
                Global_Protection_Flag[3][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[3].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT4_DIS;
            HAL_Delay(10);
            REV_PORT4_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(3, 0, 0);
            // DAC80508_SetDacData(3, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(3, 0, 1);
            HAL_Delay(20);

            PORT4_DIS;
            HAL_Delay(10);
            REV_PORT4_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(3, 0, 0);
            // DAC80508_SetDacData(3, 0, 1);
        }
    }
    if(ch & CH_5) {
        if(step_work[4].step_type == standby) {             //静置
            DAC80508_SetDacData(4, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(4, 0, 0);
            DAC80508_SetDacData(4, 0, 1);
        } else if(step_work[4].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(4, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BUCK_MODE, V_MAX, step_work[4].step_work_current);

            REV_PORT5_EN;
            HAL_Delay(20);
            PORT5_EN;
        } else if(step_work[4].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(4, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BUCK_MODE, step_work[4].step_work_voltage, Iport_MAX);

            REV_PORT5_EN;
            HAL_Delay(20);
            PORT5_EN;
        } else if(step_work[4].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(4, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BUCK_MODE, step_work[4].step_work_voltage, step_work[4].step_work_current);

            REV_PORT5_EN;
            HAL_Delay(20);
            PORT5_EN;
        } else if(step_work[4].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[4].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(4, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BOOST_MODE, V_MIN, step_work[4].step_work_current);

            REV_PORT5_EN;
            HAL_Delay(20);
            PORT5_EN;
        } else if(step_work[4].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(4, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BOOST_MODE, step_work[4].step_work_voltage, Iport_MAX);

            REV_PORT5_EN;
            HAL_Delay(20);
            PORT5_EN;
        } else if(step_work[4].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(4, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BOOST_MODE, step_work[4].step_work_voltage, step_work[4].step_work_current);

            REV_PORT5_EN;
            HAL_Delay(20);
            PORT5_EN;
        } else if(step_work[4].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[0].step_type == loop_begin) {   //循环模式开始
        //     running_circle[0].circle_start = step_work[0].step_number;
        //     Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //     Step_Run_Execute(CH_1);
        //     running_circle[0].circle_count = 1;
        // }
        else if(step_work[4].step_type == loop_end) {     //循环模式结束
            if(running_circle[4][2].circle_flag == 0x01) {
                if(running_circle[4][2].circle_error == 0x00) {
                    running_circle[4][2].circle_count += 1;
                    if(running_circle[4][2].circle_count < running_circle[4][2].circle_number) {
                        Step_Run_Switch(CH_5, running_circle[4][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_5, step_work[4].step_number);
                    }
                } else {
                    Protection_Action(CH_5, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[4][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[4][1].circle_flag == 0x01) {
                if(running_circle[4][1].circle_error == 0x00) {
                    running_circle[4][1].circle_count += 1;
                    if(running_circle[4][1].circle_count < running_circle[4][1].circle_number) {
                        Step_Run_Switch(CH_5, running_circle[4][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_5, step_work[4].step_number);
                    }
                } else {
                    Protection_Action(CH_5, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[4][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[4][0].circle_flag == 0x01) {
                if(running_circle[4][0].circle_error == 0x00) {
                    running_circle[4][0].circle_count += 1;
                    if(running_circle[4][0].circle_count < running_circle[4][0].circle_number) {
                        Step_Run_Switch(CH_5, running_circle[4][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_5, step_work[4].step_number);
                    }
                } else {
                    Protection_Action(CH_5, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[4][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_5, global_protection_flag, step_stop_status);
                Global_Protection_Flag[4][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[4].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(4, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(4, 0, 0);
            // DAC80508_SetDacData(4, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(4, 0, 1);
            HAL_Delay(20);

            PORT5_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(4, 0, 0);
            // DAC80508_SetDacData(4, 0, 1);
        }
    } else if(ch & CH_6) {
        if(step_work[5].step_type == standby) {             //静置
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT6_DIS;
            HAL_Delay(10);
            REV_PORT6_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);
        } else if(step_work[5].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT6_DIS;
            HAL_Delay(10);
            REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH6, BUCK_MODE, V_MAX, step_work[5].step_work_current);

            REV_PORT6_EN;
            HAL_Delay(20);
            PORT6_EN;
        } else if(step_work[5].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT6_DIS;
            HAL_Delay(10);
            REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH6, BUCK_MODE, step_work[5].step_work_voltage, Iport_MAX);

            REV_PORT6_EN;
            HAL_Delay(20);
            PORT6_EN;
        } else if(step_work[5].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT6_DIS;
            HAL_Delay(10);
            REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH6, BUCK_MODE, step_work[5].step_work_voltage, step_work[5].step_work_current);

            REV_PORT6_EN;
            HAL_Delay(20);
            PORT6_EN;
        } else if(step_work[5].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[5].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT6_DIS;
            HAL_Delay(10);
            REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH6, BOOST_MODE, V_MIN, step_work[5].step_work_current);

            REV_PORT6_EN;
            HAL_Delay(20);
            PORT6_EN;
        } else if(step_work[5].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT6_DIS;
            HAL_Delay(10);
            REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH6, BOOST_MODE, step_work[5].step_work_voltage, Iport_MAX);

            REV_PORT6_EN;
            HAL_Delay(20);
            PORT6_EN;
        } else if(step_work[5].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT6_DIS;
            HAL_Delay(10);
            REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH6, BOOST_MODE, step_work[5].step_work_voltage, step_work[5].step_work_current);

            REV_PORT6_EN;
            HAL_Delay(20);
            PORT6_EN;
        } else if(step_work[5].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[0].step_type == loop_begin) {   //循环模式开始
        //     running_circle[0].circle_start = step_work[0].step_number;
        //     Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //     Step_Run_Execute(CH_1);
        //     running_circle[0].circle_count = 1;
        // }
        else if(step_work[5].step_type == loop_end) {     //循环模式结束
            if(running_circle[5][2].circle_flag == 0x01) {
                if(running_circle[5][2].circle_error == 0x00) {
                    running_circle[5][2].circle_count += 1;
                    if(running_circle[5][2].circle_count < running_circle[5][2].circle_number) {
                        Step_Run_Switch(CH_6, running_circle[5][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_6, step_work[5].step_number);
                    }
                } else {
                    Protection_Action(CH_6, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[5][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[5][1].circle_flag == 0x01) {
                if(running_circle[5][1].circle_error == 0x00) {
                    running_circle[5][1].circle_count += 1;
                    if(running_circle[5][1].circle_count < running_circle[5][1].circle_number) {
                        Step_Run_Switch(CH_6, running_circle[5][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_6, step_work[5].step_number);
                    }
                } else {
                    Protection_Action(CH_6, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[5][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[5][0].circle_flag == 0x01) {
                if(running_circle[5][0].circle_error == 0x00) {
                    running_circle[5][0].circle_count += 1;
                    if(running_circle[5][0].circle_count < running_circle[5][0].circle_number) {
                        Step_Run_Switch(CH_6, running_circle[5][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_6, step_work[5].step_number);
                    }
                } else {
                    Protection_Action(CH_6, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[5][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_6, global_protection_flag, step_stop_status);
                Global_Protection_Flag[5][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[5].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT6_DIS;
            HAL_Delay(10);
            REV_PORT6_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(5, 0, 1);
            HAL_Delay(20);

            PORT6_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(5, 0, 0);
            // DAC80508_SetDacData(5, 0, 1);
        }
    } else if(ch & CH_7) {
        if(step_work[6].step_type == standby) {             //静置
            DAC80508_SetDacData(6, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);
        } else if(step_work[6].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(6, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BUCK_MODE, V_MAX, step_work[6].step_work_current);

            REV_PORT7_EN;
            HAL_Delay(20);
            PORT7_EN;
        } else if(step_work[6].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(6, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BUCK_MODE, step_work[6].step_work_voltage, Iport_MAX);

            REV_PORT7_EN;
            HAL_Delay(20);
            PORT7_EN;
        } else if(step_work[6].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(6, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BUCK_MODE, step_work[6].step_work_voltage, step_work[6].step_work_current);

            REV_PORT7_EN;
            HAL_Delay(20);
            PORT7_EN;
        } else if(step_work[6].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[6].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(6, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BOOST_MODE, V_MIN, step_work[6].step_work_current);

            REV_PORT7_EN;
            HAL_Delay(20);
            PORT7_EN;
        } else if(step_work[6].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(6, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BOOST_MODE, step_work[6].step_work_voltage, Iport_MAX);

            REV_PORT7_EN;
            HAL_Delay(20);
            PORT7_EN;
        } else if(step_work[6].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(6, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BOOST_MODE, step_work[6].step_work_voltage, step_work[6].step_work_current);

            REV_PORT7_EN;
            HAL_Delay(20);
            PORT7_EN;
        } else if(step_work[6].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[0].step_type == loop_begin) {   //循环模式开始
        //     running_circle[0].circle_start = step_work[0].step_number;
        //     Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //     Step_Run_Execute(CH_1);
        //     running_circle[0].circle_count = 1;
        // }
        else if(step_work[6].step_type == loop_end) {     //循环模式结束
            if(running_circle[6][2].circle_flag == 0x01) {
                if(running_circle[6][2].circle_error == 0x00) {
                    running_circle[6][2].circle_count += 1;
                    if(running_circle[6][2].circle_count < running_circle[6][2].circle_number) {
                        Step_Run_Switch(CH_7, running_circle[6][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_7, step_work[6].step_number);
                    }
                } else {
                    Protection_Action(CH_7, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[6][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[6][1].circle_flag == 0x01) {
                if(running_circle[6][1].circle_error == 0x00) {
                    running_circle[6][1].circle_count += 1;
                    if(running_circle[6][1].circle_count < running_circle[6][1].circle_number) {
                        Step_Run_Switch(CH_7, running_circle[6][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_7, step_work[6].step_number);
                    }
                } else {
                    Protection_Action(CH_7, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[6][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[6][0].circle_flag == 0x01) {
                if(running_circle[6][0].circle_error == 0x00) {
                    running_circle[6][0].circle_count += 1;
                    if(running_circle[6][0].circle_count < running_circle[6][0].circle_number) {
                        Step_Run_Switch(CH_7, running_circle[6][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_7, step_work[6].step_number);
                    }
                } else {
                    Protection_Action(CH_7, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[6][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_7, global_protection_flag, step_stop_status);
                Global_Protection_Flag[6][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[6].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(6, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(6, 0, 1);
            HAL_Delay(20);

            PORT7_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(6, 0, 0);
            // DAC80508_SetDacData(6, 0, 1);
        }
    } else if(ch & CH_8) {
        if(step_work[7].step_type == standby) {             //静置
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT8_DIS;
            HAL_Delay(10);
            REV_PORT8_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(7, 0, 0);
            DAC80508_SetDacData(7, 0, 1);
        } else if(step_work[7].step_type == CC) {           //恒流充电
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT8_DIS;
            HAL_Delay(10);
            REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH8, BUCK_MODE, V_MAX, step_work[7].step_work_current);

            REV_PORT8_EN;
            HAL_Delay(20);
            PORT8_EN;
        } else if(step_work[7].step_type == CV) {           //恒压充电
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT8_DIS;
            HAL_Delay(10);
            REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH8, BUCK_MODE, step_work[7].step_work_voltage, Iport_MAX);

            REV_PORT8_EN;
            HAL_Delay(20);
            PORT8_EN;
        } else if(step_work[7].step_type == CC_to_CV) {     //恒流转恒压充电
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT8_DIS;
            HAL_Delay(10);
            REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH8, BUCK_MODE, step_work[7].step_work_voltage, step_work[7].step_work_current);

            REV_PORT8_EN;
            HAL_Delay(20);
            PORT8_EN;
        } else if(step_work[7].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[7].step_type == DCC) {          //恒流放电
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT8_DIS;
            HAL_Delay(10);
            REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH8, BOOST_MODE, V_MIN, step_work[7].step_work_current);

            REV_PORT8_EN;
            HAL_Delay(20);
            PORT8_EN;
        } else if(step_work[7].step_type == DCV) {          //恒压放电
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT8_DIS;
            HAL_Delay(10);
            REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH8, BOOST_MODE, step_work[7].step_work_voltage, Iport_MAX);

            REV_PORT8_EN;
            HAL_Delay(20);
            PORT8_EN;
        } else if(step_work[7].step_type == DCC_to_DCV) {   //恒流转恒压放电
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT8_DIS;
            HAL_Delay(10);
            REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH8, BOOST_MODE, step_work[7].step_work_voltage, step_work[7].step_work_current);

            REV_PORT8_EN;
            HAL_Delay(20);
            PORT8_EN;
        } else if(step_work[7].step_type == DCP) {          //恒功率放电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        }
        // else if(step_work[0].step_type == loop_begin) {   //循环模式开始
        //     running_circle[0].circle_start = step_work[0].step_number;
        //     Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //     Step_Run_Execute(CH_1);
        //     running_circle[0].circle_count = 1;
        // }
        else if(step_work[7].step_type == loop_end) {     //循环模式结束
            if(running_circle[7][2].circle_flag == 0x01) {
                if(running_circle[7][2].circle_error == 0x00) {
                    running_circle[7][2].circle_count += 1;
                    if(running_circle[7][2].circle_count < running_circle[7][2].circle_number) {
                        Step_Run_Switch(CH_8, running_circle[7][2].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_8, step_work[7].step_number);
                    }
                } else {
                    Protection_Action(CH_8, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[7][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[7][1].circle_flag == 0x01) {
                if(running_circle[7][1].circle_error == 0x00) {
                    running_circle[7][1].circle_count += 1;
                    if(running_circle[7][1].circle_count < running_circle[7][1].circle_number) {
                        Step_Run_Switch(CH_8, running_circle[7][1].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_8, step_work[7].step_number);
                    }
                } else {
                    Protection_Action(CH_8, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[7][step_data_error_trigger] = 0x01;
                }
            } else if(running_circle[7][0].circle_flag == 0x01) {
                if(running_circle[7][0].circle_error == 0x00) {
                    running_circle[7][0].circle_count += 1;
                    if(running_circle[7][0].circle_count < running_circle[7][0].circle_number) {
                        Step_Run_Switch(CH_8, running_circle[7][0].circle_start-1);
                    } else {
                        Step_Run_Switch(CH_8, step_work[7].step_number);
                    }
                } else {
                    Protection_Action(CH_8, global_protection_flag, step_stop_status);
                    Global_Protection_Flag[7][step_data_error_trigger] = 0x01;
                }
            } else {
                Protection_Action(CH_8, global_protection_flag, step_stop_status);
                Global_Protection_Flag[7][step_data_error_trigger] = 0x01;
            }
        }
        else if(step_work[7].step_type == end_step) {     //工步结束
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT8_DIS;
            HAL_Delay(10);
            REV_PORT8_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(7, 0, 0);
            // DAC80508_SetDacData(7, 0, 1);

            //执行结束相关的动作
        } else {
            DAC80508_SetDacData(7, 0, 1);
            HAL_Delay(20);

            PORT8_DIS;
            HAL_Delay(10);
            REV_PORT8_DIS;
            HAL_Delay(10);

            // DAC80508_SetDacData(7, 0, 0);
            // DAC80508_SetDacData(7, 0, 1);
        }
    }
#endif
}

void Step_Run_Analysis(void)
{
    if(STEP_STATUS_SETTING_FLAG) {
        for(int i = 0; i < CHANNELS; i++) {
            if(STEP_STATUS_SETTING_FLAG & (0x01<<i)) {
                switch(step_run[i].run_status) {
                    case step_start_setting: {
                        led_run_setting(0, led_normal, &(led_control));

                        last_record_condition[i].record_current = 0;
                        last_record_condition[i].record_voltage = 0;
                        last_record_condition[i].record_time = 0;

                        this_record_condition[i].record_current = 0;
                        this_record_condition[i].record_voltage = 0;
                        this_record_condition[i].record_time = 0;

                        fluctuation_voltage_flag[i] = 0;
                        fluctuation_current_flag[i] = 0;

                        fluctuation_protection_old[i].voltage = 0;
                        fluctuation_protection_old[i].voltage_time = 0;
                        fluctuation_protection_old[i].current = 0;
                        fluctuation_protection_old[i].current_time = 0;

                        fluctuation_protection_now[i].voltage = 0;
                        fluctuation_protection_now[i].voltage_time = 0;
                        fluctuation_protection_now[i].current = 0;
                        fluctuation_protection_now[i].current_time = 0;

                        jump_protection_old[i].current = 0;
                        jump_protection_old[i].voltage = 0;
                        jump_protection_old[i].current_positive_jump_number = 0;
                        jump_protection_old[i].current_negative_jump_number = 0;
                        jump_protection_old[i].voltage_positive_jump_number = 0;
                        jump_protection_old[i].voltage_negative_jump_number = 0;

                        jump_protection_now[i].current = 0;
                        jump_protection_now[i].voltage = 0;
                        jump_protection_now[i].current_positive_jump_number = 0;
                        jump_protection_now[i].current_negative_jump_number = 0;
                        jump_protection_now[i].voltage_positive_jump_number = 0;
                        jump_protection_now[i].voltage_negative_jump_number = 0;

                        PG_count[i] = 0;

                        record_data.channel_record_data[i].battery_capacitance = 0;
                        record_data.channel_record_data[i].battery_energy = 0;
                        record_data.channel_record_data[i].crosscurrent_ratio = 0;
                        record_data.channel_record_data[i].sum_capacitance = 0;

                        CV_mode[i] = 0;
                        DCV_mode[i] = 0;

                        timing_voltage_time_flag[i] = 0x00;
                        timing_voltage_change_time1_flag[i] = 0x00;
                        timing_voltage_change_time2_flag[i] = 0x00;

                        for(int j = 0; j < 3; j++) {
                            running_circle[i][j].circle_start = 0;
                            running_circle[i][j].circle_serial = 0;
                            running_circle[i][j].circle_number = 0;
                            running_circle[i][j].circle_count = 0;
                            running_circle[i][j].circle_flag = 0;
                            running_circle[i][j].circle_error = 0;
                        }

                        // memset(Global_Protection_Flag[i], 0, global_protection_trigger_end);
                        // memset(Step_Protection_Flag[i], 0, step_protection_trigger_end);


                        //--------------------------------------------------------------------------
                        if(running_step[i].status != step_contact_status) {
                            //清除上一工步的值-------------------------------------------------------
                            step_work[i].step_number = 0;
                            step_work[i].step_type = 0;
                            step_work[i].step_work_voltage = 0;
                            step_work[i].step_work_current = 0;//Ich_MAX;
                            step_work[i].step_work_power = 0;
                            step_work[i].step_abort_priority = step_time;
                            step_work[i].step_abort_time = 0;
                            step_work[i].step_abort_voltage = 0;
                            step_work[i].step_abort_current = 0;
                            step_work[i].step_abort_capacitance = 0;

                            step_record_condition[i].record_time = 0;
                            step_record_condition[i].record_voltage = 0;
                            step_record_condition[i].record_current = 0;

                            step_protection[i].protection_OverTemperature_alarm = 0;
                            step_protection[i].protection_charge_capacitance = 0;
                            step_protection[i].protection_discharge_capacitance = 0;
                            step_protection[i].protection_voltage_high_limit = 0;
                            step_protection[i].protection_voltage_low_limit = 0;
                            step_protection[i].protection_charge_current_limit = 0;
                            step_protection[i].protection_discharge_current_limit = 0;
                            step_protection[i].protection_current_positive_jump_range = 0;
                            step_protection[i].protection_current_positive_jump_number = 0;
                            step_protection[i].protection_current_negative_jump_range = 0;
                            step_protection[i].protection_current_negative_jump_number = 0;
                            step_protection[i].protection_voltage_positive_jump_range = 0;
                            step_protection[i].protection_voltage_positive_jump_number = 0;
                            step_protection[i].protection_voltage_negative_jump_range = 0;
                            step_protection[i].protection_voltage_negative_jump_number = 0;
                            step_protection[i].protection_current_OutOfTolerance = 0;
                            step_protection[i].protection_voltage_OutOfTolerance = 0;
                            //----------------------------------------------------------------------

                            running_step[i].status = step_start_flag;
                            Record_Report(0x01<<i);

                            running_step[i].status = step_contact_status;

                        #if GLOBAL_initial_voltage_high
                            if( (FLOAT_NOT_ZERO(global_protection[i].initial_voltage_high) > 0) && \
                                (record_data.channel_record_data[i].battery_voltage >= global_protection[i].initial_voltage_high)) {
                                Protection_Action((0x01<<i), global_protection_flag, step_stop_status);

                                Global_Protection_Flag[i][initial_voltage_high_trigger] = 0x01;

                                step_run[i].run_status = 0xff;
                                STEP_STATUS_SETTING_FLAG = 0x00;
                                return;
                            } else {
                                Global_Protection_Flag[i][initial_voltage_high_trigger] = 0x00;
                            }
                        #endif

                        #if GLOBAL_initial_voltage_low
                            if( (FLOAT_NOT_ZERO(global_protection[i].initial_voltage_low) > 0) && \
                                (record_data.channel_record_data[i].battery_voltage <= global_protection[i].initial_voltage_low)) {
                                Protection_Action((0x01<<i), global_protection_flag, step_stop_status);

                                Global_Protection_Flag[i][initial_voltage_low_trigger] = 0x01;

                                step_run[i].run_status = 0xff;
                                STEP_STATUS_SETTING_FLAG = 0x00;
                                return;
                            } else {
                                Global_Protection_Flag[i][initial_voltage_low_trigger] = 0x00;
                            }
                        #endif

                        #if GLOBAL_contact_voltage_variation
                            if( (FLOAT_NOT_ZERO(global_protection[i].contact_time) > 0) && \
                                (FLOAT_NOT_ZERO(global_protection[i].contact_current) > 0) && \
                                (FLOAT_NOT_ZERO(global_protection[i].contact_voltage_variation) > 0)) {
                                step_work[i].step_type = CC;
                                step_work[i].step_work_current = global_protection[i].contact_current;
                                step_work[i].step_abort_time = global_protection[i].contact_time;
                                step_record_condition[i].record_time = 1;

                                Step_Run_Execute(0x01<<i);
                                running_step[i].type = step_work[i].step_type;
                                running_step[i].number = step_work[i].step_number;
                                running_step[i].sum_number = 1;
                                running_step[i].run_time = 0;
                                running_step[i].protection_time = 0;

                                contact_old_voltage[i] = record_data.channel_record_data[i].battery_voltage;
                            }
                        #else
                            return;
                        #endif
                        } else {
                            Step_Data_Read((0x01<<i), step_run[i].run_number);
                            Step_Run_Execute(0x01<<i);

                            running_step[i].type = step_work[i].step_type;
                            running_step[i].number = step_work[i].step_number;
                            running_step[i].sum_number = 1;
                            running_step[i].run_time = 0;
                            running_step[i].protection_time = 0;

                            if(running_step[i].type == end_step) {
                                running_step[i].status = step_complete_status;
                                Record_Report(0x01<<i);
                            } else if((running_step[i].type < standby) || (running_step[i].type > end_step)) {
                                running_step[i].status = step_stop_status;
                                Record_Report(0x01<<i);
                            } else {
                                running_step[i].status = step_running_status;
                            }
                        }
                        //--------------------------------------------------------------------------

                        break;
                    }
                    case step_stop_setting: {
                        led_run_setting((0x01<<i), led_normal, &(led_control));

                        //执行停止相关动作
                        Step_Turn_Off(0x01<<i);

                        running_step[i].status = step_stop_status;
                        Record_Report(0x01<<i);

                        break;
                    }
                    case step_pause_setting: {
                        led_run_setting((0x01<<i), led_normal, &(led_control));

                        //执行暂停相关动作
                        Step_Turn_Off(0x01<<i);

                        fluctuation_voltage_flag[i] = 0;
                        fluctuation_current_flag[i] = 0;

                        running_step[i].status = step_pause_status;
                        running_step[i].protection_time = 0;
                        Record_Report(0x01<<i);

                        break;
                    }
                    case step_continue_setting: {
                        //执行继续运行相关动作
                        if(running_step[i].status == step_pause_status) {
                            Step_Run_Execute(0x01<<i);

                            running_step[i].status = step_running_status;
                            Record_Report(0x01<<i);
                        }

                        break;
                    }
                    case step_jump_setting: {
                        //先停止相应通道
                        Step_Turn_Off(0x01<<i);
                        HAL_Delay(1000);

                        //再重新启动跳转后的工步------------------------------------------------------
                        last_record_condition[i].record_current = 0;
                        last_record_condition[i].record_voltage = 0;
                        last_record_condition[i].record_time = 0;

                        this_record_condition[i].record_current = 0;
                        this_record_condition[i].record_voltage = 0;
                        this_record_condition[i].record_time = 0;

                        fluctuation_voltage_flag[i] = 0;
                        fluctuation_current_flag[i] = 0;

                        fluctuation_protection_old[i].voltage = 0;
                        fluctuation_protection_old[i].voltage_time = 0;
                        fluctuation_protection_old[i].current = 0;
                        fluctuation_protection_old[i].current_time = 0;

                        fluctuation_protection_now[i].voltage = 0;
                        fluctuation_protection_now[i].voltage_time = 0;
                        fluctuation_protection_now[i].current = 0;
                        fluctuation_protection_now[i].current_time = 0;

                        jump_protection_old[i].current = 0;
                        jump_protection_old[i].voltage = 0;
                        jump_protection_old[i].current_positive_jump_number = 0;
                        jump_protection_old[i].current_negative_jump_number = 0;
                        jump_protection_old[i].voltage_positive_jump_number = 0;
                        jump_protection_old[i].voltage_negative_jump_number = 0;

                        jump_protection_now[i].current = 0;
                        jump_protection_now[i].voltage = 0;
                        jump_protection_now[i].current_positive_jump_number = 0;
                        jump_protection_now[i].current_negative_jump_number = 0;
                        jump_protection_now[i].voltage_positive_jump_number = 0;
                        jump_protection_now[i].voltage_negative_jump_number = 0;

                        PG_count[i] = 0;

                        record_data.channel_record_data[i].battery_capacitance = 0;
                        record_data.channel_record_data[i].battery_energy = 0;
                        record_data.channel_record_data[i].crosscurrent_ratio = 0;

                        CV_mode[i] = 0;
                        DCV_mode[i] = 0;

                        timing_voltage_time_flag[i] = 0x00;
                        timing_voltage_change_time1_flag[i] = 0x00;
                        timing_voltage_change_time2_flag[i] = 0x00;

                        // memset(Global_Protection_Flag[i], 0, global_protection_trigger_end);
                        // memset(Step_Protection_Flag[i], 0, step_protection_trigger_end);

                        Step_Data_Read((0x01<<i), step_run[i].run_number);
                        Step_Run_Execute(0x01<<i);

                        running_step[i].type = step_work[i].step_type;
                        running_step[i].number = step_work[i].step_number;
                        running_step[i].sum_number += 1;
                        running_step[i].run_time = 0;
                        running_step[i].protection_time = 0;

                        if(running_step[i].type == end_step) {
                            running_step[i].status = step_complete_status;
                            Record_Report(0x01<<i);
                        } else if((running_step[i].type < standby) || (running_step[i].type > end_step)) {
                            running_step[i].status = step_stop_status;
                            Record_Report(0x01<<i);
                        } else {
                            running_step[i].status = step_running_status;
                        }
                        //--------------------------------------------------------------------------

                        break;
                    }
                    default: {
                        break;
                    }
                }
                step_run[i].run_status = 0xff;
            }
        }

        STEP_STATUS_SETTING_FLAG = 0x00;
    }

}

void Step_Run_Switch(uint8_t run_ch, uint16_t run_number)
{
    // uint32_t new_address;
    // uint16_t new_number;

    // new_number = run_number++;
    // new_address = (run_ch-1) * 16 * 4096 + (new_number-1) * 256;

    Step_Data_Read(run_ch, run_number+1);
    Step_Run_Execute(run_ch);

    for(int i = 0; i < CHANNELS; i++) {
        if(run_ch & (0x01 << i)) {
            last_record_condition[i].record_current = 0;
            last_record_condition[i].record_voltage = 0;
            last_record_condition[i].record_time = 0;

            this_record_condition[i].record_current = 0;
            this_record_condition[i].record_voltage = 0;
            this_record_condition[i].record_time = 0;

            fluctuation_voltage_flag[i] = 0;
            fluctuation_current_flag[i] = 0;

            fluctuation_protection_old[i].voltage = 0;
            fluctuation_protection_old[i].voltage_time = 0;
            fluctuation_protection_old[i].current = 0;
            fluctuation_protection_old[i].current_time = 0;

            fluctuation_protection_now[i].voltage = 0;
            fluctuation_protection_now[i].voltage_time = 0;
            fluctuation_protection_now[i].current = 0;
            fluctuation_protection_now[i].current_time = 0;

            jump_protection_old[i].current = 0;
            jump_protection_old[i].voltage = 0;
            jump_protection_old[i].current_positive_jump_number = 0;
            jump_protection_old[i].current_negative_jump_number = 0;
            jump_protection_old[i].voltage_positive_jump_number = 0;
            jump_protection_old[i].voltage_negative_jump_number = 0;

            jump_protection_now[i].current = 0;
            jump_protection_now[i].voltage = 0;
            jump_protection_now[i].current_positive_jump_number = 0;
            jump_protection_now[i].current_negative_jump_number = 0;
            jump_protection_now[i].voltage_positive_jump_number = 0;
            jump_protection_now[i].voltage_negative_jump_number = 0;

            record_data.channel_record_data[i].battery_capacitance = 0;
            record_data.channel_record_data[i].battery_energy = 0;
            record_data.channel_record_data[i].crosscurrent_ratio = 0;

            CV_mode[i] = 0;
            DCV_mode[i] = 0;

            timing_voltage_time_flag[i] = 0x00;
            timing_voltage_change_time1_flag[i] = 0x00;
            timing_voltage_change_time2_flag[i] = 0x00;

            // memset(Global_Protection_Flag[i], 0, global_protection_trigger_end);
            // memset(Step_Protection_Flag[i], 0, step_protection_trigger_end);

            running_step[i].type = step_work[i].step_type;
            running_step[i].number = step_work[i].step_number;
            running_step[i].sum_number += 1;
            running_step[i].run_time = 0;
            running_step[i].protection_time = 0;

            if(running_step[i].type == end_step) {
                running_step[i].status = step_complete_status;
                Record_Report(0x01<<i);
                led_run_setting((0x01<<i), led_normal, &(led_control));

                running_step[i].status = step_standby_status;
            } else if((running_step[i].type < standby) || (running_step[i].type > end_step)) {
                running_step[i].status = step_stop_status;
                Record_Report(0x01<<i);

                led_run_setting((0x01<<i), led_normal, &(led_control));
            }
        }
    }


}

void Running_Switch(void)
{
    for(int i = 0; i < CHANNELS; i++) {
        if((running_step[i].status == step_running_status) && (running_step[i].protection_time >= 1000)) {
            //根据不同的运行类型进行不同的判断，判断工步是否运行完成,进行工步切换
            switch(running_step[i].type) {
                case standby:
                {
                    if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000.0) >= step_work[i].step_abort_time)) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }

                    break;
                }
                case CC:
                {
                #if STEP_CC_step_abort_priority
                    if(step_work[i].step_abort_priority == step_time) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage >= step_work[i].step_abort_voltage)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_voltage_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_voltage) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage >= step_work[i].step_abort_voltage)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_capacitance) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage >= step_work[i].step_abort_voltage)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_voltage_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        }
                    }
                #else
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage >= step_work[i].step_abort_voltage)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }
                #endif

                    break;
                }
                case CV:
                {
                #if STEP_CV_step_abort_priority
                    if(step_work[i].step_abort_priority == step_time) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_current_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_current) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_capacitance) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_current_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        }
                    }
                #else
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }
                #endif

                    break;
                }
                case CC_to_CV:
                {
                #if STEP_CC_to_CV_step_abort_priority
                    if(step_work[i].step_abort_priority == step_time) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_current_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_current) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_capacitance) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_current_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        }
                    }
                #else
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }
                #endif

                    break;
                }
                case CP:
                {
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage >= step_work[i].step_abort_voltage)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }

                    break;
                }
                case DCC:
                {
                #if STEP_DCC_step_abort_priority
                    if(step_work[i].step_abort_priority == step_time) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage <= step_work[i].step_abort_voltage)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_voltage_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_voltage) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage <= step_work[i].step_abort_voltage)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_capacitance) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage <= step_work[i].step_abort_voltage)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_voltage_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        }
                    }
                #else
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage <= step_work[i].step_abort_voltage)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }
                #endif

                    break;
                }
                case DCV:
                {
                #if STEP_DCV_step_abort_priority
                    if(step_work[i].step_abort_priority == step_time) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_current_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_current) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_capacitance) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_current_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        }
                    }
                #else
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) &&((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }
                #endif

                    break;
                }
                case DCC_to_DCV:
                {
                #if STEP_DCC_to_DCV_step_abort_priority
                    if(step_work[i].step_abort_priority == step_time) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_current_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_current) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_capacitance_abnormal_trigger] = 0x01;
                        }
                    } else if(step_work[i].step_abort_priority == step_capacitance) {
                        if((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_time_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) {
                            Protection_Action((0x01 << i), step_protection_flag, step_stop_status);
                            Step_Protection_Flag[i][step_abort_current_abnormal_trigger] = 0x01;
                        } else if((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance)) {
                            Record_Report(0x01 << i);
                            Step_Run_Switch((0x01<<i), running_step[i].number);
                        }
                    }
                #else
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }
                #endif

                    break;
                }
                case DCP:
                {
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage <= step_work[i].step_abort_voltage)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }

                    break;
                }
                default:
                {
                    break;
                }
            }
        } 
    #if GLOBAL_contact_voltage_variation
        else if(running_step[i].status == step_contact_status) {
            if((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time) {
                contact_now_voltage[i] = record_data.channel_record_data[i].battery_voltage;
                if((contact_now_voltage[i]-contact_old_voltage[i]) > global_protection[i].contact_voltage_variation) {
                    Step_Turn_Off(0x01<<i);
                    // running_step[i].status = step_standby_status;
                    step_run[i].run_status = step_start_setting;
                    STEP_STATUS_SETTING_FLAG |= (0x01<<i);
                } else {
                    Protection_Action((0x01<<i), global_protection_flag, step_stop_status);
                    Global_Protection_Flag[i][contact_voltage_variation_trigger] = 0x01;
                }
            }
        }
    #endif
    }

}

void Step_Turn_Off(uint8_t ch)
{
#if CHANNELS_1
    if(ch & 0x01) {
        DAC80508_SetDacData(0, 0, 1);
        DAC80508_SetDacData(1, 0, 1);
        DAC80508_SetDacData(2, 0, 1);
        DAC80508_SetDacData(3, 0, 1);
        DAC80508_SetDacData(4, 0, 1);
        DAC80508_SetDacData(5, 0, 1);
        DAC80508_SetDacData(6, 0, 1);
        DAC80508_SetDacData(7, 0, 1);
        HAL_Delay(20);

        PORT1_DIS;PORT2_DIS;PORT3_DIS;PORT4_DIS;
        PORT5_DIS;PORT6_DIS;PORT7_DIS;PORT8_DIS;
        HAL_Delay(10);
        REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
        REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
        HAL_Delay(10);

        // DAC80508_SetDacData(0, 0, 0);
        // DAC80508_SetDacData(0, 0, 1);
        // DAC80508_SetDacData(1, 0, 0);
        // DAC80508_SetDacData(1, 0, 1);
        // DAC80508_SetDacData(2, 0, 0);
        // DAC80508_SetDacData(2, 0, 1);
        // DAC80508_SetDacData(3, 0, 0);
        // DAC80508_SetDacData(3, 0, 1);
        // DAC80508_SetDacData(4, 0, 0);
        // DAC80508_SetDacData(4, 0, 1);
        // DAC80508_SetDacData(5, 0, 0);
        // DAC80508_SetDacData(5, 0, 1);
        // DAC80508_SetDacData(6, 0, 0);
        // DAC80508_SetDacData(6, 0, 1);
        // DAC80508_SetDacData(7, 0, 0);
        // DAC80508_SetDacData(7, 0, 1);
    }
#endif
#if CHANNELS_2
    for(int i = 0; i < CHANNELS;i++) {
        if(ch & (0x01<<i)) {
            DAC80508_SetDacData(i*4, 0, 1);
            DAC80508_SetDacData(i*4+1, 0, 1);
            DAC80508_SetDacData(i*4+2, 0, 1);
            DAC80508_SetDacData(i*4+3, 0, 1);
            HAL_Delay(20);

            PORT_DIS(i*4);PORT_DIS(i*4+1);PORT_DIS(i*4+2);PORT_DIS(i*4+3);
            HAL_Delay(10);
            REV_PORT_DIS(i*4);REV_PORT_DIS(i*4+1);REV_PORT_DIS(i*4+2);REV_PORT_DIS(i*4+3);
            HAL_Delay(10);

            // DAC80508_SetDacData(i*4, 0, 0);
            // DAC80508_SetDacData(i*4, 0, 1);
            // DAC80508_SetDacData(i*4+1, 0, 0);
            // DAC80508_SetDacData(i*4+1, 0, 1);
            // DAC80508_SetDacData(i*4+2, 0, 0);
            // DAC80508_SetDacData(i*4+2, 0, 1);
            // DAC80508_SetDacData(i*4+3, 0, 0);
            // DAC80508_SetDacData(i*4+3, 0, 1);
        }
    }
#endif
#if CHANNELS_4
    for(int i = 0; i < CHANNELS;i++) {
        if(ch & (0x01<<i)) {
            DAC80508_SetDacData(i*2, 0, 1);
            DAC80508_SetDacData(i*2+1, 0, 1);
            HAL_Delay(20);

            PORT_DIS(i*2);PORT_DIS(i*2+1);
            HAL_Delay(10);
            REV_PORT_DIS(i*2);REV_PORT_DIS(i*2+1);
            HAL_Delay(10);

            // DAC80508_SetDacData(i*2, 0, 0);
            // DAC80508_SetDacData(i*2, 0, 1);
            // DAC80508_SetDacData(i*2+1, 0, 0);
            // DAC80508_SetDacData(i*2+1, 0, 1);
        }
    }
#endif
#if CHANNELS_8
    for(int i = 0; i < CHANNELS;i++) {
        if(ch & (0x01<<i)) {
            DAC80508_SetDacData(i, 0, 1);
            HAL_Delay(20);

            PORT_DIS(i);
            HAL_Delay(10);
            REV_PORT_DIS(i);
            HAL_Delay(10);

            // DAC80508_SetDacData(i, 0, 0);
            // DAC80508_SetDacData(i, 0, 1);
        }
    }
#endif
}
