/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Bsp_NCA9555.h
** Author      : YI LI HUA
** Date        : 2022-07-21
** Description : CAN驱动文件  硬件测试用
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "Bsp_CanLink.h"
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t RxDataCAN1[8] = {0};  	//CAN接收数据
uint8_t RxBuffCAN1[64] = {0};  	//CAN接收数据缓冲
uint8_t TxBuffCAN1[256] = {0x5B, 0x5B, 0xB8, 0x00, 0x08, 0x00, 0x00, 0x22};  	//CAN接收数据缓冲
uint8_t RxCount = 0;
uint32_t ID = 0x04000003;       //CAN ID

float VsenseKB[2] = {1, 0};
float VtzKB[2] = {1, 0};
uint8_t KBSwitch;  //sense KB系数, 探针KB系数切换 
uint8_t BypassEN;  //
s_DebugInqDatStruct1 inq1[8];
uint8_t SByPassSwitch;
extern uint8_t CurrOk[2];

uint16_t ErrCnt = 0;

/* SW1 高电平导通*/
PortPinType	SSwA_P_PortPin[8] = 
{
	{SSwA_P1_MCU_GPIO_Port, SSwA_P1_MCU_Pin}, {SSwA_P2_MCU_GPIO_Port, SSwA_P2_MCU_Pin},
	{SSwA_P3_MCU_GPIO_Port, SSwA_P3_MCU_Pin}, {SSwA_P4_MCU_GPIO_Port, SSwA_P4_MCU_Pin},
	{SSwA_P5_MCU_GPIO_Port, SSwA_P5_MCU_Pin}, {SSwA_P6_MCU_GPIO_Port, SSwA_P6_MCU_Pin},
	{SSwA_P7_MCU_GPIO_Port, SSwA_P7_MCU_Pin}, {SSwA_P8_MCU_GPIO_Port, SSwA_P8_MCU_Pin},
};

/* SW2  高电平导通*/
PortPinType	SSwA_N_PortPin[8] = 
{
	{SSwA_N1_MCU_GPIO_Port, SSwA_N1_MCU_Pin}, {SSwA_N2_MCU_GPIO_Port, SSwA_N2_MCU_Pin},
	{SSwA_N3_MCU_GPIO_Port, SSwA_N3_MCU_Pin}, {SSwA_N4_MCU_GPIO_Port, SSwA_N4_MCU_Pin},
	{SSwA_N5_MCU_GPIO_Port, SSwA_N5_MCU_Pin}, {SSwA_N6_MCU_GPIO_Port, SSwA_N6_MCU_Pin},
	{SSwA_N7_MCU_GPIO_Port, SSwA_N7_MCU_Pin}, {SSwA_N8_MCU_GPIO_Port, SSwA_N8_MCU_Pin},
};

/* 旁路MOS完全打开 高导通*/
PortPinType	SBypassA_P_PortPin[8] = 
{
	{SBypassA_P1_MCU_GPIO_Port, SBypassA_P1_MCU_Pin}, {SBypassA_P2_MCU_GPIO_Port, SBypassA_P2_MCU_Pin},
	{SBypassA_P3_MCU_GPIO_Port, SBypassA_P3_MCU_Pin}, {SBypassA_P4_MCU_GPIO_Port, SBypassA_P4_MCU_Pin},
	{SBypassA_P5_MCU_GPIO_Port, SBypassA_P5_MCU_Pin}, {SBypassA_P6_MCU_GPIO_Port, SBypassA_P6_MCU_Pin},
	{SBypassA_P7_MCU_GPIO_Port, SBypassA_P7_MCU_Pin}, {SBypassA_P8_MCU_GPIO_Port, SBypassA_P8_MCU_Pin},
};

PortPinType	CTR_ON_PortPin[8] = 
{
	{CTR_ON1_MCU_GPIO_Port, CTR_ON1_MCU_Pin}, {CTR_ON2_MCU_GPIO_Port, CTR_ON2_MCU_Pin},
	{CTR_ON3_MCU_GPIO_Port, CTR_ON3_MCU_Pin}, {CTR_ON4_MCU_GPIO_Port, CTR_ON4_MCU_Pin},
	{CTR_ON5_MCU_GPIO_Port, CTR_ON5_MCU_Pin}, {CTR_ON6_MCU_GPIO_Port, CTR_ON6_MCU_Pin},
	{CTR_ON7_MCU_GPIO_Port, CTR_ON7_MCU_Pin}, {CTR_ON8_MCU_GPIO_Port, CTR_ON8_MCU_Pin},
};


/* Table of CRC values for high–order byte */
const uint8_t crctablehi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40};

/* Table of CRC values for low–order byte */
const uint8_t crctablelo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40};

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/


/**********************************************************************************************************
**	函 数 名 : crc16table
**	功能说明 : CRC查表校验
**	形    参 : ptr: 指向需要校验数据的指针
			   len: 代校验数据长度
**	返 回 值 : 擦除成功返回1，擦除失败返回0
**********************************************************************************************************/
uint16_t crc16table(uint8_t *ptr, uint16_t len)
{
	uint8_t crchi = 0xff; // CRC高字节初始化
	uint8_t crclo = 0xff; // CRC低字节初始化
	uint16_t index;		  // CRC循环索引
	while (len--)
	{
		index = crclo ^ *ptr++; //计算CRC
		crclo = crchi ^ crctablehi[index];
		crchi = crctablelo[index];
	}
	return (crchi << 8 | crclo); //返回校验结果
}

/**********************************************************************************************************
**	函 数 名 : CAN1_Send_Msg(uint8_t* msg,uint8_t len)
**	功能说明 : CAN发送一组数据
**	形    参 : Data: 发送数据指针, ID: 帧ID, Len: 发送字节数
**	返 回 值 : 0: 发送成功, 1: 发动失败
**********************************************************************************************************/
uint8_t CAN1_Send_Msg(uint8_t* Data, uint32_t ID, uint8_t Len)
{	
	CAN_TxHeaderTypeDef	CAN_TxHeader;      //发送
	
    uint8_t i = 0;
	uint32_t TxMailbox;  //CAN邮箱
	uint8_t Message[8];	 //发送缓存
	
    CAN_TxHeader.StdId = 0x00;        	//标准标识符
    CAN_TxHeader.ExtId = ID;        	//扩展标识符(29位)
    CAN_TxHeader.IDE = CAN_ID_EXT;      //使用扩展帧
    CAN_TxHeader.RTR = CAN_RTR_DATA;    //数据帧
    CAN_TxHeader.DLC = Len;    
	
    for(i = 0; i < Len; i++)		//装入要发送的数据
    {
		Message[i] = Data[i];
	}
	
    if(HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeader, Message, &TxMailbox) != HAL_OK)  //开始发送
	{
		return 1;
	}
	
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3);  //等待发送完成
	
    return 0;
}

/**********************************************************************************************************
**	函 数 名 : CAN1_Receive_Msg(uint8_t *Data)
**	功能说明 : CAN接收一组数据
**	形    参 : Data: 接收数据指针, 
**	返 回 值 : DLC: 接收字节数
**********************************************************************************************************/
uint8_t CAN1_Receive_Msg(uint8_t *Data)
{
	CAN_RxHeaderTypeDef CAN_RxHeader;      
	
 	uint32_t i;
	uint8_t	RxData[8];

	if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) != 1)
	{
		return 0xF1;
	}

	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN_RxHeader, RxData) != HAL_OK)
	{
		return 0xF2;
	}
	
    for(i = 0; i < CAN_RxHeader.DLC; i++)
	{
		Data[i] = RxData[i];
	}
	
	return CAN_RxHeader.DLC;
}

/**********************************************************************************************************
**	函 数 名 : CAN1_Send_Pack(uint8_t* Data, uint16_t Len)
**	功能说明 : CAN发送一个数据包
**	形    参 : Data: 发送数据指针, 
**	返 回 值 : DLC: 发送数据包长度
**********************************************************************************************************/
void CAN1_Send_Pack(uint8_t* Data, uint16_t Len)
{
	uint8_t TxData[8];  //发送数据
	uint8_t Num;  		//帧数目
	uint8_t TailLen;	//末尾帧长度 
	
	uint8_t i;
	
	Num = (Len / 8) + 1;
	TailLen = Len % 8;
	
	for(i = 0; i < Num; i++)
	{
		ID += (0x00000001 << 10);  //帧数递增
		if(i != (Num - 1))
		{
			memcpy(TxData, &Data[8*i], 8); 
			CAN1_Send_Msg(TxData, ID, 8);
		}
		else if(TailLen != 0)
		{
			memcpy(TxData, &Data[8*i], TailLen);
			CAN1_Send_Msg(TxData, ID, TailLen);
		}
	}
	ID += (0x00000001 << 18);  //包数递增
	ID = ID & 0xFFFC03FF;  //帧序清零
}

/**********************************************************************************************************
**	函 数 名 : 旁路开关控制
**	功能说明 : CH要旁路的通道编号
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
uint8_t ByPassSwitch(uint8_t CH)
{
	uint8_t i;
	uint8_t SByPass = 0;
	CH = BypassEN;
	for(i = 0; i < 8; i++)
	{
		if((CH && 0x01) == 0x01)
		{
			SByPass = SByPass | (0x01 << i);
			HAL_GPIO_WritePin(SSwA_N_PortPin[i].Port, SSwA_N_PortPin[i].Pin, GPIO_PIN_RESET);   //先关闭SW2
			bsp_DelayMS(1);  //关断后延时1ms给小MCU发送信号，避免电池短路
			HAL_GPIO_WritePin(CTR_ON_PortPin[i].Port, CTR_ON_PortPin[i].Pin, GPIO_PIN_RESET);   	//CTRON
			bsp_DelayMS(1);  
			HAL_GPIO_WritePin(CTR_ON_PortPin[i].Port, CTR_ON_PortPin[i].Pin, GPIO_PIN_SET);   	//
		}
		CH = CH >> 1;
	}
	return SByPass;
}

/**********************************************************************************************************
**	函 数 名 : 旁路开关完全打开控制
**	功能说明 : 
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
uint8_t SByPassEN;
void PassSwitch(void)
{
	uint8_t i;
	//uint8_t SByPassEN;

	if(Pca9555_RegRead(I2C_NCA9555, 0x40, &CurrOk) == 0)
	{
		SByPassEN = (~CurrOk[0]) & SByPassSwitch;
		for(i = 0; i < 8; i++)
		{
			if((SByPassEN  && 0x01) == 0x01)
			{
				HAL_GPIO_WritePin(SBypassA_P_PortPin[i].Port, SBypassA_P_PortPin[i].Pin, GPIO_PIN_SET);
			}
			SByPassEN = SByPassEN >> 1;
		}
	}
}

void SW1SW2ON(void)
{
	uint8_t i;
	for(i = 0; i < 8; i++)
	{
		HAL_GPIO_WritePin(SSwA_P_PortPin[i].Port, SSwA_P_PortPin[i].Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(SSwA_N_PortPin[i].Port, SSwA_N_PortPin[i].Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(CTR_ON_PortPin[i].Port, CTR_ON_PortPin[i].Pin, GPIO_PIN_SET);
	}
}
/**********************************************************************************************************
**	函 数 名 : CAN_Send()
**	功能说明 : CAN接收命令发送函数
**	形    参 : 
**	返 回 值 : 
**********************************************************************************************************/
void CAN_Send(void)
{
	uint8_t i;
	uint8_t num = 0;
	uint16_t CRCValue;
	for(i = 0; i < 8; i++)
	{
//		inq1[i].VsenseDebug = (((float)ADC2_DATA_AVG[i] / 0x7FFFFF) * 5.0 - 0.15f) * 1.2f * VsenseKB[0] + VsenseKB[1] ;	
//		inq1[i].VtzDebug = ((float)ADC1_DATA_AVG[i] / 0x7FFFFF) * 5.0 * 1.2f * VtzKB[0] + VtzKB[1];
	}
	for(i = 0; i < 8; i++)
	{
		memcpy(&TxBuffCAN1[8] + num, &inq1[i], sizeof(inq1[i]));
        num = num + sizeof(inq1[i]);
	}
	TxBuffCAN1[4] = RxBuffCAN1[4];
	CRCValue = crc16table(&TxBuffCAN1[2], 182);
	TxBuffCAN1[184] = CRCValue & 0x00FF;
	TxBuffCAN1[185] = (CRCValue & 0xFF00) >> 8;
	TxBuffCAN1[186] = 0xA5;
	TxBuffCAN1[187] = 0xA5;
 	if(RxBuffCAN1[7] == 0x22 && RxBuffCAN1[10] == 0xA5 &&  RxBuffCAN1[11] == 0xA5)
	{
		CAN1_Send_Pack(TxBuffCAN1, 188);
		RxBuffCAN1[7] = 0;
		RxBuffCAN1[10] = 0;
		RxBuffCAN1[11] = 0;
		RxCount = 0;
	}
	if(RxBuffCAN1[7] == 0x21 && RxBuffCAN1[20] == 0xA5 && RxBuffCAN1[21] == 0xA5)
	{
		CAN1_Send_Pack(TxBuffCAN1, 8);
		if(RxBuffCAN1[9] == 0x01)
		{
			if(KBSwitch == 0x00)
			{
				memcpy(VsenseKB, &RxBuffCAN1[10], 8);
			}
			else
			{
				memcpy(VtzKB, &RxBuffCAN1[10], 8);
			}
		}
		BypassEN = RxBuffCAN1[6];
		RxBuffCAN1[7] = 0;
		RxBuffCAN1[20] = 0;
		RxBuffCAN1[21] = 0;
 		RxCount = 0;
		
		SByPassSwitch = ByPassSwitch(BypassEN);
	}
	if(RxBuffCAN1[7] == 0x23 && RxBuffCAN1[12] == 0xA5 && RxBuffCAN1[13] == 0xA5)
	{
		CAN1_Send_Pack(TxBuffCAN1, 8);
		RxBuffCAN1[7] = 0;
		RxBuffCAN1[12] = 0;
		RxBuffCAN1[13] = 0;
		KBSwitch = RxBuffCAN1[9];
 		RxCount = 0;
	}
}





/**********************************************************************************************************
**	函 数 名 : HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
**	功能说明 : CAN中断回调函数
**	形    参 : hcan: 指向CAN结构体指针
**	返 回 值 : 
**********************************************************************************************************/
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//	uint8_t Num;
//	if(hcan->Instance == CAN1)
//	{
//		Num = CAN1_Receive_Msg(RxDataCAN1);
//		memcpy(&RxBuffCAN1[RxCount], RxDataCAN1, Num);  //将接收到的数据拷贝至缓冲区
//		RxCount = RxCount + Num;
//	}
//	if(hcan->Instance == CAN2)
//	{
//		
//	}
//}

CAN_TxHeaderTypeDef CAN_TxMsg;
CAN_RxHeaderTypeDef CAN1_RxMsg;
CAN_RxHeaderTypeDef CAN2_RxMsg;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	uint8_t  Can1Data[8] = {0};
	uint32_t CanMsgId = 0;
	HAL_StatusTypeDef CanStatus = HAL_OK;
	
	if(hcan->Instance == CAN1)
	{
		CanStatus = HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN1_RxMsg, Can1Data);
	
		if(HAL_OK == CanStatus)
		{
			//判断ID类型
			if(CAN_ID_STD == CAN1_RxMsg.IDE)
			{
				CanMsgId = CAN1_RxMsg.StdId;
			}
			else
			{
				CanMsgId = CAN1_RxMsg.ExtId;
			}
			
			Can_Network_Rx_Process(CanMsgId, Can1Data);
		}
		
	}
	
	if(hcan->Instance == CAN2)
	{
		CanStatus = HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &CAN2_RxMsg, &DcdcRecvBuffer.ChFlag);
		
		if(HAL_OK == CanStatus)
		{
			//判断ID类型
			if(CAN_ID_STD == CAN2_RxMsg.IDE)
			{
				CanMsgId = CAN2_RxMsg.StdId;
			}
			else
			{
				CanMsgId = CAN2_RxMsg.ExtId;
			}
			
			//判断ID号是否正确
//			if(RECV_MSG_ID == CanMsgId)
//			{
				//释放CAN接收信号量
				osSemaphoreRelease(SemaDcdcCan);
//			}
		}		
	}
}

/**********************************************************************************************************
**  函 数 名 : CanSendMsg()
**  功能说明 : CAN发送数据函数
**  形    参 : *hcan - CAN类型，*pMsg - 帧数据缓冲区首地址， MsgLen - 帧数据长度，MsgId - 帧ID
**  返 回 值 : 无
**********************************************************************************************************/
void  CanSendMsg(CAN_HandleTypeDef *hcan, uint8_t *pMsg, uint8_t MsgLen, uint32_t MsgId)
{
	CAN_TxMsg.DLC = MsgLen;
	CAN_TxMsg.StdId = 0x0000;
	CAN_TxMsg.ExtId = MsgId;
	CAN_TxMsg.IDE = CAN_ID_EXT;
	CAN_TxMsg.RTR = CAN_RTR_DATA;
	
	/* while(HAL_CAN_GetTxMailboxesFreeLevel(&CAN1_HANDLE) == 0)
	** 等待发送完毕后再发送,否则会造成数据丢包,上述写法仅适用于单帧数据发送,
	** 当多帧数据发送时,需采用下面这种写法,否则会造成多帧数据发送乱序的情况
	*/
	ErrCnt = 0;
	while(HAL_CAN_GetTxMailboxesFreeLevel(hcan) < 3)
	{
		if(ErrCnt > 12)
		{
			ErrCnt = 0;
			break;
		}
	}
	HAL_CAN_AddTxMessage(hcan, &CAN_TxMsg, pMsg, (uint32_t*)CAN_TX_MAILBOX0);
}
