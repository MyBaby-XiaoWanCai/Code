/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_AD7606.c
** Author      : wangliang
** Date        : 2021-11-05
** Description : bsp_dac80508 source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
AD7606_Sel whoIsConV = NoBody;
AD7606_Object AD7606_A0 = {AD7606_Iout,{0},1,{0},{0}}; //AD7606_Vsense
AD7606_Object AD7606_A1 = {AD7606_Vsense,{0},1,{0},{0}}; //

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void AD7606_Busy0_EXINT_IrqCallback(void);
static void AD7606_Busy1_EXINT_IrqCallback(void);

/**********************************************************************************************************
**	函 数 名 : AD7606_SpiInit()
**	功能说明 : AD7606 SPI配置初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void AD7606_SpiInit(void)
{
    stc_gpio_init_t stcGpioCfg;
    stc_spi_init_t stcSpiInit;

    /* Configure peripheral clock */
    PWC_Fcg1PeriphClockCmd(AD7606_UNIT_CLOCK, Enable);

    /* Port configurate, High driving capacity for output pin. */
    
    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(AD7606_SCK_PORT,  AD7606_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(AD7606_MOSI_PORT, AD7606_MOSI_PIN, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(AD7606_MISO_PORT, AD7606_MISO_PIN, &stcGpioCfg);

    /* Configure SPI Port function for master */
    GPIO_SetFunc(AD7606_SCK_PORT,  AD7606_SCK_PIN,  AD7606_SCK_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(AD7606_MISO_PORT, AD7606_MISO_PIN, AD7606_MISO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(AD7606_MOSI_PORT, AD7606_MOSI_PIN, AD7606_MOSI_FUNC, PIN_SUBFUNC_DISABLE);

    /* SPI De-initialize */
    SPI_DeInit(AD7606_SPI_UNIT);
    /* Clear initialize structure */
    (void)SPI_StructInit(&stcSpiInit);

    stcSpiInit.u32WireMode          = SPI_WIRE_3;              //3线SPI
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;         //全双工
    stcSpiInit.u32MasterSlave       = SPI_MASTER;              //SPI主机
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;   //SPI通信挂起关闭
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;       //禁用出错探测
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;      //奇偶校验禁用
    stcSpiInit.u32SpiMode           = SPI_MODE_3;              //使用模式3时序
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_PCLK1_DIV8;      //波特率=PCLK1/8
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_16BIT;     //16位数据传输模式
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;           //MSB优先传输
    (void)SPI_Init(AD7606_SPI_UNIT, &stcSpiInit);

    /* Command SPI function */
    SPI_FunctionCmd(AD7606_SPI_UNIT, Enable);
}

/**********************************************************************************************************
**  函 数 名 : AD7606Reset()
**  功能说明 : AD7606硬件复位
**  形    参 : OB: 操作对象结构体
**  返 回 值 : 无
**********************************************************************************************************/
void AD7606Reset(AD7606_Object * OB)
{
    if(OB->Type==AD7606_Vsense)
    {
        /*复位A0*/
        A0_REST_0();
        bsp_DelayUS(1);
        A0_REST_1();
        bsp_DelayUS(1);
        A0_REST_0();
        bsp_DelayUS(1);
    } 
		else if(OB->Type==AD7606_Iout)
    {
        /*复位A1*/
        A1_REST_0();
        bsp_DelayUS(1);
        A1_REST_1();
        bsp_DelayUS(1);
        A1_REST_0();
        bsp_DelayUS(1);
    }
}

/**********************************************************************************************************
**  函 数 名 : bsp_AD7606_init()
**  功能说明 : 2块 AD7606 spi 初始化
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void bsp_AD7606_init(void)
{
    stc_gpio_init_t stcGpioInit;
    stc_exint_init_t stcExintInit;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* 设置输出信号，包括CS，REST，CONV */
    (void)GPIO_StructInit(&stcGpioInit);       //恢复default值
    stcGpioInit.u16PinState = PIN_STATE_SET;   //CS CONV设置初始电平=1
    stcGpioInit.u16PullUp = PIN_PU_ON;         //上拉打开
    stcGpioInit.u16PinDir = PIN_DIR_OUT;       //输出
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;      //高驱动能力
    stcGpioInit.u16PinOType = PIN_OTYPE_CMOS;  //开漏关闭

    (void)GPIO_Init(A0_Cs_Port,  A0_Cs_Pin,  &stcGpioInit); //初始化7606（电池sense电压采集）CS信号
    (void)GPIO_Init(A1_Cs_Port,  A1_Cs_Pin,  &stcGpioInit); //初始化7606（模块电流采集）CS信号

    GPIO_SetPins(A0_Cs_Port,A0_Cs_Pin);         //片选输出高
    GPIO_SetPins(A1_Cs_Port,A1_Cs_Pin);         //片选输出高

    (void)GPIO_Init(A0_Cov_Port,  A0_Cov_Pin,  &stcGpioInit); //初始化7606（电池sense电压采集）转换信号
    (void)GPIO_Init(A1_Cov_Port,  A1_Cov_Pin,  &stcGpioInit); //初始化7606（模块电流采集）转换信号

    GPIO_SetPins(A0_Cov_Port,A0_Cov_Pin);       //CONV输出高
    GPIO_SetPins(A1_Cov_Port,A1_Cov_Pin);       //CONV输出高

    stcGpioInit.u16PinState = PIN_STATE_RESET;   //Reset 设置初始电平=0
    (void)GPIO_Init(A0_Reset_Port,  A0_Reset_Pin,  &stcGpioInit); //初始化7606（电池sense电压采集）复位信号
    (void)GPIO_Init(A1_Reset_Port,  A1_Reset_Pin,  &stcGpioInit); //初始化7606（模块电流采集）复位信号

    GPIO_ResetPins(A0_Reset_Port,A0_Reset_Pin);   //Rest输出低，上升沿有效
    GPIO_ResetPins(A1_Reset_Port,A0_Reset_Pin);   //Rest输出低，上升沿有效

    /* 设置输入信号，包括BUSY */
    (void)GPIO_StructInit(&stcGpioInit);       //恢复default值
    stcGpioInit.u16ExInt = PIN_EXINT_ON;       //使能IO外部中断功能
    stcGpioInit.u16PullUp = PIN_PU_OFF;        //默认无上拉
    (void)GPIO_Init(AD7606_Busy_Port,  AD7606_Busy_Pin,  &stcGpioInit); //初始化7606（公用）Busy信号

    /* 外部中断配置 */
    (void)EXINT_StructInit(&stcExintInit);
    stcExintInit.u32ExIntCh = AD7606_EXINT_CH;         //选择触发通道
    stcExintInit.u32ExIntLvl = EXINT_TRIGGER_FALLING;  //下降沿触发
    (void)EXINT_Init(&stcExintInit);

    /* IRQ注册 */
    stcIrqSignConfig.enIntSrc = AD7606_INT_SRC;        //选择触发源
    stcIrqSignConfig.enIRQn   = AD7606_IRQn;           //选择中断号
    stcIrqSignConfig.pfnCallback = &AD7606_Busy1_EXINT_IrqCallback; //注册中断回调函数
    (void)INTC_IrqSignIn(&stcIrqSignConfig);

    /* 中断向量表配置 */
    NVIC_ClearPendingIRQ(AD7606_IRQn);                  //清中断标志
    NVIC_SetPriority(AD7606_IRQn, DDL_IRQ_PRIORITY_14); //设置优先级
    NVIC_DisableIRQ(AD7606_IRQn);                       //中断默认关闭

/***************************************************************************************/
    /* 设置输入信号，包括BUSY0 */
    (void)GPIO_StructInit(&stcGpioInit);       //恢复default值
    stcGpioInit.u16ExInt = PIN_EXINT_ON;       //使能IO外部中断功能
    stcGpioInit.u16PullUp = PIN_PU_OFF;        //默认无上拉
    (void)GPIO_Init(AD7606_Busy0_Port,  AD7606_Busy0_Pin,  &stcGpioInit); //初始化7606（公用）Busy信号

    /* 外部中断配置 */
    (void)EXINT_StructInit(&stcExintInit);
    stcExintInit.u32ExIntCh = AD7606_Busy0_EXINT_CH;   //选择触发通道
    stcExintInit.u32ExIntLvl = EXINT_TRIGGER_FALLING;  //下降沿触发
    (void)EXINT_Init(&stcExintInit);

    /* IRQ注册 */
    stcIrqSignConfig.enIntSrc = AD7606_Busy0_INT_SRC;        //选择触发源
    stcIrqSignConfig.enIRQn   = AD7606_Busy0_IRQn;           //选择中断号
    stcIrqSignConfig.pfnCallback = &AD7606_Busy0_EXINT_IrqCallback; //注册中断回调函数
    (void)INTC_IrqSignIn(&stcIrqSignConfig);

    /* 中断向量表配置 */
    NVIC_ClearPendingIRQ(AD7606_Busy0_IRQn);                  //清中断标志
    NVIC_SetPriority(AD7606_Busy0_IRQn, DDL_IRQ_PRIORITY_14); //设置优先级
    NVIC_DisableIRQ(AD7606_Busy0_IRQn);                       //中断默认关闭
/***************************************************************************************/

    /*初始化SPI接口*/
    AD7606_SpiInit();
    
    NVIC_EnableIRQ(AD7606_IRQn);                        //开启中断
		NVIC_EnableIRQ(AD7606_Busy0_IRQn);                        //开启中断
}

/**********************************************************************************************************
**	函 数 名 : AD7606_ReadData()
**	功能说明 : AD7606 读通道单次采样数据到对象结构体
**	形    参 : OB:操作对象
**	返 回 值 :
**********************************************************************************************************/
uint32_t AD7606_ReadData(AD7606_Object * OB)
{
    uint8_t i = 0;

    uint32_t ret = 0;

    if(OB->Type==AD7606_Vsense)
    {
        /* 读电池sense电压采集ADC值 */
        A0_CS_0();
        for(i=0; i<AD7606_CH; i++)
        {
            ret = SPI_Receive(AD7606_SPI_UNIT,  &OB->ADC[i], 1);   //读SPI数据
        }
        A0_CS_1();
    }
		else if(OB->Type==AD7606_Iout)
    {
        /* 读模块电流采集ADC值 */
        A1_CS_0();
        for(i=0; i<AD7606_CH; i++)
        {
            ret = SPI_Receive(AD7606_SPI_UNIT,  &OB->ADC[i], 1);   //读SPI数据
        }
        A1_CS_1();
    }
    return ret;
}

/**********************************************************************************************************
**	函 数 名 : AD7606_StartConV()
**	功能说明 : AD7606 启动AD转换
**	形    参 : OB:操作对象
**	返 回 值 :
**********************************************************************************************************/
void AD7606_StartConV(AD7606_Object * OB)
{
    whoIsConV = OB->Type;  //更新转换全局标志
    
    if(OB->Type==AD7606_Vsense)
    {
        /* 给sense电压采集ADC发送转换信号 */        
        A0_CNV_0();
        bsp_DelayUS(1);
        A0_CNV_1();
        bsp_DelayUS(1);
    } 
		else if(OB->Type==AD7606_Iout)
    {
        /* 给模块电流采集ADC发送转换信号 */
        A1_CNV_0();
        bsp_DelayUS(1);
        A1_CNV_1();
        bsp_DelayUS(1);
    }
}

/**********************************************************************************************************
**  函 数 名 : AD7606_Process()
**  功能说明 : AD7606数据处理函数
**  形    参 : OB:对象   AVG_num：采样平均次数 
**  返 回 值 : 无
**********************************************************************************************************/
void AD7606_Process(AD7606_Object * OB , uint16_t AVG_num)
{
    static uint16_t cnt = 0;

		if(cnt< AVG_num-1)   //未执行到约定采样次数
    {
        /*逐个通道采样历史累加*/
        for(uint16_t i=0; i<AD7606_CH ; i++)
        {
            OB->SUM_ADC[i]+=OB->ADC[i];
        }
        cnt++;
    } 
		else	//执行到约定采样次数
    {
        /*逐个通道采样历史累加*/
        for(uint16_t i=0; i<AD7606_CH ; i++)
        {
            OB->SUM_ADC[i]+=OB->ADC[i];
        }
        /*逐个通道求采样平均*/
        for(uint16_t i=0; i<AD7606_CH ; i++)
        {
            OB->AVG_ADC[i]= OB->SUM_ADC[i]/AVG_num;
        }
        /*逐个通道清空历史累加*/
        for(uint16_t i=0; i<AD7606_CH ; i++)
        {
            OB->SUM_ADC[i]=0;
        }        
        OB->Sample_NUM = AVG_num;
        cnt=0;  //清空计数
    }
}

/**********************************************************************************************************
**  函 数 名 : AD7606_Busy0_EXINT_IrqCallback()
**  功能说明 : AD7606 Busy0 中断回调函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void AD7606_Busy0_EXINT_IrqCallback(void)
{
	if(Set == EXINT_GetExIntSrc(AD7606_Busy0_EXINT_CH))	//PA15
	{
		EXINT_ClrExIntSrc(AD7606_Busy0_EXINT_CH);                  //清中断标志

		/* Disable interrupt */
//		NVIC_DisableIRQ(AD7606_Busy0_IRQn);                        //关闭中断，防止嵌套

		AD7606_ReadData(&AD7606_A0); //AD7606_A0	电压采样
		AD7606_Process(&AD7606_A0, SAMP_CNT);                        //处理上次转换完成的数据

		whoIsConV=NoBody;
		/* Enable interrupt */
//		NVIC_EnableIRQ(AD7606_Busy0_IRQn);
	}
	else
	{
			//do nothing;
	}
}

/**********************************************************************************************************
**  函 数 名 : AD7606_EXINT_IrqCallback()
**  功能说明 : AD7606 中断回调函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void AD7606_Busy1_EXINT_IrqCallback(void)
{
	if(Set == EXINT_GetExIntSrc(AD7606_EXINT_CH))
	{
		EXINT_ClrExIntSrc(AD7606_EXINT_CH);                  //清中断标志

		/* Disable interrupt */
//		NVIC_DisableIRQ(AD7606_IRQn);                        //关闭中断，防止嵌套

		AD7606_ReadData(&AD7606_A1);		//电流采样
		AD7606_Process(&AD7606_A1, SAMP_CNT);                        //处理上次转换完成的数据       

		whoIsConV=NoBody;
		/* Enable interrupt */
//		NVIC_EnableIRQ(AD7606_IRQn);
	}
	else
	{
			//do nothing;
	}
}

