#include "main.h"
#include "step_work.h"
#include "data_analysis.h"
#include <string.h>
#include "discharge.h"


#if CHANNELS_1
    RunningStep running_step[CHANNELS];
#endif
#if CHANNELS_2
    RunningStep running_step[CHANNELS];
#endif
#if CHANNELS_4
    // uint8_t  running_step_type[CHANNELS];
    // uint8_t  running_step_status[CHANNELS];
    // uint16_t running_step_number[CHANNELS];
    // uint16_t running_step_number_sum[CHANNELS] = {0};
    // uint32_t running_step_time[CHANNELS] = {0};

    RunningStep running_step[CHANNELS] = {  [0].status = step_stop_status, [1].status = step_stop_status, [2].status = step_stop_status, [3].status = step_stop_status,
                                            [0].type = 0, [1].type = 0, [2].type = 0, [3].type = 0,
                                            [0].number = 0, [1].number = 0, [2].number = 0, [3].number = 0,
                                            [0].sum_number = 0, [1].sum_number = 0, [2].sum_number = 0, [3].sum_number = 0,
                                            [0].run_time = 0, [1].run_time = 0, [2].run_time = 0, [3].run_time = 0,};
    StepRecordCondition last_record_condition[CHANNELS];
    StepRecordCondition this_record_condition[CHANNELS];
    RunningCircle running_circle[CHANNELS] = {  [0].circle_serial = 0, [0].circle_number = 0, [0].circle_count = 0,
                                                [1].circle_serial = 0, [1].circle_number = 0, [1].circle_count = 0,
                                                [2].circle_serial = 0, [2].circle_number = 0, [2].circle_count = 0,
                                                [3].circle_serial = 0, [3].circle_number = 0, [3].circle_count = 0,};
    // RecordData record_data[CHANNELS];
    RecordData record_data;
#endif
#if CHANNELS_8
    RunningStep running_step[CHANNELS];
#endif

// SampleData accumulated_data_1ms[8] = {  [0].v_sense = 0.0, [0].i_out = 0.0, [0].v_mod = 0.0, [0].battery_temperature = 0.0, [0].mod_tempetature = 0.0, [0].v_j = 0.0,
//                                         [1].v_sense = 0.0, [1].i_out = 0.0, [1].v_mod = 0.0, [1].battery_temperature = 0.0, [1].mod_tempetature = 0.0, [1].v_j = 0.0,
//                                         [2].v_sense = 0.0, [2].i_out = 0.0, [2].v_mod = 0.0, [2].battery_temperature = 0.0, [2].mod_tempetature = 0.0, [2].v_j = 0.0,
//                                         [3].v_sense = 0.0, [3].i_out = 0.0, [3].v_mod = 0.0, [3].battery_temperature = 0.0, [3].mod_tempetature = 0.0, [3].v_j = 0.0,
//                                         [4].v_sense = 0.0, [4].i_out = 0.0, [4].v_mod = 0.0, [4].battery_temperature = 0.0, [4].mod_tempetature = 0.0, [4].v_j = 0.0,
//                                         [5].v_sense = 0.0, [5].i_out = 0.0, [5].v_mod = 0.0, [5].battery_temperature = 0.0, [5].mod_tempetature = 0.0, [5].v_j = 0.0,
//                                         [6].v_sense = 0.0, [6].i_out = 0.0, [6].v_mod = 0.0, [6].battery_temperature = 0.0, [6].mod_tempetature = 0.0, [6].v_j = 0.0,
//                                         [7].v_sense = 0.0, [7].i_out = 0.0, [7].v_mod = 0.0, [7].battery_temperature = 0.0, [7].mod_tempetature = 0.0, [7].v_j = 0.0,};
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
        step_work[data_ch].step_work_current = 0;
        step_work[data_ch].step_work_power = 0;
        step_work[data_ch].step_abort_time = 0;
        step_work[data_ch].step_abort_voltage = 0;
        step_work[data_ch].step_abort_current = 0;
        step_work[data_ch].step_abort_capacitance = 0;

        step_record_condition[data_ch].record_time = 0;
        step_record_condition[data_ch].record_voltage = 0;
        step_record_condition[data_ch].record_current = 0;

        step_protection[data_ch].protection_battery_temperature_limit = 0;
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
        //------------------------------------------------------------------------------------------

        step_work[data_ch].step_number = number;
        step_work[data_ch].step_type = temp_data[2];
        read_len += 3;

        if(step_work[data_ch].step_type == end_step) {

        }
        // else if(step_work[data_ch].step_type == loop_begin) {
        //     memcpy(&running_circle[data_ch].circle_serial, temp_data+read_len, 2);      read_len += 2;
        //     memcpy(&running_circle[data_ch].circle_number, temp_data+read_len, 2);      read_len += 2;
        // } else if(step_work[data_ch].step_type == loop_end) {
        //     memcpy(&temp16, temp_data+read_len, 2);         read_len += 2;
        //     if(temp16 == running_circle[data_ch].circle_serial) {
        //         running_circle[data_ch].circle_end_flag = 0x01;
        //     }
        // }
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
                    case step_battery_temperature_limit: {
                        step_protection[data_ch].protection_battery_temperature_limit = ftemp;
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
                    default:
                        break;
                }
            }
            //--------------------------------------------------------------------------------------

            // //step_work-----------------------------------------------------------------------------
            // memcpy(&(step_work[data_ch].step_work_current), temp_data+read_len, 4);             read_len += 4;
            // memcpy(&temp32, &(step_work[data_ch].step_work_current), 4);
            // if(temp32 == 0xffffffff) {
            //     step_work[data_ch].step_work_current = 0;
            // }

            // memcpy(&(step_work[data_ch].step_work_voltage), temp_data+read_len, 4);             read_len += 4;
            // memcpy(&temp32, &(step_work[data_ch].step_work_voltage), 4);
            // if(temp32 == 0xffffffff) {
            //     step_work[data_ch].step_work_voltage = 0;
            // }

            // memcpy(&(step_work[data_ch].step_work_power), temp_data+read_len, 4);               read_len += 4;
            // memcpy(&temp32, &(step_work[data_ch].step_work_power), 4);
            // if(temp32 == 0xffffffff) {
            //     step_work[data_ch].step_work_power = 0;
            // }

            // memcpy(&(step_work[data_ch].step_abort_current), temp_data+read_len, 4);            read_len += 4;
            // memcpy(&temp32, &(step_work[data_ch].step_abort_current), 4);
            // if(temp32 == 0xffffffff) {
            //     step_work[data_ch].step_abort_current = 0;
            // }

            // memcpy(&(step_work[data_ch].step_abort_voltage), temp_data+read_len, 4);            read_len += 4;
            // memcpy(&temp32, &(step_work[data_ch].step_abort_voltage), 4);
            // if(temp32 == 0xffffffff) {
            //     step_work[data_ch].step_abort_voltage = 0;
            // }

            // memcpy(&(step_work[data_ch].step_abort_time), temp_data+read_len, 4);               read_len += 4;
            // memcpy(&temp32, &(step_work[data_ch].step_abort_time), 4);
            // if(temp32 == 0xffffffff) {
            //     step_work[data_ch].step_abort_time = 0;
            // }

            // memcpy(&(step_work[data_ch].step_abort_capacitance), temp_data+read_len, 4);        read_len += 4;
            // memcpy(&temp32, &(step_work[data_ch].step_abort_capacitance), 4);
            // if(temp32 == 0xffffffff) {
            //     step_work[data_ch].step_abort_capacitance = 0;
            // }
            // //--------------------------------------------------------------------------------------

            // //step_record_condition-----------------------------------------------------------------
            // memcpy(&(step_record_condition[data_ch].record_current), temp_data+read_len, 4);              read_len += 4;
            // memcpy(&temp32, &(step_record_condition[data_ch].record_current), 4);
            // if(temp32 == 0xffffffff) {
            //     step_record_condition[data_ch].record_current = 0;
            // }

            // memcpy(&(step_record_condition[data_ch].record_voltage), temp_data+read_len, 4);              read_len += 4;
            // memcpy(&temp32, &(step_record_condition[data_ch].record_voltage), 4);
            // if(temp32 == 0xffffffff) {
            //     step_record_condition[data_ch].record_voltage = 0;
            // }

            // memcpy(&(step_record_condition[data_ch].record_time), temp_data+read_len, 4);                 read_len += 4;
            // memcpy(&temp32, &(step_record_condition[data_ch].record_time), 4);
            // if(temp32 == 0xffffffff) {
            //     step_record_condition[data_ch].record_time = 0;
            // }
            // //--------------------------------------------------------------------------------------

            // //step_protection-----------------------------------------------------------------------
            // memcpy(&(step_protection[data_ch].protection_battery_temperature_limit), temp_data+read_len, 4);        read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_battery_temperature_limit), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_battery_temperature_limit = 0;
            // }

            // memcpy(&(step_protection[data_ch].protection_charge_capacitance), temp_data+read_len, 4);               read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_charge_capacitance), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_charge_capacitance = 0;
            // }

            // memcpy(&(step_protection[data_ch].protection_discharge_capacitance), temp_data+read_len, 4);            read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_discharge_capacitance), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_discharge_capacitance = 0;
            // }

            // memcpy(&(step_protection[data_ch].protection_voltage_high_limit), temp_data+read_len, 4);               read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_voltage_high_limit), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_voltage_high_limit = 0;
            // }

            // memcpy(&(step_protection[data_ch].protection_voltage_low_limit), temp_data+read_len, 4);                read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_voltage_low_limit), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_voltage_low_limit = 0;
            // }

            // memcpy(&(step_protection[data_ch].protection_charge_current_limit), temp_data+read_len, 4);             read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_charge_current_limit), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_charge_current_limit = 0;
            // }

            // memcpy(&(step_protection[data_ch].protection_discharge_current_limit), temp_data+read_len, 4);          read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_discharge_current_limit), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_discharge_current_limit = 0;
            // }

            // memcpy(&(step_protection[data_ch].protection_current_positive_jump_range), temp_data+read_len, 4);      read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_current_positive_jump_range), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_current_positive_jump_range = 0;
            // }

            // memcpy(&ftemp, temp_data+read_len, 4);     read_len += 4;
            // memcpy(&temp32, &ftemp, 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_current_positive_jump_number = 0;
            // } else {
            //     step_protection[data_ch].protection_current_positive_jump_number = (uint32_t)ftemp;
            // }

            // memcpy(&(step_protection[data_ch].protection_current_negative_jump_range), temp_data+read_len, 4);      read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_current_negative_jump_range), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_current_negative_jump_range = 0;
            // }

            // memcpy(&ftemp, temp_data+read_len, 4);     read_len += 4;
            // memcpy(&temp32, &ftemp, 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_current_negative_jump_number = 0;
            // } else {
            //     step_protection[data_ch].protection_current_negative_jump_number = (uint32_t)ftemp;
            // }

            // memcpy(&(step_protection[data_ch].protection_voltage_positive_jump_range), temp_data+read_len, 4);      read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_voltage_positive_jump_range), 4);
            // if(temp32 = 0xffffffff) {
            //     step_protection[data_ch].protection_voltage_positive_jump_range = 0;
            // }

            // memcpy(&ftemp, temp_data+read_len, 4);     read_len += 4;
            // memcpy(&temp32, &ftemp, 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_voltage_positive_jump_number = 0;
            // } else {
            //     step_protection[data_ch].protection_voltage_positive_jump_number = (uint32_t)ftemp;
            // }

            // memcpy(&(step_protection[data_ch].protection_voltage_negative_jump_range), temp_data+read_len, 4);      read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_voltage_negative_jump_range), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_voltage_negative_jump_range = 0;
            // }

            // memcpy(&ftemp, temp_data+read_len, 4);     read_len += 4;
            // memcpy(&temp32, &ftemp, 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_voltage_negative_jump_number = 0;
            // } else {
            //     step_protection[data_ch].protection_voltage_negative_jump_number = (uint32_t)ftemp;
            // }

            // memcpy(&(step_protection[data_ch].protection_current_OutOfTolerance), temp_data+read_len, 4);           read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_current_OutOfTolerance), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_current_OutOfTolerance = 0;
            // }

            // memcpy(&(step_protection[data_ch].protection_voltage_OutOfTolerance), temp_data+read_len, 4);           read_len += 4;
            // memcpy(&temp32, &(step_protection[data_ch].protection_voltage_OutOfTolerance), 4);
            // if(temp32 == 0xffffffff) {
            //     step_protection[data_ch].protection_voltage_OutOfTolerance = 0;
            // }
            // //--------------------------------------------------------------------------------------

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

    //         memcpy(&(step_protection[data_ch].protection_battery_temperature_limit), temp_data+read_len, 4);        read_len += 4;
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

#endif
#if CHANNELS_2

#endif
#if CHANNELS_4
    if(ch & CH_1) {
        if(step_work[0].step_type == standby) {             //静置
            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(0, 0, 0);
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 0);
            DAC80508_SetDacData(1, 0, 1);
        } else if(step_work[0].step_type == CC) {           //恒流充电
            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BUCK_MODE, V_MAX, step_work[0].step_work_current/2);
            Discharge_Control(DAC80508_CH2, BUCK_MODE, V_MAX, step_work[0].step_work_current/2);

            REV_PORT1_EN;REV_PORT2_EN;
            HAL_Delay(20);
            PORT1_EN;PORT2_EN;
        } else if(step_work[0].step_type == CV) {           //恒压充电
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
            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/2);
            Discharge_Control(DAC80508_CH2, BUCK_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/2);

            REV_PORT1_EN;REV_PORT2_EN;
            HAL_Delay(20);
            PORT1_EN;PORT2_EN;
        } else if(step_work[0].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[0].step_type == DCC) {          //恒流放电
            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BOOST_MODE, V_MIN, step_work[0].step_work_current/2);
            Discharge_Control(DAC80508_CH2, BOOST_MODE, V_MIN, step_work[0].step_work_current/2);

            REV_PORT1_EN;REV_PORT2_EN;
            HAL_Delay(20);
            PORT1_EN;PORT2_EN;
        } else if(step_work[0].step_type == DCV) {          //恒压放电
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
            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH1, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/2);
            Discharge_Control(DAC80508_CH2, BOOST_MODE, step_work[0].step_work_voltage, step_work[0].step_work_current/2);

            REV_PORT1_EN;REV_PORT2_EN;
            HAL_Delay(20);
            PORT1_EN;PORT2_EN;
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
        // } else if(step_work[0].step_type == loop_end) {     //循环模式结束
        //     if(running_circle[0].circle_end_flag == 0x01) {
        //         running_circle[0].circle_end_flag = 0x00;

        //         if(running_circle[0].circle_count < running_circle[0].circle_number) {
        //             Step_Data_Read(CH_1, running_circle[0].circle_start+1);
        //             Step_Run_Execute(CH_1);
        //             running_circle[0].circle_count += 1;
        //         } else {
        //             running_circle[0].circle_start = 0;
        //             running_circle[0].circle_serial = 0;
        //             running_circle[0].circle_number = 0;
        //             running_circle[0].circle_count = 0;
        //             running_circle[0].circle_end_flag = 0;
        //         }
        //     }
        // }
        else if(step_work[0].step_type == end_step) {     //工步结束
            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(0, 0, 0);
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 0);
            DAC80508_SetDacData(1, 0, 1);

            //执行结束相关的动作
        } else {
            PORT1_DIS;PORT2_DIS;
            HAL_Delay(10);
            REV_PORT1_DIS;REV_PORT2_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(0, 0, 0);
            DAC80508_SetDacData(0, 0, 1);
            DAC80508_SetDacData(1, 0, 0);
            DAC80508_SetDacData(1, 0, 1);
        }
    } else if(ch & CH_2) {
        if(step_work[1].step_type == standby) {             //静置
            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(2, 0, 0);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 0);
            DAC80508_SetDacData(3, 0, 1);
        } else if(step_work[1].step_type == CC) {           //恒流充电
            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            // //test----------------------------------------------------------------------------------
            // V3_MOD;V4_MOD
            // V_number = (uint16_t)(record_data.channel_record_data[1].battery_voltage * calibration[2].Buck_Vset_mod_K + calibration[2].Buck_Vset_mod_B);
            // I_number = (uint16_t)(1 * calibration[0].Buck_Iset_LK + calibration[0].Buck_Iset_LB);
            // DAC80508_SetDacData(2, V_number, 0);
            // DAC80508_SetDacData(2, I_number, 1);
            // V_number = (uint16_t)(record_data.channel_record_data[1].battery_voltage * calibration[3].Buck_Vset_mod_K + calibration[3].Buck_Vset_mod_B);
            // DAC80508_SetDacData(3, V_number, 0);
            // DAC80508_SetDacData(3, I_number, 1);
            // PORT3_EN;PORT4_EN;
            // HAL_Delay(1000);
            // REV_PORT3_EN;REV_PORT4_EN;
            // HAL_Delay(10);
            // PORT3_DIS;PORT4_DIS;
            // V3_SENSE;V4_SENSE;
            // //--------------------------------------------------------------------------------------

            Discharge_Control(DAC80508_CH3, BUCK_MODE, V_MAX, step_work[1].step_work_current/2);
            Discharge_Control(DAC80508_CH4, BUCK_MODE, V_MAX, step_work[1].step_work_current/2);

            REV_PORT3_EN;REV_PORT4_EN;
            HAL_Delay(20);
            PORT3_EN;PORT4_EN;
        } else if(step_work[1].step_type == CV) {           //恒压充电
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
            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH3, BUCK_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/2);
            Discharge_Control(DAC80508_CH4, BUCK_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/2);

            REV_PORT3_EN;REV_PORT4_EN;
            HAL_Delay(20);
            PORT3_EN;PORT4_EN;
        } else if(step_work[1].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[1].step_type == DCC) {          //恒流放电
            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH3, BOOST_MODE, V_MIN, step_work[1].step_work_current/2);
            Discharge_Control(DAC80508_CH4, BOOST_MODE, V_MIN, step_work[1].step_work_current/2);

            REV_PORT3_EN;REV_PORT4_EN;
            HAL_Delay(20);
            PORT3_EN;PORT4_EN;
        } else if(step_work[1].step_type == DCV) {          //恒压放电
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
            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH3, BOOST_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/2);
            Discharge_Control(DAC80508_CH4, BOOST_MODE, step_work[1].step_work_voltage, step_work[1].step_work_current/2);

            REV_PORT3_EN;REV_PORT4_EN;
            HAL_Delay(20);
            PORT3_EN;PORT4_EN;
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
        // } else if(step_work[1].step_type == loop_end) {     //循环模式结束
        //     if(running_circle[1].circle_end_flag == 0x01) {
        //         running_circle[1].circle_end_flag = 0x00;

        //         if(running_circle[1].circle_count < running_circle[1].circle_number) {
        //             Step_Data_Read(CH_2, running_circle[1].circle_start+1);
        //             Step_Run_Execute(CH_2);
        //             running_circle[1].circle_count += 1;
        //         } else {
        //             running_circle[1].circle_start = 0;
        //             running_circle[1].circle_serial = 0;
        //             running_circle[1].circle_number = 0;
        //             running_circle[1].circle_count = 0;
        //             running_circle[1].circle_end_flag = 0;
        //         }
        //     }
        // }
        else if(step_work[1].step_type == end_step) {     //工步结束
            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(2, 0, 0);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 0);
            DAC80508_SetDacData(3, 0, 1);

            //执行结束相关的动作
        } else {
            PORT3_DIS;PORT4_DIS;
            HAL_Delay(10);
            REV_PORT3_DIS;REV_PORT4_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(2, 0, 0);
            DAC80508_SetDacData(2, 0, 1);
            DAC80508_SetDacData(3, 0, 0);
            DAC80508_SetDacData(3, 0, 1);
        }
    } else if(ch & CH_3) {
        if(step_work[2].step_type == standby) {             //静置
            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(4, 0, 0);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 0);
            DAC80508_SetDacData(5, 0, 1);
        } else if(step_work[2].step_type == CC) {           //恒流充电
            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BUCK_MODE, V_MAX, step_work[2].step_work_current/2);
            Discharge_Control(DAC80508_CH6, BUCK_MODE, V_MAX, step_work[2].step_work_current/2);

            REV_PORT5_EN;REV_PORT6_EN;
            HAL_Delay(20);
            PORT5_EN;PORT6_EN;
        } else if(step_work[2].step_type == CV) {           //恒压充电
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
            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BUCK_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current/2);
            Discharge_Control(DAC80508_CH6, BUCK_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current/2);

            REV_PORT5_EN;REV_PORT6_EN;
            HAL_Delay(20);
            PORT5_EN;PORT6_EN;
        } else if(step_work[2].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[2].step_type == DCC) {          //恒流放电
            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BOOST_MODE, V_MIN, step_work[2].step_work_current/2);
            Discharge_Control(DAC80508_CH6, BOOST_MODE, V_MIN, step_work[2].step_work_current/2);

            REV_PORT5_EN;REV_PORT6_EN;
            HAL_Delay(20);
            PORT5_EN;PORT6_EN;
        } else if(step_work[2].step_type == DCV) {          //恒压放电
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
            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH5, BOOST_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current/2);
            Discharge_Control(DAC80508_CH6, BOOST_MODE, step_work[2].step_work_voltage, step_work[2].step_work_current/2);

            REV_PORT5_EN;REV_PORT6_EN;
            HAL_Delay(20);
            PORT5_EN;PORT6_EN;
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
        // } else if(step_work[2].step_type == loop_end) {     //循环模式结束
        //     if(running_circle[2].circle_end_flag == 0x01) {
        //         running_circle[2].circle_end_flag = 0x00;

        //         if(running_circle[2].circle_count < running_circle[2].circle_number) {
        //             Step_Data_Read(CH_3, running_circle[2].circle_start+1);
        //             Step_Run_Execute(CH_3);
        //             running_circle[2].circle_count += 1;
        //         } else {
        //             running_circle[2].circle_start = 0;
        //             running_circle[2].circle_serial = 0;
        //             running_circle[2].circle_number = 0;
        //             running_circle[2].circle_count = 0;
        //             running_circle[2].circle_end_flag = 0;
        //         }
        //     }
        // }
        else if(step_work[2].step_type == end_step) {     //工步结束
            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(4, 0, 0);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 0);
            DAC80508_SetDacData(5, 0, 1);

            //执行结束相关的动作
        } else {
            PORT5_DIS;PORT6_DIS;
            HAL_Delay(10);
            REV_PORT5_DIS;REV_PORT6_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(4, 0, 0);
            DAC80508_SetDacData(4, 0, 1);
            DAC80508_SetDacData(5, 0, 0);
            DAC80508_SetDacData(5, 0, 1);
        }
    } else if(ch & CH_4) {
        if(step_work[3].step_type == standby) {             //静置
            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(6, 0, 0);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 0);
            DAC80508_SetDacData(7, 0, 1);
        } else if(step_work[3].step_type == CC) {           //恒流充电
            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BUCK_MODE, V_MAX, step_work[3].step_work_current/2);
            Discharge_Control(DAC80508_CH8, BUCK_MODE, V_MAX, step_work[3].step_work_current/2);

            REV_PORT7_EN;REV_PORT8_EN;
            HAL_Delay(20);
            PORT7_EN;PORT8_EN;
        } else if(step_work[3].step_type == CV) {           //恒压充电
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
            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BUCK_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current/2);
            Discharge_Control(DAC80508_CH8, BUCK_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current/2);

            REV_PORT7_EN;REV_PORT8_EN;
            HAL_Delay(20);
            PORT7_EN;PORT8_EN;
        } else if(step_work[3].step_type == CP) {           //恒功率充电
            //进行采集计算相应电流、电压
            /*
            code...
            */
        } else if(step_work[3].step_type == DCC) {          //恒流放电
            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BOOST_MODE, V_MIN, step_work[3].step_work_current/2);
            Discharge_Control(DAC80508_CH8, BOOST_MODE, V_MIN, step_work[3].step_work_current/2);

            REV_PORT7_EN;REV_PORT8_EN;
            HAL_Delay(20);
            PORT7_EN;PORT8_EN;
        } else if(step_work[3].step_type == DCV) {          //恒压放电
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
            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            Discharge_Control(DAC80508_CH7, BOOST_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current/2);
            Discharge_Control(DAC80508_CH8, BOOST_MODE, step_work[3].step_work_voltage, step_work[3].step_work_current/2);

            REV_PORT7_EN;REV_PORT8_EN;
            HAL_Delay(20);
            PORT7_EN;PORT8_EN;
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
        // } else if(step_work[3].step_type == loop_end) {     //循环模式结束
        //     if(running_circle[3].circle_end_flag == 0x01) {
        //         running_circle[3].circle_end_flag = 0x00;

        //         if(running_circle[3].circle_count < running_circle[3].circle_number) {
        //             Step_Data_Read(CH_4, running_circle[3].circle_start+1);
        //             Step_Run_Execute(CH_4);
        //             running_circle[3].circle_count += 1;
        //         } else {
        //             running_circle[3].circle_start = 0;
        //             running_circle[3].circle_serial = 0;
        //             running_circle[3].circle_number = 0;
        //             running_circle[3].circle_count = 0;
        //             running_circle[3].circle_end_flag = 0;
        //         }
        //     }
        // }
        else if(step_work[3].step_type == end_step) {     //工步结束
            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(6, 0, 0);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 0);
            DAC80508_SetDacData(7, 0, 1);

            //执行结束相关的动作
        } else {
            PORT7_DIS;PORT8_DIS;
            HAL_Delay(10);
            REV_PORT7_DIS;REV_PORT8_DIS;
            HAL_Delay(10);

            DAC80508_SetDacData(6, 0, 0);
            DAC80508_SetDacData(6, 0, 1);
            DAC80508_SetDacData(7, 0, 0);
            DAC80508_SetDacData(7, 0, 1);
        }
    }
#endif
#if CHANNELS_8

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

                        CV_mode[i] = 0;
                        DCV_mode[i] = 0;

                        memset(Global_Protection_Flag[i], 0, global_protection_trigger_end);
                        memset(Step_Protection_Flag[i], 0, step_protection_trigger_end);

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
                        break;
                    }
                    default: {
                        break;
                    }
                }

            }
        }

        STEP_STATUS_SETTING_FLAG = 0x00;
    }

// #if CHANNELS_1

// #endif
// #if CHANNELS_2

// #endif
// #if CHANNELS_4
//     if(STEP_STATUS_SETTING_FLAG) {
//         if(STEP_STATUS_SETTING_FLAG & CH_1) {
//             if(step_run[0].run_status == step_start_setting) {
//                 led_run_setting(0, led_normal, &(led_control));

//                 last_record_condition[0].record_current = 0;
//                 last_record_condition[0].record_voltage = 0;
//                 last_record_condition[0].record_time = 0;

//                 this_record_condition[0].record_current = 0;
//                 this_record_condition[0].record_voltage = 0;
//                 this_record_condition[0].record_time = 0;

//                 fluctuation_voltage_flag[0] = 0;
//                 fluctuation_current_flag[0] = 0;

//                 fluctuation_protection_old[0].voltage = 0;
//                 fluctuation_protection_old[0].voltage_time = 0;
//                 fluctuation_protection_old[0].current = 0;
//                 fluctuation_protection_old[0].current_time = 0;

//                 fluctuation_protection_now[0].voltage = 0;
//                 fluctuation_protection_now[0].voltage_time = 0;
//                 fluctuation_protection_now[0].current = 0;
//                 fluctuation_protection_now[0].current_time = 0;

//                 jump_protection_old[0].current = 0;
//                 jump_protection_old[0].voltage = 0;
//                 jump_protection_old[0].current_positive_jump_number = 0;
//                 jump_protection_old[0].current_negative_jump_number = 0;
//                 jump_protection_old[0].voltage_positive_jump_number = 0;
//                 jump_protection_old[0].voltage_negative_jump_number = 0;

//                 jump_protection_now[0].current = 0;
//                 jump_protection_now[0].voltage = 0;
//                 jump_protection_now[0].current_positive_jump_number = 0;
//                 jump_protection_now[0].current_negative_jump_number = 0;
//                 jump_protection_now[0].voltage_positive_jump_number = 0;
//                 jump_protection_now[0].voltage_negative_jump_number = 0;

//                 PG_count[0] = 0;

//                 record_data.channel_record_data[0].battery_capacitance = 0;
//                 record_data.channel_record_data[0].battery_energy = 0;
//                 record_data.channel_record_data[0].crosscurrent_ratio = 0;

//                 CV_mode[0] = 0;
//                 DCV_mode[0] = 0;

//                 memset(Global_Protection_Flag[0], 0, global_protection_trigger_end);
//                 memset(Step_Protection_Flag[0], 0, step_protection_trigger_end);

//                 Step_Data_Read(CH_1, step_run[0].run_number);
//                 Step_Run_Execute(CH_1);

//                 // running_step[0].status = step_running_status;
//                 running_step[0].type = step_work[0].step_type;
//                 running_step[0].number = step_work[0].step_number;
//                 running_step[0].sum_number = 1;
//                 running_step[0].run_time = 0;
//                 running_step[0].protection_time = 0;

//                 if(running_step[0].type == end_step) {
//                     running_step[0].status = step_complete_status;
//                     Record_Report(0x01);
//                 } else if((running_step[0].type < standby) || (running_step[0].type > end_step)) {
//                     running_step[0].status = step_stop_status;
//                     Record_Report(0x01);
//                 } else {
//                     running_step[0].status = step_running_status;
//                 }

//             } else if(step_run[0].run_status == step_stop_setting) {
//                 led_run_setting(CH_1, led_normal, &(led_control));

//                 //执行停止相关动作
//                 PORT1_DIS;PORT2_DIS;
//                 HAL_Delay(10);
//                 REV_PORT1_DIS;REV_PORT2_DIS;
//                 HAL_Delay(10);

//                 DAC80508_SetDacData(0, 0, 0);
//                 DAC80508_SetDacData(0, 0, 1);
//                 DAC80508_SetDacData(1, 0, 0);
//                 DAC80508_SetDacData(1, 0, 1);

//                 running_step[0].status = step_stop_status;
//                 Record_Report(0x01);
//             } else if(step_run[0].run_status == step_pause_setting) {
//                 led_run_setting(CH_1, led_normal, &(led_control));

//                 //执行暂停相关动作
//                 PORT1_DIS;PORT2_DIS;
//                 HAL_Delay(10);
//                 REV_PORT1_DIS;REV_PORT2_DIS;
//                 HAL_Delay(10);

//                 DAC80508_SetDacData(0, 0, 0);
//                 DAC80508_SetDacData(0, 0, 1);
//                 DAC80508_SetDacData(1, 0, 0);
//                 DAC80508_SetDacData(1, 0, 1);

//                 fluctuation_voltage_flag[0] = 0;
//                 fluctuation_current_flag[0] = 0;

//                 running_step[0].status = step_pause_status;
//                 running_step[0].protection_time = 0;
//                 Record_Report(0x01);
//             } else if(step_run[0].run_status == step_continue_setting) {
//                 //执行继续运行相关动作
//                 if(running_step[0].status == step_pause_status) {
//                     // PORT1_EN;
//                     // PORT2_EN;
//                     Step_Run_Execute(CH_1);

//                     running_step[0].status = step_running_status;
//                     Record_Report(0x01);
//                 }

//             } else if(step_run[0].run_status == step_jump_setting) {
//                 //执行跳转相关动作

//             }
//         }
//         if(STEP_STATUS_SETTING_FLAG & CH_2) {
//             if(step_run[1].run_status == step_start_setting) {
//                 led_run_setting(0, led_normal, &(led_control));

//                 last_record_condition[1].record_current = 0;
//                 last_record_condition[1].record_voltage = 0;
//                 last_record_condition[1].record_time = 0;

//                 this_record_condition[1].record_current = 0;
//                 this_record_condition[1].record_voltage = 0;
//                 this_record_condition[1].record_time = 0;

//                 fluctuation_voltage_flag[1] = 0;
//                 fluctuation_current_flag[1] = 0;

//                 fluctuation_protection_old[1].voltage = 0;
//                 fluctuation_protection_old[1].voltage_time = 0;
//                 fluctuation_protection_old[1].current = 0;
//                 fluctuation_protection_old[1].current_time = 0;

//                 fluctuation_protection_now[1].voltage = 0;
//                 fluctuation_protection_now[1].voltage_time = 0;
//                 fluctuation_protection_now[1].current = 0;
//                 fluctuation_protection_now[1].current_time = 0;

//                 jump_protection_old[1].current = 0;
//                 jump_protection_old[1].voltage = 0;
//                 jump_protection_old[1].current_positive_jump_number = 0;
//                 jump_protection_old[1].current_negative_jump_number = 0;
//                 jump_protection_old[1].voltage_positive_jump_number = 0;
//                 jump_protection_old[1].voltage_negative_jump_number = 0;

//                 jump_protection_now[1].current = 0;
//                 jump_protection_now[1].voltage = 0;
//                 jump_protection_now[1].current_positive_jump_number = 0;
//                 jump_protection_now[1].current_negative_jump_number = 0;
//                 jump_protection_now[1].voltage_positive_jump_number = 0;
//                 jump_protection_now[1].voltage_negative_jump_number = 0;

//                 PG_count[1] = 0;

//                 record_data.channel_record_data[1].battery_capacitance = 0;
//                 record_data.channel_record_data[1].battery_energy = 0;
//                 record_data.channel_record_data[1].crosscurrent_ratio = 0;

//                 CV_mode[1] = 0;
//                 DCV_mode[1] = 0;

//                 memset(Global_Protection_Flag[1], 0, global_protection_trigger_end);
//                 memset(Step_Protection_Flag[1], 0, step_protection_trigger_end);

//                 Step_Data_Read(CH_2, step_run[1].run_number);
//                 Step_Run_Execute(CH_2);

//                 // running_step[1].status = step_running_status;
//                 running_step[1].type = step_work[1].step_type;
//                 running_step[1].number = step_work[1].step_number;
//                 running_step[1].sum_number = 1;
//                 running_step[1].run_time = 0;
//                 running_step[1].protection_time = 0;

//                 if(running_step[1].type == end_step) {
//                     running_step[1].status = step_complete_status;
//                     Record_Report(0x02);
//                 } else if((running_step[1].type < standby) || (running_step[1].type > end_step)) {
//                     running_step[1].status = step_stop_status;
//                     Record_Report(0x02);
//                 } else {
//                     running_step[1].status = step_running_status;
//                 }
//             } else if(step_run[1].run_status == step_stop_setting) {
//                 led_run_setting(CH_2, led_normal, &(led_control));

//                 //执行停止相关动作
//                 PORT3_DIS;PORT4_DIS;
//                 HAL_Delay(10);
//                 REV_PORT3_DIS;REV_PORT4_DIS;
//                 HAL_Delay(10);

//                 DAC80508_SetDacData(2, 0, 0);
//                 DAC80508_SetDacData(2, 0, 1);
//                 DAC80508_SetDacData(3, 0, 0);
//                 DAC80508_SetDacData(3, 0, 1);

//                 running_step[1].status = step_stop_status;
//                 Record_Report(0x02);
//             } else if(step_run[1].run_status == step_pause_setting) {
//                 led_run_setting(CH_2, led_normal, &(led_control));

//                 //执行暂停相关动作
//                 PORT3_DIS;PORT4_DIS;
//                 HAL_Delay(10);
//                 REV_PORT3_DIS;REV_PORT4_DIS;
//                 HAL_Delay(10);

//                 DAC80508_SetDacData(2, 0, 0);
//                 DAC80508_SetDacData(2, 0, 1);
//                 DAC80508_SetDacData(3, 0, 0);
//                 DAC80508_SetDacData(3, 0, 1);

//                 fluctuation_voltage_flag[1] = 0;
//                 fluctuation_current_flag[1] = 0;

//                 running_step[1].status = step_pause_status;
//                 running_step[1].protection_time = 0;
//                 Record_Report(0x02);
//             } else if(step_run[1].run_status == step_continue_setting) {
//                 //执行继续运行相关动作
//                 if(running_step[1].status == step_pause_status) {
//                     // PORT3_EN;
//                     // PORT4_EN;
//                     Step_Run_Execute(CH_2);

//                     running_step[1].status = step_running_status;
//                     Record_Report(0x02);
//                 }
//             } else if(step_run[1].run_status == step_jump_setting) {
//                 //执行跳转相关动作
//                 /*
//                 code
//                 */
//             }
//         }
//         if(STEP_STATUS_SETTING_FLAG & CH_3) {
//             if(step_run[2].run_status == step_start_setting) {
//                 led_run_setting(0, led_normal, &(led_control));

//                 last_record_condition[2].record_current = 0;
//                 last_record_condition[2].record_voltage = 0;
//                 last_record_condition[2].record_time = 0;

//                 this_record_condition[2].record_current = 0;
//                 this_record_condition[2].record_voltage = 0;
//                 this_record_condition[2].record_time = 0;

//                 fluctuation_voltage_flag[2] = 0;
//                 fluctuation_current_flag[2] = 0;

//                 fluctuation_protection_old[2].voltage = 0;
//                 fluctuation_protection_old[2].voltage_time = 0;
//                 fluctuation_protection_old[2].current = 0;
//                 fluctuation_protection_old[2].current_time = 0;

//                 fluctuation_protection_now[2].voltage = 0;
//                 fluctuation_protection_now[2].voltage_time = 0;
//                 fluctuation_protection_now[2].current = 0;
//                 fluctuation_protection_now[2].current_time = 0;

//                 jump_protection_old[2].current = 0;
//                 jump_protection_old[2].voltage = 0;
//                 jump_protection_old[2].current_positive_jump_number = 0;
//                 jump_protection_old[2].current_negative_jump_number = 0;
//                 jump_protection_old[2].voltage_positive_jump_number = 0;
//                 jump_protection_old[2].voltage_negative_jump_number = 0;

//                 jump_protection_now[2].current = 0;
//                 jump_protection_now[2].voltage = 0;
//                 jump_protection_now[2].current_positive_jump_number = 0;
//                 jump_protection_now[2].current_negative_jump_number = 0;
//                 jump_protection_now[2].voltage_positive_jump_number = 0;
//                 jump_protection_now[2].voltage_negative_jump_number = 0;

//                 PG_count[2] = 0;

//                 record_data.channel_record_data[2].battery_capacitance = 0;
//                 record_data.channel_record_data[2].battery_energy = 0;
//                 record_data.channel_record_data[2].crosscurrent_ratio = 0;

//                 CV_mode[2] = 0;
//                 DCV_mode[2] = 0;

//                 memset(Global_Protection_Flag[2], 0, global_protection_trigger_end);
//                 memset(Step_Protection_Flag[2], 0, step_protection_trigger_end);

//                 Step_Data_Read(CH_3, step_run[2].run_number);
//                 Step_Run_Execute(CH_3);

//                 // running_step[2].status = step_running_status;
//                 running_step[2].type = step_work[2].step_type;
//                 running_step[2].number = step_work[2].step_number;
//                 running_step[2].sum_number = 1;
//                 running_step[2].run_time = 0;
//                 running_step[2].protection_time = 0;

//                 if(running_step[2].type == end_step) {
//                     running_step[2].status = step_complete_status;
//                     Record_Report(0x04);
//                 } else if((running_step[2].type < standby) || (running_step[2].type > end_step)) {
//                     running_step[2].status = step_stop_status;
//                     Record_Report(0x04);
//                 } else {
//                     running_step[2].status = step_running_status;
//                 }
//             } else if(step_run[2].run_status == step_stop_setting) {
//                 led_run_setting(CH_3, led_normal, &(led_control));

//                 //执行停止相关动作
//                 PORT5_DIS;PORT6_DIS;
//                 HAL_Delay(10);
//                 REV_PORT5_DIS;REV_PORT6_DIS;
//                 HAL_Delay(10);

//                 DAC80508_SetDacData(4, 0, 0);
//                 DAC80508_SetDacData(4, 0, 1);
//                 DAC80508_SetDacData(5, 0, 0);
//                 DAC80508_SetDacData(5, 0, 1);

//                 running_step[2].status = step_stop_status;
//                 Record_Report(0x04);
//             } else if(step_run[2].run_status == step_pause_setting) {
//                 led_run_setting(CH_3, led_normal, &(led_control));

//                 //执行暂停相关动作
//                 PORT5_DIS;PORT6_DIS;
//                 HAL_Delay(10);
//                 REV_PORT5_DIS;REV_PORT6_DIS;
//                 HAL_Delay(10);

//                 DAC80508_SetDacData(4, 0, 0);
//                 DAC80508_SetDacData(4, 0, 1);
//                 DAC80508_SetDacData(5, 0, 0);
//                 DAC80508_SetDacData(5, 0, 1);

//                 fluctuation_voltage_flag[2] = 0;
//                 fluctuation_current_flag[2] = 0;

//                 running_step[2].status = step_pause_status;
//                 running_step[2].protection_time = 0;
//                 Record_Report(0x04);
//             } else if(step_run[2].run_status == step_continue_setting) {
//                 //执行继续运行相关动作
//                 if(running_step[2].status == step_pause_status) {
//                     // PORT5_EN;
//                     // PORT6_EN;
//                     Step_Run_Execute(CH_3);

//                     running_step[2].status = step_running_status;
//                     Record_Report(0x04);
//                 }
//             } else if(step_run[2].run_status == step_jump_setting) {
//                 //执行跳转相关动作
//                 /*
//                 code
//                 */
//             }
//         }
//         if(STEP_STATUS_SETTING_FLAG & CH_4) {
//             if(step_run[3].run_status == step_start_setting) {
//                 led_run_setting(0, led_normal, &(led_control));

//                 last_record_condition[3].record_current = 0;
//                 last_record_condition[3].record_voltage = 0;
//                 last_record_condition[3].record_time = 0;

//                 this_record_condition[3].record_current = 0;
//                 this_record_condition[3].record_voltage = 0;
//                 this_record_condition[3].record_time = 0;

//                 fluctuation_voltage_flag[3] = 0;
//                 fluctuation_current_flag[3] = 0;

//                 fluctuation_protection_old[3].voltage = 0;
//                 fluctuation_protection_old[3].voltage_time = 0;
//                 fluctuation_protection_old[3].current = 0;
//                 fluctuation_protection_old[3].current_time = 0;

//                 fluctuation_protection_now[3].voltage = 0;
//                 fluctuation_protection_now[3].voltage_time = 0;
//                 fluctuation_protection_now[3].current = 0;
//                 fluctuation_protection_now[3].current_time = 0;

//                 jump_protection_old[3].current = 0;
//                 jump_protection_old[3].voltage = 0;
//                 jump_protection_old[3].current_positive_jump_number = 0;
//                 jump_protection_old[3].current_negative_jump_number = 0;
//                 jump_protection_old[3].voltage_positive_jump_number = 0;
//                 jump_protection_old[3].voltage_negative_jump_number = 0;

//                 jump_protection_now[3].current = 0;
//                 jump_protection_now[3].voltage = 0;
//                 jump_protection_now[3].current_positive_jump_number = 0;
//                 jump_protection_now[3].current_negative_jump_number = 0;
//                 jump_protection_now[3].voltage_positive_jump_number = 0;
//                 jump_protection_now[3].voltage_negative_jump_number = 0;

//                 PG_count[3] = 0;

//                 record_data.channel_record_data[3].battery_capacitance = 0;
//                 record_data.channel_record_data[3].battery_energy = 0;
//                 record_data.channel_record_data[3].crosscurrent_ratio = 0;

//                 CV_mode[3] = 0;
//                 DCV_mode[3] = 0;

//                 memset(Global_Protection_Flag[3], 0, global_protection_trigger_end);
//                 memset(Step_Protection_Flag[3], 0, step_protection_trigger_end);

//                 Step_Data_Read(CH_4, step_run[3].run_number);
//                 Step_Run_Execute(CH_4);

//                 // running_step[3].status = step_running_status;
//                 running_step[3].type = step_work[3].step_type;
//                 running_step[3].number = step_work[3].step_number;
//                 running_step[3].sum_number = 1;
//                 running_step[3].run_time = 0;
//                 running_step[3].protection_time = 0;

//                 if(running_step[3].type == end_step) {
//                     running_step[3].status = step_complete_status;
//                     Record_Report(0x08);
//                 } else if((running_step[3].type < standby) || (running_step[3].type > end_step)) {
//                     running_step[3].status = step_stop_status;
//                     Record_Report(0x08);
//                 } else {
//                     running_step[3].status = step_running_status;
//                 }
//             } else if(step_run[3].run_status == step_stop_setting) {
//                 led_run_setting(CH_4, led_normal, &(led_control));

//                 //执行停止相关动作
//                 PORT7_DIS;PORT8_DIS;
//                 HAL_Delay(10);
//                 REV_PORT7_DIS;REV_PORT8_DIS;
//                 HAL_Delay(10);

//                 DAC80508_SetDacData(6, 0, 0);
//                 DAC80508_SetDacData(6, 0, 1);
//                 DAC80508_SetDacData(7, 0, 0);
//                 DAC80508_SetDacData(7, 0, 1);

//                 running_step[3].status = step_stop_status;
//                 Record_Report(0x08);
//             } else if(step_run[3].run_status == step_pause_setting) {
//                 led_run_setting(CH_4, led_normal, &(led_control));

//                 //执行暂停相关动作
//                 PORT7_DIS;PORT8_DIS;
//                 HAL_Delay(10);
//                 REV_PORT7_DIS;REV_PORT8_DIS;
//                 HAL_Delay(10);

//                 DAC80508_SetDacData(6, 0, 0);
//                 DAC80508_SetDacData(6, 0, 1);
//                 DAC80508_SetDacData(7, 0, 0);
//                 DAC80508_SetDacData(7, 0, 1);

//                 fluctuation_voltage_flag[3] = 0;
//                 fluctuation_current_flag[3] = 0;

//                 running_step[3].status = step_pause_status;
//                 running_step[3].protection_time = 0;
//                 Record_Report(0x08);
//             } else if(step_run[3].run_status == step_continue_setting) {
//                 //执行继续运行相关动作
//                 if(running_step[3].status == step_pause_status) {
//                     // PORT7_EN;
//                     // PORT8_EN;
//                     Step_Run_Execute(CH_4);

//                     running_step[3].status = step_running_status;
//                     Record_Report(0x08);
//                 }
//             } else if(step_run[3].run_status == step_jump_setting) {
//                 //执行跳转相关动作
//                 /*
//                 code
//                 */
//             }
//         }

//         STEP_STATUS_SETTING_FLAG = 0x00;
//     }
// #endif
// #if CHANNELS_8

// #endif
}

void Step_Run_Switch(uint8_t run_ch, uint16_t run_number)
{
    // uint32_t new_address;
    // uint16_t new_number;

    // new_number = run_number++;
    // new_address = (run_ch-1) * 16 * 4096 + (new_number-1) * 256;

    Step_Data_Read(run_ch, run_number+1);
    Step_Run_Execute(run_ch);

#if CHANNELS_1

#endif
#if CHANNELS_2

#endif
#if CHANNELS_4
    if(run_ch & 0x01) {
        // running_step_type[0]   = step_work[0].step_type;
        // running_step_number[0] = step_work[0].step_number;
        // running_step_number_sum[0] += 1;

        last_record_condition[0].record_current = 0;
        last_record_condition[0].record_voltage = 0;
        last_record_condition[0].record_time = 0;

        this_record_condition[0].record_current = 0;
        this_record_condition[0].record_voltage = 0;
        this_record_condition[0].record_time = 0;

        fluctuation_voltage_flag[0] = 0;
        fluctuation_current_flag[0] = 0;

        fluctuation_protection_old[0].voltage = 0;
        fluctuation_protection_old[0].voltage_time = 0;
        fluctuation_protection_old[0].current = 0;
        fluctuation_protection_old[0].current_time = 0;

        fluctuation_protection_now[0].voltage = 0;
        fluctuation_protection_now[0].voltage_time = 0;
        fluctuation_protection_now[0].current = 0;
        fluctuation_protection_now[0].current_time = 0;

        jump_protection_old[0].current = 0;
        jump_protection_old[0].voltage = 0;
        jump_protection_old[0].current_positive_jump_number = 0;
        jump_protection_old[0].current_negative_jump_number = 0;
        jump_protection_old[0].voltage_positive_jump_number = 0;
        jump_protection_old[0].voltage_negative_jump_number = 0;

        jump_protection_now[0].current = 0;
        jump_protection_now[0].voltage = 0;
        jump_protection_now[0].current_positive_jump_number = 0;
        jump_protection_now[0].current_negative_jump_number = 0;
        jump_protection_now[0].voltage_positive_jump_number = 0;
        jump_protection_now[0].voltage_negative_jump_number = 0;

        record_data.channel_record_data[0].battery_capacitance = 0;
        record_data.channel_record_data[0].battery_energy = 0;
        record_data.channel_record_data[0].crosscurrent_ratio = 0;

        CV_mode[0] = 0;
        DCV_mode[0] = 0;

        // memset(Global_Protection_Flag[0], 0, global_protection_trigger_end);
        // memset(Step_Protection_Flag[0], 0, step_protection_trigger_end);

        running_step[0].type = step_work[0].step_type;
        running_step[0].number = step_work[0].step_number;
        running_step[0].sum_number += 1;
        running_step[0].run_time = 0;
        running_step[0].protection_time = 0;

        if(running_step[0].type == end_step) {
            running_step[0].status = step_complete_status;
            Record_Report(0x01);

            led_run_setting(CH_1, led_normal, &(led_control));
        } else if((running_step[0].type < standby) || (running_step[0].type > end_step)) {
            running_step[0].status = step_stop_status;
            Record_Report(0x01);

            led_run_setting(CH_1, led_normal, &(led_control));
        }

    } else if(run_ch & 0x02) {
        last_record_condition[1].record_current = 0;
        last_record_condition[1].record_voltage = 0;
        last_record_condition[1].record_time = 0;

        this_record_condition[1].record_current = 0;
        this_record_condition[1].record_voltage = 0;
        this_record_condition[1].record_time = 0;

        fluctuation_voltage_flag[1] = 0;
        fluctuation_current_flag[1] = 0;

        fluctuation_protection_old[1].voltage = 0;
        fluctuation_protection_old[1].voltage_time = 0;
        fluctuation_protection_old[1].current = 0;
        fluctuation_protection_old[1].current_time = 0;

        fluctuation_protection_now[1].voltage = 0;
        fluctuation_protection_now[1].voltage_time = 0;
        fluctuation_protection_now[1].current = 0;
        fluctuation_protection_now[1].current_time = 0;

        jump_protection_old[1].current = 0;
        jump_protection_old[1].voltage = 0;
        jump_protection_old[1].current_positive_jump_number = 0;
        jump_protection_old[1].current_negative_jump_number = 0;
        jump_protection_old[1].voltage_positive_jump_number = 0;
        jump_protection_old[1].voltage_negative_jump_number = 0;

        jump_protection_now[1].current = 0;
        jump_protection_now[1].voltage = 0;
        jump_protection_now[1].current_positive_jump_number = 0;
        jump_protection_now[1].current_negative_jump_number = 0;
        jump_protection_now[1].voltage_positive_jump_number = 0;
        jump_protection_now[1].voltage_negative_jump_number = 0;

        record_data.channel_record_data[1].battery_capacitance = 0;
        record_data.channel_record_data[1].battery_energy = 0;
        record_data.channel_record_data[1].crosscurrent_ratio = 0;

        CV_mode[1] = 0;
        DCV_mode[1] = 0;

        // memset(Global_Protection_Flag[1], 0, global_protection_trigger_end);
        // memset(Step_Protection_Flag[1], 0, step_protection_trigger_end);

        running_step[1].type = step_work[1].step_type;
        running_step[1].number = step_work[1].step_number;
        running_step[1].sum_number += 1;
        running_step[1].run_time = 0;
        running_step[1].protection_time = 0;

        if(running_step[1].type == end_step) {
            running_step[1].status = step_complete_status;
            Record_Report(0x02);

            led_run_setting(CH_2, led_normal, &(led_control));
        } else if((running_step[1].type < standby) || (running_step[1].type > end_step)) {
            running_step[1].status = step_stop_status;
            Record_Report(0x02);

            led_run_setting(CH_2, led_normal, &(led_control));
        }

    } else if(run_ch & 0x04) {
        last_record_condition[2].record_current = 0;
        last_record_condition[2].record_voltage = 0;
        last_record_condition[2].record_time = 0;

        this_record_condition[2].record_current = 0;
        this_record_condition[2].record_voltage = 0;
        this_record_condition[2].record_time = 0;

        fluctuation_voltage_flag[2] = 0;
        fluctuation_current_flag[2] = 0;

        fluctuation_protection_old[2].voltage = 0;
        fluctuation_protection_old[2].voltage_time = 0;
        fluctuation_protection_old[2].current = 0;
        fluctuation_protection_old[2].current_time = 0;

        fluctuation_protection_now[2].voltage = 0;
        fluctuation_protection_now[2].voltage_time = 0;
        fluctuation_protection_now[2].current = 0;
        fluctuation_protection_now[2].current_time = 0;

        jump_protection_old[2].current = 0;
        jump_protection_old[2].voltage = 0;
        jump_protection_old[2].current_positive_jump_number = 0;
        jump_protection_old[2].current_negative_jump_number = 0;
        jump_protection_old[2].voltage_positive_jump_number = 0;
        jump_protection_old[2].voltage_negative_jump_number = 0;

        jump_protection_now[2].current = 0;
        jump_protection_now[2].voltage = 0;
        jump_protection_now[2].current_positive_jump_number = 0;
        jump_protection_now[2].current_negative_jump_number = 0;
        jump_protection_now[2].voltage_positive_jump_number = 0;
        jump_protection_now[2].voltage_negative_jump_number = 0;

        record_data.channel_record_data[2].battery_capacitance = 0;
        record_data.channel_record_data[2].battery_energy = 0;
        record_data.channel_record_data[2].crosscurrent_ratio = 0;

        CV_mode[2] = 0;
        DCV_mode[2] = 0;

        // memset(Global_Protection_Flag[2], 0, global_protection_trigger_end);
        // memset(Step_Protection_Flag[2], 0, step_protection_trigger_end);

        running_step[2].type = step_work[2].step_type;
        running_step[2].number = step_work[2].step_number;
        running_step[2].sum_number += 1;
        running_step[2].run_time = 0;
        running_step[2].protection_time = 0;

        if(running_step[2].type == end_step) {
            running_step[2].status = step_complete_status;
            Record_Report(0x04);

            led_run_setting(CH_3, led_normal, &(led_control));
        } else if((running_step[2].type < standby) || (running_step[2].type > end_step)) {
            running_step[2].status = step_stop_status;
            Record_Report(0x04);

            led_run_setting(CH_3, led_normal, &(led_control));
        }

    } else if(run_ch & 0x08) {
        last_record_condition[3].record_current = 0;
        last_record_condition[3].record_voltage = 0;
        last_record_condition[3].record_time = 0;

        this_record_condition[3].record_current = 0;
        this_record_condition[3].record_voltage = 0;
        this_record_condition[3].record_time = 0;

        fluctuation_voltage_flag[3] = 0;
        fluctuation_current_flag[3] = 0;

        fluctuation_protection_old[3].voltage = 0;
        fluctuation_protection_old[3].voltage_time = 0;
        fluctuation_protection_old[3].current = 0;
        fluctuation_protection_old[3].current_time = 0;

        fluctuation_protection_now[3].voltage = 0;
        fluctuation_protection_now[3].voltage_time = 0;
        fluctuation_protection_now[3].current = 0;
        fluctuation_protection_now[3].current_time = 0;

        jump_protection_old[3].current = 0;
        jump_protection_old[3].voltage = 0;
        jump_protection_old[3].current_positive_jump_number = 0;
        jump_protection_old[3].current_negative_jump_number = 0;
        jump_protection_old[3].voltage_positive_jump_number = 0;
        jump_protection_old[3].voltage_negative_jump_number = 0;

        jump_protection_now[3].current = 0;
        jump_protection_now[3].voltage = 0;
        jump_protection_now[3].current_positive_jump_number = 0;
        jump_protection_now[3].current_negative_jump_number = 0;
        jump_protection_now[3].voltage_positive_jump_number = 0;
        jump_protection_now[3].voltage_negative_jump_number = 0;

        record_data.channel_record_data[3].battery_capacitance = 0;
        record_data.channel_record_data[3].battery_energy = 0;
        record_data.channel_record_data[3].crosscurrent_ratio = 0;

        CV_mode[3] = 0;
        DCV_mode[3] = 0;

        // memset(Global_Protection_Flag[3], 0, global_protection_trigger_end);
        // memset(Step_Protection_Flag[3], 0, step_protection_trigger_end);

        running_step[3].type = step_work[3].step_type;
        running_step[3].number = step_work[3].step_number;
        running_step[3].sum_number += 1;
        running_step[3].run_time = 0;
        running_step[3].protection_time = 0;

        if(running_step[3].type == end_step) {
            running_step[3].status = step_complete_status;
            Record_Report(0x08);

            led_run_setting(CH_4, led_normal, &(led_control));
        } else if((running_step[3].type < standby) || (running_step[3].type > end_step)) {
            running_step[3].status = step_stop_status;
            Record_Report(0x08);

            led_run_setting(CH_4, led_normal, &(led_control));
        }

    }
#endif
#if CHANNELS_8

#endif
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
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage >= step_work[i].step_abort_voltage)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }

                    break;
                }
                case CV:
                {
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }

                    break;
                }
                case CC_to_CV:
                {
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }

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
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_voltage) > 0) && (record_data.channel_record_data[i].battery_voltage <= step_work[i].step_abort_voltage)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }

                    break;
                }
                case DCV:
                {
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) &&((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }

                    break;
                }
                case DCC_to_DCV:
                {
                    if( ((FLOAT_NOT_ZERO(step_work[i].step_abort_time) > 0) && ((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_current) > 0) && (record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current)) || \
                        ((FLOAT_NOT_ZERO(step_work[i].step_abort_capacitance) > 0) && (record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance))) {
                        Record_Report(0x01 << i);
                        Step_Run_Switch((0x01<<i), running_step[i].number);
                    }

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
            // if(running_step[i].type == standby) {
            //     if((record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time) {
            //         Step_Run_Switch(CH(i+1), running_step[i].number);
            //     }
            // } else if(running_step[i].type == CC) {
            //     if( (record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time || \
            //         record_data.channel_record_data[i].battery_voltage >= step_work[i].step_abort_voltage || \
            //         record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance) {

            //         Step_Run_Switch(CH(i+1), running_step[i].number);
            //     }
            // } else if(running_step[i].type == CV) {
            //     if( (record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time || \
            //         record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current || \
            //         record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance) {

            //         Step_Run_Switch(CH(i+1), running_step[i].number);
            //     }
            // } else if(running_step[i].type == CC_to_CV) {
            //     if( (record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time || \
            //         record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current || \
            //         record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance) {

            //         Step_Run_Switch(CH(i+1), running_step[i].number);
            //     }
            // } else if(running_step[i].type == CP) {
            //     if( (record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time || \
            //         record_data.channel_record_data[i].battery_voltage >= step_work[i].step_abort_voltage || \
            //         record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance) {

            //         Step_Run_Switch(CH(i+1), running_step[i].number);
            //     }
            // } else if(running_step[i].type == DCC) {
            //     if( (record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time || \
            //         record_data.channel_record_data[i].battery_voltage <= step_work[i].step_abort_voltage || \
            //         record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance) {

            //         Step_Run_Switch(CH(i+1), running_step[i].number);
            //     }
            // } else if(running_step[i].type == DCV) {
            //     if( (record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time || \
            //         record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current || \
            //         record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance) {

            //         Step_Run_Switch(CH(i+1), running_step[i].number);
            //     }
            // } else if(running_step[i].type == DCC_to_DCV) {
            //     if( (record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time || \
            //         record_data.channel_record_data[i].output_current <= step_work[i].step_abort_current || \
            //         record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance) {

            //         Step_Run_Switch(CH(i+1), running_step[i].number);
            //     }
            // } else if(running_step[i].type == DCP) {
            //     if( (record_data.channel_record_data[i].run_time/1000) >= step_work[i].step_abort_time || \
            //         record_data.channel_record_data[i].battery_voltage <= step_work[i].step_abort_voltage || \
            //         record_data.channel_record_data[i].battery_capacitance >= step_work[i].step_abort_capacitance) {

            //         Step_Run_Switch(CH(i+1), running_step[i].number);
            //     }
            // }
        }
    }
// #if CHANNELS_1

// #endif
// #if CHANNELS_2

// #endif
// #if CHANNELS_4
//     if(running_step[0].status == step_running_status) {
//         //根据不同的运行类型进行不同的判断，判断工步是否运行完成,进行工步切换
//         if(running_step[0].type == standby) {
//             if(running_step[0].run_time >= step_work[0].step_abort_time) {
//                 Step_Run_Switch(0x01, running_step[0].number);
//             }
//         } else if(running_step[0].type == CC) {
//             if( running_step[0].run_time >= step_work[0].step_abort_time || \
//                 recode_query[0].battery_voltage >= step_work[0].step_abort_voltage || \
//                 recode_query[0].battery_capacitance >= step_work[0].step_abort_capacitance) {

//                 Step_Run_Switch(0x01, running_step[0].number);
//             }
//         } else if(running_step[0].type == CV) {
//             if( running_step[0].run_time >= step_work[0].step_abort_time || \
//                 recode_query[0].output_current <= step_work[0].step_abort_current || \
//                 recode_query[0].battery_capacitance >= step_work[0].step_abort_capacitance) {

//                 Step_Run_Switch(0x01, running_step[0].number);
//             }
//         } else if(running_step[0].type == CC_to_CV) {
//             if( running_step[0].run_time >= step_work[0].step_abort_time || \
//                 recode_query[0].output_current <= step_work[0].step_abort_current || \
//                 recode_query[0].battery_capacitance >= step_work[0].step_abort_capacitance) {

//                 Step_Run_Switch(0x01, running_step[0].number);
//             }
//         } else if(running_step[0].type == CP) {
//             if( running_step[0].run_time >= step_work[0].step_abort_time || \
//                 recode_query[0].battery_voltage >= step_work[0].step_abort_voltage || \
//                 recode_query[0].battery_capacitance >= step_work[0].step_abort_capacitance) {

//                 Step_Run_Switch(0x01, running_step[0].number);
//             }
//         } else if(running_step[0].type == DCC) {
//             if( running_step[0].run_time >= step_work[0].step_abort_time || \
//                 recode_query[0].battery_voltage <= step_work[0].step_abort_voltage || \
//                 recode_query[0].battery_capacitance >= step_work[0].step_abort_capacitance) {

//                 Step_Run_Switch(0x01, running_step[0].number);
//             }
//         } else if(running_step[0].type == DCV) {
//             if( running_step[0].run_time >= step_work[0].step_abort_time || \
//                 recode_query[0].output_current <= step_work[0].step_abort_current || \
//                 recode_query[0].battery_capacitance >= step_work[0].step_abort_capacitance) {

//                 Step_Run_Switch(0x01, running_step[0].number);
//             }
//         } else if(running_step[0].type == DCC_to_DCV) {
//             if( running_step[0].run_time >= step_work[0].step_abort_time || \
//                 recode_query[0].output_current <= step_work[0].step_abort_current || \
//                 recode_query[0].battery_capacitance >= step_work[0].step_abort_capacitance) {

//                 Step_Run_Switch(0x01, running_step[0].number);
//             }
//         } else if(running_step[0].type == DCP) {
//             if( running_step[0].run_time >= step_work[0].step_abort_time || \
//                 recode_query[0].battery_voltage <= step_work[0].step_abort_voltage || \
//                 recode_query[0].battery_capacitance >= step_work[0].step_abort_capacitance) {

//                 Step_Run_Switch(0x01, running_step[0].number);
//             }
//         }
//     }

//     if(running_step[1].status == step_running_status) {

//         //根据不同的运行类型进行不同的判断，判断工步是否运行完成或超过警戒
//         if(running_step[1].type == standby) {
//             if(running_step[1].run_time >= step_work[1].step_abort_time) {
//                 Step_Run_Switch(0x02, running_step[1].number);
//             }
//         } else if(running_step[1].type == CC) {
//             if( running_step[1].run_time >= step_work[1].step_abort_time || \
//                 recode_query[1].battery_voltage >= step_work[1].step_abort_voltage || \
//                 recode_query[1].battery_capacitance >= step_work[1].step_abort_capacitance) {

//                 Step_Run_Switch(0x02, running_step[1].number);
//             }
//         } else if(running_step[1].type == CV) {
//             if( running_step[1].run_time >= step_work[1].step_abort_time || \
//                 recode_query[1].output_current <= step_work[1].step_abort_current || \
//                 recode_query[1].battery_capacitance >= step_work[1].step_abort_capacitance) {

//                 Step_Run_Switch(0x02, running_step[1].number);
//             }
//         } else if(running_step[1].type == CC_to_CV) {
//             if( running_step[1].run_time >= step_work[1].step_abort_time || \
//                 recode_query[1].output_current <= step_work[1].step_abort_current || \
//                 recode_query[1].battery_capacitance >= step_work[1].step_abort_capacitance) {

//                 Step_Run_Switch(0x02, running_step[1].number);
//             }
//         } else if(running_step[1].type == CP) {
//             if( running_step[1].run_time >= step_work[1].step_abort_time || \
//                 recode_query[1].battery_voltage >= step_work[1].step_abort_voltage || \
//                 recode_query[1].battery_capacitance >= step_work[1].step_abort_capacitance) {

//                 Step_Run_Switch(0x02, running_step[1].number);
//             }
//         } else if(running_step[1].type == DCC) {
//             if( running_step[1].run_time >= step_work[1].step_abort_time || \
//                 recode_query[1].battery_voltage <= step_work[1].step_abort_voltage || \
//                 recode_query[1].battery_capacitance >= step_work[1].step_abort_capacitance) {

//                 Step_Run_Switch(0x02, running_step[1].number);
//             }
//         } else if(running_step[1].type == DCV) {
//             if( running_step[1].run_time >= step_work[1].step_abort_time || \
//                 recode_query[1].output_current <= step_work[1].step_abort_current || \
//                 recode_query[1].battery_capacitance >= step_work[1].step_abort_capacitance) {

//                 Step_Run_Switch(0x02, running_step[1].number);
//             }
//         } else if(running_step[1].type == DCC_to_DCV) {
//             if( running_step[1].run_time >= step_work[1].step_abort_time || \
//                 recode_query[1].output_current <= step_work[1].step_abort_current || \
//                 recode_query[1].battery_capacitance >= step_work[1].step_abort_capacitance) {

//                 Step_Run_Switch(0x02, running_step[1].number);
//             }
//         } else if(running_step[1].type == DCP) {
//             if( running_step[1].run_time >= step_work[1].step_abort_time || \
//                 recode_query[1].battery_voltage <= step_work[1].step_abort_voltage || \
//                 recode_query[1].battery_capacitance >= step_work[1].step_abort_capacitance) {

//                 Step_Run_Switch(0x02, running_step[1].number);
//             }
//         }
//     }

//     if(running_step[2].status == step_running_status) {

//         //根据不同的运行类型进行不同的判断，判断工步是否运行完成或超过警戒
//         if(running_step[2].type == standby) {
//             if(running_step[2].run_time >= step_work[2].step_abort_time) {
//                 Step_Run_Switch(0x04, running_step[2].number);
//             }
//         } else if(running_step[2].type == CC) {
//             if( running_step[2].run_time >= step_work[2].step_abort_time || \
//                 recode_query[2].battery_voltage >= step_work[2].step_abort_voltage || \
//                 recode_query[2].battery_capacitance >= step_work[2].step_abort_capacitance) {

//                 Step_Run_Switch(0x04, running_step[2].number);
//             }
//         } else if(running_step[2].type == CV) {
//             if( running_step[2].run_time >= step_work[2].step_abort_time || \
//                 recode_query[2].output_current <= step_work[2].step_abort_current || \
//                 recode_query[2].battery_capacitance >= step_work[2].step_abort_capacitance) {

//                 Step_Run_Switch(0x04, running_step[2].number);
//             }
//         } else if(running_step[2].type == CC_to_CV) {
//             if( running_step[2].run_time >= step_work[2].step_abort_time || \
//                 recode_query[2].output_current <= step_work[2].step_abort_current || \
//                 recode_query[2].battery_capacitance >= step_work[2].step_abort_capacitance) {

//                 Step_Run_Switch(0x04, running_step[2].number);
//             }
//         } else if(running_step[2].type == CP) {
//             if( running_step[2].run_time >= step_work[2].step_abort_time || \
//                 recode_query[2].battery_voltage >= step_work[2].step_abort_voltage || \
//                 recode_query[2].battery_capacitance >= step_work[2].step_abort_capacitance) {

//                 Step_Run_Switch(0x04, running_step[2].number);
//             }
//         } else if(running_step[2].type == DCC) {
//             if( running_step[2].run_time >= step_work[2].step_abort_time || \
//                 recode_query[2].battery_voltage <= step_work[2].step_abort_voltage || \
//                 recode_query[2].battery_capacitance >= step_work[2].step_abort_capacitance) {

//                 Step_Run_Switch(0x04, running_step[2].number);
//             }
//         } else if(running_step[2].type == DCV) {
//             if( running_step[2].run_time >= step_work[2].step_abort_time || \
//                 recode_query[2].output_current <= step_work[2].step_abort_current || \
//                 recode_query[2].battery_capacitance >= step_work[2].step_abort_capacitance) {

//                 Step_Run_Switch(0x04, running_step[2].number);
//             }
//         } else if(running_step[2].type == DCC_to_DCV) {
//             if( running_step[2].run_time >= step_work[2].step_abort_time || \
//                 recode_query[2].output_current <= step_work[2].step_abort_current || \
//                 recode_query[2].battery_capacitance >= step_work[2].step_abort_capacitance) {

//                 Step_Run_Switch(0x04, running_step[2].number);
//             }
//         } else if(running_step[2].type == DCP) {
//             if( running_step[2].run_time >= step_work[2].step_abort_time || \
//                 recode_query[2].battery_voltage <= step_work[2].step_abort_voltage || \
//                 recode_query[2].battery_capacitance >= step_work[2].step_abort_capacitance) {

//                 Step_Run_Switch(0x04, running_step[2].number);
//             }
//         }
//     }

//     if(running_step[3].status == step_running_status) {

//         //根据不同的运行类型进行不同的判断，判断工步是否运行完成或超过警戒
//         if(running_step[3].type == standby) {
//             if(running_step[3].run_time >= step_work[3].step_abort_time) {
//                 Step_Run_Switch(0x08, running_step[3].number);
//             }
//         } else if(running_step[3].type == CC) {
//             if( running_step[3].run_time >= step_work[3].step_abort_time || \
//                 recode_query[3].battery_voltage >= step_work[3].step_abort_voltage || \
//                 recode_query[3].battery_capacitance >= step_work[3].step_abort_capacitance) {

//                 Step_Run_Switch(0x08, running_step[3].number);
//             }
//         } else if(running_step[3].type == CV) {
//             if( running_step[3].run_time >= step_work[3].step_abort_time || \
//                 recode_query[3].output_current <= step_work[3].step_abort_current || \
//                 recode_query[3].battery_capacitance >= step_work[3].step_abort_capacitance) {

//                 Step_Run_Switch(0x08, running_step[3].number);
//             }
//         } else if(running_step[3].type == CC_to_CV) {
//             if( running_step[3].run_time >= step_work[3].step_abort_time || \
//                 recode_query[3].output_current <= step_work[3].step_abort_current || \
//                 recode_query[3].battery_capacitance >= step_work[3].step_abort_capacitance) {

//                 Step_Run_Switch(0x08, running_step[3].number);
//             }
//         } else if(running_step[3].type == CP) {
//             if( running_step[3].run_time >= step_work[3].step_abort_time || \
//                 recode_query[3].battery_voltage >= step_work[3].step_abort_voltage || \
//                 recode_query[3].battery_capacitance >= step_work[3].step_abort_capacitance) {

//                 Step_Run_Switch(0x08, running_step[3].number);
//             }
//         } else if(running_step[3].type == DCC) {
//             if( running_step[3].run_time >= step_work[3].step_abort_time || \
//                 recode_query[3].battery_voltage <= step_work[3].step_abort_voltage || \
//                 recode_query[3].battery_capacitance >= step_work[3].step_abort_capacitance) {

//                 Step_Run_Switch(0x08, running_step[3].number);
//             }
//         } else if(running_step[3].type == DCV) {
//             if( running_step[3].run_time >= step_work[3].step_abort_time || \
//                 recode_query[3].output_current <= step_work[3].step_abort_current || \
//                 recode_query[3].battery_capacitance >= step_work[3].step_abort_capacitance) {

//                 Step_Run_Switch(0x08, running_step[3].number);
//             }
//         } else if(running_step[3].type == DCC_to_DCV) {
//             if( running_step[3].run_time >= step_work[3].step_abort_time || \
//                 recode_query[3].output_current <= step_work[3].step_abort_current || \
//                 recode_query[3].battery_capacitance >= step_work[3].step_abort_capacitance) {

//                 Step_Run_Switch(0x08, running_step[3].number);
//             }
//         } else if(running_step[3].type == DCP) {
//             if( running_step[3].run_time >= step_work[3].step_abort_time || \
//                 recode_query[3].battery_voltage <= step_work[3].step_abort_voltage || \
//                 recode_query[3].battery_capacitance >= step_work[3].step_abort_capacitance) {

//                 Step_Run_Switch(0x08, running_step[3].number);
//             }
//         }
//     }
// #endif
// #if CHANNELS_8

// #endif
}

void Step_Turn_Off(uint8_t ch)
{
#if CHANNELS_1
    if(ch & 0x01) {
        PORT1_DIR;PORT2_DIR;PORT3_DIR;PORT4_DIR;
        PORT5_DIR;PORT6_DIR;PORT7_DIR;PORT8_DIR;
        HAL_Delay(10);
        REV_PORT1_DIS;REV_PORT2_DIS;REV_PORT3_DIS;REV_PORT4_DIS;
        REV_PORT5_DIS;REV_PORT6_DIS;REV_PORT7_DIS;REV_PORT8_DIS;
        HAL_Delay(10);

        DAC80508_SetDacData(0, 0, 0);
        DAC80508_SetDacData(0, 0, 1);
        DAC80508_SetDacData(1, 0, 0);
        DAC80508_SetDacData(1, 0, 1);
        DAC80508_SetDacData(2, 0, 0);
        DAC80508_SetDacData(2, 0, 1);
        DAC80508_SetDacData(3, 0, 0);
        DAC80508_SetDacData(3, 0, 1);
        DAC80508_SetDacData(4, 0, 0);
        DAC80508_SetDacData(4, 0, 1);
        DAC80508_SetDacData(5, 0, 0);
        DAC80508_SetDacData(5, 0, 1);
        DAC80508_SetDacData(6, 0, 0);
        DAC80508_SetDacData(6, 0, 1);
        DAC80508_SetDacData(7, 0, 0);
        DAC80508_SetDacData(7, 0, 1);
    }
#endif
#if CHANNELS_2
    for(int i = 0; i < CHANNELS;i++) {
        if(ch & (0x01<<i)) {
            PORT_DIS(i*4);PORT_DIS(i*4+1);PORT_DIS(i*4+2);PORT_DIS(i*4+3);
            HAL_Delay(10);
            REV_PORT_DIS(i*4);REV_PORT_DIS(i*4+1);REV_PORT_DIS(i*4+2);REV_PORT_DIS(i*4+3);
            HAL_Delay(10);

            DAC80508_SetDacData(i*4, 0, 0);
            DAC80508_SetDacData(i*4, 0, 1);
            DAC80508_SetDacData(i*4+1, 0, 0);
            DAC80508_SetDacData(i*4+1, 0, 1);
            DAC80508_SetDacData(i*4+2, 0, 0);
            DAC80508_SetDacData(i*4+2, 0, 1);
            DAC80508_SetDacData(i*4+3, 0, 0);
            DAC80508_SetDacData(i*4+3, 0, 1);
        }
    }
#endif
#if CHANNELS_4
    for(int i = 0; i < CHANNELS;i++) {
        if(ch & (0x01<<i)) {
            PORT_DIS(i*2);PORT_DIS(i*2+1);
            HAL_Delay(10);
            REV_PORT_DIS(i*2);REV_PORT_DIS(i*2+1);
            HAL_Delay(10);

            DAC80508_SetDacData(i*2, 0, 0);
            DAC80508_SetDacData(i*2, 0, 1);
            DAC80508_SetDacData(i*2+1, 0, 0);
            DAC80508_SetDacData(i*2+1, 0, 1);
        }
    }
#endif
#if CHANNELS_8
    for(int i = 0; i < CHANNELS;i++) {
        if(ch & (0x01<<i)) {
            PORT_DIS(i);
            HAL_Delay(10);
            REV_PORT_DIS(i);
            HAL_Delay(10);

            DAC80508_SetDacData(i, 0, 0);
            DAC80508_SetDacData(i, 0, 1);
        }
    }
#endif
}
