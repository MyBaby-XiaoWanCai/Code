/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: WorkStep_Record.c
// Created on: 2020-4-11
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义
struct_Past_Data        Past_Data[NUM_PAST_DATA];
struct_Past_Data_Manage Past_Data_Manage;

//函数声明


//函数定义
void Function_Record(void)
{
    if(Past_Data_Manage.Req_Save==TRUE)
    {
        DINT;
        Past_Data_Manage.Req_Save=FALSE;
        EINT;
        SaveDataToRecord(Past_Data_Manage.Ptr_In->Ptr_Image);

        //上报周期大于1S或时间差记录禁止，按照1S周期做Delta检查
        if((WorkStep_Data.Record_Time.Delta_Time>1000)||(WorkStep_Data.Record_Time.IsRecord == FALSE))
        {
            CopyImage(Past_Data_Manage.Ptr_In->Ptr_Image,&Image_Protect.Record);
            Protect_Check_Delta();
        }

        if(((Fault_Code.Err_Global&(cBatVoltFlucFault|cOPCurrentFlucFault))==0)&&\
           ((Fault_Code.Err_Step  &(cBatVoltPositiveJump|cBatVoltNegativeJump|cOPCPositiveJump|cOPCNegativeJump))==0))
        {
            Past_Data_Manage.Ptr_In=Past_Data_Manage.Ptr_In->Ptr_Next;
            if(Past_Data_Manage.Ptr_Out==Past_Data_Manage.Ptr_In)
            {
                Past_Data_Manage.Ptr_Out=Past_Data_Manage.Ptr_Out->Ptr_Next;
            }
        }
    }

    if(WorkStep_Data.Record_Time.IsRecord == TRUE)
    {
        SaveDataToRecord(&Image_UpData.Record);
        CopyImage(&Image_UpData.Record,&Image_Protect.Record);
        WorkStep_Data.Record_Time.IsRecord = FALSE;
        WorkStep_Data.Record_Vol.Value_Last=Image_UpData.Record.wBatSenseAVoltNew_UP;
        WorkStep_Data.Record_Cur.Value_Last=Image_UpData.Record.dwOutPutCurrentNew_UP;

        /*if(!(((DCDC_Service.Now_Mode==DCDC_CCtoCV)&&(DCDC_Service.Flag_CV_Step ==TRUE))||((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==TRUE))))
        {
            Image_UpData.Record.wBatSenseAVoltNew_UP = Get_Middle_Value();
            Image_Protect.Record.wBatSenseAVoltNew_UP = Image_UpData.Record.wBatSenseAVoltNew_UP;
            // Image_UpData.Record.wBatSenseAVoltNew_UP = Voltage_Muta_Check(Image_UpData.Record.wBatSenseAVoltNew_UP);
        }
        Updata_Mutation_Check();
        Middle_Cnt = 0;*/

        //上报周期小于等于1S，按照上报周期做delta检查
        if(WorkStep_Data.Record_Time.Delta_Time<=1000)
        {
            Protect_Check_Delta();
        }

        //上报数据做单次检查
        Protect_Check_Once();

        //请求上抛数据
        Image_UpData.Req_TX=TRUE;

        Past_Data_Manage.Req_Reset=TRUE;
    }

    if(WorkStep_Service.State==STEP_RUN)
    {
        if(WorkStep_Data.Record_Vol.EN==cON)
        {
            WorkStep_Data.Record_Vol.Value_Now=Data_Sum.Vol_Sense;
            WorkStep_Data.Record_Vol.Delta_Now=WorkStep_Data.Record_Vol.Value_Now-WorkStep_Data.Record_Vol.Value_Last;
            if(WorkStep_Data.Record_Vol.Delta_Now<0)
            {
                WorkStep_Data.Record_Vol.Delta_Now=-WorkStep_Data.Record_Vol.Delta_Now;
            }

            if(WorkStep_Data.Record_Vol.Delta_Now>=WorkStep_Data.Record_Vol.Delta_Ref_Step)
            {
                WorkStep_Data.Record_Vol.Value_Last=WorkStep_Data.Record_Vol.Value_Now;
                WorkStep_Data.Record_Vol.IsRecord = TRUE;

                SaveDataToRecord(&Image_UpData.Record);

                //上报数据做单次检查
                Protect_Check_Once();

                //请求上抛数据
                Image_UpData.Req_TX=TRUE;

                Past_Data_Manage.Req_Reset=TRUE;
            }
        }

        if(WorkStep_Data.Record_Cur.EN==cON)
        {
            WorkStep_Data.Record_Cur.Value_Now=Parall_Master.Now_Cur_Total;
            WorkStep_Data.Record_Cur.Delta_Now=WorkStep_Data.Record_Cur.Value_Now-WorkStep_Data.Record_Cur.Value_Last;
            if(WorkStep_Data.Record_Cur.Delta_Now<0)
            {
                WorkStep_Data.Record_Cur.Delta_Now=-WorkStep_Data.Record_Cur.Delta_Now;
            }

            if(WorkStep_Data.Record_Cur.Delta_Now>=WorkStep_Data.Record_Cur.Delta_Ref_Step)
            {
                WorkStep_Data.Record_Cur.Value_Last=WorkStep_Data.Record_Cur.Value_Now;
                WorkStep_Data.Record_Cur.IsRecord = TRUE;

                SaveDataToRecord(&Image_UpData.Record);

                //上报数据做单次检查
                Protect_Check_Once();

                //请求上抛数据
                Image_UpData.Req_TX=TRUE;

                Past_Data_Manage.Req_Reset=TRUE;
            }
        }
    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Past_Data_Manage_Init(void)
{
    Uint16 Cnt=0;
    Uint32 Add=0;
    for(Cnt=0;Cnt<NUM_PAST_DATA;Cnt++)
    {
        Add=sizeof(struct_Past_Data);
        Add=Add*Cnt+(Uint32)&Past_Data[0];
        Past_Data[Cnt].Ptr_Image=(struct_Image *)Add;
        SaveDataToRecord(Past_Data[Cnt].Ptr_Image);
        Add=sizeof(struct_Past_Data);
        Add=Add*(Cnt+1)+(Uint32)&Past_Data[0];
        Past_Data[Cnt].Ptr_Next=(struct_Past_Data *)Add;
    }

    Add=(Uint32)&Past_Data[0];
    Past_Data[NUM_PAST_DATA-1].Ptr_Next=(struct_Past_Data *)Add;
    Past_Data_Manage.Ptr_In=(struct_Past_Data *)Add;

    Add=sizeof(struct_Past_Data);
    Add=Add*(NUM_PAST_DATA-1)+(Uint32)&Past_Data[0];
    Past_Data_Manage.Ptr_Out=(struct_Past_Data *)Add;

    Past_Data_Manage.Period_Save    = 1000;
    Past_Data_Manage.Cnt_Save       = 0;
    Past_Data_Manage.Req_Save       = FALSE;
    Past_Data_Manage.Req_TX         = FALSE;
    Past_Data_Manage.EN             = FALSE;
    Past_Data_Manage.Req_Reset      = FALSE;
}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Data_Up(void)
{
	if(DCDC_Service.Mode_Parall==PARALL_SLAVER)
	{
		Image_Protect.Req_TX=FALSE;
        Image_StepEnd.Req_TX=FALSE;
        Image_UpData.Req_TX=FALSE;
        Past_Data_Manage.Req_TX=FALSE;
		return;
	}

    //数据上报
    if(Past_Data_Manage.Req_TX==TRUE)
    {
        if(Past_Data_Manage.Ptr_Out==Past_Data_Manage.Ptr_In)
        {
            Past_Data_Manage.Req_TX=FALSE;
        }
        else
        {
            sReplyRecondDataCommand(Past_Data_Manage.Ptr_Out->Ptr_Image,CH_TX_UPDATA);
            Past_Data_Manage.Ptr_Out=Past_Data_Manage.Ptr_Out->Ptr_Next;
        }
    }
    else if(Image_Protect.Req_TX==TRUE)
    {
    	sReplyRecondDataCommand((struct_Image *)&Image_Protect.Record,CH_TX_UPDATA);
        Image_Protect.Req_TX=FALSE;
        Image_StepEnd.Req_TX=FALSE;
        Image_UpData.Req_TX=FALSE;
        CopyImage((struct_Image *)&Image_Protect.Record,(struct_Image *)&Image_Protect_Bak);
    }
    else if(Image_StepEnd.Req_TX==TRUE)
    {
    	if((Image_StepEnd.Record.ubWorkStepName==cStepStatic)||(Image_StepEnd.Record.ubWorkStepName==cStepNegPressure))
    	{
    		Image_StepEnd.Record.dwOutPutCurrentNew_UP=0;
    	}

    	sReplyRecondDataCommand((struct_Image *)&Image_StepEnd.Record,CH_TX_UPDATA);
        Image_StepEnd.Req_TX=FALSE;
        Image_UpData.Req_TX=FALSE;
    }
    else if(Image_UpData.Req_TX==TRUE)
    {
    	if((Image_UpData.Record.ubWorkStepName==cStepStatic)||(Image_UpData.Record.ubWorkStepName==cStepNegPressure))
    	{
    		Image_UpData.Record.dwOutPutCurrentNew_UP=0;
    	}

    	sReplyRecondDataCommand((struct_Image *)&Image_UpData.Record,CH_TX_UPDATA);
        Image_UpData.Req_TX=FALSE;
    }

}


//File end
