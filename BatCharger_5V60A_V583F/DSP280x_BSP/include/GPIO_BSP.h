/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: GPIO_BSP.h
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef DSP2803X_BSP_INCLUDE_GPIO_BSP_H_
#define DSP2803X_BSP_INCLUDE_GPIO_BSP_H_

//逻辑输入输出IO定义
#define CONFIG_GPIO_AD7606_BUSY {GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0; GpioCtrlRegs.GPADIR.bit.GPIO14=0;}
#define AD7606_IS_BUSY           GpioDataRegs.GPADAT.bit.GPIO14==1
#define AD7606_IS_READY          GpioDataRegs.GPADAT.bit.GPIO14==0

#define CONFIG_GPIO_AD7606_CONV {GpioCtrlRegs.GPAPUD.bit.GPIO15 = 1;GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;GpioCtrlRegs.GPADIR.bit.GPIO15 = 1;GpioDataRegs.GPACLEAR.bit.GPIO15 = 1;}
#define SET_AD7606_CONV         {GpioDataRegs.GPASET.bit.GPIO15   = 1;}
#define CLEAR_AD7606_CONV       {GpioDataRegs.GPACLEAR.bit.GPIO15 = 1;}

#define CONFIG_GPIO_FLASH_CS    {GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;}
#define SET_FLASH_CS            {GpioDataRegs.GPASET.bit.GPIO19   = 1;}
#define CLEAR_FLASH_CS          {GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;}

#define CONFIG_GPIO_W5500_CS    {GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 0;GpioCtrlRegs.GPADIR.bit.GPIO23 = 1;GpioDataRegs.GPACLEAR.bit.GPIO23 = 1;}
#define SET_W5500_CS            {GpioDataRegs.GPASET.bit.GPIO23   = 1;}
#define CLEAR_W5500_CS          {GpioDataRegs.GPACLEAR.bit.GPIO23 = 1;}

#define CONFIG_GPIO_CTR_OUT_CHA {GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;GpioDataRegs.GPBSET.bit.GPIO34 = 1;}
#define DC_OUT_CHA_OFF          {GpioDataRegs.GPBSET.bit.GPIO34   = 1;}
#define DC_OUT_CHA_ON           {GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;}
#define DC_OUT_CHA_TOGGLE       {GpioDataRegs.GPBTOGGLE.bit.GPIO34=1;}

#define CONFIG_GPIO_CTR_OUT_CHB {GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;}
#define DC_OUT_CHB_ON           {GpioDataRegs.GPASET.bit.GPIO5   = 1;}
#define DC_OUT_CHB_OFF          {GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;}
#define DC_OUT_CHB_TOGGLE       {GpioDataRegs.GPATOGGLE.bit.GPIO5=1;}

#define CONFIG_GPIO_HDW_OCP     {GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0; GpioCtrlRegs.GPADIR.bit.GPIO7=0;}
#define IS_HDW_OCP              GpioDataRegs.GPADAT.bit.GPIO7==1

#define CONFIG_GPIO_LED_RED     {GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;}
#define LED_RED_ON              {GpioDataRegs.GPASET.bit.GPIO8   = 1;}
#define LED_RED_OFF             {GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;}
#define LED_RED_TOGGLE          {GpioDataRegs.GPATOGGLE.bit.GPIO8=1;}

#define CONFIG_GPIO_LED_GREEN   {GpioCtrlRegs.GPAPUD.bit.GPIO9 = 1;GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;}
#define LED_GREEN_ON            {GpioDataRegs.GPASET.bit.GPIO9   = 1;}
#define LED_GREEN_OFF           {GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;}
#define LED_GREEN_TOGGLE        {GpioDataRegs.GPATOGGLE.bit.GPIO9=1;}

#define CONFIG_GPIO_CD4053_RANGE   {GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;GpioDataRegs.GPASET.bit.GPIO10 = 1;}
#define SET_CUR_RANGE_H         {GpioDataRegs.GPASET.bit.GPIO10   = 1;}
#define SET_CUR_RANGE_L         {GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;}

#define CONFIG_GPIO_W5500_REQ   {GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0; GpioCtrlRegs.GPADIR.bit.GPIO11=0;}
#define IS_W5500_REQ            GpioDataRegs.GPADAT.bit.GPIO11==0

#define CONFIG_GPIO_FAULT_FAN   {GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0; GpioCtrlRegs.GPADIR.bit.GPIO12=0;}
#define IS_FAULT_FAN            GpioDataRegs.GPADAT.bit.GPIO12==0

#define CONFIG_GPIO_AD7606_RST  {GpioCtrlRegs.GPAPUD.bit.GPIO13 = 1;GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;GpioCtrlRegs.GPADIR.bit.GPIO13 = 1;GpioDataRegs.GPACLEAR.bit.GPIO13 = 1;}
#define SET_AD7606_RST          {GpioDataRegs.GPASET.bit.GPIO13   = 1;}
#define CLEAR_AD7606_RST        {GpioDataRegs.GPACLEAR.bit.GPIO13 = 1;}


//结构体定义


//联合体定义


//对外部文件开放全局变量声明


//对外部文件开放函数声明
extern void InitGPIO_BSP(void);

#endif /* DSP2803X_BSP_INCLUDE_GPIO_BSP_H_ */
