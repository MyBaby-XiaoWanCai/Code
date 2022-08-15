/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Can_Network_Transplant.c
** Author      : Cai Heng
** Date        : 2021-02-12
** Description : CAN组网协议移植文件
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "Can_Network_Transplant.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
CAN_TxHeaderTypeDef     TxMeg;
CAN_RxHeaderTypeDef     RxMeg;

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void CAN_SendMultiFrames(CAN_HandleTypeDef* canHandle, uint32_t id, uint32_t type, uint8_t* pData, uint16_t len);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : Can_Data_Pack_Send
**  功能说明 : 通过CAN发送数据包
**  形    参 : feature_id - 数据包特征ID
**	           id_type    - CAN的ID类型(标准帧/扩展帧)
**	           tx_dat     - 待发送数据指针
**	           tx_len     - 待发送数据长度
**  返 回 值 : 无
**********************************************************************************************************/
void Can_Data_Pack_Send(u32_t feature_id, u32_t id_type, u8_t *tx_dat, u16_t tx_len)
{
	if(id_type == STD_FRM_ID)
	{
		CAN_SendMultiFrames(&hcan1, feature_id, CAN_ID_STD, tx_dat, tx_len);
	}
	else if(id_type == EXT_FRM_ID)
	{
		CAN_SendMultiFrames(&hcan1, feature_id, CAN_ID_EXT, tx_dat, tx_len);
	}
}

/**********************************************************************************************************
**  函 数 名 : Can_Data_Pack_Analysis
**  功能说明 : CAN数据包解析处理
**  形    参 : rx_dat - 接收待解析数据指针
**	           rx_len - 接收待解析数据长度
**	           tx_dat - 解析待发送数据指针
**	           tx_len - 解析待发送数据长度指针
**  返 回 值 : 0    - 接收数据解析完成
**	           其他 - 接收数据解析异常
**********************************************************************************************************/
s8_t Can_Data_Pack_Analysis(u8_t *rx_dat, u16_t rx_len, u8_t* tx_dat, u16_t* tx_len)
{
	s8_t analysis_ret = 0;
	
	analysis_ret = ProtocolDataProcess(rx_dat, rx_len, tx_dat, tx_len);
	
	return (0 - analysis_ret);
}

/**********************************************************************************************************
**  函 数 名 : Err_Info_Pack
**  功能说明 : 
**  形    参 : 
**  返 回 值 : 
**********************************************************************************************************/
void Err_Info_Pack(u8_t _no, u8_t* err_dat, u16_t* err_len, u32_t* _id)
{
	u16_t head    = 0x5B5B;
	u16_t len     = 10 + err_buffer.info[_no].len;
	u8_t  feat    = _no + 240;
	u8_t  sta     = err_buffer.info[_no].sta;
	u16_t crc     = 0;
	u16_t tail    = 0xA5A5;
	
	memcpy(err_dat + 0 , &head , 2);
	memcpy(err_dat + 2 , &len  , 2);
	memcpy(err_dat + 4 , &feat , 1);
	memcpy(err_dat + 5 , &sta  , 1);
	memcpy(err_dat + 6 , &err_buffer.info[_no].id    , 4);
	memcpy(err_dat + 10, &err_buffer.info[_no].dat[0], err_buffer.info[_no].len);
	crc = CRC16N(err_dat + 2, err_buffer.info[_no].len + 8);
	memcpy(err_dat + 10 + err_buffer.info[_no].len, &crc  , 2);
	memcpy(err_dat + 12 + err_buffer.info[_no].len, &tail , 2);
	
	*err_len = 14 + err_buffer.info[_no].len;
	
	*_id = (DEV_TYPE << DEV_TYPE_OFFSET) | (feat << FEAT_CODE_OFFSET) | (FRAM_SEQ << FRAM_SEQ_OFFSET) | (DEV_ADDR << DEV_ADDR_OFFSET) | (COM_TYPE_VOD << COM_TYPE_OFFSET) | (COM_DIR_UP << COM_DIR_OFFSET);
}

/**********************************************************************************************************
**	函 数 名 : CAN_SendSingleFrame()
**	功能说明 : 单帧数据发送函数
**	形    参 : canHandle - CAN句柄
**	           id        - 帧ID
**	           type      - 帧类型
**	           pData     - 待发送数据
**	           len       - 待发送长度
**	返 回 值 : 无
**********************************************************************************************************/
void CAN_SendSingleFrame(CAN_HandleTypeDef* canHandle, uint32_t id, uint32_t type, uint8_t* pData, uint16_t len)
{
	if(type == CAN_ID_STD)
	{
		TxMeg.StdId = id;
	}
	else if(type == CAN_ID_EXT)
	{
		TxMeg.ExtId = id;
	}
	TxMeg.IDE = type;
	TxMeg.RTR = CAN_RTR_DATA;
	TxMeg.DLC = len;

	/* while(HAL_CAN_GetTxMailboxesFreeLevel(&CAN1_HANDLE) == 0)
	** 等待发送完毕后再发送,否则会造成数据丢包,上述写法仅适用于单帧数据发送,
	** 当多帧数据发送时,需采用下面这种写法,否则会造成多帧数据发送乱序的情况
	*/
	while(HAL_CAN_GetTxMailboxesFreeLevel(canHandle) < 3);
	
	if(canHandle == &hcan1)
	{
		HAL_CAN_AddTxMessage(canHandle, &TxMeg, pData, (uint32_t*)CAN_TX_MAILBOX0);
	}
}

/**********************************************************************************************************
**	函 数 名 : CAN_SendMultiFrames()
**	功能说明 : 多帧数据发送函数
**	形    参 : canHandle - CAN句柄
**	           ?id        - 帧ID
**	           type      - 帧类型
**	           pData     - 待发送数据
**	           len       - 待发送长度
**	返 回 值 : 无
**********************************************************************************************************/
void CAN_SendMultiFrames(CAN_HandleTypeDef* canHandle, uint32_t id, uint32_t type, uint8_t* pData, uint16_t len)
{
	uint32_t idTemp = id & ( ~(0x000000FF << 10) );
	uint8_t* p = pData;
	uint16_t lenTemp = len;
	
	while(lenTemp)
	{
		idTemp += (0x00000001 << 10);
		
		if(lenTemp > 8)
		{
			CAN_SendSingleFrame(canHandle, idTemp, type, p, 8);
			lenTemp -= 8;
			p       += 8;			
		}
		else
		{
			CAN_SendSingleFrame(canHandle, idTemp, type, p, lenTemp);
			lenTemp = 0;
		}
	}
}



/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
