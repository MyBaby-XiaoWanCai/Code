/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: main.c
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义
Uint16 Flag_Tick=0;
Uint16 Cnt_Tick=0;

//函数声明
void InitPieVectTable_BSP(void);

//函数定义
void main(void)
{
    //
    DINT;

    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the DSP280x_SysCtrl.c file.
    InitSysCtrl();

    MemCopy(&RamfuncsLoadStart,&RamfuncsLoadEnd,&RamfuncsRunStart);

    InitFlash();
    // Step 2. Initalize GPIO:
    // This example function is found in the DSP280x_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    InitGpio();
    InitGPIO_BSP();

    PWM1OFF;
    PWM2OFF;

    DC_OUT_CHA_OFF;
    DC_OUT_CHB_OFF;

    InitEPwmGpio();

    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    DINT;

    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the DSP280x_PieCtrl.c file.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in DSP280x_DefaultIsr.c.
    // This function is found in DSP280x_PieVect.c.
    InitPieVectTable();

    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    InitPieVectTable_BSP();//

    // Step 4. Initialize all the Device Peripherals:
    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer2, 100, 1000);//1mS中断1次
    StartCpuTimer2();

    InitADC_BSP();

    InitDCDC_BSP();

    InitI2C_BSP();

    Read_Para_Cali();//debug

    Read_Para_Cabinet();//debug

    InitFlash_BSP();

    InitEPwmGpio();

    Fault_Code_DogReset_TX();

    InitW5500_BSP();

    InitCAN_BSP();

    InitSCI_BSP();

    EALLOW;
    CONFIG_GPIO_FLASH_CS;
    CONFIG_GPIO_W5500_CS;
    EDIS;
    //SET_FLASH_CS;
    // Step 5. User specific code, enable interrupts:


    // Enable CPU INT1 which is connected to CPU-Timer 0:
    IER |= M_INT3;
    IER |= M_INT9;
    IER |= M_INT14; //for Timer2
    // Enable TINT0 in the PIE: Group 1 interrupt 7
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1;     // Enable SCIA RX interrut
    PieCtrlRegs.PIEIER9.bit.INTx2 = 1;     // Enable SCIA TX interrut

    //是否跳转固件升级Boot判断
    if(ReadBootFlag()==0x01)
    {
        JumpToBoot();
    }


    WorkStep_Service_Init();

    Past_Data_Manage_Init();

    Protect_Global_Init();

    Protect_Step_Init();

    Read_Data_PowerFail();

    Read_Data_Parall();

    Read_Data_Compen_Cur();

    Auto_Aging_Init();

    InitWD();

    // Enable global Interrupts and higher priority real-time debug events:
    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM
    // Step 6. IDLE loop. Just sit and loop forever (optional):InitEPwm1Gpio
    while(1)
    {

        if((Flag_Tick==TRUE)&&(Cnt_Tick==0))
        {
            Flag_Tick=FALSE;
            Socket_Work();
            Auto_To_Master();
            ServiceDog();
        }

        if((Flag_Tick==TRUE)&&(Cnt_Tick==5))
        {
            Flag_Tick=FALSE;
            Socket_Debug();
            sSCIAParsing();
            Auto_Adjust_PI_Vol_Charge();
            Auto_Adjust_PI_Vol_Discharge();
        }

        if((Flag_Tick==TRUE)&&(Cnt_Tick==10))
        {
            Flag_Tick=FALSE;
            Protect_CH_Imbalance();

        }

        if((Flag_Tick==TRUE)&&(Cnt_Tick==15))
        {
            Flag_Tick=FALSE;
            Ctr_LED();
            Fan_Fault_Check();
			if((WorkStep_Service.State==STEP_RUN)&&(DCDC_Service.Mode_Parall!=PARALL_SLAVER)&&(WorkStep_Data.Type!=cStepStatic)&&(WorkStep_Data.Type!=cStepNegPressure))
            {
                Ah_Accumulate();
                Wh_Accumulate();
            }
            Data_Sum.Temp_PT	= sBatTemperatureRealCal(Data_Sum.AD_PT);

            #if VER_HARDWARE==C017 || VER_HARDWARE==C017_
            Data_Sum.Temp_NTC1  = sTemperature1SampleAvgCal(Data_Sum.AD_NTC1);
            #elif VER_HARDWARE==C017B || VER_HARDWARE==C017D || VER_HARDWARE==C017D_
            Data_Sum.Temp_NTC1	= sTemperature1SampleAvgCal_B3380(Data_Sum.AD_NTC1);
            #endif

            Data_Sum.Temp_NTC2	= sTemperature1SampleAvgCal(Data_Sum.AD_NTC2);
            Sink_OverHeat_Check();
            Ctr_Fan();
            DINT;
            Parall_Slaver.IDx1_To_M.IDx_L.IDx1_L.Temp_Sink=Data_Sum.Temp_NTC2;
            Parall_Slaver.IDx1_To_M.IDx_L.IDx1_L.Temp_Shunt=Data_Sum.Temp_NTC1;
            EINT;
        }

        if((Flag_Tick==TRUE)&&(Cnt_Tick==20))
        {
            Flag_Tick=FALSE;
            ServiceDog();
            Process_Average_Analog();
            Protect_Mask_Counter();
            Function_Record();
            Protect_Check();
            Fault_Mapping();
            Upload_Fault_Code();
            WorkStep_Flow();
            Data_Up();
            Socket_Work();
        }

        if((Flag_Tick==TRUE)&&(Cnt_Tick==25))
        {
            Flag_Tick=FALSE;
        }

        if((Flag_Tick==TRUE)&&(Cnt_Tick==30))
        {
            Flag_Tick=FALSE;

            Slaver_Abnormal_Check();
            Socket_Work_Timeout_Check();
        }

        if((Flag_Tick==TRUE)&&(Cnt_Tick==35))
        {
            Flag_Tick=FALSE;
            Auto_Aging_Run();
            WorkStep_Aging_WritetoFlash();
            Set_By_CAN();
            Function_Compen_Cur();
        }

        if(WorkStep_Service.Req_PowerFailSave == TRUE)
        {
            ServiceDog();
            EEPROM_Write_Uint16(cEepromPowerOffStep,WorkStep_Data.Index);
            EEPROM_Write_Uint16(cEepromPowerOfftime,WorkStep_Service.Step_Time/1000);
            EEPROM_Write_Uint16(cEepromPowerOffAH,WorkStep_Service.Step_Ah/100);
            EEPROM_Write_Uint16(cEepromPowerOffEnergy,WorkStep_Service.Step_Wh/100);
            EEPROM_Write_Uint16(cEepromVol_T1,WorkStep_Data.Prot_Step_Vol_Change_Time.Value_t1&0x0000FFFF);
            WorkStep_Service.Req_PowerFailSave      = FALSE;
            WorkStep_PowerFail.Step_Index_EEPROM    = WorkStep_Data.Index;
            WorkStep_PowerFail.Step_State_EEPROM    = WorkStep_PowerFail.Step_State_Now;
            WorkStep_PowerFail.Step_Time_EEPROM     = (WorkStep_Service.Step_Time/1000)*1000;
            WorkStep_PowerFail.Step_Ah_EEPROM       = WorkStep_Service.Step_Ah;
            WorkStep_PowerFail.Step_Wh_EEPROM       = WorkStep_Service.Step_Wh;
        }

    };
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************

#pragma CODE_SECTION(Cputimer2Interrupt,"ramfuncs");
interrupt void Cputimer2Interrupt(void)
{
    //debug GpioDataRegs.GPATOGGLE.bit.GPIO6 = 1;
    //后台任务时间片分配
    if(++Cnt_Tick==40)
    {
        Cnt_Tick=0;
    }
    Flag_Tick=TRUE;
    DateTime++;
    if(WorkStep_Service.State==STEP_RUN)//debug 用WorkStep_Service.Step_State是否更好
    {
        WorkStep_Service.Step_Time++;
        if( WorkStep_Service.Step_Time%1000==0)
        {
            WorkStep_Service.Run_TimeTotal++;
        }
        if((WorkStep_Data.Record_Time.Delta_Time>1000)&&(WorkStep_Service.Step_Time==1000))
        {
            WorkStep_Data.Record_Time.IsRecord=TRUE;
        }

        if(WorkStep_Data.Record_Time.EN==TRUE)
        {
            WorkStep_Data.Record_Time.Cnt_Time++;
            if(WorkStep_Data.Record_Time.Cnt_Time>=WorkStep_Data.Record_Time.Delta_Time)
            {
                WorkStep_Data.Record_Time.Cnt_Time=0;
                WorkStep_Data.Record_Time.IsRecord=TRUE;
                Past_Data_Manage.Cnt_Save=0;
                Past_Data_Manage.Req_Save=FALSE;
                DCDC_Service.Flag_CV_Step2 = FALSE;
                DCDC_Service.Flag_DCV_Step2 = FALSE;
            }
            else if((WorkStep_Data.Record_Time.Cnt_Time%1000 == 0)&&(WorkStep_Data.Record_Time.Delta_Time >= 2000))
            {
                if((DCDC_Service.Flag_CV_Step2== TRUE)||(DCDC_Service.Flag_DCV_Step2 == TRUE))
                {
                     WorkStep_Data.Record_Time.IsRecord=TRUE;
                }

            }
        }

        if(++Past_Data_Manage.Cnt_Save>=Past_Data_Manage.Period_Save)
        {
            Past_Data_Manage.Cnt_Save=0;
            Past_Data_Manage.Req_Save=TRUE;
        }
    }

    //串口不完整数据帧超时检测
    if(SCIA_RX.State_Recived==FALSE)
    {
        if(SCIA_RX.Cnt_RX!=0)
        {
            if(++SCIA_RX.Cnt_Timeout>=SCIA_RX.Num_Timeout)
            {
                SCIA_RX.Cnt_Timeout=0;
                SCIA_RX.Cnt_RX=0;
            }
        }
    }

    CpuTimer2Regs.TCR.bit.TIF   = 1;
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void InitPieVectTable_BSP(void)
{
    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.EPWM1_INT  = &EPWM1_INT_ISR_BSP;
    PieVectTable.TINT2      = &Cputimer2Interrupt;
    PieVectTable.SCIRXINTA  = &SCIAReceiveInterrupt;
    PieVectTable.SCITXINTA  = &SCIATransmitInterrupt;
    EDIS;    // This is needed to disable write to EALLOW protected registers
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void WriteBootFlag(void)
{
    EEPROM_Write_Uint16(cBootFlag,0x0001);
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
Uint16 ReadBootFlag(void)
{
    return(EEPROM_Read_Uint16(cBootFlag));
}


//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Fault_Code_DogReset_TX(void)
{
    Uint16 Temp=0;
    Uint16 Sn_TX_WR=0;

    InitSpicGpio();

    SpicRegs.SPICCR.bit.SPISWRESET=0;
    SpicRegs.SPICCR.all =0x07;
    //SpicRegs.SPICCR.bit.SPILBK=1;
    SpicRegs.SPICTL.all =0x0E;
    SpicRegs.SPIBRR =0x0F;//20200414 长帧出现校验码错误降低SPI频率  SpicRegs.SPIBRR =0x07;
    SpicRegs.SPIFFTX.all=0xC008;
    SpicRegs.SPIFFRX.all=0x0008;
    SpicRegs.SPIFFCT.all=0x1;
    SpicRegs.SPIPRI.all=0x0010;
    SpicRegs.SPIPRI.bit.FREE = 1;
    SpicRegs.SPICCR.bit.SPISWRESET=1;
    SpicRegs.SPIFFTX.bit.TXFIFO=1;
    SpicRegs.SPIFFRX.bit.RXFIFORESET=1;

    EALLOW;
    CONFIG_GPIO_W5500_CS;
    EDIS;

    if((SysCtrlRegs.WDCR&0x0080)!=0)
    {
        SET_W5500_CS;
        DELAY_US(10);
        SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
        SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
        CLEAR_W5500_CS;
        SpicRegs.SPITXBUF   = REG_SOCK_IR&0xFF00;
        SpicRegs.SPITXBUF   = (REG_SOCK_IR<<8)&0xFF00;
        SpicRegs.SPITXBUF   = (BL_REG_SOCK+RWB_READ+OM_N_Byte)<<8;
        SpicRegs.SPITXBUF   = 0xAA00;
        SpicRegs.SPITXBUF   = 0xAA00;
        DELAY_US(100);
        CLEAR_W5500_CS;

        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF&0x00FF;

        if(Temp!=SR_SOCK_ESTABLISHED)
        {
            return;
        }


        SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
        SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
        CLEAR_W5500_CS;
        SpicRegs.SPITXBUF= REG_VERSIONR    &0xFF00;
        SpicRegs.SPITXBUF=(REG_VERSIONR<<8)&0xFF00;
        SpicRegs.SPITXBUF=(BL_REG_COMMON+RWB_READ+OM_N_Byte)<<8;
        SpicRegs.SPITXBUF=0xAA00;

        DELAY_US(100);
        SET_W5500_CS;

        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;

        //读W5500发送缓冲区空闲空间
        SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
        SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
        CLEAR_W5500_CS;
        SpicRegs.SPITXBUF= REG_SOCK_TX_FSR0    &0xFF00;
        SpicRegs.SPITXBUF=(REG_SOCK_TX_FSR0<<8)&0xFF00;
        SpicRegs.SPITXBUF=(BL_REG_SOCK+RWB_READ+OM_N_Byte)<<8;
        SpicRegs.SPITXBUF=0xAA00;
        SpicRegs.SPITXBUF=0xAA00;
        SpicRegs.SPITXBUF=0xAA00;
        SpicRegs.SPITXBUF=0xAA00;
        SpicRegs.SPITXBUF=0xAA00;
        SpicRegs.SPITXBUF=0xAA00;
        SpicRegs.SPITXBUF=0xAA00;
        SpicRegs.SPITXBUF=0xAA00;
        SpicRegs.SPITXBUF=0xAA00;
        SpicRegs.SPITXBUF=0xAA00;
        SpicRegs.SPITXBUF=0xAA00;
        SpicRegs.SPITXBUF=0xAA00;
        DELAY_US(100);
        SET_W5500_CS;

        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;

        Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
        Temp=Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff();
        Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
        Temp =Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff();
        Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
        Sn_TX_WR =Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff();
        Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
        Temp=Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff()&0x0FFF;
        Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
        Temp =Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff()&0x0FFF;
        Temp=(SpicRegs.SPIRXBUF<<8)&0xFF00;
        Temp =Temp|(SpicRegs.SPIRXBUF&0x00FF);//Read_Uint16_From_Buff()&0x0FFF;

        //数据写入W5500发送RAM
        DELAY_US(20);
        SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
        SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
        CLEAR_W5500_CS;
        SpicRegs.SPITXBUF   =  Sn_TX_WR    &0xFF00;
        SpicRegs.SPITXBUF   = (Sn_TX_WR<<8)&0xFF00;
        SpicRegs.SPITXBUF   =(BL_RAM_TX+RWB_WRITE+OM_N_Byte)<<8;
        SpicRegs.SPITXBUF   = 0x0600;
        Sn_TX_WR++;
        SpicRegs.SPITXBUF   = 0x0500;
        Sn_TX_WR++;
        SpicRegs.SPITXBUF   = 0x5100; //0x2A00;    //看门狗保护
        Sn_TX_WR++;
        SpicRegs.SPITXBUF   = 0x4E00;
        Sn_TX_WR++;
        SpicRegs.SPITXBUF   = 0x1300;
        Sn_TX_WR++;
        SpicRegs.SPITXBUF   = 0x0D00;
        Sn_TX_WR++;

        DELAY_US(100);
        SET_W5500_CS;

        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;

        SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
        SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
        CLEAR_W5500_CS;
        SpicRegs.SPITXBUF   =  REG_SOCK_TX_WR0    &0xFF00;
        SpicRegs.SPITXBUF   = (REG_SOCK_TX_WR0<<8)&0xFF00;
        SpicRegs.SPITXBUF   =(BL_REG_SOCK+RWB_WRITE+OM_N_Byte)<<8;
        SpicRegs.SPITXBUF   =  Sn_TX_WR    &0xFF00;
        SpicRegs.SPITXBUF   = (Sn_TX_WR<<8)&0xFF00;
        DELAY_US(100);
        SET_W5500_CS;

        //启动数据发送
        DELAY_US(20);
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;
        Temp=SpicRegs.SPIRXBUF;

        SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
        SpicRegs.SPIFFRX.bit.RXFIFORESET=1;
        CLEAR_W5500_CS;
        SpicRegs.SPITXBUF   =  REG_SOCK_CR    &0xFF00;
        SpicRegs.SPITXBUF   = (REG_SOCK_CR<<8)&0xFF00;
        SpicRegs.SPITXBUF   =(BL_REG_SOCK+RWB_WRITE+OM_N_Byte)<<8;
        SpicRegs.SPITXBUF   =  CMD_CR_SEND<<8;
        DELAY_US(100);
        SET_W5500_CS;

        //延时等待W5500将数据发出
        DELAY_US(20000);
    }
}

//File end
