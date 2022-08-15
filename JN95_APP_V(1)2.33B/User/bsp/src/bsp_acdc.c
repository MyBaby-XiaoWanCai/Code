/**********************************************************************************************************
*
*   模块名称 : ACtoDC 模块
*   文件名称 : bsp_acdc.c
*   版    本 : V1.0
*   说    明 : ACtoDC 模块功能
*   作   者：Robert.Li
*   日   期:  2021.7.14

**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp.h"

#include "stm32f4xx_hal_can.h"


/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

CAN_TxHeaderTypeDef     TxMeg;
CAN_RxHeaderTypeDef     RxMeg;

ACDC_Inf ACDC= {1,{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}; //定义ACDC结构体，默认国电ACDC，参数初始化为0

extern uint32_t IP_Alarm_Flag[8];       //内部保护标志位
extern __IO uint8_t ACDC_UpdateFlg;


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/*
*********************************************************************************************************
*   函 数 名: MX    CAN1 Initialization Function
*   功能说明: None
*   形    参:  None
*   返 回 值: None
*********************************************************************************************************
*/

/* CAN1 init function */
void MX_CAN1_Init(void)
{
    /*主频180M，Apb1clock=45M,速率125Kbps*/
    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 18;
    hcan1.Init.Mode = CAN_MODE_NORMAL;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_16TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_3TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = DISABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = ENABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = ENABLE;   //使能发送fifo
    if(HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

}

/*
*********************************************************************************************************
*   函 数 名: CAN   MSP Initialization
*   功能说明: This function configures the hardware resources used in this example
*   形    参: hadc: CAN handle pointer
*   返 回 值: None
*********************************************************************************************************
*/

void HAL_CAN_MspInit(CAN_HandleTypeDef *canHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(canHandle->Instance == CAN1)
    {
        /* USER CODE BEGIN CAN1_MspInit 0 */

        /* USER CODE END CAN1_MspInit 0 */
        /* CAN1 clock enable */
        __HAL_RCC_CAN1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**CAN1 GPIO Configuration
        PA11     ------> CAN1_RX
        PA12     ------> CAN1_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* CAN1 interrupt Init */
        HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 2, 1);
        HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
        HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 2, 1);
        HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
        /* USER CODE BEGIN CAN1_MspInit 1 */

        /* USER CODE END CAN1_MspInit 1 */
    }
}



/*
*********************************************************************************************************
*   函 数 名: CAN MSP De-Initialization
*   功能说明: This function freeze the hardware resources used in this example
*   形    参: hadc: CAN handle pointer
*   返 回 值: None
*********************************************************************************************************
*/

void HAL_CAN_MspDeInit(CAN_HandleTypeDef *canHandle)
{

    if(canHandle->Instance == CAN1)
    {
        /* USER CODE BEGIN CAN1_MspDeInit 0 */

        /* USER CODE END CAN1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_CAN1_CLK_DISABLE();

        /**CAN1 GPIO Configuration
        PA11     ------> CAN1_RX
        PA12     ------> CAN1_TX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);

        /* CAN1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
        HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
        /* USER CODE BEGIN CAN1_MspDeInit 1 */

        /* USER CODE END CAN1_MspDeInit 1 */
    }
}


/*
*********************************************************************************************************
*   函 数 名: CAN_User_Init
*   功能说明: 用户CAN过滤器初始化函数
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/

void CAN_User_Init(CAN_HandleTypeDef *hcan)
{

    CAN_FilterTypeDef  sFilterConfig;//CAN1滤波器
    HAL_StatusTypeDef  HAL_Status;
    /*国电ACDC的canID过滤器配置*/
    uint32_t ext_id1 = 0x10000000; //国电ACDC的can接收ID
    uint32_t mask = 0;            //屏蔽位，1有效，0屏蔽

    MX_CAN1_Init();              //can1硬件初始化
    sFilterConfig.FilterBank = 0;//过滤器0 其中0～13是给主CAN的，14～27是给从CAN的
    sFilterConfig.FilterMode =  CAN_FILTERMODE_IDMASK;//标识符屏蔽位模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;//滤波器位宽为单个32位

    /*设置标识符寄存器*/
    sFilterConfig.FilterIdHigh = ((ext_id1 << 3) >> 16) & 0xffff;
    sFilterConfig.FilterIdLow = (uint16_t)(ext_id1 << 3) | CAN_ID_EXT;
    /*设置屏蔽位寄存器,接收0x_102xxxxxID段*/
    mask = 0x1F000000;
    mask <<= 3;
    //mask |= 0X0A;                                     //只接收数据帧，RTR=0
    sFilterConfig.FilterMaskIdHigh = (mask >> 16) & 0xffff; //报文屏蔽位，根据协议来
    sFilterConfig.FilterMaskIdLow  = mask & 0xffff;

    sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO0;//接收到的报文放入到FIFO0中
    sFilterConfig.FilterActivation = ENABLE;  //使能过滤器
    sFilterConfig.SlaveStartFilterBank  = 14; //14
    HAL_Status = HAL_CAN_ConfigFilter(hcan, &sFilterConfig);



    /*普德ACDC的canID过滤器配置,接收0x_060xxxxxID段*/
    uint32_t ext_id2 = 0x06000000; //普德ACDC的can接收ID

    sFilterConfig.FilterBank = 4;//过滤器4 其中0～13是给主CAN的，14～27是给从CAN的
    sFilterConfig.FilterMode =  CAN_FILTERMODE_IDMASK;//标识符屏蔽位模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;//滤波器位宽为单个32位

    /*设置标识符寄存器*/
    sFilterConfig.FilterIdHigh = ((ext_id2 << 3) >> 16) & 0xffff;
    sFilterConfig.FilterIdLow = (uint16_t)(ext_id2 << 3) | CAN_ID_EXT;
    /*设置屏蔽位寄存器,接收0x_060xxxxxID段*/
    mask = 0x1FF00000;
    mask <<= 3;
    //mask |= 0X0A;                                     //只接收数据帧，RTR=0
    sFilterConfig.FilterMaskIdHigh = (mask >> 16) & 0xffff; //报文屏蔽位，根据协议来
    sFilterConfig.FilterMaskIdLow  = mask & 0xffff;

    sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO1;//接收到的报文放入到FIFO1中
    sFilterConfig.FilterActivation = ENABLE;  //使能过滤器
    sFilterConfig.SlaveStartFilterBank  = 14; //14
    HAL_Status = HAL_CAN_ConfigFilter(hcan, &sFilterConfig);

    /*普德ACDC的canID过滤器配置,接收0x_1cxxxxxxID段*/
    uint32_t ext_id3 = 0x1c000000; //普德ACDC的can接收ID(升级用)

    sFilterConfig.FilterBank = 8;//过滤器8 其中0～13是给主CAN的，14～27是给从CAN的
    sFilterConfig.FilterMode =  CAN_FILTERMODE_IDMASK;//标识符屏蔽位模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;//滤波器位宽为单个32位

    /*设置标识符寄存器*/
    sFilterConfig.FilterIdHigh = ((ext_id3 << 3) >> 16) & 0xffff;
    sFilterConfig.FilterIdLow = (uint16_t)(ext_id3 << 3) | CAN_ID_EXT;
    /*设置屏蔽位寄存器,接收0x_1cxxxxxxID段*/
    mask = 0x1F000000;
    mask <<= 3;
    //mask |= 0X0A;                                     //只接收数据帧，RTR=0
    sFilterConfig.FilterMaskIdHigh = (mask >> 16) & 0xffff; //报文屏蔽位，根据协议来
    sFilterConfig.FilterMaskIdLow  = mask & 0xffff;

    sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO1;//接收到的报文放入到FIFO1中
    sFilterConfig.FilterActivation = ENABLE;  //使能过滤器
    sFilterConfig.SlaveStartFilterBank  = 14; //14
    HAL_Status = HAL_CAN_ConfigFilter(hcan, &sFilterConfig);


    HAL_Status = HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING); //使能fifo0中断通知
    if(HAL_Status != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    HAL_Status = HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING); //使能fifo1中断通知
    if(HAL_Status != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    HAL_Status = HAL_CAN_Start(hcan); //开启CAN
    if(HAL_Status != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
}


/*
*********************************************************************************************************
*   函 数 名: bsp_InitACDC
*   功能说明: ACDC接口初始化
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitACDC(void)
{
    CAN_User_Init(&hcan1);
}
/*
*********************************************************************************************************
*   函 数 名: HAL_CAN_RxFifo0MsgPendingCallback
*   功能说明: CAN接收回调函数
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

}


/*
*********************************************************************************************************
*   函 数 名: CANx_SendData
*   功能说明: CAN发送数据函数
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
HAL_StatusTypeDef CANx_SendData(CAN_HandleTypeDef *hcan, uint32_t ID, uint8_t *pData, uint16_t Len)
{
	uint16_t timeout = 0;
    HAL_StatusTypeDef HAL_RetVal;
	
    TxMeg.IDE = CAN_ID_EXT;
    TxMeg.StdId = ID;
    TxMeg.ExtId = ID;
    TxMeg.DLC = Len;
	
    if(ACDC_UpdateFlg == 0) 
	{
		if((ACDC.Type==2)||(ACDC.Type==22))  //普德
		{
			while((HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) < 3) && (timeout < 1000))	//超时等待1ms
			{
				timeout++;
				Feed_WDG();		//手动喂狗
				bsp_DelayUS(1);
			};
			timeout = 0;
		}
	}
	else if(ACDC_UpdateFlg == 1)
	{
		while((HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) && (timeout < 1000))		//超时等待1ms
		{
			timeout++;
			Feed_WDG();			//手动喂狗
			bsp_DelayUS(1);			
		}
		timeout = 0;
	}
	
	HAL_RetVal = HAL_CAN_AddTxMessage(&hcan1, &TxMeg, pData, (uint32_t *)CAN_TX_MAILBOX0);
	
	if(HAL_RetVal == HAL_ERROR)
	{
		for(uint8_t j = 0; j < CHANNEL_CFG; j++)
		{
			IP_Alarm_Flag[j] |= IntelProtect_C1E;	//can1故障  
		}
    }
    return HAL_RetVal;
}

/*
*********************************************************************************************************
*   函 数 名: ACDC_Fan_Speed51_ModeTest
*   功能说明: ACDC模块测试，广播方式设置风扇到51%转速，并测试当前设备模块类型，初始化调用
*   形    参: 无
*   返 回 值: 0:测试失败，1：国电，2：普德，非0情况下，可赋值给全局变量ACDC.Type
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
    for(i=0;i<3;i++)
    {
    CANx_SendData(&hcan1, ID1, &Tdata1[0], 8);  //发给模块0,1
    osDelay(200);       //延时等待
    }
#endif
    
#if 0
    /*国电模块点播测试*/
    uint32_t ID1 = 0x10200000 | (uint16_t)(0x00 << 8);
    uint8_t Tdata1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Tdata1[1] = 0x37;
    Tdata1[7] = 0x33;   //51%转速
    CANx_SendData(&hcan1, ID1, &Tdata1[0], 8);  //发给模块0
    bsp_DelayMS(200);    //等待0.2S接收处理
    
#ifndef YCJ    
    ID1= 0x10200000 | (uint16_t)(0x01 << 8);
    CANx_SendData(&hcan1, ID1, &Tdata1[0], 8);  //发给模块1
    bsp_DelayMS(200);    //等待0.2S接收处理
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
        return 1;      //2个ACDC都正常
    }else if((ACDC.FanSpd[0]==0x33)||(ACDC.FanSpd[1]==0x33))
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
    for(i=0;i<3;i++)       //发3包，再检测，避免丢包
    {    
    CANx_SendData(&hcan1, ID2, &Tdata2[0], 8); 
    osDelay(200);        //延时等待     
    }
    
#ifdef YCJ
    if(ACDC.FanSpd[0]==0x33)
    {
        return 2;
    }    
#else    
    if((ACDC.FanSpd[0]==0x33)&&(ACDC.FanSpd[1]==0x33))
    {
        return 2;      //2个ACDC都正常
    }else if((ACDC.FanSpd[0]==0x33)||(ACDC.FanSpd[1]==0x33))
    {
        return 22;    //1个ACDC正常，一个不正常
    }
#endif    


    return 0;
}


/*
*********************************************************************************************************
*   函 数 名: ACDC_Fan_Speed_CMD
*   功能说明: 风扇调速命令
*   形    参: type:1国电*2，2普德*2，addr:模块地址  Speed：0~100
*   返 回 值: 无
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
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
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
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   函 数 名: ACDC_DCV_Query_CMD
*   功能说明: ACDC直流侧电压查询发送指令，执行后，延时0.5S检查ACDC.更新值
*   形    参: type:1国电*2，2普德*2,addr:模块地址
*   返 回 值: 无
*********************************************************************************************************
*/
void ACDC_DCV_Query_CMD(uint8_t type, uint8_t addr)
{
    //国电DC电压查询
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
    }
    //普德DC电压查询
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x20;    //查询请求
        Tdata2[3] = 0x01;    //DC电压查询指令
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   函 数 名: ACDC_DCI_Query_CMD
*   功能说明: ACDC直流侧电流查询,执行后，延时0.5S检查ACDC.更新值
*   形    参: type:1国电*2，2普德*2,addr:模块地址
*   返 回 值: 无
*********************************************************************************************************
*/
void ACDC_DCI_Query_CMD(uint8_t type, uint8_t addr)
{
    //国电DC电流查询
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
    }
    //普德DC电流查询
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x20;    //查询请求
        Tdata2[3] = 0x02;    //DC电流查询指令
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   函 数 名: ACDC_ACV_Query_CMD
*   功能说明: ACDC交流电压查询,执行后，延时0.5S检查ACDC.更新值
*   形    参: type:1国电*2，2普德*2,addr:模块地址
*   返 回 值: 无
*********************************************************************************************************
*/
void ACDC_ACV_Query_CMD(uint8_t type, uint8_t addr)
{
    //国电AC电压查询
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
    }
    //普德AC电压查询
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x20;    //查询请求
        Tdata2[3] = 0x05;    //AC电压查询指令
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   函 数 名: ACDC_ACI_Query_CMD
*   功能说明: ACDC交流电流查询
*   形    参: type:1国电*2，2普德*2,addr:模块地址
*   返 回 值: 无
*********************************************************************************************************
*/
void ACDC_ACI_Query_CMD(uint8_t type, uint8_t addr)
{
    //国电AC电流查询
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
    }
    //普德AC电流查询
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x20;    //查询请求
        Tdata2[3] = 0x06;    //AC电流查询指令
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   函 数 名: ACDC_ACF_Query_CMD
*   功能说明: ACDC交流频率查询
*   形    参: addr:模块地址
*   返 回 值: 无
*********************************************************************************************************
*/
void ACDC_ACF_Query_CMD(uint8_t type, uint8_t addr)
{
    //国电AC频率查询
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
    }
    //普德AC频率查询
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //分组广播
        Tdata2[1] = 0x20;    //查询请求
        Tdata2[3] = 0x08;    //AC频率查询指令
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   函 数 名: ACDC_Fault_Clear_CMD
*   功能说明: ACDC故障清除
*   形    参: addr:模块地址
*   返 回 值: 无
*********************************************************************************************************
*/
void ACDC_Fault_Clear_CMD(uint8_t type, uint8_t addr)
{
    //国电故障清除
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
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
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
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
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}







/**********************************************************************************************************
**  函 数 名 :
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
