/*
*********************************************************************************************************
*
*	模块名称 :
*	文件名称 :
*	版    本 : V1.0
*	说    明 :
*
*	修改记录 :
*		版本号  日期         作者     说明
*		V1.0    2020-03-02   ZYL      正式发布
*********************************************************************************************************
*/

#include "main.h"

void led_run_setting(uint8_t ch, uint8_t status, uint16_t* led_value)
{
    switch(ch) {
        case 0:
        {
            switch(status) {
                case led_normal:
                {
                    *led_value = (*led_value | RUN_LED_R) & (~RUN_LED_G);
                    pca9555SetOutput(*led_value);
                    break;
                }
                case led_error:
                {
                    *led_value = (*led_value | RUN_LED_G) & (~RUN_LED_R);
                    pca9555SetOutput(*led_value);
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case CH_1:
        {
            switch(status) {
                case led_normal:
                {
                    *led_value = (*led_value | CH1_LED_R) & (~CH1_LED_G);
                    pca9555SetOutput(*led_value);
                    break;
                }
                case led_charge:
                case led_discharge:
                {
                    *led_value = (*led_value | CH1_LED_R) ^ CH1_LED_G;
                    pca9555SetOutput(*led_value);
                    break;
                }
                case led_error:
                {
                    *led_value = (*led_value | CH1_LED_G) & (~CH1_LED_R);
                    pca9555SetOutput(*led_value);
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case CH_2:
        {
            switch(status) {
                case led_normal:
                {
                    *led_value = (*led_value | CH2_LED_R) & (~CH2_LED_G);
                    pca9555SetOutput(*led_value);
                    break;
                }
                case led_charge:
                case led_discharge:
                {
                    *led_value = (*led_value | CH2_LED_R) ^ CH2_LED_G;
                    pca9555SetOutput(*led_value);
                    break;
                }
                case led_error:
                {
                    *led_value = (*led_value | CH2_LED_G) & (~CH2_LED_R);
                    pca9555SetOutput(*led_value);
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case CH_3:
        {
            switch(status) {
                case led_normal:
                {
                    *led_value = (*led_value | CH3_LED_R) & (~CH3_LED_G);
                    pca9555SetOutput(*led_value);
                    break;
                }
                case led_charge:
                case led_discharge:
                {
                    *led_value = (*led_value | CH3_LED_R) ^ CH3_LED_G;
                    pca9555SetOutput(*led_value);
                    break;
                }
                case led_error:
                {
                    *led_value = (*led_value | CH3_LED_G) & (~CH3_LED_R);
                    pca9555SetOutput(*led_value);
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case CH_4:
        {
            switch(status) {
                case led_normal:
                {
                    CH4_LED_R = 0;
                    CH4_LED_G = 1;
                    break;
                }
                case led_charge:
                case led_discharge:
                {
                    CH4_LED_R = 0;
                    CH4_LED_G = ~CH4_LED_G;
                    break;
                }
                case led_error:
                {
                    CH4_LED_R = 1;
                    CH4_LED_G = 0;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
}

void ip_scanning(void)
{
    uint8_t temp8 = 0;

    if(ADDR1 == 1) {
        HAL_Delay(10);
        if(ADDR1 == 1) {
            temp8 += (0x01<<2);
        }
    }
    if(ADDR2 == 1) {
        HAL_Delay(10);
        if(ADDR2 == 1) {
            temp8 += (0x01<<1);
        }
    }
    if(ADDR3 == 1) {
        HAL_Delay(10);
        if(ADDR3 == 1) {
            temp8 += (0x01<<0);
        }
    }
    temp8 += 1;
    IP_ADDRESS[2] = temp8;


    temp8 = 0;
    if(ADDR4 == 1) {
        HAL_Delay(10);
        if(ADDR4 == 1) {
            temp8 += (0x01<<4);
        }
    }
    if(ADDR5 == 1) {
        HAL_Delay(10);
        if(ADDR5 == 1) {
            temp8 += (0x01<<3);
        }
    }
    if(ADDR6 == 1) {
        HAL_Delay(10);
        if(ADDR6 == 1) {
            temp8 += (0x01<<2);
        }
    }
    if(ADDR7 == 1) {
        HAL_Delay(10);
        if(ADDR7 == 1) {
            temp8 += (0x01<<1);
        }
    }
    if(ADDR8 == 1) {
        HAL_Delay(10);
        if(ADDR8 == 1) {
            temp8 += (0x01<<0);
        }
    }
    temp8 += 11;
    IP_ADDRESS[3] = temp8;

    // if((temp8 != 0) && (temp8 != 1) && (temp8 != 255)) {
    //     IP_ADDRESS[3] = temp8;
    // }

}

/*****************************(END OF FILE) *********************************/