/*
 * CAN_JN910.c
 *
 *  Created on: 2021年6月17日
 *      Author: JC
 */
#include "F28x_Project.h"
extern float32 Vol_out,Cur_out,Vdc_in;
float testa,testb,testc;
unsigned char Ver_Software[]  ="Vxxxx";
//函数定义
typedef void (*ptrCANFun)();
static const  ptrCANFun sCANFunJump[] =
{
    sSetProtct,                 //0x01
    sInquireSetProtct,          //0x02
    sCANullCommand,             //0x03
    sInquireRecondData,         //0x04
    sSetControlPara,            //0x05
    sSetCharger,                //0x06
    sInquireParaData,           //0x07
    sCANullCommand,             //0x08
    sSetCalibrationPara,        //0x09
    sInquireCalibrationData,    //0x0a
    sInquireADData,             //0x0b
    sSysInquireData,            //0x0c
    sSysSetCommand,             //0x0d
    sSysUpdateSet,               //0x0E
    sSysDebug                   //0x0F
};
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Init_CAN_JN910(void)
{
    CAN_JN910_RX.Flag_RX                       = FALSE;
    CAN_JN910_RX.Flag_Receiving                = FALSE;
    CAN_JN910_RX.Cnt_Byte_RX                   = 0;
    CAN_JN910_RX.Num_To_RX                     = 0;
    CAN_JN910_RX.Cnt_Timeout                   = 0;
    CAN_JN910_RX.Num_Timeout                   = 10000;//10mS

    CAN_JN910_RX_Broadcast.Flag_RX             = FALSE;
    CAN_JN910_RX_Broadcast.Flag_Receiving      = FALSE;
    CAN_JN910_RX_Broadcast.Cnt_Byte_RX         = 0;
    CAN_JN910_RX_Broadcast.Num_To_RX           = 0;
    CAN_JN910_RX_Broadcast.Cnt_Timeout         = 0;
    CAN_JN910_RX_Broadcast.Num_Timeout         = 10000;//10mS


    CAN_JN910_TX.Cnt_Byte_Sent                 = 0;
    CAN_JN910_TX.Num_Byte                      = 0;
    CAN_JN910_TX.Req_TX                        = FALSE;
    CAN_JN910_TX.First_TX_MBOX                 = TRUE;
    CAN_JN910_TX.Cnt_Timeout                   = 0;
    CAN_JN910_TX.Num_Timeout                   = 10000;//10mS

    CAN_JN910_TX_Error.Cnt_Byte_Sent           = 0;
    CAN_JN910_TX_Error.Num_Byte                = 0;
    CAN_JN910_TX_Error.Req_TX                  = FALSE;
    CAN_JN910_TX_Error.First_TX_MBOX           = TRUE;
    CAN_JN910_TX_Error.Cnt_Timeout             = 0;
    CAN_JN910_TX_Error.Num_Timeout             = 10000;//10mS


    union union_ID_CAN ID_CAN;

    ID_CAN.ID_JN910.NC                         =0x06;
    ID_CAN.ID_JN910.Priority_ID                =0x00;
    ID_CAN.ID_JN910.Devices_Type               =0x00;
    ID_CAN.ID_JN910.Frame_Seque                =0x40;
    ID_CAN.ID_JN910.Src_Addr                   =0x00;
    ID_CAN.ID_JN910.Dst_Addr                   =0x00;


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
   Uint16 uwCrcTemp = 0xFFFF;
   for(Cnt=0;Cnt<Offset;Cnt++)
   {
       Ptr++;
   }

   for(Cnt=0;Cnt<Num;Cnt++)
   {
       uwCrcTemp = sCRC16(*Ptr++,uwCrcTemp);
   }
   return uwCrcTemp;
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


void CAN_CMD_Broadcast(void)
{
    if(CAN_JN910_RX_Broadcast.ID_CAN.ID_JN910.Dst_Addr==CMD_CODE_MULTI_FRAME)
    {
        Uint16 Data_CRC;

          Data_CRC=(CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX-4]<<8)+CAN_JN910_RX_Broadcast.Data[CAN_JN910_RX_Broadcast.Cnt_Byte_RX-3];
          if(CheckSum(CAN_JN910_RX_Broadcast.Data,2,CAN_JN910_RX_Broadcast.Cnt_Byte_RX-6)==Data_CRC)
          {
              if((CAN_JN910_RX_Broadcast.Data[5]>=0x01)&&(CAN_JN910_RX_Broadcast.Data[5]<=0x0F))
              {
                  sCANFunJump[CAN_JN910_RX_Broadcast.Data[5]-1]();
              }

          }
          else
          {
            //报数据校验错误
             // Return_Data_Checksum_Error();
          }

    }

}
//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void CAN_APP_RX(void)
{
    if(CAN_JN910_RX_Broadcast.Error_Code!=CAN_OK)
    {
        CAN_JN910_RX_Broadcast.Error_Code=CAN_OK;
        DINT;
        CAN_JN910_RX_Broadcast.Flag_RX=FALSE;
        EINT;
    }
    else if(CAN_JN910_RX.Error_Code!=CAN_OK)
    {
        CAN_JN910_RX.Error_Code=CAN_OK;
        DINT;
        CAN_JN910_RX.Flag_RX=FALSE;
        EINT;
    }
    else if((CAN_JN910_RX_Broadcast.Flag_RX==TRUE)&&(CAN_JN910_TX.Req_TX==FALSE))
    {
        CAN_CMD_Broadcast();
        DINT;
        CAN_JN910_RX_Broadcast.Flag_RX=FALSE;
        EINT;
    }
    else if((CAN_JN910_RX.Flag_RX==TRUE)&&(CAN_JN910_TX.Req_TX==FALSE))
    {
   //     CAN_CMD_Private();
        DINT;
        CAN_JN910_RX.Flag_RX=FALSE;
        EINT;
    }

    if((Active_Report.Req_Report==TRUE)&&(CAN_JN910_TX.Req_TX==FALSE))
    {
        //上抛数据
        CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
        CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
        CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
        CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
        CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;

        CAN_JN910_TX.Num_Byte=0;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x16;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x84;
      //  if(DCDC_Service.Now_Mode==DCDC_STANDBY)
        {
           CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0;
        }
       /* else if(DCDC_Service.Now_Mode==DCDC_SHUTDOWN)
        {
           CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=3;
        }
        else if((DCDC_Service.Now_Mode>=DCDC_CC)&&(DCDC_Service.Now_Mode<=DCDC_CV)&&(DCDC_Service.Cnt_Step_Mask==0))
        {
         CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=2;
        }
        else
        {
           CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=1;
        }*/
       // CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=DCDC_Service.Now_Mode;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
       // CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(DCDC_Service.Fault_Module|DCDC_Service.Fault_Slaver1)&0x00FF;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x02;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x03;
        float Temp=80.0;
       // Temp=Temp/10000;
        Float2Byte(Vol_out,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
        CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;
        Temp=90.0;

        Float2Byte(Cur_out,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
        CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;
        Temp=100.0;

        Float2Byte(Vdc_in,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
        CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;


        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,(CAN_JN910_TX.Num_Byte-2))&0xFF00>>8);
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,(CAN_JN910_TX.Num_Byte-2))&0x00FF);
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;

        CAN_JN910_TX.Req_TX    = TRUE;

        DINT;
        Active_Report.Req_Report=FALSE;
        EINT;
    }

}

void CAN_APP_TX(void)
{
    if(CAN_JN910_TX.Req_TX==TRUE)
       {

          // if((CAN_JN910_TX.First_TX_MBOX==TRUE)||(CANTRS.bit.TRS4==0))
           if(1)
           {
               if(CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr==0xFE)
               {
                   //多帧发送
                   if(CAN_JN910_TX.Flag_Sending==FALSE)
                   {

                       CAN_JN910_TX.Length_Data=8;
                       CAN_JN910_TX.pucMsgData[0]    = CAN_JN910_TX.Data[0];
                       CAN_JN910_TX.pucMsgData[1]    = CAN_JN910_TX.Data[1];
                       CAN_JN910_TX.pucMsgData[2]    = CAN_JN910_TX.Data[2];
                       CAN_JN910_TX.pucMsgData[3]    = CAN_JN910_TX.Data[3];
                       CAN_JN910_TX.pucMsgData[4]    = CAN_JN910_TX.Data[4];
                       CAN_JN910_TX.pucMsgData[5]    = CAN_JN910_TX.Data[5];
                       CAN_JN910_TX.pucMsgData[6]    = CAN_JN910_TX.Data[6];
                       CAN_JN910_TX.pucMsgData[7]    = CAN_JN910_TX.Data[7];
                       CAN_BSP((struct_CAN_JN910_TX*)&CAN_JN910_TX);

                       CAN_JN910_TX.Cnt_Byte_Sent         = 8;
                       CAN_JN910_TX.Cnt_Timeout           = 0;

                       if(CAN_JN910_TX.Num_Byte<=8)
                       {
                           CAN_JN910_TX.Req_TX            = FALSE;
                           CAN_JN910_TX.Flag_Sending      = FALSE;
                       }
                       else
                       {
                           CAN_JN910_TX.Flag_Sending      = TRUE;
                       }
                       CAN_JN910_TX.First_TX_MBOX         = FALSE;
                   }
                   else
                   {

                       if(CAN_JN910_TX.Num_Byte-CAN_JN910_TX.Cnt_Byte_Sent<8)
                       {
                           CAN_JN910_TX.Length_Data   = CAN_JN910_TX.Num_Byte-CAN_JN910_TX.Cnt_Byte_Sent;
                       }
                       else
                       {
                           CAN_JN910_TX.Length_Data   = 8;
                       }


                       CAN_JN910_TX.pucMsgData[0]    = CAN_JN910_TX.Data[CAN_JN910_TX.Cnt_Byte_Sent++];
                       CAN_JN910_TX.pucMsgData[1]     = CAN_JN910_TX.Data[CAN_JN910_TX.Cnt_Byte_Sent++];
                       CAN_JN910_TX.pucMsgData[2]    = CAN_JN910_TX.Data[CAN_JN910_TX.Cnt_Byte_Sent++];
                       CAN_JN910_TX.pucMsgData[3]    = CAN_JN910_TX.Data[CAN_JN910_TX.Cnt_Byte_Sent++];
                       CAN_JN910_TX.pucMsgData[4]     = CAN_JN910_TX.Data[CAN_JN910_TX.Cnt_Byte_Sent++];
                       CAN_JN910_TX.pucMsgData[5]     = CAN_JN910_TX.Data[CAN_JN910_TX.Cnt_Byte_Sent++];
                       CAN_JN910_TX.pucMsgData[6]     = CAN_JN910_TX.Data[CAN_JN910_TX.Cnt_Byte_Sent++];
                       CAN_JN910_TX.pucMsgData[7]     = CAN_JN910_TX.Data[CAN_JN910_TX.Cnt_Byte_Sent++];

                       CAN_BSP((struct_CAN_JN910_TX*)&CAN_JN910_TX);
                       CAN_JN910_TX.Cnt_Timeout=0;
                       if((CAN_JN910_TX.Cnt_Byte_Sent>=CAN_JN910_TX.Num_Byte)||(CAN_JN910_TX.Cnt_Byte_Sent>=LEN_BUFF_CAN))
                       {
                           CAN_JN910_TX.Req_TX            = FALSE;
                           CAN_JN910_TX.Flag_Sending      = FALSE;
                       }
                   }
               }

           }
       }


}

//------------------------保护设置功能指令----------------------//
void sSetProtct(void)
{
    CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
    CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;

    CAN_JN910_TX.Num_Byte=0;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x0a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x81;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
    if(CAN_JN910_RX_Broadcast.Data[6]==0x01)
    {
        testa =(Byte2Float(CAN_JN910_RX_Broadcast.Data,7));
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
    }
    else
    {
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
    }
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x02;

    if(CAN_JN910_RX_Broadcast.Data[11]==0x02)
    {
        testb =(int32)(Byte2Float(CAN_JN910_RX_Broadcast.Data,12));
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
    }
    else
    {
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
    }
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,(CAN_JN910_TX.Num_Byte-2))&0xFF00>>8);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,(CAN_JN910_TX.Num_Byte-2))&0x00FF);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;

    CAN_JN910_TX.Req_TX    = TRUE;

}

//-----------------保护功能设置查询指令------------------//
void sInquireSetProtct(void)
{

    CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
    CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;


    CAN_JN910_TX.Num_Byte=0;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x10;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x82;

    if(CAN_JN910_RX_Broadcast.Data[6]==0x01)
    {

        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
        Float2Byte(testa,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
        CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;
    }
    if(CAN_JN910_RX_Broadcast.Data[7]==0x02)
    {
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x02;
        Float2Byte(testb,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
        CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;
    }
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,CAN_JN910_TX.Num_Byte)&0xFF00>>8);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,CAN_JN910_TX.Num_Byte)&0x00FF);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;


    CAN_JN910_TX.Req_TX    = TRUE;


}


void sInquireRecondData(void)//记录数据上返
{

       Active_Report.Req_Report=TRUE;
}

void sSetControlPara(void) //设置控制参数
{
       Uint16 Temp_Mode;
      if(CAN_JN910_RX_Broadcast.Data[6]==0x01)
      {
         RELAY_ON;
      }
      else
      {
          RELAY_OFF;
      }



      if(CAN_JN910_RX_Broadcast.Data[7]==0x01)
      {
        //  testdata=Byte2Float(CAN_JN910_RX_Broadcast.Data,8);
          testa=Byte2Float(CAN_JN910_RX_Broadcast.Data,8);
        //  Interface_DCDC.Req_Vol                          =(int32)(Byte2Float(CAN_JN910_RX_Broadcast.Data,8)*10000+0.5);
      }
      if(CAN_JN910_RX_Broadcast.Data[12]==0x02)
      {
          testb=Byte2Float(CAN_JN910_RX_Broadcast.Data,13);
        //  Interface_DCDC.Req_Cur_Total                    =(int32)(Byte2Float(CAN_JN910_RX_Broadcast.Data,13)*10000+0.5);
      }
      if(CAN_JN910_RX_Broadcast.Data[17]==0x03)
      {
          testc=Byte2Float(CAN_JN910_RX_Broadcast.Data,18);
      //    Interface_DCDC.Req_Power  =(int32)(Byte2Float(CAN_JN910_RX_Broadcast.Data,18)*10000+0.5);
      }
      if(CAN_JN910_RX_Broadcast.Data[22]==0x01)
      {
          Active_Report.Num_Period=(Uint32)(CAN_JN910_RX_Broadcast.Data[23]+(CAN_JN910_RX_Broadcast.Data[24]<<8))*100;
          Active_Report.Cnt_Period=0;
      }
    CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
    CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;

    CAN_JN910_TX.Num_Byte=0;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x07;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x85;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,(CAN_JN910_TX.Num_Byte-2))&0xFF00>>8);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,(CAN_JN910_TX.Num_Byte-2))&0x00FF);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;

    CAN_JN910_TX.Req_TX    = TRUE;

}

void sSetCharger(void)
{
    Uint16 i;
    for( i=0;i<4;i++)
    {
        if(CAN_JN910_RX_Broadcast.Data[4]&1==1)
        {
            if(CAN_JN910_RX_Broadcast.Data[6]==0x01)
            {
                Interface_DCDC[i].Req_ChangeMode=TRUE;
                Interface_DCDC[i].Req_Mode=DCDC_DEBUG;
            }
            else if(CAN_JN910_RX_Broadcast.Data[6]==0x02)
            {
                Interface_DCDC[i].Req_Mode         = DCDC_STANDBY;
                Interface_DCDC[i].Req_ChangeMode   = TRUE;

            }

        }
        CAN_JN910_RX_Broadcast.Data[4]=CAN_JN910_RX_Broadcast.Data[4]>>1;
    }


    CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
    CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;


    CAN_JN910_TX.Num_Byte=0;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x07;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x86;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;

    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,(CAN_JN910_TX.Num_Byte-2))&0xFF00>>8);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,(CAN_JN910_TX.Num_Byte-2))&0x00FF);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
    CAN_JN910_TX.Req_TX    = TRUE;


}

void sInquireParaData(void)
{

    CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
    CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;


    CAN_JN910_TX.Num_Byte=0;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x19;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x87;

    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
   // CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=Interface_DCDC.Req_Mode;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
    Float2Byte(testa,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
    CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;

    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x02;
    Float2Byte(testb,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
    CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;

    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x03;
    Float2Byte(testc,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
    CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;

    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(Active_Report.Num_Period&0xFF00)>>8;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=Active_Report.Num_Period&0x00FF;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,CAN_JN910_TX.Num_Byte)&0xFF00>>8);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,CAN_JN910_TX.Num_Byte)&0x00FF);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;


    CAN_JN910_TX.Req_TX    = TRUE;

}

void sSetCalibrationPara(void)
{
      CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
      CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
      CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
      CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
      CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;

      CAN_JN910_TX.Num_Byte=0;
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x0a;
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x89;
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
      if(CAN_JN910_RX_Broadcast.Data[6]==0x01)
      {
          testa =(Byte2Float(CAN_JN910_RX_Broadcast.Data,7));
          CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
      }
      else
      {
          CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
      }
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x02;

      if(CAN_JN910_RX_Broadcast.Data[11]==0x02)
      {
          testb =(int32)(Byte2Float(CAN_JN910_RX_Broadcast.Data,12));
          CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
      }
      else
      {
          CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
      }
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,8)&0xFF00>>8);
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,8)&0x00FF);
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
      CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;

      CAN_JN910_TX.Req_TX    = TRUE;



}

void sInquireCalibrationData(void)
{
    CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
    CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
    CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;


    CAN_JN910_TX.Num_Byte=0;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x10;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x8A;

    if(CAN_JN910_RX_Broadcast.Data[6]==0x01)
    {

        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
        Float2Byte(testa,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
        CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;
    }
    if(CAN_JN910_RX_Broadcast.Data[7]==0x02)
    {
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x02;
        Float2Byte(testb,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
        CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;
    }
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,CAN_JN910_TX.Num_Byte)&0xFF00>>8);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,CAN_JN910_TX.Num_Byte)&0x00FF);
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;


    CAN_JN910_TX.Req_TX    = TRUE;

}

void sInquireADData(void)
{
   CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
   CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
   CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
   CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
   CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;


   CAN_JN910_TX.Num_Byte=0;
   CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
   CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
   CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
   CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x10;
   CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
   CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x8B;

   if(CAN_JN910_RX_Broadcast.Data[6]==0x01)
   {

       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
       Float2Byte(testa,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
       CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;
   }
   if(CAN_JN910_RX_Broadcast.Data[7]==0x02)
   {
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x02;
       Float2Byte(testb,CAN_JN910_TX.Data,CAN_JN910_TX.Num_Byte);
       CAN_JN910_TX.Num_Byte=CAN_JN910_TX.Num_Byte+4;
   }
   CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,CAN_JN910_TX.Num_Byte)&0xFF00>>8);
   CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,CAN_JN910_TX.Num_Byte)&0x00FF);
   CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
   CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;


   CAN_JN910_TX.Req_TX    = TRUE;

}

void sSysInquireData(void)
{
       Uint16 Cnt,Num;
       CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
       CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
       CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
       CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
       CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;

       Num=sizeof(Ver_Software)/sizeof(unsigned char)-1;
       CAN_JN910_TX.Num_Byte=0;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=7+Num;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x8C;




       if(CAN_JN910_RX_Broadcast.Data[6]==0x01)
       {

           CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
           for(Cnt=0;Cnt<Num;Cnt++)
           {
                       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=Ver_Software[Cnt];
           }
       }

       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,CAN_JN910_TX.Num_Byte)&0xFF00>>8);
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,CAN_JN910_TX.Num_Byte)&0x00FF);
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;


       CAN_JN910_TX.Req_TX    = TRUE;

}

void sSysSetCommand(void)
{
       CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
       CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
       CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
       CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
       CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;

       CAN_JN910_TX.Num_Byte=0;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x0a;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x8D;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
       if(CAN_JN910_RX_Broadcast.Data[6]==0x01)
       {
           testa =(Byte2Float(CAN_JN910_RX_Broadcast.Data,7));
           CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
       }
       else
       {
           CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
       }
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x02;

       if(CAN_JN910_RX_Broadcast.Data[11]==0x02)
       {
           testb =(int32)(Byte2Float(CAN_JN910_RX_Broadcast.Data,12));
           CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
       }
       else
       {
           CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
       }
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,8)&0xFF00>>8);
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,8)&0x00FF);
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
       CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;

       CAN_JN910_TX.Req_TX    = TRUE;


}

void sSysUpdateSet(void)
{
        CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
        CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
        CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
        CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
        CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;

        CAN_JN910_TX.Num_Byte=0;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x0a;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x8E;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
        if(CAN_JN910_RX_Broadcast.Data[6]==0x01)
        {
            testa =(Byte2Float(CAN_JN910_RX_Broadcast.Data,7));
            CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
        }
        else
        {
            CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
        }
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x02;

        if(CAN_JN910_RX_Broadcast.Data[11]==0x02)
        {
            testb =(int32)(Byte2Float(CAN_JN910_RX_Broadcast.Data,12));
            CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;
        }
        else
        {
            CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
        }
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,8)&0xFF00>>8);
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,8)&0x00FF);
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;

        CAN_JN910_TX.Req_TX    = TRUE;

}

void sCANullCommand(void)
{

}

void sSysDebug(void)
{

       Uint16 i;
       unsigned char CH;

        CAN_JN910_TX.ID_CAN.ID_JN910.Priority_ID                =0x00;
        CAN_JN910_TX.ID_CAN.ID_JN910.Devices_Type               =0x03;
        CAN_JN910_TX.ID_CAN.ID_JN910.Frame_Seque                =0x00;
        CAN_JN910_TX.ID_CAN.ID_JN910.Src_Addr                   =0x00;
        CAN_JN910_TX.ID_CAN.ID_JN910.Dst_Addr                   =0xFE;

        CAN_JN910_TX.Num_Byte=0;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x5a;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x00;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x0a;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=CAN_JN910_RX_Broadcast.Data[4];
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x8F;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0x01;

        CH=CAN_JN910_RX_Broadcast.Data[4];
          for( i=0;i<4;i++)
          {
              if(CH&1==1)
              {
                    if(CAN_JN910_RX_Broadcast.Data[6]==0x01)
                    {
                        CMPAValue[i] =(Uint16)((Byte2Float(CAN_JN910_RX_Broadcast.Data,7))*PWM_PERIOD);
                        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte]=0x01;
                    }
                    else
                    {
                        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte]=0x00;
                    }
                    CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte+1]=0x02;
                    if(CAN_JN910_RX_Broadcast.Data[11]==0x02)
                    {
                        CMPBValue[i] =(Uint16)((Byte2Float(CAN_JN910_RX_Broadcast.Data,12))*PWM_PERIOD);

                            CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte+2]=0x01;
                    }
                    else
                    {
                            CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte+2]=0x00;
                    }

              }
              CH=CH>>1;
          }
          CAN_JN910_TX.Num_Byte++;
          CAN_JN910_TX.Num_Byte++;
          CAN_JN910_TX.Num_Byte++;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,(CAN_JN910_TX.Num_Byte-2))&0xFF00>>8);
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=(INT8U)(CheckSum(CAN_JN910_TX.Data,2,(CAN_JN910_TX.Num_Byte-2))&0x00FF);
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;
        CAN_JN910_TX.Data[CAN_JN910_TX.Num_Byte++]=0xa5;

        CAN_JN910_TX.Req_TX    = TRUE;


}


