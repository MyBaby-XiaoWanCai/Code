/*
*********************************************************************************************************
*
*	ģ������ : can
*	�ļ����� : can_process.c
*	��    �� : V1.0
*	˵    �� : �ṩCAN���ͺͽ��մ�����ʼ������
*
*
*
*	�޸ļ�¼ :
*	�汾��  ����         ����     ˵��
*	V1.0    2020-03-06   ZYL      ��ʽ����
*********************************************************************************************************
*/

#include "main.h"

//2��3����ȵ�FIFO
#define   CAN1FIFO   CAN_RX_FIFO0
#define   CAN2FIFO   CAN_RX_FIFO1  //

CAN_TxHeaderTypeDef     TxMeg;
CAN_RxHeaderTypeDef     RxMeg;

uint8_t  ACDC_Staste0[2]={0};   //ACDC����״̬��0��������1�Ǵӻ�
uint8_t  ACDC_Staste1[2]={0};   //ACDC����״̬��0��������1�Ǵӻ�
uint16_t ACDC_ACV[2]={0};       //ACDC������ѹ��0��������1�Ǵӻ�  ʵ��ֵ10��
uint16_t ACDC_ACI[2]={0};       //ACAI����������0��������1�Ǵӻ�  ʵ��ֵ10��
uint16_t ACDC_DCV[2]={0};       //ACDCֱ����ѹ��0��������1�Ǵӻ�  ʵ��ֵ100�����������ϴ�
uint16_t ACDC_DCI[2]={0};       //ACDIֱ��������0��������1�Ǵӻ�  ʵ��ֵ10�����������ϴ�

uint16_t DCI = 0;
/*
*********************************************************************************************************
*	�� �� ��: CAN_User_Init
*	����˵��: �û�CAN��������ʼ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void CAN_User_Init(CAN_HandleTypeDef* hcan )
{
    CAN_FilterTypeDef  sFilterConfig;//CAN1�˲���
    HAL_StatusTypeDef  HAL_Status;

    // uint16_t std_id=0x7e9;
    uint32_t ext_id=0x102b0000;
    uint32_t mask=0;
    // RxMeg.DLC=8;
    // RxMeg.IDE=CAN_ID_STD;
    // RxMeg.RTR=CAN_RTR_DATA;

    sFilterConfig.FilterBank = 0;//������0 ����0��13�Ǹ���CAN�ģ�14��27�Ǹ���CAN��
    sFilterConfig.FilterMode =  CAN_FILTERMODE_IDMASK;//��ʶ������λģʽ
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;//�˲���λ��Ϊ����32λ


    sFilterConfig.FilterIdHigh =((ext_id<<3)>>16)&0xffff;
    sFilterConfig.FilterIdLow = (uint16_t)(ext_id<<3)|CAN_ID_EXT;

	// mask =(std_id<<18);
	// mask^=ext_id;
	// mask=~mask;
	// mask<<=3;
	// mask|=0X02;

    mask=0x1FFF00FF;
    mask<<=3;
    mask|=0X0A;

    sFilterConfig.FilterMaskIdHigh =(mask>>16)&0xffff;//��������λ������Э����
    sFilterConfig.FilterMaskIdLow  = mask&0xffff;

    sFilterConfig.FilterFIFOAssignment = CAN1FIFO;//���յ��ı��ķ��뵽FIFO0��
    sFilterConfig.FilterActivation = ENABLE;  //ʹ�ܹ�����
    sFilterConfig.SlaveStartFilterBank  = 0; //14

    HAL_Status=HAL_CAN_ConfigFilter(hcan, &sFilterConfig);

    HAL_Status=HAL_CAN_Start(hcan);  //����CAN
    if(HAL_Status!=HAL_OK)
    {
        Error_Handler();
    }
    HAL_Status=HAL_CAN_ActivateNotification(hcan,CAN_IT_RX_FIFO0_MSG_PENDING);//ʹ���ж�֪ͨ
    if(HAL_Status!=HAL_OK)
    {
        Error_Handler();
    }
 }

/*
*********************************************************************************************************
*	�� �� ��: HAL_CAN_RxFifo0MsgPendingCallback
*	����˵��: CAN���ջص�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    uint8_t  Data[8];
    HAL_StatusTypeDef	HAL_RetVal;
    uint32_t  ID=0;
    // RxMeg.DLC=8;
    // RxMeg.IDE=CAN_ID_STD;
    // RxMeg.RTR=CAN_RTR_DATA;

    if(hcan ==&hcan1)
    {
        HAL_RetVal=HAL_CAN_GetRxMessage(hcan,  CAN1FIFO, &RxMeg,  Data);
        ID=RxMeg.ExtId;
        if ( HAL_OK==HAL_RetVal)
        {//�������������
            if(((ID&0X102B0000)==0X102B0000)&&(Data[0]==0XF0))
            {
                switch(Data[1])
                {
                    case 0x53://ACDCֱ�����ѹ
                        ACDC_DCV[(ID>>8)&0x01]=(uint16_t)(Data[6]<<8)+Data[7];
                        break;
                    case 0x54://ACDCֱ�������
                        ACDC_DCI[(ID>>8)&0x01]=(uint16_t)(Data[6]<<8)+Data[7];
                        break;
                    case 0x59://ACDC����״̬���������Э��
                        ACDC_Staste0[(ID>>8)&0x01]=Data[7];
                        ACDC_Staste1[(ID>>8)&0x01]=Data[6];
                        break;
                    case 0x62://ACDC�������ѹ
                        ACDC_ACV[(ID>>8)&0x01]=(uint16_t)(Data[6]<<8)+Data[7];
                        break;
                    case 0x63://ACDC���������
                        ACDC_ACI[(ID>>8)&0x01]=(uint16_t)(Data[6]<<8)+Data[7];
                        break;
                    default:
                        break;
                }
            }
        }
  }
}
/*
*********************************************************************************************************
*	�� �� ��: CANx_SendData
*	����˵��: CAN�������ݺ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CANx_SendData(CAN_HandleTypeDef* hcan,uint32_t ID,uint8_t *pData,uint16_t Len)
{
    HAL_StatusTypeDef	HAL_RetVal;
    TxMeg.RTR = CAN_RTR_DATA;//����֡
    TxMeg.IDE = CAN_ID_EXT;  //��չ֡
    TxMeg.StdId=ID;
    TxMeg.ExtId=ID;
    TxMeg.DLC=Len;
    //TxMeg.TransmitGlobalTime = DISABLE;

    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1)==0);//��ѯ�����Ƿ���
    HAL_RetVal=HAL_CAN_AddTxMessage(&hcan1,&TxMeg,pData,(uint32_t*)CAN_TX_MAILBOX0);
}
/*
*********************************************************************************************************
*	�� �� ��: ACDC_ONOFF_CMD
*	����˵��: ACDC���ػ�����
*	��    ��: addr:ģ���ַ ONOFF��1������0�ػ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_ONOFF_CMD(uint8_t addr,uint8_t ONOFF)
{
    uint32_t ID=0x10200000|(uint16_t)(addr<<8);
    uint8_t Tdata[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    if(ONOFF)
    Tdata[1]=0x51;
    else Tdata[1]=0x52;
    CANx_SendData(&hcan1,ID,&Tdata[0],8);
}
/*
*********************************************************************************************************
*	�� �� ��: ACDC_Fan_Speed_CMD
*	����˵��: ���ȵ�������
*	��    ��: addr��Ϊģ���ַ��(0x00~0x0F)��㲥ʶ����(0xFF) Speed��0~100
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_Fan_Speed_CMD(uint8_t addr,uint8_t Speed)
{
    uint32_t ID=0x10200000|(uint16_t)(addr<<8);
    uint8_t Tdata[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    Tdata[1]=0x37;
    Tdata[7]=Speed;
    CANx_SendData(&hcan1,ID,&Tdata[0],8);
}
/*
*********************************************************************************************************
*	�� �� ��: ACDC_DCV_Query_CMD
*	����˵��: ACDCֱ�����ѹ��ѯ
*	��    ��: addr:ģ���ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_DCV_Query_CMD(uint8_t addr)
{
    uint32_t ID=0x10200000|(uint16_t)(addr<<8);
    uint8_t Tdata[8]={0x00,0x53,0x00,0x00,0x00,0x00,0x00,0x00};
    CANx_SendData(&hcan1,ID,&Tdata[0],8);
}
/*
*********************************************************************************************************
*	�� �� ��: ACDC_DCI_Query_CMD
*	����˵��: ACDCֱ���������ѯ
*	��    ��: addr:ģ���ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_DCI_Query_CMD(uint8_t addr)
{
    uint32_t ID=0x10200000|(uint16_t)(addr<<8);
    uint8_t Tdata[8]={0x00,0x54,0x00,0x00,0x00,0x00,0x00,0x00};
    CANx_SendData(&hcan1,ID,&Tdata[0],8);
}
/*
*********************************************************************************************************
*	�� �� ��: ACDC_ACV_Query_CMD
*	����˵��: ACDC������ѹ��ѯ
*	��    ��: addr:ģ���ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_ACV_Query_CMD(uint8_t addr)
{
    uint32_t ID=0x10200000|(uint16_t)(addr<<8);
    uint8_t Tdata[8]={0x00,0x62,0x00,0x00,0x00,0x00,0x00,0x00};
    CANx_SendData(&hcan1,ID,&Tdata[0],8);
}
/*
*********************************************************************************************************
*	�� �� ��: ACDC_ACI_Query_CMD
*	����˵��: ACDC����������ѯ
*	��    ��: addr:ģ���ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_ACI_Query_CMD(uint8_t addr)
{
    uint32_t ID=0x10200000|(uint16_t)(addr<<8);
    uint8_t Tdata[8]={0x00,0x63,0x00,0x00,0x00,0x00,0x00,0x00};
    CANx_SendData(&hcan1,ID,&Tdata[0],8);
}
/*
*********************************************************************************************************
*	�� �� ��: ACDC_ACF_Query_CMD
*	����˵��: ACDC����Ƶ�ʲ�ѯ
*	��    ��: addr:ģ���ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_ACF_Query_CMD(uint8_t addr)
{
    uint32_t ID=0x10200000|(uint16_t)(addr<<8);
    uint8_t Tdata[8]={0x00,0x64,0x00,0x00,0x00,0x00,0x00,0x00};
    CANx_SendData(&hcan1,ID,&Tdata[0],8);
}
/*
*********************************************************************************************************
*	�� �� ��: ACDC_Fault_Clear_CMD
*	����˵��: ACDC�������
*	��    ��: addr:ģ���ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ACDC_Fault_Clear_CMD(uint8_t addr)
{
    uint32_t ID=0x10200000|(uint16_t)(addr<<8);
    uint8_t Tdata[8]={0x00,0x79,0x00,0x00,0x00,0x00,0x00,0x00};
    CANx_SendData(&hcan1,ID,&Tdata[0],8);
}
/*****************************(END OF FILE) *********************************/
