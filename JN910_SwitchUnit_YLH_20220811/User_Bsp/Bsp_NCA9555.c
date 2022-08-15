/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Bsp_NCA9555.h
** Author      : YI LI HUA
** Date        : 2022-07-20
** Description : NCA9555驱动文件
**********************************************************************************************************/

/**********************************************************************************************************
输入状态寄存器      1:高电平    0:低电平
输出状态寄存器      1:高电平    0:低电平
状态取反寄存器      1:反转      0:保留
方向配置寄存器      1:输入      0:输出 (带上拉)
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "Bsp_NCA9555.h"

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
**	函 数 名 : PCA9555_IOConfigReg()
**	功能说明 : PCA9555端口输入输出配置
**	形    参 : 0 - 输出; 1 - 输入
**	返 回 值 : PcaStatus - PCA状态，0 - OK，1 - ERR
**********************************************************************************************************/
StatusType PCA9555_IOConfigReg(I2C_DevType DevNum, uint16_t Pca9555Addr,uint8_t *IOArr)
{
    StatusType PcaStatus = _OK;

    i2c_Start(DevNum);

    i2c_SendByte(DevNum, Pca9555Addr | I2C_WR);  //Addr
    /* 等待应答，若无应答则停止I2C */
    if(i2c_WaitAck(DevNum))
    {
        i2c_Stop(DevNum);
        return (PcaStatus=_ERR);
    }

    i2c_SendByte(DevNum, PCA9555_CONFIG0_REG);  //Cmd
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
StatusType Pca9555_RegRead(I2C_DevType DevNum, uint16_t Pca9555Addr,uint8_t *RegData)
{
    StatusType PcaStatus = _OK;

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
**                                          End Of File
**********************************************************************************************************/




