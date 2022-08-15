/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: WorkStep_Flow.c
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义
long long DateTime=0x0000016EF39C4C68;
struct_WorkStep_Data        WorkStep_Data;
struct_WorkStep_Service     WorkStep_Service;
struct_WorkStep_PowerFail   WorkStep_PowerFail;
struct_WorkStep_Cycle		WorkStep_Cycle;

struct_Filter_Elimin        Elimin_Vol_Sense;
struct_Filter_Elimin        Elimin_Vol_Out;
struct_Filter_Workstep      Filter_Workstep_Last; 

struct_Filter_Median        Median_Vol_Sense;
struct_Filter_Median        Median_Vol_Out;
struct_Filter_Median        Median_Cur_Total;

struct_Rate_Cur_Compute     Rate_Cur_Compute;

struct_Image_TX Image_Protect;       //故障保护时保存数据镜像
struct_Image_TX Image_StepEnd;       //工步结束数据镜像
struct_Image_TX Image_UpData;        //需上报数据记录镜像

int32 Volt_1S_Array[250];
int32 Cur_1S_Array[250];
Uint16 Middle_Cnt = 0;

//函数声明


//函数定义
//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void WorkStep_Flow(void)
{
    //从机状态不需要处理工步管理
    if(DCDC_Service.Mode_Parall==PARALL_SLAVER)
    {return;}

    switch(WorkStep_Service.State)
    {
        case STEP_STANDBY:
        {
            WorkStep_Service.Step_State=cStopStatus;
            //有全局工艺保护，单工步保护，告警、硬件故障时不运行工步
            if(TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus==TRUE)
            {
                WorkStep_Service.Cnt_Delay=WorkStep_Service.Cnt_Delay+40;
            }
            else
            {
                WorkStep_Service.Cnt_Delay=0;
            }

            if(WorkStep_Service.Cnt_Delay>=1000)
            {
                WorkStep_Service.Cnt_Delay=0;
                SaveDataToRecord(&Image_UpData.Record);
                Image_UpData.Record.Res_Contact=0;
                Image_UpData.Record.Res_Loop=0;
                Image_UpData.Record.dwBatEnergy=0;
                Image_UpData.Record.dwOutPutCurrentNew_UP=0;
                Image_UpData.Record.ubChargerStatus=cStepStandby;
                Image_UpData.Record.ubWorkStepName=0;
                Image_UpData.Record.udwAH_CCdivCV=0;
                Image_UpData.Record.udwAH_DCCdivDCV=0;
                Image_UpData.Record.udwChargerCapacityNew=0;
                Image_UpData.Record.udwSuperRunTime=0;
                Image_UpData.Record.uwStepCycleStartIndex=0;
                Image_UpData.Record.uwWorkStepNum=0;
                Image_UpData.Record.uwWorkStepTotalNum=0;
                WorkStep_Data.Formula_NumberH=0;
                WorkStep_Data.Formula_NumberL=0;
                if(Interface_DCDC.Req_Aging==FALSE)
                {
                    Image_UpData.Req_TX=TRUE;
                }
            }

            if((Fault_Code.Err_Global!=0)||(Fault_Code.Err_Step!=0)||(Fault_Code.Alarm!=0)||(DCDC_Service.Fault_Module!=0))
            {return;}

            if(WorkStep_Service.Req_CMD==WORKSTEP_RUN)
            {

                WorkStep_Service.Cnt_Delay=0;
                SaveDataToRecord(&Image_UpData.Record);
                Image_UpData.Record.Res_Contact=0;
                Image_UpData.Record.Res_Loop=0;
                Image_UpData.Record.dwBatEnergy=0;
                Image_UpData.Record.dwOutPutCurrentNew_UP=0;
                Image_UpData.Record.ubChargerStatus=cStepStart;
                Image_UpData.Record.ubWorkStepName=0;
                Image_UpData.Record.udwAH_CCdivCV=0;
                Image_UpData.Record.udwAH_DCCdivDCV=0;
                Image_UpData.Record.udwChargerCapacityNew=0;
                Image_UpData.Record.udwSuperRunTime=0;
                Image_UpData.Record.uwStepCycleStartIndex=0;
                Image_UpData.Record.uwWorkStepNum=0;
                Image_UpData.Record.uwWorkStepTotalNum=0;
                Image_UpData.Req_TX=TRUE;
                WorkStep_Service.Run_TimeTotal=0;
                if((WorkStep_Service.Req_WorkStep_Index>0)&&(WorkStep_Service.Req_WorkStep_Index<=NUM_WORKSTEP_MAX))
                {
                    WorkStep_Service.Flag_FlowEnd_Once  = FALSE;
                    WorkStep_Service.Flag_FlowStop_Once = FALSE;
                    WorkStep_Data.Vol_Interval_FreshFlag= FALSE;
                    if((Interface_DCDC.Req_Aging==TRUE)||(WorkStep_Service.Req_PowerFailContine == TRUE)||(Protect_ContactCheck.Protect_EN==FALSE))
                    {
                        WorkStep_Data.Index                 = WorkStep_Service.Req_WorkStep_Index;
                        WorkStep_Service.Flag_SingleStep    = WorkStep_Service.Req_SingleStep;
                        WorkStep_Service.Req_SingleStep     = FALSE;
                        //工艺流程起始
                        Protect_Global_Cnt_Reset();//全局保护使能与计数器初始化
                        WorkStep_Service.Total_Time = 0;
                        WorkStep_Service.Total_Ah   = 0;
                        WorkStep_Service.Total_Wh   = 0;
                        WorkStep_Service.Step_Cnt   = 0;

                        WorkStep_Service.Step_Time  = 0;

                        Image_Protect.Req_TX        = FALSE;
                        Image_StepEnd.Req_TX        = FALSE;
                      //Image_UpData.Req_TX         = FALSE;

                        //循环控制部分数据初始化
                        for(WorkStep_Cycle.Now_Cycle=0;WorkStep_Cycle.Now_Cycle<NUM_CYCLE_MAX;WorkStep_Cycle.Now_Cycle++)
                        {
                            WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Cnt_Cycle      = 0;
                            WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Index_Cycle    = 0;
                            WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Num_Cycle      = 0;
                            WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Start_Cycle    = 0;
                        }
                        WorkStep_Cycle.Now_Cycle=0;

                        EEPROM_Write_Uint16(cEepromStepComplete,cNotComplete);
                        WorkStep_PowerFail.Step_State_Now   = cNotComplete;
                        WorkStep_Service.Flag_FlowEnd_Once  = FALSE;
                        WorkStep_Service.Flag_FlowStop_Once = FALSE;
                        WorkStep_Service.State              = STEP_REQ_READ;

                        Past_Data_Manage.Req_Reset          = FALSE;

                        WorkStep_Service.Step_State         = cWorkStepRunStatus;

                        WorkStep_Service.Req_CMD            = 0;
                    }
                    else
                    {
                        if((Data_Sum.Vol_Sense>=Protect_ContactCheck.Limit_Vol_L_Start)&&(Data_Sum.Vol_Sense<=Protect_ContactCheck.Limit_Vol_H_Start))
                        {
                            if((Fault_Code.Err_Global==0)&&(Fault_Code.Err_Step==0)&&(DCDC_Service.Fault_Module==0)&&(Fault_Code.Alarm==0))
                            {
                                WorkStep_Service.Step_Time      = 0;
                                WorkStep_Data.Type              = cStepCC;
                                Protect_ContactCheck.Vol_Start  = Data_Sum.Vol_Sense;
                                Interface_DCDC.Req_Mode         = DCDC_CC;
                                if(Protect_ContactCheck.Cur<=WorkStep_Service.Cur_Limited_Set)
                                {
                                    Interface_DCDC.Req_Cur_Total= Protect_ContactCheck.Cur;
                                }
                                else
                                {
                                    Interface_DCDC.Req_Cur_Total=WorkStep_Service.Cur_Limited_Set;
                                }

                               // Protect_Global.Cur_Diff_Over.Ref_Value=Interface_DCDC.Req_Cur_Total;
                                WorkStep_Data.Prot_Step_Cur_Diff_Over.Ref_Value_Diff=3000;
                                WorkStep_Data.Prot_Step_Cur_Diff_Over.Ref_Value=Interface_DCDC.Req_Cur_Total;
                                WorkStep_Data.Prot_Step_Cur_Diff_Over.Protect_EN =TRUE;
                                Protect_Step_Cnt_Reset();

                                Protect_Global_Cnt_Reset();
                                Protect_Global.Vol_Diff_Over.Cnt_Change_Mask    = 0;
                                Protect_Global.Vol_Diff_Over.Cnt_Check          = 0;

                                Protect_Global.Cur_Diff_Over.Cnt_Change_Mask    = 0;
                                Protect_Global.Cur_Diff_Over.Cnt_Check          = 0;

                                WorkStep_Data.Prot_Step_Vol_Diff_Over.Cnt_Change_Mask    = 0;
                                WorkStep_Data.Prot_Step_Vol_Diff_Over.Cnt_Check          = 0;

                                WorkStep_Data.Prot_Step_Cur_Diff_Over.Cnt_Change_Mask    = 0;
                                WorkStep_Data.Prot_Step_Cur_Diff_Over.Cnt_Check          = 0;

                                WorkStep_Data.Record_Vol.Value_Last=0;
                                WorkStep_Data.Record_Cur.Value_Last=0;

                                //Protect_Configure_CC();
                                DINT;
                                //加范围判断
                                CH_A.Cur_Scale_L.K_Now      = Cur_Scale_L_K_Check(CH_A.Cur_Scale_L.K_ROM_CH);
                                CH_A.Cur_Scale_L.B_Now      = Cur_Scale_L_B_Check(CH_A.Cur_Scale_L.B_ROM_CH);
                                CH_A.Cur_Scale_H.K_Now      = Cur_Scale_H_K_Check(CH_A.Cur_Scale_H.K_ROM_CH);
                                CH_A.Cur_Scale_H.B_Now      = Cur_Scale_H_B_Check(CH_A.Cur_Scale_H.B_ROM_CH);

                                CH_B.Cur_Scale_L.K_Now      = Cur_Scale_L_K_Check(CH_B.Cur_Scale_L.K_ROM_CH);
                                CH_B.Cur_Scale_L.B_Now      = Cur_Scale_L_B_Check(CH_B.Cur_Scale_L.B_ROM_CH);
                                CH_B.Cur_Scale_H.K_Now      = Cur_Scale_H_K_Check(CH_B.Cur_Scale_H.K_ROM_CH);
                                CH_B.Cur_Scale_H.B_Now      = Cur_Scale_H_B_Check(CH_B.Cur_Scale_H.B_ROM_CH);
                                EINT;
                                Interface_DCDC.Req_ChangeMode   = TRUE;
                                WorkStep_Service.State          = STEP_CONTACT_CHECK;
                                WorkStep_Service.Cnt_Delay      = 0;
                            }
                            WorkStep_Service.Req_CMD            = 0;
                        }
                        else
                        {
                            WorkStep_Service.Req_CMD            = 0;
                            WorkStep_Service.Req_Stop_Socket_Err= FALSE;
                            WorkStep_Service.Req_SingleStep     = FALSE;
                            WorkStep_Service.State              = STEP_FLOW_STOP;
                            Fault_Code.Err_Step                 = Fault_Code.Err_Step|cBatErrorVol_Start;
                        }
                    }
                }
                else
                {	//需要上报一个故障码
                	Fault_Code.Err_Global			   |= cStepDataError;
                	WorkStep_Service.Req_CMD			= 0;
                    WorkStep_Service.Req_SingleStep		= FALSE;
                    WorkStep_Service.Req_WorkStep_Index	= 0;
                }
            }
            else if(WorkStep_Service.Req_CMD==WORKSTEP_PRECHARGE)
            {
                //WorkStep_Service.Total_Ah			= 0;
                //WorkStep_Service.Total_Wh			= 0;
                //WorkStep_Service.Step_Cnt			= 0;
                WorkStep_Service.Req_CMD			= 0;
                //WorkStep_Service.Req_SingleStep		= FALSE;
                //WorkStep_Service.Req_WorkStep_Index	= 0;
                //WorkStep_Service.Cnt_Delay			= 0;
                //WorkStep_Service.State				= STEP_PRECHARGE;
				//WorkStep_Service.Step_State         = cWorkStepRunStatus;
            }
            else
            {WorkStep_Service.Req_CMD=0;}
        break;}



        case STEP_CONTACT_CHECK:
        {
            WorkStep_Service.Step_Time=WorkStep_Service.Step_Time+40;
            if( WorkStep_Service.Step_Time%1000==0)
            {
                 WorkStep_Service.Run_TimeTotal++;
            }

            if((WorkStep_Service.Req_CMD==WORKSTEP_STOP)||(WorkStep_Service.Req_Stop_Socket_Err==TRUE)||
               (Fault_Code.Err_Global!=0)||(Fault_Code.Err_Step!=0)||(DCDC_Service.Fault_Module!=0)||(Fault_Code.Alarm!=0))
            {
                WorkStep_Service.Req_CMD        = 0;
                //WorkStep_Service.Req_Stop_Socket_Err=FALSE;
                Interface_DCDC.Req_Mode         = DCDC_STANDBY;
                Interface_DCDC.Req_ChangeMode   = TRUE;
                WorkStep_Service.State          = STEP_FLOW_STOP;
            }
            else if(WorkStep_Service.Req_CMD==WORKSTEP_PAUSE)
            {
                WorkStep_Service.Req_CMD        = 0;
            }
            else if(WorkStep_Service.Step_Time>=Protect_ContactCheck.Time)
            {
                Protect_ContactCheck.Vol_End           = Data_Sum.Vol_Sense;
                WorkStep_Service.State              = STEP_CONTACT_END;
                //WorkStep_Service.Req_Stop_Socket_Err= FALSE;
                Interface_DCDC.Req_Mode             = DCDC_STANDBY;
                Interface_DCDC.Req_ChangeMode       = TRUE;
            }

            WorkStep_Service.Cnt_Delay=WorkStep_Service.Cnt_Delay+40;
            if(WorkStep_Service.Cnt_Delay>=1000)
            {
                WorkStep_Service.Cnt_Delay=0;
                SaveDataToRecord(&Image_UpData.Record);
                Image_UpData.Record.ubChargerStatus=cStepContactCheck;
                Image_UpData.Record.dwBatEnergy=0;
                Image_UpData.Record.ubWorkStepName=cStepCC;
                Image_UpData.Record.udwAH_CCdivCV=0;
                Image_UpData.Record.udwAH_DCCdivDCV=0;
                Image_UpData.Record.udwChargerCapacityNew=0;
                Image_UpData.Record.uwStepCycleStartIndex=0;
                Image_UpData.Record.uwWorkStepNum=0;
                Image_UpData.Record.uwWorkStepTotalNum=0;
                Image_UpData.Req_TX=TRUE;
            }
        break;}

        case STEP_CONTACT_END:
        {
            if((Protect_ContactCheck.Vol_End-Protect_ContactCheck.Vol_Start)>=Protect_ContactCheck.Vol_Rise)
            {
                WorkStep_Data.Index                 = WorkStep_Service.Req_WorkStep_Index;
                WorkStep_Service.Flag_SingleStep    = WorkStep_Service.Req_SingleStep;
                WorkStep_Service.Req_SingleStep     = FALSE;
                //工艺流程起始
                Protect_Global_Cnt_Reset();//全局保护使能与计数器初始化
                WorkStep_Service.Total_Time = 0;
                WorkStep_Service.Total_Ah   = 0;
                WorkStep_Service.Total_Wh   = 0;
                WorkStep_Service.Step_Cnt   = 0;

                WorkStep_Service.Step_Time  = 0;


                Image_Protect.Req_TX        = FALSE;
                Image_StepEnd.Req_TX        = FALSE;
                Image_UpData.Req_TX         = FALSE;

                //循环控制部分数据初始化
                for(WorkStep_Cycle.Now_Cycle=0;WorkStep_Cycle.Now_Cycle<NUM_CYCLE_MAX;WorkStep_Cycle.Now_Cycle++)
                {
                    WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Cnt_Cycle      = 0;
                    WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Index_Cycle    = 0;
                    WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Num_Cycle      = 0;
                    WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Start_Cycle    = 0;
                }
                WorkStep_Cycle.Now_Cycle=0;

                EEPROM_Write_Uint16(cEepromStepComplete,cNotComplete);
                WorkStep_PowerFail.Step_State_Now   = cNotComplete;
                WorkStep_Service.Flag_FlowEnd_Once  = FALSE;
                WorkStep_Service.Flag_FlowStop_Once = FALSE;
                WorkStep_Service.State              = STEP_REQ_READ;

                Past_Data_Manage.Req_Reset          = FALSE;

                WorkStep_Service.Step_State         = cWorkStepRunStatus;
            }
            else
            {
                WorkStep_Service.Req_CMD            = 0;
                WorkStep_Service.Req_Stop_Socket_Err= FALSE;
                Interface_DCDC.Req_Mode             = DCDC_STANDBY;
                Interface_DCDC.Req_ChangeMode       = TRUE;
                WorkStep_Service.State              = STEP_FLOW_STOP;
                Fault_Code.Err_Step                 = Fault_Code.Err_Step|cBatErrorContacCheck;
            }
            WorkStep_Service.Cnt_Delay=0;
        break;}

        case STEP_REQ_READ:
        {
            if(WorkStep_Data.Index>NUM_WORKSTEP_MAX)
            {
                WorkStep_Data.Type          = cStepStatic;      //数据错误工步类型设定为静置
                Fault_Code.Err_Global      |= cStepDataError;   //需要报一个故障码
                WorkStep_Service.State      = STEP_FLOW_STOP;
            }
            else
            {
                SPI_Flash_Read(SERV_FLASH_WORKSTEP,(Uint32)(WorkStep_Data.Index-1)*0x1000,177);
                WorkStep_Service.State      = STEP_WAIT_DATA;
            }
            WorkStep_Service.Cnt_Delay  = 0;
        break;}

        case STEP_WAIT_DATA:
        {
            if(Qurey_SPI_Flash_Read(SERV_FLASH_WORKSTEP)==TRUE)
            {
                WorkStep_Service.State				= STEP_DATA_ANALYSE;
                //WorkStep_Service.Req_CMD			= 0;
                WorkStep_Service.Req_SingleStep		= FALSE;
                WorkStep_Service.Req_WorkStep_Index	= 0;
                WorkStep_Service.Cnt_Delay			= 0;
            }
            else
            {
                if(++WorkStep_Service.Cnt_Delay>3)
                {
                	//3次读取Flash失败，报故障
                	WorkStep_Service.Cnt_Delay	= 0;
                	Fault_Code.Err_Global	   |= cStepDataError;
                	WorkStep_Service.State		= STEP_FLOW_STOP;
                }
                else
                {
                    Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                    WorkStep_Service.State		= STEP_REQ_READ;
                }
            }

        break;}

        case STEP_DATA_ANALYSE:
        {
            //将数据从Flash缓冲区解析出来
            Uint16 Temp;
            Filter_Workstep_Last.Type = WorkStep_Data.Type;
            Filter_Workstep_Last.Rated_Cur = WorkStep_Data.Rated_Cur;
            Filter_Workstep_Last.Cur_End = WorkStep_Data.StepEnd_Cur.End_Value;

            WorkStep_Data.Type_Last=WorkStep_Data.Type;
            Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0];
            if((Temp<=0)||(Temp>LEN_SPI_FLASH_BUFF))
            {
            	WorkStep_Data.Type			= cStepStatic;		//数据错误工步类型设定为静置
            	Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
            	Fault_Code.Err_Global	   |= cStepDataError;
                WorkStep_Service.State		= STEP_FLOW_STOP;	//
                return;
            }

            //工步数据解析
            if((Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[Temp-1]==0x0D)&&
               (sCRC16N(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],Temp-3)==Char2Uint16(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],Temp-3)))
            {
                //数据帧长度
                WorkStep_Data.Length=Temp;

                //工步序号
                WorkStep_Data.Index = Char2Uint16(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],2);

                //工步类型设定
                Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[4];
                if((Temp>0)&&(Temp<=0x0C)&&(Temp!=0x08)&&(Temp!=0x07))
                {
                    if(Temp==cStepCycleStart)
                    {
                    	WorkStep_Data.Index++;
                    	if(WorkStep_Data.Index>NUM_WORKSTEP_MAX)
                    	{
                    		//已经到达最大工步数
                        	WorkStep_Data.Type			= cStepStatic;		//数据错误工步类型设定为静置
                        	Fault_Code.Err_Global	   |= cStepDataError;   //需要报一个故障码
                        	WorkStep_Service.State		= STEP_FLOW_STOP;
                            Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                        	return;
                    	}

                    	//读取下一个工步数据
                        Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                        WorkStep_Service.State		= STEP_REQ_READ;
                        return;

                    }
                    else if(Temp==cStepCycleEnd)
                    {
                        if(WorkStep_Cycle.Now_Cycle==0)
                        {
                            WorkStep_Cycle.Now_Cycle=1;

                            //循环次数清零
                            WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Cnt_Cycle=0;

                            //循环序号
                            Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[6]&0x00FF;
                            Temp=Temp<<8;
                            Temp=Temp|(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[7]&0x00FF);
                            WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Index_Cycle=Temp;

                            //循环结束后的跳转逻辑
                            WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].EndLogic_Cycle=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[10]&0x00FF;

                            //循环需要执行的次数
                            Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[11]&0x00FF;
                            Temp=Temp<<8;
                            Temp=Temp|(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[12]&0x00FF);
                            WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Num_Cycle=Temp;

                            //当前循环起始工步号，用于当前循环结束后跳转用
                            Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[13]&0x00FF;
                            Temp=Temp<<8;
                            Temp=Temp|(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[14]&0x00FF);
                            WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Start_Cycle=Temp;

                            //当前循环结束工步号
                            Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[2]&0x00FF;
                            Temp=Temp<<8;
                            Temp=Temp|(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[3]&0x00FF);
                            WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].End_Cycle =Temp;

                            //循环起始工步号和循环结束工步号是否错误检查
                            if(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].End_Cycle<WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Start_Cycle)
                            {
                                WorkStep_Data.Type          = cStepStatic;      //数据错误工步类型设定为静置
                                Fault_Code.Err_Global      |= cStepDataError;   //需要报一个故障码
                                WorkStep_Service.State      = STEP_FLOW_STOP;
                                Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                                return;
                            }
                        }
                        else
                        {
                            Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[2]&0x00FF;
                            Temp=Temp<<8;
                            Temp=Temp|(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[3]&0x00FF);

                            if(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].End_Cycle !=Temp)
                            {
                                //新的循环开始
                                WorkStep_Cycle.Now_Cycle++;

                                //循环嵌套是否超出最大深度
                                if(WorkStep_Cycle.Now_Cycle>NUM_CYCLE_MAX)
                                {
                                    WorkStep_Data.Type          = cStepStatic;      //数据错误工步类型设定为静置
                                    Fault_Code.Err_Global      |= cStepDataError;   //需要报一个故障码
                                    WorkStep_Service.State      = STEP_FLOW_STOP;
                                    Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                                    return;
                                }

                                //新循环循环次数清零
                                WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Cnt_Cycle=0;

                                //循环序号
                                Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[6]&0x00FF;
                                Temp=Temp<<8;
                                Temp=Temp|(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[7]&0x00FF);
                                WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Index_Cycle=Temp;

                                //循环结束后的跳转逻辑
                                WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].EndLogic_Cycle=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[10]&0x00FF;

                                //循环需要执行的次数
                                Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[11]&0x00FF;
                                Temp=Temp<<8;
                                Temp=Temp|(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[12]&0x00FF);
                                WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Num_Cycle=Temp;

                                //当前循环起始工步号，用于当前循环结束后跳转用
                                Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[13]&0x00FF;
                                Temp=Temp<<8;
                                Temp=Temp|(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[14]&0x00FF);
                                WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Start_Cycle=Temp;

                                //当前循环结束工步号
                                Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[2]&0x00FF;
                                Temp=Temp<<8;
                                Temp=Temp|(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[3]&0x00FF);
                                WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].End_Cycle =Temp;

                                //循环起始工步号和循环结束工步号是否错误检查
                                if(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].End_Cycle<WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Start_Cycle)
                                {
                                    WorkStep_Data.Type          = cStepStatic;      //数据错误工步类型设定为静置
                                    Fault_Code.Err_Global      |= cStepDataError;   //需要报一个故障码
                                    WorkStep_Service.State      = STEP_FLOW_STOP;
                                    Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                                    return;
                                }

                                //循环是否交叉检查
                                if((WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].End_Cycle  >=WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle-1].End_Cycle)||
                                   (WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Start_Cycle<=WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle-1].Start_Cycle))
                                {
                                    WorkStep_Data.Type          = cStepStatic;      //数据错误工步类型设定为静置
                                    Fault_Code.Err_Global      |= cStepDataError;   //需要报一个故障码
                                    WorkStep_Service.State      = STEP_FLOW_STOP;
                                    Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                                    return;
                                }
                            }
                        }

                        //循环运行完一次，循环计数器增加
                        ++WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Cnt_Cycle;

                        //循环结束判断
                        if(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Cnt_Cycle>=WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Num_Cycle)
                        {
                            if(WorkStep_Cycle.Now_Cycle>=1)
                            {
                                if(WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].EndLogic_Cycle==0x01)
                                {
                                    //当前循环结束继续向下执行
                                    WorkStep_Cycle.Now_Cycle--;
                                    WorkStep_Data.Index++;

                                    if(WorkStep_Data.Index>NUM_WORKSTEP_MAX)
                                    {
                                        //已经到达最大工步数
                                        WorkStep_Data.Type          = cStepStatic;      //数据错误工步类型设定为静置
                                        Fault_Code.Err_Global      |= cStepDataError;   //需要报一个故障码
                                        WorkStep_Service.State      = STEP_FLOW_STOP;
                                        Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                                        return;
                                    }

                                    Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                                    WorkStep_Service.State      = STEP_REQ_READ;
                                    return;
                                }
                                else
                                {
                                    //当前循环结束停止工步
                                    WorkStep_Cycle.Now_Cycle--;
                                    Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                                    WorkStep_Service.State      = STEP_FLOW_END;
                                    return;
                                }
                            }
                            else
                            {
                                //嵌套退出超越最底层
                                WorkStep_Data.Type          = cStepStatic;      //数据错误工步类型设定为静置
                                Fault_Code.Err_Global      |= cStepDataError;   //需要报一个故障码
                                WorkStep_Service.State      = STEP_FLOW_STOP;
                                Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                                return;
                            }
                        }
                        else
                        {
                            //循环次数还未达到，继续循环
                            WorkStep_Data.Index=WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Start_Cycle;
                            Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                            WorkStep_Service.State      = STEP_REQ_READ;
                            return;
                        }
                    }
                    else
                    {
                    	WorkStep_Data.Type=Temp;
                    }
                }
                else
                {
                	WorkStep_Data.Type			= cStepStatic;		//数据错误工步类型设定为静置
                	Fault_Code.Err_Global	   |= cStepDataError;   //需要报一个故障码
                	WorkStep_Service.State		= STEP_FLOW_STOP;
                	return;
                }

                //目标气压电压设定
                Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[5];
                if(Temp==cStepPressure)
                {
                    WorkStep_Data.Pressure=Char2int32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],6);
                    if(WorkStep_Data.Pressure<-100)
                        WorkStep_Data.Pressure=-100;
                    if(WorkStep_Data.Pressure>0)
                        WorkStep_Data.Pressure=0;
                }

                //目标电压设定
                Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[10];
                if(Temp==cStepVolt)
                {
                	WorkStep_Data.Rated_Vol=Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],11);
                //    Protect_Global.Vol_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Vol;
                    WorkStep_Data.Prot_Step_Vol_Diff_Over.Ref_Value     = WorkStep_Data.Rated_Vol;
                }
                else if(Temp==cStepPower)
                {
                	WorkStep_Data.Rated_Vol=0;
                //    Protect_Global.Vol_Diff_Over.Ref_Value              = 0;
                    WorkStep_Data.Prot_Step_Vol_Diff_Over.Ref_Value     = 0;
                }
                else if(Temp==0xFF)
                {
                 	WorkStep_Data.Rated_Vol=0;
                // 	Protect_Global.Vol_Diff_Over.Ref_Value              = 0;
                 	WorkStep_Data.Prot_Step_Vol_Diff_Over.Ref_Value     = 0;
                }
                else
                {
                	WorkStep_Data.Rated_Vol		= 0;
                	Fault_Code.Err_Global	   |= cStepDataError;  //需要报一个故障码debug
                	WorkStep_Service.State		= STEP_FLOW_STOP;//需要报一个故障码debug
                	return;
                }

                //目标电流设定
                Temp=Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[15];
                if(Temp==cStepCurrent)
                {
                	WorkStep_Data.Rated_Cur=Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],16);
                    if(WorkStep_Data.Rated_Cur>WorkStep_Service.Cur_Limited_Set)
                    {
                        WorkStep_Data.Rated_Cur=WorkStep_Service.Cur_Limited_Set;
                    }
                 //   Protect_Global.Cur_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Cur;
                    WorkStep_Data.Prot_Step_Cur_Diff_Over.Ref_Value     = WorkStep_Data.Rated_Cur;
                }
                else if(Temp==0xFF)
                {
                	WorkStep_Data.Rated_Cur=0;
                	WorkStep_Data.Prot_Step_Cur_Diff_Over.Ref_Value     =0;
                }
                else
                {
                	WorkStep_Data.Rated_Cur=0;
                	Fault_Code.Err_Global	   |= cStepDataError;  //需要报一个故障码debug
                	WorkStep_Service.State		= STEP_FLOW_STOP;
                	return;
                }

                //工步时间差记录条件
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[20]==cRecondTime)
                {
                    WorkStep_Data.Record_Time.EN 					= TRUE;
                    WorkStep_Data.Record_Time.Delta_Time			= Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],21);
                    if(WorkStep_Data.Record_Time.Delta_Time>1000)
                    {
                        Past_Data_Manage.EN = TRUE;
                    }
                    else
                    {
                        Past_Data_Manage.EN = FALSE;
                    }
                }
                else
                {
                    Past_Data_Manage.EN                             = FALSE;
                    WorkStep_Data.Record_Time.EN 					= FALSE;
                }

                //工步电压差记录条件
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[25]==cRecondVolt)
                {
                    WorkStep_Data.Record_Vol.EN 			= TRUE;
                    WorkStep_Data.Record_Vol.Delta_Ref_Step	= Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],26);
                }
                else
                {
                    WorkStep_Data.Record_Vol.EN 			= FALSE;
                }

                //工步电流差记录条件
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[30]==cRecondCurrent)
                {
                    WorkStep_Data.Record_Cur.EN 			= TRUE;
                    WorkStep_Data.Record_Cur.Delta_Ref_Step	= Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],31);
                }
                else
                {
                    WorkStep_Data.Record_Cur.EN 			= FALSE;
                }

                //截止条件解析
                WorkStep_Data.StepEnd_Time.End_EN   = FALSE;
                WorkStep_Data.StepEnd_Vol.End_EN    = FALSE;
                WorkStep_Data.StepEnd_Cur.End_EN    = FALSE;
                WorkStep_Data.StepEnd_Ah.End_EN     = FALSE;

                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[35]==0x01)
                {
                    StepEnd_TimeSet(35);                             //工步时间截止条件
                    WorkStep_Data.StepEnd_Time.priority=0x01;
                }
                else if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[35]==0x02)
                {
                    StepEnd_VolSet(35);                                //工步电压截止条件
                    WorkStep_Data.StepEnd_Vol.priority=0x01;
                }
                else if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[35]==0x03)
                {
                    StepEnd_CurSet(35);
                    WorkStep_Data.StepEnd_Cur.priority=0x01;
                }
                else if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[35]==0x07)
                {
                    StepEnd_AhSet(35);
                    WorkStep_Data.StepEnd_Ah.priority=0x01;
                }



                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[44]==0x01)
                {
                     StepEnd_TimeSet(44);                             //工步时间截止条件
                     WorkStep_Data.StepEnd_Time.priority=0x02;

                }
                else if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[44]==0x02)
                {
                     StepEnd_VolSet(44);                                //工步电压截止条件
                     WorkStep_Data.StepEnd_Vol.priority=0x02;
                }
                else if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[44]==0x03)
                {
                     StepEnd_CurSet(44);                                   //工步电流截止条件
                     WorkStep_Data.StepEnd_Cur.priority=0x02;
                }
                else if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[44]==0x07)
                {
                      StepEnd_AhSet(44);
                      WorkStep_Data.StepEnd_Ah.priority=0x02;
                }



                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[53]==0x01)
                {
                      StepEnd_TimeSet(53);                             //工步时间截止条件
                      WorkStep_Data.StepEnd_Time.priority=0x03;

                }
                else if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[53]==0x02)
                {
                      StepEnd_VolSet(53);                                //工步电压截止条件
                      WorkStep_Data.StepEnd_Vol.priority=0x03;
                }
                else if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[53]==0x03)
                {
                      StepEnd_CurSet(53);
                      WorkStep_Data.StepEnd_Cur.priority=0x03;             //工步容量截止条件
                }
                else if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[53]==0x07)
                {
                      StepEnd_AhSet(53);
                      WorkStep_Data.StepEnd_Ah.priority=0x03;
                }


                if(WorkStep_Data.Type==cStepNegPressure)
                {
                    WorkStep_Data.StepEnd_Ah.End_EN=FALSE;
                    WorkStep_Data.StepEnd_Vol.End_EN=FALSE;
                    WorkStep_Data.StepEnd_Cur.End_EN=FALSE;
                    WorkStep_Data.StepEnd_Time.End_EN=FALSE;
                }

                //工步电池过温告警
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[62]==cSSProtectBatTempUpperLimit)
                {
                    WorkStep_Data.Prot_Step_Bat_Over_Temp.Protect_EN    = TRUE;
                    WorkStep_Data.Prot_Step_Bat_Over_Temp.Ref_Value		= Char2Uint16(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],63);
                    WorkStep_Data.Prot_Step_Bat_Over_Temp.Equation		= cStepGreater;
                }
                else
                {
                    WorkStep_Data.Prot_Step_Bat_Over_Temp.Protect_EN    = FALSE;
                }

                //工步充电过容保护
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[65]==cSSProtectCapUpperLimit)
                {
                    WorkStep_Data.Prot_Step_CH_Over_Ah.Protect_EN       = TRUE;
                    Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[65]	= 0;//为方便读取数据置0
                    WorkStep_Data.Prot_Step_CH_Over_Ah.Ref_Value		= Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],65)*10;
                    WorkStep_Data.Prot_Step_CH_Over_Ah.Equation			= cStepGreater;
                }
                else
                {
                    WorkStep_Data.Prot_Step_CH_Over_Ah.Protect_EN       = FALSE;
                }

                //工步放电过容保护
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[69]==cSSProtectCapLowerLimit)
                {
                    WorkStep_Data.Prot_Step_DIS_Over_Ah.Protect_EN      = TRUE;
                    Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[69]	= 0;
                    WorkStep_Data.Prot_Step_DIS_Over_Ah.Ref_Value		= Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],69)*10;
                    WorkStep_Data.Prot_Step_DIS_Over_Ah.Equation		= cStepGreater;
                }
                else
                {
                    WorkStep_Data.Prot_Step_DIS_Over_Ah.Protect_EN      = FALSE;
                }


                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[73]==cSSProtectBatVoltUpperLimit)
                {
                    //工步充电过压保护
                    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Protect_EN  = TRUE;
                    Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[73]	= 0;
                    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Ref_Value	= Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],73);
                    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Equation	= cStepGreater;

                    //静置工步输出端口过压
                    WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Protect_EN  = TRUE;
                    WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Ref_Value   = WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Ref_Value;
                    WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Equation    = cStepGreater;
                }
                else
                {
                    WorkStep_Data.Prot_Step_CH_Over_Vol_Bat.Protect_EN  = FALSE;
                    WorkStep_Data.Prot_Step_CH_Over_Vol_Out.Protect_EN  = FALSE;
                }


                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[77]==cSSProtectBatVoltLowerLimit)
                {
                    //工步放电欠压保护
                    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Protect_EN=TRUE;
                    Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[77]	= 0;
                    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Ref_Value	= Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],77);
                    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Equation	= cStepLess;

                    //静置工步输出端口欠压
                    WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Protect_EN=TRUE;
                    WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Ref_Value = WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Ref_Value;
                    WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Equation  = cStepLess;
                }
                else
                {
                    WorkStep_Data.Prot_Step_DIS_Under_Vol_Bat.Protect_EN=FALSE;
                    WorkStep_Data.Prot_Step_DIS_Under_Vol_Out.Protect_EN=FALSE;
                }

                //工步充电过流保护
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[81]==cSSProtectCurrUpperLimit)
                {
                    WorkStep_Data.Prot_Step_CH_Over_Cur.Protect_EN      = TRUE;
                    Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[81]	= 0;
                    WorkStep_Data.Prot_Step_CH_Over_Cur.Ref_Value		= Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],81);
                    WorkStep_Data.Prot_Step_CH_Over_Cur.Equation		= cStepGreater;
                }
                else
                {
                    WorkStep_Data.Prot_Step_CH_Over_Cur.Protect_EN      = FALSE;
                }

                //工步放电过流保护
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[85]==cSSProtectCurrLowerLimit)
                {
                    WorkStep_Data.Prot_Step_DIS_Over_Cur.Protect_EN     = TRUE;
                    Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[85]	= 0;
                    WorkStep_Data.Prot_Step_DIS_Over_Cur.Ref_Value		= Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],85);
                    WorkStep_Data.Prot_Step_DIS_Over_Cur.Equation		= cStepGreater;
                }
                else
                {
                    WorkStep_Data.Prot_Step_DIS_Over_Cur.Protect_EN     = FALSE;
                }

                //工步电流正跳变保护
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[89]==cSSProtectOPCPositiveJump)
                {
                    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Protect_EN    = TRUE;//WorkStep_Data.Prot_Step_Cur_Delta_Pos.Protect_EN    = TRUE;
                    Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[89]	= 0;
                    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Ref_Delta 	= Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],89);
                    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Num_Check     = Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[93];
                    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Equation  	= cStepGreater;
                }
                else
                {
                    WorkStep_Data.Prot_Step_Cur_Delta_Pos.Protect_EN    = FALSE;
                }

                //工步电流负跳变保护
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[94]==cSSProtectOPCNegativeJump)
                {
                    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Protect_EN    = FALSE;//WorkStep_Data.Prot_Step_Cur_Delta_Neg.Protect_EN    = TRUE;
                    Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[94]	= 0;
                    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Ref_Delta 	=-Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],94);
                    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Num_Check 	= Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[98];
                    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Equation  	= cStepLess;
                }
                else
                {
                    WorkStep_Data.Prot_Step_Cur_Delta_Neg.Protect_EN    = FALSE;
                }

                //工步电压正跳变保护
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[99]==cSSProtectBatVoltPositiveJump)
                {
                    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Protect_EN    = TRUE;
                    Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[99]	= 0;
                    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Ref_Delta 	= Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],99);
                    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Num_Check 	= Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[103];
                    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Equation  	= cStepGreater;
                }
                else
                {
                    WorkStep_Data.Prot_Step_Vol_Delta_Pos.Protect_EN    = FALSE;
                }

                //工步电压负跳变保护
                if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[104]==cSSProtectBatVoltNegativeJump)
                {
                    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Protect_EN    = TRUE;
                    Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[104]	= 0;
                    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Ref_Delta 	=-Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],104);
                    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Num_Check 	= Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[108];
                    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Equation  	= cStepLess;
                }
                else
                {
                    WorkStep_Data.Prot_Step_Vol_Delta_Neg.Protect_EN    = FALSE;
                }

                //新加工步保护数据解析与老数据进行兼容
                if(WorkStep_Data.Length==175)
                {
                    //定时电压异常保护
                    if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[117]!=0xFF)//0x0B
                    {
                        WorkStep_Data.Prot_Step_Vol_Time.Protect_EN         = TRUE;
                        WorkStep_Data.Prot_Step_Vol_Time.Time_Delay         = Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],109);

                        if((Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[114]&0x80)!=0)
                        {
                            Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[113]=0xFF;
                        }
                        else
                        {
                            Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[113]=0x00;
                        }
                        WorkStep_Data.Prot_Step_Vol_Time.Ref_Value_H        = Char2int32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],113);

                        if((Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[118]&0x80)!=0)
                        {
                            Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[117]=0xFF;
                        }
                        else
                        {
                            Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[117]=0x00;
                        }
                        WorkStep_Data.Prot_Step_Vol_Time.Ref_Value_L        = Char2int32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],117);
                    }
                    else
                    {
                        WorkStep_Data.Prot_Step_Vol_Time.Protect_EN         = FALSE;
                    }

                    //定时电压变化量异常保护
                    if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[129]!=0xFF)//0x0C
                    {
                        WorkStep_Data.Prot_Step_Vol_Change_Time.Protect_EN  = TRUE;
                        WorkStep_Data.Prot_Step_Vol_Change_Time.Time_Delay1 = Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],121);
                        WorkStep_Data.Prot_Step_Vol_Change_Time.Time_Delay2 = Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],125);

                        if((Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[130]&0x80)!=0)
                        {
                            Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[129]=0xFF;
                        }
                        else
                        {
                            Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[129]=0x00;
                        }
                        WorkStep_Data.Prot_Step_Vol_Change_Time.Ref_Delta   = Char2int32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],129);
                    }
                    else
                    {
                        WorkStep_Data.Prot_Step_Vol_Change_Time.Protect_EN  = FALSE;
                    }

                    //电池内部短路保护
                    if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[141]!=0xFF)//0x0D
                    {
                        WorkStep_Data.Prot_Step_BatShort.Protect_EN         = TRUE;

                        WorkStep_Data.Prot_Step_BatShort.Time_Delay         = Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],133);

                        if((Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[138]&0x80)!=0)
                        {
                            Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[137]=0xFF;
                        }
                        else
                        {
                            Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[137]=0x00;
                        }
                        WorkStep_Data.Prot_Step_BatShort.Ref_Value_H        = Char2int32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],137);

                        if((Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[142]&0x80)!=0)
                        {
                            Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[141]=0xFF;
                        }
                        else
                        {
                            Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[141]=0x00;
                        }
                        WorkStep_Data.Prot_Step_BatShort.Ref_Value_L        = Char2int32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],141);
                    }
                    else
                    {
                        WorkStep_Data.Prot_Step_BatShort.Protect_EN         = FALSE;
                    }

                    //工步电压超差保护
                    if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[145]==cSSProtectVol_Diff)
                    {
                        WorkStep_Data.Prot_Step_Vol_Diff_Over.Protect_EN    = TRUE;
                        Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[145]   = 0;
                        WorkStep_Data.Prot_Step_Vol_Diff_Over.Ref_Value_Diff   = Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],145);
                    }
                    else
                    {
                        WorkStep_Data.Prot_Step_Vol_Diff_Over.Protect_EN    = FALSE;
                    }

                    //工步电流超差保护
                    if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[149]==cSSProtectCur_Diff)
                    {
                        WorkStep_Data.Prot_Step_Cur_Diff_Over.Protect_EN    = TRUE;
                        Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[149]   = 0;
                        WorkStep_Data.Prot_Step_Cur_Diff_Over.Ref_Value_Diff   = Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],149);
                    }
                    else
                    {
                        WorkStep_Data.Prot_Step_Cur_Diff_Over.Protect_EN    = FALSE;
                    }

                    //跨工步正间隔电压变化量保护
                    if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[153]==cSSProtectVol_Interval_Pos)
                    {
                       WorkStep_Data.Prot_Step_Vol_Interval_Pos.Protect_EN    = TRUE;
                       Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[153]   = 0;
                       WorkStep_Data.Prot_Step_Vol_Interval_Pos.Ref_Delta     = Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],153);
                       WorkStep_Data.Prot_Step_Vol_Interval_Pos.Equation      = cStepGreater;
                    }
                    else
                    {
                       WorkStep_Data.Prot_Step_Vol_Interval_Pos.Protect_EN    = FALSE;
                    }

                    //跨工步负间隔电压变化量保护
                    if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[157]==cSSProtectVol_Interval_Neg)
                    {
                       WorkStep_Data.Prot_Step_Vol_Interval_Neg.Protect_EN    = TRUE;
                       Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[157]  = 0;
                       WorkStep_Data.Prot_Step_Vol_Interval_Neg.Ref_Delta     =-Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],157);
                       WorkStep_Data.Prot_Step_Vol_Interval_Neg.Equation      = cStepLess;
                    }
                    else
                    {
                       WorkStep_Data.Prot_Step_Vol_Interval_Neg.Protect_EN    = FALSE;
                    }
                    //工步电池低温告警
                    if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[161]==cSSProtectBatTempDownUpperLimit)
                    {
                       WorkStep_Data.Prot_Step_Bat_Under_Temp.Protect_EN    = TRUE;
                       WorkStep_Data.Prot_Step_Bat_Under_Temp.Ref_Value     = Char2int16(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],162);
                       WorkStep_Data.Prot_Step_Bat_Under_Temp.Equation      = cStepLess;
                    }
                    else
                    {
                       WorkStep_Data.Prot_Step_Bat_Under_Temp.Protect_EN    = FALSE;
                    }
                    WorkStep_Data.Formula_NumberH=Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],164);
                    WorkStep_Data.Formula_NumberL=Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],168);

                }
                else
                {
                    WorkStep_Data.Prot_Step_Vol_Time.Protect_EN         = FALSE;
                    WorkStep_Data.Prot_Step_Vol_Change_Time.Protect_EN  = FALSE;
                    WorkStep_Data.Prot_Step_BatShort.Protect_EN         = FALSE;
                }

                //工步类型映射到底层DCDC工作类型
                if((WorkStep_Data.Type==cStepStatic)||(WorkStep_Data.Type==cStepNegPressure))
                {
                    Interface_DCDC.Req_Mode		= DCDC_STATIC;
                    //Protect_Configure_STATIC();
                }
                else if(WorkStep_Data.Type==cStepCC)
                {
                    Interface_DCDC.Req_Mode		= DCDC_CC;
                    //Protect_Configure_CC();
                    DINT;
                    //加范围判断
                    CH_A.Cur_Scale_L.K_Now      = Cur_Scale_L_K_Check(CH_A.Cur_Scale_L.K_ROM_CH);
                    CH_A.Cur_Scale_L.B_Now      = Cur_Scale_L_B_Check(CH_A.Cur_Scale_L.B_ROM_CH);
                    CH_A.Cur_Scale_H.K_Now      = Cur_Scale_H_K_Check(CH_A.Cur_Scale_H.K_ROM_CH);
                    CH_A.Cur_Scale_H.B_Now      = Cur_Scale_H_B_Check(CH_A.Cur_Scale_H.B_ROM_CH);

                    CH_B.Cur_Scale_L.K_Now      = Cur_Scale_L_K_Check(CH_B.Cur_Scale_L.K_ROM_CH);
                    CH_B.Cur_Scale_L.B_Now      = Cur_Scale_L_B_Check(CH_B.Cur_Scale_L.B_ROM_CH);
                    CH_B.Cur_Scale_H.K_Now      = Cur_Scale_H_K_Check(CH_B.Cur_Scale_H.K_ROM_CH);
                    CH_B.Cur_Scale_H.B_Now      = Cur_Scale_H_B_Check(CH_B.Cur_Scale_H.B_ROM_CH);
                    EINT;
                }
                else if(WorkStep_Data.Type==cStepCV)
                {
                    if(Interface_DCDC.Req_Aging==TRUE)
                	{
                    	Interface_DCDC.Req_Mode	= DCDC_CV;
                        //Protect_Configure_CV_PWM();
                	}
                    else
                    {
                    	Interface_DCDC.Req_Mode	= DCDC_CCtoCV;
                    	WorkStep_Data.Rated_Cur	= (int32)DCDC_Service.Parall_Total*CUR_MAX_CHANNEL*2;
                        if(WorkStep_Data.Rated_Cur>WorkStep_Service.Cur_Limited_Set)
                        {
                            WorkStep_Data.Rated_Cur=WorkStep_Service.Cur_Limited_Set;
                        }
                        //Protect_Configure_CCtoCV();
                    }

                	DINT;
                    //加范围判断
                    CH_A.Cur_Scale_L.K_Now      = Cur_Scale_L_K_Check(CH_A.Cur_Scale_L.K_ROM_CH);
                    CH_A.Cur_Scale_L.B_Now      = Cur_Scale_L_B_Check(CH_A.Cur_Scale_L.B_ROM_CH);
                    CH_A.Cur_Scale_H.K_Now      = Cur_Scale_H_K_Check(CH_A.Cur_Scale_H.K_ROM_CH);
                    CH_A.Cur_Scale_H.B_Now      = Cur_Scale_H_B_Check(CH_A.Cur_Scale_H.B_ROM_CH);

                    CH_B.Cur_Scale_L.K_Now      = Cur_Scale_L_K_Check(CH_B.Cur_Scale_L.K_ROM_CH);
                    CH_B.Cur_Scale_L.B_Now      = Cur_Scale_L_B_Check(CH_B.Cur_Scale_L.B_ROM_CH);
                    CH_B.Cur_Scale_H.K_Now      = Cur_Scale_H_K_Check(CH_B.Cur_Scale_H.K_ROM_CH);
                    CH_B.Cur_Scale_H.B_Now      = Cur_Scale_H_B_Check(CH_B.Cur_Scale_H.B_ROM_CH);
                    EINT;
                }
                else if(WorkStep_Data.Type==cStepCCtoCV)
                {
                    Interface_DCDC.Req_Mode=DCDC_CCtoCV;
                    //Protect_Configure_CCtoCV();
                    DINT;
                    //加范围判断
                    CH_A.Cur_Scale_L.K_Now      = Cur_Scale_L_K_Check(CH_A.Cur_Scale_L.K_ROM_CH);
                    CH_A.Cur_Scale_L.B_Now      = Cur_Scale_L_B_Check(CH_A.Cur_Scale_L.B_ROM_CH);
                    CH_A.Cur_Scale_H.K_Now      = Cur_Scale_H_K_Check(CH_A.Cur_Scale_H.K_ROM_CH);
                    CH_A.Cur_Scale_H.B_Now      = Cur_Scale_H_B_Check(CH_A.Cur_Scale_H.B_ROM_CH);

                    CH_B.Cur_Scale_L.K_Now      = Cur_Scale_L_K_Check(CH_B.Cur_Scale_L.K_ROM_CH);
                    CH_B.Cur_Scale_L.B_Now      = Cur_Scale_L_B_Check(CH_B.Cur_Scale_L.B_ROM_CH);
                    CH_B.Cur_Scale_H.K_Now      = Cur_Scale_H_K_Check(CH_B.Cur_Scale_H.K_ROM_CH);
                    CH_B.Cur_Scale_H.B_Now      = Cur_Scale_H_B_Check(CH_B.Cur_Scale_H.B_ROM_CH);
                    EINT;
                }
                else if(WorkStep_Data.Type==cStepDCC)
                {
                    Interface_DCDC.Req_Mode=DCDC_DCC;
                    //Protect_Configure_DCC();

                    DINT;
                    //加范围判断
                    CH_A.Cur_Scale_L.K_Now      =  Cur_Scale_L_K_Check(CH_A.Cur_Scale_L.K_ROM_DIS);
                    CH_A.Cur_Scale_L.B_Now      = -Cur_Scale_L_B_Check(CH_A.Cur_Scale_L.B_ROM_DIS);
                    CH_A.Cur_Scale_H.K_Now      =  Cur_Scale_H_K_Check(CH_A.Cur_Scale_H.K_ROM_DIS);
                    CH_A.Cur_Scale_H.B_Now      = -Cur_Scale_H_B_Check(CH_A.Cur_Scale_H.B_ROM_DIS);

                    CH_B.Cur_Scale_L.K_Now      =  Cur_Scale_L_K_Check(CH_B.Cur_Scale_L.K_ROM_DIS);
                    CH_B.Cur_Scale_L.B_Now      = -Cur_Scale_L_B_Check(CH_B.Cur_Scale_L.B_ROM_DIS);
                    CH_B.Cur_Scale_H.K_Now      =  Cur_Scale_H_K_Check(CH_B.Cur_Scale_H.K_ROM_DIS);
                    CH_B.Cur_Scale_H.B_Now      = -Cur_Scale_H_B_Check(CH_B.Cur_Scale_H.B_ROM_DIS);
                    EINT;
                }
                else if(WorkStep_Data.Type==cStepDCCtoDCV)
                {
                    Interface_DCDC.Req_Mode=DCDC_DCCtoDCV;
                    //Protect_Configure_DCCtoDCV();
                    DINT;
                    //加范围判断
                    CH_A.Cur_Scale_L.K_Now      =  Cur_Scale_L_K_Check(CH_A.Cur_Scale_L.K_ROM_DIS);
                    CH_A.Cur_Scale_L.B_Now      = -Cur_Scale_L_B_Check(CH_A.Cur_Scale_L.B_ROM_DIS);
                    CH_A.Cur_Scale_H.K_Now      =  Cur_Scale_H_K_Check(CH_A.Cur_Scale_H.K_ROM_DIS);
                    CH_A.Cur_Scale_H.B_Now      = -Cur_Scale_H_B_Check(CH_A.Cur_Scale_H.B_ROM_DIS);

                    CH_B.Cur_Scale_L.K_Now      =  Cur_Scale_L_K_Check(CH_B.Cur_Scale_L.K_ROM_DIS);
                    CH_B.Cur_Scale_L.B_Now      = -Cur_Scale_L_B_Check(CH_B.Cur_Scale_L.B_ROM_DIS);
                    CH_B.Cur_Scale_H.K_Now      =  Cur_Scale_H_K_Check(CH_B.Cur_Scale_H.K_ROM_DIS);
                    CH_B.Cur_Scale_H.B_Now      = -Cur_Scale_H_B_Check(CH_B.Cur_Scale_H.B_ROM_DIS);
                    EINT;
                }
                else if(WorkStep_Data.Type==cStepDCV)
                {
                    Interface_DCDC.Req_Mode		= DCDC_DCCtoDCV;
                	WorkStep_Data.Rated_Cur		= (int32)DCDC_Service.Parall_Total*CUR_MAX_CHANNEL*2;
                    if(WorkStep_Data.Rated_Cur>WorkStep_Service.Cur_Limited_Set)
                    {
                        WorkStep_Data.Rated_Cur=WorkStep_Service.Cur_Limited_Set;
                    }

                	//Protect_Configure_DCCtoDCV();
                    DINT;
                    //加范围判断
                    CH_A.Cur_Scale_L.K_Now      =  Cur_Scale_L_K_Check(CH_A.Cur_Scale_L.K_ROM_DIS);
                    CH_A.Cur_Scale_L.B_Now      = -Cur_Scale_L_B_Check(CH_A.Cur_Scale_L.B_ROM_DIS);
                    CH_A.Cur_Scale_H.K_Now      =  Cur_Scale_H_K_Check(CH_A.Cur_Scale_H.K_ROM_DIS);
                    CH_A.Cur_Scale_H.B_Now      = -Cur_Scale_H_B_Check(CH_A.Cur_Scale_H.B_ROM_DIS);

                    CH_B.Cur_Scale_L.K_Now      =  Cur_Scale_L_K_Check(CH_B.Cur_Scale_L.K_ROM_DIS);
                    CH_B.Cur_Scale_L.B_Now      = -Cur_Scale_L_B_Check(CH_B.Cur_Scale_L.B_ROM_DIS);
                    CH_B.Cur_Scale_H.K_Now      =  Cur_Scale_H_K_Check(CH_B.Cur_Scale_H.K_ROM_DIS);
                    CH_B.Cur_Scale_H.B_Now      = -Cur_Scale_H_B_Check(CH_B.Cur_Scale_H.B_ROM_DIS);
                    EINT;
                }

                if((WorkStep_Data.Type==cStepStatic)||(WorkStep_Data.Type==cStepNegPressure))
                {
                    Protect_Global.Vol_Fluct_Over.Num_Change_Mask=375;
                }
                else
                {
                    Protect_Global.Vol_Fluct_Over.Num_Change_Mask=76;
                }


                Interface_DCDC.Req_Vol 			= WorkStep_Data.Rated_Vol;
                Interface_DCDC.Req_Cur_Total 	= WorkStep_Data.Rated_Cur;

              //  Protect_Global.Cur_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Cur;
             //   Protect_Global.Vol_Diff_Over.Ref_Value              = WorkStep_Data.Rated_Vol;
                WorkStep_Data.Prot_Step_Vol_Diff_Over.Ref_Value     = WorkStep_Data.Rated_Vol;
                WorkStep_Data.Prot_Step_Cur_Diff_Over.Ref_Value     = WorkStep_Data.Rated_Cur;


                WorkStep_Service.Step_Cnt++;
                WorkStep_Service.State			= STEP_END_CHECK;
                Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
            }
            else
            {
                Reset_SPI_FLASH_Service(SERV_FLASH_WORKSTEP);
                WorkStep_Service.Cnt_Delay		= 0;
                WorkStep_Data.Type              = cStepStatic;      //数据错误工步类型设定为静置
                Fault_Code.Err_Global          |= cStepDataError;   //需要报一个故障码
                WorkStep_Service.State			= STEP_FLOW_STOP;
            }
        break;}

        case STEP_END_CHECK:
        {
            if(WorkStep_Data.StepEnd_Vol.End_EN==TRUE)
            {
                SaveDataToRecord(&Image_StepEnd.Record);
                Image_StepEnd.Record.Res_Contact=0;
                Image_StepEnd.Record.Res_Loop=0;
                Image_StepEnd.Record.dwBatEnergy=0;
                Image_StepEnd.Record.dwOutPutCurrentNew_UP=0;
                Image_StepEnd.Record.udwAH_CCdivCV=0;
                Image_StepEnd.Record.udwAH_DCCdivDCV=0;
                Image_StepEnd.Record.udwSuperRunTime=0;
                Image_StepEnd.Record.udwChargerCapacityNew=0;
                if(WorkStep_Data.StepEnd_Vol.End_Logic==cStepLess)
            	{
            		if(Image_StepEnd.Record.wBatSenseAVoltNew_UP<=WorkStep_Data.StepEnd_Vol.End_Value)
            		{
            			if(++WorkStep_Service.Cnt_Delay>=3)
            			{
            				WorkStep_Data.StepEnd_Vol.IsStepEnd	= TRUE;
            			    if(WorkStep_Data.StepEnd_Vol.priority==0x02)
            			    {
            			            WorkStep_Service.State  = STEP_FLOW_STOP;
            			            Fault_Code.Err_Step |= cStepEnd2Fault;
            			    }
            			    else if(WorkStep_Data.StepEnd_Vol.priority==0x03)
                            {
                                    WorkStep_Service.State  = STEP_FLOW_STOP;
                                    Fault_Code.Err_Step |= cStepEnd3Fault;
                            }
            			    else
            			    {
            				     WorkStep_Service.State				= STEP_STEPEND;//启动前工步满足结束条件
            			    }
            				WorkStep_Service.Cnt_Delay			= 0;
            				Image_StepEnd.Req_TX                = TRUE;
            			}
            		}
            		else
            		{
            			WorkStep_Service.State		= STEP_STANDBY2RUN;
            			WorkStep_Service.Cnt_Delay	= 0;
            		}
            	}
            	else if(WorkStep_Data.StepEnd_Vol.End_Logic==cStepGreater)
            	{
            		if(Image_StepEnd.Record.wBatSenseAVoltNew_UP>=WorkStep_Data.StepEnd_Vol.End_Value)
            		{
            			if(++WorkStep_Service.Cnt_Delay>=3)
            			{
            				WorkStep_Data.StepEnd_Vol.IsStepEnd	= TRUE;
                            if(WorkStep_Data.StepEnd_Vol.priority==0x02)
                            {
                                WorkStep_Service.State  = STEP_FLOW_STOP;
                                Fault_Code.Err_Step |= cStepEnd2Fault;
                            }
                            else if(WorkStep_Data.StepEnd_Vol.priority==0x03)
                            {
                                WorkStep_Service.State  = STEP_FLOW_STOP;
                                Fault_Code.Err_Step |= cStepEnd3Fault;
                            }
            				else
            				{
            				    WorkStep_Service.State				= STEP_STEPEND;//启动前工步满足结束条件
            				}
            				WorkStep_Service.Cnt_Delay			= 0;
            				Image_StepEnd.Req_TX                = TRUE;
            			}
            		}
            		else
            		{
            			WorkStep_Service.State		= STEP_STANDBY2RUN;
            			WorkStep_Service.Cnt_Delay	= 0;
            		}
            	}
            }
            else
            {
                WorkStep_Service.State		= STEP_STANDBY2RUN;
                WorkStep_Service.Cnt_Delay	= 0;
            }

        break;}

        case STEP_STANDBY2RUN:
        {
            if(WorkStep_Service.Req_PowerFailContine == TRUE)
            {
                WorkStep_Service.Step_Time  = WorkStep_PowerFail.Step_Time_EEPROM;
                WorkStep_Service.Step_Ah    = WorkStep_PowerFail.Step_Ah_EEPROM;
                WorkStep_Service.Step_Wh    = WorkStep_PowerFail.Step_Wh_EEPROM;
                WorkStep_Data.Prot_Step_Vol_Change_Time.Value_t1 =    WorkStep_PowerFail.Vol_T1_EEPROM;
                WorkStep_Data.Prot_Step_Vol_Change_Time.Value_t2 =    WorkStep_PowerFail.Vol_T1_EEPROM;
                WorkStep_Service.Req_PowerFailContine = FALSE;
            }
            else
            {
                WorkStep_Service.Step_Time  = 0;
                WorkStep_Service.Step_Ah    = 0;
                WorkStep_Service.Step_Wh    = 0;
            }

            Past_Data_Manage.Cnt_Save       = 0;
			
            WorkStep_Service.CC_Ah      	= 0;
            WorkStep_Service.CCdivCV_Ah 	= 0;
            WorkStep_Service.DCC_Ah     	= 0;
            WorkStep_Service.DCCdivDCV_Ah	= 0;
            WorkStep_Service.Remain_Ah  	= 0;
            WorkStep_Service.Remain_Wh  	= 0;
            WorkStep_Data.Record_Time.Cnt_Time=0;

            WorkStep_Data.StepEnd_Ah.Cnt_Check=0;
            WorkStep_Data.StepEnd_Cur.Cnt_Check=0;
            WorkStep_Data.StepEnd_Time.Cnt_Check=0;
            WorkStep_Data.StepEnd_Vol.Cnt_Check=0;

            //工步保护检测使能与计数器初始化
            Protect_Step_Cnt_Reset();

			Protect_Global_Cnt_Reset();
            Protect_Global.Vol_Diff_Over.Cnt_Change_Mask    = 0;
            Protect_Global.Vol_Diff_Over.Cnt_Check          = 0;

            Protect_Global.Cur_Diff_Over.Cnt_Change_Mask    = 0;
            Protect_Global.Cur_Diff_Over.Cnt_Check          = 0;

            WorkStep_Data.Prot_Step_Vol_Diff_Over.Cnt_Change_Mask    = 0;
            WorkStep_Data.Prot_Step_Vol_Diff_Over.Cnt_Check          = 0;

            WorkStep_Data.Prot_Step_Cur_Diff_Over.Cnt_Change_Mask    = 0;
            WorkStep_Data.Prot_Step_Cur_Diff_Over.Cnt_Check          = 0;

            WorkStep_Data.Record_Vol.Value_Last=0;
            WorkStep_Data.Record_Cur.Value_Last=0;

            Interface_DCDC.Req_ChangeMode   = TRUE;
            WorkStep_Service.State          = STEP_RUN;
            Middle_Cnt = 0;
            //WorkStep_Service.Req_CMD      = 0;
            WorkStep_Service.Req_CMD_OnRunning=0;
            Elimin_Vol_Sense.Data_In       = 0;
            Elimin_Vol_Sense.Data_Last1    = 0;
            Elimin_Vol_Sense.Data_Last2    = 0;
            Elimin_Vol_Sense.Data_Last3    = 0;
            Elimin_Vol_Sense.Delta_Last    = 0;
            Elimin_Vol_Sense.Delta_Last1    = 0;
            Elimin_Vol_Sense.Data_Out      = 0;
            Elimin_Vol_Sense.Data_Update_cnt = 0;
            if((WorkStep_Data.Type >= cStepCC)&&(WorkStep_Data.Type <= cStepCCtoCV)&&(WorkStep_Data.Rated_Cur > 0))
            {
                Filter_Workstep_Last.Voltage_Trend = 1;//Rise
            }
            else if((((WorkStep_Data.Type >= cStepDCC)&&(WorkStep_Data.Type <= cStepDW))||(WorkStep_Data.Type == cStepDCCtoDCV))&&(WorkStep_Data.Rated_Cur > 0))
            {
                Filter_Workstep_Last.Voltage_Trend = 2;//Fall
            }
            else if(WorkStep_Data.Type == cStepStatic)
            {
                if((Filter_Workstep_Last.Type >= cStepCC)&&(Filter_Workstep_Last.Type <= cStepCCtoCV)&&(Filter_Workstep_Last.Rated_Cur > 0))
                {
                    Filter_Workstep_Last.Voltage_Trend = 2;//fall
                }
                else if((((Filter_Workstep_Last.Type >= cStepDCC)&&(Filter_Workstep_Last.Type <= cStepDW))||(Filter_Workstep_Last.Type == cStepDCCtoDCV))&&(Filter_Workstep_Last.Rated_Cur > 0))
                {
                    Filter_Workstep_Last.Voltage_Trend = 1;//rise
                }
                else
                {
                    Filter_Workstep_Last.Voltage_Trend = 0;//static
                }
            }
            else
            {
                Filter_Workstep_Last.Voltage_Trend = 0;//static
            }
            /*if(WorkStep_Data.Type == cStepStatic)
            {
                Protect_Global.Vol_Fluct_Over.Num_Change_Mask = 15;
            }
            else
            {
                Protect_Global.Vol_Fluct_Over.Num_Change_Mask = 2;
            }*/

        break;}

        case STEP_PAUSE2RUN:
        {
            if((WorkStep_Data.Type==cStepStatic)||(WorkStep_Data.Type==cStepNegPressure))
            {
                Interface_DCDC.Req_Mode=DCDC_STATIC;
            }
            else if(WorkStep_Data.Type==cStepCC)
            {
                Interface_DCDC.Req_Mode=DCDC_CC;;
            }
            else if(WorkStep_Data.Type==cStepCV)
            {
                if(Interface_DCDC.Req_Aging==TRUE)
            	{
                	Interface_DCDC.Req_Mode	= DCDC_CV;
            	}
                else
                {
                	Interface_DCDC.Req_Mode	= DCDC_CCtoCV;
                }
            }
            else if(WorkStep_Data.Type==cStepCCtoCV)
            {
                Interface_DCDC.Req_Mode=DCDC_CCtoCV;
            }
            else if(WorkStep_Data.Type==cStepDCC)
            {
                Interface_DCDC.Req_Mode=DCDC_DCC;
            }
            else if(WorkStep_Data.Type==cStepDCCtoDCV)
            {
                Interface_DCDC.Req_Mode=DCDC_DCCtoDCV;
            }
            else if(WorkStep_Data.Type==cStepDCV)
            {
                Interface_DCDC.Req_Mode=DCDC_DCV;
            }

            //工步保护检测使能与计数器初始化
            Protect_Step_Cnt_Reset();

            Protect_Global.Vol_Diff_Over.Cnt_Change_Mask    = 0;
            Protect_Global.Vol_Diff_Over.Cnt_Check          = 0;

            Protect_Global.Cur_Diff_Over.Cnt_Change_Mask    = 0;
            Protect_Global.Cur_Diff_Over.Cnt_Check          = 0;

            Protect_Global.Vol_Fluct_Over.Cnt_Change_Mask   = 0;
            Protect_Global.Vol_Fluct_Over.Cnt_Check         = 0;

            Protect_Global.Cur_Fluct_Over.Cnt_Change_Mask   = 0;
            Protect_Global.Cur_Fluct_Over.Cnt_Check         = 0;

            Protect_Global.Res_Contact_Over.Cnt_Change_Mask = 0;
            Protect_Global.Res_Contact_Over.Cnt_Check       = 0;

            Protect_Global.Res_Loop_Over.Cnt_Change_Mask    = 0;
            Protect_Global.Res_Loop_Over.Cnt_Check          = 0;

            WorkStep_Data.Record_Vol.Value_Last=0;
            WorkStep_Data.Record_Cur.Value_Last=0;

            WorkStep_Data.StepEnd_Cur.Cnt_Mask          = 0;

            Interface_DCDC.Req_ChangeMode   = TRUE;
            WorkStep_Service.State          = STEP_RUN;
            Middle_Cnt = 0;
            //WorkStep_Service.Req_CMD      = 0;
            WorkStep_Service.Req_CMD_OnRunning= 0;
            Elimin_Vol_Sense.Data_In       = 0;
            Elimin_Vol_Sense.Data_Last1    = 0;
            Elimin_Vol_Sense.Data_Last2    = 0;
            Elimin_Vol_Sense.Data_Last3    = 0;
            Elimin_Vol_Sense.Delta_Last    = 0;
            Elimin_Vol_Sense.Delta_Last1    = 0;
            Elimin_Vol_Sense.Data_Out      = 0;
            Elimin_Vol_Sense.Data_Update_cnt = 0;
            if((WorkStep_Data.Type >= cStepCC)&&(WorkStep_Data.Type <= cStepCCtoCV)&&(WorkStep_Data.Rated_Cur > 0))
            {
                Filter_Workstep_Last.Voltage_Trend = 1;//Rise
            }
            else if((((WorkStep_Data.Type >= cStepDCC)&&(WorkStep_Data.Type <= cStepDW))||(WorkStep_Data.Type == cStepDCCtoDCV))&&(WorkStep_Data.Rated_Cur > 0))
            {
                Filter_Workstep_Last.Voltage_Trend = 2;//Fall
            }
            else if(WorkStep_Data.Type == cStepStatic)
            {
                if((Filter_Workstep_Last.Type >= cStepCC)&&(Filter_Workstep_Last.Type <= cStepCCtoCV)&&(Filter_Workstep_Last.Rated_Cur > 0))
                {
                    Filter_Workstep_Last.Voltage_Trend = 2;//Fall
                }
                else if((((Filter_Workstep_Last.Type >= cStepDCC)&&(Filter_Workstep_Last.Type <= cStepDW))||(Filter_Workstep_Last.Type == cStepDCCtoDCV))&&(Filter_Workstep_Last.Rated_Cur > 0))
                {
                    Filter_Workstep_Last.Voltage_Trend = 1;//Rise
                }
                else
                {
                    Filter_Workstep_Last.Voltage_Trend = 0;//static
                }
            }
            else
            {
                Filter_Workstep_Last.Voltage_Trend = 0;//static
            }
            /*if(WorkStep_Data.Type == cStepStatic)
            {
                Protect_Global.Vol_Fluct_Over.Num_Change_Mask = 15;
            }
            else
            {
                Protect_Global.Vol_Fluct_Over.Num_Change_Mask = 2;
            }*/
        break;}


        case STEP_RUN:
        {
            WorkStep_Service.Step_State =cWorkStepRunStatus;
            if((WorkStep_Service.Req_CMD==WORKSTEP_STOP)||(Fault_Code.Err_Global!=0)||(Fault_Code.Err_Step!=0)||(DCDC_Service.Fault_Module!=0))
            {
                WorkStep_Service.Req_CMD        = 0;
                Interface_DCDC.Req_Mode         = DCDC_STANDBY;
                Interface_DCDC.Req_ChangeMode   = TRUE;
                WorkStep_Service.State          = STEP_FLOW_STOP;//
            }
            else if((WorkStep_Service.Req_CMD==WORKSTEP_PAUSE)||(WorkStep_Service.Req_Stop_Socket_Err==TRUE)||(Fault_Code.Alarm!=0))
            {
                WorkStep_Service.Req_CMD        = 0;
                WorkStep_Service.Req_Stop_Socket_Err=FALSE;
                Interface_DCDC.Req_Mode         = DCDC_STANDBY;
                Interface_DCDC.Req_ChangeMode   = TRUE;
                WorkStep_Service.Flag_Pause_Once= FALSE;
                WorkStep_Service.State          = STEP_PAUSE;
            }
            else if(WorkStep_Service.Req_CMD_OnRunning==WORKSTEP_RUN)
            {
                WorkStep_Service.Req_CMD_OnRunning   = 0;
                Interface_DCDC.Req_Mode         = DCDC_STANDBY;
                Interface_DCDC.Req_ChangeMode   = TRUE;
                WorkStep_Data.Index             = WorkStep_Service.Req_WorkStep_Index;
                WorkStep_Service.State          = STEP_REQ_READ;
            }
            else
            {
                SaveDataToRecord(&Image_StepEnd.Record);
                WorkStep_Data.StepEnd_Ah.Now_Value  = Image_StepEnd.Record.udwChargerCapacityNew;//NewWorkStep_Service.Step_Ah;
                WorkStep_Data.StepEnd_Cur.Now_Value = Image_StepEnd.Record.dwOutPutCurrentNew_UP;//Parall_Master.Now_Cur_Total;
                WorkStep_Data.StepEnd_Time.Now_Value= Image_StepEnd.Record.udwSuperRunTime;//WorkStep_Service.Step_Time;
                WorkStep_Data.StepEnd_Vol.Now_Value = Image_StepEnd.Record.wBatSenseAVoltNew_UP;//DCDC_Service.Now_Vol_Sense;

                //debug
                if(DCDC_Service.Now_Mode>=DCDC_CC)
                {
                    WorkStep_End_Check(&WorkStep_Data.StepEnd_Ah);
                    WorkStep_End_Check(&WorkStep_Data.StepEnd_Cur);
                    WorkStep_End_Check(&WorkStep_Data.StepEnd_Time);
                    WorkStep_End_Check(&WorkStep_Data.StepEnd_Vol);
                }

                if((WorkStep_Data.StepEnd_Ah.IsStepEnd==TRUE)||(WorkStep_Data.StepEnd_Cur.IsStepEnd==TRUE)||
                   (WorkStep_Data.StepEnd_Time.IsStepEnd==TRUE)||(WorkStep_Data.StepEnd_Vol.IsStepEnd==TRUE))
                {

                    WorkStep_Data.Vol_Interval_FreshFlag=TRUE;
                    WorkStep_Data.Prot_Step_Vol_Interval_Pos.Last_Value = Image_StepEnd.Record.wBatSenseAVoltNew_UP;
                    WorkStep_Data.Prot_Step_Vol_Interval_Neg.Last_Value = Image_StepEnd.Record.wBatSenseAVoltNew_UP;
                    Interface_DCDC.Req_Mode         = DCDC_STANDBY;
                    Interface_DCDC.Req_ChangeMode   = TRUE;

                    WorkStep_Service.State  = STEP_STEPEND;
                    if((WorkStep_Data.StepEnd_Ah.IsStepEnd==TRUE)||(WorkStep_Data.StepEnd_Cur.IsStepEnd==TRUE)||(WorkStep_Data.StepEnd_Vol.IsStepEnd==TRUE))
                    Image_StepEnd.Req_TX=TRUE;
                }

                if(WorkStep_Data.StepEnd_Cur.IsStepEnd==TRUE)
                {
                    //电流截止时尾数处理
                    int32 Cur_Mantissa;
                    Cur_Mantissa=Image_StepEnd.Record.dwOutPutCurrentNew_UP%100;
                    if(Cur_Mantissa<50)
                    {
                        Image_StepEnd.Record.dwOutPutCurrentNew_UP=WorkStep_Data.StepEnd_Cur.End_Value-Cur_Mantissa;
                    }
                    else
                    {
                        Image_StepEnd.Record.dwOutPutCurrentNew_UP=WorkStep_Data.StepEnd_Cur.End_Value-Cur_Mantissa+50;
                    }

                }
            }

            //恒流比计算
            if(DCDC_Service.Flag_CV_Step==TRUE)
            {
                //if((WorkStep_Service.Step_Ah-WorkStep_Service.CC_Ah)!=0)
                //{
                //    WorkStep_Service.CCdivCV_Ah=WorkStep_Service.CC_Ah*100/(WorkStep_Service.Step_Ah-WorkStep_Service.CC_Ah);
                //}
                if((WorkStep_Service.Step_Ah)!=0)
                {
                    WorkStep_Service.CCdivCV_Ah=WorkStep_Service.CC_Ah*100/(WorkStep_Service.Step_Ah);
                }
            }
            else
            {
                WorkStep_Service.CCdivCV_Ah=0;
            }

            if(DCDC_Service.Flag_DCV_Step==TRUE)
            {
                //if((WorkStep_Service.Step_Ah-WorkStep_Service.DCC_Ah)!=0)
                //{
                //    WorkStep_Service.DCCdivDCV_Ah=WorkStep_Service.DCC_Ah*100/(WorkStep_Service.Step_Ah-WorkStep_Service.DCC_Ah);
                //}
                if((WorkStep_Service.Step_Ah)!=0)
                {
                    WorkStep_Service.DCCdivDCV_Ah=WorkStep_Service.DCC_Ah*100/(WorkStep_Service.Step_Ah);
                }
            }
            else
            {
                WorkStep_Service.DCCdivDCV_Ah=0;
            }



            WorkStep_Service.Req_CMD=0;

        break;}

        case STEP_PAUSE:
        {
            WorkStep_Service.Step_State =cPauseStatus;
            if(WorkStep_Service.Flag_Pause_Once!=TRUE)
            {
                SaveDataToRecord(&Image_UpData.Record);
                Image_UpData.Req_TX=TRUE;
                WorkStep_Service.Flag_Pause_Once=TRUE;
            }

            if((WorkStep_Service.Req_CMD==WORKSTEP_RUN)&&(Fault_Code.Alarm==0))
            {
                WorkStep_Service.Req_CMD    = 0;
                WorkStep_Service.State      = STEP_PAUSE2RUN;
            }
            else if((WorkStep_Service.Req_CMD==WORKSTEP_STOP)||(Fault_Code.Err_Global!=0)||(Fault_Code.Err_Step!=0)||(DCDC_Service.Fault_Module!=0))
            {
                WorkStep_Service.Req_CMD        = 0;
                Interface_DCDC.Req_Mode         = DCDC_STANDBY;
                Interface_DCDC.Req_ChangeMode   = TRUE;
                WorkStep_Service.State          = STEP_FLOW_STOP;
            }

            WorkStep_Service.Cnt_Delay=WorkStep_Service.Cnt_Delay+40;
            if(WorkStep_Service.Cnt_Delay>=1000)
            {
                WorkStep_Service.Cnt_Delay=0;
                SaveDataToRecord(&Image_UpData.Record);
                Image_UpData.Record.Res_Contact=0;
                Image_UpData.Record.Res_Loop=0;
                Image_UpData.Record.dwOutPutCurrentNew_UP=0;
                Image_UpData.Record.ubChargerStatus=cPauseStatus;
                if(Interface_DCDC.Req_Aging==FALSE)
                {
                    Image_UpData.Req_TX=TRUE;
                }
            }


            WorkStep_Service.Req_CMD=0;
        break;}

        case STEP_STEPEND:
        {
            //工步结束
            if(WorkStep_Data.StepEnd_Vol.IsStepEnd==TRUE)
            {
                WorkStep_Data.StepEnd_Vol.IsStepEnd=FALSE;
                if(WorkStep_Data.StepEnd_Vol.priority==0x02)
                {
                    WorkStep_Service.State  = STEP_FLOW_STOP;
                    Fault_Code.Err_Step  |= cStepEnd2Fault;
                }
                else if(WorkStep_Data.StepEnd_Vol.priority==0x03)
                {
                    WorkStep_Service.State  = STEP_FLOW_STOP;
                    Fault_Code.Err_Step  |= cStepEnd3Fault;
                }
                else if(WorkStep_Service.Flag_SingleStep==TRUE)
                {
                    WorkStep_Service.Flag_SingleStep=FALSE;
                    WorkStep_Service.Req_SingleStep=FALSE;
                    WorkStep_Service.State  = STEP_FLOW_END;
                }
                else if(WorkStep_Data.StepEnd_Vol.End_Result==cStepNext)
                {
                    ++WorkStep_Data.Index;
                    WorkStep_Service.State	= STEP_REQ_READ;
                }
                else if(WorkStep_Data.StepEnd_Vol.End_Result==cStepStop)
                {
                    WorkStep_Service.State	= STEP_FLOW_END;
                }
                else if(WorkStep_Data.StepEnd_Vol.End_Result==cStepJump)
                {
                    WorkStep_Data.Index     = WorkStep_Data.StepEnd_Vol.End_Jump_Index;//WorkStep_Data.Index		= WorkStep_Data.Index+WorkStep_Data.StepEnd_Vol.End_Jump_Index;
                    WorkStep_Service.State	= STEP_REQ_READ;
                }
                else
                {
                	WorkStep_Service.State	= STEP_FLOW_STOP;
                }
            }
            else if(WorkStep_Data.StepEnd_Time.IsStepEnd==TRUE)
            {
                WorkStep_Data.StepEnd_Time.IsStepEnd=FALSE;

                if(WorkStep_Data.StepEnd_Time.priority==0x02)
                {
                    WorkStep_Service.State  = STEP_FLOW_STOP;
                    Fault_Code.Err_Step |= cStepEnd2Fault;
                }
                else if(WorkStep_Data.StepEnd_Time.priority==0x03)
                {
                    WorkStep_Service.State  = STEP_FLOW_STOP;
                    Fault_Code.Err_Step |= cStepEnd3Fault;
                }
                else if(WorkStep_Service.Flag_SingleStep==TRUE)
                {
                    WorkStep_Service.Flag_SingleStep=FALSE;
                    WorkStep_Service.Req_SingleStep=FALSE;
                    WorkStep_Service.State  = STEP_FLOW_END;
                }
                else if(WorkStep_Data.StepEnd_Time.End_Result==cStepNext)
                {
                    ++WorkStep_Data.Index;
                    WorkStep_Service.State	= STEP_REQ_READ;
                }
                else if(WorkStep_Data.StepEnd_Time.End_Result==cStepStop)
                {
                    WorkStep_Service.State	= STEP_FLOW_END;
                }
                else if(WorkStep_Data.StepEnd_Time.End_Result==cStepJump)
                {
                    WorkStep_Data.Index     = WorkStep_Data.StepEnd_Time.End_Jump_Index;//WorkStep_Data.Index		= WorkStep_Data.Index+WorkStep_Data.StepEnd_Time.End_Jump_Index;
                    WorkStep_Service.State  = STEP_REQ_READ;
                }
                else
                {
                	WorkStep_Service.State	= STEP_FLOW_STOP;
                }
            }
            else if(WorkStep_Data.StepEnd_Ah.IsStepEnd==TRUE)
            {
                WorkStep_Data.StepEnd_Ah.IsStepEnd=FALSE;
                if(WorkStep_Data.StepEnd_Ah.priority==0x02)
                {
                    WorkStep_Service.State  = STEP_FLOW_STOP;
                    Fault_Code.Err_Step |= cStepEnd2Fault;
                }
                else if(WorkStep_Data.StepEnd_Ah.priority==0x03)
                {
                    WorkStep_Service.State  = STEP_FLOW_STOP;
                    Fault_Code.Err_Step |= cStepEnd3Fault;
                }
                else if(WorkStep_Service.Flag_SingleStep==TRUE)
                {
                    WorkStep_Service.Flag_SingleStep=FALSE;
                    WorkStep_Service.Req_SingleStep=FALSE;
                    WorkStep_Service.State  = STEP_FLOW_END;
                }
                else if(WorkStep_Data.StepEnd_Ah.End_Result==cStepNext)
                {
                    ++WorkStep_Data.Index;
                    WorkStep_Service.State	= STEP_REQ_READ;
                }
                else if(WorkStep_Data.StepEnd_Ah.End_Result==cStepStop)
                {
                    WorkStep_Service.State	= STEP_FLOW_END;
                }
                else if(WorkStep_Data.StepEnd_Ah.End_Result==cStepJump)
                {
                    WorkStep_Data.Index     = WorkStep_Data.StepEnd_Ah.End_Jump_Index;//WorkStep_Data.Index		= WorkStep_Data.Index+WorkStep_Data.StepEnd_Ah.End_Jump_Index;
                    WorkStep_Service.State	= STEP_REQ_READ;
                }
                else
                {
                	WorkStep_Service.State	= STEP_FLOW_STOP;
                }
            }
            else if(WorkStep_Data.StepEnd_Cur.IsStepEnd==TRUE)
            {
                WorkStep_Data.StepEnd_Cur.IsStepEnd=FALSE;
                if(WorkStep_Data.StepEnd_Cur.priority==0x02)
                {
                     WorkStep_Service.State  = STEP_FLOW_STOP;
                     Fault_Code.Err_Step |= cStepEnd2Fault;
                }
                else if(WorkStep_Data.StepEnd_Cur.priority==0x03)
                {
                     WorkStep_Service.State  = STEP_FLOW_STOP;
                     Fault_Code.Err_Step |= cStepEnd3Fault;
                }
                else if(WorkStep_Service.Flag_SingleStep==TRUE)
                {
                    WorkStep_Service.Flag_SingleStep=FALSE;
                    WorkStep_Service.Req_SingleStep=FALSE;
                    WorkStep_Service.State  = STEP_FLOW_END;
                }
                else if(WorkStep_Data.StepEnd_Cur.End_Result==cStepNext)
                {
                    ++WorkStep_Data.Index;
                    WorkStep_Service.State	= STEP_REQ_READ;
                }
                else if(WorkStep_Data.StepEnd_Cur.End_Result==cStepStop)
                {
                    WorkStep_Service.State 	= STEP_FLOW_END;
                }
                else if(WorkStep_Data.StepEnd_Cur.End_Result==cStepJump)
                {
                    WorkStep_Data.Index     = WorkStep_Data.StepEnd_Cur.End_Jump_Index;//WorkStep_Data.Index		= WorkStep_Data.Index+WorkStep_Data.StepEnd_Cur.End_Jump_Index;
                    WorkStep_Service.State	= STEP_REQ_READ;
                }
                else
                {
                	WorkStep_Service.State	= STEP_FLOW_STOP;
                }
            }
            WorkStep_Service.Cnt_Delay      = 0;
        break;}

        case STEP_FLOW_END:
        {
            WorkStep_Service.Step_State=cStepComplete;
            if(WorkStep_Service.Flag_FlowEnd_Once!=TRUE)
            {
                Interface_DCDC.Req_Mode         = DCDC_STANDBY;
                Interface_DCDC.Req_ChangeMode   = TRUE;

                //WorkStep_Service.Req_CMD=0;
                WorkStep_Service.Req_Stop_Socket_Err=FALSE;

                SaveDataToRecord(&Image_UpData.Record);

                Image_UpData.Record.dwOutPutCurrentNew_UP=0;
				Image_UpData.Record.Res_Loop=0;
                Image_UpData.Record.Res_Contact=0;

                Image_UpData.Req_TX=TRUE;
                if(WorkStep_PowerFail.Step_State_Now==cNotComplete)
                {
                    WorkStep_PowerFail.Step_State_Now=cComplete;
                    EEPROM_Write_Uint16(cEepromStepComplete,cComplete);
                }
                WorkStep_Service.Flag_FlowEnd_Once = TRUE;
                WorkStep_Service.State          = STEP_STANDBY;

                Filter_Workstep_Last.Type =  cStepErr;
                Filter_Workstep_Last.Rated_Cur = 0;
                Filter_Workstep_Last.Cur_End = 0;
                Filter_Workstep_Last.Voltage_Trend = 0;
            }

            WorkStep_Service.Cnt_Delay      = 0;
            //if(WorkStep_Service.Req_CMD!=0)
            //{
            //    WorkStep_Service.State          = STEP_STANDBY;
            //}
        break;}

        case STEP_FLOW_STOP:
        {
            WorkStep_Service.Step_State		= cStopStatus;
            if(WorkStep_Service.Flag_FlowStop_Once!=TRUE)
            {
                Interface_DCDC.Req_Mode         = DCDC_STANDBY;
                Interface_DCDC.Req_ChangeMode   = TRUE;
                WorkStep_Service.Req_Stop_Socket_Err=FALSE;

                //任意原因导致流程没有正常走完都保存一次当前工步数据，可以通过下发强制接续命令从当前工步继续运行
                if((DCDC_Service.Fault_Module&(FAULT_BUS_L|FAULT_BUS_H))==0)
                {WorkStep_Service.Req_PowerFailSave = TRUE;}

                //SaveDataToRecord(&Image_UpData.Record);
                //Image_UpData.Record.dwOutPutCurrentNew_UP=0;
               // Image_UpData.Req_TX=TRUE;
                WorkStep_Service.Flag_FlowStop_Once = TRUE;
                Filter_Workstep_Last.Type =  cStepErr;
                Filter_Workstep_Last.Rated_Cur = 0;
                Filter_Workstep_Last.Cur_End = 0;
                Filter_Workstep_Last.Voltage_Trend = 0;

            }
            else
            {
                if(TCP_Socket[SERV_TCP_WORK].Flag_SocketStatus==TRUE)
                {
                    WorkStep_Service.Cnt_Delay=WorkStep_Service.Cnt_Delay+40;
                }
                else
                {
                    WorkStep_Service.Cnt_Delay=0;
                }

                if(WorkStep_Service.Cnt_Delay>=1000)
                {
                    WorkStep_Service.Cnt_Delay=0;
                    SaveDataToRecord(&Image_UpData.Record);
                    Image_UpData.Record.Res_Contact=0;
                    Image_UpData.Record.Res_Loop=0;
                    Image_UpData.Record.dwBatEnergy=0;
                    Image_UpData.Record.dwOutPutCurrentNew_UP=0;
                    Image_UpData.Record.ubChargerStatus=cStepFault;
                    Image_UpData.Record.ubWorkStepName=0;
                    Image_UpData.Record.udwAH_CCdivCV=0;
                    Image_UpData.Record.udwAH_DCCdivDCV=0;
                    Image_UpData.Record.udwChargerCapacityNew=0;
                    Image_UpData.Record.udwSuperRunTime=0;
                    Image_UpData.Record.uwStepCycleStartIndex=0;
                    Image_UpData.Record.uwWorkStepNum=0;
                    Image_UpData.Record.uwWorkStepTotalNum=0;
                    if(Interface_DCDC.Req_Aging==FALSE)
                    {
                        Image_UpData.Req_TX=TRUE;
                    }
                }
            }


            if((Fault_Code.Err_Global==0)&&(Fault_Code.Err_Step==0)&&(Fault_Code.Alarm==0)&&(DCDC_Service.Fault_Module==0))
            {
                WorkStep_Service.State          = STEP_STANDBY;
                WorkStep_Service.Cnt_Delay      = 0;
            }
            else
            {
                WorkStep_Service.Req_CMD        = 0;
            }
            break;
        }

        case 0:
        {
            Interface_DCDC.Req_Mode         = DCDC_STANDBY;
            Interface_DCDC.Req_ChangeMode   = TRUE;
            WorkStep_Service.State          = STEP_STANDBY;
            WorkStep_Service.Req_CMD        = 0;
        break;}

        default:
        {
            Interface_DCDC.Req_Mode         = DCDC_STANDBY;
            Interface_DCDC.Req_ChangeMode   = TRUE;
            WorkStep_Service.State          = STEP_STANDBY;
            WorkStep_Service.Req_CMD        = 0;
        break;}
    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void WorkStep_Service_Init(void)
{
    WorkStep_Service.State=0;
    WorkStep_Service.Req_CMD=0;
    WorkStep_Service.Req_WorkStep_Index=0;
    WorkStep_Service.Req_SingleStep =FALSE;
    WorkStep_Service.Req_Cur_PreCharge=0;
    WorkStep_Service.Req_Time_PreCharge=0;
    WorkStep_Service.Req_PowerFailSave=FALSE;
    WorkStep_Service.Req_PowerFailContine=FALSE;
    WorkStep_Service.Req_Stop_Socket_Err=FALSE;
    WorkStep_Service.Cnt_Delay=0;
    WorkStep_Service.Run_TimeTotal=0;

    WorkStep_Data.Index=1;
    WorkStep_Data.Type =cStepErr;
    WorkStep_Data.Pressure=-70;

    WorkStep_Service.Step_State=cStopStatus;
    WorkStep_Service.Step_Time = 0;
    WorkStep_Service.Step_Cnt  = 0;
    WorkStep_Service.Step_Wh   = 0;
    WorkStep_Service.Step_Ah   = 0;
    WorkStep_Service.CCdivCV_Ah=0;
    WorkStep_Service.DCCdivDCV_Ah=0;
    WorkStep_Service.Res_Contact =0;
    WorkStep_Service.Res_Loop =0;

    WorkStep_Service.Remain_Ah=0;
    WorkStep_Service.Remain_Wh=0;

    Fault_Code.Alarm=0;
    Fault_Code.Alarm_Bak=0;
    Fault_Code.Cnt_S2M_Fault_Mask=0;
    Fault_Code.Err_Global=0;
    Fault_Code.Err_Global_Bak=0;
    Fault_Code.Err_Step=0;
    Fault_Code.Err_Step_Bak=0;
    Fault_Code.Code=0;

    Parall_Master.Now_Cur_S1=0;
    Parall_Master.Now_Cur_S2=0;
    Parall_Master.Now_Cur_S3=0;
    Parall_Master.Now_Cur_S4=0;

    for(WorkStep_Cycle.Now_Cycle=0;WorkStep_Cycle.Now_Cycle<NUM_CYCLE_MAX;WorkStep_Cycle.Now_Cycle++)
    {
    	WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Cnt_Cycle 		= 0;
    	WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Index_Cycle 	= 0;
    	WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Num_Cycle		= 0;
    }
    WorkStep_Cycle.Now_Cycle=0;
	Elimin_Vol_Sense.Data_In       = 0;
    Elimin_Vol_Sense.Data_Last1    = 0;
    Elimin_Vol_Sense.Data_Last2    = 0;
    Elimin_Vol_Sense.Data_Last3    = 0;
    Elimin_Vol_Sense.Delta_Last    = 0;
    Elimin_Vol_Sense.Delta_Last1    = 0;
    Elimin_Vol_Sense.Data_Out      = 0;
    Elimin_Vol_Sense.Elimin_Abs_Max= 50;//
    Elimin_Vol_Sense.Elimin_Abs_Min= 2 ;//0.4mV~5mV之间的正负跳变剔除
    Elimin_Vol_Sense.Data_Update_cnt = 0;

    Elimin_Vol_Out.Data_In          = 0;
    Elimin_Vol_Out.Data_Last1       = 0;
    Elimin_Vol_Out.Data_Last2       = 0;
    Elimin_Vol_Out.Data_Last3       = 0;
    Elimin_Vol_Out.Delta_Last       = 0;
    Elimin_Vol_Out.Delta_Last1       = 0;
    Elimin_Vol_Out.Data_Out         = 0;
    Elimin_Vol_Out.Elimin_Abs_Max   = 50;//
    Elimin_Vol_Out.Elimin_Abs_Min   = 2 ;//0.4mV~5mV之间的正负跳变剔除
    Elimin_Vol_Out.Data_Update_cnt = 0;

    Filter_Workstep_Last.Type =  cStepErr;
    Filter_Workstep_Last.Rated_Cur = 0;
    Filter_Workstep_Last.Cur_End = 0;
    Filter_Workstep_Last.Voltage_Trend = 0;

    for(Median_Vol_Sense.Cnt=0;Median_Vol_Sense.Cnt<5;Median_Vol_Sense.Cnt++)
    {
        Median_Vol_Sense.Data[Median_Vol_Sense.Cnt]=0;
    }
    Median_Vol_Sense.Cnt            = 0;
    Median_Vol_Sense.Data_Out       = 0;

    for(Median_Vol_Out.Cnt=0;Median_Vol_Out.Cnt<5;Median_Vol_Out.Cnt++)
    {
        Median_Vol_Out.Data[Median_Vol_Out.Cnt]=0;
    }
    Median_Vol_Out.Cnt              = 0;
    Median_Vol_Out.Data_Out         = 0;


    for(Median_Cur_Total.Cnt=0;Median_Cur_Total.Cnt<5;Median_Cur_Total.Cnt++)
    {
        Median_Cur_Total.Data[Median_Cur_Total.Cnt]=0;
    }
    Median_Cur_Total.Cnt              = 0;
    Median_Cur_Total.Data_Out         = 0;

    Image_Protect.Req_TX        = FALSE;
    Image_StepEnd.Req_TX        = FALSE;
    Image_UpData.Req_TX         = FALSE;

    WorkStep_Data.Type_Last     = cStepErr;

    for(Rate_Cur_Compute.Cnt_Delay=0;Rate_Cur_Compute.Cnt_Delay<11;Rate_Cur_Compute.Cnt_Delay++)
    {
        Rate_Cur_Compute.Data_Cur[Rate_Cur_Compute.Cnt_Delay]=0;
    }
    Rate_Cur_Compute.Cnt_Delay  = 0;
    Rate_Cur_Compute.Cnt_Error  = 0;
    Rate_Cur_Compute.Cnt_Error_Reset  = 0;
}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void SaveDataToRecord(struct_Image *Ptr)
{

    Ptr->ubChargerStatus=WorkStep_Service.Step_State;

    Ptr->udwSuperRunTime=WorkStep_Service.Step_Time;

    Ptr->wBatSenseAVoltNew_UP=Data_Sum.Vol_Sense;

    Ptr->dwOutPutCurrentNew_UP=Parall_Master.Now_Cur_Total;

    Ptr->udwChargerCapacityNew=WorkStep_Service.Step_Ah;

    Ptr->uwBatteryDegreeNew=  Data_Sum.Temp_PT;//Filter_Workstep_Last.Voltage_Trend;//

    Ptr->uwWorkStepNum=WorkStep_Data.Index;

    Ptr->ubWorkStepName=WorkStep_Data.Type;

    Ptr->uwStepCycleStartIndex=WorkStep_Cycle.Cycle_Stack[WorkStep_Cycle.Now_Cycle].Index_Cycle;

    Ptr->uwWorkStepTotalNum=WorkStep_Service.Step_Cnt;

    Ptr->udwAH_CCdivCV=WorkStep_Service.CCdivCV_Ah;

    Ptr->udwAH_DCCdivDCV=WorkStep_Service.DCCdivDCV_Ah;

    Ptr->dwBatEnergy=WorkStep_Service.Step_Wh;//debug Rate_Cur_Compute.Rate_Cur_Filter;//

    Ptr->wOutPutVoltNew_UP=Data_Sum.Vol_Out;//Data_Sum.Vol_Sense;

    Ptr->uwChannel1NTCDegreeNew=Data_Sum.Temp_NTC1;//Data_Sum.Temp_NTC1;

    Ptr->Vol_Contact=Data_Sum.Vol_Contact;

    Ptr->Res_Contact=WorkStep_Service.Res_Contact;

    Ptr->Res_Loop=WorkStep_Service.Res_Loop;

    DINT;
    Ptr->DateTime=DateTime;
    EINT;

    Ptr->NegPressure=WorkStep_Data.Pressure;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void CopyImage(struct_Image *Source,struct_Image *Dest)
{

    Dest->ubChargerStatus=Source->ubChargerStatus;

    Dest->udwSuperRunTime=Source->udwSuperRunTime;

    Dest->wBatSenseAVoltNew_UP=Source->wBatSenseAVoltNew_UP;

    Dest->dwOutPutCurrentNew_UP=Source->dwOutPutCurrentNew_UP;

    Dest->udwChargerCapacityNew=Source->udwChargerCapacityNew;

    Dest->uwBatteryDegreeNew= Source->uwBatteryDegreeNew;

    Dest->uwWorkStepNum=Source->uwWorkStepNum;

    Dest->ubWorkStepName=Source->ubWorkStepName;

    Dest->uwStepCycleStartIndex=Source->uwStepCycleStartIndex;

    Dest->uwWorkStepTotalNum=Source->uwWorkStepTotalNum;

    Dest->udwAH_CCdivCV=Source->udwAH_CCdivCV;

    Dest->udwAH_DCCdivDCV=Source->udwAH_DCCdivDCV;

    Dest->dwBatEnergy=Source->dwBatEnergy;

    Dest->wOutPutVoltNew_UP=Source->wOutPutVoltNew_UP;

    Dest->uwChannel1NTCDegreeNew=Source->uwChannel1NTCDegreeNew;

    Dest->Vol_Contact=Source->Vol_Contact;

    Dest->Res_Contact=Source->Res_Contact;

    Dest->Res_Loop=Source->Res_Loop;

    Dest->DateTime=Source->DateTime;
}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Ah_Accumulate(void)
{
    Uint32 Temp;
    WorkStep_Service.Remain_Ah  += Parall_Master.Now_Cur_Total;

    //0.1mAh=360mA*s=9000mA*40mS=90000*0.1mA*40mS
    if(WorkStep_Service.Remain_Ah>=90000)
    {
        Temp = WorkStep_Service.Remain_Ah/90000;
        WorkStep_Service.Remain_Ah = WorkStep_Service.Remain_Ah%90000;
        WorkStep_Service.Step_Ah  += Temp;
        if((DCDC_Service.Now_Mode==DCDC_CC)||(DCDC_Service.Now_Mode==DCDC_CV)||(DCDC_Service.Now_Mode==DCDC_CCtoCV))
        {
            WorkStep_Service.Total_Ah += Temp;
        }
        else if((DCDC_Service.Now_Mode==DCDC_DCC)||(DCDC_Service.Now_Mode==DCDC_DCV)||(DCDC_Service.Now_Mode==DCDC_DCCtoDCV))
        {
            WorkStep_Service.Total_Ah -= Temp;
        }
    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Wh_Accumulate(void)
{
    Uint32 Temp;
    //200A*6.6V不溢出
    WorkStep_Service.Remain_Wh=WorkStep_Service.Remain_Wh+((Uint32)Parall_Master.Now_Cur_Total/50)*((Uint32)Data_Sum.Vol_Sense>>1);
    //0.1mWh=360mW*s=9000mW*40mS=9000*1000*1mA*1mV*40mS
    if(WorkStep_Service.Remain_Wh>=9000000)
    {
        Temp = WorkStep_Service.Remain_Wh/9000000;
        WorkStep_Service.Remain_Wh   = WorkStep_Service.Remain_Wh%9000000;
        WorkStep_Service.Step_Wh    += Temp;
        WorkStep_Service.Total_Wh   += Temp;
    }

}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Res_Contact_Compute(void)
{
    if((DCDC_Service.Now_Mode>=DCDC_CC)&&(DCDC_Service.Now_Mode<=DCDC_DCCtoDCV))
    {
        if(Parall_Master.Now_Cur_Total >= 100000)
        {
            int32 Delta_Vol;
        	if(DCDC_Service.Mode_Parall==PARALL_MASTER)
            {
            	Delta_Vol=Data_Sum.Vol_Contact-Data_Sum.Vol_Sense;
			}
			else if(DCDC_Service.Mode_Parall==SINGLE)
            {
                Delta_Vol = 0;
            }
            else
            {
            	Delta_Vol=0;
            }

        	if(Delta_Vol<0) Delta_Vol=-Delta_Vol;

        	if(Delta_Vol>VOL_MAX_SENSE)
        	{
        		Delta_Vol=VOL_MAX_SENSE;
        	}

        	//1微欧=0.1uV/0.1A
            
        	WorkStep_Service.Res_Contact=(long long)Delta_Vol*1000000/Parall_Master.Now_Cur_Total;

    		if(WorkStep_Service.Res_Contact>65535) WorkStep_Service.Res_Contact=65535;
        }
        else
        {
        	WorkStep_Service.Res_Contact = 0;
        }
    }
    else
    {
    	WorkStep_Service.Res_Contact = 0;
    }
   // Image_UpData.Record.Res_Contact=WorkStep_Service.Res_Contact;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Res_Loop_Compute(void)
{
	int32 Delta_Vol;
    if((DCDC_Service.Now_Mode>=DCDC_CC)&&(DCDC_Service.Now_Mode<=DCDC_DCCtoDCV))
       {
           if(Parall_Master.Now_Cur_Total>=100000)
           {

               Delta_Vol=Data_Sum.Vol_Out-Data_Sum.Vol_Sense;

               if(Delta_Vol<0) Delta_Vol=-Delta_Vol;

               if(Delta_Vol>VOL_MAX_SENSE)
               {
                   Delta_Vol=VOL_MAX_SENSE;
               }

               //1微欧=0.1nV/0.1mA
               WorkStep_Service.Res_Loop=(long long)Delta_Vol*1000000/DCDC_Service.Now_Cur_AB;

               if(WorkStep_Service.Res_Loop>65535) WorkStep_Service.Res_Loop=65535;
           }
           else
           {
               WorkStep_Service.Res_Loop = 0;
           }
       }
       else
       {
           WorkStep_Service.Res_Loop = 0;
       }
      // Image_UpData.Record.Res_Loop=WorkStep_Service.Res_Loop;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void WorkStep_End_Check(struct_StepEnd *Ptr)
{
    if(Ptr->End_EN==FALSE)
    {
        Ptr->Cnt_Check=0;
        Ptr->IsStepEnd=FALSE;
        return;
    }

    if(Ptr->Cnt_Mask<Ptr->Num_Mask)
    {
    	Ptr->Cnt_Mask++;
    	return;
    }

    if(Ptr->End_Logic==cStepLess)
    {
        if(Ptr->Now_Value<=Ptr->End_Value)
        {
           if(++Ptr->Cnt_Check>=1)
           {
               Ptr->Cnt_Check=0;
               Ptr->IsStepEnd=TRUE;
           }
        }
        else
        {
            Ptr->Cnt_Check=0;
        }
    }
    else if(Ptr->End_Logic==cStepGreater)
    {
        if(Ptr->Now_Value>=Ptr->End_Value)
        {
           if(++Ptr->Cnt_Check>=1)
           {
               Ptr->Cnt_Check=0;
               Ptr->IsStepEnd=TRUE;
           }
        }
        else
        {
            Ptr->Cnt_Check=0;
        }
    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
Uint32 Char2Uint32(unsigned char *Ptr,Uint16 Offset)
{
    Uint32 Data=0;
    Ptr  = (unsigned char * )((Uint32)Ptr+Offset*sizeof(unsigned char));
    Data = Data |((Uint32)(*Ptr++)<<24);
    Data = Data |((Uint32)(*Ptr++)<<16);
    Data = Data |((Uint32)(*Ptr++)<<8);
    Data = Data |*Ptr++;
    return(Data);
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
int32 Char2int32(unsigned char *Ptr,Uint16 Offset)
{
    int32 Data=0;
    Ptr  = (unsigned char * )((Uint32)Ptr+Offset*sizeof(unsigned char));
    Data = Data |((int32)(*Ptr++)<<24);
    Data = Data |((int32)(*Ptr++)<<16);
    Data = Data |((int32)(*Ptr++)<<8);
    Data = Data |*Ptr++;
    return(Data);
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
Uint16 Char2Uint16(unsigned char *Ptr,Uint16 Offset)
{
    Uint16 Data=0;
    Ptr  = (unsigned char * )((Uint32)Ptr+Offset*sizeof(unsigned char));
    Data = Data |((Uint16)(*Ptr++)<<8);
    Data = Data |*Ptr++;
    return(Data);
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
int16 Char2int16(unsigned char *Ptr,Uint16 Offset)
{
    int16 Data=0;
    Ptr  = (unsigned char * )((Uint32)Ptr+Offset*sizeof(unsigned char));
    Data = Data |((int16)(*Ptr++)<<8);
    Data = Data |*Ptr++;
    return(Data);
}




Uint32 fab(int32 x)
{
    if(x < 0)
    {
        return -x;
    }
    else
    {
        return x;
    }
}
int32 Get_Middle_Value(void)
{
    int i,j;
    int32 temp = 0;
    if(Middle_Cnt >= 5)
    {
        for(i= 0; i < 3; i++)
        {
            for(j = 0; j < 4-i; j++)
            {
                if(Volt_1S_Array[Middle_Cnt-5+j] > Volt_1S_Array[Middle_Cnt-4+j])
                {
                    temp = Volt_1S_Array[Middle_Cnt-4+j];
                    Volt_1S_Array[Middle_Cnt-4+j] = Volt_1S_Array[Middle_Cnt-5+j];
                    Volt_1S_Array[Middle_Cnt-5+j] = temp;
                }
            }
        }
        return Volt_1S_Array[Middle_Cnt-3];
    }
    else
    {
        /*temp = 0;
        for(i = 0; i < Middle_Cnt; i++)
        {
            temp += Volt_1S_Array[i];
        }
        temp = temp/Middle_Cnt;*/
        return Data_Sum.Vol_Sense;
    }
}
void Updata_Mutation_Check(void)
{
    int cnt = 0;
    if((DCDC_Service.Now_Mode==DCDC_CC )||((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==FALSE))||
    (DCDC_Service.Now_Mode==DCDC_DCC)||((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==FALSE)))
    {
        if(fab(Image_UpData.Record.dwOutPutCurrentNew_UP - DCDC_Service.Set_Cur_Total) > 10)
        {
            if(Middle_Cnt >= 1)
            {
                for(cnt = Middle_Cnt-1; cnt>= 0; cnt--)
                {
                    if(fab(Cur_1S_Array[cnt] - DCDC_Service.Set_Cur_Total) < 100)
                    {
                        Image_UpData.Record.dwOutPutCurrentNew_UP = DCDC_Service.Set_Cur_Total;
						Image_Protect.Record.dwOutPutCurrentNew_UP = Image_UpData.Record.dwOutPutCurrentNew_UP;
                        break;
                     }
                 }
            }
        }
    }
    else if(((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==TRUE))||((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==TRUE)))
    {
        if(fab(Image_UpData.Record.wBatSenseAVoltNew_UP - DCDC_Service.Set_Vol) >= 2)
        {
            if(Middle_Cnt >= 1)
            {
                for(cnt = Middle_Cnt-1; cnt>= 0; cnt--)
                {
                   if(fab(Volt_1S_Array[cnt] - DCDC_Service.Set_Vol) < 10)
                   {
                       Image_UpData.Record.wBatSenseAVoltNew_UP = DCDC_Service.Set_Vol;
					   Image_Protect.Record.wBatSenseAVoltNew_UP = Image_UpData.Record.wBatSenseAVoltNew_UP;
                       break;
                    }
                }
            }
        }
    }
}
int32 Voltage_Muta_Check(int32 Value)
{
    Elimin_Vol_Sense.Data_Update_cnt++;
    if((WorkStep_Data.Type == cStepStatic)&&(Filter_Workstep_Last.Voltage_Trend != 0))//充放电后的静置2min后稳定
    {
        if(Elimin_Vol_Sense.Data_Update_cnt > 120)
        {
            Filter_Workstep_Last.Voltage_Trend = 0;
        }
    }
    Elimin_Vol_Sense.Data_Out = Value;
    Elimin_Vol_Sense.Delta_Now = Value - Elimin_Vol_Sense.Data_Last1;
    Elimin_Vol_Sense.Delta_Last = Elimin_Vol_Sense.Data_Last1 - Elimin_Vol_Sense.Data_Last2;
    Elimin_Vol_Sense.Delta_Last1 = Elimin_Vol_Sense.Data_Last2 - Elimin_Vol_Sense.Data_Last3;
    if(Elimin_Vol_Sense.Data_Update_cnt >= 4)
    {
        if((DCDC_Service.Flag_DCV_Step!=TRUE)&&(DCDC_Service.Flag_CV_Step !=TRUE))
        {
            if(Filter_Workstep_Last.Voltage_Trend == 0)
            {
                if((fab(Elimin_Vol_Sense.Delta_Now) > Elimin_Vol_Sense.Elimin_Abs_Min)&&(Elimin_Vol_Sense.Delta_Now < Elimin_Vol_Sense.Elimin_Abs_Max)&&
                (fab(Elimin_Vol_Sense.Delta_Last) <= Elimin_Vol_Sense.Elimin_Abs_Min))//静态波动太大
                {
                    Elimin_Vol_Sense.Data_Out = Get_Middle_Value();
                }
            }
            else if(Filter_Workstep_Last.Voltage_Trend == 1)
            {
                if((Elimin_Vol_Sense.Delta_Now <-Elimin_Vol_Sense.Elimin_Abs_Min)&&((Value - Elimin_Vol_Sense.Data_Last2) < 0))//上升趋势发现负偏
                {
                    if(Elimin_Vol_Sense.Delta_Now > -Elimin_Vol_Sense.Elimin_Abs_Max)
                    {
                        Elimin_Vol_Sense.Data_Out = Get_Middle_Value();
                    }
                }
                //上升中上升过快
                else if((Elimin_Vol_Sense.Delta_Now > 0)&&(Elimin_Vol_Sense.Delta_Last >= 0)&&(Elimin_Vol_Sense.Delta_Now - Elimin_Vol_Sense.Delta_Last > Elimin_Vol_Sense.Elimin_Abs_Min))
                {
                    if(Elimin_Vol_Sense.Delta_Now < Elimin_Vol_Sense.Elimin_Abs_Max)
                    {
                        Elimin_Vol_Sense.Data_Out = Get_Middle_Value();
                    }
                }
            }
            else if(Filter_Workstep_Last.Voltage_Trend == 2)
            {
                if(DCDC_Service.Flag_DCV_Step!=TRUE)
                {
                    if((Elimin_Vol_Sense.Delta_Now > Elimin_Vol_Sense.Elimin_Abs_Min)&&((Value - Elimin_Vol_Sense.Data_Last2) >= 0))//下降趋势发现正偏
                    {
                        Elimin_Vol_Sense.Data_Out = Get_Middle_Value();
                    }
                    else if((Elimin_Vol_Sense.Delta_Now < 0)&&(Elimin_Vol_Sense.Delta_Last <= 0)&&(Elimin_Vol_Sense.Delta_Now - Elimin_Vol_Sense.Delta_Last < -Elimin_Vol_Sense.Elimin_Abs_Min))
                    {
                        Elimin_Vol_Sense.Data_Out = Get_Middle_Value();
                    }
                }
            }
        }
    }
    else
    {
        Elimin_Vol_Sense.Data_Out = Get_Middle_Value();
    }
    Elimin_Vol_Sense.Data_Last3 = Elimin_Vol_Sense.Data_Last2;
    Elimin_Vol_Sense.Data_Last2 = Elimin_Vol_Sense.Data_Last1;
    Elimin_Vol_Sense.Data_Last1 = Elimin_Vol_Sense.Data_Out;
    return Elimin_Vol_Sense.Data_Out;



}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Median_Data(struct_Filter_Median *Ptr)
{
    Ptr->Data[Ptr->Cnt]=Ptr->Data_In;
    if(++Ptr->Cnt>=5)
    {
        Uint16 Cnt1=0,Cnt2=0;
        Ptr->Cnt=0;
        int32 Data=0;
        for(Cnt2=4;Cnt2>=1;Cnt2--)
        {
            for(Cnt1=1;Cnt1<=Cnt2;Cnt1++)
            {
                if(Ptr->Data[Cnt1-1]>Ptr->Data[Cnt1])
                {
                    Data=Ptr->Data[Cnt1];
                    Ptr->Data[Cnt1]=Ptr->Data[Cnt1-1];
                    Ptr->Data[Cnt1-1]=Data;
                }
            }
        }
        Ptr->Data_Out=Ptr->Data[2];
    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Median_Data_3(struct_Filter_Median *Ptr)
{
    Ptr->Data[Ptr->Cnt]=Ptr->Data_In;
    if(++Ptr->Cnt>=3)
    {
        Uint16 Cnt1=0,Cnt2=0;
        Ptr->Cnt=0;
        int32 Data=0;
        for(Cnt2=2;Cnt2>=1;Cnt2--)
        {
            for(Cnt1=1;Cnt1<=Cnt2;Cnt1++)
            {
                if(Ptr->Data[Cnt1-1]>Ptr->Data[Cnt1])
                {
                    Data=Ptr->Data[Cnt1];
                    Ptr->Data[Cnt1]=Ptr->Data[Cnt1-1];
                    Ptr->Data[Cnt1-1]=Data;
                }
            }
        }
        Ptr->Data_Out=Ptr->Data[1];
    }
}

void Moving_Average(struct_Filter_Median *Ptr,Uint16 Num_Aver)
{
    Uint16 Cnt1;
     int32 Sum=0;
    for(Cnt1=9;Cnt1>=1;Cnt1--)
    {
        Ptr->Data[Cnt1]=Ptr->Data[Cnt1-1];
    }
    Ptr->Data[0]=Ptr->Data_In;

    for(Cnt1=0;Cnt1<Num_Aver;Cnt1++)
    {
        Sum=Sum+Ptr->Data[Cnt1];
    }

    Ptr->Data_Out=Sum/Num_Aver;
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Process_Average_Analog(void)
{
    int32 Temp=0;
    if(Data_Sum.Flag_Refresh == TRUE)
    {
        Data_Sum.Cur_CHA    = Data_Sum.Sum_Cur_CHA_Copy/Data_Sum.Num_Avg;
        if(Data_Sum.Cur_CHA<0)
        {Data_Sum.Cur_CHA=-Data_Sum.Cur_CHA;}


        Data_Sum.Cur_CHB    = Data_Sum.Sum_Cur_CHB_Copy/Data_Sum.Num_Avg;
        if(Data_Sum.Cur_CHB<0)
        {Data_Sum.Cur_CHB=-Data_Sum.Cur_CHB;}

        Median_Vol_Sense.Data_In=Data_Sum.Sum_Vol_Sense_Copy/Data_Sum.Num_Avg;
        Median_Vol_Out.Data_In  =Data_Sum.Sum_Vol_Out_Copy/Data_Sum.Num_Avg;


        Data_Sum.Vol_Bus    = Data_Sum.Sum_Vol_Bus_Copy/Data_Sum.Num_Avg;
        Data_Sum.AD_PT      = Data_Sum.Sum_AD_PT_Copy/Data_Sum.Num_Avg;
        Data_Sum.AD_NTC1    = Data_Sum.Sum_AD_NTC1_Copy/Data_Sum.Num_Avg;
        Data_Sum.AD_NTC2    = Data_Sum.Sum_AD_NTC2_Copy/Data_Sum.Num_Avg;
        Data_Sum.AD_NTC_BATB=Data_Sum.Sum_AD_NTC_BATB_Copy/Data_Sum.Num_Avg;

        DINT;
        Data_Sum.Flag_Refresh       = FALSE;
        EINT;

        Median_Data(&Median_Vol_Sense);
        Data_Sum.Vol_Sense          = Median_Vol_Sense.Data_Out;
        //Data_Sum.Vol_Sense          = Median_Vol_Sense.Data_In;

        Median_Data(&Median_Vol_Out);
        Data_Sum.Vol_Out            = Median_Vol_Out.Data_Out;
        //Data_Sum.Vol_Out            = Median_Vol_Out.Data_In;

        //工步转换后1S快速更新数据
        if(DCDC_Service.Cnt_FastRefresh<15)
        {
            DCDC_Service.Cnt_FastRefresh++;
            Data_Sum.Vol_Sense  = Median_Vol_Sense.Data_In;
            Data_Sum.Vol_Out    = Median_Vol_Out.Data_In;
        }

        #if VER_HARDWARE == C017D || VER_HARDWARE == C017D_
        Data_Sum.Vol_5V=((int32)Data_Sum.AD_NTC_BATB*50000)>>14;
        #endif

        if((CH_A.EN_Run==TRUE)&&(CH_A.EN_Run_Comm==TRUE))
        {
            Temp +=Data_Sum.Cur_CHA;
        }

        if((CH_B.EN_Run==TRUE)&&(CH_B.EN_Run_Comm==TRUE))
        {
            Temp +=Data_Sum.Cur_CHB;
        }

        DINT;
        DCDC_Service.Now_Cur_AB=Temp;
        EINT;

        if(DCDC_Service.Mode_Parall==PARALL_MASTER)
        {
            Temp  = 0;
            Temp += DCDC_Service.Now_Cur_AB;

            if(DCDC_Service.Flag_SingleCH==FALSE)
            {
                Temp += Parall_Master.Now_Cur_S1;

                if(DCDC_Service.Parall_Total>=3)
                {
                    Temp += Parall_Master.Now_Cur_S2;
                }

                if(DCDC_Service.Parall_Total>=4)
                {
                    Temp += Parall_Master.Now_Cur_S3;
                }

                if(DCDC_Service.Parall_Total>=5)
                {
                    Temp += Parall_Master.Now_Cur_S4;
                }
            }
        }
        else
        {
            Temp  = DCDC_Service.Now_Cur_AB;
        }

        //单机模式或主机模式
        /*if((DCDC_Service.Mode_Parall==PARALL_MASTER)||(DCDC_Service.Mode_Parall==SINGLE))
        {
            int32 Delta;

            //恒流阶段实际电流值与目标值差值的绝对值在1mA~300mA之间的数据进行替换
            if((DCDC_Service.Now_Mode==DCDC_CC )||((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==FALSE))||
               (DCDC_Service.Now_Mode==DCDC_DCC)||((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==FALSE)))
            {
                Delta=Temp-DCDC_Service.Set_Cur_Total;
                if(Delta<0)
                {
                    Delta=-Delta;
                }

                if((Delta<=3000)&&(Delta>=10)&&(Interface_DCDC.Req_Aging==FALSE))
                {
                    Temp=DCDC_Service.Set_Cur_Total;
                }
            }

            if(((DCDC_Service.Now_Mode==DCDC_CCtoCV  )&&(DCDC_Service.Flag_CV_Step ==TRUE))||
               ((DCDC_Service.Now_Mode==DCDC_DCCtoDCV)&&(DCDC_Service.Flag_DCV_Step==TRUE)))
            {
                //CV阶段电流变化率计算
                Rate_Cur_Compute.Data_In=Temp;

                Rate_Cur_Compute.Rate_Cur_Now=Rate_Cur_Compute.Data_In-Rate_Cur_Compute.Data_Cur[0];

                if(Rate_Cur_Compute.Cnt_Delay<20)//2S
                {
                    Rate_Cur_Compute.Cnt_Delay++;
                    Rate_Cur_Compute.Rate_Cur_Filter=0;
                }
                else if(Rate_Cur_Compute.Cnt_Delay<60)//6S
                {
                    Rate_Cur_Compute.Cnt_Delay++;

                    Rate_Cur_Compute.Rate_Cur_1S=Rate_Cur_Compute.Data_In-Rate_Cur_Compute.Data_Cur[9];

                    Rate_Cur_Compute.Rate_Cur_Filter=(Rate_Cur_Compute.Rate_Cur_Filter*124+Rate_Cur_Compute.Rate_Cur_1S*4)>>7;

                    Rate_Cur_Compute.Rate_Cur_Out=Rate_Cur_Compute.Rate_Cur_Filter/10;

                    Rate_Cur_Compute.Rate_Max_Expect=Rate_Cur_Compute.Rate_Cur_Out+500;

                    if(Rate_Cur_Compute.Rate_Max_Expect>100)
                    {
                        Rate_Cur_Compute.Rate_Max_Expect=100;
                    }

                    Rate_Cur_Compute.Rate_Min_Expect=Rate_Cur_Compute.Rate_Cur_Out-500;
                }
                else
                {
                    if((Rate_Cur_Compute.Rate_Cur_Now>Rate_Cur_Compute.Rate_Max_Expect)||(Rate_Cur_Compute.Rate_Cur_Now<Rate_Cur_Compute.Rate_Min_Expect))
                    {
                        if(Rate_Cur_Compute.Cnt_Error<5)
                        {
                            Rate_Cur_Compute.Data_In=Rate_Cur_Compute.Data_Cur[0]+Rate_Cur_Compute.Rate_Cur_Out;//修正变化率计算用数据
                            Temp=Rate_Cur_Compute.Data_In;//修正上层使用的数据

                            Rate_Cur_Compute.Rate_Cur_1S=Rate_Cur_Compute.Data_In-Rate_Cur_Compute.Data_Cur[9];//修正变化率,保持前面的变化率

                            Rate_Cur_Compute.Rate_Cur_Filter=(Rate_Cur_Compute.Rate_Cur_Filter*124+Rate_Cur_Compute.Rate_Cur_1S*4)>>7;

                            Rate_Cur_Compute.Rate_Cur_Out=Rate_Cur_Compute.Rate_Cur_Filter/10;

                            Rate_Cur_Compute.Rate_Max_Expect=Rate_Cur_Compute.Rate_Cur_Out+500;

                            if(Rate_Cur_Compute.Rate_Max_Expect>100)
                            {
                                Rate_Cur_Compute.Rate_Max_Expect=100;
                            }

                            Rate_Cur_Compute.Rate_Min_Expect=Rate_Cur_Compute.Rate_Cur_Out-500;

                            Rate_Cur_Compute.Cnt_Error++;
                            Rate_Cur_Compute.Cnt_Error_Reset=0;
                        }
                        else
                        {
                            Rate_Cur_Compute.Cnt_Error=0;
                            Rate_Cur_Compute.Cnt_Error_Reset=0;
                            Rate_Cur_Compute.Cnt_Delay=0;
                        }
                    }
                    else
                    {
                        Rate_Cur_Compute.Rate_Cur_1S=Rate_Cur_Compute.Data_In-Rate_Cur_Compute.Data_Cur[9];

                        Rate_Cur_Compute.Rate_Cur_Filter=(Rate_Cur_Compute.Rate_Cur_Filter*124+Rate_Cur_Compute.Rate_Cur_1S*4)>>7;

                        Rate_Cur_Compute.Rate_Cur_Out=Rate_Cur_Compute.Rate_Cur_Filter/10;

                        Rate_Cur_Compute.Rate_Max_Expect=Rate_Cur_Compute.Rate_Cur_Out+500;

                        if(Rate_Cur_Compute.Rate_Max_Expect>100)
                        {
                            Rate_Cur_Compute.Rate_Max_Expect=100;
                        }

                        Rate_Cur_Compute.Rate_Min_Expect=Rate_Cur_Compute.Rate_Cur_Out-500;

                        Rate_Cur_Compute.Cnt_Error=0;
                        Rate_Cur_Compute.Cnt_Error_Reset=0;
                    }
                }

                //
                for(Delta=10;Delta>=1;Delta--)
                {
                    Rate_Cur_Compute.Data_Cur[Delta]=Rate_Cur_Compute.Data_Cur[Delta-1];
                }
                Rate_Cur_Compute.Data_Cur[0]=Rate_Cur_Compute.Data_In;
            }


            //4个数据的滑动平均滤波
            //Median_Cur_Total.Data_In=Temp;
            //Moving_Average(&Median_Cur_Total,4);
            //Temp=Median_Cur_Total.Data_Out;
        }*/

        DINT;
        Parall_Master.Now_Cur_Total = Temp;
        Data_Sum.Vol_Contact    = Parall_Master.Now_Sense_S1;
        Data_Sum.Temp_Shunt_S1  = Parall_Master.IDx1_Fr_S1.IDx_L.IDx1_L.Temp_Shunt;
        Data_Sum.Temp_Sink_S1   = Parall_Master.IDx1_Fr_S1.IDx_L.IDx1_L.Temp_Sink;
        EINT;
        Res_Contact_Compute();
        Res_Loop_Compute();
	}
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Cali_PT_5V(void)
{
    int32 Vol_PT;
    int32 R_PT=10974;
    Vol_PT=(Data_Sum.AD_PT*50000)>>15;//0.1mV/LSB
    Vol_PT=(Vol_PT*10000+R_PT*Vol_PT/20)/R_PT+Vol_PT;
    if((Vol_PT>=48000)&&(Vol_PT<=52000))
    {
        Data_Sum.Vol_5V=Vol_PT;
    }
    else
    {
        Data_Sum.Vol_5V=50000;
    }
    EEPROM_Write_Uint16(cEepromCaliVolPT,Data_Sum.Vol_5V);
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
const INT16U uwBatNTCResTable[16] = {10000,10390,10780,11167,11554,11940,12325,12708,13090,13471,13851,14229,14607,14983,15358,15733};
const INT16U uwBatNTCResDelTable[16] = {0,390,390,387,387,386,385,383,382,381,380,378,378,376,375,375};
INT16U sBatTemperatureRealCal(INT32S dwBatTempSampleAvg)
{
    INT8U R;
    INT16U uwBatNTCResistance;
    //INT32S dwBatNTCResistanceTemp;
    INT32U udwBatNTCDegree;
    INT16U uwBatteryDegreeNew;
    //dwBatNTCResistanceTemp = 32984-((dwBatTempSampleAvg*1076)>>10);
    //uwBatNTCResistance = (dwBatTempSampleAvg*10000)/dwBatNTCResistanceTemp;

    int32 Vol_PT=0;

    Vol_PT=(dwBatTempSampleAvg*50000)>>15;//0.1mV/LSB

    uwBatNTCResistance=(int32)(Vol_PT*10000/(Data_Sum.Vol_5V-Vol_PT-Vol_PT/20));//0.1Ω/LSB


    for(R=0;R<16;R++)
    {
        if(uwBatNTCResTable[R]>uwBatNTCResistance)
        {
            break;
        }
    }
    if(R == 0)
    {
        uwBatteryDegreeNew = 0;         //Min Temperature
    }
    else if(R >= 16)
    {
        uwBatteryDegreeNew = 1500;      //Max Temperature
    }
    else
    {
        udwBatNTCDegree = (uwBatNTCResistance-uwBatNTCResTable[R-1])*100;
        udwBatNTCDegree = udwBatNTCDegree/uwBatNTCResDelTable[R];
        udwBatNTCDegree = udwBatNTCDegree+(R-1)*100;
        uwBatteryDegreeNew = (INT16U)udwBatNTCDegree;
    }
    return(uwBatteryDegreeNew);
}


//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
const INT16U uwNTCADCTable_B3380[79]=
{		341,
		355,
		369,
		384,
		400,
		415,
		432,
		448,
		465,
		483,
		501,
		520,
		539,
		559,
		579,
		599,
		621,
		642,
		665,
		687,
		711,
		734,
		759,
		784,
		809,
		835,
		862,
		889,
		916,
		945,
		973,
		1002,
		1032,
		1062,
		1093,
		1125,
		1156,
		1189,
		1222,
		1255,
		1289,
		1323,
		1358,
		1393,
		1429,
		1465,
		1501,
		1538,
		1576,
		1613,
		1652,
		1690,
		1729,
		1768,
		1808,
		1848,
		1888,
		1929,
		1969,
		2010,
		2052,
		2093,
		2135,
		2177,
		2219,
		2261,
		2304,
		2347,
		2389,
		2432,
		2475,
		2518,
		2561,
		2604,
		2648,
		2691,
		2734,
		2777,
		2820
};

INT16U sTemperature1SampleAvgCal_B3380(INT16U uwChannel1NTCSample)
{
    INT8U i;
    INT16U uwChannel1NTCSampleTemp;
    if(uwChannel1NTCSample < uwNTCADCTable_B3380[0])
    {
        uwChannel1NTCSampleTemp = 90;
    }
    else if(uwChannel1NTCSample > uwNTCADCTable_B3380[78])
    {
        uwChannel1NTCSampleTemp = 890;
    }
    else
    {
       if(uwChannel1NTCSample > uwNTCADCTable_B3380[39])
       {
           i = 39;
           while(uwChannel1NTCSample > uwNTCADCTable_B3380[i])
           {
              i++;
           }
           uwChannel1NTCSampleTemp = (i + 9)*10;
       }
       else
       {
           i = 0;
           while(uwChannel1NTCSample > uwNTCADCTable_B3380[i])
           {
              i++;
           }
           uwChannel1NTCSampleTemp = (i + 9)*10;

        }
   }
   return(uwChannel1NTCSampleTemp);
}


const INT16U uwNTCADCTable[79]={
668,
695,
722,
749,
778,
807,
836,
867,
898,
930,
963,
996,
1031,
1065,
1101,
1137,
1174,
1212,
1250,
1289,
1329,
1369,
1410,
1452,
1494,
1537,
1580,
1624,
1669,
1714,
1759,
1805,
1852,
1899,
1946,
1994,
2042,
2090,
2139,
2188,
2237,
2287,
2337,
2387,
2437,
2487,
2538,
2588,
2639,
2690,
2741,
2791,
2842,
2893,
2944,
2994,
3045,
3095,
3145,
3195,
3245,
3294,
3344,
3393,
3441,
3490,
3538,
3586,
3633,
3681,
3727,
3774,
3820,
3865,
3911,
3955,
4000,
4044,
4087};

INT16U sTemperature1SampleAvgCal(INT16U uwChannel1NTCSample)
{
    INT8U i;
    INT16U uwChannel1NTCSampleTemp;
    if(uwChannel1NTCSample < uwNTCADCTable[0])
    {
        uwChannel1NTCSampleTemp = 90;
    }
    else if(uwChannel1NTCSample > uwNTCADCTable[78])
    {
        uwChannel1NTCSampleTemp = 890;
    }
    else
    {
       if(uwChannel1NTCSample > uwNTCADCTable[39])
       {
           i = 39;
           while(uwChannel1NTCSample > uwNTCADCTable[i])
           {
              i++;
           }
           uwChannel1NTCSampleTemp = (i + 9)*10;
       }
       else
       {
           i = 0;
           while(uwChannel1NTCSample > uwNTCADCTable[i])
           {
              i++;
           }
           uwChannel1NTCSampleTemp = (i + 9)*10;

        }
   }
   return(uwChannel1NTCSampleTemp);
}


void StepEnd_TimeSet(Uint16 offset)
{
      WorkStep_Data.StepEnd_Time.End_EN           = TRUE;
      if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[offset+1]==cStepGreater)
      {
           WorkStep_Data.StepEnd_Time.End_Logic    = cStepGreater;
      }
      else
      {
           WorkStep_Data.StepEnd_Time.End_Logic    = cStepLess;
      }

      WorkStep_Data.StepEnd_Time.End_Value        = Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],(offset+2));
      WorkStep_Data.StepEnd_Time.End_Result       = Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[offset+6];
      WorkStep_Data.StepEnd_Time.End_Jump_Index   = Char2Uint16(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],(offset+7));
      WorkStep_Data.StepEnd_Time.Cnt_Mask         = 0;
      WorkStep_Data.StepEnd_Time.Num_Mask         = 0;
}
void StepEnd_VolSet(Uint16 offset)
{
      WorkStep_Data.StepEnd_Vol.End_EN            = TRUE;
      if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[offset+1]==cStepGreater)
      {
             WorkStep_Data.StepEnd_Vol.End_Logic     = cStepGreater;
      }
      else
      {
             WorkStep_Data.StepEnd_Vol.End_Logic     = cStepLess;
      }

      WorkStep_Data.StepEnd_Vol.End_Value         = Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],(offset+2));
      WorkStep_Data.StepEnd_Vol.End_Result        = Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[offset+6];
      WorkStep_Data.StepEnd_Vol.End_Jump_Index    = Char2Uint16(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],(offset+7));
      WorkStep_Data.StepEnd_Vol.Cnt_Mask          = 0;
      WorkStep_Data.StepEnd_Vol.Num_Mask          = 0;
}

void StepEnd_CurSet(Uint16 offset)
{
      WorkStep_Data.StepEnd_Cur.End_EN            = TRUE;
      if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[offset+1]==cStepGreater)
      {
           WorkStep_Data.StepEnd_Cur.End_Logic     = cStepGreater;
      }
      else
      {
           WorkStep_Data.StepEnd_Cur.End_Logic     = cStepLess;
      }
      WorkStep_Data.StepEnd_Cur.End_Value         = Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],(offset+2));
      WorkStep_Data.StepEnd_Cur.End_Result        = Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[offset+6];
      WorkStep_Data.StepEnd_Cur.End_Jump_Index    = Char2Uint16(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],(offset+7));
      WorkStep_Data.StepEnd_Cur.Cnt_Mask          = 0;
      WorkStep_Data.StepEnd_Cur.Num_Mask          = 40;//25;//10;
}
void StepEnd_AhSet(Uint16 offset)
{
      WorkStep_Data.StepEnd_Ah.End_EN             = TRUE;

      if(Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[offset+1]==cStepGreater)
      {
           WorkStep_Data.StepEnd_Ah.End_Logic      = cStepGreater;
      }
      else
      {
           WorkStep_Data.StepEnd_Ah.End_Logic      = cStepLess;
      }
      WorkStep_Data.StepEnd_Ah.End_Value          = Char2Uint32(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],(offset+2))*10;
      WorkStep_Data.StepEnd_Ah.End_Result         = Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[offset+6];
      WorkStep_Data.StepEnd_Ah.End_Jump_Index     = Char2Uint16(&Interface_SPI_Flash[SERV_FLASH_WORKSTEP].Data[0],(offset+7));
      WorkStep_Data.StepEnd_Ah.Cnt_Mask           = 0;
      WorkStep_Data.StepEnd_Ah.Num_Mask           = 0;
}


//File end
