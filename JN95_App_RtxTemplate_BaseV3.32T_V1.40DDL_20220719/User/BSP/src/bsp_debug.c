/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_debug.c
** Author      : Huang Cheng
** Date        : 2021-11-15
** Description : bsp_debug source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_debug.h"
#include "string.h"
#include "bsp_ad7124.h"
#include "bsp_gpio.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
stc_usart_uart_init_t UartHandle;
DBGPort_TypeDef				s_dbgPort;
//extern	osSemaphoreId_t dbg_Semaphore;
extern uint32_t ad7124_value[16];
extern uint8_t W5500_IP_ADDRESS[4];
extern float AD_Amb;
extern float AD_Aux;
extern float AD_Bus1;
extern float AD_Bus2;

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : bsp_InitUSART()
**	功能说明 : 初始化USART配置
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void bsp_InitUSART(void)
{
	const stc_usart_uart_init_t stcUartInit = 
	{
        .u32Baudrate = 115200UL,
        .u32BitDirection = USART_LSB,
        .u32StopBit = USART_STOPBIT_1BIT,
        .u32Parity = USART_PARITY_NONE,
        .u32DataWidth = USART_DATA_LENGTH_8BIT,
        .u32ClkMode = USART_INTERNCLK_NONE_OUTPUT,
        .u32PclkDiv = USART_PCLK_DIV64,
        .u32OversamplingBits = USART_OVERSAMPLING_8BIT,
        .u32NoiseFilterState = USART_NOISE_FILTER_DISABLE,
        .u32SbDetectPolarity = USART_SB_DETECT_FALLING,
    };
	
	/* Configure USART RX/TX pin. */
    GPIO_SetFunc(USART_RX_PORT, USART_RX_PIN, USART_RX_GPIO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(USART_TX_PORT, USART_TX_PIN, USART_TX_GPIO_FUNC, PIN_SUBFUNC_DISABLE);
	
	/* Enable peripheral clock */
    PWC_Fcg3PeriphClockCmd(USART_FUNCTION_CLK_GATE, Enable);

    USART_DeInit(USART_UNIT);
    /* Initialize UART function. */
    if (Ok != USART_UartInit(USART_UNIT, &stcUartInit))
    {
		//测试
        for (;;)
        {
					printf("USART3 is ERROR !!!\r\n");
        }
    }
	else
	{
			printf("USART3 is OK!!!!\r\n");
	}
		
	stc_irq_signin_config_t stcIrqRegiCfg;
	/*	设置串口接收中断	*/
	stcIrqRegiCfg.enIRQn = Int004_IRQn;//设置中断优先级
	stcIrqRegiCfg.enIntSrc = INT_USART3_RI;//设置中断名称
	stcIrqRegiCfg.pfnCallback = &HC_USART_RxIrq_Callback;//设置中断回调函数
	(void)INTC_IrqSignIn(&stcIrqRegiCfg);
	NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_00);//DDL_IRQ_PRIORITY_DEFAULT DDL_IRQ_PRIORITY_00
	NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
		
	/* 设置串口错误接收中断 */
	stcIrqRegiCfg.enIRQn = Int003_IRQn;
	stcIrqRegiCfg.enIntSrc = INT_USART3_EI;
	stcIrqRegiCfg.pfnCallback = &HC_USART3_RxErr_IrqCallback;
	(void)INTC_IrqSignIn(&stcIrqRegiCfg);
	NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_01);
	NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
	
	/* Enable RX/TX function */
	USART_FuncCmd(USART_UNIT, (USART_RX | USART_INT_RX | USART_TX), Enable);
}

/*
*********************************************************************************************************
*	函 数 名: int fputc(int ch, FILE *f)
*	功能说明: 重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int32_t fputc(int32_t ch, FILE *f)
{      
	USART_SendData(USART_UNIT, (char)ch);
	while (Reset == USART_GetStatus(USART_UNIT, USART_FLAG_TXE)) /* Wait Tx data register empty */
	{
		;
	}
	
	return ch;
}



/**********************************************************************************************************
**	函 数 名 : HC_USART_RxIrq_Callback()
**	功能说明 : 串口接收中断处理回调函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
uint16_t Send_Cnt = 0;
uint16_t Rec_Data = 0;
void HC_USART_RxIrq_Callback()
{	
/*
	  USART_SendData(USART_UNIT,1234);
	
		while(Reset == USART_GetStatus(USART_UNIT,USART_SR_TXE))
		{
				Send_Cnt++;
				printf("New Send_Cnt = %d\r\n",Send_Cnt);
		}
*/
	DBGRS232_RxHandler(USART_RecData(USART_UNIT));
	HAL_DBGCmdProcess();
}
/**********************************************************************************************************
**	函 数 名 : HC_USART3_RxErr_IrqCallback()
**	功能说明 : 串口错误中断处理回调函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void HC_USART3_RxErr_IrqCallback()
{

}

/**********************************************************************************************************
**	函 数 名 : DBGRS232_RxHandler()
**	功能说明 : 串口接收数据处理函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void DBGRS232_RxHandler (uint8_t	rxData)
{
    if(s_dbgPort.rxFlag == dbstart)
    {
        if(rxData == DBG_CMD_HEAD_START)
        {
            s_dbgPort.rxFlag = valid;
            s_dbgPort.rxlen	 = 0;
					
			printf("s_dbgPort.rxFlag = %d,s_dbgPort.rxlen = %d",s_dbgPort.rxFlag,s_dbgPort.rxFlag);
        }
    }
    if(s_dbgPort.rxFlag == valid)
    {
        if(s_dbgPort.rxlen < DEF_DBG_BUFSIZE)
        {
            s_dbgPort.rxdBuf[s_dbgPort.rxlen++]	=	rxData;
						printf("s_dbgPort.rxdBuf[s_dbgPort.rxlen] = %d\r\n",s_dbgPort.rxdBuf[s_dbgPort.rxlen - 1]);

            if((rxData == '\r') || (rxData == '\n'))
            {
                //正确接收一个命令包
				USART_FuncCmd(USART_UNIT,USART_RX | USART_INT_RX,Disable);
                s_dbgPort.rxFlag = end;
                //处理命令包
                //osSemaphoreRelease(dbg_Semaphore);
            }
        }
        else
        {
            s_dbgPort.rxFlag = dbstart;
            s_dbgPort.rxlen	= 0;
            memset(s_dbgPort.rxdBuf,	'\0',	DEF_DBG_BUFSIZE);
        }
    }
}


/*
*********************************************************************************************************
*	函 数 名: HAL_DBGCmdProcess
*	功能说明: 调试信息测试信息输出。
*	形    参:  无
*	返 回 值: 无
*	说	明：//命令格式：“$Q_VERB\r\n"		查询版本号
*********************************************************************************************************
*/
void HAL_DBGCmdProcess (void)
{
    uint16_t	codeLen=0,paraLen=0;
    uint8_t	tmpCode[DBG_CMD_CODE_SIZE];
    uint8_t	tmpPara[DBG_CMD_PARA_SIZE];

    memset(tmpCode,	'\0',	sizeof(tmpCode));
    memset(tmpPara,	'\0',	sizeof(tmpPara));

    if(s_dbgPort.rxdBuf[0] == DBG_CMD_HEAD_START)
    {
        // 复制命令头
        for(codeLen=0;	((s_dbgPort.rxdBuf[codeLen+1] != ':') && (s_dbgPort.rxdBuf[codeLen+1] != '\r') && (s_dbgPort.rxdBuf[codeLen+1] != '\n') && (codeLen < DBG_CMD_CODE_SIZE) && ((codeLen+1) < s_dbgPort.rxlen));	codeLen++)
        {
            tmpCode[codeLen] = s_dbgPort.rxdBuf[codeLen+1];
        }
        tmpCode[codeLen] = '\0';

        // 复制参数区
        for(paraLen=0;	((s_dbgPort.rxdBuf[paraLen+codeLen+2] != '\r') && (s_dbgPort.rxdBuf[paraLen+codeLen+2] != '\n') && (paraLen < DBG_CMD_PARA_SIZE) && ((paraLen+codeLen+2) < s_dbgPort.rxlen));	paraLen++)
        {
            tmpPara[paraLen] = s_dbgPort.rxdBuf[paraLen+codeLen+2];
        }
        tmpPara[paraLen] = '\0';
        // 查询 IP
        if(strcmp((char *)DBG_CMD_CODE_QUEIP,	(char *)tmpCode) == 0)
        {
            if ((W5500_IP_ADDRESS[0] == 192)&(W5500_IP_ADDRESS[1]==168))
            {
                printf("\r\nQ_IP=%d.%d.%d.%d\r\n",	W5500_IP_ADDRESS[0],W5500_IP_ADDRESS[1],W5500_IP_ADDRESS[2],W5500_IP_ADDRESS[3]);
            }
            else
            {
                printf("\r\nQ_IP=[Error-Read!]\r\n");
            }
        }
        // 查询 程序版本号
        else if(strcmp((char *)DBG_CMD_CODE_QVERB, (char *)tmpCode) == 0)
        {
            printf("\r\nQ_VERB=%s\r\n",SOFTWARE_VERSION);
        }
        else if(strcmp((char *)DBG_CMD_CODE_QUEADC_AMB, (char *)tmpCode) == 0)
        {
            printf("\r\n[AD_Amb]=%f\r\n",	AD_Amb);
        }
        else if(strcmp((char *)DBG_CMD_CODE_QUEADC_AUX, (char *)tmpCode) == 0)
        {
            printf("\r\n[AD_Aux]=%f\r\n",AD_Aux);
        }
        else if(strcmp((char *)DBG_CMD_CODE_QUEADC_BUS1, (char *)tmpCode) == 0)
        {
            printf("\r\n[AD_Bus1]=%f\r\n",AD_Bus1);
        }
        else if(strcmp((char *)DBG_CMD_CODE_QUEADC_BUS2, (char *)tmpCode) == 0)
        {
            printf("\r\n[AD_Bus2]=%f\r\n",AD_Bus2);
        }
        else if(strcmp((char *)DBG_CMD_CODE_QUEEEPROM, (char *)tmpCode) == 0)
        {

        }
        else if(strcmp((char *)DBG_CMD_CODE_QUEFLASH, (char *)tmpCode) == 0)
        {

        }
        else if(strcmp((char *)DBG_CMD_CODE_QUEAD7124, (char *)tmpCode) == 0)
        {
            printf("\r\n[AD7124_DAT0]=0x%08x",ad7124_value[0]);
            printf("\r\n[AD7124_DAT1]=0x%08x",ad7124_value[1]);
            printf("\r\n[AD7124_DAT2]=0x%08x",ad7124_value[2]);
            printf("\r\n[AD7124_DAT3]=0x%08x",ad7124_value[3]);
            printf("\r\n[AD7124_DAT4]=0x%08x",ad7124_value[4]);
            printf("\r\n[AD7124_DAT5]=0x%08x",ad7124_value[5]);
            printf("\r\n[AD7124_DAT6]=0x%08x",ad7124_value[6]);
            printf("\r\n[AD7124_DAT7]=0x%08x",ad7124_value[7]);
            printf("\r\n[AD7124_DAT8]=0x%08x",ad7124_value[8]);
            printf("\r\n[AD7124_DAT9]=0x%08x",ad7124_value[9]);
            printf("\r\n[AD7124_DAT10]=0x%08x",ad7124_value[10]);
            printf("\r\n[AD7124_DAT11]=0x%08x",ad7124_value[11]);
            printf("\r\n[AD7124_DAT12]=0x%08x",ad7124_value[12]);
            printf("\r\n[AD7124_DAT13]=0x%08x",ad7124_value[13]);
            printf("\r\n[AD7124_DAT14]=0x%08x",ad7124_value[14]);
            printf("\r\n[AD7124_DAT15]=0x%08x\r\n",ad7124_value[15]);
        }
        else if(strcmp((char *)DBG_CMD_CODE_QUEAD7175, (char *)tmpCode) == 0)
        {
            printf("\r\n[AD7175_ID_0]=0x%02x",AD7175_ID[0]);
            printf("\r\n[AD7175_ID_1]=0x%02x",AD7175_ID[1]);
            printf("\r\n[AD7175_ID_2]=0x%02x",AD7175_ID[2]);
            printf("\r\n[AD7175_DAT0]=0x%08x",ad7175_value[0]);
            printf("\r\n[AD7175_DAT1]=0x%08x",ad7175_value[1]);
            printf("\r\n[AD7175_DAT2]=0x%08x",ad7175_value[2]);
            printf("\r\n[AD7175_DAT3]=0x%08x",ad7175_value[3]);
            printf("\r\n[AD7175_DAT4]=0x%08x",ad7175_value[4]);
            printf("\r\n[AD7175_DAT5]=0x%08x",ad7175_value[5]);
            printf("\r\n[AD7175_DAT6]=0x%08x",ad7175_value[6]);
            printf("\r\n[AD7175_DAT7]=0x%08x",ad7175_value[7]);
            printf("\r\n[AD7175_DAT8]=0x%08x",ad7175_value[8]);
            printf("\r\n[AD7175_DAT9]=0x%08x",ad7175_value[9]);
            printf("\r\n[AD7175_DAT10]=0x%08x",ad7175_value[10]);
            printf("\r\n[AD7175_DAT11]=0x%08x",ad7175_value[11]);
            printf("\r\n[AD7175_DAT12]=0x%08x",ad7175_value[12]);
            printf("\r\n[AD7175_DAT13]=0x%08x",ad7175_value[13]);
            printf("\r\n[AD7175_DAT14]=0x%08x",ad7175_value[14]);
            printf("\r\n[AD7175_DAT15]=0x%08x\r\n",ad7175_value[15]);
        }
    }

    //重新使能接收
    s_dbgPort.rxFlag = 0;
    USART_FuncCmd(USART_UNIT,USART_RX | USART_INT_RX,Enable);//使能串口接收、接收中断
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
