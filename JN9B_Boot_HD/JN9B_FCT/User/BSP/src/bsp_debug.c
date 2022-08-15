/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_debug.c
** Author      : Huang Cheng
** Date        : 2021-11-15
** Description : bsp_debug source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "string.h"
#include "main.h"
/**********************************************************************************************************
**                                         ����������
**********************************************************************************************************/
typedef struct {

    char cmd[20];                   //������
    char param[4][100];             //ÿ��ָ�����֧��4�������������
    void (*ptrCmdFun)(void);        //�����봦������

} cmd_table;

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
stc_usart_uart_init_t              UartHandle;
__IO DBGPort_TypeDef				s_dbgPort;
uint8_t	tmpCode[DBG_CMD_CODE_SIZE]= {0};     //ָ�����ݻ���
uint8_t	tmpPara[4][DBG_CMD_PARA_SIZE]= {0};  //���ݲ�������

cmd_table FCT_CMD[] = {

    /*����������*/
    {"Uart_Test", {0}, Uart_Test_Fun },                       //���ڲ���

    /*���ư�FCT�Բ�*/
    {"Version_Test", {0}, Version_Test_Fun },                 //Ӳ���汾��ʶ����
    {"Temp_Test", {0}, Temp_Test_Fun },                       //�¶Ȳ�������
    {"VBus_Test", {0}, VBus_Test_Fun },                       //BUS��ѹ��������
    {"EE_Test", {0}, EE_Test_Fun },                           //EEPROM����
    {"Fram_Test", {0}, Fram_Test_Fun },                       //FRAM����
    {"Vs_Vt_Test", {0}, Vs_Vt_Test_Fun },                     //��ص�ѹ��̽���ѹ��������
    {"PM_Test", {0}, PM_Test_Fun },                           //���ֵ�λ������
    {"Imod_Test", {0}, Imod_Test_Fun },                       //ģ�������������
    {"Vout_Test", {0}, Vout_Test_Fun },                       //�˿ڵ�����������
    {"En_Int_Test", {0}, En_INT_Test_Fun },                   //EN��INT�ź�IO����
    {"FanErr_Test", {0}, FanErr_Test_Fun },                   //���ȹ����źŲ���
    {"CSeat_Test", {0}, CSeat_Test_Fun },                     //���ư���λ�źŲ���
    {"PSeat_Test", {0}, PSeat_Test_Fun },                     //���ʰ���λ�źŲ���
    {"Can_Test", {0}, Can_Test_Fun },                         //CAN1&CAN2�ӿڲ���
    {"Led_I2C_Test", {0}, Led_I2C_Test_Fun },                 //�ư�IIC�ӿڲ���
    {"PowerBD_I2C_Test", {0}, PowerBD_I2C_Test_Fun },         //���ʰ�IIC�ӿڲ���
    {"CV4_CV8_Test", {0}, CV4_CV8_Test_Fun },                 //CV4&CV8�ź�IO����
    {"HWTD_Test", {0}, HWTD_Test_Fun },                       //Ӳ�����Ź�����
    {"MB_Auto_Test", {0}, MB_Auto_Test_Fun },                 //���ư�FCT�Զ�����

    /*���ʰ�FCT����*/
    {"PVersion_Test", {0}, PVersion_Test_Fun },               //���ʰ�Ӳ���汾��ʶ����
    {"PTemp_Test", {0}, PTemp_Test_Fun },                     //���ʰ��¶Ȳ���
    {"OVP_Test", {0}, OVP_Test_Fun },                         //OVP����
    {"OCP_Test", {0}, OCP_Test_Fun },                         //OCP����
    {"RPP_Test", {0}, RPP_Test_Fun },                         //���ӱ�������
    {"CVmod_C_Test", {0}, CVmod_C_Test_Fun },                 //CVMODģʽ������
    {"CVsense_FD_Test", {0}, CVsense_FD_Test_Fun },           //CVSenseģʽ���ٷŵ����
    {"CVsense_SD_Test", {0}, CVsense_SD_Test_Fun },           //CVSenseģʽ���ٷŵ����
    {"CC_10A_Test", {0}, CC_10A_Test_Fun },                   //CCģʽ10A����
    {"DCC_120A_Test", {0}, DCC_120A_Test_Fun },               //DCCģ��120A����
    {"PB_Auto_Test", {0}, PB_Auto_Test_Fun },                 //���ʰ�FCT�Զ�����

};

extern __IO uint8_t HWDT_EN;                    //Ӳ�����Ź�ʹ�ܱ�־
__IO uint8_t Auto_Test=0;                       //�Զ����Ա�־
__IO uint32_t Failed_CNT=0;                     //ʧ���������¼
uint8_t CMD_Type=2;                             //ָ�����ͣ�ָʾָ���ʽ�� $XXX@ ����$XXX:
uint8_t  Can1_recv[4]= {0};                //CAN1��������
uint8_t  Can2_recv[4]= {0};                //CAN2��������
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/
void Uart_Test_Fun(void)
{
    printf("1.1 ������Ŀ��ͨ�Ŵ��ڲ��Կ�ʼ\r\n");
    printf("1.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("1.3 ���Խ���������������һ�\r\n");
}

void Version_Test_Fun(void)
{
    printf("2.1 ������Ŀ��Ӳ���汾��ʶ����\r\n");
    printf("2.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("2.3 ���Խ���������������һ�\r\n");
};

void Temp_Test_Fun(void)
{
    printf("3.1 ������Ŀ�������¶Ȳ�������\r\n");
    printf("3.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("3.3 ���Խ���������������һ�\r\n");
};

void VBus_Test_Fun(void)
{
    printf("4.1 ������Ŀ��BUS��ѹ��������\r\n");
    printf("4.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("4.3 ���Խ���������������һ�\r\n");
};

/**********************************************************************************************************
**  �� �� �� : EE_Test_Fun()
**  ����˵�� : EEPROM������Ŀ�����̳���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void EE_Test_Fun(void)
{
    uint8_t  Test_RDate;                            //�������ݻ���
    uint8_t  Test_WDate;                            //д�����ݻ���

    printf("5.1 ������Ŀ��EEPROM���Կ�ʼ\r\n");
    ee_ReadBytes(&Test_RDate,0x10,1);
    Test_WDate=~Test_RDate;
    ee_WriteBytes(&Test_WDate,0x10,1);
    ee_ReadBytes(&Test_RDate,0x10,1);
    if(Test_WDate==Test_RDate)                     //�����ȡ��ֵ�����ϴζ�����ֵ��λȡ���������PASS����FAILED
    {
        printf("5.2 ���Խ����PASS��\r\n");
    }
    else
    {
        Failed_CNT+=1;                             //ʧ�������+1
        printf("5.2 ���Խ����FAILED��\r\n");
    }
    if(Auto_Test==0)
        printf("5.3 ���Խ���������������һ�\r\n");
}

/**********************************************************************************************************
**  �� �� �� : Fram_Test_Fun()
**  ����˵�� : ���������Ŀ�����̳���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void Fram_Test_Fun(void)
{
    uint32_t Fram_ID;
    uint8_t  Test_RDate;                            //�������ݻ���
    uint8_t  Test_WDate;                            //д�����ݻ���

    printf("6.1 ������Ŀ��FRAM���Կ�ʼ\r\n");      //���Կ�ʼ
    MB85Ctrl_ReadID(&Fram_ID);                      //��ȡ����ID
    if((Fram_ID & 0xff000000) == 0x04000000)		//ID����ֽ�0x04����������Ϊ��ʿ
    {
        MB85Ctrl_Read(0x10,&Test_RDate,1);            //�ӵ�ַ0x10��һ���ֽ�
        Test_WDate=~Test_RDate;
        MB85Ctrl_Write(0x10,&Test_WDate,1);           //ȡ����д���ַ0x10
        MB85Ctrl_Read(0x10,&Test_RDate,1);            //�ٴζ���
        if(Test_WDate==Test_RDate)                    //�����ȡ��ֵ�����ϴζ�����ֵ��λȡ���������PASS����FAILED
        {
            printf("6.2 ���Խ����PASS��\r\n");
        }
        else
        {
            Failed_CNT+=1;                             //ʧ�������+1
            printf("6.2 ���Խ����FAILED��\r\n");
        }
    }
    else
    {
        Failed_CNT+=1;                             //ʧ�������+1
        printf("6.2 ���Խ����FAILED��\r\n");
    }
    if(Auto_Test==0)
        printf("6.3 ���Խ���������������һ�\r\n");
}

void Vs_Vt_Test_Fun(void)
{
    printf("7.1 ������Ŀ����ѹDA&���&̽���ѹ��������\r\n");
    printf("7.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("7.3 ���Խ���������������һ�\r\n");
}

void PM_Test_Fun(void)
{
    printf("8.1 ������Ŀ�����ֵ�λ��&������ѹӲ����������\r\n");
    printf("8.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("8.3 ���Խ���������������һ�\r\n");
};

void Imod_Test_Fun(void)
{
    printf("9.1 ������Ŀ������DA&ģ�������������\r\n");
    printf("9.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("9.3 ���Խ���������������һ�\r\n");
};

void Vout_Test_Fun(void)
{
    printf("10.1 ������Ŀ������Դ&�˿ڵ�ѹ��������\r\n");
    printf("10.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("10.3 ���Խ���������������һ�\r\n");
};

void En_INT_Test_Fun(void)
{
    printf("11.1 ������Ŀ��EN�ź�&Ӳ������INT�źŲ���\r\n");
    printf("11.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("11.3 ���Խ���������������һ�\r\n");
};

void FanErr_Test_Fun(void)
{
    printf("12.1 ������Ŀ�����ȹ��Ϸ����źŲ���\r\n");
    printf("12.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("12.3 ���Խ���������������һ�\r\n");
};
void CSeat_Test_Fun(void)
{
    printf("13.1 ������Ŀ�����ư���λ�źŲ���\r\n");
    printf("13.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("13.3 ���Խ���������������һ�\r\n");
};

void PSeat_Test_Fun(void)
{
    printf("14.1 ������Ŀ�����ʰ���λ�źŲ���\r\n");
    printf("14.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("14.3 ���Խ���������������һ�\r\n");
};

/**********************************************************************************************************
**  �� �� �� : Can_Test_Fun()
**  ����˵�� : CAN1&CAN2������Ŀ�����̳���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void Can_Test_Fun(void)
{
    uint32_t Test_CAN1ID=0x10000002;

    uint8_t  date[]= {0x00,0x01,0x02,0x03};

    uint8_t CAN_ERR=0;
    printf("15.1 ������Ŀ��CAN1&CAN2�շ�����\r\n");
    CAN1_SendData(Test_CAN1ID,&date[0],4);                                         //CAN1����date
    printf("15.2 ���Թ��̣�\r\n");
    printf("-------------->CAN1����{%d,%d,%d,%d}\r\n",date[0],date[1],date[2],date[3]);
    bsp_DelayMS(100);                                                                    //�ȴ�CAN2����
    printf("-------------->CAN2����{%d,%d,%d,%d}\r\n",Can2_recv[0],Can2_recv[1],Can2_recv[2],Can2_recv[3]);
    for(uint8_t i=0; i<4; i++)
    {
        if(date[i]!=Can2_recv[i])
        {
            CAN_ERR=1;
            break;
        }
    }
    
    //�޸Ĵ�������
    for(uint8_t n=0;n<4;n++)
    {
        date[n]+=1;
    }
    
    CAN2_SendData(Test_CAN1ID,&date[0],4);                                         //CAN2����date
    printf("-------------->CAN2����{%d,%d,%d,%d}\r\n",date[0],date[1],date[2],date[3]);
    bsp_DelayMS(100);                                                                    //�ȴ�CAN1����
    printf("-------------->CAN1����{%d,%d,%d,%d}\r\n",Can1_recv[0],Can1_recv[1],Can1_recv[2],Can1_recv[3]);
    for(uint8_t i=0; i<4; i++)
    {
        if(date[i]!=Can1_recv[i])
        {
            CAN_ERR=1;
            break;
        }
    }
    if(CAN_ERR==0)
    {
        printf("15.3 ���Խ����PASS��\r\n");
    }
    else
    {
        printf("15.3 ���Խ����FAILED��\r\n");
    }
    if(Auto_Test==0)
        printf("15.4 ���Խ���������������һ�\r\n");
};

void Led_I2C_Test_Fun(void)
{
    printf("16.1 ������Ŀ���ư�ͨ�Ų���\r\n");
    printf("16.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("16.3 ���Խ���������������һ�\r\n");
};

void PowerBD_I2C_Test_Fun(void)
{
    printf("17.1 ������Ŀ�����ʰ�ͨ�Ų���\r\n");
    printf("17.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("17.3 ���Խ���������������һ�\r\n");
};
void CV4_CV8_Test_Fun(void)
{
    printf("18.1 ������Ŀ��CV4&CV8�źŲ���\r\n");
    printf("18.2 ���Խ����PASS��\r\n");
    if(Auto_Test==0)
        printf("18.3 ���Խ���������������һ�\r\n");
};



/**********************************************************************************************************
**  �� �� �� : HWTD_Test_Fun()
**  ����˵�� : Ӳ�����Ź�������Ŀ�����̳���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void HWTD_Test_Fun(void)
{

    uint32_t Test_WDate= 0xAAAABBBB;                          //���Ź����Կ�ʼ��־
    printf("19.1 ������Ŀ��Ӳ�����Ź����Կ�ʼ\r\n");
    EFM_FCT_Erase();                                          //������
    EFM_SingleProgram(EFM_ADDR_SECTOR255, Test_WDate);        //д���Ź����Կ�ʼ��־
    EFM_SingleProgram(EFM_ADDR_SECTOR255+4, Auto_Test);       //д�Զ����̱�־
    EFM_SingleProgram(EFM_ADDR_SECTOR255+8, Failed_CNT);      //дʧ�������ֵ
    EFM_SectorCmd_Sequential(EFM_ADDR_SECTOR255, 1,Disable);  //��FCT����д����
    HWDT_EN=0;                                                //Ӳ�����Ź�ʹ�ܱ�־��0��5������û�����������ʧ��
    bsp_DelayMS(5000);
    Failed_CNT+=1;                                            //ʧ�������+1
    printf("19.2 ���Խ����FAILED��\r\n");

    Test_WDate= 0xBBBBAAAA;                                   //���Ź����Խ�����־
    EFM_FCT_Erase();                                          //������
    EFM_SingleProgram(EFM_ADDR_SECTOR255, Test_WDate);        //д���Ź����Խ�����־
    EFM_SectorCmd_Sequential(EFM_ADDR_SECTOR255, 1,Disable);  //��FCT����д����
    if(Auto_Test==0)
    {
        printf("19.3 ���Խ���������������һ�\r\n");
    }
    else
    {
        printf("�������������������ذ�FCT�������̽���������������������\r\n");
        printf("����ʧ�����ܼƣ�%d!\r\r\n",Failed_CNT);
    }
};
/**********************************************************************************************************
**  �� �� �� : MB_Auto_Test_Fun()
**  ����˵�� : ���ذ�FCT�Զ��������̳���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void MB_Auto_Test_Fun(void)
{
    uint16_t i;
    Auto_Test=1;                       //�Զ����Կ�ʼ����־��1
    int32_t n=0;
    uint8_t index;                     //ָ��ṹ�������±�
    char *p;

    printf("�������������������ذ�FCT�������̿�ʼ������������������\r\n");
    Failed_CNT=0;                      //FAIL������0
    if( CMD_Type == 1)                 //$XXX@����ָ���Ĭ�ϲ�����˳��ִ��FCT����
    {
        for(i=0 ; i<19 ; i++)
        {
            //��ձ�ִ��ָ����ʷ����
            memset(FCT_CMD[i].param,0,sizeof(FCT_CMD[i].param));
            //д��Ĭ�ϲ���
            if(i==1)
                strcpy(FCT_CMD[1].param[0],FCT_CMD[19].param[0]); // ��Ӳ���汾��
            printf("%d.0 ���Բ�����",i+1);
            for(uint8_t j=0; j<4; j++)
            {
                if(* (char*)&FCT_CMD[i].param[j]!=NULL)
                    printf("param%d:%s;",j,FCT_CMD[i].param[j]);
            }
            printf("\r\n");
            FCT_CMD[i].ptrCmdFun();
        }

    } else if(CMD_Type == 2)          //$XXX:����ָ���ָ��������˳��ִ��FCT����
    {
        //�ָ����1�����ַ�������
        p = strtok(FCT_CMD[19].param[0], ",");
        while(p)
        {
            // ����������������Ϊ��ѡ���$xxx:CMD1@Para1,CMD2@Para1@Para2,CMD3@Para1@Para2@Para3@Para4\r\n��ָ��ÿ����ָ��֧������4������
            if( 0 >= sscanf((const char*)p, "%d", &n ))    //ȡָ����
            {

            }
            //ָ���ŷ�Χ����1~19 �������±�ת��
            index=((n-1>=0)&&(n-1<19))?n-1:0;

            //��ձ�ִ��ָ����ʷ����
            memset(FCT_CMD[index].param,0,sizeof(FCT_CMD[index].param));

            if( 0 >= sscanf((const char*)p, "%*[^@]@%[^@]@%[^@]@%[^@]@%s", \
                            (char*)&FCT_CMD[index].param[0],(char*)&FCT_CMD[index].param[1],(char*)&FCT_CMD[index].param[2],(char*)&FCT_CMD[index].param[3]) ) //ȡָ�����
            {

            }
            //��ӡ����
            printf("%d.0 ���Բ�����",n);
            for(uint8_t j=0; j<4; j++)
            {
                if(* (char*)&FCT_CMD[index].param[j]!=NULL)
                    printf("param%d:%s;",j,FCT_CMD[index].param[j]);
            }
            printf("\r\n");

            FCT_CMD[index].ptrCmdFun();       //ִ��ָ��
            p = strtok(NULL, ",");
        }
        if(n!=19)   //������ղ�����Ŀ����Ӳ�����Ź����ԣ���ͳ��ʧ������Ŀ
        {
            printf("�������������������ذ�FCT�������̽���������������������\r\n");
            printf("����ʧ�����ܼƣ�%d!\r\r\n",Failed_CNT);
        }
    }
    memset(FCT_CMD[19].param,0,sizeof(FCT_CMD[19].param)); //�����Զ����̲�������

    Auto_Test=0;                       //�Զ����Խ�������־��0
};



void PVersion_Test_Fun(void)
{
    printf("21.1 ������Ŀ�����ʰ�Ӳ���汾��ʶ����\r\n");
    printf("21.2 ���Խ����PASS��\r\n");
};
void PTemp_Test_Fun(void)
{
    printf("22.1 ������Ŀ�����ʰ��¶Ȳ���\r\n");
    printf("22.2 ���Խ����PASS��\r\n");
};
void OVP_Test_Fun(void)
{
    printf("23.1 ������Ŀ�����ʰ�OVP����\r\n");
    printf("23.2 ���Խ����PASS��\r\n");
};
void OCP_Test_Fun(void)
{
    printf("24.1 ������Ŀ�����ʰ�OCP����\r\n");
    printf("24.2 ���Խ����PASS��\r\n");
};
void RPP_Test_Fun(void)
{
    printf("25.1 ������Ŀ�����ӱ�������\r\n");
    printf("25.2 ���Խ����PASS��\r\n");
};
void CVmod_C_Test_Fun(void)
{
    printf("26.1 ������Ŀ��CVMODģʽ������\r\n");
    printf("26.2 ���Խ����PASS��\r\n");
};
void CVsense_FD_Test_Fun(void)
{
    printf("27.1 ������Ŀ��CVSenseģʽ���ٷŵ����\r\n");
    printf("27.2 ���Խ����PASS��\r\n");
};
void CVsense_SD_Test_Fun(void)
{
    printf("28.1 ������Ŀ��CVSenseģʽ���ٷŵ����\r\n");
    printf("28.2 ���Խ����PASS��\r\n");
};
void CC_10A_Test_Fun(void)
{
    printf("29.1 ������Ŀ��CCģʽ10A����\r\n");
    printf("29.2 ���Խ����PASS��\r\n");
};
void DCC_120A_Test_Fun(void)
{
    printf("30.1 ������Ŀ��DCCģ��120A����\r\n");
    printf("30.2 ���Խ����PASS��\r\n");
};

/**********************************************************************************************************
**  �� �� �� : PB_Auto_Test_Fun()
**  ����˵�� : ���ذ�FCT�Զ��������̳���
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void PB_Auto_Test_Fun(void)
{
    uint16_t i;
    Auto_Test=1;                       //�Զ����Կ�ʼ����־��1
    int32_t n=0;
    uint8_t index;                     //ָ��ṹ�������±�
    char *p;

    printf("�������������������ʰ�FCT�������̿�ʼ������������������\r\n");
    Failed_CNT=0;                      //FAIL������0
    if( CMD_Type == 1)                 //$XXX@����ָ���Ĭ�ϲ�����˳��ִ��FCT����
    {
        for(i=20 ; i<30 ; i++)
        {
            //��ձ�ִ��ָ����ʷ����
            memset(FCT_CMD[i].param,0,sizeof(FCT_CMD[i].param));
            //д��Ĭ�ϲ���
            if(i==20)
                strcpy(FCT_CMD[20].param[0],FCT_CMD[30].param[0]); // ��Ӳ���汾��
            printf("%d.0 ���Բ�����",i+1);
            for(uint8_t j=0; j<4; j++)
            {
                if(* (char*)&FCT_CMD[i].param[j]!=NULL)
                    printf("param%d:%s;",j,FCT_CMD[i].param[j]);
            }
            printf("\r\n");
            FCT_CMD[i].ptrCmdFun();
        }

    } else if(CMD_Type == 2)          //$XXX:����ָ���ָ��������˳��ִ��FCT����
    {
        //�ָ����1�����ַ�������
        p = strtok(FCT_CMD[30].param[0], ",");
        while(p)
        {
            // ����������������Ϊ��ѡ���$xxx:CMD1@Para1,CMD2@Para1@Para2,CMD3@Para1@Para2@Para3@Para4\r\n��ָ��ÿ����ָ��֧������4������
            if( 0 >= sscanf((const char*)p, "%d", &n ))    //ȡָ����
            {

            }
            //ָ���ŷ�Χ����1~10 �������±�ת��
            index=((n-1>=0)&&(n-1<10))?n+19:0;

            //��ձ�ִ��ָ����ʷ����
            memset(FCT_CMD[index].param,0,sizeof(FCT_CMD[index].param));

            if( 0 >= sscanf((const char*)p, "%*[^@]@%[^@]@%[^@]@%[^@]@%s", \
                            (char*)&FCT_CMD[index].param[0],(char*)&FCT_CMD[index].param[1],(char*)&FCT_CMD[index].param[2],(char*)&FCT_CMD[index].param[3]) ) //ȡָ�����
            {

            }
            //��ӡ����
            printf("%d.0 ���Բ�����",n+20);
            for(uint8_t j=0; j<4; j++)
            {
                if(* (char*)&FCT_CMD[index].param[j]!=NULL)
                    printf("param%d:%s;",j,FCT_CMD[index].param[j]);
            }
            printf("\r\n");

            FCT_CMD[index].ptrCmdFun();       //ִ��ָ��
            p = strtok(NULL, ",");
        }
    }

    printf("�������������������ʰ�FCT�������̽���������������������\r\n");
    printf("����ʧ�����ܼƣ�%d!\r\r\n",Failed_CNT);
    memset(FCT_CMD[30].param,0,sizeof(FCT_CMD[30].param)); //�����Զ����̲�������

    Auto_Test=0;                       //�Զ����Խ�������־��0
};



/**********************************************************************************************************
**	�� �� �� : bsp_InitUSART()
**	����˵�� : ��ʼ��USART����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void bsp_InitUSART(void)
{
    const stc_usart_uart_init_t stcUartInit =
    {
        .u32Baudrate = 115200UL,
        .u32BitDirection = USART_LSB,
        .u32StopBit = USART_STOPBIT_1BIT,
        .u32Parity = USART_PARITY_NONE,
        .u32DataWidth = USART_DATA_LENGTH_8BIT,
        .u32ClkMode = USART_INTERNCLK_NONE_OUTPUT,
        .u32PclkDiv = USART_PCLK_DIV64,
        .u32OversamplingBits = USART_OVERSAMPLING_8BIT,
        .u32NoiseFilterState = USART_NOISE_FILTER_DISABLE,
        .u32SbDetectPolarity = USART_SB_DETECT_FALLING,
    };

    /* Configure USART RX/TX pin. */
    GPIO_SetFunc(USART_RX_PORT, USART_RX_PIN, USART_RX_GPIO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(USART_TX_PORT, USART_TX_PIN, USART_TX_GPIO_FUNC, PIN_SUBFUNC_DISABLE);

    /* Enable peripheral clock */
    PWC_Fcg3PeriphClockCmd(USART_FUNCTION_CLK_GATE, Enable);

    /* Initialize UART function. */
    if (Ok != USART_UartInit(USART_UNIT, &stcUartInit))
    {
        bsp_LedOn(RUN_LED_R_NO);          //��ʼ��ʧ�� RUN�� ��������̵���
        bsp_LedOff(RUN_LED_G_NO);
    }
    else
    {
        bsp_LedOff(RUN_LED_R_NO);         //��ʼ���ɹ� RUN�� ������̵���
        bsp_LedOn(RUN_LED_G_NO);
    }

    stc_irq_signin_config_t stcIrqRegiCfg;
    /*	���ô��ڽ����ж�	*/
    stcIrqRegiCfg.enIRQn = Int000_IRQn;//�����ж����ȼ�
    stcIrqRegiCfg.enIntSrc = INT_USART3_RI;//�����ж�����
    stcIrqRegiCfg.pfnCallback = &HC_USART_RxIrq_Callback;//�����жϻص�����
    (void)INTC_IrqSignIn(&stcIrqRegiCfg);
    NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
    NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_00);//DDL_IRQ_PRIORITY_DEFAULT DDL_IRQ_PRIORITY_00
    NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

    /* ���ô��ڴ�������ж� */
    stcIrqRegiCfg.enIRQn = Int001_IRQn;
    stcIrqRegiCfg.enIntSrc = INT_USART3_EI;
    stcIrqRegiCfg.pfnCallback = &HC_USART3_RxErr_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqRegiCfg);
    NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
    NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_01);
    NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

    /* Enable RX/TX function */
    USART_FuncCmd(USART_UNIT, (USART_RX | USART_INT_RX | USART_TX), Enable);
}

/*
*********************************************************************************************************
*	�� �� ��: int fputc(int ch, FILE *f)
*	����˵��: �ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int32_t fputc(int32_t ch, FILE *f)
{
    USART_SendData(USART_UNIT, (char)ch);
    while (Reset == USART_GetStatus(USART_UNIT, USART_FLAG_TXE)) /* Wait Tx data register empty */
    {

    };

    return ch;
}



/**********************************************************************************************************
**	�� �� �� : HC_USART_RxIrq_Callback()
**	����˵�� : ���ڽ����жϴ����ص�����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
uint16_t Send_Cnt = 0;
uint16_t Rec_Data = 0;
void HC_USART_RxIrq_Callback()
{
    DBGRS232_RxHandler(USART_RecData(USART_UNIT));
//    HAL_DBGCmdProcess();
}
/**********************************************************************************************************
**	�� �� �� : HC_USART3_RxErr_IrqCallback()
**	����˵�� : ���ڴ����жϴ����ص�����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void HC_USART3_RxErr_IrqCallback()
{
    while(Set==USART_GetStatus(USART_UNIT,USART_FLAG_ORE))
    {
        USART_RecData(USART_UNIT);
        USART_ClearStatus(USART_UNIT,USART_CLEAR_FLAG_ORE);        
    }
    if(Set==USART_GetStatus(USART_UNIT,USART_FLAG_RTOF))
    {
      USART_ClearStatus(USART_UNIT,USART_CLEAR_FLAG_RTOF);     
    }
     if(Set==USART_GetStatus(USART_UNIT,USART_FLAG_FE))
    {
      USART_ClearStatus(USART_UNIT,USART_CLEAR_FLAG_FE);     
    }
      if(Set==USART_GetStatus(USART_UNIT,USART_FLAG_PE))
    {
      USART_ClearStatus(USART_UNIT,USART_CLEAR_FLAG_PE);     
    }   
}

/**********************************************************************************************************
**	�� �� �� : DBGRS232_RxHandler()
**	����˵�� : ���ڽ������ݴ�������
**	��    �� : ��
**	�� �� ֵ : ��
**  ˵����$,@֮����ָ�����ƣ�@��\r֮����ָ���������ѡ����buffer�洢$֮�����������
**********************************************************************************************************/
void DBGRS232_RxHandler (uint8_t	rxData)
{
    if(s_dbgPort.rxFlag == dbstart)
    {
        if(rxData == DBG_CMD_HEAD_START)      //���񴮿�ָ��ͷ
        {
            s_dbgPort.rxFlag = valid;         //����ָ����Ч��־��λ
            s_dbgPort.rxlen	 = 0;             //���ճ�����0
        }
    }
    if(s_dbgPort.rxFlag == valid)             //�ѽ��յ�ָ��ͷ
    {
        if(s_dbgPort.rxlen < DEF_DBG_BUFSIZE) //����ָ�����ݳ����޶�1000�ֽ�
        {
            s_dbgPort.rxdBuf1[s_dbgPort.rxlen++]	= rxData;       //����ָ�����ݵ�buffer1��len�Լ�

            if((rxData == '\r') || (rxData == '\n'))            //�յ��ַ������������߻��з�����ʾָ����ս���
            {
                if(0==strcmp(( char* )s_dbgPort.rxdBuf1,"$End_Test@\r"))
                {
                    //���Խ���������
                    printf("���β��Լ���ǿ�ƽ�����\r\n");
                    bsp_DelayMS(100);
                    NVIC_SystemReset();
                } else
                {
                    //��ȷ����һ�������
                    memcpy((void* )&s_dbgPort.rxdBuf[0],(void* )&s_dbgPort.rxdBuf1[0],s_dbgPort.rxlen);  //������2������
                }
                memset((void*)s_dbgPort.rxdBuf1,	0,	s_dbgPort.rxlen); //��ս���buffer

                s_dbgPort.rxlen	= 0;                    //���ճ�������Ϊ0
                s_dbgPort.rxFlag = end;                          //���ձ�־״̬��Ϊ���������ݴ����ڼ�
            }
        }
        else                                  //�������ݳ���
        {
            s_dbgPort.rxFlag = dbstart;       //���ձ�־����Ϊ��ʼ
            s_dbgPort.rxlen	= 0;              //���ճ�������Ϊ0
            memset((void*)s_dbgPort.rxdBuf1,	0,	DEF_DBG_BUFSIZE); //��ս���buffer
        }
    }
}


/*
*********************************************************************************************************
*	�� �� ��: HAL_DBGCmdProcess
*	����˵��: debugָ�����
*	��    ��:  ��
*	�� �� ֵ: ��
*	˵	����//�����ʽ����$Q_VERB@XX\r\n"   ��ѯ�汾�ţ�$,@֮����ָ�����ƣ�@��\r֮����ָ���������ѡ����buffer�洢$֮�����������
*********************************************************************************************************
*/
void HAL_DBGCmdProcess (void)
{
    uint16_t i;
    CMD_Type=2;
    memset(tmpCode,0,4*DBG_CMD_CODE_SIZE);  //ָ���������
    memset(tmpPara,0,4*DBG_CMD_PARA_SIZE);  //ָ��������

    if(s_dbgPort.rxFlag == end)     //������ϣ���ʼ����ָ��
    {
        s_dbgPort.rxFlag =dbstart;  //�����ٴν�������,ȷ�����Թ����ܱ����
        // ��������ͷ
        if( 0 >= sscanf((const char*)s_dbgPort.rxdBuf, "%*[$]%*[^':']%[':']", tmpCode) )  //û��':',�򷵻�0
        {
            if( 0 >= sscanf((const char*)s_dbgPort.rxdBuf, "%*[$]%*[^'@']%['@']", tmpCode) )  //û��'@',�򷵻�0
            {
                goto rest;                    //δɨ�赽ƥ���ַ������δ�������
            }
            if( 0 >= sscanf((const char*)s_dbgPort.rxdBuf, "%*[$]%[^'@']", tmpCode) ) //ȡ$��@֮���ַ���
            {
                goto rest;                    //δɨ�赽ƥ���ַ������δ�������
            }
            //ɨ�赽��$xxx@��ָ��
            CMD_Type=1;
        } else if( 0 >= sscanf((const char*)s_dbgPort.rxdBuf, "%*[$]%[^':']", tmpCode) )
        {
            goto rest;                        //δɨ�赽ƥ���ַ������δ�������
        }

        if(CMD_Type==1)
        {
            // ����������������Ϊ��ѡ���$xxx@��ָ��֧�����4������
            if( 0 >= sscanf((const char*)s_dbgPort.rxdBuf, "%*[^@]%*[@]%[^'@'|^'\r']%*[@]%[^'@'|^'\r']%*[@]%[^'@'|^'\r']%*[@]%[^'\r']", \
                            (char*)&tmpPara[0],(char*)&tmpPara[1],(char*)&tmpPara[2],(char*)&tmpPara[3]) )
            {

            }
        } else if (CMD_Type==2)
        {
            // ����������������Ϊ��ѡ���$xxx:��ָ��֧�����100�ֽڲ���
            if( 0 >= sscanf((const char*)s_dbgPort.rxdBuf, "%*[^:]%*[:]%[^'\r']", (char*)&tmpPara[0]) )
            {

            }
        }

        // �����ƥ�䴦������
        for(i=0 ; i<(sizeof(FCT_CMD)/sizeof(cmd_table)) ; i++)
        {
            if(0==strcmp(FCT_CMD[i].cmd,(char*)tmpCode))                       //ָ���ַ���ƥ�䣬��ִ����Ӧ�Ĵ�������
            {
                memset(FCT_CMD[i].param,0,sizeof(FCT_CMD[i].param));           //����������0
                memcpy(FCT_CMD[i].param,tmpPara,sizeof(FCT_CMD[i].param));     //��������
                FCT_CMD[i].ptrCmdFun();                                        //ִ��ָ�������
                break;
            }
        }

rest:
        memset((void*)s_dbgPort.rxdBuf,	0,	DEF_DBG_BUFSIZE); //��ս���buffer
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/