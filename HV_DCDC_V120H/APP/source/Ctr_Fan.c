/*
 * Ctr_Fan.c
 *
 *  Created on: 2021Äê6ÔÂ21ÈÕ
 *      Author: JC
 */
#include "F28x_Project.h"

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Fan_Fault_Check(void)
{
    static Uint16 Cnt_Fan_Fault_Check[4]={0,0,0,0};
    Uint16 i;
    Uint16 FAULT_FAN[4];
    FAULT_FAN[0]=((GpioDataRegs.GPCDAT.bit.GPIO89==0)||(GpioDataRegs.GPCDAT.bit.GPIO90==0))?1:0;
    FAULT_FAN[1]=((GpioDataRegs.GPCDAT.bit.GPIO91==0)||(GpioDataRegs.GPCDAT.bit.GPIO92==0))?1:0;
    FAULT_FAN[2]=((GpioDataRegs.GPCDAT.bit.GPIO93==0)||(GpioDataRegs.GPCDAT.bit.GPIO94==0))?1:0;
    FAULT_FAN[3]=((GpioDataRegs.GPDDAT.bit.GPIO99==0)||(GpioDataRegs.GPEDAT.bit.GPIO133==0))?1:0;
    for(i=0;i<4;i++)
    {
        if(FAULT_FAN[i])
        {
            if(++Cnt_Fan_Fault_Check[i]>=25)
            {
                Cnt_Fan_Fault_Check[i]=0;
                DINT;
                Fault_Code[i].Err_Global   |= cFanFault;
                EINT;
            }
        }
        else
        {
            Cnt_Fan_Fault_Check[i]=0;
        }
    }
}
//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void Ctr_Fan(void)
{
    static Uint16 Cnt=0;
   // if((Data_Sum.Cur_CHA>20000)||(Data_Sum.Cur_CHB>20000))
    {
        Cnt=0;
  //      EPwm3Regs.CMPA.half.CMPA = 0;
    }
   // else
    {
        if(++Cnt>4500)
        {
            Cnt=4500;
    //        EPwm3Regs.CMPA.half.CMPA = 3000;
        }
    }
}
