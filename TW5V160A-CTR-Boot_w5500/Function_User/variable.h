/*
*********************************************************************************************************
*
*	模块名称 :
*	文件名称 :
*	版    本 : V1.0
*	说    明 :
*	修改记录 :
*	版本号  日期         作者       说明
*	V1.0    2020-03-02   ZYL        正式发布
*
*********************************************************************************************************
*/

#ifndef _variable_H_
#define _variable_H_

#include "main.h"

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

enum led_run_status {
    led_normal = 0x01,
    led_charge,
    led_discharge,
    led_error,
};

extern uint16_t led_control;
#define NORMAL_LED 0xffff
#define RUN_LED_G 0x0001
#define RUN_LED_R 0x0002
#define PORT8_LED_G LED0_G
#define PORT8_LED_R LED0_R
#define PORT7_LED_G 0x0004
#define PORT7_LED_R 0x0008
#define PORT6_LED_G 0x0010
#define PORT6_LED_R 0x0020
#define PORT5_LED_G 0x0040
#define PORT5_LED_R 0x0080
#define PORT1_LED_R 0x0100
#define PORT1_LED_G 0x0200
#define PORT2_LED_R 0x0400
#define PORT2_LED_G 0x0800
#define PORT3_LED_R 0x1000
#define PORT3_LED_G 0x2000
#define PORT4_LED_R 0x4000
#define PORT4_LED_G 0x8000

#define CH1_LED_G PORT6_LED_G
#define CH1_LED_R PORT6_LED_R
#define CH2_LED_G PORT4_LED_G
#define CH2_LED_R PORT4_LED_R
#define CH3_LED_G PORT2_LED_G
#define CH3_LED_R PORT2_LED_R

#define CH4_LED_G PORT8_LED_G
#define CH4_LED_R PORT8_LED_R

void led_run_setting(uint8_t ch, uint8_t status, uint16_t* led_value);

void ip_scanning(void);

#endif

/***************************** (END OF FILE) *********************************/
