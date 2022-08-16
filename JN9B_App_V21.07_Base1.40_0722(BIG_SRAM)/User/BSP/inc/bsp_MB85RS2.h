/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_MB85RS2.h
** Author      : Huang Cheng
** Date        : 2021-11-09
** Description : bsp_MB85RS2 head file
**********************************************************************************************************/

#ifndef _BSP_MB85RS2_H_
#define _BSP_MB85RS2_H_

#ifdef __cplusplus
extern "C" {
#endif

    /**********************************************************************************************************
    **                                          头文件引用区
    **********************************************************************************************************/
#include "main.h"


    /**********************************************************************************************************
    **                                            宏定义区
    **********************************************************************************************************/
#define ALRM_MB85_EN               0         //使能故障报警
#define MX_NUM                     5         //最大使用设备数量，默认5片

#define MB85RS2_CMD_WREN 		   0x06      //写使能
#define MB85RS2_CMD_WRDI		   0x04      //写禁止
#define MB85RS2_CMD_RDSR		   0x05      //读状态寄存器
#define MB85RS2_CMD_WRSR		   0x01      //写状态寄存器
#define MB85RS2_CMD_READ		   0x03      //24位地址模式读数据 
#define MB85RS2_CMD_WRITE		   0x02      //写数据
#define MB85RS2_CMD_FSTRD      0x0B      //32位地址模式（高位对齐，低位填0）读数据
#define MB85RS2_CMD_RDID		   0x9f      //读ID，32位ID
#define MB85RS2_CMD_SLEEP		   0xB9      //低功耗模式

#define StartADD_WorkStatus	       0          // 上一次断电工步状态记录，铁电起始地址（8字节）
#define StartADD_MB85RS2DeviceSN   8          // 设备SN条码编号（3字节）地址

#define APP_Copy_Flag_ADD        MB85RS2_Size-1   //铁电最后一个地址作为APP拷贝标志地址 
#define JUMP_FCT_Flag_ADD        MB85RS2_Size-4   //铁电倒数第4字节作为FCT程序跳转标志
#define MB85R_SPI_UNIT           (M4_SPI6)
#define MB85R_NSS_PORT           (GPIO_PORT_B)   //片选IO定义
#define MB85R_NSS_PIN            (GPIO_PIN_15)
#define MB85R_SCK_PORT           (GPIO_PORT_G)
#define MB85R_SCK_PIN            (GPIO_PIN_13)
#define MB85R_SCK_FUNC           (GPIO_FUNC_46_SPI6_SCK)
#define MB85R_MISO_PORT          (GPIO_PORT_G)
#define MB85R_MISO_PIN           (GPIO_PIN_12)
#define MB85R_MISO_FUNC          (GPIO_FUNC_48_SPI6_MISO)
#define MB85R_MOSI_PORT          (GPIO_PORT_G)
#define MB85R_MOSI_PIN           (GPIO_PIN_14)
#define MB85R_MOSI_FUNC          (GPIO_FUNC_47_SPI6_MOSI)

    /**********************************************************************************************************
    **                                           类型定义区
    **********************************************************************************************************/
    
    /*SPI片选函数注册类型*/
    typedef  struct _CS0_MB85
    {
        void (*_select)(void);	 ///< 片选0选中
        void (*_diselect)(void); ///< 片选0解除

    }CS0_MB85;

    /*SPI接口和初始化函数注册类型*/

    typedef  struct _SPI0_MB85
    {
        uint32_t (*_tran_re_byte)(const void * wb,void *rb,uint32_t len);   ///写读传输bytes函数
        void (*_init)(void);                   ///接口初始化函数
    }SPI0_MB85;

    /*用户注册函数类型结构体*/
    typedef  struct __UserFunMB85_Type
    {
        uint32_t ID[5];               //保存设备ID
        uint32_t MB85RS2_Size[5];     //保存设备存储空间大小
        
        /**
         * 延时函数注册类型
         */
        struct  _DELAY_MB85
        {
            void (*_delyus)(uint32_t us); ///< 延时函数US
            void (*_delyms)(uint32_t ms); ///< 延时函数MS
        } DELAY_MB85;


        /**
         * 临界保护函数注册类型
         */
         struct  _CRIS0_MB85
        {
            void (*_enter)(void); ///< 临界保护区进入函数
            void (*_exit)(void);  ///< 临界保护区退出函数
        } CRIS0_MB85;


        CS0_MB85 CS_MB85[MX_NUM];    //针对多片应用，使用结构体数组存储函数指针

        SPI0_MB85 SPI_MB85[MX_NUM];    //针对多片应用，使用结构体数组存储函数指针

    } UserFunMB85_Type;


    /**********************************************************************************************************
    **                                         外部引用变量区
    **********************************************************************************************************/
   extern uint32_t MB85RS2_Size;

    /**********************************************************************************************************
    **                                         外部引用函数区
    **********************************************************************************************************/
    void MB85Ctrl_Read(uint32_t addr, uint8_t *buf, uint16_t len, uint8_t index);
    void MB85Ctrl_Write(uint32_t addr, uint8_t *buf, uint16_t len, uint8_t index);
    void MB85Ctrl_ReadID(uint32_t *buf,uint8_t index);
    uint8_t MB85Ctrl_CheckWrite(uint32_t addr, uint8_t *buf, uint16_t len, uint8_t index);
    void bsp_InitMB85R(void);
#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
