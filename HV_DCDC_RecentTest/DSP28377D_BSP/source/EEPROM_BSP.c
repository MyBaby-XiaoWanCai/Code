/*
 * EEPROM_BSP.c
 *
 *  Created on: 2021年6月16日
 *      Author: JC
 */
#include "F28x_Project.h"
 INTERFACE_I2C Interface_I2C[NUM_SERVICE_I2C];         //有读写EEPROM需求的模块根据需要可以分配1个或多个读写服务
STATUS_I2C_SERVICE I2C_Service;                            //I2C服务状态机内部用变量
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
   F28x_usDelay(20);
   I2caRegs.I2CMDR.all              = 0x0020;       // Take I2C out of reset
   F28x_usDelay(20);

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
    I2caRegs.I2CSAR.all = (ADDR_24C08 |(Addr>>8));
    I2caRegs.I2CCNT = 1;
    I2caRegs.I2CDXR.all = (unsigned char)Addr;
    I2caRegs.I2CMDR.all = 0x2620;
    while(I2caRegs.I2CSTR.bit.ARDY==0){};
    I2caRegs.I2CCNT = 1;
    I2caRegs.I2CMDR.all = 0x2C20;
    while(I2caRegs.I2CMDR.bit.STP == 1)
    {   DELAY_US(2);    }
    ubRdata = I2caRegs.I2CDRR.all;
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

    I2caRegs.I2CSAR.all =  (ADDR_24C08 |(Addr>>8));
    I2caRegs.I2CCNT = 2;
    I2caRegs.I2CDXR.all = (unsigned char)Addr;
    I2caRegs.I2CDXR.all = (unsigned char)Data;

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

void InitI2CGpio()
{
// For this example, only init the pins for the SCI-A port.
// These functions are found in the F2837xS_Gpio.c file.
//   GPIO_SetupPinMux(32, GPIO_MUX_CPU1, 1);
//   GPIO_SetupPinMux(33, GPIO_MUX_CPU1, 1);
     EALLOW;
    /* Enable internal pull-up for the selected I2C pins */
    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;

    /* Set qualification for the selected I2C pins */
    GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;

    /* Configure which of the possible GPIO pins will be I2C_A pins using GPIO regs*/

    GpioCtrlRegs.GPBGMUX1.bit.GPIO32 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;//SDAA
    GpioCtrlRegs.GPBGMUX1.bit.GPIO33 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;//SCLA

    EDIS;
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
