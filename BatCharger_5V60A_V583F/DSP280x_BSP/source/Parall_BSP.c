/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: Parall_BSP.c
// Created on: 2020-3-19
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义
struct_Parall_Slaver Parall_Slaver;
struct_Parall_Master Parall_Master;
    struct ECAN_REGS ECanaShadow;

//函数声明


//函数定义
void Parall_Service(void)
{
    if(DCDC_Service.Mode_Parall==PARALL_MASTER)
    {
        //主机广播自己的地址及并机总数量
        if(Parall_Master.ID0_To_Sx.Req_TX==TRUE)
        {
            //判断发送是否可用
            ECanaShadow.CANTA.all=ECanaRegs.CANTA.all;
            if((Parall_Master.ID0_To_Sx.Flag_First_TX==TRUE)||(ECanaShadow.CANTA.bit.TA0==1))
            {
                Parall_Master.ID0_To_Sx.Flag_First_TX=FALSE;
                Parall_Master.ID0_To_Sx.Req_TX=FALSE;

                //装载数据放到请求发送的地方
                ECanaMboxes.MBOX0.MDL.all=Parall_Master.ID0_To_Sx.ID0_L.All;
                ECanaMboxes.MBOX0.MDH.all=Parall_Master.ID0_To_Sx.ID0_H.All;
                //启动MBOX0的发送
                ECanaShadow.CANTRS.all=ECanaRegs.CANTRS.all;
                ECanaShadow.CANTRS.bit.TRS0 = 1;
                ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;
            }

        }

        //主机发往从机的心跳包
        if(++Parall_Master.Cnt_Period>=50)//1mS
        {
            Parall_Master.Cnt_Period=0;
            Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Refresh_Cur=TRUE;
            Parall_Master.ID1_To_Sx.Req_TX=TRUE;
        }


        if(Parall_Master.ID1_To_Sx.Req_TX==TRUE)
        {
            //判断发送是否可用
            ECanaShadow.CANTRS.all=ECanaRegs.CANTRS.all;
            if((Parall_Master.ID1_To_Sx.Flag_First_TX==TRUE)||(ECanaShadow.CANTRS.bit.TRS1==0))
            {
            	Parall_Master.ID1_To_Sx.Flag_First_TX=FALSE;
                Parall_Master.ID1_To_Sx.Req_TX=FALSE;

                //心跳包发送计数器清零
                Parall_Master.Cnt_Period=0;

               // if(Parall_Master.ID1_To_Sx.Cnt_TX_Req_Change_Mode>0)
               // {
               //     Parall_Master.ID1_To_Sx.Cnt_TX_Req_Change_Mode--;
                //    Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_ChangeMode   =TRUE;
                //}

                Parall_Master.ID1_To_Sx.ID1_L.Bit.Now_Mode_Master=DCDC_Service.Now_Mode;



                Parall_Master.ID1_To_Sx.Req_TX=FALSE;
                ECanaMboxes.MBOX1.MDL.all=Parall_Master.ID1_To_Sx.ID1_L.All;
                ECanaMboxes.MBOX1.MDH.all=Parall_Master.ID1_To_Sx.ID1_H.All;
                //启动MBOX1的发送

                ECanaRegs.CANTRS.bit.TRS1 = 1;

                //发送完成后清除相关请求标志
                Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Fault_Clear   = FALSE;
                Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_ChangeMode    = FALSE;
                Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Refresh_Cur   = FALSE;
            }
        }

        //主机接收
        if(Fault_Code.Cnt_S2M_Fault_Mask<100)
        {
            Fault_Code.Cnt_S2M_Fault_Mask++;
        }


        ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
        if(ECanaShadow.CANRMP.bit.RMP18==1)
        {
            Parall_Master.IDx_Fr_S1.IDx_L.All=ECanaMboxes.MBOX18.MDL.all;
            Parall_Master.IDx_Fr_S1.IDx_H.All=ECanaMboxes.MBOX18.MDH.all;
            Parall_Master.IDx_Fr_S1.Flag_RX=TRUE;

            ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
            ECanaShadow.CANRMP.bit.RMP18=1;
            ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;

            Parall_Master.Cnt_M_OT_S1=0;
            Parall_Master.Now_Cur_S1=Parall_Master.IDx_Fr_S1.IDx_H.Now_Cur_AB;
            if(Parall_Master.IDx_Fr_S1.IDx_L.Bit.Fault_Module!=0)
            {
                if(Fault_Code.Cnt_S2M_Fault_Mask>=100)
                {
                    DCDC_Service.Fault_Slaver1 = Parall_Master.IDx_Fr_S1.IDx_L.Bit.Fault_Module;
                   // DCDC_Service.Fault_Module |= FAULT_SLAVER1;
                }
            }
        }



        if(ECanaShadow.CANRMP.bit.RMP19==1)
        {
            Parall_Master.IDx_Fr_S2.IDx_L.All=ECanaMboxes.MBOX19.MDL.all;
            Parall_Master.IDx_Fr_S2.IDx_H.All=ECanaMboxes.MBOX19.MDH.all;
            Parall_Master.IDx_Fr_S2.Flag_RX=TRUE;

            ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
            ECanaShadow.CANRMP.bit.RMP19=1;
            ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;

            Parall_Master.Cnt_M_OT_S2=0;
            Parall_Master.Now_Cur_S2=Parall_Master.IDx_Fr_S2.IDx_H.Now_Cur_AB;
            if(Parall_Master.IDx_Fr_S2.IDx_L.Bit.Fault_Module!=0)
            {
                if(Fault_Code.Cnt_S2M_Fault_Mask>=100)
                {
                    DCDC_Service.Fault_Slaver2 = Parall_Master.IDx_Fr_S2.IDx_L.Bit.Fault_Module;
                    //DCDC_Service.Fault_Module |= FAULT_SLAVER2;
                }
            }
        }

        if(ECanaShadow.CANRMP.bit.RMP20==1)
        {
            Parall_Master.IDx_Fr_S3.IDx_L.All=ECanaMboxes.MBOX20.MDL.all;
            Parall_Master.IDx_Fr_S3.IDx_H.All=ECanaMboxes.MBOX20.MDH.all;
            Parall_Master.IDx_Fr_S3.Flag_RX=TRUE;

            ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
            ECanaShadow.CANRMP.bit.RMP20=1;
            ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;

            Parall_Master.Cnt_M_OT_S3=0;
            Parall_Master.Now_Cur_S3=Parall_Master.IDx_Fr_S3.IDx_H.Now_Cur_AB;
            if(Parall_Master.IDx_Fr_S3.IDx_L.Bit.Fault_Module!=0)
            {
                if(Fault_Code.Cnt_S2M_Fault_Mask>=100)
                {
                    DCDC_Service.Fault_Slaver3 = Parall_Master.IDx_Fr_S3.IDx_L.Bit.Fault_Module;
                   // DCDC_Service.Fault_Module |= FAULT_SLAVER3;
                }
            }
        }

        if(ECanaShadow.CANRMP.bit.RMP21==1)
        {
            Parall_Master.IDx_Fr_S4.IDx_L.All=ECanaMboxes.MBOX21.MDL.all;
            Parall_Master.IDx_Fr_S4.IDx_H.All=ECanaMboxes.MBOX21.MDH.all;
            Parall_Master.IDx_Fr_S4.Flag_RX=TRUE;

            ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
            ECanaShadow.CANRMP.bit.RMP21=1;
            ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;

            Parall_Master.Cnt_M_OT_S4=0;
            Parall_Master.Now_Cur_S4=Parall_Master.IDx_Fr_S4.IDx_H.Now_Cur_AB;
            if(Parall_Master.IDx_Fr_S4.IDx_L.Bit.Fault_Module!=0)
            {
                if(Fault_Code.Cnt_S2M_Fault_Mask>=100)
                {
                    DCDC_Service.Fault_Slaver4 = Parall_Master.IDx_Fr_S4.IDx_L.Bit.Fault_Module;
                   // DCDC_Service.Fault_Module |= FAULT_SLAVER4;
                }
            }
        }

        if(ECanaShadow.CANRMP.bit.RMP22==1)
        {
            Parall_Master.IDx1_Fr_S1.IDx_L.All	= ECanaMboxes.MBOX22.MDL.all;
            Parall_Master.IDx1_Fr_S1.IDx_H.All	= ECanaMboxes.MBOX22.MDH.all;
            Parall_Master.IDx1_Fr_S1.Flag_RX	= TRUE;

            ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
            ECanaShadow.CANRMP.bit.RMP22=1;
            ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;


            Parall_Master.Now_Sense_S1=Parall_Master.IDx1_Fr_S1.IDx_H.Now_Vol_Sense;

        }

        //CAN通讯中断超时时间100mS
        if(DCDC_Service.Parall_Total>=2)
        {
            if((DCDC_Service.Fault_Module&FAULT_S1_CAN)==0)
            {
                if(++Parall_Master.Cnt_M_OT_S1>=10000)
                {
                    DCDC_Service.Fault_Module |= FAULT_S1_CAN;
                    Parall_Master.Cnt_M_OT_S1=0;
                }
            }
        }

        if(DCDC_Service.Parall_Total>=3)
        {
            if((DCDC_Service.Fault_Module&FAULT_S2_CAN)==0)
            {
                if(++Parall_Master.Cnt_M_OT_S2>=10000)
                {
                    DCDC_Service.Fault_Module |= FAULT_S2_CAN;
                    Parall_Master.Cnt_M_OT_S2=0;
                }
            }
        }

        if(DCDC_Service.Parall_Total>=4)
        {
            if((DCDC_Service.Fault_Module&FAULT_S3_CAN)==0)
            {
                if(++Parall_Master.Cnt_M_OT_S3>=10000)
                {
                    DCDC_Service.Fault_Module |= FAULT_S3_CAN;
                    Parall_Master.Cnt_M_OT_S3=0;
                }
            }
        }

        if(DCDC_Service.Parall_Total>=5)
        {
            if((DCDC_Service.Fault_Module&FAULT_S4_CAN)==0)
            {
                if(++Parall_Master.Cnt_M_OT_S4>=10000)
                {
                    DCDC_Service.Fault_Module |= FAULT_S4_CAN;
                    Parall_Master.Cnt_M_OT_S4=0;
                }
            }
        }
    }
    else if(DCDC_Service.Mode_Parall==PARALL_SLAVER)
    {
        //从机发送数据,也是从机发给主机的心跳包
        if(++Parall_Slaver.Cnt_Period>=200)//20mS
        {
            Parall_Slaver.Cnt_Period=0;
            Parall_Slaver.IDx_To_M.Req_TX = TRUE;
            if(W5500_Service.SIPR[3]==Parall_Slaver.Addr_Master+1)
            {
                Parall_Slaver.IDx1_To_M.Req_TX = TRUE;
            }
        }

        //判断发送是否可用
        if(Parall_Slaver.IDx_To_M.Req_TX == TRUE)
        {
            ECanaShadow.CANTA.all=ECanaRegs.CANTA.all;
            if((Parall_Slaver.IDx_To_M.Flag_First_TX==TRUE)||(ECanaShadow.CANTA.bit.TA2==1))
            {
                Parall_Slaver.IDx_To_M.Flag_First_TX=FALSE;
                Parall_Slaver.IDx_To_M.Req_TX =FALSE;


                Parall_Slaver.IDx_To_M.IDx_L.Bit.Fault_Module   = DCDC_Service.Fault_Module;
                Parall_Slaver.IDx_To_M.IDx_L.Bit.Now_Mode       = DCDC_Service.Now_Mode;
                if(DCDC_Service.Now_Mode>=4)
                {
                    Parall_Slaver.IDx_To_M.IDx_H.Now_Cur_AB     = DCDC_Service.Now_Cur_AB;
                }
                else
                {
                    Parall_Slaver.IDx_To_M.IDx_H.Now_Cur_AB     =0;
                }

                ECanaMboxes.MBOX2.MDL.all       =Parall_Slaver.IDx_To_M.IDx_L.All;
                ECanaMboxes.MBOX2.MDH.all       =Parall_Slaver.IDx_To_M.IDx_H.All;
                //启动MBOX2的发送
                ECanaRegs.CANTRS.bit.TRS2 = 1;
            }
        }

        //判断发送是否可用
        if(Parall_Slaver.IDx1_To_M.Req_TX == TRUE)
        {
            ECanaShadow.CANTA.all=ECanaRegs.CANTA.all;
            if((Parall_Slaver.IDx1_To_M.Flag_First_TX==TRUE)||(ECanaShadow.CANTA.bit.TA3==1))
            {
                Parall_Slaver.IDx1_To_M.Flag_First_TX=FALSE;
                Parall_Slaver.IDx1_To_M.Req_TX =FALSE;

                //Parall_Slaver.IDx1_To_M.IDx_L.All=0;
                Parall_Slaver.IDx1_To_M.IDx_H.Now_Vol_Sense=Data_Sum.Vol_Sense;

                ECanaMboxes.MBOX3.MDL.all       = Parall_Slaver.IDx1_To_M.IDx_L.All;
                ECanaMboxes.MBOX3.MDH.all       = Parall_Slaver.IDx1_To_M.IDx_H.All;
                //启动MBOX3的发送
                ECanaRegs.CANTRS.bit.TRS3 = 1;
            }
        }

        //从机接收数据处理
        ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
        if(ECanaShadow.CANRMP.bit.RMP17==1)
        {
            Parall_Slaver.ID1_Fr_M.ID1_L.All=ECanaMboxes.MBOX17.MDL.all;
            Parall_Slaver.ID1_Fr_M.ID1_H.All=ECanaMboxes.MBOX17.MDH.all;
            Parall_Slaver.ID1_Fr_M.Flag_RX=TRUE;

            ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
            ECanaShadow.CANRMP.bit.RMP17=1;
            ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;

            if(Parall_Slaver.ID1_Fr_M.ID1_L.Bit.Symbol==SYMBOL_ID1)
            {
                Interface_DCDC.EN_MOS_Out   = Parall_Slaver.ID1_Fr_M.ID1_L.Bit.EN_MOS_Out;
                Interface_DCDC.EN_Run       = Parall_Slaver.ID1_Fr_M.ID1_L.Bit.EN_Run;

                if(Parall_Slaver.ID1_Fr_M.ID1_L.Bit.Req_Fault_Clear==TRUE)
                {
                    Interface_DCDC.Req_ChangeMode   = FALSE;
                    Interface_DCDC.Req_Mode         = DCDC_STANDBY;//20200417
                    DCDC_Service.Fault_Module       = 0;
                    Fault_Code.Err_Global           = 0;
                    Fault_Code.Err_Step             = 0;
                    Fault_Code.Alarm                = 0;//20200417
                    Fault_Code.Err_Global_Bak       = 0;//20200417
                    Fault_Code.Err_Step_Bak         = 0;//20200417
                    Fault_Code.Alarm_Bak            = 0;//20200417
                }

                if(Parall_Slaver.ID1_Fr_M.ID1_L.Bit.Req_ChangeMode==TRUE)
                {
                    Interface_DCDC.Req_Mode         = Parall_Slaver.ID1_Fr_M.ID1_L.Bit.Req_Mode;
                    Interface_DCDC.Req_ChangeMode   = TRUE;
                }

                if(Parall_Slaver.ID1_Fr_M.ID1_L.Bit.Req_Refresh_Cur==TRUE)
                {
                    Interface_DCDC.Req_Cur_Total    = Parall_Slaver.ID1_Fr_M.ID1_H.All;
                    Interface_DCDC.Req_Refresh_Cur  = TRUE;
                }

                Parall_Slaver.Cnt_S_OT_M   = 0;
            }
        }

        if(ECanaShadow.CANRMP.bit.RMP16==1)
        {
            Parall_Slaver.ID0_Fr_M.ID0_L.All=ECanaMboxes.MBOX16.MDL.all;
            Parall_Slaver.ID0_Fr_M.ID0_H.All=ECanaMboxes.MBOX16.MDH.all;
            Parall_Slaver.ID0_Fr_M.Flag_RX=TRUE;

            ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
            ECanaShadow.CANRMP.bit.RMP16=1;
            ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;

            //从机处于待机状态才可以再次接收并机设置数据，判断自己的地址是否在主机要求范围，在范围内进入从机模式
            if((W5500_Service.SIPR[3]<Parall_Slaver.Addr_Master+Parall_Slaver.Parall_Total)&&(W5500_Service.SIPR[3]>Parall_Slaver.Addr_Master)&&
               (DCDC_Service.Now_Mode==DCDC_STANDBY)&&(Parall_Slaver.ID0_Fr_M.ID0_L.Bit.Symbol==SYMBOL_ID0))
            {
                Interface_DCDC.Mode_Parall      = PARALL_SLAVER;
                Interface_DCDC.Parall_Total     = Parall_Slaver.Parall_Total;
                Parall_Slaver.Cnt_S_OT_M        = 0;
            }
        }

        //从机CAN接收超时判断
        if((DCDC_Service.Fault_Module&FAULT_SLAVER_CAN)==0)
        {
            if(++Parall_Slaver.Cnt_S_OT_M>=10000)
            {
                DCDC_Service.Fault_Module |= FAULT_SLAVER_CAN;
                Parall_Slaver.Cnt_S_OT_M =0;
            }
        }
    }

    if(DCDC_Service.Mode_Parall==SINGLE)
    {
        ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
        if(ECanaShadow.CANRMP.bit.RMP16==1)
        {
            Parall_Slaver.ID0_Fr_M.ID0_L.All=ECanaMboxes.MBOX16.MDL.all;
            Parall_Slaver.ID0_Fr_M.ID0_H.All=ECanaMboxes.MBOX16.MDH.all;
            Parall_Slaver.ID0_Fr_M.Flag_RX=TRUE;

            ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
            ECanaShadow.CANRMP.bit.RMP16=1;
            ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;

            if((Parall_Slaver.ID0_Fr_M.ID0_L.Bit.Symbol==SYMBOL_ID0)&&((WorkStep_Service.State==STEP_STANDBY)||(WorkStep_Service.State==STEP_FLOW_STOP)))
            {
                Parall_Slaver.Addr_Master   =Parall_Slaver.ID0_Fr_M.ID0_H.Bit.Add_Master;
                Parall_Slaver.Parall_Total  =Parall_Slaver.ID0_Fr_M.ID0_H.Bit.Parall_Total;

                //单机模式接收到特殊数据帧，判断自己的地址是否在主机要求范围，在范围内进入从机模式
                if((W5500_Service.SIPR[3]<Parall_Slaver.Addr_Master+Parall_Slaver.Parall_Total)&&(W5500_Service.SIPR[3]>Parall_Slaver.Addr_Master))
                {
                    Interface_DCDC.Mode_Parall      = PARALL_SLAVER;
                    Interface_DCDC.Parall_Total     = Parall_Slaver.Parall_Total;

                    WorkStep_Service.Req_CMD        = WORKSTEP_STOP;

                    //收到并机指令，将状态机切换到待机状态
                    DCDC_Service.Fault_Module       = 0;
                    Fault_Code.Err_Global           = 0;
                    Fault_Code.Err_Step             = 0;
                    Fault_Code.Alarm                = 0;
                    Fault_Code.Err_Global_Bak       = 0;
                    Fault_Code.Err_Step_Bak         = 0;
                    Fault_Code.Alarm_Bak            = 0;
                    Interface_DCDC.Req_Mode         = DCDC_STANDBY;
                    Interface_DCDC.Req_ChangeMode   = TRUE;
                    Parall_Slaver.Cnt_S_OT_M        = 0;

                    //
                    Init_ID_Slaver();
                }
            }
        }
    }
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Slaver_Abnormal_Check(void)
{
    static Uint16 Cnt=0;
    if(DCDC_Service.Mode_Parall==PARALL_SLAVER)
    {
        if((DCDC_Service.Now_Mode>=DCDC_CC)&&(DCDC_Service.Now_Mode<=DCDC_STATIC)&&((DCDC_Service.Fault_Module&FAULT_SLAVER_CAN)==0))
        {
        	if(Parall_Slaver.ID1_Fr_M.ID1_L.Bit.Now_Mode_Master!=DCDC_Service.Now_Mode)
        	{
        		if(++Cnt>=25)
        		{
        			DINT;
        			DCDC_Service.Fault_Module |= FAULT_SLAVER_CAN;
        			EINT;
        			Cnt=0;
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
}


//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Init_ID_Single(void)
{
    ECanaRegs.CANME.all &= (~0x00FF000F);

    //ID0发送邮箱
    ECanaMboxes.MBOX0.MSGID.all         = 0x98FF3000;
    ECanaMboxes.MBOX0.MSGCTRL.all       = 0x00000F08;

    //ID0接收邮箱
    ECanaMboxes.MBOX16.MSGID.all        = 0x98FF3000;
    ECanaMboxes.MBOX16.MSGCTRL.all      = 0x00000008;

    //模块IP地址设置接收邮箱
    ECanaMboxes.MBOX23.MSGID.all        = 0x98FF7000;
    ECanaMboxes.MBOX23.MSGCTRL.all      = 0x00000008;

    ECanaRegs.CANME.all |= 0x00810001;
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Init_ID_Master(void)
{
    DINT;
    ECanaRegs.CANME.all &= (~0x00FF000F);

    //ID0发送邮箱
    ECanaMboxes.MBOX0.MSGID.all         = 0x98FF3000;
    ECanaMboxes.MBOX0.MSGCTRL.all       = 0x00000F08;

    //ID1发送邮箱
    ECanaMboxes.MBOX1 .MSGID.all        = 0x98FF4000;
    ECanaMboxes.MBOX1.MSGCTRL.all       = 0x00000E08;

    //IDx接收邮箱
    ECanaMboxes.MBOX18.MSGID.all        = 0x98FF5000|(W5500_Service.SIPR[3]+1);
    ECanaMboxes.MBOX19.MSGID.all        = 0x98FF5000|(W5500_Service.SIPR[3]+2);
    ECanaMboxes.MBOX20.MSGID.all        = 0x98FF5000|(W5500_Service.SIPR[3]+3);
    ECanaMboxes.MBOX21.MSGID.all        = 0x98FF5000|(W5500_Service.SIPR[3]+4);
    ECanaMboxes.MBOX18.MSGCTRL.all      = 0x00000008;
    ECanaMboxes.MBOX19.MSGCTRL.all      = 0x00000008;
    ECanaMboxes.MBOX20.MSGCTRL.all      = 0x00000008;
    ECanaMboxes.MBOX21.MSGCTRL.all      = 0x00000008;

    //IDx1接收邮箱
    ECanaMboxes.MBOX22.MSGID.all        = 0x98FF6000|(W5500_Service.SIPR[3]+1);
    ECanaMboxes.MBOX22.MSGCTRL.all      = 0x00000008;

    ECanaRegs.CANME.all |= 0x007C0003;

    EINT;
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Init_ID_Slaver(void)
{
    ECanaRegs.CANME.all &= (~0x00FF000F);

    //IDx发送邮箱
    ECanaMboxes.MBOX2.MSGID.all         = 0x98FF5000|W5500_Service.SIPR[3];
    ECanaMboxes.MBOX2.MSGCTRL.all       = 0x00000808;

    //IDx1发送邮箱
    ECanaMboxes.MBOX3.MSGID.all         = 0x98FF6000|W5500_Service.SIPR[3];
    ECanaMboxes.MBOX3.MSGCTRL.all       = 0x00000808;

    //ID0接收邮箱
    ECanaMboxes.MBOX16.MSGID.all        = 0x98FF3000;
    ECanaMboxes.MBOX16.MSGCTRL.all      = 0x00000008;

    //ID1接收邮箱
    ECanaMboxes.MBOX17.MSGID.all        = 0x98FF4000;
    ECanaMboxes.MBOX17.MSGCTRL.all      = 0x00000008;

    ECanaRegs.CANME.all |= 0x0003000C;
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void InitCAN_BSP(void)
{
    Parall_Master.Cnt_M_OT_S1               = 0;
    Parall_Master.Cnt_M_OT_S2               = 0;
    Parall_Master.Cnt_M_OT_S3               = 0;
    Parall_Master.Cnt_M_OT_S4               = 0;
    Parall_Master.Cnt_Period                = 0;

    Parall_Master.ID0_To_Sx.Flag_First_TX           = TRUE;
    Parall_Master.ID0_To_Sx.Req_TX                  = FALSE;
    Parall_Master.ID0_To_Sx.ID0_H.All               = 0;
    Parall_Master.ID0_To_Sx.ID0_H.Bit.Add_Master    = W5500_Service.SIPR[3];
    Parall_Master.ID0_To_Sx.ID0_H.Bit.Parall_Total  = 1;
    Parall_Master.ID0_To_Sx.ID0_L.All               = 0;
    Parall_Master.ID0_To_Sx.ID0_L.Bit.Symbol        = SYMBOL_ID0;

    Parall_Master.ID1_To_Sx.Flag_First_TX               = TRUE;
    Parall_Master.ID1_To_Sx.Req_TX                      = FALSE;
    Parall_Master.ID1_To_Sx.ID1_H.Set_Cur_Total         = 0;
    Parall_Master.ID1_To_Sx.ID1_L.Bit.Symbol            = SYMBOL_ID1;
    Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_ChangeMode    = FALSE;
    Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Mode          = DCDC_STANDBY;
    Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Refresh_Cur   = FALSE;
    Parall_Master.ID1_To_Sx.ID1_L.Bit.Req_Fault_Clear   = FALSE;
    Parall_Master.ID1_To_Sx.ID1_L.Bit.EN_Run            = FALSE;
    Parall_Master.ID1_To_Sx.ID1_L.Bit.EN_MOS_Out        = OFF;
    Parall_Master.ID1_To_Sx.Cnt_TX_Req_Change_Mode      = 0;

    Parall_Master.IDx_Fr_S1.Flag_RX         = FALSE;
    Parall_Master.IDx_Fr_S2.Flag_RX         = FALSE;
    Parall_Master.IDx_Fr_S3.Flag_RX         = FALSE;
    Parall_Master.IDx_Fr_S4.Flag_RX         = FALSE;
    Parall_Master.IDx1_Fr_S1.Flag_RX		= FALSE;

    Parall_Master.Now_Cur_S1                = 0;
    Parall_Master.Now_Cur_S2                = 0;
    Parall_Master.Now_Cur_S3                = 0;
    Parall_Master.Now_Cur_S4                = 0;
    Parall_Master.Now_Cur_Total             = 0;
    Parall_Master.Now_Sense_S1				= 0;

    Parall_Master.Cnt_Delay					= 0;


    Parall_Slaver.Addr_Master               = 0;
    Parall_Slaver.Cnt_S_OT_M                = 0;
    Parall_Slaver.Cnt_Period                = 0;
    Parall_Slaver.ID0_Fr_M.Flag_RX          = FALSE;
    Parall_Slaver.ID1_Fr_M.Flag_RX          = FALSE;

    Parall_Slaver.IDx_To_M.Flag_First_TX    = TRUE;
    Parall_Slaver.IDx_To_M.Req_TX           = FALSE;
    Parall_Slaver.IDx_To_M.IDx_H.Now_Cur_AB         = 0;
    Parall_Slaver.IDx_To_M.IDx_L.All                = 0;
    Parall_Slaver.IDx_To_M.IDx_L.Bit.Fault_Module   = 0;
    Parall_Slaver.IDx_To_M.IDx_L.Bit.Now_Mode       = DCDC_STANDBY;

    Parall_Slaver.IDx1_To_M.Flag_First_TX	= TRUE;
    Parall_Slaver.IDx1_To_M.Req_TX			= FALSE;


    Parall_Slaver.Parall_Total              = 1;



    /*Configure GPIO pin for eCAN */
    InitECanaGpio();

    //初始化CAN_A模块寄存器
    InitECana();

    if(ECanaRegs.CANTRS.all!=0)
    {
        ECanaRegs.CANTRR.all=ECanaRegs.CANTRS.all;
    }

    while(ECanaRegs.CANTRS.all!=0);

    // Configure Mailboxes 0-15 as Tx, 16-31 as Rx
    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
    ECanaRegs.CANMD.all = 0xFFFF0000;

    //CAN收发邮箱初始化为单机模式
    Init_ID_Single();

    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
    EALLOW;
    ECanaRegs.CANMIM.all = 0x00000000;
    ECanaRegs.CANGIM.all = 0x00000000;

    // Configure the eCAN for self test mode
    // Enable the enhanced features of the eCAN.
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.STM = 0;    // 1=自测试模式;0=正常模式
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    EDIS;
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Read_Data_Parall(void)
{
    Parall_Master.EEPROM_Parall_Mode   	= EEPROM_Read_Char(cEepromParallMode);
    Parall_Master.EEPROM_Parall_Num  	= EEPROM_Read_Char(cEepromParallNum);

    if(Parall_Master.EEPROM_Parall_Mode==PARALL_MASTER)
    {
    	if((Parall_Master.EEPROM_Parall_Num<2)||(Parall_Master.EEPROM_Parall_Num>5))
    	{
    		Parall_Master.EEPROM_Parall_Num=2;
    	}
    }
    else
    {
    	Parall_Master.EEPROM_Parall_Mode=SINGLE;
    	Parall_Master.EEPROM_Parall_Num=1;
    }
}

//*********************************************************************************
//Description:
//Parameters :
//Return     :
//*********************************************************************************
void Auto_To_Master(void)
{
	if(Parall_Master.Cnt_Delay<75)
	{	++Parall_Master.Cnt_Delay;	}
	else
	{
		if(Parall_Master.Cnt_Delay==75)
		{
			if(Parall_Master.EEPROM_Parall_Mode==PARALL_MASTER)
			{
				Init_ID_Master();

				Parall_Master.Cnt_Period		= 0;
				Interface_DCDC.Mode_Parall		= PARALL_MASTER;
				Interface_DCDC.Parall_Total		= Parall_Master.EEPROM_Parall_Num;
				Interface_DCDC.Req_Mode 		= DCDC_STANDBY;
				DCDC_Service.Fault_Module       = 0;
				Fault_Code.Err_Global           = 0;
				Fault_Code.Err_Step             = 0;
                Fault_Code.Err_Global_Bak       = 0;//20210102
                Fault_Code.Err_Step_Bak         = 0;//20210102
                Fault_Code.Alarm_Bak            = 0;//20210102

				PI_Vol_Charge.Limit_Out_max 	= CUR_MAX_CHANNEL*2*Interface_DCDC.Parall_Total;
				PI_Vol_Charge.Limit_PI_max  	= PI_Vol_Charge.Limit_Out_max<<PI_Vol_Charge.Q_Type;

				PI_Vol_Discharge.Limit_Out_max 	= CUR_MAX_CHANNEL*2*Interface_DCDC.Parall_Total;
				PI_Vol_Discharge.Limit_PI_max  	= PI_Vol_Discharge.Limit_Out_max<<PI_Vol_Discharge.Q_Type;

				//Interface_DCDC.Req_ChangeMode   = TRUE;

				Parall_Master.ID0_To_Sx.ID0_H.All=0;
				Parall_Master.ID0_To_Sx.ID0_H.Bit.Add_Master    =W5500_Service.SIPR[3];
				Parall_Master.ID0_To_Sx.ID0_H.Bit.Parall_Total  =Interface_DCDC.Parall_Total;
				Parall_Master.ID0_To_Sx.ID0_L.All=0;
				Parall_Master.ID0_To_Sx.ID0_L.Bit.Symbol=SYMBOL_ID0;

				Parall_Master.ID0_To_Sx.Req_TX=TRUE;
			}
		}
		Parall_Master.Cnt_Delay=76;
	}

}

void Set_By_CAN(void)
{
    struct ECAN_REGS ECanaShadow;
    ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
    if(ECanaShadow.CANRMP.bit.RMP23==1)
    {
        union CANMDL_REG  MDL;
        union CANMDH_REG  MDH;
        MDH.all=ECanaMboxes.MBOX23.MDH.all;
        MDL.all=ECanaMboxes.MBOX23.MDL.all;

        ECanaShadow.CANRMP.all=ECanaRegs.CANRMP.all;
        ECanaShadow.CANRMP.bit.RMP23=1;
        ECanaRegs.CANRMP.all=ECanaShadow.CANRMP.all;

        if(MDH.byte.BYTE7==0x79)
        {
            switch(MDH.byte.BYTE6)
            {
                case 1:{
                        W5500_Service.SIPR[0] = 192;
                        W5500_Service.SIPR[1] = 168;
                        W5500_Service.SIPR[2] = MDH.byte.BYTE5;
                        W5500_Service.SIPR[3] = MDH.byte.BYTE4;

                        EEPROM_Write_Uint16(cEepromIP_Addr0,W5500_Service.SIPR[0]);
                        EEPROM_Write_Uint16(cEepromIP_Addr1,W5500_Service.SIPR[1]);
                        EEPROM_Write_Uint16(cEepromIP_Addr2,W5500_Service.SIPR[2]);
                        EEPROM_Write_Uint16(cEepromIP_Addr3,W5500_Service.SIPR[3]);

                        if((W5500_Service.SIPR[3]==100)||(W5500_Service.SIPR[3]==102)||(W5500_Service.SIPR[3]==104)||(W5500_Service.SIPR[3]==106)||(W5500_Service.SIPR[3]==108))
                        {
                            Auto_Aging.Req_WR_M1=TRUE;
                        }

                        if((W5500_Service.SIPR[3]==101)||(W5500_Service.SIPR[3]==103)||(W5500_Service.SIPR[3]==105)||(W5500_Service.SIPR[3]==107)||(W5500_Service.SIPR[3]==109))
                        {
                            Auto_Aging.Req_WR_M2=TRUE;
                        }

                        break;}

                case 2:{
                        Uint16 uwData=0;
                        uwData=MDH.word.HI_WORD;
                        if((uwData<CUR_H_K_MIN)||(uwData>CUR_H_K_MAX))
                        {
                            uwData=CUR_H_K_DEFAULT;
                        }

                        EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjAK,uwData);
                        EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjBK,uwData);
                        EEPROM_Write_Uint16(cEepromDischargeAdjHAK,uwData);
                        EEPROM_Write_Uint16(cEepromDischargeAdjHBK,uwData);

                        uwData=MDL.word.LOW_WORD;
                        if((uwData<CUR_L_K_MIN)||(uwData>CUR_L_K_MAX))
                        {
                            uwData=CUR_L_K_DEFAULT;
                        }

                        EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjAK,uwData);
                        EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjBK,uwData);
                        EEPROM_Write_Uint16(cEepromDischargeAdjLAK,uwData);
                        EEPROM_Write_Uint16(cEepromDischargeAdjLBK,uwData);

                        int16 Offset;
                        Offset=MDL.byte.BYTE1&0x00FF;

                        if(Offset>=128)
                        {
                            Offset=Offset|0xFF00;
                        }

                        if((Offset<CUR_H_B_MIN)||(Offset>CUR_H_B_MAX))
                        {
                            Offset=CUR_H_B_DEFAULT;
                        }

                        EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjAb,Offset);
                        EEPROM_Write_Uint16(cEepromOutPutCurrentHAdjBb,Offset);
                        EEPROM_Write_Uint16(cEepromDischargeAdjHAB,-Offset);
                        EEPROM_Write_Uint16(cEepromDischargeAdjHBB,-Offset);

                        Offset=MDL.byte.BYTE0&0x00FF;

                        if(Offset>=128)
                        {
                            Offset=Offset|0xFF00;
                        }

                        if((Offset<CUR_L_B_MIN)||(Offset>CUR_L_B_MAX))
                        {
                            Offset=CUR_L_B_DEFAULT;
                        }

                        EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjAb,Offset);
                        EEPROM_Write_Uint16(cEepromOutPutCurrentLAdjBb,Offset);
                        EEPROM_Write_Uint16(cEepromDischargeAdjLAB,-Offset);
                        EEPROM_Write_Uint16(cEepromDischargeAdjLBB,-Offset);

                        break;}

                case 3:{
                        Uint16 uwData=0;
                        uwData=MDH.word.HI_WORD;
                        if(uwData>50000)
                        {
                            uwData=50000;//4A
                        }

                        EEPROM_Write_Uint16(cEepromCurDiffAging,uwData);
                        break;}

                case 4:{
                        Uint16 uwData=0;
                        uwData=MDH.word.HI_WORD;
                        if(uwData>400)
                        {
                            uwData=400;//400A
                        }

                        if(uwData<10)
                        {
                            uwData=10;//10A
                        }

                        EEPROM_Write_Uint16(cEepromCurLimitedSet,uwData);
                        break;}
            }
        }
    }
}
