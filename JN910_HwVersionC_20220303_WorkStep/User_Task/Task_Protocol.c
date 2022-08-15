/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Protocol.c
** Author      : Huang Cheng
** Date        : 2022-03-07
** Description : 协议解析任务模块
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t  g_ucSysMode  = SYS_NORMAL_MODE;
uint8_t  ucProtocolVersion = 1;     /* 0 - old version   1 - new version   default - new version */

//原app_timer_periodic_adc3中定义
int64_t Tstamp = 0;

uint16_t lenAnalysis = 0;
s_Tier1_Struct Tier1;
s_Tier2_Struct Tier2;
s_ProtocolStruct unpack;
s_ProtocolStruct pack;
s_SystemStruct SystemStatus;   //系统设置状态

uint8_t workstep_flag_RunstaSet = 0;
uint8_t refresh_continue[8] ={1, 1, 1, 1, 1, 1, 1, 1};
uint16_t debug_ch_onoff_state = 0;
uint8_t heart_beat_flag = 0;
uint8_t upcast_enable = 0xFF;   /* 全部通道使能上抛 */
__IO float data_upcast_period = 1.0f;   /* 数据上抛时间 */
uint8_t  feed_hardware_watchdog_enable = 1;

uint8_t record_data_upcast_flag[8] = {0};		/* 数据上抛标识 */

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void AppTaskProtocol(void *argument);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/**********************************************************************************************************
**  函 数 名 : RecvUnpack()
**  功能说明 : 接收拆包函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
static uint8_t RecvUnpack(uint8_t *pRecv, uint16_t lenRecv)
{
    /*
    ** 旧版本：帧头(2 Byte) + 帧长(2 Byte) + 通道号(1 Byte) + 主命令码(1 Byte) + ...... + 校验码(2 Byte) + 帧尾(2 Byte)
    **
    ** 新版本：帧头(2 Byte) + 帧长(2 Byte) + 特征码(1 Byte) + 状态码(1 Byte) + 通道号(1 Byte) + 主命令码(1 Byte) + ...... + 校验码(2 Byte) + 帧尾(2 Byte)
    */
    if(*(pRecv + 5) != 0)           /* 非零则为主命令码 */
    {
        ucProtocolVersion = 0;              /* 旧版本协议 */
    }
    else                            /* 为零则为状态码 */
    {
        ucProtocolVersion = 1;              /* 新版本协议 */
    }

    if(ucProtocolVersion == 0)      /* 旧版本协议 */
    {
        /* 注: 使用memcpy()直接赋值适用于小端模式 */
        memcpy(&unpack.head, pRecv + 0, 2);             /* 帧头 */
        memcpy(&unpack.len , pRecv + 2, 2);             /* 帧长 */
        memcpy(&unpack.chEn, pRecv + 4, 1);             /* 通道号 */
        memcpy(&unpack.crc , pRecv + lenRecv - 4, 2);   /* 校验码 */
        memcpy(&unpack.tail, pRecv + lenRecv - 2, 2);   /* 帧尾 */
    }
    else                            /* 新版本协议 */
    {
        /* 注: 使用memcpy()直接赋值适用于小端模式 */
        memcpy(&unpack.head, pRecv + 0, 2);             /* 帧头 */
        memcpy(&unpack.len , pRecv + 2, 2);             /* 帧长 */
        memcpy(&unpack.feat, pRecv + 4, 1);             /* 特征码 */
        memcpy(&unpack.sta , pRecv + 5, 1);             /* 状态码 */
        memcpy(&unpack.chEn, pRecv + 6, 1);             /* 通道号 */
        memcpy(&unpack.crc , pRecv + lenRecv - 4, 2);   /* 校验码 */
        memcpy(&unpack.tail, pRecv + lenRecv - 2, 2);   /* 帧尾 */
    }

    /* 帧头判断 */
    if(unpack.head != FRAME_HEAD_RECV)
    {
        return FRAME_HEAD_ERR;      /* 帧头错误 */
    }

    /* 帧长判断 */  /* 注: 数据长度不包括帧头(2Byte)和帧尾(2Byte) */
    if(unpack.len != (lenRecv - 4))
    {
        return FRAME_LEN_ERR;       /* 帧长错误 */
    }

    if(ucProtocolVersion == 0)      /* 旧版本协议 */
    {
        /* 校验码判断 */    /* 注: 从数据长度到功能数据段结束 */
        if(unpack.crc != CRC16N(pRecv + 2, lenRecv - 6))
        {
            //          return FRAME_CRC_ERR;   /* 校验码错误 */
        }

        /* 帧尾判断 */
        if(unpack.tail != FRAME_TAIL_RECV)
        {
            return FRAME_TAIL_ERR;  /* 帧尾错误 */
        }
    }
    else                            /* 新版本协议 */
    {
        /* 特征码判断 */    /* 注: 相邻两帧数据特征码必须不同且特征码不能为0 */
        if(unpack.feat == pack.feat)
        {
			if(g_ucSysMode == SYS_NORMAL_MODE)
			{
				return FRAME_FEAT_ERR;  /* 特征码错误 */
			}
        }

        /* 状态码判断 */
        if(unpack.sta != 0)
        {
            return FRAME_STATE_ERR; /* 状态码错误 */
        }

        /* 校验码判断 */    /* 注: 从数据长度到功能数据段结束 */
        if(unpack.crc != CRC16N(pRecv + 2, lenRecv - 6))
        {
            //          return FRAME_CRC_ERR;   /* 校验码错误 */
        }

        /* 帧尾判断 */
        if(unpack.tail != FRAME_TAIL_RECV)
        {
            return FRAME_TAIL_ERR;  /* 帧尾错误 */
        }
    }

    return FRAME_IS_OK;             /* 帧解析OK */
}

/**********************************************************************************************************
**  函 数 名 : GlobalProtectSetAnalysis()
**  功能说明 : 全局保护设置解析函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t GlobalProtectSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* 子命令码 */
    uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint8_t scmd_reply = 0;     /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_GLOBAL_PROTECT_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    s_Tier1_ProtectStruct *p1;      /* 一阶结构体指针(存放未处理的功能段数据) */
    s_Tier2_ProtectStruct *p2;      /* 二阶结构体指针(存放已处理的功能段数据) */

    /* 利用二阶结构体数据重置一阶结构体数据(避免结构体部分数据的写操作影响其余数据) */
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

    /* 功能段数据解析 */
    for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 5)
    {
        memcpy(&scmd, pRecv + i + 0, 1);    /* 子命令码 */
        memcpy(&sdat, pRecv + i + 1, 4);    /* 子命令码数据 */

        /* 待处理子命令码合法性判断 */
        if(!((scmd > SCMD_GLOBAL_PROTECT_SET_BEGIN) && (scmd < SCMD_GLOBAL_PROTECT_SET_END)) && \
                !((scmd > SCMD_GLOBAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_GLOBAL_PROTECT_NSET_END)))
        {
            scmd_reply = 0xFF;
            memcpy(pSend + *lenSend, &scmd      , 1);
            *lenSend += 1;
            memcpy(pSend + *lenSend, &scmd_reply, 1);
            *lenSend += 1;
            if(scmd != SCMD_INVALID)    /* 非无效子命令码 */    /* 无效码为非异常码 */
            {
                result = FRAME_SCMD_ERR;                    /* 子命令码为异常码 */
            }
            continue;                                       /* 子命令码不匹配(跳转下一个) */
        }

        /* 待处理子命令码偏移计算 */
        if((scmd > SCMD_GLOBAL_PROTECT_SET_BEGIN) && (scmd < SCMD_GLOBAL_PROTECT_SET_END))
        {
            scmd_offset = (scmd - SCMD_GLOBAL_PROTECT_SET_BEGIN) - 1;
        }
        else
            if((scmd > SCMD_GLOBAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_GLOBAL_PROTECT_NSET_END))
            {
                scmd_offset = (scmd - SCMD_GLOBAL_PROTECT_NSET_BEGIN) - 1;
            }

        /* 待处理子命令码数据处理 */
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                p1 = &Tier1.gProt[j].VbatThrH;
                p2 = &Tier2.gProt[j].VbatThrH;

                /* 指定该段对应的结构体成员变量 */
                uint8_t *scmd_addr = &(p1 + scmd_offset)->cmd;
                float   *sdat_addr = &(p1 + scmd_offset)->dat;
                float   *smin_addr = &(p2 + scmd_offset)->min;
                float   *smax_addr = &(p2 + scmd_offset)->max;
                /* 将数据拷贝到对应的结构体成员变量中 */
                memcpy(scmd_addr, &scmd, 1);
                memcpy(sdat_addr, &sdat, 4);
                if((*scmd_addr > SCMD_GLOBAL_PROTECT_SET_BEGIN) && (*scmd_addr < SCMD_GLOBAL_PROTECT_SET_END))                  /* 保护使能 */
                {
                    if((*smin_addr == 0) && (*smax_addr == 0))                          /* 不校验 */
                    {
                        SET_0(scmd_reply, j);
                    }
                    else
                        if((*sdat_addr >= *smin_addr) && (*sdat_addr <= *smax_addr))   /* 校验成功 */
                        {
                            SET_0(scmd_reply, j);
                        }
                        else                                                                /* 校验失败 */
                        {
                            SET_1(scmd_reply, j);
                            result = FRAME_SCMD_ERR;
                        }
                }
                else /* if((*scmd_addr > SCMD_GLOBAL_PROTECT_NSET_BEGIN) && (*scmd_addr < SCMD_GLOBAL_PROTECT_NSET_END)) */     /* 保护失能 */
                {
                    SET_0(scmd_reply, j);                                               /* 不校验 */
                }
            }
        }
        memcpy(pSend + *lenSend, &scmd      , 1);
        *lenSend += 1;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
    }

    /* 解析结果处理 */
    if(result == 0)
    {
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                p1 = &Tier1.gProt[j].VbatThrH;
                p2 = &Tier2.gProt[j].VbatThrH;

                /* 更新二阶结构体数据(子命令解析正常) */
                for(int k = SCMD_GLOBAL_PROTECT_SET_BEGIN; k < SCMD_GLOBAL_PROTECT_SET_END - 1; k++, p1++, p2++)
                {
                    p2->isEnable = p1->cmd;
                    p2->value    = p1->dat;
                    /* 存储到EEPROM中的全局保护子命令码必须禁止 */
                    SET_1(p1->cmd, 7);
                }
//                EEP_DAT_SYNC(PROTECT_GLOBAL_ID(j));    //暂未用到，后面再考虑
            }
        }
    }

    return result;
}

/**********************************************************************************************************
**  函 数 名 : GlobalProtectInqAnalysis()
**  功能说明 : 全局保护查询解析函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t GlobalProtectInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* 子命令码 */
    uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    //  float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    float   scmd_reply = 0.0f;  /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_GLOBAL_PROTECT_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 功能段数据解析 */
    for(int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            s_Tier1_ProtectStruct *p1 = &Tier1.gProt[j].VbatThrH;       /* 一阶结构体指针 */
            s_Tier2_ProtectStruct *p2 = &Tier2.gProt[j].VbatThrH;       /* 二阶结构体指针 */
            /* 利用二阶结构体数据更新一阶结构体数据(用于回复) */
            for(int k = SCMD_GLOBAL_PROTECT_INQ_BEGIN; k < SCMD_GLOBAL_PROTECT_INQ_END - 1; k++, p1++, p2++)
            {
                p1->cmd = p2->isEnable;
                p1->dat = p2->value;
            }
            /* 重置指针初始值 */
            p1 = &Tier1.gProt[j].VbatThrH;
            p2 = &Tier2.gProt[j].VbatThrH;

            /* 子命令码段循环解析 */
            for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 1)
            {
                memcpy(&scmd, pRecv + i, 1);

                if(scmd == SCMD_INVALID)    /* 无效码 */
                {
                    if(i == lenAnalysis)    /* 首个子命令码为无效码 - 全部查询 */
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
                else
                    if((scmd <= SCMD_GLOBAL_PROTECT_INQ_BEGIN) || (scmd >= SCMD_GLOBAL_PROTECT_INQ_END))
                    {
                        scmd_reply = 0xFFFFFFFF;
                        memcpy(pSend + *lenSend, &scmd      , 1);
                        *lenSend += 1;
                        memcpy(pSend + *lenSend, &scmd_reply, 4);
                        *lenSend += 4;
                        result = FRAME_SCMD_ERR;    /* 子命令码为异常码 */
                        continue;                   /* 子命令码不匹配(跳转下一个) */
                    }

                /* 子命令码偏移量 */
                scmd_offset = (scmd - SCMD_GLOBAL_PROTECT_INQ_BEGIN) - 1;
                /* 子命令码回复 */
                scmd        = (p1 + scmd_offset)->cmd;
                /* 子命令码数据回复 */
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
**  函 数 名 : InternalProtectSetAnalysis()
**  功能说明 : 内部保护设置解析函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t InternalProtectSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* 子命令码 */
    uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint8_t scmd_reply = 0;     /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_INTERNAL_PROTECT_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    s_Tier1_ProtectStruct *p1;      /* 一阶结构体指针(存放未处理的功能段数据) */
    s_Tier2_ProtectStruct *p2;      /* 二阶结构体指针(存放已处理的功能段数据) */

    /* 利用二阶结构体数据重置一阶结构体数据(避免结构体部分数据的写操作影响其余数据) */
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

    /* 功能段数据解析 */
    for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 5)
    {
        memcpy(&scmd, pRecv + i + 0, 1);    /* 子命令码 */
        memcpy(&sdat, pRecv + i + 1, 4);    /* 子命令码数据 */

        /* 待处理子命令码合法性判断 */
        if(!((scmd > SCMD_INTERNAL_PROTECT_SET_BEGIN) && (scmd < SCMD_INTERNAL_PROTECT_SET_END)) && \
                !((scmd > SCMD_INTERNAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_INTERNAL_PROTECT_NSET_END)))
        {
            scmd_reply = 0xFF;
            memcpy(pSend + *lenSend, &scmd      , 1);
            *lenSend += 1;
            memcpy(pSend + *lenSend, &scmd_reply, 1);
            *lenSend += 1;
            if(scmd != SCMD_INVALID)    /* 非无效子命令码 */    /* 无效码为非异常码 */
            {
                result = FRAME_SCMD_ERR;                    /* 子命令码为异常码 */
            }
            continue;                                       /* 子命令码不匹配(跳转下一个) */
        }

        /* 待处理子命令码偏移计算 */
        if((scmd > SCMD_INTERNAL_PROTECT_SET_BEGIN) && (scmd < SCMD_INTERNAL_PROTECT_SET_END))
        {
            scmd_offset = (scmd - SCMD_INTERNAL_PROTECT_SET_BEGIN) - 1;
        }
        else
            if((scmd > SCMD_INTERNAL_PROTECT_NSET_BEGIN) && (scmd < SCMD_INTERNAL_PROTECT_NSET_END))
            {
                scmd_offset = (scmd - SCMD_INTERNAL_PROTECT_NSET_BEGIN) - 1;
            }

        /* 待处理子命令码数据处理 */
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                p1 = &Tier1.iProt[j].IoutThr;
                p2 = &Tier2.iProt[j].IoutThr;

                /* 指定该段对应的结构体成员变量 */
                uint8_t *scmd_addr = &(p1 + scmd_offset)->cmd;
                float   *sdat_addr = &(p1 + scmd_offset)->dat;
                float   *smin_addr = &(p2 + scmd_offset)->min;
                float   *smax_addr = &(p2 + scmd_offset)->max;
                /* 将数据拷贝到对应的结构体成员变量中 */
                memcpy(scmd_addr, &scmd, 1);
                memcpy(sdat_addr, &sdat, 4);
                if((*scmd_addr > SCMD_INTERNAL_PROTECT_SET_BEGIN) && (*scmd_addr < SCMD_INTERNAL_PROTECT_SET_END))              /* 保护使能 */
                {
                    if((*smin_addr == 0) && (*smax_addr == 0))                          /* 不校验 */
                    {
                        SET_0(scmd_reply, j);
                    }
                    else
                        if((*sdat_addr >= *smin_addr) && (*sdat_addr <= *smax_addr))   /* 校验成功 */
                        {
                            SET_0(scmd_reply, j);
                        }
                        else                                                                /* 校验失败 */
                        {
                            SET_1(scmd_reply, j);
                            result = FRAME_SCMD_ERR;
                        }
                }
                else /* if((*scmd_addr > SCMD_INTERNAL_PROTECT_NSET_BEGIN) && (*scmd_addr < SCMD_INTERNAL_PROTECT_NSET_END)) */ /* 保护失能 */
                {
                    SET_0(scmd_reply, j);                                               /* 不校验 */
                }
            }
        }
        memcpy(pSend + *lenSend, &scmd      , 1);
        *lenSend += 1;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
    }

    /* 解析结果处理 */
    if(result == 0)
    {
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                p1 = &Tier1.iProt[j].IoutThr;
                p2 = &Tier2.iProt[j].IoutThr;

                /* 更新二阶结构体数据(子命令解析正常) */
                for(int k = SCMD_INTERNAL_PROTECT_SET_BEGIN; k < SCMD_INTERNAL_PROTECT_SET_END - 1; k++, p1++, p2++)
                {
                    p2->isEnable = p1->cmd;
                    p2->value    = p1->dat;
                    /* 存储到EEPROM中的内部保护子命令码必须使能 */
                    SET_0(p1->cmd, 7);
                }
//                EEP_DAT_SYNC(PROTECT_INTERNAL_ID(j));//暂未用到，后面再考虑
            }
        }
    }

    return result;
}

/**********************************************************************************************************
**  函 数 名 : InternalProtectInqAnalysis()
**  功能说明 : 内部保护查询解析函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t InternalProtectInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* 子命令码 */
    uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    //  float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    float   scmd_reply = 0.0f;  /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_INTERNAL_PROTECT_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 功能段数据解析 */
    for(int j = 0; j < CHANNEL_CFG; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            s_Tier1_ProtectStruct *p1 = &Tier1.iProt[j].IoutThr;        /* 一阶结构体指针 */
            s_Tier2_ProtectStruct *p2 = &Tier2.iProt[j].IoutThr;        /* 二阶结构体指针 */
            /* 利用二阶结构体数据更新一阶结构体数据(用于回复) */
            for(int k = SCMD_INTERNAL_PROTECT_INQ_BEGIN; k < SCMD_INTERNAL_PROTECT_INQ_END - 1; k++, p1++, p2++)
            {
                p1->cmd = p2->isEnable;
                p1->dat = p2->value;
            }
            /* 重置指针初始值 */
            p1 = &Tier1.iProt[j].IoutThr;
            p2 = &Tier2.iProt[j].IoutThr;

            for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 1)
            {
                memcpy(&scmd, pRecv + i, 1);

                if(scmd == SCMD_INVALID)
                {
                    if(i == lenAnalysis)    /* 首个子命令码为无效码 - 全部查询 */
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
                else
                    if((scmd <= SCMD_INTERNAL_PROTECT_INQ_BEGIN) || (scmd >= SCMD_INTERNAL_PROTECT_INQ_END))
                    {
                        scmd_reply = 0xFFFFFFFF;
                        memcpy(pSend + *lenSend, &scmd      , 1);
                        *lenSend += 1;
                        memcpy(pSend + *lenSend, &scmd_reply, 4);
                        *lenSend += 4;
                        result = FRAME_SCMD_ERR;        /* 子命令码为异常码 */
                        continue;                       /* 子命令码不匹配(跳转下一个) */
                    }

                /* 子命令码偏移量 */
                scmd_offset = (scmd - SCMD_INTERNAL_PROTECT_INQ_BEGIN) - 1;
                /* 子命令码回复 */
                scmd        = (p1 + scmd_offset)->cmd;
                /* 子命令码数据回复 */
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
**  函 数 名 : RecordDatSync()
**  功能说明 : 记录数据同步函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
void RecordDatSync(uint8_t chx)
{
#if 0
	uint8_t j = chx;
	
	// 1. 时间戳
	Tier2.record[j].Tstamp   = Tstamp;
	// 2. 运行状态
	if(start_state_flag[j] == 2)
	{
		start_state_flag[j] = 0;
		Tier2.record[j].RunSta = WORKSTEP_RUNSTA_REF_START;
	}
	else
	{
		Tier2.record[j].RunSta = Tier2.workstepRunning[j].RunSta;
	}
	// 3. 工步类型
	if(Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_START)
	{
		Tier2.record[j].StepType = Tier2.workstepCache1[j].Type;	// 避免发生工艺流程开始帧时，偶然出现工步类型为空的情况
	}
	else
	{
		Tier2.record[j].StepType = Tier2.workstepRunning[j].StepType;
	}
	// 4. 工步号
	Tier2.record[j].StepNumb = Tier2.workstepRunning[j].StepNumb;
	// 5. 运行时间
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
	// 6. 电池电压
	Tier2.record[j].Vbat     = Tier2.chRecord[j].Vbat;
	// 7. 输出电流(正充负放)
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
	// 8. 输出电压
	Tier2.record[j].Vout     = Tier2.chRecord[j].Vout;
	// 9. 探针电压
	Tier2.record[j].Vtz      = Tier2.chRecord[j].Vtz;
	//10. 电池容量
	Tier2.record[j].Cbat     = Tier2.chRecord[j].Cbat;
	//11. 电池能量
	Tier2.record[j].Ebat     = Tier2.chRecord[j].Ebat;
	//12. 模式标识
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
	//13. 故障类型
	if(Protect_Flag_Global[j] != 0)						// 全局保护
	{
		SET_1(Tier2.record[j].ErrType, 0);
	}
	else
	{
		SET_0(Tier2.record[j].ErrType, 0);
	}
	if(Protect_Flag_Workstep[j] != 0)					// 单工步保护
	{
		SET_1(Tier2.record[j].ErrType, 1);
	}
	else
	{
		SET_0(Tier2.record[j].ErrType, 1);
	}
	if(Protect_Flag_Internal[j] != 0)					// 内部保护
	{
		SET_1(Tier2.record[j].ErrType, 2);
	}
	else
	{
		SET_0(Tier2.record[j].ErrType, 2);
	}
	
	if((ACDC.Type == 1) || (ACDC.Type == 11))			// 国电
	{
		if((ACDC.State[0] & 0x0FF8) || (ACDC.State[1] & 0x0FF8))	// ACDC保护
		{
			SET_1(Tier2.record[j].ErrType, 3);
		}
		else
		{
			SET_0(Tier2.record[j].ErrType, 3);
		}
	}
	else if((ACDC.Type == 2) || (ACDC.Type == 22))		// 普德
	{
		if((ACDC.State[0] & 0xBFF0) || (ACDC.State[1] & 0xBFF0))	// ACDC保护
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
#endif
}

/**********************************************************************************************************
**  函 数 名 : RecordInqAnalysis()
**  功能说明 : 记录数据查询解析函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t RecordInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    //  uint8_t scmd = 0;           /* 子命令码 */
    //  uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    //  float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    //  uint8_t scmd_reply = 0;     /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_RECORD_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 功能段数据解析 */
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
**  函 数 名 : WorkstepSetAnalysis()
**  功能说明 : 工步设置解析函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t WorkstepSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    //  uint8_t scmd = 0;           /* 子命令码 */
    //  uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    //  float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint8_t scmd_reply = 0;     /* 子命令码回复 */

    uint16_t step_numb  = 0;    /* 工步号 */
    uint8_t  step_type  = 0;    /* 工步类型 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_WORKSTEP_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 获取工步号及工步类型 */
    memcpy(&step_numb, pRecv + lenAnalysis + 0, 2);     /* 获取工步号 */
    memcpy(&step_type, pRecv + lenAnalysis + 2, 1);     /* 获取工步类型 */

    /* 工步号合法性判断 */
    if((step_numb <= SCMD_WORKSTEP_NUM_BEGIN) || (step_numb >= SCMD_WORKSTEP_NUM_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 1;     /* 工步号不匹配(跳出) */
        memcpy(pSend + *lenSend, &step_numb , 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    /* 工步类型合法性判断 */
    if((step_type <= SCMD_WORKSTEP_TYPE_BEGIN) || (step_type >= SCMD_WORKSTEP_TYPE_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 2;     /* 工步类型不匹配(跳出) */
        memcpy(pSend + *lenSend, &step_numb , 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    if(step_type != SCMD_WORKSTEP_TYPE_FINISH)      /* 非结束工步 */
    {
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                memset(&Tier1.workstep[j], 0, sizeof(Tier1.workstep[j]));           /* 重置一阶结构体(避免1>前次异常解析2>运行帧与结束帧的影响) */  /* Status不需要重置 */
                memcpy(&Tier1.workstep[j], pRecv + lenAnalysis, lenRecv - 12);      /* 将接收的工步信息存放在一阶结构体中 */

                uint8_t data_check = 0;         /* 数据校验 */
                s_WorkStepSegmentStruct *p = &Tier1.workstep[j].RunSeg[0];

                /* 运行工步参数合法性判断 */
                for(int i = 0; i < RUN_SEG_NUM; i++)        /* 运行工步设置 */
                {
                    if(((p + i)->type != SCMD_INVALID) && \
                            (((p + i)->type <= SCMD_WORKSTEP_PARAM_BEGIN) || \
                             ((p + i)->type >= SCMD_WORKSTEP_PARAM_END)))
                    {
                        result = FRAME_SCMD_ERR;
                        scmd_reply = 3;         /* 工步设定类型不匹配(跳出) */
                        memcpy(pSend + *lenSend, &step_numb , 2);
                        *lenSend += 2;
                        memcpy(pSend + *lenSend, &scmd_reply, 1);
                        *lenSend += 1;
                        return result;
                    }
                    /* 恒流充放电工步类型,运行参数仅运行设置电流值 */
                    else
                        if((step_type == SCMD_WORKSTEP_TYPE_CC) || \
                                (step_type == SCMD_WORKSTEP_TYPE_DCC))
                        {
                            if((p + i)->type == SCMD_WORKSTEP_PARAM_CURRENT)
                            {
                                data_check = 2;
                            }
                            else
                                if((p + i)->type == SCMD_INVALID)
                                {
                                    continue;
                                }
                                else
                                {
                                    result = FRAME_SCMD_ERR;
                                    scmd_reply = 3;     /* 工步设定类型不匹配(跳出) */
                                    memcpy(pSend + *lenSend, &step_numb , 2);
                                    *lenSend += 2;
                                    memcpy(pSend + *lenSend, &scmd_reply, 1);
                                    *lenSend += 1;
                                    return result;
                                }
                        }
                    /* 恒压充放电工步类型,运行参数仅运行设置电流值(可选)及电压值(必选) */
                        else
                            if((step_type == SCMD_WORKSTEP_TYPE_CV) || \
                                    (step_type == SCMD_WORKSTEP_TYPE_DCV))
                            {
                                if((p + i)->type == SCMD_WORKSTEP_PARAM_VOLTAGE)
                                {
                                    data_check = 2;
                                }
                                else
                                    if((p + i)->type == SCMD_INVALID)
                                    {
                                        continue;
                                    }
                                    else
                                        if((p + i)->type != SCMD_WORKSTEP_PARAM_CURRENT)
                                        {
                                            result = FRAME_SCMD_ERR;
                                            scmd_reply = 3;     /* 工步设定类型不匹配(跳出) */
                                            memcpy(pSend + *lenSend, &step_numb , 2);
                                            *lenSend += 2;
                                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                                            *lenSend += 1;
                                            return result;
                                        }
                            }
                    /* 恒流恒压充放电工步类型,运行参数仅运行设置电流值(必选)及电压值(必选) */
                            else
                                if((step_type == SCMD_WORKSTEP_TYPE_CCTOCV) || \
                                        (step_type == SCMD_WORKSTEP_TYPE_DCCTODCV))
                                {
                                    if(((p + i)->type == SCMD_WORKSTEP_PARAM_VOLTAGE) || \
                                            ((p + i)->type == SCMD_WORKSTEP_PARAM_CURRENT))
                                    {
                                        data_check++;
                                    }
                                    else
                                        if((p + i)->type == SCMD_INVALID)
                                        {
                                            continue;
                                        }
                                        else
                                        {
                                            result = FRAME_SCMD_ERR;
                                            scmd_reply = 3;     /* 工步设定类型不匹配(跳出) */
                                            memcpy(pSend + *lenSend, &step_numb , 2);
                                            *lenSend += 2;
                                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                                            *lenSend += 1;
                                            return result;
                                        }
                                }
                    /* 静置模式不允许设置运行参数 */
                                else
                                    if(step_type == SCMD_WORKSTEP_TYPE_STANDBY)
                                    {
                                        if((p + i)->type == SCMD_INVALID)
                                        {
                                            data_check = 2;
                                        }
                                        else
                                        {
                                            result = FRAME_SCMD_ERR;
                                            scmd_reply = 3;         /* 工步设定类型不匹配(跳出) */
                                            memcpy(pSend + *lenSend, &step_numb , 2);
                                            *lenSend += 2;
                                            memcpy(pSend + *lenSend, &scmd_reply, 1);
                                            *lenSend += 1;
                                            return result;
                                        }
                                    }
                }

                /* 运行工步参数合法性校验 */
                if(data_check == 2)                         /* 运行工步参数设置正确 */
                {
                    data_check = 0;
                }
                else /* if(data_check != 2) */              /* 运行工步参数设置错误 */
                {
                    result = FRAME_SCMD_ERR;
                    scmd_reply = 3;         /* 工步设定类型不匹配(跳出) */
                    memcpy(pSend + *lenSend, &step_numb , 2);
                    *lenSend += 2;
                    memcpy(pSend + *lenSend, &scmd_reply, 1);
                    *lenSend += 1;
                    return result;
                }
						
                /* 截止工步参数合法性判断 */
				for(int i = RUN_SEG_NUM; i < (RUN_SEG_NUM + ABT_SEG_NUM); i++)
				{
					uint8_t abort_condition = (p + i)->type;
//					uint8_t abort_priority  = (abort_condition >> 4) & 0x0F;
					uint8_t abort_type      = (abort_condition >> 0) & 0x0F;
					
					/* 无效截止条件 */
					if(abort_type == 0x0F)
					{
						continue;
					}
								
					/* 截止类型不符 */
					if((abort_type <= SCMD_WORKSTEP_PARAM_BEGIN) || (abort_type >= SCMD_WORKSTEP_PARAM_END))
					{
						result = FRAME_SCMD_ERR;
						scmd_reply = 3;				/* 工步设定类型不匹配(跳出) */
						memcpy(pSend + *lenSend, &step_numb , 2);
						*lenSend += 2;
						memcpy(pSend + *lenSend, &scmd_reply, 1);
						*lenSend += 1;		
						return result;
					}
					/* 恒流充放电仅运行时间截止、电压截止及容量截止 */
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
							scmd_reply = 3;			/* 工步设定类型不匹配(跳出) */
							memcpy(pSend + *lenSend, &step_numb , 2);
							*lenSend += 2;
							memcpy(pSend + *lenSend, &scmd_reply, 1);
							*lenSend += 1;		
							return result;						
						}
					}
					/* 恒压充放电仅运行时间截止、电流截止及容量截止 */
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
							scmd_reply = 3;			/* 工步设定类型不匹配(跳出) */
							memcpy(pSend + *lenSend, &step_numb , 2);
							*lenSend += 2;
							memcpy(pSend + *lenSend, &scmd_reply, 1);
							*lenSend += 1;		
							return result;								
						}
					}
					/* 恒流恒压充放电仅运行时间截止、电流截止及容量截止 */
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
							scmd_reply = 3;			/* 工步设定类型不匹配(跳出) */
							memcpy(pSend + *lenSend, &step_numb , 2);
							*lenSend += 2;
							memcpy(pSend + *lenSend, &scmd_reply, 1);
							*lenSend += 1;		
							return result;								
						}
					}
					/* 静置模式仅允许设置时间截止 */
					else if(step_type == SCMD_WORKSTEP_TYPE_STANDBY)
					{
						if(abort_type == SCMD_WORKSTEP_PARAM_TIME)
						{
							data_check = 2;
						}					
						else
						{
							result = FRAME_SCMD_ERR;
							scmd_reply = 3;			/* 工步设定类型不匹配(跳出) */
							memcpy(pSend + *lenSend, &step_numb , 2);
							*lenSend += 2;
							memcpy(pSend + *lenSend, &scmd_reply, 1);
							*lenSend += 1;		
							return result;							
						}
					}						
				}

                /* 截止工步参数合法性校验 */
                if(data_check == 2)                         /* 截止工步参数设置正确 */
                {
                    data_check = 0;
                }
                else                                        /* 截止工步参数设置错误 */
                {
                    result = FRAME_SCMD_ERR;
                    scmd_reply = 3;             /* 工步设定类型不匹配(跳出) */
                    memcpy(pSend + *lenSend, &step_numb , 2);
                    *lenSend += 2;
                    memcpy(pSend + *lenSend, &scmd_reply, 1);
                    *lenSend += 1;
                    return result;
                }

                /* 工步保护参数合法性判断 */
                for(int i = (RUN_SEG_NUM + ABT_SEG_NUM); i < (RUN_SEG_NUM + ABT_SEG_NUM + PRO_SEG_NUM); i++)
                {
                    if(!(((p + i)->type > SCMD_WORKSTEP_PROT_BEGIN) && ((p + i)->type < SCMD_WORKSTEP_PROT_END)) && \
                            !(((p + i)->type > SCMD_WORKSTEP_NPROT_BEGIN) && ((p + i)->type < SCMD_WORKSTEP_NPROT_END)))
                    {
                        if((p + i)->type != SCMD_INVALID)
                        {
                            result = FRAME_SCMD_ERR;
                            scmd_reply = 4;     /* 工步保护类型不匹配(跳出) */
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
    else                                            /* 结束工步   */
    {
        for(int j = 0; j < CHANNEL_CFG; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                memset(&Tier1.workstep[j], 0, sizeof(Tier1.workstep[j]));           /* 重置一阶结构体(避免1>前次异常解析2>运行帧与结束帧的影响) */  /* Status不需要重置 */
                memcpy(&Tier1.workstep[j], pRecv + lenAnalysis, lenRecv - 12);      /* 将接收的工步信息存放在一阶结构体中 */
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
            if((Tier2.record[j].ErrType != 0) && (Tier2.record[j].RunSta == WORKSTEP_RUNSTA_REF_WAIT) && (AcDumpConnectFlag == 1))
            {
                return result;
            }
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                if(Tier2.workstepCache1[j].Numb == 0)                               /* 若Cache1为空，将接收工步存储在Cache1 */
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
                else /* if(Tier2.workstepCache2[j].Numb == 0) */                    /* 若Cache1非空，将接收工步存储在Cache2(如果Cache2中有工步信息将被覆盖) */
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
**  函 数 名 : WorkstepRunstaSetAnalysis()
**  功能说明 : 工步状态设置解析函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t WorkstepRunstaSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    //  uint8_t scmd = 0;           /* 子命令码 */
    //  uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    //  float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint8_t scmd_reply = 0;     /* 子命令码回复 */

    uint16_t step_numb  = 0;    /* 工步号 */
    uint8_t  step_type  = 0;    /* 工步类型 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_WORKSTEP_RUNSTA_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 获取工步号及工步类型 */
    memcpy(&step_numb, pRecv + lenAnalysis + 0, 2);     /* 获取工步号 */
    memcpy(&step_type, pRecv + lenAnalysis + 2, 1);     /* 获取工步类型 */

    /* 工步号合法性判断(仅启动和跳转使用到) */
    if((step_numb <= SCMD_WORKSTEP_NUM_BEGIN) || (step_numb >= SCMD_WORKSTEP_NUM_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 1;     /* 工步号不匹配(跳出) */
        memcpy(pSend + *lenSend, &step_numb , 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    /* 工步类型合法性判断 */
    if(step_type >= WORKSTEP_RUNSTA_SET_END)
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 5;     /* 工步运行类型不匹配(跳出) */
        memcpy(pSend + *lenSend, &step_numb , 2);
        *lenSend += 2;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    /* 工步类型对应逻辑 */
    switch(step_type)
    {
        case WORKSTEP_RUNSTA_SET_STOP:      // 停止
        {
            for(int j = 0; j < CHANNEL_CFG; j++)
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

        case WORKSTEP_RUNSTA_SET_PAUSE:     // 暂停
        {
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                uint8_t umask = 0x01 << j;
                if(unpack.chEn & umask)
                {
                    if((Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
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

        case WORKSTEP_RUNSTA_SET_START:     // 启动
        {
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                uint8_t umask = 0x01 << j;
                if(unpack.chEn & umask)
                {
//                  /* 工步号匹配检测 */
//                  if(step_numb != Tier2.workstepCache1[j].Numb)
//                  {
//                      result = FRAME_SCMD_ERR;
//                      scmd_reply = 1;     /* 启动工步不匹配 */
//                      memcpy(pSend + *lenSend, &step_numb , 2);
//                      *lenSend += 2;
//                      memcpy(pSend + *lenSend, &scmd_reply, 1);
//                      *lenSend += 1;
//                      return result;
//                  }
					if(((Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_STOP    ) || \
						(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_COMPLETE) ) && \
						(Tier2.workstepCache1[j].Numb != 0) && (Tier2.workstepCache1[j].Type > SCMD_WORKSTEP_TYPE_BEGIN) && (Tier2.workstepCache1[j].Type < SCMD_WORKSTEP_TYPE_FINISH) )
                    {				
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

        case WORKSTEP_RUNSTA_SET_CONTINUE:  // 继续
        {
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                uint8_t umask = 0x01 << j;
                if(unpack.chEn & umask)
                {
                    if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_PAUSE)
                    {
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

        case WORKSTEP_RUNSTA_SET_SKIP:      // 跳转
        {
            for(int j = 0; j < CHANNEL_CFG; j++)
            {
                uint8_t umask = 0x01 << j;
                if(unpack.chEn & umask)
                {
//                  if(step_numb != Tier2.workstepCache2[j].Numb)
//                  {
//                      result = FRAME_SCMD_ERR;
//                      scmd_reply = 2;     /* 跳转工步不匹配 */
//                      memcpy(pSend + *lenSend, &step_numb , 2);
//                      *lenSend += 2;
//                      memcpy(pSend + *lenSend, &scmd_reply, 1);
//                      *lenSend += 1;
//                      return result;
//                  }
                    if(Tier2.workstepRunning[j].RunSta == WORKSTEP_RUNSTA_REF_RUNNING)
                    {
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

        default
                :
            {
                for(int j = 0; j < CHANNEL_CFG; j++)
                {
                    uint8_t umask = 0x01 << j;
                    if(unpack.chEn & umask)
                    {
                        result = FRAME_SCMD_ERR;
                        scmd_reply = 3;     /* 无对应工步运行类型 */
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
**  函 数 名 : WorkstepInqAnalysis()
**  功能说明 : 工步查询解析函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t WorkstepInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    //  uint8_t scmd = 0;           /* 子命令码 */
    //  uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    //  float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint8_t scmd_reply = 0;     /* 子命令码回复 */

    uint16_t step_numb  = 0;    /* 工步号 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_WORKSTEP_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 获取工步号 */
    memcpy(&step_numb, pRecv + lenAnalysis, 2);

    /* 工步号合法性判断 */
    if((step_numb <= SCMD_WORKSTEP_NUM_BEGIN) || (step_numb >= SCMD_WORKSTEP_NUM_END))
    {
        result = FRAME_SCMD_ERR;
        scmd_reply = 0xFF;      /* 查询工步号越界(跳出) */
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
            else
                if(step_numb == Tier2.workstepCache2[j].Numb)
                {
                    memset(&Tier1.workstep[j], 0, sizeof(Tier1.workstep[j]));
                    memcpy(&Tier1.workstep[j], &Tier2.workstepCache2[j], sizeof(Tier2.workstepCache2[j]));
                }
                else        /* 工步查询不匹配 */
                {
                    SET_1(scmd_reply, j);
                }

            /* 任一通道工步查询不匹配 */
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
**  函 数 名 : CalibrateSetAnalysis()
**  功能说明 : 校准参数设置解析函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t CalibrateSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* 子命令码 */
    uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint8_t scmd_reply = 0;     /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_CALIBRATE_PARAM_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);               /* 更新发送缓存区主命令码 */
    *lenSend += 1;

    /* 利用二阶结构体数据重置一阶结构体数据(避免结构体部分数据的写操作影响其余数据) */
    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            memset(&Tier1.calibrate[j], 0                  , sizeof(Tier1.calibrate[j]));
            memcpy(&Tier1.calibrate[j], &Tier2.calibrate[j], sizeof(Tier2.calibrate[j]));
        }
    }

    /* 功能段数据解析 */
    for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 5)
    {
        memcpy(&scmd, pRecv + i + 0, 1);    /* 子命令码 */
        memcpy(&sdat, pRecv + i + 1, 4);    /* 子命令码数据 */

        /* 待处理子命令码合法性判断 */
        if((scmd <= SCMD_CALIBRATE_PARAM_SET_BEGIN) || (scmd >= SCMD_CALIBRATE_PARAM_SET_END))
        {
            scmd_reply = 0xFF;
            memcpy(pSend + *lenSend, &scmd      , 1);
            *lenSend += 1;
            memcpy(pSend + *lenSend, &scmd_reply, 1);
            *lenSend += 1;
            if(scmd != SCMD_INVALID)    /* 非无效子命令码 */    /* 无效码为非异常码 */
            {
                result = FRAME_SCMD_ERR;                    /* 子命令码为异常码 */
            }
            continue;                                       /* 子命令码不匹配(跳转下一个) */
        }

        /* 待处理子命令码偏移计算 */
        scmd_offset = (scmd - SCMD_CALIBRATE_PARAM_SET_BEGIN) - 1;

        /* 待处理子命令码数据处理 */
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                float *sdat_addr = &Tier1.calibrate[j].VsenseSetK_C + scmd_offset;
                memcpy(sdat_addr, &sdat, 4);
            }
        }
        scmd_reply = 0;
        memcpy(pSend + *lenSend, &scmd      , 1);
        *lenSend += 1;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
    }

    /* 解析结果处理 */
    if(result == 0)     /* 正确解析 */
    {
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                /* 更新二阶结构体数据(子命令解析正常) */
                memset(&Tier2.calibrate[j], 0                  , sizeof(Tier2.calibrate[j]));
                memcpy(&Tier2.calibrate[j], &Tier1.calibrate[j], sizeof(Tier1.calibrate[j]));
//                EEP_DAT_SYNC(CALIBRATE_ID(j));
            }
        }
    }

    return result;
}

/**********************************************************************************************************
**  函 数 名 : CalibrateInqAnalysis()
**  功能说明 : 校准参数查询解析函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t CalibrateInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* 子命令码 */
    uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    //  float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    float   scmd_reply = 0.0f;  /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_CALIBRATE_PARAM_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 功能段数据解析 */
    for(int j = 0; j < CHANNEL_MAX; j++)
    {
        uint8_t umask = 0x01 << j;
        if(unpack.chEn & umask)
        {
            /* 更新一阶结构体数据(用于回复) */
            memcpy(&Tier1.calibrate[j], &Tier2.calibrate[j], sizeof(Tier2.calibrate[j]));

            for(int i = lenAnalysis; i < (lenRecv - 4); i = i + 1)
            {
                memcpy(&scmd, pRecv + i, 1);

                /* 待处理子命令码合法性判断 */
                if((scmd <= SCMD_CALIBRATE_PARAM_INQ_BEGIN) || (scmd >= SCMD_CALIBRATE_PARAM_INQ_END))
                {
                    scmd_reply = 0xFFFFFFFF;
                    memcpy(pSend + *lenSend, &scmd      , 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &scmd_reply, 4);
                    *lenSend += 4;
                    if(scmd != SCMD_INVALID)    /* 非无效子命令码 */    /* 无效码为非异常码 */
                    {
                        result = FRAME_SCMD_ERR;                    /* 子命令码为异常码 */
                    }
                    continue;                                       /* 子命令码不匹配(跳转下一个) */
                }

                /* 子命令码偏移量 */
                scmd_offset = (scmd - SCMD_CALIBRATE_PARAM_INQ_BEGIN) - 1;
                /* 待处理数据 */
                scmd_reply = *(&Tier1.calibrate[j].VsenseSetK_C + scmd_offset);
                /* 处理流程 */
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
**  函 数 名 : AdDataInqAnalysis()
**  功能说明 : AD数据查询解析函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t AdDataInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* 子命令码 */
    uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    //  float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint32_t scmd_reply = 0.0f; /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
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

                /* 待处理子命令码合法性判断 */
                if((scmd <= SCMD_AD_DATA_INQ_BEGIN) || (scmd >= SCMD_AD_DATA_INQ_END))
                {
                    scmd_reply = 0xFFFFFFFF;
                    memcpy(pSend + *lenSend, &scmd      , 1);
                    *lenSend += 1;
                    memcpy(pSend + *lenSend, &scmd_reply, 4);
                    *lenSend += 4;
                    if(scmd != SCMD_INVALID)    /* 非无效子命令码 */    /* 无效码为非异常码 */
                    {
                        result = FRAME_SCMD_ERR;                    /* 子命令码为异常码 */
                    }
                    continue;                                       /* 子命令码不匹配(跳转下一个) */
                }

                /* 子命令码偏移量 */
                scmd_offset = (scmd - SCMD_AD_DATA_INQ_BEGIN) - 1;
                /* 待处理数据 */
                scmd_reply = *(&Tier2.adSum[j].VsenseAdSum + scmd_offset);
                /* 处理流程 */
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
**  函 数 名 : SystemInqAnalysis()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t SystemInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* 子命令码 */
    //  uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    //  float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint8_t scmd_reply = 0;     /* 子命令码回复 */

    uint8_t Read_En = 0;
    uint8_t Bar_Code[50] = {0};
    uint8_t Bar_Code_Len = 0;
    uint32_t Base_Add = 0; //EE ，铁电， flash查询基址
    uint8_t  Date_len = 0; //EE ，铁电， flash查询数据长度

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_SYSTEM_INQ_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 将子命令码回复加载到发送缓存区 */
    memcpy(&scmd, pRecv + lenAnalysis, 1);
    memcpy(pSend + *lenSend, &scmd, 1);
    *lenSend += 1;

    /* 待处理子命令码合法性判断 */
    if((scmd <= SCMD_SYSTEM_INQ_BEGIN) || (scmd >= SCMD_SYSTEM_INQ_END))
    {
        result = FRAME_SCMD_ERR;        /* 子命令码为异常码 */
        scmd_reply = 0x01;
        memcpy(pSend + *lenSend, &scmd_reply, 1);
        *lenSend += 1;
        return result;
    }

    switch(scmd)
    {
        case SCMD_SOFTWARE_VERSION:     /* 软件版本查询 */
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

        case SCMD_WARMMING_MESSAGE:     /* 告警信息查询 */
        {
            for(int j = 0; j < CHANNEL_MAX; j++)
            {
                uint8_t umask = 0x01 << j;
                if(unpack.chEn & umask)
                {
                    /* 告警码上报 */
                }
            }
        }
        break;

        case SCMD_SYSTEM_MODE:          /* 系统模式查询 */
        {
            *(pSend + *lenSend) = g_ucSysMode;
            *lenSend += 1;
        }
        break;

        case SCMD_SYSTEM_CLIENT:        /* 客户端远端设置查询 */
        {
  
        }
        break;

        case SCMD_SYSTEM_MAC_Q:     /* MAC设置查询 */
        {
 
        }
        break;

        case SCMD_SYSTEM_BAR_Q:     /* 条码设置查询 */
        {
            ee_ReadBytes(&Read_En, MacEn_flag_addr , 1);    //读EE
            if(Read_En == 1)
            {
                ee_ReadBytes(&Bar_Code_Len, BarCode_Len_flag_addr , 1);  //读条码长度
                if(Bar_Code_Len < 50)                       //条码字符串长度限制50字节
                {
                    ee_ReadBytes(Bar_Code, BarCode_End_flag_addr - Bar_Code_Len + 1, Bar_Code_Len);  //根据长度读条码内容
                    memcpy(pSend + *lenSend, Bar_Code, Bar_Code_Len) ;
                    *lenSend += Bar_Code_Len;
                }
            }
        }
        break;

        case SCMD_SYSTEM_FRAM_Q:        /* 铁电设置查询 */
        {
            if(lenRecv != 0x12)
            {
                result = FRAME_DATA_ERR; //帧数据内容或格式错误
            }
            else
            {
                lenAnalysis += 1;
                memcpy(&Base_Add, pRecv + lenAnalysis, 4);  //获取基地址
                if(Base_Add > MB85RS2_Size - 1)  //地址超范围
                {
                    result = FRAME_DATA_ERR; //帧数据内容或格式错误
                    break;
                }
                lenAnalysis += 4;
                memcpy(&Date_len, pRecv + lenAnalysis, 1);  //获取读取数据长度
                memcpy(pSend + *lenSend, &Date_len, 1) ;    //填充回复数据长度段
                *lenSend += 1;
                MB85Ctrl_Read(Base_Add, (uint8_t *)(pSend + *lenSend), Date_len); //读取指定数据，填充回复数据内容段
                *lenSend += Date_len;
            }
        }
        break;

        case SCMD_SYSTEM_EE_Q:         /* EE设置查询 */
        {
            if(lenRecv != 0x12)
            {
                result = FRAME_DATA_ERR; //帧数据内容或格式错误
            }
            else
            {
                lenAnalysis += 1;
                memcpy(&Base_Add, pRecv + lenAnalysis, 4);  //获取基地址
                if(Base_Add > Last_addr)  //地址超范围
                {
                    result = FRAME_DATA_ERR; //帧数据内容或格式错误
                    break;
                }
                lenAnalysis += 4;
                memcpy(&Date_len, pRecv + lenAnalysis, 1);  //获取读取数据长度
                memcpy(pSend + *lenSend, &Date_len, 1) ;    //填充回复数据长度段
                *lenSend += 1;
                ee_ReadBytes((uint8_t *)(pSend + *lenSend), Base_Add, Date_len); //读取指定数据，填充回复数据内容段
                *lenSend += Date_len;
            }
        }
        break;

        case SCMD_SYSTEM_FLASH_Q:       /* FLASH设置查询 */
        {
  
        }
        break;

        case SCMD_SYSTEM_PG_Q:         /* PG脚状态查询 */
        {

        }
        break;

        case SCMD_SYSTEM_AC_Q:         /* AC侧电压查询 */
        {

        }
        break;

        case SCMD_SYSTEM_ACDC_VERSION_Q:           /* ACDC版本查询 */
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
**  函 数 名 : SystemSetAnalysis()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t SystemSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* 子命令码 */
//  uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    uint8_t sdat = 0;           /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint8_t scmd_reply = 0;     /* 子命令码回复 */

    uint8_t Read_En = 0;
    uint8_t Bar_Code[50] = {0};
    uint8_t Bar_Code_Len = 0;
    uint8_t Pyear, Pmonth; //生产年月
    uint16_t Pid; //流水号
    uint16_t Pym; //年月编码
    uint32_t Pmac = 0; //mac编号(低3字节有效)
    uint8_t err = 0;

    uint32_t Base_Add = 0; //flash,ee,铁电设置数据的起始地址
    uint8_t  date_len;     //flash,ee,铁电写入数据长度


    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_SYSTEM_SET_REPLY;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 将子命令码回复加载到发送缓存区 */
    memcpy(&scmd, pRecv + lenAnalysis, 1);
    memcpy(pSend + *lenSend, &scmd, 1);
    *lenSend += 1;

    /* 待处理子命令码合法性判断 */
    if((scmd <= SCMD_SYSTEM_SET_BEGIN) || (scmd >= SCMD_SYSTEM_SET_END))
    {
        result = FRAME_SCMD_ERR;        /* 子命令码为异常码 */
        scmd_reply = SCMD_NG;
    }
    else
    {
        switch(scmd)
        {
            case 0x01:      /* 系统告警清除 */
            {
				#if 0 //暂未用到
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
                Clear_AM_Flg = 1;  // 通知清除告警
				#endif
            }
            break;

            case 0x02:      /* 系统时间设定 */
            {
                memcpy(&Tstamp, pRecv + *lenSend, 8);
            }
            break;

            case 0x03:      /* 心跳包指令 */
            {
                heart_beat_flag = 0;
            }
            break;

            case 0x04:      /* 系统模式设置 */
            {
                memcpy(&sdat, pRecv + *lenSend, 1);

                if(sdat == 0x01)            /* 普通模式 */
                {
                        g_ucSysMode = SYS_NORMAL_MODE;
                }
                else
				{
                    if(sdat == 0x02)       /* 校准模式 */
                    {
                        g_ucSysMode = SYS_CALIBRATION_MODE;
                    }
                    else                        /* 其他模式 */
                    {
                        /* do nothing */
                    }
				}
            }
            break;

            case 0x05:      /* 使能数据上抛 */
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

            case 0x06:      /* 禁止数据上抛 */
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

            case 0x07:      /* 设置数据上抛周期 */
            {
                float sdat_data_upcast_period = 0.0f;
                memcpy(&sdat_data_upcast_period, pRecv + lenAnalysis + 1, 4);
                data_upcast_period = (sdat_data_upcast_period != 0.0f) ? sdat_data_upcast_period : 1.0f;
            }
            break;

            case 0x08:      /* 系统初始化 */
            {
                /* do nothing */
            }
            break;

            case 0x09:      /* 系统终止 */
            {
                /* do nothing */
            }
            break;

            case 0x0a:      /* 使能心跳包 */
            {
                feed_hardware_watchdog_enable = 1;
            }
            break;

            case 0x0b:      /* 禁止心跳包 */
            {
                feed_hardware_watchdog_enable = 0;
            }
            break;

            case 0x0c:      /* 探针未压合 */
            {
                SystemStatus.Neilsbed = Neilsbed_Nofit;
            }
            break;

            case 0x0d:      /* 探针压合中 */
            {
                SystemStatus.Neilsbed = Neilsbed_Fitting;
            }
            break;

            case 0x0e:      /* 探针已压合 */
            {
                SystemStatus.Neilsbed = Neilsbed_Fitted;
            }
            break;

            case 0x0f:      /* 设置客户端模式下,远端IP端口值,仅服务器模式生效 */
            {
            
            }
            break;

            case 0x10:      /* 设置条码(MAC后3字节) */
            {
                
            }
            break;

            case 0x11:      /* 设置铁电数据 */
            {
                lenAnalysis += 1;
                memcpy(&Base_Add, pRecv + lenAnalysis, 4); //获取数据基地址
                lenAnalysis += 4;
                memcpy(&date_len, pRecv + lenAnalysis, 1); //获取写入长度
                lenAnalysis += 1;
                if(Base_Add + date_len > MB85RS2_Size)
                {
                    result = FRAME_DATA_ERR; //帧数据内容或格式错误
                    break;
                }
                MB85Ctrl_Write(Base_Add, pRecv + lenAnalysis, date_len); //设置数据写入铁电
            }
            break;

            case 0x12:      /* 设置EE数据 */
            {
                lenAnalysis += 1;
                memcpy(&Base_Add, pRecv + lenAnalysis, 4); //获取数据基地址
                lenAnalysis += 4;
                memcpy(&date_len, pRecv + lenAnalysis, 1); //获取写入长度
                lenAnalysis += 1;
                if(Base_Add + date_len> Last_addr+1)
                {
                    result = FRAME_DATA_ERR; //帧数据内容或格式错误
                    break;
                }
                ee_WriteBytesWithCheck(pRecv + lenAnalysis, Base_Add, date_len); //设置数据写入EE

            }
            break;

            case 0x13:      /* 设置FLASH数据 */
            {   
				#if 0 //暂时不用
                lenAnalysis += 1;
                memcpy(&Base_Add, pRecv + lenAnalysis, 4); //获取数据基地址
                lenAnalysis += 4;
                memcpy(&date_len, pRecv + lenAnalysis, 1); //获取写入长度
                lenAnalysis += 1;
                if(Base_Add + date_len > FLASH_SIZE ) //地址超范围
                {
                    result = FRAME_DATA_ERR; //帧数据内容或格式错误
                    break;
                }
                SPI_Flash_WriteWithCheck(pRecv + lenAnalysis, Base_Add, date_len); //设置数据写入FLASH
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
        scmd_reply = SCMD_OK;
    }
    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

    return result;
}

/**********************************************************************************************************
**  函 数 名 : SystemUpdateAnalysis()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t SystemUpdateAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* 子命令码 */
    //  uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    //  uint8_t sdat = 0;           /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint8_t scmd_reply = 0;     /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_SYSTEM_UPDATE;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 将子命令码回复加载到发送缓存区 */
    memcpy(&scmd, pRecv + lenAnalysis, 1);
    memcpy(pSend + *lenSend, &scmd, 1);
    *lenSend += 1;

    /* 待处理子命令码合法性判断 */
    if((scmd <= SCMD_SYSTEM_UPDATE_BEGIN) || (scmd >= SCMD_SYSTEM_UPDATE_END))
    {
        result = FRAME_SCMD_ERR;        /* 子命令码为异常码 */
        scmd_reply = 0x01;
    }
    else
    {
        if(scmd == 0x01)                /* 跳转boot模式 */
        {
            uint8_t BOOT_FLAG1 = 0xBB;
            uint8_t BOOT_FLAG2 = 0xBB;
            ee_WriteBytes(&BOOT_FLAG1, 0x7FFE, 1);
            ee_WriteBytes(&BOOT_FLAG2, 0x7FFF, 1);
            NVIC_SystemReset();
        }
        scmd_reply = 0x00;
    }
    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

    return result;
}

/**********************************************************************************************************
**  函 数 名 : DebugParamSetAnalysis()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t DebugParamSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
//  uint8_t scmd = 0;           /* 子命令码 */
//  uint8_t scmd_offset = 0;    /* 子命令码偏移 */
//  float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint8_t scmd_reply = 0;     /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_DEBUG_PARAM_SET;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 将调试参数设置值存到结构体变量 */
    memcpy(&Tier2.debug.set, pRecv + lenAnalysis, sizeof(Tier2.debug.set));

    /* 设置参数合法性校验 */
    if((Tier2.debug.set.Vtemp <= 7.0f) && (Tier2.debug.set.Itemp <= 95.0f))
    {
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 0x01 << j;
            if(unpack.chEn & umask)
            {
                if(Tier2.debug.set.PortEnable == 0x00)          /* 关闭输出 */
                {
					if( ((debug_ch_onoff_state >> (2 * j)) & 0x03) != 3)
					{
						debug_ch_onoff_state &= ~(3 << (2 * j));
						debug_ch_onoff_state |= (1 << (2 * j));
					}						
					
//                    Chg_Dis_Off_Cfg(1 << j);
#ifdef YCJ
                    g_ucLedChSta[j  + 1] = CH_COMPLETE;
#else
//                    g_ucLedChSta[j / 2 + 1] = CH_COMPLETE;  //暂时不用
#endif
                }
                else if(Tier2.debug.set.PortEnable == 0x03)     /* 打开输出 */
				{
					if( ((debug_ch_onoff_state >> (2 * j)) & 0x03) != 3)
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
			#if 0 //暂时屏蔽，为解决编译问题
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
            Clear_AM_Flg = 1;  // 通知清除告警
			#endif
        }

        scmd_reply = SCMD_OK;
    }
    else                                                                        /* 设置参数非法 */
    {
        result     = FRAME_DATA_ERR;
        scmd_reply = SCMD_NG;
    }

    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

    return result;
}

/**********************************************************************************************************
**  函 数 名 : DebugDataInqAnalysis()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t DebugDataInqAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
//  uint8_t scmd = 0;           /* 子命令码 */
//  uint8_t scmd_offset = 0;    /* 子命令码偏移 */
//  float   sdat = 0.0f;        /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
//  uint8_t scmd_reply = 0;     /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_DEBUG_DATA_INQ;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    if(unpack.chEn == 0x00) /* 多通道查询 */
    {
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            memcpy(pSend + *lenSend, &Tier2.debug.inq1[j], sizeof(Tier2.debug.inq1[j]));
            *lenSend += sizeof(Tier2.debug.inq1[j]);
        }
        memcpy(pSend + *lenSend, &Tier2.debug.inq2, sizeof(Tier2.debug.inq2));
        *lenSend += sizeof(Tier2.debug.inq2);
    }
    else                    /* 单通道查询 */
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
**  函 数 名 : DebugSystemSetAnalysis()
**  功能说明 :
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t DebugSystemSetAnalysis(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t result = 0;
    uint8_t scmd = 0;           /* 子命令码 */
//  uint8_t scmd_offset = 0;    /* 子命令码偏移 */
    uint8_t sdat = 0;           /* 子命令码数据 */
    uint8_t pcmd_reply = 0;     /* 主命令码回复 */
    uint8_t scmd_reply = 0;     /* 子命令码回复 */

    /* 将主命令码回复加载到发送缓存区 */
    pcmd_reply = CMD_DEBUG_SYSTEM_SET;
    memcpy(pSend + *lenSend, &pcmd_reply, 1);
    *lenSend += 1;

    /* 获取子命令码及子命令码数据 */
    memcpy(&scmd, pRecv + lenAnalysis + 0, 1);
    memcpy(&sdat, pRecv + lenAnalysis + 1, 1);

    if((scmd >= SCMD_DEBUG_SYSTEM_SET_BEGIN) && (scmd <= SCMD_DEBUG_SYSTEM_SET_END))
    {
        for(int j = 0; j < CHANNEL_MAX; j++)
        {
            uint8_t umask = 0x01 << j;
            if(sdat & umask)
            {
				#if 0 //暂时屏蔽解决编译问题
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
					Chg_Dis_Port_Cfg(Channel_Port_Table[j], PORT_MODE_MASTER);
				}
				#endif
            }
        }
        scmd_reply = SCMD_OK;
    }
    else							/* 非正确命令码 */
    {
        result     = FRAME_SCMD_ERR;
        scmd_reply = SCMD_NG;
    }

    memcpy(pSend + *lenSend, &scmd      , 1);
    *lenSend += 1;
    memcpy(pSend + *lenSend, &scmd_reply, 1);
    *lenSend += 1;

    return result;
}

/**********************************************************************************************************
**  函 数 名 : DefaultAnalyze()
**  功能说明 : 未定义主命令码处理函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t DefaultAnalyze(uint8_t *pSend, uint16_t *lenSend, uint8_t errCmd)
{
    memcpy(pSend + *lenSend, &errCmd, 1);

    *lenSend += 1;

    return FRAME_PCMD_ERR;
}

/**********************************************************************************************************
**  函 数 名 : SendPack()
**  功能说明 : 发送打包函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
static void SendPack(uint8_t *pSend, uint16_t *lenSend)
{
    if(ucProtocolVersion == 0)          /* 旧版本协议 */
    {
        pack.head = FRAME_HEAD_SEND_OLD;            /* 帧头 */
        pack.len  = *lenSend;                       /* 帧长 */      /* 注: 数据长度不包括帧头(2Byte)和帧尾(2Byte) */
        pack.chEn = unpack.chEn;                    /* 通道号 */
        memcpy(pSend + 0, &pack.head, 2);
        memcpy(pSend + 2, &pack.len , 2);
        memcpy(pSend + 4, &pack.chEn, 1);
        pack.crc  = CRC16N(pSend + 2, *lenSend - 2);/* 校验码 */
        pack.tail = FRAME_TAIL_SEND_OLD;            /* 帧尾 */
    }
    else                                /* 新版本协议 */
    {
        pack.head = FRAME_HEAD_SEND_NEW;            /* 帧头 */
        pack.len  = *lenSend;                       /* 帧长 */
        pack.feat = unpack.feat;                    /* 特征码 */
        //      pack.sta  = 0;                              /* 状态码 */
        pack.chEn = unpack.chEn;                    /* 通道号 */
        memcpy(pSend + 0, &pack.head  , 2);
        memcpy(pSend + 2, &pack.len   , 2);
        memcpy(pSend + 4, &unpack.feat, 1);
        memcpy(pSend + 5, &pack.sta   , 1);
        memcpy(pSend + 6, &pack.chEn  , 1);
        pack.crc  = CRC16N(pSend + 2, *lenSend - 2);/* 校验码 */
        pack.tail = FRAME_TAIL_SEND_NEW;            /* 帧尾 */
    }

    memcpy(pSend + *lenSend + 0, &pack.crc , 2);
    memcpy(pSend + *lenSend + 2, &pack.tail, 2);
    *lenSend += 4;      /* 总长 */
}

/**********************************************************************************************************
**  函 数 名 : ProtocolDataProcess()
**  功能说明 : 协议数据处理函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
uint8_t ProtocolDataProcess(uint8_t *pRecv, uint16_t lenRecv, uint8_t *pSend, uint16_t *lenSend)
{
    uint8_t pcmd = 0;       /* 主命令码 */

    /* 1. 入参检查 */
    if(pRecv == NULL)       /* 拆包缓存区首地址指针为空 */
    {
        return FRAME_PARAM_ERR;         /* 帧形参错误 */
    }

    if(pSend == NULL)       /* 发包缓存区首地址指针为空 */
    {
        return FRAME_PARAM_ERR;         /* 帧形参错误 */
    }

    if(lenRecv < 10)        /* 待拆包数据长度异常 */
    {
        return FRAME_PARAM_ERR;         /* 帧形参错误 */
    }

    if(lenSend == NULL)     /* 待发包数据长度地址指针为空 */
    {
        return FRAME_PARAM_ERR;         /* 帧形参错误 */
    }

    /* 2. 拆包校验 */
    pack.sta = RecvUnpack(pRecv, lenRecv);

    if(ucProtocolVersion == 0)          /* 旧版本协议 */
    {
        lenAnalysis = 5;    /* 帧头(2 Byte) + 帧长(2 Byte) + 通道号(1 Byte) */

        *lenSend = 5;
    }
    else                                /* 新版本协议 */
    {
        lenAnalysis = 7;    /* 帧头(2 Byte) + 帧长(2 Byte) + 特征码(1 Byte) + 状态码(1 Byte) + 通道号(1 Byte) */

        *lenSend = 7;
    }

    if(pack.sta != FRAME_IS_OK)         /* 校验异常 */
    {
        SendPack(pSend, lenSend);       /* 异常回复 */

        return pack.sta;
    }

    memcpy(&pcmd, pRecv + lenAnalysis, 1);      /* 主命令码 */

    lenAnalysis += 1;                           /* 解析计数 */

    /* 3. 协议解析 */
    switch(pcmd)
    {
        /* 全局保护设置 */
        case CMD_GLOBAL_PROTECT_SET:
        {
            pack.sta = GlobalProtectSetAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 全局保护查询 */
        case CMD_GLOBAL_PROTECT_INQ:
        {
            pack.sta = GlobalProtectInqAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 内部保护设置 */
        case CMD_INTERNAL_PROTECT_SET:
        {
            pack.sta = InternalProtectSetAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 内部保护查询 */
        case CMD_INTERNAL_PROTECT_INQ:
        {
            pack.sta = InternalProtectInqAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 记录数据查询 */
        case CMD_RECORD_INQ:
        {
            pack.sta = RecordInqAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 工步设置 */
        case CMD_WORKSTEP_SET:
        {
            pack.sta = WorkstepSetAnalysis(pRecv, lenRecv, pSend, lenSend);

        }
        break;

        /* 工步运行状态设置 */
        case CMD_WORKSTEP_RUNSTA_SET:
        {
            pack.sta = WorkstepRunstaSetAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 工步查询 */
        case CMD_WORKSTEP_INQ:
        {
            pack.sta = WorkstepInqAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 校准参数设置 */
        case CMD_CALIBRATE_PARAM_SET:
        {
            pack.sta = CalibrateSetAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 校准参数查询 */
        case CMD_CALIBRATE_PARAM_INQ:
        {
            pack.sta = CalibrateInqAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* AD数据读取 */
        case CMD_AD_DATA_INQ:
        {
            pack.sta = AdDataInqAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 系统查询 */
        case CMD_SYSTEM_INQ:
        {
            pack.sta = SystemInqAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 系统设置 */
        case CMD_SYSTEM_SET:
        {
            pack.sta = SystemSetAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 系统升级 */
        case CMD_SYSTEM_UPDATE:
        {
            pack.sta = SystemUpdateAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* ACDC升级*/
        case CMD_ACDC_UPDATE:
        {

        }
        break;

        /* 调试参数设置命令/回复 */
        case CMD_DEBUG_PARAM_SET:
        {
            pack.sta = DebugParamSetAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 调试数据查询命令/回复 */
        case CMD_DEBUG_DATA_INQ:
        {
            pack.sta = DebugDataInqAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* 调试系统设置命令/回复 */
        case CMD_DEBUG_SYSTEM_SET:
        {
            pack.sta = DebugSystemSetAnalysis(pRecv, lenRecv, pSend, lenSend);
        }
        break;

        /* Boot升级*/
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

    SendPack(pSend, lenSend);           /* 打包发送 */

    return pack.sta;
}

//uint8_t  recordBuffer[256] = {0};		/* 上抛数据缓存 */

int64_t TstampPre[8] = {0};
uint8_t err_send_flag = 0;

/**********************************************************************************************************
**  函 数 名 : RecordDatUpcast()
**  功能说明 : 记录数据上抛函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
void RecordDatUpcast(uint8_t chx, uint8_t sync_flag)
{	
//	int32_t ls,us; 
//	
//	Dis_int();
//	ls = osKernelLock(); 	
	
	if((upcast_enable & (1 << chx)) == 0)	
	{
		return;		/* 通道失能禁止上抛 */
	}
	
	if(heart_beat_flag == 2)
	{
		return;		/* 心跳丢失禁止上抛 */
	}
	
	if(g_ucSysMode != SYS_NORMAL_MODE)
	{
		return;		/* 非正常模式禁止上抛 */
	}
	
	uint8_t  j = chx;
	uint8_t  recordBuffer[256] = {0};		/* 上抛数据缓存 */
	uint16_t head = 0;						/* 帧头         */
	uint16_t len  = 0;						/* 帧长         */
	uint8_t  feat = 0;						/* 特征码       */
	uint8_t  sta  = 0;						/* 状态码       */
	uint8_t  ch   = 1 << j;					/* 通道号       */
	uint8_t  pcmd = CMD_RECORD_INQ_REPLY;	/* 回复码       */
	uint16_t crc  = 0;						/* 校验码       */
	uint16_t tail = 0;						/* 帧尾         */
	
//	memset(recordBuffer, 0, 256);
	
	if(ucProtocolVersion == 0)				/* 旧版本协议 */
	{
		head = FRAME_HEAD_SEND_OLD;
		tail = FRAME_TAIL_SEND_OLD;
		memcpy(recordBuffer + 0, &head, 2);
	//	memcpy(recordBuffer + 2, &len , 2);
		memcpy(recordBuffer + 4, &ch  , 1);
		memcpy(recordBuffer + 5, &pcmd, 1);
		len += 6;		
	}
	else									/* 新版本协议 */
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
	
	memcpy(recordBuffer + 2       , &len , 2);	/* 帧长   */
	crc  = CRC16N(recordBuffer + 2, len - 2);	/* 校验码 */
	memcpy(recordBuffer + len + 0 , &crc , 2);	/* 校验码 */
	memcpy(recordBuffer + len + 2 , &tail, 2);	/* 帧尾 */
	len += 4;
	
	/* 数据上抛 */
//	send(0, recordBuffer, len);	//暂未用到
	
	Can_Network_Upcast(recordBuffer, len);
}

/**********************************************************************************************************
**  函 数 名 : WorkstepProtUpcast()
**  功能说明 : 工步保护数据上抛函数
**  形    参 :
**  返 回 值 :
**********************************************************************************************************/
void WorkstepProtUpcast(uint8_t chx)
{
    uint8_t  j = chx;
    uint8_t  workstepProtBuffer[256] = {0};
    uint16_t head = 0;                          /* 帧头         */
    uint16_t len  = 0;                          /* 帧长         */
    uint8_t  feat = 0;                          /* 特征码       */
    uint8_t  sta  = 0;                          /* 状态码       */
    uint8_t  ch   = 1 << j;                     /* 通道号       */
    uint8_t  pcmd = CMD_WORKSTEP_PROTECT_TRIG;  /* 回复码       */
    uint16_t crc  = 0;                          /* 校验码       */
    uint16_t tail = 0;                          /* 帧尾         */

    if(ucProtocolVersion == 0)                  /* 旧版本协议 */
    {
        head = FRAME_HEAD_SEND_OLD;
        tail = FRAME_TAIL_SEND_OLD;
        memcpy(workstepProtBuffer + 0, &head, 2);
        //  memcpy(workstepProtBuffer + 2, &len , 2);
        memcpy(workstepProtBuffer + 4, &ch  , 1);
        memcpy(workstepProtBuffer + 5, &pcmd, 1);
        len += 6;
    }
    else                                        /* 新版本协议 */
    {
        head = FRAME_HEAD_SEND_NEW;
        tail = FRAME_TAIL_SEND_NEW;
        memcpy(workstepProtBuffer + 0, &head, 2);
        //  memcpy(workstepProtBuffer + 2, &len , 2);
        memcpy(workstepProtBuffer + 4, &feat, 1);
        memcpy(workstepProtBuffer + 5, &sta , 1);
        memcpy(workstepProtBuffer + 6, &ch  , 1);
        memcpy(workstepProtBuffer + 7, &pcmd, 1);
        len += 8;
    }

    for(uint8_t i = 1; i < SCMD_WORKSTEP_PROT_END; i++)
    {
		#if 0 //暂时屏蔽不用
        uint16_t umask = 1 << (i - 1);
        if(workstep_flag_ErrWord[j] & umask)
        {
            memcpy(workstepProtBuffer + len, &i, 1);
            len += 1;
        }
		#endif
    }
    memcpy(workstepProtBuffer + 2       , &len , 2);    /* 帧长   */
    crc  = CRC16N(workstepProtBuffer + 2, len - 2);     /* 校验码 */
    memcpy(workstepProtBuffer + len + 0 , &crc , 2);    /* 校验码 */
    memcpy(workstepProtBuffer + len + 2 , &tail, 2);    /* 帧尾 */
    len += 4;

    /* 数据上抛 */
//    send(0, workstepProtBuffer, len);
	
	Can_Network_Upcast(workstepProtBuffer, len);
}

/**********************************************************************************************************
**  函 数 名 : AppTaskStart()
**  功能说明 : 协议解析任务
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AppTaskProtocol(void *argument)
{
    while(1)
    {	 
		Can_Network_Func(10,1,1,1,1);
		
        osDelay(1);		
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


