/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_acdc.c
** Author      : Huang Cheng
** Date        : 2021-11-15
** Description : bsp_acdc source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_acdc.h"
#include "bsp_dwt.h"
#include "bsp_debug.h"
#include "global.h"
#include "bsp.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t m_u8ErrType = 0;                //CAN��������
uint32_t m_u32StatusVal = 0;            //CAN�жϱ�־λ
ACDC_UpdateStrcut CanRet = {0, {0}, 0}; //ACDC����can�ظ��ṹ��

extern uint8_t ACDC_UpdateFlg;                      //ACDC������־λ
extern osSemaphoreId_t acdc_alarm_Semaphore;        //ACDC�澯�ź���ID

#if 0 //��δ�õ�
const static char *m_s8IDTypeStr[] = 
{
	"standard",
	"extended",
};

const static char *m_s8FrameTypeStr[] = 
{
	"normal",
	"CAN-FD",
};

const static char *m_s8ErrorTypeStr[] = 
{
	"no error.",
	"Bit Error.",
	"Form Error.",
	"Stuff Error.",
	"ACK Error.",
	"CRC Error.",
	"Other Error.",
	"Value of error type is invalid.",
};

const static char *m_s8TxBufTypeStr[] = 
{
	"PTB",
	"STB",
};
	
/*
 * DLC to data size.
 * Row: 0 is normal frame, 1 is CAN-FD frame.
 * Column: data length code(0 ~ 15).
 */
const static uint8_t m_au8DLC2Size[2U][16U] =
{
    {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 8U, 8U, 8U, 8U, 8U, 8U, 8U},
    {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U},
};

/* Data buffers. */
static uint8_t m_au8TxPayload[APP_DATA_SIZE];
static uint8_t m_au8RxPayload1[APP_DATA_SIZE];
static uint8_t m_au8RxPayload2[APP_DATA_SIZE];
static uint8_t m_au8RxPayload3[APP_DATA_SIZE];
#endif

static uint8_t CanRxBuffer[MSG_MAX_NUM][APP_DATA_SIZE];       //CAN���ջ���

ACDC_Inf ACDC= {1,{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}; //����ACDC�ṹ�壬Ĭ�Ϲ���ACDC��������ʼ��Ϊ0

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
static void Can1Config(void);
static void Can1IrqConfig(void);
static void CAN1_IrqCallback(void);
static void ACDC_DataHandle(stc_can_rx_t *pCanRx, uint8_t u8RxCnt);
static void CANx_ResvData(M4_CAN_TypeDef *CANx);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : bsp_InitACDC
**	����˵�� : ACDCģ���ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitACDC(void)
{
	Can1Config();
}

/**********************************************************************************************************
**	�� �� �� : CANx_SendData
**	����˵�� : CAN���߷������ݺ���
**	��    �� : *CANx - CAN1/CAN2��ID - ID�ţ�*pData - �������ݣ�Len - ���ݳ���
**	�� �� ֵ : enRet - ����״̬@ref en_result_t
**********************************************************************************************************/
en_result_t CANx_SendData(M4_CAN_TypeDef *CANx,  uint32_t ID, uint8_t *pData, uint16_t Len)
{
	en_result_t enTxResult;
	stc_can_tx_t stcTx = {0};
	
	/* �����Ҫ���͵����� */
	stcTx.pu8Data = pData;
	stcTx.u32Ctrl = 0U;
	stcTx.DLC = Len;
	stcTx.u32ID = ID;
	stcTx.IDE = 1U;
	
	enTxResult = CAN_TransData(CANx, &stcTx, CAN_BUF_STB, CAN_STB_TRANS_ALL, 0U);
	if(enTxResult != Ok)
    {
		for(uint8_t j = 0; j < CHANNEL_CFG; j++)
		{
			IP_Alarm_Flag[j] |= IntelProtect_C1E; //can1����  
		}
    }
	
	return enTxResult;
}

/**********************************************************************************************************
**	�� �� �� : ACDC_DataHandle()
**	����˵�� : ACDC���ݴ�����
**	��    �� : *pCanRx - �������ݣ�u8RxCnt - ��Ϣ֡��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void ACDC_DataHandle(stc_can_rx_t *pCanRx, uint8_t u8RxCnt)
{
	uint8_t AcdcIndex = 0;
	
	//����ACDC���ݽ��մ�������
	if((pCanRx[u8RxCnt].u32ID&0x1FFF0000) == 0x102B0000)
	{
		if(pCanRx[u8RxCnt].pu8Data[0] != 0XF0)
		{
			return;
		}
		
		if(0x00 == ((pCanRx[u8RxCnt].u32ID>>8)&0xFF))
		{
			AcdcIndex = 0;
		}
		else if(0x01 == ((pCanRx[u8RxCnt].u32ID>>8)&0xFF))
		{
			AcdcIndex = 1;
		}
		else
		{
			//do nothing;
		}
		
		switch(pCanRx[u8RxCnt].pu8Data[1])
		{
			case 0x37: //����ת�ٸ���
			{
				ACDC.FanSpd[AcdcIndex] = pCanRx[u8RxCnt].pu8Data[7];    
				break;
			}
			case 0x53: //ACDCֱ�����ѹ����
			{
				ACDC.DCV[AcdcIndex] = ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 10.0;
				break;
			}
			case 0x54: //ACDCֱ�����������
				ACDC.DCI[AcdcIndex] = ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 10.0;
				break;
			case 0x59://ģ��������������
			{
				/*�������״̬�ޱ仯���򲻸���ACDC״̬*/
				if(getSn_SR(SOCK_0_NUMB) == SOCK_ESTABLISHED)
				{
					if(ACDC.State[AcdcIndex] != ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]))
					{
						ACDC.BitFlg[AcdcIndex] = ACDC.State[AcdcIndex] ^ ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]); //��¼״̬�ı��bit
						ACDC.State[AcdcIndex] = ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]);  //����state�ֶ�
						osSemaphoreRelease(acdc_alarm_Semaphore);    //�ͷ��ź�����ͬ����������
					}
				}
				break;
			}
			case 0x62://ACDC�������ѹ����
			{
				ACDC.ACV[AcdcIndex] = ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 10.0;
				break;
			}
			case 0x63://ACDC�������������
			{
				ACDC.ACI[AcdcIndex] = ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 10.0;
				break;
			}
			default:
			{
				break;
			}
		}
			
	}
	else if((pCanRx[u8RxCnt].u32ID&0X1F000000) == 0X1c000000)
	{
		uint32_t TempId = 0;
		if(0x01 != ACDC_UpdateFlg)
		{
			return;
		}
		
		//�������ݵ��ṹ�壬�ṹ�巢��Ϣ����
		TempId = (pCanRx[u8RxCnt].u32ID>>24&0xFF)+((pCanRx[u8RxCnt].u32ID>>16&0xFF)<<8)+  \
				 ((pCanRx[u8RxCnt].u32ID>>8&0xFF)<<16) +((pCanRx[u8RxCnt].u32ID&0xFF)<<24); //ID��С��ת�� 
		CanRet.ID = TempId;
		memcpy(CanRet.Date, pCanRx[u8RxCnt].pu8Data, 8);
		CanRet.processedFlg = 1;    //1��ʾ�յ������ظ����ݴ�����
	}
	else if((pCanRx[u8RxCnt].u32ID&0X1FFF0000) == 0X06080000)
	{
		if(0x00 == ((pCanRx[u8RxCnt].u32ID>>3)&0xFF))
		{
			AcdcIndex = 0;
		}
		else if(0x01 == ((pCanRx[u8RxCnt].u32ID>>3)&0xFF))
		{
			AcdcIndex = 1;
		}
		else
		{
			//do nothing;
		}
		
		if((0x00==ACDC_UpdateFlg) && (0x30==pCanRx[u8RxCnt].pu8Data[1]))
		{
			switch(pCanRx[u8RxCnt].pu8Data[3])
			{
				case 0x01://ACDCֱ�����ѹ����
				{
					ACDC.DCV[AcdcIndex] = (int)((uint16_t)(pCanRx[u8RxCnt].pu8Data[4] << 24) + (uint16_t)(pCanRx[u8RxCnt].pu8Data[5] << 16) +  \
											   (uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 1024.0;
					break;
				}
				case 0x02://ACDCֱ�����������
				{
					ACDC.DCI[AcdcIndex] = (int)((uint16_t)(pCanRx[u8RxCnt].pu8Data[4] << 24) + (uint16_t)(pCanRx[u8RxCnt].pu8Data[5] << 16) +  \
											   (uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 1024.0;
					break;
				}
				case 0x43://ACDC����״̬���������Э��
				{
					/*�������״̬�ޱ仯���򲻸���ACDC״̬*/
					if(ACDC.State[AcdcIndex] != ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]))
					{
						ACDC.BitFlg[AcdcIndex] = ACDC.State[AcdcIndex] ^ ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]); //��¼״̬�ı��bit
						ACDC.State[AcdcIndex] = ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]);   //����״̬
						osSemaphoreRelease(acdc_alarm_Semaphore);     //�ͷ��ź�����ͬ����������
					}
					break;
				}
				case 0x05://ACDC�������ѹ����
				{
					ACDC.ACV[AcdcIndex] = (int)((uint16_t)(pCanRx[u8RxCnt].pu8Data[4] << 24) + (uint16_t)(pCanRx[u8RxCnt].pu8Data[5] << 16) +  \
											   (uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 1024.0;
					break;
				}
				case 0x06://ACDC�������������
				{
					ACDC.ACI[AcdcIndex] = (int)((uint16_t)(pCanRx[u8RxCnt].pu8Data[4] << 24) + (uint16_t)(pCanRx[u8RxCnt].pu8Data[5] << 16) +  \
											   (uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 1024.0;
					break;
				}
				default:
				{
					break;
				}
			}
		}
		else if((0x00==ACDC_UpdateFlg) && (0x10==pCanRx[u8RxCnt].pu8Data[1]) && (0x1F==pCanRx[u8RxCnt].pu8Data[3]))
		{
			ACDC.FanSpd[AcdcIndex] = pCanRx[u8RxCnt].pu8Data[7];   //���·���ת��
		}
		else
		{
			//do nothing;
		}
	}
	else
	{
		//do nothing;
	}
}

/**********************************************************************************************************
**	�� �� �� : CANx_ResvData
**	����˵�� : CAN���߽������ݺ���
**	��    �� : *CANx - CAN1/CAN2
**	�� �� ֵ : enRet - ����״̬@ref en_result_t
**********************************************************************************************************/
static void CANx_ResvData(M4_CAN_TypeDef *CANx)
{
	uint8_t u8RxCnt;
//    uint8_t u8DataIdx;
//    uint8_t u8DataSize;
    uint8_t u8RxFrameCnt;
    stc_can_rx_t stcRx[3U];

    stcRx[0U].pu8Data = (uint8_t*)CanRxBuffer[0U];
    stcRx[1U].pu8Data = (uint8_t*)CanRxBuffer[1U];
    stcRx[2U].pu8Data = (uint8_t*)CanRxBuffer[2U];

	//���ڵ�ǰ�豸ֻ��һ�֣�MSG_MAX_NUM����Ϊ1���ɸ���ʵ�������������������8
    (void)CAN_ReceiveData(APP_CAN_UNIT, &stcRx[0U], &u8RxFrameCnt, MSG_MAX_NUM);
    for (u8RxCnt = 0U; u8RxCnt < u8RxFrameCnt; u8RxCnt++)
    {
		#ifdef TEST
        printf("CAN received %s frame with %s ID %.8x:\n", \
            m_s8FrameTypeStr[stcRx[u8RxCnt].FDF], \
            m_s8IDTypeStr[stcRx[u8RxCnt].IDE],    \
            (unsigned int)stcRx[u8RxCnt].u32ID);
        u8DataSize = m_au8DLC2Size[stcRx[u8RxCnt].FDF][stcRx[u8RxCnt].DLC];
        for (u8DataIdx=0; u8DataIdx<u8DataSize; u8DataIdx++)
        {
            printf(" %.2x.", stcRx[u8RxCnt].pu8Data[u8DataIdx]);
            stcRx[u8RxCnt].pu8Data[u8DataIdx] = 0U;
        }
        printf("\n");
		#endif
		
		ACDC_DataHandle(&stcRx[u8RxCnt], u8RxCnt);
    }
}

/**********************************************************************************************************
**	�� �� �� : Can1Config
**	����˵�� : CAN1��ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� ��CANʱ�ӣ�240MHz / 6 = 40Mhz��CAN�����ʣ�40MHz / Prescaler / (SEG1+SEG2) = 125Kbps
**********************************************************************************************************/
static void Can1Config(void)
{
    stc_can_init_t stcInit;
    stc_can_af_cfg_t astcAFCfg[] = 
	{ 
        {APP_CAN_AF1_ID, APP_CAN_AF1_ID_MSK, APP_CAN_AF1_MSK_TYPE}, \
        {APP_CAN_AF2_ID, APP_CAN_AF2_ID_MSK, APP_CAN_AF2_MSK_TYPE}, \
        {APP_CAN_AF3_ID, APP_CAN_AF3_ID_MSK, APP_CAN_AF3_MSK_TYPE}, \
    };

    /* Set the function of CAN pins. */
    GPIO_SetFunc(APP_CAN_TX_PORT, APP_CAN_TX_PIN, \
                 APP_CAN_TX_PIN_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(APP_CAN_RX_PORT, APP_CAN_RX_PIN, \
                 APP_CAN_RX_PIN_FUNC, PIN_SUBFUNC_DISABLE);

    /* Configures the clock of CAN. */
    CLK_CAN_ClkConfig(APP_CAN_CLK_CH, APP_CAN_CLK_DIV);

    /* Initializes CAN. */
    (void)CAN_StructInit(&stcInit);
//	stcInit.u8WorkMode = CAN_MODE_ILB;   //�ڲ��ػ����ڲ���
	stcInit.u8WorkMode = CAN_MODE_NORMAL;
	stcInit.u8RBSWarnLimit = MSG_MAX_NUM;         //�趨RingBuffer�����ж���󻺴�Ϊ1��Range[1,8]
    stcInit.pstcAFCfg = astcAFCfg;
    stcInit.u8RBOvfOp = CAN_RB_OVF_SAVE_NEW;
    stcInit.u16AFSel  = APP_CAN_AF_SEL;

	//����CAN������Ϊ125Kbps
    stcInit.stcSBT.u32SEG1 = 1U + 15U;
    stcInit.stcSBT.u32SEG2 = 4U;
    stcInit.stcSBT.u32SJW  = 4U;
    stcInit.stcSBT.u32Prescaler = 16U;

    PWC_Fcg1PeriphClockCmd(APP_CAN_PERIP_CLK, Enable);

    (void)CAN_Init(APP_CAN_UNIT, &stcInit);

    /* Configures the interrupts if needed. */
    Can1IrqConfig();
}

#if 0  //��δʹ��
/**********************************************************************************************************
**	�� �� �� : CanDebugInfPrint
**	����˵�� : CAN1������Ϣ��ӡ���
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void CanDebugInfPrint(void)
{
    uint8_t u8ErrType;
    if (m_u32StatusVal != 0U)
    {
        u8ErrType = (m_u8ErrType >> 5U);
        printf("---> CAN error type: %u, %s\n", u8ErrType, m_s8ErrorTypeStr[u8ErrType]);
        printf("CAN status flags %.8x\n", (unsigned int)m_u32StatusVal);

        if ((m_u32StatusVal & CAN_FLAG_PTB_TRANS_OK) != 0UL)
        {
            /* Transmission OK. */
            printf("PTB has been transmitted.\n");
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_PTB_TRANS_OK);
        }
        if ((m_u32StatusVal & CAN_FLAG_STB_TRANS_OK) != 0UL)
        {
            printf("STB has been transmitted.\n");
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_STB_TRANS_OK);
        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_OFF) != 0UL)
        {
            /* BUS OFF. Needs power on reset or 128 occurrence of 11 consecutive 'recessive' bits to resume. */
            printf("BUS OFF!!!\n");
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_BUS_OFF);
        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_TX) != 0UL)
        {
            /* The BUS is transmitting. Can abort the transmission if needed. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_BUS_TX);
        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_RX) != 0UL)
        {
            /* The BUS is receiving. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_BUS_RX);
        }
        if ((m_u32StatusVal & CAN_FLAG_TB_FULL) != 0UL)
        {
            /* TX buffers are full. */
            printf("Transmit buffers are full.\n");
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_TB_FULL);
        }
        if ((m_u32StatusVal & CAN_FLAG_RB_OVF) != 0UL)
        {
            /* RX buffer overflow. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_RB_OVF);
        }
        if ((m_u32StatusVal & CAN_FLAG_TRANS_ABORTED) != 0UL)
        {
            /* Transmission is aborted. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_TRANS_ABORTED);
        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_INT) != 0UL)
        {
            /* Error interrupt. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_ERR_INT);
        }
        if ((m_u32StatusVal & CAN_FLAG_RB_ALMOST_FULL) != 0UL)
        {
            /* Receive buffers are almost full. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_RB_ALMOST_FULL);
        }
        if ((m_u32StatusVal & CAN_FLAG_RB_FIFO_FULL) != 0UL)
        {
            /* Receive buffers are full. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_RB_FIFO_FULL);
        }
        if ((m_u32StatusVal & CAN_FLAG_RX_OVERRUN) != 0UL)
        {
            /* Receive buffer overrun. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_RX_OVERRUN);
        }
        if ((m_u32StatusVal & CAN_FLAG_RX_OK) != 0UL)
        {
            /* Received a frame. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_RX_OK);
        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_ERR) != 0UL)
        {
            /* Bus error. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_BUS_ERR);
        }
        if ((m_u32StatusVal & CAN_FLAG_ARB_LOST) != 0UL)
        {
            /* Arbitration lost. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_ARB_LOST);
        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_PASSIVE) != 0UL)
        {
            /* Error passive. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_ERR_PASSIVE);
        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_PASSIVE_NODE) != 0UL)
        {
            /* The node is an error passive node. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_ERR_PASSIVE_NODE);
        }
        if ((m_u32StatusVal & CAN_FLAG_REACH_WARN_LIMIT) != 0UL)
        {
            /* TEC or REC reached warning limit. */
            m_u32StatusVal &= (uint32_t)(~CAN_FLAG_REACH_WARN_LIMIT);
        }
    }
}
#endif 

/**********************************************************************************************************
**	�� �� �� : Can1IrqConfig
**	����˵�� : CAN1�ж�����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Can1IrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = APP_CAN_INT_SRC;
    stcCfg.enIRQn      = APP_CAN_IRQn;
    stcCfg.pfnCallback = &CAN1_IrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);
    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, APP_CAN_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    //����CAN�����ж�Ϊ���ս����ж�
    CAN_IntCmd(APP_CAN_UNIT, APP_CAN_INT_TYPE, Enable);
}

/**********************************************************************************************************
**	�� �� �� : Can1IrqConfig
**	����˵�� : CAN1�жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void CAN1_IrqCallback(void)
{
    m_u32StatusVal = CAN_GetStatusVal(APP_CAN_UNIT);
    m_u8ErrType    = CAN_GetErrType(APP_CAN_UNIT);
	
	//�����������
	CANx_ResvData(APP_CAN_UNIT);

	//���CAN�жϱ�־λ
    CAN_ClrStatus(APP_CAN_UNIT, m_u32StatusVal);
}

/*
*********************************************************************************************************
**  �� �� ��: ACDC_Fan_Speed51_ModeTest
**  ����˵��: ACDCģ����ԣ��㲥��ʽ���÷��ȵ�51%ת�٣������Ե�ǰ�豸ģ�����ͣ���ʼ������
**  ��    ��: ��
**  �� �� ֵ: 0:����ʧ�ܣ�1�����磬2���յ£���0����£��ɸ�ֵ��ȫ�ֱ���ACDC.Type
*********************************************************************************************************
*/
uint8_t ACDC_Fan_Speed51_ModeTest(void)
{
    /*����ģ��㲥����*/
    uint32_t ID1 = 0x10200000 | (uint16_t)(0xFF << 8);
    uint8_t Tdata1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Tdata1[1] = 0x37;
    Tdata1[7] = 0x33;   //51%ת��
    CANx_SendData(APP_CAN_UNIT, ID1, &Tdata1[0], 8);  //����ģ��0,1
    bsp_DelayMS(1000);    //�ȴ�2S���մ���
	
    if(ACDC.FanSpd[0] == 0x33)
    {
        return 1;
    }

    /*�յ�ģ��㲥����*/
    ACDC.FanSpd[0]=0;
    ACDC.FanSpd[1]=0;
    uint32_t ID2 = 0xC600000B;
    uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Tdata2[0] = 0x01;
    Tdata2[3] = 0x1F;
    Tdata2[7] = 0x33;      //51%ת��
    CANx_SendData(APP_CAN_UNIT, ID2, &Tdata2[0], 8);
    bsp_DelayMS(1000);     //�ȴ�1S���մ���
	
    if(ACDC.FanSpd[0] == 0x33)
    {
        return 2;
    }

    return 0;
}


/*
*********************************************************************************************************
**  �� �� ��: ACDC_Fan_Speed_CMD
**  ����˵��: ���ȵ�������
**  ��    ��: type:1����*2��2�յ�*2��addr:ģ���ַ  Speed��0~100
**  �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_Fan_Speed_CMD(uint8_t type, uint8_t addr, uint8_t Speed)
{
    //�������
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata[1] = 0x37;
        Tdata[7] = Speed;
        CANx_SendData(APP_CAN_UNIT, ID, &Tdata[0], 8);
    }

    //�յµ���
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x01, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x00;    //��������
        Tdata2[3] = 0x1F;    //���ȵ���
        Tdata2[7] = Speed;   //�趨ֵ
        CANx_SendData(APP_CAN_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  �� �� ��: ACDC_DCV_Query_CMD
**  ����˵��: ACDCֱ�����ѹ��ѯ����ָ�ִ�к���ʱ0.5S���ACDC.����ֵ
**  ��    ��: type:1����*2��2�յ�*2,addr:ģ���ַ
**  �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_DCV_Query_CMD(uint8_t type, uint8_t addr)
{
    //����DC��ѹ��ѯ
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN_UNIT, ID, &Tdata[0], 8);
    }
    //�յ�DC��ѹ��ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x01;    //DC��ѹ��ѯָ��
        CANx_SendData(APP_CAN_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  �� �� ��: ACDC_DCI_Query_CMD
**  ����˵��: ACDCֱ���������ѯ,ִ�к���ʱ0.5S���ACDC.����ֵ
**  ��    ��: type:1����*2��2�յ�*2,addr:ģ���ַ
**  �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_DCI_Query_CMD(uint8_t type, uint8_t addr)
{
    //����DC������ѯ
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN_UNIT, ID, &Tdata[0], 8);
    }
    //�յ�DC������ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x02;    //DC������ѯָ��
        CANx_SendData(APP_CAN_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  �� �� ��: ACDC_ACV_Query_CMD
**  ����˵��: ACDC������ѹ��ѯ,ִ�к���ʱ0.5S���ACDC.����ֵ
**  ��    ��: type:1����*2��2�յ�*2,addr:ģ���ַ
**  �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_ACV_Query_CMD(uint8_t type, uint8_t addr)
{
    //����AC��ѹ��ѯ
    if(type == 1) 
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN_UNIT, ID, &Tdata[0], 8);
    }
    //�յ�AC��ѹ��ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x05;    //AC��ѹ��ѯָ��
        CANx_SendData(APP_CAN_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  �� �� ��: ACDC_ACI_Query_CMD
**  ����˵��: ACDC����������ѯ
**  ��    ��: type:1����*2��2�յ�*2,addr:ģ���ַ
**  �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_ACI_Query_CMD(uint8_t type, uint8_t addr)
{
    //����AC������ѯ
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN_UNIT, ID, &Tdata[0], 8);
    }
    //�յ�AC������ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x06;    //AC������ѯָ��
        CANx_SendData(APP_CAN_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  �� �� ��: ACDC_ACF_Query_CMD
**  ����˵��: ACDC����Ƶ�ʲ�ѯ
**  ��    ��: addr:ģ���ַ
**  �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_ACF_Query_CMD(uint8_t type, uint8_t addr)
{
    //����ACƵ�ʲ�ѯ
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN_UNIT, ID, &Tdata[0], 8);
    }
    //�յ�ACƵ�ʲ�ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x08;    //ACƵ�ʲ�ѯָ��
        CANx_SendData(APP_CAN_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  �� �� ��: ACDC_Fault_Clear_CMD
**  ����˵��: ACDC�������
**  ��    ��: addr:ģ���ַ
**  �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_Fault_Clear_CMD(uint8_t type, uint8_t addr)
{
    //����������
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN_UNIT, ID, &Tdata[0], 8);
    }
    //�յ¹������
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x00;    //��������
        Tdata2[3] = 0x42;    //�������ָ��
        Tdata2[7] = 0x01;    //����ֵ
        CANx_SendData(APP_CAN_UNIT, ID2, &Tdata2[0], 8);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: ACDC_FW_VERSION_CMD
*   ����˵��: ACDC�̼��汾��ѯ
*   ��    ��: addr:ģ���ַ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_FW_VERSION_CMD(uint8_t type, uint8_t addr)
{
    //����̼��汾��ѯ
    if(type == 1)
    {

    }
    //�յ¹̼��汾��ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��������
        Tdata2[3] = 0xBE;    //�汾��ѯ
        CANx_SendData(APP_CAN_UNIT, ID2, &Tdata2[0], 8);
    }
}







/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
