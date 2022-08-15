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


/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/

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
 void Peripheral_WE(void)
{
    /* Unlock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Unlock();
    /* Unlock PWC register: FCG0 */
    PWC_FCG0_Unlock();
    /* Unlock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Unlock(PWC_UNLOCK_CODE_0 | PWC_UNLOCK_CODE_1 | PWC_UNLOCK_CODE_2);
  
    /* Unlock all EFM registers */
    EFM_Unlock();                    //������е�EFM�Ĵ���д����  
    /* Unlock EFM register: FWMC */
    EFM_FWMC_Unlock();               //���FWMC�Ĵ���д������FWMC�Ĵ��������ñ�̲���ģʽ������״̬��  

}

/**********************************************************************************************************
**	�� �� �� : Peripheral_WP()
**	����˵�� : MCU����Ĵ���д��������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
 void Peripheral_WP(void)
{
    /* Lock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Lock();
    /* Lock PWC register: FCG0 */
    PWC_FCG0_Lock();
    /* Lock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Lock(PWC_UNLOCK_CODE_0 | PWC_UNLOCK_CODE_1 | PWC_UNLOCK_CODE_2);
    
    /* lock EFM register: FWMC */
    EFM_FWMC_Lock();               //���FWMC�Ĵ���д������FWMC�Ĵ��������ñ�̲���ģʽ������״̬��  
    /* lock all EFM registers */
    EFM_Lock();                    //������е�EFM�Ĵ���д����  

    
    
   
}

/**********************************************************************************************************
**	�� �� �� : bsp_Init()
**	����˵�� : ��ʼ�����е�Ӳ���豸���ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����ֻ��Ҫ����һ�Ρ�
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void bsp_Init(void)
{
	bsp_InitDWT();                 //��ʼ��DWT
	bsp_InitLed();                 //��ʼ��LED��IO 
	bsp_InitSWDT();                //��ʼ��������Ź�
	bsp_FR_ADDR_InitGPIO();        //��ʼ��ǰ��岦��IO
	bsp_HWDG_InitGPIO();           //��ʼ��Ӳ�����Ź�
	W25Q_MB85R_SpiInit();          //��ʼ������flash����SPI�ӿ�
	TimerAUint1_Config(APP_TMRA1_PCLK_DIV, APP_TMRA1_PERIOD_VAL);  //����TimerAUint1����8ms    
    TimerAUintStart(TMRA1_UNIT);                                   //����TimerAUint1

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
**	�� �� �� : System_Init()
**	����˵�� : ϵͳ��ʼ��(��Ҫ����MPU��Cache��Sys_Clk����)
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void System_Init(void)
{

	
	SystemClock_Config();   //����ϵͳʱ��
	bsp_Init();             //����ϵͳ����
	

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
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
