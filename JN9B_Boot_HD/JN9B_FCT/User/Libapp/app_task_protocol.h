/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_task_protocol.h
** Author      : Cai Heng
** Date        : 2021-08-08
** Description :
**********************************************************************************************************/

#ifndef _APP_TASK_PROTOCOL_H_
#define _APP_TASK_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "hc32_ddl.h"
#include "cmsis_os2.h"
#include "bsp_lan.h"

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
/* 帧头帧尾 */
#define FRAME_HEAD_RECV						0x5A5A		/* 数据方向：中位机 -> 下位机 */
#define	FRAME_TAIL_RECV						0xA5A5
#define FRAME_HEAD_SEND_OLD					0x5A5A		/* 数据方向：下位机 -> 中位机 */
#define FRAME_HEAD_SEND_NEW					0x5B5B
#define	FRAME_TAIL_SEND_OLD					0xA5A5
#define	FRAME_TAIL_SEND_NEW					0xA5A5
/* 解析结果 */
#define FRAME_IS_OK							0			/* 帧解析正确 */
#define FRAME_HEAD_ERR						1			/* 帧头错误 */
#define	FRAME_LEN_ERR						2			/* 帧长错误 */
#define FRAME_FEAT_ERR						3			/* 特征码错误 */
#define FRAME_STATE_ERR						4			/* 状态码错误 */
#define FRAME_CRC_ERR						5			/* 校验码错误 */
#define FRAME_TAIL_ERR						6			/* 帧尾错误 */
#define FRAME_PCMD_ERR						7			/* 主命令码错误 */
#define FRAME_SCMD_ERR						8			/* 子命令码错误 */
#define FRAME_PARAM_ERR						9			/* 帧形参错误 */
#define FRAME_DATA_ERR						10			/* 数据内容格式错误 */
/* 发送方式 */
#define SEND_TOGETHER						0			/* 整包发送 */
#define SEND_SEPARATE						1			/* 分包发送 */

/* 主命令码 */
/* 1 保护类 */
/* 全局保护 */
#define	CMD_GLOBAL_PROTECT_SET				0x01		/* 全局保护设置命令 */
#define	CMD_GLOBAL_PROTECT_SET_REPLY		0x81		/* 全局保护设置回复 */
#define	CMD_GLOBAL_PROTECT_INQ				0x02		/* 全局保护设置查询命令 */
#define	CMD_GLOBAL_PROTECT_INQ_REPLY		0x82		/* 全局保护设置查询回复 */
#define	CMD_GLOBAL_PROTECT_TRIG				0x83		/* 全局保护触发命令 */
/* 内部保护 */
#define CMD_INTERNAL_PROTECT_SET			0x0F		/* 内部保护设置命令 */
#define CMD_INTERNAL_PROTECT_SET_REPLY		0x8F		/* 内部保护设置回复 */
#define CMD_INTERNAL_PROTECT_INQ			0x10		/* 内部保护设置查询命令 */
#define CMD_INTERNAL_PROTECT_INQ_REPLY		0x90		/* 内部保护设置查询回复 */
#define CMD_INTERNAL_PROTECT_TRIG			0xA0		/* 内部保护触发命令 */
/* AC/DC保护 */
#define CMD_ACDC_PROTECT_TRIG				0xA1		/* AC/DC保护触发命令 */
/* 2 记录类 */
#define	CMD_RECORD_INQ						0x04		/* 记录数据查询命令 */
#define CMD_RECORD_INQ_REPLY				0x84		/* 记录数据查询回复 */
/* 3 工步类 */
#define CMD_WORKSTEP_SET					0x05		/* 工步设置命令 */
#define CMD_WORKSTEP_SET_REPLY				0x85		/* 工步设置回复 */
#define CMD_WORKSTEP_RUNSTA_SET				0x06		/* 工步运行状态设置命令 */
#define CMD_WORKSTEP_RUNSTA_SET_REPLY		0x86		/* 工步运行状态设置回复 */
#define CMD_WORKSTEP_INQ					0x07		/* 工步查询命令 */
#define CMD_WORKSTEP_INQ_REPLY				0x87		/* 工步查询回复 */
#define CMD_WORKSTEP_PROTECT_TRIG			0x88		/* 工步保护触发命令 */
/* 4 校准类 */
#define	CMD_CALIBRATE_PARAM_SET				0x09		/* 校准参数设定命令 */
#define	CMD_CALIBRATE_PARAM_SET_REPLY		0x89		/* 校准参数设定回复 */
#define	CMD_CALIBRATE_PARAM_INQ				0x0A		/* 校准参数查询命令 */
#define	CMD_CALIBRATE_PARAM_INQ_REPLY		0x8A		/* 校准参数查询回复 */
#define	CMD_AD_DATA_INQ						0x0B		/* AD数据读取命令 */
#define	CMD_AD_DATA_INQ_REPLY				0x8B		/* AD数据读取回复 */
/* 5 系统类 */
#define CMD_SYSTEM_INQ						0x0C		/* 系统查询命令 */
#define CMD_SYSTEM_INQ_REPLY				0x8C		/* 系统查询回复 */
#define CMD_SYSTEM_SET						0x0D		/* 系统设置命令 */
#define CMD_SYSTEM_SET_REPLY				0x8D		/* 系统设置回复 */
#define CMD_SYSTEM_UPDATE					0x0E		/* 系统升级命令/回复 */
/* 6 调试类 */
#define CMD_DEBUG_PARAM_SET					0x21		/* 调试参数设置命令/回复 */
#define CMD_DEBUG_DATA_INQ					0x22		/* 调试数据查询命令/回复 */
#define CMD_DEBUG_SYSTEM_SET				0x23		/* 调试系统设置命令/回复 */
/* 7 升级类 */
#define CMD_ACDC_UPDATE						0x60		/* AC/DC升级命令 */
#define CMD_ACDC_UPDATE_REPLY				0xE0		/* AC/DC升级回复 */
/* 8 无效类 */
#define	CMD_INVALID							0xFF		/* 无效命令 */


/* 子命令失能(保护) */
#define SCMD_DISABLE						(1 << 7)

/* 子命令码 */
/* 1 保护类 */
/* 全局保护 */
#define SCMD_GLOBAL_PROTECT_SET_BEGIN		0x00
#define SCMD_GLOBAL_PROTECT_SET_END			0x0B
#define SCMD_GLOBAL_PROTECT_NSET_BEGIN		(SCMD_GLOBAL_PROTECT_SET_BEGIN | SCMD_DISABLE)
#define SCMD_GLOBAL_PROTECT_NSET_END		(SCMD_GLOBAL_PROTECT_SET_END | SCMD_DISABLE)
#define	SCMD_GLOBAL_PROTECT_INQ_BEGIN		SCMD_GLOBAL_PROTECT_SET_BEGIN
#define SCMD_GLOBAL_PROTECT_INQ_END			SCMD_GLOBAL_PROTECT_SET_END
#define	SCMD_GLOBAL_PROTECT_TRIG_BEGIN		0x00
#define	SCMD_GLOBAL_PROTECT_TRIG_END		0x0C
/* 内部保护 */
#define SCMD_INTERNAL_PROTECT_SET_BEGIN		0x00
#define SCMD_INTERNAL_PROTECT_SET_END		0x12
#define SCMD_INTERNAL_PROTECT_NSET_BEGIN	(SCMD_INTERNAL_PROTECT_SET_BEGIN | SCMD_DISABLE)
#define SCMD_INTERNAL_PROTECT_NSET_END		(SCMD_INTERNAL_PROTECT_SET_END | SCMD_DISABLE)
#define SCMD_INTERNAL_PROTECT_INQ_BEGIN		SCMD_INTERNAL_PROTECT_SET_BEGIN
#define SCMD_INTERNAL_PROTECT_INQ_END		SCMD_INTERNAL_PROTECT_SET_END
#define SCMD_INTERNAL_PROTECT_TRIG_BEGIN	0x00
#define SCMD_INTERNAL_PROTECT_TRIG_END		0x14
/* AC/DC保护 */
#define SCMD_ACDC_PROTECT_TRIG_BEGIN		0x00
#define SCMD_ACDC_PROTECT_TRIG_END			0x0C
/* 3 工步类 */
/* 工步号 */
#define SCMD_WORKSTEP_NUM_BEGIN				0x00
#define SCMD_WORKSTEP_NUM_END				0xFF
/* 工步类型 */
#define SCMD_WORKSTEP_TYPE_BEGIN			0x00
#define SCMD_WORKSTEP_TYPE_STANDBY			0x01		/* 静置     */
#define SCMD_WORKSTEP_TYPE_CC				0x02		/* CC       */
#define SCMD_WORKSTEP_TYPE_CV				0x03		/* CV       */
#define SCMD_WORKSTEP_TYPE_CCTOCV			0x04		/* CCtoCV   */
#define SCMD_WORKSTEP_TYPE_DCC				0x05		/* DCC      */
#define SCMD_WORKSTEP_TYPE_DCV				0x06		/* DCV      */
#define SCMD_WORKSTEP_TYPE_DCCTODCV			0x07		/* DCCtoDCV */
#define SCMD_WORKSTEP_TYPE_CP				0x08		/* CP       */
#define SCMD_WORKSTEP_TYPE_DCP				0x09		/* DCP      */
#define SCMD_WORKSTEP_TYPE_FINISH			0x0A		/* 结束     */
#define SCMD_WORKSTEP_TYPE_END				0x0B
/* 工步(运行/截止)参数类型 */
#define SCMD_WORKSTEP_PARAM_BEGIN			0x00
#define SCMD_WORKSTEP_PARAM_TIME			0x01		/* 时间 */
#define SCMD_WORKSTEP_PARAM_VOLTAGE			0x02		/* 电压 */
#define SCMD_WORKSTEP_PARAM_CURRENT			0x03		/* 电流 */
#define SCMD_WORKSTEP_PARAM_CAPACITY		0x04		/* 容量 */
//#define SCMD_WORKSTEP_PARAM_ELECTRIC		0x05		/* 电量 */
#define SCMD_WORKSTEP_PARAM_END				0x05
/* 工步保护类型 */
#define SCMD_WORKSTEP_PROT_BEGIN			0x00
#define SCMD_WORKSTEP_PROT_VLIMITH			0x01		/* 电压上限保护 */
#define SCMD_WORKSTEP_PROT_VLIMITL			0x02		/* 电压下限保护 */
#define SCMD_WORKSTEP_PROT_OVI				0x03		/* 过流保护     */
#define SCMD_WORKSTEP_PROT_OVC				0x04		/* 过容保护     */
#define SCMD_WORKSTEP_PROT_VSUPERDEV		0x05		/* 电压超差保护 */
#define SCMD_WORKSTEP_PROT_ISUPERDEV		0x06		/* 电流超差保护 */
#define SCMD_WORKSTEP_PROT_END				0x0C
#define SCMD_WORKSTEP_NPROT_BEGIN			(SCMD_WORKSTEP_PROT_BEGIN | SCMD_DISABLE)
#define SCMD_WORKSTEP_NPROT_END				(SCMD_WORKSTEP_PROT_END | SCMD_DISABLE)
/* 工步运行状态设置类型 */
#define WORKSTEP_RUNSTA_SET_STOP			0x00		/* 停止 */
#define WORKSTEP_RUNSTA_SET_PAUSE			0x01		/* 暂停 */
#define WORKSTEP_RUNSTA_SET_START			0x02		/* 启动 */
#define WORKSTEP_RUNSTA_SET_CONTINUE		0x03		/* 继续 */
#define WORKSTEP_RUNSTA_SET_SKIP			0x04		/* 跳转 */
#define WORKSTEP_RUNSTA_SET_END				0x05
/* 工步运行状态反馈类型 */
#define WORKSTEP_RUNSTA_REF_STOP			0x00		/* 停止         */
#define WORKSTEP_RUNSTA_REF_PAUSE			0x01		/* 暂停         */
#define WORKSTEP_RUNSTA_REF_RUNNING			0x02		/* 运行中       */
#define WORKSTEP_RUNSTA_REF_COMPLETE		0x03		/* 完成         */
#define WORKSTEP_RUNSTA_REF_START			0x04		/* 工艺流程开始 */
#define WORKSTEP_RUNSTA_REF_WAIT			0x05		/* 等待         */
#define WORKSTEP_RUNSTA_REF_CONTACT			0x06		/* 接触测试     */

/* 4 校准类 */
#define SCMD_CALIBRATE_PARAM_SET_BEGIN		0x00
#define SCMD_CALIBRATE_PARAM_SET_END		0x25
#define SCMD_CALIBRATE_PARAM_INQ_BEGIN		SCMD_CALIBRATE_PARAM_SET_BEGIN
#define SCMD_CALIBRATE_PARAM_INQ_END		SCMD_CALIBRATE_PARAM_SET_END
#define	SCMD_AD_DATA_INQ_BEGIN				0x00
#define	SCMD_AD_DATA_INQ_END				0x05
/* 5 系统类 */
#define SCMD_SYSTEM_INQ_BEGIN				0x00
#define SCMD_SOFTWARE_VERSION				0x01		/* 软件版本查询 */
#define SCMD_WARMMING_MESSAGE				0x02		/* 告警信息查询 */
#define SCMD_SYSTEM_MODE  					0x03        /* 系统模式查询 */
#define SCMD_SYSTEM_CLIENT					0x04        /* 客户端远端设置查询 */
#define SCMD_SYSTEM_MAC_Q   	    	    0x05        /* MAC设置查询 */
#define SCMD_SYSTEM_BAR_Q    	    	    0x06        /* 条码设置查询 */
#define SCMD_SYSTEM_FRAM_Q  	    	    0x07        /* 铁电设置查询 */
#define SCMD_SYSTEM_EE_Q  	    	        0x08        /* EE设置查询 */
#define SCMD_SYSTEM_FLASH_Q  	    	    0x09        /* flash设置查询 */
#define SCMD_SYSTEM_PG_Q  	    	        0x0a        /* PG状态查询 */
#define SCMD_SYSTEM_AC_Q  	    	        0x0b        /* AC侧电压查询 */
#define SCMD_SYSTEM_ACDC_VERSION_Q  	    0x0c        /* ACDC版本查询 */
#define SCMD_SYSTEM_INQ_END					0x0d

#define SCMD_SYSTEM_SET_BEGIN				0x00
#define SCMD_SYSTEM_SET_END					0x14
#define SCMD_SYSTEM_UPDATE_BEGIN			0x00
#define SCMD_SYSTEM_UPDATE_END				0x06
/* 6 调试类 */
#define SCMD_DEBUG_SYSTEM_SET_BEGIN			0x01
#define SCMD_DEBUG_SYSTEM_SET_END			0x03
/* 7 升级类 */
#define SCMD_ACDC_UPDATE_SET_BEGIN          0x01    	/*升级设置子命令起始值*/
#define SCMD_ACDC_UPDATE_SET_END            0x03    	/*升级设置子命令结束值*/
#define SCMD_ACDC_UPDATE_INQ_BEGIN          0x01    	/*升级设置回复子命令起始值*/
#define SCMD_ACDC_UPDATE_INQ_END            0x03    	/*升级设置回复子命令结束值*/
/* 8 无效类 */
#define	SCMD_INVALID						0xFF		/* 无效命令 */

#ifdef YCJ
/* 版本号 */
#define SOFTWARE_PRIVERSION					0x0C	/* 主版本号 */
#define SOFTWARE_SUBVERSION					0x1B	/* 子版本号 */
#else
/* 版本号(V2.28) */
#define SOFTWARE_PRIVERSION					0x02	/* 主版本号 */
#define SOFTWARE_SUBVERSION					0x1C	/* 子版本号 */
#endif

/* 复位与置位 */
#define SET_0(x, y)							(x &= ~(1 << y))
#define SET_1(x, y)							(x |=  (1 << y))

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
typedef struct
{
	uint16_t head;					/* 数据帧头  */
	uint16_t len;					/* 数据长度  */
	uint8_t  feat;					/* 特征码    */
	uint8_t  sta;					/* 状态码    */
	uint8_t  chEn;					/* 通道标志  */
	uint16_t crc;					/* CRC校验值 */
	uint16_t tail;					/* 数据帧尾  */
} s_ProtocolStruct;

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/
extern osSemaphoreId_t w5500_rcv_Semaphore;        /* 网口接收信号量ID */
extern rcv_Struct w5500_rcvbuf;

extern uint8_t workstep_flag_RunstaSet;
extern uint8_t workstep_flag_AbortPrio[8];
	
extern uint8_t workstep_flag_CriticalZone[8]; 

extern uint8_t heart_beat_flag;
	
extern uint8_t record_data_upcast_flag[8];

extern uint8_t err_send_flag;

/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/
extern uint16_t CRC16N(uint8_t *s, uint16_t len);

extern void RecordDatSync(uint8_t chx);

extern void RecordDatUpcast(uint8_t chx, uint8_t sync_flag);

extern void WorkstepProtUpcast(uint8_t chx);

extern void AppTaskProtocol(void *argument);


#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
