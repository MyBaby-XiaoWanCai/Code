/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : main.c
** Author      : wang liang
** Date        : 2021-11-22
** Description : RTX5 & HC32F4A0 Demo Project main file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "hc32_ddl.h"
#include "bsp.h"
#include "main.h"

/**********************************************************************************************************
**                                         类型申明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
pFunction Jump_To_Application;   //跳转执行函数指针
uint32_t  JumpAddress;           //程序执行首地址

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/**********************************************************************************************************
**  函 数 名 : EFM_FCT_Init()
**  功能说明 : 标准C程序入口
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void EFM_FCT_Init(void)
{
    stc_efm_cfg_t stcEfmCfg;             //EFM配置结构体变量
    en_int_status_t flag1;               //flash0 ready flag
    en_int_status_t flag2;               //flash1 ready flag
    /* EFM default config. */
    (void)EFM_StructInit(&stcEfmCfg);     //还原默认EFM配置
    /*
     * Clock <= 40MHZ             EFM_WAIT_CYCLE_0
     * 40MHZ < Clock <= 80MHZ     EFM_WAIT_CYCLE_1
     * 80MHZ < Clock <= 120MHZ    EFM_WAIT_CYCLE_2
     * 120MHZ < Clock <= 160MHZ   EFM_WAIT_CYCLE_3
     * 160MHZ < Clock <= 200MHZ   EFM_WAIT_CYCLE_4
     * 200MHZ < Clock <= 240MHZ   EFM_WAIT_CYCLE_5
     */
    stcEfmCfg.u32WaitCycle = EFM_WAIT_CYCLE_5;   //根据当前系统时钟配置修改u32WaitCycle周期个数

    /* EFM config */
    (void)EFM_Init(&stcEfmCfg);                  //配置EFM寄存器组

    /* Wait flash0, flash1 ready. */             //等待flash0和flash1准备好
    do {
        flag1 = EFM_GetFlagStatus(EFM_FLAG_RDY0);
        flag2 = EFM_GetFlagStatus(EFM_FLAG_RDY1);
    } while((Set != flag1) || (Set != flag2));
}


/*
*********************************************************************************************************
*   函 数 名: Boot_JMP_APP
*   功能说明: 跳转到APP，注意：中断向量表偏移处理在APP程序内完成
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void Boot_JMP_APP(void)
{
    __set_PRIMASK(1)    /* 禁止全局中断 */;

    /*跳转条件：SP指向地址是否合法(在SRAMH地址范围内),华大 SRAMH 地址范围：0x1FFE0000~0x1FFFFFFF*/
    if(((*(__IO uint32_t *)APPLICATION_ADDRESS)>= 0x1FFE0000)&&((*(__IO uint32_t *)APPLICATION_ADDRESS)<=0x1FFFFFFF))
    {
        /* Jump to user application */
        JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS + 4);           // 取中断向量表中的复位中断地址
        Jump_To_Application = (pFunction) JumpAddress;                       // 跳转函数使用该地址
        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t *) APPLICATION_ADDRESS);                   //根据跳转地址，重置MSP

        Jump_To_Application();                                               //执行跳转函数实现APP跳转
    }
}

/*
*********************************************************************************************************
*   函 数 名: Erase_APP_Zone
*   功能说明: 擦除APP区数据
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void Erase_APP_Zone(void)
{
    en_result_t Erase_Flag;
    EFM_SectorCmd_Sequential(EFM_ADDR_SECTOR16, 112 ,Enable);        //关闭FCT APP扇区(16~127)写保护
    __set_PRIMASK(1);                                                  //关中断
    for(uint8_t i=0; i<112 ; i++)
    {
    do {
                                                    //关总中断
        Erase_Flag=EFM_SectorErase(EFM_ADDR_SECTOR16 + i*0x2000);     //逐个擦除每一个扇区，共112个扇区，每个扇区大小8K
        Feed_WDG();                                                   //喂狗
    } while(Erase_Flag != Ok );
    }
    __set_PRIMASK(0);                                                 //开中断
}

/**********************************************************************************************************
**  函 数 名 : main()
**  功能说明 : 标准C程序入口
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
int32_t main(void)
{
    uint8_t APP_Copy_Flag=0;             //APP拷贝标志
    uint8_t state;                       //铁电回读状态
    uint32_t date=0;                     //FLASH读出单字缓存
    uint32_t i;                          //地址增量    

    Peripheral_WE();                                      //外设寄存器写使能
    System_Init();                                        //系统和外设初始化
    EFM_FCT_Init();                                       //初始化EFM寄存器
    
    MB85Ctrl_Read(APP_Copy_Flag_ADD,&APP_Copy_Flag,1);     //读取APP拷贝标志
    
     /*如果拷贝标志=0xCC，则执行拷贝升级数据到APP运行区操作,否则直接跳转运行区*/
    if(APP_Copy_Flag==0xCC)
    {
        GPIO_ResetPins(GPIO_PORT_LED82, GPIO_PIN_LED82);                 //点亮LED82
        /*擦除目前APP运行区数据*/
        Erase_APP_Zone();

        /*拷贝升级区数据到运行区,单次拷贝1个字（4字节）*/
        for( i=0; i<112*0x2000; i=i+4)
        {
            date= *(uint32_t *)(UPDATE_ADDRESS+i);                      //读取升级区数据
            EFM_SingleProgram(APPLICATION_ADDRESS+i , date);            //写入APP区 
          if(!(i%0x2000))            
            GPIO_TogglePins(GPIO_PORT_LED83, GPIO_PIN_LED83);           //每拷贝完一个扇区，翻转一次板子灯          
        }
        
        GPIO_SetPins(GPIO_PORT_LED82, GPIO_PIN_LED82);                  //熄灭LED82
        
        /*修改APP_Copy_Flag=0xAA*/
        APP_Copy_Flag=0xAA;
        state = MB85Ctrl_CheckWrite(APP_Copy_Flag_ADD,&APP_Copy_Flag,1);             //修改的拷贝标志写入铁电存储器        
        EFM_SectorCmd_Sequential(EFM_ADDR_SECTOR16, 112 ,Disable);          //打开FCT APP扇区写保护        
        Peripheral_WP();                  //使能外设寄存器写保护
        
        /*设备重启*/
        if(state)                         //确保铁电回读成功
        {
          NVIC_SystemReset();             //设备重启
        }
    }
    else
    {
        Peripheral_WP();                       //使能外设寄存器写保护
        TimerAUintStop(TMRA1_UNIT);            //关中断前先停止
        NVIC_DisableIRQ(APP_TMRA1_IRQn);       //跳转前关单个中断，防止APP未使用该中断时再次开启总中断后找不到入口
        __set_PRIMASK(1);                      //关总中断
        Boot_JMP_APP();                        //跳转APP运行区
    }
    
    /*如果没有跳转APP则执行到此处*/
    NVIC_EnableIRQ(APP_TMRA1_IRQn);       //执行到此处表示，跳转失败，重新打开喂狗定时器
    
    while(1)
    {
       GPIO_SetPins(GPIO_PORT_LED82, GPIO_PIN_LED82);
       GPIO_ResetPins(GPIO_PORT_LED83, GPIO_PIN_LED83);
       bsp_DelayMS(1000);
       GPIO_TogglePins(GPIO_PORT_LED83, GPIO_PIN_LED83);    
       GPIO_TogglePins(GPIO_PORT_LED82, GPIO_PIN_LED82); 
       bsp_DelayMS(1000);        
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
