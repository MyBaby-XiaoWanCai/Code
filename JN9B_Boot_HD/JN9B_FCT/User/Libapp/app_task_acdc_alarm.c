/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_task_acdc_alarm.c
** Author      : Wl
** Date        : 2021-08-20
** Description : Acdc_alarm application task file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t features = 0; //������
uint16_t ACDC_a1Flg=0; //ģ��1�������㱨����־������λ�����������
uint16_t ACDC_a2Flg=0; //ģ��2�������㱨����־������λ�����������
/*���籨��״̬*/
enum gd_state
{
    GDAC_OV = 0x01 << 5,  //AC��ѹ��state0
    GDAC_UV = 0x01 << 6,  //ACǷѹ, state0
    GDAC_OF = 0x01 << 4,  //AC��Ƶ, state0
    GDGD_PT = 0x01 << 7,  //�µ�����, state0
    GDDC_OV = 0x01 << 10, //DC��ѹ, state1
    GDDC_OA = 0x01 << 11, //DC����, state1
    GDDC_ST = 0x01 << 9,  //DC��·, state1
    GDFN_ER = 0x01 << 3,  //�ͻ����ȹ���|ACDC���ȹ���, state0, ��ACDC��������Ϊ�ڲ�����
    GDTM_OT = 0x01 << 8,  //���±���, state1
};

/*���籨����������ұ�*/
arr_list gdlist[] =
{
    {0x01, GDAC_OV},
    {0x02, GDAC_UV},
    {0x03, GDAC_OF},
    {0x06, GDGD_PT},
    {0x07, GDDC_OV},
    {0x08, GDDC_OA},
    {0x09, GDDC_ST},
    {0x0a, GDFN_ER},
    {0x0b, GDTM_OT}
};

/*�յ±���״̬*/
enum pd_state
{
    PDAC_OV = 0x01 << 4,  //AC��ѹ
    PDAC_UV = 0x01 << 5,  //ACǷѹ
    PDAC_OF = 0x01 << 6,  //AC��Ƶ
    PDAC_UF = 0x01 << 7,  //ACǷƵ
    PDAC_OP = 0x01 << 8,  //AC�ϵ�
    PDGD_PT = 0x01 << 9,  //�µ�����
    PDDC_OV = 0x01 << 10, //DC��ѹ
    PDDC_OA = 0x01 << 12, //DC����
    PDDC_ST = 0x01 << 11, //DC��·
    PDTM_OT = 0x01 << 13, //���±���
    PDFN_ER1 = 0x01 << 14, //�ͻ����ȹ��ϣ���ACDC��������Ϊ�ڲ�����
    PDFN_ER2 = 0x01 << 15, //�ڲ����ȹ���
};

/*�յ±�����������ұ�*/
arr_list pdlist[] =
{
    {0x01, PDAC_OV},
    {0x02, PDAC_UV},
    {0x03, PDAC_OF},
    {0x04, PDAC_UF},
    {0x05, PDAC_OP},
    {0x06, PDGD_PT},
    {0x07, PDDC_OV},
    {0x08, PDDC_OA},
    {0x09, PDDC_ST},
    {0x0a, PDFN_ER2},
    {0x0b, PDTM_OT}
};

acdc_alarmStruct acdc_AlarmBuf;   //����״̬��������buf
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� �� : AppACDC_alarm()
**  ����˵�� : ACDC�澯�ϱ�����
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void AppACDC_alarm(void *argument)
{
    uint16_t alarm1, alarm2; //bit=1��λ���б仯��״̬Ϊ�澯��λ
    uint8_t i;

//	int test_printf_acdc = 0;

    while(1)
    {
        if(osSemaphoreAcquire(acdc_alarm_Semaphore, osWaitForever) == osOK) //�ȴ�ͬ���ź���
        {
            /*ACDC������֡ͷ2�ֽ�+���ݳ���2�ֽ�+������1�ֽ�+״̬��1�ֽ�+ͨ����־1�ֽ�+�����ֶ�2�ֽڣ�������+�����+У��2�ֽ�+֡β2�ֽ�*/
            acdc_AlarmBuf.FramHead = 0x5B5B;   //��λ������λ������֡ͷ0x5B5B
            acdc_AlarmBuf.Datelen = 0x09;      //�̶�����9
            acdc_AlarmBuf.FeatureCode = 0x00;  //�����룬ÿ���ϱ��Լ�1
            acdc_AlarmBuf.StateCode = 0x00;    //״̬��=0��OK
            acdc_AlarmBuf.ChanelFlag = 0x00;   //ACDC����ʱ���̶�0
            acdc_AlarmBuf.CommandCode = 0xA1;  //ACDC����,�̶�
            acdc_AlarmBuf.ChildCommand = 0x00; //����can�ϱ����ݺ��������붨��ȷ��
            acdc_AlarmBuf.CRC16Code = 0x00;    //��������
            acdc_AlarmBuf.FramTail = 0xA5A5;   //֡β

            //����ACDC��ǰ״̬���������ϱ����ݣ���¼�б仯,�Ҹ��º��ֵΪ1��bit
            alarm1 = ACDC.BitFlg[0] & ACDC.State[0];
            alarm2 = ACDC.BitFlg[1] & ACDC.State[1];

            //�жϹ�������ͨ��
            if(alarm1)
            {
                acdc_AlarmBuf.ChanelFlag |= 0x00;   //ACDC����������canid��ͨ����־Ϊ���ͨ��������ʱĬ��0
            }
            if(alarm2)
            {
                acdc_AlarmBuf.ChanelFlag |= 0x00;  //ACDC����������canid��ͨ����־Ϊ���ͨ��������ʱĬ��0
            }

            //����ʹ�õ�ACDC���ͣ�������Э���ֲ�ȷ����������
            switch(ACDC.Type)
            {
            case 1:    //���籨������
                for(i = 0; i < 8; i++)
                {
                    if(alarm1 & gdlist[i].state)
                    {
						Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);   
                        acdc_AlarmBuf.ChildCommand = gdlist[i].ChildCom; //����ȡ��������
                        acdc_AlarmBuf.FeatureCode = features++;         //������+1
                        acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //����CRCУ���ֶ�
                        W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //���ڷ��ͱ�����Ϣ��0Ϊ������socket
                    }
                    if(alarm2 & gdlist[i].state)
                    {
						Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);        
                        acdc_AlarmBuf.ChildCommand = gdlist[i].ChildCom; //����ȡ��������
                        acdc_AlarmBuf.FeatureCode = features++;         //������+1
                        acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //����CRCУ���ֶ�
                        W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //���ڷ��ͱ�����Ϣ��0Ϊ������socket
                    }
                }
                /*�ͻ����ȹ��ϣ������ڲ�������ע�⣺�����Э�鲻����ACDC���ȺͿͻ����ȹ��ϣ��˴��ڲ�����������������*/
                if((alarm1 & GDFN_ER) | (alarm2 & GDFN_ER))
                {
					Protect_BitAction_Func(ALLCH, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP);   
                        for(uint8_t j = 0; j < CHANNEL_CFG ; j++)
                        { IP_Alarm_Flag[j] |= IntelProtect_FHE; }   //�ڲ����ȸ澯
                }

                break;

            case 2:   //�յ±�������
                for(i = 0; i < 11; i++)
                {
                    if(alarm1 & pdlist[i].state)
                    {
						Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);    
                        acdc_AlarmBuf.ChildCommand = pdlist[i].ChildCom; //����ȡ��������
                        acdc_AlarmBuf.FeatureCode = features++;         //������+1
                        acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //����CRCУ���ֶ�
                        W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //���ڷ��ͱ�����Ϣ��0Ϊ������socket
                    }
                    if(alarm2 & pdlist[i].state)
                    {
						Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP); 
                        acdc_AlarmBuf.ChildCommand = pdlist[i].ChildCom; //����ȡ��������
                        acdc_AlarmBuf.FeatureCode = features++;         //������+1
                        acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //����CRCУ���ֶ�
                        W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //���ڷ��ͱ�����Ϣ��0Ϊ������socket
                    }
                }

                /*�ͻ����ȹ��ϣ������ڲ�����*/
                if((alarm1 & PDFN_ER1) | (alarm2 & PDFN_ER1))
                {
					Protect_BitAction_Func(ALLCH, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP);   
                        for(uint8_t j = 0; j < CHANNEL_CFG ; j++)
                        { IP_Alarm_Flag[j] |= IntelProtect_FHE; }   //�ڲ����ȸ澯
                }
                break;
            default :
                break;
            }

        }
		
//		if((++test_printf_acdc % 500) == 0)
		{
			printf("acdc task is running!");
		}		
		
        osDelay(1);
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
