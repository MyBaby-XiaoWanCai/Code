/*
 * DCDC_BSP.c
 *
 *  Created on: 2021年6月19日
 *      Author: JC
 */

#include "F28x_Project.h"
struct_Channel CH_A[CHANNELS];                    //CHA相关数据
struct_Channel CH_B[CHANNELS];                    //CHB相关数据
INTERFACE_DCDC Interface_DCDC[CHANNELS];          //DCDC模块工作参数请求接口
struct_DCDC_SERVICE DCDC_Service[CHANNELS];       //DCDC状态机相关数据

Uint16 CMPAValue[4],CMPBValue[4];

void InitDCDC_BSP(void)
{
    Uint16 i;
    //DCDC接口初始化
    for(i=0;i<4;i++)
    {
        Interface_DCDC[i].Req_ILSet1_Pu = 0;
        DCDC_Service[i].Now_Mode=2;
        CH_A[i].Set_Cur=0;
        CH_B[i].Set_Cur=0;
    }


}



void DCDC_Service_BSP(void)
{
    Uint16 i;
    for(i=0;i<CHANNELS;i++)
    {
        switch(DCDC_Service[i].Now_Mode)
        {
            case DCDC_STANDBY:
            {
                if(Interface_DCDC[i].Req_ChangeMode==TRUE)
                {
                    Interface_DCDC[i].Req_ChangeMode=FALSE;
                    if(Interface_DCDC[i].Req_Mode==DCDC_DEBUG)
                    {

                        DCDC_Service[i].Now_Mode=DCDC_DEBUG;
                        PWMON_Config(2*i+1);
                        PWMON_Config(2*i+2);

                    }

                }
                break;
            }
            case DCDC_DEBUG:
            {

                (*ePWM[2*i+1]).CMPA.bit.CMPA=CMPAValue[i];
                (*ePWM[2*i+1]).CMPB.bit.CMPB=CMPAValue[i];
                (*ePWM[2*i+2]).CMPA.bit.CMPA=CMPBValue[i];
                (*ePWM[2*i+2]).CMPB.bit.CMPB=CMPBValue[i];

                if(Interface_DCDC[i].Req_ChangeMode==TRUE)
                {
                    DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                }


                break;
            }
            case DCDC_PWM_OFF:
            {
                PWMOFF_Config(2*i+1);
                PWMOFF_Config(2*i+2);
                DCDC_Service[i].Now_Mode=DCDC_STANDBY;
                break;
            }
            default:

                break;
        }

    }

}
