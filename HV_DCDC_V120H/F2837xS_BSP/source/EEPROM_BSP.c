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
  /* I2caRegs.I2CPSC.all              = 9;            // 10MHz Prescaler - need 7-12 Mhz on module clk
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
   I2caRegs.I2CIER.all = 0x0024;
   I2caRegs.I2CMDR.all              = 0x0000;       // Take I2C out of reset
   F28x_usDelay(20);
   I2caRegs.I2CMDR.all              = 0x0020;       // Take I2C out of reset
   F28x_usDelay(20);*/
  /*  I2caRegs.I2CMDR.all = 0x4000;    // reset I2C
    I2caRegs.I2CPSC.all              = 9;            // 10MHz Prescaler - need 7-12 Mhz on module clk
    I2caRegs.I2CCLKL                 = 40;           // NOTE: must be non zero
    I2caRegs.I2CCLKH                 = 30;           // NOTE: must be non zero
    I2caRegs.I2CIER.all = 0x0024;   // SCD & ARDY interrupts

        I2caRegs.I2CMDR.all = 0x4020;   // Take I2C out of reset
                                        // Stop I2C when suspended

        I2caRegs.I2CFFTX.all = 0x6040;  // Enable FIFO mode and TXFIFO
        I2caRegs.I2CFFRX.all = 0x2040;  // Enable RXFIFO, clear RXFFINT,*/



    /*
    I2caRegs.I2CPSC.all              = 9;            // 10MHz Prescaler - need 7-12 Mhz on module clk
       I2caRegs.I2CCLKL                 = 40;           // NOTE: must be non zero
       I2caRegs.I2CCLKH                 = 30;           // NOTE: must be non zero
       I2caRegs.I2CMDR.all              = 0x0020;       // Take I2C out of reset

       // Enable FIFO mode and TXFIFO
       I2caRegs.I2CFFTX.all = 0x6040;  // Enable FIFO mode and TXFIFO

       // Enable RXFIFO, clear RXFFINT,
       I2caRegs.I2CFFRX.all = 0x2040;  // Enable RXFIFO, clear RXFFINT
       I2caRegs.I2CIER.all = 0x0024;
       I2caRegs.I2CMDR.all              = 0x0000;       // Take I2C out of reset
       F28x_usDelay(20);
       I2caRegs.I2CMDR.all              = 0x0020;       // Take I2C out of reset
       F28x_usDelay(20);*/
    I2caRegs.I2CMDR.bit.IRS =0;          //使能I2C


     I2caRegs.I2CPSC.all = 9;         // Prescaler - need 7-12 Mhz on module clk
     I2caRegs.I2CCLKL = 15;           // NOTE: must be non zero
     I2caRegs.I2CCLKH = 15;           // NOTE: must be non zero
     I2caRegs.I2CIER.all = 0x00;      // Enable SCD & ARDY __interrupts

     I2caRegs.I2CIER.all = 0x24;
     I2caRegs.I2CFFTX.all = 0x0000;
     I2caRegs.I2CMDR.bit.IRS = 1;

}

Uint16 I2CA_WriteData(Uint16 slaveaddr,Uint16 addr,Uint16 len,unsigned char * str)
{
   Uint16 i;

   if (I2caRegs.I2CMDR.bit.STP == 1)
   {
      return I2C_STP_NOT_READY_ERROR;
   }

   // Setup slave address
   I2caRegs.I2CSAR.all = slaveaddr;

   // Check if bus busy
   if (I2caRegs.I2CSTR.bit.BB == 1)
   {
      return I2C_BUS_BUSY_ERROR;
   }

   // Setup number of bytes to send
   // MsgBuffer + Address
   I2caRegs.I2CCNT = len+2;

   // Setup data to send
   I2caRegs.I2CDXR.all = addr>>8;
   I2caRegs.I2CMDR.all = 0x6E20;
   while(!I2C_xrdy());
   I2caRegs.I2CDXR.all = addr & 0xff;

   for (i=0; i<len; i++)

   {
      while(!I2C_xrdy());
      I2caRegs.I2CDXR.all = *(str+i);
      if (I2caRegs.I2CSTR.bit.NACK == 1)
      return    I2C_BUS_BUSY_ERROR;
   }

   // Send start as master transmitter

   return I2C_SUCCESS;
}
//////////////////////////////////////
Uint16 Writetoeepromstr(Uint16 addr,Uint16 len,unsigned char * str)
{
 Uint16 i=0,j=0;
 j=len/8;
 if(j==0)
 {
  for(i=0;i<1;i++)
  {
    I2CA_WriteData(0x50,addr+i,len,str+i);
    DELAY_US(10000);//必须等待5MS，EEPROM规定连续写间隔
  }
 }
 else
 {
     for(i=0;i<j;i++)
     {
      I2CA_WriteData(0x50,addr+i*8,8,str+i*8);
      DELAY_US(10000);//必须等待5MS，EEPROM规定连续写间隔
     }
  }
 return 1;
}

Uint16 I2CA_ReadData(Uint16 slaveaddr,Uint16 addr,Uint16 len,unsigned char * str)
{
   Uint16 i;
   if (I2caRegs.I2CMDR.bit.STP == 1)
   {
      return I2C_STP_NOT_READY_ERROR;
   }

   I2caRegs.I2CSAR.all = slaveaddr;

   if (I2caRegs.I2CSTR.bit.BB == 1)
    {
         return I2C_BUS_BUSY_ERROR;
    }
      I2caRegs.I2CCNT = 2;
      I2caRegs.I2CDXR.all = addr >>8;
      I2caRegs.I2CMDR.all = 0x6620;         // Send data to setup RTC address
      while(!I2C_xrdy());
      I2caRegs.I2CDXR.all = addr&0xff;
   if (I2caRegs.I2CSTR.bit.NACK == 1)
   return   I2C_BUS_BUSY_ERROR;
   while(!I2C_xrdy());

   I2caRegs.I2CSAR.all = slaveaddr;
   I2caRegs.I2CCNT = len;
   I2caRegs.I2CMDR.all = 0x6C20;
   if (I2caRegs.I2CSTR.bit.NACK == 1)
        return  I2C_BUS_BUSY_ERROR;
   for(i=0;i<len;i++)
   {
      while(!I2C_rrdy());
      *(str + i) = I2caRegs.I2CDRR.all;
      if (I2caRegs.I2CSTR.bit.NACK == 1)
          return    I2C_BUS_BUSY_ERROR;
   }
   DELAY_US(200);
   return I2C_SUCCESS;
}


Uint16  I2C_xrdy()
{
    Uint16  t;
    t = I2caRegs.I2CSTR.bit.XRDY;//数据发送中断标志位；0未做好准备
    return t;
}

Uint16  I2C_rrdy()
{
    Uint16  t;
    t = I2caRegs.I2CSTR.bit.RRDY;//数据接受中断标志位
    return t;
}

void ReadfromeepromStr(Uint16 addr,Uint16 len,unsigned char * str)
{
 Uint16 i=0,j=0;
 j=len/8;
 if(j==0)
 {
 for(i=0;i<1;i++)
 I2CA_ReadData(0x50,addr+i,len,str+i);
 }
 else
 {
 for(i=0;i<=j;i++)
 {
  I2CA_ReadData(0x50,addr+i*8,8,str+i*8);
   DELAY_US(20);
  }
 }

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
    GpioCtrlRegs.GPBPUD.bit.GPIO42 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO43 = 0;

    /* Set qualification for the selected I2C pins */
    GpioCtrlRegs.GPBQSEL1.bit.GPIO42 = 3;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO43 = 3;

    /* Configure which of the possible GPIO pins will be I2C_A pins using GPIO regs*/

    GpioCtrlRegs.GPBGMUX1.bit.GPIO42 = 1;
    GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 2;//SDAA
    GpioCtrlRegs.GPBGMUX1.bit.GPIO43 =1;
    GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 2;//SCLA

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

