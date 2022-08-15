/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: FLASH_BSP.h
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef DSP2803X_BSP_INCLUDE_FLASH_BSP_H_
#define DSP2803X_BSP_INCLUDE_FLASH_BSP_H_

//常量定义
#define NUM_SERVICE_FLASH       3                           //Flash读写服务接口数量
#define LEN_SPI_FLASH_BUFF      177                         //Flash读写服务数据缓冲区长度

#define SERV_FLASH_WORKSTEP     0                           //工步流程管理模块使用的FLASH服务索引
#define SERV_FLASH_NET          1                           //网口保存工步数据使用的FLASH服务索引
#define SERV_FLASH_DEBUG        2                           //网口调试使用的FLASH服务索引

//结构体定义
typedef struct INTERFACE_SPI_FLASH
{
                 Uint16 Req_RW;                             //client->service读写请求，Req_RW为空闲时才能使用
                 Uint16 Result_RW;                          //service->client读写结果
                 Uint32 Addr_RW;                            //待读写数据字节在FLASH中的起始地址
                 Uint32 Offset_Sector;                       //待操作Sector的索引顺序号
                 Uint32 Offset_In_Sector;                   //待读写数据字节在Sector起始字节地址
                 Uint16 Num_Bytes;                          //待读写字节数,必须小于等于LEN_SPI_FLASH_BUFF
                 Uint16 Num_Overtime;                       //读写超时时间,50uS/LSB
                 Uint16 Len_Buff;                           //当前接口数据读出写入缓冲区大小
          unsigned char Data[LEN_SPI_FLASH_BUFF];           //数据读出写入缓冲区
    struct INTERFACE_SPI_FLASH *Ptr_Next;
} INTERFACE_SPI_FLASH;

typedef struct STATUS_SPI_FLASH_SERVICE
{
                Uint16 State;                               //Flash读写状态机状态标志
                Uint16 Cnt_Bytes;                           //已读写完成字节数计数器
                Uint16 Cnt_Page_Write;                      //
                Uint16 Num_Last_RW;                         //前次读写字节数
                Uint16 Cnt_Overtime;                        //超时时间计数器
                Uint16 Cnt_Error;                           //总线出错次数累计计数器，在超过一定值后复位SPI模块，正确读取一次后清零
                Uint16 Serv_Ready;                          //服务是否可以使用标志，0-不可以，1-可以
                Uint32 Addr_RW_Now;                             //当前操作读写地址累加用
 struct INTERFACE_SPI_FLASH *Ptr;                           //当前SPI接口驱动程序服务的请求对象指针
} STATUS_SPI_FLASH_SERVICE;

//client->service请求定义
#define REQ_SPI_FLASH_IDLE            0           //0x00-空闲
#define REQ_SPI_FLASH_WRITE           1           //0x01-请求写
#define REQ_SPI_FLASH_READ            2           //0x02-请求读

//service->client读写结果定义
#define SPI_FLASH_IDLE                0           //0x00-空闲
#define SPI_FLASH_WORKING             1           //0x01-正在读写状态
#define SPI_FLASH_WRITE_OK            2           //0x02-写成功
#define SPI_FLASH_READ_OK             3           //0x03-读成功
#define SPI_FLASH_WRITE_ERR           4           //0x04-写失败
#define SPI_FLASH_READ_ERR            5           //0x05-读失败


//联合体定义


//对外部文件开放全局变量声明
extern INTERFACE_SPI_FLASH Interface_SPI_Flash[];

//对外部文件开放函数声明
extern void   InitFlash_BSP(void);
extern void   SPI_Flash_Service(void);
extern Uint16 Reset_SPI_FLASH_Service(Uint16 Index);
extern Uint16 Qurey_SPI_Flash_Read(Uint16 Index);
extern Uint16 SPI_Flash_Read(Uint16 Index,Uint32 Addr,Uint16 Num);
extern Uint16 Qurey_SPI_Flash_Write(Uint16 Index);
extern Uint16 SPI_Flash_Write(Uint16 Index,Uint32 Addr,Uint16 Num,unsigned char *Ptr);
#endif /* DSP2803X_BSP_INCLUDE_FLASH_BSP_H_ */
