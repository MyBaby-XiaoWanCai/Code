/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_WorkStep.h
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef APP_INCLUDE_NET_WORKSTEP_H_
#define APP_INCLUDE_NET_WORKSTEP_H_



//结构体定义


//联合体定义


//对外部文件开放全局变量声明


//对外部文件开放函数声明
extern void sSetWorkStepsCommand(void);
extern void sSetSetpsCommand(void);
//-------------------父功能类----------------------
#define cSetProtect             0x01
#define cSetProtectACK          0x02
#define cSetProtectInquire      0x03
#define cSetProtectReply        0x04
#define cSetProtectTrigger      0x05
#define cSetRecond              0x06
#define cSetRecondACK           0x07
#define cInquireRecondData      0x08
#define cReplyRecondData        0x09
#define cSetStep                0x0A
#define cSetStepACK             0x0B
#define cSetChargerStatus       0x0C
#define cSetChargerStatusACK    0x0D
#define cSysVersionInquire      0x0E
#define cSysVersionReply        0x0F
#define cSysCabNumInquire       0x10
#define cSysCabAddressReply     0x11
#define cSysTimeSet             0x12
#define cSysTimeSetACK          0x13
#define cFaultClearSet          0x14
#define cFaultClearACK          0x15
#define cSetParallelMode        0x16
#define cSetParallelModeACK     0x17
#define cInquirePowerOffEn      0x18
#define cReplyPowerOffEn        0x19
#define cSetPowerOffMode        0x1A
#define cSetPowerOffModeACK     0x1B
#define cSetFlucMutatProtect    0x1C
#define cetFlucMutatProtectACK  0x1D
#define cInquireFlucMutatData   0x1E
#define cReplyFlucMutatData     0x1F
//------------------------------------
#define cAgingCalibrationSet         0x20
#define cAgingCalibrationSetAck      0x21
#define cAgingCalibrationInqure      0x22
#define cAgingCalibrationInqureAck   0x23
//------------------------------------
#define cSetPreChargeStatus    0x24
#define cSetPreChargeStatusACK 0x25
#define cLifeSingal            0x26
#define cLifeSingalACK         0x27
#define cSSProtectTrigger       0x28
#define cSetGlobalProtectACK    0x2B
#define cFaultReply            0x30
#define cError                  0xff
//-------------------保护类----------------------
#define cSetProtectCV               0x01
#define cSetProtectCC               0x02
#define cSetProtectDCV              0x03
#define cSetProtectDCC              0x04
#define cSetProtectCAH              0x05
#define cSetProtectOPC              0x06
#define cSetProtectT                0x07
#define cSetProtectBatReverse       0x08
#define cSetProtectOPShort          0x0A
#define cSetProtectBatShort         0x0B
#define cSetProtectFanErr           0x0C
#define cSetProtectBusHigh          0x0D
#define cSetProtectBatFluc          0x0E
#define cSetProtectBatMutat         0x0F
#define cSetProtectOPCFluc          0x10
#define cSetProtectOPCMutat         0x11
#define cSetProtectTerminal         0x12
#define cSetProtectLowT             0x13
#define cSetProtectDAH              0x14
#define cSetProtectImpedance        0x15
#define cSetProtectOCE              0x16
#define cSetProtectOVE              0x17
#define cSetProtectLoopImpedance    0x18
#define cSetProtectPowerVolt        0x19
#define cSetProtectSinkHigh         0x20//20191210
#define cSetProtectBusLow           0x21//20191230
//#define Code_cCaliDataError		0x22
//#define Code_cStepDataError		0x23
//#define Code_cHardwareError		0x24
#define cSetProtectTotalAH          0x25
#define cSetContacCheck_Vol_H       0x26
#define cSetContacCheck_Vol_L       0x27
#define cSetContacCheck_Time        0x28
#define cSetContacCheck_Cur         0x29
#define cSetContacCheck_Vol_Rise    0x2A
#define cSetBatTempAlarm            0x2B
#define cSetBatTempOverAlarm        0x2C

#define cSet_Cur_Limited_Set        0xFD

//上报故障码与全局保护设置子功能码分开定义
#define Code_ProtectCV              0x01
#define Code_ProtectCC              0x02
#define Code_ProtectDCV             0x03
#define Code_ProtectDCC             0x04
#define Code_ProtectCAH             0x05

#define Code_ProtectT               0x07
#define Code_ProtectBatReverse      0x08

#define Code_ProtectBatFluc         0x0E
#define Code_ProtectBatMutat        0x0F
#define Code_ProtectOPCFluc         0x10
#define Code_ProtectOPCMutat        0x11
#define Code_ProtectTerminal        0x12

#define Code_ProtectLowT            0x13
#define Code_ProtectDAH             0x14
#define Code_ProtectImpedance       0x15
#define Code_ProtectOCE             0x16
#define Code_ProtectOVE             0x17
#define Code_ProtectLoopImpedance   0x18

#define Code_ProtectTotalAH         0x25
#define Code_BatErrorVol_Start      0x26
#define Code_BatErrorContacCheck    0x27

#define Code_cOtherError		    0xFE
#define Code_cBatTempAlarmFault     0x2B
#define Code_cBatTempOverAlarmFault 0x2C





//内部保护
#define Code_ProtectOPC             0x46 //0x06
#define Code_ProtectOPShort         0x47 //0x0A
#define Code_ProtectBatShort        0x48 //0x0B
#define Code_ProtectFanErr          0x49 //0x0C
#define Code_ProtectBusHigh         0x4A //0x0D
#define Code_ProtectPowerVolt       0x4B //0x19
#define Code_ProtectSinkHigh        0x4C //0x20
#define Code_ProtectBusLow          0x4D //0x21
#define Code_cCaliDataError         0x4E //0x22
#define Code_cStepDataError         0x4F //0x23
#define Code_cHardwareError         0x50 //0x24


//---------------------------------------------------
#define cSSProtectBatTempUpperLimit         0x01
#define cSSProtectCapUpperLimit             0x02
#define cSSProtectCapLowerLimit             0x03
#define cSSProtectBatVoltUpperLimit         0x04
#define cSSProtectBatVoltLowerLimit         0x05
#define cSSProtectCurrUpperLimit            0x06
#define cSSProtectCurrLowerLimit            0x07
#define cSSProtectOPCPositiveJump           0x08
#define cSSProtectOPCNegativeJump           0x09
#define cSSProtectBatVoltPositiveJump       0x0a
#define cSSProtectBatVoltNegativeJump       0x0b



#define cSSProtectBatShort                  0x0C
#define cSSProtectVol_Time_Error            0x0D
#define cSSProtectVol_Change_Time_Error     0x0E

#define cSSProtectVol_Diff                  0x10
#define cSSProtectCur_Diff                  0x11
#define cSSProtectVol_Interval_Pos          0x12
#define cSSProtectVol_Interval_Neg          0x13
#define cSSProtectBatTempDownUpperLimit     0x14
#define Code_StepEnd2Abnormal               0x15 //0x28
#define Code_StepEnd3Abnormal               0x16 //0x29

//-------------------记录条件类----------------------
#define cRecondTime             0x01
#define cRecondVolt             0x02
#define cRecondCurrent          0x03
#define cRecondTimeVolt         0x04
#define cRecondTimeCurrent      0x05
#define cRecondVoltCurrent      0x06
#define cRecondTimeVoltCurrent  0x07
#define cRecondErr              0xff
//-------------------记录数据查询类---------------------
#define cCabinetsNum                0x01
#define cModuleAddress              0x02
#define cRuntime                    0x03
#define cRunMode                    0x04
#define cBatVolt                    0x05
#define cOPCurrent                  0x06
#define cCAH                        0x07
#define cBatTemp                    0x08
#define cSetpNum                    0x09
#define cSetpName                   0x0a
#define cCycleNum                   0x0b
#define cSysTime                    0x0c
#define cStepTotalNum               0x0d
#define cRatio                      0x0f
#define cEnergy                     0x10
#define cOPVolt                     0x11
#define cDataErr                    0xff
//-------------------工步名称设置类----------------------
#define cStepStatic                 0x01
#define cStepCC                     0x02
#define cStepCV                     0x03
#define cStepCCtoCV                 0x04
#define cStepDCC                    0x05
#define cStepDCV                    0x06
#define cStepDR                     0x07
#define cStepDW                     0x08
#define cStepCycleStart             0x09
#define cStepCycleEnd               0x0A
#define cStepDCCtoDCV               0x0b //DCC到DCV模式
#define cStepNegPressure            0x0C
#define cStepErr                    0xff
//-------------------工步设定截止类----------------------
#define cStepTime                   0x01
#define cStepVolt                   0x02
#define cStepCurrent                0x03
#define cStepRes                    0x04
#define cStepPower                  0x05
#define cStepAH                     0x07
#define cStepPressure               0x08
//-------------------工步截止逻辑类----------------------
#define cStepGreater                0x01
#define cStepLess                   0x02
//-------------------工步截止结果类----------------------
#define cStepNext                   0x01
#define cStepJump                   0x02
#define cStepStop                   0x03
//-------------------工步循环设置类----------------------
#define cStepCycleIndex                 0x01
#define cStepCycleTimes                 0x02
//-------------------运行状态设置类----------------------
#define cStopStatus                 0x00
#define cPauseStatus                0x01
#define cWorkStepRunStatus          0x02
#define cFaultStatus                0x03
#define cStepComplete               0x04
#define cSingalStepRunStatus        0x05
#define cStepChangeStatus           0x06
#define cStatusErr                  0xFF
//-------------------数据响应类----------------------
#define cFail           0x00
#define cSuccess        0x01
//-------------------告警清除类型-------------------
#define cFaultCleraSet          0x01
#define cCleraFaultErr      0xff
//-------------------并机模式类型-------------------
#define cMasterMode         0x01
#define cSlaveMode          0x02
#define cSingleMode         0x03
//--------------------断电自运行类型-----------------------------
#define cPoweOffModeCancel      0x00
#define cPoweOffModeRun         0x01
#define cPoweOffModeDisable     0x02
#define cPoweOffModeEn          0x03
//-------------------波动/突变保护类----------------------
#define cSetFlucProtectV            0x01
#define cSetMutatProtectV           0x02
#define cSetFlucProtectI            0x03
#define cSetMutatProtectI           0x04
#define cSetIntervalTime            0x05

#define cPreChargeStatus            0x01
#define cPreChargeCur               0x02
#define cPreChargeTime              0x03
#define cNeedleBedNotPressFit       0x00
#define cNeedleBedPressFit          0x01

#define cEnd            0x0d
//----------------------------------------
#define cNoCycle        0
#define cIndex1         1
#define cIndex2         2
#define cIndex3         3
#define cIndex4         4
#define cIndex5         5
#define cIndex6         6
#define cIndex7         7
#define cIndex8         8
#define cIndex9         9
#define cIndex10        10
#define cIndex11        11
#define cIndex12        12
#define cIndex13            13
#define cIndex14            14
#define cIndex15            15
#define cIndex16            16
#define cIndex17            17
#define cIndex18            18
#define cIndex19            19
#define cIndex20            20
#define cIndex21            21
#define cIndex22            22
#define cIndex23            23
#define cIndex24            24
#define cIndex25            25
#define cIndex26            26
#define cIndex27            27
#define cIndex28            28
#define cIndex29            29
#define cIndex30            30

#endif /* APP_INCLUDE_PROT_NET_GLOBAL_H_ */

//File end
