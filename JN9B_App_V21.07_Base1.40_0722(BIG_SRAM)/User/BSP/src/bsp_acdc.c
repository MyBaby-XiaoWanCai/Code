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
uint8_t TW_AS_UpdateFlag=0x00;                      //ͼΪ�澯״̬ͬ�����±�־

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

//static uint8_t CanRxBuffer[MSG_MAX_NUM][APP_DATA_SIZE];       //CAN���ջ���

ACDC_Inf ACDC= {1,{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}; //����ACDC�ṹ�壬Ĭ�Ϲ���ACDC��������ʼ��Ϊ0

uint8_t TW_HisAlam = 0;         //ͼΪ��ʷ�澯
uint8_t TW_ABitFlg =0;          //ͼΪ�澯�仯λ =1��ʶ�б仯��bit
uint8_t TW_HisStatus = 0x01;    //ͼΪ��ʷ״̬
uint8_t TW_SBitFlg =0;          //ͼΪ״̬�仯λ =1��ʶ�б仯��bit
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
static void Can1Config(void);
static void Can1IrqConfig(void);
static void CAN1_IrqCallback(void);
static void ACDC_DataHandle(stc_can_rx_t *pCanRx, uint8_t u8RxCnt);


/* Modify by CAI HENG (Begin) */
void Can2Config(void);
/* Modify by CAI HENG (End) */

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
    /* Modify by CAI HENG (Begin) */
//    Can2Config();
//    /* Modify by CAI HENG (End) */
}

/**********************************************************************************************************
**	�� �� �� : CANx_SendData
**	����˵�� : CAN���߷������ݺ���
**	��    �� : *CANx - CAN1/CAN2��ID - ID�ţ�*pData - �������ݣ�Len - ���ݳ���
**	�� �� ֵ : enRet - ����״̬@ref en_result_t
**********************************************************************************************************/
en_result_t CANx_SendData(M4_CAN_TypeDef *CANx,  uint32_t ID, uint8_t *pData, uint16_t Len)
{
    en_result_t enTxResult = Ok;
    stc_can_tx_t stcTx = {0};
    uint8_t Err_CNT=0;                //���η���ʧ�����Դ���

    /* �����Ҫ���͵����� */
    stcTx.pu8Data = pData;
    stcTx.u32Ctrl = 0U;
    stcTx.DLC = Len;
    stcTx.u32ID = ID;
    stcTx.IDE = 1U;

    /* �����������ʧ������3�� */
    while ((Ok != CAN_TransData(CANx, &stcTx, CAN_BUF_STB, CAN_STB_TRANS_ALL, 0U)) && (Err_CNT < 4))
    {
//        while(Set==CAN_GetStatus(CANx,CAN_FLAG_TB_FULL))
//        {
//            __NOP();   //��buffer��
//        }
        bsp_DelayUS(5);
        Err_CNT++;
    }

    bsp_DelayUS(5);

    if(Err_CNT==4)
    {
        for(uint8_t j = 0; j < CHANNEL_CFG; j++)
        {
//		IP_Alarm_Flag[j] |= IntelProtect_C1E; //can1����
        }
        enTxResult= Error;
    }

    return enTxResult;
}

/**********************************************************************************************************
**	�� �� �� : CAN1_SendData
**	����˵�� : CAN���߷������ݺ���
**	��    �� : *CANx - CAN1/CAN2��ID - ID�ţ�*pData - �������ݣ�Len - ���ݳ���
**	�� �� ֵ : enRet - ����״̬@ref en_result_t 1 �ɹ� 0 ʧ��
**********************************************************************************************************/
uint8_t CAN1_SendData(uint32_t ID, uint8_t *pData, uint16_t len)
{
		uint8_t TimeOut = 0;
    uint8_t enTxResult=1;
    stc_can_tx_t stcTx = {0};
    uint8_t Err_CNT=0;                //���η���ʧ�����Դ���
    /*��ֺ�С��8�ֽ�β������*/
    /* �����Ҫ���͵����� */
    stcTx.pu8Data = pData; //������ݣ�����=β��ʣ�೤��
    stcTx.u32Ctrl = 0U;          //���ƶ�������0
    stcTx.DLC = len;             //������
    stcTx.u32ID = ID;            //���ID
    stcTx.IDE = 1U;              //��չ֡

		if(Set==CAN_GetStatus(APP_CAN1_UNIT,CAN_FLAG_BUS_OFF))
		{
			heart_beat_flag = 2;
			CAN_AbortTrans(APP_CAN1_UNIT,CAN_BUF_STB);
			bsp_DelayUS(2000);
			CAN_SWReset(APP_CAN1_UNIT);
			bsp_DelayUS(2000);
			enTxResult=0;
			return enTxResult;			
		}

		while((Set==CAN_GetStatus(APP_CAN1_UNIT,CAN_FLAG_TB_FULL))&&TimeOut<10)
		{	
			 TimeOut++;
			 bsp_DelayUS(100);
			 Feed_WDG();	
    }
//    while(Set==CAN_GetStatus(APP_CAN1_UNIT,CAN_FLAG_TB_FULL));
		
    TimeOut=0;
    /* �����������ʧ������3�� */
    while ((Ok != CAN_TransData(APP_CAN1_UNIT, &stcTx, CAN_BUF_STB, CAN_STB_TRANS_ALL, 0U)) && (Err_CNT < 4))
    {
        Err_CNT++;

				while((Set==CAN_GetStatus(APP_CAN1_UNIT,CAN_FLAG_TB_FULL))&&TimeOut<10)
				{	
					TimeOut++;
					bsp_DelayUS(100);
					Feed_WDG();	
			 }
			 TimeOut=0;
			 bsp_DelayUS(5);
    }

    while((Set==CAN_GetStatus(APP_CAN1_UNIT,CAN_FLAG_BUS_TX))&&TimeOut<10)
		{	
			 TimeOut++;
			 bsp_DelayUS(100);
			 Feed_WDG();	
		}
		TimeOut=0;
		bsp_DelayMS(1);

    if(Err_CNT==4)
    {
        enTxResult=0;
    }
    Err_CNT=0;

    return enTxResult;

/******************************************************************************************************/

//    uint8_t enTxResult=1;
//    stc_can_tx_t stcTx = {0};
//    uint8_t Err_CNT=0;                //���η���ʧ�����Դ���
//    /*��ֺ�С��8�ֽ�β������*/
//    /* �����Ҫ���͵����� */
//    stcTx.pu8Data = pData; //������ݣ�����=β��ʣ�೤��
//    stcTx.u32Ctrl = 0U;          //���ƶ�������0
//    stcTx.DLC = len;             //������
//    stcTx.u32ID = ID;            //���ID
//    stcTx.IDE = 1U;              //��չ֡

//    while(Set==CAN_GetStatus(APP_CAN1_UNIT,CAN_FLAG_TB_FULL));

//    /* �����������ʧ������3�� */
//    while ((Ok != CAN_TransData(APP_CAN1_UNIT, &stcTx, CAN_BUF_STB, CAN_STB_TRANS_ALL, 0U)) && (Err_CNT < 4))
//    {
//        Err_CNT++;

//        while(Set==CAN_GetStatus(APP_CAN1_UNIT,CAN_FLAG_TB_FULL)) {};

//        bsp_DelayUS(5);
//    }

//    while(Set==CAN_GetStatus(APP_CAN1_UNIT,CAN_FLAG_BUS_TX)) {};

//    bsp_DelayMS(1);

//    if(Err_CNT==4)
//    {
//        enTxResult=0;
//    }
//    Err_CNT=0;

//    return enTxResult;
}

/**********************************************************************************************************
**	�� �� �� : CAN2_SendData
**	����˵�� : CAN���߷������ݺ���
**	��    �� : *CANx - CAN1/CAN2��ID - ID�ţ�*pData - �������ݣ�Len - ���ݳ���<=8
**	�� �� ֵ : enRet - ����״̬@ref en_result_t 1 �ɹ� 0 ʧ��
**********************************************************************************************************/
uint8_t CAN2_SendData(uint32_t ID, uint8_t *pData, uint16_t len)
{
    uint8_t enTxResult=1;
    stc_can_tx_t stcTx = {0};
    uint8_t Err_CNT=0;                //���η���ʧ�����Դ���
    uint8_t TimeOut=0;
    /*��ֺ�С��8�ֽ�β������*/
    /* �����Ҫ���͵����� */
    stcTx.pu8Data = pData;            //������ݣ�����=β��ʣ�೤��
    stcTx.u32Ctrl = 0U;               //���ƶ�������0
    if(len<=8)                        //ȷ�����Ȳ�����8�ֽڣ�������෢8�ֽ�
    {
        stcTx.DLC = len;                  //������
    }
    else
    {
        stcTx.DLC=8;
    }
    stcTx.u32ID = ID;                 //���ID
    stcTx.IDE = 1U;                   //��չ֡

    while((Set==CAN_GetStatus(APP_CAN2_UNIT,CAN_FLAG_TB_FULL))&&(TimeOut<10))        //��ѯTB����־����ʱ10������
    {
        TimeOut++;
        bsp_DelayUS(100);
    };
    if(TimeOut==10)
    {
        CAN_AbortTrans(APP_CAN2_UNIT,CAN_BUF_STB);                                    //����buf����ȡ��STB����
        CAN_ClrStatus(APP_CAN2_UNIT,CAN_FLAG_TB_FULL);                                //���TB_FULL��־
        bsp_DelayUS(100);
        TimeOut=0;
    }
    /* �����������ʧ������3�� */
    while ((Ok != CAN_TransData(APP_CAN2_UNIT, &stcTx, CAN_BUF_STB, CAN_STB_TRANS_ALL, 0U)) && (Err_CNT < 4))
    {
        CAN_AbortTrans(APP_CAN2_UNIT,CAN_BUF_STB);                                    //����buf����ȡ��STB����
        CAN_ClrStatus(APP_CAN2_UNIT,CAN_FLAG_TB_FULL);                                //���TB_FULL��־
        Err_CNT++;

        while((Set==CAN_GetStatus(APP_CAN2_UNIT,CAN_FLAG_TB_FULL))&&(TimeOut<10))        //��ѯTB����־����ʱ10������
        {
            TimeOut++;
            bsp_DelayUS(100);
        };
        if(TimeOut==10)
        {
            CAN_AbortTrans(APP_CAN2_UNIT,CAN_BUF_STB);                                    //����buf����ȡ��STB����
            CAN_ClrStatus(APP_CAN2_UNIT,CAN_FLAG_TB_FULL);                                //���TB_FULL��־
            bsp_DelayUS(100);
            TimeOut=0;
        }
        bsp_DelayUS(5);
    }

    while(Set==CAN_GetStatus(APP_CAN2_UNIT,CAN_FLAG_BUS_TX)) {};                          //�ȴ��������

    bsp_DelayUS(5);

    if(Err_CNT==4)
    {
        for(uint8_t j = 0; j < CHANNEL_CFG; j++)
        {
            IP_Alarm_Flag[j] |= IntelProtect_C2E; //can2����
        }
        enTxResult=0;
    }
    Err_CNT=0;

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
    uint8_t i = 0;
    uint8_t AcdcIndex = 0;

    if(ACDC_UpdateFlg == 1)			/*����ģʽ���ظ�����*/
    {
        if((pCanRx[u8RxCnt].u32ID&0X1F000000) == 0X10500000)		//����
        {
            uint32_t TempId = 0;

            //�������ݵ��ṹ�壬�ṹ�巢��Ϣ����
            TempId = (pCanRx[u8RxCnt].u32ID>>24&0xFF)+((pCanRx[u8RxCnt].u32ID>>16&0xFF)<<8)+  \
                     ((pCanRx[u8RxCnt].u32ID>>8&0xFF)<<16) +((pCanRx[u8RxCnt].u32ID&0xFF)<<24); //ID��С��ת��
            CanRet.ID = TempId;
            memcpy(CanRet.Date, pCanRx[u8RxCnt].pu8Data, 8);
            CanRet.processedFlg = 1;    //1��ʾ�յ������ظ����ݴ�����
        }
        else if((pCanRx[u8RxCnt].u32ID&0X1F000000) == 0X1c000000)
        {
            uint32_t TempId = 0;

            //�������ݵ��ṹ�壬�ṹ�巢��Ϣ����
            TempId = (pCanRx[u8RxCnt].u32ID>>24&0xFF)+((pCanRx[u8RxCnt].u32ID>>16&0xFF)<<8)+  \
                     ((pCanRx[u8RxCnt].u32ID>>8&0xFF)<<16) +((pCanRx[u8RxCnt].u32ID&0xFF)<<24); //ID��С��ת��
            CanRet.ID = TempId;
            memcpy(CanRet.Date, pCanRx[u8RxCnt].pu8Data, 8);
            CanRet.processedFlg = 1;    //1��ʾ�յ������ظ����ݴ�����
        }
        else
        {
            //do nothing
        }
    }
    else if(ACDC_UpdateFlg == 0)	/*������ģʽ���ظ�����*/
    {
        if((pCanRx[u8RxCnt].u32ID & 0x00000FFF) == 0x00000380)         //ͼΪ�澯
        {
            if(pCanRx[u8RxCnt].pu8Data[0] != 0x7F)                     //�㲥֡�ж�
            {
                return;
            }
            if(pCanRx[u8RxCnt].pu8Data[1] != 0x90)                     //�������жϣ���λΪ 1��������Ӧ��أ���λΪ 0x10+0x00��\
                Service code 0x01 ��ʾ��Դ�澯��Ϣ�����룬\
                0x00 ��ʾ��ǰ�澯������ 1#��Դ�ĸ澯��Ϣ
            {
                return;
            }

            if( TW_HisAlam != pCanRx[u8RxCnt].pu8Data[2])               //�жϸ澯״̬�Ƿ��и���
            {
                TW_ABitFlg  = TW_HisAlam ^ pCanRx[u8RxCnt].pu8Data[2];  //��¼״̬�ı��bit
                TW_HisAlam  = pCanRx[u8RxCnt].pu8Data[2];               //������ʷ�澯ֵ
                TW_AS_UpdateFlag |= 0x10;                               //���¸澯״̬ͬ����־�ĸ澯bitλ
                osSemaphoreRelease(acdc_alarm_Semaphore);               //�ͷ��ź�����ͬ����������
            }

        } else if((pCanRx[u8RxCnt].u32ID & 0x00000FFF) == 0x00000480)   //ͼΪ��״̬
        {
            if(pCanRx[u8RxCnt].pu8Data[0] != 0x7F)                      //�㲥֡�ж�
            {
                return;
            }
            if(pCanRx[u8RxCnt].pu8Data[1] != 0x90)                      //�������жϣ���λΪ 1��������Ӧ��أ���λΪ 0x10+0x00��\
                Service code 0x01 ��ʾ��Դ״̬��Ϣ�����룬\
                0x00 ��ʾ��ǰ״̬������ 1#��Դ��״̬��Ϣ
            {
                return;
            }
            if(TW_HisStatus != (pCanRx[u8RxCnt].pu8Data[2] & 0x01))    //�ж��е��Ƿ�OK��1��������0��������
            {
                TW_SBitFlg = 0x01;                                     //��ʶ�б仯��bit
                TW_HisStatus  = (pCanRx[u8RxCnt].pu8Data[2] & 0x01);   //������ʷ�澯ֵ
                TW_AS_UpdateFlag |= 0x01;                              //���¸澯״̬ͬ����־��״̬bitλ
                osSemaphoreRelease(acdc_alarm_Semaphore);              //�ͷ��ź�����ͬ����������
            }
        }
        else if((pCanRx[u8RxCnt].u32ID & 0x1FFF0000) == 0x102B0000)     //����
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
                if(i==0)
                    ACDC.QueryWaitFlag=QueryACV1;        //���µȴ���־
                else if(i==1)
                    ACDC.QueryWaitFlag=QueryACV2;        //���µȴ���־
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
        else if((pCanRx[u8RxCnt].u32ID&0X1FFF0000) == 0X06080000)     //�յ�
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
                    if(i==0)
                        ACDC.QueryWaitFlag=QueryACV1;        //���µȴ���־
                    else if(i==1)
                        ACDC.QueryWaitFlag=QueryACV2;        //���µȴ���־
                    break;
                }
                case 0x06://ACDC�������������
                {
                    ACDC.ACI[AcdcIndex] = (int)((uint16_t)(pCanRx[u8RxCnt].pu8Data[4] << 24) + (uint16_t)(pCanRx[u8RxCnt].pu8Data[5] << 16) +  \
                                                (uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 1024.0;
                    break;
                }
                case 0xBE://ACDC�̼��汾��ѯ����
                {
                    for(uint8_t m=0; m<4; m++)
                        ACDC.VERSION[i][m]= pCanRx[u8RxCnt].pu8Data[4+m];
                    if(i==0)
                        ACDC.QueryWaitFlag=QueryVERSION1;        //���µȴ���־
                    else if(i==1)
                        ACDC.QueryWaitFlag=QueryVERSION2;        //���µȴ���־
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
    else
    {
        //do nothing;
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

    /* ����ɸѡ������ */
    stc_can_af_cfg_t astcAFCfg[] =
    {
        {APP_CAN1_AF1_ID, APP_CAN1_AF1_ID_MSK, APP_CAN1_AF1_MSK_TYPE}, \
        {APP_CAN1_AF2_ID, APP_CAN1_AF2_ID_MSK, APP_CAN1_AF2_MSK_TYPE} \
    };

    /* Set the function of CAN pins. */
    GPIO_SetFunc(APP_CAN1_TX_PORT, APP_CAN1_TX_PIN, \
                 APP_CAN1_TX_PIN_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(APP_CAN1_RX_PORT, APP_CAN1_RX_PIN, \
                 APP_CAN1_RX_PIN_FUNC, PIN_SUBFUNC_DISABLE);

    /* Configures the clock of CAN. */
    CLK_CAN_ClkConfig(APP_CAN1_CLK_CH, APP_CAN1_CLK_DIV);

    /* Initializes CAN. */
    (void)CAN_StructInit(&stcInit);
    stcInit.u8WorkMode = CAN_MODE_NORMAL;
    stcInit.u8RBSWarnLimit = MSG_MAX_NUM;         //�趨RingBuffer�����ж���󻺴�Ϊ3��Range[1,8]
    stcInit.pstcAFCfg = astcAFCfg;
    stcInit.u8RBOvfOp = CAN_RB_OVF_SAVE_NEW;
    stcInit.u16AFSel  = CAN_AF1 | CAN_AF2;

//    stcInit.stcSBT.u32SEG1 = 3U;
//    stcInit.stcSBT.u32SEG2 = 2U;
//    stcInit.stcSBT.u32SJW  = 1U;

//#if (CAN_BAUDRATE == 500)
//    stcInit.stcSBT.u32Prescaler = 16U;        //16
//#else
//    stcInit.stcSBT.u32Prescaler = 8U;
//#endif

    stcInit.stcSBT.u32SEG1 = 12U;
    stcInit.stcSBT.u32SEG2 = 4U;
    stcInit.stcSBT.u32SJW  = 3U;

#if (CAN_BAUDRATE == 500)
    stcInit.stcSBT.u32Prescaler = 5U;        //16
#else
    stcInit.stcSBT.u32Prescaler = 8U;
#endif

    PWC_Fcg1PeriphClockCmd(APP_CAN1_PERIP_CLK, Enable);

    (void)CAN_Init(APP_CAN1_UNIT, &stcInit);

    /* Configures the interrupts if needed. */
    Can1IrqConfig();
}

/* Modify by CAI HENG (Begin) */

/**********************************************************************************************************
**	�� �� �� : CAN1_ResvData
**	����˵�� : CAN���߽������ݺ���
**	��    �� : stcRx �������ݽ����ṹ��
**	�� �� ֵ :
**********************************************************************************************************/
void CAN1_ResvData(void)
{
    stc_can_rx_t stcRx;

    uint8_t rev[8]= {0}; //���ݻ�����

    stcRx.pu8Data = rev;

    while (READ_REG8_BIT(M4_CAN1->RCTRL, CAN_RCTRL_RSTAT) != CAN_RB_STAT_EMPTY)
    {
        CAN_ReceiveData(APP_CAN1_UNIT, &stcRx, NULL, 1);

        Can_Network_Rx_Process(stcRx.u32ID, rev);
    }
}

/**********************************************************************************************************
**	�� �� �� : Can1IrqConfig
**	����˵�� : CAN1�ж�����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Can1IrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = APP_CAN1_INT_SRC;
    stcCfg.enIRQn      = APP_CAN1_IRQn;
    stcCfg.pfnCallback = &CAN1_IrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);
    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, APP_CAN1_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of CAN. */
    CAN_IntCmd(APP_CAN1_UNIT, APP_CAN1_INT_TYPE, Enable);
}

/**********************************************************************************************************
**	�� �� �� : Can1IrqConfig
**	����˵�� : CAN1�жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void CAN1_IrqCallback(void)
{
    uint8_t m_u8ErrType = 0;
    uint32_t m_u32StatusVal = 0;
    m_u32StatusVal = CAN_GetStatusVal(APP_CAN1_UNIT);
    m_u8ErrType    = CAN_GetErrType(APP_CAN1_UNIT);

		Feed_WDG(); 
	
    if (m_u32StatusVal != 0U)
    {
        m_u8ErrType = (m_u8ErrType >> 5U);                     //��ȡ�������ͣ�0:CAN_ERR_NO,1: CAN_ERR_BIT,2: CAN_ERR_FORM \
                                                                3:CAN_ERR_STUFF,4:CAN_ERR_ACK,5:CAN_ERR_CRC,6:CAN_ERR_OTHER
        if ((m_u32StatusVal & CAN_FLAG_PTB_TRANS_OK) != 0UL)   //PTB���ͳɹ�
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_STB_TRANS_OK) != 0UL)   //STB���ͳɹ�
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_OFF) != 0UL)        //����BUSOFF״̬����ȴ�11bit�������Ե�ƽ�������ϵ�ָ�����
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_TX) != 0UL)         //����æ������ȡ������
        {

        }

        if ((m_u32StatusVal & CAN_FLAG_BUS_RX) != 0UL)        //�������ݽ�����
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_TB_FULL) != 0UL)       //����buffer��
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RB_OVF) != 0UL)        //����buffer���
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_TRANS_ABORTED) != 0UL) //����ȡ��
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_INT) != 0UL)       //�����ж�
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RB_ALMOST_FULL) != 0UL)//���ս����ж�
        {
            CAN1_ResvData();                                  //CAN1���մ���
        }
        if ((m_u32StatusVal & CAN_FLAG_RB_FIFO_FULL) != 0UL)  //�������ж�
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RX_OVERRUN) != 0UL)    //���ճ����ж�
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RX_OK) != 0UL)         //��֡�����ж�
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_ERR) != 0UL)       //���ߴ����ж�
        {
            CAN_ClrStatus(APP_CAN1_UNIT,CAN_FLAG_BUS_ERR);    //��ʱ��������־
        }
        if ((m_u32StatusVal & CAN_FLAG_ARB_LOST) != 0UL)      //�ٲ�ʧ���ж�
        {
            CAN_ClrStatus(APP_CAN1_UNIT,CAN_FLAG_ARB_LOST);   //��ʱ��������־
        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_PASSIVE) != 0UL)   //���������ж�
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_PASSIVE_NODE) != 0UL)//��������ڵ�
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_REACH_WARN_LIMIT) != 0UL)//TEC/REC�ﵽ�澯�޶�ֵ
        {

        }

        CAN_ClrStatus(APP_CAN1_UNIT, m_u32StatusVal);              //���CAN1״̬��־
    }

}


/**********************************************************************************************************
**	�� �� �� : CAN2_ResvData
**	����˵�� : CAN2���߽������ݺ���
**	��    �� : stcRx �������ݽ����ṹ��
**	�� �� ֵ :
**********************************************************************************************************/
void CAN2_ResvData(void)
{
    stc_can_rx_t stcRx;
    uint8_t rev[8]= {0}; //���ݻ�����
    stcRx.pu8Data = rev; //�����ڴ��ʼ��Ϊ0

    while (READ_REG8_BIT(M4_CAN2->RCTRL, CAN_RCTRL_RSTAT) != CAN_RB_STAT_EMPTY)       //���RB״̬�Ƿ�Ϊ��
    {
        CAN_ReceiveData(APP_CAN2_UNIT, &stcRx, NULL, 1);                               //ȡ����д��ṹ��
        ACDC_DataHandle(&stcRx, 0);                                                   //ִ��CAN���ݴ�����
    }
}

/**********************************************************************************************************
**	�� �� �� : CAN2_IrqCallback
**	����˵�� : CAN2�жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void CAN2_IrqCallback(void)
{
    uint8_t m_u8ErrType = 0;
    uint32_t m_u32StatusVal = 0;
    m_u32StatusVal = CAN_GetStatusVal(APP_CAN2_UNIT);          //��ȡCAN1״̬��־
    m_u8ErrType    = CAN_GetErrType(APP_CAN2_UNIT);            //��ȡCAN1��������

    if (m_u32StatusVal != 0U)
    {
        m_u8ErrType = (m_u8ErrType >> 5U);                     //��ȡ�������ͣ�0:CAN_ERR_NO,1: CAN_ERR_BIT,2: CAN_ERR_FORM \
                                                                3:CAN_ERR_STUFF,4:CAN_ERR_ACK,5:CAN_ERR_CRC,6:CAN_ERR_OTHER
        if ((m_u32StatusVal & CAN_FLAG_PTB_TRANS_OK) != 0UL)   //PTB�������
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_STB_TRANS_OK) != 0UL)   //STB�������
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_OFF) != 0UL)        //����BUSOFF״̬����ȴ�11bit�������Ե�ƽ�������ϵ�ָ����������жϱ�־
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_TX) != 0UL)         //����æ������ȡ�����ͣ����жϱ�־
        {

        }

        if ((m_u32StatusVal & CAN_FLAG_BUS_RX) != 0UL)        //�������ݽ����У����жϱ�־
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_TB_FULL) != 0UL)       //����buffer��
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RB_OVF) != 0UL)        //����buffer��������жϱ�־
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_TRANS_ABORTED) != 0UL) //����ȡ��
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_INT) != 0UL)       //�����ж�
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RB_ALMOST_FULL) != 0UL)//���ս����ж�
        {
            CAN2_ResvData();                                  //CAN2���մ���
        }
        if ((m_u32StatusVal & CAN_FLAG_RB_FIFO_FULL) != 0UL)  //�������ж�
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RX_OVERRUN) != 0UL)    //���ճ���
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RX_OK) != 0UL)         //��֡��������
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_ERR) != 0UL)       //���ߴ���
        {
            CAN_ClrStatus(APP_CAN2_UNIT,CAN_FLAG_BUS_ERR);    //��ʱ��������־
        }
        if ((m_u32StatusVal & CAN_FLAG_ARB_LOST) != 0UL)      //�ٲ�ʧ��
        {
            CAN_ClrStatus(APP_CAN2_UNIT,CAN_FLAG_ARB_LOST);    //��ʱ��������־
        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_PASSIVE) != 0UL)   //��������
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_PASSIVE_NODE) != 0UL)//��������ڵ�
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_REACH_WARN_LIMIT) != 0UL)//TEC/REC�ﵽ�澯�޶�ֵ
        {

        }

        CAN_ClrStatus(APP_CAN2_UNIT, m_u32StatusVal);              //���CAN2״̬���жϱ�־
    }

}

/**********************************************************************************************************
**	�� �� �� : Can2IrqConfig
**	����˵�� : CAN2�ж�����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Can2IrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;
    stcCfg.enIntSrc    = APP_CAN2_INT_SRC;                      //�趨�ж�Դ
    stcCfg.enIRQn      = APP_CAN2_IRQn;                         //�趨�жϺ�
    stcCfg.pfnCallback = &CAN2_IrqCallback;                    //�趨�жϻص�����
    (void)INTC_IrqSignIn(&stcCfg);                             //ע���ж�����
    NVIC_ClearPendingIRQ(stcCfg.enIRQn);                       //����жϹ���״̬
    NVIC_SetPriority(stcCfg.enIRQn, APP_CAN2_INT_PRIO);         //�趨�ж����ȼ�
    NVIC_EnableIRQ(stcCfg.enIRQn);                             //ʹ���жϺ�

    //����CAN�����ж�Ϊ���ս����ж�
    CAN_IntCmd(APP_CAN2_UNIT, APP_CAN2_INT_TYPE, Enable);        //ʹ��CAN1�����ж�
}

/**********************************************************************************************************
**	�� �� �� : Can2Config
**	����˵�� : CAN2��ʼ������
**	��    �� : ���ں�ͼΪACDCͨ��
**	�� �� ֵ : ��
**  ˵    �� ��CANʱ�ӣ�240MHz / 6 = 40Mhz��CAN�����ʣ�40MHz / Prescaler / (SEG1+SEG2) = 500Kbps
**********************************************************************************************************/
void Can2Config(void)
{
    stc_can_init_t stcInit;
    CAN_DeInit(APP_CAN2_UNIT);	//CAN1 Deinit

    stc_can_af_cfg_t astcAFCfg[] =
    {
        {APP_CAN2_AF1_ID, APP_CAN2_AF1_ID_MSK, APP_CAN2_AF1_MSK_TYPE},               //����ͼΪ�澯����
        {APP_CAN2_AF2_ID, APP_CAN2_AF2_ID_MSK, APP_CAN2_AF2_MSK_TYPE},               //����ͼΪ״̬����
    };

    /* Set the function of CAN pins. */
    GPIO_SetFunc(APP_CAN2_TX_PORT, APP_CAN2_TX_PIN, \
                 APP_CAN2_TX_PIN_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(APP_CAN2_RX_PORT, APP_CAN2_RX_PIN, \
                 APP_CAN2_RX_PIN_FUNC, PIN_SUBFUNC_DISABLE);

    /* Configures the clock of CAN. */
    CLK_CAN_ClkConfig(APP_CAN2_CLK_CH, APP_CAN2_CLK_DIV);

    /* Initializes CAN. */
    (void)CAN_StructInit(&stcInit);
//	stcInit.u8WorkMode = CAN_MODE_ILB;                     //�ڲ��ػ����ڲ���
    stcInit.u8WorkMode = CAN_MODE_NORMAL;                  //����ģʽ
    stcInit.u8RBSWarnLimit = MSG_MAX_NUM;                  //�趨RingBuffer�����ж���󻺴�Ϊ1��Range[1,8]
    stcInit.pstcAFCfg = astcAFCfg;                         //ID����������
    stcInit.u8RBOvfOp = CAN_RB_OVF_SAVE_NEW;               //����д����
    stcInit.u16AFSel  = APP_CAN2_AF_SEL;                    //ָ��AF������

    //����CAN������Ϊ500Kbps
    stcInit.stcSBT.u32SEG1 = 3U;
    stcInit.stcSBT.u32SEG2 = 2U;
    stcInit.stcSBT.u32SJW  = 1U;
    stcInit.stcSBT.u32Prescaler = 16U;

    PWC_Fcg1PeriphClockCmd(APP_CAN2_PERIP_CLK, Enable);      //ʹ������ʱ��

    (void)CAN_Init(APP_CAN2_UNIT, &stcInit);                 //ִ�г�ʼ������

    /* Configures the interrupts if needed. */
    Can2IrqConfig();
}
/* Modify by CAI HENG (End) */

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
    uint8_t i;
#if 1
    /*����ģ��㲥����*/
    uint32_t ID1 = 0x10200000 | (uint16_t)(0xFF << 8);
    uint8_t Tdata1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Tdata1[1] = 0x37;
    Tdata1[7] = 0x33;   //51%ת��
    ACDC.FanSpd[0]=0;   //�ж�������0
    ACDC.FanSpd[1]=0;
    for(i=0; i<3; i++)
    {
        CANx_SendData(APP_CAN2_UNIT, ID1, &Tdata1[0], 8);  //����ģ��0,1
        osDelay(200);       //��ʱ�ȴ�
    }
#endif
#if 0
    /*����ģ��㲥����*/
    uint32_t ID1 = 0x10200000 | (uint16_t)(0x00 << 8);
    uint8_t Tdata1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Tdata1[1] = 0x37;
    Tdata1[7] = 0x33;   //51%ת��
    CANx_SendData(APP_CAN_UNIT, ID1, &Tdata1[0], 8);  //����ģ��0
    bsp_DelayMS(500);    //�ȴ�0.5S���մ���

#ifndef YCJ
    ID1= 0x10200000 | (uint16_t)(0x01 << 8);
    CANx_SendData(APP_CAN_UNIT, ID1, &Tdata1[0], 8);  //����ģ��1
#endif
#endif

#ifdef YCJ
    if(ACDC.FanSpd[0]==0x33)
    {
        return 1;
    }
#else

    if((ACDC.FanSpd[0]==0x33)&&(ACDC.FanSpd[1]==0x33))
    {
        return 1;      //2��ACDC������
    } else if((ACDC.FanSpd[0]==0x33)||(ACDC.FanSpd[1]==0x33))
    {
        return 11;    //1��ACDC������һ��������
    }
#endif

    /*�յ�ģ��㲥����*/
    ACDC.FanSpd[0]=0;
    ACDC.FanSpd[1]=0;
    uint32_t ID2 = 0xC600000B;
    uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Tdata2[0] = 0x01;
    Tdata2[3] = 0x1F;
    Tdata2[7] = 0x33;      //51%ת��
    for(i=0; i<3; i++)     //��3�����ټ�⣬���ⶪ��
    {
        CANx_SendData(APP_CAN2_UNIT, ID2, &Tdata2[0], 8);
        osDelay(200);        //��ʱ�ȴ�
    }

#ifdef YCJ
    if(ACDC.FanSpd[0]==0x33)
    {
        return 2;
    }
#else
    if((ACDC.FanSpd[0]==0x33)&&(ACDC.FanSpd[1]==0x33))
    {
        return 2;      //2��ACDC������
    } else if((ACDC.FanSpd[0]==0x33)||(ACDC.FanSpd[1]==0x33))
    {
        return 22;    //1��ACDC������һ��������
    }
#endif
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
//    //�������
//    if(type == 1)
//    {
//        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
//        uint8_t Tdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//        Tdata[1] = 0x37;
//        Tdata[7] = Speed;
//        CANx_SendData(APP_CAN_UNIT, ID, &Tdata[0], 8);
//    }

//    //�յµ���
//    if(type == 2)
//    {
//        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
//        uint8_t Tdata2[8] = {0x01, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00};
//        Tdata2[0] = 0x01;    //����㲥
//        Tdata2[1] = 0x00;    //��������
//        Tdata2[3] = 0x1F;    //���ȵ���
//        Tdata2[7] = Speed;   //�趨ֵ
//        CANx_SendData(APP_CAN_UNIT, ID2, &Tdata2[0], 8);
//    }
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
        CANx_SendData(APP_CAN2_UNIT, ID, &Tdata[0], 8);
    }
    //�յ�DC��ѹ��ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x01;    //DC��ѹ��ѯָ��
        CANx_SendData(APP_CAN2_UNIT, ID2, &Tdata2[0], 8);
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
        CANx_SendData(APP_CAN2_UNIT, ID, &Tdata[0], 8);
    }
    //�յ�DC������ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x02;    //DC������ѯָ��
        CANx_SendData(APP_CAN2_UNIT, ID2, &Tdata2[0], 8);
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
        CANx_SendData(APP_CAN2_UNIT, ID, &Tdata[0], 8);
    }
    //�յ�AC��ѹ��ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x05;    //AC��ѹ��ѯָ��
        CANx_SendData(APP_CAN2_UNIT, ID2, &Tdata2[0], 8);
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
        CANx_SendData(APP_CAN2_UNIT, ID, &Tdata[0], 8);
    }
    //�յ�AC������ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x06;    //AC������ѯָ��
        CANx_SendData(APP_CAN2_UNIT, ID2, &Tdata2[0], 8);
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
        CANx_SendData(APP_CAN2_UNIT, ID, &Tdata[0], 8);
    }
    //�յ�ACƵ�ʲ�ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x08;    //ACƵ�ʲ�ѯָ��
        CANx_SendData(APP_CAN2_UNIT, ID2, &Tdata2[0], 8);
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
        CANx_SendData(APP_CAN2_UNIT, ID, &Tdata[0], 8);
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
        CANx_SendData(APP_CAN2_UNIT, ID2, &Tdata2[0], 8);
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
        CANx_SendData(APP_CAN2_UNIT, ID2, &Tdata2[0], 8);
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
