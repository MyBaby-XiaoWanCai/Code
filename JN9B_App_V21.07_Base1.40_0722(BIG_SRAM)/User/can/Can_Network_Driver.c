/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Can_Network_Driver.c
** Author      : Cai Heng
** Date        : 2021-01-25
** Description : CAN组网协议底层驱动文件
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "Can_Network_Driver.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
Data_Buffer_Struct rx_buffer __attribute__ ((at(0x20040000)));		/* 接收缓存区 */
Data_Buffer_Struct tx_buffer __attribute__ ((at(0x20046000)));		/* 发送缓存区 */
Err_Buffer_Struct err_buffer;		/* 故障缓存区 */

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

static void Err_Buffer_Clr(uint8_t _no)
{
	if(_no < MAX_ERR_INFO)
	{
		memset(&err_buffer.info[_no], 0, sizeof(err_buffer.info[_no]));
	}
}

/**********************************************************************************************************
**  函 数 名 : Err_Info_Pop
**  功能说明 : 
**  形    参 : 
**  返 回 值 : 
**********************************************************************************************************/
s8_t Err_Info_Pop(void)
{
	u8_t pop_handler = 0;
	u8_t err_info_dat[512 + 14] = {0};
	u16_t err_info_len = 0;
	u32_t err_info_id = 0;
	
	if(err_buffer.info_cnt == 0)
	{
		return -1;		/* 【错误】: 故障缓存区为空 */
	}

	err_buffer.info_cnt--;

	pop_handler = err_buffer.tail_ptr;
	
	if(err_buffer.tail_ptr < (MAX_ERR_INFO - 1))
	{
		err_buffer.tail_ptr++;
	}
	else
	{
		err_buffer.tail_ptr = 0;
	}

	Err_Info_Pack(pop_handler, err_info_dat, &err_info_len, &err_info_id);

	Err_Buffer_Clr(pop_handler);
	
	Can_Data_Pack_Send(err_info_id, EXT_FRM_ID, err_info_dat, err_info_len);
	
	return 0;
}

/**********************************************************************************************************
**  函 数 名 : Err_Info_Push
**  功能说明 : 
**  形    参 : 
**  返 回 值 : 
**********************************************************************************************************/
void Err_Info_Push(u8_t err_sta, u32_t err_id, u16_t err_len, u8_t* err_dat)
{
	u8_t push_handler = 0;
	
	while(err_buffer.info_cnt >= MAX_ERR_INFO)
	{
		Err_Info_Pop();
	}
	
	err_buffer.info_cnt++;

	push_handler = err_buffer.head_ptr;
	
	if(err_buffer.head_ptr < (MAX_ERR_INFO - 1))
	{
		err_buffer.head_ptr++;
	}
	else
	{
		err_buffer.head_ptr = 0;
	}

	err_buffer.info[push_handler].sta = err_sta;
	
	err_buffer.info[push_handler].id  = err_id;
	
	err_buffer.info[push_handler].len = err_len;
	
	memcpy(&err_buffer.info[push_handler].dat, err_dat, err_len);	
}

/**********************************************************************************************************
**  函 数 名 : Data_Handler_Request
**  功能说明 : 缓存区空闲内存申请
**  形    参 : p   - 待申请空闲内存的缓存区指针 rx_buffer / tx_buffer
**	           cmd - 申请优先级命令 COMMON_REQUEST - 普通优先级(先进先出,可插队)
**	                                MAJOR_REQUEST  - 紧急优先级(后进先出,不插队)
**  返 回 值 : ≥0 - 申请的缓存区空闲内存标号(0,DATA_PACK_NUM-1)
**	           -1 - 【错误】: 缓存区存储空间溢出
**********************************************************************************************************/
static s8_t Data_Handler_Request(Data_Buffer_Struct *p, u8_t cmd)
{
	u8_t request_handler = 0;		/* 请求的空闲缓存区指针 */

//	if(rx_buffer.pack_cnt >= DATA_PACK_NUM)
	if(p->pack_cnt >= DATA_PACK_NUM)
	{
		return -1;		/* 【错误】: 缓存区存储空间溢出 */
	}

	p->pack_cnt++;		/* 缓存数据包数自增 */
	
	if(cmd == MAJOR_REQUEST)		/* 紧急请求(最高处理优先级) */
	{
		/* 尾指针自减并返回自减后的缓存句柄作为空闲内存标号(0,DATA_PACK_NUM-1) */
		if(p->tail_ptr == 0)
		{
			p->tail_ptr = DATA_PACK_NUM - 1;
		}
		else
		{
			p->tail_ptr--;
		}

		request_handler = p->tail_ptr;
	}
	else							/* 普通请求(最低处理优先级) */
	{
		/* 头指针自增并返回自增前的缓存句柄作为空闲内存标号(0,DATA_PACK_NUM-1) */
		request_handler = p->head_ptr;		/* 待使用的空闲缓存区指针 */
		
		if(p->head_ptr < (DATA_PACK_NUM - 1))
		{
			p->head_ptr++;
		}
		else
		{
			p->head_ptr = 0;
		}
	}
	
	return (s8_t)request_handler;
}

/**********************************************************************************************************
**  函 数 名 : Data_Handler_Inquiry
**  功能说明 : 缓存区占用内存查询
**  形    参 : p - 待查询占用内存的缓存区指针 rx_buffer / tx_buffer
**  返 回 值 : ≥0 - 查询的缓存区占用内存标号(0,DATA_PACK_NUM-1)
**	           -1 - 【错误】: 缓存区为空
**********************************************************************************************************/
static s8_t Data_Handler_Inquiry(Data_Buffer_Struct *p)
{
	u8_t inquiry_handler = 0;		/* 查询待处理的缓存区指针 */

	if(p->pack_cnt == 0)
	{
		return -1;		/* 【错误】: 缓存区为空 */
	}

	p->pack_cnt--;		/* 缓存数据包数自减 */
	
	/* 尾指针自增并返回自增前的缓存句柄作为占用内存标号(0,DATA_PACK_NUM-1) */
	inquiry_handler = p->tail_ptr;		/* 待处理的数据缓存区指针 */
	
	if(p->tail_ptr < (DATA_PACK_NUM - 1))
	{
		p->tail_ptr++;
	}
	else
	{
		p->tail_ptr = 0;
	}	

	return (s8_t)inquiry_handler;
}

/**********************************************************************************************************
**  函 数 名 : Network_Data_Pop
**  功能说明 : 组包数据压出缓存
**  形    参 : 
**  返 回 值 :  0 - 组包数据压出缓存成功
**	           -1 - 【错误】: 缓存区为空
**	           -2 - 【错误】: 压出数据长度超限
**********************************************************************************************************/
s8_t Network_Data_Pop(u8_t* dat, u32_t* feat_id, u16_t* len, Data_Buffer_Struct *p)
{
	s8_t inq_handler = 0;
	
	inq_handler = Data_Handler_Inquiry(p);
	
	if(inq_handler < 0)
	{
		return -1;		/* 【错误】: 缓存区为空 */
	}
	
	if(p->data_pack[inq_handler].len > DATA_PACK_LEN)
	{		
		Err_Info_Push(1, p->data_pack[inq_handler].feat_id, p->data_pack[inq_handler].len, p->data_pack[inq_handler].dat);
		
		/* 清除查询取出数据的缓存区 */
		p->data_pack[inq_handler].feat_id = 0;
		
		p->data_pack[inq_handler].len = 0;
		
		memset(p->data_pack[inq_handler].dat, 0, DATA_PACK_LEN);
		
		return -2;		/* 【错误】: 压出数据长度超限 */
	}
	
	/* 将数据压出缓存区 */
	*feat_id = p->data_pack[inq_handler].feat_id;
	
	*len = p->data_pack[inq_handler].len;
	
	memcpy(dat, p->data_pack[inq_handler].dat, p->data_pack[inq_handler].len);
	
	/* 清除查询取出数据的缓存区 */
	p->data_pack[inq_handler].feat_id = 0;
	
	p->data_pack[inq_handler].len = 0;
	
	memset(p->data_pack[inq_handler].dat, 0, DATA_PACK_LEN);
	
	return 0;
}

/**********************************************************************************************************
**  函 数 名 : Network_Data_Push_Tx_Buffer
**  功能说明 : 组包数据压入发送缓存
**  形    参 : 
**  返 回 值 :  0 - 数据压入发送缓存区成功
**	           -1 - 【错误】：待压入发送缓存区数据长度超限
**	           -2 - 【错误】：发送缓存区待发送数据长度超限
**********************************************************************************************************/
s8_t Network_Data_Push_Tx_Buffer(u8_t* dat, u32_t feat_id, u16_t len, u8_t cmd)
{
	s8_t req_handler_tx = 0;
	s8_t inq_handler_tx = 0;
	s8_t push_ret = 0;
	
	if(len > DATA_PACK_LEN)
	{
		Err_Info_Push(2, feat_id, len, dat);		
		
		return -1;	/* 【错误】：待压入发送缓存区数据长度超限 */
	}
	
	/* 在发送缓存区申请空闲缓存 */
	req_handler_tx = Data_Handler_Request(&tx_buffer, cmd);
	
	if(req_handler_tx == -1)	/* 发送缓存区无空闲缓存 */
	{
		/* 在发送缓存区查询待发送缓存 */
		inq_handler_tx = Data_Handler_Inquiry(&tx_buffer);
		
		if(tx_buffer.data_pack[inq_handler_tx].len > DATA_PACK_LEN)
		{
			Err_Info_Push(3, tx_buffer.data_pack[inq_handler_tx].feat_id, tx_buffer.data_pack[inq_handler_tx].len, tx_buffer.data_pack[inq_handler_tx].dat);
			
			push_ret = -2;	/* 【错误】：发送缓存区待发送数据长度超限 */
		}
		else
		{
			/* 发送从发送缓存区查询取出数据 */
			Can_Data_Pack_Send(tx_buffer.data_pack[inq_handler_tx].feat_id, EXT_FRM_ID, tx_buffer.data_pack[inq_handler_tx].dat, tx_buffer.data_pack[inq_handler_tx].len);
		}
		
		/* 清除查询取出数据的发送缓存区 */
		tx_buffer.data_pack[inq_handler_tx].feat_id = 0;
		
		tx_buffer.data_pack[inq_handler_tx].len = 0;
		
		memset(tx_buffer.data_pack[inq_handler_tx].dat, 0, DATA_PACK_LEN);
		
		
		/* 申请空闲的发送缓存区存储数据 */
		req_handler_tx = Data_Handler_Request(&tx_buffer, cmd);
		
		/* 将数据压入发送缓存区 */
		tx_buffer.data_pack[req_handler_tx].feat_id = feat_id;
		
		tx_buffer.data_pack[req_handler_tx].len = len;
		
		memcpy(tx_buffer.data_pack[req_handler_tx].dat, dat, len);		

	}
	else						/* 发送缓存区有空闲缓存 */
	{
		/* 将数据压入发送缓存区 */
		tx_buffer.data_pack[req_handler_tx].feat_id = feat_id;
		
		tx_buffer.data_pack[req_handler_tx].len = len;
		
		memcpy(tx_buffer.data_pack[req_handler_tx].dat, dat, len);
	}
	
	return push_ret;
}


/**********************************************************************************************************
**  函 数 名 : Network_Data_Push_Rx_Buffer
**  功能说明 : 组包数据压入接收缓存
**  形    参 : 
**  返 回 值 :  0 - 数据压入接收缓存区成功
**	           -1 - 【错误】：待压入接收缓存区数据长度超限
**	           -2 - 【错误】：接收缓存区待解析数据长度超限
**	           
**********************************************************************************************************/
s8_t Network_Data_Push_Rx_Buffer(u8_t* dat, u32_t feat_id, u16_t len, u8_t cmd)
{
	s8_t req_handler_rx = 0;
	s8_t inq_handler_rx = 0;
	s8_t push_ret = 0;
	
	if(len > DATA_PACK_LEN)
	{
		Err_Info_Push(4, feat_id, len, dat);
		
		return -1;	/* 【错误】：待压入接收缓存区数据长度超限 */
	}
	
	/* 在接收缓存区申请空闲缓存 */
	req_handler_rx = Data_Handler_Request(&rx_buffer, cmd);
	
	if(req_handler_rx == -1)	/* 接收缓存区无空闲缓存 */
	{
		/* 在接收缓存区查询待解析缓存 */
		inq_handler_rx = Data_Handler_Inquiry(&rx_buffer);
		
		if(rx_buffer.data_pack[inq_handler_rx].len > DATA_PACK_LEN)
		{	
			Err_Info_Push(5, rx_buffer.data_pack[inq_handler_rx].feat_id, rx_buffer.data_pack[inq_handler_rx].len, rx_buffer.data_pack[inq_handler_rx].dat);
			
			push_ret = -2;	/* 【错误】：接收缓存区待解析数据长度超限 */
		}
		else
		{
			u8_t tx_dat[DATA_PACK_LEN] = {0};
			
			u32_t tx_feat_id = rx_buffer.data_pack[inq_handler_rx].feat_id | 0x00000003;
			
			u16_t tx_len = 0;
			
			s8_t analysis_ret = Can_Data_Pack_Analysis(rx_buffer.data_pack[inq_handler_rx].dat, rx_buffer.data_pack[inq_handler_rx].len, tx_dat, &tx_len);
		
			if(analysis_ret == 0)
			{	
				Network_Data_Push_Tx_Buffer(tx_dat, tx_feat_id, tx_len, COMMON_REQUEST);
			}
			else
			{
				Err_Info_Push(6, rx_buffer.data_pack[inq_handler_rx].feat_id, rx_buffer.data_pack[inq_handler_rx].len, rx_buffer.data_pack[inq_handler_rx].dat);
				
				push_ret = analysis_ret - 2;
			}
		}
		
		/* 清除查询取出数据的接收缓存区 */
		rx_buffer.data_pack[inq_handler_rx].feat_id = 0;
		
		rx_buffer.data_pack[inq_handler_rx].len = 0;
		
		memset(rx_buffer.data_pack[inq_handler_rx].dat, 0, DATA_PACK_LEN);
		
		
		/* 申请空闲的接收缓存区存储数据 */
		req_handler_rx = Data_Handler_Request(&rx_buffer, cmd);
		
		/* 将数据压入接收缓存区 */
		rx_buffer.data_pack[req_handler_rx].feat_id = feat_id;
		
		rx_buffer.data_pack[req_handler_rx].len = len;
		
		memcpy(rx_buffer.data_pack[req_handler_rx].dat, dat, len);				
	}
	else						/* 接收缓存区有空闲缓存 */
	{
		/* 将数据压入接收缓存区 */
		rx_buffer.data_pack[req_handler_rx].feat_id = feat_id;
		
		rx_buffer.data_pack[req_handler_rx].len = len;
		
		memcpy(rx_buffer.data_pack[req_handler_rx].dat, dat, len);
	}
	
	return push_ret;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
