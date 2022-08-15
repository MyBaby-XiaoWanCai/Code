#ifndef __ETH_KSZ8041_H__
#define __ETH_KSZ8041_H__

#include "sys.h"
#include "stm32f4xx_hal.h"

extern ETH_HandleTypeDef ETH_Handler;
extern ETH_DMADescTypeDef DMARxDscrTab[ETH_RXBUFNB];
extern ETH_DMADescTypeDef DMATxDscrTab[ETH_TXBUFNB];
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];
extern uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];

HAL_StatusTypeDef ETH_KSZ8041_Init(void);
u32 KSZ8041_ReadPHY(u16 reg);
void KSZ8041_WriterPHY(u16 reg, u16 value);
u32 ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc);

#endif
