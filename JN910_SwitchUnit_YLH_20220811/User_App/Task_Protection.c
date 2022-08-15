/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Protection.c
** Author      : Yi Li Hua
** Date        : 2022-08-08
** Description :
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "Task_Protection.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint64_t IP_Alarm_Flag[8] = {0};         // 内部保护标志位
uint32_t GP_Alarm_Flag[8] = {0};         // 全局保护标志位
uint32_t IP_Alarm_Flag_History[8] = {0}; // 内部保护历史标志位
uint32_t GP_Alarm_Flag_History[8] = {0}; // 全局保护历史标志位
uint8_t First_Alarm = 0;                 // 第一次报警标志(避免第一次重报) 0-第一次报警 1-不是第一次报警
uint8_t ucIProtCntPre[12][8] = {0};      // 内部故障检测计时(前次)
uint8_t SysModeProtMaskFlag = 0;         // 故障屏蔽标识(0-不屏蔽故障 1-屏蔽故障)
uint8_t ucIProtCntCur[12][8] = {0};      // 内部故障检测计时(当前)
uint8_t ucGProtCntPre[4][8] = {0};       // 全局故障检测计时(前次)
uint8_t ucGProtCntCur[4][8] = {0};       // 全局故障检测计时(当前)
protect_alarmStruct Protect_Alarm;       // 内部报警帧

uint8_t features;

CMDProject IP_CheckList[] =
{
   {0x01, IntelProtect_OLA, 0, 0},			// 输出限流保护
   {0x02, IntelProtect_BOV, 0, 0},			// BUS过压
   {0x03, IntelProtect_BUV, 0, 0},			// BUS欠压
   {0x04, IntelProtect_APA, 0, 0},			// 电流精度告警
   {0x05, IntelProtect_UPA, 0, 0},			// 电压精度告警
   {0x06, IntelProtect_VDQ, 0, 0},			// 短路保护告警
   {0x07, IntelProtect_MOT, 0, 0},			// 模块过温
   {0x08, IntelProtect_EOT, 0, 0},			// 环境过温
   {0x09, IntelProtect_HPA, 0, 0},			// 硬件保护
   {0x0a, IntelProtect_FHE, ALLCH, ALLCH},	// 风扇故障,没有使能设置命令，默认开启
   {0x0b, IntelProtect_UHE, ALLCH, ALLCH},	// CPU异常,没有使能设置命令，默认开启
   {0x0c, IntelProtect_C1E, ALLCH, ALLCH},	// can1异常,没有使能设置命令，默认开启
   {0x0d, IntelProtect_C2E, ALLCH, ALLCH},	// can2异常,没有使能设置命令，默认开启
   {0x0e, IntelProtect_DAE, ALLCH, ALLCH},	// DA异常,没有使能设置命令，默认开启
   {0x0f, IntelProtect_ADE, ALLCH, ALLCH},	// AD异常,没有使能设置命令，默认开启
   {0x10, IntelProtect_FLE, ALLCH, ALLCH},	// FLASH异常,没有使能设置命令，默认开启
   {0x11, IntelProtect_EEE, ALLCH, ALLCH},	// Eeprom异常,没有使能设置命令，默认开启
   {0x12, IntelProtect_FRE, ALLCH, ALLCH},	// FRAM异常,没有使能设置命令，默认开启
   {0x13, IntelProtect_PME, ALLCH, ALLCH},	// 电位器异常,没有使能设置命令，默认开启
		
   {0x14, IntelProtect_VNN, ALLCH, ALLCH},			// 电压异常，协议新增
   {0x15, IntelProtect_CVP, 0, 0},			// 回路压差保护，协议新增
   {0x16, IntelProtect_VUQ, 0, 0},			// 电压快速上升，协议新增
   {0x17, IntelProtect_CNN, 0, 0},			// 电流异常，协议新增
   {0x18, IntelProtect_CUQ, 0, 0},			// 电流快速上升，协议新增
   {0x19, IntelProtect_LOV, 0, 0},			// 辅助电压异常（探针电压大于5.1V），协议新增
   {0x1A, IntelProtect_MAC, 0, 0},			// MAC异常
   {0x1B, IntelProtect_MAC, 0, 0},			//通道模块异常  
};

/*全局保护触发子命令查找表*/
CMDProject GP_CheckList[] =
{
    {0x01, GlobalProtect_OV, 0, 0},   // 过压保护
    {0x02, GlobalProtect_UV, 0, 0},   // 欠压保护
    {0x03, GlobalProtect_COA, 0, 0},  // 充电过流保护
    {0x04, GlobalProtect_DOA, 0, 0},  // 放电过流保护
    {0x05, GlobalProtect_OC, 0, 0},   // 过容保护
    {0x06, GlobalProtect_MV, 0, 0},   // 初始电压上限保护
    {0x07, GlobalProtect_LV, 0, 0},   // 初始电压下限保护
    {0x08, IntelProtect_NONE1, 0, 0}, // 未定义
    {0x09, IntelProtect_NONE2, 0, 0}, // 未定义
    {0x0a, IntelProtect_VCV, 0, 0},   // 接触检测电压变化量
    {0xfe, IntelProtect_OTE, 0, 0},   // 其他故障
    {0xff, IntelProtect_INV, 0, 0},   // 无效
};

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : IntelProtectDeviceCheck()
**  功能说明 : 内部保护开机检测,用于cpu、can1、eeprom、fram,开机执行一次
**  形    参 : 无
**  返 回 值 : 内部保护报警状态
**********************************************************************************************************/
uint32_t IntelProtectDeviceCheck(void)
{
    if (SysModeProtMaskFlag == 0) //故障屏蔽标识无效
    {

        /* 检测fram,读ID */
        uint32_t FramID = 0;

        MB85Ctrl_ReadID(&FramID); //读取FRAM的ID

        if ((FramID & 0xff000000) != 0x04000000) // ID最高字节0x04代表制造商为富士
        {
            for (uint8_t j = 0; j < CHANNEL_CFG; j++)
            {
                IP_Alarm_Flag[j] |= IntelProtect_FRE; // fram故障
            }
        }
    }
    return 1;
}

/**********************************************************************************************************
**  函 数 名 : IntelProtectRunParmCheck()
**  功能说明 : 内部保护运行参数检测,用于输出限流保护、bus过压、bus欠压等运行参数检测,执行周期1ms
**  形    参 : 无
**  返 回 值 : 内部保护报警状态
**********************************************************************************************************/
uint32_t IntelProtectRunParmCheck(void)
{
    uint8_t ch;
    if (SysModeProtMaskFlag == 0) //故障屏蔽标识无效
    {
        for (uint8_t i = 0; i < CHANNEL_CFG; i++)
        {
            //-------------------------------1.输出限流检测-------------------------------//
            /* 1.1 更新前次故障检测计时 */
            ucIProtCntPre[0][i] = ucIProtCntCur[0][i];
            /* 1.2 故障检测逻辑 */
            if ((Tier2.iProt[i].IoutThr.isEnable & 0x80) == 0)
            {
                if ((Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_STOP) &&
                    (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_COMPLETE) &&
                    (ProtectMaskFlag[i] != 1))
                {
                    if (Tier2.chRecord[i].Iout > Tier2.iProt[i].IoutThr.value)
                    {
                        for(ch = 0; ch < CHANNEL_CFG; ch++)  //1-8通道同时动作
                        {
                            ucIProtCntCur[0][ch]++;
                            if (ucIProtCntCur[0][ch] >= PROT_TRIG_MS(400, 400))
                            {
                                IP_Alarm_Flag[ch] |= IntelProtect_OLA;
                                ucIProtCntCur[0][ch] = 0;
                            }
                        }
                    }
                }
            }
            /* 1.3 判断故障是否持续发生 */
            if (ucIProtCntPre[0][i] == ucIProtCntCur[0][i])
            {
                ucIProtCntPre[0][i] = 0;
                ucIProtCntCur[0][i] = 0;
            }

            //-------------------------------2.BUS过压检测-------------------------------//
            /* 2.1 更新前次故障检测计时 */
            ucIProtCntPre[1][i] = ucIProtCntCur[1][i];
            ucIProtCntPre[10][i] = ucIProtCntCur[10][i];
            /* 2.2 故障检测逻辑 */
            if ((Tier2.iProt[i].VbusThrH.isEnable & 0x80) == 0)
            {
                //				if(ProtectMaskFlag[i] != 1)
                {
                    if (Tier2.sampleCal.Vbus1 > Tier2.iProt[i].VbusThrH.value) // bus1对应前一半通道
                    {
                        ucIProtCntCur[1][i]++;
                        if (ucIProtCntCur[1][i] >= PROT_TRIG_MS(2000, 400))
                        {
                            for (uint8_t j = 0; j < CHANNEL_CFG; j++)
                            {
                                IP_Alarm_Flag[j] |= IntelProtect_BOV;
                            }
                            ucIProtCntCur[1][i] = 0;
                        }
                    }
                }
            }
            /* 2.3 判断故障是否持续发生 */
            if (ucIProtCntPre[1][i] == ucIProtCntCur[1][i])
            {
                ucIProtCntPre[1][i] = 0;
                ucIProtCntCur[1][i] = 0;
            }

            //-------------------------------4.电压精度检测-------------------------------//
            /* 4.1 更新前次故障检测计时 */
            ucIProtCntPre[3][i] = ucIProtCntCur[3][i];
            /* 4.2 故障检测逻辑 */
            if ((Tier2.iProt[i].VcvDiff.isEnable & 0x80) == 0)
            {
                if ((workstep_flag_CriticalZone[i] == 3) &&
                    (ProtectMaskFlag[i] != 1))
                {
                    if (fabs(Tier2.chRecord[i].Vbat - Tier2.workstepAnalyze[i].StepRunVol) > Tier2.iProt[i].VcvDiff.value)
                    {
                        ucIProtCntCur[3][i]++;
                        if (ucIProtCntCur[3][i] >= PROT_TRIG_MS(2000, 400))
                        {
                            IP_Alarm_Flag[i] |= IntelProtect_UPA;
                            ucIProtCntCur[3][i] = 0;
                        }
                    }
                }
            }
            /* 4.3 判断故障是否持续发生 */
            if (ucIProtCntPre[3][i] == ucIProtCntCur[3][i])
            {
                ucIProtCntPre[3][i] = 0;
                ucIProtCntCur[3][i] = 0;
            }

            //-------------------------------5.模块过温检测-------------------------------//
            /* 5.1 更新前次故障检测计时 */
            ucIProtCntPre[4][i] = ucIProtCntCur[4][i];
            /* 5.2 故障检测逻辑 */
            if ((Tier2.iProt[i].Totmod.isEnable & 0x80) == 0)
            {
                //				if(ProtectMaskFlag[i] != 1)
                {
                    if (Tier2.chRecord[i].Tmod > Tier2.iProt[i].Totmod.value)
                    {
                        ucIProtCntCur[4][i]++;
                        if (ucIProtCntCur[4][i] >= PROT_TRIG_MS(2000, 400))
                        {
                            IP_Alarm_Flag[i] |= IntelProtect_MOT;
                            ucIProtCntCur[4][i] = 0;
                        }
                    }
                }
            }
            /* 5.3 判断故障是否持续发生 */
            if (ucIProtCntPre[4][i] == ucIProtCntCur[4][i])
            {
                ucIProtCntPre[4][i] = 0;
                ucIProtCntCur[4][i] = 0;
            }

            //-------------------------------6.环境过温检测-------------------------------//
            /* 6.1 更新前次故障检测计时 */
            ucIProtCntPre[5][i] = ucIProtCntCur[5][i];
            /* 6.2 故障检测逻辑 */
            if ((Tier2.iProt[i].Totenv.isEnable & 0x80) == 0)
            {
                //				if(ProtectMaskFlag[i] != 1)
                {
                    if (Tier2.sampleCal.Tamb > Tier2.iProt[i].Totenv.value)
                    {
                        ucIProtCntCur[5][i]++;
                        if (ucIProtCntCur[5][i] >= PROT_TRIG_MS(2000, 400))
                        {
                            IP_Alarm_Flag[i] |= IntelProtect_EOT;
                            ucIProtCntCur[5][i] = 0;
                        }
                    }
                }
            }
            /* 6.3 判断故障是否持续发生 */
            if (ucIProtCntPre[5][i] == ucIProtCntCur[5][i])
            {
                ucIProtCntPre[5][i] = 0;
                ucIProtCntCur[5][i] = 0;
            }

            //-------------------------------7.电压异常检测-------------------------------//
            /* 7.1 更新前次故障检测计时 */
            ucIProtCntPre[6][i] = ucIProtCntCur[6][i];
            /* 7.2 故障检测逻辑 */
            if ((Tier2.iProt[i].SNERR.isEnable & 0x80) == 0)
            {
                if ((Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_START) &&
                    (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_CONTACT) &&
                    ((Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_RUNNING) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_STANDBY)) &&
                    (ProtectMaskFlag[i] != 1))
                {
                    if (fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vtz) > (3 * Tier2.iProt[i].SNERR.value))
                    {
                        ucIProtCntCur[6][i]++;
                        if (ucIProtCntCur[6][i] >= PROT_TRIG_MS(4000, 400))
                        {
                            IP_Alarm_Flag[i] |= IntelProtect_VNN;
                            ucIProtCntCur[6][i] = 0;
                        }
                    }
                }
            }
            /* 7.3 判断故障是否持续发生 */
            if (ucIProtCntPre[6][i] == ucIProtCntCur[6][i])
            {
                ucIProtCntPre[6][i] = 0;
                ucIProtCntCur[6][i] = 0;
            }

            //-------------------------------8.回路压差检测-------------------------------//
            /* 8.1 更新前次故障检测计时 */
            ucIProtCntPre[7][i] = ucIProtCntCur[7][i];
            /* 8.2 故障检测逻辑 */
            if ((Tier2.iProt[i].VloopDiff.isEnable & 0x80) == 0)
            {
                if ((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) &&
                    (ProtectMaskFlag[i] != 1))
                {
                    if (fabs(Tier2.chRecord[i].Vbat - Tier2.chRecord[i].Vout) > Tier2.iProt[i].VloopDiff.value)
                    {
                        ucIProtCntCur[7][i]++;
                        if (ucIProtCntCur[7][i] >= PROT_TRIG_MS(400, 400))
                        {
                            IP_Alarm_Flag[i] |= IntelProtect_CVP;
                            ucIProtCntCur[7][i] = 0;
                        }
                    }
                }
            }
            /* 8.3 判断故障是否持续发生 */
            if (ucIProtCntPre[7][i] == ucIProtCntCur[7][i])
            {
                ucIProtCntPre[7][i] = 0;
                ucIProtCntCur[7][i] = 0;
            }
            //-------------------------------9.电流精度检测-------------------------------//
            /* 9.1 更新前次故障检测计时 */
            ucIProtCntPre[8][i] = ucIProtCntCur[8][i];
            /* 9.2 故障检测逻辑 */
            if ((Tier2.iProt[i].IccDiff.isEnable & 0x80) == 0)
            {
                if ((Tier2.workstepRunning[i].StepType != SCMD_WORKSTEP_TYPE_STANDBY) &&
                    (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) &&
                    (workstep_flag_CriticalZone[i] == 0) && (ProtectMaskFlag[i] != 1))
                {
                    if (fabs(Tier2.chRecord[i].Iout - Tier2.workstepAnalyze[i].StepRunCur) > 0.32f)
                    {
                        ucIProtCntCur[8][i]++;
                        if (ucIProtCntCur[8][i] >= PROT_TRIG_MS(2000, 400))
                        {
                            if (Tier2.chRecord[i].Iout < 0.1f) //模块通道异常
                            {
                                IP_Alarm_Flag[i] |= IntelProtect_CHE;
                            }
                            else if (fabs(Tier2.chRecord[i].Iout - Tier2.workstepAnalyze[i].StepRunCur) > 2.0f) //电流异常
                            {
                                IP_Alarm_Flag[i] |= IntelProtect_CNN;
                            }
                            else //电流精度
                            {
                                IP_Alarm_Flag[i] |= IntelProtect_APA;
                            }
                            ucIProtCntCur[8][i] = 0;
                        }
                    }
                }
            }
            /* 9.3 判断故障是否持续发生 */
            if (ucIProtCntPre[8][i] == ucIProtCntCur[8][i])
            {
                ucIProtCntPre[8][i] = 0;
                ucIProtCntCur[8][i] = 0;
            }

            //-------------------------------10.电流异常检测-------------------------------//
            /* 10.1 更新前次故障检测计时 */
            ucIProtCntPre[9][i] = ucIProtCntCur[9][i];
            /* 10.2 故障检测逻辑 */
            if ((Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_START) &&
                (Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_CONTACT) &&
                ((Tier2.workstepRunning[i].RunSta != WORKSTEP_RUNSTA_REF_RUNNING) || (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_STANDBY)) &&
                (ProtectMaskFlag[i] != 1))
            {
                if (Tier2.chRecord[i].Iout > 0.1f)
                {
                    ucIProtCntCur[9][i]++;
                    if (ucIProtCntCur[9][i] >= PROT_TRIG_MS(2000, 400))
                    {
                        IP_Alarm_Flag[i] |= IntelProtect_CNN;
                        ucIProtCntCur[9][i] = 0;
                    }
                }
            }
            /* 10.3 判断故障是否持续发生 */
            if (ucIProtCntPre[9][i] == ucIProtCntCur[9][i])
            {
                ucIProtCntPre[9][i] = 0;
                ucIProtCntCur[9][i] = 0;
            }

            //-------------------------------11.辅助电压异常检测-------------------------------//
        }
    }

    return 1;
}

/**********************************************************************************************************
**  函 数 名 : GlobleProtectRunParmCheck()
**  功能说明 : 全局运行参数检测，用于电池电压上下限，回路压差，冲放电过流等运行参数检测，执行周期1ms
**  形    参 : 无
**  返 回 值 : 内部保护报警状态
**********************************************************************************************************/
uint32_t GlobleProtectRunParmCheck(void)
{
    if (SysModeProtMaskFlag == 0) //故障屏蔽标识无效
    {
        for (uint8_t i = 0; i < CHANNEL_CFG; i++)
        {
            //-------------------------------1.电池过压检测-------------------------------//
            /* 1.1 更新前次故障检测计时 */
            ucGProtCntPre[0][i] = ucGProtCntCur[0][i];
            /* 1.2 故障检测逻辑 */
            if ((Tier2.gProt[i].VbatThrH.isEnable & 0x80) == 0)
            {
                if (ProtectMaskFlag[i] != 1)
                {
                    if (Tier2.chRecord[i].Vbat > Tier2.gProt[i].VbatThrH.value)
                    {
                        ucGProtCntCur[0][i]++;
                        if (ucGProtCntCur[0][i] >= PROT_TRIG_MS(400, 400))
                        {
                            GP_Alarm_Flag[i] |= GlobalProtect_OV;
                            ucGProtCntCur[0][i] = 0;
                        }
                    }
                }
            }
            /* 1.3 判断故障是否持续发生 */
            if (ucGProtCntPre[0][i] == ucGProtCntCur[0][i])
            {
                ucGProtCntPre[0][i] = 0;
                ucGProtCntCur[0][i] = 0;
            }

            //-------------------------------2.电池欠压检测-------------------------------//
            /* 2.1 更新前次故障检测计时 */
            ucGProtCntPre[1][i] = ucGProtCntCur[1][i];
            /* 2.2 故障检测逻辑 */
            if ((Tier2.gProt[i].VbatThrL.isEnable & 0x80) == 0)
            {
                if (((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) ||
                     (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_WAIT)) &&
                    (ProtectMaskFlag[i] != 1))
                {
                    if (Tier2.chRecord[i].Vbat < Tier2.gProt[i].VbatThrL.value)
                    {
                        ucGProtCntCur[1][i]++;
                        if (ucGProtCntCur[1][i] >= PROT_TRIG_MS(400, 400))
                        {
                            GP_Alarm_Flag[i] |= GlobalProtect_UV;
                            ucGProtCntCur[1][i] = 0;
                        }
                    }
                }
            }
            /* 2.3 判断故障是否持续发生 */
            if (ucGProtCntPre[1][i] == ucGProtCntCur[1][i])
            {
                ucGProtCntPre[1][i] = 0;
                ucGProtCntCur[1][i] = 0;
            }

            //-------------------------------3.充电过流检测-------------------------------//
            /* 3.1 更新前次故障检测计时 */
            ucGProtCntPre[2][i] = ucGProtCntCur[2][i];
            /* 3.2 故障检测逻辑 */
            if ((Tier2.gProt[i].Iovcharge.isEnable & 0x80) == 0)
            {
                uint8_t err_check_flg = 0;

                if ((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) &&
                    (ProtectMaskFlag[i] != 1))
                {
                    err_check_flg = 1;
                }
                else if ((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) &&
                         ((Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CC) ||
                          (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CV) ||
                          (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_CCTOCV)) &&
                         (ProtectMaskFlag[i] != 1))
                {
                    err_check_flg = 1;
                }

                if (err_check_flg == 1)
                {
                    if (Tier2.chRecord[i].Iout > Tier2.gProt[i].Iovcharge.value)
                    {
                        ucGProtCntCur[2][i]++;
                        if (ucGProtCntCur[2][i] >= PROT_TRIG_MS(400, 400))
                        {
                            GP_Alarm_Flag[i] |= GlobalProtect_COA;
                            ucGProtCntCur[2][i] = 0;
                        }
                    }
                }
            }
            /* 3.3 判断故障是否持续发生 */
            if (ucGProtCntPre[2][i] == ucGProtCntCur[2][i])
            {
                ucGProtCntPre[2][i] = 0;
                ucGProtCntCur[2][i] = 0;
            }

            //-------------------------------4.放电过流检测-------------------------------//
            /* 4.1 更新前次故障检测计时 */
            ucGProtCntPre[3][i] = ucGProtCntCur[3][i];
            /* 4.2 故障检测逻辑 */
            if ((Tier2.gProt[i].Iovdischg.isEnable & 0x80) == 0)
            {
                if ((Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) &&
                    ((Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCC) ||
                     (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCV) ||
                     (Tier2.workstepRunning[i].StepType == SCMD_WORKSTEP_TYPE_DCCTODCV)) &&
                    (ProtectMaskFlag[i] != 1))
                {
                    if (Tier2.chRecord[i].Iout > Tier2.gProt[i].Iovdischg.value)
                    {
                        ucGProtCntCur[3][i]++;
                        if (ucGProtCntCur[3][i] >= PROT_TRIG_MS(400, 400))
                        {
                            GP_Alarm_Flag[i] |= GlobalProtect_DOA;
                            ucGProtCntCur[3][i] = 0;
                        }
                    }
                }
            }
            /* 4.3 判断故障是否持续发生 */
            if (ucGProtCntPre[3][i] == ucGProtCntCur[3][i])
            {
                ucGProtCntPre[3][i] = 0;
                ucGProtCntCur[3][i] = 0;
            }

            //-------------------------------5.单工步过容检测-------------------------------//
        }
    }

    return 1;
}

/**********************************************************************************************************
**  函 数 名 : AlarmQuarry()
**  功能说明 : 查询实时报警状态
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AlarmQuarry(void)
{
    uint8_t i = 0;
    uint16_t cnt = 0;   //故障个数计数
    uint16_t lenth = 0; //长度
    uint16_t crc16 = 0; //校验码
    uint8_t AlarmSendBuf[1024] = {0};

    AlarmSendBuf[0] = 0x5B;
    AlarmSendBuf[1] = 0x5B; //下位机给中位机发，帧头0x5B5B
    AlarmSendBuf[2] = 0x09;
    AlarmSendBuf[3] = 0x00; //固定长度9
    AlarmSendBuf[4] = 0x00; //特征码，每次上报自加1
    AlarmSendBuf[5] = 0x00; //状态码0-OK
    AlarmSendBuf[6] = 0x00; //通道号

    IntelProtectRunParmCheck();  //内部保护检测
    GlobleProtectRunParmCheck(); //全局保护检测

    /*内部保护，全局保护查询上抛，按通道查询*/
    for (i = 0; i < CHANNEL_MAX; i++)
    {
        if (IP_Alarm_Flag[i] != 0)
        {
            AlarmSendBuf[4] = features++; //特征码+1
            AlarmSendBuf[6] = 0x01 << i;  //内部保护报警通道号
            AlarmSendBuf[7] = 0xA0;       //主命令，内部保护报警,固定0xa0, 全局保护报警：0x83
            for (uint8_t j = 0; j < sizeof(IP_CheckList) / sizeof(CMDProject); j++)
            {
                if (IP_CheckList[j].P_Flg & IP_Alarm_Flag[i])
                {
                    cnt++;
                    AlarmSendBuf[7 + cnt] = IP_CheckList[j].childCMD;
                }
            }
            lenth = 8 + cnt;
            AlarmSendBuf[2] = lenth & 0x00ff; //更新帧长
            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;
            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt); //计算CRC校验字段
            AlarmSendBuf[8 + cnt] = crc16 & 0x00ff;
            AlarmSendBuf[9 + cnt] = (crc16 & 0xff00) >> 8;
            AlarmSendBuf[10 + cnt] = 0xa5; //添加帧尾
            AlarmSendBuf[11 + cnt] = 0xa5;
            Can_Network_Upcast((uint8_t *)&AlarmSendBuf, lenth + 4);

            First_Alarm = 1;
            cnt = 0; //子命令计数清零
            bsp_DelayMS(1);
        }

        cnt = 0; //子命令计数清零

        if (GP_Alarm_Flag[i] != 0)
        {
            AlarmSendBuf[4] = features++; //特征码+1
            AlarmSendBuf[6] = 0x01 << i;  //报警通道号
            AlarmSendBuf[7] = 0x83;       //主命令，内部保护报警,固定0xa0, 全局保护报警：0x83
            for (uint8_t j = 0; j < sizeof(GP_CheckList) / sizeof(CMDProject); j++)
            //添加子命令
            {
                if (GP_CheckList[j].P_Flg & GP_Alarm_Flag[i])
                {
                    cnt++;
                    AlarmSendBuf[7 + cnt] = GP_CheckList[j].childCMD;
                }
            }
            lenth = 8 + cnt;
            AlarmSendBuf[2] = lenth & 0x0ff; //更新帧长
            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;

            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt); //计算CRC校验字段
            AlarmSendBuf[8 + cnt] = crc16 & 0x00ff;
            AlarmSendBuf[9 + cnt] = (crc16 & 0xff00) >> 8;

            AlarmSendBuf[10 + cnt] = 0xa5; //添加帧尾
            AlarmSendBuf[11 + cnt] = 0xa5;

            Can_Network_Upcast((uint8_t *)&AlarmSendBuf, lenth + 4);

            First_Alarm = 1;
            cnt = 0; //子命令计数清零
            bsp_DelayMS(1);
        }
    }
}

/**********************************************************************************************************
**  函 数 名 : AlarmClear()
**  功能说明 : 清除内部和全局报警
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AlarmClear(void)
{
	uint8_t i;
	/*更新告警标志，重复检测结果不自动清零*/
	for(i = 0; i < 8; i++)
	{
		IP_Alarm_Flag[i] = 0;
		IP_Alarm_Flag_History[i] = 0;
		GP_Alarm_Flag[i] = 0;
		GP_Alarm_Flag_History[i] = 0;
	}
}
/**********************************************************************************************************
**  函 数 名 : AppTaskDcdc()
**  功能说明 : 保护任务
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AppTaskProtection(void *argument)
{
    osStatus_t OsSemRt;
    uint8_t i;
    uint16_t cnt = 0; //子命令计数
    uint16_t lenth;   //长度
    uint16_t crc16;   //校验码

    uint8_t AlarmSendBuf[1024] = {0};

    AlarmSendBuf[0] = 0x5B;
    AlarmSendBuf[1] = 0x5B; //下位机给中位机发，帧头0x5B5B

    AlarmSendBuf[2] = 0x09;
    AlarmSendBuf[3] = 0x00; //固定长度9

    AlarmSendBuf[4] = 0x00; //特征码，每次上报自加1
    AlarmSendBuf[5] = 0x00; //状态码=0，OK
    AlarmSendBuf[6] = 0x00; //通道号
    while (1)
    {
        OsSemRt = osSemaphoreAcquire(SemaProtection, osWaitForever);

        if (osOK == OsSemRt)
        {
            if(Reconnection_Flg == 0x01)
            {
                AlarmQuarry();	//重连告警上抛
                Reconnection_Flg = 0x00;
            }
            if (First_Alarm == 0) //避免保护重传  
            {
                /*内部保护，全局保护查询上抛，按通道查询*/
                for (i = 0; i < CHANNEL_MAX; i++)
                {
                    if (IP_Alarm_Flag[i] != 0) //内部告警标志非0
                    {
                        if (IP_Alarm_Flag[i] != IP_Alarm_Flag_History[i]) //内部告警标志非0且不等于历史值,准备上抛告警
                        {
                            Protect_Action_Func(i, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP); //执行告警动作
                            IP_Alarm_Flag_History[i] = IP_Alarm_Flag[i];                        //更新新的告警历史记录
                            AlarmSendBuf[4] = features++;                                       //特征码+1
                            AlarmSendBuf[6] = 0x01 << i;                                        //内部保护报警通道号
                            AlarmSendBuf[7] = 0xA0;                                             //主命令，内部保护报警,固定0xa0, 全局保护报警：0x83
                            for (uint8_t j = 0; j < sizeof(IP_CheckList) / sizeof(CMDProject); j++)   //告警子命令码填充
                            {
                                if (IP_CheckList[j].P_Flg & IP_Alarm_Flag[i])
                                {
                                    cnt++;
                                    AlarmSendBuf[7 + cnt] = IP_CheckList[j].childCMD;
                                }
                            }
                            
                            lenth = 8 + cnt;
                            AlarmSendBuf[2] = lenth & 0x00ff; //更新帧长
                            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;

                            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt); //计算CRC校验字段
                            AlarmSendBuf[8 + cnt] = crc16 & 0x00FF;
                            AlarmSendBuf[9 + cnt] = (crc16 & 0xFF00) >> 8;

                            AlarmSendBuf[10 + cnt] = 0xa5; //添加帧尾
                            AlarmSendBuf[11 + cnt] = 0xa5;

                            Can_Network_Upcast((uint8_t *)&AlarmSendBuf, lenth + 4);

                            cnt = 0; //子命令计数清零
                        }
                    }

                    cnt = 0; //子命令计数清零

                    if (GP_Alarm_Flag[i] != 0) //全局告警标志非0
                    {
                        if (GP_Alarm_Flag[i] != GP_Alarm_Flag_History[i]) //全局告警标志非0且不等于历史值
                        {
                            Protect_Action_Func(i, PROTECT_GLOBAL, WORKSTEP_RUNSTA_REF_STOP); //执行告警动作
                            GP_Alarm_Flag_History[i] = GP_Alarm_Flag[i];                      //更新新的告警历史记录
                            AlarmSendBuf[4] = features++;                                     //特征码+1
                            AlarmSendBuf[6] = 0x01 << i;                                      //报警通道号
                            AlarmSendBuf[7] = 0x83;                                           //主命令，内部保护报警,固定0xa0, 全局保护报警：0x83
                            for (uint8_t j = 0; j < sizeof(GP_CheckList) / sizeof(CMDProject); j++)
                            //添加子命令
                            {
                                if (GP_CheckList[j].P_Flg & GP_Alarm_Flag[i])
                                {
                                    cnt++;
                                    AlarmSendBuf[7 + cnt] = GP_CheckList[j].childCMD;
                                }
                            }
							
                            lenth = 8 + cnt;
							
                            AlarmSendBuf[2] = (lenth & 0x00ff); //更新帧长
                            AlarmSendBuf[3] = (lenth & 0xff00) >> 8;

                            crc16 = CRC16N((uint8_t *)&AlarmSendBuf[2], 6 + cnt); //计算CRC校验字段
                            AlarmSendBuf[8 + cnt] = crc16 & 0x00FF;
                            AlarmSendBuf[9 + cnt] = (crc16 & 0xFF00) >> 8;

                            AlarmSendBuf[10 + cnt] = 0xa5; //添加帧尾
                            AlarmSendBuf[11 + cnt] = 0xa5;

                            Can_Network_Upcast((uint8_t *)&AlarmSendBuf, lenth + 4);

                            cnt = 0; //子命令计数清零
                        }
                    }
                }
            }
            else
            {
                First_Alarm = 0;
                for (i = 0; i < CHANNEL_MAX; i++)
                {
                    if (IP_Alarm_Flag[i] != 0) //内部告警标志非0
                    {
                        if (IP_Alarm_Flag[i] != IP_Alarm_Flag_History[i]) //内部告警标志非0且不等于历史值,准备上抛告警
                        {
                            Protect_Action_Func(i, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP); //执行告警保护动作
                            IP_Alarm_Flag_History[i] = IP_Alarm_Flag[i];                        //更新新的告警历史记录
                        }
                    }

                    if (GP_Alarm_Flag[i] != 0) //全局告警标志非0
                    {
                        if (GP_Alarm_Flag[i] != GP_Alarm_Flag_History[i]) //全局告警标志非0且不等于历史值
                        {
                            Protect_Action_Func(i, PROTECT_GLOBAL, WORKSTEP_RUNSTA_REF_STOP); //执行告警动作
                            GP_Alarm_Flag_History[i] = GP_Alarm_Flag[i];                      //更新新的告警历史记录
                        }
                    }
                }
            }
        }
        else
        {
            // do nothing;
        }
        osDelay(PROTECTION_TASK_PERIOD);
    }
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/