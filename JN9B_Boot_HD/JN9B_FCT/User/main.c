/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : main.c
** Author      : wang liang
** Date        : 2021-11-22
** Description : RTX5 & HC32F4A0 Demo Project main file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "hc32_ddl.h"
#include "bsp.h"
#include "main.h"

/**********************************************************************************************************
**                                         ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
pFunction Jump_To_Application;   //��תִ�к���ָ��
uint32_t  JumpAddress;           //����ִ���׵�ַ

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/
/**********************************************************************************************************
**  �� �� �� : EFM_FCT_Init()
**  ����˵�� : ��׼C�������
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void EFM_FCT_Init(void)
{
    stc_efm_cfg_t stcEfmCfg;             //EFM���ýṹ�����
    en_int_status_t flag1;               //flash0 ready flag
    en_int_status_t flag2;               //flash1 ready flag
    /* EFM default config. */
    (void)EFM_StructInit(&stcEfmCfg);     //��ԭĬ��EFM����
    /*
     * Clock <= 40MHZ             EFM_WAIT_CYCLE_0
     * 40MHZ < Clock <= 80MHZ     EFM_WAIT_CYCLE_1
     * 80MHZ < Clock <= 120MHZ    EFM_WAIT_CYCLE_2
     * 120MHZ < Clock <= 160MHZ   EFM_WAIT_CYCLE_3
     * 160MHZ < Clock <= 200MHZ   EFM_WAIT_CYCLE_4
     * 200MHZ < Clock <= 240MHZ   EFM_WAIT_CYCLE_5
     */
    stcEfmCfg.u32WaitCycle = EFM_WAIT_CYCLE_5;   //���ݵ�ǰϵͳʱ�������޸�u32WaitCycle���ڸ���

    /* EFM config */
    (void)EFM_Init(&stcEfmCfg);                  //����EFM�Ĵ�����

    /* Wait flash0, flash1 ready. */             //�ȴ�flash0��flash1׼����
    do {
        flag1 = EFM_GetFlagStatus(EFM_FLAG_RDY0);
        flag2 = EFM_GetFlagStatus(EFM_FLAG_RDY1);
    } while((Set != flag1) || (Set != flag2));
}


/*
*********************************************************************************************************
*   �� �� ��: Boot_JMP_APP
*   ����˵��: ��ת��APP��ע�⣺�ж�������ƫ�ƴ�����APP���������
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void Boot_JMP_APP(void)
{
    __set_PRIMASK(1)    /* ��ֹȫ���ж� */;

    /*��ת������SPָ���ַ�Ƿ�Ϸ�(��SRAMH��ַ��Χ��),���� SRAMH ��ַ��Χ��0x1FFE0000~0x1FFFFFFF*/
    if(((*(__IO uint32_t *)APPLICATION_ADDRESS)>= 0x1FFE0000)&&((*(__IO uint32_t *)APPLICATION_ADDRESS)<=0x1FFFFFFF))
    {
        /* Jump to user application */
        JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS + 4);           // ȡ�ж��������еĸ�λ�жϵ�ַ
        Jump_To_Application = (pFunction) JumpAddress;                       // ��ת����ʹ�øõ�ַ
        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t *) APPLICATION_ADDRESS);                   //������ת��ַ������MSP

        Jump_To_Application();                                               //ִ����ת����ʵ��APP��ת
    }
}

/*
*********************************************************************************************************
*   �� �� ��: Erase_APP_Zone
*   ����˵��: ����APP������
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void Erase_APP_Zone(void)
{
    en_result_t Erase_Flag;
    EFM_SectorCmd_Sequential(EFM_ADDR_SECTOR16, 112 ,Enable);        //�ر�FCT APP����(16~127)д����
    __set_PRIMASK(1);                                                  //���ж�
    for(uint8_t i=0; i<112 ; i++)
    {
    do {
                                                    //�����ж�
        Erase_Flag=EFM_SectorErase(EFM_ADDR_SECTOR16 + i*0x2000);     //�������ÿһ����������112��������ÿ��������С8K
        Feed_WDG();                                                   //ι��
    } while(Erase_Flag != Ok );
    }
    __set_PRIMASK(0);                                                 //���ж�
}

/**********************************************************************************************************
**  �� �� �� : main()
**  ����˵�� : ��׼C�������
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
int32_t main(void)
{
    uint8_t APP_Copy_Flag=0;             //APP������־
    uint8_t state;                       //����ض�״̬
    uint32_t date=0;                     //FLASH�������ֻ���
    uint32_t i;                          //��ַ����    

    Peripheral_WE();                                      //����Ĵ���дʹ��
    System_Init();                                        //ϵͳ�������ʼ��
    EFM_FCT_Init();                                       //��ʼ��EFM�Ĵ���
    
    MB85Ctrl_Read(APP_Copy_Flag_ADD,&APP_Copy_Flag,1);     //��ȡAPP������־
    
     /*���������־=0xCC����ִ�п����������ݵ�APP����������,����ֱ����ת������*/
    if(APP_Copy_Flag==0xCC)
    {
        GPIO_ResetPins(GPIO_PORT_LED82, GPIO_PIN_LED82);                 //����LED82
        /*����ĿǰAPP����������*/
        Erase_APP_Zone();

        /*�������������ݵ�������,���ο���1���֣�4�ֽڣ�*/
        for( i=0; i<112*0x2000; i=i+4)
        {
            date= *(uint32_t *)(UPDATE_ADDRESS+i);                      //��ȡ����������
            EFM_SingleProgram(APPLICATION_ADDRESS+i , date);            //д��APP�� 
          if(!(i%0x2000))            
            GPIO_TogglePins(GPIO_PORT_LED83, GPIO_PIN_LED83);           //ÿ������һ����������תһ�ΰ��ӵ�          
        }
        
        GPIO_SetPins(GPIO_PORT_LED82, GPIO_PIN_LED82);                  //Ϩ��LED82
        
        /*�޸�APP_Copy_Flag=0xAA*/
        APP_Copy_Flag=0xAA;
        state = MB85Ctrl_CheckWrite(APP_Copy_Flag_ADD,&APP_Copy_Flag,1);             //�޸ĵĿ�����־д������洢��        
        EFM_SectorCmd_Sequential(EFM_ADDR_SECTOR16, 112 ,Disable);          //��FCT APP����д����        
        Peripheral_WP();                  //ʹ������Ĵ���д����
        
        /*�豸����*/
        if(state)                         //ȷ������ض��ɹ�
        {
          NVIC_SystemReset();             //�豸����
        }
    }
    else
    {
        Peripheral_WP();                       //ʹ������Ĵ���д����
        TimerAUintStop(TMRA1_UNIT);            //���ж�ǰ��ֹͣ
        NVIC_DisableIRQ(APP_TMRA1_IRQn);       //��תǰ�ص����жϣ���ֹAPPδʹ�ø��ж�ʱ�ٴο������жϺ��Ҳ������
        __set_PRIMASK(1);                      //�����ж�
        Boot_JMP_APP();                        //��תAPP������
    }
    
    /*���û����תAPP��ִ�е��˴�*/
    NVIC_EnableIRQ(APP_TMRA1_IRQn);       //ִ�е��˴���ʾ����תʧ�ܣ����´�ι����ʱ��
    
    while(1)
    {
       GPIO_SetPins(GPIO_PORT_LED82, GPIO_PIN_LED82);
       GPIO_ResetPins(GPIO_PORT_LED83, GPIO_PIN_LED83);
       bsp_DelayMS(1000);
       GPIO_TogglePins(GPIO_PORT_LED83, GPIO_PIN_LED83);    
       GPIO_TogglePins(GPIO_PORT_LED82, GPIO_PIN_LED82); 
       bsp_DelayMS(1000);        
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
