/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Protect_Global_Step.c
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义

//化成全局保护变量
struct_Protect_Global       Protect_Global;
struct_Fault_Code           Fault_Code;
struct_Protect_ContactCheck Protect_ContactCheck;
//函数声明


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

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Value_Once(struct_Protect_Value *Ptr)
{
    if(Ptr->Protect_EN==FALSE)
    {
        Ptr->Cnt_Change_Mask=0;
        Ptr->Cnt_Check=0;
        return;
    }

    if(Ptr->Cnt_Change_Mask<Ptr->Num_Change_Mask)
    {
        return;
    }

    if(Ptr->Equation==cStepLess)
    {
        if(Ptr->Now_Value<=Ptr->Ref_Value)
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
    }
    else if(Ptr->Equation==cStepGreater)
    {
        if(Ptr->Now_Value>=Ptr->Ref_Value)
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
    }
    else
    {
        *Ptr->Ptr   |=Ptr->Mask;
    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Value_Diff(struct_Protect_Value_Diff *Ptr)
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

    Ptr->Abs_Value_Diff=Ptr->Now_Value-Ptr->Ref_Value;
    if(Ptr->Abs_Value_Diff<0)
    {
        Ptr->Abs_Value_Diff=-Ptr->Abs_Value_Diff;
    }

    if(Ptr->Equation==cStepLess)
    {
        if(Ptr->Abs_Value_Diff<=Ptr->Ref_Value_Diff)
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
        if(Ptr->Abs_Value_Diff>=Ptr->Ref_Value_Diff)
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

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Value_Diff_Once(struct_Protect_Value_Diff *Ptr)
{
    if(Ptr->Protect_EN==FALSE)
    {
        Ptr->Cnt_Change_Mask=0;
        Ptr->Cnt_Check=0;
        return;
    }

    if(Ptr->Cnt_Change_Mask<Ptr->Num_Change_Mask)
    {
        return;
    }

    Ptr->Abs_Value_Diff=Ptr->Now_Value-Ptr->Ref_Value;
    if(Ptr->Abs_Value_Diff<0)
    {
        Ptr->Abs_Value_Diff=-Ptr->Abs_Value_Diff;
    }

    if(Ptr->Equation==cStepLess)
    {
        if(Ptr->Abs_Value_Diff<=Ptr->Ref_Value_Diff)
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
    }
    else if(Ptr->Equation==cStepGreater)
    {
        if(Ptr->Abs_Value_Diff>=Ptr->Ref_Value_Diff)
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
    }
    else
    {
        *Ptr->Ptr   |=Ptr->Mask;
    }
}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Delta(struct_Protect_Delta *Ptr)
{
    if(Ptr->Protect_EN==FALSE)
    {
        Ptr->Cnt_Change_Mask=0;
        Ptr->Cnt_Check=0;
        return;
    }

    if(Ptr->Cnt_Change_Mask<Ptr->Num_Change_Mask)
    {
        Ptr->Last_Value=Ptr->Now_Value;
        return;
    }

    //Ptr->Cnt_Delta += WorkStep_Data.Record_Time.Delta_Time;

    //if(Ptr->Cnt_Delta>=Ptr->Num_Delta)
    {
        Ptr->Cnt_Delta=0;
        Ptr->Now_Delta=Ptr->Now_Value-Ptr->Last_Value;
        Ptr->Last_Value=Ptr->Now_Value;

        if(Ptr->Equation==cStepLess)
        {
            if(Ptr->Now_Delta<=Ptr->Ref_Delta)
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
            if(Ptr->Now_Delta>=Ptr->Ref_Delta)
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
}
//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Deltafabs(struct_Protect_Delta *Ptr)
{
    if(Ptr->Protect_EN==FALSE)
    {
        Ptr->Cnt_Change_Mask=0;
        Ptr->Cnt_Check=0;
        return;
    }

    if(Ptr->Cnt_Change_Mask<Ptr->Num_Change_Mask)
    {
        Ptr->Last_Value=Ptr->Now_Value;
        return;
    }

    //Ptr->Cnt_Delta += WorkStep_Data.Record_Time.Delta_Time;

    //if((Ptr->Cnt_Delta>=Ptr->Num_Delta)||(WorkStep_Data.Record_Time.Delta_Time <= 1000))
    {
        Ptr->Cnt_Delta=0;
        Ptr->Now_Delta=Ptr->Now_Value-Ptr->Last_Value;
        Ptr->Last_Value=Ptr->Now_Value;
        if(Ptr->Now_Delta < 0)
        {
            Ptr->Now_Delta = -Ptr->Now_Delta;
        }

        if(Ptr->Equation==cStepLess)
        {
            if(Ptr->Now_Delta<=Ptr->Ref_Delta)
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
            if(Ptr->Now_Delta>=Ptr->Ref_Delta)
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
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Range_Once(struct_Protect_Range *Ptr)
{
    if(Ptr->Protect_EN==FALSE)
    {
        return;
    }

    if(WorkStep_Service.Step_Time<Ptr->Time_Delay)
    {
        return;
    }

    if(Ptr->Flag_Check==TRUE)
    {
        return;
    }

    if(Ptr->Equation==cStepLess)
    {
        if((Ptr->Now_Value<Ptr->Ref_Value_H)&&(Ptr->Now_Value>Ptr->Ref_Value_L))
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
    }
    else if(Ptr->Equation==cStepGreater)
    {
        if(Ptr->Now_Value>Ptr->Ref_Value_H)
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
        else if(Ptr->Now_Value<Ptr->Ref_Value_L)
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
    }
    else
    {
        *Ptr->Ptr   |=Ptr->Mask;
    }
    Ptr->Flag_Check=TRUE;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Range_Time(struct_Protect_Range *Ptr)
{
    if(Ptr->Protect_EN==FALSE)
    {
        return;
    }

    if(WorkStep_Service.Step_Time>Ptr->Time_Delay)
    {
        return;
    }

    if(Ptr->Equation==cStepLess)
    {
        if((Ptr->Now_Value<Ptr->Ref_Value_H)&&(Ptr->Now_Value>Ptr->Ref_Value_L))
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
    }
    else if(Ptr->Equation==cStepGreater)
    {
        if(Ptr->Now_Value>Ptr->Ref_Value_H)
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
        else if(Ptr->Now_Value<Ptr->Ref_Value_L)
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
    }
    else
    {
        *Ptr->Ptr   |=Ptr->Mask;
    }
    Ptr->Flag_Check=TRUE;
}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Vol_Change(struct_Protect_Vol_Change *Ptr)
{
    if(Ptr->Protect_EN==FALSE)
    {
        return;
    }

    if(WorkStep_Service.Step_Time<Ptr->Time_Delay1)
    {
        Ptr->Value_t1=Ptr->Now_Value;
    }

    if(WorkStep_Service.Step_Time<Ptr->Time_Delay2)
    {
        Ptr->Value_t2=Ptr->Now_Value;
        return;
    }

    if(Ptr->Flag_Check==TRUE)
    {
        return;
    }

    Ptr->Abs_Delta=Ptr->Value_t2-Ptr->Value_t1;

    if(Ptr->Abs_Delta<0) Ptr->Abs_Delta=-Ptr->Abs_Delta;

    if(Ptr->Equation==cStepLess)
    {
        if(Ptr->Abs_Delta<Ptr->Ref_Delta)
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
    }
    else if(Ptr->Equation==cStepGreater)
    {
        if(Ptr->Abs_Delta>Ptr->Ref_Delta)
        {
            *Ptr->Ptr   |=Ptr->Mask;
        }
    }
    else
    {
        *Ptr->Ptr   |=Ptr->Mask;
    }

    Ptr->Flag_Check=TRUE;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Vol_IntervalChange(struct_Protect_Interval *Ptr)
{
    if(Ptr->Protect_EN==FALSE)
    {
        return;
    }
    if(WorkStep_Service.Step_Time>Ptr->Time_Delay)
    {
        return;
    }

    Ptr->Now_Delta=Ptr->Now_Value-Ptr->Last_Value;

    if(Ptr->Equation==cStepLess)
    {
        if(Ptr->Now_Delta<Ptr->Ref_Delta)
        {
            *Ptr->Ptr|=Ptr->Mask;
        }
    }
    else if(Ptr->Equation==cStepGreater)
    {
        if(Ptr->Now_Delta>Ptr->Ref_Delta)
        {
             *Ptr->Ptr |=Ptr->Mask;
        }
    }
    else
    {
        *Ptr->Ptr   |=Ptr->Mask;
    }

}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_CH_Imbalance(void)
{
    static Uint16 Cnt=0;
    if((Interface_DCDC.Req_Aging==TRUE)&&((DCDC_Service.Now_Mode==DCDC_CV)||(DCDC_Service.Now_Mode==DCDC_DCV)))
    {
        if((Data_Sum.Cur_CHA>CUR_MAX_CHANNEL+80000)||(Data_Sum.Cur_CHB>CUR_MAX_CHANNEL+80000))
        {
            if(++Cnt>4)
            {
                Cnt=0;
                DINT;
                DCDC_Service.Fault_Module|=FAULT_CH_IMBALA;
                EINT;
            }
        }
        else
        {
            Cnt=0;
        }
    }
    else
    {
        Cnt=0;
    }
}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_BatShort(void)
{
	static Uint16 Cnt=0;
	if((Fault_Code.Err_Global&cBatShortFault)!=0)
	{
		if((Data_Sum.Vol_Sense<=200)&&(Data_Sum.Cur_CHA>150)&&(Data_Sum.Cur_CHB>150))
		{
			if(++Cnt>4)
			{
				Cnt=0;
				Fault_Code.Err_Global=Fault_Code.Err_Global|cBatShortFault;
			}
		}
		else
		{
			Cnt=0;
		}
	}
	else
	{
		Cnt=0;
	}
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Mask_Counter(void)
{
    if((DCDC_Service.Mode_Parall==PARALL_SLAVER)||(WorkStep_Service.State!=STEP_RUN))
    {   return;     }

    //Delta类保护屏蔽计数器
    if(Protect_Global.Cur_Fluct_Over.Cnt_Change_Mask<Protect_Global.Cur_Fluct_Over.Num_Change_Mask)
    {
        Protect_Global.Cur_Fluct_Over.Cnt_Change_Mask++;
        Protect_Global.Cur_Fluct_Over.Last_Value=Protect_Global.Cur_Fluct_Over.Now_Value;
    }

    if(Protect_Global.Vol_Fluct_Over.Cnt_Change_Mask<Protect_Global.Vol_Fluct_Over.Num_Change_Mask)
    {
        Protect_Global.Vol_Fluct_Over.Cnt_Change_Mask++;
        Protect_Global.Vol_Fluct_Over.Last_Value=Protect_Global.Vol_Fluct_Over.Now_Value;
    }

    if(WorkStep_Data.Prot_Step_Cur_Delta_Pos.Cnt_Change_Mask<WorkStep_Data.Prot_Step_Cur_Delta_Pos.Num_Change_Mask)
    {
        WorkStep_Data.Prot_Step_Cur_Delta_Pos.Cnt_Change_Mask++;
        WorkStep_Data.Prot_Step_Cur_Delta_Pos.Last_Value=WorkStep_Data.Prot_Step_Cur_Delta_Pos.Now_Value;
    }

    if(WorkStep_Data.Prot_Step_Cur_Delta_Neg.Cnt_Change_Mask<WorkStep_Data.Prot_Step_Cur_Delta_Neg.Num_Change_Mask)
    {
        WorkStep_Data.Prot_Step_Cur_Delta_Neg.Cnt_Change_Mask++;
        WorkStep_Data.Prot_Step_Cur_Delta_Neg.Last_Value=WorkStep_Data.Prot_Step_Cur_Delta_Neg.Now_Value;
    }

    if(WorkStep_Data.Prot_Step_Vol_Delta_Pos.Cnt_Change_Mask<WorkStep_Data.Prot_Step_Vol_Delta_Pos.Num_Change_Mask)
    {
        WorkStep_Data.Prot_Step_Vol_Delta_Pos.Cnt_Change_Mask++;
        WorkStep_Data.Prot_Step_Vol_Delta_Pos.Last_Value=WorkStep_Data.Prot_Step_Vol_Delta_Pos.Now_Value;
    }

    if(WorkStep_Data.Prot_Step_Vol_Delta_Neg.Cnt_Change_Mask<WorkStep_Data.Prot_Step_Vol_Delta_Neg.Num_Change_Mask)
    {
        WorkStep_Data.Prot_Step_Vol_Delta_Neg.Cnt_Change_Mask++;
        WorkStep_Data.Prot_Step_Vol_Delta_Neg.Last_Value=WorkStep_Data.Prot_Step_Vol_Delta_Neg.Now_Value;
    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Check_Delta(void)
{
    if((DCDC_Service.Mode_Parall==PARALL_SLAVER)||(DCDC_Service.Flag_Aging==TRUE)||(WorkStep_Service.State!=STEP_RUN))
    {   return;     }

    //全局电流波动保护只在充电或放电的恒流阶段检测
    if((DCDC_Service.Now_Mode==DCDC_CC )||((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==FALSE))||
       (DCDC_Service.Now_Mode==DCDC_DCC)||((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==FALSE)))
    {
        Protect_Global.Cur_Fluct_Over.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
        Protect_Deltafabs(&Protect_Global.Cur_Fluct_Over);
    }

    //全局电压波动保护在任意工步都检测
    Protect_Global.Vol_Fluct_Over.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
    Protect_Deltafabs(&Protect_Global.Vol_Fluct_Over);

    //单工步电压正跳变保护只在恒流放电阶段检测
    if((DCDC_Service.Now_Mode==DCDC_DCC)||((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==FALSE)))
    {
        WorkStep_Data.Prot_Step_Vol_Delta_Pos.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Delta(&WorkStep_Data.Prot_Step_Vol_Delta_Pos);
    }

    //单工步电压负跳变保护只在恒流充电阶段检测
    if((DCDC_Service.Now_Mode==DCDC_CC )||((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==FALSE)))
    {
        WorkStep_Data.Prot_Step_Vol_Delta_Neg.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Delta(&WorkStep_Data.Prot_Step_Vol_Delta_Neg);
    }

    //单工步电流正跳变保护只在恒流阶段检测
    if((DCDC_Service.Now_Mode==DCDC_CC )||(DCDC_Service.Now_Mode==DCDC_CCtoCV)||
       (DCDC_Service.Now_Mode==DCDC_DCC)||(DCDC_Service.Now_Mode==DCDC_DCCtoDCV))
    {
        WorkStep_Data.Prot_Step_Cur_Delta_Pos.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
        Protect_Delta(&WorkStep_Data.Prot_Step_Cur_Delta_Pos);
    }

    //单工步电流负跳变保护禁用
    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
    Protect_Delta(&WorkStep_Data.Prot_Step_Cur_Delta_Neg);
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Check(void)
{
    if(Image_Protect.Req_TX==FALSE)
    {
        SaveDataToRecord(&Image_Protect.Record);
    }

    if(DCDC_Service.Mode_Parall==PARALL_SLAVER)
    {
    	//从机只从输出端口电压检测电池是否反接
        if(DCDC_Service.Flag_Aging==FALSE)
        {
            Protect_Global.Bat_Reverse_Out.Now_Value=Image_Protect.Record.wOutPutVoltNew_UP;
            Protect_Value(&Protect_Global.Bat_Reverse_Out);
        }
    	return;
    }
    else
    {	//主机从输出端口电压和Sense电压同时检测电池是否反接
        if(DCDC_Service.Flag_Aging==FALSE)
        {
            Protect_Global.Bat_Reverse_Sense.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
            Protect_Value(&Protect_Global.Bat_Reverse_Sense);

            Protect_Global.Bat_Reverse_Out.Now_Value=Image_Protect.Record.wOutPutVoltNew_UP;
            Protect_Value(&Protect_Global.Bat_Reverse_Out);
        }
    }

    if(DCDC_Service.Flag_Aging==FALSE)
    {
        Protect_Global.Bat_Over_Temp.Now_Value=Image_Protect.Record.uwBatteryDegreeNew;
        Protect_Value(&Protect_Global.Bat_Over_Temp);

        Protect_Global.Bat_Under_Temp.Now_Value=Image_Protect.Record.uwBatteryDegreeNew;
        Protect_Value(&Protect_Global.Bat_Under_Temp);

        Protect_Global.Bat_Temp_Alarm.Now_Value=Image_Protect.Record.uwBatteryDegreeNew;
        Protect_Value(&Protect_Global.Bat_Temp_Alarm);

        Protect_Global.Bat_Temp_Over_Alarm.Now_Value=Image_Protect.Record.uwBatteryDegreeNew;
        Protect_Value(&Protect_Global.Bat_Temp_Over_Alarm);

        Protect_Global.CH_Over_Vol_Bat.Now_Value    =Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Value(&Protect_Global.CH_Over_Vol_Bat);
    }

    //接触测试阶段检测项
    if((WorkStep_Service.State==STEP_CONTACT_CHECK)&&(DCDC_Service.Flag_Aging==FALSE))
    {
        Protect_Global.CH_Over_Cur.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
        Protect_Value(&Protect_Global.CH_Over_Cur);

        Protect_Global.Out_Cur_Over.Now_Value       =Image_Protect.Record.dwOutPutCurrentNew_UP;
        Protect_Value(&Protect_Global.Out_Cur_Over);

      //  Protect_Global.CH_Over_Vol_Bat.Now_Value    =Image_Protect.Record.wBatSenseAVoltNew_UP;
      //  Protect_Value(&Protect_Global.CH_Over_Vol_Bat);

      //  Protect_Global.Cur_Diff_Over.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
     //  Protect_Value_Diff(&Protect_Global.Cur_Diff_Over);
        WorkStep_Data.Prot_Step_Cur_Diff_Over.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
        Protect_Value_Diff(&WorkStep_Data.Prot_Step_Cur_Diff_Over);

        //Protect_Global.Bat_Over_Temp.Now_Value=Image_Protect.Record.uwBatteryDegreeNew;
        //if((Fault_Code.Err_Global&cBatTempHighFault)==0)
        //{Protect_Value(&Protect_Global.Bat_Over_Temp);}
    }


    //工步运行阶段全局保护检测项
    if(WorkStep_Service.State==STEP_RUN)
    {
        //充电阶段检测项
        if((DCDC_Service.Now_Mode==DCDC_CC)||(DCDC_Service.Now_Mode==DCDC_CCtoCV))
        {

            Protect_Global.CH_Over_Cur.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
            Protect_Value(&Protect_Global.CH_Over_Cur);

            //老化模式不检测
            if(DCDC_Service.Flag_Aging==FALSE)
            {
                Protect_Global.CH_Over_Ah.Now_Value=Image_Protect.Record.udwChargerCapacityNew;
                Protect_Value(&Protect_Global.CH_Over_Ah);

                //模块输出端口电压检测
            	Protect_Global.CH_Over_Vol_Out.Now_Value=Image_Protect.Record.wOutPutVoltNew_UP;
            //	Protect_Global.CH_Over_Vol_Out.Ref_Value=Protect_Global.CH_Over_Vol_Bat.Ref_Value+(long long)DCDC_Service.Now_Cur_AB*Protect_Global.LineRes/10000;
            	Protect_Global.CH_Over_Vol_Out.Ref_Value=Protect_Global.CH_Over_Vol_Bat.Ref_Value+ Protect_Global.LineVol;
            	Protect_Value(&Protect_Global.CH_Over_Vol_Out);
            }
        }

        //放电阶段检测项
        if((DCDC_Service.Now_Mode==DCDC_DCC)||(DCDC_Service.Now_Mode==DCDC_DCCtoDCV))
        {

            Protect_Global.DIS_Over_Cur.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
            Protect_Value(&Protect_Global.DIS_Over_Cur);

            //老化模式不检测
            if(DCDC_Service.Flag_Aging==FALSE)
            {
                Protect_Global.DIS_Over_Ah.Now_Value=Image_Protect.Record.udwChargerCapacityNew;
                Protect_Value(&Protect_Global.DIS_Over_Ah);

                //模块输出端口电压检测
            	Protect_Global.DIS_Under_Vol_Out.Now_Value=Image_Protect.Record.wOutPutVoltNew_UP;
            	//Protect_Global.DIS_Under_Vol_Out.Ref_Value=Protect_Global.DIS_Under_Vol_Bat.Ref_Value-(long long)DCDC_Service.Now_Cur_AB*Protect_Global.LineRes/10000;
            	Protect_Global.DIS_Under_Vol_Out.Ref_Value=Protect_Global.DIS_Under_Vol_Bat.Ref_Value-Protect_Global.LineVol;
            	Protect_Value(&Protect_Global.DIS_Under_Vol_Out);
            }
        }

        //输出过流，任意工步都检测
        Protect_Global.Out_Cur_Over.Now_Value		=Image_Protect.Record.dwOutPutCurrentNew_UP;
        Protect_Value(&Protect_Global.Out_Cur_Over);


        //老化模式不检测
        if(DCDC_Service.Flag_Aging==FALSE)
        {
            //全局电池过压任意工步都检测
        //    Protect_Global.CH_Over_Vol_Bat.Now_Value    =Image_Protect.Record.wBatSenseAVoltNew_UP;
        //    Protect_Value(&Protect_Global.CH_Over_Vol_Bat);

            Protect_Global.Total_Over_Ah.Now_Value =WorkStep_Service.Total_Ah;
            Protect_Value(&Protect_Global.Total_Over_Ah);

            //全局电池低压任意工步都检测
            Protect_Global.DIS_Under_Vol_Bat.Now_Value  =Image_Protect.Record.wBatSenseAVoltNew_UP;
            Protect_Value(&Protect_Global.DIS_Under_Vol_Bat);

            //电池短路保护检测
            //Protect_BatShort();//debug

            //Protect_Global.Bat_Over_Temp.Now_Value=Image_Protect.Record.uwBatteryDegreeNew;
            //Protect_Value(&Protect_Global.Bat_Over_Temp);

          //  Protect_Global.Bat_Temp_Alarm.Now_Value=Image_Protect.Record.uwBatteryDegreeNew;
          //  Protect_Value(&Protect_Global.Bat_Temp_Alarm);

            //Protect_Global.Bat_Under_Temp.Now_Value=Image_Protect.Record.uwBatteryDegreeNew;
            //Protect_Value(&Protect_Global.Bat_Under_Temp);

        	Protect_Global.Res_Contact_Over.Now_Value=WorkStep_Service.Res_Contact;
            Protect_Value(&Protect_Global.Res_Contact_Over);

         //   Protect_Global.Res_Loop_Over.Now_Value=WorkStep_Service.Res_Loop;
          //  Protect_Value(&Protect_Global.Res_Loop_Over);

        	/*if((DCDC_Service.Now_Mode==DCDC_CC )||((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==FALSE))||
        	   (DCDC_Service.Now_Mode==DCDC_DCC)||((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==FALSE)))
        	{
        		Protect_Global.Cur_Diff_Over.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
        		Protect_Value_Diff(&Protect_Global.Cur_Diff_Over);
        	}

        	if(((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==TRUE))||
        	   ((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==TRUE)))
        	{
        		Protect_Global.Vol_Diff_Over.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        		Protect_Value_Diff(&Protect_Global.Vol_Diff_Over);
        	}*/
        }
    }

    //非电池过热进入暂停状态也进行电池过温检测
	//if((DCDC_Service.Flag_Aging==FALSE)&&(WorkStep_Service.State==STEP_PAUSE))
    //{
    //    Protect_Global.Bat_Over_Temp.Now_Value=Image_Protect.Record.uwBatteryDegreeNew;
    //    if((Fault_Code.Err_Global&cBatTempHighFault)==0)
    //    {Protect_Value(&Protect_Global.Bat_Over_Temp);}
    //}

    //工步运行状态工步保护检测
    if((DCDC_Service.Flag_Aging==FALSE)&&(WorkStep_Service.State==STEP_RUN))
    {
        //充电阶段检测项
        if((DCDC_Service.Now_Mode==DCDC_CC)||(DCDC_Service.Now_Mode==DCDC_CCtoCV)||(DCDC_Service.Now_Mode==DCDC_STATIC))
        {
            WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
            Protect_Value(&WorkStep_Data.Prot_Step_CH_Over_Vol_Bat);
        }

        if((DCDC_Service.Now_Mode==DCDC_CC)||(DCDC_Service.Now_Mode==DCDC_CCtoCV))
        {
            WorkStep_Data.Prot_Step_CH_Over_Ah.Now_Value=Image_Protect.Record.udwChargerCapacityNew;
            Protect_Value(&WorkStep_Data.Prot_Step_CH_Over_Ah);

            WorkStep_Data.Prot_Step_CH_Over_Cur.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
            Protect_Value(&WorkStep_Data.Prot_Step_CH_Over_Cur);
        }

        if(DCDC_Service.Now_Mode==DCDC_STATIC)
        {
            WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Now_Value=Image_Protect.Record.wOutPutVoltNew_UP;
            Protect_Value(&WorkStep_Data.Prot_Step_CH_Over_Vol_Out);

            WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Now_Value=Image_Protect.Record.wOutPutVoltNew_UP;
            Protect_Value(&WorkStep_Data.Prot_Step_DIS_Under_Vol_Out);

            if(DCDC_Service.Mode_Parall==PARALL_MASTER)
            {
                WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Now_Value=Image_Protect.Record.Vol_Contact;
                Protect_Value(&WorkStep_Data.Prot_Step_CH_Over_Vol_Out);

                WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Now_Value=Image_Protect.Record.Vol_Contact;
                Protect_Value(&WorkStep_Data.Prot_Step_DIS_Under_Vol_Out);
            }
        }

        //放电阶段检测项
        if((DCDC_Service.Now_Mode==DCDC_DCC)||(DCDC_Service.Now_Mode==DCDC_DCCtoDCV)||(DCDC_Service.Now_Mode==DCDC_STATIC))
        {
            WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
            Protect_Value(&WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat);
        }

        if((DCDC_Service.Now_Mode==DCDC_DCC)||(DCDC_Service.Now_Mode==DCDC_DCCtoDCV))
        {
        	WorkStep_Data.Prot_Step_DIS_Over_Ah.Now_Value=Image_Protect.Record.udwChargerCapacityNew;
            Protect_Value(&WorkStep_Data.Prot_Step_DIS_Over_Ah);

            WorkStep_Data.Prot_Step_DIS_Over_Cur.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
            Protect_Value(&WorkStep_Data.Prot_Step_DIS_Over_Cur);
        }

        //新加单工步电压超差保护
        if(((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==TRUE))||
                       ((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==TRUE)))
        {
            WorkStep_Data.Prot_Step_Vol_Diff_Over.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
            Protect_Value_Diff(&WorkStep_Data.Prot_Step_Vol_Diff_Over);
        }
        //新加单工步电流超差保护
        if((DCDC_Service.Now_Mode==DCDC_CC )||((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==FALSE))||
                   (DCDC_Service.Now_Mode==DCDC_DCC)||((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==FALSE)))
        {
            WorkStep_Data.Prot_Step_Cur_Diff_Over.Now_Value=Image_Protect.Record.dwOutPutCurrentNew_UP;
            Protect_Value_Diff(&WorkStep_Data.Prot_Step_Cur_Diff_Over);
        }

        //STATIC,CC,DCC,CCtoCV,DCCtoDCV都需要检查
        WorkStep_Data.Prot_Step_Bat_Over_Temp.Now_Value=Image_Protect.Record.uwBatteryDegreeNew;
        Protect_Value(&WorkStep_Data.Prot_Step_Bat_Over_Temp);

        //STATIC,CC,DCC,CCtoCV,DCCtoDCV都需要检查
        WorkStep_Data.Prot_Step_Bat_Under_Temp.Now_Value=Image_Protect.Record.uwBatteryDegreeNew;
        Protect_Value(&WorkStep_Data.Prot_Step_Bat_Under_Temp);

        //新加单工步保护
        WorkStep_Data.Prot_Step_Vol_Time.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Range_Once(&WorkStep_Data.Prot_Step_Vol_Time);

        WorkStep_Data.Prot_Step_Vol_Change_Time.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Vol_Change(&WorkStep_Data.Prot_Step_Vol_Change_Time);

        WorkStep_Data.Prot_Step_BatShort.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Range_Time(&WorkStep_Data.Prot_Step_BatShort);

        if(WorkStep_Data.Vol_Interval_FreshFlag==FALSE)
            WorkStep_Data.Prot_Step_Vol_Interval_Pos.Last_Value = Image_Protect.Record.wBatSenseAVoltNew_UP;
        WorkStep_Data.Prot_Step_Vol_Interval_Pos.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Vol_IntervalChange(&WorkStep_Data.Prot_Step_Vol_Interval_Pos);

        if(WorkStep_Data.Vol_Interval_FreshFlag==FALSE)
                    WorkStep_Data.Prot_Step_Vol_Interval_Neg.Last_Value = Image_Protect.Record.wBatSenseAVoltNew_UP;
        WorkStep_Data.Prot_Step_Vol_Interval_Neg.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Vol_IntervalChange(&WorkStep_Data.Prot_Step_Vol_Interval_Neg);

    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Check_Once(void)
{
    if((DCDC_Service.Mode_Parall==PARALL_SLAVER)||(DCDC_Service.Flag_Aging==TRUE))
    {   return; }

    //工步运行阶段全局保护检测项
    if(WorkStep_Service.State==STEP_RUN)
    {
        //充电阶段检测项
        if((DCDC_Service.Now_Mode==DCDC_CC)||(DCDC_Service.Now_Mode==DCDC_CCtoCV))
        {

            Protect_Global.CH_Over_Cur.Now_Value=Image_UpData.Record.dwOutPutCurrentNew_UP;
            Protect_Value_Once(&Protect_Global.CH_Over_Cur);
        }

        //放电阶段检测项
        if((DCDC_Service.Now_Mode==DCDC_DCC)||(DCDC_Service.Now_Mode==DCDC_DCCtoDCV))
        {
            Protect_Global.DIS_Over_Cur.Now_Value=Image_UpData.Record.dwOutPutCurrentNew_UP;
            Protect_Value_Once(&Protect_Global.DIS_Over_Cur);
        }

        //输出过流，任意工步都检测
        Protect_Global.Out_Cur_Over.Now_Value       =Image_UpData.Record.dwOutPutCurrentNew_UP;
        Protect_Value_Once(&Protect_Global.Out_Cur_Over);

        Protect_Global.Res_Contact_Over.Now_Value   =Image_UpData.Record.Res_Contact;
        Protect_Value_Once(&Protect_Global.Res_Contact_Over);

      //  Protect_Global.Res_Loop_Over.Now_Value      =Image_UpData.Record.Res_Loop;
      //  Protect_Value_Once(&Protect_Global.Res_Loop_Over);

        /*  if((DCDC_Service.Now_Mode==DCDC_CC )||((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==FALSE))||
           (DCDC_Service.Now_Mode==DCDC_DCC)||((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==FALSE)))
        {
            Protect_Global.Cur_Diff_Over.Now_Value=Image_UpData.Record.dwOutPutCurrentNew_UP;
            Protect_Value_Diff_Once(&Protect_Global.Cur_Diff_Over);
        }

        if(((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==TRUE))||
           ((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==TRUE)))
        {
            Protect_Global.Vol_Diff_Over.Now_Value=Image_UpData.Record.wBatSenseAVoltNew_UP;
            Protect_Value_Diff_Once(&Protect_Global.Vol_Diff_Over);
        }
        */
        //充电阶段检测项
        if((DCDC_Service.Now_Mode==DCDC_CC)||(DCDC_Service.Now_Mode==DCDC_CCtoCV))
        {
            WorkStep_Data.Prot_Step_CH_Over_Cur.Now_Value=Image_UpData.Record.dwOutPutCurrentNew_UP;
            Protect_Value_Once(&WorkStep_Data.Prot_Step_CH_Over_Cur);
        }

        //放电阶段检测项
        if((DCDC_Service.Now_Mode==DCDC_DCC)||(DCDC_Service.Now_Mode==DCDC_DCCtoDCV))
        {
            WorkStep_Data.Prot_Step_DIS_Over_Cur.Now_Value=Image_UpData.Record.dwOutPutCurrentNew_UP;
            Protect_Value_Once(&WorkStep_Data.Prot_Step_DIS_Over_Cur);
        }

        //新加单工步保护
        WorkStep_Data.Prot_Step_Vol_Time.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Range_Once(&WorkStep_Data.Prot_Step_Vol_Time);

        WorkStep_Data.Prot_Step_Vol_Change_Time.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Vol_Change(&WorkStep_Data.Prot_Step_Vol_Change_Time);

        WorkStep_Data.Prot_Step_BatShort.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Range_Time(&WorkStep_Data.Prot_Step_BatShort);

        //新加单工步电压电流超差保护
        if((DCDC_Service.Now_Mode==DCDC_CC )||((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==FALSE))||
                 (DCDC_Service.Now_Mode==DCDC_DCC)||((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==FALSE)))
        {
             WorkStep_Data.Prot_Step_Cur_Diff_Over.Now_Value=Image_UpData.Record.dwOutPutCurrentNew_UP;
             Protect_Value_Diff_Once(&WorkStep_Data.Prot_Step_Cur_Diff_Over);
        }

        if(((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==TRUE))||
                 ((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==TRUE)))
        {
             WorkStep_Data.Prot_Step_Vol_Diff_Over.Now_Value=Image_UpData.Record.wBatSenseAVoltNew_UP;
             Protect_Value_Diff_Once(&WorkStep_Data.Prot_Step_Vol_Diff_Over);
        }

        if(WorkStep_Data.Vol_Interval_FreshFlag==FALSE)
            WorkStep_Data.Prot_Step_Vol_Interval_Pos.Last_Value = Image_Protect.Record.wBatSenseAVoltNew_UP;
        WorkStep_Data.Prot_Step_Vol_Interval_Pos.Now_Value=Image_Protect.Record.wBatSenseAVoltNew_UP;
        Protect_Vol_IntervalChange(&WorkStep_Data.Prot_Step_Vol_Interval_Pos);

        if(WorkStep_Data.Vol_Interval_FreshFlag==FALSE)
           WorkStep_Data.Prot_Step_Vol_Interval_Neg.Last_Value = Image_UpData.Record.wBatSenseAVoltNew_UP;
        WorkStep_Data.Prot_Step_Vol_Interval_Neg.Now_Value=Image_UpData.Record.wBatSenseAVoltNew_UP;
        Protect_Vol_IntervalChange(&WorkStep_Data.Prot_Step_Vol_Interval_Neg);


    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Fault_Mapping(void)
{
    //底层故障映射到上层
    Uint32 Temp=DCDC_Service.Fault_Module|DCDC_Service.Fault_Slaver1|DCDC_Service.Fault_Slaver2|DCDC_Service.Fault_Slaver3|DCDC_Service.Fault_Slaver4;
    if(((Temp&FAULT_OC_CHA)!=0)||((Temp&FAULT_OC_CHB)!=0))
    {
        Fault_Code.Err_Global |= cOPShortFault;
    }

    if((Temp&FAULT_FAN)!=0)
    {
        Fault_Code.Err_Global |= cFanFault;
    }

    if((Temp&FAULT_OT_SINK)!=0)
    {
        Fault_Code.Err_Global |= cModuleSinkHigh;
    }

    if((Temp&FAULT_BUS_H)!=0)
    {
        Fault_Code.Err_Global |= cBusVoltHighFault;
    }

    if((Temp&FAULT_BUS_L)!=0)
    {
        Fault_Code.Err_Global |= cBusLow;
    }

    if((Temp&FAULT_OUT_VOL_HIGH)!=0)
    {
        Fault_Code.Err_Global |= cPowerVoltFault;
    }

    if((Temp&FAULT_DATA_CALI)!=0)
    {
        Fault_Code.Err_Global |= cCaliDataError;
    }

    if((Temp&FAULT_AD7606)!=0)
    {
        Fault_Code.Err_Global |= cHardwareError;
    }

    if((Temp&FAULT_SLAVER_CAN)!=0)
    {
        Fault_Code.Err_Global |= cHardwareError;
    }
    if((DCDC_Service.Fault_Module&0x0F000000)!=0)
    {
        Fault_Code.Err_Global |= cHardwareError;
    }

    if((DCDC_Service.Fault_Module&FAULT_CH_IMBALA)!=0)
    {
        Fault_Code.Err_Global |= cHardwareError;
    }

    //记录时间到无故障时清除Past缓冲区数据
    if((Past_Data_Manage.Req_Reset==TRUE)&&(Fault_Code.Err_Global==0)&&(Fault_Code.Err_Step==0)&&(Fault_Code.Alarm==0))
    {
        Past_Data_Manage.Req_Reset=FALSE;
        Past_Data_Manage.Ptr_Out=Past_Data_Manage.Ptr_In;
        Past_Data_Manage.Req_Save=FALSE;
    }

    //故障时数据上报请求
    if(TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus==TRUE)
    {
        if(Fault_Code.Resend_Alarm != 1)
        {
            if(((Fault_Code.Err_Step^Fault_Code.Err_Step_Bak)!=0)||((Fault_Code.Alarm^Fault_Code.Alarm_Bak)!=0))
            {
                Image_Protect.Req_TX=TRUE;
                if(Past_Data_Manage.EN==TRUE)
                {
                    Past_Data_Manage.Req_TX=TRUE;
                }
            }
            else if((Fault_Code.Err_Global^Fault_Code.Err_Global_Bak)!=0)
            {
                if(Past_Data_Manage.EN==TRUE)
                {
                    Past_Data_Manage.Req_TX=TRUE;
                }

                if(((Fault_Code.Err_Global^Fault_Code.Err_Global_Bak)&(cHardwareError|cCaliDataError|cBusLow|cBusVoltHighFault|cModuleSinkHigh|cFanFault|cOPShortFault))!=0)
                {
                   return;
                }

                Image_Protect.Req_TX=TRUE;
             }
        }
        else
        {
            Fault_Code.Resend_Alarm=0;
        }
    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Global_Init(void)
{

    Protect_Global.Bat_Over_Temp.Equation           = cStepGreater;
    Protect_Global.Bat_Over_Temp.Mask               = cBatTempHighFault;
    Protect_Global.Bat_Over_Temp.Num_Change_Mask    = 20;
    Protect_Global.Bat_Over_Temp.Num_Check          = 1;
    Protect_Global.Bat_Over_Temp.Protect_EN         = TRUE;//FALSE;
    Protect_Global.Bat_Over_Temp.Ptr                = &Fault_Code.Alarm;
    Protect_Global.Bat_Over_Temp.Ref_Value          = 2000;

    Protect_Global.Bat_Temp_Alarm.Equation           = cStepGreater;
    Protect_Global.Bat_Temp_Alarm.Mask               = cBatTempAlarmFault;
    Protect_Global.Bat_Temp_Alarm.Num_Change_Mask    = 20;
    Protect_Global.Bat_Temp_Alarm.Num_Check          = 1;
    Protect_Global.Bat_Temp_Alarm.Protect_EN         = TRUE;//FALSE;
    Protect_Global.Bat_Temp_Alarm.Ptr                = &Fault_Code.Alarm;
    Protect_Global.Bat_Temp_Alarm.Ref_Value          = 1900;

    Protect_Global.Bat_Temp_Over_Alarm.Equation      = cStepGreater;
    Protect_Global.Bat_Temp_Over_Alarm.Mask          = cBatTempOverAlarmFault;
    Protect_Global.Bat_Temp_Over_Alarm.Num_Change_Mask= 20;
    Protect_Global.Bat_Temp_Over_Alarm.Num_Check      = 1;
    Protect_Global.Bat_Temp_Over_Alarm.Protect_EN     = TRUE;//FALSE;
    Protect_Global.Bat_Temp_Over_Alarm.Ptr            = &Fault_Code.Alarm;
    Protect_Global.Bat_Temp_Over_Alarm.Ref_Value      = 1900;

    Protect_Global.Bat_Reverse_Sense.Equation       = cStepLess;
    Protect_Global.Bat_Reverse_Sense.Mask           = cBatPNFault;
    Protect_Global.Bat_Reverse_Sense.Num_Change_Mask= 4;
    Protect_Global.Bat_Reverse_Sense.Num_Check      = 1;
    Protect_Global.Bat_Reverse_Sense.Protect_EN     = TRUE;
    Protect_Global.Bat_Reverse_Sense.Ptr            = &Fault_Code.Err_Global;
    Protect_Global.Bat_Reverse_Sense.Ref_Value      = -6000;
    Protect_Global.Bat_Reverse_Sense.Cnt_Check		= 0;
    Protect_Global.Bat_Reverse_Sense.Cnt_Change_Mask= 0;


    Protect_Global.Bat_Reverse_Out.Equation         = cStepLess;
    Protect_Global.Bat_Reverse_Out.Mask             = cBatPNFault;
    Protect_Global.Bat_Reverse_Out.Num_Change_Mask  = 4;
    Protect_Global.Bat_Reverse_Out.Num_Check        = 1;
    Protect_Global.Bat_Reverse_Out.Protect_EN       = TRUE;
    Protect_Global.Bat_Reverse_Out.Ptr              = &Fault_Code.Err_Global;
    Protect_Global.Bat_Reverse_Out.Ref_Value        = -60000;
    Protect_Global.Bat_Reverse_Out.Cnt_Check		= 0;
    Protect_Global.Bat_Reverse_Out.Cnt_Change_Mask	= 0;

    Protect_Global.Bat_Under_Temp.Equation          = cStepLess;
    Protect_Global.Bat_Under_Temp.Mask              = cBatTempLowFault;
    Protect_Global.Bat_Under_Temp.Num_Change_Mask   = 1;
    Protect_Global.Bat_Under_Temp.Num_Check         = 30;
    Protect_Global.Bat_Under_Temp.Protect_EN        = TRUE;//FALSE;
    Protect_Global.Bat_Under_Temp.Ptr               = &Fault_Code.Alarm;
    Protect_Global.Bat_Under_Temp.Ref_Value         = -10;

    Protect_Global.CH_Over_Ah.Equation              = cStepGreater;
    Protect_Global.CH_Over_Ah.Mask                  = cChargeOverCAHFault;
    Protect_Global.CH_Over_Ah.Num_Change_Mask       = 0;
    Protect_Global.CH_Over_Ah.Num_Check             = 1;
    Protect_Global.CH_Over_Ah.Protect_EN            = TRUE;//FALSE;
    Protect_Global.CH_Over_Ah.Ptr                   = &Fault_Code.Err_Global;
    Protect_Global.CH_Over_Ah.Ref_Value             = 10000000;

    Protect_Global.CH_Over_Cur.Equation             = cStepGreater;
    Protect_Global.CH_Over_Cur.Mask                 = cProtectCCFault;
    Protect_Global.CH_Over_Cur.Num_Change_Mask      = 0;
    Protect_Global.CH_Over_Cur.Num_Check            = 2;
    Protect_Global.CH_Over_Cur.Protect_EN           = TRUE;//FALSE;
    Protect_Global.CH_Over_Cur.Ptr                  = &Fault_Code.Err_Global;
    Protect_Global.CH_Over_Cur.Ref_Value            = CUR_MAX_CHANNEL*2+10000;

    Protect_Global.CH_Over_Vol_Bat.Equation         = cStepGreater;
    Protect_Global.CH_Over_Vol_Bat.Mask             = cProtectCVFault;
    Protect_Global.CH_Over_Vol_Bat.Num_Change_Mask  = 0;
    Protect_Global.CH_Over_Vol_Bat.Num_Check        = 1;
    Protect_Global.CH_Over_Vol_Bat.Protect_EN       = TRUE;//FALSE;
    Protect_Global.CH_Over_Vol_Bat.Ptr              = &Fault_Code.Err_Global;
    Protect_Global.CH_Over_Vol_Bat.Ref_Value        = 120000;

    Protect_Global.CH_Over_Vol_Out.Equation         = cStepGreater;
    Protect_Global.CH_Over_Vol_Out.Mask             = cPowerVoltFault;
    Protect_Global.CH_Over_Vol_Out.Num_Change_Mask  = 50;
    Protect_Global.CH_Over_Vol_Out.Num_Check        = 2;
    Protect_Global.CH_Over_Vol_Out.Protect_EN       = TRUE;//FALSE;
    Protect_Global.CH_Over_Vol_Out.Ptr              = &Fault_Code.Err_Global;
    Protect_Global.CH_Over_Vol_Out.Ref_Value        = 120000;//42000;

    Protect_Global.Cur_Diff_Over.Equation           = cStepGreater;
    Protect_Global.Cur_Diff_Over.Mask               = cOPCOverDiffFault;
    Protect_Global.Cur_Diff_Over.Num_Change_Mask    = 20;
    Protect_Global.Cur_Diff_Over.Num_Check          = 2;
    Protect_Global.Cur_Diff_Over.Protect_EN         = TRUE;//FALSE;
    Protect_Global.Cur_Diff_Over.Ptr                = &Fault_Code.Err_Global;
    Protect_Global.Cur_Diff_Over.Ref_Value_Diff     = 3000;
    Protect_Global.Cur_Diff_Over.Ref_Value          = CUR_MAX_CHANNEL*2;

    Protect_Global.Cur_Fluct_Over.Equation          = cStepGreater;
    Protect_Global.Cur_Fluct_Over.Flag_SynUpData	= TRUE;
    Protect_Global.Cur_Fluct_Over.Mask              = cOPCurrentFlucFault;
    Protect_Global.Cur_Fluct_Over.Num_Change_Mask   = 50;//屏蔽2S
    Protect_Global.Cur_Fluct_Over.Num_Check         = 100;//连续10次波动报警
    Protect_Global.Cur_Fluct_Over.Protect_EN        = TRUE;//FALSE;
    Protect_Global.Cur_Fluct_Over.Ptr               = &Fault_Code.Err_Global;
    Protect_Global.Cur_Fluct_Over.Ref_Delta         = 810000;
    Protect_Global.Cur_Fluct_Over.Num_Delta         = 1000;

    Protect_Global.Cur_Mutat_Over.Equation          = cStepGreater;
    Protect_Global.Cur_Mutat_Over.Flag_SynUpData	= FALSE;
    Protect_Global.Cur_Mutat_Over.Mask              = cOPCurrentMutatFault;
    Protect_Global.Cur_Mutat_Over.Num_Change_Mask   = 2;
    Protect_Global.Cur_Mutat_Over.Num_Check         = 1;
    Protect_Global.Cur_Mutat_Over.Protect_EN        = FALSE;
    Protect_Global.Cur_Mutat_Over.Ptr               = &Fault_Code.Err_Global;
    Protect_Global.Cur_Mutat_Over.Ref_Delta         = 600;
    Protect_Global.Cur_Mutat_Over.Num_Delta         = 1000;


    Protect_Global.DIS_Over_Ah.Equation             = cStepGreater;
    Protect_Global.DIS_Over_Ah.Mask                 = cDisChargeOverAHFault;
    Protect_Global.DIS_Over_Ah.Num_Change_Mask      = 0;
    Protect_Global.DIS_Over_Ah.Num_Check            = 1;
    Protect_Global.DIS_Over_Ah.Protect_EN           = TRUE;//FALSE;
    Protect_Global.DIS_Over_Ah.Ptr                  = &Fault_Code.Err_Global;
    Protect_Global.DIS_Over_Ah.Ref_Value            = 10000000;

    Protect_Global.DIS_Over_Cur.Equation            = cStepGreater;
    Protect_Global.DIS_Over_Cur.Mask                = cProtectDCCFault;
    Protect_Global.DIS_Over_Cur.Num_Change_Mask     = 0;
    Protect_Global.DIS_Over_Cur.Num_Check           = 2;
    Protect_Global.DIS_Over_Cur.Protect_EN          = TRUE;//FALSE;
    Protect_Global.DIS_Over_Cur.Ptr                 = &Fault_Code.Err_Global;
    Protect_Global.DIS_Over_Cur.Ref_Value           = CUR_MAX_CHANNEL*2+10000;

    Protect_Global.DIS_Under_Vol_Bat.Equation       = cStepLess;
    Protect_Global.DIS_Under_Vol_Bat.Mask           = cProtectDCVFault;
    Protect_Global.DIS_Under_Vol_Bat.Num_Change_Mask= 0;
    Protect_Global.DIS_Under_Vol_Bat.Num_Check      = 1;
    Protect_Global.DIS_Under_Vol_Bat.Protect_EN     = TRUE;//FALSE;
    Protect_Global.DIS_Under_Vol_Bat.Ptr            = &Fault_Code.Err_Global;
    Protect_Global.DIS_Under_Vol_Bat.Ref_Value      = -60000;

    Protect_Global.DIS_Under_Vol_Out.Equation       = cStepLess;
    Protect_Global.DIS_Under_Vol_Out.Mask           = cPowerVoltFault;
    Protect_Global.DIS_Under_Vol_Out.Num_Change_Mask= 50;
    Protect_Global.DIS_Under_Vol_Out.Num_Check      = 2;
    Protect_Global.DIS_Under_Vol_Out.Protect_EN     = TRUE;//FALSE;
    Protect_Global.DIS_Under_Vol_Out.Ptr            = &Fault_Code.Err_Global;
    Protect_Global.DIS_Under_Vol_Out.Ref_Value      = -60000;

    Protect_Global.Out_Cur_Over.Equation            = cStepGreater;
    Protect_Global.Out_Cur_Over.Mask                = cOPCurrentLimitFault;
    Protect_Global.Out_Cur_Over.Num_Change_Mask     = 0;
    Protect_Global.Out_Cur_Over.Num_Check           = 2;
    Protect_Global.Out_Cur_Over.Protect_EN          = TRUE;//FALSE;
    Protect_Global.Out_Cur_Over.Ptr                 = &Fault_Code.Err_Global;
    Protect_Global.Out_Cur_Over.Ref_Value           =  WorkStep_Service.Cur_Limited_Set+50000;

    Protect_Global.Res_Contact_Over.Equation        = cStepGreater;
    Protect_Global.Res_Contact_Over.Mask            = cImpedanceFault;
    Protect_Global.Res_Contact_Over.Num_Change_Mask = 50;
    Protect_Global.Res_Contact_Over.Num_Check       = 2;
    Protect_Global.Res_Contact_Over.Protect_EN      = TRUE;//FALSE;
    Protect_Global.Res_Contact_Over.Ptr             = &Fault_Code.Err_Global;
    Protect_Global.Res_Contact_Over.Ref_Value       = 30000;

    Protect_Global.Res_Loop_Over.Equation           = cStepGreater;
    Protect_Global.Res_Loop_Over.Mask               = cLoopImpedanceFault;
    Protect_Global.Res_Loop_Over.Num_Change_Mask    = 50;
    Protect_Global.Res_Loop_Over.Num_Check          = 2;
    Protect_Global.Res_Loop_Over.Protect_EN         = FALSE;//FALSE;
    Protect_Global.Res_Loop_Over.Ptr                = &Fault_Code.Err_Global;
    Protect_Global.Res_Loop_Over.Ref_Value          = 30000;

    Protect_Global.Vol_Diff_Over.Equation           = cStepGreater;
    Protect_Global.Vol_Diff_Over.Mask               = cBatVOverDiffFault;
    Protect_Global.Vol_Diff_Over.Num_Change_Mask    = 20;
    Protect_Global.Vol_Diff_Over.Num_Check          = 2;
    Protect_Global.Vol_Diff_Over.Protect_EN         = TRUE;//FALSE;
    Protect_Global.Vol_Diff_Over.Ptr                = &Fault_Code.Err_Global;
    Protect_Global.Vol_Diff_Over.Ref_Value_Diff     = 30;
    Protect_Global.Vol_Diff_Over.Ref_Value          = 42000;

    Protect_Global.Vol_Fluct_Over.Equation          = cStepGreater;
    Protect_Global.Vol_Fluct_Over.Flag_SynUpData	= TRUE;
    Protect_Global.Vol_Fluct_Over.Mask              = cBatVoltFlucFault;
    Protect_Global.Vol_Fluct_Over.Num_Change_Mask   = 76;
    Protect_Global.Vol_Fluct_Over.Num_Check         = 100;
    Protect_Global.Vol_Fluct_Over.Protect_EN        = TRUE;//FALSE;
    Protect_Global.Vol_Fluct_Over.Ptr               = &Fault_Code.Err_Global;
    Protect_Global.Vol_Fluct_Over.Ref_Delta         = 60000;
    Protect_Global.Vol_Fluct_Over.Num_Delta         = 1000;

    Protect_Global.Vol_Mutat_Over.Equation          = cStepGreater;
    Protect_Global.Vol_Mutat_Over.Flag_SynUpData	= FALSE;
    Protect_Global.Vol_Mutat_Over.Mask              = cBatVoltMutatFault;
    Protect_Global.Vol_Mutat_Over.Num_Change_Mask   = 20;
    Protect_Global.Vol_Mutat_Over.Num_Check         = 1;
    Protect_Global.Vol_Mutat_Over.Protect_EN        = FALSE;
    Protect_Global.Vol_Mutat_Over.Ptr               = &Fault_Code.Err_Global;
    Protect_Global.Vol_Mutat_Over.Ref_Delta         = 30;
    Protect_Global.Vol_Mutat_Over.Num_Delta         = 1000;

    Protect_Global.Total_Over_Ah.Equation           = cStepGreater;
    Protect_Global.Total_Over_Ah.Mask               = cTotalOverAHFault;
    Protect_Global.Total_Over_Ah.Num_Change_Mask    = 0;
    Protect_Global.Total_Over_Ah.Num_Check          = 1;
    Protect_Global.Total_Over_Ah.Protect_EN         = TRUE;//FALSE;
    Protect_Global.Total_Over_Ah.Ptr                = &Fault_Code.Err_Global;
    Protect_Global.Total_Over_Ah.Ref_Value          = 10000000;

    Protect_Global.LineRes                          = 350;//35毫欧
    Protect_Global.LineVol                          = 15000;

    Protect_ContactCheck.Protect_EN                 = TRUE;
    Protect_ContactCheck.Limit_Vol_H_Start          = 50000;        //5V
    Protect_ContactCheck.Limit_Vol_L_Start          =-1000;     //-100mV
    Protect_ContactCheck.Cur                        = 10000;    //1A
    Protect_ContactCheck.Time                       = 10000;    //10S
    Protect_ContactCheck.Vol_Rise                   = 1;     //0.01V

}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Global_Cnt_Reset(void)
{
    //全局相关保护计数器清零
    Protect_Global.Bat_Over_Temp.Cnt_Change_Mask    = 0;
    Protect_Global.Bat_Over_Temp.Cnt_Check          = 0;

    Protect_Global.Bat_Reverse_Sense.Cnt_Change_Mask= 0;
    Protect_Global.Bat_Reverse_Sense.Cnt_Check      = 0;

    Protect_Global.Bat_Reverse_Out.Cnt_Change_Mask  = 0;
    Protect_Global.Bat_Reverse_Out.Cnt_Check        = 0;

    Protect_Global.Bat_Under_Temp.Cnt_Change_Mask   = 0;
    Protect_Global.Bat_Under_Temp.Cnt_Check         = 0;

    Protect_Global.CH_Over_Ah.Cnt_Change_Mask       = 0;
    Protect_Global.CH_Over_Ah.Cnt_Check             = 0;

    Protect_Global.CH_Over_Cur.Cnt_Change_Mask      = 0;
    Protect_Global.CH_Over_Cur.Cnt_Check            = 0;

    Protect_Global.CH_Over_Vol_Bat.Cnt_Change_Mask  = 0;
    Protect_Global.CH_Over_Vol_Bat.Cnt_Check        = 0;

    Protect_Global.CH_Over_Vol_Out.Cnt_Change_Mask  = 0;//debug
    Protect_Global.CH_Over_Vol_Out.Cnt_Check        = 0;//debug

    Protect_Global.DIS_Over_Ah.Cnt_Change_Mask      = 0;
    Protect_Global.DIS_Over_Ah.Cnt_Check            = 0;

    Protect_Global.DIS_Over_Cur.Cnt_Change_Mask     = 0;
    Protect_Global.DIS_Over_Cur.Cnt_Check           = 0;

    Protect_Global.DIS_Under_Vol_Bat.Cnt_Change_Mask= 0;
    Protect_Global.DIS_Under_Vol_Bat.Cnt_Check      = 0;

    Protect_Global.DIS_Under_Vol_Out.Cnt_Change_Mask= 0;//debug
    Protect_Global.DIS_Under_Vol_Out.Cnt_Check      = 0;//debug

    Protect_Global.Cur_Diff_Over.Cnt_Change_Mask    = 0;
    Protect_Global.Cur_Diff_Over.Cnt_Check          = 0;

    Protect_Global.Cur_Fluct_Over.Cnt_Change_Mask   = 0;
    Protect_Global.Cur_Fluct_Over.Cnt_Check         = 0;
    Protect_Global.Cur_Fluct_Over.Cnt_Delta	        = 0;

    Protect_Global.Cur_Mutat_Over.Cnt_Change_Mask   = 0;
    Protect_Global.Cur_Mutat_Over.Cnt_Check         = 0;
    Protect_Global.Cur_Mutat_Over.Cnt_Delta         = 0;

    Protect_Global.Vol_Diff_Over.Cnt_Change_Mask    = 0;
    Protect_Global.Vol_Diff_Over.Cnt_Check          = 0;

    Protect_Global.Vol_Fluct_Over.Cnt_Change_Mask   = 0;
    Protect_Global.Vol_Fluct_Over.Cnt_Check         = 0;
    Protect_Global.Vol_Fluct_Over.Cnt_Delta			= 0;

    Protect_Global.Vol_Mutat_Over.Cnt_Change_Mask   = 0;
    Protect_Global.Vol_Mutat_Over.Cnt_Check         = 0;
    Protect_Global.Vol_Mutat_Over.Cnt_Delta			= 0;

    Protect_Global.Res_Contact_Over.Cnt_Change_Mask = 0;
    Protect_Global.Res_Contact_Over.Cnt_Check       = 0;

    Protect_Global.Res_Loop_Over.Cnt_Change_Mask    = 0;
    Protect_Global.Res_Loop_Over.Cnt_Check          = 0;

    Protect_Global.Out_Cur_Over.Cnt_Change_Mask     = 0;
    Protect_Global.Out_Cur_Over.Cnt_Check           = 0;

    Protect_Global.Total_Over_Ah.Cnt_Change_Mask      = 0;
    Protect_Global.Total_Over_Ah.Cnt_Check            = 0;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Step_Cnt_Reset(void)
{
    //单工步相关保护相关保护计数器清零
    WorkStep_Data.Prot_Step_CH_Over_Cur.Cnt_Change_Mask         = 0;
    WorkStep_Data.Prot_Step_CH_Over_Cur.Cnt_Check               = 0;

    WorkStep_Data.Prot_Step_CH_Over_Ah.Cnt_Change_Mask          = 0;
    WorkStep_Data.Prot_Step_CH_Over_Ah.Cnt_Check                = 0;

    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Cnt_Change_Mask     = 0;
    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Cnt_Check           = 0;

    WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Cnt_Change_Mask     = 0;
    WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Cnt_Check           = 0;

    WorkStep_Data.Prot_Step_DIS_Over_Cur.Cnt_Change_Mask        = 0;
    WorkStep_Data.Prot_Step_DIS_Over_Cur.Cnt_Check              = 0;

    WorkStep_Data.Prot_Step_DIS_Over_Ah.Cnt_Change_Mask         = 0;
    WorkStep_Data.Prot_Step_DIS_Over_Ah.Cnt_Check               = 0;

    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Cnt_Change_Mask   = 0;
    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Cnt_Check         = 0;

    WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Cnt_Change_Mask   = 0;
    WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Cnt_Check         = 0;

    WorkStep_Data.Prot_Step_Bat_Over_Temp.Cnt_Change_Mask       = 0;
    WorkStep_Data.Prot_Step_Bat_Over_Temp.Cnt_Check             = 0;

    WorkStep_Data.Prot_Step_Bat_Under_Temp.Cnt_Change_Mask       = 0;
    WorkStep_Data.Prot_Step_Bat_Under_Temp.Cnt_Check             = 0;

    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Cnt_Change_Mask       = 0;
    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Cnt_Check             = 0;
    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Cnt_Delta             = 0;

    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Cnt_Change_Mask       = 0;
    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Cnt_Check             = 0;
    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Cnt_Delta             = 0;

    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Cnt_Change_Mask       = 0;
    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Cnt_Check             = 0;
    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Cnt_Delta             = 0;

    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Cnt_Change_Mask       = 0;
    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Cnt_Check             = 0;
    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Cnt_Delta             = 0;

    Protect_Global.Cur_Fluct_Over.Cnt_Change_Mask   			= 0;
    Protect_Global.Cur_Fluct_Over.Cnt_Check         			= 0;
    Protect_Global.Cur_Fluct_Over.Cnt_Delta	        			= 0;

    Protect_Global.Vol_Fluct_Over.Cnt_Change_Mask   			= 0;
    Protect_Global.Vol_Fluct_Over.Cnt_Check         			= 0;
    Protect_Global.Vol_Fluct_Over.Cnt_Delta						= 0;

    //新工步保护
    WorkStep_Data.Prot_Step_Vol_Diff_Over.Cnt_Change_Mask    = 0;
    WorkStep_Data.Prot_Step_Vol_Diff_Over.Cnt_Check          = 0;

    WorkStep_Data.Prot_Step_Cur_Diff_Over.Cnt_Change_Mask    = 0;
    WorkStep_Data.Prot_Step_Cur_Diff_Over.Cnt_Check          = 0;

    WorkStep_Data.Prot_Step_Vol_Time.Flag_Check                 = FALSE;
    WorkStep_Data.Prot_Step_Vol_Change_Time.Flag_Check          = FALSE;
    WorkStep_Data.Prot_Step_BatShort.Flag_Check                 = FALSE;
}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Step_Init(void)
{
    WorkStep_Data.Prot_Step_CH_Over_Cur.Protect_EN              = FALSE;
    WorkStep_Data.Prot_Step_CH_Over_Cur.Mask                    = cCurrUpperLimit;
    WorkStep_Data.Prot_Step_CH_Over_Cur.Num_Change_Mask         = 0;
    WorkStep_Data.Prot_Step_CH_Over_Cur.Num_Check               = 2;
    WorkStep_Data.Prot_Step_CH_Over_Cur.Ptr                     = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_CH_Over_Ah.Protect_EN               = FALSE;
    WorkStep_Data.Prot_Step_CH_Over_Ah.Mask                     = cCapacityUpperLimit;
    WorkStep_Data.Prot_Step_CH_Over_Ah.Num_Change_Mask          = 0;
    WorkStep_Data.Prot_Step_CH_Over_Ah.Num_Check                = 1;
    WorkStep_Data.Prot_Step_CH_Over_Ah.Ptr                      = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Protect_EN          = FALSE;
    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Mask                = cBatVoltUpperLimit;
    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Num_Change_Mask     = 0;
    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Num_Check           = 1;
    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Ptr                 = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Protect_EN          = FALSE;
    WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Mask                = cBatVoltUpperLimit;
    WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Num_Change_Mask     = 40;
    WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Num_Check           = 1;
    WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Ptr                 = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_DIS_Over_Cur.Protect_EN             = FALSE;
    WorkStep_Data.Prot_Step_DIS_Over_Cur.Mask                   = cCurrLowerLimit;
    WorkStep_Data.Prot_Step_DIS_Over_Cur.Num_Change_Mask        = 0;
    WorkStep_Data.Prot_Step_DIS_Over_Cur.Num_Check              = 2;
    WorkStep_Data.Prot_Step_DIS_Over_Cur.Ptr                    = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_DIS_Over_Ah.Protect_EN              = FALSE;
    WorkStep_Data.Prot_Step_DIS_Over_Ah.Mask                    = cCapacityLowerLimit;
    WorkStep_Data.Prot_Step_DIS_Over_Ah.Num_Change_Mask         = 0;
    WorkStep_Data.Prot_Step_DIS_Over_Ah.Num_Check               = 1;
    WorkStep_Data.Prot_Step_DIS_Over_Ah.Ptr                     = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Protect_EN        = FALSE;
    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Mask              = cBatVoltLowerLimit;
    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Num_Change_Mask   = 0;
    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Num_Check         = 1;
    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Ptr               = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Protect_EN        = FALSE;
    WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Mask              = cBatVoltLowerLimit;
    WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Num_Change_Mask   = 40;
    WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Num_Check         = 1;
    WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Ptr               = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_Bat_Over_Temp.Protect_EN            = FALSE;
    WorkStep_Data.Prot_Step_Bat_Over_Temp.Mask                  = cBatTempUpperLimit;
    WorkStep_Data.Prot_Step_Bat_Over_Temp.Num_Change_Mask       = 0;
    WorkStep_Data.Prot_Step_Bat_Over_Temp.Num_Check             = 1;
    WorkStep_Data.Prot_Step_Bat_Over_Temp.Ptr                   = &Fault_Code.Alarm;

    WorkStep_Data.Prot_Step_Bat_Under_Temp.Protect_EN            = FALSE;
    WorkStep_Data.Prot_Step_Bat_Under_Temp.Mask                  = cBatTempDownUpperLimit;
    WorkStep_Data.Prot_Step_Bat_Under_Temp.Num_Change_Mask       = 1;
    WorkStep_Data.Prot_Step_Bat_Under_Temp.Num_Check             = 30;
    WorkStep_Data.Prot_Step_Bat_Under_Temp.Ptr                   = &Fault_Code.Alarm;
    WorkStep_Data.Prot_Step_Bat_Under_Temp.Ref_Value             = -10;

    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Protect_EN            = FALSE;
    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Flag_SynUpData		= TRUE;
    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Mask                  = cOPCPositiveJump;
    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Num_Change_Mask       = 50;
    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Num_Check             = 1;
    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Num_Delta             = 0;
    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Ptr                   = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Protect_EN            = FALSE;
    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Flag_SynUpData		= TRUE;
    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Mask                  = cBatVoltPositiveJump;
    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Num_Change_Mask       = 76;
    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Num_Check             = 1;
    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Num_Delta             = 0;
    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Ptr                   = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Protect_EN            = FALSE;
    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Flag_SynUpData		= TRUE;
    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Mask                  = cOPCNegativeJump;
    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Num_Change_Mask       = 50;
    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Num_Check             = 1;
    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Num_Delta             = 0;
    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Ptr                   = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Protect_EN            = FALSE;
    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Flag_SynUpData		= TRUE;
    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Mask                  = cBatVoltNegativeJump;
    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Num_Change_Mask       = 76;
    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Num_Check             = 1;
    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Num_Delta             = 0;
    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Ptr                   = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_Vol_Time.Protect_EN                 = FALSE;
    WorkStep_Data.Prot_Step_Vol_Time.Equation                   = cStepGreater;//cStepGreater表示超出范围认为故障
    WorkStep_Data.Prot_Step_Vol_Time.Flag_Check                 = FALSE;
    WorkStep_Data.Prot_Step_Vol_Time.Mask                       = cVol_Time_Error;
    WorkStep_Data.Prot_Step_Vol_Time.Ptr                        = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_Vol_Change_Time.Protect_EN          = FALSE;
    WorkStep_Data.Prot_Step_Vol_Change_Time.Equation            = cStepGreater;//cStepGreater表示大于参考值认为故障
    WorkStep_Data.Prot_Step_Vol_Change_Time.Flag_Check          = FALSE;
    WorkStep_Data.Prot_Step_Vol_Change_Time.Mask                = cVol_Change_Time_Error;
    WorkStep_Data.Prot_Step_Vol_Change_Time.Ptr                 = &Fault_Code.Err_Step;

    WorkStep_Data.Prot_Step_BatShort.Protect_EN                 = FALSE;
    WorkStep_Data.Prot_Step_BatShort.Equation                   = cStepGreater;//cStepGreater表示超出范围认为故障
    WorkStep_Data.Prot_Step_BatShort.Flag_Check                 = FALSE;
    WorkStep_Data.Prot_Step_BatShort.Mask                       = cBatShort;
    WorkStep_Data.Prot_Step_BatShort.Ptr                        = &Fault_Code.Err_Step;

    //新加工步电压超差保护
    WorkStep_Data.Prot_Step_Vol_Diff_Over.Equation           = cStepGreater;
    WorkStep_Data.Prot_Step_Vol_Diff_Over.Mask               = cVol_Diff;
    WorkStep_Data.Prot_Step_Vol_Diff_Over.Num_Change_Mask    = 20;
    WorkStep_Data.Prot_Step_Vol_Diff_Over.Num_Check          = 2;
    WorkStep_Data.Prot_Step_Vol_Diff_Over.Protect_EN         = FALSE;//FALSE;
    WorkStep_Data.Prot_Step_Vol_Diff_Over.Ptr                = &Fault_Code.Err_Step;
    WorkStep_Data.Prot_Step_Vol_Diff_Over.Ref_Value_Diff     = 30;
    WorkStep_Data.Prot_Step_Vol_Diff_Over.Ref_Value          = 42000;

    //新加工步电流超差保护
    WorkStep_Data.Prot_Step_Cur_Diff_Over.Equation           = cStepGreater;
    WorkStep_Data.Prot_Step_Cur_Diff_Over.Mask               = cCur_Diff;
    WorkStep_Data.Prot_Step_Cur_Diff_Over.Num_Change_Mask    = 20;
    WorkStep_Data.Prot_Step_Cur_Diff_Over.Num_Check          = 2;
    WorkStep_Data.Prot_Step_Cur_Diff_Over.Protect_EN         = FALSE;//FALSE;
    WorkStep_Data.Prot_Step_Cur_Diff_Over.Ptr                = &Fault_Code.Err_Step;
    WorkStep_Data.Prot_Step_Cur_Diff_Over.Ref_Value_Diff     = 3000;
    WorkStep_Data.Prot_Step_Cur_Diff_Over.Ref_Value          = CUR_MAX_CHANNEL*2;

    //新加跨工步正间隔电压变化保护
    WorkStep_Data.Prot_Step_Vol_Interval_Pos.Protect_EN      = FALSE;
    WorkStep_Data.Prot_Step_Vol_Interval_Pos.Equation        = cStepGreater;//cStepGreater表示超出范围认为故障
    WorkStep_Data.Prot_Step_Vol_Interval_Pos.Mask            = cVoltIntervalPos;
    WorkStep_Data.Prot_Step_Vol_Interval_Pos.Ptr             = &Fault_Code.Err_Step;
    WorkStep_Data.Prot_Step_Vol_Interval_Pos.Time_Delay      = 2000;

    //新加跨工步负间隔电压变化保护
    WorkStep_Data.Prot_Step_Vol_Interval_Neg.Protect_EN      = FALSE;
    WorkStep_Data.Prot_Step_Vol_Interval_Neg.Equation        = cStepLess;//cStepGreater表示超出范围认为故障
    WorkStep_Data.Prot_Step_Vol_Interval_Neg.Mask            = cVoltIntervalNeg;
    WorkStep_Data.Prot_Step_Vol_Interval_Neg.Ptr             = &Fault_Code.Err_Step;
    WorkStep_Data.Prot_Step_Vol_Interval_Neg.Time_Delay      = 2000;




}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Configure_CC(void)
{
    Protect_Global.Bat_Over_Temp.Protect_EN             = TRUE;
    Protect_Global.Bat_Temp_Alarm.Protect_EN            = TRUE;
    Protect_Global.Bat_Reverse_Sense.Protect_EN         = TRUE;
    Protect_Global.Bat_Reverse_Out.Protect_EN         	= TRUE;
    Protect_Global.Bat_Under_Temp.Protect_EN            = TRUE;
    Protect_Global.CH_Over_Ah.Protect_EN                = TRUE;
    Protect_Global.CH_Over_Cur.Protect_EN               = TRUE;
    Protect_Global.CH_Over_Vol_Bat.Protect_EN           = TRUE;
    Protect_Global.CH_Over_Vol_Out.Protect_EN           = TRUE;

    Protect_Global.DIS_Over_Ah.Protect_EN               = FALSE;
    Protect_Global.DIS_Over_Cur.Protect_EN              = FALSE;
    Protect_Global.DIS_Under_Vol_Bat.Protect_EN         = TRUE;
    Protect_Global.DIS_Under_Vol_Out.Protect_EN         = FALSE;

    Protect_Global.Cur_Diff_Over.Protect_EN             = TRUE;
    Protect_Global.Cur_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Cur;

    Protect_Global.Vol_Diff_Over.Protect_EN             = FALSE;
    Protect_Global.Vol_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Vol;

    Protect_Global.Cur_Fluct_Over.Protect_EN			= TRUE;
    Protect_Global.Vol_Fluct_Over.Protect_EN			= TRUE;

    Protect_Global.Res_Contact_Over.Protect_EN          = TRUE;
    Protect_Global.Res_Loop_Over.Protect_EN             = TRUE;
    Protect_Global.Out_Cur_Over.Protect_EN              = TRUE;
    Protect_Global.Total_Over_Ah.Protect_EN             = TRUE;
    //单工步相关保护禁止与使能
    //WorkStep_Data.Prot_Step_CH_Over_Cur.Protect_EN      = TRUE;
    //WorkStep_Data.Prot_Step_CH_Over_Ah.Protect_EN       = TRUE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Protect_EN  = TRUE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Protect_EN  = FALSE;

    //WorkStep_Data.Prot_Step_DIS_Over_Cur.Protect_EN     = FALSE;
    //WorkStep_Data.Prot_Step_DIS_Over_Ah.Protect_EN      = FALSE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Protect_EN= FALSE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Protect_EN= FALSE;

    //WorkStep_Data.Prot_Step_Bat_Over_Temp.Protect_EN    = TRUE;

    //WorkStep_Data.Prot_Step_Vol_Delta_Pos.Protect_EN    = FALSE;
    //WorkStep_Data.Prot_Step_Vol_Delta_Neg.Protect_EN    = TRUE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Pos.Protect_EN    = TRUE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Neg.Protect_EN    = FALSE;

    //WorkStep_Data.Prot_Step_Vol_Time.Protect_EN         = TRUE;
    //WorkStep_Data.Prot_Step_Vol_Change_Time.Protect_EN  = TRUE;
    //WorkStep_Data.Prot_Step_BatShort.Protect_EN         = TRUE;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Configure_CCtoCV(void)
{
    //全局相关保护禁止与使能
    Protect_Global.Bat_Over_Temp.Protect_EN             = TRUE;
    Protect_Global.Bat_Temp_Alarm.Protect_EN            = TRUE;
    Protect_Global.Bat_Reverse_Sense.Protect_EN         = TRUE;
    Protect_Global.Bat_Reverse_Out.Protect_EN         	= TRUE;
    Protect_Global.Bat_Under_Temp.Protect_EN            = TRUE;
    Protect_Global.CH_Over_Ah.Protect_EN                = TRUE;
    Protect_Global.CH_Over_Cur.Protect_EN               = TRUE;
    Protect_Global.CH_Over_Vol_Bat.Protect_EN           = TRUE;
    Protect_Global.CH_Over_Vol_Out.Protect_EN           = TRUE;

    Protect_Global.DIS_Over_Ah.Protect_EN               = FALSE;
    Protect_Global.DIS_Over_Cur.Protect_EN              = FALSE;
    Protect_Global.DIS_Under_Vol_Bat.Protect_EN         = TRUE;
    Protect_Global.DIS_Under_Vol_Out.Protect_EN         = FALSE;

    Protect_Global.Cur_Diff_Over.Protect_EN             = TRUE;
    Protect_Global.Cur_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Cur;

    Protect_Global.Vol_Diff_Over.Protect_EN             = TRUE;
    Protect_Global.Vol_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Vol;

    Protect_Global.Res_Contact_Over.Protect_EN          = TRUE;
    Protect_Global.Res_Loop_Over.Protect_EN             = TRUE;
    Protect_Global.Out_Cur_Over.Protect_EN              = TRUE;

    Protect_Global.Cur_Fluct_Over.Protect_EN			= TRUE;
    Protect_Global.Vol_Fluct_Over.Protect_EN			= TRUE;
    Protect_Global.Total_Over_Ah.Protect_EN             = TRUE;
    //单工步相关保护禁止与使能
    //WorkStep_Data.Prot_Step_CH_Over_Cur.Protect_EN      = TRUE;
    //WorkStep_Data.Prot_Step_CH_Over_Ah.Protect_EN       = TRUE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Protect_EN  = TRUE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Protect_EN  = FALSE;

    //WorkStep_Data.Prot_Step_DIS_Over_Cur.Protect_EN     = FALSE;
    //WorkStep_Data.Prot_Step_DIS_Over_Ah.Protect_EN      = FALSE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Protect_EN= FALSE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Protect_EN= FALSE;

    //WorkStep_Data.Prot_Step_Bat_Over_Temp.Protect_EN    = TRUE;

    //WorkStep_Data.Prot_Step_Vol_Delta_Pos.Protect_EN    = FALSE;
    //WorkStep_Data.Prot_Step_Vol_Delta_Neg.Protect_EN    = TRUE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Pos.Protect_EN    = TRUE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Neg.Protect_EN    = FALSE;

    //WorkStep_Data.Prot_Step_Vol_Time.Protect_EN         = TRUE;
    //WorkStep_Data.Prot_Step_Vol_Change_Time.Protect_EN  = TRUE;
    //WorkStep_Data.Prot_Step_BatShort.Protect_EN         = TRUE;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Configure_DCC(void)
{
    //全局相关保护禁止与使能
    Protect_Global.Bat_Over_Temp.Protect_EN             = TRUE;
    Protect_Global.Bat_Reverse_Sense.Protect_EN         = TRUE;
    Protect_Global.Bat_Temp_Alarm.Protect_EN            = TRUE;
    Protect_Global.Bat_Reverse_Out.Protect_EN         	= TRUE;
    Protect_Global.Bat_Under_Temp.Protect_EN            = TRUE;
    Protect_Global.CH_Over_Ah.Protect_EN                = FALSE;
    Protect_Global.CH_Over_Cur.Protect_EN               = FALSE;
    Protect_Global.CH_Over_Vol_Bat.Protect_EN           = TRUE;
    Protect_Global.CH_Over_Vol_Out.Protect_EN           = FALSE;

    Protect_Global.DIS_Over_Ah.Protect_EN               = TRUE;
    Protect_Global.DIS_Over_Cur.Protect_EN              = TRUE;
    Protect_Global.DIS_Under_Vol_Bat.Protect_EN         = TRUE;
    Protect_Global.DIS_Under_Vol_Out.Protect_EN         = TRUE;

    Protect_Global.Cur_Diff_Over.Protect_EN             = TRUE;
    Protect_Global.Cur_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Cur;

    Protect_Global.Vol_Diff_Over.Protect_EN             = FALSE;
    Protect_Global.Vol_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Vol;

    Protect_Global.Res_Contact_Over.Protect_EN          = TRUE;
    Protect_Global.Res_Loop_Over.Protect_EN             = TRUE;
    Protect_Global.Out_Cur_Over.Protect_EN              = TRUE;

    Protect_Global.Cur_Fluct_Over.Protect_EN			= TRUE;
    Protect_Global.Vol_Fluct_Over.Protect_EN			= TRUE;
    Protect_Global.Total_Over_Ah.Protect_EN             = TRUE;
    //单工步相关保护禁止与使能
    //WorkStep_Data.Prot_Step_CH_Over_Cur.Protect_EN      = FALSE;
    //WorkStep_Data.Prot_Step_CH_Over_Ah.Protect_EN       = FALSE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Protect_EN  = FALSE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Protect_EN  = FALSE;

    //WorkStep_Data.Prot_Step_DIS_Over_Cur.Protect_EN     = TRUE;
    //WorkStep_Data.Prot_Step_DIS_Over_Ah.Protect_EN      = TRUE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Protect_EN= TRUE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Protect_EN= FALSE;

    //WorkStep_Data.Prot_Step_Bat_Over_Temp.Protect_EN    = TRUE;

    //WorkStep_Data.Prot_Step_Vol_Delta_Pos.Protect_EN    = TRUE;
    //WorkStep_Data.Prot_Step_Vol_Delta_Neg.Protect_EN    = FALSE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Pos.Protect_EN    = TRUE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Neg.Protect_EN    = FALSE;

    //WorkStep_Data.Prot_Step_Vol_Time.Protect_EN         = TRUE;
    //WorkStep_Data.Prot_Step_Vol_Change_Time.Protect_EN  = TRUE;
    //WorkStep_Data.Prot_Step_BatShort.Protect_EN         = TRUE;
}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Configure_DCCtoDCV(void)
{
    //全局相关保护禁止与使能
    Protect_Global.Bat_Over_Temp.Protect_EN             = TRUE;
    Protect_Global.Bat_Temp_Alarm.Protect_EN            = TRUE;
    Protect_Global.Bat_Reverse_Sense.Protect_EN         = TRUE;
    Protect_Global.Bat_Reverse_Out.Protect_EN         	= TRUE;
    Protect_Global.Bat_Under_Temp.Protect_EN            = TRUE;
    Protect_Global.CH_Over_Ah.Protect_EN                = FALSE;
    Protect_Global.CH_Over_Cur.Protect_EN               = FALSE;
    Protect_Global.CH_Over_Vol_Bat.Protect_EN           = TRUE;
    Protect_Global.CH_Over_Vol_Out.Protect_EN           = FALSE;

    Protect_Global.DIS_Over_Ah.Protect_EN               = TRUE;
    Protect_Global.DIS_Over_Cur.Protect_EN              = TRUE;
    Protect_Global.DIS_Under_Vol_Bat.Protect_EN         = TRUE;
    Protect_Global.DIS_Under_Vol_Out.Protect_EN         = TRUE;

    Protect_Global.Cur_Diff_Over.Protect_EN             = TRUE;
    Protect_Global.Cur_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Cur;

    Protect_Global.Vol_Diff_Over.Protect_EN             = TRUE;
    Protect_Global.Vol_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Vol;

    Protect_Global.Res_Contact_Over.Protect_EN          = TRUE;
    Protect_Global.Res_Loop_Over.Protect_EN             = TRUE;
    Protect_Global.Out_Cur_Over.Protect_EN              = TRUE;

    Protect_Global.Cur_Fluct_Over.Protect_EN			= TRUE;
    Protect_Global.Vol_Fluct_Over.Protect_EN			= TRUE;
    Protect_Global.Total_Over_Ah.Protect_EN             = TRUE;

    //单工步相关保护禁止与使能
    //WorkStep_Data.Prot_Step_CH_Over_Cur.Protect_EN      = FALSE;
    //WorkStep_Data.Prot_Step_CH_Over_Ah.Protect_EN       = FALSE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Protect_EN  = FALSE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Protect_EN  = FALSE;

    //WorkStep_Data.Prot_Step_DIS_Over_Cur.Protect_EN     = TRUE;
    //WorkStep_Data.Prot_Step_DIS_Over_Ah.Protect_EN      = TRUE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Protect_EN= TRUE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Protect_EN= FALSE;


    //WorkStep_Data.Prot_Step_Bat_Over_Temp.Protect_EN    = TRUE;

    //WorkStep_Data.Prot_Step_Vol_Delta_Pos.Protect_EN    = TRUE;
    //WorkStep_Data.Prot_Step_Vol_Delta_Neg.Protect_EN    = FALSE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Pos.Protect_EN    = TRUE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Neg.Protect_EN    = FALSE;

    //WorkStep_Data.Prot_Step_Vol_Time.Protect_EN         = TRUE;
    //WorkStep_Data.Prot_Step_Vol_Change_Time.Protect_EN  = TRUE;
    //WorkStep_Data.Prot_Step_BatShort.Protect_EN         = TRUE;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Configure_STATIC(void)
{
    //全局相关保护禁止与使能
    Protect_Global.Bat_Over_Temp.Protect_EN             = TRUE;
    Protect_Global.Bat_Temp_Alarm.Protect_EN            = TRUE;
    Protect_Global.Bat_Reverse_Sense.Protect_EN         = TRUE;
    Protect_Global.Bat_Reverse_Out.Protect_EN         	= TRUE;
    Protect_Global.Bat_Under_Temp.Protect_EN            = TRUE;
    Protect_Global.CH_Over_Ah.Protect_EN                = FALSE;
    Protect_Global.CH_Over_Cur.Protect_EN               = FALSE;
    Protect_Global.CH_Over_Vol_Bat.Protect_EN           = TRUE;
    Protect_Global.CH_Over_Vol_Out.Protect_EN           = FALSE;

    Protect_Global.DIS_Over_Ah.Protect_EN               = FALSE;
    Protect_Global.DIS_Over_Cur.Protect_EN              = FALSE;
    Protect_Global.DIS_Under_Vol_Bat.Protect_EN         = TRUE;
    Protect_Global.DIS_Under_Vol_Out.Protect_EN         = FALSE;
    Protect_Global.Total_Over_Ah.Protect_EN             = FALSE;
    Protect_Global.Cur_Diff_Over.Protect_EN             = FALSE;

    Protect_Global.Vol_Diff_Over.Protect_EN             = FALSE;

    Protect_Global.Res_Contact_Over.Protect_EN          = FALSE;
    Protect_Global.Res_Loop_Over.Protect_EN             = FALSE;
    Protect_Global.Out_Cur_Over.Protect_EN              = TRUE;

    Protect_Global.Cur_Fluct_Over.Protect_EN			= FALSE;
    Protect_Global.Vol_Fluct_Over.Protect_EN			= TRUE;

    //单工步相关保护禁止与使能
    //WorkStep_Data.Prot_Step_CH_Over_Cur.Protect_EN      = FALSE;
    //WorkStep_Data.Prot_Step_CH_Over_Ah.Protect_EN       = FALSE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Protect_EN  = TRUE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Protect_EN  = TRUE;

    //WorkStep_Data.Prot_Step_DIS_Over_Cur.Protect_EN     = FALSE;
    //WorkStep_Data.Prot_Step_DIS_Over_Ah.Protect_EN      = FALSE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Protect_EN= TRUE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Protect_EN= TRUE;

    //WorkStep_Data.Prot_Step_Bat_Over_Temp.Protect_EN    = TRUE;

    //WorkStep_Data.Prot_Step_Vol_Delta_Pos.Protect_EN    = FALSE;
    //WorkStep_Data.Prot_Step_Vol_Delta_Neg.Protect_EN    = FALSE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Pos.Protect_EN    = FALSE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Neg.Protect_EN    = FALSE;

    //WorkStep_Data.Prot_Step_Vol_Time.Protect_EN         = FALSE;
    //WorkStep_Data.Prot_Step_Vol_Change_Time.Protect_EN  = FALSE;
    //WorkStep_Data.Prot_Step_BatShort.Protect_EN         = FALSE;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Protect_Configure_CV_PWM(void)
{
    //全局相关保护禁止与使能
    Protect_Global.Bat_Over_Temp.Protect_EN             = FALSE;
    Protect_Global.Bat_Temp_Alarm.Protect_EN            = FALSE;
    Protect_Global.Bat_Reverse_Sense.Protect_EN         = FALSE;
    Protect_Global.Bat_Reverse_Out.Protect_EN         	= FALSE;
    Protect_Global.Bat_Under_Temp.Protect_EN            = FALSE;
    Protect_Global.CH_Over_Ah.Protect_EN                = FALSE;
    Protect_Global.CH_Over_Cur.Protect_EN               = FALSE;
    Protect_Global.CH_Over_Vol_Bat.Protect_EN           = TRUE;
    Protect_Global.CH_Over_Vol_Out.Protect_EN           = FALSE;

    Protect_Global.DIS_Over_Ah.Protect_EN               = FALSE;
    Protect_Global.DIS_Over_Cur.Protect_EN              = FALSE;
    Protect_Global.DIS_Under_Vol_Bat.Protect_EN         = FALSE;
    Protect_Global.DIS_Under_Vol_Out.Protect_EN         = FALSE;

    Protect_Global.Cur_Diff_Over.Protect_EN             = FALSE;
    Protect_Global.Cur_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Cur;

    Protect_Global.Vol_Diff_Over.Protect_EN             = TRUE;
    Protect_Global.Vol_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Vol;

    Protect_Global.Res_Contact_Over.Protect_EN          = FALSE;
    Protect_Global.Res_Loop_Over.Protect_EN             = FALSE;
    Protect_Global.Out_Cur_Over.Protect_EN              = TRUE;

    Protect_Global.Cur_Fluct_Over.Protect_EN			= FALSE;
    Protect_Global.Vol_Fluct_Over.Protect_EN			= FALSE;
    Protect_Global.Total_Over_Ah.Protect_EN             = FALSE;
    //单工步相关保护禁止与使能
    //WorkStep_Data.Prot_Step_CH_Over_Cur.Protect_EN      = FALSE;
    //WorkStep_Data.Prot_Step_CH_Over_Ah.Protect_EN       = FALSE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Protect_EN  = FALSE;
    //WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Protect_EN  = FALSE;

    //WorkStep_Data.Prot_Step_DIS_Over_Cur.Protect_EN     = FALSE;
    //WorkStep_Data.Prot_Step_DIS_Over_Ah.Protect_EN      = FALSE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Protect_EN= FALSE;
    //WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Protect_EN= FALSE;

    //WorkStep_Data.Prot_Step_Bat_Over_Temp.Protect_EN    = FALSE;

    //WorkStep_Data.Prot_Step_Vol_Delta_Pos.Protect_EN    = FALSE;
    //WorkStep_Data.Prot_Step_Vol_Delta_Neg.Protect_EN    = FALSE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Pos.Protect_EN    = FALSE;
    //WorkStep_Data.Prot_Step_Cur_Delta_Neg.Protect_EN    = FALSE;

    //WorkStep_Data.Prot_Step_Vol_Time.Protect_EN         = FALSE;
    //WorkStep_Data.Prot_Step_Vol_Change_Time.Protect_EN  = FALSE;
    //WorkStep_Data.Prot_Step_BatShort.Protect_EN         = FALSE;

}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
/*void Protect_Configure_DCV(void)
{
    //全局相关保护禁止与使能
    Protect_Global.Bat_Over_Temp.Protect_EN             = TRUE;
    Protect_Global.Bat_Temp_Alarm.Protect_EN            = FASLE;
    Protect_Global.Bat_Reverse_Sense.Protect_EN         = TRUE;
    Protect_Global.Bat_Reverse_Out.Protect_EN         	= TRUE;
    Protect_Global.Bat_Under_Temp.Protect_EN            = TRUE;
    Protect_Global.CH_Over_Ah.Protect_EN                = FALSE;
    Protect_Global.CH_Over_Cur.Protect_EN               = FALSE;
    Protect_Global.CH_Over_Vol_Bat.Protect_EN           = TRUE;
    Protect_Global.CH_Over_Vol_Out.Protect_EN           = FALSE;

    Protect_Global.DIS_Over_Ah.Protect_EN               = TRUE;
    Protect_Global.DIS_Over_Cur.Protect_EN              = TRUE;
    Protect_Global.DIS_Under_Vol_Bat.Protect_EN         = TRUE;
    Protect_Global.DIS_Under_Vol_Out.Protect_EN         = TRUE;

    Protect_Global.Cur_Diff_Over.Protect_EN             = TRUE;
    Protect_Global.Cur_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Cur;

    Protect_Global.Vol_Diff_Over.Protect_EN             = FALSE;
    Protect_Global.Vol_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Vol;

    Protect_Global.Res_Contact_Over.Protect_EN          = TRUE;
    Protect_Global.Res_Loop_Over.Protect_EN             = TRUE;
    Protect_Global.Out_Cur_Over.Protect_EN              = TRUE;

    Protect_Global.Cur_Fluct_Over.Protect_EN			= TRUE;
    Protect_Global.Vol_Fluct_Over.Protect_EN			= TRUE;

    //单工步相关保护禁止与使能
    WorkStep_Data.Prot_Step_CH_Over_Cur.Protect_EN      = FALSE;
    WorkStep_Data.Prot_Step_CH_Over_Ah.Protect_EN       = FALSE;
    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Protect_EN  = FALSE;

    WorkStep_Data.Prot_Step_DIS_Over_Cur.Protect_EN     = TRUE;
    WorkStep_Data.Prot_Step_DIS_Over_Ah.Protect_EN      = TRUE;
    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Protect_EN= TRUE;

    WorkStep_Data.Prot_Step_Bat_Over_Temp.Protect_EN    = TRUE;

    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Protect_EN    = TRUE;
    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Protect_EN    = TRUE;
    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Protect_EN    = FALSE;
    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Protect_EN    = FALSE;

    WorkStep_Data.Prot_Step_Vol_Time.Protect_EN         = FALSE;
    WorkStep_Data.Prot_Step_Vol_Change_Time.Protect_EN  = FALSE;
    WorkStep_Data.Prot_Step_BatShort.Protect_EN         = FALSE;
}*/

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Fault_Code_Global_TX(unsigned char Code)
{
    INT16U  uwTcpCmdCrcCheck = 0;
    INT8U   ubTcpCmdCrcTemp0 = 0;
    INT8U   ubTcpCmdCrcTemp1 = 0;
    INT16U  uwTcpipIndex=0;

    Fault_Code.Code=Code;

    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = 0x06;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = cSetProtectTrigger;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = Code;
    uwTcpipIndex = uwTcpipIndex+1;
    //校验位HL
    uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[0],uwTcpipIndex);
    ubTcpCmdCrcTemp0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
    ubTcpCmdCrcTemp1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = ubTcpCmdCrcTemp0;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = ubTcpCmdCrcTemp1;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = cEnd;
    uwTcpipIndex = uwTcpipIndex+1;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Num_To_TX=uwTcpipIndex;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Result_TX=0;
    TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Req_TX=1;

}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Fault_Code_Step_TX(unsigned char Code)
{
     INT16U  uwTcpCmdCrcCheck = 0;
     INT8U   ubTcpCmdCrcTemp0 = 0;
     INT8U   ubTcpCmdCrcTemp1 = 0;
     INT16U  uwTcpipIndex=0;

     Fault_Code.Code=Code+128;

     TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = 0x06;
     uwTcpipIndex = uwTcpipIndex+1;
     TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = cSSProtectTrigger;
     uwTcpipIndex = uwTcpipIndex+1;
     TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = Code;
     uwTcpipIndex = uwTcpipIndex+1;
     //校验位HL
     uwTcpCmdCrcCheck = sCRC16N(&TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[0],uwTcpipIndex);
     ubTcpCmdCrcTemp0 = (INT8U)((uwTcpCmdCrcCheck&0xFF00)>>8);
     ubTcpCmdCrcTemp1 = (INT8U)(uwTcpCmdCrcCheck&0x00FF);
     TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = ubTcpCmdCrcTemp0;
     uwTcpipIndex = uwTcpipIndex+1;
     TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = ubTcpCmdCrcTemp1;
     uwTcpipIndex = uwTcpipIndex+1;
     TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Data[uwTcpipIndex] = cEnd;
     uwTcpipIndex = uwTcpipIndex+1;
     TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Num_To_TX=uwTcpipIndex;
     TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Result_TX=0;
     TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Req_TX=1;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Upload_Fault_Code(void)
{
	if(DCDC_Service.Mode_Parall==PARALL_SLAVER)
	{	return;	}

	if(TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus==FALSE)
	{
		return;
	}

	if(TCP_Socket[SERV_TCP_WORK].Data_TX[CH_TX_ALARM].Req_TX!=FALSE)
    {	return;	}

    if(((Fault_Code.Err_Global&cProtectCVFault)!=0)&&((Fault_Code.Err_Global_Bak&cProtectCVFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cProtectCVFault;
        Fault_Code_Global_TX(Code_ProtectCV);
    }
    else  if(((Fault_Code.Err_Global&cProtectCCFault)!=0)&&((Fault_Code.Err_Global_Bak&cProtectCCFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cProtectCCFault;
        Fault_Code_Global_TX(Code_ProtectCC);
    }
    else  if(((Fault_Code.Err_Global&cProtectDCVFault)!=0)&&((Fault_Code.Err_Global_Bak&cProtectDCVFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cProtectDCVFault;
        Fault_Code_Global_TX(Code_ProtectDCV);
    }
    else  if(((Fault_Code.Err_Global&cProtectDCCFault)!=0)&&((Fault_Code.Err_Global_Bak&cProtectDCCFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cProtectDCCFault;
        Fault_Code_Global_TX(Code_ProtectDCC);
    }
    else  if(((Fault_Code.Err_Global&cChargeOverCAHFault)!=0)&&((Fault_Code.Err_Global_Bak&cChargeOverCAHFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cChargeOverCAHFault;
        Fault_Code_Global_TX(Code_ProtectCAH);
    }
    else  if(((Fault_Code.Err_Global&cOPCurrentLimitFault)!=0)&&((Fault_Code.Err_Global_Bak&cOPCurrentLimitFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cOPCurrentLimitFault;
        Fault_Code_Global_TX(Code_ProtectOPC);
    }
    else  if(((Fault_Code.Err_Global&cBatPNFault)!=0)&&((Fault_Code.Err_Global_Bak&cBatPNFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cBatPNFault;
        Fault_Code_Global_TX(Code_ProtectBatReverse);
    }
    else  if(((Fault_Code.Err_Global&cOPShortFault)!=0)&&((Fault_Code.Err_Global_Bak&cOPShortFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cOPShortFault;
        Fault_Code_Global_TX(Code_ProtectOPShort);
    }
    else  if(((Fault_Code.Err_Global&cBatShortFault)!=0)&&((Fault_Code.Err_Global_Bak&cBatShortFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cBatShortFault;
        Fault_Code_Global_TX(Code_ProtectBatShort);
    }
    else  if(((Fault_Code.Err_Global&cFanFault)!=0)&&((Fault_Code.Err_Global_Bak&cFanFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cFanFault;
        Fault_Code_Global_TX(Code_ProtectFanErr);
    }
    else  if(((Fault_Code.Err_Global&cBusVoltHighFault)!=0)&&((Fault_Code.Err_Global_Bak&cBusVoltHighFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cBusVoltHighFault;
        Fault_Code_Global_TX(Code_ProtectBusHigh);
    }
    else  if(((Fault_Code.Err_Global&cBusLow)!=0)&&((Fault_Code.Err_Global_Bak&cBusLow)==0))
    {
        Fault_Code.Err_Global_Bak |= cBusLow;
        Fault_Code_Global_TX(Code_ProtectBusLow);
    }
    else  if(((Fault_Code.Err_Global&cBatVoltFlucFault)!=0)&&((Fault_Code.Err_Global_Bak&cBatVoltFlucFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cBatVoltFlucFault;
        Fault_Code_Global_TX(Code_ProtectBatFluc);
    }
    else  if(((Fault_Code.Err_Global&cBatVoltMutatFault)!=0)&&((Fault_Code.Err_Global_Bak&cBatVoltMutatFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cBatVoltMutatFault;
        Fault_Code_Global_TX(Code_ProtectBatMutat);
    }
    else  if(((Fault_Code.Err_Global&cOPCurrentFlucFault)!=0)&&((Fault_Code.Err_Global_Bak&cOPCurrentFlucFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cOPCurrentFlucFault;
        Fault_Code_Global_TX(Code_ProtectOPCFluc);
    }
    else  if(((Fault_Code.Err_Global&cOPCurrentMutatFault)!=0)&&((Fault_Code.Err_Global_Bak&cOPCurrentMutatFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cOPCurrentMutatFault;
        Fault_Code_Global_TX(Code_ProtectOPCMutat);
    }
    else  if(((Fault_Code.Err_Global&cTerminalConnectFault)!=0)&&((Fault_Code.Err_Global_Bak&cTerminalConnectFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cTerminalConnectFault;
        Fault_Code_Global_TX(Code_ProtectTerminal);
    }
    else  if(((Fault_Code.Alarm&cBatTempHighFault)!=0)&&((Fault_Code.Alarm_Bak&cBatTempHighFault)==0))
    {
        Fault_Code.Alarm_Bak |= cBatTempHighFault;
        Fault_Code_Global_TX(Code_ProtectT);
    }
    else  if(((Fault_Code.Alarm&cBatTempAlarmFault)!=0)&&((Fault_Code.Alarm_Bak&cBatTempAlarmFault)==0))
    {
        Fault_Code.Alarm_Bak |= cBatTempAlarmFault;
        Fault_Code_Global_TX(Code_cBatTempAlarmFault);
    }
    else  if(((Fault_Code.Alarm&cBatTempOverAlarmFault)!=0)&&((Fault_Code.Alarm_Bak&cBatTempOverAlarmFault)==0))
    {
        Fault_Code.Alarm_Bak |= cBatTempOverAlarmFault;
        Fault_Code_Global_TX(Code_cBatTempOverAlarmFault);
    }
    else  if(((Fault_Code.Alarm&cBatTempLowFault)!=0)&&((Fault_Code.Alarm_Bak&cBatTempLowFault)==0))
    {
        Fault_Code.Alarm_Bak |= cBatTempLowFault;
        Fault_Code_Global_TX(Code_ProtectLowT);
    }
    else  if(((Fault_Code.Err_Global&cImpedanceFault)!=0)&&((Fault_Code.Err_Global_Bak&cImpedanceFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cImpedanceFault;
        Fault_Code_Global_TX(Code_ProtectImpedance);
    }
    else  if(((Fault_Code.Err_Global&cOPCOverDiffFault)!=0)&&((Fault_Code.Err_Global_Bak&cOPCOverDiffFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cOPCOverDiffFault;
        Fault_Code_Global_TX(Code_ProtectOCE);
    }
    else  if(((Fault_Code.Err_Global&cBatVOverDiffFault)!=0)&&((Fault_Code.Err_Global_Bak&cBatVOverDiffFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cBatVOverDiffFault;
        Fault_Code_Global_TX(Code_ProtectOVE);
    }
    else  if(((Fault_Code.Err_Global&cLoopImpedanceFault)!=0)&&((Fault_Code.Err_Global_Bak&cLoopImpedanceFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cLoopImpedanceFault;
        Fault_Code_Global_TX(Code_ProtectLoopImpedance);
    }
    else  if(((Fault_Code.Err_Global&cDisChargeOverAHFault)!=0)&&((Fault_Code.Err_Global_Bak&cDisChargeOverAHFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cDisChargeOverAHFault;
        Fault_Code_Global_TX(Code_ProtectDAH);
    }
    else  if(((Fault_Code.Err_Global&cPowerVoltFault)!=0)&&((Fault_Code.Err_Global_Bak&cPowerVoltFault)==0))
    {
        Fault_Code.Err_Global_Bak |= cPowerVoltFault;
        Fault_Code_Global_TX(Code_ProtectPowerVolt);
    }
    else  if(((Fault_Code.Err_Global&cModuleSinkHigh)!=0)&&((Fault_Code.Err_Global_Bak&cModuleSinkHigh)==0))
    {
        Fault_Code.Err_Global_Bak |= cModuleSinkHigh;
        Fault_Code_Global_TX(Code_ProtectSinkHigh);
    }
    else  if(((Fault_Code.Err_Global&cCaliDataError)!=0)&&((Fault_Code.Err_Global_Bak&cCaliDataError)==0))
    {
        Fault_Code.Err_Global_Bak |= cCaliDataError;
        Fault_Code_Global_TX(Code_cCaliDataError);
    }
    else  if(((Fault_Code.Err_Global&cStepDataError)!=0)&&((Fault_Code.Err_Global_Bak&cStepDataError)==0))
    {
        Fault_Code.Err_Global_Bak |= cStepDataError;
        Fault_Code_Global_TX(Code_cStepDataError);
    }
    else  if(((Fault_Code.Err_Global&cHardwareError)!=0)&&((Fault_Code.Err_Global_Bak&cHardwareError)==0))
    {
        Fault_Code.Err_Global_Bak |= cHardwareError;
        Fault_Code_Global_TX(Code_cHardwareError);
    }
    else  if(((Fault_Code.Err_Global&cOtherError)!=0)&&((Fault_Code.Err_Global_Bak&cOtherError)==0))
    {
        Fault_Code.Err_Global_Bak |= cOtherError;
        Fault_Code_Global_TX(Code_cOtherError);
    }
    else if(((Fault_Code.Err_Global&cTotalOverAHFault)!=0)&&((Fault_Code.Err_Global_Bak&cTotalOverAHFault)==0))
    {
           Fault_Code.Err_Global_Bak |= cTotalOverAHFault;
           Fault_Code_Global_TX(Code_ProtectTotalAH);
    }
    else if(((Fault_Code.Alarm&cBatTempUpperLimit)!=0)&&((Fault_Code.Alarm_Bak&cBatTempUpperLimit)==0))
    {
        Fault_Code.Alarm_Bak |= cBatTempUpperLimit;
        Fault_Code_Step_TX(cSSProtectBatTempUpperLimit);
    }
    else if(((Fault_Code.Alarm&cBatTempDownUpperLimit)!=0)&&((Fault_Code.Alarm_Bak&cBatTempDownUpperLimit)==0))
    {
        Fault_Code.Alarm_Bak |= cBatTempDownUpperLimit;
        Fault_Code_Step_TX(cSSProtectBatTempDownUpperLimit);
    }
    else if(((Fault_Code.Err_Step&cCapacityUpperLimit)!=0)&&((Fault_Code.Err_Step_Bak&cCapacityUpperLimit)==0))
    {
        Fault_Code.Err_Step_Bak |= cCapacityUpperLimit;
        Fault_Code_Step_TX(cSSProtectCapUpperLimit);
    }
    else if(((Fault_Code.Err_Step&cCapacityLowerLimit)!=0)&&((Fault_Code.Err_Step_Bak&cCapacityLowerLimit)==0))
    {
        Fault_Code.Err_Step_Bak |= cCapacityLowerLimit;
        Fault_Code_Step_TX(cSSProtectCapLowerLimit);
    }
    else if(((Fault_Code.Err_Step&cBatVoltUpperLimit)!=0)&&((Fault_Code.Err_Step_Bak&cBatVoltUpperLimit)==0))
    {
        Fault_Code.Err_Step_Bak |= cBatVoltUpperLimit;
        Fault_Code_Step_TX(cSSProtectBatVoltUpperLimit);
    }
    else if(((Fault_Code.Err_Step&cBatVoltLowerLimit)!=0)&&((Fault_Code.Err_Step_Bak&cBatVoltLowerLimit)==0))
    {
        Fault_Code.Err_Step_Bak |= cBatVoltLowerLimit;
        Fault_Code_Step_TX(cSSProtectBatVoltLowerLimit);
    }
    else if(((Fault_Code.Err_Step&cCurrUpperLimit)!=0)&&((Fault_Code.Err_Step_Bak&cCurrUpperLimit)==0))
    {
        Fault_Code.Err_Step_Bak |= cCurrUpperLimit;
        Fault_Code_Step_TX(cSSProtectCurrUpperLimit);
    }
    else if(((Fault_Code.Err_Step&cCurrLowerLimit)!=0)&&((Fault_Code.Err_Step_Bak&cCurrLowerLimit)==0))
    {
        Fault_Code.Err_Step_Bak |= cCurrLowerLimit;
        Fault_Code_Step_TX(cSSProtectCurrLowerLimit);
    }
    else if(((Fault_Code.Err_Step&cOPCPositiveJump)!=0)&&((Fault_Code.Err_Step_Bak&cOPCPositiveJump)==0))
    {
        Fault_Code.Err_Step_Bak |= cOPCPositiveJump;
        Fault_Code_Step_TX(cSSProtectOPCPositiveJump);
    }
    else if(((Fault_Code.Err_Step&cOPCNegativeJump)!=0)&&((Fault_Code.Err_Step_Bak&cOPCNegativeJump)==0))
    {
        Fault_Code.Err_Step_Bak |= cOPCNegativeJump;
        Fault_Code_Step_TX(cSSProtectOPCNegativeJump);
    }
    else if(((Fault_Code.Err_Step&cBatVoltPositiveJump)!=0)&&((Fault_Code.Err_Step_Bak&cBatVoltPositiveJump)==0))
    {
        Fault_Code.Err_Step_Bak |= cBatVoltPositiveJump;
        Fault_Code_Step_TX(cSSProtectBatVoltPositiveJump);
    }
    else if(((Fault_Code.Err_Step&cBatVoltNegativeJump)!=0)&&((Fault_Code.Err_Step_Bak&cBatVoltNegativeJump)==0))
    {
        Fault_Code.Err_Step_Bak |= cBatVoltNegativeJump;
        Fault_Code_Step_TX(cSSProtectBatVoltNegativeJump);
    }
    else if(((Fault_Code.Err_Step&cBatErrorVol_Start)!=0)&&((Fault_Code.Err_Step_Bak&cBatErrorVol_Start)==0))
    {
        Fault_Code.Err_Step_Bak |= cBatErrorVol_Start;
        Fault_Code_Global_TX(Code_BatErrorVol_Start);
    }
    else if(((Fault_Code.Err_Step&cBatErrorContacCheck)!=0)&&((Fault_Code.Err_Step_Bak&cBatErrorContacCheck)==0))
    {
        Fault_Code.Err_Step_Bak |= cBatErrorContacCheck;
        Fault_Code_Global_TX(Code_BatErrorContacCheck);
    }
    else if(((Fault_Code.Err_Step&cStepEnd2Fault)!=0)&&((Fault_Code.Err_Step_Bak&cStepEnd2Fault)==0))
    {
        Fault_Code.Err_Step_Bak |= cStepEnd2Fault;
        //Fault_Code_Global_TX(Code_StepEnd2Abnormal);
        Fault_Code_Step_TX(Code_StepEnd2Abnormal );
    }
    else if(((Fault_Code.Err_Step&cStepEnd3Fault)!=0)&&((Fault_Code.Err_Step_Bak&cStepEnd3Fault)==0))
    {
        Fault_Code.Err_Step_Bak |= cStepEnd3Fault;
       // Fault_Code_Global_TX(Code_StepEnd3Abnormal);
        Fault_Code_Step_TX(Code_StepEnd3Abnormal );
    }
    else if(((Fault_Code.Err_Step&cVol_Time_Error)!=0)&&((Fault_Code.Err_Step_Bak&cVol_Time_Error)==0))
    {
        Fault_Code.Err_Step_Bak |= cVol_Time_Error;
        Fault_Code_Step_TX(cSSProtectVol_Time_Error );
    }
    else if(((Fault_Code.Err_Step&cVol_Change_Time_Error)!=0)&&((Fault_Code.Err_Step_Bak&cVol_Change_Time_Error)==0))
    {
        Fault_Code.Err_Step_Bak |= cVol_Change_Time_Error;
        Fault_Code_Step_TX(cSSProtectVol_Change_Time_Error);
    }
    else if(((Fault_Code.Err_Step&cBatShort)!=0)&&((Fault_Code.Err_Step_Bak&cBatShort)==0))
    {
        Fault_Code.Err_Step_Bak |= cBatShort;
        Fault_Code_Step_TX(cSSProtectBatShort);
    }
    else if(((Fault_Code.Err_Step&cVol_Diff)!=0)&&((Fault_Code.Err_Step_Bak&cVol_Diff)==0))
    {
        Fault_Code.Err_Step_Bak |= cVol_Diff;
        Fault_Code_Step_TX(cSSProtectVol_Diff);
    }
    else if(((Fault_Code.Err_Step&cCur_Diff)!=0)&&((Fault_Code.Err_Step_Bak&cCur_Diff)==0))
    {
        Fault_Code.Err_Step_Bak |= cCur_Diff;
        Fault_Code_Step_TX(cSSProtectCur_Diff);
    }
    else if(((Fault_Code.Err_Step&cVoltIntervalPos)!=0)&&((Fault_Code.Err_Step_Bak&cVoltIntervalPos)==0))
    {
        Fault_Code.Err_Step_Bak |= cVoltIntervalPos;
        Fault_Code_Step_TX(cSSProtectVol_Interval_Pos);
    }
    else if(((Fault_Code.Err_Step&cVoltIntervalNeg)!=0)&&((Fault_Code.Err_Step_Bak&cVoltIntervalNeg)==0))
    {
        Fault_Code.Err_Step_Bak |= cVoltIntervalNeg;
        Fault_Code_Step_TX(cSSProtectVol_Interval_Neg);
    }

}



//File end

