/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp.c
** Author      : Huang Cheng
** Date        : 2021-11-23
** Description : This file is used to initialize and drive the hardware 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
extern PcaDataType Pca9555_Data;
uint8_t Res_9548 = 2;
uint8_t Res_9555 = 3;
uint8_t xLedWriteData[2] = {0xFF,0x00}; /* {0xFF,0x00} LED2-9 �����; {0x00,0xFF} LED2-9 ���̵� */

uint8_t xIOLEDVerConfigSet_1[2] = {0xFF,0xFF}; //�ư���汾PCA9555 IO����
uint8_t xIOLEDVerConfigSet_2[2] = {0x3F,0xFF};

//uint8_t xPowPca9555_IOConfigSet_1[2] = {0xFF,0xE0};//���ģʽ���� 1-���룬0-��� C033A��B
uint8_t xPowPca9555_IOConfigSet_1[2] = {0xFF,0xF0};//���ģʽ���� 1-���룬0-��� C033C��D
uint8_t xPowPca9555_IOConfigSet_2[2] = {0x7F,0xF0};//���ģʽ���� 1-���룬0-���
uint8_t xIOConfigWriteData[2] = {0x00,0x08};
extern Vers_Mana vers_mana;	 //���ذ弰�ư��ȡ�汾��Ϣ�洢
extern AD7606_Object AD7606_A0;
extern AD7606_Object AD7606_A1;
extern st_dac_handle_t* Bovp;      //�����洢DAC��ʼ������ֵ
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
static void Peripheral_WE(void);
static void Peripheral_WP(void);
static void SystemClock_Config(void);
static void bsp_Init(void);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : Peripheral_WE()
**	����˵�� : MCU����Ĵ���дʹ�ܺ���
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Peripheral_WE(void)
{
    /* Unlock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Unlock();
    /* Unlock PWC register: FCG0 */
    PWC_FCG0_Unlock();
    /* Unlock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Unlock(PWC_UNLOCK_CODE_0 | PWC_UNLOCK_CODE_1 | PWC_UNLOCK_CODE_2);

    /* Unlock all EFM registers */
    EFM_Unlock();
}

/**********************************************************************************************************
**	�� �� �� : Peripheral_WP()
**	����˵�� : MCU����Ĵ���д��������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Peripheral_WP(void)
{
    /* Lock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Lock();
    /* Lock PWC register: FCG0 */
    PWC_FCG0_Lock();
    /* Lock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Lock(PWC_UNLOCK_CODE_0 | PWC_UNLOCK_CODE_1 | PWC_UNLOCK_CODE_2);
    /* Lock all EFM registers */
    EFM_Lock();
}

/**********************************************************************************************************
**	�� �� �� : PCA9555_Mode_Init()
**	����˵�� : 
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
uint8_t Pca9548ReadDat[8] = {0x00};

void PowerPCA9555_Mode_Init()
{
	uint8_t loop_ch = 0;
	
	GPIO_SetPins(I2C2_SMBA_GPIO,I2C2_SMBA_PIN);
	bsp_DelayMS(50);
	
	for(uint8_t ch_i = 0;ch_i < 8;ch_i++)
	{
		Res_9548 = Pca9548A_Select(I2C_PCA9548,ch_i,PCA_9555_ON);	//����9548ѡ�� ͨ��1
		bsp_DelayUS(100);
		Pca9548A_Read_Ch(I2C_PCA9548,&Pca9548ReadDat[ch_i]);	//9548�� Test
		
		PCA9555_IOConfigReg(I2C_PCA9548,PCA9555_DEVICE_ADDR,xPowPca9555_IOConfigSet_1); //���ʰ�PCA9555�˿�����
		bsp_Tmp75Init(I2C_PCA9548,TMP75_ADDR);	//��ʼ�����ʰ�TMP75
		HardwareFaultTypeRead(ch_i);//���ʰ�����źŶ�ȡ - ����жϡ���ȡ���ʰ�汾��
		vers_mana.Pow_Ver[ch_i] = Pca9555_Data.Power_Board[ch_i].ReadData[0];	//���ʰ�汾�Ż���
		Pca9548A_Select(I2C_PCA9548,9,PCA_9555_ALLOFF);	//�ر�����ͨ��
		bsp_DelayUS(100);
		
		//C033C��D�� IO��������
		Res_9548 = Pca9548A_Select(I2C_PCA9548,ch_i,PCA_9555_ON);	//����9548ѡ�� ͨ��1
		PCA9555_IOConfigReg(I2C_PCA9548,PCA9555_DEVICE_ADDR,xPowPca9555_IOConfigSet_2); //���ʰ�PCA9555�˿������������
		Pca9548A_Select(I2C_PCA9548,9,PCA_9555_ALLOFF);	//�ر�����ͨ��
		bsp_DelayUS(100);
		
		Feed_WDG();   
	}
	
	bsp_InitCharge();	//LM5170�����ƶ˿ڳ�ʼ��
	
	for(loop_ch = 0;loop_ch < 8;loop_ch++)
	{
		Disable_Mloop(loop_ch);	//MLoopΪ�ͣ�LoopΪ��
		bsp_DelayMS(2);
	}
	
	PCA9555_IOConfigReg(I2C_LED,PCA9555_VERDIAL_ADDR,xIOLEDVerConfigSet_1); //�ư�1�汾�˿�����
	bsp_DelayMS(1);
	Pca9555_RegRead(I2C_LED,PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].ReadData);	//�ư�1�汾��ȡ
	bsp_DelayMS(1);
	vers_mana.LED_Ver = Pca9555_Data.LED_Board[0].ReadData[0];	//�ư�汾�Ż���
	bsp_DelayMS(1);
	PCA9555_IOConfigReg(I2C_LED,PCA9555_VERDIAL_ADDR,xIOLEDVerConfigSet_2); //�ư�PCA9555_1 IO��������
	PCA9555_ConfigOutputReg(I2C_LED,PCA9555_LED_ADDR); //�ư�2����Ϊ���
	
	if((vers_mana.LED_Ver & 0xE0) == 0x00)	//8ͨ������ 000
	{
		used_channel_cfg = 8;	//ͨ����
	}
	else 
	if((vers_mana.LED_Ver & 0xE0) == 0x40)	//4ͨ������ 010
	{
		used_channel_cfg = 4;	//ͨ����
	}
	else if((vers_mana.LED_Ver & 0xE0) == 0x60)	//2ͨ������
	{
		used_channel_cfg = 2;	//ͨ����
	}
	else if((vers_mana.LED_Ver & 0xE0) == 0x20)	//6ͨ������
	{
		used_channel_cfg = 6;	//ͨ����
	}
}

/**********************************************************************************************************
**	�� �� �� : bsp_Init()
**	����˵�� : ��ʼ�����е�Ӳ���豸���ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����ֻ��Ҫ����һ�Ρ�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void bsp_Init(void)
{
	TIM6_4_PWMB_Config();	//��ʼ������PWM
	
	bsp_InitLed();
	bsp_InitI2C();
  bsp_Tmp75_AlertInit();         //��ʼ��Alert��    
  bsp_Tmp75Init(I2C_EE_TEMP,TMP75_ADDR);                         //��ʼ�����ذ�TMP75
	PowerPCA9555_Mode_Init(); //���ʰ�PCA9555��ʼ������
	
	ADC_Switch_ConfigGPIO();
	bsp_InitUSART();
	bsp_InitRNG();
	bsp_GpioInit();
	IP_Scanning();
	bsp_dacx0508_init();
	
	/*�����������MAC*/
	BarCode_Mac_CheckSet();  //����MACһ���Լ�鲢����MAC��3�ֽ�
	W5500_RST_Init();
//	Lan_cfg();
	W5500_SpiInit();
//	bsp_InitW5500();

#ifdef C042A_7606
	bsp_AD7606_init();             //��ʼ��7606
	bsp_SGM58601Init();            //��ʼ��58601
	AD7606Reset(&AD7606_A0);
	AD7606Reset(&AD7606_A1);
	Bovp = bsp_bovpInit();	//��ʼ���ڲ�DAC
	AD5245_Vpro_Set(Bovp, 5.1f);	//�����ڲ�DAC���������ѹ5.1v
#else
	bsp_AD7124Init();
	bsp_AD7175Init();
#endif

//	AD5245_Init();
	DMA_adc3_init();
	
	bsp_InitACDC();	//CANx��ʼ��
//	bsp_InitSysTick(); 

	PowerFault_Init();	//���ʰ���ϼ��IO��ʼ��

	TimerAUint1_Config(APP_TMRA1_PCLK_DIV, APP_TMRA1_PERIOD_VAL);  //TimerAUint1����40ms
	TimerAUint2_Config(APP_TMRA2_PCLK_DIV, APP_TMRA2_PERIOD_VAL);  //TimerAUint2����200ms
	TimerAUint3_Config(APP_TMRA3_PCLK_DIV, APP_TMRA3_PERIOD_VAL);  //TimerAUint3����1ms
	TimerAUint4_Config(APP_TMRA4_PCLK_DIV, APP_TMRA4_PERIOD_VAL);  //TimerAUint4����1ms
	
	TimerAUintStart(TMRA2_UNIT);
	TimerAUintStart(TMRA3_UNIT);
	TimerAUintStart(TMRA4_UNIT);

	Version_Mana_Read(); //���ذ�Ӳ���汾��ȡ
	AllIOInfo_Read();	//Ӳ�����IO��Ϣ��ȡ

	RMU_CPULockUpCmd(Enable);
/******************************  �������� ***************************/
#if 0
	GPIO_ResetPins(GPIO_PORT_CVK_4,GPIO_PIN_CVK_4);
	GPIO_ResetPins(GPIO_PORT_CVK_8,GPIO_PIN_CVK_8);
	
	uint8_t MS_j = 0;
	for(MS_j = 0;MS_j < 8;MS_j++)
	{
		if(MS_j%2 == 0)
			Chg_Dis_Port_Cfg(Channel_Port_Table[MS_j], PORT_MODE_MASTER);	//����ͨ������Ϊ����ģʽ
		else if(MS_j%2 == 1)
			Chg_Dis_Port_Cfg(Channel_Port_Table[MS_j], PORT_MODE_SLAVE);	//ż��ͨ������Ϊ�ӻ�ģʽ
	}
#endif
}

/**********************************************************************************************************
**	�� �� �� : sys_cfg_set()
**	����˵�� : ��ʼ��ϵͳ����Ĭ�ϲ�������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void sys_cfg_set(void)
{
	
}

/**********************************************************************************************************
**	�� �� �� : Error_Handler()
**	����˵�� : ��������
**	��    �� : file - Դ�����ļ�����(�ؼ���__FILE__��ʾԴ�����ļ���)
**             line - �����к�(�ؼ���__LINE__��ʾԴ�����к�)
**	�� �� ֵ : ��
**********************************************************************************************************/
void Error_Handler(char *file, uint32_t line)
{
	printf("\r\nWrong parameters value: file %s on line %d\r\n", file, line);	
	
	/* ����һ����ѭ��������ʧ��ʱ������ڴ˴��������Ա����û���� */
	if (line == 0)
	{
		return;
	}
	
	while(1)
	{
		
	}
}

/**********************************************************************************************************
**	�� �� �� : SystemClock_Config()
**	����˵�� : ��ʼ��ϵͳʱ��
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵    �� ��Clock source is XTAL   8MHz
               PLLHP_freq             ((PLL_source / PLLM) * PLLN) / PLLP = 240MHz
			   PLLM                   1
               PLLN                   120
               PLLP                   4
               PLLQ                   4
               PLLR                   4
               HCLK                   240MHz
               PCLK0                  240MHz
               PCLK1                  120MHz
               PCLK2                  60MHz
               PCLK3                  60MHz
               PCLK4                  120MHz
               EX_BUS                 120MHz
               EFM_WaitCycle          5
**********************************************************************************************************/
static void SystemClock_Config(void)
{
		stc_clk_pllh_init_t stcPLLHInit;
    stc_clk_xtal_init_t stcXtalInit;

    /* Configures XTAL. PLLH input source is XTAL. */
    (void)CLK_XtalStrucInit(&stcXtalInit);
    stcXtalInit.u8XtalState = CLK_XTAL_ON;
    stcXtalInit.u8XtalDrv   = CLK_XTALDRV_LOW;
    stcXtalInit.u8XtalMode  = CLK_XTALMODE_OSC;
    stcXtalInit.u8XtalStb   = CLK_XTALSTB_499US;
    (void)CLK_XtalInit(&stcXtalInit);

    /* PCLK0, HCLK  Max 240MHz */
    /* PCLK1, PCLK4 Max 120MHz */
    /* PCLK2, PCLK3 Max 60MHz  */
    /* EX BUS Max 120MHz */
    CLK_ClkDiv(CLK_CATE_ALL,                                        \
               (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 | \
                CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |  \
                CLK_HCLK_DIV1));

    (void)CLK_PLLHStrucInit(&stcPLLHInit);
    /* VCO = 8/1*120 = 960MHz*/
	 /*
     * PLLHP_freq 
	 *           = ((PLL_source / PLLM) * PLLN) / PLLP
     *           = (8 / 1) * 120 / 4
     *           = 240
	 * PLLP = PLLQ = PLLR -> PLLHP_freq = PLLHQ_freq = PLLHR_freq
     */
    stcPLLHInit.u8PLLState = CLK_PLLH_ON;
    stcPLLHInit.PLLCFGR    = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM = 1UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLN = 120UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLP = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLQ = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLR = 4UL - 1UL;

    /* stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLLSRC_XTAL; */
    (void)CLK_PLLHInit(&stcPLLHInit);
	
    /* Set EFM wait cycle. 5 wait cycles needed when system clock is 240MHz @ref User Guide's EFM */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE_5);

    (void)CLK_PLLHCmd(Enable);
    CLK_SetSysClkSrc(CLK_SYSCLKSOURCE_PLLH);
}

/**********************************************************************************************************
**  �� �� �� : User_Set_Vector()
**  ����˵�� : �ж��������ú���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void User_Set_Vector(void)
{
	uint8_t APP_Copy_Flag = 0;
	MB85Ctrl_Read(APP_Copy_Flag_ADD,&APP_Copy_Flag,1);     //��ȡAPP������־

	/*boot��������APP�� ���BOOT FLAGֵ����Ϊ0xAA
	�򿽱��ж�������APPƫ�Ƶ�ַ��APP��������ʹ���ж�*/
#if 1
	__disable_irq();
	SCB->VTOR = EFM_START_ADDR | 0x20000;          //�����ж�������ƫ����
	__enable_irq();
	while(APP_Copy_Flag !=0xAA)
	{
		APP_Copy_Flag=0xAA;
		MB85Ctrl_Write(APP_Copy_Flag_ADD,&APP_Copy_Flag,1);    //дAPP������־
		Feed_WDG();                                            //ι��
		MB85Ctrl_Read(APP_Copy_Flag_ADD,&APP_Copy_Flag,1);     //��ȡAPP������־
		if(APP_Copy_Flag==0xAA)
			NVIC_SystemReset();	//ϵͳ����
	}
#endif
}

/**********************************************************************************************************
**	�� �� �� : System_Init()
**	����˵�� : ϵͳ��ʼ��(��Ҫ����MPU��Cache��Sys_Clk����)
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void System_Init(void)
{
	Peripheral_WE();   //����Ĵ���дʹ��
	SystemClock_Config();	//ϵͳʱ������
	GPIO_SetReadWaitCycle(GPIO_READ_WAIT_4);   //ϵͳʱ��240M��GPIO��������Ҫ�ȴ�4��ʱ������
	
	TimerAUintStop(TMRA1_UNIT);            //��ͣTIMA1
	NVIC_DisableIRQ(APP_TMRA1_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
	TimerAUintStop(TMRA2_UNIT);            //��ͣTIMA2
	NVIC_DisableIRQ(APP_TMRA2_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
	TimerAUintStop(TMRA3_UNIT);            //��ͣTIMA3
	NVIC_DisableIRQ(APP_TMRA3_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
	TimerAUintStop(TMRA4_UNIT);            //��ͣTIMA4
	NVIC_DisableIRQ(APP_TMRA4_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
	
	NVIC_DisableIRQ(AD7124_IRQn);          //�ر�7124�ж�
	NVIC_DisableIRQ(AD7175_IRQn);          //�ر�7175�ж�
	
	bsp_InitDWT();                             //��ʼ��DWT
	bsp_InitSWDT();                            //��ʼ��������Ź�
	bsp_HWDG_InitGPIO();                       //��ʼ��Ӳ�����Ź�IO

	Feed_WDG();                                //ι��
	W25Q_MB85R_SpiInit();                      //��ʼ������ 

	User_Set_Vector();                        //���CP��־�������ж�����ƫ�� 20220228 ���Ρ��ݲ�ʹ�øù���	

	bsp_Init();	//bsp��ʼ��
	
	Peripheral_WP();  //����Ĵ���д����
}

/**********************************************************************************************************
**	�� �� �� : bsp_Idle()
**	����˵�� : ����ʱִ�еĺ�����һ����������for��whileѭ������������Ҫ����CPU_IDLE()�������ñ�������
**             ������ȱʡΪ�ղ������û��������ι��������CPU��������ģʽ�ȹ��ܡ�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_Idle(void)
{
	/* ι�� */

	/* ��CPU�������ߣ���Systick��ʱ�жϻ��ѻ��������жϻ��� */
}

/**********************************************************************************************************
**  �� �� �� : Dis_int()
**  ����˵�� : ���жϣ�����ʱ����
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void Dis_int(void)
{
	NVIC_DisableIRQ(APP_TMRA1_IRQn); 
	NVIC_DisableIRQ(APP_TMRA2_IRQn);
	NVIC_DisableIRQ(APP_CAN1_IRQn);
	NVIC_DisableIRQ(APP_CAN2_IRQn); 	
	NVIC_DisableIRQ(Int012_IRQn);	//USART
	NVIC_DisableIRQ(Int013_IRQn);

	NVIC_DisableIRQ(SGM58601_IRQn);
	NVIC_DisableIRQ(AD7606_IRQn);
	NVIC_DisableIRQ(AD7606_Busy0_IRQn);
//	NVIC_DisableIRQ(AD7175_IRQn);
//	NVIC_DisableIRQ(AD7124_IRQn);

	NVIC_DisableIRQ(POWERFAULT_1_IRQn);
	NVIC_DisableIRQ(POWERFAULT_2_IRQn);
	NVIC_DisableIRQ(POWERFAULT_3_IRQn);
	NVIC_DisableIRQ(POWERFAULT_4_IRQn);
	NVIC_DisableIRQ(POWERFAULT_5_IRQn);
	NVIC_DisableIRQ(POWERFAULT_6_IRQn);
	NVIC_DisableIRQ(POWERFAULT_7_IRQn);
	NVIC_DisableIRQ(POWERFAULT_8_IRQn);

	NVIC_DisableIRQ(APP_TMRA3_IRQn);
	NVIC_DisableIRQ(APP_DMA_IRQn);
	NVIC_DisableIRQ(APP_TMRA4_IRQn);
	bsp_DelayUS(1);
}

/**********************************************************************************************************
**  �� �� �� : En_int()
**  ����˵�� : ���жϣ�����ʱ����
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void En_int(void)
{    
	NVIC_EnableIRQ(APP_TMRA1_IRQn); 
	NVIC_EnableIRQ(APP_TMRA2_IRQn);
	NVIC_EnableIRQ(APP_CAN1_IRQn);       
	NVIC_EnableIRQ(APP_CAN2_IRQn);
	NVIC_EnableIRQ(Int012_IRQn);
	NVIC_EnableIRQ(Int013_IRQn);

	NVIC_EnableIRQ(SGM58601_IRQn);
	NVIC_EnableIRQ(AD7606_IRQn);
	NVIC_EnableIRQ(AD7606_Busy0_IRQn);
//	NVIC_EnableIRQ(AD7175_IRQn);
//	NVIC_EnableIRQ(AD7124_IRQn);

	NVIC_EnableIRQ(POWERFAULT_1_IRQn);
	NVIC_EnableIRQ(POWERFAULT_2_IRQn);
	NVIC_EnableIRQ(POWERFAULT_3_IRQn);
	NVIC_EnableIRQ(POWERFAULT_4_IRQn);
	NVIC_EnableIRQ(POWERFAULT_5_IRQn);
	NVIC_EnableIRQ(POWERFAULT_6_IRQn);
	NVIC_EnableIRQ(POWERFAULT_7_IRQn);
	NVIC_EnableIRQ(POWERFAULT_8_IRQn);	
	
	NVIC_EnableIRQ(APP_DMA_IRQn);
	NVIC_EnableIRQ(APP_TMRA3_IRQn);
	NVIC_EnableIRQ(APP_TMRA4_IRQn);
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
