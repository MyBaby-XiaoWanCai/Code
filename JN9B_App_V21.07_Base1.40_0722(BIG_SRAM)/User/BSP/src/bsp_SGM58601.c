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
extern uint32_t OFC_DATA;
extern uint32_t FSC_DATA;
uint32_t SGM58601_Data[8] = {0x00};
uint8_t DRDY_NUM = 0;

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void SGM58601_EXINT_IrqCallback(void);

/**********************************************************************************************************
**	函 数 名 : SGM58601_SpiInit()
**	功能说明 : SGM58601 SPI配置初始化
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void SGM58601_SpiInit(void)
{
    stc_gpio_init_t stcGpioCfg;
    stc_spi_init_t stcSpiInit;

    /* Configure peripheral clock */
    PWC_Fcg1PeriphClockCmd(SGM58601_UNIT_CLOCK, Enable);

    /* Port configurate, High driving capacity for output pin. */

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    stcGpioCfg.u16PinState = PIN_STATE_SET;
    stcGpioCfg.u16PullUp = PIN_PU_ON;               //上拉
    (void)GPIO_Init(SGM58601_CS_PORT,  SGM58601_CS_PIN, &stcGpioCfg);
    GPIO_SetPins(SGM58601_CS_PORT,SGM58601_CS_PIN);         //片选输出高

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(SGM58601_SCK_PORT,  SGM58601_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(SGM58601_MOSI_PORT, SGM58601_MOSI_PIN, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(SGM58601_MISO_PORT, SGM58601_MISO_PIN, &stcGpioCfg);

    /* Configure SPI Port function for master */
    GPIO_SetFunc(SGM58601_SCK_PORT,  SGM58601_SCK_PIN,  SGM58601_SCK_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(SGM58601_MISO_PORT, SGM58601_MISO_PIN, SGM58601_MISO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(SGM58601_MOSI_PORT, SGM58601_MOSI_PIN, SGM58601_MOSI_FUNC, PIN_SUBFUNC_DISABLE);

    /* SPI De-initialize */
    SPI_DeInit(SGM58601_SPI_UNIT);
    /* Clear initialize structure */
    (void)SPI_StructInit(&stcSpiInit);

    stcSpiInit.u32WireMode          = SPI_WIRE_3;              //3线SPI
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;         //全双工
    stcSpiInit.u32MasterSlave       = SPI_MASTER;              //SPI主机
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;   //SPI通信挂起关闭
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;       //禁用出错探测
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;      //奇偶校验禁用
    stcSpiInit.u32SpiMode           = SPI_MODE_1;              //使用模式1时序
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_PCLK1_DIV16;      //波特率=PCLK1/16 = 120M/64 = 1.875M  SPI_BR_PCLK1_DIV64
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;      //8位数据传输模式
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;           //MSB优先传输
    (void)SPI_Init(SGM58601_SPI_UNIT, &stcSpiInit);

    /* Command SPI function */
    SPI_FunctionCmd(SGM58601_SPI_UNIT, Enable);
}

/**********************************************************************************************************
**	函 数 名 : ADC_WHITE_TO_ONE_REG()
**	功能说明 : SGM58601 SPI写一个寄存器
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_WHITE_TO_ONE_REG(uint8_t reg_addr,uint8_t reg_data)
{
    uint8_t txDate[3]= {0};
    uint8_t rxDate[3]= {0};
    uint8_t wreg=0x50;
    wreg=wreg|reg_addr;
    txDate[0]=wreg;
    txDate[2]=reg_data;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,txDate,rxDate,sizeof(txDate));
}

/**********************************************************************************************************
**	函 数 名 : ADC_READ_FROM_ONE_REG()
**	功能说明 : SGM58601 SPI读一个寄存器
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
uint8_t ADC_READ_FROM_ONE_REG(uint8_t reg_addr)
{
    uint8_t txDate[3]= {0};
    uint8_t rxDate = 0;
    uint8_t rreg=0x10;
    rreg=rreg|reg_addr;
    txDate[0]=rreg;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate[0],&rxDate,1);
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate[1],&rxDate,1);
    bsp_DelayUS(10); //t6>>50*tclkin=0.128us*50=6.4us

    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate[2],&rxDate,1);
    return rxDate;
}

/**********************************************************************************************************
**	函 数 名 : ADC_RDATA_COMMAND()
**	功能说明 : SGM58601 SPI单次读数据指令
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_RDATA_COMMAND(void)  //DRDY为低时执行，读取ADC
{
    uint8_t txDate = RDATA_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	函 数 名 : ADC_RDATAC_COMMAND()
**	功能说明 : SGM58601 SPI连续读数据指令
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_RDATAC_COMMAND(void) //DRDY为低时执行，连续读取ADC
{
    uint8_t txDate = RDATAC_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	函 数 名 : ADC_RDATAC_COMMAND()
**	功能说明 : SGM58601 SPI连续转换停止指令
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SDATAC_COMMAND(void) //DRDY为低时执行，停止连续读取ADC
{
    uint8_t txDate = SDATAC_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	函 数 名 : ADC_GET_CONDATA()
**	功能说明 : 读取ADC转换值，在DRDY=0时执行
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
uint32_t ADC_GET_CONDATA(void)
{
    uint32_t data_adc=0;
    uint32_t MSB=0;
    uint32_t MID=0;
    uint32_t LSB=0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&data_adc,&MSB,1);
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&data_adc,&MID,1);
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&data_adc,&LSB,1);
    data_adc=(MSB<<16)+(MID<<8)+LSB;
    return data_adc;
}

uint32_t ADC_GET_OFCDATA(void)
{
    uint32_t MSB=0;
    uint32_t MID=0;
    uint32_t LSB=0;
    uint32_t OFC_DATA=0;
    uint32_t txDate=0;
    uint32_t rxDate=0;
    txDate=0x10|OFC0_REG_ADDR; //起始地址，读有效
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=0x02;  //读3个寄存器,寄存器个数=2+1.
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=0x00;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&LSB,1);
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&MID,1);
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&MSB,1);

    OFC_DATA=(MSB<<16)+(MID<<8)+LSB;
    return OFC_DATA;
}

uint32_t ADC_GET_FSCDATA(void)
{
    uint32_t MSB=0;
    uint32_t MID=0;
    uint32_t LSB=0;
    uint32_t FSC_DATA=0;
    uint32_t txDate=0;
    uint32_t rxDate=0;
    txDate=0x10|FSC0_REG_ADDR; //起始地址，读有效
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=0x02;  //读3个寄存器,寄存器个数=2+1.
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=0x00;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&LSB,1);
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&MID,1);
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&MSB,1);
    FSC_DATA=(MSB<<16)+(MID<<8)+LSB;
    return FSC_DATA;
}

/**********************************************************************************************************
**	函 数 名 : ADC_SYNC_COMMAND()
**	功能说明 : 此命令使 ADC 停止当前转换，复位数字滤波器。
               当输入信号突变的时候，需要发送 SYNC_COMMAND 命令 +WAKEUP_COMMAND 命令或者通过
               SYNC 引脚来复位 ADC 数字滤波器。
               当通道切换的时候，ADC 会自动执行 SYNC 以及 WAKEUP 命令，无需再发送这两条命令。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SYNC_COMMAND(void)
{
    uint8_t txDate = SYNC_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	函 数 名 : ADC_WAKEUP_COMMAND()
**	功能说明 : 发送 WAKEUP 命令，用在 SYNC 和 Standby 命令之后。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_WAKEUP_COMMAND(void)
{
    uint8_t txDate = WAKEUP_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	函 数 名 : ADC_STANDBY_COMMAND()
**	功能说明 :此命令使 ADC 进入低功耗 Standby mode。确保 CS 为低时，SCLK 无翻转，否则将会打断
              Standby mode。发送 WAKEUP 命令可以使 ADC 跳出 Standby mode。STANDBY 和 WAKEUP
              配合可以实现 one-shot conversion。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_STANDBY_COMMAND(void)
{
    uint8_t txDate = STANDBY_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	函 数 名 : ADC_RESET_COMMAND()
**	功能说明 :ADC 复位命令，执行此命令后，除了 ADCON 寄存器中控制 CLKOUT 输出的 CLK0、CLK1 bit
              外，所有寄存器复位到默认值。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_RESET_COMMAND(void)
{
    uint8_t txDate = RESET_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SELF_CALIBRATION()
**	功能说明 : ADC 自校准，包括 OFFSET ERROR 和 GAIN ERROR。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SELF_CALIBRATION(void)
{
    uint8_t txDate = SELFCAL_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    bsp_DelayMS(100);//自校准后需要delay一段时间，等待ADC完成校准。此段时间内，对ADC的其他操作无效。此时间根据DRATE不同而不同。
}

/**********************************************************************************************************
**	函 数 名 : ADC_SELF_OFFSET_CALIBRATION()
**	功能说明 : ADC OFFSET ERROR 自校准。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SELF_OFFSET_CALIBRATION(void)
{
    uint8_t txDate = SELFOCAL_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SELF_OFFSET_CALIBRATION()
**	功能说明 : ADC GAIN ERROR 自校准。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SELF_GAIN_CALIBRATION(void)
{
    uint8_t txDate = SELFGCAL_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SYS_OFFSET_CALIBRATION()
**	功能说明 : ADC 系统OFFSET ERROR校准，包括ADC内部及外部电路OFFSET ERROR。系统OFFSET ERROR
               校准必须在系统输入端提供 0V 差分输入信号。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SYS_OFFSET_CALIBRATION(void)
{
    uint8_t txDate = SYSOCAL_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SYS_GAIN_CALIBRATION()
**	功能说明 : ADC 系统 GAIN ERROR 校准，包括 ADC 内部及外部电路 GAIN ERROR。系统 GAIN ERROR 校
               准必须在系统输入端提供满量程差分输入信号。
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SYS_GAIN_CALIBRATION(void)
{
    uint8_t txDate = SYSGCAL_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

//void ADC_ONE_SHOT_MODE(void)  //单次采样模式
//{
//	SPI1_ReadWriteByte(STANDBY_COMMAND);
//	SPI1_ReadWriteByte(WAKEUP_COMMAND);
//}
/**********************************************************************************************************
**	函 数 名 : ADC_SET_CHANNEL()
**	功能说明 : SGM58601 选择输入通道
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SET_CHANNEL(uint8_t AIN_P,uint8_t AIN_N)
{
    uint8_t channel=0;
    channel=(AIN_P<<4)+AIN_N;
    ADC_WHITE_TO_ONE_REG(MUX_REG_ADDR,channel);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SET_PGA()
**	功能说明 : SGM58601 设置PGA
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SET_PGA(uint8_t PGA)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(ADCON_REG_ADDR);
    temp=temp&0xf8;
    temp=temp|PGA;  //寄存器其他位不变，PGA配置位置0
    ADC_WHITE_TO_ONE_REG(ADCON_REG_ADDR,temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SET_DATARATE()
**	功能说明 : SGM58601 设置数据输出速率
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SET_DATARATE(uint8_t DATARATE)
{
    ADC_WHITE_TO_ONE_REG(DRATE_REG_ADDR,DATARATE);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SET_SDCS()
**	功能说明 : SGM58601 设置传感器开短路检测
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SET_SDCS(uint8_t SDCS_UA)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(ADCON_REG_ADDR);
    temp=temp&0xe7;
    temp=temp|SDCS_UA;
    ADC_WHITE_TO_ONE_REG(ADCON_REG_ADDR,temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_SET_CLKOUT()
**	功能说明 : SGM58601 设置CLK频率
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_SET_CLKOUT(uint8_t CLKOUT_F)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(ADCON_REG_ADDR);
    temp=temp&0x9f;
    temp=temp|CLKOUT_F;
    ADC_WHITE_TO_ONE_REG(ADCON_REG_ADDR,temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_EN_INPUTBUFFER()
**	功能说明 : SGM58601 使能输入缓存
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_EN_INPUTBUFFER(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp|0x02;//寄存器其他位不变，EN为1
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_DISABLE_INPUTBUFFER()
**	功能说明 : SGM58601 失能输入缓存
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_DISABLE_INPUTBUFFER(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp&0xfd;//寄存器其他位不变，EN配置位置0
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_EN_AUTOCALIBRATION()
**	功能说明 : SGM58601 使能自动校准
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_EN_AUTOCALIBRATION(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp|0x04;//寄存器其他位不变，EN为1
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_DISABLE_AUTOCALIBRATION()
**	功能说明 : SGM58601 失能自动校准
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_DISABLE_AUTOCALIBRATION(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp&0xfb;//寄存器其他位不变，EN为0
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_MSB_FIRST()
**	功能说明 : SGM58601  设置数据输出顺序MSB_FIRST
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_MSB_FIRST(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp&0xf7;//寄存器其他位不变order=0
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**	函 数 名 : ADC_LSB_FIRST()
**	功能说明 : SGM58601  设置数据输出顺序LSB_FIRST
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void ADC_LSB_FIRST(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp|0x08;//寄存器其他位不变 order=1
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**  函 数 名 : ADC_SGM58601_Init()
**  功能说明 : ADC_SGM58601 寄存器配置
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void ADC_SGM58601_Init(void)
{
    uint8_t STATUS_REG_DATA=0x00;
    uint8_t MUX_REG_DATA=0x00;
    uint8_t ADCON_REG_DATA=0x00;
    uint8_t DRATE_REG_DATA=0x00;
    uint8_t GPIO_REG_DATA=0x00;

    uint8_t txDate = 0;
    uint8_t rxDate = 0;

//    static uint8_t Feed_WDG_Cnt = 0;

    //ADC configure
    STATUS_REGInit_Struct   STATUS_REG_Struct;
    MUX_REGInit_Struct      MUX_REG_Struct;
    ADCON_REGInit_Struct    ADCON_REG_Struct;
    DRATE_REGInit_Struct	DRATE_REG_Struct;
    GPIO_REGInit_Struct     GPIO_REG_Struct;

    STATUS_REG_Struct.ORDER = MSB_FIRST;               //数据输出MSB先出
    STATUS_REG_Struct.AUTO_CALI = AUTO_CALI_DISABLE;   //自动校准关闭
    STATUS_REG_Struct.BUFFER = BUFFER_DISABLE;         //模拟输入buffer关闭

    MUX_REG_Struct.AIN_P = AIN0;       //输入正极通道选中
    MUX_REG_Struct.AIN_N = AINCOM;     //输入负极通道选中

    ADCON_REG_Struct.CLK_OUT = CLKOUT_OFF; //时钟输出关闭
    ADCON_REG_Struct.SENSOR_DETECT = SDCS_OFF ;//电流源探测关闭
    ADCON_REG_Struct.PGA = PGA_1; //可编程增益放大器选中模式1

    DRATE_REG_Struct.DRATE = DR_7500SPS; //数据输出速率60KSPS

    //GPIO3-0 direction setting
    GPIO_REG_Struct.DIR3=Direction_Input;   //GPIO输入
    GPIO_REG_Struct.DIR2=Direction_Input;   //GPIO输入
    GPIO_REG_Struct.DIR1=Direction_Input;   //GPIO输入
    GPIO_REG_Struct.DIR0=Direction_Input;   //GPIO输入

    //GPIO3-0 data setting，When the digital I/O pin is configured as an input ,DIO bit will have no effect
    GPIO_REG_Struct.DIO3=0;                 //做输出时，IO电平
    GPIO_REG_Struct.DIO2=1;                 //做输出时，IO电平
    GPIO_REG_Struct.DIO1=0;                 //做输出时，IO电平
    GPIO_REG_Struct.DIO0=1;                 //做输出时，IO电平

    /* Check the ADC parameters */
//    assert_param(IS_ADC_ORDER(STATUS_REG_Struct.ORDER));
//    assert_param(IS_ADC_AUTO_CALI(STATUS_REG_Struct.AUTO_CALI));
//    assert_param(IS_ADC_BUFFER(STATUS_REG_Struct.BUFFER));
//    assert_param(IS_ADC_AIN(MUX_REG_Struct.AIN_P));
//    assert_param(IS_ADC_AIN(MUX_REG_Struct.AIN_N));
//    assert_param(IS_ADC_PGA(ADCON_REG_Struct.PGA));
//    assert_param(IS_ADC_SDCS(ADCON_REG_Struct.SENSOR_DETECT));
//    assert_param(IS_ADC_CLKOUT(ADCON_REG_Struct.CLK_OUT));
//    assert_param(IS_ADC_DRATE(DRATE_REG_Struct.DRATE));

    /*寄存器写入数据*/
    STATUS_REG_DATA=STATUS_REG_Struct.ORDER|STATUS_REG_Struct.AUTO_CALI|STATUS_REG_Struct.BUFFER;
    MUX_REG_DATA=(MUX_REG_Struct.AIN_P<<4)|MUX_REG_Struct.AIN_N;
    ADCON_REG_DATA=ADCON_REG_Struct.CLK_OUT|ADCON_REG_Struct.SENSOR_DETECT|ADCON_REG_Struct.PGA;
    DRATE_REG_DATA=DRATE_REG_Struct.DRATE;
    GPIO_REG_DATA=(GPIO_REG_Struct.DIR3<<7)|(GPIO_REG_Struct.DIR2<<6)|(GPIO_REG_Struct.DIR1<<5)|(GPIO_REG_Struct.DIR0<<4)|
                  (GPIO_REG_Struct.DIO3<<3)|(GPIO_REG_Struct.DIO2<<2)|(GPIO_REG_Struct.DIO1<<1)|(GPIO_REG_Struct.DIO0<<0);

    txDate=0x50|STATUS_REG_ADDR; //起始地址，0x50写有效
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=0x04;   //写5个寄存器,寄存器个数=4+1.
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=STATUS_REG_DATA;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=MUX_REG_DATA;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=ADCON_REG_DATA;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=DRATE_REG_DATA;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=GPIO_REG_DATA;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
		
//    ADC_SELF_CALIBRATION();	//自校准
//    for(Feed_WDG_Cnt = 0; Feed_WDG_Cnt<25; Feed_WDG_Cnt++)
//    {
//        bsp_DelayUS(200);
//        Feed_WDG();
//    }

//	//读取校准值
//	SPI1_ReadWriteByte(0x10|OFC0_REG_ADDR);//起始地址，0x10读有效
//	SPI1_ReadWriteByte(0x05);//读6个寄存器,寄存器个数=5+1.
//	LSB=SPI1_ReadWriteByte(0x00);
//	MID=SPI1_ReadWriteByte(0x00);
//	MSB=SPI1_ReadWriteByte(0x00);
//	OFC_DATA=(MSB<<16)+(MID<<8)+LSB;
//	LSB=SPI1_ReadWriteByte(0x00);
//	MID=SPI1_ReadWriteByte(0x00);
//	MSB=SPI1_ReadWriteByte(0x00);
//	FSC_DATA=(MSB<<16)+(MID<<8)+LSB;
    //ADC_GET_OFCDATA(void);
    //ADC_GET_FSCDATA(void);

//	ADC_WHITE_TO_ONE_REG(STATUS_REG,0x00); //XXXX000X MSB first，Auto cali disable，buffer disable
//  ADC_SET_CHANNEL(AIN0,AINCOM); //配置通道AIN0接P,AINCOM接N
//	ADC_WHITE_TO_ONE_REG(ADCON_REG,0x00);//0 00 00 000；clock out off;sensor detect off;pga=1;
//	ADC_SET_DATARATE(DR_60000SPS);//设置采样率
//	ADC_SET_CLKOUT(CLKOUT_FCLK_Prescaler_1);
//	ADC_SELF_CALIBRATION();
}

/**********************************************************************************************************
**  函 数 名 : SGM58601_EXTIX_Init()
**  功能说明 : SGM58601 BUSY脚中断配置
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void SGM58601_EXTIX_Init(void)
{
    stc_gpio_init_t stcGpioInit;
    stc_exint_init_t stcExintInit;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* 设置输入信号，包括BUSY */
    (void)GPIO_StructInit(&stcGpioInit);       //恢复default值
    stcGpioInit.u16ExInt = PIN_EXINT_ON;       //使能IO外部中断功能
    stcGpioInit.u16PullUp = PIN_PU_ON;         //默认上拉
    (void)GPIO_Init(SGM58601_EXTI_PORT,  SGM58601_EXTI_PIN,  &stcGpioInit); //初始化58601（公用）Busy信号

    /* 外部中断配置 */
    (void)EXINT_StructInit(&stcExintInit);
    stcExintInit.u32ExIntCh = SGM58601_EXINT_CH;         //选择触发通道
    stcExintInit.u32ExIntLvl = EXINT_TRIGGER_FALLING;     //下降沿触发
    (void)EXINT_Init(&stcExintInit);

    /* IRQ注册 */
    stcIrqSignConfig.enIntSrc = SGM58601_INT_SRC;        //选择触发源
    stcIrqSignConfig.enIRQn   = SGM58601_IRQn;           //选择中断号
    stcIrqSignConfig.pfnCallback = &SGM58601_EXINT_IrqCallback; //注册中断回调函数
    (void)INTC_IrqSignIn(&stcIrqSignConfig);

    /* 中断向量表配置 */
    NVIC_ClearPendingIRQ(SGM58601_IRQn);                  //清中断标志
    NVIC_SetPriority(SGM58601_IRQn, DDL_IRQ_PRIORITY_02); //设置优先级
    NVIC_DisableIRQ(SGM58601_IRQn);                       //中断默认关闭
}

/**********************************************************************************************************
**  函 数 名 : bsp_SGM58601Init()
**  功能说明 : bsp_SGM58601 初始化，默认初始采样通道0
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void bsp_SGM58601Init(void)
{
    static uint8_t SGM58601_Start_Read=0;

    SGM58601_SpiInit();
    SGM58601_EXTIX_Init();

    if(SGM58601_Start_Read == 0)
    {
        SGM58601_Start_Read = 1;
        SGM58601_CS_0();                //CS拉低，选中
        ADC_SGM58601_Init();
        SGM58601_CS_1();
        bsp_DelayMS(10);
//        SGM58601_Write_Reg(MUX_REG_ADDR,0x01);
//        test=SGM58601_Read_Reg(MUX_REG_ADDR);
        /* Enable interrupt */
        NVIC_EnableIRQ(SGM58601_IRQn);  //开RDY中断
    }
}

/**********************************************************************************************************
**  函 数 名 : SGM58601_Switch_Ch()
**  功能说明 : bsp_SGM58601 切换采样通道
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void SGM58601_Switch_Ch(uint8_t CH)
{
    SGM58601_CS_0();                //CS拉低，选中
    ADC_SET_CHANNEL(CH,AINCOM);    //负端采用公共端
    SGM58601_CS_1();                //CS拉低，选中
}

/**********************************************************************************************************
**  函 数 名 : SGM58601_Read_Data()
**  功能说明 : bsp_SGM58601 只能在RDY=0时使用
**  形    参 : 无
**  返 回 值 : 24bit转换数据 低24位有效
**********************************************************************************************************/
uint32_t SGM58601_Read_Data(void)
{
		en_result_t ReadRet = 0;
		static uint8_t Err_Cnt = 0;
	
    uint32_t ret=0;
    uint8_t txDate[3]= {0};
    uint8_t rxDate[3]= {0};
    SGM58601_CS_0();                //CS拉低，选中
    ADC_RDATA_COMMAND();            //发送RDATA指令
    // bsp_DelayUS(7);                //等待10US ，50个时钟周期约等于6.7US
    ReadRet = SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,3);
    SGM58601_CS_1();                //CS拉高
    ret=(rxDate[0]<<16)+(rxDate[1]<<8)+rxDate[0];
		
		if(ReadRet != 0)
		{
			Err_Cnt++;
			
			if(Err_Cnt >= 2)
			{
				for(uint8_t j = 0;j < 8;j++)
				{
					IP_Alarm_Flag[j] |= IntelProtect_ADE;  //ADC故障
				}
				Err_Cnt = 0;
			}
		}
		
    return ret;
}

/**********************************************************************************************************
**  函 数 名 : SGM58601_Read_Reg()
**  功能说明 : bsp_SGM58601 读寄存器
**  形    参 : 无
**  返 回 值 : 24bit转换数据 低24位有效
**********************************************************************************************************/
uint8_t SGM58601_Read_Reg(uint8_t reg_add)
{
    uint8_t ret=0;
    SGM58601_CS_0();                //CS拉低，选中
    ret=ADC_READ_FROM_ONE_REG(reg_add);
    SGM58601_CS_1();                //CS拉高
    return ret;
}

/**********************************************************************************************************
**  函 数 名 : SGM58601_Write_Reg()
**  功能说明 : bsp_SGM58601 读寄存器
**  形    参 : 无
**  返 回 值 : 24bit转换数据 低24位有效
**********************************************************************************************************/
void SGM58601_Write_Reg(uint8_t reg_add ,uint8_t Date)
{
    SGM58601_CS_0();                //CS拉低，选中
    ADC_WHITE_TO_ONE_REG(reg_add, Date);
    SGM58601_CS_1();                //CS拉高
}

/**********************************************************************************************************
**  函 数 名 : SGM58601_EXINT_IrqCallback()
**  功能说明 : SGM58601 中断回调函数
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
uint8_t SGM58601_Debug = 0x00;
int32_t SGM58601_L = 0x0029632A;	//4V
int32_t SGM58601_H = 0x002B632A;
uint8_t ch_x_sgm = 0;

static void SGM58601_EXINT_IrqCallback(void)
{
    static uint8_t cnt=0;

    if(Set == EXINT_GetExIntSrc(SGM58601_EXINT_CH))
    {
//        EXINT_ClrExIntSrc(SGM58601_EXINT_CH);	//清中断标志

        cnt++;

        NVIC_DisableIRQ(SGM58601_IRQn);

        if(cnt == 2)
        {
            SGM58601_Data[DRDY_NUM] = SGM58601_Read_Data();  //读数据

            if(DRDY_NUM<7)
            {
                DRDY_NUM++;
            }
            else
            {
                DRDY_NUM=0;
            }
						
/*****************************************************************************************/						
						if(SGM58601_Debug == 0x01)
						{
							for(uint8_t k = 0;k<2;k++)
							{
								if(SGM58601_L > SGM58601_Data[k] || SGM58601_Data[k] > SGM58601_H)
								{
									SGM58601_Debug = 0x02;
								}
							}
						}
/*****************************************************************************************/							
        }
        else if(cnt == 3)
        {
            SGM58601_Switch_Ch(DRDY_NUM);    //切换通道
        }
        else if(cnt == 5)
        {
            cnt = 0;
        }

        NVIC_EnableIRQ(SGM58601_IRQn);
				
				EXINT_ClrExIntSrc(SGM58601_EXINT_CH);	//清中断标志
    }
    else
    {

    }
}



