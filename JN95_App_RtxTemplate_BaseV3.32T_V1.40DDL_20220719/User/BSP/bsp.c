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
        /* Unlock EFM register: FWMC */
    EFM_FWMC_Unlock();               //���FWMC�Ĵ���д������FWMC�Ĵ��������ñ�̲���ģʽ������״̬��
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
    
     /* lock EFM register: FWMC */
    EFM_FWMC_Lock();               //���FWMC�Ĵ���д������FWMC�Ĵ��������ñ�̲���ģʽ������״̬�� 
    /* Lock all EFM registers */
    EFM_Lock();
}

/**********************************************************************************************************
**	�� �� �� : bsp_Init()
**	����˵�� : ��ʼ�����е�Ӳ���豸���ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����ֻ��Ҫ����һ�Ρ�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void bsp_Init(void)
{
	bsp_InitLed();                 //��ʼ���ư�
	bsp_InitCharge();           
	ADC_Switch_ConfigGPIO();
	
	bsp_InitUSART();
	bsp_InitRNG();
	
	bsp_M_Loop_InitGPIO();
	bsp_FR_ADDR_InitGPIO();
	
	
	bsp_dacx0508_init();
	
	W5500_RST_Init();
    IP_Scanning();
//	Lan_cfg();
	W5500_SpiInit();
//	bsp_InitW5500();
	bsp_AD7124Init();
	bsp_AD7175Init();

	bsp_InitI2C();
	
#ifdef EE_Test
    uint8_t  TDate[300]= {
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x26,0x07,0x08,0x78,0xFE,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
        0x01,0x02,0x03,0x04,0x05,0x26,0x07,0x08,0x78,0xFE
    };
    uint8_t  RDate[300]={0};
    ee_WriteBytes(TDate,2,300);
    ee_ReadBytes(RDate,2,300);
    for(uint16_t i=0;i<300;i++)
    ee_ReadBytes(&RDate[i], 2 + i, 1); //�������
#endif	
	
	AD5245_Init();
	
	
	DMA_adc3_init();
	bsp_InitACDC();
//	bsp_InitSysTick();  
     
     
	TimerAUint1_Config(APP_TMRA1_PCLK_DIV, APP_TMRA1_PERIOD_VAL);  //TimerAUint1����40ms
	TimerAUint2_Config(APP_TMRA2_PCLK_DIV, APP_TMRA2_PERIOD_VAL);  //TimerAUint2����200ms
	TimerAUint3_Config(APP_TMRA3_PCLK_DIV, APP_TMRA3_PERIOD_VAL);  //TimerAUint3����1ms
	
//	AllTimerAUintStart();

//  TimerAUintStart(TMRA1_UNIT);
	TimerAUintStart(TMRA2_UNIT);
	TimerAUintStart(TMRA3_UNIT);
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
    stcXtalInit.u8XtalStb   = CLK_XTALSTB_31MS;
    (void)CLK_XtalInit(&stcXtalInit);

    /* PCLK0, HCLK  Max 240MHz */
    /* PCLK1, PCLK4 Max 120MHz */
    /* PCLK2, PCLK3 Max 60MHz  */
    /* EX BUS Max 120MHz */
    CLK_ClkDiv(CLK_CATE_ALL,										\
               (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 |	\
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
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE_0, SRAM_WAIT_CYCLE_0);
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE_2, SRAM_WAIT_CYCLE_2);
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
		NVIC_SystemReset();                                    //ϵͳ����
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
	Peripheral_WE();           //����Ĵ���дʹ��
	
    SystemClock_Config();
    
    TimerAUintStop(TMRA1_UNIT);            //��ͣTIMA1
    NVIC_DisableIRQ(APP_TMRA1_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
    TimerAUintStop(TMRA2_UNIT);            //��ͣTIMA2
    NVIC_DisableIRQ(APP_TMRA2_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
    TimerAUintStop(TMRA3_UNIT);            //��ͣTIMA3
    NVIC_DisableIRQ(APP_TMRA3_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
    NVIC_DisableIRQ(AD7124_IRQn);          //�ر�7124�ж�
    NVIC_DisableIRQ(AD7175_IRQn);          //�ر�7175�ж�
    
    GPIO_SetReadWaitCycle(GPIO_READ_WAIT_4);   //GPIO����ʱ4���ڣ���ϵͳʱ���й�    
	bsp_InitDWT();                             //��ʼ��DWT
    bsp_InitSWDT();                            //��ʼ��������Ź�
    bsp_HWDG_InitGPIO();                       //��ʼ��Ӳ�����Ź�IO
    
    Feed_WDG();                                //ι��
	W25Q_MB85R_SpiInit();                      //��ʼ������ 
    User_Set_Vector();                         //���CP��־�������ж�����ƫ��
	bsp_Init();                                //���������ʼ��
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
				IP_Alarm_Flag[j] |= IntelProtect_UHE;    //CPU�쳣�澯���Ƕϵ��������
			}
			printf("rst pin\r\n");   //�Ƕϵ�����
		}
	}
	
	for(int i= 0;i<10;i++)pdata[i] = i;	
	Peripheral_WP();                           //����Ĵ���д����
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

//    TimerAUintStop(TMRA1_UNIT);            //��ͣTIMA1
//    NVIC_DisableIRQ(APP_TMRA1_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
//    TimerAUintStop(TMRA2_UNIT);            //��ͣTIMA2
//    NVIC_DisableIRQ(APP_TMRA2_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
//    TimerAUintStop(TMRA3_UNIT);            //��ͣTIMA3
//    NVIC_DisableIRQ(APP_TMRA3_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
//    NVIC_DisableIRQ(AD7124_IRQn);          //�ر�7124�ж�
//    NVIC_DisableIRQ(AD7175_IRQn);          //�ر�7175�ж�	
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
	  
//	NVIC_EnableIRQ(APP_TMRA1_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
//	TimerAUintStop(TMRA1_UNIT);            //��ͣTIMA1
//	NVIC_EnableIRQ(APP_TMRA2_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
//	TimerAUintStop(TMRA2_UNIT);            //��ͣTIMA2
//	NVIC_EnableIRQ(APP_TMRA3_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
//	TimerAUintStop(TMRA3_UNIT);            //��ͣTIMA3
//	NVIC_EnableIRQ(AD7124_IRQn);          //�ر�7124�ж�
//	NVIC_EnableIRQ(AD7175_IRQn);          //�ر�7175�ж�		
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
