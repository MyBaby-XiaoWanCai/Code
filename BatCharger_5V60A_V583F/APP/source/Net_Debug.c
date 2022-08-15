/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_Debug.c
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义

int32 Simulate_Data0=17000;
int32 Simulate_Data1=3100;//debug
int32 Simulate_Data2=17000;
int32 Simulate_Data3=17000;
int32 Simulate_Data4=11000;
int32 Simulate_Data5=11000;
int32 Simulate_Data6=-14712;
int32 Simulate_Data7=250;//-14712;

Uint16 Req_DogReset=0;

extern struct_PI CH_A_PI_Cur_Charge;
extern struct_PI CH_A_PI_Cur_Discharge;
extern struct_PI CH_B_PI_Cur_Charge;
extern struct_PI CH_B_PI_Cur_Discharge;
//函数声明

//函数定义
//*********************************************************************************
//Description: 网口调试相关协议数据处理
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Socket_Debug(void)
{
    if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Status_Recive==0x01)
    {
        Uint16 temp=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[0];
        //Uint16 Data_CRC;
        if((temp>=3)&&(temp<=LEN_TCP_TX_BUFF)&&(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[temp-1]==0x0D))
        {
            //Data_CRC=((Uint16)TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[temp-3]<<8)+TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[temp-2];
            //if(sCRC16N(TCP_Socket[SERV_TCP_DEBUG].Data_RX.pData,temp-3)==Data_CRC)
            {
                if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[1]==0x81)
                {
                  Uint16 Num_Byte=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[7];
                  Uint16 Cnt_Byte=0;

                  Uint32 Addr0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[6];
                  Uint32 Addr1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[5];
                  Uint32 Addr2=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[4];
                  Uint32 Addr3=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[3];
                  Uint32 Addr=(Addr3<<24)+(Addr2<<16)+(Addr1<<8)+Addr0;
                  switch(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[2])
                  {
                      //读DSP的RAM区
                      case 0x01:
                      {
                          Uint16 *Ptr=(Uint16 *)Addr;
                          Uint16 Data=0;
                          if(Num_Byte>LEN_TCP_TX_BUFF) Num_Byte=LEN_TCP_TX_BUFF;
                          for(Cnt_Byte=0;Cnt_Byte<Num_Byte;Cnt_Byte++)
                          {
                              Data=*Ptr++;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[Cnt_Byte++]=(Data>>8)&0x00FF;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[Cnt_Byte]=(Data)&0x00FF;
                          }
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=Num_Byte;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}

                      //读外挂FLASH区
                      case 0x02:
                      {
                          if(Num_Byte>LEN_TCP_TX_BUFF-6) Num_Byte=LEN_TCP_TX_BUFF-6;
                          SPI_Flash_Read(SERV_FLASH_DEBUG,Addr,Num_Byte);
                          while(Qurey_SPI_Flash_Read(SERV_FLASH_DEBUG)!=TRUE);

                          for(Cnt_Byte=0;Cnt_Byte<Num_Byte;Cnt_Byte++)
                          {
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[Cnt_Byte]=Interface_SPI_Flash[SERV_FLASH_DEBUG].Data[Cnt_Byte];
                              Interface_SPI_Flash[SERV_FLASH_DEBUG].Data[Cnt_Byte]=0;
                          }
                          Reset_SPI_FLASH_Service(SERV_FLASH_DEBUG);
                          Num_Byte = sCRC16N(&TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0],Cnt_Byte);
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[Cnt_Byte++]=(INT8U)((Num_Byte&0xFF00)>>8);
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[Cnt_Byte++]=(INT8U)((Num_Byte&0x00FF));
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[Cnt_Byte++]=0x0D;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=Cnt_Byte;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}

                      //读外挂EEPROM区
                      case 0x03:
                      {
                          if(Num_Byte>16) Num_Byte=16;
                          EEPROM_Read_Num(Addr,Num_Byte,&TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]);
                          //while(Qurey_EEPROM_Read(SERV_EE_DEBUG)!=TRUE);
                          //for(Cnt_Byte=0;Cnt_Byte<Num_Byte;Cnt_Byte++)
                          //{
                          //    TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[Cnt_Byte]=Interface_I2C[SERV_EE_DEBUG].Data[Cnt_Byte].Data;
                          //    Interface_I2C[SERV_EE_DEBUG].Data[Cnt_Byte].Data=0;
                          //}
                          //Reset_EEPROM_Service(SERV_EE_DEBUG);
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=Num_Byte;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}

                      //读PWM调试接口脉宽数据
                      case 0x04:
                      {
                    	  TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x00;
                    	  TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                    	  TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x04;
                    	  TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=(CH_A.PWMxA_Debug>>8)&0x00FF;
                    	  TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=(CH_A.PWMxA_Debug)&0x00FF;
                    	  TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=(CH_A.PWMxB_Debug>>8)&0x00FF;
                    	  TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=(CH_A.PWMxB_Debug)&0x00FF;

                    	  TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=(CH_B.PWMxA_Debug>>8)&0x000000FF;
                    	  TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=(CH_B.PWMxA_Debug)&0x000000FF;
                    	  TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=(CH_B.PWMxB_Debug>>8)&0x000000FF;
                    	  TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=(CH_B.PWMxB_Debug)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=11;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}

                      //读PWM调试接口脉宽数据
                      case 0x05:
                      {
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x00;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x05;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Value_t1>>24)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Value_t1>>16)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Value_t1>>8)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Value_t1)&0x00FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Value_t2>>24)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Value_t2>>16)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Value_t2>>8)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Value_t2)&0x00FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[11]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Ref_Delta>>24)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[12]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Ref_Delta>>16)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[13]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Ref_Delta>>8)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[14]=(WorkStep_Data.Prot_Step_Vol_Change_Time.Ref_Delta)&0x00FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[15]=(WorkStep_Data.Prot_Step_BatShort.Time_Delay>>24)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[16]=(WorkStep_Data.Prot_Step_BatShort.Time_Delay>>16)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[17]=(WorkStep_Data.Prot_Step_BatShort.Time_Delay>>8)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[18]=(WorkStep_Data.Prot_Step_BatShort.Time_Delay)&0x00FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[19]=(WorkStep_Data.Prot_Step_BatShort.Ref_Value_H>>24)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[20]=(WorkStep_Data.Prot_Step_BatShort.Ref_Value_H>>16)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[21]=(WorkStep_Data.Prot_Step_BatShort.Ref_Value_H>>8)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[22]=(WorkStep_Data.Prot_Step_BatShort.Ref_Value_H)&0x00FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[23]=(WorkStep_Data.Prot_Step_BatShort.Ref_Value_L>>24)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[24]=(WorkStep_Data.Prot_Step_BatShort.Ref_Value_L>>16)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[25]=(WorkStep_Data.Prot_Step_BatShort.Ref_Value_L>>8)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[26]=(WorkStep_Data.Prot_Step_BatShort.Ref_Value_L)&0x00FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[27]=(WorkStep_Data.Prot_Step_BatShort.Now_Value>>24)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[28]=(WorkStep_Data.Prot_Step_BatShort.Now_Value>>16)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[29]=(WorkStep_Data.Prot_Step_BatShort.Now_Value>>8)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[30]=(WorkStep_Data.Prot_Step_BatShort.Now_Value)&0x00FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[31]=(WorkStep_Data.Prot_Step_Bat_Under_Temp.Ref_Value>>24)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[32]=(WorkStep_Data.Prot_Step_Bat_Under_Temp.Ref_Value>>16)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[33]=(WorkStep_Data.Prot_Step_Bat_Under_Temp.Ref_Value>>8)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[34]=(WorkStep_Data.Prot_Step_Bat_Under_Temp.Ref_Value)&0x00FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[35]=(WorkStep_Data.Prot_Step_Vol_Interval_Neg.Ref_Delta>>24)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[36]=(WorkStep_Data.Prot_Step_Vol_Interval_Neg.Ref_Delta>>16)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[37]=(WorkStep_Data.Prot_Step_Vol_Interval_Neg.Ref_Delta>>8)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[38]=(WorkStep_Data.Prot_Step_Vol_Interval_Neg.Ref_Delta)&0x00FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[39]=(WorkStep_Data.Prot_Step_Vol_Interval_Pos.Ref_Delta>>24)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[40]=(WorkStep_Data.Prot_Step_Vol_Interval_Pos.Ref_Delta>>16)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[41]=(WorkStep_Data.Prot_Step_Vol_Interval_Pos.Ref_Delta >>8)&0x00FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[42]=(WorkStep_Data.Prot_Step_Vol_Interval_Pos.Ref_Delta )&0x00FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=43;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}

                      //读故障数据
                      case 0x06:
                      {
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x00;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x06;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=(Fault_Code.Err_Global>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=(Fault_Code.Err_Global>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=(Fault_Code.Err_Global>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=(Fault_Code.Err_Global   )&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=(Fault_Code.Err_Step>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=(Fault_Code.Err_Step>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=(Fault_Code.Err_Step>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=(Fault_Code.Err_Step)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[11]=(Fault_Code.Alarm>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[12]=(Fault_Code.Alarm>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[13]=(Fault_Code.Alarm>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[14]=(Fault_Code.Alarm)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[15]=(DCDC_Service.Fault_Module>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[16]=(DCDC_Service.Fault_Module>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[17]=(DCDC_Service.Fault_Module>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[18]=(DCDC_Service.Fault_Module)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[19]=(DCDC_Service.Fault_Slaver1>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[20]=(DCDC_Service.Fault_Slaver1>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[21]=(DCDC_Service.Fault_Slaver1>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[22]=(DCDC_Service.Fault_Slaver1)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[23]= ((-Rate_Cur_Compute.Rate_Cur_Filter)>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[24]= ((-Rate_Cur_Compute.Rate_Cur_Filter))&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[25]= (Rate_Cur_Compute.Cnt_Delay)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[26]= (Rate_Cur_Compute.Cnt_Error)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[27]=(Data_Sum.Temp_NTC1>>8)&0x000000FF;//
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[28]=(Data_Sum.Temp_NTC1)&0x000000FF;//
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[29]=(Data_Sum.Temp_NTC2>>8)&0x000000FF;//(DCDC_Service.Fault_Slaver3>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[30]=(Data_Sum.Temp_NTC2)&0x000000FF;//(DCDC_Service.Fault_Slaver3)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[31]=(DCDC_Service.Fault_Slaver4>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[32]=(DCDC_Service.Fault_Slaver4>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[33]=(DCDC_Service.Fault_Slaver4>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[34]=(DCDC_Service.Fault_Slaver4)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[35]=Interface_DCDC.Req_Mode;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[36]=Interface_DCDC.Req_ChangeMode;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[37]=Interface_DCDC.Req_Refresh_Cur;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[38]=DCDC_Service.Now_Mode;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=39;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}


                      //模拟量数据
                      case 0x07:
                      {
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x23;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x07;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=(DCDC_Service.Now_Vol_Bus>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=(DCDC_Service.Now_Vol_Bus>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=(DCDC_Service.Now_Vol_Bus>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=(DCDC_Service.Now_Vol_Bus   )&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=(DCDC_Service.Now_Vol_Out>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=(DCDC_Service.Now_Vol_Out>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=(DCDC_Service.Now_Vol_Out>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=(DCDC_Service.Now_Vol_Out)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[11]=(DCDC_Service.Now_Vol_Sense>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[12]=(DCDC_Service.Now_Vol_Sense>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[13]=(DCDC_Service.Now_Vol_Sense>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[14]=(DCDC_Service.Now_Vol_Sense)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[15]=(CH_A.Now_Cur>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[16]=(CH_A.Now_Cur>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[17]=(CH_A.Now_Cur>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[18]=(CH_A.Now_Cur)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[19]=(CH_B.Now_Cur>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[20]=(CH_B.Now_Cur>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[21]=(CH_B.Now_Cur>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[22]=(CH_B.Now_Cur)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[23]=(DCDC_Service.Set_Cur_Total>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[24]=(DCDC_Service.Set_Cur_Total>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[25]=(DCDC_Service.Set_Cur_Total>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[26]=(DCDC_Service.Set_Cur_Total)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[27]=(DCDC_Service.Set_Vol>>24)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[28]=(DCDC_Service.Set_Vol>>16)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[29]=(DCDC_Service.Set_Vol>>8)&0x000000FF;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[30]=(DCDC_Service.Set_Vol)&0x000000FF;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[31]=Interface_DCDC.Req_Mode;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[32]=DCDC_Service.Mode_Parall;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[33]=DCDC_Service.Parall_Total;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[34]=TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus;

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=35;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}

                      //读同步整流使能标志
                        case 0x08:
                        {
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x0A;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x08;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=Interface_DCDC.EN_CHA_SynRect;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=Interface_DCDC.EN_CHB_SynRect;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=CH_A.EN_SynRect;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=CH_B.EN_SynRect;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=Interface_DCDC.Req_Mode;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=DCDC_Service.Now_Mode;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=0x0D;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=10;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                        break;}

                        //AD数据
                        case 0x09:
                        {
                        	int32 Temp;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x23;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x09;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=(Data_AD.F2808_CH2>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=(Data_AD.F2808_CH2   )&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=(Data_AD.AD7606_CH3>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=(Data_AD.AD7606_CH3)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=(Data_AD.AD7606_CH2>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=(Data_AD.AD7606_CH2)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[11]=(CH_A.Cur_Scale_H.K_Now>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[12]=(CH_A.Cur_Scale_H.K_Now>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[13]=(CH_A.Cur_Scale_H.K_Now>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[14]=(CH_A.Cur_Scale_H.K_Now)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[15]=(Data_AD.AD7606_CH7>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[16]=(Data_AD.AD7606_CH7)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[17]=(Data_AD_Sum.Sum_Cur_CHA_AD_Copy>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[18]=(Data_AD_Sum.Sum_Cur_CHA_AD_Copy>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[19]=(Data_AD_Sum.Sum_Cur_CHA_AD_Copy>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[20]=(Data_AD_Sum.Sum_Cur_CHA_AD_Copy)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[21]=(Data_AD.AD7606_CH6>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[22]=(Data_AD.AD7606_CH6)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[23]=(Data_AD.AD7606_CH0>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[24]=(Data_AD.AD7606_CH0)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[25]=(Data_AD.F2808_CH0>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[26]=(Data_AD.F2808_CH0)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[27]=(Data_AD.F2808_CH1>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[28]=(Data_AD.F2808_CH1)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[29]=(WorkStep_Service.Res_Contact>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[30]=(WorkStep_Service.Res_Contact)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[31]=DCDC_Service.Flag_Aging;//WorkStep_Service.State;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[32]=Interface_DCDC.Req_Aging;//WorkStep_Service.Step_State;

                            Temp=Data_Sum.Vol_Out-Data_Sum.Vol_Sense;
                            if(Temp<0) Temp=-Temp;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[33]=(Temp>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[34]=(Temp)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=35;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                        break;}

                        //DCDC_Service相关数据数据
                        case 0x0A:
                        {
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x23;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x0A;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=(CH_A.Set_Cur>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=(CH_A.Set_Cur>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=(CH_A.Set_Cur>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=(CH_A.Set_Cur   )&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=(CH_B.Set_Cur>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=(CH_B.Set_Cur>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=(CH_B.Set_Cur>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=(CH_B.Set_Cur)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[11]=(CH_A.Now_Cur>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[12]=(CH_A.Now_Cur>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[13]=(CH_A.Now_Cur>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[14]=(CH_A.Now_Cur)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[15]=(CH_B.Now_Cur>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[16]=(CH_B.Now_Cur>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[17]=(CH_B.Now_Cur>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[18]=(CH_B.Now_Cur)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[19]=(CH_A.Limit_Set_Cur>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[20]=(CH_A.Limit_Set_Cur>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[21]=(CH_A.Limit_Set_Cur>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[22]=(CH_A.Limit_Set_Cur)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[23]=(DCDC_Service.Set_Cur_Total>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[24]=(DCDC_Service.Set_Cur_Total>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[25]=(DCDC_Service.Set_Cur_Total>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[26]=(DCDC_Service.Set_Cur_Total)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[27]=(DCDC_Service.Set_Vol>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[28]=(DCDC_Service.Set_Vol>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[29]=(DCDC_Service.Set_Vol>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[30]=(DCDC_Service.Set_Vol)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[31]=DCDC_Service.Flag_CV_Step;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[32]=DCDC_Service.Mode_Parall;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[33]=DCDC_Service.Parall_Total;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[34]=DCDC_Service.Now_Mode;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=35;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                        break;}


                        //掉电接续相关数据
                        case 0x0B:
                        {
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x23;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x0B;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=(WorkStep_PowerFail.Step_State_Now>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=(WorkStep_PowerFail.Step_State_Now)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=(WorkStep_PowerFail.Step_State_EEPROM>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=(WorkStep_PowerFail.Step_State_EEPROM)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=(WorkStep_PowerFail.Step_Index_EEPROM>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=(WorkStep_PowerFail.Step_Index_EEPROM)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=(WorkStep_PowerFail.Step_Time_EEPROM>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=(WorkStep_PowerFail.Step_Time_EEPROM>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[11]=(WorkStep_PowerFail.Step_Time_EEPROM>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[12]=(WorkStep_PowerFail.Step_Time_EEPROM)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[13]=(WorkStep_PowerFail.Step_Ah_EEPROM>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[14]=(WorkStep_PowerFail.Step_Ah_EEPROM>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[15]=(WorkStep_PowerFail.Step_Ah_EEPROM>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[16]=(WorkStep_PowerFail.Step_Ah_EEPROM)&0x000000FF;


                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[17]=(WorkStep_PowerFail.Step_Wh_EEPROM>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[18]=(WorkStep_PowerFail.Step_Wh_EEPROM>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[19]=(WorkStep_PowerFail.Step_Wh_EEPROM>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[20]=(WorkStep_PowerFail.Step_Wh_EEPROM)&0x000000FF;


                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[21]=(WorkStep_Service.Step_Time>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[22]=(WorkStep_Service.Step_Time>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[23]=(WorkStep_Service.Step_Time>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[24]=(WorkStep_Service.Step_Time)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[25]=(WorkStep_Service.Step_Ah>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[26]=(WorkStep_Service.Step_Ah>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[27]=(WorkStep_Service.Step_Ah>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[28]=(WorkStep_Service.Step_Ah)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[29]=(WorkStep_Service.Step_Wh>>24)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[30]=(WorkStep_Service.Step_Wh>>16)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[31]=(WorkStep_Service.Step_Wh>>8)&0x000000FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[32]=(WorkStep_Service.Step_Wh)&0x000000FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[33]=DCDC_Service.Parall_Total;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[34]=DCDC_Service.Now_Mode;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=35;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                        break;}

                        //读取PI参数
                        case 0x0C:
                        {
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=14;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x0C;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=0x01;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=CH_A_PI_Cur_Charge.Kp;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=0x02;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=CH_A_PI_Cur_Charge.Ki;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=0x03;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=CH_A_PI_Cur_Discharge.Kp;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=0x04;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=CH_A_PI_Cur_Discharge.Ki;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[11]=Parall_Master.EEPROM_Parall_Mode;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[12]=Parall_Master.EEPROM_Parall_Num;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[13]=(PI_Vol_Charge.Ki>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[14]=(PI_Vol_Charge.Ki)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[15]=(PI_Vol_Discharge.Ki>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[16]=(PI_Vol_Discharge.Ki)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[17]=0x0D;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=18;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                        break;}

                        //读取PWM数据
                        case 0x0D:
                        {
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=14;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x0D;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=0x01;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=(CH_A.PWMxA>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=(CH_A.PWMxA)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=(CH_A.PWMxB>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=(CH_A.PWMxB)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=(CH_B.PWMxA>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=(CH_B.PWMxA)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=(CH_B.PWMxB>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[11]=(CH_B.PWMxB)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[12]=0;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[13]=0x0D;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=14;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                        break;}

                        //电流补偿数据
                        case 0x0E:
                        {
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=15;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x0E;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=(Compen_Cur.T_Cali>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=(Compen_Cur.T_Cali)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=(Compen_Cur.T_Env>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=(Compen_Cur.T_Env)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=(Compen_Cur.K_Coeff>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=(Compen_Cur.K_Coeff)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=(CH_A.Cur_Scale_L.B_C_Res>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=(CH_A.Cur_Scale_L.B_C_Res)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[11]=(CH_A.Cur_Scale_L.B_C_Env>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[12]=(CH_A.Cur_Scale_L.B_C_Env)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[13]=(CH_A.Cur_Scale_L.B_Now>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[14]=(CH_A.Cur_Scale_L.B_Now)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[15]=(CH_B.Cur_Scale_L.B_C_Res>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[16]=(CH_B.Cur_Scale_L.B_C_Res)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[17]=(CH_B.Cur_Scale_L.B_C_Env>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[18]=(CH_B.Cur_Scale_L.B_C_Env)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[19]=(CH_B.Cur_Scale_L.B_Now>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[20]=(CH_B.Cur_Scale_L.B_Now)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[21]=0xAA;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[22]=0xAA;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[23]=0x0D;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=24;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                        break;}

                        //电流补偿数据
                        case 0x0F:
                        {
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=18;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x0F;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=(WorkStep_Cycle.Now_Cycle)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Num_Cycle>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Num_Cycle)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Cnt_Cycle>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Cnt_Cycle)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Start_Cycle>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Start_Cycle)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[11]=(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].End_Cycle>>8)&0x00FF;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[12]=(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].End_Cycle)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[13]=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[14]=(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].EndLogic_Cycle)&0x00FF;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[15]=0xAA;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[16]=0xAA;

                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[17]=0x0D;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=18;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                        break;}


                      //写DSP的RAM区
                      case 0x11:
                      {
                      break;}

                      //写外挂FLASH区
                      case 0x12:
                      {
                          if(Num_Byte>117) Num_Byte=117;
                          SPI_Flash_Write(SERV_FLASH_DEBUG,Addr,Num_Byte,(unsigned char *)(&TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[0]+8));
                          //while(Qurey_SPI_Flash_Write(SERV_FLASH_DEBUG)!=TRUE);
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x0B;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x12;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=Addr3;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=Addr2;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=Addr1;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=Addr0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=Num_Byte;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=0xAA;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=0xAA;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=0x0D;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=11;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}
                      //写外挂EEPROM区
                      case 0x13:
                      {
                          if(Num_Byte>4) Num_Byte=4;

                          EEPROM_Write_Num(Addr,Num_Byte,(unsigned char *)(&TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[0]+8));
                          //while(Qurey_EEPROM_Write(SERV_EE_DEBUG)!=TRUE);
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x0B;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x13;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=Addr3;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=Addr2;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=Addr1;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=Addr0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=Num_Byte;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=0xAA;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=0xAA;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=0x0D;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=11;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}

                      //写PWM调试接口数据
                      case 0x14:
                      {
                          CH_A.PWMxA_Debug=((Uint16)TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[5]<<8)+TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[6];
                          CH_A.PWMxB_Debug=((Uint16)TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[7]<<8)+TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[8];
                          CH_B.PWMxA_Debug=((Uint16)TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[9]<<8)+TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[10];
                          CH_B.PWMxB_Debug=((Uint16)TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[11]<<8)+TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[12];

                          Interface_DCDC.EN_Run=TRUE;
                          Interface_DCDC.EN_MOS_Out=TRUE;

                          CH_A.EN_Debug=TRUE;
                          CH_B.EN_Debug=TRUE;

                          if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[3]==1)
                          {Interface_DCDC.Req_Mode=DCDC_DEBUG;}
                          else
                          {Interface_DCDC.Req_Mode=DCDC_STANDBY;}

                          if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[4]==1)
                          {Interface_DCDC.Req_ChangeMode=TRUE;}
                          else
                          {Interface_DCDC.Req_ChangeMode=FALSE;}

                          for(temp=0;temp<16;temp++)
                          {
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[temp]=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[temp];
                          }
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=16;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}

                      case 0x15:
                      {
                          int32 Data0,Data1;
                          Data1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[3];
                          Data0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[4];
                          Simulate_Data7=(Data1<<8)+(Data0);


                          Data1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[5];
                          Data0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[6];
                          Simulate_Data6=(Data1<<8)+(Data0);

                          Data1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[7];
                          Data0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[8];
                          Simulate_Data5=(Data1<<8)+(Data0);

                          Data1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[9];
                          Data0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[10];
                          Simulate_Data4=(Data1<<8)+(Data0);

                          Data1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[11];
                          Data0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[12];
                          Simulate_Data3=(Data1<<8)+(Data0);

                          Data1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[13];
                          Data0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[14];
                          Simulate_Data2=(Data1<<8)+(Data0);

                          Data1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[15];
                          Data0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[16];
                          Simulate_Data1=(Data1<<8)+(Data0);

                          Data1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[17];
                          Data0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[18];
                          Simulate_Data0=(Data1<<8)+(Data0);

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x07;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x81;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x15;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=0x01;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=0x00;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=0x00;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=0x0D;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0];
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}

                      case 0x16:
                      {
                          Uint32 Temp3=0,Temp2=0,Temp1=0,Temp0=0;
                          Temp3=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[3]&0x00FF;
                          Temp2=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[4]&0x00FF;
                          Temp1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[5]&0x00FF;
                          Temp0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[6]&0x00FF;

                          Temp0=(Temp3<<24)+(Temp2<<16)+(Temp1<<8)+Temp0;
                          DCDC_Service.Fault_Module  |= Temp0;

                          Temp3=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[7]&0x00FF;
                          Temp2=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[8]&0x00FF;
                          Temp1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[9]&0x00FF;
                          Temp0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[10]&0x00FF;

                          Temp0=(Temp3<<24)+(Temp2<<16)+(Temp1<<8)+Temp0;
                          Fault_Code.Err_Global  |= Temp0;

                          Temp3=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[11]&0x00FF;
                          Temp2=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[12]&0x00FF;
                          Temp1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[13]&0x00FF;
                          Temp0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[14]&0x00FF;

                          Temp0=(Temp3<<24)+(Temp2<<16)+(Temp1<<8)+Temp0;
                          Fault_Code.Err_Step  |= Temp0;

                          Temp3=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[15]&0x00FF;
                          Temp2=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[16]&0x00FF;
                          Temp1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[17]&0x00FF;
                          Temp0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[18]&0x00FF;

                          Temp0=(Temp3<<24)+(Temp2<<16)+(Temp1<<8)+Temp0;
                          Fault_Code.Alarm  |= Temp0;

                      break;}

                      case 0x17:
                      {
                          if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[3]==0xAA)
                          {
                              Req_DogReset=1;
                          }
                      break;}


                      //同步整流使能控制
                      case 0x18:
                      {
                          if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[3]==0xAA)
                          {
                              Interface_DCDC.EN_CHA_SynRect=TRUE;
                          }
                          else
                          {
                              Interface_DCDC.EN_CHA_SynRect=FALSE;
                          }

                          if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[4]==0xAA)
                          {
                              Interface_DCDC.EN_CHB_SynRect=TRUE;
                          }
                          else
                          {
                              Interface_DCDC.EN_CHB_SynRect=FALSE;
                          }

                      break;}

                      //设置PI参数
                      case 0x1C:
                      {
                          if(DCDC_Service.Now_Mode==DCDC_STANDBY)
                          {

                        	  if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[3]==1)
                        	  {
                        		  if((TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[4]>=1)&&(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[4]<=100))
                        		  {
                        			  CH_A_PI_Cur_Charge.Kp = TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[4];
                        			  CH_B_PI_Cur_Charge.Kp = CH_A_PI_Cur_Charge.Kp;
                        		  }
                        	  }

                        	  if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[5]==2)
                        	  {
                        		  if((TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[6]>=1)&&(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[6]<=32))
                        		  {
                        			  CH_A_PI_Cur_Charge.Ki = TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[6];
                        			  CH_B_PI_Cur_Charge.Ki =CH_A_PI_Cur_Charge.Ki ;
                        		  }
                        	  }

                        	  if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[7]==3)
                        	  {
                        		  if((TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[8]>=1)&&(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[8]<=100))
                        		  {
                        			  CH_A_PI_Cur_Discharge.Kp = TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[8];
                        			  CH_B_PI_Cur_Discharge.Kp =CH_A_PI_Cur_Discharge.Kp;
                        		  }
                        	  }

                        	  if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[9]==4)
                        	  {
                        		  if((TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[10]>=1)&&(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[10]<=32))
                        		  {
                        			  CH_A_PI_Cur_Discharge.Ki = TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[10];
                        			  CH_B_PI_Cur_Discharge.Ki =CH_A_PI_Cur_Discharge.Ki;
                        		  }
                        	  }

                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x07;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x15;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=0x01;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=0x00;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=0x00;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=0x0D;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0];
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                          TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                          }
                          else
                          {
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x07;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x1C;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=0x00;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=0x00;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=0x00;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=0x0D;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0];
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                          }
                      break;}

                      //前馈系数设定
                      case 0x1D:
                      {
                          if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[3]==0xAA)
                          {
                              Uint16 Data0,Data1;
                              Data1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[4];
                              Data0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[5];
                              Data1=(Data1<<8)+(Data0);
                              if(Data1<1024)
                              {
                            	  Bus_Forward.K_Forward=Data1;
                              }
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x07;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x1D;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=(Bus_Forward.K_Forward>>8)&0x00FF;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=(Bus_Forward.K_Forward)&0x00FF;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=0x00;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=0x0D;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0];
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                          }
                       break;}

                      //电流补偿设置
                       case 0x1E:
                          {
                              Uint16 Data0,Data1;
                              if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[3]==0xAA)
                              {
                                  Data1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[4];
                                  Data0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[5];
                                  Data1=(Data1<<8)+(Data0);
                                  Compen_Cur.T_Env=Data1;
                              }

                              if(TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[6]==0xAA)
                              {
                                  Data1=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[7];
                                  Data0=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[8];
                                  Data1=(Data1<<8)+(Data0);
                                  Compen_Cur.K_Coeff=(int16)Data1;
                              }

                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0]=0x0C;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[1]=0x91;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[2]=0x1E;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[3]=(Compen_Cur.T_Cali>>8)&0x00FF;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[4]=(Compen_Cur.T_Cali)&0x00FF;

                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[5]=(Compen_Cur.T_Env>>8)&0x00FF;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[6]=(Compen_Cur.T_Env)&0x00FF;

                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[7]=(Compen_Cur.K_Coeff>>8)&0x00FF;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[8]=(Compen_Cur.K_Coeff)&0x00FF;

                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[9]=0;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[10]=0;

                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[11]=0x0D;

                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=12;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
                              TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;
                      break;}
                  }
                }
            }
        }
        else
        {
            /*INT8U   ubCmdType1 = 0;
            INT16U  uwTcpCmdCrcCheck = 0;
            INT8U   ubTcpCmdCrcTemp0 = 0;
            INT8U   ubTcpCmdCrcTemp1 = 0;
            INT16U  uwTcpipIndex=0;

            uwTcpipIndex=TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[0];

            SPI_Flash_Read(SERV_FLASH_DEBUG,(uwTcpipIndex-1)*0x1000,120);
            while(Qurey_SPI_Flash_Read(SERV_FLASH_DEBUG)==FALSE);

            for(uwTcpipIndex=0;uwTcpipIndex<120;uwTcpipIndex++)
            {
                TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex]=Interface_SPI_Flash[SERV_FLASH_DEBUG].Data[uwTcpipIndex];
                Interface_SPI_Flash[SERV_FLASH_DEBUG].Data[uwTcpipIndex]=0;
            }
            Reset_SPI_FLASH_Service(SERV_FLASH_DEBUG);
           ubCmdType1 = TCP_Socket[SERV_TCP_DEBUG].Data_RX.Data[2];
            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
            uwTcpipIndex = uwTcpipIndex+1;
            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSetProtectACK;
            uwTcpipIndex = uwTcpipIndex+1;
            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCmdType1;
            uwTcpipIndex = uwTcpipIndex+1;
            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cFail;
            uwTcpipIndex = uwTcpipIndex+1;
            //校验位HL
            uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
            ubTcpCmdCrcTemp0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
            ubTcpCmdCrcTemp1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp0;
            uwTcpipIndex = uwTcpipIndex+1;
            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp1;
            uwTcpipIndex = uwTcpipIndex+1;
            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
            uwTcpipIndex = uwTcpipIndex+1;
            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Num_To_TX=128;//uwTcpipIndex;
            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Result_TX=0;
            TCP_Socket[SERV_TCP_DEBUG].Data_TX[CH_TX_RETURN].Req_TX=1;*/
        }
        TCP_Socket[SERV_TCP_DEBUG].Data_RX.Status_Recive=0;
    }
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Init_ID_Debug(void)
{
    ECanaRegs.CANME.all &= (~0x80008000);

    //ID15发送邮箱
    ECanaMboxes.MBOX15.MSGID.all        = 0x98FF50E5;
    ECanaMboxes.MBOX15.MSGCTRL.all      = 0x00000F08;

    //ID31接收邮箱
    ECanaMboxes.MBOX31.MSGID.all  		= 0x98FF50E4;
    ECanaMboxes.MBOX31.MSGCTRL.all      = 0x00000008;

    ECanaRegs.CANME.all |= 0x80008000;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
#define NUM_RECORD 256
//Uint16 Data0[NUM_RECORD];
//Uint16 Data1[NUM_RECORD];
//Uint16 Data2[NUM_RECORD];
//Uint16 Data3[NUM_RECORD];
//Uint16 Flag_Trigger=0;
Uint16 Cnt_Record=0;
Uint16 State_Record_TX=0;
#pragma CODE_SECTION(Record_TX, "ramfuncs");
void Record_TX(void)
{
	static Uint16 Cnt_TX_Period=0;
	static Uint16 Cnt_Send=0;
	static Uint16 Cnt_Record=0;
	static struct ECAN_REGS ECanaShadow1;

	if(DCDC_Service.Mode_Parall==PARALL_SLAVER) return;

	if(State_Record_TX>100)
	{
		//实时发送方式
		if(++Cnt_TX_Period>=3)
		{
			Cnt_TX_Period=0;
			if(DCDC_Service.Now_Mode==DCDC_CC||DCDC_Service.Now_Mode==DCDC_CCtoCV)
			{
                ECanaMboxes.MBOX15.MDL.word.HI_WORD  = DCDC_Service.Now_Vol_Sense;//Image_UpData.Record.dwOutPutCurrentNew_UP>>16;
                ECanaMboxes.MBOX15.MDL.word.LOW_WORD = DCDC_Service.Vol_Sense_LPF;
                ECanaMboxes.MBOX15.MDH.word.HI_WORD  = CH_A_PI_Cur_Charge.Ref>>4;
                ECanaMboxes.MBOX15.MDH.word.LOW_WORD = CH_A.Now_Cur>>4;//Image_UpData.Req_TX;
			}
			else if(DCDC_Service.Now_Mode==DCDC_DCC||DCDC_Service.Now_Mode==DCDC_DCCtoDCV)
			{
                ECanaMboxes.MBOX15.MDL.word.HI_WORD  = DCDC_Service.Now_Vol_Sense;//Image_UpData.Record.dwOutPutCurrentNew_UP>>16;
                ECanaMboxes.MBOX15.MDL.word.LOW_WORD = DCDC_Service.Vol_Sense_LPF;
                ECanaMboxes.MBOX15.MDH.word.HI_WORD  = CH_A_PI_Cur_Discharge.Ref>>4;
                ECanaMboxes.MBOX15.MDH.word.LOW_WORD = CH_A.Now_Cur>>4;//Image_UpData.Req_TX;
			}
			else
			{
                ECanaMboxes.MBOX15.MDL.word.HI_WORD  = DCDC_Service.Now_Vol_Sense;//Image_UpData.Record.dwOutPutCurrentNew_UP>>16;
                ECanaMboxes.MBOX15.MDL.word.LOW_WORD = DCDC_Service.Vol_Sense_LPF;
                ECanaMboxes.MBOX15.MDH.word.HI_WORD  = CH_A_PI_Cur_Charge.Ref>>4;
                ECanaMboxes.MBOX15.MDH.word.LOW_WORD = CH_A.Now_Cur>>4;//Image_UpData.Req_TX;
            }
			ECanaRegs.CANTRS.bit.TRS15 = 1;
		}
		//实时发送方式

		//存储录波方式
		/*if((Flag_Trigger==1)&&(Cnt_Record<NUM_RECORD))
	  	{
			Data0[Cnt_Record]=Cnt_Record;
			Data1[Cnt_Record]=Num_Err;
	  		Data2[Cnt_Record]=Data_AD.AD7606_CH7;
	  		Data3[Cnt_Record]=DCDC_Service.Now_Mode;
	  		Cnt_Record++;
	  	}

	  	if(++Cnt_TX_Period>=5)
	  	{
	  		Cnt_TX_Period=0;

			if(Cnt_Record>=NUM_RECORD)
	 	 	{
		 	 	 if(Flag_First!=0)
		 	 	 {
			 	 	 ECanaShadow1.CANTA.all = ECanaRegs.CANTA.all;
		 	 	 	 if(ECanaShadow1.CANTA.bit.TA15 == 0)
		 				return;  					// 0=没有发送成功
		 	 	 	 else
		 		 	 	ECanaRegs.CANTA.bit.TA15 = 0x1;	// 清除发送成功标志
		 	 	 }

		 	 	 Flag_First=1;

		 	 	 ECanaMboxes.MBOX15.MDL.word.HI_WORD 	= Data0[Cnt_Send];//Cnt_Send;
		 	 	 ECanaMboxes.MBOX15.MDL.word.LOW_WORD  	= Data1[Cnt_Send];
		 	 	 ECanaMboxes.MBOX15.MDH.word.HI_WORD 	= Data2[Cnt_Send];
		 	 	 ECanaMboxes.MBOX15.MDH.word.LOW_WORD  	= Data3[Cnt_Send];

		 	 	 EALLOW;
		 	 	 //set transmit request to start transmit process
		 	 	 ECanaShadow1.CANTRS.all = ECanaRegs.CANTRS.all;
		 	 	 ECanaShadow1.CANTRS.bit.TRS15 = 1;
		 	 	 ECanaRegs.CANTRS.all = ECanaShadow1.CANTRS.all;
		 	 	 EDIS;

		 	 	 if(++Cnt_Send>=NUM_RECORD)
		 	 	 {
		 			Cnt_Send=0;
		 			Cnt_Record=0;
				 	//ECanaShadow1.CANRMP.all= ECanaRegs.CANRMP.all;
				    //if(ECanaShadow1.CANRMP.bit.RMP31==1)
				 	//{
				 	//	if(ECanaMboxes.MBOX31.MDL.word.LOW_WORD<=11)
				 	//	{Record_CH =ECanaMboxes.MBOX11.MDL.word.LOW_WORD;}
				 	//	ECanaRegs.CANRMP.bit.RMP31=1;
				 	//}
		 	 	 }
	 	 	 }
	 	}*/
		//存储录波方式
	}
	else
	{
		if(State_Record_TX==50)
		{
			ECanaShadow1.CANME.all = ECanaRegs.CANME.all;

			ECanaShadow1.CANME.all &= (~0x80008000);

			ECanaRegs.CANME.all=ECanaShadow1.CANME.all;

			//ID15发送邮箱
			ECanaMboxes.MBOX15.MSGID.all        = 0x98FF50E5;
			ECanaMboxes.MBOX15.MSGCTRL.all      = 0x00000F08;

			//ID31接收邮箱
			ECanaMboxes.MBOX31.MSGID.all  		= 0x98FF50E4;
			ECanaMboxes.MBOX31.MSGCTRL.all      = 0x00000008;

			ECanaShadow1.CANME.all |= 0x80008000;

			ECanaRegs.CANME.all=ECanaShadow1.CANME.all;
		}
		State_Record_TX++;
	}
 }

//File end
