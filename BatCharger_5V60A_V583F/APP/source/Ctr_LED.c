/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Ctr_LED.c
// Created on: 2020-4-4
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//ȫ�ֱ�������
Uint16 Cnt1_LED_Red=0;
Uint16 Cnt2_LED_Red=0;
Uint16 Cnt1_LED_Green=0;

//��������


//��������
//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Ctr_LED_Work(void)
{
        if(DCDC_Service.Fault_Module!=0)
        {
            //ģ��Ӳ�����ϣ�ǰ��Ϊ��ɫLED��2S��2S��Ȼ��0.5S����1�α�ʾbit0Ϊ1��0.5S����2�α�ʾbit1Ϊ1
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
            //����ȫ�ֱ�����ǰ��Ϊ��ɫLED��4S��4S��Ȼ��0.5S����1�α�ʾbit0Ϊ1��0.5S����2�α�ʾbit1Ϊ1
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
            //����ȫ�ֱ�����ǰ��Ϊ��ɫLED��8S��8S��Ȼ��0.5S����1�α�ʾbit0Ϊ1��0.5S����2�α�ʾbit1Ϊ1
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
            //�澯����ɫLED����ɫLED��0.2SϨ��0.2S
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
            //�ӻ��ڴ���״̬����ɫLED��1.8SϨ��0.2S
            if(++Cnt1_LED_Green>=50)
            {Cnt1_LED_Green=0;}
            if(Cnt1_LED_Green==0)
            {LED_GREEN_ON;}
            if(Cnt1_LED_Green==45)
            {LED_GREEN_OFF;}
        }
        else
        {
            //����ģʽ�򵥻�ģʽ�ڴ���״̬����ɫLED����
            LED_GREEN_ON;
        }
        //��ɫLEDϨ��
        LED_RED_OFF;
    }
    else if((DCDC_Service.Now_Mode==DCDC_CC)||(DCDC_Service.Now_Mode==DCDC_CCtoCV)||(DCDC_Service.Now_Mode==DCDC_CV))
    {
        //���ģʽ����ɫLED��0.5SϨ��0.5S
        if(++Cnt1_LED_Green>=12)
        {
            Cnt1_LED_Green=0;
            LED_GREEN_TOGGLE;
        }
        //��ɫLEDϨ��
        LED_RED_OFF;
    }
    else if((DCDC_Service.Now_Mode==DCDC_DCC)||(DCDC_Service.Now_Mode==DCDC_DCCtoDCV)||(DCDC_Service.Now_Mode==DCDC_DCV))
    {
        //�ŵ�ģʽ����ɫLED��0.2SϨ��0.2S
        if(++Cnt1_LED_Green>=5)
        {
            Cnt1_LED_Green=0;
            LED_GREEN_TOGGLE;
        }
        //��ɫLEDϨ��
        LED_RED_OFF;
    }
    else if(DCDC_Service.Now_Mode==DCDC_STATIC)
    {
        //����״̬����ɫLED��0.2Ϩ��1.8S
        if(++Cnt1_LED_Green>=50)
        {Cnt1_LED_Green=0;}
        if(Cnt1_LED_Green==0)
        {LED_GREEN_ON;}
        if(Cnt1_LED_Green==5)
        {LED_GREEN_OFF;}
        //��ɫLEDϨ��
        LED_RED_OFF;
    }
    else if(DCDC_Service.Now_Mode==DCDC_DEBUG)
    {
        //����ģʽ����ɫLED����ɫLED��0.5SϨ��0.5S
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
            //�ϻ�����׼���ã���ɫLED����ɫLED��0.5SϨ��0.5S
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
            //�ϻ�����׼���ã���ɫLED����ɫLED��1SϨ��1S
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
        //�ϻ��������������꣬��ɫLEDϨ����ɫLED��4SϨ��4S
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
        //�ϻ������������ģ�����δ���꣬��ɫLED�ͺ�ɫLED������0.5SϨ��0.5S
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
