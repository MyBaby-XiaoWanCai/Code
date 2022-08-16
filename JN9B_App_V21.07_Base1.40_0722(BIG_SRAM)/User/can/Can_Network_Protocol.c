/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Can_Network_Protocol.c
** Author      : Cai Heng
** Date        : 2021-01-25
** Description : CAN组网协议层文件
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "Can_Network_Protocol.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
static u8_t upcast_feat_id = 128;
Can_Rx_Buffer_Struct can_rx_buffer __attribute__ ((at(0x20020000)));		/* 组包接收缓存区结构体 */

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : Can_Rx_Buffer_Clr
**  功能说明 : CAN接收缓存区清除
**  形    参 : buffer_id - 待清除的接收缓存区标号
**  返 回 值 : 无
**********************************************************************************************************/
static void Can_Rx_Buffer_Clr(u8_t buffer_id)
{
	if(buffer_id < MAX_RX_PACK)
	{
		memset(&can_rx_buffer.rx_pack[buffer_id], 0, sizeof(can_rx_buffer.rx_pack[buffer_id]));
	}
}

/**********************************************************************************************************
**  函 数 名 : Can_Network_Tx_Process
**  功能说明 : CAN组网数据包发送处理
**  形    参 : 无
**  返 回 值 :  0 - 整包数据发送完成
**	           -1 - 【错误】: 缓存区为空
**	           -2 - 【错误】: 压出数据长度超限
**********************************************************************************************************/
s8_t Can_Network_Tx_Process(void)
{
		u8_t tx_dat[DATA_PACK_LEN] = {0};	/* 待压出发送的发送缓存区数据       */
//	u8_t tx_dat[DATA_PACK_LEN] __attribute__((aligned (4))) = {0};	/* 待压出发送的发送缓存区数据       */
	
	u32_t tx_feat_id = 0;				/* 待压出发送的发送缓存区数据特征ID */
	u16_t tx_len = 0;					/* 待压出发送的发送缓存区数据长度   */
	s8_t tx_pop_ret = 0;				/* 压出结果 */
	
	/* 从发送缓存中压出待发送数据包 */
	tx_pop_ret = Network_Data_Pop(tx_dat, &tx_feat_id, &tx_len, &tx_buffer);
	
	/* 待发送数据包压出成功 */
	if(tx_pop_ret == 0)
	{
		Can_Data_Pack_Send(tx_feat_id, EXT_FRM_ID, tx_dat, tx_len);	/* CAN数据包发送 */
	}
	
	return tx_pop_ret;
}

/**********************************************************************************************************
**  函 数 名 : Err_Info_Tx_Process
**  功能说明 : 故障信息发送处理
**  形    参 : 无
**  返 回 值 : 
**********************************************************************************************************/
s8_t Err_Info_Tx_Process(void)
{
	return Err_Info_Pop();
}

/**********************************************************************************************************
**  函 数 名 : Can_Network_Analysis_Process
**  功能说明 :  CAN组网数据包解析处理
**  形    参 : 无
**  返 回 值 :  0  - 从接收缓存区压出处理的数据已完成解析并成功压入发送缓存区
**	           -1  - 【错误】：接收缓存区为空
**	           -2  - 【错误】：接收缓存区压出数据长度超限
**	           -3  - 【错误】：待压入发送缓存区数据长度超限
**	           -4  - 【错误】：发送缓存区待发送数据长度超限
**	           -5  - 【错误】：
**	           -6  - 【错误】：
**********************************************************************************************************/
s8_t Can_Network_Analysis_Process(void)
{
	u8_t rx_dat[DATA_PACK_LEN] = {0};	/* 待压出处理的接收缓存区数据       */
	u32_t rx_feat_id = 0;				/* 待压出处理的接收缓存区数据特征ID */
	u16_t rx_len = 0;					/* 待压出处理的接收缓存区数据长度   */
	s8_t rx_pop_ret = -1;				/* 压出结果 */
	
	s8_t analysis_ret = 0;				/* 解析处理结果 */
	
	u8_t tx_dat[DATA_PACK_LEN] = {0};	/* 待压入发送的解析处理数据       */
	u32_t tx_feat_id = 0;				/* 待压入发送的解析处理数据特征ID */
	u16_t tx_len = 0;					/* 待压入发送的解析处理数据长度   */
	s8_t tx_push_ret = -1;				/* 压入结果 */
	
	/* 从接收缓存中压出待处理数据包 */
	rx_pop_ret = Network_Data_Pop(rx_dat, &rx_feat_id, &rx_len, &rx_buffer);
	
	if(rx_pop_ret == 0)		/* 待处理数据包压出成功 */
	{
		/* 待处理数据包解析 */
		analysis_ret = Can_Data_Pack_Analysis(rx_dat, rx_len, tx_dat, &tx_len);
		
		if(analysis_ret == 0)	/* 待处理数据包解析成功 */
		{
			/* 更新待发送数据包特征ID */
			tx_feat_id = rx_feat_id | 0x00000003;	/* 数据以点播方式从下位机到中/上位机 */
		
			/* 将待发送数据包压入发送缓存 */
			tx_push_ret = Network_Data_Push_Tx_Buffer(tx_dat, tx_feat_id, tx_len, COMMON_REQUEST);
			
			if(tx_push_ret == 0)	/* 待发送数据包压入成功 */
			{
				return 0;	/* 从接收缓存区压出处理的数据已完成解析并成功压入发送缓存区 */
			}
			else					/* 待发送数据包压入失败 */
			{
				return (tx_push_ret - 2);	/* -3、-4 */
			}
		}
		else					/* 待处理数据包解析失败 */
		{
			Err_Info_Push(7, rx_feat_id, rx_len, rx_dat);
			
			return (analysis_ret - 4);		/* -5、-6、... */
		}
	}
	else					/* 待处理数据包压出失败 */
	{
		return rx_pop_ret;					/* -1、-2 */
	}
}

/**********************************************************************************************************
**  函 数 名 : Can_Network_Rx_Timeout
**  功能说明 : CAN组网数据包接收超时判断
**  形    参 : 无
**  返 回 值 : bit_x - 组网数据包缓存区x接收超时(x=0~15)
**********************************************************************************************************/
u32_t Can_Network_Rx_Timeout(u16_t rx_timeout, u16_t rx_timeout_period)
{
	u32_t ret = 0;
	
	/* 轮询接收数据缓存区空闲标识是否占用 */
	for(u8_t pack_seq = 0; pack_seq < MAX_RX_PACK; pack_seq++)
	{
		if(can_rx_buffer.rx_pack[pack_seq].pack_feat_id != 0)	/* 接收数据缓存区空闲标识已占用 */
		{	
			if(can_rx_buffer.rx_pack[pack_seq].pack_timeout >= (rx_timeout / rx_timeout_period - 1))
			{
				u8_t nop_dat[8] = {0};
				
				Err_Info_Push(8, can_rx_buffer.rx_pack[pack_seq].pack_feat_id, 8, nop_dat);
				
				Can_Rx_Buffer_Clr(pack_seq);		/* 【清除数据缓存区】 */
				
				ret |= 1 << pack_seq;	/* 【错误】: 数据包接收超时 */
			}
			else
			{
				can_rx_buffer.rx_pack[pack_seq].pack_timeout++;
			}
		}
		else													/* 接收数据缓存区空闲标识未占用 */
		{
			can_rx_buffer.rx_pack[pack_seq].pack_timeout = 0;
		}
	}
	
	return ret;
}

/**********************************************************************************************************
**  函 数 名 : Can_Network_Rx_Process
**  功能说明 : CAN组网数据包接收处理(由CAN接收中断调用)
**  形    参 : rx_id   - 接收数据帧ID
**	           rx_data - 接收数据帧数据域指针
**  返 回 值 : ≥0 - 整包数据接收完成(接收数据缓存区标号)
**	           -1  - 数据包接收中
**	           -2 - 【错误】: 接收缓存区溢出(既无匹配也无空闲的数据接收缓存区)
**	           -3 - 【错误】: 数据帧帧序溢出(超出组包数据接收缓存区最大帧序或数据帧帧序为0)
**	           -4 - 【错误】: 数据帧帧序超限(超出组包数据接收缓存区目标帧数)
**	           -5 - 【错误】: 数据帧帧序重复(组包数据接收缓存区接收到重复帧序的数据)
**	           -6 - 【错误】: 数据帧组包异常(组包数据缓存过程中出现异常)
**	           -7 - 【错误】：待压入接收缓存区数据长度超限
**	           -8 - 【错误】：接收缓存区待解析数据长度超限
**
**********************************************************************************************************/
s8_t Can_Network_Rx_Process(u32_t rx_id, u8_t* rx_data)		/* CAN接收中断调用 */
{
	u8_t frame_seq = (rx_id >> CAN_ID_BIT_OFFSET_FRM_SEQU) & 0xFF;			/* 帧序 */	
	u32_t feat_id = rx_id & (~(0x000000FF << CAN_ID_BIT_OFFSET_FRM_SEQU));	/* 特征ID */
	u8_t pack_match_flg = 0;	/* 数据缓存区包匹配标识 */
	u8_t pack_match_seq = 0;	/* 数据缓存区包匹配序号 */
	u8_t quo = 0;	/* 商 */
	u8_t rem = 0;	/* 余 */
	
	/* 1. 接收判断 */
	
	/* 轮询接收数据缓存区空闲标识是否匹配 */
	for(u8_t pack_seq = 0; pack_seq < MAX_RX_PACK; pack_seq++)
	{
		if(can_rx_buffer.rx_pack[pack_seq].pack_feat_id == feat_id)		/* 空闲标识与数据包特征ID匹配 */
		{
			pack_match_flg = 1;
			pack_match_seq = pack_seq;
			break;
		}
	}
	
	/* 轮询接收数据缓存区是否有空闲缓存 */
	if(pack_match_flg == 0)
	{
		for(u8_t pack_seq = 0; pack_seq < MAX_RX_PACK; pack_seq++)
		{
			if(can_rx_buffer.rx_pack[pack_seq].pack_feat_id == 0)		/* 数据包缓存区空闲 */
			{
				can_rx_buffer.rx_pack[pack_seq].pack_feat_id = feat_id;
				pack_match_flg = 1;
				pack_match_seq = pack_seq;
				break;
			}
		}
	}
	
	/* 接收数据缓存区空闲标识既不匹配也无空闲 */
	if(pack_match_flg == 0)
	{
		Err_Info_Push(9, rx_id, 8, rx_data);
		
		return -2;		/* 【错误】: 接收缓存区溢出(既无匹配也无空闲的数据接收缓存区) */	/* 无其他处理 */
	}
	
	/* 数据帧帧序判断: 是否在[1,64]范围 */
	if((frame_seq < 1) || (frame_seq > MAX_RX_FRAME))
	{
		Err_Info_Push(10, rx_id, 8, rx_data);
		
		Can_Rx_Buffer_Clr(pack_match_seq);		/* 【清除数据缓存区】 */
		
		return -3;		/* 【错误】: 数据帧帧序溢出(超出组包数据接收缓存区最大帧序或数据帧帧序为0) */
	}
	
	/* 数据帧帧序判断: 是否在目标帧数范围 */
	if(can_rx_buffer.rx_pack[pack_match_seq].target_frame_num != 0)
	{
		if(frame_seq > can_rx_buffer.rx_pack[pack_match_seq].target_frame_num)
		{
			Err_Info_Push(11, rx_id, 8, rx_data);
			
			Can_Rx_Buffer_Clr(pack_match_seq);	/* 【清除数据缓存区】 */
			
			return -4;	/* 【错误】: 数据帧帧序超限(超出组包数据接收缓存区目标帧数) */
		}
	}
	
	/* 数据帧对应缓存区是否占用 */
	quo = (frame_seq - 1) / 8;
	rem = (frame_seq - 1) % 8;
	if(can_rx_buffer.rx_pack[pack_match_seq].frame_idle_flag[quo] & (1 << rem))		/* 数据帧缓存区有数据 */
	{
		Err_Info_Push(12, rx_id, 8, rx_data);
		
		Can_Rx_Buffer_Clr(pack_match_seq);		/* 【清除数据缓存区】 */
		
		return -5;		/* 【错误】: 数据帧帧序重复(组包数据接收缓存区接收到重复帧序的数据) */
	}
	
	/* 2. 数据更新 */
	
	/* 首帧数据帧 */
	if(frame_seq == 1)
	{
		u16_t rx_data_len = (*(rx_data + 3) << 8) | *(rx_data + 2) + 4;		/* 数据包长度 */
		
		quo = rx_data_len / 8;
		
		rem = rx_data_len % 8;
		
		/* 更新目标帧数 */
		can_rx_buffer.rx_pack[pack_match_seq].target_frame_num = (rem == 0) ? quo : (quo + 1);
		
		/* 更新尾帧数据长度 */
		can_rx_buffer.rx_pack[pack_match_seq].tail_frame_len = (rem == 0) ? 8 : rem;
	}
	
	/* 更新最大缓存帧序 */
	if(can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq < frame_seq)
	{
		can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq = frame_seq;
	}
	
	/* 更新缓存帧数 */
	can_rx_buffer.rx_pack[pack_match_seq].recive_frame_num += 1;	
	
	/* 更新数据帧缓存区空闲标志 */
	quo = (frame_seq - 1) / 8;
	rem = (frame_seq - 1) % 8;	
	can_rx_buffer.rx_pack[pack_match_seq].frame_idle_flag[quo] |= (1 << rem);
	
	/* 更新数据帧缓存区 */
	for(u8_t dat_seq = 0; dat_seq < 8; dat_seq++)
	{
		can_rx_buffer.rx_pack[pack_match_seq].rx_frame[frame_seq - 1].dat[dat_seq] = *(rx_data + dat_seq);
	}
	
	
	/* 3. 组包判断 */
	
	/* 尾帧 */	/* 最大缓存帧序=缓存帧数=目标帧数 */
	if( (can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq == can_rx_buffer.rx_pack[pack_match_seq].recive_frame_num) && \
		(can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq == can_rx_buffer.rx_pack[pack_match_seq].target_frame_num) )
	{
		/* 组包数据接收缓存区数据包长度 */
		u16_t pack_dat_len = (can_rx_buffer.rx_pack[pack_match_seq].target_frame_num - 1) * 8 + can_rx_buffer.rx_pack[pack_match_seq].tail_frame_len;
		
		/* 组包数据接收缓存区数据包压入rx_buffer */
		s8_t pack_push_ret = Network_Data_Push_Rx_Buffer(can_rx_buffer.rx_pack[pack_match_seq].rx_frame[0].dat, feat_id, pack_dat_len, COMMON_REQUEST);
	
		/* 清除被压入rx_buffer的组包数据接收缓存区 */
		Can_Rx_Buffer_Clr(pack_match_seq);		/* 【清除数据缓存区】 */
		
		if(pack_push_ret == 0)
		{
			return (s8_t)pack_match_seq;		/* 被压入的组包数据接收缓存区标号 */
		}
		else
		{
			return pack_push_ret - 6;
		}	
	}
	/* 已收到首帧的过程帧 */	/* 缓存帧数≤最大缓存帧序≤目标帧数(等号不同时成立) */
	else if( (can_rx_buffer.rx_pack[pack_match_seq].recive_frame_num <= can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq) && \
			 (can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq <= can_rx_buffer.rx_pack[pack_match_seq].target_frame_num) )
	{
		/* Do Nothing */
	}
	/* 未收到首帧的过程帧 */	/* 缓存帧数≤最大帧序且目标帧数=0 */
	else if( (can_rx_buffer.rx_pack[pack_match_seq].recive_frame_num <= can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq) && \
			 (can_rx_buffer.rx_pack[pack_match_seq].target_frame_num == 0) )
	{
		/* Do Nothing */
	}
	/* 异常组包帧 */
	else
	{
		Err_Info_Push(13, rx_id, 8, rx_data);
		
		Can_Rx_Buffer_Clr(pack_match_seq);		/* 【清除数据缓存区】 */
		
		return -6;		/* 【错误】: 数据帧组包异常(组包数据缓存过程中出现异常) */
	}
	
	return -1;
}

/**********************************************************************************************************
**  函 数 名 : Can_Network_Upcast
**  功能说明 : CAN组网数据主动上抛(等待调用)
**  形    参 : 无
**  返 回 值 : 
**********************************************************************************************************/
s8_t Can_Network_Upcast(u8_t* tx_dat, u16_t tx_len)                        /* 等待调用 */
{
	s8_t upcast_ret = 0;
	if(heart_beat_flag == 2)
	{
			return upcast_ret;
	}	
//	u32_t feat_id = (4 << 26) | (upcast_feat_id << 18) | (0 << 10) | (0 << 2) | (1 << 1) | (1 << 0);
	
	u32_t feat_id = (DEV_TYPE << DEV_TYPE_OFFSET) | (upcast_feat_id << FEAT_CODE_OFFSET) | (FRAM_SEQ << FRAM_SEQ_OFFSET) | (DEV_ADDR << DEV_ADDR_OFFSET) | (COM_TYPE_VOD << COM_TYPE_OFFSET) | (COM_DIR_UP << COM_DIR_OFFSET);
	
	if(upcast_feat_id < 239)
	{
		upcast_feat_id++;
	}
	else
	{
		upcast_feat_id = 128;
	}
	
	upcast_ret = Network_Data_Push_Tx_Buffer(tx_dat, feat_id, tx_len, COMMON_REQUEST);
	
	return upcast_ret;
}


static u16_t rx_analysis_cnt = 0;
static u16_t tx_reply_cnt = 0;
static u16_t tx_err_cnt = 0;

/**********************************************************************************************************
**  函 数 名 : Can_Network_Func
**  功能说明 : 
**  形    参 : 
**  返 回 值 : 
**********************************************************************************************************/
s8_t Can_Network_Func(u16_t rx_timeout_period, u16_t rx_analysis_period, u16_t tx_reply_period, u16_t tx_err_period, u16_t period_unit)
{
	if((rx_timeout_period == 0) || (rx_analysis_period == 0) || (tx_reply_period == 0) || (tx_err_period == 0) || (period_unit == 0))
	{
		return -1;
	}
	
	Can_Network_Rx_Timeout(rx_timeout_period, period_unit);
	
	if(rx_analysis_cnt >= ((rx_analysis_period - 1) / period_unit))
	{
		rx_analysis_cnt = 0;
		
		Can_Network_Analysis_Process();
	}
	else
	{
		rx_analysis_cnt++;
	}
	
	if(tx_reply_cnt >= ((tx_reply_period - 1) / period_unit))
	{
		tx_reply_cnt = 0;
		
		Can_Network_Tx_Process();
	}
	else
	{
		tx_reply_cnt++;
	}
	
	if(tx_err_cnt >= ((tx_err_period - 1) / period_unit))
	{
		tx_err_cnt = 0;
		
		Err_Info_Tx_Process();
	}
	else
	{
		tx_err_cnt++;
	}
	
	return 0;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
