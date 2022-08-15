/**********************************************************************************************************
*
*   ģ������ : ACtoDC ģ��
*   �ļ����� : bsp_acdc.c
*   ��    �� : V1.0
*   ˵    �� : ACtoDC ģ�鹦��
*   ��   �ߣ�Robert.Li
*   ��   ��:  2021.7.14

**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp.h"

#include "stm32f4xx_hal_can.h"


/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

CAN_TxHeaderTypeDef     TxMeg;
CAN_RxHeaderTypeDef     RxMeg;

ACDC_Inf ACDC= {1,{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}; //����ACDC�ṹ�壬Ĭ�Ϲ���ACDC��������ʼ��Ϊ0

extern uint32_t IP_Alarm_Flag[8];       //�ڲ�������־λ
extern __IO uint8_t ACDC_UpdateFlg;


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/*
*********************************************************************************************************
*   �� �� ��: MX    CAN1 Initialization Function
*   ����˵��: None
*   ��    ��:  None
*   �� �� ֵ: None
*********************************************************************************************************
*/

/* CAN1 init function */
void MX_CAN1_Init(void)
{
    /*��Ƶ180M��Apb1clock=45M,����125Kbps*/
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
    hcan1.Init.TransmitFifoPriority = ENABLE;   //ʹ�ܷ���fifo
    if(HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

}

/*
*********************************************************************************************************
*   �� �� ��: CAN   MSP Initialization
*   ����˵��: This function configures the hardware resources used in this example
*   ��    ��: hadc: CAN handle pointer
*   �� �� ֵ: None
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
*   �� �� ��: CAN MSP De-Initialization
*   ����˵��: This function freeze the hardware resources used in this example
*   ��    ��: hadc: CAN handle pointer
*   �� �� ֵ: None
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
*   �� �� ��: CAN_User_Init
*   ����˵��: �û�CAN��������ʼ������
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/

void CAN_User_Init(CAN_HandleTypeDef *hcan)
{

    CAN_FilterTypeDef  sFilterConfig;//CAN1�˲���
    HAL_StatusTypeDef  HAL_Status;
    /*����ACDC��canID����������*/
    uint32_t ext_id1 = 0x10000000; //����ACDC��can����ID
    uint32_t mask = 0;            //����λ��1��Ч��0����

    MX_CAN1_Init();              //can1Ӳ����ʼ��
    sFilterConfig.FilterBank = 0;//������0 ����0��13�Ǹ���CAN�ģ�14��27�Ǹ���CAN��
    sFilterConfig.FilterMode =  CAN_FILTERMODE_IDMASK;//��ʶ������λģʽ
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;//�˲���λ��Ϊ����32λ

    /*���ñ�ʶ���Ĵ���*/
    sFilterConfig.FilterIdHigh = ((ext_id1 << 3) >> 16) & 0xffff;
    sFilterConfig.FilterIdLow = (uint16_t)(ext_id1 << 3) | CAN_ID_EXT;
    /*��������λ�Ĵ���,����0x_102xxxxxID��*/
    mask = 0x1F000000;
    mask <<= 3;
    //mask |= 0X0A;                                     //ֻ��������֡��RTR=0
    sFilterConfig.FilterMaskIdHigh = (mask >> 16) & 0xffff; //��������λ������Э����
    sFilterConfig.FilterMaskIdLow  = mask & 0xffff;

    sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO0;//���յ��ı��ķ��뵽FIFO0��
    sFilterConfig.FilterActivation = ENABLE;  //ʹ�ܹ�����
    sFilterConfig.SlaveStartFilterBank  = 14; //14
    HAL_Status = HAL_CAN_ConfigFilter(hcan, &sFilterConfig);



    /*�յ�ACDC��canID����������,����0x_060xxxxxID��*/
    uint32_t ext_id2 = 0x06000000; //�յ�ACDC��can����ID

    sFilterConfig.FilterBank = 4;//������4 ����0��13�Ǹ���CAN�ģ�14��27�Ǹ���CAN��
    sFilterConfig.FilterMode =  CAN_FILTERMODE_IDMASK;//��ʶ������λģʽ
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;//�˲���λ��Ϊ����32λ

    /*���ñ�ʶ���Ĵ���*/
    sFilterConfig.FilterIdHigh = ((ext_id2 << 3) >> 16) & 0xffff;
    sFilterConfig.FilterIdLow = (uint16_t)(ext_id2 << 3) | CAN_ID_EXT;
    /*��������λ�Ĵ���,����0x_060xxxxxID��*/
    mask = 0x1FF00000;
    mask <<= 3;
    //mask |= 0X0A;                                     //ֻ��������֡��RTR=0
    sFilterConfig.FilterMaskIdHigh = (mask >> 16) & 0xffff; //��������λ������Э����
    sFilterConfig.FilterMaskIdLow  = mask & 0xffff;

    sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO1;//���յ��ı��ķ��뵽FIFO1��
    sFilterConfig.FilterActivation = ENABLE;  //ʹ�ܹ�����
    sFilterConfig.SlaveStartFilterBank  = 14; //14
    HAL_Status = HAL_CAN_ConfigFilter(hcan, &sFilterConfig);

    /*�յ�ACDC��canID����������,����0x_1cxxxxxxID��*/
    uint32_t ext_id3 = 0x1c000000; //�յ�ACDC��can����ID(������)

    sFilterConfig.FilterBank = 8;//������8 ����0��13�Ǹ���CAN�ģ�14��27�Ǹ���CAN��
    sFilterConfig.FilterMode =  CAN_FILTERMODE_IDMASK;//��ʶ������λģʽ
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;//�˲���λ��Ϊ����32λ

    /*���ñ�ʶ���Ĵ���*/
    sFilterConfig.FilterIdHigh = ((ext_id3 << 3) >> 16) & 0xffff;
    sFilterConfig.FilterIdLow = (uint16_t)(ext_id3 << 3) | CAN_ID_EXT;
    /*��������λ�Ĵ���,����0x_1cxxxxxxID��*/
    mask = 0x1F000000;
    mask <<= 3;
    //mask |= 0X0A;                                     //ֻ��������֡��RTR=0
    sFilterConfig.FilterMaskIdHigh = (mask >> 16) & 0xffff; //��������λ������Э����
    sFilterConfig.FilterMaskIdLow  = mask & 0xffff;

    sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO1;//���յ��ı��ķ��뵽FIFO1��
    sFilterConfig.FilterActivation = ENABLE;  //ʹ�ܹ�����
    sFilterConfig.SlaveStartFilterBank  = 14; //14
    HAL_Status = HAL_CAN_ConfigFilter(hcan, &sFilterConfig);


    HAL_Status = HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING); //ʹ��fifo0�ж�֪ͨ
    if(HAL_Status != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    HAL_Status = HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING); //ʹ��fifo1�ж�֪ͨ
    if(HAL_Status != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    HAL_Status = HAL_CAN_Start(hcan); //����CAN
    if(HAL_Status != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
}


/*
*********************************************************************************************************
*   �� �� ��: bsp_InitACDC
*   ����˵��: ACDC�ӿڳ�ʼ��
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitACDC(void)
{
    CAN_User_Init(&hcan1);
}
/*
*********************************************************************************************************
*   �� �� ��: HAL_CAN_RxFifo0MsgPendingCallback
*   ����˵��: CAN���ջص�����
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

}


/*
*********************************************************************************************************
*   �� �� ��: CANx_SendData
*   ����˵��: CAN�������ݺ���
*   ��    ��: ��
*   �� �� ֵ: ��
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
		if((ACDC.Type==2)||(ACDC.Type==22))  //�յ�
		{
			while((HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) < 3) && (timeout < 1000))	//��ʱ�ȴ�1ms
			{
				timeout++;
				Feed_WDG();		//�ֶ�ι��
				bsp_DelayUS(1);
			};
			timeout = 0;
		}
	}
	else if(ACDC_UpdateFlg == 1)
	{
		while((HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) && (timeout < 1000))		//��ʱ�ȴ�1ms
		{
			timeout++;
			Feed_WDG();			//�ֶ�ι��
			bsp_DelayUS(1);			
		}
		timeout = 0;
	}
	
	HAL_RetVal = HAL_CAN_AddTxMessage(&hcan1, &TxMeg, pData, (uint32_t *)CAN_TX_MAILBOX0);
	
	if(HAL_RetVal == HAL_ERROR)
	{
		for(uint8_t j = 0; j < CHANNEL_CFG; j++)
		{
			IP_Alarm_Flag[j] |= IntelProtect_C1E;	//can1����  
		}
    }
    return HAL_RetVal;
}

/*
*********************************************************************************************************
*   �� �� ��: ACDC_Fan_Speed51_ModeTest
*   ����˵��: ACDCģ����ԣ��㲥��ʽ���÷��ȵ�51%ת�٣������Ե�ǰ�豸ģ�����ͣ���ʼ������
*   ��    ��: ��
*   �� �� ֵ: 0:����ʧ�ܣ�1�����磬2���յ£���0����£��ɸ�ֵ��ȫ�ֱ���ACDC.Type
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
    for(i=0;i<3;i++)
    {
    CANx_SendData(&hcan1, ID1, &Tdata1[0], 8);  //����ģ��0,1
    osDelay(200);       //��ʱ�ȴ�
    }
#endif
    
#if 0
    /*����ģ��㲥����*/
    uint32_t ID1 = 0x10200000 | (uint16_t)(0x00 << 8);
    uint8_t Tdata1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Tdata1[1] = 0x37;
    Tdata1[7] = 0x33;   //51%ת��
    CANx_SendData(&hcan1, ID1, &Tdata1[0], 8);  //����ģ��0
    bsp_DelayMS(200);    //�ȴ�0.2S���մ���
    
#ifndef YCJ    
    ID1= 0x10200000 | (uint16_t)(0x01 << 8);
    CANx_SendData(&hcan1, ID1, &Tdata1[0], 8);  //����ģ��1
    bsp_DelayMS(200);    //�ȴ�0.2S���մ���
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
    }else if((ACDC.FanSpd[0]==0x33)||(ACDC.FanSpd[1]==0x33))
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
    for(i=0;i<3;i++)       //��3�����ټ�⣬���ⶪ��
    {    
    CANx_SendData(&hcan1, ID2, &Tdata2[0], 8); 
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
    }else if((ACDC.FanSpd[0]==0x33)||(ACDC.FanSpd[1]==0x33))
    {
        return 22;    //1��ACDC������һ��������
    }
#endif    


    return 0;
}


/*
*********************************************************************************************************
*   �� �� ��: ACDC_Fan_Speed_CMD
*   ����˵��: ���ȵ�������
*   ��    ��: type:1����*2��2�յ�*2��addr:ģ���ַ  Speed��0~100
*   �� �� ֵ: ��
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
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
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
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   �� �� ��: ACDC_DCV_Query_CMD
*   ����˵��: ACDCֱ�����ѹ��ѯ����ָ�ִ�к���ʱ0.5S���ACDC.����ֵ
*   ��    ��: type:1����*2��2�յ�*2,addr:ģ���ַ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_DCV_Query_CMD(uint8_t type, uint8_t addr)
{
    //����DC��ѹ��ѯ
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
    }
    //�յ�DC��ѹ��ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x01;    //DC��ѹ��ѯָ��
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   �� �� ��: ACDC_DCI_Query_CMD
*   ����˵��: ACDCֱ���������ѯ,ִ�к���ʱ0.5S���ACDC.����ֵ
*   ��    ��: type:1����*2��2�յ�*2,addr:ģ���ַ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_DCI_Query_CMD(uint8_t type, uint8_t addr)
{
    //����DC������ѯ
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
    }
    //�յ�DC������ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x02;    //DC������ѯָ��
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   �� �� ��: ACDC_ACV_Query_CMD
*   ����˵��: ACDC������ѹ��ѯ,ִ�к���ʱ0.5S���ACDC.����ֵ
*   ��    ��: type:1����*2��2�յ�*2,addr:ģ���ַ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_ACV_Query_CMD(uint8_t type, uint8_t addr)
{
    //����AC��ѹ��ѯ
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
    }
    //�յ�AC��ѹ��ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x05;    //AC��ѹ��ѯָ��
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   �� �� ��: ACDC_ACI_Query_CMD
*   ����˵��: ACDC����������ѯ
*   ��    ��: type:1����*2��2�յ�*2,addr:ģ���ַ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_ACI_Query_CMD(uint8_t type, uint8_t addr)
{
    //����AC������ѯ
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
    }
    //�յ�AC������ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x06;    //AC������ѯָ��
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   �� �� ��: ACDC_ACF_Query_CMD
*   ����˵��: ACDC����Ƶ�ʲ�ѯ
*   ��    ��: addr:ģ���ַ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_ACF_Query_CMD(uint8_t type, uint8_t addr)
{
    //����ACƵ�ʲ�ѯ
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
    }
    //�յ�ACƵ�ʲ�ѯ
    if(type == 2)
    {
        uint32_t ID2 = 0xC608000B | (uint16_t)(addr << 11);
        uint8_t Tdata2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        Tdata2[0] = 0x01;    //����㲥
        Tdata2[1] = 0x20;    //��ѯ����
        Tdata2[3] = 0x08;    //ACƵ�ʲ�ѯָ��
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}
/*
*********************************************************************************************************
*   �� �� ��: ACDC_Fault_Clear_CMD
*   ����˵��: ACDC�������
*   ��    ��: addr:ģ���ַ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_Fault_Clear_CMD(uint8_t type, uint8_t addr)
{
    //����������
    if(type == 1)
    {
        uint32_t ID = 0x10200000 | (uint16_t)(addr << 8);
        uint8_t Tdata[8] = {0x00, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CANx_SendData(&hcan1, ID, &Tdata[0], 8);
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
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
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
        CANx_SendData(&hcan1, ID2, &Tdata2[0], 8);
    }
}







/**********************************************************************************************************
**  �� �� �� :
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
