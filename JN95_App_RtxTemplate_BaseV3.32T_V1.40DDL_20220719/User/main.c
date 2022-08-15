/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : main.c
** Author      : Huang Cheng
** Date        : 2021-11-22
** Description : RTX5 & HC32F4A0 Demo Project main file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "hc32_ddl.h"
#include "bsp.h"
#include "main.h"
#include "easyflash.h"
/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
 #define MQ_SIZE 16                            /* ������Ϣ���������Ϣ���� */
 uint8_t put_Uart = 5;
uint8_t put_Uart2 = 8;
uint8_t get_Uart;
uint64_t nowtime1, nowtime2;
 uint8_t SelfTest_flag = 1;                  //�Լ��־ 1δ�Լ� 0�����
 
__IO uint32_t record_upcast_cnt = 0;
__IO uint32_t heart_beat_cnt = 0;



extern __IO uint8_t Recon_TimStart; //������ʱ������־
extern uint8_t  w5500_remote_ip[4];	//Զ�˷�����IP	
extern uint16_t w5500_remote_port;	//Զ�˷������˿�



/* ������ */
osThreadId_t ThreadIdStart = NULL;
osThreadId_t ThreadIdTaskLed = NULL;
osThreadId_t ThreadIdTaskProtection = NULL;
osThreadId_t ThreadIdTaskDebug = NULL;
osThreadId_t ThreadIdTaskEth = NULL;
osThreadId_t ThreadIdTaskAgeing = NULL;             /* �Լ��ϻ����� */
osThreadId_t ThreadIdTaskACDC_alarm = NULL;
osThreadId_t ThreadIdTaskProtocol = NULL;
osThreadId_t ThreadIdTaskWorkstep = NULL;

osSemaphoreId_t dbg_Semaphore = NULL;               /* debug�ź���ID */
osSemaphoreId_t acdc_alarm_Semaphore = NULL;        /* ACDC�澯�ź���ID */
osSemaphoreId_t IPGP_Alarm_Semaphore = NULL;       /* ȫ���ڲ��澯�ź���ID */
osSemaphoreId_t w5500_rcv_Semaphore = NULL;        /* ���ڽ����ź���ID */
osTimerId_t  timerID_Periodic_adc3 = NULL;          /* ADC3��ʱ��ID */
osMessageQueueId_t ACDC_UpdateQueue;                 /* ACDC������Ϣ���� */

/* ������������ */
const osThreadAttr_t ThreadStart_Attr =
{
    .name = "osRtxStartThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 4096,
};

const osThreadAttr_t ThreadLed_Attr =
{
    .name = "osRtxLedThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 2048,
};

const osThreadAttr_t ThreadProtection_Attr =
{
    .name = "osRtxProtectionThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 4096,
};

const osThreadAttr_t ThreadDebug_Attr =
{
    .name = "osRtxDebugThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 10240,
};

const osThreadAttr_t ThreadEth_Attr =
{
    .name = "osRtxEthThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 4096,
};

const osThreadAttr_t ThreadAgeing_Attr =
{
    .name = "osRtxAgeingThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal,
    .stack_size = 2048,
};

const osThreadAttr_t ThreadACDC_alarm_Attr =
{
    .name = "osRtxACDC_alarmThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 4096,
};

const osThreadAttr_t ThreadProtocol_Attr =
{
    .name = "osRtxProtocolThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 4096,
};

const osThreadAttr_t ThreadWorkstep_Attr =
{
    .name = "osRtxWorkstepThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal2,
    .stack_size = 10240,
};  

/* �ź������� */
const osSemaphoreAttr_t sema_New_Attr_debug =
{
    .name = "debug Semaphore",                                   /* �ź��������� */

};

/* �ź������� */
const osSemaphoreAttr_t sema_New_Attr_eth =
{
    .name = "Eth Semaphore",                                     /* �ź��������� */
};

/* �ź������� */
const osSemaphoreAttr_t sema_New_Attr_acdc_alarm =
{
    .name = "Acdc_alarm Semaphore",                              /* ACDC�澯�仯֪ͨ */
};
/* �ź������� */
const osSemaphoreAttr_t sema_New_Attr_IPGP_alarm =
{
    .name = "IPGP_alarm Semaphore",                              /* ȫ���ڲ��澯�仯֪ͨ */
};

/* �ź������� */
const osSemaphoreAttr_t sema_New_Attr_w5500_rcv =
{
    .name = "w5500_rcv Semaphore",                               /* W5500�����ź��� */
};

/* ��ʱ������ */
const osTimerAttr_t timer_Periodic_Attr =
{
    .name = "adc3 periodic timer",                  /* ��ʱ������ */
};

/* CAN1��Ϣ�������� */
const osMessageQueueAttr_t ACDC_UpdateQueue_Attr =
{
    .name = "ACDC_UpdateQueue",                 /* ��Ϣ���е����� */

};

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
void AppTaskStart(void *argument);
extern uint64_t bsp_CheckRunTime(uint64_t _LastTime);
/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : Product_test()
**  ����˵�� : Ӳ�����Դ���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void Product_test(void)
{
    /* MB85RS2���Դ��� */
#ifdef MB85RS2_test
    uint8_t date[1000] = {1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
						1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
						1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
						1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
						1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         1, 3, 5, 0, 0 ,9 ,6 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 5,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8,
                         9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8, 9, 3, 5, 0, 0 ,9 ,4 ,5 ,7, 8};
    uint8_t test[1000]={0};
    MB85Ctrl_CheckWrite(0, date, 1000);
    MB85Ctrl_Read(0,test,1000);

#endif

    /*����ʱ���������Դ��� */
#ifdef RUNTIME_test
    nowtime1 = bsp_GetRunTime();
    bsp_DelayMS(100);
    nowtime2 = bsp_GetRunTime();
    if(100 == nowtime2 - nowtime1)
    { printf("very precise\n"); }
    else
    { printf("Not precise\n"); }
#endif

    /* ���ACDCģ������ */
    uint8_t Modtype;
    Modtype = ACDC_Fan_Speed51_ModeTest(); //���ACDC���ͣ������÷��ȵ�51%
    if(Modtype)
    {
        ACDC.Type = Modtype;
        if((ACDC.Type==1)||(ACDC.Type==11)) //����
        { 
        ACDC_Fan_Speed_CMD(ACDC.Type, 0, 20); //ģ��0�������õ�20%
        ACDC_Fan_Speed_CMD(ACDC.Type, 1, 20); //ģ��2�������õ�20%
        }
        else if((ACDC.Type==2)||(ACDC.Type==22))//�յ�
        {
        ACDC_Fan_Speed_CMD(ACDC.Type, 1, 20); //ģ��1�������õ�20%
        ACDC_Fan_Speed_CMD(ACDC.Type, 2, 20); //ģ��2�������õ�20%         
        }
    }
    else
    {
        printf("ACDC Test ERR!\n"); //ACDCģ�������
    }

    ACDC.FanSpd[0] = 0x33;
    ACDC.FanSpd[1] = 0x33;

    /* easyflash��ʼ�� */
    uint8_t flash_en = 0;
    ACDC_Inf ACDC_read;
    //SPI_Flash_Erase_Chip();    //�������ȫ��flash��ʹ��easyflash֮ǰ�����Ƭ����һ��
    if(easyflash_init() == EF_NO_ERR)                                 //��ʼ��easyflash
    {
        ef_get_env_blob("flash_en", (void *) &flash_en, 1, NULL);       //��flash������ʹ��λ
        if(flash_en == 1)
        {
            if(EF_NO_ERR == ef_set_env_blob("ACDC_State", (void *) &ACDC, sizeof(ACDC_Inf)))
            {
                printf("creat sucessful");
            }
        }
    }
    ef_get_env_blob("ACDC_State", (void *) &ACDC_read, sizeof(ACDC_Inf), NULL);
}
/**********************************************************************************************************
**  �� �� �� : BarCode_Mac_CheckSet()
**  ����˵�� : ����Mac����������ú�������Ҫ����EE
**  ��    �� : ��
**  �� �� ֵ : 1��������һ���ԣ�ʹ��Ĭ��MAC��0������һ���ԣ�ʹ��EE�ڲ�MAC
**********************************************************************************************************/
uint8_t BarCode_Mac_CheckSet(void)
{
    uint8_t Read_En = 0;
    uint8_t Bar_Code[50] = {0};
    uint8_t Bar_Code_Len = 0;
    uint8_t Pyear, Pmonth; //��������
    uint16_t Pid; //��ˮ��
    uint16_t Pym; //���±���
    uint32_t Pmac = 0; //mac���(��3�ֽ���Ч)
    uint8_t err = 0;
    
    ee_ReadBytes(&Read_En, MacEn_flag_addr , 1);    //��EE
    if(Read_En == 1)
    {
        ee_ReadBytes(SN, MacID_flag_addr , 3);       //��MAC
        ee_ReadBytes(&Bar_Code_Len, BarCode_Len_flag_addr , 1);  //�����볤��
        if(Bar_Code_Len < 50)                       //�����ַ�����������50�ֽ�
        {
            ee_ReadBytes(Bar_Code, BarCode_End_flag_addr - Bar_Code_Len + 1, Bar_Code_Len);  //���ݳ��ȶ���������

            /*���������Ч��*/
            if((Bar_Code[0] == 'J') && (Bar_Code[1] == 'N'))            //�ж�����ͷ, JN��ͷ
            {
                for(uint8_t i = 0; i < 8; i++)
                {
                    if(Bar_Code[4 + i] > 0x39 || Bar_Code[4 + i] < 0x30) //�ж�Ҫ����λ(������ˮ)�Ƿ�Ϊ����ASC��
                    {
                        err = 1;
                        break;
                    }
                }
            }
            else
            {
                err = 1;
            }
            /*������MAC����*/
            if(err == 0)
            {
                /*����������ˮ*/
                Pyear = (Bar_Code[4] - 0x30) * 10 + (Bar_Code[5] - 0x30);
                Pmonth = (Bar_Code[6] - 0x30) * 10 + (Bar_Code[7] - 0x30);
                if((Pyear < 21) || (Pmonth > 12) || ((Pyear == 21) && (Pmonth < 10))) //�ٴ��ж�������Ч��Χ
                {
                    err = 1;
                }
                if(err == 0)
                {
                    Pid = (Bar_Code[8] - 0x30) * 1000 + (Bar_Code[9] - 0x30) * 100 + (Bar_Code[10] - 0x30) * 10 + (Bar_Code[11] - 0x30); //����ˮ��
                    Pym = (Pyear == 21) ? (Pmonth - 10) : ((Pyear - 22) * 12 + Pmonth + 2); //�����±��
                    Pmac = (SN[0] << 16) + (SN[1] << 8) + SN[2]; //��MAC���
                    if(Pmac != (Pym << 14) + Pid)
                    {
                        err = 1;
                    }
                }
            }
        }
        else
        {
            err = 1;
        }

        if(err)
        {
            SN[0] = 0;  //ʹ��Ĭ��MAC
            SN[1] = 0;
            SN[2] = 0;
           for(uint8_t j = 0; j < CHANNEL_CFG; j++)
           { IP_Alarm_Flag[j] |= IntelProtect_MAC; } //MAC�쳣,�澯��λ            
        }
    }
    return err;
}
/*
*********************************************************************************************************
*                                     OSTmr_Count_Start
* Description: Time Count start base on OS [note]Must use after OS start!!!!!
* Argument(s): Max count 0xFFFFFFFF
* Return(s)  :
* Note(s)    :
*********************************************************************************************************
*/
void    OSTmr_Count_Start(uint32_t  *pCounter)
{
    *pCounter   =   osKernelGetTickCount();
}


/*
*********************************************************************************************************
*                                     OSTmr_Count_Get
* Description: Time Count stop [note]Must use after OS start!!!!!
* Argument(s): none.
* Return(s)  : time * ms (os tick must be 1000HZ)
* Note(s)    :
*********************************************************************************************************
*/
uint32_t    OSTmr_Count_Get(uint32_t    *pCounter)
{
    uint32_t    tmptime = 0;
    tmptime =   osKernelGetTickCount();
    /* Overflow */
    if(tmptime < *pCounter)
    { return (0xffffffff - *pCounter + tmptime); }
    else
    { return (tmptime - *pCounter); }
}
/**********************************************************************************************************
**  �� �� �� : main()
**  ����˵�� : ��׼C�������
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
int main(void)
{
    /* ϵͳ��ʼ��(HAL�⡢MPU��Cache��Clk��) */
    System_Init();

//  User_Set_Vector();   /*�����ж�������ƫ�Ƶ�ַ*/

    /*���Ź���������λ����8�룬���Ա������ο��Ź�*/
//    bsp_InitSWDT();
    
    /* �ں˿���ǰ�ر�HAL��ʱ���׼ */
//    HAL_SuspendTick();

    /* �ں˳�ʼ�� */
    osKernelInitialize();

    /* ������������ */
    ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);

    /* ���������� */
    osKernelStart();

    while(1);
}


/**********************************************************************************************************
**  �� �� �� : CreateTask()
**  ����˵�� : ����Ӧ������
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
static void CreateTask(void)
{
    ThreadIdTaskLed = osThreadNew(AppTaskLed, NULL, &ThreadLed_Attr);
	ThreadIdTaskDebug = osThreadNew(AppTaskDebug, NULL, &ThreadDebug_Attr);
	ThreadIdTaskAgeing = osThreadNew(AppTaskAgeing, NULL, &ThreadAgeing_Attr);
	ThreadIdTaskWorkstep = osThreadNew(AppTaskWorkstep, NULL, &ThreadWorkstep_Attr);
	ThreadIdTaskACDC_alarm = osThreadNew(AppACDC_alarm, NULL, &ThreadACDC_alarm_Attr);
	ThreadIdTaskProtection = osThreadNew(AppTaskProtection, NULL, &ThreadProtection_Attr);
	
}

extern void ExternalAdDatProcess(void);
extern void EepromDatInit(void);
extern uint8_t workstep_flag_AbortPrio[8];

uint8_t flag_of_intelprot = 0;
uint8_t contact_test_once_flag[8] = {0};
/**********************************************************************************************************
**  �� �� �� : AppTaskStart()
**  ����˵�� : ��������
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/

void AppTaskStart(void *argument)
{
	uint8_t Read_En = 0;
    ee_ReadBytes(&Read_En, Remote_En_flag_addr , 1);    //��EE
    if(Read_En == 1)
    {
        ee_ReadBytes(w5500_remote_ip, Remote_IPPort_flag_addr , 4);
        ee_ReadBytes((uint8_t *)&w5500_remote_port, Remote_IPPort_flag_addr + 4 , 2);
    }
	
    IP_Scanning(); 

	if(((W5500_IP_ADDRESS[2] == 8) && ((W5500_IP_ADDRESS[3] == 42) || (W5500_IP_ADDRESS[3] == 41))) || (g_ucSysMode != SYS_NORMAL_MODE))
	{
		SysModeProtMaskFlag = 1;
	}
	else
	{
		SysModeProtMaskFlag = 0;
	}
	
    features = RNG_Get_RandomRange(1, 254); //���������������ʼֵ
    /*�����������MAC*/
    BarCode_Mac_CheckSet();  //����MACһ���Լ�鲢����MAC��3�ֽ�
	
     /* ��������Э������ͬ���ź��� */
    w5500_rcv_Semaphore = osSemaphoreNew(1U, 0U, &sema_New_Attr_w5500_rcv);

    /* �������ں�Э������ */
    ThreadIdTaskEth = osThreadNew(AppTaskEth, NULL, &ThreadEth_Attr);
    ThreadIdTaskProtocol = osThreadNew(AppTaskProtocol, NULL, &ThreadProtocol_Attr);
    
	/* �����Լ� */
	if(SelfTest_flag)
	{
		bsp_LedOn(LED_82_NO);
		for(uint8_t m=0;m<10;m++)
		{  
			bsp_DelayMS(500);
			bsp_LedToggle(LED_82_NO); 
			bsp_LedToggle(LED_83_NO); 
			Feed_WDG();
		}
		bsp_LedOff(LED_82_NO);
		bsp_LedOff(LED_83_NO);
		
		bsp_DelayMS(50);
		IntelProtectDeviceCheck();      //����Ӳ����⣬�����ڲ�����������־

		ACDC.Type = ACDC_Fan_Speed51_ModeTest();
		SelfTest_flag = 0;
	}
	
	/* �Լ��־λ��ֵ */
	if((IP_Alarm_Flag[0] & IntelProtect_FLE) || (IP_Alarm_Flag[0] & IntelProtect_EEE) || \
       (IP_Alarm_Flag[0] & IntelProtect_FRE) || (IP_Alarm_Flag[0] & IntelProtect_C1E) || \
       (IP_Alarm_Flag[0] & IntelProtect_C2E))
	{
		flag_of_intelprot = 1;
	}	
	
//	if(SelfTest_flag == 0)
//	{
//		TimerAUintStart(TMRA1_UNIT); //�����ڲ���ȫ�ֱ�����ⶨʱ��A��Ԫ1������40MS
//	}
    /*  ϵͳ������ʼ��   */
    sys_cfg_set();
	
	Record_Msg_Init();
	
	SystemStatus.Neilsbed = Neilsbed_Nofit;	

	uint8_t read_flag = 0;
	
	MB85Ctrl_Read(8192, &read_flag, 1);
	
	MB85Ctrl_Read(8208, power_off_continue, 8);
	
	if(read_flag == 0x01)
	{
		uint32_t read_addr = 4096;
		
		for(int j = 0; j < CHANNEL_CFG; j++)
		{
			if(power_off_continue[j] == 1)
			{
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstep[j]       , sizeof(Tier2.workstep[j]      ));
				read_addr += sizeof(Tier2.workstep[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepCache1[j] , sizeof(Tier2.workstepCache1[j]));
				read_addr += sizeof(Tier2.workstepCache1[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepCache2[j] , sizeof(Tier2.workstepCache2[j]));
				read_addr += sizeof(Tier2.workstepCache2[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepAnalyze[j], sizeof(Tier2.workstepAnalyze[j]));
				read_addr += sizeof(Tier2.workstepAnalyze[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepRunning[j], sizeof(Tier2.workstepRunning[j]));
				read_addr += sizeof(Tier2.workstepRunning[j]);
				MB85Ctrl_Read(read_addr, &workstep_flag_AbortPrio[j]         , sizeof(workstep_flag_AbortPrio[j]));
				read_addr += sizeof(workstep_flag_AbortPrio[j]);
				
				if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT)
				{
					ContactCheckFlag[j] = 1;
					contact_test_once_flag[j] = 1;
				}				
				Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_PAUSE;
				
			}
		}
	}
	else if(read_flag == 0x02)
	{
		uint32_t read_addr = 0;
		
		for(int j = 0; j < CHANNEL_CFG; j++)
		{
			if(power_off_continue[j] == 1)
			{
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstep[j]       , sizeof(Tier2.workstep[j]      ));
				read_addr += sizeof(Tier2.workstep[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepCache1[j] , sizeof(Tier2.workstepCache1[j]));
				read_addr += sizeof(Tier2.workstepCache1[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepCache2[j] , sizeof(Tier2.workstepCache2[j]));
				read_addr += sizeof(Tier2.workstepCache2[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepAnalyze[j], sizeof(Tier2.workstepAnalyze[j]));
				read_addr += sizeof(Tier2.workstepAnalyze[j]);
				MB85Ctrl_Read(read_addr, (uint8_t *)&Tier2.workstepRunning[j], sizeof(Tier2.workstepRunning[j]));
				read_addr += sizeof(Tier2.workstepRunning[j]);
				MB85Ctrl_Read(read_addr, &workstep_flag_AbortPrio[j]         , sizeof(workstep_flag_AbortPrio[j]));
				read_addr += sizeof(workstep_flag_AbortPrio[j]);

				if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT)
				{
					ContactCheckFlag[j] = 1;
					contact_test_once_flag[j] = 1;
				}				
				Tier2.workstepRunning[j].RunSta = WORKSTEP_RUNSTA_REF_PAUSE;

			}
		}	
	}
	
	/* eeprom���ݳ�ʼ�� */
	EepromDatInit();	
	
	/* �����ź��� */
    dbg_Semaphore = osSemaphoreNew(1U, 0U,   &sema_New_Attr_debug);

    /* �����ź��� */
    acdc_alarm_Semaphore = osSemaphoreNew(1U, 0U,   &sema_New_Attr_acdc_alarm);
    
    /* �����ź��� */
    IPGP_Alarm_Semaphore = osSemaphoreNew(1U, 0U,   &sema_New_Attr_IPGP_alarm);

    /*������Ϣ����*/
    ACDC_UpdateQueue = osMessageQueueNew(MQ_SIZE, sizeof(ACDC_UpdateStrcut), &ACDC_UpdateQueue_Attr);      /* ��Ϣ����������Ϣ������  ��Ϣ�Ĵ�С����λ: Byte�� ��Ϣ���е����� */
		
    /* �������� */
    CreateTask();
	
	if(SelfTest_flag == 0)
	{
		TimerAUintStart(TMRA1_UNIT); //�����ڲ���ȫ�ֱ�����ⶨʱ��A��Ԫ1������40MS
	}
    
    while(1)
    {
		
		if(((W5500_IP_ADDRESS[2] == 8) && ((W5500_IP_ADDRESS[3] == 42) || (W5500_IP_ADDRESS[3] == 41))) || (g_ucSysMode != SYS_NORMAL_MODE))
		{
			SysModeProtMaskFlag = 1;
		}
		else
		{
			SysModeProtMaskFlag = 0;
		}		
		
        /* �����Դ������� */
		if((record_upcast_cnt >(data_upcast_period * 1000))&&(Recon_TimStart==1)&&(ACDC_UpdateFlg==0))
		{
			printf("Start task is running!");
			
			record_upcast_cnt = 1;
			
			for(int i = 0; i < CHANNEL_CFG; i++)
			{
				if((Tier2.record[i].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.record[i].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE) || (protect_flag_upcast[i] == 1))
				{
	//					RecordDatUpcast(i, 1);
				}
				else
				{
	//					RecordDatUpcast(i, 0);
				}
				osDelay(1); 
			}
			
			for(int i = 0; i < CHANNEL_CFG; i++)
			{
				if(workstep_flag_CriticalZone[i] == 1)
				{
					workstep_flag_CriticalZone[i] = 2;
				}
				else if(workstep_flag_CriticalZone[i] == 2)
				{
					workstep_flag_CriticalZone[i] = 3;
				}
			}
		}
		
		if(heart_beat_flag == 1)
		{
			if(heart_beat_cnt > 8000)	/* ��λ������8sһ�Σ�����3��δ��⵽�������ж���λ������λ���Ͽ����� */
			{
                heart_beat_cnt=0;
				heart_beat_flag = 2;

				for(int i = 0; i < CHANNEL_CFG; i++)
				{
					if( (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_STOP) && \
						(Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_COMPLETE) )
					{
	//					RecordDatUpcast(i, 1);
						record_data_upcast_flag[i] = 1;
						osDelay(1);						
						BatteryFormattingStop(i);
						Tier2.workstepRunning[i].RunSta = WORKSTEP_RUNSTA_REF_PAUSE;
							
					}
				}
				
			}		
		}
		else if((heart_beat_flag == 0)&&(Recon_TimStart==1))
		{
			heart_beat_flag = 1;
			heart_beat_cnt = 0;
		}		
			
        if(g_ucSysMode != SYS_NORMAL_MODE)  
        {
			Protect_Trigger_Judgemet();             //������ģʽר�ñ�������
		}
        osDelay(1);
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/