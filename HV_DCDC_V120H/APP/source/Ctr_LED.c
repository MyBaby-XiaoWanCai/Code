/*
 * Ctr_LED.c
 *
 *  Created on: 2022年1月6日
 *      Author: JC
 */
#include "F28x_Project.h"

//全局变量定义
Uint16 Cnt1_LED_Red[4];

Uint16 Cnt1_LED_Green[4];
Uint16 Cnt2_LED_Green=0;

//LED指示
void Ctr_LED_Work(void)
{
    Uint16 i;



    LED_RUN_RedOFF;
    if(++Cnt2_LED_Green>=500)
    {
         Cnt2_LED_Green=0;
    }

    if(Cnt2_LED_Green==0)
    {
         LED_RUN_GreenON;
    }
    if(Cnt2_LED_Green==250)
    {
         LED_RUN_GreenOFF;
    }



    for(i=0;i<4;i++)
    {

      //  if((DCDC_Service[i].Fault_Module==0)&&(Fault_Code[i].Err_Global==0))
        if(Fault_Code[i].Err_Global!=0)
        {
             if(i==0)
             {
                 LED_CH1_RedON;
                 LED_CH1_GreenOFF;
             }
             else if(i==1)
             {
                 LED_CH2_RedON;
                 LED_CH2_GreenOFF;
             }
             else if(i==2)
             {
                 LED_CH3_RedON;
                 LED_CH3_GreenOFF;
             }
             else if(i==3)
             {
                 LED_CH4_RedON;
                 LED_CH4_GreenOFF;
             }
        }
        else if(Fault_Code[i].Err_Global!=0)
        {

            if(++Cnt1_LED_Red[i]>=200)
            {
                Cnt1_LED_Red[i]=0;
            }

            if(Cnt1_LED_Red[i]==0)
            {

               if(i==0)
               {
                   LED_CH1_RedON;
                   LED_CH1_GreenOFF;
               }
               else if(i==1)
               {
                   LED_CH2_RedON;
                   LED_CH2_GreenOFF;
               }
               else if(i==2)
               {
                   LED_CH3_RedON;
                   LED_CH3_GreenOFF;
               }
               else if(i==3)
               {
                   LED_CH4_RedON;
                   LED_CH4_GreenOFF;
               }
            }
            if(Cnt1_LED_Red[i]==100)
            {
                  if(i==0)
                  {
                      LED_CH1_RedOFF;
                  }
                  else if(i==1)
                  {
                      LED_CH2_RedOFF;
                  }
                  else if(i==2)
                  {
                      LED_CH3_RedOFF;
                  }
                  else if(i==3)
                  {
                      LED_CH4_RedOFF;
                  }
            }

        }
        else if(DCDC_Service[i].Now_Mode==DCDC_STANDBY)
        {
              if(i==0)
              {
                  LED_CH1_RedOFF;
                  LED_CH1_GreenOFF;
              }
              else if(i==1)
              {
                  LED_CH2_RedOFF;
                  LED_CH2_GreenOFF;
              }
              else if(i==2)
              {
                  LED_CH3_RedOFF;
                  LED_CH3_GreenOFF;
              }
              else if(i==3)
              {
                  LED_CH4_RedOFF;
                  LED_CH4_GreenOFF;
              }
         }
        else if((DCDC_Service[i].Now_Mode==DCDC_CC)||(DCDC_Service[i].Now_Mode==DCDC_CCtoCV)||(DCDC_Service[i].Now_Mode==DCDC_CV))
        {
              if(++Cnt1_LED_Green[i]>=10)
              {
                  Cnt1_LED_Green[i]=0;
              }

              if(Cnt1_LED_Green[i]==0)
              {

                 if(i==0)
                 {
                     LED_CH1_RedOFF;
                     LED_CH1_GreenON;
                 }
                 else if(i==1)
                 {
                     LED_CH2_RedOFF;
                     LED_CH2_GreenON;
                 }
                 else if(i==2)
                 {
                     LED_CH3_RedOFF;
                     LED_CH3_GreenON;
                 }
                 else if(i==3)
                 {
                     LED_CH4_RedOFF;
                     LED_CH4_GreenON;
                 }
              }
              if(Cnt1_LED_Green[i]==5)
              {
                    if(i==0)
                    {
                        LED_CH1_GreenOFF;
                    }
                    else if(i==1)
                    {
                        LED_CH2_GreenOFF;
                    }
                    else if(i==2)
                    {
                        LED_CH3_GreenOFF;
                    }
                    else if(i==3)
                    {
                        LED_CH4_GreenOFF;
                    }
              }
        }
        else if((DCDC_Service[i].Now_Mode==DCDC_DCC)||(DCDC_Service[i].Now_Mode==DCDC_DCCtoDCV)||(DCDC_Service[i].Now_Mode==DCDC_DCV))
        {
                if(++Cnt1_LED_Green[i]>=10)
                {
                    Cnt1_LED_Green[i]=0;
                }

                if(Cnt1_LED_Green[i]==0)
                {

                   if(i==0)
                   {
                       LED_CH1_RedOFF;
                       LED_CH1_GreenON;
                   }
                   else if(i==1)
                   {
                       LED_CH2_RedOFF;
                       LED_CH2_GreenON;
                   }
                   else if(i==2)
                   {
                       LED_CH3_RedOFF;
                       LED_CH3_GreenON;
                   }
                   else if(i==3)
                   {
                       LED_CH4_RedOFF;
                       LED_CH4_GreenON;
                   }
                }
                if(Cnt1_LED_Green[i]==5)
                {
                      if(i==0)
                      {
                          LED_CH1_GreenOFF;
                      }
                      else if(i==1)
                      {
                          LED_CH2_GreenOFF;
                      }
                      else if(i==2)
                      {
                          LED_CH3_GreenOFF;
                      }
                      else if(i==3)
                      {
                          LED_CH4_GreenOFF;
                      }
                }
        }



    }

}
