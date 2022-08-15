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
extern uint8_t write_flag_disable;

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
void AppTaskDebug(void *argument);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

uint8_t test_flag = 1;

float Vaux_set = 0.0f;

/**********************************************************************************************************
**	函 数 名 : 	AppTaskDebug()
**	功能说明 :	查询任务以及其他功能
**	形    参 : 	无
**	返 回 值 : 	无
**********************************************************************************************************/
void AppTaskDebug(void *argument)
{
    //  int j = 0;

//    uint32_t addr_fram = 0;
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
				
				AD5245_Vpro_Set(Vaux_set);
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
					
					AD5245_Vpro_Set(Vaux_set);
					
					break;
				}
			}

			/* 实时写FRAM逻辑 */
			addr_offset = sizeof(Tier2.workstep[0]) + sizeof(Tier2.workstepCache1[0]) + sizeof(Tier2.workstepCache2[0]) + \
						  sizeof(Tier2.workstepAnalyze[0]) + sizeof(Tier2.workstepRunning[0]) + sizeof(workstep_flag_AbortPrio[0]);

			if(write_flag_disable == 0)
			{
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
				}		
				
				uint8_t temp8_tt = 0x01;
				MB85Ctrl_CheckWrite(8192, &temp8_tt, 1);
				osDelay(5);
				
				for(uint32_t m = 0; m < addr_offset * 4; m++)
				{
					MB85Ctrl_CheckWrite(m, temp_arry + m, 1);
					osDelay(5);
				}
			}
			else
			{
				osDelay(500);
			}
			
			/* 写两次FRAM？ */
			if(write_flag_disable == 0)
			{
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
				}	
			
				uint8_t temp8_tt = 0x02;
				MB85Ctrl_CheckWrite(8192, &temp8_tt, 1);		
				osDelay(5);
			
				for(uint32_t m = 0; m < addr_offset * 4; m++)
				{
					MB85Ctrl_CheckWrite(4096 + m, temp_arry + m, 1);
					osDelay(5);
				}
			}
			else
			{
				osDelay(500);
			}
		
		}
	}
}
/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
