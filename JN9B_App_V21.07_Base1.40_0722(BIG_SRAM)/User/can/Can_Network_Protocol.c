/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Can_Network_Protocol.c
** Author      : Cai Heng
** Date        : 2021-01-25
** Description : CAN����Э����ļ�
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Can_Network_Protocol.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
static u8_t upcast_feat_id = 128;
Can_Rx_Buffer_Struct can_rx_buffer __attribute__ ((at(0x20020000)));		/* ������ջ������ṹ�� */

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : Can_Rx_Buffer_Clr
**  ����˵�� : CAN���ջ��������
**  ��    �� : buffer_id - ������Ľ��ջ��������
**  �� �� ֵ : ��
**********************************************************************************************************/
static void Can_Rx_Buffer_Clr(u8_t buffer_id)
{
	if(buffer_id < MAX_RX_PACK)
	{
		memset(&can_rx_buffer.rx_pack[buffer_id], 0, sizeof(can_rx_buffer.rx_pack[buffer_id]));
	}
}

/**********************************************************************************************************
**  �� �� �� : Can_Network_Tx_Process
**  ����˵�� : CAN�������ݰ����ʹ���
**  ��    �� : ��
**  �� �� ֵ :  0 - �������ݷ������
**	           -1 - ������: ������Ϊ��
**	           -2 - ������: ѹ�����ݳ��ȳ���
**********************************************************************************************************/
s8_t Can_Network_Tx_Process(void)
{
		u8_t tx_dat[DATA_PACK_LEN] = {0};	/* ��ѹ�����͵ķ��ͻ���������       */
//	u8_t tx_dat[DATA_PACK_LEN] __attribute__((aligned (4))) = {0};	/* ��ѹ�����͵ķ��ͻ���������       */
	
	u32_t tx_feat_id = 0;				/* ��ѹ�����͵ķ��ͻ�������������ID */
	u16_t tx_len = 0;					/* ��ѹ�����͵ķ��ͻ��������ݳ���   */
	s8_t tx_pop_ret = 0;				/* ѹ����� */
	
	/* �ӷ��ͻ�����ѹ�����������ݰ� */
	tx_pop_ret = Network_Data_Pop(tx_dat, &tx_feat_id, &tx_len, &tx_buffer);
	
	/* ���������ݰ�ѹ���ɹ� */
	if(tx_pop_ret == 0)
	{
		Can_Data_Pack_Send(tx_feat_id, EXT_FRM_ID, tx_dat, tx_len);	/* CAN���ݰ����� */
	}
	
	return tx_pop_ret;
}

/**********************************************************************************************************
**  �� �� �� : Err_Info_Tx_Process
**  ����˵�� : ������Ϣ���ʹ���
**  ��    �� : ��
**  �� �� ֵ : 
**********************************************************************************************************/
s8_t Err_Info_Tx_Process(void)
{
	return Err_Info_Pop();
}

/**********************************************************************************************************
**  �� �� �� : Can_Network_Analysis_Process
**  ����˵�� :  CAN�������ݰ���������
**  ��    �� : ��
**  �� �� ֵ :  0  - �ӽ��ջ�����ѹ���������������ɽ������ɹ�ѹ�뷢�ͻ�����
**	           -1  - �����󡿣����ջ�����Ϊ��
**	           -2  - �����󡿣����ջ�����ѹ�����ݳ��ȳ���
**	           -3  - �����󡿣���ѹ�뷢�ͻ��������ݳ��ȳ���
**	           -4  - �����󡿣����ͻ��������������ݳ��ȳ���
**	           -5  - �����󡿣�
**	           -6  - �����󡿣�
**********************************************************************************************************/
s8_t Can_Network_Analysis_Process(void)
{
	u8_t rx_dat[DATA_PACK_LEN] = {0};	/* ��ѹ������Ľ��ջ���������       */
	u32_t rx_feat_id = 0;				/* ��ѹ������Ľ��ջ�������������ID */
	u16_t rx_len = 0;					/* ��ѹ������Ľ��ջ��������ݳ���   */
	s8_t rx_pop_ret = -1;				/* ѹ����� */
	
	s8_t analysis_ret = 0;				/* ���������� */
	
	u8_t tx_dat[DATA_PACK_LEN] = {0};	/* ��ѹ�뷢�͵Ľ�����������       */
	u32_t tx_feat_id = 0;				/* ��ѹ�뷢�͵Ľ���������������ID */
	u16_t tx_len = 0;					/* ��ѹ�뷢�͵Ľ����������ݳ���   */
	s8_t tx_push_ret = -1;				/* ѹ���� */
	
	/* �ӽ��ջ�����ѹ�����������ݰ� */
	rx_pop_ret = Network_Data_Pop(rx_dat, &rx_feat_id, &rx_len, &rx_buffer);
	
	if(rx_pop_ret == 0)		/* ���������ݰ�ѹ���ɹ� */
	{
		/* ���������ݰ����� */
		analysis_ret = Can_Data_Pack_Analysis(rx_dat, rx_len, tx_dat, &tx_len);
		
		if(analysis_ret == 0)	/* ���������ݰ������ɹ� */
		{
			/* ���´��������ݰ�����ID */
			tx_feat_id = rx_feat_id | 0x00000003;	/* �����Ե㲥��ʽ����λ������/��λ�� */
		
			/* �����������ݰ�ѹ�뷢�ͻ��� */
			tx_push_ret = Network_Data_Push_Tx_Buffer(tx_dat, tx_feat_id, tx_len, COMMON_REQUEST);
			
			if(tx_push_ret == 0)	/* ���������ݰ�ѹ��ɹ� */
			{
				return 0;	/* �ӽ��ջ�����ѹ���������������ɽ������ɹ�ѹ�뷢�ͻ����� */
			}
			else					/* ���������ݰ�ѹ��ʧ�� */
			{
				return (tx_push_ret - 2);	/* -3��-4 */
			}
		}
		else					/* ���������ݰ�����ʧ�� */
		{
			Err_Info_Push(7, rx_feat_id, rx_len, rx_dat);
			
			return (analysis_ret - 4);		/* -5��-6��... */
		}
	}
	else					/* ���������ݰ�ѹ��ʧ�� */
	{
		return rx_pop_ret;					/* -1��-2 */
	}
}

/**********************************************************************************************************
**  �� �� �� : Can_Network_Rx_Timeout
**  ����˵�� : CAN�������ݰ����ճ�ʱ�ж�
**  ��    �� : ��
**  �� �� ֵ : bit_x - �������ݰ�������x���ճ�ʱ(x=0~15)
**********************************************************************************************************/
u32_t Can_Network_Rx_Timeout(u16_t rx_timeout, u16_t rx_timeout_period)
{
	u32_t ret = 0;
	
	/* ��ѯ�������ݻ��������б�ʶ�Ƿ�ռ�� */
	for(u8_t pack_seq = 0; pack_seq < MAX_RX_PACK; pack_seq++)
	{
		if(can_rx_buffer.rx_pack[pack_seq].pack_feat_id != 0)	/* �������ݻ��������б�ʶ��ռ�� */
		{	
			if(can_rx_buffer.rx_pack[pack_seq].pack_timeout >= (rx_timeout / rx_timeout_period - 1))
			{
				u8_t nop_dat[8] = {0};
				
				Err_Info_Push(8, can_rx_buffer.rx_pack[pack_seq].pack_feat_id, 8, nop_dat);
				
				Can_Rx_Buffer_Clr(pack_seq);		/* ��������ݻ������� */
				
				ret |= 1 << pack_seq;	/* ������: ���ݰ����ճ�ʱ */
			}
			else
			{
				can_rx_buffer.rx_pack[pack_seq].pack_timeout++;
			}
		}
		else													/* �������ݻ��������б�ʶδռ�� */
		{
			can_rx_buffer.rx_pack[pack_seq].pack_timeout = 0;
		}
	}
	
	return ret;
}

/**********************************************************************************************************
**  �� �� �� : Can_Network_Rx_Process
**  ����˵�� : CAN�������ݰ����մ���(��CAN�����жϵ���)
**  ��    �� : rx_id   - ��������֡ID
**	           rx_data - ��������֡������ָ��
**  �� �� ֵ : ��0 - �������ݽ������(�������ݻ��������)
**	           -1  - ���ݰ�������
**	           -2 - ������: ���ջ��������(����ƥ��Ҳ�޿��е����ݽ��ջ�����)
**	           -3 - ������: ����֡֡�����(����������ݽ��ջ��������֡�������֡֡��Ϊ0)
**	           -4 - ������: ����֡֡����(����������ݽ��ջ�����Ŀ��֡��)
**	           -5 - ������: ����֡֡���ظ�(������ݽ��ջ��������յ��ظ�֡�������)
**	           -6 - ������: ����֡����쳣(������ݻ�������г����쳣)
**	           -7 - �����󡿣���ѹ����ջ��������ݳ��ȳ���
**	           -8 - �����󡿣����ջ��������������ݳ��ȳ���
**
**********************************************************************************************************/
s8_t Can_Network_Rx_Process(u32_t rx_id, u8_t* rx_data)		/* CAN�����жϵ��� */
{
	u8_t frame_seq = (rx_id >> CAN_ID_BIT_OFFSET_FRM_SEQU) & 0xFF;			/* ֡�� */	
	u32_t feat_id = rx_id & (~(0x000000FF << CAN_ID_BIT_OFFSET_FRM_SEQU));	/* ����ID */
	u8_t pack_match_flg = 0;	/* ���ݻ�������ƥ���ʶ */
	u8_t pack_match_seq = 0;	/* ���ݻ�������ƥ����� */
	u8_t quo = 0;	/* �� */
	u8_t rem = 0;	/* �� */
	
	/* 1. �����ж� */
	
	/* ��ѯ�������ݻ��������б�ʶ�Ƿ�ƥ�� */
	for(u8_t pack_seq = 0; pack_seq < MAX_RX_PACK; pack_seq++)
	{
		if(can_rx_buffer.rx_pack[pack_seq].pack_feat_id == feat_id)		/* ���б�ʶ�����ݰ�����IDƥ�� */
		{
			pack_match_flg = 1;
			pack_match_seq = pack_seq;
			break;
		}
	}
	
	/* ��ѯ�������ݻ������Ƿ��п��л��� */
	if(pack_match_flg == 0)
	{
		for(u8_t pack_seq = 0; pack_seq < MAX_RX_PACK; pack_seq++)
		{
			if(can_rx_buffer.rx_pack[pack_seq].pack_feat_id == 0)		/* ���ݰ����������� */
			{
				can_rx_buffer.rx_pack[pack_seq].pack_feat_id = feat_id;
				pack_match_flg = 1;
				pack_match_seq = pack_seq;
				break;
			}
		}
	}
	
	/* �������ݻ��������б�ʶ�Ȳ�ƥ��Ҳ�޿��� */
	if(pack_match_flg == 0)
	{
		Err_Info_Push(9, rx_id, 8, rx_data);
		
		return -2;		/* ������: ���ջ��������(����ƥ��Ҳ�޿��е����ݽ��ջ�����) */	/* ���������� */
	}
	
	/* ����֡֡���ж�: �Ƿ���[1,64]��Χ */
	if((frame_seq < 1) || (frame_seq > MAX_RX_FRAME))
	{
		Err_Info_Push(10, rx_id, 8, rx_data);
		
		Can_Rx_Buffer_Clr(pack_match_seq);		/* ��������ݻ������� */
		
		return -3;		/* ������: ����֡֡�����(����������ݽ��ջ��������֡�������֡֡��Ϊ0) */
	}
	
	/* ����֡֡���ж�: �Ƿ���Ŀ��֡����Χ */
	if(can_rx_buffer.rx_pack[pack_match_seq].target_frame_num != 0)
	{
		if(frame_seq > can_rx_buffer.rx_pack[pack_match_seq].target_frame_num)
		{
			Err_Info_Push(11, rx_id, 8, rx_data);
			
			Can_Rx_Buffer_Clr(pack_match_seq);	/* ��������ݻ������� */
			
			return -4;	/* ������: ����֡֡����(����������ݽ��ջ�����Ŀ��֡��) */
		}
	}
	
	/* ����֡��Ӧ�������Ƿ�ռ�� */
	quo = (frame_seq - 1) / 8;
	rem = (frame_seq - 1) % 8;
	if(can_rx_buffer.rx_pack[pack_match_seq].frame_idle_flag[quo] & (1 << rem))		/* ����֡������������ */
	{
		Err_Info_Push(12, rx_id, 8, rx_data);
		
		Can_Rx_Buffer_Clr(pack_match_seq);		/* ��������ݻ������� */
		
		return -5;		/* ������: ����֡֡���ظ�(������ݽ��ջ��������յ��ظ�֡�������) */
	}
	
	/* 2. ���ݸ��� */
	
	/* ��֡����֡ */
	if(frame_seq == 1)
	{
		u16_t rx_data_len = (*(rx_data + 3) << 8) | *(rx_data + 2) + 4;		/* ���ݰ����� */
		
		quo = rx_data_len / 8;
		
		rem = rx_data_len % 8;
		
		/* ����Ŀ��֡�� */
		can_rx_buffer.rx_pack[pack_match_seq].target_frame_num = (rem == 0) ? quo : (quo + 1);
		
		/* ����β֡���ݳ��� */
		can_rx_buffer.rx_pack[pack_match_seq].tail_frame_len = (rem == 0) ? 8 : rem;
	}
	
	/* ������󻺴�֡�� */
	if(can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq < frame_seq)
	{
		can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq = frame_seq;
	}
	
	/* ���»���֡�� */
	can_rx_buffer.rx_pack[pack_match_seq].recive_frame_num += 1;	
	
	/* ��������֡���������б�־ */
	quo = (frame_seq - 1) / 8;
	rem = (frame_seq - 1) % 8;	
	can_rx_buffer.rx_pack[pack_match_seq].frame_idle_flag[quo] |= (1 << rem);
	
	/* ��������֡������ */
	for(u8_t dat_seq = 0; dat_seq < 8; dat_seq++)
	{
		can_rx_buffer.rx_pack[pack_match_seq].rx_frame[frame_seq - 1].dat[dat_seq] = *(rx_data + dat_seq);
	}
	
	
	/* 3. ����ж� */
	
	/* β֡ */	/* ��󻺴�֡��=����֡��=Ŀ��֡�� */
	if( (can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq == can_rx_buffer.rx_pack[pack_match_seq].recive_frame_num) && \
		(can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq == can_rx_buffer.rx_pack[pack_match_seq].target_frame_num) )
	{
		/* ������ݽ��ջ��������ݰ����� */
		u16_t pack_dat_len = (can_rx_buffer.rx_pack[pack_match_seq].target_frame_num - 1) * 8 + can_rx_buffer.rx_pack[pack_match_seq].tail_frame_len;
		
		/* ������ݽ��ջ��������ݰ�ѹ��rx_buffer */
		s8_t pack_push_ret = Network_Data_Push_Rx_Buffer(can_rx_buffer.rx_pack[pack_match_seq].rx_frame[0].dat, feat_id, pack_dat_len, COMMON_REQUEST);
	
		/* �����ѹ��rx_buffer��������ݽ��ջ����� */
		Can_Rx_Buffer_Clr(pack_match_seq);		/* ��������ݻ������� */
		
		if(pack_push_ret == 0)
		{
			return (s8_t)pack_match_seq;		/* ��ѹ���������ݽ��ջ�������� */
		}
		else
		{
			return pack_push_ret - 6;
		}	
	}
	/* ���յ���֡�Ĺ���֡ */	/* ����֡������󻺴�֡���Ŀ��֡��(�ȺŲ�ͬʱ����) */
	else if( (can_rx_buffer.rx_pack[pack_match_seq].recive_frame_num <= can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq) && \
			 (can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq <= can_rx_buffer.rx_pack[pack_match_seq].target_frame_num) )
	{
		/* Do Nothing */
	}
	/* δ�յ���֡�Ĺ���֡ */	/* ����֡�������֡����Ŀ��֡��=0 */
	else if( (can_rx_buffer.rx_pack[pack_match_seq].recive_frame_num <= can_rx_buffer.rx_pack[pack_match_seq].max_frame_seq) && \
			 (can_rx_buffer.rx_pack[pack_match_seq].target_frame_num == 0) )
	{
		/* Do Nothing */
	}
	/* �쳣���֡ */
	else
	{
		Err_Info_Push(13, rx_id, 8, rx_data);
		
		Can_Rx_Buffer_Clr(pack_match_seq);		/* ��������ݻ������� */
		
		return -6;		/* ������: ����֡����쳣(������ݻ�������г����쳣) */
	}
	
	return -1;
}

/**********************************************************************************************************
**  �� �� �� : Can_Network_Upcast
**  ����˵�� : CAN����������������(�ȴ�����)
**  ��    �� : ��
**  �� �� ֵ : 
**********************************************************************************************************/
s8_t Can_Network_Upcast(u8_t* tx_dat, u16_t tx_len)                        /* �ȴ����� */
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
**  �� �� �� : Can_Network_Func
**  ����˵�� : 
**  ��    �� : 
**  �� �� ֵ : 
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
