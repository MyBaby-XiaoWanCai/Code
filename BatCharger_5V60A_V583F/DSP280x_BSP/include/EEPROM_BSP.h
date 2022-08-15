/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: EEPROM_BSP.h
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef DSP2803X_BSP_INCLUDE_EEPROM_BSP_H_
#define DSP2803X_BSP_INCLUDE_EEPROM_BSP_H_

//��������
#define ADDR_24C08              0x50        //24C08������ַ
#define cMaxAddrSize            1023        //

#define NUM_SERVICE_I2C         8           //I2C����ӿ�������
#define LEN_DATA_OBJ_I2C        32          //I2C����ӿ����ݻ���������

#define SERV_EE_COM             0           //������
#define SERV_EE_NET             1           //������
#define SERV_EE_DEBUG           2           //����ģ����
#define SERV_EE_WORKSTEP        3           //��������ģ����
#define SERV_EE_POWERFAIL       4           //���籣��������
#define SERV_EE_DAIG            5           //�������ģ����


#define     cEepromCabinetsNum          0x0000
#define     cEepromModuleAddress        0x0002
#define     cEepromDcBusVoltAdjK        0x0004
#define     cEepromDcBusVoltAdjb        0x0006

#define     cEepromBatteryVoltAdjK      0x0008
#define     cEepromBatteryVoltAdjb      0x000A
#define     cEepromBatSenseVoltAdjK     0x000C
#define     cEepromBatSenseVoltAdjb     0x000E
//--------------------------------------------
#define     cEepromOutPutCurrentHAdjAK  0x0010
#define     cEepromOutPutCurrentHAdjAb  0x0012
#define     cEepromOutPutCurrentHAdjBK  0x0014
#define     cEepromOutPutCurrentHAdjBb  0x0016
#define     cEepromOutPutCurrentLAdjAK  0x0018
#define     cEepromOutPutCurrentLAdjAb  0x001A
#define     cEepromOutPutCurrentLAdjBK  0x001C
#define     cEepromOutPutCurrentLAdjBb  0x001E

//--------------------------------------------
#define     cEepromGateway_IP0          0x0028
#define     cEepromGateway_IP1          0x002A
#define     cEepromGateway_IP2          0x002C
#define     cEepromGateway_IP3          0x002E
#define     cEepromSub_Mask0            0x0030
#define     cEepromSub_Mask1            0x0032
#define     cEepromSub_Mask2            0x0034
#define     cEepromSub_Mask3            0x0036
#define     cEepromPhy_Addr0            0x0038
#define     cEepromPhy_Addr1            0x003A
#define     cEepromPhy_Addr2            0x003C
#define     cEepromPhy_Addr3            0x003E
#define     cEepromPhy_Addr4            0x0040
#define     cEepromPhy_Addr5            0x0042
#define     cEepromIP_Addr0             0x0044
#define     cEepromIP_Addr1             0x0046
#define     cEepromIP_Addr2             0x0048
#define     cEepromIP_Addr3             0x004A
#define     cEepromS0_Port0             0x004C
#define     cEepromS0_Port1             0x004E
#define     cEepromCCIKP                0x0050
#define     cEepromCCIKI                0x0052
#define     cEepromDCIKP                0x0054
#define     cEepromDCIKI                0x0056
#define     cEepromCVKP                 0x0058
#define     cEepromCVKI                 0x005A
#define     cEepromDCVKP                0x005C
#define     cEepromDCVKI                0x005E
#define     cEepromStepComplete         0x0060
#define     cEepromPowerOffStep         0x0062
#define     cEepromPowerOfftime         0x0064
#define     cEepromPowerOffAH           0x0066
//------------------�ŵ�K,Bֵϵ��-------------------
#define    cEepromDischargeAdjLAK         0x0068//A�͵�λK
#define    cEepromDischargeAdjLAB         0x0070//A�͵�λB
#define    cEepromDischargeAdjHAK         0x0072//A�͵�λK
#define    cEepromDischargeAdjHAB         0x0074//A�͵�λB
#define    cEepromDischargeAdjLBK         0x0076//A�͵�λK
#define    cEepromDischargeAdjLBB         0x0078//A�͵�λB
#define    cEepromDischargeAdjHBK         0x0080//A�͵�λK
#define    cEepromDischargeAdjHBB         0x0082//A�͵�λB
//--------------------------------------------------
//20191019 ��������¶Ȳ��������洢��ַ
#define    cEepromT_Cali                0x0084
#define    Default_T_Cali               250
#define    cEepromK_Coeff               0x0086
#define    Default_K_Coeff              -25
//20191019

//20191107
#define    cEepromT_Offset_CC           0x0088
#define    Default_Offset_CC            0
#define    cEepromT_Offset_DCC          0x008A
#define    Default_Offset_DCC           0
//20191107

//20191226
#define    cEepromPowerOffEnergy        0x008C
//20191226

#define    cEepromParallMode        	0x008E	//��������ģʽ
#define    cEepromParallNum        		0x008F	//����������

#define    cEepromCaliVolPT             0x0090  //���ֽ��޷�����

#define    cEepromStepAging             0x0092  //2�ֽ�

#define    cEepromCurLimitedSet         0x0094  //2�ֽ�

#define    cEepromCurDiffAging          0x0096  //2�ֽ�

#define    cEepromVol_T1                0x0098  //2�ֽ�


#define     cBootFlag                   0x0300
#define     cEepromCheckCode            0x03FE


//�ṹ�嶨��
//I2C��д��ַ�����ݶԶ���
typedef struct DATA_OBJ_I2C
{
 Uint16 Data;
 Uint16 Addr;
} DATA_OBJ_I2C;

//I2C�ӿڶ���
typedef struct INTERFACE_I2C
{
                 Uint16  Req_RW;            //client->service��д����Req_RWΪ����ʱ����ʹ��
                 Uint16  Result_RW;         //service->client��д�����Result_RW����ʱ��д���
                 Uint16  Addr_Device;       //��ǰ����ӿڴ���дI2C������ַ,��������1λ
                 Uint16  Num_Bytes;         //��ǰ����ӿڴ���д����,����С�ڵ���LEN_DATA_OBJ_I2C
                 Uint16  Num_Overtime;      //��д��ʱʱ��,100uS/LSB
                 Uint16  Len_Buff;          //��ǰ����ӿ�I2C���ݶ���д�뻺������С
           DATA_OBJ_I2C  Data[LEN_DATA_OBJ_I2C];//��ǰ����ӿ����ݻ�����
   struct INTERFACE_I2C *Ptr_Next;          //ָ����һ��I2C����ӿڣ��γɻ��ζ���
} INTERFACE_I2C;

//client->service������
#define REQ_I2C_FREE            0x00        //0x00-����
#define REQ_I2C_WRITE           0x01        //0x01-����д
#define REQ_I2C_READ            0x02        //0x02-�����

//service->client��д�������
#define I2C_FREE                0x00        //0x00-����
#define I2C_WORKING             0x01        //0x01-���ڶ�д״̬
#define I2C_WRITE_OK            0x02        //0x02-д�ɹ�
#define I2C_READ_OK             0x03        //0x03-���ɹ�
#define I2C_WRITE_ERR           0x12        //0x12-дʧ��
#define I2C_READ_ERR            0x13        //0x13-��ʧ��

//I2C����״̬������
typedef struct STATUS_I2C_SERVICE
{
                 Uint16  Cnt_Bytes;         //��ǰ�����Ѷ�д�������������
                 Uint16  Cnt_Overtime;      //��ʱʱ�������
                 Uint16  Cnt_Error;         //���߳�������ۼƼ��������ڳ���һ��ֵ��λI2Cģ�飬��ȷ��ȡһ�κ�����
                 Uint16  Cnt_Delay;
                 Uint16  State;             //I2C��д״̬��״̬��־
                 Uint16  Serv_Ready;        //�����Ƿ����ʹ�ñ�־��0-�����ԣ�1-����
          INTERFACE_I2C *Ptr;               //��ǰI2C�������������������ָ�룬��ǰ������ɺ�ָ����һ������
} STATUS_I2C_SERVICE;

//���ⲿ�ļ�����ȫ�ֱ�������
extern INTERFACE_I2C        Interface_I2C[NUM_SERVICE_I2C];       //�ж�дEEPROM�����ģ�������Ҫ���Է���1��������д����
extern STATUS_I2C_SERVICE   I2C_Service;

//���ⲿ�ļ����ź�������
extern unsigned char EEPROM_Read_Char(Uint16 Addr);
extern Uint16   EEPROM_Read_Uint16(Uint16 Addr);
extern Uint16   EEPROM_Read_Num(Uint16 Addr,Uint16 Num,unsigned char *Ptr_Data);
extern Uint16   EEPROM_Write_Char(Uint16 Addr,unsigned char Data);
extern Uint16   EEPROM_Write_Uint16(Uint16 Addr,Uint16 Data);
extern Uint16   EEPROM_Write_Num(Uint16 Addr,Uint16 Num,unsigned char *Ptr_Data);

extern Uint16   EEPROM_Write_Uint16_Serv(Uint16 Index,Uint16 Addr,Uint16 Data);
extern void     Read_Para_Net(void);
extern void     Read_Para_Cali(void);
extern void     Read_Para_Cabinet(void);
extern void     InitI2C_BSP(void);
extern void     EEPROM_Service(void);
extern Uint16   Reset_EEPROM_Service(Uint16 Index);
extern Uint16   EEPROM_Read_Start(Uint16 Index);
extern Uint16   Qurey_EEPROM_Read(Uint16 Index);
extern Uint16   Qurey_EEPROM_Write(Uint16 Index);
extern Uint16   EEPROM_Write_Start(Uint16 Index);
extern Uint16   EEPROM_Write_Data(Uint16 Index,Uint16 Addr,Uint16 Data);
extern Uint16   EEPROM_Read_Data(Uint16 Index,Uint16 Addr);
extern Uint16   EEPROM_Read_Data_Num(Uint16 Index,Uint16 Addr,Uint16 Num);
extern Uint16   EEPROM_Write_Data_Num(Uint16 Index,Uint16 Addr,Uint16 Num,unsigned char *Ptr_Data);
extern Uint16   EEPROM_Read_Data_Uint16(Uint16 Index,Uint16 Addr);

extern void Read_Data_PowerFail(void);



#endif /* DSP2803X_BSP_INCLUDE_EEPROM_BSP_H_ */
