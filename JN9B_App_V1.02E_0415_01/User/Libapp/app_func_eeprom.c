/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : app_func_eemprom.c
** Author      : Cai Heng
** Date        : 2021-07-29
** Description :
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
uint8_t ucEepBuf[EEPROM_BUF_SIZE];		/* 1.不定义为EepDatLoad()及EepDatSync()内部变量，否则需要给任务 */
										/* 2.不使用malloc()及free()定义，操作结构体需要保证数组地址连续 */

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : Tier1ToTier2()
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void Tier1ToTier2(uint8_t chx, uint8_t type)
{
	switch(type)
	{
		case SYNC_TYPE_GPROT:
		{
			s_Tier1_ProtectStruct *p1 = &Tier1.gProt[chx].VbatThrH;
			s_Tier2_ProtectStruct *p2 = &Tier2.gProt[chx].VbatThrH;
			for(int k = 0; k < SCMD_GLOBAL_PROTECT_SET_END - 1; k++, p1++, p2++)
			{
				p2->isEnable = p1->cmd;
				p2->value    = p1->dat;
			}		
		}
		break;
		
		case SYNC_TYPE_IPROT:
		{
			s_Tier1_ProtectStruct *p1 = &Tier1.iProt[chx].IoutThr;
			s_Tier2_ProtectStruct *p2 = &Tier2.iProt[chx].IoutThr;
			for(int k = 0; k < SCMD_INTERNAL_PROTECT_SET_END - 1; k++, p1++, p2++)
			{
				p2->isEnable = p1->cmd;
				p2->value    = p1->dat;
			}			
		}
		break;
		
		case SYNC_TYPE_CALI:
		{
			memcpy(&Tier2.calibrate[chx], &Tier1.calibrate[chx], sizeof(Tier1.calibrate[chx]));
		}
		break;
		
		default:
		{
		
		}
		break;
	}
}

/**********************************************************************************************************
**	函 数 名 : Tier2ToTier1()
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void Tier2ToTier1(uint8_t chx, uint8_t type)
{
	switch(type)
	{
		case SYNC_TYPE_GPROT:
		{
			s_Tier1_ProtectStruct *p1 = &Tier1.gProt[chx].VbatThrH;
			s_Tier2_ProtectStruct *p2 = &Tier2.gProt[chx].VbatThrH;
			for(int k = 0; k < SCMD_GLOBAL_PROTECT_SET_END - 1; k++, p1++, p2++)
			{
				p1->cmd = p2->isEnable;
				p1->dat = p2->value;
			}			
		}
		break;
		
		case SYNC_TYPE_IPROT:
		{
			s_Tier1_ProtectStruct *p1 = &Tier1.iProt[chx].IoutThr;
			s_Tier2_ProtectStruct *p2 = &Tier2.iProt[chx].IoutThr;
			for(int k = 0; k < SCMD_INTERNAL_PROTECT_SET_END - 1; k++, p1++, p2++)
			{
				p1->cmd = p2->isEnable;
				p1->dat = p2->value;
			}			
		}
		break;
		
		case SYNC_TYPE_CALI:
		{
			memcpy(&Tier1.calibrate[chx], &Tier2.calibrate[chx], sizeof(Tier2.calibrate[chx]));
		}
		break;
		
		default:
		{
		
		}
		break;
	}
}

/**********************************************************************************************************
**	函 数 名 : EepDatLoad()
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void EepDatLoad(uint8_t id)
{
	for(int j = 0; j < CHANNEL_CFG; j++)
	{
		if(id == PROTECT_GLOBAL_ID(j))
		{
			/* 读取EEPROM数据并缓存到ucEepBuf中 */
			ee_ReadBytes(ucEepBuf, PROTECT_GLOBAL_BASEADDR(j), sizeof(PROTECT_GLOBAL_STRUCT(j)));
			/* 将缓存区数据赋给结构体 */
			memcpy(&PROTECT_GLOBAL_STRUCT(j), ucEepBuf, sizeof(PROTECT_GLOBAL_STRUCT(j)));		
			Tier1ToTier2(j, SYNC_TYPE_GPROT);
			return;
		}
	}	
	
	for(int j = 0; j < CHANNEL_CFG; j++)
	{
		if(id == PROTECT_INTERNAL_ID(j))
		{
			/* 读取EEPROM数据并缓存到ucEepBuf中 */
			ee_ReadBytes(ucEepBuf, PROTECT_INTERNAL_BASEADDR(j), sizeof(PROTECT_INTERNAL_STRUCT(j)));
			/* 将缓存区数据赋给结构体 */
			memcpy(&PROTECT_INTERNAL_STRUCT(j), ucEepBuf, sizeof(PROTECT_INTERNAL_STRUCT(j)));		
			Tier1ToTier2(j, SYNC_TYPE_IPROT);
			return;
		}
	}	

	for(int j = 0; j < CHANNEL_MAX; j++)
	{
		if(id == CALIBRATE_ID(j))
		{
			/* 读取EEPROM数据并缓存到ucEepBuf中 */
			ee_ReadBytes(ucEepBuf, CALIBRATE_BASEADDR(j), sizeof(CALIBRATE_STRUCT(j)));
			/* 将缓存区数据赋给结构体 */
			memcpy(&CALIBRATE_STRUCT(j), ucEepBuf, sizeof(CALIBRATE_STRUCT(j)));		
			Tier1ToTier2(j, SYNC_TYPE_CALI);
			return;
		}
	}
}

/**********************************************************************************************************
**	函 数 名 : EepDatSync()
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void EepDatSync(uint8_t id)
{
 int32_t ls,us;  
	
	for(int j = 0; j < CHANNEL_CFG; j++)
	{
		if(id == PROTECT_GLOBAL_ID(j))
		{
//			Tier2ToTier1(j, SYNC_TYPE_GPROT);
			/* 将结构体数据拷贝到缓存区 */
			memcpy(ucEepBuf, &PROTECT_GLOBAL_STRUCT(j), sizeof(PROTECT_GLOBAL_STRUCT(j)));
			/* 将缓存区数据写入到EEPROM */
			Dis_int();
			ls = osKernelLock(); 
			ee_WriteBytesWithCheck(ucEepBuf, PROTECT_GLOBAL_BASEADDR(j), sizeof(PROTECT_GLOBAL_STRUCT(j)));			
			us =osKernelUnlock();
			osKernelRestoreLock(us);
			osKernelRestoreLock(ls);
			En_int();
			bsp_DelayMS(1);	//必须添加，切换任务重置任务锁
			
			return;
		}
	}
	
	for(int j = 0; j < CHANNEL_CFG; j++)
	{
		if(id == PROTECT_INTERNAL_ID(j))
		{
//			Tier2ToTier1(j, SYNC_TYPE_IPROT);
			/* 将结构体数据拷贝到缓存区 */
			memcpy(ucEepBuf, &PROTECT_INTERNAL_STRUCT(j), sizeof(PROTECT_INTERNAL_STRUCT(j)));
			/* 将缓存区数据写入到EEPROM */
			Dis_int();
			ls = osKernelLock(); 
			ee_WriteBytesWithCheck(ucEepBuf, PROTECT_INTERNAL_BASEADDR(j), sizeof(PROTECT_INTERNAL_STRUCT(j)));
			us =osKernelUnlock();
			osKernelRestoreLock(us);
			osKernelRestoreLock(ls);
			En_int();	
			bsp_DelayMS(1);	//必须添加，切换任务重置任务锁
		
			return;
		}
	}	
	
	for(int j = 0; j < CHANNEL_MAX; j++)
	{
		if(id == CALIBRATE_ID(j))
		{
//			Tier2ToTier1(j, SYNC_TYPE_CALI);
			/* 将结构体数据拷贝到缓存区 */
			memcpy(ucEepBuf, &CALIBRATE_STRUCT(j), sizeof(CALIBRATE_STRUCT(j)));
			/* 将缓存区数据写入到EEPROM */
			Dis_int();
			ls = osKernelLock();             
			ee_WriteBytesWithCheck(ucEepBuf, CALIBRATE_BASEADDR(j), sizeof(CALIBRATE_STRUCT(j)));
			us =osKernelUnlock();
			osKernelRestoreLock(us);
			osKernelRestoreLock(ls);
			En_int();
			bsp_DelayMS(1);	//必须添加，切换任务重置任务锁
			
			return;
		}
	}
}

/**********************************************************************************************************
**	函 数 名 : GlobalProtectDatInit()
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void GlobalProtectDatInit(uint8_t chx)
{
	uint8_t j = chx;
	
	Tier2.gProt[j].VbatThrH.isEnable     = 0x81;
	Tier2.gProt[j].VbatThrL.isEnable     = 0x82;
	Tier2.gProt[j].Iovcharge.isEnable    = 0x83;
	Tier2.gProt[j].Iovdischg.isEnable    = 0x84;
	Tier2.gProt[j].Cov.isEnable          = 0x85;
	Tier2.gProt[j].VinitThrH.isEnable    = 0x86;
	Tier2.gProt[j].VinitThrL.isEnable    = 0x87;
	Tier2.gProt[j].Tcontact.isEnable     = 0x88;
	Tier2.gProt[j].Icontact.isEnable     = 0x89;
	Tier2.gProt[j].VcontactDiff.isEnable = 0x8A;
	
	Tier2.gProt[j].VbatThrH.value     = 4.5;
	Tier2.gProt[j].VbatThrL.value     = 3.0;
    
#ifdef	YCJ	
    Tier2.gProt[j].Iovcharge.value    = 32;
	Tier2.gProt[j].Iovdischg.value    = 32;
#else    
	Tier2.gProt[j].Iovcharge.value    = 162;
	Tier2.gProt[j].Iovdischg.value    = 162;
#endif

	Tier2.gProt[j].Cov.value          = 1000;
	Tier2.gProt[j].VinitThrH.value    = 4.2;
	Tier2.gProt[j].VinitThrL.value    = 3.2;
	Tier2.gProt[j].Tcontact.value     = 10;
	Tier2.gProt[j].Icontact.value     = 60;
	Tier2.gProt[j].VcontactDiff.value = 0.01;
	
	Tier2ToTier1(j, SYNC_TYPE_GPROT);
}

/**********************************************************************************************************
**	函 数 名 : InternalProtectDatInit()
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void InternalProtectDatInit(uint8_t chx)
{
	uint8_t j = chx;

	Tier2.iProt[j].IoutThr.isEnable   = 0x01;
	Tier2.iProt[j].VbusThrH.isEnable  = 0x02;	
	Tier2.iProt[j].VbusThrL.isEnable  = 0x03;	
	Tier2.iProt[j].IccDiff.isEnable   = 0x04;	
	Tier2.iProt[j].VcvDiff.isEnable   = 0x05;	
	Tier2.iProt[j].AmpVdrop.isEnable  = 0x06;	
	Tier2.iProt[j].CntVdrop.isEnable  = 0x07;	
	Tier2.iProt[j].Vovsense.isEnable  = 0x08;	
	Tier2.iProt[j].Totmod.isEnable    = 0x09;	
	Tier2.iProt[j].Totenv.isEnable    = 0x0A;	
	Tier2.iProt[j].VloopDiff.isEnable = 0x0B;
	Tier2.iProt[j].AmpVup.isEnable    = 0x0C;   
	Tier2.iProt[j].CntVup.isEnable    = 0x0D;   
	Tier2.iProt[j].AmpCup.isEnable    = 0x0E;   
	Tier2.iProt[j].CntCup.isEnable    = 0x0F;
	Tier2.iProt[j].VauxThr.isEnable   = 0x10;
	Tier2.iProt[j].SNERR.isEnable     = 0x11;
	
#ifdef	YCJ	
	Tier2.iProt[j].IoutThr.value   = 132;	//YCJ 电流上限
#else    
	Tier2.iProt[j].IoutThr.value   = 168;
#endif    
	Tier2.iProt[j].VbusThrH.value  = 17;	
	Tier2.iProt[j].VbusThrL.value  = 12;	
	Tier2.iProt[j].IccDiff.value   = 3.2;	
	Tier2.iProt[j].VcvDiff.value   = 0.1;	
	Tier2.iProt[j].AmpVdrop.value  = 0.05;	
	Tier2.iProt[j].CntVdrop.value  = 5;	
	Tier2.iProt[j].Vovsense.value  = 0;	
	Tier2.iProt[j].Totmod.value    = 90;	
	Tier2.iProt[j].Totenv.value    = 55;	
	Tier2.iProt[j].VloopDiff.value = 1.5;
	Tier2.iProt[j].AmpVup.value    = 0.05;   
	Tier2.iProt[j].CntVup.value    = 5;   
	Tier2.iProt[j].AmpCup.value    = 2;   
	Tier2.iProt[j].CntCup.value    = 5;
	Tier2.iProt[j].VauxThr.value   = 5.1;
	Tier2.iProt[j].SNERR.value     = 0.01;
	
	Tier2ToTier1(j, SYNC_TYPE_IPROT);
}

/**********************************************************************************************************
**	函 数 名 : CalibrateDatInit()
**	功能说明 : 默认K、B系数
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void CalibrateDatInit(uint8_t chx)
{
	uint8_t j = chx;
	
#ifdef	YCJ
	Tier2.calibrate[j].VsenseSetK_C = 10818.486328;	//充电-电池电压基准设定参数K
	Tier2.calibrate[j].VsenseSetB_C = 0;
	Tier2.calibrate[j].VmodSetK_C   = 8931.53027;	//充电-模块端口电压基准设定参数K
	Tier2.calibrate[j].VmodSetB_C   = 0;
	Tier2.calibrate[j].IhSetK_C     = 476; 			//修改1 充电-高档位电流基准设定参数K
	Tier2.calibrate[j].IhSetB_C     = 0;   			//修改1
	Tier2.calibrate[j].IlSetK_C     = 1835.53503;	//充电-低档位电流基准设定参数K
	Tier2.calibrate[j].IlSetB_C     = 0;
	Tier2.calibrate[j].VsenseK_C    = 3;			//充电-电池电压采样参数K
	Tier2.calibrate[j].VsenseB_C    = 0;
	Tier2.calibrate[j].VmodK_C      = 13;			//充电-模块端口电压采样参数K
	Tier2.calibrate[j].VmodB_C      = -0.329;
	Tier2.calibrate[j].IhK_C        = 140.68;		//修改1 充电-输出电流高档位采样参数K
	Tier2.calibrate[j].IhB_C        = 0;			//修改1 
	Tier2.calibrate[j].IlK_C        = 39.28514; 	//充电-输出电流低档位采样参数K
	Tier2.calibrate[j].IlB_C        = 0;
	Tier2.calibrate[j].VtzK_C       = 6.184;		//充电-探针电压采样参数K
	Tier2.calibrate[j].VtzB_C       = 0;

	Tier2.calibrate[j].VsenseSetK_D = 9849.36328;	//放电-电池电压基准设定参数K
	Tier2.calibrate[j].VsenseSetB_D = 0;
	Tier2.calibrate[j].VmodSetK_D   = 8931.53027;	//放电-模块端口电压基准设定参数K
	Tier2.calibrate[j].VmodSetB_D   = 0;
	Tier2.calibrate[j].IhSetK_D     = 476;			//修改1 放电-高档位电流基准设定参数K
	Tier2.calibrate[j].IhSetB_D     = 0;			//修改1
	Tier2.calibrate[j].IlSetK_D     = 1835.53503;	//放电-低档位电流基准设定参数K
	Tier2.calibrate[j].IlSetB_D     = 0;
	Tier2.calibrate[j].VsenseK_D    = 3;			//放电-电池电压采样参数K
	Tier2.calibrate[j].VsenseB_D    = 0;
	Tier2.calibrate[j].VmodK_D      = 13;			//放电-模块端口电压采样参数K
	Tier2.calibrate[j].VmodB_D      = -0.329;
	Tier2.calibrate[j].IhK_D        = 140.68;		//修改1 放电-输出电流高档位采样参数K
	Tier2.calibrate[j].IhB_D        = 0;			//修改1
	Tier2.calibrate[j].IlK_D        = 39.28514;		//放电-输出电流低档位采样参数K
	Tier2.calibrate[j].IlB_D        = 0;
	Tier2.calibrate[j].VtzK_D       = 6.184;		//放电-探针电压采样参数K
	Tier2.calibrate[j].VtzB_D       = 0;    
	
#else    
	Tier2.calibrate[j].VsenseSetK_C = 9849.36328;	// 9838.254883; 充电-电池电压基准设定参数K
	Tier2.calibrate[j].VsenseSetB_C = 0;			// -5.830758; 充电-电池电压基准设定参数B
	Tier2.calibrate[j].VmodSetK_C   = 8931.53027;	// 8930.311523;
	Tier2.calibrate[j].VmodSetB_C   = 0;			// 5.915592;
	Tier2.calibrate[j].IhSetK_C     = 680;			// 734.005493; 充电-高档位电流基准设定参数K
	Tier2.calibrate[j].IhSetB_C     = 0;			// 0;
	Tier2.calibrate[j].IlSetK_C     = 1835.53503;	// 1835.535034;
	Tier2.calibrate[j].IlSetB_C     = 0;			// 0;
	Tier2.calibrate[j].VsenseK_C    = 5.50281;		// 5.499214;
	Tier2.calibrate[j].VsenseB_C    = -0.329;		// -0.329117;
	Tier2.calibrate[j].VmodK_C      = 9.09;			// 8.942759;
	Tier2.calibrate[j].VmodB_C      = -0.25;		// -0.272346;
	Tier2.calibrate[j].IhK_C        = 98.23536;		// 98.235359;
	Tier2.calibrate[j].IhB_C        = 0;			// 0;
	Tier2.calibrate[j].IlK_C        = 39.28514;		// 39.285141;
	Tier2.calibrate[j].IlB_C        = 0;			// 0;
	Tier2.calibrate[j].VtzK_C       = 5.00094;		// 4.998415;
	Tier2.calibrate[j].VtzB_C       = 0;			// 0.002736;

	Tier2.calibrate[j].VsenseSetK_D = 9849.36328;	// 9838.254883;
	Tier2.calibrate[j].VsenseSetB_D = 0;			// -5.830758;
	Tier2.calibrate[j].VmodSetK_D   = 8931.53027;	// 8930.311523;
	Tier2.calibrate[j].VmodSetB_D   = 0;			// 5.915592;
	Tier2.calibrate[j].IhSetK_D     = 680;			// 734.005493;
	Tier2.calibrate[j].IhSetB_D     = 0;			// 0;
	Tier2.calibrate[j].IlSetK_D     = 1835.53503;	// 1835.535034;
	Tier2.calibrate[j].IlSetB_D     = 0;			// 0;
	Tier2.calibrate[j].VsenseK_D    = 5.50281;		// 5.499214;
	Tier2.calibrate[j].VsenseB_D    = -0.329;		// -0.329117;
	Tier2.calibrate[j].VmodK_D      = 9.09;			// 8.942759;
	Tier2.calibrate[j].VmodB_D      = -0.25;		// -0.272346;
	Tier2.calibrate[j].IhK_D        = 98.23536;		// 98.235359;
	Tier2.calibrate[j].IhB_D        = 0;			// 0;
	Tier2.calibrate[j].IlK_D        = 39.28514;		// 39.285141; 放电-输出电流低档位采样参数K
	Tier2.calibrate[j].IlB_D        = 0;			// 0;
	Tier2.calibrate[j].VtzK_D       = 5.00094;		// 4.998415;
	Tier2.calibrate[j].VtzB_D       = 0;			// 0.002736;
#endif	
	Tier2ToTier1(j, SYNC_TYPE_CALI);
}

/**********************************************************************************************************
**	函 数 名 : EepromDatInit()
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void EepromDatInit(void)
{
	uint8_t  ucBuf[2];
	uint16_t usage_flag = 0;
	
	/* 校准参数 */
	ee_ReadBytes(ucBuf, CALIBRATE_USAGE_FLAG_ADDR, 2);
	memcpy(&usage_flag, ucBuf, 2);
	if(usage_flag == CALIBRATE_USED)
	{
		for(int j = 0; j < CHANNEL_MAX; j++)
		{
			EEP_DAT_LOAD(CALIBRATE_ID(j));
		}	
	}
	else /* if(usage_flag == CALIBRATE_UNUSED) */
	{
		usage_flag = CALIBRATE_USED;
		memcpy(ucBuf, &usage_flag, 2);
		ee_WriteBytes(ucBuf, CALIBRATE_USAGE_FLAG_ADDR, 2);
		for(int j = 0; j < CHANNEL_MAX; j++)
		{
			CalibrateDatInit(j);
			EEP_DAT_SYNC(CALIBRATE_ID(j));
		}	
	}
	
	/* 全局保护参数 */
	ee_ReadBytes(ucBuf, PROTECT_GLOBAL_USAGE_FLAG_ADDR, 2);
	memcpy(&usage_flag, ucBuf, 2);	
	if(usage_flag == PROTECT_GLOBAL_USED)
	{
		for(int j = 0; j < CHANNEL_MAX; j++)
		{
			EEP_DAT_LOAD(PROTECT_GLOBAL_ID(j));
		}	
	}
	else /* if(usage_flag == PROTECT_GLOBAL_UNUSED) */
	{
		usage_flag = PROTECT_GLOBAL_USED;
		memcpy(ucBuf, &usage_flag, 2);
		ee_WriteBytes(ucBuf, PROTECT_GLOBAL_USAGE_FLAG_ADDR, 2);		
		for(int j = 0; j < CHANNEL_MAX; j++)
		{
			GlobalProtectDatInit(j);
			EEP_DAT_SYNC(PROTECT_GLOBAL_ID(j));
		}			
	}
	
	/* 内部保护参数 */
	ee_ReadBytes(ucBuf, PROTECT_INTERNAL_USAGE_FLAG_ADDR, 2);
	memcpy(&usage_flag, ucBuf, 2);	
	if(usage_flag == PROTECT_INTERNAL_USED)
	{
		for(int j = 0; j < CHANNEL_MAX; j++)
		{
			EEP_DAT_LOAD(PROTECT_INTERNAL_ID(j));
		}	
	}
	else /* if(usage_flag == PROTECT_INTERNAL_UNUSED) */
	{
		usage_flag = PROTECT_INTERNAL_USED;
		memcpy(ucBuf, &usage_flag, 2);
		ee_WriteBytes(ucBuf, PROTECT_INTERNAL_USAGE_FLAG_ADDR, 2);		
		for(int j = 0; j < CHANNEL_MAX; j++)
		{
			InternalProtectDatInit(j);
			EEP_DAT_SYNC(PROTECT_INTERNAL_ID(j));
		}			
	}	

	Tier2.calibrateEx.Vbus1K = 20.976;	//29.535;
	Tier2.calibrateEx.Vbus1B = -0.12655;
	Tier2.calibrateEx.Vbus2K = 29.535;
	Tier2.calibrateEx.Vbus2B = -0.12655;
	Tier2.calibrateEx.VauxK = 20.83333;
	Tier2.calibrateEx.VauxB = 0;
	Tier2.calibrateEx.TambK = 167.33333;
	Tier2.calibrateEx.TambB = -50;
	Tier2.calibrateEx.TmodK = 183.33333;
	Tier2.calibrateEx.TmodB = -50;
}


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
