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
#include "bsp_i2c.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
I2C_HandleFuncType *I2C_PcaHandle = &I2C_HandleFunc; //I2C设备操作函数句柄
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**	函 数 名 : PCA9555_ConfigInputReg()
**	功能说明 : PCA9555输入寄存器配置函数
**	形    参 : DevNum - 设备类型
**	返 回 值 : PcaStatus - PCA状态，0 - OK，1 - ERR
**********************************************************************************************************/
Pca9555_StatusType PCA9555_ConfigInputReg(uint8_t DevNum)
{
	Pca9555_StatusType PcaStatus = PCA9555_OK;
	
	/* I2C开始通讯 */
	I2C_PcaHandle->Start(DevNum);
	
	/* 发送写指令 */
	I2C_PcaHandle->SendByte(DevNum, PCA9555_DEVICE_ADDR|I2C_WR);
	
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* 发送输入寄存器配置指令 */
	I2C_PcaHandle->SendByte(DevNum, PCA9555_CONFIG0_REG);
	
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* 设置Prot0为输入端口 */
	I2C_PcaHandle->SendByte(DevNum, 0xFF);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* 设置Prot1为输入端口 */
	I2C_PcaHandle->SendByte(DevNum, 0xFF);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	I2C_PcaHandle->Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	函 数 名 : PCA9555_ConfigOutputReg()
**	功能说明 : PCA9555输出寄存器配置函数
**	形    参 : DevNum - 设备类型
**	返 回 值 : PcaStatus - PCA状态，0 - OK，1 - ERR
**********************************************************************************************************/
Pca9555_StatusType PCA9555_ConfigOutputReg(uint8_t DevNum)
{
	Pca9555_StatusType PcaStatus = PCA9555_OK;
	
	I2C_PcaHandle->Start(DevNum);
	
	I2C_PcaHandle->SendByte(DevNum, PCA9555_DEVICE_ADDR|I2C_WR);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	I2C_PcaHandle->SendByte(DevNum, PCA9555_CONFIG0_REG);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* 设置Prot0为输出端口 */
	I2C_PcaHandle->SendByte(DevNum, 0x00);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* 设置Prot1为输出端口 */
	I2C_PcaHandle->SendByte(DevNum, 0x00);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	I2C_PcaHandle->Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	函 数 名 : Pca9555_RegRead()
**	功能说明 : 读取PCA9555 IO输入寄存器的数据
**	形    参 : DevNum - 设备类型，*RegData - 寄存器数据
**	返 回 值 : PcaStatus - PCA状态，0 - OK，1 - ERR
**********************************************************************************************************/
Pca9555_StatusType Pca9555_RegRead(uint8_t DevNum, uint8_t *RegData)
{
	Pca9555_StatusType PcaStatus = PCA9555_OK;
	
	// Require that RegData are not NULL pointers
	assert(RegData);
	
	I2C_PcaHandle->Start(DevNum);
	
	/* 发送设备地址并写入 */
	I2C_PcaHandle->SendByte(DevNum, PCA9555_DEVICE_ADDR|I2C_WR);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* 发送指令类型 */
	I2C_PcaHandle->SendByte(DevNum, PCA9555_INPUT0_REG);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* 重启I2C设备 */
	I2C_PcaHandle->Start(DevNum);
	
	/* 开始读取设备寄存器数据 */
	I2C_PcaHandle->SendByte(DevNum, PCA9555_DEVICE_ADDR|I2C_RD);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/*读取数据*/
	RegData[0] = I2C_PcaHandle->ReadByte(DevNum);
	I2C_PcaHandle->Ack(DevNum);
	RegData[1] = I2C_PcaHandle->ReadByte(DevNum);
	I2C_PcaHandle->NAck(DevNum);
	
	/* 发送I2C总线停止信号 */
	I2C_PcaHandle->Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	函 数 名 : Pca9555_RegWrite()
**	功能说明 : 写PCA9555 IO输出寄存器的数据
**	形    参 : DevNum - 设备类型，*RegData - 寄存器数据
**	返 回 值 : PcaStatus - PCA状态，0 - OK，1 - ERR
**********************************************************************************************************/
Pca9555_StatusType Pca9555_RegWrite(uint8_t DevNum, uint8_t *RegData)
{
	Pca9555_StatusType PcaStatus = PCA9555_OK;
	
	// Require that RegData are not NULL pointers
	assert(RegData);
	
	I2C_PcaHandle->Start(DevNum);
	
	I2C_PcaHandle->SendByte(DevNum, PCA9555_DEVICE_ADDR|I2C_WR);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	I2C_PcaHandle->SendByte(DevNum, PCA9555_OUTPUT0_REG);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* 设置Prot0输出 */
	I2C_PcaHandle->SendByte(DevNum, RegData[0]);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* 设置Prot1输出 */
	I2C_PcaHandle->SendByte(DevNum, RegData[1]);
	/* 等待应答，若无应答则停止I2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	I2C_PcaHandle->Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


