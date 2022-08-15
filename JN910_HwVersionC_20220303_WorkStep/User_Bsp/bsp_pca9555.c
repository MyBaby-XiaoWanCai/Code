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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp_pca9555.h"
#include "bsp_i2c.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
I2C_HandleFuncType *I2C_PcaHandle = &I2C_HandleFunc; //I2C�豸�����������
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : PCA9555_ConfigInputReg()
**	����˵�� : PCA9555����Ĵ������ú���
**	��    �� : DevNum - �豸����
**	�� �� ֵ : PcaStatus - PCA״̬��0 - OK��1 - ERR
**********************************************************************************************************/
Pca9555_StatusType PCA9555_ConfigInputReg(uint8_t DevNum)
{
	Pca9555_StatusType PcaStatus = PCA9555_OK;
	
	/* I2C��ʼͨѶ */
	I2C_PcaHandle->Start(DevNum);
	
	/* ����дָ�� */
	I2C_PcaHandle->SendByte(DevNum, PCA9555_DEVICE_ADDR|I2C_WR);
	
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* ��������Ĵ�������ָ�� */
	I2C_PcaHandle->SendByte(DevNum, PCA9555_CONFIG0_REG);
	
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* ����Prot0Ϊ����˿� */
	I2C_PcaHandle->SendByte(DevNum, 0xFF);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* ����Prot1Ϊ����˿� */
	I2C_PcaHandle->SendByte(DevNum, 0xFF);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	I2C_PcaHandle->Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	�� �� �� : PCA9555_ConfigOutputReg()
**	����˵�� : PCA9555����Ĵ������ú���
**	��    �� : DevNum - �豸����
**	�� �� ֵ : PcaStatus - PCA״̬��0 - OK��1 - ERR
**********************************************************************************************************/
Pca9555_StatusType PCA9555_ConfigOutputReg(uint8_t DevNum)
{
	Pca9555_StatusType PcaStatus = PCA9555_OK;
	
	I2C_PcaHandle->Start(DevNum);
	
	I2C_PcaHandle->SendByte(DevNum, PCA9555_DEVICE_ADDR|I2C_WR);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	I2C_PcaHandle->SendByte(DevNum, PCA9555_CONFIG0_REG);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* ����Prot0Ϊ����˿� */
	I2C_PcaHandle->SendByte(DevNum, 0x00);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* ����Prot1Ϊ����˿� */
	I2C_PcaHandle->SendByte(DevNum, 0x00);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	I2C_PcaHandle->Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	�� �� �� : Pca9555_RegRead()
**	����˵�� : ��ȡPCA9555 IO����Ĵ���������
**	��    �� : DevNum - �豸���ͣ�*RegData - �Ĵ�������
**	�� �� ֵ : PcaStatus - PCA״̬��0 - OK��1 - ERR
**********************************************************************************************************/
Pca9555_StatusType Pca9555_RegRead(uint8_t DevNum, uint8_t *RegData)
{
	Pca9555_StatusType PcaStatus = PCA9555_OK;
	
	// Require that RegData are not NULL pointers
	assert(RegData);
	
	I2C_PcaHandle->Start(DevNum);
	
	/* �����豸��ַ��д�� */
	I2C_PcaHandle->SendByte(DevNum, PCA9555_DEVICE_ADDR|I2C_WR);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* ����ָ������ */
	I2C_PcaHandle->SendByte(DevNum, PCA9555_INPUT0_REG);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* ����I2C�豸 */
	I2C_PcaHandle->Start(DevNum);
	
	/* ��ʼ��ȡ�豸�Ĵ������� */
	I2C_PcaHandle->SendByte(DevNum, PCA9555_DEVICE_ADDR|I2C_RD);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/*��ȡ����*/
	RegData[0] = I2C_PcaHandle->ReadByte(DevNum);
	I2C_PcaHandle->Ack(DevNum);
	RegData[1] = I2C_PcaHandle->ReadByte(DevNum);
	I2C_PcaHandle->NAck(DevNum);
	
	/* ����I2C����ֹͣ�ź� */
	I2C_PcaHandle->Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	�� �� �� : Pca9555_RegWrite()
**	����˵�� : дPCA9555 IO����Ĵ���������
**	��    �� : DevNum - �豸���ͣ�*RegData - �Ĵ�������
**	�� �� ֵ : PcaStatus - PCA״̬��0 - OK��1 - ERR
**********************************************************************************************************/
Pca9555_StatusType Pca9555_RegWrite(uint8_t DevNum, uint8_t *RegData)
{
	Pca9555_StatusType PcaStatus = PCA9555_OK;
	
	// Require that RegData are not NULL pointers
	assert(RegData);
	
	I2C_PcaHandle->Start(DevNum);
	
	I2C_PcaHandle->SendByte(DevNum, PCA9555_DEVICE_ADDR|I2C_WR);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	I2C_PcaHandle->SendByte(DevNum, PCA9555_OUTPUT0_REG);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* ����Prot0��� */
	I2C_PcaHandle->SendByte(DevNum, RegData[0]);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(I2C_PcaHandle->WaitAck(DevNum))
	{
		I2C_PcaHandle->Stop(DevNum);
		return (PcaStatus=PCA9555_ERR);
	}
	
	/* ����Prot1��� */
	I2C_PcaHandle->SendByte(DevNum, RegData[1]);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
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


