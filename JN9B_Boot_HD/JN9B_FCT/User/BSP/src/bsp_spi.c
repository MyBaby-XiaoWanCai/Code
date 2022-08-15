/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_spi.c
** Author      : Huang Cheng
** Date        : 2021-11-25
** Description : bsp_spi source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_spi.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : W5500_SpiInit()
**	功能说明 : W5500 SPI配置初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void W5500_SpiInit(void)
{
	stc_gpio_init_t stcGpioCfg;
	stc_spi_init_t stcSpiInit;
	
	/* Set the JTAG RST and JTDO to Normal GPIO */
	GPIO_SetDebugPort(GPIO_PIN_TDO | GPIO_PIN_TRST, Disable);
	
	/* Configure peripheral clock */
    PWC_Fcg1PeriphClockCmd(W5500_UNIT_CLOCK, Enable);
	
	/* Port configurate, High driving capacity for output pin. */
    (void)GPIO_StructInit(&stcGpioCfg);
	stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    stcGpioCfg.u16PinState = PIN_STATE_SET;
    (void)GPIO_Init(W5500_NSS_PORT,  W5500_NSS_PIN, &stcGpioCfg);

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(W5500_SCK_PORT,  W5500_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(W5500_MOSI_PORT, W5500_MOSI_PIN, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(W5500_MISO_PORT, W5500_MISO_PIN, &stcGpioCfg);

    /* Configure SPI Port function for master */
    GPIO_SetFunc(W5500_SCK_PORT,  W5500_SCK_PIN,  W5500_SCK_FUNC, PIN_SUBFUNC_DISABLE);
	GPIO_SetFunc(W5500_MISO_PORT, W5500_MISO_PIN, W5500_MISO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(W5500_MOSI_PORT, W5500_MOSI_PIN, W5500_MOSI_FUNC, PIN_SUBFUNC_DISABLE);
	
	/* SPI De-initialize */
    SPI_DeInit(W5500_SPI_UNIT);
	/* Clear initialize structure */
    (void)SPI_StructInit(&stcSpiInit);   

    stcSpiInit.u32WireMode          = W5500_WIRE_MODE;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = SPI_MASTER;
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;
    stcSpiInit.u32SpiMode           = W5500_SPI_MODE;
    stcSpiInit.u32BaudRatePrescaler = W5500_BR_DIV;
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;
    (void)SPI_Init(W5500_SPI_UNIT, &stcSpiInit);
	
	/* Command SPI function */
    SPI_FunctionCmd(W5500_SPI_UNIT, Enable);
}

/**********************************************************************************************************
**	函 数 名 : AD7124_SpiInit()
**	功能说明 : AD7124 SPI配置初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AD7124_SpiInit(void)
{
	stc_gpio_init_t stcGpioCfg;
	stc_spi_init_t stcSpiInit;
	
	/* Configure peripheral clock */
    PWC_Fcg1PeriphClockCmd(AD7124_UNIT_CLOCK, Enable);
	
	/* Port configurate, High driving capacity for output pin. */
    (void)GPIO_StructInit(&stcGpioCfg);
	stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    stcGpioCfg.u16PinState = PIN_STATE_SET;
    (void)GPIO_Init(AD7124_NSS_PORT,  AD7124_NSS_PIN, &stcGpioCfg);

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(AD7124_SCK_PORT,  AD7124_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(AD7124_MOSI_PORT, AD7124_MOSI_PIN, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(AD7124_MISO_PORT, AD7124_MISO_PIN, &stcGpioCfg);

    /* Configure SPI Port function for master */
    GPIO_SetFunc(AD7124_SCK_PORT,  AD7124_SCK_PIN,  AD7124_SCK_FUNC, PIN_SUBFUNC_DISABLE);
	GPIO_SetFunc(AD7124_MISO_PORT, AD7124_MISO_PIN, AD7124_MISO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(AD7124_MOSI_PORT, AD7124_MOSI_PIN, AD7124_MOSI_FUNC, PIN_SUBFUNC_DISABLE);
	
	/* SPI De-initialize */
    SPI_DeInit(AD7124_SPI_UNIT);
	/* Clear initialize structure */
    (void)SPI_StructInit(&stcSpiInit);   

    stcSpiInit.u32WireMode          = AD7124_WIRE_MODE;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = SPI_MASTER;
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;
    stcSpiInit.u32SpiMode           = AD7124_SPI_MODE;
    stcSpiInit.u32BaudRatePrescaler = AD7124_BR_DIV;
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;
    (void)SPI_Init(AD7124_SPI_UNIT, &stcSpiInit);
	
	/* Command SPI function */
    SPI_FunctionCmd(AD7124_SPI_UNIT, Enable);
}

/**********************************************************************************************************
**	函 数 名 : AD7175_SpiInit()
**	功能说明 : AD7175 SPI配置初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void AD7175_SpiInit(void)
{
	stc_gpio_init_t stcGpioCfg;
	stc_spi_init_t stcSpiInit;
	
	/* Configure peripheral clock */
    PWC_Fcg1PeriphClockCmd(AD7175_UNIT_CLOCK, Enable);
	
	/* Port configurate, High driving capacity for output pin. */
    (void)GPIO_StructInit(&stcGpioCfg);
	stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    stcGpioCfg.u16PinState = PIN_STATE_SET;
    (void)GPIO_Init(AD7175_NSS_PORT,  AD7175_NSS_PIN, &stcGpioCfg);

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(AD7175_SCK_PORT,  AD7175_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(AD7175_MOSI_PORT, AD7175_MOSI_PIN, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(AD7175_MISO_PORT, AD7175_MISO_PIN, &stcGpioCfg);

    /* Configure SPI Port function for master */
    GPIO_SetFunc(AD7175_SCK_PORT,  AD7175_SCK_PIN,  AD7175_SCK_FUNC, PIN_SUBFUNC_DISABLE);
	GPIO_SetFunc(AD7175_MISO_PORT, AD7175_MISO_PIN, AD7175_MISO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(AD7175_MOSI_PORT, AD7175_MOSI_PIN, AD7175_MOSI_FUNC, PIN_SUBFUNC_DISABLE);
	
	/* SPI De-initialize */
    SPI_DeInit(AD7175_SPI_UNIT);
	/* Clear initialize structure */
    (void)SPI_StructInit(&stcSpiInit);   

    stcSpiInit.u32WireMode          = AD7175_WIRE_MODE;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = SPI_MASTER;
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;
    stcSpiInit.u32SpiMode           = AD7175_SPI_MODE;
    stcSpiInit.u32BaudRatePrescaler = AD7175_BR_DIV;
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;
    (void)SPI_Init(AD7175_SPI_UNIT, &stcSpiInit);
	
	/* Command SPI function */
    SPI_FunctionCmd(AD7175_SPI_UNIT, Enable);
}

/**********************************************************************************************************
**	函 数 名 : DAC1_SpiInit()
**	功能说明 : DAC1 SPI配置初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void DAC1_SpiInit(void)
{
	stc_gpio_init_t stcGpioCfg;
	stc_spi_init_t stcSpiInit;
	
	/* Configure peripheral clock */
    PWC_Fcg1PeriphClockCmd(DAC1_UNIT_CLOCK, Enable);
	
	/* Port configurate, High driving capacity for output pin. */
    (void)GPIO_StructInit(&stcGpioCfg);
	stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    stcGpioCfg.u16PinState = PIN_STATE_SET;
    (void)GPIO_Init(DAC1_NSS_PORT,  DAC1_NSS_PIN, &stcGpioCfg);

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(DAC1_SCK_PORT,  DAC1_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(DAC1_MOSI_PORT, DAC1_MOSI_PIN, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(DAC1_MISO_PORT, DAC1_MISO_PIN, &stcGpioCfg);

    /* Configure SPI Port function for master */
    GPIO_SetFunc(DAC1_SCK_PORT,  DAC1_SCK_PIN,  DAC1_SCK_FUNC, PIN_SUBFUNC_DISABLE);
	GPIO_SetFunc(DAC1_MISO_PORT, DAC1_MISO_PIN, DAC1_MISO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(DAC1_MOSI_PORT, DAC1_MOSI_PIN, DAC1_MOSI_FUNC, PIN_SUBFUNC_DISABLE);
	
	/* SPI De-initialize */
    SPI_DeInit(DAC1_SPI_UNIT);
	/* Clear initialize structure */
    (void)SPI_StructInit(&stcSpiInit);   

    stcSpiInit.u32WireMode          = DAC1_WIRE_MODE;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = SPI_MASTER;
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;
    stcSpiInit.u32SpiMode           = DAC1_SPI_MODE;
    stcSpiInit.u32BaudRatePrescaler = DAC1_BR_DIV;
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;
    (void)SPI_Init(DAC1_SPI_UNIT, &stcSpiInit);
	
	/* Command SPI function */
    SPI_FunctionCmd(DAC1_SPI_UNIT, Enable);
}

/**********************************************************************************************************
**	函 数 名 : DAC2_SpiInit()
**	功能说明 : DAC2 SPI配置初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void DAC2_SpiInit(void)
{
	stc_gpio_init_t stcGpioCfg;
	stc_spi_init_t stcSpiInit;
	
	/* Configure peripheral clock */
    PWC_Fcg1PeriphClockCmd(DAC2_UNIT_CLOCK, Enable);
	
	/* Port configurate, High driving capacity for output pin. */
    (void)GPIO_StructInit(&stcGpioCfg);
	stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    stcGpioCfg.u16PinState = PIN_STATE_SET;
    (void)GPIO_Init(DAC2_NSS_PORT,  DAC2_NSS_PIN, &stcGpioCfg);

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(DAC2_SCK_PORT,  DAC2_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(DAC2_MOSI_PORT, DAC2_MOSI_PIN, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(DAC2_MISO_PORT, DAC2_MISO_PIN, &stcGpioCfg);

    /* Configure SPI Port function for master */
    GPIO_SetFunc(DAC2_SCK_PORT,  DAC2_SCK_PIN,  DAC2_SCK_FUNC, PIN_SUBFUNC_DISABLE);
	GPIO_SetFunc(DAC2_MISO_PORT, DAC2_MISO_PIN, DAC2_MISO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(DAC2_MOSI_PORT, DAC2_MOSI_PIN, DAC2_MOSI_FUNC, PIN_SUBFUNC_DISABLE);
	
	/* SPI De-initialize */
    SPI_DeInit(DAC2_SPI_UNIT);
	/* Clear initialize structure */
    (void)SPI_StructInit(&stcSpiInit);   

    stcSpiInit.u32WireMode          = DAC2_WIRE_MODE;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = SPI_MASTER;
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;
    stcSpiInit.u32SpiMode           = DAC2_SPI_MODE;
    stcSpiInit.u32BaudRatePrescaler = DAC2_BR_DIV;
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;
    (void)SPI_Init(DAC2_SPI_UNIT, &stcSpiInit);
	
	/* Command SPI function */
    SPI_FunctionCmd(DAC2_SPI_UNIT, Enable);
}

/**********************************************************************************************************
**	函 数 名 : W25Q_MB85R_SpiInit()
**	功能说明 : W25Q和MB85RS SPI配置初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void W25Q_MB85R_SpiInit(void)
{
	stc_gpio_init_t stcGpioCfg;
	stc_spi_init_t stcSpiInit;
	
	/* Configure peripheral clock */
    PWC_Fcg1PeriphClockCmd(W25Q_UNIT_CLOCK, Enable);
	
	/* Port configurate, High driving capacity for output pin. */
    (void)GPIO_StructInit(&stcGpioCfg);
	stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    stcGpioCfg.u16PinState = PIN_STATE_SET;
	(void)GPIO_Init(MB85R_NSS_PORT,  MB85R_NSS_PIN, &stcGpioCfg);

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(W25Q_SCK_PORT,  W25Q_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(W25Q_MOSI_PORT, W25Q_MOSI_PIN, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(W25Q_MISO_PORT, W25Q_MISO_PIN, &stcGpioCfg);

    /* Configure SPI Port function for master */
    GPIO_SetFunc(W25Q_SCK_PORT,  W25Q_SCK_PIN,  W25Q_SCK_FUNC, PIN_SUBFUNC_DISABLE);
	GPIO_SetFunc(W25Q_MISO_PORT, W25Q_MISO_PIN, W25Q_MISO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(W25Q_MOSI_PORT, W25Q_MOSI_PIN, W25Q_MOSI_FUNC, PIN_SUBFUNC_DISABLE);
	
	/* SPI De-initialize */
    SPI_DeInit(W25Q_SPI_UNIT);
	/* Clear initialize structure */
    (void)SPI_StructInit(&stcSpiInit);   

    stcSpiInit.u32WireMode          = W25Q_WIRE_MODE;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = SPI_MASTER;
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;
    stcSpiInit.u32SpiMode           = W25Q_SPI_MODE;
    stcSpiInit.u32BaudRatePrescaler = W25Q_BR_DIV;
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;
    (void)SPI_Init(W25Q_SPI_UNIT, &stcSpiInit);
    
    MB85R_NSS_1();                      //FRAM CS Disable
	/* Command SPI function */
    SPI_FunctionCmd(W25Q_SPI_UNIT, Enable);
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/

