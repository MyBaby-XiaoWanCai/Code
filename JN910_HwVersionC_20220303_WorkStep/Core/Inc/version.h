/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : version.h
** Author      : Huang Cheng
** Date        : 2022-01-12
** Description : project version.
**********************************************************************************************************/
#ifndef _VERSION_H_
#define _VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**													更新记录
**	-------------------------------------------------------------------------------------------------------
**	Date		|Author		|Description
**	-------------------------------------------------------------------------------------------------------
**	2022-01-05	|Huang Cheng|1) 创建cubemx工程，配置底层外设驱动
**	------------|-----------|------------------------------------------------------------------------------
**	2022-01-07	|Wang Liang	|1) 评审cubemx工程，添加驱动模板、任务模板
**              |           |2) 添加铁电FRAM驱动
**              |           |3) 添加DWT定时器驱动
**	------------|-----------|------------------------------------------------------------------------------
**  2022-01-10  |Huang Cheng|1) 修改cubemx工程的ADC驱动通道映射配置
**              |           |2) 添加风扇、温度采集驱动
**              |           |3) 添加随机数生成驱动
**	------------|-----------|------------------------------------------------------------------------------
**  2022-01-11  |Wang Liang |1) 添加ADS131M02驱动
**              |Huang Cheng|2) 添加DAC80501驱动
**              |           |3) 添加充电切换逻辑驱动
**	------------|-----------|------------------------------------------------------------------------------
**  2022-01-12  |Huang Cheng|1) 添加模拟I2C驱动
**              |           |2) 添加PCA9555驱动
**	------------|-----------|------------------------------------------------------------------------------
**  2022-01-13  |Huang Cheng|1) 添加灯板驱动
**	------------|-----------|------------------------------------------------------------------------------
**  2022-01-14  |Huang Cheng|1) 添加拨码开关、故障读取驱动
**	------------|-----------|------------------------------------------------------------------------------
**  2022-01-17  |Huang Cheng|1) 移植eeprom驱动
**	------------|-----------|------------------------------------------------------------------------------
**  2022-01-21  |Huang Cheng|1) 风扇驱动测试正常
**              |           |2) 灯板驱动测试正常
**              |           |3) 拨码开关测试正常
**	------------|-----------|------------------------------------------------------------------------------
**  2022-01-24  |Huang Cheng|1) 温度采样测试正常
**	------------|-----------|------------------------------------------------------------------------------
**  2022-01-26  |Huang Cheng|1) 铁电FRAM测试正常
**              |           |2) EEPROM测试正常
**	------------|-----------|------------------------------------------------------------------------------
**  2022-01-27  |Huang Cheng|1) DAC80501驱动测试正常
**	------------|-----------|------------------------------------------------------------------------------
**  2022-03-02  |Huang Cheng|1) DCDC CAN收发测试正常，暂时为固定ID
**              |Huang Cheng|2) 优化I2C驱动代码，采用生成器模式，注册GPIO参数即可生成I2C设备驱动
**	------------|-----------|------------------------------------------------------------------------------
**  2022-03-03  |Huang Cheng|1) 移植蔡工的CAN收发组包机制
**              |Huang Cheng|2) 添加ADS131M02采样的逻辑，DRDY触发中断时间较短，因此采用定时开启关闭中断
**	------------|-----------|------------------------------------------------------------------------------
**********************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

