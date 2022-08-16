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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
extern uint32_t OFC_DATA;
extern uint32_t FSC_DATA;
uint32_t SGM58601_Data[8] = {0x00};
uint8_t DRDY_NUM = 0;

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
static void SGM58601_EXINT_IrqCallback(void);

/**********************************************************************************************************
**	�� �� �� : SGM58601_SpiInit()
**	����˵�� : SGM58601 SPI���ó�ʼ��
**	��    �� : ��
**	�� �� ֵ : ��
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
    stcGpioCfg.u16PullUp = PIN_PU_ON;               //����
    (void)GPIO_Init(SGM58601_CS_PORT,  SGM58601_CS_PIN, &stcGpioCfg);
    GPIO_SetPins(SGM58601_CS_PORT,SGM58601_CS_PIN);         //Ƭѡ�����

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

    stcSpiInit.u32WireMode          = SPI_WIRE_3;              //3��SPI
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;         //ȫ˫��
    stcSpiInit.u32MasterSlave       = SPI_MASTER;              //SPI����
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;   //SPIͨ�Ź���ر�
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;       //���ó���̽��
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;      //��żУ�����
    stcSpiInit.u32SpiMode           = SPI_MODE_1;              //ʹ��ģʽ1ʱ��
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_PCLK1_DIV16;      //������=PCLK1/16 = 120M/64 = 1.875M  SPI_BR_PCLK1_DIV64
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;      //8λ���ݴ���ģʽ
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;           //MSB���ȴ���
    (void)SPI_Init(SGM58601_SPI_UNIT, &stcSpiInit);

    /* Command SPI function */
    SPI_FunctionCmd(SGM58601_SPI_UNIT, Enable);
}

/**********************************************************************************************************
**	�� �� �� : ADC_WHITE_TO_ONE_REG()
**	����˵�� : SGM58601 SPIдһ���Ĵ���
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : ADC_READ_FROM_ONE_REG()
**	����˵�� : SGM58601 SPI��һ���Ĵ���
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : ADC_RDATA_COMMAND()
**	����˵�� : SGM58601 SPI���ζ�����ָ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_RDATA_COMMAND(void)  //DRDYΪ��ʱִ�У���ȡADC
{
    uint8_t txDate = RDATA_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	�� �� �� : ADC_RDATAC_COMMAND()
**	����˵�� : SGM58601 SPI����������ָ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_RDATAC_COMMAND(void) //DRDYΪ��ʱִ�У�������ȡADC
{
    uint8_t txDate = RDATAC_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	�� �� �� : ADC_RDATAC_COMMAND()
**	����˵�� : SGM58601 SPI����ת��ָֹͣ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SDATAC_COMMAND(void) //DRDYΪ��ʱִ�У�ֹͣ������ȡADC
{
    uint8_t txDate = SDATAC_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	�� �� �� : ADC_GET_CONDATA()
**	����˵�� : ��ȡADCת��ֵ����DRDY=0ʱִ��
**	��    �� : ��
**	�� �� ֵ : ��
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
    txDate=0x10|OFC0_REG_ADDR; //��ʼ��ַ������Ч
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=0x02;  //��3���Ĵ���,�Ĵ�������=2+1.
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
    txDate=0x10|FSC0_REG_ADDR; //��ʼ��ַ������Ч
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=0x02;  //��3���Ĵ���,�Ĵ�������=2+1.
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=0x00;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&LSB,1);
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&MID,1);
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&MSB,1);
    FSC_DATA=(MSB<<16)+(MID<<8)+LSB;
    return FSC_DATA;
}

/**********************************************************************************************************
**	�� �� �� : ADC_SYNC_COMMAND()
**	����˵�� : ������ʹ ADC ֹͣ��ǰת������λ�����˲�����
               �������ź�ͻ���ʱ����Ҫ���� SYNC_COMMAND ���� +WAKEUP_COMMAND �������ͨ��
               SYNC ��������λ ADC �����˲�����
               ��ͨ���л���ʱ��ADC ���Զ�ִ�� SYNC �Լ� WAKEUP ��������ٷ������������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SYNC_COMMAND(void)
{
    uint8_t txDate = SYNC_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	�� �� �� : ADC_WAKEUP_COMMAND()
**	����˵�� : ���� WAKEUP ������� SYNC �� Standby ����֮��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_WAKEUP_COMMAND(void)
{
    uint8_t txDate = WAKEUP_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	�� �� �� : ADC_STANDBY_COMMAND()
**	����˵�� :������ʹ ADC ����͹��� Standby mode��ȷ�� CS Ϊ��ʱ��SCLK �޷�ת�����򽫻���
              Standby mode������ WAKEUP �������ʹ ADC ���� Standby mode��STANDBY �� WAKEUP
              ��Ͽ���ʵ�� one-shot conversion��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_STANDBY_COMMAND(void)
{
    uint8_t txDate = STANDBY_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	�� �� �� : ADC_RESET_COMMAND()
**	����˵�� :ADC ��λ���ִ�д�����󣬳��� ADCON �Ĵ����п��� CLKOUT ����� CLK0��CLK1 bit
              �⣬���мĴ�����λ��Ĭ��ֵ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_RESET_COMMAND(void)
{
    uint8_t txDate = RESET_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	�� �� �� : ADC_SELF_CALIBRATION()
**	����˵�� : ADC ��У׼������ OFFSET ERROR �� GAIN ERROR��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SELF_CALIBRATION(void)
{
    uint8_t txDate = SELFCAL_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    bsp_DelayMS(100);//��У׼����Ҫdelayһ��ʱ�䣬�ȴ�ADC���У׼���˶�ʱ���ڣ���ADC������������Ч����ʱ�����DRATE��ͬ����ͬ��
}

/**********************************************************************************************************
**	�� �� �� : ADC_SELF_OFFSET_CALIBRATION()
**	����˵�� : ADC OFFSET ERROR ��У׼��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SELF_OFFSET_CALIBRATION(void)
{
    uint8_t txDate = SELFOCAL_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	�� �� �� : ADC_SELF_OFFSET_CALIBRATION()
**	����˵�� : ADC GAIN ERROR ��У׼��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SELF_GAIN_CALIBRATION(void)
{
    uint8_t txDate = SELFGCAL_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	�� �� �� : ADC_SYS_OFFSET_CALIBRATION()
**	����˵�� : ADC ϵͳOFFSET ERRORУ׼������ADC�ڲ����ⲿ��·OFFSET ERROR��ϵͳOFFSET ERROR
               У׼������ϵͳ������ṩ 0V ��������źš�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SYS_OFFSET_CALIBRATION(void)
{
    uint8_t txDate = SYSOCAL_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

/**********************************************************************************************************
**	�� �� �� : ADC_SYS_GAIN_CALIBRATION()
**	����˵�� : ADC ϵͳ GAIN ERROR У׼������ ADC �ڲ����ⲿ��· GAIN ERROR��ϵͳ GAIN ERROR У
               ׼������ϵͳ������ṩ�����̲�������źš�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SYS_GAIN_CALIBRATION(void)
{
    uint8_t txDate = SYSGCAL_COMMAND;
    uint8_t rxDate = 0;
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
}

//void ADC_ONE_SHOT_MODE(void)  //���β���ģʽ
//{
//	SPI1_ReadWriteByte(STANDBY_COMMAND);
//	SPI1_ReadWriteByte(WAKEUP_COMMAND);
//}
/**********************************************************************************************************
**	�� �� �� : ADC_SET_CHANNEL()
**	����˵�� : SGM58601 ѡ������ͨ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SET_CHANNEL(uint8_t AIN_P,uint8_t AIN_N)
{
    uint8_t channel=0;
    channel=(AIN_P<<4)+AIN_N;
    ADC_WHITE_TO_ONE_REG(MUX_REG_ADDR,channel);
}

/**********************************************************************************************************
**	�� �� �� : ADC_SET_PGA()
**	����˵�� : SGM58601 ����PGA
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SET_PGA(uint8_t PGA)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(ADCON_REG_ADDR);
    temp=temp&0xf8;
    temp=temp|PGA;  //�Ĵ�������λ���䣬PGA����λ��0
    ADC_WHITE_TO_ONE_REG(ADCON_REG_ADDR,temp);
}

/**********************************************************************************************************
**	�� �� �� : ADC_SET_DATARATE()
**	����˵�� : SGM58601 ���������������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_SET_DATARATE(uint8_t DATARATE)
{
    ADC_WHITE_TO_ONE_REG(DRATE_REG_ADDR,DATARATE);
}

/**********************************************************************************************************
**	�� �� �� : ADC_SET_SDCS()
**	����˵�� : SGM58601 ���ô���������·���
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : ADC_SET_CLKOUT()
**	����˵�� : SGM58601 ����CLKƵ��
**	��    �� : ��
**	�� �� ֵ : ��
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
**	�� �� �� : ADC_EN_INPUTBUFFER()
**	����˵�� : SGM58601 ʹ�����뻺��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_EN_INPUTBUFFER(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp|0x02;//�Ĵ�������λ���䣬ENΪ1
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**	�� �� �� : ADC_DISABLE_INPUTBUFFER()
**	����˵�� : SGM58601 ʧ�����뻺��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_DISABLE_INPUTBUFFER(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp&0xfd;//�Ĵ�������λ���䣬EN����λ��0
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**	�� �� �� : ADC_EN_AUTOCALIBRATION()
**	����˵�� : SGM58601 ʹ���Զ�У׼
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_EN_AUTOCALIBRATION(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp|0x04;//�Ĵ�������λ���䣬ENΪ1
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**	�� �� �� : ADC_DISABLE_AUTOCALIBRATION()
**	����˵�� : SGM58601 ʧ���Զ�У׼
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_DISABLE_AUTOCALIBRATION(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp&0xfb;//�Ĵ�������λ���䣬ENΪ0
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**	�� �� �� : ADC_MSB_FIRST()
**	����˵�� : SGM58601  �����������˳��MSB_FIRST
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_MSB_FIRST(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp&0xf7;//�Ĵ�������λ����oorder=0
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**	�� �� �� : ADC_LSB_FIRST()
**	����˵�� : SGM58601  �����������˳��LSB_FIRST
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void ADC_LSB_FIRST(void)
{
    uint8_t temp=0;
    temp=ADC_READ_FROM_ONE_REG(STATUS_REG_ADDR);
    temp=temp|0x08;//�Ĵ�������λ���� order=1
    ADC_WHITE_TO_ONE_REG(STATUS_REG_ADDR,temp);
}

/**********************************************************************************************************
**  �� �� �� : ADC_SGM58601_Init()
**  ����˵�� : ADC_SGM58601 �Ĵ�������
**  ��    �� : ��
**  �� �� ֵ : ��
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

    STATUS_REG_Struct.ORDER = MSB_FIRST;               //�������MSB�ȳ�
    STATUS_REG_Struct.AUTO_CALI = AUTO_CALI_DISABLE;   //�Զ�У׼�ر�
    STATUS_REG_Struct.BUFFER = BUFFER_DISABLE;         //ģ������buffer�ر�

    MUX_REG_Struct.AIN_P = AIN0;       //��������ͨ��ѡ��
    MUX_REG_Struct.AIN_N = AINCOM;     //���븺��ͨ��ѡ��

    ADCON_REG_Struct.CLK_OUT = CLKOUT_OFF; //ʱ������ر�
    ADCON_REG_Struct.SENSOR_DETECT = SDCS_OFF ;//����Դ̽��ر�
    ADCON_REG_Struct.PGA = PGA_1; //�ɱ������Ŵ���ѡ��ģʽ1

    DRATE_REG_Struct.DRATE = DR_7500SPS; //�����������60KSPS

    //GPIO3-0 direction setting
    GPIO_REG_Struct.DIR3=Direction_Input;   //GPIO����
    GPIO_REG_Struct.DIR2=Direction_Input;   //GPIO����
    GPIO_REG_Struct.DIR1=Direction_Input;   //GPIO����
    GPIO_REG_Struct.DIR0=Direction_Input;   //GPIO����

    //GPIO3-0 data setting��When the digital I/O pin is configured as an input ,DIO bit will have no effect
    GPIO_REG_Struct.DIO3=0;                 //�����ʱ��IO��ƽ
    GPIO_REG_Struct.DIO2=1;                 //�����ʱ��IO��ƽ
    GPIO_REG_Struct.DIO1=0;                 //�����ʱ��IO��ƽ
    GPIO_REG_Struct.DIO0=1;                 //�����ʱ��IO��ƽ

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

    /*�Ĵ���д������*/
    STATUS_REG_DATA=STATUS_REG_Struct.ORDER|STATUS_REG_Struct.AUTO_CALI|STATUS_REG_Struct.BUFFER;
    MUX_REG_DATA=(MUX_REG_Struct.AIN_P<<4)|MUX_REG_Struct.AIN_N;
    ADCON_REG_DATA=ADCON_REG_Struct.CLK_OUT|ADCON_REG_Struct.SENSOR_DETECT|ADCON_REG_Struct.PGA;
    DRATE_REG_DATA=DRATE_REG_Struct.DRATE;
    GPIO_REG_DATA=(GPIO_REG_Struct.DIR3<<7)|(GPIO_REG_Struct.DIR2<<6)|(GPIO_REG_Struct.DIR1<<5)|(GPIO_REG_Struct.DIR0<<4)|
                  (GPIO_REG_Struct.DIO3<<3)|(GPIO_REG_Struct.DIO2<<2)|(GPIO_REG_Struct.DIO1<<1)|(GPIO_REG_Struct.DIO0<<0);

    txDate=0x50|STATUS_REG_ADDR; //��ʼ��ַ��0x50д��Ч
    SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,1);
    txDate=0x04;   //д5���Ĵ���,�Ĵ�������=4+1.
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
		
//    ADC_SELF_CALIBRATION();	//��У׼
//    for(Feed_WDG_Cnt = 0; Feed_WDG_Cnt<25; Feed_WDG_Cnt++)
//    {
//        bsp_DelayUS(200);
//        Feed_WDG();
//    }

//	//��ȡУ׼ֵ
//	SPI1_ReadWriteByte(0x10|OFC0_REG_ADDR);//��ʼ��ַ��0x10����Ч
//	SPI1_ReadWriteByte(0x05);//��6���Ĵ���,�Ĵ�������=5+1.
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

//	ADC_WHITE_TO_ONE_REG(STATUS_REG,0x00); //XXXX000X MSB first��Auto cali disable��buffer disable
//  ADC_SET_CHANNEL(AIN0,AINCOM); //����ͨ��AIN0��P,AINCOM��N
//	ADC_WHITE_TO_ONE_REG(ADCON_REG,0x00);//0 00 00 000��clock out off;sensor detect off;pga=1;
//	ADC_SET_DATARATE(DR_60000SPS);//���ò�����
//	ADC_SET_CLKOUT(CLKOUT_FCLK_Prescaler_1);
//	ADC_SELF_CALIBRATION();
}

/**********************************************************************************************************
**  �� �� �� : SGM58601_EXTIX_Init()
**  ����˵�� : SGM58601 BUSY���ж�����
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void SGM58601_EXTIX_Init(void)
{
    stc_gpio_init_t stcGpioInit;
    stc_exint_init_t stcExintInit;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* ���������źţ�����BUSY */
    (void)GPIO_StructInit(&stcGpioInit);       //�ָ�defaultֵ
    stcGpioInit.u16ExInt = PIN_EXINT_ON;       //ʹ��IO�ⲿ�жϹ���
    stcGpioInit.u16PullUp = PIN_PU_ON;         //Ĭ������
    (void)GPIO_Init(SGM58601_EXTI_PORT,  SGM58601_EXTI_PIN,  &stcGpioInit); //��ʼ��58601�����ã�Busy�ź�

    /* �ⲿ�ж����� */
    (void)EXINT_StructInit(&stcExintInit);
    stcExintInit.u32ExIntCh = SGM58601_EXINT_CH;         //ѡ�񴥷�ͨ��
    stcExintInit.u32ExIntLvl = EXINT_TRIGGER_FALLING;     //�½��ش���
    (void)EXINT_Init(&stcExintInit);

    /* IRQע�� */
    stcIrqSignConfig.enIntSrc = SGM58601_INT_SRC;        //ѡ�񴥷�Դ
    stcIrqSignConfig.enIRQn   = SGM58601_IRQn;           //ѡ���жϺ�
    stcIrqSignConfig.pfnCallback = &SGM58601_EXINT_IrqCallback; //ע���жϻص�����
    (void)INTC_IrqSignIn(&stcIrqSignConfig);

    /* �ж����������� */
    NVIC_ClearPendingIRQ(SGM58601_IRQn);                  //���жϱ�־
    NVIC_SetPriority(SGM58601_IRQn, DDL_IRQ_PRIORITY_02); //�������ȼ�
    NVIC_DisableIRQ(SGM58601_IRQn);                       //�ж�Ĭ�Ϲر�
}

/**********************************************************************************************************
**  �� �� �� : bsp_SGM58601Init()
**  ����˵�� : bsp_SGM58601 ��ʼ����Ĭ�ϳ�ʼ����ͨ��0
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void bsp_SGM58601Init(void)
{
    static uint8_t SGM58601_Start_Read=0;

    SGM58601_SpiInit();
    SGM58601_EXTIX_Init();

    if(SGM58601_Start_Read == 0)
    {
        SGM58601_Start_Read = 1;
        SGM58601_CS_0();                //CS���ͣ�ѡ��
        ADC_SGM58601_Init();
        SGM58601_CS_1();
        bsp_DelayMS(10);
//        SGM58601_Write_Reg(MUX_REG_ADDR,0x01);
//        test=SGM58601_Read_Reg(MUX_REG_ADDR);
        /* Enable interrupt */
        NVIC_EnableIRQ(SGM58601_IRQn);  //��RDY�ж�
    }
}

/**********************************************************************************************************
**  �� �� �� : SGM58601_Switch_Ch()
**  ����˵�� : bsp_SGM58601 �л�����ͨ��
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void SGM58601_Switch_Ch(uint8_t CH)
{
    SGM58601_CS_0();                //CS���ͣ�ѡ��
    ADC_SET_CHANNEL(CH,AINCOM);    //���˲��ù�����
    SGM58601_CS_1();                //CS���ͣ�ѡ��
}

/**********************************************************************************************************
**  �� �� �� : SGM58601_Read_Data()
**  ����˵�� : bsp_SGM58601 ֻ����RDY=0ʱʹ��
**  ��    �� : ��
**  �� �� ֵ : 24bitת������ ��24λ��Ч
**********************************************************************************************************/
uint32_t SGM58601_Read_Data(void)
{
		en_result_t ReadRet = 0;
		static uint8_t Err_Cnt = 0;
	
    uint32_t ret=0;
    uint8_t txDate[3]= {0};
    uint8_t rxDate[3]= {0};
    SGM58601_CS_0();                //CS���ͣ�ѡ��
    ADC_RDATA_COMMAND();            //����RDATAָ��
    // bsp_DelayUS(7);                //�ȴ�10US ��50��ʱ������Լ����6.7US
    ReadRet = SPI_TransmitReceive(SGM58601_SPI_UNIT,&txDate,&rxDate,3);
    SGM58601_CS_1();                //CS����
    ret=(rxDate[0]<<16)+(rxDate[1]<<8)+rxDate[0];
		
		if(ReadRet != 0)
		{
			Err_Cnt++;
			
			if(Err_Cnt >= 2)
			{
				for(uint8_t j = 0;j < 8;j++)
				{
					IP_Alarm_Flag[j] |= IntelProtect_ADE;  //ADC����
				}
				Err_Cnt = 0;
			}
		}
		
    return ret;
}

/**********************************************************************************************************
**  �� �� �� : SGM58601_Read_Reg()
**  ����˵�� : bsp_SGM58601 ���Ĵ���
**  ��    �� : ��
**  �� �� ֵ : 24bitת������ ��24λ��Ч
**********************************************************************************************************/
uint8_t SGM58601_Read_Reg(uint8_t reg_add)
{
    uint8_t ret=0;
    SGM58601_CS_0();                //CS���ͣ�ѡ��
    ret=ADC_READ_FROM_ONE_REG(reg_add);
    SGM58601_CS_1();                //CS����
    return ret;
}

/**********************************************************************************************************
**  �� �� �� : SGM58601_Write_Reg()
**  ����˵�� : bsp_SGM58601 ���Ĵ���
**  ��    �� : ��
**  �� �� ֵ : 24bitת������ ��24λ��Ч
**********************************************************************************************************/
void SGM58601_Write_Reg(uint8_t reg_add ,uint8_t Date)
{
    SGM58601_CS_0();                //CS���ͣ�ѡ��
    ADC_WHITE_TO_ONE_REG(reg_add, Date);
    SGM58601_CS_1();                //CS����
}

/**********************************************************************************************************
**  �� �� �� : SGM58601_EXINT_IrqCallback()
**  ����˵�� : SGM58601 �жϻص�����
**  ��    �� : ��
**  �� �� ֵ : ��
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
//        EXINT_ClrExIntSrc(SGM58601_EXINT_CH);	//���жϱ�־

        cnt++;

        NVIC_DisableIRQ(SGM58601_IRQn);

        if(cnt == 2)
        {
            SGM58601_Data[DRDY_NUM] = SGM58601_Read_Data();  //������

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
            SGM58601_Switch_Ch(DRDY_NUM);    //�л�ͨ��
        }
        else if(cnt == 5)
        {
            cnt = 0;
        }

        NVIC_EnableIRQ(SGM58601_IRQn);
				
				EXINT_ClrExIntSrc(SGM58601_EXINT_CH);	//���жϱ�־
    }
    else
    {

    }
}



