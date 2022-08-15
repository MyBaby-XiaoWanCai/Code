#include "W5500.h"
int32_t InitW5500(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    int32_t ret = 0;
    __HAL_RCC_GPIOI_CLK_ENABLE();                   /* 打开GPIOI时钟 */

    GPIO_Initure.Pin =  W5500_PIN_CS;               /*输出引脚 */
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;        /* 设为推挽模式 */
    GPIO_Initure.Pull = GPIO_NOPULL;                /* 上下拉电阻不使能 */
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;           /* IO口最大速度 */
    HAL_GPIO_Init(GPIOI, &GPIO_Initure);            //GPIOI
    W5500_CS_1();
    return ret;
}