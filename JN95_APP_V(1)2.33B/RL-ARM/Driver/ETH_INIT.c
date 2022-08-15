/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : ETH_INIT.c
** Author      : Cai Heng
** Date        : 2021-06-17
** Description : 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/

/* 以太网连接状态：
**     0 - 初始临时状态
**     1 - 初始临时状态
**     2 - 已连接
**     3 - 已断开 
**/
__IO uint8_t g_ucEthLinkStatus = 0;  

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : netETH_Notify()
**	功能说明 : 以太网状态消息
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
void netETH_Notify (uint32_t if_num, netETH_Event event, uint32_t val) 
{
	NET_ETH_LINK_INFO *info;

	switch(event) 
	{
		case netETH_LinkDown:
			if(g_ucEthLinkStatus == 2)
			{
				g_ucEthLinkStatus = 3;
			}
			else
			{
				g_ucEthLinkStatus = 1;
			}
#if (ETH_INIT_PRINTF_CFG == ENABLE_CFG)
			APP_Printf("Link is down\r\n");
#endif
			break;
		
		case netETH_LinkUp:
			g_ucEthLinkStatus = 2;
#if (ETH_INIT_PRINTF_CFG == ENABLE_CFG)
			APP_Printf("Link is up\r\n");
#endif
			info = (NET_ETH_LINK_INFO *)&val;
			
			switch (info->speed) 
			{
				case 0:
#if (ETH_INIT_PRINTF_CFG == ENABLE_CFG)
					APP_Printf("10 MBit\r\n");
#endif
					break;
				
				case 1:
#if (ETH_INIT_PRINTF_CFG == ENABLE_CFG)
					APP_Printf("100 MBit\r\n");
#endif
					break;
				
				case 2:
#if (ETH_INIT_PRINTF_CFG == ENABLE_CFG)
					APP_Printf("1 GBit\r\n");
#endif
					break;
			}
			
			switch (info->duplex) 
			{
				case 0:
#if (ETH_INIT_PRINTF_CFG == ENABLE_CFG)
					APP_Printf("Half duplex\r\n");
#endif
					break;
				case 1:
#if (ETH_INIT_PRINTF_CFG == ENABLE_CFG)
					APP_Printf("Full duplex\r\n");
#endif
					break;
			}
			break;
		
		case netETH_Wakeup:
#if (ETH_INIT_PRINTF_CFG == ENABLE_CFG)
			APP_Printf("Wakeup frame received\r\n");
#endif
			break;
		
		case netETH_TimerAlarm:
#if (ETH_INIT_PRINTF_CFG == ENABLE_CFG)
			APP_Printf("Timer alarm\r\n");
#endif
			break;
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
