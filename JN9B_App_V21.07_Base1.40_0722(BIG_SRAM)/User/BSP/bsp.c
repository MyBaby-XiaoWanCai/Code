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
#define Ret_RAM_ADDR 0x200F0000
uint32_t *pdata = (uint32_t *)Ret_RAM_ADDR;
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
//extern Vers_Mana vers_mana;	 //���ذ弰�ư��ȡ�汾��Ϣ�洢
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
void PowerPCA9555_Mode_Init()
{
	GPIO_SetPins(I2C2_SMBA_GPIO,I2C2_SMBA_PIN);
	bsp_DelayMS(50);
	
	for(uint8_t ch_i = 0;ch_i < 8;ch_i++)
	{
		Res_9548 = Pca9548A_Select(I2C_PCA9548,ch_i,PCA_9555_ON);	//����9548ѡ�� ͨ��1
		bsp_DelayUS(100);

if(Bom_Vers == C042C_BOOM_VER)
{
		PCA9555_IOConfigReg(I2C_PCA9548,PB_Addr_Arr[ch_i],xPowPca9555_IOConfigSet_1); //���ʰ�PCA9555�˿�����
		bsp_Tmp75Init(I2C_PCA9548,Tmp75AddrArr[ch_i]);	//��ʼ�����ʰ�TMP75
}
else
//if(vers_mana.Bom_Vers == C042B_BOOM_VER)	
{
		PCA9555_IOConfigReg(I2C_PCA9548,PCA9555_POWER_ADDR,xPowPca9555_IOConfigSet_1); //���ʰ�PCA9555�˿�����
		bsp_Tmp75Init(I2C_PCA9548,TMP75_ADDR_MAIN);	//��ʼ�����ʰ�TMP75
}
//		HardwareFaultTypeRead(ch_i);//���ʰ�����źŶ�ȡ - ����жϡ���ȡ���ʰ�汾��
//		Pow_Ver[ch_i] = Pca9555_Data.Power_Board[ch_i].ReadData[0];	//���ʰ�汾�Ż���
//		Pca9548A_Select(I2C_PCA9548,9,PCA_9555_ALLOFF);	//�ر�����ͨ��
//		bsp_DelayUS(100);
//		
//		//C033C��D�� IO��������
//		Res_9548 = Pca9548A_Select(I2C_PCA9548,ch_i,PCA_9555_ON);	//����9548ѡ�� ͨ��1

//if(Bom_Vers == C042C_BOOM_VER)
//{
//		PCA9555_IOConfigReg(I2C_PCA9548,PB_Addr_Arr[ch_i],xPowPca9555_IOConfigSet_2); //���ʰ�PCA9555�˿������������
//}
//else
////if(vers_mana.Bom_Vers == C042B_BOOM_VER)
//{
//		PCA9555_IOConfigReg(I2C_PCA9548,PCA9555_POWER_ADDR,xPowPca9555_IOConfigSet_2); //���ʰ�PCA9555�˿������������
//}
// 
//		Pca9548A_Select(I2C_PCA9548,9,PCA_9555_ALLOFF);	//�ر�����ͨ��
//		bsp_DelayUS(100);
//		
//		Feed_WDG();   
	}
	
	bsp_DelayMS(500);	//2022.05.31
	Feed_WDG(); 
	bsp_DelayMS(500);
	Feed_WDG(); 
	
	bsp_InitCharge();	//LM5170�����ƶ˿ڳ�ʼ��
	
	for(uint8_t _ch = 0;_ch < 8;_ch++)
	{
		Disable_Mloop(_ch);	//MLoopΪ�ͣ�LoopΪ�� - Ĭ�Ͽ��ٻ�
		bsp_DelayMS(1);
		
		HardwareFaultTypeRead(_ch);//���ʰ�����źŶ�ȡ - ����жϡ���ȡ���ʰ�汾��
		Pow_Ver[_ch] = Pca9555_Data.Power_Board[_ch].ReadData[0];	//���ʰ�汾�Ż���
		Pca9548A_Select(I2C_PCA9548,9,PCA_9555_ALLOFF);	//�ر�����ͨ��
		bsp_DelayUS(100);
		
		//C033C��D�� IO��������
		Res_9548 = Pca9548A_Select(I2C_PCA9548,_ch,PCA_9555_ON);	//����9548ѡ�� ͨ��1

if(Bom_Vers == C042C_BOOM_VER)
{
		PCA9555_IOConfigReg(I2C_PCA9548,PB_Addr_Arr[_ch],xPowPca9555_IOConfigSet_2); //���ʰ�PCA9555�˿������������
}
else
//if(vers_mana.Bom_Vers == C042B_BOOM_VER)
{
		PCA9555_IOConfigReg(I2C_PCA9548,PCA9555_POWER_ADDR,xPowPca9555_IOConfigSet_2); //���ʰ�PCA9555�˿������������
}
 
		Pca9548A_Select(I2C_PCA9548,9,PCA_9555_ALLOFF);	//�ر�����ͨ��
		bsp_DelayUS(100);
		
		Feed_WDG(); 		
	}
	
	PCA9555_IOConfigReg(I2C_LED,PCA9555_VERDIAL_ADDR,xIOLEDVerConfigSet_1); //�ư�1�汾�˿�����
	bsp_DelayMS(1);
	Pca9555_RegRead(I2C_LED,PCA9555_VERDIAL_ADDR,Pca9555_Data.LED_Board[0].ReadData);	//�ư�1�汾��ȡ
	bsp_DelayMS(1);
	LED_Ver = Pca9555_Data.LED_Board[0].ReadData[0];	//�ư�汾�Ż���
	bsp_DelayMS(1);
	PCA9555_IOConfigReg(I2C_LED,PCA9555_VERDIAL_ADDR,xIOLEDVerConfigSet_2); //�ư�PCA9555_1 IO��������
	PCA9555_ConfigOutputReg(I2C_LED,PCA9555_LED_ADDR); //�ư�2����Ϊ���
	
	if((LED_Ver & 0xE0) == 0x00)	//8ͨ������ 000
	{
		used_channel_cfg = 8;	//ͨ����
	}
	else 
	if((LED_Ver & 0xE0) == 0x40)	//4ͨ������ 010
	{
		used_channel_cfg = 4;	//ͨ����
	}
	else 
	if((LED_Ver & 0xE0) == 0x60)	//2ͨ������ 011
	{
		used_channel_cfg = 2;	//ͨ����
	}
	else 
	if((LED_Ver & 0xE0) == 0x20)	//6ͨ������ 001
	{
		used_channel_cfg = 6;	//ͨ����
	}
	
	if(LED_Ver == 0x80)	//4CH 240Aͨ������ 100
	{
		used_channel_cfg = 8;	//ͨ����
		Weaver_Config = 4;	//��������
	}
	else 
	if(LED_Ver == 0xA0)	//1CH 600Aͨ������ 101
	{
		used_channel_cfg = 8;	//ͨ����
		Weaver_Config = 1;	//��������	
	}

/******************************* ������������ *********************************/
	
	if (Weaver_Config >= 1)	//8���� 1CH 600A
	{
		ChannelTab[0] = CHANNEL1_1;
		ALLCH = 0x01;
		HALFCH1 = 0x01;
		HALFCH2 = 0x01;
		
		if(Weaver_Config >= 2)	//4����
		{
			ChannelTab[0] = CHANNEL2_1;
			ChannelTab[1] = CHANNEL2_2;
			ALLCH = 0x03;
			HALFCH1 = 0x01;
			HALFCH2 = 0x02;			
			
			if(Weaver_Config >= 4)	//2���� 4CH 240A
			{
				ChannelTab[0] = CHANNEL4_1;
				ChannelTab[1] = CHANNEL4_2;
				ChannelTab[2] = CHANNEL4_3;
				ChannelTab[3] = CHANNEL4_4;
				ALLCH = 0x0F;
				HALFCH1 = 0x03;
				HALFCH2 = 0x0C;			
				
				if(Weaver_Config >= 8)	//���� 8CH 120A
				{		
					ChannelTab[0] = CHANNEL8_1;
					ChannelTab[1] = CHANNEL8_2;
					ChannelTab[2] = CHANNEL8_3;
					ChannelTab[3] = CHANNEL8_4;
					ChannelTab[4] = CHANNEL8_5;
					ChannelTab[5] = CHANNEL8_6;
					ChannelTab[6] = CHANNEL8_7;
					ChannelTab[7] = CHANNEL8_8;
					ALLCH = 0xFF;
					HALFCH1 = 0x0F;
					HALFCH2 = 0xF0;							
				}
			}
		}	
	}
	
	IP_CheckList[0] = (CMDProject){0x01, IntelProtect_OLA, 0, 0}; // �����������
	IP_CheckList[1] = (CMDProject){0x02, IntelProtect_BOV, 0, 0}; // BUS��ѹ
	IP_CheckList[2] = (CMDProject){0x03, IntelProtect_BUV, 0, 0}; // BUSǷѹ
	IP_CheckList[3] = (CMDProject){0x04, IntelProtect_APA, 0, 0}; // �������ȸ澯
	IP_CheckList[4] = (CMDProject){0x05, IntelProtect_UPA, 0, 0}; // ��ѹ���ȸ澯
	IP_CheckList[5] = (CMDProject){0x06, IntelProtect_VDQ, 0, 0}; // ��·�����澯
	IP_CheckList[6] = (CMDProject){0x07, IntelProtect_MOT, 0, 0}; // ģ�����
	IP_CheckList[7] = (CMDProject){0x08, IntelProtect_EOT, 0, 0}; // ��������
	IP_CheckList[8] = (CMDProject){0x09, IntelProtect_HPA, 0, 0}; // Ӳ������
	IP_CheckList[9] = (CMDProject){0x0a, IntelProtect_FHE, ALLCH, ALLCH};	// ���ȹ���,û��ʹ���������Ĭ�Ͽ���
	IP_CheckList[10] = (CMDProject){0x0b, IntelProtect_UHE, ALLCH, ALLCH};	// CPU�쳣,û��ʹ���������Ĭ�Ͽ���
	IP_CheckList[11] = (CMDProject){0x0c, IntelProtect_C1E, ALLCH, ALLCH};	// can1�쳣,û��ʹ���������Ĭ�Ͽ���
	IP_CheckList[12] = (CMDProject){0x0d, IntelProtect_C2E, ALLCH, ALLCH};	// can2�쳣,û��ʹ���������Ĭ�Ͽ���
	IP_CheckList[13] = (CMDProject){0x0e, IntelProtect_DAE, ALLCH, ALLCH};	// DA�쳣,û��ʹ���������Ĭ�Ͽ���
	IP_CheckList[14] = (CMDProject){0x0f, IntelProtect_ADE, ALLCH, ALLCH};	// AD�쳣,û��ʹ���������Ĭ�Ͽ���
	IP_CheckList[15] = (CMDProject){0x10, IntelProtect_FLE, ALLCH, ALLCH};	// FLASH�쳣,û��ʹ���������Ĭ�Ͽ���
	IP_CheckList[16] = (CMDProject){0x11, IntelProtect_EEE, ALLCH, ALLCH};	// Eeprom�쳣,û��ʹ���������Ĭ�Ͽ���
	IP_CheckList[17] = (CMDProject){0x12, IntelProtect_FRE, ALLCH, ALLCH};	// FRAM�쳣,û��ʹ���������Ĭ�Ͽ���
	IP_CheckList[18] = (CMDProject){0x13, IntelProtect_PME, ALLCH, ALLCH};	// ��λ���쳣,û��ʹ���������Ĭ�Ͽ���
	IP_CheckList[19] = (CMDProject){0x14, IntelProtect_VNN, 0, 0};			// ��ѹ�쳣��Э������
	IP_CheckList[20] = (CMDProject){0x15, IntelProtect_CVP, 0, 0};			// ��·ѹ�����Э������
	IP_CheckList[21] = (CMDProject){0x16, IntelProtect_VUQ, 0, 0};			// ��ѹ����������Э������
	IP_CheckList[22] = (CMDProject){0x17, IntelProtect_CNN, 0, 0};			// �����쳣��Э������
	IP_CheckList[23] = (CMDProject){0x18, IntelProtect_CUQ, 0, 0};			// ��������������Э������
	IP_CheckList[24] = (CMDProject){0x19, IntelProtect_LOV, 0, 0};			// ������ѹ�쳣��̽���ѹ����5.1V����Э������
	IP_CheckList[25] = (CMDProject){0x1A, IntelProtect_MAC, 0, 0};			// MAC�쳣
  IP_CheckList[26] = (CMDProject){0x1B, IntelProtect_CHE, 0, 0};			// ͨ��ģ���쳣
	IP_CheckList[27] = (CMDProject){0x1C, IntelProtect_OSF, 0, 0};			// �����·
	IP_CheckList[28] = (CMDProject){0x1D, IntelProtect_BSF, 0, 0};			// ��ض�·
	IP_CheckList[29] = (CMDProject){0x1E, IntelProtect_NEG, 0, 0};			// ��ط��ӱ��� - NEG
	IP_CheckList[30] = (CMDProject){0x1F, IntelProtect_CDE, 0, 0};			// У׼�����쳣
	IP_CheckList[31] = (CMDProject){0x20, IntelProtect_SDE, 0, 0};			// ���������쳣
	IP_CheckList[32] = (CMDProject){0x21, IntelProtect_OCP, 0, 0};			// Ӳ������ - OCP IntelProtect_OCP
	IP_CheckList[33] = (CMDProject){0x22, IntelProtect_OVP, 0, 0};			// Ӳ������ - OVP IntelProtect_OVP
	IP_CheckList[34] = (CMDProject){0x23, IntelProtect_OTP, 0, 0};			// Ӳ������ - OTP IntelProtect_OTP
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
	bsp_InitUSART();
	bsp_GpioInit();	//Test
	bsp_InitLed();
	bsp_InitI2C();
  bsp_Tmp75_AlertInit();         //��ʼ��Alert��    
  bsp_Tmp75Init(I2C_EE_TEMP,TMP75_ADDR_MAIN);                         //��ʼ�����ذ�TMP75
	PowerPCA9555_Mode_Init(); //���ʰ�PCA9555��ʼ������
	
//	ADC_Switch_ConfigGPIO();
	bsp_InitRNG();
	IP_Scanning();
	bsp_dacx0508_init();
	
	/*�����������MAC*/
//	BarCode_Mac_CheckSet();  //����MACһ���Լ�鲢����MAC��3�ֽ�
//	W5500_RST_Init();
//	Lan_cfg();
//	W5500_SpiInit();
//	bsp_InitW5500();

	GPIO_SetDebugPort(GPIO_PIN_TDI, Disable);
#ifdef C042A_7606
	bsp_AD7606_init();             //��ʼ��7606
	bsp_SGM58601Init();            //��ʼ��58601
	AD7606Reset(&AD7606_A0);
	AD7606Reset(&AD7606_A1);
#else
	bsp_AD7124Init();
	bsp_AD7175Init();
#endif
	Bovp = bsp_bovpInit();	//��ʼ���ڲ�DAC
	AD5245_Vpro_Set(Bovp, 7.2f);	//�����ڲ�DAC���������ѹ5.1v����ʼ����ֵ����Ϊ��� 2022.04.29

//	AD5245_Init();
	ADC_Switch_ConfigGPIO();
	DMA_adc3_init();
	
//	for(uint8_t i = 0;i < 10;i++)
//	{
//		bsp_DelayMS(500);
//		Feed_WDG();
//	}
//	bsp_InitACDC();	//CANx��ʼ��

//	bsp_InitSysTick(); 

	PowerFault_Init();	//���ʰ���ϼ��IO��ʼ��

	TimerAUint1_Config(APP_TMRA1_PCLK_DIV, APP_TMRA1_PERIOD_VAL);  //TimerAUint1����8ms
//	TimerAUint2_Config(APP_TMRA2_PCLK_DIV, APP_TMRA2_PERIOD_VAL);  //TimerAUint2����4ms
	TimerAUint3_Config(APP_TMRA3_PCLK_DIV, APP_TMRA3_PERIOD_VAL);  //TimerAUint3����1ms
	TimerAUint4_Config(APP_TMRA4_PCLK_DIV, APP_TMRA4_PERIOD_VAL);  //TimerAUint4����1ms
	
//	TimerAUintStart(TMRA2_UNIT);
	TimerAUintStart(TMRA3_UNIT);
	TimerAUintStart(TMRA4_UNIT);

	AllIOInfo_Read();	//Ӳ�����IO��Ϣ��ȡ

	RMU_CPULockUpCmd(Enable);

	bsp_DelayMS(1000);
	
	bsp_InitACDC();	//CANx��ʼ��
	TimerAUint2_Config(APP_TMRA2_PCLK_DIV, APP_TMRA2_PERIOD_VAL);  //TimerAUint2����4ms
	TimerAUintStart(TMRA2_UNIT);
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
    (void)CLK_XtalStructInit(&stcXtalInit);
    stcXtalInit.u8XtalState = CLK_XTAL_ON;
    stcXtalInit.u8XtalDrv   = CLK_XTALDRV_LOW;
    stcXtalInit.u8XtalMode  = CLK_XTALMODE_OSC;
    stcXtalInit.u8XtalStb   = CLK_XTALSTB_31MS;	//CLK_XTALSTB_499US
    (void)CLK_XtalInit(&stcXtalInit);

    /* PCLK0, HCLK  Max 240MHz */
    /* PCLK1, PCLK4 Max 120MHz */
    /* PCLK2, PCLK3 Max 60MHz  */
    /* EX BUS Max 120MHz */
    CLK_ClkDiv(CLK_CATE_ALL,                                        \
               (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 | \
                CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |  \
                CLK_HCLK_DIV1));

    (void)CLK_PLLHStructInit(&stcPLLHInit);
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

    SRAM_WTCR_Unlock(); 
    SRAM_CKCR_Unlock();
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE_0, SRAM_WAIT_CYCLE_0); //SRAM_WAIT_CYCLE_0
//    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE_2, SRAM_WAIT_CYCLE_2);
		SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE_1, SRAM_WAIT_CYCLE_1);	//2022.05.11�޸Ĳ���
    SRAM_WTCR_Lock(); 
    SRAM_CKCR_Lock();
    
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
	MB85Ctrl_Read(APP_Copy_Flag_ADD,&APP_Copy_Flag,1 ,0);     //��ȡAPP������־

	/*boot��������APP�� ���BOOT FLAGֵ����Ϊ0xAA
	�򿽱��ж�������APPƫ�Ƶ�ַ��APP��������ʹ���ж�*/
#if 1
	__disable_irq();
	SCB->VTOR = EFM_START_ADDR | 0x20000;          //�����ж�������ƫ����
	__enable_irq();
	while(APP_Copy_Flag !=0xAA)
	{
		APP_Copy_Flag=0xAA;
		MB85Ctrl_Write(APP_Copy_Flag_ADD,&APP_Copy_Flag,1 ,0);    //дAPP������־
		Feed_WDG();                                            //ι��
		MB85Ctrl_Read(APP_Copy_Flag_ADD,&APP_Copy_Flag,1 ,0);     //��ȡAPP������־
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

	Feed_WDG();		//ι��
	bsp_InitMB85R();
	
	User_Set_Vector();                        //���CP��־�������ж�����ƫ�� 20220228 ���Ρ��ݲ�ʹ�øù���	

	bsp_Init();	//bsp��ʼ��

	/* Reset the VBAT area */
	PWC_VBAT_Reset();
    
  for(int i= 0;i<10;i++)
	{
		if(pdata[i] != i)	
		{
			printf("power up\r\n");   //�ϵ�����
			 break;
		}
		else if(i==9) 
		{
      for(uint8_t j = 0; j < CHANNEL_CFG; j++)
			{ 
				IP_Alarm_Flag[j] |= IntelProtect_UHE;    //CPU�쳣
			}
			printf("rst pin\r\n");   //�Ƕϵ�����
		}
	}
	
	for(int i= 0;i<10;i++)pdata[i] = i;

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
//	NVIC_DisableIRQ(APP_CAN1_IRQn);
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
//	NVIC_DisableIRQ(APP_TMRA4_IRQn);
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
//	NVIC_EnableIRQ(APP_CAN1_IRQn);       
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
//	NVIC_EnableIRQ(APP_TMRA4_IRQn);
}

/*�û��Զ����ж�����ö��*/
typedef enum
{
    UD_TIM1_INT      =  0x01<<0,
    UD_TIM2_INT      =  0x01<<1,
    UD_CAN1_INT      =  0x01<<2,
    UD_CAN2_INT      =  0x01<<3,
    UD_UARTRI_INT    =  0x01<<4,
    UD_UARTEI_INT    =  0x01<<5,
    UD_SGM58601_INT  =  0x01<<6,
    UD_AD7606A1_INT  =  0x01<<7,
    UD_AD7606A0_INT  =  0x01<<8,
    UD_AD7175_INT    =  0x01<<9,
    UD_AD7124_INT    =  0x01<<10,
    UD_PG1_INT       =  0x01<<11,
    UD_PG2_INT       =  0x01<<12,
    UD_PG3_INT       =  0x01<<13,
    UD_PG4_INT       =  0x01<<14,
    UD_PG5_INT       =  0x01<<15,
    UD_PG6_INT       =  0x01<<16,
    UD_PG7_INT       =  0x01<<17,
    UD_PG8_INT       =  0x01<<18,
    UD_ADCDMA_INT    =  0x01<<19,
    UD_TIM3_INT      =  0x01<<20,
    UD_TIM4_INT      =  0x01<<21,
    ALL_INT          =  0xFFFFFF
}Int_Type;


/**********************************************************************************************************
**  �� �� �� : Dis_int_UserDefine()
**  ����˵�� : ��ָ���жϣ�����ʱ����
**  ��    �� : OP_INT��ҪDISABLE���жϣ�������Int_Type�����
**  �� �� ֵ : ��
**********************************************************************************************************/
void Dis_int_UserDefine(void)
{
	NVIC_DisableIRQ(APP_TMRA1_IRQn); 
	NVIC_DisableIRQ(APP_TMRA2_IRQn);
//	NVIC_DisableIRQ(APP_CAN1_IRQn);
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
//	NVIC_DisableIRQ(APP_TMRA4_IRQn);
	bsp_DelayUS(1);
}

/**********************************************************************************************************
**  �� �� �� : En_int_UserDefine()
**  ����˵�� : ��ָ���ж�
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void En_int_UserDefine(void)
{    
	NVIC_EnableIRQ(APP_TMRA1_IRQn); 
	NVIC_EnableIRQ(APP_TMRA2_IRQn);
//	NVIC_EnableIRQ(APP_CAN1_IRQn);       
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
//	NVIC_EnableIRQ(APP_TMRA4_IRQn);
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
