#ifndef __PROTECTION_JUDGMENT_H__
#define __PROTECTION_JUDGMENT_H__

#include "main.h"
#include "step_work.h"
#include "data_analysis.h"
#include "discharge.h"

//全局保护使能宏定义==================================================================================
        //charge_OverVoltage                    //充电过压保护
        //charge_OverCurrent                    //充电过流保护
        //charge_OverCapacitance                //充电过容保护
        //discharge_UnderVoltage                //放电欠压保护
        //discharge_OverCurrent                 //放电过流保护
        //discharge_OverCapacitance             //放电过容保护
        //output_CurrentLimit                   //输出直流限流保护
        //battery_OverTemperature_warning       //电池过温预警保护
        //battery_OverTemperature_alarm         //电池过温报警保护
        //reverse_battery                       //电池反接保护
        //contact_impedance                     //接触阻抗保护
        //fluctuation_BatteryVoltage            //电池电压波动保护
        //fluctuation_OutputCurrent             //输出电流波动保护
        //output_OverVoltage                    //模块端口输出过压保护
        //loop_impedance                        //回路阻抗保护
        //hardware_fault                        //硬件保护
        //output_ShortCircuit                   //输出端短路故障
        //battery_ShortCircuit                  //电池短路故障
        //fan_fault                             //风扇故障
        //BUS_low_fault                         //输入 BUS 低故障
        //BUS_high_fault                        //输入 BUS 高故障
        //AC_dump                               //AC掉电故障保护
        //module_OverTemperature_alarm          //模块过温报警保护
        //calibration_data_error                //校准数据错误报警
        //step_data_error                       //工步数据错误报警
        //other_fault                           //其他故障报警

//全局保护-standby模式使能宏定义----------------------------------------------------------------------
#define GLOBAL_STANDBY_charge_OverVoltage                   1
#define GLOBAL_STANDBY_charge_OverCurrent                   0
#define GLOBAL_STANDBY_charge_OverCapacitance               0
#define GLOBAL_STANDBY_discharge_UnderVoltage               1
#define GLOBAL_STANDBY_discharge_OverCurrent                0
#define GLOBAL_STANDBY_discharge_OverCapacitance            0
// #define GLOBAL_STANDBY_output_CurrentLimit                  1
#define GLOBAL_STANDBY_battery_UnderTemperature_alarm       0
#define GLOBAL_STANDBY_battery_OverTemperature_warning      0
#define GLOBAL_STANDBY_battery_OverTemperature_alarm        0
#define GLOBAL_STANDBY_reverse_battery                      1
#define GLOBAL_STANDBY_contact_impedance                    0
#define GLOBAL_STANDBY_fluctuation_BatteryVoltage           1
#define GLOBAL_STANDBY_fluctuation_OutputCurrent            0
#define GLOBAL_STANDBY_output_OverVoltage                   0
#define GLOBAL_STANDBY_loop_impedance                       0
#define GLOBAL_STANDBY_hardware_fault                       1
#define GLOBAL_STANDBY_output_ShortCircuit                  1
#define GLOBAL_STANDBY_battery_ShortCircuit                 1
#define GLOBAL_STANDBY_fan_fault                            1
#define GLOBAL_STANDBY_BUS_low_fault                        0
#define GLOBAL_STANDBY_BUS_high_fault                       0
#define GLOBAL_STANDBY_AC_dump                              1
#define GLOBAL_STANDBY_module_OverTemperature_alarm         1
#define GLOBAL_STANDBY_calibration_data_error               1
#define GLOBAL_STANDBY_step_data_error                      1
#define GLOBAL_STANDBY_other_fault                          1
//--------------------------------------------------------------------------------------------------

//全局保护-CC模式使能宏定义---------------------------------------------------------------------------
#define GLOBAL_CC_charge_OverVoltage                   1
#define GLOBAL_CC_charge_OverCurrent                   1
#define GLOBAL_CC_charge_OverCapacitance               1
#define GLOBAL_CC_discharge_UnderVoltage               1
#define GLOBAL_CC_discharge_OverCurrent                0
#define GLOBAL_CC_discharge_OverCapacitance            0
// #define GLOBAL_CC_output_CurrentLimit                  1
#define GLOBAL_CC_battery_UnderTemperature_alarm       0
#define GLOBAL_CC_battery_OverTemperature_warning      0
#define GLOBAL_CC_battery_OverTemperature_alarm        0
#define GLOBAL_CC_reverse_battery                      1
#define GLOBAL_CC_contact_impedance                    1
#define GLOBAL_CC_fluctuation_BatteryVoltage           1
#define GLOBAL_CC_fluctuation_OutputCurrent            1
#define GLOBAL_CC_output_OverVoltage                   1
#define GLOBAL_CC_loop_impedance                       1
#define GLOBAL_CC_hardware_fault                       1
#define GLOBAL_CC_output_ShortCircuit                  1
#define GLOBAL_CC_battery_ShortCircuit                 1
#define GLOBAL_CC_fan_fault                            1
#define GLOBAL_CC_BUS_low_fault                        0
#define GLOBAL_CC_BUS_high_fault                       0
#define GLOBAL_CC_AC_dump                              1
#define GLOBAL_CC_module_OverTemperature_alarm         1
#define GLOBAL_CC_calibration_data_error               1
#define GLOBAL_CC_step_data_error                      1
#define GLOBAL_CC_other_fault                          1
//--------------------------------------------------------------------------------------------------

//全局保护-CV模式使能宏定义---------------------------------------------------------------------------
#define GLOBAL_CV_charge_OverVoltage                   1
#define GLOBAL_CV_charge_OverCurrent                   1
#define GLOBAL_CV_charge_OverCapacitance               1
#define GLOBAL_CV_discharge_UnderVoltage               1
#define GLOBAL_CV_discharge_OverCurrent                0
#define GLOBAL_CV_discharge_OverCapacitance            0
// #define GLOBAL_CV_output_CurrentLimit                  1
#define GLOBAL_CV_battery_UnderTemperature_alarm       0
#define GLOBAL_CV_battery_OverTemperature_warning      0
#define GLOBAL_CV_battery_OverTemperature_alarm        0
#define GLOBAL_CV_reverse_battery                      1
#define GLOBAL_CV_contact_impedance                    1
#define GLOBAL_CV_fluctuation_BatteryVoltage           1
#define GLOBAL_CV_fluctuation_OutputCurrent            1
#define GLOBAL_CV_output_OverVoltage                   1
#define GLOBAL_CV_loop_impedance                       1
#define GLOBAL_CV_hardware_fault                       1
#define GLOBAL_CV_output_ShortCircuit                  1
#define GLOBAL_CV_battery_ShortCircuit                 1
#define GLOBAL_CV_fan_fault                            1
#define GLOBAL_CV_BUS_low_fault                        0
#define GLOBAL_CV_BUS_high_fault                       0
#define GLOBAL_CV_AC_dump                              1
#define GLOBAL_CV_module_OverTemperature_alarm         1
#define GLOBAL_CV_calibration_data_error               1
#define GLOBAL_CV_step_data_error                      1
#define GLOBAL_CV_other_fault                          1
//--------------------------------------------------------------------------------------------------

//全局保护-CC_to_CV模式使能宏定义---------------------------------------------------------------------
#define GLOBAL_CC_to_CV_charge_OverVoltage                   1
#define GLOBAL_CC_to_CV_charge_OverCurrent                   1
#define GLOBAL_CC_to_CV_charge_OverCapacitance               1
#define GLOBAL_CC_to_CV_discharge_UnderVoltage               1
#define GLOBAL_CC_to_CV_discharge_OverCurrent                0
#define GLOBAL_CC_to_CV_discharge_OverCapacitance            0
// #define GLOBAL_CC_to_CV_output_CurrentLimit                  1
#define GLOBAL_CC_to_CV_battery_UnderTemperature_alarm       0
#define GLOBAL_CC_to_CV_battery_OverTemperature_warning      0
#define GLOBAL_CC_to_CV_battery_OverTemperature_alarm        0
#define GLOBAL_CC_to_CV_reverse_battery                      1
#define GLOBAL_CC_to_CV_contact_impedance                    1
#define GLOBAL_CC_to_CV_fluctuation_BatteryVoltage           1
#define GLOBAL_CC_to_CV_fluctuation_OutputCurrent            1
#define GLOBAL_CC_to_CV_output_OverVoltage                   1
#define GLOBAL_CC_to_CV_loop_impedance                       1
#define GLOBAL_CC_to_CV_hardware_fault                       1
#define GLOBAL_CC_to_CV_output_ShortCircuit                  1
#define GLOBAL_CC_to_CV_battery_ShortCircuit                 1
#define GLOBAL_CC_to_CV_fan_fault                            1
#define GLOBAL_CC_to_CV_BUS_low_fault                        0
#define GLOBAL_CC_to_CV_BUS_high_fault                       0
#define GLOBAL_CC_to_CV_AC_dump                              1
#define GLOBAL_CC_to_CV_module_OverTemperature_alarm         1
#define GLOBAL_CC_to_CV_calibration_data_error               1
#define GLOBAL_CC_to_CV_step_data_error                      1
#define GLOBAL_CC_to_CV_other_fault                          1
//--------------------------------------------------------------------------------------------------

//全局保护-CP模式使能宏定义---------------------------------------------------------------------------
#define GLOBAL_CP_charge_OverVoltage                   0
#define GLOBAL_CP_charge_OverCurrent                   0
#define GLOBAL_CP_charge_OverCapacitance               0
#define GLOBAL_CP_discharge_UnderVoltage               0
#define GLOBAL_CP_discharge_OverCurrent                0
#define GLOBAL_CP_discharge_OverCapacitance            0
// #define GLOBAL_CP_output_CurrentLimit                  0
#define GLOBAL_CP_battery_UnderTemperature_alarm       0
#define GLOBAL_CP_battery_OverTemperature_warning      0
#define GLOBAL_CP_battery_OverTemperature_alarm        0
#define GLOBAL_CP_reverse_battery                      0
#define GLOBAL_CP_contact_impedance                    0
#define GLOBAL_CP_fluctuation_BatteryVoltage           0
#define GLOBAL_CP_fluctuation_OutputCurrent            0
#define GLOBAL_CP_output_OverVoltage                   0
#define GLOBAL_CP_loop_impedance                       0
#define GLOBAL_CP_hardware_fault                       0
#define GLOBAL_CP_output_ShortCircuit                  0
#define GLOBAL_CP_battery_ShortCircuit                 0
#define GLOBAL_CP_fan_fault                            0
#define GLOBAL_CP_BUS_low_fault                        0
#define GLOBAL_CP_BUS_high_fault                       0
#define GLOBAL_CP_AC_dump                              0
#define GLOBAL_CP_module_OverTemperature_alarm         0
#define GLOBAL_CP_calibration_data_error               0
#define GLOBAL_CP_step_data_error                      0
#define GLOBAL_CP_other_fault                          0
//--------------------------------------------------------------------------------------------------

//全局保护-DCC模式使能宏定义---------------------------------------------------------------------------
#define GLOBAL_DCC_charge_OverVoltage                   1
#define GLOBAL_DCC_charge_OverCurrent                   0
#define GLOBAL_DCC_charge_OverCapacitance               0
#define GLOBAL_DCC_discharge_UnderVoltage               1
#define GLOBAL_DCC_discharge_OverCurrent                1
#define GLOBAL_DCC_discharge_OverCapacitance            1
// #define GLOBAL_DCC_output_CurrentLimit                  1
#define GLOBAL_DCC_battery_UnderTemperature_alarm       0
#define GLOBAL_DCC_battery_OverTemperature_warning      0
#define GLOBAL_DCC_battery_OverTemperature_alarm        0
#define GLOBAL_DCC_reverse_battery                      1
#define GLOBAL_DCC_contact_impedance                    1
#define GLOBAL_DCC_fluctuation_BatteryVoltage           1
#define GLOBAL_DCC_fluctuation_OutputCurrent            1
#define GLOBAL_DCC_output_OverVoltage                   1
#define GLOBAL_DCC_loop_impedance                       1
#define GLOBAL_DCC_hardware_fault                       1
#define GLOBAL_DCC_output_ShortCircuit                  1
#define GLOBAL_DCC_battery_ShortCircuit                 1
#define GLOBAL_DCC_fan_fault                            1
#define GLOBAL_DCC_BUS_low_fault                        0
#define GLOBAL_DCC_BUS_high_fault                       0
#define GLOBAL_DCC_AC_dump                              1
#define GLOBAL_DCC_module_OverTemperature_alarm         1
#define GLOBAL_DCC_calibration_data_error               1
#define GLOBAL_DCC_step_data_error                      1
#define GLOBAL_DCC_other_fault                          1
//--------------------------------------------------------------------------------------------------

//全局保护-DCV模式使能宏定义---------------------------------------------------------------------------
#define GLOBAL_DCV_charge_OverVoltage                   1
#define GLOBAL_DCV_charge_OverCurrent                   0
#define GLOBAL_DCV_charge_OverCapacitance               0
#define GLOBAL_DCV_discharge_UnderVoltage               1
#define GLOBAL_DCV_discharge_OverCurrent                1
#define GLOBAL_DCV_discharge_OverCapacitance            1
// #define GLOBAL_DCV_output_CurrentLimit                  1
#define GLOBAL_DCV_battery_UnderTemperature_alarm       0
#define GLOBAL_DCV_battery_OverTemperature_warning      0
#define GLOBAL_DCV_battery_OverTemperature_alarm        0
#define GLOBAL_DCV_reverse_battery                      1
#define GLOBAL_DCV_contact_impedance                    1
#define GLOBAL_DCV_fluctuation_BatteryVoltage           1
#define GLOBAL_DCV_fluctuation_OutputCurrent            1
#define GLOBAL_DCV_output_OverVoltage                   1
#define GLOBAL_DCV_loop_impedance                       1
#define GLOBAL_DCV_hardware_fault                       1
#define GLOBAL_DCV_output_ShortCircuit                  1
#define GLOBAL_DCV_battery_ShortCircuit                 1
#define GLOBAL_DCV_fan_fault                            1
#define GLOBAL_DCV_BUS_low_fault                        0
#define GLOBAL_DCV_BUS_high_fault                       0
#define GLOBAL_DCV_AC_dump                              1
#define GLOBAL_DCV_module_OverTemperature_alarm         1
#define GLOBAL_DCV_calibration_data_error               1
#define GLOBAL_DCV_step_data_error                      1
#define GLOBAL_DCV_other_fault                          1
//--------------------------------------------------------------------------------------------------

//全局保护-DCC_to_DCV模式使能宏定义-------------------------------------------------------------------
#define GLOBAL_DCC_to_DCV_charge_OverVoltage                   1
#define GLOBAL_DCC_to_DCV_charge_OverCurrent                   0
#define GLOBAL_DCC_to_DCV_charge_OverCapacitance               0
#define GLOBAL_DCC_to_DCV_discharge_UnderVoltage               1
#define GLOBAL_DCC_to_DCV_discharge_OverCurrent                1
#define GLOBAL_DCC_to_DCV_discharge_OverCapacitance            1
// #define GLOBAL_DCC_to_DCV_output_CurrentLimit                  1
#define GLOBAL_DCC_to_DCV_battery_UnderTemperature_alarm       0
#define GLOBAL_DCC_to_DCV_battery_OverTemperature_warning      0
#define GLOBAL_DCC_to_DCV_battery_OverTemperature_alarm        0
#define GLOBAL_DCC_to_DCV_reverse_battery                      1
#define GLOBAL_DCC_to_DCV_contact_impedance                    1
#define GLOBAL_DCC_to_DCV_fluctuation_BatteryVoltage           1
#define GLOBAL_DCC_to_DCV_fluctuation_OutputCurrent            1
#define GLOBAL_DCC_to_DCV_output_OverVoltage                   1
#define GLOBAL_DCC_to_DCV_loop_impedance                       1
#define GLOBAL_DCC_to_DCV_hardware_fault                       1
#define GLOBAL_DCC_to_DCV_output_ShortCircuit                  1
#define GLOBAL_DCC_to_DCV_battery_ShortCircuit                 1
#define GLOBAL_DCC_to_DCV_fan_fault                            1
#define GLOBAL_DCC_to_DCV_BUS_low_fault                        0
#define GLOBAL_DCC_to_DCV_BUS_high_fault                       0
#define GLOBAL_DCC_to_DCV_AC_dump                              1
#define GLOBAL_DCC_to_DCV_module_OverTemperature_alarm         1
#define GLOBAL_DCC_to_DCV_calibration_data_error               1
#define GLOBAL_DCC_to_DCV_step_data_error                      1
#define GLOBAL_DCC_to_DCV_other_fault                          1
//--------------------------------------------------------------------------------------------------

//全局保护-DCP模式使能宏定义--------------------------------------------------------------------------
#define GLOBAL_DCP_charge_OverVoltage                   0
#define GLOBAL_DCP_charge_OverCurrent                   0
#define GLOBAL_DCP_charge_OverCapacitance               0
#define GLOBAL_DCP_discharge_UnderVoltage               0
#define GLOBAL_DCP_discharge_OverCurrent                0
#define GLOBAL_DCP_discharge_OverCapacitance            0
// #define GLOBAL_DCP_output_CurrentLimit                  0
#define GLOBAL_DCP_battery_UnderTemperature_alarm       0
#define GLOBAL_DCP_battery_OverTemperature_warning      0
#define GLOBAL_DCP_battery_OverTemperature_alarm        0
#define GLOBAL_DCP_reverse_battery                      0
#define GLOBAL_DCP_contact_impedance                    0
#define GLOBAL_DCP_fluctuation_BatteryVoltage           0
#define GLOBAL_DCP_fluctuation_OutputCurrent            0
#define GLOBAL_DCP_output_OverVoltage                   0
#define GLOBAL_DCP_loop_impedance                       0
#define GLOBAL_DCP_hardware_fault                       0
#define GLOBAL_DCP_output_ShortCircuit                  0
#define GLOBAL_DCP_battery_ShortCircuit                 0
#define GLOBAL_DCP_fan_fault                            0
#define GLOBAL_DCP_BUS_low_fault                        0
#define GLOBAL_DCP_BUS_high_fault                       0
#define GLOBAL_DCP_AC_dump                              0
#define GLOBAL_DCP_module_OverTemperature_alarm         0
#define GLOBAL_DCP_calibration_data_error               0
#define GLOBAL_DCP_step_data_error                      0
#define GLOBAL_DCP_other_fault                          0
//--------------------------------------------------------------------------------------------------

#define GLOBAL_PROTECTION(type, protection) GLOBAL_##type##protection
//==================================================================================================


//单工步保护使能宏定义================================================================================
        //battery_temperature_limit                 //电池温度上限保护
        //charge_capacitance                        //充电容量保护
        //discharge_capacitance                     //放电容量保护
        //voltage_high_limit                        //电压上限保护
        //voltage_low_limit                         //电压下限保护
        //charge_current_limit                      //充电电流上限保护
        //discharge_current_limit                   //放电电流上限保护
        //current_positive_jump                     //电流正跳幅度保护
        //current_negative_jump                     //电流负跳幅度保护
        //voltage_positive_jump                     //电压正跳幅度保护
        //voltage_negative_jump                     //电压负跳幅度保护
        //current_OutOfTolerance                    //电流超差保护
        //voltage_OutOfTolerance                    //电压超差保护

//单工步保护-standby模式-----------------------------------------------------------------------------
#define STEP_STANDBY_battery_temperature_limit          1
#define STEP_STANDBY_charge_capacitance                 0
#define STEP_STANDBY_discharge_capacitance              0
#define STEP_STANDBY_voltage_high_limit                 1
#define STEP_STANDBY_voltage_low_limit                  1
#define STEP_STANDBY_charge_current_limit               0
#define STEP_STANDBY_discharge_current_limit            0
#define STEP_STANDBY_current_positive_jump              0
#define STEP_STANDBY_current_negative_jump              0
#define STEP_STANDBY_voltage_positive_jump              0
#define STEP_STANDBY_voltage_negative_jump              0
#define STEP_STANDBY_current_OutOfTolerance             0
#define STEP_STANDBY_voltage_OutOfTolerance             0
//--------------------------------------------------------------------------------------------------

//单工步保护-CC模式----------------------------------------------------------------------------------
#define STEP_CC_battery_temperature_limit          1
#define STEP_CC_charge_capacitance                 1
#define STEP_CC_discharge_capacitance              0
#define STEP_CC_voltage_high_limit                 1
#define STEP_CC_voltage_low_limit                  0
#define STEP_CC_charge_current_limit               1
#define STEP_CC_discharge_current_limit            0
#define STEP_CC_current_positive_jump              0
#define STEP_CC_current_negative_jump              0
#define STEP_CC_voltage_positive_jump              1
#define STEP_CC_voltage_negative_jump              1
#define STEP_CC_current_OutOfTolerance             1
#define STEP_CC_voltage_OutOfTolerance             0
//--------------------------------------------------------------------------------------------------

//单工步保护-CV模式----------------------------------------------------------------------------------
#define STEP_CV_battery_temperature_limit          1
#define STEP_CV_charge_capacitance                 1
#define STEP_CV_discharge_capacitance              0
#define STEP_CV_voltage_high_limit                 1
#define STEP_CV_voltage_low_limit                  0
#define STEP_CV_charge_current_limit               1
#define STEP_CV_discharge_current_limit            0
#define STEP_CV_current_positive_jump              1
#define STEP_CV_current_negative_jump              1
#define STEP_CV_voltage_positive_jump              1
#define STEP_CV_voltage_negative_jump              1
#define STEP_CV_current_OutOfTolerance             1
#define STEP_CV_voltage_OutOfTolerance             1
//--------------------------------------------------------------------------------------------------

//单工步保护-CC_to_CV模式----------------------------------------------------------------------------
#define STEP_CC_to_CV_battery_temperature_limit          1
#define STEP_CC_to_CV_charge_capacitance                 1
#define STEP_CC_to_CV_discharge_capacitance              0
#define STEP_CC_to_CV_voltage_high_limit                 1
#define STEP_CC_to_CV_voltage_low_limit                  0
#define STEP_CC_to_CV_charge_current_limit               1
#define STEP_CC_to_CV_discharge_current_limit            0
#define STEP_CC_to_CV_current_positive_jump              1
#define STEP_CC_to_CV_current_negative_jump              1
#define STEP_CC_to_CV_voltage_positive_jump              1
#define STEP_CC_to_CV_voltage_negative_jump              1
#define STEP_CC_to_CV_current_OutOfTolerance             1
#define STEP_CC_to_CV_voltage_OutOfTolerance             1
//--------------------------------------------------------------------------------------------------

//单工步保护-CP模式----------------------------------------------------------------------------------
#define STEP_CP_battery_temperature_limit          0
#define STEP_CP_charge_capacitance                 0
#define STEP_CP_discharge_capacitance              0
#define STEP_CP_voltage_high_limit                 0
#define STEP_CP_voltage_low_limit                  0
#define STEP_CP_charge_current_limit               0
#define STEP_CP_discharge_current_limit            0
#define STEP_CP_current_positive_jump              0
#define STEP_CP_current_negative_jump              0
#define STEP_CP_voltage_positive_jump              0
#define STEP_CP_voltage_negative_jump              0
#define STEP_CP_current_OutOfTolerance             0
#define STEP_CP_voltage_OutOfTolerance             0
//--------------------------------------------------------------------------------------------------

//单工步保护-DCC模式---------------------------------------------------------------------------------
#define STEP_DCC_battery_temperature_limit          1
#define STEP_DCC_charge_capacitance                 0
#define STEP_DCC_discharge_capacitance              1
#define STEP_DCC_voltage_high_limit                 0
#define STEP_DCC_voltage_low_limit                  1
#define STEP_DCC_charge_current_limit               0
#define STEP_DCC_discharge_current_limit            1
#define STEP_DCC_current_positive_jump              0
#define STEP_DCC_current_negative_jump              0
#define STEP_DCC_voltage_positive_jump              1
#define STEP_DCC_voltage_negative_jump              1
#define STEP_DCC_current_OutOfTolerance             1
#define STEP_DCC_voltage_OutOfTolerance             0
//--------------------------------------------------------------------------------------------------

//单工步保护-DCV模式---------------------------------------------------------------------------------
#define STEP_DCV_battery_temperature_limit          1
#define STEP_DCV_charge_capacitance                 0
#define STEP_DCV_discharge_capacitance              1
#define STEP_DCV_voltage_high_limit                 0
#define STEP_DCV_voltage_low_limit                  1
#define STEP_DCV_charge_current_limit               0
#define STEP_DCV_discharge_current_limit            1
#define STEP_DCV_current_positive_jump              1
#define STEP_DCV_current_negative_jump              1
#define STEP_DCV_voltage_positive_jump              1
#define STEP_DCV_voltage_negative_jump              1
#define STEP_DCV_current_OutOfTolerance             1
#define STEP_DCV_voltage_OutOfTolerance             1
//--------------------------------------------------------------------------------------------------

//单工步保护-DCC_to_DCV模式--------------------------------------------------------------------------
#define STEP_DCC_to_DCV_battery_temperature_limit          1
#define STEP_DCC_to_DCV_charge_capacitance                 0
#define STEP_DCC_to_DCV_discharge_capacitance              1
#define STEP_DCC_to_DCV_voltage_high_limit                 0
#define STEP_DCC_to_DCV_voltage_low_limit                  1
#define STEP_DCC_to_DCV_charge_current_limit               0
#define STEP_DCC_to_DCV_discharge_current_limit            1
#define STEP_DCC_to_DCV_current_positive_jump              1
#define STEP_DCC_to_DCV_current_negative_jump              1
#define STEP_DCC_to_DCV_voltage_positive_jump              1
#define STEP_DCC_to_DCV_voltage_negative_jump              1
#define STEP_DCC_to_DCV_current_OutOfTolerance             1
#define STEP_DCC_to_DCV_voltage_OutOfTolerance             1
//--------------------------------------------------------------------------------------------------

//单工步保护-DCP模式---------------------------------------------------------------------------------
#define STEP_DCP_battery_temperature_limit          0
#define STEP_DCP_charge_capacitance                 0
#define STEP_DCP_discharge_capacitance              0
#define STEP_DCP_voltage_high_limit                 0
#define STEP_DCP_voltage_low_limit                  0
#define STEP_DCP_charge_current_limit               0
#define STEP_DCP_discharge_current_limit            0
#define STEP_DCP_current_positive_jump              0
#define STEP_DCP_current_negative_jump              0
#define STEP_DCP_voltage_positive_jump              0
#define STEP_DCP_voltage_negative_jump              0
#define STEP_DCP_current_OutOfTolerance             0
#define STEP_DCP_voltage_OutOfTolerance             0
//--------------------------------------------------------------------------------------------------

#define STEP_PROTECTION(type, protection) STEP_##type##protection
//==================================================================================================

#define BUS_LOW_LIMIT   16
#define BUS_HIGH_LIMIT  24

#define MOD_OVER_TEMPERATURE    90

#define FLOAT_NOT_ZERO(x) fabs(x-0.0)

enum protection_flag {
    global_protection_flag = 0x01,
    step_protection_flag,
};

typedef struct fluctuation_protection_struct {
    float voltage;
    float voltage_time;
    float current;
    float current_time;
} FluctuationProtection;

typedef struct jump_protection_struct {
    float current;
    float voltage;
    uint32_t current_positive_jump_number;
    uint32_t current_negative_jump_number;
    uint32_t voltage_positive_jump_number;
    uint32_t voltage_negative_jump_number;
} JumpProtection;

// typedef struct step_protection_enable_struct {
//     uint32_t step_battery_temperature_limit:1;                  //电池温度上限保护
//     uint32_t step_charge_capacitance:1;                         //充电容量保护
//     uint32_t step_discharge_capacitance:1;                      //放电容量保护
//     uint32_t step_voltage_high_limit:1;                         //电压上限保护
//     uint32_t step_voltage_low_limit:1;                          //电压下限保护
//     uint32_t step_charge_current_limit:1;                       //充电电流上限保护
//     uint32_t step_discharge_current_limit:1;                    //放电电流上限保护
//     uint32_t step_current_positive_jump_range:1;                //电流正跳幅度保护
//     uint32_t step_current_positive_jump_number:1;               //电流正跳次数保护
//     uint32_t step_current_negative_jump_range:1;                //电流负跳幅度保护
//     uint32_t step_current_negative_jump_number:1;               //电流负跳次数保护
//     uint32_t step_voltage_positive_jump_range:1;                //电压正跳幅度保护
//     uint32_t step_voltage_positive_jump_number:1;               //电压正跳次数保护
//     uint32_t step_voltage_negative_jump_range:1;                //电压负跳幅度保护
//     uint32_t step_voltage_negative_jump_number:1;               //电压负跳次数保护
//     uint32_t step_current_OutOfTolerance:1;                     //电流超差保护
//     uint32_t step_voltage_OutOfTolerance:1;                     //电压超差保护
//     uint32_t :15;
// } StepProtectionEnable;

extern uint8_t Global_Protection_Flag[CHANNELS][global_protection_trigger_end];
extern uint8_t Step_Protection_Flag[CHANNELS][step_protection_trigger_end];

extern uint8_t GLOBAL_PROT_FLAG[CHANNELS];
extern uint8_t STEP_PROT_FLAG[CHANNELS];

extern uint8_t CV_mode[CHANNELS];
extern uint8_t DCV_mode[CHANNELS];

extern uint8_t fluctuation_voltage_flag[CHANNELS];
extern uint8_t fluctuation_current_flag[CHANNELS];

extern FluctuationProtection fluctuation_protection_old[CHANNELS];
extern FluctuationProtection fluctuation_protection_now[CHANNELS];

extern JumpProtection jump_protection_old[CHANNELS];
extern JumpProtection jump_protection_now[CHANNELS];
extern uint8_t jump_flag[CHANNELS];

extern uint8_t PG_count[CHANNELS];
#define PG_NUMBER 7

void Protection_Action(uint8_t ch, uint8_t flag, uint8_t status);
void Protection_Judgment(void);

#endif
