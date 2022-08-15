/*
 * Protect_Global.c
 *
 *  Created on: 2021年6月21日
 *      Author: JC
 */
#include "F28x_Project.h"

struct_Protect_Global       Protect_Global[CHANNELS];
struct_Fault_Code           Fault_Code[CHANNELS];




//函数定义
//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Value(struct_Protect_Value *Ptr)
{
    if(Ptr->Protect_EN==FALSE)
    {
        Ptr->Cnt_Change_Mask=0;
        Ptr->Cnt_Check=0;
        return;
    }

    if(Ptr->Cnt_Change_Mask<Ptr->Num_Change_Mask)
    {
        Ptr->Cnt_Change_Mask++;
        return;
    }

    if(Ptr->Equation==cStepLess)
    {
        if(Ptr->Now_Value<=Ptr->Ref_Value)
        {
           if(++Ptr->Cnt_Check>=Ptr->Num_Check)
           {
               Ptr->Cnt_Check=0;
               *Ptr->Ptr   |=Ptr->Mask;
           }
        }
        else
        {
            Ptr->Cnt_Check=0;
        }
    }
    else if(Ptr->Equation==cStepGreater)
    {
        if(Ptr->Now_Value>=Ptr->Ref_Value)
        {
           if(++Ptr->Cnt_Check>=Ptr->Num_Check)
           {
               Ptr->Cnt_Check=0;
               *Ptr->Ptr   |=Ptr->Mask;
           }
        }
        else
        {
            Ptr->Cnt_Check=0;
        }
    }
    else
    {
        *Ptr->Ptr   |=Ptr->Mask;
    }
}


void Protect_Check(void)
{
  Uint16 i;
  for(i=0;i<4;i++)
  {
  //  Protect_Global[i].CH_Over_Cur.Now_Value=0;
 //   Protect_Value(&Protect_Global[i].CH_Over_Cur);
  }


}


void Protect_Global_Init(void)
{
    Uint16 i;
      for(i=0;i<4;i++)
      {
          Protect_Global[i].CH_Over_Cur[0].Equation             = cStepGreater;
          Protect_Global[i].CH_Over_Cur[0].Mask                 = cProtectCLFault;
          Protect_Global[i].CH_Over_Cur[0].Num_Change_Mask      = 0;
          Protect_Global[i].CH_Over_Cur[0].Num_Check            = 1;
          Protect_Global[i].CH_Over_Cur[0].Protect_EN           = TRUE;//FALSE;
          Protect_Global[i].CH_Over_Cur[0].Ptr                  = &Fault_Code[i].Err_Global;
          Protect_Global[i].CH_Over_Cur[0].Ref_Value            = 60;

          Protect_Global[i].CH_Over_Cur[1].Equation             = cStepGreater;
          Protect_Global[i].CH_Over_Cur[1].Mask                 = cProtectCLFault;
          Protect_Global[i].CH_Over_Cur[1].Num_Change_Mask      = 0;
          Protect_Global[i].CH_Over_Cur[1].Num_Check            = 1;
          Protect_Global[i].CH_Over_Cur[1].Protect_EN           = TRUE;//FALSE;
          Protect_Global[i].CH_Over_Cur[1].Ptr                  = &Fault_Code[i].Err_Global;
          Protect_Global[i].CH_Over_Cur[1].Ref_Value            = 60;

          Protect_Global[i].CH_Over_Cur[2].Equation             = cStepGreater;
          Protect_Global[i].CH_Over_Cur[2].Mask                 = cProtectCCFault;
          Protect_Global[i].CH_Over_Cur[2].Num_Change_Mask      = 0;
          Protect_Global[i].CH_Over_Cur[2].Num_Check            = 1;
          Protect_Global[i].CH_Over_Cur[2].Protect_EN           = TRUE;//FALSE;
          Protect_Global[i].CH_Over_Cur[2].Ptr                  = &Fault_Code[i].Err_Global;
          Protect_Global[i].CH_Over_Cur[2].Ref_Value            = 10;

          Protect_Global[i].CH_Over_Vol.Equation             = cStepGreater;
          Protect_Global[i].CH_Over_Vol.Mask                 = cProtectCVFault;
          Protect_Global[i].CH_Over_Vol.Num_Change_Mask      = 0;
          Protect_Global[i].CH_Over_Vol.Num_Check            = 1;
          Protect_Global[i].CH_Over_Vol.Protect_EN           = TRUE;//FALSE;
          Protect_Global[i].CH_Over_Vol.Ptr                  = &Fault_Code[i].Err_Global;
          Protect_Global[i].CH_Over_Vol.Ref_Value            = 60;

          Protect_Global[i].Bat_Reverse_Out.Equation         = cStepLess;
          Protect_Global[i].Bat_Reverse_Out.Mask             = cBatPNFault;
          Protect_Global[i].Bat_Reverse_Out.Num_Change_Mask  = 2;
          Protect_Global[i].Bat_Reverse_Out.Num_Check        = 1;
          Protect_Global[i].Bat_Reverse_Out.Protect_EN       = TRUE;
          Protect_Global[i].Bat_Reverse_Out.Ptr              = &Fault_Code[i].Err_Global;
          Protect_Global[i].Bat_Reverse_Out.Ref_Value        = -3;
          Protect_Global[i].Bat_Reverse_Out.Cnt_Check        = 0;
          Protect_Global[i].Bat_Reverse_Out.Cnt_Change_Mask  = 0;

          Protect_Global[i].CH_Over_Vol_Bat.Equation         = cStepGreater;
          Protect_Global[i].CH_Over_Vol_Bat.Mask             = cProtectCVFault;
          Protect_Global[i].CH_Over_Vol_Bat.Num_Change_Mask  = 0;
          Protect_Global[i].CH_Over_Vol_Bat.Num_Check        = 1;
          Protect_Global[i].CH_Over_Vol_Bat.Protect_EN       = TRUE;//FALSE;
          Protect_Global[i].CH_Over_Vol_Bat.Ptr              = &Fault_Code[i].Err_Global;
          Protect_Global[i].CH_Over_Vol_Bat.Ref_Value        = 5;

          Fault_Code[i].Err_Global_Bak=0;
      }

}


void Fault_Code_Global_TX(unsigned char Code,unsigned char Channel)
{
     Fault_Code[Channel].Code=Code;
     sTXCAN_JN910.Data[0]=1<<Channel;
     sTXCAN_JN910.Data[1]=0x83;
     sTXCAN_JN910.Data[2]=Code;
     sTXCAN_JN910.Data[3]=0;
     sTXCAN_JN910.Data[4]=0;
     sTXCAN_JN910.Data[5]=0;
     sTXCAN_JN910.Data[6]=0;
     sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
     if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
     {

     }

}

void Upload_Fault_Code(void)
{
    Uint16 i;
    for(i=0;i<4;i++)
    {
        if(((Fault_Code[i].Err_Global&cProtectCLFault)!=0)&&((Fault_Code[i].Err_Global_Bak&cProtectCLFault)==0))
        {
             Fault_Code[i].Err_Global_Bak |= cProtectCLFault;
             Fault_Code_Global_TX(Code_ProtectCL,i);
        }
        else if(((Fault_Code[i].Err_Global&cProtectCCFault)!=0)&&((Fault_Code[i].Err_Global_Bak&cProtectCCFault)==0))
        {
             Fault_Code[i].Err_Global_Bak |= cProtectCCFault;
             Fault_Code_Global_TX(Code_ProtectCC,i);
        }
        else if(((Fault_Code[i].Err_Global&cProtectCVFault)!=0)&&((Fault_Code[i].Err_Global_Bak&cProtectCVFault)==0))
        {
             Fault_Code[i].Err_Global_Bak |= cProtectCVFault;
             Fault_Code_Global_TX(Code_ProtectCV,i);
        }
        else if(((Fault_Code[i].Err_Global&cBusVoltHighFault)!=0)&&((Fault_Code[i].Err_Global_Bak&cBusVoltHighFault)==0))
        {
             Fault_Code[i].Err_Global_Bak |= cBusVoltHighFault;
             Fault_Code_Global_TX(Code_Protect_BusVoltHigh,i);
        }
        else if(((Fault_Code[i].Err_Global&cBusVoltLowFault)!=0)&&((Fault_Code[i].Err_Global_Bak&cBusVoltLowFault)==0))
        {
              Fault_Code[i].Err_Global_Bak |= cBusVoltLowFault;
              Fault_Code_Global_TX(Code_Protect_BusVoltLow,i);
        }
        else if(((Fault_Code[i].Err_Global&cHardwareError)!=0)&&((Fault_Code[i].Err_Global_Bak&cHardwareError)==0))
        {
              Fault_Code[i].Err_Global_Bak |= cHardwareError;
              Fault_Code_Global_TX(Code_Protect_HardwareError,i);
        }
        else if(((Fault_Code[i].Err_Global&cFanFault)!=0)&&((Fault_Code[i].Err_Global_Bak&cFanFault)==0))
        {
              Fault_Code[i].Err_Global_Bak |= cFanFault;
              Fault_Code_Global_TX(Code_Protect_FanFault,i);
        }
        else if(((Fault_Code[i].Err_Global&cWatchdogFault)!=0)&&((Fault_Code[i].Err_Global_Bak&cWatchdogFault)==0))
        {
              Fault_Code[i].Err_Global_Bak |= cWatchdogFault;
              Fault_Code_Global_TX(Code_Protect_WatchdogFault,i);
        }
        else if(((Fault_Code[i].Err_Global&cBatPNFault)!=0)&&((Fault_Code[i].Err_Global_Bak&cBatPNFault)==0))
        {
              Fault_Code[i].Err_Global_Bak |= cBatPNFault;
              Fault_Code_Global_TX(Code_Protect_BatPNFault,i);
        }
        else if(((Fault_Code[i].Err_Global&cPowerVoltFault)!=0)&&((Fault_Code[i].Err_Global_Bak&cPowerVoltFault)==0))
        {
               Fault_Code[i].Err_Global_Bak |= cPowerVoltFault;
               Fault_Code_Global_TX(Code_Protect_PowerVoltFault,i);
        }

    }

}

void Data_Up(void)
{
    unsigned char CH;
    Uint16 i;
    if(Active_Report.Req_Report==TRUE)
    {
        CH=pack.data[0];

        sTXCAN_JN910.ui32MsgID=0x00000001;
        //上抛数据
        for(i=0;i<4;i++)
        {
            if(CH&(0x01<<i))
            {
                  sTXCAN_JN910.Data[0]=1<<i;
                  sTXCAN_JN910.Data[1]=0x84;
                  sTXCAN_JN910.Data[2]=0x01;
                  sTXCAN_JN910.Data[3]=DCDC_Service[i].Now_Mode;
                  sTXCAN_JN910.Data[4]=Run_Mode_Var[i][0];
                  sTXCAN_JN910.Data[5]=Fault_Code[i].Err_Global&0x000000FF;
                  sTXCAN_JN910.Data[6]=(Fault_Code[i].Err_Global>>8)&0x000000FF;
                  sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                  if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
                  {

                  }
                  //sense 电压
                  sTXCAN_JN910.Data[2]=0x02;
                  Float2Byte( Data_Sum[i].Vol_Sense,sTXCAN_JN910.Data,3);
                  sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                  if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
                  {

                  }
                  //输出电流
                  sTXCAN_JN910.Data[2]=0x03;
                  Float2Byte(Data_Sum[i].Cur_Out,sTXCAN_JN910.Data,3);
                  sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                  if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
                  {

                  }
                  //输出电压
                   sTXCAN_JN910.Data[2]=0x04;
                   Float2Byte(Data_Sum[i].Vol_Out,sTXCAN_JN910.Data,3);
                   sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                   if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
                   {

                   }
                  //BUS电压
                   sTXCAN_JN910.Data[2]=0x05;
                   Float2Byte(Data_Sum[i].Vol_Bus,sTXCAN_JN910.Data,3);
                   sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                   if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
                   {

                   }
                 //第一路电感电流
                  sTXCAN_JN910.Data[2]=0x06;
                  Float2Byte(fclVars[i].CH_A_PI_Cur.Fbk,sTXCAN_JN910.Data,3);
                  sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                  if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
                  {

                  }
                 //第二路电感电流
                  sTXCAN_JN910.Data[2]=0x07;
                  Float2Byte(fclVars[i].CH_B_PI_Cur.Fbk,sTXCAN_JN910.Data,3);
                  sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                  if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
                  {

                  }
                 //温度
                  sTXCAN_JN910.Data[2]=0x08;
                  Float2Byte(fclVars[i].CH_B_PI_Cur.Out,sTXCAN_JN910.Data,3);
                  sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                  if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
                  {

                  }
                  //第一路占空比
                  sTXCAN_JN910.Data[2]=0x09;
                  Float2Byte(DCDC_Service[i].Now_Vol_JR,sTXCAN_JN910.Data,3);
                  sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                  if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
                  {

                  }
                  //第二路占空比
                   sTXCAN_JN910.Data[2]=0x0A;
                   if(i==0)
                   {
                      // Float2Byte(fclVars[i].CH_B_PI_Cur.Out,sTXCAN_JN910.Data,3);
                       sTXCAN_JN910.Data[3]=EPwm1Regs.CMPA.bit.CMPA&0x00FF;
                       sTXCAN_JN910.Data[4]=(EPwm1Regs.CMPA.bit.CMPA&0xFF00)>>8;
                       sTXCAN_JN910.Data[5]=gCputime0_time3&0x000000FF;
                       sTXCAN_JN910.Data[6]=(gCputime0_time3&0x0000FF00)>>8;
                       sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                   }
                   else if(i==1)
                   {
                       if(gCputime0_time3>gCputime0_max)
                                gCputime0_max=gCputime0_time3;
                       sTXCAN_JN910.Data[3]=(int)(CH_PI_Vsense[0].Umax*100)&0x00FF;
                       sTXCAN_JN910.Data[4]=((int)(CH_PI_Vsense[0].Umax*100)&0xFF00)>>8;
                       sTXCAN_JN910.Data[5]=gCputime0_max&0x000000FF;
                       sTXCAN_JN910.Data[6]=(gCputime0_max&0x0000FF00)>>8;
                       sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                   }
                   else if(i==2)
                   {
                        // Float2Byte(fclVars[i].CH_B_PI_Cur.Out,sTXCAN_JN910.Data,3);
                       sTXCAN_JN910.Data[3]=(int)(CH_PI_Vsense[0].up*100)&0x00FF;
                       sTXCAN_JN910.Data[4]=((int)(CH_PI_Vsense[0].up*100)&0xFF00)>>8;
                       sTXCAN_JN910.Data[5]=(int)(CH_PI_Vsense[0].ui*100)&0x00FF;
                       sTXCAN_JN910.Data[6]=((int)(CH_PI_Vsense[0].ui*100)&0xFF00)>>8;
                       sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                   }
                   else if(i==3)
                   {
                          // Float2Byte(fclVars[i].CH_B_PI_Cur.Out,sTXCAN_JN910.Data,3);
                       sTXCAN_JN910.Data[3]=(int)(CH_PI_Vsense[0].i1*100)&0x00FF;
                       sTXCAN_JN910.Data[4]=((int)(CH_PI_Vsense[0].i1*100)&0xFF00)>>8;
                       sTXCAN_JN910.Data[5]=(int)(CH_PI_Vsense[0].Out*100)&0x00FF;
                       sTXCAN_JN910.Data[6]=((int)(CH_PI_Vsense[0].Out*100)&0xFF00)>>8;
                       sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
                   }

                   if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
                   {

                   }

            }
        }
        DINT;
        Active_Report.Req_Report=FALSE;
        EINT;
    }

}

