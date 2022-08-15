#include "eth_ksz8041.h"
#include "ethernetif.h"
#include "eth_process.h"


ETH_HandleTypeDef ETH_Handler;       //Global Ethernet handle
// ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB];
// ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB];
// uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];
// uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;/* Ethernet Rx MA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;/* Ethernet Tx DMA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END; /* Ethernet Receive Buffer */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END; /* Ethernet Transmit Buffer */




//ETH 底层驱动，引脚配置
void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{
    /*----------------------------------------------------------------------------------------------
    **  ETH GPIO Configuration  ----->  RMII
    **
    **  ETH_MDIO                ----->  PA2
    **  ETH_MDC                 ----->  PC1
    **  ETH_REF_CLK             ----->  PA1
    **  ETH_CRS_DV              ----->  PA7
    **  ETH_RXD0                ----->  PC4
    **  ETH_RXD1                ----->  PC5
    **  ETH_RXEN                ----->  PI10
    **  ETH_TX_EN               ----->  PB11
    **  ETH_TXD0                ----->  PB12
    **  ETH_TXD1                ----->  PB13
    **
    ----------------------------------------------------------------------------------------------*/

    if(ethHandle->Instance == ETH) {
        GPIO_InitTypeDef GPIO_InitStruct;

        //开启相应的时钟
        __HAL_RCC_ETH_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOI_CLK_ENABLE();

        //PA1,2,7
        GPIO_InitStruct.Pin       = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;                         //推挽复用
        GPIO_InitStruct.Pull      = GPIO_NOPULL;                             //不带上下拉
        GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;                         //高速
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;                           //复用为 ETH 功能
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        //PB11,12,13
        GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        //PC1,4,5
        GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        //PI10
        // GPIO_InitStruct.Pin = GPIO_PIN_10;
        // HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(ETH_IRQn, 0, 0);           //设置网络中断优先级
        HAL_NVIC_EnableIRQ(ETH_IRQn);
    }


}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{
    /*----------------------------------------------------------------------------------------------
    **  ETH GPIO Configuration  ----->  RMII
    **
    **  ETH_MDIO                ----->  PA2
    **  ETH_MDC                 ----->  PC1
    **  ETH_REF_CLK             ----->  PA1
    **  ETH_CRS_DV              ----->  PA7
    **  ETH_RXD0                ----->  PC4
    **  ETH_RXD1                ----->  PC5
    **  ETH_RXEN                ----->  PI10
    **  ETH_TX_EN               ----->  PB11
    **  ETH_TXD0                ----->  PB12
    **  ETH_TXD1                ----->  PB13
    **
    ----------------------------------------------------------------------------------------------*/

    if(ethHandle->Instance == ETH) {
        __HAL_RCC_ETH_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7);
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5);
        HAL_GPIO_DeInit(GPIOI, GPIO_PIN_10);

        HAL_NVIC_DisableIRQ(ETH_IRQn);
    }
}

//KSZ8041 初始化
HAL_StatusTypeDef ETH_KSZ8041_Init(void)
{
    HAL_StatusTypeDef ret;
    u8 MAC_Address[6];

    HAL_ETH_DeInit(&ETH_Handler);

    ETH->DMABMR |= ETH_DMABMR_SR;

    MAC_Address[0] = MAC_ADDR0;
    MAC_Address[1] = MAC_ADDR1;
    MAC_Address[2] = MAC_ADDR2;
    MAC_Address[3] = MAC_ADDR3;
    MAC_Address[4] = MAC_ADDR4;
    MAC_Address[5] = MAC_ADDR5;

    ETH_Handler.Instance             = ETH;
    ETH_Handler.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;    //使能自动协商模式
    ETH_Handler.Init.Speed           = ETH_SPEED_100M;                //配置网络速度，开启自动协商模式时次配置无效
    ETH_Handler.Init.DuplexMode      = ETH_MODE_FULLDUPLEX;           //配置全双工模式，开启自动协商时此配置无效
    ETH_Handler.Init.PhyAddress      = KSZ8041_PHY_ADDRESS;           //PHY 地址
    ETH_Handler.Init.MACAddr         = MAC_Address;                   //MAC 地址
    ETH_Handler.Init.RxMode          = ETH_RXINTERRUPT_MODE;          //中断接收模式
    ETH_Handler.Init.ChecksumMode    = ETH_CHECKSUM_BY_HARDWARE;      //硬件帧校验
    ETH_Handler.Init.MediaInterface  = ETH_MEDIA_INTERFACE_RMII;      //RMII 接口

    ret = HAL_ETH_Init(&ETH_Handler);

    HAL_ETH_DMATxDescListInit(&ETH_Handler, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);//初始化发送描述符
    HAL_ETH_DMARxDescListInit(&ETH_Handler, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);//初始化接收描述符

    return ret;
}

//读取 PHY 寄存器的值
u32 KSZ8041_ReadPHY(u16 reg)
{
    u32 regval;
    HAL_ETH_ReadPHYRegister(&ETH_Handler, reg, &regval);
    return regval;
}

//向 PHY 的寄存器里写值
void KSZ8041_WriterPHY(u16 reg, u16 value)
{
    u32 temp = value;
    HAL_ETH_WritePHYRegister(&ETH_Handler, reg, temp);
}

//获取接收到的帧长度
u32 ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc)
{
    u32 frameLength = 0;
    if(((DMARxDesc->Status&ETH_DMARXDESC_OWN)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_ES)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_LS)!=(uint32_t)RESET))
    {
        frameLength=((DMARxDesc->Status&ETH_DMARXDESC_FL)>>ETH_DMARXDESC_FRAME_LENGTHSHIFT);
    }
    return frameLength;
}

//ETH 中断服务函数
void ETH_IRQHandler(void)
{
    // HAL_ETH_IRQHandler(&ETH_Handler);

    while(ETH_GetRxPktSize(ETH_Handler.RxDesc))
    {
        ethernetif_input(&default_netif);
    }
    //清除中断标志位
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_NIS);    //清除DMA中断标志位
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_R);      //清除DMA接收中断标志位
}

// void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
// {

// }

// void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
// {

// }

// void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth)
// {

// }
