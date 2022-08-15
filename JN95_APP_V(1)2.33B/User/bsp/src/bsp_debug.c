/**********************************************************************************************************
*
*	模块名称 : debug 处理函数
*	文件名称 : bsp_debug.c
*	版    本 : V1.0
*	说    明 : debug 处理函数
*	作	 者：Robert.Li
*	日	 期:  2021.7.5

**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
UART_HandleTypeDef UartHandle;
DBGPort_TypeDef				s_dbgPort;
extern	osSemaphoreId_t dbg_Semaphore;

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/*
*********************************************************************************************************
*	函 数 名: void bsp_debug_usart_config(void)
*	功能说明: 初始化HW Debug UART3  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/

void bsp_debug_usart_config(void)
{

    UartHandle.Instance          = DEBUG_USART;
    UartHandle.Init.BaudRate     = DEBUG_USART_BAUDRATE;
    UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits     = UART_STOPBITS_1;
    UartHandle.Init.Parity       = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode         = UART_MODE_TX_RX;


    if (HAL_UART_Init(&UartHandle) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    __HAL_UART_ENABLE_IT(&UartHandle,UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&UartHandle,UART_IT_IDLE);
}

/*
*********************************************************************************************************
*	函 数 名: HAL_UART_MspInit
*	功能说明: UART MSP 初始化
*	形    参:  huart: UART handle
*	返 回 值: 无
*********************************************************************************************************
*/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    DEBUG_USART_CLK_ENABLE();

    DEBUG_USART_RX_GPIO_CLK_ENABLE();
    DEBUG_USART_TX_GPIO_CLK_ENABLE();

//USART3 GPIO Configuration
    // PD8     ------> USART3_TX
    //PD9     ------> USART3_RX

    // 配置Tx引脚为复用功能
    GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = DEBUG_USART_TX_AF;
    HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);

    // 配置Rx引脚为复用功能
    GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
    GPIO_InitStruct.Alternate = DEBUG_USART_RX_AF;
    HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);

    //抢占优先级0，子优先级1
    HAL_NVIC_SetPriority(DEBUG_USART_IRQ ,0,1);
    //使能USART3中断通道
    HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ );

}

/*
*********************************************************************************************************
*	函 数 名: HAL_UART_MspDeInit
*	功能说明: 配置 Debug UART3   该函数被 HAL_UART_Init() 调用。
*	形    参: huart: UART handle
*	返 回 值: 无
*********************************************************************************************************
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

    if(uartHandle->Instance==USART3)
    {
        /* USER CODE BEGIN USART3_MspDeInit 0 */

        /* USER CODE END USART3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART3_CLK_DISABLE();

        /**USART3 GPIO Configuration
        PD8     ------> USART3_TX
        PD9     ------> USART3_RX
        */
        HAL_GPIO_DeInit(GPIOD, DEBUG_USART_TX_PIN|DEBUG_USART_RX_PIN);

        /* USART3 interrupt Deinit */
        HAL_NVIC_DisableIRQ(DEBUG_USART_IRQ);
        /* USER CODE BEGIN USART3_MspDeInit 1 */

        /* USER CODE END USART3_MspDeInit 1 */
    }
}


/*
*********************************************************************************************************
*	函 数 名: void Usart_SendString(uint8_t *str)
*	功能说明:   发送字符串
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/

void Usart_SendString(uint8_t *str)
{
    unsigned int k=0;
    do
    {
        HAL_UART_Transmit(&UartHandle,(uint8_t *)(str + k) ,1,1000);
        k++;
    } while(*(str + k)!='\0');

}


/*
*********************************************************************************************************
*	函 数 名: int fputc(int ch, FILE *f)
*	功能说明: 重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到串口DEBUG_USART */
    HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);


    return (ch);
}

/*
*********************************************************************************************************
*	函 数 名: int fgetc(FILE *f)
*	功能说明: 重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
int fgetc(FILE *f)
{

    int ch;
    HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);
    return (ch);
}



static	void	DBGRS232_TxHandler (void)
{


}
/*
******************************************************************************
* Function Name  : DBGRS232_RxHandler
* Description    :
*
* Input          : None
* Output         : None
* Return         :
******************************************************************************
*/
static	void	DBGRS232_RxHandler (uint8_t	rxData)
{
    if(s_dbgPort.rxFlag == dbstart)
    {
        if(rxData == DBG_CMD_HEAD_START)
        {
            s_dbgPort.rxFlag	=valid;
            s_dbgPort.rxlen		=0;
        }
    }
    if(s_dbgPort.rxFlag == valid)
    {
        if(s_dbgPort.rxlen < DEF_DBG_BUFSIZE)
        {
            s_dbgPort.rxdBuf[s_dbgPort.rxlen++]	=	rxData;

            if((rxData == '\r') || (rxData == '\n'))
            {
                // 正确接收一个命令包
                __HAL_UART_DISABLE_IT(&UartHandle,UART_IT_RXNE);
                s_dbgPort.rxFlag	=end;
                // 处理命令包
                osSemaphoreRelease(dbg_Semaphore);
            }
        }
        else
        {
            s_dbgPort.rxFlag	=dbstart;
            s_dbgPort.rxlen		=0;
            memset(s_dbgPort.rxdBuf,	'\0',	DEF_DBG_BUFSIZE);
        }
    }
}



/*
******************************************************************************
* Function Name  : HAL_DBGUART_RxTxISRHandler
* Description 		:
*
* Input          : None
* Output         : None
* Return         :
******************************************************************************
*/
void	HAL_DBGUART_RxTxISRHandler (void)
{
    uint8_t     rx_data;

    if(__HAL_UART_GET_FLAG( &UartHandle, UART_FLAG_RXNE ) != RESET)
    {
        rx_data = (uint16_t) READ_REG(UartHandle.Instance->DR);
        DBGRS232_RxHandler(rx_data);
        __HAL_UART_CLEAR_FLAG(&UartHandle,UART_FLAG_RXNE);
    }

    else	if (__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_IDLE) != RESET) {
        DBGRS232_TxHandler();
        __HAL_UART_CLEAR_IDLEFLAG(&UartHandle);
    }
    else {
        __HAL_UART_GET_FLAG( &UartHandle, UART_FLAG_RXNE );
        DBGRS232_RxHandler(rx_data);
        __HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_IDLE);
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

void	HAL_DBGCmdProcess (void)
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
            printf("\r\n[AD_Amb]=%f",	AD_Amb);
        }
        else if(strcmp((char *)DBG_CMD_CODE_QUEADC_AUX, (char *)tmpCode) == 0)
        {
            printf("\r\n[AD_Aux]=%f",AD_Aux);
        }
        else if(strcmp((char *)DBG_CMD_CODE_QUEADC_BUS1, (char *)tmpCode) == 0)
        {
            printf("\r\n[AD_Bus1]=%f",AD_Bus1);
        }
        else if(strcmp((char *)DBG_CMD_CODE_QUEADC_BUS2, (char *)tmpCode) == 0)
        {
            printf("\r\n[AD_Bus2]=%f",AD_Bus2);
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

    // 重新使能接收
    s_dbgPort.rxFlag =0;
    __HAL_UART_ENABLE_IT(&UartHandle,UART_IT_RXNE);
}















/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
