/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Can_Network_Driver.c
** Author      : Cai Heng
** Date        : 2021-01-25
** Description : CAN����Э��ײ������ļ�
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Can_Network_Driver.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
Data_Buffer_Struct rx_buffer __attribute__ ((at(0x20040000)));		/* ���ջ����� */
Data_Buffer_Struct tx_buffer __attribute__ ((at(0x20046000)));		/* ���ͻ����� */
Err_Buffer_Struct err_buffer;		/* ���ϻ����� */

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

static void Err_Buffer_Clr(uint8_t _no)
{
	if(_no < MAX_ERR_INFO)
	{
		memset(&err_buffer.info[_no], 0, sizeof(err_buffer.info[_no]));
	}
}

/**********************************************************************************************************
**  �� �� �� : Err_Info_Pop
**  ����˵�� : 
**  ��    �� : 
**  �� �� ֵ : 
**********************************************************************************************************/
s8_t Err_Info_Pop(void)
{
	u8_t pop_handler = 0;
	u8_t err_info_dat[512 + 14] = {0};
	u16_t err_info_len = 0;
	u32_t err_info_id = 0;
	
	if(err_buffer.info_cnt == 0)
	{
		return -1;		/* ������: ���ϻ�����Ϊ�� */
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
**  �� �� �� : Err_Info_Push
**  ����˵�� : 
**  ��    �� : 
**  �� �� ֵ : 
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
**  �� �� �� : Data_Handler_Request
**  ����˵�� : �����������ڴ�����
**  ��    �� : p   - ����������ڴ�Ļ�����ָ�� rx_buffer / tx_buffer
**	           cmd - �������ȼ����� COMMON_REQUEST - ��ͨ���ȼ�(�Ƚ��ȳ�,�ɲ��)
**	                                MAJOR_REQUEST  - �������ȼ�(����ȳ�,�����)
**  �� �� ֵ : ��0 - ����Ļ����������ڴ���(0,DATA_PACK_NUM-1)
**	           -1 - ������: �������洢�ռ����
**********************************************************************************************************/
static s8_t Data_Handler_Request(Data_Buffer_Struct *p, u8_t cmd)
{
	u8_t request_handler = 0;		/* ����Ŀ��л�����ָ�� */

//	if(rx_buffer.pack_cnt >= DATA_PACK_NUM)
	if(p->pack_cnt >= DATA_PACK_NUM)
	{
		return -1;		/* ������: �������洢�ռ���� */
	}

	p->pack_cnt++;		/* �������ݰ������� */
	
	if(cmd == MAJOR_REQUEST)		/* ��������(��ߴ������ȼ�) */
	{
		/* βָ���Լ��������Լ���Ļ�������Ϊ�����ڴ���(0,DATA_PACK_NUM-1) */
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
	else							/* ��ͨ����(��ʹ������ȼ�) */
	{
		/* ͷָ����������������ǰ�Ļ�������Ϊ�����ڴ���(0,DATA_PACK_NUM-1) */
		request_handler = p->head_ptr;		/* ��ʹ�õĿ��л�����ָ�� */
		
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
**  �� �� �� : Data_Handler_Inquiry
**  ����˵�� : ������ռ���ڴ��ѯ
**  ��    �� : p - ����ѯռ���ڴ�Ļ�����ָ�� rx_buffer / tx_buffer
**  �� �� ֵ : ��0 - ��ѯ�Ļ�����ռ���ڴ���(0,DATA_PACK_NUM-1)
**	           -1 - ������: ������Ϊ��
**********************************************************************************************************/
static s8_t Data_Handler_Inquiry(Data_Buffer_Struct *p)
{
	u8_t inquiry_handler = 0;		/* ��ѯ������Ļ�����ָ�� */

	if(p->pack_cnt == 0)
	{
		return -1;		/* ������: ������Ϊ�� */
	}

	p->pack_cnt--;		/* �������ݰ����Լ� */
	
	/* βָ����������������ǰ�Ļ�������Ϊռ���ڴ���(0,DATA_PACK_NUM-1) */
	inquiry_handler = p->tail_ptr;		/* ����������ݻ�����ָ�� */
	
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
**  �� �� �� : Network_Data_Pop
**  ����˵�� : �������ѹ������
**  ��    �� : 
**  �� �� ֵ :  0 - �������ѹ������ɹ�
**	           -1 - ������: ������Ϊ��
**	           -2 - ������: ѹ�����ݳ��ȳ���
**********************************************************************************************************/
s8_t Network_Data_Pop(u8_t* dat, u32_t* feat_id, u16_t* len, Data_Buffer_Struct *p)
{
	s8_t inq_handler = 0;
	
	inq_handler = Data_Handler_Inquiry(p);
	
	if(inq_handler < 0)
	{
		return -1;		/* ������: ������Ϊ�� */
	}
	
	if(p->data_pack[inq_handler].len > DATA_PACK_LEN)
	{		
		Err_Info_Push(1, p->data_pack[inq_handler].feat_id, p->data_pack[inq_handler].len, p->data_pack[inq_handler].dat);
		
		/* �����ѯȡ�����ݵĻ����� */
		p->data_pack[inq_handler].feat_id = 0;
		
		p->data_pack[inq_handler].len = 0;
		
		memset(p->data_pack[inq_handler].dat, 0, DATA_PACK_LEN);
		
		return -2;		/* ������: ѹ�����ݳ��ȳ��� */
	}
	
	/* ������ѹ�������� */
	*feat_id = p->data_pack[inq_handler].feat_id;
	
	*len = p->data_pack[inq_handler].len;
	
	memcpy(dat, p->data_pack[inq_handler].dat, p->data_pack[inq_handler].len);
	
	/* �����ѯȡ�����ݵĻ����� */
	p->data_pack[inq_handler].feat_id = 0;
	
	p->data_pack[inq_handler].len = 0;
	
	memset(p->data_pack[inq_handler].dat, 0, DATA_PACK_LEN);
	
	return 0;
}

/**********************************************************************************************************
**  �� �� �� : Network_Data_Push_Tx_Buffer
**  ����˵�� : �������ѹ�뷢�ͻ���
**  ��    �� : 
**  �� �� ֵ :  0 - ����ѹ�뷢�ͻ������ɹ�
**	           -1 - �����󡿣���ѹ�뷢�ͻ��������ݳ��ȳ���
**	           -2 - �����󡿣����ͻ��������������ݳ��ȳ���
**********************************************************************************************************/
s8_t Network_Data_Push_Tx_Buffer(u8_t* dat, u32_t feat_id, u16_t len, u8_t cmd)
{
	s8_t req_handler_tx = 0;
	s8_t inq_handler_tx = 0;
	s8_t push_ret = 0;
	
	if(len > DATA_PACK_LEN)
	{
		Err_Info_Push(2, feat_id, len, dat);		
		
		return -1;	/* �����󡿣���ѹ�뷢�ͻ��������ݳ��ȳ��� */
	}
	
	/* �ڷ��ͻ�����������л��� */
	req_handler_tx = Data_Handler_Request(&tx_buffer, cmd);
	
	if(req_handler_tx == -1)	/* ���ͻ������޿��л��� */
	{
		/* �ڷ��ͻ�������ѯ�����ͻ��� */
		inq_handler_tx = Data_Handler_Inquiry(&tx_buffer);
		
		if(tx_buffer.data_pack[inq_handler_tx].len > DATA_PACK_LEN)
		{
			Err_Info_Push(3, tx_buffer.data_pack[inq_handler_tx].feat_id, tx_buffer.data_pack[inq_handler_tx].len, tx_buffer.data_pack[inq_handler_tx].dat);
			
			push_ret = -2;	/* �����󡿣����ͻ��������������ݳ��ȳ��� */
		}
		else
		{
			/* ���ʹӷ��ͻ�������ѯȡ������ */
			Can_Data_Pack_Send(tx_buffer.data_pack[inq_handler_tx].feat_id, EXT_FRM_ID, tx_buffer.data_pack[inq_handler_tx].dat, tx_buffer.data_pack[inq_handler_tx].len);
		}
		
		/* �����ѯȡ�����ݵķ��ͻ����� */
		tx_buffer.data_pack[inq_handler_tx].feat_id = 0;
		
		tx_buffer.data_pack[inq_handler_tx].len = 0;
		
		memset(tx_buffer.data_pack[inq_handler_tx].dat, 0, DATA_PACK_LEN);
		
		
		/* ������еķ��ͻ������洢���� */
		req_handler_tx = Data_Handler_Request(&tx_buffer, cmd);
		
		/* ������ѹ�뷢�ͻ����� */
		tx_buffer.data_pack[req_handler_tx].feat_id = feat_id;
		
		tx_buffer.data_pack[req_handler_tx].len = len;
		
		memcpy(tx_buffer.data_pack[req_handler_tx].dat, dat, len);		

	}
	else						/* ���ͻ������п��л��� */
	{
		/* ������ѹ�뷢�ͻ����� */
		tx_buffer.data_pack[req_handler_tx].feat_id = feat_id;
		
		tx_buffer.data_pack[req_handler_tx].len = len;
		
		memcpy(tx_buffer.data_pack[req_handler_tx].dat, dat, len);
	}
	
	return push_ret;
}


/**********************************************************************************************************
**  �� �� �� : Network_Data_Push_Rx_Buffer
**  ����˵�� : �������ѹ����ջ���
**  ��    �� : 
**  �� �� ֵ :  0 - ����ѹ����ջ������ɹ�
**	           -1 - �����󡿣���ѹ����ջ��������ݳ��ȳ���
**	           -2 - �����󡿣����ջ��������������ݳ��ȳ���
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
		
		return -1;	/* �����󡿣���ѹ����ջ��������ݳ��ȳ��� */
	}
	
	/* �ڽ��ջ�����������л��� */
	req_handler_rx = Data_Handler_Request(&rx_buffer, cmd);
	
	if(req_handler_rx == -1)	/* ���ջ������޿��л��� */
	{
		/* �ڽ��ջ�������ѯ���������� */
		inq_handler_rx = Data_Handler_Inquiry(&rx_buffer);
		
		if(rx_buffer.data_pack[inq_handler_rx].len > DATA_PACK_LEN)
		{	
			Err_Info_Push(5, rx_buffer.data_pack[inq_handler_rx].feat_id, rx_buffer.data_pack[inq_handler_rx].len, rx_buffer.data_pack[inq_handler_rx].dat);
			
			push_ret = -2;	/* �����󡿣����ջ��������������ݳ��ȳ��� */
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
		
		/* �����ѯȡ�����ݵĽ��ջ����� */
		rx_buffer.data_pack[inq_handler_rx].feat_id = 0;
		
		rx_buffer.data_pack[inq_handler_rx].len = 0;
		
		memset(rx_buffer.data_pack[inq_handler_rx].dat, 0, DATA_PACK_LEN);
		
		
		/* ������еĽ��ջ������洢���� */
		req_handler_rx = Data_Handler_Request(&rx_buffer, cmd);
		
		/* ������ѹ����ջ����� */
		rx_buffer.data_pack[req_handler_rx].feat_id = feat_id;
		
		rx_buffer.data_pack[req_handler_rx].len = len;
		
		memcpy(rx_buffer.data_pack[req_handler_rx].dat, dat, len);				
	}
	else						/* ���ջ������п��л��� */
	{
		/* ������ѹ����ջ����� */
		rx_buffer.data_pack[req_handler_rx].feat_id = feat_id;
		
		rx_buffer.data_pack[req_handler_rx].len = len;
		
		memcpy(rx_buffer.data_pack[req_handler_rx].dat, dat, len);
	}
	
	return push_ret;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
