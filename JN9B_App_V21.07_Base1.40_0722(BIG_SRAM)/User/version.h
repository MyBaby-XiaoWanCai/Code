/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : version.h
** Author      : Cai Heng
** Date        : 2021-06-17
** Description : 
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
**	2021-07-23	|Cai Heng	|1) 增加PHYLINK状态检测，避免上电未插网线程序卡死
**				|			|2) bsp_spi.c及bsp_spi.h文件修改
**				|			|3) 添加AD7124及AD7175驱动程序并测试验证
**				|			|4) FLASH及EEPROM驱动程序测试验证
**	------------|-----------|------------------------------------------------------------------------------
**	2021-07-29	|Cai Heng	|1) 新增global.h文件(存放通用变量,如通讯结构体定义)
**				|			|2) 修改app_task_eeprom.c及app_task_eeprom.h文件,使用结构体进行存取操作并完成测试验证
**	------------|-----------|------------------------------------------------------------------------------
**  2021-11-22  |           |1) 新增HC32F4A0STIB芯片的底层驱动
**  2021-12-01  |           |2) 修复AD7124移植的bug
**  2021-12-01  |Huang Cheng|3) 可运行AppTaskLed任务
**  2021-12-02  |           |4) 修复W5500重复初始化SPI的bug，建议不要使用go to语句
**              |           |5) 修复网口断开后重连不上的bug，删除sprintf打印网口信息，修改w5500socket.文件（为适应ringbuffer读写）
**  2021-12-03  |           |6) 修改文件夹目录结构
**              |           |7) 可运行AppTaskLed、AppTaskEth、AppTaskAgeing、AppTaskProtocol任务
**  2021-12-06  |           |8) osTimerStart开启软定时器会导致TimerA运行异常，具体原因待查，内部adc采集放在TimerA_Uint3中处理
**              |           |9) 修复AD7124读取器件ID无CRC校验的bug，读取器件ID返回的数据不带CRC校验数据
**  2021-12-07  |           |10)修复I2C初始化的bug，原因是stcGpioCfg.u16PinOType选成stcGpioCfg.u16PinIType
**  2021-12-08  |           |11)添加ACDC数据处理代码，CAN配置采用将满中断，最多缓冲3帧数据最大可设置8帧
**  2021-12-10  |           |12)修复CAN发送函数的bug：定时器轮询发送时会出现发送STB FIFO满的问题，修改
**              |           |   原来发送方式CAN_STB_TRANS_ONE为CAN_STB_TRANS_ALL即可解决此问题
**              |           |13)当前版本 程序可以跑对充对放流程，数据上抛暂无异常，校准参数设置功能暂未测试
**  2021-12-14  |           |14)校准参数设置查询无问题，该H版控制版拨码开关第1位无法读取，同样程序在F版控制板可正常读取
**              |           |15)该本版软件可正常跑老化程序
**	------------|-----------|------------------------------------------------------------------------------
**
**	------------|-----------|------------------------------------------------------------------------------
**
**	JN9B下位机代码修改记录
**
**	------------|-----------|------------------------------------------------------------------------------
**  
**	2022-02-11 增加主控板TMP75采集的验证
**  2022-02-12 风扇PWM输出的验证
** 	2022-02-13 修改 Mod_Rev_Prot 初始化模式，以及PCA9548初始化打开方式，增加多通道的切换选择控制
**	2022-02-14 修改ageing 老化部分代码,修改保护部分最大值限制Protocol ITEMP_MAX
**	2022-02-15 修改ADC3采样滤波次数以及SN开关切换延时、当上位机设置的校准系数超过范围时，会导致任务死掉，定时器仍然正常工作
**  2022-02-16 修改了ADC采样滤波次数、修改灯板控制逻辑
**  2022-02-17 增加通过读取控制板温度来控制风扇PWM输出、增加在位信号、过压信号以及风扇故障信号检测读取、增加功率板故障信号检测
**  2022-02-18 搭建整机FCT代码框架、增加功率板温度采集以及上传、修改LOOP及MS信号的初始化状态及控制方式
**  2022-02-23 将APP代码移植到JN952.32E版本上并测试
**  2022-02-24 电流上升时间的修改、更新ADC3部分驱动以及硬件故障上抛
**  2022-02-26 对过压信号不触发INT中断进行排查(功率板PCA9555INT接了下拉)
**  2022-02-28 将代码应用部分更新至与JN95最新版同步
**  2022-03-01 增加PCA9548通道选择回读功能
**  2022-03-28 增加温度采集异常数据过滤部分代码
**	2022-04-19 增加CAN错误中断处理、修改了整机老化的时间及整机FCT结束时，点亮状态和标志写入的顺序。
**	2022-04-21 N9B整机老化及FCT流程修改为：当上电读取拨码IP为 192.168.8.41时，便默认进入整机FCT模式。当上电读取拨码IP为 192.168.8.42时，先读取整机FCT结果，若全OK，则进入老化模式。
**	2022-04-29 将JN9B下位机BOvp电压设置逻辑修改为：在初始化时将过压保护设置为最高7.1V，在程序正常运行起来后，再设置回5.1V。
**  2022-05-17 修改硬件故障部分读取逻辑、在硬件故障触发时、只作标志，随后在定时器中读取
**  2022-06-06 对CPU异常告警问题进行了优化、优化了EE驱动(修改为单字节读写)以初始化写EE的顺序、对开机初始化顺序及时间进行了修改优化
**  2022-06-07 按照需求对 240A 机型的硬件故障上报、Debug、以及老化部分逻辑进行了修改
**  2022-06-21 增加通过灯板实现对并机机型的识别功能
**  2022-06-22 根据厦门现场实际使用场景，将JB应用部分代码作相应的修改
**  2022-06-30 修改240A关机顺序
**  2022-07-07 修改硬件故障触发清除逻辑 - 硬件故障触发时，点击清除故障后，熄灭对应通道红灯、增加AD7606以及SGM58601通信故障检测
**  2022-07-08 增加中断优先级分组及调整部分定时器回调函数功能、具体优先级分配可见"使用说明部分"
**  2022-07-12 修改进入CV模式的延迟时间
**
**	------------|-----------|------------------------------------------------------------------------------
**
**********************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
