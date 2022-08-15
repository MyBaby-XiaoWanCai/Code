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
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"


/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
float ADCx_Temp[8] = {0};	

/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/


/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/

/**********************************************************************************************************
**	�� �� �� : TmpI2C_SendData()
**	����˵�� : ������ӻ������������ݣ��������üĴ���
**	��    �� : SlaveAddr���ӻ���ַ����7λ�Ǵӻ���ַ�����λ��д��־0��;����Ϊ0X90
**	�� �� ֵ : ת��״̬��0 - OK��1 - ERR
**********************************************************************************************************/
void TmpI2C_SendData( I2C_DevType DevNum , uint8_t SlaveAddr , uint8_t Reg ,uint16_t date)
{
    SlaveAddr &= 0xFE;//ȷ���ӻ���ַ���λ��0������д��ַ����
    i2c_Start(DevNum);//����I2C����
    i2c_SendByte(DevNum,SlaveAddr);//����оƬ��ַ
    i2c_WaitAck(DevNum);
    i2c_SendByte(DevNum,Reg);      //���ͼĴ�����ַ�����üĴ���
    i2c_WaitAck(DevNum);
    if((Reg==TL_Reg)||(Reg==TH_Reg))
    {
        i2c_SendByte(DevNum,(uint8_t)((date&0xFF00)>>8));  //���üĴ���ֵ��      
        i2c_WaitAck(DevNum);
        i2c_SendByte(DevNum,(uint8_t)(date&0x00FF));  //���üĴ���ֵ.
    }
    else
    {
        i2c_SendByte(DevNum,(uint8_t)(date&0x00FF));  //���üĴ���ֵ��
    }
    i2c_WaitAck(DevNum);
    i2c_Stop(DevNum);           //������ϣ�ֹͣI2C����
}


/**********************************************************************************************************
**	�� �� �� : TmpI2C_ReadData()
**	����˵�� : ����ͨ��I2C������ӻ���ȡ�Ĵ���ֵ
**	��    �� : SlaveAddr���ӻ���ַ����7λ�Ǵӻ���ַ�����λ��д��־0��;����Ϊ0x90
               Temp�� ���ݴ洢ָ��
**	�� �� ֵ : ת��״̬��0 - OK��1 - ERR
**********************************************************************************************************/
void TmpI2C_ReadData(I2C_DevType DevNum , uint8_t SlaveAddr , uint8_t Reg , uint16_t* Temp)
{
    SlaveAddr &= 0xFE;	//ȷ�����λ��0

    i2c_Start(DevNum);//����I2C����

    i2c_SendByte(DevNum,SlaveAddr);//SlaveAddr  ���ʹӻ���ַ
    i2c_WaitAck(DevNum);

    i2c_SendByte(DevNum, Reg);//SubAddr       ָ��Ҫ��ȡ�ļĴ�����ַ
    i2c_WaitAck(DevNum);

    i2c_Start(DevNum);//�����ظ���ʼ����

    SlaveAddr |= 0x01;//��ַ������
    i2c_SendByte(DevNum,SlaveAddr);
    i2c_WaitAck(DevNum);
    if((Reg==TL_Reg)||(Reg==TH_Reg)||(Reg==TMP_Reg))
    {
        *Temp = i2c_ReadByte(DevNum)<<8;//��λ
        i2c_Ack(DevNum);
        *Temp |= i2c_ReadByte(DevNum);//��λ
    }
    else
    {
        *Temp = i2c_ReadByte(DevNum);
    }

    i2c_Ack(DevNum);
    i2c_Stop(DevNum);//������ϣ�ֹͣI2C����
}

/**********************************************************************************************************
**	�� �� �� : bsp_Tmp75_AlertInit()
**	����˵�� : TMP75 IO��ʼ�� ��ʼ��ALERT��
**	��    �� :
**	�� �� ֵ :
**********************************************************************************************************/
void bsp_Tmp75_AlertInit(void)
{
    stc_gpio_init_t stcCVKGpioInit;
    /* ����OTP�ź� pin������:��©���� , ������ ����ʼ��״̬ΪRESET�� */
		stcCVKGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_StructInit(&stcCVKGpioInit);    
    (void)GPIO_Init(GPIO_PORT_AMBIENTTEMP,  GPIO_PIN_AMBIENTTEMP,  &stcCVKGpioInit);
//		GPIO_InMOSCmd(GPIO_PORTE_IDX,Enable);
}

/**********************************************************************************************************
**	�� �� �� : bsp_Tmp75Init()
**	����˵�� : TMP75��ʼ��
**	��    �� :
**	�� �� ֵ : ת������¶�ֵ
**********************************************************************************************************/
void bsp_Tmp75Init(I2C_DevType DevNum , uint8_t TmpI2CAddr)
{
    uint16_t cfgDate = 0x60;       //���üĴ������ݣ�Ϊ12λ�ֱ��ʣ��������������¶�ת�����ر�shutdownģʽ���Ƚ���ģʽ�򿪣��������-��SD=0 ,TM=0 ,POL=0 ,F0=1 ,F1=1
		float TL = 83.0f;   //TL=83��
    float TH = 85.0f;   //TH=85��
	
//	  if(DevNum==I2C_EE_TEMP)	//630��������֤ 2022.04.03
//			cfgDate = 0x60;
//		else
//			cfgDate = 0x64;              //���ʰ�OTP����	
    
    TmpI2C_SendData(DevNum , TmpI2CAddr , CFG_Reg , cfgDate);     //�������üĴ���
		bsp_DelayMS(500);
		Feed_WDG();  
    cfgDate=0;
    TmpI2C_ReadData(DevNum , TmpI2CAddr , CFG_Reg , &cfgDate); 
    
    Tmp75Value_SetTH(DevNum , TmpI2CAddr ,  TH);            //����OTP TH
    bsp_DelayMS(200);
		Feed_WDG();  
    Tmp75Value_SetTL(DevNum , TmpI2CAddr ,  TL);            //����OTP TL   
    bsp_DelayMS(200);
		Feed_WDG();  
}

/**********************************************************************************************************
**	�� �� �� : Tmp75Value_Get()
**	����˵�� : ��ȡ�¶�ֵ
**	��    �� :
**	�� �� ֵ : ת������¶�ֵ
**********************************************************************************************************/
float Tmp75Value_Get(I2C_DevType DevNum , uint8_t TmpI2CAddr)
{
    uint8_t Tmp1 = 0,Tmp2 = 0;
    uint16_t TmpBuf;	//���ڴ�����¶ȼĴ�����ȡ�������ֽ�

    float TmpValue;  //�¶�ֵ
    float symbol;    //+ -����

    TmpI2C_ReadData(DevNum , TmpI2CAddr , TMP_Reg , &TmpBuf);          //��������   0x00Ϊ��ȡ���¶ȼĴ�����ַ

    //������������
    if(((TmpBuf>>15)&0x01)==0)    //���λ=0����Ϊ����
    {
        Tmp1 = TmpBuf>>8;      //��8λ�¶���������
        Tmp2 = (uint8_t)(TmpBuf&0x00FF) >> 4; //��4λ�¶�С������
        symbol= 1.0f;           //����Ϊ+
    }
    else                         //���λΪ1����Ϊ����
    {
        TmpBuf=~TmpBuf+1;        //����
        Tmp1 = TmpBuf>>8;      //��8λ�¶���������
        Tmp2 = (uint8_t)(TmpBuf&0x00FF) >> 4; //��4λ�¶�С������
        symbol= -1.0f;               //����Ϊ-
    }

    TmpValue = symbol*(Tmp1 + Tmp2*0.0625f);//С�����������*����λ

    return TmpValue;
}

/**********************************************************************************************************
**	�� �� �� : Tmp75Value_SetTH()
**	����˵�� : ����OTP��������
**	��    �� :
**	�� �� ֵ : 
**********************************************************************************************************/
void Tmp75Value_SetTH(I2C_DevType DevNum , uint8_t TmpI2CAddr ,float tempH)
{
  float zs,xs; 
  int16_t zz;          //��������
  uint16_t xz;         //С������
    
  uint16_t TH_Val;  //THд��ֵ
  DDL_ASSERT((tempH<=125.0f)&&(tempH>=-40.0f));    //�����ж��¶ȷ�Χ�Ƿ�������÷�Χ
  zs=floor(tempH);        //��ȡ��������
  xs=fabs(tempH- zs);     //��ȡС������
    
  xz = (uint16_t)(xs/0.0625f+0.5f);    //����С�����ֶ����Ʊ�ʾ,��������
  zz = (int16_t)zs;                    //�����������ֶ����Ʊ�ʾ
  if(tempH>=0)
  {        
  TH_Val=(zz<<8)+(xz<<4);
    
  }else{
    TH_Val=(abs(zz)<<4)+xz; //�����ֵ
    TH_Val=~TH_Val+1;       //����
    TH_Val=TH_Val<<4;       //�����
  }
  
  TmpI2C_SendData(DevNum, TMP75_ADDR, TH_Reg, TH_Val);  //����THֵ    
} 

/**********************************************************************************************************
**	�� �� �� : Tmp75Value_SetTL()
**	����˵�� : ����OTP��������
**	��    �� :
**	�� �� ֵ : 
**********************************************************************************************************/
void Tmp75Value_SetTL(I2C_DevType DevNum , uint8_t TmpI2CAddr ,float tempL)
{
  float zs,xs; 
  int16_t zz;          //��������
  uint16_t xz;         //С������
    
  uint16_t TL_Val;  //TLд��ֵ
  DDL_ASSERT((tempL<=125.0f)&&(tempL>=-40.0f));    //�����ж��¶ȷ�Χ�Ƿ�������÷�Χ
  zs=floor(tempL);        //��ȡ��������
  xs=fabs(tempL- zs);     //��ȡС������
    
  xz = (uint16_t)(xs/0.0625f+0.5f);    //����С�����ֶ����Ʊ�ʾ,��������
  zz = (int16_t)zs;                    //�����������ֶ����Ʊ�ʾ
  if(tempL>=0)
  {        
  TL_Val=(zz<<8)+(xz<<4);
    
  }else{
    TL_Val=(abs(zz)<<4)+xz; //�����ֵ
    TL_Val=~TL_Val+1;       //����
    TL_Val=TL_Val<<4;       //�����
  }
  
  TmpI2C_SendData(DevNum, TMP75_ADDR, TL_Reg, TL_Val);  //����THֵ    
} 

/**********************************************************************************************************
**	�� �� �� : PowerTmp75Value_Get()
**	����˵�� : ���ʰ��ȡ�¶�ֵ
**	��    �� : DevNum - IIC���� TmpI2CAddr - оƬ��ַ CHx - ͨ����
**	�� �� ֵ : ת������¶�ֵ
**********************************************************************************************************/
float PowerTmp75Value_Get(I2C_DevType DevNum,uint8_t TmpI2CAddr,uint8_t CHx)
{
	float PowerTmpValue;

	Pca9548A_Select(DevNum,(CHx - 1),PCA_9555_ON);	//ѡ����Ҫ��ȡ��ͨ��
	
	PowerTmpValue = Tmp75Value_Get(DevNum,TmpI2CAddr); //TMP75�¶ȶ�ȡ
	
//	Pca9548A_Select(DevNum,9,PCA_9555_ALLOFF);	//�ر�����ͨ��
	
	return PowerTmpValue;
}

/**********************************************************************************************************
**	�� �� �� : SysTempSamp()
**	����˵�� : �����豸�¶Ȳ�������
**	��    �� : DevNum - IIC���� TmpI2CAddr - оƬ��ַ CHx - ͨ����
**	�� �� ֵ : ��
**********************************************************************************************************/
uint16_t SysTempSamp(uint8_t CHx)
{
	static float MaxTempValue = 0;
	static uint32_t FanPwmValue = 0;
	
	int32_t ls, us;
	Dis_int();
	ls = osKernelLock(); 
	Tier2.sampleCal.Tamb = Tmp75Value_Get(I2C_EE_TEMP,TMP75_ADDR);	//�����¶�
	us = osKernelUnlock();
	osKernelRestoreLock(us);
	osKernelRestoreLock(ls);
	En_int();
	bsp_DelayMS(1);
	
	Tier2.sampleCal.Tmod[CHx - 1] = ADCx_Temp[CHx - 1] = PowerTmp75Value_Get(I2C_PCA9548,TMP75_ADDR,CHx);	//ģ���¶�

//	MaxTempValue = ArrDataMax_Get(Tier2.sampleCal.Iout,8);
//	FanPwmValue = 400 - (uint32_t)(MaxTempValue * 2);
//	if(FanPwmValue < 100)
//		FanPwmValue = 100;
//	FanPwmSet(FanPwmValue);	//���ȵ���
}

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


