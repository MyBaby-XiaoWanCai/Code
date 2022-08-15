/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_tmp75.c
** Author      : Huang Cheng
** Date        : 2022-01-18
** Description : bsp_tmp75.c source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"


/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
float ADCx_Temp[8] = {0};	

/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : TmpI2C_SendData()
**	功能说明 : 主机向从机发送配置数据，用于配置寄存器
**	形    参 : SlaveAddr：从机地址（高7位是从机地址，最低位是写标志0）;这里为0X90
**	返 回 值 : 转换状态，0 - OK，1 - ERR
**********************************************************************************************************/
void TmpI2C_SendData( I2C_DevType DevNum , uint8_t SlaveAddr , uint8_t Reg ,uint16_t date)
{
    SlaveAddr &= 0xFE;//确保从机地址最低位是0，即需写地址操作
    i2c_Start(DevNum);//启动I2C总线
    i2c_SendByte(DevNum,SlaveAddr);//发送芯片地址
    i2c_WaitAck(DevNum);
    i2c_SendByte(DevNum,Reg);      //发送寄存器地址，配置寄存器
    i2c_WaitAck(DevNum);
    if((Reg==TL_Reg)||(Reg==TH_Reg))
    {
        i2c_SendByte(DevNum,(uint8_t)((date&0xFF00)>>8));  //设置寄存器值。      
        i2c_WaitAck(DevNum);
        i2c_SendByte(DevNum,(uint8_t)(date&0x00FF));  //设置寄存器值.
    }
    else
    {
        i2c_SendByte(DevNum,(uint8_t)(date&0x00FF));  //设置寄存器值。
    }
    i2c_WaitAck(DevNum);
    i2c_Stop(DevNum);           //发送完毕，停止I2C总线
}


/**********************************************************************************************************
**	函 数 名 : TmpI2C_ReadData()
**	功能说明 : 主机通过I2C总线向从机读取寄存器值
**	形    参 : SlaveAddr：从机地址（高7位是从机地址，最低位是写标志0）;这里为0x90
               Temp： 数据存储指针
**	返 回 值 : 转换状态，0 - OK，1 - ERR
**********************************************************************************************************/
void TmpI2C_ReadData(I2C_DevType DevNum , uint8_t SlaveAddr , uint8_t Reg , uint16_t* Temp)
{
    SlaveAddr &= 0xFE;	//确保最低位是0

    i2c_Start(DevNum);//启动I2C总线

    i2c_SendByte(DevNum,SlaveAddr);//SlaveAddr  发送从机地址
    i2c_WaitAck(DevNum);

    i2c_SendByte(DevNum, Reg);//SubAddr       指向要读取的寄存器地址
    i2c_WaitAck(DevNum);

    i2c_Start(DevNum);//发送重复起始条件

    SlaveAddr |= 0x01;//地址读操作
    i2c_SendByte(DevNum,SlaveAddr);
    i2c_WaitAck(DevNum);
    if((Reg==TL_Reg)||(Reg==TH_Reg)||(Reg==TMP_Reg))
    {
        *Temp = i2c_ReadByte(DevNum)<<8;//高位
        i2c_Ack(DevNum);
        *Temp |= i2c_ReadByte(DevNum);//低位
    }
    else
    {
        *Temp = i2c_ReadByte(DevNum);
    }

    i2c_Ack(DevNum);
    i2c_Stop(DevNum);//接收完毕，停止I2C总线
}

/**********************************************************************************************************
**	函 数 名 : bsp_Tmp75_AlertInit()
**	功能说明 : TMP75 IO初始化 初始化ALERT脚
**	形    参 :
**	返 回 值 :
**********************************************************************************************************/
void bsp_Tmp75_AlertInit(void)
{
    stc_gpio_init_t stcCVKGpioInit;
    /* 设置OTP信号 pin的属性:开漏输入 , 无上拉 ，初始化状态为RESET等 */
		stcCVKGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_StructInit(&stcCVKGpioInit);    
    (void)GPIO_Init(GPIO_PORT_AMBIENTTEMP,  GPIO_PIN_AMBIENTTEMP,  &stcCVKGpioInit);
//		GPIO_InMOSCmd(GPIO_PORTE_IDX,Enable);
}

/**********************************************************************************************************
**	函 数 名 : bsp_Tmp75Init()
**	功能说明 : TMP75初始化
**	形    参 :
**	返 回 值 : 转换后的温度值
**********************************************************************************************************/
void bsp_Tmp75Init(I2C_DevType DevNum , uint8_t TmpI2CAddr)
{
    uint16_t cfgDate = 0x60;       //配置寄存器数据，为12位分辨率，并且设置连续温度转换，关闭shutdown模式，比较器模式打开，输出极性-。SD=0 ,TM=0 ,POL=0 ,F0=1 ,F1=1
		float TL = 83.0f;   //TL=83℃
    float TH = 85.0f;   //TH=85℃
	
//	  if(DevNum==I2C_EE_TEMP)	//630套整机验证 2022.04.03
//			cfgDate = 0x60;
//		else
//			cfgDate = 0x64;              //功率板OTP反接	
    
    TmpI2C_SendData(DevNum , TmpI2CAddr , CFG_Reg , cfgDate);     //设置配置寄存器
		bsp_DelayMS(500);
		Feed_WDG();  
    cfgDate=0;
    TmpI2C_ReadData(DevNum , TmpI2CAddr , CFG_Reg , &cfgDate); 
    
    Tmp75Value_SetTH(DevNum , TmpI2CAddr ,  TH);            //设置OTP TH
    bsp_DelayMS(200);
		Feed_WDG();  
    Tmp75Value_SetTL(DevNum , TmpI2CAddr ,  TL);            //设置OTP TL   
    bsp_DelayMS(200);
		Feed_WDG();  
}

/**********************************************************************************************************
**	函 数 名 : Tmp75Value_Get()
**	功能说明 : 读取温度值
**	形    参 :
**	返 回 值 : 转换后的温度值
**********************************************************************************************************/
float Tmp75Value_Get(I2C_DevType DevNum , uint8_t TmpI2CAddr)
{
    uint8_t Tmp1 = 0,Tmp2 = 0;
    uint16_t TmpBuf;	//用于存放人温度寄存器读取的两个字节

    float TmpValue;  //温度值
    float symbol;    //+ -符号

    TmpI2C_ReadData(DevNum , TmpI2CAddr , TMP_Reg , &TmpBuf);          //接收数据   0x00为读取的温度寄存器地址

    //计算整数部分
    if(((TmpBuf>>15)&0x01)==0)    //最高位=0，则为正温
    {
        Tmp1 = TmpBuf>>8;      //高8位温度整数部分
        Tmp2 = (uint8_t)(TmpBuf&0x00FF) >> 4; //低4位温度小数部分
        symbol= 1.0f;           //符号为+
    }
    else                         //最高位为1，则为负温
    {
        TmpBuf=~TmpBuf+1;        //求补码
        Tmp1 = TmpBuf>>8;      //高8位温度整数部分
        Tmp2 = (uint8_t)(TmpBuf&0x00FF) >> 4; //低4位温度小数部分
        symbol= -1.0f;               //符号为-
    }

    TmpValue = symbol*(Tmp1 + Tmp2*0.0625f);//小数和整数相加*符号位

    return TmpValue;
}

/**********************************************************************************************************
**	函 数 名 : Tmp75Value_SetTH()
**	功能说明 : 设置OTP高温限制
**	形    参 :
**	返 回 值 : 
**********************************************************************************************************/
void Tmp75Value_SetTH(I2C_DevType DevNum , uint8_t TmpI2CAddr ,float tempH)
{
  float zs,xs; 
  int16_t zz;          //整数部分
  uint16_t xz;         //小数部分
    
  uint16_t TH_Val;  //TH写入值
  DDL_ASSERT((tempH<=125.0f)&&(tempH>=-40.0f));    //断言判断温度范围是否符合设置范围
  zs=floor(tempH);        //获取整数部分
  xs=fabs(tempH- zs);     //获取小数部分
    
  xz = (uint16_t)(xs/0.0625f+0.5f);    //计算小数部分二进制表示,四舍五入
  zz = (int16_t)zs;                    //计算整数部分二进制表示
  if(tempH>=0)
  {        
  TH_Val=(zz<<8)+(xz<<4);
    
  }else{
    TH_Val=(abs(zz)<<4)+xz; //求绝对值
    TH_Val=~TH_Val+1;       //求补码
    TH_Val=TH_Val<<4;       //左对齐
  }
  
  TmpI2C_SendData(DevNum, TMP75_ADDR, TH_Reg, TH_Val);  //设置TH值    
} 

/**********************************************************************************************************
**	函 数 名 : Tmp75Value_SetTL()
**	功能说明 : 设置OTP低温限制
**	形    参 :
**	返 回 值 : 
**********************************************************************************************************/
void Tmp75Value_SetTL(I2C_DevType DevNum , uint8_t TmpI2CAddr ,float tempL)
{
  float zs,xs; 
  int16_t zz;          //整数部分
  uint16_t xz;         //小数部分
    
  uint16_t TL_Val;  //TL写入值
  DDL_ASSERT((tempL<=125.0f)&&(tempL>=-40.0f));    //断言判断温度范围是否符合设置范围
  zs=floor(tempL);        //获取整数部分
  xs=fabs(tempL- zs);     //获取小数部分
    
  xz = (uint16_t)(xs/0.0625f+0.5f);    //计算小数部分二进制表示,四舍五入
  zz = (int16_t)zs;                    //计算整数部分二进制表示
  if(tempL>=0)
  {        
  TL_Val=(zz<<8)+(xz<<4);
    
  }else{
    TL_Val=(abs(zz)<<4)+xz; //求绝对值
    TL_Val=~TL_Val+1;       //求补码
    TL_Val=TL_Val<<4;       //左对齐
  }
  
  TmpI2C_SendData(DevNum, TMP75_ADDR, TL_Reg, TL_Val);  //设置TH值    
} 

/**********************************************************************************************************
**	函 数 名 : PowerTmp75Value_Get()
**	功能说明 : 功率板读取温度值
**	形    参 : DevNum - IIC类型 TmpI2CAddr - 芯片地址 CHx - 通道号
**	返 回 值 : 转换后的温度值
**********************************************************************************************************/
float PowerTmp75Value_Get(I2C_DevType DevNum,uint8_t TmpI2CAddr,uint8_t CHx)
{
	float PowerTmpValue;

	Pca9548A_Select(DevNum,(CHx - 1),PCA_9555_ON);	//选择所要读取的通道
	
	PowerTmpValue = Tmp75Value_Get(DevNum,TmpI2CAddr); //TMP75温度读取
	
//	Pca9548A_Select(DevNum,9,PCA_9555_ALLOFF);	//关闭所有通道
	
	return PowerTmpValue;
}

/**********************************************************************************************************
**	函 数 名 : SysTempSamp()
**	功能说明 : 整机设备温度采样函数
**	形    参 : DevNum - IIC类型 TmpI2CAddr - 芯片地址 CHx - 通道号
**	返 回 值 : 无
**********************************************************************************************************/
uint16_t SysTempSamp(uint8_t CHx)
{
	static float MaxTempValue = 0;
	static uint32_t FanPwmValue = 0;
	
	int32_t ls, us;
	Dis_int();
	ls = osKernelLock(); 
	Tier2.sampleCal.Tamb = Tmp75Value_Get(I2C_EE_TEMP,TMP75_ADDR);	//环境温度
	us = osKernelUnlock();
	osKernelRestoreLock(us);
	osKernelRestoreLock(ls);
	En_int();
	bsp_DelayMS(1);
	
	Tier2.sampleCal.Tmod[CHx - 1] = ADCx_Temp[CHx - 1] = PowerTmp75Value_Get(I2C_PCA9548,TMP75_ADDR,CHx);	//模块温度

//	MaxTempValue = ArrDataMax_Get(Tier2.sampleCal.Iout,8);
//	FanPwmValue = 400 - (uint32_t)(MaxTempValue * 2);
//	if(FanPwmValue < 100)
//		FanPwmValue = 100;
//	FanPwmSet(FanPwmValue);	//风扇调速
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


