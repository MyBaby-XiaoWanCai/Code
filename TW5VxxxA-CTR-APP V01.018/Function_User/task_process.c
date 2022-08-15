/*
*********************************************************************************************************
*
*	模块名称 : 任务处理
*	文件名称 : task_process.c
*	版    本 : V1.0
*	说    明 :
*
*	修改记录 :
*		版本号  日期         作者     说明
*		V1.0    2020-03-12   ZYL      正式发布
*********************************************************************************************************
*/

#include "main.h"
#include "data_analysis.h"
#include "step_work.h"
#include "ad7175_8.h"
#include <math.h>

#define Task_Num   6   //任务<Task_Num

#define Task1ms_COUNT         0//10        //1ms
#define Task10ms_COUNT        0//100       //10ms
// #define Task20ms_COUNT        200   //20ms
#define Task25ms_COUNT        250       //25ms
#define Task100ms_COUNT       1000      //100ms
#define Task500ms_COUNT       5000      //500ms
#define Task1000ms_COUNT      10000     //1s

static uint8_t adc_count = 0;

TaskTime  TasksPare[Task_Num];

// static uint16_t sample_counter = 0;
// static uint16_t ad_count = 0;

extern uint32_t ageing_time;

void Timer_Task_Count(void)  //事件任务计数
{
    uint16_t Task_Count=0;

    for(Task_Count=0;Task_Count<Task_Num;Task_Count++)
    {
        if((TasksPare[Task_Count].Task_Count<TasksPare[Task_Count].Task_Period)&&(TasksPare[Task_Count].Task_Period>0))
        {
            TasksPare[Task_Count].Task_Count++;
        }
    }
}

void Task_RUN(void)
{
    uint16_t Task_Count=0;

    for(Task_Count=0;Task_Count < Task_Num;Task_Count++)
    {
        if((TasksPare[Task_Count].Task_Count>=TasksPare[Task_Count].Task_Period)&&(TasksPare[Task_Count].Task_Period > 0))
        {
            TasksPare[Task_Count].Task_Function();
            TasksPare[Task_Count].Task_Count=0;
        }
    }
}

void Task_1ms_Callback(void)
{
    // // LED81 = 0;
    // Time_Stamp++;

    // for(int i = 0; i < 8; i++) {
    //     ad_sample[i].v_sense += ad7175_value[i];
    //     ad_sample[i].i_out += ad7175_value[i+8];
    //     ad_sample[i].v_j += ad7124_value[i*2];

    //     calibration_ad_add[i].Vsense_AD += ad7175_value[i];
    //     calibration_ad_add[i].Vmod_AD += ADCx_Vmod[i];
    //     calibration_ad_add[i].Iout_AD += ad7175_value[i+8];
    //     calibration_ad_add[i].Vj_AD += ad7124_value[i*2];
    // }

    // sample_counter++;
    // ad_count++;

    // //AD7175采样,24位；7124采样，24位-----------------------------------------------------------------
    // if(sample_counter >= SAMPLE_NUMBER) {
    //     for(int i = 0; i < 8; i++) {
    //         if(Dir_flag[i] == BUCK_MODE) {
    //             filter_data_now[i].v_sense = (float)((calibration[i].Buck_Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 16777216) + calibration[i].Buck_Vsense_B;

    //             if(Irange_flag[i] == I_LOW) {
    //                 filter_data_now[i].i_out = (float)((calibration[i].Buck_Iout_LK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Buck_Iout_LB;
    //             } else if(Irange_flag[i] == I_HIGH) {
    //                 filter_data_now[i].i_out = (float)((calibration[i].Buck_Iout_HK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Buck_Iout_HB;
    //             }

    //             filter_data_now[i].v_j = (float)((calibration[i].Buck_Vj_K * (ad_sample[i].v_j / sample_counter)) / 16777216) + calibration[i].Buck_Vj_B;
    //         } else if(Dir_flag[i] == BOOST_MODE) {
    //             filter_data_now[i].v_sense = (float)((calibration[i].Boost_Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 16777216) + calibration[i].Boost_Vsense_B;

    //             if(Irange_flag[i] == I_LOW) {
    //                 filter_data_now[i].i_out = (float)((calibration[i].Boost_Iout_LK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Boost_Iout_LB;
    //             } else if(Irange_flag[i] == I_HIGH) {
    //                 filter_data_now[i].i_out = (float)((calibration[i].Boost_Iout_HK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Boost_Iout_HB;
    //             }

    //             filter_data_now[i].v_j = (float)((calibration[i].Boost_Vj_K * (ad_sample[i].v_j / sample_counter)) / 16777216) + calibration[i].Boost_Vj_B;
    //         } else {
    //             filter_data_now[i].v_sense = 0;
    //             filter_data_now[i].i_out = 0;
    //             filter_data_now[i].v_j = 0;
    //         }


    //         ad_sample[i].v_sense = 0;
    //         ad_sample[i].i_out = 0;
    //         ad_sample[i].v_j = 0;
    //     }

    //     sample_counter = 0;
    // }
    // //----------------------------------------------------------------------------------------------

    // //校准 AD 累加值处理-----------------------------------------------------------------------------
    // if(ad_count >= AD_NUMBER) {
    //     for(int i = 0; i < 8; i++) {
    //         calibration_ad_record[i].Vsense_AD = calibration_ad_add[i].Vsense_AD;
    //         calibration_ad_record[i].Vmod_AD = calibration_ad_add[i].Vmod_AD;
    //         calibration_ad_record[i].Iout_AD = calibration_ad_add[i].Iout_AD;
    //         calibration_ad_record[i].Vj_AD = calibration_ad_add[i].Vj_AD;

    //         calibration_ad_add[i].Vsense_AD = 0;
    //         calibration_ad_add[i].Vmod_AD = 0;
    //         calibration_ad_add[i].Iout_AD = 0;
    //         calibration_ad_add[i].Vj_AD = 0;
    //     }

    //     ad_count = 0;
    // }
    // //----------------------------------------------------------------------------------------------

    // for(int i = 0; i < CHANNELS; i++) {
    //     if(SYS_MODE == SYS_NORMAL_MODE) {
    //         if(running_step[i].status == step_running_status) {
    //             running_step[i].run_time++;
    //             running_step[i].protection_time++;
    //         }
    //     }
    // }


    // if(SYS_MODE == SYS_NORMAL_MODE) {
    //     //判断记录数据上传条件，满足条件则上传记录数据--------------------------------------------------
    //     for(int i = 0; i < CHANNELS; i++) {
    //         if(running_step[i].status == step_running_status) {
    //             this_record_condition[i].record_current = record_data.channel_record_data[i].output_current;
    //             this_record_condition[i].record_voltage = record_data.channel_record_data[i].battery_voltage;
    //             this_record_condition[i].record_time    = running_step[i].run_time;

    //             if( ((FLOAT_NOT_ZERO(step_record_condition[i].record_current) > 0) && (fabs(this_record_condition[i].record_current - last_record_condition[i].record_current) >= step_record_condition[i].record_current)) ||
    //                 ((FLOAT_NOT_ZERO(step_record_condition[i].record_voltage) > 0) && (fabs(this_record_condition[i].record_voltage - last_record_condition[i].record_voltage) >= step_record_condition[i].record_voltage)) ||
    //                 ((FLOAT_NOT_ZERO(step_record_condition[i].record_time) > 0) && ((this_record_condition[i].record_time - last_record_condition[i].record_time) >= (step_record_condition[i].record_time*1000))) ) {

    //                 Record_Report(0x01 << i);

    //                 last_record_condition[i].record_current = this_record_condition[i].record_current;
    //                 last_record_condition[i].record_voltage = this_record_condition[i].record_voltage;
    //                 last_record_condition[i].record_time = this_record_condition[i].record_time;

    //                 //波动保护数据记录---------------------------------------------------------------
    //                 fluctuation_protection_now[i].voltage = fabs(recode_query[i].battery_voltage);
    //                 // fluctuation_protection_now[i].voltage_time = recode_query[i].run_time;
    //                 fluctuation_protection_now[i].voltage_time = running_step[i].protection_time;
    //                 fluctuation_protection_now[i].current = fabs(recode_query[i].output_current);
    //                 // fluctuation_protection_now[i].current_time = recode_query[i].run_time;
    //                 fluctuation_protection_now[i].current_time = running_step[i].protection_time;
    //                 //------------------------------------------------------------------------------

    //                 //跳变保护数据记录---------------------------------------------------------------
    //                 if(running_step[i].protection_time <= 1000) {
    //                     jump_protection_old[i].current = fabs(recode_query[i].output_current);
    //                     jump_protection_old[i].voltage = fabs(recode_query[i].battery_voltage);
    //                 } else {
    //                     jump_protection_old[i].current = jump_protection_now[i].current;
    //                     jump_protection_old[i].voltage = jump_protection_now[i].voltage;
    //                 }

    //                 jump_protection_now[i].current = fabs(recode_query[i].output_current);
    //                 jump_protection_now[i].voltage = fabs(recode_query[i].battery_voltage);

    //                 jump_flag[i][0] = 1;
    //                 jump_flag[i][1] = 1;
    //                 jump_flag[i][2] = 1;
    //                 jump_flag[i][3] = 1;
    //                 //------------------------------------------------------------------------------

    //                 // //检测 CC 转 CV------------------------------------------------------------------
    //                 // if((recode_query[i].run_status == step_running_status) && (CV_mode[i] == 0) && (DCV_mode[i] == 0)) {
    //                 //     // if(recode_query[i].run_status == step_running_status) {
    //                 //         if((recode_query[i].step_type == CV) || (recode_query[i].step_type == CC_to_CV)) {
    //                 //             if((fabs(recode_query[i].output_current-step_work[i].step_work_current) >= 0.02) && (fabs(recode_query[i].battery_voltage-step_work[i].step_work_voltage) <= 0.1) ) {
    //                 //                 CV_mode[i] = 1;
    //                 //             }
    //                 //         } else if((recode_query[i].step_type == DCV) || (recode_query[i].step_type == DCC_to_DCV)) {
    //                 //             if((fabs(recode_query[i].output_current-step_work[i].step_work_current) >= 0.02) && (fabs(recode_query[i].battery_voltage-step_work[i].step_work_voltage) <= 0.1) ) {
    //                 //                 DCV_mode[i] = 1;
    //                 //             }
    //                 //         }
    //                 //     // }
    //                 // }
    //                 // //------------------------------------------------------------------------------

    //             }
    //         }
    //     }
    //     //------------------------------------------------------------------------------------------

    // }

    // // LED81 = 1;

}

void Task_10ms_Callback(void)
{
    // for(int i = 0; i < 8; i++) {
    //     // filter_data_now[i].v_sense = (float)((calibration[i].Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 16777216) + calibration[i].Vsense_B;
    //     filter_data_now[i].v_sense = (float)((calibration[i].Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 65536) + calibration[i].Vsense_B;
    //     if(Irange_flag[i] == I_LOW) {
    //         // filter_data_now[i].i_out = (float)((calibration[i].Iout_LK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Iout_LB;
    //         filter_data_now[i].i_out = (float)((calibration[i].Iout_LK * (ad_sample[i].i_out / sample_counter)) / 65536) + calibration[i].Iout_LB;
    //     } else if(Irange_flag[i] == I_HIGH) {
    //         // filter_data_now[i].i_out = (float)((calibration[i].Iout_HK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Iout_HB;
    //         filter_data_now[i].i_out = (float)((calibration[i].Iout_HK * (ad_sample[i].i_out / sample_counter)) / 65536) + calibration[i].Iout_HB;
    //     }
    //     filter_data_now[i].v_j = (float)((calibration[i].Vj_K * (ad_sample[i].v_j / sample_counter)) / 16777216) + calibration[i].Vj_B;

    //     ad_sample[i].v_sense = 0;
    //     ad_sample[i].i_out = 0;
    //     ad_sample[i].v_j = 0;
    // }

    // sample_counter = 0;


    // ADC_Start_Stop_DMA();
    // adc_count++;

    // if(adc_count == 4) {
    //     adc_count = 0;

    //     for(int i = 0; i < 8; i++) {
    //         filter_data_now[i].v_mod = (float)((calibration[i].Buck_Vmod_K * ADCx_Vmod[i]) / 4096) + calibration[i].Buck_Vmod_B;
    //         filter_data_now[i].mod_tempetature = (float)((special_calibration.Temperature_K * ADCx_Temp[i]) / 4096) + special_calibration.Temperature_B;
    //     }

    //     special_sample.Bus1 = (float)((special_calibration.Bus1_K * AD_Bus1) / 4096) + special_calibration.Bus1_B;
    //     special_sample.Bus2 = (float)((special_calibration.Bus2_K * AD_Bus2) / 4096) + special_calibration.Bus2_B;
    //     special_sample.Aux = (float)((special_calibration.Aux_K * AD_Aux) / 4096) + special_calibration.Aux_B;
    //     special_sample.Amb = (float)((special_calibration.Amb_K * AD_Amb) / 4096) + special_calibration.Amb_B;
    // }
}

void Task_25ms_Callback(void)
{
    ADC_Start_Stop_DMA();
#if w5500_EN
	Socket_Work_Timeout_Check();
#endif
}


void Task_100ms_Callback(void)
{
    //自身 AD 采样，12位-----------------------------------------------------------------------------
    for(int i = 0; i < 8; i++) {
        if(Dir_flag[i] == BUCK_MODE) {
            filter_data_now[i].v_mod = (float)((calibration[i].Buck_Vmod_K * ADCx_Vmod[i]) / 4096) + calibration[i].Buck_Vmod_B;
        } else if(Dir_flag[i] == BOOST_MODE) {
            filter_data_now[i].v_mod = (float)((calibration[i].Boost_Vmod_K * ADCx_Vmod[i]) / 4096) + calibration[i].Boost_Vmod_B;

        }

        filter_data_now[i].mod_tempetature = (float)((special_calibration.Temperature_K * ADCx_Temp[i]) / 4096) + special_calibration.Temperature_B;

    }

    special_sample.Bus1 = (float)((special_calibration.Bus1_K * AD_Bus1) / 4096) + special_calibration.Bus1_B;
    special_sample.Bus2 = (float)((special_calibration.Bus2_K * AD_Bus2) / 4096) + special_calibration.Bus2_B;
    special_sample.Aux = (float)((special_calibration.Aux_K * AD_Aux) / 4096) + special_calibration.Aux_B;
    special_sample.Amb = (float)((special_calibration.Amb_K * AD_Amb) / 4096) + special_calibration.Amb_B;
    //----------------------------------------------------------------------------------------------

    //获取计算各采样数据------------------------------------------------------------------------------
#if CHANNELS_1
    record_data.channel_record_data[0].run_time = (float)running_step[0].run_time;
    record_data.channel_record_data[0].battery_voltage = filter_data_now[0].v_sense;
    record_data.channel_record_data[0].output_current = filter_data_now[0].i_out + filter_data_now[1].i_out + filter_data_now[2].i_out + filter_data_now[3].i_out \
                                                        filter_data_now[4].i_out + filter_data_now[5].i_out + filter_data_now[6].i_out + filter_data_now[7].i_out;
    record_data.channel_record_data[0].output_voltage = filter_data_now[0].v_mod;
    record_data.channel_record_data[0].v_j = filter_data_now[0].v_j;
    // record_data.channel_record_data[0].battery_temperature = filter_data_now[0].battery_temperature>filter_data_now[1].battery_temperature?filter_data_now[0].battery_temperature:filter_data_now[1].battery_temperature;
    // record_data.channel_record_data[0].battery_temperature = record_data.channel_record_data[0].battery_temperature>filter_data_now[2].battery_temperature?record_data.channel_record_data[0].battery_temperature:filter_data_now[2].battery_temperature;
    // record_data.channel_record_data[0].battery_temperature = record_data.channel_record_data[0].battery_temperature>filter_data_now[3].battery_temperature?record_data.channel_record_data[0].battery_temperature:filter_data_now[3].battery_temperature;
    // record_data.channel_record_data[0].battery_temperature = record_data.channel_record_data[0].battery_temperature>filter_data_now[4].battery_temperature?record_data.channel_record_data[0].battery_temperature:filter_data_now[4].battery_temperature;
    // record_data.channel_record_data[0].battery_temperature = record_data.channel_record_data[0].battery_temperature>filter_data_now[5].battery_temperature?record_data.channel_record_data[0].battery_temperature:filter_data_now[5].battery_temperature;
    // record_data.channel_record_data[0].battery_temperature = record_data.channel_record_data[0].battery_temperature>filter_data_now[6].battery_temperature?record_data.channel_record_data[0].battery_temperature:filter_data_now[6].battery_temperature;
    // record_data.channel_record_data[0].battery_temperature = record_data.channel_record_data[0].battery_temperature>filter_data_now[7].battery_temperature?record_data.channel_record_data[0].battery_temperature:filter_data_now[7].battery_temperature;
    record_data.channel_record_data[0].module_temperature = filter_data_now[0].mod_tempetature>filter_data_now[1].mod_tempetature?filter_data_now[0].mod_tempetature:filter_data_now[1].mod_tempetature;
    record_data.channel_record_data[0].module_temperature = record_data.channel_record_data[0].module_temperature>filter_data_now[2].mod_tempetature?record_data.channel_record_data[0].module_temperature:filter_data_now[2].mod_tempetature;
    record_data.channel_record_data[0].module_temperature = record_data.channel_record_data[0].module_temperature>filter_data_now[3].mod_tempetature?record_data.channel_record_data[0].module_temperature:filter_data_now[3].mod_tempetature;
    record_data.channel_record_data[0].module_temperature = record_data.channel_record_data[0].module_temperature>filter_data_now[4].mod_tempetature?record_data.channel_record_data[0].module_temperature:filter_data_now[4].mod_tempetature;
    record_data.channel_record_data[0].module_temperature = record_data.channel_record_data[0].module_temperature>filter_data_now[5].mod_tempetature?record_data.channel_record_data[0].module_temperature:filter_data_now[5].mod_tempetature;
    record_data.channel_record_data[0].module_temperature = record_data.channel_record_data[0].module_temperature>filter_data_now[6].mod_tempetature?record_data.channel_record_data[0].module_temperature:filter_data_now[6].mod_tempetature;
    record_data.channel_record_data[0].module_temperature = record_data.channel_record_data[0].module_temperature>filter_data_now[7].mod_tempetature?record_data.channel_record_data[0].module_temperature:filter_data_now[7].mod_tempetature;
    record_data.channel_record_data[0].crosscurrent_ratio = 0;


    if((running_step[0].status == step_running_status) && (running_step[0].type != standby)) {
        record_data.channel_record_data[0].battery_capacitance += record_data.channel_record_data[0].output_current * INTEGRAL_TIME / 3600.0;
        record_data.channel_record_data[0].battery_energy += record_data.channel_record_data[0].battery_voltage * record_data.channel_record_data[0].output_current * INTEGRAL_TIME / 3600.0;

        if((running_step[0].type == CC) || (running_step[0].type == CV) || (running_step[0].type == CC_to_CV)) {
                record_data.channel_record_data[0].sum_capacitance += record_data.channel_record_data[0].battery_capacitance;
            } else if((running_step[0].type == DCC) || (running_step[0].type == DCV) || (running_step[0].type == DCC_to_DCV)) {
                record_data.channel_record_data[0].sum_capacitance -= record_data.channel_record_data[0].battery_capacitance;
            }
    }

    if(record_data.channel_record_data[0].output_current >= 5.0) {
        record_data.channel_record_data[0].contact_impedance = fabs(record_data.channel_record_data[0].v_j-record_data.channel_record_data[0].battery_voltage) / record_data.channel_record_data[0].output_current;
        record_data.channel_record_data[0].loop_impedance = fabs(record_data.channel_record_data[0].battery_voltage - record_data.channel_record_data[0].output_voltage) / record_data.channel_record_data[0].output_current;
    } else {
        record_data.channel_record_data[0].contact_impedance = 0;
        record_data.channel_record_data[0].loop_impedance = 0;
    }
#endif

#if CHANNELS_2
    for(int i = 0; i < CHANNELS; i++) {
        record_data.channel_record_data[i].run_time = (float)running_step[i].run_time;
        record_data.channel_record_data[i].battery_voltage = filter_data_now[4*i].v_sense;
        record_data.channel_record_data[i].output_current = filter_data_now[4*i].i_out + filter_data_now[4*i+1].i_out + filter_data_now[4*i+2].i_out + filter_data_now[4*i+3].i_out;
        record_data.channel_record_data[i].output_voltage = filter_data_now[4*i].v_mod;
        record_data.channel_record_data[i].v_j = filter_data_now[4*i].v_j;
        // record_data.channel_record_data[i].battery_temperature = filter_data_now[4*i].battery_temperature>filter_data_now[4*i+1].battery_temperature?filter_data_now[4*i].battery_temperature:filter_data_now[4*i+1].battery_temperature;
        // record_data.channel_record_data[i].battery_temperature = record_data.channel_record_data[i].battery_temperature>filter_data_now[4*i+2].battery_temperature?record_data.channel_record_data[i].battery_temperature:filter_data_now[4*i+2].battery_temperature;
        // record_data.channel_record_data[i].battery_temperature = record_data.channel_record_data[i].battery_temperature>filter_data_now[4*i+3].battery_temperature?record_data.channel_record_data[i].battery_temperature:filter_data_now[4*i+3].battery_temperature;
        record_data.channel_record_data[i].module_temperature = filter_data_now[4*i].mod_tempetature>filter_data_now[4*i+1].mod_tempetature?filter_data_now[4*i].mod_tempetature:filter_data_now[4*i+1].mod_tempetature;
        record_data.channel_record_data[i].module_temperature = record_data.channel_record_data[i].module_temperature>filter_data_now[4*i+2].mod_tempetature?record_data.channel_record_data[i].module_temperature:filter_data_now[4*i+2].mod_tempetature;
        record_data.channel_record_data[i].module_temperature = record_data.channel_record_data[i].module_temperature>filter_data_now[4*i+3].mod_tempetature?record_data.channel_record_data[i].module_temperature:filter_data_now[4*i+3].mod_tempetature;
        record_data.channel_record_data[i].crosscurrent_ratio = 0;


        if((running_step[i].status == step_running_status) && (running_step[i].type != standby)) {
            record_data.channel_record_data[i].battery_capacitance += record_data.channel_record_data[i].output_current * INTEGRAL_TIME / 3600.0;
            record_data.channel_record_data[i].battery_energy += record_data.channel_record_data[i].battery_voltage * record_data.channel_record_data[i].output_current * INTEGRAL_TIME / 3600.0;

            if((running_step[i].type == CC) || (running_step[i].type == CV) || (running_step[i].type == CC_to_CV)) {
                record_data.channel_record_data[i].sum_capacitance += record_data.channel_record_data[i].battery_capacitance;
            } else if((running_step[i].type == DCC) || (running_step[i].type == DCV) || (running_step[i].type == DCC_to_DCV)) {
                record_data.channel_record_data[i].sum_capacitance -= record_data.channel_record_data[i].battery_capacitance;
            }
        }

        if(record_data.channel_record_data[i].output_current >= 5.0) {
            record_data.channel_record_data[i].contact_impedance = fabs(record_data.channel_record_data[i].v_j-record_data.channel_record_data[i].battery_voltage) / record_data.channel_record_data[i].output_current;
            record_data.channel_record_data[i].loop_impedance = fabs(record_data.channel_record_data[i].battery_voltage - record_data.channel_record_data[i].output_voltage) / record_data.channel_record_data[i].output_current;
        } else {
            record_data.channel_record_data[i].contact_impedance = 0;
            record_data.channel_record_data[i].loop_impedance = 0;
        }

    }
#endif

#if CHANNELS_4
    for(int i = 0; i < CHANNELS; i++) {
        record_data.channel_record_data[i].run_time = (float)running_step[i].run_time;
        record_data.channel_record_data[i].battery_voltage = filter_data_now[2*i].v_sense;
        record_data.channel_record_data[i].output_current = filter_data_now[2*i].i_out + filter_data_now[2*i+1].i_out;
        record_data.channel_record_data[i].output_voltage = filter_data_now[2*i].v_mod;
        record_data.channel_record_data[i].v_j = filter_data_now[2*i].v_j;
        // record_data.channel_record_data[i].battery_temperature = filter_data_now[2*i].battery_temperature>filter_data_now[2*i+1].battery_temperature?filter_data_now[2*i].battery_temperature:filter_data_now[2*i+1].battery_temperature;
        record_data.channel_record_data[i].module_temperature = filter_data_now[2*i].mod_tempetature>filter_data_now[2*i+1].mod_tempetature?filter_data_now[2*i].mod_tempetature:filter_data_now[2*i+1].mod_tempetature;
        record_data.channel_record_data[i].crosscurrent_ratio = 0;


        if((running_step[i].status == step_running_status) && (running_step[i].type != standby)) {
            record_data.channel_record_data[i].battery_capacitance += record_data.channel_record_data[i].output_current * INTEGRAL_TIME / 3600.0;
            record_data.channel_record_data[i].battery_energy += record_data.channel_record_data[i].battery_voltage * record_data.channel_record_data[i].output_current * INTEGRAL_TIME / 3600.0;

            if((running_step[i].type == CC) || (running_step[i].type == CV) || (running_step[i].type == CC_to_CV)) {
                record_data.channel_record_data[i].sum_capacitance = 0;
            } else if((running_step[i].type == DCC) || (running_step[i].type == DCV) || (running_step[i].type == DCC_to_DCV)) {
                record_data.channel_record_data[i].sum_capacitance = 0;//record_data.channel_record_data[i].battery_capacitance;
            }

        }
        // else {
        //     // record_data.channel_record_data[i].output_current = 0;

        //     record_data.channel_record_data[i].contact_impedance = 0;
        //     record_data.channel_record_data[i].loop_impedance = 0;
        // }

        // if(record_data.channel_record_data[i].output_current > 0.0) {
        //     record_data.channel_record_data[i].contact_impedance = fabs(record_data.channel_record_data[i].v_j) / record_data.channel_record_data[i].output_current;
        //     record_data.channel_record_data[i].loop_impedance = fabs(record_data.channel_record_data[i].battery_voltage - record_data.channel_record_data[i].output_voltage) / record_data.channel_record_data[i].output_current;
        // }

        if(record_data.channel_record_data[i].output_current >= 5.0) {
            record_data.channel_record_data[i].contact_impedance = fabs(record_data.channel_record_data[i].v_j-record_data.channel_record_data[i].battery_voltage) / record_data.channel_record_data[i].output_current;
            record_data.channel_record_data[i].loop_impedance = fabs(record_data.channel_record_data[i].battery_voltage - record_data.channel_record_data[i].output_voltage) / record_data.channel_record_data[i].output_current;
        } else {
            record_data.channel_record_data[i].contact_impedance = 0;
            record_data.channel_record_data[i].loop_impedance = 0;
        }

    }
#endif

#if CHANNELS_8
    for(int i = 0; i < CHANNELS; i++) {
        record_data.channel_record_data[i].run_time = (float)running_step[i].run_time;
        record_data.channel_record_data[i].battery_voltage = filter_data_now[i].v_sense;
        record_data.channel_record_data[i].output_current = filter_data_now[i].i_out;
        record_data.channel_record_data[i].output_voltage = filter_data_now[i].v_mod;
        record_data.channel_record_data[i].v_j = filter_data_now[i].v_j;
        // record_data.channel_record_data[i].battery_temperature = filter_data_now[i].battery_temperature;
        record_data.channel_record_data[i].module_temperature = filter_data_now[i].mod_tempetature;
        record_data.channel_record_data[i].crosscurrent_ratio = 0;


        if((running_step[i].status == step_running_status) && (running_step[i].type != standby)) {
            record_data.channel_record_data[i].battery_capacitance += record_data.channel_record_data[i].output_current * INTEGRAL_TIME / 3600.0;
            record_data.channel_record_data[i].battery_energy += record_data.channel_record_data[i].battery_voltage * record_data.channel_record_data[i].output_current * INTEGRAL_TIME / 3600.0;

            if((running_step[i].type == CC) || (running_step[i].type == CV) || (running_step[i].type == CC_to_CV)) {
                record_data.channel_record_data[i].sum_capacitance += record_data.channel_record_data[i].battery_capacitance;
            } else if((running_step[i].type == DCC) || (running_step[i].type == DCV) || (running_step[i].type == DCC_to_DCV)) {
                record_data.channel_record_data[i].sum_capacitance -= record_data.channel_record_data[i].battery_capacitance;
            }
        }

        if(record_data.channel_record_data[i].output_current >= 5.0) {
            record_data.channel_record_data[i].contact_impedance = fabs(record_data.channel_record_data[i].v_j-record_data.channel_record_data[i].battery_voltage) / record_data.channel_record_data[i].output_current;
            record_data.channel_record_data[i].loop_impedance = fabs(record_data.channel_record_data[i].battery_voltage - record_data.channel_record_data[i].output_voltage) / record_data.channel_record_data[i].output_current;
        } else {
            record_data.channel_record_data[i].contact_impedance = 0;
            record_data.channel_record_data[i].loop_impedance = 0;
        }

    }
#endif

    record_data.Bus1 = special_sample.Bus1;
    record_data.Bus2 = special_sample.Bus2;
    record_data.Aux = special_sample.Aux;
    record_data.Amb = special_sample.Amb;
    //----------------------------------------------------------------------------------------------

    //检测 CC 转 CV---------------------------------------------------------------------------------
    if(SYS_MODE == SYS_NORMAL_MODE) {
        for(int i = 0; i < CHANNELS; i++) {
            if((running_step[i].status == step_running_status) && (CV_mode[i] == 0) && (DCV_mode[i] == 0)) {
                if((running_step[i].type == CV) || (running_step[i].type == CC_to_CV)) {
                    if((fabs(record_data.channel_record_data[i].output_current-step_work[i].step_work_current) >= 0.02) && (fabs(record_data.channel_record_data[i].battery_voltage-step_work[i].step_work_voltage) <= 0.005) ) {
                        CV_mode[i] = 1;
                    }
                } else if((running_step[i].type == DCV) || (running_step[i].type == DCC_to_DCV)) {
                    if((fabs(record_data.channel_record_data[i].output_current-step_work[i].step_work_current) >= 0.02) && (fabs(record_data.channel_record_data[i].battery_voltage-step_work[i].step_work_voltage) <= 0.005) ) {
                        DCV_mode[i] = 1;
                    }
                }

            }
        }

    }

    //----------------------------------------------------------------------------------------------

    //保护判断以及上报-------------------------------------------------------------------------------
    Protection_Judgment();
    // if((SYS_MODE == SYS_NORMAL_MODE) && (TCP_SERVER_FLAG & 0x20)) {
    if(SYS_MODE == SYS_NORMAL_MODE) {
        for(int i = 0; i < CHANNELS; i++) {
            if((STEP_PROT_FLAG[i] == 0x01) ||(STEP_PROT_FLAG[i] == 0x02)) {
                // led_run_setting(0x01<<i, led_error, &(led_control));
                // led_run_setting(0, led_error, &(led_control));

                if(TCP_SERVER_FLAG & 0x20) {
                    Step_Protection_Report(0x01<<i);
                    // HAL_Delay(200);
                    Record_Report(0x01 << i);

                    STEP_PROT_FLAG[i] = 0x03;
                } else {
                    STEP_PROT_FLAG[i] = 0x02;
                }
            }

            if((GLOBAL_PROT_FLAG[i] == 0x01) || (GLOBAL_PROT_FLAG[i] == 0x02)) {
                // led_run_setting(0x01<<i, led_error, &(led_control));
                // led_run_setting(0, led_error, &(led_control));

                if(TCP_SERVER_FLAG & 0x20) {
                    Global_Protection_Report(0x01<<i);
                    // HAL_Delay(200);
                    Record_Report(0x01 << i);

                    GLOBAL_PROT_FLAG[i] = 0x03;
                } else {
                    GLOBAL_PROT_FLAG[i] = 0x02;
                }
            }
        }
    } else if((SYS_MODE == SYS_CALIBRATION_MODE) && (TCP_SERVER_FLAG & 0x20)) {
        if(GLOBAL_PROT_FLAG[0] == 0x01) {
            // HAL_Delay(50);
            Global_Protection_Report(0x01);

            led_run_setting(0, led_error, &(led_control));
            GLOBAL_PROT_FLAG[0] = 0x02;
        }
    }


    //----------------------------------------------------------------------------------------------

    // if(SYS_MODE == SYS_NORMAL_MODE) {
    //     //判断工步运行截止条件，满足条件则切换工步------------------------------------------------------
    //     Running_Switch();
    //     //------------------------------------------------------------------------------------------
    // }

    if(TCP_SERVER_FLAG & 0x01) {
        LED82 = ~LED82;
        TCP_SERVER_FLAG &= ~(0x01);
    }
}

void Task_500ms_Callback(void)//LED的闪烁
{
    IWDG->KR=0XAAAA;    //reload

    LED81 = ~LED81;

    for(int i = 0; i < CHANNELS; i++) {
        if(running_step[i].status == step_running_status) {
            if( (running_step[i].type == DCC) || \
                (running_step[i].type == DCV) || \
                (running_step[i].type == DCC_to_DCV) || \
                (running_step[i].type == DCP)) {
                led_run_setting(0x01<<i, led_discharge, &(led_control));
            }
        }
    }
}

void Task_1000ms_Callback(void)
{
    //老化时的运行灯及时间操作------------------------------------------------------------------------
    if(ageing_flag == 0x00) {
        if(SYS_MODE == SYS_AGEING_MODE) {
            ageing_time++;
            led_run_setting(0x01<<0, led_charge, &(led_control));
            led_run_setting(0x01<<1, led_charge, &(led_control));
            led_run_setting(0x01<<2, led_discharge, &(led_control));
            led_run_setting(0x01<<3, led_discharge, &(led_control));
        } else if(SYS_MODE == SYS_SELFTEST_MODE) {
            ageing_time++;
        }
    }
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    if(SYS_MODE == SYS_NORMAL_MODE) {
        for(int i = 0; i < CHANNELS; i++) {
            //工步运行时的指示灯操作------------------------------------------------------------------
            if(running_step[i].status == step_running_status) {
                if( (running_step[i].type == CC) || \
                    (running_step[i].type == CV) || \
                    (running_step[i].type == CC_to_CV) || \
                    (running_step[i].type == CP)) {
                    led_run_setting(0x01<<i, led_charge, &(led_control));
                }
            }
            //--------------------------------------------------------------------------------------

            //待机时的数据上传-----------------------------------------------------------------------
            else if((running_step[i].status == step_standby_status) && (TCP_SERVER_FLAG & 0x20)) {
                 Record_Report(0x01 << i);
            }
            //--------------------------------------------------------------------------------------
        }

        //心跳检测-----------------------------------------------------------------------------------
        // if(TCP_SERVER_FLAG & 0x20) {
        //     if(heartbeat_flag == 0x01) {
        //         heartbeat_count = 0;
        //         heartbeat_flag = 0x00;
        //     } else {
        //         heartbeat_count++;
        //         if(heartbeat_count >= 30) {
        //             for(int i = 0; i < CHANNELS; i++) {
        //                 Step_Turn_Off(0x01<<i);
        //                 running_step[i].status = step_stop_status;
        //             }

        //             TCP_SERVER_FLAG &= ~0x20;
        //             heartbeat_count = 0;
        //         }
        //     }
        // }
        //------------------------------------------------------------------------------------------
    }

    //----------------------------------------------------------------------------------------------


    // ACDC_DCI_Query_CMD(0);
    // ACDC_DCI_Query_CMD(1);
    // ACDC_DCI_Query_CMD(0xff);

    // ACDC_Fan_Speed_CMD(0, 20+((ACDC_DCI[0]/Iport_MAX)*2));
    // ACDC_Fan_Speed_CMD(1, 20+((ACDC_DCI[1]/Iport_MAX)*2));
    // DCI = ACDC_DCI[0]>ACDC_DCI[1]? ACDC_DCI[0] : ACDC_DCI[0];
    // ACDC_Fan_Speed_CMD(0xff, 20+((DCI/Iport_MAX)*8));

// #if CHANNELS_1
//     fan_I = record_data.channel_record_data[0].output_current;
// #endif
// #if CHANNELS_2
//     fan_I = record_data.channel_record_data[0].output_current>record_data.channel_record_data[1].output_current?record_data.channel_record_data[0].output_current:record_data.channel_record_data[1].output_current;
//     // if(record_data.channel_record_data[0].output_current > record_data.channel_record_data[1].output_current) {
//     //     fan_I = record_data.channel_record_data[0].output_current;
//     // } else {
//     //     fan_I = record_data.channel_record_data[1].output_current;
//     // }
// #endif
// #if CHANNELS_4
//     fan[0] = record_data.channel_record_data[0].output_current>record_data.channel_record_data[1].output_current?record_data.channel_record_data[0].output_current:record_data.channel_record_data[1].output_current;
//     fan[1] = record_data.channel_record_data[2].output_current>record_data.channel_record_data[3].output_current?record_data.channel_record_data[2].output_current:record_data.channel_record_data[3].output_current;
//     fan_I = fan[0]>fan[1]?fan[0]:fan[1];
//     // if(record_data.channel_record_data[0].output_current > record_data.channel_record_data[1].output_current) {
//     //     if(record_data.channel_record_data[0].output_current > record_data.channel_record_data[2].output_current) {
//     //         if(record_data.channel_record_data[0].output_current > record_data.channel_record_data[3].output_current) {
//     //             fan_I = record_data.channel_record_data[0].output_current;
//     //         } else {
//     //             fan_I = record_data.channel_record_data[3].output_current;
//     //         }
//     //     } else if(record_data.channel_record_data[2].output_current > record_data.channel_record_data[3].output_current) {
//     //         fan_I = record_data.channel_record_data[2].output_current;
//     //     } else {
//     //         fan_I = record_data.channel_record_data[3].output_current;
//     //     }
//     // } else if(record_data.channel_record_data[1].output_current > record_data.channel_record_data[2].output_current) {
//     //     if(record_data.channel_record_data[1].output_current > record_data.channel_record_data[3].output_current) {
//     //         fan_I = record_data.channel_record_data[1].output_current;
//     //     } else {
//     //         fan_I = record_data.channel_record_data[3].output_current;
//     //     }
//     // } else if(record_data.channel_record_data[2].output_current > record_data.channel_record_data[3].output_current) {
//     //     fan_I = record_data.channel_record_data[2].output_current;
//     // } else {
//     //     fan_I = record_data.channel_record_data[3].output_current;
//     // }
// #endif
// #if CHANNELS_8
//     fan[0] = record_data.channel_record_data[0].output_current>record_data.channel_record_data[1].output_current?record_data.channel_record_data[0].output_current:record_data.channel_record_data[1].output_current;
//     fan[1] = record_data.channel_record_data[2].output_current>record_data.channel_record_data[3].output_current?record_data.channel_record_data[2].output_current:record_data.channel_record_data[3].output_current;
//     fan[2] = record_data.channel_record_data[4].output_current>record_data.channel_record_data[5].output_current?record_data.channel_record_data[4].output_current:record_data.channel_record_data[5].output_current;
//     fan[3] = record_data.channel_record_data[6].output_current>record_data.channel_record_data[7].output_current?record_data.channel_record_data[6].output_current:record_data.channel_record_data[7].output_current;

//     fan[4] = fan[0]>fan[1]?fan[0]:fan[1];
//     fan[5] = fan[2]>fan[3]?fan[2]:fan[3];

//     fan_I = fan[4]>fan[5]?fan[4]:fan[5];
// #endif

//     if(I_MAX < Fan_I_Max) {
//         DCI = fan_I/I_MAX*80;
//     } else {
//         DCI = fan_I/Fan_I_Max*80;
//     }
//     ACDC_Fan_Speed_CMD(0xff, 20+DCI);

    fan[0] = filter_data_now[0].i_out>filter_data_now[1].i_out?filter_data_now[0].i_out:filter_data_now[1].i_out;
    fan[1] = filter_data_now[2].i_out>filter_data_now[3].i_out?filter_data_now[2].i_out:filter_data_now[3].i_out;
    fan[2] = filter_data_now[4].i_out>filter_data_now[5].i_out?filter_data_now[4].i_out:filter_data_now[5].i_out;
    fan[3] = filter_data_now[6].i_out>filter_data_now[7].i_out?filter_data_now[6].i_out:filter_data_now[7].i_out;

    fan[4] = fan[0]>fan[1]?fan[0]:fan[1];
    fan[5] = fan[2]>fan[3]?fan[2]:fan[3];

    fan_I = fan[4]>fan[5]?fan[4]:fan[5];
    DCI = fan_I;
    ACDC_Fan_Speed_CMD(0xff, 20+DCI);

    if(TCP_SERVER_FLAG & 0x20) {
        LED83 = 0;
    } else {
        LED83 = 1;
    }
}

void Task_Add(void)
{
    TasksPare[0].Task_Period=Task1ms_COUNT;//1ms
    TasksPare[0].Task_Count=0;
    TasksPare[0].Task_Function=Task_1ms_Callback;

    TasksPare[1].Task_Period=Task10ms_COUNT;//10ms
    TasksPare[1].Task_Count=0;
    TasksPare[1].Task_Function=Task_10ms_Callback;

    TasksPare[2].Task_Period=Task25ms_COUNT;//25ms
    TasksPare[2].Task_Count=0;
    TasksPare[2].Task_Function=Task_25ms_Callback;

    TasksPare[3].Task_Period=Task100ms_COUNT;//100ms
    TasksPare[3].Task_Count=0;
    TasksPare[3].Task_Function= Task_100ms_Callback;

    TasksPare[4].Task_Period = Task500ms_COUNT;//500ms
    TasksPare[4].Task_Count=0;
    TasksPare[4].Task_Function = Task_500ms_Callback;

    TasksPare[5].Task_Period = Task1000ms_COUNT;//1000ms
    TasksPare[5].Task_Count=0;
    TasksPare[5].Task_Function = Task_1000ms_Callback;
}

static uint16_t sample_counter = 0;
static uint16_t ad_count = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == htim1.Instance)
    {
        Timer_Task_Count( );//100us
    }
    if(htim->Instance == htim2.Instance)
    {
        LED82 = 1;
        Time_Stamp++;

        for(int i = 0; i < 8; i++) {
            ad_sample[i].v_sense += ad7175_value[i];
            ad_sample[i].i_out += ad7175_value[i+8];
            ad_sample[i].v_j += ad7124_value[i*2];

            calibration_ad_add[i].Vsense_AD += ad7175_value[i];
            calibration_ad_add[i].Vmod_AD += ADCx_Vmod[i];
            calibration_ad_add[i].Iout_AD += ad7175_value[i+8];
            calibration_ad_add[i].Vj_AD += ad7124_value[i*2];
        }

        sample_counter++;
        ad_count++;

        //AD7175采样,24位；7124采样，24位-------------------------------------------------------------
        if(sample_counter >= SAMPLE_NUMBER) {
            for(int i = 0; i < 8; i++) {
                if(Dir_flag[i] == BUCK_MODE) {
                    filter_data_now[i].v_sense = (float)((calibration[i].Buck_Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 16777216) + calibration[i].Buck_Vsense_B;

                    if(Irange_flag[i] == I_LOW) {
                        filter_data_now[i].i_out = (float)((calibration[i].Buck_Iout_LK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Buck_Iout_LB;
                    } else if(Irange_flag[i] == I_HIGH) {
                        filter_data_now[i].i_out = (float)((calibration[i].Buck_Iout_HK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Buck_Iout_HB;
                    }

                    // filter_data_now[i].v_j = (float)((calibration[i].Buck_Vj_K * (ad_sample[i].v_j / sample_counter)) / 16777216) + calibration[i].Buck_Vj_B;
                } else if(Dir_flag[i] == BOOST_MODE) {
                    filter_data_now[i].v_sense = (float)((calibration[i].Boost_Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 16777216) + calibration[i].Boost_Vsense_B;

                    if(Irange_flag[i] == I_LOW) {
                        filter_data_now[i].i_out = (float)((calibration[i].Boost_Iout_LK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Boost_Iout_LB;
                    } else if(Irange_flag[i] == I_HIGH) {
                        filter_data_now[i].i_out = (float)((calibration[i].Boost_Iout_HK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Boost_Iout_HB;
                    }

                    // filter_data_now[i].v_j = (float)((calibration[i].Boost_Vj_K * (ad_sample[i].v_j / sample_counter)) / 16777216) + calibration[i].Boost_Vj_B;
                } else {
                    filter_data_now[i].v_sense = 0;
                    filter_data_now[i].i_out = 0;
                    filter_data_now[i].v_j = 0;
                }

                filter_data_now[i].v_j = (float)((calibration[i].Buck_Vj_K * (ad_sample[i].v_j / sample_counter)) / 16777216) + calibration[i].Buck_Vj_B;


                ad_sample[i].v_sense = 0;
                ad_sample[i].i_out = 0;
                ad_sample[i].v_j = 0;
            }

            sample_counter = 0;
        }
        //------------------------------------------------------------------------------------------

        //校准 AD 累加值处理-------------------------------------------------------------------------
        if(ad_count >= AD_NUMBER) {
            for(int i = 0; i < 8; i++) {
                calibration_ad_record[i].Vsense_AD = calibration_ad_add[i].Vsense_AD;
                calibration_ad_record[i].Vmod_AD = calibration_ad_add[i].Vmod_AD;
                calibration_ad_record[i].Iout_AD = calibration_ad_add[i].Iout_AD;
                calibration_ad_record[i].Vj_AD = calibration_ad_add[i].Vj_AD;

                calibration_ad_add[i].Vsense_AD = 0;
                calibration_ad_add[i].Vmod_AD = 0;
                calibration_ad_add[i].Iout_AD = 0;
                calibration_ad_add[i].Vj_AD = 0;
            }

            ad_count = 0;
        }
        //------------------------------------------------------------------------------------------

        //运行时间累加-------------------------------------------------------------------------------
        for(int i = 0; i < CHANNELS; i++) {
            if(SYS_MODE == SYS_NORMAL_MODE) {
                if((running_step[i].status == step_running_status) || (running_step[i].status == step_contact_status)) {
                    running_step[i].run_time++;
                    running_step[i].protection_time++;
                }
            }
        }
        //------------------------------------------------------------------------------------------

        LED82 = 0;
    }
}

// USER CODE END
