#ifndef __W5500_H__
#define __W5500_H__

#include "sys.h"
#include "stm32f4xx_hal.h"

/* ����GPIO�˿� */
#define W5500_PORT_CS              GPIOI
#define W5500_PIN_CS               GPIO_PIN_6

/* ���������0����1�ĺ� */
#define W5500_CS_0()           HAL_GPIO_WritePin(W5500_PORT_CS, W5500_PIN_CS, GPIO_PIN_RESET);
#define W5500_CS_1()           HAL_GPIO_WritePin(W5500_PORT_CS, W5500_PIN_CS, GPIO_PIN_SET);


extern int32_t InitW5500(void);
#endif
