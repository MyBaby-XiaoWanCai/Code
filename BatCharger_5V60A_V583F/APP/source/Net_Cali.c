/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Net_Cali.c
// Created on: 2020-4-8
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义


//函数声明


//函数定义

void sAgingCalibrationACKCommand(void)
{
    INT8U   ubCmdType1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcTemp0 = 0;
    INT8U   ubTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpipIndex=0;
    ubCmdType1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cAgingCalibrationSetAck;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCmdType1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cSuccess;
    uwTcpipIndex = uwTcpipIndex+1;
    //校验位HL
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
    ubTcpCmdCrcTemp0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
    ubTcpCmdCrcTemp1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
}
void sAgingCalibrationNACKCommand(void)
{
    INT8U   ubCmdType1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcTemp0 = 0;
    INT8U   ubTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpipIndex=0;
    ubCmdType1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x07;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cAgingCalibrationSetAck;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubCmdType1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cFail;
    uwTcpipIndex = uwTcpipIndex+1;
    //校验位HL
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);
    ubTcpCmdCrcTemp0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
    ubTcpCmdCrcTemp1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
}

void sAgingCalibrationSetCommand(void)
{
    INT8U   ubTcpCmdCnt = 0;
    INT8U   ubSubfunction = 0;
    INT8U   ubDataHigh = 0;
    INT8U   ubDataLow = 0;
    INT16S  uwData = 0;   //原先位INT16U
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    ubSubfunction = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    ubDataHigh = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    ubDataLow = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwData = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwData = uwData<<8;
    uwData = uwData + TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[5];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[6];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcCheck==uwTcpCmdCrcTemp)
    {
        if(ubSubfunction==0x00)//老化模式使能
        {
            if(ubDataLow==0x01)
            {
                if(Interface_DCDC.Req_Aging==FALSE)
                {
                    Interface_DCDC.Req_Aging=TRUE;
                    /*Init_ID_Single();
                    Parall_Master.Cnt_Period=0;
                    Parall_Slaver.Cnt_Period=0;
                    Parall_Slaver.Cnt_S_OT_M=0;
                    Interface_DCDC.Mode_Parall=SINGLE;
                    Interface_DCDC.Req_Mode = DCDC_STANDBY;
                    Interface_DCDC.Parall_Total=1;

                    DCDC_Service.Fault_Module=0;
                    DCDC_Service.Fault_Slaver1=0;
                    DCDC_Service.Fault_Slaver2=0;
                    DCDC_Service.Fault_Slaver3=0;
                    DCDC_Service.Fault_Slaver4=0;
                    Fault_Code.Err_Global=0;
                    Fault_Code.Err_Global_Bak=0;
                    Fault_Code.Err_Step=0;
                    Fault_Code.Err_Step_Bak=0;
                    Fault_Code.Alarm=0;
                    Fault_Code.Alarm_Bak=0;
                    Fault_Code.Cnt_S2M_Fault_Mask=0;

                    DCDC_Service.Cnt_Bus_H              = 0;
                    DCDC_Service.Cnt_Bus_L              = 0;
                    DCDC_Service.Cnt_Fault_AD7606       = 0;
                    CH_A.Cnt_Out_OC                     = 0;
                    CH_B.Cnt_Out_OC                     = 0;

                    PI_Vol_Charge.Limit_Out_max = CUR_MAX_CHANNEL*2*Interface_DCDC.Parall_Total;
                    PI_Vol_Charge.Limit_PI_max  = PI_Vol_Charge.Limit_Out_max<<PI_Vol_Charge.Q_Type;

                    PI_Vol_Discharge.Limit_Out_max = CUR_MAX_CHANNEL*2*Interface_DCDC.Parall_Total;
                    PI_Vol_Discharge.Limit_PI_max  = PI_Vol_Discharge.Limit_Out_max<<PI_Vol_Discharge.Q_Type;
                */
                }
            }
            else
            {
                if(Interface_DCDC.Req_Aging==TRUE)
                {
                    Interface_DCDC.Req_Aging=FALSE;
                    //Parall_Master.Cnt_Delay=0;
                }
            }
           //debug Interface_DCDC.Req_ChangeMode=TRUE;
           sAgingCalibrationACKCommand();
        }
        else if(ubSubfunction==0x0b)//PWM通道使能
        {
            if(ubDataHigh==0x01)
            {
                Interface_DCDC.EN_CHA_Comm=TRUE;
            }
            else
            {
                Interface_DCDC.EN_CHA_Comm=FALSE;
            }
            if(ubDataLow==0x01)
            {
                Interface_DCDC.EN_CHB_Comm=TRUE;
            }
            else
            {
                Interface_DCDC.EN_CHB_Comm=FALSE;
            }
            sAgingCalibrationACKCommand();
        }
        else if(ubSubfunction==0x20)
        {
            if(ubDataLow==0)
            {
                Interface_DCDC.Req_CurRange=CUR_RANGE_L;
            }
            else
            {
                Interface_DCDC.Req_CurRange=CUR_RANGE_H;
            }
            sAgingCalibrationACKCommand();
        }
        else if(ubSubfunction==0x21)
        {
            if((0<=uwData)&&(uwData<=700))
            {
                Compen_Cur.T_Cali = uwData;
                //sEepromWrite(cEepromT_Cali,uwData,2);DELAY_US(5000);
                if(EEPROM_Write_Uint16(cEepromT_Cali,uwData)==TRUE)
                {
                    //EEPROM_Write_Start(SERV_EE_NET);
                    sAgingCalibrationACKCommand();
                }
                else
                {
                    sAgingCalibrationNACKCommand();
                }
            }
            else
            {sAgingCalibrationNACKCommand();}
        }
        else if(ubSubfunction==0x22)
        {
            if((-70<=uwData)&&(uwData<=70))
            {
                Compen_Cur.K_Coeff = uwData;
                //sEepromWrite(cEepromK_Coeff,uwData,2);DELAY_US(5000);
                if(EEPROM_Write_Uint16(cEepromK_Coeff,uwData)==TRUE)
                {
                    //EEPROM_Write_Start(SERV_EE_NET);
                    sAgingCalibrationACKCommand();
                }
                else
                {
                    sAgingCalibrationNACKCommand();
                }
            }
            else
            {sAgingCalibrationNACKCommand();}
        }
        else if(ubSubfunction==0x23)
        {
            if((-600<=uwData)&&(uwData<=600))
            {
                Compen_Cur.Cur_Offset_CH=uwData;
                sAgingCalibrationNACKCommand();
            }
            else
            {sAgingCalibrationNACKCommand();}
        }
        else if(ubSubfunction==0x24)
        {
            if((0<=uwData)&&(uwData<=700))
            {
                Compen_Cur.T_Env = uwData;
                sAgingCalibrationACKCommand();
            }
            else
            {sAgingCalibrationNACKCommand();}
        }
        else if(ubSubfunction==0x25)
        {
            if((-600<=uwData)&&(uwData<=600))
            {
                Compen_Cur.Cur_Offset_DIS=uwData;
                sAgingCalibrationNACKCommand();
            }
            else
            {sAgingCalibrationNACKCommand();}
        }
        else if(ubSubfunction==0x26)
        {
            Cali_PT_5V();
            sAgingCalibrationACKCommand();
        }
        else if(ubSubfunction==0x27)
        {
            EEPROM_Write_Uint16(cEepromStepAging,STEP_AGING_READY);
            Auto_Aging.Step_Aging=STEP_AGING_READY;
            sAgingCalibrationACKCommand();
        }
        else if(ubSubfunction==0x28)
        {

        }
        else if(ubSubfunction==0x29)
        {

        }
        else if(ubSubfunction==0x2A)
        {
            if((10<=uwData)&&(uwData<=400))
            {
                WorkStep_Service.Cur_Limited_Set=((Uint32)uwData)*10000;
                EEPROM_Write_Uint16(cEepromCurLimitedSet,uwData);
                sAgingCalibrationACKCommand();
            }
            else
            {sAgingCalibrationNACKCommand();}
        }
        else if(Interface_DCDC.Req_Aging==FALSE)
        {
            sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x01)//sense的K系数
        {
            if((uwData<=VOL_K_SENSE_MAX)&&(uwData>=VOL_K_SENSE_MIN))
            {
                  //sEepromWrite(cEepromBatSenseVoltAdjK,uwData,2);
                DCDC_Service.Vol_Scale.K_ROM_Sense  = (int32)uwData;
                DCDC_Service.Vol_Scale.K_ROM_Out    = DCDC_Service.Vol_Scale.K_ROM_Sense;

                DCDC_Service.Vol_Scale.K_Now_Sense  = DCDC_Service.Vol_Scale.K_ROM_Sense*10;
                DCDC_Service.Vol_Scale.K_Now_Out    = DCDC_Service.Vol_Scale.K_ROM_Out*10;

                if(EEPROM_Write_Uint16(cEepromBatSenseVoltAdjK,uwData)==TRUE)
                {
                    //EEPROM_Write_Start(SERV_EE_NET);
                    sAgingCalibrationACKCommand();
                }
                else
                {
                    sAgingCalibrationNACKCommand();
                }
            }
            else
            {
                sAgingCalibrationNACKCommand();
            }
        }
        else if(ubSubfunction==0x02)//sense的B系数
        {
            if((VOL_B_SENSE_MIN<=uwData)&&(uwData<=VOL_B_SENSE_MAX))
            {
              //sEepromWrite(cEepromBatSenseVoltAdjb,uwData,2);
              DCDC_Service.Vol_Scale.B_ROM_Sense    = (int32)uwData;
              DCDC_Service.Vol_Scale.B_ROM_Out      = DCDC_Service.Vol_Scale.B_ROM_Sense;
              DCDC_Service.Vol_Scale.B_Now_Sense    = DCDC_Service.Vol_Scale.B_ROM_Sense*10;
              DCDC_Service.Vol_Scale.B_Now_Out      = DCDC_Service.Vol_Scale.B_ROM_Out*10;
              if(EEPROM_Write_Uint16(cEepromBatSenseVoltAdjb,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else
            {sAgingCalibrationNACKCommand();}

        }
        else if(ubSubfunction==0x03)//A通道低档位K系数
        {
            if((CUR_L_K_MIN<=uwData)&&(uwData<=CUR_L_K_MAX))
            {
              //sEepromWrite(cEepromOutPutCurrentLAdjAK,uwData,2);
              CH_A.Cur_Scale_L.K_ROM_CH = (int32)uwData;
              CH_A.Cur_Scale_L.K_Now =CH_A.Cur_Scale_L.K_ROM_CH*10;

              if(EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjAK,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x04)//A通道低档位B系数
        {
            //sSetOutPutCurrentLAdjAb(uwData);
            if((CUR_L_B_MIN<=uwData)&&(uwData<=CUR_L_B_MAX))
            {
              //sEepromWrite(cEepromOutPutCurrentLAdjAb,uwData,2);
              CH_A.Cur_Scale_L.B_ROM_CH = (int32)uwData;
              CH_A.Cur_Scale_L.B_Now =CH_A.Cur_Scale_L.B_ROM_CH*10;

              if(EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjAb,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x05)//A通道高档位K系数
        {
            if((CUR_H_K_MIN<=uwData)&&(uwData<=CUR_H_K_MAX))
            {
              //sEepromWrite(cEepromOutPutCurrentHAdjAK,uwData,2);
              CH_A.Cur_Scale_H.K_ROM_CH = (int32)uwData;
              CH_A.Cur_Scale_H.K_Now =CH_A.Cur_Scale_H.K_ROM_CH*10;

              if(EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjAK,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x06)//A通道高档位B系数
        {

            if((CUR_H_B_MIN<=uwData)&&(uwData<=CUR_H_B_MAX))
            {
               //sEepromWrite(cEepromOutPutCurrentHAdjAb,uwData,2);
               CH_A.Cur_Scale_H.B_ROM_CH = (int32)uwData;
               CH_A.Cur_Scale_H.B_Now =CH_A.Cur_Scale_H.B_ROM_CH*10;

               if(EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjAb,uwData)==TRUE)
               {
                   //EEPROM_Write_Start(SERV_EE_NET);
                   sAgingCalibrationACKCommand();
               }
               else
               {
                   sAgingCalibrationNACKCommand();
               }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x07)//B通道低档位K系数
        {
            if((CUR_L_K_MIN<=uwData)&&(uwData<=CUR_L_K_MAX))
            {
              //sEepromWrite(cEepromOutPutCurrentLAdjBK,uwData,2);DELAY_US(5000);
              CH_B.Cur_Scale_L.K_ROM_CH = (int32)uwData;
              CH_B.Cur_Scale_L.K_Now =CH_B.Cur_Scale_L.K_ROM_CH*10;

              if(EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjBK,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x08)//B通道低档位B系数
        {
            if((CUR_L_B_MIN<=uwData)&&(uwData<=CUR_L_B_MAX))
            {
              //sEepromWrite(cEepromOutPutCurrentLAdjBb,uwData,2);DELAY_US(5000);
              CH_B.Cur_Scale_L.B_ROM_CH = (int32)uwData;
              CH_B.Cur_Scale_L.B_Now =CH_B.Cur_Scale_L.B_ROM_CH*10;

              if(EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjBb,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x09)//B通道高档位K系数
        {
            if((CUR_H_K_MIN<=uwData)&&(uwData<=CUR_H_K_MAX))
            {
              //sEepromWrite(cEepromOutPutCurrentHAdjBK,uwData,2);DELAY_US(5000);
              CH_B.Cur_Scale_H.K_ROM_CH = (int32)uwData;
              CH_B.Cur_Scale_H.K_Now =CH_B.Cur_Scale_H.K_ROM_CH*10;

              if(EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjBK,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x0a)//B通道高档位B系数
        {
            if((CUR_H_B_MIN<=uwData)&&(uwData<=CUR_H_B_MAX))
            {
              //sEepromWrite(cEepromOutPutCurrentHAdjBb,uwData,2);DELAY_US(5000);
              CH_B.Cur_Scale_H.B_ROM_CH = (int32)uwData;
              CH_B.Cur_Scale_H.B_Now =CH_B.Cur_Scale_H.B_ROM_CH*10;

              if(EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjBb,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x0c)//模块PI参数
        {

        }
        else if(ubSubfunction==0x0d)//读取AD值
        {

        }
        else if(ubSubfunction==0x0e)//放电A通道低电流档位参数K
        {
            if((CUR_L_K_MIN<=uwData)&&(uwData<=CUR_L_K_MAX))
            {
              //sEepromWrite(cEepromDischargeAdjLAK,uwData,2);DELAY_US(5000);
              CH_A.Cur_Scale_L.K_ROM_DIS = (int32)uwData;
              CH_A.Cur_Scale_L.K_Now    = CH_A.Cur_Scale_L.K_ROM_DIS*10;
              if(EEPROM_Write_Uint16(cEepromDischargeAdjLAK,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x0f)//放电A通道低电流档位参数B
        {
            if((CUR_L_B_MIN<=uwData)&&(uwData<=CUR_L_B_MAX))
            {
              //sEepromWrite(cEepromDischargeAdjLAB,uwData,2);DELAY_US(5000);
              CH_A.Cur_Scale_L.B_ROM_DIS = (int32)uwData;
              CH_A.Cur_Scale_L.B_Now    = CH_A.Cur_Scale_L.B_ROM_DIS*10;
              if(EEPROM_Write_Uint16(cEepromDischargeAdjLAB,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x10)//放电A通道高电流档位参数K
        {
            if((CUR_H_K_MIN<=uwData)&&(uwData<=CUR_H_K_MAX))
            {
              //sEepromWrite(cEepromDischargeAdjHAK,uwData,2);DELAY_US(5000);
              CH_A.Cur_Scale_H.K_ROM_DIS = (int32)uwData;
              CH_A.Cur_Scale_H.K_Now    = CH_A.Cur_Scale_H.K_ROM_DIS*10;
              if(EEPROM_Write_Uint16(cEepromDischargeAdjHAK,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x11)//放电A通道高电流档位参数B
        {
            if((CUR_H_B_MIN<=uwData)&&(uwData<=CUR_H_B_MAX))
            {
              //sEepromWrite(cEepromDischargeAdjHAB,uwData,2);DELAY_US(5000);
              CH_A.Cur_Scale_H.B_ROM_DIS = (int32)uwData;
              CH_A.Cur_Scale_H.B_Now    = CH_A.Cur_Scale_H.B_ROM_DIS*10;
              if(EEPROM_Write_Uint16(cEepromDischargeAdjHAB,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x12)//放电B通道低电流档位参数K
        {
            if((CUR_L_K_MIN<=uwData)&&(uwData<=CUR_L_K_MAX))
            {
              //sEepromWrite(cEepromDischargeAdjLBK,uwData,2);DELAY_US(5000);
              CH_B.Cur_Scale_L.K_ROM_DIS = (int32)uwData;
              CH_B.Cur_Scale_L.K_Now    = CH_B.Cur_Scale_L.K_ROM_DIS*10;
              if(EEPROM_Write_Uint16(cEepromDischargeAdjLBK,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x13)//放电B通道低电流档位参数B
        {
            if((CUR_L_B_MIN<=uwData)&&(uwData<=CUR_L_B_MAX))
            {
              //sEepromWrite(cEepromDischargeAdjLBB,uwData,2);DELAY_US(5000);
              CH_B.Cur_Scale_L.B_ROM_DIS = (int32)uwData;
              CH_B.Cur_Scale_L.B_Now    = CH_B.Cur_Scale_L.B_ROM_DIS*10;
              if(EEPROM_Write_Uint16(cEepromDischargeAdjLBB,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x14)//放电B通道高电流档位参数K
        {
            if((CUR_H_K_MIN<=uwData)&&(uwData<=CUR_H_K_MAX))
            {
              //sEepromWrite(cEepromDischargeAdjHBK,uwData,2);DELAY_US(5000);
              CH_B.Cur_Scale_H.K_ROM_DIS = (int32)uwData;
              CH_B.Cur_Scale_H.K_Now    = CH_B.Cur_Scale_H.K_ROM_DIS*10;
              if(EEPROM_Write_Uint16(cEepromDischargeAdjHBK,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }
        else if(ubSubfunction==0x15)//放电B通道高电流档位参数B
        {
            if((CUR_H_B_MIN<=uwData)&&(uwData<=CUR_H_B_MAX))
            {
              //sEepromWrite(cEepromDischargeAdjHBB,uwData,2);DELAY_US(5000);
              CH_B.Cur_Scale_H.B_ROM_DIS = (int32)uwData;
              CH_B.Cur_Scale_H.B_Now    = CH_B.Cur_Scale_H.B_ROM_DIS*10;
              if(EEPROM_Write_Uint16(cEepromDischargeAdjHBB,uwData)==TRUE)
              {
                  //EEPROM_Write_Start(SERV_EE_NET);
                  sAgingCalibrationACKCommand();
              }
              else
              {
                  sAgingCalibrationNACKCommand();
              }
            }
            else sAgingCalibrationNACKCommand();
        }


    }
    else
    {
        sAgingCalibrationNACKCommand();
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}

void sReplyAgingCalibrationParama(INT16U uwTemp,INT8U ubSubfunction)
{
    INT8U ubTempH = 0;
    INT8U ubTempL = 0;
    INT8U  ubTcpCmdCrcCheck0 = 0;
    INT8U  ubTcpCmdCrcCheck1 = 0;
    INT16U uwTcpCmdCrcCheck = 0;
    INT16U  uwTcpipIndex=0;

    ubTempH = (INT8U)((uwTemp&0xFF00)>>8);
    ubTempL = (INT8U)((uwTemp&0x00FF));
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x08;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cAgingCalibrationInqureAck;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSubfunction;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTempH;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTempL;
    uwTcpipIndex = uwTcpipIndex+1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);//校验位HL
    ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
    ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
    uwTcpipIndex = uwTcpipIndex+1;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
}
//---------------------------------------------------//

void sReplyADData(INT32S Data,INT8U ubSubfunction)
{
    INT8U  ubTcpCmdCrcCheck0 = 0;
    INT8U  ubTcpCmdCrcCheck1 = 0;
    INT16U uwTcpCmdCrcCheck = 0;
    INT16U  uwTcpipIndex=0;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = 0x0A;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cAgingCalibrationInqureAck;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubSubfunction;
    uwTcpipIndex = uwTcpipIndex+1;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = (INT8U)((Data&0xFF000000)>>24);//HWK0827
    uwTcpipIndex = uwTcpipIndex+1;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] =(INT8U)((Data&0x00FF0000)>>16);
    uwTcpipIndex = uwTcpipIndex+1;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] =(INT8U)((Data&0x0000FF00)>>8);
    uwTcpipIndex = uwTcpipIndex+1;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = (INT8U)(Data&0x000000FF);
    uwTcpipIndex = uwTcpipIndex+1;

    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[0],uwTcpipIndex);//校验位HL
    ubTcpCmdCrcCheck0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
    ubTcpCmdCrcCheck1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = ubTcpCmdCrcCheck1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Data[uwTcpipIndex] = cEnd;
    uwTcpipIndex = uwTcpipIndex+1;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Num_To_TX=uwTcpipIndex;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Result_TX=0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_RETURN].Req_TX=1;
}



void sAgingCalibrationSetInqureCommand(void)
{
    INT16U  TempDada = 0;
    INT16S  TempDada1=0;
    INT8U   ubdata = 0;
    INT16U  uwReplyData = 0;
    INT8U   ubTcpCmdCnt = 0;
    INT8U   ubSubfunction = 0;
    INT16U  uwTcpCmdCrcTemp = 0;
    INT16U  uwTcpCmdCrcTemp0 = 0;
    INT16U  uwTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpCmdCrcCheck = 0;
    ubTcpCmdCnt = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0];
    ubSubfunction = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[2];
    uwTcpCmdCrcTemp0 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[3];
    uwTcpCmdCrcTemp1 = TCP_Socket[SERV_TCP_WORK].Data_RX.Data[4];
    uwTcpCmdCrcTemp = (uwTcpCmdCrcTemp0<<8)+uwTcpCmdCrcTemp1;
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_RX.Data[0],ubTcpCmdCnt-3);
    if(uwTcpCmdCrcTemp==uwTcpCmdCrcCheck)
    {
        if(ubSubfunction==0x00)
        {
            sReplyAgingCalibrationParama(Interface_DCDC.Req_Aging,0x00);
        }
        else if(ubSubfunction==0x01)
        {
            //sReplyAgingCalibrationParama(sGetBatSenseVoltAdjK(),0x01);
            TempDada = EEPROM_Read_Uint16(cEepromBatSenseVoltAdjK);//DELAY_US(5000);
            DCDC_Service.Vol_Scale.K_ROM_Sense  = (int32)TempDada;
            DCDC_Service.Vol_Scale.K_ROM_Out    = DCDC_Service.Vol_Scale.K_ROM_Sense;

            DCDC_Service.Vol_Scale.K_Now_Sense  = DCDC_Service.Vol_Scale.K_ROM_Sense*10;
            DCDC_Service.Vol_Scale.K_Now_Out    = DCDC_Service.Vol_Scale.K_ROM_Out*10;
            sReplyAgingCalibrationParama(TempDada,0x01);
        }
        else if(ubSubfunction==0x02)
        {
            //sReplyAgingCalibrationParama(sGetBatSenseVoltAdjb(),0x02);
            TempDada1 = EEPROM_Read_Uint16(cEepromBatSenseVoltAdjb);//DELAY_US(5000);
            DCDC_Service.Vol_Scale.B_ROM_Sense  = (int32)TempDada1;
            DCDC_Service.Vol_Scale.B_ROM_Out    = DCDC_Service.Vol_Scale.B_ROM_Sense;

            DCDC_Service.Vol_Scale.B_Now_Sense  = DCDC_Service.Vol_Scale.B_ROM_Sense*10;
            DCDC_Service.Vol_Scale.B_Now_Out    = DCDC_Service.Vol_Scale.B_ROM_Out*10;
            sReplyAgingCalibrationParama(TempDada1,0x02);
        }
        else if(ubSubfunction==0x03)//充电A通道低电流档位参数K
        {
            //sReplyAgingCalibrationParama(PosAdjK[0],0x03);
            TempDada = EEPROM_Read_Uint16(cEepromOutPutCurrentLAdjAK);//DELAY_US(5000);

            CH_A.Cur_Scale_L.K_ROM_CH = (int32)TempDada;
            CH_A.Cur_Scale_L.K_Now =CH_A.Cur_Scale_L.K_ROM_CH*10;

            sReplyAgingCalibrationParama(TempDada,0x03);
        }
        else if(ubSubfunction==0x04)//充电A通道低电流档位参数B
        {
            //sReplyAgingCalibrationParama(PosAdjB[0],0x04);
            TempDada1 = EEPROM_Read_Uint16(cEepromOutPutCurrentLAdjAb);//DELAY_US(5000);

            CH_A.Cur_Scale_L.B_ROM_CH = (int32)TempDada1;
            CH_A.Cur_Scale_L.B_Now =CH_A.Cur_Scale_L.B_ROM_CH*10;

            sReplyAgingCalibrationParama(TempDada1,0x04);
        }
        else if(ubSubfunction==0x05)//充电A通道高电流档位参数K
        {
            //sReplyAgingCalibrationParama(PosAdjK[2],0x05);
            TempDada = EEPROM_Read_Uint16(cEepromOutPutCurrentHAdjAK);//DELAY_US(5000);

            CH_A.Cur_Scale_H.K_ROM_CH = (int32)TempDada;
            CH_A.Cur_Scale_H.K_Now =CH_A.Cur_Scale_H.K_ROM_CH*10;

            sReplyAgingCalibrationParama(TempDada,0x05);
        }
        else if(ubSubfunction==0x06)//充电A通道高电流档位参数B
        {
            //sReplyAgingCalibrationParama(PosAdjB[2],0x06);
            TempDada1 = EEPROM_Read_Uint16(cEepromOutPutCurrentHAdjAb);//DELAY_US(5000);


            CH_A.Cur_Scale_H.B_ROM_CH = (int32)TempDada1;
            CH_A.Cur_Scale_H.B_Now =CH_A.Cur_Scale_H.B_ROM_CH*10;


            sReplyAgingCalibrationParama(TempDada1,0x06);
        }
        else if(ubSubfunction==0x07)//充电B通道低电流档位参数K
        {
            //sReplyAgingCalibrationParama(PosAdjK[1],0x07);
            TempDada = EEPROM_Read_Uint16(cEepromOutPutCurrentLAdjBK);//DELAY_US(5000);


            CH_B.Cur_Scale_L.K_ROM_CH = (int32)TempDada;
            CH_B.Cur_Scale_L.K_Now =CH_B.Cur_Scale_L.K_ROM_CH*10;

            sReplyAgingCalibrationParama(TempDada,0x07);
        }
        else if(ubSubfunction==0x08)//充电B通道低电流档位参数B
        {
            //sReplyAgingCalibrationParama(PosAdjB[1],0x08);
            TempDada1 = EEPROM_Read_Uint16(cEepromOutPutCurrentLAdjBb);//DELAY_US(5000);


            CH_B.Cur_Scale_L.B_ROM_CH = (int32)TempDada1;
            CH_B.Cur_Scale_L.B_Now =CH_B.Cur_Scale_L.B_ROM_CH*10;

            sReplyAgingCalibrationParama(TempDada1,0x08);
        }
        else if(ubSubfunction==0x09)//充电B通道高电流档位参数K
        {
            //sReplyAgingCalibrationParama(PosAdjK[3],0x09);
            TempDada = EEPROM_Read_Uint16(cEepromOutPutCurrentHAdjBK);//DELAY_US(5000);


            CH_B.Cur_Scale_H.K_ROM_CH = (int32)TempDada;
            CH_B.Cur_Scale_H.K_Now =CH_B.Cur_Scale_H.K_ROM_CH*10;

            sReplyAgingCalibrationParama(TempDada,0x09);
        }
        else if(ubSubfunction==0x0A)//充电B通道高电流档位参数B
        {
            //sReplyAgingCalibrationParama(PosAdjB[3],0x0a);
            TempDada1 = EEPROM_Read_Uint16(cEepromOutPutCurrentHAdjBb);//DELAY_US(5000);


            CH_B.Cur_Scale_H.B_ROM_CH = (int32)TempDada1;
            CH_B.Cur_Scale_H.B_Now =CH_B.Cur_Scale_H.B_ROM_CH*10;



            sReplyAgingCalibrationParama(TempDada1,0x0a);
        }
        else if(ubSubfunction==0x0B)//PWM使能
        {
            uwReplyData = Interface_DCDC.EN_CHA_Comm;
            uwReplyData = uwReplyData <<8;
            uwReplyData = uwReplyData + Interface_DCDC.EN_CHB_Comm;
            sReplyAgingCalibrationParama(uwReplyData,0x0b);
        }
        else if(ubSubfunction==0x0C)//PI查询
        {

        }
        else if(ubSubfunction==0x0D)
        {

        }
        else if(ubSubfunction==0x0E)//放电A通道低电流档位参数K
        {
            //sReplyAgingCalibrationParama(NegAdjK[0],0x0e);
            TempDada = EEPROM_Read_Uint16(cEepromDischargeAdjLAK);//DELAY_US(5000);

            CH_A.Cur_Scale_L.K_ROM_DIS = (int32)TempDada;
            CH_A.Cur_Scale_L.K_Now =CH_A.Cur_Scale_L.K_ROM_DIS*10;

            sReplyAgingCalibrationParama(TempDada,0x0e);
        }
        else if(ubSubfunction==0x0F)//放电A通道低电流档位参数B
        {
            //sReplyAgingCalibrationParama(NegAdjB[0],0x0f);
            TempDada1 = EEPROM_Read_Uint16(cEepromDischargeAdjLAB);//DELAY_US(5000);


            CH_A.Cur_Scale_L.B_ROM_DIS = (int32)TempDada1;
            CH_A.Cur_Scale_L.B_Now =CH_A.Cur_Scale_L.B_ROM_DIS*10;

            sReplyAgingCalibrationParama(TempDada1,0x0f);
        }
        else if(ubSubfunction==0x10)//放电A通道高电流档位参数K
        {
            //sReplyAgingCalibrationParama(NegAdjK[2],0x10);
            TempDada = EEPROM_Read_Uint16(cEepromDischargeAdjHAK);//DELAY_US(5000);

            CH_A.Cur_Scale_H.K_ROM_DIS = (int32)TempDada;
            CH_A.Cur_Scale_H.K_Now =CH_A.Cur_Scale_H.K_ROM_DIS*10;

            sReplyAgingCalibrationParama(TempDada,0x10);
        }
        else if(ubSubfunction==0x11)//放电A通道高电流档位参数B
        {
            //sReplyAgingCalibrationParama(NegAdjB[2],0x11);

            TempDada1 = EEPROM_Read_Uint16(cEepromDischargeAdjHAB);//DELAY_US(5000);


            CH_A.Cur_Scale_H.B_ROM_DIS = (int32)TempDada1;
            CH_A.Cur_Scale_H.B_Now =CH_A.Cur_Scale_H.B_ROM_DIS*10;

            sReplyAgingCalibrationParama(TempDada1,0x11);
        }
        else if(ubSubfunction==0x12)//放电B通道低电流档位参数K
        {
            //sReplyAgingCalibrationParama(NegAdjK[1],0x12);
            TempDada = EEPROM_Read_Uint16(cEepromDischargeAdjLBK);//DELAY_US(5000);

            CH_B.Cur_Scale_L.K_ROM_DIS = (int32)TempDada;
            CH_B.Cur_Scale_L.K_Now =CH_B.Cur_Scale_L.K_ROM_DIS*10;

            sReplyAgingCalibrationParama(TempDada,0x12);
        }
        else if(ubSubfunction==0x13)//放电B通道低电流档位参数B
        {
            //sReplyAgingCalibrationParama(NegAdjB[1],0x13);
            TempDada1 = EEPROM_Read_Uint16(cEepromDischargeAdjLBB);//DELAY_US(5000);

            CH_B.Cur_Scale_L.B_ROM_DIS = (int32)TempDada1;
            CH_B.Cur_Scale_L.B_Now =CH_B.Cur_Scale_L.B_ROM_DIS*10;

            sReplyAgingCalibrationParama(TempDada1,0x13);
        }
        else if(ubSubfunction==0x14)//放电B通道高电流档位参数K
        {
            //sReplyAgingCalibrationParama(NegAdjK[3],0x14);
            TempDada = EEPROM_Read_Uint16(cEepromDischargeAdjHBK);//DELAY_US(5000);

            CH_B.Cur_Scale_H.K_ROM_DIS = (int32)TempDada;
            CH_B.Cur_Scale_H.K_Now =CH_B.Cur_Scale_H.K_ROM_DIS*10;

            sReplyAgingCalibrationParama(TempDada,0x14);
        }
        else if(ubSubfunction==0x15)//放电B通道高电流档位参数B
        {
            //sReplyAgingCalibrationParama(NegAdjB[3],0x15);
            TempDada1 = EEPROM_Read_Uint16(cEepromDischargeAdjHBB);//DELAY_US(5000);

            CH_B.Cur_Scale_H.B_ROM_DIS = (int32)TempDada1;
            CH_B.Cur_Scale_H.B_Now =CH_B.Cur_Scale_H.B_ROM_DIS*10;

            sReplyAgingCalibrationParama(TempDada1,0x15);
        }
        else if(ubSubfunction==0x16)//SenseA
        {
            sReplyADData(Data_AD_Sum.Sum_Vol_Sense_AD_Copy,0x16);
        }
        else if(ubSubfunction==0x17)//V_Output
        {
            sReplyADData(Data_AD_Sum.Sum_Vol_Out_AD_Copy,0x17);
        }
        else if(ubSubfunction==0x18)//CH_A
        {
            sReplyADData(Data_AD_Sum.Sum_Cur_CHA_AD_Copy,0x18);
        }
        else if(ubSubfunction==0x19)//CH_B
        {
            sReplyADData(Data_AD_Sum.Sum_Cur_CHB_AD_Copy,0x19);
        }
        else if(ubSubfunction==0x1A)//20190920  temperature NTC1
        {
            sReplyADData(Data_Sum.Temp_NTC1,0x1A);
        }
        else if(ubSubfunction==0x20)
        {
            if(DCDC_Service.Flag_CurRange==CUR_RANGE_H)
            {ubdata = 1;}//sGetCD4053Status();
            else
            {ubdata = 0;}
            uwReplyData = (INT16U)ubdata;
            sReplyAgingCalibrationParama(uwReplyData,0x20);
        }
        else if(ubSubfunction==0x21)
        {
            TempDada = EEPROM_Read_Uint16(cEepromT_Cali);//DELAY_US(5000);
            sReplyAgingCalibrationParama(TempDada,0x21);
        }
        else if(ubSubfunction==0x22)
        {
            TempDada1 = EEPROM_Read_Uint16(cEepromK_Coeff);//DELAY_US(5000);
            sReplyAgingCalibrationParama(TempDada1,0x22);
        }
        else if(ubSubfunction==0x23)
        {
            sReplyAgingCalibrationParama(Compen_Cur.Cur_Offset_CH,0x23);
        }
        else if(ubSubfunction==0x24)
        {
            sReplyAgingCalibrationParama(Compen_Cur.T_Env,0x24);
        }
        else if(ubSubfunction==0x25)
        {
            sReplyAgingCalibrationParama(Compen_Cur.Cur_Offset_DIS,0x25);
        }
        else if(ubSubfunction==0x26)
        {
            //sReplyAgingCalibrationParama(Delta_K1*10,0x26);
        }
        else if(ubSubfunction==0x27)
        {
            Auto_Aging.Step_Aging=EEPROM_Read_Uint16(cEepromStepAging);
            sReplyAgingCalibrationParama(Auto_Aging.Step_Aging,0x27);
        }
        else if(ubSubfunction==0x28)
        {
            //sReplyAgingCalibrationParama(Delta_K3*10,0x28);sGetSciAdjustModeEn
        }
        else if(ubSubfunction==0x29)
        {
            //sReplyAgingCalibrationParama(Delta_K4*10,0x29);
        }
        else if(ubSubfunction==0x2A)
        {
            Uint32 Data=EEPROM_Read_Uint16(cEepromCurLimitedSet);
            if(Data<10)
            {
                Data=10;
            }

            if(Data>400)
            {
                Data=400;
            }
            WorkStep_Service.Cur_Limited_Set=Data*10000;
            sReplyAgingCalibrationParama(Data,0x2A);
        }
    }
    TCP_Socket[SERV_TCP_WORK].Data_RX.Status_Recive=0;
}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
int32 Cur_Scale_L_K_Check(int32 Data)
{
   if((Data>=CUR_L_K_MIN)&&(Data<=CUR_L_K_MAX))
   {
       return(Data*10);
   }
   else
   {
       //置校准数据错误标志
	   DINT;
       DCDC_Service.Fault_Module |= FAULT_DATA_CALI;
       EINT;
       return((int32)CUR_L_K_DEFAULT*10);
   }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
int32 Cur_Scale_L_B_Check(int32 Data)
{
   if((Data>=CUR_L_B_MIN)&&(Data<=CUR_L_B_MAX))
   {
       return(Data*10);
   }
   else
   {
       //置校准数据错误标志
	   DINT;
       DCDC_Service.Fault_Module |= FAULT_DATA_CALI;
       EINT;
       return((int32)CUR_L_B_DEFAULT*10);
   }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
int32 Cur_Scale_H_K_Check(int32 Data)
{
   if((Data>=CUR_H_K_MIN)&&(Data<=CUR_H_K_MAX))
   {
       return(Data*10);
   }
   else
   {
       //置校准数据错误标志
	   DINT;
       DCDC_Service.Fault_Module |= FAULT_DATA_CALI;
       EINT;
       return((int32)CUR_H_K_DEFAULT*10);
   }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
int32 Cur_Scale_H_B_Check(int32 Data)
{
   if((Data>=CUR_H_B_MIN)&&(Data<=CUR_H_B_MAX))
   {
       return(Data*10);
   }
   else
   {
       //置校准数据错误标志
	   DINT;
       DCDC_Service.Fault_Module |= FAULT_DATA_CALI;
       EINT;
       return((int32)CUR_H_B_DEFAULT*10);
   }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
int32 Vol_Scale_K_Sense_Check(int32 Data)
{
   if((Data>=VOL_K_SENSE_MIN)&&(Data<=VOL_K_SENSE_MAX))
   {
       return(Data*10);
   }
   else
   {
       //置校准数据错误标志
	   DINT;
       DCDC_Service.Fault_Module |= FAULT_DATA_CALI;
       EINT;
       return((int32)VOL_K_SENSE_DEFAULT*10);
   }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
int32 Vol_Scale_B_Sense_Check(int32 Data)
{
   if((Data>=VOL_B_SENSE_MIN)&&(Data<=VOL_B_SENSE_MAX))
   {
       return(Data*10);
   }
   else
   {
       //置校准数据错误标志
	   DINT;
       DCDC_Service.Fault_Module |= FAULT_DATA_CALI;
       EINT;
       return((int32)VOL_B_SENSE_DEFAULT*10);
   }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
int32 Vol_Scale_K_Out_Check(int32 Data)
{
   if((Data>=VOL_K_OUT_MIN)&&(Data<=VOL_K_OUT_MAX))
   {
       return(Data*10);
   }
   else
   {
       //置校准数据错误标志
	   DINT;
       DCDC_Service.Fault_Module |= FAULT_DATA_CALI;
       EINT;
       return((int32)VOL_K_OUT_DEFAULT*10);
   }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
int32 Vol_Scale_B_Out_Check(int32 Data)
{
   if((Data>=VOL_B_OUT_MIN)&&(Data<=VOL_B_OUT_MAX))
   {
       return(Data*10);
   }
   else
   {
       //置校准数据错误标志
	   DINT;
       DCDC_Service.Fault_Module |= FAULT_DATA_CALI;
       EINT;
       return((int32)VOL_B_OUT_DEFAULT*10);
   }
}

//File end
