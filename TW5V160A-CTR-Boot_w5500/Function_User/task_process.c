/*
*********************************************************************************************************
*
*	ģ������ : ������
*	�ļ����� : task_process.c
*	��    �� : V1.0
*	˵    �� :
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0    2020-03-12   ZYL      ��ʽ����
*********************************************************************************************************
*/

#include "main.h"
#include "data_analysis.h"
#include "step_work.h"
#include "ad7175_8.h"
#include <math.h>

#define Task_Num   6   //����<Task_Num

#define Task1ms_COUNT         10        //1ms
#define Task10ms_COUNT        0//100       //10ms
// #define Task20ms_COUNT        200   //20ms
#define Task25ms_COUNT        250       //25ms
#define Task100ms_COUNT       1000      //100ms
#define Task500ms_COUNT       5000      //500ms
#define Task1000ms_COUNT      10000     //1s

static uint8_t adc_count = 0;

TaskTime  TasksPare[Task_Num];

static uint16_t sample_counter = 0;
static uint16_t ad_count = 0;

void Timer_Task_Count(void)  //�¼��������
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
//    for(int i = 0; i < 8; i++) {
//        ad_sample[i].v_sense += ad7175_value[i];
//        ad_sample[i].i_out += ad7175_value[i+8];
//        ad_sample[i].v_j += ad7124_value[i*2];

//        calibration_ad_add[i].Vsense_AD += ad7175_value[i];
//        calibration_ad_add[i].Vmod_AD += ADCx_Vmod[i];
//        calibration_ad_add[i].Iout_AD += ad7175_value[i+8];
//        calibration_ad_add[i].Vj_AD += ad7124_value[i*2];
//    }

//    sample_counter++;
//    ad_count++;

//    //AD7175����,24λ��7124������23λ-----------------------------------------------------------------
//    if(sample_counter >= SAMPLE_NUMBER) {
//        for(int i = 0; i < 8; i++) {
//            if(Dir_flag[i] == BUCK_MODE) {
//                filter_data_now[i].v_sense = (float)((calibration[i].Buck_Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 16777216) + calibration[i].Buck_Vsense_B;

//                if(Irange_flag[i] == I_LOW) {
//                    filter_data_now[i].i_out = (float)((calibration[i].Buck_Iout_LK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Buck_Iout_LB;
//                } else if(Irange_flag[i] == I_HIGH) {
//                    filter_data_now[i].i_out = (float)((calibration[i].Buck_Iout_HK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Buck_Iout_HB;
//                }

//                filter_data_now[i].v_j = (float)((calibration[i].Buck_Vj_K * (ad_sample[i].v_j / sample_counter)) / 8388608) + calibration[i].Buck_Vj_B;
//            } else if(Dir_flag[i] == BOOST_MODE) {
//                filter_data_now[i].v_sense = (float)((calibration[i].Boost_Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 16777216) + calibration[i].Boost_Vsense_B;

//                if(Irange_flag[i] == I_LOW) {
//                    filter_data_now[i].i_out = (float)((calibration[i].Boost_Iout_LK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Boost_Iout_LB;
//                } else if(Irange_flag[i] == I_HIGH) {
//                    filter_data_now[i].i_out = (float)((calibration[i].Boost_Iout_HK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Boost_Iout_HB;
//                }

//                filter_data_now[i].v_j = (float)((calibration[i].Boost_Vj_K * (ad_sample[i].v_j / sample_counter)) / 8388608) + calibration[i].Boost_Vj_B;
//            } else {
//                filter_data_now[i].v_sense = 0;
//                filter_data_now[i].i_out = 0;
//                filter_data_now[i].v_j = 0;
//            }


//            ad_sample[i].v_sense = 0;
//            ad_sample[i].i_out = 0;
//            ad_sample[i].v_j = 0;
//        }

//        sample_counter = 0;
//    }
//    //----------------------------------------------------------------------------------------------

//    //У׼ AD �ۼ�ֵ����-----------------------------------------------------------------------------
//    if(ad_count >= AD_NUMBER) {
//        for(int i = 0; i < 8; i++) {
//            calibration_ad_record[i].Vsense_AD = calibration_ad_add[i].Vsense_AD;
//            calibration_ad_record[i].Vmod_AD = calibration_ad_add[i].Vmod_AD;
//            calibration_ad_record[i].Iout_AD = calibration_ad_add[i].Iout_AD;
//            calibration_ad_record[i].Vj_AD = calibration_ad_add[i].Vj_AD;

//            calibration_ad_add[i].Vsense_AD = 0;
//            calibration_ad_add[i].Vmod_AD = 0;
//            calibration_ad_add[i].Iout_AD = 0;
//            calibration_ad_add[i].Vj_AD = 0;
//        }

//        ad_count = 0;
//    }
//    //----------------------------------------------------------------------------------------------

//#if CHANNELS_1

//#endif
//#if CHANNELS_2

//#endif
//#if CHANNELS_4
//    if(SYS_MODE == SYS_NORMAL_MODE) {
//        if(running_step[0].status == step_running_status) {
//            running_step[0].run_time++;
//            running_step[0].protection_time++;
//        }
//        if(running_step[1].status == step_running_status) {
//            running_step[1].run_time++;
//            running_step[1].protection_time++;
//        }
//        if(running_step[2].status == step_running_status) {
//            running_step[2].run_time++;
//            running_step[2].protection_time++;
//        }
//        if(running_step[3].status == step_running_status) {
//            running_step[3].run_time++;
//            running_step[3].protection_time++;
//        }
//    }
//#endif
//#if CHANNELS_8

//#endif

//    if(SYS_MODE == SYS_NORMAL_MODE) {
//        //�жϼ�¼�����ϴ������������������ϴ���¼����--------------------------------------------------
//        for(int i = 0; i < CHANNELS; i++) {
//            if(running_step[i].status == step_running_status) {
//                this_record_condition[i].record_current = record_data.channel_record_data[i].output_current;
//                this_record_condition[i].record_voltage = record_data.channel_record_data[i].battery_voltage;
//                this_record_condition[i].record_time    = running_step[i].run_time;

//                if( ((FLOAT_NOT_ZERO(step_record_condition[i].record_current) > 0) && (fabs(this_record_condition[i].record_current - last_record_condition[i].record_current) >= step_record_condition[i].record_current)) ||
//                    ((FLOAT_NOT_ZERO(step_record_condition[i].record_voltage) > 0) && (fabs(this_record_condition[i].record_voltage - last_record_condition[i].record_voltage) >= step_record_condition[i].record_voltage)) ||
//                    ((FLOAT_NOT_ZERO(step_record_condition[i].record_time) > 0) && ((this_record_condition[i].record_time - last_record_condition[i].record_time) >= (step_record_condition[i].record_time*1000))) ) {

//                    Record_Report(0x01 << i);

//                    last_record_condition[i].record_current = this_record_condition[i].record_current;
//                    last_record_condition[i].record_voltage = this_record_condition[i].record_voltage;
//                    last_record_condition[i].record_time = this_record_condition[i].record_time;

//                    //�����������ݼ�¼---------------------------------------------------------------
//                    fluctuation_protection_now[i].voltage = fabs(recode_query[i].battery_voltage);
//                    // fluctuation_protection_now[i].voltage_time = recode_query[i].run_time;
//                    fluctuation_protection_now[i].voltage_time = running_step[i].protection_time;
//                    fluctuation_protection_now[i].current = fabs(recode_query[i].output_current);
//                    // fluctuation_protection_now[i].current_time = recode_query[i].run_time;
//                    fluctuation_protection_now[i].current_time = running_step[i].protection_time;
//                    //------------------------------------------------------------------------------

//                    //���䱣�����ݼ�¼---------------------------------------------------------------
//                    jump_protection_old[i].current = jump_protection_now[i].current;
//                    jump_protection_old[i].voltage = jump_protection_now[i].voltage;

//                    jump_protection_now[i].current = fabs(recode_query[i].output_current);
//                    jump_protection_now[i].voltage = fabs(recode_query[i].battery_voltage);

//                    jump_flag[i] = 1;
//                    //------------------------------------------------------------------------------

//                    // //��� CC ת CV------------------------------------------------------------------
//                    // if((recode_query[i].run_status == step_running_status) && (CV_mode[i] == 0) && (DCV_mode[i] == 0)) {
//                    //     // if(recode_query[i].run_status == step_running_status) {
//                    //         if((recode_query[i].step_type == CV) || (recode_query[i].step_type == CC_to_CV)) {
//                    //             if((fabs(recode_query[i].output_current-step_work[i].step_work_current) >= 0.02) && (fabs(recode_query[i].battery_voltage-step_work[i].step_work_voltage) <= 0.1) ) {
//                    //                 CV_mode[i] = 1;
//                    //             }
//                    //         } else if((recode_query[i].step_type == DCV) || (recode_query[i].step_type == DCC_to_DCV)) {
//                    //             if((fabs(recode_query[i].output_current-step_work[i].step_work_current) >= 0.02) && (fabs(recode_query[i].battery_voltage-step_work[i].step_work_voltage) <= 0.1) ) {
//                    //                 DCV_mode[i] = 1;
//                    //             }
//                    //         }
//                    //     // }
//                    // }
//                    // //------------------------------------------------------------------------------

//                }
//            }
//        }
//        //------------------------------------------------------------------------------------------

//    }

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
    //ADC_Start_Stop_DMA();
	  #if w5500_EN
		Socket_Work_Timeout_Check();//30ms
		#endif    
}


void Task_100ms_Callback(void)
{
//    //���� AD ������12λ-----------------------------------------------------------------------------
//    for(int i = 0; i < 8; i++) {
//        if(Dir_flag[i] == BUCK_MODE) {
//            filter_data_now[i].v_mod = (float)((calibration[i].Buck_Vmod_K * ADCx_Vmod[i]) / 4096) + calibration[i].Buck_Vmod_B;
//        } else if(Dir_flag[i] == BOOST_MODE) {
//            filter_data_now[i].v_mod = (float)((calibration[i].Boost_Vmod_K * ADCx_Vmod[i]) / 4096) + calibration[i].Boost_Vmod_B;

//        }

//        filter_data_now[i].mod_tempetature = (float)((special_calibration.Temperature_K * ADCx_Temp[i]) / 4096) + special_calibration.Temperature_B;

//    }

//    special_sample.Bus1 = (float)((special_calibration.Bus1_K * AD_Bus1) / 4096) + special_calibration.Bus1_B;
//    special_sample.Bus2 = (float)((special_calibration.Bus2_K * AD_Bus2) / 4096) + special_calibration.Bus2_B;
//    special_sample.Aux = (float)((special_calibration.Aux_K * AD_Aux) / 4096) + special_calibration.Aux_B;
//    special_sample.Amb = (float)((special_calibration.Amb_K * AD_Amb) / 4096) + special_calibration.Amb_B;
//    //----------------------------------------------------------------------------------------------

//    // //AD7175����,24λ��7124������23λ-----------------------------------------------------------------
//    // for(int i = 0; i < 8; i++) {
//    //     if(Dir_flag[i] == BUCK_MODE) {
//    //         filter_data_now[i].v_sense = (float)((calibration[i].Buck_Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 16777216) + calibration[i].Buck_Vsense_B;
//    //         // filter_data_now[i].v_sense = (float)((calibration[i].Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 65536) + calibration[i].Vsense_B;
//    //         if(Irange_flag[i] == I_LOW) {
//    //             filter_data_now[i].i_out = (float)((calibration[i].Buck_Iout_LK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Buck_Iout_LB;
//    //             // filter_data_now[i].i_out = (float)((calibration[i].Iout_LK * (ad_sample[i].i_out / sample_counter)) / 65536) + calibration[i].Iout_LB;
//    //         } else if(Irange_flag[i] == I_HIGH) {
//    //             filter_data_now[i].i_out = (float)((calibration[i].Buck_Iout_HK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Buck_Iout_HB;
//    //             // filter_data_now[i].i_out = (float)((calibration[i].Iout_HK * (ad_sample[i].i_out / sample_counter)) / 65536) + calibration[i].Iout_HB;
//    //         }
//    //         filter_data_now[i].v_j = (float)((calibration[i].Buck_Vj_K * (ad_sample[i].v_j / sample_counter)) / 16777216) + calibration[i].Buck_Vj_B;
//    //     } else if(Dir_flag[i] == BOOST_MODE) {
//    //         filter_data_now[i].v_sense = (float)((calibration[i].Boost_Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 16777216) + calibration[i].Boost_Vsense_B;
//    //         // filter_data_now[i].v_sense = (float)((calibration[i].Vsense_K * (ad_sample[i].v_sense / sample_counter)) / 65536) + calibration[i].Vsense_B;
//    //         if(Irange_flag[i] == I_LOW) {
//    //             filter_data_now[i].i_out = (float)((calibration[i].Boost_Iout_LK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Boost_Iout_LB;
//    //             // filter_data_now[i].i_out = (float)((calibration[i].Iout_LK * (ad_sample[i].i_out / sample_counter)) / 65536) + calibration[i].Iout_LB;
//    //         } else if(Irange_flag[i] == I_HIGH) {
//    //             filter_data_now[i].i_out = (float)((calibration[i].Boost_Iout_HK * (ad_sample[i].i_out / sample_counter)) / 16777216) + calibration[i].Boost_Iout_HB;
//    //             // filter_data_now[i].i_out = (float)((calibration[i].Iout_HK * (ad_sample[i].i_out / sample_counter)) / 65536) + calibration[i].Iout_HB;
//    //         }
//    //         filter_data_now[i].v_j = (float)((calibration[i].Boost_Vj_K * (ad_sample[i].v_j / sample_counter)) / 16777216) + calibration[i].Boost_Vj_B;
//    //     }


//    //     ad_sample[i].v_sense = 0;
//    //     ad_sample[i].i_out = 0;
//    //     ad_sample[i].v_j = 0;
//    // }

//    // sample_counter = 0;
//    // //----------------------------------------------------------------------------------------------

//    //��ȡ�������������------------------------------------------------------------------------------
//    for(int i = 0; i < CHANNELS; i++) {
//        record_data.channel_record_data[i].run_time = (float)running_step[i].run_time;
//        record_data.channel_record_data[i].battery_voltage = filter_data_now[2*i].v_sense;
//        record_data.channel_record_data[i].output_current = filter_data_now[2*i].i_out + filter_data_now[2*i+1].i_out;
//        record_data.channel_record_data[i].output_voltage = filter_data_now[2*i].v_mod;
//        record_data.channel_record_data[i].v_j = filter_data_now[2*i].v_j;
//        record_data.channel_record_data[i].battery_temperature = filter_data_now[2*i].battery_temperature;
//        record_data.channel_record_data[i].module_temperature = filter_data_now[2*i].mod_tempetature;
//        record_data.channel_record_data[i].crosscurrent_ratio = 0;


//        if((running_step[i].status == step_running_status) && (running_step[i].type != standby)) {
//            // record_data.channel_record_data[i].output_current = filter_data_now[2*i].i_out + filter_data_now[2*i+1].i_out;

//            record_data.channel_record_data[i].battery_capacitance += record_data.channel_record_data[i].output_current * INTEGRAL_TIME / 3600.0;
//            record_data.channel_record_data[i].battery_energy += record_data.channel_record_data[i].battery_voltage * record_data.channel_record_data[i].output_current * INTEGRAL_TIME / 3600.0;

//            // record_data.channel_record_data[i].contact_impedance = record_data.channel_record_data[i].v_j / record_data.channel_record_data[i].output_current;
//            // record_data.channel_record_data[i].loop_impedance = fabs(record_data.channel_record_data[i].battery_voltage - record_data.channel_record_data[i].output_voltage) / record_data.channel_record_data[i].output_current;
//        }
//        // else {
//        //     // record_data.channel_record_data[i].output_current = 0;

//        //     record_data.channel_record_data[i].contact_impedance = 0;
//        //     record_data.channel_record_data[i].loop_impedance = 0;
//        // }

//        // if(record_data.channel_record_data[i].output_current > 0.0) {
//        //     record_data.channel_record_data[i].contact_impedance = fabs(record_data.channel_record_data[i].v_j) / record_data.channel_record_data[i].output_current;
//        //     record_data.channel_record_data[i].loop_impedance = fabs(record_data.channel_record_data[i].battery_voltage - record_data.channel_record_data[i].output_voltage) / record_data.channel_record_data[i].output_current;
//        // }

//        if(record_data.channel_record_data[i].output_current >= 5.0) {
//            record_data.channel_record_data[i].contact_impedance = fabs(record_data.channel_record_data[i].v_j) / record_data.channel_record_data[i].output_current;
//            record_data.channel_record_data[i].loop_impedance = fabs(record_data.channel_record_data[i].battery_voltage - record_data.channel_record_data[i].output_voltage) / record_data.channel_record_data[i].output_current;
//        } else {
//            record_data.channel_record_data[i].contact_impedance = 0;
//            record_data.channel_record_data[i].loop_impedance = 0;
//        }

//    }
//    record_data.Bus1 = special_sample.Bus1;
//    record_data.Bus2 = special_sample.Bus2;
//    record_data.Aux = special_sample.Aux;
//    record_data.Amb = special_sample.Amb;
//    //----------------------------------------------------------------------------------------------

//    //��� CC ת CV---------------------------------------------------------------------------------
//    if(SYS_MODE == SYS_NORMAL_MODE) {
//        for(int i = 0; i < CHANNELS; i++) {
//            if((running_step[i].status == step_running_status) && (CV_mode[i] == 0) && (DCV_mode[i] == 0)) {
//                if((running_step[i].type == CV) || (running_step[i].type == CC_to_CV)) {
//                    if((fabs(record_data.channel_record_data[i].output_current-step_work[i].step_work_current) >= 0.02) && (fabs(record_data.channel_record_data[i].battery_voltage-step_work[i].step_work_voltage) <= 0.1) ) {
//                        CV_mode[i] = 1;
//                    }
//                } else if((running_step[i].type == DCV) || (running_step[i].type == DCC_to_DCV)) {
//                    if((fabs(record_data.channel_record_data[i].output_current-step_work[i].step_work_current) >= 0.02) && (fabs(record_data.channel_record_data[i].battery_voltage-step_work[i].step_work_voltage) <= 0.1) ) {
//                        DCV_mode[i] = 1;
//                    }
//                }

//            }
//        }

//    }

//    //----------------------------------------------------------------------------------------------

//    //�����ж��Լ��ϱ�-------------------------------------------------------------------------------
//    Protection_Judgment();
//    for(int i = 0; i < CHANNELS; i++) {
//        if(STEP_PROT_FLAG[i] == 0x01) {
//            // HAL_Delay(10);
//            Step_Protection_Report(0x01<<i);
//            HAL_Delay(50);
//            Record_Report(0x01 << i);

//            led_run_setting(0x01<<i, led_error, &(led_control));
//            led_run_setting(0, led_error, &(led_control));

//            STEP_PROT_FLAG[i] = 0x02;
//        }

//        if(GLOBAL_PROT_FLAG[i] == 0x01) {
//            // HAL_Delay(10);
//            Global_Protection_Report(0x01<<i);
//            HAL_Delay(50);
//            Record_Report(0x01 << i);

//            led_run_setting(0x01<<i, led_error, &(led_control));
//            led_run_setting(0, led_error, &(led_control));

//            GLOBAL_PROT_FLAG[i] = 0x02;
//        }
//    }

//    //----------------------------------------------------------------------------------------------

//    if(SYS_MODE == SYS_NORMAL_MODE) {
//        // //�жϼ�¼�����ϴ������������������ϴ���¼����--------------------------------------------------
//        // for(int i = 0; i < CHANNELS; i++) {
//        //     if(running_step[i].status == step_running_status) {
//        //         this_record_condition[i].record_current = record_data.channel_record_data[i].output_current;
//        //         this_record_condition[i].record_voltage = record_data.channel_record_data[i].battery_voltage;
//        //         this_record_condition[i].record_time    = record_data.channel_record_data[i].run_time / 1000.0;

//        //         if( ((step_record_condition[i].record_current != 0) && (fabs(this_record_condition[i].record_current - last_record_condition[i].record_current) >= step_record_condition[i].record_current)) ||
//        //             ((step_record_condition[i].record_voltage != 0) && (fabs(this_record_condition[i].record_voltage - last_record_condition[i].record_voltage) >= step_record_condition[i].record_voltage)) ||
//        //             ((step_record_condition[i].record_time != 0) && ((this_record_condition[i].record_time - last_record_condition[i].record_time) >= step_record_condition[i].record_time)) ) {

//        //             Record_Report(0x01 << i);

//        //             last_record_condition[i].record_current = this_record_condition[i].record_current;
//        //             last_record_condition[i].record_voltage = this_record_condition[i].record_voltage;
//        //             last_record_condition[i].record_time = this_record_condition[i].record_time;
//        //         }
//        //     }
//        // }
//        // //------------------------------------------------------------------------------------------

//        //�жϹ������н�ֹ�����������������л�����------------------------------------------------------
//        Running_Switch();
//        //------------------------------------------------------------------------------------------
//    }




//// #if CHANNELS_1

//// #endif
//// #if CHANNELS_2

//// #endif
//// #if CHANNELS_4

//// #endif
//// #if CHANNELS_8

//// #endif

}
uint16_t led_control_G=0x5555;
void Task_500ms_Callback(void)//LED����˸
{
    LED81 = ~LED81;
    LED0_G = ~LED0_G;
    led_control = (~led_control)|led_control_G;
    pca9555SetOutput(led_control);

//    for(int i = 0; i < CHANNELS; i++) {
//        if(running_step[i].status == step_running_status) {
//            if( (running_step[i].type == DCC) || \
//                (running_step[i].type == DCV) || \
//                (running_step[i].type == DCC_to_DCV) || \
//                (running_step[i].type == DCP)) {
//                led_run_setting(0x01<<i, led_discharge, &(led_control));
//            }
//        }
//    }
}

void Task_1000ms_Callback(void)
{
//    for(int i = 0; i < CHANNELS; i++) {
//        if(running_step[i].status == step_running_status) {
//            if( (running_step[i].type == CC) || \
//                (running_step[i].type == CV) || \
//                (running_step[i].type == CC_to_CV) || \
//                (running_step[i].type == CP)) {
//                led_run_setting(0x01<<i, led_charge, &(led_control));
//            }
//        }
//    }
//		
//		ACDC_ONOFF_CMD(0,0);
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



// USER CODE END
