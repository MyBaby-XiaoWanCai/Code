/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Protocol.c
** Author      : Cai Heng
** Date        : 2022-07-26
** Description : Э�鴦������
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Task_Protocol.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t ucProtocolVersion = 1; /* ����Э������ 0-��Э�顢1-��Э�� */
uint16_t lenAnalysis = 0;
uint8_t refresh_continue[8] = {1, 1, 1, 1, 1, 1, 1, 1};
uint8_t workstep_flag_RunstaSet = 0;
uint8_t upcast_enable = 0xFF; /* ȫ��ͨ��ʹ������ */
uint8_t clear_flag = 0;
int64_t Tstamp = 0;

uint8_t heart_beat_flag = 2;
uint8_t Reconnection_Flg = 0;	//CAN������־
float data_upcast_period = 1.0f; /* ��������ʱ�� */


uint8_t workstep_flag_CriticalZone[8] = {0};

s_Tier1_Struct Tier1;
s_Tier2_Struct Tier2;
s_ProtocolStruct unpack;
s_ProtocolStruct pack;
s_SystemStruct SystemStatus;
st_DebugParameterType st_DebugParameter;
en_SysMode_Type en_SysMode = Normal;  //ϵͳ����ģʽ

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

    for (i = 0; i < 8; i++)
    {
        if (crc_tmp & 0x01)
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

    for (i = 0; i < len; i++)
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
    if (*(pRecv + 5) != 0) /* ������Ϊ�������� */
    {
        ucProtocolVersion = 0; /* �ɰ汾Э�� */
    }
    else /* Ϊ����Ϊ״̬�� */
    {
        ucProtocolVersion = 1; /* �°汾Э�� */
    }

    if (ucProtocolVersion == 0) /* �ɰ汾Э�� */
    {
        /* ע: ʹ��memcpy()ֱ�Ӹ�ֵ������С��ģʽ */
        memcpy(&unpack.head, pRecv + 0, 2);           /* ֡ͷ */
        memcpy(&unpack.len, pRecv + 2, 2);            /* ֡�� */
        memcpy(&unpack.chEn, pRecv + 4, 1);           /* ͨ���� */
        memcpy(&unpack.crc, pRecv + lenRecv - 4, 2);  /* У���� */
        memcpy(&unpack.tail, pRecv + lenRecv - 2, 2); /* ֡β */
    }
    else /* �°汾Э�� */
    {
        /* ע: ʹ��memcpy()ֱ�Ӹ�ֵ������С��ģʽ */
        memcpy(&unpack.head, pRecv + 0, 2);           /* ֡ͷ */
        memcpy(&unpack.len, pRecv + 2, 2);            /* ֡�� */
        memcpy(&unpack.feat, pRecv + 4, 1);           /* ������ */
        memcpy(&unpack.sta, pRecv + 5, 1);            /* ״̬�� */
        memcpy(&unpack.chEn, pRecv + 6, 1);           /* ͨ���� */
        memcpy(&unpack.crc, pRecv + lenRecv - 4, 2);  /* У���� */
        memcpy(&unpack.tail, pRecv + lenRecv - 2, 2); /* ֡β */
    }

    /* ֡ͷ�ж� */
    if (unpack.head != FRAME_HEAD_RECV)
    {
        return FRAME_HEAD_ERR; /* ֡ͷ���� */
    }

    /* ֡���ж� */ /* ע: ���ݳ��Ȳ�����֡ͷ(2Byte)��֡β(2Byte) */
    if (unpack.len != (lenRecv - 4))
    {
        return FRAME_LEN_ERR; /* ֡������ */
    }

    if (ucProtocolVersion == 0) /* �ɰ汾Э�� */
    {
        /* У�����ж� */ /* ע: �����ݳ��ȵ��������ݶν��� */
        if (unpack.crc != CRC16N(pRecv + 2, lenRecv - 6))
        {
            //          return FRAME_CRC_ERR;   /* У������� */
        }

        /* ֡β�ж� */
        if (unpack.tail != FRAME_TAIL_RECV)
        {
            return FRAME_TAIL_ERR; /* ֡β���� */
        }
    }
    else /* �°汾Э�� */
    {
        /* �������ж� */ /* ע: ������֡������������벻ͬ�������벻��Ϊ0 */
        if (unpack.feat == pack.feat)
        {
            //			if(g_ucSysMode == SYS_NORMAL_MODE)
            {
                return FRAME_FEAT_ERR; /* ��������� */
            }
        }

        /* ״̬���ж� */
        if (unpack.sta != 0)
        {
            return FRAME_STATE_ERR; /* ״̬����� */
        }

        /* У�����ж� */ /* ע: �����ݳ��ȵ��������ݶν��� */
        if (unpack.crc != CRC16N(pRecv + 2, lenRecv - 6))
        {
            //          return FRAME_CRC_ERR;   /* У������� */
        }

        /* ֡β�ж� */
        if (unpack.tail != FRAME_TAIL_RECV)
        {
            return FRAME_TAIL_ERR; /* ֡β���� */
        }
    }

    return FRAME_IS_OK; /* ֡����OK */
}

/**********************************************************************************************************
**  �� �� �� : SendPack()
**  ����˵�� : ���ʹ������
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
static void SendPack(uint8_t *pSend, uint16_t *lenSend)
{
    if (ucProtocolVersion == 0) /* �ɰ汾Э�� */
    {
        pack.head = FRAME_HEAD_SEND_OLD; /* ֡ͷ */
        pack.len = *lenSend; /* ֡�� */  /* ע: ���ݳ��Ȳ�����֡ͷ(2Byte)��֡β(2Byte) */
        pack.chEn = unpack.chEn;         /* ͨ���� */
        memcpy(pSend + 0, &pack.head, 2);
        memcpy(pSend + 2, &pack.len, 2);
        memcpy(pSend + 4, &pack.chEn, 1);
        pack.crc = CRC16N(pSend + 2, *lenSend - 2); /* У���� */
        pack.tail = FRAME_TAIL_SEND_OLD;            /* ֡β */
    }
    else /* �°汾Э�� */
    {
        pack.head = FRAME_HEAD_SEND_NEW; /* ֡ͷ */
        pack.len = *lenSend;             /* ֡�� */
        pack.feat = unpack.feat;         /* ������ */
                                         //      pack.sta  = 0;                              /* ״̬�� */
        pack.chEn = unpack.chEn;         /* ͨ���� */
        memcpy(pSend + 0, &pack.head, 2);
        memcpy(pSend + 2, &pack.len, 2);
        memcpy(pSend + 4, &unpack.feat, 1);
        memcpy(pSend + 5, &pack.sta, 1);
        memcpy(pSend + 6, &pack.chEn, 1);
        pack.crc = CRC16N(pSend + 2, *lenSend - 2); /* У���� */
        pack.tail = FRAME_TAIL_SEND_NEW;            /* ֡β */
    }

    memcpy(pSend + *lenSend + 0, &pack.crc, 2);
    memcpy(pSend + *lenSend + 2, &pack.tail, 2);
    *lenSend += 4; /* �ܳ� */
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
    uint8_t scmd = 0;        /* �������� */
    uint8_t scmd_offset = 0; /* ��������ƫ�� */
    float sdat = 0.0f;       /* ������������ */
    uint8_t pcmd_reply = 0;  /* ��������ظ� */
    uint8_t scmd_reply = 0;  /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_GLOBAL_PROTECT_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    s_Tier1_ProtectStruct *p1; /* һ�׽ṹ��ָ��(���δ����Ĺ��ܶ�����) */
    s_Tier2_ProtectStruct *p2; /* ���׽ṹ��ָ��(����Ѵ���Ĺ��ܶ�����) */

    /* ���ö��׽ṹ����������һ�׽ṹ������(����ṹ�岿�����ݵ�д����Ӱ����������) */
    for (int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if (unpack.chEn & umask)
        {
            p1 = &Tier1.gProt[j].VbatThrH;
            p2 = &Tier2.gProt[j].VbatThrH;
            for (int k = SCMD_GLOBAL_PROTECT_SET_BEGIN; k < SCMD_GLOBAL_PROTECT_SET_END - 1; k++, p1++, p2++)
            {
                p1->cmd = p2->isEnable;
                p1->dat = p2->value;
            }
        }
    }

    /* ���ܶ����ݽ��� */
    for (int i = lenAnalysis; i < (lenRecv - 4); i = i + 5)
    {
        memcpy(&scmd, pRecv + i + 0, 1); /* �������� */
        memcpy(&sdat, pRecv + i + 1, 4); /* ������������ */

        /* ��������������Ϸ����ж� */
        if (!((scmd > SCMD_GLOBAL_PROTECT_SET_BEGIN) && (scmd < SCMD_GLOBAL_PROTECT_SET_END)) &&
            !((scmd > SCMD_GLOBAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_GLOBAL_PROTECT_NSET_END)))
        {
            scmd_reply = 0xFF;
            memcpy(pSend + *lenSend, &scmd, 1);
            *lenSend += 1;
            memcpy(pSend + *lenSend, &scmd_reply, 1);
            *lenSend += 1;
            if (scmd != SCMD_INVALID) /* ����Ч�������� */ /* ��Ч��Ϊ���쳣�� */
            {
                result = FRAME_SCMD_ERR; /* ��������Ϊ�쳣�� */
            }
            continue; /* �������벻ƥ��(��ת��һ��) */
        }

        /* ��������������ƫ�Ƽ��� */
        if ((scmd > SCMD_GLOBAL_PROTECT_SET_BEGIN) && (scmd < SCMD_GLOBAL_PROTECT_SET_END))
        {
            scmd_offset = (scmd - SCMD_GLOBAL_PROTECT_SET_BEGIN) - 1;
        }
        else if ((scmd > SCMD_GLOBAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_GLOBAL_PROTECT_NSET_END))
        {
            scmd_offset = (scmd - SCMD_GLOBAL_PROTECT_NSET_BEGIN) - 1;
        }

        /* �����������������ݴ��� */
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                p1 = &Tier1.gProt[j].VbatThrH;
                p2 = &Tier2.gProt[j].VbatThrH;

                /* ָ���öζ�Ӧ�Ľṹ���Ա���� */
                uint8_t *scmd_addr = &(p1 + scmd_offset)->cmd;
                float *sdat_addr = &(p1 + scmd_offset)->dat;
                float *smin_addr = &(p2 + scmd_offset)->min;
                float *smax_addr = &(p2 + scmd_offset)->max;
                /* �����ݿ�������Ӧ�Ľṹ���Ա������ */
                memcpy(scmd_addr, &scmd, 1);
                memcpy(sdat_addr, &sdat, 4);
                if ((*scmd_addr > SCMD_GLOBAL_PROTECT_SET_BEGIN) && (*scmd_addr < SCMD_GLOBAL_PROTECT_SET_END)) /* ����ʹ�� */
                {
                    if ((*smin_addr == 0) && (*smax_addr == 0)) /* ��У�� */
                    {
                        SET_0(scmd_reply, j);
                    }
                    else if ((*sdat_addr >= *smin_addr) && (*sdat_addr <= *smax_addr)) /* У��ɹ� */
                    {
                        SET_0(scmd_reply, j);
                    }
                    else /* У��ʧ�� */
                    {
                        SET_1(scmd_reply, j);
                        result = FRAME_SCMD_ERR;
                    }
                }
                else /* if((*scmd_addr > SCMD_GLOBAL_PROTECT_NSET_BEGIN) && (*scmd_addr < SCMD_GLOBAL_PROTECT_NSET_END)) */ /* ����ʧ�� */
                {
                    SET_0(scmd_reply, j); /* ��У�� */
                }
            }
        }
        memcpy(pSend + *lenSend, &scmd, 1);
        *lenSend += 1;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
    }

    /* ����������� */
    if (result == 0)
    {
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                p1 = &Tier1.gProt[j].VbatThrH;
                p2 = &Tier2.gProt[j].VbatThrH;

                /* ���¶��׽ṹ������(�������������) */
                for (int k = SCMD_GLOBAL_PROTECT_SET_BEGIN; k < SCMD_GLOBAL_PROTECT_SET_END - 1; k++, p1++, p2++)
                {
                    p2->isEnable = p1->cmd;
                    p2->value = p1->dat;
                    /* �洢��EEPROM�е�ȫ�ֱ���������������ֹ */
                    SET_1(p1->cmd, 7);
                }
                //				EEP_DAT_SYNC(PROTECT_GLOBAL_ID(j));
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
    uint8_t scmd = 0;        /* �������� */
    uint8_t scmd_offset = 0; /* ��������ƫ�� */
                             //  float   sdat = 0.0f;        /* ������������ */
    uint8_t pcmd_reply = 0;  /* ��������ظ� */
    float scmd_reply = 0.0f; /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_GLOBAL_PROTECT_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ���ܶ����ݽ��� */
    for (int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if (unpack.chEn & umask)
        {
            s_Tier1_ProtectStruct *p1 = &Tier1.gProt[j].VbatThrH; /* һ�׽ṹ��ָ�� */
            s_Tier2_ProtectStruct *p2 = &Tier2.gProt[j].VbatThrH; /* ���׽ṹ��ָ�� */
            /* ���ö��׽ṹ�����ݸ���һ�׽ṹ������(���ڻظ�) */
            for (int k = SCMD_GLOBAL_PROTECT_INQ_BEGIN; k < SCMD_GLOBAL_PROTECT_INQ_END - 1; k++, p1++, p2++)
            {
                p1->cmd = p2->isEnable;
                p1->dat = p2->value;
            }
            /* ����ָ���ʼֵ */
            p1 = &Tier1.gProt[j].VbatThrH;
            p2 = &Tier2.gProt[j].VbatThrH;

            /* ���������ѭ������ */
            for (int i = lenAnalysis; i < (lenRecv - 4); i = i + 1)
            {
                memcpy(&scmd, pRecv + i, 1);

                if (scmd == SCMD_INVALID) /* ��Ч�� */
                {
                    if (i == lenAnalysis) /* �׸���������Ϊ��Ч�� - ȫ����ѯ */
                    {
                        for (int k = SCMD_GLOBAL_PROTECT_INQ_BEGIN; k < SCMD_GLOBAL_PROTECT_INQ_END - 1; k++, p1++, p2++)
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
                        scmd_reply = 0.0f;
                        memcpy(pSend + *lenSend, &scmd, 1);
                        *lenSend += 1;
                        memcpy(pSend + *lenSend, &scmd_reply, 4);
                        *lenSend += 4;
                        continue;
                    }
                }
                else if ((scmd <= SCMD_GLOBAL_PROTECT_INQ_BEGIN) || (scmd >= SCMD_GLOBAL_PROTECT_INQ_END))
                {
                    scmd_reply = 0.0f;
                    memcpy(pSend + *lenSend, &scmd, 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &scmd_reply, 4);
                    *lenSend += 4;
                    result = FRAME_SCMD_ERR; /* ��������Ϊ�쳣�� */
                    continue;                /* �������벻ƥ��(��ת��һ��) */
                }

                /* ��������ƫ���� */
                scmd_offset = (scmd - SCMD_GLOBAL_PROTECT_INQ_BEGIN) - 1;
                /* ��������ظ� */
                scmd = (p1 + scmd_offset)->cmd;
                /* �����������ݻظ� */
                scmd_reply = (p1 + scmd_offset)->dat;
                memcpy(pSend + *lenSend, &scmd, 1);
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
    uint8_t scmd = 0;        /* �������� */
    uint8_t scmd_offset = 0; /* ��������ƫ�� */
    float sdat = 0.0f;       /* ������������ */
    uint8_t pcmd_reply = 0;  /* ��������ظ� */
    uint8_t scmd_reply = 0;  /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_INTERNAL_PROTECT_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    s_Tier1_ProtectStruct *p1; /* һ�׽ṹ��ָ��(���δ����Ĺ��ܶ�����) */
    s_Tier2_ProtectStruct *p2; /* ���׽ṹ��ָ��(����Ѵ���Ĺ��ܶ�����) */

    /* ���ö��׽ṹ����������һ�׽ṹ������(����ṹ�岿�����ݵ�д����Ӱ����������) */
    for (int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if (unpack.chEn & umask)
        {
            p1 = &Tier1.iProt[j].IoutThr;
            p2 = &Tier2.iProt[j].IoutThr;
            for (int k = SCMD_INTERNAL_PROTECT_SET_BEGIN; k < SCMD_INTERNAL_PROTECT_SET_END - 1; k++, p1++, p2++)
            {
                p1->cmd = p2->isEnable;
                p1->dat = p2->value;
            }
        }
    }

    /* ���ܶ����ݽ��� */
    for (int i = lenAnalysis; i < (lenRecv - 4); i = i + 5)
    {
        memcpy(&scmd, pRecv + i + 0, 1); /* �������� */
        memcpy(&sdat, pRecv + i + 1, 4); /* ������������ */

        /* ��������������Ϸ����ж� */
        if (!((scmd > SCMD_INTERNAL_PROTECT_SET_BEGIN) && (scmd < SCMD_INTERNAL_PROTECT_SET_END)) &&
            !((scmd > SCMD_INTERNAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_INTERNAL_PROTECT_NSET_END)))
        {
            scmd_reply = 0xFF;
            memcpy(pSend + *lenSend, &scmd, 1);
            *lenSend += 1;
            memcpy(pSend + *lenSend, &scmd_reply, 1);
            *lenSend += 1;
            if (scmd != SCMD_INVALID) /* ����Ч�������� */ /* ��Ч��Ϊ���쳣�� */
            {
                result = FRAME_SCMD_ERR; /* ��������Ϊ�쳣�� */
            }
            continue; /* �������벻ƥ��(��ת��һ��) */
        }

        /* ��������������ƫ�Ƽ��� */
        if ((scmd > SCMD_INTERNAL_PROTECT_SET_BEGIN) && (scmd < SCMD_INTERNAL_PROTECT_SET_END))
        {
            scmd_offset = (scmd - SCMD_INTERNAL_PROTECT_SET_BEGIN) - 1;
        }
        else if ((scmd > SCMD_INTERNAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_INTERNAL_PROTECT_NSET_END))
        {
            scmd_offset = (scmd - SCMD_INTERNAL_PROTECT_NSET_BEGIN) - 1;
        }

        /* �����������������ݴ��� */
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                p1 = &Tier1.iProt[j].IoutThr;
                p2 = &Tier2.iProt[j].IoutThr;

                /* ָ���öζ�Ӧ�Ľṹ���Ա���� */
                uint8_t *scmd_addr = &(p1 + scmd_offset)->cmd;
                float *sdat_addr = &(p1 + scmd_offset)->dat;
                float *smin_addr = &(p2 + scmd_offset)->min;
                float *smax_addr = &(p2 + scmd_offset)->max;
                /* �����ݿ�������Ӧ�Ľṹ���Ա������ */
                memcpy(scmd_addr, &scmd, 1);
                memcpy(sdat_addr, &sdat, 4);
                if ((*scmd_addr > SCMD_INTERNAL_PROTECT_SET_BEGIN) && (*scmd_addr < SCMD_INTERNAL_PROTECT_SET_END)) /* ����ʹ�� */
                {
                    if ((*smin_addr == 0) && (*smax_addr == 0)) /* ��У�� */
                    {
                        SET_0(scmd_reply, j);
                    }
                    else if ((*sdat_addr >= *smin_addr) && (*sdat_addr <= *smax_addr)) /* У��ɹ� */
                    {
                        SET_0(scmd_reply, j);
                    }
                    else /* У��ʧ�� */
                    {
                        SET_1(scmd_reply, j);
                        result = FRAME_SCMD_ERR;
                    }
                }
                else /* if((*scmd_addr > SCMD_INTERNAL_PROTECT_NSET_BEGIN) && (*scmd_addr < SCMD_INTERNAL_PROTECT_NSET_END)) */ /* ����ʧ�� */
                {
                    SET_0(scmd_reply, j); /* ��У�� */
                }
            }
        }
        memcpy(pSend + *lenSend, &scmd, 1);
        *lenSend += 1;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
    }

    /* ����������� */
    if (result == 0)
    {
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                p1 = &Tier1.iProt[j].IoutThr;
                p2 = &Tier2.iProt[j].IoutThr;

                /* ���¶��׽ṹ������(�������������) */
                for (int k = SCMD_INTERNAL_PROTECT_SET_BEGIN; k < SCMD_INTERNAL_PROTECT_SET_END - 1; k++, p1++, p2++)
                {
                    p2->isEnable = p1->cmd;
                    p2->value = p1->dat;
                    /* �洢��EEPROM�е��ڲ����������������ʹ�� */
                    SET_0(p1->cmd, 7);
                }
                //				EEP_DAT_SYNC(PROTECT_INTERNAL_ID(j));
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
    uint8_t scmd = 0;        /* �������� */
    uint8_t scmd_offset = 0; /* ��������ƫ�� */
                             //  float   sdat = 0.0f;        /* ������������ */
    uint8_t pcmd_reply = 0;  /* ��������ظ� */
    float scmd_reply = 0.0f; /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_INTERNAL_PROTECT_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ���ܶ����ݽ��� */
    for (int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if (unpack.chEn & umask)
        {
            s_Tier1_ProtectStruct *p1 = &Tier1.iProt[j].IoutThr; /* һ�׽ṹ��ָ�� */
            s_Tier2_ProtectStruct *p2 = &Tier2.iProt[j].IoutThr; /* ���׽ṹ��ָ�� */
            /* ���ö��׽ṹ�����ݸ���һ�׽ṹ������(���ڻظ�) */
            for (int k = SCMD_INTERNAL_PROTECT_INQ_BEGIN; k < SCMD_INTERNAL_PROTECT_INQ_END - 1; k++, p1++, p2++)
            {
                p1->cmd = p2->isEnable;
                p1->dat = p2->value;
            }
            /* ����ָ���ʼֵ */
            p1 = &Tier1.iProt[j].IoutThr;
            p2 = &Tier2.iProt[j].IoutThr;

            for (int i = lenAnalysis; i < (lenRecv - 4); i = i + 1)
            {
                memcpy(&scmd, pRecv + i, 1);

                if (scmd == SCMD_INVALID)
                {
                    if (i == lenAnalysis) /* �׸���������Ϊ��Ч�� - ȫ����ѯ */
                    {
                        for (int k = SCMD_INTERNAL_PROTECT_INQ_BEGIN; k < SCMD_INTERNAL_PROTECT_INQ_END - 1; k++, p1++, p2++)
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
                        scmd_reply = 0.0f;
                        memcpy(pSend + *lenSend, &scmd, 1);
                        *lenSend += 1;
                        memcpy(pSend + *lenSend, &scmd_reply, 4);
                        *lenSend += 4;
                        continue;
                    }
                }
                else if ((scmd <= SCMD_INTERNAL_PROTECT_INQ_BEGIN) || (scmd >= SCMD_INTERNAL_PROTECT_INQ_END))
                {
                    scmd_reply = 0.0f;
                    memcpy(pSend + *lenSend, &scmd, 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &scmd_reply, 4);
                    *lenSend += 4;
                    result = FRAME_SCMD_ERR; /* ��������Ϊ�쳣�� */
                    continue;                /* �������벻ƥ��(��ת��һ��) */
                }

                /* ��������ƫ���� */
                scmd_offset = (scmd - SCMD_INTERNAL_PROTECT_INQ_BEGIN) - 1;
                /* ��������ظ� */
                scmd = (p1 + scmd_offset)->cmd;
                /* �����������ݻظ� */
                scmd_reply = (p1 + scmd_offset)->dat;
                memcpy(pSend + *lenSend, &scmd, 1);
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
    Tier2.record[j].Tstamp = Tstamp;
    // 2. ����״̬
    if (start_state_flag[j] == 2)
    {
        start_state_flag[j] = 0;
        Tier2.record[j].RunSta = WORKSTEP_RUNSTA_REF_START;
    }
    else
    {
        Tier2.record[j].RunSta = Tier2.workstepRunning[j].RunSta;
    }
    // 3. ��������
    if (Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_START)
    {
        Tier2.record[j].StepType = Tier2.workstepCache1[j].Type; // ���ⷢ���������̿�ʼ֡ʱ��żȻ���ֹ�������Ϊ�յ����
    }
    else
    {
        Tier2.record[j].StepType = Tier2.workstepRunning[j].StepType;
    }
    // 4. ������
    if (Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_START)
    {
        Tier2.record[j].StepNumb = Tier2.workstepCache1[j].Numb;
    }
    else
    {
        Tier2.record[j].StepNumb = Tier2.workstepRunning[j].StepNumb;
    }
    // 5. ����ʱ��
    if (time_over_stop[j] == 1)
    {
        Tier2.record[j].Trun = Tier2.workstepRunning[j].Trun / 1000;
    }
    else
    {
        Tier2.record[j].Trun = Tier2.workstepRunning[j].Trun / 1000.0f;
    }
    // 6. ��ص�ѹ
    Tier2.record[j].Vbat = Tier2.chRecord[j].Vbat;
    // 7. �������(���为��)
    if ((Tier2.workstepRunning[j].StepType > SCMD_WORKSTEP_TYPE_STANDBY) &&
        (Tier2.workstepRunning[j].StepType < SCMD_WORKSTEP_TYPE_FINISH) &&
        ((Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) ||
         (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)))
    {
        if (Tier2.chRecord[j].Iout < (0.0005f * WORK_CUR_MAX))
        {
            Tier2.record[j].Iout = 0.0f;
        }
        else
        {
            Tier2.record[j].Iout = Tier2.chRecord[j].Iout;
        }
    }
    else
    {
        if (Tier2.chRecord[j].Iout < (0.0005f * WORK_CUR_MAX))
        {
            Tier2.record[j].Iout = 0.0f;
        }
        else
        {
            if (ProtectMaskFlag[j] != 1)
            {
                Tier2.record[j].Iout = Tier2.chRecord[j].Iout;
            }
            else
            {
                Tier2.record[j].Iout = Tier2.chRecord[j].Iout;
            }
        }
    }
    // 8. �����ѹ
    Tier2.record[j].Vout = Tier2.chRecord[j].Vout;
    // 9. ̽���ѹ
    Tier2.record[j].Vtz = Tier2.chRecord[j].Vtz;
    // 10. �������
    Tier2.record[j].Cbat = Tier2.chRecord[j].Cbat;
    // 11. �������
    Tier2.record[j].Ebat = Tier2.chRecord[j].Ebat;
    // 12. ģʽ��ʶ
    Tier2.record[j].ModFlag = 0;
    // 13. ��������
    //	if(Protect_Flag_Global[j] != 0)						// ȫ�ֱ���
    //	{
    //		SET_1(Tier2.record[j].ErrType, 0);
    //	}
    //	else
    //	{
    //		SET_0(Tier2.record[j].ErrType, 0);
    //	}
    //	if(Protect_Flag_Workstep[j] != 0)					// ����������
    //	{
    //		SET_1(Tier2.record[j].ErrType, 1);
    //	}
    //	else
    //	{
    //		SET_0(Tier2.record[j].ErrType, 1);
    //	}
    //	if(Protect_Flag_Internal[j] != 0)					// �ڲ�����
    //	{
    //		SET_1(Tier2.record[j].ErrType, 2);
    //	}
    //	else
    //	{
    //		SET_0(Tier2.record[j].ErrType, 2);
    //	}
    //
    //	if((ACDC.Type == 1) || (ACDC.Type == 11))			// ����
    //	{
    //		if((ACDC.State[0] & 0x0FF8) || (ACDC.State[1] & 0x0FF8))	// ACDC����
    //		{
    //			SET_1(Tier2.record[j].ErrType, 3);
    //		}
    //		else
    //		{
    //			SET_0(Tier2.record[j].ErrType, 3);
    //		}
    //	}
    //	else if((ACDC.Type == 2) || (ACDC.Type == 22))		// �յ�
    //	{
    //		if((ACDC.State[0] & 0xBFF0) || (ACDC.State[1] & 0xBFF0))	// ACDC����
    //		{
    //			SET_1(Tier2.record[j].ErrType, 3);
    //		}
    //		else
    //		{
    //			SET_0(Tier2.record[j].ErrType, 3);
    //		}
    //	}
    Tier2.record[j].ErrType = 0;

    Tier2.record[j].AbortType = abort_type_flag[j];

    if (abort_type_flag[j] != 0)
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
    uint8_t pcmd_reply = 0; /* ��������ظ� */
                            //	uint8_t scmd_reply = 0;		/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_RECORD_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ���ܶ����ݽ��� */
    for (int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if (unpack.chEn & umask)
        {
            RecordDatSync(j);
            memcpy(pSend + *lenSend, &Tier2.record[j], sizeof(Tier2.record[j]));
            *lenSend += sizeof(Tier2.record[j]);
        }
    }

    return result;
}

/**********************************************************************************************************
**  �� �� �� : RecordDatUpcast()
**  ����˵�� : ��¼�������׺���
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
void RecordDatUpcast(uint8_t chx)
{
    if ((upcast_enable & (1 << chx)) == 0)
    {
        return; /* ͨ��ʧ�ܽ�ֹ���� */
    }

    uint8_t j = chx;
    uint8_t recordBuffer[256] = {0};     /* �������ݻ��� */
    uint16_t head = 0;                   /* ֡ͷ         */
    uint16_t len = 0;                    /* ֡��         */
    uint8_t feat = 0;                    /* ������       */
    uint8_t sta = 0;                     /* ״̬��       */
    uint8_t ch = 1 << j;                 /* ͨ����       */
    uint8_t pcmd = CMD_RECORD_INQ_REPLY; /* �ظ���       */
    uint16_t crc = 0;                    /* У����       */
    uint16_t tail = 0;                   /* ֡β         */

    if (ucProtocolVersion == 0) /* �ɰ汾Э�� */
    {
        head = FRAME_HEAD_SEND_OLD;
        tail = FRAME_TAIL_SEND_OLD;
        memcpy(recordBuffer + 0, &head, 2);
        //		memcpy(recordBuffer + 2, &len , 2);
        memcpy(recordBuffer + 4, &ch, 1);
        memcpy(recordBuffer + 5, &pcmd, 1);
        len += 6;
    }
    else /* �°汾Э�� */
    {
        head = FRAME_HEAD_SEND_NEW;
        tail = FRAME_TAIL_SEND_NEW;
        memcpy(recordBuffer + 0, &head, 2);
        //		memcpy(recordBuffer + 2, &len , 2);
        memcpy(recordBuffer + 4, &feat, 1);
        memcpy(recordBuffer + 5, &sta, 1);
        memcpy(recordBuffer + 6, &ch, 1);
        memcpy(recordBuffer + 7, &pcmd, 1);
        len += 8;
    }

    RecordDatSync(j);

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

    memcpy(recordBuffer + 2, &len, 2);        /* ֡��   */
    crc = CRC16N(recordBuffer + 2, len - 2);  /* У���� */
    memcpy(recordBuffer + len + 0, &crc, 2);  /* У���� */
    memcpy(recordBuffer + len + 2, &tail, 2); /* ֡β */
    len += 4;

    /* �������� */
    Can_Network_Upcast(recordBuffer, len);
}

/**********************************************************************************************************
**  �� �� �� : WorkstepProtUpcast()
**  ����˵�� : ���������������׺���
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
void WorkstepProtUpcast(uint8_t chx)
{
    uint8_t j = chx;
    uint8_t workstepProtBuffer[256] = {0};
    uint16_t head = 0;                        /* ֡ͷ         */
    uint16_t len = 0;                         /* ֡��         */
    uint8_t feat = 0;                         /* ������       */
    uint8_t sta = 0;                          /* ״̬��       */
    uint8_t ch = 1 << j;                      /* ͨ����       */
    uint8_t pcmd = CMD_WORKSTEP_PROTECT_TRIG; /* �ظ���       */
    uint16_t crc = 0;                         /* У����       */
    uint16_t tail = 0;                        /* ֡β         */

    if (ucProtocolVersion == 0) /* �ɰ汾Э�� */
    {
        head = FRAME_HEAD_SEND_OLD;
        tail = FRAME_TAIL_SEND_OLD;
        memcpy(workstepProtBuffer + 0, &head, 2);
        //		memcpy(workstepProtBuffer + 2, &len , 2);
        memcpy(workstepProtBuffer + 4, &ch, 1);
        memcpy(workstepProtBuffer + 5, &pcmd, 1);
        len += 6;
    }
    else /* �°汾Э�� */
    {
        head = FRAME_HEAD_SEND_NEW;
        tail = FRAME_TAIL_SEND_NEW;
        memcpy(workstepProtBuffer + 0, &head, 2);
        //		memcpy(workstepProtBuffer + 2, &len , 2);
        memcpy(workstepProtBuffer + 4, &feat, 1);
        memcpy(workstepProtBuffer + 5, &sta, 1);
        memcpy(workstepProtBuffer + 6, &ch, 1);
        memcpy(workstepProtBuffer + 7, &pcmd, 1);
        len += 8;
    }

    for (uint8_t i = 1; i < SCMD_WORKSTEP_PROT_END; i++)
    {
        uint16_t umask = 1 << (i - 1);
        if (workstep_flag_ErrWord[j] & umask)
        {
            memcpy(workstepProtBuffer + len, &i, 1);
            len += 1;
        }
    }
    memcpy(workstepProtBuffer + 2, &len, 2);        /* ֡��   */
    crc = CRC16N(workstepProtBuffer + 2, len - 2);  /* У���� */
    memcpy(workstepProtBuffer + len + 0, &crc, 2);  /* У���� */
    memcpy(workstepProtBuffer + len + 2, &tail, 2); /* ֡β */
    len += 4;

    /* �������� */
    Can_Network_Upcast(workstepProtBuffer, len);
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
    //  uint8_t scmd = 0;           /* �������� */
    //  uint8_t scmd_offset = 0;    /* ��������ƫ�� */
    //  float   sdat = 0.0f;        /* ������������ */
    uint8_t pcmd_reply = 0; /* ��������ظ� */
    uint8_t scmd_reply = 0; /* ��������ظ� */

    uint16_t step_numb = 0; /* ������ */
    uint8_t step_type = 0;  /* �������� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_WORKSTEP_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ��ȡ�����ż��������� */
    memcpy(&step_numb, pRecv + lenAnalysis + 0, 2); /* ��ȡ������ */
    memcpy(&step_type, pRecv + lenAnalysis + 2, 1); /* ��ȡ�������� */

    /* �����źϷ����ж� */
    if ((step_numb <= SCMD_WORKSTEP_NUM_BEGIN) || (step_numb >= SCMD_WORKSTEP_NUM_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 1; /* �����Ų�ƥ��(����) */
        memcpy(pSend + *lenSend, &step_numb, 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    /* �������ͺϷ����ж� */
    if ((step_type <= SCMD_WORKSTEP_TYPE_BEGIN) || (step_type >= SCMD_WORKSTEP_TYPE_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 2; /* �������Ͳ�ƥ��(����) */
        memcpy(pSend + *lenSend, &step_numb, 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    if (step_type != SCMD_WORKSTEP_TYPE_FINISH) /* �ǽ������� */
    {
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                memset(&Tier1.workstep[j], 0, sizeof(Tier1.workstep[j])); /* ����һ�׽ṹ��(����1>ǰ���쳣����2>����֡�����֡��Ӱ��) */ /* Status����Ҫ���� */
                memcpy(&Tier1.workstep[j], pRecv + lenAnalysis, lenRecv - 12);                                                           /* �����յĹ�����Ϣ�����һ�׽ṹ���� */

                uint8_t data_check = 0; /* ����У�� */
                s_WorkStepSegmentStruct *p = &Tier1.workstep[j].RunSeg[0];

                /* ���й��������Ϸ����ж� */
                for (int i = 0; i < RUN_SEG_NUM; i++) /* ���й������� */
                {
                    if (((p + i)->type != SCMD_INVALID) &&
                        (((p + i)->type <= SCMD_WORKSTEP_PARAM_BEGIN) ||
                         ((p + i)->type >= SCMD_WORKSTEP_PARAM_END)))
                    {
                        result = FRAME_SCMD_ERR;
                        scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
                        memcpy(pSend + *lenSend, &step_numb, 2);
                        *lenSend += 2;
                        memcpy(pSend + *lenSend, &scmd_reply, 1);
                        *lenSend += 1;
                        return result;
                    }
                    /* ������ŵ繤������,���в������������õ���ֵ */
//					else if ((step_type == SCMD_WORKSTEP_TYPE_CC) || (step_type == SCMD_WORKSTEP_TYPE_DCC))
					else if (step_type == SCMD_WORKSTEP_TYPE_CC)
                    {
                        if ((p + i)->type == SCMD_WORKSTEP_PARAM_CURRENT)
                        {
                            data_check++;
                        }
						else if ((p + i)->type == SCMD_WORKSTEP_PARAM_VOLTAGE)
						{
							data_check++;
						}
                        else if ((p + i)->type == SCMD_INVALID)
                        {
                            continue;
                        }
                        else
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb, 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
//                    /* ��ѹ��ŵ繤������,���в������������õ���ֵ(��ѡ)����ѹֵ(��ѡ) */
//                    else if ((step_type == SCMD_WORKSTEP_TYPE_CV) || (step_type == SCMD_WORKSTEP_TYPE_DCV))
//                    {
//                        if ((p + i)->type == SCMD_WORKSTEP_PARAM_VOLTAGE)
//                        {
//                            data_check = 2;
//                        }
//                        else if ((p + i)->type == SCMD_INVALID)
//                        {
//                            continue;
//                        }
//                        else if ((p + i)->type != SCMD_WORKSTEP_PARAM_CURRENT)
//                        {
//                            result = FRAME_SCMD_ERR;
//                            scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
//                            memcpy(pSend + *lenSend, &step_numb, 2);
//                            *lenSend += 2;
//                            memcpy(pSend + *lenSend, &scmd_reply, 1);
//                            *lenSend += 1;
//                            return result;
//                        }
//                    }
//                    /* ������ѹ��ŵ繤������,���в������������õ���ֵ(��ѡ)����ѹֵ(��ѡ) */
//                    else if ((step_type == SCMD_WORKSTEP_TYPE_CCTOCV) || (step_type == SCMD_WORKSTEP_TYPE_DCCTODCV))
//                    {
//                        if (((p + i)->type == SCMD_WORKSTEP_PARAM_VOLTAGE) || ((p + i)->type == SCMD_WORKSTEP_PARAM_CURRENT))
//                        {
//                            data_check++;
//                        }
//                        else if ((p + i)->type == SCMD_INVALID)
//                        {
//                            continue;
//                        }
//                        else
//                        {
//                            result = FRAME_SCMD_ERR;
//                            scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
//                            memcpy(pSend + *lenSend, &step_numb, 2);
//                            *lenSend += 2;
//                            memcpy(pSend + *lenSend, &scmd_reply, 1);
//                            *lenSend += 1;
//                            return result;
//                        }
//                    }
                    /* ����ģʽ�������������в��� */
                    else if (step_type == SCMD_WORKSTEP_TYPE_STANDBY)
                    {
                        if ((p + i)->type == SCMD_INVALID)
                        {
                            data_check = 2;
                        }
                        else
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb, 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
					else
					{
						result = FRAME_SCMD_ERR;
						scmd_reply = 3;         /* �����趨���Ͳ�ƥ��(����) */
						memcpy(pSend + *lenSend, &step_numb , 2);
						*lenSend += 2;
						memcpy(pSend + *lenSend, &scmd_reply, 1);
						*lenSend += 1;
						return result;					
					}
                }

                /* ���й��������Ϸ���У�� */
                if (data_check == 2) /* ���й�������������ȷ */
                {
                    data_check = 0;
                }
                else /* if(data_check != 2) */ /* ���й����������ô��� */
                {
                    result = FRAME_SCMD_ERR;
                    scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
                    memcpy(pSend + *lenSend, &step_numb, 2);
                    *lenSend += 2;
                    memcpy(pSend + *lenSend, &scmd_reply, 1);
                    *lenSend += 1;
                    return result;
                }

                /* ��ֹ���������Ϸ����ж� */
                for (int i = RUN_SEG_NUM; i < (RUN_SEG_NUM + ABT_SEG_NUM); i++)
                {
                    uint8_t abort_condition = (p + i)->type;
                    //					uint8_t abort_priority  = (abort_condition >> 4) & 0x0F;
                    uint8_t abort_type = (abort_condition >> 0) & 0x0F;

                    /* ��Ч��ֹ���� */
                    if (abort_type == 0x0F)
                    {
                        continue;
                    }

                    /* ��ֹ���Ͳ��� */
                    if ((abort_type <= SCMD_WORKSTEP_PARAM_BEGIN) || (abort_type >= SCMD_WORKSTEP_PARAM_END))
                    {
                        result = FRAME_SCMD_ERR;
                        scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
                        memcpy(pSend + *lenSend, &step_numb, 2);
                        *lenSend += 2;
                        memcpy(pSend + *lenSend, &scmd_reply, 1);
                        *lenSend += 1;
                        return result;
                    }
                    /* ������ŵ������ʱ���ֹ����ѹ��ֹ��������ֹ */
//					if ((step_type == SCMD_WORKSTEP_TYPE_CC) ||
//                      (step_type == SCMD_WORKSTEP_TYPE_DCC))
                    if (step_type == SCMD_WORKSTEP_TYPE_CC)
                    {
                        if (abort_type == SCMD_WORKSTEP_PARAM_VOLTAGE)
                        {
                            data_check = 2;
                        }
                        else if ((abort_type == SCMD_WORKSTEP_PARAM_TIME) ||
                                 (abort_type == SCMD_WORKSTEP_PARAM_CAPACITY))
                        {
                            if (data_check != 2)
                            {
                                data_check = 1;
                            }
                        }
                        else
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb, 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
//                    /* ��ѹ��ŵ������ʱ���ֹ��������ֹ��������ֹ */
//                    else if ((step_type == SCMD_WORKSTEP_TYPE_CV) ||
//                             (step_type == SCMD_WORKSTEP_TYPE_DCV))
//                    {
//                        if ((abort_type == SCMD_WORKSTEP_PARAM_TIME) ||
//                            (abort_type == SCMD_WORKSTEP_PARAM_CURRENT) ||
//                            (abort_type == SCMD_WORKSTEP_PARAM_CAPACITY))
//                        {
//                            data_check = 2;
//                        }
//                        else
//                        {
//                            result = FRAME_SCMD_ERR;
//                            scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
//                            memcpy(pSend + *lenSend, &step_numb, 2);
//                            *lenSend += 2;
//                            memcpy(pSend + *lenSend, &scmd_reply, 1);
//                            *lenSend += 1;
//                            return result;
//                        }
//                    }
//                    /* ������ѹ��ŵ������ʱ���ֹ��������ֹ��������ֹ */
//                    else if ((step_type == SCMD_WORKSTEP_TYPE_CCTOCV) ||
//                             (step_type == SCMD_WORKSTEP_TYPE_DCCTODCV))
//                    {
//                        if ((abort_type == SCMD_WORKSTEP_PARAM_TIME) ||
//                            (abort_type == SCMD_WORKSTEP_PARAM_CURRENT) ||
//                            (abort_type == SCMD_WORKSTEP_PARAM_CAPACITY))
//                        {
//                            data_check = 2;
//                        }
//                        else
//                        {
//                            result = FRAME_SCMD_ERR;
//                            scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
//                            memcpy(pSend + *lenSend, &step_numb, 2);
//                            *lenSend += 2;
//                            memcpy(pSend + *lenSend, &scmd_reply, 1);
//                            *lenSend += 1;
//                            return result;
//                        }
//                    }
                    /* ����ģʽ����������ʱ���ֹ */
                    else if (step_type == SCMD_WORKSTEP_TYPE_STANDBY)
                    {
                        if (abort_type == SCMD_WORKSTEP_PARAM_TIME)
                        {
                            data_check = 2;
                        }
                        else
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb, 2);
                            *lenSend += 2;
                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                            *lenSend += 1;
                            return result;
                        }
                    }
                }

                /* ��ֹ���������Ϸ���У�� */
                if (data_check == 2) /* ��ֹ��������������ȷ */
                {
                    data_check = 0;
                }
                else /* ��ֹ�����������ô��� */
                {
                    result = FRAME_SCMD_ERR;
                    scmd_reply = 3; /* �����趨���Ͳ�ƥ��(����) */
                    memcpy(pSend + *lenSend, &step_numb, 2);
                    *lenSend += 2;
                    memcpy(pSend + *lenSend, &scmd_reply, 1);
                    *lenSend += 1;
                    return result;
                }

                /* �������������Ϸ����ж� */
                for (int i = (RUN_SEG_NUM + ABT_SEG_NUM); i < (RUN_SEG_NUM + ABT_SEG_NUM + PRO_SEG_NUM); i++)
                {
                    if (!(((p + i)->type > SCMD_WORKSTEP_PROT_BEGIN) && ((p + i)->type < SCMD_WORKSTEP_PROT_END)) &&
                        !(((p + i)->type > SCMD_WORKSTEP_NPROT_BEGIN) && ((p + i)->type < SCMD_WORKSTEP_NPROT_END)))
                    {
                        if ((p + i)->type != SCMD_INVALID)
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 4; /* �����������Ͳ�ƥ��(����) */
                            memcpy(pSend + *lenSend, &step_numb, 2);
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
    else /* ��������   */
    {
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                memset(&Tier1.workstep[j], 0, sizeof(Tier1.workstep[j])); /* ����һ�׽ṹ��(����1>ǰ���쳣����2>����֡�����֡��Ӱ��) */ /* Status����Ҫ���� */
                memcpy(&Tier1.workstep[j], pRecv + lenAnalysis, lenRecv - 12);                                                           /* �����յĹ�����Ϣ�����һ�׽ṹ���� */
            }
        }
    }
    memcpy(pSend + *lenSend, &step_numb, 2);
    *lenSend += 2;
    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

    if (result == 0)
    {
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            if ((Tier2.record[j].ErrType != 0) && (Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_WAIT) && (AcDumpConnectFlag == 1))
            {
                return result;
            }
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                if (Tier2.workstepCache1[j].Numb == 0) /* ��Cache1Ϊ�գ������չ����洢��Cache1 */
                {
                    if ((step_type == SCMD_WORKSTEP_TYPE_FINISH) &&
                        ((Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_STOP) || (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)))
                    {
                        /* do nothing */
                    }
                    else
                    {
                        memset(&Tier2.workstepCache1[j], 0, sizeof(Tier2.workstepCache1[j]));
                        memcpy(&Tier2.workstepCache1[j], &Tier1.workstep[j], sizeof(Tier1.workstep[j]));
                    }
                }
                else /* if(Tier2.workstepCache2[j].Numb == 0) */ /* ��Cache1�ǿգ������չ����洢��Cache2(���Cache2���й�����Ϣ��������) */
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
    //  uint8_t scmd = 0;           /* �������� */
    //  uint8_t scmd_offset = 0;    /* ��������ƫ�� */
    //  float   sdat = 0.0f;        /* ������������ */
    uint8_t pcmd_reply = 0; /* ��������ظ� */
    uint8_t scmd_reply = 0; /* ��������ظ� */

    uint16_t step_numb = 0; /* ������ */
    uint8_t step_type = 0;  /* �������� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_WORKSTEP_RUNSTA_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ��ȡ�����ż��������� */
    memcpy(&step_numb, pRecv + lenAnalysis + 0, 2); /* ��ȡ������ */
    memcpy(&step_type, pRecv + lenAnalysis + 2, 1); /* ��ȡ�������� */

    /* �����źϷ����ж�(����������תʹ�õ�) */
    if ((step_numb <= SCMD_WORKSTEP_NUM_BEGIN) || (step_numb >= SCMD_WORKSTEP_NUM_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 1; /* �����Ų�ƥ��(����) */
        memcpy(pSend + *lenSend, &step_numb, 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    /* �������ͺϷ����ж� */
    if (step_type >= WORKSTEP_RUNSTA_SET_END)
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 5; /* �����������Ͳ�ƥ��(����) */
        memcpy(pSend + *lenSend, &step_numb, 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    /* �������Ͷ�Ӧ�߼� */
    switch (step_type)
    {
    case WORKSTEP_RUNSTA_SET_STOP: // ֹͣ
    {
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
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

    case WORKSTEP_RUNSTA_SET_PAUSE: // ��ͣ
    {
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                if ((Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) ||
                    (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING))
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

    case WORKSTEP_RUNSTA_SET_START: // ����
    {
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                if (((Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_STOP) ||
                     (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE)) &&
                    (Tier2.workstepCache1[j].Numb != 0) && (Tier2.workstepCache1[j].Type > SCMD_WORKSTEP_TYPE_BEGIN) && (Tier2.workstepCache1[j].Type < SCMD_WORKSTEP_TYPE_FINISH))
                {
                    if (Tier2.record[j].ErrType == 0)
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

    case WORKSTEP_RUNSTA_SET_CONTINUE: // ����
    {
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                if (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_PAUSE)
                {
                    if (Tier2.record[j].ErrType == 0)
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

    case WORKSTEP_RUNSTA_SET_SKIP: // ��ת
    {
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                if (Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
                {
                    if (Tier2.record[j].ErrType == 0)
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
        for (int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                result = FRAME_SCMD_ERR;
                scmd_reply = 3; /* �޶�Ӧ������������ */
                memcpy(pSend + *lenSend, &step_numb, 2);
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
    memcpy(pSend + *lenSend, &step_numb, 2);
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
    //  uint8_t scmd = 0;           /* �������� */
    //  uint8_t scmd_offset = 0;    /* ��������ƫ�� */
    //  float   sdat = 0.0f;        /* ������������ */
    uint8_t pcmd_reply = 0; /* ��������ظ� */
    uint8_t scmd_reply = 0; /* ��������ظ� */

    uint16_t step_numb = 0; /* ������ */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_WORKSTEP_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ��ȡ������ */
    memcpy(&step_numb, pRecv + lenAnalysis, 2);

    /* �����źϷ����ж� */
    if ((step_numb <= SCMD_WORKSTEP_NUM_BEGIN) || (step_numb >= SCMD_WORKSTEP_NUM_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 0xFF; /* ��ѯ������Խ��(����) */
        memcpy(pSend + *lenSend, &step_numb, 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    for (int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if (unpack.chEn & umask)
        {
            if (step_numb == Tier2.workstepCache1[j].Numb)
            {
                memset(&Tier1.workstep[j], 0, sizeof(Tier1.workstep[j]));
                memcpy(&Tier1.workstep[j], &Tier2.workstepCache1[j], sizeof(Tier2.workstepCache1[j]));
            }
            else if (step_numb == Tier2.workstepCache2[j].Numb)
            {
                memset(&Tier1.workstep[j], 0, sizeof(Tier1.workstep[j]));
                memcpy(&Tier1.workstep[j], &Tier2.workstepCache2[j], sizeof(Tier2.workstepCache2[j]));
            }
            else /* ������ѯ��ƥ�� */
            {
                SET_1(scmd_reply, j);
            }

            /* ��һͨ��������ѯ��ƥ�� */
            if ((j == (CHANNEL_CFG - 1)) && (scmd_reply != 0))
            {
                result = FRAME_SCMD_ERR;
                memcpy(pSend + *lenSend, &step_numb, 2);
                *lenSend += 2;
                memcpy(pSend + *lenSend, &scmd_reply, 1);
                *lenSend += 1;
                return result;
            }
        }
    }

    for (int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if (unpack.chEn & umask)
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
    uint8_t scmd = 0;        /* �������� */
    uint8_t scmd_offset = 0; /* ��������ƫ�� */
    float sdat = 0.0f;       /* ������������ */
    uint8_t pcmd_reply = 0;  /* ��������ظ� */
    uint8_t scmd_reply = 0;  /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_CALIBRATE_PARAM_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1); /* ���·��ͻ������������� */
    *lenSend += 1;

    /* ���ö��׽ṹ����������һ�׽ṹ������(����ṹ�岿�����ݵ�д����Ӱ����������) */
    for (int j = 0; j < CHANNEL_MAX; j++)
    {
        uint8_t umask = 0x01 << j;
        if (unpack.chEn & umask)
        {
            memset(&Tier1.calibrate[j], 0, sizeof(Tier1.calibrate[j]));
            memcpy(&Tier1.calibrate[j], &Tier2.calibrate[j], sizeof(Tier2.calibrate[j]));
        }
    }

    /* ���ܶ����ݽ��� */
    for (int i = lenAnalysis; i < (lenRecv - 4); i = i + 5)
    {
        memcpy(&scmd, pRecv + i + 0, 1); /* �������� */
        memcpy(&sdat, pRecv + i + 1, 4); /* ������������ */

        /* ��������������Ϸ����ж� */
        if ((scmd <= SCMD_CALIBRATE_PARAM_SET_BEGIN) || (scmd >= SCMD_CALIBRATE_PARAM_SET_END))
        {
            scmd_reply = 0xFF;
            memcpy(pSend + *lenSend, &scmd, 1);
            *lenSend += 1;
            memcpy(pSend + *lenSend, &scmd_reply, 1);
            *lenSend += 1;
            if (scmd != SCMD_INVALID) /* ����Ч�������� */ /* ��Ч��Ϊ���쳣�� */
            {
                result = FRAME_SCMD_ERR; /* ��������Ϊ�쳣�� */
            }
            continue; /* �������벻ƥ��(��ת��һ��) */
        }

        /* ��������������ƫ�Ƽ��� */
        scmd_offset = (scmd - SCMD_CALIBRATE_PARAM_SET_BEGIN) - 1;

        /* �����������������ݴ��� */
        for (int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                float *sdat_addr = &Tier1.calibrate[j].VsenseK_C + scmd_offset;
                memcpy(sdat_addr, &sdat, 4);
            }
        }
        scmd_reply = 0;
        memcpy(pSend + *lenSend, &scmd, 1);
        *lenSend += 1;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
    }

    /* ����������� */
    if (result == 0) /* ��ȷ���� */
    {
        for (int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 0x01 << j;
            if (unpack.chEn & umask)
            {
                /* ���¶��׽ṹ������(�������������) */
                memset(&Tier2.calibrate[j], 0, sizeof(Tier2.calibrate[j]));
                memcpy(&Tier2.calibrate[j], &Tier1.calibrate[j], sizeof(Tier1.calibrate[j]));
            }
        }
        MB85Ctrl_Write(0x00, &Tier2.calibrate, sizeof(Tier2.calibrate)); //�����׽ṹ������д��FRAM
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
    uint8_t scmd = 0;        /* �������� */
    uint8_t scmd_offset = 0; /* ��������ƫ�� */
                             //  float   sdat = 0.0f;        /* ������������ */
    uint8_t pcmd_reply = 0;  /* ��������ظ� */
    float scmd_reply = 0.0f; /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_CALIBRATE_PARAM_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ���ܶ����ݽ��� */
    for (int j = 0; j < CHANNEL_MAX; j++)
    {
        uint8_t umask = 0x01 << j;
        if (unpack.chEn & umask)
        {
            /* ����һ�׽ṹ������(���ڻظ�) */
            memcpy(&Tier1.calibrate[j], &Tier2.calibrate[j], sizeof(Tier2.calibrate[j]));

            for (int i = lenAnalysis; i < (lenRecv - 4); i = i + 1)
            {
                memcpy(&scmd, pRecv + i, 1);

                /* ��������������Ϸ����ж� */
                if ((scmd <= SCMD_CALIBRATE_PARAM_INQ_BEGIN) || (scmd >= SCMD_CALIBRATE_PARAM_INQ_END))
                {
                    scmd_reply = 0.0f;
                    memcpy(pSend + *lenSend, &scmd, 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &scmd_reply, 4);
                    *lenSend += 4;
                    if (scmd != SCMD_INVALID) /* ����Ч�������� */ /* ��Ч��Ϊ���쳣�� */
                    {
                        result = FRAME_SCMD_ERR; /* ��������Ϊ�쳣�� */
                    }
                    continue; /* �������벻ƥ��(��ת��һ��) */
                }

                /* ��������ƫ���� */
                scmd_offset = (scmd - SCMD_CALIBRATE_PARAM_INQ_BEGIN) - 1;
                /* ���������� */
                scmd_reply = *(&Tier1.calibrate[j].VsenseK_C + scmd_offset);
                /* �������� */
                memcpy(pSend + *lenSend, &scmd, 1);
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
    uint8_t scmd = 0;        /* �������� */
    uint8_t scmd_offset = 0; /* ��������ƫ�� */
                             //  float   sdat = 0.0f;        /* ������������ */
    uint8_t pcmd_reply = 0;  /* ��������ظ� */
    uint32_t scmd_reply = 0; /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_AD_DATA_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    for (int j = 0; j < CHANNEL_MAX; j++)
    {
        uint8_t umask = 0x01 << j;
        if (unpack.chEn & umask)
        {
            for (int i = lenAnalysis; i < (lenRecv - 4); i = i + 1)
            {
                memcpy(&scmd, pRecv + i, 1);

                /* ��������������Ϸ����ж� */
                if ((scmd <= SCMD_AD_DATA_INQ_BEGIN) || (scmd >= SCMD_AD_DATA_INQ_END))
                {
                    scmd_reply = 0xFFFFFFFF;
                    memcpy(pSend + *lenSend, &scmd, 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &scmd_reply, 4);
                    *lenSend += 4;
                    if (scmd != SCMD_INVALID) /* ����Ч�������� */ /* ��Ч��Ϊ���쳣�� */
                    {
                        result = FRAME_SCMD_ERR; /* ��������Ϊ�쳣�� */
                    }
                    continue; /* �������벻ƥ��(��ת��һ��) */
                }

                /* ��������ƫ���� */
                scmd_offset = (scmd - SCMD_AD_DATA_INQ_BEGIN) - 1;
                /* ���������� */
                scmd_reply = *(&Tier2.adSum[j].VsenseAdSum + scmd_offset);
                /* �������� */
                memcpy(pSend + *lenSend, &scmd, 1);
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
    uint8_t scmd = 0;       /* �������� */
                            //  uint8_t scmd_offset = 0;    /* ��������ƫ�� */
                            //  float   sdat = 0.0f;        /* ������������ */
    uint8_t pcmd_reply = 0; /* ��������ظ� */
    uint8_t scmd_reply = 0; /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_SYSTEM_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ����������ظ����ص����ͻ����� */
    memcpy(&scmd, pRecv + lenAnalysis, 1);
    memcpy(pSend + *lenSend, &scmd, 1);
    *lenSend += 1;

    /* ��������������Ϸ����ж� */
    if ((scmd <= SCMD_SYSTEM_INQ_BEGIN) || (scmd >= SCMD_SYSTEM_INQ_END))
    {
        result = FRAME_SCMD_ERR; /* ��������Ϊ�쳣�� */
        scmd_reply = 0x01;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    switch (scmd)
    {
    case SCMD_SOFTWARE_VERSION: /* ����汾��ѯ */
    {
        float version_tmp = SOFTWARE_PRIVERSION;

        memcpy(pSend + *lenSend, &version_tmp, 4);
        *lenSend += 4;
        *(pSend + *lenSend) = SOFTWARE_SUBVERSION;
        *lenSend += 1;
    }
    break;

    case SCMD_WARMMING_MESSAGE: /* �澯��Ϣ��ѯ */
    {
    }
    break;

    case SCMD_SYSTEM_MODE: /* ϵͳģʽ��ѯ */
    {
    }
    break;

    case SCMD_SYSTEM_CLIENT: /* �ͻ���Զ�����ò�ѯ */
    {
    }
    break;

    case SCMD_SYSTEM_MAC_Q: /* MAC���ò�ѯ */
    {
    }
    break;

    case SCMD_SYSTEM_BAR_Q: /* �������ò�ѯ */
    {
        uint8_t SN_CODE_TMP[9] = {'J', 'N', '9', '1', '0', 'T', 'E', 'S', 'T'};
        memcpy(pSend + *lenSend, SN_CODE_TMP, 9);
        *lenSend += 9;
    }
    break;

    case SCMD_SYSTEM_FRAM_Q: /* �������ò�ѯ */
    {
    }
    break;

    case SCMD_SYSTEM_EE_Q: /* EE���ò�ѯ */
    {
    }
    break;

    case SCMD_SYSTEM_FLASH_Q: /* FLASH���ò�ѯ */
    {
    }
    break;

    case SCMD_SYSTEM_PG_Q: /* PG��״̬��ѯ */
    {
    }
    break;

    case SCMD_SYSTEM_AC_Q: /* AC���ѹ��ѯ */
    {
    }
    break;

    case SCMD_SYSTEM_ACDC_VERSION_Q: /* ACDC�汾��ѯ */
    {
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
**  �� �� �� : SystemSetAnalysis()
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t SystemSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;       /* �������� */
                            //  uint8_t scmd_offset = 0;    /* ��������ƫ�� */
    uint8_t sdat = 0;       /* ������������ */
    uint8_t pcmd_reply = 0; /* ��������ظ� */
    uint8_t scmd_reply = 0; /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_SYSTEM_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ����������ظ����ص����ͻ����� */
    memcpy(&scmd, pRecv + lenAnalysis, 1);
    memcpy(pSend + *lenSend, &scmd, 1);
    *lenSend += 1;

    /* ��������������Ϸ����ж� */
    if (((scmd <= SCMD_SYSTEM_SET_BEGIN) || (scmd >= SCMD_SYSTEM_SET_END)) && (scmd != 0x80) && (scmd != 0x81) && (scmd != 0x82))
    {
        result = FRAME_SCMD_ERR; /* ��������Ϊ�쳣�� */
        scmd_reply = SCMD_NG;
    }
    else
    {
        switch (scmd)
        {
        case 0x01: /* ϵͳ�澯��� */
        {
            clear_flag = 1;
        }
        break;

        case 0x02: /* ϵͳʱ���趨 */
        {
        }
        break;

        case 0x03: /* ������ָ�� */
        {
            if(heart_beat_flag == 0x02)
            {
                Reconnection_Flg = 0x01;	//can������־
            }		
            heart_beat_flag = 0;
			heart_beat_cnt = 0;
        }
        break;

        case 0x04: /* ϵͳģʽ���� */
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&sdat, pRecv + lenAnalysis, 1); //������������
            en_SysMode = sdat;
        }
        break;

        case 0x05: /* ʹ���������� */
        {
            for (int j = 0; j < CHANNEL_CFG; j++)
            {
                uint8_t umask = 1 << j;
                if (unpack.chEn & umask)
                {
                    SET_1(upcast_enable, j);
                }
            }
        }
        break;

        case 0x06: /* ��ֹ�������� */
        {
            for (int j = 0; j < CHANNEL_CFG; j++)
            {
                uint8_t umask = 1 << j;
                if (unpack.chEn & umask)
                {
                    SET_0(upcast_enable, j);
                }
            }
        }
        break;

        case 0x07: /* ���������������� */
        {
            float sdat_data_upcast_period = 0.0f;
            memcpy(&sdat_data_upcast_period, pRecv + lenAnalysis + 1, 4);
            data_upcast_period = (sdat_data_upcast_period != 0.0f) ? sdat_data_upcast_period : 1.0f;
        }
        break;

        case 0x08: /* ϵͳ��ʼ�� */
        {
        }
        break;

        case 0x09: /* ϵͳ��ֹ */
        {
        }
        break;

        case 0x0a: /* ʹ�������� */
        {
        }
        break;

        case 0x0b: /* ��ֹ������ */
        {
        }
        break;

        case 0x0c: /* ̽��δѹ�� */
        {
        }
        break;

        case 0x0d: /* ̽��ѹ���� */
        {
        }
        break;

        case 0x0e: /* ̽����ѹ�� */
        {
        }
        break;

        case 0x0f: /* ���ÿͻ���ģʽ��,Զ��IP�˿�ֵ,��������ģʽ��Ч */
        {
        }
        break;

        case 0x10: /* ��������(MAC��3�ֽ�) */
        {
        }
        break;

        case 0x11: /* ������������ */
        {
        }
        break;

        case 0x12: /* ����EE���� */
        {
        }
        break;

        case 0x13: /* ����FLASH���� */
        {
        }
        break;

        case 0x14:
        {
            // SW1_ON
        }
        break;

        case 0x15: /* ǿ����·���� */
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&st_DebugParameter.MainRoadCtr, pRecv + lenAnalysis, 1); //������������
        }
        break;

        case 0x16: /* ǿ����·���� */
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&st_DebugParameter.ByPassCtr, pRecv + lenAnalysis, 1); //������������
        }
        break;

        case 0x17: /* ��·��·�л� */
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&st_DebugParameter.RoadSwitch, pRecv + lenAnalysis, 1); //������������
            st_DebugParameter.RoadSwitch++;
        }
        break;

        case 0x18: /* DA������ֵ */
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&st_DebugParameter.DAMax, pRecv + lenAnalysis, 4); //������������
        }
        break;

        case 0x19: /* DA����趨ֵ */
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&st_DebugParameter.DASet, pRecv + lenAnalysis, 4); //������������
        }
        break;

        case 0x1A: /* DA�������ʱ�� */
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&st_DebugParameter.DAUpTime, pRecv + lenAnalysis, 2); //������������
        }
        break;

        case 0x1B: /* DA�������ʱ�� */
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&st_DebugParameter.DAHoldTime, pRecv + lenAnalysis, 2); //������������
        }
        break;

        case 0x1C: /* DA�ض���ʱʱ�� */
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&st_DebugParameter.DADelayTime, pRecv + lenAnalysis, 2); //������������
        }
        break;

        case 0x1D: /* ��·��·�л�����ʱ��*/
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&st_DebugParameter.DeadTime, pRecv + lenAnalysis, 2); //������������
        }
        break;

        case 0x1E:
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&st_DebugParameter.SetVsense, pRecv + lenAnalysis, 4); //������������
        }
        break;

        case 0x1F:
        {
            lenAnalysis = lenAnalysis + 1;
            memcpy(&st_DebugParameter.SetVtz, pRecv + lenAnalysis, 4);   //������������
        }
        break;

        case 0x80: /* ��λ */
        {
        }
        break;

        case 0x81: /* ������ģʽ */
        {
        }
        break;

        case 0x82: /* �ͻ���ģʽ */
        {
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
    uint8_t pcmd_reply = 0; /* ��������ظ� */
    uint8_t scmd_reply = 0; /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_DEBUG_PARAM_SET;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* �����Բ�������ֵ�浽�ṹ����� */
    memcpy(&Tier2.debug.set, pRecv + lenAnalysis, sizeof(Tier2.debug.set));

    /* ���ò����Ϸ���У�� */
    if ((Tier2.debug.set.Vtemp <= 7.0f) && (Tier2.debug.set.Itemp <= 160.0f))
    {
        scmd_reply = SCMD_OK; /* ���·��ͻ������ظ�����(ACK) */
    }
    else /* ���ò����Ƿ� */
    {
        result = FRAME_DATA_ERR;
        scmd_reply = SCMD_NG; /* ���·��ͻ������ظ�����(NAK) */
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
    uint8_t pcmd_reply = 0; /* ��������ظ� */
                            //	uint8_t scmd_reply = 0;		/* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_DEBUG_DATA_INQ;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    if (unpack.chEn == 0x00) /* ��ͨ����ѯ */
    {
        for (int j = 0; j < CHANNEL_MAX; j++)
        {
            memcpy(pSend + *lenSend, &Tier2.debug.inq1[j], sizeof(Tier2.debug.inq1[j]));
            *lenSend += sizeof(Tier2.debug.inq1[j]);
        }
        memcpy(pSend + *lenSend, &Tier2.debug.inq2, sizeof(Tier2.debug.inq2));
        *lenSend += sizeof(Tier2.debug.inq2);
    }
    else /* ��ͨ����ѯ */
    {
        for (int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 1 << j;

            if (unpack.chEn & umask)
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
    uint8_t scmd = 0;       /* �������� */
                            //	uint8_t scmd_offset = 0;	/* ��������ƫ�� */
    uint8_t sdat = 0;       /* ������������ */
    uint8_t pcmd_reply = 0; /* ��������ظ� */
    uint8_t scmd_reply = 0; /* ��������ظ� */

    /* ����������ظ����ص����ͻ����� */
    pcmd_reply = CMD_DEBUG_SYSTEM_SET;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* ��ȡ�������뼰������������ */
    memcpy(&scmd, pRecv + lenAnalysis + 0, 1);
    memcpy(&sdat, pRecv + lenAnalysis + 1, 1);

    if ((scmd >= SCMD_DEBUG_SYSTEM_SET_BEGIN) && (scmd <= SCMD_DEBUG_SYSTEM_SET_END))
    {
        scmd_reply = SCMD_OK; /* ���·��ͻ�����������������(ACK) */
    }
    else
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = SCMD_NG; /* ���·��ͻ�����������������(NAK) */
    }

    memcpy(pSend + *lenSend, &scmd, 1);
    *lenSend += 1;
    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

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
**  �� �� �� : ProtocolDataProcess()
**  ����˵�� : Э�����ݴ�����
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/
uint8_t ProtocolDataProcess(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t pcmd = 0; /* �������� */

    /* 1. ��μ�� */
    if (pRecv == NULL) /* ����������׵�ַָ��Ϊ�� */
    {
        return FRAME_PARAM_ERR; /* ֡�βδ��� */
    }

    if (pSend == NULL) /* �����������׵�ַָ��Ϊ�� */
    {
        return FRAME_PARAM_ERR; /* ֡�βδ��� */
    }

    if (lenRecv < 10) /* ��������ݳ����쳣 */
    {
        return FRAME_PARAM_ERR; /* ֡�βδ��� */
    }

    if (lenSend == NULL) /* ���������ݳ��ȵ�ַָ��Ϊ�� */
    {
        return FRAME_PARAM_ERR; /* ֡�βδ��� */
    }

    /* 2. ���У�� */
    pack.sta = RecvUnpack(pRecv, lenRecv);

    if (ucProtocolVersion == 0) /* �ɰ汾Э�� */
    {
        lenAnalysis = 5; /* ֡ͷ(2 Byte) + ֡��(2 Byte) + ͨ����(1 Byte) */

        *lenSend = 5;
    }
    else /* �°汾Э�� */
    {
        lenAnalysis = 7; /* ֡ͷ(2 Byte) + ֡��(2 Byte) + ������(1 Byte) + ״̬��(1 Byte) + ͨ����(1 Byte) */

        *lenSend = 7;
    }

    if (pack.sta != FRAME_IS_OK) /* У���쳣 */
    {
        SendPack(pSend, lenSend); /* �쳣�ظ� */

        return pack.sta;
    }

    memcpy(&pcmd, pRecv + lenAnalysis, 1); /* �������� */

    lenAnalysis += 1; /* �������� */

    /* 3. Э����� */
    switch (pcmd)
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
    }
    break;

    /* ACDC����*/
    case CMD_ACDC_UPDATE:
    {
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
    }
    break;

    default:
    {
        pack.sta = DefaultAnalyze(pSend, lenSend, pcmd);
    }
    break;
    }

    if ((pcmd != CMD_SYSTEM_UPDATE) ||
        (pcmd != CMD_ACDC_UPDATE) ||
        (pcmd != CMD_DEBUG_PARAM_SET) ||
        (pcmd != CMD_DEBUG_DATA_INQ) ||
        (pcmd != CMD_DEBUG_SYSTEM_SET) ||
        (pcmd != CMD_BOOT_UPDATE))
    {
    }

    SendPack(pSend, lenSend); /* ������� */

    return pack.sta;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
