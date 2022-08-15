/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp.c
** Author      :  
** Date        : 2022-01-17
** Description : This file is used to initialize and drive the hardware 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          Í·ÎÄ¼þÒýÓÃÇø
**********************************************************************************************************/
#include "bsp_pwm.h"
#include "math.h"
#include "stdlib.h"

/**********************************************************************************************************
**                                         È«¾Ö±äÁ¿¶¨ÒåÇø
**********************************************************************************************************/
float FanDuty_set = 0.5;

/**********************************************************************************************************
**                                           º¯ÊýÉùÃ÷Çø
**********************************************************************************************************/

/**********************************************************************************************************
**                                           Ó¦ÓÃº¯ÊýÇø
**********************************************************************************************************/

void TIM6_4_PWMB_Config(void)
{
	uint32_t                       u32Period;
	uint32_t                       u32Compare;
	stc_tmr6_basecnt_cfg_t         stcTIM6BaseCntCfg;
	stc_tmr6_port_output_cfg_t     stcTIM6PortOutCfg;
	stc_gpio_init_t                stcGpioInit;
	stc_irq_signin_config_t        stcIrqRegiConf;
	stc_tmr6_buf_func_cfg_t        stcBufCfg;

	(void)TMR6_BaseCntStructInit(&stcTIM6BaseCntCfg);
	(void)TMR6_PortOutputStructInit(&stcTIM6PortOutCfg);
	(void)GPIO_StructInit(&stcGpioInit);
	(void)TMR6_BufFuncStructInit(&stcBufCfg);

	PWC_Fcg2PeriphClockCmd(PWC_FCG2_TMR6_4, Enable);

	/* Timer6 PWM output port configuration */
	GPIO_SetFunc(FAN_TMR6_4_PWMB_PORT, FAN_TMR6_4_PWMB_PIN, GPIO_FUNC_3_TIM64_PWMB, PIN_SUBFUNC_DISABLE);
	
	TMR6_DeInit(M4_TMR6_4);
	/* Timer6 general count function configuration */
	stcTIM6BaseCntCfg.u32CntMode = TMR6_MODE_SAWTOOTH;
	stcTIM6BaseCntCfg.u32CntDir = TMR6_CNT_INCREASE;
	stcTIM6BaseCntCfg.u32CntClkDiv = TMR6_CLK_PCLK0_DIV128;	//240/16=15Mhz
	stcTIM6BaseCntCfg.u32CntStpAftOvf = TMR6_CNT_CONTINUOUS;
	(void)TMR6_Init(M4_TMR6_4, &stcTIM6BaseCntCfg);

	/* Period register set */
	u32Period = 600 - 1; //15Mhz: 400->37.5K 600->25K ÆµÂÊ
	TMR6_SetPeriodReg(M4_TMR6_4, TMR6_PERIOD_REG_A, u32Period);

	/* Set General Compare RegisterA Value */
	u32Compare = 400 - 1;
	TMR6_SetGenCmpReg(M4_TMR6_4, TMR6_CMP_REG_B, u32Compare); /* General comprare register B */
	TMR6_SetGenCmpReg(M4_TMR6_4, TMR6_CMP_REG_D, u32Compare); /* General comprare register D, as buffer register for GCMAR */

	/* General compare buffer function configurate */
	stcBufCfg.u32BufFunCmd = TMR6_BUF_FUNC_ON;
	stcBufCfg.u32BufNum = TMR6_BUF_FUNC_SINGLE;
	stcBufCfg.u32BufTransTim = TMR6_BUF_TRANS_TIM_OVERFLOW;
	(void)TMR6_GenCmpBufCfg(M4_TMR6_4, TMR6_CH_B, &stcBufCfg);

	 /* Configurate PWM output */
	stcTIM6PortOutCfg.u32NextPeriodForceSta = TMR6_FORCE_PORT_OUTPUT_INVALID;
	stcTIM6PortOutCfg.u32DownCntMatchAnotherCmpRegSta = TMR6_PORT_OUTPUT_STA_HOLD;
	stcTIM6PortOutCfg.u32UpCntMatchAnotherCmpRegSta = TMR6_PORT_OUTPUT_STA_HOLD;
	stcTIM6PortOutCfg.u32DownCntMatchCmpRegSta = TMR6_PORT_OUTPUT_STA_HIGH;
	stcTIM6PortOutCfg.u32UpCntMatchCmpRegSta = TMR6_PORT_OUTPUT_STA_LOW;
	stcTIM6PortOutCfg.u32UnderflowSta = TMR6_PORT_OUTPUT_STA_HOLD;
	stcTIM6PortOutCfg.u32OverflowSta = TMR6_PORT_OUTPUT_STA_HIGH;
	stcTIM6PortOutCfg.u32StopSta = TMR6_PORT_OUTPUT_STA_HIGH;
	stcTIM6PortOutCfg.u32StartSta = TMR6_PORT_OUTPUT_STA_HIGH;
	
	(void)TMR6_PortOutputConfig(M4_TMR6_4, TMR6_IO_PWMB, &stcTIM6PortOutCfg);
	TMR6_SetFunc(M4_TMR6_4, TMR6_CH_B, TMR6_PORT_COMPARE_OUTPUT);

	/* Start timer6 */
	TMR6_CountCmd(M4_TMR6_4, Enable);
}

/**********************************************************************************************************
**	º¯ Êý Ãû : FanPwmSet
**	¹¦ÄÜËµÃ÷ : ÉèÖÃ·çÉÈpwmÖµ
**	ÐÎ    ²Î : pwmval - ÉèÖÃpwmÊýÖµ
**	·µ »Ø Öµ : ÎÞ
**********************************************************************************************************/
//void FanPwmSet(uint32_t pwmval)
//{	
//	 TMR6_SetGenCmpReg(M4_TMR6_4, TMR6_CMP_REG_D, pwmval); 	//ÉèÖÃPWMÊä³ö
//}

/**********************************************************************************************************
**	º¯ Êý Ãû : ArrDataMax_Get
**	¹¦ÄÜËµÃ÷ : Êý×é×î´óÖµµÄ»ñÈ¡
**	ÐÎ    ²Î : Arr - Êý×éÃû£¬len - lenÊý×é³¤¶È
**	·µ »Ø Öµ : ÎÞ
**********************************************************************************************************/
float ArrDataMax_Get(float *Arr,uint8_t len)
{
	uint8_t i = 0;
	float ArrMax = fabs(Arr[0]);
	
	for(i =0;i < len;i++)
	{
		if(fabs(Arr[i]) > ArrMax)
		{
			ArrMax = fabs(Arr[i]);
		}
	}
	return ArrMax;
}

void FanPwmSet(float duty)
{        
	uint32_t pwmval = 600*(1 - duty);        
	TMR6_SetGenCmpReg(M4_TMR6_4, TMR6_CMP_REG_D, pwmval);
}

void FanPwmControl(void)
{
	float MaxTempValue = 0;
	float FanDuty;
	MaxTempValue = ArrDataMax_Get(Tier2.sampleCal.Iout,8);
	FanDuty = MaxTempValue/141;// 120*0.85最大占空比85%
	
	if(FanDuty > 1)
	{
		FanDuty = 1;
	}
	if(FanDuty < 0.3)
	{
		FanDuty = 0.3;
	}
	if(FanDuty_set > FanDuty)
	{
		FanDuty_set -= 0.05;
		if(FanDuty_set  < FanDuty)
		{
			FanDuty_set = FanDuty;        
		}
	}
	else if(FanDuty_set < FanDuty)
	{
		FanDuty_set += 0.05;
		if(FanDuty_set  > FanDuty)
		{
			FanDuty_set = FanDuty;        
		}
	}
	
	FanPwmSet(FanDuty_set);                
}


