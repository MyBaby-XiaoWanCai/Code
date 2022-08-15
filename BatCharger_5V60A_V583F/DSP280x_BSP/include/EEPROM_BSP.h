/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: EEPROM_BSP.h
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef DSP2803X_BSP_INCLUDE_EEPROM_BSP_H_
#define DSP2803X_BSP_INCLUDE_EEPROM_BSP_H_

//常量定义
#define ADDR_24C08              0x50        //24C08器件地址
#define cMaxAddrSize            1023        //

#define NUM_SERVICE_I2C         8           //I2C服务接口总数量
#define LEN_DATA_OBJ_I2C        32          //I2C服务接口数据缓冲区长度

#define SERV_EE_COM             0           //串口用
#define SERV_EE_NET             1           //网口用
#define SERV_EE_DEBUG           2           //调试模块用
#define SERV_EE_WORKSTEP        3           //工步处理模块用
#define SERV_EE_POWERFAIL       4           //掉电保存数据用
#define SERV_EE_DAIG            5           //故障诊断模块用


#define     cEepromCabinetsNum          0x0000
#define     cEepromModuleAddress        0x0002
#define     cEepromDcBusVoltAdjK        0x0004
#define     cEepromDcBusVoltAdjb        0x0006

#define     cEepromBatteryVoltAdjK      0x0008
#define     cEepromBatteryVoltAdjb      0x000A
#define     cEepromBatSenseVoltAdjK     0x000C
#define     cEepromBatSenseVoltAdjb     0x000E
//--------------------------------------------
#define     cEepromOutPutCurrentHAdjAK  0x0010
#define     cEepromOutPutCurrentHAdjAb  0x0012
#define     cEepromOutPutCurrentHAdjBK  0x0014
#define     cEepromOutPutCurrentHAdjBb  0x0016
#define     cEepromOutPutCurrentLAdjAK  0x0018
#define     cEepromOutPutCurrentLAdjAb  0x001A
#define     cEepromOutPutCurrentLAdjBK  0x001C
#define     cEepromOutPutCurrentLAdjBb  0x001E

//--------------------------------------------
#define     cEepromGateway_IP0          0x0028
#define     cEepromGateway_IP1          0x002A
#define     cEepromGateway_IP2          0x002C
#define     cEepromGateway_IP3          0x002E
#define     cEepromSub_Mask0            0x0030
#define     cEepromSub_Mask1            0x0032
#define     cEepromSub_Mask2            0x0034
#define     cEepromSub_Mask3            0x0036
#define     cEepromPhy_Addr0            0x0038
#define     cEepromPhy_Addr1            0x003A
#define     cEepromPhy_Addr2            0x003C
#define     cEepromPhy_Addr3            0x003E
#define     cEepromPhy_Addr4            0x0040
#define     cEepromPhy_Addr5            0x0042
#define     cEepromIP_Addr0             0x0044
#define     cEepromIP_Addr1             0x0046
#define     cEepromIP_Addr2             0x0048
#define     cEepromIP_Addr3             0x004A
#define     cEepromS0_Port0             0x004C
#define     cEepromS0_Port1             0x004E
#define     cEepromCCIKP                0x0050
#define     cEepromCCIKI                0x0052
#define     cEepromDCIKP                0x0054
#define     cEepromDCIKI                0x0056
#define     cEepromCVKP                 0x0058
#define     cEepromCVKI                 0x005A
#define     cEepromDCVKP                0x005C
#define     cEepromDCVKI                0x005E
#define     cEepromStepComplete         0x0060
#define     cEepromPowerOffStep         0x0062
#define     cEepromPowerOfftime         0x0064
#define     cEepromPowerOffAH           0x0066
//------------------放电K,B值系数-------------------
#define    cEepromDischargeAdjLAK         0x0068//A低档位K
#define    cEepromDischargeAdjLAB         0x0070//A低档位B
#define    cEepromDischargeAdjHAK         0x0072//A低档位K
#define    cEepromDischargeAdjHAB         0x0074//A低档位B
#define    cEepromDischargeAdjLBK         0x0076//A低档位K
#define    cEepromDischargeAdjLBB         0x0078//A低档位B
#define    cEepromDischargeAdjHBK         0x0080//A低档位K
#define    cEepromDischargeAdjHBB         0x0082//A低档位B
//--------------------------------------------------
//20191019 输出电流温度补偿参数存储地址
#define    cEepromT_Cali                0x0084
#define    Default_T_Cali               250
#define    cEepromK_Coeff               0x0086
#define    Default_K_Coeff              -25
//20191019

//20191107
#define    cEepromT_Offset_CC           0x0088
#define    Default_Offset_CC            0
#define    cEepromT_Offset_DCC          0x008A
#define    Default_Offset_DCC           0
//20191107

//20191226
#define    cEepromPowerOffEnergy        0x008C
//20191226

#define    cEepromParallMode        	0x008E	//主机工作模式
#define    cEepromParallNum        		0x008F	//并机总数量

#define    cEepromCaliVolPT             0x0090  //两字节无符号数

#define    cEepromStepAging             0x0092  //2字节

#define    cEepromCurLimitedSet         0x0094  //2字节

#define    cEepromCurDiffAging          0x0096  //2字节

#define    cEepromVol_T1                0x0098  //2字节


#define     cBootFlag                   0x0300
#define     cEepromCheckCode            0x03FE


//结构体定义
//I2C读写地址与数据对定义
typedef struct DATA_OBJ_I2C
{
 Uint16 Data;
 Uint16 Addr;
} DATA_OBJ_I2C;

//I2C接口定义
typedef struct INTERFACE_I2C
{
                 Uint16  Req_RW;            //client->service读写请求，Req_RW为空闲时才能使用
                 Uint16  Result_RW;         //service->client读写结果，Result_RW非零时读写完成
                 Uint16  Addr_Device;       //当前服务接口待读写I2C器件地址,无需左移1位
                 Uint16  Num_Bytes;         //当前服务接口待读写字数,必须小于等于LEN_DATA_OBJ_I2C
                 Uint16  Num_Overtime;      //读写超时时间,100uS/LSB
                 Uint16  Len_Buff;          //当前服务接口I2C数据读出写入缓冲区大小
           DATA_OBJ_I2C  Data[LEN_DATA_OBJ_I2C];//当前服务接口数据缓冲区
   struct INTERFACE_I2C *Ptr_Next;          //指向下一个I2C服务接口，形成环形队列
} INTERFACE_I2C;

//client->service请求定义
#define REQ_I2C_FREE            0x00        //0x00-空闲
#define REQ_I2C_WRITE           0x01        //0x01-请求写
#define REQ_I2C_READ            0x02        //0x02-请求读

//service->client读写结果定义
#define I2C_FREE                0x00        //0x00-空闲
#define I2C_WORKING             0x01        //0x01-正在读写状态
#define I2C_WRITE_OK            0x02        //0x02-写成功
#define I2C_READ_OK             0x03        //0x03-读成功
#define I2C_WRITE_ERR           0x12        //0x12-写失败
#define I2C_READ_ERR            0x13        //0x13-读失败

//I2C服务状态机数据
typedef struct STATUS_I2C_SERVICE
{
                 Uint16  Cnt_Bytes;         //当前服务已读写完成字数计数器
                 Uint16  Cnt_Overtime;      //超时时间计数器
                 Uint16  Cnt_Error;         //总线出错次数累计计数器，在超过一定值后复位I2C模块，正确读取一次后清零
                 Uint16  Cnt_Delay;
                 Uint16  State;             //I2C读写状态机状态标志
                 Uint16  Serv_Ready;        //服务是否可以使用标志，0-不可以，1-可以
          INTERFACE_I2C *Ptr;               //当前I2C驱动程序服务的请求对象指针，当前服务完成后指向下一个服务
} STATUS_I2C_SERVICE;

//对外部文件开放全局变量声明
extern INTERFACE_I2C        Interface_I2C[NUM_SERVICE_I2C];       //有读写EEPROM需求的模块根据需要可以分配1个或多个读写服务
extern STATUS_I2C_SERVICE   I2C_Service;

//对外部文件开放函数声明
extern unsigned char EEPROM_Read_Char(Uint16 Addr);
extern Uint16   EEPROM_Read_Uint16(Uint16 Addr);
extern Uint16   EEPROM_Read_Num(Uint16 Addr,Uint16 Num,unsigned char *Ptr_Data);
extern Uint16   EEPROM_Write_Char(Uint16 Addr,unsigned char Data);
extern Uint16   EEPROM_Write_Uint16(Uint16 Addr,Uint16 Data);
extern Uint16   EEPROM_Write_Num(Uint16 Addr,Uint16 Num,unsigned char *Ptr_Data);

extern Uint16   EEPROM_Write_Uint16_Serv(Uint16 Index,Uint16 Addr,Uint16 Data);
extern void     Read_Para_Net(void);
extern void     Read_Para_Cali(void);
extern void     Read_Para_Cabinet(void);
extern void     InitI2C_BSP(void);
extern void     EEPROM_Service(void);
extern Uint16   Reset_EEPROM_Service(Uint16 Index);
extern Uint16   EEPROM_Read_Start(Uint16 Index);
extern Uint16   Qurey_EEPROM_Read(Uint16 Index);
extern Uint16   Qurey_EEPROM_Write(Uint16 Index);
extern Uint16   EEPROM_Write_Start(Uint16 Index);
extern Uint16   EEPROM_Write_Data(Uint16 Index,Uint16 Addr,Uint16 Data);
extern Uint16   EEPROM_Read_Data(Uint16 Index,Uint16 Addr);
extern Uint16   EEPROM_Read_Data_Num(Uint16 Index,Uint16 Addr,Uint16 Num);
extern Uint16   EEPROM_Write_Data_Num(Uint16 Index,Uint16 Addr,Uint16 Num,unsigned char *Ptr_Data);
extern Uint16   EEPROM_Read_Data_Uint16(Uint16 Index,Uint16 Addr);

extern void Read_Data_PowerFail(void);



#endif /* DSP2803X_BSP_INCLUDE_EEPROM_BSP_H_ */
