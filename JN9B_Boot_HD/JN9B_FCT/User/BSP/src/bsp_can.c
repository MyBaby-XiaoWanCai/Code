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
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"


extern  uint8_t  Can1_recv[4];                //CAN1接收数据
extern  uint8_t  Can2_recv[4];                //CAN2接收数据


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void Can1IrqConfig(void);
static void Can2IrqConfig(void);
static void CAN1_IrqCallback(void);
static void CAN2_IrqCallback(void);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/**********************************************************************************************************
**	函 数 名 : bsp_InitCAN1
**	功能说明 : ACDC模块初始化配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitCAN1(void)
{
	Can1Config();      //配置CAN1寄存器
}

/**********************************************************************************************************
**	函 数 名 : bsp_InitCAN2
**	功能说明 : ACDC模块初始化配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitCAN2(void)
{
	Can2Config();     //配置CAN2寄存器
}


/**********************************************************************************************************
**	函 数 名 : Can1Config
**	功能说明 : CAN1初始化配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：CAN时钟：240MHz / 6 = 40Mhz，CAN波特率：40MHz / Prescaler / (SEG1+SEG2) = 125Kbps
**********************************************************************************************************/
void Can1Config(void)
{
    stc_can_init_t stcInit;

    /* 接收筛选器配置 */
#ifdef FCT_TEST
    /*FCT测试要求CAN１CAN２互相收发，因此滤波配置要保持一致*/
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
    stcInit.u8RBSWarnLimit = MSG_MAX_NUM;         //设定RingBuffer将满中断最大缓存为3，Range[1,8]
    stcInit.pstcAFCfg = astcAFCfg;
    stcInit.u8RBOvfOp = CAN_RB_OVF_SAVE_NEW;      //接收溢出覆盖
#ifdef FCT_TEST    
    stcInit.u16AFSel  = CAN_AF1|CAN_AF2;                  //选择接收滤波器1，2
#else
    stcInit.u16AFSel  = CAN_AF1|CAN_AF2|CAN_AF3;          //选择接收滤波器1，2，3
#endif
#ifdef FCT_TEST
    /*FCT测试要求CAN１CAN２互相收发，因此通信速率配置要保持一致*/
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
**	函 数 名 : Can2Config
**	功能说明 : CAN2初始化配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：CAN时钟：240MHz / 6 = 40Mhz，CAN波特率：40MHz / Prescaler / (SEG1+SEG2) = 500Kbps
**********************************************************************************************************/
void Can2Config(void)
{
    stc_can_init_t stcInit;

    /* 接收筛选器配置 */
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
    stcInit.u8RBSWarnLimit = MSG_MAX_NUM;         //设定RingBuffer将满中断最大缓存为3，Range[1,8]
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
**	函 数 名 : Can1IrqConfig
**	功能说明 : CAN1中断配置
**	形    参 : 无
**	返 回 值 : 无
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
**	函 数 名 : Can2IrqConfig
**	功能说明 : CAN2中断配置
**	形    参 : 无
**	返 回 值 : 无
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
**	函 数 名 : CAN1_IrqCallback
**	功能说明 : CAN1中断回调函数配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void CAN1_IrqCallback(void)
{
    stc_can_rx_t CAN1_RX;
    uint8_t rev[4]={0};  //数据缓存区
    uint8_t m_u8ErrType = 0;
    uint32_t m_u32StatusVal = 0;
    
    m_u32StatusVal = CAN_GetStatusVal(APP_CAN1_UNIT);
    m_u8ErrType    = CAN_GetErrType(APP_CAN1_UNIT);
    CAN1_RX.pu8Data = (uint8_t *)&rev[0]; //接收数据到缓存
    CAN1_ResvData(&CAN1_RX);	          //加入接收、解析代码
    memcpy(Can1_recv,rev,sizeof(rev));    //拷贝到全局变量
    CAN_ClrStatus(APP_CAN1_UNIT, m_u32StatusVal);
}

/**********************************************************************************************************
**	函 数 名 : CAN2_IrqCallback
**	功能说明 : CAN2中断回调函数配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void CAN2_IrqCallback(void)
{
    stc_can_rx_t CAN2_RX;
    uint8_t rev[4]={0};  //数据缓存区
    uint8_t m_u8ErrType = 0;
    uint32_t m_u32StatusVal = 0;
    m_u32StatusVal = CAN_GetStatusVal(APP_CAN2_UNIT);
    m_u8ErrType    = CAN_GetErrType(APP_CAN2_UNIT);
    CAN2_RX.pu8Data = (uint8_t *)&rev[0];  //接收数据到缓存
    CAN2_ResvData(&CAN2_RX);	           //加入接收、解析代码 
    memcpy(Can2_recv,rev,sizeof(rev));     //拷贝到全局变量   
    CAN_ClrStatus(APP_CAN2_UNIT, m_u32StatusVal);
}


/**********************************************************************************************************
**	函 数 名 : CAN1_SendData
**	功能说明 : CAN总线发送数据函数
**	形    参 : *CANx - CAN1/CAN2，ID - ID号，*pData - 发送数据
**	返 回 值 : enRet - 发送状态@ref en_result_t 1 成功 0 失败
**********************************************************************************************************/
uint8_t CAN1_SendData(uint32_t ID, uint8_t *pData , uint16_t len)
{
    uint8_t enTxResult=1;
    stc_can_tx_t stcTx = {0};
    uint8_t Err_CNT=0;                //单次发送失败重试次数

    /* 填充需要发送的数据 */
    stcTx.pu8Data = pData; //填充数据，长度8字节
    stcTx.u32Ctrl = 0U;    //控制段配置清0
    stcTx.DLC = len;       //数据长度
    stcTx.u32ID = ID;      //填充ID
    stcTx.IDE = 1U;        //扩展帧

    /* 单包发送最多失败重试3次 */
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
**	函 数 名 : CAN2_SendData
**	功能说明 : CAN总线发送数据函数
**	形    参 : *CANx - CAN1/CAN2，ID - ID号，*pData - 发送数据，Len - 数据长度
**	返 回 值 : enRet - 发送状态@ref en_result_t 1 成功 0 失败
**********************************************************************************************************/
uint8_t CAN2_SendData(uint32_t ID, uint8_t *pData, uint16_t len)
{
    uint8_t enTxResult=1;
    stc_can_tx_t stcTx = {0};
    uint16_t Num=0;                //发送次数

    Num=len/8;
    uint8_t Err_CNT=0;                //单次发送失败重试次数

    /*拆分后8字节整包发送*/
    for(uint16_t i=0; i<Num; i++)
    {
        /* 填充需要发送的数据 */
        stcTx.pu8Data = pData+i*8; //填充数据，长度8字节
        stcTx.u32Ctrl = 0U;    //控制段配置清0
        stcTx.DLC = 8;         //数据长度8
        stcTx.u32ID = ID;      //填充ID
        stcTx.IDE = 1U;        //扩展帧

        /* 单包发送最多失败重试3次 */
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

    /*拆分后小于8字节尾包发送*/
    /* 填充需要发送的数据 */
    stcTx.pu8Data = pData+Num*8; //填充数据，长度=尾包剩余长度
    stcTx.u32Ctrl = 0U;          //控制段配置清0
    stcTx.DLC = len%8;           //尾包剩余长度
    stcTx.u32ID = ID;            //填充ID
    stcTx.IDE = 1U;              //扩展帧

    /* 单包发送最多失败重试3次 */
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
**	函 数 名 : CAN1_ResvData
**	功能说明 : CAN总线接收数据函数
**	形    参 : stcRx 接收数据解析结构体
**	返 回 值 : 
**********************************************************************************************************/
void CAN1_ResvData( stc_can_rx_t* stcRx )
{
    CAN_ReceiveData(APP_CAN1_UNIT, &stcRx[0U], NULL, MSG_MAX_NUM);    
}


/**********************************************************************************************************
**	函 数 名 : CAN2_ResvData
**	功能说明 : CAN总线接收数据函数
**	形    参 : stcRx 接收数据解析结构体
**	返 回 值 : 
**********************************************************************************************************/
void CAN2_ResvData( stc_can_rx_t* stcRx )
{
    CAN_ReceiveData(APP_CAN2_UNIT, &stcRx[0U], NULL, MSG_MAX_NUM);
}


#if 0
/**********************************************************************************************************
**	函 数 名 : CanDebugInfPrint
**	功能说明 : CAN1调试信息打印输出
**	形    参 : 无
**	返 回 值 : 无
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
