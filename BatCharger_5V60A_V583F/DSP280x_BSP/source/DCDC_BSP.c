/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: DCDC_BSP.c
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义
Uint16 Cnt_Driver=0;

struct_PI PI_Vol_Charge;                //CCtoCV模式电压环PI控制数据结构体
struct_PI PI_Vol_Discharge;             //DCCtoDCV模式电压环PI控制数据结构体
struct_PI PI_Vol_CV;                    //CV模式电压环PI控制数据结构体
struct_PI PI_Vol_DCV;                   //DCV模式电压环PI控制数据结构体

struct_PI CH_A_PI_Cur_Charge;           //CHA充电模式电流环PI控制结构体
struct_PI CH_A_PI_Cur_Discharge;        //CHA放电模式电流环PI控制结构体

struct_PI CH_B_PI_Cur_Charge;           //CHB充电模式电流环PI控制结构体
struct_PI CH_B_PI_Cur_Discharge;        //CHB放电模式电流环PI控制结构体

     struct_Data_AD Data_AD;            //从AD7606和F2808读出的AD原始数据
 struct_Data_AD_Sum Data_AD_Sum;        //校准用AD数据累加值
    struct_Data_Sum Data_Sum;           //定标转换后的物理量进行平均后得到的值
     INTERFACE_DCDC Interface_DCDC;     //DCDC模块工作参数请求接口
     struct_Channel CH_A;               //CHA相关数据
     struct_Channel CH_B;               //CHB相关数据
struct_DCDC_SERVICE DCDC_Service;       //DCDC状态机相关数据
struct_Bus_Forward  Bus_Forward;		//Bus前馈用
struct_Bus_Fluct_Check Bus_Fluct_Check; //Bus波动检测

struct_Adjust_PI_Vol Adjust_PI_Vol_Charge;
struct_Adjust_PI_Vol Adjust_PI_Vol_Discharge;

//函数声明
void DCDC_Service_BSP(void);
void Fault_OC_Check(void);

//ePWM中断函数
#pragma CODE_SECTION(EPWM1_INT_ISR_BSP,"ramfuncs");
interrupt void EPWM1_INT_ISR_BSP(void)
{
    //启动AD7606对输入的模拟信号开始AD转换
    SET_AD7606_CONV;
    asm(" RPT #2 || NOP");
    CLEAR_AD7606_CONV;
    asm(" RPT #6 || NOP");
    SET_AD7606_CONV;

    //重新启动DSP内部AD转换
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;      //start AD

    #if (VER_HARDWARE==C017 && VER_PRODUCT==PRODUCT_5V60A)||(VER_HARDWARE==C017D_ && VER_PRODUCT==PRODUCT_5V80A)
    CH_A.Now_Cur=(int32)(((long long)CH_A.Cur_Scale_H.K_Now*Data_AD.AD7606_CH7)>>14)-CH_A.Cur_Scale_H.B_Now;
    CH_B.Now_Cur=(int32)(((long long)CH_B.Cur_Scale_H.K_Now*Data_AD.AD7606_CH6)>>14)-CH_B.Cur_Scale_H.B_Now;
    #else
    //电流定标转换
    if(DCDC_Service.Flag_CurRange==CUR_RANGE_L)
    {
        CH_A.Now_Cur=(int32)(((long long)CH_A.Cur_Scale_L.K_Now*Data_AD.AD7606_CH7)>>14)-CH_A.Cur_Scale_L.B_Now;
        CH_B.Now_Cur=(int32)(((long long)CH_B.Cur_Scale_L.K_Now*Data_AD.AD7606_CH6)>>14)-CH_B.Cur_Scale_L.B_Now;
    }
    else
    {
        CH_A.Now_Cur=(int32)(((long long)CH_A.Cur_Scale_H.K_Now*Data_AD.AD7606_CH7)>>14)-CH_A.Cur_Scale_H.B_Now;
        CH_B.Now_Cur=(int32)(((long long)CH_B.Cur_Scale_H.K_Now*Data_AD.AD7606_CH6)>>14)-CH_B.Cur_Scale_H.B_Now;
    }
    #endif

    //电压定标转换
    DCDC_Service.Now_Vol_Sense  = (int32)(((long long)DCDC_Service.Vol_Scale.K_Now_Sense*Data_AD.AD7606_CH2)>>15)-DCDC_Service.Vol_Scale.B_Now_Sense;
    DCDC_Service.Now_Vol_Out    = (int32)(((long long)DCDC_Service.Vol_Scale.K_Now_Out  *Data_AD.AD7606_CH3)>>15)-DCDC_Service.Vol_Scale.B_Now_Out;
    DCDC_Service.Now_Vol_Bus    = (int32)(((long long)DCDC_Service.Vol_Scale.K_Now_Bus  *Data_AD.F2808_CH2 )>>10);

    //Bus过压检测
    if((DCDC_Service.Fault_Module&FAULT_BUS_H)==0)
    {
        if(DCDC_Service.Now_Vol_Bus>VOL_BUS_H)
        {
            if(++DCDC_Service.Cnt_Bus_H>=5)
            {
                DCDC_Service.Fault_Module|=FAULT_BUS_H;
                DCDC_Service.Now_Mode=DCDC_PWM_OFF;

                PWM1OFF;
                PWM2OFF;

                if((WorkStep_Service.Step_State==cWorkStepRunStatus)||(WorkStep_Service.Step_State==cPauseStatus))
                {
                    WorkStep_Service.Req_PowerFailSave = TRUE;
                    WorkStep_PowerFail.Step_State_Now=cPowerFailNotComplete;
                }
            }
        }
        else
        {
            DCDC_Service.Cnt_Bus_H=0;
        }
    }

    //输出端口过压检测
    if((DCDC_Service.Fault_Module&FAULT_OUT_VOL_HIGH)==0)
    {
        if(Data_AD.AD7606_CH3>31166)//31166*6834/32768=6.5V
        {
            if(++DCDC_Service.Cnt_Out_H>=100)
            {
                DCDC_Service.Fault_Module|=FAULT_OUT_VOL_HIGH;
                DCDC_Service.Now_Mode=DCDC_PWM_OFF;
                PWM1OFF;
                PWM2OFF;
            }
        }
        else
        {
            DCDC_Service.Cnt_Out_H=0;
        }
    }

    //Bus低压检测,上电后延时4S开始Bus低压检测
    if((DCDC_Service.Fault_Module&FAULT_BUS_L)==0)
    {
        if(DCDC_Service.Cnt_Bus_L>=150000)
        {
            if(DCDC_Service.Now_Vol_Bus<VOL_BUS_L)
            {
                if(++DCDC_Service.Cnt_Bus_L>=150050)
                {

                    DCDC_Service.Now_Mode=DCDC_PWM_OFF;
                    PWM1OFF;
                    PWM2OFF;

                    if((WorkStep_Service.Step_State==cWorkStepRunStatus)||(WorkStep_Service.Step_State==cPauseStatus))
                    {
                        if((DCDC_Service.Fault_Module&FAULT_BUS_H)==0)
                        {
                            DCDC_Service.Fault_Module|=FAULT_BUS_L;
                            WorkStep_Service.Req_PowerFailSave = TRUE;
                            WorkStep_PowerFail.Step_State_Now=cPowerFailNotComplete;
                        }
                    }
                }
            }
            else
            {
                DCDC_Service.Cnt_Bus_L=150000;
            }
        }
        else
        {
            ++DCDC_Service.Cnt_Bus_L;
        }
    }

    //输出电流快速过流保护检测
    Fault_OC_Check();

    //读取DSP内部AD转换数据
    Data_AD.F2808_CH0 = AdcRegs.ADCRESULT0>>4;
    Data_AD.F2808_CH1 = AdcRegs.ADCRESULT1>>4;
    Data_AD.F2808_CH2 = AdcRegs.ADCRESULT2>>4;

    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;  //reset AD

    //模拟量定标转换后数据平均和部分AD数据平均
    Data_Sum.Sum_Cur_CHA    += CH_A.Now_Cur;
    Data_Sum.Sum_Cur_CHB    += CH_B.Now_Cur;
    Data_Sum.Sum_Vol_Bus    += DCDC_Service.Now_Vol_Bus;
    Data_Sum.Sum_Vol_Out    += DCDC_Service.Now_Vol_Out;
    Data_Sum.Sum_Vol_Sense  += DCDC_Service.Now_Vol_Sense;
    Data_Sum.Sum_AD_PT      += Data_AD.AD7606_CH0;
    Data_Sum.Sum_AD_NTC1    += Data_AD.F2808_CH0;
    Data_Sum.Sum_AD_NTC2    += Data_AD.F2808_CH1;
    Data_Sum.Sum_AD_NTC_BATB+=Data_AD.AD7606_CH4;

    if(++Data_Sum.Cnt_Sum>=Data_Sum.Num_Avg)
    {
        if(Data_Sum.Flag_Refresh==FALSE)
        {
            Data_Sum.Sum_Cur_CHA_Copy   = Data_Sum.Sum_Cur_CHA;
            Data_Sum.Sum_Cur_CHB_Copy   = Data_Sum.Sum_Cur_CHB;
            Data_Sum.Sum_Vol_Bus_Copy   = Data_Sum.Sum_Vol_Bus;
            Data_Sum.Sum_Vol_Out_Copy   = Data_Sum.Sum_Vol_Out;
            Data_Sum.Sum_Vol_Sense_Copy = Data_Sum.Sum_Vol_Sense;
            Data_Sum.Sum_AD_PT_Copy     = Data_Sum.Sum_AD_PT;
            Data_Sum.Sum_AD_NTC1_Copy   = Data_Sum.Sum_AD_NTC1;
            Data_Sum.Sum_AD_NTC2_Copy   = Data_Sum.Sum_AD_NTC2;
            Data_Sum.Sum_AD_NTC_BATB_Copy=Data_Sum.Sum_AD_NTC_BATB;
            Data_Sum.Flag_Refresh       = TRUE;
        }

        Data_Sum.Sum_Cur_CHA        = 0;
        Data_Sum.Sum_Cur_CHB        = 0;
        Data_Sum.Sum_Vol_Bus        = 0;
        Data_Sum.Sum_Vol_Out        = 0;
        Data_Sum.Sum_Vol_Sense      = 0;
        Data_Sum.Sum_AD_PT          = 0;
        Data_Sum.Sum_AD_NTC1        = 0;
        Data_Sum.Sum_AD_NTC2        = 0;
        Data_Sum.Sum_AD_NTC_BATB    = 0;
        Data_Sum.Cnt_Sum            = 0;
    }

    if(Interface_DCDC.Req_Aging==TRUE)//if(DCDC_Service.Flag_Aging==TRUE)
    {
        Data_AD_Sum.Sum_Cur_CHA_AD += Data_AD.AD7606_CH7;
        Data_AD_Sum.Sum_Cur_CHB_AD += Data_AD.AD7606_CH6;
        Data_AD_Sum.Sum_Vol_Out_AD += Data_AD.AD7606_CH3;
        Data_AD_Sum.Sum_Vol_Sense_AD += Data_AD.AD7606_CH2;
        if(++Data_AD_Sum.Cnt_Sum>=Data_AD_Sum.Num_Avg)
        {
            Data_AD_Sum.Cnt_Sum=0;
            Data_AD_Sum.Sum_Cur_CHA_AD_Copy = Data_AD_Sum.Sum_Cur_CHA_AD;
            Data_AD_Sum.Sum_Cur_CHA_AD=0;
            Data_AD_Sum.Sum_Cur_CHB_AD_Copy = Data_AD_Sum.Sum_Cur_CHB_AD;
            Data_AD_Sum.Sum_Cur_CHB_AD=0;
            Data_AD_Sum.Sum_Vol_Out_AD_Copy = Data_AD_Sum.Sum_Vol_Out_AD;
            Data_AD_Sum.Sum_Vol_Out_AD=0;
            Data_AD_Sum.Sum_Vol_Sense_AD_Copy = Data_AD_Sum.Sum_Vol_Sense_AD;
            Data_AD_Sum.Sum_Vol_Sense_AD=0;
        }
    }

    //Bus电压前馈值计算
    Bus_Forward.Vol_Bus_Fast_Last   = (Bus_Forward.Vol_Bus_Fast_Last*17+((int32)Data_AD.F2808_CH2<<6)*15)>>5;//转折频率3500Hz
    Bus_Forward.Vol_Bus_Fast        = (Bus_Forward.Vol_Bus_Fast_Last)>>6;

    Bus_Forward.Vol_Bus_Var         =  Bus_Forward.Vol_Bus_Fast-Bus_Forward.Vol_Bus_Slow;
    Bus_Forward.PWM_Forward         = (Bus_Forward.Vol_Bus_Var*Bus_Forward.K_Forward)>>8;

    if(Bus_Forward.PWM_Forward<Bus_Forward.Limit_Neg)
    {
        Bus_Forward.PWM_Forward=Bus_Forward.Limit_Neg;
    }
    else if(Bus_Forward.PWM_Forward>Bus_Forward.Limit_Pos)
    {
        Bus_Forward.PWM_Forward=Bus_Forward.Limit_Pos;
    }


    //从SPIB缓冲区读取AD数据
    if(SpibRegs.SPIFFRX.bit.RXFFST==8)
    {
        Data_AD.AD7606_CH0 = SpibRegs.SPIRXBUF;
        Data_AD.AD7606_CH1 = SpibRegs.SPIRXBUF;
        Data_AD.AD7606_CH2 = SpibRegs.SPIRXBUF;
        Data_AD.AD7606_CH3 = SpibRegs.SPIRXBUF;
        Data_AD.AD7606_CH4 = SpibRegs.SPIRXBUF;
        Data_AD.AD7606_CH5 = SpibRegs.SPIRXBUF;
        Data_AD.AD7606_CH6 = SpibRegs.SPIRXBUF;
        Data_AD.AD7606_CH7 = SpibRegs.SPIRXBUF;
        DCDC_Service.Cnt_Fault_AD7606=0;
    }
    else
    {
        //SPI收到的数据字节数错误，
        //SpibRegs.SPIFFRX.bit.RXFIFORESET = 0;
        //SpibRegs.SPIFFRX.bit.RXFIFORESET = 1;
        if((DCDC_Service.Fault_Module&FAULT_AD7606)==0)
        {
            if(++DCDC_Service.Cnt_Fault_AD7606>=10)
            {
                DCDC_Service.Fault_Module |= FAULT_AD7606;
            }
        }
    }

    //启动SPI读取AD数据
    DCDC_Service.Cnt_Timeout_Busy=0;
    while((AD7606_IS_BUSY)&&(DCDC_Service.Cnt_Timeout_Busy<50))
    {
        ++DCDC_Service.Cnt_Timeout_Busy;
    }

    if(AD7606_IS_READY)
    {
        SpibRegs.SPIFFRX.bit.RXFIFORESET=0;
        SpibRegs.SPIFFRX.bit.RXFIFORESET=1;
        SpibRegs.SPITXBUF = 0xAAAA;
        SpibRegs.SPITXBUF = 0xAAAA;
        SpibRegs.SPITXBUF = 0xAAAA;
        SpibRegs.SPITXBUF = 0xAAAA;
        SpibRegs.SPITXBUF = 0xAAAA;
        SpibRegs.SPITXBUF = 0xAAAA;
        SpibRegs.SPITXBUF = 0xAAAA;
        SpibRegs.SPITXBUF = 0xAAAA;
    }

    DCDC_Service.Sum_Vol_Sense  = DCDC_Service.Sum_Vol_Sense + DCDC_Service.Now_Vol_Sense;
    if(++DCDC_Service.Cnt_Sum_Vol_Sense>=DCDC_Service.Num_Sum_Vol_Sense)
    {
        DCDC_Service.Cnt_Sum_Vol_Sense=0;
        //DCDC_Service.Vol_Sense_LPF_Last=DCDC_Service.Vol_Sense_LPF;
        DCDC_Service.Vol_Sense_LPF=DCDC_Service.Sum_Vol_Sense/DCDC_Service.Num_Sum_Vol_Sense;
        /*if(DCDC_Service.Vol_Sense_LPF>DCDC_Service.Vol_Sense_LPF_Last+5)
        {
            DCDC_Service.Vol_Sense_LPF=DCDC_Service.Vol_Sense_LPF_Last+5;
        }
        else if(DCDC_Service.Vol_Sense_LPF<DCDC_Service.Vol_Sense_LPF_Last-5)
        {
            DCDC_Service.Vol_Sense_LPF=DCDC_Service.Vol_Sense_LPF_Last-5;
        }*/
        DCDC_Service.Sum_Vol_Sense=0;
    }

    //DCDC控制状态机
    if(DCDC_Service.Now_Mode==DCDC_CCtoCV)
    {
        Function_DCDC_CCtoCV();
    }
    else if(DCDC_Service.Now_Mode==DCDC_DCCtoDCV)
    {
        Function_DCDC_DCCtoDCV();
    }
    else
    {
        DCDC_Service_BSP();
    }

    //外围驱动采用轮询方式实现
    if(Cnt_Driver==0)
    {
        Cnt_Driver=1;
        W5500_Service_BSP();
    }
    else if(Cnt_Driver==1)
    {
        Cnt_Driver=2;
        SPI_Flash_Service();
    }
    else if(Cnt_Driver==2)
    {
        Cnt_Driver=3;
        //EEPROM_Service();
        //Record_TX();//debug
        if(++Bus_Forward.Cnt_Sample>=4)
        {
            Bus_Forward.Cnt_Sample          = 0;
            Bus_Forward.Vol_Bus_Slow_Last   = (Bus_Forward.Vol_Bus_Slow_Last*255+((int32)Data_AD.F2808_CH2<<9))>>8;//转折频率1.5Hz
            Bus_Forward.Vol_Bus_Slow        = (Bus_Forward.Vol_Bus_Slow_Last)>>9;
        }
    }
    else if(Cnt_Driver==3)
    {
        Cnt_Driver=4;
        SPI_Flash_Service();
    }
    else if(Cnt_Driver==4)
    {
        Cnt_Driver=0;
        Parall_Service();
    }
    EPwm1Regs.ETCLR.all      |=0x01;
    PieCtrlRegs.PIEACK.all    =PIEACK_GROUP3;

}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
#pragma CODE_SECTION(Algorithm_PI,"ramfuncs");
void Algorithm_PI(struct_PI *Ptr)
{
    //误差计算
    Ptr->Error=Ptr->Ref-Ptr->Feed;

    //误差上限
    if(Ptr->Error>Ptr->Limit_Err_Max)
    {Ptr->Error=Ptr->Limit_Err_Max;}

    //误差下限
    if(Ptr->Error<Ptr->Limit_Err_Min)
    {Ptr->Error=Ptr->Limit_Err_Min;}

    //比例输出
    Ptr->Out_P=Ptr->Error*Ptr->Kp;

    //积分输出
    Ptr->Out_I += Ptr->Error*Ptr->Ki;

    //积分输出上限饱和
    if(Ptr->Out_I>Ptr->Limit_PI_max)
    {Ptr->Out_I=Ptr->Limit_PI_max;}

    //积分输出下限饱和
    if(Ptr->Out_I<Ptr->Limit_PI_min)
    {Ptr->Out_I=Ptr->Limit_PI_min;}

    //PI运算叠加
    Ptr->Out_PI=Ptr->Out_P+Ptr->Out_I;

    //PI运算结果缩小后的输出
    Ptr->Out=Ptr->Out_PI>>Ptr->Q_Type;

    //叠加前馈分量
    Ptr->Out=Ptr->Out+Ptr->Forward_Out;

    //输出上限饱和
    if(Ptr->Out>Ptr->Limit_Out_max)
    {Ptr->Out=Ptr->Limit_Out_max;}

    //输出下限饱和
    if(Ptr->Out<Ptr->Limit_Out_min)
    {Ptr->Out=Ptr->Limit_Out_min;}
}

//*********************************************************************************
//Description: Buck模式同步整流管PWM控制
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
#pragma CODE_SECTION(Fault_OC_Check,"ramfuncs");
void Fault_OC_Check(void)
{
    int32 Data_A,Data_B;
    //低电流档位预留量程较小容易误触发，不做过流检测
    //if(DCDC_Service.Flag_CurRange==CUR_RANGE_L) return;

    if((DCDC_Service.Fault_Module&(FAULT_OC_CHA|FAULT_OC_CHB))!=0)
    {return;}

    Data_A=Data_AD.AD7606_CH7;
    Data_B=Data_AD.AD7606_CH6;
    if(Data_A<0)
    {
        Data_A=-Data_A;
    }

    if(Data_B<0)
    {
        Data_B=-Data_B;
    }

    if(Data_A>DCDC_Service.Cur_Fast_OC_AD)
    {
      if(++CH_A.Cnt_Out_OC>=5)
      {
         DCDC_Service.Fault_Module|=FAULT_OC_CHA;
      }
    }
    else
    {
        CH_A.Cnt_Out_OC=0;
    }

    if(Data_B>DCDC_Service.Cur_Fast_OC_AD)
    {
      if(++CH_B.Cnt_Out_OC>=5)
      {
         DCDC_Service.Fault_Module|=FAULT_OC_CHB;
      }
    }
    else
    {
        CH_B.Cnt_Out_OC=0;
    }

    if((DCDC_Service.Fault_Module&(FAULT_OC_CHA|FAULT_OC_CHB))!=0)
    {
        PWM1OFF;
        PWM2OFF;
        DCDC_Service.Now_Mode=DCDC_PWM_OFF;
    }
}

//*********************************************************************************
//Description: Buck模式同步整流管PWM控制
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
#pragma CODE_SECTION(SynRect_Buck,"ramfuncs");
void SynRect_Buck(struct_Channel *Ptr)
{
    if(Ptr->EN_SynRect==TRUE)
    {
        if(Ptr->Flag_SynRect==TRUE)
        {
        	if(Ptr->PWMxA>=150)
        	{
        		Ptr->PWMxB=Ptr->PWMxA;
        	}
        	else
        	{
        		Ptr->PWMxB=150;
        	}
        }
        else
        {
            if(Ptr->Cnt_SynRect>=3000)
            {
                if(++Ptr->Cnt_SynRect>=3020)
                {
                    Ptr->Cnt_SynRect=3000;
                    if(Ptr->PWMxB>Ptr->PWMxA)
                    {
                        Ptr->PWMxB=Ptr->PWMxB-1;
                        if(Ptr->PWMxB<=150)
                        {
                        	Ptr->PWMxB=150;
                        	Ptr->Flag_SynRect=TRUE;
                        }
                    }
                    else
                    {
                        Ptr->Flag_SynRect=TRUE;
                        Ptr->PWMxB=Ptr->PWMxA;
                    }
                }
            }
            else
            {
                ++Ptr->Cnt_SynRect;
            }
        }
    }
    else
    {
        Ptr->Cnt_SynRect=0;
        Ptr->Flag_SynRect=FALSE;
        Ptr->PWMxB=PWM_PERIOD-PWM_DEADTIME;
    }
}

//*********************************************************************************
//Description: Boost模式同步整流管PWM控制
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
#pragma CODE_SECTION(SynRect_Boost,"ramfuncs");
void SynRect_Boost(struct_Channel *Ptr)
{
    if(Ptr->EN_SynRect==TRUE)
     {
         if(Ptr->Flag_SynRect==TRUE)
         {
        	 if(Ptr->PWMxB<1850)
        	 {
        		 Ptr->PWMxA=Ptr->PWMxB;
             }
             else
             {
            	 Ptr->PWMxA=1850;
             }
         }
         else
         {
             if(Ptr->Cnt_SynRect>=3000)
             {
                 if(++Ptr->Cnt_SynRect>=3020)
                 {
                     Ptr->Cnt_SynRect=3000;
                     if(Ptr->PWMxA<Ptr->PWMxB)
                     {
                         Ptr->PWMxA=Ptr->PWMxA+1;
                         if(Ptr->PWMxA>=1850)
                         {
                              Ptr->PWMxA=1850;
                              Ptr->Flag_SynRect=TRUE;
                         }
                     }
                     else
                     {
                         Ptr->Flag_SynRect=TRUE;
                         Ptr->PWMxA=Ptr->PWMxB;
                     }
                 }
             }
             else
             {
                 ++Ptr->Cnt_SynRect;
             }
         }
     }
     else
     {
         Ptr->Cnt_SynRect=0;
         Ptr->Flag_SynRect=FALSE;
         Ptr->PWMxA=PWM_DEADTIME;
     }
}

void InitSPIB_BSP(void)
{
    // Initialize SPI-B
    SpibRegs.SPICCR.bit.SPISWRESET=0;

    SpibRegs.SPICCR.all =0x4F;  // 空闲时，CLK=1 Reset on, rising edge, 16-bit char bits
    SpibRegs.SPICTL.all =0x06;  // Enable master mode, normal phase,enable talk, and SPI int enabled.
    SpibRegs.SPIBRR =0x06;      //波特率 LSPCLK/(SPIBRR+1) 20M   LSPCLK=100M

    SpibRegs.SPIFFTX.all=0xC008;
    SpibRegs.SPIFFRX.all=0x0008;
    SpibRegs.SPIFFCT.all=0x1;
    SpibRegs.SPIPRI.all=0x0010;

    SpibRegs.SPICCR.bit.SPISWRESET=1;         // Relinquish SPI from Reset
    SpibRegs.SPIFFTX.bit.TXFIFO=1;
    SpibRegs.SPIFFRX.bit.RXFIFORESET=1;
}

//*********************************************************************************
//Description: DCDC服务状态机
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
#pragma CODE_SECTION(Function_DCDC_CCtoCV,"ramfuncs");
void Function_DCDC_CCtoCV(void)
{
    if(DCDC_Service.Time_Running<100000)
    {
        ++DCDC_Service.Time_Running;
    }
    else
    {
        if(DCDC_Service.Time_Running==100000)
        {
            DCDC_Service.Cnt_CCtoCV         = 0;
            DCDC_Service.Num_CCtoCV         = 1000;
            DCDC_Service.Cnt_Sum_Vol_Sense  = 0;
            DCDC_Service.Num_Sum_Vol_Sense  = 1000;
            DCDC_Service.Sum_Vol_Sense      = 0;

            PI_Vol_Charge.Ki                = PI_Vol_Charge_Ki;
            PI_Vol_Charge.Kp                = 0;
            PI_Vol_Charge.Period            = 1000;
            PI_Vol_Charge.Limit_Err_Max     = 25;
            PI_Vol_Charge.Limit_Err_Min     =-25;
            DCDC_Service.Time_Running       = 100001;
        }
    }


    //恒流阶段
        if(DCDC_Service.Flag_CV_Step==FALSE)
        {
            //请求刷新设定电流，更新A,B通道目标电流
            if(Interface_DCDC.Req_Refresh_Cur==TRUE)
            {
                DCDC_Service.Set_Cur_Total=Interface_DCDC.Req_Cur_Total;
                CH_A.Set_Cur = (DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
                CH_B.Set_Cur = CH_A.Set_Cur;
                Interface_DCDC.Req_Refresh_Cur=FALSE;
            }

            //A通道电流给定缓升缓降
            if(CH_A_PI_Cur_Charge.Ref<CH_A.Set_Cur)
            {
                CH_A_PI_Cur_Charge.Ref=CH_A_PI_Cur_Charge.Ref+CH_A.Slew_Cur_ON;
                if(CH_A_PI_Cur_Charge.Ref>CH_A.Set_Cur)
                {CH_A_PI_Cur_Charge.Ref=CH_A.Set_Cur;}

                if(CH_A_PI_Cur_Charge.Ref>CH_A.Limit_Set_Cur)
                {CH_A_PI_Cur_Charge.Ref=CH_A.Limit_Set_Cur;}
            }
            else if(CH_A_PI_Cur_Charge.Ref>CH_A.Set_Cur)
            {
                CH_A_PI_Cur_Charge.Ref=CH_A_PI_Cur_Charge.Ref-CH_A.Slew_Cur_OFF;
                if(CH_A_PI_Cur_Charge.Ref<CH_A.Set_Cur)
                {CH_A_PI_Cur_Charge.Ref=CH_A.Set_Cur;}

                if(CH_A_PI_Cur_Charge.Ref<0)
                {CH_A_PI_Cur_Charge.Ref=0;}
            }

            //B通道目标电流给定与A通道相同
            CH_B_PI_Cur_Charge.Ref=CH_A_PI_Cur_Charge.Ref;

            //检测恒流转恒压条件是否达到,只允许单机模式和并机的主机模式检测
            if((DCDC_Service.Mode_Parall==SINGLE)||(DCDC_Service.Mode_Parall==PARALL_MASTER))
            {
                if((DCDC_Service.Now_Vol_Sense>=DCDC_Service.Set_Vol)||(Data_Sum.Vol_Sense >= DCDC_Service.Set_Vol))
                {
                    if(DCDC_Service.Vol_Sense_LPF>=DCDC_Service.Set_Vol)
                    {
                        DCDC_Service.Cnt_CCtoCV++;
                    }
                    else
                    {
                        DCDC_Service.Cnt_CCtoCV = 0;
                    }
                    if(Data_Sum.Vol_Sense>=DCDC_Service.Set_Vol)
                    {
                        DCDC_Service.Cnt_CCtoCV2++;
                    }
                    else
                    {
                        DCDC_Service.Cnt_CCtoCV2 = 0;
                    }
                    if((DCDC_Service.Cnt_CCtoCV>=DCDC_Service.Num_CCtoCV)||(DCDC_Service.Cnt_CCtoCV2 >= 10000))
                    {
                        DCDC_Service.Flag_CV_Step   = TRUE;
                        DCDC_Service.Flag_CV_Step2   = TRUE;
                        WorkStep_Service.CC_Ah      = WorkStep_Service.Step_Ah;
                        DCDC_Service.Cnt_CCtoCV     = 0;
                        PI_Vol_Charge.Cnt           = 0;
                        //CCtoCV前电压环积分值初始化以与CC模式衔接起来，避免电流跳变
                        PI_Vol_Charge.Out_I=((CH_A_PI_Cur_Charge.Ref<<1)*DCDC_Service.Parall_Total)<<PI_Vol_Charge.Q_Type;
                        //PI_Vol_Charge.Ki  = PI_Vol_Charge_Ki;
                        Rate_Cur_Compute.Cnt_Delay=0;
                        Rate_Cur_Compute.Cnt_Error=0;
                        DCDC_Service.Cnt_Time_CV_DCV=0;

                        CH_A.Req_FastStart          =FALSE;
                        CH_B.Req_FastStart          =FALSE;
                        CH_A.Req_FastStart_DCM      =FALSE;
                        CH_B.Req_FastStart_DCM      =FALSE;
                        //DCDC_Service.Vol_Sense_LPF_Last=DCDC_Service.Now_Vol_Sense;
                        //DCDC_Service.Vol_Sense_LPF     =DCDC_Service.Now_Vol_Sense;
                    }
                }
                else
                {
                    DCDC_Service.Cnt_CCtoCV=0;
                    DCDC_Service.Cnt_CCtoCV2 = 0;
                }
            }
        }
        else
        {
            //充电恒压环PI运算调用周期计数
            if(++PI_Vol_Charge.Cnt>=PI_Vol_Charge.Period)
            {
                PI_Vol_Charge.Cnt   = 0;
                PI_Vol_Charge.Ref   = DCDC_Service.Set_Vol;
                PI_Vol_Charge.Feed  = DCDC_Service.Vol_Sense_LPF;
                Algorithm_PI(&PI_Vol_Charge);
                DCDC_Service.Set_Cur_Total    = PI_Vol_Charge.Out;

                if(DCDC_Service.Flag_SingleCH==FALSE)
                {
                    CH_A_PI_Cur_Charge.Ref   = (DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
                }
                else
                {
                    CH_A_PI_Cur_Charge.Ref   = DCDC_Service.Set_Cur_Total;
                }

                if(CH_A_PI_Cur_Charge.Ref>CH_A.Limit_Set_Cur)
                {
                    CH_A_PI_Cur_Charge.Ref=CH_A.Limit_Set_Cur;
                }
                else if(CH_A_PI_Cur_Charge.Ref<0)
                {
                    CH_A_PI_Cur_Charge.Ref=0;
                }
                CH_B_PI_Cur_Charge.Ref=CH_A_PI_Cur_Charge.Ref;

                //主机发送电流设定数据给从机
                if(DCDC_Service.Mode_Parall==PARALL_MASTER)
                {
                    Parall_Master.ID1_To_Sx.ID1_H.Set_Cur_Total       = DCDC_Service.Set_Cur_Total;
                    Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Refresh_Cur = TRUE;
                    Parall_Master.ID1_To_Sx.Req_TX                    = TRUE;
                    Parall_Master.Cnt_Period                          = 0;
                }
            }
        }

        //A通道充电电流环运算与控制
        CH_A_PI_Cur_Charge.Feed  = CH_A.Now_Cur;

        //快速启动请求处理
        if(CH_A.Req_FastStart==TRUE)
        {
      	  CH_A_PI_Cur_Charge.Out_I += ((int32)8<<CH_A_PI_Cur_Charge.Q_Type);

          if((CH_A.PWMxA>=DCDC_Service.PWM_Ready+80)||(++CH_A.Cnt_FastStart>500))
          {
        	  CH_A.Req_FastStart=FALSE;
        	  //CH_A.Slew_Cur_ON=CUR_SLEW_DEFAULT;
          }
        }
        else if(CH_A.Req_FastStart_DCM==TRUE)
        {
            CH_A_PI_Cur_Charge.Out_I += ((int32)1<<CH_A_PI_Cur_Charge.Q_Type);

            if((CH_A.Now_Cur+500>=CH_A.Set_Cur)||(++CH_A.Cnt_FastStart>650)||(CH_A.PWMxA>=DCDC_Service.PWM_Ready))
            {
                CH_A.Req_FastStart_DCM=FALSE;
                //CH_A.Slew_Cur_ON=CUR_SLEW_DEFAULT;
            }
        }

        CH_A_PI_Cur_Charge.Forward_Out = (-Bus_Forward.PWM_Forward*CH_A_PI_Cur_Charge.Out)>>13;

        if((CH_A_PI_Cur_Charge.Forward_Out==1)||(CH_A_PI_Cur_Charge.Forward_Out==-1))
        {
            CH_A_PI_Cur_Charge.Forward_Out=0;
        }

        Algorithm_PI(CH_A.Ptr_PI_Cur_Charge);

        CH_A.PWMxA    = CH_A_PI_Cur_Charge.Out;

        //充电同步整流控制
        SynRect_Buck(&CH_A);

        //B通道充电电流环运算与控制
        CH_B_PI_Cur_Charge.Feed  = CH_B.Now_Cur;

        //快速启动请求处理
        if(CH_B.Req_FastStart==TRUE)
        {
        	CH_B_PI_Cur_Charge.Out_I += ((int32)8<<CH_B_PI_Cur_Charge.Q_Type);

            if((CH_B.PWMxA>=DCDC_Service.PWM_Ready+80)||(++CH_B.Cnt_FastStart>500))
            {
          	  CH_B.Req_FastStart=FALSE;
        	  //CH_B.Slew_Cur_ON=CUR_SLEW_DEFAULT;
            }
        }
        else if(CH_B.Req_FastStart_DCM==TRUE)
        {
            CH_B_PI_Cur_Charge.Out_I += ((int32)1<<CH_B_PI_Cur_Charge.Q_Type);

            if((CH_B.Now_Cur+500>=CH_B.Set_Cur)||(++CH_B.Cnt_FastStart>650)||(CH_B.PWMxA>=DCDC_Service.PWM_Ready))
            {
                CH_B.Req_FastStart_DCM=FALSE;
                //CH_B.Slew_Cur_ON=CUR_SLEW_DEFAULT;
            }
        }

        CH_B_PI_Cur_Charge.Forward_Out = (-Bus_Forward.PWM_Forward*CH_B_PI_Cur_Charge.Out)>>13;

        if((CH_B_PI_Cur_Charge.Forward_Out==1)||(CH_B_PI_Cur_Charge.Forward_Out==-1))
        {
            CH_B_PI_Cur_Charge.Forward_Out=0;
        }

        Algorithm_PI(CH_B.Ptr_PI_Cur_Charge);

        CH_B.PWMxA    = CH_B_PI_Cur_Charge.Out;

        //充电同步整流控制
        SynRect_Buck(&CH_B);

        EPwm1Regs.CMPA.half.CMPA  =CH_A.PWMxA;
        EPwm1Regs.CMPB            =CH_A.PWMxB;
        EPwm2Regs.CMPA.half.CMPA  =CH_B.PWMxA;
        EPwm2Regs.CMPB            =CH_B.PWMxB;

        if(Interface_DCDC.Req_ChangeMode==TRUE)
        {
            DCDC_Service.Now_Mode=DCDC_SOFTOFF_CH;
        }

        if(DCDC_Service.Fault_Module!=0)
        {
            DCDC_Service.Now_Mode=DCDC_SOFTOFF_CH;
        }
}

//*********************************************************************************
//Description: DCDC服务状态机
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
#pragma CODE_SECTION(Function_DCDC_DCCtoDCV,"ramfuncs");
void Function_DCDC_DCCtoDCV(void)
{
    if(DCDC_Service.Time_Running<100000)
    {
        ++DCDC_Service.Time_Running;
    }
    else
    {
        if(DCDC_Service.Time_Running==100000)
        {
            DCDC_Service.Cnt_DCCtoDCV     = 0;
            DCDC_Service.Num_DCCtoDCV     = 1000;
            DCDC_Service.Cnt_Sum_Vol_Sense= 0;
            DCDC_Service.Num_Sum_Vol_Sense= 1000;
            DCDC_Service.Sum_Vol_Sense    = 0;

            PI_Vol_Discharge.Ki           = PI_Vol_Discharge_Ki;
            PI_Vol_Discharge.Kp           = 0;
            PI_Vol_Discharge.Period       = 1000;
            PI_Vol_Discharge.Limit_Err_Max= 25;
            PI_Vol_Discharge.Limit_Err_Min=-25;

            DCDC_Service.Time_Running=100001;
        }
    }


        if(DCDC_Service.Flag_DCV_Step==FALSE)
        {
            //请求刷新设定电流，更新A,B通道目标电流
            if(Interface_DCDC.Req_Refresh_Cur==TRUE)
            {
                DCDC_Service.Set_Cur_Total=Interface_DCDC.Req_Cur_Total;
                CH_A.Set_Cur = (DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
                CH_B.Set_Cur = CH_A.Set_Cur;
                Interface_DCDC.Req_Refresh_Cur=FALSE;
            }

            //A通道电流给定缓升缓降
            if(CH_A_PI_Cur_Discharge.Ref<CH_A.Set_Cur)
            {
                CH_A_PI_Cur_Discharge.Ref=CH_A_PI_Cur_Discharge.Ref+CH_A.Slew_Cur_ON;
                if(CH_A_PI_Cur_Discharge.Ref>CH_A.Set_Cur)
                {CH_A_PI_Cur_Discharge.Ref=CH_A.Set_Cur;}

                if(CH_A_PI_Cur_Discharge.Ref>CH_A.Limit_Set_Cur)
                {CH_A_PI_Cur_Discharge.Ref=CH_A.Limit_Set_Cur;}
            }
            else if(CH_A_PI_Cur_Discharge.Ref>CH_A.Set_Cur)
            {
                CH_A_PI_Cur_Discharge.Ref=CH_A_PI_Cur_Discharge.Ref-CH_A.Slew_Cur_OFF;
                if(CH_A_PI_Cur_Discharge.Ref<CH_A.Set_Cur)
                {CH_A_PI_Cur_Discharge.Ref=CH_A.Set_Cur;}

                if(CH_A_PI_Cur_Discharge.Ref<0)
                {CH_A_PI_Cur_Discharge.Ref=0;}
            }

            //B通道电流给定与A通道相同
            CH_B_PI_Cur_Discharge.Ref=CH_A_PI_Cur_Discharge.Ref;

            //检测恒流转恒压条件是否达到,只允许单机模式和并机的主机模式检测
            if((DCDC_Service.Mode_Parall==SINGLE)||(DCDC_Service.Mode_Parall==PARALL_MASTER))
            {
                if((DCDC_Service.Now_Vol_Sense<=DCDC_Service.Set_Vol)||(Data_Sum.Vol_Sense<=DCDC_Service.Set_Vol))
                {
                    if(DCDC_Service.Vol_Sense_LPF<=DCDC_Service.Set_Vol)
                    {
                        DCDC_Service.Cnt_DCCtoDCV++;
                    }
                    else
                    {
                        DCDC_Service.Cnt_DCCtoDCV = 0;
                    }
                    if(Data_Sum.Vol_Sense<=DCDC_Service.Set_Vol)
                    {
                        DCDC_Service.Cnt_DCCtoDCV2++;
                    }
                    else
                    {
                        DCDC_Service.Cnt_DCCtoDCV2 = 0;
                    }
                   // if(DCDC_Service.Cnt_DCCtoDCV>=2500)
                    if((DCDC_Service.Cnt_DCCtoDCV>=DCDC_Service.Num_DCCtoDCV)||(DCDC_Service.Cnt_DCCtoDCV2 >= 10000))
                    {
                        DCDC_Service.Flag_DCV_Step  = TRUE;
                        DCDC_Service.Flag_DCV_Step2 = TRUE;
                        DCDC_Service.Cnt_DCCtoDCV   = 0;
                        DCDC_Service.Cnt_DCCtoDCV2  = 0;
                        WorkStep_Service.DCC_Ah     = WorkStep_Service.Step_Ah;
                        PI_Vol_Discharge.Cnt        = 0;
                        //DCCtoDCV前电压环积分值初始化以与DCC模式衔接起来，避免电流跳变
                        PI_Vol_Discharge.Out_I      =((CH_A_PI_Cur_Discharge.Ref<<1)*DCDC_Service.Parall_Total)<<PI_Vol_Discharge.Q_Type;
                        //PI_Vol_Discharge.Ki         = PI_Vol_Discharge_Ki;
                        Rate_Cur_Compute.Cnt_Delay  = 0;
                        Rate_Cur_Compute.Cnt_Error  = 0;
                        DCDC_Service.Cnt_Time_CV_DCV=0;

                        CH_A.Req_FastStart          =FALSE;
                        CH_B.Req_FastStart          =FALSE;
                        CH_A.Req_FastStart_DCM      =FALSE;
                        CH_B.Req_FastStart_DCM      =FALSE;
                        //DCDC_Service.Vol_Sense_LPF_Last=DCDC_Service.Now_Vol_Sense;
                        //DCDC_Service.Vol_Sense_LPF     =DCDC_Service.Now_Vol_Sense;
                    }
                }
                else
                {
                    DCDC_Service.Cnt_DCCtoDCV=0;
                    DCDC_Service.Cnt_DCCtoDCV2=0;
                }
            }
        }
        else
        {
            //放电恒压环PI运算调用周期计数
            if(++PI_Vol_Discharge.Cnt>=PI_Vol_Discharge.Period)
            {
                PI_Vol_Discharge.Cnt=0;
                PI_Vol_Discharge.Feed=DCDC_Service.Set_Vol;
                PI_Vol_Discharge.Ref=DCDC_Service.Vol_Sense_LPF;
                Algorithm_PI(&PI_Vol_Discharge);
                DCDC_Service.Set_Cur_Total = PI_Vol_Discharge.Out;

                if(DCDC_Service.Flag_SingleCH==FALSE)
                {
                    CH_A_PI_Cur_Discharge.Ref=(DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
                }
                else
                {
                    CH_A_PI_Cur_Discharge.Ref=DCDC_Service.Set_Cur_Total;
                }

                if(CH_A_PI_Cur_Discharge.Ref>CH_A.Limit_Set_Cur)
                {
                    CH_A_PI_Cur_Discharge.Ref=CH_A.Limit_Set_Cur;
                }
                else if(CH_A_PI_Cur_Discharge.Ref<0)
                {
                    CH_A_PI_Cur_Discharge.Ref=0;
                }
                CH_B_PI_Cur_Discharge.Ref=CH_A_PI_Cur_Discharge.Ref;
                //主机发送电流设定数据给从机
                if(DCDC_Service.Mode_Parall==PARALL_MASTER)
                {
                    Parall_Master.ID1_To_Sx.ID1_H.Set_Cur_Total       = DCDC_Service.Set_Cur_Total;
                    Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Refresh_Cur = TRUE;
                    Parall_Master.ID1_To_Sx.Req_TX                    = TRUE;
                    Parall_Master.Cnt_Period                          = 0;
                }
            }
        }

        //A通道放电电流环运算与控制
        CH_A_PI_Cur_Discharge.Feed=-CH_A.Now_Cur;

        //快速启动请求处理
        if(CH_A.Req_FastStart==TRUE)
        {
        	CH_A_PI_Cur_Discharge.Out_I += ((int32)20<<CH_A_PI_Cur_Discharge.Q_Type);

            if((CH_A.PWMxB<=DCDC_Service.PWM_Ready)||(++CH_A.Cnt_FastStart>500))
            {
            	CH_A.Req_FastStart=FALSE;
            	//CH_A.Slew_Cur_ON=CUR_SLEW_DEFAULT;
            }
        }
        else if(CH_A.Req_FastStart_DCM==TRUE)
        {
            CH_A_PI_Cur_Discharge.Out_I += ((int32)3<<CH_A_PI_Cur_Discharge.Q_Type);

            if(((-CH_A.Now_Cur)+500>=CH_A.Set_Cur)||(++CH_A.Cnt_FastStart>600)||(CH_A.PWMxB<=DCDC_Service.PWM_Ready))
            {
                CH_A.Req_FastStart_DCM=FALSE;
                //CH_A.Slew_Cur_ON=CUR_SLEW_DEFAULT;
            }
        }

        CH_A_PI_Cur_Discharge.Forward_Out = (Bus_Forward.PWM_Forward*(PWM_PERIOD-CH_A_PI_Cur_Discharge.Out))>>13;

        if((CH_A_PI_Cur_Discharge.Forward_Out==1)||(CH_A_PI_Cur_Discharge.Forward_Out==-1))
        {
            CH_A_PI_Cur_Discharge.Forward_Out=0;
        }

        Algorithm_PI(&CH_A_PI_Cur_Discharge);

        CH_A.PWMxB=PWM_PERIOD-CH_A_PI_Cur_Discharge.Out;

        //放电同步整流控制
        SynRect_Boost(&CH_A);

        //B通道放电电流环运算与控制
        CH_B_PI_Cur_Discharge.Feed=-CH_B.Now_Cur;

        //快速启动请求处理
        if(CH_B.Req_FastStart==TRUE)
        {
        	CH_B_PI_Cur_Discharge.Out_I += ((int32)20<<CH_B_PI_Cur_Discharge.Q_Type);

            if((CH_B.PWMxB<=DCDC_Service.PWM_Ready)||(++CH_B.Cnt_FastStart>500))
            {
            	CH_B.Req_FastStart=FALSE;
            	//CH_B.Slew_Cur_ON=CUR_SLEW_DEFAULT;
            }
        }
        else if(CH_B.Req_FastStart_DCM==TRUE)
        {
            CH_B_PI_Cur_Discharge.Out_I += ((int32)3<<CH_B_PI_Cur_Discharge.Q_Type);

            if(((-CH_B.Now_Cur)+500>=CH_B.Set_Cur)||(++CH_B.Cnt_FastStart>600)||(CH_B.PWMxB<=DCDC_Service.PWM_Ready))
            {
                CH_B.Req_FastStart_DCM=FALSE;
                //CH_B.Slew_Cur_ON=CUR_SLEW_DEFAULT;
            }
        }

        CH_B_PI_Cur_Discharge.Forward_Out = (Bus_Forward.PWM_Forward*(PWM_PERIOD-CH_B_PI_Cur_Discharge.Out))>>13;

        if((CH_B_PI_Cur_Discharge.Forward_Out==1)||(CH_B_PI_Cur_Discharge.Forward_Out==-1))
        {
            CH_B_PI_Cur_Discharge.Forward_Out=0;
        }

        Algorithm_PI(&CH_B_PI_Cur_Discharge);

        CH_B.PWMxB=PWM_PERIOD-CH_B_PI_Cur_Discharge.Out;

        //放电同步整流控制
        SynRect_Boost(&CH_B);

        EPwm1Regs.CMPA.half.CMPA  =CH_A.PWMxA;
        EPwm1Regs.CMPB            =CH_A.PWMxB;
        EPwm2Regs.CMPA.half.CMPA  =CH_B.PWMxA;
        EPwm2Regs.CMPB            =CH_B.PWMxB;

        if(Interface_DCDC.Req_ChangeMode==TRUE)
        {
            DCDC_Service.Now_Mode=DCDC_SOFTOFF_DISCH;
        }

        if(DCDC_Service.Fault_Module!=0)
        {
            DCDC_Service.Now_Mode=DCDC_SOFTOFF_DISCH;
        }
}


//*********************************************************************************
//Description: DCDC服务状态机
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
//#pragma CODE_SECTION(DCDC_Service_BSP,"ramfuncs");
void DCDC_Service_BSP(void)
{
    switch(DCDC_Service.Now_Mode)
    {
    case DCDC_CC:
  {
      //请求刷新设定电流，更新A,B通道目标电流
      if(Interface_DCDC.Req_Refresh_Cur==TRUE)
      {
          DCDC_Service.Set_Cur_Total=Interface_DCDC.Req_Cur_Total;
          CH_A.Set_Cur = (DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
          CH_B.Set_Cur = CH_A.Set_Cur;
          Interface_DCDC.Req_Refresh_Cur=FALSE;
      }

      //A通道电流给定缓升缓降
      if(CH_A_PI_Cur_Charge.Ref<CH_A.Set_Cur)
      {
          CH_A_PI_Cur_Charge.Ref   = CH_A_PI_Cur_Charge.Ref+CH_A.Slew_Cur_ON;
          if(CH_A_PI_Cur_Charge.Ref>CH_A.Set_Cur)
          {CH_A_PI_Cur_Charge.Ref  = CH_A.Set_Cur;}

          if(CH_A_PI_Cur_Charge.Ref>CH_A.Limit_Set_Cur)
          {CH_A_PI_Cur_Charge.Ref  = CH_A.Limit_Set_Cur;}
      }
      else if(CH_A_PI_Cur_Charge.Ref>CH_A.Set_Cur)
      {
          CH_A_PI_Cur_Charge.Ref   = CH_A_PI_Cur_Charge.Ref-CH_A.Slew_Cur_OFF;
          if(CH_A_PI_Cur_Charge.Ref<CH_A.Set_Cur)
          {CH_A_PI_Cur_Charge.Ref  = CH_A.Set_Cur;}

          if(CH_A_PI_Cur_Charge.Ref<0)
          {CH_A_PI_Cur_Charge.Ref  = 0;}
      }

      //B通道目标电流给定与A通道相同
      CH_B_PI_Cur_Charge.Ref   = CH_A_PI_Cur_Charge.Ref;

      //A通道充电电流环运算
      CH_A_PI_Cur_Charge.Feed  = CH_A.Now_Cur;

      //快速启动请求处理
      if(CH_A.Req_FastStart==TRUE)
      {
      	  CH_A_PI_Cur_Charge.Out_I += ((int32)8<<CH_A_PI_Cur_Charge.Q_Type);

          if((CH_A.PWMxA>=DCDC_Service.PWM_Ready+80)||(++CH_A.Cnt_FastStart>500))
          {
        	  CH_A.Req_FastStart=FALSE;
        	  //CH_A.Slew_Cur_ON=CUR_SLEW_DEFAULT;
          }
      }
      else if(CH_A.Req_FastStart_DCM==TRUE)
      {
          CH_A_PI_Cur_Charge.Out_I += ((int32)1<<CH_A_PI_Cur_Charge.Q_Type);

          if((CH_A.Now_Cur+500>=CH_A.Set_Cur)||(++CH_A.Cnt_FastStart>650)||(CH_A.PWMxA>=DCDC_Service.PWM_Ready))
          {
              CH_A.Req_FastStart_DCM=FALSE;
              //CH_A.Slew_Cur_ON=CUR_SLEW_DEFAULT;
          }
      }

      CH_A_PI_Cur_Charge.Forward_Out = (-Bus_Forward.PWM_Forward*CH_A_PI_Cur_Charge.Out)>>13;

      if((CH_A_PI_Cur_Charge.Forward_Out==1)||(CH_A_PI_Cur_Charge.Forward_Out==-1))
      {
          CH_A_PI_Cur_Charge.Forward_Out=0;
      }

      Algorithm_PI(CH_A.Ptr_PI_Cur_Charge);

      CH_A.PWMxA    = CH_A_PI_Cur_Charge.Out;

      //充电同步整流控制
      SynRect_Buck(&CH_A);

      //B通道充电电流环运算
      CH_B_PI_Cur_Charge.Feed  = CH_B.Now_Cur;

      //快速启动请求处理
      if(CH_B.Req_FastStart==TRUE)
      {
      	  CH_B_PI_Cur_Charge.Out_I += ((int32)8<<CH_B_PI_Cur_Charge.Q_Type);

          if((CH_B.PWMxA>=DCDC_Service.PWM_Ready+80)||(++CH_B.Cnt_FastStart>500))
          {
        	  CH_B.Req_FastStart=FALSE;
        	  //CH_B.Slew_Cur_ON=CUR_SLEW_DEFAULT;
          }
      }
      else if(CH_B.Req_FastStart_DCM==TRUE)
      {
          CH_B_PI_Cur_Charge.Out_I += ((int32)1<<CH_B_PI_Cur_Charge.Q_Type);

          if((CH_B.Now_Cur+500>=CH_B.Set_Cur)||(++CH_B.Cnt_FastStart>650)||(CH_B.PWMxA>=DCDC_Service.PWM_Ready))
          {
              CH_B.Req_FastStart_DCM=FALSE;
              //CH_B.Slew_Cur_ON=CUR_SLEW_DEFAULT;
          }
      }

      CH_B_PI_Cur_Charge.Forward_Out = (-Bus_Forward.PWM_Forward*CH_B_PI_Cur_Charge.Out)>>13;

      if((CH_B_PI_Cur_Charge.Forward_Out==1)||(CH_B_PI_Cur_Charge.Forward_Out==-1))
      {
          CH_B_PI_Cur_Charge.Forward_Out=0;
      }

      Algorithm_PI(CH_B.Ptr_PI_Cur_Charge);

      CH_B.PWMxA    = CH_B_PI_Cur_Charge.Out;

      //充电同步整流控制
      SynRect_Buck(&CH_B);

      //PWM值输出到ePWM模块
      EPwm1Regs.CMPA.half.CMPA  =CH_A.PWMxA;
      EPwm1Regs.CMPB            =CH_A.PWMxB;
      EPwm2Regs.CMPA.half.CMPA  =CH_B.PWMxA;
      EPwm2Regs.CMPB            =CH_B.PWMxB;

      if(Interface_DCDC.Req_ChangeMode==TRUE)
      {
          DCDC_Service.Now_Mode=DCDC_SOFTOFF_CH;
      }

      if(DCDC_Service.Fault_Module!=0)
      {
          DCDC_Service.Now_Mode=DCDC_SOFTOFF_CH;
      }
          break;}

  case DCDC_DCC:
  {
      //请求刷新设定电流，更新A,B通道目标电流
      if(Interface_DCDC.Req_Refresh_Cur==TRUE)
      {
          DCDC_Service.Set_Cur_Total=Interface_DCDC.Req_Cur_Total;
          CH_A.Set_Cur = (DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
          CH_B.Set_Cur = CH_A.Set_Cur;
          Interface_DCDC.Req_Refresh_Cur=FALSE;
      }

      //A通道电流给定缓升缓降
      if(CH_A.Ptr_PI_Cur_Discharge->Ref<CH_A.Set_Cur)
      {
          CH_A.Ptr_PI_Cur_Discharge->Ref=CH_A.Ptr_PI_Cur_Discharge->Ref+CH_A.Slew_Cur_ON;
          if(CH_A.Ptr_PI_Cur_Discharge->Ref>CH_A.Set_Cur)
          {CH_A.Ptr_PI_Cur_Discharge->Ref=CH_A.Set_Cur;}

          if(CH_A.Ptr_PI_Cur_Discharge->Ref>CH_A.Limit_Set_Cur)
          {CH_A.Ptr_PI_Cur_Discharge->Ref=CH_A.Limit_Set_Cur;}
      }
      else if(CH_A.Ptr_PI_Cur_Discharge->Ref>CH_A.Set_Cur)
      {
          CH_A.Ptr_PI_Cur_Discharge->Ref=CH_A.Ptr_PI_Cur_Discharge->Ref-CH_A.Slew_Cur_OFF;
          if(CH_A.Ptr_PI_Cur_Discharge->Ref<CH_A.Set_Cur)
          {CH_A.Ptr_PI_Cur_Discharge->Ref=CH_A.Set_Cur;}

          if(CH_A.Ptr_PI_Cur_Discharge->Ref<0)
          {CH_A.Ptr_PI_Cur_Discharge->Ref=0;}
      }

      //B通道电流给定与A通道相同
      CH_B.Ptr_PI_Cur_Discharge->Ref=CH_A.Ptr_PI_Cur_Discharge->Ref;

      //A通道放电电流环运算
      CH_A.Ptr_PI_Cur_Discharge->Feed=-CH_A.Now_Cur;

      //快速启动请求处理
      if(CH_A.Req_FastStart==TRUE)
      {
    	  CH_A_PI_Cur_Discharge.Out_I += ((int32)20<<CH_A_PI_Cur_Discharge.Q_Type);

          if((CH_A.PWMxB<=DCDC_Service.PWM_Ready)||(++CH_A.Cnt_FastStart>500))
          {
          	CH_A.Req_FastStart=FALSE;
          	//CH_A.Slew_Cur_ON=CUR_SLEW_DEFAULT;
          }
      }
      else if(CH_A.Req_FastStart_DCM==TRUE)
      {
          CH_A_PI_Cur_Discharge.Out_I += ((int32)3<<CH_A_PI_Cur_Discharge.Q_Type);

          if(((-CH_A.Now_Cur)+500>=CH_A.Set_Cur)||(++CH_A.Cnt_FastStart>600)||(CH_A.PWMxB<=DCDC_Service.PWM_Ready))
          {
              CH_A.Req_FastStart_DCM=FALSE;
              //CH_A.Slew_Cur_ON=CUR_SLEW_DEFAULT;
          }
      }

      CH_A_PI_Cur_Discharge.Forward_Out=(Bus_Forward.PWM_Forward*(PWM_PERIOD-CH_A_PI_Cur_Discharge.Out))>>13;

      if((CH_A_PI_Cur_Discharge.Forward_Out==1)||(CH_A_PI_Cur_Discharge.Forward_Out==-1))
      {
          CH_A_PI_Cur_Discharge.Forward_Out=0;
      }

      Algorithm_PI(CH_A.Ptr_PI_Cur_Discharge);

      CH_A.PWMxB=PWM_PERIOD-CH_A.Ptr_PI_Cur_Discharge->Out;

      //放电同步整流控制
      SynRect_Boost(&CH_A);

      //B通道放电电流环运算
      CH_B.Ptr_PI_Cur_Discharge->Feed=-CH_B.Now_Cur;

      //快速启动请求处理
      if(CH_B.Req_FastStart==TRUE)
      {
    	  CH_B_PI_Cur_Discharge.Out_I += ((int32)20<<CH_B_PI_Cur_Discharge.Q_Type);

          if((CH_B.PWMxB<=DCDC_Service.PWM_Ready)||(++CH_B.Cnt_FastStart>500))
          {
          	CH_B.Req_FastStart=FALSE;
          	//CH_B.Slew_Cur_ON=CUR_SLEW_DEFAULT;
          }
      }
      else if(CH_B.Req_FastStart_DCM==TRUE)
      {
          CH_B_PI_Cur_Discharge.Out_I += ((int32)3<<CH_B_PI_Cur_Discharge.Q_Type);

          if(((-CH_B.Now_Cur)+500>=CH_B.Set_Cur)||(++CH_B.Cnt_FastStart>600)||(CH_B.PWMxB<=DCDC_Service.PWM_Ready))
          {
              CH_B.Req_FastStart_DCM=FALSE;
              //CH_B.Slew_Cur_ON=CUR_SLEW_DEFAULT;
          }
      }

      CH_B_PI_Cur_Discharge.Forward_Out=(Bus_Forward.PWM_Forward*(PWM_PERIOD-CH_B_PI_Cur_Discharge.Out))>>13;

      if((CH_B_PI_Cur_Discharge.Forward_Out==1)||(CH_B_PI_Cur_Discharge.Forward_Out==-1))
      {
          CH_B_PI_Cur_Discharge.Forward_Out=0;
      }

      Algorithm_PI(CH_B.Ptr_PI_Cur_Discharge);

      CH_B.PWMxB=PWM_PERIOD-CH_B.Ptr_PI_Cur_Discharge->Out;

      //放电同步整流控制
      SynRect_Boost(&CH_B);

      EPwm1Regs.CMPA.half.CMPA  =CH_A.PWMxA;
      EPwm1Regs.CMPB            =CH_A.PWMxB;
      EPwm2Regs.CMPA.half.CMPA  =CH_B.PWMxA;
      EPwm2Regs.CMPB            =CH_B.PWMxB;

      if(Interface_DCDC.Req_ChangeMode==TRUE)
      {
          DCDC_Service.Now_Mode=DCDC_SOFTOFF_DISCH;
      }

      if(DCDC_Service.Fault_Module!=0)
      {
          DCDC_Service.Now_Mode=DCDC_SOFTOFF_DISCH;
      }
          break;}


  case DCDC_CV:
  {
      //请求刷新设定电压，更新目标电压
      if(Interface_DCDC.Req_Refresh_Vol==TRUE)
      {
          DCDC_Service.Set_Vol = Interface_DCDC.Req_Vol;
          Interface_DCDC.Req_Refresh_Vol=FALSE;
      }

      //设定电压缓升缓降
      if(DCDC_Service.Ptr_PI_Vol_CV->Ref<DCDC_Service.Set_Vol)
      {
          DCDC_Service.Ptr_PI_Vol_CV->Ref   = DCDC_Service.Ptr_PI_Vol_CV->Ref+DCDC_Service.Slew_Vol_ON;
          if(DCDC_Service.Ptr_PI_Vol_CV->Ref>DCDC_Service.Set_Vol)
          {
              DCDC_Service.Ptr_PI_Vol_CV->Ref=DCDC_Service.Set_Vol;
          }

          if(DCDC_Service.Ptr_PI_Vol_CV->Ref>DCDC_Service.Limit_Set_Vol)
          {
              DCDC_Service.Ptr_PI_Vol_CV->Ref=DCDC_Service.Limit_Set_Vol;
          }
      }
      else if(DCDC_Service.Ptr_PI_Vol_CV->Ref>DCDC_Service.Set_Vol)
      {
          DCDC_Service.Ptr_PI_Vol_CV->Ref   = DCDC_Service.Ptr_PI_Vol_CV->Ref-DCDC_Service.Slew_Vol_ON;
          if(DCDC_Service.Ptr_PI_Vol_CV->Ref<DCDC_Service.Set_Vol)
          {
              DCDC_Service.Ptr_PI_Vol_CV->Ref=DCDC_Service.Set_Vol;
          }

          if(DCDC_Service.Ptr_PI_Vol_CV->Ref<0)
          {
              DCDC_Service.Ptr_PI_Vol_CV->Ref=0;
          }
      }

      DCDC_Service.Ptr_PI_Vol_CV->Feed=DCDC_Service.Now_Vol_Out;
      Algorithm_PI(DCDC_Service.Ptr_PI_Vol_CV);
      CH_A.PWMxA=DCDC_Service.Ptr_PI_Vol_CV->Out;
      CH_B.PWMxA=CH_A.PWMxA;

      if((CH_A.EN_SynRect==TRUE)&&(CH_A.EN_Run==TRUE)&&(CH_A.EN_Run_Comm==TRUE))
      {
          if(CH_A.Flag_SynRect==TRUE)
          {
              if(CH_A.PWMxA>=50)
              {
            	  CH_A.PWMxB=CH_A.PWMxA;
              }
              else
              {
                  CH_A.PWMxB=50;
              }
          }
          else
          {
        	  if(CH_A.PWMxB>CH_A.PWMxA)
        	  {
        		  CH_A.PWMxB=CH_A.PWMxB-1;
        	  }
        	  else
        	  {
        		  CH_A.Flag_SynRect=TRUE;
                  CH_A.PWMxB=CH_A.PWMxA;
              }
          }
      }
      else
      {
          CH_A.Cnt_SynRect=0;
          CH_A.Flag_SynRect=FALSE;
          CH_A.PWMxB=PWM_PERIOD-PWM_DEADTIME;
      }

      if((CH_B.EN_SynRect==TRUE)&&(CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE))
      {
          if(CH_B.Flag_SynRect==TRUE)
          {
              if(CH_B.PWMxA>=50)
              {
            	  CH_B.PWMxB=CH_B.PWMxA;
              }
              else
              {
                  CH_B.PWMxB=50;
              }
          }
          else
          {
        	  if(CH_B.PWMxB>CH_B.PWMxA)
        	  {
        		  CH_B.PWMxB=CH_B.PWMxB-1;
        	  }
        	  else
        	  {
        		  CH_B.Flag_SynRect=TRUE;
                  CH_B.PWMxB=CH_B.PWMxA;
              }
          }
      }
      else
      {
          CH_B.Cnt_SynRect=0;
          CH_B.Flag_SynRect=FALSE;
          CH_B.PWMxB=PWM_PERIOD-PWM_DEADTIME;
      }

      EPwm1Regs.CMPA.half.CMPA  =CH_A.PWMxA;
      EPwm1Regs.CMPB            =CH_A.PWMxB;
      EPwm2Regs.CMPA.half.CMPA  =CH_B.PWMxA;
      EPwm2Regs.CMPB            =CH_B.PWMxB;

      if(Interface_DCDC.Req_ChangeMode==TRUE)
      {
          DCDC_Service.Now_Mode=DCDC_PWM_OFF;
      }

      if(DCDC_Service.Fault_Module!=0)
      {
          DCDC_Service.Now_Mode=DCDC_PWM_OFF;
      }
          break;}


  case DCDC_DCV:
  {
      //请求刷新设定电压，更新目标电压
      if(Interface_DCDC.Req_Refresh_Vol==TRUE)
      {
          DCDC_Service.Set_Vol = Interface_DCDC.Req_Vol;
          Interface_DCDC.Req_Refresh_Vol=FALSE;
      }

      if(DCDC_Service.Ptr_PI_Vol_DCV->Ref<DCDC_Service.Set_Vol)
      {
          DCDC_Service.Ptr_PI_Vol_DCV->Ref=DCDC_Service.Ptr_PI_Vol_DCV->Ref+DCDC_Service.Slew_Vol_ON;
          if(DCDC_Service.Ptr_PI_Vol_DCV->Ref>DCDC_Service.Set_Vol)
          {
              DCDC_Service.Ptr_PI_Vol_DCV->Ref=DCDC_Service.Set_Vol;
          }

          if(DCDC_Service.Ptr_PI_Vol_DCV->Ref>DCDC_Service.Limit_Set_Vol)
          {
              DCDC_Service.Ptr_PI_Vol_DCV->Ref=DCDC_Service.Limit_Set_Vol;
          }
      }
      else if(DCDC_Service.Ptr_PI_Vol_DCV->Ref>DCDC_Service.Set_Vol)
      {
          DCDC_Service.Ptr_PI_Vol_DCV->Ref=DCDC_Service.Ptr_PI_Vol_DCV->Ref-DCDC_Service.Slew_Vol_ON;
          if(DCDC_Service.Ptr_PI_Vol_DCV->Ref<DCDC_Service.Set_Vol)
          {
              DCDC_Service.Ptr_PI_Vol_DCV->Ref=DCDC_Service.Set_Vol;
          }

          if(DCDC_Service.Ptr_PI_Vol_DCV->Ref<0)
          {
              DCDC_Service.Ptr_PI_Vol_DCV->Ref=0;
          }
      }

      DCDC_Service.Ptr_PI_Vol_DCV->Feed=DCDC_Service.Now_Vol_Out;
      Algorithm_PI(DCDC_Service.Ptr_PI_Vol_DCV);
      CH_A.PWMxA=DCDC_Service.Ptr_PI_Vol_DCV->Out;
      CH_A.PWMxB=CH_A.PWMxA;
      CH_B.PWMxA=CH_A.PWMxA;
      CH_B.PWMxB=CH_A.PWMxA;


      EPwm1Regs.CMPA.half.CMPA  =PWM_PERIOD_AGE-CH_A.PWMxA;
      EPwm1Regs.CMPB            =PWM_PERIOD_AGE-CH_A.PWMxB;
      EPwm2Regs.CMPA.half.CMPA  =PWM_PERIOD_AGE-CH_B.PWMxA;
      EPwm2Regs.CMPB            =PWM_PERIOD_AGE-CH_B.PWMxB;

      if(Interface_DCDC.Req_ChangeMode==TRUE)
      {
          DCDC_Service.Now_Mode=DCDC_PWM_OFF;
      }

      if(DCDC_Service.Fault_Module!=0)
      {
          DCDC_Service.Now_Mode=DCDC_PWM_OFF;
      }
          break;}


      case DCDC_SHUTDOWN:
      {
          if(DCDC_Service.Fault_Module==0)//20200417 &&(Interface_DCDC.EN_Run==TRUE))
          {
              //Bus过压、低压，A，B通道过流检测计数器复位
              CH_A.Cnt_Out_OC       =0;
              CH_B.Cnt_Out_OC       =0;
              DCDC_Service.Cnt_Bus_H=0;
              DCDC_Service.Cnt_Bus_L=0;
              DCDC_Service.Cnt_Fault_AD7606=0;

              //关闭输出MOS
              DC_OUT_CHA_OFF;
              DC_OUT_CHB_OFF;

              //延时20mS开启PWM
              if(DCDC_Service.Mode_Parall==PARALL_SLAVER)
              {
                  if(++DCDC_Service.Cnt_Delay<1025)
                  {return;}
              }
              else
              {
                  if(++DCDC_Service.Cnt_Delay<1000)
                  {return;}
              }


              DCDC_Service.Cnt_Delay=0;

              //状态切换
              DCDC_Service.Now_Mode=DCDC_STANDBY;
          }
          //DELAY_US(5);
          break;}

      case DCDC_STANDBY:
      {
			 DCDC_Service.Flag_Aging   = Interface_DCDC.Req_Aging;
             DCDC_Service.Mode_Parall  = Interface_DCDC.Mode_Parall;
             DCDC_Service.Parall_Total = Interface_DCDC.Parall_Total;
          if((Interface_DCDC.Req_ChangeMode==TRUE)&&(Interface_DCDC.EN_Run==TRUE)&&\
             (Fault_Code.Err_Global==0)&&(Fault_Code.Err_Step==0)&&(DCDC_Service.Fault_Module==0)&&(Fault_Code.Alarm==0))
          {
              DCDC_Service.Mode_Parall  = Interface_DCDC.Mode_Parall;
              DCDC_Service.Parall_Total = Interface_DCDC.Parall_Total;
             
              if(DCDC_Service.Mode_Parall==PARALL_SLAVER)
              {
                  //从机模式只允许进入CC,CCtoCV,DCC,DCCtoDCV,STATIC状态
                  if((Interface_DCDC.Req_Mode<DCDC_CC)||(Interface_DCDC.Req_Mode>DCDC_STATIC))
                  {Interface_DCDC.Req_ChangeMode=FALSE;return;}
              }
              else if(DCDC_Service.Mode_Parall==PARALL_MASTER)
              {
                  //主机模式只允许进入CC,CCtoCV,DCC,DCCtoDCV,STATIC状态
                  if((Interface_DCDC.Req_Mode<DCDC_CC)||(Interface_DCDC.Req_Mode>DCDC_STATIC))
                  {Interface_DCDC.Req_ChangeMode=FALSE;return;}

                  //从机工作数据装载
                  if((Interface_DCDC.Req_Cur_Total>CUR_TO_SINGLE_CH*2*Interface_DCDC.Parall_Total)||(DCDC_Service.Flag_Aging==TRUE)||(Interface_DCDC.Req_Mode==DCDC_STATIC))
                  {
                      Parall_Master.ID1_To_Sx.ID1_L.Bit.EN_Run      = Interface_DCDC.EN_Run_Slaver;
                      Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Mode    = Interface_DCDC.Req_Mode;
                  }
                  else
                  {
                      Parall_Master.ID1_To_Sx.ID1_L.Bit.EN_Run      = FALSE;
                      Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Mode    = DCDC_STANDBY;
                  }

                  Parall_Master.ID1_To_Sx.ID1_H.Set_Cur_Total       = Interface_DCDC.Req_Cur_Total;//20200416
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.EN_MOS_Out      = Interface_DCDC.EN_MOS_Out_Slaver;
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Fault_Clear = TRUE;
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_ChangeMode  = TRUE;
                  Parall_Master.ID1_To_Sx.Cnt_TX_Req_Change_Mode    = 1;
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Refresh_Cur = TRUE;
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.Symbol          = SYMBOL_ID1;
                  Parall_Master.ID1_To_Sx.Req_TX                    = TRUE;
                  Parall_Master.Cnt_Period                          = 0;
              }
              else if(DCDC_Service.Mode_Parall==SINGLE)
              {
                  //单机模式允许进入CC,CCtoCV,DCC,DCCtoDCV,STATIC,CV,DCV,DIAGN_A2B,DIAGN_B2A,DEBUG状态
                  if((Interface_DCDC.Req_Mode<DCDC_CC)||(Interface_DCDC.Req_Mode>DCDC_DEBUG))
                  {Interface_DCDC.Req_ChangeMode=FALSE;return;}
              }
              else
              {   //模式异常，不执行
                  Interface_DCDC.Req_ChangeMode=FALSE;return;
              }

              //数据初始化
              DCDC_Service.Set_Cur_Total    = Interface_DCDC.Req_Cur_Total;
              DCDC_Service.Set_Vol          = Interface_DCDC.Req_Vol;
              CH_A.EN_Run_Comm              = Interface_DCDC.EN_CHA_Comm;
              CH_A.EN_SynRect               = Interface_DCDC.EN_CHA_SynRect;
              CH_B.EN_Run_Comm              = Interface_DCDC.EN_CHB_Comm;
              CH_B.EN_SynRect               = Interface_DCDC.EN_CHB_SynRect;
              DCDC_Service.Flag_Soft_OFF    = FALSE;
              DCDC_Service.Cnt_Delay        = 0;
              Interface_DCDC.Req_ChangeMode = FALSE;

              //计算推高输出电容电压需要的占空比
              if(DCDC_Service.Now_Vol_Bus>DCDC_Service.Now_Vol_Out)
              {
            	  DCDC_Service.PWM_Ready=DCDC_Service.Now_Vol_Out*(PWM_PERIOD-PWM_DEADTIME)/DCDC_Service.Now_Vol_Bus;

                  #if VER_HARDWARE==C017 || VER_HARDWARE==C017_
                      if(DCDC_Service.PWM_Ready<50)
                      {
                          DCDC_Service.PWM_Ready=0;
                      }

                      if(DCDC_Service.PWM_Ready>PWM_MAX_CHARGE)
                      {
                          DCDC_Service.PWM_Ready=PWM_MAX_CHARGE;
                      }
                      DCDC_Service.Now_Mode         = DCDC_READY2;
                  #elif VER_HARDWARE==C017B || VER_HARDWARE==C017D || VER_HARDWARE==C017D_

                      if(DCDC_Service.PWM_Ready<50)
                      {
                          DCDC_Service.PWM_Ready=0;
                          DCDC_Service.Now_Mode         = DCDC_READY2;
                      }
                      else
                      {
                          if(DCDC_Service.PWM_Ready>PWM_MAX_CHARGE)
                          {DCDC_Service.PWM_Ready=PWM_MAX_CHARGE;}

                          CH_A.PWMxA =	PWM_DEADTIME;
                          CH_B.PWMxA =	PWM_DEADTIME;

                          CH_A.PWMxB =	PWM_PERIOD-PWM_DEADTIME;
                          CH_B.PWMxB =	PWM_PERIOD-PWM_DEADTIME;

                          PWM1ON;
                          PWM2ON;

                          DCDC_Service.Now_Mode         = DCDC_READY1;
                      }
                  #endif
              }
              else
              {
                  DCDC_Service.Fault_Module	   |= FAULT_BUS_L;
                  DCDC_Service.Now_Mode			= DCDC_PWM_OFF;
                  PWM1OFF;
                  PWM2OFF;
              }
              DCDC_Service.Cnt_Delay=0;
		  }
		  else if((Interface_DCDC.Req_ChangeMode==TRUE)&&(DCDC_Service.Mode_Parall==PARALL_MASTER)&&((Fault_Code.Err_Global!=0)||(Fault_Code.Err_Step!=0)||(DCDC_Service.Fault_Module!=0)||(Fault_Code.Alarm!=0)))
          {

                  //从机工作数据装载
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.EN_Run          = TRUE;
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Mode        = DCDC_STANDBY;

                  Parall_Master.ID1_To_Sx.ID1_H.Set_Cur_Total       = 0;
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.EN_MOS_Out      = Interface_DCDC.EN_MOS_Out_Slaver;
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Fault_Clear = FALSE;
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_ChangeMode  = TRUE;
                  Parall_Master.ID1_To_Sx.Cnt_TX_Req_Change_Mode    = 1;
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Refresh_Cur = FALSE;
                  Parall_Master.ID1_To_Sx.ID1_L.Bit.Symbol          = SYMBOL_ID1;
                  Parall_Master.ID1_To_Sx.Req_TX                    = TRUE;
                  Parall_Master.Cnt_Period                          = 0;

                  Interface_DCDC.Req_ChangeMode = FALSE;
          }

          else
          {
              Interface_DCDC.Req_ChangeMode = FALSE;
          }
          break;}

      case DCDC_READY1:
      {
    	  //开启同步整流，将输出电容电压推高到和输出端相近的电压
    	  if(CH_A.PWMxA<DCDC_Service.PWM_Ready)
    	  {
    		  CH_A.PWMxA=CH_A.PWMxA+1;
    		  CH_B.PWMxA=CH_A.PWMxA;
    	  }

    	  if(CH_A.PWMxB>DCDC_Service.PWM_Ready)
    	  {
    		  CH_A.PWMxB=CH_A.PWMxB-1;
    		  CH_B.PWMxB=CH_A.PWMxB;
    	  }

          //PWM值输出到ePWM模块
          EPwm1Regs.CMPA.half.CMPA  =CH_A.PWMxA;
          EPwm1Regs.CMPB            =CH_A.PWMxB;
          EPwm2Regs.CMPA.half.CMPA  =CH_B.PWMxA;
          EPwm2Regs.CMPB            =CH_B.PWMxB;

    	  if((CH_A.PWMxA<DCDC_Service.PWM_Ready)||(CH_A.PWMxB>DCDC_Service.PWM_Ready))
    	  {
              //DELAY_US(5);
    		  return;
    	  }

          PWM1OFF;
          PWM2OFF;

          DCDC_Service.Now_Mode         = DCDC_READY2;
          break;}

      case DCDC_READY2:
      {
          if(DCDC_Service.Flag_Aging==TRUE)
          {   //处于老化模式都工作于双通道模式
              CH_A.Set_Cur  = (DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
              CH_B.Set_Cur  = CH_A.Set_Cur;
              CH_A.EN_Run   = TRUE;
              CH_B.EN_Run   = TRUE;
              if(CH_A.Set_Cur<CUR_SYNRECT_OFF)
              {
                  CH_A.EN_SynRect=FALSE;
                  CH_B.EN_SynRect=FALSE;
              }
              DCDC_Service.Flag_SingleCH=FALSE;
          }
          else
          {
        	  if((DCDC_Service.Set_Cur_Total>CUR_TO_SINGLE_CH*2*DCDC_Service.Parall_Total)||(DCDC_Service.Mode_Parall==PARALL_SLAVER))
        	  {
                  //电流大于一定值或处于从机状态
        		  CH_A.Set_Cur  = (DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
                  CH_B.Set_Cur  = CH_A.Set_Cur;
                  CH_A.EN_Run   = TRUE;
                  CH_B.EN_Run   = TRUE;
                  if(CH_A.Set_Cur<CUR_SYNRECT_OFF)
                  {
                      CH_A.EN_SynRect=FALSE;
                      CH_B.EN_SynRect=FALSE;
                  }
                  DCDC_Service.Flag_SingleCH=FALSE;
        	  }
        	  else
        	  {
        		  //单通道工作模式
        		  CH_A.Set_Cur  = DCDC_Service.Set_Cur_Total;
        		  CH_B.Set_Cur  = 0;
        		  CH_A.EN_Run   = TRUE;
        		  CH_B.EN_Run   = FALSE;
        		  CH_A.EN_SynRect=FALSE;
        		  CH_B.EN_SynRect=FALSE;
        		  DCDC_Service.Flag_SingleCH=TRUE;
        	  }
          }

          //是否允许开启输出MOS判断
          if((Interface_DCDC.EN_MOS_Out==ON)&&(Interface_DCDC.Req_Mode!=DCDC_STATIC))
          {

              #if VER_HARDWARE==C017 || VER_HARDWARE==C017_
                  if(((CH_A.EN_Run==TRUE)&&(CH_A.EN_Run_Comm==TRUE))||((CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE)))
                  {
                      CH_A.Flag_MOS_Out = ON;
                      DC_OUT_CHA_ON;
                  }
                  else
                  {
                      CH_A.Flag_MOS_Out = OFF;
                      DC_OUT_CHA_OFF;
                  }

                  if((CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE))
                  {
                      CH_B.Flag_MOS_Out = ON;
                      DC_OUT_CHB_ON;
                  }
                  else
                  {
                      CH_B.Flag_MOS_Out = OFF;
                      DC_OUT_CHB_OFF;
                  }
              #elif VER_HARDWARE==C017B || VER_HARDWARE==C017D || VER_HARDWARE==C017D_
                  if((CH_A.EN_Run==TRUE)&&(CH_A.EN_Run_Comm==TRUE))
                  {
                      CH_A.Flag_MOS_Out = ON;
                      DC_OUT_CHA_ON;
                  }
                  else
                  {
                      CH_A.Flag_MOS_Out = OFF;
                      DC_OUT_CHA_OFF;
                  }

                  if((CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE))
                  {
                      CH_B.Flag_MOS_Out = ON;
                      DC_OUT_CHB_ON;
                  }
                  else
                  {
                      CH_B.Flag_MOS_Out = OFF;
                      DC_OUT_CHB_OFF;
                  }
              #endif
          }
          else
          {
              DC_OUT_CHA_OFF;
              DC_OUT_CHB_OFF;
              CH_A.Flag_MOS_Out = OFF;
              CH_B.Flag_MOS_Out = OFF;
          }
          DCDC_Service.Now_Mode         = DCDC_READY;
    	  break;}

      case DCDC_READY:
      {
          DCDC_Service.Flag_Soft_OFF=FALSE;

          //转换到工作模式前等待0.04秒，等待输出电容冲击电流下降
          if(++DCDC_Service.Cnt_Delay<2000)
          {
              //DELAY_US(5);//避免时间过短，AD7606还未完成转换就开始读取数据
        	  return;
          }
          DCDC_Service.Cnt_Delay=0;

          //各模式通用初始化部分
          CH_A.Cnt_SynRect=0;
          CH_A.Flag_SynRect=FALSE;
          CH_A.Cnt_SynRect_Off=0;
          CH_B.Cnt_SynRect=0;
          CH_B.Cnt_SynRect_Off=0;
          CH_B.Flag_SynRect=FALSE;

    	  CH_A.Req_FastStart=FALSE;
    	  CH_B.Req_FastStart=FALSE;

          CH_A.Req_FastStart_DCM=FALSE;
          CH_B.Req_FastStart_DCM=FALSE;

    	  CH_A.Cnt_FastStart=0;
    	  CH_B.Cnt_FastStart=0;

          if((DCDC_Service.Mode_Parall==PARALL_SLAVER)&&(Interface_DCDC.Req_Mode!=DCDC_STANDBY))
          {
              Interface_DCDC.Req_ChangeMode=FALSE;
          }

          //数据平均与工步运行进行同步
          Data_Sum.Sum_Cur_CHA        = 0;
          Data_Sum.Sum_Cur_CHB        = 0;
          Data_Sum.Sum_Vol_Bus        = 0;
          Data_Sum.Sum_Vol_Out        = 0;
          Data_Sum.Sum_Vol_Sense      = 0;
          Data_Sum.Sum_AD_PT          = 0;
          Data_Sum.Sum_AD_NTC1        = 0;
          Data_Sum.Sum_AD_NTC2        = 0;
          Data_Sum.Cnt_Sum            = 0;

          DCDC_Service.Cnt_FastRefresh=0;
          DCDC_Service.Time_Running=0;

          if(Interface_DCDC.Req_Mode==DCDC_CC)
          {
              //进入CC模式前初始化
              DCDC_Service.Now_Mode=DCDC_CC;

              //CHA通道数据初始化
              CH_A.Ptr_PI_Cur_Charge->Ref   = CH_A.Slew_Cur_ON;
              CH_A.Ptr_PI_Cur_Charge->Out_I = (int32)PWM_DEADTIME<<CH_A.Ptr_PI_Cur_Charge->Q_Type;
              CH_A.Ptr_PI_Cur_Charge->Out   = PWM_DEADTIME;
              CH_A.PWMxA                    = PWM_DEADTIME;
              CH_A.PWMxB                    = PWM_PERIOD-PWM_DEADTIME;//PWMxB设置0占空比状态

              //CHB通道数据初始化
              CH_B.Ptr_PI_Cur_Charge->Ref   = CH_B.Slew_Cur_ON;
              CH_B.Ptr_PI_Cur_Charge->Out_I = (int32)PWM_DEADTIME<<CH_B.Ptr_PI_Cur_Charge->Q_Type;
              CH_B.Ptr_PI_Cur_Charge->Out   = PWM_DEADTIME;
              CH_B.PWMxA                    = PWM_DEADTIME;
              CH_B.PWMxB                    = PWM_PERIOD-PWM_DEADTIME;//PWMxB设置0占空比状态

              //每通道电流设定值计算及通道使能控制
              if((DCDC_Service.Set_Cur_Total>CUR_TO_SINGLE_CH*2*DCDC_Service.Parall_Total)||
                 (DCDC_Service.Flag_Aging==TRUE)||(DCDC_Service.Mode_Parall==PARALL_SLAVER))
              {   //总电流大于一定电流或处于老化模式或处于从机模式都工作于双通道模式
                  CH_A.Set_Cur  = (DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
                  CH_B.Set_Cur  = CH_A.Set_Cur;
                  CH_A.EN_Run   = TRUE;
                  CH_B.EN_Run   = TRUE;
                  if(CH_A.Set_Cur<CUR_SYNRECT_OFF)
                  {
                      CH_A.EN_SynRect=FALSE;
                      CH_B.EN_SynRect=FALSE;
                  }
                  DCDC_Service.Flag_SingleCH=FALSE;
              }
              else
              {   //单通道工作模式
                  CH_A.Set_Cur  = DCDC_Service.Set_Cur_Total;
                  CH_B.Set_Cur  = 0;
                  CH_A.EN_Run   = TRUE;
                  CH_B.EN_Run   = FALSE;

                  if(CH_A.Set_Cur<CUR_SYNRECT_OFF)
                  {
                      CH_A.EN_SynRect=FALSE;
                  }
                  else
                  {
                      CH_A.EN_SynRect=TRUE;
                  }

                  CH_B.EN_SynRect=FALSE;
                  DCDC_Service.Flag_SingleCH=TRUE;
              }

              #if (VER_HARDWARE==C017 && VER_PRODUCT==PRODUCT_5V60A)||(VER_HARDWARE==C017D_ && VER_PRODUCT==PRODUCT_5V80A)
              //切换到高电流档位
              SET_CUR_RANGE_H;
              DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
              CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;
              #else
              //电流档位切换
              if((CH_A.Set_Cur<=CUR_TO_LOW_RANGE)&&(DCDC_Service.Mode_Parall!=PARALL_SLAVER))
              {
                  //切换到低电流档位
                  SET_CUR_RANGE_L;
                  DCDC_Service.Flag_CurRange    = CUR_RANGE_L;
                  CH_A.Limit_Set_Cur            = CUR_TO_LOW_RANGE;//防止出现异常将电流给定值设定超出电流量程
                  CH_B.Limit_Set_Cur            = CUR_TO_LOW_RANGE;//防止出现异常将电流给定值设定超出电流量程
                  DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_L;
              }
              else
              {
                  //切换到高电流档位
                  SET_CUR_RANGE_H;
                  DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
                  CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                  CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                  DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;
              }
              #endif

              //快速启动使能判断
              if(CH_A.Set_Cur>CUR_TO_FAST_START)
              {
            	  CH_A.Req_FastStart=TRUE;
            	  CH_B.Req_FastStart=TRUE;
            	  //启动斜率设定
            	  CH_A.Slew_Cur_ON    = (CH_A.Set_Cur*10)>>13;
            	  if(CH_A.Slew_Cur_ON<100)
            	  {
            		  CH_A.Slew_Cur_ON    =100;
            	  }
            	  else if(CH_A.Slew_Cur_ON>CUR_SLEW_MAX)
            	  {
            		  CH_A.Slew_Cur_ON=CUR_SLEW_MAX;
            	  }
            	  CH_B.Slew_Cur_ON    = CH_A.Slew_Cur_ON;
          	  }
              else
              {
                  CH_A.Req_FastStart=FALSE;
                  CH_B.Req_FastStart=FALSE;
                  CH_A.Req_FastStart_DCM=TRUE;
                  CH_B.Req_FastStart_DCM=TRUE;
                  CH_A.Slew_Cur_ON=32;
                  CH_B.Slew_Cur_ON=32;
              }

              //前馈系数初始化
              Bus_Forward.K_Forward			= Bus_Forward.K_Forward_Charge;

              if((CH_A.EN_Run==TRUE)&&(CH_A.EN_Run_Comm==TRUE))
              { PWM1ON; }

              if((CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE))
              { PWM2ON; }
          }
          else if(Interface_DCDC.Req_Mode==DCDC_CCtoCV)
          {
              //状态机切换
              DCDC_Service.Now_Mode=DCDC_CCtoCV;

              //CHA通道数据初始化
              CH_A.Ptr_PI_Cur_Charge->Ref   = CH_A.Slew_Cur_ON;
              CH_A.Ptr_PI_Cur_Charge->Out_I = (int32)PWM_DEADTIME<<CH_A.Ptr_PI_Cur_Charge->Q_Type;
              CH_A.Ptr_PI_Cur_Charge->Out   = PWM_DEADTIME;
              CH_A.PWMxA                    = PWM_DEADTIME;
              CH_A.PWMxB                    = PWM_PERIOD-PWM_DEADTIME;

              //CHB通道数据初始化
              CH_B.Ptr_PI_Cur_Charge->Ref   = CH_B.Slew_Cur_ON;
              CH_B.Ptr_PI_Cur_Charge->Out_I = (int32)PWM_DEADTIME<<CH_B.Ptr_PI_Cur_Charge->Q_Type;
              CH_B.Ptr_PI_Cur_Charge->Out   = PWM_DEADTIME;
              CH_B.PWMxA                    = PWM_DEADTIME;
              CH_B.PWMxB                    = PWM_PERIOD-PWM_DEADTIME;

              //每通道电流设定值计算及通道使能控制
              if((DCDC_Service.Set_Cur_Total>CUR_TO_SINGLE_CH*2*DCDC_Service.Parall_Total)||
                 (DCDC_Service.Flag_Aging==TRUE)||(DCDC_Service.Mode_Parall==PARALL_SLAVER))
              {   //总电流大于一定电流或处于老化模式都工作于双通道模式
                  CH_A.Set_Cur  = (DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
                  CH_B.Set_Cur  = CH_A.Set_Cur;
                  CH_A.EN_Run   = TRUE;
                  CH_B.EN_Run   = TRUE;
                  if(CH_A.Set_Cur<CUR_SYNRECT_OFF)
                  {
                      CH_A.EN_SynRect=FALSE;
                      CH_B.EN_SynRect=FALSE;
                  }
                  DCDC_Service.Flag_SingleCH=FALSE;
              }
              else
              {   //单通道工作模式
                  CH_A.Set_Cur  = DCDC_Service.Set_Cur_Total;
                  CH_B.Set_Cur  = 0;
                  CH_A.EN_Run   = TRUE;
                  CH_B.EN_Run   = FALSE;

                  if(CH_A.Set_Cur<CUR_SYNRECT_OFF)
                  {
                      CH_A.EN_SynRect=FALSE;
                  }
                  else
                  {
                      CH_A.EN_SynRect=TRUE;
                  }

                  CH_B.EN_SynRect=FALSE;
                  DCDC_Service.Flag_SingleCH=TRUE;
              }

              #if (VER_HARDWARE==C017 && VER_PRODUCT==PRODUCT_5V60A)||(VER_HARDWARE==C017D_ && VER_PRODUCT==PRODUCT_5V80A)
              //切换到高电流档位
              SET_CUR_RANGE_H;
              DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
              CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;
              #else
              //电流档位切换
              if((CH_A.Set_Cur<=CUR_TO_LOW_RANGE)&&(DCDC_Service.Mode_Parall!=PARALL_SLAVER))
              {
                  //切换到低电流档位
                  SET_CUR_RANGE_L;
                  DCDC_Service.Flag_CurRange    = CUR_RANGE_L;
                  CH_A.Limit_Set_Cur            = CUR_TO_LOW_RANGE;//防止出现异常将电流给定值设定超出电流量程
                  CH_B.Limit_Set_Cur            = CUR_TO_LOW_RANGE;//防止出现异常将电流给定值设定超出电流量程
                  DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_L;
              }
              else
              {
                  //切换到高电流档位
                  SET_CUR_RANGE_H;
                  DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
                  CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                  CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                  DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;
              }
              #endif

              //恒压充电标志复位,电压环PI初始值在恒流转恒压时初始化
              DCDC_Service.Flag_CV_Step = 0;
              DCDC_Service.Flag_CV_Step2   =0;
              DCDC_Service.Cnt_CCtoCV   = 0;
              DCDC_Service.Num_CCtoCV   = 25;
              DCDC_Service.Cnt_CCtoCV2  = 0;
              DCDC_Service.Cnt_Sum_Vol_Sense=0;
              DCDC_Service.Num_Sum_Vol_Sense=25;
              DCDC_Service.Sum_Vol_Sense=0;

              PI_Vol_Charge.Ki          = PI_Vol_Charge_Ki/20;
              PI_Vol_Charge.Kp          = 40;
              PI_Vol_Charge.Period      = 50;
              PI_Vol_Charge.Limit_Err_Max=2000;
              PI_Vol_Charge.Limit_Err_Min=-2000;
              PI_Vol_Charge.Limit_Out_max=DCDC_Service.Set_Cur_Total;
              PI_Vol_Charge.Limit_PI_max = PI_Vol_Charge.Limit_Out_max<<PI_Vol_Charge.Q_Type;

              if(CH_A.Set_Cur>CUR_TO_FAST_START)
              {
            	  CH_A.Req_FastStart=TRUE;
            	  CH_B.Req_FastStart=TRUE;

            	  //启动斜率设定
            	  CH_A.Slew_Cur_ON    = (CH_A.Set_Cur*10)>>13;
            	  if(CH_A.Slew_Cur_ON<100)
            	  {
            		  CH_A.Slew_Cur_ON  = 100;
            	  }
            	  else if(CH_A.Slew_Cur_ON>CUR_SLEW_MAX)
            	  {
            		  CH_A.Slew_Cur_ON	= CUR_SLEW_MAX;
            	  }
            	  CH_B.Slew_Cur_ON    = CH_A.Slew_Cur_ON;
          	  }
              else
              {
                  CH_A.Req_FastStart=FALSE;
                  CH_B.Req_FastStart=FALSE;
                  CH_A.Req_FastStart_DCM=TRUE;
                  CH_B.Req_FastStart_DCM=TRUE;
                  CH_A.Slew_Cur_ON=32;
                  CH_B.Slew_Cur_ON=32;
              }

              //前馈系数初始化
              Bus_Forward.K_Forward			= Bus_Forward.K_Forward_Charge;

              if((CH_A.EN_Run==TRUE)&&(CH_A.EN_Run_Comm==TRUE))
              { PWM1ON; }

              if((CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE))
              { PWM2ON; }

          }
          else if(Interface_DCDC.Req_Mode==DCDC_CV)
          {
              //状态机切换
              DCDC_Service.Now_Mode=DCDC_CV;

              //开关频率调整到55kHz
              EALLOW;
              EPwm1Regs.TBPRD               = PWM_PERIOD_AGE;             // Set timer period
              EPwm2Regs.TBPRD               = PWM_PERIOD_AGE;             // Set timer period
              EDIS;

              //CV控制PI数据初始化,
              DCDC_Service.Ptr_PI_Vol_CV->Ref   = 0;

              CH_A.PWMxA=PWM_DEADTIME;
              CH_A.PWMxB=PWM_PERIOD-PWM_DEADTIME;
              CH_B.PWMxA=CH_A.PWMxA;
              CH_B.PWMxB=CH_A.PWMxB;
              DCDC_Service.Ptr_PI_Vol_CV->Out_I = (int32)CH_A.PWMxA<<DCDC_Service.Ptr_PI_Vol_CV->Q_Type;
              DCDC_Service.Ptr_PI_Vol_CV->Out   = CH_A.PWMxA;

              //通道使能
              CH_A.EN_Run                   = TRUE;
              CH_B.EN_Run                   = TRUE;
              DCDC_Service.Flag_SingleCH    = FALSE;

              //开启同步整流
              CH_A.EN_SynRect				= TRUE;
              CH_B.EN_SynRect				= TRUE;

            #if (VER_HARDWARE==C017 && VER_PRODUCT==PRODUCT_5V60A)||(VER_HARDWARE==C017D_ && VER_PRODUCT==PRODUCT_5V80A)
              //切换到高电流档位
              SET_CUR_RANGE_H;
              DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
              CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;
            #else

              if(DCDC_Service.Flag_Aging==FALSE)
              {
                  //切换到高电流档位
                  SET_CUR_RANGE_H;
                  DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
                  CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                  CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                  DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;
              }
              else
              {
                  if(Interface_DCDC.Req_CurRange==CUR_RANGE_L)
                  {
                      //切换到低电流档位
                      SET_CUR_RANGE_L;
                      DCDC_Service.Flag_CurRange    = CUR_RANGE_L;
                      CH_A.Limit_Set_Cur            = CUR_TO_LOW_RANGE;//防止出现异常将电流给定值设定超出电流量程
                      CH_B.Limit_Set_Cur            = CUR_TO_LOW_RANGE;//防止出现异常将电流给定值设定超出电流量程
                      DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_L;
                  }
                  else
                  {
                      //切换到高电流档位
                       SET_CUR_RANGE_H;
                       DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
                       CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                       CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                       DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;
                  }
              }
            #endif

              if((CH_A.EN_Run==TRUE)&&(CH_A.EN_Run_Comm==TRUE))
              { PWM1ON; }

              if((CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE))
              { PWM2ON; }
          }
          else if(Interface_DCDC.Req_Mode==DCDC_DCC)
          {
              //状态机切换
              DCDC_Service.Now_Mode=DCDC_DCC;

              //CHA通道数据初始化
              CH_A.Ptr_PI_Cur_Discharge->Ref    = CH_A.Slew_Cur_ON;
              CH_A.Ptr_PI_Cur_Discharge->Out_I  = (int32)PWM_DEADTIME<<CH_A.Ptr_PI_Cur_Discharge->Q_Type;
              CH_A.Ptr_PI_Cur_Discharge->Out    = PWM_DEADTIME;
              CH_A.PWMxA                        = PWM_DEADTIME;
              CH_A.PWMxB                        = PWM_PERIOD-PWM_DEADTIME;

              //CHB通道数据初始化
              CH_B.Ptr_PI_Cur_Discharge->Ref    = CH_B.Slew_Cur_ON;
              CH_B.Ptr_PI_Cur_Discharge->Out_I  = (int32)PWM_DEADTIME<<CH_B.Ptr_PI_Cur_Discharge->Q_Type;
              CH_B.Ptr_PI_Cur_Discharge->Out    = PWM_DEADTIME;
              CH_B.PWMxA                        = PWM_DEADTIME;
              CH_B.PWMxB                        = PWM_PERIOD-PWM_DEADTIME;

              //每通道电流设定值计算及通道使能控制
              if((DCDC_Service.Set_Cur_Total>CUR_TO_SINGLE_CH*2*DCDC_Service.Parall_Total)||
                 (DCDC_Service.Flag_Aging==TRUE)||(DCDC_Service.Mode_Parall==PARALL_SLAVER))
              {   //总电流大于一定电流或处于老化模式都工作于双通道模式
                  CH_A.Set_Cur  = (DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
                  CH_B.Set_Cur  = CH_A.Set_Cur;
                  CH_A.EN_Run   = TRUE;
                  CH_B.EN_Run   = TRUE;
                  if(CH_A.Set_Cur<CUR_SYNRECT_OFF)
                  {
                      CH_A.EN_SynRect=FALSE;
                      CH_B.EN_SynRect=FALSE;
                  }
                  DCDC_Service.Flag_SingleCH=FALSE;
              }
              else
              {   //单通道工作模式
                  CH_A.Set_Cur  = DCDC_Service.Set_Cur_Total;
                  CH_B.Set_Cur  = 0;
                  CH_A.EN_Run   = TRUE;
                  CH_B.EN_Run   = FALSE;

                  if(CH_A.Set_Cur<CUR_SYNRECT_OFF)
                  {
                      CH_A.EN_SynRect=FALSE;
                  }
                  else
                  {
                      CH_A.EN_SynRect=TRUE;
                  }

                  CH_B.EN_SynRect=FALSE;
                  DCDC_Service.Flag_SingleCH=TRUE;
              }

            #if (VER_HARDWARE==C017 && VER_PRODUCT==PRODUCT_5V60A)||(VER_HARDWARE==C017D_ && VER_PRODUCT==PRODUCT_5V80A)
              //切换到高电流档位
              SET_CUR_RANGE_H;
              DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
              CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;
            #else
              //电流档位切换
              if((CH_A.Set_Cur<=CUR_TO_LOW_RANGE)&&(DCDC_Service.Mode_Parall!=PARALL_SLAVER))
              {
                  //切换到低电流档位
                  SET_CUR_RANGE_L;
                  DCDC_Service.Flag_CurRange    = CUR_RANGE_L;
                  CH_A.Limit_Set_Cur            = CUR_TO_LOW_RANGE;//防止出现异常将电流给定值设定超出电流量程
                  CH_B.Limit_Set_Cur            = CUR_TO_LOW_RANGE;//防止出现异常将电流给定值设定超出电流量程
                  DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_L;
              }
              else
              {
                  //切换到高电流档位
                  SET_CUR_RANGE_H;
                  DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
                  CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                  CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                  DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;
              }
            #endif

              //快速启动使能判断
              if(CH_A.Set_Cur>CUR_TO_FAST_START)
              {
            	  CH_A.Req_FastStart=TRUE;
            	  CH_B.Req_FastStart=TRUE;

            	  //启动斜率设定
            	  CH_A.Slew_Cur_ON    = (CH_A.Set_Cur*10)>>13;
            	  if(CH_A.Slew_Cur_ON<100)
            	  {
            		  CH_A.Slew_Cur_ON  = 100;
            	  }
            	  else if(CH_A.Slew_Cur_ON>CUR_SLEW_MAX)
            	  {
            		  CH_A.Slew_Cur_ON	= CUR_SLEW_MAX;
            	  }
            	  CH_B.Slew_Cur_ON    = CH_A.Slew_Cur_ON;
          	  }
              else
              {
                  CH_A.Req_FastStart=FALSE;
                  CH_B.Req_FastStart=FALSE;
                  CH_A.Req_FastStart_DCM=TRUE;
                  CH_B.Req_FastStart_DCM=TRUE;
                  CH_A.Slew_Cur_ON=32;
                  CH_B.Slew_Cur_ON=32;
              }

              //前馈系数初始化
              Bus_Forward.K_Forward			= Bus_Forward.K_Forward_Discharge;

              if((CH_A.EN_Run==TRUE)&&(CH_A.EN_Run_Comm==TRUE))
              { PWM1ON; }

              if((CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE))
              { PWM2ON; }

          }
          else if(Interface_DCDC.Req_Mode==DCDC_DCCtoDCV)
          {
              //状态机切换
              DCDC_Service.Now_Mode=DCDC_DCCtoDCV;

              //CHA通道数据初始化
              CH_A.Ptr_PI_Cur_Discharge->Ref    = CH_A.Slew_Cur_ON;
              CH_A.Ptr_PI_Cur_Discharge->Out_I  = (int32)PWM_DEADTIME<<CH_A.Ptr_PI_Cur_Discharge->Q_Type;
              CH_A.Ptr_PI_Cur_Discharge->Out    = PWM_DEADTIME;
              CH_A.PWMxA                        = PWM_DEADTIME;
              CH_A.PWMxB                        = PWM_PERIOD-PWM_DEADTIME;;

              //CHB通道数据初始化
              CH_B.Ptr_PI_Cur_Discharge->Ref    = CH_B.Slew_Cur_ON;
              CH_B.Ptr_PI_Cur_Discharge->Out_I  = (int32)PWM_DEADTIME<<CH_B.Ptr_PI_Cur_Discharge->Q_Type;
              CH_B.Ptr_PI_Cur_Discharge->Out    = PWM_DEADTIME;
              CH_B.PWMxA                        = PWM_DEADTIME;
              CH_B.PWMxB                        = PWM_PERIOD-PWM_DEADTIME;;

              //每通道电流设定值计算及通道使能控制
              if((DCDC_Service.Set_Cur_Total>CUR_TO_SINGLE_CH*2*DCDC_Service.Parall_Total)||
                 (DCDC_Service.Flag_Aging==TRUE)||(DCDC_Service.Mode_Parall==PARALL_SLAVER))
              {   //总电流大于一定电流或处于老化模式都工作于双通道模式
                  CH_A.Set_Cur  = (DCDC_Service.Set_Cur_Total/DCDC_Service.Parall_Total)>>1;
                  CH_B.Set_Cur  = CH_A.Set_Cur;
                  CH_A.EN_Run   = TRUE;
                  CH_B.EN_Run   = TRUE;
                  if(CH_A.Set_Cur<CUR_SYNRECT_OFF)
                  {
                      CH_A.EN_SynRect=FALSE;
                      CH_B.EN_SynRect=FALSE;
                  }

                  DCDC_Service.Flag_SingleCH=FALSE;
              }
              else
              {   //单通道工作模式
                  CH_A.Set_Cur  = DCDC_Service.Set_Cur_Total;
                  CH_B.Set_Cur  = 0;
                  CH_A.EN_Run   = TRUE;
                  CH_B.EN_Run   = FALSE;

                  if(CH_A.Set_Cur<CUR_SYNRECT_OFF)
                  {
                      CH_A.EN_SynRect=FALSE;
                  }
                  else
                  {
                      CH_A.EN_SynRect=TRUE;
                  }

                  CH_B.EN_SynRect=FALSE;
                  DCDC_Service.Flag_SingleCH=TRUE;
              }

            #if (VER_HARDWARE==C017 && VER_PRODUCT==PRODUCT_5V60A)||(VER_HARDWARE==C017D_ && VER_PRODUCT==PRODUCT_5V80A)
              //切换到高电流档位
              SET_CUR_RANGE_H;
              DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
              CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;
            #else
              //电流档位切换
              if((CH_A.Set_Cur<=CUR_TO_LOW_RANGE)&&(DCDC_Service.Mode_Parall!=PARALL_SLAVER))
              {
                  //切换到低电流档位
                  SET_CUR_RANGE_L;
                  DCDC_Service.Flag_CurRange    = CUR_RANGE_L;
                  CH_A.Limit_Set_Cur            = CUR_TO_LOW_RANGE;//防止出现异常将电流给定值设定超出电流量程
                  CH_B.Limit_Set_Cur            = CUR_TO_LOW_RANGE;//防止出现异常将电流给定值设定超出电流量程
                  DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_L;
              }
              else
              {
                  //切换到高电流档位
                  SET_CUR_RANGE_H;
                  DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
                  CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                  CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
                  DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;
              }
            #endif

              //电压环数据初始化
              DCDC_Service.Flag_DCV_Step    = 0;
              DCDC_Service.Flag_DCV_Step2   = 0;
              DCDC_Service.Cnt_DCCtoDCV     = 0;
              DCDC_Service.Num_DCCtoDCV     = 25;
              DCDC_Service.Cnt_DCCtoDCV2    = 0;
              DCDC_Service.Cnt_Sum_Vol_Sense= 0;
              DCDC_Service.Num_Sum_Vol_Sense= 25;
              DCDC_Service.Sum_Vol_Sense    = 0;

              PI_Vol_Discharge.Limit_Out_max= DCDC_Service.Set_Cur_Total;
              PI_Vol_Discharge.Limit_PI_max = PI_Vol_Discharge.Limit_Out_max<<PI_Vol_Discharge.Q_Type;
              PI_Vol_Discharge.Ki           = PI_Vol_Discharge_Ki/20;
              PI_Vol_Discharge.Kp           = 40;
              PI_Vol_Discharge.Period       = 50;
              PI_Vol_Discharge.Limit_Err_Max= 2000;
              PI_Vol_Discharge.Limit_Err_Min=-2000;

              //快速启动使能判断
              if(CH_A.Set_Cur>CUR_TO_FAST_START)
              {
            	  CH_A.Req_FastStart=TRUE;
            	  CH_B.Req_FastStart=TRUE;

            	  //启动斜率设定
            	  CH_A.Slew_Cur_ON    = (CH_A.Set_Cur*10)>>13;
            	  if(CH_A.Slew_Cur_ON<100)
            	  {
            		  CH_A.Slew_Cur_ON  = 100;
            	  }
            	  else if(CH_A.Slew_Cur_ON>CUR_SLEW_MAX)
            	  {
            		  CH_A.Slew_Cur_ON	= CUR_SLEW_MAX;
            	  }
            	  CH_B.Slew_Cur_ON    = CH_A.Slew_Cur_ON;
          	  }
              else
              {
                  CH_A.Req_FastStart=FALSE;
                  CH_B.Req_FastStart=FALSE;
                  CH_A.Req_FastStart_DCM=TRUE;
                  CH_B.Req_FastStart_DCM=TRUE;
                  CH_A.Slew_Cur_ON=32;
                  CH_B.Slew_Cur_ON=32;
              }

              //前馈系数初始化
              Bus_Forward.K_Forward			= Bus_Forward.K_Forward_Discharge;

              if((CH_A.EN_Run==TRUE)&&(CH_A.EN_Run_Comm==TRUE))
              { PWM1ON; }

              if((CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE))
              { PWM2ON; }

          }
          else if(Interface_DCDC.Req_Mode==DCDC_DCV)
          {
              //状态机切换
              DCDC_Service.Now_Mode=DCDC_DCCtoDCV;

              //CV控制PI数据初始化,占空比初始化为D=Vout/Vbus
              DCDC_Service.Ptr_PI_Vol_DCV->Ref  = DCDC_Service.Now_Vol_Out;
              CH_A.PWMxA=DCDC_Service.Ptr_PI_Vol_DCV->Ref*PWM_PERIOD_AGE/DCDC_Service.Now_Vol_Bus+PWM_DEADTIME;
              if(CH_A.PWMxA<PWM_DEADTIME)
              {CH_A.PWMxA   = PWM_DEADTIME;}

              if(CH_A.PWMxA>PWM_PERIOD_AGE-PWM_DEADTIME)
              {CH_A.PWMxA   = PWM_PERIOD_AGE-PWM_DEADTIME;}

              CH_A.PWMxB    = CH_A.PWMxA;
              CH_B.PWMxA    = CH_A.PWMxA;
              CH_B.PWMxB    = CH_A.PWMxA;
              DCDC_Service.Ptr_PI_Vol_DCV->Out_I    = (int32)CH_A.PWMxA<<DCDC_Service.Ptr_PI_Vol_CV->Q_Type;
              DCDC_Service.Ptr_PI_Vol_DCV->Out      = CH_A.PWMxA;

              //两个通道都使能
              CH_A.EN_Run                   = TRUE;
              CH_B.EN_Run                   = TRUE;
              DCDC_Service.Flag_SingleCH    = FALSE;

              //开启同步整流
              CH_A.EN_SynRect				= TRUE;
              CH_B.EN_SynRect				= TRUE;

              //切换到高电流档位
              SET_CUR_RANGE_H;
              DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
              CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              DCDC_Service.Cur_Fast_OC_AD   = CUR_FAST_OC_H;

              if((CH_A.EN_Run==TRUE)&&(CH_A.EN_Run_Comm==TRUE))
              { PWM1ON; }

              if((CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE))
              { PWM2ON; }
          }
          else if(Interface_DCDC.Req_Mode==DCDC_STATIC)
          {
              //状态机切换
              DCDC_Service.Now_Mode     =DCDC_STATIC;

              //切换到高电流档位
              SET_CUR_RANGE_H;
              DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
              CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值

              //两个通道都使能
              CH_A.EN_Run                   = FALSE;
              CH_B.EN_Run                   = FALSE;
              DCDC_Service.Flag_SingleCH    = FALSE;

              if((CH_A.EN_Run==TRUE)&&(CH_A.EN_Run_Comm==TRUE))
              { PWM1ON; }

              if((CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE))
              { PWM2ON; }

          }
          else if(Interface_DCDC.Req_Mode==DCDC_DEBUG)
          {
              //状态机切换
              DCDC_Service.Now_Mode         =DCDC_DEBUG;

              //切换到高电流档位
              SET_CUR_RANGE_H;
              DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
              CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值
              CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;//防止出现异常将电流给定值设定超出额定电流值

              //两个通道都使能
              if(CH_A.EN_Debug==TRUE)
              {
                  PWM1ON;
              }

              if(CH_B.EN_Debug==TRUE)
              {
                  PWM2ON;
              }
              DCDC_Service.Flag_SingleCH    = FALSE;
          }
          else
          {
              //异常请求都转到待机状态
        	  Interface_DCDC.Req_Mode 		= DCDC_STANDBY;
        	  //状态机切换
              DCDC_Service.Now_Mode         = DCDC_PWM_OFF;
          }
          break;}



      case DCDC_STATIC:
      {
          if(Interface_DCDC.Req_ChangeMode==TRUE)
          {
              DCDC_Service.Now_Mode=DCDC_PWM_OFF;
          }

          if(DCDC_Service.Fault_Module!=0)
          {
              DCDC_Service.Now_Mode=DCDC_PWM_OFF;
          }
              break;}

      case DCDC_DEBUG:
      {
          if(CH_A.EN_Debug==TRUE)
          {
              //上管占空比限制
              if(CH_A.PWMxA_Debug<PWM_DEADTIME)
              {
                  EPwm1Regs.CMPA.half.CMPA=PWM_DEADTIME;
              }
              else if(CH_A.PWMxA_Debug>PWM_PERIOD-PWM_DEADTIME)
              {
                  EPwm1Regs.CMPA.half.CMPA=PWM_PERIOD-PWM_DEADTIME;
              }
              else
              {   EPwm1Regs.CMPA.half.CMPA=CH_A.PWMxA_Debug;    }

              //防止上下管直通
              if(CH_A.PWMxB_Debug>=EPwm1Regs.CMPA.half.CMPA)
              {
                  //下管占空比限制
                  if(CH_A.PWMxB_Debug<PWM_DEADTIME)
                  {
                      EPwm1Regs.CMPB=PWM_DEADTIME;
                  }
                  else if(CH_A.PWMxB_Debug>PWM_PERIOD-PWM_DEADTIME)
                  {
                      EPwm1Regs.CMPB=PWM_PERIOD-PWM_DEADTIME;
                  }
                  else
                  {EPwm1Regs.CMPB=CH_A.PWMxB_Debug;}
              }
              else
              {
                  EPwm1Regs.CMPB=EPwm1Regs.CMPA.half.CMPA;
              }
          }
          else
          {
              EPwm1Regs.CMPA.half.CMPA=PWM_DEADTIME;
              EPwm1Regs.CMPB          =PWM_PERIOD-PWM_DEADTIME;
          }

          if(CH_B.EN_Debug==TRUE)
          {
              //上管占空比限制
              if(CH_B.PWMxA_Debug<PWM_DEADTIME)
              {
                  EPwm2Regs.CMPA.half.CMPA=PWM_DEADTIME;
              }
              else if(CH_B.PWMxA_Debug>PWM_PERIOD-PWM_DEADTIME)
              {
                  EPwm2Regs.CMPA.half.CMPA=PWM_PERIOD-PWM_DEADTIME;
              }
              else
              {EPwm2Regs.CMPA.half.CMPA=CH_B.PWMxA_Debug;}

              //防止上下管直通
              if(CH_B.PWMxB_Debug>=EPwm2Regs.CMPA.half.CMPA)
              {
                  //下管占空比限制
                  if(CH_B.PWMxB_Debug<PWM_DEADTIME)
                  {
                      EPwm2Regs.CMPB=PWM_DEADTIME;
                  }
                  else if(CH_B.PWMxB_Debug>PWM_PERIOD-PWM_DEADTIME)
                  {
                      EPwm2Regs.CMPB=PWM_PERIOD-PWM_DEADTIME;
                  }
                  else
                  {   EPwm2Regs.CMPB=CH_B.PWMxB_Debug;  }
              }
              else
              {
                  EPwm2Regs.CMPB=EPwm2Regs.CMPA.half.CMPA;
              }
          }
          else
          {
              EPwm2Regs.CMPA.half.CMPA=PWM_DEADTIME;
              EPwm2Regs.CMPB          =PWM_PERIOD-PWM_DEADTIME;
          }


          if(Interface_DCDC.Req_ChangeMode==TRUE)
          {
              DCDC_Service.Now_Mode=DCDC_PWM_OFF;
          }

          if(DCDC_Service.Fault_Module!=0)
          {
              DCDC_Service.Now_Mode=DCDC_PWM_OFF;
          }
              break;}

      case DCDC_SOFTOFF_CH:
      {
          DCDC_Service.Flag_CV_Step           = FALSE;
          DCDC_Service.Flag_CV_Step2          = FALSE;
          if((DCDC_Service.Flag_Soft_OFF==FALSE)&&(DCDC_Service.Mode_Parall==PARALL_MASTER))
          {
              DCDC_Service.Flag_Soft_OFF=TRUE;

              Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_ChangeMode  = TRUE;
              Parall_Master.ID1_To_Sx.Cnt_TX_Req_Change_Mode    = 1;
              if(DCDC_Service.Fault_Module!=0)
              {Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Mode        = DCDC_STANDBY; }
              else
              {Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Mode        = Interface_DCDC.Req_Mode;}
              Parall_Master.ID1_To_Sx.ID1_L.Bit.EN_Run = TRUE;
              Parall_Master.ID1_To_Sx.Req_TX = TRUE;
          }

          CH_A.Ptr_PI_Cur_Charge->Ref=CH_A.Ptr_PI_Cur_Charge->Ref-CH_A.Slew_Cur_OFF;
          if(CH_A.Ptr_PI_Cur_Charge->Ref<CUR_SOFTOFF_END)
          {
              CH_A.Ptr_PI_Cur_Charge->Ref=CUR_SOFTOFF_END;
          }

          CH_B.Ptr_PI_Cur_Charge->Ref=CH_B.Ptr_PI_Cur_Charge->Ref-CH_B.Slew_Cur_OFF;
          if(CH_B.Ptr_PI_Cur_Charge->Ref<CUR_SOFTOFF_END)
          {
              CH_B.Ptr_PI_Cur_Charge->Ref=CUR_SOFTOFF_END;
          }

          if((CH_A.Ptr_PI_Cur_Charge->Ref<=CUR_SOFTOFF_END)&&(CH_A.Ptr_PI_Cur_Charge->Ref<=CUR_SOFTOFF_END))
          {
              DCDC_Service.Now_Mode=DCDC_PWM_OFF;
          }
          else
          {
              //A通道电流环计算
              CH_A.Ptr_PI_Cur_Charge->Feed=CH_A.Now_Cur;
              Algorithm_PI(CH_A.Ptr_PI_Cur_Charge);
              CH_A.PWMxA                =CH_A.Ptr_PI_Cur_Charge->Out;
              SynRect_Buck(&CH_A);//做同步整流是否启动判断

              //B通道电流环计算
              CH_B.Ptr_PI_Cur_Charge->Feed=CH_B.Now_Cur;
              Algorithm_PI(CH_B.Ptr_PI_Cur_Charge);
              CH_B.PWMxA                =CH_B.Ptr_PI_Cur_Charge->Out;
              SynRect_Buck(&CH_B);//做同步整流是否启动判断

              EPwm1Regs.CMPA.half.CMPA  =CH_A.PWMxA;
              EPwm1Regs.CMPB            =CH_A.PWMxB;
              EPwm2Regs.CMPA.half.CMPA  =CH_B.PWMxA;
              EPwm2Regs.CMPB            =CH_B.PWMxB;
          }
              break;}

      case DCDC_SOFTOFF_DISCH:
      {
          DCDC_Service.Flag_DCV_Step          = FALSE;
          DCDC_Service.Flag_DCV_Step2         = FALSE;
          if((DCDC_Service.Flag_Soft_OFF==FALSE)&&(DCDC_Service.Mode_Parall==PARALL_MASTER))
          {
              DCDC_Service.Flag_Soft_OFF=TRUE;

              Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_ChangeMode=TRUE;
              Parall_Master.ID1_To_Sx.Cnt_TX_Req_Change_Mode  = 1;

              if(DCDC_Service.Fault_Module!=0)
              {Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Mode        = DCDC_STANDBY; }
              else
              {Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Mode        = Interface_DCDC.Req_Mode;}
              Parall_Master.ID1_To_Sx.ID1_L.Bit.EN_Run = TRUE;
              Parall_Master.ID1_To_Sx.Req_TX = TRUE;
          }

          CH_A.Ptr_PI_Cur_Discharge->Ref=CH_A.Ptr_PI_Cur_Discharge->Ref-CH_A.Slew_Cur_OFF;
          if(CH_A.Ptr_PI_Cur_Discharge->Ref<CUR_SOFTOFF_END)
          {
              CH_A.Ptr_PI_Cur_Discharge->Ref=CUR_SOFTOFF_END;
          }

          CH_B.Ptr_PI_Cur_Discharge->Ref=CH_B.Ptr_PI_Cur_Discharge->Ref-CH_B.Slew_Cur_OFF;
          if(CH_B.Ptr_PI_Cur_Discharge->Ref<CUR_SOFTOFF_END)
          {
              CH_B.Ptr_PI_Cur_Discharge->Ref=CUR_SOFTOFF_END;
          }

          if((CH_A.Ptr_PI_Cur_Discharge->Ref<=CUR_SOFTOFF_END)&&(CH_B.Ptr_PI_Cur_Discharge->Ref<=CUR_SOFTOFF_END))
          {
              DCDC_Service.Now_Mode=DCDC_PWM_OFF;
          }
          else
          {
              //A通道电流环计算
              CH_A.Ptr_PI_Cur_Discharge->Feed=-CH_A.Now_Cur;
              Algorithm_PI(CH_A.Ptr_PI_Cur_Discharge);
              CH_A.PWMxB=PWM_PERIOD-CH_A.Ptr_PI_Cur_Discharge->Out;
              SynRect_Boost(&CH_A);//做同步整流是否启动判断

              //B通道电流环计算
              CH_B.Ptr_PI_Cur_Discharge->Feed=-CH_B.Now_Cur;
              Algorithm_PI(CH_B.Ptr_PI_Cur_Discharge);
              CH_B.PWMxB=PWM_PERIOD-CH_B.Ptr_PI_Cur_Discharge->Out;
              SynRect_Boost(&CH_B);//做同步整流是否启动判断

              EPwm1Regs.CMPA.half.CMPA  =CH_A.PWMxA;
              EPwm1Regs.CMPB            =CH_A.PWMxB;
              EPwm2Regs.CMPA.half.CMPA  =CH_B.PWMxA;
              EPwm2Regs.CMPB            =CH_B.PWMxB;
          }
              break;}

      case DCDC_PWM_OFF:
      {
          //禁止PWM信号输出
          PWM1OFF;
          PWM2OFF;
          DC_OUT_CHA_OFF;
          DC_OUT_CHB_OFF;
          EALLOW;
          EPwm1Regs.TBPRD               = PWM_PERIOD;             // Set timer period
          EPwm2Regs.TBPRD               = PWM_PERIOD;             // Set timer period
          EDIS;

          CH_A.Slew_Cur_ON    = CUR_SLEW_DEFAULT;//42mA/20uS
          CH_B.Slew_Cur_ON    = CUR_SLEW_DEFAULT;//42mA/20uS


          //有故障时目标模式设为待机，防止故障标志被清除后运行其它模式
          if(DCDC_Service.Fault_Module!=0)
          {
              Interface_DCDC.Req_Mode = DCDC_STANDBY;
          }

          //未经过软关断，主机发命令关闭从机
          if((DCDC_Service.Flag_Soft_OFF==FALSE)&&(DCDC_Service.Mode_Parall==PARALL_MASTER))
          {
              Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_ChangeMode=TRUE;
              Parall_Master.ID1_To_Sx.Cnt_TX_Req_Change_Mode    = 1;
              if(DCDC_Service.Fault_Module!=0)
              {Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Mode        = DCDC_STANDBY; }
              else
              {Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Mode        = Interface_DCDC.Req_Mode;}
              Parall_Master.ID1_To_Sx.ID1_L.Bit.EN_Run = TRUE;
              Parall_Master.ID1_To_Sx.Req_TX = TRUE;
          }

          //切换到高电流档位
          SET_CUR_RANGE_H;
          DCDC_Service.Flag_CurRange    = CUR_RANGE_H;
          CH_A.Limit_Set_Cur            = CUR_MAX_CHANNEL;
          CH_B.Limit_Set_Cur            = CUR_MAX_CHANNEL;
          DCDC_Service.Cnt_Delay        = 0;
          DCDC_Service.Now_Mode         = DCDC_SHUTDOWN;
              break;}

      case DCDC_STRUCT_INIT:
      {

          break;}

      default:
      {
          DCDC_Service.Now_Mode=DCDC_PWM_OFF;
          break;}
  }

}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void InitEPWM1_BSP(void)
{
    //
    EPwm1Regs.TZSEL.all            = 0x0100;
    EPwm1Regs.TZCTL.all            = 0x000A;
    EPwm1Regs.TZFRC.all            = 0x0004;

    //
    EPwm1Regs.TBPRD                = PWM_PERIOD;             // Set timer period
    EPwm1Regs.TBPHS.half.TBPHS     = 0x0000;           // Phase is 0
    EPwm1Regs.TBCTR                = 0x0000;           // Clear counter

    // Setup TBCLK
    EPwm1Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
    EPwm1Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
    EPwm1Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

    // Setup compare
    EPwm1Regs.CMPA.half.CMPA       = PWM_DEADTIME;
    EPwm1Regs.CMPB                 = PWM_PERIOD-PWM_DEADTIME;
    // Set actions
    EPwm1Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
    EPwm1Regs.AQCTLA.bit.ZRO       = AQ_SET;
    EPwm1Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
    EPwm1Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
    EPwm1Regs.AQCTLB.bit.ZRO       = AQ_SET;
    EPwm1Regs.AQCTLB.bit.PRD       = AQ_CLEAR;

    // Active Low PWMs - Setup Deadband
    EPwm1Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
    EPwm1Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
    EPwm1Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

    EPwm1Regs.DBRED                = PWM_DEADTIME+1;
    EPwm1Regs.DBFED                = PWM_DEADTIME+1;

    // Interrupt where we will change the Deadband
    EPwm1Regs.ETSEL.bit.INTEN      = 1;                // Enable INT
    EPwm1Regs.ETSEL.bit.INTSEL     = ET_CTR_ZERO;      // Select INT on Zero event
    EPwm1Regs.ETPS.bit.INTPRD      = ET_1ST;           // Generate INT on 1rd event

    EPwm1Regs.ETCLR.all           |= 0x01;
    EPwm1Regs.ETCLR.bit.SOCA       = 0x01;

    EPwm1Regs.TBPHS.half.TBPHS	   = 0;
    EPwm1Regs.TBCTL.bit.SYNCOSEL   = TB_CTR_ZERO;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void InitEPWM2_BSP(void)
{
    //
    EPwm2Regs.TZSEL.all            = 0x0100;
    EPwm2Regs.TZCTL.all            = 0x000A;
    EPwm2Regs.TZFRC.all            = 0x0004;

    //
    EPwm2Regs.TBPRD                = PWM_PERIOD;       // Set timer period
    EPwm2Regs.TBPHS.half.TBPHS     = 0x0000;           // Phase is 0
    EPwm2Regs.TBCTR                = 0x0000;           // Clear counter

    // Setup TBCLK
    EPwm2Regs.TBCTL.bit.CTRMODE    = TB_COUNT_UP;      // Count up
    EPwm2Regs.TBCTL.bit.PHSEN      = TB_DISABLE;       // Disable phase loading
    EPwm2Regs.TBCTL.bit.HSPCLKDIV  = TB_DIV1;          // Clock ratio to SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV     = TB_DIV1;

    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;        // Load registers every ZERO
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;//CC_CTR_ZERO;
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;//CC_CTR_ZERO;

    // Setup compare
    EPwm2Regs.CMPA.half.CMPA       = PWM_DEADTIME;
    EPwm2Regs.CMPB                 = PWM_PERIOD-PWM_DEADTIME;
    // Set actions
    EPwm2Regs.AQCTLA.bit.CAU       = AQ_CLEAR;         // Set PWM1A on Zero
    EPwm2Regs.AQCTLA.bit.PRD       = AQ_CLEAR;
    EPwm2Regs.AQCTLA.bit.ZRO       = AQ_SET;
    EPwm2Regs.AQCTLB.bit.CBU       = AQ_CLEAR;           // Set PWM1A on Zero
    EPwm2Regs.AQCTLB.bit.PRD       = AQ_CLEAR;
    EPwm2Regs.AQCTLB.bit.ZRO       = AQ_SET;

    // Active Low PWMs - Setup Deadband
    EPwm2Regs.DBCTL.bit.IN_MODE    = DBA_RED_DBB_FED;
    EPwm2Regs.DBCTL.bit.OUT_MODE   = DB_FULL_ENABLE;//DBB_ENABLE;
    EPwm2Regs.DBCTL.bit.POLSEL     = DB_ACTV_HIC;

    EPwm2Regs.DBRED                = PWM_DEADTIME+1;
    EPwm2Regs.DBFED                = PWM_DEADTIME+1;

    EPwm2Regs.TBPHS.half.TBPHS	   = 1000;
    EPwm2Regs.TBCTL.bit.PHSEN      = TB_ENABLE;       // Disable phase loading
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void InitEPWM3_BSP(void)
{
    EALLOW;
    //Initilize EPwm3
    EPwm3Regs.TBPRD = 4000;                         // Set timer period
    EPwm3Regs.TBPHS.half.TBPHS = 0x0000;            // Phase is 0
    EPwm3Regs.TBCTR = 0x0000;                       // Clear counter
    // Set Compare values
    EPwm3Regs.CMPA.half.CMPA = 3000;                   // Set compare A value
    // Setup counter mode
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;          // Count up
    EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;             // Disable phase loading
    EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;     // Sync disable
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;            // Clock ratio to SYSCLKOUT
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    // Setup shadowing
    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;        // Load on Period
    EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;
    // Set actions
    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;                // Clare ,PWM1A on up = CMPA
    EPwm3Regs.AQCTLA.bit.PRD = AQ_SET;
    EPwm3Regs.AQCTLB.bit.CBU = AQ_SET;
    EPwm3Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm3Regs.TZSEL.all = 0x0;
    EPwm3Regs.TZCLR.all = 0x0007;
    EPwm3Regs.TZFRC.all = 0x0;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;       // Enable TBCLK within the ePWM
    EDIS;

}

void CH_Init(struct_Channel *Ptr)
{
    Ptr->Set_Cur        = 0;
    Ptr->EN_Run         = TRUE;
    Ptr->EN_Run_Comm    = TRUE;
    Ptr->EN_SynRect     = TRUE;
    Ptr->EN_Debug       = FALSE;
    Ptr->EN_Forward		= FALSE;
    Ptr->PWMxA_Debug    = PWM_DEADTIME;
    Ptr->PWMxB_Debug    = PWM_PERIOD-PWM_DEADTIME;
    Ptr->Slew_Cur_ON    = CUR_SLEW_DEFAULT;//42mA/20uS
    Ptr->Slew_Cur_OFF   = CUR_SLEW_DEFAULT;
    Ptr->Limit_Set_Cur  = CUR_MAX_CHANNEL;

    Ptr->Cnt_SynRect    = 0;
    Ptr->Cnt_SynRect_Off= 0;
    Ptr->Flag_SynRect   = FALSE;
    Ptr->PWMxA          = PWM_DEADTIME;
    Ptr->PWMxB          = PWM_PERIOD-PWM_DEADTIME;
    Ptr->Now_Cur        = 0;
    Ptr->Cnt_Out_OC     = 0;
    Ptr->Flag_MOS_Out	= OFF;

    Ptr->Ptr_PI_Cur_Charge->Cnt                 = 0;
    Ptr->Ptr_PI_Cur_Charge->Period              = 1;
    Ptr->Ptr_PI_Cur_Charge->Q_Type              = 16;//调整前需考虑数据乘法运算溢出的可能性
    Ptr->Ptr_PI_Cur_Charge->Kp                  = PI_Cur_Charge_Kp;
    Ptr->Ptr_PI_Cur_Charge->Ki                  = PI_Cur_Charge_Ki;
    Ptr->Ptr_PI_Cur_Charge->Limit_Err_Max       = 40000;//调整前需考虑数据乘法运算溢出的可能性
    Ptr->Ptr_PI_Cur_Charge->Limit_Err_Min       =-40000;//调整前需考虑数据乘法运算溢出的可能性
    Ptr->Ptr_PI_Cur_Charge->Limit_Out_max       = PWM_MAX_CHARGE;
    Ptr->Ptr_PI_Cur_Charge->Limit_Out_min       = PWM_DEADTIME;

    Ptr->Ptr_PI_Cur_Charge->Limit_PI_max        = Ptr->Ptr_PI_Cur_Charge->Limit_Out_max<<Ptr->Ptr_PI_Cur_Charge->Q_Type;
    Ptr->Ptr_PI_Cur_Charge->Limit_PI_min        = 0;
    Ptr->Ptr_PI_Cur_Charge->Forward_Out			= 0;

    Ptr->Ptr_PI_Cur_Discharge->Cnt              = 0;
    Ptr->Ptr_PI_Cur_Discharge->Period           = 1;
    Ptr->Ptr_PI_Cur_Discharge->Q_Type           = 16;//调整前需考虑数据乘法运算溢出的可能性
    Ptr->Ptr_PI_Cur_Discharge->Kp               = PI_Cur_Discharge_Kp;
    Ptr->Ptr_PI_Cur_Discharge->Ki               = PI_Cur_Discharge_Ki;
    Ptr->Ptr_PI_Cur_Discharge->Limit_Err_Max    = 40000;//调整前需考虑数据乘法运算溢出的可能性
    Ptr->Ptr_PI_Cur_Discharge->Limit_Err_Min    =-40000;//调整前需考虑数据乘法运算溢出的可能性
    Ptr->Ptr_PI_Cur_Discharge->Limit_Out_max    = PWM_MAX_DISCH;
    Ptr->Ptr_PI_Cur_Discharge->Limit_Out_min    = PWM_DEADTIME;

    Ptr->Ptr_PI_Cur_Discharge->Limit_PI_max     = Ptr->Ptr_PI_Cur_Discharge->Limit_Out_max<<Ptr->Ptr_PI_Cur_Discharge->Q_Type;
    Ptr->Ptr_PI_Cur_Discharge->Limit_PI_min     = 0;
    Ptr->Ptr_PI_Cur_Discharge->Forward_Out		= 0;

}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Reset_AD7606(void)
{
    CLEAR_AD7606_RST;
    SET_AD7606_RST;
    asm(" RPT #100 || NOP");
    CLEAR_AD7606_RST;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void InitDCDC_BSP(void)
{
    //PWM硬件初始化
    InitEPWM1_BSP();
    InitEPWM2_BSP();

    PWM1OFF;
    PWM2OFF;

    InitEPWM3_BSP();
    InitSpibGpio();
    InitSPIB_BSP();
    Reset_AD7606();

    //DCDC接口初始化
    Interface_DCDC.Req_Cur_Total        = 0;
    Interface_DCDC.Req_Vol              = 0;
    Interface_DCDC.Req_Refresh_Cur      = FALSE;
    Interface_DCDC.Req_Refresh_Vol      = FALSE;
    Interface_DCDC.Req_CurRange         = CUR_RANGE_H;

    Interface_DCDC.Req_Mode             = DCDC_STANDBY;
    Interface_DCDC.Req_ChangeMode       = FALSE;//
    Interface_DCDC.Parall_Total         = 1;
    Interface_DCDC.EN_Run               = TRUE;
    Interface_DCDC.EN_MOS_Out           = ON;
    Interface_DCDC.EN_CHA_Comm          = TRUE;
    Interface_DCDC.EN_CHA_SynRect       = TRUE;
    Interface_DCDC.EN_CHB_Comm          = TRUE;
    Interface_DCDC.EN_CHB_SynRect       = TRUE;
    Interface_DCDC.Mode_Parall          = SINGLE;
    Interface_DCDC.Req_Aging            = FALSE;
    Interface_DCDC.EN_Run_Slaver        = TRUE;
    Interface_DCDC.EN_MOS_Out_Slaver    = ON;

    //DCDC服务数据初始化
    DCDC_Service.Set_Cur_Total          = 0;
    DCDC_Service.Set_Vol                = 0;
    DCDC_Service.Parall_Total           = 1;//单机
    DCDC_Service.Mode_Parall            = SINGLE;//单机
    DCDC_Service.Flag_Aging             = FALSE;

    DCDC_Service.Now_Mode               = DCDC_STRUCT_INIT;
    DCDC_Service.Now_Cur_AB             = 0;
    DCDC_Service.Now_Vol_Sense          = 0;
    DCDC_Service.Now_Vol_Out            = 0;
    DCDC_Service.Now_Vol_Bus            = 0;

    DCDC_Service.Cnt_Delay              = 0;
    DCDC_Service.Cnt_CCtoCV             = 0;
    DCDC_Service.Cnt_CCtoCV2             = 0;
    DCDC_Service.Flag_CV_Step           = FALSE;
    DCDC_Service.Flag_CV_Step2          = FALSE;
    DCDC_Service.Cnt_DCCtoDCV           = 0;
    DCDC_Service.Cnt_DCCtoDCV2           = 0;
    DCDC_Service.Flag_DCV_Step          = FALSE;
    DCDC_Service.Flag_DCV_Step2         = FALSE;
    DCDC_Service.Cur_Fast_OC_AD         = CUR_FAST_OC_H;

    DCDC_Service.Flag_SingleCH          = FALSE;
    DCDC_Service.Flag_CurRange          = CUR_RANGE_H;
    DCDC_Service.Flag_Soft_OFF          = FALSE;

    DCDC_Service.Limit_Set_Vol          = VOL_MAX_SENSE;
    DCDC_Service.Slew_Vol_ON            = 100;//10mV/20uS
    DCDC_Service.Cnt_Bus_H              = 0;
    DCDC_Service.Cnt_Bus_L              = 0;
    DCDC_Service.Cnt_Out_H              = 0;
    DCDC_Service.Cnt_Fault_AD7606       = 0;

    DCDC_Service.Fault_Module           = 0;
    DCDC_Service.Fault_Slaver1          = 0;
    DCDC_Service.Fault_Slaver2          = 0;
    DCDC_Service.Fault_Slaver3          = 0;
    DCDC_Service.Fault_Slaver4          = 0;

    DCDC_Service.Vol_Sense_LPF_Last     = 0;
    DCDC_Service.Cnt_Sum_Vol_Sense      = 0;
    DCDC_Service.Num_Sum_Vol_Sense      = 50;

    //电压环PI结构体指针初始化
    DCDC_Service.Ptr_PI_Vol_CV          = &PI_Vol_CV;
    DCDC_Service.Ptr_PI_Vol_Charge      = &PI_Vol_Charge;
    DCDC_Service.Ptr_PI_Vol_DCV         = &PI_Vol_DCV;
    DCDC_Service.Ptr_PI_Vol_Discharge   = &PI_Vol_Discharge;

    DCDC_Service.Ptr_PI_Vol_CV->Cnt                 = 0;
    DCDC_Service.Ptr_PI_Vol_CV->Period              = 2;
    DCDC_Service.Ptr_PI_Vol_CV->Q_Type              = 19;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_CV->Kp                  = PI_Vol_CV_Kp;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_CV->Ki                  = PI_Vol_CV_Ki;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_CV->Limit_Err_Max       = 2000;//200mV
    DCDC_Service.Ptr_PI_Vol_CV->Limit_Err_Min       =-2000;//-200mV
    DCDC_Service.Ptr_PI_Vol_CV->Limit_Out_max       = PWM_MAX_CV;
    DCDC_Service.Ptr_PI_Vol_CV->Limit_PI_max        = DCDC_Service.Ptr_PI_Vol_CV->Limit_Out_max<<DCDC_Service.Ptr_PI_Vol_CV->Q_Type;
    DCDC_Service.Ptr_PI_Vol_CV->Limit_PI_min        = 0;
    DCDC_Service.Ptr_PI_Vol_CV->Limit_Out_min       = PWM_DEADTIME;
    DCDC_Service.Ptr_PI_Vol_CV->Forward_Out			= 0;

    DCDC_Service.Ptr_PI_Vol_Charge->Cnt             = 0;
    DCDC_Service.Ptr_PI_Vol_Charge->Period          = 1000;//20mS周期
    DCDC_Service.Ptr_PI_Vol_Charge->Q_Type          = 4;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_Charge->Kp              = PI_Vol_Charge_Kp;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_Charge->Ki              = PI_Vol_Charge_Ki;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_Charge->Limit_Err_Max   = 25;//2000;//200mV
    DCDC_Service.Ptr_PI_Vol_Charge->Limit_Err_Min   =-25;//-2000;//-200mV
    DCDC_Service.Ptr_PI_Vol_Charge->Limit_Out_max   = CUR_MAX_CHANNEL*2;//并机设定时需要再次设置,由并机数量决定
    DCDC_Service.Ptr_PI_Vol_Charge->Limit_PI_max    = DCDC_Service.Ptr_PI_Vol_Charge->Limit_Out_max<<DCDC_Service.Ptr_PI_Vol_Charge->Q_Type;
    DCDC_Service.Ptr_PI_Vol_Charge->Limit_PI_min    = 0;
    DCDC_Service.Ptr_PI_Vol_Charge->Limit_Out_min   = 0;
    DCDC_Service.Ptr_PI_Vol_Charge->Forward_Out		= 0;

    DCDC_Service.Ptr_PI_Vol_DCV->Cnt                = 0;
    DCDC_Service.Ptr_PI_Vol_DCV->Period             = 2;
    DCDC_Service.Ptr_PI_Vol_DCV->Q_Type             = 15;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_DCV->Kp                 = PI_Vol_DCV_Kp;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_DCV->Ki                 = PI_Vol_DCV_Ki;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_DCV->Limit_Err_Max      = 2000;//200mV
    DCDC_Service.Ptr_PI_Vol_DCV->Limit_Err_Min      =-2000;//-200mV
    DCDC_Service.Ptr_PI_Vol_DCV->Limit_Out_max      = PWM_MAX_DCV;
    DCDC_Service.Ptr_PI_Vol_DCV->Limit_PI_max       = DCDC_Service.Ptr_PI_Vol_DCV->Limit_Out_max<<DCDC_Service.Ptr_PI_Vol_DCV->Q_Type;
    DCDC_Service.Ptr_PI_Vol_DCV->Limit_PI_min       = 0;
    DCDC_Service.Ptr_PI_Vol_DCV->Limit_Out_min      = PWM_DEADTIME;
    DCDC_Service.Ptr_PI_Vol_DCV->Forward_Out		= 0;

    DCDC_Service.Ptr_PI_Vol_Discharge->Cnt          = 0;
    DCDC_Service.Ptr_PI_Vol_Discharge->Period       = 1000;//20mS周期
    DCDC_Service.Ptr_PI_Vol_Discharge->Q_Type       = 4;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_Discharge->Kp           = PI_Vol_Discharge_Kp;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_Discharge->Ki           = PI_Vol_Discharge_Ki;//调整前需考虑数据乘法运算溢出的可能性
    DCDC_Service.Ptr_PI_Vol_Discharge->Limit_Err_Max= 25;//2000  //200mV
    DCDC_Service.Ptr_PI_Vol_Discharge->Limit_Err_Min=-25;//-2000 //-200mV
    DCDC_Service.Ptr_PI_Vol_Discharge->Limit_Out_max= CUR_MAX_CHANNEL*2;//并机设定时需要再次设置,由并机数量决定
    DCDC_Service.Ptr_PI_Vol_Discharge->Limit_PI_max = DCDC_Service.Ptr_PI_Vol_Discharge->Limit_Out_max<<DCDC_Service.Ptr_PI_Vol_Discharge->Q_Type;
    DCDC_Service.Ptr_PI_Vol_Discharge->Limit_PI_min = 0;
    DCDC_Service.Ptr_PI_Vol_Discharge->Forward_Out	= 0;

    DCDC_Service.Ptr_PI_Vol_Discharge->Limit_Out_min= 0;

    //Bus电压定标转换参数初始化
    DCDC_Service.Vol_Scale.K_ROM_Bus    = VOL_K_BUS_DEFAULT;
    DCDC_Service.Vol_Scale.K_Now_Bus    = DCDC_Service.Vol_Scale.K_ROM_Bus*10;

    //Out电压、Sense电压校准数据初始化为默认值
    DCDC_Service.Vol_Scale.K_ROM_Out    = VOL_K_OUT_DEFAULT;
    DCDC_Service.Vol_Scale.B_ROM_Out    = VOL_B_OUT_DEFAULT;
    DCDC_Service.Vol_Scale.K_Now_Out    = DCDC_Service.Vol_Scale.K_ROM_Out*10;
    DCDC_Service.Vol_Scale.B_Now_Out    = DCDC_Service.Vol_Scale.B_ROM_Out*10;


    DCDC_Service.Vol_Scale.K_ROM_Sense  = VOL_K_SENSE_DEFAULT;
    DCDC_Service.Vol_Scale.B_ROM_Sense  = VOL_B_SENSE_DEFAULT;
    DCDC_Service.Vol_Scale.K_Now_Sense  = DCDC_Service.Vol_Scale.K_ROM_Sense*10;
    DCDC_Service.Vol_Scale.B_Now_Sense  = DCDC_Service.Vol_Scale.B_ROM_Sense*10;

    //A B通道高低电流档校准数据初始化为默认值
    CH_A.Cur_Scale_H.K_ROM_CH   = CUR_H_K_DEFAULT;
    CH_A.Cur_Scale_H.B_ROM_CH   = CUR_H_B_DEFAULT;
    CH_A.Cur_Scale_H.K_ROM_DIS  = CUR_H_K_DEFAULT;
    CH_A.Cur_Scale_H.B_ROM_DIS  = CUR_H_B_DEFAULT;
    CH_A.Cur_Scale_H.B_C_Res    = 0;
    CH_A.Cur_Scale_H.B_C_Cur    = 0;
    CH_A.Cur_Scale_H.B_C_Env    = 0;
    CH_A.Cur_Scale_H.K_Now      = CH_A.Cur_Scale_H.K_ROM_CH*10;
    CH_A.Cur_Scale_H.B_Now      = CH_A.Cur_Scale_H.B_ROM_CH*10;


    CH_A.Cur_Scale_L.K_ROM_CH   = CUR_L_K_DEFAULT;
    CH_A.Cur_Scale_L.B_ROM_CH   = CUR_L_B_DEFAULT;
    CH_A.Cur_Scale_L.K_ROM_DIS  = CUR_L_K_DEFAULT;
    CH_A.Cur_Scale_L.B_ROM_DIS  = CUR_L_B_DEFAULT;
    CH_A.Cur_Scale_L.B_C_Res    = 0;
    CH_A.Cur_Scale_L.B_C_Cur    = 0;
    CH_A.Cur_Scale_L.B_C_Env    = 0;
    CH_A.Cur_Scale_L.K_Now      = CH_A.Cur_Scale_L.K_ROM_CH*10;
    CH_A.Cur_Scale_L.B_Now      = CH_A.Cur_Scale_L.B_ROM_CH*10;


    CH_B.Cur_Scale_H.K_ROM_CH   = CUR_H_K_DEFAULT;
    CH_B.Cur_Scale_H.B_ROM_CH   = CUR_H_B_DEFAULT;
    CH_B.Cur_Scale_H.K_ROM_DIS  = CUR_H_K_DEFAULT;
    CH_B.Cur_Scale_H.B_ROM_DIS  = CUR_H_B_DEFAULT;
    CH_B.Cur_Scale_H.B_C_Res    = 0;
    CH_B.Cur_Scale_H.B_C_Cur    = 0;
    CH_B.Cur_Scale_H.B_C_Env    = 0;
    CH_B.Cur_Scale_H.K_Now      = CH_B.Cur_Scale_H.K_ROM_CH*10;
    CH_B.Cur_Scale_H.B_Now      = CH_B.Cur_Scale_H.B_ROM_CH*10;

    CH_B.Cur_Scale_L.K_ROM_CH   = CUR_L_K_DEFAULT;
    CH_B.Cur_Scale_L.B_ROM_CH   = CUR_L_B_DEFAULT;
    CH_B.Cur_Scale_L.K_ROM_DIS  = CUR_L_K_DEFAULT;
    CH_B.Cur_Scale_L.B_ROM_DIS  = CUR_L_B_DEFAULT;
    CH_B.Cur_Scale_L.B_C_Res    = 0;
    CH_B.Cur_Scale_L.B_C_Cur    = 0;
    CH_B.Cur_Scale_L.B_C_Env    = 0;
    CH_B.Cur_Scale_L.K_Now      = CH_B.Cur_Scale_L.K_ROM_CH*10;
    CH_B.Cur_Scale_L.B_Now      = CH_B.Cur_Scale_L.B_ROM_CH*10;

    //A，B通道电流环PI参数指针初始化
    CH_A.Ptr_PI_Cur_Charge      = &CH_A_PI_Cur_Charge;
    CH_A.Ptr_PI_Cur_Discharge   = &CH_A_PI_Cur_Discharge;
    CH_B.Ptr_PI_Cur_Charge      = &CH_B_PI_Cur_Charge;
    CH_B.Ptr_PI_Cur_Discharge   = &CH_B_PI_Cur_Discharge;

    //A，B通道内部参数初始化
    CH_Init(&CH_A);
    CH_Init(&CH_B);

    Data_Sum.Num_Avg            = 5000;//0.08S,修改时需考虑数据累加溢出的可能
    Data_Sum.Cnt_Sum            = 0;
    Data_Sum.Flag_Refresh       = FALSE;
    Data_Sum.Sum_AD_NTC1        = 0;
    Data_Sum.Sum_AD_NTC2        = 0;
    Data_Sum.Sum_AD_PT          = 0;
    Data_Sum.Sum_Cur_CHA        = 0;
    Data_Sum.Sum_Cur_CHB        = 0;
    Data_Sum.Sum_Vol_Bus        = 0;
    Data_Sum.Sum_Vol_Out        = 0;
    Data_Sum.Sum_Vol_Sense      = 0;
    Data_Sum.AD_NTC1            = 0;
    Data_Sum.AD_NTC2            = 0;
    Data_Sum.AD_PT              = 0;
    Data_Sum.Cur_CHA            = 0;
    Data_Sum.Cur_CHB            = 0;
    Data_Sum.Vol_Out            = 0;
    Data_Sum.Sum_Vol_Out_Copy   = 0;
    Data_Sum.Vol_Sense          = 0;
    Data_Sum.Sum_Vol_Sense_Copy = 0;
    Data_Sum.Vol_Contact		= 0;
    Data_Sum.Temp_NTC1          = 250;
    Data_Sum.Temp_NTC2          = 250;
    Data_Sum.Temp_PT            = 250;
    Data_Sum.Vol_5V             = 50000;

    Data_AD_Sum.Num_Avg         = 50000;//修改时需考虑数据累加溢出的可能
    Data_AD_Sum.Cnt_Sum         = 0;
    Data_AD_Sum.Sum_Cur_CHA_AD  = 0;
    Data_AD_Sum.Sum_Cur_CHB_AD  = 0;
    Data_AD_Sum.Sum_Vol_Bus_AD  = 0;
    Data_AD_Sum.Sum_Vol_Out_AD  = 0;
    Data_AD_Sum.Sum_Vol_Sense_AD= 0;
    Data_AD_Sum.Sum_Cur_CHA_AD_Copy  = 0;
    Data_AD_Sum.Sum_Cur_CHB_AD_Copy  = 0;
    Data_AD_Sum.Sum_Vol_Bus_AD_Copy  = 0;
    Data_AD_Sum.Sum_Vol_Out_AD_Copy  = 0;
    Data_AD_Sum.Sum_Vol_Sense_AD_Copy= 0;

    Bus_Forward.Limit_Pos           = 2800;//约3V
    Bus_Forward.Limit_Neg           =-2800;//约-3V
    Bus_Forward.K_Forward           = 700;
    Bus_Forward.Cnt_Sample          = 0;
    Bus_Forward.PWM_Forward         = 0;
    Bus_Forward.Vol_Bus_Fast        = 0;
    Bus_Forward.Vol_Bus_Fast_Last   = 0;
    Bus_Forward.Vol_Bus_Var         = 0;
    Bus_Forward.Vol_Bus_Slow        = 0;
    Bus_Forward.Vol_Bus_Slow_Last   = 0;
    Bus_Forward.K_Forward_Charge    = 700;
    Bus_Forward.K_Forward_Discharge = 550;

    Bus_Fluct_Check.Num_Mask        = 4000;//80mS Bus电压波动屏蔽时间，
    Bus_Fluct_Check.Flag_Bus_Low    = TRUE;
    Bus_Fluct_Check.Flag_Bus_Fluct  = FALSE;
    Bus_Fluct_Check.Cnt_Mask        = 0;
    Bus_Fluct_Check.Cnt_Delay       = 0;

    DCDC_Service.Now_Mode       	= DCDC_SHUTDOWN;

    Adjust_PI_Vol_Charge.Limit_Max          = PI_Vol_Charge_Ki;
    Adjust_PI_Vol_Charge.Limit_Min          = 40;//40*10*50/16=125mA/mV/S
    Adjust_PI_Vol_Charge.Num                = 5;
    Adjust_PI_Vol_Charge.Ref_Vol_Error      =-2;
    Adjust_PI_Vol_Charge.Cnt_Rise           = 0;
    Adjust_PI_Vol_Charge.Cnt_Fall           = 0;

    Adjust_PI_Vol_Discharge.Limit_Max       = PI_Vol_Discharge_Ki;
    Adjust_PI_Vol_Discharge.Limit_Min       =40;//40*10*50/16=125mA/mV/S
    Adjust_PI_Vol_Discharge.Num             = 5;
    Adjust_PI_Vol_Discharge.Ref_Vol_Error   =-2;
    Adjust_PI_Vol_Discharge.Cnt_Rise        = 0;
    Adjust_PI_Vol_Discharge.Cnt_Fall        = 0;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Auto_Adjust_PI_Vol_Charge(void)
{
    if(DCDC_Service.Time_Running<100001)
    {
        return;
    }


    if((DCDC_Service.Now_Mode==DCDC_CCtoCV)&&(DCDC_Service.Flag_CV_Step ==TRUE))
    {
        int32 Temp=PI_Vol_Charge.Ki;

        if(DCDC_Service.Cnt_Time_CV_DCV<850)
        {
            DCDC_Service.Cnt_Time_CV_DCV++;
        }
        else
        {
            DCDC_Service.Cnt_Time_CV_DCV=850;
        }

        if(DCDC_Service.Cnt_Time_CV_DCV<100)
        {
            return;
        }

        if(DCDC_Service.Cnt_Time_CV_DCV<850)
        {
            if((PI_Vol_Charge.Error>-20)&&(PI_Vol_Charge.Error<0))
            {
                Adjust_PI_Vol_Charge.Cnt_Rise=0;
                if(++Adjust_PI_Vol_Charge.Cnt_Fall>=Adjust_PI_Vol_Charge.Num)
                {
                    Adjust_PI_Vol_Charge.Cnt_Fall=0;
                     Temp=Temp-4;
                }
            }
            else
            {
                Adjust_PI_Vol_Charge.Cnt_Fall=0;
                if(++Adjust_PI_Vol_Charge.Cnt_Rise>=Adjust_PI_Vol_Charge.Num)
                {
                     Adjust_PI_Vol_Charge.Cnt_Rise=0;
                     Temp=Temp+1;
                }
            }
        }
        else
        {
            if(PI_Vol_Charge.Error<=Adjust_PI_Vol_Charge.Ref_Vol_Error-2)
            {
                Adjust_PI_Vol_Charge.Cnt_Fall=0;
                if(++Adjust_PI_Vol_Charge.Cnt_Rise>=Adjust_PI_Vol_Charge.Num)
                {
                    Adjust_PI_Vol_Charge.Cnt_Rise=0;
                    Temp=Temp+2;
                }
            }
            else if(PI_Vol_Charge.Error<=Adjust_PI_Vol_Charge.Ref_Vol_Error-1)
            {
                Adjust_PI_Vol_Charge.Cnt_Fall=0;
                if(++Adjust_PI_Vol_Charge.Cnt_Rise>=Adjust_PI_Vol_Charge.Num)
                {
                    Adjust_PI_Vol_Charge.Cnt_Rise=0;
                    Temp=Temp+1;
                }
            }
            else if((PI_Vol_Charge.Error>0)||(PI_Vol_Charge.Error==Adjust_PI_Vol_Charge.Ref_Vol_Error))
            {
                Adjust_PI_Vol_Charge.Cnt_Fall=0;
                Adjust_PI_Vol_Charge.Cnt_Rise=0;
            }
            else if(PI_Vol_Charge.Error>=Adjust_PI_Vol_Charge.Ref_Vol_Error+2)
            {
                Adjust_PI_Vol_Charge.Cnt_Rise=0;
                if(++Adjust_PI_Vol_Charge.Cnt_Fall>=Adjust_PI_Vol_Charge.Num)
                {
                    Adjust_PI_Vol_Charge.Cnt_Fall=0;
                    Temp=Temp-2;
                }
            }
            else if(PI_Vol_Charge.Error>=Adjust_PI_Vol_Charge.Ref_Vol_Error+1)
            {
                Adjust_PI_Vol_Charge.Cnt_Rise=0;
                if(++Adjust_PI_Vol_Charge.Cnt_Fall>=Adjust_PI_Vol_Charge.Num)
                {
                    Adjust_PI_Vol_Charge.Cnt_Fall=0;
                    Temp=Temp-1;
                }
            }
        }

        if(Temp>Adjust_PI_Vol_Charge.Limit_Max)
        {
            Temp=Adjust_PI_Vol_Charge.Limit_Max;
        }
        else if(Temp<Adjust_PI_Vol_Charge.Limit_Min)
        {
            Temp=Adjust_PI_Vol_Charge.Limit_Min;
        }

        DINT;
        PI_Vol_Charge.Ki=Temp;
        EINT;
    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Auto_Adjust_PI_Vol_Discharge(void)
{
    if(DCDC_Service.Time_Running<100001)
    {
        return;
    }

    if((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step ==TRUE))
    {
        int32 Temp=PI_Vol_Discharge.Ki;

        if(DCDC_Service.Cnt_Time_CV_DCV<850)
        {
            DCDC_Service.Cnt_Time_CV_DCV++;
        }
        else
        {
            DCDC_Service.Cnt_Time_CV_DCV=850;
        }

        if(DCDC_Service.Cnt_Time_CV_DCV<50)
        {
            return;
        }

        if(DCDC_Service.Cnt_Time_CV_DCV<850)
        {
            if((PI_Vol_Discharge.Error>-20)&&(PI_Vol_Discharge.Error<0))
            {
                Adjust_PI_Vol_Discharge.Cnt_Rise=0;
                if(++Adjust_PI_Vol_Discharge.Cnt_Fall>=Adjust_PI_Vol_Discharge.Num)
                {
                    Adjust_PI_Vol_Discharge.Cnt_Fall=0;
                     Temp=Temp-6;
                }
            }
            else
            {
                Adjust_PI_Vol_Discharge.Cnt_Fall=0;
                if(++Adjust_PI_Vol_Discharge.Cnt_Rise>=Adjust_PI_Vol_Discharge.Num)
                {
                     Adjust_PI_Vol_Discharge.Cnt_Rise=0;
                     Temp=Temp+1;
                }
            }
        }
        else
        {
            if(PI_Vol_Discharge.Error<=Adjust_PI_Vol_Discharge.Ref_Vol_Error-2)
            {
                Adjust_PI_Vol_Discharge.Cnt_Fall=0;
                if(++Adjust_PI_Vol_Discharge.Cnt_Rise>=Adjust_PI_Vol_Discharge.Num)
                {
                    Adjust_PI_Vol_Discharge.Cnt_Rise=0;
                    Temp=Temp+2;
                }
            }
            else if(PI_Vol_Discharge.Error<=Adjust_PI_Vol_Discharge.Ref_Vol_Error-1)
            {
                Adjust_PI_Vol_Discharge.Cnt_Fall=0;
                if(++Adjust_PI_Vol_Discharge.Cnt_Rise>=Adjust_PI_Vol_Discharge.Num)
                {
                    Adjust_PI_Vol_Discharge.Cnt_Rise=0;
                    Temp=Temp+1;
                }
            }
            else if((PI_Vol_Discharge.Error>0)||(PI_Vol_Discharge.Error==Adjust_PI_Vol_Discharge.Ref_Vol_Error))
            {
                Adjust_PI_Vol_Discharge.Cnt_Fall=0;
                Adjust_PI_Vol_Discharge.Cnt_Rise=0;
            }
            else if(PI_Vol_Discharge.Error>=Adjust_PI_Vol_Discharge.Ref_Vol_Error+2)
            {
                Adjust_PI_Vol_Discharge.Cnt_Rise=0;
                if(++Adjust_PI_Vol_Discharge.Cnt_Fall>=Adjust_PI_Vol_Discharge.Num)
                {
                    Adjust_PI_Vol_Discharge.Cnt_Fall=0;
                    Temp=Temp-2;
                }
            }
            else if(PI_Vol_Discharge.Error>=Adjust_PI_Vol_Discharge.Ref_Vol_Error+1)
            {
                Adjust_PI_Vol_Discharge.Cnt_Rise=0;
                if(++Adjust_PI_Vol_Discharge.Cnt_Fall>=Adjust_PI_Vol_Discharge.Num)
                {
                    Adjust_PI_Vol_Discharge.Cnt_Fall=0;
                    Temp=Temp-1;
                }
            }
        }

        if(Temp>Adjust_PI_Vol_Discharge.Limit_Max)
        {
            Temp=Adjust_PI_Vol_Discharge.Limit_Max;
        }
        else if(Temp<Adjust_PI_Vol_Discharge.Limit_Min)
        {
            Temp=Adjust_PI_Vol_Discharge.Limit_Min;
        }

        DINT;
        PI_Vol_Discharge.Ki=Temp;
        EINT;
    }
}

//File end
