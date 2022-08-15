/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Ctr_Fan.c
// Created on: 2020-4-19
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义


//函数声明


//函数定义
//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Fan_Fault_Check(void)
{
    static Uint16 Cnt_Fan_Fault_Check=0;
    if(IS_FAULT_FAN)
    {
        if(++Cnt_Fan_Fault_Check>=25)
        {
            Cnt_Fan_Fault_Check=0;
            DINT;
            DCDC_Service.Fault_Module	|= FAULT_FAN;
            EINT;
        }
    }
    else
    {
        Cnt_Fan_Fault_Check=0;
    }
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Sink_OverHeat_Check(void)
{
    static Uint16 Cnt_Sink_OverHeat_Check=0;
    if((Data_Sum.Temp_NTC1>=850)||(Data_Sum.Temp_NTC2>=850))//85度
    {
        if(++Cnt_Sink_OverHeat_Check>=25)
        {
            Cnt_Sink_OverHeat_Check=0;
            DINT;
            DCDC_Service.Fault_Module	|= FAULT_OT_SINK;
            EINT;
        }
    }
    else
    {
        Cnt_Sink_OverHeat_Check=0;
    }
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Ctr_Fan(void)
{
	static Uint16 Cnt=0;
	if((Data_Sum.Cur_CHA>20000)||(Data_Sum.Cur_CHB>20000))
	{
		Cnt=0;
		EPwm3Regs.CMPA.half.CMPA = 0;
	}
	else
	{
		if(++Cnt>4500)
		{
			Cnt=4500;
			EPwm3Regs.CMPA.half.CMPA = 3000;
		}
	}
}
