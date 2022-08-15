/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_pca9555.c
** Author      : Huang Cheng
** Date        : 2022-01-12
** Description : bsp_pca9555 source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_pca9555.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
PcaDataType Pca9555_Data;
uint8_t PCA9548ReadRes[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};	//PCA9548通道选择读取结果
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : PCA9555_IOConfigReg()
**	功能说明 : PCA9555端口输入输出配置
**	形    参 : 0 - 输出; 1 - 输入
**	返 回 值 : PcaStatus - PCA状态，0 - OK，1 - ERR
**********************************************************************************************************/
StatusType PCA9555_IOConfigReg(I2C_DevType DevNum,uint16_t Pca9555Addr,uint8_t *IOArr)
{
	StatusType PcaStatus = _OK;
	
	i2c_Start(DevNum);
	
	i2c_SendByte(DevNum, Pca9555Addr | I2C_WR);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	i2c_SendByte(DevNum, PCA9555_CONFIG0_REG);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* 设置Prot0  0-输出、1-输入 */
	i2c_SendByte(DevNum, IOArr[0]);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* 设置Prot1  0-输出、1-输入 */
	i2c_SendByte(DevNum, IOArr[1]);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	i2c_Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	函 数 名 : PCA9555_ConfigInputReg()
**	功能说明 : PCA9555输入寄存器配置函数
**	形    参 : DevNum - 设备类型
**	返 回 值 : PcaStatus - PCA状态，0 - OK，1 - ERR
**********************************************************************************************************/
StatusType PCA9555_ConfigInputReg(I2C_DevType DevNum,uint16_t Pca9555Addr)
{
	StatusType PcaStatus = _OK;
	
	/* I2C开始通讯 */
	i2c_Start(DevNum);
	
	/* 发送写指令 */
	i2c_SendByte(DevNum, Pca9555Addr | I2C_WR);
	
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus = _ERR);
	}
	
	/* 发送输入寄存器配置指令 */
	i2c_SendByte(DevNum, PCA9555_CONFIG0_REG);
	
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* 设置Prot0为输入端口 */
	i2c_SendByte(DevNum, 0xFF);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* 设置Prot1为输入端口 */
	i2c_SendByte(DevNum, 0xFF);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	i2c_Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	函 数 名 : PCA9555_ConfigOutputReg()
**	功能说明 : PCA9555输出寄存器配置函数
**	形    参 : DevNum - 设备类型
**	返 回 值 : PcaStatus - PCA状态，0 - OK，1 - ERR
**********************************************************************************************************/
StatusType PCA9555_ConfigOutputReg(I2C_DevType DevNum,uint16_t Pca9555Addr)
{
	StatusType PcaStatus = _OK;
	
	i2c_Start(DevNum);
	
	i2c_SendByte(DevNum, Pca9555Addr | I2C_WR);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	i2c_SendByte(DevNum, PCA9555_CONFIG0_REG);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* 设置Prot0为输出端口 */
	i2c_SendByte(DevNum, 0x00);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* 设置Prot1为输出端口 */
	i2c_SendByte(DevNum, 0x00);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	i2c_Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	函 数 名 : Pca9555_RegRead()
**	功能说明 : 读取PCA9555 IO输入寄存器的数据
**	形    参 : DevNum - 设备类型，*RegData - 寄存器数据
**	返 回 值 : PcaStatus - PCA状态，0 - OK，1 - ERR
**********************************************************************************************************/
//StatusType Pca9555_RegRead(I2C_DevType DevNum, uint8_t *RegData)
StatusType Pca9555_RegRead(I2C_DevType DevNum, uint16_t Pca9555Addr,uint8_t *RegData)
{
	StatusType PcaStatus = _OK;
	
	// Require that RegData are not NULL pointers
//	assert(RegData);
	
	i2c_Start(DevNum);
	
	/* 发送设备地址并写入 */
	i2c_SendByte(DevNum, Pca9555Addr | I2C_WR);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* 发送指令类型 */
	i2c_SendByte(DevNum, PCA9555_INPUT0_REG);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus= _ERR);
	}
	
	/* 重启I2C设备 */
	i2c_Start(DevNum);
	
	/* 开始读取设备寄存器数据 */
	i2c_SendByte(DevNum, Pca9555Addr | I2C_RD);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/*读取数据*/
	RegData[0] = i2c_ReadByte(DevNum);
	i2c_Ack(DevNum);
	RegData[1] = i2c_ReadByte(DevNum);
	i2c_NAck(DevNum);
	
	/* 发送I2C总线停止信号 */
	i2c_Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	函 数 名 : Pca9555_RegWrite()
**	功能说明 : 写PCA9555 IO输出寄存器的数据
**	形    参 : DevNum - 设备类型，*RegData - 寄存器数据
**	返 回 值 : PcaStatus - PCA状态，0 - OK，1 - ERR
**********************************************************************************************************/
StatusType Pca9555_RegWrite(I2C_DevType DevNum, uint16_t Pca9555Addr, uint8_t *RegData)
{
	StatusType PcaStatus = _OK;
	
	// Require that RegData are not NULL pointers
//	assert(RegData);
	
	i2c_Start(DevNum);
	
	i2c_SendByte(DevNum, Pca9555Addr|I2C_WR);
	
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	i2c_SendByte(DevNum, PCA9555_OUTPUT0_REG);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* 设置Prot0输出 IO0_0 -> IO0_7 */
	i2c_SendByte(DevNum, RegData[0]);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* 设置Prot1输出 IO1_0 -> IO1_7 */
	i2c_SendByte(DevNum, RegData[1]);
	/* 等待应答，若无应答则停止I2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	i2c_Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	函 数 名 : Pca9548A_Select()
**	功能说明 : 选择PCA955通道
**	形    参 : DevNum - IIC 通道;	CHx - 所选通道
**	返 回 值 : 0 - OK，1 - ERR
**********************************************************************************************************/
StatusType Pca9548A_Select(I2C_DevType DevNum,uint8_t CHx,uint8_t Mode)	//0~7
{
//	int32_t ls, us;
//	Dis_int();
//	ls = osKernelLock(); 
	StatusType Res = _OK;
	
	i2c_Start(DevNum);
	
	i2c_SendByte(DevNum,PCA9548A_DEVICE_ADDR | I2C_WR);
	
	if (i2c_WaitAck(DevNum) != 0)
	{
		goto cmd_fail;	//若无应答则终止IIC
	}

	if(Mode == PCA_9555_ON)	//选择IIC通道
	{
		i2c_SendByte(DevNum,1 << CHx);
	}
	else if(Mode == PCA_9555_ALLOFF)
	{
		i2c_SendByte(DevNum,0x00);
	}
	else if(Mode == PCA_9555_ALLON)
	{
		i2c_SendByte(DevNum,0xFF);
	}
	
	if (i2c_WaitAck(DevNum) != 0)
	{
		goto cmd_fail;
	}
	
	i2c_Stop(DevNum);
	
	if (i2c_WaitAck(DevNum) != 0)
	{
		goto cmd_fail;
	}
	
	i2c_Stop(DevNum);
	
//	us =osKernelUnlock();
//	osKernelRestoreLock(us);
//	osKernelRestoreLock(ls);
//	En_int();        
//	bsp_DelayMS(1);        //必须添加，切换任务重置任务锁
	
	return (Res = _OK);
 
	cmd_fail:
	{
		i2c_Stop(DevNum);
//		us = osKernelUnlock();
//		osKernelRestoreLock(us);
//		osKernelRestoreLock(ls);
//		En_int();        
//		bsp_DelayMS(1);        //必须添加，切换任务重置任务锁
		return (Res = _ERR);
	}
}

/**********************************************************************************************************
**	函 数 名 : Pca9548A_Read_Ch()
**	功能说明 : 9548通道读取
**	形    参 : 
**	返 回 值 : RegData - 接收数据
**********************************************************************************************************/
StatusType Pca9548A_Read_Ch(I2C_DevType DevNum,uint8_t *ReadData)
{
	StatusType Res = _OK;
	
	i2c_Start(DevNum);

	i2c_SendByte(DevNum, PCA9548A_DEVICE_ADDR | I2C_RD);
	
	if (i2c_WaitAck(DevNum) != 0)
	{
		goto cmd_fail;	//若无应答则终止IIC
	}
	
	/* 读取数据 */
	ReadData[0] = i2c_ReadByte(DevNum);
	
	i2c_NAck(DevNum);
	
	i2c_Stop(DevNum);
	
	return (Res = _OK);
	
cmd_fail:
	i2c_Stop(DevNum);
	
	return (Res = _ERR);
}

/**********************************************************************************************************
**	函 数 名 : PCA_9548WRTo_9555()
**	功能说明 : 
**	形    参 : DevNum - IIC类型; CHx - 通道号; Mode - 读写模式
							 WriteData - 写入的数组; State - 写0/写1
**	返 回 值 : _OK - 读/写成功; _ERR - 读/写失败
**********************************************************************************************************/
StatusType PCA_9548WRTo_9555(I2C_DevType DevNum,uint8_t CHx,uint8_t Mode,uint8_t *WriteData,uint8_t State)
{
	uint8_t PCA9548ReadData[1] = {0};
	
//	int32_t ls, us;
//	Dis_int();
//	ls = osKernelLock(); 	
	StatusType Res = _OK;

	for(uint8_t cnt = 0;cnt < 20;cnt++)
	{
		Pca9548A_Select(DevNum,(CHx - 1),PCA_9555_ON);
		bsp_DelayUS(10);
		Pca9548A_Read_Ch(DevNum,PCA9548ReadData);
		
		if( PCA9548ReadData[0] == PCA9548ReadRes[CHx - 1] )	//通道选择读取正确
		{
//			Pca9548A_Select(DevNum,(CHx - 1),PCA_9555_ON);
			break;
		}
		if(cnt > 15)
			return (Res = _ERR);
	}
	
//	if(CHxSelect_Res == 0x00)//通道选择成功
	{
		if(Pca9555_RegRead(DevNum,PCA9555_DEVICE_ADDR,Pca9555_Data.Power_Board[CHx - 1].ReadData) == _ERR)
		{
//			//读取当前所选通道各IO状态失败、返回错误状态
//			return (Res = _ERR);
		}
		
		//数据读取成功
		if(Mode == PCA_WRITE)	//写操作
		{
			if(State == 0x00)	//写0
			{
					WriteData[0] = Pca9555_Data.Power_Board[CHx - 1].ReadData[0] & WriteData[0]; //保持原有数组第一个字符数据保持不变
					WriteData[1] = Pca9555_Data.Power_Board[CHx - 1].ReadData[1] & WriteData[1]; //将读取到的数据与待写入的数据进行运算  当前位置0,其他位保持不变
			}
			else if(State == 0x01) //写1
			{
					WriteData[0] = Pca9555_Data.Power_Board[CHx - 1].ReadData[0] & WriteData[0]; //此处为保持数组原有数据第一个字符不变
					WriteData[1] = Pca9555_Data.Power_Board[CHx - 1].ReadData[1] | WriteData[1]; //当前位置1，其他位保持不变
			}

			Res = Pca9555_RegWrite(DevNum,PCA9555_DEVICE_ADDR,WriteData);	//一次写两个字节
		}
		else if(Mode == PCA_READ)	//读操作
		{
			if(Pca9555_RegRead(DevNum,PCA9555_DEVICE_ADDR,Pca9555_Data.Power_Board[CHx - 1].ReadData) == _ERR)	//读取当前所选通道各IO状态,读两个字节
			{
				//数据读取失败，错误处理
				return (Res = _ERR);
			}
		}
	}
	
	bsp_DelayUS(100);
	
//	us =osKernelUnlock();
//	osKernelRestoreLock(us);
//	osKernelRestoreLock(ls);
//	En_int();        
//	bsp_DelayMS(1);        //必须添加，切换任务重置任务锁
	
	return Res;
}

/**********************************************************************************************************
**	函 数 名 : Pca9555_WRTo_M_Loop()
**	功能说明 : 
**	形    参 : DevNum - IIC类型; CHx - 通道号; State - 写数据
							 WriteData - 写入的数组;
**	返 回 值 : _OK - 读/写成功; _ERR - 读/写失败
**********************************************************************************************************/
StatusType Pca9555_WRTo_M_Loop(I2C_DevType DevNum,uint8_t CHx,uint8_t State,uint8_t *WriteData)
{
	uint8_t PCA9548ReadData[1] = {0};
	
	int32_t ls, us;
	Dis_int();
	ls = osKernelLock(); 	
	StatusType Res = _OK;

	for(uint8_t cnt = 0;cnt < 20;cnt++)
	{
		Pca9548A_Select(DevNum,CHx,PCA_9555_ON);
		bsp_DelayUS(10);
		Pca9548A_Read_Ch(DevNum,PCA9548ReadData);
		
		if( PCA9548ReadData[0] == PCA9548ReadRes[CHx] )	//通道选择读取正确
		{
//			Pca9548A_Select(DevNum,(CHx - 1),PCA_9555_ON);
			break;
		}
		if(cnt > 15)
		{
			us =osKernelUnlock();
			osKernelRestoreLock(us);
			osKernelRestoreLock(ls);
			En_int();        
			bsp_DelayMS(1);        //必须添加，切换任务重置任务锁
			return (Res = _ERR);
		}
	}	
	
	if(Pca9555_RegRead(DevNum,PCA9555_DEVICE_ADDR,Pca9555_Data.Power_Board[CHx - 1].ReadData) == _ERR)	
	{
		//读取当前所选通道各IO状态失败、返回错误状态
//		return (Res = _ERR);
	}
	
	if(State == 0x01)	//写1
	{	
			WriteData[0] =  Pca9555_Data.Power_Board[CHx - 1].ReadData[0] | WriteData[0]; // & 0xFF 保持该八位数据不变
			WriteData[1] =  Pca9555_Data.Power_Board[CHx - 1].ReadData[1] & WriteData[1]; //对选中位置高
	}
	else if(State == 0x00) //写0
	{
		WriteData[0] =  Pca9555_Data.Power_Board[CHx - 1].ReadData[0] & WriteData[0];
		WriteData[1] =  Pca9555_Data.Power_Board[CHx - 1].ReadData[1] & WriteData[1]; //选中位置低
	}
	Res = Pca9555_RegWrite(DevNum,PCA9555_DEVICE_ADDR,WriteData);

	bsp_DelayMS(1);
	
	us =osKernelUnlock();
	osKernelRestoreLock(us);
	osKernelRestoreLock(ls);
	En_int();        
	bsp_DelayMS(1);        //必须添加，切换任务重置任务锁

	return Res;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


