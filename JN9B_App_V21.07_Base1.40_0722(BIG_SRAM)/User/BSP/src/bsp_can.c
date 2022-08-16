#include "bsp_can.h"

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
#if (DDL_PRINT_ENABLE == DDL_ON)
const static char *m_s8IDTypeStr[] = {
    "standard",
    "extended",
};

const static char *m_s8FrameTypeStr[] = {
    "normal",
    "CAN-FD",
};

const static char *m_s8ErrorTypeStr[] = {
    "no error.",
    "Bit Error.",
    "Form Error.",
    "Stuff Error.",
    "ACK Error.",
    "CRC Error.",
    "Other Error.",
    "Value of error type is invalid.",
};
#endif

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

/* One second timing flag. */
static uint8_t m_u8TmrFlag = 0U;

static void CanConfig(void);
static void CanTx(void);
static void CanTxEntity(const stc_can_tx_t *pstcTx);
static void CanRx(void);


/**
 * @brief  CAN configuration.
 * @param  None
 * @retval None
 */
static void CanConfig(void)
{
    stc_can_init_t stcInit;
    stc_can_af_cfg_t astcAFCfg[] = { \
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
    stcInit.u8WorkMode   = APP_CAN_MODE;
    stcInit.pstcAFCfg    = astcAFCfg;
    stcInit.u8RBOvfOp    = CAN_RB_OVF_SAVE_NEW;
    stcInit.u16AFSel     = APP_CAN_AF_SEL;
#if (APP_CAN_MODE == CAN_MODE_ELB)
    stcInit.u8SelfACKCmd = CAN_SELF_ACK_ENABLE;
#else
    stcInit.u8SelfACKCmd = CAN_SELF_ACK_DISABLE;
#endif
    stcInit.stcSBT.u32SEG1 = 1U + 14U;
    stcInit.stcSBT.u32SEG2 = 5U;
    stcInit.stcSBT.u32SJW  = 5U;
    stcInit.stcSBT.u32Prescaler = 4U;

#if (APP_CAN_FD_ENABLE > 0U)
    stcInit.stcFDCfg.u8TDCSSP = 4U;
    stcInit.stcFDCfg.u8CANFDMode = CAN_FD_MODE_BOSCH;
    stcInit.stcFDCfg.stcFBT.u32SEG1 = 8U;
    stcInit.stcFDCfg.stcFBT.u32SEG2 = 2U;
    stcInit.stcFDCfg.stcFBT.u32SJW  = 2U;
    stcInit.stcFDCfg.stcFBT.u32Prescaler = 1U;
#endif

    PWC_Fcg1PeriphClockCmd(APP_CAN_PERIP_CLK, Enable);
    (void)CAN_Init(APP_CAN_UNIT, &stcInit);
}

/**
 * @brief  CAN transmits data.
 * @param  None
 * @retval None
 */
static void CanTx(void)
{
    uint8_t i;
    stc_can_tx_t stcTx;

    for (i=0U; i<APP_DATA_SIZE; i++)
    {
        m_au8TxPayload[i] = i + 1U;
    }

    stcTx.u32Ctrl = 0x0U;
    stcTx.pu8Data = (uint8_t *)&m_au8TxPayload[0U];

    /* Configures the frame with ID 'APP_ID_1'. */
#if (APP_CAN_FD_ENABLE > 0U)
    stcTx.FDF = 1U;
    stcTx.BRS = 1U;
    stcTx.DLC = CAN_DLC_64;
#else
    stcTx.DLC = CAN_DLC_8;
#endif
    stcTx.u32ID = APP_ID_1;
    stcTx.IDE   = APP_ID_1_IDE;
    CanTxEntity(&stcTx);

    /* Configures the frame with ID 'APP_ID_2'. */
    stcTx.u32Ctrl = 0x0U;
    stcTx.u32ID   = APP_ID_2;
    stcTx.IDE     = APP_ID_2_IDE;
    stcTx.DLC     = CAN_DLC_7;
    CanTxEntity(&stcTx);

    /* Configures the frame with ID 'APP_ID_3'. */
    stcTx.u32Ctrl = 0x0U;
    stcTx.u32ID   = APP_ID_3;
    stcTx.IDE     = APP_ID_3_IDE_ENABLE;
    stcTx.DLC     = CAN_DLC_3;
    CanTxEntity(&stcTx);

    stcTx.u32Ctrl = 0x0U;
#if (APP_CAN_FD_ENABLE > 0U)
    stcTx.FDF = 1U;
    stcTx.BRS = 1U;
    stcTx.DLC = CAN_DLC_32;
#else
    stcTx.DLC = CAN_DLC_3;
#endif
    stcTx.u32ID = APP_ID_3;
    stcTx.IDE   = APP_ID_3_IDE_DISABLE;
    CanTxEntity(&stcTx);
}

/**
 * @brief  CAN transmission entity.
 * @param  [in]  pstcTx                 Pointer to a frame which is going to be transmitted.
 * @retval None
 */
static void CanTxEntity(const stc_can_tx_t *pstcTx)
{
    uint8_t u8Tmp;
    uint8_t u8ErrorType;
    uint32_t u32StatusVal;
    en_result_t enTxResult;

    enTxResult = CAN_TransData(APP_CAN_UNIT, pstcTx,
                               CAN_BUF_STB, CAN_STB_TRANS_ONE, 1000U);
    u32StatusVal = CAN_GetStatusVal(APP_CAN_UNIT);
    if (enTxResult == Ok)
    {
        DBG("CAN transmitted %s frame with %s ID: %.8x   Status value: %.8x\n", \
            m_s8FrameTypeStr[pstcTx->FDF], \
            m_s8IDTypeStr[pstcTx->IDE],    \
            (unsigned int)pstcTx->u32ID,   \
            (unsigned int)u32StatusVal);
    }
    else
    {
        u8ErrorType  = CAN_GetErrType(APP_CAN_UNIT);
        u8ErrorType >>= 5U;
        DBG("---> CAN error type: %u, %s\n", u8ErrorType, m_s8ErrorTypeStr[u8ErrorType]);
        DBG("CAN transmission error. Error status flags %.8x\n", (unsigned int)u32StatusVal);

        if ((u32StatusVal & CAN_FLAG_BUS_OFF) != 0UL)
        {
            DBG("BUS OFF!!!");
        }
        if ((u32StatusVal & CAN_FLAG_BUS_TX) != 0UL)
        {
            DBG("The transmission is still executing.\n");
            u8Tmp = CAN_GetTBType(APP_CAN_UNIT);
            if ((u8Tmp == CAN_BUF_PTB) || (u8Tmp == CAN_BUF_STB))
            {
                CAN_AbortTrans(APP_CAN_UNIT, u8Tmp);
            }
        }
        if ((u32StatusVal & CAN_FLAG_TB_FULL) != 0UL)
        {
            DBG("TX buffers are full.\n");
            u8Tmp = CAN_GetTBType(APP_CAN_UNIT);
            if ((u8Tmp == CAN_BUF_PTB) || (u8Tmp == CAN_BUF_STB))
            {
                CAN_AbortTrans(APP_CAN_UNIT, u8Tmp);
            }
        }
        if ((u32StatusVal & CAN_FLAG_ERR_INT) != 0UL)
        {
            DBG("Error interrupt.\n");
        }
        if ((u32StatusVal & CAN_FLAG_BUS_ERR) != 0UL)
        {
            DBG("Bus error.\n");
        }
        if ((u32StatusVal & CAN_FLAG_ARB_LOST) != 0UL)
        {
            DBG("Arbitration lost.\n");
        }
        if ((u32StatusVal & CAN_FLAG_ERR_PASSIVE) != 0UL)
        {
            DBG("Error passive.\n");
        }
        if ((u32StatusVal & CAN_FLAG_ERR_PASSIVE_NODE) != 0UL)
        {
            DBG("The node is an error passive node.\n");
        }
        if ((u32StatusVal & CAN_FLAG_REACH_WARN_LIMIT) != 0UL)
        {
            DBG("TEC or REC reached warning limit.\n");
        }

        CAN_ClrStatus(APP_CAN_UNIT, u32StatusVal);
    }
}

/**
 * @brief  CAN receives data.
 * @param  None
 * @retval None
 */
static void CanRx(void)
{
    uint8_t u8RxCnt;
    uint8_t u8DataIdx;
    uint8_t u8DataSize;
    uint8_t u8RxFrameCnt;
    stc_can_rx_t stcRx[3U];

    stcRx[0U].pu8Data = (uint8_t *)&m_au8RxPayload1[0U];
    stcRx[1U].pu8Data = (uint8_t *)&m_au8RxPayload2[0U];
    stcRx[2U].pu8Data = (uint8_t *)&m_au8RxPayload3[0U];

    (void)CAN_ReceiveData(APP_CAN_UNIT, &stcRx[0U], &u8RxFrameCnt, 3U);
    for (u8RxCnt=0U; u8RxCnt<u8RxFrameCnt; u8RxCnt++)
    {
        DBG("CAN received %s frame with %s ID %.8x:\n", \
            m_s8FrameTypeStr[stcRx[u8RxCnt].FDF], \
            m_s8IDTypeStr[stcRx[u8RxCnt].IDE],    \
            (unsigned int)stcRx[u8RxCnt].u32ID);
        u8DataSize = m_au8DLC2Size[stcRx[u8RxCnt].FDF][stcRx[u8RxCnt].DLC];
        for (u8DataIdx=0; u8DataIdx<u8DataSize; u8DataIdx++)
        {
            DBG(" %.2x.", stcRx[u8RxCnt].pu8Data[u8DataIdx]);
            stcRx[u8RxCnt].pu8Data[u8DataIdx] = 0U;
        }
        DBG("\n");
    }
}

