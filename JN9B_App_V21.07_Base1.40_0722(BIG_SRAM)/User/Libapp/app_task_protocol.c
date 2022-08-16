/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_func_protocol.c
** Author      : Cai Heng
** Date        : 2021-08-08
** Description :
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"
#include "cmsis_os2.h"
/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
extern uint8_t SN[3] ;
uint16_t Pack_NO=0;
uint32_t Write_ADDR= EFM_ADDR_SECTOR128;

__IO uint8_t ACDC_UpdateFlg = 0;      //ACDC������־��0��ֹ��1����
uint8_t System_UpdateFlg = 0;         //ϵͳ������־��0��ֹ��1����
uint8_t ucNetSendBuf[1024] = {0};     //���ͻ���
uint8_t ucNetRecvBuf[1024] = {0};     //���ջ���
uint16_t usNetSendCnt = 0;            //���ͼ���
uint16_t usNetRecvCnt = 0;            //���ռ���

uint8_t workstep_flag_RunstaSet = 0;
uint8_t workstep_flag_AbortPrio[8] = {SCMD_WORKSTEP_PARAM_TIME, SCMD_WORKSTEP_PARAM_TIME, SCMD_WORKSTEP_PARAM_TIME, SCMD_WORKSTEP_PARAM_TIME, SCMD_WORKSTEP_PARAM_TIME, SCMD_WORKSTEP_PARAM_TIME, SCMD_WORKSTEP_PARAM_TIME, SCMD_WORKSTEP_PARAM_TIME};
uint16_t workstep_flag_ErrWord[8] = {0};
uint8_t  workstep_flag_CriticalZone[8] = {0};

uint8_t w5500_Silence = 0;       				//���ھ�Ĭ��־
s_SystemStruct SystemStatus;   					//ϵͳ����״̬
s_Tier1_Struct Tier1;
s_Tier2_Struct Tier2;
s_ProtocolStruct unpack;
s_ProtocolStruct pack;

uint8_t heart_beat_flag = 2;	//0
uint8_t upcast_enable = 0xFF;					/* ȫ��ͨ��ʹ������ */
uint8_t can_update = 0;

uint8_t record_data_upcast_flag[8] = {0};		/* �������ױ�ʶ */

uint8_t refresh_continue[8] = {1, 1, 1, 1, 1, 1, 1, 1};
uint16_t debug_ch_onoff_state = 0;
extern uint8_t PG_state;
extern uint8_t TW_HisAlam;          //ͼΪ��ʷ�澯
extern uint8_t TW_HisStatus;        //ͼΪ��ʷ״̬
extern osMessageQueueId_t ACDC_UpdateQueue;
extern ACDC_UpdateStrcut CanRet;
extern __IO uint8_t Clear_AM_Flg;
extern uint8_t  w5500_remote_ip[4];	//Զ�˷�����IP
extern uint16_t w5500_remote_port;	                //Զ�˷������˿�
extern Net_Mod Net_Mod_State;
extern osThreadId_t ThreadIdTaskDebug;
extern osThreadId_t ThreadIdTaskWorkstep;
extern osTimerId_t  timerID_Periodic_adc3;
extern osThreadId_t  ThreadIdTaskProtection;
extern osThreadId_t  ThreadIdStart;

uint8_t  ucProtocolVersion = 1;		/* 0 - old version   1 - new version   default - new version */
uint16_t lenAnalysis = 0;
uint8_t  feed_hardware_watchdog_enable = 1;
__IO float data_upcast_period = 1.0f;	/* ��������ʱ�� */
uint8_t Reconnection_Flg = 0;	//CAN������־
uint8_t ParaCHxConfig = 0x01; //������־ 1-8ch 120A,2-4ch 240A, 8-1ch 600A
uint8_t FeedWdgFlg = 0x00;
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**  �� �� ��: CRC16_2()
**  ����˵��:
**  ��    ��:
**  �� �� ֵ:
**********************************************************************************************************/
uint16_t CRC16_2(uint8_t datain, uint16_t CRCin)
{
    uint16_t i = 0;
    uint16_t crc_tmp = CRCin ^ datain;

    for(i = 0; i < 8; i++)
    {
        if(crc_tmp & 0x01)
        {
            crc_tmp = (crc_tmp >> 1) ^ 0xA001;
        }
        else
        {
            crc_tmp = (crc_tmp >> 1);
        }
    }

    return crc_tmp;
}

/**********************************************************************************************************
**  �� �� ��: CRC16N()
**  ����˵��:
**  ��    ��:
**  �� �� ֵ:
**********************************************************************************************************/
uint16_t CRC16N(uint8_t *s, uint16_t len)
{
    uint16_t i = 0;
    uint16_t crc = 0xFFFF;

    for(i = 0; i < len; i++)
    {
        crc = CRC16_2(*s++, crc);
    }

    return crc;
}

/**********************************************************************************************************
**  �� �� �� : RecvUnpack()
**  ����˵�� : ���ղ������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
static uint8_t RecvUnpack(uint8_t *pRecv, uint16_t lenRecv)
{
    /*
    ** �ɰ汾��֡ͷ(2 Byte) + ֡��(2 Byte) + ͨ����(1 Byte) + ��������(1 Byte) + ...... + У����(2 Byte) + ֡β(2 Byte)
    **
    ** �°汾��֡ͷ(2 Byte) + ֡��(2 Byte) + ������(1 Byte) + ״̬��(1 Byte) + ͨ����(1 Byte) + ��������(1 Byte) + ...... + У����(2 Byte) + ֡β(2 Byte)
    */
    if(*(pRecv + 5) != 0)			/* ������Ϊ�������� */
    {
        ucProtocolVersion = 0;				/* �ɰ汾Э�� */
    }
    else							/* Ϊ����Ϊ״̬�� */
    {
        ucProtocolVersion = 1;				/* �°汾Э�� */
    }

    if(ucProtocolVersion == 0)		/* �ɰ汾Э�� */
    {
        /* ע: ʹ��memcpy()ֱ�Ӹ�ֵ������С��ģʽ */
        memcpy(&unpack.head, pRecv + 0, 2);				/* ֡ͷ */
        memcpy(&unpack.len , pRecv + 2, 2);				/* ֡�� */
        memcpy(&unpack.chEn, pRecv + 4, 1);				/* ͨ���� */
        memcpy(&unpack.crc , pRecv + lenRecv - 4, 2);	/* У���� */
        memcpy(&unpack.tail, pRecv + lenRecv - 2, 2);	/* ֡β */
    }
    else							/* �°汾Э�� */
    {
        /* ע: ʹ��memcpy()ֱ�Ӹ�ֵ������С��ģʽ */
        memcpy(&unpack.head, pRecv + 0, 2);				/* ֡ͷ */
        memcpy(&unpack.len , pRecv + 2, 2);				/* ֡�� */
        memcpy(&unpack.feat, pRecv + 4, 1);				/* ������ */
        memcpy(&unpack.sta , pRecv + 5, 1);				/* ״̬�� */
        memcpy(&unpack.chEn, pRecv + 6, 1);				/* ͨ���� */
        memcpy(&unpack.crc , pRecv + lenRecv - 4, 2);	/* У���� */
        memcpy(&unpack.tail, pRecv + lenRecv - 2, 2);	/* ֡β */
    }

    /* ֡ͷ�ж� */
    if(unpack.head != FRAME_HEAD_RECV)
    {
        return FRAME_HEAD_ERR;      /* ֡ͷ���� */
    }

    /* ֡���ж� */	/* ע: ���ݳ��Ȳ�����֡ͷ(2Byte)��֡β(2Byte) */
    if(unpack.len != (lenRecv - 4))
    {
        return FRAME_LEN_ERR;       /* ֡������ */
    }

    if(ucProtocolVersion == 0)		/* �ɰ汾Э�� */
    {
        /* У�����ж� */	/* ע: �����ݳ��ȵ��������ݶν��� */
        if(unpack.crc != CRC16N(pRecv + 2, lenRecv - 6))
        {
//      	return FRAME_CRC_ERR;	/* У������� */
        }

        /* ֡β�ж� */
        if(unpack.tail != FRAME_TAIL_RECV)
        {
            return FRAME_TAIL_ERR;	/* ֡β���� */
        }
    }
    else							/* �°汾Э�� */
    {
        /* �������ж� */	/* ע: ������֡������������벻ͬ�������벻��Ϊ0 */
        if(unpack.feat == pack.feat)
        {
            if(g_ucSysMode == SYS_NORMAL_MODE)
            {
                return FRAME_FEAT_ERR;	/* ��������� */
            }
        }

        /* ״̬���ж� */
        if(unpack.sta != 0)
        {
            return FRAME_STATE_ERR;	/* ״̬����� */
        }

        /* У�����ж� */	/* ע: �����ݳ��ȵ��������ݶν��� */
        if(unpack.crc != CRC16N(pRecv + 2, lenRecv - 6))
        {
//			return FRAME_CRC_ERR;	/* У������� */
        }

        /* ֡β�ж� */
        if(unpack.tail != FRAME_TAIL_RECV)
        {
            return FRAME_TAIL_ERR;	/* ֡β���� */
        }
    }

    return FRAME_IS_OK;				/* ֡����OK */
}

/**********************************************************************************************************
**  �� �� �� : GlobalProtectSetAnalysis()
**  ����˵�� : ȫ�ֱ������ý�������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t GlobalProtectSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;			/* �������� */
    uint8_t scmd_offset = 0;	/* ��������ƫ�� */
    float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint8_t scmd_reply = 0;		/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_GLOBAL_PROTECT_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    s_Tier1_ProtectStruct *p1;      /* һ�׽ṹ��ָ��(���δ����Ĺ��ܶ�����) */
    s_Tier2_ProtectStruct *p2;      /* ���׽ṹ��ָ��(����Ѵ���Ĺ��ܶ�����) */

    /* ���ö��׽ṹ����������һ�׽ṹ������(����ṹ�岿�����ݵ�д����Ӱ����������) */
    for(int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            p1 = &Tier1.gProt[j].VbatThrH;
            p2 = &Tier2.gProt[j].VbatThrH;
            for(int k = SCMD_GLOBAL_PROTECT_SET_BEGIN; k < SCMD_GLOBAL_PROTECT_SET_END - 1; k++, p1++, p2++)
            {
                p1->cmd = p2->isEnable;
                p1->dat = p2->value;
            }
        }
    }

    /* ���ܶ����ݽ��� */
    for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 5)
    {
        memcpy(&scmd, pRecv + i + 0, 1);	/* �������� */
        memcpy(&sdat, pRecv + i + 1, 4);	/* ������������ */

        /* ��������������Ϸ����ж� */
        if( !((scmd > SCMD_GLOBAL_PROTECT_SET_BEGIN ) && (scmd < SCMD_GLOBAL_PROTECT_SET_END )) && \
                !((scmd > SCMD_GLOBAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_GLOBAL_PROTECT_NSET_END)) )
        {
            scmd_reply = 0xFF;
            memcpy(pSend + *lenSend, &scmd      , 1);
            *lenSend += 1;
            memcpy(pSend + *lenSend, &scmd_reply, 1);
            *lenSend += 1;
            if(scmd != SCMD_INVALID)	/* ����Ч�������� */	/* ��Ч��Ϊ���쳣�� */
            {
                result = FRAME_SCMD_ERR;					/* ��������Ϊ�쳣�� */
            }
            continue;										/* �������벻ƥ��(��ת��һ��) */
        }

        /* ��������������ƫ�Ƽ��� */
        if((scmd > SCMD_GLOBAL_PROTECT_SET_BEGIN) && (scmd < SCMD_GLOBAL_PROTECT_SET_END))
        {
            scmd_offset = (scmd - SCMD_GLOBAL_PROTECT_SET_BEGIN) - 1;
        }
        else if((scmd > SCMD_GLOBAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_GLOBAL_PROTECT_NSET_END))
        {
            scmd_offset = (scmd - SCMD_GLOBAL_PROTECT_NSET_BEGIN) - 1;
        }

        /* �����������������ݴ��� */
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                p1 = &Tier1.gProt[j].VbatThrH;
                p2 = &Tier2.gProt[j].VbatThrH;

                /* ָ���öζ�Ӧ�Ľṹ���Ա���� */
                uint8_t *scmd_addr = &(p1 + scmd_offset)->cmd;
                float   *sdat_addr = &(p1 + scmd_offset)->dat;
                float   *smin_addr = &(p2 + scmd_offset)->min;
                float   *smax_addr = &(p2 + scmd_offset)->max;
                /* �����ݿ�������Ӧ�Ľṹ���Ա������ */
                memcpy(scmd_addr, &scmd, 1);
                memcpy(sdat_addr, &sdat, 4);
                if((*scmd_addr > SCMD_GLOBAL_PROTECT_SET_BEGIN) && (*scmd_addr < SCMD_GLOBAL_PROTECT_SET_END))      			/* ����ʹ�� */
                {
                    if((*smin_addr == 0) && (*smax_addr == 0))							/* ��У�� */
                    {
                        SET_0(scmd_reply, j);
                    }
                    else if((*sdat_addr >= *smin_addr) && (*sdat_addr <= *smax_addr))   /* У��ɹ� */
                    {
                        SET_0(scmd_reply, j);
                    }
                    else                                                                /* У��ʧ�� */
                    {
                        SET_1(scmd_reply, j);
                        result = FRAME_SCMD_ERR;
                    }
                }
                else /* if((*scmd_addr > SCMD_GLOBAL_PROTECT_NSET_BEGIN) && (*scmd_addr < SCMD_GLOBAL_PROTECT_NSET_END)) */		/* ����ʧ�� */
                {
                    SET_0(scmd_reply, j);												/* ��У�� */
                }
            }
        }
        memcpy(pSend + *lenSend, &scmd      , 1);
        *lenSend += 1;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
    }

    /* ����������� */
    if(result == 0)
    {
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                p1 = &Tier1.gProt[j].VbatThrH;
                p2 = &Tier2.gProt[j].VbatThrH;

                /* ���¶��׽ṹ������(�������������) */
                for(int k = SCMD_GLOBAL_PROTECT_SET_BEGIN; k < SCMD_GLOBAL_PROTECT_SET_END - 1; k++, p1++, p2++)
                {
                    p2->isEnable = p1->cmd;
                    p2->value    = p1->dat;
                    /* �洢��EEPROM�е�ȫ�ֱ���������������ֹ */
                    SET_1(p1->cmd, 7);
                }
                EEP_DAT_SYNC(PROTECT_GLOBAL_ID(j));
            }
        }
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : GlobalProtectInqAnalysis()
**  ����˵�� : ȫ�ֱ�����ѯ��������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t GlobalProtectInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;			/* �������� */
    uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    float   scmd_reply = 0.0f;	/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_GLOBAL_PROTECT_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ���ܶ����ݽ��� */
    for(int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            s_Tier1_ProtectStruct *p1 = &Tier1.gProt[j].VbatThrH;       /* һ�׽ṹ��ָ�� */
            s_Tier2_ProtectStruct *p2 = &Tier2.gProt[j].VbatThrH;       /* ���׽ṹ��ָ�� */
            /* ���ö��׽ṹ�����ݸ���һ�׽ṹ������(���ڻظ�) */
            for(int k = SCMD_GLOBAL_PROTECT_INQ_BEGIN; k < SCMD_GLOBAL_PROTECT_INQ_END - 1; k++, p1++, p2++)
            {
                p1->cmd = p2->isEnable;
                p1->dat = p2->value;
            }
            /* ����ָ���ʼֵ */
            p1 = &Tier1.gProt[j].VbatThrH;
            p2 = &Tier2.gProt[j].VbatThrH;

            /* ���������ѭ������ */
            for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 1)
            {
                memcpy(&scmd, pRecv + i, 1);

                if(scmd == SCMD_INVALID)	/* ��Ч�� */
                {
                    if(i == lenAnalysis)	/* �׸���������Ϊ��Ч�� - ȫ����ѯ */
                    {
                        for(int k = SCMD_GLOBAL_PROTECT_INQ_BEGIN; k < SCMD_GLOBAL_PROTECT_INQ_END - 1; k++, p1++, p2++)
                        {
                            memcpy(pSend + *lenSend, &p1->cmd, 1);
                            *lenSend += 1;
                            memcpy(pSend + *lenSend, &p1->dat, 4);
                            *lenSend += 4;
                        }
                        break;
                    }
                    else
                    {
                        scmd_reply = 0xFFFFFFFF;
                        memcpy(pSend + *lenSend, &scmd      , 1);
                        *lenSend += 1;
                        memcpy(pSend + *lenSend, &scmd_reply, 4);
                        *lenSend += 4;
                        continue;
                    }
                }
                else if((scmd <= SCMD_GLOBAL_PROTECT_INQ_BEGIN) || (scmd >= SCMD_GLOBAL_PROTECT_INQ_END))
                {
                    scmd_reply = 0xFFFFFFFF;
                    memcpy(pSend + *lenSend, &scmd      , 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &scmd_reply, 4);
                    *lenSend += 4;
                    result = FRAME_SCMD_ERR;	/* ��������Ϊ�쳣�� */
                    continue;					/* �������벻ƥ��(��ת��һ��) */
                }

                /* ��������ƫ���� */
                scmd_offset = (scmd - SCMD_GLOBAL_PROTECT_INQ_BEGIN) - 1;
                /* ��������ظ� */
                scmd        = (p1 + scmd_offset)->cmd;
                /* �����������ݻظ� */
                scmd_reply  = (p1 + scmd_offset)->dat;
                memcpy(pSend + *lenSend, &scmd      , 1);
                *lenSend += 1;
                memcpy(pSend + *lenSend, &scmd_reply, 4);
                *lenSend += 4;
            }
        }
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : InternalProtectSetAnalysis()
**  ����˵�� : �ڲ��������ý�������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t InternalProtectSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;			/* �������� */
    uint8_t scmd_offset = 0;	/* ��������ƫ�� */
    float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint8_t scmd_reply = 0;		/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_INTERNAL_PROTECT_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    s_Tier1_ProtectStruct *p1;      /* һ�׽ṹ��ָ��(���δ����Ĺ��ܶ�����) */
    s_Tier2_ProtectStruct *p2;      /* ���׽ṹ��ָ��(����Ѵ���Ĺ��ܶ�����) */

    /* ���ö��׽ṹ����������һ�׽ṹ������(����ṹ�岿�����ݵ�д����Ӱ����������) */
    for(int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            p1 = &Tier1.iProt[j].IoutThr;
            p2 = &Tier2.iProt[j].IoutThr;
            for(int k = SCMD_INTERNAL_PROTECT_SET_BEGIN; k < SCMD_INTERNAL_PROTECT_SET_END - 1; k++, p1++, p2++)
            {
                p1->cmd = p2->isEnable;
                p1->dat = p2->value;
            }
        }
    }

    /* ���ܶ����ݽ��� */
    for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 5)
    {
        memcpy(&scmd, pRecv + i + 0, 1);	/* �������� */
        memcpy(&sdat, pRecv + i + 1, 4);	/* ������������ */

        /* ��������������Ϸ����ж� */
        if( !((scmd > SCMD_INTERNAL_PROTECT_SET_BEGIN ) && (scmd < SCMD_INTERNAL_PROTECT_SET_END )) && \
                !((scmd > SCMD_INTERNAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_INTERNAL_PROTECT_NSET_END)) )
        {
            scmd_reply = 0xFF;
            memcpy(pSend + *lenSend, &scmd      , 1);
            *lenSend += 1;
            memcpy(pSend + *lenSend, &scmd_reply, 1);
            *lenSend += 1;
            if(scmd != SCMD_INVALID)	/* ����Ч�������� */	/* ��Ч��Ϊ���쳣�� */
            {
                result = FRAME_SCMD_ERR;					/* ��������Ϊ�쳣�� */
            }
            continue;										/* �������벻ƥ��(��ת��һ��) */
        }

        /* ��������������ƫ�Ƽ��� */
        if((scmd > SCMD_INTERNAL_PROTECT_SET_BEGIN) && (scmd < SCMD_INTERNAL_PROTECT_SET_END))
        {
            scmd_offset = (scmd - SCMD_INTERNAL_PROTECT_SET_BEGIN) - 1;
        }
        else if((scmd > SCMD_INTERNAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_INTERNAL_PROTECT_NSET_END))
        {
            scmd_offset = (scmd - SCMD_INTERNAL_PROTECT_NSET_BEGIN) - 1;
        }

        /* �����������������ݴ��� */
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                p1 = &Tier1.iProt[j].IoutThr;
                p2 = &Tier2.iProt[j].IoutThr;

                /* ָ���öζ�Ӧ�Ľṹ���Ա���� */
                uint8_t *scmd_addr = &(p1 + scmd_offset)->cmd;
                float   *sdat_addr = &(p1 + scmd_offset)->dat;
                float   *smin_addr = &(p2 + scmd_offset)->min;
                float   *smax_addr = &(p2 + scmd_offset)->max;
                /* �����ݿ�������Ӧ�Ľṹ���Ա������ */
                memcpy(scmd_addr, &scmd, 1);
                memcpy(sdat_addr, &sdat, 4);
                if((*scmd_addr > SCMD_INTERNAL_PROTECT_SET_BEGIN) && (*scmd_addr < SCMD_INTERNAL_PROTECT_SET_END))      		/* ����ʹ�� */
                {
                    if((*smin_addr == 0) && (*smax_addr == 0))							/* ��У�� */
                    {
                        SET_0(scmd_reply, j);
                    }
                    else if((*sdat_addr >= *smin_addr) && (*sdat_addr <= *smax_addr))   /* У��ɹ� */
                    {
                        SET_0(scmd_reply, j);
                    }
                    else                                                                /* У��ʧ�� */
                    {
                        SET_1(scmd_reply, j);
                        result = FRAME_SCMD_ERR;
                    }
                }
                else /* if((*scmd_addr > SCMD_INTERNAL_PROTECT_NSET_BEGIN) && (*scmd_addr < SCMD_INTERNAL_PROTECT_NSET_END)) */	/* ����ʧ�� */
                {
                    SET_0(scmd_reply, j);												/* ��У�� */
                }
            }
        }
        memcpy(pSend + *lenSend, &scmd      , 1);
        *lenSend += 1;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
    }

    /* ����������� */
    if(result == 0)
    {
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                p1 = &Tier1.iProt[j].IoutThr;
                p2 = &Tier2.iProt[j].IoutThr;

                /* ���¶��׽ṹ������(�������������) */
                for(int k = SCMD_INTERNAL_PROTECT_SET_BEGIN; k < SCMD_INTERNAL_PROTECT_SET_END - 1; k++, p1++, p2++)
                {
                    p2->isEnable = p1->cmd;
                    p2->value    = p1->dat;
                    /* �洢��EEPROM�е��ڲ����������������ʹ�� */
                    SET_0(p1->cmd, 7);
                }
                EEP_DAT_SYNC(PROTECT_INTERNAL_ID(j));
            }
        }
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : InternalProtectInqAnalysis()
**  ����˵�� : �ڲ�������ѯ��������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t InternalProtectInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;			/* �������� */
    uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    float   scmd_reply = 0.0f;	/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_INTERNAL_PROTECT_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ���ܶ����ݽ��� */
    for(int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            s_Tier1_ProtectStruct *p1 = &Tier1.iProt[j].IoutThr;        /* һ�׽ṹ��ָ�� */
            s_Tier2_ProtectStruct *p2 = &Tier2.iProt[j].IoutThr;        /* ���׽ṹ��ָ�� */
            /* ���ö��׽ṹ�����ݸ���һ�׽ṹ������(���ڻظ�) */
            for(int k = SCMD_INTERNAL_PROTECT_INQ_BEGIN; k < SCMD_INTERNAL_PROTECT_INQ_END - 1; k++, p1++, p2++)
            {
                p1->cmd = p2->isEnable;
                p1->dat = p2->value;
            }
            /* ����ָ���ʼֵ */
            p1 = &Tier1.iProt[j].IoutThr;
            p2 = &Tier2.iProt[j].IoutThr;

            for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 1)
            {
                memcpy(&scmd, pRecv + i, 1);

                if(scmd == SCMD_INVALID)
                {
                    if(i == lenAnalysis)	/* �׸���������Ϊ��Ч�� - ȫ����ѯ */
                    {
                        for(int k = SCMD_INTERNAL_PROTECT_INQ_BEGIN; k < SCMD_INTERNAL_PROTECT_INQ_END - 1; k++, p1++, p2++)
                        {
                            memcpy(pSend + *lenSend, &p1->cmd, 1);
                            *lenSend += 1;
                            memcpy(pSend + *lenSend, &p1->dat, 4);
                            *lenSend += 4;
                        }
                        break;
                    }
                    else
                    {
                        scmd_reply = 0xFFFFFFFF;
                        memcpy(pSend + *lenSend, &scmd      , 1);
                        *lenSend += 1;
                        memcpy(pSend + *lenSend, &scmd_reply, 4);
                        *lenSend += 4;
                        continue;
                    }
                }
                else if((scmd <= SCMD_INTERNAL_PROTECT_INQ_BEGIN) || (scmd >= SCMD_INTERNAL_PROTECT_INQ_END))
                {
                    scmd_reply = 0xFFFFFFFF;
                    memcpy(pSend + *lenSend, &scmd      , 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &scmd_reply, 4);
                    *lenSend += 4;
                    result = FRAME_SCMD_ERR;		/* ��������Ϊ�쳣�� */
                    continue;						/* �������벻ƥ��(��ת��һ��) */
                }

                /* ��������ƫ���� */
                scmd_offset = (scmd - SCMD_INTERNAL_PROTECT_INQ_BEGIN) - 1;
                /* ��������ظ� */
                scmd        = (p1 + scmd_offset)->cmd;
                /* �����������ݻظ� */
                scmd_reply  = (p1 + scmd_offset)->dat;
                memcpy(pSend + *lenSend, &scmd      , 1);
                *lenSend += 1;
                memcpy(pSend + *lenSend, &scmd_reply, 4);
                *lenSend += 4;
            }
        }
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : RecordDatSync()
**  ����˵�� : ��¼����ͬ������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
void RecordDatSync(uint8_t chx)
{
    uint8_t j = chx;

    // 1. ʱ���
    Tier2.record[j].Tstamp   = Tstamp;
    // 2. ����״̬
    if(start_state_flag[j] == 2)
    {
        start_state_flag[j] = 0;
        Tier2.record[j].RunSta = WORKSTEP_RUNSTA_REF_START;
    }
    else
    {
        Tier2.record[j].RunSta = Tier2.workstepRunning[j].RunSta;
    }
    // 3. ��������
    if(Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_START)
    {
        Tier2.record[j].StepType = Tier2.workstepCache1[j].Type;	// ���ⷢ���������̿�ʼ֡ʱ��żȻ���ֹ�������Ϊ�յ����
    }
    else
    {
        Tier2.record[j].StepType = Tier2.workstepRunning[j].StepType;
    }
    // 4. ������
    if(Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_START)
	{
		Tier2.record[j].StepNumb = Tier2.workstepCache1[j].Numb;
	}
	else
	{
		Tier2.record[j].StepNumb = Tier2.workstepRunning[j].StepNumb;
	}

//    Tier2.record[j].StepNumb = Tier2.workstepRunning[j].StepNumb;
    
    // 5. ����ʱ��
//	if( (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
//		(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) )
//	{
//		Tier2.record[j].Trun = Tier2.workstepRunning[j].Trun / 1000.0f;
//	}
//	else if( (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE) || \
//			 (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_WAIT    ) )
//	{
//		if(workstep_flag_AbortPrio[j] == SCMD_WORKSTEP_PARAM_TIME)
//		{
//			Tier2.record[j].Trun = Tier2.workstepRunning[j].Trun / 1000;
//		}
//		else
//		{
//			Tier2.record[j].Trun = Tier2.workstepRunning[j].Trun / 1000.0f;
//		}
//	}
//	else
//	{
//		Tier2.record[j].Trun = Tier2.workstepRunning[j].Trun / 1000.0f;
//	}
    if(time_over_stop[j] == 1)
    {
        Tier2.record[j].Trun = Tier2.workstepRunning[j].Trun / 1000;
    }
    else
    {
        Tier2.record[j].Trun = Tier2.workstepRunning[j].Trun / 1000.0f;
    }
    // 6. ��ص�ѹ
    Tier2.record[j].Vbat     = Tier2.chRecord[j].Vbat;
    // 7. �������(���为��)
    if( (Tier2.workstepRunning[j].StepType > SCMD_WORKSTEP_TYPE_STANDBY  ) && \
            (Tier2.workstepRunning[j].StepType < SCMD_WORKSTEP_TYPE_FINISH   ) && \
            ( (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
              (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) ) )
    {
        if(Tier2.chRecord[j].Iout < (0.0005f * WORK_CUR_MAX))
        {
            Tier2.record[j].Iout = 0.0f;
        }
        else if(Mod_DIR_Flag[(CHANNEL_MAX / CHANNEL_CFG) * j] == BUCK_MODE)
        {
            Tier2.record[j].Iout = Tier2.chRecord[j].Iout;
        }
        else if(Mod_DIR_Flag[(CHANNEL_MAX / CHANNEL_CFG) * j] == BOOST_MODE)
        {
            Tier2.record[j].Iout = -Tier2.chRecord[j].Iout;
        }
    }
    else
    {
        Tier2.record[j].Iout = 0.0f;
    }
    // 8. �����ѹ
    Tier2.record[j].Vout     = Tier2.chRecord[j].Vout;
    // 9. ̽���ѹ
    Tier2.record[j].Vtz      = Tier2.chRecord[j].Vtz;
    //10. �������
    Tier2.record[j].Cbat     = Tier2.chRecord[j].Cbat;
    //11. �������
    Tier2.record[j].Ebat     = Tier2.chRecord[j].Ebat;
    //12. ģʽ��ʶ
    if(workstep_flag_CriticalZone[j] == 0)
    {
        Tier2.record[j].ModFlag = 0;
    }
    else if(workstep_flag_CriticalZone[j] == 1)
    {
        Tier2.record[j].ModFlag = 2;
    }
    else if(workstep_flag_CriticalZone[j] == 2)
    {
        Tier2.record[j].ModFlag = 3;
    }
    else if(workstep_flag_CriticalZone[j] == 3)
    {
        Tier2.record[j].ModFlag = 1;
    }
    //13. ��������
    if(Protect_Flag_Global[j] != 0)						// ȫ�ֱ���
    {
        SET_1(Tier2.record[j].ErrType, 0);
    }
    else
    {
        SET_0(Tier2.record[j].ErrType, 0);
    }
    if(Protect_Flag_Workstep[j] != 0)					// ����������
    {
        SET_1(Tier2.record[j].ErrType, 1);
    }
    else
    {
        SET_0(Tier2.record[j].ErrType, 1);
    }
    if(Protect_Flag_Internal[j] != 0)					// �ڲ�����
    {
        SET_1(Tier2.record[j].ErrType, 2);
    }
    else
    {
        SET_0(Tier2.record[j].ErrType, 2);
    }

    if((ACDC.Type == 1) || (ACDC.Type == 11))			// ����
    {
        if((ACDC.State[0] & 0x0FF8) || (ACDC.State[1] & 0x0FF8))	// ACDC����
        {
            SET_1(Tier2.record[j].ErrType, 3);
        }
        else
        {
            SET_0(Tier2.record[j].ErrType, 3);
        }
    }
    else if((ACDC.Type == 2) || (ACDC.Type == 22))		// �յ�
    {
        if((ACDC.State[0] & 0xBFF0) || (ACDC.State[1] & 0xBFF0))	// ACDC����
        {
            SET_1(Tier2.record[j].ErrType, 3);
        }
        else
        {
            SET_0(Tier2.record[j].ErrType, 3);
        }
    }
		else if(ACDC.Type == 3)		                       // ͼΪ
		{
				if((TW_HisAlam & 0xFF) || ((TW_HisStatus & 0x01)==0x0))	   // ACDC����
				{
						SET_1(Tier2.record[j].ErrType, 3);
				}
				else
				{
						SET_0(Tier2.record[j].ErrType, 3);
				}  
		}
		Tier2.record[j].AbortType = abort_type_flag[j];

		if(abort_type_flag[j] != 0)
		{
				abort_type_flag[j] = 0;
		}
}

/**********************************************************************************************************
**  �� �� �� : RecordInqAnalysis()
**  ����˵�� : ��¼���ݲ�ѯ��������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t RecordInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
//	uint8_t scmd = 0;			/* �������� */
//	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
//	uint8_t scmd_reply = 0;		/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_RECORD_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ���ܶ����ݽ��� */
    for(int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            RecordDatSync(j);
            memcpy(pSend + *lenSend, &Tier2.record[j], sizeof(Tier2.record[j]));
            *lenSend += sizeof(Tier2.record[j]);
        }
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : WorkstepSetAnalysis()
**  ����˵�� : �������ý�������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t WorkstepSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
//	uint8_t scmd = 0;			/* �������� */
//	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint8_t scmd_reply = 0;		/* ��������ظ� */

    uint16_t step_numb  = 0;	/* ������ */
    uint8_t  step_type  = 0;	/* �������� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_WORKSTEP_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ��ȡ�����ż��������� */
    memcpy(&step_numb, pRecv + lenAnalysis + 0, 2);		/* ��ȡ������ */
    memcpy(&step_type, pRecv + lenAnalysis + 2, 1);		/* ��ȡ�������� */

    /* �����źϷ����ж� */
    if((step_numb <= SCMD_WORKSTEP_NUM_BEGIN) || (step_numb >= SCMD_WORKSTEP_NUM_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 1;		/* �����Ų�ƥ��(����) */
        memcpy(pSend + *lenSend, &step_numb , 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    /* �������ͺϷ����ж� */
    if((step_type <= SCMD_WORKSTEP_TYPE_BEGIN) || (step_type >= SCMD_WORKSTEP_TYPE_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 2;		/* �������Ͳ�ƥ��(����) */
        memcpy(pSend + *lenSend, &step_numb , 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    if(step_type != SCMD_WORKSTEP_TYPE_FINISH)		/* �ǽ������� */
    {
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                memset(&Tier1.workstep[j], 0, sizeof(Tier1.workstep[j]));			/* ����һ�׽ṹ��(����1>ǰ���쳣����2>����֡�����֡��Ӱ��) */	/* Status����Ҫ���� */
                memcpy(&Tier1.workstep[j], pRecv + lenAnalysis, lenRecv - 12);		/* �����յĹ�����Ϣ�����һ�׽ṹ���� */

                uint8_t data_check = 0;			/* ����У�� */
                s_WorkStepSegmentStruct *p = &Tier1.workstep[j].RunSeg[0];

                /* ���й��������Ϸ����ж� */
                for(int i = 0; i < RUN_SEG_NUM; i++)		/* ���й������� */
                {
                    if( ((p + i)->type != SCMD_INVALID               ) && \
                            ( ((p + i)->type <= SCMD_WORKSTEP_PARAM_BEGIN) || \
                              ((p + i)->type >= SCMD_WORKSTEP_PARAM_END  ) ) )
                    {
                        result = FRAME_SCMD_ERR;
                        scmd_reply = 3;			/* �����趨���Ͳ�ƥ��(����) */
                        memcpy(pSend + *lenSend, &step_numb , 2);
                        *lenSend += 2;
                        memcpy(pSend + *lenSend, &scmd_reply, 1);
                        *lenSend += 1;
                        return result;
                    }
                    /* ������ŵ繤������,���в������������õ���ֵ */
                    else if( (step_type == SCMD_WORKSTEP_TYPE_CC ) || \
                             (step_type == SCMD_WORKSTEP_TYPE_DCC) )
                    {
                        if((p + i)->type == SCMD_WORKSTEP_PARAM_CURRENT)
                        {
                            data_check = 2;
                        }
                        else if((p + i)->type == SCMD_INVALID)
                        {
                            continue;
                        }
                        else
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3;		/* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb , 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
                    /* ��ѹ��ŵ繤������,���в������������õ���ֵ(��ѡ)����ѹֵ(��ѡ) */
                    else if( (step_type == SCMD_WORKSTEP_TYPE_CV ) || \
                             (step_type == SCMD_WORKSTEP_TYPE_DCV) )
                    {
                        if((p + i)->type == SCMD_WORKSTEP_PARAM_VOLTAGE)
                        {
                            data_check = 2;
                        }
                        else if((p + i)->type == SCMD_INVALID)
                        {
                            continue;
                        }
                        else if((p + i)->type != SCMD_WORKSTEP_PARAM_CURRENT)
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3;		/* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb , 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
                    /* ������ѹ��ŵ繤������,���в������������õ���ֵ(��ѡ)����ѹֵ(��ѡ) */
                    else if( (step_type == SCMD_WORKSTEP_TYPE_CCTOCV  ) || \
                             (step_type == SCMD_WORKSTEP_TYPE_DCCTODCV) )
                    {
                        if( ((p + i)->type == SCMD_WORKSTEP_PARAM_VOLTAGE) || \
                                ((p + i)->type == SCMD_WORKSTEP_PARAM_CURRENT) )
                        {
                            data_check++;
                        }
                        else if((p + i)->type == SCMD_INVALID)
                        {
                            continue;
                        }
                        else
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3;		/* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb , 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
                    /* ����ģʽ�������������в��� */
                    else if(step_type == SCMD_WORKSTEP_TYPE_STANDBY)
                    {
                        if((p + i)->type == SCMD_INVALID)
                        {
                            data_check = 2;
                        }
                        else
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3;			/* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb , 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
                }

                /* ���й��������Ϸ���У�� */
                if(data_check == 2)							/* ���й�������������ȷ */
                {
                    data_check = 0;
                }
                else /* if(data_check != 2) */				/* ���й����������ô��� */
                {
                    result = FRAME_SCMD_ERR;
                    scmd_reply = 3;			/* �����趨���Ͳ�ƥ��(����) */
                    memcpy(pSend + *lenSend, &step_numb , 2);
                    *lenSend += 2;
                    memcpy(pSend + *lenSend, &scmd_reply, 1);
                    *lenSend += 1;
                    return result;
                }

                /* ��ֹ���������Ϸ����ж� */
                for(int i = RUN_SEG_NUM; i < (RUN_SEG_NUM + ABT_SEG_NUM); i++)
                {
                    uint8_t abort_condition = (p + i)->type;
//					uint8_t abort_priority  = (abort_condition >> 4) & 0x0F;
                    uint8_t abort_type      = (abort_condition >> 0) & 0x0F;

                    /* ��Ч��ֹ���� */
                    if(abort_type == 0x0F)
                    {
                        continue;
                    }
                    /* ��ֹ���Ͳ��� */
                    if((abort_type <= SCMD_WORKSTEP_PARAM_BEGIN) || (abort_type >= SCMD_WORKSTEP_PARAM_END))
                    {
                        result = FRAME_SCMD_ERR;
                        scmd_reply = 3;				/* �����趨���Ͳ�ƥ��(����) */
                        memcpy(pSend + *lenSend, &step_numb , 2);
                        *lenSend += 2;
                        memcpy(pSend + *lenSend, &scmd_reply, 1);
                        *lenSend += 1;
                        return result;
                    }
                    /* ������ŵ������ʱ���ֹ����ѹ��ֹ��������ֹ */
                    if( (step_type == SCMD_WORKSTEP_TYPE_CC ) || \
                            (step_type == SCMD_WORKSTEP_TYPE_DCC) )
                    {
                        if( (abort_type == SCMD_WORKSTEP_PARAM_TIME    ) || \
                                (abort_type == SCMD_WORKSTEP_PARAM_VOLTAGE ) || \
                                (abort_type == SCMD_WORKSTEP_PARAM_CAPACITY) )
                        {
                            data_check = 2;
                        }
                        else
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3;			/* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb , 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
                    /* ��ѹ��ŵ������ʱ���ֹ��������ֹ��������ֹ */
                    else if( (step_type == SCMD_WORKSTEP_TYPE_CV ) || \
                             (step_type == SCMD_WORKSTEP_TYPE_DCV) )
                    {
                        if( (abort_type == SCMD_WORKSTEP_PARAM_TIME    ) || \
                                (abort_type == SCMD_WORKSTEP_PARAM_CURRENT ) || \
                                (abort_type == SCMD_WORKSTEP_PARAM_CAPACITY) )
                        {
                            data_check = 2;
                        }
                        else
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3;			/* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb , 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
                    /* ������ѹ��ŵ������ʱ���ֹ��������ֹ��������ֹ */
                    else if( (step_type == SCMD_WORKSTEP_TYPE_CCTOCV  ) || \
                             (step_type == SCMD_WORKSTEP_TYPE_DCCTODCV) )
                    {
                        if( (abort_type == SCMD_WORKSTEP_PARAM_TIME    ) || \
                                (abort_type == SCMD_WORKSTEP_PARAM_CURRENT ) || \
                                (abort_type == SCMD_WORKSTEP_PARAM_CAPACITY) )
                        {
                            data_check = 2;
                        }
                        else
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3;			/* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb , 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
                    /* ����ģʽ����������ʱ���ֹ */
                    else if(step_type == SCMD_WORKSTEP_TYPE_STANDBY)
                    {
                        if(abort_type == SCMD_WORKSTEP_PARAM_TIME)
                        {
                            data_check = 2;
                        }
                        else
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3;			/* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb , 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
                }

                /* ��ֹ���������Ϸ���У�� */
                if(data_check == 2)							/* ��ֹ��������������ȷ */
                {
                    data_check = 0;
                }
                else										/* ��ֹ�����������ô��� */
                {
                    result = FRAME_SCMD_ERR;
                    scmd_reply = 3;				/* �����趨���Ͳ�ƥ��(����) */
                    memcpy(pSend + *lenSend, &step_numb , 2);
                    *lenSend += 2;
                    memcpy(pSend + *lenSend, &scmd_reply, 1);
                    *lenSend += 1;
                    return result;
                }

                /* �������������Ϸ����ж� */
                for(int i = (RUN_SEG_NUM + ABT_SEG_NUM); i < (RUN_SEG_NUM + ABT_SEG_NUM + PRO_SEG_NUM); i++)
                {
                    if( !(((p + i)->type > SCMD_WORKSTEP_PROT_BEGIN ) && ((p + i)->type < SCMD_WORKSTEP_PROT_END )) && \
                            !(((p + i)->type > SCMD_WORKSTEP_NPROT_BEGIN) && ((p + i)->type < SCMD_WORKSTEP_NPROT_END)) )
                    {
                        if((p + i)->type != SCMD_INVALID)
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 4;		/* �����������Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb , 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
                }
            }
        }
    }
    else											/* ��������   */
    {
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                memset(&Tier1.workstep[j], 0, sizeof(Tier1.workstep[j]));			/* ����һ�׽ṹ��(����1>ǰ���쳣����2>����֡�����֡��Ӱ��) */	/* Status����Ҫ���� */
                memcpy(&Tier1.workstep[j], pRecv + lenAnalysis, lenRecv - 12);		/* �����յĹ�����Ϣ�����һ�׽ṹ���� */
            }
        }
    }
    memcpy(pSend + *lenSend, &step_numb , 2);
    *lenSend += 2;
    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

    if(result == 0)
    {
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            // �ϵ��й���ʱ������ϴδ��ڵȴ�״̬���쳣���磬�򲻽�����Ӧ�Ĺ�����
            if((Tier2.record[j].ErrType != 0) && (Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_WAIT) && (AcDumpConnectFlag == 1))
            {
                return result;
            }

            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                if(Tier2.workstepCache1[j].Numb == 0)								/* ��Cache1Ϊ�գ������չ����洢��Cache1 */
                {
                    if( (step_type == SCMD_WORKSTEP_TYPE_FINISH) && \
                            ((Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)) )
                    {
                        /* do nothing */
                    }
                    else
                    {
                        memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
                        memcpy(&Tier2.workstepCache1[j], &Tier1.workstep[j], sizeof(Tier1.workstep[j]));
                    }
                }
                else /* if(Tier2.workstepCache2[j].Numb == 0) */					/* ��Cache1�ǿգ������չ����洢��Cache2(���Cache2���й�����Ϣ��������) */
                {
                    memset(&Tier2.workstepCache2[j], 0, sizeof(Tier2.workstepCache2[j]));
                    memcpy(&Tier2.workstepCache2[j], &Tier1.workstep[j], sizeof(Tier1.workstep[j]));
                }
            }
        }
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : WorkstepRunstaSetAnalysis()
**  ����˵�� : ����״̬���ý�������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t WorkstepRunstaSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
//	uint8_t scmd = 0;			/* �������� */
//	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint8_t scmd_reply = 0;		/* ��������ظ� */

    uint16_t step_numb  = 0;	/* ������ */
    uint8_t  step_type  = 0;	/* �������� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_WORKSTEP_RUNSTA_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ��ȡ�����ż��������� */
    memcpy(&step_numb, pRecv + lenAnalysis + 0, 2);		/* ��ȡ������ */
    memcpy(&step_type, pRecv + lenAnalysis + 2, 1);		/* ��ȡ�������� */

    /* �����źϷ����ж�(����������תʹ�õ�) */
    if((step_numb <= SCMD_WORKSTEP_NUM_BEGIN) || (step_numb >= SCMD_WORKSTEP_NUM_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 1;		/* �����Ų�ƥ��(����) */
        memcpy(pSend + *lenSend, &step_numb , 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    /* �������ͺϷ����ж� */
    if(step_type >= WORKSTEP_RUNSTA_SET_END)
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 5;		/* �����������Ͳ�ƥ��(����) */
        memcpy(pSend + *lenSend, &step_numb , 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    /* �������Ͷ�Ӧ�߼� */
    switch(step_type)
    {
    case WORKSTEP_RUNSTA_SET_STOP:		// ֹͣ
    {
        for(int j = 0; j < CHANNEL_USE; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                refresh_continue[j] = 0;
				
                AcDumpConnectFlag = 0;
                Tier2.workstepControl[j].StepNumb = step_numb;
                Tier2.workstepControl[j].StepType = step_type;
                SET_1(workstep_flag_RunstaSet, j);
            }
        }
    }
    break;

    case WORKSTEP_RUNSTA_SET_PAUSE:		// ��ͣ
    {
        for(int j = 0; j < CHANNEL_USE; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                if( (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
                        (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) )
                {
                    refresh_continue[j] = 0;
					
                    Tier2.workstepControl[j].StepNumb = step_numb;
                    Tier2.workstepControl[j].StepType = step_type;
                    SET_1(workstep_flag_RunstaSet, j);
                }
            }
        }
    }
    break;

    case WORKSTEP_RUNSTA_SET_START:		// ����
    {
        for(int j = 0; j < CHANNEL_USE; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
//					/* ������ƥ���� */
//					if(step_numb != Tier2.workstepCache1[j].Numb)
//					{
//						result = FRAME_SCMD_ERR;
//						scmd_reply = 1;		/* ����������ƥ�� */
//						memcpy(pSend + *lenSend, &step_numb , 2);
//						*lenSend += 2;
//						memcpy(pSend + *lenSend, &scmd_reply, 1);
//						*lenSend += 1;
//						return result;
//					}
                if(((Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_STOP    ) || \
                        (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE) ) && \
                        (Tier2.workstepCache1[j].Numb != 0) && (Tier2.workstepCache1[j].Type > SCMD_WORKSTEP_TYPE_BEGIN) && (Tier2.workstepCache1[j].Type < SCMD_WORKSTEP_TYPE_FINISH) )
                {
//						if( (Protect_Flag_Global[j]   == 0) && \
//							(Protect_Flag_Internal[j] == 0) && \
//							(Protect_Flag_Workstep[j] == 0) && \
//							(1 /*ACDC�޹��� */ ) )
                    if(Tier2.record[j].ErrType == 0)
                    {
                        refresh_continue[j] = 0;

                        start_state_flag[j] = 1;
						
                        AcDumpConnectFlag = 0;
                        Tier2.workstepControl[j].StepNumb = step_numb;
                        Tier2.workstepControl[j].StepType = step_type;
                        SET_1(workstep_flag_RunstaSet, j);
                    }
                }
            }
        }
    }
    break;

    case WORKSTEP_RUNSTA_SET_CONTINUE:	// ����
    {
        for(int j = 0; j < CHANNEL_USE; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_PAUSE)
                {
//						if( (Protect_Flag_Global[j]   == 0) && \
//							(Protect_Flag_Internal[j] == 0) && \
//							(Protect_Flag_Workstep[j] == 0) && \
//							(1 /*ACDC�޹��� */ ) )
                    if(Tier2.record[j].ErrType == 0)
                    {
                        refresh_continue[j] = 0;
						
                        AcDumpConnectFlag = 0;
                        Tier2.workstepControl[j].StepNumb = step_numb;
                        Tier2.workstepControl[j].StepType = step_type;
                        SET_1(workstep_flag_RunstaSet, j);
                    }
                }
            }
        }
    }
    break;

    case WORKSTEP_RUNSTA_SET_SKIP:		// ��ת
    {
        for(int j = 0; j < CHANNEL_USE; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
//					if(step_numb != Tier2.workstepCache2[j].Numb)
//					{
//						result = FRAME_SCMD_ERR;
//						scmd_reply = 2;		/* ��ת������ƥ�� */
//						memcpy(pSend + *lenSend, &step_numb , 2);
//						*lenSend += 2;
//						memcpy(pSend + *lenSend, &scmd_reply, 1);
//						*lenSend += 1;
//						return result;
//					}
                if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
                {
//						if( (Protect_Flag_Global[j]   == 0) && \
//							(Protect_Flag_Internal[j] == 0) && \
//							(Protect_Flag_Workstep[j] == 0) && \
//							(1 /*ACDC�޹��� */ ) )
                    if(Tier2.record[j].ErrType == 0)
                    {
                        refresh_continue[j] = 0;
						
                        AcDumpConnectFlag = 0;
                        Tier2.workstepControl[j].StepNumb = step_numb;
                        Tier2.workstepControl[j].StepType = step_type;
                        SET_1(workstep_flag_RunstaSet, j);
                    }
                }
            }
        }
    }
    break;

    default:
    {
        for(int j = 0; j < CHANNEL_USE; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                result = FRAME_SCMD_ERR;
                scmd_reply = 3;		/* �޶�Ӧ������������ */
                memcpy(pSend + *lenSend, &step_numb , 2);
                *lenSend += 2;
                memcpy(pSend + *lenSend, &scmd_reply, 1);
                *lenSend += 1;
                return result;
            }
        }
    }
    break;
    }
    scmd_reply = 0;
    memcpy(pSend + *lenSend, &step_numb , 2);
    *lenSend += 2;
    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

    return result;
}

/**********************************************************************************************************
**  �� �� �� : WorkstepInqAnalysis()
**  ����˵�� : ������ѯ��������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t WorkstepInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
//	uint8_t scmd = 0;			/* �������� */
//	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint8_t scmd_reply = 0;		/* ��������ظ� */

    uint16_t step_numb  = 0;	/* ������ */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_WORKSTEP_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ��ȡ������ */
    memcpy(&step_numb, pRecv + lenAnalysis, 2);

    /* �����źϷ����ж� */
    if((step_numb <= SCMD_WORKSTEP_NUM_BEGIN) || (step_numb >= SCMD_WORKSTEP_NUM_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 0xFF;		/* ��ѯ������Խ��(����) */
        memcpy(pSend + *lenSend, &step_numb , 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    for(int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            if(step_numb == Tier2.workstepCache1[j].Numb)
            {
                memset(&Tier1.workstep[j], 0, sizeof(Tier1.workstep[j]));
                memcpy(&Tier1.workstep[j], &Tier2.workstepCache1[j], sizeof(Tier2.workstepCache1[j]));
            }
            else if(step_numb == Tier2.workstepCache2[j].Numb)
            {
                memset(&Tier1.workstep[j], 0, sizeof(Tier1.workstep[j]));
                memcpy(&Tier1.workstep[j], &Tier2.workstepCache2[j], sizeof(Tier2.workstepCache2[j]));
            }
            else		/* ������ѯ��ƥ�� */
            {
                SET_1(scmd_reply, j);
            }

            /* ��һͨ��������ѯ��ƥ�� */
            if((j == (CHANNEL_CFG - 1)) && (scmd_reply != 0))
            {
                result = FRAME_SCMD_ERR;
                memcpy(pSend + *lenSend, &step_numb , 2);
                *lenSend += 2;
                memcpy(pSend + *lenSend, &scmd_reply, 1);
                *lenSend += 1;
                return result;
            }
        }
    }

    for(int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            memcpy(pSend + *lenSend, &Tier1.workstep[j], sizeof(Tier1.workstep[j]));
            *lenSend += sizeof(Tier1.workstep[j]);
        }
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : CalibrateSetAnalysis()
**  ����˵�� : У׼�������ý�������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t CalibrateSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* �������� */
    uint8_t scmd_offset = 0;    /* ��������ƫ�� */
    float   sdat = 0.0f;        /* ������������ */
    uint8_t pcmd_reply = 0;     /* ��������ظ� */
    uint8_t scmd_reply = 0;     /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_CALIBRATE_PARAM_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);               /* ���·��ͻ������������� */
    *lenSend += 1;

    /* ���ö��׽ṹ����������һ�׽ṹ������(����ṹ�岿�����ݵ�д����Ӱ����������) */
    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            memset(&Tier1.calibrate[j], 0                  , sizeof(Tier1.calibrate[j]));
            memcpy(&Tier1.calibrate[j], &Tier2.calibrate[j], sizeof(Tier2.calibrate[j]));
        }
    }

    /* ���ܶ����ݽ��� */
    for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 5)
    {
        memcpy(&scmd, pRecv + i + 0, 1);    /* �������� */
        memcpy(&sdat, pRecv + i + 1, 4);    /* ������������ */

        /* ��������������Ϸ����ж� */
        if((scmd <= SCMD_CALIBRATE_PARAM_SET_BEGIN) || (scmd >= SCMD_CALIBRATE_PARAM_SET_END))
        {
            scmd_reply = 0xFF;
            memcpy(pSend + *lenSend, &scmd      , 1);
            *lenSend += 1;
            memcpy(pSend + *lenSend, &scmd_reply, 1);
            *lenSend += 1;
            if(scmd != SCMD_INVALID)    /* ����Ч�������� */    /* ��Ч��Ϊ���쳣�� */
            {
                result = FRAME_SCMD_ERR;                    /* ��������Ϊ�쳣�� */
            }
            continue;                                       /* �������벻ƥ��(��ת��һ��) */
        }

        /* ��������������ƫ�Ƽ��� */
        scmd_offset = (scmd - SCMD_CALIBRATE_PARAM_SET_BEGIN) - 1;
				scmd_reply = 0;
				
        /* �����������������ݴ��� */
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                float *sdat_addr = &Tier1.calibrate[j].VsenseSetK_C + scmd_offset;
                memcpy(sdat_addr, &sdat, 4);
				
								if(ee_WriteBytesWithCheck(pRecv + i + 1, CALIBRATE_BASEADDR(j) + 4 * scmd_offset, 4) != 1)
								{
									scmd_reply |= umask;
								}
            }
        }
//      scmd_reply = 0;
        memcpy(pSend + *lenSend, &scmd      , 1);
        *lenSend += 1;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
    }

    /* ����������� */
    if(result == 0)     /* ��ȷ���� */
    {
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                /* ���¶��׽ṹ������(�������������) */
                memset(&Tier2.calibrate[j], 0                  , sizeof(Tier2.calibrate[j]));
                memcpy(&Tier2.calibrate[j], &Tier1.calibrate[j], sizeof(Tier1.calibrate[j]));
//				EEP_DAT_SYNC(CALIBRATE_ID(j));
            }
        }
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : CalibrateInqAnalysis()
**  ����˵�� : У׼������ѯ��������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t CalibrateInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* �������� */
    uint8_t scmd_offset = 0;    /* ��������ƫ�� */
//  float   sdat = 0.0f;        /* ������������ */
    uint8_t pcmd_reply = 0;     /* ��������ظ� */
    float   scmd_reply = 0.0f;  /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_CALIBRATE_PARAM_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ���ܶ����ݽ��� */
    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            /* ����һ�׽ṹ������(���ڻظ�) */
            memcpy(&Tier1.calibrate[j], &Tier2.calibrate[j], sizeof(Tier2.calibrate[j]));

            for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 1)
            {
                memcpy(&scmd, pRecv + i, 1);

                /* ��������������Ϸ����ж� */
                if((scmd <= SCMD_CALIBRATE_PARAM_INQ_BEGIN) || (scmd >= SCMD_CALIBRATE_PARAM_INQ_END))
                {
                    scmd_reply = 0xFFFFFFFF;
                    memcpy(pSend + *lenSend, &scmd      , 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &scmd_reply, 4);
                    *lenSend += 4;
                    if(scmd != SCMD_INVALID)    /* ����Ч�������� */    /* ��Ч��Ϊ���쳣�� */
                    {
                        result = FRAME_SCMD_ERR;                    /* ��������Ϊ�쳣�� */
                    }
                    continue;                                       /* �������벻ƥ��(��ת��һ��) */
                }

				uint8_t dat_tmp_buff[4] = {0};
				
                /* ��������ƫ���� */
                scmd_offset = (scmd - SCMD_CALIBRATE_PARAM_INQ_BEGIN) - 1;
                /* ���������� */
				if(ee_ReadBytes(dat_tmp_buff, CALIBRATE_BASEADDR(j) + 4 * scmd_offset, 4) != 1)
				{
					scmd_reply = 0xFFFFFFFF;
				}
				else
				{
					memcpy(&Tier1.calibrate[j].VsenseSetK_C + scmd_offset, dat_tmp_buff, 4);
					scmd_reply = *(&Tier1.calibrate[j].VsenseSetK_C + scmd_offset);
				}

//				scmd_reply = *(&Tier1.calibrate[j].VsenseSetK_C + scmd_offset);
                /* �������� */
                memcpy(pSend + *lenSend, &scmd      , 1);
                *lenSend += 1;
                memcpy(pSend + *lenSend, &scmd_reply, 4);
                *lenSend += 4;
            }
        }
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : AdDataInqAnalysis()
**  ����˵�� : AD���ݲ�ѯ��������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t AdDataInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;			/* �������� */
    uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint32_t scmd_reply = 0.0f;	/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_AD_DATA_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 1)
            {
                memcpy(&scmd, pRecv + i, 1);

                /* ��������������Ϸ����ж� */
                if((scmd <= SCMD_AD_DATA_INQ_BEGIN) || (scmd >= SCMD_AD_DATA_INQ_END))
                {
                    scmd_reply = 0xFFFFFFFF;
                    memcpy(pSend + *lenSend, &scmd      , 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &scmd_reply, 4);
                    *lenSend += 4;
                    if(scmd != SCMD_INVALID)	/* ����Ч�������� */	/* ��Ч��Ϊ���쳣�� */
                    {
                        result = FRAME_SCMD_ERR;					/* ��������Ϊ�쳣�� */
                    }
                    continue;										/* �������벻ƥ��(��ת��һ��) */
                }

                /* ��������ƫ���� */
                scmd_offset = (scmd - SCMD_AD_DATA_INQ_BEGIN) - 1;
                /* ���������� */
                scmd_reply = *(&Tier2.adSum[j].VsenseAdSum + scmd_offset);
                /* �������� */
                memcpy(pSend + *lenSend, &scmd      , 1);
                *lenSend += 1;
                memcpy(pSend + *lenSend, &scmd_reply, 4);
                *lenSend += 4;
            }
        }
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : SystemInqAnalysis()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t SystemInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;			/* �������� */
//	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint8_t scmd_reply = 0;		/* ��������ظ� */

    uint8_t Read_En = 0;
    uint8_t Bar_Code[50] = {0};
    uint8_t Bar_Code_Len = 0;
    uint32_t Base_Add = 0; //EE �����磬 flash��ѯ��ַ
    uint8_t  Date_len = 0; //EE �����磬 flash��ѯ���ݳ���

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_SYSTEM_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ����������ظ����ص����ͻ����� */
    memcpy(&scmd, pRecv + lenAnalysis, 1);
    memcpy(pSend + *lenSend, &scmd, 1);
    *lenSend += 1;

    /* ��������������Ϸ����ж� */
    if((scmd <= SCMD_SYSTEM_INQ_BEGIN) || (scmd >= SCMD_SYSTEM_INQ_END))
    {
        result = FRAME_SCMD_ERR;		/* ��������Ϊ�쳣�� */
        scmd_reply = 0x01;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    switch(scmd)
    {
    case SCMD_SOFTWARE_VERSION:		/* ����汾��ѯ */
    {		
//          *(pSend + *lenSend) = SOFTWARE_PRIVERSION;
//          *lenSend += 1;
//          *(pSend + *lenSend) = SOFTWARE_SUBVERSION;
//          *lenSend += 1;
			
			float ttttt = SOFTWARE_PRIVERSION;
			 memcpy(pSend + *lenSend, &ttttt, 4);
			
//          *(pSend + *lenSend) = SOFTWARE_PRIVERSION;
            *lenSend += 4;
            *(pSend + *lenSend) = SOFTWARE_SUBVERSION;
            *lenSend += 1;	
    }
    break;

    case SCMD_WARMMING_MESSAGE:		/* �澯��Ϣ��ѯ */
    {
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                /* �澯���ϱ� */
            }
        }
    }
    break;

    case SCMD_SYSTEM_MODE:			/* ϵͳģʽ��ѯ */
    {
        *(pSend + *lenSend) = g_ucSysMode;
        *lenSend += 1;
    }
    break;

    case SCMD_SYSTEM_CLIENT:		/* �ͻ���Զ�����ò�ѯ */
    {
        memcpy(pSend + *lenSend, w5500_remote_ip              , 4) ;
        *lenSend += 4;
        memcpy(pSend + *lenSend, (uint8_t *)&w5500_remote_port, 2) ;
        *lenSend += 2;
    }
    break;

    case SCMD_SYSTEM_MAC_Q:     /* MAC���ò�ѯ */
    {
        memcpy(pSend + *lenSend, SN, 3) ;
        *lenSend += 3;
    }
    break;

    case SCMD_SYSTEM_BAR_Q:     /* �������ò�ѯ */
    {
        ee_ReadBytes(&Read_En, MacEn_flag_addr , 1);    //��EE
        if(Read_En == 1)
        {
            ee_ReadBytes(&Bar_Code_Len, BarCode_Len_flag_addr , 1);  //�����볤��
            if(Bar_Code_Len < 50)                       //�����ַ�����������50�ֽ�
            {
                ee_ReadBytes(Bar_Code, BarCode_End_flag_addr - Bar_Code_Len + 1, Bar_Code_Len);  //���ݳ��ȶ���������
                memcpy(pSend + *lenSend, Bar_Code, Bar_Code_Len) ;
                *lenSend += Bar_Code_Len;
            }
        }
    }
    break;

    case SCMD_SYSTEM_FRAM_Q:        /* �������ò�ѯ */
    {
        if(lenRecv != 0x12)
        {
            result = FRAME_DATA_ERR; //֡�������ݻ��ʽ����
        }
        else
        {
            lenAnalysis += 1;
            memcpy(&Base_Add, pRecv + lenAnalysis, 4);  //��ȡ����ַ
            if(Base_Add > MB85RS2_Size - 1)  //��ַ����Χ
            {
                result = FRAME_DATA_ERR; //֡�������ݻ��ʽ����
                break;
            }
            lenAnalysis += 4;
            memcpy(&Date_len, pRecv + lenAnalysis, 1);  //��ȡ��ȡ���ݳ���
            memcpy(pSend + *lenSend, &Date_len, 1) ;    //���ظ����ݳ��ȶ�
            *lenSend += 1;
            MB85Ctrl_Read(Base_Add, (uint8_t *)(pSend + *lenSend), Date_len ,0); //��ȡָ�����ݣ����ظ��������ݶ�
            *lenSend += Date_len;
        }
    }
    break;

    case SCMD_SYSTEM_EE_Q:         /* EE���ò�ѯ */
    {
        if(lenRecv != 0x12)
        {
            result = FRAME_DATA_ERR; //֡�������ݻ��ʽ����
        }
        else
        {
            lenAnalysis += 1;
            memcpy(&Base_Add, pRecv + lenAnalysis, 4);  //��ȡ����ַ
            if(Base_Add > Last_addr)  //��ַ����Χ
            {
                result = FRAME_DATA_ERR; //֡�������ݻ��ʽ����
                break;
            }
            lenAnalysis += 4;
            memcpy(&Date_len, pRecv + lenAnalysis, 1);  //��ȡ��ȡ���ݳ���
            memcpy(pSend + *lenSend, &Date_len, 1) ;    //���ظ����ݳ��ȶ�
            *lenSend += 1;
						int32_t ls, us;
						Dis_int();
						ls = osKernelLock(); 
            ee_ReadBytes((uint8_t *)(pSend + *lenSend), Base_Add, Date_len); //��ȡָ�����ݣ����ظ��������ݶ�
						us = osKernelUnlock();
						osKernelRestoreLock(us);
						osKernelRestoreLock(ls);
						bsp_DelayMS(1);
            *lenSend += Date_len;
        }
    }
    break;

//    case SCMD_SYSTEM_FLASH_Q:       /* FLASH���ò�ѯ */
//    {
//        if(lenRecv != 0x12)
//        {
//            result = FRAME_DATA_ERR; //֡�������ݻ��ʽ����
//        }
//        else
//        {
//            lenAnalysis += 1;
//            memcpy(&Base_Add, pRecv + lenAnalysis, 4);  //��ȡ����ַ
//            if(Base_Add > FLASH_SIZE - 1) //��ַ����Χ
//            {
//                result = FRAME_DATA_ERR; //֡�������ݻ��ʽ����
//                break;
//            }
//            lenAnalysis += 4;
//            memcpy(&Date_len, pRecv + lenAnalysis, 1);  //��ȡ��ȡ���ݳ���
//            memcpy(pSend + *lenSend, &Date_len, 1) ;    //���ظ����ݳ��ȶ�
//            *lenSend += 1;
//            SPI_Flash_Read((uint8_t *)(pSend + *lenSend), Base_Add, Date_len); //��ȡָ�����ݣ����ظ��������ݶ�
//            *lenSend += Date_len;
//        }
//    }
//    break;

    case SCMD_SYSTEM_PG_Q:         /* PG��״̬��ѯ */
    {
        *(uint8_t *)(pSend + *lenSend) = PG_state; //��ȡ��ȡ���ݳ���
        *lenSend += 1 ;

    }
    break;

    case SCMD_SYSTEM_AC_Q:         /* AC���ѹ��ѯ */
    {
        uint16_t timeout = 0;
        memset((void *)&ACDC.ACV[0], 0, 4); //���ACV����
        memset((void *)&ACDC.ACV[1], 0, 4);
        ACDC.QueryWaitFlag = QueryWait;
#ifdef YCJ    //Ԥ�����ACDCģ�飬�����ַ0���յµ�ַ1
        ACDC_ACV_Query_CMD(ACDC.Type, ACDC.Type - 1);
        while(ACDC.QueryWaitFlag != QueryACV1)    //�ȴ�ACDC�ظ�����ʱ200MS����
        {
            bsp_DelayMS(1);
            timeout++;
            if(timeout > 200)
            {
                break;
            }
        }
        memcpy(pSend + *lenSend, (const float *)&ACDC.ACV[0], 4) ;    //���ظ����ݳ��ȶ�
        *lenSend += 4;
#else
        if((ACDC.Type == 1) || (ACDC.Type == 11)) //����
        {
            ACDC_ACV_Query_CMD(1, 0); //��ѯģ��0��AC��ѹ
            while(ACDC.QueryWaitFlag != QueryACV1)    //�ȴ�ACDC�ظ�����ʱ200MS����
            {
                bsp_DelayMS(1);
                timeout++;
                if(timeout > 200)
                {
                    break;
                }
            }
            memcpy(pSend + *lenSend, (const float *)&ACDC.ACV[0], 4); //��ȡ��ѹ
            *lenSend += 4;

            timeout = 0;
            ACDC_ACV_Query_CMD(1, 1); //��ѯģ��1��AC��ѹ
            while(ACDC.QueryWaitFlag != QueryACV2)    //�ȴ�ACDC�ظ�����ʱ200MS����
            {
                bsp_DelayMS(1);
                timeout++;
                if(timeout > 200)
                {
                    break;
                }
            }
            memcpy(pSend + *lenSend, (const float *)&ACDC.ACV[1], 4); //��ȡ��ѹ
            *lenSend += 4;
        }
        else if((ACDC.Type == 2) || (ACDC.Type == 22)) //�յ�
        {
            ACDC_ACV_Query_CMD(2, 1); //��ѯģ��1��AC��ѹ
            while(ACDC.QueryWaitFlag != QueryACV1)    //�ȴ�ACDC�ظ�����ʱ200MS����
            {
                bsp_DelayMS(1);
                timeout++;
                if(timeout > 200)

                {
                    break;
                }
            }
            memcpy(pSend + *lenSend, (const float *)&ACDC.ACV[0], 4) ;    //���ظ����ݳ��ȶ�
            *lenSend += 4;
            timeout = 0;
            ACDC_ACV_Query_CMD(2, 2); //��ѯģ��2��AC��ѹ
            while(ACDC.QueryWaitFlag != QueryACV2)    //�ȴ�ACDC�ظ�����ʱ200MS����
            {
                bsp_DelayMS(1);
                timeout++;
                if(timeout > 200)
                {
                    break;
                }
            }
            memcpy(pSend + *lenSend, (const float *)&ACDC.ACV[1], 4) ;    //���ظ����ݳ��ȶ�
            *lenSend += 4;
        }
#endif

    }
    break;

    case SCMD_SYSTEM_ACDC_VERSION_Q:           /* ACDC�汾��ѯ */
    {
        uint16_t timeout = 0;
        memset((void *)&ACDC.VERSION[0][0], 0, 4); //���version����
        memset((void *)&ACDC.VERSION[1][0], 0, 4);
        ACDC.QueryWaitFlag = QueryWait;
#ifdef YCJ    //Ԥ�����ACDCģ�飬�����ַ0���յµ�ַ1
        ACDC_FW_VERSION_CMD(ACDC.Type, ACDC.Type - 1);
        while(ACDC.QueryWaitFlag != QueryVERSION1)    //�ȴ�ACDC�ظ�����ʱ200MS����
        {
            bsp_DelayMS(1);
            timeout++;
            if(timeout > 200)
            {
                break;
            }
        }
        memcpy(pSend + *lenSend, (const float *)&ACDC.VERSION[0][0], 4); //��ȡAC�̼��汾
        *lenSend += 4;
#else
        if((ACDC.Type == 1) || (ACDC.Type == 11)) //����
        {
            ACDC_FW_VERSION_CMD(1, 0); //��ѯģ��0�Ĺ̼��汾
            ACDC_FW_VERSION_CMD(1, 1); //��ѯģ��1�Ĺ̼��汾

        }
        else if((ACDC.Type == 2) || (ACDC.Type == 22)) //�յ�
        {
            ACDC_FW_VERSION_CMD(2, 1); //��ѯģ��1�Ĺ̼��汾
            while(ACDC.QueryWaitFlag != QueryVERSION1)    //�ȴ�ACDC�ظ�����ʱ200MS����
            {
                bsp_DelayMS(1);
                timeout++;
                if(timeout > 200)
                {
                    break;
                }
            }

            memcpy(pSend + *lenSend, (const float *)&ACDC.VERSION[0][0], 4); //��ȡAC�̼��汾
            *lenSend += 4;
            timeout = 0;

            ACDC_FW_VERSION_CMD(2, 2); //��ѯģ��2�Ĺ̼��汾
            while(ACDC.QueryWaitFlag != QueryVERSION2)    //�ȴ�ACDC�ظ�����ʱ200MS����
            {
                bsp_DelayMS(1);
                timeout++;
                if(timeout > 200)
                {
                    break;
                }
            }
            memcpy(pSend + *lenSend, (const float *)&ACDC.VERSION[1][0], 4); //��ȡAC�̼��汾
            *lenSend += 4;
        }
#endif
    }
    break;

    default
            :
        {
            /* do nothing */
        }
        break;
    }

    return result;
}

extern 	__IO uint32_t heart_beat_cnt;

/**********************************************************************************************************
**  �� �� �� : SystemSetAnalysis()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t SystemSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
		static uint8_t EnFeedCnt = 0;
	
    uint8_t result = 0;
    uint8_t scmd = 0;			/* �������� */
//	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
    uint8_t sdat = 0;			/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint8_t scmd_reply = 0;		/* ��������ظ� */

    uint8_t Read_En = 0;
    uint8_t Bar_Code[50] = {0};
    uint8_t Bar_Code_Len = 0;
    uint8_t Pyear, Pmonth; //��������
    uint16_t Pid; //��ˮ��
    uint16_t Pym; //���±���
    uint32_t Pmac = 0; //mac���(��3�ֽ���Ч)
    uint8_t err = 0;

    uint32_t Base_Add = 0; //flash,ee,�����������ݵ���ʼ��ַ
    uint8_t  date_len;     //flash,ee,����д�����ݳ���

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_SYSTEM_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ����������ظ����ص����ͻ����� */
    memcpy(&scmd, pRecv + lenAnalysis, 1);
    memcpy(pSend + *lenSend, &scmd, 1);
    *lenSend += 1;

    /* ��������������Ϸ����ж� */
    if( (scmd <= SCMD_SYSTEM_SET_BEGIN || scmd >= SCMD_SYSTEM_SET_END) && (scmd != 0x80) && (scmd != 0x82) )
    {
        result = FRAME_SCMD_ERR;		/* ��������Ϊ�쳣�� */
        scmd_reply = SCMD_NG;
    }
    else
    {
        switch(scmd)
        {
        case 0x01:		/* ϵͳ�澯��� */
        {
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                Protect_Flag_Global[j]   = 0;
                Protect_Flag_Internal[j] = 0;
                Protect_Flag_Workstep[j] = 0;
                workstep_flag_ErrWord[j] = 0;

                protect_flag_upcast[j] = 0;
            }

            for(int j = 0; j < CHANNEL_MAX; j++)
            {
                ChannelError[j] = 0;
            }
            ChannelErrorFlag = 0;
            ChannelStopFlag = 0;
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                g_ucLedChSta[j + 1] = CH_STANDBY;
            }
            Clear_AM_Flg = 1;  // ֪ͨ����澯
        }
        break;

        case 0x02:		/* ϵͳʱ���趨 */
        {
            memcpy(&Tstamp, pRecv + *lenSend, 8);
        }
        break;

        case 0x03:		/* ������ָ�� */
        {
						if(heart_beat_flag == 0x02)
						{
							Reconnection_Flg = 0x01;	//can������־
						}
						
            heart_beat_flag = 0;
						heart_beat_cnt = 0;
        }
        break;

        case 0x04:      /* ϵͳģʽ���� */
        {
            memcpy(&sdat, pRecv + *lenSend, 1);

            if(sdat == 0x01)            /* ��ͨģʽ */
            {
//                    if(((Tier2.workstepRunning[0].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.workstepRunning[0].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)) && \
//                            ((Tier2.workstepRunning[1].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.workstepRunning[1].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)) && \
//                           ((Tier2.workstepRunning[2].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.workstepRunning[2].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)) && \
//                            ((Tier2.workstepRunning[3].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.workstepRunning[3].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)))
#ifdef YCJ
                {
                    /* 1��3��5��7����ģʽ */
                    Chg_Dis_Port_Cfg(Channel_Port_Table[0], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[2], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[4], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[6], PORT_MODE_MASTER);
                    /* 2��4��6��8����ģʽ */
                    Chg_Dis_Port_Cfg(Channel_Port_Table[1], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[3], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[5], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[7], PORT_MODE_MASTER);
                    /* ��ͨģʽ */
                    g_ucSysMode = SYS_NORMAL_MODE;
                }
#else
                {
                    /* 1��3��5��7����ģʽ */
                    Chg_Dis_Port_Cfg(Channel_Port_Table[0], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[2], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[4], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[6], PORT_MODE_MASTER);
                    /* 2��4��6��8�ӻ�ģʽ */
                    Chg_Dis_Port_Cfg(Channel_Port_Table[1], PORT_MODE_SLAVE);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[3], PORT_MODE_SLAVE);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[5], PORT_MODE_SLAVE);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[7], PORT_MODE_SLAVE);
                    /* ��ͨģʽ */
                    g_ucSysMode = SYS_NORMAL_MODE;
                }
#endif
            }
            else if(sdat == 0x02)      /* У׼ģʽ */
            {
//                        if(((Tier2.workstepRunning[0].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.workstepRunning[0].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)) && \
//                                ((Tier2.workstepRunning[1].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.workstepRunning[1].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)) && \
//                               ((Tier2.workstepRunning[2].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.workstepRunning[2].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)) && \
//                               ((Tier2.workstepRunning[3].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.workstepRunning[3].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)))

#ifdef YCJ
                {
                    /* 1��3��5��7����ģʽ */
                    Chg_Dis_Port_Cfg(Channel_Port_Table[0], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[2], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[4], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[6], PORT_MODE_MASTER);
                    /* 2��4��6��8����ģʽ */
                    Chg_Dis_Port_Cfg(Channel_Port_Table[1], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[3], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[5], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[7], PORT_MODE_MASTER);
                    /* У׼ģʽ */
                    g_ucSysMode = SYS_CALIBRATION_MODE;
                }
#else
                {
                    /* 1��3��5��7����ģʽ */
                    Chg_Dis_Port_Cfg(Channel_Port_Table[0], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[2], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[4], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[6], PORT_MODE_MASTER);
                    /* 2��4��6��8����ģʽ */
                    Chg_Dis_Port_Cfg(Channel_Port_Table[1], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[3], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[5], PORT_MODE_MASTER);
                    Chg_Dis_Port_Cfg(Channel_Port_Table[7], PORT_MODE_MASTER);
                    /* У׼ģʽ */
                    g_ucSysMode = SYS_CALIBRATION_MODE;
                }
#endif
            }
            else                        /* ����ģʽ */
            {
                /* do nothing */
            }
        }
        break;

        case 0x05:		/* ʹ���������� */
        {
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                uint8_t umask = 1 << j;
                if(unpack.chEn & umask)
                {
                    SET_1(upcast_enable, j);
                }
            }
        }
        break;

        case 0x06:		/* ��ֹ�������� */
        {
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                uint8_t umask = 1 << j;
                if(unpack.chEn & umask)
                {
                    SET_0(upcast_enable, j);
                }
            }
        }
        break;

        case 0x07:		/* ���������������� */
        {
            float sdat_data_upcast_period = 0.0f;
            memcpy(&sdat_data_upcast_period, pRecv + lenAnalysis + 1, 4);
            data_upcast_period = (sdat_data_upcast_period != 0.0f) ? sdat_data_upcast_period : 1.0f;
        }
        break;

        case 0x08:		/* ϵͳ��ʼ�� */
        {
            /* do nothing */
        }
        break;

        case 0x09:		/* ϵͳ��ֹ */
        {
            /* do nothing */
        }
        break;

        case 0x0a:		/* ʹ�������� */
        {
            feed_hardware_watchdog_enable = 1;
        }
        break;

        case 0x0b:		/* ��ֹ������ */
        {
            feed_hardware_watchdog_enable = 0;
        }
        break;

        case 0x0c:		/* ̽��δѹ�� */
        {
            SystemStatus.Neilsbed = Neilsbed_Nofit;
        }
        break;

        case 0x0d:		/* ̽��ѹ���� */
        {
            SystemStatus.Neilsbed = Neilsbed_Fitting;
        }
        break;

        case 0x0e:		/* ̽����ѹ�� */
        {
            SystemStatus.Neilsbed = Neilsbed_Fitted;
        }
        break;

        case 0x0f:		/* ���ÿͻ���ģʽ��,Զ��IP�˿�ֵ,��������ģʽ��Ч */
        {
//////////            if(Net_Mod_State.Mode == 0)
            {
                Read_En = 1;
                memcpy(w5500_remote_ip, pRecv + lenAnalysis + 1, 4);
                memcpy(&w5500_remote_port, pRecv + lenAnalysis + 5, 2);
                ee_WriteBytesWithCheck(&Read_En, Remote_En_flag_addr, 1); //дEE
                ee_WriteBytesWithCheck(w5500_remote_ip, Remote_IPPort_flag_addr, 4);
                ee_WriteBytesWithCheck((uint8_t *)&w5500_remote_port, Remote_IPPort_flag_addr + 4, 2);
            }
        }
        break;

        case 0x10:		/* ��������(MAC��3�ֽ�) */
        {
//////////            if(Net_Mod_State.Mode == 0)
            {
                Read_En = 1;

                memcpy(&Bar_Code_Len, pRecv + lenAnalysis + 1, 1);
                if(Bar_Code_Len < 50)
                {
                    memcpy(Bar_Code, pRecv + lenAnalysis + 2, Bar_Code_Len);

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
                            Pmac = (Pym << 14) + Pid; //��MAC���
                            SN[0] = (Pmac >> 16) & 0xFF;
                            SN[1] = (Pmac >> 8) & 0xFF;
                            SN[2] = Pmac & 0xFF;
                            ee_WriteBytesWithCheck(&Read_En, MacEn_flag_addr, 1); //дEE
                            ee_WriteBytesWithCheck(SN, MacID_flag_addr, 3);
                            ee_WriteBytesWithCheck(&Bar_Code_Len, BarCode_Len_flag_addr, 1);
                            ee_WriteBytesWithCheck(Bar_Code, BarCode_End_flag_addr - Bar_Code_Len + 1, Bar_Code_Len);
                        }
                    }

                }
            }
        }
        break;

        case 0x11:      /* ������������ */
        {
            lenAnalysis += 1;
            memcpy(&Base_Add, pRecv + lenAnalysis, 4); //��ȡ���ݻ���ַ
            lenAnalysis += 4;
            memcpy(&date_len, pRecv + lenAnalysis, 1); //��ȡд�볤��
            lenAnalysis += 1;
            if(Base_Add + date_len > MB85RS2_Size)
            {
                result = FRAME_DATA_ERR; //֡�������ݻ��ʽ����
                break;
            }
            MB85Ctrl_Write(Base_Add, pRecv + lenAnalysis, date_len ,0); //��������д������
        }
        break;

        case 0x12:      /* ����EE���� */
        {
            lenAnalysis += 1;
            memcpy(&Base_Add, pRecv + lenAnalysis, 4); //��ȡ���ݻ���ַ
            lenAnalysis += 4;
            memcpy(&date_len, pRecv + lenAnalysis, 1); //��ȡд�볤��
            lenAnalysis += 1;
            if(Base_Add + date_len> Last_addr+1)
            {
                result = FRAME_DATA_ERR; //֡�������ݻ��ʽ����
                break;
            }
            ee_WriteBytesWithCheck(pRecv + lenAnalysis, Base_Add, date_len); //��������д��EE
        }
        break;

//        case 0x13:      /* ����FLASH���� */
//        {
//            lenAnalysis += 1;
//            memcpy(&Base_Add, pRecv + lenAnalysis, 4); //��ȡ���ݻ���ַ
//            lenAnalysis += 4;
//            memcpy(&date_len, pRecv + lenAnalysis, 1); //��ȡд�볤��
//            lenAnalysis += 1;
//            if(Base_Add + date_len > FLASH_SIZE ) //��ַ����Χ
//            {
//                result = FRAME_DATA_ERR; //֡�������ݻ��ʽ����
//                break;
//            }
//            SPI_Flash_WriteWithCheck(pRecv + lenAnalysis, Base_Add, date_len); //��������д��FLASH
//        }
//        break;
				
				case 0x15:	//��ֹι��
				{
					EnFeedCnt++;
					
					if(EnFeedCnt%2 == 0)
					{
						FeedWdgFlg = 0x00;
					}
					else 
					if(EnFeedCnt%2 == 1)
					{
						FeedWdgFlg = 0x01;
					}
				}
				break;
					
				case 0x80:		//��λ
				{
//					if( (*(pRecv + lenAnalysis + 1) == 0x19) && \
//						(*(pRecv + lenAnalysis + 2) == 0x91) && \
//						(*(pRecv + lenAnalysis + 3) == 0x05) && \
//						(*(pRecv + lenAnalysis + 4) == 0x21) )
						{
							NVIC_SystemReset();
						}				
				}
				break;		

				case 0x82:	//�ָ���������FCTģʽ
				{
					uint8_t jump_buf[2] = {0xFF,0xFF};
					MB85Ctrl_Write(JUMP_FCT_Flag_ADD,jump_buf,2,0);
					bsp_DelayMS(1);
					NVIC_SystemReset();
				}
				break;
		
        default:
				{
						/* do nothing */
				}
				break;
			}
			scmd_reply = SCMD_OK;
    }
    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

    return result;
}

/*
*********************************************************************************************************
*   �� �� ��: Erase_Update_Zone
*   ����˵��: ��������������
*   ��    ��: ��
*   �� �� ֵ: ����״̬
*********************************************************************************************************
*/
en_result_t Erase_Update_Zone(void)
{
    en_result_t Erase_Flag= Ok;
    en_result_t Stauts = Ok;
    uint8_t Retry=0;
    EFM_SectorCmd_Sequential(EFM_ADDR_SECTOR128, 112 ,Enable);         //�ر�FCT APP����(128~239)д����
    __set_PRIMASK(1);                                                  //���ж�
    for(uint8_t i=0; i<112 ; i++)
    {
        Retry=0;
        do {
            Retry++;
            //�����ж�
            Erase_Flag=EFM_SectorErase(EFM_ADDR_SECTOR128 + i*0x2000);     //�������ÿһ����������112��������ÿ��������С8K
            Feed_WDG();                                                    //ι��
        } while((Erase_Flag != Ok)&&(Retry<4));                            //ÿ��������ೢ��3��
        if(Retry==4)
        {
            Stauts=Error;
        }
    }
    __set_PRIMASK(0);                                                 //���ж�
    return Stauts;
}

/**********************************************************************************************************
**  �� �� �� : SystemUpdateAnalysis()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t SystemUpdateAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    int32_t ls, us;
    uint8_t result = 0;
    uint16_t Rx_Pack_NO=0;        //��¼���հ����
    uint8_t updatePack[1024]= {0}; //����д�뻺��
    uint8_t scmd = 0;			/* �������� */
//	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	uint8_t sdat = 0;			/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint8_t scmd_reply = 0;		/* ��������ظ� */
    uint8_t APP_Copy_Flag=0;
    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_SYSTEM_UPDATE;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ����������ظ����ص����ͻ����� */
    memcpy(&scmd, pRecv + lenAnalysis, 1);
    memcpy(pSend + *lenSend, &scmd, 1);
    *lenSend += 1;

    /* ��������������Ϸ����ж� */
    if((scmd <= SCMD_SYSTEM_UPDATE_BEGIN) || (scmd >= SCMD_SYSTEM_UPDATE_END))
    {
        result = FRAME_SCMD_ERR;		/* ��������Ϊ�쳣�� */
        scmd_reply = 0x01;
    }
    else
    {
        switch (scmd) {
        case 0x01:                /* ��תbootģʽ */
            if(System_UpdateFlg==0)  //���������в�����ִ��2��
            {
                System_UpdateFlg = 1;                    //ACDC����͸����־��λ����������
                ls = osKernelLock(); //����������
                osThreadSuspend(ThreadIdStart);          //start�������
                WRITE_REG8(M4_ADC3->STR, ADC_STR_STOP);  //��DMA����

                osTimerStop(timerID_Periodic_adc3);      //ֹͣ������
                osThreadSuspend(ThreadIdTaskProtection); //�����������
                osThreadSuspend(ThreadIdTaskDebug);      //��¼�������
                osThreadSuspend(ThreadIdTaskWorkstep);   //�����������

                osTimerDelete(ThreadIdStart);              //ɾ��start����
                osTimerDelete(timerID_Periodic_adc3);      //ɾ����ʱ����
                osThreadTerminate(ThreadIdTaskProtection); //ɾ����������
                osThreadTerminate(ThreadIdTaskDebug);      //ɾ����¼����
                osThreadTerminate(ThreadIdTaskWorkstep);   //ɾ����������
                TimerAUintStop(TMRA1_UNIT);
                NVIC_DisableIRQ(APP_TMRA1_IRQn);           //�رո澯�ж϶�ʱ��
                TimerAUintStop(TMRA3_UNIT);
                NVIC_DisableIRQ(APP_TMRA3_IRQn);           //�ر�����ʱ������ʱ��
                NVIC_DisableIRQ(APP_CAN2_IRQn);             //�ر�CAN�ж�
								TimerAUintStop(TMRA4_UNIT);
//                NVIC_DisableIRQ(AD7124_IRQn);              //�ر�7124�ж�
//                NVIC_DisableIRQ(AD7175_IRQn);              //�ر�7175�ж�
                NVIC_DisableIRQ(AD7606_Busy0_IRQn);              //�ر�7124�ж�
                NVIC_DisableIRQ(AD7606_IRQn);              //�ر�7175�ж�
								NVIC_DisableIRQ(SGM58601_IRQn);

                us = osKernelUnlock();
                osKernelRestoreLock(us);
                osKernelRestoreLock(ls);  //�ر�������
            }
            bsp_DelayMS(1);           //��ʱ1ms  
            
            scmd_reply = 0x01;
            break;

        case 0x02:                /*��ѯģʽ�������񣬹��жϣ��ظ�����*/
//            if(System_UpdateFlg==0)  //���������в�����ִ��2��
//            {
//                System_UpdateFlg = 1;                    //ACDC����͸����־��λ����������
//                ls = osKernelLock(); //����������
//                osThreadSuspend(ThreadIdStart);          //start�������
//                WRITE_REG8(M4_ADC3->STR, ADC_STR_STOP);  //��DMA����

//                osTimerStop(timerID_Periodic_adc3);      //ֹͣ������
//                osThreadSuspend(ThreadIdTaskProtection); //�����������
//                osThreadSuspend(ThreadIdTaskDebug);      //��¼�������
//                osThreadSuspend(ThreadIdTaskWorkstep);   //�����������

//                osTimerDelete(ThreadIdStart);              //ɾ��start����
//                osTimerDelete(timerID_Periodic_adc3);      //ɾ����ʱ����
//                osThreadTerminate(ThreadIdTaskProtection); //ɾ����������
//                osThreadTerminate(ThreadIdTaskDebug);      //ɾ����¼����
//                osThreadTerminate(ThreadIdTaskWorkstep);   //ɾ����������
//                TimerAUintStop(TMRA1_UNIT);
//                NVIC_DisableIRQ(APP_TMRA1_IRQn);           //�رո澯�ж϶�ʱ��
//                TimerAUintStop(TMRA3_UNIT);
//                NVIC_DisableIRQ(APP_TMRA3_IRQn);           //�ر�����ʱ������ʱ��
//                NVIC_DisableIRQ(APP_CAN_IRQn);             //�ر�CAN�ж�

//                NVIC_DisableIRQ(AD7124_IRQn);              //�ر�7124�ж� 20220102����
//                NVIC_DisableIRQ(AD7175_IRQn);              //�ر�7175�ж�


//                us = osKernelUnlock();
//                osKernelRestoreLock(us);
//                osKernelRestoreLock(ls);  //�ر�������
//            }
//            bsp_DelayMS(1);           //��ʱ1ms
if(System_UpdateFlg==1)
{
            scmd_reply = 0x01;
			can_update = 0;
}
else
{
			can_update = 1;
            scmd_reply = 0x05;
}
            break;

        case 0x03:                /*��������������*/

            EFM_Unlock();                    //������е�EFMд����

			EFM_FWMC_Unlock();               //���FWMC�Ĵ���д������FWMC�Ĵ��������ñ�̲���ģʽ������״̬��
		
            if(Ok==Erase_Update_Zone())      //��������������
            {
                scmd_reply = 0x01;           //�ظ�01��ʾ�����ɹ�
            }
            else
            {
                scmd_reply = 0x05;           //�ظ�05��ʾ����ʧ��
            }
            break;

        case 0x04:                   /*д���������ݣ�һ��1024�ֽ�*/
           if(lenRecv==1039)      //������ݳ����Ƿ�����������ݰ��̶�����9+1024+4
            {
                 Rx_Pack_NO=(pRecv[10]<<8)+pRecv[9]; //��¼���հ����
                if(Rx_Pack_NO == Pack_NO)
                {
                    memcpy(updatePack,&pRecv[11],1024);  //�����������ݵ�����
                    __set_PRIMASK(1);                    //���ж�
                    if(Ok== EFM_SequenceProgram(Write_ADDR,1024,(uint32_t *)&updatePack[0]))//д��1K����
                    {
                        Write_ADDR+=1024;                   //��ַ��1024
                        Pack_NO++;                          //����ż�1
                        scmd_reply = 0x01;           //�ظ�01��ʾд��ɹ�
                    } else
                    {
                        Write_ADDR = EFM_ADDR_SECTOR128;
                        Pack_NO=0;
                        scmd_reply = 0x05;            //�ظ�05��ʾд��ʧ��
                    }
                    __set_PRIMASK(0);                         //���ж�
                } else
                {
                    scmd_reply = 0x04;             //�ظ�04��ʾ����Ŵ���

                }

            }
            break;
        case 0x05:                /*��ʾ����������д������־����תBOOT*/
			
            EFM_SectorCmd_Sequential(EFM_ADDR_SECTOR128, 112 ,Disable);         //FCT APP����(128~239)д����				
			EFM_FWMC_Lock();               //���FWMC�Ĵ���д������FWMC�Ĵ��������ñ�̲���ģʽ������״̬��	
		
            EFM_Lock();                                            //���е�EFMд����        
            MB85Ctrl_Read(APP_Copy_Flag_ADD,&APP_Copy_Flag,1 ,0);     //��ȡAPP������־
            while(APP_Copy_Flag !=0xCC)
            {
                APP_Copy_Flag=0xCC;
                MB85Ctrl_Write(APP_Copy_Flag_ADD,&APP_Copy_Flag,1 ,0);    //дAPP������־
                APP_Copy_Flag=0;
                MB85Ctrl_Read(APP_Copy_Flag_ADD,&APP_Copy_Flag,1 ,0);     //��ȡAPP������־
                if(APP_Copy_Flag==0xCC) 
                {
                 scmd_reply = 0x01; 
                 memcpy(pSend + *lenSend, &scmd_reply, 1);
                 *lenSend += 1;              
                 W5500_send(0,pSend,*lenSend);                         //�ظ�����
                 
//				NVIC_DisableIRQ(APP_TMRA2_IRQn);           //�ر�����ʱ������ʱ��	
					
                NVIC_SystemReset();                                    //����������־ȷ�ϣ�ϵͳ����
                }
            }
            break;
        }
    }
    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

    return result;
}

/**********************************************************************************************************
**  �� �� �� : ACDCUpdateAnalysis()
**  ����˵�� : ACDC���������������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t ACDCUpdateAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint32_t ID = 0;
    uint8_t  Data[8] = {0};
    uint16_t TimOutCNT = 0;

    uint8_t result = 0;
    uint8_t scmd = 0;			/* �������� */
//	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint8_t   scmd_reply = 0;	/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_ACDC_UPDATE_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ���µ�ǰ�������������� */
    memcpy(&scmd, pRecv + lenAnalysis, 1);
    /* ��������������Ϸ����ж� */
    if((scmd < SCMD_ACDC_UPDATE_SET_BEGIN) || (scmd > SCMD_ACDC_UPDATE_SET_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 0x01;
        memcpy(pSend + *lenSend, &scmd      , 1);
        *lenSend += 1;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    switch(scmd)
    {
        case 0x01:  /* ��������-CAN͸��ģʽ */
        {
            int32_t ls, us;

            if(ACDC_UpdateFlg == 0) //���������в�����ִ��2��
            {
                ACDC_UpdateFlg = 1;                    //ACDC����͸����־��λ����������
                bsp_DelayMS(1);           //��ʱ1ms
                ls = osKernelLock(); //����������
                osThreadSuspend(ThreadIdStart);       //start�������
                WRITE_REG8(M4_ADC3->STR, ADC_STR_STOP);  //��DMA����
                osTimerStop(timerID_Periodic_adc3);      //ֹͣ������
                osThreadSuspend(ThreadIdTaskProtection); //�����������
                osThreadSuspend(ThreadIdTaskDebug);      //��¼�������
                osThreadSuspend(ThreadIdTaskWorkstep);   //�����������

//            osTimerDelete(ThreadIdStart);              //ɾ��start����
//            osTimerDelete(timerID_Periodic_adc3);      //ɾ����ʱ����
//            osThreadTerminate(ThreadIdTaskProtection); //ɾ����������
//            osThreadTerminate(ThreadIdTaskDebug);      //ɾ����¼����
//            osThreadTerminate(ThreadIdTaskWorkstep);   //ɾ����������


                TimerAUintStop(TMRA1_UNIT);
                NVIC_DisableIRQ(APP_TMRA1_IRQn);           //�رո澯�ж϶�ʱ��
                TimerAUintStop(TMRA3_UNIT);
                NVIC_DisableIRQ(APP_TMRA3_IRQn);           //�ر�����ʱ������ʱ��                

//                NVIC_DisableIRQ(AD7124_IRQn);              //�ر�7124�ж�
//                NVIC_DisableIRQ(AD7175_IRQn);              //�ر�7175�ж�
                NVIC_DisableIRQ(AD7606_Busy0_IRQn);        //�ر�7124�ж�
                NVIC_DisableIRQ(AD7606_IRQn);              //�ر�7175�ж�
								NVIC_DisableIRQ(SGM58601_IRQn);


                us = osKernelUnlock();
                osKernelRestoreLock(us);
                osKernelRestoreLock(ls);  //�ر�������
            }


            bsp_DelayMS(1);           //��ʱ1ms

            memcpy(pSend + *lenSend, &scmd, 1);
            *lenSend += 1;
        }
        break;

        case 0x02:  /* ͸������ */
        {
            if((ACDC_UpdateFlg == 1) && ((ACDC.Type == 2) || (ACDC.Type == 22))) //�յ�ģ������
            {
                //              memcpy(&ID, (const void *)(pRecv + lenAnalysis + 1), 4);                        //�������ID��С��ģʽ���ͣ�����С��ת��
                ID = (*(pRecv + lenAnalysis + 1) << 24) + (*(pRecv + lenAnalysis + 2) << 16) + \
                     (*(pRecv + lenAnalysis + 3) << 8) + (*(pRecv + lenAnalysis + 4) << 0);     //�������ID����˷��ͣ���С��ת��
                memcpy(Data, pRecv + lenAnalysis + 5 , 8);  //�����������ݲ���
                CAN2_SendData(ID, Data, 8);         //ת��can

                /*�ȴ�can�������ݣ�������*/
                if((!(((ID >> 24) & 0xFF) == 0x1c && ((ID >> 16) & 0xF0) == 0xb0)) && \
                        (!(((ID >> 24) & 0xFF) == 0x1c && ((ID >> 16) & 0xF0) == 0xa0)) && \
                        (!(((ID >> 24) & 0xFF) == 0x1c && ((ID >> 16) & 0xF0) == 0xd0)))            //can���ݱ� ID��0x1c 0xbx��ͷ��ACDCģ�鲻�ظ�,����ȴ�
                {
#if 1
                    w5500_Silence = 0;
                    while(!CanRet.processedFlg)
                    {
                        __nop();
                    }                       //У��������������ȴ��ظ�ȷ��

                    memcpy(pSend + *lenSend, &scmd       , 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &CanRet.ID  , 4);
                    *lenSend += 4;
                    memcpy(pSend + *lenSend, &CanRet.Date, 8);
                    *lenSend += 8;
                    CanRet.processedFlg = 0;        //�������־����

#else
                    w5500_Silence = 0;
                    while(TimOutCNT < 4000)        //У��������������ȴ��ظ�ȷ�ϣ����ȴ�4��
                    {
                        TimOutCNT++;
                        bsp_DelayMS(1);
                        if(CanRet.processedFlg)
                        {
                            break;
                        }
                    }
                    if(TimOutCNT < 4000)
                    {
                        memcpy(pSend + *lenSend, &scmd       , 1);
                        *lenSend += 1;
                        memcpy(pSend + *lenSend, &CanRet.ID  , 4);
                        *lenSend += 4;
                        memcpy(pSend + *lenSend, &CanRet.Date, 8);
                        *lenSend += 8;
                        CanRet.processedFlg = 0;        //�������־����
                    }
                    else
                    {
                        w5500_Silence = 1;
                    }
#endif
                }
                else
                    if(((ID >> 24) & 0xFF) == 0x1c && ((ID >> 16) & 0xF0) == 0xa0)
                    {
                        w5500_Silence = 0;
                        while(TimOutCNT < 8000)        //���ְ���Ҫ�ȴ��ظ�ȷ��
                        {
                            TimOutCNT++;
                            bsp_DelayMS(1);
                            if(CanRet.processedFlg)
                            {
                                break;
                            }
                        }
                        if(TimOutCNT < 8000)
                        {
                            memcpy(pSend + *lenSend, &scmd       , 1);
                            *lenSend += 1;
                            memcpy(pSend + *lenSend, &CanRet.ID  , 4);
                            *lenSend += 4;
                            memcpy(pSend + *lenSend, &CanRet.Date, 8);
                            *lenSend += 8;
                            CanRet.processedFlg = 0;    //�������־����
                        }

                        else
                        {
                            w5500_Silence = 1;
                        }
                    }
                    else
                        if(((ID >> 24) & 0xFF) == 0x1c && ((ID >> 16) & 0xF0) == 0xd0)
                        {
                            w5500_Silence = 0;
                            while(TimOutCNT < 3000)        //������������Ҫ�ȴ��ظ�ȷ��
                            {
                                TimOutCNT++;
                                bsp_DelayMS(1);
                                if(CanRet.processedFlg)
                                {
                                    break;
                                }
                            }
                            if(TimOutCNT < 3000)
                            {
                                memcpy(pSend + *lenSend, &scmd       , 1);
                                *lenSend += 1;
                                memcpy(pSend + *lenSend, &CanRet.ID  , 4);
                                *lenSend += 4;
                                memcpy(pSend + *lenSend, &CanRet.Date, 8);
                                *lenSend += 8;
                                CanRet.processedFlg = 0;    //�������־����
                            }

                            else
                            {
                                w5500_Silence = 1;
                            }
                        }
                        else
                        {
                            w5500_Silence = 1;
                        }
            }

            if((ACDC_UpdateFlg == 1) && ((ACDC.Type == 1) || (ACDC.Type == 11))) //����ģ������
            {
                /*��������ֱ��ת��can��*/
                ID = (*(pRecv + lenAnalysis + 1) << 24) + (*(pRecv + lenAnalysis + 2) << 16) + \
                     (*(pRecv + lenAnalysis + 3) << 8) + (*(pRecv + lenAnalysis + 4) << 0);     //�������ID����˷��ͣ���С��ת��
                memcpy(Data, pRecv + lenAnalysis + 5 , 8);  //�����������ݲ���
                CAN2_SendData( ID, Data, 8);         //ת��can

            /*�ȴ�can�������ݣ�������*/
            if(((ID >> 24) & 0xFF) == 0x10 && ((ID >> 16) & 0xFF) == 0x51)            //can���ݱ� ID��0x10 0x51��ͷ�������ְ�
            {
                w5500_Silence = 0;
                while(TimOutCNT < 10000)        //���ְ���Ҫ�ȴ��ظ�ȷ��,���ȴ�1��
                {
                    TimOutCNT++;
                    bsp_DelayUS(100);
                    if(CanRet.processedFlg)     //can�յ��ظ�
                    {
                        break;
                    }
                }
                if(TimOutCNT < 10000)      // 1�����յ����ֻظ���������ת��������
                {
                    memcpy(pSend + *lenSend, &scmd       , 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &CanRet.ID  , 4);
                    *lenSend += 4;
                    memcpy(pSend + *lenSend, &CanRet.Date, 8);
                    *lenSend += 8;
                    CanRet.processedFlg = 0;    //�������־����
                }
                else
                {
                    w5500_Silence = 1; //���ھ�Ĭ
                }
            }
            else if(((ID >> 24) & 0xFF) == 0x10 && ((ID >> 16) & 0xFF) == 0x52)           //can���ݱ� ID��0x10 0x52��ͷ���ǲ�����
            {
                w5500_Silence = 0;
                while(TimOutCNT < 20000)        //��������Ҫ�ȴ��ظ�ȷ��,���ȴ�20��
                {
                    TimOutCNT++;
                    bsp_DelayUS(1000);
                    if(CanRet.processedFlg)     //can�յ��ظ�
                    {
                        break;
                    }
                }
                if(TimOutCNT < 20000)      // 20�����յ������ظ���������ת��������
                {
                    memcpy(pSend + *lenSend, &scmd       , 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &CanRet.ID  , 4);
                    *lenSend += 4;
                    memcpy(pSend + *lenSend, &CanRet.Date, 8);
                    *lenSend += 8;
                    CanRet.processedFlg = 0;    //�������־����
                }
                else
                {
                    w5500_Silence = 1; //���ھ�Ĭ
                }
            }
            else if(((ID >> 24) & 0xFF) == 0x10 && ((ID >> 16) & 0xFF) == 0x55)           //can���ݱ� ID��0x10 0x55��ͷ����Check��
            {
                w5500_Silence = 0;
                while(TimOutCNT < 15000)        //Check����Ҫ�ȴ��ظ�ȷ��,���ȴ�15��
                {
                    TimOutCNT++;
                    bsp_DelayUS(1000);
                    if(CanRet.processedFlg)     //can�յ��ظ�
                    {
                        break;
                    }
                }
                if(TimOutCNT < 15000)      // 15�����յ�Check�ظ���������ת��������
                {
                    memcpy(pSend + *lenSend, &scmd       , 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &CanRet.ID  , 4);
                    *lenSend += 4;
                    memcpy(pSend + *lenSend, &CanRet.Date, 8);
                    *lenSend += 8;
                    CanRet.processedFlg = 0;    //�������־����
                }
                else
                {
                    w5500_Silence = 1; //���ھ�Ĭ
                }
            }
            else if(((ID >> 24) & 0xFF) == 0x10 && ((ID >> 16) & 0xFF) == 0x56)           //can���ݱ� ID��0x10 0x56��ͷ����������ɰ�
            {
                w5500_Silence = 0;
                while(TimOutCNT < 15000)        //������ɰ���Ҫ�ȴ��ظ�ȷ��,���ȴ�15��
                {
                    TimOutCNT++;
                    bsp_DelayUS(1000);
                    if(CanRet.processedFlg)     //can�յ��ظ�
                    {
                        break;
                    }
                }
                if(TimOutCNT < 15000)      // 15�����յ�������ɻظ���������ת��������
                {
                    memcpy(pSend + *lenSend, &scmd       , 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &CanRet.ID  , 4);
                    *lenSend += 4;
                    memcpy(pSend + *lenSend, &CanRet.Date, 8);
                    *lenSend += 8;
                    CanRet.processedFlg = 0;	//�������־����
                }
                else
                {
                    w5500_Silence = 1;
                }
            }
            else
            {
                w5500_Silence = 1;
            }
        }
    }
    break;

    case 0x03:	/*�˳�͸��ģʽ*/
    {
        ACDC_UpdateFlg = 2;      //ACDC�������
        w5500_Silence = 0;       //������ھ�Ĭ
        memcpy(pSend + *lenSend, &scmd, 1);
        *lenSend += 1;
//			HAL_NVIC_EnableIRQ(TIM3_IRQn);
    }
    break;

    default:
    {
        /* do nothing */
    }
    break;
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : DebugParamSetAnalysis()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t DebugParamSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
//	uint8_t scmd = 0;			/* �������� */
//	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint8_t scmd_reply = 0;		/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_DEBUG_PARAM_SET;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* �����Բ�������ֵ�浽�ṹ����� */
    memcpy(&Tier2.debug.set, pRecv + lenAnalysis, sizeof(Tier2.debug.set));

    /* ���ò����Ϸ���У�� */
    if((Tier2.debug.set.Vtemp <= 7.0f) && (Tier2.debug.set.Itemp <= ITEMP_MAX)) //2022.02.14
    {
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                if(Tier2.debug.set.PortEnable == 0x00)          /* �ر���� */
                {
//                    if( ((debug_ch_onoff_state >> (2 * j)) & 0x03) != 3)
                    	if( ((debug_ch_onoff_state >> (2 * j)) & 0x03) == 0)
                    {
                        debug_ch_onoff_state &= ~(3 << (2 * j));
                        debug_ch_onoff_state |= (1 << (2 * j));
                    }
					
//									Chg_Dis_Off_Cfg(1 << j);					
#ifdef YCJ
                    g_ucLedChSta[j  + 1] = CH_COMPLETE;
#else
                    g_ucLedChSta[j / 2 + 1] = CH_COMPLETE;
#endif
                }
                else if(Tier2.debug.set.PortEnable == 0x03)		/* ����� */
                {
//                    if( ((debug_ch_onoff_state >> (2 * j)) & 0x03) != 3)
                    if( ((debug_ch_onoff_state >> (2 * j)) & 0x03) == 0)
                    {
                        debug_ch_onoff_state &= ~(3 << (2 * j));
                        debug_ch_onoff_state |= (2 << (2 * j));
                    }
					
//					Chg_Dis_On_Cfg(1 << j, Tier2.debug.set.PortDir, Mod_V_Range_Flag[j], Tier2.debug.set.Vtemp, Tier2.debug.set.Itemp);
                }
            }
        }

        if((unpack.chEn == 0xFF) && (Tier2.debug.set.PortEnable == 0x00))
        {
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                Protect_Flag_Global[j]   = 0;
                Protect_Flag_Internal[j] = 0;
                Protect_Flag_Workstep[j] = 0;
                workstep_flag_ErrWord[j] = 0;

                protect_flag_upcast[j] = 0;
            }

            for(int j = 0; j < CHANNEL_MAX; j++)
            {
                ChannelError[j] = 0;
            }
            ChannelErrorFlag = 0;
            ChannelStopFlag = 0;
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                g_ucLedChSta[j + 1] = CH_STANDBY;
            }
            Clear_AM_Flg = 1;  // ֪ͨ����澯
        }

        scmd_reply = SCMD_OK;			/* ���·��ͻ������ظ�����(ACK) */
    }
    else																		/* ���ò����Ƿ� */
    {
        result     = FRAME_DATA_ERR;
        scmd_reply = SCMD_NG;			/* ���·��ͻ������ظ�����(NAK) */
    }

    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

    return result;
}

/**********************************************************************************************************
**  �� �� �� : DebugDataInqAnalysis()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t DebugDataInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
//	uint8_t scmd = 0;			/* �������� */
//	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
//	float   sdat = 0.0f;		/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
//	uint8_t scmd_reply = 0;		/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_DEBUG_DATA_INQ;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    if(unpack.chEn == 0x00)	/* ��ͨ����ѯ */
    {
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            memcpy(pSend + *lenSend, &Tier2.debug.inq1[j], sizeof(Tier2.debug.inq1[j]));
            *lenSend += sizeof(Tier2.debug.inq1[j]);
        }
        memcpy(pSend + *lenSend, &Tier2.debug.inq2, sizeof(Tier2.debug.inq2));
        *lenSend += sizeof(Tier2.debug.inq2);
    }
    else					/* ��ͨ����ѯ */
    {
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask =  1 << j;

            if(unpack.chEn & umask)
            {
                memcpy(pSend + *lenSend, &Tier2.debug.inq1[j], sizeof(Tier2.debug.inq1[j]));
                *lenSend += sizeof(Tier2.debug.inq1[j]);
            }
        }
        memcpy(pSend + *lenSend, &Tier2.debug.inq2, sizeof(Tier2.debug.inq2));
        *lenSend += sizeof(Tier2.debug.inq2);
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : DebugSystemSetAnalysis()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t DebugSystemSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;			/* �������� */
//	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
    uint8_t sdat = 0;			/* ������������ */
    uint8_t pcmd_reply = 0;		/* ��������ظ� */
    uint8_t scmd_reply = 0;		/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_DEBUG_SYSTEM_SET;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ��ȡ�������뼰������������ */
    memcpy(&scmd, pRecv + lenAnalysis + 0, 1);
    memcpy(&sdat, pRecv + lenAnalysis + 1, 1);

    if((scmd >= SCMD_DEBUG_SYSTEM_SET_BEGIN) && (scmd <= SCMD_DEBUG_SYSTEM_SET_END))
    {
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 0x01 << j;
            if(sdat & umask)
            {
                if(scmd == 0x01)
                {
                    Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_RANGE_MOD);
                }
                else if(scmd == 0x02)
                {
                    /* do nothing */
                }
                else if(scmd == 0x03)
                {
                    Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_MODE_SLAVE);
                }
            }
            else
            {
                if(scmd == 0x01)
                {
                    Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_RANGE_SENSE);
                }
                else if(scmd == 0x02)
                {
                    /* do nothing */
                }
                else if(scmd == 0x03)
                {
									if(sdat == 0x00)
									{
										GPIO_ResetPins(GPIO_PORT_CVK_4,GPIO_PIN_CVK_4);
										GPIO_ResetPins(GPIO_PORT_CVK_8,GPIO_PIN_CVK_8);
										
										ParaCHxConfig = 0x01;				

										Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_MODE_MASTER);
									}
									else if (sdat == 0xAA)	//2ͨ������
									{
//										GPIO_SetPins(GPIO_PORT_CVK_4,GPIO_PIN_CVK_4);
//										GPIO_SetPins(GPIO_PORT_CVK_8,GPIO_PIN_CVK_8);
										
										ParaCHxConfig = 0x02;
									}
									else if(sdat == 0xEE)	//4ͨ������
									{
//										GPIO_ResetPins(GPIO_PORT_CVK_4,GPIO_PIN_CVK_4);
//										GPIO_SetPins(GPIO_PORT_CVK_8,GPIO_PIN_CVK_8);
										
										ParaCHxConfig = 0x04;
									}								
									else if(sdat == 0xFE)	//8ͨ������
									{
//										GPIO_ResetPins(GPIO_PORT_CVK_4,GPIO_PIN_CVK_4);
//										GPIO_ResetPins(GPIO_PORT_CVK_8,GPIO_PIN_CVK_8);
										
										ParaCHxConfig = 0x08;
									}
									
//                  Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_MODE_MASTER);
                }
            }
        }
        scmd_reply = SCMD_OK;			/* ���·��ͻ�����������������(ACK) */
    }
    else
    {
        result     = FRAME_SCMD_ERR;
        scmd_reply = SCMD_NG;			/* ���·��ͻ�����������������(NAK) */
    }

    memcpy(pSend + *lenSend, &scmd      , 1);
    *lenSend += 1;
    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

    return result;
}
/**********************************************************************************************************
**  �� �� �� : BootUpdateAnalysis()
**  ����˵�� : BOOT���������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t BootUpdateAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* �������� */
    uint8_t pcmd_reply = 0;     /* ��������ظ� */
    uint8_t scmd_reply = 0;     /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_BOOT_UPDATE + 0x80;      //��������ظ�
    memcpy(pSend + *lenSend, &pcmd_reply, 1);  //�����ظ��ֽڣ��ظ�ָ���ַ+1
    *lenSend += 1;

    /* ��ȡ�������� */
    memcpy(&scmd, pRecv + lenAnalysis + 0, 1);
    /* ��������������Ϸ����ж� */
    if((scmd < CMD_BOOT_UPDATE_ENTER) || (scmd > CMD_BOOT_UPDATE_EXIT))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 0x01;
        memcpy(pSend + *lenSend, &scmd      , 1);
        *lenSend += 1;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    switch(scmd)
    {
    case CMD_BOOT_UPDATE_ENTER:
        break;
    case CMD_BOOT_UPDATE_EARSE:
        break;
    case CMD_BOOT_UPDATE_WRITE:
        break;
    case CMD_BOOT_UPDATE_EXIT:
        break;
    default
            :
        break;

    }
    return result;
}
/**********************************************************************************************************
**  �� �� �� : DefaultAnalyze()
**  ����˵�� : δ�����������봦����
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t DefaultAnalyze(uint8_t *pSend, uint16_t *lenSend, uint8_t errCmd)
{
    memcpy(pSend + *lenSend, &errCmd, 1);

    *lenSend += 1;

    return FRAME_PCMD_ERR;
}

/**********************************************************************************************************
**  �� �� �� : SendPack()
**  ����˵�� : ���ʹ������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
static void SendPack(uint8_t *pSend, uint16_t *lenSend)
{
    if(ucProtocolVersion == 0)			/* �ɰ汾Э�� */
    {
        pack.head = FRAME_HEAD_SEND_OLD;			/* ֡ͷ */
        pack.len  = *lenSend;						/* ֡�� */		/* ע: ���ݳ��Ȳ�����֡ͷ(2Byte)��֡β(2Byte) */
        pack.chEn = unpack.chEn;					/* ͨ���� */
        memcpy(pSend + 0, &pack.head, 2);
        memcpy(pSend + 2, &pack.len , 2);
        memcpy(pSend + 4, &pack.chEn, 1);
        pack.crc  = CRC16N(pSend + 2, *lenSend - 2);/* У���� */
        pack.tail = FRAME_TAIL_SEND_OLD;			/* ֡β */
    }
    else								/* �°汾Э�� */
    {
        pack.head = FRAME_HEAD_SEND_NEW;			/* ֡ͷ */
        pack.len  = *lenSend;						/* ֡�� */
        pack.feat = unpack.feat;					/* ������ */
//		pack.sta  = 0;								/* ״̬�� */
        pack.chEn = unpack.chEn;					/* ͨ���� */
        memcpy(pSend + 0, &pack.head  , 2);
        memcpy(pSend + 2, &pack.len   , 2);
        memcpy(pSend + 4, &unpack.feat, 1);
        memcpy(pSend + 5, &pack.sta   , 1);
        memcpy(pSend + 6, &pack.chEn  , 1);
        pack.crc  = CRC16N(pSend + 2, *lenSend - 2);/* У���� */
        pack.tail = FRAME_TAIL_SEND_NEW;			/* ֡β */
    }

    memcpy(pSend + *lenSend + 0, &pack.crc , 2);
    memcpy(pSend + *lenSend + 2, &pack.tail, 2);
    *lenSend += 4;		/* �ܳ� */
}

/**********************************************************************************************************
**  �� �� �� : ProtocolDataProcess()
**  ����˵�� : Э�����ݴ�����
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t ProtocolDataProcess(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t pcmd = 0;		/* �������� */

    /* 1. ��μ�� */
    if(pRecv == NULL)		/* ����������׵�ַָ��Ϊ�� */
    {
        return FRAME_PARAM_ERR;			/* ֡�βδ��� */
    }

    if(pSend == NULL)		/* �����������׵�ַָ��Ϊ�� */
    {
        return FRAME_PARAM_ERR;			/* ֡�βδ��� */
    }

    if(lenRecv < 10)		/* ��������ݳ����쳣 */
    {
        return FRAME_PARAM_ERR;			/* ֡�βδ��� */
    }

    if(lenSend == NULL)		/* ���������ݳ��ȵ�ַָ��Ϊ�� */
    {
        return FRAME_PARAM_ERR;			/* ֡�βδ��� */
    }

    /* 2. ���У�� */
    pack.sta = RecvUnpack(pRecv, lenRecv);

    if(ucProtocolVersion == 0)			/* �ɰ汾Э�� */
    {
        lenAnalysis = 5;	/* ֡ͷ(2 Byte) + ֡��(2 Byte) + ͨ����(1 Byte) */

        *lenSend = 5;
    }
    else								/* �°汾Э�� */
    {
        lenAnalysis = 7;	/* ֡ͷ(2 Byte) + ֡��(2 Byte) + ������(1 Byte) + ״̬��(1 Byte) + ͨ����(1 Byte) */

        *lenSend = 7;
    }

    if(pack.sta != FRAME_IS_OK)			/* У���쳣 */
    {
        SendPack(pSend, lenSend);		/* �쳣�ظ� */

        return pack.sta;
    }

    memcpy(&pcmd, pRecv + lenAnalysis, 1);		/* �������� */

    lenAnalysis += 1;							/* �������� */

    /* 3. Э����� */
    switch(pcmd)
    {
			/* ȫ�ֱ������� */
			case CMD_GLOBAL_PROTECT_SET:
			{
					pack.sta = GlobalProtectSetAnalysis(pRecv, lenRecv, pSend, lenSend);     
			}
			break;

			/* ȫ�ֱ�����ѯ */
			case CMD_GLOBAL_PROTECT_INQ:
			{
					pack.sta = GlobalProtectInqAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* �ڲ��������� */
			case CMD_INTERNAL_PROTECT_SET:
			{
					pack.sta = InternalProtectSetAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* �ڲ�������ѯ */
			case CMD_INTERNAL_PROTECT_INQ:
			{
					pack.sta = InternalProtectInqAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* ��¼���ݲ�ѯ */
			case CMD_RECORD_INQ:
			{
					pack.sta = RecordInqAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* �������� */
			case CMD_WORKSTEP_SET:
			{
					pack.sta = WorkstepSetAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* ��������״̬���� */
			case CMD_WORKSTEP_RUNSTA_SET:
			{
					pack.sta = WorkstepRunstaSetAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* ������ѯ */
			case CMD_WORKSTEP_INQ:
			{
					pack.sta = WorkstepInqAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* У׼�������� */
			case CMD_CALIBRATE_PARAM_SET:
			{
					pack.sta = CalibrateSetAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* У׼������ѯ */
			case CMD_CALIBRATE_PARAM_INQ:
			{
					pack.sta = CalibrateInqAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* AD���ݶ�ȡ */
			case CMD_AD_DATA_INQ:
			{
					pack.sta = AdDataInqAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* ϵͳ��ѯ */
			case CMD_SYSTEM_INQ:
			{
					pack.sta = SystemInqAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* ϵͳ���� */
			case CMD_SYSTEM_SET:
			{
					pack.sta = SystemSetAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* ϵͳ���� */
			case CMD_SYSTEM_UPDATE:
			{
					pack.sta = SystemUpdateAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* ACDC����*/
			case CMD_ACDC_UPDATE:
			{
					pack.sta = ACDCUpdateAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* ���Բ�����������/�ظ� */
			case CMD_DEBUG_PARAM_SET:
			{
					pack.sta = DebugParamSetAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* �������ݲ�ѯ����/�ظ� */
			case CMD_DEBUG_DATA_INQ:
			{
					pack.sta = DebugDataInqAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			/* ����ϵͳ��������/�ظ� */
			case CMD_DEBUG_SYSTEM_SET:
			{
					pack.sta = DebugSystemSetAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;
		
			/* Boot����*/
			case CMD_BOOT_UPDATE:
			{
					pack.sta = BootUpdateAnalysis(pRecv, lenRecv, pSend, lenSend);
			}
			break;

			default:
			{
					pack.sta = DefaultAnalyze(pSend, lenSend, pcmd);
			}
			break;
    }

    if((pcmd!=CMD_DEBUG_PARAM_SET)||(pcmd!=CMD_DEBUG_DATA_INQ)||(pcmd!=CMD_DEBUG_SYSTEM_SET)||\
        (pcmd!=CMD_ACDC_UPDATE)||(pcmd!=CMD_SYSTEM_UPDATE)||(pcmd!=CMD_BOOT_UPDATE))    //��DEBUG������ָ����඼��Ϊ�����ж�
    {
			heart_beat_flag=0;
			heart_beat_cnt=0;
    }
    
    SendPack(pSend, lenSend);			/* ������� */

    return pack.sta;
}


//uint8_t  recordBuffer[256] = {0};		/* �������ݻ��� */

int64_t TstampPre[8] = {0};
uint8_t err_send_flag = 0;

/**********************************************************************************************************
**  �� �� �� : RecordDatUpcast()
**  ����˵�� : ��¼�������׺���
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
void RecordDatUpcast(uint8_t chx, uint8_t sync_flag)
{
//	int32_t ls,us;
//
//	Dis_int();
//	ls = osKernelLock();

    if((upcast_enable & (1 << chx)) == 0)
    {
        return;		/* ͨ��ʧ�ܽ�ֹ���� */
    }

    if(heart_beat_flag == 2)
    {
        return;		/* ������ʧ��ֹ���� */
    }

    if(g_ucSysMode != SYS_NORMAL_MODE)
    {
        return;  /* ������ģʽ��ֹ���� */
    }
		if(can_update == 1)
		{
				return;
		}

    uint8_t  j = chx;
    uint8_t  recordBuffer[256] = {0};		/* �������ݻ��� */
    uint16_t head = 0;						/* ֡ͷ         */
    uint16_t len  = 0;						/* ֡��         */
    uint8_t  feat = 0;						/* ������       */
    uint8_t  sta  = 0;						/* ״̬��       */
    uint8_t  ch   = 1 << j;					/* ͨ����       */
    uint8_t  pcmd = CMD_RECORD_INQ_REPLY;	/* �ظ���       */
    uint16_t crc  = 0;						/* У����       */
    uint16_t tail = 0;						/* ֡β         */

//	memset(recordBuffer, 0, 256);

    if(ucProtocolVersion == 0)				/* �ɰ汾Э�� */
    {
        head = FRAME_HEAD_SEND_OLD;
        tail = FRAME_TAIL_SEND_OLD;
        memcpy(recordBuffer + 0, &head, 2);
        //	memcpy(recordBuffer + 2, &len , 2);
        memcpy(recordBuffer + 4, &ch  , 1);
        memcpy(recordBuffer + 5, &pcmd, 1);
        len += 6;
    }
    else									/* �°汾Э�� */
    {
        head = FRAME_HEAD_SEND_NEW;
        tail = FRAME_TAIL_SEND_NEW;
        memcpy(recordBuffer + 0, &head, 2);
        //	memcpy(recordBuffer + 2, &len , 2);
        memcpy(recordBuffer + 4, &feat, 1);
        memcpy(recordBuffer + 5, &sta , 1);
        memcpy(recordBuffer + 6, &ch  , 1);
        memcpy(recordBuffer + 7, &pcmd, 1);
        len += 8;
    }

    if(sync_flag == 1)
    {
        RecordDatSync(j);
    }

    memcpy(recordBuffer + len, &Tier2.record[j], sizeof(Tier2.record[j]));
    len += sizeof(Tier2.record[j]);

//	// 1. ʱ���
//	memcpy(recordBuffer + len, &Tier2.record[j].Tstamp, 8);
//	len += 8;
//	// 2. ����״̬
//	memcpy(recordBuffer + len, &Tier2.record[j].RunSta, 1);
//	len += 1;
//	// 3. ��������
//	memcpy(recordBuffer + len, &Tier2.record[j].StepType, 1);
//	len += 1;
//	// 4. ������
//	memcpy(recordBuffer + len, &Tier2.record[j].StepNumb, 2);
//	len += 2;
//	// 5. ����ʱ��
//	memcpy(recordBuffer + len, &Tier2.record[j].Trun, 4);
//	len += 4;
//	// 6. ��ص�ѹ
//	memcpy(recordBuffer + len, &Tier2.record[j].Vbat, 4);
//	len += 4;
//	// 7. �������(���为��)
//	memcpy(recordBuffer + len, &Tier2.record[j].Iout, 4);
//	len += 4;
//	// 8. �����ѹ
//	memcpy(recordBuffer + len, &Tier2.record[j].Vout, 4);
//	len += 4;
//	// 9. ̽���ѹ
//	memcpy(recordBuffer + len, &Tier2.record[j].Vtz, 4);
//	len += 4;
//	//10. �������
//	memcpy(recordBuffer + len, &Tier2.record[j].Cbat, 4);
//	len += 4;
//	//11. �������
//	memcpy(recordBuffer + len, &Tier2.record[j].Ebat, 4);
//	len += 4;
//	//12. ģʽ��ʶ
//	memcpy(recordBuffer + len, &Tier2.record[j].ModFlag, 4);
//	len += 2;
//	//13. ��������
//	memcpy(recordBuffer + len, &Tier2.record[j].ErrType, 4);
//	len += 2;

    memcpy(recordBuffer + 2       , &len , 2);	/* ֡��   */
    crc  = CRC16N(recordBuffer + 2, len - 2);	/* У���� */
    memcpy(recordBuffer + len + 0 , &crc , 2);	/* У���� */
    memcpy(recordBuffer + len + 2 , &tail, 2);	/* ֡β */
    len += 4;

    /* �������� */
    W5500_send(0, recordBuffer, len);
	
	Can_Network_Upcast(recordBuffer, len);

//	int64_t temp64 = 0;
//	if(ucProtocolVersion == 0)					/* �ɰ汾Э�� */
//	{
//		memcpy(&temp64, recordBuffer + 6, 8);
//	}
//	else
//	{
//		memcpy(&temp64, recordBuffer + 8, 8);
//	}
//
//	if(TstampPre[j] <= temp64)
//	{
//		/* �������� */
//		send(0, recordBuffer, len);
//	}
//	else
//	{
//		err_send_flag = 1;
//	}

//	/* �������� */
//	send(0, recordBuffer, len);

//	us =osKernelUnlock();
//	osKernelRestoreLock(us);
//	osKernelRestoreLock(ls);
//	En_int();
//
//	bsp_DelayMS(1);
}

/**********************************************************************************************************
**	�� �� �� : WorkstepProtUpcast()
**	����˵�� : ���������������׺���
**	��    �� :
**	�� �� ֵ :
**********************************************************************************************************/
void WorkstepProtUpcast(uint8_t chx)
{
    uint8_t  j = chx;
    uint8_t  workstepProtBuffer[256] = {0};
    uint16_t head = 0;							/* ֡ͷ         */
    uint16_t len  = 0;							/* ֡��         */
    uint8_t  feat = 0;							/* ������       */
    uint8_t  sta  = 0;							/* ״̬��       */
    uint8_t  ch   = 1 << j;						/* ͨ����       */
    uint8_t  pcmd = CMD_WORKSTEP_PROTECT_TRIG;	/* �ظ���       */
    uint16_t crc  = 0;							/* У����       */
    uint16_t tail = 0;							/* ֡β         */

    if(ucProtocolVersion == 0)					/* �ɰ汾Э�� */
    {
        head = FRAME_HEAD_SEND_OLD;
        tail = FRAME_TAIL_SEND_OLD;
        memcpy(workstepProtBuffer + 0, &head, 2);
        //	memcpy(workstepProtBuffer + 2, &len , 2);
        memcpy(workstepProtBuffer + 4, &ch  , 1);
        memcpy(workstepProtBuffer + 5, &pcmd, 1);
        len += 6;
    }
    else										/* �°汾Э�� */
    {
        head = FRAME_HEAD_SEND_NEW;
        tail = FRAME_TAIL_SEND_NEW;
        memcpy(workstepProtBuffer + 0, &head, 2);
        //	memcpy(workstepProtBuffer + 2, &len , 2);
        memcpy(workstepProtBuffer + 4, &feat, 1);
        memcpy(workstepProtBuffer + 5, &sta , 1);
        memcpy(workstepProtBuffer + 6, &ch  , 1);
        memcpy(workstepProtBuffer + 7, &pcmd, 1);
        len += 8;
    }

    for(uint8_t i = 1; i < SCMD_WORKSTEP_PROT_END; i++)
    {
        uint16_t umask = 1 << (i - 1);
        if(workstep_flag_ErrWord[j] & umask)
        {
            memcpy(workstepProtBuffer + len, &i, 1);
            len += 1;
        }
    }
    memcpy(workstepProtBuffer + 2       , &len , 2);	/* ֡��   */
    crc  = CRC16N(workstepProtBuffer + 2, len - 2);		/* У���� */
    memcpy(workstepProtBuffer + len + 0 , &crc , 2);	/* У���� */
    memcpy(workstepProtBuffer + len + 2 , &tail, 2);	/* ֡β */
    len += 4;

    /* �������� */
    W5500_send(0, workstepProtBuffer, len);
	
	Can_Network_Upcast(workstepProtBuffer, len);
}

/**********************************************************************************************************
**  �� �� �� : AppTaskProtocol()
**  ����˵�� : ����Э���������
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void AppTaskProtocol(void *argument)
{
    while(1)
    {
        if(osSemaphoreAcquire(w5500_rcv_Semaphore, osWaitForever) == osOK) //�ȴ�ͬ���ź���
        {
            uint16_t total_len = 0;
            uint16_t err_flag = 0;
            uint8_t  buff[1300] = {0};
            ReadRingBuffer(buff, w5500_rcvbuf.rcv_len); //�ӻ��λ���ȡ����

            while(total_len < w5500_rcvbuf.rcv_len)
            {
                uint16_t singal_len = 0;
                uint16_t head = 0;
                uint16_t tail = 0;

                memcpy(&head, &buff[total_len + 0], 2);
                memcpy(&singal_len, &buff[total_len + 2], 2);
                if(head != 0x5A5A)
                {
                    //��֡����
                    err_flag = 1;
                    break;
                }
                if((total_len + singal_len + 4) > w5500_rcvbuf.rcv_len)
                {
                    //��֡����
                    err_flag = 1;
                    break;
                }
                memcpy(&tail, &buff[total_len + singal_len + 2], 2);
                if(tail != 0xA5A5)
                {
                    //��֡����
                    err_flag = 1;
                    break;
                }
                total_len += singal_len + 4;
            }

            total_len = 0;

            if(err_flag == 0)
            {
                while(total_len < w5500_rcvbuf.rcv_len)
                {
                    uint16_t singal_len = 0;

                    memcpy(&singal_len, &buff[total_len + 2], 2);

                    usNetRecvCnt = singal_len + 4;

                    memcpy(&ucNetRecvBuf[0], &buff[total_len], usNetRecvCnt);

                    total_len += singal_len + 4;

                    ProtocolDataProcess(ucNetRecvBuf, usNetRecvCnt, ucNetSendBuf, &usNetSendCnt);   //���ݽ���

                    if(!w5500_Silence)
                    {
                        while(SOCK_BUSY == W5500_send(SOCK_0_NUMB, ucNetSendBuf, usNetSendCnt))
                            ;  //���ݽ�������ظ�������λ��
                        if(ACDC_UpdateFlg==2)         //ACDC�������
                        {
                            bsp_DelayMS(5000);                       //�ȴ�5��
                            NVIC_SystemReset();                      //����
                        }

                    }
                }
            }
        }

        w5500_rcvbuf.rcv_len = 0;

//			if((++test_printf_eth % 500) == 0)
//      {
//          printf("Protocol task is running!");
//      }

        osDelay(1);
    }
}



/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
