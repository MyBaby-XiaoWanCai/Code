/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Ctr_LED.c
// Created on: 2020-4-4
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义
Uint16 Cnt1_LED_Red=0;
Uint16 Cnt2_LED_Red=0;
Uint16 Cnt1_LED_Green=0;

//函数声明


//函数定义
//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Ctr_LED_Work(void)
{
        if(DCDC_Service.Fault_Module!=0)
        {
            //模块硬件故障，前导为红色LED亮2S灭2S，然后0.5S亮灭1次表示bit0为1，0.5S亮灭2次表示bit1为1
            Cnt1_LED_Green=0;
            LED_GREEN_OFF;
            if(Cnt1_LED_Red<124)
            {
                if(Cnt1_LED_Red==0)
                {LED_RED_ON;}
                if(Cnt1_LED_Red==50)
                {LED_RED_OFF;}

                if(Cnt1_LED_Red==100)
                {LED_RED_ON;}
                if(Cnt1_LED_Red==112)
                {LED_RED_OFF;}
                ++Cnt1_LED_Red;
            }
            else
            {
                if((DCDC_Service.Fault_Module&(0x00000001<<Cnt2_LED_Red))==0)
                {
                    if(Cnt1_LED_Red==124)
                    {LED_RED_ON;}
                    if(Cnt1_LED_Red==136)
                    {LED_RED_OFF;}

                    if(++Cnt1_LED_Red>148)
                    {
                        Cnt1_LED_Red=124;
                        if(++Cnt2_LED_Red>31)
                        {
                            Cnt1_LED_Red=0;
                            Cnt2_LED_Red=0;
                        }
                    }
                }
                else
                {
                    Cnt1_LED_Red=0;
                    Cnt2_LED_Red=0;
                }
            }
        }
        else if(Fault_Code.Err_Global!=0)
        {
            //化成全局保护，前导为红色LED亮4S灭4S，然后0.5S亮灭1次表示bit0为1，0.5S亮灭2次表示bit1为1
            Cnt1_LED_Green=0;
            LED_GREEN_OFF;
            if(Cnt1_LED_Red<224)
            {
                if(Cnt1_LED_Red==0)
                {LED_RED_ON;}
                if(Cnt1_LED_Red==100)
                {LED_RED_OFF;}

                if(Cnt1_LED_Red==200)
                {LED_RED_ON;}
                if(Cnt1_LED_Red==212)
                {LED_RED_OFF;}
                ++Cnt1_LED_Red;
            }
            else
            {
                if((Fault_Code.Err_Global&(0x00000001<<Cnt2_LED_Red))==0)
                {
                    if(Cnt1_LED_Red==224)
                    {LED_RED_ON;}
                    if(Cnt1_LED_Red==236)
                    {LED_RED_OFF;}

                    if(++Cnt1_LED_Red>248)
                    {
                        Cnt1_LED_Red=224;
                        if(++Cnt2_LED_Red>31)
                        {
                            Cnt1_LED_Red=0;
                            Cnt2_LED_Red=0;
                        }
                    }
                }
                else
                {
                    Cnt1_LED_Red=0;
                    Cnt2_LED_Red=0;
                }
            }
        }
        else if(Fault_Code.Err_Step!=0)
        {
            //化成全局保护，前导为红色LED亮8S灭8S，然后0.5S亮灭1次表示bit0为1，0.5S亮灭2次表示bit1为1
            Cnt1_LED_Green=0;
            LED_GREEN_OFF;
            if(Cnt1_LED_Red<824)
            {
                if(Cnt1_LED_Red==0)
                {LED_RED_ON;}
                if(Cnt1_LED_Red==400)
                {LED_RED_OFF;}

                if(Cnt1_LED_Red==800)
                {LED_RED_ON;}
                if(Cnt1_LED_Red==812)
                {LED_RED_OFF;}
                ++Cnt1_LED_Red;
            }
            else
            {
                if((Fault_Code.Err_Step&(0x00000001<<Cnt2_LED_Red))==0)
                {
                    if(Cnt1_LED_Red==824)
                    {LED_RED_ON;}
                    if(Cnt1_LED_Red==836)
                    {LED_RED_OFF;}

                    if(++Cnt1_LED_Red>848)
                    {
                        Cnt1_LED_Red=824;
                        if(++Cnt2_LED_Red>31)
                        {
                            Cnt1_LED_Red=0;
                            Cnt2_LED_Red=0;
                        }
                    }
                }
                else
                {
                    Cnt1_LED_Red=0;
                    Cnt2_LED_Red=0;
                }
            }
        }
        else if(Fault_Code.Alarm!=0)
        {
            //告警，绿色LED、红色LED亮0.2S熄灭0.2S
            if(++Cnt1_LED_Green>=10)
            {
                Cnt1_LED_Green=0;
            }

            if(Cnt1_LED_Green==0)
            {
                LED_GREEN_ON;
                LED_RED_ON;
            }

            if(Cnt1_LED_Green==5)
            {
                LED_GREEN_OFF;
                LED_RED_OFF;
            }
        }
    else if(DCDC_Service.Now_Mode==DCDC_STANDBY)
    {
        Cnt1_LED_Red=0;
        Cnt2_LED_Red=0;
        if(DCDC_Service.Mode_Parall==PARALL_SLAVER)
        {
            //从机在待机状态，绿色LED亮1.8S熄灭0.2S
            if(++Cnt1_LED_Green>=50)
            {Cnt1_LED_Green=0;}
            if(Cnt1_LED_Green==0)
            {LED_GREEN_ON;}
            if(Cnt1_LED_Green==45)
            {LED_GREEN_OFF;}
        }
        else
        {
            //主机模式或单机模式在待机状态，绿色LED常亮
            LED_GREEN_ON;
        }
        //红色LED熄灭
        LED_RED_OFF;
    }
    else if((DCDC_Service.Now_Mode==DCDC_CC)||(DCDC_Service.Now_Mode==DCDC_CCtoCV)||(DCDC_Service.Now_Mode==DCDC_CV))
    {
        //充电模式，绿色LED亮0.5S熄灭0.5S
        if(++Cnt1_LED_Green>=12)
        {
            Cnt1_LED_Green=0;
            LED_GREEN_TOGGLE;
        }
        //红色LED熄灭
        LED_RED_OFF;
    }
    else if((DCDC_Service.Now_Mode==DCDC_DCC)||(DCDC_Service.Now_Mode==DCDC_DCCtoDCV)||(DCDC_Service.Now_Mode==DCDC_DCV))
    {
        //放电模式，绿色LED亮0.2S熄灭0.2S
        if(++Cnt1_LED_Green>=5)
        {
            Cnt1_LED_Green=0;
            LED_GREEN_TOGGLE;
        }
        //红色LED熄灭
        LED_RED_OFF;
    }
    else if(DCDC_Service.Now_Mode==DCDC_STATIC)
    {
        //静置状态，绿色LED亮0.2熄灭1.8S
        if(++Cnt1_LED_Green>=50)
        {Cnt1_LED_Green=0;}
        if(Cnt1_LED_Green==0)
        {LED_GREEN_ON;}
        if(Cnt1_LED_Green==5)
        {LED_GREEN_OFF;}
        //红色LED熄灭
        LED_RED_OFF;
    }
    else if(DCDC_Service.Now_Mode==DCDC_DEBUG)
    {
        //调试模式，绿色LED、红色LED亮0.5S熄灭0.5S
        if(++Cnt1_LED_Green>=24)
        {
            Cnt1_LED_Green=0;
        }

        if(Cnt1_LED_Green==0)
        {
            LED_GREEN_ON;
            LED_RED_ON;
        }

        if(Cnt1_LED_Green==12)
        {
            LED_GREEN_OFF;
            LED_RED_OFF;
        }
    }
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Ctr_LED_Aging(void)
{
    if(Auto_Aging.Step_Aging==STEP_AGING_READY)
    {
        if((W5500_Service.SIPR[3]==100)||(W5500_Service.SIPR[3]==102)||(W5500_Service.SIPR[3]==104)||(W5500_Service.SIPR[3]==106)||(W5500_Service.SIPR[3]==108))
        {
            //老化流程准备好，绿色LED、红色LED亮0.5S熄灭0.5S
            if(++Cnt1_LED_Green>=24)
            {
                Cnt1_LED_Green=0;
            }

            if(Cnt1_LED_Green==0)
            {
                LED_GREEN_ON;
                LED_RED_ON;
            }

            if(Cnt1_LED_Green==12)
            {
                LED_GREEN_OFF;
                LED_RED_OFF;
            }
        }
        else
        {
            //老化流程准备好，绿色LED、红色LED亮1S熄灭1S
            if(++Cnt1_LED_Green>=48)
            {
                Cnt1_LED_Green=0;
            }

            if(Cnt1_LED_Green==0)
            {
                LED_GREEN_ON;
                LED_RED_ON;
            }

            if(Cnt1_LED_Green==24)
            {
                LED_GREEN_OFF;
                LED_RED_OFF;
            }
        }
    }
    else if(Auto_Aging.Step_Aging==STEP_AGING_OK)
    {
        LED_RED_OFF;
        //老化流程正常运行完，红色LED熄灭，绿色LED亮4S熄灭4S
        if(++Cnt1_LED_Green>=200)
        {
            Cnt1_LED_Green=0;
        }

        if(Cnt1_LED_Green==0)
        {
            LED_GREEN_ON;
        }

        if(Cnt1_LED_Green==100)
        {
            LED_GREEN_OFF;
        }
    }
    else if((Auto_Aging.Step_Aging==STEP_AGING_ERR_SOURCE)||((Auto_Aging.Step_Aging==STEP_AGING_RUN)&&(Auto_Aging.State==6)))
    {
        //老化流程由于陪测模块故障未走完，绿色LED和红色LED交替亮0.5S熄灭0.5S
        if(++Cnt1_LED_Green>=24)
        {
            Cnt1_LED_Green=0;
        }

        if(Cnt1_LED_Green==0)
        {
            LED_GREEN_ON;
            LED_RED_OFF;
        }

        if(Cnt1_LED_Green==12)
        {
            LED_GREEN_OFF;
            LED_RED_ON;
        }
    }
    else
    {
        Ctr_LED_Work();
    }
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Ctr_LED(void)
{
    if((W5500_Service.SIPR[3]==100)||(W5500_Service.SIPR[3]==102)||(W5500_Service.SIPR[3]==104)||(W5500_Service.SIPR[3]==106)||(W5500_Service.SIPR[3]==108)||\
       (W5500_Service.SIPR[3]==101)||(W5500_Service.SIPR[3]==103)||(W5500_Service.SIPR[3]==105)||(W5500_Service.SIPR[3]==107)||(W5500_Service.SIPR[3]==109))
    {
        Ctr_LED_Aging();
    }
    else
    {
        Ctr_LED_Work();
    }
}

//File end
