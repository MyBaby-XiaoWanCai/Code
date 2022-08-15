/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_can.c
** Author      :
** Date        : 2022-01-18
** Description : bsp_can source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"


extern  uint8_t  Can1_recv[4];                //CAN1��������
extern  uint8_t  Can2_recv[4];                //CAN2��������


/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
static void Can1IrqConfig(void);
static void Can2IrqConfig(void);
static void CAN1_IrqCallback(void);
static void CAN2_IrqCallback(void);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/
/**********************************************************************************************************
**	�� �� �� : bsp_InitCAN1
**	����˵�� : ACDCģ���ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitCAN1(void)
{
	Can1Config();      //����CAN1�Ĵ���
}

/**********************************************************************************************************
**	�� �� �� : bsp_InitCAN2
**	����˵�� : ACDCģ���ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitCAN2(void)
{
	Can2Config();     //����CAN2�Ĵ���
}


/**********************************************************************************************************
**	�� �� �� : Can1Config
**	����˵�� : CAN1��ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� ��CANʱ�ӣ�240MHz / 6 = 40Mhz��CAN�����ʣ�40MHz / Prescaler / (SEG1+SEG2) = 125Kbps
**********************************************************************************************************/
void Can1Config(void)
{
    stc_can_init_t stcInit;

    /* ����ɸѡ������ */
#ifdef FCT_TEST
    /*FCT����Ҫ��CAN��CAN�������շ�������˲�����Ҫ����һ��*/
    stc_can_af_cfg_t astcAFCfg[] =
    {
        {APP_CAN2_AF1_ID, APP_CAN2_AF1_ID_MSK, APP_CAN2_AF1_MSK_TYPE}, \
        {APP_CAN2_AF2_ID, APP_CAN2_AF2_ID_MSK, APP_CAN2_AF2_MSK_TYPE} \

    };
#else    
    stc_can_af_cfg_t astcAFCfg[] =
    {
        {APP_CAN1_AF1_ID, APP_CAN1_AF1_ID_MSK, APP_CAN1_AF1_MSK_TYPE}, \
        {APP_CAN1_AF2_ID, APP_CAN1_AF2_ID_MSK, APP_CAN1_AF2_MSK_TYPE}, \
        {APP_CAN1_AF3_ID, APP_CAN1_AF3_ID_MSK, APP_CAN1_AF3_MSK_TYPE}, \
    };
#endif
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
    stcInit.u8RBOvfOp = CAN_RB_OVF_SAVE_NEW;      //�����������
#ifdef FCT_TEST    
    stcInit.u16AFSel  = CAN_AF1|CAN_AF2;                  //ѡ������˲���1��2
#else
    stcInit.u16AFSel  = CAN_AF1|CAN_AF2|CAN_AF3;          //ѡ������˲���1��2��3
#endif
#ifdef FCT_TEST
    /*FCT����Ҫ��CAN��CAN�������շ������ͨ����������Ҫ����һ��*/
    stcInit.stcSBT.u32SEG1 = 3U;
    stcInit.stcSBT.u32SEG2 = 2U;
    stcInit.stcSBT.u32SJW  = 1U;
    stcInit.stcSBT.u32Prescaler = 16U;
#else      
    stcInit.stcSBT.u32SEG1 = 1U + 15U;
    stcInit.stcSBT.u32SEG2 = 4U;
    stcInit.stcSBT.u32SJW  = 4U;
    stcInit.stcSBT.u32Prescaler = 16U;
#endif
    PWC_Fcg1PeriphClockCmd(APP_CAN1_PERIP_CLK, Enable);

    (void)CAN_Init(APP_CAN1_UNIT, &stcInit);

    /* Configures the interrupts if needed. */
    Can1IrqConfig();
}


/**********************************************************************************************************
**	�� �� �� : Can2Config
**	����˵�� : CAN2��ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� ��CANʱ�ӣ�240MHz / 6 = 40Mhz��CAN�����ʣ�40MHz / Prescaler / (SEG1+SEG2) = 500Kbps
**********************************************************************************************************/
void Can2Config(void)
{
    stc_can_init_t stcInit;

    /* ����ɸѡ������ */
    stc_can_af_cfg_t astcAFCfg[] =
    {
        {APP_CAN2_AF1_ID, APP_CAN2_AF1_ID_MSK, APP_CAN2_AF1_MSK_TYPE}, \
        {APP_CAN2_AF2_ID, APP_CAN2_AF2_ID_MSK, APP_CAN2_AF2_MSK_TYPE} \

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
    stcInit.u8WorkMode = CAN_MODE_NORMAL;
    stcInit.u8RBSWarnLimit = MSG_MAX_NUM;         //�趨RingBuffer�����ж���󻺴�Ϊ3��Range[1,8]
    stcInit.pstcAFCfg = astcAFCfg;
    stcInit.u8RBOvfOp = CAN_RB_OVF_SAVE_NEW;
    stcInit.u16AFSel  = CAN_AF3|CAN_AF4;

    stcInit.stcSBT.u32SEG1 = 3U;
    stcInit.stcSBT.u32SEG2 = 2U;
    stcInit.stcSBT.u32SJW  = 1U;
    stcInit.stcSBT.u32Prescaler = 16U;

    PWC_Fcg1PeriphClockCmd(APP_CAN2_PERIP_CLK, Enable);

    (void)CAN_Init(APP_CAN2_UNIT, &stcInit);

    /* Configures the interrupts if needed. */
    Can2IrqConfig();
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
**	�� �� �� : Can2IrqConfig
**	����˵�� : CAN2�ж�����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Can2IrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = APP_CAN2_INT_SRC;
    stcCfg.enIRQn      = APP_CAN2_IRQn;
    stcCfg.pfnCallback = &CAN2_IrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);
    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, APP_CAN2_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of CAN. */
    CAN_IntCmd(APP_CAN2_UNIT, APP_CAN2_INT_TYPE, Enable);
}


/**********************************************************************************************************
**	�� �� �� : CAN1_IrqCallback
**	����˵�� : CAN1�жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void CAN1_IrqCallback(void)
{
    stc_can_rx_t CAN1_RX;
    uint8_t rev[4]={0};  //���ݻ�����
    uint8_t m_u8ErrType = 0;
    uint32_t m_u32StatusVal = 0;
    
    m_u32StatusVal = CAN_GetStatusVal(APP_CAN1_UNIT);
    m_u8ErrType    = CAN_GetErrType(APP_CAN1_UNIT);
    CAN1_RX.pu8Data = (uint8_t *)&rev[0]; //�������ݵ�����
    CAN1_ResvData(&CAN1_RX);	          //������ա���������
    memcpy(Can1_recv,rev,sizeof(rev));    //������ȫ�ֱ���
    CAN_ClrStatus(APP_CAN1_UNIT, m_u32StatusVal);
}

/**********************************************************************************************************
**	�� �� �� : CAN2_IrqCallback
**	����˵�� : CAN2�жϻص���������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void CAN2_IrqCallback(void)
{
    stc_can_rx_t CAN2_RX;
    uint8_t rev[4]={0};  //���ݻ�����
    uint8_t m_u8ErrType = 0;
    uint32_t m_u32StatusVal = 0;
    m_u32StatusVal = CAN_GetStatusVal(APP_CAN2_UNIT);
    m_u8ErrType    = CAN_GetErrType(APP_CAN2_UNIT);
    CAN2_RX.pu8Data = (uint8_t *)&rev[0];  //�������ݵ�����
    CAN2_ResvData(&CAN2_RX);	           //������ա��������� 
    memcpy(Can2_recv,rev,sizeof(rev));     //������ȫ�ֱ���   
    CAN_ClrStatus(APP_CAN2_UNIT, m_u32StatusVal);
}


/**********************************************************************************************************
**	�� �� �� : CAN1_SendData
**	����˵�� : CAN���߷������ݺ���
**	��    �� : *CANx - CAN1/CAN2��ID - ID�ţ�*pData - ��������
**	�� �� ֵ : enRet - ����״̬@ref en_result_t 1 �ɹ� 0 ʧ��
**********************************************************************************************************/
uint8_t CAN1_SendData(uint32_t ID, uint8_t *pData , uint16_t len)
{
    uint8_t enTxResult=1;
    stc_can_tx_t stcTx = {0};
    uint8_t Err_CNT=0;                //���η���ʧ�����Դ���

    /* �����Ҫ���͵����� */
    stcTx.pu8Data = pData; //������ݣ�����8�ֽ�
    stcTx.u32Ctrl = 0U;    //���ƶ�������0
    stcTx.DLC = len;       //���ݳ���
    stcTx.u32ID = ID;      //���ID
    stcTx.IDE = 1U;        //��չ֡

    /* �����������ʧ������3�� */
    while ((Ok != CAN_TransData(APP_CAN1_UNIT, &stcTx, CAN_BUF_STB, CAN_STB_TRANS_ALL, 0U)) && (Err_CNT < 4))
    {
        Err_CNT++;
    }
    if(Err_CNT==4)
    {
        enTxResult=0;       
    }
    return enTxResult;
}

/**********************************************************************************************************
**	�� �� �� : CAN2_SendData
**	����˵�� : CAN���߷������ݺ���
**	��    �� : *CANx - CAN1/CAN2��ID - ID�ţ�*pData - �������ݣ�Len - ���ݳ���
**	�� �� ֵ : enRet - ����״̬@ref en_result_t 1 �ɹ� 0 ʧ��
**********************************************************************************************************/
uint8_t CAN2_SendData(uint32_t ID, uint8_t *pData, uint16_t len)
{
    uint8_t enTxResult=1;
    stc_can_tx_t stcTx = {0};
    uint16_t Num=0;                //���ʹ���

    Num=len/8;
    uint8_t Err_CNT=0;                //���η���ʧ�����Դ���

    /*��ֺ�8�ֽ���������*/
    for(uint16_t i=0; i<Num; i++)
    {
        /* �����Ҫ���͵����� */
        stcTx.pu8Data = pData+i*8; //������ݣ�����8�ֽ�
        stcTx.u32Ctrl = 0U;    //���ƶ�������0
        stcTx.DLC = 8;         //���ݳ���8
        stcTx.u32ID = ID;      //���ID
        stcTx.IDE = 1U;        //��չ֡

        /* �����������ʧ������3�� */
        while ((Ok != CAN_TransData(APP_CAN2_UNIT, &stcTx, CAN_BUF_STB, CAN_STB_TRANS_ALL, 0U)) && (Err_CNT < 4))
        {
            Err_CNT++;
        }
        if(Err_CNT==4)
        {
            enTxResult=0;
            return enTxResult;
        }
        Err_CNT=0;
    }

    /*��ֺ�С��8�ֽ�β������*/
    /* �����Ҫ���͵����� */
    stcTx.pu8Data = pData+Num*8; //������ݣ�����=β��ʣ�೤��
    stcTx.u32Ctrl = 0U;          //���ƶ�������0
    stcTx.DLC = len%8;           //β��ʣ�೤��
    stcTx.u32ID = ID;            //���ID
    stcTx.IDE = 1U;              //��չ֡

    /* �����������ʧ������3�� */
    while ((Ok != CAN_TransData(APP_CAN2_UNIT, &stcTx, CAN_BUF_STB, CAN_STB_TRANS_ALL, 0U)) && (Err_CNT < 4))
    {
        Err_CNT++;
    }
    if(Err_CNT==4)
    {
        enTxResult=0;
    }
    Err_CNT=0;

    return enTxResult;
}

/**********************************************************************************************************
**	�� �� �� : CAN1_ResvData
**	����˵�� : CAN���߽������ݺ���
**	��    �� : stcRx �������ݽ����ṹ��
**	�� �� ֵ : 
**********************************************************************************************************/
void CAN1_ResvData( stc_can_rx_t* stcRx )
{
    CAN_ReceiveData(APP_CAN1_UNIT, &stcRx[0U], NULL, MSG_MAX_NUM);    
}


/**********************************************************************************************************
**	�� �� �� : CAN2_ResvData
**	����˵�� : CAN���߽������ݺ���
**	��    �� : stcRx �������ݽ����ṹ��
**	�� �� ֵ : 
**********************************************************************************************************/
void CAN2_ResvData( stc_can_rx_t* stcRx )
{
    CAN_ReceiveData(APP_CAN2_UNIT, &stcRx[0U], NULL, MSG_MAX_NUM);
}


#if 0
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
