/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
// Project: BatCharger
// File name: main_cpu1.c
// Created on: 2020-4-20
// Author: Hu
/**************************************************************************************/
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "driverlib/can.h"
//全局变量定义
Uint16 Flag_Tick=0;
//Uint16 Cnt_Tick=0;
// 其它参数定义



// Defines
Uint16 AdcaResult0;
Uint16 AdcaResult1;
Uint16 AdcaResult2;
Uint16 AdcaResult3;
Uint16 AdcaResult4;
Uint16 AdcaResult5;
Uint16 AdcaResult14;
Uint16 AdcaResult15;

Uint16 AdcbResult0;
Uint16 AdcbResult1;
Uint16 AdcbResult2;
Uint16 AdcbResult3;

Uint16 AdccResult2;
Uint16 AdccResult3;

Uint16 AdcdResult0;
Uint16 AdcdResult1;
Uint16 testdata;
interrupt void xint1_isr(void);
interrupt void xint2_isr(void);
interrupt void xint3_isr(void);
interrupt void xint4_isr(void);

#define SFO_INCOMPLETE      0
#define SFO_COMPLETE        1
#define SFO_ERROR           2
Uint16 status;
const unsigned char ubStep251A_Buffer[112] = {0x70,0x0A,0x00,0xFB,0x03,0xFF,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0xC3,0x50,0xFF,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x03,0xE8,0xFF,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x89,0x54,0x40,0x01,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x01,0x07,0xD0,0x02,0x01,0xDC,0x91,0x03,0x01,0xDC,0x91,0x04,0x00,0xD6,0xD8,0x05,0x00,0x88,0xB8,0x06,0x0C,0xF8,0x50,0x07,0x0C,0xF8,0x50,0x08,0x00,0x27,0x1A,0x01,0x09,0x00,0x27,0x10,0x01,0x0A,0x00,0x27,0x10,0x01,0x0B,0x00,0x27,0x10,0x01,0x36,0xAD,0x0D};
Uint32 Temp3=0,Temp2=0,Temp1=0,Temp0=0;
unsigned char   dat1[]={0,0,0,0,0,0,0,0};
unsigned char   dat[]={ 0x11,0x02,0x33,0x44,0x55,0x66,0x77,0x88};
void main(void)
{
    Uint16 i=0;

    //外设初始化
   InitSystem();
   //
   SetADRst();

   Init_CAN_JN910();
   Protect_Global_Init();
   ClrADRst();

   InitDCDC_BSP();
   // Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global interrupt INTM
   ERTM;   // Enable Global realtime interrupt DBGM

   StartCpuTimer0();
   StartCpuTimer2();


   I2CA_WriteData(0x50,0x60,8,dat);
   DELAY_US(10000);
   I2CA_ReadData(0x50,0x60,8,dat1);
   while(1)
   {

         if(Tm1ms>=20)
         {
            Tm1ms=0;
            Task1ms();

         }

         if(Tm2ms>=40)
         {
            Tm2ms=0;
            Task2ms();

           /*SPI_Flash_Write(SERV_FLASH_DEBUG,(Uint32)(250)*0x1000,ubStep251A_Buffer[0],(unsigned char *)ubStep251A_Buffer);
            while(Qurey_SPI_Flash_Write(SERV_FLASH_DEBUG)==BUSY);
            Reset_SPI_FLASH_Service(SERV_FLASH_DEBUG);
            SPI_Flash_Read(SERV_FLASH_NET,(Uint32)(250)*0x1000,128);
            while(Qurey_SPI_Flash_Read(SERV_FLASH_NET)==BUSY);

            Temp3=Interface_SPI_Flash[SERV_FLASH_NET].Data[4]&0x00FF;
            Temp2=Interface_SPI_Flash[SERV_FLASH_NET].Data[5]&0x00FF;
            Temp1=Interface_SPI_Flash[SERV_FLASH_NET].Data[6]&0x00FF;
            Temp0=Interface_SPI_Flash[SERV_FLASH_NET].Data[7]&0x00FF;*/

         }

         if(Tm10ms>=200)
         {
             Tm10ms=0;
             Task10ms();
         }
         if(Tm1s>=20000)
         {
             Tm1s=0;
             Task1s();
             LED_TOGGLE;
         }

   }


}





