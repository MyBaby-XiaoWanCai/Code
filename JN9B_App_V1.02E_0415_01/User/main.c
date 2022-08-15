/**
 *******************************************************************************
 * @file  template/source/main.c
 * @brief Main program template for the Device Driver Library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2020-07-03       Chengy          First version
   2020-09-10       Heqb            Update license
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by HDSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ddl.h"
#include "bsp.h"
#include "main.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
#define MQ_SIZE 16                            /* 定义消息队列里的消息总数 */
uint8_t used_channel_cfg = 8;	//通道数 - 默认为8
uint8_t contact_test_once_flag[8] = {0};
uint8_t flag_of_intelprot = 0;
uint8_t SelfTest_flag = 1;                  //自检标志 1未自检 0已完成
extern __IO uint8_t ACDC_UpdateFlg;

__IO uint32_t record_upcast_cnt = 0;
__IO uint32_t heart_beat_cnt = 0;

extern uint8_t features; //特征码
extern __IO uint8_t Recon_TimStart; //重连定时启动标志
extern uint8_t  w5500_remote_ip[4];	//远端服务器IP	
extern uint16_t w5500_remote_port;	//远端服务器端口
extern uint32_t IP_Alarm_Flag[8];     //内部保护标志位
extern uint8_t SN[3];

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
//测试
uint8_t read_date[4] = {0};
uint32_t FramID = 0;
uint8_t BOOT_FLAG1, BOOT_FLAG2 = 0;
uint8_t CanTestCnt = 0;

/* 任务句柄 */
osThreadId_t ThreadIdStart = NULL;
osThreadId_t ThreadIdTaskLed = NULL;
osThreadId_t ThreadIdTaskProtection = NULL;
osThreadId_t ThreadIdTaskDebug = NULL;
osThreadId_t ThreadIdTaskEth = NULL;
osThreadId_t ThreadIdTaskAgeing = NULL;             /* 自检老化任务 */
osThreadId_t ThreadIdTaskACDC_alarm = NULL;
osThreadId_t ThreadIdTaskProtocol = NULL;
osThreadId_t ThreadIdTaskWorkstep = NULL;
osThreadId_t ThreadIdTmpAlarmRead = NULL;		//温度、硬件故障读取任务

osSemaphoreId_t dbg_Semaphore = NULL;               /* debug信号量ID */
osSemaphoreId_t acdc_alarm_Semaphore = NULL;        /* ACDC告警信号量ID */
osSemaphoreId_t IPGP_Alarm_Semaphore = NULL;       /* 全局内部告警信号量ID */
osSemaphoreId_t w5500_rcv_Semaphore = NULL;        /* 网口接收信号量ID */
osTimerId_t  timerID_Periodic_adc3 = NULL;          /* ADC3定时器ID */
osMessageQueueId_t ACDC_UpdateQueue;                 /* ACDC升级消息队列 */
osSemaphoreId_t SignalDetect_Semaphore = NULL;	//IO信号检测信号量ID
osSemaphoreId_t SignalTmpAlarmRead_Semaphore = NULL;	//温度、硬件故障读取信号量

/* 任务属性设置 */
const osThreadAttr_t ThreadStart_Attr =
{
    .name = "osRtxStartThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 4096,
};

const osThreadAttr_t ThreadLed_Attr =
{
    .name = "osRtxLedThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 2048,
};

const osThreadAttr_t ThreadProtection_Attr =
{
    .name = "osRtxProtectionThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 4096,
};

const osThreadAttr_t ThreadDebug_Attr =
{
    .name = "osRtxDebugThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 10240,
};

const osThreadAttr_t ThreadEth_Attr =
{
    .name = "osRtxEthThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 4096,
};

const osThreadAttr_t ThreadAgeing_Attr =
{
    .name = "osRtxAgeingThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 2048,
};

const osThreadAttr_t ThreadACDC_alarm_Attr =
{
    .name = "osRtxACDC_alarmThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 2048,
};

const osThreadAttr_t ThreadProtocol_Attr =
{
    .name = "osRtxProtocolThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 4096,
};

const osThreadAttr_t ThreadWorkstep_Attr =
{
    .name = "osRtxWorkstepThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 10240,
};

const osThreadAttr_t ThreadTmpAlarmRead_Attr =
{
    .name = "osRtxTmpAlarmReadThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 4096,
};	//温度、硬件故障读取

/*温度、硬件故障读取  信号量属性 */
const osSemaphoreAttr_t sema_New_Attr_TmpAlarmRead =
{
    .name = "TmpAlarmRead Semaphore",                                   /* 信号量的名字 */
};

/* 信号量属性 */
const osSemaphoreAttr_t sema_New_Attr_debug =
{
    .name = "debug Semaphore",                                   /* 信号量的名字 */
};

/* 信号量属性 */
const osSemaphoreAttr_t sema_New_Attr_eth =
{
    .name = "Eth Semaphore",                                     /* 信号量的名字 */
};

/* 信号量属性 */
const osSemaphoreAttr_t sema_New_Attr_acdc_alarm =
{
    .name = "Acdc_alarm Semaphore",                              /* ACDC告警变化通知 */
};
/* 信号量属性 */
const osSemaphoreAttr_t sema_New_Attr_IPGP_alarm =
{
    .name = "IPGP_alarm Semaphore",                              /* 全局内部告警变化通知 */
};

/* 信号量属性 */
const osSemaphoreAttr_t sema_New_Attr_w5500_rcv =
{
    .name = "w5500_rcv Semaphore",                               /* W5500接收信号量 */
};

/* IO信号检测信号量属性 */
const osSemaphoreAttr_t sema_New_Attr_SignalDectect =
{
    .name = "Signal_Dectect Semaphore",      //信号IO任务检测属性设置
};

/* 定时器属性 */
const osTimerAttr_t timer_Periodic_Attr =
{
    .name = "adc3 periodic timer",                  /* 定时器名字 */
};

/* CAN1消息队列属性 */
const osMessageQueueAttr_t ACDC_UpdateQueue_Attr =
{
    .name = "ACDC_UpdateQueue",                 /* 消息队列的名字 */
};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void AppTaskStart(void *argument);
extern uint64_t bsp_GetRunTime(void);
extern uint64_t bsp_CheckRunTime(uint64_t _LastTime);
/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/**********************************************************************************************************
**  函 数 名 : Product_test()
**  功能说明 : 硬件测试代码
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void Product_test(void)
{
    /* flash测试代码(跨扇区)*/
#ifdef FLASH_test
    uint8_t set_date[] = {1, 2, 3, 4};
    uint8_t read_date[4] = {0};
    SPI_Flash_WriteWithCheck(set_date, 4094, 4);
    SPI_Flash_Erase_Sector(0);
    SPI_Flash_Read(read_date, 4094, 4);
#endif

    /* AD5245测试代码*/
#ifdef AD5245_test
    AD5245_Value_Set(Write_A0_Add, 145);
    AD5245_Full_RS(Write_A0_Add);
    AD5245_Value_Read(Read_A0_Add);
    AD5245_Half_RS(Write_A0_Add);
    AD5245_Value_Read(Read_A0_Add);
    AD5245_Vpro_Set(4.35);     //此时C551电容两端电压应为1.977V
    AD5245_Value_Read(Read_A0_Add);
#endif

    /* MB85RS2测试代码 */
#ifdef MB85RS2_test
    uint8_t date[1000] = {1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
						1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
						1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
						1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
						1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8};
    uint8_t test[1000]={0};
    MB85Ctrl_CheckWrite(0, date, 1000);
    MB85Ctrl_Read(0,test,1000);

#endif

    /*运行时计数器测试代码 */
#ifdef RUNTIME_test
    nowtime1 = bsp_GetRunTime();
    bsp_DelayMS(100);
    nowtime2 = bsp_GetRunTime();
    if(100 == nowtime2 - nowtime1)
    { printf("very precise\n"); }
    else
    { printf("Not precise\n"); }
#endif

    /* 检测ACDC模块类型 */
//    uint8_t Modtype;
//    Modtype = ACDC_Fan_Speed51_ModeTest(); //检查ACDC类型，并设置风扇到51%
//    if(Modtype)
//    {
//        ACDC.Type = Modtype;
//        if((ACDC.Type==1)||(ACDC.Type==11)) //国电
//        { 
//        ACDC_Fan_Speed_CMD(ACDC.Type, 0, 20); //模块0风扇设置到20%
//        ACDC_Fan_Speed_CMD(ACDC.Type, 1, 20); //模块2风扇设置到20%
//        }
//        else if((ACDC.Type==2)||(ACDC.Type==22))//普德
//        {
//        ACDC_Fan_Speed_CMD(ACDC.Type, 1, 20); //模块1风扇设置到20%
//        ACDC_Fan_Speed_CMD(ACDC.Type, 2, 20); //模块2风扇设置到20%         
//        }
//    }
//    else
//    {
//        printf("ACDC Test ERR!\n"); //ACDC模块检测错误
//    }

//    ACDC.FanSpd[0] = 0x33;
//    ACDC.FanSpd[1] = 0x33;

//    /* easyflash初始化 */
//    uint8_t flash_en = 0;
//    ACDC_Inf ACDC_read;
//    //SPI_Flash_Erase_Chip();    //如果不是全新flash，使用easyflash之前最好整片擦除一次
//    if(easyflash_init() == EF_NO_ERR)                                 //初始化easyflash
//    {
//        ef_get_env_blob("flash_en", (void *) &flash_en, 1, NULL);       //从flash读配置使能位
//        if(flash_en == 1)
//        {
//            if(EF_NO_ERR == ef_set_env_blob("ACDC_State", (void *) &ACDC, sizeof(ACDC_Inf)))
//            {
//                printf("creat sucessful");
//            }
//        }
//    }
//    ef_get_env_blob("ACDC_State", (void *) &ACDC_read, sizeof(ACDC_Inf), NULL);
}
/**********************************************************************************************************
**  函 数 名 : BarCode_Mac_CheckSet()
**  功能说明 : 条码Mac关联检查设置函数，需要操作EE
**  形    参 : 无
**  返 回 值 : 1：不满足一致性，使用默认MAC。0：满足一致性，使用EE内部MAC
**********************************************************************************************************/
uint8_t BarCode_Mac_CheckSet(void)
{
    uint8_t Read_En = 0;
    uint8_t Bar_Code[50] = {0};
    uint8_t Bar_Code_Len = 0;
    uint8_t Pyear, Pmonth; //生产年月
    uint16_t Pid; //流水号
    uint16_t Pym; //年月编码
    uint32_t Pmac = 0; //mac编号(低3字节有效)
    uint8_t err = 0;
    
    ee_ReadBytes(&Read_En, MacEn_flag_addr , 1);    //读EE
    if(Read_En == 1)
    {
        ee_ReadBytes(SN, MacID_flag_addr , 3);       //读MAC
        ee_ReadBytes(&Bar_Code_Len, BarCode_Len_flag_addr , 1);  //读条码长度
        if(Bar_Code_Len < 50)                       //条码字符串长度限制50字节
        {
            ee_ReadBytes(Bar_Code, BarCode_End_flag_addr - Bar_Code_Len + 1, Bar_Code_Len);  //根据长度读条码内容

            /*检查数据有效性*/
            if((Bar_Code[0] == 'J') && (Bar_Code[1] == 'N'))            //判读条码头, JN开头
            {
                for(uint8_t i = 0; i < 8; i++)
                {
                    if(Bar_Code[4 + i] > 0x39 || Bar_Code[4 + i] < 0x30) //判断要处理位(年月流水)是否为数字ASC码
                    {
                        err = 1;
                        break;
                    }
                }
            }
            else
            {
                err = 1;
            }
            /*检查关联MAC处理*/
            if(err == 0)
            {
                /*计算年月流水*/
                Pyear = (Bar_Code[4] - 0x30) * 10 + (Bar_Code[5] - 0x30);
                Pmonth = (Bar_Code[6] - 0x30) * 10 + (Bar_Code[7] - 0x30);
                if((Pyear < 21) || (Pmonth > 12) || ((Pyear == 21) && (Pmonth < 10))) //再次判断年月有效范围
                {
                    err = 1;
                }
                if(err == 0)
                {
                    Pid = (Bar_Code[8] - 0x30) * 1000 + (Bar_Code[9] - 0x30) * 100 + (Bar_Code[10] - 0x30) * 10 + (Bar_Code[11] - 0x30); //求流水号
                    Pym = (Pyear == 21) ? (Pmonth - 10) : ((Pyear - 22) * 12 + Pmonth + 2); //求年月编号
                    Pmac = (SN[0] << 16) + (SN[1] << 8) + SN[2]; //求MAC编号
                    if(Pmac != (Pym << 14) + Pid)
                    {
                        err = 1;
                    }
                }
            }
        }
        else
        {
            err = 1;
        }

        if(err)
        {
            SN[0] = 0;  //使用默认MAC
            SN[1] = 0;
            SN[2] = 0;
           for(uint8_t j = 0; j < CHANNEL_CFG; j++)
           { IP_Alarm_Flag[j] |= IntelProtect_MAC; } //MAC异常,告警置位            
        }
    }
    return err;
}
/*
*********************************************************************************************************
*                                     OSTmr_Count_Start
* Description: Time Count start base on OS [note]Must use after OS start!!!!!
* Argument(s): Max count 0xFFFFFFFF
* Return(s)  :
* Note(s)    :
*********************************************************************************************************
*/
void    OSTmr_Count_Start(uint32_t  *pCounter)
{
    *pCounter   =   osKernelGetTickCount();
}
/*
*********************************************************************************************************
*                                     OSTmr_Count_Get
* Description: Time Count stop [note]Must use after OS start!!!!!
* Argument(s): none.
* Return(s)  : time * ms (os tick must be 1000HZ)
* Note(s)    :
*********************************************************************************************************
*/
uint32_t OSTmr_Count_Get(uint32_t *pCounter)
{
    uint32_t    tmptime = 0;
    tmptime =   osKernelGetTickCount();
    /* Overflow */
    if(tmptime < *pCounter)
    { return (0xffffffff - *pCounter + tmptime); }
    else
    { return (tmptime - *pCounter); }
}

/**********************************************************************************************************
**  函 数 名 : CreateTask()
**  功能说明 : 创建应用任务
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void CreateTask(void)
{
  ThreadIdTaskLed = osThreadNew(AppTaskLed, NULL, &ThreadLed_Attr);
	ThreadIdTaskDebug = osThreadNew(AppTaskDebug, NULL, &ThreadDebug_Attr);
	ThreadIdTaskAgeing = osThreadNew(AppTaskAgeing, NULL, &ThreadAgeing_Attr);
	ThreadIdTaskACDC_alarm = osThreadNew(AppACDC_alarm, NULL, &ThreadACDC_alarm_Attr);
	ThreadIdTaskWorkstep = osThreadNew(AppTaskWorkstep, NULL, &ThreadWorkstep_Attr);
	ThreadIdTaskProtection = osThreadNew(AppTaskProtection, NULL, &ThreadProtection_Attr);
	
//	ThreadIdTaskProtection = osThreadNew(AppTaskTmpAlarmRead, NULL, &ThreadTmpAlarmRead_Attr);	//硬件故障以及温度读取
}

extern void ExternalAdDatProcess(void);
extern void EepromDatInit(void);
extern uint8_t workstep_flag_AbortPrio[8];

uint8_t heart_cnt_flag = 0;

/**********************************************************************************************************
**  函 数 名 : AppTaskStart()
**  功能说明 : 启动任务
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AppTaskStart(void *argument)
{
	uint8_t Read_En = 0;
    
	ee_ReadBytes(&Read_En, Remote_En_flag_addr , 1);    //读EE
	if(Read_En == 1)
	{
			ee_ReadBytes(w5500_remote_ip, Remote_IPPort_flag_addr , 4);
			ee_ReadBytes((uint8_t *)&w5500_remote_port, Remote_IPPort_flag_addr + 4 , 2);
	}

	IP_Scanning(); 

	if(((W5500_IP_ADDRESS[2] == 8) && ((W5500_IP_ADDRESS[3] == 42) || (W5500_IP_ADDRESS[3] == 41))) || (g_ucSysMode != SYS_NORMAL_MODE))
	{
		SysModeProtMaskFlag = 1;
	}
	else
	{
		SysModeProtMaskFlag = 0;
	}
	
//	AD5245_Vpro_Set(4.35);
	features = RNG_Get_RandomRange(1, 254); //随机生成特征码起始值

	/* 创建网口协议任务同步信号量 */
	w5500_rcv_Semaphore = osSemaphoreNew(1U, 0U, &sema_New_Attr_w5500_rcv);

	/* 创建网口和协议任务 */
	ThreadIdTaskEth = osThreadNew(AppTaskEth, NULL, &ThreadEth_Attr);
	ThreadIdTaskProtocol = osThreadNew(AppTaskProtocol, NULL, &ThreadProtocol_Attr);
    
	/* 开机自检 */
	if(SelfTest_flag)
	{
		bsp_LedOn(LED_82_NO);
		for(uint8_t m=0;m<10;m++)
		{  
			bsp_DelayMS(50);
			bsp_LedToggle(LED_82_NO); 
			bsp_LedToggle(LED_83_NO); 
			Feed_WDG();
		}
		bsp_LedOff(LED_82_NO);
		bsp_LedOff(LED_83_NO);
		
		bsp_DelayMS(50);
		IntelProtectDeviceCheck();      //开机硬件检测，更新内部保护触发标志

		ACDC.Type = ACDC_Fan_Speed51_ModeTest();
		SelfTest_flag = 0;
	}
	
	/* 自检标志位赋值 */
//	if((IP_Alarm_Flag[0] & IntelProtect_FLE) || (IP_Alarm_Flag[0] & IntelProtect_EEE) || \
//       (IP_Alarm_Flag[0] & IntelProtect_FRE) || (IP_Alarm_Flag[0] & IntelProtect_C1E) || \
//       (IP_Alarm_Flag[0] & IntelProtect_C2E))
	if( (IP_Alarm_Flag[0] & IntelProtect_FLE) || (IP_Alarm_Flag[0] & IntelProtect_EEE) || \
		 (IP_Alarm_Flag[0] & IntelProtect_FRE) || (IP_Alarm_Flag[0] & IntelProtect_C1E) )	//20220228 修改、暂时不控ACDC
	{
//		flag_of_intelprot = 1;
	}	
	
	/*  系统参数初始化   */
	SystemStatus.Neilsbed = Neilsbed_Fitted;	

	uint8_t read_flag = 0;
	
	MB85Ctrl_Read(8192, &read_flag, 1);
	
	MB85Ctrl_Read(8208, power_off_continue, 8);
	
	if(read_flag == 0x01)
	{
		uint32_t read_addr = 4096;
		
		for(int j = 0; j < CHANNEL_USE; j++)
		{
			if(power_off_continue[j] == 1)
			{
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstep[j]       , sizeof(Tier2.workstep[j]      ));
				read_addr += sizeof(Tier2.workstep[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepCache1[j] , sizeof(Tier2.workstepCache1[j]));
				read_addr += sizeof(Tier2.workstepCache1[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepCache2[j] , sizeof(Tier2.workstepCache2[j]));
				read_addr += sizeof(Tier2.workstepCache2[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepAnalyze[j], sizeof(Tier2.workstepAnalyze[j]));
				read_addr += sizeof(Tier2.workstepAnalyze[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepRunning[j], sizeof(Tier2.workstepRunning[j]));
				read_addr += sizeof(Tier2.workstepRunning[j]);
				MB85Ctrl_Read(read_addr, &workstep_flag_AbortPrio[j]         , sizeof(workstep_flag_AbortPrio[j]));
				read_addr += sizeof(workstep_flag_AbortPrio[j]);
				
				if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT)
				{
					ContactCheckFlag[j] = 1;
					contact_test_once_flag[j] = 1;
				}				
				Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_PAUSE;
			}
		}
	}
	else if(read_flag == 0x02)
	{
		uint32_t read_addr = 0;
		
		for(int j = 0; j < CHANNEL_USE; j++)
		{
			if(power_off_continue[j] == 1)
			{
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstep[j]       , sizeof(Tier2.workstep[j]      ));
				read_addr += sizeof(Tier2.workstep[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepCache1[j] , sizeof(Tier2.workstepCache1[j]));
				read_addr += sizeof(Tier2.workstepCache1[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepCache2[j] , sizeof(Tier2.workstepCache2[j]));
				read_addr += sizeof(Tier2.workstepCache2[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepAnalyze[j], sizeof(Tier2.workstepAnalyze[j]));
				read_addr += sizeof(Tier2.workstepAnalyze[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepRunning[j], sizeof(Tier2.workstepRunning[j]));
				read_addr += sizeof(Tier2.workstepRunning[j]);
				MB85Ctrl_Read(read_addr, &workstep_flag_AbortPrio[j]         , sizeof(workstep_flag_AbortPrio[j]));
				read_addr += sizeof(workstep_flag_AbortPrio[j]);

				if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT)
				{
					ContactCheckFlag[j] = 1;
					contact_test_once_flag[j] = 1;
				}				
				Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_PAUSE;
			}
		}	
	}
	
	/* eeprom数据初始化 */
	EepromDatInit();	
	
	/* 创建信号量 */
	dbg_Semaphore = osSemaphoreNew(1U, 0U,   &sema_New_Attr_debug);

	/* 创建信号量 */
	acdc_alarm_Semaphore = osSemaphoreNew(1U, 0U,   &sema_New_Attr_acdc_alarm);
	
	/* 创建信号量 */
	IPGP_Alarm_Semaphore = osSemaphoreNew(1U, 0U,   &sema_New_Attr_IPGP_alarm);

	/*创建消息队列*/
	ACDC_UpdateQueue = osMessageQueueNew(MQ_SIZE, sizeof(ACDC_UpdateStrcut), &ACDC_UpdateQueue_Attr);      /* 消息队列里存放消息的总数  消息的大小（单位: Byte） 消息队列的属性 */
	
	/* 创建任务 */
	CreateTask();
	
	if(SelfTest_flag == 0)
	{
		TimerAUintStart(TMRA1_UNIT); //启动内部，全局保护检测定时器A单元1，周期40MS
	}
    
	heart_cnt_flag = 1;
	
	while(1)
	{
		if(((W5500_IP_ADDRESS[2] == 8) && ((W5500_IP_ADDRESS[3] == 42) || (W5500_IP_ADDRESS[3] == 41))) || (g_ucSysMode != SYS_NORMAL_MODE))
		{
			SysModeProtMaskFlag = 1;
		}
		else
		{
			SysModeProtMaskFlag = 0;
		}		
		
    /* 周期性处理程序 */
		if((record_upcast_cnt >(data_upcast_period * 1000))&&(Recon_TimStart==1)&&(ACDC_UpdateFlg==0))
		{
			printf("Start task is running!");
			
			record_upcast_cnt = 1;
			
			for(int i = 0; i < CHANNEL_USE; i++)
			{
				if((Tier2.record[i].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.record[i].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE) || (protect_flag_upcast[i] == 1))
				{
//					RecordDatUpcast(i, 1);
				}
				else
				{
//					RecordDatUpcast(i, 0);
				}
				osDelay(1); 
			}
			
			for(int i = 0; i < CHANNEL_USE; i++)
			{
				if(workstep_flag_CriticalZone[i] == 1)
				{
					workstep_flag_CriticalZone[i] = 2;
				}
				else if(workstep_flag_CriticalZone[i] == 2)
				{
					workstep_flag_CriticalZone[i] = 3;
				}
			}
		}
		
//		heart_beat_cnt++;
		
		if(heart_beat_flag == 1)
		{
			if(heart_beat_cnt > 6000)	/* 中位机心跳2s一次，连续3次未检测到心跳，判定中位机与下位机断开连接 */
			{
				heart_beat_cnt=0;
				heart_beat_flag = 2;

				for(int i = 0; i < CHANNEL_USE; i++)
				{
					if( (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_STOP) && \
							(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_COMPLETE) )
					{
	//					RecordDatUpcast(i, 1);
						record_data_upcast_flag[i] = 1;
						osDelay(1);						
						BatteryFormattingStop(i);
						Tier2.workstepRunning[i].RunSta = WORKSTEP_RUNSTA_REF_PAUSE;		
					}
				}
			}		
		}
		else 
		if(heart_beat_flag == 0)
		{
			heart_beat_flag = 1;
			heart_beat_cnt = 0;
		}		
//        bsp_ProPer1ms();        
//        ExternalAdDatProcess();
		
		if(g_ucSysMode != SYS_NORMAL_MODE)  
		{
//			Protect_Trigger_Judgemet();             //非正常模式专用保护函数
		}
		
		/* Modify by CAI HENG (Begin) */
//		Can_Network_Func(1000, 5, 5, 5, 1);
		/* Modify by CAI HENG (End) */
		
		osDelay(1);
  }
}

/**
 * @brief  Main function of GPIO project
 * @param  None
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{	
	System_Init();	//初始化函数
	
	/* 内核初始化 */
	osKernelInitialize();

	/* 创建启动任务 */
	ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);

	/* 开启多任务 */
	osKernelStart();
	
	while(1);
}

/**
 * @}
 */

/**
 * @}
 */


/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
