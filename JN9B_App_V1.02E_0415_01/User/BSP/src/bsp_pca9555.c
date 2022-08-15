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

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
PcaDataType Pca9555_Data;
uint8_t PCA9548ReadRes[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};	//PCA9548ͨ��ѡ���ȡ���
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : PCA9555_IOConfigReg()
**	����˵�� : PCA9555�˿������������
**	��    �� : 0 - ���; 1 - ����
**	�� �� ֵ : PcaStatus - PCA״̬��0 - OK��1 - ERR
**********************************************************************************************************/
StatusType PCA9555_IOConfigReg(I2C_DevType DevNum,uint16_t Pca9555Addr,uint8_t *IOArr)
{
	StatusType PcaStatus = _OK;
	
	i2c_Start(DevNum);
	
	i2c_SendByte(DevNum, Pca9555Addr | I2C_WR);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	i2c_SendByte(DevNum, PCA9555_CONFIG0_REG);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* ����Prot0  0-�����1-���� */
	i2c_SendByte(DevNum, IOArr[0]);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* ����Prot1  0-�����1-���� */
	i2c_SendByte(DevNum, IOArr[1]);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	i2c_Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	�� �� �� : PCA9555_ConfigInputReg()
**	����˵�� : PCA9555����Ĵ������ú���
**	��    �� : DevNum - �豸����
**	�� �� ֵ : PcaStatus - PCA״̬��0 - OK��1 - ERR
**********************************************************************************************************/
StatusType PCA9555_ConfigInputReg(I2C_DevType DevNum,uint16_t Pca9555Addr)
{
	StatusType PcaStatus = _OK;
	
	/* I2C��ʼͨѶ */
	i2c_Start(DevNum);
	
	/* ����дָ�� */
	i2c_SendByte(DevNum, Pca9555Addr | I2C_WR);
	
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus = _ERR);
	}
	
	/* ��������Ĵ�������ָ�� */
	i2c_SendByte(DevNum, PCA9555_CONFIG0_REG);
	
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* ����Prot0Ϊ����˿� */
	i2c_SendByte(DevNum, 0xFF);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* ����Prot1Ϊ����˿� */
	i2c_SendByte(DevNum, 0xFF);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	i2c_Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	�� �� �� : PCA9555_ConfigOutputReg()
**	����˵�� : PCA9555����Ĵ������ú���
**	��    �� : DevNum - �豸����
**	�� �� ֵ : PcaStatus - PCA״̬��0 - OK��1 - ERR
**********************************************************************************************************/
StatusType PCA9555_ConfigOutputReg(I2C_DevType DevNum,uint16_t Pca9555Addr)
{
	StatusType PcaStatus = _OK;
	
	i2c_Start(DevNum);
	
	i2c_SendByte(DevNum, Pca9555Addr | I2C_WR);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	i2c_SendByte(DevNum, PCA9555_CONFIG0_REG);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* ����Prot0Ϊ����˿� */
	i2c_SendByte(DevNum, 0x00);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* ����Prot1Ϊ����˿� */
	i2c_SendByte(DevNum, 0x00);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	i2c_Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	�� �� �� : Pca9555_RegRead()
**	����˵�� : ��ȡPCA9555 IO����Ĵ���������
**	��    �� : DevNum - �豸���ͣ�*RegData - �Ĵ�������
**	�� �� ֵ : PcaStatus - PCA״̬��0 - OK��1 - ERR
**********************************************************************************************************/
//StatusType Pca9555_RegRead(I2C_DevType DevNum, uint8_t *RegData)
StatusType Pca9555_RegRead(I2C_DevType DevNum, uint16_t Pca9555Addr,uint8_t *RegData)
{
	StatusType PcaStatus = _OK;
	
	// Require that RegData are not NULL pointers
//	assert(RegData);
	
	i2c_Start(DevNum);
	
	/* �����豸��ַ��д�� */
	i2c_SendByte(DevNum, Pca9555Addr | I2C_WR);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* ����ָ������ */
	i2c_SendByte(DevNum, PCA9555_INPUT0_REG);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus= _ERR);
	}
	
	/* ����I2C�豸 */
	i2c_Start(DevNum);
	
	/* ��ʼ��ȡ�豸�Ĵ������� */
	i2c_SendByte(DevNum, Pca9555Addr | I2C_RD);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/*��ȡ����*/
	RegData[0] = i2c_ReadByte(DevNum);
	i2c_Ack(DevNum);
	RegData[1] = i2c_ReadByte(DevNum);
	i2c_NAck(DevNum);
	
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	�� �� �� : Pca9555_RegWrite()
**	����˵�� : дPCA9555 IO����Ĵ���������
**	��    �� : DevNum - �豸���ͣ�*RegData - �Ĵ�������
**	�� �� ֵ : PcaStatus - PCA״̬��0 - OK��1 - ERR
**********************************************************************************************************/
StatusType Pca9555_RegWrite(I2C_DevType DevNum, uint16_t Pca9555Addr, uint8_t *RegData)
{
	StatusType PcaStatus = _OK;
	
	// Require that RegData are not NULL pointers
//	assert(RegData);
	
	i2c_Start(DevNum);
	
	i2c_SendByte(DevNum, Pca9555Addr|I2C_WR);
	
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	i2c_SendByte(DevNum, PCA9555_OUTPUT0_REG);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* ����Prot0��� IO0_0 -> IO0_7 */
	i2c_SendByte(DevNum, RegData[0]);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	
	/* ����Prot1��� IO1_0 -> IO1_7 */
	i2c_SendByte(DevNum, RegData[1]);
	/* �ȴ�Ӧ������Ӧ����ֹͣI2C */
	if(i2c_WaitAck(DevNum))
	{
		i2c_Stop(DevNum);
		return (PcaStatus=_ERR);
	}
	i2c_Stop(DevNum);
	
	return PcaStatus;
}

/**********************************************************************************************************
**	�� �� �� : Pca9548A_Select()
**	����˵�� : ѡ��PCA955ͨ��
**	��    �� : DevNum - IIC ͨ��;	CHx - ��ѡͨ��
**	�� �� ֵ : 0 - OK��1 - ERR
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
		goto cmd_fail;	//����Ӧ������ֹIIC
	}

	if(Mode == PCA_9555_ON)	//ѡ��IICͨ��
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
//	bsp_DelayMS(1);        //������ӣ��л���������������
	
	return (Res = _OK);
 
	cmd_fail:
	{
		i2c_Stop(DevNum);
//		us = osKernelUnlock();
//		osKernelRestoreLock(us);
//		osKernelRestoreLock(ls);
//		En_int();        
//		bsp_DelayMS(1);        //������ӣ��л���������������
		return (Res = _ERR);
	}
}

/**********************************************************************************************************
**	�� �� �� : Pca9548A_Read_Ch()
**	����˵�� : 9548ͨ����ȡ
**	��    �� : 
**	�� �� ֵ : RegData - ��������
**********************************************************************************************************/
StatusType Pca9548A_Read_Ch(I2C_DevType DevNum,uint8_t *ReadData)
{
	StatusType Res = _OK;
	
	i2c_Start(DevNum);

	i2c_SendByte(DevNum, PCA9548A_DEVICE_ADDR | I2C_RD);
	
	if (i2c_WaitAck(DevNum) != 0)
	{
		goto cmd_fail;	//����Ӧ������ֹIIC
	}
	
	/* ��ȡ���� */
	ReadData[0] = i2c_ReadByte(DevNum);
	
	i2c_NAck(DevNum);
	
	i2c_Stop(DevNum);
	
	return (Res = _OK);
	
cmd_fail:
	i2c_Stop(DevNum);
	
	return (Res = _ERR);
}

/**********************************************************************************************************
**	�� �� �� : PCA_9548WRTo_9555()
**	����˵�� : 
**	��    �� : DevNum - IIC����; CHx - ͨ����; Mode - ��дģʽ
							 WriteData - д�������; State - д0/д1
**	�� �� ֵ : _OK - ��/д�ɹ�; _ERR - ��/дʧ��
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
		
		if( PCA9548ReadData[0] == PCA9548ReadRes[CHx - 1] )	//ͨ��ѡ���ȡ��ȷ
		{
//			Pca9548A_Select(DevNum,(CHx - 1),PCA_9555_ON);
			break;
		}
		if(cnt > 15)
			return (Res = _ERR);
	}
	
//	if(CHxSelect_Res == 0x00)//ͨ��ѡ��ɹ�
	{
		if(Pca9555_RegRead(DevNum,PCA9555_DEVICE_ADDR,Pca9555_Data.Power_Board[CHx - 1].ReadData) == _ERR)
		{
//			//��ȡ��ǰ��ѡͨ����IO״̬ʧ�ܡ����ش���״̬
//			return (Res = _ERR);
		}
		
		//���ݶ�ȡ�ɹ�
		if(Mode == PCA_WRITE)	//д����
		{
			if(State == 0x00)	//д0
			{
					WriteData[0] = Pca9555_Data.Power_Board[CHx - 1].ReadData[0] & WriteData[0]; //����ԭ�������һ���ַ����ݱ��ֲ���
					WriteData[1] = Pca9555_Data.Power_Board[CHx - 1].ReadData[1] & WriteData[1]; //����ȡ�����������д������ݽ�������  ��ǰλ��0,����λ���ֲ���
			}
			else if(State == 0x01) //д1
			{
					WriteData[0] = Pca9555_Data.Power_Board[CHx - 1].ReadData[0] & WriteData[0]; //�˴�Ϊ��������ԭ�����ݵ�һ���ַ�����
					WriteData[1] = Pca9555_Data.Power_Board[CHx - 1].ReadData[1] | WriteData[1]; //��ǰλ��1������λ���ֲ���
			}

			Res = Pca9555_RegWrite(DevNum,PCA9555_DEVICE_ADDR,WriteData);	//һ��д�����ֽ�
		}
		else if(Mode == PCA_READ)	//������
		{
			if(Pca9555_RegRead(DevNum,PCA9555_DEVICE_ADDR,Pca9555_Data.Power_Board[CHx - 1].ReadData) == _ERR)	//��ȡ��ǰ��ѡͨ����IO״̬,�������ֽ�
			{
				//���ݶ�ȡʧ�ܣ�������
				return (Res = _ERR);
			}
		}
	}
	
	bsp_DelayUS(100);
	
//	us =osKernelUnlock();
//	osKernelRestoreLock(us);
//	osKernelRestoreLock(ls);
//	En_int();        
//	bsp_DelayMS(1);        //������ӣ��л���������������
	
	return Res;
}

/**********************************************************************************************************
**	�� �� �� : Pca9555_WRTo_M_Loop()
**	����˵�� : 
**	��    �� : DevNum - IIC����; CHx - ͨ����; State - д����
							 WriteData - д�������;
**	�� �� ֵ : _OK - ��/д�ɹ�; _ERR - ��/дʧ��
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
		
		if( PCA9548ReadData[0] == PCA9548ReadRes[CHx] )	//ͨ��ѡ���ȡ��ȷ
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
			bsp_DelayMS(1);        //������ӣ��л���������������
			return (Res = _ERR);
		}
	}	
	
	if(Pca9555_RegRead(DevNum,PCA9555_DEVICE_ADDR,Pca9555_Data.Power_Board[CHx - 1].ReadData) == _ERR)	
	{
		//��ȡ��ǰ��ѡͨ����IO״̬ʧ�ܡ����ش���״̬
//		return (Res = _ERR);
	}
	
	if(State == 0x01)	//д1
	{	
			WriteData[0] =  Pca9555_Data.Power_Board[CHx - 1].ReadData[0] | WriteData[0]; // & 0xFF ���ָð�λ���ݲ���
			WriteData[1] =  Pca9555_Data.Power_Board[CHx - 1].ReadData[1] & WriteData[1]; //��ѡ��λ�ø�
	}
	else if(State == 0x00) //д0
	{
		WriteData[0] =  Pca9555_Data.Power_Board[CHx - 1].ReadData[0] & WriteData[0];
		WriteData[1] =  Pca9555_Data.Power_Board[CHx - 1].ReadData[1] & WriteData[1]; //ѡ��λ�õ�
	}
	Res = Pca9555_RegWrite(DevNum,PCA9555_DEVICE_ADDR,WriteData);

	bsp_DelayMS(1);
	
	us =osKernelUnlock();
	osKernelRestoreLock(us);
	osKernelRestoreLock(ls);
	En_int();        
	bsp_DelayMS(1);        //������ӣ��л���������������

	return Res;
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


