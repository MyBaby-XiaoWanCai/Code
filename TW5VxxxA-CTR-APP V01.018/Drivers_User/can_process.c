/*
*********************************************************************************************************
*
*	模块名称 : can
*	文件名称 : can_process.c
*	版    本 : V1.0
*	说    明 : 提供CAN发送和接收处理及初始化函数
*
*
*
*	修改记录 :
*	版本号  日期         作者     说明
*	V1.0    2020-03-06   ZYL      正式发布
*********************************************************************************************************
*/

#include "main.h"

//2个3级深度的FIFO
#define   CAN1FIFO   CAN_RX_FIFO0
#define   CAN2FIFO   CAN_RX_FIFO1  //

CAN_TxHeaderTypeDef     TxMeg;
CAN_RxHeaderTypeDef     RxMeg;

uint8_t  ACDC_Staste0[2]={0};   //ACDC工作状态，0是主机，1是从机
uint8_t  ACDC_Staste1[2]={0};   //ACDC工作状态，0是主机，1是从机
uint16_t ACDC_ACV[2]={0};       //ACDC交流电压，0是主机，1是从机  实际值10倍
uint16_t ACDC_ACI[2]={0};       //ACAI交流电流，0是主机，1是从机  实际值10倍
uint16_t ACDC_DCV[2]={0};       //ACDC直流电压，0是主机，1是从机  实际值100倍，不主动上传
uint16_t ACDC_DCI[2]={0};       //ACDI直流电流，0是主机，1是从机  实际值10倍，不主动上传

uint16_t DCI = 0;
/*
*********************************************************************************************************
*	函 数 名: CAN_User_Init
*	功能说明: 用户CAN过滤器初始化函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void CAN_User_Init(CAN_HandleTypeDef* hcan )
{
    CAN_FilterTypeDef  sFilterConfig;//CAN1滤波器
    HAL_StatusTypeDef  HAL_Status;

    // uint16_t std_id=0x7e9;
    uint32_t ext_id=0x102b0000;
    uint32_t mask=0;
    // RxMeg.DLC=8;
    // RxMeg.IDE=CAN_ID_STD;
    // RxMeg.RTR=CAN_RTR_DATA;

    sFilterConfig.FilterBank = 0;//过滤器0 其中0～13是给主CAN的，14～27是给从CAN的
    sFilterConfig.FilterMode =  CAN_FILTERMODE_IDMASK;//标识符屏蔽位模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;//滤波器位宽为单个32位


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

    sFilterConfig.FilterMaskIdHigh =(mask>>16)&0xffff;//报文屏蔽位，根据协议来
    sFilterConfig.FilterMaskIdLow  = mask&0xffff;

    sFilterConfig.FilterFIFOAssignment = CAN1FIFO;//接收到的报文放入到FIFO0中
    sFilterConfig.FilterActivation = ENABLE;  //使能过滤器
    sFilterConfig.SlaveStartFilterBank  = 0; //14

    HAL_Status=HAL_CAN_ConfigFilter(hcan, &sFilterConfig);

    HAL_Status=HAL_CAN_Start(hcan);  //开启CAN
    if(HAL_Status!=HAL_OK)
    {
        Error_Handler();
    }
    HAL_Status=HAL_CAN_ActivateNotification(hcan,CAN_IT_RX_FIFO0_MSG_PENDING);//使能中断通知
    if(HAL_Status!=HAL_OK)
    {
        Error_Handler();
    }
 }

/*
*********************************************************************************************************
*	函 数 名: HAL_CAN_RxFifo0MsgPendingCallback
*	功能说明: CAN接收回调函数
*	形    参: 无
*	返 回 值: 无
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
        {//在这里接收数据
            if(((ID&0X102B0000)==0X102B0000)&&(Data[0]==0XF0))
            {
                switch(Data[1])
                {
                    case 0x53://ACDC直流侧电压
                        ACDC_DCV[(ID>>8)&0x01]=(uint16_t)(Data[6]<<8)+Data[7];
                        break;
                    case 0x54://ACDC直流侧电流
                        ACDC_DCI[(ID>>8)&0x01]=(uint16_t)(Data[6]<<8)+Data[7];
                        break;
                    case 0x59://ACDC工作状态，解析详见协议
                        ACDC_Staste0[(ID>>8)&0x01]=Data[7];
                        ACDC_Staste1[(ID>>8)&0x01]=Data[6];
                        break;
                    case 0x62://ACDC交流侧电压
                        ACDC_ACV[(ID>>8)&0x01]=(uint16_t)(Data[6]<<8)+Data[7];
                        break;
                    case 0x63://ACDC交流侧电流
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
*	函 数 名: CANx_SendData
*	功能说明: CAN发送数据函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void CANx_SendData(CAN_HandleTypeDef* hcan,uint32_t ID,uint8_t *pData,uint16_t Len)
{
    HAL_StatusTypeDef	HAL_RetVal;
    TxMeg.RTR = CAN_RTR_DATA;//数据帧
    TxMeg.IDE = CAN_ID_EXT;  //扩展帧
    TxMeg.StdId=ID;
    TxMeg.ExtId=ID;
    TxMeg.DLC=Len;
    //TxMeg.TransmitGlobalTime = DISABLE;

    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1)==0);//查询邮箱是否满
    HAL_RetVal=HAL_CAN_AddTxMessage(&hcan1,&TxMeg,pData,(uint32_t*)CAN_TX_MAILBOX0);
}
/*
*********************************************************************************************************
*	函 数 名: ACDC_ONOFF_CMD
*	功能说明: ACDC开关机命令
*	形    参: addr:模块地址 ONOFF：1开机，0关机
*	返 回 值: 无
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
*	函 数 名: ACDC_Fan_Speed_CMD
*	功能说明: 风扇调速命令
*	形    参: addr：为模块地址码(0x00~0x0F)或广播识别码(0xFF) Speed：0~100
*	返 回 值: 无
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
*	函 数 名: ACDC_DCV_Query_CMD
*	功能说明: ACDC直流侧电压查询
*	形    参: addr:模块地址
*	返 回 值: 无
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
*	函 数 名: ACDC_DCI_Query_CMD
*	功能说明: ACDC直流侧电流查询
*	形    参: addr:模块地址
*	返 回 值: 无
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
*	函 数 名: ACDC_ACV_Query_CMD
*	功能说明: ACDC交流电压查询
*	形    参: addr:模块地址
*	返 回 值: 无
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
*	函 数 名: ACDC_ACI_Query_CMD
*	功能说明: ACDC交流电流查询
*	形    参: addr:模块地址
*	返 回 值: 无
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
*	函 数 名: ACDC_ACF_Query_CMD
*	功能说明: ACDC交流频率查询
*	形    参: addr:模块地址
*	返 回 值: 无
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
*	函 数 名: ACDC_Fault_Clear_CMD
*	功能说明: ACDC故障清除
*	形    参: addr:模块地址
*	返 回 值: 无
*********************************************************************************************************
*/
void ACDC_Fault_Clear_CMD(uint8_t addr)
{
    uint32_t ID=0x10200000|(uint16_t)(addr<<8);
    uint8_t Tdata[8]={0x00,0x79,0x00,0x00,0x00,0x00,0x00,0x00};
    CANx_SendData(&hcan1,ID,&Tdata[0],8);
}
/*****************************(END OF FILE) *********************************/
