/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Task_Init.c
** Author      : Cai Heng
** Date        : 2022-08-01
** Description : 
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "Task_Init.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/


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
**	函 数 名 : GlobalProtectDatInit()
**	功能说明 : 全局保护初始化
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void GlobalProtectDatInit(uint8_t chx)
{
	uint8_t j = chx;
	
	Tier2.gProt[j].VbatThrH.isEnable     = 0x81;	//最高为表示故障保护是否使能，为0 使能， 为1 不使能 默认不使能
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
	Tier2.gProt[j].Iovcharge.value    = 162;
	Tier2.gProt[j].Iovdischg.value    = 162;
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
**	功能说明 : 内部保护初始化
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
	 
    Tier2.iProt[j].IoutThr.value   = 168;  
	Tier2.iProt[j].VbusThrH.value  = 24;	
	Tier2.iProt[j].VbusThrL.value  = 16;	
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
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void CalibrateDatInit(uint8_t chx)
{
	uint8_t j = chx;

	// Tier2.calibrate[j].VsenseSetK_C = 1.0;                		 // 9838.254883;
	// Tier2.calibrate[j].VsenseSetB_C = 0;                         // -5.830758;
	// Tier2.calibrate[j].VmodSetK_C   = 1.0;                		 // 8930.311523;
	// Tier2.calibrate[j].VmodSetB_C   = 0;                         // 5.915592;
	// Tier2.calibrate[j].IhSetK_C     = 0;                        // 734.005493;
	// Tier2.calibrate[j].IhSetB_C     = 0;                         // 0;
	// Tier2.calibrate[j].IlSetK_C     = 1835.53503;                // 1835.535034;
	// Tier2.calibrate[j].IlSetB_C     = 0;                         // 0;
	// Tier2.calibrate[j].VsenseK_C    = 5.50281;                   // 5.499214;
	// Tier2.calibrate[j].VsenseB_C    = -0.329;                    // -0.329117;
	// Tier2.calibrate[j].VmodK_C      = 9.09;                      // 8.942759;
	// Tier2.calibrate[j].VmodB_C      = -0.25;                     // -0.272346;
	// Tier2.calibrate[j].IhK_C        = 98.23536;                  // 98.235359;
	// Tier2.calibrate[j].IhB_C        = 0;                         // 0;
	// Tier2.calibrate[j].IlK_C        = 39.28514;                  // 39.285141;
	// Tier2.calibrate[j].IlB_C        = 0;                         // 0;
	// Tier2.calibrate[j].VtzK_C       = 5.00094;                   // 4.998415;
	// Tier2.calibrate[j].VtzB_C       = 0;                         // 0.002736;

	// Tier2.calibrate[j].VsenseSetK_D = 9849.36328;                // 9838.254883;
	// Tier2.calibrate[j].VsenseSetB_D = 0;                         // -5.830758;
	// Tier2.calibrate[j].VmodSetK_D   = 8931.53027;                // 8930.311523;
	// Tier2.calibrate[j].VmodSetB_D   = 0;                         // 5.915592;
	// Tier2.calibrate[j].IhSetK_D     = 680;                       // 734.005493;
	// Tier2.calibrate[j].IhSetB_D     = 0;                         // 0;
	// Tier2.calibrate[j].IlSetK_D     = 1835.53503;                // 1835.535034;
	// Tier2.calibrate[j].IlSetB_D     = 0;                         // 0;
	// Tier2.calibrate[j].VsenseK_D    = 5.50281;                   // 5.499214;
	// Tier2.calibrate[j].VsenseB_D    = -0.329;                    // -0.329117;
	// Tier2.calibrate[j].VmodK_D      = 9.09;                      // 8.942759;
	// Tier2.calibrate[j].VmodB_D      = -0.25;                     // -0.272346;
	// Tier2.calibrate[j].IhK_D        = 98.23536;                  // 98.235359;
	// Tier2.calibrate[j].IhB_D        = 0;                         // 0;
	// Tier2.calibrate[j].IlK_D        = 39.28514;                  // 39.285141;
	// Tier2.calibrate[j].IlB_D        = 0;                         // 0;
	// Tier2.calibrate[j].VtzK_D       = 5.00094;                   // 4.998415;
	// Tier2.calibrate[j].VtzB_D       = 0;                         // 0.002736;
		
	MB85Ctrl_Read(0x00, &Tier2.calibrate, sizeof(Tier2.calibrate));  //读取铁电数据
	Tier2ToTier1(j, SYNC_TYPE_CALI);  //拷贝2阶结构体数据到1阶结构体
}

/**********************************************************************************************************
**	函 数 名 : EepromDatInit()
**	功能说明 :
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void EepromDatInit(void)
{
	for(int j = 0; j < CHANNEL_MAX; j++)
	{
		CalibrateDatInit(j);
		
		GlobalProtectDatInit(j);
		
		InternalProtectDatInit(j);
	}

    Tier2.calibrateEx.Vbus1K = 29.535;
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
