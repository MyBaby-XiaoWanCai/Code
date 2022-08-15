/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: EEPROM_BSP.c
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义
     INTERFACE_I2C Interface_I2C[NUM_SERVICE_I2C];         //有读写EEPROM需求的模块根据需要可以分配1个或多个读写服务
STATUS_I2C_SERVICE I2C_Service;                            //I2C服务状态机内部用变量

//函数声明


//函数定义
//*********************************************************************************
//Description: I2CA初始化
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void I2CA_Init(void)
{
   // Initialize I2C
   I2caRegs.I2CPSC.all              = 9;            // 10MHz Prescaler - need 7-12 Mhz on module clk
   I2caRegs.I2CCLKL                 = 40;           // NOTE: must be non zero
   I2caRegs.I2CCLKH                 = 30;           // NOTE: must be non zero
   I2caRegs.I2CMDR.all              = 0x0020;       // Take I2C out of reset

   // Enable FIFO mode and TXFIFO
   I2caRegs.I2CFFTX.bit.I2CFFEN     =1;
   I2caRegs.I2CFFTX.bit.TXFFIENA    =0;
   I2caRegs.I2CFFTX.bit.TXFFRST     =1;
   I2caRegs.I2CFFTX.bit.TXFFIL      =0;

   // Enable RXFIFO, clear RXFFINT,
   I2caRegs.I2CFFRX.bit.RXFFIENA    =0;
   I2caRegs.I2CFFRX.bit.RXFFRST     =1;
   I2caRegs.I2CFFRX.bit.RXFFIL      =0;

   I2caRegs.I2CMDR.all              = 0x0000;       // Take I2C out of reset
   DSP28x_usDelay(20);
   I2caRegs.I2CMDR.all              = 0x0020;       // Take I2C out of reset
   DSP28x_usDelay(20);

}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
unsigned char EEPROM_Read_Char(Uint16 Addr)
{
    unsigned char ubRdata;

    while((I2caRegs.I2CSTR.bit.BB == 1)||(I2caRegs.I2CMDR.bit.STP==1));
    I2caRegs.I2CSAR = (ADDR_24C08 |(Addr>>8));
    I2caRegs.I2CCNT = 1;
    I2caRegs.I2CDXR = (unsigned char)Addr;
    I2caRegs.I2CMDR.all = 0x2620;
    while(I2caRegs.I2CSTR.bit.ARDY==0){};
    I2caRegs.I2CCNT = 1;
    I2caRegs.I2CMDR.all = 0x2C20;
    while(I2caRegs.I2CMDR.bit.STP == 1)
    {   DELAY_US(2);    }
    ubRdata = I2caRegs.I2CDRR;
    DELAY_US(200);
    return ubRdata;
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
Uint16 EEPROM_Read_Uint16(Uint16 Addr)
{
    Uint16 ubRdata;
    ubRdata  = EEPROM_Read_Char(Addr);
    ubRdata  = ubRdata<<8;
    ubRdata |= EEPROM_Read_Char(Addr+1);
    return ubRdata;
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
Uint16 EEPROM_Read_Num(Uint16 Addr,Uint16 Num,unsigned char *Ptr_Data)
{
    Uint16 Cnt;
    for(Cnt=0;Cnt<Num;Cnt++)
    {
        if(Addr>cMaxAddrSize)
        {
            return FALSE;
        }
        *Ptr_Data++=EEPROM_Read_Char(Addr++);
    }
    return TRUE;
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
Uint16 EEPROM_Write_Char(Uint16 Addr,unsigned char Data)
{
    if(Addr>cMaxAddrSize)
    {
        return FALSE;
    }
    while(I2caRegs.I2CSTR.bit.BB == 1){};

    I2caRegs.I2CSAR =  (ADDR_24C08 |(Addr>>8));
    I2caRegs.I2CCNT = 2;
    I2caRegs.I2CDXR = (unsigned char)Addr;
    I2caRegs.I2CDXR = (unsigned char)Data;

    I2caRegs.I2CMDR.all = 0x2E20;
    DELAY_US(5000);
    return TRUE;
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
Uint16 EEPROM_Write_Uint16(Uint16 Addr,Uint16 Data)
{
    if(Addr>cMaxAddrSize)
    {
        return FALSE;
    }
    EEPROM_Write_Char(Addr,(unsigned char)(Data>>8));
    EEPROM_Write_Char(Addr+1,(unsigned char)(Data&0x00FF));
    return TRUE;
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
Uint16 EEPROM_Write_Num(Uint16 Addr,Uint16 Num,unsigned char *Ptr_Data)
{
    Uint16 Cnt;
    for(Cnt=0;Cnt<Num;Cnt++)
    {
        if(Addr>cMaxAddrSize)
        {
            return FALSE;
        }
        EEPROM_Write_Char(Addr++,*Ptr_Data++);
    }
    return TRUE;
}

//*********************************************************************************
//Description: I2C初始化
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void InitI2C_BSP(void)
{
    Uint16 Cnt=0;
    Uint32 temp1,temp2;
    InitI2CGpio();
    I2CA_Init();

    temp1=(Uint32)&Interface_I2C;
    temp2=sizeof(INTERFACE_I2C);

    //I2C服务接口数据结构初始化
    for(Cnt=0;Cnt<NUM_SERVICE_I2C;Cnt++)
    {
        Interface_I2C[Cnt].Addr_Device  = ADDR_24C08;
        Interface_I2C[Cnt].Len_Buff     = LEN_DATA_OBJ_I2C;
        Interface_I2C[Cnt].Num_Overtime = 1000;
        Interface_I2C[Cnt].Num_Bytes    = 0;
        Interface_I2C[Cnt].Req_RW       = REQ_I2C_FREE;
        Interface_I2C[Cnt].Result_RW    = I2C_FREE;

        if(Cnt==NUM_SERVICE_I2C-1)
        {Interface_I2C[Cnt].Ptr_Next    = (INTERFACE_I2C *)temp1;}
        else
        {Interface_I2C[Cnt].Ptr_Next    = (INTERFACE_I2C *)(temp1+(Cnt+1)*temp2);}
    }

    I2C_Service.Cnt_Bytes     = 0;
    I2C_Service.Cnt_Error     = 0;
    I2C_Service.Cnt_Overtime  = 0;
    I2C_Service.Ptr           = (INTERFACE_I2C *)temp1;
    I2C_Service.State         = 0;
}

void Read_Para_Net(void)
{
    W5500_Service.GAR[0]    = EEPROM_Read_Uint16(cEepromGateway_IP0);
    W5500_Service.GAR[1]    = EEPROM_Read_Uint16(cEepromGateway_IP1);
    W5500_Service.GAR[2]    = EEPROM_Read_Uint16(cEepromGateway_IP2);
    W5500_Service.GAR[3]    = EEPROM_Read_Uint16(cEepromGateway_IP3);

    W5500_Service.SUBR[0]    = EEPROM_Read_Uint16(cEepromSub_Mask0);
    W5500_Service.SUBR[1]    = EEPROM_Read_Uint16(cEepromSub_Mask1);
    W5500_Service.SUBR[2]    = EEPROM_Read_Uint16(cEepromSub_Mask2);
    W5500_Service.SUBR[3]    = EEPROM_Read_Uint16(cEepromSub_Mask3);

    W5500_Service.SHAR[0]    = EEPROM_Read_Uint16(cEepromPhy_Addr0);
    W5500_Service.SHAR[1]    = EEPROM_Read_Uint16(cEepromPhy_Addr1);
    W5500_Service.SHAR[2]    = EEPROM_Read_Uint16(cEepromPhy_Addr2);
    W5500_Service.SHAR[3]    = EEPROM_Read_Uint16(cEepromPhy_Addr3);
    W5500_Service.SHAR[4]    = EEPROM_Read_Uint16(cEepromPhy_Addr4);
    W5500_Service.SHAR[5]    = EEPROM_Read_Uint16(cEepromPhy_Addr5);

    W5500_Service.SIPR[0]    = EEPROM_Read_Uint16(cEepromIP_Addr0);
    W5500_Service.SIPR[1]    = EEPROM_Read_Uint16(cEepromIP_Addr1);
    W5500_Service.SIPR[2]    = EEPROM_Read_Uint16(cEepromIP_Addr2);
    W5500_Service.SIPR[3]    = EEPROM_Read_Uint16(cEepromIP_Addr3);

    TCP_Socket[SERV_TCP_WORK].Port_Source= (EEPROM_Read_Uint16(cEepromS0_Port0)<<8)+EEPROM_Read_Uint16(cEepromS0_Port1);
    TCP_Socket[SERV_TCP_DEBUG].Port_Source= TCP_Socket[SERV_TCP_WORK].Port_Source+1;
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Read_Data_PowerFail(void)
{
    WorkStep_PowerFail.Step_State_EEPROM   = EEPROM_Read_Uint16(cEepromStepComplete);
    WorkStep_PowerFail.Step_Index_EEPROM   = EEPROM_Read_Uint16(cEepromPowerOffStep);
    WorkStep_PowerFail.Step_Time_EEPROM    = (Uint32)EEPROM_Read_Uint16(cEepromPowerOfftime)*1000;
    WorkStep_PowerFail.Step_Ah_EEPROM      = (Uint32)EEPROM_Read_Uint16(cEepromPowerOffAH)*100;
    WorkStep_PowerFail.Step_Wh_EEPROM      = (Uint32)EEPROM_Read_Uint16(cEepromPowerOffEnergy)*100;
    WorkStep_PowerFail.Vol_T1_EEPROM       = EEPROM_Read_Uint16(cEepromVol_T1);
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Read_Para_Cali(void)
{

    DCDC_Service.Vol_Scale.K_ROM_Bus    = EEPROM_Read_Uint16(cEepromDcBusVoltAdjK);
    DCDC_Service.Vol_Scale.K_Now_Bus    = (int32)VOL_K_BUS_DEFAULT*10;

    DCDC_Service.Vol_Scale.K_ROM_Sense  = (int32)EEPROM_Read_Uint16(cEepromBatSenseVoltAdjK);
    if(DCDC_Service.Vol_Scale.K_ROM_Sense == 0xFFFF)
    {
        EEPROM_Write_Uint16(cEepromBatSenseVoltAdjK,VOL_K_SENSE_DEFAULT);
        DCDC_Service.Vol_Scale.K_ROM_Sense = VOL_K_SENSE_DEFAULT;
        EEPROM_Write_Uint16(cEepromBatSenseVoltAdjb,VOL_B_SENSE_DEFAULT);
        DCDC_Service.Vol_Scale.B_ROM_Sense = VOL_B_SENSE_DEFAULT;
    }
    DCDC_Service.Vol_Scale.K_ROM_Out    = DCDC_Service.Vol_Scale.K_ROM_Sense;
    DCDC_Service.Vol_Scale.K_Now_Sense  = Vol_Scale_K_Sense_Check(DCDC_Service.Vol_Scale.K_ROM_Sense);
    DCDC_Service.Vol_Scale.K_Now_Out    = Vol_Scale_K_Out_Check(DCDC_Service.Vol_Scale.K_ROM_Out);

    DCDC_Service.Vol_Scale.B_ROM_Sense  = (int16)EEPROM_Read_Uint16(cEepromBatSenseVoltAdjb);
    DCDC_Service.Vol_Scale.B_ROM_Out    = DCDC_Service.Vol_Scale.B_ROM_Sense;
    DCDC_Service.Vol_Scale.B_Now_Sense  = Vol_Scale_B_Sense_Check(DCDC_Service.Vol_Scale.B_ROM_Sense);
    DCDC_Service.Vol_Scale.B_Now_Out    = Vol_Scale_B_Out_Check(DCDC_Service.Vol_Scale.B_ROM_Out);

    //充电
    CH_A.Cur_Scale_H.K_ROM_CH   = (int32)EEPROM_Read_Uint16(cEepromOutPutCurrentHAdjAK);
    CH_A.Cur_Scale_H.B_ROM_CH   = (int16)EEPROM_Read_Uint16(cEepromOutPutCurrentHAdjAb);
    if(CH_A.Cur_Scale_H.K_ROM_CH == 0xFFFF)
    {
        EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjAK,CUR_H_K_DEFAULT);
        CH_A.Cur_Scale_H.K_ROM_CH = CUR_H_K_DEFAULT;
        EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjAb,CUR_H_B_DEFAULT);
        CH_A.Cur_Scale_H.B_ROM_CH = CUR_H_B_DEFAULT;
    }

    CH_B.Cur_Scale_H.K_ROM_CH   = (int32)EEPROM_Read_Uint16(cEepromOutPutCurrentHAdjBK);
    CH_B.Cur_Scale_H.B_ROM_CH   = (int16)EEPROM_Read_Uint16(cEepromOutPutCurrentHAdjBb);
    if(CH_B.Cur_Scale_H.K_ROM_CH == 0xFFFF)
    {
        EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjBK,CUR_H_K_DEFAULT);
        CH_B.Cur_Scale_H.K_ROM_CH = CUR_H_K_DEFAULT;
        EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjBb,CUR_H_B_DEFAULT);
        CH_B.Cur_Scale_H.B_ROM_CH = CUR_H_B_DEFAULT;
    }

    CH_A.Cur_Scale_L.K_ROM_CH   = (int32)EEPROM_Read_Uint16(cEepromOutPutCurrentLAdjAK);
    CH_A.Cur_Scale_L.B_ROM_CH   = (int16)EEPROM_Read_Uint16(cEepromOutPutCurrentLAdjAb);
    if(CH_A.Cur_Scale_L.K_ROM_CH == 0xFFFF)
    {
        EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjAK,CUR_L_K_DEFAULT);
        CH_A.Cur_Scale_L.K_ROM_CH = CUR_L_K_DEFAULT;
        EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjAb,CUR_L_B_DEFAULT);
        CH_A.Cur_Scale_L.B_ROM_CH = CUR_L_B_DEFAULT;
    }

    CH_B.Cur_Scale_L.K_ROM_CH   = (int32)EEPROM_Read_Uint16(cEepromOutPutCurrentLAdjBK);
    CH_B.Cur_Scale_L.B_ROM_CH   = (int16)EEPROM_Read_Uint16(cEepromOutPutCurrentLAdjBb);
    if(CH_B.Cur_Scale_L.K_ROM_CH == 0xFFFF)
    {
        EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjBK,CUR_L_K_DEFAULT);
        CH_B.Cur_Scale_L.K_ROM_CH = CUR_L_K_DEFAULT;
        EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjBb,CUR_L_B_DEFAULT);
        CH_B.Cur_Scale_L.B_ROM_CH = CUR_L_B_DEFAULT;
    }

    //放电
    CH_A.Cur_Scale_L.K_ROM_DIS  = (int32)EEPROM_Read_Uint16(cEepromDischargeAdjLAK);
    CH_A.Cur_Scale_L.B_ROM_DIS  = (int16)EEPROM_Read_Uint16(cEepromDischargeAdjLAB);
    if(CH_A.Cur_Scale_L.K_ROM_DIS == 0xFFFF)
    {
        EEPROM_Write_Uint16(cEepromDischargeAdjLAK,CUR_L_K_DEFAULT);
        CH_A.Cur_Scale_L.K_ROM_DIS = CUR_L_K_DEFAULT;
        EEPROM_Write_Uint16(cEepromDischargeAdjLAB,CUR_L_B_DEFAULT);
        CH_A.Cur_Scale_L.B_ROM_DIS = CUR_L_B_DEFAULT;
    }

    CH_A.Cur_Scale_H.K_ROM_DIS  = (int32)EEPROM_Read_Uint16(cEepromDischargeAdjHAK);
    CH_A.Cur_Scale_H.B_ROM_DIS  = (int16)EEPROM_Read_Uint16(cEepromDischargeAdjHAB);
    if(CH_A.Cur_Scale_H.K_ROM_DIS == 0xFFFF)
    {
        EEPROM_Write_Uint16(cEepromDischargeAdjHAK,CUR_H_K_DEFAULT);
        CH_A.Cur_Scale_H.K_ROM_DIS = CUR_H_K_DEFAULT;
        EEPROM_Write_Uint16(cEepromDischargeAdjHAB,CUR_H_B_DEFAULT);
        CH_A.Cur_Scale_H.B_ROM_DIS = CUR_H_B_DEFAULT;
    }

    CH_B.Cur_Scale_L.K_ROM_DIS  = (int32)EEPROM_Read_Uint16(cEepromDischargeAdjLBK);
    CH_B.Cur_Scale_L.B_ROM_DIS  = (int16)EEPROM_Read_Uint16(cEepromDischargeAdjLBB);
    if(CH_B.Cur_Scale_L.K_ROM_DIS == 0xFFFF)
    {
        EEPROM_Write_Uint16(cEepromDischargeAdjLBK,CUR_L_K_DEFAULT);
        CH_B.Cur_Scale_L.K_ROM_DIS = CUR_L_K_DEFAULT;
        EEPROM_Write_Uint16(cEepromDischargeAdjLBB,CUR_L_B_DEFAULT);
        CH_B.Cur_Scale_L.B_ROM_DIS = CUR_L_B_DEFAULT;
    }

    CH_B.Cur_Scale_H.K_ROM_DIS  = (int32)EEPROM_Read_Uint16(cEepromDischargeAdjHBK);
    CH_B.Cur_Scale_H.B_ROM_DIS  = (int16)EEPROM_Read_Uint16(cEepromDischargeAdjHBB);
    if(CH_B.Cur_Scale_H.K_ROM_DIS == 0xFFFF)
    {
        EEPROM_Write_Uint16(cEepromDischargeAdjHBK,CUR_H_K_DEFAULT);
        CH_B.Cur_Scale_H.K_ROM_DIS = CUR_H_K_DEFAULT;
        EEPROM_Write_Uint16(cEepromDischargeAdjHBB,CUR_H_B_DEFAULT);
        CH_B.Cur_Scale_H.B_ROM_DIS = CUR_H_B_DEFAULT;
    }

    //加范围判断
    CH_A.Cur_Scale_L.K_Now      = Cur_Scale_L_K_Check(CH_A.Cur_Scale_L.K_ROM_CH);
    CH_A.Cur_Scale_L.B_Now      = Cur_Scale_L_B_Check(CH_A.Cur_Scale_L.B_ROM_CH);
    CH_A.Cur_Scale_H.K_Now      = Cur_Scale_H_K_Check(CH_A.Cur_Scale_H.K_ROM_CH);
    CH_A.Cur_Scale_H.B_Now      = Cur_Scale_H_B_Check(CH_A.Cur_Scale_H.B_ROM_CH);

    CH_B.Cur_Scale_L.K_Now      = Cur_Scale_L_K_Check(CH_B.Cur_Scale_L.K_ROM_CH);
    CH_B.Cur_Scale_L.B_Now      = Cur_Scale_L_B_Check(CH_B.Cur_Scale_L.B_ROM_CH);
    CH_B.Cur_Scale_H.K_Now      = Cur_Scale_H_K_Check(CH_B.Cur_Scale_H.K_ROM_CH);
    CH_B.Cur_Scale_H.B_Now      = Cur_Scale_H_B_Check(CH_B.Cur_Scale_H.B_ROM_CH);

    Data_Sum.Vol_5V             = EEPROM_Read_Uint16(cEepromCaliVolPT);

    if((Data_Sum.Vol_5V<48000)||(Data_Sum.Vol_5V>52000))
    {
        Data_Sum.Vol_5V=50000;
    }

    WorkStep_Service.Cur_Limited_Set=EEPROM_Read_Uint16(cEepromCurLimitedSet);

    if(WorkStep_Service.Cur_Limited_Set==0xFFFF)
    {
        WorkStep_Service.Cur_Limited_Set=CUR_MAX_CHANNEL*2;
        EEPROM_Write_Uint16(cEepromCurLimitedSet,WorkStep_Service.Cur_Limited_Set/10000);
    }
    else
    {
        WorkStep_Service.Cur_Limited_Set=WorkStep_Service.Cur_Limited_Set*10000;
    }
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Read_Para_Cabinet(void)
{
    Num_Cabinet = EEPROM_Read_Uint16(cEepromCabinetsNum);
    Addr_Module = EEPROM_Read_Uint16(cEepromModuleAddress);
}


//*********************************************************************************
//Description: EEPROM读写服务状态机
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void EEPROM_Service(void)
{
    switch(I2C_Service.State)
    {
        case 1:{//当前指向的服务是否有读或写EEPROM请求检查
                if((I2C_Service.Ptr->Result_RW==I2C_FREE)&&(I2C_Service.Ptr->Req_RW!=REQ_I2C_FREE))
                {
                    I2C_Service.Cnt_Overtime=0;
                    I2C_Service.Cnt_Bytes=0;
                    I2C_Service.Cnt_Error=0;
                    if(I2C_Service.Ptr->Req_RW==REQ_I2C_READ)
                    {   //读请求
                        I2C_Service.State=2;
                    }
                    else if(I2C_Service.Ptr->Req_RW==REQ_I2C_WRITE)
                    {   //写请求
                        I2C_Service.State=12;
                    }
                    else
                    {   //异常值，指向下一个服务对象
                        I2C_Service.Ptr->Req_RW=REQ_I2C_FREE;
                        I2C_Service.Ptr=I2C_Service.Ptr->Ptr_Next;
                    }
                }
                else
                {   //指向下一个服务对象
                    I2C_Service.Ptr=I2C_Service.Ptr->Ptr_Next;
                }
                 break;}

        case 2:{
                if(I2caRegs.I2CSTR.bit.SCD==1)
                {I2caRegs.I2CSTR.bit.SCD=1;}
                // Setup Device Address
                Uint16 Temp;
                Temp=I2C_Service.Ptr->Data[I2C_Service.Cnt_Bytes].Addr;
                I2caRegs.I2CSAR = I2C_Service.Ptr->Addr_Device+((Temp&0x0300)>>8);

                // Setup Register Address
                I2caRegs.I2CDXR = Temp&0x00FF;

                // Setup Number of Bytes
                I2caRegs.I2CCNT = 1;

                // Send start as master transmitter
                I2caRegs.I2CMDR.all = 0x6E20;

                I2C_Service.Cnt_Overtime=0;
                I2C_Service.State=3;
                break;}

        case 3:{if((I2caRegs.I2CSTR.bit.BB==0)&&(I2caRegs.I2CSTR.bit.SCD==1))
                {
                    I2caRegs.I2CSTR.bit.SCD=1;

                    // Setup Slave Address
                    Uint16 Temp;
                    Temp=I2C_Service.Ptr->Data[I2C_Service.Cnt_Bytes].Addr;
                    I2caRegs.I2CSAR = I2C_Service.Ptr->Addr_Device+((Temp&0x0300)>>8);

                    // Setup Number of Bytes
                    I2caRegs.I2CCNT = 1;

                    // Send start as master transmitter
                    I2caRegs.I2CMDR.all = 0x6C20;
                    I2C_Service.Cnt_Overtime=0;
                    I2C_Service.State=4;
                }
                else
                {
                    if(++I2C_Service.Cnt_Overtime>=I2C_Service.Ptr->Num_Overtime)
                    {
                         I2C_Service.Cnt_Overtime=0;
                         I2C_Service.Cnt_Error++;
                         I2C_Service.Ptr->Req_RW=REQ_I2C_FREE;
                         I2C_Service.Ptr->Result_RW=I2C_READ_ERR;
                         I2C_Service.Ptr=I2C_Service.Ptr->Ptr_Next;
                         I2C_Service.State=1;
                    }
                }
                break;}

        case 4:{if((I2caRegs.I2CSTR.bit.SCD==1)&&(I2caRegs.I2CFFRX.bit.RXFFST==1))
                {
                    I2caRegs.I2CSTR.bit.SCD=1;

                    I2C_Service.Cnt_Error=0;//正确读取一次后错误计数器清零
                    I2C_Service.Cnt_Overtime=0;

                    I2C_Service.Ptr->Data[I2C_Service.Cnt_Bytes].Data=I2caRegs.I2CDRR&0x00FF;

                    I2C_Service.Cnt_Bytes++;
                    if((I2C_Service.Cnt_Bytes>=I2C_Service.Ptr->Num_Bytes)||(I2C_Service.Cnt_Bytes>=I2C_Service.Ptr->Len_Buff))
                    {
                        I2C_Service.Ptr->Req_RW=REQ_I2C_FREE;
                        I2C_Service.Ptr->Result_RW=I2C_READ_OK;
                        I2C_Service.Ptr=I2C_Service.Ptr->Ptr_Next;
                        I2C_Service.State=1;
                    }
                    else
                    {
                        I2C_Service.State=2;
                    }
                }
                else
                {
                    if(++I2C_Service.Cnt_Overtime>=I2C_Service.Ptr->Num_Overtime)
                    {
                        I2C_Service.Cnt_Overtime=0;
                        I2C_Service.Cnt_Error++;
                        I2C_Service.Ptr->Req_RW=REQ_I2C_FREE;
                        I2C_Service.Ptr->Result_RW=I2C_READ_ERR;
                        I2C_Service.Ptr=I2C_Service.Ptr->Ptr_Next;
                        I2C_Service.State=1;
                    }
                }
                break;}

       case 12:{
                if(I2caRegs.I2CSTR.bit.SCD==1)
                {I2caRegs.I2CSTR.bit.SCD=1;}
                // Setup Slave Address
                Uint16 Temp;
                Temp=I2C_Service.Ptr->Data[I2C_Service.Cnt_Bytes].Addr;
                I2caRegs.I2CSAR = I2C_Service.Ptr->Addr_Device+((Temp&0x0300)>>8);

                // Setup Register Address
                I2caRegs.I2CDXR = Temp&0x00FF;

                I2caRegs.I2CDXR = I2C_Service.Ptr->Data[I2C_Service.Cnt_Bytes].Data&0x00FF;

                // Setup Number of Bytes
                I2caRegs.I2CCNT = 2;

                // Send start as master transmitter
                I2caRegs.I2CMDR.all     = 0x6E20;
                I2C_Service.Cnt_Overtime= 0;
                I2C_Service.State       = 13;
                break;}

       case 13:{if(I2caRegs.I2CSTR.bit.SCD==1)
                {
                    I2caRegs.I2CSTR.bit.SCD=1;
                    I2C_Service.Cnt_Error=0;
                    I2C_Service.Cnt_Overtime=0;

                    I2C_Service.Cnt_Bytes++;
                    if((I2C_Service.Cnt_Bytes>=I2C_Service.Ptr->Num_Bytes)||(I2C_Service.Cnt_Bytes>=I2C_Service.Ptr->Len_Buff))
                    {
                        I2C_Service.Ptr->Req_RW=REQ_I2C_FREE;
                        I2C_Service.Ptr->Result_RW=I2C_FREE;//I2C_WRITE_OK;
                        I2C_Service.Ptr=I2C_Service.Ptr->Ptr_Next;
                        I2C_Service.State=1;
                    }
                    else
                    {
                        I2C_Service.Cnt_Delay=0;
                        I2C_Service.State=14;
                    }
                }
                else
                {
                    if(++I2C_Service.Cnt_Overtime>=I2C_Service.Ptr->Num_Overtime)
                    {
                        I2C_Service.Cnt_Error++;
                        I2C_Service.Ptr->Req_RW=REQ_I2C_FREE;
                        I2C_Service.Ptr->Result_RW=I2C_WRITE_ERR;
                        I2C_Service.Ptr=I2C_Service.Ptr->Ptr_Next;
                        I2C_Service.State=1;
                    }
                }
                break;}

       case 14:{
                if(++I2C_Service.Cnt_Delay>60)
                {
                    I2C_Service.Cnt_Delay=0;
                    I2C_Service.State=12;
                }
               break;}

        case 0:{I2C_Service.State=1;
                I2C_Service.Serv_Ready=1;
                break;}

        default:I2C_Service.State=0;
    }
}


//File end
