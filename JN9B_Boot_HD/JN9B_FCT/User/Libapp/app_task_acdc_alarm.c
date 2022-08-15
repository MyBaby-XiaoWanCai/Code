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
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t features = 0; //特征码
uint16_t ACDC_a1Flg=0; //模块1被动清零报警标志，靠上位机发命令清除
uint16_t ACDC_a2Flg=0; //模块2被动清零报警标志，靠上位机发命令清除
/*国电报警状态*/
enum gd_state
{
    GDAC_OV = 0x01 << 5,  //AC过压，state0
    GDAC_UV = 0x01 << 6,  //AC欠压, state0
    GDAC_OF = 0x01 << 4,  //AC过频, state0
    GDGD_PT = 0x01 << 7,  //孤岛保护, state0
    GDDC_OV = 0x01 << 10, //DC过压, state1
    GDDC_OA = 0x01 << 11, //DC过流, state1
    GDDC_ST = 0x01 << 9,  //DC短路, state1
    GDFN_ER = 0x01 << 3,  //客户风扇故障|ACDC风扇故障, state0, 非ACDC保护，归为内部保护
    GDTM_OT = 0x01 << 8,  //过温保护, state1
};

/*国电报警子命令查找表*/
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

/*普德报警状态*/
enum pd_state
{
    PDAC_OV = 0x01 << 4,  //AC过压
    PDAC_UV = 0x01 << 5,  //AC欠压
    PDAC_OF = 0x01 << 6,  //AC过频
    PDAC_UF = 0x01 << 7,  //AC欠频
    PDAC_OP = 0x01 << 8,  //AC断电
    PDGD_PT = 0x01 << 9,  //孤岛保护
    PDDC_OV = 0x01 << 10, //DC过压
    PDDC_OA = 0x01 << 12, //DC过流
    PDDC_ST = 0x01 << 11, //DC短路
    PDTM_OT = 0x01 << 13, //过温保护
    PDFN_ER1 = 0x01 << 14, //客户风扇故障，非ACDC保护，归为内部保护
    PDFN_ER2 = 0x01 << 15, //内部风扇故障
};

/*普德报警子命令查找表*/
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

acdc_alarmStruct acdc_AlarmBuf;   //网口状态报警发送buf
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : AppACDC_alarm()
**  功能说明 : ACDC告警上报任务
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
void AppACDC_alarm(void *argument)
{
    uint16_t alarm1, alarm2; //bit=1的位是有变化且状态为告警的位
    uint8_t i;

//	int test_printf_acdc = 0;

    while(1)
    {
        if(osSemaphoreAcquire(acdc_alarm_Semaphore, osWaitForever) == osOK) //等待同步信号量
        {
            /*ACDC报警，帧头2字节+数据长度2字节+特征码1字节+状态码1字节+通道标志1字节+功能字段2字节（命令码+子命令）+校验2字节+帧尾2字节*/
            acdc_AlarmBuf.FramHead = 0x5B5B;   //下位机给中位机发，帧头0x5B5B
            acdc_AlarmBuf.Datelen = 0x09;      //固定长度9
            acdc_AlarmBuf.FeatureCode = 0x00;  //特征码，每次上报自加1
            acdc_AlarmBuf.StateCode = 0x00;    //状态码=0，OK
            acdc_AlarmBuf.ChanelFlag = 0x00;   //ACDC报警时，固定0
            acdc_AlarmBuf.CommandCode = 0xA1;  //ACDC报警,固定
            acdc_AlarmBuf.ChildCommand = 0x00; //根据can上报内容和子命令码定义确定
            acdc_AlarmBuf.CRC16Code = 0x00;    //需最后计算
            acdc_AlarmBuf.FramTail = 0xA5A5;   //帧尾

            //解析ACDC当前状态，向网口上报数据，记录有变化,且更新后的值为1的bit
            alarm1 = ACDC.BitFlg[0] & ACDC.State[0];
            alarm2 = ACDC.BitFlg[1] & ACDC.State[1];

            //判断故障所在通道
            if(alarm1)
            {
                acdc_AlarmBuf.ChanelFlag |= 0x00;   //ACDC报警不区分canid，通道标志为输出通道，报警时默认0
            }
            if(alarm2)
            {
                acdc_AlarmBuf.ChanelFlag |= 0x00;  //ACDC报警不区分canid，通道标志为输出通道，报警时默认0
            }

            //根据使用的ACDC类型，按厂家协议手册确定报警类型
            switch(ACDC.Type)
            {
            case 1:    //国电报警处理
                for(i = 0; i < 8; i++)
                {
                    if(alarm1 & gdlist[i].state)
                    {
						Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);   
                        acdc_AlarmBuf.ChildCommand = gdlist[i].ChildCom; //查表获取子命令码
                        acdc_AlarmBuf.FeatureCode = features++;         //特征码+1
                        acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //计算CRC校验字段
                        W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //网口发送报警信息，0为服务器socket
                    }
                    if(alarm2 & gdlist[i].state)
                    {
						Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);        
                        acdc_AlarmBuf.ChildCommand = gdlist[i].ChildCom; //查表获取子命令码
                        acdc_AlarmBuf.FeatureCode = features++;         //特征码+1
                        acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //计算CRC校验字段
                        W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //网口发送报警信息，0为服务器socket
                    }
                }
                /*客户风扇故障，触发内部保护，注意：因国电协议不区分ACDC风扇和客户风扇故障，此处内部保护报警可能是误报*/
                if((alarm1 & GDFN_ER) | (alarm2 & GDFN_ER))
                {
					Protect_BitAction_Func(ALLCH, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP);   
                        for(uint8_t j = 0; j < CHANNEL_CFG ; j++)
                        { IP_Alarm_Flag[j] |= IntelProtect_FHE; }   //内部风扇告警
                }

                break;

            case 2:   //普德报警处理
                for(i = 0; i < 11; i++)
                {
                    if(alarm1 & pdlist[i].state)
                    {
						Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP);    
                        acdc_AlarmBuf.ChildCommand = pdlist[i].ChildCom; //查表获取子命令码
                        acdc_AlarmBuf.FeatureCode = features++;         //特征码+1
                        acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //计算CRC校验字段
                        W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //网口发送报警信息，0为服务器socket
                    }
                    if(alarm2 & pdlist[i].state)
                    {
						Protect_BitAction_Func(ALLCH, PROTECT_ACDC, WORKSTEP_RUNSTA_REF_STOP); 
                        acdc_AlarmBuf.ChildCommand = pdlist[i].ChildCom; //查表获取子命令码
                        acdc_AlarmBuf.FeatureCode = features++;         //特征码+1
                        acdc_AlarmBuf.CRC16Code = CRC16N((uint8_t *)&acdc_AlarmBuf.Datelen, 7); //计算CRC校验字段
                        W5500_send(0, (uint8_t *)&acdc_AlarmBuf.FramHead, 13);    //网口发送报警信息，0为服务器socket
                    }
                }

                /*客户风扇故障，触发内部保护*/
                if((alarm1 & PDFN_ER1) | (alarm2 & PDFN_ER1))
                {
					Protect_BitAction_Func(ALLCH, PROTECT_INTERNAL, WORKSTEP_RUNSTA_REF_STOP);   
                        for(uint8_t j = 0; j < CHANNEL_CFG ; j++)
                        { IP_Alarm_Flag[j] |= IntelProtect_FHE; }   //内部风扇告警
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
