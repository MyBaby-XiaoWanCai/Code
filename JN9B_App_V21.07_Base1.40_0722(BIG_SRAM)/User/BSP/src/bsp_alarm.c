/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp.c
** Author      :
** Date        : 2022-01-20
** Description : This file is used to initialize and drive the hardware
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
extern PcaDataType Pca9555_Data;
uint8_t Fault1TypeArr[4] = {0x10,0x20,0x40,0x80};	//OCP_PG、OVP_PG、Alarn_PG
uint8_t PG_Fault_Flg[8] = {0x00};
uint64_t PG_Fault_Weaver_Record[8] = {0x00};	//内部记录硬件故障-非上抛数据

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : 硬件故障触发后故障类型读取
**	功能说明 : HardwareFault1TypeRead()
**	形    参 : CHx 通道 1-8
**	返 回 值 : 无
**********************************************************************************************************/
void HardwareFaultTypeRead(uint8_t CHx)
{
		Pca9548A_Select(I2C_PCA9548,CHx,PCA_9555_ON);	//打开对应故障读取通道

    if(Bom_Vers == C042C_BOOM_VER)
    {
        Pca9555_RegRead(I2C_PCA9548, PB_Addr_Arr[CHx], Pca9555_Data.Power_Board[CHx].ReadData);	//故障信息读取
    }
    else
//		if(vers_mana.Bom_Vers == C042B_BOOM_VER)
    {
        Pca9555_RegRead(I2C_PCA9548, PCA9555_POWER_ADDR, Pca9555_Data.Power_Board[CHx].ReadData);	//故障信息读取
    }

    if((Pca9555_Data.Power_Board[CHx].ReadData[1] & Fault1TypeArr[1]) == 0x00)	//通道CHx OCP故障
    {
			PG_Fault_Weaver_Record[CHx] |= IntelProtect_OCP;
			PG_Fault_Weaver_Record[CHx] |= IntelProtect_HPA;	//记录硬件故障
			
			if((CHANNEL_CFG == 0x04 || CHANNEL_CFG == 0x01) && FCTRun_State == 0x00)	//并机模式且不处于FCT状态
			{
				if(CHANNEL_CFG == 0x04)	//240A
				{
					if(CHx == 0x00 || CHx == 0x01)
					{
						IP_Alarm_Flag[0] |= IntelProtect_OCP;
						IP_Alarm_Flag[0] |= IntelProtect_HPA;//硬件故障
					}
					else
					if(CHx == 0x02 || CHx == 0x03)
					{
						IP_Alarm_Flag[1] |= IntelProtect_OCP;
						IP_Alarm_Flag[1] |= IntelProtect_HPA;//硬件故障
					}
					else
					if(CHx == 0x04 || CHx == 0x05)
					{
						IP_Alarm_Flag[2] |= IntelProtect_OCP;
						IP_Alarm_Flag[2] |= IntelProtect_HPA;//硬件故障
					}
					else
					if(CHx == 0x06 || CHx == 0x07)
					{
						IP_Alarm_Flag[3] |= IntelProtect_OCP;
						IP_Alarm_Flag[3] |= IntelProtect_HPA;//硬件故障
					}
				}
				else 
				if(CHANNEL_CFG == 0x01)	//600A
				{
					IP_Alarm_Flag[0] |= IntelProtect_OCP;
					IP_Alarm_Flag[0] |= IntelProtect_HPA;//硬件故障
				}
				
				Chg_Dis_Port_Cfg(CHx + 1, PORT_OUT_ENABLE);	//检测到过压信号时，将该通道的EN信号置高
			}
			else	//单机模式
			{
        IP_Alarm_Flag[CHx] |= IntelProtect_OCP;
        IP_Alarm_Flag[CHx] |= IntelProtect_HPA;//硬件故障
			}
    }
    else 
		if((Pca9555_Data.Power_Board[CHx].ReadData[1] & Fault1TypeArr[2]) == 0x00) //通道CHx OVP故障
    {
			PG_Fault_Weaver_Record[CHx] |= IntelProtect_OVP;
			PG_Fault_Weaver_Record[CHx] |= IntelProtect_HPA;	//记录硬件故障
			
			if((CHANNEL_CFG == 0x04 || CHANNEL_CFG == 0x01) && FCTRun_State == 0x00)	//并机模式且不处于FCT状态
			{
				if(CHANNEL_CFG == 0x04)
				{
					if(CHx == 0x00 || CHx == 0x01)
					{
						IP_Alarm_Flag[0] |= IntelProtect_OVP;
						IP_Alarm_Flag[0] |= IntelProtect_HPA;//硬件故障				
					}
					else 
					if(CHx == 0x02 || CHx == 0x03)
					{
						IP_Alarm_Flag[1] |= IntelProtect_OVP;
						IP_Alarm_Flag[1] |= IntelProtect_HPA;//硬件故障							
					}
					if(CHx == 0x04 || CHx == 0x05)
					{
						IP_Alarm_Flag[2] |= IntelProtect_OVP;
						IP_Alarm_Flag[2] |= IntelProtect_HPA;//硬件故障							
					}
					if(CHx == 0x06 || CHx == 0x07)
					{
						IP_Alarm_Flag[3] |= IntelProtect_OVP;
						IP_Alarm_Flag[3] |= IntelProtect_HPA;//硬件故障							
					}
				}
				else 
				if(CHANNEL_CFG == 0x01)
				{
						IP_Alarm_Flag[0] |= IntelProtect_OVP;
						IP_Alarm_Flag[0] |= IntelProtect_HPA;//硬件故障		
				}
			}
			else	//单机模式
			{
        IP_Alarm_Flag[CHx] |= IntelProtect_OVP;
        IP_Alarm_Flag[CHx] |= IntelProtect_HPA;//硬件故障
			}
			
			Chg_Dis_Port_Cfg(CHx + 1, PORT_OUT_ENABLE);	//检测到过压信号时，将该通道的EN信号置高
    }
    else 
		if((Pca9555_Data.Power_Board[CHx].ReadData[1] & Fault1TypeArr[3]) == 0x00) //通道CHx Alarn故障
    {
			PG_Fault_Weaver_Record[CHx] |= IntelProtect_NEG;
			PG_Fault_Weaver_Record[CHx] |= IntelProtect_HPA;	//记录硬件故障
			
			if((CHANNEL_CFG == 0x04 || CHANNEL_CFG == 0x01) && FCTRun_State == 0x00)	//并机模式且不处于FCT状态
			{
				if(CHANNEL_CFG == 0x04)
				{
					if(CHx == 0x00 || CHx == 0x01)
					{
						IP_Alarm_Flag[0] |= IntelProtect_NEG;
						IP_Alarm_Flag[0] |= IntelProtect_HPA;//硬件故障					
					}
					else 
					if(CHx == 0x02 || CHx == 0x03)
					{
						IP_Alarm_Flag[0] |= IntelProtect_NEG;
						IP_Alarm_Flag[0] |= IntelProtect_HPA;//硬件故障								
					}
					if(CHx == 0x04 || CHx == 0x05)
					{
						IP_Alarm_Flag[0] |= IntelProtect_NEG;
						IP_Alarm_Flag[0] |= IntelProtect_HPA;//硬件故障								
					}
					if(CHx == 0x06 || CHx == 0x07)
					{
						IP_Alarm_Flag[0] |= IntelProtect_NEG;
						IP_Alarm_Flag[0] |= IntelProtect_HPA;//硬件故障									
					}
				}
				else
				if(CHANNEL_CFG == 0x01)
				{
					IP_Alarm_Flag[0] |= IntelProtect_NEG;
					IP_Alarm_Flag[0] |= IntelProtect_HPA;//硬件故障		
				}
			}
			else
			{
        IP_Alarm_Flag[CHx] |= IntelProtect_NEG;
        IP_Alarm_Flag[CHx] |= IntelProtect_HPA;//硬件故障
			}
    }
		else
		{
			if( (PG_Fault_Flg[CHx] == 0x01) && (Tier2.sampleCal.Tmod[CHx] >= 83) )
			{
				PG_Fault_Weaver_Record[CHx] |= IntelProtect_OTP;
				PG_Fault_Weaver_Record[CHx] |= IntelProtect_HPA;	//记录硬件故障
				
				if( (CHANNEL_CFG == 0x04 || CHANNEL_CFG == 0x01) && (FCTRun_State == 0x00) )	//并机模式且不处于FCT状态
				{
					if(CHANNEL_CFG == 0x04)	//240A
					{
						if(CHx == 0x00 || CHx == 0x01)
						{
							IP_Alarm_Flag[0] |= IntelProtect_OTP;
							IP_Alarm_Flag[0] |= IntelProtect_HPA;//硬件故障
						}
						else
						if(CHx == 0x02 || CHx == 0x03)
						{
							IP_Alarm_Flag[1] |= IntelProtect_OTP;
							IP_Alarm_Flag[1] |= IntelProtect_HPA;//硬件故障
						}
						else
						if(CHx == 0x04 || CHx == 0x05)
						{
							IP_Alarm_Flag[2] |= IntelProtect_OTP;
							IP_Alarm_Flag[2] |= IntelProtect_HPA;//硬件故障
						}
						else
						if(CHx == 0x06 || CHx == 0x07)
						{
							IP_Alarm_Flag[3] |= IntelProtect_OTP;
							IP_Alarm_Flag[3] |= IntelProtect_HPA;//硬件故障
						}
					}
					else 
					if(CHANNEL_CFG == 0x01)	//600A
					{
						IP_Alarm_Flag[0] |= IntelProtect_OTP;
						IP_Alarm_Flag[0] |= IntelProtect_HPA;//硬件故障
					}			
				}
				else
				{		
					IP_Alarm_Flag[CHx] |= IntelProtect_OTP;
					IP_Alarm_Flag[CHx] |= IntelProtect_HPA;//硬件故障
				}		
			}
		}

		if( (PG_Fault_Weaver_Record[CHx] & IntelProtect_OCP) 
		 || (PG_Fault_Weaver_Record[CHx] & IntelProtect_OVP) 
		 || (PG_Fault_Weaver_Record[CHx] & IntelProtect_NEG)
		 || (PG_Fault_Weaver_Record[CHx] & IntelProtect_OTP) )
		{
				PG_Fault_Flg[CHx] = 0x00;	//硬件故障读取完成、标志清除
		}			
}

/**********************************************************************************************************
**	函 数 名 : PowerFault1_EXINT_IrqCallback
**	功能说明 : 功率板1故障中断回调函数
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void PowerFault1_EXINT_IrqCallback(void)
{
    static uint16_t powfau1_cnt = 0;

    if(Set == EXINT_GetExIntSrc(POWERFAULT_1_EXINT_CH))
    {
        EXINT_ClrExIntSrc(POWERFAULT_1_EXINT_CH);

        powfau1_cnt++;

        /* Disable interrupt */
        NVIC_DisableIRQ(POWERFAULT_1_IRQn);

        /* 防止一直触发数据中断，导致其他任务无法正常运行 */
//		if(0x00 == powfau1_cnt%4)
        {
            //处理函数
            PG_Fault_Flg[PCA9548_CH1] = 0x01;
        }

        /* Enable interrupt */
        NVIC_EnableIRQ(POWERFAULT_1_IRQn);
    }
    else
    {
        //do nothing;
    }
}

/**********************************************************************************************************
**	函 数 名 : PowerFault2_EXINT_IrqCallback
**	功能说明 : 功率板2中断回调函数
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void PowerFault2_EXINT_IrqCallback(void)
{
    static uint16_t powfau2_cnt = 0;

    if(Set == EXINT_GetExIntSrc(POWERFAULT_2_EXINT_CH))
    {
        EXINT_ClrExIntSrc(POWERFAULT_2_EXINT_CH);

        powfau2_cnt++;

        /* Disable interrupt */
        NVIC_DisableIRQ(POWERFAULT_2_IRQn);

        /* 防止一直触发数据中断，导致其他任务无法正常运行 */
//		if(0x00 == powfau2_cnt%4)
        {
            //处理函数
            PG_Fault_Flg[PCA9548_CH2] = 0x01;
        }

        /* Enable interrupt */
        NVIC_EnableIRQ(POWERFAULT_2_IRQn);
    }
    else
    {
        //do nothing;
    }
}

/**********************************************************************************************************
**	函 数 名 : PowerFault3_EXINT_IrqCallback
**	功能说明 : 功率板3中断回调函数
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void PowerFault3_EXINT_IrqCallback(void)
{
    static uint16_t powfau3_cnt = 0;

    if(Set == EXINT_GetExIntSrc(POWERFAULT_3_EXINT_CH))
    {
        EXINT_ClrExIntSrc(POWERFAULT_3_EXINT_CH);

        powfau3_cnt++;

        /* Disable interrupt */

        NVIC_DisableIRQ(POWERFAULT_3_IRQn);

        /* 防止一直触发数据中断，导致其他任务无法正常运行 */
//		if(powfau3_cnt%4 == 0x00)
        {
            //处理函数
            PG_Fault_Flg[PCA9548_CH3] = 0x01;
        }

        /* Enable interrupt */
        NVIC_EnableIRQ(POWERFAULT_3_IRQn);
    }
    else
    {
        //do nothing;
    }
}

/**********************************************************************************************************
**	函 数 名 : PowerFault4_EXINT_IrqCallback
**	功能说明 : 功率板4中断回调函数
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void PowerFault4_EXINT_IrqCallback(void)
{
    static uint16_t powfau4_cnt = 0;

    if(Set == EXINT_GetExIntSrc(POWERFAULT_4_EXINT_CH))
    {
        EXINT_ClrExIntSrc(POWERFAULT_4_EXINT_CH);

        powfau4_cnt++;

        /* Disable interrupt */
        NVIC_DisableIRQ(POWERFAULT_4_IRQn);

        /* 防止一直触发数据中断，导致其他任务无法正常运行 */
//		if(powfau4_cnt%4 == 0x00)
        {
            //故障信号读取
            PG_Fault_Flg[PCA9548_CH4] = 0x01;
        }

        /* Enable interrupt */
        NVIC_EnableIRQ(POWERFAULT_4_IRQn);
    }
    else
    {
        //do nothing;
    }
}

/**********************************************************************************************************
**	函 数 名 : PowerFault5_EXINT_IrqCallback
**	功能说明 : 功率板5中断回调函数
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void PowerFault5_EXINT_IrqCallback(void)
{
    static uint16_t powfau5_cnt = 0;

    if(Set == EXINT_GetExIntSrc(POWERFAULT_5_EXINT_CH))
    {
        EXINT_ClrExIntSrc(POWERFAULT_5_EXINT_CH);

        powfau5_cnt++;

        /* Disable interrupt */
        NVIC_DisableIRQ(POWERFAULT_5_IRQn);

        /* 防止一直触发数据中断，导致其他任务无法正常运行 */
//		if(powfau5_cnt%4 == 0x00)
        {
            //处理函数
            PG_Fault_Flg[PCA9548_CH5] = 0x01;
        }

        /* Enable interrupt */
        NVIC_EnableIRQ(POWERFAULT_5_IRQn);
    }
    else
    {
        //do nothing;
    }
}

/**********************************************************************************************************
**	函 数 名 : PowerFault6_EXINT_IrqCallback
**	功能说明 : 功率板6中断回调函数
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void PowerFault6_EXINT_IrqCallback(void)
{
    static uint16_t powfau6_cnt = 0;

    if(Set == EXINT_GetExIntSrc(POWERFAULT_6_EXINT_CH))
    {
        EXINT_ClrExIntSrc(POWERFAULT_6_EXINT_CH);

        powfau6_cnt++;

        /* Disable interrupt */
        NVIC_DisableIRQ(POWERFAULT_6_IRQn);

        /* 防止一直触发数据中断，导致其他任务无法正常运行 */
//		if(powfau6_cnt%4 == 0x00)
        {
            //处理函数
            PG_Fault_Flg[PCA9548_CH6] = 0x01;
        }

        /* Enable interrupt */
        NVIC_EnableIRQ(POWERFAULT_6_IRQn);
    }
    else
    {
        //do nothing;
    }
}

/**********************************************************************************************************
**	函 数 名 : PowerFault7_EXINT_IrqCallback
**	功能说明 : 功率板7中断回调函数
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void PowerFault7_EXINT_IrqCallback(void)
{
    static uint16_t powfau7_cnt = 0;

    if(Set == EXINT_GetExIntSrc(POWERFAULT_7_EXINT_CH))
    {
        EXINT_ClrExIntSrc(POWERFAULT_7_EXINT_CH);

        powfau7_cnt++;

        /* Disable interrupt */
        NVIC_DisableIRQ(POWERFAULT_7_IRQn);

        /* 防止一直触发数据中断，导致其他任务无法正常运行 */
//		if(powfau7_cnt%4 == 0x00)
        {
            //处理函数
            PG_Fault_Flg[PCA9548_CH7] = 0x01;
        }

        /* Enable interrupt */
        NVIC_EnableIRQ(POWERFAULT_7_IRQn);
    }
    else
    {
        //do nothing;
    }
}

/**********************************************************************************************************
**	函 数 名 : PowerFault8_EXINT_IrqCallback
**	功能说明 : 功率板8中断回调函数
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void PowerFault8_EXINT_IrqCallback(void)
{
    static uint16_t powfau8_cnt = 0;

    if(Set == EXINT_GetExIntSrc(POWERFAULT_8_EXINT_CH))
    {
        EXINT_ClrExIntSrc(POWERFAULT_8_EXINT_CH);

        powfau8_cnt++;

        /* Disable interrupt */
        NVIC_DisableIRQ(POWERFAULT_8_IRQn);

        /* 防止一直触发数据中断，导致其他任务无法正常运行 */
//		if(powfau8_cnt%4 == 0x00)
        {
            //处理函数
            PG_Fault_Flg[PCA9548_CH8] = 0x01;
        }

        /* Enable interrupt */
        NVIC_EnableIRQ(POWERFAULT_8_IRQn);
    }
    else
    {
        //do nothing;
    }
}

/**********************************************************************************************************
**	函 数 名 : PowerFaultExint_Init
**	功能说明 : 功率板故障中断IO检测初始化
**	形    参 : u8Port - 端口; u16Pin - pin脚; exint_ch - 通道; exint_src - 中断号
							 exint_irqn - 中断类型; irqcallback - 回调函数; Priority - 优先级分组
**	返 回 值 : 无
**********************************************************************************************************/
void PowerFaultExint_Init(uint8_t u8Port, uint16_t u16Pin,uint32_t Exint_ch,en_int_src_t Exint_src,IRQn_Type Exint_irqn,func_ptr_t Irqcallback,uint32_t Priority)
{
    stc_exint_init_t stcExintInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    stc_gpio_init_t stcGpioInit;

    /* GPIO config */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16ExInt = PIN_EXINT_ON;
    stcGpioInit.u16PullUp = PIN_PU_ON;
    stcGpioInit.u16PinDir = PIN_DIR_IN;	//输入
    stcGpioInit.u16PinState = PIN_STATE_SET;
    (void)GPIO_Init(u8Port, u16Pin, &stcGpioInit);
    GPIO_SetPins(u8Port,u16Pin);

    /* Exint config */
    (void)EXINT_StructInit(&stcExintInit);
    stcExintInit.u32ExIntCh = Exint_ch;
    stcExintInit.u32ExIntLvl = EXINT_TRIGGER_FALLING;
    (void)EXINT_Init(&stcExintInit);

    /* IRQ sign-in */
    stcIrqSignConfig.enIntSrc = Exint_src;
    stcIrqSignConfig.enIRQn   = Exint_irqn;
    stcIrqSignConfig.pfnCallback = Irqcallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);

    /* NVIC config */
    NVIC_ClearPendingIRQ(Exint_irqn);
    NVIC_SetPriority(Exint_irqn, Priority);
//	NVIC_DisableIRQ(Exint_irqn);
    NVIC_EnableIRQ(Exint_irqn); //使能中断
}

/**********************************************************************************************************
**	函 数 名 : PowerFault_Init
**	功能说明 : 功率板IO中断初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void PowerFault_Init(void)
{
    PowerFaultExint_Init(GPIO_PORT_POWERFAULT_1,GPIO_PIN_POWERFAULT_1,POWERFAULT_1_EXINT_CH,
                         POWERFAULT_1_INT_SRC,POWERFAULT_1_IRQn,PowerFault1_EXINT_IrqCallback,PROIORITY_GROUP_1);	//功率板硬件故障检测中断1初始化
    PowerFaultExint_Init(GPIO_PORT_POWERFAULT_2,GPIO_PIN_POWERFAULT_2,POWERFAULT_2_EXINT_CH,
                         POWERFAULT_2_INT_SRC,POWERFAULT_2_IRQn,PowerFault2_EXINT_IrqCallback,PROIORITY_GROUP_2);
    PowerFaultExint_Init(GPIO_PORT_POWERFAULT_3,GPIO_PIN_POWERFAULT_3,POWERFAULT_3_EXINT_CH,
                         POWERFAULT_3_INT_SRC,POWERFAULT_3_IRQn,PowerFault3_EXINT_IrqCallback,PROIORITY_GROUP_3);
    PowerFaultExint_Init(GPIO_PORT_POWERFAULT_4,GPIO_PIN_POWERFAULT_4,POWERFAULT_4_EXINT_CH,
                         POWERFAULT_4_INT_SRC,POWERFAULT_4_IRQn,PowerFault4_EXINT_IrqCallback,PROIORITY_GROUP_4);
    PowerFaultExint_Init(GPIO_PORT_POWERFAULT_5,GPIO_PIN_POWERFAULT_5,POWERFAULT_5_EXINT_CH,
                         POWERFAULT_5_INT_SRC,POWERFAULT_5_IRQn,PowerFault5_EXINT_IrqCallback,PROIORITY_GROUP_5);
    PowerFaultExint_Init(GPIO_PORT_POWERFAULT_6,GPIO_PIN_POWERFAULT_6,POWERFAULT_6_EXINT_CH,
                         POWERFAULT_6_INT_SRC,POWERFAULT_6_IRQn,PowerFault6_EXINT_IrqCallback,PROIORITY_GROUP_6);
    PowerFaultExint_Init(GPIO_PORT_POWERFAULT_7,GPIO_PIN_POWERFAULT_7,POWERFAULT_7_EXINT_CH,
                         POWERFAULT_7_INT_SRC,POWERFAULT_7_IRQn,PowerFault7_EXINT_IrqCallback,PROIORITY_GROUP_7);
    PowerFaultExint_Init(GPIO_PORT_POWERFAULT_8,GPIO_PIN_POWERFAULT_8,POWERFAULT_8_EXINT_CH,
                         POWERFAULT_8_INT_SRC,POWERFAULT_8_IRQn,PowerFault8_EXINT_IrqCallback,PROIORITY_GROUP_8);
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
