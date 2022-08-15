/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_eeprom.h
** Author      : Cai Heng        Huang Cheng
** Date        : 2021-06-16      2022-01-17(modify)
** Description : 
**********************************************************************************************************/

#ifndef _BSP_EEPROM_H_
#define _BSP_EEPROM_H_

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
#define AT24C256						// 配置器件类型

#ifdef AT24C02
#define EE_MODEL_NAME	"AT24C02"
#define EE_DEV_ADDR		0xA0			// 设备地址       (Configurable)
#define EE_PAGE_SIZE	8				// 页面大小(字节) (Configurable)
#define EE_SIZE			256				// 总容量(字节)   (Configurable)
#define EE_ADDR_BYTES	1				// 地址字节个数   (Configurable)
#endif

#ifdef AT24C128
#define EE_MODEL_NAME	"AT24C128"
#define EE_DEV_ADDR		0xA0			// 设备地址       (Configurable)
#define EE_PAGE_SIZE	64				// 页面大小(字节) (Configurable)
#define EE_SIZE			16 * 1024		// 总容量(字节)   (Configurable)
#define EE_ADDR_BYTES	2				// 地址字节个数   (Configurable)
#endif

#ifdef AT24C256
#define EE_MODEL_NAME	"AT24C256"
#define EE_DEV_ADDR		0xA0			// 设备地址       (Configurable)
#define EE_PAGE_SIZE	64				// 页面大小(字节) (Configurable)
#define EE_SIZE			32 * 1024		// 总容量(字节)   (Configurable)
#define EE_ADDR_BYTES	2				// 地址字节个数   (Configurable)
#endif

/*************************************************************************************************************************
**                                         尾部特殊地址
--------------------------------------------------------------------------------------------------------------------------
ADD：|Last_addr-6           |Last_addr-5 ~ -3      |Last_addr-2     |Last_addr-1      |Last_addr        |   
     |MacEn_flag_addr(1字节)|MacID_flag_addr(3字节)|Test_addr(1字节)|BOOT1_addr(1字节)|BOOT2_addr(1字节)|
--------------------------------------------------------------------------------------------------------------------------                      
ADD: |Last_addr-15                |Last_addr-14                |Last_addr-13              |Last_addr-12                  |                       
     |BarCode_End_flag_addr(1字节)|BarCode_Len_flag_addr(1字节)|Remote_En_flag_addr(1字节)|Remote_IPPort_flag_addr(6字节)|                      
---------------------------------------------------------------------------------------------------------------------------                        
**************************************************************************************************************************/
#define Last_addr                      EE_SIZE-1              //末尾字节地址
#define BOOT2_addr                     Last_addr              //BOOT2地址
#define BOOT1_addr                     Last_addr-1            //BOOT1地址
#define Test_addr                      Last_addr-2            //自检测试地址
#define MacID_flag_addr                Last_addr-5            //MAC（后3字节）存储地址
#define MacEn_flag_addr                Last_addr-6            //MAC条码设置使能标志地址
#define Remote_IPPort_flag_addr        Last_addr-12           //远端IP端口存储地址
#define Remote_En_flag_addr            Last_addr-13           //远端IP端口设置使能标志地址
#define BarCode_Len_flag_addr          Last_addr-14           //条码长度存储地址
#define BarCode_End_flag_addr          Last_addr-15           //条码结束字节存储地址
/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
void bsp_EepromInit(void);
uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytesWithCheck(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
