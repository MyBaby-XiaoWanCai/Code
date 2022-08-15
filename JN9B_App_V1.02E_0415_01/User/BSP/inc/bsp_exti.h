#ifndef __BSP_EXTI_H
#define __BSP_EXTI_H

#include <stdint.h>
#include "hc32_ddl.h"
#include "bsp_ad7124.h"
#include "bsp_ad7175.h"

#define AD7175_EXTI_PORT      (GPIO_PORT_E)
#define AD7175_EXTI_PIN       (GPIO_PIN_00)
#define AD7175_EXINT_CH       (EXINT_CH00)
#define AD7175_INT_SRC        (INT_PORT_EIRQ0)
#define AD7175_IRQn           (Int001_IRQn)

#define AD7124_EXTI_PORT      (GPIO_PORT_E)
#define AD7124_EXTI_PIN       (GPIO_PIN_01)
#define AD7124_EXINT_CH       (EXINT_CH01)
#define AD7124_INT_SRC        (INT_PORT_EIRQ1)
#define AD7124_IRQn           (Int002_IRQn)

void AD7175_EXINT_Init(void);
void AD7124_EXINT_Init(void);

#endif



