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
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t m_u8ErrType = 0;                //CAN错误类型
uint32_t m_u32StatusVal = 0;            //CAN中断标志位
ACDC_UpdateStrcut CanRet = {0, {0}, 0}; //ACDC升级can回复结构体
uint8_t AACV_FLAG=0;                    //AC故障电压更新标志
uint8_t ADCV_FLAG=0;                    //AC故障电压更新标志
extern  __IO uint8_t ACDC_UpdateFlg;                      //ACDC升级标志位
extern osSemaphoreId_t acdc_alarm_Semaphore;        //ACDC告警信号量ID

#if 0 //暂未用到
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

static uint8_t CanRxBuffer[MSG_MAX_NUM][APP_DATA_SIZE];       //CAN接收缓冲

ACDC_Inf ACDC= {1,{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}; //定义ACDC结构体，默认国电ACDC，参数初始化为0

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void Can1Config(void);
static void Can1IrqConfig(void);
static void CAN1_IrqCallback(void);
static void ACDC_DataHandle(stc_can_rx_t *pCanRx, uint8_t u8RxCnt);
static void CANx_ResvData(M4_CAN_TypeDef *CANx);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : bsp_InitACDC
**	功能说明 : ACDC模块初始化配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitACDC(void)
{
    Can1Config();
}

/**********************************************************************************************************
**	函 数 名 : CANx_SendData
**	功能说明 : CAN总线发送数据函数
**	形    参 : *CANx - CAN1/CAN2，ID - ID号，*pData - 发送数据，Len - 数据长度
**	返 回 值 : enRet - 发送状态@ref en_result_t
**********************************************************************************************************/
en_result_t CANx_SendData(M4_CAN_TypeDef *CANx,  uint32_t ID, uint8_t *pData, uint16_t Len)
{
    uint8_t TimeOut = 0;
    en_result_t enTxResult = Ok;
    stc_can_tx_t stcTx = {0};
    uint8_t Err_CNT=0;                //单次发送失败重试次数

    /* 填充需要发送的数据 */
    stcTx.pu8Data = pData;
    stcTx.u32Ctrl = 0U;
    stcTx.DLC = Len;
    stcTx.u32ID = ID;
    stcTx.IDE = 1U;

    if(Set==CAN_GetStatus(CANx,CAN_FLAG_BUS_OFF))
    {
        heart_beat_flag = 2;
        CAN_AbortTrans(CANx,CAN_BUF_STB);
        bsp_DelayUS(2000);
        CAN_SWReset(CANx);
        bsp_DelayUS(2000);
        enTxResult=0;
        return enTxResult;
    }

    while((Set==CAN_GetStatus(CANx,CAN_FLAG_TB_FULL))&&TimeOut<10)  //最大等1MS 等TB空闲
    {
        TimeOut++;
        bsp_DelayUS(100);
        Feed_WDG();
    }
    TimeOut=0;

    /* 单包发送最多失败重试3次 */
    while ((Ok != CAN_TransData(CANx, &stcTx, CAN_BUF_STB, CAN_STB_TRANS_ALL, 0U)) && (Err_CNT < 4))
    {
        Err_CNT++;

        while((Set==CAN_GetStatus(CANx,CAN_FLAG_TB_FULL))&&TimeOut<10)
        {
            TimeOut++;
            bsp_DelayUS(100);
            Feed_WDG();
        }
        TimeOut=0;
        bsp_DelayUS(5);
    }

    while((Set==CAN_GetStatus(CANx,CAN_FLAG_BUS_TX))&&TimeOut<10)
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
		for(uint8_t j = 0; j < CHANNEL_CFG; j++)
		{
			IP_Alarm_Flag[j] |= IntelProtect_C1E;	//can1故障
		}		
    }
    Err_CNT=0;

    return enTxResult;
}

/**********************************************************************************************************
**	函 数 名 : CAN1_SendData
**	功能说明 : CAN总线发送数据函数
**	形    参 : *CANx - CAN1/CAN2，ID - ID号，*pData - 发送数据，Len - 数据长度
**	返 回 值 : enRet - 发送状态@ref en_result_t 1 成功 0 失败
**********************************************************************************************************/
uint8_t CAN1_SendData(uint32_t ID, uint8_t *pData, uint16_t len)
{
    uint8_t TimeOut = 0;
    uint8_t enTxResult=1;
    stc_can_tx_t stcTx = {0};
    uint8_t Err_CNT=0;                //单次发送失败重试次数
    /*拆分后小于8字节尾包发送*/
    /* 填充需要发送的数据 */
    stcTx.pu8Data = pData; //填充数据，长度=尾包剩余长度
    stcTx.u32Ctrl = 0U;          //控制段配置清0
    stcTx.DLC = len;             //包长度
    stcTx.u32ID = ID;            //填充ID
    stcTx.IDE = 1U;              //扩展帧

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
    /* 单包发送最多失败重试3次 */
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
//    uint8_t Err_CNT=0;                //单次发送失败重试次数
//    /*拆分后小于8字节尾包发送*/
//    /* 填充需要发送的数据 */
//    stcTx.pu8Data = pData; //填充数据，长度=尾包剩余长度
//    stcTx.u32Ctrl = 0U;          //控制段配置清0
//    stcTx.DLC = len;             //包长度
//    stcTx.u32ID = ID;            //填充ID
//    stcTx.IDE = 1U;              //扩展帧

//    while(Set==CAN_GetStatus(APP_CAN1_UNIT,CAN_FLAG_TB_FULL));

//    /* 单包发送最多失败重试3次 */
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
**	函 数 名 : CAN2_SendData
**	功能说明 : CAN总线发送数据函数
**	形    参 : *CANx - CAN1/CAN2，ID - ID号，*pData - 发送数据，Len - 数据长度<=8
**	返 回 值 : enRet - 发送状态@ref en_result_t 1 成功 0 失败
**********************************************************************************************************/
uint8_t CAN2_SendData(uint32_t ID, uint8_t *pData, uint16_t len)
{
    uint8_t enTxResult=1;
    stc_can_tx_t stcTx = {0};
    uint8_t Err_CNT=0;                //单次发送失败重试次数
    uint8_t TimeOut=0;
    /*拆分后小于8字节尾包发送*/
    /* 填充需要发送的数据 */
    stcTx.pu8Data = pData;            //填充数据，长度=尾包剩余长度
    stcTx.u32Ctrl = 0U;               //控制段配置清0
    if(len<=8)                        //确保长度不超过8字节，否则最多发8字节
    {
        stcTx.DLC = len;                  //包长度
    }
    else
    {
        stcTx.DLC=8;
    }
    stcTx.u32ID = ID;                 //填充ID
    stcTx.IDE = 1U;                   //扩展帧

    while((Set==CAN_GetStatus(APP_CAN2_UNIT,CAN_FLAG_TB_FULL))&&(TimeOut<10))        //查询TB满标志，超时10次跳出
    {
        TimeOut++;
        bsp_DelayUS(100);
    };
    if(TimeOut==10)
    {
        CAN_AbortTrans(APP_CAN2_UNIT,CAN_BUF_STB);                                    //发送buf满，取消STB发送
        CAN_ClrStatus(APP_CAN2_UNIT,CAN_FLAG_TB_FULL);                                //清除TB_FULL标志
        bsp_DelayUS(100);
        TimeOut=0;
    }
    /* 单包发送最多失败重试3次 */
    while ((Ok != CAN_TransData(APP_CAN2_UNIT, &stcTx, CAN_BUF_STB, CAN_STB_TRANS_ALL, 0U)) && (Err_CNT < 4))
    {
        CAN_AbortTrans(APP_CAN2_UNIT,CAN_BUF_STB);                                    //发送buf满，取消STB发送
        CAN_ClrStatus(APP_CAN2_UNIT,CAN_FLAG_TB_FULL);                                //清除TB_FULL标志
        Err_CNT++;

        while((Set==CAN_GetStatus(APP_CAN2_UNIT,CAN_FLAG_TB_FULL))&&(TimeOut<10))        //查询TB满标志，超时10次跳出
        {
            TimeOut++;
            bsp_DelayUS(100);
        };
        if(TimeOut==10)
        {
            CAN_AbortTrans(APP_CAN2_UNIT,CAN_BUF_STB);                                    //发送buf满，取消STB发送
            CAN_ClrStatus(APP_CAN2_UNIT,CAN_FLAG_TB_FULL);                                //清除TB_FULL标志
            bsp_DelayUS(100);
            TimeOut=0;
        }
        bsp_DelayUS(5);
    }

    while(Set==CAN_GetStatus(APP_CAN2_UNIT,CAN_FLAG_BUS_TX)) {};                          //等待发送完成

    bsp_DelayUS(5);

    if(Err_CNT==4)
    {
        for(uint8_t j = 0; j < CHANNEL_CFG; j++)
        {
            IP_Alarm_Flag[j] |= IntelProtect_C2E; //can2故障
        }
        enTxResult=0;
    }
    Err_CNT=0;

    return enTxResult;
}

/**********************************************************************************************************
**	函 数 名 : ACDC_DataHandle()
**	功能说明 : ACDC数据处理函数
**	形    参 : *pCanRx - 接收数据，u8RxCnt - 消息帧数
**	返 回 值 : 无
**********************************************************************************************************/
static void ACDC_DataHandle(stc_can_rx_t *pCanRx, uint8_t u8RxCnt)
{
    uint8_t i = 0;
    uint8_t AcdcIndex = 0;

    if(ACDC_UpdateFlg == 1)			/*升级模式，回复处理*/
    {
        if((pCanRx[u8RxCnt].u32ID&0X1F000000) == 0X10500000)		//国电
        {
            uint32_t TempId = 0;

            //拷贝数据到结构体，结构体发消息队列
            TempId = (pCanRx[u8RxCnt].u32ID>>24&0xFF)+((pCanRx[u8RxCnt].u32ID>>16&0xFF)<<8)+  \
                     ((pCanRx[u8RxCnt].u32ID>>8&0xFF)<<16) +((pCanRx[u8RxCnt].u32ID&0xFF)<<24); //ID大小端转换
            CanRet.ID = TempId;
            memcpy(CanRet.Date, pCanRx[u8RxCnt].pu8Data, 8);
            CanRet.processedFlg = 1;    //1表示收到升级回复数据待处理
        }
        else if((pCanRx[u8RxCnt].u32ID&0X1F000000) == 0X1c000000)
        {
            uint32_t TempId = 0;

            //拷贝数据到结构体，结构体发消息队列
            TempId = (pCanRx[u8RxCnt].u32ID>>24&0xFF)+((pCanRx[u8RxCnt].u32ID>>16&0xFF)<<8)+  \
                     ((pCanRx[u8RxCnt].u32ID>>8&0xFF)<<16) +((pCanRx[u8RxCnt].u32ID&0xFF)<<24); //ID大小端转换
            CanRet.ID = TempId;
            memcpy(CanRet.Date, pCanRx[u8RxCnt].pu8Data, 8);
            CanRet.processedFlg = 1;    //1表示收到升级回复数据待处理
        }
        else
        {
            //do nothing
        }
    }
    else if(ACDC_UpdateFlg == 0)	/*非升级模式，回复处理*/
    {
        if((pCanRx[u8RxCnt].u32ID & 0x1FFF0000) == 0x102B0000)
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
            case 0x37: //风扇转速更新
            {
                ACDC.FanSpd[AcdcIndex] = pCanRx[u8RxCnt].pu8Data[7];
                break;
            }
            case 0x53: //ACDC直流侧电压更新
            {
                ACDC.DCV[AcdcIndex] = ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 10.0;
                break;
            }
            case 0x54: //ACDC直流侧电流更新
                ACDC.DCI[AcdcIndex] = ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 10.0;
                break;
            case 0x59://模块主动报警处理
            {
                /*如果报警状态无变化，则不更新ACDC状态*/
                if(getSn_SR(SOCK_0_NUMB) == SOCK_ESTABLISHED)
                {
                    if(ACDC.State[AcdcIndex] != ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]))
                    {
                        ACDC.BitFlg[AcdcIndex] = ACDC.State[AcdcIndex] ^ ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]); //记录状态改变的bit
                        ACDC.State[AcdcIndex] = ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]);  //更新state字段
                        osSemaphoreRelease(acdc_alarm_Semaphore);    //释放信号量，同步报警任务
                    }
                }
                break;
            }
            case 0x62://ACDC交流侧电压更新
            {
                ACDC.ACV[AcdcIndex] = ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 10.0;
                if(i==0)
                    ACDC.QueryWaitFlag=QueryACV1;        //更新等待标志
                else if(i==1)
                    ACDC.QueryWaitFlag=QueryACV2;        //更新等待标志
                break;
            }
            case 0x63://ACDC交流侧电流更新
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
                case 0x01://ACDC直流侧电压更新
                {
                    ACDC.DCV[AcdcIndex] = (int)((uint16_t)(pCanRx[u8RxCnt].pu8Data[4] << 24) + (uint16_t)(pCanRx[u8RxCnt].pu8Data[5] << 16) +  \
                                                (uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 1024.0;
                    break;
                }
                case 0x02://ACDC直流侧电流更新
                {
                    ACDC.DCI[AcdcIndex] = (int)((uint16_t)(pCanRx[u8RxCnt].pu8Data[4] << 24) + (uint16_t)(pCanRx[u8RxCnt].pu8Data[5] << 16) +  \
                                                (uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 1024.0;
                    break;
                }
                case 0x43://ACDC工作状态，解析详见协议
                {
                    /*如果报警状态无变化，则不更新ACDC状态*/
                    if(ACDC.State[AcdcIndex] != ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]))
                    {
                        ACDC.BitFlg[AcdcIndex] = ACDC.State[AcdcIndex] ^ ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]); //记录状态改变的bit
                        ACDC.State[AcdcIndex] = ((uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]);   //更新状态
                        osSemaphoreRelease(acdc_alarm_Semaphore);     //释放信号量，同步报警任务
                    }
                    break;
                }
//                 case 0x44://ACDC故障时AC电压更新
//                {
//                    ACDC.AACV[AcdcIndex]=(int)((uint16_t)(pCanRx[u8RxCnt].pu8Data[4] << 8) + pCanRx[u8RxCnt].pu8Data[5])/10.0f;
//                    AACV_FLAG=1;
//                    break;
//                }
//                case 0x45://ACDC故障时DC电压更新
//                {
//                    ACDC.ADCV[AcdcIndex]=(int)((uint16_t)(pCanRx[u8RxCnt].pu8Data[4] << 8) + pCanRx[u8RxCnt].pu8Data[5])/100.0f;
//                    ADCV_FLAG=1;
//                    break;
//                }
                case 0x05://ACDC交流侧电压更新
                {
                    ACDC.ACV[AcdcIndex] = (int)((uint16_t)(pCanRx[u8RxCnt].pu8Data[4] << 24) + (uint16_t)(pCanRx[u8RxCnt].pu8Data[5] << 16) +  \
                                                (uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 1024.0;
                    if(i==0)
                        ACDC.QueryWaitFlag=QueryACV1;        //更新等待标志
                    else if(i==1)
                        ACDC.QueryWaitFlag=QueryACV2;        //更新等待标志
                    break;
                }
                case 0x06://ACDC交流侧电流更新
                {
                    ACDC.ACI[AcdcIndex] = (int)((uint16_t)(pCanRx[u8RxCnt].pu8Data[4] << 24) + (uint16_t)(pCanRx[u8RxCnt].pu8Data[5] << 16) +  \
                                                (uint16_t)(pCanRx[u8RxCnt].pu8Data[6] << 8) + pCanRx[u8RxCnt].pu8Data[7]) / 1024.0;
                    break;
                }
                case 0xBE://ACDC固件版本查询更新
                {
                    for(uint8_t m=0; m<4; m++)
                        ACDC.VERSION[i][m]= pCanRx[u8RxCnt].pu8Data[4+m];
                    if(i==0)
                        ACDC.QueryWaitFlag=QueryVERSION1;        //更新等待标志
                    else if(i==1)
                        ACDC.QueryWaitFlag=QueryVERSION2;        //更新等待标志
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
                ACDC.FanSpd[AcdcIndex] = pCanRx[u8RxCnt].pu8Data[7];   //更新风扇转速
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
**	函 数 名 : CANx_ResvData
**	功能说明 : CAN总线接收数据函数
**	形    参 : *CANx - CAN1/CAN2
**	返 回 值 : enRet - 发送状态@ref en_result_t
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

    //由于当前设备只有一种，MSG_MAX_NUM设置为1，可根据实际情况来设置最大可设置8
    (void)CAN_ReceiveData(APP_CAN1_UNIT, &stcRx[0U], &u8RxFrameCnt, MSG_MAX_NUM);
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

///**********************************************************************************************************
//**	函 数 名 : CAN1_ResvData
//**	功能说明 : CAN总线接收数据函数
//**	形    参 : stcRx 接收数据解析结构体
//**	返 回 值 :
//**********************************************************************************************************/
//void CAN1_ResvData(void)
//{
//    stc_can_rx_t stcRx;

//    uint8_t rev[8]= {0}; //数据缓存区

//    stcRx.pu8Data = rev;

//    while (READ_REG8_BIT(M4_CAN1->RCTRL, CAN_RCTRL_RSTAT) != CAN_RB_STAT_EMPTY)
//    {
//        CAN_ReceiveData(APP_CAN1_UNIT, &stcRx, NULL, 1);
//        ACDC_DataHandle(&stcRx[u8RxCnt], u8RxCnt);
//        Can_Network_Rx_Process(stcRx.u32ID, rev);
//    }
//}

/**********************************************************************************************************
**	函 数 名 : Can1Config
**	功能说明 : CAN1初始化配置
**	形    参 : 无
**	返 回 值 : 无
**  说    明 ：CAN时钟：240MHz / 6 = 40Mhz，CAN波特率：40MHz / Prescaler / (SEG1+SEG2) = 125Kbps
**********************************************************************************************************/
static void Can1Config(void)
{
    stc_can_init_t stcInit;
    CAN_DeInit(APP_CAN1_UNIT);
    stc_can_af_cfg_t astcAFCfg[] =
    {
        {APP_CAN_AF1_ID, APP_CAN_AF1_ID_MSK, APP_CAN_AF1_MSK_TYPE}, \
        {APP_CAN_AF2_ID, APP_CAN_AF2_ID_MSK, APP_CAN_AF2_MSK_TYPE}, \
        {APP_CAN_AF3_ID, APP_CAN_AF3_ID_MSK, APP_CAN_AF3_MSK_TYPE}, \
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
//	stcInit.u8WorkMode = CAN_MODE_ILB;   //内部回环用于测试
    stcInit.u8WorkMode = CAN_MODE_NORMAL;
    stcInit.u8RBSWarnLimit = MSG_MAX_NUM;         //设定RingBuffer将满中断最大缓存为1，Range[1,8]
    stcInit.pstcAFCfg = astcAFCfg;
    stcInit.u8RBOvfOp = CAN_RB_OVF_SAVE_NEW;
    stcInit.u16AFSel  = APP_CAN_AF_SEL;

    //配置CAN波特率为125Kbps
    stcInit.stcSBT.u32SEG1 = 1U + 15U;
    stcInit.stcSBT.u32SEG2 = 4U;
    stcInit.stcSBT.u32SJW  = 4U;
    stcInit.stcSBT.u32Prescaler = 16U;

    PWC_Fcg1PeriphClockCmd(APP_CAN1_PERIP_CLK, Enable);

    (void)CAN_Init(APP_CAN1_UNIT, &stcInit);

    /* Configures the interrupts if needed. */
    Can1IrqConfig();
}

#if 0  //暂未使用
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

    //配置CAN接收中断为接收将满中断
    CAN_IntCmd(APP_CAN1_UNIT, APP_CAN1_INT_TYPE, Enable);
}

/**********************************************************************************************************
**	函 数 名 : Can1IrqConfig
**	功能说明 : CAN1中断回调函数配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void CAN1_IrqCallback(void)
{
uint8_t m_u8ErrType = 0;
    uint32_t m_u32StatusVal = 0;
    m_u32StatusVal = CAN_GetStatusVal(APP_CAN1_UNIT);
    m_u8ErrType    = CAN_GetErrType(APP_CAN1_UNIT);

    if (m_u32StatusVal != 0U)
    {
        m_u8ErrType = (m_u8ErrType >> 5U);                     //获取错误类型：0:CAN_ERR_NO,1: CAN_ERR_BIT,2: CAN_ERR_FORM \
                                                                3:CAN_ERR_STUFF,4:CAN_ERR_ACK,5:CAN_ERR_CRC,6:CAN_ERR_OTHER
        if ((m_u32StatusVal & CAN_FLAG_PTB_TRANS_OK) != 0UL)   //PTB发送成功
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_STB_TRANS_OK) != 0UL)   //STB发送成功
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_OFF) != 0UL)        //进入BUSOFF状态，需等待11bit总线隐性电平或者重上电恢复正常
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_TX) != 0UL)         //总线忙，可以取消发送
        {

        }

        if ((m_u32StatusVal & CAN_FLAG_BUS_RX) != 0UL)        //总线数据接收中
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_TB_FULL) != 0UL)       //发送buffer满
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RB_OVF) != 0UL)        //接收buffer溢出
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_TRANS_ABORTED) != 0UL) //发送取消
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_INT) != 0UL)       //错误中断
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RB_ALMOST_FULL) != 0UL)//接收将满中断
        {
            CANx_ResvData(APP_CAN1_UNIT);                     //CAN1接收处理
        }
        if ((m_u32StatusVal & CAN_FLAG_RB_FIFO_FULL) != 0UL)  //接收满中断
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RX_OVERRUN) != 0UL)    //接收超限中断
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_RX_OK) != 0UL)         //单帧接收中断
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_BUS_ERR) != 0UL)       //总线错误中断
        {
            CAN_ClrStatus(APP_CAN1_UNIT,CAN_FLAG_BUS_ERR);    //及时清除错误标志
        }
        if ((m_u32StatusVal & CAN_FLAG_ARB_LOST) != 0UL)      //仲裁失败中断
        {
            CAN_ClrStatus(APP_CAN1_UNIT,CAN_FLAG_ARB_LOST);   //及时清除错误标志
        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_PASSIVE) != 0UL)   //被动错误中断
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_ERR_PASSIVE_NODE) != 0UL)//被动错误节点
        {

        }
        if ((m_u32StatusVal & CAN_FLAG_REACH_WARN_LIMIT) != 0UL)//TEC/REC达到告警限定值
        {

        }

        CAN_ClrStatus(APP_CAN1_UNIT, m_u32StatusVal);              //清除CAN1状态标志
    }
}

/*
*********************************************************************************************************
**  函 数 名: ACDC_Fan_Speed51_ModeTest
**  功能说明: ACDC模块测试，广播方式设置风扇到51%转速，并测试当前设备模块类型，初始化调用
**  形    参: 无
**  返 回 值: 0:测试失败，1：国电，2：普德，非0情况下，可赋值给全局变量ACDC.Type
*********************************************************************************************************
*/
uint8_t ACDC_Fan_Speed51_ModeTest(void)
{
    uint8_t i;
#if 1
    /*国电模块广播测试*/
    uint32_t ID1 = 0x10200000 | (uint16_t)(0xFF << 8);
    uint8_t Tdata1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Tdata1[1] = 0x37;
    Tdata1[7] = 0x33;   //51%转速
    ACDC.FanSpd[0]=0;   //判断条件清0
    ACDC.FanSpd[1]=0;
    for(i=0; i<3; i++)
    {
        CANx_SendData(APP_CAN1_UNIT, ID1, &Tdata1[0], 8);  //发给模块0,1
        osDelay(200);       //延时等待
    }
#endif
#if 0
    /*国电模块点播测试*/
    uint32_t ID1 = 0x10200000 | (uint16_t)(0x00 << 8);
    uint8_t Tdata1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Tdata1[1] = 0x37;
    Tdata1[7] = 0x33;   //51%转速
    CANx_SendData(APP_CAN1_UNIT, ID1, &Tdata1[0], 8);  //发给模块0
    bsp_DelayMS(500);    //等待0.5S接收处理

#ifndef YCJ
    ID1= 0x10200000 | (uint16_t)(0x01 << 8);
    CANx_SendData(APP_CAN1_UNIT, ID1, &Tdata1[0], 8);  //发给模块1
#endif
#endif

#ifdef YCJ
    if((ACDC.FanSpd[0]==0x33)||(ACDC.FanSpd[1]==0x33))
    {
        return 1;
    }
#else

    if((ACDC.FanSpd[0]==0x33)&&(ACDC.FanSpd[1]==0x33))
    {
        return 1;      //2个ACDC都正常
    } else if((ACDC.FanSpd[0]==0x33)||(ACDC.FanSpd[1]==0x33))
    {
        return 11;    //1个ACDC正常，一个不正常
    }
#endif

    /*普德模块广播测试*/
    ACDC.FanSpd[0]=0;
    ACDC.FanSpd[1]=0;
    uint32_t ID2 = 0xC600000B;
    uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Tdata2[0] = 0x01;
    Tdata2[3] = 0x1F;
    Tdata2[7] = 0x33;      //51%转速
    for(i=0; i<3; i++)     //发3包，再检测，避免丢包
    {
        CANx_SendData(APP_CAN1_UNIT, ID2, &Tdata2[0], 8);
        osDelay(200);        //延时等待
    }

#ifdef YCJ
    if((ACDC.FanSpd[0]==0x33)||(ACDC.FanSpd[1]==0x33))
    {
        return 2;
    }
#else
    if((ACDC.FanSpd[0]==0x33)&&(ACDC.FanSpd[1]==0x33))
    {
        return 2;      //2个ACDC都正常
    } else if((ACDC.FanSpd[0]==0x33)||(ACDC.FanSpd[1]==0x33))
    {
        return 22;    //1个ACDC正常，一个不正常
    }
#endif


    return 0;

}


/*
*********************************************************************************************************
**  函 数 名: ACDC_Fan_Speed_CMD
**  功能说明: 风扇调速命令
**  形    参: type:1国电*2，2普德*2，addr:模块地址  Speed：0~100
**  返 回 值: 无
*********************************************************************************************************
*/
void ACDC_Fan_Speed_CMD(uint8_t type, uint8_t addr, uint8_t Speed)
{
    //国电调速
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata[1] = 0x37;
        Tdata[7] = Speed;
        CANx_SendData(APP_CAN1_UNIT, ID, &Tdata[0], 8);
    }

    //普德调速
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x01, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x00;    //设置请求
        Tdata2[3] = 0x1F;    //风扇调速
        Tdata2[7] = Speed;   //设定值
        CANx_SendData(APP_CAN1_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  函 数 名: ACDC_DCV_Query_CMD
**  功能说明: ACDC直流侧电压查询发送指令，执行后，延时0.5S检查ACDC.更新值
**  形    参: type:1国电*2，2普德*2,addr:模块地址
**  返 回 值: 无
*********************************************************************************************************
*/
void ACDC_DCV_Query_CMD(uint8_t type, uint8_t addr)
{
    //国电DC电压查询
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN1_UNIT, ID, &Tdata[0], 8);
    }
    //普德DC电压查询
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x20;    //查询请求
        Tdata2[3] = 0x01;    //DC电压查询指令
        CANx_SendData(APP_CAN1_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  函 数 名: ACDC_DCI_Query_CMD
**  功能说明: ACDC直流侧电流查询,执行后，延时0.5S检查ACDC.更新值
**  形    参: type:1国电*2，2普德*2,addr:模块地址
**  返 回 值: 无
*********************************************************************************************************
*/
void ACDC_DCI_Query_CMD(uint8_t type, uint8_t addr)
{
    //国电DC电流查询
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN1_UNIT, ID, &Tdata[0], 8);
    }
    //普德DC电流查询
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x20;    //查询请求
        Tdata2[3] = 0x02;    //DC电流查询指令
        CANx_SendData(APP_CAN1_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  函 数 名: ACDC_ACV_Query_CMD
**  功能说明: ACDC交流电压查询,执行后，延时0.5S检查ACDC.更新值
**  形    参: type:1国电*2，2普德*2,addr:模块地址
**  返 回 值: 无
*********************************************************************************************************
*/
void ACDC_ACV_Query_CMD(uint8_t type, uint8_t addr)
{
    //国电AC电压查询
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN1_UNIT, ID, &Tdata[0], 8);
    }
    //普德AC电压查询
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x20;    //查询请求
        Tdata2[3] = 0x05;    //AC电压查询指令
        CANx_SendData(APP_CAN1_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  函 数 名: ACDC_ACI_Query_CMD
**  功能说明: ACDC交流电流查询
**  形    参: type:1国电*2，2普德*2,addr:模块地址
**  返 回 值: 无
*********************************************************************************************************
*/
void ACDC_ACI_Query_CMD(uint8_t type, uint8_t addr)
{
    //国电AC电流查询
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN1_UNIT, ID, &Tdata[0], 8);
    }
    //普德AC电流查询
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x20;    //查询请求
        Tdata2[3] = 0x06;    //AC电流查询指令
        CANx_SendData(APP_CAN1_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  函 数 名: ACDC_ACF_Query_CMD
**  功能说明: ACDC交流频率查询
**  形    参: addr:模块地址
**  返 回 值: 无
*********************************************************************************************************
*/
void ACDC_ACF_Query_CMD(uint8_t type, uint8_t addr)
{
    //国电AC频率查询
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN1_UNIT, ID, &Tdata[0], 8);
    }
    //普德AC频率查询
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x20;    //查询请求
        Tdata2[3] = 0x08;    //AC频率查询指令
        CANx_SendData(APP_CAN1_UNIT, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
**  函 数 名: ACDC_Fault_Clear_CMD
**  功能说明: ACDC故障清除
**  形    参: addr:模块地址
**  返 回 值: 无
*********************************************************************************************************
*/
void ACDC_Fault_Clear_CMD(uint8_t type, uint8_t addr)
{
    //国电故障清除
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(APP_CAN1_UNIT, ID, &Tdata[0], 8);
    }
    //普德故障清除
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x00;    //设置请求
        Tdata2[3] = 0x42;    //故障清除指令
        Tdata2[7] = 0x01;    //设置值
        CANx_SendData(APP_CAN1_UNIT, ID2, &Tdata2[0], 8);
    }
}

/*
*********************************************************************************************************
*   函 数 名: ACDC_FW_VERSION_CMD
*   功能说明: ACDC固件版本查询
*   形    参: addr:模块地址
*   返 回 值: 无
*********************************************************************************************************
*/
void ACDC_FW_VERSION_CMD(uint8_t type, uint8_t addr)
{
    //国电固件版本查询
    if(type == 1)
    {

    }
    //普德固件版本查询
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x20;    //设置请求
        Tdata2[3] = 0xBE;    //版本查询
        CANx_SendData(APP_CAN1_UNIT, ID2, &Tdata2[0], 8);
    }
}







/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
