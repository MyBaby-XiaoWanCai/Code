/*
 * CAN_JN910.c
 *
 *  Created on: 2021年6月17日
 *      Author: JC
 */
#include "F28x_Project.h"

float32 Calibration_Var[4][12];
float32 Test_Var[4][10];
float32 Protect_Para_Var[4][4];
float32 Control_Para_Var[4][12];
float32 Calibration_Para_Var[4][10];
float32 Debug_Para_Var[4][8];

float32 Para_Var[4][12];
Uint32  AD_Para_Var[4][6];
Uint16  Run_Mode_Var[4][1];

unsigned char Manufacturer[]  ="JingNeng";
unsigned char Model[]         ="5V160A";
unsigned char SN[]            ="PBT2011004";
unsigned char Ver_Software[]  ="V120";
unsigned char Ver_Hardware[]  ="Vxxxx";

//函数定义
typedef void (*ptrCANFun)();
static const  ptrCANFun sCANFunJump[] =
{
    sSetProtct,                 //0x01  //保护类设置
    sInquireSetProtct,          //0x02  //保护类查询
    sCANullCommand,             //0x03
    sInquireRecondData,         //0x04
    sSetControlPara,            //0x05
    sSetCharger,                //0x06
    sInquireParaData,           //0x07
    sSetCalibrationPara,        //0x08
    sInquireCalibrationData,    //0x09
    sInquireADData,             //0x0a
    sSysInquireData,            //0x0b
    sSysSetCommand,             //0x0c
    sSysUpdateSet,              //0x0d
    sDebugParaSet,              //0x0E
    sInquireDebugPara,          //0x0F
    sDebugRecondData,           //0x10
    sReadRecordData             //0x11
};
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Init_CAN_JN910(void)
{

    Active_Report.Cnt_Period                    = 0;
    Active_Report.Num_Period                    = 0;
    Active_Report.Req_Report                    = FALSE;
    Active_Report.Flag_Status_Error_Report      = FALSE;
    Active_Report.Flag_Temp_Bat_Report          = FALSE;
    Active_Report.Flag_Vol_Cur_Report           = FALSE;
}
//*********************************************************************************
//Description: 单字节CRC16校验
//Parameters :
//Return     :
//*********************************************************************************
Uint16 sCRC16(unsigned char ubDataIn,Uint16 uwCrcIn)
{
    Uint16 i;
    Uint16 uwTempCrc;
    uwTempCrc = uwCrcIn^(ubDataIn&0x00FF);
    for(i = 0; i < 8; i++)
    {
        if((uwTempCrc&0x0001)!=0 )
        {
            uwTempCrc =(uwTempCrc>>1)^0xA001;
        }
        else
        {
            uwTempCrc = uwTempCrc>>1;
        }
    }
    return uwTempCrc;
}

Uint16 CheckSum(unsigned char *Ptr,Uint16 Offset,Uint16 Num)
{
    Uint16 Cnt;
    Uint16 Sum=0;
    for(Cnt=0;Cnt<Offset;Cnt++)
    {
        Ptr++;
    }
    for(Cnt=0;Cnt<Num;Cnt++)
    {
        Sum=Sum+*Ptr++;
    }
    return(Sum&0x00FF);
}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Float2Byte(float Data,unsigned char *Ptr_Buff,Uint16 Offset_Buff)
{
    static float Data_Convert=0;
    Uint16 *Ptr=(Uint16 *)&Data_Convert;
    Data_Convert=Data;
    while(Offset_Buff-->0)
    {
        Ptr_Buff++;
    }
    *Ptr_Buff++=(*Ptr)&0x00FF;
    *Ptr_Buff++=((*Ptr++)>>8)&0x00FF;
    *Ptr_Buff++=(*Ptr)&0x00FF;
    *Ptr_Buff++=((*Ptr++)>>8)&0x00FF;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
float Byte2Float(unsigned char *Ptr_Buff,Uint16 Offset_Buff)
{
    static float Data_Convert=0;
    Uint16 *Ptr=(Uint16 *)&Data_Convert;
    while(Offset_Buff-->0)
    {
        Ptr_Buff++;
    }
    Uint16 Temp1,Temp2;

    Temp1=(*Ptr_Buff++)&0x00FF;
    Temp2=(*Ptr_Buff++)&0x00FF;

    *Ptr++=(Temp2<<8)+Temp1;


    Temp1=(*Ptr_Buff++)&0x00FF;
    Temp2=(*Ptr_Buff++)&0x00FF;

    *Ptr++=(Temp2<<8)+Temp1;

    return(Data_Convert);
}


void sCRCNACKCommand()
{

      sTXCAN_JN910.ui32MsgID=0x00000001;
      sTXCAN_JN910.Data[0]=pack.data[0];
      sTXCAN_JN910.Data[1]=0x80+pack.data[1];
      sTXCAN_JN910.Data[3]=1;
      sTXCAN_JN910.Data[4]=0;
      sTXCAN_JN910.Data[5]=0;
      sTXCAN_JN910.Data[6]=0;
      sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCANMessage.Data,0,7));

        if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
        {

        }

}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void CAN_APP_RX(void)
{

    if(BSP_CAN_Que_Out(&que1Con,&pack) == DEF_CAN_QUE_OK)
    {
        if(CheckSum(pack.data,0,7)==pack.data[7])
        {
            if((pack.data[1]>=0x01)&&(pack.data[1]<=0x12))
            {
                sCANFunJump[pack.data[1]-1]();
            }
        }
        else
        {
            sCRCNACKCommand();
        }
    }
}


void CAN_APP_TX(void)
{
    static Uint16 record_tx_cnt=0;

    if(BSP_CAN_Que_Out(&que2Con,&pack_tx) == DEF_CAN_QUE_OK)
    {
       CAN_BSP((CAN_APP_PACK*)&pack_tx);
    }
    if(CH_Public_Variable.Flag_Record==1)
    {

          if(BSP_Record_Que_Out(&que5Con,&pack_tx_record) == DEF_CAN_QUE_OK)
          {
              CAN_BSP((CAN_APP_PACK*)&pack_tx_record);
          }
        /*  else
          {
              CH_Public_Variable.Flag_Record=0;
          }*/
          record_tx_cnt++;
          if(record_tx_cnt>=2000)
          {
              record_tx_cnt=0;
              CH_Public_Variable.Flag_Record=0;
              sTXCAN_JN910.ui32MsgID=0x0000000B;
              sTXCAN_JN910.Data[0]=1;
              sTXCAN_JN910.Data[1]=2;
              sTXCAN_JN910.Data[3]=1;
              sTXCAN_JN910.Data[4]=0;
              sTXCAN_JN910.Data[5]=0;
              sTXCAN_JN910.Data[6]=0;
              sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCANMessage.Data,0,7));

                if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
                {

                }
          }


    }

}

//------------------------保护设置功能指令----------------------//
void sSetProtct(void)
{

    Uint16 i,j;
    unsigned char CH;
    CH=pack.data[0];

    if(((CH&0x03)==0x03)||((CH&0x0C)==0x0C))
    {
        CH_Public_Variable.Parall_Total=2;
    }
    else
    {
        CH_Public_Variable.Parall_Total=1;
    }

    if(pack.data[2]==0x01)
    {
         for(i=0;i<4;i++)
         {
              if(CH&(0x01<<i))
              {
                  Protect_Para_Var[i][0] =  Byte2Float(pack.data,3);
                   //额定电压保护
                  if(CH_Public_Variable.Parall_Total==1)
                  {
                      Protect_Global[i].CH_Over_Vol_Bat.Ref_Value=Protect_Para_Var[i][0];
                      Protect_Global[i].CH_Over_Vol.Ref_Value=Protect_Para_Var[i][0]+10;
                  }
                  else if(CH_Public_Variable.Parall_Total==2)
                  {
                      Protect_Global[i].CH_Over_Vol_Bat.Ref_Value=Protect_Para_Var[i][0];
                      Protect_Global[i].CH_Over_Vol.Ref_Value=Protect_Para_Var[i][0]+10;
                  }
              }
         }
    }
    else if(pack.data[2]==0x02)
    {
        //额定电流保护
         for(i=0;i<4;i++)
         {
              if(CH&(0x01<<i))
              {
                  Protect_Para_Var[i][1] =  Byte2Float(pack.data,3);
                  if(CH_Public_Variable.Parall_Total==1)
                   {
                       Protect_Global[i].CH_Over_Cur[2].Ref_Value=Protect_Para_Var[i][1];
                   }
                   else if(CH_Public_Variable.Parall_Total==2)
                   {
                       Protect_Global[i].CH_Over_Cur[2].Ref_Value=Protect_Para_Var[i][1]/2;
                   }
              }

         }

    }


    sCRCACKCommand();

}

void sCRCACKCommand()
{

    sTXCAN_JN910.ui32MsgID=0x00000001;
    sTXCAN_JN910.Data[0]=pack.data[0];
    sTXCAN_JN910.Data[1]=0x80+pack.data[1];
    sTXCAN_JN910.Data[2]=pack.data[2];
    sTXCAN_JN910.Data[3]=0;
    sTXCAN_JN910.Data[4]=0;
    sTXCAN_JN910.Data[5]=0;
    sTXCAN_JN910.Data[6]=0;
    sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
    if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
    {

    }
}


//-----------------保护功能设置查询指令------------------//
void sInquireSetProtct(void)
{
    Uint16 i,j,k;
    unsigned char CH;


    sTXCAN_JN910.ui32MsgID=0x00000001;

    sTXCAN_JN910.Data[0]=pack.data[0];
    sTXCAN_JN910.Data[1]=0x82;
    CH=pack.data[0];
    k=0;
    for(i=0;i<4;i++)
    {
         if(CH& (0x01<<i))
         {
             k++;
         }
    }


    for(i=0;i<4;i++)
    {
         if(CH&(0x01<<i))
         {
            if(pack.data[2]==0x01)
            {
                 Float2Byte(Protect_Global[i].CH_Over_Vol_Bat.Ref_Value,sTXCAN_JN910.Data,3);
            }
            else if(pack.data[2]==0x02)
            {
                 Float2Byte(Protect_Global[i].CH_Over_Cur[2].Ref_Value,sTXCAN_JN910.Data,3);
            }

         }
    }
    sTXCAN_JN910.Data[2]=pack.data[2];
    sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
    if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
    {

    }
}


void sInquireRecondData(void)//记录数据上返
{
    Active_Report.Req_Report=TRUE;
}

void sSetControlPara(void) //设置控制参数
{
       Uint16 i;
       unsigned char CH;
       CH=pack.data[0];
       if(((CH&0x03)==0x03)||((CH&0x0C)==0x0C))
       {
             CH_Public_Variable.Parall_Total=2;
       }
       else
       {
             CH_Public_Variable.Parall_Total=1;
       }
       sTXCAN_JN910.ui32MsgID=0x00000001;
       sTXCAN_JN910.Data[0]=pack.data[0];
       sTXCAN_JN910.Data[1]=0x85;

       if(pack.data[2]==0x01)
       {
            for(i=0;i<4;i++)
            {
                 if(CH&(0x01<<i))
                 {
                     Run_Mode_Var[i][0] =pack.data[3];
                 }
            }
       }
       else if(pack.data[2]==0x02)//额定电压
       {
            for(i=0;i<4;i++)
            {
                 if(CH&(0x01<<i))
                 {
                     Control_Para_Var[i][0] =  Byte2Float(pack.data,3);
                     DCDC_Service[i].Set_Vout= Control_Para_Var[i][0];
                 }
            }
       }
       else if(pack.data[2]==0x03)//额定电流
       {
            for(i=0;i<4;i++)
            {
                  if(CH&(0x01<<i))
                  {
                      Control_Para_Var[i][1] = Byte2Float(pack.data,3);
                      if(CH_Public_Variable.Parall_Total==1)
                      {
                          DCDC_Service[i].Set_Iout=Control_Para_Var[i][1];
                      }
                      else if(CH_Public_Variable.Parall_Total==2)
                      {
                          DCDC_Service[i].Set_Iout=Control_Para_Var[i][1]/2;
                      }
                      Interface_DCDC[i].Req_Refresh_Cur=TRUE;

                  }
            }


       }
       sTXCAN_JN910.Data[2]=pack.data[2];
       sTXCAN_JN910.Data[3]=0;
       sTXCAN_JN910.Data[4]=0;
       sTXCAN_JN910.Data[5]=0;
       sTXCAN_JN910.Data[6]=0;
       sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));

       if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
       {

       }
}

void sSetCharger(void)
{
      Uint16 i,j;
      unsigned char CH;
      CH=pack.data[0];

      sTXCAN_JN910.ui32MsgID=0x00000001;

      sTXCAN_JN910.Data[0]=pack.data[0];
      sTXCAN_JN910.Data[1]=0x86;

      if(pack.data[2]==0x01)  //启动
      {
           for(i=0;i<4;i++)
           {
                if(CH&(0x01<<i))
                {
                    if(DCDC_Service[i].Now_Mode==2)
                    {
                        if(Run_Mode_Var[i][0]==1)
                        {
                           Interface_DCDC[i].Req_Mode=DCDC_CC;   //恒流充电
                        }
                        else if(Run_Mode_Var[i][0]==2)
                        {
                            Interface_DCDC[i].Req_Mode=DCDC_CV;  //恒压充电
                        }
                        else if(Run_Mode_Var[i][0]==3)
                        {
                            Interface_DCDC[i].Req_Mode=DCDC_DCC; //恒流放电
                        }
                        else if(Run_Mode_Var[i][0]==4)
                        {
                            Interface_DCDC[i].Req_Mode=DCDC_DCV; //恒压放电
                        }
                        Interface_DCDC[i].Req_ChangeMode=TRUE;
                    }
                }
           }
      }
      else if(pack.data[2]==0x02)    //停机
      {
           for(i=0;i<4;i++)
           {
               if(CH&(0x01<<i))
               {
                  // DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                   Interface_DCDC[i].Req_ChangeMode=TRUE;
                   Interface_DCDC[i].Req_Mode  = DCDC_STANDBY;

               }
           }

      }
      else if(pack.data[2]==0x03)   //清除故障
      {
           for(i=0;i<4;i++)
           {
              if(CH&(0x01<<i))
              {
                  Fault_Code[i].Err_Global=0;
                  Fault_Code[i].Err_Global_Bak=0;
                  DCDC_Service[i].Fault_Module=0;
                  Fault_Code[i].Code=0;
                  if(i==0)
                  {
                      EALLOW;
                      EPwm1Regs.TZCLR.bit.DCAEVT1=1;
                      EPwm1Regs.TZCLR.bit.DCBEVT1=1;
                      EPwm2Regs.TZCLR.bit.DCAEVT1=1;
                      EPwm2Regs.TZCLR.bit.DCBEVT1=1;
                      EDIS;
                  }
                  else if(i==1)
                  {
                      EALLOW;
                      EPwm3Regs.TZCLR.bit.DCAEVT1=1;
                      EPwm3Regs.TZCLR.bit.DCBEVT1=1;
                      EPwm4Regs.TZCLR.bit.DCAEVT1=1;
                      EPwm4Regs.TZCLR.bit.DCBEVT1=1;
                      EDIS;
                  }
                  else if(i==2)
                  {
                      EALLOW;
                      EPwm5Regs.TZCLR.bit.DCAEVT1=1;
                      EPwm5Regs.TZCLR.bit.DCBEVT1=1;
                      EPwm6Regs.TZCLR.bit.DCAEVT1=1;
                      EPwm6Regs.TZCLR.bit.DCBEVT1=1;
                      EDIS;
                  }
                  else if(i==3)
                  {
                     EALLOW;
                     EPwm7Regs.TZCLR.bit.DCAEVT1=1;
                     EPwm7Regs.TZCLR.bit.DCBEVT1=1;
                     EPwm8Regs.TZCLR.bit.DCAEVT1=1;
                     EPwm8Regs.TZCLR.bit.DCBEVT1=1;
                     EDIS;
                  }
              }
          }

      }
      sTXCAN_JN910.Data[2]=pack.data[2];
      sTXCAN_JN910.Data[3]=0;
      sTXCAN_JN910.Data[4]=0;
      sTXCAN_JN910.Data[5]=0;
      sTXCAN_JN910.Data[6]=0;
      sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));

      if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
      {

      }

}

void sInquireParaData(void)
{
    Uint16 i,j,k;
    unsigned char CH;

    sTXCAN_JN910.ui32MsgID=0x00000001;

    sTXCAN_JN910.Data[0]=pack.data[0];
    sTXCAN_JN910.Data[1]=0x87;
    CH=pack.data[0];
    k=0;
    for(i=0;i<4;i++)
    {
         if(CH& (0x01<<i))
         {
             k++;
         }
    }

   /* if(k!=1)
    {
        sTXCAN_JN910.Data[2]=pack.data[2];
        sTXCAN_JN910.Data[3]=0;
        sTXCAN_JN910.Data[4]=0;
        sTXCAN_JN910.Data[5]=0;
        sTXCAN_JN910.Data[6]=0;
        sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(CAN_JN910_TX.Data,0,7));

        return;
    }*/

    for(i=0;i<4;i++)
    {
        if(CH& (0x01<<i))
        {
            if(pack.data[2]==0x01)
            {
                sTXCAN_JN910.Data[3]=Run_Mode_Var[i][0];
                sTXCAN_JN910.Data[4]=0;
                sTXCAN_JN910.Data[5]=0;
                sTXCAN_JN910.Data[6]=0;
            }
            else if(pack.data[2]==0x02)
            {
                 Float2Byte(DCDC_Service[i].Set_Vout,sTXCAN_JN910.Data,3);
            }
            else if(pack.data[2]==0x03)
            {
                 Float2Byte( DCDC_Service[i].Set_Iout,sTXCAN_JN910.Data,3);
            }
        }
    }
    sTXCAN_JN910.Data[2]=pack.data[2];
    sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
    if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
    {

    }

}

void sSetCalibrationPara(void)
{
      Uint16 i;
      unsigned char CH;
      CH=pack.data[0];

      sTXCAN_JN910.ui32MsgID=0x00000001;
      sTXCAN_JN910.Data[0]=pack.data[0];
      sTXCAN_JN910.Data[1]=0x88;

      if(pack.data[2]==0x01)  //sense电压采样参数K
      {
           for(i=0;i<4;i++)
           {
                if(CH&(0x01<<i))
                {
                    Calibration_Para_Var[i][0] = Byte2Float(pack.data,3);
                }
           }
      }
      else if(pack.data[2]==0x02)  //电池电压采样参数B
      {
           for(i=0;i<4;i++)
           {
                if(CH&(0x01<<i))
                {
                    Calibration_Para_Var[i][1] =  Byte2Float(pack.data,3);
                }
           }
      }
      else if(pack.data[2]==0x03)  //模块端口电压采样参数K
      {
           for(i=0;i<4;i++)
           {
                 if(CH&(0x01<<i))
                 {
                     Calibration_Para_Var[i][2] =  Byte2Float(pack.data,3);
                 }
           }
      }
      else if(pack.data[2]==0x04)  //模块端口电压采样参数B
      {
          for(i=0;i<4;i++)
          {
                if(CH&(0x01<<i))
                {
                    Calibration_Para_Var[i][3] =  Byte2Float(pack.data,3);
                }
          }
      }
      else if(pack.data[2]==0x05)  //充电-输出电流采样参数K
      {
          for(i=0;i<4;i++)
          {
               if(CH&(0x01<<i))
               {
                   Calibration_Para_Var[i][4] =  Byte2Float(pack.data,3);
               }
          }
      }
      else if(pack.data[2]==0x06)//充电-输出电流采样参数B
      {
          for(i=0;i<4;i++)
          {
               if(CH&(0x01<<i))
               {
                   Calibration_Para_Var[i][5] =  Byte2Float(pack.data,3);
               }
         }
     }
     else if(pack.data[2]==0x07)//放电-输出电流采样参数K
     {
           for(i=0;i<4;i++)
           {
                if(CH&(0x01<<i))
                {
                    Calibration_Para_Var[i][6] =  Byte2Float(pack.data,3);
                }
          }
    }
     else if(pack.data[2]==0x08)//放电-输出电流采样参数B
     {
            for(i=0;i<4;i++)
            {
                 if(CH&(0x01<<i))
                 {
                     Calibration_Para_Var[i][7] =  Byte2Float(pack.data,3);
                 }
           }
     }
      sTXCAN_JN910.Data[2]=pack.data[2];
      sTXCAN_JN910.Data[3]=0;
      sTXCAN_JN910.Data[4]=0;
      sTXCAN_JN910.Data[5]=0;
      sTXCAN_JN910.Data[6]=0;
      sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));

      if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
      {

      }



}

void sInquireCalibrationData(void)
{
   Uint16 i,j,k;
   unsigned char CH;

   sTXCAN_JN910.ui32MsgID=0x00000001;

   sTXCAN_JN910.Data[0]=pack.data[0];
   sTXCAN_JN910.Data[1]=0x89;
   CH=pack.data[0];
   k=0;
   for(i=0;i<4;i++)
   {
        if(CH& (0x01<<i))
        {
            k++;
        }
   }

  /* if(k!=1)
   {
       sTXCAN_JN910.Data[2]=pack.data[2];
       sTXCAN_JN910.Data[3]=0;
       sTXCAN_JN910.Data[4]=0;
       sTXCAN_JN910.Data[5]=0;
       sTXCAN_JN910.Data[6]=0;
       sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(CAN_JN910_TX.Data,0,7));

       return;
   }*/

   for(i=0;i<4;i++)
   {
       if(CH& (0x01<<i))
       {
           if(pack.data[2]==0x01)
           {
               Float2Byte(Calibration_Para_Var[i][0],sTXCAN_JN910.Data,3);
           }
           else if(pack.data[2]==0x02)
           {
                Float2Byte(Calibration_Para_Var[i][1],sTXCAN_JN910.Data,3);
           }
           else if(pack.data[2]==0x03)
           {
                Float2Byte(Calibration_Para_Var[i][2],sTXCAN_JN910.Data,3);
           }
           else if(pack.data[2]==0x04)
           {
                Float2Byte(Calibration_Para_Var[i][3],sTXCAN_JN910.Data,3);
           }
           else if(pack.data[2]==0x05)
           {
                Float2Byte(Calibration_Para_Var[i][4],sTXCAN_JN910.Data,3);
           }
           else if(pack.data[2]==0x06)
           {
                Float2Byte(Calibration_Para_Var[i][5],sTXCAN_JN910.Data,3);
           }
           else if(pack.data[2]==0x07)
           {
                Float2Byte(Calibration_Para_Var[i][6],sTXCAN_JN910.Data,3);
           }
           else if(pack.data[2]==0x08)
           {
                Float2Byte(Calibration_Para_Var[i][7],sTXCAN_JN910.Data,3);
           }
       }
   }
   sTXCAN_JN910.Data[2]=pack.data[2];
   sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
   if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
   {

   }

}

void sInquireADData(void)
{
      Uint16 i,j,k;
      unsigned char CH;

      sTXCAN_JN910.ui32MsgID=0x00000001;

      sTXCAN_JN910.Data[0]=pack.data[0];
      sTXCAN_JN910.Data[1]=0x8a;
      CH=pack.data[0];
      k=0;
      for(i=0;i<4;i++)
      {
           if(CH& (0x01<<i))
           {
               k++;
           }
      }

     /* if(k!=1)
      {
          sTXCAN_JN910.Data[2]=pack.data[2];
          sTXCAN_JN910.Data[3]=0;
          sTXCAN_JN910.Data[4]=0;
          sTXCAN_JN910.Data[5]=0;
          sTXCAN_JN910.Data[6]=0;
          sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(CAN_JN910_TX.Data,0,7));

          return;
      }*/

      for(i=0;i<4;i++)
      {
          if(CH& (0x01<<i))
          {
              if(pack.data[2]==0x01)
              {
                  AD_Para_Var[i][0]=(((Uint32)pack.data[6] << 24) |
                                     ((Uint32)pack.data[5] << 16) |
                                     ((Uint32)pack.data[4] << 8)  |
                                     ((Uint32)pack.data[3]));

              }
              else if(pack.data[2]==0x02)
              {
                  AD_Para_Var[i][1]=(((Uint32)pack.data[6] << 24) |
                                   ((Uint32)pack.data[5] << 16) |
                                   ((Uint32)pack.data[4] << 8)  |
                                   ((Uint32)pack.data[3]));
              }
              else if(pack.data[2]==0x03)
              {
                  AD_Para_Var[i][2]=(((Uint32)pack.data[6] << 24) |
                                     ((Uint32)pack.data[5] << 16) |
                                     ((Uint32)pack.data[4] << 8)  |
                                     ((Uint32)pack.data[3]));
              }
              else if(pack.data[2]==0x04)
              {
                  AD_Para_Var[i][3]=(((Uint32)pack.data[6] << 24) |
                                     ((Uint32)pack.data[5] << 16) |
                                     ((Uint32)pack.data[4] << 8)  |
                                     ((Uint32)pack.data[3]));
              }
              else if(pack.data[2]==0x05)
              {
                  AD_Para_Var[i][4]=(((Uint32)pack.data[6] << 24) |
                                     ((Uint32)pack.data[5] << 16) |
                                     ((Uint32)pack.data[4] << 8)  |
                                     ((Uint32)pack.data[3]));
              }
              else if(pack.data[2]==0x06)
              {
                  AD_Para_Var[i][5]=(((Uint32)pack.data[6] << 24) |
                                     ((Uint32)pack.data[5] << 16) |
                                     ((Uint32)pack.data[4] << 8)  |
                                     ((Uint32)pack.data[3]));
              }


          }
      }
      sTXCAN_JN910.Data[2]=pack.data[2];
      sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
      if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
      {

      }


}

void sSysInquireData(void)
{
    Uint16 i,j,k;
    unsigned char CH;
    Uint16 Num;

      sTXCAN_JN910.ui32MsgID=0x00000001;

      sTXCAN_JN910.Data[0]=pack.data[0];
      sTXCAN_JN910.Data[1]=0x8B;
      CH=pack.data[0];
      k=0;
      for(i=0;i<4;i++)
      {
           if(CH& (0x01<<i))
           {
               k++;
           }
      }

     /* if(k!=1)
      {
          sTXCAN_JN910.Data[2]=pack.data[2];
          sTXCAN_JN910.Data[3]=0;
          sTXCAN_JN910.Data[4]=0;
          sTXCAN_JN910.Data[5]=0;
          sTXCAN_JN910.Data[6]=0;
          sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));

          return;
      }*/


      if(pack.data[2]==0x01)
      {
          Num=sizeof(Ver_Software)/sizeof(unsigned char)-1;
          for(i=0;i<4;i++)
              sTXCAN_JN910.Data[i+3]=Ver_Software[i];
      }
      else if(pack.data[2]==0x02)
      {

      }
      else if(pack.data[2]==0x03)
      {

      }
      else if(pack.data[2]==0x04)
      {

      }
      else if(pack.data[2]==0x05)
      {

      }
      else if(pack.data[2]==0x06)
      {

      }
      else if(pack.data[2]==0x07)
      {

      }
      else if(pack.data[2]==0x08)
      {

      }
      sTXCAN_JN910.Data[2]=pack.data[2];
      sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
      if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
      {

      }

}

void sSysSetCommand(void)
{


}

void sSysUpdateSet(void)
{

}

void sCANullCommand(void)
{

}

void sDebugParaSet(void)
{

      Uint16 i,j;
      unsigned char CH;
      CH=pack.data[0];

      sTXCAN_JN910.ui32MsgID=0x00000001;
      sTXCAN_JN910.Data[0]=pack.data[0];
      sTXCAN_JN910.Data[1]=0x8E;

      if(pack.data[2]==0x01)
      {
           for(i=0;i<4;i++)
           {
                if(CH&(0x01<<i))
                {
                    Debug_Para_Var[i][0] = Byte2Float(pack.data,3);//电感电流闭环Kp
                    fclVars[i].CH_A_PI_Cur.Kp= Debug_Para_Var[i][0];
                    fclVars[i].CH_B_PI_Cur.Kp= Debug_Para_Var[i][0];

                }
           }
      }
      else if(pack.data[2]==0x02)
      {
           for(i=0;i<4;i++)
           {
                if(CH&(0x01<<i))
                {
                    Debug_Para_Var[i][1] =  Byte2Float(pack.data,3);//电感电流闭环Ki
                    fclVars[i].CH_A_PI_Cur.Ki=Debug_Para_Var[i][1];
                    fclVars[i].CH_B_PI_Cur.Ki=Debug_Para_Var[i][1];

                }
           }
      }
      else if(pack.data[2]==0x03)
      {
           for(i=0;i<4;i++)
           {
                 if(CH&(0x01<<i))
                 {
                     Debug_Para_Var[i][2] =  Byte2Float(pack.data,3);//输出电流闭环Kp
                     CH_PI_Iout[i].Kp=Debug_Para_Var[i][2];
                 }
           }
      }
      else if(pack.data[2]==0x04)
      {
          for(i=0;i<4;i++)
          {
                if(CH&(0x01<<i))
                {
                    Debug_Para_Var[i][3] =  Byte2Float(pack.data,3);//输出电流闭环Ki
                    CH_PI_Iout[i].Ki=Debug_Para_Var[i][3];
                }
          }
      }
      else if(pack.data[2]==0x05)
      {
          for(i=0;i<4;i++)
          {
                if(CH&(0x01<<i))
                {
                    Debug_Para_Var[i][4] =  Byte2Float(pack.data,3);//电压闭环Kp
                    CH_PI_Vout[i].Kp=Debug_Para_Var[i][4];
                    CH_PI_Vsense[i].Kp=Debug_Para_Var[i][4];
                }
          }
      }
      else if(pack.data[2]==0x06)
      {
           for(i=0;i<4;i++)
           {
                 if(CH&(0x01<<i))
                 {
                     Debug_Para_Var[i][5] =  Byte2Float(pack.data,3);//电压闭环Ki
                     CH_PI_Vout[i].Ki=Debug_Para_Var[i][5];
                     CH_PI_Vsense[i].Ki=Debug_Para_Var[i][5];
                 }
           }
      }
      else if(pack.data[2]==0x07)
       {
            for(i=0;i<4;i++)
            {
                  if(CH&(0x01<<i))
                  {
                      Debug_Para_Var[i][6] =  Byte2Float(pack.data,3);//风扇
                      if(i==0)
                      {
                          EPwm9Regs.CMPA.bit.CMPA         = 10000-10000*Debug_Para_Var[i][6] ;
                          EPwm9Regs.CMPB.bit.CMPB    =10000*Debug_Para_Var[i][6];
                      }
                      if(i==1)
                      {
                          EPwm10Regs.CMPA.bit.CMPA    =10000-10000*Debug_Para_Var[i][6] ;
                          EPwm10Regs.CMPB.bit.CMPB = 10000*Debug_Para_Var[i][6];
                      }
                      if(i==2)
                      {
                          EPwm11Regs.CMPA.bit.CMPA         = 10000-10000*Debug_Para_Var[i][6];
                          EPwm11Regs.CMPB.bit.CMPB    =10000*Debug_Para_Var[i][6];
                      }
                      if(i==3)
                      {
                          EPwm12Regs.CMPA.bit.CMPA    =10000-10000*Debug_Para_Var[i][6] ;
                          EPwm12Regs.CMPB.bit.CMPB = 10000*Debug_Para_Var[i][6];
                      }
                  }
            }
       }
      sTXCAN_JN910.Data[2]=pack.data[2];
      sTXCAN_JN910.Data[3]=0;
      sTXCAN_JN910.Data[4]=0;
      sTXCAN_JN910.Data[5]=0;
      sTXCAN_JN910.Data[6]=0;
      sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));

      if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
      {

      }

}

void sInquireDebugPara(void)
{

    Uint16 i,j,k;
    unsigned char CH;

    sTXCAN_JN910.ui32MsgID=0x00000001;

    sTXCAN_JN910.Data[0]=pack.data[0];
    sTXCAN_JN910.Data[1]=0x8F;
    CH=pack.data[0];
    k=0;
    for(i=0;i<4;i++)
    {
         if(CH& (0x01<<i))
         {
             k++;
         }
    }

   /* if(k!=1)
    {
        sTXCAN_JN910.Data[2]=pack.data[2];
        sTXCAN_JN910.Data[3]=0;
        sTXCAN_JN910.Data[4]=0;
        sTXCAN_JN910.Data[5]=0;
        sTXCAN_JN910.Data[6]=0;
        sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));

        return;
    }*/

    for(i=0;i<4;i++)
    {
        if(CH& (0x01<<i))
        {
            if(pack.data[2]==0x01)
            {
                  Float2Byte(Debug_Para_Var[i][0],sTXCAN_JN910.Data,3);
            }
            else if(pack.data[2]==0x02)
            {
                 Float2Byte(Debug_Para_Var[i][1],sTXCAN_JN910.Data,3);
            }
            else if(pack.data[2]==0x03)
            {
                 Float2Byte(Debug_Para_Var[i][2],sTXCAN_JN910.Data,3);
            }
            else if(pack.data[2]==0x04)
           {
                Float2Byte(Debug_Para_Var[i][3],sTXCAN_JN910.Data,3);
           }
           else if(pack.data[2]==0x05)
           {
                Float2Byte(Debug_Para_Var[i][4],sTXCAN_JN910.Data,3);
           }
           else if(pack.data[2]==0x06)
           {
                Float2Byte(Debug_Para_Var[i][5],sTXCAN_JN910.Data,3);
           }
           else if(pack.data[2]==0x07)
            {
                 Float2Byte(Debug_Para_Var[i][6],sTXCAN_JN910.Data,3);

            }
        }
    }
    sTXCAN_JN910.Data[2]=pack.data[2];
    sTXCAN_JN910.Data[7]=(INT8U)(CheckSum(sTXCAN_JN910.Data,0,7));
    if(BSP_CAN_Que_In(&que2Con,&sTXCAN_JN910) == DEF_CAN_QUE_FULL)
    {

    }

}
Uint16 n=0;
void sDebugRecondData(void)
{

}

void sReadRecordData(void)
{
    CH_Public_Variable.Flag_Record=1;

}

