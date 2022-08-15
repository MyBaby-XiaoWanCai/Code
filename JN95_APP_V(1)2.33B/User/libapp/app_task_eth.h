/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_task_eth.h
** Author      : Cai Heng
** Date        : 2021-06-17
** Description :
**********************************************************************************************************/

#ifndef _APP_TASK_ETH_H_
#define _APP_TASK_ETH_H_

#ifdef __cplusplus
extern "C" {
#endif

    /**********************************************************************************************************
    **                                          头文件引用区
    **********************************************************************************************************/
#include "stm32f4xx.h"

    /**********************************************************************************************************
    **                                            宏定义区
    **********************************************************************************************************/
#define ETH_TASK_PERIOD			5			//ETH任务周期设置(ms)
#define MSG_PRO_TASK_PERIOD		2			//MSG_PRO任务周期设置(ms)
#define NET_PORT_NUM			1001		//TCP服务器监听端口号

    /**********************************************************************************************************
    **                                           类型定义区
    **********************************************************************************************************/
typedef struct
{
 uint8_t Mode  ;  //网口模式 0 服务器 1 客户端
__IO uint32_t NetMod_cnt;     //网口模式切换计数，周期1ms
__IO uint8_t NetMod_Disable;       //禁止切换标志
}Net_Mod;

    /**********************************************************************************************************
    **                                         外部引用变量区
    **********************************************************************************************************/


    extern __IO uint8_t g_ucEthLinkStatus;



    /**********************************************************************************************************
    **                                         外部引用函数区
    **********************************************************************************************************/
    extern void W5500_Poll(void);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
