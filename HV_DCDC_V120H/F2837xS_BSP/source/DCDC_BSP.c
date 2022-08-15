/*
 * DCDC_BSP.c
 *
 *  Created on: 2021年6月19日
 *      Author: JC
 */

#include "F28x_Project.h"
Uint32    *ExRamStart = (Uint32 *)0x380000;

volatile Uint16 Tm200us;
volatile Uint16 Tm1ms;
volatile Uint16 Tm2ms;
volatile Uint16 Tm10ms;
volatile Uint16 Tm1s=0;
PI_CLA CH_PI_Vout[CHANNELS];
PI_CLA CH_PI_Vsense[CHANNELS];
//PI_CLA CH_A_PI_Cur[CHANNELS];
//PI_CLA CH_B_PI_Cur[CHANNELS];
PI_CLA CH_PI_Iout[CHANNELS];
//struct_Channel CH_A[CHANNELS];                    //CHA相关数据
//struct_Channel CH_B[CHANNELS];                    //CHB相关数据
INTERFACE_DCDC Interface_DCDC[CHANNELS];          //DCDC模块工作参数请求接口
struct_DCDC_SERVICE DCDC_Service[CHANNELS];       //DCDC状态机相关数据
struct_FILTER_COEFF Filter_Coeff;
struct_Data_AD_Sum Data_AD_Sum[CHANNELS];        //校准用AD数据累加值
struct_Data_Sum Data_Sum[CHANNELS];           //定标转换后的物理量进行平均后得到的值
struct_CH_PUBLIC_VARIABLE  CH_Public_Variable;

Uint32 gCputime0_time1,gCputime0_time2,gCputime0_time3=0,gCputime0_max=0;
Uint16 cnt=0,count=0;
Uint16 Cnt_Tick=0,time_interupt=0;
float32 test[200];
float32 testm[200];
Uint32 Read_Data[8];
Uint32 Middle_Data[8];
int16 Result_Data[8];
float test_value[4];

extern float Compen_Cur[4];
Uint16 waitimecnt=0,waitimecnt1=0;
float Temp235;

#pragma CODE_SECTION(EPWM1_INT_ISR_BSP,".TI.ramfunc");


interrupt void EPWM1_INT_ISR_BSP(void)
{
    Uint16 i,j;
    gCputime0_time1=CpuTimer0.RegsAddr->TIM.all;

    fVal++;
    Tm1ms++;
    Tm2ms++;
    Tm10ms++;
    Tm1s++;

    Filter_Coeff.Ad_Result[0][2] = (int16)(AdcbResultRegs.ADCRESULT0)*0.017725512-3.438749328; //第一个通道输出电压
    Filter_Coeff.Ad_Result[1][2] = (int16)(AdcbResultRegs.ADCRESULT1)*0.017834182-3.459831308; //第二个通道输出电压
    Filter_Coeff.Ad_Result[2][2] = (int16)(AdcbResultRegs.ADCRESULT2)*0.017798921-3.455488483;  //第三个通道输出电压
    Filter_Coeff.Ad_Result[3][2] = (int16)(AdcbResultRegs.ADCRESULT3)*0.017790501-3.427839427;  //第四个通道输出电压//0.0177745599856322
    DCDC_Service[0].Now_Vol_Out= Filter_Coeff.Ad_Result[0][2];
    DCDC_Service[1].Now_Vol_Out= Filter_Coeff.Ad_Result[1][2];
    DCDC_Service[2].Now_Vol_Out= Filter_Coeff.Ad_Result[2][2];
    DCDC_Service[3].Now_Vol_Out= Filter_Coeff.Ad_Result[3][2];


    Filter_Coeff.Ad_Result[0][3] = AdccResultRegs.ADCRESULT2*0.022023829+0.161537808; //母线电压 84V对应2.9232V
    Filter_Coeff.Ad_Result[1][3] = AdcdResultRegs.ADCRESULT0*0.021983571+0.234380817; //母线电压
    Filter_Coeff.Ad_Result[2][3] = AdccResultRegs.ADCRESULT4*0.022059186+0.179203917; //母线电压
    Filter_Coeff.Ad_Result[3][3] = AdcdResultRegs.ADCRESULT2*0.022780994+0.232382556; //母线电压




    DCDC_Service[0].Now_Vol_Bus=Filter_Coeff.Ad_Result[0][3];
    DCDC_Service[1].Now_Vol_Bus=Filter_Coeff.Ad_Result[1][3];
    DCDC_Service[2].Now_Vol_Bus=Filter_Coeff.Ad_Result[2][3];
    DCDC_Service[3].Now_Vol_Bus=Filter_Coeff.Ad_Result[3][3];

    Temp235=AdcdResultRegs.ADCRESULT3*0.0006103515625;
    Temp235=(Temp235-0.5)*100;


    //AD7606数据读取，外部扩展口处理， 由于boot的与外部扩展口共同占用GPIO72管脚，故外部扩展口从EM1D12整体上移一位到EM1D16
    Read_Data[0]=*RDADC_DAT;
    Middle_Data[0]=(Read_Data[0]&0x0001E000)>>1;
    Result_Data[0]=(Middle_Data[0]&0x0000F000)|(Read_Data[0]&0x00000FFF);
    Read_Data[1]=*RDADC_DAT;
    Middle_Data[1]=(Read_Data[1]&0x0001E000)>>1;
    Result_Data[1]=(Middle_Data[1]&0x0000F000)|(Read_Data[1]&0x00000FFF);
    Read_Data[2]=*RDADC_DAT;
    Middle_Data[2]=(Read_Data[2]&0x0001E000)>>1;
    Result_Data[2]=(Middle_Data[2]&0x0000F000)|(Read_Data[2]&0x00000FFF);
    Read_Data[3]=*RDADC_DAT;
    Middle_Data[3]=(Read_Data[3]&0x0001E000)>>1;
    Result_Data[3]=(Middle_Data[3]&0x0000F000)|(Read_Data[3]&0x00000FFF);
    Read_Data[4]=*RDADC_DAT;
    Middle_Data[4]=(Read_Data[4]&0x0001E000)>>1;
    Result_Data[4]=(Middle_Data[4]&0x0000F000)|(Read_Data[4]&0x00000FFF);
    Read_Data[5]=*RDADC_DAT;
    Middle_Data[5]=(Read_Data[5]&0x0001E000)>>1;
    Result_Data[5]=(Middle_Data[5]&0x0000F000)|(Read_Data[5]&0x00000FFF);
    Read_Data[6]=*RDADC_DAT;
    Middle_Data[6]=(Read_Data[6]&0x0001E000)>>1;
    Result_Data[6]=(Middle_Data[6]&0x0000F000)|(Read_Data[6]&0x00000FFF);
    Read_Data[7]=*RDADC_DAT;
    Middle_Data[7]=(Read_Data[7]&0x0001E000)>>1;
    Result_Data[7]=((Middle_Data[7]&0x0000F000)|(Read_Data[7]&0x00000FFF));


    Filter_Coeff.Ad_Result[3][4] = Result_Data[0]*0.002179827; //电池电压4通道   60V对应4.2V
    Filter_Coeff.Ad_Result[2][4] = Result_Data[1]*0.002179827; //电池电压3通道
    Filter_Coeff.Ad_Result[1][4] = Result_Data[2]*0.002182896+0.0008; //电池电压2通道
    Filter_Coeff.Ad_Result[0][4] = Result_Data[3]*0.002181633+0.00214453; //电池电压1通道

    Filter_Coeff.Ad_Result[3][5] = Result_Data[4]*0.004048185-0.0037+Compen_Cur[3];//0.00762939453125; //0.0030517578125; //输出电流4通道
    Filter_Coeff.Ad_Result[2][5] = Result_Data[5]*0.004048185-0.0037+Compen_Cur[2]; //0.0030517578125; //输出电流3通道
    Filter_Coeff.Ad_Result[1][5] = Result_Data[6]*0.004042683-0.026367+Compen_Cur[1];//0.0030517578125; //输出电流2通道
    Filter_Coeff.Ad_Result[0][5] = Result_Data[7]*0.004048242+0.00874+Compen_Cur[0];//0.0030517578125; //输出电流1通道


    Data_AD_Sum[1].Sum_Cur_Out_AD += Result_Data[6];//输出电流AD累加
    Data_AD_Sum[3].Sum_Vol_Out_AD += AdcbResultRegs.ADCRESULT3;//输出电压AD累加
    Data_AD_Sum[1].Sum_Vol_Sense_AD += Result_Data[2];//电池电压AD累加


    // 累加AD 取平均值
    if(++Data_AD_Sum[0].Cnt_Sum>=Data_AD_Sum[0].Num_Avg)
    {

          Data_AD_Sum[0].Cnt_Sum=0;
          Data_AD_Sum[0].Sum_Cur_Out_AD_Copy = Data_AD_Sum[0].Sum_Cur_Out_AD;
          Data_AD_Sum[0].Sum_Cur_Out_AD=0;

          Data_AD_Sum[3].Sum_Vol_Out_AD_Copy = Data_AD_Sum[3].Sum_Vol_Out_AD;
          Data_AD_Sum[3].Sum_Vol_Out_AD=0;

          Data_AD_Sum[1].Sum_Vol_Sense_AD_Copy = Data_AD_Sum[1].Sum_Vol_Sense_AD;
          Data_AD_Sum[1].Sum_Vol_Sense_AD=0;
    }

    DCDC_Service[0].Now_Vol_Sense=Filter_Coeff.Ad_Result[0][4];
    DCDC_Service[1].Now_Vol_Sense=Filter_Coeff.Ad_Result[1][4];
    DCDC_Service[2].Now_Vol_Sense=Filter_Coeff.Ad_Result[2][4];
    DCDC_Service[3].Now_Vol_Sense=Filter_Coeff.Ad_Result[3][4];
    
    //电池电压累加
    Data_Sum[0].Sum_Vol_Sense += DCDC_Service[0].Now_Vol_Sense;
    Data_Sum[1].Sum_Vol_Sense += DCDC_Service[1].Now_Vol_Sense;
    Data_Sum[2].Sum_Vol_Sense += DCDC_Service[2].Now_Vol_Sense;
    Data_Sum[3].Sum_Vol_Sense += DCDC_Service[3].Now_Vol_Sense;
    //输出电流累加
    Data_Sum[0].Sum_Cur_Out += Filter_Coeff.Ad_Result[0][5];
    Data_Sum[1].Sum_Cur_Out += Filter_Coeff.Ad_Result[1][5];
    Data_Sum[2].Sum_Cur_Out += Filter_Coeff.Ad_Result[2][5];
    Data_Sum[3].Sum_Cur_Out += Filter_Coeff.Ad_Result[3][5];
    //输出电流累加
    Data_Sum[0].Sum_Vol_Out += Filter_Coeff.Ad_Result[0][2];
    Data_Sum[1].Sum_Vol_Out += Filter_Coeff.Ad_Result[1][2];
    Data_Sum[2].Sum_Vol_Out += Filter_Coeff.Ad_Result[2][2];
    Data_Sum[3].Sum_Vol_Out += Filter_Coeff.Ad_Result[3][2];
    //BUS电压累加
    Data_Sum[0].Sum_Vol_Bus += Filter_Coeff.Ad_Result[0][3];
    Data_Sum[1].Sum_Vol_Bus += Filter_Coeff.Ad_Result[1][3];
    Data_Sum[2].Sum_Vol_Bus += Filter_Coeff.Ad_Result[2][3];
    Data_Sum[3].Sum_Vol_Bus += Filter_Coeff.Ad_Result[3][3];
    //累加值平均值计算
   if(++Data_Sum[0].Cnt_Sum>=Data_Sum[0].Num_Avg)
   {
       Data_Sum[0].Flag_Refresh       = TRUE;
       Data_Sum[0].Cnt_Sum=0;
       Data_Sum[0].Sum_Vol_Sense_Copy= Data_Sum[0].Sum_Vol_Sense;
       Data_Sum[0].Sum_Vol_Sense=0;
       Data_Sum[1].Sum_Vol_Sense_Copy= Data_Sum[1].Sum_Vol_Sense;
       Data_Sum[1].Sum_Vol_Sense=0;
       Data_Sum[2].Sum_Vol_Sense_Copy= Data_Sum[2].Sum_Vol_Sense;
       Data_Sum[2].Sum_Vol_Sense=0;
       Data_Sum[3].Sum_Vol_Sense_Copy= Data_Sum[3].Sum_Vol_Sense;
       Data_Sum[3].Sum_Vol_Sense=0;
       
       Data_Sum[0].Sum_Cur_Out_Copy= Data_Sum[0].Sum_Cur_Out;
       Data_Sum[0].Sum_Cur_Out=0;
       Data_Sum[1].Sum_Cur_Out_Copy= Data_Sum[1].Sum_Cur_Out;
       Data_Sum[1].Sum_Cur_Out=0;
       Data_Sum[2].Sum_Cur_Out_Copy= Data_Sum[2].Sum_Cur_Out;
       Data_Sum[2].Sum_Cur_Out=0;
       Data_Sum[3].Sum_Cur_Out_Copy= Data_Sum[3].Sum_Cur_Out;
       Data_Sum[3].Sum_Cur_Out=0;

       Data_Sum[0].Sum_Vol_Out_Copy= Data_Sum[0].Sum_Vol_Out;
       Data_Sum[0].Sum_Vol_Out=0;
       Data_Sum[1].Sum_Vol_Out_Copy= Data_Sum[1].Sum_Vol_Out;
       Data_Sum[1].Sum_Vol_Out=0;
       Data_Sum[2].Sum_Vol_Out_Copy= Data_Sum[2].Sum_Vol_Out;
       Data_Sum[2].Sum_Vol_Out=0;
       Data_Sum[3].Sum_Vol_Out_Copy= Data_Sum[3].Sum_Vol_Out;
       Data_Sum[3].Sum_Vol_Out=0;

       Data_Sum[0].Sum_Vol_Bus_Copy= Data_Sum[0].Sum_Vol_Bus;
       Data_Sum[0].Sum_Vol_Bus=0;
       Data_Sum[1].Sum_Vol_Bus_Copy= Data_Sum[1].Sum_Vol_Bus;
       Data_Sum[1].Sum_Vol_Bus=0;
       Data_Sum[2].Sum_Vol_Bus_Copy= Data_Sum[2].Sum_Vol_Bus;
       Data_Sum[2].Sum_Vol_Bus=0;
       Data_Sum[3].Sum_Vol_Bus_Copy= Data_Sum[3].Sum_Vol_Bus;
       Data_Sum[3].Sum_Vol_Bus=0;

   }

   for(i=0;i<4;i++)
    {
          //AB通道电感电流保护
           Protect_Global[i].CH_Over_Cur[0].Now_Value=fclVars[i].CH_A_PI_Cur.Fbk;
           Protect_Global[i].CH_Over_Cur[1].Now_Value=fclVars[i].CH_B_PI_Cur.Fbk;
           //输出充电放电取绝对值保护
           if(Filter_Coeff.Ad_Result[i][5]<0)
           {
                Protect_Global[i].CH_Over_Cur[2].Now_Value=-Filter_Coeff.Ad_Result[i][5];
           }
           else
           {
                Protect_Global[i].CH_Over_Cur[2].Now_Value=Filter_Coeff.Ad_Result[i][5];
           }

           //输出过压、电池过压、电池反接保护
           Protect_Global[i].CH_Over_Vol.Now_Value=Filter_Coeff.Ad_Result[i][2];
           Protect_Global[i].CH_Over_Vol_Bat.Now_Value = Filter_Coeff.Ad_Result[i][4];
           Protect_Global[i].Bat_Reverse_Out.Now_Value= Filter_Coeff.Ad_Result[i][2] ;


           Protect_Value(&Protect_Global[i].CH_Over_Cur[0]);
           Protect_Value(&Protect_Global[i].CH_Over_Cur[1]);
           Protect_Value(&Protect_Global[i].CH_Over_Cur[2]);
           Protect_Value(&Protect_Global[i].CH_Over_Vol);
           Protect_Value(&Protect_Global[i].CH_Over_Vol_Bat);
           Protect_Value(&Protect_Global[i].Bat_Reverse_Out);

           //故障后关PWM以及MOS
           if(Fault_Code[i].Err_Global!=0)
           {
               if(CH_Public_Variable.Parall_Total==1)
               {
                 if(i==0)
                 {
                     PWMOFF_Config(1);
                     PWMOFF_Config(2);
                     Rev_Port1_OFF;
                     DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                 }
                 if(i==1)
                 {
                     PWMOFF_Config(3);
                     PWMOFF_Config(4);
                     Rev_Port2_OFF;
                     DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                 }
                 if(i==2)
                 {
                     PWMOFF_Config(5);
                     PWMOFF_Config(6);
                     Rev_Port3_OFF;
                     DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                 }
                 if(i==3)
                 {
                     PWMOFF_Config(7);
                     PWMOFF_Config(8);
                     Rev_Port4_OFF;
                     DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                 }
               }
               else if(CH_Public_Variable.Parall_Total==2)
               {
                   if((i==0)||(i==1))
                   {
                       PWMOFF_Config(1);
                       PWMOFF_Config(2);
                       Rev_Port1_OFF;
                       PWMOFF_Config(3);
                       PWMOFF_Config(4);
                       Rev_Port2_OFF;
                       DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                   }
                   if((i==2)||(i==3))
                   {
                       PWMOFF_Config(5);
                       PWMOFF_Config(6);
                       Rev_Port3_OFF;
                       PWMOFF_Config(7);
                       PWMOFF_Config(8);
                       Rev_Port4_OFF;
                       DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                   }

               }
           }
          //Bus过压检测
          if((Fault_Code[i].Err_Global&cBusVoltHighFault)==0)
          {
               if(DCDC_Service[i].Now_Vol_Bus>VOL_BUS_H)
               {
                    Fault_Code[i].Err_Global|=cBusVoltHighFault;
                    DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;

               }
          }

        /*  if((Fault_Code[i].Err_Global&cBusVoltLowFault)==0)
          {
              if((DCDC_Service[i].Now_Vol_Bus<VOL_BUS_L)&&(DCDC_Service[i].Now_Vol_Bus>65))
              {
                    Fault_Code[i].Err_Global|=cBusVoltLowFault;
                    DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;

              }

          }*/
          //硬件故障锁存后标志位处理

          if(i==0)
          {
               if((EPwm1Regs.TZFLG.bit.DCAEVT1!=0)||(EPwm2Regs.TZFLG.bit.DCAEVT1!=0))
               {
                    Fault_Code[0].Err_Global|=cHardwareError;
                    DCDC_Service[0].Now_Mode=DCDC_PWM_OFF;
               }
          }
          if(i==1)
          {
               if((EPwm3Regs.TZFLG.bit.DCAEVT1!=0)||(EPwm4Regs.TZFLG.bit.DCAEVT1!=0))
               {
                   Fault_Code[1].Err_Global|=cHardwareError;
                   DCDC_Service[1].Now_Mode=DCDC_PWM_OFF;
               }
          }
          if(i==2)
          {
               if((EPwm5Regs.TZFLG.bit.DCAEVT1!=0)||(EPwm6Regs.TZFLG.bit.DCAEVT1!=0))
               {
                   Fault_Code[2].Err_Global|=cHardwareError;
                   DCDC_Service[2].Now_Mode=DCDC_PWM_OFF;
               }
          }
          if(i==3)
          {
               if((EPwm7Regs.TZFLG.bit.DCAEVT1!=0)||(EPwm8Regs.TZFLG.bit.DCAEVT1!=0))
               {
                   Fault_Code[3].Err_Global|=cHardwareError;
                   DCDC_Service[3].Now_Mode=DCDC_PWM_OFF;
               }
          }
    }


    //控制状态机工作
    DCDC_Service_BSP();
    Cnt_Tick++;
       //后台任务时间片分配
    if(Cnt_Tick==20)
    {
        Cnt_Tick=0;

    }
    //记录故障发生前的数据
  //  if(Fault_Code[0].Err_Global==0)
    if(DCDC_Service[0].Now_Mode==11)
    {

       sTXRecord_JN910.ui32MsgID=0x0000000A;
       sTXRecord_JN910.Data[0]=((int)(CH_PI_Iout[0].Out*100))&0x00FF;
       sTXRecord_JN910.Data[1]=(((int)(CH_PI_Iout[0].Out*100))&0xFF00)>>8;
       sTXRecord_JN910.Data[2]=((int)(CH_PI_Vsense[0].Out*100))&0x00FF;
       sTXRecord_JN910.Data[3]=(((int)(CH_PI_Vsense[0].Out*100))&0xFF00)>>8;
       sTXRecord_JN910.Data[4]=((int)(CH_PI_Vsense[0].Umax*100))&0x00FF;
       sTXRecord_JN910.Data[5]=(((int)(CH_PI_Vsense[0].Umax*100))&0xFF00)>>8;
       sTXRecord_JN910.Data[6]=((int)(DCDC_Service[0].Set_Iout*100))&0x00FF;//EPwm1Regs.CMPA.bit.CMPA&0x00FF;
       sTXRecord_JN910.Data[7]=(((int)(DCDC_Service[0].Set_Iout*100))&0xFF00)>>8;//(EPwm1Regs.CMPA.bit.CMPA&0xFF00)>>8;

       if(BSP_Record_Que_In(&que5Con,&sTXRecord_JN910) == DEF_CAN_QUE_FULL)
       {

       }
    }

  //  test[Cnt_Tick]= ADCA_Sample.ADC0;
 //   testm[Cnt_Tick]=ADCA_Sample.ADC1;
 // testm[Cnt_Tick]= fclVars[2].CH_B_PI_Cur.Fbk;
    //触发AD7606转换脉冲

    //FLASH读写操作状态机
 // SPI_Flash_Service();


    ClrADConst();
    //中断运行时间计算
    gCputime0_time2=CpuTimer0.RegsAddr->TIM.all;
    if(gCputime0_time1>gCputime0_time2)
         gCputime0_time3=gCputime0_time1-gCputime0_time2;
    else
         gCputime0_time3=gCputime0_time1+CpuTimer0.RegsAddr->PRD.all-gCputime0_time2;

    //软件触发内部ADCB、ADCC、ADCD转换
    AdcbRegs.ADCSOCFRC1.all = 0x000F; //SOC0 and SOC3
    AdccRegs.ADCSOCFRC1.all = 0x0014; //SOC2 and SOC4
    AdcdRegs.ADCSOCFRC1.all = 0x000D; //SOC0 and SOC2
    SetADConst();

    EPwm1Regs.ETCLR.all      |=0x01;
    PieCtrlRegs.PIEACK.all    =PIEACK_GROUP3;

}

void InitDCDC_BSP(void)
{
    Uint16 i;
    //高精度分辨率的常量参数
    MEP_Scale = MEP_ScaleFactor;

    //DCDC接口初始化
    for(i=0;i<4;i++)
    {
        Interface_DCDC[i].Req_ILSet1 = 0;
        DCDC_Service[i].Now_Mode=DCDC_STANDBY;
        Interface_DCDC[i].Req_ChangeMode=FALSE;

        //控制参数初始化

          Protect_Para_Var[i][0]=8.4;
          Protect_Para_Var[i][1]=30;

          Control_Para_Var[i][0]=0;
          Control_Para_Var[i][1]=0;

          Debug_Para_Var[i][0]=0.1;
          Debug_Para_Var[i][1]=0.01;   //电流内环Kp、Ki

          Debug_Para_Var[i][2] =0.1;
          Debug_Para_Var[i][3] =0.01; //电流外环Kp、Ki

          Debug_Para_Var[i][4]=1;
          Debug_Para_Var[i][5]=0.1;

          Protect_Global[i].CH_Over_Vol_Bat.Ref_Value=Protect_Para_Var[i][0];
          Protect_Global[i].CH_Over_Cur[2].Ref_Value=Protect_Para_Var[i][1];
          DCDC_Service[i].Set_Vout= Control_Para_Var[i][0];
          DCDC_Service[i].Set_Iout=Control_Para_Var[i][1];

         fclVars[i].CH_A_PI_Cur.Kp= Debug_Para_Var[i][0];
         fclVars[i].CH_B_PI_Cur.Kp= Debug_Para_Var[i][0];
         fclVars[i].CH_A_PI_Cur.Ki=Debug_Para_Var[i][1];
         fclVars[i].CH_B_PI_Cur.Ki=Debug_Para_Var[i][1];

         CH_PI_Iout[i].Kp=Debug_Para_Var[i][2];
         CH_PI_Iout[i].Ki=Debug_Para_Var[i][3];

         CH_PI_Vout[i].Kp=Debug_Para_Var[i][4];
         CH_PI_Vout[i].Ki=Debug_Para_Var[i][5];
         CH_PI_Vsense[i].Kp=Debug_Para_Var[i][4];
         CH_PI_Vsense[i].Ki=Debug_Para_Var[i][5];

         Run_Mode_Var[i][0]=0;

         DCDC_Service[i].Fault_Module=0;
         DCDC_Service[i].CV_Period=20;
         DCDC_Service[i].CV_Cnt=0;
    }




   Data_AD_Sum[0].Sum_Vol_Sense_AD=0;
   Data_AD_Sum[0].Num_Avg            = 2000;
   Data_AD_Sum[0].Cnt_Sum            = 0;
   Data_AD7124_Sum.Num_Avg           =20;
   Data_AD7124_Sum.Cnt_Sum           =0;
   Data_AD7124_Sum.Sum_J1Vol_AD_Copy =0;
   Data_AD7124_Sum.Sum_J1Vol_AD=0;

   Data_Sum[0].Cnt_Sum=0;
   Data_Sum[0].Num_Avg =500;
   Data_Sum[0].Sum_Vol_JR=0;
   Data_Sum[1].Sum_Cur_Out=0;
   Data_Sum[1].Sum_Vol_Out=0;

   Data_Sum[0].Flag_Refresh       = FALSE;

   for(i=0;i<4;i++)
   {
       fclVars[i].CH_A_PI_Cur.Fbk=0;
       fclVars[i].CH_A_PI_Cur.Out=0;
       fclVars[i].CH_A_PI_Cur.Ref=0;
       fclVars[i].CH_A_PI_Cur.i1=0;
       fclVars[i].CH_A_PI_Cur.ui=0;
       fclVars[i].CH_A_PI_Cur.up=0;
       fclVars[i].CH_A_PI_Cur.v1=0;

       fclVars[i].CH_A_PI_Cur.Umax = 0.95;
       fclVars[i].CH_A_PI_Cur.Umin =0;

       fclVars[i].CH_B_PI_Cur.Fbk=0;
       fclVars[i].CH_B_PI_Cur.Out=0;
       fclVars[i].CH_B_PI_Cur.Ref=0;
       fclVars[i].CH_B_PI_Cur.i1=0;
       fclVars[i].CH_B_PI_Cur.ui=0;
       fclVars[i].CH_B_PI_Cur.up=0;
       fclVars[i].CH_B_PI_Cur.v1=0;

       fclVars[i].CH_B_PI_Cur.Umax = 0.95;
       fclVars[i].CH_B_PI_Cur.Umin =0;

       fclVars[i].lsw=ENC_ALIGNMENT;

       CH_PI_Iout[i].Fbk=0;
       CH_PI_Iout[i].Out=0;
       CH_PI_Iout[i].Ref=0;
       CH_PI_Iout[i].i1=0;
       CH_PI_Iout[i].ui=0;
       CH_PI_Iout[i].up=0;
       CH_PI_Iout[i].v1=0;

       CH_PI_Iout[i].Umax =2;
       CH_PI_Iout[i].Umin =-2;

       CH_PI_Vout[i].Fbk=0;
       CH_PI_Vout[i].Out=0;
       CH_PI_Vout[i].Ref=0;
       CH_PI_Vout[i].i1=0;
       CH_PI_Vout[i].ui=0;
       CH_PI_Vout[i].up=0;
       CH_PI_Vout[i].v1=0;

       CH_PI_Vout[i].Umax =60;
       CH_PI_Vout[i].Umin =-60;

       CH_PI_Vsense[i].Fbk=0;
       CH_PI_Vsense[i].Out=0;
       CH_PI_Vsense[i].Ref=0;
       CH_PI_Vsense[i].i1=0;
       CH_PI_Vsense[i].ui=0;
       CH_PI_Vsense[i].up=0;
       CH_PI_Vsense[i].v1=0;

       CH_PI_Vsense[i].Umax =60;
       CH_PI_Vsense[i].Umin =-60;

   }


}




void PI_Cal(PI_CLA *v)
{
    /* proportional term */
    v->up = v->Ref - v->Fbk;

    /* integral term */
    v->ui = (v->Out == v->v1)?((v->Ki* v->up)+ v->i1) : v->i1;

    v->i1 = v->ui;

    /* control output */
    v->v1 = (v->Kp* v->up + v->ui)/84;//DCDC_Service[1].Now_Vol_Bus;

    v->Out=(((v->v1 > v->Umax) ? v->Umax : v->v1)>v->Umin)?((v->v1 > v->Umax) ? v->Umax : v->v1):v->Umin;

}

void PI_Cal_Cur(PI_CLA *v)
{
    /* proportional term */
    v->up = v->Ref - v->Fbk;

    /* integral term */
    v->ui = (v->Out == v->v1)?((v->Ki* v->up)+ v->i1) : v->i1;

    v->i1 = v->ui;

    /* control output */
    v->v1 = (v->Kp* v->up + v->ui);//DCDC_Service[1].Now_Vol_Bus;

    v->Out=(((v->v1 > v->Umax) ? v->Umax : v->v1)>v->Umin)?((v->v1 > v->Umax) ? v->Umax : v->v1):v->Umin;

}

void PI_Cal_Vol(PI_CLA *v)
{
    /* proportional term */
    v->up = v->Ref - v->Fbk;

    /* integral term */
    v->ui = (v->Out == v->v1)?((v->Ki* v->up)+ v->i1) : v->i1;

    v->i1 = v->ui;

    /* control output */
    v->v1 = (v->Kp* v->up + v->ui);

    v->Out=(((v->v1 > v->Umax) ? v->Umax : v->v1)>v->Umin)?((v->v1 > v->Umax) ? v->Umax : v->v1):v->Umin;

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

                    if((Interface_DCDC[i].Req_Mode<DCDC_CC)||(Interface_DCDC[i].Req_Mode>DCDC_DEBUG))
                    {
                        Interface_DCDC[i].Req_ChangeMode=FALSE;
                        return;
                    }

                    Interface_DCDC[i].Req_ChangeMode = FALSE;


                   if(Data_Sum[i].Sum_Vol_Bus>Data_Sum[i].Vol_Out)
                    {
                       //根据输出电压端口检测电压进入预充状态
                         DCDC_Service[i].PWM_Ready=Data_Sum[i].Vol_Out*PWM_PERIOD/Data_Sum[i].Vol_Bus;
                         PWM_Ready[i]=DCDC_Service[i].PWM_Ready;
                         if(DCDC_Service[i].PWM_Ready<30)
                         {
                                DCDC_Service[i].PWM_Ready  = 0;
                                DCDC_Service[i].Now_Mode  = DCDC_READY3;
                                waitimecnt1=0;
                                CH_A[i].PWMxA =  PWM_DEADFEDTIME;
                                CH_B[i].PWMxA =  PWM_DEADFEDTIME;

                                CH_A[i].PWMxB = 920; //PWM_PERIOD-PWM_DEADFEDTIME;
                                CH_B[i].PWMxB = 920; //PWM_PERIOD-PWM_DEADFEDTIME;

                                if(i==0)
                                {
                                    PWMON_Config(1);
                                    PWMON_Config(2);

                                }
                                if(i==1)
                                {
                                    PWMON_Config(3);
                                    PWMON_Config(4);
                                }
                                if(i==2)
                                {
                                    PWMON_Config(5);
                                    PWMON_Config(6);
                                }
                                if(i==3)
                                {
                                    PWMON_Config(7);
                                    PWMON_Config(8);
                                }
                         }
                         else
                         {

                              if( DCDC_Service[i].PWM_Ready>PWM_MAX_CHARGE)
                              {
                                  DCDC_Service[i].PWM_Ready=PWM_MAX_CHARGE;
                              }

                              CH_A[i].PWMxA =  PWM_DEADFEDTIME;
                              CH_B[i].PWMxA =  PWM_DEADFEDTIME;

                              CH_A[i].PWMxB =  PWM_PERIOD-PWM_DEADFEDTIME;
                              CH_B[i].PWMxB =  PWM_PERIOD-PWM_DEADFEDTIME;

                              if(i==0)
                              {
                                  PWMON_Config(1);
                                  PWMON_Config(2);

                              }
                              if(i==1)
                              {
                                  PWMON_Config(3);
                                  PWMON_Config(4);
                              }
                              if(i==2)
                              {
                                  PWMON_Config(5);
                                  PWMON_Config(6);
                              }
                              if(i==3)
                              {
                                  PWMON_Config(7);
                                  PWMON_Config(8);
                              }
                              DCDC_Service[i].Now_Mode         = DCDC_READY1;
                         }
                    }
                    else
                    {
                            //  DCDC_Service.Fault_Module    |= FAULT_BUS_L;
                            DCDC_Service[i].Now_Mode         = DCDC_PWM_OFF;
                            if(i==0)
                            {
                                PWMOFF_Config(1);
                                PWMOFF_Config(2);

                            }
                            if(i==1)
                            {
                                PWMOFF_Config(3);
                                PWMOFF_Config(4);
                            }
                            if(i==2)
                            {
                                PWMOFF_Config(5);
                                PWMOFF_Config(6);
                            }
                            if(i==3)
                            {
                                PWMOFF_Config(7);
                                PWMOFF_Config(8);
                            }
                    }
                }
                break;
            }
            case DCDC_READY1:
            {
                //开启同步整流，将输出电容电压推高到和输出端相近的电压
                if(CH_A[i].PWMxA<DCDC_Service[i].PWM_Ready)
                {
                    CH_A[i].PWMxA=CH_A[i].PWMxA+1;
                    CH_B[i].PWMxA=CH_A[i].PWMxA;
                }

                if(CH_A[i].PWMxB>DCDC_Service[i].PWM_Ready)
                {
                    CH_A[i].PWMxB=CH_A[i].PWMxB-1;
                    CH_B[i].PWMxB=CH_A[i].PWMxB;
                }
                if(i==0)
                {
                   EPwm1Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                   EPwm1Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                   EPwm2Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                   EPwm2Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                }
                if(i==1)
                {
                   EPwm3Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                   EPwm3Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                   EPwm4Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                   EPwm4Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                }
                if(i==2)
                {
                    EPwm5Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                    EPwm5Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                    EPwm6Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                    EPwm6Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                }
                if(i==3)
               {
                   EPwm7Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                   EPwm7Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                   EPwm8Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                   EPwm8Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
               }

                if((CH_A[i].PWMxA<DCDC_Service[i].PWM_Ready)||(CH_A[i].PWMxB>DCDC_Service[i].PWM_Ready))
                {
                    //DELAY_US(5);
                    // return;
                    DCDC_Service[i].Now_Mode         = DCDC_READY1;
                }
                else
                {
                    DCDC_Service[i].Now_Mode         = DCDC_Waitime;
                    waitimecnt=0;
                }
                break;
            }
            case DCDC_Waitime:
            {
               //等待时间超过10.5ms
                waitimecnt++;
                if(waitimecnt>210)
                {
                      waitimecnt=0;
                      DCDC_Service[i].Now_Mode         = DCDC_Waitime2;
                }

                 break;

            }
            case DCDC_Waitime2:
            {
                 //当电池电压与预充电压在0.5V范围内跳出预充，否则在预充范围内预充
                if(DCDC_Service[i].Now_Vol_JR>(Data_Sum[i].Vol_Out+0.5))
                {
                      CH_A[i].PWMxA=CH_A[i].PWMxA-1;
                      CH_B[i].PWMxA=CH_A[i].PWMxA;
                      CH_A[i].PWMxB=CH_A[i].PWMxA;
                      CH_B[i].PWMxB=CH_A[i].PWMxA;
                      if(i==0)
                     {
                        EPwm1Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                        EPwm1Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                        EPwm2Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                        EPwm2Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                     }
                     if(i==1)
                     {
                        EPwm3Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                        EPwm3Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                        EPwm4Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                        EPwm4Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                     }
                     if(i==2)
                     {
                         EPwm5Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                         EPwm5Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                         EPwm6Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                         EPwm6Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                     }
                     if(i==3)
                    {
                        EPwm7Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                        EPwm7Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                        EPwm8Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                        EPwm8Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                    }
                    DCDC_Service[i].Now_Mode         = DCDC_Waitime;
                    if(CH_A[i].PWMxA<20)
                    {
                        DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                        DCDC_Service[i].Fault_Module|=FAULT_PreCharg;
                    }
               }
                else if(DCDC_Service[i].Now_Vol_JR<(Data_Sum[i].Vol_Out-0.5))
                {

                     CH_A[i].PWMxA=CH_A[i].PWMxA+1;
                     CH_B[i].PWMxA=CH_A[i].PWMxA;
                     CH_A[i].PWMxB=CH_A[i].PWMxA;
                     CH_B[i].PWMxB=CH_A[i].PWMxA;
                     if(i==0)
                    {
                       EPwm1Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                       EPwm1Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                       EPwm2Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                       EPwm2Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                    }
                    if(i==1)
                    {
                       EPwm3Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                       EPwm3Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                       EPwm4Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                       EPwm4Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                    }
                    if(i==2)
                    {
                        EPwm5Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                        EPwm5Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                        EPwm6Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                        EPwm6Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                    }
                    if(i==3)
                   {
                       EPwm7Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                       EPwm7Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                       EPwm8Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                       EPwm8Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                   }
                    DCDC_Service[i].Now_Mode         = DCDC_Waitime;
                    if(CH_A[i].PWMxA>(DCDC_Service[i].PWM_Ready+200))
                    {
                        DCDC_Service[i].Fault_Module|=FAULT_PreCharg;
                        DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                    }

                }
                else
                {
                       if(i==0)
                       {
                           PWMOFF_Config(1);
                           PWMOFF_Config(2);
                       }
                       if(i==1)
                       {
                           PWMOFF_Config(3);
                           PWMOFF_Config(4);
                       }
                       if(i==2)
                       {
                           PWMOFF_Config(5);
                           PWMOFF_Config(6);
                       }
                       if(i==3)
                       {
                           PWMOFF_Config(7);
                           PWMOFF_Config(8);
                       }

                       DCDC_Service[i].Now_Mode         = DCDC_READY2;
                       DCDC_Service[i].Cnt_Delay=0;
                }
                if(Interface_DCDC[i].Req_ChangeMode==TRUE)
                 {
                       DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                 }
                break;
            }
            case DCDC_READY3:
            {
                    //该状态是不带电池，带电子负载时输出电容有电压需要放掉
                   if(DCDC_Service[i].Now_Vol_JR>2)
                   {
                        if(++waitimecnt1>19)
                        {
                           CH_A[i].PWMxB=CH_A[i].PWMxB-1;
                           if(CH_A[i].PWMxB<100)
                           {
                                CH_A[i].PWMxB=100;
                           }
                           CH_B[i].PWMxB=CH_A[i].PWMxB;
                           waitimecnt1=0;
                        }

                        if(i==0)
                        {
                          EPwm1Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                          EPwm1Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                          EPwm2Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                          EPwm2Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                        }
                        if(i==1)
                        {
                          EPwm3Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                          EPwm3Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                          EPwm4Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                          EPwm4Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                        }
                        if(i==2)
                        {
                           EPwm5Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                           EPwm5Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                           EPwm6Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                           EPwm6Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                        }
                        if(i==3)
                       {
                          EPwm7Regs.CMPA.bit.CMPA       = CH_A[i].PWMxA;
                          EPwm7Regs.CMPB.bit.CMPB       = CH_A[i].PWMxB;
                          EPwm8Regs.CMPA.bit.CMPA       = CH_B[i].PWMxA;
                          EPwm8Regs.CMPB.bit.CMPB       = CH_B[i].PWMxB;
                       }

                   }
                   else
                   {
                     if(i==0)
                     {
                         PWMOFF_Config(1);
                         PWMOFF_Config(2);
                     }
                     if(i==1)
                     {
                         PWMOFF_Config(3);
                         PWMOFF_Config(4);
                     }
                     if(i==2)
                     {
                         PWMOFF_Config(5);
                         PWMOFF_Config(6);
                     }
                     if(i==3)
                     {
                         PWMOFF_Config(7);
                         PWMOFF_Config(8);
                     }
                       DCDC_Service[i].Now_Mode         = DCDC_READY2;
                   }

                   if(Interface_DCDC[i].Req_ChangeMode==TRUE)
                   {
                         DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                   }
                   break;
             }
            case DCDC_READY2:
            {

                if(i==0)
                {

                   CH_A[0].PWMxA=PWM_DEADREDTIME;
                   CH_A[0].PWMxB=PWM_PERIOD-PWM_DEADFEDTIME;
                   CH_B[0].PWMxA=PWM_DEADREDTIME;
                   CH_B[0].PWMxB=PWM_PERIOD-PWM_DEADFEDTIME;
                   EPwm1Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
                   EPwm1Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
                   EPwm2Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
                   EPwm2Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
                   Rev_Port1_ON;
                }
                if(i==1)
                {
                    CH_A[1].PWMxA=PWM_DEADREDTIME;
                    CH_A[1].PWMxB=PWM_PERIOD-PWM_DEADFEDTIME;
                    CH_B[1].PWMxA=PWM_DEADREDTIME;
                    CH_B[1].PWMxB=PWM_PERIOD-PWM_DEADFEDTIME;
                    EPwm3Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
                    EPwm3Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
                    EPwm4Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
                    EPwm4Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
                    Rev_Port2_ON;
                 }
                if(i==2)
                {
                    CH_A[2].PWMxA=PWM_DEADREDTIME;
                    CH_A[2].PWMxB=PWM_PERIOD-PWM_DEADFEDTIME;
                    CH_B[2].PWMxA=PWM_DEADREDTIME;
                    CH_B[2].PWMxB=PWM_PERIOD-PWM_DEADFEDTIME;
                    EPwm5Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
                    EPwm5Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
                    EPwm6Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
                    EPwm6Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
                    Rev_Port3_ON;
                }
                if(i==3)
                {
                    CH_A[3].PWMxA=PWM_DEADREDTIME;
                    CH_A[3].PWMxB=PWM_PERIOD-PWM_DEADFEDTIME;
                    CH_B[3].PWMxA=PWM_DEADREDTIME;
                    CH_B[3].PWMxB=PWM_PERIOD-PWM_DEADFEDTIME;
                    EPwm7Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
                    EPwm7Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
                    EPwm8Regs.CMPA.bit.CMPA       = PWM_DEADREDTIME;
                    EPwm8Regs.CMPB.bit.CMPB       = PWM_PERIOD-PWM_DEADFEDTIME;//PWM_DEADTIME
                    Rev_Port4_ON;
                }

              //  GPIO_WritePin(44+i,1);

                DCDC_Service[i].Now_Mode         = DCDC_Waitime1;

                break;
            }
            case DCDC_Waitime1:
            {
                //转换到工作模式前等待0.04秒，等待输出电容冲击电流下降
                if(CH_Public_Variable.Parall_Total==2)
                {
                    if((DCDC_Service[0].Now_Mode==DCDC_Waitime1)&&(DCDC_Service[1].Now_Mode==DCDC_Waitime1))
                    {
                          if(++DCDC_Service[0].Cnt_Delay>800)
                          {
                                    DCDC_Service[0].Now_Mode  = DCDC_READY;
                                    DCDC_Service[1].Now_Mode  = DCDC_READY;
                          }
                    }
                    if((DCDC_Service[2].Now_Mode==DCDC_Waitime1)&&(DCDC_Service[3].Now_Mode==DCDC_Waitime1))
                    {
                         if(++DCDC_Service[2].Cnt_Delay>800)
                          {
                                   DCDC_Service[2].Now_Mode  = DCDC_READY;
                                   DCDC_Service[3].Now_Mode  = DCDC_READY;
                          }

                    }
                }
                else if(CH_Public_Variable.Parall_Total==1)
                {

                    if(++DCDC_Service[i].Cnt_Delay>800)
                    {
                               DCDC_Service[i].Now_Mode         = DCDC_READY;
                    }

                }
                if(Interface_DCDC[i].Req_ChangeMode==TRUE)
                {
                    DCDC_Service[i].Now_Mode=DCDC_PWM_OFF;
                }

                break;

            }
            case DCDC_READY:
            {

                DCDC_Service[i].PWM_Ready=0;
                DCDC_Service[i].Cnt_Delay=0;
                CH_A[i].Req_FastStart=FALSE;
                CH_B[i].Req_FastStart=FALSE;

                CH_A[i].Req_FastStart_DCM=FALSE;
                CH_B[i].Req_FastStart_DCM=FALSE;
                CH_A[i].Cnt_SynRect=0;
                CH_B[i].Cnt_SynRect=0;
                CH_Public_Variable.Flag_Record=0;

                CH_A[i].Cnt_FastStart=0;
                CH_B[i].Cnt_FastStart=0;
                if((Interface_DCDC[i].Req_Mode==DCDC_CC)||(Interface_DCDC[i].Req_Mode==DCDC_DCC)||(Interface_DCDC[i].Req_Mode==DCDC_CV)||(Interface_DCDC[i].Req_Mode==DCDC_DCV))
                {
                     DCDC_Service[i].Now_Mode=DCDC_RUN;
                    // DCDC_Service[i].Now_Mode=DCDC_STANDBY;

                     fclVars[i].CH_A_PI_Cur.i1=2.52;
                     fclVars[i].CH_A_PI_Cur.ui=0;
                     fclVars[i].CH_A_PI_Cur.up=0;
                     fclVars[i].CH_A_PI_Cur.v1=0;
                     fclVars[i].CH_B_PI_Cur.i1=2.52;
                     fclVars[i].CH_B_PI_Cur.ui=0;
                     fclVars[i].CH_B_PI_Cur.up=0;
                     fclVars[i].CH_B_PI_Cur.v1=0;
                     CH_PI_Iout[i].i1=0;
                     CH_PI_Iout[i].ui=0;
                     CH_PI_Iout[i].up=0;
                     CH_PI_Iout[i].v1=0;
                     CH_PI_Vout[i].i1=0;
                     CH_PI_Vout[i].ui=0;
                     CH_PI_Vout[i].up=0;
                     CH_PI_Vout[i].v1=0;

                     CH_PI_Vsense[i].i1=0;
                     CH_PI_Vsense[i].ui=0;
                     CH_PI_Vsense[i].up=0;
                     CH_PI_Vsense[i].v1=0;

                     fclVars[i].Empty_Full_Flag=0;
                     fclVars[i].Neg_Full_Flag=0;
                     fclVars[i].Once_Flag=0;
                     //CHA通道数据初始化
                     fclVars[i].CH_A_PI_Cur.Ref   = 0;
                     fclVars[i].CH_A_PI_Cur.Out   = 0;
                     DCDC_Service[i].Flag_Swtich=FALSE;

                     DCDC_Service[i].Ref_Iout=0;
                     DCDC_Service[i].Ref_Vout=0;
                     DCDC_Service[i].CV_Ref_Iout=0;
                     DCDC_Service[i].Slew_Cur_ON=DCDC_Service[i].Set_Iout/300;

                     //CHB通道数据初始化
                     fclVars[i].CH_B_PI_Cur.Ref   = 0;
                     fclVars[i].CH_B_PI_Cur.Out   = 0;

                     CH_A[i].Req_FastStart=TRUE;
                     CH_B[i].Req_FastStart=TRUE;

                     CH_Public_Variable.Limit_Set_Cur=100;
                     CH_A[i].EN_SynRect=FALSE;
                     CH_B[i].EN_SynRect=FALSE;
                     CH_A[i].Flag_SynRect=FALSE;
                     CH_B[i].Flag_SynRect=FALSE;

                     CH_A[i].Flag_SynRect=FALSE;
                     CH_B[i].Flag_SynRect=FALSE;

                     CH_A[i].Cnt_SynRect=0;
                     CH_B[i].Cnt_SynRect=0;
                     DCDC_Service[i].Flag_Mode=0;
                     DCDC_Service[i].Flag_Once=0;
                     //输出电流
                     CH_PI_Iout[i].Ref   = 0;
                     CH_PI_Iout[i].Out   = 0;
                     if(Interface_DCDC[i].Req_Mode==DCDC_CV)
                     {
                         DCDC_Service[i].Flag_Mode=2;
                         CH_PI_Vout[i].Umax =5;
                         CH_PI_Vout[i].Umin =-0.5;
                         CH_A[i].EN_SynRect=TRUE;
                         CH_B[i].EN_SynRect=TRUE;
                         fclVars[i].Empty_Full_Flag=0;

                     }
                     else if(Interface_DCDC[i].Req_Mode==DCDC_DCV)
                     {
                         CH_PI_Vout[i].Umax =0;
                         CH_PI_Vout[i].Umin =DCDC_Service[i].Set_Iout;

                         CH_PI_Iout[i].Umax =0;
                         CH_PI_Iout[i].Umin =-80;
                     }
                     else if(Interface_DCDC[i].Req_Mode==DCDC_DCC)
                     {
                         DCDC_Service[i].Flag_Mode=3;
                         CH_PI_Iout[i].Umax=40;
                         CH_PI_Iout[i].Umin =-40;
                         fclVars[i].Empty_Full_Flag=1;
                         CH_PI_Vsense[i].Umax =0;
                         CH_PI_Vsense[i].Umin =DCDC_Service[i].Set_Iout;
                         CH_PI_Vsense[i].i1=DCDC_Service[i].Set_Iout;
                         CH_PI_Iout[i].Ref=DCDC_Service[i].Set_Iout;
                     }
                     else if(Interface_DCDC[i].Req_Mode==DCDC_CC)
                     {
                         DCDC_Service[i].Flag_Mode=1;
                         CH_PI_Iout[i].Umax=40;
                         CH_PI_Iout[i].Umin =-40;
                         fclVars[i].Empty_Full_Flag=1;
                         CH_PI_Vsense[i].Umax =DCDC_Service[i].Set_Iout;
                         CH_PI_Vsense[i].Umin =0;
                         CH_PI_Vsense[i].i1=DCDC_Service[i].Set_Iout;
                         CH_PI_Iout[i].Ref=DCDC_Service[i].Set_Iout;

                     }


                     if(i==0)
                     {
                        PWMON_Config(1);
                        PWMON_Config(2);

                     }
                     if(i==1)
                     {
                        PWMON_Config(3);
                        PWMON_Config(4);
                     }
                     if(i==2)
                     {
                        PWMON_Config(5);
                        PWMON_Config(6);
                     }
                     if(i==3)
                     {
                        PWMON_Config(7);
                        PWMON_Config(8);
                     }
               }
               else
               {
                    //异常请求都转到待机状态
                     Interface_DCDC[i].Req_Mode       = DCDC_STANDBY;
                   //状态机切换
                     DCDC_Service[i].Now_Mode         = DCDC_PWM_OFF;
               }
               break;
            }
            case DCDC_RUN:
            {
                 if((Interface_DCDC[i].Req_Mode==DCDC_DCV)||(Interface_DCDC[i].Req_Mode==DCDC_CV))
                 {

                 }
                 else if((Interface_DCDC[i].Req_Mode==DCDC_CC)||(Interface_DCDC[i].Req_Mode==DCDC_DCC))
                 {

                      //    CH_PI_Iout[i].Ref=DCDC_Service[i].Set_Iout;

                 }
                //根据电流大小判断同步整流开启与否
                 if((Filter_Coeff.Ad_Result[i][5]>5)||(Filter_Coeff.Ad_Result[i][5]<-5))
                 {
                       CH_A[i].EN_SynRect=TRUE;
                       CH_B[i].EN_SynRect=TRUE;
                 }
                //外环电流闭环参与控制条件判断
                if(  DCDC_Service[i].Flag_Swtich==TRUE)
                {
                     CH_PI_Iout[i].Fbk=Filter_Coeff.Ad_Result[i][5];
                     PI_Cal_Cur(&CH_PI_Iout[i]);
                }


                // 该模式恒流充电及恒压充电自动切换，当充电电流没有达到设定的额定电压会自动进入恒流，当充电电流达到额定的电压会自动进入恒压状态
                if(Interface_DCDC[i].Req_Mode==DCDC_CC)
                {
                      //控制精度电流环是50us计算一次，电压环是1ms控制一次
                       if(++DCDC_Service[i].CV_Cnt>=DCDC_Service[i].CV_Period)
                       {

                               if(CH_PI_Vsense[i].Umax<=DCDC_Service[i].Set_Iout)
                               {
                                   //上位机在运行中重新下发设定电流，电压环根据下发的额定电流实时调整电压环的最大输出
                                   if(Interface_DCDC[i].Req_Refresh_Cur==TRUE)
                                   {
                                       CH_PI_Vsense[i].Umax=DCDC_Service[i].Set_Iout;
                                       Interface_DCDC[i].Req_Refresh_Cur=FALSE;
                                   }

                               }
                               else
                               {
                                   //上位机在运行中重新下发设定电流，电压环根据下发的额定电流实时调整电压环的最大输出
                                      if(Interface_DCDC[i].Req_Refresh_Cur==TRUE)
                                      {
                                          CH_PI_Vsense[i].Umax=DCDC_Service[i].Set_Iout;
                                          Interface_DCDC[i].Req_Refresh_Cur=FALSE;
                                          CH_PI_Vsense[i].i1=DCDC_Service[i].Set_Iout;
                                          CH_PI_Vsense[i].ui=0;
                                          CH_PI_Vsense[i].up=0;
                                          CH_PI_Vsense[i].v1=0;
                                          CH_PI_Vsense[i].Out=0;
                                      }

                               }
                               DCDC_Service[i].CV_Cnt=0;
                               //电压环PI控制
                               CH_PI_Vsense[i].Ref=DCDC_Service[i].Set_Vout;
                               CH_PI_Vsense[i].Fbk=Filter_Coeff.Ad_Result[i][4];
                               PI_Cal_Vol(&CH_PI_Vsense[i]);
                               //电压环输出作为输出电流精度环的控制参考
                               CH_PI_Iout[i].Ref=CH_PI_Vsense[i].Out;
                       }
                       // 第一次启动时控制策略调整，电流缓启动，当参考电流逐加达到额定电流，输出电流精度环切入
                       if(DCDC_Service[i].Flag_Once!=TRUE)
                       {
                              if(DCDC_Service[i].Ref_Iout<DCDC_Service[i].Set_Iout)
                              {
                                  //当参考电流逐加还未达到额定电流值时，不开启输出电流精度环
                                    DCDC_Service[i].Ref_Iout=DCDC_Service[i].Ref_Iout+DCDC_Service[i].Slew_Cur_ON;
                                    DCDC_Service[i].Flag_Swtich=FALSE;
                                    CH_PI_Iout[i].i1=0;
                                    CH_PI_Iout[i].ui=0;
                                    CH_PI_Iout[i].up=0;
                                    CH_PI_Iout[i].v1=0;
                                    CH_PI_Iout[i].Out=0;

                                    if(DCDC_Service[i].Ref_Iout>=DCDC_Service[i].Set_Iout)
                                    {

                                        //当参考电流达到额定电流时，开启输出电流精度环控制
                                        DCDC_Service[i].Ref_Iout=DCDC_Service[i].Set_Iout;
                                        DCDC_Service[i].Flag_Swtich=TRUE;
                                        DCDC_Service[i].Flag_Once=TRUE;
                                    }

                               }
                               else if(DCDC_Service[i].Ref_Iout>DCDC_Service[i].Set_Iout)
                               {
                                   //当参考电流逐加还未达到额定电流值时，不开启输出电流精度环
                                     DCDC_Service[i].Ref_Iout=DCDC_Service[i].Ref_Iout-DCDC_Service[i].Slew_Cur_ON;
                                     DCDC_Service[i].Flag_Swtich=FALSE;
                                     CH_PI_Iout[i].i1=0;
                                     CH_PI_Iout[i].ui=0;
                                     CH_PI_Iout[i].up=0;
                                     CH_PI_Iout[i].v1=0;
                                     if( DCDC_Service[i].Ref_Iout<=DCDC_Service[i].Set_Iout)
                                     {
                                         //当参考电流达到额定电流时，开启输出电流精度环控制
                                         DCDC_Service[i].Ref_Iout=DCDC_Service[i].Set_Iout;
                                         DCDC_Service[i].Flag_Swtich=TRUE;
                                         DCDC_Service[i].Flag_Once=TRUE;
                                     }

                                }

                       }
                       //参考电流与输出电流精度环输出作为电感电流内环的参考
                       IdRef[i]=(DCDC_Service[i].Ref_Iout+CH_PI_Iout[i].Out)*0.5;

                       //电感电流开启的标志位
                       fclVars[i].lsw=ENC_WAIT_FOR_INDEX;

                }
                else if(Interface_DCDC[i].Req_Mode==DCDC_CV)
                {
                      //输出电压参考缓升
                      if(DCDC_Service[i].Ref_Vout<DCDC_Service[i].Set_Vout)
                      {
                            DCDC_Service[i].Ref_Vout=DCDC_Service[i].Ref_Vout+DCDC_Service[i].Set_Vout/800;

                            if(DCDC_Service[i].Ref_Vout>=DCDC_Service[i].Set_Vout)
                            {
                                DCDC_Service[i].Ref_Vout=DCDC_Service[i].Set_Vout;
                            }
                      }
                      else if(DCDC_Service[i].Ref_Vout>DCDC_Service[i].Set_Vout)
                      {
                             DCDC_Service[i].Ref_Vout=DCDC_Service[i].Ref_Vout-DCDC_Service[i].Set_Vout/800;

                             if( DCDC_Service[i].Ref_Vout<=DCDC_Service[i].Set_Vout)
                             {
                                 DCDC_Service[i].Ref_Vout=DCDC_Service[i].Set_Vout;
                             }

                      }
                   //当参考电压缓升达到额定电压
                   if(DCDC_Service[i].Ref_Vout==DCDC_Service[i].Set_Vout)
                   {
                       //当参考电压达到额定电压在判断输出电流是否大于2A以此作为输出是否带载的一个判断标志
                         if(Filter_Coeff.Ad_Result[i][5]>2)
                         {
                               fclVars[i].Empty_Full_Flag=1;
                               fclVars[i].Once_Flag=1;
                               CH_PI_Vout[i].Umax =DCDC_Service[i].Set_Iout;
                         }
                         else
                         {
                             if(fclVars[i].Once_Flag==1)
                             {
                                 fclVars[i].Once_Flag=2;

                             }
                               fclVars[i].Empty_Full_Flag=0;

                         }
                   }
                   //此条件判断输出电流是否存在电流反向的一个标志
                   if(Filter_Coeff.Ad_Result[i][5]<-2)
                   {
                       fclVars[i].Neg_Full_Flag=1;
                       fclVars[i].Isense_Curr=Filter_Coeff.Ad_Result[i][5];

                   }
                   else
                   {
                       fclVars[i].Neg_Full_Flag=0;
                   }
                    //输出电压环的PI控制
                    CH_PI_Vout[i].Ref=DCDC_Service[i].Ref_Vout;
                    CH_PI_Vout[i].Fbk= Filter_Coeff.Ad_Result[i][2];
                    PI_Cal_Vol(&CH_PI_Vout[i]);
                    //输出电压环的输出作为电感电流的内环参考
                    IdRef[i]= CH_PI_Vout[i].Out*0.5;
                    //电感电流环工作的启动标志位
                    fclVars[i].lsw=ENC_WAIT_FOR_INDEX;
                }
                else if(Interface_DCDC[i].Req_Mode==DCDC_DCC)   // 该模式恒流放电及恒压放电自动切换，当放电电流没有达到设定的额定电压会自动进入恒流，当放电电流达到额定的电压会自动进入恒压状态
                {
                    //控制精度电流环是50us计算一次，电压环是1ms控制一次
                    if(++DCDC_Service[i].CV_Cnt>=DCDC_Service[i].CV_Period)
                    {
                        //上位机在运行中重新下发设定电流，电压环根据下发的额定电流实时调整电压环的最大输出
                              if(CH_PI_Vsense[i].Umin>=DCDC_Service[i].Set_Iout)
                              {
                                  if(Interface_DCDC[i].Req_Refresh_Cur==TRUE)
                                  {
                                      CH_PI_Vsense[i].Umin=DCDC_Service[i].Set_Iout;
                                      Interface_DCDC[i].Req_Refresh_Cur=FALSE;
                                  }

                              }
                              else
                              {
                                  //上位机在运行中重新下发设定电流，电压环根据下发的额定电流实时调整电压环的最大输出
                                     if(Interface_DCDC[i].Req_Refresh_Cur==TRUE)
                                     {
                                         CH_PI_Vsense[i].Umin=DCDC_Service[i].Set_Iout;
                                         Interface_DCDC[i].Req_Refresh_Cur=FALSE;
                                         CH_PI_Vsense[i].i1=DCDC_Service[i].Set_Iout;
                                         CH_PI_Vsense[i].ui=0;
                                         CH_PI_Vsense[i].up=0;
                                         CH_PI_Vsense[i].v1=0;
                                         CH_PI_Vsense[i].Out=0;
                                     }

                              }
                              DCDC_Service[i].CV_Cnt=0;
                              //电压环PI控制
                              CH_PI_Vsense[i].Ref=DCDC_Service[i].Set_Vout;
                              CH_PI_Vsense[i].Fbk=Filter_Coeff.Ad_Result[i][4];
                              PI_Cal_Vol(&CH_PI_Vsense[i]);
                              //电压环输出作为输出电流精度环的控制参考
                              CH_PI_Iout[i].Ref=CH_PI_Vsense[i].Out;
                    }
                    // 第一次启动时控制策略调整，电流缓启动，当参考电流逐加达到额定电流，输出电流精度环切入
                    if(DCDC_Service[i].Flag_Once!=TRUE)
                    {
                           //当参考电流逐加还未达到额定电流值时，不开启输出电流精度环
                            if(DCDC_Service[i].Ref_Iout>DCDC_Service[i].Set_Iout)
                            {
                                  DCDC_Service[i].Ref_Iout=DCDC_Service[i].Ref_Iout+DCDC_Service[i].Slew_Cur_ON;
                                  DCDC_Service[i].Flag_Swtich=FALSE;
                                  CH_PI_Iout[i].i1=0;
                                  CH_PI_Iout[i].ui=0;
                                  CH_PI_Iout[i].up=0;
                                  CH_PI_Iout[i].v1=0;

                                  if(DCDC_Service[i].Ref_Iout<=DCDC_Service[i].Set_Iout)
                                  {
                                      //当参考电流达到额定电流时，开启输出电流精度环控制
                                      DCDC_Service[i].Ref_Iout=DCDC_Service[i].Set_Iout;
                                      DCDC_Service[i].Flag_Swtich=TRUE;
                                      DCDC_Service[i].Flag_Once=TRUE;
                                  }
                            }
                            else if(DCDC_Service[i].Ref_Iout<DCDC_Service[i].Set_Iout)  //当参考电流逐加还未达到额定电流值时，不开启输出电流精度环
                            {
                                   DCDC_Service[i].Ref_Iout=DCDC_Service[i].Ref_Iout-DCDC_Service[i].Slew_Cur_ON;
                                   DCDC_Service[i].Flag_Swtich=FALSE;
                                   CH_PI_Iout[i].i1=0;
                                   CH_PI_Iout[i].ui=0;
                                   CH_PI_Iout[i].up=0;
                                   CH_PI_Iout[i].v1=0;
                                   if( DCDC_Service[i].Ref_Iout>=DCDC_Service[i].Set_Iout)
                                   {
                                       //当参考电流达到额定电流时，开启输出电流精度环控制
                                       DCDC_Service[i].Ref_Iout=DCDC_Service[i].Set_Iout;
                                       DCDC_Service[i].Flag_Swtich=TRUE;
                                       DCDC_Service[i].Flag_Once=TRUE;
                                   }
                            }
                    }
                    //参考电流与输出电流精度环输出作为电感电流内环的参考
                    IdRef[i]=-(DCDC_Service[i].Ref_Iout+CH_PI_Iout[i].Out)*0.5;
                    if(IdRef[i]<=0)
                           IdRef[i]=0;
                    fclVars[i].lsw=ENC_CALIBRATION_DONE;
                }

                //该条件判断为自动进入缓关状态
                if(Interface_DCDC[i].Req_ChangeMode==TRUE)
                {
                      DCDC_Service[i].Now_Mode=DCDC_SOFTOFF;

                      DCDC_Service[i].Ref_Iout=IdRef[i]+IdRef[i];
                      DCDC_Service[i].Slew_Cur_OFF=DCDC_Service[i].Ref_Iout/300;//缓降斜率
                }

                break;
            }
            case DCDC_SOFTOFF:
            {
                  //缓降
                  DCDC_Service[i].Ref_Iout=DCDC_Service[i].Ref_Iout-DCDC_Service[i].Slew_Cur_OFF;

                  //如果缓降的电流小于1A进入PWM关状态
                  if(DCDC_Service[i].Ref_Iout<1)
                  {
                      Interface_DCDC[i].Req_Mode       = DCDC_STANDBY;
                      DCDC_Service[i].Now_Mode         = DCDC_PWM_OFF;
                  }
                  else
                  {
                      //缓关根据不同的标志工作模式进入不同的工作模式
                        if(DCDC_Service[i].Flag_Mode==1)
                        {
                            IdRef[i]=DCDC_Service[i].Ref_Iout*0.5;
                            fclVars[i].lsw=ENC_WAIT_FOR_INDEX;
                        }
                        else if(DCDC_Service[i].Flag_Mode==2)
                        {

                            IdRef[i]=DCDC_Service[i].Ref_Iout*0.5;
                            fclVars[i].lsw=ENC_WAIT_FOR_INDEX;
                        }
                        else if(DCDC_Service[i].Flag_Mode==3)
                        {
                            IdRef[i]=DCDC_Service[i].Ref_Iout*0.5;
                            fclVars[i].lsw=ENC_CALIBRATION_DONE;
                        }
                      
                  }
                 break;

            }
            case DCDC_PWM_OFF:
            {
                fclVars[i].lsw=ENC_ALIGNMENT;

                if(i==0)
                {
                      PWMOFF_Config(1);
                      PWMOFF_Config(2);
                      Rev_Port1_OFF;

                }
                if(i==1)
                {
                      PWMOFF_Config(3);
                      PWMOFF_Config(4);
                      Rev_Port2_OFF;
                }
                if(i==2)
                {
                      PWMOFF_Config(6);
                      PWMOFF_Config(5);
                      Rev_Port3_OFF;
                }
                if(i==3)
                {
                      PWMOFF_Config(7);
                      PWMOFF_Config(8);
                      Rev_Port4_OFF;
                }
                DCDC_Service[i].Now_Mode=DCDC_STANDBY;
                break;
            }
            default:

                break;
        }

    }

}



