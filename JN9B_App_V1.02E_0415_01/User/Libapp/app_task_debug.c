/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :JN95
** --------------------------------------------------------------------------------------------------------
** File name   : AppTaskdebug.c
** Author      : Robert.Li
** Date        : 2021-07-05
** Description : debug application task file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
extern	osSemaphoreId_t dbg_Semaphore;
extern uint32_t IP_Alarm_Flag[8];       //内部保护标志位

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void AppTaskDebug(void *argument);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

uint8_t test_flag = 1;

float Vaux_set = 0.0f;

uint8_t power_off_continue[8] = {0};

extern uint8_t write_flag_disable[8];

/**********************************************************************************************************
**	函 数 名 : 	AppTaskDebug()
**	功能说明 :	查询任务以及其他功能
**	形    参 : 	无
**	返 回 值 : 	无
**********************************************************************************************************/
void AppTaskDebug(void *argument)
{
    uint32_t addr_offset = 0;
	
    while(1)
    {
			if(test_flag)
			{
					test_flag = 0;
			}

			if(g_ucSysMode == SYS_CALIBRATION_MODE)
			{
				if(Vaux_set != 8.0f)
				{
					Vaux_set = 8.0f;
					
					AD5245_Vpro_Set(Bovp, Vaux_set);
				}
			}
			else
			{
				if((W5500_IP_ADDRESS[2] == 8) && (W5500_IP_ADDRESS[3] == 42))
				{
					if(Vaux_set != 8.0f)
					{
						Vaux_set = 8.0f;
						
						AD5245_Vpro_Set(Bovp, Vaux_set);
					}

				}
				else
				{
					for(int i = 0; i < CHANNEL_CFG; i++)
					{
						if(Vaux_set != Tier2.iProt[i].VauxThr.value)
						{
							Vaux_set = Tier2.iProt[i].VauxThr.value;

							for(int k = 0; k < CHANNEL_CFG; k++)
							{
								Tier2.iProt[k].VauxThr.value = Vaux_set;
							}
							
							AD5245_Vpro_Set(Bovp, Vaux_set);
							
							break;
						}
					}

				}
			}

			/* 实时写FRAM逻辑 */
			addr_offset = sizeof(Tier2.workstep[0]) + sizeof(Tier2.workstepCache1[0]) + sizeof(Tier2.workstepCache2[0]) + \
							sizeof(Tier2.workstepAnalyze[0]) + sizeof(Tier2.workstepRunning[0]) + sizeof(workstep_flag_AbortPrio[0]);

			uint8_t temp_arry[CHANNEL_CFG * 1024] = {0};
			uint32_t temp_array_offset = 0;
					
			for(int i = 0; i < CHANNEL_CFG; i++)
			{
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&Tier2.workstep[i]         , sizeof(Tier2.workstep[i])         );
				temp_array_offset += sizeof(Tier2.workstep[i]);
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&Tier2.workstepCache1[i]   , sizeof(Tier2.workstepCache1[i])   );
				temp_array_offset += sizeof(Tier2.workstepCache1[i]);
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&Tier2.workstepCache2[i]   , sizeof(Tier2.workstepCache2[i])   );
				temp_array_offset += sizeof(Tier2.workstepCache2[i]);
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&Tier2.workstepAnalyze[i]  , sizeof(Tier2.workstepAnalyze[i])  );
				temp_array_offset += sizeof(Tier2.workstepAnalyze[i]);
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&Tier2.workstepRunning[i]  , sizeof(Tier2.workstepRunning[i])  );
				temp_array_offset += sizeof(Tier2.workstepRunning[i]);
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&workstep_flag_AbortPrio[i], sizeof(workstep_flag_AbortPrio[i]));
				temp_array_offset += sizeof(workstep_flag_AbortPrio[i]);
								
				if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_START  ) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_WAIT   ) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_PAUSE  ) )
				{
					write_flag_disable[i] = 1;
				}
				else
				{
					write_flag_disable[i] = 0;
				}			
			}
		
			uint8_t temp8_tt = 0x01;
			MB85Ctrl_Write(8192, &temp8_tt, 1);
			osDelay(5);
					
			for(int i = 0; i < CHANNEL_CFG; i++)
			{
				if(write_flag_disable[i] == 1)
				{
					write_flag_disable[i] = 0;
					
					MB85Ctrl_Write(addr_offset * i, temp_arry + addr_offset * i, addr_offset);
					
					osDelay(250);
				}
				else
				{
					osDelay(250);
				}
			}
					
			memset(temp_arry, 0, 4096);
			temp_array_offset = 0;

			for(int i = 0; i < CHANNEL_CFG; i++)
			{
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&Tier2.workstep[i]         , sizeof(Tier2.workstep[i])         );
				temp_array_offset += sizeof(Tier2.workstep[i]);
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&Tier2.workstepCache1[i]   , sizeof(Tier2.workstepCache1[i])   );
				temp_array_offset += sizeof(Tier2.workstepCache1[i]);
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&Tier2.workstepCache2[i]   , sizeof(Tier2.workstepCache2[i])   );
				temp_array_offset += sizeof(Tier2.workstepCache2[i]);
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&Tier2.workstepAnalyze[i]  , sizeof(Tier2.workstepAnalyze[i])  );
				temp_array_offset += sizeof(Tier2.workstepAnalyze[i]);
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&Tier2.workstepRunning[i]  , sizeof(Tier2.workstepRunning[i])  );
				temp_array_offset += sizeof(Tier2.workstepRunning[i]);
				memcpy(temp_arry + temp_array_offset, (uint8_t *)&workstep_flag_AbortPrio[i], sizeof(workstep_flag_AbortPrio[i]));
				temp_array_offset += sizeof(workstep_flag_AbortPrio[i]);
				
				if( (Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_RUNNING) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_CONTACT) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_START  ) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_WAIT   ) || \
					(Tier2.workstepRunning[i].RunSta == WORKSTEP_RUNSTA_REF_PAUSE  ) )
				{
					write_flag_disable[i] = 1;
				}
				else
				{
					write_flag_disable[i] = 0;
				}
			}	
				
			temp8_tt = 0x02;
			MB85Ctrl_Write(8192, &temp8_tt, 1);		
			osDelay(5);
				
			for(int i = 0; i < CHANNEL_CFG; i++)
			{
				if(write_flag_disable[i] == 1)
				{				
					write_flag_disable[i] = 0;
					
					MB85Ctrl_Write(4096 + addr_offset * i, temp_arry + addr_offset * i, addr_offset);
					
					osDelay(250);
				}
				else
				{
					osDelay(250);
				}
			}
	}
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
