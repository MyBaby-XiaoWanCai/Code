/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : Bsp_NCA9555.h
** Author      : YI LI HUA
** Date        : 2022-07-20
** Description : NCA9555�����ļ�
**********************************************************************************************************/

/**********************************************************************************************************
����״̬�Ĵ���      1:�ߵ�ƽ    0:�͵�ƽ
���״̬�Ĵ���      1:�ߵ�ƽ    0:�͵�ƽ
״̬ȡ���Ĵ���      1:��ת      0:����
�������üĴ���      1:����      0:��� (������)
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "Bsp_NCA9555.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/

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
StatusType PCA9555_IOConfigReg(I2C_DevType DevNum, uint16_t Pca9555Addr,uint8_t *IOArr)
{
    StatusType PcaStatus = _OK;

    i2c_Start(DevNum);

    i2c_SendByte(DevNum, Pca9555Addr | I2C_WR);  //Addr
    /* �ȴ�Ӧ������Ӧ����ֹͣI2C */
    if(i2c_WaitAck(DevNum))
    {
        i2c_Stop(DevNum);
        return (PcaStatus=_ERR);
    }

    i2c_SendByte(DevNum, PCA9555_CONFIG0_REG);  //Cmd
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
StatusType Pca9555_RegRead(I2C_DevType DevNum, uint16_t Pca9555Addr,uint8_t *RegData)
{
    StatusType PcaStatus = _OK;

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
**                                          End Of File
**********************************************************************************************************/




